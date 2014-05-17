#include "StdAfx.h"
#include "Edit.h"

namespace Windows
{

   Edit::Edit() 
   {
   }

   Edit::~Edit(void)
   {
   }

   
   bool  Edit::Create(const TCHAR*  szText, DWORD  dwStyle, BaseWindow*  pParent, const UINT  iControlID, HINSTANCE  hInstance, Rect  oRect)
   {
      return BaseWindow::Create(szText, dwStyle, NULL, pParent, iControlID, hInstance, &oRect);
   }

   

} // END: namespace Window