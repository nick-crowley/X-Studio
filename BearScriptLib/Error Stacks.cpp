//
// Error Stacks.cpp  -- Represents a single error and provides a 'source trace' with each item in the stack 
//                        representing a different function within the trace. Also contains 'Attachment messages'
//                        which contain data relevant to the error that may be useful for debugging.
// 
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createErrorStack
// Description     : Creates an empty ERROR_STACK object
//
// Return type : New error stack, you are responsible for destroying it
//
ERROR_STACK*   createErrorStack()
{
   ERROR_STACK*  pErrorStack;

   // Create new object
   pErrorStack = utilCreateEmptyObject(ERROR_STACK);

   // Set properties
   pErrorStack->pfnDeleteItem = destructorErrorMessage;
   pErrorStack->eType         = ET_ERROR;

   // Return new object
   return pErrorStack;
}


/// Function name  : createErrorStackV
// Description     : Creates a new error containing a 'location' and 'explanation' error message
// 
// CONST UINT     iErrorCode  : [in] Error code
// CONST TCHAR*   szErrorCode : [in] String version of above
// CONST TCHAR*   szFunction  : [in] Name of function where the error occurred
// CONST TCHAR*   szFile      : [in] Name of file where the error occurred
// CONST UINT     iLine       : [in] Line number where error occurred
// va_list        pArguments  : [in] Custom arguments
// 
// Return Value   : 
// 
ERROR_STACK*  createErrorStackV(CONST UINT  iErrorCode, CONST TCHAR*  szErrorCode, CONST TCHAR*  szFunction, CONST TCHAR*  szFile, CONST UINT  iLine, va_list  pArguments)
{
   ERROR_MESSAGE* pMessage;      // Message to follow the 'location' message
   ERROR_STACK*   pError;        // Error being created
   
   // Prepare
   pError = createErrorStack();

   /// [LOCATION] Generate location message : "%s(..) has encountered an %s error (ID: %04d) in %s, line %d"
   enhanceError(pError, ERROR_ID(IDS_ERROR_APPEND_LOCATION), szFunction, szErrorCode, iErrorCode, szFile, iLine);   

   /// [ENHANCEMENT] Enhance message with input error and arguments
   pMessage = createErrorMessageV(iErrorCode, szErrorCode, pArguments);
   pushErrorStack(pError, pMessage);

   // Return Error
   return pError;
}


/// Function name  : destructorErrorStack
// Description     : Delete an ERROR_QUEUE ListItem
// 
// LPARAM  pErrorStack   : [in] ListItem data, (ERROR_STACK*)
// 
BearScriptAPI
VOID  destructorErrorStack(LPARAM  pErrorStack)
{
   // Delete error stack
   deleteErrorStack((ERROR_STACK*&)pErrorStack);
}


/// Function name  : duplicateErrorStack
// Description     : Create a new ErrorStack containing a copy of the contents of an existing ErrorStack
// 
// CONST ERROR_STACK*  pOriginal   : [in] ErrorStack to duplicate
// 
// Return Value   : New ErrorStack, you are responsible for destroying it
// 
BearScriptAPI
ERROR_STACK*  duplicateErrorStack(CONST ERROR_STACK*  pErrorStack)
{
   ERROR_STACK*    pNewStack;    // New ErrorStack
   ERROR_MESSAGE*  pMessage;     // ErrorStack message iterator

   // [CHECK] Ensure stack isn't empty
   ASSERT(getStackItemCount(pErrorStack));

   /// Create new ErrorStack
   pNewStack = createErrorStack();

   // Set properties
   pNewStack->eType = pErrorStack->eType;

   /// Iterate through messages
   for (UINT iMessage = 0; findListObjectByIndex(pErrorStack, iMessage, (LPARAM&)pMessage); iMessage++)
      // Add duplicate message
      pushErrorStack(pNewStack, duplicateErrorMessage(pMessage));

   // Return duplicate
   return pNewStack;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                      HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : hasErrors
// Description     : Determines whether an ErrorStack is not empty
// 
// CONST ERROR_STACK*  ErrorStack   : [in] ErrorStack to test
// 
// Return Value   : TRUE/FALSE
// 
BearScriptAPI
BOOL   hasErrors(CONST ERROR_STACK*  pErrorStack)
{
   return getStackItemCount(pErrorStack) > 0;
}


/// Function name  : identifyErrorTypeString
// Description     : Retrieve the string version of an error type
// 
// CONST ERROR_TYPE  eType : [in] Error type
// 
// Return Value   : String equivilent
// 
BearScriptAPI
CONST TCHAR*  identifyErrorTypeString(CONST ERROR_TYPE  eType)
{
   CONST TCHAR*  szOutput;

   // Examine error type
   switch (eType)
   {
   case ET_ERROR:       szOutput = TEXT("Error");        break;
   case ET_WARNING:     szOutput = TEXT("Warning");      break;
   case ET_INFORMATION: szOutput = TEXT("Information");  break;
   default:             szOutput = TEXT("Corrupt");      break;
   }

   // Return string
   return szOutput;
}

/// Function name  : identifyGameVersionIconID
// Description     : Identifies the resource ID of the icon matching the game version
// 
// CONST GAME_VERSION  eGameVersion : [in] GameVersion
// 
// Return Value   : Resource ID of the game version icon
// 
BearScriptAPI
CONST TCHAR*  identifyErrorTypeIconID(CONST ERROR_TYPE  eType)
{
   CONST TCHAR*  szOutput;

   switch (eType)
   {
   case ET_ERROR:       szOutput = TEXT("ERROR_ICON");        break;
   case ET_WARNING:     szOutput = TEXT("WARNING_ICON");      break;
   case ET_INFORMATION: szOutput = TEXT("INFORMATION_ICON");  break;
   }

   return szOutput;
}

/// Function name  : isError
// Description     : Ensures an error of severity ET_ERROR exists
// 
// CONST ERROR_STACK*  pErrorStack : [in] ErrorStack to examine
// 
// Return Value   : FALSE if error is NULL or severity is not ET_ERROR, otherwise TRUE
// 
BearScriptAPI
BOOL   isError(CONST ERROR_STACK*  pErrorStack)
{
   return pErrorStack AND (pErrorStack->eType == ET_ERROR);
}


/// Function name  : setErrorType
// Description     : Sets the severity of an error
// 
// ERROR_STACK*      pErrorStack : [in] ErrorStack to adjust
// CONST ERROR_TYPE  eType       : [in] New type
// 
BearScriptAPI
VOID  setErrorType(ERROR_STACK*  pErrorStack, CONST ERROR_TYPE  eType)
{
   pErrorStack->eType = eType;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                  FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : attachTextToError
// Description     : Attaches plain text to the current error of an error queue
// 
// ERROR_STACK*  pError : [in] Error to append
// CONST TCHAR*  szText : [in] Text to be appended
// 
BearScriptAPI
VOID   attachTextToError(ERROR_STACK*  pError, CONST TCHAR*  szText)
{
   // [ERROR] "Appending relevant text: '%s'"
   enhanceError(pError, ERROR_ID(IDS_ERROR_APPEND_TEXT), szText);
}


/// Function name  : attachXMLToError
// Description     : Extracts 64 characters either side of a character and adds it an 'XML Attachment' message
// 
// ERROR_STACK*  pError    : [in] Error to attach to
// CONST TCHAR*  szXML     : [in] Buffer containing the XML to attach
// CONST UINT     iPosition : [in] Character index of this XML snippet within the string it came from
// 
BearScriptAPI
VOID   attachXMLToError(ERROR_STACK*  pError, CONST TCHAR*  szXML, CONST UINT  iPosition)
{
   TCHAR*         szSnippet;
   CONST TCHAR*   szStart;
   
   // Create snippet string
   szSnippet = utilCreateEmptyString(64);

   // Extract the XML from up to 16 characters before the position of the error, if possible
   szStart = &szXML[-min((INT)iPosition, 16)];

   // Extract up to 64 characters from the start of the extract
   StringCchCopyN(szSnippet, 64, szStart, 64); 

   // [ERROR] "Appending relevant XML source code:\r\n\r\n'...%s...'"
   enhanceError(pError, ERROR_ID(IDS_ERROR_APPEND_XML), szSnippet);

   // Cleanup
   utilDeleteString(szSnippet);
}


/// Function name  : displayOperationError
// Description     : Handle an error according to the user's 'error handling' preference. This may involve requesting
//                    how to proceed from the user.
// 
// HWND                hParentWnd  : [in] Parent window that will actually own the error dialog
// CONST ERROR_STACK*  pError      : [in] Error stack containing the error to display
// CONST UINT          iMessageID  : [in] Resource ID of the question message to display to the user
// CONST TCHAR*        szMessageID : [in] Question message ID as string
//     ...             ...         : [in][optional] Variable arguments for the question message
// 
// Return Value   : EH_ABORT  - The user chose to abort (or their preferences indicate they always wish to abort)
//                  EH_IGNORE - The user chose to ignore the error (or their preferences indicate they always wish to ignore errors)
// 
BearScriptAPI
ERROR_HANDLING   displayOperationError(HWND  hParentWnd, ERROR_STACK*  pError, CONST UINT  iMessageID, CONST TCHAR*  szMessageID, ...)
{
   ERROR_HANDLING   eResult;      // Operation result
   TCHAR           *szQuestion,   // Message box question message
                   *szFormat;     // Variable argument formatting string
   ERROR_MESSAGE   *pQuestionMessage;
   va_list          pArguments;   // Variable argument pointer

#ifdef _TESTING
   // [TESTING] Always return 'IGNORE' so that TestCases can continue as far as possible and the handling is uniform.
   return EH_IGNORE;
#endif

   // [CHECK] Never prompt the user over warnings or 'information'
   if (pError->eType != ET_ERROR)
      return EH_IGNORE;

   // Examine user's error handling preference
   switch (getAppPreferences()->eErrorHandling)
   {
   /// [ABORT]  Always abort on errors without informing the user
   /// [IGNORE] Always ignore errors without informing the user
   case EH_ABORT:   
   case EH_IGNORE:  
      eResult = getAppPreferences()->eErrorHandling;   
      break;

   /// [PROMPT] Ask the user for the desired action
   case EH_PROMPT:
      // Prepare
      szQuestion = utilCreateEmptyString(512);
      szFormat   = loadString(iMessageID, 512);
      pArguments = utilGetFirstVariableArgument(&iMessageID);

      // Temporarily add the question message to the Error
      pQuestionMessage = createErrorMessageV(iMessageID, szMessageID, pArguments);
      pushErrorStack(pError, pQuestionMessage);
      
      // Request parent window to display an error dialog.
      eResult = (ERROR_HANDLING)SendMessage(hParentWnd, UM_PROCESSING_ERROR, MB_YESNO, (LPARAM)pError);

      // [CHECK] Result should be abort or ignore
      ASSERT(eResult == EH_ABORT || eResult == EH_IGNORE);

      // Remove the question message from the error
      popErrorStack(pError);
      
      // Cleanup
      deleteErrorMessage(pQuestionMessage);
      utilDeleteStrings(szFormat, szQuestion);
      break;
   }

   // Always return either IGNORE or ABORT
   return eResult;
}

/// Function name  : enhanceError
// Description     : Push a new error message onto an error stack
//
// ERROR_STACK* pErrorStack : [in] ErrorStack to add new message to
// CONST UINT   iErrorCode  : [in] Error code (message ID)
// CONST TCHAR* szErrorCode : [in] Error code as string
//
// CONST ....   ...         : [in] Variable printf arguments
//  
BearScriptAPI
VOID   enhanceError(ERROR_STACK* pErrorStack, CONST UINT iErrorCode, CONST TCHAR*  szErrorCode, ...)
{
   ERROR_MESSAGE* pNewMessage;
   va_list        pArguments;

   // Prepare
   pArguments = utilGetFirstVariableArgument(&szErrorCode);

   // Assemble new error message
   pNewMessage = createErrorMessageV(iErrorCode, szErrorCode, pArguments);

   // Add to the error stack
   pushErrorStack(pErrorStack, pNewMessage);
}


/// Function name  : enhanceWarning
// Description     : Push a new error message onto an error stack and convert it to a warning
//
// ERROR_STACK* pErrorStack : [in] ErrorStack to convert and add new message to
// CONST UINT   iErrorCode  : [in] Error code (message ID)
// CONST TCHAR* szErrorCode : [in] Error code as string
//
// CONST ....   ...         : [in] Variable printf arguments
//  
BearScriptAPI
VOID   enhanceWarning(ERROR_STACK*  pErrorStack, CONST UINT  iErrorCode, CONST TCHAR*  szErrorCode, ...)
{
   ERROR_MESSAGE*  pNewMessage;  // New message to assemble
   va_list         pArguments;   // Variable argument pointer

   // Prepare
   pArguments = utilGetFirstVariableArgument(&szErrorCode);

   // Assemble new error message
   pNewMessage = createErrorMessageV(iErrorCode, szErrorCode, pArguments);

   // Add to the error stack
   setErrorType(pErrorStack, ET_WARNING);
   pushErrorStack(pErrorStack, pNewMessage);
}


/// Function name  : generateDualError
// Description     : Creates a new error containing a 'location' and 'explanation' error message
//
/// Following arguments designed be used with HERE(...) macros
//    CONST UINT     iErrorCode    : [in] Error code
//    CONST TCHAR*   szErrorCode   : [in] String version of above
//    CONST TCHAR*   szFunction    : [in] Name of function where the error occurred
//    CONST TCHAR*   szFile        : [in] Name of file where the error occurred
//    CONST UINT     iLine         : [in] Line number where error occurred
/// Manually specified:
//    ...            ....          : [in] Custom arguments
//
// Return Type : New ErrorStack, you are responsible for destroying it
// 
BearScriptAPI   
ERROR_STACK*   generateDualError(CONST UINT  iErrorCode, CONST TCHAR*  szErrorCode, CONST TCHAR*  szFunction, CONST TCHAR*  szFile, CONST UINT  iLine, ...)
{
   va_list  pArguments;    // Variable arguments helper
   
   // Prepare
   pArguments = utilGetFirstVariableArgument(&iLine);

   /// Return new error
   return createErrorStackV(iErrorCode, szErrorCode, szFunction, szFile, iLine, pArguments);
}


/// Function name  : generateDualInformation
// Description     : Creates a new error containing a 'location' and 'explanation' error message
//
/// Following arguments designed be used with HERE(...) macros
//    CONST UINT     iErrorCode    : [in] Error code
//    CONST TCHAR*   szErrorCode   : [in] String version of above
//    CONST TCHAR*   szFunction    : [in] Name of function where the error occurred
//    CONST TCHAR*   szFile        : [in] Name of file where the error occurred
//    CONST UINT     iLine         : [in] Line number where error occurred
/// Manually specified:
//    ...            ....          : [in] Custom arguments
//
// Return Type : New ErrorStack, you are responsible for destroying it
// 
BearScriptAPI   
ERROR_STACK*   generateDualInformation(CONST UINT  iErrorCode, CONST TCHAR*  szErrorCode, CONST TCHAR*  szFunction, CONST TCHAR*  szFile, CONST UINT  iLine, ...)
{
   ERROR_STACK*  pError;        // Error being created
   va_list       pArguments;    // Variable arguments helper
   
   // Prepare
   pArguments = utilGetFirstVariableArgument(&iLine);

   /// Create 'information' error
   pError = createErrorStackV(iErrorCode, szErrorCode, szFunction, szFile, iLine, pArguments);
   setErrorType(pError, ET_INFORMATION);

   // Return error
   return pError;
}


/// Function name  : generateDualWarning
// Description     : Creates a new warning containing a 'location' and 'explanation' error message
//
/// Following arguments designed be used with HERE(...) macros
//    CONST UINT     iErrorCode    : [in] Error code
//    CONST TCHAR*   szErrorCode   : [in] String version of above
//    CONST TCHAR*   szFunction    : [in] Name of function where the error occurred
//    CONST TCHAR*   szFile        : [in] Name of file where the error occurred
//    CONST UINT     iLine         : [in] Line number where error occurred
/// Manually specified:
//    ...            ....          : [in] Custom arguments
//
// Return Type : New ErrorStack, you are responsible for destroying it
// 
BearScriptAPI   
ERROR_STACK*   generateDualWarning(CONST UINT  iErrorCode, CONST TCHAR*  szErrorCode, CONST TCHAR*  szFunction, CONST TCHAR*  szFile, CONST UINT  iLine, ...)
{
   ERROR_STACK*  pError;        // Error being created
   va_list       pArguments;    // Variable arguments helper
   
   // Prepare
   pArguments = utilGetFirstVariableArgument(&iLine);

   /// Create 'information' error
   pError = createErrorStackV(iErrorCode, szErrorCode, szFunction, szFile, iLine, pArguments);
   setErrorType(pError, ET_WARNING);

   // Return error
   return pError;
}


/// Function name  : generateExceptionError
// Description     : Generates the appropriate error from a system exception
// 
// CONST EXCEPTION_POINTERS*  pExceptionData : [in] Exception data returned by GetExceptionInformation()
// ERROR_QUEUE*               pErrorQueue    : [in/out] ErrorQueue to add message to
// 
// Return Value   : EXCEPTION_EXECUTE_HANDLER
// 
BearScriptAPI
UINT    generateExceptionError(CONST EXCEPTION_POINTERS*  pExceptionData, ERROR_STACK*  &pError)
{
   FUNCTION_CALL      oCaller = { TEXT("Unknown"), TEXT("Unknown"), NULL },
                     *pCaller;                                         // Last call
   TCHAR             *szCallStack = utilCreateEmptyString(2048);       // CallStack as text
   LIST              *pCallStack;                                      // Callstack as list
   EXCEPTION_RECORD  *pException;                                      // Convenience pointer
   EXCEPTION_STRING  *pAssertion;                                      // Debug assertion that caused the error, if any
   
   // Prepare
   pException = pExceptionData->ExceptionRecord;
   pCallStack = generateStackTrace(*pExceptionData->ContextRecord);
   
   // Attempt to lookup last function
   /*if (findListObjectByIndex(pCallStack, 1, (LPARAM&)pCaller))
      oCaller = *pCaller;*/

   /// Attempt to lookup last function
   for (UINT  iIndex = 0; findListObjectByIndex(pCallStack, iIndex, (LPARAM&)pCaller); iIndex++)
   {
      if (utilCompareString(pCaller->szFunction, "generateExceptionError") OR utilCompareString(pCaller->szFunction, "generateQueuedExceptionError"))
         continue;

      oCaller = *pCaller;
      break;
   }

   // Examine exception
   switch (pException->ExceptionCode)
   {
   /// [ACCESS-VIOLATION] "An access violation has occurred in %s on line %u of %s"
   case EXCEPTION_ACCESS_VIOLATION:
      pError = generateDualError(IDS_UNHANDLED_ACCESS_VIOLATION, L"IDS_UNHANDLED_ACCESS_VIOLATION", oCaller.szFunction, oCaller.szFileName, oCaller.iLine, oCaller.szFunction, oCaller.iLine, oCaller.szFileName);
      setAppError(AE_ACCESS_VIOLATION);
      break;
   
   /// [ASSERTION] "The debugging check '%s' has failed in %s(..) on line %u of %s"
   case EXCEPTION_BREAKPOINT:
      // Extract string containing assertion
      pAssertion = (EXCEPTION_STRING*)pException->ExceptionInformation;

      // Generate error and display assertion
      pError = generateDualError(IDS_UNHANDLED_DEBUG_ASSERTION, L"IDS_UNHANDLED_DEBUG_ASSERTION", pAssertion->szFunction, pAssertion->szFileName, pAssertion->iLine, pAssertion->szAssertion, pAssertion->szFunction, pAssertion->iLine, pAssertion->szFileName);
      setAppError(AE_DEBUG_ASSERTION);

      // Cleanup
      utilDeleteExceptionString(pAssertion);
      break;

   /// [OTHER] "An unhandled exception with code %#x has occurred in %s(..) on line %u of %s"
   default:
      pError = generateDualError(IDS_UNHANDLED_EXCEPTION, L"IDS_UNHANDLED_EXCEPTION", oCaller.szFunction, oCaller.szFileName, oCaller.iLine, pException->ExceptionCode, oCaller.szFunction, oCaller.iLine, oCaller.szFileName);
      setAppError(AE_EXCEPTION);
      break;
   }

   // Prepare
   StringCchCopy(szCallStack, 2048, TEXT("\r\nCall Stack:"));

   // Flatten call stack to text
   for (UINT  iIndex = 0; findListObjectByIndex(pCallStack, iIndex, (LPARAM&)pCaller); iIndex++)
   {
      if (!utilCompareString(pCaller->szFunction, "generateExceptionError") AND !utilCompareString(pCaller->szFunction, "generateQueuedExceptionError"))
         utilStringCchCatf(szCallStack, 2048, TEXT("\r\n%s(..) on line %u of %s"), pCaller->szFunction, pCaller->iLine, pCaller->szFileName);
   }

   // Set display message
   generateOutputTextFromError(pError);
   attachTextToError(pError, szCallStack);
   
   // Cleanup and return 'Execute Handler'
   deleteList(pCallStack);
   utilDeleteString(szCallStack);
   return EXCEPTION_EXECUTE_HANDLER;
}


/// Function name  : generateFlattenedErrorStackText
// Description     : 'Flatten' an ErrorStack into an output buffer, with error messages preceeding attachments.
// 
// CONST ERROR_STACK*  pErrorStack   : [in]  ErrorStack to flatten
// 
// Return Value   : New string containing all messages and attachments in an ErrorStack
// 
BearScriptAPI
TCHAR*   generateFlattenedErrorStackText(CONST ERROR_STACK*  pErrorStack)
{
   CONST ERROR_MESSAGE  *pMessage;           // ErrorMessage being processed
   TEXT_STREAM          *pTextStream;        // 
   TCHAR                *szOutput;           // Output

   // Prepare
   pTextStream = createTextStream(4 * ERROR_LENGTH);

   // Append heading
   //appendStringToTextStreamf(pTextStream, /*TEXT(SMALL_PARTITION) TEXT("\r\n")*/ TEXT("%s:\r\n"), identifyErrorTypeString(pErrorStack->eType));

   /// Iterate through error messages
   for (UINT  iIndex = 0; findErrorMessageByLogicalIndex(pErrorStack, iIndex, MT_MESSAGE, pMessage); iIndex++)
      // [ERROR] Output Error ID and message
      appendStringToTextStreamf(pTextStream, TEXT("%s: %s\r\n"), pMessage->szID, pMessage->szMessage);

   /// Iterate through attachments
   for (UINT  iIndex = 0; findErrorMessageByLogicalIndex(pErrorStack, iIndex, MT_ATTACHMENT, pMessage); iIndex++)
      // [ATTACHMENT] Output attachment text
      appendStringToTextStreamf(pTextStream, TEXT("Attachment #%u: %s\r\n"), iIndex + 1, pMessage->szMessage);

   // Append heading
   //appendStringToTextStreamf(pTextStream, TEXT(SMALL_PARTITION)); // TEXT("\r\n"));
   
   /// Generate output
   szOutput = utilDuplicateString(pTextStream->szBuffer, pTextStream->iBufferUsed);

   // Cleanup and Return output
   deleteTextStream(pTextStream);
   return szOutput;
}


/// Function name  : generateOutputTextFromError
// Description     : Generate an 'Output Text' attachment from the last error added to an ErrorStack
// 
// ERROR_STACK*  pError : [in/out] ErrorStack to generate attachment from and to add the new attachment to
// 
BearScriptAPI 
VOID  generateOutputTextFromError(ERROR_STACK*  pError)
{
   ERROR_MESSAGE*  pNewMessage;

   // Create new message by duplicating the last error in the stack
   pNewMessage = duplicateErrorMessage(topErrorStack(pError));
   
   /// Alter ID to 'output text attachment'
   pNewMessage->iID = IDS_ERROR_APPEND_OUTPUT;
   utilReplaceString(pNewMessage->szID, TEXT("IDS_ERROR_APPEND_OUTPUT"));

   // Add new message to the stack
   pushErrorStack(pError, pNewMessage);
}



/// Function name  : generateSafeDualError
// Description     : Only creates a new error containing a 'location' and 'explanation' error message if the error does not already exist
//
//    ERROR_STACK*   pError        : [in][optional] Existing error
//
/// Following arguments designed to be used with HERE(...) macro
//    CONST UINT     iErrorCode    : [in]           Error code
//    CONST TCHAR*   szErrorCode   : [in]           String version of above
//    CONST TCHAR*   szFunction    : [in]           Name of function where the error occurred
//    CONST TCHAR*   szFile        : [in]           Name of file where the error occurred
//    CONST UINT     iLine         : [in]           Line number where error occurred
//
/// Manually specified:
//    ...            ....          : [in]           Custom arguments
//
// Return Type : New ErrorStack, you are responsible for destroying it
// 
BearScriptAPI   
ERROR_STACK*   generateSafeDualError(ERROR_STACK*  pError, CONST UINT  iErrorCode, CONST TCHAR*  szErrorCode, CONST TCHAR*  szFunction, CONST TCHAR*  szFile, CONST UINT  iLine, ...)
{
   ERROR_MESSAGE* pMessage;      // Message to follow the 'location' message
   va_list        pArguments;    // Variable arguments helper
   
   // Prepare
   pArguments = utilGetFirstVariableArgument(&iLine);

   /// Create an error with a 'location' message
   if (pError == NULL)
   {
      pError = createErrorStack();
      enhanceError(pError, ERROR_ID(IDS_ERROR_APPEND_LOCATION), szFunction, szErrorCode, iErrorCode, szFile, iLine);   
   }

   // Add 'enhancement' message to the error
   pMessage = createErrorMessageV(iErrorCode, szErrorCode, pArguments);
   pushErrorStack(pError, pMessage);

   // Cleanup and return
   va_end(pArguments);
   return pError;
}

