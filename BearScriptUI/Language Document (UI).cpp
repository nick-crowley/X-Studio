//
// Insert Argument Dialog.cpp : The 'new argument' dialog for the 'Arguments' document properties
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////

#define    STRING_COLUMN_ID         0      // String ID column
#define    STRING_COLUMN_TEXT       1      // Text column

#define    ListView_DeSelectAll(hCtrl)         ListView_SetItemState(hCtrl, -1, NULL, LVIS_SELECTED)
#define    ListView_SelectItem(hCtrl, iItem)   ListView_SetItemState(hCtrl, iItem, LVIS_SELECTED, LVIS_SELECTED)

enum  ResolveOperation  { ResolvePageIndex, ResolveStringIndex };

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : displayLanguageDocumentGameString
// Description     : Finds and selects the desired GameString
// 
// LANGUAGE_DOCUMENT*  pDocument : [in] Document
// const GAME_STRING*  pString   : [in] String to display
// 
VOID  displayLanguageDocumentGameString(LANGUAGE_DOCUMENT*  pDocument, const GAME_STRING*  pString)
{
   AVL_TREE_OPERATION*  pOperation;
   GAME_STRING*         pConfirm;

   // [CHECK] Abort if GameString isn't present
   if (!findObjectInAVLTreeByValue(getLanguageDocumentGameStringTree(pDocument), pString->iPageID, pString->iID, (LPARAM&)pConfirm))
      return;

   // Prepare
   pOperation              = createAVLTreeOperation(treeprocResolveGameStringIndex, ATT_PREORDER);
   pOperation->xFirstInput = (LPARAM)pString;
   pOperation->iFlags      = ResolvePageIndex;

   /// Resolve ListView index of page
   if (performOperationOnAVLTree(getLanguageDocumentGamePageTree(pDocument), pOperation))
   {
      // [OPTIONAL] Select input page
      if (ListView_GetSelected(pDocument->hPageList) != pOperation->xOutput)
      {
         ListView_DeSelectAll(pDocument->hPageList);
         ListView_SelectItem(pDocument->hPageList, pOperation->xOutput);
         ListView_EnsureVisible(pDocument->hPageList, pOperation->xOutput, FALSE);
      }

      /// Resolve ListView index of string
      pOperation->iFlags = ResolveStringIndex;
      performOperationOnAVLTree(pDocument->pPageStringsByID, pOperation);

      // [OPTIONAL] Deselect current string and select desired string
      if (ListView_GetSelected(pDocument->hStringList) != pOperation->xOutput)
      {
         ListView_DeSelectAll(pDocument->hStringList);
         ListView_SelectItem(pDocument->hStringList, pOperation->xOutput);
         ListView_EnsureVisible(pDocument->hStringList, pOperation->xOutput, FALSE);
      }
   }

   // Cleanup
   deleteAVLTreeOperation(pOperation);
}


/// Function name  : treeprocResolveGameStringIndex
// Description     : Locates the index of a specified GamePage or GameString
// 
// AVL_TREE_NODE*       pNode   : [in] Node containing a GameString or a GamePage
// AVL_TREE_OPERATION*  pData   : [in] xFirstInput : GameString to locate
//                                     iFlags      : ResolvePageIndex or ResolveStringIndex
//                                     xOutput     : Node index
// 
VOID  treeprocResolveGameStringIndex(AVL_TREE_NODE*  pNode, AVL_TREE_OPERATION*  pData)
{
   GAME_STRING*  pTargetString = (GAME_STRING*)pData->xFirstInput; // extractPointerFromTreeNode(pNode, GAME_STRING);
   BOOL          bMatch        = FALSE;
   
   // [PAGE] Compare GamePage node against target PageID
   if (pData->iFlags == ResolvePageIndex)
      bMatch = (extractPointerFromTreeNode(pNode, GAME_PAGE)->iPageID == pTargetString->iPageID);
   // [STRING] Compare GameString node against target StringID
   else
      bMatch = (extractPointerFromTreeNode(pNode, GAME_STRING)->iID == pTargetString->iID);
   
   /// [MATCH] Return index and abort search
   if (bMatch)
   {
      pData->xOutput     = pNode->iIndex;
      pData->bResult     = TRUE;
      pData->bProcessing = FALSE;
   }
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////


/// Function name  : onLanguageDocument_DeletePage
// Description     : 'Delete Page' Context menu handler
// 
// LANGUAGE_DOCUMENT*  pDocument : [in] Document
// GAME_PAGE*          pPage     : [in] Page to delete
// 
VOID  onLanguageDocument_DeletePage(LANGUAGE_DOCUMENT*  pDocument, GAME_PAGE*  pPage)
{
   // [CHECK] Document must not be virtual
   ASSERT(!pDocument->bVirtual);

   // Hide/Destroy Document PageStrings
   INT  iSelection = ListView_GetSelected(pDocument->hPageList);
   ListView_DeSelectAll(pDocument->hPageList);

   /// Delete all strings with input PageID
   deleteLanguageDocumentGamePage(pDocument, pPage);

   // Re-create Document PageStrings
   ListView_SelectItem(pDocument->hPageList, min(iSelection, ListView_GetItemCount(pDocument->hPageList)));
}


/// Function name  : onLanguageDocument_DeleteString
// Description     : 'Delete String' context menu handler
// 
// LANGUAGE_DOCUMENT*  pDocument : [in] Document
// GAME_STRING*        pString   : [in] String to delete
// 
VOID  onLanguageDocument_DeleteString(LANGUAGE_DOCUMENT*  pDocument, GAME_STRING*  pString)
{
   // [CHECK] Document must not be virtual
   ASSERT(!pDocument->bVirtual);

   // Hide/Destroy Document PageStrings
   INT  iSelection = ListView_GetSelected(pDocument->hStringList);
   ListView_DeSelectAll(pDocument->hStringList);

   /// Delete target string
   deleteLanguageDocumentGameString(pDocument, pString);

   // Re-create Document PageStrings
   ListView_SelectItem(pDocument->hStringList, min(iSelection, ListView_GetItemCount(pDocument->hStringList)));
}


/// Function name  : onLanguageDocument_EditPage
// Description     : 'Edit Page' Context menu handler
// 
// LANGUAGE_DOCUMENT*  pDocument : [in] Document
// GAME_PAGE*          pOldPage  : [in] Page being edited
// GAME_PAGE*          pNewPage  : [in] New page data
// 
VOID  onLanguageDocument_EditPage(LANGUAGE_DOCUMENT*  pDocument, GAME_PAGE*  pOldPage, GAME_PAGE*  pNewPage)
{
   // [CHECK] Document must not be virtual
   ASSERT(!pDocument->bVirtual);

   // Prepare
   INT  iSelected = ListView_GetSelected(pDocument->hPageList);

   /// [SAME ID] Update properties
   if (pOldPage->iPageID == pNewPage->iPageID)
   {
      // Update Values
      utilReplaceString(pOldPage->szTitle,       pNewPage->szTitle);
      utilReplaceString(pOldPage->szDescription, pNewPage->szDescription);
      pOldPage->bVoice = pNewPage->bVoice;

      // Cleanup
      deleteGamePage(pNewPage);
   }
   /// [NEW ID] Change ID + Properties
   else
   {
      // Hide/Destroy Document PageStrings
      ListView_DeSelectAll(pDocument->hPageList);

      /// Change PageID of all strings within page
      modifyLanguageDocumentGamePageID(pDocument, pOldPage, pNewPage);
      
      // Create/Show document PageStrings
      ListView_SelectItem(pDocument->hPageList, iSelected);
   }

   // Redraw current page
   ListView_RedrawItems(pDocument->hPageList, iSelected, iSelected);
}


/// Function name  : onLanguageDocument_EditString
// Description     : 'Edit String' context menu handler
// 
// LANGUAGE_DOCUMENT*  pDocument : [in] Document
// const GAME_STRING*  pString   : [in] String associated with currently selected string
// 
VOID  onLanguageDocument_EditString(LANGUAGE_DOCUMENT*  pDocument, GAME_STRING*  pString)
{
   BOOL  bChanged;

   // [CHECK] Ensure string selected
   ASSERT(ListView_GetSelected(pDocument->hStringList) != -1);

   // Examine subitem
   switch (pDocument->oStringClick.iSubItem)
   {
   /// [MODIFY ID]
   case STRING_COLUMN_ID:
      // [CHECK] Ensure non-virtual
      ASSERT(!pDocument->bVirtual);

      // Initiate label editing
      editCustomListViewItem(pDocument->hStringList, pDocument->oStringClick.iItem, STRING_COLUMN_ID, LVLT_EDIT);
      break;

   /// [EDIT FORMATTING]
   case STRING_COLUMN_TEXT:
      // Save current string
      ListView_DeSelectAll(pDocument->hStringList);

      // Display sourceText editor and redisplay string
      bChanged = (displaySourceTextDialog(pDocument, pString, getAppWindow()) == IDOK);
      ListView_SelectItem(pDocument->hStringList, pDocument->oStringClick.iItem);

      // [DOCUMENT CHANGED]
      if (bChanged)
         sendDocumentUpdated(AW_DOCUMENTS_CTRL);
      break;
   }
}


/// Function name  : onLanguageDocument_EditStringBegin
// Description     : Provide EditCtrl with StringID
// 
// LANGUAGE_DOCUMENT*   pDocument : [in] Document
// const NMLVDISPINFO*  pString   : [in] Label editing data
// 
VOID  onLanguageDocument_EditStringBegin(LANGUAGE_DOCUMENT*  pDocument, NMLVDISPINFO*  pHeader)
{
   // Display current StringID
   utilSetWindowInt(pHeader->hdr.hwndFrom, pDocument->pCurrentString->iID);
}


/// Function name  : onLanguageDocument_EditStringEnd
// Description     : Validate new ID then change string ID
// 
// LANGUAGE_DOCUMENT*   pDocument : [in] Document
// const NMLVDISPINFO*  pString   : [in] Label editing data
// 
BOOL  onLanguageDocument_EditStringEnd(LANGUAGE_DOCUMENT*  pDocument, NMLVDISPINFO*  pHeader)
{
   UINT          iNewStringID   = utilGetWindowInt(pHeader->hdr.hwndFrom);
   GAME_STRING*  pCurrentString = pDocument->pCurrentString;

   // Do nothing if the ID is unchanged
   if (iNewStringID == pCurrentString->iID)
      return TRUE;

   // [CHECK] Ensure desired ID is available
   if (!validateLanguagePageStringID(pDocument, iNewStringID))
      return FALSE;

   // Save current string
   INT  iSelected = ListView_GetSelected(pDocument->hStringList);
   ListView_DeSelectAll(pDocument->hStringList);

   /// Edit StringID + Refresh ListView
   modifyLanguageDocumentGameStringID(pDocument, pCurrentString, iNewStringID);

   // [DOCUMENT CHANGED] Select original string
   ListView_SelectItem(pDocument->hStringList, iSelected);
   sendDocumentUpdated(AW_DOCUMENTS_CTRL); 
   return TRUE;
}


/// Function name  : onLanguageDocument_GetMenuItemState
// Description     : Determines whether a toolbar/menu command relating to the document should be enabled or disabled
// 
// LANGUAGE_DOCUMENT*  pDocument : [in]     Document
// CONST UINT        iCommandID  : [in]     Menu/toolbar Command
// UINT*             piState     : [in/out] Combination of MF_ENABLED, MF_DISABLED, MF_CHECKED, MF_UNCHECKED
// 
BOOL   onLanguageDocument_GetMenuItemState(LANGUAGE_DOCUMENT*  pDocument, CONST UINT  iCommandID, UINT*  piState)
{
   *piState = MF_DISABLED;
   return TRUE;
}

/// Function name  : onLanguageDocument_InsertPage
// Description     : 'Insert Page' context menu handler
// 
// LANGUAGE_DOCUMENT*  pDocument : [in] Document
// GAME_PAGE*          pNewPage  : [in] Page being inserted
// 
VOID  onLanguageDocument_InsertPage(LANGUAGE_DOCUMENT*  pDocument, GAME_PAGE*  pNewPage)
{
   // [CHECK] Document must not be virtual
   ASSERT(!pDocument->bVirtual);

   // Hide/Destroy document PageStrings
   INT  iSelection = ListView_GetSelected(pDocument->hPageList);
   ListView_DeSelectAll(pDocument->hPageList);

   /// Insert new GamePage + Refresh ListView
   insertLanguageDocumentGamePage(pDocument, pNewPage);

   // Create/Show document PageStrings
   ListView_SelectItem(pDocument->hPageList, iSelection);
}


/// Function name  : onLanguageDocument_InsertString
// Description     : 'Insert String' context menu handler
// 
// LANGUAGE_DOCUMENT*  pDocument : [in] Document
// const UINT          iPageID   : [in] Page for new string
// 
VOID  onLanguageDocument_InsertString(LANGUAGE_DOCUMENT*  pDocument, const UINT  iPageID)
{
   GAME_STRING*  pNewString;

   // [CHECK] Non-Virtual document + Any page selected
   ASSERT(!pDocument->bVirtual AND pDocument->pCurrentPage);

   // De-select current string
   ListView_DeSelectAll(pDocument->hStringList);

   /// Insert new GameString (into current page) into Tree + ListView
   pNewString = createGameString(TEXT("NEW STRING"), identifyLanguagePageStringNextID(pDocument), iPageID, ST_INTERNAL);
   insertLanguageDocumentGameString(pDocument, pNewString);

   // Select new string
   ListView_SelectItem(pDocument->hStringList, ListView_GetItemCount(pDocument->hStringList) - 1);
}


/// Function name  : onLanguageDocument_PageSelectionChanged
// Description     : Displays new page strings
// 
// LANGUAGE_DOCUMENT*  pDocument : [in] Document
// CONST INT           iItem     : [in] Item index  (Only valid when bSelected == TRUE)
// CONST BOOL          bSelected : [in] Whether selected or de-selected
// 
VOID   onLanguageDocument_PageSelectionChanged(LANGUAGE_DOCUMENT*  pDocument, CONST INT  iItem, CONST BOOL  bSelected)
{
   INT  iLogicalItem;

   if (bSelected)
   {
      // [ITEM] Calculate item index 
      if ((iLogicalItem = GroupedListView_PhysicalToLogical(pDocument->hPageList, iItem)) != -1)
      {
         // [CHECK] Ensure Page + PageStrings do not exist
         ASSERT(!pDocument->pCurrentPage AND !pDocument->pPageStringsByID);

         // Generate strings for new page
         findLanguageDocumentGamePageByIndex(pDocument, iLogicalItem, pDocument->pCurrentPage);
         pDocument->pPageStringsByID = generateLanguagePageStringsTree(pDocument, pDocument->pCurrentPage);

         /// Display contents + select first
         ListView_SetItemCount(pDocument->hStringList, getTreeNodeCount(pDocument->pPageStringsByID));
         ListView_SetItemState(pDocument->hStringList, 0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
      }
      //// [HEADING] Select next item
      //else
      //{
      //   ListView_DeSelectAll(pDocument->hPageList);
      //   ListView_SelectItem(pDocument->hPageList, iItem + 2);
      //}
   }
   else
   {
      // [CHECK] Ensure user didn't de-select a heading
      if (pDocument->pCurrentPage AND pDocument->pPageStringsByID)
      {
         // Deselect current string, if any
         ListView_SetItemState(pDocument->hStringList, -1, NULL, LVIS_SELECTED);
         ListView_SetItemCount(pDocument->hStringList, 0);

         // Delete existing page contents
         deleteAVLTree(pDocument->pPageStringsByID);
         pDocument->pCurrentPage = NULL;
      }
   }
}


/// Function name  : onLanguageDocument_PropertyChanged
// Description     : Refresh current string when text or properties change
//
// LANGUAGE_DOCUMENT*  pDocument  : [in] Document 
// CONST UINT          iControlID : [in] ID of the control responsible for the change
// 
VOID  onLanguageDocument_PropertyChanged(LANGUAGE_DOCUMENT*  pDocument, CONST UINT  iControlID)
{
   INT   iSelected;

   switch (iControlID)
   {
   /// [MESSAGE CONTENTS]
   case IDC_LANGUAGE_EDIT:
   /// [MESSAGE PROPERTY]
   case IDC_AUTHOR_EDIT:
   case IDC_TITLE_EDIT:
   case IDC_COLUMN_ONE_RADIO:  
   case IDC_COLUMN_TWO_RADIO:  
   case IDC_COLUMN_THREE_RADIO:
   case IDC_COLUMN_WIDTH_CHECK:
   case IDC_COLUMN_SPACING_CHECK:
   case IDC_COLUMN_WIDTH_SLIDER:
   case IDC_COLUMN_SPACING_SLIDER:
      // [NON-VIRTUAL] Save text/properties to current message
      if (!pDocument->bVirtual)
      {
         getRichEditText(pDocument->hRichEdit, pDocument->pCurrentMessage);
         generateSourceTextFromRichText(pDocument->pCurrentMessage, pDocument->pCurrentString);
      }
      // Fall through...

   /// [STRING PROPERTY]
   case IDC_STRING_VERSION_COMBO:
      // Redraw current string
      iSelected = ListView_GetSelected(pDocument->hStringList);
      ListView_RedrawItems(pDocument->hStringList, iSelected, iSelected);
      break;

   }
}

#define RichEdit_FindTextEx(hCtrl, iFlags, pData)  SendMessage(hCtrl, EM_FINDTEXTEX, iFlags, (LPARAM)(FINDTEXTEX*)(pData))

VOID  highlightSpecial(HWND  hRichEdit)
{
   CHARFORMAT2  oFormat;
   FINDTEXTEX  oStart,
               oEnd;
   CHARRANGE   oOriginal;

   // Prepare
   utilZeroObject(&oStart, FINDTEXTEX);
   utilZeroObject(&oEnd, FINDTEXTEX);
   utilZeroObject(&oFormat, CHARFORMAT);

   // Setup search
   oStart.chrg.cpMax = -1;
   oStart.lpstrText  = TEXT("\\{");
   oEnd.lpstrText    = TEXT("\\}");
   oEnd.chrg.cpMax   = -1;

   // Setup format
   oFormat.cbSize = sizeof(CHARFORMAT2);
   oFormat.dwMask = CFM_COLOR | CFM_LINK; // CFM_UNDERLINE | CFM_COLOR | CFM_UNDERLINETYPE; // CFM_ITALIC    //CFM_BACKCOLOR | CFM_COLOR | 
   oFormat.dwEffects = CFE_LINK; // CFE_UNDERLINE;  // CFE_ITALIC
   //oFormat.bUnderlineType = CFU_UNDERLINEDOTTED;
   //oFormat.crBackColor = RGB(255,242,0);
   oFormat.crTextColor = RGB(255,255,255); // RGB(0,0,0);

   // Search for opening tag
   if (RichEdit_FindTextEx(hRichEdit, FR_DOWN, &oStart) != -1)
   {
      // [FOUND] Search from opening tag
      oEnd.chrg.cpMin = oStart.chrgText.cpMin;

      // Search for closing tag
      if (RichEdit_FindTextEx(hRichEdit, FR_DOWN, &oEnd) != -1)
      {
         // Preserve selection
         RichEdit_HideSelection(hRichEdit, TRUE);
         Edit_GetSelEx(hRichEdit, &oOriginal.cpMin, &oOriginal.cpMax);

         // Highlight tag
         Edit_SetSel(hRichEdit, oStart.chrgText.cpMin, oEnd.chrgText.cpMax);
         RichEdit_SetCharFormat(hRichEdit, SCF_SELECTION, &oFormat);

         // Restore selection
         Edit_SetSel(hRichEdit, oOriginal.cpMin, oOriginal.cpMax);
         RichEdit_HideSelection(hRichEdit, FALSE);
      }
   }
}

/// Function name  : onLanguageDocument_StringSelectionChanged
// Description     : Displays new string
// 
// LANGUAGE_DOCUMENT*  pDocument : [in] Document
// CONST INT           iItem     : [in] Item index  (Only valid when bSelected == TRUE)
// CONST BOOL          bSelected : [in] Whether selected or de-selected
// 
VOID   onLanguageDocument_StringSelectionChanged(LANGUAGE_DOCUMENT*  pDocument, CONST INT  iItem, CONST BOOL  bSelected)
{
   LANGUAGE_BUTTON*  pButtonData;

   // Disable EN_CHANGE notifications
   INT  iOldMask = RichEdit_SetEventMask(pDocument->hRichEdit, RichEdit_GetEventMask(pDocument->hRichEdit) ^ ENM_CHANGE);

   /// [SELECTED] Display string in RichEdit
   if (bSelected)
   {
      // Lookup desired string
      findLanguageDocumentGameStringByIndex(pDocument, iItem, pDocument->pCurrentString);

      // Generate RichText + display
      if (pDocument->bFormattingError = !generateMessageFromGameString(pDocument->pCurrentString, pDocument->pCurrentMessage, pDocument->pFormatErrors))
         SetWindowText(pDocument->hRichEdit, TEXT(""));
      else
      {
         // Insert text + buttons
         setRichEditText(pDocument->hRichEdit, pDocument->pCurrentMessage, false, GTC_BLACK);
         Edit_SetModify(pDocument->hRichEdit, FALSE);

         /// [DEBUG]
         //highlightSpecial(pDocument->hRichEdit);

         // Store data for inserted buttons
         for (INT iIndex = 0; findButtonInRichEditByIndex(pDocument->hRichEdit, iIndex, pButtonData); iIndex--)
            insertObjectIntoAVLTree(pDocument->pButtonsByID, (LPARAM)pButtonData);
      }

      // Enable/Disable window by result
      EnableWindow(pDocument->hRichEdit, !pDocument->bFormattingError);
   }
   /// [UNSELECTED] Update string from RichEdit
   else
   {
      // [MODIFIED + NON-VIRTUAL] Generate new source-text from RichEdit
      if (!pDocument->bVirtual AND Edit_GetModify(pDocument->hRichEdit))
      {
         getRichEditText(pDocument->hRichEdit, pDocument->pCurrentMessage);
         generateSourceTextFromRichText(pDocument->pCurrentMessage, pDocument->pCurrentString);
      }

      // Disable/Clear RichEdit
      SetWindowText(pDocument->hRichEdit, TEXT(""));
      EnableWindow(pDocument->hRichEdit, FALSE);

      // Destroy current Message/Errors
      deleteLanguageMessage(pDocument->pCurrentMessage);
      clearErrorQueue(pDocument->pFormatErrors);
      pDocument->pCurrentString = NULL;
   }

   // Refresh string properties
   sendDocumentUpdated(AW_PROPERTIES);

   // Re-Enable EN_CHANGE
   RichEdit_SetEventMask(pDocument->hRichEdit, iOldMask);
}


/// Function name  : onLanguageDocument_ViewFormattingError
// Description     : Displays the current formatting error, if any
// 
// LANGUAGE_DOCUMENT*  pDocument : [in] Document
// 
VOID   onLanguageDocument_ViewFormattingError(LANGUAGE_DOCUMENT*  pDocument)
{
   if (pDocument->bFormattingError)
   {
      if (getQueueItemCount(pDocument->pFormatErrors) > 1)
         displayErrorQueueDialog(getAppWindow(), pDocument->pFormatErrors, TEXT("String Formatting Error"));
      else
         displayErrorMessageDialog(getAppWindow(), firstErrorQueue(pDocument->pFormatErrors), TEXT("String Formatting Error"), MDF_OK | MDF_ERROR);
   }
}