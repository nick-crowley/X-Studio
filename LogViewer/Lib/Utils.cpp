
#include "stdafx.h"
#include "Utils.hpp"

namespace Utils
{
   

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                    HELPERS
   /// /////////////////////////////////////////////////////////////////////////////////////////

   void  Printf(const TCHAR*  szFormat, ...)
   {
      StringBufferW  szOutput(512);
      va_list        pArguments = va_start(pArguments, szFormat);

      // Format and print
      StringCchVPrintf(szOutput, 512, szFormat, pArguments);
      OutputDebugString(szOutput);
   }


   int  DisplayException(const CHAR*  szLocation, HWND  hParent, std::exception&  ex)
   {
      string szMessage;
      
      szMessage = string("A C++ exception has been caught in ") + szLocation + string(" - ") + ex.what();
      return ::MessageBoxA(hParent, szMessage.c_str(), "An exception has occurred", MB_OK | MB_ICONERROR);
   }


}