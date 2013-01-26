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

#define    ListView_UnSelectAll(hCtrl)         ListView_SetItemState(hCtrl, -1, NULL, LVIS_SELECTED)
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
         ListView_UnSelectAll(pDocument->hPageList);
         ListView_SelectItem(pDocument->hPageList, pOperation->xOutput);
         ListView_EnsureVisible(pDocument->hPageList, pOperation->xOutput, FALSE);
      }

      /// Resolve ListView index of string
      pOperation->iFlags = ResolveStringIndex;
      performOperationOnAVLTree(pDocument->pPageStringsByID, pOperation);

      // [OPTIONAL] Deselect current string and select desired string
      if (ListView_GetSelected(pDocument->hStringList) != pOperation->xOutput)
      {
         ListView_UnSelectAll(pDocument->hStringList);
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
   ListView_UnSelectAll(pDocument->hPageList);

   /// Delete all strings with input PageID
   deleteLanguageDocumentGamePage(pDocument, pPage);

   // Re-create Document PageStrings
   ListView_SelectItem(pDocument->hPageList, min(iSelection, ListView_GetItemCount(pDocument->hPageList)));
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
      ListView_UnSelectAll(pDocument->hPageList);

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
      ListView_UnSelectAll(pDocument->hStringList);

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
   ListView_UnSelectAll(pDocument->hStringList);

   /// Edit StringID + Refresh ListView
   modifyLanguageDocumentGameStringID(pDocument, pCurrentString, iNewStringID);

   // [DOCUMENT CHANGED] Select original string
   ListView_SelectItem(pDocument->hStringList, iSelected);
   sendDocumentUpdated(AW_DOCUMENTS_CTRL); 
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
   ListView_UnSelectAll(pDocument->hPageList);

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
   ListView_UnSelectAll(pDocument->hStringList);

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
      if ((iLogicalItem = convertGroupedListViewPhysicalIndex(pDocument->hPageList, iItem)) != -1)
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
      //   ListView_SelectItem(pDocument->hPageList, GroupedListView_GetNextValidItem(pDocument->hPageList, iItem));
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
         generatePlainTextFromLanguageMessage(pDocument->pCurrentMessage, pDocument->pCurrentString);
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


/// Function name  : onLanguageDocument_StringSelectionChanged
// Description     : Displays new string
// 
// LANGUAGE_DOCUMENT*  pDocument : [in] Document
// CONST INT           iItem     : [in] Item index  (Only valid when bSelected == TRUE)
// CONST BOOL          bSelected : [in] Whether selected or de-selected
// 
VOID   onLanguageDocument_StringSelectionChanged(LANGUAGE_DOCUMENT*  pDocument, CONST INT  iItem, CONST BOOL  bSelected)
{
   IRichEditOle*  pRichEdit;  // RichEdit control OLE interface
   REOBJECT       oImage;     // RichEdit control OLE object attributes

   // Disable EN_CHANGE notifications
   INT  iOldMask = RichEdit_SetEventMask(pDocument->hRichEdit, RichEdit_GetEventMask(pDocument->hRichEdit) ^ ENM_CHANGE);

   /// [SELECTED] Display string in RichEdit
   if (bSelected)
   {
      // Lookup desired string
      findLanguageDocumentGameStringByIndex(pDocument, iItem, pDocument->pCurrentString);

      // Generate RichText + display
      if (pDocument->bFormattingError = !generateLanguageMessageFromGameString(pDocument->pCurrentString, pDocument->pCurrentMessage, pDocument->pFormatErrors))
         SetWindowText(pDocument->hRichEdit, TEXT(""));
      else
      {
         // Prepare
         utilZeroObject(&oImage, REOBJECT);
         oImage.cbStruct = sizeof(REOBJECT);
         RichEdit_GetOLEInterface(pDocument->hRichEdit, &pRichEdit);

         // Insert text + buttons
         setRichEditText(pDocument->hRichEdit, pDocument->pCurrentMessage, GTC_BLACK);
         Edit_SetModify(pDocument->hRichEdit, FALSE);

         // Store button data
         for (INT iIndex = 0; pRichEdit->GetObject(iIndex, &oImage, REO_GETOBJ_NO_INTERFACES) == S_OK; iIndex--)
            insertObjectIntoAVLTree(pDocument->pButtonsByID, (LPARAM)(LANGUAGE_BUTTON*)oImage.dwUser);
         
         // Cleanup
         utilReleaseInterface(pRichEdit);
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
         generatePlainTextFromLanguageMessage(pDocument->pCurrentMessage, pDocument->pCurrentString);
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