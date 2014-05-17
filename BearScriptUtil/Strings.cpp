//
// Strings.cpp : String Utility functions
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       CREATION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : utilCreateEmptyString
// Description     : Allocate a zero'd string buffer of a specified character length
// 
// CONST UINT  iBufferSizeInChars : [in] Size of buffer, in characters
// 
// Return type : Newly created buffer, you are responsible for destroying it 
//
UtilityAPI 
TCHAR*      utilCreateEmptyString(CONST UINT  iBufferSizeInChars)
{
   TCHAR*  pNewString;
   
   // Allocate and zero string
   pNewString = utilCreateString(iBufferSizeInChars);
   utilZeroMemory(pNewString, sizeof(TCHAR) * iBufferSizeInChars);

   // Return new string
   return pNewString;
}


/// Function name  : utilCreateEmptyStringA
// Description     : Allocate a zero'd ANSI string buffer of a specified character length
// 
// CONST UINT  iBufferSizeInChars : [in] Size of buffer, in characters
// 
// Return type : Newly created ANSI string buffer, you are responsible for destroying it 
//
UtilityAPI 
CHAR*      utilCreateEmptyStringA(CONST UINT  iBufferSizeInChars)
{
   CHAR*  pNewString;
   
   // Allocate and zero string
   pNewString = (CHAR*)utilAllocateMemory(iBufferSizeInChars);
   utilZeroMemory(pNewString, iBufferSizeInChars);

   // Return new string
   return pNewString;
}


/// Function name  : utilCreateString
// Description     : Allocate a string buffer of a specified character length
// 
// CONST UINT  iBufferSizeInChars : [in] Size of buffer, in characters
// 
// Return type : Newly created buffer, you are responsible for destroying it 
//
UtilityAPI 
TCHAR*      utilCreateString(CONST UINT  iBufferSizeInChars)
{
   // Allocate the specified number of TCHARs
   return (TCHAR*)utilAllocateMemory(sizeof(TCHAR) * iBufferSizeInChars);
}


/// Function name  : utilCreateStringf
// Description     : Creates a formatted string buffer
// 
// CONST UINT    iLength  : [in] Size of buffer, in characters
// CONST TCHAR*  szFormat : [in] Formatting string
// ...           ...      : [in] Variable arguments
// 
// Return type : Newly created buffer, you are responsible for destroying it 
//
UtilityAPI 
TCHAR*      utilCreateStringf(CONST UINT  iLength, CONST TCHAR*  szFormat, ...)
{
   TCHAR*    szOutput;  // String being created
   va_list   pArgList;  // Argument list

   // Prepare
   pArgList = utilGetFirstVariableArgument(&szFormat);
   szOutput = utilCreateEmptyString(iLength);

   // Generate Fromated output
   StringCchVPrintf(szOutput, iLength, szFormat, pArgList);

   // Return result
   return szOutput;
}


/// Function name  : utilCreateStringFromInteger
// Description     : Creates a new string containing a numeric representation
// 
// CONST INT  iValue : [in] Value to convert into a string
// 
// Return Value   : New string buffer, you are responsible for destroying it
//
UtilityAPI
TCHAR*      utilCreateStringFromInteger(CONST INT  iValue)
{
   TCHAR*  pNewString;     // String being created

   // Create new object
   pNewString = utilCreateEmptyString(32);
   StringCchPrintf(pNewString, 32, TEXT("%d"), iValue);

   // Return new object
   return pNewString;
}


/// Function name  : utilCreateStringVf
// Description     : Creates a formatted string buffer
// 
// CONST UINT    iLength  : [in] Size of buffer, in characters
// CONST TCHAR*  szFormat : [in] Formatting string
// va_list       pArgList : [in] Variable argument list
// 
// Return type : Newly created buffer, you are responsible for destroying it 
//
UtilityAPI 
TCHAR*      utilCreateStringVf(CONST UINT  iLength, CONST TCHAR*  szFormat, va_list  pArgList)
{
   TCHAR*  szOutput;    // String being created

   // Prepare
   szOutput = utilCreateEmptyString(iLength);

   // Fromat output
   StringCchVPrintf(szOutput, iLength, szFormat, pArgList);

   // Return result
   return szOutput;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : utilDeleteString
// Description     : Free a string buffer previously allocated using 'utilCreateString' or 'utilCreateEmptyString'
// 
// TCHAR*  &szStringBuffer : [in] Reference to a string buffer to be deleted
// 
UtilityAPI 
VOID   utilDeleteString(TCHAR*  &szStringBuffer)
{
   // [CHECK] Ensure string exists
   if (szStringBuffer)
      utilDeleteObject(szStringBuffer);
}


/// Function name  : utilDeleteStringA
// Description     : Free a string buffer previously allocated using 'utilCreateString' or 'utilCreateEmptyString'
// 
// CHAR*  &szStringBufferA : [in] Reference to an ANSI string buffer to be deleted
// 
UtilityAPI 
VOID   utilDeleteStringA(CHAR*  &szStringBuffer)
{
   // [CHECK] Ensure string exists
   if (szStringBuffer)
      utilDeleteObject(szStringBuffer);
}


/// Function name  : utilDeleteStrings
// Description     : Free multiple string buffers previously allocated using 'utilCreateString' or 'utilCreateEmptyString'
// 
// TCHAR*  &szFirstString  : [in] String to delete
// TCHAR*  &szSecondString : [in] String to delete
// 
UtilityAPI 
VOID   utilDeleteStrings(TCHAR*  &szFirstString, TCHAR*  &szSecondString)
{
   // [CHECK] Ensure strings exist
   if (szFirstString)
      utilDeleteObject(szFirstString);

   if (szSecondString)
      utilDeleteObject(szSecondString);
}

/// Function name  : utilDeleteStrings
// Description     : Free multiple string buffers previously allocated using 'utilCreateString' or 'utilCreateEmptyString'
// 
// TCHAR*  &szFirstString  : [in] String to delete
// TCHAR*  &szSecondString : [in] String to delete
// TCHAR*  &szThirdString  : [in] String to delete
// 
UtilityAPI 
VOID   utilDeleteStrings(TCHAR*  &szFirstString, TCHAR*  &szSecondString, TCHAR*  &szThirdString)
{
   // [CHECK] Ensure strings exist
   if (szFirstString)
      utilDeleteObject(szFirstString);
   if (szSecondString)
      utilDeleteObject(szSecondString);    
   if (szThirdString)
      utilDeleteObject(szThirdString);
}


/// Function name  : utilSafeDeleteString
// Description     : Attempt to free a string buffer, if it exists.
// 
// TCHAR*  &szStringBuffer : [in] Reference to a string buffer to be deleted
// 
UtilityAPI 
VOID   utilSafeDeleteString(TCHAR*  &szStringBuffer)
{
   // [CHECK] Ensure string exist
   if (szStringBuffer)
      // Destroy string
      utilDeleteObject(szStringBuffer);
}


/// Function name  : utilSafeDeleteStrings
// Description     : Attempt to free multiple string buffers, if they exist
// 
// TCHAR*  &szFirstString  : [in] String to delete
// TCHAR*  &szSecondString : [in] String to delete
// 
UtilityAPI 
VOID   utilSafeDeleteStrings(TCHAR*  &szFirstString, TCHAR*  &szSecondString)
{
   // [CHECK] Ensure strings exists
   if (szFirstString)
      utilDeleteObject(szFirstString);

   // [CHECK] Ensure strings exists
   if (szSecondString)
      utilDeleteObject(szSecondString);   
}


/// Function name  : utilSafeDeleteStrings
// Description     : Attempt to free multiple string buffers, if they exist
// 
// TCHAR*  &szFirstString  : [in] String to delete
// TCHAR*  &szSecondString : [in] String to delete
// TCHAR*  &szThirdString  : [in] String to delete
// 
UtilityAPI 
VOID   utilSafeDeleteStrings(TCHAR*  &szFirstString, TCHAR*  &szSecondString, TCHAR*  &szThirdString)
{
   // [CHECK] Ensure strings exists
   if (szFirstString)
      utilDeleteObject(szFirstString);

   // [CHECK] Ensure strings exists
   if (szSecondString)
      utilDeleteObject(szSecondString);   

   // [CHECK] Ensure strings exists
   if (szThirdString)
      utilDeleteObject(szThirdString);   
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                      FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : utilDuplicateString
// Description     : Create a new string on the heap with the same dimensions and content as an existing string.
//
// CONST TCHAR*  szSource      : [in] Existing string
// CONST UINT    iSourceLength : [in] Length of existing string, in characters
// 
// Return type : Newly allocated string, you are responsible for destroying it
//
UtilityAPI 
TCHAR*   utilDuplicateString(CONST TCHAR*  szSource, CONST UINT  iSourceLength)
{
   TCHAR*   szNewString;

   // Create new string
   szNewString = utilCreateString(iSourceLength + 1);

   // Copy string data from existing string
   if (szSource)
      utilCopyString(szNewString, szSource, iSourceLength);

   // Null terminate
   szNewString[iSourceLength] = NULL;   

   // Return new string
   return szNewString;   
}


/// Function name  : utilDuplicateSubString
// Description     : Create a new string from the sub-string of an existing string
//
// CONST TCHAR*  szSource         : [in] Input string
// CONST UINT    iSourceLength    : [in] Length of input string, in characters
// CONST UINT    iOffset          : [in] Zero-based character offset at which sub-string begins
// CONST UINT    iSubStringLength : [in] Length of sub-string, in characters
// 
// Return type : Newly allocated string, you are responsible for destroying it
//
UtilityAPI
TCHAR*   utilDuplicateSubString(CONST TCHAR*  szSource, CONST UINT  iSourceLength, CONST UINT  iOffset, CONST UINT  iSubStringLength)
{
   CONST TCHAR*  szStart;    // Convenience pointer
   TCHAR*        szOutput;   // Operation result

   /// Determine the start of the substring
   szStart = &szSource[min(iOffset, iSourceLength)];  // Do not exceed the length of the string

   /// Extract substring
   szOutput = utilDuplicateString(szStart, min(iSubStringLength, iSourceLength - iOffset));     // Do not exceed the length of the string

   // Return result
   return szOutput;
}


/// Function name  : utilExtendString
// Description     : Extend a string buffer by re-allocating it and copying the original contents across.
//
// TCHAR*          szSource        : String buffer to re-allocate
///                                                         NOTE: Original buffer is deleted
// CONST UINT      iSourceLength   : Buffer length -OR- String length.  (Both in characters)
// CONST UINT      iNewLength      : New buffer length, in characters
// 
// Return type : New string buffer, you are responsible for deleting it.
///                                                         NOTE: Extended buffer portion is zero'd
UtilityAPI 
TCHAR*   utilExtendString(TCHAR*  szSource, CONST UINT  iSourceLength, CONST UINT  iNewLength)
{
   TCHAR*  szNewString;

   // [CHECK] Ensure new length is longer than existing length
   if (iNewLength <= iSourceLength)
      return NULL;

   // Allocate new buffer
   szNewString = utilCreateString(iNewLength + 1);      

   // Copy over contents of existing buffer
   utilCopyString(szNewString, szSource, iSourceLength);       
   
   // Zero right-hand-side and null terminate
   utilZeroString(&szNewString[iSourceLength], iNewLength - iSourceLength);      
   szNewString[iNewLength] = NULL;
   
   // Cleanup and return
   utilDeleteString(szSource);   
   return szNewString;
}


/// Function name  : utilLoadString
// Description     : Loads a string resource into a newly minted string buffer
//
// HINSTANCE   hInstance     : [in] Handle to the instance containing the string resource
// CONST UINT  iResourceID   : [in] ID of the string resource
// UINT        iBufferLength : [in][optional] Desired length of the string buffer or NULL to use length of string
// 
// Return type : New string, you are responsible for deleting it
//
UtilityAPI 
TCHAR*   utilLoadString(HINSTANCE  hInstance, CONST UINT  iResourceID, UINT  iBufferLength /*= NULL*/)
{
   TCHAR*  szNewString;
   HRSRC   hResource;

   if (iBufferLength == NULL)
   {
      /// BUG: Doesn't find the resources
      // Query length of string and calculate necessary buffer length (in characters)
      hResource = FindResource(hInstance, (TCHAR*)(iResourceID), RT_STRING);
      ASSERT(hResource);
      iBufferLength = 1 + SizeofResource(hInstance, hResource) / sizeof(TCHAR);
   }

   // Load string
   szNewString = utilCreateEmptyString(iBufferLength);
   LoadString(hInstance, iResourceID, szNewString, iBufferLength);

   return szNewString;
}

/// Function name  : utilReAllocString
// Description     : Re-allocates a string using a shorter buffer
//
// TCHAR*  szSource      : [in] Existing string
// 
// Return type : Newly allocated string, you are responsible for destroying it
//
UtilityAPI 
TCHAR*   utilReAllocString(TCHAR*  szSource)
{
   TCHAR*  szCopy = utilDuplicateSimpleString(szSource);
   utilDeleteString(szSource);
   return szCopy;
}

/// Function name  : utilReplaceString
// Description     : Delete an existing string buffer and replace it with the contents of another
// 
// TCHAR*        &szExistingString : [in/out] The string buffer to replace
// CONST TCHAR*  &szNewString      : [in]     The string buffer to duplicate in it's place
// 
// Return type : New string buffer, you are responsible for destroying it.
//
UtilityAPI
TCHAR*   utilReplaceString(TCHAR*  &szExistingString, CONST TCHAR*  szNewString)
{
   // [CHECK] Ensure input strings are not the same
   if (szExistingString != szNewString)
   {
      // Replace existing string
      utilSafeDeleteString(szExistingString);
      szExistingString = utilDuplicateString(szNewString, lstrlen(szNewString));
   }

   // Return new string
   return szExistingString;
}


/// Function name  : utilReplaceStringf
// Description     : Replace an existing string with a new formatted string
// 
// TCHAR*        &szExistingString : [in]  The string to replace (can be unallocated)
///                                  [out] The new formatted string
// CONST UINT     iNewLength       : [in]  Length of the new formatted output
// CONST TCHAR*   szFormat         : [in]  Formatting string
// ...            ....             : [in]  Variable arguments
// 
// Return type : New string buffer, you are responsible for destroying it.
//
UtilityAPI
TCHAR*   utilReplaceStringf(TCHAR*  &szExistingString, CONST UINT  iNewLength, CONST TCHAR*  szFormat, ...)
{
   va_list  pArgList;      // Variable argument pointer
   
   // Prepare
   pArgList = utilGetFirstVariableArgument(&szFormat);

   // Delete existing string, if any
   utilSafeDeleteString(szExistingString);

   // Create output
   szExistingString = utilCreateStringVf(iNewLength, szFormat, pArgList);
   
   // Return output
   return szExistingString;
}


/// Function name  : utilStringCchCatf
// Description     : Append a printf formatted string to an existing string
// 
// TCHAR*        szDestination : [in] Destination string
// CONST UINT    iLength       : [in] Length of destination string
// CONST TCHAR*  szFormat      : [in] Formatting string
// ...           ....          : [in] Variable arguments
// 
// Return Value   : StringCchPrintf result code
// 
UtilityAPI
HRESULT  utilStringCchCatf(TCHAR*  szDestination, CONST UINT  iLength, CONST TCHAR*  szFormat, ...)
{
   UINT     iDestinationUsed;
   va_list  xArgList;

   // Prepare
   va_start(xArgList, szFormat);
   iDestinationUsed = lstrlen(szDestination);

   // Pass to printf
   return StringCchVPrintf(&szDestination[iDestinationUsed], iLength - iDestinationUsed, szFormat, xArgList);
}


/// Function name  : utilTranslateStringToUNICODE
// Description     : Convert an ANSI string to it's UNICODE equivilent
// 
// CONST CHAR*  szText      : [in] ANSI string to convert
// CONST UINT   iTextLength : [in] Length of string, in characters
// 
// Return Value   : Pointer to new UNICODE string, if succesful, otherwise NULL
// 
UtilityAPI 
TCHAR*  utilTranslateStringToUNICODE(CONST CHAR*  szText, CONST UINT  iTextLength)
{
   TCHAR*  szOutput;
   UINT    iCharsTranslated;

   // Create null terminated output buffer
   szOutput = utilCreateString(iTextLength + 1);
   szOutput[iTextLength] = NULL;

   // [CHECK] No processing required for empty strings
   if (iTextLength > 0)
   {
      /// Attempt to convert ANSI to UNICODE
      if (iCharsTranslated = MultiByteToWideChar(CP_UTF8, 0, szText, iTextLength, szOutput, iTextLength))
         // [SUCCESS] Ensure string is properly NULL terminated, as characters are sometimes dropped in translation
         szOutput[iCharsTranslated] = NULL;
      else
         // [FAILED] Cleanup
         utilDeleteString(szOutput);
   }

   // Return new string (or NULL)
   return szOutput;
}


/// Function name  : utilTranslateStringToUNICODEEx
// Description     : Convert an ANSI string to it's UNICODE equivilent, and update the string length to match the output
// 
// CONST CHAR*  szText      : [in]     ANSI string to convert
// UINT&        iTextLength : [in/out] Length of string in characters
///                                          This may be altered if un-translatable characters were dropped during processing
// 
// Return Value   : Pointer to new UNICODE string, if succesful, otherwise NULL
// 
UtilityAPI 
TCHAR*  utilTranslateStringToUNICODEEx(CONST CHAR*  szText, UINT&  iTextLength)
{
   TCHAR*  szOutput;

   // Create null terminated output buffer
   szOutput = utilCreateString(iTextLength + 1);
   szOutput[iTextLength] = NULL;

   // [CHECK] No processing required for empty strings
   if (iTextLength > 0)
   {
      /// Attempt to convert ANSI to UNICODE
      if (iTextLength = MultiByteToWideChar(CP_UTF8, 0, szText, iTextLength, szOutput, iTextLength))
         // [SUCCESS] Ensure string is properly NULL terminated, as characters are sometimes dropped in translation 
         szOutput[iTextLength] = NULL;
      else
         // [FAILED] Cleanup
         utilDeleteString(szOutput);
   }

   // Return new string (or NULL)
   return szOutput;
}


/// Function name  : utilTranslateStringToANSI
// Description     : Convert an UNICODE string to it's ANSI equivilent
// 
// CONST TCHAR*  szText           : [in]     UNICODE string to convert
// UINT&         iTextLength      : [in/out] Length of string in characters
///                                             This may be altered if un-translatable characters were dropped during processing
// CONST UINT    iBufferExtension : [in]     Length of the output buffer compared to the input buffer, as a percentage. Must be greater than 100
// 
// Return Value   : New ANSI string if succesful, otherwise NULL
// 
UtilityAPI 
CHAR*  utilTranslateStringToANSI(CONST TCHAR*  szText, UINT&  iTextLength, CONST UINT  iBufferExtension)
{
   CHAR*  szOutput;        // Output string, as ANSI
   UINT   iBufferSize,     // Length of output buffer
          iCharsConverted;

   // [CHECK] Ensure extension percentage is greater than 100
   ASSERT(iBufferExtension >= 100);

   // Calculate buffer size as 10% longer than the input buffer (To account for translated characters)
   iBufferSize = utilCalculatePercentage(iTextLength, iBufferExtension);

   // Create null terminated output buffer
   szOutput = utilCreateEmptyObjectArray(CHAR, iBufferSize + 1);
   
   // [CHECK] No processing is required for empty strings
   if (iTextLength)
   {
      /// Attempt to convert UNICODE to ANSI
      if (iCharsConverted = WideCharToMultiByte(CP_UTF8, NULL, szText, iTextLength, szOutput, iBufferSize, NULL, NULL))
         // [SUCCESS] Update text length
         iTextLength = iCharsConverted;

      // [FAILED] Was there insufficient buffer space to complete the translation?
      else if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
      {
         /// [INSUFFICENT BUFFER] Try again using a longer buffer
         utilDeleteObject(szOutput);
         szOutput = utilTranslateStringToANSI(szText, iTextLength, iBufferExtension + 25);
      }
      else
         /// [OTHER] Cleanup and return NULL
         utilDeleteObject(szOutput);
   }
   
   // Return output (or NULL)
   return szOutput;
}
