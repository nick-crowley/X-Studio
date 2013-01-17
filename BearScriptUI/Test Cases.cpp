//
// Test Cases.cpp : Game Data Test cases
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"
#ifdef _TESTING

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                              TYPES
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// Contains the data necessary to search for a message within an ErrorQueue and hold it's result
//
struct MESSAGE_SEARCH
{
   UINT  iMessage;      // ID of the message being searched for
   BOOL  bFound;        // Whether the message was found or not
};

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       CONSTANTS / GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// Operation result strings
CONST TCHAR*  szOperationResults[3] = 
{
   TEXT("SUCCESS"), 
   TEXT("FAILURE"), 
   TEXT("ABORTED") 
};  

// TestCase properties
CONST UINT   iFirstGameDataTestCase          = TC_MISSING_LANGUAGE_FILE,     // First GameData TestCase
             iLastGameDataTestCase           = TC_TFILE_UNKNOWN_RACE_ID,     // Last GameData TestCase
             iFirstScriptTranslationTestCase = TC_MISSING_SCRIPT_TAG,        // First Script Translation TestCase
             iLastScriptTranslationTestCase  = TC_GAME_STRING_REFERENCE_NOT_FOUND; // Last Script Translation TestCase
             

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                              HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Configuration  : TESTING
/// Function name  : checkCommandTranslation
// Description     : Check whether the translation of a command matches a specified string
// 
// CONST LIST*   pCommandList   : [in] List of translated COMMAND objects found in a ScriptFile
// CONST UINT    iIndex         : [in] Index of the COMMAND to verify
// CONST TCHAR*  szTranslation  : [in] String to verify the COMMAND translation against
// 
// Return Value   : TRUE if successfully verified, FALSE otherwise
// 
BOOL  checkCommandTranslation(CONST LIST* pCommandList, CONST UINT  iIndex, CONST TCHAR*  szTranslation)
{
   LIST_ITEM*  pListItem;  // ListItem wrapper
   COMMAND*    pCommand;   // Translated COMMAND to check
   BOOL        bResult;    // Verification result

   // Prepare
   bResult = FALSE;

   /// Lookup COMMAND
   if (findListItemByIndex(pCommandList, iIndex, pListItem))
   {
      // Extract COMMAND
      pCommand = (COMMAND*)pListItem->oData;

      /// Check translation
      bResult = (StrCmpI(szTranslation, pCommand->szBuffer) == 0);

      // [FAILED] Print out desired and actual translation
      if (!bResult)
         CONSOLE("ERROR: Command '%s' incorrectly translated as '%s'", szTranslation, pCommand->szBuffer);
   }
   // [ERROR] Command not translated
   else
      CONSOLE("ERROR: Command '%s' (index %u) was not present", szTranslation, iIndex);
   
   /// Return result
   return bResult;
}


/// Configuration  : TESTING
/// Function name  : checkErrorQueueMessages
// Description     : Checks an ErrorQueue for the specified Message IDs and prints which errors are missing (if any)
//                      to the debugging console
// 
// CONST ERROR_QUEUE*  pErrorQueue   : [in] ErrorQueue to search
// CONST UINT          iTestCaseID   : [in] TestCase number
// CONST UINT          iMessageCount : [in] Number of message IDs to search for
//     ...             ...           : [in] Message IDs to search for
// 
// Return Value   : TRUE if all messages were found, FALSE otherwise
// 
BOOL  checkErrorQueueMessages(CONST ERROR_QUEUE*  pErrorQueue, CONST UINT  iTestCaseID, CONST UINT  iMessageCount, ...)
{
   MESSAGE_SEARCH  oMessages[10];   // Message IDs to be searched for and their results
   va_list         pArgument;       // Variable argument iterator
   BOOL            bResult;         // Operation result

   // Prepare
   pArgument = utilGetFirstVariableArgument(&iMessageCount);
   utilZeroObjectArray(oMessages, MESSAGE_SEARCH, 10);
   bResult = TRUE;

   /// Search the ErrorQueue for each of the MessageIDs specified in the variable arguments
   for (UINT i = 0; i < iMessageCount; i++)
   {
      // Retrieve message ID
      oMessages[i].iMessage = utilGetCurrentVariableArgument(pArgument, UINT);
      pArgument = utilGetNextVariableArgument(pArgument, UINT);

      // Search error queue
      oMessages[i].bFound = hasErrorQueueMessage(pErrorQueue, oMessages[i].iMessage);
   }

   /// [CHECK] Were all messages found?
   for (UINT i = 0; i < iMessageCount; i++)
   {
      // [NOT FOUND] Print to console and change result to FALSE
      if (oMessages[i].bFound == FALSE)
      {
         CONSOLE("WARNING: TC.%03u : The specified ErrorQueue does not contain Message index #%u (ID:%03u)", iTestCaseID, i + 1, oMessages[i].iMessage);
         bResult = FALSE;
      }
   }

   // [CHECK] Where the correct number of messages present?
   if (iMessageCount != getErrorQueueCount(pErrorQueue))
   {
      CONSOLE("WARNING: TC.%03u : The ErrorQueue has %u messages when %u were expected", iTestCaseID, getErrorQueueCount(pErrorQueue), iMessageCount);
      bResult = FALSE;
   }

   // [FAILED] Display error message
   if (!bResult AND hasErrors(pErrorQueue))
      displayErrorQueueDialog(NULL, pErrorQueue, NULL);

   // Return result
   return bResult;
}


/// Configuration  : TESTING
/// Function name  : generateTestCaseFolder
// Description     : Generate the path of the folder containing the data necessary to run the specified TestCase
// 
// CONST UINT  iTestCaseID : [in] TestCase number
// 
// Return Value   : New string containing the full path of the test case folder  (with a trailing backslash)
// 
TCHAR*  generateTestCaseFolder(CONST UINT  iTestCaseID)
{
   TCHAR*  szOutput;    // Output path

   // Create empty string
   szOutput = utilCreateEmptyPath();
   // Generate appropriate folder path
   StringCchPrintf(szOutput, MAX_PATH, TEXT("C:\\My Projects\\VS9\\BearScript\\TestData\\TC.%03u\\"), iTestCaseID);
   return szOutput;
}


/// Configuration  : TESTING
/// Function name  : hasErrorStackMessage
// Description     : Determine whether an ErrorStack contains a specific message
// 
// CONST ERROR_STACK*  pError      : [in] ErrorStack to check
// CONST UINT          iMessageID  : [in] ID of the message to check for
// 
// Return Value   : TRUE if stack contains the message, FALSE otherwise
// 
BOOL  hasErrorStackMessage(CONST ERROR_STACK*  pError, CONST UINT  iMessageID)
{
   /// Iterate through stack messages
   for (UINT i = 0; i < pError->iCount; i++)
      if (getErrorStackItem(pError, i)->iID == iMessageID)
         // [FOUND] Return TRUE
         return TRUE;

   // [NOT FOUND] Return FALSE
   return FALSE;
}


/// Configuration  : TESTING
/// Function name  : hasErrorQueueMessage
// Description     : Determine whether an ErrorQueue contains an ErrorStack containing a specific message
// 
// CONST ERROR_QUEUE*  pErrorQueue : [in] ErrorQueue to search
// CONST UINT          iMessageID  : [in] ID of the message to search for
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BOOL  hasErrorQueueMessage(CONST ERROR_QUEUE*  pErrorQueue, CONST UINT  iMessageID)
{
   /// Iterate through queue items
   for (UINT i = 0; i < getErrorQueueCount(pErrorQueue); i++)
      if (hasErrorStackMessage(getErrorQueueItem(pErrorQueue, i), iMessageID))
         // [FOUND] Return TRUE
         return TRUE;

   // [NOT FOUND] Return FALSE
   return FALSE;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                            FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Configuration  : TESTING
/// Function name  : findNextTestCaseID
// Description     : Determine the next TestCase ID from an existing TestCase ID
// 
// CONST UINT  iTestCaseID   : [in]  Existing TestCase ID
// UINT       &iOutput       : [out] Next TestCase ID
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BOOL  findNextTestCaseID(CONST UINT  iTestCaseID, UINT&  iOutput)
{
   // [CHECK] Ensure TestCase is valid
   ASSERT(iTestCaseID > iFirstGameDataTestCase);

   // Prepare
   iOutput = NULL;

   // [GAME DATA] Return next GameData test case
   if (iTestCaseID < iLastGameDataTestCase)
      iOutput = (iTestCaseID + 1);

   // [LAST GAME DATA] Return first Script-Translation
   else if (iTestCaseID == iLastGameDataTestCase)
      iOutput = iFirstScriptTranslationTestCase;

   // [SCRIPT TRANSLATION] Return next Script Translation test case
   else if (iTestCaseID < iLastScriptTranslationTestCase)
      iOutput = (iTestCaseID + 1);

   // Return TRUE if subsequent ID exists
   return (iOutput != NULL);
}


/// Configuration  : TESTING
/// Function name  : identifyTestCaseType
// Description     : Determine the category of a TestCase from it's ID
// 
// CONST UINT  iTestCaseID : [in] TestCase ID to identify
// 
// Return Value   : TestCase category if successful, otherwise TCT_UNKNOWN (NULL)
// 
TEST_CASE_TYPE  identifyTestCaseType(CONST UINT  iTestCaseID)
{
   // [CHECK] Ensure test case isn't NULL
   ASSERT(iTestCaseID > iFirstGameDataTestCase);

   // [GAME DATA] 
   if (iTestCaseID >= iFirstGameDataTestCase AND iTestCaseID <= iLastGameDataTestCase)
      return TCT_GAME_DATA;

   // [SCRIPT TRANSLATION]
   if (iTestCaseID >= iFirstScriptTranslationTestCase AND iTestCaseID <= iLastScriptTranslationTestCase)
      return TCT_SCRIPT_TRANSLATION;

   // [ERROR] Unknown TestCase type
   ASSERT(FALSE);
   return TCT_UNKNOWN;
}


/// Configuration  : TESTING
/// Function name  : runGameDataTestCase
// Description     : Initiate a GameData TestCase. This will complete asynchronously
// 
// MAIN_WINDOW_DATA*  pMainWindowData : [in] Main window data
// CONST UINT        iTestCaseID     : [in] Number of the TestCase to run
// 
VOID  runGameDataTestCase(MAIN_WINDOW_DATA*  pMainWindowData, CONST UINT  iTestCaseID)
{
   TCHAR*  szFolderPath;

   // [CHECK] Ensure TestCase ID is valid
   ASSERT(identifyTestCaseType(iTestCaseID) == TCT_GAME_DATA);

   // Print title
   CONSOLE("Beginning Game Data Test Case %03u...", iTestCaseID);
   CONSOLE("-------------------------------------");

   // Change GameData folder to the TestCase data folder
   szFolderPath = generateTestCaseFolder(iTestCaseID);
   setAppPreferencesGameDataFolder(szFolderPath);

   // Unload existing data and attempt to load TestCase data
   performMainWindowReInitialisation(pMainWindowData, iTestCaseID);

   // Cleanup
   utilDeleteString(szFolderPath);
}


/// Configuration  : TESTING
/// Function name  : runScriptTranslationTestCase
// Description     : Initiate a MSCI Script Translation TestCase. This will complete asynchronously
// 
// MAIN_WINDOW_DATA*  pMainWindowData : [in] Main window data
// CONST UINT        iTestCaseID     : [in] ID of the TestCase to run
// 
VOID  runScriptTranslationTestCase(MAIN_WINDOW_DATA*  pMainWindowData, CONST UINT  iTestCaseID)
{
   OPERATION_DATA*  pOperationData;  // Translation operation data
   TCHAR*        szScriptPath;    // Full path of the TestCase script

   // [CHECK] Ensure TestCase ID is valid
   ASSERT(identifyTestCaseType(iTestCaseID) == TCT_SCRIPT_TRANSLATION);

   // Print TestCase title
   CONSOLE("Beginning Script Translation Test Case %03u...", iTestCaseID);
   CONSOLE("----------------------------------------------");

   // Generate Script path
   szScriptPath = generateTestCaseFolder(iTestCaseID);
   StringCchCat(szScriptPath, MAX_PATH, TEXT("TestScript.xml"));

   // Launch script translation thread
   printOperationNameToOutputDialog(pMainWindowData->hOutputDlg, OT_LOAD_SCRIPT_FILE, szScriptPath);
   pOperationData = commandLoadScript(pMainWindowData->hMainWnd, szScriptPath, iTestCaseID);
   createProgressDialog(pMainWindowData, pOperationData);

   // Cleanup
   utilDeleteString(szScriptPath);
}


/// Configuration  : TESTING
/// Function name  : verifyGameDataTestCase
// Description     : Verify the results of a GameData test case
// 
// CONST UINT          iTestCaseID    : [in] TestCase number
// CONST OPERATION_DATA*  pOperationData : [in] TestCase operation data
// 
// Return Value   : TRUE if validated succesfully, FALSE otherwise
// 
BOOL  verifyGameDataTestCase(CONST UINT  iTestCaseID, CONST OPERATION_DATA*  pOperationData)
{
   OPERATION_RESULT  eDesiredResult;   // Correct result for the current TestCase
   ERROR_QUEUE*      pErrorQueue;      // Convenience pointer for the thread error queue
   BOOL              bVerified;        // Verifcation result, defaults to FALSE

   // Prepare
   pErrorQueue      = pOperationData->pErrorQueue;
   bVerified        = FALSE;
   
   /// Verify ErrorQueue and Operation Result
   switch (iTestCaseID)
   {
   /// [001: MISSING LANGUAGE FILE] - Check for 'No Main Language File' (+ 'No Catalogues')
   //  [CRITICAL ERROR]
   case TC_MISSING_LANGUAGE_FILE:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 2, IDS_MAIN_LANGUAGE_FILE_NOT_FOUND, IDS_VFS_CATALOGUES_NOT_FOUND);
      eDesiredResult = OR_FAILURE;
      break;

   /// [002: MISSING SPEECH DEFINITION FILE] - Check for 'No Speech File' (+ 'No Catalogues')
   //  [CRITICAL ERROR]
   case TC_MISSING_SPEECH_FILE:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 2, IDS_MEDIA_SPEECH_FILE_IO_ERROR, IDS_VFS_CATALOGUES_NOT_FOUND);
      eDesiredResult = OR_FAILURE;
      break;

   /// [003: MISSING SFX DEFINITION FILE] - Check for 'No SFX File' (+ 'No Catalogues')
   //  [CRITICAL ERROR]
   case TC_MISSING_SOUND_EFFECT_FILE:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 2, IDS_MEDIA_SFX_FILE_IO_ERROR, IDS_VFS_CATALOGUES_NOT_FOUND);
      eDesiredResult = OR_FAILURE;
      break;

   /// [004: MISSING VIDEO DEFINITION FILE] - Check for 'No Video File' (+ 'No Catalogues')
   //  [CRITICAL ERROR]
   case TC_MISSING_VIDEO_FILE:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 2, IDS_MEDIA_VIDEO_FILE_IO_ERROR, IDS_VFS_CATALOGUES_NOT_FOUND);
      eDesiredResult = OR_FAILURE;
      break;

   /// [005-016: MISSING TYPE FILE] - Check for 'TFile not found' (+ 'No Catalogues')
   //  [CRITICAL ERROR]
   case TC_MISSING_DOCKS_FILE:
   case TC_MISSING_FACTORIES_FILE:
   case TC_MISSING_LASERS_FILE:
   case TC_MISSING_MISSILES_FILE:
   case TC_MISSING_SHIELDS_FILE:
   case TC_MISSING_SHIPS_FILE:
   case TC_MISSING_WARE_B_FILE:
   case TC_MISSING_WARE_E_FILE:
   case TC_MISSING_WARE_F_FILE:
   case TC_MISSING_WARE_M_FILE:
   case TC_MISSING_WARE_N_FILE:
   case TC_MISSING_WARE_T_FILE:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 2, IDS_TFILE_IO_ERROR, IDS_VFS_CATALOGUES_NOT_FOUND);
      eDesiredResult = OR_FAILURE;
      break;

   /// [017: LANGUAGE-FILE: BLANK or NON-XML] - This is not classified as a valid LanguageFile by the FileSearch therefore it's considered 'not found'
   //  [CRITICAL ERROR]
   case TC_LANGUAGE_BLANK:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 2, IDS_MAIN_LANGUAGE_FILE_NOT_FOUND, IDS_VFS_CATALOGUES_NOT_FOUND);
      eDesiredResult = OR_FAILURE;
      break;

   /// [018: LANGUAGE-FILE: UNSUPPORTED SCHEMA-TAG] - Check for 'Unexpected schema tag' (+ 'No Catalogues')
   //  [MINOR ERROR]
   case TC_LANGUAGE_UNSUPPORTED_SCHEMA_TAG:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 2, IDS_XML_UNEXPECTED_SCHEMA_TOKEN, IDS_VFS_CATALOGUES_NOT_FOUND);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [019: LANGUAGE-FILE: UNSUPPORTED TAG] - Check for 'Unexpected tag' (+ 'No Catalogues')
   //  [MINOR ERROR]
   case TC_LANGUAGE_UNSUPPORTED_TAG:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 2, IDS_XML_UNEXPECTED_TOKEN, IDS_VFS_CATALOGUES_NOT_FOUND);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [020: LANGUAGE-FILE: UNCLOSED OPENING STRING TAG] - Check for 'No Catalogues'
   //  [MINOR ERROR]
   case TC_LANGUAGE_UNCLOSED_STRING_TAG:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 1, IDS_VFS_CATALOGUES_NOT_FOUND);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [021: LANGUAGE-FILE: UNOPENED STRING TAG] - Check for '3 x Unexpected token' (+ 'No Catalogues')
   //  [MINOR ERROR]
   case TC_LANGUAGE_UNOPENED_STRING_TAG:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 4, IDS_XML_UNEXPECTED_TOKEN, IDS_XML_UNEXPECTED_TOKEN, IDS_XML_UNEXPECTED_TOKEN, IDS_VFS_CATALOGUES_NOT_FOUND);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [022: LANGUAGE-FILE: STRING TAG WITHOUT ID PROPERTY] - Check for 'Missing ID property' + '2x Unexpected token' (for the string and closing tag) (+ 'No Catalogues')
   //  [MINOR ERROR]
   case TC_LANGUAGE_MISSING_STRING_TAG_ID:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 4, IDS_XML_ID_PROPERTY_MISSING, IDS_XML_UNEXPECTED_TOKEN, IDS_XML_UNEXPECTED_TOKEN, IDS_VFS_CATALOGUES_NOT_FOUND);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [023: LANGUAGE-FILE: UNSUPPORTED TAG PROPERTY] - Check for 'Unsupported property' + 'Unexpected token' (From the closing tag) (+ 'No Catalogues')
   //  [MINOR ERROR]
   case TC_LANGUAGE_UNSUPPORTED_TAG_PROPERTY:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 3, IDS_XML_TAG_UNKNOWN_PROPERTY, IDS_XML_UNEXPECTED_TOKEN, IDS_VFS_CATALOGUES_NOT_FOUND);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [024: LANGUAGE-FILE: MISSING SCHEMA TAG] - Check for 'No Catalogues'
   //  [MINOR ERROR]
   case TC_LANGUAGE_MISSING_SCHEMA_TAG:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 1, IDS_VFS_CATALOGUES_NOT_FOUND);
      eDesiredResult = OR_SUCCESS;
      break;
   
   /// [025: LANGUAGE-FILE: MISSING LANGUAGE TAG] - Check for 'no language file' (+ 'No Catalogues')
   //  [CRITICAL ERROR]
   case TC_LANGUAGE_MISSING_LANGUAGE_TAG:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 2, IDS_MAIN_LANGUAGE_FILE_NOT_FOUND, IDS_VFS_CATALOGUES_NOT_FOUND);
      eDesiredResult = OR_FAILURE;
      break;

   /// [026: LANGUAGE-FILE: UNSUPPORTED LANGUAGE ID] - Check for 'language mismatch' (+ 'No Catalogues')
   //  [MINOR ERROR]
   case TC_LANGUAGE_UNSUPPORTED_LANGUAGE_ID:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 2, IDS_XML_LANGUAGE_MISMATCH, IDS_VFS_CATALOGUES_NOT_FOUND);
      eDesiredResult = OR_SUCCESS;
      break;



   /// [027: T-FILE: OBJECT COUNT NOT PRESENT] - Check for 'object count not found' (+ no cats)
   //  [MINOR ERROR]
   case TC_TFILE_MISSING_OBJECT_COUNT:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 2, IDS_TFILE_OBJECT_COUNT_NOT_FOUND, IDS_VFS_CATALOGUES_NOT_FOUND);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [028: T-FILE: OBJECT COUNT INCORRECT] - Check for 'object count incorrect' (+ no cats)
   //  [MINOR ERROR]
   case TC_TFILE_INCORRECT_OBJECT_COUNT:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 2, IDS_TFILE_OBJECT_COUNT_INCORRECT, IDS_VFS_CATALOGUES_NOT_FOUND);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [029: T-FILE: EMPTY LINE] - Check for 'insufficient properties'+'object count mismatch' (For the blank line)
   //  [MINOR ERROR]
   case TC_TFILE_EMPTY_LINE:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 3, IDS_TFILE_INSUFFICIENT_PROPERTIES, IDS_TFILE_OBJECT_COUNT_INCORRECT, IDS_VFS_CATALOGUES_NOT_FOUND);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [030: T-FILE: UNKNOWN OBJECT NAME] - Check for 'name not found'
   //  [MINOR ERROR]
   case TC_TFILE_UNKNOWN_NAME_ID:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 2, IDS_TFILE_OBJECT_NAME_NOT_FOUND, IDS_VFS_CATALOGUES_NOT_FOUND);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [031: T-FILE: UNKNOWN OBJECT RACE] - Check for 'race not found' 
   //  [MINOR ERROR]
   case TC_TFILE_UNKNOWN_RACE_ID:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 2, IDS_TFILE_OBJECT_RACE_NOT_FOUND, IDS_VFS_CATALOGUES_NOT_FOUND);
      eDesiredResult = OR_SUCCESS;
      break;
   }

   // [CHECK] Operation result
   if (eDesiredResult != pOperationData->eResult)
   {
      CONSOLE("WARNING: TC.%03u Operation Result is '%s' when it should be '%s'", iTestCaseID, szOperationResults[pOperationData->eResult], szOperationResults[eDesiredResult]);
      bVerified = FALSE;
   }

   // Return result
   return bVerified;
}


/// Configuration  : TESTING
/// Function name  : verifyScriptTranslationTestCase
// Description     : Verify the results of a Script Translation test case
// 
// CONST UINT          iTestCaseID    : [in] TestCase ID
// CONST OPERATION_DATA*  pOperationData : [in] Operation data for the test case
// 
// Return Value   : TRUE if validated succesfully, FALSE otherwise
// 
BOOL  verifyScriptTranslationTestCase(CONST UINT  iTestCaseID, CONST OPERATION_DATA*  pOperationData)
{
   OPERATION_RESULT  eDesiredResult;   // Correct result for the current TestCase
   ERROR_QUEUE*      pErrorQueue;      // Convenience pointer for the thread error queue
   BOOL              bVerified;        // Verifcation result, defaults to FALSE

   // Prepare
   pErrorQueue = pOperationData->pErrorQueue;
   bVerified   = FALSE;
   
   /// Verify ErrorQueue and Operation Result
   switch (iTestCaseID)
   {
   /// [050: MISSING SCRIPT TAG] - Check for 'Missing <Script> Tag' (+ 'Unexpected closing tag')
   //  [CRITICAL ERROR]
   case TC_MISSING_SCRIPT_TAG:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 2, IDS_SCRIPT_TAG_MISSING, IDS_XML_UNEXPECTED_CLOSING_TAG);
      eDesiredResult = OR_FAILURE;
      break;

   /// [051: MISSING CODEARRAY TAG] - Check for 'Missing <codearray> Tag' (+ 'Unexpected closing tag')
   //  [CRITICAL ERROR]
   case TC_MISSING_CODEARRAY_TAG:
#ifdef _DEBUG
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 2, IDS_CODEARRAY_TAG_MISSING, IDS_XML_MISMATCHED_CLOSING_TAG);
#else
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 3, IDS_CODEARRAY_TAG_MISSING, IDS_XML_UNEXPECTED_CLOSING_BRACKET, IDS_XML_MISMATCHED_CLOSING_TAG);
#endif
      eDesiredResult = OR_FAILURE;
      break;

   /// [052-056: MISSING SCRIPT PROPERTY TAG] - Check for 'Missing Property Tag' (+ 'Unexpected closing tag')
   //  [CRITICAL ERROR]
   case TC_MISSING_NAME_TAG:
   case TC_MISSING_DESCRIPTION_TAG:
   case TC_MISSING_ENGINEVERSION_TAG:
   case TC_MISSING_VERSION_TAG:
   case TC_MISSING_ARGUMENTS_TAG:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 1, IDS_SCRIPT_PROPERTY_TAG_MISSING);
      eDesiredResult = OR_FAILURE;
      break;

   /// [057: MISSING SOURCETEXT TAG] - Check for 'Missing Property Tag' (+ 'Unexpected closing tag')
   //  [NO ERROR]
   case TC_MISSING_SOURCETEXT_TAG:
      bVerified = !hasErrors(pErrorQueue);
      eDesiredResult = OR_SUCCESS;
      break;


   /// [058: INCORRECT CODEARRAY SIZE] - Check for 'Invalid CodeArray' 
   //  [CRITICAL ERROR]
   case TC_CODEARRAY_INCORRECT_SIZE:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 1, IDS_CODEARRAY_INVALID);
      eDesiredResult = OR_FAILURE;
      break;

   /// [059: INVALID STANDARD BRANCH] - Check for 'Invalid Standard Branch' 
   //  [CRITICAL ERROR]
   case TC_CODEARRAY_INVALID_STANDARD_BRANCH:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 1, IDS_CODEARRAY_STANDARD_BRANCH_INVALID);
      eDesiredResult = OR_FAILURE;
      break;

   /// [060: EMPTY STANDARD BRANCH] - Check for 'Empty Standard Branch' 
   //  [CRITICAL ERROR]
   case TC_CODEARRAY_EMPTY_STANDARD_BRANCH:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 1, IDS_CODEARRAY_STANDARD_BRANCH_EMPTY);
      eDesiredResult = OR_FAILURE;
      break;

   /// [061: INVALID AUXILIARY BRANCH] - Check for 'Invalid Auxiliary Branch' 
   //  [CRITICAL ERROR]
   case TC_CODEARRAY_INVALID_AUXILIARY_BRANCH:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 1, IDS_CODEARRAY_AUXILIARY_BRANCH_INVALID);
      eDesiredResult = OR_FAILURE;
      break;

   /// [062: INVALID VARIABLES BRANCH] - Check for 'Invalid Variables Branch' 
   //  [CRITICAL ERROR]
   case TC_CODEARRAY_INVALID_VARIABLES_BRANCH:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 1, IDS_CODEARRAY_VARIABLES_BRANCH_INVALID);
      eDesiredResult = OR_FAILURE;
      break;

   /// [063: MISSING COMMAND ID] - Check for 'Missing Command ID' 
   //  [CRITICAL ERROR]
   case TC_CODEARRAY_MISSING_COMMAND_ID:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 1, IDS_CODEARRAY_COMMAND_ID_MISSING);
      eDesiredResult = OR_FAILURE;
      break;

   /// [065: MISSING SCRIPT DESCRIPTION] - No Error
   //  [NO ERROR]
   case TC_MISSING_SCRIPT_DESCRIPTION:
      bVerified = !hasErrors(pErrorQueue);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [064, 066, 067: MISSING SCRIPT NAME/VERSION/ENGINEVERSION] - Check for 'Missing Script property' 
   //  [CRITICAL ERROR]
   case TC_MISSING_SCRIPT_NAME:
   case TC_MISSING_SCRIPT_VERSION:
   case TC_MISSING_SCRIPT_ENGINEVERSION:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 1, IDS_SCRIPT_PROPERTY_TAG_MISSING);
      eDesiredResult = OR_FAILURE;
      break;

   /// [068: ASSIGNMENT] - Ensure result is '$a = $b'
   //  [NO ERROR] 
   case TC_EXPRESSION_ASSIGNMENT:
      bVerified = checkCommandTranslation(pOperationData->pScriptFile->pCommandList, 0, TEXT("$a = $b"))
                  AND !hasErrors(pErrorQueue);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [069: SIMPLE SMALL EXPRESSION] - Ensure result is '$a = $b + $c'
   //  [NO ERROR] 
   case TC_EXPRESSION_SIMPLE_SMALL:
      bVerified = checkCommandTranslation(pOperationData->pScriptFile->pCommandList, 0, TEXT("$a = $b + $c"))
                  AND !hasErrors(pErrorQueue);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [070: SIMPLE LARGE EXPRESSION] - Ensure result is '$a = $b + $c - $d / $e * $f AND ($g OR ! $h)'
   //  [NO ERROR] 
   case TC_EXPRESSION_SIMPLE_LARGE:
      bVerified = checkCommandTranslation(pOperationData->pScriptFile->pCommandList, 0, TEXT("$a = $b + $c - $d / $e * $f AND ($g OR !$h)"))
                  AND !hasErrors(pErrorQueue);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [071: SIMPLE HUGE EXPRESSION] - Ensure result is '$a = $b + $c - $d / $e * $f AND ($g OR !$h) + $c * $d - ($e * $f + ($g - $h)) OR $b AND $c'
   //  [NO ERROR] 
   case TC_EXPRESSION_SIMPLE_HUGE:
      bVerified = checkCommandTranslation(pOperationData->pScriptFile->pCommandList, 0, TEXT("$a = $b + $c - $d / $e * $f AND ($g OR !$h) + $c * $d - ($e * $f + ($g - $h)) OR $b AND $c"))
                  AND !hasErrors(pErrorQueue);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [072: COMPLEX SMALL EXPRESSION] - Ensure result is '$a = $b AND ($c == {1 GJ Shield})'
   //  [NO ERROR] 
   case TC_EXPRESSION_COMPLEX_SMALL:
      bVerified = checkCommandTranslation(pOperationData->pScriptFile->pCommandList, 0, TEXT("$a = $b AND ($c == {1 GJ Shield})"))
                  AND !hasErrors(pErrorQueue);
      eDesiredResult = OR_SUCCESS;
      break;
     
   /// [073: COMPLEX LARGE EXPRESSION] - Ensure result is '$a = $b AND ($c == {1 GJ Shield}) OR (5 >= 8) AND 'Hello' == {Banshee Missile}'
   //  [NO ERROR] 
   case TC_EXPRESSION_COMPLEX_LARGE:
      bVerified = checkCommandTranslation(pOperationData->pScriptFile->pCommandList, 0, TEXT("$a = $b AND ($c == {1 GJ Shield}) OR (5 >= 8) AND 'Hello' == {Banshee Missile}"))
                  AND !hasErrors(pErrorQueue);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [074: 'IF' CONDITIONAL] - Ensure result is 'if $a == $b'
   //  [NO ERROR] 
   case TC_CONDITIONAL_IF:
      bVerified = checkCommandTranslation(pOperationData->pScriptFile->pCommandList, 0, TEXT("if $a == $b"))
                  AND !hasErrors(pErrorQueue);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [075: 'IF NOT' CONDITIONAL] - Ensure result is 'if not $a == $b'
   //  [NO ERROR] 
   case TC_CONDITIONAL_IF_NOT:
      bVerified = checkCommandTranslation(pOperationData->pScriptFile->pCommandList, 0, TEXT("if not $a == $b"))
                  AND !hasErrors(pErrorQueue);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [076: 'SKIP IF' CONDITIONAL] - Ensure result is 'skip if $a == $b'
   //  [NO ERROR] 
   case TC_CONDITIONAL_SKIP_IF:
      bVerified = checkCommandTranslation(pOperationData->pScriptFile->pCommandList, 0, TEXT("skip if $a == $b"))
                  AND !hasErrors(pErrorQueue);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [077: 'SKIP IF NOT' CONDITIONAL] - Ensure result is 'skip if not $a == $b'
   //  [NO ERROR] 
   case TC_CONDITIONAL_SKIP_IF_NOT:
      bVerified = checkCommandTranslation(pOperationData->pScriptFile->pCommandList, 0, TEXT("skip if not $a == $b"))
                  AND !hasErrors(pErrorQueue);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [078: 'WHILE' CONDITIONAL] - Ensure result is 'while $a == $b'
   //  [NO ERROR] 
   case TC_CONDITIONAL_WHILE:
      bVerified = checkCommandTranslation(pOperationData->pScriptFile->pCommandList, 0, TEXT("while $a == $b"))
                  AND !hasErrors(pErrorQueue);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [079: 'WHILE NOT' CONDITIONAL] - Ensure result is 'while not $a == $b'
   //  [NO ERROR] 
   case TC_CONDITIONAL_WHILE_NOT:
      bVerified = checkCommandTranslation(pOperationData->pScriptFile->pCommandList, 0, TEXT("while not $a == $b"))
                  AND !hasErrors(pErrorQueue);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [080: 'ELSE' CONDITIONAL] - Ensure result is 'else'
   //  [NO ERROR] 
   case TC_CONDITIONAL_ELSE:
      bVerified = checkCommandTranslation(pOperationData->pScriptFile->pCommandList, 1, TEXT("else"))
                  AND !hasErrors(pErrorQueue);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [081: 'ELSE IF' CONDITIONAL] - Ensure result is 'else if $a == $b'
   //  [NO ERROR] 
   case TC_CONDITIONAL_ELSE_IF:
      bVerified = checkCommandTranslation(pOperationData->pScriptFile->pCommandList, 1, TEXT("else if $c == $d"))
                  AND !hasErrors(pErrorQueue);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [082: 'ELSE IF NOT' CONDITIONAL] - Ensure result is 'else if not $a == $b'
   //  [NO ERROR] 
   case TC_CONDITIONAL_ELSE_IF_NOT:
      bVerified = checkCommandTranslation(pOperationData->pScriptFile->pCommandList, 1, TEXT("else if not $c == $d"))
                  AND !hasErrors(pErrorQueue);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [083: COMMENT COMMAND] - Ensure result is '* Example script'
   //  [NO ERROR] 
   case TC_COMMAND_COMMENT:
      bVerified = checkCommandTranslation(pOperationData->pScriptFile->pCommandList, 0, TEXT("* Example script"))
                  AND !hasErrors(pErrorQueue);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [084: DEFINE LABEL] - Ensure result is 'start:'
   //  [NO ERROR] 
   case TC_COMMAND_LABEL:
      bVerified = checkCommandTranslation(pOperationData->pScriptFile->pCommandList, 1, TEXT("start:"))
                  AND !hasErrors(pErrorQueue);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [085: INCREMENT] - Ensure result is 'inc $a'
   //  [NO ERROR] 
   case TC_COMMAND_INCREMENT:
      bVerified = checkCommandTranslation(pOperationData->pScriptFile->pCommandList, 3, TEXT("inc $a"))
                  AND !hasErrors(pErrorQueue);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [086: DECREMENT] - Ensure result is 'dec $a'
   //  [NO ERROR] 
   case TC_COMMAND_DECREMENT:
      bVerified = checkCommandTranslation(pOperationData->pScriptFile->pCommandList, 4, TEXT("dec $a"))
                  AND !hasErrors(pErrorQueue);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [087: WAIT] - Ensure result is '= wait 40 ms'
   //  [NO ERROR] 
   case TC_COMMAND_WAIT:
      bVerified = checkCommandTranslation(pOperationData->pScriptFile->pCommandList, 5, TEXT("= wait 40 ms"))
                  AND !hasErrors(pErrorQueue);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [088: GOTO LABEL] - Ensure result is 'goto label start:'
   //  [NO ERROR] 
   case TC_COMMAND_GOTO_LABEL:
      bVerified = checkCommandTranslation(pOperationData->pScriptFile->pCommandList, 6, TEXT("goto label start:"))
                  AND !hasErrors(pErrorQueue);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [089: GOTO SUBROUTINE] - Ensure result is 'gosub start:'
   //  [NO ERROR] 
   case TC_COMMAND_GOTO_SUBROUTINE:
      bVerified = checkCommandTranslation(pOperationData->pScriptFile->pCommandList, 7, TEXT("gosub start:"))
                  AND !hasErrors(pErrorQueue);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [090: BREAK] - Ensure result is 'break'
   //  [NO ERROR] 
   case TC_COMMAND_BREAK:
      bVerified = checkCommandTranslation(pOperationData->pScriptFile->pCommandList, 8, TEXT("break"))
                  AND !hasErrors(pErrorQueue);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [091: CONTINUE] - Ensure result is 'continue'
   //  [NO ERROR] 
   case TC_COMMAND_CONTINUE:
      bVerified = checkCommandTranslation(pOperationData->pScriptFile->pCommandList, 9, TEXT("continue"))
                  AND !hasErrors(pErrorQueue);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [092: END] - Ensure result is 'end'
   //  [NO ERROR] 
   case TC_COMMAND_END:
      bVerified = checkCommandTranslation(pOperationData->pScriptFile->pCommandList, 10, TEXT("end"))
                  AND !hasErrors(pErrorQueue);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [093: RETURN] - Ensure result is 'return'
   //  [NO ERROR] 
   case TC_COMMAND_RETURN:
      bVerified = checkCommandTranslation(pOperationData->pScriptFile->pCommandList, 11, TEXT("return null"))
                  AND !hasErrors(pErrorQueue);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [094: COMMAND NODE COUNT MISMATCH] - Check for 'SourceValue count mismatch' 
   //  [MINOR ERROR]
   case TC_COMMAND_NODE_COUNT_MISMATCH:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 1, IDS_SCRIPT_SOURCEVALUE_SIZE_MISMATCH);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [095: INCORRECT COMMAND NODE COUNT] - Check for 'Command node count incorrect'
   //  [MINOR ERROR]
   case TC_COMMAND_NODE_COUNT_INCORRECT:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 1, IDS_SCRIPT_COMMAND_NODE_COUNT_MISMATCH);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [096: INVALID PARAMETER TYPE] - Check for 'Invalid parameter type'
   //  [MINOR ERROR]
   case TC_PARAMETER_INVALID_TYPE:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 1, IDS_SCRIPT_PARAMETER_TUPLE_TYPE_INVALID);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [097: INVALID CONDITIONAL] - Check for 'Invalid conditional'
   //  [MINOR ERROR]
   case TC_PARAMETER_INVALID_CONDITIONAL:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 1, IDS_RETURN_OBJECT_CONDITIONAL_INVALID);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [098: INVALID VARIABLE] - Check for 'Invalid variable ID'
   //  [MINOR ERROR]
   case TC_PARAMETER_INVALID_VARIABLE_ID:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 1, IDS_TRANSLATION_VARIABLE_INVALID);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [099: INVALID COMMAND] - Check for 'Command syntax not found'
   //  [MINOR ERROR]
   case TC_PARAMETER_INVALID_COMMAND_ID:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 1, IDS_SCRIPT_COMMAND_SYNTAX_MISSING);
      eDesiredResult = OR_SUCCESS; 
      break;   

   /// [100: INVALID LABEL NUMBER] - Check for 'Label number not found'
   //  [MINOR ERROR]
   case TC_PARAMETER_INVALID_LABEL_ID:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 1, IDS_SCRIPT_LABEL_NUMBER_NOT_FOUND);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [101: INVALID RETURN VARIABLE] - Check for 'Return variable invalid'
   //  [MINOR ERROR]
   case TC_PARAMETER_INVALID_RETURN_VARIABLE:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 1, IDS_RETURN_OBJECT_VARIABLE_INVALID);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [102-117: VALID PARAMETER TYPE] - Ensure no errors
   //  [NO ERROR]
   case TC_PARAMETER_VALID_CONSTANT:
   case TC_PARAMETER_VALID_DATATYPE:
   case TC_PARAMETER_VALID_FLIGHT_RETURN:
   case TC_PARAMETER_VALID_OBJECT_CLASS:
   case TC_PARAMETER_VALID_OBJECT_COMMAND:
   case TC_PARAMETER_VALID_OPERATOR:
   case TC_PARAMETER_VALID_SCRIPTDEF:
   case TC_PARAMETER_VALID_SECTOR:
   case TC_PARAMETER_VALID_SHIP:
   case TC_PARAMETER_VALID_STATION:
   case TC_PARAMETER_VALID_STATION_SERIAL:
   case TC_PARAMETER_VALID_TRANSPORT_CLASS:
   case TC_PARAMETER_VALID_RACE:
   case TC_PARAMETER_VALID_RELATION:
   case TC_PARAMETER_VALID_WARE:
   case TC_PARAMETER_VALID_WING_COMMAND:
      bVerified = !hasErrors(pErrorQueue);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [118,120-125,128-130,134: INVALID PARAMETER TYPE] - Check for 'Invalid parameter type'
   //  [MINOR ERROR]
   case TC_PARAMETER_INVALID_CONSTANT:
   case TC_PARAMETER_INVALID_FLIGHT_RETURN:
   case TC_PARAMETER_INVALID_OBJECT_CLASS:
   case TC_PARAMETER_INVALID_OBJECT_COMMAND:
   case TC_PARAMETER_INVALID_OPERATOR:
   case TC_PARAMETER_INVALID_SCRIPTDEF:
   case TC_PARAMETER_INVALID_SECTOR:
   case TC_PARAMETER_INVALID_STATION_SERIAL:
   case TC_PARAMETER_INVALID_TRANSPORT_CLASS:
   case TC_PARAMETER_INVALID_RACE:
   case TC_PARAMETER_INVALID_WING_COMMAND:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 1, IDS_TRANSLATION_PARAMETER_INVALID);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [119+131: INVALID DATATYPE] - Check for 'Invalid parameter type'
   //  [MINOR ERROR]
   case TC_PARAMETER_INVALID_DATATYPE:
   case TC_PARAMETER_INVALID_RELATION:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 1, IDS_TRANSLATION_TYPE_CONVERSION_FAILED);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [126+127,132+133: INVALID SHIP, STATION or WARE] - Check for 'Invalid ware'
   //  [MINOR ERROR]
   case TC_PARAMETER_INVALID_SHIP:
   case TC_PARAMETER_INVALID_STATION:
   case TC_PARAMETER_INVALID_WARE_MAINTYPE:
   case TC_PARAMETER_INVALID_WARE_SUBTYPE:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 1, IDS_TRANSLATION_WARE_INVALID);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [135+137: HUGE STRING or COMMENT] - Ensure no errors
   //  [NO ERROR]
   case TC_PARAMETER_STRING_HUGE:
   case TC_PARAMETER_COMMENT_HUGE:
      bVerified = !hasErrors(pErrorQueue);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [136,138,139: INVALID CHARACTERS in STRING or COMMENT or LABEL NAME] 
   //  [MINOR ERROR]
   case TC_PARAMETER_STRING_ILLEGAL_CHARS:
   case TC_PARAMETER_COMMENT_ILLEGAL_CHARS:
   case TC_LABEL_NAME_ILLEGAL_CHARS:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 3, IDS_SCRIPT_SOURCEVALUE_SIZE_MISMATCH, IDS_XML_UNEXPECTED_CLOSING_BRACKET, IDS_SCRIPT_SOURCEVALUE_SIZE_MISMATCH);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [140,141: INVALID CHARACTERS in a SCRIPT NAME or DESCRIPTION] 
   //  [MINOR ERROR]
   case TC_SCRIPT_NAME_ILLEGAL_CHARS:
   case TC_SCRIPT_DESCRIPTION_ILLEGAL_CHARS:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 5, IDS_XML_UNEXPECTED_OPENING_BRACKET, IDS_XML_UNEXPECTED_CLOSING_BRACKET, IDS_XML_MISMATCHED_CLOSING_TAG, IDS_XML_UNCLOSED_OPENING_TAG, IDS_XML_UNCLOSED_OPENING_TAG);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [142,143: INVALID CHARACTERS in an ARGUMENT NAME or DESCRIPTION] 
   //  [MINOR ERROR]
   case TC_ARGUMENT_NAME_ILLEGAL_CHARS:
   case TC_ARGUMENT_DESCRIPTION_ILLEGAL_CHARS:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 2, IDS_XML_UNEXPECTED_OPENING_BRACKET, IDS_XML_UNEXPECTED_CLOSING_BRACKET);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [144: INVALID CHARACTERS in a SOURCE TEXT LINE] 
   //  [MINOR ERROR]
   case TC_SOURCE_TEXT_ILLEGAL_CHARS:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 1, IDS_XML_UNEXPECTED_CLOSING_BRACKET);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [145: INVALID ARGUMENT INDEX] 
   //  [ERROR CANNOT BE DETECTED]
   case TC_ARGUMENT_INVALID_INDEX:
      bVerified = !hasErrors(pErrorQueue);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [146: INVALID ARGUMENT TYPE] 
   //  [CRITICAL ERROR]
   case TC_ARGUMENT_INVALID_TYPE:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 1, IDS_SCRIPT_ARGUMENT_TYPE_INVALID);
      eDesiredResult = OR_FAILURE;
      break;
   
   /// [147: UNSUPPORTED ENGINE VERSION] 
   //  [MINOR ERROR]
   case TC_ENGINE_VERSION_UNSUPPORTED:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 1, IDS_SCRIPT_ENGINE_VERSION_MISMATCH);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [148: MISSING SCRIPT TARGET] 
   //  [MINOR ERROR]
   case TC_SCRIPT_CALL_TARGET_MISSING:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 2, IDS_XML_UNEXPECTED_CLOSING_BRACKET, IDS_SCRIPT_LOAD_IO_ERROR);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [149: SCRIPT TARGET CORRUPT] 
   //  [MINOR ERROR]
   case TC_SCRIPT_CALL_TARGET_CORRUPT:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 2, IDS_XML_UNEXPECTED_CLOSING_BRACKET, IDS_SCRIPT_PROPERTY_TAG_MISSING);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [150: SCRIPT TARGET VALID] 
   //  [MINOR ERROR]
   case TC_SCRIPT_CALL_TARGET_VALID:
      bVerified = checkCommandTranslation(pOperationData->pScriptFile->pCommandList, 0, TEXT("$a = [THIS]-> call script 'script.valid' : iWaitTime=42 szExample='Fandango' bDestroy=[TRUE]"))
                  AND checkErrorQueueMessages(pErrorQueue, iTestCaseID, 1, IDS_XML_UNEXPECTED_CLOSING_BRACKET);
      eDesiredResult = OR_SUCCESS;
      break;

   /// [151: GAME STRING REFERENCE NOT FOUND]
   //  [MINOR ERROR]
   case TC_GAME_STRING_REFERENCE_NOT_FOUND:
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 1, IDS_TRANSLATION_GAME_STRING_REFERENCE_NOT_FOUND);
      eDesiredResult = OR_SUCCESS;
      break;
   }
   
   
   /*
   /// [0: ] - Check for ''
   //  [CRITICAL ERROR]
   case :
      bVerified = checkErrorQueueMessages(pErrorQueue, iTestCaseID, 2, IDS, IDS);
      eDesiredResult = OR_FAILURE;
      break;
   */

   // [CHECK] Operation result
   if (eDesiredResult != pOperationData->eResult)
   {
      CONSOLE("WARNING: TC.%03u Operation Result is '%s' when it should be '%s'", iTestCaseID, szOperationResults[pOperationData->eResult], szOperationResults[eDesiredResult]);
      bVerified = FALSE;
   }

   // Return result
   return bVerified;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         MESSAGE HANDLERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Configuration  : TESTING
/// Function name  : onTestCaseComplete
// Description     : Verify the results of a TestCase
// 
// MAIN_WINDOW_DATA*    pMainWindowData  : [in] Main window data
// CONST OPERATION_DATA*  pOperationData   : [in] OperationData of the thread loading the GameData
// 
// Return Value   : TRUE if validation was succesful, FALSE otherwise
// 
BOOL  onTestCaseComplete(MAIN_WINDOW_DATA*  pMainWindowData, CONST OPERATION_DATA*  pOperationData)
{
   BOOL   bResult;            // Verification result
   UINT   iNextTestCaseID;    // ID of the TestCase following the input TestCase

   // [CHECK] OperationData is a TestCase
   if (pOperationData->iTestCaseID == NULL)
      return FALSE;

   /// Verify result according to type
   switch (identifyTestCaseType(pOperationData->iTestCaseID))
   {
   // [GAME DATA]
   case TCT_GAME_DATA:  
      bResult = verifyGameDataTestCase(pOperationData->iTestCaseID, pOperationData);
      break;

   // [SCRIPT TRANSLATION]
   case TCT_SCRIPT_TRANSLATION:
      bResult = verifyScriptTranslationTestCase(pOperationData->iTestCaseID, pOperationData);
      
      // Close open document
      if (bResult)
         onMainWindowFileClose(pMainWindowData);
      break;
   }

   /// Print result and (possibly) launch next test case
   switch (bResult)
   {
   // [SUCCESS] Run next test case. (give the progress dialog time to destroy itself)
   case TRUE:
      CONSOLE("TestCase %03u: SUCCESSFUL", pOperationData->iTestCaseID);

      // Check whether there are any more test cases
      if (findNextTestCaseID(pOperationData->iTestCaseID, iNextTestCaseID))
         // Launch next test case
         PostMessage(pMainWindowData->hMainWnd, UM_TESTCASE_BEGIN, identifyTestCaseType(iNextTestCaseID), iNextTestCaseID);
      break;

   // [FAILURE] Abort processing
   case FALSE:
      CONSOLE("TestCase %03u: FAILED! Aborting TestCases....", pOperationData->iTestCaseID);
      break;
   }

   // DEBUGGING
   CONSOLE("---------------------------");
   // Return result
   return bResult;
}


#endif // END: ifdef _TESTING
