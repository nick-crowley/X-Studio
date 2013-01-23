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

//enum PAGE_RESULT { Cancelled, Edited, Created };

struct  PAGE_DIALOG_DATA
{
   LANGUAGE_DOCUMENT*  pDocument;      // Document
   GAME_PAGE*          pEditPage;      // NULL to create new Page, non-NULL to edit existing page
};

// Functions
BOOL     initInsertPageDialog(PAGE_DIALOG_DATA*  pDialogData, HWND  hDialog, HWND  hTooltip);
BOOL     onInsertPageDialogCommand(PAGE_DIALOG_DATA*  pDialogData, HWND  hDialog, CONST UINT  iControlID, CONST UINT  iNotification, HWND  hCtrl);

// Dialog Proc
INT_PTR  dlgprocInsertPageDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : displayInsertPageDialog
// Description     : Displays the 'Insert Page' dialog
// 
// LANGUAGE_DOCUMENT*  pDocument   : [in] Language Document
// GAME_PAGE*          pPageToEdit : [in] Existing page to edit, if any, otherwise NULL
// HWND                hParentWnd  : [in] Parent window for the dialog
// 
// Return Value   : TRUE if new GamePage was inserted, otherwise FALSE
// 
GAME_PAGE*  displayInsertPageDialog(LANGUAGE_DOCUMENT*  pDocument, GAME_PAGE*  pPageToEdit, HWND  hParentWnd)
{
   PAGE_DIALOG_DATA  oData = { pDocument, pPageToEdit };

   // Display 'Insert Page' dialog
   return (GAME_PAGE*)DialogBoxParam(getResourceInstance(), TEXT("INSERT_PAGE_DIALOG"), hParentWnd, dlgprocInsertPageDialog, (LPARAM)&oData);
}


/// Function name  : initInsertPageDialog
// Description     : Initialise the 'Insert Page' dialog by setting the invalid icon / disabled OK button and creating the dialog tooltips
// 
// LANGUAGE_DOCUMENT*  pDocument : [in] Language Document
// HWND                hDialog   : [in] Window handle of the 'Insert Page' dialog
// HWND                hTooltip  : [in] Window handle of the dialog's tooltip control
// 
// Return Value   : TRUE
// 
BOOL  initInsertPageDialog(PAGE_DIALOG_DATA*  pDialogData, HWND  hDialog, HWND  hTooltip)
{
   // Create tooltips
   /*addTooltipTextToControl(hTooltip, hDialog, IDC_ARGUMENT_NAME_EDIT);
   addTooltipTextToControl(hTooltip, hDialog, IDC_ARGUMENT_NAME_ICON);
   addTooltipTextToControl(hTooltip, hDialog, IDC_ARGUMENT_TYPE_COMBO);
   addTooltipTextToControl(hTooltip, hDialog, IDC_ARGUMENT_DESCRIPTION_EDIT);*/

   /// [EDIT EXISTING] Display properties
   if (pDialogData->pEditPage)
   {
      SetDlgItemInt(hDialog,  IDC_PAGE_ID_EDIT,          pDialogData->pEditPage->iPageID, FALSE);
      SetDlgItemText(hDialog, IDC_PAGE_TITLE_EDIT,       pDialogData->pEditPage->szTitle);
      SetDlgItemText(hDialog, IDC_PAGE_DESCRIPTION_EDIT, pDialogData->pEditPage->szDescription);
      CheckDlgButton(hDialog, IDC_PAGE_VOICED_CHECK,     pDialogData->pEditPage->bVoice);

      // Enable 'OK' --> 'Valid' icon
      utilEnableDlgItem(hDialog, IDOK, TRUE);
      SendDlgItemMessage(hDialog, IDC_PAGE_ID_ICON, STM_SETICON, (WPARAM)LoadIcon(getResourceInstance(), TEXT("VALID_ICON")), NULL);
   }
   /// [NEW] Just display 'Invalid'
   else
   {
      // Disable 'OK' --> 'Invalid' Icon
      utilEnableDlgItem(hDialog, IDOK, FALSE);
      SendDlgItemMessage(hDialog, IDC_PAGE_ID_ICON, STM_SETICON, (WPARAM)LoadIcon(getResourceInstance(), TEXT("INVALID_ICON")), NULL);
   }

   // [OWNER DRAW]
   utilAddWindowStyle(GetControl(hDialog, IDC_DIALOG_ICON),      SS_OWNERDRAW);
   utilAddWindowStyle(GetControl(hDialog, IDC_DIALOG_TITLE),     SS_OWNERDRAW);
   utilAddWindowStyle(GetControl(hDialog, IDC_DIALOG_HEADING_1), SS_OWNERDRAW);
   utilAddWindowStyle(GetControl(hDialog, IDC_DIALOG_HEADING_2), SS_OWNERDRAW);

   // [DIALOG]
   SetWindowText(hDialog, getAppName());
   utilCentreWindow(GetParent(hDialog), hDialog);

   // Return TRUE
   return TRUE;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onInsertPageDialogCommand
// Description     : WM_COMMAND processing for the 'Insert Page' dialog
// 
// LANGUAGE_DOCUMENT*  pDocument     : [in] Language Document
// HWND                hDialog       : [in] Window handle of the 'Insert Page' dialog
// CONST UINT          iControlID    : [in] ID of the control sending the command
// CONST UINT          iNotification : [in] Notification code
// HWND                hCtrl         : [in] Window handle of the control sending the command
// 
// Return Value   : TRUE if processed, FALSE otherwise
// 
BOOL   onInsertPageDialogCommand(PAGE_DIALOG_DATA*  pDialogData, HWND  hDialog, CONST UINT  iControlID, CONST UINT  iNotification, HWND  hCtrl)
{
   GAME_PAGE*  pGamePage;          // Page being created
   TCHAR      *szTitle,            // Title
              *szDescription;      // Description
   UINT        iPageID;            // Page ID
   BOOL        bVoiced,            // Voiced
               bValidation;        // Whether PageID is unique

   // Examine sender
   switch (iControlID)
   {
   /// [PAGE ID] Validate PageID
   case IDC_PAGE_ID_EDIT:
      // [CHANGED]
      if (iNotification == EN_UPDATE)
      {
         // [CHECK] Ensure PageID is unique / unchanged
         iPageID = GetDlgItemInt(hDialog, IDC_PAGE_ID_EDIT, NULL, FALSE);
         bValidation = (pDialogData->pEditPage AND iPageID == pDialogData->pEditPage->iPageID) OR !findLanguageDocumentGamePageByID(pDialogData->pDocument, iPageID, pGamePage);

         // [ICON/OK] Set 'OK' button state and Validation icon
         utilEnableDlgItem(hDialog, IDOK, bValidation);
         Static_SetIcon(GetControl(hDialog, IDC_PAGE_ID_ICON), LoadIcon(getResourceInstance(), bValidation ? TEXT("VALID_ICON") : TEXT("INVALID_ICON")));
         return TRUE;
      }
      break;

   /// [OK] Generate new Page and return as dialog result
   case IDOK:
      // Prepare
      iPageID = GetDlgItemInt(hDialog, IDC_PAGE_ID_EDIT, NULL, FALSE);
      szTitle = utilGetDlgItemText(hDialog, IDC_PAGE_TITLE_EDIT);
      szDescription = utilGetDlgItemText(hDialog, IDC_PAGE_DESCRIPTION_EDIT);
      bVoiced = IsDlgButtonChecked(hDialog, IDC_PAGE_VOICED_CHECK);

      // Generate + Return new page
      pGamePage = createGamePage(iPageID, szTitle, szDescription, bVoiced);
      utilDeleteStrings(szTitle, szDescription);
      EndDialog(hDialog, (INT_PTR)pGamePage);
      return TRUE;

   /// [CANCEL] Return NULL
   case IDCANCEL:
      EndDialog(hDialog, NULL);
      return TRUE;
   }

   // [UNHANDLED]
   return FALSE;
}


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : dlgprocInsertPageDialog
// Description     : Window procedure for the 'Insert Page' dialog
// 
// 
INT_PTR  dlgprocInsertPageDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   PAGE_DIALOG_DATA*  pDialogData = (PAGE_DIALOG_DATA*)GetWindowLong(hDialog, DWL_USER);   
   BOOL               bResult     = FALSE;

   // Examine message
   switch (iMessage)
   {
   /// [CREATION] Initialise dialog
   case WM_INITDIALOG:
      // Store data + Init Dialog
      SetWindowLong(hDialog, DWL_USER, lParam);
      bResult = initInsertPageDialog((PAGE_DIALOG_DATA*)lParam, hDialog, NULL);
      break;

   /// [COMMAND PROCESSING] -- Process name change, OK and CANCEL
   case WM_COMMAND:
      bResult = onInsertPageDialogCommand(pDialogData, hDialog, LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
      break;

   /// [OWNER DRAW]
   case WM_DRAWITEM:
      if (wParam == IDC_DIALOG_ICON)
         bResult = onOwnerDrawStaticIcon(lParam, TEXT("INSERT_PAGE_ICON"), 96);
      break;

   /// [HELP] Invoke help
   case WM_HELP:
      bResult = displayHelp(TEXT("TODO"));
      break;
   }

   // Return result
   return (bResult ? TRUE : dlgprocVistaStyleDialog(hDialog, iMessage, wParam, lParam));
}


