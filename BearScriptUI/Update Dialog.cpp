//
// Update Dialog.cpp : UpdateCheck dialog
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        MACROS
/// /////////////////////////////////////////////////////////////////////////////////////////

// Functions
VOID     initUpdateDialog(HWND  hDialog, const UPDATE_OPERATION*  pOperationData);

// Handlers
BOOL     onUpdateDialog_Command(HWND  hDialog, CONST UINT  iControlID, CONST UINT  iNotification);
BOOL     onUpdateDialog_LinkClick(HWND  hDialog, const UPDATE_OPERATION*  pOperationData);

// Window proc
INT_PTR  dlgprocUpdateDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);

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

/// Function name  : displayUpdateDialog
// Description     : Displays the update dialog
// 
// HWND               hParent        : [in] Parent 
// UPDATE_OPERATION*  pOperationData : [in] Update data
// 
// Return Value   : 
// 
BOOL   displayUpdateDialog(HWND  hParent, UPDATE_OPERATION*  pOperationData)
{
   // [VERBOSE]
   CONSOLE_ACTION_BOLD();

   // Display dialog
   return showDialog(TEXT("UPDATE_DIALOG"), utilEither(hParent, utilGetTopWindow(getAppWindow())), dlgprocUpdateDialog, (LPARAM)pOperationData);
}


/// Function name  : initUpdateDialog
// Description     : Displays the types of errors encountered and the user's forum username
// 
// HWND               hDialog        : [in] Update Dialog window handle
// UPDATE_OPERATION*  pOperationData : [in] Update data
// 
VOID   initUpdateDialog(HWND  hDialog, const UPDATE_OPERATION*  pOperationData)
{
   TCHAR*  szFormat;

   // Set name + date
   utilSetWindowTextf(GetControl(hDialog, IDC_UPDATE_STATIC), szFormat = utilGetDlgItemText(hDialog, IDC_UPDATE_STATIC), pOperationData->szName, pOperationData->szDate); 
   utilDeleteString(szFormat);

   // Set URL
   utilSetWindowTextf(GetControl(hDialog, IDC_UPDATE_LINK), TEXT("Click here to <a href=\"%s\">download this update...</a>"), pOperationData->szURL);

   // Set description
   SubclassWindow(GetControl(hDialog, IDC_UPDATE_EDIT), wndprocCustomRichEditControl);
   setRichEditText(GetControl(hDialog, IDC_UPDATE_EDIT), pOperationData->pDescription, 8, TRUE, GTC_WHITE);
   //SendDlgItemMessage(hDialog, IDC_UPDATE_EDIT, EM_LINESCROLL, 0, -1000);

   Edit_SetSel(GetControl(hDialog, IDC_UPDATE_EDIT), 0, 0);
   Edit_ScrollCaret(GetControl(hDialog, IDC_UPDATE_EDIT));

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

/// Function name  : onUpdateDialog_Command
// Description     : 
// 
// HWND        hDialog        : [in] Update Dialog window handle
// CONST UINT  iControlID     : [in] Source of the message
// CONST UINT  iNotification  : [in] Notification
// 
// Return Value   : TRUE if processed, otherwise FALSE
// 
BOOL   onUpdateDialog_Command(HWND  hDialog, CONST UINT  iControlID, CONST UINT  iNotification)
{
   BOOL bResult = FALSE;   // Whether message was handled or not

   // Examine sender
   switch (iControlID)
   {
   /// [OK/CANCEL] Close dialog 
   case IDOK:
   case IDCANCEL:
      EndDialog(hDialog, iControlID);
      bResult = TRUE;
      break;
   }

   // Return result
   return bResult;
}


/// Function name  : onUpdateDialog_LinkClick
// Description     : Launches the update link in a web browser
// 
// HWND               hDialog        : [in] Update Dialog window handle
// UPDATE_OPERATION*  pOperationData : [in] Update data
// 
// Return Value   : TRUE
// 
BOOL  onUpdateDialog_LinkClick(HWND  hDialog, const UPDATE_OPERATION*  pOperationData)
{
   /// Launch URL
   return utilLaunchURL(getAppWindow(), pOperationData->szURL, SW_SHOWMAXIMIZED);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : dlgprocUpdateDialog
// Description     : Dialog procedure for the Update Dialog
// 
// 
INT_PTR  dlgprocUpdateDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   UPDATE_OPERATION*  pOperationData = (UPDATE_OPERATION*)GetWindowLong(hDialog, DWL_USER);
   NMHDR*             pHeader;
   BOOL               bResult = TRUE;

   // Examine message
   switch (iMessage)
   {
   /// [CREATE] Initialise dialog
   case WM_INITDIALOG:
      SetWindowLong(hDialog, DWL_USER, lParam);
      initUpdateDialog(hDialog, (UPDATE_OPERATION*)lParam);
      break;

   /// [COMMAND]
   case WM_COMMAND:
      bResult = onUpdateDialog_Command(hDialog, LOWORD(wParam), HIWORD(wParam));
      break;

   /// [NOTIFICATION]
   case WM_NOTIFY:
      // Prepare
      pHeader = (NMHDR*)lParam;

      /// [CLICK] Launch link
      if (pHeader->idFrom == IDC_UPDATE_LINK AND pHeader->code == NM_CLICK)
         bResult = onUpdateDialog_LinkClick(hDialog, pOperationData);         
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

