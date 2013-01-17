//
// Output Dialog.cpp  :  Error output window
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                  CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createOutputDialog
// Description     : Create and display the OutputDialog.  Unlike all the other dialogs the data for the OutputDialog
//                     is stored externally so it persists between instances.
// 
// OUTPUT_DIALOG_DATA*  pDialogData : [in] OutputDialog window data
// HWND                 hParentWnd  : [in] Dialog's parent window
// 
// Return Value   : OutputDialog window handle if succesful, otherwise NULL
// 
HWND  createOutputDialog(OUTPUT_DIALOG_DATA*  pDialogData, HWND  hParentWnd)
{
   HWND   hDialog;    // New dialog window handle

   // Create dialog
   hDialog = CreateDialogParam(getResourceInstance(), TEXT("OUTPUT_DIALOG"), hParentWnd, dlgprocOutputDialog, (LPARAM)pDialogData);
   ERROR_CHECK("creating output dialog", hDialog);

   // [DEBUG]
   DEBUG_WINDOW("Output Dialog", hDialog);

   // Return window handle
   return hDialog;
}


/// Function name  : createOutputDialogData
// Description     : Create Output dialog data
// 
// Return Value   : New OutputDialog data, you are responsible for destroying it
// 
OUTPUT_DIALOG_DATA*  createOutputDialogData()
{
   OUTPUT_DIALOG_DATA*  pDialogData;

   // Create empty object
   pDialogData = utilCreateEmptyObject(OUTPUT_DIALOG_DATA);

   // Set properties
   pDialogData->iLastOperationID = 1;

   /// Create data tree
   pDialogData->pItemTreeByOperation = createAVLTree(extractOutputDialogTreeNode, deleteOutputDialogTreeNode, createAVLTreeSortKey(AK_PAGE_ID, AO_ASCENDING), createAVLTreeSortKey(AK_ID, AO_ASCENDING), NULL);
   
   // Return new data
   return pDialogData;
}


/// Function name  : createOutputDialogItem
// Description     : Create an OutputDialog item from text. This has no message and cannot be double-clicked.
// 
// CONST OPERATION_DATA*  pOperation      : [in] Operation data
// CONST UINT             iOperationIndex : [in] Index within operation
// CONST TCHAR*           szText          : [in] Item text
// CONST TCHAR*           iIconID         : [in] Resource ID of the item's icon
// CONST UINT             iIndentation    : [in] Item indentation
// 
// Return Value   : New OutputDialog item, you are responsible for destroying it
// 
OUTPUT_DIALOG_ITEM*  createOutputDialogItem(CONST TCHAR*  szText, CONST TCHAR*  szIconID, CONST OPERATION_DATA*  pOperation, CONST UINT  iOperationIndex, CONST ERROR_STACK*  pError)
{
   OUTPUT_DIALOG_ITEM*  pNewItem;

   // Create item
   pNewItem = utilCreateEmptyObject(OUTPUT_DIALOG_ITEM);

   // Set properties
   pNewItem->szText     = utilDuplicateSimpleString(szText);
   pNewItem->iIconIndex = getAppImageTreeIconIndex(ITS_SMALL, szIconID);
   pNewItem->pError     = (pError ? duplicateErrorStack(pError) : NULL);

   // [OPERATION] Set operation properties
   if (pOperation)
   {
      pNewItem->iOperationID    = pOperation->iID;
      pNewItem->eOperationType  = pOperation->eType;
      pNewItem->iOperationIndex = iOperationIndex;

      // [ERROR] Indent within operation
      if (pError)
         pNewItem->iIndent = (pError->eType == ET_INFORMATION ? 1 : 2);
   }

   // Return new item
   return pNewItem;
}


/// Function name  : deleteOutputDialogData
// Description     : Destroy dialog data for the OutputDialog
// 
// OUTPUT_DIALOG_DATA*  pDialogData   : [in] OutputDialog data
// 
VOID   deleteOutputDialogData(OUTPUT_DIALOG_DATA*  pDialogData)
{
   // Delete item data
   deleteAVLTree(pDialogData->pItemTreeByOperation);

   // Delete calling object
   utilDeleteObject(pDialogData);
}


/// Function name  : deleteOutputDialogItem
// Description     : Destroy an output dialog item 
// 
// OUTPUT_DIALOG_ITEM*  &pItem   : [in] 
// 
VOID   deleteOutputDialogItem(OUTPUT_DIALOG_ITEM*  &pItem)
{
   // Delete text
   utilDeleteString(pItem->szText);

   // Delete error (if any)
   if (pItem->pError)
      deleteErrorStack(pItem->pError);

   // Delete calling object
   utilDeleteObject(pItem);
}


/// Function name  : deleteOutputDialogTreeNode
// Description     : Destroy an output dialog ListItem 
// 
// LPARAM  pOutputDialogItem   : [in] OutputDialogItem
// 
VOID  deleteOutputDialogTreeNode(LPARAM  pOutputDialogItem)
{
   deleteOutputDialogItem((OUTPUT_DIALOG_ITEM*&)pOutputDialogItem);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : calculateOutputDialogOperationID
// Description     : Calculates unique IDs for tracking operations
// 
// HWND  hDialog   : [in] Output dialog
// 
// Return Value   : Unique ID
// 
UINT  calculateOutputDialogOperationID(HWND  hDialog)
{
   OUTPUT_DIALOG_DATA*  pDialogData;

   // Prepare
   pDialogData = getOutputDialogData(hDialog);

   /// Calculate and return new ID
   return ++pDialogData->iLastOperationID;
}


/// Function name  : extractOutputDialogTreeNode
// Description     : Extract the desired property from a given TreeNode containing a OutputDialogItem
// 
// LPARAM                   pItem      : [in] OUTPUT_DIALOG_ITEM* : Node data containing an OutputDialogItem
// CONST AVL_TREE_SORT_KEY  eProperty  : [in] Property to retrieve
// 
// Return Value   : Value of the specified property or NULL if the property is unsupported
// 
LPARAM  extractOutputDialogTreeNode(LPARAM  pItem, CONST AVL_TREE_SORT_KEY  eProperty)
{
   OUTPUT_DIALOG_ITEM*  pOutputDialogItem;    // Convenience pointer
   LPARAM               xOutput;              // Property value output

   // Prepare
   pOutputDialogItem = (OUTPUT_DIALOG_ITEM*)pItem;

   // Examine property
   switch (eProperty)
   {
   case AK_PAGE_ID:   xOutput = (LPARAM)pOutputDialogItem->iOperationID;      break;
   case AK_ID:        xOutput = (LPARAM)pOutputDialogItem->iOperationIndex;   break;

   // [ERROR]
   default:          xOutput = NULL; ASSERT(FALSE);                break;
   }

   // Return property value
   return xOutput;
}


/// Function name  : findOutputDialogItem
// Description     : Find output dialog item data
// 
// OUTPUT_DIALOG_DATA*   pDialogData : [in]  OutputDialog data
// CONST UINT            iIndex      : [in]  Zero based index of the item to retrieve
// OUTPUT_DIALOG_ITEM*  &pOutput     : [out] OutputDialogItem if found, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BOOL   findOutputDialogItem(OUTPUT_DIALOG_DATA*  pDialogData, CONST UINT  iIndex, OUTPUT_DIALOG_ITEM*  &pOutput)
{
   // Query list
   return findObjectInAVLTreeByIndex(pDialogData->pItemTreeByOperation, iIndex, (LPARAM&)pOutput);
}


/// Function name  : identifyOutputDialogIconID
// Description     : Identifies the resource ID for an output dialog item
// 
// CONST OUTPUT_DIALOG_ICON  eIcon : [in] Output dialog icon ID
// 
// Return Value   : Resource ID
// 
CONST TCHAR*  identifyOutputDialogIconID(CONST OUTPUT_DIALOG_ICON  eIcon)
{
   CONST TCHAR*  szIconID;

   // Examine icon
   switch (eIcon)
   {
   case ODI_OPERATION:     szIconID = TEXT("PREFERENCES_ICON");    break;
   case ODI_SUCCESS:       szIconID = TEXT("VALID_ICON");          break;
   case ODI_FAILURE:       szIconID = TEXT("INVALID_ICON");        break;
   case ODI_INFORMATION:   szIconID = TEXT("INFORMATION_ICON");    break;
   case ODI_WARNING:       szIconID = TEXT("WARNING_ICON");        break;
   case ODI_ERROR:         szIconID = TEXT("ERROR_ICON");          break;
   }

   // Return ID
   return szIconID;
}

/// Function name  : getOutputDialogData
// Description     : Retrieve dialog data for the Output Dialog
// 
// HWND  hDialog   : [in] Window handle of the OutputDialog
// 
// Return Value   : Output dialog window data
// 
OUTPUT_DIALOG_DATA*  getOutputDialogData(HWND  hDialog)
{
   /// Extract from window (if exists), otherwise access via MainWindow data
   return (IsWindow(hDialog) ? (OUTPUT_DIALOG_DATA*)GetWindowLong(hDialog, DWL_USER) : getMainWindowData()->pOutputDlgData);
}


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : clearOutputDialogText
// Description     : Clear the output dialog items
// 
// HWND  hDialog   : [in] OutputDialog window handle
// 
VOID  clearOutputDialogText(HWND  hDialog)
{
   OUTPUT_DIALOG_DATA*  pDialogData;   // Output dialog data

   // Prepare
   pDialogData = getOutputDialogData(hDialog);

   /// Delete items and update ListView
   deleteAVLTreeContents(pDialogData->pItemTreeByOperation);
   updateOutputDialogList(pDialogData);
}


/// Function name  : initOutputDialog
// Description     : Initialise the OutputDialog
// 
// OUTPUT_DIALOG_DATA*  pDialogData  : [in] Window data
// 
VOID  initOutputDialog(OUTPUT_DIALOG_DATA*  pDialogData)
{
   LISTVIEW_COLUMNS oColumns = { 1, IDS_OUTPUT_COLUMN, 0, NULL, NULL, NULL, NULL };      // ListView column data
   RECT             rcClientRect;  // Dialog client rectangle

   /// Store window data
   SetWindowLong(pDialogData->hDialog, DWL_USER, (LONG)pDialogData);
   pDialogData->hListView = GetDlgItem(pDialogData->hDialog, IDC_OUTPUT_LIST);

   /// Stretch ListView over entire dialog
   GetClientRect(pDialogData->hDialog, &rcClientRect);
   utilSetClientRect(pDialogData->hListView, &rcClientRect, TRUE);

   /// Setup ListView
   oColumns.iColumnWidths[0] = (rcClientRect.right - rcClientRect.left);
   initReportModeListView(pDialogData->hListView, &oColumns, TRUE);
   
   /// Display items
   updateOutputDialogList(pDialogData);
}


/// Function name  : insertOutputDialogItem
// Description     : Append a new item to the output dialog
// 
// HWND                 hDialog  : [in] Output dialog
// OUTPUT_DIALOG_ITEM*  pNewItem : [in] New item to append
// 
VOID  insertOutputDialogItem(HWND  hDialog, OUTPUT_DIALOG_ITEM*  pNewItem)
{
   OUTPUT_DIALOG_DATA*  pDialogData;

   // [CHECK] Ensure item has an OperationID
   ASSERT(pNewItem->iOperationID);

   // Prepare
   pDialogData = getOutputDialogData(hDialog);

   /// Add item to list
   insertObjectIntoAVLTree(pDialogData->pItemTreeByOperation, (LPARAM)pNewItem);

   // [DEBUG] Print item to the console
   consolePrint(pNewItem->szText);
}


/// Function name  : printErrorToOutputDialog
// Description     : Appends an ErrorStack to the output dialog
// 
// CONST ERROR_STACK*  pError  : [in] ErrorStack to search for relevant attachment message
// 
VOID  printErrorToOutputDialog(CONST ERROR_STACK*  pError)
{
   MAIN_WINDOW_DATA*  pWindowData;

   // Prepare
   pWindowData = getMainWindowData();

   // [CHECK] Ensure main window and output data has been created
   if (pWindowData AND pWindowData->pOutputDlgData)
   {
      // Append error and update ListView
      printOperationErrorToOutputDialog(NULL, NULL, pError, TRUE);
      updateOutputDialogList(pWindowData->pOutputDlgData);
   }
}


/// Function name  : printMessageToOutputDialogf
// Description     : Append a formatted string with a custom icon to the output dialog
// 
// CONST UINT  iMessageID : [in] Resource ID of the formatted string to display
// ...         ...        : [in] ... 
// 
VOID  printMessageToOutputDialogf(CONST OUTPUT_DIALOG_ICON  eIcon, CONST UINT  iMessageID, ...)
{
   OUTPUT_DIALOG_ITEM  *pNewItem;        // Item being created
   va_list              pArguments;      // Variable argument list
   TCHAR               *szFormat,        // Formatting string
                       *szOutput;        // Generated item text
   HWND                 hDialog;         // Output dialog

   // [TRACK]
   TRACK_FUNCTION();

   // Prepare
   pArguments = utilGetFirstVariableArgument(&iMessageID);
   hDialog    = getMainWindowData()->hOutputDlg;
   
   /// Load and format string
   szFormat = utilLoadString(getResourceInstance(), iMessageID, 512);
   szOutput = utilCreateStringVf(512, szFormat, pArguments);

   /// Create new item
   pNewItem = createOutputDialogItem(szOutput, identifyOutputDialogIconID(eIcon), NULL, 0, NULL);

   // [ORPHAN] Display alone with no indentation
   pNewItem->iOperationID = calculateOutputDialogOperationID(hDialog);

   // Add item and update ListView
   insertOutputDialogItem(hDialog, pNewItem);
   updateOutputDialogList(getMainWindowData()->pOutputDlgData);

   // Cleanup
   utilDeleteStrings(szFormat, szOutput);
   END_TRACKING();
}


/// Function name  : printOperationErrorToOutputDialog
// Description     : Inserts an error into the appropriate operation in the output dialog
// 
// CONST UINT          iOperationID : [in] [optional] ID of operation to display message under, or NULL to display alone
// CONST UINT          iItemIndex   : [in] [optional] Index within operation, if any
// CONST ERROR_STACK*  pError       : [in]            ErrorStack to search for relevant attachment message
// 
VOID  printOperationErrorToOutputDialog(CONST OPERATION_DATA*  pOperation, CONST UINT  iItemIndex, CONST ERROR_STACK*  pError, CONST BOOL  bAddToConsole)
{
   ERROR_MESSAGE*       pOutputMessage;   // ErrorStack item iterator
   OUTPUT_DIALOG_ITEM*  pNewItem;         // New item being created
   HWND                 hDialog;          // Output dialog

   // Prepare
   TRACK_FUNCTION();
   pOutputMessage = NULL;

   // [CHECK] Ensure ErrorStack isn't empty
   ASSERT(getErrorStackCount(pError));

   /// [VERBOSE] Print error stack to console
   if (bAddToConsole AND isAppVerbose())
      consolePrintError(pError);

   // Prepare
   hDialog = getMainWindowData()->hOutputDlg;

   /// Search for the last 'OutputText Attachment' 
   for (INT iIndex = (getErrorStackCount(pError) - 1); findErrorMessageByIndex(pError, iIndex, pOutputMessage); iIndex--)
   {
      // [CHECK] Search for Output attachment if any, otherwise Use message following 'location' message.
      if (pOutputMessage->iID == IDS_ERROR_APPEND_OUTPUT OR iIndex <= 1)
         // [FOUND] Use this message as the output string
         break;
   }

   // [CHECK] Ensure message was found
   ASSERT(pOutputMessage);

   /// Create new item
   pNewItem = createOutputDialogItem(pOutputMessage->szMessage, identifyErrorTypeIconID(pError->eType), pOperation, iItemIndex, pError);

   // [ORPHAN] Display alone with no indentation
   if (!pOperation)
      pNewItem->iOperationID = calculateOutputDialogOperationID(hDialog);

   // Insert new item. Do not update ListView, this function is often used in batch.
   insertOutputDialogItem(hDialog, pNewItem);

   // [TRACK]
   END_TRACKING();
}


/// Function name  : printOperationErrorQueueToOutputDialog
// Description     : Prints the error queue of an operation to the output dialog
// 
// CONST OPERATION_DATA*  pOperation    : [in] Operation data
// CONST UINT             iInitialIndex : [in] Item index for the first message, they are sequential after that
// 
VOID  printOperationErrorQueueToOutputDialog(CONST OPERATION_DATA*  pOperation)
{
   ERROR_STACK*  pError;
   UINT          iIndex;

   // Prepare
   iIndex = 1;

   // [VERBOSE]
   VERBOSE_HEADING("Appending operation ErrorQueue...");

   /// Add to error to the output dialog
   for (LIST_ITEM*  pIterator = getListHead(pOperation->pErrorQueue); pError = extractListItemPointer(pIterator, ERROR_STACK); pIterator = pIterator->pNext)
      // Output at current index, then increment
      printOperationErrorToOutputDialog(pOperation, iIndex++, pError, FALSE);

   // Do not update ListView yet.
   
   /// [CONSOLE] Print to console
   consolePrintErrorQueue(pOperation->pErrorQueue);
}


/// Function name  : printOperationStateToOutputDialogf
// Description     : Appends an operation name, or inserts a result under the appropriate operation of the output dialog
// 
// CONST OPERATION_DATA*  pOperation      : [in] Operation data
// CONST UINT             iMessageID      : [in] Resource ID of the formatted string to display
// ...                    ...             : [in] ... 
// 
VOID  printOperationStateToOutputDialogf(OPERATION_DATA*  pOperation, CONST UINT  iMessageID, ...)
{
   OUTPUT_DIALOG_ITEM  *pNewItem;        // Item being created
   OUTPUT_DIALOG_ICON   eIcon;           // Item icon
   va_list              pArguments;      // Variable argument list
   TCHAR               *szFormat,        // Formatting string
                       *szOutput;        // Generated item text
   HWND                 hDialog;         // Output dialog
   UINT                 iItemIndex;      // Index of item being inserted

   // [TRACK]
   TRACK_FUNCTION();

   // [CHECK] Ensure operation exists
   ASSERT(pOperation);

   // Prepare
   pArguments = utilGetFirstVariableArgument(&iMessageID);
   hDialog    = getMainWindowData()->hOutputDlg;
   eIcon      = ODI_OPERATION;
   iItemIndex = (isOperationComplete(pOperation) ? 1 : 0);

   // [CHECK] Is this the first operation message?
   if (!isOperationComplete(pOperation))
      /// [STARTING] Calculate new operation ID
      pOperation->iID = calculateOutputDialogOperationID(hDialog);
   
   /// [COMPLETE] Print operation errors, if any
   else if (hasErrors(pOperation->pErrorQueue))
   {
      // Print errorQueue, increment output 
      printOperationErrorQueueToOutputDialog(pOperation);
      iItemIndex += getQueueItemCount(pOperation->pErrorQueue);
   }
   
   /// Load and format string
   szFormat = utilLoadString(getResourceInstance(), iMessageID, 512);
   szOutput = utilCreateStringVf(512, szFormat, pArguments);

   // [RESULT] Use the success/failure icon
   if (isOperationComplete(pOperation))
      eIcon = (pOperation->eResult == OR_SUCCESS ? ODI_SUCCESS : ODI_FAILURE);

   /// Create new item
   pNewItem = createOutputDialogItem(szOutput, identifyOutputDialogIconID(eIcon), pOperation, iItemIndex, NULL);

   // Add item and refresh ListView
   insertOutputDialogItem(hDialog, pNewItem);
   updateOutputDialogList(getMainWindowData()->pOutputDlgData);

   // Cleanup
   utilDeleteStrings(szFormat, szOutput);
   END_TRACKING();
}

//VOID  setOutputDialogRedraw(HWND  hDialog, CONST BOOL  bEnable)
//{
//   OUTPUT_DIALOG_DATA*  pDialogData;
//
//   // [CHECK] Ensure window is visible
//   if (pDialogData = getOutputDialogData(hDialog))
//      // [SUCCESS]
//      SetWindowRedraw(pDialogData->hListView, bEnable);
//}

/// Function name  : updateOutputDialogList
// Description     : Update the ListView to reflect the current number of items
// 
// OUTPUT_DIALOG_DATA*  pDialogData : [in] Output dialog data
// 
VOID  updateOutputDialogList(OUTPUT_DIALOG_DATA*  pDialogData)
{
   INT   iItemCount;

   // Re-index tree
   performAVLTreeIndexing(pDialogData->pItemTreeByOperation);

   // [CHECK] Is dialog visible?
   if (IsWindow(pDialogData->hDialog))
   {
      // Prepare
      iItemCount = getTreeNodeCount(pDialogData->pItemTreeByOperation);

      /// Set item count and scroll to the last item
      ListView_SetItemCountEx(pDialogData->hListView, iItemCount, LVSICF_NOSCROLL);
      ListView_EnsureVisible(pDialogData->hListView, max(0, iItemCount - 1), FALSE);
   }
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onOutputDialogClose
// Description     : Command the main window to toggle the visibility of the dialog
// 
// OUTPUT_DIALOG_DATA*  pDialogData : [in] Window data
//
VOID  onOutputDialogClose(OUTPUT_DIALOG_DATA*  pDialogData)
{
   // Send 'Toggle Output Dialog' to MainWindow
   SendMessage(getAppWindow(), WM_COMMAND, IDM_VIEW_COMPILER_OUTPUT, NULL);
}


/// Function name  : onOutputDialogCommand
// Description     : Command processing
// 
// OUTPUT_DIALOG_DATA*  pDialogData   : [in] Window data
// CONST UINT           iControlID    : [in] ID of the control
// CONST UINT           iNotification : [in] Notification
// 
// Return Value   : TRUE if processed, FALSE otherwise
// 
BOOL  onOutputDialogCommand(OUTPUT_DIALOG_DATA*  pDialogData, CONST UINT  iControlID, CONST UINT  iNotification)
{
   BOOL   bResult;

   // Prepare
   TRACK_FUNCTION();
   bResult = FALSE;

   // Examine source
   switch (iControlID)
   {
   /// [CLEAR] Destroy current items
   case IDM_OUTPUT_CLEAR:
      clearOutputDialogText(pDialogData->hDialog);
      bResult = TRUE;
      break;
   }

   // Return result
   END_TRACKING();
   return bResult;
}

/// Function name  : onOutputDialogContextMenu
// Description     : Display some kind of context menu
// 
// OUTPUT_DIALOG_DATA*  pDialogData : [in] Window data
// HWND                 hCtrl       : [in] Window handle of the control that was right-clicked
// POINT*               ptCursor    : [in] Cursor position in screen co-ordinates
// 
VOID  onOutputDialogContextMenu(OUTPUT_DIALOG_DATA*  pDialogData, HWND  hCtrl, POINT*  ptCursor)
{
   CUSTOM_MENU*  pCustomMenu;    // Custom Popup menu

   TRACK_FUNCTION();

   // [CHECK] Ensure source was the ListView
   if (GetDlgCtrlID(hCtrl) == IDC_OUTPUT_LIST)
   {
      /// Create CustomMenu
      pCustomMenu = createCustomMenu(TEXT("DIALOG_MENU"), TRUE, IDM_OUTPUT_POPUP);

      /// [CLEAR] Enable/Disable item based on whether dialog contains any items 
      EnableMenuItem(pCustomMenu->hPopup, IDM_OUTPUT_CLEAR, (getTreeNodeCount(pDialogData->pItemTreeByOperation) ? MF_ENABLED : MF_DISABLED));
      
      // Display the pop-up menu
      TrackPopupMenu(pCustomMenu->hPopup, TPM_LEFTALIGN WITH TPM_TOPALIGN WITH TPM_LEFTBUTTON, ptCursor->x, ptCursor->y, NULL, pDialogData->hDialog, NULL);
      
      // Cleanup
      deleteCustomMenu(pCustomMenu);
   }

   END_TRACKING();
}


/// Function name  : onOutputDialogCustomDraw
// Description     : Draws the ListView
// 
// OUTPUT_DIALOG_DATA*  pDialogData   : [in] Window data
// NMLVCUSTOMDRAW*      pMessageData  : [in] Custom draw data
// 
// Return type : TRUE if processed, FALSE otherwise 
//
BOOL   onOutputDialogCustomDraw(OUTPUT_DIALOG_DATA*  pDialogData, NMLVCUSTOMDRAW*  pMessageData)
{
   CONST UINT           iIconSize = 16 + (GetSystemMetrics(SM_CXEDGE) * 2);      // Defines distance between start of icon and start of text
   OUTPUT_DIALOG_ITEM*  pDialogItem;     // Item data
   NMCUSTOMDRAW*        pDrawData;       // Convenience pointer for the custom draw data
   BOOL                 bResult;         // Operation result
   POINT                ptIcon;          // Icon position
   RECT                 rcItem,          // Bounding rectangle
                        rcText;          // Text rectangle
   
   // Prepare
   TRACK_FUNCTION();
   pDrawData = &pMessageData->nmcd;
   bResult   = FALSE;

   // Examine draw stage
   switch (pDrawData->dwDrawStage)
   {
   /// [PREPAINT]
   case CDDS_PREPAINT:
      // Request per-item custom draw.
      SetWindowLong(pDialogData->hDialog, DWL_MSGRESULT, CDRF_NOTIFYITEMDRAW);
      bResult = TRUE;
      break;

   /// [ITEM PAINT]
   case CDDS_ITEMPREPAINT:
      // Lookup Item
      findOutputDialogItem(pDialogData, pDrawData->dwItemSpec, pDialogItem);
      ASSERT(pDialogItem);

      // Get bounding item rectangle
      ListView_GetItemRect(pDialogData->hListView, pDrawData->dwItemSpec, &rcItem, LVIR_BOUNDS);
      rcText = rcItem;

      // Calculate text rectangle and icon position
      InflateRect(&rcText, -GetSystemMetrics(SM_CXEDGE), NULL);
      utilConvertRectangleToPoint(&rcText, &ptIcon);
      rcText.left += iIconSize;

      /// [INDENT] Offset icon/text appropriately
      if (pDialogItem->iIndent)
      {
         ptIcon.x += iIconSize * pDialogItem->iIndent;
         OffsetRect(&rcText, iIconSize * pDialogItem->iIndent, NULL);
      }

      // [CHECK] Is non-information item selected?
      if ((pDrawData->uItemState INCLUDES CDIS_SELECTED) AND pDialogItem->pError)
      {
         /// [SELECTED] Draw a highlight rectangle and selected icon
         drawCustomSelectionRectangle(pDrawData->hdc, &rcItem);
         drawIcon(getAppImageList(ITS_SMALL), pDialogItem->iIconIndex, pDrawData->hdc, ptIcon.x, ptIcon.y, IS_SELECTED);
      }
      else
      {
         /// [DEFAULT] Draw a white background and normal icon
         FillRect(pDrawData->hdc, &rcItem, GetSysColorBrush(COLOR_WINDOW));
         drawIcon(getAppImageList(ITS_SMALL), pDialogItem->iIconIndex, pDrawData->hdc, ptIcon.x, ptIcon.y, IS_NORMAL);
      }

      /// [TEXT] Always draw text in blank
      DrawText(pDrawData->hdc, pDialogItem->szText, lstrlen(pDialogItem->szText), &rcText, DT_LEFT WITH DT_VCENTER WITH DT_SINGLELINE WITH DT_END_ELLIPSIS);

      // Inform dialog we've painted entire item
      SetWindowLong(pDialogData->hDialog, DWL_MSGRESULT, CDRF_SKIPDEFAULT);
      bResult = TRUE;
      break;

   }

   // Cleanup and return
   END_TRACKING();
   return bResult;
}


/// Function name  : onOutputDialogDoubleClick
// Description     : Extract and display errors within items
// 
// OUTPUT_DIALOG_DATA*  pDialogData : [in] Window data
// NMITEMACTIVATE*      pClickData  : [in] 
// 
// Return type : TRUE if processed, FALSE otherwise 
//
BOOL  onOutputDialogDoubleClick(OUTPUT_DIALOG_DATA*  pDialogData, NMITEMACTIVATE*  pClickData)
{
   OUTPUT_DIALOG_ITEM*  pItem;      // Output dialog item
   CONST TCHAR*         szErrorType;
   TCHAR*               szTitle;

   // Prepare
   TRACK_FUNCTION();
   szTitle = NULL;

   // [ERROR ITEMS] Display the ErrorStack in a message box
   if (pClickData->iItem != -1 AND findOutputDialogItem(pDialogData, pClickData->iItem, pItem))
   {
      // [CHECK] Ensure item is an error
      if (pItem->pError AND pItem->pError->eType != ET_INFORMATION)
      {
         // Prepare
         szErrorType = identifyErrorTypeString(pItem->pError->eType);

         // Generate title
         switch (pItem->eOperationType)
         {
           case OT_LOAD_GAME_DATA:       szTitle = utilCreateStringf(128, TEXT("Game Data Translation %s"), szErrorType);     break;
           case OT_LOAD_PROJECT_FILE:    szTitle = utilCreateStringf(128, TEXT("Project Loading %s"), szErrorType);           break;
           case OT_SAVE_PROJECT_FILE:    szTitle = utilCreateStringf(128, TEXT("Project Saving %s"), szErrorType);            break;
           case OT_LOAD_SCRIPT_FILE:     szTitle = utilCreateStringf(128, TEXT("MSCI Script Decompiler %s"), szErrorType);    break;
           case OT_LOAD_LANGUAGE_FILE:   szTitle = utilCreateStringf(128, TEXT("Language File Decompiler %s"), szErrorType);  break;
           case OT_SAVE_SCRIPT_FILE:     szTitle = utilCreateStringf(128, TEXT("MSCI Script Compiler %s"), szErrorType);      break;
           case OT_SAVE_LANGUAGE_FILE:   szTitle = utilCreateStringf(128, TEXT("Langage File Compiler %s"), szErrorType);     break;
           case OT_SUBMIT_BUG_REPORT:    szTitle = utilCreateStringf(128, TEXT("Bug Submission %s"), szErrorType);            break;
           case OT_SUBMIT_CORRECTION:    szTitle = utilCreateStringf(128, TEXT("Correction Submission %s"), szErrorType);     break;
           case OT_SEARCH_SCRIPT_CALLS:  szTitle = utilCreateStringf(128, TEXT("Dependency Search %s"), szErrorType);         break;
           case OT_VALIDATE_SCRIPT_FILE: szTitle = utilCreateStringf(128, TEXT("MSCI Script Validation %s"), szErrorType);    break;
         }

         // Display message
         displayErrorMessageDialog(NULL, pItem->pError, szTitle, MDF_OK);

         // Cleanup
         utilSafeDeleteString(szTitle);
      }
   }

   // Return TRUE
   END_TRACKING();
   return TRUE;
}


/// Function name  : onOutputDialogMoving
// Description     : Prevents the output dialog being dragged around by it's caption
// 
// OUTPUT_DIALOG_DATA*  pDialogData  : [in] Dialog data
// RECT*                pWindowRect  : [in/out] Window rectangle
// 
// Return Value   : TRUE if processed, otherwise FALSE
// 
BOOL  onOutputDialogMoving(OUTPUT_DIALOG_DATA*  pDialogData, RECT*  pWindowRect)
{
   PANE*    pOutputPane;      // Workspace pane containing the dialog
   BOOL     bResult;          // Operation result

   // Prepare
   bResult = TRUE;

   // [CHECK] Lookup workspace pane containing the dialog
   if (pWindowRect AND findWorkspacePaneByHandle(getMainWindowData()->hWorkspace, pDialogData->hDialog, NULL, NULL, pOutputPane))
   {
      /// Change window rectangle to the pane rectangle
      *pWindowRect = pOutputPane->rcPane;

      // Convert to screen co-ordinates
      utilScreenToClientRect(pDialogData->hDialog, pWindowRect);
   }
   else
      // [FAILED] Return FALSE
      bResult = FALSE;

   // Return result
   return bResult;
}


/// Function name  : onOutputDialogNotify
// Description     : Handlers notification processing
// 
// OUTPUT_DIALOG_DATA*  pDialogData : [in] Window data
// CONST UINT           iControlID  : [in] ID of source control
// NMHDR*               pMessage    : [in] Message data
// 
// Return Value   : TRUE if processed, FALSE otherwise
// 
BOOL  onOutputDialogNotify(OUTPUT_DIALOG_DATA*  pDialogData, CONST UINT  iControlID, NMHDR*  pMessage)
{
   NMLVDISPINFO*  pDisplayInfo;
   BOOL           bResult;

   // [TRACK]
   TRACK_FUNCTION();

   // Prepare
   bResult = TRUE;

   // Examine notification
   switch (pMessage->code)
   {
   /// [LISTVIEW DOUBLE CLICK]
   case NM_DBLCLK:
      bResult = onOutputDialogDoubleClick(pDialogData, (NMITEMACTIVATE*)pMessage);
      break;

   /// [LISTVIEW REQUEST DATA]
   case LVN_GETDISPINFO:
      // Prepare
      pDisplayInfo = (NMLVDISPINFO*)pMessage;
      // Supply data
      onOutputDialogRequestData(pDialogData, &pDisplayInfo->item);
      break;

   // [UNHANDLED] Pass to CustomListView handler
   default: 
      bResult = onCustomListViewNotify(pDialogData->hDialog, TRUE, IDC_OUTPUT_LIST, pMessage);
      break;
   }

   // Return result
   END_TRACKING();
   return bResult;
}


/// Function name  : onOutputDialogResize
// Description     : Stretch the ListView over the entire dialog
// 
// OUTPUT_DIALOG_DATA*  pDialogData : [in] Window data
// CONST SIZE*          pNewSize    : [in] New size
// 
VOID  onOutputDialogResize(OUTPUT_DIALOG_DATA*  pDialogData, CONST SIZE*  pNewSize)
{
   RECT  rcClientRect;     // Dialog client rectangle        
   SIZE  siClient;         // Size of dialog client rect

   TRACK_FUNCTION();
   
   /// Stretch ListView control over entire dialog
   GetClientRect(pDialogData->hDialog, &rcClientRect);
   utilSetClientRect(pDialogData->hListView, &rcClientRect, TRUE);

   // Adjust ListView column width to match
   utilConvertRectangleToSize(&rcClientRect, &siClient);
   ListView_SetColumnWidth(pDialogData->hListView, 0, siClient.cx - GetSystemMetrics(SM_CXVSCROLL));

   END_TRACKING();
}


/// Function name  : onOutputDialogRequestData
// Description     : Supply the text and icon 
// 
// OUTPUT_DIALOG_DATA*  pDialogData : [in]     Window data
// LVITEM*              pOutput     : [in/out] Desired item data
// 
VOID  onOutputDialogRequestData(OUTPUT_DIALOG_DATA*  pDialogData, LVITEM*  pOutput)
{
   OUTPUT_DIALOG_ITEM*  pItem;
   BOOL                 bDrBullwinkle = TRUE;

   TRACK_FUNCTION();
   ASSERT(bDrBullwinkle AND pOutput != NULL);

   /// Lookup desired item
   if (findOutputDialogItem(pDialogData, pOutput->iItem, pItem))
   {
      // [DEBUG] DrBullWinkle checks
      ASSERT(bDrBullwinkle AND pItem != NULL);
      ASSERT(bDrBullwinkle AND pItem->szText != NULL);

      // [TEXT]
      if (pOutput->mask INCLUDES LVIF_TEXT)
      {
         ASSERT(bDrBullwinkle AND pOutput->pszText != NULL);
         StringCchCopy(pOutput->pszText, pOutput->cchTextMax, pItem->szText);
      }

      // [ICON/INDENT] 
      pOutput->iImage  = pItem->iIconIndex;
      pOutput->iIndent = pItem->iIndent;

      // [STATE] Disable item if it doesn't represent an error
      if (!pItem->pError)
      {
         pOutput->state = LVIS_DISABLED;
         pOutput->mask |= LVIF_STATE;
      }
   }
   // [ERROR] Item not found
   else if (pOutput->mask INCLUDES LVIF_TEXT)
      StringCchPrintf(pOutput->pszText, pOutput->cchTextMax, TEXT("ERROR: Item %d of %d not found"), pOutput->iItem, getTreeNodeCount(pDialogData->pItemTreeByOperation));
   else
      pOutput->pszText = TEXT("ERROR: Item not found");

   END_TRACKING();
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     DIALOG PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : dlgprocOutputDialog
// Description     : Output Dialog window procedure
// 
INT_PTR  dlgprocOutputDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   OUTPUT_DIALOG_DATA*  pDialogData;      // Dialog data
   ERROR_STACK*         pException;       // Exception error
   POINT                ptCursor;         // Cursor position
   SIZE                 siWindow;         // Window size
   BOOL                 bResult;          // Operation result

   // Prepare
   TRACK_FUNCTION();
   bResult = TRUE;

   /// [GUARD BLOCK]
   __try
   {
      // Get dialog data
      pDialogData = getOutputDialogData(hDialog);

      // Examine message
      switch (iMessage)
      {
      /// [CREATION] Initialise
      case WM_INITDIALOG:
         // Extract window data
         pDialogData = (OUTPUT_DIALOG_DATA*)lParam;
         pDialogData->hDialog = hDialog;
         // Initialise
         initOutputDialog(pDialogData);
         break;

      /// [CLOSE] Block message and invoke the main window's "Show/Hide Output" command
      case WM_CLOSE:
         onOutputDialogClose(pDialogData);
         bResult = FALSE; 
         break;

      /// [COMMMAND]
      case WM_COMMAND:
         bResult = onOutputDialogCommand(pDialogData, LOWORD(wParam), HIWORD(wParam));
         break;

      /// [NOTIFICATION]
      case WM_NOTIFY:
         bResult = onOutputDialogNotify(pDialogData, wParam, (NMHDR*)lParam);
         break;

      /// [CONTEXT MENU]
      case WM_CONTEXTMENU:
         ptCursor.x = LOWORD(lParam);
         ptCursor.y = HIWORD(lParam);
         onOutputDialogContextMenu(pDialogData, (HWND)wParam, &ptCursor);
         break;

      /// [MENU ITEM HOVER] Forward messages up the chain to the Main window
      case WM_MENUSELECT:
         sendAppMessage(AW_MAIN, WM_MENUSELECT, wParam, lParam);
         break;

      /// [CUSTOM MENU]
      case WM_DRAWITEM:    bResult = onWindow_DrawItem((DRAWITEMSTRUCT*)lParam);            break;
      case WM_MEASUREITEM: bResult = onWindow_MeasureItem(hDialog, (MEASUREITEMSTRUCT*)lParam);  break;

      /// [RESIZING] Stretch ListView
      case WM_SIZE:
         siWindow.cx = LOWORD(lParam);
         siWindow.cy = HIWORD(lParam);
         onOutputDialogResize(pDialogData, &siWindow);
         break;

      /// [MOVING] Prevent the dialog being moved by dragging it's caption
      case WM_MOVING:
         bResult = onOutputDialogMoving(pDialogData, (RECT*)lParam);
         break;

      /// [NON-CLIENT PAINT]
      case WM_NCPAINT:
         onWindow_PaintNonClient(pDialogData->hDialog, (HRGN)wParam);
         break;

      /// [VISUAL STYLES] White background
      case WM_CTLCOLORSTATIC:
         bResult = (BOOL)GetStockObject(WHITE_BRUSH);
         break;

      // [UNHANDLED]
      default:
         bResult = FALSE;
         break;
      }
   }
   /// [EXCEPTION HANDLER]
   __except (generateExceptionError(GetExceptionInformation(), pException))
   {
      // [ERROR] "An unidentified and unexpected critical error has occurred in the output tool window"
      enhanceError(pException, ERROR_ID(IDS_EXCEPTION_OUTPUT_DIALOG));
      displayException(pException);
      // Return TRUE
      bResult = TRUE;
   }

   // Return result
   END_TRACKING();
   return (INT_PTR)bResult;
}

