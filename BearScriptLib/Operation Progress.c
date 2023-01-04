//
// Operation Progress.cpp : Handles the progress tracking of multi-threaded operations
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

/// Function name  : createOperationProgress
// Description     : Creates data for a OperationProgress
// 
// Return Value   : New OperationProgress, you are responsible for destroying it
// 
BearScriptAPI
OPERATION_PROGRESS*   createOperationProgress()
{
   OPERATION_PROGRESS*   pProgress;       // Object being created

   // Create object
   pProgress = utilCreateEmptyObject(OPERATION_PROGRESS);

   // Set properties
   pProgress->pStages       = createList(destructorSimpleObject);
   pProgress->pCurrentStage = NULL;

   // Return object
   return pProgress;
}

/// Function name  : createProgressStage
// Description     : Creates data for a ProgressStage
// 
// Return Value   : New ProgressStage, you are responsible for destroying it
// 
BearScriptAPI
PROGRESS_STAGE*   createProgressStage(CONST UINT  iResourceID)
{
   PROGRESS_STAGE*   pProgressStage;       // Object being created

   // Create object
   pProgressStage = utilCreateEmptyObject(PROGRESS_STAGE);

   // Set properties
   pProgressStage->iResourceID = iResourceID;
   pProgressStage->iMaximum    = 100;
   pProgressStage->iProgress   = 0;

   // Return object
   return pProgressStage;
}


/// Function name  : deleteOperationProgress
// Description     : Destroys the data for the OperationProgress
// 
// OPERATION_PROGRESS*  &pProgress   : [in] OperationProgress to destroy
// 
BearScriptAPI
VOID  deleteOperationProgress(OPERATION_PROGRESS*  &pProgress)
{
   // Destroy contents
   deleteList(pProgress->pStages);

   // Destroy calling object
   utilDeleteObject(pProgress);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : addStageToOperationProgress
// Description     : Adds a new stage to a progress object
// 
// OPERATION_PROGRESS*  pProgress   : [in] Progress tracker
// CONST UINT           iResourceID : [in] Resource ID of new stage name
// 
BearScriptAPI
VOID  addStageToOperationProgress(OPERATION_PROGRESS*  pProgress, CONST UINT  iResourceID)
{
   PROGRESS_STAGE*  pStage;

   // Prepare
   pStage = createProgressStage(iResourceID);

   /// Create new stage and append to stages list
   appendObjectToList(pProgress->pStages, (LPARAM)pStage);

   // [CHECK] Set initial stage
   if (!pProgress->pCurrentStage)
      pProgress->pCurrentStage = pStage;
}


/// Function name  : advanceOperationProgressStage
// Description     : Advances operation progress to the next stage
// 
// OPERATION_PROGRESS*  pProgress : [in] Progress tracker
// 
// Return Value   : Resource ID of new stage
// 
BearScriptAPI
UINT  advanceOperationProgressStage(OPERATION_PROGRESS*  pProgress)
{
   LIST_ITEM*   pStageItem;

   // [CHECK] Ensure we have a current stage
   ASSERT(pProgress->pCurrentStage);

   /// [STAGE] Find current stage ListItem wrapper
   findListItemByValue(pProgress->pStages, (LPARAM)pProgress->pCurrentStage, pStageItem);

   // [CHECK] Ensure this isn't the last stage
   ASSERT(pStageItem->pNext);

   /// Move to the next stage
   pProgress->pCurrentStage = extractListItemPointer(pStageItem->pNext, PROGRESS_STAGE);
   pProgress->iStageIndex++;

   // Return resource ID of new stage
   return pProgress->pCurrentStage->iResourceID;
}


/// Function name  : advanceOperationProgressValue
// Description     : Increment the progress of the current stage of an OperationProgress object
// 
// OPERATION_PROGRESS*  pProgress : [in] OperationProgress object to increment
// 
BearScriptAPI 
VOID  advanceOperationProgressValue(OPERATION_PROGRESS*  pProgress)
{
   // [CHECK] Ensure progress exists
   if (pProgress)
   {
      // [CHECK] Validate current stage
      ASSERT(pProgress->pCurrentStage);

      // Increment stage progress
      pProgress->pCurrentStage->iProgress++;
   }
}


/// Function name  : calculateOperationProgressPercentage
// Description     : Calculate the progress percentage of a the entire operation
// 
// CONST OPERATION_PROGRESS*  pProgress : [in] Progress object
// 
// Return Value   : Percentage represented by the range 0 <= n <= 10,000
// 
BearScriptAPI
UINT  calculateOperationProgressPercentage(CONST OPERATION_PROGRESS*  pProgress)
{
   UINT  iAverageStageSize,         // Percentage that represents a completed stage
         iCurrentStageProgress;     // Percentage that represents the progress of the current stage
   
   // [CHECK] Validate current stage
   ASSERT(pProgress->pCurrentStage AND pProgress->pCurrentStage->iMaximum);

   // Determine per-stage percentage
   iAverageStageSize = (10000 / getListItemCount(pProgress->pStages));

   // Calculate completed portion of current stage
   iCurrentStageProgress = (iAverageStageSize * pProgress->pCurrentStage->iProgress / pProgress->pCurrentStage->iMaximum);

   // Return : Complete stages + portion of current stage
   return (iAverageStageSize * pProgress->iStageIndex) + iCurrentStageProgress;
}


/// Function name  : calculateOperationProgressStagePercentage
// Description     : Calculate the progress percentage of the current stage
// 
// CONST OPERATION_PROGRESS*  pProgress : [in] Progress object
// 
// Return Value   : Percentage represented by the range 0 <= n <= 10,000
// 
BearScriptAPI
UINT  calculateOperationProgressStagePercentage(CONST OPERATION_PROGRESS*  pProgress)
{
   // [CHECK] Validate current stage
   ASSERT(pProgress->pCurrentStage AND pProgress->pCurrentStage->iMaximum);

   // Return percentage between 0 and 10,000
   return (10000 * pProgress->pCurrentStage->iProgress / pProgress->pCurrentStage->iMaximum);
}


/// Function name  : getOperationProgressStageID
// Description     : Retrieve the Resource ID of the current stage
// 
// CONST OPERATION_PROGRESS*  pProgress   : [in] Progress object
// 
// Return Value   : Resource ID of the current stage
// 
BearScriptAPI
UINT  getOperationProgressStageID(CONST OPERATION_PROGRESS*  pProgress)
{
   // [CHECK] Validate current stage
   ASSERT(pProgress->pCurrentStage);

   // Return resource ID of the current stage
   return pProgress->pCurrentStage->iResourceID;
}


/// Function name  : updateOperationProgressMaximum
// Description     : Set the maximum progress for the current operation stage. Resets the current progress to zero
// 
// OPERATION_PROGRESS*   pProgress : [in] OperationProgress object to alter
// CONST UINT            iMaximum  : [in] New maximum
// 
BearScriptAPI 
VOID  updateOperationProgressMaximum(OPERATION_PROGRESS*  pProgress, CONST UINT  iMaximum)
{
   // [CHECK] Ensure progress object exists
   if (pProgress)
   {
      // [CHECK] Validate current stage
      ASSERT(pProgress->pCurrentStage AND pProgress->pCurrentStage->iMaximum);

      // Set maximum and reset progress
      pProgress->pCurrentStage->iMaximum  = utilEither(iMaximum, 100);     // [CHECK] Prevent max even being set to zero
      pProgress->pCurrentStage->iProgress = 0;
   }
}


/// Function name  : updateOperationProgressValue
// Description     : Set the progress of the current stage to a specific value
// 
// OPERATION_PROGRESS*   pProgress : [in] OperationProgress object to alter
// CONST UINT            iValue    : [in] New progress value for the current stage
// 
BearScriptAPI 
VOID  updateOperationProgressValue(OPERATION_PROGRESS*  pProgress, CONST UINT  iValue)
{
   // [CHECK] Validate current stage
   ASSERT(pProgress->pCurrentStage);

   // Set stage progress
   pProgress->pCurrentStage->iProgress = iValue;
}



