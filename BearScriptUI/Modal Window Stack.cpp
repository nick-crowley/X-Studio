//
// Modal Dialog Stack.cpp : Ensures that only the top floating window is ever enabled, creating a modal illusion
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

//
// THIS IDEA NEEDS TO BE REWORKED, IT DOESN'T PROPERLY REFLECT THE FACT THAT THE TOP
// MODAL WINDOW IS NOT ALWAYS THE ONE THAT NEEDS TO BE REMOVED
// 
// A SEARCH FUNCTION TO REMOVE WINDOWS WITHIN THE STACK BY HANDLE WOULD BE USEFUL
//

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : popModalWindowStack
// Description     : Removes a window from the stack, enabling the window directly beneath it
// 
HWND   topModalWindowStack()
{
   MAIN_WINDOW_DATA*  pMainWindowData;    // Main window data
   HWND               hTopWindow;          // Window directly beneath the top item

   // Prepare
   hTopWindow = NULL;

   // [CHECK] Ensure main window exists
   if (pMainWindowData = getMainWindowData())
      /// [SUCCESS] Peek top item, if any
      hTopWindow = (HWND)topStackObject(pMainWindowData->pWindowStack);
   
   // Return item
   return hTopWindow;
}


/// Function name  : pushModalWindowStack
// Description     : Adds a new window to the stack, disabling those beneath it
// 
// HWND   hNewWnd : [in] Window to add
// 
VOID    pushModalWindowStack(HWND  hNewWnd)
{
   MAIN_WINDOW_DATA*  pMainWindowData;
   //HWND               hTopWnd;

   // [CHECK] Window must be enabled when added
   ASSERT(IsWindowEnabled(hNewWnd));

   // [CHECK] Ensure main window exists
   if (pMainWindowData = getMainWindowData())
   {
      //// Get current top window
      //if (hTopWnd = (HWND)topStackObject(pMainWindowData->pWindowStack))
      //{
      //   // [SPECIAL CASE] Ensure we're not adding the properties dialog
      //   if (hNewWnd != pMainWindowData->hPropertiesSheet)
      //   {
      //      /// Disable current top window and main window
      //      EnableWindow(hTopWnd, FALSE);
      //      EnableWindow(pMainWindowData->hMainWnd, FALSE);    // 
      //   }
      //}

      /// Push new window onto stack
      pushObjectOntoStack(pMainWindowData->pWindowStack, (LPARAM)hNewWnd);
   }
}


/// Function name  : popModalWindowStack
// Description     : Removes a window from the stack, enabling the window directly beneath it
// 
VOID   popModalWindowStack()
{
   MAIN_WINDOW_DATA*  pMainWindowData;    // Main window data
   //HWND               hLowerWnd;          // Window directly beneath the top item

   // [CHECK] Ensure main window exists
   if (pMainWindowData = getMainWindowData())
   {
      // [CHECK] Stack must not be empty
      ASSERT(getStackItemCount(pMainWindowData->pWindowStack) > 0);

      /// Remove top item and discard
      popObjectFromStack(pMainWindowData->pWindowStack);

      //// Enable next window, if any
      //if (hLowerWnd = (HWND)topStackObject(pMainWindowData->pWindowStack))
      //{
      //   /// [FOUND] Enable next window
      //   EnableWindow(hLowerWnd, TRUE);

      //   // [SPECIAL CASE] Also enable main window if we're enabling the documents dialog
      //   if (hLowerWnd == pMainWindowData->hPropertiesSheet)
      //      EnableWindow(pMainWindowData->hMainWnd, TRUE);
      //}
   }
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////


