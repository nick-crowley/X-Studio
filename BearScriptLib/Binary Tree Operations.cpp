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



/// Function name  : performBinaryTreeScriptDependenciesOutput
// Description     : Output a ScriptDependencies binary tree into a ListView
// 
// BINARY_TREE*  pTree     : [in] ScriptDependendies binary tree
// HWND          hListView : [in] Window handle of the ListView to fill.  Any existing contents will be erased.
// 
/// RESCINDED: REMOVE
//
BearScriptAPI
VOID   performBinaryTreeScriptDependenciesOutput(AVL_TREE*  pTree, HWND  hListView)
{
   AVL_TREE_OPERATION*  pOperationData;

   // Prepare
   pOperationData = createAVLTreeOperation(treeprocOutputScriptDependencies, ATT_INORDER);
   
   // Set listview data
   pOperationData->xFirstInput = (LPARAM)hListView;

   // Erase existing items
   SendMessage(hListView, LVM_DELETEALLITEMS, NULL, NULL);

   // Perform operation
   performOperationOnAVLTree(pTree, pOperationData);
   
   // Cleanup
   deleteAVLTreeOperation(pOperationData);
}




/// Function name  : performBinaryTreeReplication
// Description     : Attempts to inserts all the nodes in one binary tree into another. This is used for creating
//                    copies of tree sorted by a different property.
// 
// BINARY_TREE*  pSourceTree       : [in]  Tree to replicate
// BINARY_TREE*  pDestinationTree  : [out] Output tree
// 
BearScriptAPI
VOID    performBinaryTreeReplication(AVL_TREE*  pSourceTree, AVL_TREE*  pDestinationTree)
{
   AVL_TREE_OPERATION*   pOperationData;

   // [CHECK] - Destination tree must not be a storage tree, since a 'deep copy' is not performed
   ASSERT(pDestinationTree->pfnDeleteNode == NULL);

   // Prepare
   pOperationData = createAVLTreeOperation(treeprocReplicateBinaryTree, ATT_INORDER);

   // Setup operation
   pOperationData->pOutputTree = pDestinationTree;

   // Replicate tree
   performOperationOnAVLTree(pSourceTree, pOperationData);
   
   // Cleanup
   deleteAVLTreeOperation(pOperationData);
}

/// Function name  : performBinaryTreeStringPageReplication
// Description     : Replicate only the GameStrings in a binary tree with a certain Page ID
// 
// BINARY_TREE*  pSourceTree      : [in]  Tree containing the all the strings
// CONST UINT    iPageID          : [in]  Page ID of the strings to replicate
// BINARY_TREE*  pDestinationTree : [out] Destination tree for those GameStrings with a matching PageID
// 
BearScriptAPI
VOID  performBinaryTreeStringPageReplication(AVL_TREE*  pSourceTree, CONST UINT  iPageID, AVL_TREE*  pDestinationTree)
{
   AVL_TREE_OPERATION*  pOperationData;

   // [CHECK] - Destination tree must not be a storage tree, since a 'deep copy' is not performed
   ASSERT(pDestinationTree->pfnDeleteNode == NULL);

   // Prepare
   pOperationData = createAVLTreeOperation(treeprocReplicateStringPage, ATT_INORDER);

   // Setup operation
   pOperationData->xFirstInput = iPageID;
   pOperationData->pOutputTree = pDestinationTree;

   // Replicate
   performOperationOnAVLTree(pSourceTree, pOperationData);
   
   // Cleanup
   deleteAVLTreeOperation(pOperationData);
}



/// Function name  : performLanguageFileXMLGeneration
// Description     : Fill a LanguageFile's output buffer with the XML equivilient of the current contents of
//                    it's game-string and game-page trees.
// 
// LANGUAGE_FILE*  pLanguageFile      : [in] LanguageFile containing a valid output buffer and a valid input buffer of at least 4k.
// TCHAR*          szConversionBuffer : [in] 4096 character string buffer for performing string conversions
// 
BearScriptAPI
VOID  performLanguageFileXMLGeneration(LANGUAGE_FILE*  pLanguageFile, TCHAR*  szConversionBuffer)
{
   AVL_TREE_OPERATION*   pOperationData;

   // [CHECK] LanguageFile has a valid output buffer
   ASSERT(pLanguageFile->szOutputBuffer);
   ASSERT(pLanguageFile->iOutputSize > 0);

   // Prepare
   pOperationData = createAVLTreeOperation(treeprocGenerateLanguageFileXML, ATT_INORDER);

   // Set operation data
   pOperationData->xFirstInput   = iNullGamePageID;                   // Current PageID -- Ensure initial page ID will be different from the first node encountered.
   pOperationData->xSecondInput  = 0;                                 // Amount of buffer used, in characters
   pOperationData->xThirdInput   = (LPARAM)szConversionBuffer;
   pOperationData->xFourthInput  = (LPARAM)pLanguageFile;

   // Perform
   performOperationOnAVLTree(pLanguageFile->pGameStringsByID, pOperationData);
   
   // Cleanup
   deleteAVLTreeOperation(pOperationData);
}



