//
// Progress Dialog.cpp : Thread operation progress dialog
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// //////////////////////////////////////////////////////////////////////////////////////////////
///                                       CONSTANTS / GLOBALS
/// //////////////////////////////////////////////////////////////////////////////////////////////

CONST UINT  iProgressBarMaximum   = 10000,
            iProgressTimerID      = 3;

// onException: Display 
#define  ON_EXCEPTION()    displayException(pException);

/// //////////////////////////////////////////////////////////////////////////////////////////////
///                                       CREATION / DESTRUCTION
/// //////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createProgressDialogData
// Description     : Create dialog data for a progress dialog
// 
// Return Value   : New dialog data, you are responsible for destroying it
// 
PROGRESS_DATA*  createProgressDialogData(OPERATION_POOL*  pOperationPool)
{  
   PROGRESS_DATA*  pDialogData;

   // Create new dialog data
   pDialogData = utilCreateEmptyObject(PROGRESS_DATA);

   // Store operatio pool pointer
   pDialogData->pOperationPool = pOperationPool;

   // Return new data
   return pDialogData;
}


/// Function name  : deleteProgressDialogData
// Description     : Destroys progress dialog data
// 
// PROGRESS_DATA*  &pProgressData  : [in] Progress dialog data to destroy
// 
VOID   deleteProgressDialogData(PROGRESS_DATA*  &pProgressData)
{
   // Delete icon
   DestroyIcon(pProgressData->hIcon);

   // Delete calling object
   utilDeleteObject(pProgressData);
}

/// //////////////////////////////////////////////////////////////////////////////////////////////
///                                       HELPERS
/// //////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : getProgressDialogData
// Description     : Retrieve the dialog data for a progress dialog
// 
// HWND  hDialog   : [in] Progress dialog window handle
// 
// Return Value   : Progress dialog data
// 
PROGRESS_DATA*  getProgressDialogData(HWND  hDialog)
{
   return (PROGRESS_DATA*)GetWindowLong(hDialog, DWL_USER);
}

/// //////////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// //////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : displayProgressDialog
// Description     : Creates and displays the progress dialog
// 
// MAIN_WINDOW_DATA*  pMainWindowData : [in] Main window data
// 
// Return type : Window handle of the new dialog if successfull, otherwise NULL
//
HWND    displayProgressDialog(MAIN_WINDOW_DATA*  pMainWindowData)
{
   PROGRESS_DATA*  pDialogData;     // New dialog data
   HWND            hDialog;         // for convenience

   // [CHECK] A previous progress dialog should not exist
   ASSERT(pMainWindowData->hProgressDlg == NULL);  

   /// Create dialog data
   pDialogData = createProgressDialogData(pMainWindowData->pOperationPool);

   /// Pass data to new dialog
   hDialog = loadDialog(TEXT("PROGRESS_DIALOG"), getAppWindow(), dlgprocProgressDlg, (LPARAM)pDialogData);
   ERROR_CHECK("creating progress dialog", hDialog);

   // Store handle
   if (pMainWindowData->hProgressDlg = hDialog)
   {
      // Disable main window
      EnableWindow(getAppWindow(), FALSE);

      /// Show dialog
      UpdateWindow(hDialog);
      ShowWindow(hDialog, SW_SHOW);

      //// [MODAL] Add to modal stack
      //pushModalWindowStack(hDialog);
   }
   // [ERROR] Cleanup dialog data
   else
      deleteProgressDialogData(pDialogData);

   // Return window handle
   return hDialog;
}


/// Function name  : initProgressDialog
// Description     : Initialise the progress dialog
// 
// PROGRESS_DATA*  pDialogData : [in] Progress dialog data
// 
// Return Value   : TRUE
// 
BOOL  initProgressDialog(PROGRESS_DATA*  pDialogData)
{

   // Store window data
   SetWindowLong(pDialogData->hDialog, DWL_USER, (LONG)pDialogData);

   // [DIALOG] Center dialog and set title
   utilCentreWindow(getAppWindow(), pDialogData->hDialog);
   SetWindowText(pDialogData->hDialog, getAppName());

   // [OWNER DRAW]
   utilAddWindowStyle(GetControl(pDialogData->hDialog, IDC_PROGRESS_DIALOG_TITLE), SS_OWNERDRAW WITH CSS_CENTRE);
   utilAddWindowStyle(GetControl(pDialogData->hDialog, IDC_PROGRESS_DIALOG_HEADING), SS_OWNERDRAW WITH CSS_CENTRE);

   /// [PROGRESS] Set progress bar RANGE and set initial POSITION 
   SendDlgItemMessage(pDialogData->hDialog, IDC_PROGRESS_BAR, PBM_SETRANGE32, 0, iProgressBarMaximum);
   setProgressDialogProgressValue(pDialogData, 0);
   
   /// [STAGE] Set initial description
   updateProgressDialogStageDescription(pDialogData);

   /// [TITLE] Set the title static
   updateProgressDialogTitle(pDialogData);

   // [TIMER] Create update timer
   SetTimer(pDialogData->hDialog, iProgressTimerID, 200, NULL);

   // Show window
   ShowWindow(pDialogData->hDialog, SW_SHOW);
   UpdateWindow(pDialogData->hDialog);

   // Return TRUE
   return TRUE;
}


/// Function name  : setProgressDialogProgressValue
// Description     : Sets the value of the progress bar and taskbar (under windows 7)
// 
// PROGRESS_DATA*  pDialogData      : [in] Progress dialog
// CONST UINT      iCurrentProgress : [in] New progress between 0 and 10,000
// 
VOID  setProgressDialogProgressValue(PROGRESS_DATA*  pDialogData, CONST UINT  iCurrentProgress)
{

   /// [VALUE] Set progress bar value
   SendDlgItemMessage(pDialogData->hDialog, IDC_PROGRESS_BAR, PBM_SETPOS, iCurrentProgress, NULL);

   /// [TITLE] Set the title static
   updateProgressDialogTitle(pDialogData);

   // [CHECK] Are we running in windows 7 or newer?
   if (getAppWindowsVersion() >= WINDOWS_7)
      /// [SUCCESS] Display progress in the taskbar
      utilSetWindowProgressValue(getAppWindow(), iCurrentProgress, iProgressBarMaximum);

}


/// Function name  : updateProgressDialogStageDescription
// Description     : Updates the description of the current stage, if required
// 
// PROGRESS_DATA*  pDialogData : [in] Progress dialog data
// 
VOID   updateProgressDialogStageDescription(PROGRESS_DATA*  pDialogData)
{
   TCHAR*  szText;
   
   // [CHECK] Do nothing if stage is already being displayed
   if (getCurrentOperationStageID(pDialogData->pOperationPool) != pDialogData->iCurrentStageID)
   {
      /// Update current stage ID
      pDialogData->iCurrentStageID = getCurrentOperationStageID(pDialogData->pOperationPool);

      /// Load description for the current stage and display
      SetDlgItemText(pDialogData->hDialog, IDC_PROGRESS_STAGE_STATIC, szText=loadString(pDialogData->iCurrentStageID));
      utilDeleteString(szText);
   }
}

/// Function name  : updateProgressDialogTitle
// Description     : Sets the title static to reflect the operation
// 
// PROGRESS_DATA*  pDialogData   : [in] Dialog data
// 
VOID  updateProgressDialogTitle(PROGRESS_DATA*  pDialogData)
{
   OPERATION_DATA*  pOperationData;


   /// Lookup operation data
   if (pOperationData = getCurrentOperationData(pDialogData->pOperationPool))
   {
      // Replace title based on operation
      switch (pOperationData->eType)
      {
      case OT_LOAD_SCRIPT_FILE:
      case OT_LOAD_LANGUAGE_FILE:    SetDlgItemText(pDialogData->hDialog, IDC_PROGRESS_DIALOG_TITLE, TEXT("Loading Documents"));     break;
      case OT_SAVE_SCRIPT_FILE:
      case OT_SAVE_LANGUAGE_FILE:    SetDlgItemText(pDialogData->hDialog, IDC_PROGRESS_DIALOG_TITLE, TEXT("Saving Documents"));      break;
      case OT_LOAD_PROJECT_FILE:     SetDlgItemText(pDialogData->hDialog, IDC_PROGRESS_DIALOG_TITLE, TEXT("Loading Project"));       break;
      case OT_SAVE_PROJECT_FILE:     SetDlgItemText(pDialogData->hDialog, IDC_PROGRESS_DIALOG_TITLE, TEXT("Saving Project"));        break;
      case OT_SUBMIT_BUG_REPORT:     SetDlgItemText(pDialogData->hDialog, IDC_PROGRESS_DIALOG_TITLE, TEXT("Submitting Report"));     break;
      case OT_VALIDATE_SCRIPT_FILE:  SetDlgItemText(pDialogData->hDialog, IDC_PROGRESS_DIALOG_TITLE, TEXT("Verifying Documents"));   break;
      }
   }

}

/// //////////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// //////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onProgressDialogDestroy
// Description     : Re-enable the parent window and destroy the dialog data
// 
// PROGRESS_DATA*  pDialogData : [in] Progress dialog data
// 
VOID     onProgressDialogDestroy(PROGRESS_DATA*  pDialogData)
{

   /// [CHECK] Are we running in windows 7 or newer?
   if (getAppWindowsVersion() >= WINDOWS_7)
      // [SUCCESS] Hide taskbar progress bar
      utilSetWindowProgressState(getAppWindow(), TBPF_NOPROGRESS);

   /// Destroy timer
   KillTimer(pDialogData->hDialog, iProgressTimerID);

   /// Delete dialog data
   deleteProgressDialogData(pDialogData);
}


/// Function name  : onProgressDialogTimer
// Description     : Updates the progress bar and the stage description
// 
// PROGRESS_DATA*  pDialogData : [in] Progress dialog data
// 
VOID     onProgressDialogTimer(PROGRESS_DATA*  pDialogData)
{
   INT  iCurrentPosition;        // New progress bar position


   // Get current progress
   iCurrentPosition = getCurrentOperationProgress(pDialogData->pOperationPool);

   /// Update progress bar
   setProgressDialogProgressValue(pDialogData, iCurrentPosition);
   
   /// Update stage description if necessary
   updateProgressDialogStageDescription(pDialogData);

   // Refresh progress bar
   UpdateWindow(GetDlgItem(pDialogData->hDialog, IDC_PROGRESS_BAR));
}


/// Function name  : dlgprocProgressDlg
// Description     : Progress dialog window procedure
// 
INT_PTR   dlgprocProgressDlg(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   PROGRESS_DATA*  pDialogData;    // Dialog data
   BOOL            bResult;        // Operation result

   TRY
   {
      // Prepare
      pDialogData = getProgressDialogData(hDialog);
      bResult     = FALSE;

      // Examine message
      switch (iMessage)
      {
      /// [CREATION]
      case WM_INITDIALOG:
         // Prepare
         pDialogData = (PROGRESS_DATA*)lParam;
         pDialogData->hDialog = hDialog;
         // Initialise dialog
         bResult = initProgressDialog(pDialogData);
         break;

      /// [DESTRUCTION]
      case WM_DESTROY:
         onProgressDialogDestroy(pDialogData);
         bResult = TRUE;
         break;

      /// [TICKER] - Pass to the ticker or the completion function, depending on the 'complete' flag
      case WM_TIMER:
         onProgressDialogTimer(pDialogData);
         bResult = TRUE;
         break;

      /// [OWNER DRAW]
      case WM_DRAWITEM:
         switch (wParam)
         {
         case IDC_PROGRESS_DIALOG_ICON:     bResult = onOwnerDrawStaticIcon(lParam, TEXT("FUNCTION_ICON"), 72);  break;
         case IDC_PROGRESS_DIALOG_TITLE:    bResult = onOwnerDrawStaticTitle(lParam);       break;
         case IDC_PROGRESS_DIALOG_HEADING:  bResult = onOwnerDrawStaticHeading(lParam);     break;
         }
         break;

      /// [VISUAL STYLES]
      case WM_CTLCOLORDLG:
      case WM_CTLCOLORSTATIC:
         bResult = (BOOL)onDialog_ControlColour((HDC)wParam, COLOR_WINDOW);
         break;
      }

      // Return result
      return bResult;
   }
   /// [EXCEPTION HANDLER]
   CATCH4("iMessage=%s  wParam=%d  lParam=%d  iCurrentStageID=%d", identifyMessage(iMessage), wParam, lParam, (pDialogData ? pDialogData->iCurrentStageID : -1));
   // Ensure main window is re-enabled
   EnableWindow(getAppWindow(), TRUE);
   return FALSE;   
}



