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



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : displayInsertArgumentDialog
// Description     : Displays the 'Insert Argument' dialog
// 
// SCRIPT_FILE*  pScriptFile  : [in] ScriptFile to contain the new ARGUMENT
// HWND          hParentWnd   : [in] Parent window for the dialog
// 
// Return Value   : TRUE if new ARGUMENT was inserted, otherwise FALSE
// 
BOOL  displayInsertArgumentDialog(SCRIPT_FILE*  pScriptFile, HWND  hParentWnd)
{
   ARGUMENT*  pNewArgument;      // Newly created argument, if any

   // Display 'Insert Argument' dialog
   pNewArgument = (ARGUMENT*)DialogBoxParam(getResourceInstance(), TEXT("INSERT_ARGUMENT_DIALOG"), hParentWnd, dlgprocInsertArgumentDialog, (LPARAM)pScriptFile);

   /// [NEW ARGUMENT] -- Insert into ScriptFile
   if (pNewArgument)
   {
      // Inset Argument using next available ID
      pNewArgument->iID = (1 + getTreeNodeCount(pScriptFile->pArgumentTreeByID));
      appendArgumentToScriptFile(pScriptFile, pNewArgument);
   }

   // Return TRUE if ARGUMENT was created
   return (pNewArgument != NULL);
}


/// Function name  : initInsertArgumentDialog
// Description     : Initialise the 'Insert Argument' dialog by filling the 'Argument Type' ComboBox and setting
//                    the invalid icon / disabled OK button and creating the dialog tooltips
// 
// CONST SCRIPT_FILE*  pScriptFile : [in] ScriptFile of the active document
// HWND                hDialog     : [in] Window handle of the 'Insert Argument' dialog
// HWND                hTooltip    : [in] Window handle of the dialog's tooltip control
// 
// Return Value   : TRUE
// 
BOOL  initInsertArgumentDialog(CONST SCRIPT_FILE*  pScriptFile, HWND  hDialog, HWND  hTooltip)
{
   // Create tooltips
   addTooltipTextToControl(hTooltip, hDialog, IDC_ARGUMENT_NAME_EDIT);
   addTooltipTextToControl(hTooltip, hDialog, IDC_ARGUMENT_NAME_ICON);
   addTooltipTextToControl(hTooltip, hDialog, IDC_ARGUMENT_TYPE_COMBO);
   addTooltipTextToControl(hTooltip, hDialog, IDC_ARGUMENT_DESCRIPTION_EDIT);

   /// Populate 'Argument Types' ComboBox and select 'Value'
   performParameterSyntaxComboBoxPopulation(GetDlgItem(hDialog, IDC_ARGUMENT_TYPE_COMBO), PS_VALUE);

   /// Disable 'OK' Button and initially display 'Invalid' Icon
   utilEnableDlgItem(hDialog, IDOK, FALSE);
   SendDlgItemMessage(hDialog, IDC_ARGUMENT_NAME_ICON, STM_SETICON, (WPARAM)LoadIcon(getResourceInstance(), TEXT("INVALID_ICON")), NULL);

   // [OWNER DRAW]
   utilAddWindowStyle(GetControl(hDialog, IDC_ARGUMENT_TITLE_STATIC), SS_OWNERDRAW);
   utilAddWindowStyle(GetControl(hDialog, IDC_ARGUMENT_REQUIRED_STATIC), SS_OWNERDRAW);
   utilAddWindowStyle(GetControl(hDialog, IDC_ARGUMENT_OPTIONAL_STATIC), SS_OWNERDRAW);

   // Centre dialog
   utilCentreWindow(getAppWindow(), hDialog);

   // Return TRUE
   return TRUE;
}


/// Function name  : performParameterSyntaxComboBoxPopulation
// Description     : Populates a dialog ComboBox with parameter syntax entries
// 
// HWND                    hDialog            : [in]            Dialog containing the combo box
// CONST UINT              iControlID         : [in]            Control ID of the combo box
// CONST PARAMETER_SYNTAX  eInitialSelection  : [in] [optional] Syntax of the item to initially select, or PS_UNDETERMINED to select no syntax
// 
VOID  performParameterSyntaxComboBoxPopulation(HWND  hCtrl, CONST PARAMETER_SYNTAX  eInitialSelection)
{
   GAME_STRING*   pSyntaxString;      // Lookup for parameter syntax names
   INT            iItemIndex;       

   /// Iterate through all parameter syntax
   for (UINT eSyntax = FIRST_PARAMETER_SYNTAX; eSyntax <= LAST_PARAMETER_SYNTAX; eSyntax++)
   {
      // [CHECK] Skip 'low-level' syntax the user will not use
      switch (eSyntax)
      {
      /// [INTERNAL SYNTAX] Don't add these entries
      case PS_CONDITION:
      case PS_COMMENT:
      case PS_LABEL_NAME:
      case PS_LABEL_NUMBER:
      case PS_RETURN_OBJECT:
      case PS_REFERENCE_OBJECT:
      case PS_RETURN_OBJECT_IF:
      case PS_RETURN_OBJECT_IF_START:
      case PS_INTERRUPT_RETURN_OBJECT_IF:
      case PS_EXPRESSION:
         // [CHECK] Prevent the addition of these entries unless they should be the initial selection
         if (eSyntax != eInitialSelection)
            continue;

         // Fall through...

      /// [VALID SYNTAX] Add to the ComboBox
      default:
         // Lookup syntax
         if (findGameStringByID(eSyntax, identifyGamePageIDFromDataType(DT_SCRIPTDEF), pSyntaxString))
         {
            /// Insert item and store syntax ID as item data
            iItemIndex = appendCustomComboBoxItemEx(hCtrl, pSyntaxString->szText, NULL, TEXT("FUNCTION_ICON"), eSyntax);

            //// Use syntax name as item text
            //iItemIndex = SendDlgItemMessageW(hDialog, iControlID, CB_ADDSTRING, NULL, (LPARAM)pSyntaxString->szText);
            //SendDlgItemMessage(hDialog, iControlID, CB_SETITEMDATA, iItemIndex, eSyntax);

            // [CHECK] Select this item if appropriate
            if (eSyntax == eInitialSelection)
               ComboBox_SetCurSel(hCtrl, iItemIndex);
         }
         break;
      }
   }
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////


/// Function name  : onInsertArgumentDialogCommand
// Description     : WM_COMMAND processing for the 'Insert Argument' dialog
// 
// SCRIPT_FILE*  pScriptFile    : [in] Active document's ScriptFile
// HWND          hDialog        : [in] Window handle of the 'Insert Argument' dialog
// CONST UINT    iControlID     : [in] ID of the control sending the command
// CONST UINT    iNotification  : [in] Notification code
// HWND          hCtrl          : [in] Window handle of the control sending the command
// 
// Return Value   : TRUE if processed, FALSE otherwise
// 
BOOL   onInsertArgumentDialogCommand(SCRIPT_FILE*  pScriptFile, HWND  hDialog, CONST UINT  iControlID, CONST UINT  iNotification, HWND  hCtrl)
{
   ARGUMENT      *pArgument;              // ScriptFile's argument list iterator.   Also used to create ARGUMENT for return.
   TCHAR         *szArgumentName,         // Name of the new argument
                 *szArgumentDescription;  // Description of the new argument
   BOOL           bValidation;            // Validation result of the current argument name
   BOOL           bResult;

   // Prepare
   bResult = FALSE;

   // Examine sender
   switch (iControlID)
   {
   /// [NAME CHANGE] -- Validate the Argument name, alter the OK button and icon appropriately.
   case IDC_ARGUMENT_NAME_EDIT:
      if (iNotification == EN_UPDATE)
      {
         // Prepare
         szArgumentName = utilGetDlgItemText(hDialog, iControlID);

         // Attempt to change Argument name
         bValidation = verifyScriptFileArgumentName(pScriptFile, szArgumentName, NULL);

         // Display result icon and enable/disable OK button
         utilEnableDlgItem(hDialog, IDOK, bValidation);
         SendDlgItemMessage(hDialog, IDC_ARGUMENT_NAME_ICON, STM_SETICON, (WPARAM)LoadIcon(getResourceInstance(), bValidation ? TEXT("VALID_ICON") : TEXT("INVALID_ICON")), NULL);

         // Cleanup
         utilDeleteString(szArgumentName);
         bResult = TRUE;
      }
      break;

   /// [OK] Generate new ARGUMENT and return as dialog result
   case IDOK:
      // Prepare
      bResult = TRUE;

      // Create new ARGUMENT
      szArgumentName        = utilGetDlgItemText(hDialog, IDC_ARGUMENT_NAME_EDIT);
      szArgumentDescription = utilGetDlgItemText(hDialog, IDC_ARGUMENT_DESCRIPTION_EDIT);
      pArgument             = createArgumentFromName(szArgumentName, szArgumentDescription);

      // Set argument type (from the currently selected item's item-data)
      hCtrl = GetDlgItem(hDialog, IDC_ARGUMENT_TYPE_COMBO);
      pArgument->eType = (PARAMETER_SYNTAX)getCustomComboBoxItemParam(hCtrl, ComboBox_GetCurSel(hCtrl));

      /*iSelectedSyntax = SendDlgItemMessage(hDialog, IDC_ARGUMENT_TYPE_COMBO, CB_GETCURSEL, NULL, NULL);
      pArgument->eType = (PARAMETER_SYNTAX)SendDlgItemMessage(hDialog, IDC_ARGUMENT_TYPE_COMBO, CB_GETITEMDATA, iSelectedSyntax, NULL);*/

      // Cleanup and return ARGUMENT as the dialog result
      utilDeleteStrings(szArgumentName, szArgumentDescription);
      EndDialog(hDialog, (INT_PTR)pArgument);
      break;

   /// [CANCEL] Return NULL as dialog result
   case IDCANCEL:
      // Prepare
      bResult = TRUE;

      // Return NULL
      EndDialog(hDialog, NULL);
      break;
   }

   // Return result
   return bResult;
}


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////


/// Function name  : dlgprocInsertArgumentDialog
// Description     : Window procedure for the 'Insert Argument' dialog
// 
// 
INT_PTR  dlgprocInsertArgumentDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   static SCRIPT_FILE*  pScriptFile = NULL;     // ScriptFile of the active document
   static HWND          hTooltip = NULL;        // Dialog's Tooltip control
   BOOL                 bResult;

   // Prepare
   bResult = FALSE;

   // Examine message
   switch (iMessage)
   {
   /// [CREATION] -- Initalise the 'Types' ComboBox
   case WM_INITDIALOG:
      // Prepare
      pScriptFile = (SCRIPT_FILE*)lParam;
      hTooltip    = createTooltipWindow(hDialog);

      // Init dialog
      bResult = initInsertArgumentDialog(pScriptFile, hDialog, hTooltip);
      break;

   /// [DESTRUCTION] -- Cleanup
   case WM_DESTROY:
      // Cleanup
      pScriptFile = NULL;
      utilDeleteWindow(hTooltip);
      break;

   /// [COMMAND PROCESSING] -- Process name change, OK and CANCEL
   case WM_COMMAND:
      bResult = onInsertArgumentDialogCommand(pScriptFile, hDialog, LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
      break;

   /// [HELP]
   case WM_HELP:
      bResult = displayHelp(TEXT("Properties_ArgumentsCreate"));
      break;

   /// [OWNER DRAW]
   case WM_DRAWITEM:
      switch (wParam)
      {
      case IDC_ARGUMENT_DIALOG_ICON:      bResult = onOwnerDrawStaticIcon(lParam, TEXT("ARGUMENT_ICON"), 96);     break;
      case IDC_ARGUMENT_TITLE_STATIC:     bResult = onOwnerDrawStaticTitle(lParam);                               break;
      case IDC_ARGUMENT_REQUIRED_STATIC:  bResult = onOwnerDrawStaticHeading(lParam);                             break;
      case IDC_ARGUMENT_OPTIONAL_STATIC:  bResult = onOwnerDrawStaticHeading(lParam);                             break;
      default:                            bResult = onWindow_DrawItem((DRAWITEMSTRUCT*)lParam);                   break;
      }
      break;

   /// [CUSTOM COMBOBOX]
   case WM_MEASUREITEM:  bResult = onWindow_MeasureComboBox((MEASUREITEMSTRUCT*)lParam, ITS_SMALL, ITS_SMALL);  break;
   case WM_DELETEITEM:   bResult = onWindow_DeleteItem((DELETEITEMSTRUCT*)lParam);                              break;
   case WM_COMPAREITEM:  SetWindowLong(hDialog, DWL_MSGRESULT, compareCustomComboBoxItems(wParam, (COMPAREITEMSTRUCT*)lParam));  bResult = TRUE;  break;
   }

   // Return result
   return (bResult ? TRUE : dlgprocVistaStyleDialog(hDialog, iMessage, wParam, lParam));
}


