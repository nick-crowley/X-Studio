//
// Debugging.cpp : Debugging helpers
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        MACROS
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////



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

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////


