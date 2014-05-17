//
// Properties Dialog General (Language).cpp : The general properties page for a language document
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : enablePropertiesDialogControls
// Description     : Enables/Disables all user input controls in a specified properties page
// 
// HWND        hPage   : [in] Properties page
// const BOOL  bEnable : [in] TRUE/FALSE
// 
VOID  enablePropertiesDialogControls(HWND  hPage, const BOOL  bEnable)
{
   CONST TCHAR*  szClasses[] = { WC_EDIT, WC_BUTTON, WC_LISTVIEW, WC_COMBOBOX };
   TCHAR         szClassName[32];

   /// Lookup class for each child window
   for (HWND  hChild = GetFirstChild(hPage); GetClassName(hChild, szClassName, 32); hChild = GetNextSibling(hChild))
   {
      // [CHECK] Ensure class is Button/Combo/Edit/ListView
      for (UINT  i = 0; i < 4; i++)
         if (utilCompareStringVariables(szClassName, szClasses[i]))
            // [COMPATIBLE] Enable/Disable window as required
            EnableWindow(hChild, bEnable);
   }
}


/// Function name  : getLanguageMessage
// Description     : Retrieve current language message from properties dialog active language document
// 
// PROPERTIES_DATA*  pSheetData   : [in] Sheet data
// 
// Return Value   : Language message
// 
LANGUAGE_MESSAGE*  getLanguageMessage(PROPERTIES_DATA*  pSheetData)
{
   ASSERT(pSheetData->pLanguageDocument);
   return pSheetData->pLanguageDocument->pCurrentMessage;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onLanguagePage_Show
// Description     : Displays the values for a language document properties page
// 
// PROPERTIES_DATA*     pSheetData  : [in] Properties dialog data
// HWND                 hPage       : [in] Window handle of the page to display
// CONST PROPERTY_PAGE  ePage       : [in] ID of the page to display
// 
VOID  onLanguagePage_Show(PROPERTIES_DATA*  pSheetData, HWND  hPage, CONST PROPERTY_PAGE  ePage)
{
   LANGUAGE_DOCUMENT*  pDocument = pSheetData->pLanguageDocument;       // Document
   LANGUAGE_MESSAGE*   pMessage  = getLanguageMessage(pSheetData);      // Current message  [May be NULL]
   GAME_STRING*        pString   = pDocument->pCurrentString;           // Current string   [May be NULL]
   APP_LANGUAGE        eLanguage;
   BOOL                bEnabled  = pMessage AND !pDocument->bVirtual;   // TRUE if document is non-virtual and string is currently selected
   TCHAR*              szFolder  = NULL;                                // Document folder path
   
   // Examine page
   switch (ePage)
   {
   /// [LANGUAGE: GENERAL]
   case PP_LANGUAGE_GENERAL:
      // Enable/Disable controls appropriately
      utilEnableDlgItems(hPage, bEnabled, 5, IDC_AUTHOR_EDIT, IDC_TITLE_EDIT, IDC_LANGUAGE_COMBO, IDC_COMPATIBILITY_COMBO, IDC_STRING_VERSION_COMBO);    // , 

      // Calculate path + language
      szFolder  = (pDocument->pLanguageFile ? utilDuplicateFolderPath(pDocument->szFullPath) : generateGameDataFilePath(GFI_LANGUAGE_FOLDER, getAppPreferences()->szGameFolder, getAppPreferences()->eGameVersion));
      eLanguage = convertGameLanguageToAppLanguage(pDocument->pLanguageFile ? pDocument->pLanguageFile->eLanguage : getAppPreferences()->eGameLanguage);

      // Author/Title + DocumentPath
      SetDlgItemText(hPage, IDC_AUTHOR_EDIT,    pMessage ? pMessage->szAuthor : TEXT(""));
      SetDlgItemText(hPage, IDC_TITLE_EDIT,     pMessage ? pMessage->szTitle  : TEXT(""));
      PathSetDlgItemPath(hPage, IDC_LANGUAGE_FOLDER_EDIT, szFolder);

      // Language + Compatibility + Version
      ComboBox_SetCurSel(GetDlgItem(hPage, IDC_COMPATIBILITY_COMBO),  pMessage ? pMessage->eCompatibility : LMC_LOGBOOK);
      ComboBox_SetCurSel(GetDlgItem(hPage, IDC_STRING_VERSION_COMBO), pString  ? pString->eVersion : GV_THREAT);
      ComboBox_SetCurSel(GetDlgItem(hPage, IDC_LANGUAGE_COMBO), eLanguage);
      break;

   /// [ACTION BUTTONS]
   case PP_LANGUAGE_BUTTON:
      // Enable/Disable controls appropriately
      enablePropertiesDialogControls(hPage, bEnabled);

      // Count number of OLE objects
      ListView_SetItemCount(GetDlgItem(hPage, IDC_BUTTONS_LIST), pMessage ? getRichTextDialogButtonCount(pDocument) : 0);
      break;

   /// [COLUMN ADJUSTMENT]
   case PP_LANGUAGE_COLUMNS:
      // Enable/Disable controls appropriately
      enablePropertiesDialogControls(hPage, bEnabled);

      // Select appropriate column number
      CheckDlgButton(hPage, IDC_COLUMN_ONE_RADIO,   pMessage ? pMessage->iColumnCount <= 1 : TRUE);
      CheckDlgButton(hPage, IDC_COLUMN_TWO_RADIO,   pMessage ? pMessage->iColumnCount == 2 : FALSE);
      CheckDlgButton(hPage, IDC_COLUMN_THREE_RADIO, pMessage ? pMessage->iColumnCount >= 3 : FALSE);

      // Position width/spacing sliders
      SendDlgItemMessage(hPage, IDC_COLUMN_WIDTH_SLIDER,   TBM_SETPOS, TRUE, pMessage ? pMessage->iColumnWidth   : 0);
      SendDlgItemMessage(hPage, IDC_COLUMN_SPACING_SLIDER, TBM_SETPOS, TRUE, pMessage ? pMessage->iColumnSpacing : 0);

      // Display formatted width/spacing values
      displayColumnPageSliderText(hPage, IDC_COLUMN_WIDTH_STATIC,   pMessage ? pMessage->iColumnWidth   : 0);
      displayColumnPageSliderText(hPage, IDC_COLUMN_SPACING_STATIC, pMessage ? pMessage->iColumnSpacing : 0);

      // Check width/spacing checkboxes
      CheckDlgButton(hPage, IDC_COLUMN_WIDTH_CHECK,   pMessage ? pMessage->bCustomWidth   : FALSE);
      CheckDlgButton(hPage, IDC_COLUMN_SPACING_CHECK, pMessage ? pMessage->bCustomSpacing : FALSE);

      // Enable/Disable width/spacing controls appropriately
      EnableWindow(GetDlgItem(hPage, IDC_COLUMN_WIDTH_SLIDER),   pMessage ? pMessage->bCustomWidth   : FALSE);
      EnableWindow(GetDlgItem(hPage, IDC_COLUMN_WIDTH_STATIC),   pMessage ? pMessage->bCustomWidth   : FALSE);
      EnableWindow(GetDlgItem(hPage, IDC_COLUMN_SPACING_SLIDER), pMessage ? pMessage->bCustomSpacing : FALSE);
      EnableWindow(GetDlgItem(hPage, IDC_COLUMN_SPACING_STATIC), pMessage ? pMessage->bCustomSpacing : FALSE);
      break;

   /// [LEGACY STUFF]
   case PP_LANGUAGE_SPECIAL:
      // Enable/Disable controls appropriately
      enablePropertiesDialogControls(hPage, bEnabled);

      // Check CustomRank/Article
      CheckDlgButton(hPage, IDC_ARTICLE_CHECK,   pMessage ? pMessage->bArticle    : FALSE);
      CheckDlgButton(hPage, IDC_RANK_CHECK,      pMessage ? pMessage->bCustomRank : FALSE);
      
      // CustomRank Title/Type
      SetDlgItemText(hPage, IDC_RANK_TITLE_EDIT, pMessage ? pMessage->szRankTitle : TEXT(""));
      ComboBox_SetCurSel(GetDlgItem(hPage, IDC_RANK_TYPE_COMBO), pMessage ? pMessage->eRankType : RT_FIGHT);

      // Enable/Disable Rank controls
      EnableWindow(GetDlgItem(hPage, IDC_RANK_TYPE_COMBO), pMessage ? pMessage->bCustomRank : FALSE);
      EnableWindow(GetDlgItem(hPage, IDC_RANK_TITLE_EDIT), pMessage ? pMessage->bCustomRank : FALSE);
      break;
   }

   // Cleanup
   utilSafeDeleteString(szFolder);
}

/// Function name  : onGeneralPage_CommandL
// Description     : WM_COMMAND processing for the 'General' properties page
// 
// LANGUAGE_DOCUMENT* pDocument      : [in] Language Document data
// HWND               hDialog        : [in] 'General' properties dialog page window handle
// CONST UINT         iControl       : [in] ID of the control sending the command
// CONST UINT         iNotification  : [in] Notification being sent
// HWND               hCtrl          : [in] Window handle fo the control sending the command
// 
// Return Value   : TRUE if processed, FALSE otherwise
// 
BOOL    onGeneralPage_CommandL(PROPERTIES_DATA*  pSheetData, HWND  hDialog, CONST UINT  iControl, CONST UINT  iNotification, HWND  hCtrl)
{
   LANGUAGE_DOCUMENT*  pDocument = pSheetData->pLanguageDocument;       // Document
   LANGUAGE_MESSAGE*   pMessage  = getLanguageMessage(pSheetData);      // Current message  [May be NULL]
   BOOL                bResult   = FALSE;

   // [CHECK] Ensure change is caused by user
   if (pSheetData->bRefreshing)
      return FALSE;

   // Examine control
   switch (iControl)
   {
   case IDC_AUTHOR_EDIT:
   case IDC_TITLE_EDIT:
   case IDC_COMPATIBILITY_COMBO:
   case IDC_STRING_VERSION_COMBO:
      switch (iControl)
      {
      /// [AUTHOR/TITLE] Update appropriate property
      case IDC_AUTHOR_EDIT:
      case IDC_TITLE_EDIT:
         if (bResult = (iNotification == EN_CHANGE))
         {
            // Identify and replace property string
            TCHAR*&  szProperty = (iControl == IDC_AUTHOR_EDIT ? pMessage->szAuthor : pMessage->szTitle);
            utilReplaceWindowTextString(szProperty, hCtrl);
         }
         break;

      /*/// [COMPATIBILITY] Strip the message of customisations incompatible with the current selection
      case IDC_COMPATIBILITY_COMBO:
         if (bResult = (iNotification == CBN_SELCHANGE))
            performLanguageMessageCompatibilityChange(pMessage, (COMPATIBILITY)ComboBox_GetCurSel(hCtrl) );
         break;*/

      /// [COMPATIBILITY] Change the colour usage flag
      case IDC_COMPATIBILITY_COMBO:
         if (bResult = (iNotification == CBN_SELCHANGE))
            pMessage->eCompatibility = (COMPATIBILITY)ComboBox_GetCurSel(hCtrl);
         break;

      /// [GAME VERSION] Update GameVersion
      case IDC_STRING_VERSION_COMBO:
         if (bResult = (iNotification == CBN_SELCHANGE))
            pDocument->pCurrentString->eVersion = (GAME_VERSION)ComboBox_GetCurSel(hCtrl);
         break;

      /// [LANGUAGE] Update Language
      case IDC_LANGUAGE_COMBO:
         if (bResult = (iNotification == CBN_SELCHANGE AND pDocument->pLanguageFile))
            pDocument->pLanguageFile->eLanguage = convertAppLanguageToGameLanguage((APP_LANGUAGE)ComboBox_GetCurSel(hCtrl));
         break;
      }

      // [EVENT] Notify document that a property has changed
      if (bResult)
         sendDocumentPropertyUpdated(AW_DOCUMENTS_CTRL, iControl);
   }

   return bResult;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : dlgprocGeneralPageL
// Description     : Window procedure for the 'General' script property page
//
// 
INT_PTR   dlgprocGeneralPageL(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   PROPERTIES_DATA*  pDialogData;

   // Get dialog data
   pDialogData = getPropertiesDialogData(hDialog);

   // Intercept messages
   switch (iMessage)
   {
   /// [COMMAND]
   case WM_COMMAND:
      onGeneralPage_CommandL(pDialogData, hDialog, LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
      break;

   /// [CUSTOM COMBOBOX] Small Combo with Medium drop-down
   case WM_MEASUREITEM:
      return onWindow_MeasureComboBox((MEASUREITEMSTRUCT*)lParam, ITS_SMALL, ITS_MEDIUM);
   }

   // Pass to common handlers
   return dlgprocPropertiesPage(hDialog, iMessage, wParam, lParam, PP_LANGUAGE_GENERAL);
}

