//
// Media Player Dialog.cpp : Handles the playing of audio/video items
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       CONSTANTS / GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

CONST UINT   iPlaybackProgressTimerID = 42,
             iPlaybackControlIconSize = 24;

CONST COLORREF  rgbBackground = RGB(56,63,77);

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       CREATION / DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createMediaPlayerDialog
// Description     : Create the media player dialog
// 
// HWND  hParentWnd   : [in] Parent window for the dialog
// 
// Return Value   : Window handle of the media player dialog if successful, otherwise NULL
// 
HWND   createMediaPlayerDialog(HWND  hParentWnd)
{
   MEDIA_PLAYER_DATA*  pDialogData;
   HWND                hDialog;

   // Create dialog data
   pDialogData = utilCreateEmptyObject(MEDIA_PLAYER_DATA);

   // Create dialog
   hDialog = CreateDialogParam(getResourceInstance(), TEXT("DOCUMENT_MEDIA_PLAYER_DIALOG"), hParentWnd, dlgprocMediaPlayerDialog, (LPARAM)pDialogData);
   ERROR_CHECK("creating media player dialog", hDialog);

   // Return window handle or NULL
   return hDialog;
}


/// Function name  : createMediaPlayerDialogInterfaces
// Description     : Create the various DirectShow interfaces necessary
// 
// MEDIA_PLAYER_DATA*  pDialogData : [in] MediaPlayer dialog data
// HWND                hDialog     : [in] MediaPlayer dialog window handle
// 
// Return Value   : 
// 
HRESULT  createMediaPlayerDialogInterfaces(MEDIA_PLAYER_DATA*  pDialogData, HWND  hDialog)
{
   IBaseFilter*       pVideoRenderer;     // Video Mixing renderer filter to be manually added to the graph
   IVMRFilterConfig*  pVideoConfig;       // Configuration interface for above filter
   HRESULT            hResult;            // Operation result
   
   // [CHECK] there must be a current item
   ASSERT(pDialogData->pCurrentItem);

   // Prepare
   pVideoRenderer = NULL;
   pVideoConfig   = NULL;

   /// Create Filter Graph Manager, Media Controller and Media Seeking Controller
   hResult = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (IInterface*)&pDialogData->pGraph);
   if (FAILED(hResult))
      return hResult;      // [ERROR] - Could not create the Filter Graph Manager
   pDialogData->pGraph->AddRef();

   hResult = pDialogData->pGraph->QueryInterface(IID_IMediaControl, (IInterface*)&pDialogData->pController);
   if (FAILED(hResult))
      return hResult;      // [ERROR] - Could not create the Graph Controller
   pDialogData->pController->AddRef();

   hResult = pDialogData->pGraph->QueryInterface(IID_IMediaSeeking, (IInterface*)&pDialogData->pSeeking);
   if (FAILED(hResult))
      return hResult;      // [ERROR] - Could not create the Seeking Controller
   pDialogData->pSeeking->AddRef();

   /// Create Event Manager and Register for Events
   hResult = pDialogData->pGraph->QueryInterface(IID_IMediaEventEx, (IInterface*)&pDialogData->pEvents);
   if (FAILED(hResult))
      return hResult;      // [ERROR] - Could not create the Event Manager
   pDialogData->pEvents->AddRef();
   
   hResult = pDialogData->pEvents->SetNotifyWindow((OAHWND)hDialog, UM_MEDIA_EVENT, NULL);
   if (FAILED(hResult))
      return hResult;      // [ERROR] - Could not register for events

   /// [VIDEO ONLY] Setup for windowless video rendering
   if (pDialogData->pCurrentItem->eType == MIT_VIDEO_CLIP)
   {
      /// Create video renderer and add to the filter graph
      hResult = CoCreateInstance(CLSID_VideoMixingRenderer, NULL, CLSCTX_INPROC, IID_IBaseFilter, (IInterface*)&pVideoRenderer);
      if (FAILED(hResult))
         return hResult;
      pVideoRenderer->AddRef();
      
      /// THIS SOLVES YOUR CRASH PROBLEM: 
      //        REQUIRES MORE RESEARCH THOUGH.
      hResult = pDialogData->pGraph->AddFilter(pVideoRenderer, TEXT("Video Mixing Renderer")); 
      ASSERT(SUCCEEDED(hResult));

      // Create the Video Renderer Config interface
      hResult = pVideoRenderer->QueryInterface(IID_IVMRFilterConfig, (IInterface*)&pVideoConfig); 
      if (FAILED(hResult))
      {
         utilReleaseInterface(pVideoRenderer);
         return hResult;
      }
      pVideoConfig->AddRef();
      
      // Set the Video Renderer mode to 'Windowless'
      hResult = pVideoConfig->SetRenderingMode(VMRMode_Windowless);
      ASSERT(SUCCEEDED(hResult));

      /// Create the 'Windowless video control' and specify the appropriate window handle
      hResult = pVideoRenderer->QueryInterface(IID_IVMRWindowlessControl, (IInterface*)&pDialogData->pVideoCtrl);
      if (SUCCEEDED(hResult))
      {
         pDialogData->pVideoCtrl->AddRef();
         hResult = pDialogData->pVideoCtrl->SetVideoClippingWindow(hDialog); 
         ASSERT(SUCCEEDED(hResult));
      }

      // Cleanup
      utilReleaseInterface(pVideoConfig);
      utilReleaseInterface(pVideoRenderer);
   }

   // Return
   return hResult;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                            HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : getMediaPlayerDialogData
// Description     : Get the media player dialog data
// 
// HWND  hDialog   : [in] Window handle of the media player dialog
// 
// Return Value    : MediaPlayer dialog window data
// 
MEDIA_PLAYER_DATA*  getMediaPlayerDialogData(HWND  hDialog)
{
   return (MEDIA_PLAYER_DATA*)GetWindowLong(hDialog, DWL_USER);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                            FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

#define      ONE_MILLISECOND      (__int64)10000

/// Function name  : calculateMediaItemInReferenceTime
// Description     : Calculate the start and end positions of a MediaItem in reference time
// 
// CONST MEDIA_ITEM*  pItem     : [in]  MediaItem to convert
// REFERENCE_TIME*    piStart   : [out] Start position in reference time (nanoseconds)
// REFERENCE_TIME*    piFinish  : [out] Finish position in reference time (nanoseconds)
// 
VOID  calculateMediaItemInReferenceTime(CONST MEDIA_ITEM*  pItem, REFERENCE_TIME*  piStart, REFERENCE_TIME*  piFinish)
{
   UINT  iStartInMilliseconds;

   switch (pItem->eType)
   {
   // [SOUND EFFECT] Start and stop positioning is not stored in MediaItems for SFX
   case MIT_SOUND_EFFECT:
      ASSERT(FALSE);
      break;

   /// [SPEECH CLIP] Convert the values in sounds.txt from milliseconds to reference time
   case MIT_SPEECH_CLIP:
      *piStart  = (pItem->iPosition * ONE_MILLISECOND);
      *piFinish = *piStart + (pItem->iLength * ONE_MILLISECOND);
      break;

   /// [VIDEO CLIP] Convert the MEDIA_POSITION values into reference time
   case MIT_VIDEO_CLIP:
      // Calculate start position in milliseconds then convert to reference time
      iStartInMilliseconds = pItem->oStart.iMilliseconds + (pItem->oStart.iSeconds * 1000) + (pItem->oStart.iMinutes * 60 * 1000);
      *piStart  = (iStartInMilliseconds * ONE_MILLISECOND);
      // Convert the duration from milliseconds to reference time
      *piFinish = *piStart + (calculateMediaItemDuration(pItem) * ONE_MILLISECOND);
      break;
   }
}


/// Function name  : drawMediaPlayerButtonCtrl
// Description     : 
// 
// MEDIA_PLAYER_DATA*  pDialogData : [in] MediaPlayer dialog data
// HWND                hDialog     : [in] MediaPlayer dialog window handle
// CONST UINT          iControlID  : [in] 
// NMCUSTOMDRAW*       pDrawData   : [in] 
// 
// Return Value   : TRUE if control was drawn, FALSE otherwise
// 
BOOL  drawMediaPlayerButtonCtrl(MEDIA_PLAYER_DATA*  pDialogData, HWND  hDialog, CONST UINT  iControlID, NMCUSTOMDRAW*  pDrawData)
{
   UINT    iIconIndex;    // ImageList icon index for the button
   UINT    iButtonState;  // Button's current state (specified using Themes button states)
   HWND    hCtrl;         // Button's Window handle
   RECT    rcCtrlRect;    // Button's client rectangle
   SIZE    siCtrlSize;    // Size of button's client rectangle
   POINT   ptIconPoint;   // Co-ordinates to draw the button

   switch (pDrawData->dwDrawStage)
   {
   case CDDS_PREERASE:
      // Prepare
      hCtrl = pDrawData->hdr.hwndFrom;

      // Calculate co-ordinates to centre icon horizontally and vertically within the button
      GetClientRect(hCtrl, &rcCtrlRect);
      utilConvertRectangleToSize(&rcCtrlRect, &siCtrlSize);
      ptIconPoint.x = (siCtrlSize.cx - iPlaybackControlIconSize) / 2;
      ptIconPoint.y = (siCtrlSize.cy - iPlaybackControlIconSize) / 2;

      /// Determine button icon and state
      iIconIndex = (iControlID == IDC_MEDIA_PLAY ? 0 : 1);
      // [DISABLED] 
      if (!IsWindowEnabled(hCtrl))
         iButtonState = PBS_DISABLED;
      // [PRESSED]
      else if (SendMessage(hCtrl, BM_GETSTATE, 0, 0) INCLUDES BST_PUSHED)
         iButtonState = PBS_PRESSED;
      // [HOT / NORMAL]
      else
         iButtonState = (pDrawData->uItemState INCLUDES CDIS_HOT ? PBS_HOT : PBS_NORMAL);
            
      /// [BACKGROUND] Draw solid black rectangle
      FillRect(pDrawData->hdc, &pDrawData->rc, pDialogData->hPlayerBackground);

      /// [ICON] Draw either disabled or normal.
      switch (iButtonState)
      {
      // [PRESSED] Offset the draw rectangle to give the illusion button is presesd
      case PBS_PRESSED:
         OffsetRect(&pDrawData->rc, 1, 1);
      // [NORMAL] Draw normally
      case PBS_NORMAL:
         ImageList_Draw(pDialogData->hButtonIcons, iIconIndex, pDrawData->hdc, ptIconPoint.x, ptIconPoint.y, ILD_NORMAL WITH ILD_TRANSPARENT);
         break;
      // [HOT] Draw highlighted
      case PBS_HOT:
         ImageList_Draw(pDialogData->hButtonIcons, iIconIndex, pDrawData->hdc, ptIconPoint.x, ptIconPoint.y, ILD_BLEND50|ILD_TRANSPARENT);
         break;
      // [DISABLED] Draw greyed
      case PBS_DISABLED:
         ImageList_DrawEx(pDialogData->hButtonIcons, iIconIndex, pDrawData->hdc, ptIconPoint.x, ptIconPoint.y, iPlaybackControlIconSize, iPlaybackControlIconSize,
                          CLR_NONE, GetSysColor(COLOR_GRAYTEXT), ILD_BLEND50|ILD_TRANSPARENT);
         break;
      }
      
      // Inform system no painting is required
      SetWindowLong(hDialog, DWL_MSGRESULT, CDRF_SKIPDEFAULT);
      return TRUE;
   }

   return FALSE;
}


/// Function name  : drawMediaPlayerProgressCtrl
// Description     : Draws the media player progress dialog's progress bar
// 
// HDC                 hDC          : [in] Device context to draw to
// CONST RECT*         pDrawRect    : [in] Drawing rectangle to draw to
// HWND                hCtrl        : [in] Handle of the progress bar control
// MEDIA_PLAYER_DATA*  pDialogData  : [in] MediaPlayerDialog window data
// 
VOID  drawMediaPlayerProgressCtrl(HDC  hDC, CONST RECT*  pDrawRect, HWND  hCtrl, MEDIA_PLAYER_DATA*  pDialogData)
{
   HBITMAP      hBarBitmap,     // Blank progress bar bitmap
                hLineBitmap,    // Highlighted blue line bitmap
                hPrevBitmap;    // Original memory DC bitmap
   HDC          hMemoryDC;      // Memory DC for the painting the bitmap
   SIZE         siDrawSize;     // Size of the input drawing rectangle
   RECT         rcBarRect,      // Blank progress bar drawing rectangle
                rcLineRect;     // Highlight blue line drawing rectangle
   UINT         iLineLength;    // Length of blue line
   CONST UINT   iBitmapWidth = 400,    // Width of bar and line bitmaps
                iBarHeight   = 11,     // Height of the bar bitmap
                iLineHeight  = 3;      // Height of the line bitmap

   // Prepare
   hMemoryDC         = CreateCompatibleDC(hDC);

   // Calculate bar rectangle - centred vertically within the draw rectangle
   utilConvertRectangleToSize(pDrawRect, &siDrawSize);
   rcBarRect         = *pDrawRect;
   rcBarRect.top    += (siDrawSize.cy - iBarHeight) / 2;
   rcBarRect.bottom  = rcBarRect.top + iBarHeight;

   // Calculate line length - between 16 pixels (length of position indicator) and the length of the bar's inner track
   siDrawSize.cx    -= (8 * 2);  // Inner track length = bar length minus 8 pixels at each end
   iLineLength       = max(16, utilCalculatePercentage(siDrawSize.cx, SendMessage(hCtrl, PBM_GETPOS, NULL, NULL)));

   // Calculate line rectangle within the bar's inner track, which is 3 pixels high and offset by (x=8, y=4) pixels
   rcLineRect        = rcBarRect;
   rcLineRect.top   += 4;
   rcLineRect.left  += 8;
   rcLineRect.bottom = rcLineRect.top + iLineHeight;
   rcLineRect.right  = rcLineRect.left + iLineLength;

   // Load bar bitmap in memory DC
   hBarBitmap  = LoadBitmap(getResourceInstance(), TEXT("MEDIA_PROGRESS_BACKGROUND"));
   hPrevBitmap = (HBITMAP)SelectObject(hMemoryDC, hBarBitmap);

   /// [BACKGROUND] - Use a black background
   FillRect(hDC, pDrawRect, pDialogData->hPlayerBackground);

   /// [BAR] - Draw progress bar bitmap in two halves, first LHS then RHS
   utilConvertRectangleToSize(&rcBarRect, &siDrawSize);
   siDrawSize.cx /= 2;
   BitBlt(hDC, rcBarRect.left,                 rcBarRect.top,  siDrawSize.cx, siDrawSize.cy, hMemoryDC, 0,                            0, SRCCOPY);
   BitBlt(hDC, rcBarRect.left + siDrawSize.cx, rcBarRect.top,  siDrawSize.cx, siDrawSize.cy, hMemoryDC, iBitmapWidth - siDrawSize.cx, 0, SRCCOPY);

   // [OPERATION_PROGRESS LINE] - Draw a highlighted blue line if there is media currently playing
   if (pDialogData->bPlaying)
   {
      // Load line bitmap into memoryDC
      hLineBitmap = LoadBitmap(getResourceInstance(), TEXT("MEDIA_PROGRESS_HIGHLIGHT"));
      SelectObject(hMemoryDC, hLineBitmap);

      /// [LINE] - Draw progress line bitmap from RHS
      utilConvertRectangleToSize(&rcLineRect, &siDrawSize);
      BitBlt(hDC, rcLineRect.left, rcLineRect.top, siDrawSize.cx, siDrawSize.cy, hMemoryDC, iBitmapWidth - siDrawSize.cx, 0, SRCCOPY);

      // Cleanup
      DeleteObject(hLineBitmap);
   }
   
   // Cleanup
   SelectObject(hMemoryDC, hPrevBitmap);
   DeleteObject(hBarBitmap);
   DeleteDC(hMemoryDC);
}

/// Function name  : initMediaPlayerDialog
// Description     : Initialise the dialog controls
// 
// MEDIA_PLAYER_DATA*  pDialogData : [in] MediaPlayer dialog data
// HWND                hDialog     : [in] MediaPlayer dialog window handle
// 
// Return Value   : TRUE if succesful, FALSE otherwise
// 
BOOL   initMediaPlayerDialog(MEDIA_PLAYER_DATA*  pDialogData, HWND  hDialog)
{
   // Store dialog data
   SetWindowLong(hDialog, DWL_USER, (LONG)pDialogData);

   // Create buttons ImageList
   pDialogData->hButtonIcons = ImageList_Create(iPlaybackControlIconSize, iPlaybackControlIconSize, ILC_COLOR32 WITH ILC_MASK, 2, 1);
   ImageList_SetBkColor(pDialogData->hButtonIcons, rgbBackground);
   ImageList_AddIcon(pDialogData->hButtonIcons, LoadIcon(getResourceInstance(), TEXT("PLAY_ICON")));
   ImageList_AddIcon(pDialogData->hButtonIcons, LoadIcon(getResourceInstance(), TEXT("PAUSE_ICON")));

   // Define player background colour
   pDialogData->hPlayerBackground = CreateSolidBrush(rgbBackground);

   // Sub-class progress control
   pDialogData->wndprocProgressCtrl = (WNDPROC)SetWindowLong(GetDlgItem(hDialog,IDC_MEDIA_PROGRESS), GWL_WNDPROC, (LONG)wndprocMediaPlayerDialogProgressCtrl);
   SendDlgItemMessage(hDialog, IDC_MEDIA_PROGRESS, UM_INIT_MEDIA_PLAYER, NULL, (LPARAM)pDialogData);

   // Initialise controls
   updateMediaPlayerDialogControls(pDialogData, hDialog);
   return TRUE;
}

/// Function name  : setMediaPlayerDialogPlaybackPositions
// Description     : Define the 'start' and 'stop' positions of the current filter graph
// 
// MEDIA_PLAYER_DATA*  pDialogData : [in] MediaPlayer dialog data
// HWND                hDialog     : [in] MediaPlayer dialog window handle
// 
// Return Value   : S_OK if succesful, other values indicate errors
// 
HRESULT  setMediaPlayerDialogPlaybackPositions(MEDIA_PLAYER_DATA*  pDialogData, HWND  hDialog)
{
   REFERENCE_TIME  timeFinish;      // Timecode position where media item should stop playing at
   HRESULT         hResult;         // Operation result

   // [CHECK] Seeking controller exists
   ASSERT(pDialogData->pSeeking);

   // Examine item type
   switch (pDialogData->pCurrentItem->eType)
   {
   /// [SOUND EFFECT] - Set the start/end markers using length of the file.
   case MIT_SOUND_EFFECT:
      // Start at the beginning
      pDialogData->timeStart = 0;
      // Calculate finish time as the length of the file
      if (FAILED(hResult = pDialogData->pSeeking->GetDuration(&timeFinish)))
         return hResult;
      break;

   /// [SPEECH CLIP] - Set the start/end markers using MediaItem.
   case MIT_SPEECH_CLIP:
      // Calculate the start and finish positions from the MediaItem
      calculateMediaItemInReferenceTime(pDialogData->pCurrentItem, &pDialogData->timeStart, &timeFinish);
      break;

   /// [VIDEO CLIP] - Set the start/end markers using MediaItem.  Calculate the video rectangle
   case MIT_VIDEO_CLIP:
      // Calculate the start and finish positions from the MediaItem
      calculateMediaItemInReferenceTime(pDialogData->pCurrentItem, &pDialogData->timeStart, &timeFinish);

      // Set the video size
      setMediaPlayerDialogVideoPosition(pDialogData, hDialog);
      break;
   }

   // Store duration
   pDialogData->timeDuration = (timeFinish - pDialogData->timeStart);

   /// Set these positions in the filter graph
   return pDialogData->pSeeking->SetPositions(&pDialogData->timeStart, AM_SEEKING_AbsolutePositioning, &timeFinish, AM_SEEKING_AbsolutePositioning);
}


/// Function name  : setMediaPlayerDialogPlaybackState
// Description     : Alter the playback state of the media player dialog
// 
// MEDIA_PLAYER_DATA*  pDialogData : [in] MediaPlayer dialog data
// HWND                hDialog     : [in] MediaPlayer dialog window handle
// CONST BOOL          bPlaying     : [in] 
// 
VOID  setMediaPlayerDialogPlaybackState(MEDIA_PLAYER_DATA*  pDialogData, HWND  hDialog, CONST BOOL  bPlaying)
{
   // [STARTING] Create the playback-progress timer if it doesn't exist
   if (bPlaying AND !pDialogData->bPlaying)
      SetTimer(hDialog, iPlaybackProgressTimerID, 50, NULL);

   // [STOPPING] Destroy the playback-progress timer if it exists
   else if (!bPlaying AND pDialogData->bPlaying)
      KillTimer(hDialog, iPlaybackProgressTimerID);
   
   /// Set new state and update dialog controls
   pDialogData->bPlaying = bPlaying;
   updateMediaPlayerDialogControls(pDialogData, hDialog);
}

/// Function name  : setMediaPlayerDialogVideoPosition
// Description     : Set the video size and position based on the current dialog size
// 
// MEDIA_PLAYER_DATA*  pDialogData : [in] MediaPlayer dialog data
// HWND                hDialog     : [in] MediaPlayer dialog window handle
// 
VOID  setMediaPlayerDialogVideoPosition(MEDIA_PLAYER_DATA*  pDialogData, HWND  hDialog)
{
   RECT       rcVideoTarget;   // Video destination rectangle
   SIZE       siTargetSize;    // Size of the hidden 'video area' static control
   LONG       iAspectX,        // Video Clip aspect ratio X component
              iAspectY,        // Video clip aspect ratio Y component
              iDummy;          // Dummy value for 'GetNativeVideoSize'
   HRESULT    hResult;         // Operation result

   // [CHECK] current item is a video clip
   ASSERT(pDialogData->pCurrentItem->eType == MIT_VIDEO_CLIP);

   // Prepare
   utilZeroObject(&rcVideoTarget, RECT);

   /// Retrieve video-clip aspect ratio
   hResult = pDialogData->pVideoCtrl->GetNativeVideoSize(&iDummy, &iDummy, &iAspectX, &iAspectY);
   ASSERT(SUCCEEDED(hResult));

   // Get the 'video picture control' rectangle
   utilGetDlgItemRect(hDialog, IDC_MEDIA_VIDEO, &rcVideoTarget);
   utilConvertRectangleToSize(&rcVideoTarget, &siTargetSize);

   // Adjust to match input aspect ratio, then centre in dialog
   rcVideoTarget.right = rcVideoTarget.left + (siTargetSize.cy * (iAspectX / iAspectY));
   OffsetRect(&rcVideoTarget, (siTargetSize.cx - rcVideoTarget.right + rcVideoTarget.left) / 2, NULL);

   /// Set output rectangle
   hResult = pDialogData->pVideoCtrl->SetVideoPosition(NULL, &rcVideoTarget);
   ASSERT(SUCCEEDED(hResult));
}


/// Function name  : performMediaPlayerDialogSoundEffectHack
// Description     : Extract the desired sound effect and save it in the physical FS
// 
// CONST TCHAR*  szFileSubPath  : [in] Game sub-path of the SFX to load
// 
// Return Value   : TRUE if succesful, FALSE otherwise
// 
BOOL  performMediaPlayerDialogSoundEffectHack(CONST TCHAR*  szFileSubPath)
{
   ERROR_QUEUE*  pErrorQueue;
   BOOL          bResult;

   // Prepare
   pErrorQueue = createErrorQueue();

   // Extract the file from the game data and copy to the equivilent position in the physical FS
   bResult = performFileSystemCopy(getFileSystem(), szFileSubPath, pErrorQueue);

   // Cleanup and return
   deleteErrorQueue(pErrorQueue);
   return bResult;
}


/// Function name  : updateMediaPlayerDialogControls
// Description     : Adjust the controls to reflect the current playback state
// 
// MEDIA_PLAYER_DATA*  pDialogData : [in] MediaPlayer dialog data
// HWND                hDialog     : [in] MediaPlayer dialog window handle
// 
// Return Value   : TRUE if succesful, FALSE otherwise
// 
BOOL  updateMediaPlayerDialogControls(MEDIA_PLAYER_DATA*  pDialogData, HWND  hDialog)
{
   REFERENCE_TIME  timeCurrentPosition,    // Time-code of the current playback position
                   timeProgress;           // Distance between current playback position and start playback position
   UINT            iProgress;              // 'timeProgress' as a percentage
   HRESULT         hResult;                // Operation result

   /// Enable/Disable 'Play' and 'Pause' buttons based on playing state
   EnableWindow(GetDlgItem(hDialog, IDC_MEDIA_PLAY), !pDialogData->bPlaying);
   EnableWindow(GetDlgItem(hDialog, IDC_MEDIA_PAUSE), pDialogData->bPlaying);

   /// Update the playback progress bar
   if (pDialogData->pSeeking)
   {
      // Determine current position
      hResult = pDialogData->pSeeking->GetCurrentPosition(&timeCurrentPosition);
      if (FAILED(hResult))
         return FALSE;

      // Calculate current position as a percentage
      timeProgress = (timeCurrentPosition - pDialogData->timeStart);
      iProgress    = (UINT)((__int64)100 * timeProgress / pDialogData->timeDuration);

      // Set range and progress
      SendDlgItemMessage(hDialog, IDC_MEDIA_PROGRESS, PBM_SETRANGE32, 0, 100);
      SendDlgItemMessage(hDialog, IDC_MEDIA_PROGRESS, PBM_SETPOS, iProgress, NULL);
   }

   return TRUE;
}


/// Function name  : updateMediaPlayerDialogInterfaces
// Description     : Create the appropriate DirectShow interfaces to play the specified MediaItem
// 
// MEDIA_PLAYER_DATA*  pDialogData : [in] MediaPlayer dialog data
// HWND                hDialog     : [in] MediaPlayer dialog window handle
// MEDIA_ITEM*         pItem       : [in] MediaItem to create the interfaces for. This will be stored in the
//                                          dialog data as the current item
// 
// Return Value   : S_OK if succesful
// 
HRESULT  updateMediaPlayerDialogInterfaces(MEDIA_PLAYER_DATA*  pDialogData, HWND  hDialog, MEDIA_ITEM*  pItem)
{
   HRESULT  hResult;     // Operation Result

   // Prepare
   hResult = S_OK;

   // Stop the graph if it's currently running
   if (pDialogData->pController)
      hResult = pDialogData->pController->Stop();

   /// Release existing interfaces
   if (pDialogData->pCurrentItem)
   {
      // Un-register from event notification
      if (pDialogData->pEvents)
      {
         pDialogData->pEvents->SetNotifyWindow(NULL, NULL, NULL);
         utilReleaseInterface(pDialogData->pEvents);
      }
      // Cleanup Graph and Seeking controllers
      utilReleaseInterface(pDialogData->pSeeking);
      utilReleaseInterface(pDialogData->pController);
      // Cleanup video control
      utilReleaseInterface(pDialogData->pVideoCtrl);
      // Cleanup Graph manager
      utilReleaseInterface(pDialogData->pGraph);
   }

   // Update the current item
   pDialogData->pCurrentItem = pItem;

   /// Create appropriate interfaces current item type
   if (pDialogData->pCurrentItem)
      hResult = createMediaPlayerDialogInterfaces(pDialogData, hDialog);

   return hResult;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          MESSAGE HANDLERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onMediaPlayerDialogCommand
// Description     : Performs MediaPlayerDialog WM_COMMAND processing
// 
// MEDIA_PLAYER_DATA*  pDialogData     : [in] MediaPlayer dialog data
// HWND                hDialog         : [in] MediaPlayer dialog window handle
// CONST UINT          iControlID      : [in] ID of the control sending the command
// CONST UINT          iNotification   : [in] [CONTROLS] Notification code of the command being sent
// HWND                hCtrl           : [in] [CONTROLS] Window handle of the control sending the command
// 
// Return Value   : TRUE if processed, FALSE otherwise
// 
BOOL   onMediaPlayerDialogCommand(MEDIA_PLAYER_DATA*  pDialogData, HWND  hDialog, CONST UINT  iControlID, CONST UINT  iNotification, HWND  hCtrl)
{
   switch (iControlID)
   {
   // [PLAY BUTTON] -- Play media
   case IDC_MEDIA_PLAY:
      return onMediaPlayerDialogCommandPlay(pDialogData, hDialog);

   // [PAUSE BUTTON] -- Pause media playback
   case IDC_MEDIA_PAUSE:
      return onMediaPlayerDialogCommandPause(pDialogData, hDialog);
   }

   return TRUE;
}


/// Function name  : onMediaPlayerDialogCommandLoad
// Description     : Create the DirectShow interfaces and load the specified item
// 
// MEDIA_PLAYER_DATA*  pDialogData  : [in] Media Player dialog data
// HWND                hDialog      : [in] Media player dialog window handle
// MEDIA_ITEM*         pItem        : [in] Media Item to play
// 
// Return Value   : TRUE if succesful, FALSE otherwise
// 
BOOL   onMediaPlayerDialogCommandLoad(MEDIA_PLAYER_DATA*  pDialogData, HWND  hDialog, MEDIA_ITEM*  pItem)
{
   HRESULT  hResult;      // Operation Result
   TCHAR*   szFullPath;   // Full file path for the media item
   
   // [CHECK] Stop graph if currently playing
   if (pDialogData->bPlaying)
      pDialogData->pController->Stop();

   // Re-create the appropriate DirectShow interfaces
   hResult = updateMediaPlayerDialogInterfaces(pDialogData, hDialog, pItem);   
   if (FAILED(hResult))
      return FALSE;

   /// Assemble the file path
   szFullPath = utilCreateEmptyPath();
   switch (pItem->eType)
   {
   case MIT_SOUND_EFFECT:
      StringCchPrintf(szFullPath, MAX_PATH, TEXT("s\\%03u.wav"), pItem->iID);
      performMediaPlayerDialogSoundEffectHack(szFullPath);
      StringCchPrintf(szFullPath, MAX_PATH, TEXT("%ss\\%03u.wav"), getFileSystem()->szGameFolder, pItem->iID);
      break;

   case MIT_SPEECH_CLIP:   
      // Generate a physical path to the speech file
      StringCchPrintf(szFullPath, MAX_PATH, TEXT("%smov\\00144.dat"), getFileSystem()->szGameFolder);
      break;

   case MIT_VIDEO_CLIP:
      // Generate a physical path to the first movie file
      StringCchPrintf(szFullPath, MAX_PATH, TEXT("%smov\\00001.dat"), getFileSystem()->szGameFolder);
      break;
   }

   /// Attempt to build the filter graph
   hResult = pDialogData->pGraph->RenderFile(szFullPath, NULL);
   utilDeleteString(szFullPath);
   if (FAILED(hResult))
      return FALSE;

   // Set appropriate start and end positions
   hResult = setMediaPlayerDialogPlaybackPositions(pDialogData, hDialog);
   if (FAILED(hResult))
      return FALSE;

   // Update the dialog 
   setMediaPlayerDialogPlaybackState(pDialogData, hDialog, FALSE);
   
   // Cleanup and return
   return SUCCEEDED(hResult) ? TRUE : FALSE;
}


/// Function name  : onMediaPlayerDialogCommandPause
// Description     : Pause the current media item
// 
// MEDIA_PLAYER_DATA*  pDialogData  : [in] Media Player dialog data
// HWND                hDialog      : [in] Media player dialog window handle
// 
// Return Value   : TRUE if succesful, FALSE otherwise
// 
BOOL   onMediaPlayerDialogCommandPause(MEDIA_PLAYER_DATA*  pDialogData, HWND  hDialog)
{
   HRESULT     hResult;      // Operation result

   // [CHECK] MediaItem has been loaded
   ASSERT(pDialogData->pCurrentItem AND pDialogData->pGraph AND pDialogData->pController);

   /// Attempt to pause the media item
   hResult = pDialogData->pController->Pause();

   // Update dialog
   setMediaPlayerDialogPlaybackState(pDialogData, hDialog, FALSE);
   
   // Cleanup and return
   return SUCCEEDED(hResult) ? TRUE : FALSE;
}


/// Function name  : onMediaPlayerDialogCommandPlay
// Description     : Play the current media item
// 
// MEDIA_PLAYER_DATA*  pDialogData  : [in] Media Player dialog data
// HWND                hDialog      : [in] Media player dialog window handle
// 
// Return Value   : TRUE if succesful, FALSE otherwise
// 
BOOL   onMediaPlayerDialogCommandPlay(MEDIA_PLAYER_DATA*  pDialogData, HWND  hDialog)
{
   HRESULT    hResult;      // Operation result
   
   // [CHECK] MediaItem has been loaded
   ASSERT(pDialogData->pCurrentItem AND pDialogData->pGraph AND pDialogData->pController);
   // [CHECK] Item shouldn't be already playing
   ASSERT(pDialogData->bPlaying == FALSE);

   /// Attempt to play the media item
   hResult = pDialogData->pController->Run();
      
   // Update dialog
   setMediaPlayerDialogPlaybackState(pDialogData, hDialog, TRUE);
   
   // Cleanup and return
   return SUCCEEDED(hResult) ? TRUE : FALSE;
}


/// Function name  : onMediaPlayerDialogDestroy
// Description     : Cleanup the DirectShow interfaces
// 
// MEDIA_PLAYER_DATA*  pDialogData  : [in] MediaPlayer dialog data
// HWND                hDialog      : [in] MediaPlayer dialog window handle
// 
VOID   onMediaPlayerDialogDestroy(MEDIA_PLAYER_DATA*  pDialogData, HWND  hDialog)
{
   // Close DirectShow interfaces
   updateMediaPlayerDialogInterfaces(pDialogData, hDialog, NULL);

   // Destroy Button icons ImageList
   ImageList_Destroy(pDialogData->hButtonIcons);
   // Delete player background brush
   DeleteObject(pDialogData->hPlayerBackground);

   // Unsubclass progress bar
   SetWindowLong(GetDlgItem(hDialog,IDC_MEDIA_PROGRESS), GWL_WNDPROC, (LONG)pDialogData->wndprocProgressCtrl);

   // Destroy dialog data
   utilDeleteObject(pDialogData);
   SetWindowLong(hDialog, DWL_USER, NULL);
}


/// Function name  : onMediaPlayerDialogNotify
// Description     : 
// 
// MEDIA_PLAYER_DATA*  pDialogData : [in] 
// HWND                hDialog     : [in] 
// NMHDR*              pMessage    : [in] 
// 
// Return Value   : 
// 
BOOL  onMediaPlayerDialogNotify(MEDIA_PLAYER_DATA*  pDialogData, HWND  hDialog, NMHDR*  pMessage)
{
   switch (pMessage->code)
   {
   /// [CUSTOM DRAW]
   case NM_CUSTOMDRAW:
      ASSERT(pMessage->idFrom != IDC_MEDIA_PROGRESS);

      switch (pMessage->idFrom)
      {
      case IDC_MEDIA_PLAY:
      case IDC_MEDIA_PAUSE:
         return drawMediaPlayerButtonCtrl(pDialogData, hDialog, pMessage->idFrom, (NMCUSTOMDRAW*)pMessage);
      }
   }

   return FALSE;
}


/// Function name  : onMediaPlayerDialogPlaybackComplete
// Description     : 
// 
// MEDIA_PLAYER_DATA*  pDialogData  : [in] 
// HWND                hDialog      : [in] 
// 
// Return Value   : 
// 
BOOL  onMediaPlayerDialogPlaybackComplete(MEDIA_PLAYER_DATA*  pDialogData, HWND  hDialog)
{
   // Stop playback
   pDialogData->pController->Stop();

   // Seek back to the start of the item
   setMediaPlayerDialogPlaybackPositions(pDialogData, hDialog);

   // Update dialog
   setMediaPlayerDialogPlaybackState(pDialogData, hDialog, FALSE);
   return TRUE;
}


/// Function name  : onMediaPlayerDialogPaint
// Description     : 
// 
// MEDIA_PLAYER_DATA*  pDialogData   : [in] 
// HWND  hDialog   : [in] 
// 
// Return Value   : TRUE if processed, FALSE if not
// 
BOOL  onMediaPlayerDialogPaint(MEDIA_PLAYER_DATA*  pDialogData, HWND  hDialog)
{
   PAINTSTRUCT  oPaintData;

   // [CHECK] Current item is a video clip
   if (pDialogData->pCurrentItem == NULL OR pDialogData->pCurrentItem->eType != MIT_VIDEO_CLIP)
      return FALSE;

   // Prepare
   BeginPaint(hDialog, &oPaintData);

   // Repaint video clip
   pDialogData->pVideoCtrl->RepaintVideo(hDialog, oPaintData.hdc);

   // Cleanup and return
   EndPaint(hDialog, &oPaintData);
   return TRUE;
}


/// Function name  : onMediaPlayerDialogResize
// Description     : Resize the dialog controls
// 
// MEDIA_PLAYER_DATA*  pDialogData  : [in] Media Player dialog data
// HWND                hDialog      : [in] Window handle of the Media Player dialog
// CONST UINT          iWidth       : [in] New width of the dialog, in pixels
// CONST UINT          iHeight      : [in] New height of the dialog, in pixels
// 
VOID  onMediaPlayerDialogResize(MEDIA_PLAYER_DATA*  pDialogData, HWND  hDialog, CONST UINT  iWidth, CONST UINT  iHeight)
{
   RECT   rcClientRect,          // Client rectangle of the media player dialog
          rcControlRect;         // Rectangle of the control currently being processed
   SIZE   siClientSize,          // Size of the client rectangle
          siControlSize,         // Size of the control currently being processed
          siButtonSize;          // Size of the button controls
   INT    iBorderHeight,         // Height of the gap between the 'Video' and 'Control' areas
          iControlHeight,        // Height of the controls in the controls area
          iControlGap;           // Width of the gap between button and progress bar controls

   /// Get the dimensions of various dialog components
   // Get dialog dimensions
   GetClientRect(hDialog, &rcClientRect);
   utilConvertRectangleToSize(&rcClientRect, &siClientSize);

   // Get control area dimensions
   GetWindowRect(GetDlgItem(hDialog, IDC_MEDIA_PLAY), &rcControlRect);
   iControlHeight = (rcControlRect.bottom - rcControlRect.top);

   // Get button dimensions
   GetWindowRect(GetDlgItem(hDialog, IDC_MEDIA_PLAY), &rcControlRect);
   utilConvertRectangleToSize(&rcControlRect, &siButtonSize);

   // Calculate the video/control area gap and the distance between controls
   iBorderHeight = 3 * GetSystemMetrics(SM_CYEDGE);
   iControlGap = GetSystemMetrics(SM_CXEDGE);
   
   /// Divide the window into two areas - the 'Controls' area at the bottom with the progress bar and control buttons
   ///  and the 'Video' area - which is everything else.  A small (empty) border separates the two areas.

   // [VIDEO WINDOW] - Extend over entire client area but leave an area at the bottom for the controls
   rcControlRect         = rcClientRect;
   rcControlRect.bottom -= (iBorderHeight + iControlHeight);

   utilConvertRectangleToSize(&rcControlRect, &siControlSize);
   MoveWindow(GetDlgItem(hDialog,IDC_MEDIA_VIDEO), rcControlRect.left, rcControlRect.top, siControlSize.cx, siControlSize.cy, TRUE);

   // [PAUSE BUTTON] - Position in the bottom right hand corner
   rcControlRect = rcClientRect;
   rcControlRect.left = rcControlRect.right - siButtonSize.cx;
   rcControlRect.top  = rcControlRect.bottom - siButtonSize.cy;

   utilConvertRectangleToSize(&rcControlRect, &siControlSize);
   MoveWindow(GetDlgItem(hDialog,IDC_MEDIA_PAUSE), rcControlRect.left, rcControlRect.top, siControlSize.cx, siControlSize.cy, TRUE);

   // [PLAY BUTTON] - Position to the left of the 'Pause' button
   OffsetRect(&rcControlRect, -(siButtonSize.cx + iBorderHeight), 0);

   utilConvertRectangleToSize(&rcControlRect, &siControlSize);
   MoveWindow(GetDlgItem(hDialog,IDC_MEDIA_PLAY), rcControlRect.left, rcControlRect.top, siControlSize.cx, siControlSize.cy, TRUE);

   // [OPERATION_PROGRESS BAR] - Position over the remainder of the 'Controls' area
   OffsetRect(&rcControlRect, -(siButtonSize.cx + iBorderHeight), 0);
   rcControlRect.left = rcClientRect.left + iBorderHeight;
   rcControlRect.top  = rcControlRect.bottom - iControlHeight;

   utilConvertRectangleToSize(&rcControlRect, &siControlSize);
   MoveWindow(GetDlgItem(hDialog,IDC_MEDIA_PROGRESS), rcControlRect.left, rcControlRect.top, siControlSize.cx, siControlSize.cy, TRUE);

   // [VIDEO CLIPS] -- Resize the video
   if (pDialogData->pCurrentItem != NULL AND pDialogData->pCurrentItem->eType == MIT_VIDEO_CLIP)
      setMediaPlayerDialogVideoPosition(pDialogData, hDialog);
}


/// Function name  : dlgprocMediaPlayerDialog
// Description     : Media Player dialog window procedure
// 
INT_PTR  dlgprocMediaPlayerDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   MEDIA_PLAYER_DATA*   pDialogData;
   LONG                 iEventID;
   LONG_PTR             pEventParam1, 
                        pEventParam2;

   // Get dialog data
   pDialogData = getMediaPlayerDialogData(hDialog);

   switch (iMessage)
   {
   /// [CREATION] - Store dialog data and initialise controls
   case WM_INITDIALOG:
      initMediaPlayerDialog((MEDIA_PLAYER_DATA*)lParam, hDialog);
      return TRUE;

   /// [DESTRUCTION] - Cleanup dialog data and DirectShow
   case WM_DESTROY:
      onMediaPlayerDialogDestroy(pDialogData, hDialog);
      return TRUE;

   /// [COMMAND PROCESSING] - Play/Pause current media
   case WM_COMMAND:
      return onMediaPlayerDialogCommand(pDialogData, hDialog, LOWORD(wParam), HIWORD(wParam), (HWND)lParam);

   // [NOTIFICATIONS]
   case WM_NOTIFY:
      return onMediaPlayerDialogNotify(pDialogData, hDialog, (NMHDR*)lParam);

   /// [DATA INPUT] - Load the requested MediaItem
   case UM_LOAD_MEDIA_ITEM:
      onMediaPlayerDialogCommandLoad(pDialogData, hDialog, (MEDIA_ITEM*)lParam);
      return TRUE;

   /// [DIRECT-SHOW EVENT] - Retrieve and dispatch event
   case UM_MEDIA_EVENT:
      // [CHECK] Event interface exists
      ASSERT(pDialogData->pEvents);
      
      // Retrieve next event in the queue.  Specify no timeout.
      while (SUCCEEDED(pDialogData->pEvents->GetEvent(&iEventID, &pEventParam1, &pEventParam2, NULL)))
      {
         // Free parameter resources
         pDialogData->pEvents->FreeEventParams(iEventID, pEventParam1, pEventParam2);
         // Dispatch event
         switch (iEventID)
         {
         case EC_COMPLETE:   onMediaPlayerDialogPlaybackComplete(pDialogData, hDialog);  break;
         case EC_USERABORT:  break;
         case EC_ERRORABORT: break;
         }
      } 
      return TRUE;

   // [PAINT] - Repaint the Video Clip
   case WM_PAINT:
      return onMediaPlayerDialogPaint(pDialogData, hDialog);

   // [VISUAL STYLES] - Draw the background colour
   case WM_CTLCOLORDLG:
      //return (INT_PTR)GetStockObject(BLACK_BRUSH);
      return (INT_PTR)pDialogData->hPlayerBackground;

   // [RESIZE] - Resize the dialog controls
   case WM_SIZE:
      onMediaPlayerDialogResize(pDialogData, hDialog, LOWORD(lParam), HIWORD(lParam));
      return TRUE;

   // [PLAYBACK TIMER] - Update the playback progress bar
   case WM_TIMER:
      if (wParam == iPlaybackProgressTimerID)
         updateMediaPlayerDialogControls(pDialogData, hDialog);
      return TRUE;
   }

   return FALSE;
}


/// Function name  : wndprocMediaPlayerDialogProgressCtrl
// Description     : Window procedure for the media player dialog playback progress bar
// 
LRESULT   wndprocMediaPlayerDialogProgressCtrl(HWND  hCtrl, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   static MEDIA_PLAYER_DATA*  pDialogData = NULL;
   PAINTSTRUCT                oPaintData;

   switch (iMessage)
   {
   // [INPUT DATA] - Store dialog data containing original window procedure
   case UM_INIT_MEDIA_PLAYER:
      pDialogData = (MEDIA_PLAYER_DATA*)lParam;
      return 0;

   // [PAINT] - Custom draw
   case WM_PAINT:
      BeginPaint(hCtrl, &oPaintData);
      drawMediaPlayerProgressCtrl(oPaintData.hdc, &oPaintData.rcPaint, hCtrl, pDialogData);
      EndPaint(hCtrl, &oPaintData);
      return 0;
   }

   // [DEFAULT] - Pass to original window procedure
   return CallWindowProc(pDialogData->wndprocProgressCtrl, hCtrl, iMessage, wParam, lParam);
}