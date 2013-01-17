//
// Lists.cpp : Double Linked list
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createList
// Description     : Create an empty list
// 
// DESTRUCTOR  pfnDeleteItem : [in][optional] Function that deletes item data (if necessary)
//
// Return Value   : New list, you are responsible for destroying it
// 
UtilityAPI
LIST*  createList(DESTRUCTOR  pfnDeleteItem)
{
   LIST*  pNewList;

   // Create new list
   pNewList = utilCreateEmptyObject(LIST);

   // Store deletion function
   pNewList->pfnDeleteItem = pfnDeleteItem;

   // Return new list
   return pNewList;
}

/// Function name  : createListItem
// Description     : Create a new list item
// 
// LPARAM  oItemData : [in] Item data
//
// Return Value   : New list item, you are responsible for destroying it
// 
UtilityAPI
LIST_ITEM*  createListItem(LPARAM  oItemData)
{
   LIST_ITEM*  pNewItem;

   // Create new item
   pNewItem = utilCreateEmptyObject(LIST_ITEM);

   // Set data
   pNewItem->pData = oItemData;

   // Return item
   return pNewItem;
}


/// Function name  : deleteList
// Description     : Destroy a list and all it's items
// 
// LIST*  &pList : [in/out] List to destroy
// 
UtilityAPI
VOID  deleteList(LIST*  &pList)
{
   LIST_ITEM  *pNextItem;     // ListItem following the item currently being deleted

   /// Delete items iteratively
   for (LIST_ITEM*  pCurrentItem = getListHead(pList); pCurrentItem; pCurrentItem = pNextItem)
   {
      // Save next item
      pNextItem = pCurrentItem->pNext;

      // Delete current item
      deleteListItem(pList, pCurrentItem, TRUE);
   }

   // Delete calling object
   utilDeleteObject(pList);
}


/// Function name  : deleteListContents
// Description     : Destroy just the items in a List
// 
// LIST*  pList : [in/out] Target list
// 
UtilityAPI
VOID  deleteListContents(LIST*  pList)
{
   LIST_ITEM  *pNextItem;     // ListItem following the item currently being deleted

   /// Delete items iteratively
   for (LIST_ITEM*  pCurrentItem = getListHead(pList); pCurrentItem; pCurrentItem = pNextItem)
   {
      // Save next item
      pNextItem = pCurrentItem->pNext;

      // Delete current item
      deleteListItem(pList, pCurrentItem, TRUE);
   }

   // Zero properties
   pList->pHead = pList->pTail = NULL;

   // Reset count to zero
   pList->iCount = 0;
}


/// Function name  : deleteListItem
// Description     : Destroy a list item and optionally it's contents. Does not delete any items attached to it
// 
// CONST LIST*  pList     : [in] List containing the ListItem to destroy
// LIST_ITEM*  &pItem     : [in] ListItem to destroy
// CONST BOOL   bContents : [in] Whether to also destroy contents
// 
UtilityAPI
VOID  deleteListItem(CONST LIST*  pList, LIST_ITEM*  &pItem, CONST BOOL  bContents)
{
   // [OPTIONAL] Delete contents
   if (bContents AND pList->pfnDeleteItem)
      // Invoke item destructor
      (*pList->pfnDeleteItem)(pItem->pData);

   // Delete calling object
   utilDeleteObject(pItem);
}


/// Function name  : destructorSimpleObject
// Description     : Basic destructor for objects that are just a simple block of memory
// 
// LPARAM  pObject : [in] Reference to a pointer to a simple object
// 
UtilityAPI
VOID  destructorSimpleObject(LPARAM  pObject)
{
   utilDeleteObject((LPARAM*&)pObject);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : getListItemCount
// Description     : Returns the number of items in a list
// 
// CONST LIST*  pList   : [in] List to query
// 
// Return Value   : Number of items in the list
// 
UtilityAPI
UINT   getListItemCount(CONST LIST*  pList)
{
   // Return item count
   return pList->iCount;
}


/// Function name  : getListHead
// Description     : Retrieve the first item in a list
// 
// CONST LIST*  pList : [in] List to query
// 
// Return Value   : First item in the list, or NULL if list is empty
// 
UtilityAPI
LIST_ITEM*  getListHead(CONST LIST*  pList)
{
   // Return first item
   return pList->pHead;
}


/// Function name  : getListTail
// Description     : Retrieve the last item in a list
// 
// CONST LIST*  pList : [in] List to query
// 
// Return Value   : Last item in the list, or NULL if list is empty
// 
UtilityAPI
LIST_ITEM*  getListTail(CONST LIST*  pList)
{
   // Return last item
   return pList->pTail;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : appendItemToList
// Description     : Append a ListItem to the end of a List
// 
// LIST*       pList     : [in] List to be appended
// LIST_ITEM*  pNewItem  : [in] Item to append
// 
UtilityAPI
VOID  appendItemToList(LIST*  pList, LIST_ITEM*  pNewItem)
{
   /// [EMPTY LIST] Start new list
   if (pList->iCount == 0)
   {
      // Set new HEAD and TAIL
      pList->pHead = pNewItem;
      pList->pTail = pNewItem;

      // Set item NEXT and PREV to NULL to indicate it's both HEAD and TAIL
      pNewItem->pNext = NULL;
      pNewItem->pPrev = NULL;
   }
   /// [NON-EMPTY LIST] Add new item to the end
   else
   {
      // Connect new item to existing TAIL
      pNewItem->pPrev = getListTail(pList);
      pNewItem->pNext = NULL;

      // Connect existing TAIL to the new item
      getListTail(pList)->pNext = pNewItem;

      // Set new TAIL
      pList->pTail = pNewItem;
   }

   // Increase list size
   pList->iCount++;
}


/// Function name  : appendObjectToList
// Description     : Helper function for appending objects to lists
// 
// LIST*   pList   : [in] List
// LPARAM  pObject : [in] Object to append
// 
UtilityAPI
VOID  appendObjectToList(LIST*  pList, LPARAM  pObject)
{
   appendItemToList(pList, createListItem(pObject));
}

/// Function name  : appendListToList
// Description     : Joins two lists together. The items are removed from the source list and appended
//                      in their existing order to the destination list.
// 
// LIST*       pDestination  : [in] Destination list to append
// LIST*       pSource       : [in] Source list containing the items to copy
// CONST UINT  iStartIndex   : [in] Zero-based index of the first item to copy
// 
VOID  appendListToList(LIST*  pDestination, LIST*  pSource, CONST UINT  iStartIndex)
{
   TODO("write this function");
}


/// Function name  : destroyListItemByIndex
// Description     : Finds and destroys an item in a list, by index
// 
// LIST*       pList  : [in] List
// CONST UINT  iIndex : [in] Zero based index
// 
// Return Value   : TRUE if found and destroyed, otherwise FALSE
// 
UtilityAPI
BOOL  destroyListItemByIndex(LIST*  pList, CONST UINT  iIndex)
{
   LIST_ITEM*  pTargetItem;
   BOOL        bResult;

   // [CHECK] Lookup item
   if (bResult = removeItemFromListByIndex(pList, iIndex, pTargetItem))
      /// [FOUND] Destroy item
      deleteListItem(pList, pTargetItem, TRUE);

   // Return result
   return bResult;
}


/// Function name  : findListIndexByValue
// Description     : Find the index of an item in a list
// 
// CONST LIST*  pList   : [in]  List to search
// LPARAM       xValue  : [in]  Value to search for
// 
// Return Value   : Zero-based index if found, otherwise -1
//
UtilityAPI
UINT   findListIndexByValue(CONST LIST*  pList, LPARAM  xValue)
{
   LIST_ITEM*  pIterator;
   UINT        iIndex;

   // Prepare
   iIndex = 0;

   /// Search list for a matching item
   for (pIterator = getListHead(pList); pIterator AND (pIterator->pData != xValue); pIterator = pIterator->pNext)
      // [NOT FOUND] Increment current index
      iIndex++;

   /// Return index if found, otherwise -1
   return pIterator AND (pIterator->pData == xValue) ? iIndex : -1;
}


/// Function name  : findListItemByIndex
// Description     : Find a ListItem within a list (by index)
// 
// CONST LIST*  pList   : [in]  List to search
// UINT         iIndex  : [in]  Zero-based index to search for
// LIST_ITEM*  &pOutput : [out] Resultant ListItem if found, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
//
UtilityAPI
BOOL  findListItemByIndex(CONST LIST*  pList, UINT  iIndex, LIST_ITEM*  &pOutput)
{
   UINT  iCurrentIndex;    // List index iterator

   // Prepare
   pOutput = NULL;

   // [CHECK] Ensure index is valid
   if (iIndex >= pList->iCount)
      return FALSE;

   /// [FIRST HALF] Iterate forwards through list items
   if (iIndex < getListItemCount(pList) / 2)
   {
      // Prepare
      iCurrentIndex = 0;

      // Iterate forwards
      for (LIST_ITEM*  pIterator = getListHead(pList); !pOutput AND pIterator; pIterator = pIterator->pNext)
         // Check whether current index matches the input index
         if (iIndex == iCurrentIndex++)
            // [FOUND] Set result
            pOutput = pIterator;
   }
   /// [SECOND HALF] Iterate backwards through list items
   else
   {
      // Prepare
      iCurrentIndex = (getListItemCount(pList) - 1);

      // Iterate backwards
      for (LIST_ITEM*  pIterator = getListTail(pList); !pOutput AND pIterator; pIterator = pIterator->pPrev)
         // Check whether current index matches the input index
         if (iIndex == iCurrentIndex--)
            // [FOUND] Set result
            pOutput = pIterator;
   }
   
   // Return TRUE if found
   return (pOutput != NULL);
}


/// Function name  : findListItemByValue
// Description     : Find the item containing a specific object within a list
// 
// CONST LIST*  pList   : [in]  List to search
// LPARAM       xValue  : [in]  Value to search for
// LIST_ITEM*  &pOutput : [out] Resultant ListItem if found, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
//
UtilityAPI
BOOL  findListItemByValue(CONST LIST*  pList, LPARAM  xValue, LIST_ITEM*  &pOutput)
{
   // Prepare
   pOutput = NULL;

   /// Iterate through list
   for (LIST_ITEM*  pIterator = getListHead(pList); pIterator; pIterator = pIterator->pNext)
   {
      // [CHECK] Compare current item
      if (pIterator->pData == xValue)
      {
         /// [FOUND] Set result and return TRUE
         pOutput = pIterator;
         return TRUE;
      }
   }

   /// [NOT FOUND] Return FALSE
   return FALSE;
}


/// Function name  : findListObjectByIndex
// Description     : Find an object within a list by index
// 
// CONST LIST*  pList   : [in]  List to search
// UINT         iIndex  : [in]  Zero-based index to search for
// LPARAM      &pOutput : [out] Resultant object if found, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
//
UtilityAPI
BOOL  findListObjectByIndex(CONST LIST*  pList, UINT  iIndex, LPARAM  &pOutput)
{
   LIST_ITEM*  pListItem;  // ListItem wrapper
   BOOL        bResult;

   // Prepare
   pOutput = NULL;

   /// Lookup item
   if (bResult = findListItemByIndex(pList, iIndex, pListItem))
      // [FOUND] Set output
      pOutput = pListItem->pData;

   // Return TRUE if found
   return bResult;
}


/// Function name  : insertObjectIntoListByIndex
// Description     : Inserts an object into a list at the specified index. If an item already exists at that
//                     -> index then the existing item is shifted to the right, and the new item inserted in it's place.
///                    You can append the list by specifying the first free index of the list, but if the index 
///                     is greater than that the function will fail
// 
// LIST*       pList   : [in] List to insert item into
// CONST UINT  iIndex  : [in] Zero-based index to insert the item at
// LPARAM      pData   : [in] Object data to insert
// 
// Return Value   : TRUE if inserted, FALSE if the index was invalid
// 
UtilityAPI
BOOL  insertObjectIntoListByIndex(LIST*  pList, CONST UINT  iIndex, LPARAM  pData)
{
   LIST_ITEM   *pNewItem,           // Wrapper for item being inserted
               *pFollowingItem,     // [INSERT AT MIDDLE] Item currently at the input index
               *pPreceedingItem;    // [INSERT AT MIDDLE] Item preceeding the existing item at the input index

   // [CHECK] Ensure index is valid
   if (iIndex > pList->iCount)
      return FALSE;
   
   // Create item wrapper
   pNewItem = createListItem(pData);

   /// [EMPTY LIST] Insert as new HEAD and TAIL
   if (getListItemCount(pList) == 0)
   {
      // Define HEAD and TAIL
      pList->pHead = pNewItem;
      pList->pTail = pNewItem;

      // Ensure item has no NEXT or PREV
      pNewItem->pNext = NULL;
      pNewItem->pPrev = NULL;
   }
   /// [INSERT AT HEAD] Insert as new HEAD
   else if (iIndex == 0)
   {
      // Connect new item to existing HEAD
      pNewItem->pNext = getListHead(pList);
      pNewItem->pPrev = NULL;

      // Connect existing HEAD to new item
      getListHead(pList)->pPrev = pNewItem;

      // Set new HEAD
      pList->pHead = pNewItem;
   }
   /// [INSERT AT TAIL] Attach as the new TAIL
   else if (iIndex == pList->iCount)
   {
      // Connect new item to existing TAIL
      pNewItem->pPrev = getListTail(pList);
      pNewItem->pNext = NULL;

      // Connect existing TAIL to next item
      getListTail(pList)->pNext = pNewItem;

      // Replace TAIL
      pList->pTail = pNewItem;
   }
   /// [INSERT AT MIDDLE] Insert between two existing items
   else if (findListItemByIndex(pList, iIndex, pFollowingItem))
   {
      // Prepare convenience pointer
      pPreceedingItem = pFollowingItem->pPrev;

      // Connect PRECEEDING item to NEW item
      pNewItem->pPrev = pPreceedingItem;
      pPreceedingItem->pNext = pNewItem;

      // Connect FOLLOWING item to NEW item
      pNewItem->pNext = pFollowingItem;
      pFollowingItem->pPrev = pNewItem;
   }

   /// Increase list length and return TRUE
   pList->iCount++;
   return TRUE;
}


/// Function name  : insertListIntoListByIndex
// Description     : Inserts all the items in one list into another at the specified index
///                        NB: The items are moved, not copied, leaving the source list empty.
// 
// LIST*       pTargetList  : [in/out] Destination list
// CONST UINT  iTargetIndex : [in]     Zero-based item index at which to insert the new list
// LIST*       pSourceList  : [in/out] Source list containing the items to move. This will be emptied.
// 
// Return Value   : TRUE if list was inserted or the source list was empty, FALSE if the target index was invalid
// 
UtilityAPI
BOOL  insertListIntoListByIndex(LIST*  pTargetList, CONST UINT  iTargetIndex, LIST*  pSourceList, CONST UINT  iSourceIndex)
{
   LIST_ITEM   *pFollowingItem,     // [INSERT AT MIDDLE] Item currently at the target index
               *pPreceedingItem,    // [INSERT AT MIDDLE] Item preceeding the existing item at the target index
               *pSourceHead,        // The first item to be extracted from the source list
               *pNewSourceTail;     // The new TAIL for the source list, after the operation is complete

   // [CHECK] Ensure target and source indicies are valid
   if (iTargetIndex > getListItemCount(pTargetList) OR iSourceIndex >= getListItemCount(pSourceList))
      return FALSE;

   // Determine the first item to be moved from the source list and the new source TAIL
   findListItemByIndex(pSourceList, iSourceIndex, pSourceHead);
   pNewSourceTail = pSourceHead->pPrev;

   /// [EMPTY TARGET] Insert source as new HEAD and TAIL
   if (getListItemCount(pTargetList) == 0)
   {
      // Overwrite target HEAD and TAIL
      pTargetList->pHead = pSourceHead;
      pTargetList->pTail = getListTail(pSourceList);

      // Ensure HEAD has no NEXT and TAIL has no PREV  (Unnecessary redundancy)
      getListHead(pTargetList)->pPrev = NULL;
      getListTail(pTargetList)->pNext = NULL;
   }
   /// [INSERT AT TARGET HEAD] Insert source as new HEAD
   else if (iTargetIndex == 0)
   {
      // Connect source TAIL and target HEAD
      getListTail(pSourceList)->pNext = getListHead(pTargetList);
      getListHead(pTargetList)->pPrev = getListTail(pSourceList);

      // Overwrite target HEAD with source HEAD
      pTargetList->pHead = pSourceHead;

      // Ensure new HEAD has no PREV
      getListHead(pTargetList)->pPrev = NULL;
   }
   /// [INSERT AT TARGET TAIL] Attach source as the new TAIL
   else if (iTargetIndex == getListItemCount(pTargetList))
   {
      // Connect target TAIL and source HEAD
      getListTail(pTargetList)->pNext = pSourceHead;
      pSourceHead->pPrev = getListTail(pTargetList);

      // Overwrite target TAIL with source TAIL
      pTargetList->pTail = getListTail(pSourceList);
   }
   /// [INSERT AT TARGET MIDDLE] Insert source between two existing items
   else if (findListItemByIndex(pTargetList, iTargetIndex, pFollowingItem))
   {
      // Prepare convenience pointer
      pPreceedingItem = pFollowingItem->pPrev;

      // Connect PRECEEDING item to source HEAD
      pPreceedingItem->pNext = pSourceHead;
      pSourceHead->pPrev = pPreceedingItem;

      // Connect FOLLOWING item to source TAIL
      pFollowingItem->pPrev = getListTail(pSourceList);
      getListTail(pSourceList)->pNext = pFollowingItem;
   }

   /// Calculate new size of both lists
   pTargetList->iCount += (getListItemCount(pSourceList) - iSourceIndex);
   pSourceList->iCount -= (getListItemCount(pSourceList) - iSourceIndex);

   /// Define source list's new TAIL (if any)
   if (pSourceList->pTail = pNewSourceTail)
      getListTail(pSourceList)->pNext = NULL;

   // [MOVED ENTIRE SOURCE LIST] Define source list's lack of HEAD
   if (iSourceIndex == 0)
      pSourceList->pHead = NULL;

   // Return TRUE
   return TRUE;
}



/// Function name  : isValueInList
// Description     : Checks whether an object is within a list
// 
// CONST LIST*  pList   : [in]  List to search
// LPARAM       xValue  : [in]  Value to search for
// 
// Return Value   : TRUE if found, FALSE otherwise
//
UtilityAPI
BOOL  isValueInList(CONST LIST*  pList, LPARAM  xValue)
{
   /// Iterate through list
   for (LIST_ITEM*  pIterator = getListHead(pList); pIterator; pIterator = pIterator->pNext)
   {
      // [CHECK] Compare current item
      if (pIterator->pData == xValue)
         /// [FOUND] Set result and return TRUE
         return TRUE;
   }

   /// [NOT FOUND] Return FALSE
   return FALSE;
}


/// Function name  : removeItemFromList
// Description     : Removes a ListItem from a list (but does not delete it)
// 
// LIST*       pList : [in] List to remove an item from
// LIST_ITEM*  pItem : [in] Item to remove the list
// 
// Return Type : TRUE if successfully removed, otherwise FALSE
//
UtilityAPI
BOOL  removeItemFromList(LIST*  pList, LIST_ITEM*  pItem)
{
   LIST_ITEM  *pPrevItem,  // Convenience pointer for the item preceeding the input item
              *pNextItem;  // Convenience pointer for the item following the input item

   // Examine list count
   switch (getListItemCount(pList))
   {
   /// [EMPTY-LIST] Return FALSE
   case 0:
      return FALSE;

   /// [SINGLE ITEM] Set HEAD and TAIL to NULL
   case 1:
      pList->pHead = NULL;
      pList->pTail = NULL;
      break;

   default:
      /// [REMOVE HEAD] Replace HEAD with the following item
      if (pItem == getListHead(pList))
      {
         // Replace HEAD
         pList->pHead = getListHead(pList)->pNext;
         // Ensure new HEAD's PREV is NULL
         getListHead(pList)->pPrev = NULL;
      }
      /// [REMOVE TAIL] Replace TAIL with the preceeding item
      else if (pItem == getListTail(pList))
      {
         // Replace TAIL
         pList->pTail = getListTail(pList)->pPrev;
         // Ensure new TAIL's NEXT is NULL
         getListTail(pList)->pNext = NULL;
      }
      /// [REMOVE MIDDLE] Connect surrounding items together
      else
      {
         // Create convenience pointers
         pPrevItem = pItem->pPrev;
         pNextItem = pItem->pNext;

         // Connect them together
         pPrevItem->pNext = pNextItem;
         pNextItem->pPrev = pPrevItem;
      }
      break;
   }

   // Sever input item
   pItem->pNext = NULL;
   pItem->pPrev = NULL;

   // Shrink list and return TRUE
   pList->iCount--;
   return TRUE;
}


/// Function name  : removeItemFromListByIndex
// Description     : Finds an item in a list by index, removes it, returns it, but does not delete it
// 
// LIST*        pList   : [in/out] List containing the item to be removed
// CONST UINT   iIndex  : [in]     Zero based index of the item to remove
// LIST_ITEM*  &pOutput : [out]    Item that was removed, if found, otherwise NULL
// 
// Return Value   : TRUE if found and removed, FALSE if list was empty or index was invalid
// 
UtilityAPI
BOOL  removeItemFromListByIndex(LIST*  pList, CONST UINT  iIndex, LIST_ITEM*  &pOutput)
{
   // Prepare
   pOutput = NULL;

   // Find specified item in list
   if (findListItemByIndex(pList, iIndex, pOutput))      // Fails if item is not found
      /// Remove item (does not delete it)
      removeItemFromList(pList, pOutput);    // Can only fail if list is empty

   // Return TRUE item was found and deleted
   return (pOutput != NULL);
}


/// Function name  : removeItemFromListByIndex
// Description     : Removes an item from a list by value
// 
// LIST*    pList   : [in/out] List containing the item to be removed
// LPARAM   xValue  : [in]     Value to search for
// 
// Return Value   : TRUE if found and removed, FALSE if not found
// 
UtilityAPI
BOOL  removeObjectFromListByValue(LIST*  pList, LPARAM  xValue)
{
   /// Iterate through list
   for (LIST_ITEM*  pIterator = getListHead(pList); pIterator; pIterator = pIterator->pNext)
   {
      // [CHECK] Compare current item
      if (pIterator->pData == xValue)
      {
         /// [FOUND] Remove from list, destroy wrapper
         removeItemFromList(pList, pIterator);
         deleteListItem(pList, pIterator, FALSE);
         return TRUE;
      }
   }

   // [FAILURE] Return FALSE
   return FALSE;
}
