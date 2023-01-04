//
// Error Queues.cpp  -- Represents a queue of errors (represented as stacks) resulting from an operation that may
//                        suffer multiple errors in a sequence - like loading/saving files or game data.
// 
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createErrorQueue
// Description     : Create an empty ErrorQueue
//
// Return Value   : New ErrorQueue, you are responsible for destroying it
// 
BearScriptAPI
ERROR_QUEUE*  createErrorQueue()
{
   ERROR_QUEUE*  pQueue = utilCreateEmptyObject(ERROR_QUEUE);

   // Store deletion function
   pQueue->pfnDeleteItem = destructorErrorStack;
   pQueue->bLiveReport   = FALSE;

   // Return new queue
   return pQueue;
}


/// Function name  : duplicateErrorQueue
// Description     : Duplicate an ErrorQueue and the errors within it
// 
// CONST ERROR_QUEUE*  pErrorQueue   : [in] ErrorQueue to copy
// 
// Return Value   : New ErrorQueue, you are responsible for destroying it
// 
BearScriptAPI
ERROR_QUEUE*  duplicateErrorQueue(CONST ERROR_QUEUE*  pErrorQueue)
{
   ERROR_QUEUE*  pCopy;    // Object being created
   ERROR_STACK*  pError;   // ErrorQueue item

   // Create empty object
   pCopy = createErrorQueue();

   // Copy error stacks
   for (UINT i = 0; i < getQueueItemCount(pErrorQueue); i++)
   {
      // Extract error stack and add to the queue
      pError = getErrorQueueItem(pErrorQueue, i);
      pushErrorQueue(pCopy, duplicateErrorStack(pError));
   }

   // Return copy
   return pCopy;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : findErrorStackByIndex
// Description     : Searches an ErrorQueue for the stack at a specified index
// 
// CONST ERROR_STACK*  pErrorStack : [in]  ErrorStack to search
// CONST UINT          iIndex      : [in]  Zero-based index
// ERROR_MESSAGE*     &pOutput     : [out] ErrorStack if found, otherwise NULL
// 
// Return Value   : TRUE if found, otherwise FALSE
// 
BearScriptAPI 
BOOL  findErrorStackByIndex(CONST ERROR_QUEUE*  pErrorQueue, CONST UINT  iIndex, ERROR_STACK*  &pOutput)
{
   return findListObjectByIndex(pErrorQueue, iIndex, (LPARAM&)pOutput);
}


/// Function name  : hasErrors
// Description     : Determines whether an ErrorQueue is not empty
// 
// CONST ERROR_QUEUE*  pErrorQueue   : [in] ErrorQueue to test
// 
// Return Value   : TRUE/FALSE
// 
BearScriptAPI
BOOL   hasErrors(CONST ERROR_QUEUE*  pErrorQueue)
{
   return getQueueItemCount(pErrorQueue) > 0;
}


/// Function name  : identifyErrorQueueType
// Description     : Determines whether an error queue contains any items of type 'Error'
// 
// CONST ERROR_QUEUE*  pErrorQueue : [in] ErrorQueue to query
// 
// Return Value   : ET_ERROR   : At least one item is ET_ERROR
//                  ET_WARNING : The queue is empty, or all items are ET_WARNING/ET_INFORMATION
// 
BearScriptAPI
ERROR_TYPE   identifyErrorQueueType(CONST ERROR_QUEUE*  pErrorQueue)
{
   ERROR_STACK*   pCurrentItem;     // ErrorQueue iterator
   ERROR_TYPE     eOutput;          // Operation result

   // Prepare
   eOutput = ET_WARNING;

   // Iterate through each error in the queue
   for (UINT iIndex = 0; (eOutput == ET_WARNING) AND (pCurrentItem = getErrorQueueItem(pErrorQueue, iIndex)); iIndex++)
      /// [CHECK] Abort search if current item is not a warning
      if (pCurrentItem->eType == ET_ERROR)
         eOutput = ET_ERROR;
   
   // Return result
   return eOutput;
}


/// Function name  : getErrorQueueItem
// Description     : Retrieve but not remove the queue item at the specified position
// 
// CONST ERROR_QUEUE*  pErrorQueue : [in] ErrorQueue to access
// CONST UINT          iIndex      : [in] Zero-based index of the item to retrieve
// 
// Return Value   : Specified queue item if index was valid, otherwise NULL
//
/// THIS FUNCTION IS A DUPLICATE OF findErrorStackByIndex AND SHOULD BE REMOVED
//
BearScriptAPI
ERROR_STACK*  getErrorQueueItem(CONST ERROR_QUEUE*  pErrorQueue, CONST UINT  iIndex)
{
   ERROR_STACK*  pOutput;

   // Lookup specified item
   findListObjectByIndex(pErrorQueue, iIndex, (LPARAM&)pOutput);

   // Return item if found, otherwise NULL
   return pOutput;
}


/// Function name  : pushErrorQueue
// Description     : Adds an error a queue
// 
// ERROR_QUEUE*  pErrorQueue : [in] Queue
// ERROR_STACK*  pError      : [in] Error
// 
BearScriptAPI
VOID  pushErrorQueue(ERROR_QUEUE* pErrorQueue, ERROR_STACK*  pError)
{
   // Add to queue
   pushLastQueueObject(pErrorQueue, (LPARAM)pError);

   // [LIVE REPORT] Print to console
   if (pErrorQueue->bLiveReport)
      consolePrintTopError(pError);
}


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : attachLocationToLastError
// Description     : Push a 'location' message into the 'current' error stack of a given error queue
//
// ERROR_QUEUE*   pErrorQueue   : [out] Error queue containing 'current' error stack
// CONST UINT     iErrorCode    : [in]  Error code
// CONST TCHAR*   szErrorCode   : [in]  The error code above written as a string
// CONST TCHAR*   szFunction    : [in]  Name of function where the error occurred
// CONST TCHAR*   szFile        : [in]  Name of file where the error occurred
// CONST UINT     iLine         : [in]  Line number where error occurred
// 
VOID   attachLocationToLastError(ERROR_QUEUE*  pErrorQueue, CONST UINT  iErrorCode, CONST TCHAR*  szErrorCode, CONST TCHAR*  szFunction, CONST TCHAR*  szFile, CONST UINT  iLine)
{
   // Re-order parameters to fit the location message
   enhanceLastError(pErrorQueue, ERROR_ID(IDS_ERROR_APPEND_LOCATION), szFunction, szErrorCode, iErrorCode, szFile, iLine);   
}


/// Function name  : attachXMLToLastError
// Description     : Extracts 64 characters of XML and attaches it to the current error queue item
// 
// ERROR_QUEUE*  pErrorQueue : [in] Error queue containing 'current' error stack
// CONST TCHAR*  szXML       : [in] Buffer containing XML
// 
VOID   attachXMLToLastError(ERROR_QUEUE*  pErrorQueue, CONST TCHAR*  szXML, CONST UINT  iPosition)
{
   // [ERROR] "Appending relevant XML source code:\r\n\r\n'...%s...'"
   attachXMLToError(lastErrorQueue(pErrorQueue), szXML, iPosition);
}


/// Function name  : attachTextToLastError
// Description     : Attaches plain text to the current error of an error queue
// 
// ERROR_QUEUE*  pErrorQueue : [in] Error queue containing the current error
// CONST TCHAR*  szText      : [in] Text to append
// 
VOID   attachTextToLastError(ERROR_QUEUE*  pErrorQueue, CONST TCHAR*  szText)
{
   // [ERROR] "Appending relevant text: '%s'"
   enhanceLastError(pErrorQueue, ERROR_ID(IDS_ERROR_APPEND_TEXT), szText);
}


/// Function name  : enhanceLastError
// Description     : Push a new error message onto the 'current' error stack in an error queue.
//
// ERROR_QUEUE* pErrorQueue : [in] ERROR_QUEUE who's 'current' error message is to be enhanced
// CONST UINT   iErrorCode  : [in] Error code (message ID)
// CONST TCHAR* szErrorCode : [in] Error code as string
//
// CONST ....   ...         : [in] Variable printf arguments
//
BearScriptAPI   
VOID   enhanceLastError(ERROR_QUEUE*  pErrorQueue, CONST UINT iErrorCode, CONST TCHAR*  szErrorCode, ...)
{
   ERROR_MESSAGE* pNewMessage;
   va_list        pArguments;

   // [CHECK] Ensure queue isn't empty
   ASSERT(hasErrors(pErrorQueue)); 

   // Prepare
   pArguments = utilGetFirstVariableArgument(&szErrorCode);

   // Assemble new error message
   pNewMessage = createErrorMessageV(iErrorCode, szErrorCode, pArguments);

   // Add to the error stack of the 'current' queue item
   pushErrorStack(lastErrorQueue(pErrorQueue), pNewMessage);
}


/// Function name  : enhanceLastWarning
// Description     : Push a new 'warning' error message onto the 'current' error stack in an error queue.
//
// ERROR_QUEUE* pErrorQueue : [in] ERROR_QUEUE who's 'current' error message is to be enhanced
// CONST UINT   iErrorCode  : [in] Error code (message ID)
// CONST TCHAR* szErrorCode : [in] Error code as string
//
// CONST ....   ...         : [in] Variable printf arguments
//
BearScriptAPI   
VOID   enhanceLastWarning(ERROR_QUEUE*  pErrorQueue, CONST UINT iErrorCode, CONST TCHAR*  szErrorCode, ...)
{
   ERROR_MESSAGE* pNewMessage;
   va_list        pArguments;

   // [CHECK] Ensure queue isn't empty
   ASSERT(hasErrors(pErrorQueue)); 

   // Prepare
   pArguments = utilGetFirstVariableArgument(&szErrorCode);

   // Assemble new error message
   pNewMessage = createErrorMessageV(iErrorCode, szErrorCode, pArguments);

   // Add to the top error stack as a 'warning'
   pushErrorStack(lastErrorQueue(pErrorQueue), pNewMessage);
   setErrorType(lastErrorQueue(pErrorQueue), ET_WARNING);
}


/// Function name  : findErrorStackByID
// Description     : Checks whether an ErrorQueue contains a specific error
// 
// CONST ERROR_QUEUE*  pErrorQueue : [in] ErrorQueue to search
// CONST UINT          iErrorID    : [in] ID of the error to search for
// 
// Return Value   : TRUE if found, otherwise FALSE
// 
BearScriptAPI
BOOL   findErrorStackByID(CONST ERROR_QUEUE*  pErrorQueue, CONST UINT  iErrorID)
{
   ERROR_STACK*    pErrorStack;
   ERROR_MESSAGE*  pMessage;

   // Prepare
   pErrorStack = NULL;
   pMessage    = NULL;

   /// Search through all errors
   for (LIST_ITEM*  pIterator1 = getListHead(pErrorQueue); pErrorStack = extractListItemPointer(pIterator1, ERROR_STACK); pIterator1 = pIterator1->pNext)
   {
      /// Search through all messages
      for (LIST_ITEM*  pIterator2 = getListHead(pErrorStack); pMessage = extractListItemPointer(pIterator2, ERROR_MESSAGE); pIterator2 = pIterator2->pNext)
      {
         if (pMessage->iID == iErrorID)
            // [FOUND] Stop searching
            break;
      }

      // [CHECK] Has message been found?
      if (pMessage AND (pMessage->iID == iErrorID))
         break;
   }

   // Return TRUE if found
   return pMessage AND (pMessage->iID == iErrorID);
}

/// Function name  : generateQueuedError
// Description     : Adds a new error containing a 'location' message to an error queue
//
// ERROR_QUEUE*   pErrorQueue   : [in/out] Error queue to add new error to
// CONST UINT     iErrorCode    : [in]     Error code
// CONST TCHAR*   szErrorCode   : [in]     String version of above
// CONST TCHAR*   szFunction    : [in]     Name of function where the error occurred
// CONST TCHAR*   szFile        : [in]     Name of file where the error occurred
// CONST UINT     iLine         : [in]     Line number where error occurred
// ...            ...           : [in]     Arguments
// 
BearScriptAPI   
VOID    generateQueuedError(ERROR_QUEUE*  pErrorQueue, CONST UINT  iErrorCode, CONST TCHAR*  szErrorCode, CONST TCHAR*  szFunction, CONST TCHAR*  szFile, CONST UINT  iLine, ...)
{
   ERROR_STACK*  pError;        // Error being created
   va_list       pArguments;    // Variable arguments helper
   
   // Prepare
   pArguments = utilGetFirstVariableArgument(&iLine);

   /// Create error
   pError = createErrorStackV(iErrorCode, szErrorCode, szFunction, szFile, iLine, pArguments);

   /// Add to stack
   pushErrorQueue(pErrorQueue, pError);
}



/// Function name  : generateAttachmentError
// Description     : Flattens a Queue into an attachment, adds it to an error, inserts the error into an output queue
// 
// ERROR_QUEUE*        pQueue      : [in/out] Queue to push error into
// const ERROR_QUEUE*  pAttachment : [in] Attachment for error
// ERROR_STACK*        pError      : [in] Error
// 
VOID  generateAttachmentError(ERROR_QUEUE*  pOutput, const ERROR_QUEUE*  pAttachments, ERROR_STACK*  pError)
{
   TCHAR*        szAttachment = utilCreateEmptyString(ERROR_LENGTH);
   ERROR_STACK*  pIterator;

   // Flatten all messages into a single string
   for (UINT  iIndex = 0, iLength = 0; findErrorStackByIndex(pAttachments, iIndex, pIterator); iIndex++)
   {
      ERROR_MESSAGE*  pAttachment = topErrorStack(pIterator);

      // Measure message
      iLength += lstrlen(pAttachment->szMessage);

      // [BUFFER EXCEEDED] Commit attachment + Start new one
      if (iLength >= ERROR_LENGTH)
      {
         attachTextToError(pError, szAttachment);
         szAttachment[0] = NULL;
         // Calc new attachment length
         iLength = lstrlen(pAttachment->szMessage);
      }

      // Append message 
      utilStringCchCatf(szAttachment, ERROR_LENGTH, iIndex == 0 ? TEXT("%s") : TEXT("\r\n%s"), pAttachment->szMessage);
   }

   // Commit final attachment + Error
   attachTextToError(pError, szAttachment);
   pushErrorQueue(pOutput, pError);

   // Cleanup
   utilDeleteString(szAttachment);
}



/// Function name  : generateFlattenedErrorStackText
// Description     : 'Flatten' an ErrorStack into an output buffer, with error messages preceeding attachments.
// 
// CONST ERROR_STACK*  pErrorStack   : [in]  ErrorStack to flatten
// 
// Return Value   : New string containing all messages and attachments in an ErrorStack
// 
BearScriptAPI
TCHAR*   generateFlattenedErrorQueueText(CONST ERROR_QUEUE*  pErrorQueue)
{
   CONST ERROR_MESSAGE  *pMessage;           // ErrorMessage being processed
   TEXT_STREAM          *pTextStream;        // 
   ERROR_STACK          *pErrorStack;        // 
   TCHAR                *szOutput;           // Output

   // Prepare
   pTextStream = createTextStream(4 * ERROR_LENGTH);

   /// Iterate through stacks
   for (UINT  iStackIndex = 0; findErrorStackByIndex(pErrorQueue, iStackIndex, pErrorStack); iStackIndex++)
   {
      // [CHECK] Skip information errors
      if (pErrorStack->eType == ET_INFORMATION)
         continue;

      // Append heading
      appendStringToTextStreamf(pTextStream, TEXT(SMALL_PARTITION) TEXT("\r\n") TEXT("An %s error has occurred:\r\n"), identifyErrorTypeString(pErrorStack->eType));

      /// Iterate through error messages
      for (UINT  iIndex = 0; findErrorMessageByLogicalIndex(pErrorStack, iIndex, MT_MESSAGE, pMessage); iIndex++)
         // [ERROR] Output Error ID and message
         appendStringToTextStreamf(pTextStream, TEXT("Error %s (%d): %s\r\n"), pMessage->szID, pMessage->iID, pMessage->szMessage);

      /// Iterate through attachments
      for (UINT  iIndex = 0; findErrorMessageByLogicalIndex(pErrorStack, iIndex, MT_ATTACHMENT, pMessage); iIndex++)
         // [ATTACHMENT] Output attachment text
         appendStringToTextStreamf(pTextStream, TEXT("Attachment #%u: %s\r\n"), iIndex + 1, pMessage->szMessage);

      // Append footer
      // appendStringToTextStreamf(pTextStream, TEXT(SMALL_PARTITION) TEXT("\r\n"));
   }

   /// Generate output
   szOutput = utilDuplicateString(pTextStream->szBuffer, pTextStream->iBufferUsed);

   // Cleanup and Return output
   deleteTextStream(pTextStream);
   return szOutput;
}

/// Function name  : generateQueuedInformation
// Description     : Adds a new 'information' error containing a 'location' message to an error queue
//
// ERROR_QUEUE*   pErrorQueue   : [in/out] Error queue to add new error to
// CONST UINT     iErrorCode    : [in]     Error code
// CONST TCHAR*   szErrorCode   : [in]     String version of above
// CONST TCHAR*   szFunction    : [in]     Name of function where the error occurred
// CONST TCHAR*   szFile        : [in]     Name of file where the error occurred
// CONST UINT     iLine         : [in]     Line number where error occurred
// ...            ...           : [in]     Arguments
// 
BearScriptAPI   
VOID    generateQueuedInformation(ERROR_QUEUE*  pErrorQueue, CONST UINT  iErrorCode, CONST TCHAR*  szErrorCode, CONST TCHAR*  szFunction, CONST TCHAR*  szFile, CONST UINT  iLine, ...)
{
   ERROR_STACK*  pError;        // Error being created
   va_list       pArguments;    // Variable arguments helper
   
   // Prepare
   pArguments = utilGetFirstVariableArgument(&iLine);

   /// Create 'information' error
   pError = createErrorStackV(iErrorCode, szErrorCode, szFunction, szFile, iLine, pArguments);
   setErrorType(pError, ET_INFORMATION);

   /// Add to stack
   pushErrorQueue(pErrorQueue, pError);
}


/// Function name  : generateQueuedWarning
// Description     : Adds a new 'warning' error containing a 'location' message to an error queue
//
// ERROR_QUEUE*   pErrorQueue   : [in/out] Error queue to add new error to
// CONST UINT     iErrorCode    : [in]     Error code
// CONST TCHAR*   szErrorCode   : [in]     String version of above
// CONST TCHAR*   szFunction    : [in]     Name of function where the error occurred
// CONST TCHAR*   szFile        : [in]     Name of file where the error occurred
// CONST UINT     iLine         : [in]     Line number where error occurred
// ...            ...           : [in]     Arguments
// 
BearScriptAPI   
VOID    generateQueuedWarning(ERROR_QUEUE*  pErrorQueue, CONST UINT  iErrorCode, CONST TCHAR*  szErrorCode, CONST TCHAR*  szFunction, CONST TCHAR*  szFile, CONST UINT  iLine, ...)
{
   ERROR_STACK*  pError;        // Error being created
   va_list       pArguments;    // Variable arguments helper
   
   // Prepare
   pArguments = utilGetFirstVariableArgument(&iLine);

   /// Create 'warning' error
   pError = createErrorStackV(iErrorCode, szErrorCode, szFunction, szFile, iLine, pArguments);
   setErrorType(pError, ET_WARNING);

   /// Add to stack
   pushErrorQueue(pErrorQueue, pError);
}

/// Function name  : generateOutputTextFromError
// Description     : Generate an 'Output Text' attachment from the last error in an error queue
// 
// ERROR_QUEUE*  pErrorQueue : [in/out] ErrorQueue containing Error to generate attachment from
// 
BearScriptAPI 
VOID    generateOutputTextFromLastError(ERROR_QUEUE*  pErrorQueue)
{
   if (hasErrors(pErrorQueue))
      generateOutputTextFromError(lastErrorQueue(pErrorQueue));
}


/// Function name  : generateQueuedExceptionError
// Description     : Generates the appropriate error from a system exception
// 
// CONST EXCEPTION_POINTERS*  pExceptionData : [in] Exception data returned by GetExceptionInformation()
// ERROR_QUEUE*               pErrorQueue    : [in/out] ErrorQueue to add message to
// 
// Return Value   : EXCEPTION_EXECUTE_HANDLER
// 
BearScriptAPI
UINT    generateQueuedExceptionError(CONST EXCEPTION_POINTERS*  pExceptionData, ERROR_QUEUE*  pErrorQueue)
{
   ERROR_STACK*   pError;

   // Generate ErrorStack from exception and add to output queue
   generateExceptionError(pExceptionData, pError);
   pushErrorQueue(pErrorQueue, pError);

   // Return handling
   return EXCEPTION_EXECUTE_HANDLER;
}


/// Function name  : pushCommandAndOutputQueues
// Description     : Create an output dialog attachment from the current message in an ErrorStack and then add
//                    it to the error queue of the output dialog and the COMMAND being translated. Optionally set 
//                    the error type.
// 
// ERROR_STACK*      pError        : [in/out]        ErrorStack to be operated upon
// ERROR_QUEUE*      pOutputQueue  : [in/out]        Output dialog error queue
// ERROR_QUEUE*      pCommandQueue : [in/out]        Error queue of the COMMAND being translated
// CONST ERROR_TYPE  eType         : [in] [optional] Whether the error should be a warning or not
// 
VOID  pushCommandAndOutputQueues(ERROR_STACK*  pError, ERROR_QUEUE*  pOutputQueue, ERROR_QUEUE*  pCommandQueue, CONST ERROR_TYPE  eType)
{
   // Generate output dialog attachment from error
   generateOutputTextFromError(pError);
   // Set error type
   setErrorType(pError, eType);
   // Add to output error queue
   pushErrorQueue(pOutputQueue, pError);
   // Add (a duplicate) to COMMAND's queue
   pushErrorQueue(pCommandQueue, duplicateErrorStack(pError));
}


/// Function name  : pushCommandAndCodeEditErrorQueue
// Description     : Adds an ErrorStack to a COMMAND and then informs a CodeEdit
// 
// COMMAND*      pCommand     : [in/out] COMMAND to contain the error
// HWND          hCodeEdit  : [in/out] CodeEdit to display the error
// ERROR_STACK*  pError       : [in]     Error to add
// 
VOID  pushCommandAndCodeEditErrorQueue(COMMAND*  pCommand, HWND  hCodeEdit, ERROR_STACK*  pError)
{
   // Add error to COMMAND's error queue
   pushErrorQueue(pCommand->pErrorQueue, pError);

   // Inform CodeEdit
   setCodeEditLineError(hCodeEdit, pCommand);
}

