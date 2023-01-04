//
// Tooltip Windows.cpp : Tooltips
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"


/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                      CREATION / DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createTooltipWindow
// Description     : Create a Tooltip Window
// 
// HWND  hParentWnd : [in] Parent window
// 
// Return type : Window handle to the new Tooltip window
//
HWND  createTooltipWindow(HWND  hParentWnd)
{
   HWND  hTooltip;

   // Create Tooltip
   hTooltip = CreateWindowEx(NULL, TOOLTIPS_CLASS, NULL, WS_POPUP WITH TTS_NOPREFIX WITH TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hParentWnd, NULL, getResourceInstance(), NULL);
   SetWindowPos(hTooltip, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE WITH SWP_NOSIZE WITH SWP_NOACTIVATE);

   // Return window handle
   return hTooltip;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                           HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : getTooltipControlID
// Description     : Extract the control ID from tooltip notification data, which may have been passed as a window handle
// 
// NMTTDISPINFO*  pData  : [in] Tooltip notification data
// 
// Return Value   : Control ID
// 
UINT  getTooltipControlID(NMTTDISPINFO*  pData)
{
   UINT  iControlID; // Resultant ID

   // Determine if HWND is being passed instead of a control ID
   if (pData->uFlags INCLUDES TTF_IDISHWND)
      iControlID = GetDlgCtrlID((HWND)pData->hdr.idFrom); // Extract HWND INCLUDES convert
   else
      iControlID = pData->hdr.idFrom; 

   // Return control ID
   return iControlID;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : addTooltipTextToControl
// Description     : Associate a control ID with an existing tooltip control created with 'createTooltipWindow'
// 
// HWND        hTooltip   : [in] Window handle of tooltip
// HWND        hParentWnd : [in] Window handle of tooltip's and control's parent window
// CONST UINT  iControlID : [in] ID of the control to add a tooltip for. A string resource with this ID must 
//                               also exist in the resource DLL.
// 
VOID  addTooltipTextToControl(HWND  hTooltip, HWND  hParentWnd, CONST UINT  iControlID)
{
   TOOLINFO  oTipData;      // Tooltip definition
   
   // Define tip
   oTipData.cbSize   = sizeof(TOOLINFO);
   oTipData.hinst    = getResourceInstance();
   oTipData.lpszText = LPSTR_TEXTCALLBACK;
   oTipData.hwnd     = hParentWnd;
   oTipData.uId      = (UINT_PTR)GetDlgItem(hParentWnd, iControlID);
   oTipData.uFlags   = TTF_SUBCLASS WITH TTF_IDISHWND;
   // Add tip
   SendMessage(hTooltip, TTM_ADDTOOL, NULL, (LPARAM)&oTipData);
}

/// Function name  : onTooltipRequestText
// Description     : Lookup the text for a tooltip previously created with 'addTooltipTextToControl'
// 
// NMTTDISPINFO*  pData : [in/out] WM_NOTIFY data
// 
// Return Type     : TRUE
//
BOOL  onTooltipRequestText(NMTTDISPINFO*  pData)
{
   static TCHAR  szLargeTooltip[512];   // Large buffer for displaying large tooltips
   UINT          iTooltipLength;
   
   // Load string resource using the target control's dialog controlID
   iTooltipLength  = loadString(getTooltipControlID(pData), szLargeTooltip, 512);
   pData->lpszText = szLargeTooltip;
   pData->hinst    = NULL;

   // Force the Tooltip control (not the target ctrl) to display multi-line
   if (iTooltipLength == 0)
      StringCchCopy(szLargeTooltip, 512, TEXT("Tooltip not found"));
   else if (iTooltipLength > 80)
      SendMessage(pData->hdr.hwndFrom, TTM_SETMAXTIPWIDTH, 0, 300);

   return TRUE;
}


/// Function name  : onTooltipRequestSpecifiedText
// Description     : Provide a specific tooltip
// 
// NMTTDISPINFO*  pData       : [in/out] WM_NOTIFY data
// CONST UINT     iResourceID : [in]     String resource ID
// 
// Return Type     : TRUE
BOOL  onTooltipRequestSpecifiedText(NMTTDISPINFO*  pData, CONST UINT  iResourceID)
{
   static TCHAR  szLargeTooltip[512];   // Large buffer for displaying large tooltips
   UINT          iTooltipLength;
   
   // Load specified string resource
   iTooltipLength  = loadString(iResourceID, szLargeTooltip, 512);
   pData->lpszText = szLargeTooltip;
   pData->hinst    = NULL;

   // Force the Tooltip control to display multi-line
   if (iTooltipLength == 0)
      StringCchCopy(szLargeTooltip, 512, TEXT("Tooltip not found"));
   else if (iTooltipLength > 80)
      SendMessage(pData->hdr.hwndFrom, TTM_SETMAXTIPWIDTH, 0, 300);

   return TRUE;
}




