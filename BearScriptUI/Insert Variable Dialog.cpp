//
// Insert Variable Dialog.cpp : The 'new project variable' dialog for the 'Project Variables' dialog
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

/// Function name  : displayInsertVariableDialog
// Description     : Displays the 'Insert Variable' dialog
// 
// PROJECT_FILE*  pProjectFile : [in] ProjectFile 
// HWND           hParentWnd   : [in] Parent window for the dialog
// 
// Return Value   : TRUE if new ARGUMENT was inserted, otherwise FALSE
// 
BOOL  displayInsertVariableDialog(PROJECT_FILE*  pProjectFile, HWND  hParentWnd)
{
   PROJECT_VARIABLE*  pNewVariable;      // Newly created argument, if any

   // [TRACK]
   CONSOLE_ACTION1(getActiveProjectFileName());

   // Display 'Insert Variable' dialog
   pNewVariable = (PROJECT_VARIABLE*)showDialog(TEXT("INSERT_VARIABLE_DIALOG"), hParentWnd, dlgprocInsertVariableDialog, (LPARAM)pProjectFile);

   /// [NEW VARIABLE] -- Insert into ProjectFile
   if (pNewVariable)
   {
      debugProjectVariable(pNewVariable);
      insertVariableIntoProjectFile(pProjectFile, pNewVariable);
   }
   else
      CONSOLE("User cancelled create ProjectVariable dialog");

   // Return TRUE if New Variable was created
   return (pNewVariable != NULL);
}


/// Function name  : initInsertVariableDialog
// Description     : Initialise the 'Insert Variable' dialog by filling the 'Variable Type' ComboBox and setting
//                    the invalid icon / disabled OK button and creating the dialog tooltips
// 
// CONST SCRIPT_FILE*  pScriptFile : [in] ScriptFile of the active document
// HWND                hDialog     : [in] Window handle of the 'Insert Variable' dialog
// HWND                hTooltip    : [in] Window handle of the dialog's tooltip control
// 
// Return Value   : TRUE
// 
BOOL  initInsertVariableDialog(CONST PROJECT_FILE*  pProjectFile, HWND  hDialog, HWND  hTooltip)
{
   // Create tooltips
   /*addTooltipTextToControl(hTooltip, hDialog, IDC_ARGUMENT_NAME_EDIT);
   addTooltipTextToControl(hTooltip, hDialog, IDC_ARGUMENT_NAME_ICON);
   addTooltipTextToControl(hTooltip, hDialog, IDC_ARGUMENT_TYPE_COMBO);
   addTooltipTextToControl(hTooltip, hDialog, IDC_ARGUMENT_DESCRIPTION_EDIT);*/

   /// Disable 'OK' Button and initially display 'Invalid' Icon
   utilEnableDlgItem(hDialog, IDOK, FALSE);
   SendDlgItemMessage(hDialog, IDC_VARIABLE_NAME_ICON, STM_SETICON, (WPARAM)LoadIcon(getResourceInstance(), TEXT("INVALID_ICON")), NULL);

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


/// Function name  : onInsertVariableDialogCommand
// Description     : WM_COMMAND processing for the 'Insert Variable' dialog
// 
// SCRIPT_FILE*  pScriptFile    : [in] Active document's ScriptFile
// HWND          hDialog        : [in] Window handle of the 'Insert Variable' dialog
// CONST UINT    iControlID     : [in] ID of the control sending the command
// CONST UINT    iNotification  : [in] Notification code
// HWND          hCtrl          : [in] Window handle of the control sending the command
// 
// Return Value   : TRUE if processed, FALSE otherwise
// 
BOOL   onInsertVariableDialogCommand(PROJECT_FILE*  pProjectFile, HWND  hDialog, CONST UINT  iControlID, CONST UINT  iNotification, HWND  hCtrl)
{
   PROJECT_VARIABLE*  pVariable;          // new Variable
   TCHAR*             szName;             // Variable name
   UINT               iValue;             // Variable value
   BOOL               bValidation,        // Name validation result
                      bResult;            // operation result

   // Prepare
   pVariable = NULL;
   bResult   = FALSE;

   // Examine sender
   switch (iControlID)
   {
   /// [NAME CHANGE] Validate the name but do not save.  update icon and OK button
   case IDC_VARIABLE_NAME_EDIT:
   case IDC_VARIABLE_VALUE_EDIT:
      if (iNotification == EN_UPDATE)
      {
         // Prepare
         szName = utilGetDlgItemText(hDialog, IDC_VARIABLE_NAME_EDIT);

         // [CHECK] Ensure value is present and name is unique
         bValidation = (utilGetDlgItemTextLength(hDialog, IDC_VARIABLE_VALUE_EDIT) AND !findVariableInProjectFileByName(pProjectFile, szName, pVariable));

         // Display result icon and enable/disable OK button
         utilEnableDlgItem(hDialog, IDOK, bValidation);
         Static_SetIcon(GetControl(hDialog, IDC_VARIABLE_NAME_ICON), LoadIcon(getResourceInstance(), bValidation ? TEXT("VALID_ICON") : TEXT("INVALID_ICON")));

         // Cleanup
         utilDeleteString(szName);
         bResult = TRUE;
      }
      break;

   /// [OK] Generate new Variable and return as dialog result
   case IDOK:
      // Prepare
      iValue = GetDlgItemInt(hDialog, IDC_VARIABLE_VALUE_EDIT, NULL, FALSE);
      szName = utilGetDlgItemText(hDialog, IDC_VARIABLE_NAME_EDIT);

      /// Create Variable
      pVariable = createProjectVariable(szName, iValue);
      utilDeleteString(szName);

      // Fall through...

   /// [OK/CANCEL] Return Variable or NULL
   case IDCANCEL:
      // Return NULL
      EndDialog(hDialog, (INT_PTR)pVariable);
      bResult = TRUE;
      break;
   }

   // Return result
   return bResult;
}


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : dlgprocInsertVariableDialog
// Description     : Window procedure for the 'Insert Variable' dialog
// 
// 
INT_PTR  dlgprocInsertVariableDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   static PROJECT_FILE*  pProjectFile = NULL;     // Current project
   static HWND           hTooltip     = NULL;     // Dialog's Tooltip control
   BOOL                  bResult      = FALSE;

   // Examine message
   switch (iMessage)
   {
   /// [CREATION] Initialise dialog
   case WM_INITDIALOG:
      // Init dialog
      pProjectFile = (PROJECT_FILE*)lParam;
      //hTooltip     = createTooltipWindow(hDialog);
      bResult      = initInsertVariableDialog(pProjectFile, hDialog, hTooltip);
      break;

   /// [DESTRUCTION] -- Cleanup
   case WM_DESTROY:
      pProjectFile = NULL;
      bResult      = TRUE;
      //utilDeleteWindow(hTooltip);
      break;

   /// [COMMAND PROCESSING] -- Process name change, OK and CANCEL
   case WM_COMMAND:
      bResult = onInsertVariableDialogCommand(pProjectFile, hDialog, LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
      break;

   /// [OWNER DRAW]
   case WM_DRAWITEM:
      if (wParam == IDC_DIALOG_ICON)
         bResult = onOwnerDrawStaticIcon(lParam, TEXT("ADD_VARIABLE_ICON"), 96);
      break;

   /// [HELP] Invoke help
   case WM_HELP:
      bResult = displayHelp(TEXT("Project_VariablesCreate"));
      break;
   }

   // Return result
   return (bResult ? TRUE : dlgprocVistaStyleDialog(hDialog, iMessage, wParam, lParam));
}


