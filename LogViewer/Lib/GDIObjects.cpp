#include "StdAfx.h"
#include "GDIObjects.h"

using Windows::WindowsException;

namespace Graphics
{
   

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                    CONSTANTS / GLOBALS
   /// /////////////////////////////////////////////////////////////////////////////////////////

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                 CONSTRUCTION / DESTRUCTION
   /// /////////////////////////////////////////////////////////////////////////////////////////

   GDIObject::GDIObject() 
   {
      m_hObject = NULL;
   }

   GDIObject::GDIObject(HGDIOBJ  hObject)
   {
      Attach(hObject);
   }

   GDIObject::~GDIObject()
   {
      // Auto-Release
      try { Destroy(); } 
      catch (WindowsException& e) { e.Print(HERE); }
   }

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                    PUBLIC  METHODS
   /// /////////////////////////////////////////////////////////////////////////////////////////

   bool  GDIObject::Attach(HGDIOBJ  hObject)
   {
      if (m_hObject OR !hObject)
         return false;

      m_hObject = hObject;
      return true;
   }

   void  GDIObject::Detach()
   {
      m_hObject = NULL;
   }

   bool  GDIObject::Destroy()
   {
      if (!m_hObject)
         return false;

      // Destroy object
      if (!DeleteObject(m_hObject))
         throw WindowsException(HERE, TEXT("Unable to delete GDI object"));

      // Reset handle
      m_hObject = NULL;
      return true;
   }

   void  GDIObject::Transfer(GDIObject&  rThat)
   {
      // Replace handle
      Destroy();
      Attach(rThat.m_hObject);

      // Remove from old
      rThat.Detach();
   }

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                       BRUSH 
   /// /////////////////////////////////////////////////////////////////////////////////////////

   Brush::Brush(HBRUSH  hBrush) : GDIObject(hBrush) 
   {
   }

   Brush::Brush(COLORREF  clColour)
   {
      if (!Attach( CreateSolidBrush(clColour) ))
         throw WindowsException(HERE);
   }

   Brush::Brush(int  iSysColour)
   {
      if (!Attach( GetSysColorBrush(iSysColour) ))
         throw WindowsException(HERE);
   }

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                       PEN 
   /// /////////////////////////////////////////////////////////////////////////////////////////

   Pen::Pen(HPEN  hPen) : GDIObject(hPen) 
   {
   }

   Pen::Pen(COLORREF  clColour)
   {
      if (!Attach(CreatePen(PS_SOLID, 1, clColour)))
         throw WindowsException(HERE);
   }

   Pen::Pen(int  iWidth, COLORREF  clColour)
   {
      if (!Attach(CreatePen(PS_SOLID, iWidth, clColour)))
         throw WindowsException(HERE);
   }

   Pen::Pen(int  iStyle, int  iWidth, COLORREF  clColour)
   {
      if (!Attach(CreatePen(iStyle, iWidth, clColour)))
         throw WindowsException(HERE);
   }

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                       FONT
   /// /////////////////////////////////////////////////////////////////////////////////////////

   Font::Font(HFONT  hFont) : GDIObject(hFont) 
   {
   }

   Font::Font(wstring  szFont, int  iHeight)
   {
      if (!Attach( Create(szFont.c_str(), iHeight, FW_NORMAL, FALSE, FALSE) ))
         throw WindowsException(HERE);
   }

   Font::Font(wstring  szFont, int  iHeight, bool  bBold, bool  bItalic, bool  bUnderline)
   {
      if (!Attach( Create(szFont.c_str(), iHeight, bBold ? FW_NORMAL : FW_BOLD, bItalic ? TRUE : FALSE, bUnderline ? TRUE : FALSE) ))
         throw WindowsException(HERE);
   }

   HFONT  Font::Create(const TCHAR*  szFont, int  iHeight, int  iWeight, BOOL  bItalic, BOOL  bUnderline)
   {
      return ::CreateFont(iHeight, NULL, NULL, NULL, iWeight, bItalic, bUnderline, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, szFont);
   }

} // END: namespace Graphics

