//
// Stacks.cpp : First-In-Last-Out stack made from a linked list
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : destroyTopStackObject
// Description     : Removes and destroys the object from the top of the stack
// 
// STACK*  pStack : [in] Stack 
// 
// Return Value   : TRUE if found, otherwise FALSE
// 
UtilityAPI
BOOL  destroyTopStackObject(STACK*  pStack)
{
   STACK_ITEM*  pStackItem;      // StackItem wrapper
   BOOL         bResult;         // Operation result

   // Remove last stack item
   if (bResult = removeItemFromListByIndex(pStack, (getStackItemCount(pStack) - 1), pStackItem))
      /// Destroy item
      deleteStackItem(pStack, pStackItem, TRUE);
   
   // Return TRUE if found
   return bResult;
}


/// Function name  : pushObjectOntoStack
// Description     : Adds an object onto the top of a stack
// 
// STACK*   pStack  : [in] Target stack
// LPARAM   pObject : [in] Object to push onto the stack
// 
UtilityAPI
VOID   pushObjectOntoStack(STACK*  pStack, LPARAM  pObject)
{
   /// Add item to the end of the stack
   appendItemToList(pStack, createStackItem(pObject));
}


/// Function name  : popObjectFromStack
// Description     : Removes an object from the top of the stack
// 
// STACK*  pStack : [in] Stack to remove the item from
// 
// Return Value   : Object on the top of the stack if any, otherwise NULL
// 
UtilityAPI
LPARAM  popObjectFromStack(STACK*  pStack)
{
   STACK_ITEM*  pStackItem;      // StackItem wrapper
   LPARAM       pOutput;         // Operation result

   // Prepare
   pOutput = NULL;

   // Remove last stack item
   if (removeItemFromListByIndex(pStack, (getStackItemCount(pStack) - 1), pStackItem))
   {
      /// Extract object and destroy wrapper
      pOutput = pStackItem->pData;
      deleteStackItem(pStack, pStackItem, FALSE);
   }

   // Return object removed if any, otherwise NULL
   return pOutput;
}


/// Function name  : popObjectFromStackBottom
// Description     : Removes an object from the top of the stack
// 
// STACK*  pStack : [in] Stack to remove the item from
// 
// Return Value   : Object on the top of the stack if any, otherwise NULL
// 
UtilityAPI
LPARAM  popObjectFromStackBottom(STACK*  pStack)
{
   STACK_ITEM*  pStackItem;      // StackItem wrapper
   LPARAM       pOutput;         // Operation result

   // Prepare
   pOutput = NULL;

   // Remove last stack item
   if (removeItemFromListByIndex(pStack, 0, pStackItem))
   {
      /// Extract object and destroy wrapper
      pOutput = pStackItem->pData;
      deleteStackItem(pStack, pStackItem, FALSE);
   }

   // Return object removed if any, otherwise NULL
   return pOutput;
}


/// Function name  : topStackObject
// Description     : Retrieves the object on the top of the stack without removing it
// 
// CONST STACK*  pStack : [in] Stack to query
// 
// Return Value   : Top stack object if any, otherwise NULL
// 
UtilityAPI
LPARAM  topStackObject(CONST STACK*  pStack)
{
   LPARAM   pOutput;

   /// Lookup item on the top of the stack
   findListObjectByIndex(pStack, getStackItemCount(pStack) - 1, pOutput);

   // Return item or NULL
   return pOutput;
}

