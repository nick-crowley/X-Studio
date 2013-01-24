//
// SourceText .cpp : The 'Insert Page' dialog for the language document
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

struct DIALOG_DATA
{
   GAME_STRING*  pGameString;
   BOOL          bVirtualDoc;
};

// Functions
BOOL     initSourceTextDialog(DIALOG_DATA*  pDialogData, HWND  hDialog, HWND  hTooltip);
BOOL     onSourceTextDialogCommand(DIALOG_DATA*  pDialogData, HWND  hDialog, CONST UINT  iControlID, CONST UINT  iNotification, HWND  hCtrl);

// Dialog Proc
INT_PTR  dlgprocSourceTextDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : displaySourceTextDialog
// Description     : Displays the 'Source Text' dialog
// 
// LANGUAGE_DOCUMENT*  pDocument : [in] Document
// GAME_STRING*  pTargetString : [in/out] String to be edited
// HWND          hParentWnd    : [in]     Parent window for the dialog
// 
// Return Value   : IDOK / IDCANCEL
// 
BOOL  displaySourceTextDialog(LANGUAGE_DOCUMENT*  pDocument, GAME_STRING*  pTargetString, HWND  hParentWnd)
{
   DIALOG_DATA  oDialogData = { pTargetString, pDocument->bVirtual };

   // Display 'Insert Page' dialog
   return DialogBoxParam(getResourceInstance(), TEXT("SOURCETEXT_DIALOG"), hParentWnd, dlgprocSourceTextDialog, (LPARAM)&oDialogData);
}


/// Function name  : initSourceTextDialog
// Description     : Display text and initialise dialog
// 
// DIALOG_DATA*  pDialogData : [in] Dialog data
// HWND          hDialog     : [in] Dialog
// HWND          hTooltip    : [in] Tooltip handle
// 
// Return Value   : TRUE
// 
BOOL  initSourceTextDialog(DIALOG_DATA*  pDialogData, HWND  hDialog, HWND  hTooltip)
{
   // Create tooltips
   /*addTooltipTextToControl(hTooltip, hDialog, IDC_ARGUMENT_NAME_EDIT);
   addTooltipTextToControl(hTooltip, hDialog, IDC_ARGUMENT_NAME_ICON);
   addTooltipTextToControl(hTooltip, hDialog, IDC_ARGUMENT_TYPE_COMBO);
   addTooltipTextToControl(hTooltip, hDialog, IDC_ARGUMENT_DESCRIPTION_EDIT);*/

   /// Display source text  (and de-select it)
   SetDlgItemText(hDialog, IDC_SOURCE_EDIT, pDialogData->pGameString->szText);
   SubclassWindow(GetDlgItem(hDialog, IDC_SOURCE_EDIT), wndprocCustomRichEditControl);

   /// [GAME-DATA] Disable OK button
   utilEnableDlgItem(hDialog, IDOK, !pDialogData->bVirtualDoc);

   // [OWNER DRAW]
   utilAddWindowStyle(GetControl(hDialog, IDC_DIALOG_ICON),      SS_OWNERDRAW);
   utilAddWindowStyle(GetControl(hDialog, IDC_DIALOG_TITLE),     SS_OWNERDRAW);
   utilAddWindowStyle(GetControl(hDialog, IDC_DIALOG_HEADING_1), SS_OWNERDRAW);

   // [DIALOG]
   SetWindowText(hDialog, getAppName());
   utilCentreWindow(GetParent(hDialog), hDialog);

   // Return TRUE
   return TRUE;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onSourceTextDialogCommand
// Description     : Updates the input string and closes the dialog
// 
// DIALOG_DATA*  pDialogData   : [in] Dialog data
// HWND          hDialog       : [in] Window handle of the 'Insert Page' dialog
// CONST UINT    iControlID    : [in] ID of the control sending the command
// CONST UINT    iNotification : [in] Notification code
// HWND          hCtrl         : [in] Window handle of the control sending the command
// 
// Return Value   : TRUE if processed, FALSE otherwise
// 
BOOL   onSourceTextDialogCommand(DIALOG_DATA*  pDialogData, HWND  hDialog, CONST UINT  iControlID, CONST UINT  iNotification, HWND  hCtrl)
{
   TCHAR*  szNewText;

   switch (iControlID)
   {
   /// [OK] Store changes to input string
   case IDOK:
      // [CHECK] Ensure we're not changing game data
      if (!pDialogData->bVirtualDoc)
      {
         updateGameStringText(pDialogData->pGameString, szNewText = utilGetDlgItemText(hDialog, IDC_SOURCE_EDIT));
         utilDeleteString(szNewText);
      }
      // Fall through...

   /// [CANCEL] Close dialog
   case IDCANCEL:
      EndDialog(hDialog, (INT_PTR)iControlID);
      return TRUE;
   }

   // [UNHANDLED]
   return FALSE;
}


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : dlgprocSourceTextDialog
// Description     : Window procedure for the 'SourceText' dialog
// 
// 
INT_PTR  dlgprocSourceTextDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   DIALOG_DATA*  pDialogData = (DIALOG_DATA*)GetWindowLong(hDialog, DWL_USER);   
   BOOL          bResult     = FALSE;

   // Examine message
   switch (iMessage)
   {
   /// [CREATION] Initialise dialog
   case WM_INITDIALOG:
      // Store data + Init Dialog
      SetWindowLong(hDialog, DWL_USER, lParam);
      bResult = initSourceTextDialog((DIALOG_DATA*)lParam, hDialog, NULL);
      break;

   /// [COMMAND PROCESSING] -- Process OK / CANCEL
   case WM_COMMAND:
      bResult = onSourceTextDialogCommand(pDialogData, hDialog, LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
      break;

   /// [OWNER DRAW]
   case WM_DRAWITEM:
      if (wParam == IDC_DIALOG_ICON)
         bResult = onOwnerDrawStaticIcon(lParam, TEXT("EDIT_FORMATTING_ICON"), 96);
      break;

   /// [HELP] Invoke help
   case WM_HELP:
      bResult = displayHelp(TEXT("TODO"));
      break;
   }

   // Return result
   return (bResult ? TRUE : dlgprocVistaStyleDialog(hDialog, iMessage, wParam, lParam));
}


