//
// Call Stacks.cpp : Implmenets basic Thread calls stacks for locating the source of exceptions
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS  / GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION  /  DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createCallStack
// Description     : Creates a new CallStack for a thread
// 
// CONST DWORD  dwThreadID   : [in] Thread ID
// 
// Return Value   : New CallStack, you are responsible for destroying it
// 
CALL_STACK*  createCallStack(CONST DWORD  dwThreadID)
{
   CALL_STACK*  pStack;

   // Create object
   pStack = utilCreateEmptyObject(CALL_STACK);

   // Set properties
   pStack->dwThreadID    = dwThreadID;
   pStack->pfnDeleteItem = destructorSimpleObject;

   // Return object
   return pStack;
}


/// Function name  : createCallStackList
// Description     : Creates data for a CallStackList
// 
// Return Value   : New CallStackList, you are responsible for destroying it
// 
BearScriptAPI
CALL_STACK_LIST*   createCallStackList()
{
   CALL_STACK_LIST*  pCallStackList;

   // Create new object
   pCallStackList = utilCreateEmptyObject(CALL_STACK_LIST);

   // Set properties
   pCallStackList->pfnDeleteItem = destructorCallStack;
   pCallStackList->hMutex        = CreateMutex(NULL, FALSE, NULL);

   // Return new object
   return pCallStackList;
}


/// Function name  : createFunctionCall
// Description     : Creates a new FunctionCall
// 
// CONST TCHAR*  szFileName   : [in] FileName of the function call
///                                       -> Takes ownership of szFileName (Designed to be used with __FILE__ macro)
// CONST TCHAR*  szFunction   : [in] Name of the function
///                                       -> Takes ownership of szFunction (Designed to be used with __FUNCTION__ macro)
// CONST UINT    iLineNumber  : [in] Line number of the function
// 
// Return Value   : New FunctionCall, you are responsible for destroying it
// 
FUNCTION_CALL*  createFunctionCall(CONST TCHAR*  szFileName, CONST TCHAR*  szFunction, CONST UINT  iLineNumber)
{
   FUNCTION_CALL*  pPosition;

   // Create object
   pPosition = utilCreateEmptyObject(FUNCTION_CALL);

   // Set properties
   pPosition->iLine      = iLineNumber;
   pPosition->szFileName = szFileName;
   pPosition->szFunction = szFunction;

   // Return object
   return pPosition;
}


/// Function name  : createFunctionCall2
// Description     : Creates a new FunctionCall
// 
// CONST TCHAR*  szFileName   : [in] FileName of the function call
// CONST TCHAR*  szFunction   : [in] Name of the function
// CONST UINT    iLineNumber  : [in] Line number of the function
// 
// Return Value   : New FunctionCall, you are responsible for destroying it
// 
FUNCTION_CALL*  createFunctionCall2(CONST TCHAR*  szFileName, CONST TCHAR*  szFunction, CONST UINT  iLineNumber)
{
   FUNCTION_CALL*  pPosition;

   // Create object
   pPosition = utilCreateEmptyObject(FUNCTION_CALL);

   // Set properties
   pPosition->iLine      = iLineNumber;
   pPosition->szFileName = utilDuplicateSimpleString(szFileName);
   pPosition->szFunction = utilDuplicateSimpleString(szFunction);

   // Return object
   return pPosition;
}


/// Function name  : deleteCallStackList
// Description     : Destroys a CallStack list
// 
// CALL_STACK_LIST*  &pCallStackList   : [in] CallStackList to destroy
// 
BearScriptAPI
VOID   deleteCallStackList(CALL_STACK_LIST*  &pCallStackList)
{
   // Delete Mutex
   utilCloseHandle(pCallStackList->hMutex);

   // Delete base object
   deleteList((LIST*&)pCallStackList);
}


/// Function name  : destructorCallStack
// Description     : Destroys a CallStack in a list
// 
// LPARAM*  &pCallStack : [in] CallStack to destroy
// 
BearScriptAPI
VOID   destructorCallStack(LPARAM  pCallStack)
{
   deleteCallStack(pCallStack);
}


/// Function name  : destructorFunctionCall
// Description     : Destroys a FunctionCall in a list
// 
// LPARAM*  &pFunctionCall : [in] FunctionCall to destroy
// 
BearScriptAPI
VOID   destructorFunctionCall(LPARAM  pFunctionCall)
{
   FUNCTION_CALL*&  pCall = (FUNCTION_CALL*&)pFunctionCall;

   utilDeleteStrings((TCHAR*&)pCall->szFileName, (TCHAR*&)pCall->szFunction);
   utilDeleteObject(pCall);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

///// Function name  : getCallStackSizeByThread
//// Description     : Determines the number of function calls in the specified CallStack
//// 
//// CONST CALL_STACK_LIST*  pCallStackList : [in] List of CallStacks
//// CONST DWORD             dwThreadID     : [in] ID of the thread to retrieve the CallSTack for
//// 
//// Return Value   : Number of function calls in the specified CallStack if found, otherwise NULL
//// 
//UINT   getCallStackSizeByThread(CONST CALL_STACK_LIST*  pCallStackList, CONST DWORD  dwThreadID)
//{
//   CALL_STACK*  pCallStack;      // Target CallStack
//   UINT         iSize;           // Size of target CallStack
//
//   // Prepare
//   iSize = NULL;
//
//   /// Attempt to lookup CallStack
//   if (findCallStackByThread(pCallStackList, dwThreadID, pCallStack))
//      // [FOUND] Return size
//      iSize = getStackItemCount(pCallStack->pStack);
//
//   // Return size if found, otherwise NULL
//   return iSize;
//}


/// Function name  :  findCallStackByThread
// Description     : Finds a CallStack by ThreadID
// 
// CONST CALL_STACK_LIST*  pCallStackList : [in]  List of CallStacks
// CONST DWORD             dwThreadID     : [in]  ID of the Thread to retrieve the CallStack for
// CALL_STACK*            &pOutput        : [out] CallStack if found, otherwise NULL
// 
// Return Value   : TRUE if found, otherwise FALSE
// 
BOOL   findCallStackByThread(CONST CALL_STACK_LIST*  pCallStackList, CONST DWORD  dwThreadID, CALL_STACK*  &pOutput)
{
   // Prepare
   pOutput = NULL;

   /// Iterate over ExecutionList
   for (LIST_ITEM*  pIterator = getListHead(pCallStackList); !pOutput AND pIterator; pIterator = pIterator->pNext)
   {
      // Examine current item
      if (extractListItemPointer(pIterator, CALL_STACK)->dwThreadID == dwThreadID)
         /// [FOUND] Set result and abort
         pOutput = extractListItemPointer(pIterator, CALL_STACK);
   }

   // Return TRUE if item was found
   return (pOutput != NULL);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : destroyTopFunctionCallByThread
/// Macro Name     : END_TRACKING()
// Description     : Removes and destroys the top function call from the CallStack of the specified thread
// 
// CALL_STACK_LIST*  pCallStackList : [in] List of CallStacks
// CONST DWORD       dwThreadID     : [in] ThreadID of the CallStack create a new FunctionCall upon
// 
BearScriptAPI
VOID  destroyTopFunctionCallByThread(CALL_STACK_LIST*  pCallStackList, CONST DWORD  dwThreadID)
{
   FUNCTION_CALL*  pTopCall;

   /// [LOCK] 
   WaitForSingleObject(pCallStackList->hMutex, INFINITE);

   // Attempt to remove top call  (May not exist if an exception occurred and emptied the stack for error reporting)
   if (pTopCall = popFunctionCallByThread(pCallStackList, dwThreadID))
      // [FOUND] Destroy top function call from stack
      deleteFunctionCall(pTopCall);

   /// [UNLOCK] 
   ReleaseMutex(pCallStackList->hMutex);
}


/// Function name  : generateCallStackStringByThread
/// Macro Name     : FLATTEN_CALL_STACK()
// Description     : Removes and returns the FunctionCall on the top of the CallStack of the specified thread
///                           This function UN-LOCKS the CallStackList
// 
// CALL_STACK_LIST*  pCallStackList : [in] List of CallStacks
// CONST DWORD       dwThreadID     : [in] ID of the thread
// 
// Return Value   : Top FunctionCall if found, NULL if the thread had no callstack
// 
BearScriptAPI
TCHAR*  generateCallStackStringByThread(CALL_STACK_LIST*  pCallStackList, CONST DWORD  dwThreadID)
{
   FUNCTION_CALL*  pTopCall;        // Call on the top of the call stack
   TCHAR*          szOutput;        // Output string

   // Prepare
   szOutput = utilCreateEmptyString(1024);

   // Append header
   utilStringCchCatf(szOutput, 1024, TEXT("Current call stack for thread %#x:"), dwThreadID);

   /// Retrieve next FunctionCall from CallStack
   while (pTopCall = popFunctionCallByThread(pCallStackList, dwThreadID))
   {
      // Populate function call string and destroy FunctionCall
      utilStringCchCatf(szOutput, 1024, TEXT("\r\n  %s(..) on line %u of %s"), pTopCall->szFunction, pTopCall->iLine, pTopCall->szFileName);
      deleteFunctionCall(pTopCall);
   }

   /// [UNLOCK] 
   ReleaseMutex(pCallStackList->hMutex);

   // Return result
   return szOutput;
}


/// Function name  : pushFunctionCallByThread
/// Macro Name     : TRACK_FUNCTION()
// Description     : Creates a new FunctionCall on the CallStack of the specified thread
// 
// CALL_STACK_LIST*  pCallStackList : [in] List of CallStacks
// CONST DWORD       dwThreadID     : [in] ThreadID of the CallStack create a new FunctionCall upon
// CONST TCHAR*      szFileName     : [in] Name of the file containing the function call
// CONST TCHAR*      szFunction     : [in] Name of the function call
// CONST UINT        iLineNumber    : [in] Line number of the funciton call
// 
BearScriptAPI
VOID  pushFunctionCallByThread(CALL_STACK_LIST*  pCallStackList, CONST DWORD  dwThreadID, CONST TCHAR*  szFileName, CONST TCHAR*  szFunction, CONST UINT  iLineNumber)
{
   CALL_STACK*     pCallStack;     // Existing CallStack matching the input thread ID, or new CallStack created with the input ID

   /// [LOCK] 
   WaitForSingleObject(pCallStackList->hMutex, INFINITE);

   // Lookup matching CallStack
   if (!findCallStackByThread(pCallStackList, dwThreadID, pCallStack))
   {
      /// [NOT FOUND] Create new stack
      pCallStack = createCallStack(dwThreadID);
      appendObjectToList(pCallStackList, (LPARAM)pCallStack);
   }
      
   /// Push new position onto the stack
   pushObjectOntoStack(pCallStack, (LPARAM)createFunctionCall(szFileName, szFunction, iLineNumber));

   /// [UNLOCK] 
   ReleaseMutex(pCallStackList->hMutex);
}


/// Function name  : popFunctionCallByThread
// Description     : Removes and returns the FunctionCall on the top of the CallStack of the specified thread
// 
// CALL_STACK_LIST*  pCallStackList : [in] List of CallStacks
// CONST DWORD       dwThreadID     : [in] ID of the thread
// 
// Return Value   : Top FunctionCall if found, NULL if the thread had no callstack
// 
BearScriptAPI
FUNCTION_CALL*  popFunctionCallByThread(CALL_STACK_LIST*  pCallStackList, CONST DWORD  dwThreadID)
{
   CALL_STACK*     pCallStack;     // CallStack matching the input thread ID
   FUNCTION_CALL*  pTopCall;       // Call on the top of the call stack

   // Prepare
   pTopCall = NULL;

   // Lookup matching CallStack
   if (findCallStackByThread(pCallStackList, dwThreadID, pCallStack))
   {
      /// Pop top call from the stack (if any)
      pTopCall = (FUNCTION_CALL*)popObjectFromStack(pCallStack);

      // [CHECK] Is the stack now empty?
      if (!getStackItemCount(pCallStack))
      {
         /// [EMPTY] Remove CallStack from list
         removeObjectFromListByValue(pCallStackList, (LPARAM)pCallStack);
         deleteCallStack(pCallStack);
      }
   }
      
   // Return result if any, otherwise NULL
   return pTopCall;
}


/// Function name  : topFunctionCallByThread
/// Macro Name     : GET_LAST_FUNCTION()
// Description     : Retrieves but does not remove the top FunctionCall of the CallStack of the specified thread
///                           This function LOCKS the CallStackList
// 
// CALL_STACK_LIST*  pCallStackList : [in] List of CallStacks
// CONST DWORD       dwThreadID     : [in] ID of the thread
// 
// Return Value   : Top FunctionCall from the CallSTack of the specified thread if found, otherwise NULL
// 
BearScriptAPI
FUNCTION_CALL*  topFunctionCallByThread(CALL_STACK_LIST*  pCallStackList, CONST DWORD  dwThreadID)
{
   CALL_STACK*     pCallStack;      // CallStack matching the input thread ID
   FUNCTION_CALL*  pTopCall;        // Call on the top of the call stack

   // Prepare
   pTopCall = NULL;

   /// [LOCK] 
   WaitForSingleObject(pCallStackList->hMutex, INFINITE);

   // Lookup matching CallStack
   if (findCallStackByThread(pCallStackList, dwThreadID, pCallStack))
      /// [FOUND] Pop top call from the stack (if any)
      pTopCall = (FUNCTION_CALL*)topStackObject(pCallStack);

   // Return result if any, otherwise NULL
   return pTopCall;
}
