//
// Ssytem.cpp : System/Process related utilities
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        MACROS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                  CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : utilCreateExceptionString
// Description     : Creates a string used for passing data with an exception
// 
// CONST TCHAR*  szText : [in] Exception text
// 
// Return Value   : New exception string, you are responsible for destroying it
// 
UtilityAPI
EXCEPTION_STRING*  utilCreateExceptionString(CONST TCHAR*  szAssertion, CONST TCHAR*  szFunction, CONST UINT  iLine, CONST TCHAR*  szFileName)
{
   EXCEPTION_STRING*  pExceptionString;

   // Prepare
   pExceptionString = utilCreateEmptyObject(EXCEPTION_STRING);

   // Set properties
   pExceptionString->szAssertion = szAssertion;
   pExceptionString->szFunction  = szFunction;
   pExceptionString->szFileName  = szFileName;
   pExceptionString->iLine       = iLine;

   // Return new object
   return pExceptionString;
}


/// Function name  : utilDeleteExceptionString
// Description     : Deletes a string used for passing data with an exception
// 
// CONST TCHAR*  szText : [in] Exception text
// 
UtilityAPI
VOID  utilDeleteExceptionString(EXCEPTION_STRING*  &pExceptionString)
{
   // Do not destroy string literal

   // Delete calling object
   utilDeleteObject(pExceptionString);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : utilCloseHandle
// Description     : Close and zero a windows API handle
// 
// HANDLE&  pHandle : [in] Reference to a system handle
// 
UtilityAPI
VOID    utilCloseHandle(HANDLE&  pHandle)
{
   CloseHandle(pHandle);
   pHandle = NULL;
}


/// Function name  : utilInitProcessImageList
// Description     : Initialises the per-process sytem imagelist
// 
// Return Value   : TRUE/FALSE
// 
UtilityAPI
BOOL   utilInitProcessImageList()
{
   FILE_ICON_INIT  pfnFileIconInit;    // FileIconInit function pointer
   BOOL            bResult;            // Operation result

   // Prepare
   bResult = FALSE;

   // Manually extract function from ordinal 660 of Shell32.dll
   if (pfnFileIconInit = (FILE_ICON_INIT)GetProcAddress(GetModuleHandle(TEXT("Shell32.dll")), (CONST CHAR*)660))
      /// [SUCCESS] Initialise ImageList
      bResult = (*pfnFileIconInit)(TRUE);

   // Return result
   return bResult;
}


/// Function name  : utilGetCachedIconIndex
// Description     : Adds an icon to the system ImageList
// 
// Return Value   : TRUE/FALSE
// 
UtilityAPI
BOOL   utilGetCachedIconIndex(CONST TCHAR*  szPath, INT  iIconIndex, UINT  iFlags)
{
   GET_CACHED_IMAGE_INDEX  pfnGetCachedIconIndex;    // FileIconInit function pointer
   BOOL                    bResult;                   // Operation result

   // Prepare
   bResult = FALSE;

   // Manually extract function from ordinal 72 of Shell32.dll
   if (pfnGetCachedIconIndex = (GET_CACHED_IMAGE_INDEX)GetProcAddress(GetModuleHandle(TEXT("Shell32.dll")), (CONST CHAR*)72))
      /// [SUCCESS] Extract icon into ImageList
      bResult = (*pfnGetCachedIconIndex)(szPath, iIconIndex, iFlags);

   // Return result
   return bResult;
}


/// Function name  : utilGetWindowsVersion
// Description     : Determines the current version of Windows
// 
// Return Value   : Windows version, or WINDOWS_UNKNOWN if newer than Windows 7
// 
UtilityAPI 
WINDOWS_VERSION    utilGetWindowsVersion()
{
   OSVERSIONINFO    oWindowsData;      // Windows data
   WINDOWS_VERSION  eOutput;           // Operation result

   // Prepare
   utilZeroObject(&oWindowsData, OSVERSIONINFO);
   oWindowsData.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
   eOutput                          = WINDOWS_FUTURE;
   
   // Query windows version
   if (GetVersionEx(&oWindowsData))
   {
      // [SUCCESS] Examine result
      switch (oWindowsData.dwMajorVersion)
      {
      /// [WINDOWS NT 5] Windows 2000 or Windows XP
      case 5:  
         switch (oWindowsData.dwMinorVersion)
         {
         case 0:     eOutput = WINDOWS_2000;   break;
         case 1:     eOutput = WINDOWS_XP;     break;
         case 2:     eOutput = WINDOWS_2003;   break;
         }
         break;

      /// [WINDOWS NT 6] Windows Vista, 7, or newer
      case 6:
         switch (oWindowsData.dwMinorVersion)
         {
         case 0:     eOutput = WINDOWS_VISTA;   break;
         case 1:     eOutput = WINDOWS_7;       break;
         default:    eOutput = WINDOWS_FUTURE;  break;
         }
         break;

      /// [WINDOWS NT ?] Newer
      default:
         eOutput = WINDOWS_FUTURE;
         break;
      }
   }

   // Return result
   return eOutput;
}

/// Function name  : utilGetWindowsVersionString
// Description     : Determines the current version of Windows
// 
// Return Value   : New string containing windows version
// 
UtilityAPI 
TCHAR*   utilGetWindowsVersionString()
{
   OSVERSIONINFO    oWindowsData;      // Windows data
   CONST TCHAR*     szVersion;         // Windows string
   TCHAR*           szOutput;          // Operation result

   // Prepare
   utilZeroObject(&oWindowsData, OSVERSIONINFO);
   oWindowsData.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

   /// Identify windows version string
   switch (utilGetWindowsVersion())
   {
   case WINDOWS_2000:    szVersion = TEXT("Windows 2000");         break;
   case WINDOWS_XP:      szVersion = TEXT("Windows XP");           break;
   case WINDOWS_2003:    szVersion = TEXT("Windows Server 2003");  break;
   case WINDOWS_VISTA:   szVersion = TEXT("Windows Vista");        break;
   case WINDOWS_7:       szVersion = TEXT("Windows 7");            break;
   case WINDOWS_FUTURE:  szVersion = TEXT("Windows Future");       break;
   }

   /// Query service pack
   if (GetVersionEx(&oWindowsData))
      // [SUCCESS] Generate full string
      szOutput = utilCreateStringf(128, TEXT("%s %s"), szVersion, oWindowsData.szCSDVersion);
   else
      // [FAILED] Output version alone
      szOutput = utilDuplicateSimpleString(szVersion);

   // Return version string
   return szOutput;
}

/// Function name  : utilLoadResourceFile
// Description     : Fill a buffer with the contents of a binary resource converted to UNICODE
// 
// HINSTANCE     hInstance       : [in]  Handle of the module containing the resource
// CONST TCHAR*  szResourceName  : [in]  Name of the resource
// TCHAR*       &pOutput         : [out] New buffer containing the UNICODE version of the resource if succesful, otherwise NULL
// 
// Return Value   : Length of resource (in characters) if succesful, otherwise NULL
// 
UtilityAPI
UINT  utilLoadResourceFile(HINSTANCE  hInstance, CONST TCHAR*  szResourceName, TCHAR*  &szOutput)
{
   HRSRC    hResource;         // Handle to the resource containing the file
   HGLOBAL  hResourceData;     // Handle to the global memory where the resource was loaded
   CHAR*    szResourceFileA;   // ANSI buffer containing the resource data
   UINT     iFileSize;         // File size, in characters

   // Prepare
   iFileSize = NULL;

   /// Find the specified resource
   if (hResource = FindResource(hInstance, szResourceName, TEXT("RT_RCDATA")))
   {
      // Load resource into global memory, retrieve an accessible pointer
      hResourceData   = LoadResource(hInstance, hResource);
      szResourceFileA = (CHAR*)LockResource(hResourceData);

      // Determine resource size
      iFileSize = SizeofResource(hInstance, hResource);

      /// Create a UNICODE duplicate of the resource
      szOutput = utilTranslateStringToUNICODE(szResourceFileA, iFileSize);

      // Cleanup
      FreeResource(hResourceData);

      // NOTE: According to MSDN it is not necessary to release either of the resource handles
      //      'It is not necessary to unlock resources because the system automatically deletes them when the process that created them terminates'
   }

   // return the resource size
   return iFileSize;
}


/// Function name  : utilLoadResourceFileA
// Description     : Returns the buffer containing a binary resource
// 
// HINSTANCE     hInstance       : [in]  Handle of the module containing the resource
// CONST TCHAR*  szResourceName  : [in]  Name of the resource
// CONST BYTE*  &pOutput         : [out] Buffer containing the ANSI version of the resource if succesful, otherwise NULL
// 
// Return Value   : Length of resource (in characters) if succesful, otherwise NULL
// 
UtilityAPI
UINT  utilLoadResourceFileA(HINSTANCE  hInstance, CONST TCHAR*  szResourceName, CONST BYTE*  &szOutputA)
{
   HRSRC    hResource;         // Handle to the resource containing the file
   HGLOBAL  hResourceData;     // Handle to the global memory where the resource was loaded
   //CHAR*    szResourceFileA;   // ANSI buffer containing the resource data
   UINT     iFileSize;         // File size, in characters

   // Prepare
   iFileSize = NULL;
   szOutputA = NULL;

   /// Find the specified resource
   if (hResource = FindResource(hInstance, szResourceName, TEXT("RT_RCDATA")))
   {
      // Load resource into global memory, retrieve an accessible pointer
      hResourceData = LoadResource(hInstance, hResource);
      szOutputA     = (CONST BYTE*)LockResource(hResourceData);

      // Determine resource size
      iFileSize = SizeofResource(hInstance, hResource);

      // Cleanup
      FreeResource(hResourceData);

      // NOTE: According to MSDN it is not necessary to release either of the resource handles
      //      'It is not necessary to unlock resources because the system automatically deletes them when the process that created them terminates'
   }

   // return the resource size
   return iFileSize;
}


/// Function name  : utilSetProcessWorkingFolder
// Description     : Sets the working folder to that containing the process EXE
// 
UtilityAPI
VOID  utilSetProcessWorkingFolder()
{
   TCHAR   *szModulePath,         // Full path of the EXE
           *szCurrentFolder;      // Folder containing the EXE

   // Prpeare
   szModulePath = utilCreateEmptyPath();

   /// Ensure folder containing the exe is the working folder
   GetModuleFileName(NULL, szModulePath, MAX_PATH);
   szCurrentFolder = utilDuplicateFolderPath(szModulePath);
   SetCurrentDirectory(szCurrentFolder);

   // Cleanup
   utilDeleteStrings(szCurrentFolder, szModulePath);
}




/// Configuration  : DEBUG 
/// Function name  : utilSetThreadName
// Description     : Sets the name of the current thread in the MS-VC debugger
// 
// CHAR*  szThreadNameA : [in] Name for the thread
// 
#ifdef _DEBUG
UtilityAPI 
VOID  utilSetThreadName(CHAR*  szThreadNameA)
{
   THREAD_NAME oThreadName;

   // Define MS:VC debugger data    (Code taken directly from MSDN article 'SetThreadName')
   oThreadName.dwType     = 0x1000;
   oThreadName.szName     = szThreadNameA;
   oThreadName.dwThreadID = GetCurrentThreadId();
   oThreadName.dwFlags    = NULL;

   /// Invoke MS:VC debugging exception. (Code taken directly from MSDN article 'SetThreadName')
   __try
   {
      RaiseException(MS_VC_EXCEPTION, 0, sizeof(oThreadName)/sizeof(ULONG_PTR), (ULONG_PTR*)&oThreadName );
   }
   __except(EXCEPTION_EXECUTE_HANDLER)
   {
   }
}
#endif
