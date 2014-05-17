//
// File Dialog (Suggestions).cpp : 
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        MACROS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : displayFileDialogSuggestions
// Description     : Shows or Hides the Suggestion list
// 
// FILE_DIALOG_DATA*  pDialogData : [in] File dialog data
// CONST BOOL         bShow       : [in] Whether to show or hide
// 
VOID   displayFileDialogSuggestions(FILE_DIALOG_DATA*  pDialogData, CONST BOOL  bShow)
{
   LISTVIEW_COLUMNS    oListView = { 1, IDS_AUTOCOMPLETE_COLUMN_NAME, 400, NULL, NULL, NULL, NULL };

   // [SHOW]
   if (bShow AND !pDialogData->hSuggestionList)
   {
      /// Create invisible ListView beneath the caret
      pDialogData->hSuggestionList = CreateWindow(WC_LISTVIEW, NULL, (WS_POPUP WITH WS_VSCROLL WITH WS_BORDER) WITH (LVS_OWNERDATA WITH LVS_SHOWSELALWAYS WITH LVS_REPORT WITH LVS_SINGLESEL WITH LVS_NOCOLUMNHEADER),
                                                  CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
                                                  pDialogData->hDialog, NULL, getAppInstance(), NULL);

      ERROR_CHECK("creating FileDialog Suggestion ListView", pDialogData->hSuggestionList);

      // [CHECK] Ensure control was created successfully
      if (pDialogData->hSuggestionList)
      {
         // Setup ListView
         initReportModeListView(pDialogData->hSuggestionList, &oListView, FALSE);
         ListView_SetImageList(pDialogData->hSuggestionList, pDialogData->hImageList, LVSIL_SMALL);

         // Set font
         SetWindowFont(pDialogData->hSuggestionList, GetWindowFont(pDialogData->hDialog), FALSE);
         
         // Set item count and resize ListView to fit
         ListView_SetItemCount(pDialogData->hSuggestionList, getTreeNodeCount(pDialogData->pSuggestionTree));
         updateFileDialogSuggestionPosition(pDialogData);

         // Display, and change focus back to the EditCtrl
         ShowWindow(pDialogData->hSuggestionList, SW_SHOWNOACTIVATE);
         SetFocus(pDialogData->hFileEdit);
      }
   }
   else if (!bShow AND pDialogData->hSuggestionList)
   {
      // Destroy the control and results tree
      utilDeleteWindow(pDialogData->hSuggestionList);      
      deleteAVLTree(pDialogData->pSuggestionTree);
   }
}


/// Function name  : updateFileDialogSuggestionPosition
// Description     : Resizes and positions the suggestion list
// 
// FILE_DIALOG_DATA*  pDialogData : [in] Dialog data
// 
VOID  updateFileDialogSuggestionPosition(FILE_DIALOG_DATA*  pDialogData)
{
   SIZE   siListView,      // ListView size
          siItem;          // ListView item size
   RECT   rcListView,      // ListView rectangle
          rcItem,          // ListView item rectangle
          rcEdit;          // Filename edit rectangle
   UINT   iDisplayCount;   // Number of items to display

   // Prepare
   GetWindowRect(pDialogData->hFileEdit, &rcEdit);

   // Calculate item height
   ListView_GetItemRect(pDialogData->hSuggestionList, 0, &rcItem, LVIR_BOUNDS);
   utilConvertRectangleToSize(&rcItem, &siItem);

   // Calculate items to display
   iDisplayCount = min(getTreeNodeCount(pDialogData->pSuggestionTree), 8);

   // Calculate ListView size
   siListView.cx = (rcEdit.right - rcEdit.left);
   siListView.cy = (siItem.cy * iDisplayCount);

   /// Reposition ListView
   utilSetRectangle(&rcListView, rcEdit.left, rcEdit.bottom, siListView.cx, siListView.cy);
   utilSetWindowRect(pDialogData->hSuggestionList, &rcListView, TRUE);

   // [COLUMN] Resize to encompass entire ListView
   if (iDisplayCount < getTreeNodeCount(pDialogData->pSuggestionTree))
      siListView.cx -= GetSystemMetrics(SM_CXHSCROLL);
   ListView_SetColumnWidth(pDialogData->hSuggestionList, 0, siListView.cx);
}


/// Function name  : updateFileDialogSuggestionResults
// Description     : Updates the results tree according to the current partial filename
// 
// FILE_DIALOG_DATA*  pDialogData : [in] Dialog data
// 
VOID   updateFileDialogSuggestionResults(FILE_DIALOG_DATA*  pDialogData)
{
   AVL_TREE_OPERATION*  pOperationData;
   TCHAR*               szSearchTerm;

   // [CHECK] Delete suggestion tree
   if (pDialogData->pSuggestionTree)
      deleteAVLTree(pDialogData->pSuggestionTree);

   /// Re-create suggestion tree
   pDialogData->pSuggestionTree = createFileItemTree(AK_PATH, AO_DESCENDING);
   pDialogData->pSuggestionTree->pfnDeleteNode = NULL;

   // Prepare
   pOperationData = createAVLTreeOperation(treeprocBuildFileDialogSuggestionTree, ATT_INORDER);
   szSearchTerm   = utilGetWindowText(pDialogData->hFileEdit);

   // Set parameters
   pOperationData->xFirstInput  = (LPARAM)szSearchTerm;
   pOperationData->xSecondInput = (LPARAM)(utilFindCharacterInSet(szSearchTerm, "?*") != NULL);
   pOperationData->pOutputTree  = pDialogData->pSuggestionTree;

   /// Populate tree
   performOperationOnAVLTree(pDialogData->pFileSearch->pResultsTree, pOperationData);
   performAVLTreeIndexing(pOperationData->pOutputTree);

   /// Save new tree
   pDialogData->pSuggestionTree = pOperationData->pOutputTree;

   // Cleanup
   deleteAVLTreeOperation(pOperationData);
   utilDeleteString(szSearchTerm);
}


/// Function name  : updateFileDialogSuggestions
// Description     : Shows or hides the suggestion list appropriately
// 
// FILE_DIALOG_DATA*  pDialogData : [in] Dialog data
// 
VOID  updateFileDialogSuggestions(FILE_DIALOG_DATA*  pDialogData)
{
   // [CHECK] Is a partial filename present?
   if (GetWindowTextLength(pDialogData->hFileEdit) > 0)
   {
      /// [SUCCESS] Update suggestion tree
      updateFileDialogSuggestionResults(pDialogData);

      // [CHECK] Are there at least two results?
      if (getTreeNodeCount(pDialogData->pSuggestionTree) > 0)
      {
         // [CHECK] Does list already exist?
         if (pDialogData->hSuggestionList)
         {
            /// [UPDATE] Resize list to fit results
            ListView_SetItemCount(pDialogData->hSuggestionList, getTreeNodeCount(pDialogData->pSuggestionTree));
            updateFileDialogSuggestionPosition(pDialogData);
         }
         else
            /// [NEW] Create results list
            displayFileDialogSuggestions(pDialogData, TRUE);
      }
      else
         /// [NO RESULTS] Hide list
         displayFileDialogSuggestions(pDialogData, FALSE);
   }
   /// [FAILED] Destroy list
   else if (pDialogData->hSuggestionList)
      displayFileDialogSuggestions(pDialogData, FALSE);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onFileDialog_InsertSuggestion
// Description     : Inserts the suggestion and hides the control
// 
// FILE_DIALOG_DATA*  pDialogData : [in] Dialog data
// CONST INT          iItemIndex  : [in] Suggestion tree index of the suggestion to insert
// 
VOID  onFileDialog_InsertSuggestion(FILE_DIALOG_DATA*  pDialogData, CONST INT  iItemIndex)
{
   FILE_ITEM*  pFileItem;

   // [CHECK] Lookup item
   if (findObjectInAVLTreeByIndex(pDialogData->pSuggestionTree, iItemIndex, (LPARAM&)pFileItem))
   {
      /// [FOUND] Insert and destroy list
      SetWindowText(pDialogData->hFileEdit, pFileItem->szDisplayName);
      Edit_SetSel(pDialogData->hFileEdit, 32768, 32768);
      displayFileDialogSuggestions(pDialogData, FALSE);
   }
}


/// Function name  : onFileDialog_SuggestionNotify
// Description     : WM_NOTIFY message processing
// 
// FILE_DIALOG_DATA*  pDialogData  : [in] File dialog data
// NMHDR*             pMessage     : [in] WM_NOTIFY message data
// 
// Return Value : TRUE if handled, otherwise FALSE
//
BOOL  onFileDialog_SuggestionNotify(FILE_DIALOG_DATA*  pDialogData, NMHDR*  pMessage)
{
   NMITEMACTIVATE*    pClickInfo;         // NM_DBLCLICK data
   NMLVKEYDOWN*       pKeyPressInfo;      //
   BOOL               bResult;            // Operation result
   
   
   // Prepare
   bResult = FALSE;

   // Examine notification
   switch (pMessage->code)
   {
   /// [ITEM DOUBLE CLICK] Insert selected item
   case NM_DBLCLK:
      // Prepare
      pClickInfo = (NMITEMACTIVATE*)pMessage;

      // [CHECK] User clicked an item
      if (pClickInfo->iItem != -1)
      {
         onFileDialog_InsertSuggestion(pDialogData, pClickInfo->iItem);
         bResult = TRUE;
      }
      break;

   /// [TAB/ENTER] Insert selected item
   case LVN_KEYDOWN:
      // Prepare
      pKeyPressInfo = (NMLVKEYDOWN*)pMessage;

      // Examine key
      switch (pKeyPressInfo->wVKey)
      {
      case VK_TAB:
      case VK_RETURN:
         onFileDialog_InsertSuggestion(pDialogData, ListView_GetNextItem(pDialogData->hSuggestionList, -1, LVNI_SELECTED));
         bResult = TRUE;
         break;
      }
      break;

   /// [REQUEST DATA]
   case LVN_GETDISPINFO:
      onFileDialog_RequestSuggestion(pDialogData, (NMLVDISPINFO*)pMessage);
      bResult = TRUE;
      break;

   /// [CUSTOM DRAW]
   case NM_CUSTOMDRAW:
      bResult = onCustomDrawListView(pDialogData->hDialog, pMessage->hwndFrom, (NMLVCUSTOMDRAW*)pMessage);
      SetWindowLong(pDialogData->hDialog, DWL_MSGRESULT, bResult);
      break;
   }

   // Cleanup and return result
   return bResult;
}


/// Function name  : onFileDialog_RequestSuggestion
// Description     : Provides suggestion listview item data
// 
// FILE_DIALOG_DATA*  pDialogData : [in] Dialog data
// NMLVDISPINFO*      pMessage    : [in] Message header
// 
VOID  onFileDialog_RequestSuggestion(FILE_DIALOG_DATA*  pDialogData, NMLVDISPINFO*  pMessage)
{
   LVITEM&     oOutput = pMessage->item;   // Conveience pointer for output data
   FILE_ITEM*  pFileItem;                   // Associated FileItem for the specified item

   /// Lookup suggestion item
   if (findObjectInAVLTreeByIndex(pDialogData->pSuggestionTree, oOutput.iItem, (LPARAM&)pFileItem))
   {
      // [TEXT] Supply data
      if (oOutput.mask INCLUDES LVIF_TEXT)
         StringCchCopy(oOutput.pszText, oOutput.cchTextMax, pFileItem->szDisplayName);

      // [IMAGE]
      oOutput.iImage = pFileItem->iIconIndex;
   }
   else if (oOutput.mask INCLUDES LVIF_TEXT)
      // [FAILED] Error
      StringCchPrintf(oOutput.pszText, oOutput.cchTextMax, TEXT("Error: Item %d of %d not found"), oOutput.iItem, getTreeNodeCount(pDialogData->pSuggestionTree));
   else
      oOutput.pszText = TEXT("ERROR: Item not found");
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     TREE FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////


BOOL  isPatternInString(CONST TCHAR*  szString, CONST TCHAR*  szPattern)
{
   CONST TCHAR *cp = NULL, *mp = NULL;


   while ((szString[0]) AND (szPattern[0] != '*')) 
   {
      if ((szPattern[0] != szString[0]) AND (szPattern[0] != '?')) 
         return FALSE;
   
      szPattern++;
      szString++;
   }

   while (szString[0]) 
   {
      if (szPattern[0] == '*') 
      {
         if (!szPattern[1]) 
            return TRUE;
         
         mp = ++szPattern;
         cp = (szString + 1);
      } 
      else if ((szPattern[0] == szString[0]) OR (szPattern[0] == '?')) 
      {
         szPattern++;
         szString++;
      } 
      else 
      {
         szPattern = mp;
         szString = cp++;
      }
   }

   while (szPattern[0] == '*') 
      szPattern++;
   
   return !szPattern[0];
}


/// Function name  : treeprocBuildFileDialogSuggestionTree
// Description     : Builds the auto-complete results tree for the file dialog
// 
// AVL_TREE_NODE*       pCurrentNode   : [in] Current node
// AVL_TREE_OPERATION*  pOperationData : [in] Operation data
// 
//       Operation Data:
///        CONST TCHAR* xFirstInput  : [in] Search term
///        BOOL         xSecondInput : [in] Use wildcards
// 
VOID  treeprocBuildFileDialogSuggestionTree(AVL_TREE_NODE*  pCurrentNode, AVL_TREE_OPERATION*  pOperationData)
{
   FILE_ITEM*      pCurrentFile;       // Node contents
   CONST TCHAR*    szSearchTerm;       // 
   BOOL            bMatch;

   // Extract parameters
   pCurrentFile = extractObjectPointer(pCurrentNode, FILE_ITEM);
   szSearchTerm = (CONST TCHAR*)pOperationData->xFirstInput;
   
   // [CHECK] Ensure item is not a folder
   if (bMatch = utilExcludes(pCurrentFile->iAttributes, FIF_FOLDER))
   {
      // [WILDCARDS] Check whether search term matches pattern
      if (pOperationData->xSecondInput)
         bMatch = isPatternInString(pCurrentFile->szDisplayName, szSearchTerm);

      // [NORMAL] Check for a partial string match
      else
         bMatch = utilCompareStringVariablesN(pCurrentFile->szDisplayName, szSearchTerm, lstrlen(szSearchTerm)) AND !utilCompareStringVariables(pCurrentFile->szDisplayName, szSearchTerm);
   }

   /// [SUCCESS] Insert into output tree
   if (bMatch)
      insertObjectIntoAVLTree(pOperationData->pOutputTree, (LPARAM)pCurrentFile);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : wndprocFileDialogFilenameEdit
// Description     : FileDialog edit control window proc
// 
LRESULT  wndprocFileDialogFilenameEdit(HWND  hCtrl, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   FILE_DIALOG_DATA*  pDialogData;
   WNDCLASS           oWindowClass;
   BOOL               bPassToBase;
   MSG*               pDialogMsg;

   // Prepare
   GetClassInfo(NULL, WC_EDIT, &oWindowClass);
   pDialogData = getFileDialogData(GetParent(hCtrl));
   bPassToBase = TRUE;

   // Examine message
   switch (iMessage)
   {
   case WM_GETDLGCODE:
      // Prepare
      pDialogMsg = (MSG*)lParam;

      // Examine key
      if (pDialogMsg AND pDialogMsg->message == WM_KEYDOWN)
      {
         // [ESCAPE/TAB/RETURN] Stop dialog processing these keys if suggestions are visible
         switch (wParam)
         {
         case VK_ESCAPE:
         case VK_TAB:
         case VK_RETURN:
            if (pDialogData->hSuggestionList)
               return DLGC_WANTMESSAGE;
         }
      }
      break;

   // [KEY-PRESS]
   case WM_KEYDOWN:
      // Examine press
      switch (wParam)
      {
      /// [ESCAPE] Abort Suggestions
      case VK_ESCAPE:
         if (pDialogData->hSuggestionList)
         {
            displayFileDialogSuggestions(pDialogData, FALSE);
            bPassToBase = FALSE;
         }
         break;

      /// [TAB/RETURN] Insert suggestion
      case VK_TAB:
      case VK_RETURN:
         if (pDialogData->hSuggestionList)
         {
            onFileDialog_InsertSuggestion(pDialogData, ListView_GetNextItem(pDialogData->hSuggestionList, -1, LVNI_SELECTED));
            displayFileDialogSuggestions(pDialogData, FALSE);
            bPassToBase = FALSE;
         }
         // [NO SUGGESTIONS] Move focus
         else if (wParam == VK_TAB)
         {
            SetFocus(GetNextDlgTabItem(pDialogData->hDialog, hCtrl, GetKeyState(VK_SHIFT) < 0));
            bPassToBase = FALSE;
         }
         break;

      /// [UP/DOWN/PAGEUP/PAGEDOWN] Scroll SuggestionList
      case VK_UP:
      case VK_DOWN:
      case VK_NEXT:
      case VK_PRIOR:
         if (pDialogData->hSuggestionList)
         {
            SendMessage(pDialogData->hSuggestionList, iMessage, wParam, lParam);
            bPassToBase = FALSE;
         }
         break;      
      }
      break;

   /// [KEY PRESS COMPLETE] Update suggestions
   case WM_KEYUP:
      switch (wParam)
      {
      case VK_UP:     case VK_DOWN:  
      case VK_LEFT:   case VK_RIGHT:
      case VK_ESCAPE: 
         break;

      default: 
         updateFileDialogSuggestions(pDialogData); 
         break;
      }
      break;

   /// [MOUSE-WHEEL] Scroll SuggestionList
   case WM_MOUSEWHEEL:
      if (pDialogData->hSuggestionList)
      {
         SendMessage(pDialogData->hSuggestionList, iMessage, wParam, lParam);
         bPassToBase = FALSE;
      }
      break;
   }
   
   // Pass to base, if appropriate
   return (bPassToBase ? CallWindowProc(oWindowClass.lpfnWndProc, hCtrl, iMessage, wParam, lParam) : 0);
}


