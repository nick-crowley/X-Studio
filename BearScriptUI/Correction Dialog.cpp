//
// Correction Dialog.cpp : Implements the 'Submit Correction' dialog
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C WITH /C WITH ++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////

// Submission text format
CONST TCHAR*   szCorrectionFormat = TEXT("X-Studio %s Correction from %s\r\n") 
                                    TEXT("=========================================\r\n") 
                                    TEXT("\r\n") 
                                    TEXT("Command/Object %d: %s\r\n") 
                                    TEXT("\r\n") 
                                    TEXT("Original Description:\r\n") 
                                    TEXT("--------------------\r\n")
                                    TEXT("%s\r\n") 
                                    TEXT("\r\n") 
                                    TEXT("\r\n") 
                                    TEXT("Correction:\r\n") 
                                    TEXT("----------\r\n")
                                    TEXT("%s\r\n");

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createCorrectionDialogData
// Description     : Creates data for a CorrectionDialogData
// 
// Return Value   : New CorrectionDialogData, you are responsible for destroying it
// 
CORRECTION_DIALOG_DATA*   createCorrectionDialogData(CONST RESULT_TYPE  eType, SUGGESTION_RESULT  xResult)
{
   CORRECTION_DIALOG_DATA*   pDialogData;       // Object being created

   // Create object
   pDialogData = utilCreateEmptyObject(CORRECTION_DIALOG_DATA);

   // Set properties
   pDialogData->eObjectType = eType;
   pDialogData->xResult     = xResult;

   // Return object
   return pDialogData;
}


/// Function name  : deleteCorrectionDialogData
// Description     : Destroys the data for the CorrectionDialogData
// 
// CORRECTION_DIALOG_DATA*  &pDialogData   : [in] CorrectionDialogData to destroy
// 
VOID  deleteCorrectionDialogData(CORRECTION_DIALOG_DATA*  &pDialogData)
{
   // Destroy contents
   utilSafeDeleteString(pDialogData->szCorrection);

   // Destroy calling object
   utilDeleteObject(pDialogData);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : displayCorrectionDialog
// Description     : Displays the CorrectionDialog and launches the submission thread, if appropriate
// 
// HWND  hParentWnd : [in] Parent window
// 
VOID   displayCorrectionDialog(HWND  hParentWnd, CONST RESULT_TYPE  eType, SUGGESTION_RESULT  xResult)
{
   CORRECTION_DIALOG_DATA*  pDialogData;

   // [VERBOSE]
   VERBOSE_UI_COMMAND();

   // [CHECK] Ensure object has a description
   if ((eType == RT_COMMANDS AND xResult.asCommandSyntax->pTooltipDescription) OR (eType != RT_COMMANDS AND xResult.asObjectName->pDescription))
   {
      // Create dialog data
      pDialogData = createCorrectionDialogData(eType, xResult);

      /// [CHECK] Display correction dialog
      if (DialogBoxParam(getResourceInstance(), TEXT("CORRECTION_DIALOG"), hParentWnd, dlgprocCorrectionDialog, (LPARAM)pDialogData) == IDOK)
         /// [SUBMIT] Launch submission thread
         commandSubmitReport(getMainWindowData(), pDialogData->szCorrection);
         
      // Cleanup
      deleteCorrectionDialogData(pDialogData);
   }
}


/// Function name  : initCorrectionDialog
// Description     : Displays the types of errors encountered and the user's forum username
// 
// HWND  hDialog   : [in] Correction Dialog
// 
VOID   initCorrectionDialog(HWND  hDialog, CORRECTION_DIALOG_DATA*  pDialogData)
{
   /// Store dialog data
   pDialogData->hDialog = hDialog;
   SetWindowLong(hDialog, DWL_USER, (LONG)pDialogData);

   // Limit forum name to 32 characters
   SendDlgItemMessage(hDialog, IDC_SUBMISSION_FORUM_USERNAME_EDIT, EM_LIMITTEXT, 32, NULL);

   /// Display existing forum name (if any)
   SetDlgItemText(hDialog, IDC_SUBMISSION_FORUM_USERNAME_EDIT, getAppPreferences()->szForumUserName);

   /// Display description 
   switch (pDialogData->eObjectType)
   {
   case RT_COMMANDS:        setRichEditText(GetControl(hDialog, IDC_CORRECTION_DESCRIPTION_RICHEDIT), pDialogData->xResult.asCommandSyntax->pTooltipDescription, GTC_WHITE, FALSE);   break;
   case RT_GAME_OBJECTS:    setRichEditText(GetControl(hDialog, IDC_CORRECTION_DESCRIPTION_RICHEDIT), pDialogData->xResult.asObjectName->pDescription,           GTC_WHITE, FALSE);   break;
   case RT_SCRIPT_OBJECTS:  setRichEditText(GetControl(hDialog, IDC_CORRECTION_DESCRIPTION_RICHEDIT), pDialogData->xResult.asObjectName->pDescription,           GTC_WHITE, FALSE);   break;
   }

   // [DIALOG] Center dialog and set text
   utilCentreWindow(getAppWindow(), hDialog);
   SetWindowText(hDialog, getAppName());

   // [OWNER DRAW]
   utilAddWindowStyle(GetControl(hDialog, IDC_DIALOG_TITLE), SS_OWNERDRAW);
   utilAddWindowStyle(GetControl(hDialog, IDC_DIALOG_HEADING_1), SS_OWNERDRAW);
   utilAddWindowStyle(GetControl(hDialog, IDC_DIALOG_HEADING_2), SS_OWNERDRAW);

   // [FOCUS]
   SetFocus(GetControl(hDialog, IDC_CORRECTION_SUBMISSION_EDIT));
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onCorrectionDialog_Command
// Description     : Handles the buttons in the dialog and ensures the user enters a forum username
// 
// HWND        hDialog        : [in] Bug Report Dialog window handle
// CONST UINT  iControlID     : [in] Source of the message
// CONST UINT  iNotification  : [in] Notification
// 
// Return Value   : TRUE if processed, otherwise FALSE
// 
BOOL   onCorrectionDialog_Command(CORRECTION_DIALOG_DATA*  pDialogData, CONST UINT  iControlID, CONST UINT  iNotification)
{
   BOOL   bEnabled,  // Whether OK button is enabled
          bResult;   // Whether message was handled

   // Prepare
   bResult = FALSE;

   // Examine sender
   switch (iControlID)
   {
   /// [OK] Store forum username and correction text
   case IDOK:
      onCorrectionDialog_OK(pDialogData);
      // Fall through...

   /// [OK/CANCEL] Close dialog and return IDOK/IDCANCEL
   case IDCANCEL:
      EndDialog(pDialogData->hDialog, iControlID);
      bResult = TRUE;
      break;

   /// [USERNAME/CORRECTION CHANGE] Ensure both exist before enabling the OK Button
   case IDC_SUBMISSION_FORUM_USERNAME_EDIT:
   case IDC_CORRECTION_SUBMISSION_EDIT:
      // [TEXT CHANGED]
      if (iNotification == EN_CHANGE)
      {
         bEnabled = utilGetDlgItemTextLength(pDialogData->hDialog, IDC_SUBMISSION_FORUM_USERNAME_EDIT) AND utilGetDlgItemTextLength(pDialogData->hDialog, IDC_CORRECTION_SUBMISSION_EDIT);
         utilEnableDlgItem(pDialogData->hDialog, IDOK, bEnabled);
         bResult = TRUE;
      }
      break;
   }

   // Return result
   return bResult;
}


/// Function name  : onCorrectionDialog_OK
// Description     : Stores the X-Universe forum username
// 
// HWND  hDialog   : [in] Correction Dialog
// 
VOID  onCorrectionDialog_OK(CORRECTION_DIALOG_DATA*  pDialogData)
{
   CONST TCHAR  *szObjectType[3] = { TEXT("Command"), TEXT("Game Object"), TEXT("Script Object") },
                *szObjectID;          // Command ID or object ID
   TCHAR        *szUserName,          // XForums username
                *szCorrection,        // User's correction text
                *szObjectText;        // Current description
   UINT          iSubmissionLength,   // Estimated length of submission
                 iObjectID;           // Syntax or object name

   // [CHECK] Ensure forum name is alpha-numeric
   szUserName = utilGetDlgItemText(pDialogData->hDialog, IDC_SUBMISSION_FORUM_USERNAME_EDIT);
   cleanSubmissionForumUserName(szUserName, lstrlen(szUserName));

   /// Store forum username to preferences
   setAppPreferencesForumUserName(szUserName);

   // Extract correction text and object text
   szObjectText = utilGetDlgItemText(pDialogData->hDialog, IDC_CORRECTION_DESCRIPTION_RICHEDIT);
   szCorrection = utilGetDlgItemText(pDialogData->hDialog, IDC_CORRECTION_SUBMISSION_EDIT);

   // Identify IDs
   switch (pDialogData->eObjectType)
   {
   case RT_COMMANDS:        
      iObjectID  = pDialogData->xResult.asCommandSyntax->iID;  
      szObjectID = pDialogData->xResult.asCommandSyntax->szSyntax;
      break;

   case RT_GAME_OBJECTS:    
   case RT_SCRIPT_OBJECTS:
      iObjectID  = pDialogData->xResult.asObjectName->iID;
      szObjectID = pDialogData->xResult.asObjectName->szText;
      break;
   }

   /// Generate submission text
   iSubmissionLength = lstrlen(szObjectText) + lstrlen(szCorrection) + 512;
   pDialogData->szCorrection = utilCreateStringf(iSubmissionLength, szCorrectionFormat, szObjectType[pDialogData->eObjectType], szUserName, iObjectID, szObjectID, szObjectText, szCorrection);

   // Cleanup
   utilDeleteString(szUserName);
   utilDeleteStrings(szObjectText, szCorrection);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : dlgprocCorrectionDialog
// Description     : Dialog procedure for the Correction Dialog
// 
// 
INT_PTR  dlgprocCorrectionDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   CORRECTION_DIALOG_DATA*  pDialogData;
   BOOL                     bResult;

   // Prepare
   pDialogData = (CORRECTION_DIALOG_DATA*)GetWindowLong(hDialog, DWL_USER);
   bResult     = FALSE;

   // Examine message
   switch (iMessage)
   {
   /// [CREATE] Initialise dialog
   case WM_INITDIALOG:
      initCorrectionDialog(hDialog, (CORRECTION_DIALOG_DATA*)lParam);
      bResult = FALSE;
      break;

   /// [COMMAND]
   case WM_COMMAND:
      bResult = onCorrectionDialog_Command(pDialogData, LOWORD(wParam), HIWORD(wParam));
      break;

   /// [OWNERDRAW]
   case WM_DRAWITEM:
      if (wParam == IDC_DIALOG_ICON)
      {
         onOwnerDrawStaticIcon(lParam, TEXT("SUBMIT_REPORT_ICON"), 96);    
         bResult = TRUE;
      }
      break;
   }

   // Return result
   return (bResult ? TRUE : dlgprocVistaStyleDialog(hDialog, iMessage, wParam, lParam));
}


