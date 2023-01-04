//
// File Options dialog.cpp : Advanced loading options
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

/// Function name  : createFileOptionsDialogData
// Description     : Creates data for a FileOptionsDialogData
// 
// CONST LOADING_OPTIONS*  pExistingOptions : [in] Existing loading options
//
// Return Value   : New FileOptionsDialogData, you are responsible for destroying it
// 
OPTIONS_DIALOG_DATA*   createFileOptionsDialogData(CONST LOADING_OPTIONS*  pExistingOptions)
{
   OPTIONS_DIALOG_DATA*   pDialogData;       // Object being created

   // Create object
   pDialogData = utilCreateEmptyObject(OPTIONS_DIALOG_DATA);

   // Copy options
   pDialogData->oOptions = *pExistingOptions;

   // Set properties from options
   pDialogData->bScriptCalls    = (pExistingOptions->iRecursionDepth >= 1);
   pDialogData->bRecursiveCalls = (pExistingOptions->iRecursionDepth >= 2);
   pDialogData->bCompilerTest   = (pExistingOptions->eCompilerTest != ODT_NONE);

   // Return object
   return pDialogData;
}


/// Function name  : deleteFileOptionsDialogData
// Description     : Destroys the data for the FileOptionsDialogData
// 
// OPTIONS_DIALOG_DATA*  &pDialogData   : [in] FileOptionsDialogData to destroy
// 
VOID  deleteFileOptionsDialogData(OPTIONS_DIALOG_DATA*  &pDialogData)
{
   // Destroy contents


   // Destroy calling object
   utilDeleteObject(pDialogData);
}


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : displayFileOptionsDialog
// Description     : Displays the FileDialog options dialog
// 
// HWND                    hParentWnd    : [in] Parent window
// CONST LOADING_OPTIONS*  pExistingData : [in] Current settings
// 
// Return Value   : Dialog data if user clicked OK, otherwise NULL
// 
OPTIONS_DIALOG_DATA*   displayFileOptionsDialog(HWND  hParentWnd, CONST LOADING_OPTIONS*  pExistingOptions)
{
   OPTIONS_DIALOG_DATA*  pDialogData;    // New window data

   // Prepare
   pDialogData = createFileOptionsDialogData(pExistingOptions);

   // [CHECK] Display dialog
   if (showDialog(TEXT("FILE_OPTIONS_DIALOG"), hParentWnd, dlgprocFileOptionsDialog, (LPARAM)pDialogData) == IDOK)
      // [CONFIRMATION]
      return pDialogData;
   
   // Return NULL
   deleteFileOptionsDialogData(pDialogData);
   return NULL;
}


/// Function name  : initFileOptionsDialog
// Description     : Initialises the dialog and displays the current contents of the dialog data
// 
// OPTIONS_DIALOG_DATA*  pDialogData : [in] Dialog data
// HWND                  hDialog     : [in] Dialog
// 
// Return Value   : TRUE
// 
BOOL   initFileOptionsDialog(OPTIONS_DIALOG_DATA*  pDialogData, HWND  hDialog)
{
   // Store window data
   SetWindowLong(hDialog, DWL_USER, (LONG)pDialogData);
   pDialogData->hDialog = hDialog;

   /// [DIALOG] Centre dialog and set caption
   utilCentreWindow(GetParent(hDialog), hDialog);
   SetWindowText(hDialog, getAppName());

   /// Setup slider
   SendDlgItemMessage(hDialog, IDC_FILE_OPTIONS_RECURSION_SLIDER, TBM_SETRANGE, FALSE, MAKE_LONG(2, 5));

   /// Check/Uncheck options
   CheckDlgButton(hDialog, IDC_FILE_OPTIONS_SCRIPTCALL_CHECK, pDialogData->bScriptCalls);
   CheckDlgButton(hDialog, IDC_FILE_OPTIONS_RECURSION_CHECK,  pDialogData->bRecursiveCalls);
   CheckDlgButton(hDialog, IDC_FILE_OPTIONS_COMPILER_CHECK,   pDialogData->bCompilerTest);

   /// Select compiler test
   CheckDlgButton(hDialog, (pDialogData->oOptions.eCompilerTest == ODT_VALIDATION ? IDC_FILE_OPTIONS_VALIDATION_RADIO : IDC_FILE_OPTIONS_GENERATION_RADIO), TRUE);

   /// Enable/Disable controls
   utilEnableDlgItem(hDialog, IDC_FILE_OPTIONS_RECURSION_CHECK,  pDialogData->bScriptCalls);   
   utilEnableDlgItem(hDialog, IDC_FILE_OPTIONS_RECURSION_SLIDER, pDialogData->bScriptCalls AND pDialogData->bRecursiveCalls);
   utilEnableDlgItem(hDialog, IDC_FILE_OPTIONS_DEPTH_STATIC,     pDialogData->bScriptCalls AND pDialogData->bRecursiveCalls);
   utilEnableDlgItem(hDialog, IDC_FILE_OPTIONS_GENERATION_RADIO, pDialogData->bCompilerTest);
   utilEnableDlgItem(hDialog, IDC_FILE_OPTIONS_VALIDATION_RADIO, pDialogData->bCompilerTest);

   // Display recursion depth
   updateFileOptionsDialogRecursionDepth(pDialogData, max(2, pDialogData->oOptions.iRecursionDepth));

   // [OWNER DRAW]
   utilAddWindowStyle(GetControl(hDialog, IDC_DIALOG_TITLE),     SS_OWNERDRAW);
   utilAddWindowStyle(GetControl(hDialog, IDC_DIALOG_HEADING_1), SS_OWNERDRAW);
   utilAddWindowStyle(GetControl(hDialog, IDC_DIALOG_HEADING_2), SS_OWNERDRAW);

   // [TUTORIAL] Delay display 'File Options' tutorial
   setMainWindowTutorialTimer(getMainWindowData(), TW_FILE_OPTIONS, TRUE);
   
   // Return TRUE
   return TRUE;
}


/// Function name  : updateFileOptionsDialogRecursionDepth
// Description     : Adjusts the slider and it's label
// 
// OPTIONS_DIALOG_DATA*  pDialogData : [in] Dialog data
// CONST UINT            iDepth      : [in] Recursion depth
// 
VOID  updateFileOptionsDialogRecursionDepth(OPTIONS_DIALOG_DATA*  pDialogData, CONST UINT  iDepth)
{
   // Update slider
   SendDlgItemMessage(pDialogData->hDialog, IDC_FILE_OPTIONS_RECURSION_SLIDER, TBM_SETPOS, FALSE, iDepth);

   // Update label
   utilSetWindowTextf(GetControl(pDialogData->hDialog, IDC_FILE_OPTIONS_DEPTH_STATIC), TEXT("%d Scripts"), iDepth);

   // Temporarily store current depth
   pDialogData->oOptions.iRecursionDepth = iDepth;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onFileOptionsDialog_AdjustDepth
// Description     : Slider adjustment handler
// 
// OPTIONS_DIALOG_DATA*  pDialogData : [in] Dialog data
// CONST UINT            eScrollType : [in] Type of scroll
// CONST UINT            iAmount     : [in] Current drag position
// 
VOID  onFileOptionsDialog_AdjustDepth(OPTIONS_DIALOG_DATA*  pDialogData, CONST UINT  eScrollType, CONST UINT  iAmount)
{
   UINT  iNewDepth,
         iMinDepth,
         iMaxDepth;

   // Prepare
   iNewDepth = pDialogData->oOptions.iRecursionDepth;
   iMinDepth = SendDlgItemMessage(pDialogData->hDialog, IDC_FILE_OPTIONS_RECURSION_SLIDER, TBM_GETRANGEMIN, NULL, NULL);
   iMaxDepth = SendDlgItemMessage(pDialogData->hDialog, IDC_FILE_OPTIONS_RECURSION_SLIDER, TBM_GETRANGEMAX, NULL, NULL);

   // Examine scroll type
   switch (eScrollType)
   {
   /// [NUDGE]
   case SB_PAGERIGHT:
   case SB_LINERIGHT:   iNewDepth++;   break;
   case SB_PAGELEFT:
   case SB_LINELEFT:    iNewDepth--;   break;

   /// [HOME/END] 
   case SB_LEFT:        iNewDepth = iMinDepth;       break;
   case SB_RIGHT:       iNewDepth = iMaxDepth;       break;

   /// [DRAG]
   case SB_THUMBTRACK:  iNewDepth = iAmount;         break;

   // [CRAP] Block
   case SB_THUMBPOSITION:
   case SB_ENDSCROLL:
      return;
   }

   // [CHECK] Enforce range
   iNewDepth = utilEnforceRange(iNewDepth, iMinDepth, iMaxDepth);

   /// Update slider and dialog data
   updateFileOptionsDialogRecursionDepth(pDialogData, iNewDepth);
}


/// Function name  : onFileDialogCommand
// Description     : Process WM_COMMAND notifications
//
// FILE_DIALOG_DATA*  pDialogData   : [in] File dialog data
// CONST UINT         iControlID    : [in] ID of the control sending the notification
// CONST UINT         iNotification : [in] Notification code
// HWND               hCtrl         : [in] Window handle of the control sending the message (if any)
// 
// Return type : TRUE if processed, FALSE if not
//
BOOL  onFileOptionsDialog_Command(OPTIONS_DIALOG_DATA*  pDialogData, CONST UINT  iControlID, CONST UINT  iNotification, HWND  hCtrl)
{
   BOOL    bResult;

   // Prepare
   bResult  = TRUE;
   
   // Examine control ID
   switch (iControlID)
   {
   /// [SCRIPT-CALL CHECK] Enable/Disable recursion check/slider
   case IDC_FILE_OPTIONS_SCRIPTCALL_CHECK:
      // Update data
      pDialogData->bScriptCalls = IsDlgButtonChecked(pDialogData->hDialog, IDC_FILE_OPTIONS_SCRIPTCALL_CHECK);

      // Enable/Disable check and slider
      utilEnableDlgItem(pDialogData->hDialog, IDC_FILE_OPTIONS_RECURSION_CHECK,  pDialogData->bScriptCalls);
      utilEnableDlgItem(pDialogData->hDialog, IDC_FILE_OPTIONS_RECURSION_SLIDER, pDialogData->bScriptCalls AND pDialogData->bRecursiveCalls);
      utilEnableDlgItem(pDialogData->hDialog, IDC_FILE_OPTIONS_DEPTH_STATIC,     pDialogData->bScriptCalls AND pDialogData->bRecursiveCalls);
      break;

   /// [RECURSION CHECK] Enable/Disable slider
   case IDC_FILE_OPTIONS_RECURSION_CHECK:
      // Update data
      pDialogData->bRecursiveCalls = IsDlgButtonChecked(pDialogData->hDialog, IDC_FILE_OPTIONS_RECURSION_CHECK);

      // Enable/Disable slider
      utilEnableDlgItem(pDialogData->hDialog, IDC_FILE_OPTIONS_RECURSION_SLIDER, pDialogData->bRecursiveCalls);
      utilEnableDlgItem(pDialogData->hDialog, IDC_FILE_OPTIONS_DEPTH_STATIC,     pDialogData->bRecursiveCalls);
      break;

   /// [COMPILER CHECK] Enable/Disable radios
   case IDC_FILE_OPTIONS_COMPILER_CHECK:
      // Update data
      pDialogData->bCompilerTest = IsDlgButtonChecked(pDialogData->hDialog, IDC_FILE_OPTIONS_COMPILER_CHECK);

      // Enable/Disable radios
      utilEnableDlgItem(pDialogData->hDialog, IDC_FILE_OPTIONS_GENERATION_RADIO, pDialogData->bCompilerTest);
      utilEnableDlgItem(pDialogData->hDialog, IDC_FILE_OPTIONS_VALIDATION_RADIO, pDialogData->bCompilerTest);
      break;

   /// [OK] Save dialog values
   case IDOK:
      onFileOptionsDialog_OK(pDialogData);
      // Fall through..

   /// [CANCEL] Close dialog
   case IDCANCEL:
      EndDialog(pDialogData->hDialog, iControlID);
      break;

   // [UNHANDLED]
   default:
      bResult = FALSE;
      break;
   }

   // Cleanup and return result
   return bResult;
}


/// Function name  : onFileOptionsDialog_OK
// Description     : Saves current dialog settings to the dialog data
// 
// OPTIONS_DIALOG_DATA*  pDialogData   : [in] Dialog data
// 
VOID  onFileOptionsDialog_OK(OPTIONS_DIALOG_DATA*  pDialogData)
{
   LOADING_OPTIONS*  pOptions;

   // Prepare
   pOptions = &pDialogData->oOptions;

   /// Script calls
   if (pDialogData->bRecursiveCalls)
      // [RECURSION] Set recursion depth
      pOptions->iRecursionDepth = SendDlgItemMessage(pDialogData->hDialog, IDC_FILE_OPTIONS_RECURSION_SLIDER, TBM_GETPOS, NULL, NULL);
   else 
      // [SCRIPT-CALLS] Set depth to one or zero
      pOptions->iRecursionDepth = (pDialogData->bScriptCalls ? 1 : 0);

   /// Compiler test
   if (pDialogData->bCompilerTest)
   {
      // [COMPILER] Set compiler test, zero recursion depth
      pOptions->eCompilerTest   = (IsDlgButtonChecked(pDialogData->hDialog, IDC_FILE_OPTIONS_GENERATION_RADIO) ? ODT_GENERATION : ODT_VALIDATION); 
      pOptions->iRecursionDepth = NULL;
   }
   else
      // [NONE]
      pOptions->eCompilerTest = ODT_NONE;
}


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : dlgprocFileOptionsDialog
// Description     : Dialog procedure for the WINDOW_NAME
// 
// 
INT_PTR  dlgprocFileOptionsDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   OPTIONS_DIALOG_DATA*   pDialogData;       // Window data
   BOOL                   bResult;

   // Prepare
   pDialogData = (OPTIONS_DIALOG_DATA*)GetWindowLong(hDialog, DWL_USER);
   bResult     = FALSE;

   // Examine message
   switch (iMessage)
   {
   /// [CREATE] Store window data and init dialog
   case WM_INITDIALOG:
      // Initialise window
      bResult = initFileOptionsDialog((OPTIONS_DIALOG_DATA*)lParam, hDialog);
      break;

   /// [COMMAND]
   case WM_COMMAND:
      bResult = onFileOptionsDialog_Command(pDialogData, LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
      break;

   /// [HELP]
   case WM_HELP:
      bResult = displayHelp(TEXT("Browser_CataloguesOptions"));
      break;

   /// [SLIDER ADJUSTMENT]
   case WM_HSCROLL:
      onFileOptionsDialog_AdjustDepth(pDialogData, LOWORD(wParam), HIWORD(wParam));
      bResult = TRUE;
      break;

   /// [DIALOG ICON]
   case WM_DRAWITEM:
      if (wParam == IDC_DIALOG_ICON)
         bResult = onOwnerDrawStaticBitmap(lParam, TEXT("FILE_OPTIONS_BITMAP"), 96, 256);
      break;
   }

   // Return result or pass to owner-draw handler
   return (bResult ? TRUE : dlgprocVistaStyleDialog(hDialog, iMessage, wParam, lParam));
}

