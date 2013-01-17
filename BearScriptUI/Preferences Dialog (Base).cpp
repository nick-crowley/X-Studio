//
// Preferences Property Sheet Base.cpp : Creation and management of pages in application preferences dialog
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                 CONSTANTS / GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// Define PropertyPage creation attributes
CONST PROPERTY_PAGE_DEFINITION  oPreferencesPages[PREFERENCES_PAGES] = 
{ 
   dlgprocPreferencesGeneralPage,    TEXT("PREFERENCES_GENERAL"),    TEXT("PREFERENCES_ICON"),   IDS_GENERAL_PAGE_TITLE,
   dlgprocPreferencesFoldersPage,    TEXT("PREFERENCES_FOLDERS"),    TEXT("FOLDERS_ICON"),       IDS_FOLDERS_PAGE_TITLE,
   dlgprocPreferencesAppearancePage, TEXT("PREFERENCES_APPEARANCE"), TEXT("APPEARANCE_ICON"),    IDS_APPEARANCE_PAGE_TITLE,
   dlgprocPreferencesSyntaxPage,     TEXT("PREFERENCES_SYNTAX"),     TEXT("SYNTAX_ICON"),        IDS_SYNTAX_PAGE_TITLE,
   dlgprocPreferencesMiscPage,       TEXT("PREFERENCES_MISC"),       TEXT("SUBMIT_REPORT_ICON"), IDS_MISC_PAGE_TITLE   
};

// Page titles for console printouts
CONST TCHAR*  szDebugPageNames[PREFERENCES_PAGES] =
{
   TEXT("General"),
   TEXT("Game Data"),
   TEXT("Appearance"),
   TEXT("Highlighting"),
   TEXT("Internet")
};

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                 CREATION  /  DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createPreferencesData
// Description     : Create the Preferences PropertySheet dialog data (ImageLists and colour schemes)
// 
// CONST PREFERENCES*  pAppPreferences   : [in] Application Preferences
// 
// Return Value   : New Preferences PropertySheet dialog data object, you are responsible for destroying it
// 
PREFERENCES_DATA*  createPreferencesData(CONST PREFERENCES*  pAppPreferences)
{
   PREFERENCES_DATA*  pDialogData;       // New dialog data
   PROPSHEETPAGE*     pPage;             // Convenience pointer
   PROPSHEETHEADER*   pHeader;           // Convenience pointer

   // Create new data
   pDialogData = utilCreateEmptyObject(PREFERENCES_DATA);

   // Iterate through all pages
   for (UINT iPage = 0; iPage < PREFERENCES_PAGES; iPage++)
   {
      // Prepare
      pPage = &pDialogData->oPage[iPage];

      /// Define property page
      pPage->dwSize      = sizeof(PROPSHEETPAGE);
      pPage->dwFlags     = PSP_USEICONID WITH PSP_USETITLE WITH PSP_PREMATURE WITH PSP_USECALLBACK;
      pPage->hInstance   = getResourceInstance();
      pPage->pszIcon     = oPreferencesPages[iPage].szIconResource;
      pPage->pszTemplate = oPreferencesPages[iPage].szTemplateResource;
      pPage->pfnDlgProc  = oPreferencesPages[iPage].dlgProc;
      pPage->pszTitle    = MAKEINTRESOURCE(oPreferencesPages[iPage].iTitleResource);
      pPage->lParam      = (LPARAM)pDialogData;      // Pass sheet data to each page as parameter
      pPage->pfnCallback = callbackPreferencesPage;
   }

   // Prepare
   pHeader = &pDialogData->oHeader;

   /// Define sheet
   pHeader->dwSize      = sizeof(PROPSHEETHEADER);
   pHeader->dwFlags     = PSH_PROPSHEETPAGE WITH PSH_USECALLBACK;
   pHeader->hInstance   = getResourceInstance();
   pHeader->pszIcon     = NULL;
   pHeader->pszCaption  = MAKEINTRESOURCE(IDS_PREFERENCES_SHEET_TITLE);
   pHeader->nPages      = PREFERENCES_PAGES;
   pHeader->ppsp        = pDialogData->oPage;
   pHeader->pfnCallback = callbackPreferencesDialog;
   
   /// Copy working copy of the input preferences
   pDialogData->pPreferencesCopy = duplicatePreferences(pAppPreferences);

   /// Create colour scheme list
   pDialogData->pColourSchemeList = generateColourSchemeListFromRegistry();

   // Return new data
   return pDialogData;
}


/// Function name  : createPreferencesTooltips
// Description     : Create the tooltips for a specified 'Preferences' property-sheet page
// 
// HWND                    hDialog  : [in] Preferences property page window handle
// CONST PREFERENCES_PAGE  ePage    : [in] The ID of the page
// 
// Return type : New Tooltip window handle
//
HWND   createPreferencesTooltips(HWND  hDialog, CONST PREFERENCES_PAGE  ePage)
{
   HWND  hTooltip;            // New Tooltip
   UINT  iFirstControlID,     // ID of the first control in the group of controls to create tooltips for
         iLastControlID;      // ID of the last control 

   // Create tooltip with 5 second delay
   hTooltip = createTooltipWindow(hDialog);
   SendMessage(hTooltip, TTM_SETDELAYTIME, TTDT_AUTOPOP, 4000);

   // Define control IDs for the page
   switch (ePage)
   {
   // [GENERAL]
   case PP_GENERAL:
      iFirstControlID = IDC_APPLICATION_LANGUAGE_COMBO;
      iLastControlID  = IDC_SUGGEST_VARIABLES_CHECK;
      break;

   // [FOLDERS]
   case PP_FOLDERS:
      iFirstControlID = IDC_GAME_FOLDER_COMBO;
      iLastControlID  = IDC_REPORT_INCOMPLETE_GAMEOBJECTS_CHECK;
      break;

   // [APPEARANCE]
   case PP_APPEARANCE:
      iFirstControlID = IDC_FONT_NAME_COMBO;
      iLastControlID  = IDC_COLOUR_SCHEME_COMBO;
      break;

   // [SYNTAX]
   case PP_SYNTAX:
      iFirstControlID = IDC_CODE_OBJECT_LIST;
      iLastControlID  = IDC_PREVIEW_CODE_EDIT;
      break;

   // [MISC]
   case PP_MISC:
      iFirstControlID = IDC_ERROR_HANDLING_COMBO;
      iLastControlID  = IDC_FORUM_USERNAME_EDIT;
      break;
   }

   // Add appropriate control ID tips
   for (UINT iControl = iFirstControlID; iControl <= iLastControlID; iControl++)
      addTooltipTextToControl(hTooltip, hDialog, iControl);

   // Return tooltip handle
   return hTooltip;
}


/// Function name  : deletePreferencesData
// Description     : Delete preferences property sheet dialog data
// 
// PREFERENCES_DATA*  pDialogData   : [in] Preferences property sheet dialog data
// 
VOID  deletePreferencesData(PREFERENCES_DATA*  pDialogData)
{
   /// Destroy ColourScheme lists
   deleteList(pDialogData->pColourSchemeList);

   /// Delete ScriptFile
   if (pDialogData->pScriptFile)
      deleteScriptFile(pDialogData->pScriptFile);

   /// Delete Preferences Copy Object
   deletePreferences(pDialogData->pPreferencesCopy);

   /// Do not delete Tooltips, they're deleted by the preferences page that uses them
   
   // Delete calling object
   utilDeleteObject(pDialogData);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : getPreferencesData
// Description     : Get the dialog data associated with the preferences PropertySheet
// 
// HWND  hDialog   : [in] Preferences dialog window handle
// 
// Return Value   : Preferences PropertySheet dialog data
// 
PREFERENCES_DATA*  getPreferencesData(HWND  hDialog)
{
   return (PREFERENCES_DATA*)GetWindowLong(hDialog, DWL_USER);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : performPreferencesPageBrowseFolder
// Description     : Ask the user to select a new game/mod/script folder
// 
// PREFERENCES*  pPreferencesCopy    : [in] Copy of application preferences values
// HWND          hDialog             : [in] Property page window handle
// CONST UINT    iDestinationControl : [in] ID of the edit control the new folder should be displayed in
// 
VOID   performPreferencesFolderBrowse(PREFERENCES_DATA*  pDialogData, HWND  hDialog, CONST UINT  iDestinationControl)
{
   BROWSEINFO   oBrowseData;      // Properties for the browse window
   ITEMIDLIST*  pFolderObject;   // The IDList of the folder the user selects
   TCHAR*       szFolderPath;      // The path of the folder the user selects

   // Prepare
   utilZeroObject(&oBrowseData, BROWSEINFO);
   oBrowseData.hwndOwner = hDialog;
   oBrowseData.pidlRoot  = NULL;
   oBrowseData.ulFlags   = BIF_RETURNONLYFSDIRS;

   // Set title
   oBrowseData.lpszTitle = utilLoadString(getResourceInstance(), IDS_GAME_FOLDER_BROWSE_TEXT, 128);

   /// Query user for a folder
   if (pFolderObject = SHBrowseForFolder(&oBrowseData))
   {
      // Resolve path
      szFolderPath = utilCreateEmptyPath();
      SHGetPathFromIDList(pFolderObject, szFolderPath);
      
      // Display
      SetDlgItemText(hDialog, iDestinationControl, szFolderPath);

      // Cleanup
      utilDeleteString(szFolderPath);
      CoTaskMemFree(pFolderObject);
   }

   // Cleanup
   utilDeleteString((TCHAR*&)oBrowseData.lpszTitle);
}


/// Function name  : performPreferencesFolderValidation
// Description     : Validate a game or mod folder
// 
// PREFERENCES*  pPreferencesCopy  : [in] Working copy of the app preferences
// HWND          hDialog           : [in] Preferences dialog window handle
// CONST UINT    iComboControlID   : [in] ID of the combo control containing the path to validate
// 
VOID   performPreferencesFolderValidation(PREFERENCES_DATA*  pDialogData, HWND  hDialog, CONST UINT  iComboControlID)
{
   TCHAR*  szFolderPath;     // Path to validate

   // Prepare
   szFolderPath = utilCreateString(MAX_PATH);

   // Get path to validate and ensure a trailing backslash
   GetDlgItemText(hDialog, iComboControlID, szFolderPath, MAX_PATH);
   PathAddBackslash(szFolderPath);

   // Determine what kind of folder we're validating
   switch (iComboControlID)
   {
   /// [GAME FOLDER] Check for either extracted data files -OR- game catalogues with an executable
   case IDC_GAME_FOLDER_COMBO:
      // Update GameFolder state and GameVersion
      pDialogData->pPreferencesCopy->eGameFolderState = calculateGameFolderState(szFolderPath, pDialogData->pPreferencesCopy->eGameVersion);
      break;
   
   /// [MOD FOLDER] Check there's a matching <something>.cat and <something>.dat file pair
   default:
      // TODO: Validate catalogues
      break;
   }
   
   // Cleanup
   utilDeleteString(szFolderPath);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         MESSAGE HANDLERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onPreferencesDialog_Close
// Description     : Re-calculate game folder state and determine what data has changed (if any)
// 
// HWND          hParentWnd      : [in]     Parent window for 'Select Game Version' dialog
// PREFERENCES*  pNewPreferences : [in/out] New preferences selected in dialog
// 
// Return Value   : Combination of the following flags:
///                     PSR_LANGUAGE_CHANGED    : The interface or game data language has changed
///                     PSR_GAME_FOLDER_CHANGED : The game folder has changed
// 
INT     onPreferencesDialog_Close(HWND  hParentWnd, PREFERENCES*  pNewPreferences)
{
   INT   iUpdateFlags;       // Flags indicating which critical preferences have changed

   // Prepare
   iUpdateFlags = NULL;

   // [CHECK] Has user changed the interface language?
   if (pNewPreferences->eAppLanguage  != getAppPreferences()->eAppLanguage)
       /// [APP LANGUAGE CHANGE] Set 'App Language Changed' flag
       iUpdateFlags = PSR_APP_LANGUAGE_CHANGED;

   // [CHECK] Has user changed the game data language?
   if (pNewPreferences->eGameLanguage != getAppPreferences()->eGameLanguage)
       /// [GAME LANGUAGE CHANGE] Set 'Game Language Changed' flag
       iUpdateFlags = PSR_GAME_LANGUAGE_CHANGED;

   // [CHECK] Has user changed the game data folder?
   if (!utilCompareStringVariables(pNewPreferences->szGameFolder, getAppPreferences()->szGameFolder))
      /// [GAME FOLDER CHANGE] Set 'Game Folder Changed' flag
      iUpdateFlags |= PSR_GAME_FOLDER_CHANGED;


   // Examine new GameFolder state
   switch (pNewPreferences->eGameFolderState)
   {
   // [VALID] Do nothing - GameVersion has been set appropriately by validation function   
   // [INVALID] Do nothing - GameVersion has been set to 'UNKNOWN' by validation function

   /// [INCOMPLETE] Ask the user to supply the game version
   case GFS_INCOMPLETE:
      // Display the 'Game Version' dialog and save result
      pNewPreferences->eGameVersion = displayGameVersionDialog(hParentWnd, pNewPreferences->eGameVersion == GV_ALBION_PRELUDE);      // If X3TC and X3AP executables are present, 'eGameVersion' == GV_ALBION_PRELUDE, otherwise GV_UNKNOWN

      // Update state
      pNewPreferences->eGameFolderState = (pNewPreferences->eGameVersion != GV_UNKNOWN ? GFS_VALID : GFS_INVALID);

      // [VALID] Set 'Game Folder Changed' flag
      if (pNewPreferences->eGameFolderState == GFS_VALID)
         iUpdateFlags |= PSR_GAME_FOLDER_CHANGED;     
      break;
   }

   // Return flags
   return iUpdateFlags;
}


/// Function name  : onPreferencesPage_Help
// Description     : Launch the appropriate help page for the current page
// 
// PREFERENCES_DATA*       pDialogData : [in] Dialog data
// CONST PREFERENCES_PAGE  ePage       : [in] Preferences page
// CONST HELPINFO*         pRequest    : [in] System WM_HELP data
// 
VOID  onPreferencesPage_Help(PREFERENCES_DATA*  pDialogData, CONST PREFERENCES_PAGE  ePage, CONST HELPINFO*  pRequest)
{
   switch (ePage)
   {
   case PP_GENERAL:     displayHelp(TEXT("Preferences_General"));     break;
   case PP_FOLDERS:     displayHelp(TEXT("Preferences_Folders"));     break;
   case PP_APPEARANCE:  displayHelp(TEXT("Preferences_Appearance"));  break;
   case PP_SYNTAX:      displayHelp(TEXT("Preferences_Syntax"));      break;
   case PP_MISC:        displayHelp(TEXT("Preferences_Misc"));        break;
   }
}


/// Function name  : onPreferencesPage_Notify
// Description     : Notification processing for a specified 'Preferences' property-sheet page
//
// PREFERENCES_DATA*       pDialogData : [in] Preferences dialog data
// HWND                    hDialog     : [in] Preferences property page window handle
// NMHDR*                  pMessage    : [in] WM_NOTIFY message header
// CONST PREFERENCES_PAGE  ePage       : [in] ID of a preferences property page
// 
// Return type : TRUE if processed, FALSE if not
//
BOOL  onPreferencesPage_Notify(PREFERENCES_DATA*  pDialogData, HWND  hDialog, NMHDR*  pMessage, CONST PREFERENCES_PAGE  ePage)
{
   PREFERENCES*  pPreferencesCopy;
   BOOL          bAllowClosure;      // If FALSE the property page will not close.

   // Prepare
   pPreferencesCopy = pDialogData->pPreferencesCopy;

   switch (pMessage->code)
   {
   // [PRE-DISPLAY] -- Load control values
   case PSN_SETACTIVE:
      onPreferencesPage_Show(pDialogData, hDialog, ePage);
      SetWindowLong(hDialog, DWL_MSGRESULT, FALSE);
      return TRUE;

   // [SHEET CANCELLED] -- Close regardless
   case PSN_QUERYCANCEL:
      SetWindowLong(hDialog, DWL_MSGRESULT, FALSE);
      return TRUE;

   // [PRE-SAVE] -- Check control values are valid, then save
   case PSN_KILLACTIVE:
      bAllowClosure = !onPreferencesPage_Hide(pDialogData, hDialog, ePage);
      SetWindowLong(hDialog, DWL_MSGRESULT, bAllowClosure);
      return TRUE;

   // [OVERRIDE INITIAL FOCUS] -- Use tabbing order?
   //case PSN_QUERYINITIALFOCUS:
   //   SetWindowLong(hDialog, DWL_MSGRESULT, <CONTROL ID>);
   //   return TRUE;

   ///                        REQUIRES EACH CONTROL ID IS UNIQUE
   // [TOOLTIP DATA] -- Supply tooltip data
   case TTN_GETDISPINFO:
      onTooltipRequestText((NMTTDISPINFO*)pMessage);
      return TRUE;
   }

   return FALSE;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          DIALOG PROCEDURE
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : dlgprocPreferencesPage
// Description     : Base window procedure for all preferences pages -- 
//                     implements the functionality common to each of them.
//
INT_PTR   dlgprocPreferencesPage(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam, CONST PREFERENCES_PAGE  ePage)
{
   PREFERENCES_DATA*   pDialogData;     // Dialog data for the entire sheet
   PROPSHEETPAGE*      pCreationData;   // Creation data for this page
   ERROR_STACK*        pException;
   BOOL                bResult;
   
   __try
   {
      // Get sheet data
      TRACK_FUNCTION();
      pDialogData = getPreferencesData(hDialog);
      bResult     = TRUE;

      // Examine message
      switch (iMessage)
      {
      /// [CREATION] - Store the dialog data parameter and create tooltips for the current page
      case WM_INITDIALOG:
         // Extract sheet data from input creation data
         pCreationData = (PROPSHEETPAGE*)lParam;
         pDialogData   = (PREFERENCES_DATA*)pCreationData->lParam;
         // Store sheet data as dialog data
         SetWindowLong(hDialog, DWL_USER, (LONG)pDialogData);

         /// Initialise sheet
         if (!pDialogData->bInitialised)
         {
            utilCentreWindow(getAppWindow(), GetParent(hDialog));
            pDialogData->bInitialised = TRUE;
            //initPreferencesDialog(pDialogData, GetParent(hDialog));
         }
         
         /// Initialise controls and tooltips
         initPreferencesPage(pDialogData, hDialog, ePage);
         pDialogData->hTooltips[ePage] = createPreferencesTooltips(hDialog, ePage);

         // [VERBOSE]
         VERBOSE("Initialised preferences dialog property page '%s'", szDebugPageNames[ePage]);
         break;

      /// [DESTRUCTION] - Cleanup tooltip for this page
      case WM_DESTROY:
         // [VERBOSE]
         VERBOSE("Destroying preferences dialog property page '%s'", szDebugPageNames[ePage]);

         // [CHECK] Dialog data will not exist if page failed to create
         if (pDialogData)
            utilDeleteWindow(pDialogData->hTooltips[ePage]);
         break;

      /// [NOTIFICATION] - Page control
      case WM_NOTIFY:
         // [CHECK] Dialog data will not exist if page failed to create
         if (pDialogData)
            bResult = onPreferencesPage_Notify(pDialogData, hDialog, (NMHDR*)lParam, ePage);
         break;

      /// [HELP] Display helpfile
      case WM_HELP:
         onPreferencesPage_Help(pDialogData, ePage, (HELPINFO*)lParam);
         break;

      /// [CUSTOM COMBOBOX]
      case WM_DRAWITEM:     bResult = onWindow_DrawItem((DRAWITEMSTRUCT*)lParam);                 break;
      case WM_MEASUREITEM:  bResult = onWindow_MeasureItem(hDialog, (MEASUREITEMSTRUCT*)lParam);  break;
      case WM_DELETEITEM:   bResult = onWindow_DeleteItem((DELETEITEMSTRUCT*)lParam);             break;

      // [UNHANDLED] Return FALSE
      default:
         bResult = FALSE;
         break;
      }
   }
   /// [EXCEPTION HANDLER]
   __except (generateExceptionError(GetExceptionInformation(), pException))
   {
      // [ERROR] "An unidentified and unexpected critical error has occurred in the preferences base window"
      enhanceError(pException, ERROR_ID(IDS_EXCEPTION_PREFERENCES_BASE));
      displayException(pException);
      // Return TRUE
      bResult = TRUE;
   }

   // Return result
   END_TRACKING();
   return (INT_PTR)bResult;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          CALLBACKS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : callbackPreferencesDialog
// Description     : Customise the Preferences PropertySheet
// 
// HWND   hSheet   : [in] Window handle of 'Preferences' property sheet
// UINT   iMessage : [in] Callback message
// LPARAM lParam   : [in] Callback data
// 
// Return type : NULL
//
INT   callbackPreferencesDialog(HWND  hSheet, UINT  iMessage, LPARAM  lParam)
{
   HIMAGELIST  hNewImageList,      // Replacement ImageList
               hOldImageList;      // Existing Tab Control ImageList
   RECT        rcApply,            // Apply button rectangle
               rcCancel,           // Cancel button rectangle
               rcSheet;
   UINT        iApplyButtonID;     // Control ID of apply button
   HWND        hApplyButton,       // Apply button window handle
               hTabCtrl;           // Tab control window handle
   
   // Examine message
   switch (iMessage)
   {
   /// [PSCB_INITIALIZED] -- Modify the appearance of the property sheet
   case PSCB_INITIALIZED:
      // [VERBOSE]
      VERBOSE("Initialising preferences dialog");

      // Prepare
      hTabCtrl      = PropSheet_GetTabControl(hSheet);
      hNewImageList = utilCreateImageList(getResourceInstance(), ITS_SMALL, 5, "PREFERENCES_ICON", "FOLDERS_ICON", "APPEARANCE_ICON", "SYNTAX_ICON", "SUBMIT_REPORT_ICON");

      /// Remove 'MultiLine' style
      utilRemoveWindowStyle(hTabCtrl, TCS_MULTILINE);

      /// Replace 16bit colour ImageList
      if (hOldImageList = TabCtrl_SetImageList(hTabCtrl, hNewImageList))
         ImageList_Destroy(hOldImageList);

      // [CHECK] Find 'apply' button
      if (hApplyButton = FindWindow(NULL, TEXT("&Apply")))
         // [SUCCESS] Resolve control ID
         iApplyButtonID = GetWindowID(hApplyButton);
      else
         // [FAILED] Define control ID manually
         iApplyButtonID = 0x3021;  // [HACK]

      // Get 'Apply' and 'Cancel' rectangles
      utilGetDlgItemRect(hSheet, iApplyButtonID, &rcApply);
      utilGetDlgItemRect(hSheet, IDCANCEL, &rcCancel);

      /// [HACK] Slightly increase size of buttons to match rest of program
      rcCancel.bottom += 6;
      rcApply.bottom  += 6;
   
      /// Move 'OK'/'Cancel' to positions of 'Cancel'/'Apply'
      utilSetDlgItemRect(hSheet, IDCANCEL, &rcApply,  FALSE);
      utilSetDlgItemRect(hSheet, IDOK,     &rcCancel, FALSE);

      // Hide 'Apply'
      utilShowDlgItem(hSheet, iApplyButtonID, FALSE);

      /// Resize dialog to account for larger buttons
      GetWindowRect(hSheet, &rcSheet);
      SetWindowPos(hSheet, NULL, NULL, NULL, (rcSheet.right - rcSheet.left), (rcSheet.bottom - rcSheet.top) + 6, SWP_NOMOVE WITH SWP_NOZORDER);
      break;

   /// [BUTTON PRESSED]
   case PSCB_BUTTONPRESSED:
      // [VERBOSE] 
      VERBOSE("Preferences dialog dismissed with '%s' button", lParam == PSBTN_OK ? TEXT("OK") : TEXT("Cancel"));
      break;
   }

   return 0;
}


/// Function name  : callbackPreferencesPage
// Description     : Called when a page is created or destroyed
// 
// HWND            hPage      : [in] NULL
// UINT            iMessage   : [in] Message
// PROPSHEETPAGE*  pPageData  : [in] Page data
// 
// Return Value   : TRUE
// 
UINT  callbackPreferencesPage(HWND  hPage, UINT  iMessage, PROPSHEETPAGE*  pPageData)
{
    //TCHAR*  szPageTitle;

    //// [CHECK] Ensure we're creating a page
    //if (iMessage == PSPCB_CREATE)
    //{
    //   // [VERBOSE]
    //   VERBOSE("Creating preferences dialog property page '%s'", szPageTitle = utilLoadString(getResourceInstance(), (UINT)pPageData->pszTitle, 64));

    //   // Cleanup
    //  utilDeleteString(szPageTitle);
    //}

    return TRUE;
}



