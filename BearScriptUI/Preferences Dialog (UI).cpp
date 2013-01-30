//
// Preferences Property Sheet UI.cpp : Implementation of logic unique to individual pages
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C WITH /C WITH ++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                     CONSTANTS / GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// Number of Font sizes
#define       FONT_SIZES             7

// Font size options available
CONST UINT     iFontSizes[FONT_SIZES] = { 8, 10, 11, 12, 14, 16, 18 };     

// Number of display CodeObjects  (All CodeObjects excluding the 'Whitespace' and 'Null' classes)
#define       DISPLAY_CODEOBJECTS    (CODEOBJECT_CLASSES - 2)

// Icon resources  (Indicies matching INTERFACE_COLOUR)
CONST TCHAR*   szColourIcons[13] = { TEXT("BLACK_ICON"), TEXT("BLUE_ICON"), TEXT("DARK_GREEN_ICON"), TEXT("DARK_GREY_ICON"), TEXT("DARK_RED_ICON"), TEXT("CYAN_ICON"), TEXT("GREEN_ICON"),
                                     TEXT("GREY_ICON"),  TEXT("RED_ICON"),  TEXT("ORANGE_ICON"),     TEXT("PURPLE_ICON"),    TEXT("YELLOW_ICON"),   TEXT("WHITE_ICON") }; 

// Maps PageID -> String
extern CONST TCHAR*  szDebugPageNames[PREFERENCES_PAGES];

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : callbackPreferencesFontEnumeration
// Description     : Add each font on the user's computer to the 'appearances' property page 'fonts' combo box
// 
// ENUMLOGFONTEX*    pFontData : [in] Font properties
// NEWTEXTMETRICEX*  pFontSize : [in] Font display size
// DWORD             dwType    : [in] Font type
// LPARAM            lParam    : [in] Window handle of the appearances property page
// 
// Return type : TRUE for more fonts
//
INT CALLBACK callbackPreferencesFontEnumeration(CONST ENUMLOGFONTEX*  pFontData, CONST NEWTEXTMETRICEX*  pFontSize, DWORD  dwType, LPARAM  lParam)
{
   CONST TCHAR*  szFont = pFontData->elfLogFont.lfFaceName;
   HWND          hPage  = (HWND)lParam;
   HDC           hDC;
   
   // [CHECK] Exclude weird font names
   if (szFont[0] != '@')
   {
      // Add font name to combo box using same font for display
      HFONT hFont = utilCreateFont(hDC = GetDC(hPage), pFontData->elfLogFont.lfFaceName, 10, FALSE, FALSE, FALSE);
      appendCustomComboBoxFontItem(GetControl(hPage, IDC_FONT_NAME_COMBO), szFont, hFont);
      ReleaseDC(hPage, hDC);
   }

   return TRUE;
}

/// Function name  : populateAppearancePageFontSizesCombo
// Description     : Populates or re-populates the font sizes combo using the currently selected font name
// 
// PREFERENCES_DATA*  pDialogData : [in] Dialog data
// HWND               hDialog     : [in] Page 
// 
VOID   populateAppearancePageFontSizesCombo(PREFERENCES_DATA*  pDialogData, HWND  hDialog)
{
   CUSTOM_COMBO_ITEM*  pItem;          // Item data for currently selected font
   HWND                hNameCombo,     // FontName combo
                       hSizeCombo;     // FontSize combo
   TCHAR              *szItemText,     // Item text for current font size
                      *szFontName;     // Name of the currently selected font
   HFONT               hItemFont,      // Font created for each item
                       hOldFont;       //
   SIZE                siTextSize;     // Metrics of font for new item
   HDC                 hDC;            // DC

   // Prepare
   hNameCombo = GetControl(hDialog, IDC_FONT_NAME_COMBO);
   hSizeCombo = GetControl(hDialog, IDC_FONT_SIZE_COMBO);

   // [CHECK] Lookup currently selected font name
   if (pItem = getCustomComboBoxItem(hNameCombo, ComboBox_GetCurSel(hNameCombo)))
   {
      // Prepare
      szItemText = utilCreateString(8);
      szFontName = pItem->szMainText;
      hDC        = GetDC(hSizeCombo);

      // Erase existing entries (if any)
      ComboBox_ResetContent(hSizeCombo);

      /// Populate ComboBox
      for (UINT iIndex = 0, iItemIndex; iIndex < FONT_SIZES; iIndex++)
      {
         // Generate text and font
         StringCchPrintf(szItemText, 8, TEXT("%d"), iFontSizes[iIndex]);
         hItemFont = utilCreateFontEx(hSizeCombo, szFontName, iFontSizes[iIndex], FALSE, FALSE, FALSE);

         /// Calculate font height
         hOldFont = SelectFont(hDC, hItemFont);
         GetTextExtentPoint32(hDC, TEXT("ABCD"), 4, &siTextSize);
         SelectFont(hDC, hOldFont);

         // Insert item and set height
         iItemIndex = appendCustomComboBoxFontItem(hSizeCombo, szItemText, hItemFont);
         ComboBox_SetItemHeight(hSizeCombo, iItemIndex, siTextSize.cy);
      }

      /// Select current Font size
      StringCchPrintf(szItemText, 8, TEXT("%d"), pDialogData->pPreferencesCopy->oColourScheme.iFontSize);
      ComboBox_SelectString(hSizeCombo, -1, szItemText);

      // Cleanup
      ReleaseDC(hSizeCombo, hDC);
      utilDeleteString(szItemText);
   }
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : displayPreferencesDialog
// Description     : Create the modeless 'Script Properties' property sheet
// 
// HWND                    hParentWnd  : [in] Parent window for the property sheet
// CONST PREFERENCES_PAGE  ePage       : [in] Page ID of the initial page to display
// 
// Return Type: PSR_NO_CHANGE           - The preferences altered do not require an application restart
//              PSR_LANGUAGE_CHANGED    - Language settings were changed
//              PSR_GAME_FOLDER_CHANGED - Game data folder was changed
//
BOOL  displayPreferencesDialog(HWND  hParentWnd, CONST PREFERENCES_PAGE  ePage)
{
   PREFERENCES_DATA*  pDialogData;              // Dialog data for the preferences PropertySheet
   ERROR_STACK*       pException;
   BOOL               iDialogResult;            // Operation result indicating which (if any) important preferences have changed

   // Prepare
   iDialogResult = PSR_NO_CHANGE;

   __try
   {
      // Create dialog data
      pDialogData = createPreferencesData(getAppPreferences());

      // Set parent window and initial page
      pDialogData->oHeader.hwndParent = hParentWnd;
      pDialogData->oHeader.nStartPage = ePage;
      
      /// Display Preferences propertysheet
      if (PropertySheet(&pDialogData->oHeader) > 0)
      {
         // Re-calculate program state and determine what data has changed (if any)
         iDialogResult = onPreferencesDialog_Close(hParentWnd, pDialogData->pPreferencesCopy);

         // Save changes
         updateAppPreferences(pDialogData->pPreferencesCopy);

         // [CHECK] Has the user changed the interface language?
         if (iDialogResult INCLUDES PSR_APP_LANGUAGE_CHANGED)
            /// [LANGUAGE CHANGED] "Your new choice of interface language will not take effect until X-Studio is restarted"
            displayMessageDialogf(NULL, IDS_GENERAL_LANGUAGE_RESTART, TEXT("Interface Language Changed"), MDF_OK WITH MDF_WARNING);

         // [CHECK] Has game data folder/language been changed?
         if (iDialogResult INCLUDES (PSR_GAME_FOLDER_CHANGED WITH PSR_GAME_LANGUAGE_CHANGED))
            /// [FOLDER/LANGUAGE CHANGE] Close all documents and reload the game data
            performMainWindowReInitialisation(getMainWindowData(), NULL);
         else
            /// [EVENT] Fire UN_PREFERENCES_CHANGED
            Preferences_Changed(hParentWnd);    // Refreshes all documents
      }
   }
   /// [EXCEPTION HANDLER]
   __except (generateExceptionError(GetExceptionInformation(), pException))
   {
      // [ERROR] "An unidentified and unexpected critical error has occurred in the preferences window - settings have not been saved."
      enhanceError(pException, ERROR_ID(IDS_EXCEPTION_DISPLAY_PREFERENCES));
      displayException(pException);
      return FALSE;
   }
      
   // Cleanup
   deletePreferencesData(pDialogData);
   return iDialogResult;
}


/// Function name  : initPreferencesPage
// Description     : Initialise controls on a specified 'Preferences' property-sheet page
// 
// PREFERENCES_DATA*       pDialogData : [in] Preferences dialog data
// HWND                    hDialog     : [in] Window handle of a page in the preferences property sheet
// CONST PREFERENCES_PAGE  ePage       : [in] ID of a page in the preferences property sheet
// 
// Returns : TRUE
//
BOOL  initPreferencesPage(PREFERENCES_DATA*  pDialogData, HWND  hDialog, CONST PREFERENCES_PAGE  ePage)
{
   BOOL   bResult;

   TRACK_FUNCTION();

   // Examine page
   switch (ePage)
   {
   /// [GENERAL] -- Add language entries
   case PP_GENERAL:    bResult = initPreferencesGeneralPage(pDialogData, hDialog);     break;
   /// [FOLDERS] -- Prevent the user from changing the game folder if documents are open
   case PP_FOLDERS:    bResult = initPreferencesFolderPage(pDialogData, hDialog);      break;
   /// [APPEARANCE] -- Enumerate fonts and colour schemes
   case PP_APPEARANCE: bResult = initPreferencesAppearancePage(pDialogData, hDialog);  break;
   /// [SYNTAX] -- Add code object names and colour entries
   case PP_SYNTAX:     bResult = initPreferencesSyntaxPage(pDialogData, hDialog);      break;
   /// [MISC] -- Add error handling entries
   case PP_MISC:       bResult = initPreferencesMiscPage(pDialogData, hDialog);        break;
   }

   // Return result
   END_TRACKING();
   return bResult;
}


/// Function name  : initAppearancePage
// Description     : Populate the font size and colour scheme ComboBoxes
// 
// PREFERENCES_DATA*  pDialogData : [in] Preferences dialog data
// HWND               hDialog     : [in] 'Appearance' page window handle
// 
// Return Value   : TRUE
// 
BOOL   initPreferencesAppearancePage(PREFERENCES_DATA*  pDialogData, HWND  hDialog)
{
   const TCHAR*  szSchemeIcons[] = { TEXT("MAIN_WINDOW"), TEXT("EXSCRIPTOR_ICON"), TEXT("THREAT_ICON"), TEXT("VISUAL_STUDIO_ICON") };
   LIST_ITEM*    pSchemeListItem;   // Colour scheme list iterator
   LOGFONT       oFontData;         // Specifies the data for enumerating fonts
   HDC           hDC;               // Dialog device context

   // Prepare
   TRACK_FUNCTION();
   utilZeroObject(&oFontData, LOGFONT);

   /// Enumerate Colour schemes in the dialog data
   for (UINT iScheme = 0; findListItemByIndex(pDialogData->pColourSchemeList, iScheme, pSchemeListItem); iScheme++)
      appendCustomComboBoxItemEx(GetDlgItem(hDialog, IDC_COLOUR_SCHEME_COMBO), extractListItemPointer(pSchemeListItem, COLOUR_SCHEME)->szName, NULL, szSchemeIcons[iScheme], NULL);

   // Select first colour scheme
   ComboBox_SetCurSel(GetDlgItem(hDialog, IDC_COLOUR_SCHEME_COMBO), 0);
   
   /// Enumerate Fonts available on the system and add to the Fonts ComboBox
   EnumFontFamiliesEx(hDC = GetDC(hDialog), &oFontData, (FONTENUMPROC)callbackPreferencesFontEnumeration, (LPARAM)hDialog, NULL);
   ReleaseDC(hDialog, hDC);

   // Return TRUE
   END_TRACKING();
   return TRUE;
}


/// Function name  : initPreferencesFolderPage
// Description     : Prevent the user from changing the game folder while documents are open
// 
// PREFERENCES_DATA*  pDialogData : [in] Preferences dialog data
// HWND               hDialog     : [in] 'Appearance' page window handle
// 
// Return Value   : TRUE
// 
BOOL   initPreferencesFolderPage(PREFERENCES_DATA*  pDialogData, HWND  hDialog)
{
   CONST TCHAR  *szRegistryFolders[4] = { TEXT("Software\\Egosoft\\X2"), TEXT("Software\\Egosoft\\X2"), TEXT("Software\\Egosoft\\X3R"), TEXT("Software\\Egosoft\\X3AP") };
   BOOL          bModifiedDocuments;   // Whether there are any modified documents open
   TCHAR        *szGameFolder,         // Game folder paths extracted from the registry
                *szLanguage;           // Language string of the language being processed

   // Prepare
   bModifiedDocuments = (getAppWindow() ? isAnyDocumentModified(getMainWindowData()->hDocumentsTab) : FALSE);     // App window will not exist on first run
   szGameFolder       = utilCreateEmptyPath();

   /// [GAME LANGUAGE] Iterate through application languages
   for (APP_LANGUAGE eLanguage = AL_ENGLISH; eLanguage <= AL_SPANISH; eLanguage = (APP_LANGUAGE)(eLanguage + 1))
   {
      // Add language name and icon to comboBox
      szLanguage = utilLoadString(getResourceInstance(), IDS_LANGUAGE_ENGLISH + eLanguage, 64);
      appendCustomComboBoxItemEx(GetControl(hDialog, IDC_GAME_LANGUAGE_COMBO), szLanguage, NULL, szLanguageIcons[eLanguage], NULL);
      // Cleanup
      utilDeleteString(szLanguage);
   }
   
   /// [GAME FOLDER] Populate the GameFolders combo
   for (GAME_VERSION  eVersion = GV_THREAT; eVersion <= GV_ALBION_PRELUDE; eVersion = (GAME_VERSION)(eVersion + 1))
   {
      // [CHECK] Does registry value exist?
      if (SHRegGetPath(HKEY_CURRENT_USER, szRegistryFolders[eVersion], TEXT("INSTALL_DIR"), szGameFolder, NULL) == ERROR_SUCCESS)
         // [SUCCESS] Add game folder and associated icon
         appendCustomComboBoxItemEx(GetControl(hDialog, IDC_GAME_FOLDER_COMBO), szGameFolder, NULL, identifyGameVersionIconID(eVersion), NULL);
   }

   // [CHECK] Add fixed string if none were found
   if (!ComboBox_GetCount(GetControl(hDialog, IDC_GAME_FOLDER_COMBO)))
      appendCustomComboBoxItem(GetControl(hDialog, IDC_GAME_FOLDER_COMBO), TEXT("(No game installations found)"));
   
   /// [FOLDER VALIDATION] Display the current validation status
   updatePreferencesFoldersPageState(pDialogData->pPreferencesCopy, hDialog, IDC_GAME_FOLDER_STATUS); 

   /// [OPEN DOCUMENTS] Disable GameFolder / GameLanguage
   utilEnableDlgItem(hDialog, IDC_GAME_FOLDER_COMBO, !bModifiedDocuments);
   utilEnableDlgItem(hDialog, IDC_GAME_FOLDER_BROWSE, !bModifiedDocuments);
   utilEnableDlgItem(hDialog, IDC_GAME_LANGUAGE_COMBO, !bModifiedDocuments);

   // Cleanup and return TRUE
   utilDeleteString(szGameFolder);
   return TRUE;
}


/// Function name  : initPreferencesGeneralPage
// Description     : Initialise the controls in the 'General' preferences page
// 
// PREFERENCES_DATA*  pDialogData : [in] Preferences dialog data
// HWND               hDialog     : [in] 'General' page window handle
// 
// Return Value   : TRUE
// 
BOOL  initPreferencesGeneralPage(PREFERENCES_DATA*  pDialogData, HWND  hDialog)
{
   TCHAR*    szLanguage;           // Language string of the language being processed

   /// [APP LANGAUGE] Iterate through application languages
   for (APP_LANGUAGE eLanguage = AL_ENGLISH; eLanguage <= AL_SPANISH; eLanguage = (APP_LANGUAGE)(eLanguage + 1))
   {
      switch (eLanguage)
      {
      // [ENGLISH/GERMAN] 
      case AL_ENGLISH:
      case AL_GERMAN:
      //case AL_RUSSIAN:  
         // Add language name and icon to comboBox
         szLanguage = utilLoadString(getResourceInstance(), IDS_LANGUAGE_ENGLISH + eLanguage, 64);
         appendCustomComboBoxItemEx(GetControl(hDialog, IDC_APPLICATION_LANGUAGE_COMBO), szLanguage, NULL, szLanguageIcons[eLanguage], NULL);
         // Cleanup
         utilDeleteString(szLanguage);
         break;
      }
   }

   /// [DELAY] Setup sliders
   SendDlgItemMessage(hDialog, IDC_CODE_TOOLTIPS_SLIDER, TBM_SETRANGE, FALSE, MAKE_LONG(1, 12));
   SendDlgItemMessage(hDialog, IDC_CODE_TOOLTIPS_SLIDER, TBM_SETTICFREQ, 1, NULL);
   
   SendDlgItemMessage(hDialog, IDC_SEARCH_TOOLTIPS_SLIDER, TBM_SETRANGE, FALSE, MAKE_LONG(1, 12));
   SendDlgItemMessage(hDialog, IDC_SEARCH_TOOLTIPS_SLIDER, TBM_SETTICFREQ, 1, NULL);

   // Return TRUE
   return TRUE;
}

/// Function name  : initPreferencesMiscPage
// Description     : Initialise the controls on the preferences 'Misc' page
// 
// PREFERENCES_DATA*  pDialogData  : [in] Preferences dialog data
// HWND               hDialog      : [in] Misc page window handle
// 
// Return Value   : TRUE
// 
BOOL  initPreferencesMiscPage(PREFERENCES_DATA*  pDialogData, HWND  hDialog)
{
   TCHAR*   szErrorHandling;   // Used for loading string resources containing Error handling options

   // Prepare
   szErrorHandling = utilCreateEmptyString(64);

   /// Add error handling entries
   for (UINT iHandling = 0; iHandling < 3; iHandling++)
   {
      // Load handling string and add to combo
      LoadString(getResourceInstance(), IDS_ERROR_HANDLING_ABORT + iHandling, szErrorHandling, 64);
      SendDlgItemMessage(hDialog, IDC_ERROR_HANDLING_COMBO, CB_ADDSTRING, NULL, (LPARAM)szErrorHandling);
   }

   /// Limit forum name to 32 characters
   SendDlgItemMessage(hDialog, IDC_FORUM_USERNAME_EDIT, EM_LIMITTEXT, 32, NULL);

   // Cleanup and return
   utilDeleteString(szErrorHandling);
   return TRUE;
}


/// Function name  : initPreferencesSyntaxPage
// Description     : Initialise the controls on the preferences syntax page
// 
// PREFERENCES_DATA*  pDialogData  : [in] Preferences dialog data
// HWND               hDialog      : [in] Syntax page window handle
// 
// Return Value   : TRUE
// 
BOOL  initPreferencesSyntaxPage(PREFERENCES_DATA*  pDialogData, HWND  hDialog)
{
   LISTVIEW_COLUMNS  oComponentsListView = { 1, IDS_SYNTAX_COLUMN_COMPONENT, 120, NULL, NULL, NULL, NULL };      // CodeObject ListView data
   COLOUR_SCHEME    *pColourScheme;  // Current colour scheme (convenience pointer)
   TCHAR            *szItemText,     // ComboBox/ListBox item text
                    *szPreviewText;  // CodeEdit preview text
   
   // Prepare
   pColourScheme = &pDialogData->pPreferencesCopy->oColourScheme;
   szPreviewText = utilLoadString(getResourceInstance(), IDS_CODE_EDIT_PREVIEW_TEXT, 512);
   szItemText    = utilCreateString(64);

   /// [CODE-EDIT] Create ScriptFile
   pDialogData->pScriptFile = createScriptFileFromText(szPreviewText, TEXT("PreviewScript.xml"));

   // Disable control and pass ScriptFile + Preferences
   utilEnableDlgItem(hDialog, IDC_PREVIEW_CODE_EDIT, FALSE);
   CodeEdit_SetPreferences(GetControl(hDialog, IDC_PREVIEW_CODE_EDIT), pDialogData->pPreferencesCopy);
   CodeEdit_SetScriptFile(GetControl(hDialog, IDC_PREVIEW_CODE_EDIT), pDialogData->pScriptFile);
   
   /// [COLOURS] Fill text/background colour combos
   for (INTERFACE_COLOUR  eColour = IC_BLACK; eColour <= IC_WHITE; eColour = (INTERFACE_COLOUR)(eColour + 1))
   {
      // Load colour string
      LoadString(getResourceInstance(), IDS_COLOUR_BLACK + eColour, szItemText, 64);

      // Add to text and background combo
      appendCustomComboBoxItemEx(GetControl(hDialog, IDC_TEXT_COLOUR_COMBO),       szItemText, NULL, szColourIcons[eColour], NULL);
      appendCustomComboBoxItemEx(GetControl(hDialog, IDC_BACKGROUND_COLOUR_COMBO), szItemText, NULL, szColourIcons[eColour], NULL);
   }
   
   // Select current background colour and text colour of the first CodeObject class (Script arguments)
   ComboBox_SetCurSel(GetControl(hDialog, IDC_BACKGROUND_COLOUR_COMBO), pColourScheme->eBackgroundColour);
   ComboBox_SetCurSel(GetControl(hDialog, IDC_TEXT_COLOUR_COMBO),       pColourScheme->eCodeObjectColours[CO_ARGUMENT]);
   
   /// [CODE-OBJECTS]
   // Calculate ListView column size
   oComponentsListView.iColumnWidths[0] = utilGetWindowWidth(GetControl(hDialog, IDC_CODE_OBJECT_LIST));

   // Setup ListView columns and ImageList
   initReportModeListView(GetControl(hDialog, IDC_CODE_OBJECT_LIST), &oComponentsListView, FALSE);
   SubclassWindow(GetControl(hDialog, IDC_CODE_OBJECT_LIST), wndprocCustomListView);

   // Select first item
   ListView_SetItemCount(GetControl(hDialog, IDC_CODE_OBJECT_LIST), DISPLAY_CODEOBJECTS);
   ListView_SetItemState(GetControl(hDialog, IDC_CODE_OBJECT_LIST), 0, LVIS_SELECTED, LVIS_SELECTED);

   // Cleanup
   utilDeleteStrings(szPreviewText, szItemText);
   return TRUE;
}


/// Function name  : updatePreferencesFoldersPageState
// Description     : Displays the appropriate icon for the validation state of the game folder or mod folder
// 
// CONST PREFERENCES*  pPreferencesCopy  : [in] Copy of the preferences object
// HWND                hPage             : [in] Window handle of the Folder page
// CONST UINT          iIconControlID    : [in] ID of the static control to display the result of the validation in.
// 
VOID  updatePreferencesFoldersPageState(CONST PREFERENCES*  pPreferencesCopy, HWND  hPage, CONST UINT  iIconControlID)
{
   TCHAR*        szScriptFolder;    // Full path of 'scripts' sub-folder
   CONST TCHAR*  szIcon;            // Icon to be displayed to indicate the result of the validation

   // Examine icon control
   switch (iIconControlID)
   {
   /// [GAME FOLDER ICON] Determine icon from game folder validation result
   case IDC_GAME_FOLDER_STATUS:
      // Examine folder state
      switch (pPreferencesCopy->eGameFolderState)
      {
      // [VALID] Use the appropriate icon for the identified game version
      case GFS_VALID:
         switch (pPreferencesCopy->eGameVersion)
         {
         case GV_THREAT:          szIcon = TEXT("VALID_THREAT_ICON");  break;
         case GV_REUNION:         szIcon = TEXT("VALID_REUNION_ICON"); break;
         case GV_TERRAN_CONFLICT: szIcon = TEXT("VALID_TERRAN_CONFLICT_ICON");     break;
         case GV_ALBION_PRELUDE:  szIcon = TEXT("VALID_ALBION_PRELUDE_ICON");      break;
         }
         break;

      // [INCOMPLETE/INVALID] Use the 'Warning' or 'Invalid' icon
      case GFS_INCOMPLETE:        szIcon = TEXT("WARNING_ICON");       break;
      case GFS_INVALID:           szIcon = TEXT("INVALID_ICON");       break;
      }

      /// [SCRIPT FOLDER] Display script folder if possible
      if (pPreferencesCopy->eGameFolderState == GFS_VALID)
      {
         // [VALID] Generate and display script folder
         szScriptFolder = generateGameDataFilePath(GFI_SCRIPT_FOLDER, pPreferencesCopy->szGameFolder, pPreferencesCopy->eGameVersion);
         PathSetDlgItemPath(hPage, IDC_SCRIPT_FOLDER_EDIT, szScriptFolder);
         utilDeleteString(szScriptFolder);
      }
      else
         // [INCOMPLETE/INVALID] Display 'Unable to Determine'
         SetDlgItemText(hPage, IDC_SCRIPT_FOLDER_EDIT, TEXT("[Unable to Determine]"));
      break;
   }

   // Display the specified icon
   Static_SetIcon(GetControl(hPage, iIconControlID), LoadIcon(getResourceInstance(), szIcon));

   // Retrieve enabled/disabled icon
   //HICON hIcon = ImageList_GetIcon(getAppImageList(ITS_MEDIUM), getAppImageTreeIconIndex(ITS_MEDIUM, szIcon), ILD_TRANSPARENT WITH ILD_BLEND);
   //SendDlgItemMessage(hPage, iIconControlID, STM_SETICON, (WPARAM)hIcon, NULL);
}


/// Function name  : updatePreferencesGeneralPageSliderValue
// Description     : Adjusts the slider and it's label
// 
// OPTIONS_DIALOG_DATA*  pDialogData : [in] Dialog data
// CONST UINT            iDepth      : [in] Recursion depth
// 
VOID  updatePreferencesGeneralPageSliderValue(PREFERENCES_DATA*  pDialogData, HWND  hPage, CONST UINT  iControlID, UINT  iDelay)
{
   UINT  iStaticID;

   // [CHECK] Enforce range
   iDelay = min(12, iDelay);

   // Determine label
   iStaticID = (iControlID == IDC_CODE_TOOLTIPS_SLIDER ? IDC_CODE_TOOLTIPS_STATIC : IDC_SEARCH_TOOLTIPS_STATIC);

   /// Update slider
   SendDlgItemMessage(hPage, iControlID, TBM_SETPOS, TRUE, iDelay);

   /// Update label
   utilSetWindowTextf(GetControl(hPage, iStaticID), (iDelay > 1 ? TEXT("%d seconds") : TEXT("%d second")), iDelay);

   // Store current delay
   switch (iControlID)
   {
   case IDC_CODE_TOOLTIPS_SLIDER:   pDialogData->pPreferencesCopy->iEditorTooltipDelay       = iDelay;   break;
   case IDC_SEARCH_TOOLTIPS_SLIDER: pDialogData->pPreferencesCopy->iSearchResultTooltipDelay = iDelay;   break;
   }
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onPreferencesAppearancePage_Command
// Description     : WM_COMMAND processing for 'Appearance' preferences PropertyPage
//
// 
// PREFERENCES*  pPreferencesCopy : [in] Copy of the current application preferences values
// HWND          hDialog          : [in] Window handle of the property page
// const UINT    iControlID       : [in] Dialog ID of the control or menu item sending this notification
// const UINT    iNotification    : [in] [CONTROLS] Event notificaton code
// 
// Return type : TRUE if processed, FALSE if not
//
BOOL  onPreferencesAppearancePage_Command(PREFERENCES_DATA*  pDialogData, HWND  hDialog, CONST UINT  iControlID, CONST UINT  iNotification, HWND  hCtrl)
{
   LIST_ITEM      *pNewSchemeItem;     // ListItem wrapper
   COLOUR_SCHEME  *pNewScheme,         // New Colour Scheme convenience pointer
                  *pOldScheme;         // Old Colour scheme convenience pointer
   TCHAR           szFontSize[4];      // New colour scheme Font size, as a string
   BOOL            bResult;

   // Prepare
   TRACK_FUNCTION();
   pOldScheme = &pDialogData->pPreferencesCopy->oColourScheme;
   bResult    = FALSE;

   // Examine source
   switch (iControlID)
   {
   /// [FONT NAME CHANGED]
   case IDC_FONT_NAME_COMBO:
      if (bResult = (iNotification == CBN_SELCHANGE))
         // Re-populate FontSize ComboBox
         populateAppearancePageFontSizesCombo(pDialogData, hDialog);
      break;

   /// [COLOUR SCHEME CHANGED]
   case IDC_COLOUR_SCHEME_COMBO:
      /// Set new scheme. Display appropriate font name/size etc.
      if (bResult = (iNotification == CBN_SELCHANGE))
      {
         // Lookup associated ColourScheme
         findListItemByIndex(pDialogData->pColourSchemeList, ComboBox_GetCurSel(GetControl(hDialog, IDC_COLOUR_SCHEME_COMBO)), pNewSchemeItem);
         ASSERT(pNewScheme = extractListItemPointer(pNewSchemeItem, COLOUR_SCHEME));

         // Update Preferences Copy         
         *pOldScheme = *pNewScheme;

         // Update the Font Name INCLUDES bold flag
         ComboBox_SelectString(GetControl(hDialog, IDC_FONT_NAME_COMBO), -1, pNewScheme->szFontName);
         CheckDlgButton(hDialog, IDC_BOLD_FONT_CHECK, pNewScheme->bFontBold);

         // Update the Font Size combo
         StringCchPrintf(szFontSize, 4, TEXT("%u"), pNewScheme->iFontSize);
         ComboBox_SelectString(GetControl(hDialog, IDC_FONT_SIZE_COMBO), -1, szFontSize);
      }
      break;

   /// [SAVE COLOUR SCHEME]
   case IDC_COLOUR_SCHEME_SAVE:
      displayMessageDialogf(NULL, IDS_FEATURE_NOT_IMPLEMENTED, MAKEINTRESOURCE(IDS_TITLE_NOT_IMPLEMENTED), MDF_OK WITH MDF_ERROR, TEXT("Saving Colour Schemes"));
      break;
   }

   // Return result
   END_TRACKING();
   return bResult;
}


/// Function name  : onPreferencesFoldersPage_Command
// Description     : Displays 'Browser' dialog and validates the folder paths after a change
//
// PREFERENCES_DATA*  pDialogData   : [in] Dialog data
// HWND               hDialog       : [in] Window handle of the property page
// const UINT         iControlID    : [in] Dialog ID of the control or menu item sending this notification
// const UINT         iNotification : [in] Notificaton code
// HWND               hCtrl         : [in] Control handle
// 
// Return type : TRUE if processed, FALSE if not
//
BOOL  onPreferencesFoldersPage_Command(PREFERENCES_DATA*  pDialogData, HWND  hDialog, CONST UINT  iControlID, CONST UINT  iNotification, HWND  hCtrl)
{
   // Examine source
   switch (iControlID)
   {
   /// [BROWSE: GAME FOLDER] Browse for new folder path
   case IDC_GAME_FOLDER_BROWSE:
      performPreferencesFolderBrowse(pDialogData, hDialog, IDC_GAME_FOLDER_COMBO);
      // Validate new folder and display result
      performPreferencesFolderValidation(pDialogData, hDialog, IDC_GAME_FOLDER_COMBO); 
      updatePreferencesFoldersPageState(pDialogData->pPreferencesCopy, hDialog, IDC_GAME_FOLDER_STATUS);
      return TRUE;

   /// [EDIT: GAME or MOD GAME FOLDER] Validate new folder path
   case IDC_GAME_FOLDER_COMBO:
      // [TEXT CHANGED] Revalidate
      if (iNotification == CBN_EDITCHANGE)
      {
         // Attempt to validate new path and display result
         performPreferencesFolderValidation(pDialogData, hDialog, iControlID);  
         updatePreferencesFoldersPageState(pDialogData->pPreferencesCopy, hDialog, IDC_GAME_FOLDER_STATUS); 
         return TRUE;
      }
      // [SELECTION CHANGED] Manually post a 'text changed' notification, as this notification is sent before the edit control is updated
      else if (iNotification == CBN_SELCHANGE)
      {
         // Update text
         SetWindowText(hCtrl, getCustomComboBoxItemText(hCtrl, ComboBox_GetCurSel(hCtrl)));
         // Post notification
         PostMessage(hDialog, WM_COMMAND, MAKE_LONG(iControlID, CBN_EDITCHANGE), (LPARAM)hCtrl);
      }
      break;
   }

   // [UNHANDLED] Return FALSE
   return FALSE;
}


/// Function name  : onPreferencesGeneralPage_Command
// Description     : Enables/Disables the sliders 
//
// PREFERENCES_DATA*  pDialogData   : [in] Dialog data
// HWND               hDialog       : [in] Window handle of the property page
// const UINT         iControlID    : [in] Dialog ID of the control or menu item sending this notification
// const UINT         iNotification : [in] Notificaton code
// HWND               hCtrl         : [in] Control handle
// 
// Return type : TRUE if processed, FALSE if not
//
BOOL  onPreferencesGeneralPage_Command(PREFERENCES_DATA*  pDialogData, HWND  hDialog, CONST UINT  iControlID, CONST UINT  iNotification, HWND  hCtrl)
{
   BOOL   bEnabled;

   // Examine source
   switch (iControlID)
   {
   /// [TOOLTIP CHECK] Enable/Disable slider
   case IDC_CODE_TOOLTIPS_CHECK:
      // Prepare
      bEnabled = IsDlgButtonChecked(hDialog, iControlID);

      // Set slider/static states
      utilEnableDlgItem(hDialog, IDC_CODE_TOOLTIPS_SLIDER, bEnabled);
      utilEnableDlgItem(hDialog, IDC_CODE_TOOLTIPS_STATIC, bEnabled);
      return TRUE;

   /// [TOOLTIP CHECK] Enable/Disable slider
   case IDC_SEARCH_TOOLTIPS_CHECK:
      // Prepare
      bEnabled = IsDlgButtonChecked(hDialog, iControlID);

      // Set slider/static states
      utilEnableDlgItem(hDialog, IDC_SEARCH_TOOLTIPS_SLIDER, bEnabled);
      utilEnableDlgItem(hDialog, IDC_SEARCH_TOOLTIPS_STATIC, bEnabled);
      return TRUE;

   }

   // [UNHANDLED] Return FALSE
   return FALSE;
}



/// Function name  : onPreferencesGeneralPage_AdjustDelay
// Description     : Slider adjustment handler
// 
// PREFERENCES_DATA*  pDialogData : [in] Dialog data
// CONST UINT         eScrollType : [in] Type of scroll
// CONST UINT         iAmount     : [in] Current drag position
// 
VOID  onPreferencesGeneralPage_AdjustDelay(PREFERENCES_DATA*  pDialogData, HWND  hPage, CONST UINT  iControlID, CONST UINT  eScrollType, CONST UINT  iAmount)
{
   UINT  iNewDelay,     // New delay
         iMinDelay,     // Minimum delay
         iMaxDelay;     // Maximum delay

   // Get current delay
   switch (iControlID)
   {
   case IDC_CODE_TOOLTIPS_SLIDER:   iNewDelay = pDialogData->pPreferencesCopy->iEditorTooltipDelay;         break;
   case IDC_SEARCH_TOOLTIPS_SLIDER: iNewDelay = pDialogData->pPreferencesCopy->iSearchResultTooltipDelay;   break;
   }

   // Get minimum and maximum
   iMinDelay = SendDlgItemMessage(hPage, iControlID, TBM_GETRANGEMIN, NULL, NULL);
   iMaxDelay = SendDlgItemMessage(hPage, iControlID, TBM_GETRANGEMAX, NULL, NULL);

   // Examine scroll type
   switch (eScrollType)
   {
   /// [NUDGE]
   case SB_PAGERIGHT:
   case SB_LINERIGHT:   iNewDelay++;   break;
   case SB_PAGELEFT:
   case SB_LINELEFT:    iNewDelay--;   break;

   /// [HOME/END] 
   case SB_LEFT:        iNewDelay = iMinDelay;  break;
   case SB_RIGHT:       iNewDelay = iMaxDelay;  break;

   /// [DRAG]
   case SB_THUMBTRACK:  iNewDelay = iAmount;    break;

   // [CRAP] Block
   case SB_THUMBPOSITION:
   case SB_ENDSCROLL:
      return;
   }

   // [CHECK] Enforce range
   iNewDelay = utilEnforceRange(iNewDelay, iMinDelay, iMaxDelay);

   /// Update slider and dialog data
   updatePreferencesGeneralPageSliderValue(pDialogData, hPage, iControlID, iNewDelay);
}


/// Function name  : onPreferencesPage_Hide
// Description     : Validate the control data for a specified 'Preferences' property-sheet page
// 
// PREFERENCES_DATA*       pDialogData : [in] Preferences dialog data
// HWND                    hDialog     : [in] Window handle of a page in the preferences property sheet
// CONST PREFERENCES_PAGE  ePage       : [in] ID of a page in the preferences property sheet
// 
// Return type : TRUE if validation was successful, FALSE if there is an error
//
BOOL  onPreferencesPage_Hide(PREFERENCES_DATA*  pDialogData, HWND  hDialog, CONST PREFERENCES_PAGE  ePage)
{
   PREFERENCES*   pPreferencesCopy;    // Preferences copy convenience pointer
   COLOUR_SCHEME* pColourScheme;       // Colour scheme convenience pointer

   // Prepare
   pPreferencesCopy = pDialogData->pPreferencesCopy;
   pColourScheme = &pPreferencesCopy->oColourScheme;

   switch (ePage)
   {
   /// [GENERAL] - Save language and options
   case PP_GENERAL:
      // App Language
      pPreferencesCopy->eAppLanguage = convertInterfaceLanguageToAppLanguage((INTERFACE_LANGUAGE)ComboBox_GetCurSel(GetControl(hDialog, IDC_APPLICATION_LANGUAGE_COMBO)));

      // Options
      pPreferencesCopy->bPreserveSession       = IsDlgButtonChecked(hDialog, IDC_PRESERVE_SESSION_CHECK);   
      pPreferencesCopy->bUseSystemDialog       = IsDlgButtonChecked(hDialog, IDC_USE_SYSTEM_DIALOG_CHECK);
      pPreferencesCopy->bCodeIndentation       = IsDlgButtonChecked(hDialog, IDC_CODE_INDENTATION_CHECK);
      pPreferencesCopy->bScriptCodeMacros      = IsDlgButtonChecked(hDialog, IDC_CODE_MACROS_CHECK);
      pPreferencesCopy->bTransparentProperties = !IsDlgButtonChecked(hDialog, IDC_PROPERTIES_TRANSPARENCY_CHECK);    // Check is for 'disable'
      pPreferencesCopy->bTutorialMode          = !IsDlgButtonChecked(hDialog, IDC_TUTORIAL_MODE_CHECK);              // Check is for 'disable'
      pPreferencesCopy->bEditorTooltips        = IsDlgButtonChecked(hDialog, IDC_CODE_TOOLTIPS_CHECK);
      pPreferencesCopy->bSearchResultTooltips  = IsDlgButtonChecked(hDialog, IDC_SEARCH_TOOLTIPS_CHECK);
      pPreferencesCopy->bUseDoIfSyntax         = IsDlgButtonChecked(hDialog, IDC_CODE_DO_IF_CHECK);
      pPreferencesCopy->bSuggestions[CST_COMMAND]       = IsDlgButtonChecked(hDialog, IDC_SUGGEST_COMMANDS_CHECK);
      pPreferencesCopy->bSuggestions[CST_GAME_OBJECT]   = IsDlgButtonChecked(hDialog, IDC_SUGGEST_GAME_OBJECTS_CHECK);
      pPreferencesCopy->bSuggestions[CST_SCRIPT_OBJECT] = IsDlgButtonChecked(hDialog, IDC_SUGGEST_SCRIPT_OBJECTS_CHECK);
      pPreferencesCopy->bSuggestions[CST_VARIABLE]      = IsDlgButtonChecked(hDialog, IDC_SUGGEST_VARIABLES_CHECK);

      // Slider Values
      pPreferencesCopy->iEditorTooltipDelay       = SendDlgItemMessage(hDialog, IDC_CODE_TOOLTIPS_SLIDER, TBM_GETPOS, NULL, NULL);
      pPreferencesCopy->iSearchResultTooltipDelay = SendDlgItemMessage(hDialog, IDC_SEARCH_TOOLTIPS_SLIDER, TBM_GETPOS, NULL, NULL);
      break;

   /// [FOLDERS] - Replace the existing strings
   case PP_FOLDERS:
      // Game Language
      pPreferencesCopy->eGameLanguage = convertAppLanguageToGameLanguage((APP_LANGUAGE)ComboBox_GetCurSel(GetControl(hDialog, IDC_GAME_LANGUAGE_COMBO)));
      // Update game folder
      GetDlgItemText(hDialog, IDC_GAME_FOLDER_COMBO, pPreferencesCopy->szGameFolder, MAX_PATH);
      PathAddBackslash(pPreferencesCopy->szGameFolder);
      // Options
      pPreferencesCopy->bReportGameStringOverwrites  = IsDlgButtonChecked(hDialog, IDC_REPORT_LANGUAGE_OVERWRITES_CHECK);
      pPreferencesCopy->bReportMissingSubStrings     = IsDlgButtonChecked(hDialog, IDC_REPORT_MISSING_SUBSTRINGS_CHECK);
      pPreferencesCopy->bReportIncompleteGameObjects = IsDlgButtonChecked(hDialog, IDC_REPORT_INCOMPLETE_GAMEOBJECTS_CHECK);
      break;

   /// [APPEARANCE] - Save font name and size
   case PP_APPEARANCE:
      // Font name
      GetDlgItemText(hDialog, IDC_FONT_NAME_COMBO, pColourScheme->szFontName, LF_FACESIZE);
      // Font size and bold
      pColourScheme->iFontSize = GetDlgItemInt(hDialog, IDC_FONT_SIZE_COMBO, NULL, FALSE);
      pColourScheme->bFontBold = IsDlgButtonChecked(hDialog, IDC_BOLD_FONT_CHECK);
      // Colour Scheme
      //GetDlgItemText(hDialog, IDC_COLOUR_SCHEME_COMBO, pColourScheme->szName, 64);
      break;

   /// [SYNTAX] - Not required -- saved by page logic
   case PP_SYNTAX:
      break;

   /// [MISC] - Save error handling choice and options
   case PP_MISC:
      // Error handling
      pPreferencesCopy->eErrorHandling = (ERROR_HANDLING)SendDlgItemMessage(hDialog, IDC_ERROR_HANDLING_COMBO, CB_GETCURSEL, NULL, NULL);
      // Options
      pPreferencesCopy->bSubmitBugReports = IsDlgButtonChecked(hDialog, IDC_BUG_REPORTS_CHECK);
      pPreferencesCopy->bPassiveFTP       = IsDlgButtonChecked(hDialog, IDC_PASSIVE_FTP_CHECK);
      pPreferencesCopy->bAutomaticUpdates = IsDlgButtonChecked(hDialog, IDC_AUTOMATIC_UPDATES_CHECK);
      // Forum UserName
      GetDlgItemText(hDialog, IDC_FORUM_USERNAME_EDIT, pPreferencesCopy->szForumUserName, 32);
      cleanSubmissionForumUserName(pPreferencesCopy->szForumUserName, 32);
      break;
   }

   return TRUE;
}

/// Function name  : onPreferencesPage_Show
// Description     : Display the input data for a specified 'Preferences' property-sheet page
// 
// PREFERENCES_DATA*       pDialogData : [in] Preferences dialog data
// HWND                    hDialog     : [in] Window handle of a page in the preferences property sheet
// CONST PREFERENCES_PAGE  ePage       : [in] ID of a page in the preferences property sheet
// 
// Returns : TRUE
//
BOOL  onPreferencesPage_Show(PREFERENCES_DATA*  pDialogData, HWND  hDialog, CONST PREFERENCES_PAGE  ePage)
{
   PREFERENCES*   pPreferencesCopy;   // Convenience pointer for the preferences copy
   COLOUR_SCHEME* pColourScheme;      // Convenience pointer for preferences copy colour scheme

   // Prepare
   pPreferencesCopy = pDialogData->pPreferencesCopy;
   pColourScheme    = &pPreferencesCopy->oColourScheme;

   // Examine page
   switch (ePage)
   {
   /// [GENERAL] - Choose language and option values
   case PP_GENERAL:
      // App Language
      ComboBox_SetCurSel(GetControl(hDialog, IDC_APPLICATION_LANGUAGE_COMBO), convertAppLanguageToInterfaceLanguage(pPreferencesCopy->eAppLanguage));

      // Select options
      CheckDlgButton(hDialog, IDC_PRESERVE_SESSION_CHECK,         pPreferencesCopy->bPreserveSession);
      CheckDlgButton(hDialog, IDC_USE_SYSTEM_DIALOG_CHECK,        pPreferencesCopy->bUseSystemDialog);
      CheckDlgButton(hDialog, IDC_CODE_INDENTATION_CHECK,         pPreferencesCopy->bCodeIndentation);
      CheckDlgButton(hDialog, IDC_CODE_MACROS_CHECK,              pPreferencesCopy->bScriptCodeMacros);
      CheckDlgButton(hDialog, IDC_CODE_DO_IF_CHECK,               pPreferencesCopy->bUseDoIfSyntax);
      CheckDlgButton(hDialog, IDC_PROPERTIES_TRANSPARENCY_CHECK, !pPreferencesCopy->bTransparentProperties);      // Check is for 'disable'
      CheckDlgButton(hDialog, IDC_TUTORIAL_MODE_CHECK,           !pPreferencesCopy->bTutorialMode);               // Check is for 'disable'
      CheckDlgButton(hDialog, IDC_CODE_TOOLTIPS_CHECK,            pPreferencesCopy->bEditorTooltips);
      CheckDlgButton(hDialog, IDC_SEARCH_TOOLTIPS_CHECK,          pPreferencesCopy->bSearchResultTooltips);
      CheckDlgButton(hDialog, IDC_SUGGEST_COMMANDS_CHECK,         pPreferencesCopy->bSuggestions[CST_COMMAND]);
      CheckDlgButton(hDialog, IDC_SUGGEST_GAME_OBJECTS_CHECK,     pPreferencesCopy->bSuggestions[CST_GAME_OBJECT]);
      CheckDlgButton(hDialog, IDC_SUGGEST_SCRIPT_OBJECTS_CHECK,   pPreferencesCopy->bSuggestions[CST_SCRIPT_OBJECT]);
      CheckDlgButton(hDialog, IDC_SUGGEST_VARIABLES_CHECK,        pPreferencesCopy->bSuggestions[CST_VARIABLE]);
      
      // Slider states
      utilEnableDlgItem(hDialog, IDC_CODE_TOOLTIPS_STATIC,   pPreferencesCopy->bEditorTooltips);
      utilEnableDlgItem(hDialog, IDC_CODE_TOOLTIPS_SLIDER,   pPreferencesCopy->bEditorTooltips);
      utilEnableDlgItem(hDialog, IDC_SEARCH_TOOLTIPS_STATIC, pPreferencesCopy->bSearchResultTooltips);
      utilEnableDlgItem(hDialog, IDC_SEARCH_TOOLTIPS_SLIDER, pPreferencesCopy->bSearchResultTooltips);

      // Slider Values
      updatePreferencesGeneralPageSliderValue(pDialogData, hDialog, IDC_CODE_TOOLTIPS_SLIDER,   pPreferencesCopy->iEditorTooltipDelay);
      updatePreferencesGeneralPageSliderValue(pDialogData, hDialog, IDC_SEARCH_TOOLTIPS_SLIDER, pPreferencesCopy->iSearchResultTooltipDelay);
      break;

   /// [FOLDERS] - Display folder paths
   case PP_FOLDERS:
      // Select Game Language
      ComboBox_SetCurSel(GetControl(hDialog, IDC_GAME_LANGUAGE_COMBO), convertGameLanguageToAppLanguage(pPreferencesCopy->eGameLanguage));

      // Display game folder
      PathRemoveBackslash(pPreferencesCopy->szGameFolder);
      SetDlgItemText(hDialog, IDC_GAME_FOLDER_COMBO, pPreferencesCopy->szGameFolder);

      // Select Options
      CheckDlgButton(hDialog, IDC_REPORT_LANGUAGE_OVERWRITES_CHECK,    pPreferencesCopy->bReportGameStringOverwrites);
      CheckDlgButton(hDialog, IDC_REPORT_MISSING_SUBSTRINGS_CHECK,     pPreferencesCopy->bReportMissingSubStrings);
      CheckDlgButton(hDialog, IDC_REPORT_INCOMPLETE_GAMEOBJECTS_CHECK, pPreferencesCopy->bReportIncompleteGameObjects);
      break;

   /// [APPEARANCE] -- Select current font and colour scheme
   case PP_APPEARANCE:
      // Select Font name. Set bold flag
      SendDlgItemMessage(hDialog, IDC_FONT_NAME_COMBO, CB_SELECTSTRING, -1, (LPARAM)pColourScheme->szFontName);
      CheckDlgButton(hDialog,     IDC_BOLD_FONT_CHECK, pColourScheme->bFontBold ? TRUE : FALSE);
      // Select Colour scheme
      SendDlgItemMessage(hDialog, IDC_COLOUR_SCHEME_COMBO, CB_SELECTSTRING, -1, (LPARAM)pColourScheme->szName);

      // Populate FontSize ComboBox
      populateAppearancePageFontSizesCombo(pDialogData, hDialog);
      break;

   /// [SYNTAX] -- Select first CodeObject
   case PP_SYNTAX:
      // Select first display CodeObject (Arguments)
      SendDlgItemMessage(hDialog, IDC_CODE_OBJECT_LIST,        LB_SETCURSEL, (UINT)0, NULL);
      // Display Background colour and Arguments text colour
      SendDlgItemMessage(hDialog, IDC_TEXT_COLOUR_COMBO,       CB_SETCURSEL, (UINT)pColourScheme->eCodeObjectColours[CO_ARGUMENT], NULL);
      SendDlgItemMessage(hDialog, IDC_BACKGROUND_COLOUR_COMBO, CB_SETCURSEL, (UINT)pColourScheme->eBackgroundColour, NULL);
      // Force 'CodeEdit Preview' to reflect font and size chosen on 'Appearance' page by sending a 'Prefs Changed' message every time we display this page
      Preferences_Changed(GetControl(hDialog, IDC_PREVIEW_CODE_EDIT));
      break;

   /// [MISC] - Display error handling and option values
   case PP_MISC:
      // Error handling
      SendDlgItemMessage(hDialog, IDC_ERROR_HANDLING_COMBO, CB_SETCURSEL, (UINT)pPreferencesCopy->eErrorHandling, NULL);
      // Options
      CheckDlgButton(hDialog, IDC_BUG_REPORTS_CHECK,       pPreferencesCopy->bSubmitBugReports ? TRUE : FALSE);
      CheckDlgButton(hDialog, IDC_PASSIVE_FTP_CHECK,       pPreferencesCopy->bPassiveFTP       ? TRUE : FALSE);
      CheckDlgButton(hDialog, IDC_AUTOMATIC_UPDATES_CHECK, pPreferencesCopy->bAutomaticUpdates ? TRUE : FALSE);
      // Display forum username
      SetDlgItemText(hDialog, IDC_FORUM_USERNAME_EDIT, pPreferencesCopy->szForumUserName);
      break;
   }

   return TRUE;
}


/// Function name  : onPreferencesSyntaxPage_Command
// Description     : WM_COMMAND processing for 'Syntax' preferences PropertyPage
//
// PREFERENCES*  pPreferencesCopy : [in] Copy of the current application preferences values
// HWND          hDialog          : [in] Window handle of the property page
// const UINT    iControlID       : [in] Dialog ID of the control or menu item sending this notification
// const UINT    iNotification    : [in] [CONTROLS] Event notificaton code
// 
// Return type : TRUE if processed, FALSE if not
//
BOOL  onPreferencesSyntaxPage_Command(PREFERENCES_DATA*  pDialogData, HWND  hDialog, CONST UINT  iControlID, CONST UINT  iNotification)
{
   COLOUR_SCHEME*    pColourScheme;       // Colour scheme convenience pointer
   CODEOBJECT_CLASS  eCodeObject;         // Currently selected CodeObject
   INTERFACE_COLOUR  eTextColour,         // Currently selected foreground colour
                     eBackgroundColour;   // Currently selected background colour
   BOOL              bResult;             // Operation result

   // Prepare
   pColourScheme = &pDialogData->pPreferencesCopy->oColourScheme;
   bResult       = FALSE;

   // Get the currently selected CodeObject, TextColour and BackgroundColour
   eCodeObject       = (CODEOBJECT_CLASS)ListView_GetNextItem(GetControl(hDialog, IDC_CODE_OBJECT_LIST), -1, LVNI_SELECTED);
   eTextColour       = (INTERFACE_COLOUR)ComboBox_GetCurSel(GetControl(hDialog, IDC_TEXT_COLOUR_COMBO));
   eBackgroundColour = (INTERFACE_COLOUR)ComboBox_GetCurSel(GetControl(hDialog, IDC_BACKGROUND_COLOUR_COMBO));

   switch (iControlID)
   {
   /// [TEXT COLOUR]
   case IDC_TEXT_COLOUR_COMBO:
      // [SELECTION CHANGED]
      if (bResult = (iNotification == CBN_SELCHANGE))
      {
         // Save new text colour in the current CodeObject
         pColourScheme->eCodeObjectColours[eCodeObject] = eTextColour;

         // [SPECIAL CASE] Use the same colour for VARIABLES as the user chooses for NULL
         if (eCodeObject == CO_VARIABLE)
            pColourScheme->eCodeObjectColours[CO_NULL] = eTextColour;

         // Update ListView
         ListView_Update(GetControl(hDialog, IDC_CODE_OBJECT_LIST), eCodeObject);
      }
      break;

   /// [BACKGROUND COLOUR]
   case IDC_BACKGROUND_COLOUR_COMBO:
      // [SELECTION CHANGED] Save new background colour
      if (bResult = (iNotification == CBN_SELCHANGE))
         pColourScheme->eBackgroundColour = eBackgroundColour;
      break;
   }

   // [CHANGE] Refresh Preview CodeEdit
   if (bResult)
      Preferences_Changed(GetControl(hDialog, IDC_PREVIEW_CODE_EDIT));

   // Return result
   return bResult;
}


/// Function name  : onPreferencesSyntaxPage_Notify
// Description     : Provides CodeObject ListView data and changes the text colour when user changes CodeObjects
// 
// PREFERENCES_DATA*  pDialogData : [in] Dialog data
// HWND               hPage       : [in] Syntax page handle
// NMHDR*             pHeader     : [in] Message header
// 
// Return Value   : TRUE if processed, otherwise FALSE
// 
BOOL  onPreferencesSyntaxPage_Notify(PREFERENCES_DATA*  pDialogData, HWND  hPage, NMHDR*  pHeader)
{
   COLOUR_SCHEME*    pColourScheme;       // Colour scheme convenience pointer
   NMLISTVIEW*       pSelection;          // Item activation message data
   NMLVDISPINFO*     pRequestInfo;        // Data request message data
   LVITEM*           pRequestItem;        // Item data for item being requeted
   INTERFACE_COLOUR  eItemColour;         // Colour of the selected CodeObject
   BOOL              bResult;             // Operation result

   // Prepare
   pColourScheme = &pDialogData->pPreferencesCopy->oColourScheme;
   bResult       = TRUE;

   // Examine notification
   switch (pHeader->code)
   {
   /// [REQUEST DATA] Supply CodeObject text
   case LVN_GETDISPINFO:
      // Prepare
      pRequestInfo = (NMLVDISPINFO*)pHeader;
      pRequestItem = &pRequestInfo->item;

      // [TEXT] Supply CodeObject description
      if (pRequestItem->mask INCLUDES LVIF_TEXT)
         LoadString(getResourceInstance(), IDS_CODE_OBJECT_ARGUMENT + pRequestItem->iItem, pRequestItem->pszText, pRequestItem->cchTextMax);
      
      // [ICON] Supply appropriate coloured icon
      if (pRequestItem->mask INCLUDES LVIF_IMAGE)
      {
         // Determine the item colour
         eItemColour = pColourScheme->eCodeObjectColours[pRequestItem->iItem];
         // Lookup the icon matching that colour
         pRequestItem->iImage = getAppImageTreeIconIndex(ITS_SMALL, szColourIcons[eItemColour]);
      }
      break;

   /// [ITEM CHANGED] Select appropriate text colour
   case LVN_ITEMCHANGED:
      // Prepare
      pSelection = (NMLISTVIEW*)pHeader;

      // [CHECK] Has a new item been selected?
      if (pSelection->uNewState INCLUDES LVIS_SELECTED)
         // Select new CodeObject's text colour
         ComboBox_SetCurSel(GetControl(hPage, IDC_TEXT_COLOUR_COMBO), pColourScheme->eCodeObjectColours[pSelection->iItem]);
      break;

   /// [CUSTOM DRAW] Draw listview
   case NM_CUSTOMDRAW:
      if (pHeader->idFrom == IDC_CODE_OBJECT_LIST)    // [BUG: doesn't work! wtf?]
         SetWindowLong(hPage, DWL_MSGRESULT, onCustomDrawListView(hPage, pHeader->hwndFrom, (NMLVCUSTOMDRAW*)pHeader));
      break;

   // [UNHANDLED]
   default:
      bResult = FALSE;
      break;
   }

   // Return result
   return bResult;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURES
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : dlgprocPreferencesAppearancePage
// Description     : Window procedure pass-through for the 'Appearance Preferences' property page
//
//
INT_PTR   dlgprocPreferencesAppearancePage(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   PREFERENCES_DATA*  pDialogData;
   ERROR_STACK*       pException;
   BOOL               bResult;

   __try
   {
      // Prepare
      TRACK_FUNCTION();
      pDialogData = getPreferencesData(hDialog);
      bResult     = FALSE;

      /// [COMMAND PROCESSING] - Control notification
      switch (iMessage)
      {
      case WM_COMMAND:
         if (onPreferencesAppearancePage_Command(pDialogData, hDialog, LOWORD(wParam), HIWORD(wParam), (HWND)lParam))
            bResult = TRUE;
         break;

      // [CUSTOM COMBO]
      case WM_MEASUREITEM:  
         bResult = onWindow_MeasureComboBox((MEASUREITEMSTRUCT*)lParam, ITS_SMALL, ITS_MEDIUM);  
         break;
      }

      // Pass to base
      END_TRACKING();
      return utilEither(bResult, dlgprocPreferencesPage(hDialog, iMessage, wParam, lParam, PP_APPEARANCE));
   }
   /// [EXCEPTION HANDLER]
   __except (generateExceptionError(GetExceptionInformation(), pException))
   {
      // [ERROR] "An unidentified and unexpected critical error has occurred in the preferences %s window"
      enhanceError(pException, ERROR_ID(IDS_EXCEPTION_PREFERENCES_DIALOG), szDebugPageNames[PP_APPEARANCE]);
      displayException(pException);
      return 0;
   }
}


/// Function name  : dlgprocPreferencesFoldersPage
// Description     : Window procedure pass-through for the 'Folder Preferences' property page
//
// 
INT_PTR   dlgprocPreferencesFoldersPage(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   PREFERENCES_DATA*  pDialogData;
   ERROR_STACK*       pException;
   BOOL               bResult;

   __try
   {
      // Prepare
      pDialogData = getPreferencesData(hDialog);
      bResult     = FALSE;

      // Examine message
      switch (iMessage)
      {
      /// [COMMAND PROCESSING] - Control notification
      case WM_COMMAND:
         if (onPreferencesFoldersPage_Command(pDialogData, hDialog, LOWORD(wParam), HIWORD(wParam), (HWND)lParam))
            bResult = TRUE;
         break;

      /// [OWNER DRAW] ComboBox
      case WM_MEASUREITEM:
         if (onWindow_MeasureComboBox((MEASUREITEMSTRUCT*)lParam, ITS_SMALL, ITS_MEDIUM))
            bResult = TRUE;
         break;
      }

      // Pass to base handler
      return utilEither(bResult, dlgprocPreferencesPage(hDialog, iMessage, wParam, lParam, PP_FOLDERS));
   }
   /// [EXCEPTION HANDLER]
   __except (generateExceptionError(GetExceptionInformation(), pException))
   {
      // [ERROR] "An unidentified and unexpected critical error has occurred in the preferences %s window"
      enhanceError(pException, ERROR_ID(IDS_EXCEPTION_PREFERENCES_DIALOG), szDebugPageNames[PP_FOLDERS]);
      displayException(pException);
      return 0;
   }
}


/// Function name  : dlgprocPreferencesGeneralPage
// Description     : Window procedure pass-through for the 'General Preferences' property page
//
// 
INT_PTR   dlgprocPreferencesGeneralPage(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   PREFERENCES_DATA*  pDialogData;
   ERROR_STACK*       pException;
   BOOL               bResult;

   __try
   {
      // Get dialog data
      pDialogData = getPreferencesData(hDialog);
      bResult     = FALSE;

      // Examine message
      switch (iMessage)
      {
      case WM_COMMAND:
         if (onPreferencesGeneralPage_Command(pDialogData, hDialog, LOWORD(wParam), HIWORD(wParam), (HWND)lParam))
            bResult = TRUE;
         break;

      /// [DELAY SLIDER]
      case WM_HSCROLL:  
         onPreferencesGeneralPage_AdjustDelay(pDialogData, hDialog, GetWindowID((HWND)lParam), LOWORD(wParam), HIWORD(wParam));
         break;
      }

      // Pass to base
      return utilEither(bResult, dlgprocPreferencesPage(hDialog, iMessage, wParam, lParam, PP_GENERAL));
   }
   /// [EXCEPTION HANDLER]
   __except (generateExceptionError(GetExceptionInformation(), pException))
   {
      // [ERROR] "An unidentified and unexpected critical error has occurred in the preferences %s window"
      enhanceError(pException, ERROR_ID(IDS_EXCEPTION_PREFERENCES_DIALOG), szDebugPageNames[PP_GENERAL]);
      displayException(pException);
      return 0;
   }
}


/// Function name  : dlgprocPreferencesMiscPage
// Description     : Window procedure pass-through for the 'Misc Preferences' property page
//
// 
INT_PTR   dlgprocPreferencesMiscPage(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   // Pass to base
   return dlgprocPreferencesPage(hDialog, iMessage, wParam, lParam, PP_MISC);
}



/// Function name  : dlgprocPreferencesSyntaxPage
// Description     : Window procedure pass-through for the 'Syntax Preferences' property page
//
//
INT_PTR   dlgprocPreferencesSyntaxPage(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   PREFERENCES_DATA*  pDialogData;
   ERROR_STACK*       pException;
   BOOL               bResult;

   __try
   {
      // Prepare
      pDialogData = getPreferencesData(hDialog);
      bResult     = FALSE;

      // Examine result
      switch (iMessage)
      {
      /// [COMMAND] 
      case WM_COMMAND:
         bResult = onPreferencesSyntaxPage_Command(pDialogData, hDialog, LOWORD(wParam), HIWORD(wParam));
         break;

      /// [NOTIFY]
      case WM_NOTIFY:
         bResult = onPreferencesSyntaxPage_Notify(pDialogData, hDialog, (NMHDR*)lParam);
         break;

      // [CUSTOM COMBO]
      case WM_MEASUREITEM:  
         bResult = onWindow_MeasureComboBox((MEASUREITEMSTRUCT*)lParam, ITS_SMALL, ITS_SMALL);  
         break;
      }

      // Return result
      return bResult ? TRUE : dlgprocPreferencesPage(hDialog, iMessage, wParam, lParam, PP_SYNTAX);
   }
   /// [EXCEPTION HANDLER]
   __except (generateExceptionError(GetExceptionInformation(), pException))
   {
      // [ERROR] "An unidentified and unexpected critical error has occurred in the preferences %s window"
      enhanceError(pException, ERROR_ID(IDS_EXCEPTION_PREFERENCES_DIALOG), szDebugPageNames[PP_SYNTAX]);
      displayException(pException);
      return 0;
   }
}


