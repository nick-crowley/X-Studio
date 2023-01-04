//
// Bug Report Dialog.cpp : Implements the 'Submit Bug Report' dialog
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C WITH /C WITH ++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : displayBugReportDialog
// Description     : Displays the BugReportDialog and returns the user input
// 
// HWND  hParentWnd   : [in] Parent window
// 
// Return Value   : IDOK or IDCANCEL
// 
BOOL   displayBugReportDialog(HWND  hParent, CONST BOOL  bManualInvoke)
{
   // [VERBOSE]
   CONSOLE_ACTION_BOLD();

   // Display modal dialog box and return result
   return showDialog(TEXT("BUG_REPORT_DIALOG"), utilEither(hParent, utilGetTopWindow(getAppWindow())), dlgprocBugReportDialog, bManualInvoke);
}


/// Function name  : initBugReportDialog
// Description     : Displays the types of errors encountered and the user's forum username
// 
// HWND        hDialog       : [in] Bug Report Dialog window handle
// CONST BOOL  bManualInvoke : [in] TRUE if dialog was manually invoked from menu, FALSE if invoked due to an error
// 
VOID   initBugReportDialog(HWND  hDialog, CONST BOOL  bManualInvoke)
{
   /// Populate error causes
   for (APPLICATION_ERROR  eErrorType = AE_ACCESS_VIOLATION; eErrorType <= AE_LOAD_SCRIPT; eErrorType = (APPLICATION_ERROR)(eErrorType + 1))
      // Check/Uncheck associated CheckBox
      CheckDlgButton(hDialog, (IDC_SUBMISSION_ACCESS_VIOLATION_CHECK + eErrorType), getAppError(eErrorType));

   // Limit forum name to 32 characters
   SendDlgItemMessage(hDialog, IDC_SUBMISSION_FORUM_USERNAME_EDIT, EM_LIMITTEXT, 32, NULL);

   /// Display existing forum name (if any) and enable OK button
   SetDlgItemText(hDialog, IDC_SUBMISSION_FORUM_USERNAME_EDIT, getAppPreferences()->szForumUserName);

   /// Enable/Disable OK button if forum name has been provided
   utilEnableDlgItem(hDialog, IDOK, lstrlen(getAppPreferences()->szForumUserName));

   // [MANUAL INVOKE] Display slightly different description
   if (bManualInvoke)
      SetDlgItemText(hDialog, IDC_SUBMISSION_DESCRIPTION_STATIC, loadTempString(IDS_GENERAL_BUG_REPORT_MANUAL));

   // [DIALOG] Center dialog and set text
   utilCentreWindow(getAppWindow(), hDialog);
   SetWindowText(hDialog, getAppName());

   // [OWNER DRAW]
   utilAddWindowStyle(GetControl(hDialog, IDC_DIALOG_ICON), SS_OWNERDRAW);
   utilAddWindowStyle(GetControl(hDialog, IDC_DIALOG_TITLE), SS_OWNERDRAW);
   utilAddWindowStyle(GetControl(hDialog, IDC_DIALOG_HEADING_1), SS_OWNERDRAW);
   utilAddWindowStyle(GetControl(hDialog, IDC_DIALOG_HEADING_2), SS_OWNERDRAW);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onBugReportDialogCommand
// Description     : Handles the buttons in the dialog and ensures the user enters a forum username
// 
// HWND        hDialog        : [in] Bug Report Dialog window handle
// CONST UINT  iControlID     : [in] Source of the message
// CONST UINT  iNotification  : [in] Notification
// 
// Return Value   : TRUE if processed, otherwise FALSE
// 
BOOL   onBugReportDialogCommand(HWND  hDialog, CONST UINT  iControlID, CONST UINT  iNotification)
{
   BOOL     bResult;   // Whether message was handled or not

   // Prepare
   bResult = FALSE;

   // Examine sender
   switch (iControlID)
   {
   /// [OK] Store forum username, close dialog and return button ID
   case IDOK:
      onBugReportDialogOK(hDialog);
      // Fall through...

   /// [CANCEL] Close dialog and return button ID
   case IDCANCEL:
      EndDialog(hDialog, iControlID);
      bResult = TRUE;
      break;

   /// [VIEW REPORT] Launch notepad to view 'Console.log'
   case IDC_SUBMISSION_VIEW_REPORT:
      onBugReportDialogViewReport(hDialog);
      bResult = TRUE;
      break;

   /// [USERNAME CHANGE] Disable OK button if user has provided no forum username
   case IDC_SUBMISSION_FORUM_USERNAME_EDIT:
      // [TEXT CHANGED]
      if (iNotification == EN_CHANGE)
      {
         utilEnableDlgItem(hDialog, IDOK, utilGetDlgItemTextLength(hDialog, IDC_SUBMISSION_FORUM_USERNAME_EDIT) > 0);
         bResult = TRUE;
      }
      break;
   }

   // Return result
   return bResult;
}


/// Function name  : onBugReportDialogOK
// Description     : Stores the X-Universe forum username
// 
// HWND  hDialog   : [in] Bug Report Dialog window handle
// 
VOID  onBugReportDialogOK(HWND  hDialog)
{
   TCHAR   *szUserName;          // X-Forums username

   // Get forum username and ensure it's alpha-numeric
   szUserName = utilGetDlgItemText(hDialog, IDC_SUBMISSION_FORUM_USERNAME_EDIT);
   cleanSubmissionForumUserName(szUserName, lstrlen(szUserName));

   // Store in preferences
   setAppPreferencesForumUserName(szUserName);

   // Cleanup
   utilDeleteString(szUserName);
}


/// Function name  : onBugReportDialogViewReport
// Description     : Displays the console logfile in windows notepad
// 
// HWND  hDialog   : [in] Bug Report Dialog window handle
// 
VOID  onBugReportDialogViewReport(HWND  hDialog)
{
   TCHAR   *szViewerPath,        // Full path to 'notepad.exe'
           *szConsoleLogPath;    // Full path to 'console.log'
   int      iResult;

   // [VERBOSE]
   CONSOLE_ACTION();

   // Generate path to 'Notepad.exe'
   /*GetWindowsDirectory(szViewerPath = utilCreateEmptyPath(), MAX_PATH);
   PathAppend(szViewerPath, TEXT("notepad.exe"));*/

   // Generate path to 'ConsoleViewer.exe'
   szViewerPath = utilGenerateAppFilePath(TEXT("LogViewer.exe"));

   /// Launch Console.log in viewer
   switch (iResult = (int)ShellExecute(hDialog, TEXT("open"), szViewerPath, szConsoleLogPath = generateConsoleLogFilePath(), NULL, SW_SHOWNORMAL))
   {
   case ERROR_FILE_NOT_FOUND:     
   case ERROR_PATH_NOT_FOUND:     CONSOLE_ERROR2("Cannot find file '%s' or '%s'", szViewerPath, szConsoleLogPath);            break;
   default:  if (iResult <= 32)   CONSOLE_ERROR2("Unspecified error opening '%s' or '%s'", szViewerPath, szConsoleLogPath);   break;
   }

   // Cleanup
   utilDeleteStrings(szConsoleLogPath, szViewerPath);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : dlgprocBugReportDialog
// Description     : Dialog procedure for the BugReport Dialog
// 
// 
INT_PTR  dlgprocBugReportDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   BOOL   bResult;

   // Prepare
   bResult = TRUE;

   // Examine message
   switch (iMessage)
   {
   /// [CREATE] Initialise dialog
   case WM_INITDIALOG:
      initBugReportDialog(hDialog, (BOOL)lParam);
      break;

   /// [COMMAND]
   case WM_COMMAND:
      bResult = onBugReportDialogCommand(hDialog, LOWORD(wParam), HIWORD(wParam));
      break;

   /// [OWNERDRAW]
   case WM_DRAWITEM:
      switch (wParam)
      {
      case IDC_DIALOG_ICON:       onOwnerDrawStaticIcon(lParam, TEXT("SUBMIT_REPORT_ICON"), 96);    break;
      case IDC_DIALOG_TITLE:      onOwnerDrawStaticTitle(lParam);     break;
      case IDC_DIALOG_HEADING_1:     
      case IDC_DIALOG_HEADING_2:  onOwnerDrawStaticHeading(lParam);   break;
      }
      break;

   // [UNHANDLED] Return FALSE
   default:
      bResult = FALSE;
      break;
   }

   // Return result
   return (bResult ? TRUE : dlgprocVistaStyleDialog(hDialog, iMessage, wParam, lParam));
}


