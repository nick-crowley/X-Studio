//
// Memory.cpp : Memory Utility functions
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS  / GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

extern HANDLE    hUtilityHeap;     // Low fragmentation process heap

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : utilAllocateMemory
// Description     : Allocate an amount of memory
//
// CONST UINT  iBytes : [in] Number of bytes to allocate
// 
// Return type : Newly allocated memory - you are responsible for destroying it
//
UtilityAPI 
VOID*   utilAllocateMemory(CONST UINT  iBytes)
{
   return HeapAlloc(hUtilityHeap, NULL, iBytes);
}

/// Function name  : utilAllocateEmptyMemory
// Description     : Allocate an amount of memory and zero it
//
// CONST UINT  iBytes : [in] Number of bytes to allocate
// 
// Return type : Newly allocated memory - you are responsible for destroying it
//
UtilityAPI 
VOID*   utilAllocateEmptyMemory(CONST UINT  iBytes)
{
   VOID*  pNewMemory;
   
   /// Allocate memory and zero
   pNewMemory = HeapAlloc(hUtilityHeap, HEAP_ZERO_MEMORY, iBytes);
   utilZeroMemory(pNewMemory, iBytes);

   return pNewMemory;
}

/// Function name  : utilFreeMemory
// Description     : Free a block of memory previously allocated with 'utilAllocateMemory' or 'utilAllocateEmptyMemory'
// 
// VOID*  &pObject : [in] Reference to a pointer to be deleted.
///                                 NOTE: Pointer **MUST** have been allocated with the utility libary
UtilityAPI
VOID    utilFreeMemory(VOID*  &pObject)
{
   ASSERT(pObject);

   /// Free memory and zero pointer
   HeapFree(hUtilityHeap, NULL, pObject);
   pObject = NULL;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : utilCopyMemory
// Description     : Copy a block of memory from A to B
//
// VOID*       pDestination : [in] Destination memory block
// VOID*       pSource      : [in] Source memory block
// CONST UINT  iBytes       : [in] Size of block, in bytes
// 
UtilityAPI 
VOID    utilCopyMemory(VOID*  pDestination, VOID*  pSource, CONST UINT  iBytes)
{
   CopyMemory(pDestination, pSource, iBytes);
}

/// Function name  : utilValidateMemory
// Description     : Validates the library heap
// 
// Return Value   : TRUE if validation was succesful, FALSE (and an assertion) otherwise
// 
UtilityAPI
BOOL    utilValidateMemory()
{
   return HeapValidate(hUtilityHeap, NULL, NULL);
}

/// Function name  : utilZeroMemory
// Description     : Zero a block of memory
//
// VOID*       pMemory : [in] Address of block of memory
// CONST UINT  iBytes  : [in] Size of memory, in bytes
// 
UtilityAPI 
VOID    utilZeroMemory(VOID*  pMemory, CONST UINT  iBytes)
{
   SecureZeroMemory(pMemory, iBytes);
}






