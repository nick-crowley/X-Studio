//
// About Dialog.cpp : About dialog
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        DECLARATIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

// Helpers
VOID  printDirectShowError(HRESULT  hResult);
VOID  playAudioFile(const TCHAR*  szFullPath);
VOID  stopAudioFile();

// Functions
BOOL  initAboutDialog(HWND  hDialog);
BOOL  initCreditsDialog(HWND  hDialog);

// Message Handlers
BOOL  onAboutDialog_Command(HWND  hDialog, CONST UINT  iControlID, CONST UINT  iNotification, HWND  hCtrl);
BOOL  onAboutDialog_Destroy(HWND  hDialog);
BOOL  onAboutDialog_LinkClick(HWND  hDialog, CONST UINT  iControlID, NMLINK*  pHeader);
BOOL  onCreditsDialog_Destroy(HWND  hDialog);
BOOL  onCreditsDialog_LinkClick(HWND  hDialog, CONST UINT  iControlID, NMLINK*  pHeader);

// Dialog procedure
INT_PTR  dlgprocAboutBox(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);
INT_PTR  dlgprocCreditsDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);
INT_PTR  dlgprocLicenseDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////

// DirectShow Interfaces
IGraphBuilder*  g_pGraphBuilder = 0;
IMediaControl*  g_pGraphControl = 0;
IBasicAudio*    g_pAudioControl = 0;

// Timer IDs
static const UINT  iMoveTimer = 42,
                   iWaitTimer = 40;

// Scroll position
static INT   iScrollPos = 0,
             iMaxScrollPos = 1130;

// Timings
static UINT  iScrollTimeout = 80,
             iWaitTimeout = 5000;

// Credits dialog
static HWND  hCreditsDialog;

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : printDirectShowError
// Description     : Prints DirectShow errors to the Console
// 
// HRESULT  hResult   : [in] 
// 
VOID  printDirectShowError(HRESULT  hResult)
{
   TCHAR*  szError = utilCreateEmptyString(512);

   // Print error to console
   AMGetErrorText(hResult, szError, 512);
   CONSOLE("ERROR: printDirectShowError() : Unable to playback file: %s", szError);
   utilDeleteString(szError);
}


/// Function name  : playAudioFile
// Description     : Plays an audio file using DirectShow
// 
// const TCHAR*  szFullPath   : [in] File to play
// 
VOID  playAudioFile(const TCHAR*  szFullPath)
{
   HRESULT  hResult;

   // Create Graph Builder
   if (SUCCEEDED(hResult= CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (IInterface*)&g_pGraphBuilder)))
      g_pGraphBuilder->AddRef();
   
   // Create graph controller
   if (g_pGraphBuilder AND SUCCEEDED(hResult= g_pGraphBuilder->QueryInterface(IID_IMediaControl, (IInterface*)&g_pGraphControl)))
      g_pGraphControl->AddRef();

   // Create audio controller
   if (g_pGraphControl AND SUCCEEDED(hResult= g_pGraphBuilder->QueryInterface(IID_IBasicAudio, (IInterface*)&g_pAudioControl)))
      g_pAudioControl->AddRef();

   /// Render file
   if (g_pGraphBuilder AND g_pGraphControl AND g_pAudioControl AND SUCCEEDED(hResult = g_pGraphBuilder->RenderFile(szFullPath, NULL)))
      /// [SUCCESS] Play 
      g_pGraphControl->Run();
   else
      // [FAILURE] Print to console
      printDirectShowError(hResult);
}


/// Function name  : stopAudioFile
// Description     : Stops playback of file
// 
// 
VOID  stopAudioFile()
{
   // Fade out
   if (g_pAudioControl)
      for (UINT  iStep = 0; iStep < 5; iStep++)
      {
         g_pAudioControl->put_Volume(iStep * -500);
         Sleep(100);
      }

   // Stop playback
   if (g_pGraphControl)
      g_pGraphControl->Stop();

   /// Release Interfaces
   utilReleaseInterface(g_pAudioControl);
   utilReleaseInterface(g_pGraphControl);
   utilReleaseInterface(g_pGraphBuilder);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : displayAboutDialog
// Description     : Displays the About dialog
// 
// HWND  hParent   : [in] Parent
// 
// Return Value   : TRUE if successful, FALSE otherwise
// 
BOOL   displayAboutDialog(HWND  hParent)
{
   // Launch the About Dialog
   return showDialog(TEXT("ABOUT_DIALOG"), hParent, dlgprocAboutBox, NULL) != -1;
}


/// Function name  : initAboutDialog
// Description     : Loads the dialog icons and centres the dialog
// 
// HWND  hDialog   : [in] About dialog
// 
// Return Value   : TRUE
// 
BOOL   initAboutDialog(HWND  hDialog)
{
   RECT    rcCredits, rcIcon, rcHeading, rcZLib;
   SIZE    siCredits;
   TCHAR*  szMixTension;

   // Prepare
   GetClientRect(hDialog, &rcCredits);
   utilGetDlgItemRect(hDialog, IDC_DIALOG_ICON, &rcIcon);
   utilGetDlgItemRect(hDialog, IDC_DIALOG_HEADING_1, &rcHeading);
   utilGetDlgItemRect(hDialog, IDC_ABOUT_ZLIB_STATIC, &rcZLib);

   // Calculate CreditsDlg rectangle
   rcCredits.top    = rcHeading.bottom + 6;
   rcCredits.left   = rcIcon.right + 2;
   rcCredits.bottom = rcZLib.top - 6;
   utilConvertRectangleToSize(&rcCredits, &siCredits);

   // Centre dialog
   utilCentreWindow(getAppWindow(), hDialog);

   // [OWNER DRAW]
   utilAddWindowStyle(GetDlgItem(hDialog, IDC_DIALOG_TITLE), SS_OWNERDRAW WITH CSS_CENTRE);
   utilAddWindowStyle(GetDlgItem(hDialog, IDC_DIALOG_HEADING_1), SS_OWNERDRAW WITH CSS_CENTRE);

   /// Play 'MixTension' soundtrack MP3
   playAudioFile( szMixTension = utilCreateStringf(MAX_PATH, TEXT("%s\\soundtrack\\08217.mp3"), getAppPreferences()->szGameFolder) );

   /// Wait 4 seconds before scrolling
   SetTimer(hDialog, iWaitTimer, iWaitTimeout, NULL);

   /// Create/position CreditsDlg
   hCreditsDialog = CreateDialogW(getResourceInstance(), TEXT("CREDITS_DIALOG"), hDialog, dlgprocCreditsDialog);
   MoveWindow(hCreditsDialog, rcCredits.left, rcCredits.top, siCredits.cx, siCredits.cy, TRUE);
   ShowWindow(hCreditsDialog, SW_SHOW);

   // Set scroll position
   iScrollPos = 0;

   // Return TRUE
   utilDeleteString(szMixTension);
   return TRUE;
}


/// Function name  : initCreditsDialog
// Description     : Loads the dialog icons and centres the dialog
// 
// HWND  hDialog   : [in] Credits dialog
// 
// Return Value   : TRUE
// 
BOOL   initCreditsDialog(HWND  hDialog)
{
   HICON   hIcon;

   // Set 'Exscriptor' icon
   hIcon = (HICON)LoadImage(getResourceInstance(), TEXT("EXSCRIPTOR_ICON"), IMAGE_ICON, 32, 32, LR_LOADTRANSPARENT);
   Static_SetIcon(GetControl(hDialog, IDC_ABOUT_EXSCRIPTOR_ICON), hIcon);

   // Set 'DoubleShow' icon
   hIcon = (HICON)LoadImage(getResourceInstance(), TEXT("DOUBLESHADOW_ICON"), IMAGE_ICON, 48, 48, LR_LOADTRANSPARENT);
   Static_SetIcon(GetControl(hDialog, IDC_ABOUT_DOUBLESHADOW_ICON), hIcon);

   // [OWNER DRAW]
   utilAddWindowStyle(GetDlgItem(hDialog, IDC_DIALOG_HEADING_2), SS_OWNERDRAW);
   utilAddWindowStyle(GetDlgItem(hDialog, IDC_DIALOG_HEADING_3), SS_OWNERDRAW);
   
   // Return TRUE
   return TRUE;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onAboutDialog_Command
// Description     : Handles closing the dialog and the license dialog
// 
// HWND        hDialog       : [in] About dialog
// CONST UINT  iControlID    : [in] ID of the control
// CONST UINT  iNotification : [in] Notification
// 
// Return Value   : TRUE if processed, FALSE otherwise
// 
BOOL  onAboutDialog_Command(HWND  hDialog, CONST UINT  iControlID, CONST UINT  iNotification, HWND  hCtrl)
{
   BOOL   bResult;

   // Prepare
   bResult = FALSE;

   // Examine source
   switch (iControlID)
   {
   /// [LICENSE DIALOG]
   case IDC_ABOUT_LICENSE:
      showDialog(TEXT("LICENSE_DIALOG"), hDialog, dlgprocLicenseDialog, NULL);
      bResult = TRUE;
      break;

   /// [OK/CANCEL] Close dialog
   case IDOK:
   case IDCANCEL:
      EndDialog(hDialog, IDOK);
      bResult = TRUE;
      break;
   }

   // Return result
   return bResult;
}


/// Function name  : onAboutDialog_Destroy
// Description     : Stops playback. Destroys Credits dialog
// 
// HWND  hDialog   : [in] About dialog
// 
// Return Value   : TRUE
// 
BOOL  onAboutDialog_Destroy(HWND  hDialog)
{
   /// Stop playback
   stopAudioFile();

   /// Kill timers
   KillTimer(hDialog, iMoveTimer);
   KillTimer(hDialog, iWaitTimer);

   /// Destroy Credits dialog
   utilDeleteWindow(hCreditsDialog);

   // Return TRUE
   return TRUE;
}



/// Function name  : onAboutDialog_Timer
// Description     : Scrolls Credits dialog
// 
// HWND        hDialog  : [in] About dialog
// const UINT  iTimerID : [in] Timer
// 
// Return Value   : TRUE
// 
BOOL  onAboutDialog_Timer(HWND  hDialog, const UINT  iTimerID)
{
   /// [WAIT] Commence scrolling
   if (iTimerID == iWaitTimer)
   {
      KillTimer(hDialog, iWaitTimer);
      SetTimer(hDialog, iMoveTimer, iScrollTimeout, NULL);
   }
   /// [SCROLL] Scroll credits dialog
   else
   {
      ScrollWindowEx(hCreditsDialog, 0, -1, NULL, NULL, NULL, NULL, SW_SCROLLCHILDREN|SW_INVALIDATE|SW_ERASE);
      UpdateWindow(hCreditsDialog);

      // [CHECK] Should we stop?
      if (--iScrollPos <= -iMaxScrollPos)
         KillTimer(hDialog, iMoveTimer);
   }

   return TRUE;
}


/// Function name  : onAboutDialog_Destroy
// Description     : Destroys the dialog icons
// 
// HWND  hDialog   : [in] About dialog
// 
// Return Value   : TRUE
// 
BOOL  onCreditsDialog_Destroy(HWND  hDialog)
{
   HICON   hIcon;

   // Destroy 'Exscriptor' icon
   hIcon = Static_GetIcon(GetControl(hDialog, IDC_ABOUT_EXSCRIPTOR_ICON), ignored);
   DestroyIcon(hIcon);

   // Destroy 'DoubleShow' icon
   hIcon = Static_GetIcon(GetControl(hDialog, IDC_ABOUT_DOUBLESHADOW_ICON), ignored);
   DestroyIcon(hIcon);

   // Return TRUE
   return TRUE;
}


/// Function name  : onCreditsDialog_LinkClick
// Description     : Launches the specified link
// 
// HWND        hDialog    : [in] Credits dialog
// CONST UINT  iControlID : [in] Control sending the notification
// NMLINK*     pLinkInfo  : [in] Link notification header
// 
// Return Value   : TRUE
// 
BOOL  onCreditsDialog_LinkClick(HWND  hDialog, CONST UINT  iControlID, NMLINK*  pHeader)
{
   LITEM*  pLink;    // Convenience pointer

   // Prepare
   pLink = &pHeader->item;

   // Prepare
   pLink->mask  = LIF_URL WITH LIF_ITEMINDEX;
   pLink->iLink = 0;
   
   /// Retrieve the URL and launch in browser
   SendDlgItemMessage(hDialog, iControlID, LM_GETITEM, NULL, (LPARAM)pLink);
   utilLaunchURL(getAppWindow(), pLink->szUrl, SW_SHOWMAXIMIZED);

   /// Mark link as 'visited'
   pLink->mask  = LIF_STATE WITH LIF_ITEMINDEX;
   pLink->state = LIS_VISITED WITH LIS_ENABLED;
   pLink->stateMask = pLink->state;
   SendDlgItemMessage(hDialog, iControlID, LM_SETITEM, NULL, (LPARAM)pLink);
   
   // Return TRUE
   return TRUE;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : dlgprocAboutBox
// Description     : 'About Box' Dialog window procedure
// 
INT_PTR   dlgprocAboutBox(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   BOOL  bResult = FALSE;   // Operation result

   // Examine message
   switch (iMessage)
   {
   /// [CREATION]
   case WM_INITDIALOG:
      bResult = initAboutDialog(hDialog);
      break;

   /// [DESTRUCTION]
   case WM_DESTROY:
      bResult = onAboutDialog_Destroy(hDialog);
      break;

   /// [COMMAND PROCESSING]
   case WM_COMMAND:
      bResult = onAboutDialog_Command(hDialog, LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
      break;

   /// [TIMER]
   case WM_TIMER:
      bResult = onAboutDialog_Timer(hDialog, wParam);
      break;

   /// [OWNER DRAW]
   case WM_DRAWITEM:
      switch (wParam)
      {
      case IDC_DIALOG_TITLE:      bResult = onOwnerDrawStaticTitle(lParam);                        break;
      case IDC_DIALOG_HEADING_1:  bResult = onOwnerDrawStaticHeading(lParam);                      break;
      case IDC_DIALOG_ICON:       bResult = onOwnerDrawStaticIcon(lParam, szMainWindowClass, 96);  break;
      }
      break;

   /// [VISUAL STYLE]
   default:
      bResult = dlgprocVistaStyleDialog(hDialog, iMessage, wParam, lParam);
      break;
   }

   // Return result
   return bResult;
}



/// Function name  : dlgprocCreditsDialog
// Description     : Scrolling 'Credits' dialog procedure
// 
INT_PTR   dlgprocCreditsDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   NMHDR*   pHeader;           // SysLink notification header
   BOOL     bResult = FALSE;   // Operation result

   // Examine message
   switch (iMessage)
   {
   /// [CREATION]
   case WM_INITDIALOG:
      bResult = initCreditsDialog(hDialog);
      break;

   /// [DESTRUCTION]
   case WM_DESTROY:
      bResult = onCreditsDialog_Destroy(hDialog);
      break;
      
   /// [NOTIFICATION]
   case WM_NOTIFY:
      // Prepare
      pHeader = (NMHDR*)lParam;

      /// [CLICK] Launch link
      if (pHeader->idFrom == IDC_ABOUT_LINK_FORUMS AND pHeader->code == NM_CLICK)
         bResult = onCreditsDialog_LinkClick(hDialog, pHeader->idFrom, (NMLINK*)pHeader);         
      break;

   /// [OWNER DRAW]
   case WM_DRAWITEM:
      switch (wParam)
      {
      case IDC_DIALOG_HEADING_2:
      case IDC_DIALOG_HEADING_3:  bResult = onOwnerDrawStaticHeading(lParam);    break;
      }
      break;

   /// [VISUAL STYLE]
   default:
      bResult = dlgprocVistaStyleDialog(hDialog, iMessage, wParam, lParam);
      break;
   }

   // Return result
   return bResult;
}


/// Function name  : dlgprocLicenseDialog
// Description     : License dialog procedure
// 
INT_PTR   dlgprocLicenseDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   //NMHDR*  pHeader;

   // Examine message
   switch (iMessage)
   {
   /// [CREATION]
   case WM_INITDIALOG:
      // Use application name for window title
      SetWindowText(hDialog, getAppName());

      // Centre dialog
      utilCentreWindow(getAppWindow(), hDialog);

      // Ensure OK button is at the top of the Z-Order
      BringWindowToTop(GetControl(hDialog, IDOK));
      return TRUE;

   /// [COMMAND PROCESSING]
   case WM_COMMAND:
      switch (LOWORD(wParam))
      {
      // [OK/CANCEL] Close dialog
      case IDOK:
      case IDCANCEL:
         if (HIWORD(wParam) == BN_CLICKED)
         {
            EndDialog(hDialog, IDOK);
            return TRUE;
         }
      }
      break;

   /// [VISUAL STYLES]
   case WM_CTLCOLORDLG:
   case WM_CTLCOLORSTATIC:
   case WM_CTLCOLORBTN:
      return (INT_PTR)onDialog_ControlColour((HDC)wParam, COLOR_WINDOW);

   /// [VISUAL STYLES]
   
      //return (INT_PTR)GetStockObject(WHITE_BRUSH);
   }

   return FALSE;
}

