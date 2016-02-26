#pragma once
#include "BaseWindow.h"


namespace Windows
{
   class Edit : public BaseWindow
   {
   public:
      Edit();
      virtual ~Edit(void);

      bool  Create(const TCHAR*  szText, DWORD  dwStyle, BaseWindow*  pParent, const UINT  iControlID, HINSTANCE  hInstance, Rect  oRect);

   protected:
      
      virtual const TCHAR*  getClassName() const { return WC_EDIT; };

   };

} // END: namespace Window