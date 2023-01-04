//
// Custom Drawing.cpp : General Custom Drawing helpers
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



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onOwnerDrawStaticImage
// Description     : Owner draws a static picture control
// 
// DRAWITEMSTRUCT*  pDrawData  : [in] OwnerDraw data
// CONST TCHAR*     szResource : [in] Resource ID of the icon
// CONST UINT       iType      : [in] IMAGE_ICON or IMAGE_BITMAP
// CONST UINT       iWidth     : [in] Image width
// CONST UINT       iHeight    : [in] Image height
// 
// Return Value   : TRUE
// 
ControlsAPI
BOOL  onOwnerDrawStaticImage(DRAWITEMSTRUCT*  pDrawData, CONST TCHAR*  szResource, CONST UINT  iType, CONST UINT  iWidth, CONST UINT  iHeight)
{
   HBITMAP  hBitmap,       // Bitmap handle
            hOldBitmap;    //
   HICON    hIcon;         // Icon handle
   HDC      hMemoryDC;     // Memory DC

   // [CHECK] Ensure window is a visible static
   if (pDrawData->CtlType != ODT_STATIC OR !IsWindowVisible(pDrawData->hwndItem))
      return FALSE;

   // Examine type   
   switch (iType)
   {
   case IMAGE_ICON:
      // Load icon
      hIcon = (HICON)LoadImage(getResourceInstance(), szResource, iType, iWidth, iHeight, LR_LOADTRANSPARENT);

      /// [ICON] Draw icon
      DrawIconEx(pDrawData->hDC, pDrawData->rcItem.left, pDrawData->rcItem.top, hIcon, iWidth, iHeight, NULL, NULL, DI_IMAGE WITH DI_MASK);

      // Cleanup
      DestroyIcon(hIcon);
      break;

   case IMAGE_BITMAP:
      // Load and select bitmap
      hMemoryDC  = CreateCompatibleDC(pDrawData->hDC);
      hBitmap    = (HBITMAP)LoadImage(getResourceInstance(), szResource, iType, iWidth, iHeight, LR_LOADTRANSPARENT);
      hOldBitmap = SelectBitmap(hMemoryDC, hBitmap);

      /// [BITMAP] Draw bitmap
      BitBlt(pDrawData->hDC, 0, 0, iWidth, iHeight, hMemoryDC, 0, 0, SRCCOPY);

      // Cleanup
      SelectBitmap(hMemoryDC, hOldBitmap);
      DeleteBitmap(hBitmap);
      DeleteDC(hMemoryDC);
      break;
   }

   // Return TRUE   
   return TRUE;
}


/// Function name  :  onOwnerDrawStaticTitle
// Description     : Convenience macro for drawing a Vista-Style dialog title
// 
// LPARAM  lParam : [in] DRAWITEMSTRUCT* : [in] OwnerDraw data
// 
// Return Value   : TRUE
// 
ControlsAPI 
BOOL   onOwnerDrawStaticTitle(LPARAM  lParam)
{
   static CONST COLORREF  clColour = RGB(42,84,153);
   CONST TCHAR*           szFont   = (getAppWindowsVersion() >= WINDOWS_VISTA ? TEXT("Segoe UI Light") : TEXT("Tahoma"));
 
   /// Draw text
   return onOwnerDrawStaticText((DRAWITEMSTRUCT*)lParam, szFont, 14, clColour, COLOR_WINDOW, FALSE);
}


/// Function name  :  onOwnerDrawStaticHeading
// Description     : Convenience macro for drawing a Vista-Style dialog sub-heading
// 
// LPARAM  lParam : [in] DRAWITEMSTRUCT* : [in] OwnerDraw data
// 
// Return Value   : TRUE
// 
ControlsAPI 
BOOL   onOwnerDrawStaticHeading(LPARAM  lParam)
{
   WINDOWS_VERSION  eVersion = getAppWindowsVersion();
   CONST TCHAR*     szFont   = (eVersion >= WINDOWS_VISTA ? TEXT("Segoe UI Bold") : TEXT("MS Shell Dlg 2"));

   /// Draw text
   return onOwnerDrawStaticText((DRAWITEMSTRUCT*)lParam, szFont, 9, GetSysColor(COLOR_WINDOWTEXT), COLOR_WINDOW, eVersion < WINDOWS_VISTA);
}


/// Function name  : onOwnerDrawStaticText
// Description     : Owner draws a static text control
// 
// DRAWITEMSTRUCT*  pDrawData  : [in] OwnerDraw data
// CONST TCHAR*     szFont     : [in] Desired font
// CONST UINT       iPointSize : [in] Font size
// CONST COLORREF   clColour   : [in] Text colour
// CONST BOOL       bBold      : [in] Bold or not
// 
// Return Value   : TRUE
// 
ControlsAPI
BOOL  onOwnerDrawStaticText(DRAWITEMSTRUCT*  pDrawData, CONST TCHAR*  szFont, CONST UINT  iPointSize, CONST COLORREF  clText, CONST UINT  iBackgroundColour, CONST BOOL  bBold)
{
   DC_STATE*  pPrevState;       // Previous drawing state
   TCHAR*     szText;           // Control text
   HFONT      hFont;            // Desired font
   DWORD      dwStyle;          // window style
   UINT       iDrawStyle;       // Window styles converted to DrawText drawing flags

   // [CHECK] Ensure window is a visible static
   if (pDrawData->CtlType != ODT_STATIC OR !IsWindowVisible(pDrawData->hwndItem)) // /*utilIncludes(GetWindowStyle(pDrawData->hwndItem), SS_ICON WITH SS_BITMAP) OR*/
      return FALSE;
   
   // Prepare
   dwStyle    = GetWindowStyle(pDrawData->hwndItem);
   szText     = utilGetWindowText(pDrawData->hwndItem);
   pPrevState = utilCreateDeviceContextState(pDrawData->hDC);
   iDrawStyle = DT_LEFT WITH DT_NOPREFIX;

   // Setup font and drawing mode
   utilSetDeviceContextFont(pPrevState, hFont = utilCreateFont(pDrawData->hDC, szFont, iPointSize, bBold, FALSE, FALSE), clText);
   utilSetDeviceContextBackgroundMode(pPrevState, TRANSPARENT);

   // [ALIGNMENT] Set text alignment 
   if (dwStyle INCLUDES CSS_CENTRE) //SS_CENTERIMAGE)
      iDrawStyle |= DT_CENTER;
   else if (dwStyle INCLUDES CSS_RIGHT)   //SS_RIGHTJUST)
      iDrawStyle |= DT_RIGHT;

   if (dwStyle INCLUDES SS_CENTERIMAGE)
      iDrawStyle |= DT_VCENTER;

   /// [BACKGROUND]
   utilFillSysColourRect(pDrawData->hDC, &pDrawData->rcItem, iBackgroundColour);

   /// [TEXT] Draw text
   DrawText(pDrawData->hDC, szText, lstrlen(szText), &pDrawData->rcItem, iDrawStyle);

   // Cleanup and return TRUE
   utilDeleteDeviceContextState(pPrevState);
   utilDeleteString(szText);
   DeleteFont(hFont);
   return TRUE;
}


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////


