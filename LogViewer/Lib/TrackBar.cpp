#include "StdAfx.h"
#include "TrackBar.h"

namespace Windows
{

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                 CONSTRUCTION / DESTRUCTION
   /// /////////////////////////////////////////////////////////////////////////////////////////

   TrackBar::TrackBar() 
   {
   }

   TrackBar::~TrackBar(void)
   {
   }

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                       METHODS
   /// /////////////////////////////////////////////////////////////////////////////////////////

   bool  TrackBar::Create(const DWORD  dwStyle, BaseWindow*  pParent, const UINT  iControlID, HINSTANCE  hInstance, Rect  oRect)
   {
      // Create control
      return BaseWindow::Create(NULL, dwStyle, NULL, pParent, iControlID, hInstance, &oRect);
   }

} // END: namespace Windows