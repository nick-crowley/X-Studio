//
// Custom Rich Edit.cpp : Functions for parsing, storing and displaying Rich text in RichEdits
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                     MACROS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// OnException: Print to console
#define ON_EXCEPTION()     printException(pException)

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       CONSTANTS / GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

CONST UINT   iMessageTextSize  = 10;    // Point size of message text
             
/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          DECLARATIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// Helpers
RICHTEXT_FORMATTING   compareRichTextAttributes(const RICHTEXT_ATTRIBUTES*  pOld, const RICHTEXT_ATTRIBUTES*  pNew);
BOOL                  getRichEditObjectByIndex(HWND  hRichEdit, const UINT  iIndex, REOBJECT*  pObject);
GAME_TEXT_COLOUR      identifyColourFromRGB(CONST COLORREF  clColour);
VOID                  identifyRichTextAttributes(HWND  hRichEdit, const INT  iIndex, RICHTEXT_ATTRIBUTES*  pState);

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                             HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : compareRichTextAttributes
// Description     : Compares values of two attributes objects
// 
// RICHTEXT_ATTRIBUTES*  pOld : [in] Old Attributes
// RICHTEXT_ATTRIBUTES*  pNew : [in] New Attributes
// 
// Return Value   : RTF_FORMATTING_EQUAL, RTF_PARAGRAPH_CHANGED, RTF_CHARACTER_CHANGED or RTF_BOTH_CHANGED
// 
RICHTEXT_FORMATTING   compareRichTextAttributes(const RICHTEXT_ATTRIBUTES*  pOld, const RICHTEXT_ATTRIBUTES*  pNew)
{
   UINT   iResult = RTF_FORMATTING_EQUAL;      // Comparison result

   // Compare alignments
   if (pOld->eAlignment != pNew->eAlignment)
      iResult = RTF_PARAGRAPH_CHANGED;

   // Compare formatting
   if (pOld->bBold != pNew->bBold OR pOld->bItalic != pNew->bItalic OR pOld->bUnderline != pNew->bUnderline OR pOld->eColour != pNew->eColour)
      iResult |= RTF_CHARACTER_CHANGED; 
   
   // Return result
   return (RICHTEXT_FORMATTING)iResult;
}


/// Function name  : getRichEditObjectByIndex
// Description     : Retrieves an OLE object
// 
// HWND        hRichEdit : [in]     Richedit
// const UINT  iIndex    : [in]     Index
// REOBJECT*   pObject   : [in/out] Object data
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BOOL  getRichEditObjectByIndex(HWND  hRichEdit, const UINT  iIndex, REOBJECT*  pObject)
{
   IRichEditOle*  pRichEdit;
   BOOL           bResult;

   // Prepare
   if (!RichEdit_GetOLEInterface(hRichEdit, &pRichEdit))
      return FALSE;

   // Prepare
   utilZeroObject(pObject, REOBJECT);
   pObject->cbStruct = sizeof(REOBJECT);

   // Get object 
   bResult = (pRichEdit->GetObject(iIndex, pObject, REO_GETOBJ_NO_INTERFACES) == S_OK);

   // Return result
   utilReleaseInterface(pRichEdit);
   return bResult;
}



/// Function name  : identifyColourFromRGB
// Description     : Match an COLORREF to a text colour enumeration
// 
// CONST COLORREF  clColour   : [in] RGB colour
// 
// Return Value   : Matching game text colour if found, otherwise GTC_DEFAULT
// 
GAME_TEXT_COLOUR   identifyColourFromRGB(CONST COLORREF  clColour)
{
   /// Return matching colour
   for (UINT iIndex = GTC_BLACK; iIndex <= GTC_YELLOW; iIndex++)
      if (clColour == clTextColours[iIndex])
         return (GAME_TEXT_COLOUR)iIndex;

   /// [NOT FOUND] Return default
   return GTC_DEFAULT;
}


/// Function name  : identifyRichTextAttributes
// Description     : Converts the character/paragraph format of the current selection into an attributes object
// 
// HWND                  hRichEdit : [in]     RichEdit
// const UINT            iIndex    : [in]     Index of Character to examine
// RICHTEXT_ATTRIBUTES*  pState    : [in/out] Attributes
// 
VOID  identifyRichTextAttributes(HWND  hRichEdit, const INT  iIndex, RICHTEXT_ATTRIBUTES*  pState)
{
   CHARFORMAT    oCharacter;    // Character attributes
   PARAFORMAT    oParagraph;    // Paragraph attributes

   // Prepare
   utilZeroObject(&oParagraph, PARAFORMAT);
   utilZeroObject(&oCharacter, CHARFORMAT);
   oParagraph.cbSize = sizeof(PARAFORMAT);
   oCharacter.cbSize = sizeof(CHARFORMAT);

   // Select character
   Edit_SetSel(hRichEdit, iIndex, iIndex + 1);

   // Get attributes
   RichEdit_GetParagraphFormat(hRichEdit, &oParagraph);
   RichEdit_GetCharFormat(hRichEdit, SCF_SELECTION, &oCharacter);

   // Convert attributes
   pState->bBold      = (oCharacter.dwEffects INCLUDES CFE_BOLD      ? TRUE : FALSE);
   pState->bItalic    = (oCharacter.dwEffects INCLUDES CFE_ITALIC    ? TRUE : FALSE);
   pState->bUnderline = (oCharacter.dwEffects INCLUDES CFE_UNDERLINE ? TRUE : FALSE);
   // Convert alignment
   pState->eAlignment = (PARAGRAPH_ALIGNMENT)oParagraph.wAlignment;
   // Convert colour
   pState->eColour    = identifyColourFromRGB(oCharacter.crTextColor);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                             FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : findButtonInRichEditByIndex
// Description     : Find the LanguageButton data associated with a specified OLE object in a RichEdit control
// 
// HWND              hRichEdit  : [in]  Window handle of the RichEdit control to search
// CONST UINT        iIndex     : [in]  Zero-based index of the object
// LANGUAGE_BUTTON* &pOutput    : [out] Resultant button data, or NULL if not found
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
ControlsAPI
BOOL  findButtonInRichEditByIndex(HWND  hRichEdit, CONST UINT  iIndex, LANGUAGE_BUTTON* &pOutput)
{
   REOBJECT  oObject;   // OLE Object

   // Prepare
   pOutput = NULL;

   // Lookup object
   if (getRichEditObjectByIndex(hRichEdit, iIndex, &oObject))
      pOutput = (LANGUAGE_BUTTON*)oObject.dwUser;
   
   // Return TRUE if found
   return pOutput != NULL;
}


/// Function name  : getRichEditText
// Description     : Fill a RichText object from the current contents of a RichEdit control
// 
// HWND        hRichEdit  : [in]     Window handle of a RichEdit control, possibly containing new text
// RICH_TEXT*  pMessage   : [in/out] Existing RichText object. Paragraph and item components will be overwritten.
// 
// Return Value   : TRUE
// 
ControlsAPI
BOOL   getRichEditText(HWND  hRichEdit, RICH_TEXT*  pMessage)
{
   GETTEXTLENGTHEX       oTextLength;          // Text length
   CHARRANGE             oOriginalSel = {0,0}; // Original selection, preserved
   RICH_PARAGRAPH*       pParagraph;           // Current paragraph 
   RICH_ITEM*            pItem;                // Current item 
   RICHTEXT_PHRASE       oPhrase;              // Current phrase
   UINT                  iTextLength,          // Exactly length of RichEdit
                         iButtonIndex = 0,     // Current button index
                         iPos;
   
   TRY
   {
      /// Reset RichText object
      deleteListContents(pMessage->pParagraphList);

      // Get exact text length
      oTextLength.codepage = 1200;
      oTextLength.flags = GTL_PRECISE WITH GTL_NUMCHARS;
      iTextLength = SendMessage(hRichEdit, EM_GETTEXTLENGTHEX, (WPARAM)&oTextLength, NULL);

      // [CHECK] Ensure RichEdit has text
      if (iTextLength == 0)
         return TRUE;

      // Hide current selection
      SetWindowRedraw(hRichEdit, FALSE);
      RichEdit_HideSelection(hRichEdit, TRUE);
      Edit_GetSelEx(hRichEdit, &oOriginalSel.cpMin, &oOriginalSel.cpMax);

      /// Query first character + Create initial paragraph
      identifyRichTextAttributes(hRichEdit, 0, &oPhrase.oState);
      appendRichTextParagraph(pMessage, pParagraph = createRichParagraph(oPhrase.oState.eAlignment));
      
      /// Iterate over each character 
      for (iPos = 0, oPhrase.iStart = 0, oPhrase.iEnd = -1; iPos < iTextLength; iPos++)
      {
         LANGUAGE_BUTTON*     pButtonData;         // Language button data
         RICHTEXT_ATTRIBUTES  oNewState;           // Attributes of current character
         UINT                 iCharType;           // Whether character is text or an OLE object

         // Query character type and attributes
         identifyRichTextAttributes(hRichEdit, iPos, &oNewState);
         iCharType = SendMessage(hRichEdit, EM_SELECTIONTYPE, NULL, NULL);

         // [CHECK] Are text attributes unchanged?
         if (iCharType == SEL_TEXT AND compareRichTextAttributes(&oPhrase.oState, &oNewState) == RTF_FORMATTING_EQUAL)
            continue; 

         /// [CHANGED] Isolate and save current phrase
         if (iPos != oPhrase.iStart)      // [CHECK] If previous character was an object, phrase is empty
         {
            oPhrase.iEnd = iPos;
            appendRichTextItem(pParagraph, pItem = createRichItemTextFromPhrase(hRichEdit, &oPhrase));
         }

         // [ALIGNMENT CHANGED] Create a new paragraph
         if (oPhrase.oState.eAlignment != oNewState.eAlignment)
            appendRichTextParagraph(pMessage, pParagraph = createRichParagraph(oNewState.eAlignment));   
            
         /// [BUTTON] Append new button to the paragraph
         if (iCharType == SEL_OBJECT)
         {
            // Extract data and generate button item
            if (findButtonInRichEditByIndex(hRichEdit, iButtonIndex++, pButtonData))
               appendRichTextItem(pParagraph, pItem = createRichItemButton(pButtonData));

            // Initialise a new phrase using the NEXT character
            oPhrase.iStart = iPos + 1;
            oPhrase.iEnd   = -1;
            identifyRichTextAttributes(hRichEdit, oPhrase.iStart, &oPhrase.oState);
         }
         else
         {
            /// [TEXT] Initialise a new phrase using the current character
            oPhrase.iStart = iPos;
            oPhrase.iEnd   = -1;
            oPhrase.oState = oNewState;
         }
      }

      /// Save final phrase 
      if (iPos != oPhrase.iStart)   // [CHECK] If previous character was an object, phrase is empty
      {
         oPhrase.iEnd = iPos;
         appendRichTextItem(pParagraph, pItem = createRichItemTextFromPhrase(hRichEdit, &oPhrase));
      }
   }
   CATCH0("");

   // Restore original selection 
   Edit_SetSel(hRichEdit, oOriginalSel.cpMin, oOriginalSel.cpMax);
   RichEdit_HideSelection(hRichEdit, FALSE);
   SetWindowRedraw(hRichEdit, TRUE);
   InvalidateRect(hRichEdit, NULL, FALSE);
   return TRUE;
}


/// Function name  : insertRichEditButton
// Description     : Create a new OLE object from a bitmap handle and inserts it into a RichEdit
// 
// IRichEditOle*     pRichEdit         : [in] RichEdit OLE interface
// HBITMAP           hBitmap           : [in] Bitmap handle
// LANGUAGE_BUTTON*  pButtonProperties : [in] Text and ID of the button
//
// Return Value: New Button if succesful, otherwise NULL
//
ControlsAPI
LANGUAGE_BUTTON*   insertRichEditButton(HWND  hRichEdit, CONST TCHAR*  szID, CONST TCHAR*  szText, const GAME_TEXT_COLOUR  eColour)
{
   LANGUAGE_BUTTON*  pButton = NULL;
   IRichEditOle*   pRichEdit = NULL;
   RichEditImage*  pImageObject = NULL;      // Resultant button object
   IOleClientSite* pClientSite = NULL;
   IOleObject*     pObject = NULL; 
   IDataObject*    pDataObject = NULL;
   ILockBytes*     pLockBytes = NULL;
   IStorage*       pStorage = NULL;
   REOBJECT        oRichEditObject;
   CLSID           iCLSID;

   // Prepare
   ZeroMemory(&oRichEditObject, sizeof(REOBJECT));
   oRichEditObject.cbStruct = sizeof(REOBJECT);

	// Get OLE Interface + container site
	if (!RichEdit_GetOLEInterface(hRichEdit, &pRichEdit) OR pRichEdit->GetClientSite(&pClientSite) != S_OK)
      return NULL;

	// Initialize a Storage Object
	if (::CreateILockBytesOnHGlobal(NULL, TRUE, &pLockBytes) == S_OK AND 
       ::StgCreateDocfileOnILockBytes(pLockBytes, STGM_SHARE_EXCLUSIVE WITH STGM_CREATE WITH STGM_READWRITE, 0, &pStorage) == S_OK)
   {
      /// Create LanguageButton
      pButton = createLanguageButton(hRichEdit, szText, szID, eColour);

      /// Create the OLE Picture
      pImageObject = new RichEditImage();
      pImageObject->QueryInterface(IID_IDataObject, (IInterface*)&pDataObject);
      pImageObject->SetBitmap(pButton->hBitmap);

      // Create a static picture OLE Object 
      if (pButton->pObject = pImageObject->Create(pClientSite, pStorage))
      {
         // Notify object it's embedded
         OleSetContainedObject(pButton->pObject, TRUE);

         // Lookup CLSID
         if (pButton->pObject->GetUserClassID(&iCLSID) == S_OK)
         {
            // Define object
            oRichEditObject.clsid    = iCLSID;
            oRichEditObject.cp       = REO_CP_SELECTION;
            oRichEditObject.dvaspect = DVASPECT_CONTENT;
            oRichEditObject.poleobj  = pButton->pObject;
            oRichEditObject.polesite = pClientSite;
            oRichEditObject.pstg     = pStorage;
            // Associate button data
            oRichEditObject.dwUser   = (DWORD)pButton;

            /// Insert the object at the current location in the richedit control
            if (pRichEdit->InsertObject(&oRichEditObject) != S_OK)
               // [FAILED] Cleanup
               deleteLanguageButton(pButton);
         }  
      }  
   }

	// Cleanup + Return
   utilReleaseInterface(pObject);
   utilReleaseInterface(pStorage);
   utilReleaseInterface(pLockBytes);
	utilReleaseInterface(pClientSite);
	utilReleaseInterface(pDataObject);
   utilReleaseInterface(pRichEdit);
   return pButton;
}


/// Function name  : modifyButtonInRichEditByIndex
// Description     : Replaces a button in a RichEdit with another with different text
// 
// HWND                    hRichEdit : [in] RichEdit
// const UINT              iIndex    : [in] Index of button to modify
// const TCHAR*            szNewText : [in] New Text
// const GAME_TEXT_COLOUR  eColour   : [in] New colour
// LANGUAGE_BUTTON*&       pOutput   : [out] New data if succesful, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
ControlsAPI
BOOL  modifyButtonInRichEditByIndex(HWND  hRichEdit, const UINT  iIndex, const TCHAR*  szNewText, const GAME_TEXT_COLOUR  eColour, LANGUAGE_BUTTON*& pOutput)
{
   LANGUAGE_BUTTON*  pOldButton;
   CHARRANGE         oSelection;    // Original selection, preserved
   REOBJECT          oObject;       // RichEdit control OLE object attributes
   TCHAR*            szID;

   // Prepare
   pOutput = NULL;

   /// Lookup object
   if (getRichEditObjectByIndex(hRichEdit, iIndex, &oObject))
   {
      // Preserve old button ID
      pOldButton = (LANGUAGE_BUTTON*)oObject.dwUser;
      szID       = utilDuplicateSimpleString(pOldButton->szID);

      // Preserve selection
      RichEdit_HideSelection(hRichEdit, TRUE);
      Edit_GetSelEx(hRichEdit, &oSelection.cpMin, &oSelection.cpMax);

      /// Delete object
      Edit_SetSel(hRichEdit, oObject.cp, oObject.cp + 1);
      Edit_ReplaceSel(hRichEdit, TEXT(""));

      /// Insert new object
      pOutput = insertRichEditButton(hRichEdit, szID, szNewText, eColour);

      // Cleanup
      Edit_SetSel(hRichEdit, oSelection.cpMin, oSelection.cpMax);
      RichEdit_HideSelection(hRichEdit, FALSE);
      utilDeleteString(szID);
   }
   
   // Return TRUE if found
   return pOutput != NULL;
}


/// Function name  : modifyButtonInRichEditByPosition
// Description     : Changes the colour of a button at a specified position
// 
// HWND                    hRichEdit : [in] RichEdit
// const UINT              iPosition : [in] Character position of button to modify
// const GAME_TEXT_COLOUR  eColour   : [in] New colour
// LANGUAGE_BUTTON*&       pOutput   : [out] New data if succesful, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
ControlsAPI
BOOL  modifyButtonInRichEditByPosition(HWND  hRichEdit, const UINT  iPosition, const GAME_TEXT_COLOUR  eColour, LANGUAGE_BUTTON* &pOutput)
{
   LANGUAGE_BUTTON*  pData;
   REOBJECT          oObject;       // RichEdit control OLE object attributes
   TCHAR*            szButtonText;

   // Prepare
   pOutput = NULL;

   /// Iterate through all objects
   for (UINT  iIndex = 0; getRichEditObjectByIndex(hRichEdit, iIndex, &oObject); iIndex++)
   {
      // Compare position and extract button data
      if (oObject.cp == iPosition AND (pData = (LANGUAGE_BUTTON*)oObject.dwUser))
      {
         // [FOUND] Attempt to modify button
         modifyButtonInRichEditByIndex(hRichEdit, iIndex, szButtonText = utilDuplicateSimpleString(pData->szText), eColour, pOutput);
         utilDeleteString(szButtonText);
         break;
      }
   }

   // Return TRUE if succesful
   return (pOutput != NULL);
}


/// Function name  : removeButtonFromRichEditByIndex
// Description     : Deletes a LanguageButton from a RichEdit control
// 
// HWND        hRichEdit : [in] RichEdit
// const UINT  iIndex    : [in] Index of button to delete
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
ControlsAPI
BOOL  removeButtonFromRichEditByIndex(HWND  hRichEdit, const UINT  iIndex)
{
   CHARRANGE      oSelection;   // Original selection, preserved
   REOBJECT       oObject;      // RichEdit control OLE object attributes

   /// Lookup object
   if (!getRichEditObjectByIndex(hRichEdit, iIndex, &oObject))
      return FALSE;

   // Preserve selection
   RichEdit_HideSelection(hRichEdit, TRUE);
   Edit_GetSelEx(hRichEdit, &oSelection.cpMin, &oSelection.cpMax);

   /// Delete object
   Edit_SetSel(hRichEdit, oObject.cp, oObject.cp + 1);
   Edit_ReplaceSel(hRichEdit, TEXT(""));

   // Restore selection
   Edit_SetSel(hRichEdit, oSelection.cpMin, oSelection.cpMax);
   RichEdit_HideSelection(hRichEdit, FALSE);
   return TRUE;
}


/// Function name  : setRichEditText
// Description     : Replace RichEdit text with the contents of a RichText object
// 
// HWND                    hRichEdit    : [in] RichEdit window handle
// CONST RICH_TEXT*        pMessage     : [in] RichText message to display
// const bool              bSkipButtons : [in] TRUE to ignore buttons, FALSE to insert
// CONST GAME_TEXT_COLOUR  eBackground  : [in] Defines the background colour. Must be GLC_BLACK or GLC_WHITE to
//                                             indicate whether background is light or dark, even if the background
//                                             colour isn't exactly white or black.  The default text colour
//                                             will be chosen to contrast the background. Other colours will not be altered.
// 
ControlsAPI
VOID  setRichEditText(HWND  hRichEdit, CONST RICH_TEXT*  pMessage, const int  iSize, const bool  bSkipButtons, CONST GAME_TEXT_COLOUR  eBackground)
{
   CHARFORMAT        oCharacterFormat;     // Text formatting attributes
   PARAFORMAT        oParagraphFormat;     // Paragraph formatting attributes
   GAME_TEXT_COLOUR  eDefaultColour;       // The 'Default' colour based on the background colour. (either BLACK or White-ish)
   RICH_PARAGRAPH*   pParagraph;           // Current paragraph
   RICH_ITEM*        pItem;                // Current item

   // Check background colour
   ASSERT(eBackground == GTC_BLACK OR eBackground == GTC_WHITE);

   // Prepare
   utilZeroObject(&oCharacterFormat, CHARFORMAT);
   utilZeroObject(&oParagraphFormat, PARAFORMAT);
   oCharacterFormat.cbSize = sizeof(CHARFORMAT);
   oParagraphFormat.cbSize = sizeof(PARAFORMAT);

   // Prepare RichEdit
   //Edit_SetSel(hRichEdit, 32768, 32768);
   SetWindowText(hRichEdit, TEXT(""));

   // Determine the default colour
   eDefaultColour = (eBackground == GTC_BLACK ? GTC_DEFAULT : GTC_BLACK);
   
   // Set formatting that is consistent for all items
   oParagraphFormat.dwMask  = PFM_ALIGNMENT;
   oCharacterFormat.dwMask  = CFM_BOLD WITH CFM_ITALIC WITH CFM_UNDERLINE WITH CFM_COLOR WITH CFM_SIZE WITH CFM_FACE;
   oCharacterFormat.yHeight = iSize * 20;
   StringCchCopy(oCharacterFormat.szFaceName, LF_FACESIZE, TEXT("Arial"));

   /// Iterate through each paragraph in the message
   for (LIST_ITEM*  pParagraphIterator = getListHead(pMessage->pParagraphList); pParagraph = extractListItemPointer(pParagraphIterator, RICH_PARAGRAPH); pParagraphIterator = pParagraphIterator->pNext)
   {
      // Set paragraph alignment
      oParagraphFormat.wAlignment = pParagraph->eAlignment;
      RichEdit_SetParagraphFormat(hRichEdit, &oParagraphFormat);

      /// Iterate through each item in the paragraph
      for (LIST_ITEM*  pItemIterator = getListHead(pParagraph->pItemList); pItem = extractListItemPointer(pItemIterator, RICH_ITEM); pItemIterator = pItemIterator->pNext)
      {
         /// [TEXT] Display text with appropriate formatting
         if (pItem->eType == RIT_TEXT)
         {  
            // Set item attributes + Colour
            oCharacterFormat.dwEffects   = (pItem->bBold ? CFE_BOLD : NULL) WITH (pItem->bItalic ? CFE_ITALIC : NULL) WITH (pItem->bUnderline ? CFE_UNDERLINE : NULL);
            oCharacterFormat.crTextColor = getGameTextColour(pItem->eColour != GTC_DEFAULT ? pItem->eColour : eDefaultColour);

            // Set formatting + text.
            RichEdit_SetCharFormat(hRichEdit, SCF_SELECTION, &oCharacterFormat);
            Edit_ReplaceSel(hRichEdit, pItem->szText);
         }
         /// [BUTTON] Insert OLE object with appropriate text
         else if (!bSkipButtons)
            insertRichEditButton(hRichEdit, pItem->szID, pItem->szText, pItem->eColour);
            
         // Move caret beyond item
         Edit_SetSel(hRichEdit, 65536, 65536);
      }
   }
}


/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          WINDOW PROC
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : wndprocCustomRichEditControl
// Description     : Prevents a RichEdit control in a dialog from having it's text initially selected
// 
//ControlsAPI
LRESULT  wndprocCustomRichEditControl(HWND  hWnd, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   WNDCLASSEX  oClassInfo;

   // Prepare
   GetClassInfoEx(NULL, RICHEDIT_CLASS, &oClassInfo);

   // Examine message
   switch (iMessage)
   {
   // [GET DIALOG CODE] Remove Initially select entire text style
   case WM_GETDLGCODE:
      return CallWindowProc(oClassInfo.lpfnWndProc, hWnd, iMessage, wParam, lParam) ^ DLGC_HASSETSEL;
   }

   // [UNHANDLED] Pass to base
   return CallWindowProc(oClassInfo.lpfnWndProc, hWnd, iMessage, wParam, lParam);
}
