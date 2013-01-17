//
// FILE_NAME.cpp : FILE_DESCRIPTION
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

/// Function name  : displayScriptCallDialog
// Description     : Displays the ScriptCall dialog
// 
// HWND       hParent      : [in] Parent window
// AVL_TREE*  pCallersTree : [in] ScriptDependencies tree
// 
VOID  displayScriptCallDialog(HWND  hParent, AVL_TREE*  pCallersTree)
{
   /// Display dialog
   DialogBoxParam(getResourceInstance(), TEXT("SCRIPTCALL_DIALOG"), hParent, dlgprocScriptCallDialog, (LPARAM)pCallersTree);
}


/// Function name  : initScriptCallDialog
// Description     : Initialises the dialog and controls
// 
// HWND       hDialog      : [in] ScriptCall dialog
// AVL_TREE*  pCallersTree : [in] ScriptDependencies tree
// 
// Return Value   : TRUE
// 
BOOL  initScriptCallDialog(HWND  hDialog, AVL_TREE*  pCallersTree)
{
   LISTVIEW_COLUMNS   oDepedenciesListView = { 2, IDS_DEPENDENCIES_COLUMN_NAME, 240, 90,  NULL, NULL, NULL };    // ListView column data
   SCRIPT_DOCUMENT*   pDocument;

   /// Store results tree as dialog data
   SetWindowLong(hDialog, DWL_USER, (LONG)pCallersTree);

   // Setup ListView
   initReportModeListView(GetControl(hDialog, IDC_DEPENDENCIES_LIST), &oDepedenciesListView, TRUE);
   ListView_SetItemCount(GetControl(hDialog, IDC_DEPENDENCIES_LIST), getTreeNodeCount(pCallersTree));
   //ListView_SetSelectedColumn(GetControl(hDialog, IDC_DEPENDENCIES_LIST), DEPENDENCIES_COLUMN_NAME);

   // Setup dialog title
   pDocument = getActiveScriptDocument();
   utilSetWindowTextf(GetControl(hDialog, IDC_SCRIPTCALL_TEXT_STATIC), TEXT("The following scripts are dependent upon '%s'"), identifyScriptName(pDocument->pScriptFile));

   // [DIALOG] Centre dialog and set caption
   SetWindowText(hDialog, getAppName());
   utilCentreWindow(getAppWindow(), hDialog);

   // [OWNER DRAW]
   utilAddWindowStyle(GetDlgItem(hDialog, IDC_DIALOG_TITLE), SS_OWNERDRAW); // WITH CSS_CENTRE);

   // Return TRUE
   return TRUE;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onScriptCallDialog_Command
// Description     : Handles closing the dialog and loading dependencies
// 
// HWND        hDialog       : [in] ScriptCall dialog
// CONST UINT  iControlID    : [in] ID of the control
// AVL_TREE*   pCallersTree  : [in] ScriptDependencies tree
// 
// Return Value   : TRUE if processed, FALSE otherwise
// 
BOOL  onScriptCallDialog_Command(HWND  hDialog, CONST UINT  iControlID, HWND  hCtrl, AVL_TREE*  pCallersTree)
{
   SCRIPT_DOCUMENT*  pDocument;
   BOOL              bResult;

   // Prepare
   bResult = FALSE;

   // Examine source
   switch (iControlID)
   {
   /// [OK/CANCEL] Close dialog
   case IDOK:
   case IDCANCEL:
      EndDialog(hDialog, IDOK);
      bResult = TRUE;
      break;

   /// [LOAD DEPENDENCIES] Load selected scripts
   case IDM_DEPENDENCIES_LOAD:
      // Lookup active script document
      ASSERT(pDocument = getActiveScriptDocument());

      // Load selected dependencies
      onDependenciesPage_LoadSelectedScripts(hDialog, pDocument->pScriptFile, pCallersTree);

      // Close dialog
      EndDialog(hDialog, IDOK);
      bResult = TRUE;
      break;
   }

   // Return result
   return bResult;
}


/// Function name  : onScriptCallDialog_ContextMenu
// Description     : Displays the ListView context menu
// 
// HWND         hDialog   : [in] ScriptCall Dialog
// HWND         hListView : [in] ListView
// CONST POINT  ptCursor  : [in] Cursor position, in screen co-ordinates
// 
VOID  onScriptCallDialog_ContextMenu(HWND  hDialog, HWND  hListView, CONST POINT*  ptCursor)
{
   CUSTOM_MENU*  pCustomMenu;    // Custom Popup menu

   // Create Dependencies Custom Popup menu
   pCustomMenu = createCustomMenu(TEXT("SCRIPT_MENU"), TRUE, IDM_DEPENDENCIES_POPUP);

   // Enable 'Load script' only if items are selected.
   EnableMenuItem(pCustomMenu->hPopup, IDM_DEPENDENCIES_LOAD, (ListView_GetSelectedCount(hListView) > 0 ? MF_ENABLED : MF_DISABLED));

   // Disable 'Refresh' always
   EnableMenuItem(pCustomMenu->hPopup, IDM_DEPENDENCIES_REFRESH, MF_DISABLED);

   /// Display context menu
   TrackPopupMenu(pCustomMenu->hPopup, TPM_TOPALIGN WITH TPM_LEFTALIGN, ptCursor->x, ptCursor->y, NULL, hDialog, NULL);

   // Cleanup and return TRUE
   deleteCustomMenu(pCustomMenu);
}


/// Function name  : onScriptCallDialog_ColumnClick
// Description     : Sorts the callers tree
// 
// HWND         hDialog      : [in] ScriptCall dialog
// AVL_TREE*    pCallersTree : [in] ScriptDependency tree
// NMLISTVIEW*  pHeader      : [in] Message header
// 
VOID  onScriptCallDialog_ColumnClick(HWND  hDialog, AVL_TREE*  pCallersTree, NMLISTVIEW*  pHeader)
{
   AVL_TREE_ORDERING  eNewOrder;
   AVL_TREE*          pNewTree;

   // Calculate opposite sort order
   eNewOrder = (pCallersTree->pSortKeys[DEPENDENCIES_COLUMN_NAME]->eDirection == AO_DESCENDING ? AO_ASCENDING : AO_DESCENDING);

   // [CHECK] Ensure user clicked the 'scriptname' column
   if (pHeader->iSubItem == DEPENDENCIES_COLUMN_NAME)
   {
      /// Sort results into new tree
      pNewTree = duplicateAVLTree(pCallersTree, createAVLTreeSortKey(AK_TEXT, eNewOrder), NULL, NULL);

      // Delete existing tree
      pCallersTree->pfnDeleteNode = NULL;
      deleteAVLTree(pCallersTree);

      /// Store new tree
      SetWindowLong(hDialog, DWL_USER, (LONG)pNewTree);

      // Update ListView
      ListView_SetItemCount(GetControl(hDialog, IDC_DEPENDENCIES_LIST), getTreeNodeCount(pNewTree));
   }
}


/// Function name  : onScriptCallDialog_Destroy
// Description     : Destroys the ScriptDependencies tree
// 
// HWND       hDialog      : [in] ScriptCall dialog
// AVL_TREE*  pCallersTree : [in] ScriptDependencies tree
// 
VOID  onScriptCallDialog_Destroy(HWND  hDialog, AVL_TREE*  pCallersTree)
{
   /// Delete data tree
   deleteAVLTree(pCallersTree);
   SetWindowLong(hDialog, DWL_USER, NULL);
}


/// Function name  : onScriptCallDialog_RequestData
// Description     : Supply the requested piece of ScriptDependencies data for the ListView (sub)item
// 
// PROPERTIES_DATA*  pSheetData   : [in]     Properties sheet dialog data
// AVL_TREE*         pCallersTree : [in]     Window handle of the dependencies page
// NMLVDISPINFO*     pMessageData : [in/out] Indicates what data is required and stores it
//
VOID   onScriptCallDialog_RequestData(HWND  hDialog, AVL_TREE*  pCallersTree, NMLVDISPINFO*  pMessageData)
{
   SCRIPT_DEPENDENCY*   pDependency;                     // ScriptDependency associated with the selected item
   LVITEM&              oOutput = pMessageData->item;    // Convenience pointer for the item output data
  
   // Prepare
   pDependency = NULL;

   // Find SCRIPT_DEPENDECY associated with item
   if (findObjectInAVLTreeByIndex(pCallersTree, oOutput.iItem, (LPARAM&)pDependency))
   {
      // Determine which property is required
      switch (oOutput.iSubItem)
      {
      /// [NAME]
      case DEPENDENCIES_COLUMN_NAME:
         if (oOutput.mask INCLUDES LVIF_TEXT)
            StringCchCopy(oOutput.pszText, oOutput.cchTextMax, pDependency->szScriptName);

         // Set anything else requested to NULL
         if (oOutput.mask INCLUDES LVIF_INDENT)
            oOutput.iIndent = NULL;
         if (oOutput.mask INCLUDES LVIF_IMAGE)
            oOutput.iImage = getAppImageTreeIconIndex(ITS_SMALL, TEXT("SCRIPT_CALLER_ICON"));
         break;

      /// [CALLS]
      case DEPENDENCIES_COLUMN_CALLS:
         if (oOutput.mask INCLUDES LVIF_TEXT)
            StringCchPrintf(oOutput.pszText, oOutput.cchTextMax, TEXT("%u"), pDependency->iCalls);
         break;
      }
   }
   else if (oOutput.mask INCLUDES LVIF_TEXT)
      StringCchCopy(oOutput.pszText, oOutput.cchTextMax, TEXT("Error: Item Not found"));
   else
      oOutput.pszText = TEXT("ERROR: Item not found");
}


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : dlgprocScriptCallDialog
// Description     : 
// 
// 
INT_PTR  dlgprocScriptCallDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   AVL_TREE*   pCallersTree;  // Callers data
   NMHDR*      pHeader;       // Notification header
   POINT       ptCursor;      // Cursor pos
   HWND        hListView;     // ListView
   BOOL        bResult;       // Operation result

   // Prepare
   pCallersTree = (AVL_TREE*)GetWindowLong(hDialog, DWL_USER);
   bResult      = FALSE;

   // Examine message
   switch (iMessage)
   {
   /// [CREATION]
   case WM_INITDIALOG:
      bResult = initScriptCallDialog(hDialog, (AVL_TREE*)lParam);
      break;

   /// [DESTRUCTION]
   case WM_DESTROY:
      onScriptCallDialog_Destroy(hDialog, pCallersTree);
      bResult = TRUE;
      break;

   /// [COMMAND PROCESSING]
   case WM_COMMAND:
      bResult = onScriptCallDialog_Command(hDialog, LOWORD(wParam), (HWND)lParam, pCallersTree);
      break;

   /// [CONTEXT MENU]
   case WM_CONTEXTMENU:
      // Prepare
      ptCursor.x = GET_X_LPARAM(lParam);
      ptCursor.y = GET_Y_LPARAM(lParam);
      hListView  = GetControl(hDialog, IDC_DEPENDENCIES_LIST);

      // [CHECK] Has user clicked the ListView?
      if (hListView == (HWND)wParam)
      {
         // [SUCCESS] Display context emnu
         onScriptCallDialog_ContextMenu(hDialog, hListView, &ptCursor);
         bResult = TRUE;
      }
      break;
      
   /// [NOTIFICATION]
   case WM_NOTIFY:
      // Prepare
      pHeader = (NMHDR*)lParam;

      // Examine source control
      switch (pHeader->idFrom)
      {
      case IDC_DEPENDENCIES_LIST:
         // [REQUEST INFO]
         if (pHeader->code == LVN_GETDISPINFO)
         {
            onScriptCallDialog_RequestData(hDialog, pCallersTree, (NMLVDISPINFO*)pHeader);
            bResult = TRUE;
         }
         // [COLUMN SORT]
         else if (pHeader->code == LVN_COLUMNCLICK)
         {
            onScriptCallDialog_ColumnClick(hDialog, pCallersTree, (NMLISTVIEW*)pHeader);
            bResult = TRUE;
         }
         else
            // [HOVER PROCESSING]
            bResult = onCustomListViewNotify(hDialog, TRUE, IDC_DEPENDENCIES_LIST, pHeader);
         break;
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
      case IDC_DIALOG_ICON:   bResult = onOwnerDrawStaticIcon(lParam, TEXT("SCRIPT_CALLER_ICON"), 96);    break;
      default:                bResult = dlgprocVistaStyleDialog(hDialog, iMessage, wParam, lParam);       break;
      }
      break;

   /// [VISUAL STYLE]
   default:
      bResult = dlgprocVistaStyleDialog(hDialog, iMessage, wParam, lParam);
      break;
   }

   // Return result
   return bResult;
}

