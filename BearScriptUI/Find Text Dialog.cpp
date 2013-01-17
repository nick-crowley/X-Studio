//
// Insert Argument Dialog.cpp : The 'new argument' dialog for the 'Arguments' document properties
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

/// Function name  : displayFindTextDialog
// Description     : Displays the FindText dialog
// 
// HWND  hParentWnd : [in] Parent window
// 
// Return Value   : Window handle if succesful, otherwise NULL
// 
HWND   displayFindTextDialog(HWND  hParentWnd)
{
   HWND  hDialog;

   /// Create FindText dialog
   hDialog = CreateDialog(getResourceInstance(), TEXT("FIND_TEXT_DIALOG"), hParentWnd, dlgprocFindTextDialog);
   ERROR_CHECK("Creating FindText dialog", hDialog);

   // [CHECK] Ensure dialog was created succesfully
   if (hDialog)
   {
      // [SUCCESS] Display dialog
      ShowWindow(hDialog, SW_SHOW);
      UpdateWindow(hDialog);
   }

   // Return window handle
   return hDialog;
}


/// Function name  : initFindTextDialog
// Description     : 
// 
// HWND  hDialog : [in] 
// 
VOID   initFindTextDialog(HWND  hDialog)
{
   TCHAR    *szNewLine,
            *szSelection;    // Currently selected text

   // [CAPTION] 
   SetWindowText(hDialog, getAppName());

   // [POSITION] Set last position
   SetWindowPos(hDialog, NULL, getAppPreferencesWindowRect(AW_FIND)->left, getAppPreferencesWindowRect(AW_FIND)->top, NULL, NULL, SWP_NOSIZE WITH SWP_NOZORDER);

   // [CHECK] Is there a current text selection?
   if (szSelection = CodeEdit_GetSelectionText(getActiveScriptCodeEdit()))
   {
      // [CHECK] Truncate at first line break
      if (szNewLine = utilFindCharacterInSet(szSelection, "\r\n"))
         szNewLine[0] = NULL;

      // [SELECTION] Display text currently selected
      SetDlgItemText(hDialog, IDC_FIND_SEARCH_EDIT, szSelection);
      utilDeleteString(szSelection);
   }
   else
      // [NO SELECTION] Display last search text
      SetDlgItemText(hDialog, IDC_FIND_SEARCH_EDIT, getAppPreferences()->szFindText);

   // Prevent EditCtrl from initially selecting text
   SubclassWindow(GetControl(hDialog, IDC_FIND_SEARCH_EDIT), wndprocCustomEditControl);
   
   // [OWNER DRAW]
   utilAddWindowStyle(GetControl(hDialog, IDC_DIALOG_ICON), SS_OWNERDRAW);
   utilAddWindowStyle(GetControl(hDialog, IDC_DIALOG_TITLE), SS_OWNERDRAW);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onFindTextDialogFindText
// Description     : Invokes the search/replace operation using the current settings
// 
// HWND                         hDialog : [in] Dialog
// CONST CODE_EDIT_SEARCH_FLAG  eAction : [in] Whether to search or replace
// 
VOID  onFindTextDialogFindText(HWND  hDialog, CONST CODE_EDIT_SEARCH_FLAG  eAction)
{
   CODE_EDIT_SEARCH       oSearchData;
   SCRIPT_DOCUMENT*       pDocument;
   BOOL                   bMatchFound;

   // [CHECK] Ensure document and search phrase exist
   if ((pDocument = getActiveScriptDocument()) AND utilGetDlgItemTextLength(hDialog, IDC_FIND_SEARCH_EDIT))
   {
      // Prepare
      oSearchData.szSearch  = utilGetDlgItemText(hDialog, IDC_FIND_SEARCH_EDIT);
      oSearchData.szReplace = utilGetDlgItemText(hDialog, IDC_FIND_REPLACE_EDIT);
      // [CHECK] Set 'Find from Top' and 'Case sensitive' options
      oSearchData.eFlags    = eAction WITH (IsDlgButtonChecked(hDialog, IDC_FIND_FROM_TOP_CHECK) ? CSF_FROM_TOP : CSF_FROM_CARET);
      oSearchData.eFlags   |= (IsDlgButtonChecked(hDialog, IDC_FIND_CASE_SENSITIVE_CHECK) ? CSF_CASE_SENSITIVE : NULL);

      /// [SEARCH] Find/replace and select next match
      bMatchFound = CodeEdit_FindText(pDocument->hCodeEdit, &oSearchData);

      // Display 'no more matches' if no match was found
      utilShowDlgItem(hDialog, IDC_FIND_COMPLETE_STATIC, !bMatchFound);
      // Untick 'Search from top' if a match was found
      CheckDlgButton(hDialog, IDC_FIND_FROM_TOP_CHECK, !bMatchFound);

      // Cleanup
      utilDeleteStrings(oSearchData.szSearch, oSearchData.szReplace);
   }
}

/// Function name  : onFindTextDialogCommand
// Description     : WM_COMMAND processing for the 'Insert Argument' dialog
// 
// HWND          hDialog        : [in] Window handle of the 'Insert Argument' dialog
// CONST UINT    iControlID     : [in] ID of the control sending the command
// CONST UINT    iNotification  : [in] Notification code
// HWND          hCtrl          : [in] Window handle of the control sending the command
// 
// Return Value   : TRUE if processed, FALSE otherwise
// 
BOOL   onFindTextDialogCommand(HWND  hDialog, CONST UINT  iControlID, CONST UINT  iNotification, HWND  hCtrl)
{
   TCHAR*   szSearchText;
   BOOL     bResult;

   // Prepare
   bResult = TRUE;

   // Examine sender
   switch (iControlID)
   {
   /// [SEARCH CHANGED] Enable/disable 'Find Next'
   case IDC_FIND_SEARCH_EDIT:
      if (bResult = (iNotification == EN_CHANGE))
         utilEnableDlgItem(hDialog, IDC_FIND_NEXT, utilGetDlgItemTextLength(hDialog, IDC_FIND_SEARCH_EDIT) != 0);
      break;

   /// [REPLACE CHANGED] Enable/disable 'Replace' 
   case IDC_FIND_REPLACE_EDIT:
      if (bResult = (iNotification == EN_CHANGE))
         utilEnableDlgItem(hDialog, IDC_FIND_REPLACE, utilGetDlgItemTextLength(hDialog, IDC_FIND_REPLACE_EDIT) != 0);
      break;

   /// [FIND/REPLACE] Search for next match
   case IDC_FIND_NEXT:     onFindTextDialogFindText(hDialog, CSF_SEARCH);   break;
   case IDC_FIND_REPLACE:  onFindTextDialogFindText(hDialog, CSF_REPLACE);  break;

   /// [CANCEL] Close dialog and zero window handle
   case IDCANCEL:
      // Save position to preferences
      GetWindowRect(hDialog, getAppPreferencesWindowRect(AW_FIND));

      // Save search to preferences
      setAppPreferencesLastFindText(szSearchText = utilGetDlgItemText(hDialog, IDC_FIND_SEARCH_EDIT));
      utilDeleteString(szSearchText);

      // Close dialog
      EndDialog(hDialog, iControlID);
      getMainWindowData()->hFindTextDlg = NULL;
      break;

   default:
      bResult = FALSE;
      break;
   }

   // Return result
   return bResult;
}


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : dlgprocFindTextDialog
// Description     : Window procedure for the 'Insert Argument' dialog
// 
// 
INT_PTR  dlgprocFindTextDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   BOOL   bResult;

   // Prepare
   bResult = FALSE;

   // Examine message
   switch (iMessage)
   {
   /// [CREATION] -- Setup dialog
   case WM_INITDIALOG:
      // Init dialog
      initFindTextDialog(hDialog);
      bResult = TRUE;
      break;

   /// [COMMAND PROCESSING] -- Process name change, OK and CANCEL
   case WM_COMMAND:
      bResult = onFindTextDialogCommand(hDialog, LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
      break;

   /// [ENABLED]
   case WM_ENABLE:
      // [CHECK] Ensure main window is in the same state
      if (IsWindowEnabled(getAppWindow()) != wParam)
         EnableWindow(getAppWindow(), wParam);

      // [CHECK] Ensure Properties dialog is in the same state
      if (getMainWindowData()->hPropertiesSheet AND IsWindowEnabled(getMainWindowData()->hPropertiesSheet) != wParam)
         EnableWindow(getMainWindowData()->hPropertiesSheet, wParam);
      break;

   /// [OWNER DRAW]
   case WM_DRAWITEM:
      if (wParam == IDC_DIALOG_ICON)
         bResult = onOwnerDrawStaticIcon(lParam, TEXT("FIND_TEXT_ICON"), 96);
      break;
   }

   // Return result
   return (bResult ? TRUE : dlgprocVistaStyleDialog(hDialog, iMessage, wParam, lParam));
}

