//
// Binary Tree Operations.cpp : Various operations performed on binary trees
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
///                                        FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : performBinaryTreeMediaPageReplication
// Description     : Replicate only the MediaItems in a binary tree with a specified PageID
// 
// BINARY_TREE*  pSourceTree      : [in]  Tree containing the all the strings
// CONST MEDIA_ITEM_TYPE  eType   : [in]  MediaItem type to replicate
// CONST UINT    iPageID          : [in]  PageID of the strings to replicate
// BINARY_TREE*  pDestinationTree : [out] Destination tree to copy matching items into
// 
BearScriptAPI
VOID  performBinaryTreeMediaPageReplication(AVL_TREE*  pSourceTree, CONST MEDIA_ITEM_TYPE  eType, CONST UINT  iPageID, AVL_TREE*  pDestinationTree)
{
   AVL_TREE_OPERATION*   pOperationData;

   // [CHECK] - Destination tree must not be a storage tree, since a 'deep copy' is not performed
   //ASSERT(pSourceTree->eType == AT_MEDIA_ITEM AND pDestinationTree->pfnDeleteNode == NULL);
   ASSERT(!pDestinationTree->pfnDeleteNode);

   // Prepare
   pOperationData = createAVLTreeOperation(treeprocReplicateMediaPage, ATT_INORDER);

   // Setup operation
   pOperationData->xFirstInput  = eType;
   pOperationData->xSecondInput = iPageID;
   pOperationData->pOutputTree  = pDestinationTree;

   // Replicate
   performOperationOnAVLTree(pSourceTree, pOperationData);

   // Cleanup
   deleteAVLTreeOperation(pOperationData);
   
   // Cleanup
   deleteAVLTreeOperation(pOperationData);
}


/// Function name  : performBinaryTreeMediaPageTypeCount
// Description     : Counts the number of items of each type in a MediaPages tree
// 
// AVL_TREE*          pTargetTree  : [in]  MediaPages tree containing pages of various types
// UINT*              piOutput     : [out] Array of type counts, must be large enough to hold the count for each possible type
// CONST UINT         iTypeCount   : [in]  Number of types
// 
BearScriptAPI
VOID  performBinaryTreeMediaPageTypeCount(AVL_TREE*  pTargetTree, UINT*  piOutput, CONST UINT  iTypeCount)
{
   AVL_TREE_OPERATION*  pOperationData;

   // Prepare
   pOperationData = createAVLTreeOperation(treeprocCountMediaPageTypes, ATT_INORDER);

   // [CHECK] Must be run on a MediaPages tree
   //ASSERT(pTargetTree->eType == AT_MEDIA_PAGE);

   // Set operation data
   pOperationData->xFirstInput  = iTypeCount;
   pOperationData->xSecondInput = (LPARAM)piOutput;

   // Perform operation
   performOperationOnAVLTree(pTargetTree, pOperationData);
   
   // Cleanup
   deleteAVLTreeOperation(pOperationData);
}




