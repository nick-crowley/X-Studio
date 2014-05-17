#include "StdAfx.h"
#include "DeviceContext.h"

using Windows::WindowsException;

namespace Graphics
{

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                    CONSTANTS / GLOBALS
   /// /////////////////////////////////////////////////////////////////////////////////////////

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                 CONSTRUCTION / DESTRUCTION
   /// /////////////////////////////////////////////////////////////////////////////////////////

   DeviceContext::DeviceContext(void)
   {
      m_hDC  = NULL;
      m_hWnd = NULL;
   }

   DeviceContext::DeviceContext(Windows::BaseWindow*  pWindow) : m_hWnd(pWindow->Handle)
   {
      m_hDC = GetDC(m_hWnd);

      if (!m_hDC)
         throw WindowsException(HERE, TEXT("Unable to retrieve DC"));
   }

   DeviceContext::~DeviceContext(void)
   {
      // Auto-Release
      try { Release(); } 
      catch (WindowsException& e) { e.Print(HERE); }
   }

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                    PUBLIC  METHODS
   /// /////////////////////////////////////////////////////////////////////////////////////////

   bool  DeviceContext::Attach(HWND  hWindow, HDC  hDC)
   {
      if (m_hDC)
         return false;

      // Acquire handle
      m_hWnd = hWindow;
      m_hDC  = hDC;
      return true;
   }


   Font  DeviceContext::CreateFont(wstring  szFont, int  iPoints)
   {
      return Font(szFont, -MulDiv(iPoints, GetDeviceCaps(m_hDC, LOGPIXELSY), 72));
   }


   Font  DeviceContext::CreateFont(wstring  szFont, int  iPoints, bool  bBold, bool  bItalic, bool  bUnderline)
   {
      return Font(szFont, -MulDiv(iPoints, GetDeviceCaps(m_hDC, LOGPIXELSY), 72), bBold, bItalic, bUnderline);
   }
   

   bool  DeviceContext::Release()
   {
      if (!m_hDC)
         return false;

      // Release handle
      if (!ReleaseDC(m_hWnd, m_hDC))
         throw WindowsException(HERE, TEXT("Unable to release DC"));

      // Zero handle
      m_hWnd = NULL;
      m_hDC  = NULL;
      return true;
   }

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                  PROTECTED METHODS
   /// /////////////////////////////////////////////////////////////////////////////////////////

   //GDIObject  DeviceContext::SelectObject(GDIObject*  pObject)
   //{
   //   HGDIOBJ  hPrevObject = ::SelectObject(m_hDC, pObject->m_hObject);

   //   // Ensure object was selected
   //   if (!hPrevObject OR hPrevObject == HGDI_ERROR)
   //      throw WindowsException(HERE, TEXT("Unable to select drawing object"));

   //   // Return previous object
   //   return hPrevObject;
   //}

   /*Brush DeviceContext::SelectBrush(Brush*  pBrush)
   { 
      return (HBRUSH)SelectObject(m_hDC, pBrush->m_hObject); 
   }


   Font  DeviceContext::SelectFont(Font*  pFont)
   { 
      return (HFONT)SelectObject(m_hDC, pFont->m_hObject); 
   }

   
   Pen   DeviceContext::SelectPen(Pen*  pPen)
   { 
      return (HPEN)SelectObject(m_hDC, pPen->m_hObject); 
   }*/

} // END: namespace Graphics

