#pragma once
#include <comdef.h>
#include "BaseException.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          CLASSES
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Utils
{

   // COM Exception - Generates error from _com_errors
   //
   class ComException : public Utils::BaseException
   {
   public:
      ComException(const TCHAR*  szLocation, HRESULT  hResult);
      ComException(const TCHAR*  szLocation, _com_error&  oError);
      virtual ~ComException() {};

      HRESULT           getResult() { return (HRESULT)getCode();          };

   protected:
      virtual wstring   getType()   { return TEXT("Utils::ComException"); };
      virtual wstring   getError();
      
   };

} // END: namespace Exceptions
