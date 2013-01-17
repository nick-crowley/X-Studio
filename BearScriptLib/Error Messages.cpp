//
// Error Messages.cpp : Messages held within ErrorStacks
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
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createErrorMessageV
// Description     : Create a new ERROR_MESSAGE with a string loaded from a string resource and customised with 
//                           printf. NOTE: This is only used by appendNewErrorMessage(...) below
// 
// CONST UINT   iErrorCode : [in] Error code (message ID)
// CONST TCHAR* szID       : [in] [optional] Error code as string or NULL for none
// va_list      pArguments : [in] Variable argument list
//
// Returns : New ERROR_MESSAGE, you are responsible for destroying it
// 
ERROR_MESSAGE*   createErrorMessageV(CONST UINT iErrorCode, CONST TCHAR*  szID, va_list  pArguments)
{
   ERROR_MESSAGE*   pNewMessage;      // Output
   TCHAR*           szFormat;         // Temporary formatting string
   
   /// Create object
   pNewMessage = utilCreateEmptyObject(ERROR_MESSAGE);

   // Set properties
   pNewMessage->szMessage = utilCreateEmptyString(ERROR_LENGTH);
   pNewMessage->iID       = iErrorCode;
   pNewMessage->szID      = (szID ? utilDuplicateSimpleString(szID) : NULL);

   // Create formatting string
   szFormat = utilCreateEmptyString(ERROR_LENGTH);

   /// Load string resource from the error code
   switch (pNewMessage->iID)
   {
   // [SPECIAL] - These may be needed before the resource DLL is loaded (or if loading fails)
   case IDS_ERROR_APPEND_LOCATION:
   case IDS_INIT_RESOURCE_DLL_FAILED:
      LoadString(getLibraryInstance(), pNewMessage->iID, szFormat, ERROR_LENGTH);         
      break;

   // [STANDARD] - Error strings are stored in the resource library
   default:
      LoadString(getResourceInstance(), pNewMessage->iID, szFormat, ERROR_LENGTH);      
      break;
   }   
      
   /// Format message
   StringCchVPrintf(pNewMessage->szMessage, ERROR_LENGTH, szFormat, pArguments);      // Format into output buffer

   // Cleanup and return
   utilDeleteString(szFormat);
   return pNewMessage;
}


/// Function name  : deleteErrorMessage
// Description     : Deletes an ERROR_MESSAGE
// 
// ERROR_MESSAGE*  &pError : [in] Reference to an ERROR_MESSAGE pointer to delete
//
VOID    deleteErrorMessage(ERROR_MESSAGE*  &pErrorMessage)
{
   // Delete message text
   if (pErrorMessage->szMessage)   // 1st item in the list has no message
      utilDeleteString(pErrorMessage->szMessage);

   // [OPTIONAL] Delete string ID
   utilSafeDeleteString(pErrorMessage->szID);

   // Delete calling object
   utilDeleteObject(pErrorMessage);
}


/// Function name  :  destructorErrorMessage
// Description     : Deletes an ErrorMessage stored in a stack
// 
// LPARAM  pErrorMessage : [in] ErrorMessage
// 
VOID   destructorErrorMessage(LPARAM  pErrorMessage)
{
   deleteErrorMessage((ERROR_MESSAGE*&)pErrorMessage);
}

/// Function name  : duplicateErrorMessage
// Description     : Create a new ErrorMessage containing the same message as an existing ErrorMessage
// 
// CONST ERROR_MESSAGE*  pOriginal   : [in] ErrorMessage to duplicate
// 
// Return Value   : New ErrorMessage object, you are responsible for destroying it
// 
ERROR_MESSAGE*  duplicateErrorMessage(CONST ERROR_MESSAGE*  pOriginal)
{
   ERROR_MESSAGE*  pDuplicate;

   // Create new message
   pDuplicate = utilCreateEmptyObject(ERROR_MESSAGE);

   // Duplicate properties
   pDuplicate->iID       = pOriginal->iID;
   pDuplicate->szID      = (pOriginal->szID ? utilDuplicateSimpleString(pOriginal->szID) : NULL);
   pDuplicate->szMessage = utilDuplicateSimpleString(pOriginal->szMessage);

   // Return new object
   return pDuplicate;
}


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : findErrorMessageByIndex
// Description     : Searches an ErrorStack for the message at a specified index
// 
// CONST ERROR_STACK*  pErrorStack : [in]  ErrorStack to search
// CONST UINT          iIndex      : [in]  Zero-based index
// ERROR_MESSAGE*     &pOutput     : [out] ErrorMessage if found, otherwise NULL
// 
// Return Value   : TRUE if found, otherwise FALSE
// 
BearScriptAPI 
BOOL  findErrorMessageByIndex(CONST ERROR_STACK*  pErrorStack, CONST UINT  iIndex, ERROR_MESSAGE*  &pOutput)
{
   return findListObjectByIndex(pErrorStack, iIndex, (LPARAM&)pOutput);
}


/// Function name  : findErrorMessageByLogicalIndex
// Description     : Find a message or attachment error message within an error stack
// 
// Attachment Index:   -   -   2   -   1   0   -   -   
//                   +---+---+---+---+---+---+---+---+       The attachments are stored in 'reverse' order because
//     Message Type: : M : M : A : M : A : A : M : M :         they're in a stack.  The indicies refer to each attachment,
//                   +---+---+---+---+---+---+---+---+         not the error stack as a whole
// 
//
//    Message Index:   5   4   3   -   2   -   1   0   
//                   +---+---+---+---+---+---+---+---+        The messages are stored in 'reverse' order because
//     Message Type: : M : M : M : A : M : A : M : M :         they're in a stack.  The indicies refer to each message,
//                   +---+---+---+---+---+---+---+---+         not the error stack as a whole
// 
// CONST ERROR_STACK*    pErrorStack   : [in]  ErrorStack to search
// CONST UINT            iLogicalIndex : [in]  Zero-based index of the desired attachment/message
// CONST ERROR_MESSAGE_TYPE    eType         : [in]  Message or Attachment
// CONST ERROR_MESSAGE* &pOutput       : [out] ErrorMessage if found, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BearScriptAPI
BOOL  findErrorMessageByLogicalIndex(CONST ERROR_STACK*  pErrorStack, CONST UINT  iLogicalIndex, CONST ERROR_MESSAGE_TYPE  eType, CONST ERROR_MESSAGE*  &pOutput)
{
   ERROR_MESSAGE*  pMessage;

   // Prepare
   pOutput = NULL;

   // [CHECK] Ensure error stack exists
   if (pErrorStack)
   {
      /// Iterate backwards through the messages in the stack
      for (INT iPhysicalIndex = (getErrorStackCount(pErrorStack) - 1), iMatchCount = 0; !pOutput AND findErrorMessageByIndex(pErrorStack, iPhysicalIndex, pMessage); iPhysicalIndex--)
      {
         // Determine whether item is a message or an attachment
         switch (pMessage->iID)
         {   
            /// [ATTACHMENT] Count if we're searching for attachments
            case IDS_ERROR_APPEND_COMMAND:
            case IDS_ERROR_APPEND_XML:
            case IDS_ERROR_APPEND_TEXT: 
               // [CHECK] Have we reached the logical index yet?
               if (eType == MT_ATTACHMENT AND iMatchCount++ == iLogicalIndex)
                  // [SUCCESS] Return this message
                  pOutput = pMessage;
               break;

            /// [OUTPUT] Never count these as messages or attachments
            case IDS_ERROR_APPEND_OUTPUT:
               break;

            /// [MESSAGE] Count if we're searching for messages
            default:
               // [CHECK] Have we reached the logical index yet?
               if (eType == MT_MESSAGE AND iMatchCount++ == iLogicalIndex)
                  // [SUCCESS] Return this message
                  pOutput = pMessage;
               break;
         }
      }
   }

   // Return TRUE if found, otherwise FALSE
   return (pOutput != NULL);
}


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////




