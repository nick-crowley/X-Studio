//
// Script Dependencies Tree.cpp : Represents the script dependencies of a script document
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    DECLARATIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

// Tree operations
VOID  treeprocFindContentInScript(AVL_TREE_NODE*  pCurrentNode, AVL_TREE_OPERATION*  pOperationData);

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


/// Function name  : generateScriptCallPath
// Description     : Generates the full path to the target of a script call
// 
// const TCHAR*  szCallerPath   : [in] Full path of calling script
// const TCHAR*  szTargetName   : [in] Target script
// 
// Return Value   : New string containing full path of target script
// 
BearScriptAPI
TCHAR*  generateScriptCallPath(const TCHAR*  szCallerPath, const TCHAR*  szTargetName)
{
   TCHAR*  szFullPath;

   // Generate full path (as .PCK)
   szFullPath = utilRenameFilePath(szCallerPath, szTargetName);
   StringCchCat(szFullPath, MAX_PATH, TEXT(".pck"));

   // Return path
   return szFullPath;
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
SCRIPT_DEPENDENCY*  insertScriptDependencyIntoAVLTree(AVL_TREE*  pTree, CONST TCHAR*  szScriptName)
{
   SCRIPT_DEPENDENCY*  pScriptDependency;

   // [CHECK] Check whether variable has been encountered before
   if (findObjectInAVLTreeByValue(pTree, (LPARAM)szScriptName, NULL, (LPARAM&)pScriptDependency))
      /// [EXISTING] Increase it's 'occurrence count'
      pScriptDependency->iCalls++;

   else
      /// [NEW] Create a ScriptDependency and add it to the tree
      insertObjectIntoAVLTree(pTree, (LPARAM)(pScriptDependency = createScriptDependency(szScriptName)) );

   // Return Dependency
   return pScriptDependency;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   THREAD FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : threadprocFindScriptContent
// Description     : Searches the script folder for scripts using a specific command
// 
// VOID*  pParameters   : OPERATION_DATA structure containing:
//                        ->   iCommandID   : [in]     ID of the command to search for
//                        ->   pCallersTree : [in/out] ScriptDependencies tree to store result in.  (Already created)
//
// Return type : Ignored
// Operation Stages : ONE (Searching)
//
// Operation Result : OR_SUCCESS - Always
// 
BearScriptAPI
DWORD   threadprocFindScriptContent(VOID*  pParameter)
{
   SCRIPT_OPERATION*      pOperationData;       // Operation data
   AVL_TREE_OPERATION*    pTreeOperation;       // Tree operation
   OPERATION_RESULT       eResult;              // Operation result
   FILE_SEARCH*           pFileSearch;          // File search containing all scripts in the script folder
   
   // [TRACKING]
   CONSOLE_COMMAND_BOLD();
   SET_THREAD_NAME("ScriptContent Search");
   setThreadLanguage(getAppPreferences()->eAppLanguage);

   __try
   {
      // Prepare
      pOperationData = (SCRIPT_OPERATION*)pParameter;
      eResult        = OR_SUCCESS;

      // [INFO] "Searching '%s' for MSCI scripts"
      pushErrorQueue(pOperationData->pErrorQueue, generateDualInformation(HERE(IDS_OUTPUT_ENUMERATING_SCRIPTS), pOperationData->szFolder));
      CONSOLE("Searching '%s' for MSCI scripts...", pOperationData->szFolder);

      /// [SEARCH] Search for all xml/pck MSCI scripts
      pFileSearch = performFileSystemSearch(getFileSystem(), pOperationData->szFolder, FF_SCRIPT_FILES, AK_PATH, AO_DESCENDING, NULL);
      CONSOLE("Searching %d MSCI scripts for content '%s'...", getFileSearchResultCount(pFileSearch), pOperationData->szContent);

      // [PROGRESS] Define progress as number of scripts found
      ASSERT(getOperationProgressStageID(pOperationData->pProgress) == IDS_PROGRESS_SEARCHING_SCRIPTS);
      updateOperationProgressMaximum(pOperationData->pProgress, getFileSearchResultCount(pFileSearch));

      // [INFO] "Searching %d scripts for '%s'"
      pushErrorQueue(pOperationData->pErrorQueue, generateDualInformation(HERE(IDS_OUTPUT_FINDING_CONTENT), getFileSearchResultCount(pFileSearch), pOperationData->szContent));

      // Create operation
      pTreeOperation = createAVLTreeThreadedOperation(treeprocFindContentInScript, ATT_INORDER, pOperationData->pProgress, 40);
     
      // Setup operation
      pTreeOperation->pErrorQueue  = pOperationData->pErrorQueue;
      pTreeOperation->xFirstInput  = pOperationData->eContent;
      pTreeOperation->xSecondInput = pOperationData->xContent.asPointer;
      pTreeOperation->pOutputTree  = pOperationData->pCallersTree;

      /// Generate + Index tree
      performOperationOnAVLTree(pFileSearch->pResultsTree, pTreeOperation);
      performAVLTreeIndexing(pOperationData->pCallersTree);

      // Cleanup
      pTreeOperation->pErrorQueue = NULL;
      deleteAVLTreeOperation(pTreeOperation);
      deleteFileSearch(pFileSearch);
   }
   PUSH_CATCH(pOperationData->pErrorQueue)
   {
      // [FAILURE]
      eResult = OR_FAILURE;
   }

   // Return result
   CONSOLE_COMPLETE("SCRIPT CONTENT SEARCH", eResult);
   closeThreadOperation(pOperationData, eResult);
   return THREAD_RETURN;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                   TREE OPERATIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : treeprocFindContentInScript
// Description     : Searches a script for a specific ScriptCall or Command
// 
// AVL_TREE_NODE*       pCurrentNode   : [in]     Current node containing a VirtualFile
// AVL_TREE_OPERATION*  pOperationData : [in/out] Operation Data : 
///                                                 xFirstInput  : [in]  SCRIPT_CONTENT : Syntax, ScriptName or ObjectName to search for
///                                                 xSecondInput : [in]  CONTENT_TYPE   : Identifies type
///                                                 pOutputTree  : [out] ScriptDependency Output tree
// 
VOID  treeprocFindContentInScript(AVL_TREE_NODE*  pCurrentNode, AVL_TREE_OPERATION*  pOperationData)
{
   FILE_ITEM*  pCurrentFile = extractPointerFromTreeNode(pCurrentNode, FILE_ITEM);
   
   __try
   {
      // [CHECK] Skip folders
      if (utilExcludes(pCurrentFile->iAttributes, FIF_FOLDER))
      {
         // Prepare
         SCRIPT_FILE*  pScriptFile = createScriptFileByOperation(SFO_TRANSLATION, pCurrentFile->szFullPath);
         ERROR_QUEUE*  pErrors     = createErrorQueue();
         
         /// Load script
         if (!loadGameFileFromFileSystemByPath(getFileSystem(), pScriptFile, NULL, pOperationData->pErrorQueue))
            // [ERROR] "The MSCI script '%s' is unavailable or could not be accessed"
            enhanceLastError(pOperationData->pErrorQueue, ERROR_ID(IDS_SCRIPT_LOAD_IO_ERROR), pCurrentFile->szDisplayName);

         /// Translate script
         else if (translateScript(pScriptFile, FALSE, FALSE, NULL, pErrors) == OR_SUCCESS)
         {
            SCRIPT_CONTENT  xContent;        // Search content
            CONTENT_TYPE    eType;           // Content type
            const TCHAR*    szScript;        // Name of script call
            COMMAND*        pCommand;        // Command iterator
            BOOL            bMatch;          // Whether command matches criteria

            // Prepare
            xContent.asPointer = pOperationData->xSecondInput;
            eType = (CONTENT_TYPE)pOperationData->xFirstInput;

            // Examine commands
            for (UINT  iIndex = 0; findCommandInTranslatorOutput(pScriptFile->pTranslator, iIndex, pCommand); iIndex++)
            {
               /// Match ScriptName / CommandSyntax / ObjectName
               switch (eType)
               {
               case CT_SCRIPT: bMatch = findScriptCallTargetInCommand(pCommand, szScript) AND utilCompareStringVariables(szScript, xContent.asScript);  break;
               case CT_SYNTAX: bMatch = isCommandID(pCommand, xContent.asSyntax->iID);                                                                  break;
               case CT_OBJECT: bMatch = isObjectNameInCommand(pCommand, xContent.asObject);                                                             break;
               }

               if (bMatch)
               {
                  // [SYNC] Wait for output tree
                  if (pOperationData->bMultiThreaded)
                     WaitForSingleObject(pOperationData->hOutputMutex, INFINITE);

                  /// Insert ScriptDependency
                  SCRIPT_DEPENDENCY*  pDependency = insertScriptDependencyIntoAVLTree(pOperationData->pOutputTree, identifyScriptName(pScriptFile)); 
                  pDependency->iLine = pCommand->iLineNumber - 1;

                  // [SYNC] Release output tree
                  if (pOperationData->bMultiThreaded)
                     ReleaseMutex(pOperationData->hOutputMutex);

                  /// [COMMAND/OBJECT] Abort after first match
                  if (eType != CT_SCRIPT)
                     break;
               }
            }

         }

         // Cleanup
         deleteErrorQueue(pErrors);
         deleteScriptFile(pScriptFile);
      }
   }
   // [EXCEPTION] Abort operation
   PUSH_CATCH(pOperationData->pErrorQueue)
   {
      pOperationData->bProcessing = FALSE;
   }
}