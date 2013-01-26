//
// Binary Tree Functors.cpp : Functors used by binary tree operations
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

CONST UINT  iNullGamePageID  =  (UINT)-1;

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        MACROS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////



/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////



/// Function name  : treeprocCountMediaPageTypes
// Description     : Counts the various type of MediaPages
//
// BINARY_TREE_NODE*            pNode          : [in]     Current tree node
// BINARY_TREE_OPERATION_DATA*  pOperationData : [in/out] xFirstInput  - Number of possible 'Types'
//                                                        xSecondInput - 'Type' array
//                                                        
VOID  treeprocCountMediaPageTypes(AVL_TREE_NODE*  pNode, AVL_TREE_OPERATION*  pOperationData)
{
   UINT*            piOutput;        // 'Type' counts array
   UINT             iTypeCount;      // Number of elements in above array
   MEDIA_PAGE*      pMediaPage;      // Convenience pointer

   // Prepare
   pMediaPage = extractPointerFromTreeNode(pNode, MEDIA_PAGE);

   // Extract parameters
   iTypeCount = (UINT)pOperationData->xFirstInput;
   piOutput   = (UINT*)pOperationData->xSecondInput;
   
   /// [CHECK] Output array contains space for this 'Type'
   ASSERT((UINT)pMediaPage->eType < iTypeCount);

   /// Count this node
   piOutput[(UINT)pMediaPage->eType]++;
}




/// Function name  : treeprocReplicateMediaPage
// Description     : Replicates a specified type of MediaItem objects in a binary tree with a matching PageID
// 
// BINARY_TREE_NODE*            pNode          : [in/out] Node to check for a match
// BINARY_TREE_OPERATION_DATA*  pOperationData : [in]     xFirstInput  -- Type to replicate
//                                                        xSecondInput -- PageID to replicate
//                                                        pOutputTree  -- Destination tree
//
///                                              This operation sets no result.
//
VOID   treeprocReplicateMediaPage(AVL_TREE_NODE*  pNode, AVL_TREE_OPERATION*  pOperationData)
{
   MEDIA_ITEM*      pMediaItem;     // Convenience pointer
   MEDIA_ITEM_TYPE  eType;          // Desired MediaItem type
   UINT             iPageID;        // Desired PageID

   // Prepare
   pMediaItem = extractPointerFromTreeNode(pNode, MEDIA_ITEM);
   eType      = (MEDIA_ITEM_TYPE)pOperationData->xFirstInput;
   iPageID    = (UINT)pOperationData->xSecondInput;

   // Compare item for matching type and page ID
   if (pMediaItem->eType == eType AND pMediaItem->iPageID == iPageID)
      // Add to destination tree
      insertObjectIntoAVLTree(pOperationData->pOutputTree, (LPARAM)pMediaItem);
}





