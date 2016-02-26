//
// Test Window.cpp : Test window
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

struct ALPHA_BITMAP
{
   BITMAPV5HEADER*   pHeader;
   RGBQUAD*          pPixelData;
};


BOOL   loadAlphaBitmap(CONST TCHAR*  szResourceID, ALPHA_BITMAP*  pOutput)
{
   HRSRC             hResource;         // Handle to the resource containing the file
   HGLOBAL           hResourceData;     // Handle to the global memory where the resource was loaded
   
   // Prepare
   utilZeroObject(pOutput, ALPHA_BITMAP);

   /// Find the specified resource
   if (hResource = FindResource(getResourceInstance(), szResourceID, RT_BITMAP))
   {
      // Load resource into global memory, retrieve an accessible pointer
      hResourceData = LoadResource(getResourceInstance(), hResource);
      pOutput->pHeader = (BITMAPV5HEADER*)LockResource(hResourceData);

      // Extract pixel data from header
      if (pOutput->pHeader)
      {
         pOutput->pPixelData = (RGBQUAD*)(pOutput->pHeader + pOutput->pHeader->bV5Size);

         /// Calculate per-pixel alpha
         for (UINT x = 0; x < 512; x++)
            for (UINT y = 0; y < 512; y++)
            {
               pOutput->pPixelData[x,y].rgbRed   *= pOutput->pPixelData[x,y].rgbReserved;
               pOutput->pPixelData[x,y].rgbGreen *= pOutput->pPixelData[x,y].rgbReserved;
               pOutput->pPixelData[x,y].rgbBlue  *= pOutput->pPixelData[x,y].rgbReserved;
            }
      }

      // Cleanup
      FreeResource(hResourceData);
   }

   // Return TRUE if pixel data was found
   return pOutput->pPixelData != NULL;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////

UINT  iProgress = 100;


VOID   onTestWindowPaint(HWND  hWnd)
{
   BLENDFUNCTION  oBlendData;
   HBITMAP        hLogo;
   HDC            hMemoryDC;
   //RECT     rcClient;
   //HBRUSH   hRedBrush;

   // Prepare
   //hDC = BeginPaint(hWnd, &oPaintData);

   

   //BITMAPINFO  oBitmapInfo;
   //RGBQUAD  *pLogoPixels,
   //         *pBitmapPixels;

   // create our DIB section and select the bitmap into the dc
   //hLogo = CreateDIBSection(hMemoryDC, &oBitmapInfo, DIB_RGB_COLORS, pBitmapPixels, NULL, 0x0);
   //SelectBitmap(hMemoryDC, hLogo);

   //BITMAPINFO oBitmap;
   //RGBQUAD*    pBitmapBits;

   
   //hLogo = (HBITMAP)LoadImage(NULL, TEXT("D:\\My Projects\\BearScript\\BearScriptRes\\Bitmaps\\X-Studio Logo alpha.bmp"), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION WITH LR_LOADFROMFILE);

   //ALPHA_BITMAP  oBitmap;

   // Prepare empty memory DC
   hMemoryDC = CreateCompatibleDC(GetDC(NULL));

   hLogo = (HBITMAP)LoadImage(getResourceInstance(), TEXT("LOGO_BITMAP"), IMAGE_BITMAP, 410, 380, LR_CREATEDIBSECTION);
   ERROR_CHECK("loading alpha bitmap", hLogo);
   //hLogo = CreateCompatibleBitmap(GetDC(NULL), 512, 512);
   SelectBitmap(hMemoryDC, hLogo);

   // Load Bitmap
   //loadAlphaBitmap(TEXT("LOGO_BITMAP"), &oBitmap);

   HPEN  hPen = CreatePen(PS_SOLID, 2, RGB(243, 255, 246));
   SelectPen(hMemoryDC, hPen);

   MoveToEx(hMemoryDC, 100, 345, NULL);
   LineTo(hMemoryDC, min(iProgress, 300), 345);

   iProgress += 10;

   SelectPen(hMemoryDC, NULL);
   DeletePen(hPen);

   //// Copy bitmap into memory DC
   //SetDIBitsToDevice(hMemoryDC, 0, 0, 512, 512, 0, 0, 0, 512, (VOID*)oBitmap.pPixelData, (BITMAPINFO*)oBitmap.pHeader, DIB_RGB_COLORS); 
   
   //// Prepare
   //pBitmapBits = utilCreateObjectArray(RGBQUAD, 512 * 512);
   //oBitmap.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
   //oBitmap.bmiHeader.biWidth = 512;
   //oBitmap.bmiHeader.biHeight = 512;
   //oBitmap.bmiHeader.biPlanes = 1;
   //oBitmap.bmiHeader.biBitCount = 32;         // four 8-bit components 
   //oBitmap.bmiHeader.biCompression = BI_RGB;
   //oBitmap.bmiHeader.biSizeImage = 512 * 512 * 4;

   /// Extract bitmap pixels
   //if (GetDIBits(hMemoryDC, hLogo, 0, 512, (VOID*)pBitmapBits, &oBitmap, DIB_RGB_COLORS))
   //{
   //   VERBOSE("Checking for per-pixel Alpha...");

   //   for (UINT x = 0; x < 512; x++)
   //   {
   //      for (UINT y = 0; y < 512; y++)
   //      {
   //         //if (pBitmapBits[x,y].rgbReserved) // INCLUDES 0xff000000)
   //         //{
   //         //   VERBOSE("[FOUND] Alpha channel detected at %u,%u", x, y);
   //         //   break;
   //         //}

   //         pBitmapBits[x,y].rgbReserved = (BYTE)x;
   //      }
   //   }
   //}

   // Cleanup
   //utilDeleteObject(pBitmapBits);

   /// Draw red background
   /*GetClientRect(hWnd, &rcClient);
   hRedBrush = CreateSolidBrush(RGB(255,0,0));
   FillRect(hDC, &rcClient, hRedBrush);
   DeleteBrush(hRedBrush);*/

   // Draw bitmap
   //BitBlt(hDC, 0, 0, 512, 512, hMemoryDC, 0, 0, SRCCOPY);

   /// Prepare blending
   utilZeroObject(&oBlendData, BLENDFUNCTION);
   oBlendData.BlendOp = AC_SRC_OVER;
   oBlendData.AlphaFormat = AC_SRC_ALPHA;
   oBlendData.SourceConstantAlpha = 255;

   /// Blend bitmap
   //ERROR_CHECK("alpha blending", AlphaBlend(hDC, 0, 0, 512, 512, hMemoryDC, 0, 0, 512, 512, oBlendData));
   //BitBlt(hDC, 0, 0, 512, 512, hMemoryDC, 0, 0, SRCCOPY);

   SIZE   siWindow;
   POINT  ptOutput;

   siWindow.cx = 410;
   siWindow.cy = 380;
   ptOutput.x  = 
      ptOutput.y = 0;

   ERROR_CHECK("updating layered window", UpdateLayeredWindow(hWnd, NULL, NULL, &siWindow, hMemoryDC, &ptOutput, 0, &oBlendData, ULW_ALPHA));

   // Cleanup
   SelectBitmap(hMemoryDC, NULL);
   DeleteBitmap(hLogo);
   DeleteDC(hMemoryDC);
   //EndPaint(hWnd, &oPaintData);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////

LRESULT  wndprocTestWindow(HWND  hWnd, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   //RECT  rcMainWindow,
   //      rcWindow;

   switch (iMessage)
   {
   case WM_CREATE:
      //GetWindowRect(getAppWindow(), &rcMainWindow;
      //GetWindowRect(hWnd, &rcWindow);
      onTestWindowPaint(hWnd);

      SetTimer(hWnd, 42, 200, NULL);
      break;

   case WM_DESTROY:
      KillTimer(hWnd, 42);
      break;

   case WM_TIMER:
      onTestWindowPaint(hWnd);
      break;

   /*case WM_PAINT:
      onTestWindowPaint(hWnd);
      break;*/

   default:
      return DefWindowProc(hWnd, iMessage, wParam, lParam);
   }

   return 0;
}

