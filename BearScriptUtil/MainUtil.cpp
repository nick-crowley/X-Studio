//
// MainLib.cpp : Handle creation and destruction of the library
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS  / GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// Low fragmentation process heap
HANDLE       hUtilityHeap  = NULL;     

// Heap flags
#define     REQ_LOW_FRAGMENTATION_HEAP       2

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION  /  DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : DllMain
// Description     : DLL Entry point
//
// HMODULE  hModule    : [in] Instance of loading module?
// DWORD    dwPurpose  : [in] Reason for call
// LPVOID   lpReserved : [in] Ignored
// 
// Return type : TRUE if successful, otherwise FALSE
//
BOOL APIENTRY   DllMain(HMODULE  hModule, DWORD  dwPurpose, LPVOID  lpReserved)
{
   UINT  iOptions;

   // Prepare
   iOptions = REQ_LOW_FRAGMENTATION_HEAP;

   // Examine reason for call
   switch (dwPurpose)
   {
   /// [PROCESS LOAD] - Create heap
   case DLL_PROCESS_ATTACH:
      // Create 32mb heap
      if (hUtilityHeap = HeapCreate(NULL, 32 * MEGABYTE, NULL))
         // [SUCCESS] Request low fragmentation heap
         HeapSetInformation(hUtilityHeap, HeapCompatibilityInformation, &iOptions, sizeof(UINT));
      else
         // [FAILED]
         MessageBox(GetDesktopWindow(), TEXT("There was an unspecified error while attempting to initialise the heap"), TEXT("X-Studio"), MB_OK WITH MB_ICONERROR);
      
      // [CHECK] Return TRUE if heap was created
      return (hUtilityHeap != NULL);
         
   /// [THREAD LOAD/UNLOAD]
   case DLL_THREAD_ATTACH:
   case DLL_THREAD_DETACH:
      break;

   /// [PROCESS UNLOAD] - Destroy heap
   case DLL_PROCESS_DETACH:
      HeapDestroy(hUtilityHeap);
      hUtilityHeap = NULL;
      break;
   }

   // [SUCCESS] Return TRUE
   return TRUE;
}

