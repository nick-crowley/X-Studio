#pragma once

#include "Utils.hpp"

namespace Windows
{

   template <typename T>
   class NotifyMessage : public Utils::EmptyStruct<T>
   {
   public:
      NotifyMessage(const UINT  iNotification, HWND  hFrom, const UINT  iFrom) : EmptyStruct()
      {
         // Fill header
         hdr.code     = iNotification;
         hdr.hwndFrom = hFrom;
         hdr.idFrom   = iFrom;
      };

      bool  Send(HWND  hWnd)
      {
         return ::SendMessage(hWnd, WM_NOTIFY, hdr.idFrom, (LPARAM)dynamic_cast<T*>(this)) ? true : false;
      };
   };

   

} // END: namespace Windows