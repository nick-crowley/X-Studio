//
// MainLib.cpp : Handle creation and destruction of the library
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// Global application data
APPLICATION*     pApplication      = NULL;     

// Logic module instance handle
HINSTANCE        hLibraryInstance  = NULL;     

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          CONSTANTS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// Size of the small/medium ImageTrees
CONST UINT       iSmallIconCount  = 53,
                 iMediumIconCount = 91,
                 iLargeIconCount  = 10;

// Defines the small icons used by controls
CONST TCHAR*     szSmallIcons[iSmallIconCount] = 
{
   // Debugging
   TEXT("MISSING_ICON"),

   // SearchResults Dialog
   TEXT("THREAT_ICON"),          TEXT("REUNION_ICON"),            TEXT("TERRAN_CONFLICT_ICON"),   TEXT("ALBION_PRELUDE_ICON"),      
   TEXT("COMMAND_LIST_ICON"),    TEXT("GAME_OBJECT_ICON"),        TEXT("SCRIPT_OBJECT_ICON"), 

   // Output Dialog
   TEXT("VALID_ICON"),           TEXT("INVALID_ICON"),            TEXT("PREFERENCES_ICON"),       TEXT("ERROR_ICON"),            TEXT("WARNING_ICON"),             TEXT("INFORMATION_ICON"),

   // Properties Dialog
   TEXT("ARGUMENT_ICON"),        TEXT("SCRIPT_CALLER_ICON"),      TEXT("SCRIPT_DEPENDENCY_ICON"), TEXT("VARIABLE_ICON"),         TEXT("VARIABLE_DEPENDENCY_ICON"), TEXT("NEW_LANGUAGE_FILE_ICON"),

   // Project dialog
   TEXT("NEW_SCRIPT_FILE_ICON"), TEXT("NEW_LANGUAGE_FILE_ICON"),  TEXT("NEW_MISSION_FILE_ICON"),  TEXT("NEW_PROJECT_FILE_ICON"), TEXT("FOLDERS_ICON"), 

   // Language Document
   TEXT("VOICED_YES_ICON"),      TEXT("VOICED_NO_ICON"),

   // File Dialog
   TEXT("NEW_FILE_ICON"),        TEXT("ALL_FILES_ICON"),

   // CodeEdit
   TEXT("VARIABLE_ICON"),        TEXT("FUNCTION_ICON"), 

   // Preferences Dialog
   TEXT("ENGLISH_ICON"),         TEXT("FRENCH_ICON"),              TEXT("GERMAN_ICON"),           TEXT("ITALIAN_ICON"),          TEXT("POLISH_ICON"),              TEXT("RUSSIAN_ICON"),
   TEXT("SPANISH_ICON"),

   // Colours
   TEXT("BLACK_ICON"),           TEXT("BLUE_ICON"),                TEXT("CYAN_ICON"),             TEXT("GREEN_ICON"),            TEXT("ORANGE_ICON"),
   TEXT("PURPLE_ICON"),          TEXT("RED_ICON"),                 TEXT("WHITE_ICON"),            TEXT("YELLOW_ICON"),           TEXT("GREY_ICON"),
   TEXT("DARK_GREEN_ICON"),      TEXT("DARK_GREY_ICON"),           TEXT("DARK_RED_ICON"),

   // Overlays
   TEXT("INVALID_OVERLAY"),      TEXT("DESCRIPTION_OVERLAY"),
};

// Defines the medium icons used by CustomMenus
CONST TCHAR*     szMediumIcons[iMediumIconCount] = 
{
   // Debugging
   TEXT("MISSING_ICON"),

   // Main Window
   TEXT("ABOUT_ICON"),             TEXT("CLOSE_DOCUMENT_ICON"),   TEXT("CLOSE_ALL_DOCUMENTS_ICON"), TEXT("COMMAND_LIST_ICON"),   TEXT("COPY_ICON"),             TEXT("CUT_ICON"), 
   TEXT("DELETE_ICON"),            TEXT("FIND_ICON"),             TEXT("FORUMS_ICON"),              TEXT("GAME_OBJECT_ICON"),    TEXT("HELP_ICON"),             TEXT("NEW_FILE_ICON"),      
   TEXT("NEW_LANGUAGE_FILE_ICON"), TEXT("NEW_MISSION_FILE_ICON"), TEXT("NEW_SCRIPT_FILE_ICON"),     TEXT("OPEN_FILE_ICON"),      TEXT("OPEN_SAMPLES_ICON"),     TEXT("OUTPUT_WINDOW_ICON"),
   TEXT("PASTE_ICON"),             TEXT("PREFERENCES_ICON"),      TEXT("PROPERTIES_ICON"),          TEXT("QUIT_ICON"),           TEXT("REDO_ICON"),             TEXT("SAVE_FILE_ICON"), 
   TEXT("SAVE_FILE_AS_ICON"),      TEXT("SAVE_ALL_FILES_ICON"),   TEXT("SCRIPT_OBJECT_ICON"),       TEXT("SELECT_ALL_ICON"),     TEXT("SUBMIT_REPORT_ICON"),    TEXT("UNDO_ICON"), 
   TEXT("UPDATE_ICON"),            TEXT("COMMENT_ICON"),          TEXT("FIND_TEXT_ICON"),           TEXT("TUTORIAL_ICON"),       TEXT("NEW_PROJECT_FILE_ICON"),
   
   // CodeEdit
   TEXT("ERROR_ICON"),             TEXT("WARNING_ICON"),          TEXT("EDIT_FORMATTING_ICON"), 

   // Properties Dialog
   TEXT("ADD_ICON"),               TEXT("REFRESH_ICON"),          TEXT("REMOVE_ICON"),              TEXT("SIGNED_ICON"),         TEXT("SCRIPT_CALLER_ICON"),    TEXT("SCRIPT_DEPENDENCY_ICON"),
   TEXT("THREAT_ICON"),            TEXT("REUNION_ICON"),          TEXT("TERRAN_CONFLICT_ICON"),     TEXT("ALBION_PRELUDE_ICON"),

   // Script Document
   TEXT("FUNCTION_ICON"),          TEXT("VARIABLE_ICON"),

   // Language Document
   TEXT("EDIT_FORMATTING_ICON"),   TEXT("DELETE_PAGE_ICON"),      TEXT("EDIT_PAGE_ICON"),           TEXT("INSERT_PAGE_ICON"),    TEXT("FORMATTING_ERROR_ICON"),  
   TEXT("VOICED_YES_ICON"),        TEXT("VOICED_NO_ICON"),

   // Message Dialog
   TEXT("VALID_ICON"),             TEXT("INVALID_ICON"),          TEXT("NEXT_ICON"),                TEXT("ATTACHMENT_ICON"),

   // Project Dialog
   TEXT("CLOSE_PROJECT_ICON"),     TEXT("NEW_PROJECT_FILE_ICON"), TEXT("INSERT_PROJECT_FILE_ICON"), TEXT("SAVE_PROJECT_ICON"),   TEXT("PROJECT_EXPLORER_ICON"), TEXT("REMOVE_PROJECT_FILE_ICON"),     

   // ProjectVariables Dialog
   TEXT("ADD_VARIABLE_ICON"),      TEXT("REMOVE_VARIABLE_ICON"),

   // Result Dialog
   TEXT("INSERT_RESULT_ICON"),

   // Preferences Dialog
   TEXT("ENGLISH_ICON"),           TEXT("FRENCH_ICON"),           TEXT("GERMAN_ICON"),              TEXT("ITALIAN_ICON"),        
   TEXT("POLISH_ICON"),            TEXT("RUSSIAN_ICON"),          TEXT("SPANISH_ICON"),

   // Colours
   TEXT("BLACK_ICON"),             TEXT("BLUE_ICON"),             TEXT("CYAN_ICON"),                TEXT("GREEN_ICON"),          TEXT("ORANGE_ICON"),
   TEXT("PURPLE_ICON"),            TEXT("RED_ICON"),              TEXT("WHITE_ICON"),               TEXT("YELLOW_ICON"),         TEXT("GREY_ICON"),
   TEXT("DARK_GREEN_ICON"),        TEXT("DARK_GREY_ICON"),        TEXT("DARK_RED_ICON")
};

// Defines the small icons used by tooltips
CONST TCHAR*     szLargeIcons[iLargeIconCount] = 
{
   // Debugging
   TEXT("MISSING_ICON"),

   // CustomTooltips
   TEXT("THREAT_ICON"),          TEXT("REUNION_ICON"),            TEXT("TERRAN_CONFLICT_ICON"),  TEXT("ALBION_PRELUDE_ICON"),      
   TEXT("GAME_OBJECT_ICON"),     TEXT("SCRIPT_OBJECT_ICON"), 

   // File dialog
   TEXT("LANGUAGE_FOLDER_ICON"), TEXT("MISSION_FOLDER_ICON"),     TEXT("SCRIPT_FOLDER_ICON"),
};

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION  /  DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createApplication
// Description     : Creates the application object
// 
// HINSTANCE  hInstance : [in] Application instance
//
BearScriptAPI
VOID   createApplication(HINSTANCE  hInstance)
{
   /// Create Application object
   pApplication = utilCreateEmptyObject(APPLICATION);

   // Allocate GameData object
   pApplication->pGameData        = utilCreateEmptyObject(GAME_DATA);

   /// Create internal objects
   pApplication->pConsole         = createConsoleData();
   pApplication->pCallStackList   = createCallStackList();
   pApplication->pSmallImageTree  = createImageTree(ITS_SMALL, iSmallIconCount);
   pApplication->pMediumImageTree = createImageTree(ITS_MEDIUM, iMediumIconCount);
   pApplication->pLargeImageTree  = createImageTree(ITS_LARGE, iLargeIconCount);

   // Set properties
   pApplication->hAppInstance     = hInstance;   
   pApplication->hLibraryInstance = hLibraryInstance;
   pApplication->eWindowsVersion  = utilGetWindowsVersion();

   // [DEBUGGING] Set working folder
   utilSetProcessWorkingFolder();
}


/// Function name  : deleteApplication
// Description     : Destroys the Application object
// 
BearScriptAPI
VOID  deleteApplication()
{
   /// Free GameData object
   utilDeleteObject(pApplication->pGameData);

   // Destroy CallStack list
   deleteCallStackList(pApplication->pCallStackList);

   // Destroy Console data
   deleteConsoleData(pApplication->pConsole);

   // Destroy ImageTrees
   deleteImageTree(pApplication->pSmallImageTree);
   deleteImageTree(pApplication->pMediumImageTree);
   deleteImageTree(pApplication->pLargeImageTree);

   /// Destroy Application object
   utilDeleteObject(pApplication);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : generateResourceLibraryPath
// Description     : Generates the full path of the resource library for the specified language
// 
// CONST APP_LANGUAGE  eLanguage : [in] Interface language
// 
// Return Value   : New string containing full path of library, you are responsible for destroying it
// 
BearScriptAPI 
TCHAR*    generateResourceLibraryPath(CONST APP_LANGUAGE  eLanguage)
{
   TCHAR  *szExePath,
          *szOutput;

   // Prepare
   szExePath = utilCreateEmptyPath();

   // Get full path of EXE
   GetModuleFileName(NULL, szExePath, MAX_PATH);

   /// Generate full path to appropriate library
   switch (eLanguage)
   {
   case AL_ENGLISH:     szOutput = utilRenameFilePath(szExePath, TEXT("X-Studio.English.DLL"));    break;
   case AL_GERMAN:      szOutput = utilRenameFilePath(szExePath, TEXT("X-Studio.German.DLL"));    break;
   case AL_RUSSIAN:     szOutput = utilRenameFilePath(szExePath, TEXT("X-Studio.Russian.DLL"));    break;
   }

   // Cleanup and return
   utilDeleteString(szExePath);
   return szOutput;
}


/// Function name  : identifyAppLanguageString
// Description     : Resolves an AppLanguage into a string. This is hardcoded because it is operated when the resource library is not loaded
// 
// CONST APP_LANGUAGE  eLanguage : [in] AppLanguage to resolve
// 
// Return Value   : Constant App language string
// 
BearScriptAPI
CONST TCHAR*  identifyAppLanguageString(CONST APP_LANGUAGE  eLanguage)
{
   CONST TCHAR*  szOutput;

   // Examine language
   switch (eLanguage)
   {
   case AL_ENGLISH:     szOutput = TEXT("English");      break;
   case AL_GERMAN:      szOutput = TEXT("German");       break;
   case AL_RUSSIAN:     szOutput = TEXT("Russian");      break;
   }

   // Return result
   return szOutput;
}


/// Function name  : identifyGameVersionString
// Description     : Retrieves the string containing the name of the specified game version
// 
// CONST GAME_VERSION  eVersion   : [in] Game version ID
// 
// Return Value   : Game version string
// 
BearScriptAPI
CONST TCHAR*  identifyGameVersionString(CONST GAME_VERSION  eVersion)
{
   return pApplication->szGameVersions[eVersion];
}


/// Function name : loadAppDialogFonts
// Description    : Temporary installs the Segoe UI font into the font table
//
// Return Value   : TRUE 
// 
BearScriptAPI
BOOL  loadAppDialogFonts()
{
   CONST TCHAR* szFontResources[2] = { TEXT("DIALOG_TITLE_FONT"), TEXT("DIALOG_HEADING_FONT") }; //, TEXT("DIALOG_BOLD_FONT"), TEXT("DIALOG_REGULAR_FONT") };
   CONST BYTE*  szBuffer;
   UINT         iFileLength;
   HANDLE       hTempFile;
   DWORD        dwBytesWritten;
   TCHAR       *szTempFolder,
               *szTempFontPath;

   // Prepare
   szTempFontPath = utilCreateEmptyPath();
   szTempFolder   = utilCreateEmptyPath();

   // Iterate through fonts
   for (UINT  iFont = 0; iFont < 2; iFont++)
   {
      /// Load font from resource library
      if (iFileLength = utilLoadResourceFileA(getResourceInstance(), szFontResources[iFont], szBuffer))
      {
         // Prepare
         GetTempPathW(MAX_PATH, szTempFolder);

         // Generate temporary filename
         if (!GetTempFileNameW(szTempFolder, TEXT("Font"), NULL, szTempFontPath))
            StringCchPrintf(szTempFontPath, MAX_PATH, TEXT("%sxstudio.font.tmp"), szTempFolder);

         // Output file to temporary filename
         if ((hTempFile = CreateFileW(szTempFontPath, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, NULL, NULL)) != INVALID_HANDLE_VALUE)
         {
            // Save temporary file
            WriteFile(hTempFile, szBuffer, iFileLength, &dwBytesWritten, NULL);
            utilCloseHandle(hTempFile);

            /// Add font and delete temp file
            ERROR_CHECK("Adding UI dialog font", AddFontResourceEx(szTempFontPath, NULL, NULL));   //FR_PRIVATE
            DeleteFile(szTempFontPath);
         }
      }
   }

   // Cleanup
   utilDeleteStrings(szTempFolder, szTempFontPath);
   return TRUE;
}


/// Function name  : loadAppImageTrees
// Description     : Loads the icons used by the controls and menus
// 
BearScriptAPI
VOID  loadAppImageTrees()
{
   IMAGE_TREE  *pSmallImageTree,    // Convenience pointer
               *pMediumImageTree,   // Convenience pointer
               *pLargeImageTree;    // Convenience pointer

   // Prepare
   pSmallImageTree  = getAppImageTree(ITS_SMALL);
   pMediumImageTree = getAppImageTree(ITS_MEDIUM);
   pLargeImageTree  = getAppImageTree(ITS_LARGE);

   /// Populate Small ImageTree
   for (UINT  iIcon = 0; iIcon < iSmallIconCount; iIcon++)
      insertImageTreeIcon(pSmallImageTree, szSmallIcons[iIcon]);

   /// Populate Medium ImageTree
   for (UINT  iIcon = 0; iIcon < iMediumIconCount; iIcon++)
      insertImageTreeIcon(pMediumImageTree, szMediumIcons[iIcon]);

   /// Populate Large ImageTree
   for (UINT  iIcon = 0; iIcon < iLargeIconCount; iIcon++)
      insertImageTreeIcon(pLargeImageTree, szLargeIcons[iIcon]);
}


/// Function name : loadHelpLibrary
// Description    : Initialises the HTML help API and launches the separate help thread
//
// Return Value   : TRUE if succesful, otherwise FALSE
// 
BearScriptAPI
BOOL  loadHelpLibrary()
{
   return (HtmlHelp(NULL, NULL, HH_INITIALIZE, (DWORD_PTR)&pApplication->hHelpFile) != NULL);
}


/// Function name  : loadResourceLibrary
// Description     : Loads the resource library for the specified game language
// 
// CONST APP_LANGUAGE  eLanguage : [in]  Game language of the library to load - must be English, German or Russian
// ERROR_STACK*       &pError    : [out] Error message, if any
// 
// Return Value   : TRUE if succesful, otherwise FALSE
// 
BearScriptAPI
BOOL  loadResourceLibrary(CONST APP_LANGUAGE  eLanguage, ERROR_STACK*  &pError)
{
   TCHAR*  szLibraryPath;     // Full path of library

   // [TRACK]
   TRACK_FUNCTION();
   
   // [CHECK] Ensure library is not already loaded
   VERBOSE("Loading %s resource library", identifyAppLanguageString(eLanguage));
   ASSERT(getResourceInstance() == NULL);
   
   // Prepare
   pError = NULL;

   // Generate appropriate library path
   szLibraryPath = generateResourceLibraryPath(eLanguage);

   /// Load resource library for the specified language
   if (pApplication->hResourceInstance = LoadLibrary(szLibraryPath))
   {
      // [SUCCESS] Load application title and resource DLL path
      LoadString(getResourceInstance(), IDS_APPLICATION_NAME, pApplication->szName, 128);
      StringCchCopy(pApplication->szResourceLibrary, MAX_PATH, szLibraryPath);

      // Load game version strings
      for (UINT iVersion = GV_THREAT; iVersion <= GV_UNKNOWN; iVersion++)
         LoadString(getResourceInstance(), (IDS_GAME_VERSION_THREAT + iVersion), pApplication->szGameVersions[iVersion], 32);
   }
   else
      // [ERROR] "There was an error while loading the dynamic link library containing the application resources - check '%s' is not missing."
      pError = generateDualError(HERE(IDS_INIT_RESOURCE_DLL_FAILED), PathFindFileName(szLibraryPath));

   // Cleanup and return TRUE if loaded successfully
   utilDeleteString(szLibraryPath);
   END_TRACKING();
   return (getResourceInstance() != NULL);
}


/// Function name  : loadRichEditLibrary
// Description     : Attempts to load the RichEdit library
// 
// Return Value   : TRUE/FALSE
// 
BearScriptAPI
BOOL  loadRichEditLibrary()
{
   /// Attempt to load RichEdit version 3.0
   pApplication->hRichEditDLL = LoadLibrary(TEXT("RichEd20.DLL"));

   // Return result
   return (pApplication->hRichEditDLL != NULL);
}


/// Function name  : loadSystemImageListIcons
// Description     : Forces application icons used by the FileDialog into the System ImageList
// 
BearScriptAPI
BOOL  loadSystemImageListIcons()
{
   UINT  iAppIconIndex,       // Ordinal of icon within resource DLL
         iSystemIconIndex;    // New system ImageList index of the icon

   /// Initialise system ImageList from disc
   if (!utilInitProcessImageList())
      return FALSE;

   /// Iterate through all Game versions
   for (GAME_VERSION  eVersion = GV_THREAT; eVersion <= GV_ALBION_PRELUDE; eVersion = (GAME_VERSION)(eVersion + 1))
   {
      /// Manually identify resource DLL icon ordinal
      switch (eVersion)
      {
      case GV_THREAT:            iAppIconIndex = 98;     break;
      case GV_REUNION:           iAppIconIndex = 81;     break;
      case GV_TERRAN_CONFLICT:   iAppIconIndex = 97;     break;
      case GV_ALBION_PRELUDE:    iAppIconIndex = 3;      break;
      }

      /// [GAME VERSION] Force the System ImageList to extract relevant GameVersion icon
      iSystemIconIndex = utilGetCachedIconIndex(getAppResourceLibraryPath(), iAppIconIndex, GIL_DONTCACHE);

      // Store system ImageList index
      pApplication->iSystemImageListIcons[eVersion] = iSystemIconIndex;
   }

   /// [DOCUMENT TYPE] Force the System ImageList to extract the DocumentType icons
   pApplication->iSystemImageListIcons[4] = utilGetCachedIconIndex(getAppResourceLibraryPath(), 56, GIL_DONTCACHE);  // Ordinal 56 == NEW_LANGUAGE_FILE_ICON
   pApplication->iSystemImageListIcons[5] = utilGetCachedIconIndex(getAppResourceLibraryPath(), 57, GIL_DONTCACHE);  // Ordinal 57 == NEW_MISSION_FILE_ICON
   pApplication->iSystemImageListIcons[6] = utilGetCachedIconIndex(getAppResourceLibraryPath(), 58, GIL_DONTCACHE);  // Ordinal 58 == NEW_PROJECT_FILE_ICON
   pApplication->iSystemImageListIcons[7] = utilGetCachedIconIndex(getAppResourceLibraryPath(), 59, GIL_DONTCACHE);  // Ordinal 59 == NEW_SCRIPT_FILE_ICON

   // [SUCCESS]
   return TRUE;
}


/// Function name  : unloadApplication
// Description     : Unloads the game data, saves preferences, unloads COM, RichEdit DLL and HTML-Help
// 
BearScriptAPI
VOID   unloadApplication()
{
   // [VERBOSE]
   VERBOSE_LIB_COMMAND();

   // Validate heap
   ERROR_CHECK("validating heap", utilValidateMemory());

   /// Delete loaded game data
   destroyGameData();

   /// [OPTIONAL] Save and delete preferences  (Do not exist if missing or newer version)
   if (getAppPreferences())
   {
      saveAppPreferences();
      deleteAppPreferences();
   }

   // [OPTIONAL] Unload HTML Help
   if (pApplication->hHelpFile)
      unloadHelpLibrary();

   // Stop COM thread
   CoUninitialize();

   /// [OPTIONAL] Unload resource library  (Do not exist if required library not found)
   if (getResourceInstance())
      unloadResourceLibrary();
}


/// Function name : unloadHelpLibrary
// Description    : Unloads the HTML help API and closes the separate help thread
//
// Return Value   : TRUE if succesful, otherwise FALSE
// 
BearScriptAPI
VOID  unloadHelpLibrary()
{
   // Close HTML thread
   HtmlHelp(NULL, NULL, HH_UNINITIALIZE, (DWORD_PTR)pApplication->hHelpFile);
   pApplication->hHelpFile = NULL;
}


/// Function name  : unloadResourceLibrary
// Description     : Unloads the currently loaded resource library
// 
BearScriptAPI
VOID  unloadResourceLibrary()
{
   // [CHECK] Ensure resource library is loaded
   ASSERT(getResourceInstance() != NULL);

   // Free resource library
   if (getResourceInstance())
   {
      FreeLibrary(getResourceInstance());
      pApplication->hResourceInstance = NULL;
   }
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        ENTRY POINT
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : DllMain
// Description     : DLL Entry point
//
// HMODULE  hModule    : [in] Instance of loading module?
// DWORD    dwPurpose  : [in] Reason for call
// LPVOID   lpReserved : [in] ???
// 
// Return type : BOOL APIENTRY 
//
BOOL APIENTRY   DllMain(HMODULE  hModule, DWORD  dwPurpose, LPVOID  lpReserved)
{
   switch (dwPurpose)
   {
   // [PROCESS LOAD] - Save instance
   case DLL_PROCESS_ATTACH:
      // Save instance
      hLibraryInstance = hModule;
      return TRUE;
         
   // [THREAD LOAD/UNLOAD]
   case DLL_THREAD_ATTACH:
   case DLL_THREAD_DETACH:
      break;

   // [PROCESS UNLOAD] - Zero instance
   case DLL_PROCESS_DETACH:
      hLibraryInstance = NULL;
      break;
   }

   return TRUE;
}

