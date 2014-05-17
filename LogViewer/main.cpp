// LogViewer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "LogViewer.h"

// Import Common controls v6.0
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////

// Global
Application   Application::m_oApp;

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                 CONSTRUCTION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

Application&  getApp()     
{ 
   return Application::m_oApp;      
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       METHODS
/// /////////////////////////////////////////////////////////////////////////////////////////

bool   Application::Create(HINSTANCE  hInstance)  
{ 
   INITCOMMONCONTROLSEX  oCommonCtrls;

   // Store instance
   m_hInstance = hInstance;  

   /// Register Common Control Windows Classes
   oCommonCtrls.dwSize = sizeof(INITCOMMONCONTROLSEX);
   oCommonCtrls.dwICC  = ICC_WIN95_CLASSES | ICC_LISTVIEW_CLASSES;
   InitCommonControlsEx(&oCommonCtrls);

   // [SUCCESS]
   return true;
}


void   Application::Destroy()
{
}


int APIENTRY _tWinMain(HINSTANCE  hInstance, HINSTANCE  hPrevInstance, LPTSTR  szCmdLine, int  iDisplayMode)
{
   int              iReturn = 0;
   OSVERSIONINFO    oVersion;      // OS version

   // Prepare
   oVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
   
   try
   {
      // Initialise application
      if (getApp().Create(hInstance))
      {
         MainWindow  oMainWindow(hInstance);
         Rect        rcInitial = Rect(0,0,1000,600) + Point(300,200);

         // Create + Display Main Window      
         oMainWindow.Create(TEXT("X-Studio Console Viewer"), &rcInitial, hInstance);
         oMainWindow.ShowAndUpdate(iDisplayMode);

         // [WINDOWS 7]
         if (GetVersionEx(&oVersion) && oVersion.dwMajorVersion >= 6)
         {
            // Enable Windows 7 Drag'n'Drop
            ChangeWindowMessageFilter(WM_DROPFILES, MSGFLT_ADD);
            ChangeWindowMessageFilter(WM_COPYDATA, MSGFLT_ADD);
            ChangeWindowMessageFilter(WM_COPYGLOBALDATA, MSGFLT_ADD);
         }
         
         // [ARGUMENTS] Load input file
         if (lstrlen(szCmdLine))
         {
            StrTrim(szCmdLine, TEXT("\""));
            oMainWindow.ReadLog(szCmdLine);
         }
     
         // Message loop
         iReturn = getApp().Pump(&oMainWindow);

         // Cleanup
         getApp().Destroy();
      }
   }
   catch (WindowsException&  e)
   {
      e.Display(HERE, NULL);
   }

   return iReturn;
}

