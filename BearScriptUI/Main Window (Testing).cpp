//
// FILE_NAME.cpp : FILE_DESCRIPTION
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

// Batch test data
BATCH_TEST  oBatchTest;

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : commandScriptValidationBatchTest
// Description     : Performs batch testing on the next file, or aborts the test
// 
// MAIN_WINDOW_DATA*         pWindowData  : [in] Window data
// CONST BATCH_TEST_COMMAND  eCommand     : [in] Command
// 
// Return Value   : TRUE if another test was started, FALSE if no more tests remain
// 
BOOL  commandScriptValidationBatchTest(MAIN_WINDOW_DATA*  pWindowData, CONST BATCH_TEST_COMMAND  eCommand)
{
   LOADING_OPTIONS  oLoadOptions;
   FILE_ITEM*       pSearchResult;

   // [TRACK]
   TRACK_FUNCTION();

   // Prepare
   utilZeroObject(&oLoadOptions, LOADING_OPTIONS);

   // [CHECK] Abort if we have finished testing
   if (!oBatchTest.pFileSearch)
      return FALSE;

   // Examine command
   switch (eCommand)
   {
   case BTC_NEXT_FILE:
      // Setup options
      oLoadOptions.bBatchCompilerTest = TRUE;
      oLoadOptions.eCompilerTest      = ODT_GENERATION;
      
      // [BATCH TESTING] Load the next document in the sequence
      while (findNextFileSearchResult(oBatchTest.pFileSearch, pSearchResult))
      {
         // [CHECK] Skip folders
         if (pSearchResult->iAttributes INCLUDES FIF_FOLDER)
            continue;

         /// [XRM] Skip X3TC scripts that contain X3AP constants
         if (utilCompareString(pSearchResult->szDisplayName, "plugin.rrf.xrm.repair.restock.xml") OR
             utilCompareString(pSearchResult->szDisplayName, "plugin.xrm.barter.xml"))
             continue;

         /// [XRM] Skip X3TC scripts that intentionally contain X3AP sectors to test for presence of X3:TC
         if (utilCompareString(pSearchResult->szDisplayName, "plugin.xrm.bounty.plkiller.xml") OR
             utilCompareString(pSearchResult->szDisplayName, "plugin.xrm.bounty.plkillerst.xml") OR 
             utilCompareString(pSearchResult->szDisplayName, "plugin.xrm.milnet.xml") OR
             utilCompareString(pSearchResult->szDisplayName, "plugin.xrm.tradedock.xml"))
            continue;

         /// [XTC] 
         if (utilCompareString(pSearchResult->szDisplayName, "!job.trade.planetsupply.pck") OR        // References a biological ware with subtype 15, which doesn't exist
             utilCompareString(pSearchResult->szDisplayName, "!move.preach.friendly.pck") OR          // Scriptname doesn't match filename, description in code-array different to description node
             utilCompareString(pSearchResult->szDisplayName, "plugin.news.show.newspaper.xml") OR     // Has character entities that aren't use in generation
             utilCompareString(pSearchResult->szDisplayName, "plugin.XTC.Job.Beetle.FlyMine.xml") OR  // Subroutine causes two variables in CodeArray defined in different order
             utilCompareString(pSearchResult->szDisplayName, "plugin.XTC.Msn.DH.07.xml") OR           // Subroutine causes two variables in CodeArray defined in different order
             utilCompareString(pSearchResult->szDisplayName, "plugin.XTC.POSY.Produce.Ship.xml") OR   // Subroutine causes two variables in CodeArray defined in different order
             utilCompareString(pSearchResult->szDisplayName, "plugin.XTC.Task.DH.Hack.Master.xml") OR // Subroutine causes two variables in CodeArray defined in different order
             utilCompareString(pSearchResult->szDisplayName, "plugin.XTC.Task.Wing.Dragon.xml"))      // Subroutine causes two variables in CodeArray defined in different order
             continue;

         /// [IR v2]
         if (utilCompareString(pSearchResult->szDisplayName, "xtl.xtlc.comm.order.customize.xml"))    // References a shield with subtype 11, which doesn't exist
             continue;

         /// [IR v1]
         if (utilCompareString(pSearchResult->szDisplayName, "plugin.ir.comm.ship.config.xml") OR        // Contains expanded apostrophes. These are converted to condensed apostrophes on save.
             utilCompareString(pSearchResult->szDisplayName, "plugin.ir.menu.new.xml") OR                // Contains expanded apostrophes. These are converted to condensed apostrophes on save.
             utilCompareString(pSearchResult->szDisplayName, "plugin.ir.nospawn.xml"))                   // ScriptName doesn't match filename -- file is automatically renamed
             continue;

         /// [MARS]
         if (utilCompareString(pSearchResult->szDisplayName, "lib.chem.strings.xml") OR                  // Extended script properties have been removed by author, in favour of the codearray
             utilCompareString(pSearchResult->szDisplayName, "plugin.gz.mars.hk.sat.dep.xml") OR         // Subroutine causes two variables in CodeArray defined in different order
             utilCompareString(pSearchResult->szDisplayName, "plugin.gz.missile.load.att.e.adv.xml") OR  // Contains an invalid return object, NULL instead of CI_DISCARD. May have been edited by hand? don't understand.
             utilCompareString(pSearchResult->szDisplayName, "plugin.gz.missile.load.att.e.std.xml"))    // Contains an invalid return object, NULL instead of CI_DISCARD. May have been edited by hand? don't understand.
             continue;

         /// [CYCROW CHEATS]
         if (utilCompareString(pSearchResult->szDisplayName, "z.cheat.cycrow.createship.xml") OR      // Variable order different
             utilCompareString(pSearchResult->szDisplayName, "z.cheat.cycrow.createshipmenu.xml") OR  // Variable order different
             utilCompareString(pSearchResult->szDisplayName, "z.cheat.cycrow.createwing.xml"))        // Contains 'get object name array' with hidden parameter
             continue;

         // [PROGRESS] Update progress
         setOperationPoolBatchProgress(pWindowData->pOperationPool, oBatchTest.pFileSearch->iCurrentResult * 10000 / getFileSearchResultCount(oBatchTest.pFileSearch));

         /// [FOUND] Load next batch file, pass remaining FileSearch
         commandLoadDocument(pWindowData, FIF_SCRIPT, pSearchResult->szFullPath, TRUE, &oLoadOptions);
         break;
      }

      // [CHECK] Have we loaded a result?
      if (pSearchResult)
         /// [SUCCESS] Return TRUE
         return TRUE;

      /// [NO MORE RESULTS]
      // Fall through...

   case BTC_STOP:
      // Destroy FileSearch
      if (oBatchTest.pFileSearch)
         deleteFileSearch(oBatchTest.pFileSearch);

      // [PROGRESS] Disable batch mode
      setOperationPoolBatchMode(pWindowData->pOperationPool, FALSE);

      // [REDRAW] Unfreeze painting in documents control
      SetWindowRedraw(pWindowData->hDocumentsTab, TRUE);
      ShowWindow(pWindowData->hDocumentsTab, TRUE);
      break;
   }

   // Return FALSE
   END_TRACKING();
   return FALSE;
}


/// Function name  : performScriptValidationBatchTest
// Description     : Validates all scripts in the script folder
// 
// MAIN_WINDOW_DATA*  pWindowData   : [in] Window data
// 
VOID  performScriptValidationBatchTest(MAIN_WINDOW_DATA*  pWindowData, CONST FILE_FILTER  eScriptType)
{
   SHFILEOPSTRUCT   oDeleteFolder;
   TCHAR           *szScriptFolder,
                   *szValidationFolder;

   // [TRACK]
   TRACK_FUNCTION();

   // Prepare
   utilZeroObject(&oDeleteFolder, SHFILEOPSTRUCT);

   // [CHECK] Ensure no documents are open
   if (getDocumentCount())
   {
      // [ERROR] "You must close all documents before performing a script validation batch test"
      displayMessageDialogf(NULL, IDS_GENERAL_VALIDATION_DOCUMENTS_OPEN, TEXT("Cannot Execute Validation Batch Test"), MDF_OK WITH MDF_ERROR);
      return;
   }

   // Generate script and validation folders
   szScriptFolder     = generateGameDataFilePath(GFI_SCRIPT_FOLDER, getAppPreferences()->szGameFolder, getAppPreferences()->eGameVersion);
   szValidationFolder = utilCreateStringf(MAX_PATH, TEXT("%sCode.Validation\\*.xml\0\0"), szScriptFolder);

   // Setup delete operation
   oDeleteFolder.hwnd   = getAppWindow();
   oDeleteFolder.wFunc  = FO_DELETE;
   oDeleteFolder.pFrom  = szValidationFolder;
   oDeleteFolder.fFlags = FOF_NOERRORUI WITH FOF_NOCONFIRMATION WITH FOF_FILESONLY;

   /// [CLEAN] Delete previous validation files
   SHFileOperation(&oDeleteFolder);

   /// [SEARCH] Perform search for all X3:AP scripts
   oBatchTest.pFileSearch = performFileSystemSearch(getFileSystem(), szScriptFolder, eScriptType, AK_PATH, AO_DESCENDING, NULL);

   // [REDRAW] Freeze painting in documents control
   SetWindowRedraw(pWindowData->hDocumentsTab, FALSE);
   ShowWindow(pWindowData->hDocumentsTab, FALSE);

   // [PROGRESS] Enable batch mode
   setOperationPoolBatchMode(pWindowData->pOperationPool, TRUE);
   setOperationPoolBatchProgress(pWindowData->pOperationPool, 0);

   /// Load multiple batch files
   for (UINT  iConcurrent = 1; iConcurrent <= 3; iConcurrent++)
      commandScriptValidationBatchTest(pWindowData, BTC_NEXT_FILE);

   // Cleanup
   utilDeleteStrings(szScriptFolder, szValidationFolder);
   END_TRACKING();
}


