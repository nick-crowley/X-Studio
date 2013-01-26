//
// Game String Trees.cpp : Operations upon trees of GameStrings
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        CONSTANTS / GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////



/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        CREATION / DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createGamePageTreeByID
// Description     : Creates an GamePage tree sorted by ID in ascending order
// 
// Return Value   : New AVLTree, you are responsible for destroying it
//  
AVL_TREE*  createGamePageTreeByID()
{
   return createAVLTree(extractGamePageTreeNode, deleteGamePageTreeNode, createAVLTreeSortKey(AK_PAGE_ID, AO_ASCENDING), NULL, NULL);
}


/// Function name  : createGameStringTreeByPageID
// Description     : Creates an GameStrings tree sorted by PageID -> StringID. Used in LanguageFiles
// 
// Return Value   : New AVLTree, you are responsible for destroying it
//  
AVL_TREE*  createGameStringTreeByPageID()
{
   return createAVLTree(extractGameStringTreeNode, deleteGameStringTreeNode, createAVLTreeSortKey(AK_PAGE_ID, AO_ASCENDING), createAVLTreeSortKey(AK_ID, AO_ASCENDING), NULL);
}


/// Function name  : createGameStringTreeByText
// Description     : Creates a GameStrings tree but sorted by TEXT, arranged in alphabetical order
// 
// Return Value   : New AVLTree, you are responsible for destroying it
//  
AVL_TREE*  createGameStringTreeByText()
{
   return createAVLTree(extractGameStringTreeNode, NULL, createAVLTreeSortKey(AK_TEXT, AO_ASCENDING), NULL, NULL);
}


/// Function name  : createGameStringTreeByPageID
// Description     : Creates an GameStrings tree sorted by GameVersion -> PageID -> StringID. Used in DescriptionFiles
// 
// Return Value   : New AVLTree, you are responsible for destroying it
// 
AVL_TREE*  createGameStringTreeByVersion()
{
   return createAVLTree(extractGameStringTreeNode, deleteGameStringTreeNode, createAVLTreeSortKey(AK_VERSION, AO_ASCENDING), createAVLTreeSortKey(AK_PAGE_ID, AO_ASCENDING), createAVLTreeSortKey(AK_ID, AO_ASCENDING));
}


/// Function name  : deleteGamePageTreeNode
// Description     : Deletes a GamePage within an AVLTree Node
// 
// LPARAM  pData : [in] Reference to a GamePage pointer
// 
VOID   deleteGamePageTreeNode(LPARAM  pData)
{
   // Delete node contents
   deleteGamePage((GAME_PAGE*&)pData);
}


/// Function name  : deleteGameStringTreeNode
// Description     : Deletes a GameString within an AVLTree Node
// 
// LPARAM  pData : [in] Reference to a GameString pointer
// 
VOID   deleteGameStringTreeNode(LPARAM  pData)
{
   // Delete node contents
   deleteGameString((GAME_STRING*&)pData);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                               HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : extractGameStringTreeNode
// Description     : Extract the desired property from a given TreeNode containing a GameString
// 
// LPARAM                   pNode      : [in] GAME_STRING* : Node data containing a GameString
// CONST AVL_TREE_SORT_KEY  eProperty  : [in] Property to retrieve
// 
// Return Value   : Value of the specified property or NULL if the property is unsupported
// 
BearScriptAPI
LPARAM  extractGameStringTreeNode(LPARAM  pNode, CONST AVL_TREE_SORT_KEY  eProperty)
{
   GAME_STRING*  pGameString;    // Convenience pointer
   LPARAM        xOutput;        // Property value output

   // Prepare
   pGameString = (GAME_STRING*)pNode;

   // Examine property
   switch (eProperty)
   {
   case AK_TEXT:     xOutput = (LPARAM)pGameString->szText;       break;
   case AK_ID:       xOutput = (LPARAM)pGameString->iID;          break;
   case AK_PAGE_ID:  xOutput = (LPARAM)pGameString->iPageID;      break;
   case AK_VERSION:  xOutput = (LPARAM)pGameString->eVersion;     break;
   default:          xOutput = NULL;                              break;
   }

   // Return property value
   return xOutput;
}


/// Function name  : extractGamePageTreeNode
// Description     : Extract the desired property from a given TreeNode containing a GamePage
// 
// LPARAM                   pNode      : [in] GAME_PAGE* : Node data containing a GamePage
// CONST AVL_TREE_SORT_KEY  eProperty  : [in] Property to retrieve
// 
// Return Value   : Value of the specified property or NULL if the property is unsupported
// 
BearScriptAPI
LPARAM  extractGamePageTreeNode(LPARAM  pNode, CONST AVL_TREE_SORT_KEY  eProperty)
{
   GAME_PAGE*  pGamePage;    // Convenience pointer
   LPARAM      xOutput;      // Property value output

   // Prepare
   pGamePage = (GAME_PAGE*)pNode;

   // Examine property
   switch (eProperty)
   {
   default:          xOutput = NULL;                            break;
   case AK_TEXT:     xOutput = (LPARAM)pGamePage->szTitle;      break;
   case AK_PAGE_ID:  xOutput = (LPARAM)pGamePage->iPageID;      break;

   // [GROUP] Derived property
   case AK_GROUP:
      if (pGamePage->iPageID >= 6000)
         xOutput = GPG_USER;
      else if (pGamePage->iPageID >= 3800)
         xOutput = GPG_NPC;
      else if (pGamePage->iPageID >= 3200)
         xOutput = GPG_PLOT;
      else if (pGamePage->iPageID >= 2032)
         xOutput = GPG_NEWS;
      else if (pGamePage->iPageID >= 1999)
         xOutput = GPG_EDITOR;
      else if (pGamePage->iPageID >= 1701)
         xOutput = GPG_MENU;
      else if (pGamePage->iPageID >= 1500)
         xOutput = GPG_BONUS;
      else if (pGamePage->iPageID >= 1290)
         xOutput = GPG_QUEST;
      else if (pGamePage->iPageID >= 1000)
         xOutput = GPG_MISC;
      else if (pGamePage->iPageID >= 101)
         xOutput = GPG_DIALOGUE;
      else
         xOutput = GPG_DATA;
      break;
   }

   // Return property value
   return xOutput;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                              FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : insertLanguageFileIntoGameData
// Description     : Inserts a processed master/supplementary language file into the game string trees
// 
// LANGUAGE_FILE*       pLanguageFile : [in/out] Language file containing the GameString and GamePages to insert
///                                                Master:        Trees are severed and used as the basis of the game data.  (GameData trees must be NULL)
///                                                Supplementary: Contents of the trees are added to the existing game data. (GameData trees must exist)
// ERROR_QUEUE*         pErrorQueue   : [in/out] Contains Warning messages generated from overwriting existing game strings
// 
// Return Value   : Number of strings overwritten 
// 
UINT  insertLanguageFileIntoGameData(LANGUAGE_FILE*  pLanguageFile, ERROR_QUEUE*  pErrorQueue)
{
   UINT    iStringsOverwritten;

   // [FUNCTION]
   TRACK_FUNCTION();

   // Prepare
   iStringsOverwritten = 0;

   /// [MASTER] Sever trees and use as game data
   if (isLanguageFileMaster(pLanguageFile))
   {
      // [CHECK] Ensure GameData does not exist
      ASSERT(!getGameData()->pGameStringsByID AND !getGameData()->pGamePagesByID);

      // Transfer LanguageFile GameString/GamePage trees 
      transferAVLTree(pLanguageFile->pGamePagesByID, getGameData()->pGamePagesByID);
      transferAVLTree(pLanguageFile->pGameStringsByID, getGameData()->pGameStringsByID);

      // Index GamePages for LanguageDocuments
      performAVLTreeIndexing(getGameData()->pGamePagesByID);

      // Return number of strings in the LanguageFile
      iStringsOverwritten = getTreeNodeCount(getGameData()->pGameStringsByID);
   }
   else
   {
      // [CHECK] Ensure GameData exists
      ASSERT(getGameData()->pGameStringsByID AND getGameData()->pGamePagesByID);

      /// [SUPPLEMENTARY] Merge contents into the game data
      VERBOSE("Merging supplementary language file '%s' into the game data", PathFindFileName(pLanguageFile->szFullPath));

      /// Merge pages into the game data
      performLanguageFileGamePageMerge(pLanguageFile);

      /// Merge strings into the game data
      iStringsOverwritten = performLanguageFileGameStringMerge(pLanguageFile, pErrorQueue);
   }

   // [SUCCESS] Return iStringsOverwritten
   END_TRACKING();
   return iStringsOverwritten;
}


/// Function name  : insertSpecialCasesIntoGameStringTrees
// Description     : Perform the special case processing for the GameStrings tree, adding a few parameter syntax entries
// 
// Return Value   : TRUE
// 
BOOL  insertSpecialCasesIntoGameStringTrees()
{
   GAME_STRING*  pGameString;       // GameString being updated/inserted
   AVL_TREE*     pGameStringTree;   // Convenience pointer

   // Prepare
   pGameStringTree = getGameData()->pGameStringsByID;

   // [CHECK] Ensure tree exists
   ASSERT(pGameStringTree);

   // [PLAYER] -- Insert the 'PLAYER' race (it's not in the language files)
   pGameString = createGameString(TEXT("Player"), 10, identifyGamePageIDFromDataType(DT_RACE), ST_INTERNAL);
   if (!insertObjectIntoAVLTree(pGameStringTree, (LPARAM)pGameString))
      deleteGameString(pGameString);

   // [THIS] -- Insert another '[THIS]' with ID 0, used by older scripts
   pGameString = createGameString(TEXT("THIS"), 0, identifyGamePageIDFromDataType(DT_CONSTANT), ST_INTERNAL);
   if (!insertObjectIntoAVLTree(pGameStringTree, (LPARAM)pGameString))
      deleteGameString(pGameString);

   // [SSTYPE_DEBRIS] -- Insert 'ShipWreck' main-type
   pGameString = createGameString(TEXT("SSTYPE_DEBRIS"), 228, identifyGamePageIDFromDataType(DT_CONSTANT), ST_INTERNAL);
   if (!insertObjectIntoAVLTree(pGameStringTree, (LPARAM)pGameString))
      deleteGameString(pGameString);

   // [SSTYPE_WRECK_SHIP] -- Insert 'ShipWreck' main-type
   pGameString = createGameString(TEXT("SSTYPE_WRECK_SHIP"), 229, identifyGamePageIDFromDataType(DT_CONSTANT), ST_INTERNAL);
   if (!insertObjectIntoAVLTree(pGameStringTree, (LPARAM)pGameString))
      deleteGameString(pGameString);

   // [SSTYPE_WRECK_DOCK] -- Insert 'ShipWreck' main-type
   pGameString = createGameString(TEXT("SSTYPE_WRECK_DOCK"), 230, identifyGamePageIDFromDataType(DT_CONSTANT), ST_INTERNAL);
   if (!insertObjectIntoAVLTree(pGameStringTree, (LPARAM)pGameString))
      deleteGameString(pGameString);

   // [SSTYPE_WRECK_STATION] -- Insert 'ShipWreck' main-type
   pGameString = createGameString(TEXT("SSTYPE_WRECK_FACTORY"), 231, identifyGamePageIDFromDataType(DT_CONSTANT), ST_INTERNAL);
   if (!insertObjectIntoAVLTree(pGameStringTree, (LPARAM)pGameString))
      deleteGameString(pGameString);

   // [TINY WARE] -- Replace '-' TransportClass with 'T' (for 'Tiny')
   if (findObjectInAVLTreeByValue(pGameStringTree, GPI_TRANSPORT_CLASSES, 0, (LPARAM&)pGameString) AND utilCompareString(pGameString->szText, "-"))
      updateGameStringText(pGameString, TEXT("T"));

   // [PIRATE TS] -- Alter ambiguous 'TS' object class to 'Pirate TS'
   if (findGameStringInTreeByID(pGameStringTree, 2035, GPI_OBJECT_CLASSES,  pGameString) AND utilCompareString(pGameString->szText, "TS"))
      updateGameStringText(pGameString, TEXT("Pirate TS"));

   // [PLAYER HQ] -- Alter ambiguous 'Headquarters' object class to 'Player Headquarters'
   if (findGameStringInTreeByID(pGameStringTree, 2045, GPI_OBJECT_CLASSES,  pGameString) AND utilCompareString(pGameString->szText, "Headquarters"))
      updateGameStringText(pGameString, TEXT("Player Headquarters"));

   // [LABEL NUMBER] -- Alter ambiguous 'Label' parameter syntax to 'Label Number'
   if (findGameStringInTreeByID(pGameStringTree, PS_LABEL_NUMBER, identifyGamePageIDFromDataType(DT_SCRIPTDEF),  pGameString))
      updateGameStringText(pGameString, TEXT("Label Number"));

   // [LABEL NAME] -- Alter ambiguous 'Label' parameter syntax to 'Label Name'
   if (findGameStringInTreeByID(pGameStringTree, PS_LABEL_NAME, identifyGamePageIDFromDataType(DT_SCRIPTDEF),  pGameString))
      updateGameStringText(pGameString, TEXT("Label Name"));

   // [INTERRUPTABLE] -- Alter ambigous 'RetVar/If' parameter syntax to '@RetVar/IF'
   if (findGameStringInTreeByID(pGameStringTree, PS_INTERRUPT_RETURN_OBJECT_IF, identifyGamePageIDFromDataType(DT_SCRIPTDEF),  pGameString))
      updateGameStringText(pGameString, TEXT("@RetVar/IF"));

   return TRUE;
}


/// Function name  : isResultSupplementaryLanguageFile
// Description     : Checks whether a FileSearch result should be loaded as a supplementary LanguageFile or not
// 
// CONST FILE_SEARCH*  pFileSearch   : [in] Supplementary language file FileSearch object
// CONST FILE_ITEM*    pSearchResult : [in] Search result to test
// 
// Return Value   : TRUE/FALSE
// 
BOOL  isResultSupplementaryLanguageFile(CONST FILE_SEARCH*  pFileSearch, CONST FILE_ITEM*  pSearchResult)
{
   TCHAR  *szLanguageID;    // Language ID filename identifier
   BOOL    bResult;         // Operation result

   // Prepare
   bResult = FALSE;

   // [CHECK] Ensure result is not a folder
   if (~pSearchResult->iAttributes INCLUDES FIF_FOLDER)
   {
      // Generate the 'L0nn' language identification portion of a language file filename  (Also format as -L0nn.' for extra safety)
      szLanguageID = utilCreateStringf(8, TEXT("-L0%02u."), getAppPreferences()->eGameLanguage);

      // [CHECK] Is this a supplementary file for the current game language?
      if (!utilCompareStringN(pSearchResult->szDisplayName, "0001-", 5) AND utilFindSubStringVariableI(pSearchResult->szDisplayName, szLanguageID))
         /// [SUCCESS] Return TRUE unless there is an equivilent PCK file
         bResult = !isResultDuplicateSupplementaryLanguageFile(pFileSearch, pSearchResult);

      // [SPECIAL CASE] Is this the english version of '0002', which has no language identifier for some reason
      else if (getAppPreferences()->eGameLanguage == GL_ENGLISH AND utilCompareStringN(pSearchResult->szDisplayName, "0002.", 5))
         /// [SUCCESS] Return TRUE unless there is an equivilent PCK file
         bResult = !isResultDuplicateSupplementaryLanguageFile(pFileSearch, pSearchResult);

      // Cleanup
      utilDeleteString(szLanguageID);
   }

   // Return result
   return bResult;
}


/// Function name  : isResultDuplicateSupplementaryLanguageFile
// Description     : Determines whether a supplementary language file search result is just the extracted XML version of an existing PCK file
// 
// CONST FILE_SEARCH*  pFileSearch   : [in] Supplementary language file FileSearch object
// CONST FILE_ITEM*    pSearchResult : [in] Search result to test
// 
// Return Value   : TRUE / FALSE
// 
BOOL  isResultDuplicateSupplementaryLanguageFile(CONST FILE_SEARCH*  pFileSearch, CONST FILE_ITEM*  pSearchResult)
{
   TCHAR      *szPackedEquivilent;     // Full path of .PCK equivilent of the input .XML file
   FILE_ITEM  *pDummy;                 // Dummy search result used for querying the file search tree
   BOOL        bResult;                // Operation result

   /// [CHECK] Is file an XML file?
   if (bResult = utilCompareString(PathFindExtension(pSearchResult->szDisplayName), ".xml"))
   {
      // Generate filepath of .PCK equivilent
      szPackedEquivilent = utilDuplicatePath(pSearchResult->szFullPath);
      PathRenameExtension(szPackedEquivilent, TEXT(".pck"));

      /// [CHECK] Is there an equivilent PCK version which should be loaded instead?
      bResult = findObjectInAVLTreeByValue(pFileSearch->pResultsTree, (LPARAM)szPackedEquivilent, NULL, (LPARAM&)pDummy);

      // Cleanup
      utilDeleteString(szPackedEquivilent);
   }

   // Return result
   return bResult;
}



/// Function name  : loadGameStringTrees
// Description     : Generate the GameString trees from the master langauge file and any supplementary language files
// 
// CONST FILE_SYSTEM*  pFileSystem : [in]  Virtual FileSystem object
// HWND                hParentWnd  : [in]  Parent window for any message dialogs
// OPERATION_PROGRESS* pProgress   : [out] Operation progress
// ERROR_QUEUE*        pErrorQueue : [out] Error message, if any
// 
// Operation Stages  : THREE
// 
// Return Value  : OR_SUCCESS - GameString trees were created succesfully. If any errors were encountered, they were ignored.
//                 OR_FAILURE - GameString trees were NOT created due to a missing/corrupt language file
//                 OR_ABORTED - GameString trees were NOT created because the user aborted after a minor error
//
OPERATION_RESULT  loadGameStringTrees(CONST FILE_SYSTEM*  pFileSystem, HWND  hParentWnd, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue)
{
   OPERATION_RESULT     eResult;             // Operation result
   LANGUAGE_FILE*       pLanguageFile;       // LanguageFile used for processing each language file
   TCHAR*               szLanguageFolder;    // Full path of the language file folder  (the 't' subfolder)
   TCHAR*               szFileSubPath;       // SubPath of the master or supplementary language file being processed
   FILE_SEARCH*         pFileSearch;         // FileSearch for all LanguageFiles
   FILE_ITEM*           pSearchResult;       // LanguageFile FileSearch result
   UINT                 iMergeCount;

   // [TRACKING]
   TRACK_FUNCTION();
   VERBOSE_THREAD_COMMAND();

   __try
   {
      // [INFO/STAGE] "Loading master language file '%s'"
      pushErrorQueue(pErrorQueue, generateDualInformation(HERE(IDS_OUTPUT_LOADING_MASTER_LANGUAGE), szFileSubPath = generateGameLanguageFileSubPath(GFI_MASTER_LANGUAGE, getAppPreferences()->eGameVersion, getAppPreferences()->eGameLanguage)));
      ASSERT(advanceOperationProgressStage(pProgress) == IDS_PROGRESS_LOADING_MASTER_STRINGS);
      utilDeleteString(szFileSubPath);

      // [CHECK] GameString trees don't exist yet
      ASSERT(!getGameData()->pGameStringsByID AND !getGameData()->pGamePagesByID);

      /// Create master LanguageFile
      pLanguageFile = createLanguageFile(LFT_STRINGS, NULL, FALSE);

      /// Translate the master language file
      if (eResult = loadLanguageFile(pFileSystem, pLanguageFile, FALSE, hParentWnd, pProgress, pErrorQueue))
      {
         // [FAILURE] "The master language file '%s' could not be loaded"
         enhanceLastError(pErrorQueue, ERROR_ID(IDS_LANGUAGE_MASTER_FILE_FAILED), szFileSubPath);
         generateOutputTextFromLastError(pErrorQueue);

         // Cleanup temporary LanguageFile
         deleteLanguageFile(pLanguageFile);
      }
      else
      {
         /// [SUCCESS] Create the initial game data from the master language file 
         insertLanguageFileIntoGameData(pLanguageFile, pErrorQueue);

         // Cleanup 
         deleteLanguageFile(pLanguageFile);
      }
   }
   /// [EXCEPTION HANDLER]
   __except (generateQueuedExceptionError(GetExceptionInformation(), pErrorQueue))
   {
      // [FAILURE] "An unidentified and unexpected critical error has occurred while loading the %s file '%s'"
      enhanceLastError(pErrorQueue, ERROR_ID(IDS_EXCEPTION_LOAD_GAME_STRING_TREES), identifyLanguageFile(pLanguageFile), identifyGameFileFilename(pLanguageFile));
      consolePrintError(lastErrorQueue(pErrorQueue));
      eResult = OR_FAILURE;
   }
    
   /// [SUCCESS] Merge the supplementary language files into the game data
   if (eResult == OR_SUCCESS)
   {
      // [VERBOSE]
      VERBOSE_HEADING("Searching for supplementary language files");

      __try
      {
         // Search for supplementary language files
         szLanguageFolder = generateGameDataFilePath(GFI_LANGUAGE_FOLDER, getAppPreferences()->szGameFolder, getAppPreferences()->eGameVersion);
         pFileSearch      = performFileSystemSearch(pFileSystem, szLanguageFolder, FF_LANGUAGE_FILES, AK_PATH, AO_DESCENDING, NULL);

         // [PROGRESS] Define progress limit as the number of supplementary language files
         ASSERT(advanceOperationProgressStage(pProgress) == IDS_PROGRESS_LOADING_SUPPLEMENTARY_STRINGS);
         updateOperationProgressMaximum(pProgress, getFileSearchResultCount(pFileSearch));

         /// Iterate through supplementary language files (unless user aborts)
         while (eResult != OR_ABORTED AND findNextFileSearchResult(pFileSearch, pSearchResult))
         {
            // [CHECK] Skip inappropriate supplemenary language files
            if (!isResultSupplementaryLanguageFile(pFileSearch, pSearchResult))
            {
               // [SKIP] Update current progress
               advanceOperationProgressValue(pProgress);
               continue;
            }

            // Create supplementary language file
            pLanguageFile = createLanguageFile(LFT_STRINGS, pSearchResult->szFullPath, TRUE);

            // Re-Generate sub-path
            szFileSubPath = generateGameLanguageFileSubPath(GFI_LANGUAGE_FOLDER, getAppPreferences()->eGameVersion, getAppPreferences()->eGameLanguage);
            StringCchCat(szFileSubPath, MAX_PATH, pSearchResult->szDisplayName);

            // [INFO] "Loading supplementary language file '%s'"
            pushErrorQueue(pErrorQueue, generateDualInformation(HERE(IDS_OUTPUT_LOADING_SUPPLEMENTARY_LANGUAGE), szFileSubPath));

            // Translate supplementary language file
            switch (eResult = loadLanguageFile(pFileSystem, pLanguageFile, TRUE, hParentWnd, NULL, pErrorQueue))
            {
            /// [SUCCESS] Merge supplementary language file with the existing game data
            case OR_SUCCESS:
               // Merge language file
               iMergeCount = insertLanguageFileIntoGameData(pLanguageFile, pErrorQueue);

               // [WARNING] "The supplementary language file '%s' has overwritten %u strings"
               if (iMergeCount > 0)
                  pushErrorQueue(pErrorQueue, generateDualWarning(HERE(IDS_LANGUAGE_SUPPLEMENTARY_FILE_MERGED), szFileSubPath, iMergeCount));
               break;

            /// [FAILURE] Switch result to SUCCESS since failing to translate a supplementary language file is not a critical failure
            case OR_FAILURE:
               eResult = OR_SUCCESS;
               // [ERROR] "Unable to merge the supplementary language file '%s' with the game data"
               enhanceLastError(pErrorQueue, ERROR_ID(IDS_LANGUAGE_SUPPLEMENTARY_FILE_FAILED), szFileSubPath);
               generateOutputTextFromLastError(pErrorQueue);
               break;
            }

            // [PROGRESS] Update current progress
            advanceOperationProgressValue(pProgress);

            // Cleanup
            utilDeleteString(szFileSubPath);
            deleteLanguageFile(pLanguageFile);
         }  // END: find next supplementary language file

         // Cleanup
         deleteFileSearch(pFileSearch);
         utilDeleteString(szLanguageFolder);
      }
      /// [EXCEPTION HANDLER]
      __except (generateQueuedExceptionError(GetExceptionInformation(), pErrorQueue))
      {
         // [FAILURE] "An unidentified and unexpected critical error has occurred while loading the %s file '%s'"
         enhanceLastError(pErrorQueue, ERROR_ID(IDS_EXCEPTION_LOAD_GAME_STRING_TREES), TEXT("supplementary language"), pSearchResult ? pSearchResult->szDisplayName : TEXT("<Unknown>"));
         consolePrintError(lastErrorQueue(pErrorQueue));
         eResult = OR_FAILURE;
      }
   } 

   /// Examine overall result
   switch (eResult)
   {
   // [SUCCESS] GameString trees loaded successfully
   case OR_SUCCESS:  VERBOSE_HEADING("Loaded %d game strings over %d game pages", getTreeNodeCount(getGameData()->pGameStringsByID), getTreeNodeCount(getGameData()->pGamePagesByID));  break;
   // [ABORTED] User aborted the translation of the master or supplementary file
   case OR_ABORTED:  VERBOSE_HEADING("ABORT: User aborted the loading of language files");           break;
   // [FAILURE] Critical error in the master language file
   case OR_FAILURE:  VERBOSE_HEADING("ERROR: Critical syntax error in the master language file");    break;
   }

   /// [SUCCESS] Generate ScriptObjects
   if (eResult == OR_SUCCESS)
   {
      // [SPECIAL CASE] Fudge and create a few entries for script translation
      insertSpecialCasesIntoGameStringTrees();

      // [PROGRESS] Define progress as number of unprocessed game strings
      ASSERT(advanceOperationProgressStage(pProgress) == IDS_PROGRESS_RESOLVING_SUBSTRINGS);
      updateOperationProgressMaximum(pProgress, 2 * getTreeNodeCount(getGameData()->pGameStringsByID));

      /// Resolve sub-strings
      performGameStringTreeSubstringResolution(pProgress, pErrorQueue);

      /// Generate ScriptObjects
      loadScriptObjectsTrees(pProgress, pErrorQueue);
   }

   // Return result
   END_TRACKING();
   return eResult;
}


/// Function name  : performGameStringTreeSubstringResolution
// Description     : Resolves the substring markers in the game strings tree
// 
// OPERATION_PROGRESS*  pProgress   : [in/out] Progress tracker, with stage and maximum already set
// ERROR_QUEUE*         pErrorQueue : [in/out] ErrorQueue, may already contain errors
// 
VOID   performGameStringTreeSubstringResolution(OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue)
{
   AVL_TREE_OPERATION*   pOperationData;

   __try
   {
      // [INFO] "Resolving language file substrings..."
      pushErrorQueue(pErrorQueue, generateDualInformation(HERE(IDS_OUTPUT_RESOLVING_SUBSTRINGS)));
      VERBOSE_HEADING("Resovling game string tree substrings");

      // Prepare
      pOperationData = createAVLTreeOperationEx(treeprocResolveGameStringSubstrings, ATT_INORDER, pErrorQueue, pProgress);

      /// Resolve all sub-strings in the language file's strings tree
      performOperationOnAVLTree(getGameData()->pGameStringsByID, pOperationData);

      // [CHECK] Were any sub-strings missing?
      if (pOperationData->xOutput)
         // [ERROR] "Unresolvable substrings are present in %u of the loaded language strings"
         pushErrorQueue(pErrorQueue, generateDualError(HERE(IDS_GAME_STRING_SUBSTRING_MISSING_COUNT), pOperationData->xOutput));

      // Cleanup
      pOperationData->pErrorQueue = NULL;
      deleteAVLTreeOperation(pOperationData);
   }
   /// [EXCEPTION HANDLER]
   __except (generateQueuedExceptionError(GetExceptionInformation(), pErrorQueue))
   {
      // [FAILURE] "An unidentified and unexpected critical error has occurred while resolving the language file substrings"
      enhanceLastError(pErrorQueue, ERROR_ID(IDS_EXCEPTION_RESOLVE_SUBSTRINGS));
      consolePrintError(lastErrorQueue(pErrorQueue));
   }
}



/// Function name  : performLanguageFileGamePageMerge
// Description     : Mergers all the GamePages within a supplementary LanguageFile into the game data. Existing pages are overwritten. No warnings are produced.
// 
// LANGUAGE_FILE*  pLanguageFile   : [in] Supplementary language file
// 
VOID  performLanguageFileGamePageMerge(LANGUAGE_FILE*  pLanguageFile)
{
   AVL_TREE_OPERATION  *pMergePageOperation;          // Operation data for merging the game pages
   
   // [CHECK] Ensure this is a supplementary Language file
   ASSERT(!isLanguageFileMaster(pLanguageFile) AND pLanguageFile->eType == LFT_STRINGS);

   // Prepare
   pMergePageOperation = createAVLTreeOperation(treeprocMergeSupplementaryGamePages, ATT_INORDER);
   
   /// Merge strings/pages into the game data
   pMergePageOperation->pOutputTree = getGameData()->pGamePagesByID;
   performOperationOnAVLTree(pLanguageFile->pGamePagesByID, pMergePageOperation);      // Ownership of the GamePages has now passed to the game data tree

   // Ensure LanguageFile does not delete pages
   pLanguageFile->pGamePagesByID->pfnDeleteNode   = NULL;

   // Cleanup
   deleteAVLTreeOperation(pMergePageOperation);
}


/// Function name  : performLanguageFileGameStringMerge
// Description     : Inserts all the GameStrings in a LanguageFile into the GameData, producing a warning for each string overwritten.
// 
// LANGUAGE_FILE*  pLanguageFile : [in]     Supplementary LanguageFile
// ERROR_QUEUE*    pErrorQueue   : [in/out] ErrorQueue
// 
// Return Value : Number of game data strings overwritten
//
UINT  performLanguageFileGameStringMerge(LANGUAGE_FILE*  pLanguageFile, ERROR_QUEUE*  pErrorQueue)
{
   AVL_TREE_OPERATION*  pMergeStringOperation;          // Operation data for merging the game pages
   UINT                 iStringsOverwritten;          // Number of strings overwritten
   
   // [CHECK] Ensure this is a supplementary Language file
   ASSERT(!isLanguageFileMaster(pLanguageFile) AND pLanguageFile->eType == LFT_STRINGS);

   // Prepare
   pMergeStringOperation = createAVLTreeOperationEx(treeprocMergeSupplementaryGameStrings, ATT_INORDER, pErrorQueue, NULL);
   
   // Pass filename, official flag and 'Report Merges' flags
   pMergeStringOperation->xFirstInput  = (LPARAM)PathFindFileName(pLanguageFile->szFullPath);
   pMergeStringOperation->xSecondInput = isLanguageFileOfficial(pLanguageFile);
   pMergeStringOperation->xThirdInput  = getAppPreferences()->bReportGameStringOverwrites;
   
   /// Merge strings into the game data
   pMergeStringOperation->pOutputTree = getGameData()->pGameStringsByID;
   performOperationOnAVLTree(pLanguageFile->pGameStringsByID, pMergeStringOperation);  // Ownership of the GameString has now passed to the game data tree

   // Set result
   iStringsOverwritten = pMergeStringOperation->xOutput;

   // Ensure LanguageFile does not delete pages
   pLanguageFile->pGameStringsByID->pfnDeleteNode = NULL;
   
   // Sever ErrorQueue and Cleanup
   pMergeStringOperation->pErrorQueue = NULL;
   deleteAVLTreeOperation(pMergeStringOperation);

   // Return strings overwritten
   return iStringsOverwritten;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        TREE OPERATIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : treeprocResolveGameStringSubstrings
// Description     : Resolve lookup sub-strings in the specified GameString. Copy mission sub-strings verbatim,
//                    and delete comment sub-strings entirely.
// 
// BINARY_TREE_NODE*            pNode           : [in] Node containing a GameString
// BINARY_TREE_OPERATION_DATA*  pOperationData  : [in] Not used
// 
VOID   treeprocResolveGameStringSubstrings(AVL_TREE_NODE*  pNode, AVL_TREE_OPERATION*  pOperationData)
{
   GAME_STRING*  pGameString;    // Convenience pointer
   TCHAR*        szOutput;       // Output string containing resolved sub-strings
   LIST*         pResolutionList;

   // Prepare
   pGameString = extractPointerFromTreeNode(pNode, GAME_STRING);

   // [CHECK] Abort if string contains no special characters
   if (findNextNonEscapedCharacters(pGameString->szText, TEXT("({")) == NULL)
      return;

   // Prepare
   szOutput = utilCreateEmptyString(MAX_STRING);
   pResolutionList = createStack(NULL);

   /// Attempt to resolve sub-strings. Increment running total of failures.
   pOperationData->xOutput += performGameStringResolution(pGameString, szOutput, MAX_STRING, pResolutionList, pOperationData->pErrorQueue);

   // Replace GameString text
   pGameString->szText = utilReplaceString(pGameString->szText, szOutput);
   pGameString->iCount = lstrlen(pGameString->szText);

   // [RESOLVED] Mark as resolved
   pGameString->bResolved = TRUE;

   // Cleanup
   deleteStack(pResolutionList);
   utilDeleteString(szOutput);
}


/// Function name  : treeprocMergeSupplementaryGamePages
// Description     : Copies the GamePage into the game data. No errors are generated.
///                    ->  Conflicting GamePages are overwritten
// 
// AVL_TREE_NODE*       pCurrentNode   : [in] Node containing a game page to merge
// AVL_TREE_OPERATION*  pOperationData : [in] Operation data
///                                             -> pOutputTree : [in/out] Game data GamePages tree
// 
VOID  treeprocMergeSupplementaryGamePages(AVL_TREE_NODE*  pCurrentNode, AVL_TREE_OPERATION*  pOperationData)
{
   GAME_PAGE    *pGamePage;    // Convenience pointer

   // Prepare
   pGamePage = extractPointerFromTreeNode(pCurrentNode, GAME_PAGE);

   /// Attempt to insert GamePage into game data
   if (!insertObjectIntoAVLTree(pOperationData->pOutputTree, (LPARAM)pGamePage))
   {
      // [CONFLICT] Destroy the conflicting GamePage and insert replacement
      destroyObjectInAVLTreeByValue(pOperationData->pOutputTree, pGamePage->iPageID, NULL);
      insertObjectIntoAVLTree(pOperationData->pOutputTree, (LPARAM)pGamePage);
   }
}


/// Function name  : treeprocMergeSupplementaryGameStrings
// Description     : Inserts each GameString into the game data
///                    -> Conflicting GameStrings are overwritten
///                    -> A warning is produced each time a conflict is overwritten
// 
// AVL_TREE_NODE*       pCurrentNode   : [in] Node containing the string to merge
// AVL_TREE_OPERATION*  pOperationData : [in] Operation data
///                                             -> pOutputTree : AVL_TREE*    : [in/out] Game data GameStrings tree
///                                             -> pErrorQueue : AVL_TREE*    : [in/out] Output error queue for conflict warnings
///                                             -> xFirstInput : CONST TCHAR* : [in]     Filename of the source language file [For conflict warnings]
///                                             -> xFirstInput : BOOL         : [in]     Whether this is an official Egosoft supplementary file [Suppresses warnings]
// 
VOID  treeprocMergeSupplementaryGameStrings(AVL_TREE_NODE*  pCurrentNode, AVL_TREE_OPERATION*  pOperationData)
{
   GAME_STRING    *pGameString,              // Convenience pointer
                  *pConflictString;
   CONST TCHAR    *szLanguageFileName;       // Name of the LanguageFile containing the strings
   TCHAR          *szPreviewText[2];         // Truncated version of the game string text, used for warning reporting
   BOOL            bEgosoftFile,             // Whether this is an official supplementary language file
                   bReportWarnings;

   // Prepare
   pGameString        = extractPointerFromTreeNode(pCurrentNode, GAME_STRING);
   szLanguageFileName = (CONST TCHAR*)pOperationData->xFirstInput;
   bEgosoftFile       = (BOOL)pOperationData->xSecondInput;
   bReportWarnings    = (BOOL)pOperationData->xThirdInput; 

   // [MOD STRING] Add flag to the GameString to indicate it came from a mod
   if (!bEgosoftFile)
      pGameString->bModContent = TRUE;

   /// Attempt to insert string into game data
   if (!insertObjectIntoAVLTree(pOperationData->pOutputTree, (LPARAM)pGameString))
   {
      /// [CONFLICT] Lookup and display conflicting GameString and insert replacement
      findObjectInAVLTreeByValue(pOperationData->pOutputTree, pGameString->iPageID, pGameString->iID, (LPARAM&)pConflictString);

      /// [COMMAND OVERRIDE] Display name of command overridden
      if (pGameString->iPageID == GPI_OBJECT_COMMANDS)
         // [WARNING] "The command %s has overridden the command %s"
         pushErrorQueue(pOperationData->pErrorQueue, generateDualWarning(HERE(IDS_LANGUAGE_SUPPLEMENTARY_COMMAND_MERGED), pGameString->szText, pConflictString->szText));
      
      // [CHECK] Suppress warnings for official supplementary language files
      else if (!bEgosoftFile AND bReportWarnings)
      {
         // [UNOFFICIAL] Extract a portion of the GameString text and convert to ST_INTERNAL
         szPreviewText[0] = generateInternalGameStringPreview(pGameString, 96);
         szPreviewText[1] = generateInternalGameStringPreview(pConflictString, 96);
         
         /// [WARNING] "The string %u:'%s' found in '%s' has overwritten game data '%s' (page %u)"
         pushErrorQueue(pOperationData->pErrorQueue, generateDualWarning(HERE(IDS_LANGUAGE_SUPPLEMENTARY_STRING_MERGED), pGameString->iID, szPreviewText[0], szLanguageFileName, szPreviewText[1], pGameString->iPageID));

         // Cleanup
         utilDeleteStrings(szPreviewText[0], szPreviewText[1]);
      }
      
      // Increment overwritten string count
      pOperationData->xOutput++;

      /// [OVER-WRITE] Destroy the conflicting GameString and insert replacement
      destroyObjectInAVLTreeByValue(pOperationData->pOutputTree, pConflictString->iPageID, pConflictString->iID);
      insertObjectIntoAVLTree(pOperationData->pOutputTree, (LPARAM)pGameString);
   }
}

