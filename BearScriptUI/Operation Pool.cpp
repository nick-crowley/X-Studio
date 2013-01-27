//
// Operation Pool.cpp : Manages threaded operations and provides a monitoring capability
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

/// Function name  : createOperationPool
// Description     : Creates an Operation Pool object for handling threaded operations.
//                      NOTE: This does not create the operation pool window, which is separate
// 
// Return Value   : New Operations Pool, you are responsible for destroying it
// 
OPERATION_POOL*  createOperationPool()
{
   OPERATION_POOL*  pOperationPool;

   // Create object
   pOperationPool = utilCreateEmptyObject(OPERATION_POOL);

   // Create operations list
   pOperationPool->pOperationList = createList(destructorOperationData);

   // Return new object
   return pOperationPool;
}


/// Function name  : createOperationPoolCtrl
// Description     : Creates the control representing the operation pool
// 
// OPERATION_POOL*  pOperationPool : [in] Operation Pool control data
// HWND             hParentWnd     : [in] Parent window
// 
// Return Value   : Window handle of the new control if successful, otherwise NULL
// 
HWND  createOperationPoolCtrl(OPERATION_POOL*  pOperationPool, HWND  hParentWnd)
{
   // [TRACK]
   TRACK_FUNCTION();

   /// Create OperationPool control
   pOperationPool->hWnd = CreateWindow(szOperationPoolClass, NULL, WS_CHILD WITH WS_VISIBLE, 0, 0, CW_USEDEFAULT, CW_USEDEFAULT, hParentWnd, (HMENU)IDC_OPERATION_POOL, getAppInstance(), NULL);
   ERROR_CHECK("creating operation pool control", pOperationPool->hWnd);

   // [CHECK] Ensure control was created successfully
   if (pOperationPool->hWnd)
   {
      // Store the data within the window
      SetWindowLong(pOperationPool->hWnd, 4, (LONG)pOperationPool);

      // Sub-class the control
      pOperationPool->wndprocProgressBar = SubclassWindow(pOperationPool->hWnd, wndprocOperationPool);
   }
  
   // Return window handle (or NULL)
   END_TRACKING();
   return pOperationPool->hWnd;
}


/// Function name  : deleteOperationPool
// Description     : Destroys both the data for an operation pool and the progress bar control
// 
// OPERATION_POOL*  &pOperationPool : [in] Operation Pool to destroy
// 
VOID  deleteOperationPool(OPERATION_POOL*  &pOperationPool)
{
   // [TRACK]
   TRACK_FUNCTION();

   /// TODO: Terminate any existing operations gracefully

   /// Destroy window
   utilDeleteWindow(pOperationPool->hWnd);

   // Delete operations list
   deleteList(pOperationPool->pOperationList);

   /// Delete calling object
   utilDeleteObject(pOperationPool);

   // [TRACK]
   END_TRACKING();
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : findOperationDataByIndex
// Description     : Retrieves operation data for the specified operation
// 
// CONST OPERATION_POOL*  pOperationPool : [in]  Operation pool to query
// CONST UINT             iIndex         : [in]  Zero based index of the target operation
// OPERATION_DATA*        pOutput        : [out] Operation data if found, otherwise NULL
// 
// Return Value   : TRUE if found, otherwise NULL
// 
BOOL  findOperationDataByIndex(CONST OPERATION_POOL*  pOperationPool, CONST UINT  iIndex, OPERATION_DATA*  &pOutput)
{
   // Prepare
   pOutput = NULL;

   // Query list
   return findListObjectByIndex(pOperationPool->pOperationList, iIndex, (LPARAM&)pOutput);
}


/// Function name  : getOperationCount
// Description     : Retrieves the current number of operations
// 
// CONST OPERATION_POOL*  pOperationPool : [in] Operation pool to query
// 
// Return Value   : Number of operations
// 
UINT  getOperationCount(CONST OPERATION_POOL*  pOperationPool)
{
   // Query list size
   return getListItemCount(pOperationPool->pOperationList);
}


/// Function name  : isOperationPoolEmpty
// Description     : Determines whether there are any operations currently executing
// 
// CONST OPERATION_POOL*  pOperationPool : [in] Operation pool to query
// 
// Return Value   : FALSE if there are operations executing, otherwise TRUE
// 
BOOL  isOperationPoolEmpty(CONST OPERATION_POOL*  pOperationPool)
{
   // Check for empty list
   return (getOperationCount(pOperationPool) == 0);
}


/// Function name  : setOperationPoolBatchMode
// Description     : Enable/Disable reporting of 'batch mode' progress instead of individual operation progress
// 
// OPERATION_POOL*  pOperationPool : [in] Operation pool
// CONST BOOL       bEnabled       : [in] TRUE/FALSE
// 
VOID  setOperationPoolBatchMode(OPERATION_POOL*  pOperationPool, CONST BOOL  bEnabled)
{
   // Set batch flag
   pOperationPool->bBatchOperation = bEnabled;
}

/// Function name  : setOperationPoolBatchProgress
// Description     : Sets the current batch mode progress
// 
// OPERATION_POOL*  pOperationPool : [in] Operation pool
// CONST UINT       iProgress      : [in] 0 <= Progress <= 10,000
// 
VOID  setOperationPoolBatchProgress(OPERATION_POOL*  pOperationPool, CONST UINT  iProgress)
{
   // Update progress
   pOperationPool->iBatchProgress = utilEnforceRange(iProgress, 0, 10000);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : getCurrentOperationData
// Description     : Retrieves the OperationData for the currently executing operation
// 
// CONST OPERATION_POOL*  pOperationPool : [in] Operation pool
// 
// Return Value   : Operation for the currently executing operation if any, otherwise NULL
// 
OPERATION_DATA*  getCurrentOperationData(CONST OPERATION_POOL*  pOperationPool)
{
   OPERATION_DATA*  pOutput;           // Operation result

   // [TRACK]
   TRACK_FUNCTION();

   // Prepare
   pOutput = NULL;

   // [CHECK] Ensure there are operations
   if (!isOperationPoolEmpty(pOperationPool))
      /// Lookup data for current operation
      findOperationDataByIndex(pOperationPool, (getOperationCount(pOperationPool) - 1), pOutput);

   // Return result
   END_TRACKING();
   return pOutput;
}


/// Function name  : getCurrentOperationProgress
// Description     : Calculates the current progress (as a percentage) of the currently executing operation
// 
// CONST OPERATION_POOL*  pOperationPool   : [in] Operation Pool
// 
// Return Value   : Value between 0 and 10,000 representing the current progress
// 
UINT  getCurrentOperationProgress(CONST OPERATION_POOL*  pOperationPool)
{
   OPERATION_DATA*  pCurrentOperation;    // Data for the current operation
   UINT             iOperationCount,
                    iResult;              // Current progress

   // Prepare
   TRACK_FUNCTION();
   iResult = 0;

   // [CHECK] Are we performing a batch operation?
   if (pOperationPool->bBatchOperation)
      /// [BATCH] Return batch operation progress
      iResult = pOperationPool->iBatchProgress;

   // [NORMAL] Examine operation count
   else switch (iOperationCount = getOperationCount(pOperationPool))
   {
   /// [NONE] Return zero
   case 0:
      break;

   /// [SINGLE] Return progress of current operation
   case 1:
      // Prepare
      pCurrentOperation = getCurrentOperationData(pOperationPool);

      // Calculate current progress
      iResult = calculateOperationProgressPercentage(pCurrentOperation->pProgress);
      break;

   /// [MULTIPLE] Return overall progress of all operations
   default:
      for (UINT  iIndex = 0; findOperationDataByIndex(pOperationPool, iIndex, pCurrentOperation); iIndex++)
         iResult += calculateOperationProgressPercentage(pCurrentOperation->pProgress);

      iResult /= iOperationCount;
      break;
   }
   
   // Return result
   END_TRACKING();
   return min(iResult, 10000);
}


/// Function name  : getCurrentOperationStageID
// Description     : Retrieves the resource ID of the description of the current stage of the currently execuing operation
// 
// CONST OPERATION_POOL*  pOperationPool   : [in] Operation Pool
// 
// Return Value   : Resource ID of the currently excuting stage, or NULL If there are no operations
// 
UINT  getCurrentOperationStageID(CONST OPERATION_POOL*  pOperationPool)
{
   OPERATION_DATA*  pCurrentOperation;    // Operation data for the currently excuting operation
   UINT             iResult;              // Resource ID of the description for the current stage of the current operation

   // Prepare
   TRACK_FUNCTION();
   iResult = NULL;

   // [CHECK] Ensure there is a current operation
   switch (getOperationCount(pOperationPool))
   {
   // [NO OPERATIONS] Return NULL
   case 0:
      break;

   /// [SINGLE OPERATION] Extract ID of the current stage
   case 1:
      pCurrentOperation = getCurrentOperationData(pOperationPool);
      iResult           = getOperationProgressStageID(pCurrentOperation->pProgress);
      break;

   /// [MUTLIPLE OPERATIONS] Return fixed string
   default:
      iResult = IDS_PROGRESS_MULTIPLE;
      break;
   }

   // Return result
   END_TRACKING();
   return iResult;
}


/// Function name  : identifyOperationStatusMessageID
// Description     : Returns the resource ID of the string describing the name or result of an operation
// 
// CONST OPERATION_DATA*  pOperation   : [in] Operation data
// 
// Return Value   : Resource ID of the operation name/result
// 
UINT  identifyOperationStatusMessageID(CONST OPERATION_DATA*  pOperation)
{
   UINT  iMessageID;
   
   // Extract operation name or result message
   switch (pOperation->eType)
   {
   case OT_LOAD_GAME_DATA:       iMessageID = (isOperationComplete(pOperation) ? IDS_OUTPUT_GAME_DATA_SUCCESS     : IDS_OUTPUT_GAME_DATA_LOADING);  break;
   case OT_LOAD_PROJECT_FILE:    iMessageID = (isOperationComplete(pOperation) ? IDS_OUTPUT_PROJECT_LOAD_SUCCESS  : IDS_OUTPUT_PROJECT_LOADING);    break;
   case OT_SAVE_PROJECT_FILE:    iMessageID = (isOperationComplete(pOperation) ? IDS_OUTPUT_PROJECT_SAVE_SUCCESS  : IDS_OUTPUT_PROJECT_SAVING);     break;
   case OT_LOAD_SCRIPT_FILE:     iMessageID = (isOperationComplete(pOperation) ? IDS_OUTPUT_SCRIPT_LOAD_SUCCESS   : IDS_OUTPUT_SCRIPT_LOADING);     break;
   case OT_LOAD_LANGUAGE_FILE:   iMessageID = (isOperationComplete(pOperation) ? IDS_OUTPUT_LANGUAGE_LOAD_SUCCESS : IDS_OUTPUT_LANGUAGE_LOADING);   break;
   case OT_SAVE_SCRIPT_FILE:     iMessageID = (isOperationComplete(pOperation) ? IDS_OUTPUT_SCRIPT_SAVE_SUCCESS   : IDS_OUTPUT_SCRIPT_SAVING);      break;
   case OT_SAVE_LANGUAGE_FILE:   iMessageID = (isOperationComplete(pOperation) ? IDS_OUTPUT_LANGUAGE_SAVE_SUCCESS : IDS_OUTPUT_LANGUAGE_SAVING);    break;
   case OT_SUBMIT_BUG_REPORT:    iMessageID = (isOperationComplete(pOperation) ? IDS_OUTPUT_SUBMIT_REPORT_SUCCESS : IDS_OUTPUT_SUBMIT_REPORT);      break;
   case OT_SUBMIT_CORRECTION:    iMessageID = (isOperationComplete(pOperation) ? IDS_OUTPUT_CORRECTION_SUCCESS    : IDS_OUTPUT_CORRECTION);         break;
   case OT_SEARCH_SCRIPT_CALLS:  iMessageID = (isOperationComplete(pOperation) ? IDS_OUTPUT_SCRIPT_SEARCH_SUCCESS : IDS_OUTPUT_SEARCHING_FOLDER);   break;
   case OT_VALIDATE_SCRIPT_FILE: iMessageID = (isOperationComplete(pOperation) ? IDS_OUTPUT_VALIDATION_SUCCESS    : IDS_OUTPUT_VALIDATING_SCRIPT);  break;

   case OT_SEARCH_FILE_SYSTEM:   
      ASSERT(FALSE);
   }

   // [COMPLETE] Calculate Success/Failure/Aborted message ID
   if (isOperationComplete(pOperation))
      iMessageID += pOperation->eResult;

   // Return result
   return iMessageID;
}


/// Function name  : insertOperationIntoOperationPool
// Description     : Appends an operation to the operation pool's list of currently executing operations
// 
// OPERATION_POOL*  pOperationPool   : [in] Operation Pool
// OPERATION_DATA*  pOperationData   : [in] Data for the operation to append
// 
VOID  insertOperationIntoOperationPool(OPERATION_POOL*  pOperationPool, OPERATION_DATA*  pOperationData)
{
   // [TRACK]
   TRACK_FUNCTION();

   // Append operation to the list
   insertObjectIntoListByIndex(pOperationPool->pOperationList, getListItemCount(pOperationPool->pOperationList), (LPARAM)pOperationData);

   // [TRACK]
   END_TRACKING();
}


/// Function name  : launchOperation
// Description     : Launches the worker thread for the desired operation
// 
// MAIN_WINDOW_DATA*     pMainWindowData : [in] Main window data
// OPERATION_DATA*       pOperationData  : [in] Data for the operation
///                                             This must have all the necessary properties set for the desired operation
// 
// Return Value   : TRUE if successful, FALSE otherwise
// 
BOOL   launchOperation(MAIN_WINDOW_DATA*  pMainWindowData, OPERATION_DATA*  pOperationData)
{
   LPTHREAD_START_ROUTINE  pfnOperationFunction;     // Function for the thread to run
   BOOL                    bResult;                  // Operation result

   // Prepare
   TRACK_FUNCTION();
   pfnOperationFunction = NULL;
   bResult              = FALSE;

   /// Determine thread function
   switch (pOperationData->eType)
   {
   case OT_LOAD_GAME_DATA:       pfnOperationFunction = threadprocLoadGameData;           break;
   case OT_LOAD_PROJECT_FILE:    pfnOperationFunction = threadprocLoadProjectFile;        break;
   case OT_SAVE_PROJECT_FILE:    pfnOperationFunction = threadprocSaveProjectFile;        break;
   case OT_LOAD_SCRIPT_FILE:     pfnOperationFunction = threadprocLoadScriptFile;         break;
   case OT_SAVE_SCRIPT_FILE:     pfnOperationFunction = threadprocSaveScriptFile;         break;
   case OT_LOAD_LANGUAGE_FILE:   pfnOperationFunction = threadprocLoadLanguageFile;       break;
   case OT_SAVE_LANGUAGE_FILE:   pfnOperationFunction = threadprocSaveLanguageFile;       break;
   case OT_SUBMIT_BUG_REPORT:    
   case OT_SUBMIT_CORRECTION:    pfnOperationFunction = threadprocSubmitReport;           break;
   case OT_SEARCH_SCRIPT_CALLS:  pfnOperationFunction = threadprocFindCallingScripts;     break;
   case OT_VALIDATE_SCRIPT_FILE: pfnOperationFunction = threadprocValidateScriptFile;     break;

   case OT_SEARCH_FILE_SYSTEM:   ASSERT(pOperationData->eType != OT_SEARCH_FILE_SYSTEM);  break;      // FileSearch is not controlled by the OperationPool
   }

   /// Ensure all notifications are sent to the operation pool control
   pOperationData->hParentWnd = pMainWindowData->hOperationPoolCtrl;

   // Display notification in output dialog
   displayOperationStatus(pOperationData);

   /// Attempt to launch new thread
   if (bResult = launchThreadedOperation(pfnOperationFunction, pOperationData))
   {
      /// [SUCCESS] Add to list of current operations and display progress dialog if necessary
      insertOperationIntoOperationPool(pMainWindowData->pOperationPool, pOperationData);

      // Examine operation
      if (pOperationData->eType == OT_LOAD_GAME_DATA)
      {
         ASSERT(!pMainWindowData->hSplashWindow);
         /// [GAME DATA] Display splash window
         pMainWindowData->hSplashWindow = displaySplashWindow(pMainWindowData);
      }
      else if (!pMainWindowData->hProgressDlg)  // May already exist
         /// [OTHER] Display progress dialog
         pMainWindowData->hProgressDlg = displayProgressDialog(pMainWindowData);
          
      // Disable drag'n'drop while the operation is in progress
      DragAcceptFiles(pMainWindowData->hMainWnd, FALSE);
   }

   // Return operation result
   END_TRACKING();
   return bResult;
}


/// Function name  : removeOperationFromOperationPool
// Description     : Removes and deletes an operation from the operation pool's list of currently executing operations
// 
// OPERATION_POOL*  pOperationPool  : [in] Operation pool
// OPERATION_DATA* &pOperationData  : [in] Data for the operation to destroy
// 
VOID  removeOperationFromOperationPool(OPERATION_POOL*  pOperationPool, OPERATION_DATA*  &pOperationData)
{
   ERROR_STACK*  pException;

   // [TRACK]
   TRACK_FUNCTION();

   __try
   {
      /// Remove from operations list and destroy
      if (removeObjectFromListByValue(pOperationPool->pOperationList, (LPARAM)pOperationData))
         deleteOperationData(pOperationData);

      // [CHECK] Ensure operation was destroyed
      ASSERT(!pOperationData);
   }
   /// [EXCEPTION HANDLER]
   __except (generateExceptionError(GetExceptionInformation(), pException))
   {
      // [ERROR] "An unidentified and unexpected critical error has occurred during completion of the %s operation"
      enhanceError(pException, ERROR_ID(IDS_EXCEPTION_OPERATION_COMPLETE), pOperationData ? identifyOperationTypeString(pOperationData->eType) : TEXT(""));
      displayException(pException);
   }

   // [TRACK]
   END_TRACKING();
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onOperationPoolDestroy
// Description     : Cleanup the window
// 
// OPERATION_POOL*  pOperationPool   : [in] Operation pool to destroy
// 
VOID  onOperationPoolDestroy(OPERATION_POOL*  pOperationPool)
{
   // [TRACK]
   TRACK_FUNCTION();

   // Sever window data
   SetWindowLong(pOperationPool->hWnd, 4, NULL);

   // Un-subclass control
   SubclassWindow(pOperationPool->hWnd, pOperationPool->wndprocProgressBar);

   // [TRACK]
   END_TRACKING();
}


/// Function name  : onOperationPoolOperationComplete
// Description     : Notifies the appropriate window that an operation has completed, deletes the operation
//                         and hides the progress dialog if necessary
// 
// OPERATION_POOL*  pOperationPool   : [in] Operation pool
// OPERATION_DATA*  pOperationData   : [in] Data for the recently completed operation
// 
VOID    onOperationPoolOperationComplete(OPERATION_POOL*  pOperationPool, OPERATION_DATA*  pOperationData)
{
   DOCUMENT_OPERATION*    pDocumentOperation;     // Convenience pointer
   SCRIPTCALL_OPERATION*  pScriptCallOperation;   // Convenience pointer
   MAIN_WINDOW_DATA*      pMainWindowData;        // Main window data
   ERROR_STACK*           pException;

   // Prepare
   TRACK_FUNCTION();
   pMainWindowData = getMainWindowData();
      
   __try
   {
      /// [OUTPUT] Print result and errors to the OutputDialog
      displayOperationStatus(pOperationData);

      // Notify the appropriate window
      switch (pOperationData->eType)
      {
      /// [GAME DATA/SUBMISSION] Inform main window
      case OT_LOAD_GAME_DATA:
      case OT_SUBMIT_BUG_REPORT:
      case OT_SUBMIT_CORRECTION:
         sendOperationComplete(AW_MAIN, pOperationData); 
         break;

      /// [LOAD/SAVE DOCUMENT] Inform document control
      case OT_LOAD_SCRIPT_FILE:
      case OT_SAVE_SCRIPT_FILE:
      case OT_LOAD_PROJECT_FILE:
      case OT_SAVE_PROJECT_FILE:
      case OT_LOAD_LANGUAGE_FILE:
      case OT_SAVE_LANGUAGE_FILE:
      case OT_VALIDATE_SCRIPT_FILE:
         // Prepare
         pDocumentOperation = (DOCUMENT_OPERATION*)pOperationData;

         // Free I/O buffers
         deleteGameFileIOBuffers(pDocumentOperation->pGameFile);

         // Notify document control
         sendOperationComplete(AW_DOCUMENTS_CTRL, pOperationData);
         break;

      /// [SCRIPT-CALL SEARCH]
      case OT_SEARCH_SCRIPT_CALLS:
         // Prepare
         pScriptCallOperation = (SCRIPTCALL_OPERATION*)pOperationData;

         // Notify properties dialog
         postScriptCallSearchComplete(AW_PROPERTIES, pScriptCallOperation->pCallersTree);
         break;
      }
   }
   /// [EXCEPTION HANDLER]
   __except (generateExceptionError(GetExceptionInformation(), pException))
   {
      // [ERROR] "An unidentified and unexpected critical error has occurred during completion of the %s operation"
      enhanceError(pException, ERROR_ID(IDS_EXCEPTION_OPERATION_COMPLETE), identifyOperationTypeString(pOperationData->eType));
      displayException(pException);
   }

   // [FAILURE] Beep!
   if (!isOperationSuccessful(pOperationData))
      MessageBeep(MB_ICONWARNING); 
   
   /// Remove and delete the completed operation from the operation pool
   removeOperationFromOperationPool(pOperationPool, pOperationData);
   
   // [FINAL OPERATION] Hide progress dialog
   if (isOperationPoolEmpty(pOperationPool))
   {
      /// [PROGRESS] Close progress dialog
      if (pMainWindowData->hProgressDlg)     // Doesn't exist for LoadGameData operations
      {
         // Enable main window and hide progress dialog
         EnableWindow(getAppWindow(), TRUE);
         utilDeleteWindow(pMainWindowData->hProgressDlg);
      }

      // Re-enable drag 'n' drop
      DragAcceptFiles(pMainWindowData->hMainWnd, TRUE);
   }

   // Refresh main window and it's toolbar
   updateMainWindowToolBar(pMainWindowData);
   //InvalidateRect(pMainWindowData->hMainWnd, NULL, FALSE);

   // [CHECK] Ensure memory is OK
   ASSERT(utilValidateMemory());

   // [TRACK]
   END_TRACKING();
}


/// Function name  : onOperationPoolProcessingError
// Description     : Displays a threaded operation processing error to the user and determines how to proceed
// 
// OPERATION_POOL*            pOperationPool : [in] Operation pool
// CONST MESSAGE_DIALOG_FLAG  eButtons       : [in] Buttons to display in dialog, must be MDF_YESNO
// CONST ERROR_STACK*         pError         : [in] Error message to display. Should contain a question on the top of the stack
// 
// Return Value   : IGNORE - The user chose to ignore the error
//                  ABORT  - The user requested further processing should be aborted
//
ERROR_HANDLING  onOperationPoolProcessingError(OPERATION_POOL*  pOperationPool, CONST UINT  eButtons, CONST ERROR_STACK*  pError)
{
   UINT  iResult;    // Dialog result

   // [TRACK]
   TRACK_FUNCTION();

   // [CHECK] Ensure buttons are YESNO and error isn't 'Information'
   ASSERT(eButtons == MDF_YESNO AND pError->eType != ET_INFORMATION);

   // [WINDOWS 7] Pause the taskbar progressbar
   if (getAppWindowsVersion() >= WINDOWS_7)
      utilSetWindowProgressState(getAppWindow(), TBPF_PAUSED);

   /// Display dialog
   iResult = displayErrorMessageDialog(NULL, pError, TEXT("Error During Processing"), eButtons);

   // [WINDOWS 7] Resume the taskbar progressbar
   if (getAppWindowsVersion() >= WINDOWS_7)
      utilSetWindowProgressState(getAppWindow(), TBPF_NORMAL);

   // Convert result to IGNORE or ABORT
   END_TRACKING();
   return (iResult == IDYES ? EH_IGNORE : EH_ABORT);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : wndprocOperationPool
// Description     : Window procedure for the operation pool
// 
// 
LRESULT  wndprocOperationPool(HWND  hCtrl, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   OPERATION_POOL*  pOperationPool;    // Operation Pool
   ERROR_STACK*     pException;        // Exception error
   LRESULT          iResult;           // Operation result

   // [TRACK]
   TRACK_FUNCTION();

   /// [GUARD BLOCK]
   __try
   {
      // Prepare
      iResult = 0;
      pOperationPool = (OPERATION_POOL*)GetWindowLong(hCtrl, 4);  

      // Examine message
      switch (iMessage)
      {
      /// [OPERATION COMPLETE]
      case UN_OPERATION_COMPLETE:
         onOperationPoolOperationComplete(pOperationPool, (OPERATION_DATA*)lParam);
         break;

      /// [PROCESSING ERROR]
      case UM_PROCESSING_ERROR:
         onOperationPoolProcessingError(pOperationPool, wParam, (ERROR_STACK*)lParam);
         break;

      /// [DESTROY] Sever window data
      case WM_DESTROY:
         onOperationPoolDestroy(pOperationPool);
         iResult = CallWindowProc(pOperationPool->wndprocProgressBar, hCtrl, iMessage, wParam, lParam);
         break;

      // [UNHANDLED]
      default:
         iResult = CallWindowProc(pOperationPool->wndprocProgressBar, hCtrl, iMessage, wParam, lParam);
         break;
      }
   }
   /// [EXCEPTION HANDLER]
   __except (generateExceptionError(GetExceptionInformation(), pException))
   {
      // [ERROR] "An unidentified and unexpected critical error has occurred in the operation pool control"
      enhanceError(pException, ERROR_ID(IDS_EXCEPTION_OPERATION_POOL));
      displayException(pException);
      
      // Ensure main window is re-enabled
      EnableWindow(getAppWindow(), TRUE);
   }

   // Result result
   END_TRACKING();
   return 0;
}

