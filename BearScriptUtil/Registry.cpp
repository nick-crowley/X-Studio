//
// Registry.cpp : Registry Utility functions
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : utilRegistryCheckAppKeyExists
// Description     : Check whether the key containing application data exists
// 
// CONST TCHAR*  szAppName  : [in] Name of the application
// 
// Return Value   : TRUE if exists, FALSE otherwise
// 
UtilityAPI
BOOL  utilRegistryCheckAppKeyExists(CONST TCHAR*  szAppName)
{
   HKEY   hAppKey;   // Key handle

   // [CHECK] Can we open the specified key?
   if (hAppKey = utilRegistryOpenAppKey(szAppName))
      // [SUCCESS] Cleanup
      RegCloseKey(hAppKey);

   // Return TRUE if successful
   return (hAppKey != NULL);
}


/// Function name  : utilRegistryCloseKey
// Description     : Closes a registry key and zeros the handle
// 
// HKEY&  hKey   : [in] Registry key handle to close
// 
UtilityAPI 
VOID  utilRegistryCloseKey(HKEY&  hKey)
{
   RegCloseKey(hKey);
   hKey = NULL;
}

/// Function name  : utilRegistryCreateAppKey
// Description     : Create a registry key for the application in the appropriate location
// 
// CONST TCHAR*  szAppName   : [in] Application name
// 
// Return Value   : New key if succesful, otherwise NULL
// 
UtilityAPI
HKEY  utilRegistryCreateAppKey(CONST TCHAR*  szAppName)
{
   HKEY    hKey;       // Key handle
   BOOL    bResult;    // Operation result
   TCHAR*  szKeyName;

   // Build full key name
   szKeyName = utilCreateString(256);
   StringCchPrintf(szKeyName, 256, TEXT("Software\\Bearware\\%s"), szAppName);

   // Attempt to create key
   bResult = (RegCreateKey(HKEY_CURRENT_USER, szKeyName, &hKey) == ERROR_SUCCESS);
   
   // Cleanup and return
   utilDeleteString(szKeyName);
   return bResult ? hKey : NULL;
}


/// Function name  : utilRegistryCreateAppSubKey
// Description     : Create a registry sub-key for the application in the appropriate location
// 
// CONST TCHAR*  szAppName   : [in] Application name
// CONST TCHAR*  szSubKey    : [in] Name of the sub-key
// 
// Return Value   : New key if succesful, otherwise NULL
// 
UtilityAPI
HKEY  utilRegistryCreateAppSubKey(CONST TCHAR*  szAppName, CONST TCHAR*  szSubKey)
{
   HKEY    hKey;       // Key handle
   BOOL    bResult;    // Operation result
   TCHAR*  szKeyName;

   // [CHECK] Ensure subkey was provided
   ASSERT(lstrlen(szSubKey));

   // Build full key name
   szKeyName = utilCreateString(256);
   StringCchPrintf(szKeyName, 256, TEXT("Software\\Bearware\\%s\\%s"), szAppName, szSubKey);

   // Attempt to create key
   bResult = (RegCreateKey(HKEY_CURRENT_USER, szKeyName, &hKey) == ERROR_SUCCESS);
   
   // Cleanup and return
   utilDeleteString(szKeyName);
   return bResult ? hKey : NULL;
}


/// Function name  : utilRegistryDeleteAppKey
// Description     : Destroys the registry key for the application 
// 
// CONST TCHAR*  szAppName : [in] Application name
// 
// Return Value : TRUE if successful, otherwise FALSE
// 
UtilityAPI
BOOL  utilRegistryDeleteAppKey(CONST TCHAR*  szAppName)
{
   BOOL    bResult;    // Operation result
   TCHAR*  szKeyName;  // Path of the key being destroyed

   // Build full key name
   szKeyName = utilCreateString(256);
   StringCchPrintf(szKeyName, 256, TEXT("Software\\Bearware\\%s"), szAppName);

   // Attempt to destroy key
   bResult = (SHDeleteKey(HKEY_CURRENT_USER, szKeyName) == ERROR_SUCCESS);
   
   // Cleanup and return
   utilDeleteString(szKeyName);
   return bResult;
}


/// Function name  : utilRegistryDeleteAppSubKey
// Description     : Destroys the specified sub-key of the app's registry key 
// 
// CONST TCHAR*  szAppName : [in] Application name
// CONST TCHAR*  szSubKey  : [in] Name of the subkey
// 
// Return Value : TRUE if successful, otherwise FALSE
// 
UtilityAPI
BOOL  utilRegistryDeleteAppSubKey(CONST TCHAR*  szAppName, CONST TCHAR*  szSubKey)
{
   BOOL    bResult;    // Operation result
   TCHAR*  szKeyName;  // Path of the key being destroyed
  
   // [CHECK] Ensure subkey was provided
   ASSERT(lstrlen(szSubKey));

   // Build full key name
   szKeyName = utilCreateString(256);
   StringCchPrintf(szKeyName, 256, TEXT("Software\\Bearware\\%s\\%s"), szAppName, szSubKey);

   // Attempt to destroy key
   bResult = (SHDeleteKey(HKEY_CURRENT_USER, szKeyName) == ERROR_SUCCESS);
   
   // Cleanup and return
   utilDeleteString(szKeyName);
   return bResult;
}

/// Function name  : utilRegistryOpenAppKey
// Description     : Open the registry key holding the application data
// 
// CONST TCHAR*  szAppName  : [in] Application name
// 
// Return Value   : Open registry key if successful, or NULL if the key does not exist (or there was an error)
// 
UtilityAPI
HKEY  utilRegistryOpenAppKey(CONST TCHAR*  szAppName)
{
   HKEY    hKey;       // Key handle
   BOOL    bResult;    // Operation result
   TCHAR*  szKeyName;

   // Build full key name
   szKeyName = utilCreateString(256);
   StringCchPrintf(szKeyName, 256, TEXT("Software\\Bearware\\%s"), szAppName);

   // Attempt to create key
   bResult = (RegOpenKey(HKEY_CURRENT_USER, szKeyName, &hKey) == ERROR_SUCCESS);
   
   // Cleanup and return
   utilDeleteString(szKeyName);
   return (bResult ? hKey : NULL);
}


/// Function name  : utilRegistryOpenAppSubKey
// Description     : Open the registry key holding the application data
// 
// CONST TCHAR*  szAppName  : [in] Application name
// CONST TCHAR*  szSubKey   : [in] Subkey name
// 
// Return Value   : Open registry key if successful, or NULL if the key does not exist (or there was an error)
// 
UtilityAPI
HKEY  utilRegistryOpenAppSubKey(CONST TCHAR*  szAppName, CONST TCHAR*  szSubKey)
{
   HKEY    hKey;       // Key handle
   BOOL    bResult;    // Operation result
   TCHAR*  szKeyName;

   // [CHECK] Ensure subkey was provided
   ASSERT(lstrlen(szSubKey));

   // Build full key name
   szKeyName = utilCreateString(256);
   StringCchPrintf(szKeyName, 256, TEXT("Software\\Bearware\\%s\\%s"), szAppName, szSubKey);

   // Attempt to create key
   bResult = (RegOpenKey(HKEY_CURRENT_USER, szKeyName, &hKey) == ERROR_SUCCESS);
   
   // Cleanup and return
   utilDeleteString(szKeyName);
   return (bResult ? hKey : NULL);
}


/// Function name  : utilRegistryReadBinary
// Description     : Read the contents of a binary registry value into a buffer
// 
// HKEY          hKey        : [in]  Already open registry key
// CONST TCHAR*  szName      : [in]  Registry value name
// BYTE*         pBuffer     : [out] Buffer to store data in
// DWORD         iBufferSize : [in]  Length of buffer above, in bytes
// 
// Return Value   : TRUE if successful, otherwise FALSE
// 
UtilityAPI
BOOL  utilRegistryReadBinary(HKEY  hKey, CONST TCHAR*  szName, BYTE*  pBuffer, DWORD  iBufferSize)
{
   return (RegQueryValueEx(hKey, szName, NULL, NULL, pBuffer, &iBufferSize) == ERROR_SUCCESS);
}


/// Function name  : utilRegistryReadInteger
// Description     : Access a registry integer
//
// HKEY          hKey     : [in]  Open registry key
// CONST TCHAR*  szName   : [in]  Name of the value to retrieve
// DWORD&        dwOutput : [out] Registry integer value
// 
// Return type : TRUE if successful, otherwise FALSE
//
UtilityAPI
BOOL  utilRegistryReadInteger(HKEY  hKey, CONST TCHAR*  szName, UINT*  piOutput)
{
   DWORD    dwRegValueSize;   // Value size
            
   dwRegValueSize = sizeof(UINT);

   return (RegQueryValueEx(hKey, szName, NULL, NULL, (BYTE*)piOutput, &dwRegValueSize) == ERROR_SUCCESS);
}


/// Function name  : utilRegistryReadString
// Description     : Access a registry string
//
// HKEY          hKey          : [in]     Open registry key
// CONST TCHAR*  szName        : [in]     Name of the value to retrieve
// TCHAR*        szBuffer      : [in/out] Buffer to hold string
// UINT          iBufferLength : [in]     Length of buffer in characters
// 
// Return type : Number of characters retrieved if succesful, otherwise FALSE
//
UtilityAPI
BOOL  utilRegistryReadString(HKEY  hKey, CONST TCHAR*  szName, TCHAR*  szBuffer, UINT  iBufferLength)
{
   // Convert buffer length from bytes to characters
   iBufferLength *= sizeof(TCHAR);

   // Ensure string is null terminated
   iBufferLength -= sizeof(TCHAR);
   szBuffer[iBufferLength];

   // Read string
   return (RegQueryValueEx(hKey, szName, NULL, NULL, (BYTE*)szBuffer, (DWORD*)&iBufferLength) == ERROR_SUCCESS ? iBufferLength : FALSE);
}


/// Function name  : utilRegistryWriteBinary
// Description     : Copy the contents of a buffer to a binary registry value
// 
// HKEY          hKey        : [in] Registry key
// CONST TCHAR*  szName      : [in] Value name
// CONST BYTE*   pBuffer     : [in] Buffer
// CONST DWORD   iBufferSize : [in] Length of buffer, in bytes
// 
// Return Value   : TRUE if successful, otherwise FALSE
// 
UtilityAPI
BOOL  utilRegistryWriteBinary(HKEY  hKey, CONST TCHAR*  szName, CONST BYTE*  pBuffer, CONST DWORD  iBufferSize)
{
   return (RegSetValueEx(hKey, szName, NULL, REG_BINARY, pBuffer, iBufferSize) == ERROR_SUCCESS);
}

/// Function name  : utilRegistryWriteInteger
// Description     : Write an integer to the registry
// 
// HKEY          hKey     : [in] Registry key
// CONST TCHAR*  szName   : [in] Value name
// CONST UINT    iValue   : [in] Buffer
// 
// Return Value   : TRUE if successful, otherwise FALSE
// 
UtilityAPI
BOOL  utilRegistryWriteInteger(HKEY  hKey, CONST TCHAR*  szName, CONST UINT  iValue)
{
   return (RegSetValueEx(hKey, szName, NULL, REG_DWORD, (CONST BYTE*)&iValue, sizeof(UINT)) == ERROR_SUCCESS);
}


/// Function name  : utilRegistryWriteString
// Description     : Saves a string to the registry
//
// HKEY          hKey      : [in] Open registry key
// CONST TCHAR*  szName    : [in] Name of the value to retrieve
// CONST TCHAR*  szStirng  : [in] String to save
// 
// Return type : TRUE if successful, otherwise FALSE
//
UtilityAPI
BOOL  utilRegistryWriteString(HKEY  hKey, CONST TCHAR*  szName, CONST TCHAR*  szString)
{
   UINT  iLength;

   // Calculate length of string in characters  (Including the NULL terminator)
   iLength = (lstrlen(szString) + 1) * sizeof(TCHAR); 

   // Save string
   return (RegSetValueEx(hKey, szName, NULL, REG_SZ, (BYTE*)szString, iLength) == ERROR_SUCCESS);
}


/// Function name  : utilRegistryQueryValueType
// Description     : Query the type of a specified registry value
//
// HKEY          hKey    : [in]  Open registry key
// CONST TCHAR*  szValue : [in]  Name of the value to retrieve
// UINT&         piType  : [out] Type of the specified value
// 
// Return type : TRUE if successful, otherwise FALSE
//
UtilityAPI
BOOL  utilRegistryQueryValueType(HKEY  hKey, CONST TCHAR*  szValue, UINT*  piType)
{
   return (RegQueryValueEx(hKey, szValue, NULL, (DWORD*)piType, NULL, NULL) == ERROR_SUCCESS);
}









