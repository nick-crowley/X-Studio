//
// FILE_NAME.cpp : FILE_DESCRIPTION
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         INTERNAL FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// These are used by the 'external' functions but they all require a 'Starting Node' parameter, so for 
//  convenience it's best not to use these directly.  To prevent it happening by mistake I've not included
//  them in BearScriptLib.h
//
BOOL   performOperationOnAVLTreeFromNode(CONST AVL_TREE*  pTree, AVL_TREE_NODE*  pCurrentNode, AVL_TREE_OPERATION*  pOperationData);
BOOL   performThreadedOperationOnAVLTreeFromNode(AVL_TREE_THREAD*  pThreadData, AVL_TREE_NODE*  pCurrentNode, AVL_TREE_OPERATION*  pOperationData);

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        MACROS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Name: extractObjectProperty
//  Descrpition: Extracts a property from a TreeNode
// 
// AVL_TREE*          pTree      : [in] Tree containing the object
// LPARAM             pObject    : [in] Object to extract the property from
// AVL_TREE_SORT_KEY  eProperty  : [in] Property to extract
//
#define    extractObjectProperty(pTree,pObject,eProperty)        (*pTree->pfnExtractValue)(pObject, eProperty)

/// Macro: performOperationFunctor
//  Description: Runs an functor on a TreeNode
//
// AVL_TREE_NODE*       pTargetNode    : [in] Node to run the operation on
// AVL_TREE_OPERATION*  pOperationData : [in] Data for the operation
//
#define    performOperationFunctor(pTargetNode,pOperationData)   (*pOperationData->pfnFunctor)(pTargetNode, pOperationData)

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createAVLTreeGroupCounter
// Description     : Creates a GroupCounter for calculating the sizes of groups within trees
// 
// CONST UINT  iMaxGroups : [in] Maximum number of groups
// 
// Return Value   : New AVLTreeGroupCounter, you are responsible for destroying it
// 
BearScriptAPI
AVL_TREE_GROUP_COUNTER*   createAVLTreeGroupCounter(CONST UINT  iMaxGroups)
{
   AVL_TREE_GROUP_COUNTER*  pGroupCounter;

   // Create object
   pGroupCounter = utilCreateEmptyObject(AVL_TREE_GROUP_COUNTER);

   // Store total number of buckets
   pGroupCounter->iBucketCount = iMaxGroups;

   // Create array to hold occurence counts
   pGroupCounter->piBuckets = utilCreateEmptyObjectArray(UINT, iMaxGroups);

   // Return object
   return pGroupCounter;
}


/// Function name  : createAVLTreeOperation
// Description     : Creates a data object for customised tree operations
// 
// CONST AVL_TREE_FUNCTOR    pfnFunctor  : [in] Operation functor
// CONST AVL_TREE_TRAVERSAL  eOrder      : [in] Type of tree traversal used when performing the operation
// 
// Return Value   : New AVLTreeOperation, you are responsible for destroying it
// 
BearScriptAPI
AVL_TREE_OPERATION*  createAVLTreeOperation(CONST AVL_TREE_FUNCTOR  pfnFunctor, CONST AVL_TREE_TRAVERSAL  eOrder)
{
   AVL_TREE_OPERATION*  pOperation;

   // Create new object
   pOperation = utilCreateEmptyObject(AVL_TREE_OPERATION);

   // Set properties
   pOperation->pfnFunctor = pfnFunctor;
   pOperation->eOrder     = eOrder;

   // Return new object
   return pOperation;
}


/// Function name  : createAVLTreeOperationEx
// Description     : Creates an AVLTreeOperation for use with an existing ErrorQueue
///                         The caller is responsible for setting the correct progress stage and maximum
// 
// CONST AVL_TREE_FUNCTOR    pfnFunctor  : [in]            Operation functor
// CONST AVL_TREE_TRAVERSAL  eOrder      : [in]            Traversal used when performing the operation
// QUEUE*                    pErrorQueue : [in] [optional] Existing ErrorQueue to use
// OPERATION_PROGRESS*       pProgress   : [in] [optional] Progress object with the correct stage already set
// 
// Return Value   : New AVLTreeOperation, you are responsible for destroying it
// 
BearScriptAPI
AVL_TREE_OPERATION*  createAVLTreeOperationEx(CONST AVL_TREE_FUNCTOR  pfnFunctor, CONST AVL_TREE_TRAVERSAL  eOrder, QUEUE*  pErrorQueue, OPERATION_PROGRESS*  pProgress)
{
   AVL_TREE_OPERATION*  pOperation;

   // Create new object
   pOperation = createAVLTreeOperation(pfnFunctor, eOrder);

   // Attach ErrorQueue + progress tracker
   pOperation->pErrorQueue = pErrorQueue;
   pOperation->pProgress   = pProgress;

   // Return new object
   return pOperation;
}


/// Function name  : createAVLTreeThreadedOperation
// Description     : Creates an AVLTreeOperation for use with a progress indicator and multi-threading
///                         The caller is responsible for setting the correct progress stage and maximum
// 
// CONST AVL_TREE_FUNCTOR    pfnFunctor      : [in]            Operation functor
// CONST AVL_TREE_TRAVERSAL  eOrder          : [in]            Type of tree traversal used when performing the operation
// OPERATION_PROGRESS*       pProgress       : [in] [optional] Progress object with the correct stage already set
// CONST UINT                iNodesPerThread : [in]            Minimum number of tree nodes per worker thread
// 
// Return Value   : New AVLTreeOperation, you are responsible for destroying it
// 
BearScriptAPI
AVL_TREE_OPERATION*  createAVLTreeThreadedOperation(CONST AVL_TREE_FUNCTOR  pfnFunctor, CONST AVL_TREE_TRAVERSAL  eOrder, OPERATION_PROGRESS*  pProgress, CONST UINT  iNodesPerThread)
{
   AVL_TREE_OPERATION*  pOperation;

   // Create new object
   pOperation = createAVLTreeOperationEx(pfnFunctor, eOrder, NULL, pProgress);

   // Set properties
   pOperation->bMultiThreaded  = TRUE;
   pOperation->iNodesPerThread = iNodesPerThread;

   // Create mutex
   pOperation->hOutputMutex   = CreateMutex(NULL, FALSE, NULL);
   pOperation->hProgressMutex = CreateMutex(NULL, FALSE, NULL);

   // Return new object
   return pOperation;
}


/// Function name  : deleteAVLTreeGroupCounter
// Description     : Destroys an AVLTreeGroupCounter object
// 
// AVL_TREE_GROUP_COUNTER*  &pGroupCounter   : [in] AVLTreeGroupCounter object to destroy
// 
BearScriptAPI
VOID  deleteAVLTreeGroupCounter(AVL_TREE_GROUP_COUNTER*  &pGroupCounter)
{
   // Delete bucket array
   utilDeleteObject(pGroupCounter->piBuckets);

   // Delete calling object
   utilDeleteObject(pGroupCounter);
}


/// Function name  : deleteAVLTreeOperation
// Description     : Deletes an AVLTreeOperation object
// 
// AVL_TREE_OPERATION*  &pOperation   : [in] Object to delete
// 
BearScriptAPI
VOID  deleteAVLTreeOperation(AVL_TREE_OPERATION*  &pOperation)
{
   // Delete error queue, if any
   if (pOperation->pErrorQueue)
      deleteQueue(pOperation->pErrorQueue);

   // Delete Mutexes, if any
   if (pOperation->hOutputMutex)
      utilCloseHandle(pOperation->hOutputMutex);

   if (pOperation->hProgressMutex)
      utilCloseHandle(pOperation->hProgressMutex);

   // Delete calling object
   utilDeleteObject(pOperation);
}


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : getAVLTreeGroupCount
// Description     : Retrieves the number of nodes in the specified group
// 
// CONST  AVL_TREE_GROUP_COUNTER*  pGroupCounter  : [in] Group counter
// CONST UINT                      iGroup         : [in] Group to retrieve the size of
// 
// Return Value   : Number of nodes in the specified group
// 
BearScriptAPI
UINT  getAVLTreeGroupCount(CONST AVL_TREE_GROUP_COUNTER*  pGroupCounter, CONST UINT  iGroup)
{
   UINT  iOccurences;

   // Prepare
   iOccurences = NULL;

   // [CHECK] Ensure index is valid
   if (iGroup < pGroupCounter->iBucketCount)
      // Lookup occurence count for the specified value
      iOccurences = pGroupCounter->piBuckets[iGroup];
   
   // Return number of occurences
   return iOccurences;
}


/// Function name  : incrementAVLTreeGroupCount
// Description     : Increments the node count for the specified group
// 
// AVL_TREE_GROUP_COUNTER*  pGroupCounter  : [in/out] NodeCounter
// CONST UINT              iGroup        : [in]     Group to increment the node count for
// 
VOID  incrementAVLTreeGroupCount(AVL_TREE_GROUP_COUNTER*  pGroupCounter, CONST UINT  iGroup)
{
   // [CHECK] Ensure index is valid
   if (iGroup < pGroupCounter->iBucketCount)
      // Increment occurence count
      pGroupCounter->piBuckets[iGroup]++;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : analyseTreeBalance
// Description     : Debug function for checking tree balance
// 
// CONST AVL_TREE_NODE*  pNode   : [in] Tree root
// CONST UINT            iDepth  : [in] Starting depth
// 
// Return Value   : String showing the node counts at each depth
// 
BearScriptAPI
TCHAR*  analyseTreeBalance(CONST AVL_TREE_NODE*  pNode, CONST UINT  iDepth)
{
   static UINT  iNodeCount[32];
   TCHAR*       szOutput;

   // [CHECK] Initialise counter array
   if (iDepth == 0)
      utilZeroObjectArray(iNodeCount, UINT, 32);

   if (iDepth < 32)
   {
      // Count current node
      iNodeCount[iDepth]++;

      // Recurse into left child
      if (pNode->pLeft)
         analyseTreeBalance(pNode->pLeft, iDepth + 1);
      // Recurse into right child
      if (pNode->pRight)
         analyseTreeBalance(pNode->pRight, iDepth + 1);
   }

   // [INITIAL CALL] Generate output
   if (iDepth == 0)
   {
      // Prepare
      szOutput = utilCreateEmptyString(1024);

      // Generate output
      for (UINT iLevel = 0; iLevel < 32 AND iNodeCount[iLevel]; iLevel++)
         utilStringCchCatf(szOutput, 1024, TEXT("Depth = %02u : Nodes = %u\r\n"), iLevel, iNodeCount[iLevel]);

      // Return output
      return szOutput;
   }

   // Return nothing
   return NULL;
}


/// Function name  : performAVLTreeIndexing
// Description     : Index the nodes in an AVL tree
// 
// AVL_TREE*  pTree : [in] Tree to index
// 
BearScriptAPI
VOID   performAVLTreeIndexing(AVL_TREE*  pTree)
{
   AVL_TREE_OPERATION*  pData;

   // [CHECK] Don't index an already indexed tree
   if (pTree->bIndexed)
      return;

   // Prepare
   pData = createAVLTreeOperation(treeprocIndexAVLTreeNode, ATT_INORDER);

   // Set parameters
   pData->xFirstInput = 0;

   // Perform operation
   performOperationOnAVLTree(pTree, pData);

   // Mark tree as 'indexed'
   pTree->bIndexed = TRUE;

   // Cleanup
   deleteAVLTreeOperation(pData);
}


/// Function name  : performAVLTreeGroupCount
// Description     : Counts the number of nodes in each group in a Tree
// 
// CONST AVL_TREE*     pTree         : [in]     Tree containing the nodes to count
// TREE_NODE_COUNTER*  pGroupCounter : [in/out] NodeCounter defining the total number of groups
// AVL_TREE_SORT_KEY   eProperty     : [in]     Node property that defines it's group
// 
BearScriptAPI
VOID  performAVLTreeGroupCount(CONST AVL_TREE*  pTree, AVL_TREE_GROUP_COUNTER*  pGroupCounter, AVL_TREE_SORT_KEY  eProperty)
{
   AVL_TREE_OPERATION*  pOperationData;

   // Prepare
   pOperationData = createAVLTreeOperation(treeprocGenerateAVLTreeNodeGroupCounts, ATT_INORDER);

   // Set parameters
   pOperationData->xFirstInput  = (LPARAM)pGroupCounter;
   pOperationData->xSecondInput = (LPARAM)eProperty;
   pOperationData->pInputTree   = pTree;

   // Perform operation
   performOperationOnAVLTree(pTree, pOperationData);

   // Cleanup
   deleteAVLTreeOperation(pOperationData);
}


/// Function name  : performOperationOnAVLTree
// Description     : Helper for running single/multi threaded operations on AVLTrees
// 
// AVL_TREE*            pTree          : [in/out] Tree to perform the operation on  (Empty trees will return FALSE)
// AVL_TREE_OPERATION*  pOperationData : [in/out] Defines the operation, traversal, input/output parameters and the result
// 
// Return Value   : TRUE/FALSE, althought exact result depends on the operation
// 
BearScriptAPI
BOOL  performOperationOnAVLTree(CONST AVL_TREE*  pTree, AVL_TREE_OPERATION*  pOperationData)
{
   // [CHECK] Ensure tree and operation data exist
   ASSERT(pTree AND pOperationData);

   // Prepare
   pOperationData->bResult = FALSE;

   // [CHECK] Are enough nodes present for it to be worthwhile to multithread?
   if (pOperationData->bMultiThreaded AND getTreeNodeCount(pTree) >= pOperationData->iNodesPerThread)
      /// [MULTI-THREADED] Pass to threaded version
      performThreadedOperationOnAVLTree(pTree, pOperationData);

   // [CHECK] Do nothing if tree is empty
   else if (getTreeNodeCount(pTree))
   {
      // Set processing flag
      pOperationData->bProcessing = TRUE;

      /// [SINGLE-THREADED] Perform operation from the root
      performOperationOnAVLTreeFromNode(pTree, pTree->pRoot, pOperationData);

      // Remove processing flag
      pOperationData->bProcessing = FALSE;
   }

   // Return operation result
   return pOperationData->bResult;
}


/// Function name  : performOperationOnAVLTreeFromNode
// Description     : Performs a customised operation on each node of an AVL tree during a pre-order, in-order or post-order traversal.  Can be used to do
//                      practically anything to the nodes in a tree, except add/remove them.
// 
// AVL_TREE*            pTree        : [in/out] Tree to perform the operation on
// AVL_TREE_NODE*       pCurrentNode : [in/out] Node currently being operation on
// AVL_TREE_OPERATION*  pOperationData : [in/out] Defines the operation itself, the order it's run, it's input and output parameters and the operation result
// 
// Return Value   : TRUE/FALSE, althought exact result depends on the operation
// 
BOOL   performOperationOnAVLTreeFromNode(CONST AVL_TREE*  pTree, AVL_TREE_NODE*  pCurrentNode, AVL_TREE_OPERATION*  pOperationData)
{
   // [CHECK] Abort processing if the processing flag has been turned off
   if (pOperationData->bProcessing == FALSE)
      return pOperationData->bResult;

   /// [PRE-ORDER] Perform operation before recursing
   if (pOperationData->eOrder == ATT_PREORDER)
   {
      // Perform operation
      performOperationFunctor(pCurrentNode, pOperationData);

      // [CHECK] Are progress updates required?
      if (pOperationData->pProgress)
         // [SUCCESS] Increment progress
         advanceOperationProgressValue(pOperationData->pProgress);
   }

   /// Recurse left
   if (pCurrentNode->pLeft)
      performOperationOnAVLTreeFromNode(pTree, pCurrentNode->pLeft, pOperationData);

   /// [IN-ORDER] Perform operation between recursing
   if (pOperationData->eOrder == ATT_INORDER)
   {
      // Perform operation
      performOperationFunctor(pCurrentNode, pOperationData);

      // [CHECK] Are progress updates required?
      if (pOperationData->pProgress)
         // [SUCCESS] Increment progress
         advanceOperationProgressValue(pOperationData->pProgress);
   }

   /// Recurse right
   if (pCurrentNode->pRight)
      performOperationOnAVLTreeFromNode(pTree, pCurrentNode->pRight, pOperationData);

   /// [POST-ORDER] Perform operation after recursing
   if (pOperationData->eOrder == ATT_POSTORDER)
   {
      // Perform operation
      performOperationFunctor(pCurrentNode, pOperationData);

      // [CHECK] Are progress updates required?
      if (pOperationData->pProgress)
         // [SUCCESS] Increment progress
         advanceOperationProgressValue(pOperationData->pProgress);
   }

   // Return operation result
   return pOperationData->bResult;
}



/// Function name  : performThreadedOperationOnAVLTree
// Description     : Perform multi-threaded operations on AVLTrees
// 
// AVL_TREE*            pTree          : [in/out] Tree to perform the operation on. If this is empty the result will be FALSE.
// AVL_TREE_OPERATION*  pOperationData : [in/out] Defines the operation itself, the order it's run, it's input and output parameters and the operation result
// 
// Return Value   : TRUE/FALSE, althought exact result depends on the operation
// 
BOOL  performThreadedOperationOnAVLTree(CONST AVL_TREE*  pTree, AVL_TREE_OPERATION*  pOperationData)
{
   UINT              iThreadCount,        // Total number of threads
                     iWorkerCount;        // Number of worker threads
   AVL_TREE_THREAD*  oThreadData;         // Data for each worker thread plus the current thread
   HANDLE*           hThreadHandles;      // Handles for each worker thread

   // [CHECK] Ensure tree/data exist and tree is not tiny
   ASSERT(pTree AND pOperationData AND getTreeNodeCount(pTree) >= pOperationData->iNodesPerThread);

   /// Calculate number of threads to use  (minimum of 2, Maximum of 16)
   iThreadCount = min(16, max(2, getTreeNodeCount(pTree) / pOperationData->iNodesPerThread));
   iWorkerCount = iThreadCount - 1;

   // Create thread data for 'n' threads and handles for 'n-1' threads
   oThreadData    = utilCreateEmptyObjectArray(AVL_TREE_THREAD, iThreadCount);
   hThreadHandles = utilCreateEmptyObjectArray(HANDLE, iWorkerCount);

   // Prepare thread data
   for (UINT iIndex = 0; iIndex < iThreadCount; iIndex++)
   {
      // Set thread data
      oThreadData[iIndex].iThreadCount   = iThreadCount;
      oThreadData[iIndex].iThreadIndex   = iIndex;
      // Set operation data
      oThreadData[iIndex].pOperationData = pOperationData;
      oThreadData[iIndex].pStartNode     = pTree->pRoot;
   }

   // Set processing flag
   pOperationData->bProcessing = TRUE;

   /// Launch worker threads and store their handles
   for (UINT iIndex = 0; iIndex < iWorkerCount; iIndex++)
      hThreadHandles[iIndex] = utilLaunchThread(threadprocAVLSubTreeOperation, &oThreadData[iIndex]);

   /// Perform operation on current thread
   performThreadedOperationOnAVLTreeFromNode(&oThreadData[iWorkerCount], pTree->pRoot, pOperationData);

   /// [WAIT] Ensure worker threads have completed before returing
   WaitForMultipleObjects(iWorkerCount, hThreadHandles, TRUE, INFINITE);

   // Remove processing flag
   pOperationData->bProcessing = FALSE;

   // Free thread handles
   for (UINT iIndex = 0; iIndex < iWorkerCount; iIndex++)
      utilCloseHandle(hThreadHandles[iIndex]);

   // Cleanup and return result
   utilDeleteObject(oThreadData);
   utilDeleteObject(hThreadHandles);
   return pOperationData->bResult;
}


/// Function name  : performThreadedOperationOnAVLTreeFromNode
// Description     : Performs a customised operation on each node of an AVL tree during a pre-order, in-order or post-order traversal.  Can be used to do
//                      practically anything to the nodes in a tree, except add/remove them.
// 
// AVL_TREE*            pTree          : [in/out] Tree to perform the operation on
// AVL_TREE_NODE*       pCurrentNode   : [in/out] Node currently being operation on
// AVL_TREE_OPERATION*  pOperationData : [in/out] Defines the operation itself, the order it's run, it's input and output parameters and the operation result
// CONST UINT           iThreadIndex   : [in]     Thread index
// 
// Return Value   : TRUE/FALSE, althought exact result depends on the operation
// 
BOOL   performThreadedOperationOnAVLTreeFromNode(AVL_TREE_THREAD*  pThreadData, AVL_TREE_NODE*  pCurrentNode, AVL_TREE_OPERATION*  pOperationData)
{
   // [CHECK] Ensure the 'abort processing' flag has not been set
   if (pOperationData->bProcessing)
   {
      // [PRE-ORDER] Perform operation before recursing
      if (pOperationData->eOrder == ATT_PREORDER)
      {
         /// [CHECK] Does node belong to this thread?
         if (pThreadData->iNodeIndex % pThreadData->iThreadCount == pThreadData->iThreadIndex)
         {
            /// [SUCCESS] Perform operation
            performOperationFunctor(pCurrentNode, pOperationData);

            // [CHECK] Are progress updates required?
            if (pOperationData->pProgress)
            {
               /// [SYNC] Wait for access to progress object before updating
               WaitForSingleObject(pOperationData->hProgressMutex, INFINITE);
               advanceOperationProgressValue(pOperationData->pProgress);
               ReleaseMutex(pOperationData->hProgressMutex);
            }
         }

         // Update thread progress
         pThreadData->iNodeIndex++;
      }

      // Recurse left
      if (pCurrentNode->pLeft)
         performThreadedOperationOnAVLTreeFromNode(pThreadData, pCurrentNode->pLeft, pOperationData);

      // [IN-ORDER] Perform operation between recursing
      if (pOperationData->eOrder == ATT_INORDER)
      {
         /// [CHECK] Does node belong to this thread?
         if (pThreadData->iNodeIndex % pThreadData->iThreadCount == pThreadData->iThreadIndex)
         {
            /// [SUCCESS] Perform operation
            performOperationFunctor(pCurrentNode, pOperationData);

            // [CHECK] Are progress updates required?
            if (pOperationData->pProgress)
            {
               /// [SYNC] Wait for access to progress object before updating
               WaitForSingleObject(pOperationData->hProgressMutex, INFINITE);
               advanceOperationProgressValue(pOperationData->pProgress);
               ReleaseMutex(pOperationData->hProgressMutex);
            }
         }

         // Update thread progress
         pThreadData->iNodeIndex++;
      }

      // Recurse right
      if (pCurrentNode->pRight)
         performThreadedOperationOnAVLTreeFromNode(pThreadData, pCurrentNode->pRight, pOperationData);

      // [POST-ORDER] Perform operation after recursing
      if (pOperationData->eOrder == ATT_POSTORDER)
      {
         /// [CHECK] Does node belong to this thread?
         if (pThreadData->iNodeIndex % pThreadData->iThreadCount == pThreadData->iThreadIndex)
         {
            /// [SUCCESS] Perform operation
            performOperationFunctor(pCurrentNode, pOperationData);

            // [CHECK] Are progress updates required?
            if (pOperationData->pProgress)
            {
               /// [SYNC] Wait for access to progress object before updating
               WaitForSingleObject(pOperationData->hProgressMutex, INFINITE);
               advanceOperationProgressValue(pOperationData->pProgress);
               ReleaseMutex(pOperationData->hProgressMutex);
            }
         }

         // Update thread progress
         pThreadData->iNodeIndex++;
      }
   }

   // Return operation result
   return pOperationData->bResult;
}


/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         TREE OPERATIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : treeprocIndexAVLTreeNode
// Description     : Index the input node and increment the current index
// 
// AVL_TREE_NODE*       pNode   : [in] Node to index
// AVL_TREE_OPERATION*  pData   : [in] Operation data
///                                      UINT  xFirstInput : Current index
// 
VOID  treeprocIndexAVLTreeNode(AVL_TREE_NODE*  pNode, AVL_TREE_OPERATION*  pData)
{
   UINT&  iCurrentIndex = (UINT&)pData->xFirstInput;

   /// Index the input node and increment the current index
   pNode->iIndex = iCurrentIndex++;
}


/// Function name  : treeprocReplicateAVLTreeNode
// Description     : Index the input node and increment the current index
// 
// AVL_TREE_NODE*       pNode   : [in] Node to index
// AVL_TREE_OPERATION*  pData   : [in] Operation data
///                                      AVL_TREE*  pOutputTree : AVLTree to replicate the current node into
// 
BearScriptAPI
VOID  treeprocReplicateAVLTreeNode(AVL_TREE_NODE*  pNode, AVL_TREE_OPERATION*  pData)
{
   /// Insert the object in the input node into the output tree
   insertObjectIntoAVLTree(pData->pOutputTree, pNode->pData);
}


/// Function name  : treeprocCountNodesByProperty
// Description     : Counts the number of nodes in each group of an AVLTree
//
// AVL_TREE_NODE*       pNode          : [in]     Current tree node
// AVL_TREE_OPERATION*  pOperationData : [in/out] xFirstInput  - NodeGroup counter
//                                                xSecondInput - Property that indicates the group
//                                                        
VOID   treeprocGenerateAVLTreeNodeGroupCounts(AVL_TREE_NODE*  pNode, AVL_TREE_OPERATION*  pOperationData)
{
   AVL_TREE_GROUP_COUNTER*  pGroupCounter;         // GroupCounter object
   AVL_TREE_SORT_KEY        eGroupProperty;        // Property which defines the group
   UINT                     iNodeGroup;            // Group of the input node

   // Prepare
   pGroupCounter  = (AVL_TREE_GROUP_COUNTER*)pOperationData->xFirstInput;
   eGroupProperty = (AVL_TREE_SORT_KEY)pOperationData->xSecondInput;

   // Extract property containing the group
   iNodeGroup = (UINT)extractObjectProperty(pOperationData->pInputTree, pNode->pData, eGroupProperty);

   // Increment node count for current group
   incrementAVLTreeGroupCount(pGroupCounter, iNodeGroup);
}


/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         THREAD OPERATIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : threadprocAVLSubTreeOperation
// Description     : Performs an AVLTree operation on the specified sub-tree
// 
// VOID*  pParameter   : [in] Threaded tree operation data
// 
DWORD   threadprocAVLSubTreeOperation(VOID*  pParameter)
{
   AVL_TREE_THREAD*  pThreadData;
   ERROR_STACK*      pException;
   CHAR              szThreadNameA[32];

   // Prepare
   pThreadData = (AVL_TREE_THREAD*)pParameter;

   // [DEBUG] Set thread name
   StringCchPrintfA(szThreadNameA, 32, "AVLTree Worker (%u of %u)", pThreadData->iThreadIndex + 1, pThreadData->iThreadCount - 1);
   SET_THREAD_NAME(szThreadNameA);
   setThreadLanguage(getAppPreferences()->eAppLanguage);

   __try
   {
      // [COM]
      CoInitializeEx(NULL, COINIT_MULTITHREADED);

      /// Process tree using current thread
      performThreadedOperationOnAVLTreeFromNode(pThreadData, pThreadData->pStartNode, pThreadData->pOperationData);

      // [COM]
      CoUninitialize();
   }
   /// [EXCEPTION HANDLER]
   __except (generateExceptionError(GetExceptionInformation(), pException))
   {
      // [ERROR] "An unidentified and unexpected critical error has occurred in the AVLTree Worker (%u of %u)"
      enhanceError(pException, ERROR_ID(IDS_EXCEPTION_TREE_WORKER_THREAD), pThreadData->iThreadIndex + 1, pThreadData->iThreadCount - 1);
      consolePrintError(pException);
      deleteErrorStack(pException);
   }

   // Return
   return THREAD_RETURN;
}

