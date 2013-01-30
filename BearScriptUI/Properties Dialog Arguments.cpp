//
// Properties Dialog Arguments.cpp : Script Arguments page of the properties dialog
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////

// ListView column IDs constants
#define       ARGUMENT_COLUMN_NAME             0
#define       ARGUMENT_COLUMN_TYPE             1
#define       ARGUMENT_COLUMN_DESCRIPTION      2

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onArgumentPage_ContextMenu
// Description     : Display the GamePage or GameString editing popup menu
// 
// SCRIPT_DOCUMENT*  pDocument : [in] Language document data
// HWND              hCtrl     : [in] Window handle of the control sending the message
// CONST POINT       ptCursor  : [in] Cursor position
// 
// Return Value   : TRUE
// 
BOOL   onArgumentPage_ContextMenu(SCRIPT_DOCUMENT*  pDocument, HWND  hCtrl, CONST POINT  ptCursor)
{
   CUSTOM_MENU*   pCustomMenu;    // Custom Popup menu
   LVHITTESTINFO  oHitTest;          // ListView hittest data
   UINT           iItem;             // ListView index of the item clicked

   // [ARGUMENT LIST] Display the editing context menu
   if (GetDlgCtrlID(hCtrl) == IDC_ARGUMENTS_LIST)
   {
      // [VERBOSE]
      VERBOSE_LIB_COMMAND();

      // Find the currently selected item (if any)
      iItem = ListView_GetNextItem(hCtrl, -1, LVNI_ALL WITH LVNI_SELECTED);
      
      /// Identify the sub-item clicked (if any)
      if (iItem != -1)
      {
         // Prepare
         oHitTest.pt = ptCursor;
         ScreenToClient(hCtrl, &oHitTest.pt);

         // Determine and store Item + SubItem
         ListView_SubItemHitTest(hCtrl, &oHitTest);
         pDocument->oLabelData.iItem    = iItem;
         pDocument->oLabelData.iSubItem = oHitTest.iSubItem;

         // Store the parent window
         pDocument->oLabelData.hParent = GetParent(hCtrl);
      }
      else
         pDocument->oLabelData.iItem = pDocument->oLabelData.iSubItem = -1;
      
      /// Create Arguments custom popup menu
      pCustomMenu = createCustomMenu(TEXT("SCRIPT_MENU"), TRUE, IDM_ARGUMENT_POPUP);

      // Disable 'Edit' and 'Remove' if user didn't click an existing argument item
      EnableMenuItem(pCustomMenu->hPopup, IDM_ARGUMENT_EDIT,   iItem != -1 ? MF_ENABLED : MF_DISABLED);
      EnableMenuItem(pCustomMenu->hPopup, IDM_ARGUMENT_DELETE, iItem != -1 ? MF_ENABLED : MF_DISABLED);

      /// Display context menu
      TrackPopupMenu(pCustomMenu->hPopup, TPM_TOPALIGN WITH TPM_LEFTALIGN, ptCursor.x, ptCursor.y, NULL, GetParent(hCtrl), NULL);

      // Cleanup
      deleteCustomMenu(pCustomMenu);
   }

   return TRUE;
}


/// Function name  : onArgumentPage_Command
// Description     : Invokes the menu item handlers
// 
// Return Value   : TRUE/FALSE
// 
BOOL  onArgumentPage_Command(SCRIPT_DOCUMENT*  pDocument, HWND  hPage, const UINT  iControl, const UINT  iNotification, HWND  hCtrl)
{
   BOOL  bResult = TRUE;

   // Examine command source
   switch (iControl)
   {
   case IDM_ARGUMENT_INSERT:
   case IDC_ADD_ARGUMENT:
      onArgumentPage_InsertArgument(pDocument, hPage);
      break;

   case IDM_ARGUMENT_EDIT:
      onArgumentPage_EditArgument(pDocument, GetDlgItem(hPage, IDC_ARGUMENTS_LIST));
      break;

   case IDM_ARGUMENT_DELETE:
   case IDC_REMOVE_ARGUMENT:
      onArgumentPage_DeleteArgument(pDocument, hPage);
      break;

   default:
      bResult = FALSE;
      break;
   }

   return bResult;
}



/// Function name  : onArgumentPage_CustomDraw
// Description     : Custom draw handler for the ListView and buttons
// 
// SCRIPT_DOCUMENT*  pDocument    : [in] 
// HWND              hPage        : [in] 
// CONST UINT        iControlID   : [in] 
// NMLVCUSTOMDRAW*   pMessageData : [in] 
// 
// Return Value   : TRUE/FALSE
// 
BOOL  onArgumentPage_CustomDraw(SCRIPT_DOCUMENT*  pDocument, HWND  hPage, CONST UINT  iControlID, NMHDR*  pHeader)
{  
   BOOL  bResult = TRUE;

   // Examine control
   switch (iControlID)
   {
   /// [BUTTONS] Add icons to each button
   case IDC_ADD_ARGUMENT:
      onCustomDrawButton(hPage, pHeader, ITS_MEDIUM, TEXT("ADD_ICON"), TRUE);
      break;

   case IDC_REMOVE_ARGUMENT:
      onCustomDrawButton(hPage, pHeader, ITS_MEDIUM, TEXT("REMOVE_ICON"), TRUE);
      break;

   /// [LISTVIEW] CustomDraw ListView
   case IDC_ARGUMENTS_LIST:
      bResult = onCustomDrawListView(hPage, pHeader->hwndFrom, (NMLVCUSTOMDRAW*)pHeader);
      SetWindowLong(hPage, DWL_MSGRESULT, bResult);
      break;

   default:
      bResult = FALSE;
      break;
   }

   // Return result
   return bResult;
}


/// Function name  : onArgumentPage_DeleteArgument
// Description     : Delete the currently selected argument ListView item and the associated ARGUMENT list item
//                    of the ScriptFile of the currently active document.
// 
// SCRIPT_DOCUMENT*  pDocument  : [in] Active document
// HWND              hDialog    : [in] Window handle of the arguments page
// 
VOID   onArgumentPage_DeleteArgument(SCRIPT_DOCUMENT*  pDocument, HWND  hDialog)
{
   INT  iItemIndex;     // Zero-based index of the currently selected argument item in the 'Arguments' ListView
   
   // [VERBOSE]
   VERBOSE_LIB_COMMAND();

   /// Delete selected ARGUMENT from ScriptFile
   removeArgumentFromScriptFileByIndex(pDocument->pScriptFile, iItemIndex = ListView_GetSelected(GetDlgItem(hDialog, IDC_ARGUMENTS_LIST)));

   /// Update ListView count
   ListView_SetItemCount(GetDlgItem(hDialog, IDC_ARGUMENTS_LIST), getScriptFileArgumentCount(pDocument->pScriptFile));

   // Unselected remaining items
   ListView_SetItemState(GetDlgItem(hDialog,IDC_ARGUMENTS_LIST), -1, NULL, LVIS_SELECTED);
   onArgumentPage_ItemChanged(pDocument, hDialog, NULL);

   // [EVENT] Notify document that a property has changed
   sendDocumentPropertyUpdated(AW_DOCUMENTS_CTRL, IDC_ARGUMENTS_LIST);
}


/// Function name  : onArgumentPage_EditArgument
// Description     : Create an appropriate control to edit the argument property specified by the 'ListView label
//                     editing data' within the specified document.
// 
// SCRIPT_DOCUMENT*  pDocument   : [in] The currently active document
// HWND              hListView   : [in] Window handle of the Arguments ListView
// 
VOID   onArgumentPage_EditArgument(SCRIPT_DOCUMENT*  pDocument, HWND  hListView)
{
   // [VERBOSE]
   VERBOSE_LIB_COMMAND();

   /// Initiate label edit on correct item/subitem
   if (pDocument->oLabelData.iSubItem == ARGUMENT_COLUMN_TYPE)
      editCustomListViewItemEx(hListView, pDocument->oLabelData.iItem, pDocument->oLabelData.iSubItem, LVLT_COMBOBOX, CBS_OWNERDRAWFIXED, compareCustomComboBoxItems);
   else
      editCustomListViewItem(hListView, pDocument->oLabelData.iItem, pDocument->oLabelData.iSubItem, LVLT_EDIT);
}


/// Function name  : onArgumentPage_EditBegin
// Description     : Populates argument property into provided Edit/ComboBox
// 
// SCRIPT_DOCUMENT*     pDocument : [in] Document
// const NMLVDISPINFO*  pHeader   : [in] Label editing data
// 
VOID  onArgumentPage_EditBegin(SCRIPT_DOCUMENT*  pDocument, NMLVDISPINFO*  pHeader)
{
   ARGUMENT*  pArgument;

   /// Lookup selected ARGUMENT
   findArgumentInScriptFileByIndex(pDocument->pScriptFile, pDocument->oLabelData.iItem, pArgument);
   ASSERT(pArgument);

   // Examine property
   switch (pDocument->oLabelData.iSubItem)
   {
   /// [NAME/DESCRIPTION] Provide property
   case ARGUMENT_COLUMN_NAME:          SetWindowText(pHeader->hdr.hwndFrom, pArgument->szName);          break;
   case ARGUMENT_COLUMN_DESCRIPTION:   SetWindowText(pHeader->hdr.hwndFrom, pArgument->szDescription);   break;

   /// [TYPE] Populate Syntax
   case ARGUMENT_COLUMN_TYPE:
      SendMessage(pHeader->hdr.hwndFrom, CB_SETDROPPEDWIDTH, 200, NULL);
      SendMessage(pHeader->hdr.hwndFrom, CB_SETMINVISIBLE,   8,   NULL);
      performParameterSyntaxComboBoxPopulation(pHeader->hdr.hwndFrom, pArgument->eType);
      break;
   }
}


/// Function name  : onArgumentPage_EditEnd
// Description     : Validate name or change type
// 
// SCRIPT_DOCUMENT*     pDocument : [in] Document
// const NMLVDISPINFO*  pString   : [in] Label editing data
// 
BOOL  onArgumentPage_EditEnd(SCRIPT_DOCUMENT*  pDocument, NMLVDISPINFO*  pHeader)
{
   ARGUMENT*  pArgument;
   TCHAR*     szNewText;
   BOOL       bValid = TRUE;

   /// Lookup selected ARGUMENT
   findArgumentInScriptFileByIndex(pDocument->pScriptFile, pDocument->oLabelData.iItem, pArgument);
   ASSERT(pArgument);

   // Examine property
   switch (pDocument->oLabelData.iSubItem)
   {
   /// [NAME/DESCRIPTION] Validate/replace property
   case ARGUMENT_COLUMN_NAME:         
   case ARGUMENT_COLUMN_DESCRIPTION:   
      // Prepare
      szNewText = utilGetWindowText(pHeader->hdr.hwndFrom);

      // [DESCRIPTION] Replace without validation
      if (pDocument->oLabelData.iSubItem == ARGUMENT_COLUMN_DESCRIPTION)
         utilReplaceString(pArgument->szDescription, szNewText);
      
      // [NAME] Validate before updating
      else if (bValid = verifyScriptFileArgumentName(pDocument->pScriptFile, szNewText, pArgument))
         utilReplaceString(pArgument->szName, szNewText);
      else
         // [FAILED] Produce a warning BEEP
         MessageBeep(MB_ICONWARNING);

      if (bValid)
      {  // [EVENT] Notify document that a property has changed and destroy control
         sendDocumentPropertyUpdated(AW_DOCUMENTS_CTRL, IDC_ARGUMENTS_LIST);
         SetFocus(GetParent(pHeader->hdr.hwndFrom));
      }

      // Cleanup
      utilDeleteString(szNewText);
      break;

   /// [TYPE] Update syntax
   case ARGUMENT_COLUMN_TYPE:
      pArgument->eType = (PARAMETER_SYNTAX)getCustomComboBoxItemParam(pHeader->hdr.hwndFrom, ComboBox_GetCurSel(pHeader->hdr.hwndFrom));
      break;
   }

   // Return validation result
   return bValid;
}


/// Function name  : onArgumentPage_InsertArgument
// Description     : Display the 'Insert Argument' dialog and (if the user creates an argument) add the argument
//                    to the document's ScriptFile and the document properties dialog Argument ListView
// 
// SCRIPT_DOCUMENT*  pDocument   : [in] Active document
// HWND              hDialog     : [in] Window handle of the Arguments property page
// 
VOID   onArgumentPage_InsertArgument(SCRIPT_DOCUMENT*  pDocument, HWND  hDialog)
{
   // [VERBOSE]
   VERBOSE_LIB_COMMAND();

   /// Display 'Insert Argument' dialog
   if (displayInsertArgumentDialog(pDocument->pScriptFile, getAppWindow()))
   {
      // [NEW ARGUMENT] Update ListView item count
      ListView_SetItemCount(GetDlgItem(hDialog, IDC_ARGUMENTS_LIST), getScriptFileArgumentCount(pDocument->pScriptFile));

      // [EVENT] Notify document that a property has changed
      sendDocumentPropertyUpdated(AW_DOCUMENTS_CTRL, IDC_ARGUMENTS_LIST);
   }
}


/// Function name  : onArgumentPage_ItemChanged
// Description     : Enable/Disable the 'Remove Argument' button based on whether an argument is selected
// 
// SCRIPT_DOCUMENT*  pActiveDocument : [in] Active Document
// HWND              hDialog         : [in] Arguments page dialog handle
// NMLISTVIEW*       pItemData       : [in] WM_NOTIFY data
// 
// Return Value   : TRUE
// 
BOOL   onArgumentPage_ItemChanged(SCRIPT_DOCUMENT*  pActiveDocument, HWND  hDialog, NMLISTVIEW*  pItemData)
{
   // Enable/Disable 'Remove Argument' button based on whether an argument is selected
   utilEnableDlgItem(hDialog, IDC_REMOVE_ARGUMENT, ListView_GetSelectedCount(GetDlgItem(hDialog, IDC_ARGUMENTS_LIST)) > 0);
   return TRUE;
}


/// Function name  :  onArgumentPage_Notify
// Description     : 
// 
// SCRIPT_DOCUMENT*  pDocument : [in] 
// HWND              hDialog   : [in] 
// NMHDR*            pMessage  : [in] 
// 
// Return Value   : 
// 
BOOL   onArgumentPage_Notify(SCRIPT_DOCUMENT*  pDocument, HWND  hDialog, NMHDR*  pMessage)
{
   BOOL  bResult = FALSE;

   // Examine notification
   switch (pMessage->code)
   {
   /// [LISTVIEW: REQUEST DATA] 
   case LVN_GETDISPINFO:
      bResult = onArgumentPage_RequestData(pDocument, hDialog, (NMLVDISPINFO*)pMessage);
      break;

   /// [LISTVIEW: EDIT ARGUMENT (begin)]
   case LVN_BEGINLABELEDIT:
      onArgumentPage_EditBegin(pDocument, (NMLVDISPINFO*)pMessage);
      break;

   /// [LISTVIEW: EDIT ARGUMENT (end)]
   case LVN_ENDLABELEDIT:
      bResult = onArgumentPage_EditEnd(pDocument, (NMLVDISPINFO*)pMessage);
      SetWindowLong(hDialog, DWL_MSGRESULT, bResult);
      break;

   /// [LISTVIEW/BUTTON: CUSTOM DRAW]
   case NM_CUSTOMDRAW:
      bResult = onArgumentPage_CustomDraw(pDocument, hDialog, pMessage->idFrom, pMessage);
      break;

   /// [LISTVIEW: SELECTION CHANGED] Enable/Disable 'Remove' button
   case LVN_ITEMCHANGED:
      bResult = onArgumentPage_ItemChanged(pDocument, hDialog, (NMLISTVIEW*)pMessage);
      break;
   }

   return bResult;
}

/// Function name  : onArgumentPage_RequestData
// Description     : Supply the requested piece of argument data for the ListView (sub)item
// 
// CONST SCRIPT_DOCUMENT*  pDocument    : [in]     Document data
// HWND                    hDialog      : [in]     Arguments property page window handle
// NMLVDISPINFO*           pMessageData : [in/out] Indicates what data is required and stores it
// 
// Return Type     : TRUE
//
BOOL   onArgumentPage_RequestData(CONST SCRIPT_DOCUMENT*  pDocument, HWND  hDialog, NMLVDISPINFO*  pMessageData)
{
   LVITEM&        oOutput = pMessageData->item;      // Convenience pointer for the item output data
   GAME_STRING*   pGameString;   // GameString used for argument type lookup
   ARGUMENT*      pArgument;     // Argument associated with the selected item

   // [CHECK] Active document exists
   if (pDocument == NULL)
      return TRUE;

   // Prepare
   pArgument = NULL;

   // Find ARGUMENT associated with item
   if (findArgumentInScriptFileByIndex(pDocument->pScriptFile, oOutput.iItem, pArgument))
   {
      // Determine which property is required
      switch (oOutput.iSubItem)
      {
      /// [NAME]
      case ARGUMENT_COLUMN_NAME:
         if (oOutput.mask INCLUDES LVIF_TEXT)
            StringCchCopy(oOutput.pszText, oOutput.cchTextMax, pArgument->szName);
         // Set anything else requested to NULL
         if (oOutput.mask INCLUDES LVIF_INDENT)
            oOutput.iIndent = NULL;
         if (oOutput.mask INCLUDES LVIF_IMAGE)
            oOutput.iImage = getAppImageTreeIconIndex(ITS_SMALL, TEXT("ARGUMENT_ICON"));
         break;

      /// [DESCRIPTION]
      case ARGUMENT_COLUMN_DESCRIPTION:
         if (oOutput.mask INCLUDES LVIF_TEXT)
            StringCchCopy(oOutput.pszText, oOutput.cchTextMax, pArgument->szDescription);
         break;

      /// [TYPE]
      case ARGUMENT_COLUMN_TYPE:
         if (oOutput.mask INCLUDES LVIF_TEXT)
         {
            findGameStringByID((UINT)pArgument->eType, identifyGamePageIDFromDataType(DT_SCRIPTDEF), pGameString);
            ASSERT(pGameString);
            StringCchCopy(oOutput.pszText, oOutput.cchTextMax, pGameString->szText);
         }
         break;
      }
   }
   // [FAILED] Error!
   else if (oOutput.mask INCLUDES LVIF_TEXT)
      StringCchPrintf(oOutput.pszText, oOutput.cchTextMax, TEXT("Error: Item %d Not found"), oOutput.iItem);
   else
      oOutput.pszText = TEXT("ERROR: Item not found");

   return TRUE;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       DIALOG PROCEDURES
/// ////////////////////////////////////////////////////////////////////////////////////////////////////


/// Function name  : dlgprocArgumentPage
// Description     : Window procedure for the 'Arguments' properties dialog page
//
// 
INT_PTR   dlgprocArgumentPage(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   PROPERTIES_DATA*  pDialogData;
   POINT             ptCursor;
   BOOL              bResult;

   // Get dialog data
   pDialogData = getPropertiesDialogData(hDialog);

   // Examine message
   switch (iMessage)
   {
   /// [CONTEXT MENU] - Display arguments list context menu
   case WM_CONTEXTMENU:
      ptCursor.x = LOWORD(lParam);
      ptCursor.y = HIWORD(lParam);
      onArgumentPage_ContextMenu(pDialogData->pScriptDocument, (HWND)wParam, ptCursor);
      return TRUE;

   /// [COMMANDS] - Process Arguments List menu items and button commands
   case WM_COMMAND:
      if (bResult = onArgumentPage_Command(pDialogData->pScriptDocument, hDialog, LOWORD(wParam), HIWORD(wParam), (HWND)lParam))
         return bResult;
      break;

   /// [NOTIFICATION] - Handle ListView notifications
   case WM_NOTIFY:
      if (bResult = onArgumentPage_Notify(pDialogData->pScriptDocument, hDialog, (NMHDR*)lParam))
         return bResult;
      break;

   }

   // Pass to base proc
   return dlgprocPropertiesPage(hDialog, iMessage, wParam, lParam, PP_SCRIPT_ARGUMENTS);
}


