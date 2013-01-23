//
// Custom Rich Edit.cpp : Functions for parsing, storing and displaying Rich text in RichEdits
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       CONSTANTS / GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

CONST UINT   iMessageTextSize       = 10;    // Point size of message text
             
/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       CREATION / DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////////////////


/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                             HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

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
   IRichEditOle*  pRichEdit;     // OLE interface for the Richedit control
   REOBJECT       oObjectData;   // RichEdit OLE object properties

   // Prepare
   utilZeroObject(&oObjectData, REOBJECT);
   oObjectData.cbStruct = sizeof(REOBJECT);
   pOutput = NULL;

   // Get object properties
   if (RichEdit_GetOLEInterface(hRichEdit, &pRichEdit) AND pRichEdit->GetObject(iIndex, &oObjectData, REO_GETOBJ_NO_INTERFACES) == S_OK)
      // [FOUND] Set result
      pOutput = (LANGUAGE_BUTTON*)oObjectData.dwUser;

   // Cleanup and return
   utilReleaseInterface(pRichEdit);
   return pOutput != NULL;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                             FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////


/// Function name  : getRichEditText
// Description     : Fill a RichText object from the current contents of a RichEdit control
// 
// HWND        hRichEdit  : [in]  Window handle of a RichEdit control, possibly containing new text
// RICH_TEXT*  pMessage   : [out] Existing RichText object. Paragraph and item components will be overwritten.
// 
// Return Value   : TRUE
// 
ControlsAPI
BOOL   getRichEditText(HWND  hRichEdit, RICH_TEXT*  pMessage)
{
   RICH_PARAGRAPH*       pParagraph;          // Current paragraph being processed
   RICH_ITEM*            pItem;               // Current item of the current paragraph being processed
   RICHTEXT_ATTRIBUTES   oState;              // Current formatting attributes of the character currently being processed
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
   CHARRANGE             oOriginalSel;        // Original selection, preserved
   
   // Prepare
   utilZeroObject(&oParagraphFormat, PARAFORMAT);
   utilZeroObject(&oCharacterFormat, CHARFORMAT);
   oParagraphFormat.cbSize = sizeof(PARAFORMAT);
   oCharacterFormat.cbSize = sizeof(CHARFORMAT);

   // Prepare
   RichEdit_HideSelection(hRichEdit, TRUE);
   RichEdit_GetOLEInterface(hRichEdit, &pRichEditOLE);
   Edit_GetSelEx(hRichEdit, &oOriginalSel.cpMin, &oOriginalSel.cpMax);

   // Get text length
   oTextLength.codepage = 1200;
   oTextLength.flags = GTL_PRECISE WITH GTL_NUMCHARS;
   iTextLength = SendMessage(hRichEdit, EM_GETTEXTLENGTHEX, (WPARAM)&oTextLength, NULL);

   /// Select and query first character
   Edit_SetSel(hRichEdit, 0, 1);
   RichEdit_GetParagraphFormat(hRichEdit, &oParagraphFormat);
   RichEdit_GetCharFormat(hRichEdit, SCF_SELECTION, &oCharacterFormat);

   // Generate initial state
   calculatePlainTextGenerateStateFromAttributes(&oCharacterFormat, &oParagraphFormat, &oState);

   /// Replace any existing content with empty paragraph using alignment of the first character
   deleteListContents(pMessage->pParagraphList);      // Delete existing paragraphs and items, but not the non-rich edit properties: Title, Author, Columns etc.
   pParagraph = createRichParagraph(oState.eAlignment);
   appendObjectToList(pMessage->pParagraphList, (LPARAM)pParagraph);

   // [CHECK] Ensure RichEdit has text
   if (iTextLength > 0)
   {
      /// Iterate through text character by character
      for (iPhraseStart = 0, iButtonIndex = 0, i = 1; i < iTextLength; i++)
      {
         // Select character
         Edit_SetSel(hRichEdit, i, i + 1);

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

            /// Append new button item to the paragraph
            pItem = createRichItemButton(pButtonData->szText, pButtonData->szID);
            appendRichTextItemToParagraph(pParagraph, pItem);

            // Start a new phrase using the NEXT character and it's formatting
            iPhraseStart = i + 1;
            Edit_SetSel(hRichEdit, iPhraseStart, iPhraseStart + 1);
            RichEdit_GetParagraphFormat(hRichEdit, &oParagraphFormat);
            RichEdit_GetCharFormat(hRichEdit, SCF_SELECTION, &oCharacterFormat);
            calculatePlainTextGenerateStateFromAttributes(&oCharacterFormat, &oParagraphFormat, &oState);
         }
         /// [TEXT]
         else
         {
            // Get formatting + alignment attributes for the character
            RichEdit_GetParagraphFormat(hRichEdit, &oParagraphFormat);
            RichEdit_GetCharFormat(hRichEdit, SCF_SELECTION, &oCharacterFormat);

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
   Edit_SetSel(hRichEdit, oOriginalSel.cpMin, oOriginalSel.cpMax);
   RichEdit_HideSelection(hRichEdit, FALSE);
   utilReleaseInterface(pRichEditOLE);
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
LANGUAGE_BUTTON*   insertRichEditButton(HWND  hRichEdit, CONST TCHAR*  szID, CONST TCHAR*  szText)
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
      pButton = createLanguageButton(hRichEdit, szText, szID);

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

/// Function name  : setRichEditText
// Description     : Replace RichEdit text with the contents of a RichText object
// 
// HWND                    hRichEdit   : [in] RichEdit window handle
// CONST RICH_TEXT*        pMessage    : [in] RichText message to display
// CONST GAME_TEXT_COLOUR  eBackground : [in] Defines the background colour. Must be GLC_BLACK or GLC_WHITE to
//                                             indicate whether background is light or dark, even if the background
//                                             colour isn't exactly white or black.  The default text colour
//                                             will be chosen to contrast the background. Other colours will not be altered.
// 
ControlsAPI
VOID  setRichEditText(HWND  hRichEdit, CONST RICH_TEXT*  pMessage, CONST GAME_TEXT_COLOUR  eBackground)
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
   Edit_SetSel(hRichEdit, 32768, 32768);

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
      RichEdit_SetParagraphFormat(hRichEdit, &oParagraphFormat);

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
            RichEdit_SetCharFormat(hRichEdit, SCF_SELECTION, &oCharacterFormat);
            Edit_ReplaceSel(hRichEdit, pItem->szText);
         }
         /// [BUTTON] Insert OLE object with appropriate text
         else 
            insertRichEditButton(hRichEdit, pItem->szID, pItem->szText);
            
         // Move caret beyond item
         Edit_SetSel(hRichEdit, 32768, 32768);
      }
   }
}
