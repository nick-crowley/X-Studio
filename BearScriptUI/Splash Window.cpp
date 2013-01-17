//
// Splash Window.cpp : Displays the splash screen during load
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        MACROS
/// /////////////////////////////////////////////////////////////////////////////////////////


#define ARGB(a,r,g,b)          ((COLORREF)(((BYTE)(r) WITH ((WORD)((BYTE)(g))<<8)) WITH (((DWORD)(BYTE)(b))<<16) WITH (((DWORD)(BYTE)(a))<<24)))

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////

// Defines the size of the splash bitmap
CONST SIZE       siLogoBitmap       = { 410, 380 };

// Defines the colour of the progress bar
CONST COLORREF   clProgressBar      = ARGB(0, 225, 232, 213);

// Defines the start and end points of the progress bar
CONST POINT      ptProgressBarStart = {95,  341},
                 ptProgressBarEnd   = {315, 341};

// Represents 100% of operation progress
CONST UINT       iProgressBarMaximum = 10000;

// Defines the timer ID of the splash window update timer
#define         SPLASH_TICKER         42

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  :  createSplashWindowData
// Description     : 
// 
// 
// Return Value   : 
// 
SPLASH_WINDOW_DATA*   createSplashWindowData()
{
   SPLASH_WINDOW_DATA*  pWindowData = utilCreateEmptyObject(SPLASH_WINDOW_DATA);

   return pWindowData;
}

/// Function name  : deleteSplashWindowData
// Description     : 
// 
// SPLASH_WINDOW_DATA*  &pWindowData : [in] 
// 
VOID  deleteSplashWindowData(SPLASH_WINDOW_DATA*  &pWindowData)
{
   TRACK_FUNCTION();

   /// Delete drawing objects
   if (pWindowData->hProgressFont)
      DeleteFont(pWindowData->hProgressFont);

   if (pWindowData->hProgressPen)
      DeletePen(pWindowData->hProgressPen);

   // Delete calling object
   utilDeleteObject(pWindowData);

   END_TRACKING();
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : displaySplashWindow
// Description     : Creates and displays the modal splash window
// 
// MAIN_WINDOW_DATA*  pMainWindowData   : [in] Main window data
// 
// Return Value   : Window handle of the splash window, if successful, otherwise NULL
// 
HWND   displaySplashWindow(MAIN_WINDOW_DATA*  pMainWindowData)
{
   SPLASH_WINDOW_DATA*  pWindowData;
   HWND                 hWnd;                // Splash window

   // Prepare
   TRACK_FUNCTION();
   pWindowData = createSplashWindowData();
   
   /// Create centered splash window   
   hWnd = CreateWindowEx(WS_EX_LAYERED, szSplashWindowClass, TEXT("Splash Window"), WS_VISIBLE WITH WS_OVERLAPPED, 
                         CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, pMainWindowData->hMainWnd, NULL, getAppInstance(), (VOID*)pWindowData);
   ERROR_CHECK("creating splash window", hWnd);

   // [CHECK] Ensure window was created successfully
   if (hWnd)
      /// [SUCCESS] Disable main window
      EnableWindow(getAppWindow(), FALSE);
   else
      // [FAILED]
      deleteSplashWindowData(pWindowData);
   
   // Return window handle
   END_TRACKING();
   return hWnd;
}


/// Function name  : drawSplashWindow
// Description     : Paints the splash window and the progress of the load game data operation
// 
// HWND  hWnd  : [in] Splash window
// 
VOID  drawSplashWindow(SPLASH_WINDOW_DATA*  pWindowData)
{
   BLENDFUNCTION  oBlendData;             // Alpha blending data
   DC_STATE*      pDCState;               // DC state
   POINT          ptOrigin;               // Drawing origin into the memory DC
   SIZE           siWindowSize;           // Size of the splash window
   RECT           rcProgressText;         // Progress stage text drawing rectangle
   TCHAR*         szProgressText;         // Progress stage text
   HDC            hScreenDC,
                  hMemoryDC;              // Memory DC
   UINT           iCurrentProgress,       // Operation progress : 0 <= n <= 10000
                  iProgressBarLength;     // Length of the progress bar, in pixels

   // Prepare
   TRACK_FUNCTION();
   utilZeroObject(&oBlendData, BLENDFUNCTION);
   siWindowSize = siLogoBitmap;
   ptOrigin.x = 0;
   ptOrigin.y = 0;

   /// Create DC, Pen, Font, Bitmap and progress text
   hScreenDC      = GetDC(NULL);
   hMemoryDC      = CreateCompatibleDC(hScreenDC);
   pDCState       = utilCreateDeviceContextState(hMemoryDC);
   szProgressText = utilLoadString(getResourceInstance(), getCurrentOperationStageID(getMainWindowData()->pOperationPool), 128);

   /// Create bitmap
   pWindowData->hLogoBitmap = (HBITMAP)LoadImage(getResourceInstance(), TEXT("LOGO_BITMAP"), IMAGE_BITMAP, siLogoBitmap.cx, siLogoBitmap.cy, LR_CREATEDIBSECTION);
   
   // Setup DC
   utilSetDeviceContextBitmap(pDCState, pWindowData->hLogoBitmap);
   utilSetDeviceContextPen(pDCState, pWindowData->hProgressPen);
   utilSetDeviceContextFont(pDCState, pWindowData->hProgressFont, clProgressBar);
   utilSetDeviceContextBackgroundMode(pDCState, TRANSPARENT);

   // Request per-pixel alpha blending
   oBlendData.BlendOp             = AC_SRC_OVER;
   oBlendData.AlphaFormat         = AC_SRC_ALPHA;
   oBlendData.SourceConstantAlpha = 255;

   // Calculate progress and text rectangles
   iCurrentProgress   = getCurrentOperationProgress(getMainWindowData()->pOperationPool);
   iProgressBarLength = (ptProgressBarEnd.x - ptProgressBarStart.x) * iCurrentProgress / iProgressBarMaximum;
   SetRect(&rcProgressText, ptProgressBarStart.x - 20, ptProgressBarStart.y + 3, ptProgressBarEnd.x + 20, ptProgressBarEnd.y + 20);

   /// [PROGRESS] Draw progress bar
   MoveToEx(hMemoryDC, ptProgressBarStart.x, ptProgressBarStart.y, NULL);
   LineTo(hMemoryDC, (ptProgressBarStart.x + iProgressBarLength), ptProgressBarEnd.y);

   /// [TEXT] Draw progress text
   DrawText(hMemoryDC, szProgressText, lstrlen(szProgressText), &rcProgressText, DT_SINGLELINE WITH DT_CENTER);

   /// [LOGO] Paint alpha-blended logo
   if (!UpdateLayeredWindow(pWindowData->hWnd, NULL, NULL, &siWindowSize, hMemoryDC, &ptOrigin, 0, &oBlendData, ULW_ALPHA))
      ERROR_CHECK("updating layered window", FALSE);

   // [CHECK] Are we running in windows 7 or newer?
   if (getAppWindowsVersion() >= WINDOWS_7)
      /// [WINDOWS 7] Display progress in the taskbar
      utilSetWindowProgressValue(getAppWindow(), iCurrentProgress, iProgressBarMaximum);

   // Cleanup
   utilDeleteDeviceContextState(pDCState);
   DeleteDC(hMemoryDC);
   DeleteBitmap(pWindowData->hLogoBitmap);
   ReleaseDC(NULL, hScreenDC);
   utilDeleteString(szProgressText);
   END_TRACKING();
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onSplashWindowCreate
// Description     : 
// 
// SPLASH_WINDOW_DATA*  pWindowData : [in] 
// HWND                 hWnd        : [in] 
// 
VOID  onSplashWindowCreate(SPLASH_WINDOW_DATA*  pWindowData, HWND  hWnd)
{
   RECT    rcMainWindow,        // Main window rectangle
           rcSplashWindow;      // Splash window rectangle
   HDC     hDC;

   TRACK_FUNCTION();

   // Prepare
   GetWindowRect(getAppWindow(), &rcMainWindow);

   /// Store window data
   SetWindowLong(hWnd, 0, (LONG)pWindowData);
   pWindowData->hWnd = hWnd;

   /// Create drawing objects
   hDC = GetDC(hWnd);
   pWindowData->hProgressPen  = CreatePen(PS_SOLID, 2, clProgressBar);
   pWindowData->hProgressFont = utilCreateFont(hDC, TEXT("Tahoma Regular"), 7, FALSE, FALSE, FALSE);      //MS Sans Serif
   ReleaseDC(hWnd, hDC);

   // Calculate centered Spash window rectangle
   SetRect(&rcSplashWindow, 0, 0, siLogoBitmap.cx, siLogoBitmap.cy);
   utilCentreRectangle(&rcMainWindow, &rcSplashWindow, UCR_HORIZONTAL WITH UCR_VERTICAL);

   /// Position and display window
   SetWindowPos(hWnd, NULL, rcSplashWindow.left, rcSplashWindow.top, siLogoBitmap.cx, siLogoBitmap.cy, SWP_NOZORDER);
   ShowWindow(hWnd, SW_SHOW);

   /// Paint window
   drawSplashWindow(pWindowData);

   // Set update timer
   SetTimer(hWnd, SPLASH_TICKER, 200, NULL);
   END_TRACKING();
}


/// Function name  : onSplashWindowDestroy
// Description     : 
// 
// SPLASH_WINDOW_DATA*  pWindowData : [in] 
// 
VOID  onSplashWindowDestroy(SPLASH_WINDOW_DATA*  pWindowData)
{
   TRACK_FUNCTION();

   // [CHECK] Are we running in windows 7
   if (getAppWindowsVersion() >= WINDOWS_7)
      /// [WINDOWS 7] Hide taskbar progress bar
      utilSetWindowProgressState(getAppWindow(), TBPF_NOPROGRESS);

   /// Destroy ticker
   KillTimer(pWindowData->hWnd, SPLASH_TICKER);

   // Sever and delete window data
   SetWindowLong(pWindowData->hWnd, 0, NULL);
   deleteSplashWindowData(pWindowData);

   END_TRACKING();
}


/// Function name  : onSplashWindowTimer
// Description     : 
// 
// SPLASH_WINDOW_DATA*  pWindowData : [in] 
// 
VOID  onSplashWindowTimer(SPLASH_WINDOW_DATA*  pWindowData)
{
   // Update window
   drawSplashWindow(pWindowData);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : wndprocSplashWindow
// Description     : Splash window procedure
// 
LRESULT  wndprocSplashWindow(HWND  hWnd, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   SPLASH_WINDOW_DATA*  pWindowData;
   CREATESTRUCT*        pCreationData;
   ERROR_STACK*         pException;
   BOOL                 bResult;

   // [TRACK]
   TRACK_FUNCTION();
   bResult = TRUE;

   /// [GUARD BLOCK]
   __try
   {
      // Prepare
      pWindowData = (SPLASH_WINDOW_DATA*)GetWindowLong(hWnd, 0);

      // Examine message
      switch (iMessage)
      {
      /// [CREATE] Display window
      case WM_CREATE:
         // Prepare
         pCreationData = (CREATESTRUCT*)lParam;
         // Initialise window and store window data
         onSplashWindowCreate((SPLASH_WINDOW_DATA*)pCreationData->lpCreateParams, hWnd);
         break;

      /// [DESTROY] Destroy ticker
      case WM_DESTROY:
         onSplashWindowDestroy(pWindowData);
         break;

      /// [TIMER] Update window
      case WM_TIMER:
         onSplashWindowTimer(pWindowData);
         break;

      // [UNHANDLED]
      default:
         bResult = DefWindowProc(hWnd, iMessage, wParam, lParam);
         break;
      }
   }
   /// [EXCEPTION HANDLER]
   __except (generateExceptionError(GetExceptionInformation(), pException))
   {
      // [ERROR] "An unidentified and unexpected critical error has occurred in the splash dialog"
      enhanceError(pException, ERROR_ID(IDS_EXCEPTION_SPLASH_DIALOG));
      displayException(pException);

      // Ensure main window is enabled
      EnableWindow(getAppWindow(), TRUE);
   }

   END_TRACKING();
   return bResult;
}

