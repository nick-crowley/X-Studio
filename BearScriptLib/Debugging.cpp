//
// Debugging.cpp : Debugging helpers
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"
#include "Dbghelp.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        MACROS
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////

typedef  SYMBOL_INFO_PACKAGEW  SymbolInfo;

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

BearScriptAPI
VOID   debugQueryWindow(HWND  hWnd, CONST TCHAR*  szName, CONST BOOL  bIncludeParent)
{
#ifdef PRINT_WINDOW_PROPERTIES
   TCHAR  *szParent,
          *szTitle;
   RECT    rcWindow;

   // [CHECK] Does window exist?
   if (IsWindow(hWnd))
   {
      // Prepare
      GetWindowRect(hWnd, &rcWindow);

      // Output window properties
      CONSOLE("DEBUG: The window '%s' is %s/%s and has title '%s'. It is located at (%d,%d), (%d,%d)", szName, IsWindowVisible(hWnd) ? TEXT("Visible") : TEXT("Hidden"), IsWindowEnabled(hWnd) ? TEXT("Enabled") : TEXT("Disabled"), 
                                                                                                               szTitle = utilGetWindowText(hWnd), rcWindow.left, rcWindow.top, rcWindow.right, rcWindow.bottom);
      // [CHECK] Are we displaying the window parent?
      if (GetParent(hWnd) AND bIncludeParent)
      {
         debugQueryWindow(GetParent(hWnd), szParent = utilCreateStringf(64, TEXT("Parent of %s"), szName), FALSE);
         utilDeleteString(szParent);
      }

      // Cleanup
      utilDeleteString(szTitle);
   }
   else
      CONSOLE("DEBUG: The window '%s' does not exist", szName);
#endif
}



BearScriptAPI 
TCHAR*  generateStackTrace(HANDLE  hProcess)
{
   IMAGEHLP_LINEW64  oLine;
   SymbolInfo        oSymbol;
   void*             pFrames[63];
   int               iFrameCount;
   TCHAR*            szOutput = utilCreateEmptyString(2048);

   // Capture addresses on the Stack
   SymInitialize(hProcess, NULL, TRUE );
   SymSetOptions(SYMOPT_LOAD_LINES);
   iFrameCount = CaptureStackBackTrace(0, 63, pFrames, NULL);

   // Extract symbol names
   for(int i = 0; i < iFrameCount; i++ )
   {
      DWORD   dwDummy = NULL;

      // Prepare
      utilZeroObject(&oSymbol, SymbolInfo);
      utilZeroObject(&oLine, IMAGEHLP_LINEW64);
      oSymbol.si.MaxNameLen   = MAX_SYM_NAME;
      oSymbol.si.SizeOfStruct = sizeof(SYMBOL_INFOW);
      oLine.SizeOfStruct      = sizeof(IMAGEHLP_LINEW64);

      // Resolve symbol, Filename, LineNumber
      SymFromAddrW(hProcess, (DWORD64)(pFrames[i]), 0, &oSymbol.si);
      //SymGetLineFromAddrW64(hProcess, oSymbol.si.Address, &dwDummy, &oLine);

      // Append to trace
      //if (oLine.FileName AND utilFindCharacterReverse(oLine.FileName, '\\'))
      if (SymGetLineFromAddrW64(hProcess, oSymbol.si.Address, &dwDummy, &oLine))
         utilStringCchCatf(szOutput, 2048, TEXT("%s() in %s on line %d\r\n"), oSymbol.si.Name, utilFindCharacterReverse(oLine.FileName, '\\') + 1, oLine.LineNumber);

      // [MAIN ADDED] Abort
      if (utilCompareString(oSymbol.si.Name, "wWinMain"))
         break;
   }

   SymCleanup(hProcess);
   return szOutput;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////


