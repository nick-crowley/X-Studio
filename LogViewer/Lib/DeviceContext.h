#pragma once
#include "GDIObjects.h"
#include "Basewindow.h"

// Macros
//#undef SelectFont
//#undef SelectBrush
//#undef SelectPen

using Windows::Rect;

namespace Graphics
{

   class DeviceContext
   {
   public:
      DeviceContext(void);
      DeviceContext(Windows::BaseWindow*  pWindow);
      virtual ~DeviceContext(void);

      // Handles
      bool  Attach(HWND  hWindow, HDC  hDC);
      bool  Release();

      // Objects
      Brush SelectObject(Brush*  pBrush)  { return (HBRUSH)::SelectObject(m_hDC, pBrush->m_hObject);  };
      Font  SelectObject(Font*  pFont)    { return (HFONT)::SelectObject(m_hDC, pFont->m_hObject);    };
      Pen   SelectObject(Pen*  pPen)      { return (HPEN)::SelectObject(m_hDC, pPen->m_hObject);      };

      // Drawing
      bool  FillRect(Rect  oRect, Brush  oBrush)               { return ::FillRect(m_hDC, &oRect, oBrush) ? true : false; };
      bool  Rectangle(Rect  oRect)                             { return ::Rectangle(m_hDC, oRect.left, oRect.top, oRect.right, oRect.bottom) ? true : false; };
      bool  RoundRect(Rect  oRect, int iWidth, int  iHeight)   { return ::RoundRect(m_hDC, oRect.left, oRect.top, oRect.right, oRect.bottom, iWidth, iHeight) ? true : false; };

      // Helpers
      Font  CreateFont(wstring  szFont, int  iPoints);
      Font  CreateFont(wstring  szFont, int  iPoints, bool  bBold, bool  bItalic, bool  bUnderline);

   protected:

      HDC   m_hDC;
      HWND  m_hWnd;
   };

} // END: namespace Graphics