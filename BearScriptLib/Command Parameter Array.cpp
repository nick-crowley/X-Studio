//
// Parameter Array.cpp : Variable size array for holding the Parameters in a Command
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createParameterArray
// Description     : Creats a new ParameterArray object of a specified size
// 
// CONST UINT  iSize   : [in] Number of elements for the ParameterArray to hold
// 
// Return Value   : New ParameterArray object, you are responsible for destroying it
// 
PARAMETER_ARRAY*   createParameterArray(CONST UINT  iSize)
{
   PARAMETER_ARRAY*   pArray;    // Array being created

   // [CHECK] Ensure size is valid
   ASSERT(iSize != NULL);

   // Prepare
   pArray = utilCreateEmptyObject(PARAMETER_ARRAY);

   // Set initial size
   pArray->pItems = utilCreateEmptyObjectArray(PARAMETER*, iSize);
   pArray->iSize  = iSize;

   // Return new object
   return pArray;
}

/// Function name  : deleteParameterArray
// Description     : Delete a ParameterArray object, but do not attempt to free the PARAMETER objects it contains
// 
// PARAMETER_ARRAY*  &pArray   : [in] ParameterArray to delete
// 
VOID  deleteParameterArray(PARAMETER_ARRAY*  &pArray)
{
   // Delete Parameters
   for (UINT i = 0; i < pArray->iSize; i++)
   {
      // Items are not necessarily contiguous
      if (pArray->pItems[i])
         deleteParameter(pArray->pItems[i]);
   }

   // Delete parameter storage array
   utilDeleteObject(pArray->pItems);

   // Delete calling object
   utilDeleteObject(pArray);
}


/// Function name  : duplicateParameterArray
// Description     : Create a duplicate of a parameter array, also duplicating the parameters within
// 
// CONST PARAMETER_ARRAY*  pArray   : [in] ParameterArray to duplicate
// 
// Return Value   : New ParameterArray, you are responsible for destroying it
// 
PARAMETER_ARRAY*  duplicateParameterArray(CONST PARAMETER_ARRAY*  pArray)
{
   PARAMETER_ARRAY*   pCopy;    // Array being created

   // Create new array
   pCopy = utilCreateEmptyObject(PARAMETER_ARRAY);

   // Allocate enough storage for all parameters
   pCopy->pItems = utilCreateEmptyObjectArray(PARAMETER*, pArray->iSize);

   // Copy properties
   pCopy->iSize  = pArray->iSize;
   pCopy->iCount = pArray->iCount;

   // Duplicate parameters
   for (UINT i = 0; i < pArray->iSize; i++)
   {
      if (pArray->pItems[i])
         pCopy->pItems[i] = duplicateParameter(pArray->pItems[i]);
   }

   // Return new copy of object
   return pCopy;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                          HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : appendParameterToArray
// Description     : Add a new Parameter to the next free element of a ParameterArray
// 
// PARAMETER_ARRAY*  pArray      : [in] ParameterArray to append
// PARAMETER*        pParameter  : [in] Parameter to add to the ParameterArray
// 
VOID  appendParameterToArray(PARAMETER_ARRAY*  pArray, PARAMETER*  pParameter)
{
   // [CHCK] Resize array if too small
   if (pArray->iCount == pArray->iSize)
      performParameterArrayResizing(pArray, pArray->iSize + 10);
      
   // Append new parameter
   pArray->pItems[pArray->iCount++] = pParameter;
}


/// Function name  : findParameterInArray
// Description     : Locate a Parameter within a ParameterArray
// 
// CONST PARAMETER_ARRAY*  pArray   : [in]  ParameterArray to search
// CONST UINT              iIndex   : [in]  Zero based index of the parameter to retrieve
// PARAMETER*             &pOutput  : [out] Parameter if found, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BearScriptAPI
BOOL  findParameterInArray(CONST PARAMETER_ARRAY*  pArray, CONST UINT  iIndex, PARAMETER*  &pOutput)
{
   // Prepare
   pOutput = NULL;

   // [CHECK] Validate index
   if (iIndex >= pArray->iCount)
      return FALSE;

   // [FOUND] Return specified parameter
   pOutput = pArray->pItems[iIndex];
   return TRUE;
}

/// Function name  : getArrayItemCount
// Description     : Returns the number of items in a ParameterArray
// 
// CONST PARAMETER_ARRAY*  pArray : [in] ParameterArray to examine
// 
// Return Value   : Number of elements in the array, they are not necessarily consecutive
// 
UINT  getArrayItemCount(CONST PARAMETER_ARRAY*  pArray)
{
   return pArray->iCount;
}


/// Function name  : insertParameterIntoArray
// Description     : Inserts a PARAMETER into a ParameterArray at the specified index
// 
// PARAMETER_ARRAY*   pArray     : [in] Array to append
// CONST UINT         iIndex     : [in] Zero based index at which to insert the new Parameter
// PARAMETER*         pParameter : [in] Parameter to insert into the array
// 
VOID  insertParameterIntoArray(PARAMETER_ARRAY*  pArray, CONST UINT  iIndex, PARAMETER*  pParameter)
{
   // [CHECK] Resize array if it's too small
   if (iIndex >= pArray->iSize)
      performParameterArrayResizing(pArray, iIndex + 5);

   // Insert new parameter
   pArray->pItems[iIndex] = pParameter;
   pArray->iCount++;
}


/// Function name  : performParameterArrayResizing
// Description     : Resize a ParameterArray to the specified number of elements, preserving any existing elements
// 
// PARAMETER_ARRAY*  pArray   : [in/out] ParameterArray to resize
// CONST UINT        iNewSize : [in]     New total number of elements for the array to hold
// 
VOID  performParameterArrayResizing(PARAMETER_ARRAY*  pArray, CONST UINT  iNewSize)
{
   PARAMETER**   pExistingItems;      // New array to replace the existing one

   // [CHECK] Ensure array is being expanded
   ASSERT(iNewSize > pArray->iSize);

   // Store the existing data and allocate new storage
   pExistingItems = pArray->pItems;
   pArray->pItems = utilCreateEmptyObjectArray(PARAMETER*, iNewSize);

   // Copy old parameters to new array
   utilCopyObjects(pArray->pItems, pExistingItems, PARAMETER*, pArray->iSize);

   // Destroy old storage and update array size
   utilDeleteObject(pExistingItems);
   pArray->iSize = iNewSize;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                          FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

