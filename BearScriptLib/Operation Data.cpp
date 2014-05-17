//
// Operation Data.cpp : Handles the creation and manipulation of thread data for multi-threaded operations
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       CONSTANTS / GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// LoadGameData Stages
CONST UINT   iGameDataStageIDs[12] = 
{ 
   IDS_PROGRESS_LOADING_CATALOGUES,   

   IDS_PROGRESS_LOADING_MASTER_STRINGS,            // translateLanguageFile -> generateXMLTree
   IDS_PROGRESS_STORING_MASTER_STRINGS,            // translateLanguageFile -> processing
   IDS_PROGRESS_CONVERTING_MASTER_STRINGS,         // translateLanguageFile -> processing
   IDS_PROGRESS_LOADING_SUPPLEMENTARY_STRINGS,     // loadGameStringTrees
   IDS_PROGRESS_RESOLVING_SUBSTRINGS,

   IDS_PROGRESS_LOADING_OBJECT_NAMES, 

   IDS_PROGRESS_LOADING_SPEECH_CLIPS,  
   IDS_PROGRESS_TRANSLATING_SPEECH_CLIPS,
   IDS_PROGRESS_LOADING_MEDIA_ITEMS,

   IDS_PROGRESS_LOADING_COMMAND_SYNTAX, 
   IDS_PROGRESS_LOADING_COMMAND_DESCRIPTIONS  
};

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       CREATION / DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createDocumentOperationData
// Description     : Creates operation data for an load or save document operation. 
///                        Number of stages must be set here to prevent division by zero by the progress dialog
//
// CONST OPERATION_TYPE  eOperation  : [in] The operation type
// CONST TCHAR*          szFullPath
//
// Return type : New DOCUMENT_OPERATION object, you are responsible for destroying it
//
BearScriptAPI
DOCUMENT_OPERATION*   createDocumentOperationData(CONST OPERATION_TYPE  eOperation, GAME_FILE*  pGameFile) 
{
   DOCUMENT_OPERATION*   pOperationData;

   // Create new object
   pOperationData = utilCreateEmptyObject(DOCUMENT_OPERATION);

   /// Setup object
   pOperationData->eType       = eOperation;
   pOperationData->pProgress   = createOperationProgress();
   pOperationData->pErrorQueue = createErrorQueue(); 

   // Set properties
   pOperationData->pGameFile   = pGameFile;
   pOperationData->szFullPath  = pGameFile->szFullPath;

   /// Define operation stages
   switch (eOperation)
   {
   // [SCRIPTS] 
   case OT_LOAD_SCRIPT_FILE:  
      addStageToOperationProgress(pOperationData->pProgress, IDS_PROGRESS_PARSING_SCRIPT);
      addStageToOperationProgress(pOperationData->pProgress, IDS_PROGRESS_TRANSLATING_SCRIPT);
      break;

   case OT_SAVE_SCRIPT_FILE:   
      addStageToOperationProgress(pOperationData->pProgress, IDS_PROGRESS_PARSING_COMMANDS);
      addStageToOperationProgress(pOperationData->pProgress, IDS_PROGRESS_GENERATING_SCRIPT);

      // [OPTIONAL] Add a third stage for compressed scripts
      if (utilCompareString(PathFindExtension(pGameFile->szFullPath), ".pck"))
         addStageToOperationProgress(pOperationData->pProgress, IDS_PROGRESS_COMPRESSING_SCRIPT);
      break;

   case OT_VALIDATE_SCRIPT_FILE:
      addStageToOperationProgress(pOperationData->pProgress, IDS_PROGRESS_PARSING_SCRIPT);
      addStageToOperationProgress(pOperationData->pProgress, IDS_PROGRESS_TRANSLATING_SCRIPT);
      addStageToOperationProgress(pOperationData->pProgress, IDS_PROGRESS_PARSING_SCRIPT);
      addStageToOperationProgress(pOperationData->pProgress, IDS_PROGRESS_TRANSLATING_SCRIPT);
      addStageToOperationProgress(pOperationData->pProgress, IDS_PROGRESS_VALIDATING_SCRIPT);
      break;

   // [PROJECTS]
   case OT_LOAD_PROJECT_FILE:  
      addStageToOperationProgress(pOperationData->pProgress, IDS_PROGRESS_LOADING_PROJECT);     
      break;

   case OT_SAVE_PROJECT_FILE:  
      addStageToOperationProgress(pOperationData->pProgress, IDS_PROGRESS_SAVING_PROJECT);    
      break;

   // [LANGUAGE] 
   case OT_LOAD_LANGUAGE_FILE: 
      addStageToOperationProgress(pOperationData->pProgress, IDS_PROGRESS_PARSING_LANGUAGE);
      addStageToOperationProgress(pOperationData->pProgress, IDS_PROGRESS_TRANSLATING_LANGUAGE);
      addStageToOperationProgress(pOperationData->pProgress, IDS_PROGRESS_PROCESSING_LANGUAGE);
      break;

   case OT_SAVE_LANGUAGE_FILE: 
      addStageToOperationProgress(pOperationData->pProgress, IDS_PROGRESS_GENERATING_LANGUAGE);
      break;  
   }
   
   // Return new object
   return pOperationData;
}


/// Function name  : createGameDataOperationData
// Description     : Creates operation data for loading game data
///                        Number of stages must be set here to prevent division by zero by the progress dialog
//
// Return type : New OPERATION_DATA structure, you are responsible for destroying it
//
BearScriptAPI
OPERATION_DATA*   createGameDataOperationData() 
{
   OPERATION_DATA*   pOperationData;

   // Create new object
   pOperationData = utilCreateEmptyObject(OPERATION_DATA);

   // Setup object
   pOperationData->eType       = OT_LOAD_GAME_DATA;
   pOperationData->pProgress   = createOperationProgress();
   pOperationData->pErrorQueue = createErrorQueue(); 

   /// Define operation stages
   for (UINT  iIndex = 0; iIndex < 12; iIndex++)
      addStageToOperationProgress(pOperationData->pProgress, iGameDataStageIDs[iIndex]);

   // Return new object
   return pOperationData;
}


/// Function name  : createSubmissionOperationData
// Description     : Creates operation data for submitting bug reports
///                        Number of stages must be set here to prevent division by zero by the progress dialog
//
// Return type : New SUBMISSION_OPERATION structure, you are responsible for destroying it
//
BearScriptAPI
SEARCH_OPERATION*   createSearchOperationData() 
{
   SEARCH_OPERATION*   pOperationData;

   // Create new object
   pOperationData = utilCreateEmptyObject(SEARCH_OPERATION);

   // Setup object   
   pOperationData->eType       = OT_SEARCH_FILE_SYSTEM;
   pOperationData->pProgress   = createOperationProgress();
   pOperationData->pErrorQueue = createErrorQueue(); 

   /// Define operation stages
   addStageToOperationProgress(pOperationData->pProgress, IDS_PROGRESS_SEARCHING_PHYSICAL);
   addStageToOperationProgress(pOperationData->pProgress, IDS_PROGRESS_SEARCHING_VIRTUAL);

   // Return new object
   return pOperationData;
}


/// Function name  : createScriptOperationData
// Description     : Creates operation data for searching for external script-calls
///                        Number of stages must be set here to prevent division by zero by the progress dialog
//
// Return type : New SCRIPT_OPERATION structure, you are responsible for destroying it
//
BearScriptAPI
SCRIPT_OPERATION*   createScriptOperationData(const CONTENT_TYPE  eType, const TCHAR*  szFolder, SCRIPT_CONTENT  xContent)
{
   SCRIPT_OPERATION*   pOperationData = utilCreateEmptyObject(SCRIPT_OPERATION);

   // Setup object   
   pOperationData->eType       = OT_SEARCH_SCRIPT_CONTENT;
   pOperationData->pProgress   = createOperationProgress();
   pOperationData->pErrorQueue = createErrorQueue();

   /// Create output tree
   pOperationData->pCallersTree = createScriptDependencyTreeByText();

   // [PROPERTIES] Set properties
   pOperationData->szFolder = utilDuplicateSimpleString(szFolder);
   pOperationData->eContent = eType;
   pOperationData->xContent = xContent;

   // Set display string
   switch (eType)
   {
   case CT_SYNTAX:  pOperationData->szContent = xContent.asSyntax->szContent; break;
   case CT_OBJECT:  pOperationData->szContent = xContent.asObject->szText;    break;
   case CT_SCRIPT:  pOperationData->szContent = xContent.asScript;            break;
   }

   /// Define single stage
   addStageToOperationProgress(pOperationData->pProgress, IDS_PROGRESS_SEARCHING_SCRIPTS);

   // Return new object
   return pOperationData;
}


/// Function name  : createSubmissionOperationData
// Description     : Creates operation data for submitting bug reports or correction text
//
// CONST TCHAR*  szCorrection : [in] [optional] Correction text. (Pass NULL to BugReport data)
//
// Return type : New SUBMISSION_OPERATION, you are responsible for destroying it
//
BearScriptAPI
SUBMISSION_OPERATION*   createSubmissionOperationData(CONST TCHAR*  szCorrection) 
{
   CONST UINT              iSubmitReportStageIDs[3] = { IDS_PROGRESS_PREPARING_REPORT, IDS_PROGRESS_CONNECTING_SERVER, IDS_PROGRESS_UPLOADING_REPORT };  // Operation stages
   SUBMISSION_OPERATION*   pOperationData;

   // Create new object
   pOperationData = utilCreateEmptyObject(SUBMISSION_OPERATION);

   // Setup object
   pOperationData->eType       = (szCorrection ? OT_SUBMIT_CORRECTION : OT_SUBMIT_BUG_REPORT);
   pOperationData->pProgress   = createOperationProgress();
   pOperationData->pErrorQueue = createErrorQueue(); 

   // [CORRECTION] Pass correction text
   pOperationData->szCorrection = (szCorrection ? utilDuplicateSimpleString(szCorrection) : NULL);

   /// Define operation stages
   for (UINT  iIndex = 0; iIndex < 3; iIndex++)
      addStageToOperationProgress(pOperationData->pProgress, iSubmitReportStageIDs[iIndex]);

   // Return new object
   return pOperationData;
}


/// Function name  : createUpdateOperation
// Description     : Creates operation data for update checking
//
// Return type : New UPDATE_OPERATION, you are responsible for destroying it
//
BearScriptAPI
UPDATE_OPERATION*  createUpdateOperation()
{
   UPDATE_OPERATION*  pOperationData = utilCreateEmptyObject(UPDATE_OPERATION);

   // Set properties
   pOperationData->eType       = OT_AUTOMATIC_UPDATE;
   pOperationData->pProgress   = createOperationProgress();
   pOperationData->pErrorQueue = createErrorQueue(); 

   /// Define single stage
   addStageToOperationProgress(pOperationData->pProgress, IDS_PROGRESS_UPDATE_DOWNLOADING);

   // Return new object
   return pOperationData;
}


/// Function name  : deleteOperationData
// Description     : Deletes a OperationData structure
// 
// OPERATION_DATA*  &pOperationData : [in] Operation data to destroy
//
VOID   deleteOperationData(OPERATION_DATA*  &pOperationData)
{
   SUBMISSION_OPERATION*  pSubmissionOperation;   // Convenience pointer
   DOCUMENT_OPERATION*    pDocumentOperation;     // Convenience pointer
   SCRIPT_OPERATION*      pScriptOperation;
   UPDATE_OPERATION*      pUpdateOperation;
   
   // Prepare
   pDocumentOperation = (DOCUMENT_OPERATION*)pOperationData;
   
   /// Delete error queue
   deleteErrorQueue(pOperationData->pErrorQueue);

   /// Delete progress object
   deleteOperationProgress(pOperationData->pProgress);

   // Destroy custom data
   switch (pOperationData->eType)
   {
   /// [LANGUAGE-FILE] Destroy LanguageFile, if present
   case OT_LOAD_LANGUAGE_FILE:  
      if (pDocumentOperation->pGameFile)    
         deleteLanguageFile((LANGUAGE_FILE*&)pDocumentOperation->pGameFile);      // Present if operation failed
      break;

   /// [PROJECT-FILE] Destroy ProjectFile, if present
   case OT_LOAD_PROJECT_FILE:   
      if (pDocumentOperation->pGameFile)
         deleteProjectFile((PROJECT_FILE*&)pDocumentOperation->pGameFile);        // Present if operation failed
      break;

   /// [SCRIPT-FILE] Destroy ScriptFile, if present
   case OT_LOAD_SCRIPT_FILE:
   case OT_VALIDATE_SCRIPT_FILE:
      if (pDocumentOperation->pGameFile)
         deleteScriptFile((SCRIPT_FILE*&)pDocumentOperation->pGameFile);       // Present if operation failed
      break;

   /// [CORRECTION] Destroy correction text
   case OT_SUBMIT_CORRECTION:
      pSubmissionOperation = (SUBMISSION_OPERATION*)pOperationData;

      utilDeleteString(pSubmissionOperation->szCorrection);
      break;

   /// [SCRIPT SEARCH] Destroy Dependencies tree
   case OT_SEARCH_SCRIPT_CONTENT:
      pScriptOperation = (SCRIPT_OPERATION*)pOperationData;

      deleteAVLTree(pScriptOperation->pCallersTree);
      utilDeleteString(pScriptOperation->szFolder);
      break;

   /// [AUTOMATIC UPDATE]
   case OT_AUTOMATIC_UPDATE:
      pUpdateOperation = (UPDATE_OPERATION*)pOperationData;

      utilSafeDeleteStrings(pUpdateOperation->szName, pUpdateOperation->szDate, pUpdateOperation->szURL);
      if (pUpdateOperation->pDescription) 
         deleteRichText(pUpdateOperation->pDescription);
      break;
   }      
   
   /// Close thread handle
   if (pOperationData->hThread)
      utilCloseHandle(pOperationData->hThread);
   
   // Delete calling object
   utilDeleteObject(pOperationData);
}


/// Function name  : destructorOperationData
// Description     : Deletes OperationData stored in a ListItem
// 
// LPARAM  pOperationData   : [in] Operation data to be destroyed
// 
BearScriptAPI
VOID  destructorOperationData(LPARAM  pOperationData)
{
   // Delete calling object
   deleteOperationData((OPERATION_DATA*&)pOperationData);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                           HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : identifyOperationResultString
// Description     : Resolves operation result
// 
// CONST OPERATION_RESULT  eResult : [in] Operation result
// 
// Return Value   : string
// 
BearScriptAPI
CONST TCHAR*  identifyOperationResultString(CONST OPERATION_RESULT  eResult)
{
   CONST TCHAR*  szOutput;

   switch (eResult)
   {
   case OR_SUCCESS:  szOutput = TEXT("SUCCESS!");    break;
   case OR_FAILURE:  szOutput = TEXT("FAILURE!");    break;
   case OR_ABORTED:  szOutput = TEXT("ABORTED!");    break;
   default:          szOutput = TEXT("CORRUPTED!");  break;
   }

   return szOutput;
}

/// Function name  : identifyOperationTypeString
// Description     : Resolves operation type
// 
// CONST OPERATION_TYPE  eType : [in] Operation type
// 
// Return Value   : string
// 
BearScriptAPI
CONST TCHAR*  identifyOperationTypeString(CONST OPERATION_TYPE  eType)
{
   CONST TCHAR*  szOutput;

   // Examine type
   switch (eType)
   {
   case OT_LOAD_GAME_DATA:          szOutput = TEXT("load game data");              break;         // Loading game data

   case OT_LOAD_LANGUAGE_FILE:      szOutput = TEXT("load language file");          break;         // Loading a language file
   case OT_LOAD_SCRIPT_FILE:        szOutput = TEXT("load MSCI script");            break;         // Loading a script
   case OT_LOAD_PROJECT_FILE:       szOutput = TEXT("load project");                break;         // Loading a project file

   case OT_SAVE_LANGUAGE_FILE:      szOutput = TEXT("save language file");          break;         // Saving a language file
   case OT_SAVE_SCRIPT_FILE:        szOutput = TEXT("save MSCI script");            break;         // Saving a script
   case OT_SAVE_PROJECT_FILE:       szOutput = TEXT("save project");                break;         // Saving a project file

   case OT_VALIDATE_SCRIPT_FILE:    szOutput = TEXT("script validation");           break;         // Generates/Validates script code
                          
   case OT_SEARCH_FILE_SYSTEM:      szOutput = TEXT("file system search");          break;         // FileDialog file search
   case OT_SEARCH_SCRIPT_CONTENT:   szOutput = TEXT("script content search");       break;         // Script content search

   case OT_SUBMIT_BUG_REPORT:       szOutput = TEXT("bug report submission");       break;         // Bug report submission
   case OT_SUBMIT_CORRECTION:       szOutput = TEXT("correction submission");       break;         // Correction submission 
   default:                         szOutput = TEXT("Corrupted");                   break;
   };

   return szOutput;
}

/// Function name  : isOperationComplete
// Description     : Determine whether an operation has completed or not
// 
// CONST OPERATION_DATA*  pOperationData : [in] Operation data of the operation to query
// 
// Return Value   : TRUE or FALSE
// 
BearScriptAPI
BOOL  isOperationComplete(CONST OPERATION_DATA*  pOperationData)
{
   return pOperationData->pProgress->bComplete;
}

/// Function name  : isOperationSuccessful
// Description     : Determine whether an operation was successful or not
// 
// CONST OPERATION_DATA*  pOperationData : [in] Operation data of the operation to query
// 
// Return Value   : TRUE or FALSE
// 
BearScriptAPI
BOOL  isOperationSuccessful(CONST OPERATION_DATA*  pOperationData)
{
   return pOperationData->eResult == OR_SUCCESS;
}


/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                           FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : closeThreadOperation
// Description     : Mark a ThreadOperation object as 'complete' and specify the result
// 
// OPERATION_DATA*         pOperationData : [in] Threaded operation data to mark as 'complete'
// CONST OPERATION_RESULT  eResult        : [in] Operation result
// 
VOID   closeThreadOperation(OPERATION_DATA*  pOperationData, CONST OPERATION_RESULT  eResult)
{
   // Mark operation as complete
   pOperationData->pProgress->bComplete = TRUE;

   // Set result
   pOperationData->eResult = eResult;

   /// Notify the parent window
   PostMessage(pOperationData->hParentWnd, UN_OPERATION_COMPLETE, NULL, (LPARAM)pOperationData);
}


/// Function name  :  launchThreadedOperation
// Description     : Launches a new thread and returns it's handle
// 
// LPTHREAD_START_ROUTINE  pfnOperationFunction : [in] Function for the thread to perform
// OPERATION_DATA*         pOperationData       : [in]  Data for the new thread
///                                               [out] Will contain the thread handle
// 
// Return Value   : TRUE if successful, FALSE otherwise.  The thread is created in the running state.
// 
BearScriptAPI
BOOL   launchThreadedOperation(LPTHREAD_START_ROUTINE  pfnOperationFunction, OPERATION_DATA*  pOperationData)
{
   //return utilLaunchThreadpoolThread(pfnOperationFunction, pOperationData);

   // Attempt to create new thread and save handle
   pOperationData->hThread = utilLaunchThread(pfnOperationFunction, pOperationData, TRUE, &pOperationData->dwThreadID);
   ERROR_CHECK("launching threaded operation", pOperationData->hThread);

   // Set thread priorty and execute
   if (pOperationData->hThread)
   {
      SetThreadPriority(pOperationData->hThread, THREAD_PRIORITY_ABOVE_NORMAL);
      ResumeThread(pOperationData->hThread);
   }

   // Return TRUE if successful
   return (pOperationData->hThread != NULL);
}

