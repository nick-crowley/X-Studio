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


BOOL  onArgumentsPageCustomDrawTooltip(HWND  hDialog, NMTTCUSTOMDRAW*  pHeader)
{
   /*if (pNotifyHeader->hwndFrom == ListView_GetToolTips(GetControl(hDialog, IDC_ARGUMENTS_LIST)))
            return onArgumentsPageCustomDrawTooltip(hDialog, (NMTTCUSTOMDRAW*)lParam);
         else*/
   switch (pHeader->nmcd.dwDrawStage)
   {
   case CDDS_PREPAINT:
      SetWindowLong(hDialog, DWL_MSGRESULT, CDRF_DODEFAULT);
      break;
   }
   return TRUE;
}

/// Function name  : onArgumentPage_CustomDraw
// Description     : Custom draw handler for the ListView and buttons
// 
// SCRIPT_DOCUMENT*  pDocument    : [in] 
// HWND              hPage        : [in] 
// CONST UINT        iControlID   : [in] 
// NMLVCUSTOMDRAW*   pMessageData : [in] 
// 
// Return Value   : 
// 
BOOL  onArgumentPage_CustomDraw(SCRIPT_DOCUMENT*  pDocument, HWND  hPage, CONST UINT  iControlID, NMHDR*  pMessageHeader)
{  
   // Examine control
   switch (iControlID)
   {
   /// [BUTTONS] Add icons to each button
   case IDC_ADD_ARGUMENT:
      onCustomDrawButton(hPage, pMessageHeader, ITS_MEDIUM, TEXT("ADD_ICON"), TRUE);
      return TRUE;

   case IDC_REMOVE_ARGUMENT:
      onCustomDrawButton(hPage, pMessageHeader, ITS_MEDIUM, TEXT("REMOVE_ICON"), TRUE);
      return TRUE;

   case IDC_ARGUMENTS_LIST:
      onCustomListViewNotify(hPage, TRUE, IDC_ARGUMENTS_LIST, pMessageHeader);
      return TRUE;
   }

   

   // [NOT HANDLED] Return FALSE
   return FALSE;
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

   // Get index of the currently selected item
   iItemIndex = SendDlgItemMessage(hDialog, IDC_ARGUMENTS_LIST, LVM_GETNEXTITEM, -1, LVNI_ALL WITH LVNI_SELECTED);

   /// Delete from active document's ScriptFile
   removeArgumentFromScriptFileByIndex(pDocument->pScriptFile, iItemIndex);

   /// Update ListView count
   SendDlgItemMessage(hDialog, IDC_ARGUMENTS_LIST, LVM_SETITEMCOUNT, getScriptFileArgumentCount(pDocument->pScriptFile), NULL);

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
   LISTVIEW_LABEL_DATA&  oLabelData = pDocument->oLabelData;     // Convenience reference for the 'ListView label editing data'
   ARGUMENT*             pArgument;       // Argument list iterator
   RECT                  rcSubItemRect;   // Sub-item rectangle
   SIZE                  siSubItemSize;   // Size of the sub-item rectangle
   
   // [VERBOSE]
   VERBOSE_LIB_COMMAND();

   /// Get the bounding rectangle for the element to be edited
   if (oLabelData.iSubItem == 0)
   {
      // [ITEM] Get 'label' rect
      rcSubItemRect.left = LVIR_LABEL;
      SendMessage(hListView, LVM_GETITEMRECT, oLabelData.iItem, (LPARAM)&rcSubItemRect);
   }
   else
   {
      // [SUB-ITEM] Get 'sub-item' rect
      rcSubItemRect.top  = oLabelData.iSubItem;
      rcSubItemRect.left = LVIR_LABEL;
      SendMessage(hListView, LVM_GETSUBITEMRECT, oLabelData.iItem, (LPARAM)&rcSubItemRect);
   }

   /// Find associated ARGUMENT
   findArgumentInScriptFileByIndex(pDocument->pScriptFile, oLabelData.iItem, pArgument);
   ASSERT(pArgument);

   // Store SCRIPT-FILE in LabelData
   oLabelData.pParameter = (LPARAM)pDocument->pScriptFile;
   
   // Create appropriate control for editing the specified sub-item
   switch (oLabelData.iSubItem)
   {
   /// [NAME/DESCRIPTION] Create Edit control
   case ARGUMENT_COLUMN_NAME:
   case ARGUMENT_COLUMN_DESCRIPTION:
      // Create sub-classed Edit control
      oLabelData.hCtrl = CreateWindowEx(NULL, szArgumentsLabelEditClass, NULL, WS_CHILD WITH WS_BORDER WITH ES_WANTRETURN WITH ES_AUTOHSCROLL, 0, 0, CW_USEDEFAULT, CW_USEDEFAULT, hListView, (HMENU)IDC_INPLACE_ARGUMENT_EDIT, getAppInstance(), (VOID*)&pDocument->oLabelData);

      // Display NAME/DESCRIPTION and move caret to the final character
      SetWindowText(oLabelData.hCtrl, (oLabelData.iSubItem == ARGUMENT_COLUMN_NAME ? pArgument->szName : pArgument->szDescription));
      SendMessage(oLabelData.hCtrl, EM_SETSEL, GetWindowTextLength(oLabelData.hCtrl), GetWindowTextLength(oLabelData.hCtrl));
      break;

   /// [TYPE] Create ComboBox
   case ARGUMENT_COLUMN_TYPE:
      // Create sub-classed ComboBox
      oLabelData.hCtrl = CreateWindowEx(NULL, szArgumentsLabelComboClass, NULL, WS_CHILD WITH WS_BORDER WITH WS_VSCROLL WITH CBS_DROPDOWNLIST WITH CBS_SORT, 0, 0, CW_USEDEFAULT, CW_USEDEFAULT, hListView, (HMENU)IDC_INPLACE_ARGUMENT_COMBO, getAppInstance(), (VOID*)&pDocument->oLabelData);
      
      // Adjust ComboBox size, Populate with 'Argument Types' and select ARGUMENT TYPE
      SendMessage(oLabelData.hCtrl, CB_SETDROPPEDWIDTH, 200, NULL);
      SendMessage(oLabelData.hCtrl, CB_SETMINVISIBLE, 8, NULL);
      performParameterSyntaxComboBoxPopulation(hListView, IDC_INPLACE_ARGUMENT_COMBO, pArgument->eType);
      break;
   }

   /// Reposition control
   utilConvertRectangleToSize(&rcSubItemRect, &siSubItemSize);
   siSubItemSize.cy += 4;
   MoveWindow(oLabelData.hCtrl, rcSubItemRect.left, rcSubItemRect.top, siSubItemSize.cx, siSubItemSize.cy, TRUE);
   SetWindowPos(oLabelData.hCtrl, HWND_TOPMOST, rcSubItemRect.left, rcSubItemRect.top, siSubItemSize.cx, siSubItemSize.cy, SWP_NOMOVE WITH SWP_NOSIZE);

   /// Assign normal font, show control and set focus
   SetWindowFont(oLabelData.hCtrl, GetStockObject(ANSI_VAR_FONT), FALSE);
   ShowWindow(oLabelData.hCtrl, SW_SHOW);
   SetFocus(oLabelData.hCtrl);
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
      SendDlgItemMessage(hDialog, IDC_ARGUMENTS_LIST, LVM_SETITEMCOUNT, getScriptFileArgumentCount(pDocument->pScriptFile), NULL);

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
   UINT  iSelectedCount;      // Number of items currently selected in the 'Arguments' ListView

   // Enable/Disable 'Remove Argument' button based on whether an argument is selected
   iSelectedCount = SendDlgItemMessage(hDialog, IDC_ARGUMENTS_LIST, LVM_GETSELECTEDCOUNT, NULL, NULL);
   utilEnableDlgItem(hDialog, IDC_REMOVE_ARGUMENT, (iSelectedCount != 0));

   return TRUE;
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
   NMHDR*            pNotifyHeader;
   POINT             ptCursor;

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
      // Examine command source
      switch (LOWORD(wParam))
      {
      case IDM_ARGUMENT_INSERT:
      case IDC_ADD_ARGUMENT:
         onArgumentPage_InsertArgument(pDialogData->pScriptDocument, hDialog);
         return TRUE;

      case IDM_ARGUMENT_EDIT:
         onArgumentPage_EditArgument(pDialogData->pScriptDocument, GetDlgItem(hDialog, IDC_ARGUMENTS_LIST));
         return TRUE;

      case IDM_ARGUMENT_DELETE:
      case IDC_REMOVE_ARGUMENT:
         onArgumentPage_DeleteArgument(pDialogData->pScriptDocument, hDialog);
         return TRUE;
      }
      break;

   /// [NOTIFICATION] - Handle ListView notifications
   case WM_NOTIFY:
      pNotifyHeader = (NMHDR*)lParam;

      // Examine notification
      switch (pNotifyHeader->code)
      {
      /// [REQUEST ARGUMENTS DATA] Supply ARGUMENT data
      case LVN_GETDISPINFO:
         return onArgumentPage_RequestData(pDialogData->pScriptDocument, hDialog, (NMLVDISPINFO*)lParam);

      /// [IN-PLACE LABEL EDITING] Block system's in place editing 
      case LVN_BEGINLABELEDIT:
         SetWindowLong(hDialog, DWL_MSGRESULT, TRUE);
         return TRUE;

      /// [CUSTOM DRAW BUTTONS]
      case NM_CUSTOMDRAW:
         return onArgumentPage_CustomDraw(pDialogData->pScriptDocument, hDialog, wParam, (NMHDR*)lParam);

      /// [SELECTION CHANGED] Enable/Disable 'Remove' button
      case LVN_ITEMCHANGED:
         return onArgumentPage_ItemChanged(pDialogData->pScriptDocument, hDialog, (NMLISTVIEW*)lParam);
      }
      break;
   }

   // Pass to base proc
   return dlgprocPropertiesPage(hDialog, iMessage, wParam, lParam, PP_SCRIPT_ARGUMENTS);
}


/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       WINDOW PROCEDURES
/// ////////////////////////////////////////////////////////////////////////////////////////////////////


/// Function name  : wndprocArgumentPageComboCtrl
// Description     : Arguments ComboBox -- Subclassed during 'in-place' label editing
// 
// 
LRESULT   wndprocArgumentPageComboCtrl(HWND  hCtrl, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   static LISTVIEW_LABEL_DATA*  pLabelData = NULL;    // Label Data
   CREATESTRUCT*                pCreationData;        // WM_CREATE message data
   ARGUMENT*                    pArgument;            // Argument being edited
   WNDCLASS                     oClass;               // Base EditCtrl window class
   INT                          iNewSelection;        // New item index
   
   // Prepare
   GetClassInfo(getAppInstance(), WC_COMBOBOX, &oClass);
   
   // Examine message
   switch (iMessage)
   {
   /// [CREATE] Store creation data and pass to the base window
   case WM_CREATE:
      pCreationData = (CREATESTRUCT*)lParam;
      pLabelData    = (LISTVIEW_LABEL_DATA*)pCreationData->lpCreateParams;

      // [CHECK] Ensure ScriptFile has been passed as a parameter
      ASSERT(pLabelData->pParameter);

      // Continue to base class
      break;

   /// [DESTROY WINDOW] Zero label data and pass to the base window
   case WM_DESTROY:
      pLabelData = NULL;
      break;

   // [COMMAND]
   case WM_COMMAND:
      switch (HIWORD(wParam))
      {
      /// [SELECTION CHANGED] Update ARGUMENT TYPE
      case CBN_SELCHANGE:
         // Determine new selection
         iNewSelection = SendMessage(hCtrl, CB_GETCURSEL, NULL, NULL);

         // Find associated ARGUMENT
         if (findArgumentInScriptFileByIndex((SCRIPT_FILE*)pLabelData->pParameter, pLabelData->iItem, pArgument))
            // Extract PARAMETER_SYNTAX from item data
            pArgument->eType = (PARAMETER_SYNTAX)SendMessage(hCtrl, CB_GETITEMDATA, iNewSelection, NULL);
         break;
      }
      break;

   /// [ESCAPE KEY] Cancel editing without updating ARGUMENT
   case WM_KEYDOWN:
      if (wParam == VK_ESCAPE)
      {
         SetFocus(GetParent(hCtrl));
         return 0;
      }
      break;

   /// [LOST FOCUS] Destroy without updating ARGUMENT
   case WM_KILLFOCUS:
      DestroyWindow(hCtrl);
      return 0;
   }

   /// [UNHANDLED] Pass to ComboBox base class
   return CallWindowProc(oClass.lpfnWndProc, hCtrl, iMessage, wParam, lParam);
}


/// Function name  : wndprocArgumentPageEditCtrl
// Description     : Arguments Edit -- Subclassed EditControl used for 'in-place' label editing of NAME and DESCRIPTION
///                           LabelData : 'iItem'     : index of the ScriptFile Argument being edited
///                                     : 'iSubItem'  : index of the property being edited
///                                     : 'pParameter': ScriptFile containing the Argument
// 
LRESULT   wndprocArgumentPageEditCtrl(HWND  hCtrl, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   static LISTVIEW_LABEL_DATA*  pLabelData = NULL;    // Label Data
   CREATESTRUCT*                pCreationData;        // WM_CREATE message data
   ARGUMENT*                    pArgument;            // Argument being edited
   WNDCLASS                     oClass;               // Base EditCtrl window class
   TCHAR*                       szNewText;            // Current window text
   
   // Prepare
   GetClassInfo(getAppInstance(), WC_EDIT, &oClass);
   
   // Examine message
   switch (iMessage)
   {
   /// [CREATE] Store creation data and pass to the base window
   case WM_CREATE:
      pCreationData = (CREATESTRUCT*)lParam;
      pLabelData    = (LISTVIEW_LABEL_DATA*)pCreationData->lpCreateParams;

      // [CHECK] Ensure ScriptFile has been passed as a parameter
      ASSERT(pLabelData->pParameter);

      // Continue to base class
      break;

   /// [DESTROY WINDOW] Zero label data and pass to the base window
   case WM_DESTROY:
      pLabelData = NULL;
      break;

   /// [KEYBOARD QUERY] Request all keyboard input
   case WM_GETDLGCODE:
      return DLGC_WANTALLKEYS;

   // [KEY PRESS] Trap ENTER and ESCAPE keys
   case WM_KEYDOWN:
      // Examine key
      switch (wParam)
      {
      /// [RETURN] Update NAME or DESCRIPTION
      case VK_RETURN:
         // Extract associated ARGUMENT from ScriptFile
         if (findArgumentInScriptFileByIndex((SCRIPT_FILE*)pLabelData->pParameter, pLabelData->iItem, pArgument))
         {
            // Prepare
            szNewText = utilGetWindowText(hCtrl);

            // Examine propery being edited
            switch (pLabelData->iSubItem)
            {
            /// [NAME] Update ARGUMENT name only if validation is successful
            case ARGUMENT_COLUMN_NAME:
               // [CHECK] Is the ARGUMENT name unique?
               if (verifyScriptFileArgumentName((SCRIPT_FILE*)pLabelData->pParameter, szNewText, pArgument))
               {
                  /// [SUCCESS] Update NAME
                  pArgument->szName = utilReplaceString(pArgument->szName, szNewText);

                  // [EVENT] Notify document that a property has changed and destroy control
                  sendDocumentPropertyUpdated(AW_DOCUMENTS_CTRL, IDC_ARGUMENTS_LIST);
                  SetFocus(GetParent(hCtrl));
               }
               /// [FAILED] Produce a warning BEEP
               else
                  MessageBeep(MB_ICONWARNING);
               break;

            /// [DESCRIPTION] Update ARGUMENT description and destroy control
            case ARGUMENT_COLUMN_DESCRIPTION:
               pArgument->szDescription = utilReplaceString(pArgument->szDescription, szNewText);

               // [EVENT] Notify document that a property has changed and destroy control
               sendDocumentPropertyUpdated(AW_DOCUMENTS_CTRL, IDC_ARGUMENTS_LIST);
               SetFocus(GetParent(hCtrl));
               break;
            }

            // Cleanup
            utilDeleteString(szNewText);
         }
         return 0;
      
      /// [ESCAPE] Cancel editing without updating ARGUMENT or notifying the document that a property has changed
      case VK_ESCAPE:
         SetFocus(GetParent(hCtrl));
         return 0;
      }
      break;

   /// [LOST FOCUS] Destroy window
   case WM_KILLFOCUS:
      DestroyWindow(hCtrl);
      return 0;
   }

   /// [UNHANDLED] Pass to edit base window
   return CallWindowProc(oClass.lpfnWndProc, hCtrl, iMessage, wParam, lParam);
}

