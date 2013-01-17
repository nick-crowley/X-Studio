//
// Rich Text.cpp : Functions for parsing, storing and displaying Rich (formatted) text
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       CONSTANTS / GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

CONST UINT   iMessageTextSize       = 8,    // Point size of message text
             iMessageTitleSize      = 8;    // Point size of message titles

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       CREATION / DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createLanguageButtonData
// Description     : Create new language button data, containing button text and ID
// 
// CONST TCHAR*  szText : [in] Button text
// CONST TCHAR*  szID   : [in] Button ID
// 
// Return Value   : New language button data, you are responsible for destroying it
// 
ControlsAPI
LANGUAGE_BUTTON*  createLanguageButtonData(CONST TCHAR*  szText, CONST TCHAR*  szID)
{
   LANGUAGE_BUTTON*  pNewData;   // New button data

   // Create new button data
   pNewData = utilCreateEmptyObject(LANGUAGE_BUTTON);

   // Copy text and ID
   pNewData->szText = utilDuplicateString(szText, lstrlen(szText));
   pNewData->szID = utilDuplicateString(szID, lstrlen(szID));

   // Return
   return pNewData;
}

/// Function name  : createLanguageButtonBitmap
// Description     : Create a custom bitmap with the specified text for display in a rich edit control
// 
// HWND          hRichEditCtrl : [in] Window handle of a RichEdit control
// CONST TCHAR*  szObjectText  : [in] Text for the button
// 
// Return Value  : Handle of the button's bitmap
// 
ControlsAPI
HBITMAP  createLanguageButtonBitmap(HWND  hRichEditCtrl, CONST TCHAR*  szObjectText)
{
   HBITMAP  hCustomButton;    // Output bitmap handle
   HDC      hDC,              // RichEdit control's device context
            hMemoryDC;        // Memory DC for creating bitmap
   RECT     rcButton;         // Drawing rectangle
   
   // Prepare
   hDC = GetDC(hRichEditCtrl);
   hMemoryDC = CreateCompatibleDC(hDC);

   // Load blank button into memory DC
   hCustomButton = LoadBitmap(getResourceInstance(), TEXT("RICHTEXT_BUTTON"));
   SelectObject(hMemoryDC, hCustomButton);

   // Setup DC
   SelectObject(hMemoryDC, GetStockObject(ANSI_VAR_FONT));
   SetBkMode(hMemoryDC, TRANSPARENT);
   SetTextColor(hMemoryDC, RGB(255,255,255));
   
   // Draw button text
   utilSetRectangle(&rcButton, 0, 0, 160, 19);
   DrawText(hMemoryDC, szObjectText, lstrlen(szObjectText), &rcButton, DT_CENTER WITH DT_VCENTER WITH DT_SINGLELINE WITH DT_END_ELLIPSIS);

   // Cleanup MemoryDC and return altered bitmap
   DeleteDC(hMemoryDC);
   return hCustomButton;
}


/// Function name  : deleteLanguageButtonData
// Description     : Delete existing language button data
// 
// LANGUAGE_BUTTON*  pData   : [in] Language button data to delete
//
ControlsAPI
VOID  deleteLanguageButtonData(LANGUAGE_BUTTON*  pData)
{
   // Delete strings
   utilDeleteStrings(pData->szText, pData->szID);
   // Delete calling object
   utilDeleteObject(pData);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                             HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : appendRichEditText
// Description     : Append the text in a RichEdit control with the given string
// 
// HWND          hRichEdit : [in] Window handle to a RichEdit control
// CONST TCHAR*  szText    : [in] String to append
// 
ControlsAPI
VOID  appendRichEditText(HWND  hRichEdit, CONST TCHAR*  szText)
{
   SendMessage(hRichEdit, EM_SETSEL, 32768, 32768);
   SendMessage(hRichEdit, EM_REPLACESEL, FALSE, (LPARAM)szText);
}


/// Function name  : calculateVisibleRichTextColour
// Description     : Alter a text colour to ensure it's visible, based on the background colour
// 
// CONST GAME_TEXT_COLOUR  eColour     : [in] Text colour
// CONST GAME_TEXT_COLOUR  eBackground : [in] Background colour, must be GTC_BLUE, GTC_BLACK or GTC_WHITE
// 
// Return Value   : RGB Colour to use instead
// 
ControlsAPI
COLORREF  calculateVisibleRichTextColour(CONST GAME_TEXT_COLOUR  eColour, CONST GAME_TEXT_COLOUR  eBackground)
{
   COLORREF  clOutput;

   switch (eBackground)
   {
   // [SELECTED BACKGROUND] -- Always draw in system highlight colour
   case GTC_BLUE:
      clOutput = GetSysColor(COLOR_HIGHLIGHTTEXT);
      break;

   // [DARK BACKGROUND] -- Don't use black text
   case GTC_BLACK:
      switch (eColour)
      {
      case GTC_BLACK:  clOutput = clTextColours[GTC_DEFAULT];  break;
      default:         clOutput = clTextColours[eColour];      break;
      }
      break;

   // [LIGHT BACKGROUND] -- Don't use white or light grey text.
   case GTC_WHITE:
      switch (eColour)
      {
      case GTC_WHITE:   
      case GTC_DEFAULT: clOutput = clTextColours[GTC_BLACK];  break;
      default:          clOutput = clTextColours[eColour];    break;
      }
      break;
   }

   return clOutput;
}


/// Function name  : calculatePlainTextGenerateStateFromAttributes
// Description     : Convert a CHARFORMAT and PARAFORMAT object into a current PlainTextGenerator state
// 
// CHARFORMAT*            pCharacterFormat : [in]  Character attributes
// PARAFORMAT*            pParagraphFormat : [in]  Paragraph attributes
// RICHTEXT_ATTRIBUTES*  pState           : [out] PlainTextGenerator formatting attributes
// 
VOID  calculatePlainTextGenerateStateFromAttributes(CHARFORMAT*  pCharacterFormat, PARAFORMAT*  pParagraphFormat, RICHTEXT_ATTRIBUTES*  pState)
{
   pState->bBold      = (pCharacterFormat->dwEffects INCLUDES CFE_BOLD      ? TRUE : FALSE);
   pState->bItalic    = (pCharacterFormat->dwEffects INCLUDES CFE_ITALIC    ? TRUE : FALSE);
   pState->bUnderline = (pCharacterFormat->dwEffects INCLUDES CFE_UNDERLINE ? TRUE : FALSE);
   pState->eAlignment = (PARAGRAPH_ALIGNMENT)pParagraphFormat->wAlignment;
   pState->eColour    = identifyGameTextColourFromRGB(pCharacterFormat->crTextColor);
}


/// Function name  : comparePlainTextGenerationState
// Description     : Compare the attributes of a plain text generator state and the character/paragraph attributes
//                    of a RichEdit control's selection.
// 
// CHARFORMAT*            pCharacterFormat : [in] Character attributes
// PARAFORMAT*            pParagraphFormat : [in] Paragraph attributes
// RICHTEXT_ATTRIBUTES*  pState           : [in] Current plain text generator state
// 
// Return Value   : Whether they are equal or have changed
// 
RICHTEXT_FORMATTING comparePlainTextGenerationState(CHARFORMAT*  pCharacterFormat, PARAFORMAT*  pParagraphFormat, RICHTEXT_ATTRIBUTES*  pState)
{
   RICHTEXT_FORMATTING   eOutput;      // Comparison result
   RICHTEXT_ATTRIBUTES  oNewState;    // Input character/paragraph attributes condensed into a plain text generator state

   // Prepare
   eOutput = RTF_FORMATTING_EQUAL;

   // Create another plain text generation state from the incoming char + para attributes
   calculatePlainTextGenerateStateFromAttributes(pCharacterFormat, pParagraphFormat, &oNewState);

   // Compare alignments
   if (pState->eAlignment != oNewState.eAlignment)
      eOutput = RTF_PARAGRAPH_CHANGED;

   // Compare formatting
   if (pState->bBold      != oNewState.bBold      OR
       pState->bItalic    != oNewState.bItalic    OR
       pState->bUnderline != oNewState.bUnderline OR
       pState->eColour    != oNewState.eColour)
   {
      eOutput = (eOutput == RTF_PARAGRAPH_CHANGED ? RTF_BOTH_CHANGED : RTF_CHARACTER_CHANGED);
   }

   return eOutput;
}


/// Function name  : findLanguageButtonInRichEditByIndex
// Description     : Find the LanguageButton data associated with a specified OLE object in a RichEdit control
// 
// HWND              hRichEdit  : [in]  Window handle of the RichEdit control to search
// CONST UINT        iIndex     : [in]  Zero-based index of the object
// LANGUAGE_BUTTON* &pOutput    : [out] Resultant button data, or NULL if not found
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
ControlsAPI
BOOL  findLanguageButtonInRichEditByIndex(HWND  hRichEdit, CONST UINT  iIndex, LANGUAGE_BUTTON* &pOutput)
{
   IRichEditOle*     pRichEditOLE;    // OLE interface for the Richedit control
   REOBJECT          oObjectData;     // RichEdit OLE object properties
   HRESULT           hResult;         // OLE Operation Result

   // Prepare
   pRichEditOLE = NULL;
   utilZeroObject(&oObjectData, REOBJECT);
   oObjectData.cbStruct = sizeof(REOBJECT);

   // Get OLE Interface
   SendMessage(hRichEdit, EM_GETOLEINTERFACE, NULL, (LPARAM)&pRichEditOLE);

   // Get object properties
   if (pRichEditOLE)
      hResult = pRichEditOLE->GetObject(iIndex, &oObjectData, REO_GETOBJ_NO_INTERFACES);
   
   // Set result
   pOutput = (LANGUAGE_BUTTON*)oObjectData.dwUser;

   // Cleanup and return
   utilReleaseInterface(pRichEditOLE);
   return pOutput != NULL;
}


/// Function name  : identifyGameTextColourFromColourMenuID
// Description     : Convert a colour menu item ID into it's equivilent colour ID
// 
// CONST UINT  iCommandID   : [in] Command ID of the colour menu item
// 
// Return Value   : Game text colour or DEFAULT if unrecognised
// 
ControlsAPI
GAME_TEXT_COLOUR  identifyGameTextColourFromColourMenuID(CONST UINT  iCommandID)
{
   GAME_TEXT_COLOUR  eOutput;
   UINT              iColour;

   // Calculate equivilent value and check it's bounds
   iColour = (iCommandID - IDM_COLOUR_BLACK);
   eOutput = (GAME_TEXT_COLOUR)(iColour >= 0 AND iColour < 10 ? iColour : GTC_DEFAULT);

   return eOutput;
}


/// Function name  : identifyGameTextColourFromRGB
// Description     : Match an COLORREF to a text colour enumeration
// 
// CONST COLORREF  clColour   : [in] RGB colour
// 
// Return Value   : Matching game text colour if found, otherwise GTC_DEFAULT
// 
GAME_TEXT_COLOUR   identifyGameTextColourFromRGB(CONST COLORREF  clColour)
{
   // Iterate through game text colours
   for (UINT iIndex = 0; iIndex < 10; iIndex++)
      if (clColour == clTextColours[iIndex])
         /// [FOUND] Convert index to enum
         return (GAME_TEXT_COLOUR)iIndex;

   /// [NOT FOUND] Return default
   return GTC_DEFAULT;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                            FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : drawRichTextInRichEdit
// Description     : Display a RichText object in a RichEdit control
// 
// HWND                    hRichEdit   : [in] RichEdit window handle
// CONST RICH_TEXT*        pMessage    : [in] RichText message to display
// CONST GAME_TEXT_COLOUR  eBackground : [in] Defines the background colour. Must be GLC_BLACK or GLC_WHITE to
//                                             indicate whether background is light or dark, even if the background
//                                             colour isn't exactly white or black.  The default text colour
//                                             will be chosen to contrast the background. Other colours will not be altered.
// CONST BOOL              bAppendText : [in] Whether to append the existing RichEdit contents or replace them
// 
ControlsAPI
VOID  drawRichTextInRichEdit(HWND  hRichEdit, CONST RICH_TEXT*  pMessage, CONST GAME_TEXT_COLOUR  eBackground, CONST BOOL  bAppendText)
{
   IRichEditOle*     pRichEditOLE;         // RichEdit control OLE interface
   CHARFORMAT        oCharacterFormat;     // Text formatting attributes
   PARAFORMAT        oParagraphFormat;     // Paragraph formatting attributes
   GAME_TEXT_COLOUR  eDefaultColour;       // The 'Default' colour based on the background colour. (either BLACK or White-ish)
   LANGUAGE_BUTTON*  pButtonData;          // Button data
   RICH_PARAGRAPH*   pParagraph;           // Current paragraph
   RICH_ITEM*        pItem;                // Current item
   HBITMAP           hButtonBitmap;        // Button bitmap

   // Check background colour
   ASSERT(eBackground == GTC_BLACK OR eBackground == GTC_WHITE);

   // Prepare
   utilZeroObject(&oCharacterFormat, CHARFORMAT);
   utilZeroObject(&oParagraphFormat, PARAFORMAT);
   oCharacterFormat.cbSize = sizeof(CHARFORMAT);
   oParagraphFormat.cbSize = sizeof(PARAFORMAT);

   // Prepare RichEdit
   if (!bAppendText)
      SetWindowText(hRichEdit, TEXT(""));
   SendMessage(hRichEdit, EM_SETSEL, 32768, 32768);

   // Determine the default colour
   eDefaultColour = (eBackground == GTC_BLACK ? GTC_DEFAULT : GTC_BLACK);
   
   // Set formatting that is consistent for all items
   oCharacterFormat.dwMask  = CFM_BOLD WITH CFM_ITALIC WITH CFM_UNDERLINE WITH CFM_COLOR WITH CFM_SIZE WITH CFM_FACE;
   oCharacterFormat.yHeight = iMessageTextSize * 20;
   StringCchCopy(oCharacterFormat.szFaceName, LF_FACESIZE, TEXT("Arial"));

   /// Iterate through each paragraph in the message
   for (LIST_ITEM*  pParagraphIterator = getListHead(pMessage->pParagraphList); pParagraph = extractListItemPointer(pParagraphIterator, RICH_PARAGRAPH); pParagraphIterator = pParagraphIterator->pNext)
   {
      // Set paragraph alignment
      oParagraphFormat.dwMask     = PFM_ALIGNMENT;
      oParagraphFormat.wAlignment = pParagraph->eAlignment;
      SendMessage(hRichEdit, EM_SETPARAFORMAT, NULL, (LPARAM)&oParagraphFormat);

      /// Iterate through each item in the paragraph
      for (LIST_ITEM*  pItemIterator = getListHead(pParagraph->pItemList); pItem = extractListItemPointer(pItemIterator, RICH_ITEM); pItemIterator = pItemIterator->pNext)
      {
         /// [TEXT] Display text with appropriate formatting
         if (pItem->eType == RIT_TEXT)
         {  
            // Convert item attributes into character formatting
            oCharacterFormat.dwEffects = (pItem->bBold ? CFE_BOLD : NULL) WITH (pItem->bItalic ? CFE_ITALIC : NULL) WITH (pItem->bUnderline ? CFE_UNDERLINE : NULL);
            // Override the 'default' colour (if specified)
            oCharacterFormat.crTextColor = clTextColours[pItem->eColour != GTC_DEFAULT ? pItem->eColour : eDefaultColour ];
            // Set formatting and insert text.
            SendMessage(hRichEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&oCharacterFormat);
            SendMessage(hRichEdit, EM_REPLACESEL, FALSE, (LPARAM)pItem->szText);
         }
         /// [BUTTON] Insert OLE object with appropriate text
         else
         {
            SendMessage(hRichEdit, EM_GETOLEINTERFACE, NULL, (LPARAM)&pRichEditOLE);
            if (pRichEditOLE)
            {
               // Create button bitmap and data
               pButtonData = createLanguageButtonData(pItem->szText, pItem->szID);
               hButtonBitmap = createLanguageButtonBitmap(hRichEdit, pItem->szText);
               // Insert as OLE object
               CLanguageButtonObject::InsertBitmap(pRichEditOLE, hButtonBitmap, pButtonData);
               // Cleanup
               pRichEditOLE->Release();
            }
         }
         
         // Move caret beyond item
         SendMessage(hRichEdit, EM_SETSEL, 32768, 32768);
      }
   }
}


/// Function name  : drawRichTextInSingleLine
// Description     : Draws as much RichText as possible on a single line. Any line breaks are converted into spaces.
// 
// HDC                    hDC          : [in] Device context to draw to
// RECT                   rcDrawRect   : [in] Bounding rectangle of the desired area to draw to
// RICH_TEXT*             pRichText    : [in] RichText message to draw. ** The contents may be altered during drawing **
// CONST GAME_TEXT_COLOUR eBackground  : [in] Background colour, must be GTC_BLACK, GTC_WHITE or GTC_BLUE. 
//
///                                             -> BLACK/WHITE - The default text colour will be altered to contrast this colour. All other colours are unchanged
///                                             -> BLUE        - The item has a 'selected' background therefore 'inverse' text colouring should be used throughout, regardless of text colour.     
// 
ControlsAPI
VOID  drawRichTextInSingleLine(HDC  hDC, RECT  rcDrawRect, RICH_TEXT*  pRichText, CONST GAME_TEXT_COLOUR  eBackground)
{
   GAME_TEXT_COLOUR   eDefaultColour;        // Colour contrasting the background used to represent the 'default' colour
   RICH_PARAGRAPH*    pParagraph;            // Paragraph being drawn
   RICH_ITEM*         pItem;                 // Item being drawn
   COLORREF           clOldColour;           //
   HFONT              hOldFont,              // 
                      hItemFont;             // Font used to draw current item
   SIZE               siTextSize;            // Size of individual phrases (RichText items) as they are output

   // [CHECK] Verify input parameters
   ASSERT(eBackground == GTC_BLACK OR eBackground == GTC_WHITE OR eBackground == GTC_BLUE);

   // Determine the default colour
   eDefaultColour = (eBackground == GTC_WHITE ? GTC_BLACK : GTC_DEFAULT);
   clOldColour    = GetTextColor(hDC);

   /// [TEXT] Iterate through paragraphs (while there is drawing rectangle remaining)
   for (LIST_ITEM*  pParagraphIterator = getListHead(pRichText->pParagraphList); (rcDrawRect.left < rcDrawRect.right) AND (pParagraph = extractListItemPointer(pParagraphIterator, RICH_PARAGRAPH)); pParagraphIterator = pParagraphIterator->pNext)
   {
      // Iterate through items (While there is drawing rectangle remaining)
      for (LIST_ITEM*  pItemIterator = getListHead(pParagraph->pItemList); (rcDrawRect.left < rcDrawRect.right) AND (pItem = extractListItemPointer(pItemIterator, RICH_ITEM)); pItemIterator = pItemIterator->pNext)
      {
         /// [TEXT] Draw only text items, ignore buttons.
         if (pItem->eType == RIT_TEXT)
         {
            // Search item text for 'newline' characters and convert them to spaces
            for (TCHAR*  szChar = utilFindCharacter(pItem->szText, '\n'); szChar; szChar = utilFindCharacter(szChar, '\n'))
               szChar[0] = ' ';

            // Set font and text colour
            hOldFont = SelectFont(hDC, hItemFont = utilDuplicateFont(hDC, pItem->bBold, pItem->bItalic, pItem->bUnderline));
            SetTextColor(hDC, calculateVisibleRichTextColour(pItem->eColour, eBackground));

            // Draw item text
            DrawText(hDC, pItem->szText, lstrlen(pItem->szText), &rcDrawRect, DT_LEFT WITH DT_VCENTER WITH DT_SINGLELINE WITH DT_END_ELLIPSIS WITH DT_NOPREFIX);

            // Shift draw rectangle right
            GetTextExtentPoint32(hDC, pItem->szText, lstrlen(pItem->szText), &siTextSize);
            rcDrawRect.left += siTextSize.cx;

            // Cleanup
            SelectFont(hDC, hOldFont);
            DeleteFont(hItemFont);
         }
      }
   }
   
   // Cleanup 
   SetTextColor(hDC, clOldColour);
}


/// Function name  : drawRichTextItemInRect
// Description     : Draws as much of a RichItem into a rectangle as possible. Text is clipped at a word boundary if the text is too long or a new-line character is encountered
///                                 You must set the appropriate font attributes before calling this function
// 
// HDC           hDC           : [in]     Device Context with attributes already set
// CONST TCHAR*  szText        : [in]     Text to draw
// CONST RECT*   pItemRect     : [in]     Drawing rectangle
// UINT*         piCharsOutput : [in/out] Number of characters drawn
// UINT*         piTextWidth   : [in/out] Total length of the characters drawn
// CONST BOOL    bMeasurement  : [in]     Whether to only perform a measurement
// 
// Return Value   : RTD_ENTIRE  - The entire item was drawn successfully
//                  RTD_PARTIAL - Only a portion of the item
///                                 In both instances piCharsOutput and piTextWidth define the size of the output
// 
RICHTEXT_DRAWING  drawRichTextItemInRect(HDC  hDC, CONST TCHAR*  szText, CONST RECT*  pItemRect, UINT*  piCharsOutput, LONG*  piTextWidth, CONST BOOL  bMeasurement)
{
   RICHTEXT_DRAWING   eResult;             // Whether item was partially or entirely drawn
   TEXTMETRIC         oTextMetrics;        // Used for calculating average character widths
   TCHAR             *szSubString,         // Partial sub-string
                     *szNewLine;           // Position of first new-line, if any
   CONST TCHAR       *szPunctuation,       // Identifies strings that won't fit on a line, but begin with commas or fullstops
                     *szWordBreak;         // Position of last word-break, if any
   SIZE               siItem,              // Size of the item drawing rectangle
                      siSubString;         // Size of the sub-string drawing rectangle
   INT                iTextLength,         // Length of input text, in characters
                      iSubStringLength;    // Length of the sub-string, in characters
   
   // Prepare
   szWordBreak = szPunctuation = NULL;
   iSubStringLength = NULL;

   // Measure text and item rectangle
   iTextLength = lstrlen(szText);
   utilConvertRectangleToSize(pItemRect, &siItem);
   
   // Determine length of sub-string we can fit into the drawing rectangle
   GetTextExtentExPoint(hDC, szText, iTextLength, siItem.cx, &iSubStringLength, NULL, &siSubString);

   // [CHECK] Are the first few characters of a string that won't fit in the rectangle punctuation?
   if (!iSubStringLength AND (szPunctuation = utilFindCharacterNotInSet(szText, ",.")))
   {
      // Examine how many characters of punctuation are at the start of the string
      switch (szPunctuation - szText)
      {
      // [FIRST ONE/TWO] Force punctuation onto this line
      case 1:
      case 2:   iSubStringLength = szPunctuation - szText;    break;
      // [NONE] Ignore
      default:  szPunctuation = NULL;   break;
      }
   }

   // Extract sub-string
   szSubString = utilDuplicateString(szText, iSubStringLength);

   // [CHECK] Does sub-string contain a new-line?
   if (szNewLine = utilFindCharacter(szSubString, '\n'))
   {
      // [NEW-LINE] Null terminate at first new-line
      szNewLine[0] = NULL;

      // Measure text without the new-line, but include new-line in returned character count
      GetTextExtentPoint(hDC, szSubString, iSubStringLength = lstrlen(szSubString), &siSubString);
      iSubStringLength++;

      /// [PARTIAL] Return updated sub-string length and size
      eResult = RTD_PARTIAL;
   }
   // [CHECK] Can we fit entire sub-string onto one line?
   else if (iSubStringLength == iTextLength)
      /// [ENTIRE] Return entire input string
      eResult = RTD_ENTIRE;
   
   else
   {
      // [CHECK] Is this a special case of forcing punctuation onto the end of a line?
      if (szPunctuation)
         // [ORPHAN PUNCTUATION] Measure punctuation
         GetTextExtentPoint(hDC, szSubString, iSubStringLength, &siSubString);

      // [CHECK] Is there a word-break?
      else if (szWordBreak = utilFindCharacterReverse(szSubString, ' '))
      {
         // [WORD-BREAK] Calculate and measure length including the word-break
         iSubStringLength = szWordBreak + 1 - szSubString;
         GetTextExtentPoint(hDC, szSubString, iSubStringLength, &siSubString);
      }
      else
         // [NO WORD-BREAK] Prevent words being broken in half
         iSubStringLength = siSubString.cx = 0;

      /// [PARTIAL] Return updated sub-string length and size
      eResult = RTD_PARTIAL;
   }

   // [CHECK] Ensure we're not performing a calculation
   if (!bMeasurement)
      /// [DRAW] Draw sub-string without trailing new-line/word-break
      DrawText(hDC, szSubString, iSubStringLength, (RECT*)pItemRect, MAKEWORD(DT_LEFT WITH DT_EXPANDTABS WITH DT_TABSTOP, 4));     // HACK: Remove 'const' - DrawText(..) does not modify rect without DT_CALCRECT
   
   // Set sub-string length and size
   *piCharsOutput = iSubStringLength;
   *piTextWidth   = siSubString.cx;

   // [TABS] Manually calculate the total width of all tab-chars in the string  (DrawText inserts 4 x AverageCharWidth per tab)
   for (CONST TCHAR*  szIterator = utilFindCharacter(szText, '\t'); szIterator; szIterator = utilFindCharacter(&szIterator[1], '\t'))
   {
      // Retrieve average char size and multiply by 4
      GetTextMetrics(hDC, &oTextMetrics);
      *piTextWidth += (4 * oTextMetrics.tmAveCharWidth);
   }

   // Cleanup and return result
   utilDeleteString(szSubString);
   return eResult;
}



/// Function name  : drawRichImageItemInRect
// Description     : Draws as much of a RichItem into a rectangle as possible. Text is clipped at a word boundary if the text is too long or a new-line character is encountered
///                                 You must set the appropriate font attributes before calling this function
// 
// HDC           hDC           : [in]     Device Context with attributes already set
// CONST TCHAR*  szText        : [in]     Text to draw
// CONST RECT*   pItemRect     : [in]     Drawing rectangle
// SIZE*         piImage       : [in/out] Size of the image drawn
// CONST BOOL    bMeasurement  : [in]     Whether to only perform a measurement
// 
// Return Value   : RTD_ENTIRE  - The entire image was drawn successfully
//                  RTD_PARTIAL - The image was not drawn
///                                 In both instances piCharsOutput and piTextWidth define the size of the output
// 
RICHTEXT_DRAWING  drawRichImageItemInRect(HDC  hDC, CONST TCHAR*  szID, CONST UINT  iImageType, CONST RECT*  pItemRect, SIZE*  piImage, CONST BOOL  bMeasurement)
{
   HBITMAP    hBitmap,        // Drawing bitmap
              hOldBitmap;     //
   BITMAP     oBitmap;        // Bitmap properties
   SIZE       siItem;         // Size of the drawing rectangle
   HDC        hMemoryDC;      // Memory DC

   // Measure item rectangle
   utilConvertRectangleToSize(pItemRect, &siItem);

   // Load Image
   if (hBitmap = (HBITMAP)LoadImage(getResourceInstance(), szID, IMAGE_BITMAP, 0, 0, NULL))
   {
      /// [SUCCESS] Measure Image and return dimensions
      GetObject(hBitmap, sizeof(BITMAP), &oBitmap);
      piImage->cx = oBitmap.bmWidth;
      piImage->cy = oBitmap.bmHeight;

      // [CHECK] Can we fit the item in the available drawing rectangle?
      if (piImage->cx <= siItem.cx)
      {
         // [CHECK] Are we performing drawing?
         if (!bMeasurement)
         {
            // [SUCCESS] Load bitmap into memory DC
            hMemoryDC  = CreateCompatibleDC(hDC);
            hOldBitmap = SelectBitmap(hMemoryDC, hBitmap);

            /// [DRAW] Draw Image 
            BitBlt(hDC, pItemRect->left, pItemRect->top, piImage->cx, piImage->cy, hMemoryDC, 0, 0, SRCCOPY);

            // Cleanup
            SelectBitmap(hMemoryDC, hOldBitmap);
            DeleteDC(hMemoryDC);
         }
      }

      /// Cleanup and return ENTIRE if drawn, otherwise PARTIAL
      DeleteBitmap(hBitmap);
      return (piImage->cx <= siItem.cx ? RTD_ENTIRE : RTD_PARTIAL);
   }
   else if (bMeasurement)
      DrawText(hDC, TEXT("Image_Missing"), 13, (RECT*)pItemRect, DT_CENTER);
   
   /// [FAILED] Return ENTIRE
   return RTD_ENTIRE;
}


/// Function name  : drawRichTextItemsInLine
// Description     : Draws or measures as much RichText as possible into the specified line
// 
// HDC                       hDC             : [in] 
// CONST RICHTEXT_POSITION*  pStartPos       : [in]               Item and character at which drawing should begin
// CONST RECT*               pLineRect       : [in]               Drawing rectangle
// RICHTEXT_POSITION*        pEndPos         : [in/out]           Item and character at which drawing ended
// UINT*                     pRemainingWidth : [in/out][optional] Unused width of the drawing rectangle
// CONST BOOL                bMeasurement    : [in]               Whether to only perform a measurement
// 
VOID  drawRichTextItemsInLine(HDC  hDC, CONST RICHTEXT_POSITION*  pStartPos, CONST RECT*  pLineRect, RICHTEXT_POSITION*  pEndPos, INT*  piRemainingWidth, INT*  piLineHeight, CONST BOOL  bMeasurement)
{
   RICHTEXT_DRAWING  eDrawResult;        // Drawing result
   CONST TCHAR      *szLineText,         // Convenience pointer for the text at the start of the line
                    *szFirstCharacters;  // Position within the line text containing the first real (non-punctuation) characters
   DC_STATE*         pDrawState;         // Drawing state
   RICH_ITEM*        pItem;              // Current item
   HFONT             hItemFont;          // Item font
   RECT              rcItem;             // Current item rectangle
   SIZE              siItem;             // Size of the item currently being drawn, in pixels
   UINT              iItemLength,        // Length of the item currently being drawn, in characters
                     iTotalWidth;        // Running total of the width of items drawn so far, in pixels
   // Prepare
   iTotalWidth = 0;

   // Set initial drawing rectangle and set the 'end' position to the 'start'
   *pEndPos = *pStartPos;
   rcItem   = *pLineRect;

   // [CHECK] Remove leading punctuation from the item
   if (pItem = extractListItemPointer(pEndPos->pCurrentItem, RICH_ITEM))
   {
      // Prepare
      szLineText = &pItem->szText[pStartPos->iCharIndex];
   
      // [LEADING SPACE/PUNCTUATION] Find the first non-punctuation characters 
      if (szFirstCharacters = utilFindCharacterNotInSet(szLineText, " ,."))
         // [FOUND] Consume them
         pEndPos->iCharIndex += (szFirstCharacters - szLineText);
   }

   /// Iterate through remaining items
   while (pItem = extractListItemPointer(pEndPos->pCurrentItem, RICH_ITEM))
   {
      // Examine item type
      switch (pItem->eType)
      {
      case RIT_TEXT:
         // [FONT] Set item font and colour
         pDrawState = utilCreateDeviceContextState(hDC);
         utilSetDeviceContextFont(pDrawState, hItemFont = utilDuplicateFont(hDC, pItem->bBold, pItem->bItalic, pItem->bUnderline), getGameTextColour(pItem->eColour != GTC_DEFAULT ? pItem->eColour : GTC_BLACK ));

         /// Attempt to draw item in remaining rectangle.  Receive item length and width.
         eDrawResult = drawRichTextItemInRect(hDC, &pItem->szText[pEndPos->iCharIndex], &rcItem, &iItemLength, &siItem.cx, bMeasurement);

         // Update item drawing rectangle and running total
         rcItem.left += siItem.cx;
         iTotalWidth += siItem.cx;

         // Reset attributes
         utilDeleteDeviceContextState(pDrawState);
         DeleteFont(hItemFont);
         break;

      case RIT_BUTTON:
      case RIT_IMAGE:
         // Prepare
         siItem.cx = 0;
         siItem.cy = (pLineRect->bottom - pLineRect->top);

         /// Attempt to draw item in remaining rectangle.  Receive image height and width.
         if (pItem->eType == RIT_BUTTON)
            eDrawResult = drawRichImageItemInRect(hDC, TEXT("LANGUAGE_BUTTON_HOT"), IMAGE_BITMAP, &rcItem, &siItem, bMeasurement);  
         else
            eDrawResult = drawRichImageItemInRect(hDC, pItem->szID, pItem->iImageType, &rcItem, &siItem, bMeasurement);  

         // Update item drawing rectangle and running total
         rcItem.left += siItem.cx;
         iTotalWidth += siItem.cx;

         /// [LINE HEIGHT] Advise parent this has resulted in a non-standard line height
         if (piLineHeight)
            *piLineHeight = siItem.cy;
         break;
      }

      // Examine drawing result
      if (eDrawResult == RTD_ENTIRE)
      {
         /// [ENTIRE] Move to next item (or NULL)
         pEndPos->pCurrentItem = pEndPos->pCurrentItem->pNext;
         pEndPos->iCharIndex   = 0;
      }
      else
      {
         /// [PARTIAL] Update end position and abort
         pEndPos->iCharIndex += iItemLength;
         break;
      }
   }

   /// [LINE REMAINING] Return length of line rectangle remaining
   if (piRemainingWidth)
      *piRemainingWidth = (pLineRect->right - pLineRect->left) - iTotalWidth - 1;    // HACK: The -1 is required for text alignment to prevent windows perpetually saying the available rectangle is too small
}
     

/// Function name  : drawRichText
// Description     : Draws one or more lines of RichText into the specified rectangle, or calculate the height of the required drawing rectangle
// 
// HDC               hDC          : [in]     Destination device context already set with the desired font
// RECT*             pTargetRect  : [in/out] Target drawing rectangle
///                                                            If performing a measurement, the height on input is ignored, the height on output is the required height of the drawing rectangle
// CONST RICH_TEXT*  pRichText    : [in]     RichText to draw
// CONST UINT        iDrawFlags   : [in]     Can be a combination of the following:
//
///                                             None        - Draws text using default paragraph alignment
///                                             DT_CALCRECT - Calculates the required rectangle height to display text
///                                             DT_VCENTER  - Centres output text vertically.
///                                             DT_RIGHT    - Aligns all text right, ignores paragraph alignment
///                                             DT_CENTER   - Aligns all text centrally, ignored paragraph alignment
// 
ControlsAPI 
VOID  drawRichText(HDC  hDC, CONST RICH_TEXT*  pRichText, RECT*  pTargetRect, CONST UINT  iDrawFlags)
{
   CONST RICH_PARAGRAPH*  pParagraph;        // Current paragraph
   PARAGRAPH_ALIGNMENT    eAlignment;
   RICHTEXT_POSITION      oPosition,         // Marks the beginning of a line 
                          oEndPosition;      // Marks the end of a line
   INT                    iLineRemaining,    // Length of the current line remaining after drawing
                          iLastLineHeight;
   SIZE                   siLineSize;        // Defines the height of a line of text using the current font
   RECT                   rcLineRect,        // Current line rectangle, or portion of a line
                          rcOutputRect;      // Drawing output rectangle
   
   // Measure the line height and width  (Based on height of the current font)
   GetTextExtentPoint32(hDC, TEXT("AZTjpqy"), 7, &siLineSize);   
   siLineSize.cx = (pTargetRect->right - pTargetRect->left);
   
   /// Calculate initial line rectangle
   utilSetRectangle(&rcLineRect, pTargetRect->left, pTargetRect->top, siLineSize.cx, siLineSize.cy);

   /// Iterate through paragraphs
   for (LIST_ITEM*  pParagraphIterator = getListHead(pRichText->pParagraphList); pParagraph = extractListItemPointer(pParagraphIterator, RICH_PARAGRAPH); pParagraphIterator = pParagraphIterator->pNext)
   {
      // [SUBSEQUENT PARAGRAPH] Add a small gap between paragraphs
      /*if (pParagraphIterator->pPrev)
         OffsetRect(&rcLineRect, 0, utilCalculatePercentage(siLineSize.cy, 50));*/

      // Override paragraph alignment if specified in the draw flags
      switch (iDrawFlags INCLUDES (DT_CENTER WITH DT_RIGHT))
      {
      case DT_CENTER:  eAlignment = PA_CENTRE;               break;
      case DT_RIGHT:   eAlignment = PA_RIGHT;                break;
      default:         eAlignment = pParagraph->eAlignment;  break;
      }

      // [START] Get first item
      oPosition.pCurrentItem = getListHead(pParagraph->pItemList);
      oPosition.iCharIndex   = 0;

      /// Iterate through parargaph items
      while (oPosition.pCurrentItem)
      {
         // Prepare
         rcOutputRect = rcLineRect;
         iLastLineHeight = siLineSize.cy;

         /// [MEASURE] Measure how many items can be fit into the current line
         drawRichTextItemsInLine(hDC, &oPosition, &rcLineRect, &oEndPosition, &iLineRemaining, &iLastLineHeight, TRUE);

         // [CHECK] Only draw if neither 'calculation' or 'vertical centre' are specified
         if (iDrawFlags INCLUDES (DT_CALCRECT WITH DT_VCENTER) ? FALSE : TRUE)
         {
            /// [ALIGNMENT] Align text by offsetting the output rectangle
            switch (eAlignment)
            {
            case PA_RIGHT:   rcOutputRect.left += iLineRemaining;        break;     // Consume the unused portion of the line
            case PA_CENTRE:  rcOutputRect.left += (iLineRemaining / 2);  break;     // Consume half the unused portion of the line
            }

            /// [DRAW] Draw items into the current line
            drawRichTextItemsInLine(hDC, &oPosition, &rcOutputRect, &oEndPosition, NULL, NULL, FALSE);
         }

         // [CHECK] Are there items or paragraphs remaining?
         if (oEndPosition.pCurrentItem OR pParagraphIterator->pNext)
            // [SUCCESS] Move to the next line 
            OffsetRect(&rcLineRect, 0, iLastLineHeight);
            
         // Move to the next item (or NULL)
         oPosition = oEndPosition;
      }
   }

   // [CHECK] Are we centring text vertically?
   if (iDrawFlags INCLUDES DT_VCENTER)
   {
      RECT  rcTextRect;

      // Store the results of the calculation
      SetRect(&rcTextRect, pTargetRect->left, pTargetRect->top, pTargetRect->right, rcLineRect.bottom);

      // [CHECK] Ensure output rectangle is large enough that we can centre text
      if (pTargetRect->bottom > rcTextRect.bottom)
         OffsetRect(&rcTextRect, 0, (pTargetRect->bottom - rcTextRect.bottom) / 2);

      /// [VERTICAL CENTRE] Draw text vertically centred
      drawRichText(hDC, pRichText, &rcTextRect, iDrawFlags INCLUDES ~DT_VCENTER); // Remove V-Centre flag

      // Set the bottom of input rectangle
      pTargetRect->bottom = rcTextRect.bottom;
   }
   else
      /// [DEFAULT] Set the bottom of the input rectangle
      pTargetRect->bottom = rcLineRect.bottom;
}


/// Function name  : updateRichTextFromRichEdit
// Description     : Update a RichText object from the current contents of a RichEdit control
// 
// HWND        hRichEdit  : [in]  Window handle of a RichEdit control, possibly containing new text
// RICH_TEXT*  pMessage   : [out] Existing RichText object. Paragraph and item components will be overwritten.
// 
// Return Value   : TRUE
// 
ControlsAPI
BOOL   updateRichTextFromRichEdit(HWND  hRichEdit, RICH_TEXT*  pMessage)
{
   RICH_PARAGRAPH*       pParagraph;          // Current paragraph being processed
   RICH_ITEM*       pItem;               // Current item of the current paragraph being processed
   RICHTEXT_ATTRIBUTES  oState;              // Current formatting attributes of the character currently being processed
   RICHTEXT_FORMATTING   eComparison;         // Comparison between character attributes
   LANGUAGE_BUTTON*      pButtonData;         // RichEdit language button OLE object data
   IRichEditOle*         pRichEditOLE;        // RichEdit OLE interface
   REOBJECT              oObjectData;         // RichEdit OLE object properties
   CHARFORMAT            oCharacterFormat;    // Character attributes
   PARAFORMAT            oParagraphFormat;    // Paragraph attributes
   UINT                  iPhraseStart, i,     // Character index of the beginning of the phrase (item) currently being processed
                         iButtonIndex,        // Index of the button currently being processed (if any)
                         iTextLength;         // Number of characters in RichEdit
   GETTEXTLENGTHEX       oTextLength;         // Query object for determining number of chars in RichEdit
   
   // Prepare
   utilZeroObject(&oParagraphFormat, PARAFORMAT);
   utilZeroObject(&oCharacterFormat, CHARFORMAT);
   oParagraphFormat.cbSize = sizeof(PARAFORMAT);
   oCharacterFormat.cbSize = sizeof(CHARFORMAT);

   // Prepare
   oTextLength.codepage = 1200;
   oTextLength.flags = GTL_PRECISE WITH GTL_NUMCHARS;
   iTextLength = SendMessage(hRichEdit, EM_GETTEXTLENGTHEX, (WPARAM)&oTextLength, NULL);
   SendMessage(hRichEdit, EM_HIDESELECTION, TRUE, NULL);
   SendMessage(hRichEdit, EM_GETOLEINTERFACE, NULL, (LPARAM)&pRichEditOLE);

   /// Select and query first character
   SendMessage(hRichEdit, EM_SETSEL, 0, 1);
   SendMessage(hRichEdit, EM_GETPARAFORMAT, NULL, (LPARAM)&oParagraphFormat);
   SendMessage(hRichEdit, EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM)&oCharacterFormat);

   calculatePlainTextGenerateStateFromAttributes(&oCharacterFormat, &oParagraphFormat, &oState);

   /// Replace any existing content with empty paragraph using alignment of the first character
   deleteListContents(pMessage->pParagraphList);      // Delete existing paragraphs and items, but not the non-rich edit properties: Title, Author, Columns etc.
   pParagraph = createRichParagraph(oState.eAlignment);
   appendObjectToList(pMessage->pParagraphList, (LPARAM)pParagraph);
   

   if (iTextLength > 0)
   {
      /// Iterate through text character by character
      for (iPhraseStart = 0, iButtonIndex = 0, i = 1; i < iTextLength; i++)
      {
         // Select character
         SendMessage(hRichEdit, EM_SETSEL, i, i + 1);

         /// [OBJECT]
         if (SendMessage(hRichEdit, EM_SELECTIONTYPE, NULL, NULL) == SEL_OBJECT)
         {
            // Save current phrase to the current paragraph. If previous char was also an object there will be no phrase
            if (iPhraseStart < i)
            {
               pItem = createRichItemTextFromEdit(hRichEdit, iPhraseStart, i - iPhraseStart, &oState);
               appendRichTextItemToParagraph(pParagraph, pItem);
            }

            // Get object's properties and extract the associated button data
            oObjectData.cbStruct = sizeof(REOBJECT);
            pRichEditOLE->GetObject(iButtonIndex++, &oObjectData, REO_GETOBJ_NO_INTERFACES);
            pButtonData = (LANGUAGE_BUTTON*)oObjectData.dwUser;
            ASSERT(pButtonData);

            /// Create a new button item using the button data
            pItem = utilCreateEmptyObject(RICH_ITEM);
            pItem->eType = RIT_BUTTON;
            utilReplaceString(pItem->szText, pButtonData->szText);
            utilReplaceString(pItem->szID,   pButtonData->szID);

            /// Append button item to the paragraph. Destroy the button data
            appendRichTextItemToParagraph(pParagraph, pItem);
            deleteLanguageButtonData(pButtonData);

            // Start a new phrase using the NEXT character and it's formatting
            iPhraseStart = i + 1;
            SendMessage(hRichEdit, EM_SETSEL, iPhraseStart, iPhraseStart + 1);
            SendMessage(hRichEdit, EM_GETPARAFORMAT, NULL, (LPARAM)&oParagraphFormat);
            SendMessage(hRichEdit, EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM)&oCharacterFormat);
            calculatePlainTextGenerateStateFromAttributes(&oCharacterFormat, &oParagraphFormat, &oState);
         }
         /// [TEXT]
         else
         {
            // Get formatting + alignment attributes for the character
            SendMessage(hRichEdit, EM_GETPARAFORMAT, NULL, (LPARAM)&oParagraphFormat);
            SendMessage(hRichEdit, EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM)&oCharacterFormat);

            // Determine whether they're the same as the previous character
            eComparison = comparePlainTextGenerationState(&oCharacterFormat, &oParagraphFormat, &oState);

            // If either has changed then save the current phrase as a new item, and possibly start a new paragraph
            if (eComparison != RTF_FORMATTING_EQUAL)
            {
               /// Save current phrase to the current paragraph
               pItem = createRichItemTextFromEdit(hRichEdit, iPhraseStart, i - iPhraseStart, &oState);
               appendRichTextItemToParagraph(pParagraph, pItem);

               /// Start a new phrase from this location (and update the current state)
               iPhraseStart = i;
               calculatePlainTextGenerateStateFromAttributes(&oCharacterFormat, &oParagraphFormat, &oState);

               // Create a new paragraph too, if appropriate.  (and using the new state)
               if (eComparison == RTF_PARAGRAPH_CHANGED OR eComparison == RTF_BOTH_CHANGED)
               {
                  /// Start a new paragraph
                  pParagraph = createRichParagraph(oState.eAlignment);
                  appendObjectToList(pMessage->pParagraphList, (LPARAM)pParagraph);
               }
            }
         }
      }

      /// Save remaining phrase to the current paragraph
      if (i > iPhraseStart)
      {
         pItem = createRichItemTextFromEdit(hRichEdit, iPhraseStart, i - iPhraseStart, &oState);

         // [CHECK] Is final phrase unintelligible formatting chars output by the RichEdit control?
         //  SOLVED: I think this was due to the RichEdit estimating the number of characters it contains

         ASSERT(lstrlen(pItem->szText) > 0);
         appendRichTextItemToParagraph(pParagraph, pItem);
      }
   }

   // Cleanup and Return
   SendMessage(hRichEdit, EM_HIDESELECTION, FALSE, NULL);
   pRichEditOLE->Release();
   return TRUE;
}


