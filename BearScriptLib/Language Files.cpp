//
// Language Files.cpp : Functions regarding language files
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"


/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       CREATION / DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createLanguageFile
// Description     : Create an empty language, speech, description or variables file
// 
// CONST LANGUAGE_FILE_TYPE  eType          : [in]            Type of language file to create
// CONST TCHAR*              szFullPath     : [in] [optional] Full path or resource name
// CONST BOOL                bSupplementary : [in]            Whether file is master or supplementary  (Only used for LFT_STRINGS)
//
// Return Value   : New language, speech, description or variables file, you are responsible for destroying it
// 
BearScriptAPI
LANGUAGE_FILE*  createLanguageFile(CONST LANGUAGE_FILE_TYPE  eType, CONST TCHAR*  szFullPath, CONST BOOL  bSupplementary)
{
   LANGUAGE_FILE*  pLanguageFile;
   TCHAR*          szCustomPath;

   // Create new LanguageFile
   pLanguageFile = utilCreateEmptyObject(LANGUAGE_FILE);

   // Set properties
   pLanguageFile->bMasterStrings = (eType == LFT_STRINGS AND !bSupplementary);
   
   // Examine type
   switch (pLanguageFile->eType = eType)
   {
   /// [STRINGS] Create GameString and GamePage trees
   case LFT_STRINGS:
      // Create trees
      pLanguageFile->pGameStringsByID = createGameStringTreeByPageID();
      pLanguageFile->pGamePagesByID   = createGamePageTreeByID();

      // [CHECK] Is this the Master game strings file?
      if (bSupplementary)
         /// [SUPPLEMENTARY] Use input path
         setGameFilePath(pLanguageFile, szFullPath);
      else
      {
         /// [MASTER] Automatically generate filepath
         szCustomPath = generateGameLanguageFilePath(GFI_MASTER_LANGUAGE, getAppPreferences()->szGameFolder, getAppPreferences()->eGameVersion, getAppPreferences()->eGameLanguage);
         setGameFilePath(pLanguageFile, szCustomPath);

         // Cleanup
         utilDeleteString(szCustomPath);
      }
      break;

   /// [SPEECH] Create SpeechClip and SpeechPage trees
   case LFT_SPEECH:
      // Create trees
      pLanguageFile->pSpeechClipsByID    = createMediaItemTreeByPageID();
      pLanguageFile->pSpeechPagesByGroup = createMediaPageTreeByGroup();

      // Automatically generate filepath
      szCustomPath = generateGameLanguageFilePath(GFI_SPEECH_CLIPS, getAppPreferences()->szGameFolder, getAppPreferences()->eGameVersion, getAppPreferences()->eGameLanguage);
      setGameFilePath(pLanguageFile, szCustomPath);

      // Cleanup
      utilDeleteString(szCustomPath);
      break;

   /// [DESCRIPTION TEXT] Create DescriptionString and GamePage trees
   case LFT_DESCRIPTIONS:
      // Create single tree
      pLanguageFile->pGameStringsByVersion = createGameStringTreeByVersion();
      pLanguageFile->pGamePagesByID        = createGamePageTreeByID();
      break;

   /// [DESCRIPTION VARIABLES] Create single tree
   case LFT_VARIABLES:
      // Create single tree
      pLanguageFile->pVariablesByText = createDescriptionVariableTreeByName();
      break;
   }

   // Return new LanguageFile object
   return pLanguageFile;
}


/// Function name  : createUserLanguageFile
// Description     : Create a 'strings' language file with a single empty page
// 
// CONST TCHAR*  szFullPath     : [in] Full path
//
// Return Value   : New language file, you are responsible for destroying it
// 
BearScriptAPI
LANGUAGE_FILE*  createUserLanguageFile(CONST TCHAR*  szFullPath, const GAME_LANGUAGE  eLanguage)
{
   LANGUAGE_FILE*  pLanguageFile = createLanguageFile(LFT_STRINGS, szFullPath, TRUE);

   // Set language
   pLanguageFile->eLanguage = eLanguage;

   // Insert empty page
   insertGamePageIntoLanguageFile(pLanguageFile, 1, TEXT("Title"), TEXT("Description"), FALSE);

   // Return
   return pLanguageFile;
}


/// Function name  : deleteLanguageFile
// Description     : Delete a language file 
// 
// LANGUAGE_FILE*  &pLanguageFile : [in] 
// 
BearScriptAPI
VOID  deleteLanguageFile(LANGUAGE_FILE*  &pLanguageFile)
{
   // [STRINGS] Delete game string trees
   if (pLanguageFile->pGameStringsByID)
      deleteAVLTree(pLanguageFile->pGameStringsByID);

   if (pLanguageFile->pGamePagesByID)
      deleteAVLTree(pLanguageFile->pGamePagesByID);

   // [SPEECH] Delete speech clip trees
   if (pLanguageFile->pSpeechClipsByID)
      deleteAVLTree(pLanguageFile->pSpeechClipsByID);

   if (pLanguageFile->pSpeechPagesByGroup)
      deleteAVLTree(pLanguageFile->pSpeechPagesByGroup);

   // [VARIABLES] Delete Variables trees
   if (pLanguageFile->pVariablesByText)
      deleteAVLTree(pLanguageFile->pVariablesByText);

   // [DESCRIPTIONS] Delete game string trees 
   if (pLanguageFile->pGameStringsByVersion)
      deleteAVLTree(pLanguageFile->pGameStringsByVersion);

   // Delete underlying GameFile buffers
   deleteGameFileIOBuffers(pLanguageFile);

   // Delete calling object
   utilDeleteObject(pLanguageFile);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : findLanguageFileGamePageByID
// Description     : Search a LanguageFile's tree of GamePages for a GamePage with a specified PageID
// 
// LANGUAGE_FILE*  pLanguageFile : [in]  Language file to search
// CONST UINT      iPageID       : [in]  Page ID to search for
// GAME_PAGE*     &pOutput       : [out] GamePage, if found
// 
// Return Value   : TRUE if found, FALSE if not
// 
BOOL   findLanguageFileGamePageByID(CONST LANGUAGE_FILE*  pLanguageFile, CONST UINT  iPageID, GAME_PAGE*  &pOutput)
{ 
   // Search LanguageFile's 'GamePages' tree
   return findObjectInAVLTreeByValue(pLanguageFile->pGamePagesByID, iPageID, NULL, (LPARAM&)pOutput);
}


/// Function name  : findLanguageFileGameStringByID
// Description     : Search a LanguageFile's tree of GameStrings for one with a specified Page ID and String ID
// 
// LANGUAGE_FILE*  pLanguageFile : [in]  Language file to search
// CONST UINT      iStringID     : [in]  String ID to search for
// CONST UINT      iPageID       : [in]  Page ID to search for
// GAME_STRING*   &pOutput       : [out] GamePage, if found
// 
// Return Value   : TRUE if found, FALSE if not
// 
BOOL   findLanguageFileGameStringByID(CONST LANGUAGE_FILE*  pLanguageFile, CONST UINT  iStringID, CONST UINT  iPageID, GAME_STRING*  &pOutput)
{ 
   // Search LanguageFile's 'GameString' tree
   return findObjectInAVLTreeByValue(pLanguageFile->pGameStringsByID, iPageID, iStringID, (LPARAM&)pOutput);
}


/// Function name  : identifyGameLanguageString
// Description     : Identifies a game language string, used for error reporting
// 
// CONST GAME_LANGUAGE  eLanguage : [in] Game language ID
// 
// Return Value   : String representation
// 
CONST TCHAR*  identifyGameLanguageString(CONST GAME_LANGUAGE  eLanguage)
{
   CONST TCHAR*   szOutput;      // Operation result

   // Examine language
   switch (eLanguage)
   {
   case GL_ENGLISH:   szOutput = TEXT("English");  break;
   case GL_FRENCH:    szOutput = TEXT("French");   break;
   case GL_GERMAN:    szOutput = TEXT("German");   break;
   case GL_ITALIAN:   szOutput = TEXT("Italian");  break;
   case GL_POLISH:    szOutput = TEXT("Polish");   break;
   case GL_RUSSIAN:   szOutput = TEXT("Russian");  break;
   case GL_SPANISH:   szOutput = TEXT("Spanish");  break;
   default:           szOutput = TEXT("Invalid");  break;
   }

   // Return result
   return szOutput;
}


/// Function name  : isGameLanguageValid
// Description     : Ensures a GameLanguage is a recognised value
// 
// const GAME_LANGUAGE  eLanguage   : [in] Language
// 
// Return Value   : TRUE/FALSE
// 
BOOL  isGameLanguageValid(const GAME_LANGUAGE  eLanguage)
{
   // Examine language
   switch (eLanguage)
   {
   case GL_ENGLISH:   
   case GL_FRENCH:    
   case GL_GERMAN:    
   case GL_ITALIAN:   
   case GL_POLISH:    
   case GL_RUSSIAN:   
   case GL_SPANISH:   return TRUE;
   default:           return FALSE;
   }
}

/// Function name  : identifyLanguageFile
// Description     : Provides a description of a LanguageFile for VERBOSE output
// 
// CONST LANGUAGE_FILE*  pLanguageFile : [in] Language/speech file
// 
// Return Value   : fixed string
// 
CONST TCHAR*  identifyLanguageFile(CONST LANGUAGE_FILE*  pLanguageFile)
{
   // [CHECK]
   if (!pLanguageFile)
      return TEXT("uninitialised language");

   // Examine type
   else switch (pLanguageFile->eType)
   {
   case LFT_SPEECH:        return TEXT("speech clip");
   case LFT_VARIABLES:     return TEXT("description variables");
   case LFT_DESCRIPTIONS:  return TEXT("object descriptions");
   default:                return (isLanguageFileMaster(pLanguageFile) ? TEXT("master language") : TEXT("supplementary language"));
   }  
}


/// Function name  : isLanguageFileMaster
// Description     : Identifies whether a LanguageFile is one of the libraries supplied with the game
// 
// CONST LANGUAGE_FILE*  pLanguageFile : [in] LanguageFile to check
// 
// Return Value   : TRUE/FALSE
// 
BOOL  isLanguageFileOfficial(CONST LANGUAGE_FILE*  pLanguageFile)
{
   CONST TCHAR  *szFileID,       // Start of file ID
                *szFileName;     // Filename
   BOOL          bResult;        // Result

   // Prepare
   szFileName = PathFindFileName(pLanguageFile->szFullPath);
   bResult    = FALSE;

   // [CHECK] Ensure filename is valid
   if (lstrlen(szFileName) >= 6)
   {
      /// [THREAT/REUNION] Format: 'xxnnnn.pck'       (x = Language ID, n = File ID)
      /// [TERRAN CONFLICT/ABLION PRELUDE] Format: 'nnnn-L0xx.pck'      (x = Language ID, n = File ID)

      // Determine start position
      szFileID = (getAppPreferences()->eGameVersion <= GV_REUNION ? &szFileName[2] : szFileName);

      // [CHECK] Does filename start 0001, 0002, 0003 or 0004 ?
      bResult = (utilCompareStringN(szFileID, "0001", 4) OR utilCompareStringN(szFileID, "0002", 4) OR utilCompareStringN(szFileID, "0003", 4) OR utilCompareStringN(szFileID, "0004", 4));
   }

   // Return result
   return bResult;
}

/// Function name  : isLanguageFileMaster
// Description     : Identifies whether a LanguageFile represents the main game data language file
// 
// CONST LANGUAGE_FILE*  pLanguageFile   : [in] LanguageFile to check
// 
// Return Value   : TRUE/FALSE
// 
BOOL  isLanguageFileMaster(CONST LANGUAGE_FILE*  pLanguageFile)
{
   return pLanguageFile->bMasterStrings;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : generateLanguageFileXML
// Description     : Fill the output buffer of a LanguageFile with the XML equivilent of it's current contents.
// 
// LANGUAGE_FILE*       pLanguageFile : [in/out] LanguageFile containing the strings/pages to convert.
// OPERATION_PROGRESS*  pProgress     : [out]    Operation progress
// ERROR_QUEUE*         pErrorQueue   : [out]    Error message, if any
// 
// Return Value   : TRUE if succesful, FALSE if there were errors
// 
BOOL  generateLanguageFileXML(LANGUAGE_FILE*  pLanguageFile, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue)
{
   AVL_TREE_OPERATION*  pOperation = createAVLTreeOperationEx(treeprocGenerateLanguageXML, ATT_INORDER, pErrorQueue, pProgress);
   TEXT_STREAM*         pOutputStream;     // XML output stream
   AVL_TREE*            pOrderedTree;

   // [VERBOSE]
   TRACK_FUNCTION();
   VERBOSE("Generating XML for %s file '%s'", identifyLanguageFile(pLanguageFile), pLanguageFile->szFullPath);

   // [STAGE]
   updateOperationProgressMaximum(pProgress, getTreeNodeCount(pLanguageFile->pGameStringsByID));

   // Prepare
   pOperation->xFirstInput = (LPARAM)(pOutputStream = createTextStream(64 * 1024));
   pOperation->pInputTree  = pLanguageFile->pGamePagesByID;

   // Add schema tags
   appendStringToTextStream(pOutputStream, TEXT("<?xml version=\"1.0\" standalone=\"yes\" encoding=\"UTF-8\"?>\r\n"));
   appendStringToTextStreamf(pOutputStream, TEXT("<-- Generated using %s -->\r\n"), getAppName());
   appendStringToTextStreamf(pOutputStream, TEXT("<language id=\"%d\">\r\n"), pLanguageFile->eLanguage);

   /// Create a copy of all strings with order: PAGE_ID, GAME_VERSION, STRING_ID
   pOrderedTree = duplicateAVLTree(pLanguageFile->pGameStringsByID, createAVLTreeSortKey(AK_PAGE_ID, AO_ASCENDING), createAVLTreeSortKey(AK_VERSION, AO_ASCENDING), createAVLTreeSortKey(AK_ID, AO_ASCENDING));
   pOrderedTree->pfnDeleteNode = NULL;

   /// Generate <page> and <t> tags
   performOperationOnAVLTree(pOrderedTree, pOperation);

   // Close final <page> tag
   appendStringToTextStream(pOutputStream, TEXT("\t</page>\r\n"));
   appendStringToTextStream(pOutputStream, TEXT("</language>\r\n"));

   // Copy output to LanguageFile
   pLanguageFile->szOutputBuffer = utilDuplicateString(pOutputStream->szBuffer, pOutputStream->iBufferUsed);
   pLanguageFile->iOutputSize    = pOutputStream->iBufferUsed;

   // Cleanup and return
   pOperation->pErrorQueue = NULL;
   deleteAVLTreeOperation(pOperation);
   deleteAVLTree(pOrderedTree);
   deleteTextStream(pOutputStream);
   END_TRACKING();
   return TRUE;
}

/// Function name  : insertGamePageIntoLanguageFile
// Description     : Attempts to insert a new GamePage into a LanguageFile.  If a conflict is found, the existing page is updated
//                      with the values from the new page, therefore the operation never fails.
// 
// LANGUAGE_FILE*  pLanguageFile  : [in] LanguageFile into which the GamePage will be inserted
// CONST UINT      iPageID        : [in] ID of the new GamePage
// CONST TCHAR*    szTitle        : [in] Title for the new GamePage
// CONST TCHAR*    szDescription  : [in] Description for the new GamePage
// CONST BOOL      bVoiced        : [in] Whether the new page is voiced
// 
VOID    insertGamePageIntoLanguageFile(LANGUAGE_FILE*  pLanguageFile, CONST UINT  iPageID, CONST TCHAR*  szTitle, CONST TCHAR*  szDescription, CONST BOOL  bVoiced)
{
   GAME_PAGE    *pNewGamePage;           // GamePage being inserted

   // Create new GamePage
   pNewGamePage = createGamePage(iPageID, szTitle, szDescription, bVoiced);

   /// Attempt to insert GamePage into LanguageFile
   if (!insertObjectIntoAVLTree(pLanguageFile->pGamePagesByID, (LPARAM)pNewGamePage))
   {
      // Destroy page and replace
      destroyObjectInAVLTreeByValue(pLanguageFile->pGamePagesByID, pNewGamePage->iPageID, NULL);
      insertObjectIntoAVLTree(pLanguageFile->pGamePagesByID, (LPARAM)pNewGamePage);
   }
}


/// Function name  : insertGameStringIntoDescriptionsFile
// Description     : Creates a new GameString and inserts into a Descriptions file
// 
// LANGUAGE_FILE*  pLanguageFile : [in]  LanguageFile to insert the new GameString into
// CONST TCHAR*    szString      : [in]  Text of the GameString
// CONST UINT      iID           : [in]  ID of the GameString
// CONST UINT      iPageID       : [in]  PageID of the GameString
// ERROR_STACK*   &pError        : [out] Error message, if any
// 
// Return Value   : TRUE if inserted successfully, FALSE if the string was already present
// 
BOOL   insertGameStringIntoDescriptionsFile(LANGUAGE_FILE*  pDescriptionsFile, CONST TCHAR*  szString, CONST UINT  iID, CONST UINT  iPageID, ERROR_STACK*  &pError)
{
   GAME_STRING    *pGameString;     // GameString being inserted
             
   // Prepare
   pError = NULL;

   // Create external GameString
   pGameString = createGameString(szString, iID, iPageID, ST_EXTERNAL);

   /// Attempt to insert Descriptions file string tree
   if (!insertObjectIntoAVLTree(pDescriptionsFile->pGameStringsByVersion, (LPARAM)pGameString))
   {
      // [ERROR] "Unable to insert the duplicate %s object description with ID:%u in page %u"
      pError = generateDualError(HERE(IDS_DESCRIPTION_DUPLICATE), identifyGameVersionString(pGameString->eVersion), iPageID, iID);
      deleteGameString(pGameString);
   }

   // Return TRUE if successful
   return (pError == NULL);
}


/// Function name  : insertGameStringIntoLanguageFile
// Description     : Attempts to create a new GameString and insert it into a specified LanguageFile
// 
// LANGUAGE_FILE*  pLanguageFile : [in]  LanguageFile to insert the new GameString into
// CONST TCHAR*    szString      : [in]  Text of the GameString
// CONST UINT      iID           : [in]  ID of the GameString
// CONST UINT      iPageID       : [in]  PageID of the GameString
// ERROR_STACK*   &pError        : [out] Error message, if any
// 
// Return Value   : TRUE if inserted successfully, FALSE if the string was already present
// 
BOOL   insertGameStringIntoLanguageFile(LANGUAGE_FILE*  pLanguageFile, CONST TCHAR*  szString, CONST UINT  iID, CONST UINT  iPageID, ERROR_STACK*  &pError)
{
   TCHAR          *szPreviewText[2];         // Truncated version of the game string text, used for warning reporting
   GAME_STRING    *pNewGameString,           // GameString being inserted
                  *pConflictString;      // Existing GameString causing a conflict
   // Prepare
   pError = NULL;

   // Create external GameString
   pNewGameString = createGameString(szString, iID, iPageID, ST_EXTERNAL);

   /// Attempt to insert language file's string tree
   if (!insertObjectIntoAVLTree(pLanguageFile->pGameStringsByID, (LPARAM)pNewGameString))
   {
      // [CONFLICT] Extract conflict for display and replacement
      findLanguageFileGameStringByID(pLanguageFile, pNewGameString->iID, pNewGameString->iPageID, pConflictString);
      ASSERT(pConflictString);

      // [CHECK] Is the text or version actually different?
      if (pNewGameString->eVersion > pConflictString->eVersion OR !utilCompareStringVariables(pNewGameString->szText, pConflictString->szText))
      {
         // [CHECK] Suppress warnings for Egosoft files. Also suppress by preferences. 
         if (!isLanguageFileOfficial(pLanguageFile) AND getAppPreferences()->bReportGameStringOverwrites)
         {
            // [NON-OFFICIAL] Extract a portion of the GameString text and convert to ST_INTERNAL
            szPreviewText[0] = generateInternalGameStringPreview(pNewGameString, 96);
            szPreviewText[1] = generateInternalGameStringPreview(pConflictString, 96);

            /// [WARNING] "The string %u:'%s' found in '%s' has overwritten '%s' (page %u) found within same file"
            pError = generateDualWarning(HERE(IDS_LANGUAGE_FILE_STRING_MERGED), iID, szPreviewText[0], PathFindFileName(pLanguageFile->szFullPath), szPreviewText[1], pConflictString->iPageID);
            utilDeleteStrings(szPreviewText[0], szPreviewText[1]);
         }

         /// [OVERWRITE-INPLACE] Overwrite the TEXT (both are ST_EXTERNAL) and the VERSION 
         updateGameStringText(pConflictString, szString);
         pConflictString->eVersion = pNewGameString->eVersion;
      }
     
      // Cleanup input string
      deleteGameString(pNewGameString);
   }

   // Return TRUE if there was no error
   return (pError == NULL);
}


/// Function name  : loadLanguageFile
// Description     : Loads a language file from disc/resource and translates it into processed GameString trees
// 
// CONST FILE_SYSTEM*   pFileSystem   : [in]     [optional] VirtualFileSystem object  (Can be NULL if loading from a resource)
// LANGUAGE_FILE*       pTargetFile   : [in/out]            LanguageFile used to process the XML and hold the output trees
///                                                            Disc:     szFullPath contains file path.  Input buffer is NULL.
///                                                            Resource: szFullPath is NULL.  Input buffer contains raw XML.
// CONST BOOL           bSubStrings   : [in]                Whether to resolve substrings
// HWND                 hParentWnd    : [in]                Parent window for handling UM_PROCESSING_ERROR messages
// OPERATION_PROGRESS*  pProgress     : [in/out] [optional] Operation progress object
// ERROR_QUEUE*         pErrorQueue   : [in/out]            Error or warning messages, if any
// 
// Return Value  : OR_SUCCESS - Input file was translated successfully into post-processed GameString trees. Any errors were ignored.
//                 OR_FAILURE - Input file was NOT translated due to an I/O error or corrupt XML structure. GameString trees were NOT created.
//                 OR_ABORTED - Input file was NOT translated because the user aborted after a minor error. GameString trees were NOT created.
// 
OPERATION_RESULT  loadLanguageFile(CONST FILE_SYSTEM*  pFileSystem, LANGUAGE_FILE*  pTargetFile, CONST BOOL  bSubStrings, HWND  hParentWnd, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE* pErrorQueue)
{
   OPERATION_RESULT   eResult;      // Operation result

   // [VERBOSE]
   TRACK_FUNCTION();
   VERBOSE_HEADING("Loading %s file '%s'", identifyLanguageFile(pTargetFile), pTargetFile->szFullPath);

   // [CHECK] Ensure resource files have data, and disc files don't.
   ASSERT(!pTargetFile->bResourceBased == !hasInputData(pTargetFile));

   // Prepare
   eResult = OR_FAILURE;

   /// [DISC FILE] Load XML into buffer
   if (!pTargetFile->bResourceBased AND !loadGameFileFromFileSystemByPath(pFileSystem, pTargetFile, TEXT(".xml"), pErrorQueue))
   {
      // [ERROR] "There was an I/O error while loading the %s file '%s'"
      enhanceLastError(pErrorQueue, ERROR_ID(IDS_LANGUAGE_FILE_IO_ERROR), identifyLanguageFile(pTargetFile), PathFindFileName(pTargetFile->szFullPath));
      generateOutputTextFromLastError(pErrorQueue);
   }
   /// Parse XML into trees
   else if ((eResult = translateLanguageFile(pTargetFile, hParentWnd, pProgress, pErrorQueue)) == OR_SUCCESS)
   {
      // [SUCCESS] Perform post-processing
      switch (pTargetFile->eType)
      {
      /// [STRINGS] Convert string type. Resolve substrings. Index pages.
      case LFT_STRINGS:
         // [CHECK] Not used during loading of supplementary game LanguageFiles
         if (pProgress)
         {
            // [PROGRESS] Define progress as number of strings converted
            advanceOperationProgressStage(pProgress);    //ASSERT(advanceOperationProgressStage(pProgress) == IDS_PROGRESS_CONVERTING_MASTER_STRINGS);
            updateOperationProgressMaximum(pProgress, getTreeNodeCount(pTargetFile->pGameStringsByID));  
         }

         // Convert strings from 'external' to 'internal'
         performLanguageFileStringConversion(pTargetFile, SPC_LANGUAGE_EXTERNAL_TO_INTERNAL, pProgress);
         break;

      /// [SPEECH] None required
      case LFT_SPEECH:
         break;

      /// [VARIABLES] Convert string type.
      case LFT_VARIABLES:
         // Convert strings from 'external' to 'internal'
         performVariablesFileStringConversion(pTargetFile);
         break;

      /// [DESCRIPTIONS] Convert string type.
      case LFT_DESCRIPTIONS:
         // Convert strings from 'external' to 'internal'
         performLanguageFileStringConversion(pTargetFile, SPC_LANGUAGE_EXTERNAL_TO_INTERNAL, NULL);
         break;
      }
   }

   // Cleanup XML buffer
   deleteGameFileIOBuffers(pTargetFile);

   // Return result
   END_TRACKING();
   return eResult;
}


/// Function name  : performLanguageFileStringConversion
// Description     : Convert all the GameStrings in a LanguageFile or DescriptionsFile from EXTERNAL to INTERNAL
// 
// LANGUAGE_FILE*       pTargetFile : [in]            LanguageFile/DescriptionsFile containing the tree to convert
// OPERATION_PROGRESS*  pProgress   : [in] [optional] Progress tracking
// 
VOID    performLanguageFileStringConversion(LANGUAGE_FILE*  pTargetFile, const UINT  iConversionFlags, OPERATION_PROGRESS*  pProgress)
{
   AVL_TREE_OPERATION*   pOperationData;

   // [VERBOSE]
   VERBOSE("Converting %s file '%s' strings from external to internal", identifyLanguageFile(pTargetFile), PathFindFileName(pTargetFile->szFullPath));

   // Setup conversion operation
   pOperationData = createAVLTreeOperationEx(treeprocConvertGameStringToInternal, ATT_INORDER, NULL, pProgress);
   pOperationData->xFirstInput = iConversionFlags;
   
   /// Convert all GameStrings in the appropriate tree to 'INTERNAL'
   switch (pTargetFile->eType)
   {
   case LFT_STRINGS:       performOperationOnAVLTree(pTargetFile->pGameStringsByID, pOperationData);       break;
   case LFT_DESCRIPTIONS:  performOperationOnAVLTree(pTargetFile->pGameStringsByVersion, pOperationData);  break;
   }
   
   // Cleanup
   deleteAVLTreeOperation(pOperationData);
}


/// Function name  : performLanguageFileStringConversion
// Description     : Convert all the descriptions in a VariablesFile's tree from EXTERNAL to INTERNAL
// 
// VARIABLES_FILE*  pVariablesFile : [in] VariablesFile containing the tree to convert
// 
VOID    performVariablesFileStringConversion(VARIABLES_FILE*  pVariablesFile)
{
   AVL_TREE_OPERATION*   pOperationData;

   // [VERBOSE]
   VERBOSE("Converting %s file '%s' strings from external to internal", identifyLanguageFile(pVariablesFile), PathFindFileName(pVariablesFile->szFullPath));

   // [CHECK] Ensure language file is not a SpeechFile
   ASSERT(pVariablesFile->eType == LFT_VARIABLES AND pVariablesFile->pVariablesByText)

   // Setup conversion operation
   pOperationData = createAVLTreeOperation(treeprocConvertDescriptionVariableToInternal, ATT_INORDER);
   
   /// Convert all descriptions in the tree to 'INTERNAL'
   performOperationOnAVLTree(pVariablesFile->pVariablesByText, pOperationData);
   
   // Cleanup
   deleteAVLTreeOperation(pOperationData);
}



/// Function name  : translateLanguageFile
// Description     : Builds the trees within a LanguageFile from the XML within the input buffer
//
///                     LANGUAGE STRING FILES:
//                        -> The strings tree contains only the most recent version of each string; Conflicting strings from previous or equal versions are discarded.
//                        -> If conflicting pages are found, the title and description is updated (if present)
//
///                     SPEECH CLIP FILES:
//                        -> If conflicting items are found, their properties are overwritten
//                        -> Pages have no properties so conflicts are discarded
//
///                     VARIABLES FILES:
//                        -> If conflicting items are found, they are discarded
//                        -> Pages are ignored
//
// LANGUAGE_FILE*       pTargetFile   : [in/out]           Language file containing raw XML in the input buffer
// HWND                 hParentWnd    : [in]               Parent window for any error dialogs that are displayed
// OPERATION_PROGRESS*  pProgress     : [in/out][optional] Operation progress
// ERROR_QUEUE*         pErrorQueue   : [out]              Error message, if any
// 
// Return type : OR_SUCCESS - The file was translated successfully (If there were errors, the user ignored them)
//               OR_FAILURE - The file was NOT translated because it was in a foreign language or the <language> tag was missing
//               OR_ABORTED - The file was partially translated - the user aborted translation due to errors
//
OPERATION_RESULT  translateLanguageFile(LANGUAGE_FILE*  pTargetFile, HWND  hParentWnd, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue)
{
   OPERATION_RESULT    eResult;           // Operation result
   XML_TREE_NODE      *pLanguageNode,     // Node of the XML Tree containing the language tag
                      *pPageNode,         // Node of the XML Tree containing the page currently being processed
                      *pItemNode;         // Node of the XML Tree containing the string currently being processed
   XML_TREE           *pXMLTree;          // XML Tree of the language file
   ERROR_STACK        *pError;            // Error encountered, if any
   LANGUAGE_FILE_ITEM  oItemData;         // Convenience object for storing item data as it's parsed

   // [VERBOSE]
   TRACK_FUNCTION();
   VERBOSE("Translating %s file '%s'", identifyLanguageFile(pTargetFile), pTargetFile->szFullPath);
   
   // [CHECK] Language file input buffer exists
   ASSERT(hasInputData(pTargetFile));
   
   // Prepare
   utilZeroObject(&oItemData, LANGUAGE_FILE_ITEM);
   eResult = OR_SUCCESS;
   pError  = NULL;

   /// Parse XML into an XML Tree      [Requires current operation stage + Next operation stage]
   if (generateXMLTree(pTargetFile->szInputBuffer, pTargetFile->iInputSize, identifyGameFileFilename(pTargetFile), hParentWnd, pXMLTree, pProgress, pErrorQueue) == OR_SUCCESS)
   {
      // Find language node
      if (!findXMLTreeNodeByName(pXMLTree->pRoot, TEXT("language"), TRUE, pLanguageNode) OR !getXMLPropertyInteger(pLanguageNode, TEXT("id"), (INT&)pTargetFile->eLanguage))
         // [ERROR] "Could not find the <language> tag in the %s file '%s' or the <language> tag does not specify a language ID"
         pError = generateDualError(HERE(IDS_LANGUAGE_TAG_MISSING), identifyLanguageFile(pTargetFile), identifyGameFileFilename(pTargetFile));
      
      // [CHECK] Ensure language matches the GameData language
      else if (pTargetFile->eLanguage != getAppPreferences()->eGameLanguage)
         // [WARNING] "The language of the %s file '%s' is %s which does not match your chosen game data language of %s"
         pushErrorQueue(pErrorQueue, generateDualWarning(HERE(IDS_LANGUAGE_TAG_MISMATCH), identifyLanguageFile(pTargetFile), identifyGameFileFilename(pTargetFile), identifyGameLanguageString(pTargetFile->eLanguage), identifyGameLanguageString(getAppPreferences()->eGameLanguage)));
      
      // [CHECK] Ensure language is valid
      if (!isGameLanguageValid(pTargetFile->eLanguage))
         pTargetFile->eLanguage = getAppPreferences()->eGameLanguage;

      /// [ERROR] Return FAILURE due to the language not matching
      if (pError)
      {
         eResult = OR_FAILURE;
         pushErrorQueue(pErrorQueue, pError);
      }
      else 
      {
         // [STAGE] Move to IDS_PROGRESS_STORING_MASTER_STRINGS / IDS_PROGRESS_PROCESSING_LANGUAGE / IDS_PROGRESS_TRANSLATING_SPEECH_CLIPS
         if (pProgress)   // [Not used by supplementary langauge files]
         {
            // [PROGRESS] Define progress based on the number of pages processed
            advanceOperationProgressStage(pProgress);
            updateOperationProgressMaximum(pProgress, getXMLNodeCount(pLanguageNode));
         }

         /// Iterate through page nodes (break upon errors)
         for (findNextXMLTreeNode(pLanguageNode, XNR_CHILD, pPageNode); pPageNode AND eResult == OR_SUCCESS; findNextXMLTreeNode(pPageNode, XNR_SIBLING, pPageNode))
         {
            // [CHECK] Ensure node is a <page> node
            if (!utilCompareString(pPageNode->szName, "page"))
               // [ERROR] "An unexpected <%s> tag was found where a <%s> tag was expected on line %u of the %s file '%s'"
               pError = generateDualError(HERE(IDS_LANGUAGE_UNEXPECTED_TAG), pPageNode->szName, TEXT("page"), pPageNode->iLineNumber, identifyLanguageFile(pTargetFile), identifyGameFileFilename(pTargetFile));
            
            // Determine page ID
            else if (!getXMLPropertyInteger(pPageNode, TEXT("id"), oItemData.iRawPageID))
               // [ERROR] "The <%s> tag on line %u of the %s file '%s' contains an invalid or missing ID property"
               pError = generateDualError(HERE(IDS_LANGUAGE_TAG_ID_MISSING), TEXT("page"), pPageNode->iLineNumber, identifyLanguageFile(pTargetFile), identifyGameFileFilename(pTargetFile));
            
            else
            {
               /// [NEW PAGE] Generate a new page from the <page> tag
               switch (pTargetFile->eType)
               {
               // [STRINGS/DESCRIPTIONS] Insert new GamePage into LanguageFile
               case LFT_DESCRIPTIONS:
               case LFT_STRINGS:
                  // Extract GamePage properties
                  getXMLPropertyString(pPageNode, TEXT("description"), oItemData.szDescription);
                  getXMLPropertyString(pPageNode, TEXT("title"),       oItemData.szTitle);
                  getXMLPropertyString(pPageNode, TEXT("voice"),       oItemData.szVoiced);

                  // Insert new GamePage
                  insertGamePageIntoLanguageFile(pTargetFile, (UINT)oItemData.iRawPageID, oItemData.szTitle, oItemData.szDescription, utilCompareString(oItemData.szVoiced, "yes"));
                  break;
               
               // [SPEECH] Insert new MediaPage into SpeechFile
               case LFT_SPEECH:
                  insertMediaPageIntoSpeechFile(pTargetFile, (UINT)oItemData.iRawPageID);
                  break;

               // [VARIABLES] Ignore page
               case LFT_VARIABLES:
                  break;
               }  

               /// Iterate through item nodes (abort on error)
               for (findNextXMLTreeNode(pPageNode, XNR_CHILD, pItemNode); pItemNode AND eResult == OR_SUCCESS; findNextXMLTreeNode(pItemNode, XNR_SIBLING, pItemNode))
               {
                  // [CHECK] Ensure node is a <t> node
                  if (!utilCompareString(pItemNode->szName, "t"))
                     // [ERROR] "An unexpected <%s> tag was found where a <%s> tag was expected on line %u of the %s file '%s'"
                     pError = generateDualError(HERE(IDS_LANGUAGE_UNEXPECTED_TAG), pItemNode->szName, TEXT("t"), pItemNode->iLineNumber, identifyLanguageFile(pTargetFile), identifyGameFileFilename(pTargetFile));

                  // [CHECK] Determine item ID.  (This is a string for variable LanguageFiles)
                  else if ((pTargetFile->eType != LFT_VARIABLES AND !getXMLPropertyInteger(pItemNode, TEXT("id"), oItemData.iItemID)) OR
                           (pTargetFile->eType == LFT_VARIABLES AND !getXMLPropertyString(pItemNode, TEXT("id"), oItemData.szVariable)))
                     // [ERROR] "The <%s> tag on line %u of the %s file '%s' contains an invalid or missing ID property"
                     pError = generateDualError(HERE(IDS_LANGUAGE_TAG_ID_MISSING), TEXT("t"), pItemNode->iLineNumber, identifyLanguageFile(pTargetFile), identifyGameFileFilename(pTargetFile));
                  
                  /// [NEW ITEM] Generate new item from the <string> tag
                  else switch (pTargetFile->eType)
                  {
                  // [DESCRIPTIONS] Insert new GameString into LanguageFile
                  case LFT_DESCRIPTIONS:
                     insertGameStringIntoDescriptionsFile(pTargetFile, (pItemNode->szText ? pItemNode->szText : TEXT("")), (UINT)oItemData.iItemID, (UINT)oItemData.iRawPageID, pError);
                     break;

                  // [STRINGS] Insert new GameString into LanguageFile
                  case LFT_STRINGS:
                     // [FIX] BUG:014 'Empty string entries in supplementary language files produce a warning and are not processed'
                     insertGameStringIntoLanguageFile(pTargetFile, utilEither(pItemNode->szText, TEXT("")), (UINT)oItemData.iItemID, (UINT)oItemData.iRawPageID, pError);
#ifdef BUG_FIX
                     // [CHECK] Ensure string has text
                     if (pItemNode->szText == NULL)
                        // [ERROR] "The <string> tag on line %u of the file '%s' contains no text"
                        pError = generateDualError(HERE(IDS_LANGUAGE_STRING_TEXT_MISSING), pItemNode->iLineNumber, identifyGameFileFilename(pTargetFile));
                     else
                        // Insert new game string into the LanguageFile
                        insertGameStringIntoLanguageFile(pTargetFile, pItemNode->szText, iItemID, iPageID, pError);  
#endif
                     break;

                  // [SPEECH] Insert new MediaItem into SpeechFile
                  case LFT_SPEECH:
                     // Extract speech clip properties
                     getXMLPropertyInteger(pPageNode, TEXT("s"), oItemData.iPosition);
                     getXMLPropertyInteger(pPageNode, TEXT("l"), oItemData.iLength);

                     // Insert new speech clip into the game data
                     insertSpeechClipIntoSpeechFile(pTargetFile, (UINT)oItemData.iItemID, MPI_SPEECH_CLIPS, oItemData.iPosition, oItemData.iLength);
                     break;

                  // [VARIABLES]
                  case LFT_VARIABLES:
                     // Extract properties (if any)
                     getXMLPropertyInteger(pItemNode, TEXT("parameters"), oItemData.iParameters);
                     if (!getXMLPropertyInteger(pItemNode, TEXT("recursive"), oItemData.bRecursive))
                        // [NOT FOUND] Default to TRUE
                        oItemData.bRecursive = TRUE;
                     
                     // Insert new DescriptionVariable into VariablesFile
                     insertDescriptionIntoVariablesFile(pTargetFile, oItemData.szVariable, pItemNode->szText, oItemData.iParameters, oItemData.bRecursive, pError);
                     break;
                  }  
                  
                  /// [ERROR] Determine whether to continue or abort
                  if (pError)
                  {
                     // Store error and use as output text
                     generateOutputTextFromError(pError);
                     pushErrorQueue(pErrorQueue, pError);
                     pError = NULL;

                     // [QUESTION] "A minor error has been detected while loading the %s file '%s', would you like to continue processing?"
                     if (displayOperationError(hParentWnd, lastErrorQueue(pErrorQueue), ERROR_ID(IDS_LANGUAGE_FILE_MINOR_ERROR), identifyLanguageFile(pTargetFile), identifyGameFileFilename(pTargetFile)) == EH_ABORT)
                        eResult = OR_ABORTED;
                  }
               }

            } // END: If (page has 'id' property)

            // [OPTIONAL] Update progress
            if (pProgress)
               advanceOperationProgressValue(pProgress);

            /// [ERROR DURING FINAL ITERATION] Determine whether to continue or abort
            if (pError)
            {
               // Store error and use as output text
               generateOutputTextFromError(pError);
               pushErrorQueue(pErrorQueue, pError);
               pError = NULL;

               // [QUESTION] "A minor error has been detected while loading the %s file '%s', would you like to continue processing?"
               if (displayOperationError(hParentWnd, lastErrorQueue(pErrorQueue), ERROR_ID(IDS_LANGUAGE_FILE_MINOR_ERROR), identifyLanguageFile(pTargetFile), identifyGameFileFilename(pTargetFile)) == EH_ABORT)
                  eResult = OR_ABORTED;
            }

         } // END: for (each page node)

      } // END: if (no critical errors)

   } // END: if (generated XML Tree OK)

   // Cleanup and return result
   deleteXMLTree(pXMLTree);
   END_TRACKING();
   return eResult;
}


/// Function name  : treeprocGenerateLanguageXML
// Description     : Produces Languagefile XML
// 
// AVL_TREE_NODE*       pNode   : [in] GameString node
// AVL_TREE_OPERATION*  pData   : [in] xFirstInput : Output TextStream*
//                                     pInputTree  : Languagefile's GamePage tree
// 
VOID  treeprocGenerateLanguageXML(AVL_TREE_NODE*  pNode, AVL_TREE_OPERATION*  pData)
{
   TEXT_STREAM*   pStream         = (TEXT_STREAM*)pData->xFirstInput;
   GAME_STRING*   pString         = extractPointerFromTreeNode(pNode, GAME_STRING);
   GAME_VERSION&  eCurrentVersion = (GAME_VERSION&)pData->xInternalData1;     // xInternalData1 : Current PageID
   UINT&          iCurrentPageID  = (UINT&)pData->xInternalData2;             // xInternalData2 : Current String Version
   TCHAR*         szConverted     = NULL;

   /// [NEW PAGE/VERSION] Generate close/open page tags
   if (pString->iPageID != iCurrentPageID OR pString->eVersion != eCurrentVersion)
   {
      GAME_PAGE*  pPage;

      // Close existing <page> tag  (if any)
      if (iCurrentPageID != NULL)
         appendStringToTextStream(pStream, TEXT("\t</page>\r\n"));

      // Lookup GamePage. 
      findObjectInAVLTreeByValue(pData->pInputTree, pString->iPageID, NULL, (LPARAM&)pPage);

      // Update PageID + Version
      iCurrentPageID  = pString->iPageID;
      eCurrentVersion = pString->eVersion;
      
      /// [PAGE] Generate <page> tag
      appendStringToTextStreamf(pStream, TEXT("\r\n\t<page id=\"%d\" title=\"%s\" desc=\"%s\" voice=\"%s\">\r\n"), calculateOutputPageID(iCurrentPageID, eCurrentVersion), 
                                                                                                                   lstrlen(pPage->szTitle)       ? pPage->szTitle       : TEXT(""), 
                                                                                                                   lstrlen(pPage->szDescription) ? pPage->szDescription : TEXT(""), 
                                                                                                                   pPage->bVoice                 ? TEXT("yes")          : TEXT("no"));
   }

   /// [STRING] Generate <t> tag
   generateConvertedString(pString->szText, SPC_LANGUAGE_INTERNAL_TO_EXTERNAL, szConverted);
   appendStringToTextStreamf(pStream, TEXT("\t\t<t id=\"%d\">%s</t>\r\n"), pString->iID, utilEither(szConverted, pString->szText));

   // Cleanup
   utilSafeDeleteString(szConverted);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       THREAD FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : threadprocLoadLanguageFile
// Description     : Asynchronously generates a LanguageFile from a supplementary language file
// 
// VOID*  pParameter : [in/out] Operation data 
//                              'szFullPath'    - [in]  Full path of the file to load
///                             'pLanguageFile' - [out] LanguageFile containing the parsed contents of the above file
// 
// Return Value  : OR_SUCCESS - LanguageFile was created succesfully. If any errors were encountered, they were ignored.
//                 OR_FAILURE - LanguageFile was NOT created due to a missing/corrupt language file
//                 OR_ABORTED - LanguageFile was NOT created because the user aborted after a minor error
// 
BearScriptAPI
DWORD   threadprocLoadLanguageFile(VOID*  pParameter)
{
   DOCUMENT_OPERATION*   pOperationData;    // Thread input data
   LANGUAGE_FILE*        pLanguageFile;     // Operation result
   OPERATION_RESULT      eResult;           // Operation result

   // [TRACKING]
   TRACK_FUNCTION();
   VERBOSE_LIB_COMMAND();
   SET_THREAD_NAME("LanguageFile Parsing");
   setThreadLanguage(getAppPreferences()->eAppLanguage);

   // Prepare
   pOperationData = (DOCUMENT_OPERATION*)pParameter;
   pLanguageFile  = (LANGUAGE_FILE*)pOperationData->pGameFile;

   __try
   {
      // [INFO] Parsing XML in language file '%s'
      pushErrorQueue(pOperationData->pErrorQueue, generateDualInformation(HERE(IDS_OUTPUT_LOADING_LANGUAGE_XML), identifyGameFileFilename(pLanguageFile)));

      /// Translate the language file
      eResult = loadLanguageFile(getFileSystem(), pLanguageFile, TRUE, pOperationData->hParentWnd, pOperationData->pProgress, pOperationData->pErrorQueue);
   }
   __except (generateQueuedExceptionError(GetExceptionInformation(), pOperationData->pErrorQueue))
   {
      // [FAILURE] "An unidentified and unexpected critical error has occurred while parsing the language file '%s'"
      enhanceLastError(pOperationData->pErrorQueue, ERROR_ID(IDS_EXCEPTION_LOAD_LANGUAGE_FILE), pOperationData->szFullPath);
      eResult = OR_FAILURE;
   }

   // [DEBUG] Separate previous output from further output for claritfy
   VERBOSE_THREAD_COMPLETE("LANGUAGE-FILE PARSING WORKER THREAD COMPLETED");

   // Cleanup and return
   closeThreadOperation(pOperationData, eResult);
   END_TRACKING();
   return THREAD_RETURN;
}


/// Function name  : threadprocSaveLanguageFile
// Description     : Convert a LANGUAGE_FILE object to XML and save it to disc
// 
// VOID*  pParameter : [in/out] Operation data:
//                               'pLanguageFile' - [in] LanguageFile containing FilePath and language entries
//                               'szFullPath'    - [in] Target file path
// 
// Return Value   : Zero
//
// Operation Stages : TWO
// 
BearScriptAPI
DWORD   threadprocSaveLanguageFile(VOID*  pParameter)
{
   DOCUMENT_OPERATION*   pOperationData;   // Convenience pointer
   LANGUAGE_FILE*        pLanguageFile;     // Operation result
   OPERATION_RESULT      eResult;           // Operation result

   // [TRACKING]
   TRACK_FUNCTION();
   SET_THREAD_NAME("Language Generation");
   setThreadLanguage(getAppPreferences()->eAppLanguage);

   // Prepare
   pOperationData = (DOCUMENT_OPERATION*)pParameter;
   pLanguageFile  = (LANGUAGE_FILE*)pOperationData->pGameFile;
   eResult        = OR_SUCCESS;

   /// [GUARD BLOCK]
   __try
   {
      // [STAGE] Parsing XML in language file '%s'
      pushErrorQueue(pOperationData->pErrorQueue, generateDualInformation(HERE(IDS_OUTPUT_GENERATING_LANGUAGE_XML), identifyGameFileFilename(pLanguageFile)));
      ASSERT(getOperationProgressStageID(pOperationData->pProgress) == IDS_PROGRESS_GENERATING_LANGUAGE);

      /// Generate LanguageFile XML
      if (!generateLanguageFileXML(pLanguageFile, pOperationData->pProgress, pOperationData->pErrorQueue))
         // No enhancement necessary
         eResult = OR_FAILURE;
      
      /// Save LanguageFile to disk
      else if (!saveDocumentToFileSystem(TEXT("c:\\temp\\LanguageFileTest.txt"), pLanguageFile, pOperationData->pErrorQueue))    //pOperationData->szFullPath
      {
         // [ERROR] "There was an I/O error while loading the %s file '%s'"
         enhanceLastError(pOperationData->pErrorQueue, ERROR_ID(IDS_LANGUAGE_FILE_IO_ERROR), identifyGameFileFilename(pLanguageFile)); 
         eResult = OR_FAILURE;
      }
   }
   /// [EXCEPTION HANDLER]
   __except (generateQueuedExceptionError(GetExceptionInformation(), pOperationData->pErrorQueue))
   {
      // [ERROR] "An unidentified and unexpected critical error has occurred while generating the language file '%s'"
      enhanceLastError(pOperationData->pErrorQueue, ERROR_ID(IDS_EXCEPTION_SAVE_SCRIPT_FILE), identifyGameFileFilename(pLanguageFile));
      eResult = OR_FAILURE;
   }

   //// [ERROR] Enhance the current error message
   //if (hasErrors(pOperationData->pErrorQueue))
   //   enhanceLastError(pOperationData->pErrorQueue, ERROR_ID(IDS_LANGUAGE_SAVE_ERROR)); 

   // [DEBUG] Separate previous output from further output for claritfy
   VERBOSE_THREAD_COMPLETE("LANGUAGE GENERATION WORKER THREAD COMPLETED");
   
   // Cleanup and return result
   closeThreadOperation(pOperationData, eResult);
   END_TRACKING();
   return THREAD_RETURN;
}



