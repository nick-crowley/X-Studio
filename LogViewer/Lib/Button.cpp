#include "StdAfx.h"
#include "Button.h"

namespace Windows
{

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                 CONSTRUCTION / DESTRUCTION
   /// /////////////////////////////////////////////////////////////////////////////////////////

   Button::Button() 
   {
   }

   Button::~Button(void)
   {
   }

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                       METHODS
   /// /////////////////////////////////////////////////////////////////////////////////////////

   bool  Button::Create(const TCHAR*  szText, const DWORD  dwStyle, BaseWindow*  pParent, const UINT  iControlID, HINSTANCE  hInstance, Rect  oRect)
   {
      // Create control
      return BaseWindow::Create(szText, dwStyle, NULL, pParent, iControlID, hInstance, &oRect);
   }
} // END: namespace Windows