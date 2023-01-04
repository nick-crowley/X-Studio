//
// First Run Dialog.cpp : Allows the user to select their language the first time the app is run
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                  CREATION  /  DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : displayFirstRunDialog
// Description     : Display the 'FirstRun' dialog to query the user's desired languages
// 
// HWND  hParentWnd   : [in] Parent window handle
// 
// Return Value   : Interface language in the LO word and game data language in the HO word
// 
APP_LANGUAGE   displayFirstRunDialog(HWND  hParentWnd)
{
   // Display 'FirstRun' dialog to query display language
   return (APP_LANGUAGE)showDialog(TEXT("FIRST_TIME_DIALOG"), hParentWnd, dlgprocFirstRunDialog, NULL);
}

// onException: Display 
#define  ON_EXCEPTION()    displayException(pException);

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : initFirstRunDialog
// Description     : POpulate the languages ComboBox
// 
// HWND   hDialog    : [in] FirstRun dialog window handle
// 
// Return Value   : TRUE
// 
BOOL  initFirstRunDialog(HWND  hDialog)
{
   // Set window caption
   SetWindowText(hDialog, getAppName());

   /// Populate interface languages ComboBox
   appendCustomComboBoxItemEx(GetControl(hDialog, IDC_FIRST_LANGUAGE_COMBO), loadTempString(IDS_LANGUAGE_ENGLISH + AL_ENGLISH), NULL, szLanguageIcons[AL_ENGLISH], NULL);
   appendCustomComboBoxItemEx(GetControl(hDialog, IDC_FIRST_LANGUAGE_COMBO), loadTempString(IDS_LANGUAGE_ENGLISH + AL_GERMAN), NULL, szLanguageIcons[AL_GERMAN], NULL);

   // [OWNER DRAW]
   utilAddWindowStyle(GetControl(hDialog, IDC_FIRST_WELCOME_STATIC), SS_OWNERDRAW);
   utilAddWindowStyle(GetControl(hDialog, IDC_FIRST_LANGUAGE_STATIC), SS_OWNERDRAW);

   /// Initially display 'English' and focus the OK button
   ComboBox_SetCurSel(GetControl(hDialog, IDC_FIRST_LANGUAGE_COMBO), IL_ENGLISH);
   SetFocus(GetDlgItem(hDialog, IDOK));

   // Return TRUE
   return TRUE;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onFirstRunDialog_Close
// Description     : Encode the selected languages as the dialog return value
// 
// HWND  hDialog   : [in] FirstRun dialog window handle
// 
// Return Value   : TRUE
// 
BOOL  onFirstRunDialog_Close(HWND  hDialog)
{
   INTERFACE_LANGUAGE  eInterfaceLanguage;   // Language selection

   // Retrieve the currently selected language
   eInterfaceLanguage = (INTERFACE_LANGUAGE)ComboBox_GetCurSel(GetControl(hDialog, IDC_FIRST_LANGUAGE_COMBO));

   // Convert into an ApplicationLanguage
   EndDialog(hDialog, (INT_PTR)convertInterfaceLanguageToAppLanguage(eInterfaceLanguage));

   // Return TRUE
   return TRUE;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                  WINDOW PROCEDURES
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : dlgprocFirstRunDialog
// Description     : FirstRun dialog window procedure
// 
INT_PTR  dlgprocFirstRunDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   BOOL   bResult = TRUE;     // Operation result

   // Prepare
   TRY
   {
      // Examine message
      switch (iMessage)
      {
      /// [CREATION]
      case WM_INITDIALOG:
         bResult = initFirstRunDialog(hDialog);
         break;

      /// [COMMAND]
      case WM_COMMAND:
         switch (LOWORD(wParam))
         {
         case IDOK:        // OK Button
         case IDCANCEL:    // Dialog Aborted
            bResult = onFirstRunDialog_Close(hDialog);
            break;
         }
         break;

      /// [OWNER DRAW]
      case WM_DRAWITEM:
         switch (wParam)
         {
         case IDC_FIRST_WELCOME_STATIC:   bResult = onOwnerDrawStaticTitle(lParam);      break;
         case IDC_FIRST_LANGUAGE_STATIC:  bResult = onOwnerDrawStaticHeading(lParam);    break;
         case IDC_FIRST_DIALOG_ICON:      bResult = onOwnerDrawStaticIcon(lParam, TEXT("MAIN_WINDOW"), 96);    break;
         default:                         bResult = FALSE;
         }
         break;

      // [UNHANDLED]
      default:
         bResult = FALSE;
         break;
      }

      /// [VISTA STYLE] Pass to VistaDialog base or return result
      return bResult ? bResult : (INT_PTR)dlgprocVistaStyleDialog(hDialog, iMessage, wParam, lParam);
   }
   /// [EXCEPTION HANDLER]
   CATCH3("iMessage=%s  wParam=%d  lParam=%d", identifyMessage(iMessage), wParam, lParam);
   return FALSE;
}



