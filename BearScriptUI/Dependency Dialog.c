//
// Dependency Dialog.cpp : FILE_DESCRIPTION
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        MACROS
/// /////////////////////////////////////////////////////////////////////////////////////////

// OnException: Display in output
#define  ON_EXCEPTION()    displayException(pException);

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////

// ScriptDependencies ListView column IDs
#define       DEPENDENCIES_COLUMN_NAME         0
#define       DEPENDENCIES_COLUMN_CALLS        1

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : displayDependencyDialog
// Description     : Displays the ScriptCall dialog
// 
// HWND       hParent      : [in] Parent window
// AVL_TREE*  pCallersTree : [in] ScriptDependencies tree
// 
VOID  displayDependencyDialog(HWND  hParent, SCRIPT_OPERATION*  pOperation)
{
   DEPENDENCY_DIALOG_DATA  oDialogData;

   TRY
   {
      // [TRACK]
      CONSOLE_COMMAND();
      debugScriptDependencyTree(pOperation->pCallersTree);

      // Prepare
      oDialogData.pResultsTree = pOperation->pCallersTree;
      oDialogData.szContent    = pOperation->szContent;
      oDialogData.szFolder     = pOperation->szFolder;
      oDialogData.eType        = pOperation->eContent;

      /// Display dialog
      showDialog(TEXT("SCRIPTCALL_DIALOG"), utilEither(hParent, utilGetTopWindow(getAppWindow())), dlgprocDependencyDialog, (LPARAM)&oDialogData);
   }
   CATCH0("");
}


/// Function name  : initDependencyDialog
// Description     : Initialises the dialog and controls
// 
// HWND       hDialog      : [in] ScriptCall dialog
// AVL_TREE*  pCallersTree : [in] ScriptDependencies tree
// 
// Return Value   : TRUE
// 
BOOL  initDependencyDialog(DEPENDENCY_DIALOG_DATA*  pDialogData, HWND  hDialog)
{
   LISTVIEW_COLUMNS   oDepedenciesListView = { 2, IDS_DEPENDENCIES_COLUMN_NAME, 240, 90,  NULL, NULL, NULL };    // ListView column data
   const TCHAR*       szFormat;
   LVCOLUMN           oColumn;
   HWND               hListView = GetControl(hDialog, IDC_DEPENDENCIES_LIST);

   TRY
   {
      /// Store dialog data
      SetWindowLong(hDialog, DWL_USER, (LONG)pDialogData);
      pDialogData->hDialog = hDialog;

      // Setup ListView
      initReportModeListView(hListView, &oDepedenciesListView, FALSE);
      SubclassWindow(hListView, wndprocCustomListView);
      ListView_SetBkColor(hListView, getThemeSysColour(TEXT("TAB"), COLOR_WINDOW));
      ListView_SetItemCount(hListView, getTreeNodeCount(pDialogData->pResultsTree));

      // Lookup description
      switch (pDialogData->eType)
      {
      case CT_SYNTAX:   szFormat = TEXT("The following scripts in '%s' use the command '%s'");              break;
      case CT_SCRIPT:   szFormat = TEXT("The following scripts in '%s' are dependent upon '%s'");           break;
      case CT_OBJECT:   szFormat = TEXT("The following scripts in '%s' use the object or constant '%s'");   break;
      }
      
      // Set description
      utilSetWindowTextf(GetControl(hDialog, IDC_SCRIPTCALL_STATIC), szFormat, pDialogData->szFolder, pDialogData->szContent); 

      // Create 'Line Number' column
      if (pDialogData->eType != CT_SCRIPT)
      {
         oColumn.mask = LVCF_TEXT;  oColumn.pszText = TEXT("Line");
         ListView_SetColumn(hListView, 1, &oColumn);
      }

      // [DIALOG] Centre dialog and set caption
      SetWindowText(hDialog, getAppName());
      utilCentreWindow(getAppWindow(), hDialog);

      // [OWNER DRAW]
      utilAddWindowStyle(GetDlgItem(hDialog, IDC_DIALOG_TITLE), SS_OWNERDRAW); 
      utilAddWindowStyle(GetDlgItem(hDialog, IDC_SCRIPTCALL_STATIC), SS_OWNERDRAW); 

      // Return TRUE
      return TRUE;
   }
   CATCH0("");
   return TRUE;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onDependenciesLoadSelectedScripts
// Description     : Loads the selected script dependencies
// 
// PROPERTIES_DATA*  pSheetData   : [in] Properties sheet dialog data
// HWND              hPage        : [in] Window handle of the dependencies page
// 
VOID   onDependencyDialog_LoadSelectedScripts(DEPENDENCY_DIALOG_DATA*  pDialogData)
{
   SCRIPT_DEPENDENCY*   pDependency;      // ScriptDependency associated with the selected item
   LOADING_OPTIONS      oOptions;         // Used for highlighting caller
   HWND                 hListView;        // Dependencies ListView
   INT                  iSelected;        // Index of the selected item
   
   TRY
   {
      /// [CHECK] Are any dependencies selected?
      if (ListView_GetSelectedCount(hListView = GetDlgItem(pDialogData->hDialog, IDC_DEPENDENCIES_LIST)) == 0)
         return;
      
      // [VERBOSE]
      CONSOLE_COMMAND_BOLD();

      // Prepare
      utilZeroObject(&oOptions, LOADING_OPTIONS);

      // [DEBUG] Print Dependencies to be loaded
      for (iSelected = ListView_GetSelected(hListView); findObjectInAVLTreeByIndex(pDialogData->pResultsTree, iSelected, (LPARAM&)pDependency); iSelected = ListView_GetNextItem(hListView, iSelected, LVNI_SELECTED))
         debugScriptDependency(pDependency);
      
      // Iterate through selected Dependencies
      for (iSelected = ListView_GetSelected(hListView); findObjectInAVLTreeByIndex(pDialogData->pResultsTree, iSelected, (LPARAM&)pDependency); iSelected = ListView_GetNextItem(hListView, iSelected, LVNI_SELECTED))
      {
         // Highlight line
         oOptions.bHighlight = TRUE;
         oOptions.iLineNumber = pDependency->iLine;
      
         /// Attempt to open .pck / .xml version of script   (Activate final document)
         if (!commandLoadScriptDependency(getMainWindowData(), pDialogData->szFolder, pDependency, ListView_GetNextItem(hListView, iSelected, LVNI_SELECTED) == -1, &oOptions)) 
            // [ERROR] "The script dependency '%s' could not be found in '%s'"
            displayMessageDialogf(pDialogData->hDialog, IDS_GENERAL_DEPENDENCY_NOT_FOUND, MDF_OK WITH MDF_ERROR, pDependency->szScriptName, pDialogData->szFolder);
      }
   }
   CATCH0("");
}


/// Function name  : onDependencyDialog_Command
// Description     : Handles closing the dialog and loading dependencies
// 
// HWND        hDialog       : [in] ScriptCall dialog
// CONST UINT  iControlID    : [in] ID of the control
// AVL_TREE*   pCallersTree  : [in] ScriptDependencies tree
// 
// Return Value   : TRUE if processed, FALSE otherwise
// 
BOOL  onDependencyDialog_Command(DEPENDENCY_DIALOG_DATA*  pDialogData, CONST UINT  iControlID, HWND  hCtrl)
{
   BOOL  bResult = FALSE;

   // Examine source
   switch (iControlID)
   {
   /// [OK/LOAD] Load selected scripts
   case IDOK:
   case IDM_DEPENDENCIES_LOAD:
      onDependencyDialog_LoadSelectedScripts(pDialogData);
      // Fall through...

   /// [CANCEL] Close dialog
   case IDCANCEL:
      EndDialog(pDialogData->hDialog, iControlID);
      bResult = TRUE;
      break;
   }

   // Return result
   return bResult;
}


/// Function name  : onDependencyDialog_ContextMenu
// Description     : Displays the ListView context menu
// 
// HWND         hDialog   : [in] ScriptCall Dialog
// HWND         hListView : [in] ListView
// CONST POINT  ptCursor  : [in] Cursor position, in screen co-ordinates
// 
VOID  onDependencyDialog_ContextMenu(DEPENDENCY_DIALOG_DATA*  pDialogData, HWND  hListView, CONST POINT*  ptCursor)
{
   CUSTOM_MENU*  pCustomMenu;    // Custom Popup menu

   TRY
   {
      // Create Dependencies Custom Popup menu
      pCustomMenu = createCustomMenu(TEXT("SCRIPT_MENU"), TRUE, IDM_DEPENDENCIES_POPUP);

      // Enable 'Load script' only if items are selected.
      EnableMenuItem(pCustomMenu->hPopup, IDM_DEPENDENCIES_LOAD, (ListView_GetSelectedCount(hListView) > 0 ? MF_ENABLED : MF_DISABLED));

      // Disable 'Refresh' always
      EnableMenuItem(pCustomMenu->hPopup, IDM_DEPENDENCIES_REFRESH, MF_DISABLED);

      /// Display context menu
      TrackPopupMenu(pCustomMenu->hPopup, TPM_TOPALIGN WITH TPM_LEFTALIGN, ptCursor->x, ptCursor->y, NULL, pDialogData->hDialog, NULL);

      // Cleanup and return TRUE
      deleteCustomMenu(pCustomMenu);
   }
   CATCH0("");
}


/// Function name  : onDependencyDialog_ColumnClick
// Description     : Sorts the callers tree
// 
// HWND         hDialog      : [in] ScriptCall dialog
// AVL_TREE*    pCallersTree : [in] ScriptDependency tree
// NMLISTVIEW*  pHeader      : [in] Message header
// 
VOID  onDependencyDialog_ColumnClick(DEPENDENCY_DIALOG_DATA*  pDialogData, NMLISTVIEW*  pHeader)
{
   AVL_TREE_ORDERING  eNewOrder;
   AVL_TREE*          pNewTree;

   TRY
   {
      // Calculate opposite sort order
      eNewOrder = (pDialogData->pResultsTree->pSortKeys[DEPENDENCIES_COLUMN_NAME]->eDirection == AO_DESCENDING ? AO_ASCENDING : AO_DESCENDING);

      // [CHECK] Ensure user clicked the 'scriptname' column
      if (pHeader->iSubItem == DEPENDENCIES_COLUMN_NAME)
      {
         /// Copy results into new tree
         pNewTree = duplicateAVLTree(pDialogData->pResultsTree, createAVLTreeSortKey(AK_TEXT, eNewOrder), NULL, NULL);

         // Replace existing tree without destroying data
         pDialogData->pResultsTree->pfnDeleteNode = NULL;
         deleteAVLTree(pDialogData->pResultsTree);
         pDialogData->pResultsTree = pNewTree;

         /// Update ListView
         ListView_SetItemCount(pHeader->hdr.hwndFrom, getTreeNodeCount(pNewTree));
      }
   }
   CATCH0("");
}


/// Function name  : onDependencyDialog_Destroy
// Description     : Destroys the ScriptDependencies tree
// 
// HWND       hDialog      : [in] ScriptCall dialog
// AVL_TREE*  pCallersTree : [in] ScriptDependencies tree
// 
VOID  onDependencyDialog_Destroy(DEPENDENCY_DIALOG_DATA*  pDialogData)
{
   /// Delete data tree
   SetWindowLong(pDialogData->hDialog, DWL_USER, NULL);
}


VOID  onDependencyDialog_OwnerDraw(DEPENDENCY_DIALOG_DATA*  pDialogData, DRAWITEMSTRUCT*  pDrawData)
{
   TCHAR*  szText = utilGetDlgItemText(pDialogData->hDialog, IDC_SCRIPTCALL_STATIC);
   DrawText(pDrawData->hDC, szText, lstrlen(szText), &pDrawData->rcItem, DT_LEFT | DT_END_ELLIPSIS | DT_WORDBREAK);
   utilDeleteString(szText);
}


/// Function name  : onDependencyDialog_RequestData
// Description     : Supply the requested piece of ScriptDependencies data for the ListView (sub)item
// 
// PROPERTIES_DATA*  pSheetData   : [in]     Properties sheet dialog data
// AVL_TREE*         pCallersTree : [in]     Window handle of the dependencies page
// NMLVDISPINFO*     pMessageData : [in/out] Indicates what data is required and stores it
//
VOID   onDependencyDialog_RequestData(DEPENDENCY_DIALOG_DATA*  pDialogData, NMLVDISPINFO*  pMessageData)
{
   SCRIPT_DEPENDENCY*   pDependency = NULL;              // ScriptDependency associated with the selected item
   LVITEM&              oOutput = pMessageData->item;    // Convenience pointer for the item output data
  
   TRY
   {
      // Find SCRIPT_DEPENDECY associated with item
      if (findObjectInAVLTreeByIndex(pDialogData->pResultsTree, oOutput.iItem, (LPARAM&)pDependency))
      {
         // Determine which property is required
         switch (oOutput.iSubItem)
         {
         /// [SCRIPT-NAME]
         case DEPENDENCIES_COLUMN_NAME:
            if (oOutput.mask INCLUDES LVIF_TEXT)
               StringCchCopy(oOutput.pszText, oOutput.cchTextMax, pDependency->szScriptName);

            // Set anything else requested to NULL
            if (oOutput.mask INCLUDES LVIF_INDENT)
               oOutput.iIndent = NULL;
            if (oOutput.mask INCLUDES LVIF_IMAGE)
               oOutput.iImage = getAppImageTreeIconIndex(ITS_SMALL, TEXT("SCRIPT_CALLER_ICON"));
            break;

         /// [CALL-COUNT/LINE-NUMBER]
         case DEPENDENCIES_COLUMN_CALLS:
            if (oOutput.mask INCLUDES LVIF_TEXT)
               StringCchPrintf(oOutput.pszText, oOutput.cchTextMax, TEXT("%u"), pDialogData->eType == CT_SCRIPT ? pDependency->iCalls : pDependency->iLine + 1);
            break;
         }
      }
      else 
         // [FAILED]
         setMissingListViewItem(&oOutput, getTreeNodeCount(pDialogData->pResultsTree));
   }
   CATCH0("");
}


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : dlgprocDependencyDialog
// Description     : Dialog proc
// 
// 
INT_PTR  dlgprocDependencyDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   DEPENDENCY_DIALOG_DATA*  pDialogData;   // Callers data
   NMHDR*                   pHeader;       // Notification header
   POINT                    ptCursor;      // Cursor pos
   BOOL                     bResult;       // Operation result

   TRY
   {
      // Prepare
      pDialogData = (DEPENDENCY_DIALOG_DATA*)GetWindowLong(hDialog, DWL_USER);
      bResult      = FALSE;

      // Examine message
      switch (iMessage)
      {
      /// [CREATION]
      case WM_INITDIALOG:
         bResult = initDependencyDialog((DEPENDENCY_DIALOG_DATA*)lParam, hDialog);
         break;

      /// [DESTRUCTION]
      case WM_DESTROY:
         onDependencyDialog_Destroy(pDialogData);
         break;

      /// [COMMAND PROCESSING]
      case WM_COMMAND:
         bResult = onDependencyDialog_Command(pDialogData, LOWORD(wParam), (HWND)lParam);
         break;

      /// [CONTEXT MENU]
      case WM_CONTEXTMENU:
         // Ensure user has clicked the ListView
         if (bResult = (GetDlgItem(hDialog, IDC_DEPENDENCIES_LIST) == (HWND)wParam))
         {
            ptCursor.x = GET_X_LPARAM(lParam);
            ptCursor.y = GET_Y_LPARAM(lParam);
            onDependencyDialog_ContextMenu(pDialogData, (HWND)wParam, &ptCursor);
         }
         break;
         
      /// [NOTIFICATION]
      case WM_NOTIFY:
         // Prepare
         pHeader = (NMHDR*)lParam;

         // Examine source control
         if (bResult = (wParam == IDC_DEPENDENCIES_LIST))
         {
            switch (pHeader->code)
            {
            // [REQUEST INFO]
            case LVN_GETDISPINFO:   onDependencyDialog_RequestData(pDialogData, (NMLVDISPINFO*)pHeader);    break;
            // [COLUMN SORT]
            case LVN_COLUMNCLICK:   onDependencyDialog_ColumnClick(pDialogData, (NMLISTVIEW*)pHeader);      break;
            // [CUSTOM DRAW]
            case NM_CUSTOMDRAW:     SetWindowLong(hDialog, DWL_MSGRESULT, onCustomDrawListView(hDialog, pHeader->hwndFrom, (NMLVCUSTOMDRAW*)pHeader));      break;
            // UNHANDLED
            default:                bResult = FALSE;
            }
         }
         break;

      /// [HELP]
      case WM_HELP:
         bResult = displayHelp(TEXT("Properties_DependenciesExternal"));
         break;

      /// [OWNER DRAW]
      case WM_DRAWITEM:
         switch (wParam)
         {
         case IDC_SCRIPTCALL_STATIC:  bResult = TRUE;   onDependencyDialog_OwnerDraw(pDialogData, (DRAWITEMSTRUCT*)lParam);     break;
         case IDC_DIALOG_ICON:        bResult = onOwnerDrawStaticIcon(lParam, TEXT("SCRIPT_CALLER_ICON"), 96);                  break;
         default:                     bResult = dlgprocVistaStyleDialog(hDialog, iMessage, wParam, lParam);                     break;
         }
         break;

      /// [VISUAL STYLE]
      default:
         bResult = dlgprocVistaStyleDialog(hDialog, iMessage, wParam, lParam);
         break;

      /// [STYLE CHANGED]
      case WM_THEMECHANGED:
         ListView_SetBkColor(GetDlgItem(hDialog, IDC_DEPENDENCIES_LIST), getThemeSysColour(TEXT("TAB"), COLOR_WINDOW));
         break;
      }

      // Return result
      return bResult;
   }
   /// [EXCEPTION HANDLER]
   CATCH3("iMessage=%s  wParam=%d  lParam=%d", identifyMessage(iMessage), wParam, lParam);
   return FALSE;
}

