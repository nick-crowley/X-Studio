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
#define       VARIABLES_COLUMN_NAME         0
#define       VARIABLES_COLUMN_VALUE        1

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : displayProjectVariablesDialog
// Description     : Displays the ProjectVariables dialog
// 
// HWND    hParent : [in] Parent window
// 
BOOL  displayProjectVariablesDialog(HWND  hParent)
{
   // [TRACK]
   CONSOLE_ACTION1(getActiveProjectFileName());

   /// Display dialog
   return (getActiveProject() ? (BOOL)showDialog(TEXT("PROJECT_VARIABLES_DIALOG"), hParent, dlgprocProjectVariablesDialog, NULL) : FALSE);
}


/// Function name  : initProjectVariablesDialog
// Description     : Initialises the dialog and controls
// 
// HWND       hDialog      : [in] ProjectVariables dialog
// AVL_TREE*  pCallersTree : [in] ScriptDependencies tree
// 
// Return Value   : TRUE
// 
BOOL  initProjectVariablesDialog(HWND  hDialog)
{
   LISTVIEW_COLUMNS   oVariablesListView = { 2, IDS_PROJECT_VARIABLES_COLUMN_NAME, 240, 90,  NULL, NULL, NULL };    // ListView column data
   PROJECT_DOCUMENT*  pProject;

   // Prepare
   pProject = getActiveProject();

   // Setup ListView
   initReportModeListView(GetControl(hDialog, IDC_VARIABLES_LIST), &oVariablesListView, TRUE);
   ListView_SetItemCount(GetControl(hDialog, IDC_VARIABLES_LIST), getProjectFileVariableCount(pProject->pProjectFile));

   // Setup dialog title
   utilSetWindowTextf(GetControl(hDialog, IDC_VARIABLES_STATIC), TEXT("The project '%s' contains the following variables:"), identifyGameFileFilename(pProject->pProjectFile));
   utilEnableDlgItem(hDialog, IDC_REMOVE_VARIABLE, FALSE);

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


/// Function name  : onProjectVariablesDialog_AddVariable
// Description     : Displays the 'Insert Variable' dialog
// 
// HWND  hDialog   : [in] ProjectVariables dialog
// HWND  hListView : [in] Variable ListView
// 
VOID   onProjectVariablesDialog_AddVariable(HWND  hDialog, HWND  hListView)
{
   PROJECT_DOCUMENT*  pProject = getActiveProject();

   // [TRACK]
   CONSOLE_ACTION();

   /// Display InsertVariable dialog
   if (pProject AND displayInsertVariableDialog(pProject->pProjectFile, hDialog))
   {
      // [CHANGED] Update ListView
      ListView_SetItemCount(hListView, getProjectFileVariableCount(pProject->pProjectFile));

      // [MODIFIED]
      setProjectModifiedFlag(pProject, TRUE);
   }
}


/// Function name  : onProjectVariablesDialog_Command
// Description     : Handles closing the dialog and loading dependencies
// 
// HWND        hDialog       : [in] ProjectVariables dialog
// CONST UINT  iControlID    : [in] ID of the control
// 
// Return Value   : TRUE if processed, FALSE otherwise
// 
BOOL  onProjectVariablesDialog_Command(HWND  hDialog, CONST UINT  iControlID, HWND  hCtrl)
{
   BOOL  bResult = FALSE;

   // Examine source
   switch (iControlID)
   {
   /// [OK/CANCEL] Close dialog
   case IDOK:
   case IDCANCEL:
      EndDialog(hDialog, IDOK);
      bResult = TRUE;
      break;

   /// [ADD VARIABLE]
   case IDC_ADD_VARIABLE:
   case IDM_PROJECT_VARIABLES_ADD:
      onProjectVariablesDialog_AddVariable(hDialog, GetControl(hDialog, IDC_VARIABLES_LIST));
      bResult = TRUE;
      break;

   /// [REMOVE VARIABLE]
   case IDC_REMOVE_VARIABLE:
   case IDM_PROJECT_VARIABLES_REMOVE:
      onProjectVariablesDialog_RemoveVariable(hDialog, GetControl(hDialog, IDC_VARIABLES_LIST));
      bResult = TRUE;
      break;
   }

   // Return result
   return bResult;
}


/// Function name  : onProjectVariablesDialog_ContextMenu
// Description     : Displays the ListView context menu
// 
// HWND         hDialog   : [in] ProjectVariables Dialog
// HWND         hListView : [in] ListView
// CONST POINT  ptCursor  : [in] Cursor position, in screen co-ordinates
// 
VOID  onProjectVariablesDialog_ContextMenu(HWND  hDialog, HWND  hListView, CONST POINT*  ptCursor)
{
   CUSTOM_MENU*  pCustomMenu;    // Custom Popup menu

   // Create ProjectVariables Custom Popup menu
   pCustomMenu = createCustomMenu(TEXT("DIALOG_MENU"), TRUE, IDM_PROJECT_VARIABLES_POPUP);

   // Enable 'Remove Variable' only if items are selected.
   EnableMenuItem(pCustomMenu->hPopup, IDM_PROJECT_VARIABLES_ADD,    MF_ENABLED);
   EnableMenuItem(pCustomMenu->hPopup, IDM_PROJECT_VARIABLES_REMOVE, (ListView_GetSelectedCount(hListView) > 0 ? MF_ENABLED : MF_DISABLED));

   /// Display context menu
   TrackPopupMenu(pCustomMenu->hPopup, TPM_TOPALIGN WITH TPM_LEFTALIGN, ptCursor->x, ptCursor->y, NULL, hDialog, NULL);

   // Cleanup and return TRUE
   deleteCustomMenu(pCustomMenu);
}


/// Function name  : onProjectVariablesDialog_RemoveVariable
// Description     : Removes the selected variable from the project file
// 
// HWND  hDialog   : [in] ProjectVariables dialog
// HWND  hListView : [in] Variable ListView
// 
VOID   onProjectVariablesDialog_RemoveVariable(HWND  hDialog, HWND  hListView)
{
   PROJECT_DOCUMENT*  pProject = getActiveProject();
   UINT               iIndex   = ListView_GetSelected(hListView);

   // [TRACK]
   CONSOLE_ACTION();

   // [CHECK] Ensure item is selected
   if (iIndex != -1)
   {
      // [SUCCESS] Destroy item and update ListView
      removeVariableFromProjectFileByIndex(pProject->pProjectFile, iIndex);
      ListView_SetItemCount(hListView, getProjectFileVariableCount(pProject->pProjectFile));

      // [MODIFIED]
      setProjectModifiedFlag(pProject, TRUE);
   }
}


/// Function name  : onProjectVariablesDialog_RequestData
// Description     : Supply the requested piece of ScriptDependencies data for the ListView (sub)item
// 
// PROPERTIES_DATA*  pSheetData   : [in]     Properties sheet dialog data
// AVL_TREE*         pCallersTree : [in]     Window handle of the dependencies page
// NMLVDISPINFO*     pMessageData : [in/out] Indicates what data is required and stores it
//
VOID   onProjectVariablesDialog_RequestData(HWND  hDialog, LVITEM&  oOutput)
{
   PROJECT_VARIABLE*   pVariable = NULL;                     // ScriptDependency associated with the selected item
   PROJECT_DOCUMENT*   pProject  = getActiveProject();

   // Find SCRIPT_DEPENDECY associated with item
   if (findVariableInProjectFileByIndex(pProject->pProjectFile, oOutput.iItem, pVariable))
   {
      // Determine which property is required
      switch (oOutput.iSubItem)
      {
      /// [NAME]
      case VARIABLES_COLUMN_NAME:
         if (oOutput.mask INCLUDES LVIF_TEXT)
            StringCchCopy(oOutput.pszText, oOutput.cchTextMax, pVariable->szName);

         // Set anything else requested to NULL
         if (oOutput.mask INCLUDES LVIF_INDENT)
            oOutput.iIndent = NULL;
         if (oOutput.mask INCLUDES LVIF_IMAGE)
            oOutput.iImage = getAppImageTreeIconIndex(ITS_SMALL, TEXT("VARIABLE_ICON"));
         break;

      /// [VALUE]
      case VARIABLES_COLUMN_VALUE:
         if (oOutput.mask INCLUDES LVIF_TEXT)
            StringCchPrintf(oOutput.pszText, oOutput.cchTextMax, TEXT("%u"), pVariable->iValue);
         break;
      }
   }
   else 
      // [ERROR]
      setMissingListViewItem(&oOutput, getTreeNodeCount(pProject->pProjectFile->pVariablesTree));
}


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : dlgprocProjectVariablesDialog
// Description     : 
// 
// 
INT_PTR  dlgprocProjectVariablesDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   NMLVDISPINFO*  pRequestData;  // 
   NMHDR*         pHeader;       // Notification header
   POINT          ptCursor;      // Cursor pos
   HWND           hListView;     // ListView
   BOOL           bResult;       // Operation result

   // Prepare
   bResult = TRUE;

   // Examine message
   switch (iMessage)
   {
   /// [CREATION]
   case WM_INITDIALOG:
      initProjectVariablesDialog(hDialog);
      break;

   /// [COMMAND PROCESSING]
   case WM_COMMAND:
      bResult = onProjectVariablesDialog_Command(hDialog, LOWORD(wParam), (HWND)lParam);
      break;

   /// [CONTEXT MENU]
   case WM_CONTEXTMENU:
      // Prepare
      ptCursor.x = GET_X_LPARAM(lParam);
      ptCursor.y = GET_Y_LPARAM(lParam);
      hListView  = GetControl(hDialog, IDC_VARIABLES_LIST);

      // [LISTVIEW] Display context emnu
      if (hListView == (HWND)wParam)
         onProjectVariablesDialog_ContextMenu(hDialog, hListView, &ptCursor);
      else
         bResult = FALSE;
      break;
      
   /// [NOTIFICATION]
   case WM_NOTIFY:
      // Prepare
      pHeader = (NMHDR*)lParam;

      // Examine source control
      switch (pHeader->code)
      {
      /// [REQUEST INFO]
      case LVN_GETDISPINFO:
         pRequestData = (NMLVDISPINFO*)pHeader;
         onProjectVariablesDialog_RequestData(hDialog, pRequestData->item);
         break;

      /// [SELECTION CHANGED]
      case LVN_ITEMCHANGED:
         utilEnableDlgItem(hDialog, IDC_REMOVE_VARIABLE, ListView_GetSelectedCount(pHeader->hwndFrom) > 0);
         onCustomListViewNotify(hDialog, TRUE, IDC_VARIABLES_LIST, pHeader);
         break;
         
      /// [CUSTOM DRAW]
      case NM_CUSTOMDRAW:
         switch (pHeader->idFrom)
         {
         case IDC_ADD_VARIABLE:     onCustomDrawButton(hDialog, pHeader, ITS_MEDIUM, TEXT("ADD_ICON"), TRUE);      break;
         case IDC_REMOVE_VARIABLE:  onCustomDrawButton(hDialog, pHeader, ITS_MEDIUM, TEXT("REMOVE_ICON"), TRUE);   break;
         default:                   bResult = onCustomListViewNotify(hDialog, TRUE, IDC_VARIABLES_LIST, pHeader);  break;
         }
         break;

      /// [HOVER]
      default:
         bResult = onCustomListViewNotify(hDialog, TRUE, IDC_VARIABLES_LIST, pHeader);
         break;
      }
      break;

   /// [OWNER DRAW]
   case WM_DRAWITEM:
      bResult = (wParam == IDC_DIALOG_ICON ? onOwnerDrawStaticIcon(lParam, TEXT("VARIABLE_DEPENDENCY_ICON"), 96) : FALSE);
      break;

   /// [HELP] Invoke help
   case WM_HELP:
      bResult = displayHelp(TEXT("Project_Variables"));
      break;

   // [UNHANDLED]
   default:
      bResult = FALSE;
      break;
   }

   // Return result
   return (bResult ? TRUE : dlgprocVistaStyleDialog(hDialog, iMessage, wParam, lParam));
}

