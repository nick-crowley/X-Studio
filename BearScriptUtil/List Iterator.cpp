//
// FILE_NAME.cpp : FILE_DESCRIPTION
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

/// Function name  : createListIterator
// Description     : Creates data for a ListIterator
// 
// CONST LIST*  pList : [in] List to iterate over
//
// Return Value   : New ListIterator, you are responsible for destroying it
// 
UtilityAPI 
LIST_ITERATOR*   createListIterator(CONST LIST*  pList)
{
   LIST_ITERATOR*   pListIterator;       // Object being created

   // Create object
   pListIterator = utilCreateEmptyObject(LIST_ITERATOR);

   // Set properties
   pListIterator->pCurrentItem = getListHead(pList);
   pListIterator->iIndex       = 0;

   // Return object
   return pListIterator;
}


/// Function name  : deleteListIterator
// Description     : Destroys the data for the ListIterator
// 
// LIST_ITERATOR*  &pListIterator   : [in] ListIterator to destroy
// 
UtilityAPI 
VOID  deleteListIterator(LIST_ITERATOR*  &pListIterator)
{
   // Destroy calling object
   utilDeleteObject(pListIterator);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : hasCurrentItem
// Description     : Determines whether iterator has a current item
// 
// CONST LIST_ITERATOR*  pListIterator : [in] ListIterator
// 
// Return Value   : TRUE/FALSE
// 
UtilityAPI 
BOOL   hasCurrentItem(CONST LIST_ITERATOR*  pListIterator)
{
   return (pListIterator->pCurrentItem != NULL);
}


/// Function name  : hasNextItem
// Description     : Determines whether iterator has a 'next' item
// 
// CONST LIST_ITERATOR*  pListIterator : [in] ListIterator
// 
// Return Value   : TRUE/FALSE
// 
UtilityAPI 
BOOL   hasNextItem(CONST LIST_ITERATOR*  pListIterator)
{
   return (pListIterator->pCurrentItem AND pListIterator->pCurrentItem->pNext);
}


/// Function name  : hasPrevItem
// Description     : Determines whether iterator has a 'previous' item
// 
// CONST LIST_ITERATOR*  pListIterator : [in] ListIterator
// 
// Return Value   : TRUE/FALSE
// 
UtilityAPI 
BOOL   hasPrevItem(CONST LIST_ITERATOR*  pListIterator)
{
   return (pListIterator->pCurrentItem AND pListIterator->pCurrentItem->pPrev);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : getCurrentItem
// Description     : Retrieves the current item
// 
// CONST LIST_ITERATOR*  pListIterator : [in]  ListIterator
// LPARAM&               pOutput       : [out] Item if any, otherwise FALSE
// 
// Return Value   : TRUE if item exists, otherwise FALSE
// 
UtilityAPI 
BOOL   getCurrentItem(CONST LIST_ITERATOR*  pListIterator, LPARAM&  pOutput)
{
   /// Set output to current item (or NULL)
   pOutput = (hasCurrentItem(pListIterator) ? pListIterator->pCurrentItem->pData : NULL);

   // Return TRUE if item exists
   return hasCurrentItem(pListIterator);
}


/// Function name  : moveNextItem
// Description     : Attempts to advance the iterator to the next item
// 
// LIST_ITERATOR*  pListIterator   : [in] ListIterator
// 
// Return Value   : TRUE if new item exists, otherwise FALSE
// 
UtilityAPI 
BOOL  moveNextItem(LIST_ITERATOR*  pListIterator)
{
   BOOL  bResult;

   // [CHECK] Ensure item is not the last
   if (bResult = hasCurrentItem(pListIterator))
   {
      /// [SUCCESS] Advance to the next item
      pListIterator->pCurrentItem = pListIterator->pCurrentItem->pNext;
      pListIterator->iIndex++;
   }

   // Return TRUE if current item is valid
   return hasCurrentItem(pListIterator);
}


/// Function name  : movePrevItem
// Description     : Attempts to retreat the iterator to the previous item
// 
// LIST_ITERATOR*  pListIterator   : [in] ListIterator
// 
// Return Value   : TRUE if new item exists, otherwise FALSE
// 
UtilityAPI 
BOOL  movePrevItem(LIST_ITERATOR*  pListIterator)
{
   BOOL  bResult;

   // [CHECK] Ensure item is not the last
   if (bResult = hasCurrentItem(pListIterator))
   {
      /// [SUCCESS] Advance to the prev item
      pListIterator->pCurrentItem = pListIterator->pCurrentItem->pPrev;
      pListIterator->iIndex--;
   }

   // Return TRUE if current item is valid
   return bResult;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////

