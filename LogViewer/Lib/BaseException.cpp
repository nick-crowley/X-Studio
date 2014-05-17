#include "StdAfx.h"
#include "BaseException.h"
#include "Dbghelp.h"

namespace Utils
{

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                    EXCEPTION
   /// /////////////////////////////////////////////////////////////////////////////////////////

   BaseException::BaseException(const TCHAR*  szLocation, const long  iCode)
   {
      Trace(szLocation);
      m_iCode = iCode;
   }


   void  BaseException::Display(const TCHAR*  szLocation, HWND  hParent)
   {
      // Update location
      if (m_oTrace.front() != szLocation)
         Trace(szLocation);

      // Print compressed version to console + Display
      OutputDebugString(getDebugMessage().c_str());
      ::MessageBox(hParent, getDisplayMessage().c_str(), getDisplayTitle().c_str(), MB_OK WITH MB_ICONERROR);
   }


   void  BaseException::Print(const TCHAR*  szLocation)
   {
      // Update location
      if (m_oTrace.front() != szLocation)
         Trace(szLocation);

      // Print to output
      OutputDebugString(getDebugMessage().c_str());
   }


   void  BaseException::Trace(const TCHAR*  szLocation)
   {
      // Add new location to front.  eg. 'Window::Create() on line 45'
      m_oTrace.push_front(szLocation);
   }

   // /////////////////////////////////////////////////////////////////////////////////////////
   //                                PROTECTED METHODS
   // /////////////////////////////////////////////////////////////////////////////////////////

   wstring  BaseException::getDebugMessage()
   {
      // eg. ComException caught in Windows::onCreate() on line 12 - IDispatch Error #1202 - Unable to create interface
      return STRING(" ERROR: ") + getError() + TEXT(" ---> ") + getType() + TEXT(" caught in ") + m_oTrace.front() + TEXT("\n");
   }

   wstring  BaseException::getDisplayMessage()
   {
      wstring  szOutput;
      
      // eg. Dispatch Error #1202 - Unable to create interface \n\n Trace:
      szOutput = getError() + TEXT("\n\n");
      //szOutput += TEXT("\nTrace:");

      // eg. Windows::onCreate() on line 12
      // eg. DirectShow::Create() on line 415
      /*for (TraceList::const_iterator  pLocation = m_oTrace.begin(); pLocation != m_oTrace.end(); ++pLocation)
         szOutput += TEXT("\n") + *pLocation;*/

      // Auto-Generate trace using DbgHelp.dll
      szOutput += getTrace();


      // Return text block
      return szOutput;
   }

   wstring  BaseException::getDisplayTitle()
   {
      return getType() + TEXT(" exception has occurred");
   }

   typedef  SYMBOL_INFO_PACKAGEW  SymbolInfo;

   wstring  BaseException::getTrace()
   {
      SymbolInfo  oSymbol;
      void*       pFrames[63];
      int         iFrameCount;
      HANDLE      hProcess;
      wstring     szOutput;

      // Capture addresses on the Stack
      SymInitialize(hProcess = GetCurrentProcess(), NULL, TRUE );
      iFrameCount = CaptureStackBackTrace(0, 63, pFrames, NULL);

      // Create empty symbol
      oSymbol.si.MaxNameLen   = MAX_SYM_NAME;
      oSymbol.si.SizeOfStruct = sizeof(SYMBOL_INFOW);

      // Extract symbol names
      for(int i = 0; i < iFrameCount; i++ )
      {
         SymFromAddrW(hProcess, (DWORD64)(pFrames[i]), 0, &oSymbol.si);
         wstring  szSymbol(oSymbol.si.Name);

         // [BASE EXCEPTION] Exclude from trace
         if (szSymbol.find(TEXT("BaseException")) != wstring::npos)
            continue;

         // [APP FUNCTION] Include in trace
         if ((oSymbol.si.Flags & SYMFLAG_EXPORT) == FALSE)
            szOutput += szSymbol + TEXT("\n");
         
         // [MAIN] Abort
         if (szSymbol == TEXT("wWinMain"))
            break;
      }

      return szOutput;
   }
   

} // END: namespace Utils