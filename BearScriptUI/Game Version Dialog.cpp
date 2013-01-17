//
// Game Version Dialog.cpp : Allows the user to select their game version
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                     CONSTANTS / GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : displayGameVersionDialog
// Description     : Displays GameVersion selection dialog
// 
// HWND        hParent      : [in] Parent window
// CONST BOOL  bOnlyTCAndAP : [in] TRUE to enable choice between X3:TC and X3:AP. FALSE to enable choice between all versions
// 
// Return Value   : Chosen GameVersion, or GV_UNKNOWN if user cancels
// 
GAME_VERSION  displayGameVersionDialog(HWND  hParent, CONST BOOL  bOnlyTCAndAP)
{
   return (GAME_VERSION)DialogBoxParam(getResourceInstance(), TEXT("GAME_VERSION_DIALOG"), hParent, dlgprocGameVersionDialog, bOnlyTCAndAP);
}


/// Function name  : initGameVersionDialog
// Description     : Set dialog icons. Enable/Disable choices
// 
// HWND        hDialog      : [in] GameVersion dialog
// CONST BOOL  bOnlyTCAndAP : [in] Whether to only disable choice between X3:TC and X3:AP
// 
// Return Value   : TRUE
// 
BOOL  initGameVersionDialog(HWND  hDialog, CONST BOOL  bOnlyTCAndAP)
{
   CONST UINT   iThreatReunionControls[4] = { IDC_VERSION_THREAT_ICON, IDC_VERSION_REUNION_ICON, IDC_VERSION_THREAT, IDC_VERSION_REUNION };      // Button/Static controls for X2 and ReUnion

   /// Set icons
   Button_SetIcon(GetControl(hDialog, IDC_VERSION_THREAT_ICON),          LoadIcon(getResourceInstance(), TEXT("THREAT_ICON")));
   Button_SetIcon(GetControl(hDialog, IDC_VERSION_REUNION_ICON),         LoadIcon(getResourceInstance(), TEXT("REUNION_ICON")));
   Button_SetIcon(GetControl(hDialog, IDC_VERSION_TERRAN_CONFLICT_ICON), LoadIcon(getResourceInstance(), TEXT("TERRAN_CONFLICT_ICON")));
   Button_SetIcon(GetControl(hDialog, IDC_VERSION_ALBION_PRELUDE_ICON),  LoadIcon(getResourceInstance(), TEXT("ALBION_PRELUDE_ICON")));

   /// Display version strings
   for (UINT iVersion = GV_THREAT; iVersion <= GV_ALBION_PRELUDE; iVersion++)
      SetDlgItemText(hDialog, (IDC_VERSION_THREAT + iVersion), identifyGameVersionString((GAME_VERSION)iVersion));

   // [ONLY TC/AP] Disable X2 and Reunion options
   if (bOnlyTCAndAP)
      for (UINT iControl = 0; iControl < 4; iControl++)
         utilEnableDlgItem(hDialog, iThreatReunionControls[iControl], FALSE);
   
   /// Select Albion Prelude
   CheckDlgButton(hDialog, IDC_VERSION_ALBION_PRELUDE_ICON, BST_CHECKED);

   // [OWNER DRAW]
   utilAddWindowStyle(GetControl(hDialog, IDC_VERSION_DIALOG_TITLE), SS_OWNERDRAW);
   utilAddWindowStyle(GetControl(hDialog, IDC_VERSION_AVAILABLE_HEADING), SS_OWNERDRAW);

   // [DIALOG] Centre dialog and set caption
   SetWindowText(hDialog, getAppName());
   utilCentreWindow(getAppWindow() ? getAppWindow() : GetDesktopWindow(), hDialog);

   // Return TRUE
   return TRUE;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onGameVersionDialog_Command
// Description     : Returns selected GameVersion as dialog result
// 
// HWND        hDialog        : [in] GameVersion dialog
// CONST UINT  iControl       : [in] Control
// CONST UINT  iNotification  : [in] Notification
// 
// Return Value   : TRUE if handled, otherwise FALSE
// 
BOOL   onGameVersionDialog_Command(HWND  hDialog, CONST UINT  iControl, CONST UINT  iNotification)
{
   CONST UINT   iRadioButtons[4] = { IDC_VERSION_THREAT_ICON, IDC_VERSION_REUNION_ICON, IDC_VERSION_TERRAN_CONFLICT_ICON, IDC_VERSION_ALBION_PRELUDE_ICON };      // Ratio button control IDs
   GAME_VERSION eSelection;         // Dialog result

   // Prepare
   eSelection = GV_UNKNOWN;

   // Examine control
   switch (iControl)
   {
   /// [OK] Return GAME VERSION
   case IDOK:
      // Determine selected GameVersion
      for (GAME_VERSION  eVersion = GV_THREAT; eVersion <= GV_ALBION_PRELUDE; eVersion = (GAME_VERSION)(eVersion + 1))
         if (IsDlgButtonChecked(hDialog, iRadioButtons[eVersion]))
            // [FOUND] Set version
            eSelection = eVersion;

      // Fall through...

   /// [CANCEL] Return GV_UKNOWN
   case IDCANCEL:
      EndDialog(hDialog, eSelection);
      return TRUE;
   }

   // [UNHANDLED] Return FALSE
   return FALSE;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : dlgprocGameVersionDialog
// Description     : Window proc for dialog asking the user to manually specify the game version
// 
// 
INT_PTR  dlgprocGameVersionDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   BOOL   bResult = FALSE;

   // Examine message
   switch (iMessage)
   {
   /// [CREATION] 
   case WM_INITDIALOG:
      bResult = initGameVersionDialog(hDialog, (BOOL)lParam);
      break;

   /// [COMMAND]
   case WM_COMMAND:
      bResult = onGameVersionDialog_Command(hDialog, LOWORD(wParam), HIWORD(wParam));
      break;

   /// [OWNER DRAW]
   case WM_DRAWITEM:
      switch (wParam)
      {
      case IDC_VERSION_DIALOG_ICON:       bResult = onOwnerDrawStaticIcon(lParam, TEXT("MAIN_WINDOW"), 96);     break;
      case IDC_VERSION_DIALOG_TITLE:      bResult = onOwnerDrawStaticTitle(lParam);    break;
      case IDC_VERSION_AVAILABLE_HEADING: bResult = onOwnerDrawStaticHeading(lParam);  break;
      }
      break;
   }

   // [UNHANDLED] Pass to VistaStyle procedure
   return (bResult ? TRUE : dlgprocVistaStyleDialog(hDialog, iMessage, wParam, lParam));
}