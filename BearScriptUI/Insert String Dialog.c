//
// Insert String Dialog.cpp : The 'Insert String' dialog for the language document
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        MACROS
/// /////////////////////////////////////////////////////////////////////////////////////////

// onException: Display 
#define  ON_EXCEPTION()    displayException(pException);

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////

// Functions
BOOL     initInsertStringDialog(LANGUAGE_DOCUMENT*  pDocument, HWND  hDialog, HWND  hTooltip);
BOOL     onInsertStringDialogCommand(LANGUAGE_DOCUMENT*  pDocument, HWND  hDialog, CONST UINT  iControlID, CONST UINT  iNotification, HWND  hCtrl);

// Dialog Proc
INT_PTR  dlgprocInsertStringDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : displayInsertStringDialog
// Description     : Displays the 'Insert String' dialog
// 
// LANGUAGE_DOCUMENT*  pDocument   : [in] Language Document
// HWND                hParentWnd  : [in] Parent window for the dialog
// 
// Return Value   : New GameString containing specified properties, or NULL if user cancelled
// 
GAME_STRING*  displayInsertStringDialog(LANGUAGE_DOCUMENT*  pDocument, HWND  hParentWnd)
{
   // [TRACK]
   CONSOLE_COMMAND();

   // Display 'Insert String' dialog
   return (GAME_STRING*)showDialog(TEXT("INSERT_STRING_DIALOG"), hParentWnd, dlgprocInsertStringDialog, (LPARAM)pDocument);
}


/// Function name  : initInsertStringDialog
// Description     : Initialise the 'Insert String' dialog by setting the invalid icon / disabled OK button and creating the dialog tooltips
// 
// LANGUAGE_DOCUMENT*  pDocument : [in] Language Document
// HWND                hDialog   : [in] Window handle of the 'Insert String' dialog
// HWND                hTooltip  : [in] Window handle of the dialog's tooltip control
// 
// Return Value   : TRUE
// 
BOOL  initInsertStringDialog(LANGUAGE_DOCUMENT*  pDocument, HWND  hDialog, HWND  hTooltip)
{
   TCHAR  *szFormat,
          *szText;

   // Create tooltips
   addTooltipTextToControl(hTooltip, hDialog, IDC_STRING_ID_EDIT);
   addTooltipTextToControl(hTooltip, hDialog, IDC_STRING_ID_ICON);

   // Popuplate dialog description
   szFormat = utilGetDlgItemText(hDialog, IDC_STRING_ID_STATIC);
   SetDlgItemText(hDialog, IDC_STRING_ID_STATIC, szText = utilCreateStringf(128, szFormat, pDocument->pCurrentPage->iPageID));
   
   // Display next available ID + 'Valid' Icon
   SetDlgItemInt(hDialog,  IDC_STRING_ID_EDIT, identifyLanguagePageStringNextID(pDocument), FALSE);
   Static_SetIcon(GetControl(hDialog, IDC_STRING_ID_ICON), LoadIcon(getResourceInstance(), TEXT("VALID_ICON")));

   // [OWNER DRAW]
   utilAddWindowStyle(GetControl(hDialog, IDC_DIALOG_ICON),      SS_OWNERDRAW);
   utilAddWindowStyle(GetControl(hDialog, IDC_DIALOG_TITLE),     SS_OWNERDRAW);
   utilAddWindowStyle(GetControl(hDialog, IDC_DIALOG_HEADING_1), SS_OWNERDRAW);

   // [DIALOG]
   SetWindowText(hDialog, getAppName());
   utilCentreWindow(GetParent(hDialog), hDialog);

   // Return TRUE
   utilDeleteStrings(szFormat, szText);
   return TRUE;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onInsertStringDialogCommand
// Description     : WM_COMMAND processing for the 'Insert String' dialog
// 
// LANGUAGE_DOCUMENT*  pDocument     : [in] Language Document
// HWND                hDialog       : [in] Window handle of the 'Insert String' dialog
// CONST UINT          iControlID    : [in] ID of the control sending the command
// CONST UINT          iNotification : [in] Notification code
// HWND                hCtrl         : [in] Window handle of the control sending the command
// 
// Return Value   : TRUE if processed, FALSE otherwise
// 
BOOL   onInsertStringDialogCommand(LANGUAGE_DOCUMENT*  pDocument, HWND  hDialog, CONST UINT  iControlID, CONST UINT  iNotification, HWND  hCtrl)
{
   GAME_STRING*   pGameString;        // String being created
   UINT           iStringID;          // String ID
   BOOL           bValidation,        // Whether StringID is unique
                  bResult = TRUE;

   // Examine sender
   switch (iControlID)
   {
   /// [ID CHANGED] Validate new ID
   case IDC_STRING_ID_EDIT:
      // [CHANGED]
      if (bResult = (iNotification == EN_UPDATE))
      {
         // [CHECK] Ensure StringID is unique / unchanged
         iStringID   = GetDlgItemInt(hDialog, IDC_STRING_ID_EDIT, NULL, FALSE);
         bValidation = validateLanguagePageStringID(pDocument, iStringID);

         // [ICON/OK] Set 'OK' button state and Validation icon
         utilEnableDlgItem(hDialog, IDOK, bValidation);
         Static_SetIcon(GetControl(hDialog, IDC_STRING_ID_ICON), LoadIcon(getResourceInstance(), bValidation ? TEXT("VALID_ICON") : TEXT("INVALID_ICON")));
      }
      break;

   /// [OK] Create/Return new GameString
   case IDOK:
      // Create new string
      iStringID   = GetDlgItemInt(hDialog, IDC_STRING_ID_EDIT, NULL, FALSE);
      pGameString = createGameString(TEXT("NEW STRING"), iStringID, pDocument->pCurrentPage->iPageID, ST_INTERNAL);

      // Return as result
      EndDialog(hDialog, (INT_PTR)pGameString);
      break;

   /// [CANCEL] Return NULL
   case IDCANCEL:
      EndDialog(hDialog, NULL);
      break;

   // [UNHANDLED]
   default:
      bResult = FALSE;
   }

   // Return result
   return bResult;
}


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : dlgprocInsertStringDialog
// Description     : Window procedure for the 'Insert String' dialog
// 
// 
INT_PTR  dlgprocInsertStringDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   LANGUAGE_DOCUMENT*  pDocument = (LANGUAGE_DOCUMENT*)GetWindowLong(hDialog, DWL_USER);   
   static HWND         hTooltip  = NULL;
   BOOL                bResult   = FALSE;

   TRY
   {
      // Examine message
      switch (iMessage)
      {
      /// [CREATION] Store Document + Initialise dialog
      case WM_INITDIALOG:
         SetWindowLong(hDialog, DWL_USER, lParam);
         bResult = initInsertStringDialog((LANGUAGE_DOCUMENT*)lParam, hDialog, hTooltip = createTooltipWindow(hDialog));
         break;

      /// [DESTROY] Destroy Tooltip
      case WM_DESTROY:
         utilDeleteWindow(hTooltip);
         break;

      /// [COMMAND PROCESSING] -- Process name change, OK and CANCEL
      case WM_COMMAND:
         bResult = onInsertStringDialogCommand(pDocument, hDialog, LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
         break;

      /// [OWNER DRAW]
      case WM_DRAWITEM:
         if (wParam == IDC_DIALOG_ICON)
            bResult = onOwnerDrawStaticIcon(lParam, TEXT("INSERT_PAGE_ICON"), 96);
         break;

      /// [HELP] Invoke help
      case WM_HELP:
         bResult = displayHelp(TEXT("Language_Pages"));
         break;
      }

      // Return result
      return (bResult ? TRUE : dlgprocVistaStyleDialog(hDialog, iMessage, wParam, lParam));
   }
   /// [EXCEPTION HANDLER]
   CATCH3("iMessage=%s  wParam=%d  lParam=%d", identifyMessage(iMessage), wParam, lParam);
   return dlgprocVistaStyleDialog(hDialog, iMessage, wParam, lParam);
}


