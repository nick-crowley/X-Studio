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


/// Function name  : onSpecialPage_Command
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
BOOL    onSpecialPage_Command(PROPERTIES_DATA*  pSheetData, HWND  hDialog, CONST UINT  iControl, CONST UINT  iNotification, HWND  hCtrl)
{
   LANGUAGE_MESSAGE*  pCurrentMessage = getLanguageMessage(pSheetData);
   BOOL               bResult = TRUE;

   // [CHECK] Ensure update is due to user action
   if (pSheetData->bRefreshing)
      return FALSE;

   // Examine control
   switch (iControl)
   {
   case IDC_ARTICLE_CHECK:
   case IDC_RANK_CHECK:
   case IDC_RANK_TYPE_COMBO:
   case IDC_RANK_TITLE_EDIT:
      switch (iControl)
      {
      /// [ARTICLE CHECK] - Save new article state
      case IDC_ARTICLE_CHECK:
         pCurrentMessage->bArticle = IsDlgButtonChecked(hDialog, IDC_ARTICLE_CHECK);
         break;

      /// [CUSTOM RANK CHECK] - Save new state and Enable/Disable custom rank controls
      case IDC_RANK_CHECK:
         utilEnableDlgItem(hDialog, IDC_RANK_TITLE_EDIT, pCurrentMessage->bCustomRank = IsDlgButtonChecked(hDialog, IDC_RANK_CHECK));
         utilEnableDlgItem(hDialog, IDC_RANK_TYPE_COMBO, pCurrentMessage->bCustomRank);
         break;

      /// [RANK TYPE] Store new rank
      case IDC_RANK_TYPE_COMBO:
         if (bResult = iNotification == CBN_SELCHANGE)
            pCurrentMessage->eRankType = (RANK_TYPE)ComboBox_GetCurSel(GetDlgItem(hDialog, IDC_RANK_TYPE_COMBO));
         break;

      /// [RANK TITLE] Store new title
      case IDC_RANK_TITLE_EDIT:
         if (bResult = iNotification == EN_CHANGE)
            utilReplaceWindowTextString(pCurrentMessage->szRankTitle, hCtrl);
         break;
      }

      return bResult;
   }

   // [UNHANDLED]
   return FALSE;
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
      if (onSpecialPage_Command(pDialogData, hDialog, LOWORD(wParam), HIWORD(wParam), (HWND)lParam))
         return TRUE;
      break;
   }

   // Pass to common handlers
   return dlgprocPropertiesPage(hDialog, iMessage, wParam, lParam, PP_LANGUAGE_SPECIAL);
}
