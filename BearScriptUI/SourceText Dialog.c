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
   SIZE          siMinimum;
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

   // [TRACK]
   CONSOLE_COMMAND_BOLD();

   // Display 'Insert Page' dialog
   return showDialog(TEXT("SOURCETEXT_DIALOG"), hParentWnd, dlgprocSourceTextDialog, (LPARAM)&oDialogData);
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
   addTooltipTextToControl(hTooltip, hDialog, IDC_SOURCE_EDIT);
   
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

   // Store window size
   utilGetWindowSize(hDialog, &pDialogData->siMinimum);
   pDialogData->siMinimum.cx += GetSystemMetrics(SM_CXSIZEFRAME) * 2;
   pDialogData->siMinimum.cy += GetSystemMetrics(SM_CYSIZEFRAME) + GetSystemMetrics(SM_CYCAPTION);

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

/// Function name  : onGetMinimumSize
// Description     : Restricts dialog size to it's initial size
// 
// DIALOG_DATA*  pDialogData : [in] Data
// HWND          hDialog     : [in] Dialog
// MINMAXINFO*   pSize       : [in/out] Minimum Size
// 
// Return Value   : 
// 
BOOL  onGetMinimumSize(DIALOG_DATA*  pDialogData, HWND  hDialog, MINMAXINFO*  pSize)
{
   pSize->ptMinTrackSize.x = pDialogData->siMinimum.cx;
   pSize->ptMinTrackSize.y = pDialogData->siMinimum.cy;
   return 0;
}


/// Function name  : onSourceTextDialogResize
// Description     : Resizes RichEdit and moves dialog buttons
// 
// DIALOG_DATA*  pDialogData : [in] Data
// HWND          hDialog     : [in] Dialog
// const UINT    iWidth      : [in] Width
// const UINT    iHeight     : [in] Height
// 
// Return Value   : 
// 
BOOL  onSourceTextDialogResize(DIALOG_DATA*  pDialogData, HWND  hDialog, const UINT  iWidth, const UINT  iHeight)
{
   RECT   rcButton1,
          rcButton2,
          rcEdit;
   POINT  ptGuide = { iWidth - 14, iHeight - 10 };

   // Move Cancel
   utilGetDlgItemRect(hDialog, IDCANCEL, &rcButton1);
   OffsetRect(&rcButton1, ptGuide.x - rcButton1.right, ptGuide.y - rcButton1.bottom);
   utilSetDlgItemRect(hDialog, IDCANCEL, &rcButton1, TRUE);

   // Move OK
   utilGetDlgItemRect(hDialog, IDOK, &rcButton2);
   OffsetRect(&rcButton2, (rcButton1.left - 14) - rcButton2.right, ptGuide.y - rcButton2.bottom);
   utilSetDlgItemRect(hDialog, IDOK, &rcButton2, TRUE);

   // Extend Edit
   utilGetDlgItemRect(hDialog, IDC_SOURCE_EDIT, &rcEdit);
   rcEdit.right  = ptGuide.x;
   rcEdit.bottom = rcButton2.top - 28;
   utilSetDlgItemRect(hDialog, IDC_SOURCE_EDIT, &rcEdit, TRUE);

   // Repaint dialog
   InvalidateRect(hDialog, NULL, TRUE);
   return 0;
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
   static HWND   hTooltip    = NULL;
   BOOL          bResult     = FALSE;

   // Examine message
   switch (iMessage)
   {
   /// [CREATION] Initialise dialog
   case WM_INITDIALOG:
      // Store data + Init Dialog
      SetWindowLong(hDialog, DWL_USER, lParam);
      bResult = initSourceTextDialog((DIALOG_DATA*)lParam, hDialog, hTooltip = createTooltipWindow(hDialog));
      break;

   /// [DESTRUCTION] Cleanup
   case WM_DESTROY:
      utilDeleteWindow(hTooltip);
      break;

   /// [BUTTONS] -- Process OK / CANCEL
   case WM_COMMAND:
      bResult = onSourceTextDialogCommand(pDialogData, hDialog, LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
      break;

   /// [GET MIN SIZE]
   case WM_GETMINMAXINFO:
      bResult = onGetMinimumSize(pDialogData, hDialog, (MINMAXINFO*)lParam);
      break;

   /// [RESIZE]
   case WM_SIZE:
      bResult = onSourceTextDialogResize(pDialogData, hDialog, LOWORD(lParam), HIWORD(lParam));
      break;

   /// [OWNER DRAW]
   case WM_DRAWITEM:
      if (wParam == IDC_DIALOG_ICON)
         bResult = onOwnerDrawStaticIcon(lParam, TEXT("EDIT_FORMATTING_ICON"), 96);
      break;

   /// [HELP] Invoke help
   case WM_HELP:
      bResult = displayHelp(TEXT("Language_Errors"));
      break;
   }

   // Return result
   return (bResult ? TRUE : dlgprocVistaStyleDialog(hDialog, iMessage, wParam, lParam));
}


