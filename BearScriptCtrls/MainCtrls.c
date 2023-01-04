//
// MainCtrls.cpp  -- Controls library main file
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////

// Window classes
CONST TCHAR*   szCodeEditClass        = TEXT("CodeEditCtrl");
CONST TCHAR*   szCommentRatioCtrl     = TEXT("CommentRatioCtrl");
CONST TCHAR*   szGroupedListViewClass = TEXT("GroupedListViewCtrl");
CONST TCHAR*   szMessageHeaderClass   = TEXT("MessageHeaderCtrl");
CONST TCHAR*   szWorkspaceClass       = TEXT("WorkspaceWindow");

// Instance
ControlsAPI HINSTANCE      hControlsInstance = NULL;

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : registerControlsWindowClasses
// Description     : Register the Custom controls window classes
// 
// Return Type : TRUE if succesful, FALSE otherwise
//
ControlsAPI
BOOL  registerControlsWindowClasses(ERROR_STACK*  &pError)
{
   WNDCLASS      oWndClass;

   // Prepare
   pError = NULL;

   /// [CLASS] CodeEdit Control
   utilZeroObject(&oWndClass, WNDCLASS);
   oWndClass.hInstance      = getControlsInstance();
   oWndClass.lpszClassName  = szCodeEditClass;
   oWndClass.lpfnWndProc    = wndprocCodeEdit;
   oWndClass.style          = CS_GLOBALCLASS WITH CS_OWNDC WITH CS_DBLCLKS; 
   oWndClass.cbWndExtra     = sizeof(CODE_EDIT_DATA*);
   oWndClass.hCursor        = LoadCursor(NULL, IDC_IBEAM);
   
   // Attempt to register class
   if (!RegisterClass(&oWndClass))
   {
      // [ERROR] "Could not register the window class for the %s window"
      pError = generateDualError(HERE(IDS_INIT_REGISTER_CLASS_FAILED), TEXT("CodeEdit control"));
      return FALSE;  // [FAILED] Return FALSE
   }

   /// [CLASS] Comment ratio control
   utilZeroObject(&oWndClass, WNDCLASS);
   oWndClass.hInstance     = getResourceInstance();    // Assign to the resource DLL
   oWndClass.lpszClassName = szCommentRatioCtrl;
   oWndClass.lpfnWndProc   = wndprocCommentRatioCtrl;
   oWndClass.style         = CS_GLOBALCLASS;
   oWndClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
   
   // Attempt to register class
   if (!RegisterClass(&oWndClass))
   {
      // [ERROR] "Could not register the window class for the %s window"
      pError = generateDualError(HERE(IDS_INIT_REGISTER_CLASS_FAILED), TEXT("Comment Ratio control"));
      return FALSE;  // [FAILED] Return FALSE
   }

   /// [CLASS] Grouped ListView
   GetClassInfo(NULL, WC_LISTVIEW, &oWndClass);
   oWndClass.hInstance     = getResourceInstance();    // Assign to resource DLL
   oWndClass.lpszClassName = szGroupedListViewClass;
   oWndClass.lpfnWndProc   = wndprocGroupedListView;
   oWndClass.cbWndExtra    = 8;
   //oWndClass.style      ^= CS_VREDRAW WITH CS_HREDRAW;
   //oWndClass.style      |= CS_GLOBALCLASS;
   
   // Attempt to register class
   if (!RegisterClass(&oWndClass))
   {
      // [ERROR] "Could not register the window class for the %s window"
      pError = generateDualError(HERE(IDS_INIT_REGISTER_CLASS_FAILED), TEXT("GroupedListView control"));
      return FALSE;  // [FAILED] Return FALSE
   }

   /// [CLASS] Workspace Window
   utilZeroObject(&oWndClass, WNDCLASS);
   oWndClass.style         = CS_HREDRAW WITH CS_VREDRAW;
   oWndClass.lpfnWndProc   = wndprocWorkspace;
   oWndClass.cbWndExtra    = sizeof(WORKSPACE_DATA*);
   oWndClass.hInstance     = getControlsInstance();
   oWndClass.lpszClassName = szWorkspaceClass;

   // Attempt to register class
   if (!RegisterClass(&oWndClass))
      // [ERROR] "Could not register the window class for the %s window"
      pError = generateDualError(HERE(IDS_INIT_REGISTER_CLASS_FAILED), TEXT("Workspace window"));
   
   // Return TRUE if there were no errors
   return (pError == NULL);
}


/// Function name  : unregisterControlsWindowClasses
// Description     : Unregisters the Custom controls window classes
// 
ControlsAPI
VOID  unregisterControlsWindowClasses()
{
   // Unregister windows classses
   UnregisterClass(szCodeEditClass,        getControlsInstance());
   UnregisterClass(szCommentRatioCtrl,     getControlsInstance());
   UnregisterClass(szGroupedListViewClass, getControlsInstance());
   UnregisterClass(szWorkspaceClass,       getControlsInstance());
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : getControlsInstance
// Description     : Access to the custom controls library instance
// 
// Return Value   : Instance of the CustomControls library
// 
ControlsAPI 
HINSTANCE  getControlsInstance()
{
   return hControlsInstance;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     ENTRY POINT
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : DllMain
// Description     : DLL Entry point
//
// HMODULE  hModule    : [in] Instance of controls module
// DWORD    dwPurpose  : [in] Action
// LPVOID   lpReserved : [in] 
// 
// Return type : TRUE
//
BOOL APIENTRY   DllMain(HMODULE  hModule, DWORD  dwPurpose, LPVOID  lpReserved)
{
   switch (dwPurpose)
   {
   // [PROCESS LOAD] -- Register CodeEdit window class
   case DLL_PROCESS_ATTACH:
      hControlsInstance = hModule;
      //registerControlWindowClasses(hModule);
      break;

   // [THREAD LOAD/UNLOAD]
   case DLL_THREAD_ATTACH:
   case DLL_THREAD_DETACH:
      break;

   // [PROCESS UNLOAD] -- Unregister CodeEdit window class
   case DLL_PROCESS_DETACH:
      //UnregisterClass(szCodeEditClass, hModule);   // MSDN: "Windows NT/2000/XP: No window classes registered by a DLL registers are unregistered when the .dll is unloaded."
      hControlsInstance = NULL;
      break;
   }

   return TRUE;
}

