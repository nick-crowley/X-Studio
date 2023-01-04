//
// Tutorial Dialog.cpp : 
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

/// Function name  : displayTutorialDialog
// Description     : Displays a tutorial dialog
// 
// HWND                   hParentWnd    : [in] Parent window
// CONST TUTORIAL_WINDOW  eDialogID     : [in] ID of tutorial to display
// CONST BOOL             bForceDisplay : [in] TRUE to display even if used before
// 
// Return Value   : TRUE if displayed, FALSE if Tutorial already displayed or Tutorial mode disabled
// 
BOOL  displayTutorialDialog(HWND  hParentWnd, CONST TUTORIAL_WINDOW  eDialogID, CONST BOOL  bForceDisplay)
{
   BOOL  bResult = FALSE;

   /// [CHECK] Are we in tutorial mode?
   if (bForceDisplay OR (getAppPreferences()->bTutorialMode AND getAppPreferences()->bTutorialsRemaining[eDialogID]))
   {
      CONSOLE_COMMAND_BOLD();

      /// [SUCCESS] Display approprate dialog
      switch (eDialogID)
      {
      // [FILE OPTIONS]
      case TW_FILE_OPTIONS: 
         bResult = (BOOL)showDialog(TEXT("TUTORIAL_FILE_OPTIONS"), hParentWnd, dlgprocTutorialDialog, NULL);  
         ERROR_CHECK("Creating FileOptions Tutorial dialog", bResult > 0);
         break;

      // [GAME DATA]
      case TW_GAME_DATA: 
         bResult = (BOOL)showDialog(TEXT("TUTORIAL_GAME_DATA"), hParentWnd, dlgprocTutorialDialog, NULL);    
         ERROR_CHECK("Creating Game-Data Tutorial dialog", bResult > 0);
         break;

      // [GAME FOLDER]
      case TW_GAME_FOLDER: 
         bResult = (BOOL)showDialog(TEXT("TUTORIAL_GAME_FOLDER"), hParentWnd, dlgprocTutorialDialog, NULL);  
         ERROR_CHECK("Creating Game-Folder Tutorial dialog", bResult > 0);
         break;

      // [GAME OBJECTS]
      case TW_GAME_OBJECTS: 
         bResult = (BOOL)showDialog(TEXT("TUTORIAL_GAME_OBJECTS"), hParentWnd, dlgprocTutorialDialog, NULL);    
         ERROR_CHECK("Creating GameObjects Tutorial dialog", bResult > 0);
         break;

      // [OPEN FILE]
      case TW_OPEN_FILE: 
         bResult = (BOOL)showDialog(TEXT("TUTORIAL_OPEN_FILE"), hParentWnd, dlgprocTutorialDialog, NULL);    
         ERROR_CHECK("Creating FileDialog Tutorial dialog", bResult > 0);
         break;

      // [PROJECTS]
      case TW_PROJECTS: 
         bResult = (BOOL)showDialog(TEXT("TUTORIAL_PROJECTS"), hParentWnd, dlgprocTutorialDialog, NULL);    
         ERROR_CHECK("Creating Projects Tutorial dialog", bResult > 0);
         break;

      // [SCRIPT COMMANDS]
      case TW_SCRIPT_COMMANDS: 
         bResult = (BOOL)showDialog(TEXT("TUTORIAL_SCRIPT_COMMANDS"), hParentWnd, dlgprocTutorialDialog, NULL);    
         ERROR_CHECK("Creating ScriptCommands Tutorial dialog", bResult > 0);
         break;

      // [SCRIPT EDITING]
      case TW_SCRIPT_EDITING: 
         bResult = (BOOL)showDialog(TEXT("TUTORIAL_SCRIPT_EDITING"), hParentWnd, dlgprocTutorialDialog, NULL);    
         ERROR_CHECK("Creating ScriptEditing Part 1 Tutorial dialog", bResult > 0);

         bResult = (BOOL)showDialog(TEXT("TUTORIAL_SCRIPT_EDITING_2"), hParentWnd, dlgprocTutorialDialog, NULL);    
         ERROR_CHECK("Creating ScriptEditing Part 2 Tutorial dialog", bResult > 0);
         break;

      // [SCRIPT OBJECTS]
      case TW_SCRIPT_OBJECTS: 
         bResult = (BOOL)showDialog(TEXT("TUTORIAL_SCRIPT_OBJECTS"), hParentWnd, dlgprocTutorialDialog, NULL);    
         ERROR_CHECK("Creating ScriptObjects Tutorial dialog", bResult > 0);
         break;
      }

      /// Mark tutorial as complete in preferences
      setAppPreferencesTutorialComplete(eDialogID);
   }

   // Return result
   return bResult;
}


/// Function name  : initTutorialDialog
// Description     : 
// 
// HWND  hDialog   : [in] 
// 
// Return Value   : 
// 
BOOL  initTutorialDialog(HWND  hDialog)
{
   // [DIALOG] Set caption and centre dialog
   SetWindowText(hDialog, getAppName());
   utilCentreWindow(getAppWindow(), hDialog);

   // [OWNER DRAW]
   utilAddWindowStyle(GetControl(hDialog, IDC_TUTORIAL_DIALOG_TITLE), SS_OWNERDRAW);
   utilAddWindowStyle(GetControl(hDialog, IDC_TUTORIAL_HEADING_1), SS_OWNERDRAW);

   // [OPTIONAL]
   if (GetControl(hDialog, IDC_TUTORIAL_HEADING_2))
      utilAddWindowStyle(GetControl(hDialog, IDC_TUTORIAL_HEADING_2), SS_OWNERDRAW);
   if (GetControl(hDialog, IDC_TUTORIAL_HEADING_3))
      utilAddWindowStyle(GetControl(hDialog, IDC_TUTORIAL_HEADING_3), SS_OWNERDRAW);

   // Return TRUE
   return TRUE;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : dlgprocTutorialDialog
// Description     : Tutorial Dialog procedure
// 
INT_PTR   dlgprocTutorialDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   BOOL   bResult;   // Operation result

   // Prepare
   bResult = FALSE;

   // Examine message
   switch (iMessage)
   {
   /// [CREATION]
   case WM_INITDIALOG:
      bResult = initTutorialDialog(hDialog);
      break;

   /// [COMMAND PROCESSING]
   case WM_COMMAND:
      switch (LOWORD(wParam))
      {
      case IDOK:
      case IDCANCEL:
         EndDialog(hDialog, TRUE);
         break;
      }
      break;
      
   /// [OWNER DRAW]
   case WM_DRAWITEM:
      switch (wParam)
      {
      case IDC_TUTORIAL_DIALOG_ICON:          bResult = onOwnerDrawStaticIcon(lParam, TEXT("HELP_ICON"), 96);                      break;
      case IDC_TUTORIAL_FILE_OPTIONS_BITMAP:  bResult = onOwnerDrawStaticBitmap(lParam, TEXT("FILE_OPTIONS_TUT_BITMAP"), 96, 256); break;
      case IDC_TUTORIAL_GAME_DATA_BITMAP:     bResult = onOwnerDrawStaticBitmap(lParam, TEXT("GAME_DATA_BITMAP"), 96, 256);        break;
      case IDC_TUTORIAL_GAME_FOLDER_BITMAP:   bResult = onOwnerDrawStaticBitmap(lParam, TEXT("GAME_FOLDER_BITMAP"), 96, 256);      break;
      case IDC_TUTORIAL_GAME_OBJECT_BITMAP:   bResult = onOwnerDrawStaticBitmap(lParam, TEXT("GAME_OBJECTS_BITMAP"), 96, 256);     break;
      case IDC_TUTORIAL_OPEN_FILE_BITMAP:     bResult = onOwnerDrawStaticBitmap(lParam, TEXT("OPEN_FILE_BITMAP"), 96, 256);        break;
      case IDC_TUTORIAL_PROJECT_BITMAP:       bResult = onOwnerDrawStaticBitmap(lParam, TEXT("PROJECTS_BITMAP"), 96, 256);         break;
      case IDC_TUTORIAL_COMMANDS_BITMAP:      bResult = onOwnerDrawStaticBitmap(lParam, TEXT("SCRIPT_COMMANDS_BITMAP"), 96, 256);  break;
      case IDC_TUTORIAL_SCRIPT_OBJECT_BITMAP: bResult = onOwnerDrawStaticBitmap(lParam, TEXT("SCRIPT_OBJECTS_BITMAP"), 96, 256);   break;
      case IDC_TUTORIAL_SCRIPT_EDITOR_BITMAP: bResult = onOwnerDrawStaticBitmap(lParam, TEXT("SCRIPT_EDITOR_BITMAP"), 96, 256);    break;
      case IDC_TUTORIAL_HEADING_1:   
      case IDC_TUTORIAL_HEADING_2: 
      case IDC_TUTORIAL_HEADING_3:            bResult = onOwnerDrawStaticHeading(lParam);  break;
      case IDC_TUTORIAL_DIALOG_TITLE:         bResult = onOwnerDrawStaticTitle(lParam);    break;
      default:                                bResult = FALSE;    break;
      }
      break;

   /// [VISUAL STYLE]
   default:
      bResult = dlgprocVistaStyleDialog(hDialog, iMessage, wParam, lParam);
      break;
   }

   // Return result
   return bResult;
}