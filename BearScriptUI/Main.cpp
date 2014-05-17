//
// MainUI.cpp : Application creation/destruction and message pump
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"
#include "Dbghelp.h"

// Import Common controls v6.0
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                   CONSTANTS / GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// Window classes
CONST TCHAR*   szMainWindowClass          = TEXT("MAIN_WINDOW");
CONST TCHAR*   szOperationPoolClass       = TEXT("OperationPoolWnd");
CONST TCHAR*   szDocumentsCtrlClass       = TEXT("DocumentsCtrl");
CONST TCHAR*   szArgumentsLabelComboClass = TEXT("ArgumentsLabelComboCtrl");
CONST TCHAR*   szFileDialogEditClass      = TEXT("FileDialogEdit");
CONST TCHAR*   szArgumentsLabelEditClass  = TEXT("ArgumentsLabelEditCtrl");
CONST TCHAR*   szScriptDocumentClass      = TEXT("ScriptDocument");
CONST TCHAR*   szLanguageDocumentClass    = TEXT("LanguageDocument");
CONST TCHAR*   szSplashWindowClass        = TEXT("SplashWindow");

// Month names, used by the Console logfile
CONST TCHAR*   szMonths[] = { TEXT("N/A"), TEXT("Jan"), TEXT("Feb"), TEXT("Mar"), TEXT("Apr"), TEXT("May"), TEXT("Jun"), TEXT("Jul"), TEXT("Aug"), TEXT("Sep"), TEXT("Oct"), TEXT("Nov"), TEXT("Dec") };

// Language icons
CONST TCHAR*   szLanguageIcons[LANGUAGES] = { TEXT("ENGLISH_ICON"), TEXT("FRENCH_ICON"), TEXT("GERMAN_ICON"), TEXT("ITALIAN_ICON"), TEXT("POLISH_ICON"), TEXT("RUSSIAN_ICON"), TEXT("SPANISH_ICON")  };

// OnException: Display+Print
#define    ON_EXCEPTION()    displayException(pException);

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                  CREATION  /  DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : getMainWindowData
// Description     : Global access to the BearScript window data.
// 
// Return Value   : Main window data
// 
MAIN_WINDOW_DATA*  getMainWindowData()
{
   return (getAppWindow() ? getMainWindowData(getAppWindow()) : NULL);
}


/// Function name  : setMainWindowAccelerators
// Description     : Activates new accelerators
// 
// const TCHAR*  szID   : [in] Resource ID
// 
// Return Value   : Previous window accelerators
// 
HACCEL  setMainWindowAccelerators(MAIN_WINDOW_DATA*  pWindowData, const TCHAR*  szID)
{
   HACCEL  hPrev = pWindowData->hAccelerators;

   // Load new accelerators
   pWindowData->hAccelerators = LoadAccelerators(getResourceInstance(), szID);
   return hPrev;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : displayApplication
// Description     : Create main window + Launch the GAME DATA thread.
// 
// INT            nCmdShow   : [in]  Initial display mode
// ERROR_STACK*  &pError     : [out] Error message, if any
// 
// Return type : TRUE if successful, otherwise FALSE
//
BOOL   displayApplication(INT  nCmdShow, ERROR_STACK*  &pError)
{
   HWND  hMainWnd;

   // Prepare
   pError = NULL;

   /// Create main window
   if (hMainWnd = createMainWindow())
   {
      /// Display main window
      ShowWindow(hMainWnd, getAppPreferences()->bMainWindowMaximised ? SW_MAXIMIZE : SW_SHOWNORMAL);
      UpdateWindow(hMainWnd);

      // [VALID GAME FOLDER] Load GameData
      if (getAppPreferences()->eGameFolderState == GFS_VALID)
         commandLoadGameData(getMainWindowData(), NULL);

      // [NO GAME DATA] "No game data was found in the folder '%s', please check your game folder is correct by clicking 'Preferences' in View menu"
      else if (getAppState() != AS_FIRST_RUN AND lstrlen(getAppPreferences()->szGameFolder))
         printMessageToOutputDialogf(ODI_INFORMATION, IDS_INIT_NO_GAME_DATA, getAppPreferences()->szGameFolder);
         
      // [NO GAME FOLDER] "You have not yet specified a game data folder, please do so by clicking 'Preferences' in the View menu"
      else if (getAppState() != AS_FIRST_RUN)
         printMessageToOutputDialogf(ODI_INFORMATION, IDS_INIT_NO_GAME_FOLDER);

      // [FIRST RUN] "Program is being run for the first time, please enter your game folder when asked"
      else
         printMessageToOutputDialogf(ODI_INFORMATION, IDS_GENERAL_FIRST_RUN);
   }
   else
      // [ERROR] "There was an error while creating the main X-Studio window"
      pError = generateDualError(HERE(IDS_INIT_MAIN_WINDOW_FAILED));
   
   // Return TRUE if successful, otherwise FALSE
   return (pError == NULL);
}


/// Function name  : displayLanguageDialog
// Description     : Requests APP LANGUAGE, GAME LANGUAGE, GAME FOLDER and updates the preferences
// 
// ERROR_STACK*  &pError : [out] Error message, if any
//
// Return Value   : TRUE if successful, otherwise FALSE
// 
BOOL   displayLanguageDialog(ERROR_STACK*  &pError)
{
   APP_LANGUAGE  eAppLanguage;      // Used selected application language
  
   // [VERBOSE]
   CONSOLE_COMMAND();

   // Prepare
   pError = NULL;

   /// Display 'Select Language' dialog
   eAppLanguage = displayFirstRunDialog(GetDesktopWindow());

   /// Save to Preferences + Set Thread language
   setAppPreferencesLanguages(eAppLanguage, convertAppLanguageToGameLanguage(eAppLanguage));
   setThreadLanguage(eAppLanguage);

   //// [LANGUAGE CHANGE] Reload resource library
   //if (eAppLanguage != AL_ENGLISH)
   //{
   //   // Unload existing resource library
   //   unloadResourceLibrary();

   //   // Attempt to load desired language
   //   if (!loadResourceLibrary(eAppLanguage, pError))
   //      // [ERROR] Chosen library missing
   //      return FALSE;     
   //}

   // [SUCCESS] Return TRUE
   return TRUE;
}


/// Function name  : displayHelp
// Description     : Display a page from the application helpfile. Launches the help window if necessary
// 
// CONST TCHAR*  szPageName : [in] Filename of the page within the application helpfile to be displayed.
// 
// Return Value   : TRUE if succesful, FALSE otherwise
// 
BOOL  displayHelp(CONST TCHAR*  szPageName)
{
   HWND    hHelpWnd;    // Window handle of help window
   TCHAR*  szFullPath;  // Filepath of Help file
          
   // Prepare
   szFullPath = utilCreateEmptyPath();

   // Get current folder
   GetModuleFileName(NULL, szFullPath, MAX_PATH);
   PathFindFileName(szFullPath)[0] = NULL;
   
   // Generate help command
   utilStringCchCatf(szFullPath, MAX_PATH, TEXT("X-Studio.chm::/%s.htm"), szPageName);

   // Launch help file
   hHelpWnd = HtmlHelp(getAppWindow(), szFullPath, HH_DISPLAY_TOPIC, NULL);

   // Cleanup and return
   utilDeleteString(szFullPath);
   return (hHelpWnd != NULL);
}


/// Function name  : initApplication
// Description     : Load the appliaction preferences and the appropriate resource library. Display the first
//                     run dialog if appropriate and ask the user for the location of the game data folder.
// 
// HINSTANCE     hInstance    : [in]  Application instance
// ERROR_QUEUE*  pErrorQueue  : [out] Error message, if any
// 
// Return Value   : TRUE if succesful, FALSE if there were errors
// 
BOOL   initApplication(ERROR_STACK*  &pError)
{
   INITCOMMONCONTROLSEX  oCommonControls;       // Common controls initialisation object
   PREFERENCES_STATE     ePreferencesState;     // Current state of application preferences
   APP_LANGUAGE          eLanguage;             // Interface language
   BOOL                  bResult;               // Operation result
   
   TRY
   {
      // [VERBOSE]
      CONSOLE_COMMAND();

      // Prepare
      pError = NULL;
      oCommonControls.dwSize = sizeof(INITCOMMONCONTROLSEX);
      oCommonControls.dwICC  = ICC_STANDARD_CLASSES WITH ICC_WIN95_CLASSES WITH ICC_USEREX_CLASSES WITH ICC_LINK_CLASS WITH ICC_PROGRESS_CLASS;
      
      // [DEBUG] Set VERBOSE mode to ALWAYS ON
      setAppVerboseMode(TRUE);

      /// Determine interface language 
      if (!getAppPreferencesLanguage(&eLanguage))
         // [FAILED] Default to ENGLISH
         eLanguage = AL_ENGLISH;

      /// Set thread language
      setThreadLanguage(eLanguage);

      /// Load relevant resource library
      if (loadResourceLibrary(eLanguage, pError))
      {
         // Create ImageTrees and load dialog font
         loadAppImageTrees();
         loadAppDialogFonts();

         /// [CHECK] Read preferences from the registry
         switch (ePreferencesState = loadAppPreferences(eLanguage, pError))
         {
         /// [NEWER VERSION] Abort...
         case PS_ERROR:      
            return FALSE;

         // [NORMAL] Verify game data folder state from current contents
         case PS_SUCCESS:    
            verifyInitialGameFolderState(); 

            /// Set initial game data state
            setAppState(AS_NO_GAME_DATA);
            break;

         // [UPGRADE] "Preferences from a previous version of X-Studio were detected and destroyed"
         case PS_UPGRADED:   
            displayMessageDialogf(GetDesktopWindow(), IDS_GENERAL_PREFERENCES_UPGRADED, MDF_OK WITH MDF_INFORMATION);   
            // Fall through...

         // [FIRST RUN] Query user for display langage and game data folder
         case PS_FIRST_RUN:
            if (!displayLanguageDialog(pError)) 
               /// [RESOURCES MISSING] Abort...
               return FALSE;
            
            /// Set first run state
            setAppState(AS_FIRST_RUN);

            // Save preferences. (Stores initial language choice and copies ColourSchemes into the registry for use during PreferencesDialog)
            saveAppPreferences();
         }

         // [FORUM USERNAME] Add to console.log
         if (lstrlen(getAppPreferences()->szForumUserName))
            CONSOLE_HEADING("X-Forums Username: %s", getAppPreferences()->szForumUserName);
         
         /// Init Common Controls library
         ERROR_CHECK("Initialising Common Controls library", bResult = InitCommonControlsEx(&oCommonControls));
         if (!bResult)
         {
            // [ERROR] "There was an error while initialising the common controls library"
            pError = generateDualError(HERE(IDS_INIT_COMMON_CONTROLS_FAILED));
            return FALSE;
         }
         
         /// Register window classes
         ERROR_CHECK("Registering windows classes", bResult = registerAppWindowClasses(pError) AND registerControlsWindowClasses(pError));
         if (!bResult)
         {
            // [ERROR]  "There was an error while registering the necessary window classes"
            enhanceError(pError, HERE(IDS_INIT_WINDOW_CLASSES_FAILED));
            return FALSE;
         }
         
         /// Initialise COM thread
         ERROR_CHECK("Initialising COM thread", SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)));

         /// Load RichEdit DLL
         ERROR_CHECK("Loading RichEdit Library", bResult = loadRichEditLibrary());
         if (!bResult)
         {
            // [ERROR] "There was an error while attempting to the system RichEdit dynamic link library. Please check 'RichEd20.DLL' is available"
            pError = generateDualError(HERE(IDS_INIT_RICH_EDIT_FAILED));
            return FALSE;
         }
         
         /// Initialise HTML Help
         ERROR_CHECK("Initialising HTML Help thread", loadHelpLibrary());      

         /// Load system ImageList and insert GameVersion/DocumentType icons
         ERROR_CHECK("Initialising the system ImageList", loadSystemImageListIcons());

         // [DEBUG] Validate heap
         ERROR_CHECK("Validating heap", utilValidateMemory());
      }

      // Return TRUE if there were no errors
      return (pError == NULL);
   }
   CATCH0("");
   return FALSE;
}


/// Function name  : initApplicationLogFile
// Description     : Prints the header of the log file
// 
// 
VOID  initApplicationLogFile()
{
   SYSTEMTIME     oCurrentTime;  // Current time/date in GMT
   TCHAR         *szDate,        // Date string 
                 *szVersion;

   // Prepare
   GetSystemTime(&oCurrentTime);

   /// Destroy any previous logfile
   deleteConsoleLogFile();

   /// Print version header
   CONSOLE("=============================");
   CONSOLE(" X-Studio v1.08  ");
   CONSOLE("=============================");

   // Generate date
   szDate = utilCreateStringf(128, TEXT("Generated at %02uh:%02um on %u %s %u"), oCurrentTime.wHour, oCurrentTime.wMinute, oCurrentTime.wDay, szMonths[oCurrentTime.wMonth], oCurrentTime.wYear);

   /// Print date and windows version
   consolePrint(szDate);
   CONSOLE("Operating System: %s", (szVersion = utilGetWindowsVersionString()));

   // Cleanup
   utilDeleteStrings(szDate, szVersion);
}


/// Function name  : parseCommandLineSwitches
// Description     : Handles the command line switches
// 
// Return Value   : TRUE to continue execution, FALSE to abort the program
// 
BOOL  parseCommandLineSwitches()
{
   CONST TCHAR  *szCommandLine,     // App command line
                *szAppName;         // Position in the command line following the module name (more or less)

   // Prepare
   szCommandLine = GetCommandLine();

   // Begin searching for command line switches after the application name
   if (szAppName = utilFindSubStringI(szCommandLine, ".exe"))
   {
      // [CHECK] Does the command line include the '-HELP' switch?
      if (utilFindSubStringI(szAppName, "-help"))
      {
         /// [HELP] Display supported switches and abort program
         MessageBox(GetDesktopWindow(), TEXT("The following command line switches are supported:\n\n-verbose : Outputs extra details to the 'Console.log' file\n-reset : Removes any application preferences stored in the registry"), TEXT("X-Studio"), MB_OK WITH MB_ICONINFORMATION);
         return FALSE; 
      }

      // [CHECK] Does the command line include the '-VERBOSE' switch?
      if (utilFindSubStringI(szAppName, "-verbose"))
         /// [VERBOSE MODE] Output extra detail to the console
         setAppVerboseMode(TRUE);

      // [CHECK] Does the command line include the '-RESET' switch?
      if (utilFindSubStringI(szAppName, "-reset"))
      {
         /// [DESTROY PREFERENCES] Destroy any existing application preferences
         deleteAppPreferencesRegistryKey();
         MessageBox(GetDesktopWindow(), TEXT("Any application preferences stored in the registry have been destroyed"), TEXT("X-Studio"), MB_OK WITH MB_ICONINFORMATION);
         return FALSE;
      }
   }

   // [SUCCESS] Continue program execution
   return TRUE;
}


/// Function name  : registerAppWindowClasses
// Description     : Registers the various window classes used by the app
//
// ERROR_STACK*  &pError    : [out] Error message, if any
//
// Return type : TRUE if successful, FALSE if not
//
BOOL  registerAppWindowClasses(ERROR_STACK*  &pError)
{
   WNDCLASS   oWndClass;      // window class

   // Prepare
   pError = NULL;

   /// [CLASS] Documents Control
   GetClassInfo(NULL, WC_TABCONTROL, &oWndClass);
   oWndClass.cbWndExtra   += sizeof(DOCUMENTS_DATA*);
   oWndClass.lpszClassName = szDocumentsCtrlClass;
   oWndClass.lpfnWndProc   = wndprocDocumentsCtrl;
   oWndClass.hCursor       = LoadCursor(NULL, IDC_ARROW);

   // Attempt to register class
   if (!RegisterClass(&oWndClass))
   {
      // [ERROR] "Could not register the window class for the %s window"
      pError = generateDualError(HERE(IDS_INIT_REGISTER_CLASS_FAILED), TEXT("Documents control"));
      return FALSE;  // [FAILED] Return FALSE
   }
   
   /// [CLASS] Operation pool window
   GetClassInfo(NULL, PROGRESS_CLASS, &oWndClass);
   oWndClass.cbWndExtra   += sizeof(OPERATION_POOL*);
   oWndClass.lpszClassName = szOperationPoolClass;

   // Attempt to register class
   if (!RegisterClass(&oWndClass))
   {
      // [ERROR] "Could not register the window class for the %s window"
      pError = generateDualError(HERE(IDS_INIT_REGISTER_CLASS_FAILED), TEXT("Operations pool"));
      return FALSE;  // [FAILED] Return FALSE
   }

   ///// [CLASS] Arguments Property page dialog In-place Edit control
   //GetClassInfo(NULL, WC_EDIT, &oWndClass);
   //oWndClass.lpszClassName = szArgumentsLabelEditClass;
   //oWndClass.lpfnWndProc   = wndprocArgumentPageEditCtrl;

   //// Attempt to register class
   //if (!RegisterClass(&oWndClass))
   //{
   //   // [ERROR] "Could not register the window class for the %s window"
   //   pError = generateDualError(HERE(IDS_INIT_REGISTER_CLASS_FAILED), TEXT("Arguments dialog Edit label control"));
   //   return FALSE;  // [FAILED] Return FALSE
   //}

   ///// [CLASS] Arguments Property page dialog In-place ComboBox control
   //GetClassInfo(NULL, WC_COMBOBOX, &oWndClass);
   //oWndClass.lpszClassName = szArgumentsLabelComboClass;
   //oWndClass.lpfnWndProc   = wndprocArgumentPageComboCtrl;

   //// Attempt to register class
   //if (!RegisterClass(&oWndClass))
   //{
   //   // [ERROR] "Could not register the window class for the %s window"
   //   pError = generateDualError(HERE(IDS_INIT_REGISTER_CLASS_FAILED), TEXT("Arguments dialog ComboBox label control"));
   //   return FALSE;  // [FAILED] Return FALSE
   //}

   /// [CLASS] FileDialog Edit Ctrl
   GetClassInfo(NULL, WC_EDIT, &oWndClass);
   oWndClass.style        |= CS_GLOBALCLASS;
   oWndClass.lpszClassName = szFileDialogEditClass;
   oWndClass.lpfnWndProc   = wndprocFileDialogFilenameEdit;
   oWndClass.hInstance     = getResourceInstance();

   // Attempt to register class
   if (!RegisterClass(&oWndClass))
   {
      // [ERROR] "Could not register the window class for the %s window"
      pError = generateDualError(HERE(IDS_INIT_REGISTER_CLASS_FAILED), TEXT("FileDialog Edit control"));
      return FALSE;  // [FAILED] Return FALSE
   }

   /// [CLASS] Splash Window
   utilZeroObject(&oWndClass, WNDCLASS);
   oWndClass.style         = CS_OWNDC;
   oWndClass.lpfnWndProc   = wndprocSplashWindow;
   oWndClass.cbWndExtra    = sizeof(SPLASH_WINDOW_DATA*);
   oWndClass.hInstance     = getAppInstance();
   oWndClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
   oWndClass.lpszClassName = szSplashWindowClass;

   // Attempt to register class
   if (!RegisterClass(&oWndClass))
   {
      // [ERROR] "Could not register the window class for the %s window"
      pError = generateDualError(HERE(IDS_INIT_REGISTER_CLASS_FAILED), TEXT("Splash window"));
      return FALSE;  // [FAILED] Return FALSE
   }

   /// [CLASS] Language Document
   utilZeroObject(&oWndClass, WNDCLASS);
   oWndClass.hInstance     = getAppInstance();
   oWndClass.lpfnWndProc   = wndprocLanguageDocument;
   oWndClass.cbWndExtra    = sizeof(LANGUAGE_DOCUMENT*);
   oWndClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
   oWndClass.lpszClassName = szLanguageDocumentClass;

   // Attempt to register class
   if (!RegisterClass(&oWndClass))
   {
      // [ERROR] "Could not register the window class for the %s window"
      pError = generateDualError(HERE(IDS_INIT_REGISTER_CLASS_FAILED), TEXT("Language Document"));
      return FALSE;  // [FAILED] Return FALSE
   }

   /// [CLASS] Script Document
   utilZeroObject(&oWndClass, WNDCLASS);
   oWndClass.hInstance     = getAppInstance();
   oWndClass.lpfnWndProc   = wndprocScriptDocument;
   oWndClass.cbWndExtra    = sizeof(SCRIPT_DOCUMENT*);
   oWndClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
   oWndClass.lpszClassName = szScriptDocumentClass;

   // Attempt to register class
   if (!RegisterClass(&oWndClass))
   {
      // [ERROR] "Could not register the window class for the %s window"
      pError = generateDualError(HERE(IDS_INIT_REGISTER_CLASS_FAILED), TEXT("Script Document"));
      return FALSE;  // [FAILED] Return FALSE
   }
   
   /// [CLASS] Main Window
   utilZeroObject(&oWndClass, WNDCLASS);
   oWndClass.style         = CS_OWNDC;
   oWndClass.lpfnWndProc   = wndprocMainWindow;
   oWndClass.cbWndExtra    = sizeof(MAIN_WINDOW_DATA*);
   oWndClass.hInstance     = getAppInstance();
   oWndClass.hIcon         = LoadIcon(getAppInstance(), szMainWindowClass);
   oWndClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
   oWndClass.lpszClassName = szMainWindowClass;

   // Attempt to register class
   if (!RegisterClass(&oWndClass))
      // [ERROR] "Could not register the window class for the %s window"
      pError = generateDualError(HERE(IDS_INIT_REGISTER_CLASS_FAILED), TEXT("Application"));
   
   // Return TRUE if there were no errors
   return (pError == NULL);
}


/// Function name  : unregisterAppWindowClasses
// Description     : Unregisters the application window classes
// 
VOID  unregisterAppWindowClasses()
{
   // Unregister windows classses
   UnregisterClass(szDocumentsCtrlClass,       getAppInstance());
   UnregisterClass(szOperationPoolClass,       getAppInstance());
   UnregisterClass(szArgumentsLabelEditClass,  getAppInstance());
   UnregisterClass(szArgumentsLabelComboClass, getAppInstance());
   UnregisterClass(szFileDialogEditClass,      getAppInstance());
   UnregisterClass(szLanguageDocumentClass,    getAppInstance());
   UnregisterClass(szMainWindowClass,          getAppInstance());
   UnregisterClass(szSplashWindowClass,        getAppInstance());
   UnregisterClass(szScriptDocumentClass,      getAppInstance());
}


/// Function name  : verifyInitialGameFolderState
// Description     : Updates the 'game folder state' to reflect whether it still contains game data or not
// 
// Return Value   : TRUE if game data is present, otherwise FALSE
// 
BOOL   verifyInitialGameFolderState()
{
   GAME_FOLDER_STATE  eNewFolderState;      // Current state of the game folder
   GAME_VERSION       eNewVersion,          // GameVersion identified from the current contents of the game folder
                      eVersionChoice;       // Encoded result from 'Select Game Version' dialog     (LOWORD = IDOK/IDCANCEL  HIWORD = GameVersion)
   BOOL               bResult;

   // Prepare
   bResult = FALSE;

   // [CHECK] Skip verification if no game folder is present
   if (lstrlen(getAppPreferences()->szGameFolder))
   {
      // Examine game folder contents
      switch (eNewFolderState = calculateGameFolderState(getAppPreferences()->szGameFolder, eNewVersion))
      {
      /// [NO GAME DATA] Generate a warning if game data was previously present. Update application state.
      case GFS_INVALID:
         // [CHECK] Was game data previously present?
         if (getAppPreferences()->eGameFolderState == GFS_VALID)
            // [WARNING] "Game data could no longer be found in the folder '%s'"
            displayMessageDialogf(GetDesktopWindow(), IDS_GENERAL_GAME_DATA_LOST, MDF_OK WITH MDF_INFORMATION, getAppPreferences()->szGameFolder);

         // Set state to INVALID
         setAppPreferencesGameFolderState(GFS_INVALID, GV_UNKNOWN);
         break;

      /// [GAME DATA PRESENT] Generate a warning if game data was previously absent. Update application state.
      case GFS_VALID:
         // [CHECK] Was game data previously absent?
         if (getAppPreferences()->eGameFolderState == GFS_INVALID)
            // [WARNING] "New game data has now been found in the folder '%s'"
            displayMessageDialogf(GetDesktopWindow(), IDS_GENERAL_GAME_DATA_FOUND, MDF_OK WITH MDF_INFORMATION, getAppPreferences()->szGameFolder);

         // Set state to VALID
         setAppPreferencesGameFolderState(GFS_VALID, eNewVersion);
         break;

      /// [INCOMPLETE DETERMINATION] Query user if game data was previously absent. 
      ///                            (If previously present the user has already been asked the supply the version)
      case GFS_INCOMPLETE:
         // [CHECK] Was the game data previously absent?
         if (getAppPreferences()->eGameFolderState == GFS_INVALID)
         {
            // [SUCCESS] Display the 'Select Game Version' dialog
            eVersionChoice = displayGameVersionDialog(GetDesktopWindow(), eNewVersion == GV_ALBION_PRELUDE);

            /// [CHECK] Save choice, and update 'valid' state accordingly
            setAppPreferencesGameFolderState(eVersionChoice != GV_UNKNOWN ? GFS_VALID : GFS_INVALID, eVersionChoice);
         }
         break;
      }

      // Return TRUE if game folder state is now valid
      bResult = getAppPreferences()->eGameFolderState == GFS_VALID;
   }

   // Return result
   return bResult;
}


/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          ENTRY POINT
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : WinMain
// Description     : Application entry point
// 
INT WINAPI   wWinMain(HINSTANCE  hInstance, HINSTANCE  hPrevInstance, TCHAR*  lpCmdLine, INT  nCmdShow)
{
   MAIN_WINDOW_DATA*  pMainWindowData;    // Main window data
   ERROR_STACK*       pError;             // Loading error, if any
   HWND               hExistingWindow;    // Previous instance window handle
   MSG                oMsg;               // Message object
   BOOL               bResult;

   // Prepare
   SET_THREAD_NAME("User Interface");
   pError = NULL;

   // [CHECK] Ensure program is not already running
   if (hExistingWindow = FindWindow(szMainWindowClass, NULL))
   {
      SetForegroundWindow(hExistingWindow);
      return FALSE;
   }

   TRY
   {
#ifdef DEBUG_HELP  
      /// Initialise DebugHelp.dll
      SymInitialize(GetCurrentProcess(), NULL, TRUE);
      SymSetOptions(SYMOPT_LOAD_LINES);
#endif

      /// [APP] Create application
      createApplication(hInstance);

      // [CONSOLE] Start new console logfile
      initApplicationLogFile();

      // [COMMAND LINE] Parse the command line to check for special operation modes
      if (!parseCommandLineSwitches())
      {
         // [TERMINATE] Cleanup
         deleteApplication();
         return FALSE;
      }

      /// [DEBUG]
#ifdef ANDREW_WILDE
      deleteAppPreferencesRegistryKey();
#endif
      
      /// [INITIALISE] Initialise application and create main window
      if (!initApplication(pError) OR !displayApplication(nCmdShow, pError)) 
      {
         // [ERROR] Display error and return FALSE
         displayErrorMessageDialog(GetDesktopWindow(), pError, TEXT("Initialisation Error"), MDF_OK WITH MDF_ERROR);
         consolePrint(generateFlattenedErrorStackText(pError));

         // [TERMINATE] Cleanup
         unloadApplication();
         deleteApplication();
         return FALSE;
      }
   
      // Get main window data
      ASSERT(pMainWindowData = getMainWindowData());

      // [DEBUG]
      DEBUG_WINDOW("PRELOAD:Workspace",     pMainWindowData->hWorkspace);
      DEBUG_WINDOW("PRELOAD:DocumentsCtrl", pMainWindowData->hDocumentsTab);
      DEBUG_WINDOW("PRELOAD:OutputDlg",     pMainWindowData->hOutputDlg);
      DEBUG_WINDOW("PRELOAD:ProjectDlg",    pMainWindowData->hProjectDlg);
      DEBUG_WINDOW("PRELOAD:SearchDlg",     pMainWindowData->hSearchTabDlg);
      
      // Main message loop
      while (bResult = GetMessage(&oMsg, NULL, 0, 0))
      {
         // [CHECK] Ensure message isn't corrupted
         ASSERT(bResult != -1);

         /// [HELP THREAD] - Reroute to help
         if (HtmlHelp(NULL, NULL, HH_PRETRANSLATEMESSAGE, (DWORD_PTR)&oMsg))
            continue;

         // [MAIN WINDOW EXISTS]
         if (pMainWindowData)
         {
            /// [ACCELERATOR] Pass all accelerators to main window
            if (TranslateAccelerator(pMainWindowData->hMainWnd, pMainWindowData->hAccelerators, &oMsg))
               continue;

            /// [SEARCH DIALOG] Dialog keyboard interface
            else if (pMainWindowData->hSearchTabDlg AND IsDialogMessage(pMainWindowData->hSearchTabDlg, &oMsg))
               continue;
            
            /// [RESULT DIALOG] Dialog keyboard interface
            else if (pMainWindowData->hSearchTabDlg AND IsDialogMessage(identifyActiveResultsDialogHandle(pMainWindowData->hSearchTabDlg), &oMsg))
               continue;

            /// [OUTPUT DIALOG] - Translate accelerators and characters messages
            else if (pMainWindowData->hOutputDlg AND (TranslateAccelerator(pMainWindowData->hOutputDlg, pMainWindowData->hAccelerators, &oMsg) OR IsDialogMessage(pMainWindowData->hOutputDlg, &oMsg)))
               continue;

            /// [PROJECT DIALOG] - Translate accelerators and characters messages
            else if (pMainWindowData->hProjectDlg AND (TranslateAccelerator(pMainWindowData->hProjectDlg, pMainWindowData->hAccelerators, &oMsg) OR IsDialogMessage(pMainWindowData->hProjectDlg, &oMsg)))
               continue;

            /// [RICH-TEXT DIALOG] - Translate accelerators and characters messages
            else if (pMainWindowData->hRichTextDlg AND (TranslateAccelerator(pMainWindowData->hRichTextDlg, pMainWindowData->hAccelerators, &oMsg) OR IsDialogMessage(pMainWindowData->hRichTextDlg, &oMsg)))
               continue;

            /// [MODELESS PROPERTIES SHEET] - Translate accelerators and characters messages
            else if (pMainWindowData->hPropertiesSheet AND PropSheet_IsDialogMessage(pMainWindowData->hPropertiesSheet, &oMsg))
            {
               // [DOCUMENT-PROPERTIES SHEET CLOSED] Destroy dialog if user has closed via 'x' button
               if (isPropertiesDialogAlreadyClosed(pMainWindowData))
               {
                  destroyPropertiesDialog(pMainWindowData);
                  updateMainWindowToolBar(pMainWindowData);
               }
               continue;
            }

            /// [FIND DIALOG] - Translate accelerators and characters messages
            else if (pMainWindowData->hFindTextDlg AND IsDialogMessage(pMainWindowData->hFindTextDlg, &oMsg))
               continue;

            /// [MAIN WINDOW BEING DESTROYED] Zero local window data pointer before processing WM_DESTROY
            else if (oMsg.message == WM_DESTROY AND oMsg.hwnd == pMainWindowData->hMainWnd)
               pMainWindowData = NULL;
         }

         /// [MAIN WINDOW] - Translate / Dispatch
         TranslateMessage(&oMsg);
         DispatchMessage(&oMsg);
      }

      // Unregister WindowClasses
      unregisterAppWindowClasses();
      unregisterControlsWindowClasses();
   
      /// Cleanup Application data
      unloadApplication();
      deleteApplication();
      return (INT)oMsg.wParam;
   }
   CATCH0("");
   
   // [TERMINATE]
   return FALSE;
}

