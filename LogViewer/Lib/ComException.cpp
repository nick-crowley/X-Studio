#include "StdAfx.h"
#include "ComException.h"

namespace Utils
{
   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                   COM EXCEPTION
   /// /////////////////////////////////////////////////////////////////////////////////////////

   ComException::ComException(const TCHAR*  szLocation, _com_error&  oError) : Utils::BaseException(szLocation, (long)oError.Error())
   {
   }

   ComException::ComException(const TCHAR*  szLocation, HRESULT  hResult) : Utils::BaseException(szLocation, (long)hResult)
   {
   }

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                     METHODS
   /// /////////////////////////////////////////////////////////////////////////////////////////

   wstring  ComException::getError()
   {
      _com_error  oError((HRESULT)getCode());

      // Use placeholder if message is NULL
      return oError.ErrorMessage() ? oError.ErrorMessage() : TEXT("Unknown COM Error");
   }

} // END: namespace Exceptions