//
// Global Helpers.cpp : FILE_DESCRIPTION
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS  / GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// Application data
extern APPLICATION*     pApplication;

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : getAppConsole
// Description     : Global access to the console window handle
// 
// Return Value   : Window handle of the console window
// 
BearScriptAPI 
HWND  getAppActiveDialog()
{
   return pApplication->hActiveDialog;
}

/// Function name  : getAppCallStackList
// Description     : Global access to the application CallStack list
// 
// Return Value   : Application CallStack list
// 
BearScriptAPI 
CALL_STACK_LIST*  getAppCallStackList()
{
   return pApplication->pCallStackList;
}


/// Function name  : getAppConsole
// Description     : Global access to the console window handle
// 
// Return Value   : Window handle of the console window
// 
BearScriptAPI 
CONSOLE_DATA*  getAppConsole()
{
   return pApplication->pConsole;
}


/// Function name  : getAppError
// Description     : Determine whether a type of critical error has occurred or not
// 
// CONST APPLICATION_ERROR   eError : [in] Critical error type
//
// Return Value   : TRUE/FALSE
// 
BearScriptAPI 
BOOL  getAppError(CONST APPLICATION_ERROR  eError)
{
   return pApplication->bCriticalErrors[eError];
}


/// Function name  : getAppFolder
// Description     : Global access to the application folder
// 
// Return Value   : Application folder path (Guaranteed to have a trailing backslash)
// 
BearScriptAPI
const TCHAR*  getAppFolder()
{
   return pApplication->szFolder;
}

/// Function name  : getAppInstance
// Description     : Global access to the application instance
// 
// Return Value   : Application instance handle
// 
BearScriptAPI 
HINSTANCE  getAppInstance()
{
   return pApplication->hAppInstance;
}


/// Function name  : getAppImageList
// Description     : Retrieves one of the application ImageLists
// 
// CONST IMAGE_TREE_SIZE  eSize : [in] Size of the ImageTree to retrieve
// 
// Return Value   : ImageList handle
// 
BearScriptAPI
HIMAGELIST   getAppImageList(CONST IMAGE_TREE_SIZE  eSize)
{
   // Retrieve associated ImageList
   return getAppImageTree(eSize)->hImageList;
}


/// Function name  : getAppImageTree
// Description     : Retrieves one of the application ImageTrees
// 
// CONST IMAGE_TREE_SIZE  eSize : [in] Desired ImageTree Size
// 
// Return Value   : ImageTree
// 
BearScriptAPI
IMAGE_TREE*   getAppImageTree(CONST IMAGE_TREE_SIZE  eSize)
{
   IMAGE_TREE*   pOutput;

   // Examine size
   switch (eSize)
   {
   case ITS_SMALL:   pOutput = pApplication->pSmallImageTree;    break;
   case ITS_MEDIUM:  pOutput = pApplication->pMediumImageTree;   break;
   case ITS_LARGE:   pOutput = pApplication->pLargeImageTree;    break;

   default:          
      VERBOSE(BUG "Invalid ImageTree size");
      pOutput = pApplication->pMediumImageTree;   break;
   }

   // Return ImageTree
   return pOutput;
}


/// Function name  : getAppImageTreeIconIndex
// Description     : Retrieves the index of an icon within an applicaiton ImageTree
// 
// CONST IMAGE_TREE_SIZE  eSize    : [in] ImageTree size
// CONST TCHAR*           szIconID : [in] Resource ID of the desired icon
// 
// Return Value   : Application ImageTree icon index
// 
BearScriptAPI
UINT  getAppImageTreeIconIndex(CONST IMAGE_TREE_SIZE  eSize, CONST TCHAR*  szIconID)
{
   return getImageTreeIconIndex(getAppImageTree(eSize), szIconID);
}


/// Function name  : getAppImageTreeIconHandle
// Description     : Retrieves the icon handle of an icon within an application ImageTree
// 
// CONST IMAGE_TREE_SIZE  eSize    : [in] ImageTree size
// CONST TCHAR*           szIconID : [in] Resource ID of the desired icon
// 
// Return Value   : Icon handle
// 
BearScriptAPI
HICON  getAppImageTreeIconHandle(CONST IMAGE_TREE_SIZE  eSize, CONST TCHAR*  szIconID)
{
   return getImageTreeIconHandle(getAppImageTree(eSize), szIconID);
}


/// Function name  : getAppName
// Description     : Global access to the application name
// 
// Return Value   : Application name string
// 
BearScriptAPI 
CONST TCHAR*  getAppName()
{
   return pApplication->szName;
}


/// Function name  : getAppRegistryKey
// Description     : Global access to the name of the app's registry key
// 
// Return Value   : Registry key name
// 
BearScriptAPI 
CONST TCHAR*  getAppRegistryKey()
{
   return TEXT("X-Studio");
}


/// Function name  : getAppRegistrySubKey
// Description     : Global access to the name of one of the app's registry sub-keys
// 
// Return Value   : Registry sub-key name
// 
BearScriptAPI 
CONST TCHAR*  getAppRegistrySubKey(CONST APPLICATION_REGISTRY_KEY  eSubKey)
{
   CONST TCHAR*  szOutput;    // Subkey name

   // Examine subkey ID
   switch (eSubKey)
   {
   case ARK_SESSION:         szOutput = TEXT("Session");          break;
   case ARK_CURRENT_SCHEME:  szOutput = TEXT("CurrentScheme");    break;
   case ARK_SCHEME_LIST:     szOutput = TEXT("ColourSchemes");    break;
   case ARK_RECENT_FILES:    szOutput = TEXT("MRU");              break;
   case ARK_RECENT_FOLDERS:  szOutput = TEXT("Folders");          break;
   }
   
   // Return result
   return szOutput;
}


/// Function name  : getAppResourceLibraryPath
// Description     : Global access to the path of the active resource library
// 
// Return Value   : Full path of active resource library
// 
BearScriptAPI
CONST TCHAR*  getAppResourceLibraryPath()
{
   return pApplication->szResourceLibrary;
}


/// Function name  : getAppState
// Description     : Retrieves the application state
// 
// Return Value : Application state
// 
BearScriptAPI
APPLICATION_STATE  getAppState()
{
   return pApplication->eState;
}


/// Function name  : getAppThemeBrush
// Description     : Retrieves a themed system colour brush
// 
// const UINT  iSysColour : [in] System colour
//
// Return Value : Brush
// 
BearScriptAPI  
HBRUSH  getAppThemeBrush(const UINT  iSysColour)
{
   return pApplication->hBrushes[iSysColour <= COLOR_MENUBAR ? iSysColour : NULL];
}


/// Function name  : getAppVersion
// Description     : Global access to the app version
// 
// Return Value   : App version
// 
BearScriptAPI 
APPLICATION_VERSION  getAppVersion()
{
   return pApplication->eVersion;
}


/// Function name  : getAppWindow
// Description     : Global access to the main window's window handle
// 
// Return Value   : Window handle of the main window
// 
BearScriptAPI 
HWND  getAppWindow()
{
   return pApplication->hMainWnd;
}

/// Function name  : getAppWindowsVersion
// Description     : Global access to the OS version
// 
// Return Value   : Windows version
// 
BearScriptAPI 
WINDOWS_VERSION  getAppWindowsVersion()
{
   return pApplication->eWindowsVersion;
}


/// Function name  : getFileSystem
// Description     : Global access to the FileSystem
// 
// Return Value   : File System
// 
BearScriptAPI
FILE_SYSTEM*   getFileSystem()
{
   return pApplication->pFileSystem;
}


/// Function name  : getGameData
// Description     : Global access to the loaded game data
// 
// Return Value   : Game Data
// 
BearScriptAPI
GAME_DATA*  getGameData()
{
   return pApplication->pGameData;
}


/// Function name  : getLibraryInstance
// Description     : Global access to the library instance
// 
// Return Value   : Library instance
// 
BearScriptAPI 
HINSTANCE  getLibraryInstance()
{
   return pApplication->hLibraryInstance;
}


/// Function name  : getResourceInstance
// Description     : Global access to the resource library instance
// 
// Return Value   : Resource library instance
// 
BearScriptAPI
HINSTANCE  getResourceInstance()
{
   return pApplication->hResourceInstance;
}


/// Function name  : getSystemImageListDocumentTypeIcon
// Description     : Retrieves the System ImageList index of the icon for the specified document type
// 
// CONST FILE_ITEM_FLAG  eDocumentType : [in] Must be FIF_LANGUAGE or FIF_MISSION
///                                                   NB: eDocumentType must be a DOCUMENT_TYPE but that doesn't exist in logic library
// 
// Return Value   : System ImageList icon index
// 
BearScriptAPI
UINT  getSystemImageListDocumentTypeIcon(CONST FILE_ITEM_FLAG  eDocumentType)
{
   UINT  iArrayIndex;

   // Convert document type to correct array index
   switch (eDocumentType)
   {
   case FIF_LANGUAGE:      iArrayIndex = 4;     break;      // ScriptFile   icon stored in index 4
   case FIF_MISSION:       iArrayIndex = 5;     break;      // MissionFile  icon stored in index 5
   case FIF_PROJECT:       iArrayIndex = 6;     break;      // ProjectFile  icon stored in index 6
   case FIF_SCRIPT:        iArrayIndex = 7;     break;      // LanguageFile icon stored in index 7
   default:                ASSERT(FALSE);
   }

   // Return relevant icon index
   return pApplication->iSystemImageListIcons[iArrayIndex];      
}


/// Function name  : getSystemImageListGameVersionIcon
// Description     : Retrieves the System ImageList index of the icon for the specified game version
// 
// CONST GAME_VERSION  eGameVersion   : [in] GameVersion
// 
// Return Value   : System ImageList icon index
// 
BearScriptAPI
UINT  getSystemImageListGameVersionIcon(CONST GAME_VERSION  eGameVersion)
{
   // [CHECK] Ensure version is valid
   ASSERT(eGameVersion != GV_UNKNOWN);
   // Return relevant icon index
   return pApplication->iSystemImageListIcons[eGameVersion];
}


/// Function name  : hasAppErrors
// Description     : Determine whether any critical errors occurred or not
//
// Return Value   : TRUE/FALSE
// 
BearScriptAPI 
BOOL  hasAppErrors()
{
   return    pApplication->bCriticalErrors[AE_ACCESS_VIOLATION] 
          OR pApplication->bCriticalErrors[AE_DEBUG_ASSERTION] 
          OR pApplication->bCriticalErrors[AE_TERMINATION] 
          OR pApplication->bCriticalErrors[AE_EXCEPTION] 
          OR pApplication->bCriticalErrors[AE_LOAD_GAME_DATA] 
          OR pApplication->bCriticalErrors[AE_LOAD_SCRIPT];
}


/// Function name  : isAppClosing
// Description     : Global access to the 'Is app closing' flag
// 
// Return Value   : TRUE/FALSE
// 
BearScriptAPI 
BOOL  isAppClosing()
{
   return pApplication->bClosing;
}


/// Function name  : isAppVerbose
// Description     : Global access to whether to output extra detail to the console
// 
// Return Value   : TRUE/FALSE
// 
BearScriptAPI 
BOOL  isAppVerbose()
{
   return pApplication->bVerbose;
}


/// Function name  : resetAppErrors
// Description     : Reset all critical error flags
//
BearScriptAPI 
VOID  resetAppErrors()
{
   // Zero all flags
   utilZeroMemory(pApplication->bCriticalErrors, 6 * sizeof(BOOL));
}


/// Function name  : setAppActiveDialog
// Description     : Change the active dialog
//
// CONST HWND  hDialog : [in] Dialog
// 
BearScriptAPI 
VOID  setAppActiveDialog(HWND  hDialog)
{
   pApplication->hActiveDialog = hDialog;
}



/// Function name  : setAppClosing
// Description     : Change the 'Application is closing' flag
//
// CONST BOOL  bClosing : [in] TRUE/FALSE
// 
BearScriptAPI 
VOID  setAppClosing(CONST BOOL  bClosing)
{
   pApplication->bClosing = bClosing;
}


/// Function name  : setAppError
// Description     : Sets the appropriate flag indicating a critical error has occurred
// 
// CONST APPLICATION_ERROR   eError : [in] Type of error that has occurred
// 
BearScriptAPI 
VOID  setAppError(CONST APPLICATION_ERROR  eError)
{
   pApplication->bCriticalErrors[eError] = TRUE;
}


/// Function name  : setAppState
// Description     : Change the 'Is Game Data Present' flag
// 
// CONST APPLICATION_STATE  eNewState : [in] New state
// 
BearScriptAPI
VOID   setAppState(CONST APPLICATION_STATE  eNewState)
{
   CONSOLE("Changing Application state from %d to %d", pApplication->eState, eNewState);
   // Save new state
   pApplication->eState = eNewState;
}


/// Function name  : setAppVerboseMode
// Description     : Sets whether to output extra detail to the console
//
// CONST BOOL  bVerboseMode : [in] TRUE/FALSE
// 
BearScriptAPI 
VOID  setAppVerboseMode(CONST BOOL  bVerboseMode)
{
   pApplication->bVerbose = bVerboseMode;
}


/// Function name  : setAppWindow
// Description     : Store the window handle of the main window
// 
// HWND  hAppWindow  : [in] Main window handle
// 
BearScriptAPI 
VOID  setAppWindow(HWND  hAppWindow)
{
   pApplication->hMainWnd = hAppWindow;
}


/// Function name  : setFileSystem
// Description     : Store the FileSystem
// 
// FILE_SYSTEM*  pFileSystem : [in] File system
// 
BearScriptAPI 
VOID  setFileSystem(FILE_SYSTEM*  pFileSystem)
{
   pApplication->pFileSystem = pFileSystem;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : generateAppThemeBrushes
// Description     : Creates/Destroys app theme brushes
//
// const BOOL  bCreate : [in] TRUE to create, FALSE to destroy
// 
BearScriptAPI
VOID  generateAppThemeBrushes(const BOOL  bCreate)
{
   for (UINT  iColour = 0; iColour <= COLOR_MENUBAR; iColour++)
   {
      // [DESTROY]
      if (!bCreate AND pApplication->hBrushes[iColour])
         DeleteBrush(pApplication->hBrushes[iColour]);

      // [CREATE]
      if (bCreate)
         pApplication->hBrushes[iColour] = getThemeSysColourBrush(TEXT("Window"), iColour);
   }
}


/// Function name  : getThemeSysColourBrush
// Description     : Retrieves system colour brush
// 
// const TCHAR*  szClass    : [in] Class name
// const int     iSysColour : [in] System colour
// 
// Return Value   : Brush, must be destroyed
//
/// [HACK] I have moved this from Controls->Logic library so it can be used by generateAppThemeBrushes()
// 
BearScriptAPI
HBRUSH  getThemeSysColourBrush(const TCHAR*  szClass, const int  iSysColour)
{
   // Lookup brush
   HTHEME hTheme = (IsThemeActive() ? OpenThemeData(getAppWindow(), szClass) : NULL);
   HBRUSH hBrush = GetThemeSysColorBrush(hTheme, iSysColour);

   // Return brush
   CloseThemeData(hTheme);
   return hBrush;
}

