//
// Properties Dialog Dependencies.cpp : Dependencies page
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////

// ScriptDependencies ListView column IDs
#define       DEPENDENCIES_COLUMN_NAME         0
#define       DEPENDENCIES_COLUMN_CALLS        1

// GameString ListView column IDs
#define       STRINGS_COLUMN_PAGE              0
#define       STRINGS_COLUMN_ID                1
#define       STRINGS_COLUMN_TEXT              2

// VariableDependencies ListView column IDs
#define       VARIABLES_COLUMN_NAME            0
#define       VARIABLES_COLUMN_TYPE            1
#define       VARIABLES_COLUMN_USAGE           2

// VariableDependencies Types
CONST TCHAR*  szVariableTypes[2]  = { TEXT("Local"), TEXT("Global") };

// VariableDependencies Usages
CONST TCHAR*  szVariableUsages[4] = { NULL, TEXT("Get"),   TEXT("Set"),   TEXT("Get/Set") };

// onException: Display 
#define  ON_EXCEPTION()    displayException(pException);

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : updateScriptDependenciesPageList
// Description     : Refresh the list of script dependencies
// 
// PROPERTIES_DATA*  pSheetData : [in] Properties sheet data
// 
VOID   updateScriptDependenciesPage_List(PROPERTIES_DATA*  pSheetData, HWND  hPage)
{
   // [VERBOSE]
   //CONSOLE("Updating 'Dependencies' page tree/list");

   // Delete existing dependencies tree (if any)
   if (pSheetData->pScriptDependecies)
      deleteAVLTree(pSheetData->pScriptDependecies);

   // Generate new dependencies tree
   pSheetData->pScriptDependecies = CodeEdit_GetScriptDependencyTree(pSheetData->pScriptDocument->hCodeEdit);

   // Set new list count
   ListView_SetItemCount(GetDlgItem(hPage, IDC_DEPENDENCIES_LIST), getTreeNodeCount(pSheetData->pScriptDependecies));
}


/// Function name  : updateScriptStringsPageList
// Description     : Refresh the list of GameString dependencies
// 
// PROPERTIES_DATA*  pSheetData : [in] Properties sheet data
// 
VOID   updateScriptStringsPage_List(PROPERTIES_DATA*  pSheetData, HWND  hPage)
{
   PROJECT_DOCUMENT*  pProject;

   // [VERBOSE]
   //CONSOLE("Updating 'Strings' page tree/list");

   // Delete existing GameStrings tree (if any)
   if (pSheetData->pStringDependencies)
      deleteAVLTree(pSheetData->pStringDependencies);

   // Get active project (if any)
   pProject = getActiveProject();

   // Generate new GameStrings tree
   pSheetData->pStringDependencies = CodeEdit_GetStringDependencyTree(pSheetData->pScriptDocument->hCodeEdit, pProject ? pProject->pProjectFile : NULL);

   // Set new list count
   ListView_SetItemCount(GetDlgItem(hPage, IDC_STRINGS_LIST), getTreeNodeCount(pSheetData->pStringDependencies));
}


/// Function name  : updateScriptVariablesPageList
// Description     : Refresh the list of Variable dependencies
// 
// PROPERTIES_DATA*  pSheetData : [in] Properties sheet data
// 
VOID   updateScriptVariablesPage_List(PROPERTIES_DATA*  pSheetData, HWND  hPage)
{
   // [VERBOSE]
   //CONSOLE("Updating 'Variables' page tree/list");

   // Delete existing dependencies tree (if any)
   if (pSheetData->pVariableDependencies)
      deleteAVLTree(pSheetData->pVariableDependencies);

   // Generate new dependencies tree
   pSheetData->pVariableDependencies = CodeEdit_GetVariableDependencyTree(pSheetData->pScriptDocument->hCodeEdit);

   // Set new list count
   ListView_SetItemCount(GetDlgItem(hPage, IDC_VARIABLES_LIST), getTreeNodeCount(pSheetData->pVariableDependencies));
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onDependenciesPage_ContextMenu
// Description     : Display the ScriptDependencies or VariableDependencies Popup Menu
// 
// SCRIPT_DOCUMENT*  pDocument : [in] Script document data
// HWND              hCtrl     : [in] Window handle of the control sending the message
// CONST UINT        iCursorX  : [in] Screen co-ordinate cursor X position
// CONST UINT        iCursorY  : [in] Screen co-ordinate cursor Y position
// 
// Return Value   : TRUE
// 
BOOL  onDependenciesPage_ContextMenu(SCRIPT_DOCUMENT*  pDocument, HWND  hCtrl, CONST UINT  iCursorX, CONST UINT  iCursorY)
{
   CUSTOM_MENU*  pCustomMenu;    // Custom Popup menu
   UINT          iSubMenuID;     // Popup submenu ID

   // [TRACK]
   CONSOLE_ACTION();

   // Determine submenu
   switch (GetWindowID(hCtrl))
   {
   /// [LISTVIEW] Display the appropriate popup menu
   case IDC_DEPENDENCIES_LIST:   iSubMenuID = IDM_DEPENDENCIES_POPUP;  CONSOLE("Displaying dependencies menu");  break;
   case IDC_VARIABLES_LIST:      iSubMenuID = IDM_VARIABLES_POPUP;     CONSOLE("Displaying variables menu");     break;
   case IDC_STRINGS_LIST:        iSubMenuID = IDM_STRINGS_POPUP;       CONSOLE("Displaying strings menu");       break;
   default:                      return FALSE;
   }

   // Create Dependencies Custom Popup menu
   pCustomMenu = createCustomMenu(TEXT("SCRIPT_MENU"), TRUE, iSubMenuID);

   /// [DEPENDENCIES] Disable 'Load script' if no item is selected
   if (GetWindowID(hCtrl) == IDC_DEPENDENCIES_LIST)
      EnableMenuItem(pCustomMenu->hPopup, IDM_DEPENDENCIES_LOAD, (ListView_GetSelectedCount(hCtrl) > 0 ? MF_ENABLED : MF_DISABLED));

   /// Display context menu
   TrackPopupMenu(pCustomMenu->hPopup, TPM_TOPALIGN WITH TPM_LEFTALIGN, iCursorX, iCursorY, NULL, GetParent(hCtrl), NULL);

   // Cleanup and return TRUE
   deleteCustomMenu(pCustomMenu);
   return TRUE;
}


/// Function name  : onDependenciesLoadSelectedScripts
// Description     : Loads the selected script dependencies
// 
// PROPERTIES_DATA*  pSheetData   : [in] Properties sheet dialog data
// HWND              hPage        : [in] Window handle of the dependencies page
// 
VOID   onDependenciesPage_LoadSelectedScripts(HWND  hPage, SCRIPT_FILE*  pScriptFile, AVL_TREE*  pDependenciesTree)
{
   SCRIPT_DEPENDENCY*   pDependency;      // ScriptDependency associated with the selected item
   LOADING_OPTIONS      oOptions;         // Used for highlighting caller
   HWND                 hListView;        // Dependencies ListView
   TCHAR*               szFolder;         // Folder of input script
   LIST*                pDependencyList;  // List of selected ScriptDepdencies
   INT                  iSelected;        // Index of the selected item
            
   /// [CHECK] Are any dependencies selected?
   if (ListView_GetSelectedCount(hListView = GetDlgItem(hPage, IDC_DEPENDENCIES_LIST)) == 0)
      return;
   
   // [VERBOSE]
   CONSOLE_COMMAND();

   // Prepare
   utilZeroObject(&oOptions, LOADING_OPTIONS);
   pDependencyList = createList(NULL);
   
   /// Extract ScriptDependencies for selected scripts
   for (iSelected = ListView_GetSelected(hListView); findObjectInAVLTreeByIndex(pDependenciesTree, iSelected, (LPARAM&)pDependency); iSelected = ListView_GetNextItem(hListView, iSelected, LVNI_SELECTED))
   {
      appendObjectToList(pDependencyList, (LPARAM)pDependency);
      debugScriptDependency(pDependency);
   }

   // Clear selection
   ListView_SetItemState(hListView, -1, NULL, LVIS_SELECTED);
   szFolder = utilDuplicateFolderPath(pScriptFile->szFullPath);

   /// Attempt to open each selected script
   for (LIST_ITEM*  pIterator = getListHead(pDependencyList); pDependency = extractListItemPointer(pIterator, SCRIPT_DEPENDENCY); pIterator = pIterator->pNext)
   {
      //// Highlight Scriptcall on load
      //oOptions.bHighlight = TRUE;
      //StringCchCopy(oOptions.szSearchText, 256, pScriptFile->szScriptname);

      /// Attempt to open .pck / .xml version of script
      if (!commandLoadScriptDependency(getMainWindowData(), szFolder, pDependency, !pIterator->pNext, &oOptions))  // Activate the final document
         // [ERROR] "The script dependency '%s' could not be found in '%s'"
         displayMessageDialogf(hPage, IDS_GENERAL_DEPENDENCY_NOT_FOUND, MDF_OK WITH MDF_ERROR, pDependency->szScriptName, szFolder);
   }

   // Cleanup
   deleteList(pDependencyList);
   utilDeleteString(szFolder);
}


/// Function name  : onDependenciesPage_PerformSearch
// Description     : Initiates a script-call search operation
// 
// PROPERTIES_DATA*  pSheetData   : [in] Properties sheet dialog data
// HWND              hPage        : [in] Window handle of the dependencies page
// 
VOID  onDependenciesPage_PerformSearch(SCRIPT_DOCUMENT*  pDocument, HWND  hPage)
{
   SCRIPT_CONTENT     xContent;
   TCHAR*             szFolder;

   // Get script folder
   szFolder = utilDuplicateFolderPath(getDocumentPath(pDocument));

   // [QUESTION] "Would you like to search '%s' for all scripts that depend upon '%s'?"
   if (displayMessageDialogf(NULL, IDS_GENERAL_CONFIRM_DEPENDENCY_SEARCH, MDF_YESNO WITH MDF_QUESTION, szFolder, identifyScriptName(pDocument->pScriptFile)) == IDYES)
   {
      // [VERBOSE]
      CONSOLE_COMMAND();
      
      /// Launch search thread
      xContent.asScript = identifyScriptName(pDocument->pScriptFile);
      launchOperation(getMainWindowData(), createScriptOperationData(CT_SCRIPT, szFolder, xContent));
   }

   // Cleanup
   utilDeleteString(szFolder);
}



/// Function name  : onDependenciesPage_RequestData
// Description     : Supply the requested piece of ScriptDependencies data for the ListView (sub)item
// 
// PROPERTIES_DATA*  pSheetData   : [in]     Properties sheet dialog data
// HWND              hPage        : [in]     Window handle of the dependencies page
// NMLVDISPINFO*     pMessageData : [in/out] Indicates what data is required and stores it
//
VOID   onDependenciesPage_RequestData(PROPERTIES_DATA*  pSheetData, HWND  hPage, NMLVDISPINFO*  pMessageData)
{
   SCRIPT_DEPENDENCY*   pDependency;                     // ScriptDependency associated with the selected item
   LVITEM&              oOutput = pMessageData->item;    // Convenience pointer for the item output data
  
   // Prepare
   pDependency = NULL;

   // Find SCRIPT_DEPENDECY associated with item
   if (findObjectInAVLTreeByIndex(pSheetData->pScriptDependecies, oOutput.iItem, (LPARAM&)pDependency))
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
            oOutput.iImage = getAppImageTreeIconIndex(ITS_SMALL, TEXT("SCRIPT_DEPENDENCY_ICON"));
         break;

      /// [CALLS]
      case DEPENDENCIES_COLUMN_CALLS:
         if (oOutput.mask INCLUDES LVIF_TEXT)
            StringCchPrintf(oOutput.pszText, oOutput.cchTextMax, TEXT("%u"), pDependency->iCalls);
         break;
      }
   }
   // [FAILED] Error!
   else 
      setMissingListViewItem(&oOutput, getTreeNodeCount(pSheetData->pScriptDependecies));
}



/// Function name  : onStringsPage_RequestData
// Description     : Supply the requested piece of GameString data for the ListView (sub)item
// 
// PROPERTIES_DATA*  pSheetData   : [in]     Properties sheet dialog data
// HWND              hPage        : [in]     Window handle of the dependencies page
// NMLVDISPINFO*     pMessageData : [in/out] Indicates what data is required and stores it
//
VOID   onStringsPage_RequestData(PROPERTIES_DATA*  pSheetData, HWND  hPage, NMLVDISPINFO*  pMessageData)
{
   STRING_DEPENDENCY*  pDependecy;                     // GameString associated with the selected item
   LVITEM&             oOutput = pMessageData->item;    // Convenience pointer for the item output data
  
   // Find SCRIPT_DEPENDECY associated with item
   if (findObjectInAVLTreeByIndex(pSheetData->pStringDependencies, oOutput.iItem, (LPARAM&)pDependecy))
   {
      // Determine which property is required
      switch (oOutput.iSubItem)
      {
      /// [PAGE ID] Supply Page ID as text and GameString icon
      case STRINGS_COLUMN_PAGE:
         if (oOutput.mask INCLUDES LVIF_TEXT)
            StringCchPrintf(oOutput.pszText, oOutput.cchTextMax, TEXT("%u"), pDependecy->iPageID);

         if (oOutput.mask INCLUDES LVIF_IMAGE)
            oOutput.iImage = getAppImageTreeIconIndex(ITS_SMALL, TEXT("NEW_LANGUAGE_FILE_ICON"));

         if (oOutput.mask INCLUDES LVIF_INDENT)
            oOutput.iIndent = NULL;
         break;

      /// [STRING ID] Supply string ID as text
      case STRINGS_COLUMN_ID:
         if (oOutput.mask INCLUDES LVIF_TEXT)
            StringCchPrintf(oOutput.pszText, oOutput.cchTextMax, TEXT("%u"), pDependecy->iStringID);
         break;

      /// [TEXT] Supply text
      case STRINGS_COLUMN_TEXT:
         if (oOutput.mask INCLUDES LVIF_TEXT)
         {
            if (!pDependecy->pGameString) 
               StringCchCopy(oOutput.pszText, oOutput.cchTextMax, TEXT("{No such string}"));
            else
            {
               ERROR_QUEUE* pErrorQueue = createErrorQueue();
               RICH_TEXT*   pRichText;

               generateRichTextFromGameString(pDependecy->pGameString, pRichText, pErrorQueue);

               oOutput.lParam = (LPARAM)pRichText;
               oOutput.mask  |= LVIF_RICHTEXT WITH LVIF_DESTROYTEXT;

               deleteErrorQueue(pErrorQueue);
            }
         }
         break;
      }
   }
   // [FAILED] Error!
   else 
      setMissingListViewItem(&oOutput, getTreeNodeCount(pSheetData->pStringDependencies));
}


/// Function name  : onVariablesPage_RequestData
// Description     : Supply the requested piece of VariableDependencies data for the ListView (sub)item
// 
// PROPERTIES_DATA*  pSheetData   : [in]     Properties sheet dialog data
// HWND              hPage        : [in]     Window handle of the dependencies page
// NMLVDISPINFO*     pMessageData : [in/out] Indicates what data is required and stores it
//
VOID   onVariablesPage_RequestData(PROPERTIES_DATA*  pSheetData, HWND  hPage, NMLVDISPINFO*  pMessageData)
{
   VARIABLE_DEPENDENCY*  pDependency;                     // VariableDependency associated with the selected item
   LVITEM&               oOutput = pMessageData->item;    // Convenience pointer for the item output data
  
   // Prepare
   pDependency = NULL;

   // Find SCRIPT_DEPENDECY associated with item
   if (findObjectInAVLTreeByIndex(pSheetData->pVariableDependencies, oOutput.iItem, (LPARAM&)pDependency))
   {
      // Determine which property is required
      switch (oOutput.iSubItem)
      {
      /// [NAME]
      case VARIABLES_COLUMN_NAME:
         if (oOutput.mask INCLUDES LVIF_TEXT)
            StringCchCopy(oOutput.pszText, oOutput.cchTextMax, pDependency->szVariableName);

         // Set anything else requested to NULL
         if (oOutput.mask INCLUDES LVIF_INDENT)
            oOutput.iIndent = NULL;
         if (oOutput.mask INCLUDES LVIF_IMAGE)
            oOutput.iImage = getAppImageTreeIconIndex(ITS_SMALL, TEXT("VARIABLE_DEPENDENCY_ICON"));
         break;

      /// [TYPE]
      case VARIABLES_COLUMN_TYPE:
         if (oOutput.mask INCLUDES LVIF_TEXT)
            StringCchCopy(oOutput.pszText, oOutput.cchTextMax, szVariableTypes[pDependency->eType]);
         break;

      /// [USAGE]
      case VARIABLES_COLUMN_USAGE:
         if (oOutput.mask INCLUDES LVIF_TEXT)
            StringCchCopy(oOutput.pszText, oOutput.cchTextMax, szVariableUsages[pDependency->eUsage]);
         break;
      }
   }
   else
      setMissingListViewItem(&oOutput, getTreeNodeCount(pSheetData->pVariableDependencies));
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       DIALOG PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : dlgprocDependenciesPage
// Description     : Window procedure for the 'ScriptDependencies' properties dialog page
//
// 
INT_PTR   dlgprocDependenciesPage(HWND  hPage, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   PROPERTIES_DATA*  pDialogData;
   NMHDR*            pMessage;

   TRY
   {
      // Get properties dialog data
      pDialogData = getPropertiesDialogData(hPage);

      switch (iMessage)
      {
      /// [COMMAND PROCESSING]
      case WM_COMMAND:
         switch (LOWORD(wParam))
         {
         // [LOAD SELECTED SCRIPTS]
         case IDM_DEPENDENCIES_LOAD:
            CONSOLE_MENU(IDM_DEPENDENCIES_LOAD);
            onDependenciesPage_LoadSelectedScripts(hPage, pDialogData->pScriptDocument->pScriptFile, pDialogData->pScriptDependecies);
            return TRUE;

         // [REFRESH SCRIPT DEPENDENCIES]
         case IDM_DEPENDENCIES_REFRESH:      // menu item
         case IDC_REFRESH_DEPENDENCIES:      // button
            CONSOLE_MENU(IDM_DEPENDENCIES_REFRESH);
            updateScriptDependenciesPage_List(pDialogData, hPage);
            return TRUE;

         // [SEARCH SCRIPT-CALLS]
         case IDC_SEARCH_DEPENDENCIES:
            CONSOLE_MENU(IDC_SEARCH_DEPENDENCIES);
            onDependenciesPage_PerformSearch(pDialogData->pScriptDocument, hPage);
            return TRUE;
         }
         break;

      /// [CONTEXT MENU]
      case WM_CONTEXTMENU:
         return onDependenciesPage_ContextMenu(pDialogData->pScriptDocument, (HWND)wParam, LOWORD(lParam), HIWORD(lParam));

      /// [NOTIFICATION] - Handle ListView notifications
      case WM_NOTIFY:
         pMessage = (NMHDR*)lParam;

         // Examine notification
         switch (pMessage->code)
         {
         // [REQUEST DEPENDENCY DATA] Supply ScriptDependency/VariableDependency data
         case LVN_GETDISPINFO:
            onDependenciesPage_RequestData(pDialogData, hPage, (NMLVDISPINFO*)lParam);
            return TRUE;

         // [CUSTOM DRAW] Draw Refresh button and ListView
         case NM_CUSTOMDRAW:
            switch (pMessage->idFrom)
            {
            case IDC_REFRESH_DEPENDENCIES:
               onCustomDrawButton(hPage, pMessage, ITS_MEDIUM, TEXT("REFRESH_ICON"), TRUE); 
               return TRUE;

            case IDC_SEARCH_DEPENDENCIES:
               onCustomDrawButton(hPage, pMessage, ITS_MEDIUM, TEXT("SCRIPT_CALLER_ICON"), TRUE); 
               return TRUE;
            
            case IDC_DEPENDENCIES_LIST:
               SetWindowLong(hPage, DWL_MSGRESULT, onCustomDrawListView(hPage, pMessage->hwndFrom, (NMLVCUSTOMDRAW*)pMessage));
               return TRUE;
            }
            break;
         }
         break;

      /// [THEME CHANGED] Update ListView background colour
      case WM_THEMECHANGED:
         ListView_SetBkColor(GetControl(hPage, IDC_DEPENDENCIES_LIST), getThemeSysColour(TEXT("TAB"), COLOR_WINDOW));
         return TRUE;
      }
   }
   /// [EXCEPTION HANDLER]
   CATCH3("iMessage=%s  wParam=%d  lParam=%d", identifyMessage(iMessage), wParam, lParam);

   // Pass to base proc
   return dlgprocPropertiesPage(hPage, iMessage, wParam, lParam, PP_SCRIPT_DEPENDENCIES);
}



/// Function name  : dlgprocStringsPage
// Description     : Dialog procedure for the 'GameString Dependencies' properties dialog page
//
// 
INT_PTR   dlgprocStringsPage(HWND  hPage, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   PROPERTIES_DATA*  pDialogData;
   NMHDR*            pMessage;

   TRY
   {
      // Get properties dialog data
      pDialogData = getPropertiesDialogData(hPage);

      // Examine message
      switch (iMessage)
      {
      /// [COMMAND PROCESSING]
      case WM_COMMAND:
         switch (LOWORD(wParam))
         {
         // [REFRESH VARIABLE DEPENDENCIES]
         case IDM_STRINGS_REFRESH:      // menu item
         case IDC_REFRESH_STRINGS:      // button
            updateScriptStringsPage_List(pDialogData, hPage);
            return TRUE;
         }
         break;

      /// [CONTEXT MENU]
      case WM_CONTEXTMENU:
         return onDependenciesPage_ContextMenu(pDialogData->pScriptDocument, (HWND)wParam, LOWORD(lParam), HIWORD(lParam));

      /// [NOTIFICATION] - Handle ListView notifications
      case WM_NOTIFY:
         // Prepare
         pMessage = (NMHDR*)lParam;

         // Examine notification
         switch (pMessage->code)
         {
         // [REQUEST DEPENDENCY DATA] Supply GameString data
         case LVN_GETDISPINFO:
            onStringsPage_RequestData(pDialogData, hPage, (NMLVDISPINFO*)lParam);
            return TRUE;

         // [CUSTOM DRAW] Draw Refresh button and ListView
         case NM_CUSTOMDRAW:
            switch (pMessage->idFrom)
            {
            case IDC_REFRESH_STRINGS:
               onCustomDrawButton(hPage, pMessage, ITS_MEDIUM, TEXT("REFRESH_ICON"), TRUE); 
               return TRUE;
            
            case IDC_STRINGS_LIST:
               SetWindowLong(hPage, DWL_MSGRESULT, onCustomDrawListView(hPage, pMessage->hwndFrom, (NMLVCUSTOMDRAW*)pMessage));
               return TRUE;
            }
            break;
         }
         break;

      /// [THEME CHANGED] Update ListView background colour
      case WM_THEMECHANGED:
         ListView_SetBkColor(GetControl(hPage, IDC_STRINGS_LIST), getThemeSysColour(TEXT("TAB"), COLOR_WINDOW));
         return TRUE;
      }

      // Pass to base proc
      return dlgprocPropertiesPage(hPage, iMessage, wParam, lParam, PP_SCRIPT_STRINGS);
   }
   /// [EXCEPTION HANDLER]
   CATCH3("iMessage=%s  wParam=%d  lParam=%d", identifyMessage(iMessage), wParam, lParam);

   // Pass to base proc
   return dlgprocPropertiesPage(hPage, iMessage, wParam, lParam, PP_SCRIPT_STRINGS);
}


/// Function name  : dlgprocVariablesPage
// Description     : Window procedure for the 'VariableDependencies' properties dialog page
//
// 
INT_PTR   dlgprocVariablesPage(HWND  hPage, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   PROPERTIES_DATA*  pDialogData;
   NMHDR*            pMessage;

   TRY
   {
      // Get properties dialog data
      pDialogData = getPropertiesDialogData(hPage);

      switch (iMessage)
      {
      /// [COMMAND PROCESSING]
      case WM_COMMAND:
         switch (LOWORD(wParam))
         {
         // [REFRESH VARIABLE DEPENDENCIES]
         case IDM_VARIABLES_REFRESH:      // menu item
         case IDC_REFRESH_VARIABLES:      // button
            updateScriptVariablesPage_List(pDialogData, hPage);
            return TRUE;

         // [PROJECT VARIABLES]
         case IDC_PROJECT_VARIABLES:
            displayProjectVariablesDialog(pDialogData->hSheetDlg);
            return TRUE;
         }
         break;

      /// [CONTEXT MENU]
      case WM_CONTEXTMENU:
         return onDependenciesPage_ContextMenu(pDialogData->pScriptDocument, (HWND)wParam, LOWORD(lParam), HIWORD(lParam));

      /// [NOTIFICATION] - Handle ListView notifications
      case WM_NOTIFY:
         pMessage = (NMHDR*)lParam;

         // Examine notification
         switch (pMessage->code)
         {
         // [REQUEST DEPENDENCY DATA] Supply VariableDependency data
         case LVN_GETDISPINFO:
            onVariablesPage_RequestData(pDialogData, hPage, (NMLVDISPINFO*)lParam);
            return TRUE;

         // [CUSTOM DRAW] Draw Refresh button and ListView
         case NM_CUSTOMDRAW:
            switch (pMessage->idFrom)
            {
            case IDC_REFRESH_VARIABLES:
               onCustomDrawButton(hPage, pMessage, ITS_MEDIUM, TEXT("REFRESH_ICON"), TRUE); 
               return TRUE;

            case IDC_PROJECT_VARIABLES:
               onCustomDrawButton(hPage, pMessage, ITS_MEDIUM, TEXT("VARIABLE_ICON"), TRUE); 
               return TRUE;

            case IDC_VARIABLES_LIST:
               SetWindowLong(hPage, DWL_MSGRESULT, onCustomDrawListView(hPage, pMessage->hwndFrom, (NMLVCUSTOMDRAW*)pMessage));
               return TRUE;
            }
            break;
         }
         break;

      /// [THEME CHANGED] Update ListView background colour
      case WM_THEMECHANGED:
         ListView_SetBkColor(GetControl(hPage, IDC_VARIABLES_LIST), getThemeSysColour(TEXT("TAB"), COLOR_WINDOW));
         return TRUE;
      }

      // Pass to base proc
      return dlgprocPropertiesPage(hPage, iMessage, wParam, lParam, PP_SCRIPT_VARIABLES);
   }
   /// [EXCEPTION HANDLER]
   CATCH3("iMessage=%s  wParam=%d  lParam=%d", identifyMessage(iMessage), wParam, lParam);

   // Pass to base proc
   return dlgprocPropertiesPage(hPage, iMessage, wParam, lParam, PP_SCRIPT_VARIABLES);
}
