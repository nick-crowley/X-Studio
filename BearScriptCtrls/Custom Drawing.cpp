//
// Custom Drawing.cpp : Custom drawing functions accessible to the controls and UI components
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"


/// ////////////////////////////////////////////////////////////////////////////////////////
///                                 CONSTANTS / GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////

// Defines the colour of the custom highlight rectangle
//
CONST COLORREF   clWhite         = RGB(255, 255, 255),    // White top
                 clMediumWhite   = RGB(234, 235, 243),    // Off-white
                 clDarkWhite     = RGB(234, 235, 243),    // Very Light grey
                 clMediumGrey    = RGB(118, 118, 118),    // Medium Grey
                 clLightCyan     = RGB(236, 245, 254),    // Very light cyan
                 clMediumCyan    = RGB(227, 240, 252),    // Light cyan
                 clDarkCyan      = RGB(214, 233, 252),    // Medium cyan
                 clLightBlue     = RGB(192, 209, 237),    // Light blue 
                 clMediumBlue    = RGB(182, 202, 234),    // Medium blue
                 clDarkBlue      = RGB(139, 172, 222);    // Dark blue

ControlsAPI const COLORREF   clNullColour    = 0xff000000;          // Colour sentinel value

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                  CREATION / DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                       HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : drawGradientRect
// Description     : Draws a shaded rectangle without a border
// 
// HDC             hDC            : [in] Device contenxt
// CONST RECT*     pRect          : [in] Drawing rectangle
// CONST COLORREF  clTopColour    : [in] Colour at the top
// CONST COLORREF  clBottomColour : [in] Colour at the bottom
// CONST DIRECTION eDirection     : [in] Whether gradient is left-right or top-bottom
// 
VOID  drawGradientRect(HDC  hDC, CONST RECT*  pRect, CONST COLORREF  clTopColour, CONST COLORREF  clBottomColour, CONST DIRECTION  eDirection)
{
   TRIVERTEX        oCorners[2];       // Corners of the gradient rectangles
   GRADIENT_RECT    oGradientRect;     // Defines a gradient rectangle

   // Define top corner
   oCorners[0].x      = pRect->left;
   oCorners[0].y      = pRect->top;
   oCorners[0].Red    = GetRValue(clTopColour) << 8;
   oCorners[0].Green  = GetGValue(clTopColour) << 8;
   oCorners[0].Blue   = GetBValue(clTopColour) << 8;
   oCorners[0].Alpha  = 255 << 8;
   oGradientRect.UpperLeft  = 0;

   // Define white corner
   oCorners[1].x      = pRect->right;
   oCorners[1].y      = pRect->bottom;
   oCorners[1].Red    = GetRValue(clBottomColour) << 8;
   oCorners[1].Green  = GetGValue(clBottomColour) << 8;
   oCorners[1].Blue   = GetBValue(clBottomColour) << 8;
   oCorners[1].Alpha  = 255 << 8;
   oGradientRect.LowerRight = 1;

   // Draw top third (WHITE -> COLOUR)
   GradientFill(hDC, oCorners, 2, &oGradientRect, 1, eDirection == VERTICAL ? GRADIENT_FILL_RECT_V : GRADIENT_FILL_RECT_H);
}


/// Function name  : getDialogBackgroundBrush
// Description     : Retrieves the background brush for a themed or non-themed dialog
// 
// Return Value   : Background brush
// 
ControlsAPI
HBRUSH  getDialogBackgroundBrush()
{
   /*HTHEME  hTheme;
   HBRUSH  hBrush;

   if (IsThemeActive() AND (hTheme = OpenThemeData(getAppWindow(), TEXT("Window::Dialog"))))
   {
      hBrush = GetThemeSysColorBrush(hTheme, COLOR_3DFACE);
      CloseThemeData(hTheme);
   }
   else
      hBrush = GetSysColorBrush(COLOR_BTNFACE);

   return hBrush;*/

   return GetSysColorBrush(IsThemeActive() ? COLOR_WINDOW : COLOR_BTNFACE);
}


/// Function name  : getDialogBackgroundColour
// Description     : Retrieves the background colour for a themed or non-themed dialog
//
// Return Value   : Background colour
// 
ControlsAPI
COLORREF  getDialogBackgroundColour()
{
   /*HTHEME    hTheme;
   COLORREF  clColour;

   if (IsThemeActive() AND (hTheme = OpenThemeData(getAppWindow(), TEXT("Window::Dialog"))))
   {
      clColour = GetThemeSysColor(hTheme, COLOR_3DFACE);
      CloseThemeData(hTheme);
   }
   else
      clColour = GetSysColor(COLOR_BTNFACE);

   return clColour;*/

   return GetSysColor(IsThemeActive() ? COLOR_WINDOW : COLOR_BTNFACE);
}


ControlsAPI
COLORREF  getThemeColour(const TCHAR*  szClass, const int  iPart, const int  iState, const int  iProperty, const int  iDefault)
{
   HTHEME    hTheme;
   COLORREF  clColour = NULL;

   // Open theme data
   if (IsThemeActive() AND (hTheme = OpenThemeData(getAppWindow(), szClass)))
   {
      // Extract colour
      GetThemeColor(hTheme, iPart, iState, iProperty, &clColour);
      CloseThemeData(hTheme);
   }
   else
      // [FAILED] Return system colour
      clColour = GetSysColor(iDefault);

   return clColour;
}

ControlsAPI
COLORREF  getThemeSysColour(const TCHAR*  szClass, const int  iSysColour)
{
   HTHEME    hTheme;
   COLORREF  clColour = NULL;

   // Open theme data
   if (IsThemeActive() AND (hTheme = OpenThemeData(getAppWindow(), szClass)))
   {
      // Extract colour
      clColour = GetThemeSysColor(hTheme, iSysColour);
      CloseThemeData(hTheme);
   }
   else
      // [FAILED] Return system colour
      clColour = GetSysColor(iSysColour);

   return clColour;
}

ControlsAPI
HBRUSH  getThemeSysColourBrush(const TCHAR*  szClass, const int  iSysColour)
{
   // Lookup brush
   HTHEME hTheme = (IsThemeActive() ? OpenThemeData(getAppWindow(), szClass) : NULL);
   HBRUSH hBrush = GetThemeSysColorBrush(hTheme, iSysColour);

   // Return brush
   CloseThemeData(hTheme);
   return hBrush;
}

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : drawIcon
// Description     : Draws an icon from an image list transparently
// 
// HIMAGELIST        hImageList  : [in] ImageList containing the icon
// CONST UINT        iIconIndex  : [in] ImageList icon index
// HDC               hDC         : [in] Target DC
// CONST UINT        iX          : [in] X co-ordinate
// CONST UINT        iY          : [in] y co-ordinate
// CONST ICON_STATE  eState      : [in] Icon state
// 
// Return Value   : TRUE if successful, otherwise FALSE
// 
BOOL  drawIcon(HIMAGELIST  hImageList, CONST UINT  iIconIndex, HDC  hDC, CONST UINT  iX, CONST UINT  iY, CONST ICON_STATE  eState)
{
   IMAGELISTDRAWPARAMS   oDrawData;

   // Prepare
   utilZeroObject(&oDrawData, IMAGELISTDRAWPARAMS);
   oDrawData.cbSize = sizeof(IMAGELISTDRAWPARAMS);

   // Set properties
   oDrawData.himl   = hImageList;
   oDrawData.i      = iIconIndex;
   oDrawData.hdcDst = hDC;
   oDrawData.x      = iX;
   oDrawData.y      = iY;
   oDrawData.fStyle = ILD_TRANSPARENT;

   // Set advanced properties
   switch (eState)
   {
   case IS_DISABLED:
      oDrawData.fState  = ILS_SATURATE;
      break;

   case IS_SELECTED:
      oDrawData.rgbFg   = CLR_DEFAULT;
      oDrawData.fStyle |= ILD_BLEND;
      break;
   }

   /// Draw icon
   return ImageList_DrawIndirect(&oDrawData);
}


/// Function name  : drawImageTreeIcon
// Description     : Convenience function for drawing an icon from an app imageTree
// 
// IMAGE_TREE_SIZE   eIconSize : [in] Icon size
// CONST TCHAR*      szIconID  : [in] Resource ID of icon
// HDC               hDC       : [in] Target DC
// CONST UINT        iX        : [in] X co-ordinate
// CONST UINT        iY        : [in] y co-ordinate
// CONST ICON_STATE  eState    : [in] Icon state
// 
// Return Value   : TRUE if successful, otherwise FALSE
// 
ControlsAPI
BOOL  drawImageTreeIcon(CONST IMAGE_TREE_SIZE  eSize, CONST TCHAR*  szIconID, HDC  hDC, CONST UINT  iX, CONST UINT  iY, CONST ICON_STATE  eState)
{
   return drawIcon(getAppImageList(eSize), getAppImageTreeIconIndex(eSize, szIconID), hDC, iX, iY, eState);
}


/// Function name  : drawBlendedIcon
// Description     : Alpha blends an icon
// 
// HDC             hDC           : [in] Target DC
// HICON           hIcon         : [in] Handle of icon to draw
// CONST UINT      iIconSize     : [in] Icon size
// CONST POINT*    ptOutput      : [in] Position to draw
// CONST COLORREF  clBlendColour : [in] Colour to blend with
// CONST FLOAT     fBlending     : [in] Blend ratio 0.0 -> 1.0
// 
/// WORK IN PROGRESS: Doesn't work properly yet. The mask doesn't seem to work properly and the icon is blurry...
//
// Return Value   : TRUE if successful, otherwise FALSE
// 
ControlsAPI 
BOOL  drawBlendedIcon(HDC  hDC, HICON  hIcon, CONST UINT  iIconSize, CONST POINT*  ptOutput, CONST COLORREF  clBlendColour, CONST FLOAT  fBlending)
{
   BLENDFUNCTION  oBlendData;
   ICONINFO       oIconData;
   RECT           rcSourceRect;
   HDC            hIconDC,
                  hBlendDC;
   HBRUSH         hBrush;
   HBITMAP        hBlendBitmap,
                  hOldBitmap1,
                  hOldBitmap2;

   /// Retrieve icon bitmap and mask
   if (!GetIconInfo(hIcon, &oIconData))
      return FALSE;

   // Prepare
   utilZeroObject(&oBlendData, BLENDFUNCTION);
   SetRect(&rcSourceRect, 0, 0, iIconSize, iIconSize);
   hBrush = CreateSolidBrush(clBlendColour);

   // Create memory DC containing the blend colour
   hBlendDC     = CreateCompatibleDC(hDC);
   hBlendBitmap = CreateCompatibleBitmap(hDC, iIconSize, iIconSize);
   hOldBitmap1  = SelectBitmap(hBlendDC, hBlendBitmap);
   FillRect(hBlendDC, &rcSourceRect, hBrush);

   // Create memory DC containing icon
   hIconDC     = CreateCompatibleDC(hDC);
   hOldBitmap2 = SelectBitmap(hIconDC, oIconData.hbmColor);

   // Setup blending
   oBlendData.SourceConstantAlpha = 0x7f;//  (BYTE)(LONG)(fBlending * 255.0f);
   oBlendData.BlendOp = AC_SRC_OVER;

   /// AlphaBlend icon with blend colour
   AlphaBlend(hBlendDC, 0, 0, iIconSize, iIconSize, hIconDC, 0, 0, iIconSize, iIconSize, oBlendData);

   /// Output alphablended icon with mask
   MaskBlt(hDC, ptOutput->x, ptOutput->y, iIconSize, iIconSize, hBlendDC, 0, 0, oIconData.hbmMask, 0, 0, MAKEROP4(SRCCOPY, SRCAND));
   //BitBlt(hDC, ptOutput->x, ptOutput->y, iIconSize, iIconSize, hBlendDC, 0, 0, SRCCOPY);

   // Cleanup
   SelectBitmap(hBlendDC, hOldBitmap1);
   SelectBitmap(hIconDC, hOldBitmap2);
   DeleteBitmap(hBlendBitmap);
   DeleteBrush(hBrush);
   DeleteDC(hBlendDC);
   DeleteDC(hIconDC);
   return TRUE;
}


/// Function name  : drawShadedRoundRectangle
// Description     : Draws a shaded, rounded rectangle
// 
// HDC             hDC           : [in] Target DC to draw to
// CONST RECT*     pDrawRect     : [in] Drawing rectangle
// CONST COLORREF  clTop         : [in] Colour of top of rectangle
// CONST COLORREF  clMiddle      : [in] Colour of middle of rectangle
// CONST COLORREF  clBottom      : [in] Colour of bottom of rectangle
// CONST UINT      iWidthPadding : [in] Whether to contract rectangle horizontally
// 
ControlsAPI
VOID  drawShadedRoundRectangle(HDC  hDC, CONST RECT*  pDrawRect, CONST COLORREF  clTop, CONST COLORREF  clMiddle, CONST COLORREF  clBottom, CONST COLORREF  clBorder, CONST UINT  iWidthPadding)
{
   HBRUSH  hOldBrush;      //
   RECT    rcTopRect,      // Top shaded rectangle (33%)
           rcBottomRect;   // Bottom shaded rectangle (66%)
   HPEN    hOldPen,        //
           hBorderPen;     // Dark blue pen

   // Prepare
   hBorderPen = CreatePen(PS_SOLID, 1, clBorder);

   // Deflate drawing rectangle by 1 pixels
   SetRect(&rcTopRect,    pDrawRect->left + 1, pDrawRect->top + 1, pDrawRect->right - 1, pDrawRect->bottom - 1);
   SetRect(&rcBottomRect, pDrawRect->left + 1, pDrawRect->top + 1, pDrawRect->right - 1, pDrawRect->bottom - 1);

   // Calculate position of top/bottom rect split
   rcTopRect.bottom -= utilCalculatePercentage(rcTopRect.bottom - rcTopRect.top, 33);
   rcBottomRect.top = rcTopRect.bottom;

   /// [CONTENT] Draw two shaded rectangles
   drawGradientRect(hDC, &rcTopRect, clTop, clMiddle, VERTICAL);
   drawGradientRect(hDC, &rcBottomRect, clMiddle, clBottom, VERTICAL);

   // Select a blue border with no interior
   hOldPen   = SelectPen(hDC, hBorderPen);
   hOldBrush = SelectBrush(hDC, GetStockObject(NULL_BRUSH));

   /// [BORDER] Draw single pixel border with a 2 pixel left/right indent
   RoundRect(hDC, pDrawRect->left + iWidthPadding, pDrawRect->top, pDrawRect->right - iWidthPadding, pDrawRect->bottom, 6, 6);

   // Cleanup
   SelectBrush(hDC, hOldBrush);
   SelectPen(hDC, hOldPen);
   DeletePen(hBorderPen);
}


/// Function name  : drawCustomBackgroundRectangle
// Description     : Draw a white shaded background rectangle for tooltips
// 
// HDC          hDC       : [in] Target DC 
// CONST RECT*  pDrawRect : [in] Drawing rectangle
// 
ControlsAPI
VOID  drawCustomBackgroundRectangle(HDC  hDC, CONST RECT*  pDrawRect)
{
   drawShadedRoundRectangle(hDC, pDrawRect, clWhite, clMediumWhite, clDarkWhite, clMediumGrey, 0);
}


/// Function name  : drawCustomSelectionRectangle
// Description     : Draw a fancy highlight rectangle for menu/combo/listview items
// 
// HDC          hDC        : [in] Target DC 
// CONST RECT*  pDrawRect  : [in] Drawing rectangle
// 
ControlsAPI
VOID  drawCustomHoverRectangle(HDC  hDC, CONST RECT*  pDrawRect)
{
   drawShadedRoundRectangle(hDC, pDrawRect, clWhite, clLightCyan, clMediumCyan, clMediumBlue, 1);
}


/// Function name  : drawCustomSelectionRectangle
// Description     : Draw a fancy highlight rectangle for menu/combo/listview items
// 
// HDC          hDC        : [in] Target DC 
// CONST RECT*  pDrawRect  : [in] Drawing rectangle
// 
ControlsAPI
VOID  drawCustomSelectionRectangle(HDC  hDC, CONST RECT*  pDrawRect)
{
   drawShadedRoundRectangle(hDC, pDrawRect, clWhite, clLightBlue, clMediumBlue, clDarkBlue, 1);
}


/// Function name  : drawShadedBar
// Description     : Draws a shaded rectangle with two edges
// 
// HDC             hDC            : [in] Device contenxt
// CONST RECT*     pRect          : [in] Drawing rectangle
// CONST COLORREF  clTopColour    : [in] Colour at the top
// CONST COLORREF  clMiddleColour : [in] Colour at the middle
// CONST COLORREF  clBottomColour : [in] Colour at the bottom
// CONST COLORREF  clBorderColour : [in] Border colour
// CONST DIRECTION eDirection     : [in] Whether bar is left-right or top-bottom
// 
// 
/// WORK IN PROGRESS: Doesn't work properly yet.
//
ControlsAPI 
VOID  drawShadedBar(HDC  hDC, CONST RECT*  pDrawRect, CONST COLORREF  clTopColour, CONST COLORREF  clMiddleColour, CONST COLORREF  clBottomColour, CONST COLORREF  clBorderColour, CONST DIRECTION  eDirection)
{
   RECT    rcFirstRect,    // Left/Top shaded rectangle (33%)
           rcSecondRect;   // Right/Bottom shaded rectangle (66%)
   HPEN    hOldPen,        //
           hBorderPen;     // Dark blue pen

   // Prepare
   hBorderPen = CreatePen(PS_SOLID, 1, clBorderColour);

   /// Calculate shading rectangles
   switch (eDirection)
   {
   // [HORIZONTAL]
   case HORIZONTAL:
      // Reduce height by 2 pixels
      SetRect(&rcFirstRect,  pDrawRect->left, pDrawRect->top + 1, pDrawRect->right, pDrawRect->bottom - 1);
      rcSecondRect = rcFirstRect;

      // Calculate position of top/bottom rect split
      rcFirstRect.bottom -= utilCalculatePercentage(rcFirstRect.bottom - rcFirstRect.top, 33);
      rcSecondRect.top = rcFirstRect.bottom;
      break;

   // [VERTICAL]
   case VERTICAL:
      // Reduce width by 2 pixels
      SetRect(&rcFirstRect,  pDrawRect->left + 1, pDrawRect->top, pDrawRect->right - 1, pDrawRect->bottom);
      rcSecondRect = rcFirstRect;

      // Calculate position of top/bottom rect split
      rcFirstRect.right -= utilCalculatePercentage(rcFirstRect.right - rcFirstRect.left, 33);
      rcSecondRect.left = rcFirstRect.right;
   }

   /// [CONTENT] Draw first and second shaded rectangles
   drawGradientRect(hDC, &rcFirstRect, clTopColour, clMiddleColour, eDirection == HORIZONTAL ? VERTICAL : HORIZONTAL);
   drawGradientRect(hDC, &rcSecondRect, clMiddleColour, clBottomColour, eDirection == HORIZONTAL ? VERTICAL : HORIZONTAL);

   // Select border pen
   hOldPen = SelectPen(hDC, hBorderPen);

   /// [BORDER] Draw top/bottom or left/right edges
   switch (eDirection)
   {
   // [HORIZONTAL]
   case HORIZONTAL:
      MoveToEx(hDC, pDrawRect->left, pDrawRect->top, NULL);
      LineTo(hDC, pDrawRect->right, pDrawRect->top);

      MoveToEx(hDC, pDrawRect->left, pDrawRect->bottom, NULL);
      LineTo(hDC, pDrawRect->right, pDrawRect->bottom);
      break;

   // [VERTICAL]
   case VERTICAL:
      MoveToEx(hDC, pDrawRect->left, pDrawRect->top, NULL);
      LineTo(hDC, pDrawRect->left, pDrawRect->bottom);

      MoveToEx(hDC, pDrawRect->right, pDrawRect->top, NULL);
      LineTo(hDC, pDrawRect->right, pDrawRect->bottom);
      break;
   }
   
   // Cleanup
   SelectPen(hDC, hOldPen);
   DeletePen(hBorderPen);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                  MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onDialog_DrawBackground
// Description     : Draws the background of a control in response to a WM_CTLCOLOR message
// 
// HDC   hDC  : [in] Drawing DC
// HWND  hWnd : [in] Window or control
// 
// Return Value : Brush used to draw background
// 
ControlsAPI
HBRUSH  onDialog_ControlColour(HDC  hDC)
{
   // Set text colour: default black
   SetTextColor(hDC, getThemeSysColour(TEXT("Window"), COLOR_WINDOWTEXT));

   // Set and return background colour: default white
   SetBkColor(hDC, getThemeSysColour(TEXT("Window"), COLOR_WINDOW));
   return getThemeSysColourBrush(TEXT("Window"), COLOR_WINDOW);
}


/// Function name  : onWindow_DeleteItem
// Description     : Convenience handler for destroying CustomCombBox items
// 
// DELETEITEMSTRUCT*  pDeleteData : [in] WM_DELETEITEM message data
// 
ControlsAPI 
BOOL  onWindow_DeleteItem(DELETEITEMSTRUCT*  pDeleteData)
{
   BOOL  bResult;

   // Prepare
   TRACK_FUNCTION();
   bResult = FALSE;

   // [CHECK] Ensure item is from a ComboBox
   if (pDeleteData->CtlType == ODT_COMBOBOX AND pDeleteData->itemData)
   {
      /// [SUCCESS] Delete combo item data
      deleteCustomComboBoxItem((CUSTOM_COMBO_ITEM*&)pDeleteData->itemData);
      bResult = TRUE;
   }

   // Return result
   END_TRACKING();
   return bResult;
}

/// Function name  : onWindow_DrawItem
// Description     : Convenience handler for any window handling OwnerDraw menus or ComboBoxes
// 
// DRAWITEMSTRUCT*  pDrawData   : [in] WM_DRAWITEM message data
// 
// Return Value   : TRUE if handled, otherwise FALSE
// 
ControlsAPI 
BOOL  onWindow_DrawItem(DRAWITEMSTRUCT*  pDrawData)
{
   BOOL   bResult;      // Operation result

   // Prepare
   TRACK_FUNCTION();
   bResult = TRUE;

   // Examine control type
   switch (pDrawData->CtlType)
   {
   case ODT_MENU:       bResult = onOwnerDrawCustomMenu(pDrawData);      break;
   case ODT_COMBOBOX:   bResult = onOwnerDrawCustomComboBox(pDrawData);  break;
   default:             bResult = FALSE;     break;
   }

   // Return result
   END_TRACKING();
   return bResult;
}


/// Function name  : onWindow_EraseBackground
// Description     : Draws a Windows Vista style background on a dialog
// 
// HWND        hDialog    : [in] Dialog
// HDC         hDC        : [in] Device Context
// CONST UINT  iControlID : [in] Resource ID of any button in the 'gutter' area
// 
// Return Value : TRUE
//
ControlsAPI 
BOOL  onWindow_EraseBackground(HWND  hDialog, HDC  hDC, CONST UINT  iControlID)
{
   RECT  rcButton,
         rcDialog,
         rcGutter;
   UINT  iGutterStart;

   // Prepare
   TRACK_FUNCTION();
   GetClientRect(hDialog, &rcDialog);
   utilGetDlgItemRect(hDialog, iControlID, &rcButton);

   // Calculate height of gutter (Ensure button is centred vertically within it)
   iGutterStart = rcButton.top - (rcDialog.bottom - rcButton.bottom);
   
   // Generate drawing rectangles
   rcGutter     = rcDialog;
   rcGutter.top = rcDialog.bottom = iGutterStart;

   /// [DIALOG] Draw background in white
   utilFillSysColourRect(hDC, &rcDialog, COLOR_WINDOW);

   /// [GUTTER] Draw in grey with an etched border between
   DrawEdge(hDC, &rcGutter, EDGE_ETCHED, BF_ADJUST WITH BF_TOP);
   utilFillSysColourRect(hDC, &rcGutter, COLOR_BTNFACE);

   // Return TRUE
   END_TRACKING();
   return TRUE;
}


/// Function name  : onWindow_MeasureItem
// Description     : Convenience handler for any window handling OwnerDraw menus or ComboBoxes
// 
// HWND                   hMenuParent   : [in] [MENU]  Window that owns the menu
// MEASUREITEMSTRUCT*     pMeasureData  : [in]         WM_MEASUREITEM message data
// 
// Return Value   : TRUE if handled, otherwise FALSE
// 
ControlsAPI 
BOOL  onWindow_MeasureItem(HWND  hMenuParent, MEASUREITEMSTRUCT*  pMeasureData)
{
   BOOL   bResult;      // Operation result

   // Prepare
   TRACK_FUNCTION();

   // Examine control type
   switch (pMeasureData->CtlType)
   {
   case ODT_MENU:       bResult = calculateCustomMenuItemSize(hMenuParent, pMeasureData);  break;
   case ODT_COMBOBOX:   bResult = calculateCustomComboBoxSize(pMeasureData);               break;
   default:             bResult = FALSE;     break;
   }

   // Return result
   END_TRACKING();
   return bResult;
}


/// Function name  : onWindow_PaintNonClient
// Description     : Paints a small caption onto a window's non-client area
// 
// HWND  hWnd          : [in] Window
// HRGN  hUpdateRegion : [in] Update region - Ignored
// 
ControlsAPI
VOID  onWindow_PaintNonClient(HWND  hWnd, HRGN  hUpdateRegion)
{
   RECT  rcWindow,
         rcCaption;
   HDC   hDC;

   // Prepare
   TRACK_FUNCTION();
   GetWindowRect(hWnd, &rcWindow);
   OffsetRect(&rcWindow, -rcWindow.left, -rcWindow.top);

   /// Get window DC and clip update region
   if (hDC = GetDCEx(hWnd, NULL, DCX_CACHE WITH DCX_CLIPCHILDREN WITH DCX_WINDOW)) //|DCX_CACHE|DCX_INTERSECTRGN|DCX_LOCKWINDOWUPDATE);
   {
      DC_STATE* pState = utilCreateDeviceContextState(hDC);
      HFONT     hFont  = utilCreateFont(hDC, TEXT("MS Shell Dlg"), 9, FALSE, FALSE, FALSE);
      TCHAR*    szText = utilGetWindowText(hWnd);
      HTHEME    hTheme = OpenThemeData(hWnd, TEXT("Window"));

      // Prepare
      utilSetDeviceContextFont(pState, hFont, getThemeSysColour(TEXT("Window"), COLOR_WINDOWTEXT));
      utilSetDeviceContextBackgroundMode(pState, TRANSPARENT);

      // [BACKGROUND]
      utilFillSysColourRect(hDC, &rcWindow, COLOR_BTNFACE);

      // [BORDER]
      DrawEdge(hDC, &rcWindow, EDGE_ETCHED, BF_RECT WITH BF_ADJUST);

      // [CAPTION]
      SetRect(&rcCaption, rcWindow.left, rcWindow.top, rcWindow.right - 1, rcWindow.top + GetSystemMetrics(SM_CYSMCAPTION) + GetSystemMetrics(SM_CYEDGE));
      //drawGradientRect(hDC, &rcCaption, RGB(191,205,219), RGB(214,227,241), HORIZONTAL);  // [FIX] DrawCaption() doesn't work on all systems
      //drawGradientRect(hDC, &rcCaption, GetThemeSysColor(hTheme, COLOR_INACTIVECAPTION), GetThemeSysColor(hTheme, COLOR_GRADIENTINACTIVECAPTION), HORIZONTAL);  // [FIX] DrawCaption() doesn't work on all systems
      drawGradientRect(hDC, &rcCaption, getThemeSysColour(TEXT("Window"), COLOR_ACTIVECAPTION), getThemeSysColour(TEXT("Window"), COLOR_GRADIENTACTIVECAPTION), HORIZONTAL);  // [FIX] DrawCaption() doesn't work on all systems
      
      // [TEXT]
      rcCaption.left += GetSystemMetrics(SM_CXFIXEDFRAME);
      DrawText(hDC, szText, lstrlen(szText), &rcCaption, DT_SINGLELINE | DT_VCENTER);
      
      // Cleanup
      utilDeleteDeviceContextState(pState);
      DeleteFont(hFont);
      utilDeleteString(szText);
      CloseThemeData(hTheme);
      ReleaseDC(hWnd, hDC);
   }
   else
      ERROR_CHECK("Retrieving non-client window DC", hDC);

   // Cleanup
   END_TRACKING();
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : dlgprocVistaStyleDialog
// Description     : Basic VistaStyle dialog
// 
ControlsAPI 
INT_PTR  dlgprocVistaStyleDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   BOOL    bResult;     // Operation result
   UINT    iButton;     // ID of default button

   // Prepare
   TRACK_FUNCTION();
   bResult = FALSE;

   // Examine message
   switch (iMessage)
   {
   /// [CUSTOM BACKGROUND]
   case WM_CTLCOLORDLG:
   case WM_CTLCOLORSTATIC:   bResult = (BOOL)onDialog_ControlColour((HDC)wParam);   break;   //bResult = (BOOL)GetStockObject(WHITE_BRUSH);                          break;
   case WM_CTLCOLORBTN:      bResult = (BOOL)GetSysColorBrush(COLOR_BTNFACE);       break;
   case WM_ERASEBKGND: 
      // [CHECK] Search for default button
      if (GetControl(hDialog, iButton = IDOK) OR GetControl(hDialog, iButton = IDCANCEL) OR GetControl(hDialog, iButton = IDYES))
         bResult = onWindow_EraseBackground(hDialog, (HDC)wParam, iButton);
      break;

   /// [CUSTOM MENU/COMBOBOX]
   case WM_MEASUREITEM:      bResult = onWindow_MeasureItem(hDialog, (MEASUREITEMSTRUCT*)lParam);  break;
   case WM_DELETEITEM:       bResult = onWindow_DeleteItem((DELETEITEMSTRUCT*)lParam);             break;
   
   /// [MENU/COMBO/DIALOG]
   case WM_DRAWITEM:
      switch (wParam)
      {
      /// [TITLE/HEADINGS]
      case IDC_DIALOG_TITLE:      bResult = onOwnerDrawStaticTitle(lParam);    break;
      case IDC_DIALOG_HEADING_1:  bResult = onOwnerDrawStaticHeading(lParam);  break;
      case IDC_DIALOG_HEADING_2:  bResult = onOwnerDrawStaticHeading(lParam);  break;
      /// [CUSTOM MENU/COMBOBOX]
      default:                    bResult = onWindow_DrawItem((DRAWITEMSTRUCT*)lParam);      break;
      }
   }

   // Return result
   END_TRACKING();
   return (INT_PTR)bResult;
}

