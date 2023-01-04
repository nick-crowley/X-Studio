//
// Search Tab Dialog.cpp : Hosts the CommandList, GameObject and ScriptObject search dialogs
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

// Debug
static const TCHAR*  szPageNames[4] = { TEXT("Commands"), TEXT("Game Objects"), TEXT("Script Objects"), TEXT("None") };

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createSearchDialog
// Description     : Creates a new SearchTab dialog
// 
// HWND               hParentWnd     : [in] Parent window
// CONST RESULT_TYPE  eInitialDialog : [in] Initial results dialog to display
// 
// Return Value   : Window handle of the new dialog if successful, otherwise NULL
// 
HWND  createSearchDialog(HWND  hParentWnd, CONST RESULT_TYPE  eInitialDialog)
{
   SEARCH_DIALOG_DATA*  pDialogData;   // New dialog data

   TRY
   {
      // Prepare
      pDialogData = createSearchDialogData(eInitialDialog);

      // Create dialog and pass it data
      pDialogData->hDialog = loadDialog(TEXT("SEARCH_DIALOG"), hParentWnd, dlgprocSearchDialog, (LPARAM)pDialogData);
      ERROR_CHECK("creating search container dialog", pDialogData->hDialog);

      // [DEBUG]
      SetWindowText(pDialogData->hDialog, TEXT("Search Tab Dialog"));
      //DEBUG_WINDOW("Search Tab Dialog", pDialogData->hDialog);

      // Return window handle or NULL
      return pDialogData->hDialog;
   }
   CATCH1("Unable to create Search dialog with initial tab '%s'", szPageNames[eInitialDialog]);
   return NULL;
}


/// Function name  : createSearchDialogData
// Description     : Creates data for the SearchTab dialog
//
// CONST RESULT_TYPE  eInitialDialog : [in] Initial results dialog to display
// 
// Return Value   : New SEARCH_DIALOG_DATA, you are responsible for destroying it
// 
SEARCH_DIALOG_DATA*   createSearchDialogData(CONST RESULT_TYPE  eInitialDialog)
{
   SEARCH_DIALOG_DATA*   pDialogData = utilCreateEmptyObject(SEARCH_DIALOG_DATA);       // Object being created
   
   // Set properties
   pDialogData->eActiveDialog = eInitialDialog;
   return pDialogData;
}


/// Function name  : deleteSearchDialogData
// Description     : Destroys the dialog data for the SearchTab dialog
// 
// SEARCH_DIALOG_DATA*  &pDialogData : [in] SearchTab dialog data to destroy
// 
VOID  deleteSearchDialogData(SEARCH_DIALOG_DATA*  &pDialogData)
{
   // Destroy contents

   // Destroy calling object
   utilDeleteObject(pDialogData);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : getSearchDialogData
// Description     : Retrieves SearchDialog dialog data
// 
// HWND  hDialog   : [in] SearchDialog window handle
// 
// Return Value   : Dialog data
// 
SEARCH_DIALOG_DATA*    getSearchDialogData(HWND  hDialog)
{
   return (SEARCH_DIALOG_DATA*)GetWindowLong(hDialog, DWL_USER);
}


/// Function name  : identifyActiveResultsDialog
// Description     : Identifies which tab/dialog is currently visible
// 
// HWND  hSearchDialog : [in] Search tab dialog window handle
// 
// Return Value   : ID of the current ResultsDialog if any, otherwise RT_NONE
// 
RESULT_TYPE  identifyActiveResultsDialog(HWND  hSearchDialog)
{
   SEARCH_DIALOG_DATA*  pDialogData = getSearchDialogData(hSearchDialog);

   // Return active dialog or RT_NONE
   return pDialogData ? pDialogData->eActiveDialog : RT_NONE;
}


/// Function name  : identifyActiveResultsDialogHandle
// Description     : Identifies which tab/dialog is currently visible
// 
// HWND  hSearchDialog : [in] Search tab dialog window handle
// 
// Return Value   : ID of the current ResultsDialog if any, otherwise RT_NONE
// 
HWND  identifyActiveResultsDialogHandle(HWND  hSearchDialog)
{
   SEARCH_DIALOG_DATA*  pDialogData = getSearchDialogData(hSearchDialog);

   // Return active dialog or RT_NONE
   return (pDialogData AND pDialogData->eActiveDialog != RT_NONE ? pDialogData->hChildren[pDialogData->eActiveDialog] : NULL);
}


/// Function name  : identifyResultsDialogFilterByType
// Description     : Identifies which filter is selected in a ResultsDialog
// 
// HWND               hSearchDialog : [in] Search dialog
// CONST RESULT_TYPE  eDialog       : [in] Results dialog ID
// 
// Return Value   : Item index of selected filter
// 
UINT  identifyResultsDialogFilterByType(HWND  hSearchDialog, CONST RESULT_TYPE  eDialog)
{
   SEARCH_DIALOG_DATA*   pDialogData = getSearchDialogData(hSearchDialog);     // Dialog data

   /// Return current filter
   return pDialogData ? identifyResultsDialogFilter(pDialogData->hChildren[eDialog]) : 0;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : setActiveResultsDialogByType
// Description     : Displays the desired search results dialog within the search tab dialog
// 
// HWND               hSearchDialog : [in] SearchDialog window handle
// CONST RESULT_TYPE  eDialog       : [in] Dialog to display
// 
// Return Value   : TRUE if the dialog was changed, otherwise FALSE
// 
BOOL  setActiveResultsDialogByType(HWND  hSearchDialog, CONST RESULT_TYPE  eDialog)
{
   SEARCH_DIALOG_DATA*   pDialogData = getSearchDialogData(hSearchDialog);     // Dialog data

   TRY
   {
      CONSOLE_COMMAND();

      /// [CHECK] Ensure dialog is not already active
      if (IsWindow(hSearchDialog) AND eDialog != pDialogData->eActiveDialog)
      {
         CONSOLE("Displaying results dialog for '%s'", szPageNames[eDialog]);

         // Hide existing dialog
         ShowWindow(pDialogData->hChildren[pDialogData->eActiveDialog], SW_HIDE);

         /// Select new tab and display new dialog
         TabCtrl_SetCurSel(pDialogData->hTabCtrl, eDialog);
         ShowWindow(pDialogData->hChildren[eDialog], SW_SHOW);

         // Update active tab
         pDialogData->eActiveDialog = eDialog;
         SetFocus(GetDlgItem(pDialogData->hChildren[eDialog], IDC_RESULTS_SEARCH));

         // [CHANGED] Return TRUE
         return TRUE;
      }

      // [NO CHANGE] Return FALSE
      return FALSE;
   }
   CATCH1("Unable to display results dialog for '%s'", szPageNames[eDialog]);
   return FALSE;
}


/// Function name  : initSearchDialog
// Description     : Intialises the search tab dialog
// 
// SEARCH_DIALOG_DATA*  pDialogData : [in] Dialog data
// HWND                 hDialog     : [in] New search tab dialog handle
// HWND                 hFirstCtrl  : [in] Control to receive the input focus
// 
// Return Value : TRUE if dialogs created successfully, FALSE if not
//
BOOL  initSearchDialog(SEARCH_DIALOG_DATA*  pDialogData, HWND  hDialog, HWND  hFocusCtrl)
{
   TCITEM   oTabItem;    // Tab item data
   HDC      hDC;         // DC used for creating font
   
   TRY
   {
      /// Store dialog data
      pDialogData->hDialog  = hDialog;
      pDialogData->hTabCtrl = GetDlgItem(hDialog, IDC_SEARCH_TAB);
      SetWindowLong(hDialog, DWL_USER, (LONG)pDialogData);

      /// Set custom tab title font  (in bold)
      pDialogData->hTabFont = utilCreateFont(hDC = GetDC(hDialog), TEXT("MS Sans Serif"), 9, TRUE, FALSE, FALSE);
      SetWindowFont(pDialogData->hTabCtrl, pDialogData->hTabFont, FALSE);
      
      /// Set custom tab ImageList  (20x20 pixels)
      pDialogData->hImageList = utilCreateImageList(getResourceInstance(), 20, 3, "COMMAND_LIST_ICON", "GAME_OBJECT_ICON", "SCRIPT_OBJECT_ICON");
      TabCtrl_SetImageList(pDialogData->hTabCtrl, pDialogData->hImageList);

      // Iterate through result types
      for (RESULT_TYPE  eDialog = RT_COMMANDS; eDialog <= RT_SCRIPT_OBJECTS; eDialog = (RESULT_TYPE)(eDialog + 1))
      {
         // Prepare
         oTabItem.iImage  = eDialog;
         oTabItem.pszText = (TCHAR*)loadTempString(IDS_SEARCH_COMMANDS + eDialog);
         oTabItem.mask    = TCIF_TEXT WITH TCIF_IMAGE;

         /// Create a tab and dialog
         TabCtrl_InsertItem(pDialogData->hTabCtrl, eDialog, &oTabItem);
         pDialogData->hChildren[eDialog] = createResultsDialog(hDialog, eDialog);
         BringWindowToTop(pDialogData->hChildren[eDialog]);
      }      

      /// Select initial tab and display initial dialog
      TabCtrl_SetCurSel(pDialogData->hTabCtrl, pDialogData->eActiveDialog);
      ShowWindow(pDialogData->hChildren[pDialogData->eActiveDialog], SW_SHOW);

      // Cleanup and return
      ReleaseDC(hDialog, hDC);
      return TRUE;
   }
   CATCH0("Unable to initialise search dialog");
   return FALSE;
}


/// Function name  : updateSearchDialog
// Description     : Refresh the current results dialog
// 
// SEARCH_DIALOG_DATA*   pDialogData : [in] Dialog data
// 
VOID  updateSearchDialog(SEARCH_DIALOG_DATA*  pDialogData)
{
   TRY
   {
      /// Refresh active ResultsDialog
      updateResultsDialog(pDialogData->hChildren[pDialogData->eActiveDialog]);
   }
   CATCH0("Unable to update Results Dialog");
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onSearchDialog_AppStateChanged
// Description     : Refresh the current results dialog
// 
// SEARCH_DIALOG_DATA*      pDialogData : [in] 
// CONST APPLICATION_STATE  eNewState   : [in] 
// 
VOID  onSearchDialog_AppStateChanged(SEARCH_DIALOG_DATA*  pDialogData, CONST APPLICATION_STATE  eNewState)
{
   CONSOLE_EVENT();

   // Refresh current dialog
   updateSearchDialog(pDialogData);
}


/// Function name  : onSearchDialog_Destroy
// Description     : Destroys the results dialogs and the dialog data
// 
// SEARCH_DIALOG_DATA* pDialogData   : [in] dialog data
// 
VOID  onSearchDialog_Destroy(SEARCH_DIALOG_DATA*  pDialogData)
{
   TRY
   {
      // Sever dialog data
      SetWindowLong(pDialogData->hDialog, DWL_USER, NULL);

      /// Delete child dialogs
      for (RESULT_TYPE  eDialog = RT_COMMANDS; eDialog <= RT_SCRIPT_OBJECTS; eDialog = (RESULT_TYPE)(eDialog + 1))
         // [CHECK] May not exist if failed to create
         utilSafeDeleteWindow(pDialogData->hChildren[eDialog]);

      // Delete tab ImageList
      TabCtrl_SetImageList(pDialogData->hTabCtrl, NULL);
      ImageList_Destroy(pDialogData->hImageList);

      // Delete tab font
      SetWindowFont(pDialogData->hTabCtrl, NULL, FALSE);
      DeleteFont(pDialogData->hTabFont);

      /// Delete dialog data
      deleteSearchDialogData(pDialogData);
   }
   CATCH0("Unable to destroy SearchDialog");
}


/// Function name  : onSearchDialog_DocumentSwitched
// Description     : Enables/disables search box and updates results according to new document game version
// 
// SEARCH_DIALOG_DATA*  pDialogData   : [in] Dialog data
// DOCUMENT*            pNewDocument  : [in] New document if any, otherwise NULL
// 
VOID  onSearchDialog_DocumentSwitched(SEARCH_DIALOG_DATA*  pDialogData, DOCUMENT*  pNewDocument)
{
   // [TRACK]
   CONSOLE_EVENT1(getDocumentFileName(pNewDocument));
   CONSOLE("Refreshing search dialog in response to new document '%s'", pNewDocument ? pNewDocument->szFullPath : NULL);

   // Update current ResultsDialog
   updateSearchDialog(pDialogData);
}


/// Function name  : onSearchDialog_DocumentPropertyUpdated
// Description     : Updates the results to reflect new game version
// 
// SEARCH_DIALOG_DATA*  pDialogData  : [in] Dialog data
// CONST UINT           iControlID   : [in] ID of the control/property that changed
// 
VOID  onSearchDialog_DocumentPropertyUpdated(SEARCH_DIALOG_DATA*  pDialogData, CONST UINT  iControlID)
{
   // [CHECK] Has the 'GameVersion' property just been changed?
   if (iControlID == IDC_SCRIPT_ENGINE_VERSION)
   {
      CONSOLE_EVENT();
      CONSOLE("Refreshing search dialog in response to engine version change");

      /// [SUCCESS] Refresh current ResultsDialog
      updateSearchDialog(pDialogData);
   }
}


/// Function name  : onSearchDialog_SelectionChange
// Description     : Displays a new tab in response to the user selecting a tab manually
// 
// SEARCH_DIALOG_DATA*  pDialogData : [in] Dialog data
// 
VOID  onSearchDialog_SelectionChange(SEARCH_DIALOG_DATA*  pDialogData)
{
   TUTORIAL_WINDOW  eTutorialWindows[3] = { TW_SCRIPT_COMMANDS, TW_GAME_OBJECTS, TW_SCRIPT_OBJECTS }; // Tutorial windows for each ResultsDialog

   TRY
   {
      CONSOLE_EVENT();

      // Display new ResultsDialog
      setActiveResultsDialogByType(pDialogData->hDialog, (RESULT_TYPE)TabCtrl_GetCurSel(pDialogData->hTabCtrl));

      // [TUTORIAL] Delay display the search dialog tutorial
      setMainWindowTutorialTimer(getMainWindowData(), eTutorialWindows[TabCtrl_GetCurSel(pDialogData->hTabCtrl)], TRUE);

      // [TOOLBAR] Update main window toolbar
      updateMainWindowToolBar(getMainWindowData());
   }
   CATCH0("Unable to activate new Results dialog");
}


/// Function name  : onSearchDialog_Resize
// Description     : Stretches the tab control over the entire dialog
// 
// SEARCH_DIALOG_DATA*  pDialogData : [in] Dialog data
// CONST SIZE*          pNewSize    : [in] New dialog size
// 
VOID  onSearchDialog_Resize(SEARCH_DIALOG_DATA*  pDialogData, CONST SIZE*  pNewSize)
{
   RECT   rcClient;    // Client rectangle

   TRY
   {
      /// Stretchtab control over entire dialog
      GetClientRect(pDialogData->hDialog, &rcClient);
      utilSetWindowRect(pDialogData->hTabCtrl, &rcClient, TRUE);

      // Calculate the appropriate display rectangle for the results dialog
      GetClientRect(pDialogData->hTabCtrl, &rcClient);
      TabCtrl_AdjustRect(pDialogData->hTabCtrl, FALSE, &rcClient);

      // Shrink to create border
      InflateRect(&rcClient, -6, -6);

      /// Re-size each child dialog
      for (RESULT_TYPE  eDialog = RT_COMMANDS; eDialog <= RT_SCRIPT_OBJECTS; eDialog = (RESULT_TYPE)(eDialog + 1))
         utilSetWindowRect(pDialogData->hChildren[eDialog], &rcClient, TRUE);
   }
   CATCH2("Unable to resize search dialog to (%d,%d)", pNewSize->cx, pNewSize->cy);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : dlgprocSearchDialog
// Description     : Window procedure for the SearchTabDailog
// 
// 
INT_PTR   dlgprocSearchDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   SEARCH_DIALOG_DATA*  pDialogData;       // dialog data
   NMHDR*               pHeader;           // Notification header
   SIZE                 siDialog;          // Dialog size
   BOOL                 bResult = TRUE;    // Operation result

   TRY
   {
      // Prepare
      pDialogData = getSearchDialogData(hDialog);

      // Examine message
      switch (iMessage)
      {
      /// [CREATE] Store dialog data
      case WM_INITDIALOG:
         initSearchDialog((SEARCH_DIALOG_DATA*)lParam, hDialog, (HWND)wParam);
         break;

      /// [DESTROY] Cleanup the dialog data
      case WM_DESTROY:
         onSearchDialog_Destroy(pDialogData);
         break;

      /// [NOTIFICATION]
      case WM_NOTIFY:
         // Prepare
         pHeader = (NMHDR*)lParam;

         /// [TAB SELECTION CHANGED]
         if (pHeader->idFrom == IDC_SEARCH_TAB AND pHeader->code == TCN_SELCHANGE)
            onSearchDialog_SelectionChange(pDialogData);
         else
            // [UNHANDLED] Return FALSE
            bResult = FALSE;
         break;

      /// [RESIZE]
      case WM_SIZE:
         siDialog.cx = LOWORD(lParam);
         siDialog.cy = HIWORD(lParam);
         onSearchDialog_Resize(pDialogData, &siDialog);
         break;

      // [APP STATE CHANGED]
      case UN_APP_STATE_CHANGED:
         onSearchDialog_AppStateChanged(pDialogData, (APPLICATION_STATE)wParam);
         break;

      // [DOCUMENT SWITCHED]
      case UN_DOCUMENT_SWITCHED:
         onSearchDialog_DocumentSwitched(pDialogData, (DOCUMENT*)lParam);
         break;

      // [DOCUMENT PROPERTY UPDATED]
      case UN_PROPERTY_UPDATED:
         onSearchDialog_DocumentPropertyUpdated(pDialogData, wParam);
         break;

      // [PREFERENCES CHANGED] Advice each dialog
      case UN_PREFERENCES_CHANGED:
         for (RESULT_TYPE  eDialog = RT_COMMANDS; eDialog <= RT_SCRIPT_OBJECTS; eDialog = (RESULT_TYPE)(eDialog + 1))
            Preferences_Changed(pDialogData->hChildren[eDialog]);
         break;

      // [UNHANDLED] Return FALSE
      default:
         bResult = FALSE;
         break;
      }

      // Return result
      return bResult;
   }
   /// [EXCEPTION HANDLER]
   CATCH3("iMessage=%s  wParam=%d  lParam=%d", identifyMessage(iMessage), wParam, lParam);
   return FALSE;
}

