//
// Script Dependencies Tree.cpp : Represents the script dependencies of a script document
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createScriptDependency
// Description     : Create a new ScriptDependency object
// 
// CONST TCHAR*  szScriptName : [in] Nae of the script to create the dependency object for
// 
// Return Value   : New dependency object with a call count of '1'
// 
BearScriptAPI 
SCRIPT_DEPENDENCY*  createScriptDependency(CONST TCHAR*  szScriptName)
{
   SCRIPT_DEPENDENCY*  pNewDependency;

   // Create new dependency
   pNewDependency = utilCreateEmptyObject(SCRIPT_DEPENDENCY);

   // Set script name
   StringCchCopy(pNewDependency->szScriptName, 128, szScriptName);

   // Set initial call count to 1
   pNewDependency->iCalls = 1;

   // Return
   return pNewDependency;
}


/// Function name  : createScriptDependencyTreeByText
// Description     : Creates a ScriptDependency tree sorted by script name
// 
// Return Value   : New AVLTree, you are responsible for destroying it
// 
BearScriptAPI
AVL_TREE*  createScriptDependencyTreeByText()
{
   // Create tree of ScriptDependency indexed by name and in alphabetical order
   return createAVLTree(extractScriptDependencyTreeNode, deleteScriptDependencyTreeNode, createAVLTreeSortKey(AK_TEXT, AO_DESCENDING), NULL, NULL);
}


/// Function name  : deleteScriptDependencyTreeNode
// Description     : Deletes a ScriptDependency within an AVLTree Node
// 
// LPARAM  pScriptDependency : SCRIPT_DEPENDENCY*& : [in] Reference to a ScriptDependency pointer
// 
VOID   deleteScriptDependencyTreeNode(LPARAM  pScriptDependency)
{
   // Delete node contents
   utilDeleteObject((SCRIPT_DEPENDENCY*&)pScriptDependency);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : extractScriptDependencyTreeNode
// Description     : Extract the desired property from a given TreeNode containing a ScriptDependency
// 
// LPARAM                   pNodeData  : [in] SCRIPT_DEPENDENCY* : Node data representing a ScriptDependency
// CONST AVL_TREE_SORT_KEY  eProperty  : [in] Property to retrieve
// 
// Return Value   : Value of the specified property or NULL if the property is unsupported
// 
LPARAM  extractScriptDependencyTreeNode(LPARAM  pNodeData, CONST AVL_TREE_SORT_KEY  eProperty)
{
   SCRIPT_DEPENDENCY*  pScriptDependency;    // Convenience pointer
   LPARAM              xOutput;              // Property value output

   // Prepare
   pScriptDependency = (SCRIPT_DEPENDENCY*)pNodeData;

   // Examine property
   switch (eProperty)
   {
   case AK_TEXT:     xOutput = (LPARAM)pScriptDependency->szScriptName;   break;
   case AK_COUNT:    xOutput = (LPARAM)pScriptDependency->iCalls;         break;
   default:          xOutput = NULL;                                      break;
   }

   // Return property value
   return xOutput;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : insertScriptDependencyIntoAVLTree
// Description     : Attempts to inserts a ScriptDependency into a VariableNames tree.
//                    If the name is already present then it's 'occurrence count' is incremented
// 
// AVL_TREE*     pTree        : [in/out] Tree to add script name to
// CONST TCHAR*  szScriptName : [in]     Name of the script name to insert/increment
//
BearScriptAPI
VOID  insertScriptDependencyIntoAVLTree(AVL_TREE*  pTree, CONST TCHAR*  szScriptName)
{
   SCRIPT_DEPENDENCY*  pScriptDependency;

   // [CHECK] Check whether variable has been encountered before
   if (findObjectInAVLTreeByValue(pTree, (LPARAM)szScriptName, NULL, (LPARAM&)pScriptDependency))
      /// [EXISTING] Increase it's 'occurrence count'
      pScriptDependency->iCalls++;

   else
      /// [NEW] Create a ScriptDependency and add it to the tree
      insertObjectIntoAVLTree(pTree, (LPARAM)createScriptDependency(szScriptName));
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   THREAD FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : threadprocFindCallingScripts
// Description     : Searches the script folder for external dependencies of a given script
// 
// VOID*  pParameters   : OPERATION_DATA structure containing:
//                        ->   szScriptName : [in]     Name of the script whose external dependencies should be calculated
//                        ->   pCallersTree : [in/out] ScriptDependencies tree to store result in.  (Already created)
//
// Return type : Ignored
// Operation Stages : ONE (Searching)
//
// Operation Result : OR_SUCCESS - Always
//                    OR_FAILURE - Never
//                    OR_ABORTED - Never
// 
BearScriptAPI
DWORD   threadprocFindCallingScripts(VOID*  pParameter)
{
   SCRIPTCALL_OPERATION*  pOperationData;      // Operation data
   AVL_TREE_OPERATION*    pTreeOperation;
   OPERATION_RESULT       eResult;             // Operation result
   FILE_SEARCH*           pFileSearch;         // File search containing all scripts in the script folder
   TCHAR*                 szScriptFolder;      // Script folder
   
   // [TRACKING]
   TRACK_FUNCTION();
   VERBOSE_LIB_COMMAND();
   SET_THREAD_NAME("ScriptCall Search");

   // [CHECK] Ensure parameter exists
   ASSERT(pParameter);
   
   // Prepare
   pOperationData = (SCRIPTCALL_OPERATION*)pParameter;
   eResult        = OR_SUCCESS;

   /// [GUARD BLOCK]
   __try
   {
      // Generate script path and output tree
      szScriptFolder = generateGameDataFilePath(GFI_SCRIPT_FOLDER, getAppPreferences()->szGameFolder, getAppPreferences()->eGameVersion);

      // [INFO] "Searching for scripts that depend upon '%s'..."
      pushErrorQueue(pOperationData->pErrorQueue, generateDualInformation(HERE(IDS_OUTPUT_SEARCHING_FOLDER), pOperationData->szScriptName));
      VERBOSE_SMALL_PARTITION();

      /// [SEARCH] Search for all XML/PCK MSCI scripts
      pFileSearch = performFileSystemSearch(getFileSystem(), szScriptFolder, FF_SCRIPT_FILES, AK_PATH, AO_DESCENDING, NULL);

      // [PROGRESS] Define progress as number of scripts found
      ASSERT(getOperationProgressStageID(pOperationData->pProgress) == IDS_PROGRESS_SEARCHING_SCRIPTS);
      updateOperationProgressMaximum(pOperationData->pProgress, getFileSearchResultCount(pFileSearch));

      // Create operation
      pTreeOperation = createAVLTreeThreadedOperation(treeprocSearchCallingScript, ATT_INORDER, pOperationData->pProgress, 40);
      pTreeOperation->pErrorQueue = pOperationData->pErrorQueue;

      // Setup operation
      pTreeOperation->xFirstInput  = (LPARAM)pOperationData->szScriptName;
      pTreeOperation->xSecondInput = (LPARAM)pOperationData->hParentWnd;
      pTreeOperation->pOutputTree  = pOperationData->pCallersTree;

      /// Perform operation
      performOperationOnAVLTree(pFileSearch->pResultsTree, pTreeOperation);

      // Cleanup
      pTreeOperation->pErrorQueue = NULL;
      deleteAVLTreeOperation(pTreeOperation);

      /// [COMPLETE] Index output tree
      performAVLTreeIndexing(pOperationData->pCallersTree);

      // Cleanup
      utilDeleteString(szScriptFolder);
   }
   /// [EXCEPTION HANDLER]
   __except (generateQueuedExceptionError(GetExceptionInformation(), pOperationData->pErrorQueue))
   {
      // [ERROR] "An unidentified and unexpected critical error has occurred while searching for scripts that depend upon '%s'"
      enhanceLastError(pOperationData->pErrorQueue, ERROR_ID(IDS_EXCEPTION_SEARCH_SCRIPT_CALLS), pOperationData->szScriptName);
      
      // [FAILURE]
      eResult = OR_FAILURE;
   }

   // Return result
   VERBOSE_THREAD_COMPLETE("SCRIPT CALL SEARCH WORKER THREAD COMPLETED");
   closeThreadOperation(pOperationData, eResult);
   END_TRACKING();
   return THREAD_RETURN;
}


/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                   TREE OPERATIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : treeprocSearchCallingScript
// Description     : Determines whether the script has any calls to a specific script
// 
// AVL_TREE_NODE*       pCurrentNode   : [in]     Current node containing a VirtualFile
// AVL_TREE_OPERATION*  pOperationData : [in/out] Operation Data : 
///                                                 CONST TCHAR* xFirstInput  : [in]  Desired script name
///                                                 HWND         hParentWnd   : [in]  Parent window
///                                                 AVL_TREE*    pOutputTree  : [out] Output tree
// 
VOID  treeprocSearchCallingScript(AVL_TREE_NODE*  pCurrentNode, AVL_TREE_OPERATION*  pOperationData)
{
   CONST TCHAR   *szScriptCallTarget,     // Target of current script-call
                 *szDesiredTarget;        // Desired target script
   FILE_ITEM*     pCurrentFile;           // File being processed
   COMMAND*       pCommand;               // Command currently being processed in the current script
   SCRIPT_FILE*   pScriptFile;            // ScriptFile containing the script currently being translated
   ERROR_QUEUE*   pTranslationErrors;     // Dummy errorQueue used in script translation - ignored.
   HWND           hParentWnd;
   
   // Extract parameters
   pCurrentFile    = extractPointerFromTreeNode(pCurrentNode, FILE_ITEM);
   szDesiredTarget = (CONST TCHAR*)pOperationData->xFirstInput;
   hParentWnd      = (HWND)pOperationData->xSecondInput;

   // [CHECK] Skip folders
   if (utilExcludes(pCurrentFile->iAttributes, FIF_FOLDER))
   {
      // Create temporary ScriptFile and error queue
      pTranslationErrors = createErrorQueue();
      pScriptFile        = createScriptFileByOperation(SFO_TRANSLATION, pCurrentFile->szFullPath);
      
      /// [LOAD] Attempt to load script - pass operation error queue
      if (!loadGameFileFromFileSystemByPath(getFileSystem(), pScriptFile, NULL, pOperationData->pErrorQueue))
         // [ERROR] "The MSCI script '%s' is unavailable or could not be accessed"
         enhanceLastError(pOperationData->pErrorQueue, ERROR_ID(IDS_SCRIPT_LOAD_IO_ERROR), pCurrentFile->szDisplayName);

      /// [TRANSLATE] Attempt to translate script - pass dummy error queue
      else if (translateScript(pScriptFile, FALSE, hParentWnd, NULL, pTranslationErrors) == OR_SUCCESS)
      {
         // [SUCCESS] Iterate through translated commands
         for (UINT  iIndex = 0; findCommandInTranslatorOutput(pScriptFile->pTranslator, iIndex, pCommand); iIndex++)
         {
            // [CHECK] Is this a script-call?  Does it target the input script?
            if (findScriptCallTargetInCommand(pCommand, szScriptCallTarget) AND utilCompareStringVariables(szDesiredTarget, szScriptCallTarget))
            {
               // [SYNC] Wait for output tree
               if (pOperationData->bMultiThreaded)
                  WaitForSingleObject(pOperationData->hOutputMutex, INFINITE);

               /// [SUCCESS] Insert calling script into output tree
               insertScriptDependencyIntoAVLTree(pOperationData->pOutputTree, identifyScriptName(pScriptFile));   

               // [SYNC] Release output tree
               if (pOperationData->bMultiThreaded)
                  ReleaseMutex(pOperationData->hOutputMutex);
            }
         }
      }

      // Cleanup
      deleteErrorQueue(pTranslationErrors);
      deleteScriptFile(pScriptFile);
   }
}
