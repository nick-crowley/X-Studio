#pragma once

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          MACROS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Macro: WIDEN(x)  
// Description: Creates a wide char equivilent of an input string (from MSDN)
// 
#define      WIDEN2(x)                L ## x
#define      WIDEN(x)                 WIDEN2(x)

/// Macro: __WFILE__
// Description: Wide character equivilent of __FILE__
//
#define      __WFILE__                WIDEN(__FILE__)

/// Macro: __WFUNCTION__
// Description: Wide character equivilent of __FUNCTION__
//
#define      __WFUNCTION__            WIDEN(__FUNCTION__)

/// Macro: __SWFILE__
// Description: Extracts only the filename from the '__FILE__' macro.         NOTE: /FC switch should achieve this, but doesn't.
//
#define      __SWFILE__               (_tcsrchr(__WFILE__, '\\') + 1)

/// Macro: convLineNumberA/W
// Description: Converts __LINE__ macro into an ANSI/wide string
//
#define      convLineNumberHelperA(x)  #x
#define      convLineNumberHelperW(x)  TEXT(#x)
#define      convLineNumberA(x)        convLineNumberHelperA(x)
#define      convLineNumberW(x)        convLineNumberHelperW(x)

/// Macro: HERE_A/W
// Description: Generates an ANSI/wide 'code location string', for use by exceptions
//
#define      HERE_A                   __FUNCTION__ "() on line " convLineNumberA(__LINE__)
#define      HERE_W                   __WFUNCTION__ TEXT("() on line ") convLineNumberW(__LINE__)

// Convenience macro:
#ifdef UNICODE
#define HERE  HERE_W
#else
#define HERE  HERE_A
#endif 

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          CLASSES
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Utils
{

   // Exception abstract base class
   class BaseException
   {
   protected:
      BaseException(const TCHAR*  szLocation, const long  iCode);

   public:
      virtual ~BaseException() {};

      void     Display(const TCHAR*  szLocation, HWND  hParent);
      void     Print(const TCHAR*  szLocation);
      void     Trace(const TCHAR*  szLocation);

      long     getCode()   { return m_iCode; };

   protected:
      virtual wstring  getError()=0;
      virtual wstring  getType()=0;

   private:
      wstring    getDebugMessage();
      wstring    getDisplayMessage();
      wstring    getDisplayTitle();
      wstring    getTrace();
      
      typedef  list<wstring>  TraceList;

      TraceList  m_oTrace;
      long       m_iCode;
   };


} // END: namespace Utils
