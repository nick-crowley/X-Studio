//
// Properties Dialog Special.cpp : Legacy properties page for a language document
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



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////


/// Function name  : onSpecialPageCommand
// Description     : WM_COMMAND processing for the 'Special' properties page
// 
// LANGUAGE_DOCUMENT* pDocument      : [in] Language Document data
// HWND               hDialog        : [in] 'Special' properties dialog page window handle
// CONST UINT         iControl       : [in] ID of the control sending the command
// CONST UINT         iNotification  : [in] Notification being sent
// HWND               hCtrl          : [in] Window handle fo the control sending the command
// 
// Return Value   : TRUE if processed, FALSE otherwise
// 
BOOL    onSpecialPageCommand(LANGUAGE_DOCUMENT*  pDocument, HWND  hDialog, CONST UINT  iControl, CONST UINT  iNotification, HWND  hCtrl)
{
   LANGUAGE_MESSAGE*  &pCurrentMessage = pDocument->pCurrentMessage;

   switch (iControl)
   {
   /// [ARTICLE CHECK] - Save new article state
   case IDC_ARTICLE_CHECK:
      pCurrentMessage->bArticle = IsDlgButtonChecked(hDialog, IDC_ARTICLE_CHECK);
      break;

   /// [CUSTOM RANK CHECK] - Save new state and Enable/Disable custom rank controls
   case IDC_RANK_CHECK:
      pCurrentMessage->bCustomRank = IsDlgButtonChecked(hDialog, IDC_RANK_CHECK);
      EnableWindow(GetDlgItem(hDialog,IDC_RANK_TITLE_EDIT), IsDlgButtonChecked(hDialog, IDC_RANK_CHECK));
      EnableWindow(GetDlgItem(hDialog,IDC_RANK_TYPE_COMBO), IsDlgButtonChecked(hDialog, IDC_RANK_CHECK));
      break;

   /// [RANK TYPE]
   case IDC_RANK_TYPE_COMBO:
      if (iNotification == CBN_SELCHANGE)
      {
         pCurrentMessage->eRankType = (RANK_TYPE)SendDlgItemMessage(hDialog, IDC_RANK_TYPE_COMBO, CB_GETCURSEL, NULL, NULL);
         break;
      }
      return FALSE;

   /// [RANK TITLE] - Store new title
   case IDC_RANK_TITLE_EDIT:
      if (iNotification == EN_CHANGE)
      {
         // Delete existing title and replace with window text
         utilReplaceWindowTextString(pCurrentMessage->szRankTitle, hCtrl);
         break;
      }
      return FALSE;

   default:
      return FALSE;
   }

   return TRUE;
}

/// Function name  : onSpecialPageCtlColour
// Description     : Applies a disabled background colour to the edit control, which uses a slightly ambiguous
//                      visual styles white colour for the disabled state
// 
// LANGUAGE_DOCUMENT*  pDocument : [in] LanguageDocument data
// HWND                hDialog   : [in] 'Special' properties page window handle
// HWND                hCtrl     : [in] Window handle of the edit control or a static control
// 
// Return Value   : Brush to use to draw the control's background
// 
//HBRUSH  onSpecialPageCtlColour(LANGUAGE_DOCUMENT*  pDocument, HWND  hDialog, HWND  hCtrl)
//{
//   // [RANK TITLE EDIT] -- Return white or grey brush
//   if (GetDlgCtrlID(hCtrl) == IDC_RANK_TITLE_EDIT)
//   {
//      if (pDocument->pCurrentMessage->bCustomRank)
//         return GetSysColorBrush(COLOR_WINDOW);
//      else
//         return (HBRUSH)GetStockObject(LTGRAY_BRUSH);
//   }
//   // [RANK TYPE AND TITLE STATICS] -- Always return grey brush
//   else
//      return GetSysColorBrush(COLOR_WINDOW);
//}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////


/// Function name  : dlgprocSpecialPage
// Description     : Window procedure for the 'General' script property page
//
// 
INT_PTR   dlgprocSpecialPage(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   PROPERTIES_DATA*  pDialogData;

   // Get dialog data
   pDialogData = getPropertiesDialogData(hDialog);

   // Intercept messages
   switch (iMessage)
   {
   case WM_COMMAND:
      if (onSpecialPageCommand((LANGUAGE_DOCUMENT*)pDialogData->pDocument, hDialog, LOWORD(wParam), HIWORD(wParam), (HWND)lParam))
         return TRUE;
      break;
   }

   // Pass to common handlers
   return dlgprocPropertiesPage(hDialog, iMessage, wParam, lParam, PP_LANGUAGE_SPECIAL);
}
