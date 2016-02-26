#pragma once
#include "basewindow.h"

namespace Windows
{

   class Button : public Windows::BaseWindow
   {
   public:
      Button();
      ~Button(void);

      bool  Create(const TCHAR*  szText, const DWORD  dwStyle, BaseWindow*  pParent, const UINT  iControlID, HINSTANCE  hInstance, Rect  oRect);

   protected:
      virtual const TCHAR*  getClassName() const  { return WC_BUTTON; };
   };

} // END: namespace Windows