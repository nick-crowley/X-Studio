//
// Queues.cpp : First-In-First-Out queue made from a linked list
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : pushLastQueueObject
// Description     : Adds an object to the back of the queue
// 
// QUEUE*  pQueue  : [in] Queue to be appended
// LPARAM  pObject : [in] Object to add to the queue
// 
VOID  pushLastQueueObject(QUEUE*  pQueue, LPARAM  pObject)
{
   /// Add item to the end of the queue
   appendItemToList(pQueue, createQueueItem(pObject));
}


/// Function name  : popLastQueueObject
// Description     : Removes an object from the back of the queue
// 
// QUEUE*  pQueue : [in] Queue to remove the item from
// 
// Return Value   : Object from the front of the queue, if any, otherwise NULL
// 
UtilityAPI
LPARAM  popLastQueueObject(QUEUE*  pQueue)
{
   QUEUE_ITEM*  pQueueItem;      // Wrapper
   LPARAM       pOutput;         // Operation result

   // Prepare
   pOutput = NULL;

   /// Attempt to lookup last item
   if (removeItemFromListByIndex(pQueue, getQueueItemCount(pQueue) - 1, pQueueItem))
   {
      // [SUCCESS] Extract object and delete wrapper
      pOutput = pQueueItem->pData;
      deleteQueueItem(pQueue, pQueueItem, FALSE);
   }

   // Return object if found, otherwise NULL
   return pOutput;
}


/// Function name  : popFirstQueueObject
// Description     : Removes an object from the front of the queue
// 
// QUEUE*  pQueue : [in] Queue to remove the item from
// 
// Return Value   : Object from the front of the queue, if any, otherwise NULL
// 
UtilityAPI
LPARAM  popFirstQueueObject(QUEUE*  pQueue)
{
   QUEUE_ITEM*  pQueueItem;      // Wrapper
   LPARAM       pOutput;         // Operation result

   // Prepare
   pOutput = NULL;

   /// Attempt to lookup first item
   if (removeItemFromListByIndex(pQueue, 0, pQueueItem))
   {
      // [SUCCESS] Extract object and delete wrapper
      pOutput = pQueueItem->pData;
      deleteQueueItem(pQueue, pQueueItem, FALSE);
   }

   // Return object if found, otherwise NULL
   return pOutput;
}


/// Function name  : firstObjectFromQueue
// Description     : Retrieves the item from the front of the queue but does not remove it
// 
// CONST QUEUE*  pQueue : [in] Queue to query
// 
// Return Value   : Item from the front of the queue, if any, otherwise NULL
// 
UtilityAPI
LPARAM  firstObjectFromQueue(CONST QUEUE*  pQueue)
{
   LPARAM  pOutput;

   /// Lookup first queue item
   findListObjectByIndex(pQueue, 0, (LPARAM&)pOutput);

   // Return first queue item if found, otherwise NULL
   return pOutput;
}


/// Function name  : lastObjectFromQueue
// Description     : Retrieves the item from the back of the queue but does not remove it
// 
// CONST QUEUE*  pQueue : [in] Queue to query
// 
// Return Value   : Item from the front of the queue, if any, otherwise NULL
// 
UtilityAPI
LPARAM  lastObjectFromQueue(CONST QUEUE*  pQueue)
{
   LPARAM  pOutput;

   /// Lookup first queue item
   findListObjectByIndex(pQueue, (getQueueItemCount(pQueue) - 1), (LPARAM&)pOutput);

   // Return first queue item if found, otherwise NULL
   return pOutput;
}

