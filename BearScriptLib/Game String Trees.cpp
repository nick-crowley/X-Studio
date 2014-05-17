//
// Game String Trees.cpp : Operations upon trees of GameStrings
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        DECLARATIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// Functions
VOID       insertLanguageFileIntoGameStringTrees(LANGUAGE_FILE*  pLanguageFile, ERROR_QUEUE*  pErrorQueue);
VOID       insertLanguagePagesIntoGameStringTrees(LANGUAGE_FILE*  pLanguageFile);
UINT       insertLanguageStringsIntoGameStringTrees(LANGUAGE_FILE*  pLanguageFile, ERROR_QUEUE*  pErrorQueue);
BOOL       insertSpecialCasesIntoGameStringTrees();
BOOL       isSearchResultDuplicate(CONST FILE_SEARCH*  pFileSearch, CONST FILE_ITEM*  pSearchResult);
BOOL       isSearchResultSupplementary(CONST FILE_SEARCH*  pFileSearch, CONST FILE_ITEM*  pSearchResult);
BOOL       resolveGameStringTreeSubstrings(OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pOutputQueue);

// Tree operations
VOID       treeprocMergeGamePages(AVL_TREE_NODE*  pCurrentNode, AVL_TREE_OPERATION*  pOperationData);
VOID       treeprocMergeGameStrings(AVL_TREE_NODE*  pCurrentNode, AVL_TREE_OPERATION*  pOperationData);

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
   return createAVLTree(extractGamePageNode, deleteGamePageNode, createAVLTreeSortKey(AK_PAGE_ID, AO_ASCENDING), NULL, NULL);
}


/// Function name  : createGameStringTreeByPageID
// Description     : Creates an GameStrings tree sorted by PageID -> StringID. Used in LanguageFiles
// 
// Return Value   : New AVLTree, you are responsible for destroying it
//  
AVL_TREE*  createGameStringTreeByPageID()
{
   return createAVLTree(extractGameStringNode, deleteGameStringNode, createAVLTreeSortKey(AK_PAGE_ID, AO_ASCENDING), createAVLTreeSortKey(AK_ID, AO_ASCENDING), NULL);
}


/// Function name  : createGameStringTreeByPageID
// Description     : Creates an GameStrings tree sorted by GameVersion -> PageID -> StringID. Used in DescriptionFiles
// 
// Return Value   : New AVLTree, you are responsible for destroying it
// 
AVL_TREE*  createGameStringTreeByVersion()
{
   return createAVLTree(extractGameStringNode, deleteGameStringNode, createAVLTreeSortKey(AK_VERSION, AO_ASCENDING), createAVLTreeSortKey(AK_PAGE_ID, AO_ASCENDING), createAVLTreeSortKey(AK_ID, AO_ASCENDING));
}


/// Function name  : deleteGamePageNode
// Description     : Deletes a GamePage within an AVLTree Node
// 
// LPARAM  pData : [in] Reference to a GamePage pointer
// 
VOID   deleteGamePageNode(LPARAM  pData)
{
   // Delete node contents
   deleteGamePage((GAME_PAGE*&)pData);
}


/// Function name  : deleteGameStringNode
// Description     : Deletes a GameString within an AVLTree Node
// 
// LPARAM  pData : [in] Reference to a GameString pointer
// 
VOID   deleteGameStringNode(LPARAM  pData)
{
   // Delete node contents
   deleteGameString((GAME_STRING*&)pData);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                               HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : extractGameStringNode
// Description     : Extract the desired property from a given TreeNode containing a GameString
// 
// LPARAM                   pNode      : [in] GAME_STRING* : Node data containing a GameString
// CONST AVL_TREE_SORT_KEY  eProperty  : [in] Property to retrieve
// 
// Return Value   : Value of the specified property or NULL if the property is unsupported
// 
BearScriptAPI
LPARAM  extractGameStringNode(LPARAM  pNode, CONST AVL_TREE_SORT_KEY  eProperty)
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


/// Function name  : extractGamePageNode
// Description     : Extract the desired property from a given TreeNode containing a GamePage
// 
// LPARAM                   pNode      : [in] GAME_PAGE* : Node data containing a GamePage
// CONST AVL_TREE_SORT_KEY  eProperty  : [in] Property to retrieve
// 
// Return Value   : Value of the specified property or NULL if the property is unsupported
// 
BearScriptAPI
LPARAM  extractGamePageNode(LPARAM  pNode, CONST AVL_TREE_SORT_KEY  eProperty)
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



/// Function name  : findGameStringByID
// Description     : Find a game string from the game data strings tree by it's string and page IDs
//
// CONST UINT     iStringID : [in]  ID of the string to search for
// CONST UINT     iPageID   : [in]  ID of the page containing the string to search for
// GAME_STRING*  &pOutput   : [out] Resultant game string object, or NULL If not found
// 
// Return type : TRUE if found, FALSE If not
//
BearScriptAPI
BOOL    findGameStringByID(CONST UINT  iStringID, CONST UINT  iPageID, GAME_STRING*  &pOutput)
{
   // Query game strings tree
   return findObjectInAVLTreeByValue(getGameData()->pGameStringsByID, iPageID, iStringID, (LPARAM&)pOutput);
}


/// Function name  :  findGameStringInTreeByID
// Description     : Search for a GameString in a specified binary tree
// 
// CONST AVL_TREE*  pTree     : [in] Binary tree to search
// CONST UINT       iStringID : [in] ID of the string to search for
// CONST UINT       iPageID   : [in] ID of the page containing the string
// GAME_STRING*    &pOutput   : [out] Resultant GameString, if found
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BearScriptAPI
BOOL   findGameStringInTreeByID(CONST AVL_TREE*  pTree, CONST UINT  iStringID, CONST UINT  iPageID, GAME_STRING*  &pOutput)
{
   // Query tree
   return findObjectInAVLTreeByValue(pTree, iPageID, iStringID, (LPARAM&)pOutput);
}


/// Function name  : findGamePageInTreeByID
// Description     : Find a GamePage in a specified tree by it's ID
// 
// CONST BINARY_TREE*  pTree    : [in]  The GamePage tree to search
// CONST UINT          iPageID  : [in]  The ID of the Page to search for
// GAME_PAGE*         &pOutput  : [out] Resultant GamePage, if found
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BearScriptAPI
BOOL   findGamePageInTreeByID(CONST AVL_TREE*  pTree, CONST UINT  iPageID, GAME_PAGE*  &pOutput)
{
   // [CHECK] Ensure tree contains game pages
   //ASSERT(pTree->eType == AT_GAME_PAGE);

   // Query tree
   return findObjectInAVLTreeByValue(pTree, iPageID, NULL, (LPARAM&)pOutput);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                              FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : insertLanguageFileIntoGameStringTrees
// Description     : Inserts a processed master/supplementary language file into the game string trees
// 
// LANGUAGE_FILE*       pLanguageFile : [in/out] Language file containing the GameString and GamePages to insert
///                                                Master:        Trees are severed and used as the basis of the game data.  (GameData trees must be NULL)
///                                                Supplementary: Contents of the trees are added to the existing game data. (GameData trees must exist)
// ERROR_QUEUE*         pErrorQueue   : [in/out] Contains Warning messages generated from overwriting existing game strings
// 
VOID  insertLanguageFileIntoGameStringTrees(LANGUAGE_FILE*  pLanguageFile, ERROR_QUEUE*  pErrorQueue)
{
   __try
   {
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
      }
      /// [SUPPLEMENTARY] Merge trees into game data
      else
      {
         // [CHECK] Ensure GameData exists
         ASSERT(getGameData()->pGameStringsByID AND getGameData()->pGamePagesByID);

         // Merge pages into the game data
         insertLanguagePagesIntoGameStringTrees(pLanguageFile);

         // Merge strings into the game data
         insertLanguageStringsIntoGameStringTrees(pLanguageFile, pErrorQueue);
      }
   }
   PUSH_CATCH(pErrorQueue)
   {
      EXCEPTION1("Unable to merge supplementary language file '%s' into the game data", PathFindFileName(pLanguageFile->szFullPath));
   }
}



/// Function name  : insertLanguagePagesIntoGameStringTrees
// Description     : Mergers all the GamePages within a supplementary LanguageFile into the game data. Existing pages are overwritten. No warnings are produced.
// 
// LANGUAGE_FILE*  pLanguageFile   : [in] Supplementary language file
// 
VOID  insertLanguagePagesIntoGameStringTrees(LANGUAGE_FILE*  pLanguageFile)
{
   AVL_TREE_OPERATION  *pMergePageOperation;          // Operation data for merging the game pages
   
   // [CHECK] Ensure this is a supplementary Language file
   ASSERT(!isLanguageFileMaster(pLanguageFile) AND pLanguageFile->eType == LFT_STRINGS);

   // Prepare
   pMergePageOperation = createAVLTreeOperation(treeprocMergeGamePages, ATT_INORDER);
   
   /// Merge strings/pages into the game data
   pMergePageOperation->pOutputTree = getGameData()->pGamePagesByID;
   performOperationOnAVLTree(pLanguageFile->pGamePagesByID, pMergePageOperation);      // Ownership of the GamePages has now passed to the game data tree

   // Ensure LanguageFile does not delete pages
   pLanguageFile->pGamePagesByID->pfnDeleteNode   = NULL;

   // Cleanup
   deleteAVLTreeOperation(pMergePageOperation);
}


/// Function name  : insertLanguageStringsIntoGameStringTrees
// Description     : Inserts all the GameStrings in a LanguageFile into the GameData, producing a warning for each string overwritten.
// 
// LANGUAGE_FILE*  pLanguageFile : [in]     Supplementary LanguageFile
// ERROR_QUEUE*    pErrorQueue   : [in/out] ErrorQueue
// 
// Return Value : Number of game data strings overwritten
//
UINT  insertLanguageStringsIntoGameStringTrees(LANGUAGE_FILE*  pLanguageFile, ERROR_QUEUE*  pErrorQueue)
{
   AVL_TREE_OPERATION*  pOperation;      // Operation data for merging the game pages
   ERROR_QUEUE*         pMergeQueue;     // Contains only errors re: overwritten strings
   UINT                 iMergeCount;     // Number of strings overwritten
   
   // [CHECK] Ensure this is a supplementary Language file
   ASSERT(!isLanguageFileMaster(pLanguageFile) AND pLanguageFile->eType == LFT_STRINGS);

   // Prepare
   pOperation = createAVLTreeOperationEx(treeprocMergeGameStrings, ATT_INORDER, pErrorQueue, NULL);
   pMergeQueue = createErrorQueue();
   
   // Pass filename + official flag + MergeQueue
   pOperation->xFirstInput  = (LPARAM)PathFindFileName(pLanguageFile->szFullPath);
   pOperation->xSecondInput = isLanguageFileOfficial(pLanguageFile);
   pOperation->xThirdInput  = (LPARAM)pMergeQueue;
   pOperation->pOutputTree  = getGameData()->pGameStringsByID;

   /// Merge strings into the game data + Take Ownership
   performOperationOnAVLTree(pLanguageFile->pGameStringsByID, pOperation);  // Ownership of the GameString has now passed to the game data tree
   pLanguageFile->pGameStringsByID->pfnDeleteNode = NULL;

   // [CHECK] Any strings overwritten?
   if (iMergeCount = pOperation->xOutput)
      // [WARNING] "The supplementary language file '%s' has overwritten %u strings"
      generateAttachmentError(pErrorQueue, pMergeQueue, generateDualWarning(HERE(IDS_LANGUAGE_SUPPLEMENTARY_FILE_MERGED), PathFindFileName(pLanguageFile->szFullPath), iMergeCount));
   
   // Cleanup + return MergeCount
   pOperation->pErrorQueue = NULL;
   deleteErrorQueue(pMergeQueue);
   deleteAVLTreeOperation(pOperation);
   return iMergeCount;
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
   if (findGameStringInTreeByID(pGameStringTree, 2035, GPI_OBJECT_CLASSES,  pGameString) AND utilCompareStringN(pGameString->szText, "TS", 2))
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


/// Function name  : isSearchResultSupplementary
// Description     : Checks whether a FileSearch result should be loaded as a supplementary LanguageFile or not
// 
// CONST FILE_SEARCH*  pFileSearch   : [in] Supplementary language file FileSearch object
// CONST FILE_ITEM*    pSearchResult : [in] Search result to test
// 
// Return Value   : TRUE/FALSE
// 
BOOL  isSearchResultSupplementary(CONST FILE_SEARCH*  pFileSearch, CONST FILE_ITEM*  pSearchResult)
{
   TCHAR  *szLanguageID;    // Language ID filename identifier
   BOOL    bResult;         // Operation result

   // Prepare
   bResult = FALSE;

   // [CHECK] Ensure result is not a folder
   if (~pSearchResult->iAttributes INCLUDES FIF_FOLDER)
   {
      if (getAppPreferences()->eGameVersion >= GV_TERRAN_CONFLICT)
      {
         // [X3TC/X3AP] Generate the 'L0nn' language file identifier (format as '-L0nn.')
         szLanguageID = utilCreateStringf(8, TEXT("-L0%02u."), getAppPreferences()->eGameLanguage);

         // [CHECK] Exclude master files + search for language ID substring
         if (!utilCompareStringN(pSearchResult->szDisplayName, "0001-", 5) AND utilFindSubStringVariableI(pSearchResult->szDisplayName, szLanguageID))
            bResult = TRUE;

         // [SPECIAL CASE] Is this the english version of '0002', which has no language identifier for some reason
         else if (getAppPreferences()->eGameLanguage == GL_ENGLISH AND utilCompareStringN(pSearchResult->szDisplayName, "0002.", 5))
            bResult = TRUE;
      }
      else
      {
         // [X3R] Generate the 'nn' language id
         szLanguageID = utilCreateStringf(8, TEXT("%02u"), getAppPreferences()->eGameLanguage);

         // [CHECK] Exclude master files + search for language ID substring
         if (!utilFindSubStringVariableI(pSearchResult->szDisplayName, TEXT("0001.")) AND utilCompareStringVariablesN(pSearchResult->szDisplayName, szLanguageID, 2))
            bResult = TRUE;
      }

      /// [SUCCESS] Return TRUE unless there is an equivilent PCK file
      if (bResult)
         bResult = !isSearchResultDuplicate(pFileSearch, pSearchResult);

      // Cleanup
      utilDeleteString(szLanguageID);
   }

   // Return result
   return bResult;
}


/// Function name  : isSearchResultDuplicate
// Description     : Determines whether a supplementary language file search result is just the extracted XML version of an existing PCK file
// 
// CONST FILE_SEARCH*  pFileSearch   : [in] Supplementary language file FileSearch object
// CONST FILE_ITEM*    pSearchResult : [in] Search result to test
// 
// Return Value   : TRUE / FALSE
// 
BOOL  isSearchResultDuplicate(CONST FILE_SEARCH*  pFileSearch, CONST FILE_ITEM*  pSearchResult)
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



/// Function name  : loadMasterLanguageFile
// Description     : Generate the GameString trees from the master language file 
// 
// CONST FILE_SYSTEM*  pFileSystem : [in]  Virtual FileSystem object
// HWND                hParentWnd  : [in]  Ignored
// OPERATION_PROGRESS* pProgress   : [in/out] Operation progress
// ERROR_QUEUE*        pErrorQueue : [in/out] Error message, if any
// 
// Operation Stages  : THREE
// 
// Return Value  : OR_SUCCESS - GameString trees were created succesfully. If any errors were encountered, they were ignored.
//                 OR_FAILURE - GameString trees may have been created
//
OPERATION_RESULT  loadMasterLanguageFile(CONST FILE_SYSTEM*  pFileSystem, HWND  hParentWnd, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue)
{
   OPERATION_RESULT     eResult;                // Operation result
   LANGUAGE_FILE*       pLanguageFile;          // LanguageFile used for processing each language file
   TCHAR*               szFileSubPath = NULL;   // SubPath of the master or supplementary language file being processed
   
   __try
   {
      CONSOLE_STAGE();
      
      // [INFO/STAGE] "Loading master language file '%s'"
      szFileSubPath = generateGameLanguageFileSubPath(GFI_MASTER_LANGUAGE, getAppPreferences()->eGameVersion, getAppPreferences()->eGameLanguage);
      pushErrorQueue(pErrorQueue, generateDualInformation(HERE(IDS_OUTPUT_LOADING_MASTER_LANGUAGE), szFileSubPath));
      ASSERT(advanceOperationProgressStage(pProgress) == IDS_PROGRESS_LOADING_MASTER_STRINGS);

      // [CHECK] GameString trees don't exist yet
      ASSERT(!getGameData()->pGameStringsByID AND !getGameData()->pGamePagesByID);

      /// Create master LanguageFile
      pLanguageFile = createLanguageFile(LFT_STRINGS, NULL, FALSE);

      /// Translate the master language file
      if ((eResult = loadLanguageFile(pFileSystem, pLanguageFile, FALSE, hParentWnd, pProgress, pErrorQueue)) == OR_SUCCESS)
         /// [SUCCESS] Create the initial game data from the master language file 
         insertLanguageFileIntoGameStringTrees(pLanguageFile, pErrorQueue);
      else
      {
         // [FAILURE] "The master language file '%s' could not be loaded"
         enhanceLastError(pErrorQueue, ERROR_ID(IDS_LANGUAGE_MASTER_FILE_FAILED), szFileSubPath);
         generateOutputTextFromLastError(pErrorQueue);
      }
      
      // Cleanup
      deleteLanguageFile(pLanguageFile);
      utilDeleteString(szFileSubPath);
      return eResult;
   }
   /// [EXCEPTION HANDLER]
   PUSH_CATCH(pErrorQueue)
   {
      EXCEPTION1("Unable to load the master language file '%s'", szFileSubPath);
      return OR_FAILURE;
   }
}


/// Function name  : loadSupplementaryLanguageFiles
// Description     : Generate the GameString trees from the master langauge file and any supplementary language files
// 
// CONST FILE_SYSTEM*  pFileSystem : [in]  Virtual FileSystem object
// HWND                hParentWnd  : [in]  Ignored
// OPERATION_PROGRESS* pProgress   : [in/out] Operation progress
// ERROR_QUEUE*        pErrorQueue : [in/out] Error message, if any
// 
// Operation Stages  : THREE
// 
// Return Value  : OR_SUCCESS - All supplementary language files loaded successfully
//                 OR_FAILURE - Some supplementary language files failed to load
//
OPERATION_RESULT  loadSupplementaryLanguageFiles(CONST FILE_SYSTEM*  pFileSystem, HWND  hParentWnd, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue)
{
   OPERATION_RESULT     eResult = OR_SUCCESS;// Operation result
   LANGUAGE_FILE*       pLanguageFile;       // LanguageFile used for processing each language file
   TCHAR*               szLanguageFolder;    // Full path of the language file folder  (the 't' subfolder)
   TCHAR*               szFileSubPath = 0;   // SubPath of the master or supplementary language file being processed
   FILE_SEARCH*         pFileSearch;         // FileSearch for all LanguageFiles
   FILE_ITEM*           pSearchResult;       // LanguageFile FileSearch result
   //UINT                 iMergeCount;

   __try
   {
      /// Merge the supplementary language files into the game data
      CONSOLE("Searching for supplementary language files");

      // Search for supplementary language files
      szLanguageFolder = generateGameDataFilePath(GFI_LANGUAGE_FOLDER, getAppPreferences()->szGameFolder, getAppPreferences()->eGameVersion);
      pFileSearch      = performFileSystemSearch(pFileSystem, szLanguageFolder, FF_LANGUAGE_FILES, AK_PATH, AO_DESCENDING, NULL);

      // [PROGRESS] Define progress limit as the number of supplementary language files
      ASSERT(advanceOperationProgressStage(pProgress) == IDS_PROGRESS_LOADING_SUPPLEMENTARY_STRINGS);
      updateOperationProgressMaximum(pProgress, getFileSearchResultCount(pFileSearch));

      /// Iterate through supplementary language files (unless user aborts)
      while (eResult == OR_SUCCESS AND findNextFileSearchResult(pFileSearch, pSearchResult))
      {
         // [CHECK] Skip inappropriate supplemenary language files
         if (!isSearchResultSupplementary(pFileSearch, pSearchResult))
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
            insertLanguageFileIntoGameStringTrees(pLanguageFile, pErrorQueue);
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
      return eResult;
   }
   PUSH_CATCH(pErrorQueue)
   {
      EXCEPTION1("Unable to load supplementary language file '%s'", pSearchResult ? pSearchResult->szDisplayName : NULL);
      return OR_FAILURE;
   }
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
   OPERATION_RESULT  eResult;    // Operation result

   __try
   {
      /// Load master language file
      if ((eResult = loadMasterLanguageFile(pFileSystem, hParentWnd, pProgress, pErrorQueue)) != OR_SUCCESS)
         return eResult;

      /// Load supplementary language files
      if ((eResult = loadSupplementaryLanguageFiles(pFileSystem, hParentWnd, pProgress, pErrorQueue)) != OR_SUCCESS)
         return eResult;

      CONSOLE("Inserting special cases...");

      // [SPECIAL CASE] Fudge and create a few entries for script translation
      insertSpecialCasesIntoGameStringTrees();

      // [PROGRESS] Define progress as number of unprocessed game strings
      ASSERT(advanceOperationProgressStage(pProgress) == IDS_PROGRESS_RESOLVING_SUBSTRINGS);
      updateOperationProgressMaximum(pProgress, 2 * getTreeNodeCount(getGameData()->pGameStringsByID));

      CONSOLE("Resolving sub-strings...");

      /// Resolve sub-strings
      if (!resolveGameStringTreeSubstrings(pProgress, pErrorQueue))
         return OR_FAILURE;

      // [SUCCESS]
      CONSOLE_HEADING("Loaded %d game strings over %d game pages", getTreeNodeCount(getGameData()->pGameStringsByID), getTreeNodeCount(getGameData()->pGamePagesByID));

      /// Generate ScriptObjects
      if (!loadScriptObjectsTrees(pProgress, pErrorQueue))
         return OR_FAILURE;
      
      // Return result
      return eResult;
   }
   PUSH_CATCH(pErrorQueue)
   {
      EXCEPTION("Unable to resolve substrings / generate script objects");
      return OR_FAILURE;
   }
}


/// Function name  : resolveGameStringTreeSubstrings
// Description     : Resolves the substring markers in the game strings tree
// 
// OPERATION_PROGRESS*  pProgress   : [in/out] Progress tracker, with stage and maximum already set
// ERROR_QUEUE*         pOutputQueue : [in/out] ErrorQueue, may already contain errors
// 
BOOL   resolveGameStringTreeSubstrings(OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pOutputQueue)
{
   AVL_TREE_OPERATION*   pOperationData;
   ERROR_QUEUE          *pErrorQueue,
                        *pAttachmentQueue;
   ERROR_STACK*          pError;
   UINT                  iProblemCount;

   __try
   {
      // [INFO] "Resolving language file substrings..."
      pushErrorQueue(pOutputQueue, generateDualInformation(HERE(IDS_OUTPUT_RESOLVING_SUBSTRINGS)));

      // Prepare
      pAttachmentQueue = createErrorQueue();
      pOperationData   = createAVLTreeOperationEx(treeprocResolveGameStringSubstrings, ATT_INORDER, pErrorQueue = createErrorQueue(), pProgress);
      
      /// Resolve all sub-strings in the language file's strings tree
      performOperationOnAVLTree(getGameData()->pGameStringsByID, pOperationData);

      // Transfer Errors to OutputQueue + Warnings to Attachment Queue
      while (pError = popErrorQueue(pErrorQueue))
         pushErrorQueue(pError->eType == ET_WARNING ? pAttachmentQueue : pOutputQueue, pError);
      
      // [CHECK] Were any sub-strings missing?
      if (iProblemCount = pOperationData->xOutput)
         // [WARNING] "Unresolvable substrings are present in %u of the loaded language strings"
         generateAttachmentError(pOutputQueue, pAttachmentQueue, generateDualWarning(HERE(IDS_GAME_STRING_SUBSTRING_MISSING_COUNT), iProblemCount));
         
      // Cleanup
      deleteErrorQueue(pAttachmentQueue);
      deleteAVLTreeOperation(pOperationData);
      return TRUE;
   }
   /// [EXCEPTION HANDLER]
   PUSH_CATCH(pOutputQueue)
   {
      EXCEPTION("Unable to resolve Gamestring tree substrings");
      return FALSE;
   }
}


/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        TREE OPERATIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : treeprocMergeGamePages
// Description     : Copies the GamePage into the game data. No errors are generated.
///                    ->  Conflicting GamePages are overwritten
// 
// AVL_TREE_NODE*       pCurrentNode   : [in] Node containing a game page to merge
// AVL_TREE_OPERATION*  pOperationData : [in] Operation data
///                                             -> pOutputTree : [in/out] Game data GamePages tree
// 
VOID  treeprocMergeGamePages(AVL_TREE_NODE*  pCurrentNode, AVL_TREE_OPERATION*  pOperationData)
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


/// Function name  : treeprocMergeGameStrings
// Description     : Inserts each GameString into the game data
///                    -> Conflicting GameStrings are overwritten
///                    -> A warning is produced each time a conflict is overwritten
// 
// AVL_TREE_NODE*       pCurrentNode   : [in] Node containing the string to merge
// AVL_TREE_OPERATION*  pOperationData : [in] Operation data
///                                             -> xFirstInput  : CONST TCHAR* : [in]     LanguageFile Filename [For conflict warnings]
///                                             -> xSecondInput : BOOL         : [in]     Official LanguageFile flag [Suppresses warnings]
///                                             -> xThirdInput  : ERROR_QUEUE* : [in]     ErrorQueue purely for warnings of string overwrites
///                                             -> pOutputTree  : AVL_TREE*    : [in/out] Game data GameStrings tree
///                                             -> pErrorQueue  : AVL_TREE*    : [in/out] Output error queue for conflict warnings
// 
VOID  treeprocMergeGameStrings(AVL_TREE_NODE*  pCurrentNode, AVL_TREE_OPERATION*  pOperationData)
{
   GAME_STRING    *pGameString,              // Convenience pointer
                  *pConflictString;
   CONST TCHAR    *szLanguageFileName;       // Name of the LanguageFile containing the strings
   ERROR_QUEUE    *pMergeQueue;              // Specialist Queue for 'merge' warnings
   TCHAR          *szPreviewText[2];         // Truncated version of the game string text, used for warning reporting
   BOOL            bEgosoftFile;             // Whether this is an official supplementary language file

   // Prepare
   pGameString        = extractPointerFromTreeNode(pCurrentNode, GAME_STRING);
   szLanguageFileName = (CONST TCHAR*)pOperationData->xFirstInput;
   bEgosoftFile       = (BOOL)pOperationData->xSecondInput;
   pMergeQueue        = (ERROR_QUEUE*)pOperationData->xThirdInput; 

   // [MOD STRING] Add flag to the GameString to indicate it came from a mod
   if (!bEgosoftFile)
      pGameString->bModContent = TRUE;

   /// Attempt to insert string into game data
   if (!insertObjectIntoAVLTree(pOperationData->pOutputTree, (LPARAM)pGameString))
   {
      /// [CONFLICT] Lookup and display conflicting GameString and insert replacement
      findGameStringInTreeByID(pOperationData->pOutputTree, pGameString->iID, pGameString->iPageID, pConflictString);

      // [CHECK] Ensure text is different
      if (StrCmp(pGameString->szText, pConflictString->szText) != 0)
      {
         /// [COMMAND OVERRIDE] Display name of command overridden
         if (pGameString->iPageID == GPI_OBJECT_COMMANDS)
            // [WARNING] "The command %s has overridden the command %s"
            pushErrorQueue(pOperationData->pErrorQueue, generateDualWarning(HERE(IDS_LANGUAGE_SUPPLEMENTARY_COMMAND_MERGED), pGameString->szText, pConflictString->szText));
         
         // [CHECK] Suppress warnings for official supplementary language files
         else if (!bEgosoftFile)
         {
            // Extract an 'ST_INTERNAL' text preview
            szPreviewText[0] = generateInternalGameStringPreview(pGameString, 96);
            szPreviewText[1] = generateInternalGameStringPreview(pConflictString, 96);
            
            /// [WARNING] "Page:%d  String:%d : '%s' overwritten by '%s'"
            generateQueuedWarning(pMergeQueue, HERE(IDS_LANGUAGE_SUPPLEMENTARY_STRING_MERGED), pGameString->iPageID, pGameString->iID, szPreviewText[1], szPreviewText[0]);
            utilDeleteStrings(szPreviewText[0], szPreviewText[1]);      // NB: If individual reporting if OFF, Push warning into 'Merge Queue'

            // Increment overwritten string count
            pOperationData->xOutput++;
         }
      }
      
      /// [OVER-WRITE] Destroy the conflicting GameString and insert replacement
      destroyObjectInAVLTreeByValue(pOperationData->pOutputTree, pConflictString->iPageID, pConflictString->iID);
      insertObjectIntoAVLTree(pOperationData->pOutputTree, (LPARAM)pGameString);
   }
}


