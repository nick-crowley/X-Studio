//
// Console.cpp -- Provides a debugging output window
// 
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 
#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////

// Console Edit control ID
#define        IDC_CONSOLE_EDIT     100  

// Max length of a console string
#define        CONSOLE_LENGTH       8192  

// Window class
CONST TCHAR    *szConsoleClass = TEXT("DebugConsole");

// Log FileName
#ifndef _DEBUG
CONST TCHAR*  szLogFileName = TEXT("Console.log");
#else
CONST TCHAR*  szLogFileName = TEXT("DebugConsole.log");
#endif

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createConsole
// Description     : Registers the console window class and creates the console window
// 
// Return Value   : Window handle to the new console window if successful, otherwise NULL
// 
HWND   createConsole()
{
   WNDCLASS    oWndClass;     // Window class
   HWND        hWnd;          // Operation result

   // Prepare
   utilZeroObject(&oWndClass, WNDCLASS);
   hWnd = NULL;

   // Register window class
   oWndClass.hInstance     = getAppInstance();
   oWndClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
   oWndClass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
   oWndClass.lpfnWndProc   = wndprocConsole;
   oWndClass.lpszClassName = szConsoleClass;
   oWndClass.style         = CS_HREDRAW WITH CS_VREDRAW;

   /// Attempt to register console window class
   if (RegisterClass(&oWndClass))
   {
      /// Create console window  (invisible)
      hWnd = CreateWindow(szConsoleClass, TEXT("Console"), WS_OVERLAPPED WITH WS_THICKFRAME WITH WS_CAPTION WITH WS_MAXIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 300, 500, NULL, NULL, getAppInstance(), NULL);
      ERROR_CHECK("creating console window", hWnd);

      // Update
      UpdateWindow(hWnd);
   }

   // Return new handle
   return hWnd;
}


/// Function name  : createConsoleData
// Description     : Creates ConsoelData used by the application object
// 
// Return Value   : New ConsoleData, you are responsible for destroying it
// 
CONSOLE_DATA*   createConsoleData()
{
   CONSOLE_DATA*  pConsole;

   // Create object
   pConsole  = utilCreateEmptyObject(CONSOLE_DATA);

   // Initialise CriticalSection
   InitializeCriticalSection(&pConsole->oCriticalSection);

   // Return object
   return pConsole;
}


/// Function name  : deleteConsole
// Description     : Destroys the console window
//
BearScriptAPI 
VOID   deleteConsole()
{
   CONSOLE_DATA*  pConsole;

   // Prepare
   pConsole = getAppConsole();

   if (pConsole->hWnd)
   {
      // Revert parent's system menu
      GetSystemMenu(GetParent(pConsole->hWnd), TRUE);

      // Destroy console window
      DestroyWindow(pConsole->hWnd);
   }
}


/// Function name  : deleteConsoleData
// Description     : Destroys ConsoleData
// 
// CONSOLE_DATA*  &pConsole   : [in] ConsoleData to destroy
// 
VOID   deleteConsoleData(CONSOLE_DATA*  &pConsole)
{
   // Cleanup CriticalSection
   DeleteCriticalSection(&pConsole->oCriticalSection);

   // Cleanup object
   utilDeleteObject(pConsole);
}


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                         HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : isConsoleVisible
// Description     : Determines whether the console is visible or not
// 
// Return Value   : TRUE/FALSE
// 
BearScriptAPI
BOOL   isConsoleVisible()
{
   return IsWindowVisible(getAppConsole()->hWnd);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                         FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : attachConsole
// Description     : Creates the console window and adds a controlling menu item to the specified window's system menu
//
// HWND  hParent    : [in] Window handle of the window to attach the console to
//
// Return type : TRUE if succesful, FALSE otherwise
//
BearScriptAPI 
BOOL   attachConsole(HWND  hParent)
{
   HMENU    hParentSystemMenu;    // Handle of parent's system menu
   HWND     hWnd;                 // Console window handle
   
   /// Attempt to create and store console window
   if (hWnd = createConsole())
   {
      // Store global handle
      getAppConsole()->hWnd = hWnd;

      // Add 'Show/Hide Console' to the parent window's system menu
      hParentSystemMenu = GetSystemMenu(hParent, FALSE);
      AppendMenu(hParentSystemMenu, MF_SEPARATOR, NULL, NULL);
      AppendMenu(hParentSystemMenu, MF_STRING, IDM_SHOW_HIDE_CONSOLE, TEXT("Show Console"));
   }

   // Return TRUE if successful
   return (hWnd != NULL);
}


/// Function name  : consoleVPrintf
// Description     : Outputs a string to the console window and log file
//
// CONST TCHAR*  szText : [in] string
// 
BearScriptAPI 
VOID   consolePrint(CONST TCHAR* szText)
{
   TCHAR*   szOutputTextW;    // UNICODE version of output text, for display
   CHAR*    szOutputTextA;    // ANSI version of output text, for log file
   HANDLE   hLogFile;         // File handle
   DWORD    dwBytesWritten;   // Number of bytes written
   UINT     iOutputLength;    // Length of output string after translation to ANSI

   // Append CRLF to output string and convert to ANSI
   szOutputTextW = utilCreateStringf(lstrlen(szText) + 32, TEXT("0x%04x: %s\r\n"), GetCurrentThreadId(), szText);

   /// [GUARD BLOCK]
   EnterCriticalSection(&getAppConsole()->oCriticalSection);

   /// Attempt to open Console.log for writing
   if ((hLogFile = CreateFile(szLogFileName, FILE_ALL_ACCESS, NULL, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)) != INVALID_HANDLE_VALUE)
   {
      // Convert output to ANSI
      iOutputLength = lstrlen(szOutputTextW);
      szOutputTextA = utilTranslateStringToANSI(szOutputTextW, iOutputLength);

      /// Append ANSI string to the end of the file
      SetFilePointer(hLogFile, 0, 0, FILE_END);
      WriteFile(hLogFile, szOutputTextA, iOutputLength, &dwBytesWritten, NULL);

      // Cleanup
      utilCloseHandle(hLogFile);
      utilDeleteObject(szOutputTextA);
   }

   /// [GUARD BLOCK] 
   LeaveCriticalSection(&getAppConsole()->oCriticalSection);

   // [CHECK] Does console window exist yet?
   if (IsWindow(getAppConsole()->hWnd))
      /// [SUCCESS] Pass UNICODE version to console window
      PostMessage(getAppConsole()->hWnd, UM_CONSOLE, NULL, (LPARAM)szOutputTextW);
   else
      // [FAILURE] Cleanup
      utilDeleteString(szOutputTextW);
}



/// Function name  : consolePrintError
// Description     : Outputs an ErrorStack to the console
//
// CONST ERROR_STACK*  pError : [in] 
// 
BearScriptAPI 
VOID   consolePrintError(CONST ERROR_STACK*  pError)
{
   TCHAR*  szErrorText;

   // [VERBOSE] Print error stack to console
   if (pError->eType != ET_INFORMATION)
   {
      // Output flattened error text
      consolePrint(szErrorText = generateFlattenedErrorStackText(pError));

      // Cleanup
      utilDeleteString(szErrorText);
   }
}


/// Function name  : consolePrintErrorQueue
// Description     : Outputs an ErrorQueue to the console
//
// CONST ERROR_QUEUE*  pErrorQueue : [in] 
// 
BearScriptAPI 
VOID   consolePrintErrorQueue(CONST ERROR_QUEUE*  pErrorQueue)
{
   TCHAR*  szErrorText;

   // Output flattened error text
   consolePrint(szErrorText = generateFlattenedErrorQueueText(pErrorQueue));

   // Cleanup
   utilDeleteString(szErrorText);
}



/// Function name  : consolePrintf
// Description     : Outputs formatted variable argument strings to the console window
//
// CONST TCHAR*  szFormat : [in] Formatting string
// <any type>    ...      : [in] Arguments
// 
BearScriptAPI 
VOID   consolePrintf(CONST TCHAR* szFormat, ...)
{
   va_list  pArgList;       // First variable arg pointer
   
   // Prepare
   pArgList = utilGetFirstVariableArgument(&szFormat);
   
   // Add to console window
   consoleVPrintf(szFormat, pArgList);
}


/// Function name  : consolePrintfA
// Description     : Helper function for outputting formatting text to the console window through a macro
//
// CONST TCHAR*  szFormat : [in] ANSI Formatting string
// <any type>    ...      : [in] Wide Arguments
// 
BearScriptAPI 
VOID   consolePrintfA(CONST CHAR* szFormatA, ...)
{
   va_list  pArgList;       // First variable arg pointer
   TCHAR*   szFormatW;
   
   // Prepare
   pArgList = utilGetFirstVariableArgument(&szFormatA);

   // Translate formatting string to UNICODE
   szFormatW = utilTranslateStringToUNICODE(szFormatA, lstrlenA(szFormatA));
   
   // Add to console window
   consoleVPrintf(szFormatW, pArgList);
   
   // Cleanup
   utilDeleteString(szFormatW);
}


/// Function name  : consolePrintLastError
// Description     : Formats and prints the last system error to the console
//
// CONST TCHAR*  szActivity  : [in] Activity being performed
// CONST TCHAR*  szFunction  : [in] Function name
// CONST TCHAR*  szFile      : [in] File Name
// CONST UINT    iLineNumber : [in] Line number
// 
BearScriptAPI 
VOID   consolePrintLastError(CONST TCHAR*  szActivity, CONST TCHAR*  szFunction, CONST TCHAR*  szFile, CONST UINT  iLineNumber)
{
   // Use 'GetLastError()'
   consolePrintLastErrorEx(GetLastError(), szActivity, szFunction, szFile, iLineNumber);
}

/// Function name  : consolePrintLastErrorEx
// Description     : Formats and prints a system error to the console
//
// CONST DWORD   dwErrorCode : [in] Error code
// CONST TCHAR*  szActivity  : [in] Activity being performed
// CONST TCHAR*  szFunction  : [in] Function name
// CONST TCHAR*  szFile      : [in] File Name
// CONST UINT    iLineNumber : [in] Line number
// 
BearScriptAPI 
VOID   consolePrintLastErrorEx(CONST DWORD  dwErrorCode, CONST TCHAR*  szActivity, CONST TCHAR*  szFunction, CONST TCHAR*  szFile, CONST UINT  iLineNumber)
{
   TCHAR*  szSystemError;     // Error mesasge

   /// Convert error code to string
   szSystemError = utilCreateEmptyString(512);
   FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwErrorCode, NULL, szSystemError, 512, NULL);
   
   // Remove LF/CRLF and trailing full stop
   StrTrim(szSystemError, TEXT(" .\r\n"));

   /// Print to console
   consolePrintf(TEXT("Error while %s in %s on line %u of %s. GetLastError(%u) returned '%s'"), szActivity, szFunction, iLineNumber, szFile, dwErrorCode, szSystemError);

   // Cleanup
   utilDeleteString(szSystemError);
}


/// Function name  : consolePrintVerboseA
// Description     : Prints console output only if verbose mode is enabled
// 
// CONST CHAR*  szFormat  : [in] ANSI Formatting string, for use with macros
// <any type>    ...      : [in] Wide formatting Arguments
// 
BearScriptAPI
VOID  consolePrintVerboseA(CONST CHAR*  szFormat, ...)
{
   va_list  pArgList;

   // Prepare
   pArgList = utilGetFirstVariableArgument(&szFormat);

   // [CHECK] Is verbose output enabled?
   if (isAppVerbose())
      /// [VERBOSE] Print formatted output
      consoleVPrintfA(szFormat, pArgList);
}


/// Function name  : consolePrintVerboseHeadingA
// Description     : Prints verbose console output, followed by a small partition
// 
// CONST CHAR*  szFormat  : [in] ANSI Formatting string, for use with macros
// <any type>    ...      : [in] Wide formatting Arguments
// 
BearScriptAPI
VOID  consolePrintVerboseHeadingA(CONST CHAR*  szFormat, ...)
{
   va_list  pArgList;

   // Prepare
   pArgList = utilGetFirstVariableArgument(&szFormat);

   // [CHECK] Is verbose output enabled?
   if (isAppVerbose())
   {
      // [PARTITION]
      consolePrintfA(SMALL_PARTITION);

      /// [VERBOSE] Print formatted output
      consoleVPrintfA(szFormat, pArgList);
      consolePrintfA(SMALL_PARTITION);
   }
}


/// Function name  : consoleVPrintf
// Description     : Outputs formatted variable argument strings to the console window
//
// CONST TCHAR*  szFormat : [in] Formatting string
// va_list       pArgList : [in] Argument list
// 
BearScriptAPI 
VOID   consoleVPrintf(CONST TCHAR* szFormat, va_list  pArgList)
{
   TCHAR*   szOutputText;   // Output buffer

   // Prepare
   szOutputText = utilCreateEmptyString(CONSOLE_LENGTH);

   /// Assemble string
   StringCchVPrintf(szOutputText, CONSOLE_LENGTH, szFormat, pArgList);
   va_end(pArgList);

   /// Write to .log file and display
   consolePrint(szOutputText);

   // Cleanup
   utilDeleteString(szOutputText);
}


/// Function name  : consoleVPrintfA
// Description     : Outputs formatted variable argument strings to the console window
//
// CONST CHAR*   szFormat : [in] ANSI Formatting string
// va_list       pArgList : [in] Wide formatted Argument list
// 
BearScriptAPI 
VOID   consoleVPrintfA(CONST CHAR* szFormatA, va_list  pArgList)
{
   TCHAR   *szFormatW;     // UNICODE version of formatting string

   // Translate formatting string
   szFormatW = utilTranslateStringToUNICODE(szFormatA, lstrlenA(szFormatA));

   // Output to console
   consoleVPrintf(szFormatW, pArgList);

   // Cleanup
   utilDeleteString(szFormatW);
}


/// Function name  : deleteConsoleLogFile
// Description     : Deletes the 'Console.log' file from a previous instance
// 
BearScriptAPI  
VOID   deleteConsoleLogFile()
{
   TCHAR   *szLogFilePath;       // Path to console.log

   // Prepare
   szLogFilePath = generateConsoleLogFilePath();

   /// Delete Console.log
   DeleteFile(szLogFilePath);

   // Cleanup
   utilDeleteString(szLogFilePath);
}


/// Function name  : generateConsoleLogFilePath
// Description     : Generates the full path to the console log file
// 
// Return Value   : New string containing full path to console logfile
// 
BearScriptAPI
TCHAR*  generateConsoleLogFilePath()
{
    TCHAR   *szModulePath,        // Path of application
            *szLogFilePath;       // Path to console.log

   // Prepare
   szModulePath = utilCreateEmptyPath();

   // Generate full path of 'Console.log'
   GetModuleFileName(NULL, szModulePath, MAX_PATH);
   szLogFilePath = utilRenameFilePath(szModulePath, szLogFileName);

   // Cleanup and return path
   utilDeleteString(szModulePath);
   return szLogFilePath;
}


/// Function name  : showConsole
// Description     : Hides or shows the console window and alters the parent's system menu to reflect this change.
// 
// CONST BOOL  bShow : [in] TRUE to show, FALSE to hide
// 
BearScriptAPI 
VOID   showConsole(CONST BOOL  bShow)
{
   HMENU      hParentSystemMenu;
      
   // Set visibility
   ShowWindow(getAppConsole()->hWnd, bShow ? SW_SHOW : SW_HIDE);

   // Modify show/hide menu item to reflect current visibility
   hParentSystemMenu = GetSystemMenu(GetParent(getAppConsole()->hWnd), FALSE);
   ModifyMenu(hParentSystemMenu, IDM_SHOW_HIDE_CONSOLE, MF_STRING WITH MF_BYCOMMAND, IDM_SHOW_HIDE_CONSOLE, bShow ? TEXT("Hide Console") : TEXT("Show Console"));
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : wndprocConsole
// Description     : Console window procedure
//
// 
LRESULT   CALLBACK   wndprocConsole(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
   static HWND  hEditCtrl;          // Edit control containing the text
   UINT         iExistingLength;    // Current text length
   TCHAR*       szNewText;          // New input text

   // Examine message
   switch (iMessage)
   {
   /// [CREATION] -- Create edit window
   case WM_CREATE:
      hEditCtrl = CreateWindow(TEXT("EDIT"), NULL, WS_HSCROLL WITH WS_VSCROLL WITH WS_CHILD WITH WS_VISIBLE WITH ES_MULTILINE WITH ES_LEFT WITH ES_DISABLENOSCROLL WITH ES_AUTOVSCROLL WITH ES_AUTOHSCROLL WITH ES_READONLY, 
                               0, 0, CW_USEDEFAULT, CW_USEDEFAULT, hWnd, (HMENU)IDC_CONSOLE_EDIT, ((CREATESTRUCT*)lParam)->hInstance, NULL);
      Edit_LimitText(hEditCtrl, -1);
      break;

   /// [DESTRUCTION] -- Destroy edit window
   case WM_DESTROY:
      DestroyWindow(hEditCtrl);
      break;

   /// [TEXT INPUT] -- Append text to the console
   case UM_CONSOLE:
      // Prepare
      iExistingLength = GetWindowTextLength(hEditCtrl);
      szNewText       = (TCHAR*)lParam;

      // Extend existing text
      Edit_SetSel(hEditCtrl, iExistingLength, iExistingLength);   
      Edit_ReplaceSel(hEditCtrl, szNewText);

      // Cleanup
      utilDeleteString(szNewText);
      break;

   /// [VISUAL STYLE]
   case WM_CTLCOLORSTATIC:
   case WM_CTLCOLOREDIT:
   case WM_CTLCOLORDLG:
      SetTextColor((HDC)wParam, RGB(255,255,255));
      SetBkColor((HDC)wParam, NULL);
      return (LRESULT)GetStockObject(BLACK_BRUSH);

   /// [SIZE] -- Resize edit window to fill console window
   case WM_SIZE:
      MoveWindow(hEditCtrl, 0, 0, LOWORD(lParam), HIWORD(lParam), FALSE);
      break;

   // [DEFAULT]
   default:
      return DefWindowProc(hWnd, iMessage, wParam, lParam);
   }

   return 0;
}


