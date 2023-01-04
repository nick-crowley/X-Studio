//
// MainUI.cpp : Application creation/destruction and message pump
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// Application preferences
PREFERENCES*         pAppPreferences = NULL;

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         CONSTANTS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// Version of preferences - only needs to be changed when structure/name of prefs actually change
APPLICATION_VERSION  ePreferencesVersion = AV_BETA_4;

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                  CREATION / DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createPreferences
// Description     : Creates a Preferences object containing default values
// 
// Return Value   : New preferences object, you are responsible for destroying it
// 
PREFERENCES*  createPreferences()
{
   PREFERENCES*  pNewPreferences = utilCreateEmptyObject(PREFERENCES);

   // Return new object
   return pNewPreferences;
}


/// Function name  : deletePreferences
// Description     : Deletes an application preferences object
// 
// PREFERENCES*  pAppPreferences : [in] Application preferences object
// 
BearScriptAPI
VOID  deletePreferences(PREFERENCES*  &pAppPreferences)
{
   // Delete calling object
   utilDeleteObject(pAppPreferences);
}


/// Function name  : duplicatePreferences
// Description     : Duplicate an application preferences object
// 
// CONST PREFERENCES*  pSource  : [in] Existing preferences object to copy
// 
// Return Type : New preferences object
//
BearScriptAPI
PREFERENCES*  duplicatePreferences(CONST PREFERENCES*  pSource)
{
   PREFERENCES*  pNewPreferences;

   // Create new object
   pNewPreferences = utilCreateEmptyObject(PREFERENCES);

   // Copy values from input object
   (*pNewPreferences) = (*pSource);

   // Return new object
   return pNewPreferences;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : getAppPreferences
// Description     : Application-wide read-only access to the application preferences
// 
// Return type : Read-only Application preferences object
//
BearScriptAPI
CONST PREFERENCES*   getAppPreferences()
{
   return pAppPreferences;
}


/// Function name  : getAppPreferencesLanguage
// Description     : Attempt to read the interface language from the registry
// 
// APP_LANGUAGE*  pLanguage : [in/out] Interface language if successful, otherwise undefined
// 
// Return Value   : TRUE if successful, FALSE if version is missing
// 
BearScriptAPI
BOOL   getAppPreferencesLanguage(APP_LANGUAGE*  pLanguage)
{
   HKEY     hAppKey;   // App registry key
   BOOL     bResult;   // Operation result
   
   // Prepare
   bResult = FALSE;

   // [VERBOSE]
   VERBOSE("Determining application interface language");

   // Attempt to open app RegistryKey
   if (hAppKey = utilRegistryOpenAppKey(getAppRegistryKey()))
   {
      // Extract Preferences version
      bResult = utilRegistryReadNumber(hAppKey, TEXT("eAppLanguage"), pLanguage);
      utilRegistryCloseKey(hAppKey);
   }

   // Return result
   return bResult;
}



/// Function name  : getAppPreferencesWindowRect
// Description     : Get a pointer to the specified window rectangle for read/write access
// 
// CONST APPLICATION_WINDOW  eWindow  : [in] ID of the window to get the rectangle for
// 
// Return Value   : Window rectangle pointer
// 
BearScriptAPI
RECT*  getAppPreferencesWindowRect(CONST APPLICATION_WINDOW  eWindow)
{
   RECT*  pOutput;

   switch (eWindow)
   {
   case AW_MAIN:              pOutput = &pAppPreferences->rcMainWindow;        break;
   case AW_FIND:              pOutput = &pAppPreferences->rcFindDialog;        break;
   case AW_PROPERTIES:        pOutput = &pAppPreferences->rcPropertiesDialog;  break;
   default: ASSERT(FALSE);    pOutput = NULL;
   }

   return pOutput;
}


/// Function name  : getAppPreferencesWindowSize
// Description     : Get a pointer to the specified window size for read/write access
// 
// CONST APPLICATION_WINDOW  eWindow  : [in] ID of the window to get the size for
// 
// Return Value   : Window rectangle pointer
// 
BearScriptAPI
SIZE*  getAppPreferencesWindowSize(CONST APPLICATION_WINDOW  eWindow)
{
   SIZE*  pOutput;

   switch (eWindow)
   {
   case AW_BROWSER:           pOutput = &pAppPreferences->siFileDialog;          break;
   default: ASSERT(FALSE);    pOutput = NULL;
   }

   return pOutput;
}


/// Function name  : getAppPreferencesVersion
// Description     : Attempt to read the preferences version from the registry
// 
// APPLICATION_VERSION*  pversion : [in/out] GameVersion if successful, otherwise undefined
// 
// Return Value   : TRUE if successful, FALSE if version is missing
// 
BOOL   getAppPreferencesVersion(APPLICATION_VERSION*  pversion)
{
   HKEY     hAppKey;   // App registry key
   BOOL     bResult;   // Operation result
   
   // Prepare
   bResult = FALSE;

   // Attempt to open app RegistryKey
   if (hAppKey = utilRegistryOpenAppKey(getAppRegistryKey()))
   {
      // Extract Preferences version
      bResult = utilRegistryReadNumber(hAppKey, TEXT("Version"), pversion);
      utilRegistryCloseKey(hAppKey);
   }

   // Return result
   return bResult;
}


/// Function name  : setAppPreferencesBackupFileName
// Description     : Store the last filename used for backup
// 
// CONST TCHAR*  szLastFileName : [in] Last filename
// 
BearScriptAPI 
VOID   setAppPreferencesBackupFileName(CONST TCHAR*  szLastFileName)
{
   // Ensure exists
   if (!lstrlen(szLastFileName))
      return;

   // Store filename
   StringCchCopy(pAppPreferences->szBackupFileName, MAX_PATH, szLastFileName);
}


/// Function name  : setAppPreferencesBackupFolder
// Description     : Store the last folder used for backup
// 
// CONST TCHAR*  szLastFolder : [in] Last folder path
// 
BearScriptAPI 
VOID   setAppPreferencesBackupFolder(CONST TCHAR*  szLastFolder)
{
   // Ensure exists
   if (!lstrlen(szLastFolder))
      return;

   // Store folder, ensure trailing backslash
   StringCchCopy(pAppPreferences->szBackupFolder, MAX_PATH, szLastFolder);
   PathAddBackslash(pAppPreferences->szBackupFolder);
}


/// Function name  : setAppPreferencesDialogSplit
// Description     : Stores the size of a tool window in the main window workspace
// 
// CONST APPLICATION_WINDOW  eWindow : [in] ID of the window
///                                                Must be AW_SEARCH, AW_OUTPUT or AW_PROJECT
// CONST UINT                iSize   : [in] Current window size  
// 
BearScriptAPI
VOID   setAppPreferencesDialogSplit(CONST APPLICATION_WINDOW  eWindow, CONST UINT  iSize)
{
   // Examine window
   switch (eWindow)
   {
   case AW_OUTPUT:    pAppPreferences->iOutputDialogSplit  = max(80, iSize);   break;      // Incase of bugs, prevent sizes lower than 80 being saved
   case AW_PROJECT:   pAppPreferences->iProjectDialogSplit = max(80, iSize);   break;
   case AW_SEARCH:    pAppPreferences->iSearchDialogSplit  = max(80, iSize);   break;

   // [UNSUPPORTED] Error
   default: ASSERT(FALSE); 
   }
}


/// Function name  : setAppPreferencesDialogVisibility
// Description     : Store the visibility of a tool window in the application preferences
// 
// CONST APPLICATION_WINDOW  eWindow  : [in] Tool window ID
///                                                Must be AW_SEARCH, AW_OUTPUT, AW_PROPERTIES or AW_PROJECT
// CONST BOOL                bVisible : [in] Whether window is visible or invisible. 
// 
BearScriptAPI
VOID   setAppPreferencesDialogVisibility(CONST APPLICATION_WINDOW  eWindow, CONST BOOL  bVisible)
{
   // Examine window
   switch (eWindow)
   {
   case AW_PROPERTIES:    pAppPreferences->bPropertiesDialogVisible = bVisible;   break;
   case AW_OUTPUT:        pAppPreferences->bOutputDialogVisible     = bVisible;   break;
   case AW_PROJECT:       pAppPreferences->bProjectDialogVisible    = bVisible;   break;
   case AW_SEARCH:        pAppPreferences->bSearchDialogVisible     = bVisible;   break;

   // [UNSUPPORTED] Error
   default: ASSERT(FALSE); 
   }
}


/// Function name  : setAppPreferencesForumUserName
// Description     : Update the X-Universe forum name
// 
// CONST TCHAR*  szUserName : [in] New UserName
// 
BearScriptAPI
VOID   setAppPreferencesForumUserName(CONST TCHAR*  szUserName)
{
   // Save new name
   StringCchCopy(pAppPreferences->szForumUserName, 32, szUserName);
}


/// Function name  : setAppPreferencesGameFolderState
// Description     : Updates the result of game folder validation
// 
// CONST GAME_FOLDER_STATE  eNewFolderState   : [in] New game folder state
// CONST GAME_VERSION       eNewGameVersion   : [in] Associated game version
// 
BearScriptAPI
VOID   setAppPreferencesGameFolderState(CONST GAME_FOLDER_STATE  eNewFolderState, CONST GAME_VERSION  eNewGameVersion)
{
   // [CHECK] Cannot set the GameFolder state to indeterminate state used for calculations
   ASSERT(eNewFolderState != GFS_INCOMPLETE);

   // Save new states
   pAppPreferences->eGameFolderState = eNewFolderState;
   pAppPreferences->eGameVersion     = eNewGameVersion;
}


/// Function name  : setAppPreferencesLanguages
// Description     : Set the application languages (determined by the 'FirstRun' wizard)
// 
// CONST APP_LANGUAGE  eAppLanguage   : [in] Application display language
// CONST GAME_LANGUAGE eGameLanguage  : [in] Game data language
// 
BearScriptAPI 
VOID   setAppPreferencesLanguages(CONST APP_LANGUAGE  eAppLanguage, CONST GAME_LANGUAGE  eGameLanguage)
{
   // Save new languages
   pAppPreferences->eAppLanguage  = eAppLanguage;
   pAppPreferences->eGameLanguage = eGameLanguage;
}


/// Function name  : setAppPreferencesLastFolder
// Description     : Store the last folder used in the file dialog
// 
// CONST TCHAR*       szLastFolder : [in] Last folder path
// CONST FILE_FILTER  eLastFilter  : [in] Last file filter
// 
BearScriptAPI 
VOID   setAppPreferencesLastFolder(CONST TCHAR*  szLastFolder, CONST FILE_FILTER  eLastFilter)
{
   // [CHECK] Ensure folder is provided
   ASSERT(lstrlen(szLastFolder));

   // Store folder, ensure trailing backslash
   StringCchCopy(pAppPreferences->szLastFolder, MAX_PATH, szLastFolder);
   PathAddBackslash(pAppPreferences->szLastFolder);

   // Store filter
   pAppPreferences->eLastFileFilter = eLastFilter;
}


/// Function name  : setAppPreferencesLastFindText
// Description     : Store the last FindText dialog search string
// 
// CONST TCHAR*   szSearchText : [in] Last search text
// 
BearScriptAPI 
VOID   setAppPreferencesLastFindText(CONST TCHAR*  szSearchText)
{
   // Store text
   StringCchCopy(pAppPreferences->szFindText, MAX_PATH, szSearchText);
}


/// Function name  : setAppPreferencesMainWindowState
// Description     : Sets whether the main window is maximised
// 
// CONST BOOL  bMaximised : [in] TRUE/FALSE
// 
BearScriptAPI
VOID  setAppPreferencesMainWindowState(CONST BOOL  bMaximised)
{
   pAppPreferences->bMainWindowMaximised = bMaximised;
}


/// Function name  : setAppPreferencesSearchDialogFilter
// Description     : Store the item index of the selected ResultsDialog filter
// 
// CONST RESULT_TYPE  eDialogID : [in] Results dialog ID
// CONST UINT         iFilter   : [in] Zero-based item index
// 
BearScriptAPI
VOID   setAppPreferencesSearchDialogFilter(CONST RESULT_TYPE  eDialogID, CONST UINT  iFilter)
{
   pAppPreferences->eSearchDialogFilters[eDialogID] = iFilter;
}


/// Function name  : setAppPreferencesSearchDialogTab
// Description     : Store the ID of the current SearchDialog tab/dialog
// 
// CONST RESULT_TYPE  eDialogID : [in] Current SearchResults dialog
// 
BearScriptAPI
VOID   setAppPreferencesSearchDialogTab(CONST RESULT_TYPE  eDialogID)
{
   pAppPreferences->eSearchDialogTab = eDialogID;   
}


/// Function name  : setAppPreferencesTutorialComplete
// Description     : Stores the flag indicating a tutorial window have been displayed
// 
// CONST TUTORIAL_WINDOW  eDialogID : [in] Tutorial window ID
// 
BearScriptAPI
VOID   setAppPreferencesTutorialComplete(CONST TUTORIAL_WINDOW  eDialogID)
{
   pAppPreferences->bTutorialsRemaining[eDialogID] = FALSE;
}


/// Function name  : updateAppPreferences
// Description     : Replace the current application preferences values with new ones
// 
// CONST PREFERENCES*  pNewPreferences : [in] New application preferences values
// 
BearScriptAPI
VOID   updateAppPreferences(CONST PREFERENCES*  pNewPreferences)
{
   // Replace existing values
   (*pAppPreferences) = (*pNewPreferences);
}


/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                      FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : deleteAppPreferencesRegistryKey
// Description     : Destroys the existing Preferences in the Registry
// 
BearScriptAPI
VOID  deleteAppPreferencesRegistryKey()
{
   // Destroy entire key
   utilRegistryDeleteAppKey(getAppRegistryKey());
}


/// Function name  : deleteAppPreferences
// Description     : Destroys the global App preferences object
// 
BearScriptAPI
VOID  deleteAppPreferences()
{
   // Destroy global preferences
   deletePreferences(pAppPreferences);
}


/// Function name  : generateAppPreferencesFromRegistry
// Description     : Create a preferences object and fills it from the App RegistryKey
// 
// CONST APP_LANGUAGE  eInterfaceLanguage : [in] Current application language
//
// Return Value   : New preferences object, you are responsible for destroying it
// 
PREFERENCES*   generateAppPreferencesFromRegistry(CONST APP_LANGUAGE  eInterfaceLanguage)
{
   PREFERENCES*    pNewPreferences;      // Object being created
   COLOUR_SCHEME*  pCurrentScheme;       // Used for retrieving the current Colour scheme 
   HKEY            hAppKey,              // App Registry Key
                   hSchemeKey;           // ColourScheme registry sub-key
   BOOL            bSetStockSize;
   
   // Prepare
   bSetStockSize = FALSE;

   /// Create new object
   pNewPreferences = createPreferences();

   /// Create/Open App RegistryKey
   hAppKey = utilRegistryCreateAppKey(getAppRegistryKey());
   
   // [LANGUAGES]
   if (!utilRegistryReadNumber(hAppKey, TEXT("eAppLanguage"), &pNewPreferences->eAppLanguage))
      pNewPreferences->eAppLanguage = eInterfaceLanguage;

   if (!utilRegistryReadNumber(hAppKey, TEXT("eGameLanguage"), &pNewPreferences->eGameLanguage))
      pNewPreferences->eGameLanguage = GL_ENGLISH;

   // [GAME DATA]
   if (!utilRegistryReadNumber(hAppKey, TEXT("eGameVersion"), &pNewPreferences->eGameVersion))
      pNewPreferences->eGameVersion = GV_UNKNOWN;

   if (!utilRegistryReadNumber(hAppKey, TEXT("eGameFolderState"), &pNewPreferences->eGameFolderState))
      pNewPreferences->eGameFolderState = GFS_INVALID;
   
   // [DIALOG VISIBILITY]
   if (!utilRegistryReadNumber(hAppKey, TEXT("bOutputDialogVisible"), &pNewPreferences->bOutputDialogVisible))
      pNewPreferences->bOutputDialogVisible = TRUE;

   if (!utilRegistryReadNumber(hAppKey, TEXT("bSearchDialogVisible"), &pNewPreferences->bSearchDialogVisible))
      pNewPreferences->bSearchDialogVisible = FALSE;

   if (!utilRegistryReadNumber(hAppKey, TEXT("eSearchDialogTab"), &pNewPreferences->eSearchDialogTab))
      pNewPreferences->eSearchDialogTab = RT_COMMANDS;

   utilRegistryReadObjectArray(hAppKey, TEXT("eSearchDialogFilters"), pNewPreferences->eSearchDialogFilters, BOOL, 3);

   if (!utilRegistryReadNumber(hAppKey, TEXT("bProjectDialogVisible"), &pNewPreferences->bProjectDialogVisible))
      pNewPreferences->bProjectDialogVisible = FALSE;

   if (!utilRegistryReadNumber(hAppKey, TEXT("bPropertiesDialogVisible"), &pNewPreferences->bPropertiesDialogVisible))
      pNewPreferences->bPropertiesDialogVisible = FALSE;

   if (!utilRegistryReadNumber(hAppKey, TEXT("bMainWindowMaximised"), &pNewPreferences->bMainWindowMaximised))
      pNewPreferences->bMainWindowMaximised = FALSE;

   // [DIALOG SPLITS]
   if (!utilRegistryReadNumber(hAppKey, TEXT("iOutputDialogSplit"), &pNewPreferences->iOutputDialogSplit))
      bSetStockSize = TRUE;

   if (!utilRegistryReadNumber(hAppKey, TEXT("iProjectDialogSplit"), &pNewPreferences->iProjectDialogSplit))
      bSetStockSize = TRUE;

   if (!utilRegistryReadNumber(hAppKey, TEXT("iSearchDialogSplit"), &pNewPreferences->iSearchDialogSplit))
      bSetStockSize = TRUE;

   // [DIALOG POSITIONS]
   if (!utilRegistryReadObject(hAppKey, TEXT("rcMainWindow"), &pNewPreferences->rcMainWindow, RECT))
      bSetStockSize = TRUE;

   if (!utilRegistryReadObject(hAppKey, TEXT("rcPropertiesDialog"), &pNewPreferences->rcPropertiesDialog, RECT))
      bSetStockSize = TRUE;

   if (!utilRegistryReadObject(hAppKey, TEXT("rcFindDialog"), &pNewPreferences->rcFindDialog, RECT))
      bSetStockSize = TRUE;
      
   if (!utilRegistryReadObject(hAppKey, TEXT("siFileDialog"), &pNewPreferences->siFileDialog, SIZE))
      bSetStockSize = TRUE;
      
   // [FEATURES]
   if (!utilRegistryReadNumber(hAppKey, TEXT("bCodeIndentation"), &pNewPreferences->bCodeIndentation))
      pNewPreferences->bCodeIndentation = TRUE;
   if (!utilRegistryReadNumber(hAppKey, TEXT("bVersionIncrement"), &pNewPreferences->bVersionIncrement))
      pNewPreferences->bVersionIncrement = FALSE;
   if (!utilRegistryReadNumber(hAppKey, TEXT("bScriptCodeMacros"), &pNewPreferences->bScriptCodeMacros))
      pNewPreferences->bScriptCodeMacros = TRUE;
   if (!utilRegistryReadNumber(hAppKey, TEXT("bEditorTooltips"), &pNewPreferences->bEditorTooltips))
      pNewPreferences->bEditorTooltips  = TRUE;
   if (!utilRegistryReadNumber(hAppKey, TEXT("bPreserveSession"), &pNewPreferences->bPreserveSession))
      pNewPreferences->bPreserveSession = TRUE;
   if (!utilRegistryReadNumber(hAppKey, TEXT("bSearchResultTooltips"), &pNewPreferences->bSearchResultTooltips))
      pNewPreferences->bSearchResultTooltips = TRUE;

   if (!utilRegistryReadNumber(hAppKey, TEXT("bSuggestCommands"), &pNewPreferences->bSuggestions[CST_COMMAND]))
      pNewPreferences->bSuggestions[CST_COMMAND] = FALSE;
   if (!utilRegistryReadNumber(hAppKey, TEXT("bSuggestGameObjects"), &pNewPreferences->bSuggestions[CST_GAME_OBJECT]))
      pNewPreferences->bSuggestions[CST_GAME_OBJECT] = TRUE;
   if (!utilRegistryReadNumber(hAppKey, TEXT("bSuggestScriptObjects"), &pNewPreferences->bSuggestions[CST_SCRIPT_OBJECT]))
      pNewPreferences->bSuggestions[CST_SCRIPT_OBJECT] = TRUE;
   if (!utilRegistryReadNumber(hAppKey, TEXT("bSuggestVariables"), &pNewPreferences->bSuggestions[CST_VARIABLE]))
      pNewPreferences->bSuggestions[CST_VARIABLE] = TRUE;

   if (!utilRegistryReadNumber(hAppKey, TEXT("bTransparentProperties"), &pNewPreferences->bTransparentProperties))
      pNewPreferences->bTransparentProperties = FALSE;
   if (!utilRegistryReadNumber(hAppKey, TEXT("bTutorialMode"), &pNewPreferences->bTutorialMode))
      pNewPreferences->bTutorialMode = TRUE;
   if (!utilRegistryReadNumber(hAppKey, TEXT("bUseDoIfSyntax"), &pNewPreferences->bUseDoIfSyntax))
      pNewPreferences->bUseDoIfSyntax = TRUE;
   if (!utilRegistryReadNumber(hAppKey, TEXT("bUseSystemDialog"), &pNewPreferences->bUseSystemDialog))
      pNewPreferences->bUseSystemDialog = TRUE;
   
   if (!utilRegistryReadNumber(hAppKey, TEXT("iEditorTooltipDelay"), &pNewPreferences->iEditorTooltipDelay))
      pNewPreferences->iEditorTooltipDelay = 2;
   if (!utilRegistryReadNumber(hAppKey, TEXT("iSearchResultTooltipDelay"), &pNewPreferences->iSearchResultTooltipDelay))
      pNewPreferences->iSearchResultTooltipDelay = 2;

   // [GAME DATA]
   if (!utilRegistryReadNumber(hAppKey, TEXT("bReportGameStringOverwrites"), &pNewPreferences->bReportGameStringOverwrites))
      pNewPreferences->bReportGameStringOverwrites = FALSE;
   if (!utilRegistryReadNumber(hAppKey, TEXT("bReportMissingSubStrings"), &pNewPreferences->bReportMissingSubStrings))
      pNewPreferences->bReportMissingSubStrings = FALSE;
   if (!utilRegistryReadNumber(hAppKey, TEXT("bReportIncompleteGameObjects"), &pNewPreferences->bReportIncompleteGameObjects))
      pNewPreferences->bReportIncompleteGameObjects = FALSE;

   // [MISC]
   if (!utilRegistryReadNumber(hAppKey, TEXT("bAutomaticUpdates"), &pNewPreferences->bAutomaticUpdates))
      pNewPreferences->bAutomaticUpdates = FALSE;
   if (!utilRegistryReadNumber(hAppKey, TEXT("bPassiveFTP"), &pNewPreferences->bPassiveFTP))
      pNewPreferences->bPassiveFTP = TRUE;
   if (!utilRegistryReadNumber(hAppKey, TEXT("bSubmitBugReports"), &pNewPreferences->bSubmitBugReports))
      pNewPreferences->bSubmitBugReports = TRUE;
   if (!utilRegistryReadNumber(hAppKey, TEXT("eErrorHandling"), &pNewPreferences->eErrorHandling))
      pNewPreferences->eErrorHandling = EH_IGNORE;

   // [STATE]
   utilRegistryReadString(hAppKey, TEXT("szFindText"), pNewPreferences->szFindText, MAX_PATH);

   if (!utilRegistryReadNumber(hAppKey, TEXT("eLastFileFilter"), &pNewPreferences->eLastFileFilter))
      pNewPreferences->eLastFileFilter = FF_SCRIPT_XML_FILES;

   if (!utilRegistryReadObjectArray(hAppKey, TEXT("bTutorialsRemaining"), pNewPreferences->bTutorialsRemaining, BOOL, TUTORIAL_COUNT))
      for (UINT  iIndex = 0; iIndex < TUTORIAL_COUNT; iIndex++)
         pNewPreferences->bTutorialsRemaining[iIndex] = TRUE;

   // [FOLDERS]
   utilRegistryReadString(hAppKey, TEXT("szLastFolder"), pNewPreferences->szLastFolder, MAX_PATH);
   utilRegistryReadString(hAppKey, TEXT("szBackupFolder"), pNewPreferences->szBackupFolder, MAX_PATH);
   utilRegistryReadString(hAppKey, TEXT("szBackupFileName"), pNewPreferences->szBackupFileName, MAX_PATH);
   utilRegistryReadString(hAppKey, TEXT("szGameFolder"), pNewPreferences->szGameFolder, MAX_PATH);
   utilRegistryReadString(hAppKey, TEXT("szForumUserName"), pNewPreferences->szForumUserName, 32);

   // [COLOUR SCHEME]
   if (hSchemeKey = utilRegistryOpenAppSubKey(getAppRegistryKey(), getAppRegistrySubKey(ARK_CURRENT_SCHEME)))
   {
      // Extract colour scheme
      pCurrentScheme = generateColourSchemeFromRegistry(hSchemeKey);
      pNewPreferences->oColourScheme = *pCurrentScheme;
      // Cleanup
      deleteColourScheme(pCurrentScheme);
      utilRegistryCloseKey(hSchemeKey);
   }
   else
      // [MISSING] Use default
      pNewPreferences->oColourScheme = oDefaultScheme[0];

   // [CHECK] Ensure app rectangle is not corrupted
   if (bSetStockSize) // OR (ULONG)pNewPreferences->rcMainWindow.left > 1000 OR (ULONG)pNewPreferences->rcMainWindow.top > 1000 OR (ULONG)pNewPreferences->rcMainWindow.right > 2000 OR (ULONG)pNewPreferences->rcMainWindow.bottom > 2000)
   {
      // Set window rectangles
      utilSetRectangle(&pNewPreferences->rcMainWindow, 100, 100, 850, 650);
      SetRect(&pNewPreferences->rcPropertiesDialog,    640, 480, NULL, NULL);
      SetRect(&pNewPreferences->rcFindDialog,          640, 150, NULL, NULL);
      utilSetSize(&pNewPreferences->siFileDialog,      740, 570);

      // Set dialog splits
      pNewPreferences->iOutputDialogSplit  = 100;
      pNewPreferences->iProjectDialogSplit = 180;
      pNewPreferences->iSearchDialogSplit  = 250;
   }

   // Cleanup and return
   utilRegistryCloseKey(hAppKey);
   return pNewPreferences;
}


/// Function name  : loadAppPreferences
// Description     : Attempt to load the application preferences from the registry into the global storage object
//
// APP_LANGUAGE  eLanguage : [in]  Application language to use
// ERROR_STACK*  &pError   : [out] Error messages, if any
//
// Return Type : PS_ERROR if unsuccessful, otherwise state indicating result
// 
BearScriptAPI
PREFERENCES_STATE   loadAppPreferences(APP_LANGUAGE  eLanguage, ERROR_STACK*  &pError)
{
   APPLICATION_VERSION  eVersion;  // Preferences version
   PREFERENCES_STATE    eResult;   // Operation Result

   // [VERBOSE]
   VERBOSE("Loading application preferences");

   // Prepare
   pError = NULL;

   // [CHECK] Does App RegistryKey exist?
   if (utilRegistryCheckAppKeyExists(getAppRegistryKey()))
   {
      /// [MISSING/NEWER VERSION] Abort...
      if (!getAppPreferencesVersion(&eVersion) OR eVersion > ePreferencesVersion)
      {
         // [ERROR] "The application preferences stored in the registry are corrupted or from an unrecognised version"
         pError  = generateDualError(HERE(IDS_INIT_PREFERENCES_CORRUPT));
         eResult = PS_ERROR;
      }
      /// [PREVIOUS VERSION] Destroy previous values and create defaults
      else if (eVersion < ePreferencesVersion)
      {
         // [VERBOSE]
         VERBOSE("Application preferences from previous version found and destroyed");

         // Destroy previous values and use defaults
         deleteAppPreferencesRegistryKey();
         pAppPreferences = generateAppPreferencesFromRegistry(eLanguage); 

         // Return UPGRADED
         eResult = PS_UPGRADED;
      }
      /// [CURRENT VERSION] Load values from Registry
      else
      {
         // Create from Registry and return SUCCESS
         pAppPreferences = generateAppPreferencesFromRegistry(eLanguage);     
         eResult = PS_SUCCESS;
      }
   }
   /// [FIRST RUN] Create App RegistryKey
   else
   {
      // [VERBOSE]
      VERBOSE("No application preferences found - creating defaults");

      // Create defaults and return FIRST-RUN
      pAppPreferences = generateAppPreferencesFromRegistry(eLanguage);
      eResult = PS_FIRST_RUN;
   }

   // Return result
   return eResult;
}


#ifdef _TESTING
/// Configuration  : _TESTING
/// Function name  : saveAppPreferences
// Description     : Do nothing
//
BearScriptAPI 
BOOL  saveAppPreferences()
{
   return TRUE;
}
#else
/// Configuration  : DEBUG / RELEASE
/// Function name  : saveAppPreferences
// Description     : Save the current application preferences to the registry
///                     Creates the App RegistryKey if necessary
//
BearScriptAPI 
BOOL  saveAppPreferences()
{
   TCHAR*  szSubKey;    // Name of the colour scheme sub-key being saved
   HKEY    hAppKey,     // App registry key
           hSchemeKey;   // Colour scheme registry sub-key

   // [VERBOSE]
   VERBOSE("Saving application preferences");

   // Prepare
   szSubKey = utilCreateEmptyString(32);

   // Create/Open app registry key
   hAppKey = utilRegistryCreateAppKey(getAppRegistryKey());

   /// [VERSION]
   utilRegistryWriteInteger(hAppKey, TEXT("Version"), ePreferencesVersion);
   
   /// [LANGUAGE]
   utilRegistryWriteNumber(hAppKey, TEXT("eAppLanguage"),  pAppPreferences->eAppLanguage);
   utilRegistryWriteNumber(hAppKey, TEXT("eGameLanguage"), pAppPreferences->eGameLanguage);

   /// [GAME DATA]
   utilRegistryWriteNumber(hAppKey, TEXT("eGameVersion"),     pAppPreferences->eGameVersion);
   utilRegistryWriteNumber(hAppKey, TEXT("eGameFolderState"), pAppPreferences->eGameFolderState);
   
   /// [DIALOG VISIBILITY]
   utilRegistryWriteNumber(hAppKey, TEXT("bOutputDialogVisible"),      pAppPreferences->bOutputDialogVisible);
   utilRegistryWriteNumber(hAppKey, TEXT("bSearchDialogVisible"),      pAppPreferences->bSearchDialogVisible);
   utilRegistryWriteNumber(hAppKey, TEXT("eSearchDialogTab"),          pAppPreferences->eSearchDialogTab);
   utilRegistryWriteObjectArray(hAppKey, TEXT("eSearchDialogFilters"), pAppPreferences->eSearchDialogFilters, BOOL, 3);
   utilRegistryWriteNumber(hAppKey, TEXT("bProjectDialogVisible"),     pAppPreferences->bProjectDialogVisible);
   utilRegistryWriteNumber(hAppKey, TEXT("bPropertiesDialogVisible"),  pAppPreferences->bPropertiesDialogVisible);
   utilRegistryWriteNumber(hAppKey, TEXT("bMainWindowMaximised"),      pAppPreferences->bMainWindowMaximised);
   
   /// [DIALOG SPLITS]
   utilRegistryWriteNumber(hAppKey, TEXT("iOutputDialogSplit"),  pAppPreferences->iOutputDialogSplit);
   utilRegistryWriteNumber(hAppKey, TEXT("iProjectDialogSplit"), pAppPreferences->iProjectDialogSplit);
   utilRegistryWriteNumber(hAppKey, TEXT("iSearchDialogSplit"),  pAppPreferences->iSearchDialogSplit);

   /// [DIALOG POSITIONS]
   utilRegistryWriteObject(hAppKey, TEXT("rcMainWindow"),       &pAppPreferences->rcMainWindow, RECT);
   utilRegistryWriteObject(hAppKey, TEXT("rcFindDialog"),       &pAppPreferences->rcFindDialog, RECT);
   utilRegistryWriteObject(hAppKey, TEXT("rcPropertiesDialog"), &pAppPreferences->rcPropertiesDialog, RECT);
   utilRegistryWriteObject(hAppKey, TEXT("siFileDialog"),       &pAppPreferences->siFileDialog, SIZE);

   /// [FEATURES]
   utilRegistryWriteNumber(hAppKey, TEXT("bCodeIndentation"),             pAppPreferences->bCodeIndentation);
   utilRegistryWriteNumber(hAppKey, TEXT("bScriptCodeMacros"),            pAppPreferences->bScriptCodeMacros);
   utilRegistryWriteNumber(hAppKey, TEXT("bVersionIncrement"),            pAppPreferences->bVersionIncrement);
   utilRegistryWriteNumber(hAppKey, TEXT("bEditorTooltips"),              pAppPreferences->bEditorTooltips);
   utilRegistryWriteNumber(hAppKey, TEXT("bSearchResultTooltips"),        pAppPreferences->bSearchResultTooltips);
   utilRegistryWriteNumber(hAppKey, TEXT("bSuggestCommands"),             pAppPreferences->bSuggestions[CST_COMMAND]);
   utilRegistryWriteNumber(hAppKey, TEXT("bSuggestGameObjects"),          pAppPreferences->bSuggestions[CST_GAME_OBJECT]);
   utilRegistryWriteNumber(hAppKey, TEXT("bSuggestScriptObjects"),        pAppPreferences->bSuggestions[CST_SCRIPT_OBJECT]);
   utilRegistryWriteNumber(hAppKey, TEXT("bSuggestVariables"),            pAppPreferences->bSuggestions[CST_VARIABLE]);
   utilRegistryWriteNumber(hAppKey, TEXT("bUseDoIfSyntax"),               pAppPreferences->bUseDoIfSyntax);
   utilRegistryWriteNumber(hAppKey, TEXT("bUseSystemDialog"),             pAppPreferences->bUseSystemDialog);
   
   utilRegistryWriteNumber(hAppKey, TEXT("iEditorTooltipDelay"),          pAppPreferences->iEditorTooltipDelay);
   utilRegistryWriteNumber(hAppKey, TEXT("iSearchResultTooltipDelay"),    pAppPreferences->iSearchResultTooltipDelay);
   
   utilRegistryWriteNumber(hAppKey, TEXT("bPreserveSession"),             pAppPreferences->bPreserveSession);
   utilRegistryWriteNumber(hAppKey, TEXT("bTransparentProperties"),       pAppPreferences->bTransparentProperties);
   utilRegistryWriteNumber(hAppKey, TEXT("bTutorialMode"),                pAppPreferences->bTutorialMode);
   
   utilRegistryWriteNumber(hAppKey, TEXT("bReportGameStringOverwrites"),  pAppPreferences->bReportGameStringOverwrites);
   utilRegistryWriteNumber(hAppKey, TEXT("bReportMissingSubStrings"),     pAppPreferences->bReportMissingSubStrings);
   utilRegistryWriteNumber(hAppKey, TEXT("bReportIncompleteGameObjects"), pAppPreferences->bReportIncompleteGameObjects);

   utilRegistryWriteNumber(hAppKey, TEXT("bAutomaticUpdates"),            pAppPreferences->bAutomaticUpdates);
   utilRegistryWriteNumber(hAppKey, TEXT("bPassiveFTP"),                  pAppPreferences->bPassiveFTP);
   utilRegistryWriteNumber(hAppKey, TEXT("bSubmitBugReports"),            pAppPreferences->bSubmitBugReports);
   utilRegistryWriteNumber(hAppKey, TEXT("eErrorHandling"),               pAppPreferences->eErrorHandling);
   
   /// [STATE]
   utilRegistryWriteNumber(hAppKey,      TEXT("eLastFileFilter"),     pAppPreferences->eLastFileFilter);
   utilRegistryWriteObjectArray(hAppKey, TEXT("bTutorialsRemaining"), pAppPreferences->bTutorialsRemaining, BOOL, TUTORIAL_COUNT);
   utilRegistryWriteString(hAppKey,      TEXT("szFindText"),          pAppPreferences->szFindText);
   
   /// [FOLDERS]
   utilRegistryWriteString(hAppKey, TEXT("szLastFolder"),     pAppPreferences->szLastFolder);
   utilRegistryWriteString(hAppKey, TEXT("szBackupFolder"),   pAppPreferences->szBackupFolder);
   utilRegistryWriteString(hAppKey, TEXT("szBackupFileName"), pAppPreferences->szBackupFileName);
   utilRegistryWriteString(hAppKey, TEXT("szGameFolder"),     pAppPreferences->szGameFolder);
   utilRegistryWriteString(hAppKey, TEXT("szForumUserName"),  pAppPreferences->szForumUserName);
   
   /// [DEFAULT SCHEMES] Ensure default schemes are saved to the registry (for the preferences dialog)
   for (UINT  iIndex = 0; iIndex < 4; iIndex++)
   {
      // Prepare
      StringCchPrintf(szSubKey, 32, TEXT("%s\\Scheme%u"), getAppRegistrySubKey(ARK_SCHEME_LIST), iIndex);

      // Create/Open scheme sub-key
      if (hSchemeKey = utilRegistryCreateAppSubKey(getAppRegistryKey(), szSubKey))
      {
         // [SUCCESS] Save colour scheme
         saveColourSchemeToRegistry(hSchemeKey, &oDefaultScheme[iIndex]);
         utilRegistryCloseKey(hSchemeKey);
      }
   }

   /// [CURRENT SCHEME] Create/Open scheme sub-key
   if (hSchemeKey = utilRegistryCreateAppSubKey(getAppRegistryKey(), getAppRegistrySubKey(ARK_CURRENT_SCHEME)))
   {
      // [SUCCESS] Save colour scheme
      saveColourSchemeToRegistry(hSchemeKey, &pAppPreferences->oColourScheme);
      utilRegistryCloseKey(hSchemeKey);
   }

   // Cleanup
   utilDeleteString(szSubKey);
   utilRegistryCloseKey(hAppKey);
   return TRUE;
}
#endif



#ifdef _TESTING
/// Configuration  : _TESTING
/// Function name  : setAppPreferencesGameDataFolder
// Description     : Alter the game folder directly. Ensure the app state isn't 'NoGameData'.
//                      Used for setting the game folder to the path of each TestCase
// 
// CONST TCHAR*  szNewFolder : [in] New GameFolder path
// 
BearScriptAPI 
VOID  setAppPreferencesGameDataFolder(CONST TCHAR*  szNewFolder)
{
   StringCchCopy(pAppPreferences->szGameFolder, MAX_PATH, szNewFolder);
   setAppState(AS_GAME_DATA_LOADED);
}
#endif


