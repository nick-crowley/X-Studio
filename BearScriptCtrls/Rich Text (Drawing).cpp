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

CONST UINT   iMessageTitleSize      = 8;    // Point size of message titles

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       CREATION / DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////////////////


/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                             HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : appendRichEditText
// Description     : Append the text in a RichEdit control with the given string
// 
// HWND          hRichEdit : [in] Window handle to a RichEdit control
// CONST TCHAR*  szText    : [in] String to append
// 
//ControlsAPI
//VOID  appendRichEditText(HWND  hRichEdit, CONST TCHAR*  szText)
//{
//   SendMessage(hRichEdit, EM_SETSEL, 32768, 32768);
//   SendMessage(hRichEdit, EM_REPLACESEL, FALSE, (LPARAM)szText);
//}


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
      case GTC_BLACK:  clOutput = getGameTextColour(GTC_DEFAULT);  break;
      default:         clOutput = getGameTextColour(eColour);      break;
      }
      break;

   // [LIGHT BACKGROUND] -- Don't use white or light grey text.
   case GTC_WHITE:
      switch (eColour)
      {
      case GTC_WHITE:   
      case GTC_DEFAULT: clOutput = getGameTextColour(GTC_BLACK);  break;
      default:          clOutput = getGameTextColour(eColour);    break;
      }
      break;
   }

   return clOutput;
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

/// Function name  : drawLanguageMessageInSingleLine
// Description     : Draws as much RichText as possible on a single line. Any line breaks are converted into spaces.
// 
// HDC                    hDC          : [in] Device context to draw to
// RECT                   rcDrawRect   : [in] Bounding rectangle of the desired area to draw to
// LANGUAGE_MESSAGE*      pMessage     : [in] LanguageMessage to draw. ** The contents may be altered during drawing **
// CONST GAME_TEXT_COLOUR eBackground  : [in] Background colour, must be GTC_BLACK, GTC_WHITE or GTC_BLUE. 
//
///                                             -> BLACK/WHITE - The default text colour will be altered to contrast this colour. All other colours are unchanged
///                                             -> BLUE        - The item has a 'selected' background therefore 'inverse' text colouring should be used throughout, regardless of text colour.     
// 
ControlsAPI
VOID  drawLanguageMessageInSingleLine(HDC  hDC, RECT  rcDrawRect, LANGUAGE_MESSAGE*  pMessage, const BOOL  bDisabled, CONST GAME_TEXT_COLOUR  eBackground)
{
   GAME_TEXT_COLOUR   eDefaultColour;        // Colour contrasting the background used to represent the 'default' colour
   COLORREF           clOldColour;           //
   HFONT              hOldFont,              // 
                      hItemFont;             // Font used to draw current item
   SIZE               siTextSize;            // Size of individual phrases (RichText items) as they are output

   // [CHECK] Verify input parameters
   ASSERT(eBackground == GTC_BLACK OR eBackground == GTC_WHITE OR eBackground == GTC_BLUE);

   // Determine default colour
   eDefaultColour = (eBackground == GTC_WHITE ? GTC_BLACK : GTC_DEFAULT);
   clOldColour    = GetTextColor(hDC);

   // Iterate through author/title
   for (int iProperty = 0; iProperty < 2; iProperty++)
   {
      // Set text
      CONST TCHAR*  szText = (iProperty == 0 ? pMessage->szAuthor : pMessage->szTitle);

      // [CHECK] Do nothing if property is empty
      if (lstrlen(szText))
      {
         // Prepare
         SetTextColor(hDC, bDisabled ? GetSysColor(COLOR_GRAYTEXT) : calculateVisibleRichTextColour(GTC_BLACK, eBackground));
         hItemFont = (iProperty == 0 ? utilDuplicateFont(hDC, TRUE, FALSE, TRUE) : utilDuplicateFont(hDC, TRUE, FALSE, FALSE));
         hOldFont  = SelectFont(hDC, hItemFont);

         // Draw text
         DrawText(hDC, szText, lstrlen(szText), &rcDrawRect, DT_LEFT WITH DT_VCENTER WITH DT_SINGLELINE WITH DT_END_ELLIPSIS WITH DT_NOPREFIX);

         // Shift draw rectangle right  (Also insert small gap)
         GetTextExtentPoint32(hDC, szText, lstrlen(szText), &siTextSize);
         rcDrawRect.left += siTextSize.cx + 4;

         // Cleanup
         SetTextColor(hDC, clOldColour);
         SelectFont(hDC, hOldFont);
         DeleteFont(hItemFont);
      }
   }

   /// [TEXT] Draw remaining text
   drawRichTextInSingleLine(hDC, rcDrawRect, pMessage, bDisabled, eBackground);
}


/// Function name  : drawRichTextInSingleLine
// Description     : Draws as much RichText as possible on a single line. Any line breaks are converted into spaces.
// 
// HDC                    hDC          : [in] Device context to draw to
// RECT                   rcDrawRect   : [in] Bounding rectangle of the desired area to draw to
// RICH_TEXT*             pRichText    : [in] RichText message to draw. ** The contents may be altered during drawing **
// const BOOL             bDisabled    : [in] Whether text should be drawn 'disabled'
// CONST GAME_TEXT_COLOUR eBackground  : [in] Background colour, must be GTC_BLACK, GTC_WHITE or GTC_BLUE. 
//
///                                             -> BLACK/WHITE - The default text colour will be altered to contrast this colour. All other colours are unchanged
///                                             -> BLUE        - The item has a 'selected' background therefore 'inverse' text colouring should be used throughout, regardless of text colour.     
// 
ControlsAPI
VOID  drawRichTextInSingleLine(HDC  hDC, RECT  rcDrawRect, RICH_TEXT*  pRichText, const BOOL  bDisabled, CONST GAME_TEXT_COLOUR  eBackground)
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

   // [DISABLED] Draw without colour
   if (bDisabled)
      SetTextColor(hDC, GetSysColor(COLOR_GRAYTEXT));

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

            // Set font, also colour if enabled
            hOldFont = SelectFont(hDC, hItemFont = utilDuplicateFont(hDC, pItem->bBold, pItem->bItalic, pItem->bUnderline));
            if (!bDisabled)
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
         utilSetDeviceContextFont(pDrawState, hItemFont = utilDuplicateFont(hDC, pItem->bBold, pItem->bItalic, pItem->bUnderline), getTooltipColour(pItem->eColour != GTC_DEFAULT ? pItem->eColour : GTC_BLACK ));

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

