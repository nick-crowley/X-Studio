//
// Comment Ratio Control.cpp : A coloured progress bar
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                  CONSTANTS / GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////

CONST UINT  iChunkSize = 10;

CONST COLORREF  clGreen  = RGB(34, 177,76),
                clYellow = RGB(221,210,0),
                clOrange = RGB(255,127,39),
                clRed    = RGB(237,28, 36);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : identifyCommentRatioColour
// Description     : Calculate an RGB colour from a percentage  (Green for high percentages, Red for low)
// 
// CONST UINT  iPercentage : [in] Percentage
// 
// Return Value   : RGB colour
// 
COLORREF  identifyCommentRatioColour(CONST UINT  iPercentage)
{
   // [75+] Return GREEN 
   if (iPercentage >= 75)
      return clGreen;

   // [50+] Return YELLOW
   else if (iPercentage >= 50)
      return clYellow;

   else 
      // [25 -> 0] Return ORANGE / RED
      return (iPercentage >= 25 ? clOrange : clRed);
}

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                        FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : drawGradientBar
// Description     : Draws a coloured bar with alpha blended top and bottom edges. For simplicity this will only
//                    draw from the top-left hand corner so it's best used for memory DCs or DCs with modified origins.
// 
// HDC             hDC      : [in] Destination device context
// CONST SIZE*     pSize    : [in] Size of the bar
// CONST COLORREF  clColour : [in] Colour of the centre of the gradient bar
// 
// Return Value   : TRUE
// 
BOOL  drawGradientBar(HDC  hDC, CONST SIZE*  pSize, CONST COLORREF  clColour)
{
   TRIVERTEX        oCorners[2];       // Corners of the gradient rectangles
   GRADIENT_RECT    oGradientRect;     // Defines a gradient rectangle
   HBRUSH           hSolidBrush;       // Brush used for the solid colour portion of the gradient bar
   RECT             rcDrawRect;        // Drawing rectangle

   // Prepare
   hSolidBrush = CreateSolidBrush(clColour);
   SetRect(&rcDrawRect, 0, pSize->cy / 3, pSize->cx, 2 * pSize->cy / 3);

   // Define white corner
   oCorners[0].x      = 0;
   oCorners[0].y      = 0;
   oCorners[0].Red    = 255 << 8;
   oCorners[0].Green  = 255 << 8;
   oCorners[0].Blue   = 255 << 8;
   oCorners[0].Alpha  = 255 << 8;
   oGradientRect.UpperLeft  = 0;

   // Define colour corner
   oCorners[1].x      = pSize->cx;
   oCorners[1].y      = pSize->cy / 3;
   oCorners[1].Red    = GetRValue(clColour) << 8;
   oCorners[1].Green  = GetGValue(clColour) << 8;
   oCorners[1].Blue   = GetBValue(clColour) << 8;
   oCorners[1].Alpha  = 0;
   oGradientRect.LowerRight = 1;

   // Draw top third (WHITE -> COLOUR)
   GradientFill(hDC, oCorners, 2, &oGradientRect, 1, GRADIENT_FILL_RECT_V);

   // Draw middle third (COLOUR)
   FillRect(hDC, &rcDrawRect, hSolidBrush);
   
   // Draw bottom third (COLOUR -> WHITE)
   oCorners[1].y = pSize->cy * 2 / 3;
   oCorners[0].y = pSize->cy;
   
   GradientFill(hDC, oCorners, 2, &oGradientRect, 1, GRADIENT_FILL_RECT_V);

   // Cleanup
   DeleteObject(hSolidBrush);
   return TRUE;
}

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                      MESSAGE HANDLERS
/// ////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onCommentRatioCtrlPaint
// Description     : Paints the comment ratio control
// 
// HWND          hCtrl        : [in] 
// PAINTSTRUCT*  pPaintData   : [in] 
// UINT          iPercentage  : [in] 
// 
// Return Value   : 
// 
BOOL  onCommentRatioCtrlPaint(HWND  hCtrl, PAINTSTRUCT*  pPaintData, UINT  iPercentage)
{
   RECT      rcClientRect,       // Client rectangle
             rcContentRect,      // Progress bar Content rectangle
             rcNonBarContentRect;// Content rectangle not covered by the progress bar
   SIZE      siClientSize,       // Client rectangle size
             siContentSize;      // Progress bar content rectangle size
   HDC       hMemoryDC;          // MemoryDC containing a coloured gradient bar
   HBITMAP   hMemoryBitmap,      // MemoryDC bitmap
             hMaskBitmap,        // Monochrome mask containing the bars to create a progress bar effect
             hOldBitmap;
   HTHEME    hTheme;             // System progress bar theme handle
   HRGN      hClipRegion;        // Client rectangle clipping region
   INT       iBarWidth;          // Width of bar, in pixels
   
   // [CHECK]
   ASSERT(iPercentage <= 100);

   // Get client and content rectangles
   GetClientRect(hCtrl, &rcClientRect);
   rcContentRect = rcClientRect;
   InflateRect(&rcContentRect, -3, -2);
   // Create convenience sizes
   utilConvertRectangleToSize(&rcClientRect, &siClientSize);
   utilConvertRectangleToSize(&rcContentRect, &siContentSize);
   // Calculate bar length and the gap on the right beyond the bar
   if (iPercentage == 100)
      iBarWidth = siContentSize.cx;
   else
   {
      iBarWidth = utilCalculatePercentage(siContentSize.cx, iPercentage);
      iBarWidth -= (iBarWidth % iChunkSize);
   }
   rcNonBarContentRect = rcContentRect;
   rcNonBarContentRect.left += iBarWidth;

   // Create MemoryDC
   hMemoryDC     = CreateCompatibleDC(pPaintData->hdc);
   hMemoryBitmap = CreateCompatibleBitmap(pPaintData->hdc, siContentSize.cx, siContentSize.cy);
   hOldBitmap    = SelectBitmap(hMemoryDC, hMemoryBitmap);

   // Clip the content rectangle to avoid flickering
   ExcludeClipRect(pPaintData->hdc, rcContentRect.left, rcContentRect.top, rcContentRect.right, rcContentRect.bottom);

   // [CHECK] Are themes active?
   if (hTheme = OpenThemeData(hCtrl, WC_EDIT))
      /// [THEME BORDER] Draw the border of an edit control
      DrawThemeBackground(hTheme, pPaintData->hdc, EP_BACKGROUNDWITHBORDER, EBWBS_NORMAL, &rcClientRect, NULL);
   else
      /// [NON-THEME BORDER] Draw simple box
      DrawEdge(pPaintData->hdc, &rcClientRect, EDGE_ETCHED, BF_RECT);

   // Revert clipping region back to the client area
   hClipRegion = CreateRectRgn(rcClientRect.left, rcClientRect.top, rcClientRect.right, rcClientRect.bottom);
   SelectClipRgn(pPaintData->hdc, hClipRegion);
   
   /// Create the progress bar effect by generating a gradient bar and masking it through a custom bitmap
   drawGradientBar(hMemoryDC, &siContentSize, identifyCommentRatioColour(iPercentage));
   hMaskBitmap = LoadBitmap(getResourceInstance(), TEXT("PROGRESS_BAR_MASK"));
   MaskBlt(pPaintData->hdc, rcContentRect.left, rcContentRect.top, iBarWidth, siContentSize.cy, hMemoryDC, 0, 0, hMaskBitmap, 0, 0, MAKEROP4(SRCCOPY, WHITENESS));

   // Erase Non-bar area
   utilFillSysColourRect(pPaintData->hdc, &rcNonBarContentRect, COLOR_WINDOW);

   // Cleanup
   SelectBitmap(hMemoryDC, hOldBitmap);
   DeleteBitmap(hMemoryBitmap);
   DeleteBitmap(hMaskBitmap);
   DeleteObject(hClipRegion);
   DeleteDC(hMemoryDC);
   CloseThemeData(hTheme);
   return TRUE;
}

/// Function name  : wndprocCommentRatioCtrl
// Description     : Comment ratio control window procedure
// 
LRESULT CALLBACK  wndprocCommentRatioCtrl(HWND  hCtrl, UINT iMessage, WPARAM  wParam, LPARAM  lParam)
{
   PAINTSTRUCT  oPaintData;
   static UINT  iPercentage = 0;
   
   switch (iMessage)
   {
   case PBM_SETPOS:
      iPercentage = wParam;
      break;

   case WM_PAINT:
      BeginPaint(hCtrl, &oPaintData);
      onCommentRatioCtrlPaint(hCtrl, &oPaintData, iPercentage % 101);
      EndPaint(hCtrl, &oPaintData);
      break;

   
   case WM_ERASEBKGND:
      return NULL;

   default:
      return DefWindowProc(hCtrl, iMessage, wParam, lParam);
   }

   return 0;
}

