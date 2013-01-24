//
// Document Property Sheet Base.cpp : Creates and manages the 'document properties' property sheet
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                 CONSTANTS / GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// ListView column counts
#define       ARGUMENT_COLUMN_COUNT            3      // Script Arguments list
#define       DEPENDENCIES_COLUMN_COUNT        2      // Script Dependencies list
#define       VARIABLES_COLUMN_COUNT           3      // Script Variables list
#define       BUTTON_COLUMN_COUNT              2      // Language Buttons list

// Attributes of the available property pages
// 
CONST PROPERTY_PAGE_DEFINITION  oPropertyPages[PROPERTY_PAGE_COUNT] = 
{ 
      dlgprocGeneralPageS,        TEXT("PROPERTIES_GENERAL_SCRIPT"),   TEXT("NEW_SCRIPT_FILE_ICON"),  IDS_SCRIPT_GENERAL_PAGE_TITLE,        //COMMAND_LIST_ICON
      dlgprocArgumentPage,        TEXT("PROPERTIES_ARGUMENTS"),        TEXT("EDIT_FORMATTING_ICON"),  IDS_SCRIPT_ARGUMENTS_PAGE_TITLE,
      dlgprocDependenciesPage,    TEXT("PROPERTIES_DEPENDENCIES"),     TEXT("VARIABLE_ICON"),         IDS_SCRIPT_DEPENDENCIES_PAGE_TITLE,
      dlgprocVariablesPage,       TEXT("PROPERTIES_VARIABLES"),        TEXT("VARIABLE_ICON"),         IDS_SCRIPT_VARIABLES_PAGE_TITLE,
      dlgprocStringsPage,         TEXT("PROPERTIES_STRINGS"),          TEXT("EDIT_PAGE_ICON"),        IDS_SCRIPT_STRINGS_PAGE_TITLE,

      dlgprocGeneralPageL,        TEXT("PROPERTIES_GENERAL_LANGUAGE"), TEXT("NEW_LANGUAGE_FILE_ICON"),IDS_LANGUAGE_GENERAL_PAGE_TITLE,
      dlgprocColumnsPage,         TEXT("PROPERTIES_COLUMNS"),          TEXT("TWO_COLUMN_ICON"),       IDS_LANGUAGE_COLUMNS_PAGE_TITLE,
      dlgprocButtonPage,          TEXT("PROPERTIES_BUTTON"),           TEXT("OUTPUT_WINDOW_ICON"),    IDS_LANGUAGE_BUTTON_PAGE_TITLE,
      dlgprocSpecialPage,         TEXT("PROPERTIES_SPECIAL"),          TEXT("PREFERENCES_ICON"),      IDS_LANGUAGE_SPECIAL_PAGE_TITLE,
      dlgprocPropertiesBlankPage, TEXT("PROPERTIES_NONE"),             TEXT("MAIN_WINDOW"),           IDS_NONE_PAGE_TITLE    
};

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                 CREATION  /  DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createPropertiesDialog
// Description     : Create the 'Document Properties' property sheet
// 
// MAIN_WINDOW_DATA*  pWindowData : [in]  Main window data
///                                     [out] Contains properties data and dialog window handle
// 
// Return Type: Window handle of the dialog if successful, otherwise NULL
//
HWND   createPropertiesDialog(MAIN_WINDOW_DATA*  pWindowData)
{
   PROPSHEETHEADER    oSheetData;         // PropertySheet creation data
   PROPSHEETPAGE      oPageData[5];       // PropertyPage creation data
   PROPERTIES_DATA*   pDialogData;        // Dialog data
   PROPERTY_PAGE      eInitialPage;       // ID of first page to be created
   RECT*              pWindowRect;        // Dialog position, from preferences

   // Prepare
   pDialogData = pWindowData->pPropertiesSheetData;

   // Determine type of pages to display from document type
   pDialogData->eType = identifyPropertiesDialogType(getActiveDocument());

   // Define sheet data
   oSheetData.dwSize      = sizeof(PROPSHEETHEADER);
   oSheetData.hwndParent  = pWindowData->hMainWnd;
   oSheetData.hInstance   = getResourceInstance();

   // Define sheet properties
   oSheetData.dwFlags     = PSH_PROPSHEETPAGE WITH PSH_MODELESS WITH PSH_USECALLBACK;
   oSheetData.pfnCallback = onPropertiesDialog_Create;
   oSheetData.nPages      = identifyPropertiesDialogPageCount(pDialogData->eType);
   oSheetData.ppsp        = oPageData;
   oSheetData.pszCaption  = MAKEINTRESOURCE(IDS_PROPERTIES_SHEET_TITLE);

   // Determine first page + Store document pointer
   switch (pDialogData->eType)
   {
   case PDT_NO_DOCUMENT:        eInitialPage = PP_NO_DOCUMENT;       break;
   case PDT_LANGUAGE_DOCUMENT:  eInitialPage = PP_LANGUAGE_GENERAL;  pDialogData->pLanguageDocument = getActiveLanguageDocument();   break;
   case PDT_SCRIPT_DOCUMENT:    eInitialPage = PP_SCRIPT_GENERAL;    pDialogData->pScriptDocument   = getActiveScriptDocument();     break;
   }

   /// Define initial pages
   for (UINT  iPage = 0; iPage < oSheetData.nPages; iPage++)
      createPropertiesDialogPageData(pDialogData, &oPageData[iPage], (PROPERTY_PAGE)(eInitialPage + iPage));

   /// Create PropertySheet and store handle
   pDialogData->hSheetDlg = (HWND)PropertySheet(&oSheetData);
   ERROR_CHECK("creating document properties PropertySheet", pDialogData->hSheetDlg);

   // Sub-class PropertySheet
   pDialogData->dlgprocSheetBase = SubclassDialog(pDialogData->hSheetDlg, dlgprocPropertiesSheet); 

   // Position according to Preferences
   pWindowRect = getAppPreferencesWindowRect(AW_PROPERTIES);
   SetWindowPos(pDialogData->hSheetDlg, NULL, pWindowRect->left, pWindowRect->top, NULL, NULL, SWP_NOZORDER WITH SWP_NOSIZE);

   // Create and insert replacement ImageList for the TabControl
   HIMAGELIST hTabImageList = TabCtrl_SetImageList(PropSheet_GetTabControl(pDialogData->hSheetDlg), createPropertiesDialogImageList(pDialogData->eType));
   ImageList_Destroy(hTabImageList);

   /// Display dialog
   ShowWindow(pDialogData->hSheetDlg, SW_SHOWNORMAL);
   UpdateWindow(pDialogData->hSheetDlg);
   
   // Return window handle
   return pDialogData->hSheetDlg;
}


/// Function name  : createPropertiesDialogData
// Description     : Create new properties dialog data
// 
// Return Value   : New dialog data, you are responsible for destroying it
// 
PROPERTIES_DATA*  createPropertiesDialogData()
{
   PROPERTIES_DATA*  pDialogData;

   // Create dialog data
   pDialogData = utilCreateEmptyObject(PROPERTIES_DATA);

   // Create column icons ImageList
   pDialogData->hColumnIcons = utilCreateImageList(getResourceInstance(), 48, 3, "ONE_COLUMN_ICON", "TWO_COLUMN_ICON", "THREE_COLUMN_ICON");

   // Return new object
   return pDialogData;
}


/// Function name  : createPropertiesDialogPageData
// Description     : Fill a PropertyPage structure for the desired page
// 
// PROPERTIES_DATA*     pPropertiesData : [in]     Properites dialog data
// PROPSHEETPAGE*       pPageData       : [in/out] PropertyPage structure
// CONST PROPERTY_PAGE  ePage           : [in]     ID of the page to generate structure for
//  
VOID  createPropertiesDialogPageData(PROPERTIES_DATA*  pPropertiesData, PROPSHEETPAGE*  pPageData, CONST PROPERTY_PAGE  ePage)
{
   // Prepare
   utilZeroObject(pPageData, PROPSHEETPAGE);

   // Set common properties
   pPageData->dwSize      = sizeof(PROPSHEETPAGE);
   pPageData->dwFlags     = PSP_USEICONID WITH PSP_USETITLE WITH PSP_PREMATURE;
   pPageData->lParam      = (LPARAM)pPropertiesData;
   pPageData->pfnCallback = NULL;
   pPageData->hInstance   = getResourceInstance();

   // Define page specific properties
   pPageData->pfnDlgProc  = oPropertyPages[ePage].dlgProc;
   pPageData->pszTemplate = oPropertyPages[ePage].szTemplateResource;
   pPageData->pszIcon     = oPropertyPages[ePage].szIconResource;
   pPageData->pszTitle    = MAKEINTRESOURCE(oPropertyPages[ePage].iTitleResource);
}



HIMAGELIST  createPropertiesDialogImageList(PROPERTY_DIALOG_TYPE  eType)
{
   const UINT  iIconSize = 18;
   HIMAGELIST  hImageList;

   switch (eType)
   {
   case PDT_NO_DOCUMENT:       hImageList = utilCreateImageList(getResourceInstance(), iIconSize, 1, "MAIN_WINDOW");  break;
   case PDT_LANGUAGE_DOCUMENT: hImageList = utilCreateImageList(getResourceInstance(), iIconSize, 4, "NEW_LANGUAGE_FILE_ICON", "TWO_COLUMN_ICON", "VARIABLE_ICON", "PREFERENCES_ICON");  break;
   case PDT_SCRIPT_DOCUMENT:   hImageList = utilCreateImageList(getResourceInstance(), iIconSize, 5, "NEW_SCRIPT_FILE_ICON", "ARGUMENT_ICON", "SCRIPT_DEPENDENCY_ICON", "VARIABLE_DEPENDENCY_ICON", "EDIT_PAGE_ICON");  break;
   }

   return hImageList;
}


/// Function name  : deletePropertiesDialogData
// Description     : Destroys properties dialog data
// 
// PROPERTIES_DATA*  &pDialogData : [in] Properties dialog data to be destroyed
// 
VOID  deletePropertiesDialogData(PROPERTIES_DATA*  &pSheetData)
{
   // Delete 'Columns' ImageList
   ImageList_Destroy(pSheetData->hColumnIcons);

   /// Delete Depdenency pages trees
   // Delete Script dependencies tree (if any)
   if (pSheetData->pScriptDependecies)
      deleteAVLTree(pSheetData->pScriptDependecies);

   // Delete String variables tree (if any)
   if (pSheetData->pStringDependencies)
      deleteAVLTree(pSheetData->pStringDependencies);

   // Delete Script variables tree (if any)
   if (pSheetData->pVariableDependencies)
      deleteAVLTree(pSheetData->pVariableDependencies);

   /// Delete calling object
   utilDeleteObject(pSheetData);
}


/// Function name  : destroyPropertiesDialog
// Description     : Destroy the 'properties' dialog but not the dialog data.
// 
// MAIN_WINDOW_DATA*  pWindowData : [in] Main Window data
// 
VOID  destroyPropertiesDialog(MAIN_WINDOW_DATA*  pWindowData)
{
   PROPERTIES_DATA*  pSheetData;    // Convenience pointer

   // Prepare
   pSheetData = pWindowData->pPropertiesSheetData;

   // Un-Sub-class PropertySheet
   SetWindowLong(pSheetData->hSheetDlg, DWL_DLGPROC, (LONG)pSheetData->dlgprocSheetBase);
   pSheetData->dlgprocSheetBase = NULL;

   // Destroy properties dialog
   utilDeleteWindow(pSheetData->hSheetDlg);

   // Zero main window data handle
   pWindowData->hPropertiesSheet = NULL;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : displayPropertiesDialog
// Description     : Shows or hides the properties dialog
// 
// MAIN_WINDOW_DATA*  pWindowData : [in] Main window data
// CONST BOOL         bShow           : [in] TRUE to show, FALSE to hide
// 
VOID  displayPropertiesDialog(MAIN_WINDOW_DATA*  pWindowData, CONST BOOL  bShow)
{
   // [TRACK]
   TRACK_FUNCTION();
   VERBOSE_LIB_COMMAND();

   /// [CHECK] Create dialog if necessary
   if (bShow AND !pWindowData->hPropertiesSheet)
   {
      // [SHOW] Create properties dialog
      pWindowData->hPropertiesSheet = createPropertiesDialog(pWindowData);

      // [MODAL WINDOW] Add window to stack
      /*pushModalWindowStack(pWindowData->hPropertiesSheet);*/
   }
   /// [CHECK] Destroy dialog if necessary
   else if (!bShow AND pWindowData->hPropertiesSheet)
   {
      // [HIDE] Destroy properties dialog
      destroyPropertiesDialog(pWindowData);

      // [MODAL WINDOW] Remove window from stack
      /*popModalWindowStack();*/
   }
   
   // Update toolbar to reflect new state
   updateMainWindowToolBar(pWindowData);
   END_TRACKING();
}


/// Function name  : getPropertiesDialogCurrentPageID
// Description     : Retrieves the ID of the properties page currently being displayed
// 
// PROPERTIES_DATA*  pPropertiesData   : [in] Properties data
// 
// Return Value   : ID of the page currently being displayed, if any, otherwise PP_NO_DOCUMENT
// 
PROPERTY_PAGE  getPropertiesDialogCurrentPageID(PROPERTIES_DATA*  pPropertiesData)
{
   PROPERTY_PAGE  eOutput;       // Operation result
   UINT           iPageIndex;    // Zero-based current page index
   
   // Prepare
   TRACK_FUNCTION();
   eOutput = PP_NO_DOCUMENT;

   // Get current page index
   iPageIndex = SendMessage(PropSheet_GetTabControl(pPropertiesData->hSheetDlg), TCM_GETCURSEL, NULL, NULL);

   // Examine dialog type
   switch (pPropertiesData->eType)
   {
   /// [SCRIPT FILE] 
   case PDT_SCRIPT_DOCUMENT:
      // Examine page index
      switch (iPageIndex)
      {
      case 0:  eOutput = PP_SCRIPT_GENERAL;        break;
      case 1:  eOutput = PP_SCRIPT_ARGUMENTS;      break;
      case 2:  eOutput = PP_SCRIPT_DEPENDENCIES;   break;
      case 3:  eOutput = PP_SCRIPT_VARIABLES;      break;
      }
      break;

   /// [LANGUAGE FILE] 
   case PDT_LANGUAGE_DOCUMENT:
      // Examine page index
      switch (iPageIndex)
      {
      case 0:  eOutput = PP_LANGUAGE_GENERAL;      break;
      case 1:  eOutput = PP_LANGUAGE_COLUMNS;      break;
      case 2:  eOutput = PP_LANGUAGE_BUTTON;       break;
      case 3:  eOutput = PP_LANGUAGE_SPECIAL;      break;
      }
      break;
   }

   // Return page ID
   END_TRACKING();
   return eOutput;
}


/// Function name  : getPropertiesDialogData
// Description     : Get the dialog data associated with the properties dialog
// 
// HWND  hDialog   : [in] Properties dialog page window handle
// 
// Return Value   : Properties dialog data
// 
PROPERTIES_DATA*  getPropertiesDialogData(HWND  hDialog)
{
   return (PROPERTIES_DATA*)GetWindowLong(hDialog, DWL_USER);
}


/// Function name  : identifyPropertiesDialogType
// Description     : Determines which type of properties dialog is required for a given document
// 
// CONST DOCUMENT*  pDocument : [in] Document to query
// 
// Return Value   : Type of pages that should be displayed
// 
PROPERTY_DIALOG_TYPE  identifyPropertiesDialogType(CONST DOCUMENT*  pDocument)
{
   PROPERTY_DIALOG_TYPE  eType;     // Operation result

   // Prepare
   eType = PDT_NO_DOCUMENT;

   // [CHECK] Ensure Document exists
   if (pDocument)
   {
      // Examine document type
      switch (pDocument->eType)
      {
      case DT_SCRIPT:    eType = PDT_SCRIPT_DOCUMENT;     break;
      case DT_LANGUAGE:  eType = PDT_LANGUAGE_DOCUMENT;   break;
      }
   }

   // Return dialog type
   return eType;
}


/// Function name  : identifyPropertiesDialogPageCount
// Description     : Identifies the number of pages each type of dialog requires
// 
// CONST PROPERTY_DIALOG_TYPE  eType : [in] Properties dialog page type
// 
// Return Value   : Number of pages for specified dialog type
// 
UINT  identifyPropertiesDialogPageCount(CONST PROPERTY_DIALOG_TYPE  eType)
{
   UINT  iPageCount;     // Operation result

   // Examine dialog type
   switch (eType)
   {
   case PDT_NO_DOCUMENT:         iPageCount = 1;   break;
   case PDT_LANGUAGE_DOCUMENT:   iPageCount = 4;   break;
   case PDT_SCRIPT_DOCUMENT:     iPageCount = 5;   break;
   }

   // Return page count
   return iPageCount;
}

/// Function name  : isPropertiesDialogAlreadyClosed
// Description     : Checks whether the user has closed the 'properties' dialog manually via the 'X' button
// 
// MAIN_WINDOW_DATA*  pWindowData : [in] Main window data
// 
// Return Value   : TRUE/FALSE
// 
BOOL   isPropertiesDialogAlreadyClosed(MAIN_WINDOW_DATA*  pWindowData)
{
   // Return TRUE if there is no current page handle
   return (PropSheet_GetCurrentPageHwnd(pWindowData->hPropertiesSheet) == NULL);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : initPropertiesDialogPage
// Description     : Initialise a page in the properties dialog
// 
// HWND                 hPage      : [in] Window handle of the page
// CONST PROPERTY_PAGE  ePage      : [in] ID of the page to initialise
// CONST PROPSHEETPAGE* pPageData  : [in] PropertySheet page data
// 
VOID  initPropertiesDialogPage(HWND  hPage, CONST PROPERTY_PAGE  ePage, CONST PROPSHEETPAGE*  pPageData)
{
   PROPERTIES_DATA*   pSheetData;        // Dialog data

   // Extract sheet data
   pSheetData = (PROPERTIES_DATA*)pPageData->lParam;

   // Store sheet data in page
   SetWindowLong(hPage, DWL_USER, (LONG)pSheetData);

   // Create tooltips
   pSheetData->hTooltips[ePage] = initPropertiesDialogPageTooltips(pSheetData, hPage, ePage);

   // Initialise page controls
   initPropertiesDialogPageControls(pSheetData, hPage, ePage);
}


/// Function name  : initPropertiesDialogPageControls
// Description     : Initialises the controls in a properties dialog page
// 
// PROPERTIES_DATA*     pSheetData : [in] Properties dialog data
// HWND                 hPage      : [in] Window handle of the page
// CONST PROPERTY_PAGE  ePage      : [in] ID of the page to initialise
// 
VOID  initPropertiesDialogPageControls(PROPERTIES_DATA*  pSheetData, HWND  hPage, CONST PROPERTY_PAGE  ePage)
{
   LISTVIEW_COLUMNS   oArgumentsListView   = { 3, IDS_ARGUMENTS_COLUMN_NAME,    120, 75,  165,  NULL, NULL };    // ListView column data for the Arguments page     // All widths should sum to 360 pixels
   LISTVIEW_COLUMNS   oDepedenciesListView = { 2, IDS_DEPENDENCIES_COLUMN_NAME, 270, 90,  NULL, NULL, NULL };    // ListView column data for the Dependencies page
   LISTVIEW_COLUMNS   oVariablesListView   = { 3, IDS_VARIABLES_COLUMN_NAME,    240, 60,  60,   NULL, NULL };    // ListView column data for the Variables page
   LISTVIEW_COLUMNS   oStringsListView     = { 3, IDS_STRINGS_COLUMN_PAGE,      60,  60,  240,  NULL, NULL };    // ListView column data for the Strings page
   LISTVIEW_COLUMNS   oButtonsListView     = { 2, IDS_BUTTONS_COLUMN_ID,        160, 200, NULL, NULL, NULL };    // ListView column data for the Buttons page

   // [DEBUG]
   TRACK_FUNCTION();

   // Examine page
   switch (ePage)
   {
   /// [SCRIPT: GENERAL]
   case PP_SCRIPT_GENERAL:
      // Populate Script GameVersion ComboBox
      for (GAME_VERSION  eVersion = GV_THREAT; eVersion <= GV_ALBION_PRELUDE; eVersion = (GAME_VERSION)(eVersion + 1))
         appendCustomComboBoxItemEx(GetControl(hPage, IDC_SCRIPT_ENGINE_VERSION), identifyGameVersionString(eVersion), NULL, identifyGameVersionIconID(eVersion), NULL);
      
      // Set text length limits
      Edit_LimitText(GetControl(hPage, IDC_SCRIPT_NAME), 128);
      Edit_LimitText(GetControl(hPage, IDC_SCRIPT_DESCRIPTION), 128);
      Edit_LimitText(GetControl(hPage, IDC_SCRIPT_COMMAND), 128);
      Edit_LimitText(GetControl(hPage, IDC_SCRIPT_VERSION), 4);

      // Setup Comment Ratio Ctrl
      SendDlgItemMessage(hPage, IDC_SCRIPT_COMMENT_RATIO, PBM_SETRANGE, 0, 100);
      break;

   /// [SCRIPT ARUGMENTS]
   case PP_SCRIPT_ARGUMENTS:     initReportModeListView(GetControl(hPage, IDC_ARGUMENTS_LIST), &oArgumentsListView, TRUE);       break;
   /// [SCRIPT DEPENDENCIES]
   case PP_SCRIPT_DEPENDENCIES:  initReportModeListView(GetControl(hPage, IDC_DEPENDENCIES_LIST), &oDepedenciesListView, TRUE);  break;
   /// [SCRIPT STRINGS]
   case PP_SCRIPT_STRINGS:       initReportModeListView(GetControl(hPage, IDC_STRINGS_LIST), &oStringsListView, TRUE);           break;
   /// [SCRIPT VARIABLES]
   case PP_SCRIPT_VARIABLES:     initReportModeListView(GetControl(hPage, IDC_VARIABLES_LIST), &oVariablesListView, TRUE);       break;

   /// [LANGUAGE: GENERAL]
   case PP_LANGUAGE_GENERAL:
      // Populate String GameVersion ComboBox
      for (GAME_VERSION  eVersion = GV_THREAT; eVersion <= GV_ALBION_PRELUDE; eVersion = (GAME_VERSION)(eVersion + 1))
         appendCustomComboBoxItemEx(GetControl(hPage, IDC_STRING_VERSION_COMBO), identifyGameVersionString(eVersion), NULL, identifyGameVersionIconID(eVersion), NULL);

      // Set edit length limits
      SendDlgItemMessage(hPage, IDC_AUTHOR_EDIT, EM_LIMITTEXT, 128, NULL);
      SendDlgItemMessage(hPage, IDC_TITLE_EDIT,  EM_LIMITTEXT, 256, NULL);
      // Compatibility Combo
      appendCustomComboBoxItemEx(GetControl(hPage, IDC_COMPATIBILITY_COMBO), TEXT("Logbook Only"),                           NULL, TEXT("NEW_LANGUAGE_FILE_ICON"), NULL);
      appendCustomComboBoxItemEx(GetControl(hPage, IDC_COMPATIBILITY_COMBO), TEXT("Logbook, Incoming Message"),              NULL, TEXT("NEW_LANGUAGE_FILE_ICON"), NULL);
      appendCustomComboBoxItemEx(GetControl(hPage, IDC_COMPATIBILITY_COMBO), TEXT("Logbook, Incoming Message, Custom Menu"), NULL, TEXT("NEW_LANGUAGE_FILE_ICON"), NULL);
      break;

   /// [ACTION BUTTONS]
   case PP_LANGUAGE_BUTTON:      
      initReportModeListView(GetControl(hPage, IDC_BUTTONS_LIST), &oButtonsListView, TRUE);   
      break;

   /// [COLUMN ADJUSTMENT]
   case PP_LANGUAGE_COLUMNS:
      // Set slider ranges
      SendDlgItemMessage(hPage, IDC_COLUMN_WIDTH_SLIDER,   TBM_SETRANGE, FALSE, MAKE_LONG(0,500));
      SendDlgItemMessage(hPage, IDC_COLUMN_SPACING_SLIDER, TBM_SETRANGE, FALSE, MAKE_LONG(0,250));
      // Set tick frequencies
      SendDlgItemMessage(hPage, IDC_COLUMN_WIDTH_SLIDER,   TBM_SETTICFREQ, 25, NULL);
      SendDlgItemMessage(hPage, IDC_COLUMN_SPACING_SLIDER, TBM_SETTICFREQ, 25, NULL);
      // Set line sizes
      SendDlgItemMessage(hPage, IDC_COLUMN_WIDTH_SLIDER,   TBM_SETLINESIZE, NULL, 25);
      SendDlgItemMessage(hPage, IDC_COLUMN_WIDTH_SLIDER,   TBM_SETPAGESIZE, NULL, 25);
      // Set page sizes
      SendDlgItemMessage(hPage, IDC_COLUMN_SPACING_SLIDER, TBM_SETLINESIZE, NULL, 25);
      SendDlgItemMessage(hPage, IDC_COLUMN_SPACING_SLIDER, TBM_SETPAGESIZE, NULL, 25);
      break;

   /// [LEGACY STUFF]
   case PP_LANGUAGE_SPECIAL:
      // Rank Type Combo
      SendDlgItemMessage(hPage, IDC_RANK_TYPE_COMBO, CB_ADDSTRING, NULL, (LPARAM)TEXT("Fight"));
      SendDlgItemMessage(hPage, IDC_RANK_TYPE_COMBO, CB_ADDSTRING, NULL, (LPARAM)TEXT("Trade"));
      // Rank Title Combo
      SendDlgItemMessage(hPage, IDC_RANK_TITLE_EDIT, EM_LIMITTEXT, 64, NULL);
      break;
   }

   // Cleanup
   END_TRACKING();
}


/// Function name  : initPropertiesDialogPageTooltips
// Description     : Creates a tooltip window for a page and adds tips for each control
// 
// PROPERTIES_DATA*     pSheetData  : [in] Propeties dialog data
// HWND                 hPage       : [in] Window handle of the page to create tooltips for
// CONST PROPERTY_PAGE  ePage       : [in] ID fo the page to create tooltips for
// 
// Return Value   : Window handle of the new Tooltip
// 
HWND  initPropertiesDialogPageTooltips(PROPERTIES_DATA*  pSheetData, HWND  hPage, CONST PROPERTY_PAGE  ePage)
{
   HWND  hTooltip;      // Newly created tooltip

   // Create tooltip
   hTooltip = createTooltipWindow(hPage);
   SendMessage(hTooltip, TTM_SETDELAYTIME, TTDT_AUTOPOP, 20000);

   // Examine page
   switch (ePage)
   {
   /// [GENERAL PROPERTIES]
   case PP_SCRIPT_GENERAL:
      addTooltipTextToControl(hTooltip, hPage, IDC_SCRIPT_NAME);
      addTooltipTextToControl(hTooltip, hPage, IDC_SCRIPT_SIGNATURE);
      addTooltipTextToControl(hTooltip, hPage, IDC_SCRIPT_DESCRIPTION);
      addTooltipTextToControl(hTooltip, hPage, IDC_SCRIPT_COMMAND);
      addTooltipTextToControl(hTooltip, hPage, IDC_SCRIPT_VERSION);
      addTooltipTextToControl(hTooltip, hPage, IDC_SCRIPT_ENGINE_VERSION);
      addTooltipTextToControl(hTooltip, hPage, IDC_SCRIPT_COMMENT_RATIO);
      addTooltipTextToControl(hTooltip, hPage, IDC_SCRIPT_FOLDER);
      break;

   /// [SCRIPT ARGUMENTS]
   case PP_SCRIPT_ARGUMENTS:
      addTooltipTextToControl(hTooltip, hPage, IDC_ARGUMENTS_LIST);
      addTooltipTextToControl(hTooltip, hPage, IDC_ADD_ARGUMENT);
      addTooltipTextToControl(hTooltip, hPage, IDC_REMOVE_ARGUMENT);
      break;

   /// [SCRIPT DEPENDENCIES]
   case PP_SCRIPT_DEPENDENCIES:
      addTooltipTextToControl(hTooltip, hPage, IDC_DEPENDENCIES_LIST);
      addTooltipTextToControl(hTooltip, hPage, IDC_REFRESH_DEPENDENCIES);
      addTooltipTextToControl(hTooltip, hPage, IDC_SEARCH_DEPENDENCIES);
      break;

   /// [SCRIPT VARIABLES]
   case PP_SCRIPT_VARIABLES:
      addTooltipTextToControl(hTooltip, hPage, IDC_VARIABLES_LIST);
      addTooltipTextToControl(hTooltip, hPage, IDC_REFRESH_VARIABLES);
      addTooltipTextToControl(hTooltip, hPage, IDC_PROJECT_VARIABLES);
      break;

   /// [SCRIPT STRINGS]
   case PP_SCRIPT_STRINGS:
      addTooltipTextToControl(hTooltip, hPage, IDC_STRINGS_LIST);
      addTooltipTextToControl(hTooltip, hPage, IDC_REFRESH_STRINGS);
      break;


   /// [GENERAL PROPERTIES]
   case PP_LANGUAGE_GENERAL:
      addTooltipTextToControl(hTooltip, hPage, IDC_AUTHOR_EDIT);
      addTooltipTextToControl(hTooltip, hPage, IDC_TITLE_EDIT);
      addTooltipTextToControl(hTooltip, hPage, IDC_COMPATIBILITY_COMBO);
      break;

   /// [ACTION BUTTONS]
   case PP_LANGUAGE_BUTTON:
      addTooltipTextToControl(hTooltip, hPage, IDC_BUTTONS_LIST);
      break;

   /// [COLUMN ADJUSTMENT]
   case PP_LANGUAGE_COLUMNS:
      addTooltipTextToControl(hTooltip, hPage, IDC_COLUMN_ONE_RADIO);
      addTooltipTextToControl(hTooltip, hPage, IDC_COLUMN_TWO_RADIO);
      addTooltipTextToControl(hTooltip, hPage, IDC_COLUMN_THREE_RADIO);
      addTooltipTextToControl(hTooltip, hPage, IDC_COLUMN_WIDTH_SLIDER);
      addTooltipTextToControl(hTooltip, hPage, IDC_COLUMN_WIDTH_CHECK);
      addTooltipTextToControl(hTooltip, hPage, IDC_COLUMN_SPACING_SLIDER);
      addTooltipTextToControl(hTooltip, hPage, IDC_COLUMN_SPACING_CHECK);
      break;

   /// [LEGACY STUFF]
   case PP_LANGUAGE_SPECIAL:
      addTooltipTextToControl(hTooltip, hPage, IDC_RANK_TYPE_COMBO);      
      addTooltipTextToControl(hTooltip, hPage, IDC_RANK_TITLE_EDIT);
      addTooltipTextToControl(hTooltip, hPage, IDC_RANK_CHECK);
      addTooltipTextToControl(hTooltip, hPage, IDC_ARTICLE_CHECK);
      break;
   }

   // Return new Tooltip
   return hTooltip;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onPropertiesDialog_Activate
// Description     : Enables/Disables window transparency
// 
// HWND        hSheet  : [in] Properties sheet
// CONST UINT  iFlags  : [in] Activation source: WA_ACTIVE, WA_MOUSEACTIVE, WA_INACTIVE
// 
VOID  onPropertiesDialog_Activate(HWND  hSheet, CONST UINT  iFlags)
{
   // [CHECK] Is transparent dialog preference set?
   if (getAppPreferences()->bTransparentProperties)
      /// [SUCCESS] Set window to transparent when it loses focus
      SetLayeredWindowAttributes(hSheet, NULL, (iFlags == WA_INACTIVE ? 100 : 255), LWA_ALPHA);
}


/// Function name  : onPropertiesDialog_Create
// Description     : Removes the buttons on the properties dialog and resizes the dialog to fit
// 
// HWND   hDialog  : [in] Window handle of property sheet
// UINT   iMessage : [in] Callback message
// LPARAM lParam   : [in] Callback data
// 
// Return type : NULL
//
INT    onPropertiesDialog_Create(HWND  hSheet, UINT  iMessage, LPARAM  lParam)
{
   //PROPERTIES_DATA*  pDialogData;
   DLGTEMPLATE*      pTemplate;        // Dialog template for the property sheet
   HWND          hTabCtrl,         // Tab control window handle
                 hButtonCtrl;      // Button control window handle
   RECT          rcTabCtrl,        // Tab control client rectangle
                 rcSheet;          // Sheet client rectangle
   SIZE          siTabCtrl;        // Size of tab control client rectangle
   //HIMAGELIST    hTabImageList;    // Existing imagelist of the tab control            

   // [DEBUG]
   TRACK_FUNCTION();

   // Examine state
   switch (iMessage)
   {
   /// [PRE-CREATE] Remove the 'visible' style from the property sheet template
   case PSCB_PRECREATE:
      // Prepare
      pTemplate = (DLGTEMPLATE*)lParam;

      // Make sheet initially invisible
      pTemplate->style ^= WS_VISIBLE;

      // Make sheet layered
      pTemplate->dwExtendedStyle |= WS_EX_LAYERED;
      break;

   /// [INITIALISE] Modify the controls in the property sheet
   case PSCB_INITIALIZED:
      // Prepare
      hButtonCtrl = NULL;
      hTabCtrl    = PropSheet_GetTabControl(hSheet);
      
      // Convert to sheet to 'Tool Window' and prevent TabCtrl from using multiple lines
      utilAddWindowStyleEx(hSheet, WS_EX_TOOLWINDOW);
      utilRemoveWindowStyle(hTabCtrl, TCS_MULTILINE);

      /// Hide and disable 'OK', 'CANCEL' and 'APPLY'
      while (hButtonCtrl = FindWindowEx(hSheet, hButtonCtrl, TEXT("BUTTON"), NULL))
      {
         ShowWindow(hButtonCtrl, SW_HIDE);
         EnableWindow(hButtonCtrl, FALSE);
      }

      // Calculate new position of tab control
      GetClientRect(hTabCtrl, &rcTabCtrl);
      OffsetRect(&rcTabCtrl, GetSystemMetrics(SM_CXDLGFRAME), GetSystemMetrics(SM_CYDLGFRAME));

      // [HACK] Reduce the height of the tab control slightly - I'm not sure why it's too high, but there's a noticeable border on Windows 7
      rcTabCtrl.bottom -= 4 * GetSystemMetrics(SM_CYDLGFRAME);

      // Calculate new size of sheet from tab control
      utilConvertRectangleToSize(&rcTabCtrl, &siTabCtrl);
      SetRect(&rcSheet, 0, 0, siTabCtrl.cx + (4 * GetSystemMetrics(SM_CXDLGFRAME)), siTabCtrl.cy + (4 * GetSystemMetrics(SM_CYDLGFRAME)) + GetSystemMetrics(SM_CYSMCAPTION));

      /// Resize sheet and reposition tab control
      utilSetClientRect(hSheet, &rcSheet, FALSE);
      utilSetClientRect(hTabCtrl, &rcTabCtrl, FALSE);

      // Turn transparency 'Off' initially
      SetLayeredWindowAttributes(hSheet, NULL, 0xFF, LWA_ALPHA);

      // Create and insert replacement ImageList for the TabControl
      /*hTabImageList = TabCtrl_SetImageList(hTabCtrl, createPropertiesDialogImageList(pDialogData->eType));
      ImageList_Destroy(hTabImageList);*/
      break;
   }

   END_TRACKING();
   return 0;
}


/// Function name  : onPropertiesDialog_DocumentSwitched
// Description     : Updates the dialog to display appropriate pages for a new document
// 
// PROPERTIES_DATA*  pSheetData   : [in] Window data for the property sheet
// DOCUMENT*         pNewDocument : [in] New document to display pages for
// 
VOID    onPropertiesDialog_DocumentSwitched(PROPERTIES_DATA*  pSheetData, DOCUMENT*  pNewDocument)
{
   PROPSHEETPAGE    oNewPageData[5];       // New page data
   HPROPSHEETPAGE   hDummyPage,            // Handle to dummy page added during switch over
                    hNewPage;              // Handle of the page currently being created
   PROPERTY_PAGE    eInitialPage;          // ID of the first new page to be added
   UINT             iOriginalTabIndex;     // Index of the currently selected tab        

   // [DEBUG]
   TRACK_FUNCTION();

   // [CHECK] Is the new document the same type as the old?
   if (pSheetData->eType == identifyPropertiesDialogType(pNewDocument))
   {
      /// [SAME TYPE] Update document pointer 
      pSheetData->pScriptDocument   = (pSheetData->pScriptDocument ? (SCRIPT_DOCUMENT*)pNewDocument : NULL);
      pSheetData->pLanguageDocument = (pSheetData->pLanguageDocument ? (LANGUAGE_DOCUMENT*)pNewDocument : NULL);

      // Refresh all page values manually
      PropSheet_QuerySiblings(pSheetData->hSheetDlg, UN_DOCUMENT_UPDATED, NULL);
   }
   /// [DIFFERENT] Change pages
   else
   {
      // Prepare
      SetWindowRedraw(pSheetData->hSheetDlg, FALSE);
      iOriginalTabIndex = TabCtrl_GetCurSel(PropSheet_GetTabControl(pSheetData->hSheetDlg));

      /// Create a blank page as a dummy  (Prevents dialog being destroyed when all pages are removed)
      createPropertiesDialogPageData(pSheetData, &oNewPageData[0], PP_NO_DOCUMENT);
      hDummyPage = CreatePropertySheetPage(&oNewPageData[0]);

      // Insert and select page
      PropSheet_AddPage(pSheetData->hSheetDlg, hDummyPage);
      PropSheet_SetCurSel(pSheetData->hSheetDlg, hDummyPage, NULL);

      /// Remove current pages
      for (UINT  iPage = 0; iPage < identifyPropertiesDialogPageCount(pSheetData->eType); iPage++)
         PropSheet_RemovePage(pSheetData->hSheetDlg, 0, NULL);  // Remove first page each time

      // Reset documents
      pSheetData->pScriptDocument   = NULL;
      pSheetData->pLanguageDocument = NULL;

      // Identify document type + first page
      switch (pSheetData->eType = identifyPropertiesDialogType(pNewDocument))
      {
      case PDT_NO_DOCUMENT:         eInitialPage = PP_NO_DOCUMENT;       break;
      case PDT_LANGUAGE_DOCUMENT:   eInitialPage = PP_LANGUAGE_GENERAL;  pSheetData->pLanguageDocument = (LANGUAGE_DOCUMENT*)pNewDocument;   break;
      case PDT_SCRIPT_DOCUMENT:     eInitialPage = PP_SCRIPT_GENERAL;    pSheetData->pScriptDocument   = (SCRIPT_DOCUMENT*)pNewDocument;     break;
      }

      /// Create and Insert new pages
      for (UINT  iPage = 0; iPage < identifyPropertiesDialogPageCount(pSheetData->eType); iPage++)
      {
         // Create new page
         createPropertiesDialogPageData(pSheetData, &oNewPageData[iPage], (PROPERTY_PAGE)(eInitialPage + iPage));
         hNewPage = CreatePropertySheetPage(&oNewPageData[iPage]);
         // Insert but not display
         PropSheet_AddPage(pSheetData->hSheetDlg, hNewPage);   // Appends page to end
      }

      /// Remove dummy page + select original tab
      PropSheet_RemovePage(pSheetData->hSheetDlg, NULL, hDummyPage);
      PropSheet_SetCurSel(pSheetData->hSheetDlg, NULL, iOriginalTabIndex);

      // Redraw window
      SetWindowRedraw(pSheetData->hSheetDlg, TRUE);
      utilRedrawWindow(pSheetData->hSheetDlg);
   }

   // [DEBUG]
   END_TRACKING();
}


/// Function name  : onPropertiesDialog_Help
// Description     : Launch the appropriate help page for the current page
// 
// PROPERTIES_DATA*        pSheetData : [in] Sheet data
// CONST PREFERENCES_PAGE  ePage      : [in] Page ID
// CONST HELPINFO*         pRequest   : [in] System WM_HELP data
// 
VOID  onPropertiesDialog_Help(PROPERTIES_DATA*  pSheetData, PROPERTY_PAGE  ePage, CONST HELPINFO*  pRequest)
{
   switch (ePage)
   {
   case PP_SCRIPT_GENERAL:       displayHelp(TEXT("Properties_Script"));        break;
   case PP_SCRIPT_ARGUMENTS:     displayHelp(TEXT("Properties_Arguments"));     break;
   case PP_SCRIPT_DEPENDENCIES:  displayHelp(TEXT("Properties_Dependencies"));  break;
   case PP_SCRIPT_VARIABLES:     displayHelp(TEXT("Properties_Variables"));     break;
   case PP_SCRIPT_STRINGS:       displayHelp(TEXT("Properties_Strings"));       break;
   }
}


/// Function name  : onPropertiesDialog_Notify
// Description     : Notification handler for the properties dialog
// 
// PROPERTIES_DATA*  pSheetData     : [in] Properties sheet data
// HWND              hPage          : [in] Page window handle
// PROPERTY_PAGE     ePage          : [in] ID of the page
// NMHDR*            pMessageHeader : [in] Notify message header
// 
// Return Value   : TRUE if handled, otherwise FALSE
// 
BOOL  onPropertiesDialog_Notify(PROPERTIES_DATA*  pSheetData, HWND  hPage, PROPERTY_PAGE  ePage, NMHDR*  pMessageHeader)
{
   BOOL   bResult;

   // Preare
   bResult = TRUE;

   // Examine notification
   switch (pMessageHeader->code)
   {
   /// [PRE-DISPLAY] -- Update page to reflect current document state
   case PSN_SETACTIVE:
      onPropertiesDialog_PageShow(pSheetData, hPage, ePage);
      SetWindowLong(hPage, DWL_MSGRESULT, FALSE);
      break;

   /// [SHEET CLOSED / PAGE SWITCHED] -- Allow closure
   case PSN_QUERYCANCEL:
   case PSN_KILLACTIVE:
      //onPropertiesDialog_PageHide(pSheetData, hPage, ePage);
      SetWindowLong(hPage, DWL_MSGRESULT, FALSE);
      break;

   /// [TOOLTIPS] -- Provide text
   case TTN_GETDISPINFO:
      onTooltipRequestText((NMTTDISPINFO*)pMessageHeader);
      return TRUE;

   // [UNHANDLED] Return FALSE
   default:
      bResult = FALSE;
      break;
   }

   // Return result
   return bResult;
}


/// Function name  : onPropertiesDialog_PageHide
// Description     : Since updates are performed immediately, no action is necessary
// 
// PROPERTIES_DATA*     pSheetData  : [in] Properties dialog data
// HWND                 hPage       : [in] Window handle of the page to hide
// CONST PROPERTY_PAGE  ePage       : [in] ID of the page to hide
// 
VOID  onPropertiesDialog_PageHide(PROPERTIES_DATA*  pSheetData, HWND  hPage, CONST PROPERTY_PAGE  ePage)
{
   
}


/// Function name  : onPropertiesDialog_PageShow
// Description     : Displays the values for a given properties dialog page
// 
// PROPERTIES_DATA*     pSheetData  : [in] Properties dialog data
// HWND                 hPage       : [in] Window handle of the page to display
// CONST PROPERTY_PAGE  ePage       : [in] ID of the page to display
// 
VOID  onPropertiesDialog_PageShow(PROPERTIES_DATA*  pSheetData, HWND  hPage, CONST PROPERTY_PAGE  ePage)
{
   // [DEBUG]
   TRACK_FUNCTION();
   pSheetData->bRefreshing = TRUE;

   // Examine page
   switch (ePage)
   {
   /// [SCRIPT PAGES]
   case PP_SCRIPT_GENERAL:
   case PP_SCRIPT_ARGUMENTS:
   case PP_SCRIPT_DEPENDENCIES:
   case PP_SCRIPT_VARIABLES:
   case PP_SCRIPT_STRINGS:
      onScriptPage_Show(pSheetData, hPage, ePage);
      break;

   /// [LANGUAGE PAGES]
   case PP_LANGUAGE_GENERAL:
   case PP_LANGUAGE_BUTTON:
   case PP_LANGUAGE_COLUMNS:
   case PP_LANGUAGE_SPECIAL:
      onLanguagePage_Show(pSheetData, hPage, ePage);
      break;
   }

   // Cleanup
   pSheetData->bRefreshing = FALSE;
   END_TRACKING();
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       WINDOW PROCEDURES
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : dlgprocPropertiesBlankPage
// Description     : Dialog procedure for 'no document' page
//
// 
INT_PTR  dlgprocPropertiesBlankPage(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   return dlgprocPropertiesPage(hDialog, iMessage, wParam, lParam, PP_NO_DOCUMENT);
}


/// Function name  : dlgprocPropertiesPage
// Description     : 'Base' dialog procedure for all 'properties dialog' pages, implements common functionality.
// 
// HWND             hPage    : [in] Window handle of a property page
//                   ...
// PROPERTY_PAGE    ePage    : [in] ID of the property page
// 
// Return Value   : TRUE if processed, FALSE otherwise
// 
INT_PTR CALLBACK  dlgprocPropertiesPage(HWND  hPage, UINT  iMessage, WPARAM  wParam, LPARAM  lParam, PROPERTY_PAGE  ePage)
{
   PROPERTIES_DATA*   pSheetData;        // Dialog data
   BOOL               bResult;

   // Get dialog data
   TRACK_FUNCTION();
   pSheetData = getPropertiesDialogData(hPage);
   bResult    = TRUE;

   // Examine message
   switch (iMessage)
   {
   /// [CREATION] - Store dialog data, initialise page
   case WM_INITDIALOG:
      initPropertiesDialogPage(hPage, ePage, (PROPSHEETPAGE*)lParam);
      break;

   /// [DESTRUCTION] - Destroy page tooltip
   case WM_DESTROY:
      utilDeleteWindow(pSheetData->hTooltips[ePage]);
      break;

   /// [DOCUMENT UPDATED] - Re-display current page values
   case PSM_QUERYSIBLINGS:
      // Examine message
      switch (wParam)
      {
      // [DOCUMENT UPDATED] Refresh current page values
      case UN_DOCUMENT_UPDATED:
         onPropertiesDialog_PageShow(pSheetData, hPage, ePage);
         break;

      default:
         bResult = FALSE;
         break;
      }
      break;

   /// [NOTIFICATION]
   case WM_NOTIFY:
      bResult = onPropertiesDialog_Notify(pSheetData, hPage, ePage, (NMHDR*)lParam);
      break;

   /// [MENU ITEM HOVER] Forward messages up the chain to the Main window
   case WM_MENUSELECT:
      sendAppMessage(AW_MAIN, WM_MENUSELECT, wParam, lParam);
      break;

   /// [HELP] Display helpfile
   case WM_HELP:
      onPropertiesDialog_Help(pSheetData, ePage, (HELPINFO*)lParam);
      break;

   /// [CUSTOM MENU/CUSTOM COMBO]
   case WM_DRAWITEM:    bResult = onWindow_DrawItem((DRAWITEMSTRUCT*)lParam);               break;
   case WM_MEASUREITEM: bResult = onWindow_MeasureItem(hPage, (MEASUREITEMSTRUCT*)lParam);  break;
   case WM_DELETEITEM:  bResult = onWindow_DeleteItem((DELETEITEMSTRUCT*)lParam);           break;

   /// [UNHANDLED] Return FALSE
   default:
      bResult = FALSE;
      break;
   }

   // Return result
   END_TRACKING();
   return bResult;
}


/// Function name  : dlgprocPropertiesSheet
// Description     : Dialog procedure for the sub-classed property sheet
// 
// HWND    hSheet  : [in] Property Sheet window handle
// 
INT_PTR   dlgprocPropertiesSheet(HWND  hSheet, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   PROPERTIES_DATA*  pSheetData;    // Properties sheet dialog data
   ERROR_STACK*      pException;    // Exception error
   BOOL              bPassToBase;

   // Prepare
   TRACK_FUNCTION();
   pSheetData  = getMainWindowData()->pPropertiesSheetData;
   bPassToBase = TRUE;

   /// [GUARD BLOCK]
   __try
   {
      // Examine message
      switch (iMessage)
      {
      /// [DOCUMENT UPDATED] -- Inform each page it should refresh it's values
      case UN_DOCUMENT_UPDATED:
         PropSheet_QuerySiblings(hSheet, UN_DOCUMENT_UPDATED, NULL);
         bPassToBase = FALSE;
         break;

      /// [DOCUMENT SWITCHED] -- Change the pages if necessary and refresh all page values
      case UN_DOCUMENT_SWITCHED:
         onPropertiesDialog_DocumentSwitched(pSheetData, (DOCUMENT*)lParam);
         bPassToBase = FALSE;
         break;

      /// [SCRIPT-CALL OPERATION COMPLETE] Inform dependencies page
      case UN_SCRIPTCALL_OPERATION_COMPLETE:
         onDependenciesPage_OperationComplete(pSheetData, (AVL_TREE*)lParam);
         bPassToBase = FALSE;
         break;

      /// [PREFERENCES CHANGED] Adjust transparency
      case UN_PREFERENCES_CHANGED:
         // Set window to transparent if preference set and dialog inactive, otherwise opaque
         SetLayeredWindowAttributes(hSheet, NULL, getAppPreferences()->bTransparentProperties AND hSheet != GetActiveWindow() ? 100 : 255, LWA_ALPHA);
         break;            

      // [ENABLED]
      case WM_ENABLE:
         // [CHECK] Ensure main window is in the same state
         if (IsWindowEnabled(getAppWindow()) != wParam)
            EnableWindow(getAppWindow(), wParam);

         // [CHECK] Ensure FindText dialog is in the same state
         if (getMainWindowData()->hFindTextDlg AND IsWindowEnabled(getMainWindowData()->hFindTextDlg) != wParam)
            EnableWindow(getMainWindowData()->hFindTextDlg, wParam);
         break;

      // [ACTIVATE] Set transparency
      case WM_ACTIVATE:
         onPropertiesDialog_Activate(hSheet, LOWORD(wParam));
         break;

      // [MOVE DIALOG] Save new position to preferences
      case WM_MOVE:
         GetWindowRect(hSheet, getAppPreferencesWindowRect(AW_PROPERTIES));
         break;
      }
   }
   /// [EXCEPTION HANDLER]
   __except (generateExceptionError(GetExceptionInformation(), pException))
   {
      // [ERROR] "An unidentified and unexpected critical error has occurred in the document properties window"
      enhanceError(pException, ERROR_ID(IDS_EXCEPTION_PROPERTIES_DIALOG));
      displayException(pException);
   }

   // Return result
   END_TRACKING();
   return (!bPassToBase OR !pSheetData ? TRUE : CallDialogProc(pSheetData->dlgprocSheetBase, hSheet, iMessage, wParam, lParam));
}
