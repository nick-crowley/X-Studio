//
// Project File.cpp : Project stored on disc
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        MACROS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createProjectFile
// Description     : Create a new Project File
// 
// CONST TCHAR*  szFullPath     : [in] Full path of project file
// 
// Return Value   : New project file
// 
BearScriptAPI
PROJECT_FILE*  createProjectFile(CONST TCHAR*  szFullPath)
{
   PROJECT_FILE* pProjectFile = utilCreateEmptyObject(PROJECT_FILE);

   // Set properties
   setGameFilePath(pProjectFile, szFullPath);

   /// Create File lists
   for (PROJECT_FOLDER  eType = PF_SCRIPT; eType <= PF_MISSION; eType = (PROJECT_FOLDER)(eType + 1))
      pProjectFile->pFileList[eType] = createList(destructorStoredDocument);

   /// Creates variables tree
   pProjectFile->pVariablesTree = createProjectVariableTreeByText();

   // Return new object
   return pProjectFile;
}


/// Function name  : createStoredDocument
// Description     : Creates a 'StoredDocument' for saving document filenames to the registry
// 
// CONST FILE_ITEM_FLAG  eType       : [in] Document type
// CONST TCHAR*          szFullPath  : [in] Full path of document
// 
// Return Value   : New StoredDocument, you are responsible for destroying it
// 
BearScriptAPI
STORED_DOCUMENT*  createStoredDocument(CONST FILE_ITEM_FLAG  eType, CONST TCHAR*  szFullPath)
{
   STORED_DOCUMENT*  pStoredDocument;

   // Create object
   pStoredDocument = utilCreateEmptyObject(STORED_DOCUMENT);

   // Set properties
   pStoredDocument->eType = eType;
   StringCchCopy(pStoredDocument->szFullPath, MAX_PATH, szFullPath);

   // Return new object
   return pStoredDocument;
}


/// Function name  : deleteProjectFile
// Description     : Destroys a project file
// 
// PROJECT_FILE*  &pProjectFile   : [in] 
// 
BearScriptAPI
VOID  deleteProjectFile(PROJECT_FILE*  &pProjectFile)
{
   // Delete I/O buffers
   deleteGameFileIOBuffers(pProjectFile);

   /// Destroy File lists
   for (PROJECT_FOLDER  eType = PF_SCRIPT; eType <= PF_MISSION; eType = (PROJECT_FOLDER)(eType + 1))
      deleteList(pProjectFile->pFileList[eType]);

   /// Destroy variables
   deleteAVLTree(pProjectFile->pVariablesTree);

   // Delete XML tree (if any)
   if (pProjectFile->pXMLTree)
      deleteXMLTree(pProjectFile->pXMLTree);
   
   // Delete calling object
   utilDeleteObject(pProjectFile);
}


/// Function name  : deleteStoredDocument
// Description     : Destroys a StoredDocument object
// 
// STORED_DOCUMENT*  &pStoredDocument   : [in] StoredDocument object to destroy
// 
BearScriptAPI
VOID  deleteStoredDocument(STORED_DOCUMENT*  &pStoredDocument)
{
   // Delete calling object
   utilDeleteObject(pStoredDocument);
}


/// Function name  : destructorStoredDocument
// Description     : Destroys a StoredDocument object
// 
// LPARAM  pStoredDocument   : [in] StoredDocument object to destroy
// 
BearScriptAPI
VOID  destructorStoredDocument(LPARAM  pStoredDocument)
{
   deleteStoredDocument((STORED_DOCUMENT*&)pStoredDocument);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : addDocumentToProjectFile
// Description     : Adds a document to the relevant ProjectFile folder
// 
// PROJECT_FILE*     pProjectFile : [in] Project file
// STORED_DOCUMENT*  pDocument    : [in] New document
// 
BearScriptAPI
VOID  addDocumentToProjectFile(PROJECT_FILE*  pProjectFile, STORED_DOCUMENT*  pDocument)
{
   PROJECT_FOLDER  eFolder;

   // Prepare
   eFolder = calculateProjectFolderFromFileType(pDocument->eType);

   /// Append to relevant list
   appendObjectToList(pProjectFile->pFileList[eFolder], (LPARAM)pDocument);
}
   

/// Function name  : calculateDocumentTypeStringFromFileType
// Description     : Converts a file type into a document type string
// 
// FILE_ITEM_FLAG  eType : [in] Type to convert
// 
// Return Value   : Document type string
// 
CONST TCHAR*  calculateDocumentTypeStringFromFileType(FILE_ITEM_FLAG  eType)
{
   CONST TCHAR*  szOutput;

   // Prepare
   szOutput = TEXT("Unknown");

   // Examine type
   switch (eType)
   {
   case FIF_SCRIPT:     szOutput = TEXT("script");    break;
   case FIF_LANGUAGE:   szOutput = TEXT("language");  break;
   case FIF_MISSION:    szOutput = TEXT("mission");   break;
   default:             ASSERT(FALSE);
   }

   // Return string
   return szOutput;
}


/// Function name  : calculateFileTypeFromDocumentTypeString
// Description     : Converts a project file type string into corresponding FileItemFlag
// 
// CONST TCHAR*     szDocumentType : [in]  String to convert
// FILE_ITEM_FLAG&  eOutput        : [out] Equivilent document type, or FIF_NONE if unsuccessful
// 
// Return Value   : TRUE if succeessful, otherwise FALSE
// 
BOOL  calculateFileTypeFromDocumentTypeString(CONST TCHAR*  szDocumentType, FILE_ITEM_FLAG&  eOutput)
{
   // Prepare
   eOutput = FIF_NONE;

   /// Convert input string
   if (utilCompareString(szDocumentType, "script"))
      eOutput = FIF_SCRIPT;
   else if (utilCompareString(szDocumentType, "language"))
      eOutput = FIF_LANGUAGE;
   else if (utilCompareString(szDocumentType, "mission"))
      eOutput = FIF_MISSION;

   // Return TRUE if found
   return (eOutput != FIF_NONE);
}


/// Function name  : calculateProjectFolderFromFileType
// Description     : Converts a FileItemFlag into a ProjectFolderIndex
// 
// CONST FILE_ITEM_FLAG  eFileType : [in] FileItemFlag
// 
// Return Value   : ProjectFolderIndex
// 
BearScriptAPI
PROJECT_FOLDER  calculateProjectFolderFromFileType(CONST FILE_ITEM_FLAG  eFileType)
{
   PROJECT_FOLDER   eFolder;

   // Examine FileType
   switch (eFileType)
   {
   case FIF_SCRIPT:     eFolder = PF_SCRIPT;      break;
   case FIF_LANGUAGE:   eFolder = PF_LANGUAGE;    break;
   case FIF_MISSION:    eFolder = PF_MISSION;     break;
   default:             ASSERT(FALSE);             break;
   }

   // Return Folder
   return eFolder;
}


/// Function name  : findDocumentInProjectFileByIndex
// Description     : Finds a document within the project
// 
// CONST PROJECT_FILE*   pProjectFile : [in]  Project file
// CONST PROJECT_FOLDER  eFolder      : [in]  Folder
// CONST UINT            iIndex       : [in] Index
// STORED_DOCUMENT*      &pOutput     : [out] Target document if found, otherwise NULL
// 
// Return Value   : TRUE if found, otherwise FALSE
// 
BearScriptAPI
BOOL   findDocumentInProjectFileByIndex(CONST PROJECT_FILE*  pProjectFile, CONST PROJECT_FOLDER  eFolder, CONST UINT  iIndex, STORED_DOCUMENT*  &pOutput)
{
   // [CHECK] Ensure project exists
   ASSERT(pProjectFile);

   // Return document
   return findListObjectByIndex(pProjectFile->pFileList[eFolder], iIndex, (LPARAM&)pOutput);
}


/// Function name  : findDocumentInProjectFileByPath
// Description     : Finds a document within the project
// 
// CONST PROJECT_FILE*   pProjectFile : [in]  Project file
// CONST TCHAR*          szPath       : [in]  Full Document path
// CONST PROJECT_FOLDER  eFolder      : [in]  Folder
// STORED_DOCUMENT*      &pOutput     : [out] Target document if found, otherwise NULL
// 
// Return Value   : TRUE if found, otherwise FALSE
// 
BearScriptAPI
BOOL   findDocumentInProjectFileByPath(CONST PROJECT_FILE*  pProjectFile, CONST TCHAR*  szPath, CONST PROJECT_FOLDER  eFolder, STORED_DOCUMENT*  &pOutput)
{
   STORED_DOCUMENT*  pCurrentDocument;

   // [CHECK] Ensure project exists
   ASSERT(pProjectFile);

   // Prepare
   pOutput = NULL;

   /// Iterate through relevant file list
   for (LIST_ITEM*  pIterator = getListHead(pProjectFile->pFileList[eFolder]); !pOutput AND (pCurrentDocument = extractListItemPointer(pIterator, STORED_DOCUMENT)); pIterator = pIterator->pNext)
   {
      // [COMPARE] Compare full paths
      if (utilCompareStringVariables(pCurrentDocument->szFullPath, szPath))
         /// [FOUND] Return result
         pOutput = pCurrentDocument;
   }

   // Return TRUE if found
   return (pOutput != NULL);
}


/// Function name  : findVariableInProjectFileByIndex
// Description     : Finds a variable within the project
// 
// CONST PROJECT_FILE*  pProjectFile : [in] Project file
// CONST UINT           iIndex       : [in] Index
// PROJECT_VARIABLE*   &pOutput      : [out] Variable if found, otherwise NULL
// 
// Return Value   : TRUE if found, otherwise FALSE
// 
BearScriptAPI
BOOL   findVariableInProjectFileByIndex(CONST PROJECT_FILE*  pProjectFile, CONST UINT  iIndex, PROJECT_VARIABLE*  &pOutput)
{
   return findObjectInAVLTreeByIndex(pProjectFile->pVariablesTree, iIndex, (LPARAM&)pOutput);
}


/// Function name  : findVariableInProjectFileByName
// Description     : Finds a variable within the project
// 
// CONST PROJECT_FILE*  pProjectFile : [in] Project file
// CONST TCHAR*         szName       : [in] Variable name
// PROJECT_VARIABLE*   &pOutput      : [out] Variable if found, otherwise NULL
// 
// Return Value   : TRUE if found, otherwise FALSE
// 
BearScriptAPI
BOOL   findVariableInProjectFileByName(CONST PROJECT_FILE*  pProjectFile, CONST TCHAR*  szName, PROJECT_VARIABLE*  &pOutput)
{
   return findObjectInAVLTreeByValue(pProjectFile->pVariablesTree, (LPARAM)szName, NULL, (LPARAM&)pOutput);
}


/// Function name  : getProjectFileCountByFolder
// Description     : Retrieves the number of files in a project folder
// 
// CONST PROJECT_FILE*   pProjectFile : [in] Project file
// PROJECT_FOLDER        eFolder      : [in] Target folder
// 
// Return Value   : Number of files
// 
BearScriptAPI
UINT  getProjectFileCountByFolder(CONST PROJECT_FILE*  pProjectFile, PROJECT_FOLDER  eFolder)
{
   return pProjectFile ? getListItemCount(pProjectFile->pFileList[eFolder]) : NULL;
}

/// Function name  : getProjectFileVariableCount
// Description     : Retrieves the number of variables in a project file
// 
// CONST PROJECT_FILE*   pProjectFile : [in] Project file
// 
// Return Value   : Number of variables
// 
BearScriptAPI
UINT  getProjectFileVariableCount(CONST PROJECT_FILE*  pProjectFile)
{
   return getTreeNodeCount(pProjectFile->pVariablesTree);
}

/// Function name  : insertVariableIntoProjectFile
// Description     : Adds a project variable to a project file
// 
// PROJECT_FILE*            pProjectFile : [in] project file
// CONST PROJECT_VARIABLE*  pVariable    : [in] variable to insert
// 
// Return Value   : TRUE if successful, otherwise FALSE
// 
BearScriptAPI
BOOL  insertVariableIntoProjectFile(PROJECT_FILE*  pProjectFile, CONST PROJECT_VARIABLE*  pVariable)
{
   BOOL  bResult;

   /// Insert Variable and re-index tree
   if (bResult = insertObjectIntoAVLTree(pProjectFile->pVariablesTree, (LPARAM)pVariable))
      performAVLTreeIndexing(pProjectFile->pVariablesTree);

   // Return result
   return bResult;
}

/// Function name  : removeFileFromProject
// Description     : Removes and destroys a StoredDocument within a project file
// 
// PROJECT_FILE*           pProjectFile : [in] Project File
// CONST STORED_DOCUMENT*  pDocument    : [in] Document
// 
// Return Value   : TRUE if found, otherwise FALSE
// 
BearScriptAPI
BOOL  removeFileFromProject(PROJECT_FILE*  pProjectFile, STORED_DOCUMENT*  &pDocument)
{
   PROJECT_FOLDER  eFolder;
   BOOL            bResult;

   // [CHECK] Ensure project exists
   ASSERT(pProjectFile);

   // Prepare
   eFolder = calculateProjectFolderFromFileType(pDocument->eType);

   /// Remove document from project
   if (bResult = removeObjectFromListByValue(pProjectFile->pFileList[eFolder], (LPARAM)pDocument))
      // [FOUND] Destroy document
      deleteStoredDocument(pDocument);

   // Return result
   return bResult;
}

/// Function name  : removeVariableFromProjectFileByIndex
// Description     : Removes a variable within the project
// 
// CONST PROJECT_FILE*  pProjectFile : [in] Project file
// CONST UINT           iIndex       : [in] Index
// 
// Return Value   : TRUE if found, otherwise FALSE
// 
BearScriptAPI
BOOL   removeVariableFromProjectFileByIndex(CONST PROJECT_FILE*  pProjectFile, CONST UINT  iIndex)
{
   BOOL  bResult;

   /// Insert Variable and re-index tree
   if (bResult = destroyObjectInAVLTreeByIndex(pProjectFile->pVariablesTree, iIndex))
      performAVLTreeIndexing(pProjectFile->pVariablesTree);

   // Return result
   return bResult;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : generateProjectFileFromXML
// Description     : Generates a PROJECT object from a project file stored as XML
// 
// CONST PROJECT_FILE*  pProjectFile : [in/out] ProjectFile containing XML on input and new Project on output
// HWND                 hParentWnd   : [in]     Error handling window
// ERROR_QUEUE*         pErrorQueue  : [in/out] Error queue
// 
// Return Value   : OR_SUCCESS if successful, otherwise OR_FAILURE
// 
OPERATION_RESULT  generateProjectFileFromXML(PROJECT_FILE*  pProjectFile, HWND  hParentWnd, ERROR_QUEUE  *pErrorQueue)
{
   PROJECT_FOLDER        eFolder;            // Folder to insert current document into
   STORED_DOCUMENT      *pDocument;          // Current document
   PROJECT_VARIABLE     *pVariable;          // Current variable
   FILE_ITEM_FLAG        eFileType;          // File type of current document
   XML_TREE_NODE        *pProjectNode,       // Project file <project> node
                        *pCurrentNode;       // Current <document> or <variable> node 
   CONST TCHAR          *szDocumentType;     // XML value specifying the FileType of the current document
   TCHAR                *szFileType,         // Description of current document file type
                        *szAlternatePath;    // Path to document if it were in the project folder
   INT                   iVariableValue;     // ProjectVariable value
   OPERATION_RESULT      eResult;

   // Prepare
   TRACK_FUNCTION();
   eResult = OR_SUCCESS;

   /// Load project file
   if ((eResult = generateXMLTree(pProjectFile->szInputBuffer, pProjectFile->iInputSize, PathFindFileName(pProjectFile->szFullPath), hParentWnd, pProjectFile->pXMLTree, NULL, pErrorQueue)) == OR_SUCCESS)
   {
      // [CHECK] Locate <project> node
      if (!pProjectFile->pXMLTree OR !getXMLNodeCount(pProjectFile->pXMLTree->pRoot) OR !findXMLTreeNodeByName(pProjectFile->pXMLTree->pRoot, TEXT("project"), TRUE, pProjectNode))
      {
         // [ERROR] "The project file is empty or the <project> node is missing"
         pushErrorQueue(pErrorQueue, generateDualError(HERE(IDS_PROJECT_NODE_MISSING)));
         eResult = OR_FAILURE;
      }  
      /// Iterate through sub-nodes
      else for (UINT  iIndex = 0; findXMLTreeNodeByIndex(pProjectNode, iIndex, pCurrentNode); iIndex++)
      {
         /// [DOCUMENT] Add new StoredDocument to ProjectFile
         if (utilCompareString(pCurrentNode->szName, "document"))
         {
            // Extract and validate properties
            if (!getXMLPropertyString(pCurrentNode, TEXT("type"), szDocumentType) OR !calculateFileTypeFromDocumentTypeString(szDocumentType, eFileType) OR !lstrlen(pCurrentNode->szText))
            {
               // [ERROR] "The project document definition on line %u is corrupt"
               pushErrorQueue(pErrorQueue, generateDualError(HERE(IDS_PROJECT_DOCUMENT_CORRUPT), pCurrentNode->iLineNumber));
               eResult = OR_FAILURE;
               break;
            }

            // Prepare
            eFolder = calculateProjectFolderFromFileType(eFileType);

            /// Create new StoredDocument and append to appropriate file list
            pDocument = createStoredDocument(eFileType, pCurrentNode->szText);
            appendObjectToList(pProjectFile->pFileList[eFolder], (LPARAM)pDocument);

            // [CHECK] Does the file exist?
            if (!PathFileExists(pDocument->szFullPath))
            {
               // [CHECK] Ensure the file isn't in same folder as the project
               if (PathFileExists(szAlternatePath = utilRenameFilePath(pProjectFile->szFullPath, PathFindFileName(pDocument->szFullPath))))
               {
                  // [FOUND] Use alternate path
                  StringCchCopy(pDocument->szFullPath, MAX_PATH, szAlternatePath);
                  utilDeleteString(szAlternatePath);
               }
               else
               {
                  // [WARNING] "The %s file '%s' appears to be missing"
                  szFileType = utilLoadString(getResourceInstance(), IDS_DOCUMENT_TYPE_SCRIPT + eFolder, 32);
                  pushErrorQueue(pErrorQueue, generateDualWarning(HERE(IDS_PROJECT_DOCUMENT_MISSING), szFileType, pDocument->szFullPath));
                  utilDeleteString(szFileType);
               }
            }
         }
         /// [VARIABLE] Add new ProjectVariable to ProjectFile
         else if (utilCompareString(pCurrentNode->szName, "variable"))
         {
            // Extract and validate properties
            if (!getXMLPropertyInteger(pCurrentNode, TEXT("value"), iVariableValue) OR !lstrlen(pCurrentNode->szText))
            {
               // [ERROR] "The project variable definition on line %u is corrupt"
               pushErrorQueue(pErrorQueue, generateDualError(HERE(IDS_PROJECT_VARIABLE_CORRUPT), pCurrentNode->iLineNumber));
               eResult = OR_FAILURE;
               break;
            }

            /// Add new ProjectVariable to ProjectFile
            pVariable = createProjectVariable(pCurrentNode->szText, iVariableValue);
            insertVariableIntoProjectFile(pProjectFile, pVariable);
         }
         else
         {
            /// [ERROR] "The project contains an unrecognised node '%s' on line %u"
            pushErrorQueue(pErrorQueue, generateDualError(HERE(IDS_PROJECT_NODE_UNRECOGNISED), pCurrentNode->szText, pCurrentNode->iLineNumber));
            eResult = OR_FAILURE;
            break;
         }
      }
   }

   // [FAILURE] Add generic error if translation failed
   if (eResult == OR_FAILURE)
      // [ERROR] "Translation of project '%s' failed, the file is corrupt"
      enhanceLastError(pErrorQueue, ERROR_ID(IDS_PROJECT_TRANSLATION_FAILED), PathFindFileName(pProjectFile->szFullPath));
   
   // Cleanup and return result
   deleteXMLTree(pProjectFile->pXMLTree);
   END_TRACKING();
   return eResult;
}


/// Function name  : generateProjectFileOutputBuffer
// Description     : Flattens the XML-Tree into the output buffer
// 
// PROJECT_FILE*  pProjectFile : [in/out] ProjectFile containing a complete XML-Tree
// ERROR_QUEUE*   pErrorQueue  : [in/out] Error messages, if any
// 
// Return Value   : TRUE
// 
BOOL  generateProjectFileOutputBuffer(PROJECT_FILE*  pProjectFile)
{
   TEXT_STREAM*  pOutputStream;     // XML output stream

   // [VERBOSE]
   VERBOSE("Performing recursive tree flattening for project '%s'", pProjectFile->szFullPath);

   // Create stream object
   pOutputStream = createTextStream(32 * 1024);

   /// Add schema tags
   appendStringToTextStreamf(pOutputStream, TEXT("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"));

   /// Flatten XMLTree
   generateTextStreamFromXMLTree(pProjectFile->pXMLTree, pOutputStream);

   /// Copy output to ScriptFile
   pProjectFile->szOutputBuffer = utilDuplicateString(pOutputStream->szBuffer, pOutputStream->iBufferUsed);
   pProjectFile->iOutputSize    = pOutputStream->iBufferUsed;

   // Cleanup and return TRUE
   deleteTextStream(pOutputStream);
   return TRUE;
}


/// Function name  : generateXMLTreeFromProjectFile
// Description     : Generates the XML-Tree within a ProjectFile
// 
// PROJECT_FILE*        pProjectFile  : [in]     ProjectFile
// OPERATION_PROGRESS*  pProgress     : [in/out] Progress
// ERROR_QUEUE*         pErrorQueue   : [in/out] Error messages, if any
// 
// Return Value   : TRUE
// 
BOOL  generateXMLTreeFromProjectFile(PROJECT_FILE*  pProjectFile, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue)
{
   STORED_DOCUMENT  *pDocument;          // Current document
   PROJECT_VARIABLE *pVariable;          // Current variable
   XML_TREE_NODE    *pDocumentNode,      // Current <document> node
                    *pVariableNode,      // Current <Variable> node
                    *pProjectNode;       // Parent <project> node
   XML_TREE         *pXMLTree;           // XML-Tree

   // [VERBOSE]
   VERBOSE("Generating XML-Tree for project '%s'", pProjectFile->szFullPath);

   /// [CHECK] Delete previous tree, if any
   if (pProjectFile->pXMLTree)
      deleteXMLTree(pProjectFile->pXMLTree);

   // Prepare
   pXMLTree = createXMLTree();

   /// Create <project> node
   pProjectNode = appendStringNodeToXMLTree(pXMLTree, pXMLTree->pRoot, TEXT("project"), NULL);
   appendPropertyToXMLTreeNode(pProjectNode, createXMLPropertyFromInteger(TEXT("version"), 1));

   // Iterate through document list
   for (LIST_ITEM*  pIterator = getListHead(pProjectFile->pFileList[0]); pDocument = extractListItemPointer(pIterator, STORED_DOCUMENT); pIterator = pIterator->pNext)
   {
      /// [DOCUMENT] Append new <document> node
      pDocumentNode = appendStringNodeToXMLTree(pXMLTree, pProjectNode, TEXT("document"), pDocument->szFullPath);
      appendPropertyToXMLTreeNode(pDocumentNode, createXMLPropertyFromString(TEXT("type"), calculateDocumentTypeStringFromFileType(pDocument->eType)));
   }

   // Iterate through variables
   for (UINT iIndex = 0; findVariableInProjectFileByIndex(pProjectFile, iIndex, pVariable); iIndex++)
   {
      /// [VARIABLE] Append new <variable> node
      pVariableNode = appendStringNodeToXMLTree(pXMLTree, pProjectNode, TEXT("variable"), pVariable->szName);
      appendPropertyToXMLTreeNode(pVariableNode, createXMLPropertyFromInteger(TEXT("value"), pVariable->iValue));
   }

   /// Replace ProjectFile XMLTree
   pProjectFile->pXMLTree = pXMLTree;

   /// Flatten tree
   return generateProjectFileOutputBuffer(pProjectFile);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     THREAD FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : threadprocLoadProjectFile
// Description     : Decodes an XML project into a ProjectFile
// 
// VOID*  pParameter : [in] OPERATION_DATA* : Operation data containing an empty ProjectFile
//
// Operation Result : OR_SUCCESS - The project was loaded successfully, any minor errors were ignored by the user
//                    OR_FAILURE - The project was NOT loaded due to critical errors that prevented translation.
// 
BearScriptAPI
DWORD   threadprocLoadProjectFile(VOID*  pParameter)
{
   OPERATION_RESULT     eResult;          // Operation result, defaults to SUCCESS
   DOCUMENT_OPERATION*  pOperationData;   // Convenience pointer
   PROJECT_FILE*        pProjectFile;     // Convenience pointer

   // [DEBUGGING]
   TRACK_FUNCTION();
   SET_THREAD_NAME("Project Translation");
   setThreadLanguage(getAppPreferences()->eAppLanguage);

   // [CHECK] Ensure parameter exists
   ASSERT(pParameter);
   
   // Prepare
   pOperationData = (DOCUMENT_OPERATION*)pParameter;
   pProjectFile   = (PROJECT_FILE*)pOperationData->pGameFile;
   eResult        = OR_SUCCESS;

   // [STAGE] Set only progress state
   ASSERT(getOperationProgressStageID(pOperationData->pProgress) == IDS_PROGRESS_LOADING_PROJECT);

   /// [GUARD BLOCK]
   __try
   {
      // [INFO] "Parsing XML from project file '%s'"
      pushErrorQueue(pOperationData->pErrorQueue, generateDualInformation(HERE(IDS_OUTPUT_LOADING_PROJECT_XML), PathFindFileName(pOperationData->szFullPath)));
      VERBOSE_SMALL_PARTITION();

      /// [LOAD] Load XML into GameFile 
      if (!loadGameFileFromFileSystemByPath(getFileSystem(), pProjectFile, NULL, pOperationData->pErrorQueue))
      {
         // [ERROR] "The project file '%s' is unavailable or could not be accessed"
         enhanceLastError(pOperationData->pErrorQueue, ERROR_ID(IDS_PROJECT_LOAD_IO_ERROR), pOperationData->szFullPath);
         eResult = OR_FAILURE;
      }
      else
         /// [SUCCESS] Attempt to translate XML into a Project
         eResult = generateProjectFileFromXML(pProjectFile, pOperationData->hParentWnd, pOperationData->pErrorQueue);
   }
   /// [EXCEPTION HANDLER]
   __except (generateQueuedExceptionError(GetExceptionInformation(), pOperationData->pErrorQueue))
   {
      // [FAILURE] "An unidentified and unexpected critical error has occurred while translating the project '%s'"
      enhanceLastError(pOperationData->pErrorQueue, ERROR_ID(IDS_EXCEPTION_LOAD_PROJECT_FILE), pOperationData->szFullPath);
      eResult = OR_FAILURE;
   }

   // [DEBUG] Separate previous output from further output for claritfy
   VERBOSE_THREAD_COMPLETE("PROJECT TRANSLATION WORKER THREAD COMPLETED");
   
   // Cleanup and return result
   closeThreadOperation(pOperationData, eResult);
   END_TRACKING();
   return  THREAD_RETURN;
}


/// Function name  : threadprocSaveProjectFile
// Description     : Encodes and saves a ProjectFile into XML
// 
// VOID*  pParameter : [in] OPERATION_DATA* : Operation data containing a ProjectFile
//
// Operation Result : OR_SUCCESS - The project was saved successfully, any minor errors were ignored by the user
//                    OR_FAILURE - The project was NOT saved due to XML or I/O generation errors
// 
BearScriptAPI
DWORD    threadprocSaveProjectFile(VOID*  pParameter)
{
   DOCUMENT_OPERATION*  pOperationData;      // Operation data
   PROJECT_FILE*        pProjectFile;        // Convenience pointer
   OPERATION_RESULT     eResult;             // Operation result

   // [DEBUGGING]
   TRACK_FUNCTION();
   SET_THREAD_NAME("Script Generation");
   setThreadLanguage(getAppPreferences()->eAppLanguage);

   // [CHECK] Ensure parameter exists
   ASSERT(pParameter);
   
   // Prepare
   pOperationData = (DOCUMENT_OPERATION*)pParameter;
   pProjectFile   = (PROJECT_FILE*)pOperationData->pGameFile;
   eResult        = OR_SUCCESS;

   // [STAGE] "Saving project file '%s' to disc"
   ASSERT(getOperationProgressStageID(pOperationData->pProgress) == IDS_PROGRESS_SAVING_PROJECT);
   pushErrorQueue(pOperationData->pErrorQueue, generateDualInformation(HERE(IDS_OUTPUT_SAVING_PROJECT_XML), PathFindFileName(pOperationData->szFullPath)));

   /// [GUARD BLOCK]
   __try
   {
      /// Generate an XML-Tree from the Project
      if (!generateXMLTreeFromProjectFile(pProjectFile, pOperationData->pProgress, pOperationData->pErrorQueue))
         // [ERROR] No further enhancement necessary
         eResult = OR_FAILURE;
      
      else 
      {
         // [VERBOSE]
         VERBOSE("Saving project file '%s'", pOperationData->szFullPath);

         /// [SUCCESS] Attempt to save output to disc
         if (!saveDocumentToFileSystem(pOperationData->szFullPath, pProjectFile, pOperationData->pErrorQueue))
         {
            // [ERROR] "There was an error while saving the MSCI script '%s', the file was not saved"
            enhanceLastError(pOperationData->pErrorQueue, ERROR_ID(IDS_PROJECT_SAVE_IO_ERROR), pOperationData->szFullPath); 
            eResult = OR_FAILURE;
         }
      }
   }
   /// [EXCEPTION HANDLER]
   __except (generateQueuedExceptionError(GetExceptionInformation(), pOperationData->pErrorQueue))
   {
      // [ERROR] "An unidentified and unexpected critical error has occurred while generating the project file '%s'"
      enhanceLastError(pOperationData->pErrorQueue, ERROR_ID(IDS_EXCEPTION_SAVE_PROJECT_FILE), PathFindFileName(pOperationData->szFullPath));
      
      // [FAILURE]
      eResult = OR_FAILURE;
   }

   // [DEBUG] Separate previous output from further output for claritfy
   VERBOSE_THREAD_COMPLETE("PROJECT GENERATION WORKER THREAD COMPLETED");
   
   // Cleanup and return result
   closeThreadOperation(pOperationData, eResult);
   END_TRACKING();
   return  THREAD_RETURN;
}
