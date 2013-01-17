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


/// Function name  : treeprocGenerateLanguageFileXML
// Description     : Generate the XML language file tags to represent a node in a LanguageFile's GameString tree.
//                     Also generates the tag for the current page, if necessary.  Since the tree is organised
//                     by PageIDs the simplest method is to output a new page tag every time the page changes
//                     and output a string at every node.
// 
// BINARY_TREE_NODE*            pCurrentNode   : [in]     Current GameString tree node
// BINARY_TREE_OPERATION_DATA*  pOperationData : [in/out] xFirstInput  - The ID of the current page
//                                                        xSecondInput - Amount of buffer used, in characters
//                                                        xThirdInput  - String conversion buffer (4,096 chars)
//                                                        xFourthInput - LanguageFile
// 
VOID  treeprocGenerateLanguageFileXML(AVL_TREE_NODE*  pNode, AVL_TREE_OPERATION*  pOperationData)
{
   LANGUAGE_FILE*  pLanguageFile;      // Convenience pointer for input LanguageFile
   GAME_STRING*    pGameString;        // Convenience pointer for current node's GameString
   GAME_PAGE*      pGamePage;          // GamePage for the new page, if one is required
   TCHAR*          szConversionBuffer; // 4,096 character buffer used to converting GameStrings to external without modifying them

   // Create references
   LPARAM&  iCurrentPageID = pOperationData->xFirstInput;    // Reference to the input data's 'current page ID'
   LPARAM&  iBufferUsed    = pOperationData->xSecondInput;     

   // Extract parameters
   pLanguageFile      = (LANGUAGE_FILE*)pOperationData->xFourthInput;
   pGameString        = extractPointerFromTreeNode(pNode, GAME_STRING);
   szConversionBuffer = (TCHAR*)pOperationData->xThirdInput;
   
   /// [NEW PAGE] Close the existing page and start a new one
   if (iCurrentPageID != pGameString->iPageID)
   {
      // Close the current page (unless this is the first page)
      if (iCurrentPageID != iNullGamePageID)
      {
         StringCchCopy(&pLanguageFile->szOutputBuffer[iBufferUsed], pLanguageFile->iOutputSize - iBufferUsed, TEXT("</page>\r\n\r\n"));
         iBufferUsed += lstrlen(TEXT("</page>\r\n\r\n"));
      }

      // Update the current page ID and retrieve it's GamePage object
      iCurrentPageID = pGameString->iPageID;
      findLanguageFileGamePageByID(pLanguageFile, iCurrentPageID, pGamePage);

      /// Assemble into 'Page tag' inside the output buffer
      StringCchPrintf(&pLanguageFile->szOutputBuffer[iBufferUsed], pLanguageFile->iOutputSize - iBufferUsed, 
                      TEXT("<page id=\"%u\" title=\"%s\" descr=\"%s\" voice=\"%s\">\r\n"), 
                      pGamePage->iPageID, pGamePage->szTitle, pGamePage->szDescription ? pGamePage->szDescription : TEXT(""), pGamePage->bVoice ? TEXT("yes") : TEXT("no"));

      // Increment amount of buffer used
      iBufferUsed += lstrlen(&pLanguageFile->szOutputBuffer[iBufferUsed]);
   }

   // [CHECK] GameString is ST_INTERNAL
   ASSERT(pGameString->eType == ST_INTERNAL);

   /// Create a copy of the GameString as ST_EXTERNAL
   StringCchCopy(szConversionBuffer, 4096, pGameString->szText);
   performStringConversion(szConversionBuffer, 4096, ST_INTERNAL, ST_EXTERNAL);

   /// Assemble into a 'string tag' inside the 'Output Buffer'
   StringCchPrintf(&pLanguageFile->szOutputBuffer[iBufferUsed], pLanguageFile->iOutputSize - iBufferUsed, 
                   TEXT("  <t id=\"%u\">%s</t>\r\n"), pGameString->iID, szConversionBuffer);
   
   // Increment buffer used
   iBufferUsed += lstrlen(&pLanguageFile->szOutputBuffer[iBufferUsed]);

   /// [CHECK] Re-allocate output buffer if more than 80% full
   if ((UINT)iBufferUsed > utilCalculatePercentage(pLanguageFile->iOutputSize, 80))
   {
      pLanguageFile->szOutputBuffer = utilExtendString(pLanguageFile->szOutputBuffer, pLanguageFile->iOutputSize, pLanguageFile->iOutputSize * 2);
      pLanguageFile->iOutputSize *= 2;
   }
}


/// Function name  : treeprocIndexBinaryTreeObjects
// Description     : Sequentially number each object in a binary tree
// 
// BINARY_TREE_NODE*            pNode          : [in/out] Current node to number
// BINARY_TREE_OPERATION_DATA*  pOperationData : [in/out] Data containing the last node number
//
///                                              NOTE: This operation sets no result
//
VOID   treeprocIndexBinaryTreeObjects(AVL_TREE_NODE*  pNode, AVL_TREE_OPERATION*  pOperationData)
{
   pNode->iIndex = pOperationData->xFirstInput++;
}


/// Function name  : treeprocOutputScriptDependencies
// Description     : Output a ScriptDependencies binary tree node into the specified ListView as a new item
// 
// AVL_TREE_NODE*       pCurrentNode   : [in] Node to output
// AVL_TREE_OPERATION*  pOperationData : [in] xFirstInput - Window handle of the destination ListView
// 
/// RESCINDED: REMOVE
//
VOID   treeprocOutputScriptDependencies(AVL_TREE_NODE*  pCurrentNode, AVL_TREE_OPERATION*  pOperationData)
{
   SCRIPT_DEPENDENCY*  pDependency;
   TCHAR               szCallCount[4];
   HWND                hListView;
   LVITEM              oItem;

   // Prepare convenience pointers
   hListView   = (HWND)pOperationData->xFirstInput;
   pDependency = extractPointerFromTreeNode(pCurrentNode, SCRIPT_DEPENDENCY);

   // Prepare
   oItem.mask = LVIF_TEXT WITH LVIF_IMAGE;
   StringCchPrintf(szCallCount, 4, TEXT("%u"), pDependency->iCalls);

   // Define 'Script Name'
   oItem.pszText  = pDependency->szScriptName;
   oItem.iItem    = 32768;
   oItem.iSubItem = 0;
   oItem.iImage   = NULL;

   // Insert item
   oItem.iItem = ListView_InsertItem(hListView, &oItem);

   // Define 'Call Count' sub-item
   oItem.iSubItem = 1;
   oItem.pszText = szCallCount;
   
   ListView_SetItem(hListView, &oItem);
}



/// Function name  : treeprocReplicateBinaryTree
// Description     : Attempts to inserts all the nodes in one binary tree into another. This is used for creating
//                    copies of tree sorted by a different property.
//
// BINARY_TREE_NODE*            pNode          : [in] Current node to copy
// BINARY_TREE_OPERATION_DATA*  pOperationData : [in] Data containing the destination tree
//
///                                              NOTE: This operation sets no result
//
VOID   treeprocReplicateBinaryTree(AVL_TREE_NODE*  pNode, AVL_TREE_OPERATION*  pOperationData)
{
   // [CHECK] - Destination tree must not be a storage tree, since a 'deep copy' is not performed
   ASSERT(pOperationData->pOutputTree->pfnDeleteNode == NULL);

   // Insert into display tree
   insertObjectIntoAVLTree(pOperationData->pOutputTree, pNode->pData);
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


/// Function name  : treeprocReplicateStringPage
// Description     : Replicates the GameStrings objects in a binary tree that match a specified page ID
// 
// BINARY_TREE_NODE*            pNode          : [in/out] Node to check for a match
// BINARY_TREE_OPERATION_DATA*  pOperationData : [in]     xFirstInput -- Page ID to replicate
//                                                        pTree       -- Destination tree
//
///                                              This operation sets no result.
//
VOID   treeprocReplicateStringPage(AVL_TREE_NODE*  pNode, AVL_TREE_OPERATION*  pOperationData)
{
   GAME_STRING*   pGameString;
   UINT           iTargetPageID;

   // [CHECK] - Destination tree must not be a storage tree, since a 'deep copy' is not performed
   ASSERT(pOperationData->pOutputTree->pfnDeleteNode == NULL);

   // Prepare
   pGameString   = extractPointerFromTreeNode(pNode, GAME_STRING);
   iTargetPageID = pOperationData->xFirstInput;

   /// Compare node's PageID against the input PageID
   if (pGameString->iPageID == pOperationData->xFirstInput)
      // [SUCCESS] Insert into destination tree
      insertObjectIntoAVLTree(pOperationData->pOutputTree, (LPARAM&)pGameString);
}



