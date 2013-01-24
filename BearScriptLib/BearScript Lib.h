//
// BearScript Lib.h : Function declarations for the BearScript library
//

#pragma once

// ////////////////////////////////////////////////////////////////////////////////////////
//                                         MACROS
// ////////////////////////////////////////////////////////////////////////////////////////

/// Macro: BearScriptAPI
// Description: Declares a function as 'imported' or 'exported' appropriately
//
#ifdef _BearScriptDLL
   #define BearScriptAPI __declspec(dllexport)
#else
   #define BearScriptAPI __declspec(dllimport)
#endif

/// Macro: TESTING_ONLY
// Description: Ensures a piece of code only exists in the 'Test' builds
//
#ifdef _TESTING
   #define TESTING_ONLY(xExpression)   xExpression
#else
   #define TESTING_ONLY(xExpression)   
#endif

/// DEBUGGING: Enable printing window properties
//#define PRINT_WINDOW_PROPERTIES
//#define BULLWINKLE
//#define ANDREW_WILDE

// ////////////////////////////////////////////////////////////////////////////////////////
//                                           HEADERS
// ////////////////////////////////////////////////////////////////////////////////////////

// Resource library
#include "../BearScriptRes/BearScript Res.h"

// Library headers
#include "resource.h"
#include "../BearScriptUtil/BearScript Util.h"
#include "Types.h"
#include "../Messages.h"

// ////////////////////////////////////////////////////////////////////////////////////////
//                                       EXPORTED GLOBALS
// ////////////////////////////////////////////////////////////////////////////////////////

// Game / Interface colours
extern BearScriptAPI CONST COLORREF     clTextColours[GAME_TEXT_COLOURS],
                                        clInterfaceColours[INTERFACE_COLOURS];

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                      INTERNAL GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////
#ifdef _BearScriptDLL

// Globals / Constants
extern CONST COLOUR_SCHEME              oDefaultScheme[4];                // Colour schemes

extern CONST TCHAR*                     szParameterSyntax[PARAMETER_SYNTAX_COUNT];  // Parameter syntax
extern CONST TCHAR*                     szSyntaxSubScripts[5];

#endif

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                    APPLICATION
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// Helpers
BearScriptAPI HWND              getAppActiveDialog();
BearScriptAPI CALL_STACK_LIST*  getAppCallStackList();
BearScriptAPI CONSOLE_DATA*     getAppConsole();
BearScriptAPI BOOL              getAppError(CONST APPLICATION_ERROR  eError);
BearScriptAPI HINSTANCE         getAppInstance();
BearScriptAPI HIMAGELIST        getAppImageList(CONST IMAGE_TREE_SIZE  eSize);
BearScriptAPI IMAGE_TREE*       getAppImageTree(CONST IMAGE_TREE_SIZE  eSize);
BearScriptAPI UINT              getAppImageTreeIconIndex(CONST IMAGE_TREE_SIZE  eSize, CONST TCHAR*  szIconID);
BearScriptAPI HICON             getAppImageTreeIconHandle(CONST IMAGE_TREE_SIZE  eSize, CONST TCHAR*  szIconID);
BearScriptAPI CONST TCHAR*      getAppName();
BearScriptAPI CONST TCHAR*      getAppRegistryKey();
BearScriptAPI CONST TCHAR*      getAppRegistrySubKey(CONST APPLICATION_REGISTRY_KEY  eSubKey);
BearScriptAPI CONST TCHAR*      getAppResourceLibraryPath();
BearScriptAPI APPLICATION_STATE getAppState();
BearScriptAPI HWND              getAppWindow();
BearScriptAPI WINDOWS_VERSION   getAppWindowsVersion();
BearScriptAPI FILE_SYSTEM*      getFileSystem();
BearScriptAPI GAME_DATA*        getGameData();
BearScriptAPI HINSTANCE         getLibraryInstance();
BearScriptAPI HINSTANCE         getResourceInstance();
BearScriptAPI UINT              getSystemImageListDocumentTypeIcon(CONST FILE_ITEM_FLAG  eDocumentType);
BearScriptAPI UINT              getSystemImageListGameVersionIcon(CONST GAME_VERSION  eGameVersion);
BearScriptAPI BOOL              hasAppErrors();
BearScriptAPI BOOL              isAppClosing();
BearScriptAPI BOOL              isAppVerbose();
BearScriptAPI VOID              resetAppErrors();
BearScriptAPI VOID              setAppActiveDialog(HWND  hDialog);
BearScriptAPI VOID              setAppClosing(CONST BOOL  bClosing);
BearScriptAPI VOID              setAppError(CONST APPLICATION_ERROR  eError);
BearScriptAPI VOID              setAppState(CONST APPLICATION_STATE  eNewState);
BearScriptAPI VOID              setAppVerboseMode(CONST BOOL  bVerboseMode);
BearScriptAPI VOID              setAppWindow(HWND  hAppWindow);
BearScriptAPI VOID              setFileSystem(FILE_SYSTEM*  pFileSystem);

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                      ARGUMENTS
/// /////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
ARGUMENT*                 createArgumentFromNode(CONST XML_TREE_NODE*  pArgumentNode, CONST UINT  iIndex, ERROR_STACK*  &pError);
BearScriptAPI ARGUMENT*   createArgumentFromName(CONST TCHAR*  szName, CONST TCHAR*  szDescription);
AVL_TREE*                 createArgumentTreeByID();
VOID                      deleteArgument(ARGUMENT*  &pArgument);
VOID                      deleteArgumentTreeNode(LPARAM  pData);

// Helpers
LPARAM                    extractArgumentTreeNode(LPARAM  pNode, CONST AVL_TREE_SORT_KEY  eProperty);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    BINARY TREE OPERATIONS
/// ////////////////////////////////////////////////////////////////////////////////////////

// Functions
BearScriptAPI VOID   performBinaryTreeMediaPageReplication(AVL_TREE*  pSourceTree, CONST MEDIA_ITEM_TYPE  eType, CONST UINT  iPageID, AVL_TREE*  pDestinationTree);
BearScriptAPI VOID   performBinaryTreeMediaPageTypeCount(AVL_TREE*  pTargetTree, UINT*  piOutput, CONST UINT  iTypeCount);
BearScriptAPI VOID   performBinaryTreeScriptDependenciesOutput(AVL_TREE*  pTree, HWND  hListView);
BearScriptAPI VOID   performBinaryTreeReplication(AVL_TREE*  pSourceTree, AVL_TREE*  pDestinationTree);
BearScriptAPI VOID   performBinaryTreeStringPageReplication(AVL_TREE*  pSourceTree, CONST UINT  iPageID, AVL_TREE*  pDestinationTree);
BearScriptAPI VOID   performLanguageFileXMLGeneration(LANGUAGE_FILE*  pLanguageFile, TCHAR*  szConversionBuffer);

/// Operations (CONVERTED)
VOID   treeprocCountMediaPageTypes(AVL_TREE_NODE*  pNode, AVL_TREE_OPERATION*  pOperationData);
VOID   treeprocOutputScriptDependencies(AVL_TREE_NODE*  pCurrentNode, AVL_TREE_OPERATION*  pOperationData);
VOID   treeprocReplicateMediaPage(AVL_TREE_NODE*  pNode, AVL_TREE_OPERATION*  pOperationData);
VOID   treeprocReplicateStringPage(AVL_TREE_NODE*  pNode, AVL_TREE_OPERATION*  pOperationData);
VOID   treeprocReplicateBinaryTree(AVL_TREE_NODE*  pNode, AVL_TREE_OPERATION*  pOperationData);
VOID   treeprocIndexBinaryTreeObjects(AVL_TREE_NODE*  pNode, AVL_TREE_OPERATION*  pOperationData);

// In progress


// Operations (NOT YET CONVERTED)
VOID   treeprocGenerateLanguageFileXML(AVL_TREE_NODE*  pNode, AVL_TREE_OPERATION*  pOperationData);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    BUG REPORTS
/// ////////////////////////////////////////////////////////////////////////////////////////

// Helpers
TCHAR*   generateBugReportFileName(CONST SUBMISSION_OPERATION*  pOperationData);
VOID     getBugReportServerResponse(TCHAR*  szOutput, DWORD  dwLength);
BOOL     getLastSystemError(TCHAR*  szOutput, CONST UINT  iLength);
CONST TCHAR*  identifySubmissionOperationString(SUBMISSION_OPERATION*  pOperationData);

// Functions
BearScriptAPI VOID   cleanSubmissionForumUserName(TCHAR*  szUserName, CONST UINT  iBufferLength);
UINT                 performBugReportCompression(SUBMISSION_OPERATION*  pOperationData, RAW_FILE*  &szOutput);
BOOL                 performBugReportNavigation(HINTERNET  hConnection, CONST TCHAR*  szUserName, OPERATION_PROGRESS*  pProgress, ERROR_STACK*  &pError);
OPERATION_RESULT     performBugReportSubmission(CONST RAW_FILE*  szSubmissionFile, CONST UINT  iFileSize, CONST TCHAR*  szUserName, SUBMISSION_OPERATION*  pOperationData);
BOOL                 performBugReportTransfer(HINTERNET  hConnection, CONST RAW_FILE*  szSubmissionFile, CONST UINT  iFileSize, CONST TCHAR*  szRemoteFileName, OPERATION_PROGRESS*  pProgress, ERROR_STACK*  &pError);

// Threads FUnctions
BearScriptAPI DWORD  threadprocSubmitReport(VOID*  pParameter);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                     CODE  OBJECTs
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
BearScriptAPI  CODEOBJECT*       createCodeObject(CONST TCHAR*  szLine);
BearScriptAPI  VOID              deleteCodeObject(CODEOBJECT*  &pCodeObject);

// Helpers
BOOL                             isCodeObjectCommandKeyword(CONST CODEOBJECT*  pCodeObject);
BOOL                             isStringKeyword(CONST TCHAR*  szString, CONST UINT  iIndex);
   
// (Exported) Helpers
BearScriptAPI  BOOL              identifyArrayIndex(CODEOBJECT*  pData);
BearScriptAPI  CHARACTER_CLASS   identifyCharacterClass(CONST TCHAR  cCharacter);
BearScriptAPI  CONDITIONAL_ID    identifyConditionalID(CONST TCHAR*  szLine);
BearScriptAPI  BOOL              isStringLogicalOperator(CONST TCHAR*  szString);

// Functions
BearScriptAPI  BOOL              findNextCodeObject(CODEOBJECT*  pData);
BearScriptAPI  BOOL              findNextCodeObjectWithArguments(CODEOBJECT*  pCodeObject, CONST SCRIPT_FILE*  pScriptFile);
BOOL                             findNextParameterCodeObject(CODEOBJECT*  pCodeObject, CONST BOOL  bSkipOperators);
BOOL                             findNextScriptCallArgumentCodeObject(CODEOBJECT*  pCodeObject, CONST COMMAND*  pCommand, TCHAR*  &szArgumentName, ERROR_STACK*  &pError);
BearScriptAPI  BOOL              findCodeObjectByCharacterIndex(CODEOBJECT*  pCodeObject, CONST UINT  iIndex);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    COLOUR SCHEMES
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
COLOUR_SCHEME*      createColourScheme();
VOID                deleteColourScheme(COLOUR_SCHEME*  &pColourScheme);
BearScriptAPI VOID  destructorColourScheme(LPARAM  pColourSchemeListItem);

// Helpers
BearScriptAPI COLORREF  getGameTextColour(CONST GAME_TEXT_COLOUR  eColour);
BearScriptAPI COLORREF  getInterfaceColour(CONST INTERFACE_COLOUR  eColour);

// Functions
BearScriptAPI LIST*  generateColourSchemeListFromRegistry();
COLOUR_SCHEME*       generateColourSchemeFromRegistry(HKEY   hSchemeKey);
VOID                 saveColourSchemeToRegistry(HKEY  hSchemeKey, CONST COLOUR_SCHEME*  pScheme);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                       COMMANDS
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
COMMAND*                createCommandFromNode(CONST SCRIPT_FILE*  pScriptFile, CONST COMMAND_NODE*  pCommandNode, ERROR_STACK*  &pError);
BearScriptAPI COMMAND*  createCommandFromText(CONST TCHAR*  szCommandText, CONST UINT  iLineNumber);
COMMAND*                createCustomCommandf(CONST UINT  iID, CONST UINT  iFlags, CONST UINT  iLineNumber, CONST TCHAR*  szFormat, ...);
COMMAND*                createHiddenJumpCommand(CONST UINT  iJumpDestination);
BearScriptAPI COMMAND*  duplicateCommand(CONST COMMAND*  pCommand);
BearScriptAPI VOID      deleteCommand(COMMAND*  &pCommand);
VOID                    destructorCommand(LPARAM  pCommand);

// Helpers
BearScriptAPI BOOL          isCommandGameStringDependent(CONST COMMAND*  pCommand);
BearScriptAPI CONST TCHAR*  identifyCommandTypeString(CONST COMMAND_TYPE  eType);
BearScriptAPI LIST*         generateCommandGameStringReferences(CONST COMMAND*  pCommand);
BearScriptAPI BOOL          isCommandComment(CONST COMMAND*  pCommand);
BearScriptAPI BOOL          isCommandTradingSearch(CONST COMMAND*  pCommand);
BearScriptAPI BOOL          isCommandID(CONST COMMAND*  pCommand, CONST UINT  iID);
BearScriptAPI BOOL          isCommandType(CONST COMMAND*  pCommand, CONST COMMAND_TYPE  eType);

// Functions
BearScriptAPI BOOL      findScriptCallTargetInCommand(CONST COMMAND*  pCommand, CONST TCHAR*  &szOutput);
VOID                    setCommandJumpDestination(COMMAND*  pCommand, CONST COMMAND*  pTarget);
VOID                    setCommandJumpDestinationByIndex(COMMAND*  pCommand, CONST UINT  iIndex);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    COMMAND (MACROS)
/// ////////////////////////////////////////////////////////////////////////////////////////

// Functions
BOOL   isCommandAddCustomMenuInfoHeading(CONST COMMAND*  pCommand, CONST TCHAR*  szTextVariable, CONST TCHAR*  &szMenuVariable);
BOOL   isCommandAddCustomMenuItem(CONST COMMAND*  pCommand, CONST TCHAR*  szTextVariable, CONST TCHAR*  &szMenuVariable, PARAMETER*  &pItemID);
BOOL   isCommandArrayAccess(CONST COMMAND*  pCommand, CONST TCHAR*  szArrayVariable, CONST TCHAR*  szElementVariable, CONST TCHAR*  &szReturnVariable);
BOOL   isCommandArrayAllocation(CONST COMMAND*  pCommand, CONST TCHAR*  &szArrayVariable, INT&  iArraySize);
BOOL   isCommandArrayAssignment(CONST COMMAND*  pCommand, CONST TCHAR*  szArrayVariable, CONST UINT  iElement, PARAMETER*  &pAssignment);
BOOL   isCommandArrayCount(CONST COMMAND*  pCommand, CONST TCHAR*  &szCountVariable, CONST TCHAR*  &szArrayVariable);
BOOL   isCommandReadText(CONST COMMAND*  pCommand, CONST TCHAR*  &szReturnVariable, PARAMETER*  &pPageID, PARAMETER*  &pStringID);
BOOL   isExpressionSimpleLoopConditional(CONST COMMAND*  pCommand, CONST TCHAR*  szIteratorName);
BOOL   isExpressionRangedLoopConditional(CONST COMMAND*  pCommand, CONST TCHAR*  szIteratorName, CONST OPERATOR_TYPE  eOperator, PARAMETER*  &pFinalValue);
BOOL   isExpressionLoopCounter(CONST COMMAND*  pCommand, CONST TCHAR*  szIteratorName, CONST INT  iLoopStep);
BOOL   isExpressionLoopInitialisation(CONST COMMAND*  pCommand, CONST TCHAR*  &szIteratorName, PARAMETER*  &pInitialValue, INT&  iStepValue);

// Functions
BearScriptAPI BOOL  isCommandGotoLabel(CONST COMMAND*  pCommand, CONST TCHAR*  &szLabelName);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    COMMAND DESCRIPTION
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
DESCRIPTION_VARIABLE* createDescriptionVariable(CONST TCHAR*  szName, CONST TCHAR*  szValue, CONST UINT  iParameters, CONST BOOL  bRecursive);
AVL_TREE*            createDescriptionVariableTreeByName();
VOID                 deleteDescriptionVariable(DESCRIPTION_VARIABLE*  &pVariable);

// Helpers
VOID              deleteDescriptionVariableTreeNode(LPARAM  pVariable);
LPARAM            extractDescriptionVariableTreeNode(LPARAM  pNode, CONST AVL_TREE_SORT_KEY  eProperty);
BOOL              insertDescriptionIntoVariablesFile(VARIABLES_FILE*  pVariablesFile, CONST TCHAR*  szName, CONST TCHAR*  szValue, CONST UINT  iParameterCount, CONST BOOL  bRecursive, ERROR_STACK*  &pError);

// Functions
TCHAR*            generatePopulatedDescriptionSource(CONST TCHAR*  szSourceText, CONST UINT  iSourceLength, CONST AVL_TREE*  pVariablesTree, CONST COMMAND_SYNTAX*  pSyntax, CONST UINT  iPageID, CONST UINT  iStringID, ERROR_QUEUE*  pErrorQueue);
OPERATION_RESULT  loadObjectDescriptions(OPERATION_PROGRESS*  pProgress, HWND  hParentWnd, ERROR_QUEUE*  pErrorQueue);
VOID              populateDescriptionSyntax(CONST TCHAR*  szText, TCHAR*  szOutput, CONST UINT  iOutputLength, CONST COMMAND_SYNTAX*  pSyntax, ERROR_QUEUE*  pErrorQueue);

// Tree operations
VOID              treeprocConvertDescriptionVariableToInternal(AVL_TREE_NODE*  pNode, AVL_TREE_OPERATION*  pOperationData);
VOID              treeprocGenerateObjectDescription(AVL_TREE_NODE*  pCurrentNode, AVL_TREE_OPERATION*  pOperationData);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    COMMAND SYNTAX
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
COMMAND_SYNTAX*      createCommandSyntax();
AVL_TREE*            createCommandSyntaxTreeByID();
AVL_TREE*            createCommandSyntaxTreeByHash();
COMMAND_SYNTAX*      duplicateCommandSyntax(CONST COMMAND_SYNTAX*  pCommandSyntax);
VOID                 deleteCommandSyntax(COMMAND_SYNTAX*  &pCommandSyntax);
VOID                 deleteCommandSyntaxTreeNode(LPARAM  pData);

// Helpers
BearScriptAPI GAME_VERSION  calculateCommandSyntaxGameVersion(CONST COMMAND_SYNTAX*  pCommandSyntax);
BearScriptAPI LPARAM extractCommandSyntaxTreeNode(LPARAM  pNode, CONST AVL_TREE_SORT_KEY  eProperty);
BearScriptAPI BOOL   findCommandSyntaxByID(CONST UINT  iID, CONST GAME_VERSION  eGameVersion, CONST COMMAND_SYNTAX*  &pOutput, ERROR_STACK*  &pError);
BOOL                 findCommandSyntaxByHash(CONST TCHAR*  szSearchHash, CONST GAME_VERSION  eGameVersion, CONST COMMAND_SYNTAX*  &pOutput, ERROR_STACK*  &pError);
BOOL                 findParameterSyntaxByName(CONST TCHAR*  szSyntax, PARAMETER_SYNTAX&  eOutput);
BOOL                 findParameterSyntaxByPhysicalIndex(CONST COMMAND_SYNTAX*  pSyntax, CONST UINT  iIndex, PARAMETER_SYNTAX&  eOutput);
UINT                 identifyCommandCompatibilityFlags(TCHAR*  szCompatilityIDs);
COMMAND_GROUP        identifyCommandGroupFromName(CONST TCHAR*  szGroupName);
BearScriptAPI CONST TCHAR*  identifyParameterSyntaxDefaultValue(CONST PARAMETER_SYNTAX  eSyntax);
PARAMETER_SYNTAX     identifyParameterSyntaxFromName(CONST TCHAR*  szSyntax);
CONST TCHAR*         identifyParameterSyntaxSubScript(TCHAR  chChar);
BOOL                 isParameterSyntaxValid(CONST PARAMETER_SYNTAX  eSyntax);

// Functions
TCHAR*               generateCommandSyntaxHash(CONST COMMAND_SYNTAX*  pSyntax);
TCHAR*               generateCommandSyntaxSuggestionText(CONST COMMAND_SYNTAX*  pCommandSyntax);
BOOL                 generateCommandSyntaxTitle(CONST COMMAND_SYNTAX*  pSyntax, CONST RICH_SYNTAX_TYPE  eType, RICH_TEXT*  &pOutput, ERROR_QUEUE*  pErrorQueue);
BOOL                 insertCommandSyntaxIntoGameData(COMMAND_SYNTAX*  pCommandSyntax);
BearScriptAPI BOOL   isCommandInterruptable(CONST COMMAND_SYNTAX*  pSyntax);
BearScriptAPI BOOL   isCommandSyntaxCompatible(CONST COMMAND_SYNTAX*  pCommandSyntax, CONST GAME_VERSION  eGameVersion);
BearScriptAPI BOOL   isCommandReferenceURLPresent(CONST COMMAND_SYNTAX*  pSyntax);
OPERATION_RESULT     loadCommandSyntaxFile(GAME_FILE*  pSyntaxFile, HWND  hParentWnd, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue);
OPERATION_RESULT     loadCommandSyntaxTree(HWND  hParentWnd, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue);
BOOL                 verifyParameterSyntax(CONST DATA_TYPE  eType, CONST PARAMETER_SYNTAX  eSyntax);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    DEBUG CONSOLE
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
HWND                   createConsole();
CONSOLE_DATA*          createConsoleData();
BearScriptAPI  VOID    deleteConsole();
VOID                   deleteConsoleData(CONSOLE_DATA*  &pConsole);


// Helpers
BearScriptAPI  BOOL    isConsoleVisible();

// Functions
BearScriptAPI  BOOL    attachConsole(HWND  hParent);
BearScriptAPI  VOID    consolePrint(CONST TCHAR* szText);
BearScriptAPI  VOID    consolePrintError(CONST ERROR_STACK*  pError);
BearScriptAPI  VOID    consolePrintErrorQueue(CONST ERROR_QUEUE*  pErrorQueue);
BearScriptAPI  VOID    consolePrintf(CONST TCHAR* szFormat, ...);
BearScriptAPI  VOID    consolePrintfA(CONST CHAR* szFormatA, ...);
BearScriptAPI  VOID    consolePrintLastError(CONST TCHAR*  szActivity, CONST TCHAR*  szFunction, CONST TCHAR*  szFile, CONST UINT  iLineNumber);
BearScriptAPI  VOID    consolePrintLastErrorEx(CONST DWORD  dwErrorCode, CONST TCHAR*  szActivity, CONST TCHAR*  szFunction, CONST TCHAR*  szFile, CONST UINT  iLineNumber);
BearScriptAPI  VOID    consolePrintVerboseA(CONST CHAR*  szFormat, ...);
BearScriptAPI  VOID    consolePrintVerboseHeadingA(CONST CHAR*  szFormat, ...);
BearScriptAPI  VOID    consoleVPrintf(CONST TCHAR* szFormat, va_list  pArgList);
BearScriptAPI  VOID    consoleVPrintfA(CONST CHAR* szFormatA, va_list  pArgList);
BearScriptAPI  VOID    deleteConsoleLogFile();
BearScriptAPI  TCHAR*  generateConsoleLogFilePath();
BearScriptAPI  VOID    showConsole(CONST BOOL  bShow);

// Window procedure
LRESULT CALLBACK       wndprocConsole(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

// Defines uniform console separators
#define  LARGE_PARTITION                       "================================================================================"
#define  SMALL_PARTITION                       "--------------------------------------------------------------------------------"

/// Macro: CONSOLE
// Description: Prints a console message 
//
#define  CONSOLE                               consolePrintfA

/// Macro: CONSOLE_ERROR
// Description: Prints a console debugging message
//
#define  CONSOLE_ERROR(szError)                consolePrintfA("ERROR: " __FUNCTION__ " : " szError)

/// Macro: VERBOSE
// Description: Prints a console message only in VERBOSE mode
//
#define  VERBOSE                               consolePrintVerboseA

/// Macro: VERBOSE_HEADING
// Description: Prints a verbose-mode heading with a small parititon
//
#define  VERBOSE_HEADING                       consolePrintVerboseHeadingA

/// Macro: VERBOSE_COMMAND
// Description: Highlights a command in VERBOSE mode
// 
// CONST CHAR*  szCommand : [in] (ANSI) Name of the command to highlight
// CONST CHAR*  eSize     : [in] Either SMALL_PARITION or LARGE_PARTIION
//
#define  VERBOSE_COMMAND(szCommand, eSize)   { consolePrintfA(LARGE_PARTITION);   consolePrintfA(szCommand __FUNCTION__);    consolePrintfA(LARGE_PARTITION);   }

/// Macro: VERBOSE_UI_COMMAND / VERBOSE_LIB_COMMAND / VERBOSE_LIB_EVENT
// Description: Highlights user-invoked or internal commands/events in VERBOSE mode
// 
#define  VERBOSE_UI_COMMAND()                  VERBOSE_COMMAND("User Interface Command: ", LARGE_PARTITION)
#define  VERBOSE_LIB_COMMAND()                 VERBOSE_COMMAND("Library Command: ", LARGE_PARTITION)
#define  VERBOSE_LIB_EVENT()                   VERBOSE_COMMAND("Event: ", LARGE_PARTITION)
#define  VERBOSE_THREAD_COMMAND()              VERBOSE_COMMAND("Operation Stage: ", LARGE_PARTITION)

/// Macro: VERBOSE_LIB_COMMAND_MINOR
// Description: Provides minor highlighting for an internal command
// 
#define  VERBOSE_LIB_COMMAND_MINOR()           VERBOSE_COMMAND("Library Command: ", SMALL_PARTITION)

/// Macro: ERROR_CHECK
// Description: Prints the contents of GetLastError to the console
// 
// CONST CHAR*  szActivityA : [in] (ANSI) Description the activity that failed
// ...   ....   xExpression : [in] Expression to test
// 
#define  ERROR_CHECK(szActivityA, xExpression) if ((xExpression) == FALSE) consolePrintLastError(TEXT(szActivityA), __WFUNCTION__, __SWFILE__, __LINE__); else VERBOSE(szActivityA " - success!");

/// Macro: DEBUG_WINDOW
// Description: Prints properties of a window to the console
// 
// CONST CHAR*  szWindowNameA : [in] Name of the window
// HWND         hWnd          : [in] Window handle
// 
#ifdef PRINT_WINDOW_PROPERTIES
   #define  DEBUG_WINDOW(szNameA, hWnd)        debugQueryWindow(hWnd, TEXT(szNameA), TRUE)
#else
   #define  DEBUG_WINDOW(szNameA, hWnd) 
#endif

/// Macro: VERBOSE_SMALL_PARTITION / VERBOSE_LARGE_PARTITION
// Description: Prints a console separator only in VERBOSE mode
// 
#define  VERBOSE_SMALL_PARTITION()             VERBOSE(SMALL_PARTITION)
#define  VERBOSE_LARGE_PARTITION()             VERBOSE(LARGE_PARTITION)

/// Macro: VERBOSE_THREAD_COMPLETE
// Description: Prints a large separator indicating a worker thread has completed
// 
// CONST CHAR*  szOperation : [in] (ANSI) Name of the operation that completed
//
#define  VERBOSE_THREAD_COMPLETE(szOperation)  VERBOSE_LARGE_PARTITION();  CONSOLE("Thread %#x:   " szOperation, GetCurrentThreadId()); VERBOSE_LARGE_PARTITION()

/// Macro: BUG
// Description: Prints a bug description to the console
//
// CONST CHAR*  szTask : [in] (ANSI) Description of the bug
//
#define  BUG(szBug)                            CONSOLE("BUG: " szBug " in " __FUNCTION__) 

/// Macro: OFFICIAL_BUG
// Description: Prints a bug description and bug ID to the console
//
// CONST UINT   iID    : [in] Bug ID
// CONST CHAR*  szTask : [in] (ANSI) Description of the bug
//
#define  OFFICIAL_BUG(iID, szBug)              CONSOLE("BUG " #iID ": " szBug " in " __FUNCTION__)

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    DEBUGGING
/// ////////////////////////////////////////////////////////////////////////////////////////

// Functions
BearScriptAPI VOID    debugQueryWindow(HWND  hWnd, CONST TCHAR*  szName, CONST BOOL  bIncludeParent);
BearScriptAPI TCHAR*  generateStackTrace(HANDLE  hProcess);


/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    ERROR QUEUES
/// ////////////////////////////////////////////////////////////////////////////////////////

/// Macros: Creation / Destruction
#define       createErrorQueue()                       createQueue(destructorErrorStack)
#define       deleteErrorQueue                         deleteQueue

/// Macros: Helpers
#define       clearErrorQueue(pErrorQueue)             deleteListContents(pErrorQueue)
#define       pushErrorQueue(pErrorQueue, pError)      pushLastQueueObject(pErrorQueue, (LPARAM)(pError))
#define       popErrorQueue(pErrorQueue)               (ERROR_STACK*)popFirstQueueObject(pErrorQueue)
#define       lastErrorQueue(pErrorQueue)              (ERROR_STACK*)lastObjectFromQueue(pErrorQueue)
#define       firstErrorQueue(pErrorQueue)             (ERROR_STACK*)firstObjectFromQueue(pErrorQueue)

// Creation / Destruction
BearScriptAPI ERROR_QUEUE*  duplicateErrorQueue(CONST ERROR_QUEUE*  pErrorQueue);

// Helpers
BearScriptAPI BOOL          findErrorStackByIndex(CONST ERROR_QUEUE*  pErrorQueue, CONST UINT  iIndex, ERROR_STACK*  &pOutput);
BearScriptAPI BOOL          hasErrors(CONST ERROR_QUEUE*  pErrorQueue);
BearScriptAPI ERROR_TYPE    identifyErrorQueueType(CONST ERROR_QUEUE*  pErrorQueue);
BearScriptAPI ERROR_STACK*  getErrorQueueItem(CONST ERROR_QUEUE*  pErrorQueue, CONST UINT  iIndex);

// Functions
VOID  attachLocationToLastError(ERROR_QUEUE*  pErrorQueue, CONST UINT  iErrorCode, CONST TCHAR*  szErrorCode, CONST TCHAR*  szFunction, CONST TCHAR*  szFile, CONST UINT  iLine);
VOID  attachXMLToLastError(ERROR_QUEUE*  pErrorQueue, CONST TCHAR*  szXML, CONST UINT  iPosition);
VOID  pushCommandAndOutputQueues(ERROR_STACK*  pError, ERROR_QUEUE*  pOutputQueue, ERROR_QUEUE*  pCommandQueue, CONST ERROR_TYPE  eType = ET_ERROR);
VOID  pushCommandAndCodeEditErrorQueue(COMMAND*  pCommand, HWND  hCodeEdit, ERROR_STACK*  pError);

// Functions
BearScriptAPI VOID   attachTextToLastError(ERROR_QUEUE*  pErrorQueue, CONST TCHAR*  szText);
BearScriptAPI VOID   enhanceLastError(ERROR_QUEUE*  pErrorQueue, CONST UINT iErrorCode, CONST TCHAR*  szErrorCode, ...);
BearScriptAPI VOID   enhanceLastWarning(ERROR_QUEUE*  pErrorQueue, CONST UINT iErrorCode, CONST TCHAR*  szErrorCode, ...);
BearScriptAPI BOOL   findErrorStackByID(CONST ERROR_QUEUE*  pErrorQueue, CONST UINT  iErrorID);
BearScriptAPI VOID   generateQueuedError(ERROR_QUEUE*  pErrorQueue, CONST UINT  iErrorCode, CONST TCHAR*  szErrorCode, CONST TCHAR*  szFunction, CONST TCHAR*  szFile, CONST UINT  iLine, ...);
BearScriptAPI TCHAR* generateFlattenedErrorQueueText(CONST ERROR_QUEUE*  pErrorQueue);
BearScriptAPI VOID   generateQueuedInformation(ERROR_QUEUE*  pErrorQueue, CONST UINT  iErrorCode, CONST TCHAR*  szErrorCode, CONST TCHAR*  szFunction, CONST TCHAR*  szFile, CONST UINT  iLine, ...);
BearScriptAPI VOID   generateQueuedWarning(ERROR_QUEUE*  pErrorQueue, CONST UINT  iErrorCode, CONST TCHAR*  szErrorCode, CONST TCHAR*  szFunction, CONST TCHAR*  szFile, CONST UINT  iLine, ...);
BearScriptAPI VOID   generateOutputTextFromLastError(ERROR_QUEUE*  pErrorQueue);
BearScriptAPI UINT   generateQueuedExceptionError(CONST EXCEPTION_POINTERS*  pExceptionData, ERROR_QUEUE*  pErrorQueue);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    ERROR STACKS
/// ////////////////////////////////////////////////////////////////////////////////////////

/// Macros: Creation / Destruction
#define      deleteErrorStack(pErrorStack)            deleteStack((STACK*&)pErrorStack)
#define      safeDeleteErrorStack(pErrorStack)      { if (pErrorStack) deleteStack((STACK*&)pErrorStack); }

/// Macros: Helpers
#define      getErrorStackCount(pErrorStack)          getStackItemCount(pErrorStack)
#define      popErrorStack(pErrorStack)               ((ERROR_MESSAGE*)popObjectFromStack(pErrorStack))
#define      pushErrorStack(pErrorStack, pMessage)    pushObjectOntoStack(pErrorStack, (LPARAM)(pMessage))
#define      topErrorStack(pErrorStack)               ((ERROR_MESSAGE*)topStackObject(pErrorStack))

// Creation / Destruction
ERROR_STACK*                 createErrorStack();
ERROR_STACK*                 createErrorStackV(CONST UINT  iErrorCode, CONST TCHAR*  szErrorCode, CONST TCHAR*  szFunction, CONST TCHAR*  szFile, CONST UINT  iLine, va_list  pArguments);
BearScriptAPI VOID           destructorErrorStack(LPARAM  pErrorStack);
BearScriptAPI ERROR_STACK*   duplicateErrorStack(CONST ERROR_STACK*  pErrorStack);

// Helpers
BearScriptAPI BOOL           hasErrors(CONST ERROR_STACK*  pErrorStack);
BearScriptAPI CONST TCHAR*   identifyErrorTypeString(CONST ERROR_TYPE  eType);
BearScriptAPI CONST TCHAR*   identifyErrorTypeIconID(CONST ERROR_TYPE  eType);
BearScriptAPI BOOL           isError(CONST ERROR_STACK*  pErrorStack);
BearScriptAPI VOID           setErrorType(ERROR_STACK*  pErrorStack, CONST ERROR_TYPE  eType);

// Functions
BearScriptAPI VOID           attachTextToError(ERROR_STACK*  pError, CONST TCHAR*  szText);
BearScriptAPI VOID           attachXMLToError(ERROR_STACK*  pError, CONST TCHAR*  szXML, CONST UINT  iPosition);
BearScriptAPI ERROR_HANDLING displayOperationError(HWND  hParentWnd, ERROR_STACK*  pError, CONST UINT  iMessageID, CONST TCHAR*  szMessageID, ...);
BearScriptAPI VOID           enhanceError(ERROR_STACK* pErrorStack, CONST UINT iErrorCode, CONST TCHAR*  szErrorCode, ...);
BearScriptAPI VOID           enhanceWarning(ERROR_STACK* pErrorStack, CONST UINT iErrorCode, CONST TCHAR*  szErrorCode, ...);
BearScriptAPI ERROR_STACK*   generateDualError(CONST UINT  iErrorCode, CONST TCHAR*  szErrorCode, CONST TCHAR*  szFunction, CONST TCHAR*  szFile, CONST UINT  iLine, ...);
BearScriptAPI ERROR_STACK*   generateDualInformation(CONST UINT  iErrorCode, CONST TCHAR*  szErrorCode, CONST TCHAR*  szFunction, CONST TCHAR*  szFile, CONST UINT  iLine, ...);
BearScriptAPI ERROR_STACK*   generateDualWarning(CONST UINT  iErrorCode, CONST TCHAR*  szErrorCode, CONST TCHAR*  szFunction, CONST TCHAR*  szFile, CONST UINT  iLine, ...);
BearScriptAPI UINT           generateExceptionError(CONST EXCEPTION_POINTERS*  pExceptionData, ERROR_STACK*  &pError);
BearScriptAPI ERROR_STACK*   generateSafeDualError(ERROR_STACK*  pError, CONST UINT  iErrorCode, CONST TCHAR*  szErrorCode, CONST TCHAR*  szFunction, CONST TCHAR*  szFile, CONST UINT  iLine, ...);
BearScriptAPI TCHAR*         generateFlattenedErrorStackText(CONST ERROR_STACK*  pErrorStack);
BearScriptAPI VOID           generateOutputTextFromError(ERROR_STACK*  pError);

/// Macro: ERROR_ID
// Description: Generates the string equivilent of an error code
//
#define      ERROR_ID(iErrorCode)      iErrorCode, TEXT(#iErrorCode)

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    ERROR MESSAGES
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
ERROR_MESSAGE*       createErrorMessageV(CONST UINT  iErrorCode, CONST TCHAR*  szID, va_list  pArguments);
VOID                 deleteErrorMessage(ERROR_MESSAGE*  &pErrorMessage);
VOID                 destructorErrorMessage(LPARAM  pErrorMessage);
ERROR_MESSAGE*       duplicateErrorMessage(CONST ERROR_MESSAGE*  pOriginal);

// Helpers
BearScriptAPI BOOL   findErrorMessageByIndex(CONST ERROR_STACK*  pErrorStack, CONST UINT  iIndex, ERROR_MESSAGE*  &pOutput);
BearScriptAPI BOOL   findErrorMessageByLogicalIndex(CONST ERROR_STACK*  pErrorStack, CONST UINT  iLogicalIndex, CONST ERROR_MESSAGE_TYPE  eType, CONST ERROR_MESSAGE*  &pOutput);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    GAME DATA
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destructions
BearScriptAPI VOID           destroyGameData();

// Helpers
BearScriptAPI GAME_LANGUAGE  convertAppLanguageToGameLanguage(CONST APP_LANGUAGE  eLanguage);
BearScriptAPI APP_LANGUAGE   convertGameLanguageToAppLanguage(CONST GAME_LANGUAGE  eLanguage);
BearScriptAPI APP_LANGUAGE   convertInterfaceLanguageToAppLanguage(CONST INTERFACE_LANGUAGE  eLanguage);
BOOL                         findGameDataFileByIndex(CONST UINT  iIndex, CONST GAME_DATA_FILE*  &pOutput);
CONST TCHAR*                 findGameDataFileSubPathByID(CONST GAME_DATA_FILE_INDEX  eGameDataFile, CONST GAME_VERSION  eGameVersion);
BOOL                         findGameObjectDataFileByMainType(CONST MAIN_TYPE  eMainType, CONST GAME_DATA_FILE*  &pOutput);
CONST TCHAR*                 findGameObjectDataFileSubPathByMainType(CONST MAIN_TYPE  eMainType, CONST GAME_VERSION  eGameVersion);
TCHAR*                       generateGameCatalogueFilePath(CONST GAME_DATA_FILE_INDEX  eGameDataFile, CONST TCHAR*  szGameFolder, CONST UINT  iCatalogue, CONST GAME_VERSION  eGameVersion);
BearScriptAPI TCHAR*         generateGameDataFilePath(CONST GAME_DATA_FILE_INDEX  eGameDataFile, CONST TCHAR*  szGameFolder, CONST GAME_VERSION  eGameVersion);
BearScriptAPI TCHAR*         generateGameExecutableFilePath(CONST TCHAR*  szGameFolder, CONST GAME_VERSION  eGameVersion);
TCHAR*                       generateGameLanguageFilePath(CONST GAME_DATA_FILE_INDEX  eGameDataFile, CONST TCHAR*  szGameFolder, CONST GAME_VERSION  eGameVersion, CONST GAME_LANGUAGE  eLanguage);
TCHAR*                       generateGameLanguageFileSubPath(CONST GAME_DATA_FILE_INDEX  eGameDataFile, CONST GAME_VERSION  eGameVersion, CONST GAME_LANGUAGE  eLanguage);
  
// Functions
BearScriptAPI GAME_FOLDER_STATE  calculateGameFolderState(CONST TCHAR*  szGameFolder, GAME_VERSION&  eVersion);
UINT                             identifyGameFolderContents(CONST TCHAR*  szGameFolder, GAME_VERSION&  eVersion);
BearScriptAPI CONST TCHAR*       identifyGameVersionIconID(CONST GAME_VERSION  eGameVersion);

// Thread Operations
BearScriptAPI DWORD          threadprocLoadGameData(VOID*   pParameter);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    GAME DATA INPUT
/// ////////////////////////////////////////////////////////////////////////////////////////

// (Exported) Creation / Destruction
BearScriptAPI GAME_FILE*    createGameFile();
BearScriptAPI GAME_FILE*    createGameFileFromAppPath(CONST TCHAR*  szFileName);
BearScriptAPI GAME_FILE*    createGameFileFromSubPath(CONST TCHAR*  szSubPath);
BearScriptAPI VOID          deleteGameFile(GAME_FILE*  &pGameFile);
BearScriptAPI VOID          deleteGameFileIOBuffers(GAME_FILE*  pGameFile);

// Helpers
BearScriptAPI BOOL          hasInputData(CONST GAME_FILE*  pGameFile);
BearScriptAPI BOOL          hasOutputData(CONST GAME_FILE*  pGameFile);
BearScriptAPI CONST TCHAR*  identifyGameFileFilename(CONST GAME_FILE*  pGameFile);
BearScriptAPI BOOL          identifyGameFileFolder(CONST GAME_FILE*  pGameFile, TCHAR*  &szOutput);
BearScriptAPI VOID          setGameFilePath(GAME_FILE*  pGameFile, CONST TCHAR*  szNewPath, ...);

// Functions
UINT   loadGameFileFromResource(HINSTANCE  hInstance, CONST TCHAR*  szResourceName, GAME_FILE*  pOutput);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                      GAME OBJECTS
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
GAME_OBJECT*       createGameObject(CONST MAIN_TYPE  eMainType, CONST UINT  iSubType, CONST TCHAR*  szText, CONST TCHAR*  szObjectID);
AVL_TREE*          createGameObjectTreeByMainType();
AVL_TREE*          createGameObjectTreeByText();
AVL_TREE*          createGameObjectTreeForCollisions();

// Helpers
UINT               convertStationSizeToStringID(CONST UINT  iStationSize);
UINT               convertVariationIDToStringID(CONST UINT  iVariationID);
BOOL               findGameObjectByIndex(CONST UINT  iIndex, GAME_OBJECT*  &pOutput);
BearScriptAPI BOOL findGameObjectByText(CONST TCHAR*  szSearchString, GAME_OBJECT*  &pOutput);
BOOL               findGameObjectByWare(CONST INT  iPackedWareID, GAME_OBJECT*  &pOutput);
BOOL               findMainTypeGameStringByID(CONST MAIN_TYPE  eMainType, GAME_STRING*  &pOutput);
OBJECT_NAME_GROUP  identifyObjectNameGroupFromMainType(CONST MAIN_TYPE  eMainType);
CONST TCHAR*       identifyMainTypeString(CONST MAIN_TYPE  eMainType);
CONST TCHAR*       identifyTFileFromMainType(CONST MAIN_TYPE  eMainType);

// (Exported) Helpers
BearScriptAPI LPARAM  extractGameObjectTreeNode(LPARAM  pObject, CONST AVL_TREE_SORT_KEY  eProperty);

// Functions
TCHAR*             generateGameObjectShipName(CONST TCHAR*  szNameID, CONST TCHAR*  szRaceID, CONST TCHAR*  szVariationID, CONST TCHAR*  szObjectID, CONST UINT  iLineNumber, ERROR_STACK*  &pError);
TCHAR*             generateGameObjectStationName(CONST TCHAR*  szNameID, CONST TCHAR*  szRaceID, CONST TCHAR*  szSizeID, CONST TCHAR*  szObjectID, CONST MAIN_TYPE  eMainType, CONST UINT  iLineNumber, ERROR_STACK*  &pError);
TCHAR*             generateGameObjectWareName(CONST TCHAR*  szNameID, CONST TCHAR*  szObjectID, CONST MAIN_TYPE  eMainType, CONST UINT  iLineNumber, ERROR_STACK*  &pError);
VOID               performGameObjectNameMangling(GAME_OBJECT*  pGameObject);
VOID               insertGameObjectCollisionsIntoGameData(CONST AVL_TREE*  pDuplicateObjectsTree, ERROR_QUEUE*  pErrorQueue);
BOOL               insertGameObjectIntoGameData(CONST GAME_OBJECT*  pGameObject);
OPERATION_RESULT   generateGameObjectsFromTypeFile(TCHAR*  szFileBuffer, CONST TCHAR*  szSubPath, CONST GAME_DATA_FILE*  pTypeFile, HWND  hParentWnd, ERROR_QUEUE*  pErrorQueue);
OPERATION_RESULT   loadGameObjectTrees(CONST FILE_SYSTEM*  pFileSystem, HWND  hParentWnd, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue);

/// NEW:
BOOL  translateGameObjectProperties(GAME_OBJECT*  pGameObject, CONST TCHAR**  aProperties, ERROR_QUEUE*  pErrorQueue);

// Tree Operations
VOID               treeprocInsertGameObjectCollisionsIntoGameData(AVL_TREE_NODE*  pCurrentNode, AVL_TREE_OPERATION*  pOperationData);

// Macros
#define           deleteGameObject         deleteObjectName

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                      GAME STRINGS
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
BearScriptAPI GAME_PAGE*    createGamePage(CONST UINT  iPageID, CONST TCHAR*  szTitle, CONST TCHAR*  szDescription, CONST BOOL  bVoiced);
BearScriptAPI GAME_STRING*  createGameString(CONST TCHAR*  szText, CONST UINT  iStringID, CONST UINT  iPageID, CONST STRING_TYPE  eType);
SUBSTRING*                  createSubString(CONST TCHAR*  szSourceText);
BearScriptAPI VOID          deleteGamePage(GAME_PAGE*  &pGamePage);
BearScriptAPI VOID          deleteGameString(GAME_STRING*  &pGameString);
VOID                        deleteSubString(SUBSTRING*  &pSubString);

// Helpers
VOID                appendGameStringText(GAME_STRING*  pGameString, CONST TCHAR*  szFormat, ...);
BearScriptAPI BOOL  findGameStringByID(CONST UINT  iStringID, CONST UINT  iPageID, GAME_STRING*  &pOutput);
BearScriptAPI BOOL  findGameStringInTreeByID(CONST AVL_TREE*  pTree, CONST UINT  iStringID, CONST UINT  iPageID, GAME_STRING*  &pOutput);
BearScriptAPI BOOL  findGamePageInTreeByID(CONST AVL_TREE*  pTree, CONST UINT  iPageID, GAME_PAGE*  &pOutput);
BearScriptAPI VOID  updateGameStringText(GAME_STRING*  pGameString, CONST TCHAR*  szNewText);

// Functions
BOOL                findGameStringBySubString(CONST TCHAR*  szSubString, CONST UINT  iLength, CONST UINT  iDefaultPageID, GAME_STRING*  &pOutput);
CONST TCHAR*        findNextNonEscapedCharacter(CONST TCHAR*  szString, CONST TCHAR  chCharacter);
CONST TCHAR*        findNextNonEscapedCharacters(CONST TCHAR*  szString, CONST TCHAR*  szCharacters);
BOOL                findNextSubString(CONST GAME_STRING*  pGameString, SUBSTRING*  pSubString, UINT&  iMissingCount, STACK*  pHistoryStack, ERROR_QUEUE*  pErrorQueue);
CONST TCHAR*        findSubStringEndMarker(CONST TCHAR*  szInput);
TCHAR*              generateGameStringPreview(CONST GAME_STRING*  pGameString, CONST UINT  iPreviewLength);
TCHAR*              generateInternalGameStringPreview(CONST GAME_STRING*  pGameString, CONST UINT  iPreviewLength);
BOOL                isSubStringLookup(CONST TCHAR*  szSubString, CONST UINT  iLength);
UINT                performGameStringResolution(CONST GAME_STRING*  pGameString, TCHAR*  szOutput, CONST UINT  iOutputLength, STACK*  pHistoryStack, ERROR_QUEUE*  pErrorQueue);

// Tree operations
VOID                treeprocConvertGameStringToInternal(AVL_TREE_NODE*  pNode, AVL_TREE_OPERATION*  pOperationData);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                  GAME STRING (OLD)
/// ////////////////////////////////////////////////////////////////////////////////////////

BOOL                performCharacterReplacement(TCHAR*  szInputBuffer, CONST UINT  iBufferLength, CONST UINT  iFlags);
BearScriptAPI BOOL  performStringConversion(TCHAR*  szBuffer, CONST UINT  iBufferLength, CONST STRING_TYPE  eFromType, CONST STRING_TYPE  eToType);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                  GAME STRING TREES
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
AVL_TREE*  createGamePageTreeByID();
AVL_TREE*  createGameStringTreeByPageID();
AVL_TREE*  createGameStringTreeByText();
AVL_TREE*  createGameStringTreeByVersion();
VOID       deleteGamePageTreeNode(LPARAM  pData);
VOID       deleteGameStringTreeNode(LPARAM  pData);

// Helpers
BearScriptAPI LPARAM    extractGameStringTreeNode(LPARAM  pNode, CONST AVL_TREE_SORT_KEY  eProperty);
BearScriptAPI LPARAM    extractGamePageTreeNode(LPARAM  pNode, CONST AVL_TREE_SORT_KEY  eProperty);

// Functions
UINT                insertLanguageFileIntoGameData(LANGUAGE_FILE*  pLanguageFile, ERROR_QUEUE*  pErrorQueue);
BOOL                insertSpecialCasesIntoGameStringTrees();
BOOL                isResultSupplementaryLanguageFile(CONST FILE_SEARCH*  pFileSearch, CONST FILE_ITEM*  pSearchResult);
BOOL                isResultDuplicateSupplementaryLanguageFile(CONST FILE_SEARCH*  pFileSearch, CONST FILE_ITEM*  pSearchResult);
OPERATION_RESULT    loadGameStringTrees(CONST FILE_SYSTEM*  pFileSystem, HWND  hParentWnd, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue);
VOID                performGameStringTreeSubstringResolution(OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue);
VOID                performLanguageFileGamePageMerge(LANGUAGE_FILE*  pLanguageFile);
UINT                performLanguageFileGameStringMerge(LANGUAGE_FILE*  pLanguageFile, ERROR_QUEUE*  pErrorQueue);

// Tree operations
VOID                treeprocResolveGameStringSubstrings(AVL_TREE_NODE*  pNode, AVL_TREE_OPERATION*  pOperationData);
VOID                treeprocMergeSupplementaryGamePages(AVL_TREE_NODE*  pCurrentNode, AVL_TREE_OPERATION*  pOperationData);
VOID                treeprocMergeSupplementaryGameStrings(AVL_TREE_NODE*  pCurrentNode, AVL_TREE_OPERATION*  pOperationData);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                  IMAGE TREE
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
BearScriptAPI IMAGE_TREE*  createImageTree(CONST UINT  iIconSize, CONST UINT  iIconCount);
IMAGE_TREE_ITEM*           createImageTreeItem(CONST TCHAR*  szID, HICON  hHandle, CONST UINT  iIndex);
BearScriptAPI VOID         deleteImageTree(IMAGE_TREE*  &pImageTree);
VOID                       deleteImageTreeNode(LPARAM  ImageTreeItem);

// Helpers
LPARAM               extractImageTreeNode(LPARAM  pObject, CONST AVL_TREE_SORT_KEY  eProperty);

// Functions
BearScriptAPI VOID   insertImageTreeIcon(IMAGE_TREE*  pImageTree, CONST TCHAR*  szIconID);
BearScriptAPI UINT   getImageTreeIconIndex(CONST IMAGE_TREE*  pImageTree, CONST TCHAR*  szID);
BearScriptAPI HICON  getImageTreeIconHandle(CONST IMAGE_TREE*  pImageTree, CONST TCHAR*  szID);

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                    LANGUAGE BUTTON TREE
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
BearScriptAPI LANGUAGE_BUTTON*  createLanguageButton(HWND  hRichEdit, CONST TCHAR*  szText, CONST TCHAR*  szID);
BearScriptAPI AVL_TREE*         createLanguageButtonTreeByID();
BearScriptAPI VOID              deleteLanguageButton(LANGUAGE_BUTTON*  pButton);

// Functions
BearScriptAPI BOOL   findLanguageButtonByObject(AVL_TREE*  pTree, IOleObject*  pObject, LANGUAGE_BUTTON* &pOutput);

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                      LANGUAGE FILE
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// (Export) Creation / Destruction
BearScriptAPI LANGUAGE_FILE*  createLanguageFile(CONST LANGUAGE_FILE_TYPE  eType, CONST TCHAR*  szFullPath, CONST BOOL  bSupplementary);
BearScriptAPI LANGUAGE_FILE*  createUserLanguageFile(CONST TCHAR*  szFullPath);
BearScriptAPI VOID            deleteLanguageFile(LANGUAGE_FILE*  &pLanguageFile);

// Helpers
BOOL              findLanguageFileGamePageByID(CONST LANGUAGE_FILE*  pLanguageFile, CONST UINT  iPageID, GAME_PAGE*  &pOutput);
BOOL              findLanguageFileGameStringByID(CONST LANGUAGE_FILE*  pLanguageFile, CONST UINT  iStringID, CONST UINT  iPageID, GAME_STRING*  &pOutput);
CONST TCHAR*      identifyGameLanguageString(CONST GAME_LANGUAGE  eLanguage);
CONST TCHAR*      identifyLanguageFile(CONST LANGUAGE_FILE*  pLanguageFile);
BOOL              isLanguageFileOfficial(CONST LANGUAGE_FILE*  pLanguageFile);
BOOL              isLanguageFileMaster(CONST LANGUAGE_FILE*  pLanguageFile);

// Functions
VOID              insertGamePageIntoLanguageFile(LANGUAGE_FILE*  pLanguageFile, CONST UINT  iPageID, CONST TCHAR*  szTitle, CONST TCHAR*  szDescription, CONST BOOL  bVoiced);
BOOL              insertGameStringIntoDescriptionsFile(LANGUAGE_FILE*  pDescriptionsFile, CONST TCHAR*  szString, CONST UINT  iID, CONST UINT  iPageID, ERROR_STACK*  &pError);
BOOL              insertGameStringIntoLanguageFile(LANGUAGE_FILE*  pLanguageFile, CONST TCHAR*  szString, CONST UINT  iID, CONST UINT  iPageID, ERROR_STACK*  &pError);
OPERATION_RESULT  loadLanguageFile(CONST FILE_SYSTEM*  pFileSystem, LANGUAGE_FILE*  pTargetFile, CONST BOOL  bSubStrings, HWND  hParentWnd, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE* pErrorQueue);
VOID              performLanguageFileStringConversion(LANGUAGE_FILE*  pTargetFile, OPERATION_PROGRESS*  pProgress);
VOID              performVariablesFileStringConversion(VARIABLES_FILE*  pVariablesFile);

// (Export) Functions
BearScriptAPI BOOL              generateLanguageFileXML(LANGUAGE_FILE*  pLanguageFile, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue);
BearScriptAPI OPERATION_RESULT  translateLanguageFile(LANGUAGE_FILE*  pTargetFile, HWND  hParentWnd, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue);
BearScriptAPI DWORD             threadprocLoadLanguageFile(VOID*  pParameter);
BearScriptAPI DWORD             threadprocSaveLanguageFile(VOID*  pParameter);

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                    MAIN (LIBRARY)
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
BearScriptAPI VOID          createApplication(HINSTANCE  hInstance);
BearScriptAPI VOID          deleteApplication();

// Helpers


// Functions
BearScriptAPI TCHAR*        generateResourceLibraryPath(CONST APP_LANGUAGE  eLanguage);
BearScriptAPI CONST TCHAR*  identifyAppLanguageString(CONST APP_LANGUAGE  eLanguage);
BearScriptAPI CONST TCHAR*  identifyGameVersionString(CONST GAME_VERSION  eVersion);
BearScriptAPI BOOL          loadAppDialogFonts();
BearScriptAPI VOID          loadAppImageTrees();
BearScriptAPI BOOL          loadHelpLibrary();
BearScriptAPI BOOL          loadResourceLibrary(CONST APP_LANGUAGE  eLanguage, ERROR_STACK*  &pError);
BearScriptAPI BOOL          loadRichEditLibrary();
BearScriptAPI BOOL          loadSystemImageListIcons();
BearScriptAPI VOID          unloadApplication();
BearScriptAPI VOID          unloadHelpLibrary();
BearScriptAPI VOID          unloadResourceLibrary();

// Entry Point
BOOL APIENTRY               DllMain(HMODULE  hModule, DWORD  dwPurpose, LPVOID  lpReserved);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                       OPERATORS
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
OPERATOR_STACK*  createOperatorStack();
VOID             deleteOperatorStack(OPERATOR_STACK*  &pStack);

// Helpers
BOOL             compareOperatorPrecedence(CONST OPERATOR_TYPE  eOperator1, CONST OPERATOR_TYPE  eOperator2);
BOOL             findOperatorTypeByText(CONST TCHAR*  szOperator, OPERATOR_TYPE&  eOutput);
BOOL             hasItems(CONST OPERATOR_STACK*  pStack);
BOOL             isUnaryOperator(CONST OPERATOR_TYPE  eOperator);
BOOL             isUnaryOperatorOrBracket(CONST OPERATOR_TYPE  eOperator);
OPERATOR_TYPE    popOperatorStack(OPERATOR_STACK*  pStack);
VOID             pushOperatorStack(OPERATOR_STACK*  pStack, CONST OPERATOR_TYPE  eOperator);
OPERATOR_TYPE    topOperatorStack(CONST OPERATOR_STACK*  pStack);

// Functions
OPERATOR_ASSOCIATIVITY   identifyOperatorAssociativity(CONST OPERATOR_TYPE  eOperator);
OPERATOR_TYPE            identifyOperatorType(CONST TCHAR*  szOperator);
UINT                     identifyOperatorPrecedence(CONST OPERATOR_TYPE  eOperator);


/// ////////////////////////////////////////////////////////////////////////////////////////
///                                       OPERATION DATA
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation
BearScriptAPI DOCUMENT_OPERATION*    createDocumentOperationData(CONST OPERATION_TYPE  eOperation, GAME_FILE*  pGameFile);
BearScriptAPI OPERATION_DATA*        createGameDataOperationData();
BearScriptAPI SEARCH_OPERATION*      createSearchOperationData();
BearScriptAPI SCRIPTCALL_OPERATION*  createScriptCallOperationData(CONST TCHAR*  szScriptName);
BearScriptAPI SUBMISSION_OPERATION*  createSubmissionOperationData(CONST TCHAR*  szCorrection);

// Destruction
BearScriptAPI VOID  deleteOperationData(OPERATION_DATA*  &pOperationData);
BearScriptAPI VOID  destructorOperationData(LPARAM  pOperationData);

// Helpers
BearScriptAPI CONST TCHAR*  identifyOperationTypeString(CONST OPERATION_TYPE  eType);
BearScriptAPI BOOL  isOperationComplete(CONST OPERATION_DATA*  pOperationData);
BearScriptAPI BOOL  isOperationSuccessful(CONST OPERATION_DATA*  pOperationData);

// Functions
VOID                closeThreadOperation(OPERATION_DATA*  pOperationData, CONST OPERATION_RESULT  eResult);
BearScriptAPI BOOL  launchThreadedOperation(LPTHREAD_START_ROUTINE  pfnOperationFunction, OPERATION_DATA*  pOperationData);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                       OPERATION PROGRESS
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
BearScriptAPI OPERATION_PROGRESS*  createOperationProgress();
BearScriptAPI PROGRESS_STAGE*      createProgressStage(CONST UINT  iResourceID);
BearScriptAPI VOID                 deleteOperationProgress(OPERATION_PROGRESS*  &pProgress);

// Functions
BearScriptAPI VOID      addStageToOperationProgress(OPERATION_PROGRESS*  pProgress, CONST UINT  iResourceID);
BearScriptAPI UINT      advanceOperationProgressStage(OPERATION_PROGRESS*  pProgress);
BearScriptAPI VOID      advanceOperationProgressValue(OPERATION_PROGRESS*  pProgress);
BearScriptAPI UINT      calculateOperationProgressPercentage(CONST OPERATION_PROGRESS*  pProgress);
BearScriptAPI UINT      calculateOperationProgressStagePercentage(CONST OPERATION_PROGRESS*  pProgress);
BearScriptAPI UINT      getOperationProgressStageID(CONST OPERATION_PROGRESS*  pProgress);
BearScriptAPI VOID      updateOperationProgressMaximum(OPERATION_PROGRESS*  pProgress, CONST UINT  iMaximum);
BearScriptAPI VOID      updateOperationProgressValue(OPERATION_PROGRESS*  pProgress, CONST UINT  iValue);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                       MEDIA ITEM
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
AVL_TREE*    createMediaItemTreeByPageID();
AVL_TREE*    createMediaPageTreeByGroup();
MEDIA_ITEM*  createSpeechClipItem(CONST UINT  iPageID, CONST UINT  iID, CONST UINT  iPosition, CONST UINT  iLength);
MEDIA_ITEM*  createSoundEffectItem(CONST UINT  iPageID, CONST UINT  iID, CONST TCHAR*  szDescription);
MEDIA_ITEM*  createVideoClipItem(CONST UINT  iPageID, CONST UINT  iID, CONST TCHAR*  szDescription, CONST MEDIA_POSITION*  pStart, CONST MEDIA_POSITION*  pFinish);
VOID         deleteMediaItem(MEDIA_ITEM*  &pMediaItem);
VOID         deleteMediaItemTreeNode(LPARAM  pData);
VOID         deleteMediaPage(MEDIA_PAGE*  &pMediaPage);
VOID         deleteMediaPageTreeNode(LPARAM  pData);

// Helpers
BearScriptAPI LPARAM  extractMediaItemTreeNode(LPARAM  pNode, CONST AVL_TREE_SORT_KEY  eProperty);
BearScriptAPI LPARAM  extractMediaPageTreeNode(LPARAM  pNode, CONST AVL_TREE_SORT_KEY  eProperty);
CONST TCHAR*          identifyMediaItemType(CONST MEDIA_ITEM*  pItem);

// (Exported) Functions
BearScriptAPI UINT    calculateMediaItemDuration(CONST MEDIA_ITEM*  pItem);
BearScriptAPI BOOL    findMediaPageByID(CONST MEDIA_ITEM_TYPE  eType, CONST UINT  iPageID, MEDIA_PAGE*  &pOutput);
BearScriptAPI BOOL    findMediaPageByIndex(CONST UINT iIndex, MEDIA_PAGE*  &pOutput);
BearScriptAPI BOOL    findMediaItemByID(CONST UINT iID, CONST UINT iPageID, MEDIA_ITEM*  &pOutput);
BearScriptAPI BOOL    findMediaItemByIndex(CONST UINT iIndex, MEDIA_ITEM*  &pOutput);
BearScriptAPI VOID    formatMediaItemDuration(CONST UINT  iDuration, TCHAR*  szOutput, CONST UINT  iOutputLength);

// Functions
BOOL              insertMediaPageIntoGameData(CONST MEDIA_ITEM_TYPE  eType, CONST UINT  iPageID, CONST TCHAR*  szTitle, ERROR_STACK*  &pError);
VOID              insertMediaPageIntoSpeechFile(SPEECH_FILE*  pSpeechFile, CONST UINT  iPageID);
BOOL              insertSoundEffectItemIntoGameData(CONST UINT  iPageID, CONST UINT  iID, CONST TCHAR*  szDescription, ERROR_STACK*  &pError);
VOID              insertSpeechClipIntoSpeechFile(SPEECH_FILE*  pSpeechFile, CONST UINT  iPageID, CONST UINT  iItemID, CONST UINT  iPosition, CONST UINT  iLength);
BOOL              insertVideoClipItemIntoGameData(CONST UINT  iPageID, CONST UINT  iVideoID, CONST TCHAR*  szDescription, CONST MEDIA_POSITION*  pStartPos, CONST MEDIA_POSITION*  pEndPos, ERROR_STACK*  &pError);
OPERATION_RESULT  loadMediaItemTrees(CONST FILE_SYSTEM*  pFileSystem, HWND  hParentWnd, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue);
OPERATION_RESULT  loadMediaSoundEffectItems(CONST FILE_SYSTEM*  pFileSystem, HWND  hParentWnd, ERROR_QUEUE*  pErrorQueue);
OPERATION_RESULT  loadMediaVideoClipItems(CONST FILE_SYSTEM*  pFileSystem, HWND  hParentWnd, ERROR_QUEUE*  pErrorQueue);


/// ////////////////////////////////////////////////////////////////////////////////////////
///                                  PARAMETER ARRAYS
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
PARAMETER_ARRAY*   createParameterArray(CONST UINT  iSize);
VOID               deleteParameterArray(PARAMETER_ARRAY*  &pArray);
PARAMETER_ARRAY*   duplicateParameterArray(CONST PARAMETER_ARRAY*  pArray);

// Functions
VOID  appendParameterToArray(PARAMETER_ARRAY*  pArray, PARAMETER*  pParameter);
UINT  getArrayItemCount(CONST PARAMETER_ARRAY*  pArray);
VOID  insertParameterIntoArray(PARAMETER_ARRAY*  pArray, CONST UINT  iIndex, PARAMETER*  pParameter);
VOID  performParameterArrayResizing(PARAMETER_ARRAY*  pArray, CONST UINT  iNewSize);

// (Exported) Functions
BearScriptAPI BOOL  findParameterInArray(CONST PARAMETER_ARRAY*  pArray, CONST UINT  iIndex, PARAMETER*  &pOutput);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                       PREFERENCES
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
PREFERENCES*                      createPreferences();
BearScriptAPI PREFERENCES*        duplicatePreferences(CONST PREFERENCES*  pSource);
BearScriptAPI VOID                deletePreferences(PREFERENCES*  &pAppPreferences);

// Helpers
BearScriptAPI CONST PREFERENCES*  getAppPreferences();
BearScriptAPI BOOL                getAppPreferencesLanguage(APP_LANGUAGE*  pLanguage);
BearScriptAPI RECT*               getAppPreferencesWindowRect(CONST APPLICATION_WINDOW  eWindow);
BearScriptAPI SIZE*               getAppPreferencesWindowSize(CONST APPLICATION_WINDOW  eWindow);
BOOL                              getAppPreferencesVersion(APPLICATION_VERSION*  pversion);
BearScriptAPI VOID                setAppPreferencesDialogSplit(CONST APPLICATION_WINDOW  eWindow, CONST UINT  iSize);
BearScriptAPI VOID                setAppPreferencesDialogVisibility(CONST APPLICATION_WINDOW  eWindow, CONST BOOL  bVisible);
BearScriptAPI VOID                setAppPreferencesForumUserName(CONST TCHAR*  szUserName);
BearScriptAPI VOID                setAppPreferencesGameFolderState(CONST GAME_FOLDER_STATE  eNewFolderState, CONST GAME_VERSION  eNewGameVersion);
BearScriptAPI VOID                setAppPreferencesLanguages(CONST APP_LANGUAGE  eAppLanguage, CONST GAME_LANGUAGE  eGameLanguage);
BearScriptAPI VOID                setAppPreferencesLastFolder(CONST TCHAR*  szLastFolder, CONST FILE_FILTER  eLastFilter);
BearScriptAPI VOID                setAppPreferencesLastFindText(CONST TCHAR*  szSearchText);
BearScriptAPI VOID                setAppPreferencesMainWindowState(CONST BOOL  bMaximised);
BearScriptAPI VOID                setAppPreferencesSearchDialogFilter(CONST RESULT_TYPE  eDialogID, CONST UINT  iFilter);
BearScriptAPI VOID                setAppPreferencesSearchDialogTab(CONST RESULT_TYPE  eDialogID);
BearScriptAPI VOID                setAppPreferencesTutorialComplete(CONST TUTORIAL_WINDOW  eDialogID);
BearScriptAPI VOID                updateAppPreferences(CONST PREFERENCES*  pNewPreferences);

// Functions
BearScriptAPI VOID                deleteAppPreferences();
BearScriptAPI VOID                deleteAppPreferencesRegistryKey();
PREFERENCES*                      generateAppPreferencesFromRegistry(CONST APP_LANGUAGE  eInterfaceLanguage);
BearScriptAPI PREFERENCES_STATE   loadAppPreferences(APP_LANGUAGE  eLanguage, ERROR_STACK*  &pError);
BearScriptAPI BOOL                saveAppPreferences();

// [TESTING]
TESTING_ONLY(BearScriptAPI VOID   setAppPreferencesGameDataFolder(CONST TCHAR*  szNewFolder));

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    RICH TEXT
/// ////////////////////////////////////////////////////////////////////////////////////////

// Helpers
BearScriptAPI VOID   appendRichTextItemToParagraph(RICH_PARAGRAPH*  pParagraph, RICH_ITEM*  pItem);
CONST TCHAR*         convertRichTextTagToString(RICHTEXT_TAG  eTag);
GAME_TEXT_COLOUR     identifyGameTextColourFromRichTextTag(CONST RICHTEXT_TAG  eTag);
RICHTEXT_TAG         identifyRichTextTag(CONST TCHAR*  szTag);
BearScriptAPI BOOL   isRichEditButtonUnique(CONST RICH_TEXT*  pRichText, CONST TCHAR*  szID);
BearScriptAPI VOID   setRichTextParagraphAlignment(RICH_TEXT*  pRichText, CONST UINT  iIndex, CONST PARAGRAPH_ALIGNMENT  eAlignment);

// Functions
BOOL                 findNextRichObject(RICHTEXT_TOKENISER*  pObject);
BearScriptAPI BOOL   generatePlainTextFromLanguageMessage(CONST LANGUAGE_MESSAGE*  pLanguageMessage, GAME_STRING*  pGameString);
BearScriptAPI BOOL   generateLanguageMessageFromGameString(CONST GAME_STRING*  pSourceText, LANGUAGE_MESSAGE*  &pOutput, ERROR_QUEUE*  pErrorQueue);
BearScriptAPI BOOL   generateRichTextFromGameString(CONST GAME_STRING*  pSourceText, RICH_TEXT*  &pOutput, ERROR_QUEUE*  pErrorQueue);
BearScriptAPI BOOL   generateRichTextFromSourceText(CONST TCHAR*  szSourceText, CONST UINT  iTextLength, CONST STRING_TYPE  eStringType, RICH_TEXT*  &pOutput, CONST RICHTEXT_TYPE  eObjectType, ERROR_QUEUE*  pErrorQueue);
BOOL                 translateLanguageMessageTag(CONST RICHTEXT_TOKENISER*  pTokeniser, LANGUAGE_MESSAGE*  pMessage, RICH_ITEM*  pButton, ERROR_QUEUE*  pErrorQueue);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    RICH TEXT (OBJECTS)
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation
BearScriptAPI RICH_PARAGRAPH*  createRichParagraph(PARAGRAPH_ALIGNMENT  eAlignment);
BearScriptAPI RICH_ITEM*       createRichItemButton(CONST TCHAR*  szText, CONST TCHAR*  szID);
RICH_ITEM*                     createRichItemPlainText(CONST TCHAR*  szPlainText, CONST UINT  iTextLength);
RICH_ITEM*                     createRichItemText(CONST RICH_ITEM*  pExistingItem);
BearScriptAPI RICH_ITEM*       createRichItemTextFromEdit(HWND  hRichEdit, CONST UINT  iCharIndex, CONST UINT  iPhraseLength, RICHTEXT_ATTRIBUTES*  pState);
BearScriptAPI RICH_TEXT*       createRichText(CONST RICHTEXT_TYPE  eType);
RICHTEXT_STACK_ITEM*           createRichTextStackItem(RICHTEXT_TOKEN_CLASS  eClass, RICHTEXT_TAG  eTag);
RICHTEXT_TOKENISER*            createRichTextTokeniser(CONST TCHAR*  szSourceText, CONST UINT  iTextLength, CONST STRING_TYPE  eStringType);
RICH_ITEM*                     duplicateRichItem(CONST RICH_ITEM*  pOriginal);
RICH_PARAGRAPH*                duplicateRichParagraph(CONST RICH_PARAGRAPH*  pOriginal);
BearScriptAPI RICH_TEXT*       duplicateRichText(CONST RICH_TEXT*  pOriginal);

// Destruction
BearScriptAPI VOID   deleteRichItem(RICH_ITEM*  &pItem);
BearScriptAPI VOID   deleteRichParagraph(RICH_PARAGRAPH*  &pParagraph);
BearScriptAPI VOID   deleteRichText(RICH_TEXT*  &pRichText);
VOID                 deleteRichTextTokeniser(RICHTEXT_TOKENISER*  &pObject);
VOID                 destructorRichItem(LPARAM  pItem);
VOID                 destructorRichParagraph(LPARAM  pParagraph);

/// Macros:
#define             deleteLanguageMessage(pLanguageMessage)         deleteRichText((RICH_TEXT*&)(pLanguageMessage))

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                   SCRIPT DEPENDENCIES
/// ////////////////////////////////////////////////////////////////////////////////////////

// (Exported) Creation / Destruction
BearScriptAPI STRING_DEPENDENCY*  createStringDependency(CONST UINT  iPageID, CONST UINT  iStringID);
BearScriptAPI AVL_TREE*           createStringDependencyTreeByPageID();

// Creation / Destruction
VOID    deleteStringDependencyNode(LPARAM  pStringDependency);

// Helpers
LPARAM  extractStringDependencyNode(LPARAM  pNodeData, CONST AVL_TREE_SORT_KEY  eProperty);

// (Exported) Functions
BearScriptAPI VOID  insertStringDependencyIntoAVLTree(AVL_TREE*  pTree, CONST UINT  iPageID, CONST UINT  iStringID);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                   STRING DEPENDENCIES
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
BearScriptAPI SCRIPT_DEPENDENCY*  createScriptDependency(CONST TCHAR*  szScriptName);
BearScriptAPI AVL_TREE*           createScriptDependencyTreeByText();
VOID                              deleteScriptDependencyTreeNode(LPARAM  pScriptDependency);

// Helpers
LPARAM  extractScriptDependencyTreeNode(LPARAM  pNodeData, CONST AVL_TREE_SORT_KEY  eProperty);

// Functions
BearScriptAPI VOID    insertScriptDependencyIntoAVLTree(AVL_TREE*  pTree, CONST TCHAR*  szScriptName);

// Thread Functions
BearScriptAPI DWORD   threadprocFindCallingScripts(VOID*  pParameter);

// Tree operations
VOID  treeprocSearchCallingScript(AVL_TREE_NODE*  pCurrentNode, AVL_TREE_OPERATION*  pOperationData);


/// ////////////////////////////////////////////////////////////////////////////////////////
///                                      SCRIPT OBJECTS
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
SCRIPT_OBJECT*     createScriptObject(CONST GAME_STRING*  pGameString);
AVL_TREE*          createScriptObjectTreeByGroup();
AVL_TREE*          createScriptObjectTreeByText();
AVL_TREE*          createScriptObjectTreeForCollisions();
VOID               deleteObjectName(OBJECT_NAME*  pObjectName);
VOID               deleteObjectNameTreeNode(LPARAM  pObject);

// Helpers
BearScriptAPI LPARAM  extractScriptObjectTreeNode(LPARAM  pObject, CONST AVL_TREE_SORT_KEY  eProperty);
BOOL                  findScriptObjectByGroup(CONST OBJECT_NAME_GROUP  eGroup, CONST UINT  iID, SCRIPT_OBJECT*  &pOutput);
BearScriptAPI BOOL    findScriptObjectByText(CONST TCHAR*  szSearchString, SCRIPT_OBJECT*  &pOutput);
OBJECT_NAME_GROUP     identifyObjectNameGroupFromGameString(CONST GAME_STRING*  pGameString);
TCHAR*                generateScriptObjectGroupString(CONST OBJECT_NAME_GROUP  eGroup);

// Functions
VOID               loadScriptObjectsTrees(OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue);
VOID               performScriptObjectNameMangling(SCRIPT_OBJECT*  pScriptObject);

// Tree Operations
VOID               treeprocBuildScriptObjectTrees(AVL_TREE_NODE*  pCurrentNode, AVL_TREE_OPERATION*  pOperationData);
VOID               treeprocMergeScriptObjectCollisions(AVL_TREE_NODE*  pCurrentNode, AVL_TREE_OPERATION*  pOperationData);

// Macros
#define           deleteScriptObject       deleteObjectName

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                       PARAMETERS
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
PARAMETER*           createParameterFromInteger(CONST INT  iValue, CONST DATA_TYPE  eType, CONST PARAMETER_SYNTAX  eSyntax, CONST UINT  iLineNumber);
PARAMETER*           createParameterFromString(CONST TCHAR*  szValue, CONST PARAMETER_SYNTAX  eSyntax, CONST UINT  iLineNumber);
PARAMETER*           createParameterFromVariable(CONST TCHAR*  szName, CONST PARAMETER_SYNTAX  eSyntax, CONST UINT  iLineNumber);
PARAMETER*           duplicateParameter(CONST PARAMETER*  pExistingParameter);
VOID                 deleteParameter(PARAMETER*  &pParameter);
VOID                 destructorParameter(LPARAM  pParameter);

// Helpers
VOID                 appendParameterToCommand(COMMAND*  pCommand, PARAMETER*  pParameter, CONST PARAMETER_TYPE  eType);
BearScriptAPI BOOL   findIntegerParameterInCommandByIndex(CONST COMMAND* pCommand, CONST UINT  iIndex, INT  &iOutput);
BearScriptAPI BOOL   findParameterInCommandByIndex(CONST COMMAND* pCommand, CONST PARAMETER_TYPE  eType, CONST UINT  iIndex, PARAMETER*  &pOutput);
BearScriptAPI BOOL   findReturnObjectParameterInCommand(CONST COMMAND*  pCommand, PARAMETER*  &pOutput);
BearScriptAPI BOOL   findReturnVariableParameterInCommand(CONST COMMAND*  pCommand, CONST TCHAR*  &szOutput);
BearScriptAPI BOOL   findStringParameterInCommandByIndex(CONST COMMAND* pCommand, CONST UINT  iIndex, CONST TCHAR* &szOutput);
BearScriptAPI BOOL   findVariableParameterInCommandByIndex(CONST COMMAND* pCommand, CONST UINT  iIndex, CONST TCHAR* &szOutput);
BearScriptAPI UINT   getCommandParameterCount(CONST COMMAND*  pCommand, CONST PARAMETER_TYPE  eType);
BOOL                 generateParameterFromMissingWare(CONST TCHAR*  szParameterText, CONST PARAMETER_SYNTAX  eSyntax, CONST UINT  iLineNumber, PARAMETER*  &pOutput);
CONST TCHAR*         identifyParameterSyntaxString(CONST PARAMETER_SYNTAX  eParameterSyntax);
VOID                 insertParameterIntoCommand(COMMAND*  pCommand, CONST PARAMETER_INDEX*  pIndex, PARAMETER*  pParameter);
BOOL                 isReferenceObject(CONST PARAMETER*  pParameter);
BearScriptAPI BOOL   isReturnVariableInCommand(CONST COMMAND*  pCommand, CONST TCHAR*  szVariable);
BOOL                 isOperatorParameterInCommandAtIndex(CONST COMMAND*  pCommand, CONST UINT  iIndex, CONST OPERATOR_TYPE  eOperator);
BOOL                 isParameterTypeInCommandAtIndex(CONST COMMAND*  pCommand, CONST UINT  iIndex, CONST DATA_TYPE  eType);
BOOL                 isVariableParameter(CONST PARAMETER*  pParameter);
BOOL                 isVariableParameterInCommand(CONST COMMAND*  pCommand, CONST TCHAR*  szVariableName);
BOOL                 isVariableParameterInCommandAtIndex(CONST COMMAND*  pCommand, CONST UINT  iIndex, CONST TCHAR*  szVariableName);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                      PROJECT FILES
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
BearScriptAPI STORED_DOCUMENT*  createStoredDocument(CONST FILE_ITEM_FLAG  eType, CONST TCHAR*  szFullPath);
BearScriptAPI PROJECT_FILE*     createProjectFile(CONST TCHAR*  szFullPath);
BearScriptAPI VOID              deleteProjectFile(PROJECT_FILE*  &pProjectFile);
BearScriptAPI VOID              deleteStoredDocument(STORED_DOCUMENT*  &pStoredDocument);
BearScriptAPI VOID              destructorStoredDocument(LPARAM  pStoredDocument);

// Helpers
BearScriptAPI VOID             addDocumentToProjectFile(PROJECT_FILE*  pProjectFile, STORED_DOCUMENT*  pDocument);
CONST TCHAR*                   calculateDocumentTypeStringFromFileType(FILE_ITEM_FLAG  eType);
BOOL                           calculateFileTypeFromDocumentTypeString(CONST TCHAR*  szDocumentType, FILE_ITEM_FLAG&  eOutput);
BearScriptAPI PROJECT_FOLDER   calculateProjectFolderFromFileType(CONST FILE_ITEM_FLAG  eFileType);
BearScriptAPI BOOL             findDocumentInProjectFileByIndex(CONST PROJECT_FILE*  pProjectFile, CONST PROJECT_FOLDER  eFolder, CONST UINT  iIndex, STORED_DOCUMENT*  &pOutput);
BearScriptAPI BOOL             findDocumentInProjectFileByPath(CONST PROJECT_FILE*  pProjectFile, CONST TCHAR*  szPath, CONST PROJECT_FOLDER  eFolder, STORED_DOCUMENT*  &pOutput);
BearScriptAPI BOOL             findVariableInProjectFileByIndex(CONST PROJECT_FILE*  pProjectFile, CONST UINT  iIndex, PROJECT_VARIABLE*  &pOutput);
BearScriptAPI BOOL             findVariableInProjectFileByName(CONST PROJECT_FILE*  pProjectFile, CONST TCHAR*  szName, PROJECT_VARIABLE*  &pOutput);
BearScriptAPI UINT             getProjectFileCountByFolder(CONST PROJECT_FILE*  pProjectFile, PROJECT_FOLDER  eFolder);
BearScriptAPI UINT             getProjectFileVariableCount(CONST PROJECT_FILE*  pProjectFile);
BearScriptAPI BOOL             insertVariableIntoProjectFile(PROJECT_FILE*  pProjectFile, CONST PROJECT_VARIABLE*  pVariable);
BearScriptAPI BOOL             removeFileFromProject(PROJECT_FILE*  pProjectFile, STORED_DOCUMENT*  &pDocument);
BearScriptAPI BOOL             removeVariableFromProjectFileByIndex(CONST PROJECT_FILE*  pProjectFile, CONST UINT  iIndex);

// Functions
OPERATION_RESULT      generateProjectFileFromXML(PROJECT_FILE*  pProjectFile, HWND  hParentWnd, ERROR_QUEUE  *pErrorQueue);
BOOL                  generateProjectFileOutputBuffer(PROJECT_FILE*  pProjectFile);
BOOL                  generateXMLTreeFromProjectFile(PROJECT_FILE*  pProjectFile, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue);

// Threads
BearScriptAPI DWORD   threadprocLoadProjectFile(VOID*  pParameter);
BearScriptAPI DWORD   threadprocSaveProjectFile(VOID*  pParameter);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    PROJECT VARIABLES
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
BearScriptAPI PROJECT_VARIABLE*  createProjectVariable(CONST TCHAR*  szName, CONST UINT  iValue);
BearScriptAPI AVL_TREE*          createProjectVariableTreeByText();
BearScriptAPI PROJECT_VARIABLE*  duplicateProjectVariable(CONST PROJECT_VARIABLE*  pSource);
BearScriptAPI VOID               deleteProjectVariable(PROJECT_VARIABLE*  &pProjectVariable);
BearScriptAPI VOID               deleteProjectVariableTreeNode(LPARAM  pProjectVariable);

// Helpers
BearScriptAPI LPARAM             extractProjectVariableTreeNode(LPARAM  pNode, CONST AVL_TREE_SORT_KEY  eProperty);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                      RETURN OBJECTS
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
RETURN_OBJECT*       createReturnObjectConditional(CONST CONDITIONAL_ID  eConditional);
RETURN_OBJECT*       createReturnObjectFromInteger(CONST INT  iReturnValue);
RETURN_OBJECT*       createReturnObjectVariable(CONST UINT  iVariableID);
VOID                 deleteReturnObject(RETURN_OBJECT*  &pReturnObject);

// Helpers
INT                  calculateReturnObjectEncoding(CONST RETURN_OBJECT*  pReturnObject);
RETURN_OBJECT_TYPE   identifyReturnObjectType(CONST INT  iReturnValue);
BearScriptAPI BOOL   isReturnObject(CONST PARAMETER_SYNTAX  eSyntax);

// Functions
RETURN_TYPE          identifyReturnTypeFromConditional(CONST CONDITIONAL_ID   eConditional);

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   SCRIPT CONVERSION
/// /////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
COMMAND_STREAM*      createCommandStream();
SCRIPT_GENERATOR*    createScriptGenerator();
SCRIPT_TRANSLATOR*   createScriptTranslator();
VOID                 deleteCommandStream(COMMAND_STREAM*  &pCommandStream);
VOID                 deleteScriptGenerator(SCRIPT_GENERATOR*  &pGenerator);
VOID                 deleteScriptTranslator(SCRIPT_TRANSLATOR*  &pTranslator);

// Functions
VOID                 appendArgumentNameToGenerator(SCRIPT_GENERATOR*  pGenerator, CONST TCHAR*  szArgumentName);
VOID                 appendCommandToGenerator(SCRIPT_GENERATOR*  pGenerator, CONST COMMAND_LIST  eList, COMMAND*  pCommand);
VOID                 appendCommandToTranslator(SCRIPT_TRANSLATOR*  pTranslator, COMMAND*  pCommand);
VOID                 appendVariableNameToTranslator(SCRIPT_TRANSLATOR*  pTranslator, CONST TCHAR*  szVariableName);
UINT                 calculateGeneratorForEachCommandCount(SCRIPT_GENERATOR*  pGenerator);
UINT                 calculateGeneratorVariableIDFromName(SCRIPT_GENERATOR*  pGenerator, CONST TCHAR*  szVariable);
BOOL                 destroyCommandInTranslatorOutputByIndex(CONST SCRIPT_TRANSLATOR*  pTranslator, CONST UINT  iIndex);
BOOL                 destroyCommandsInTranslatorOutputByIndex(CONST SCRIPT_TRANSLATOR*  pTranslator, CONST UINT  iIndex, CONST UINT  iCount);
BOOL                 findCommandInGeneratorInput(CONST SCRIPT_GENERATOR*  pGenerator, CONST UINT  iIndex, COMMAND*  &pOutput);
BearScriptAPI BOOL   findCommandInGeneratorOutput(CONST SCRIPT_GENERATOR*  pGenerator, CONST COMMAND_TYPE  eType, CONST UINT  iIndex, COMMAND*  &pOutput);
BearScriptAPI BOOL   findCommandInTranslatorOutput(CONST SCRIPT_TRANSLATOR*  pTranslator, CONST UINT  iIndex, COMMAND*  &pOutput);
BOOL                 findVariableNameInGeneratorByIndex(CONST SCRIPT_GENERATOR*  pGenerator, CONST UINT  iIndex, VARIABLE_NAME*  &pOutput);
BOOL                 findVariableNameInTranslatorByIndex(CONST SCRIPT_TRANSLATOR*  pTranslator, CONST UINT  iIndex, VARIABLE_NAME*  &pOutput);
UINT                 getScriptGeneratorVariablesCount(CONST SCRIPT_GENERATOR*  pGenerator);
UINT                 getScriptTranslatorOutputCount(CONST SCRIPT_TRANSLATOR*  pTranslator);
UINT                 getScriptTranslatorVariablesCount(CONST SCRIPT_TRANSLATOR*  pTranslator);
BearScriptAPI VOID   initScriptFileGenerator(SCRIPT_FILE*  pScriptFile);
VOID                 insertVirtualCommandToTranslator(SCRIPT_TRANSLATOR*  pTranslator, CONST UINT  iIndex, COMMAND*  pCommand);
BOOL                 isLastGeneratedCommandIndentedBySkipIf(CONST SCRIPT_GENERATOR*  pGenerator);
BOOL                 isLastTranslatedCommandIndentedBySkipIf(CONST SCRIPT_TRANSLATOR*  pTranslator, CONST LIST_ITEM*  pTargetCommand);

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     SCRIPT FILES
/// /////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
BearScriptAPI SCRIPT_FILE*   createScriptFileByOperation(CONST SCRIPT_FILE_OPERATION  eOperation, CONST TCHAR*  szFullPath);
BearScriptAPI SCRIPT_FILE*   createScriptFileFromFile(CONST TCHAR*  szFullPath, CONST BYTE*  szInputBuffer, CONST UINT  iInputSize);
BearScriptAPI SCRIPT_FILE*   createScriptFileFromText(CONST TCHAR*  szText, CONST TCHAR*  szFullPath);
BearScriptAPI VOID           deleteScriptFile(SCRIPT_FILE*  &pScriptFile);

// Helpers
BearScriptAPI UINT          getScriptFileArgumentCount(CONST SCRIPT_FILE*  pScriptFile);
BearScriptAPI CONST TCHAR*  identifyScriptName(CONST SCRIPT_FILE*  pScriptFile);

// Functions
BearScriptAPI VOID   appendArgumentToScriptFile(SCRIPT_FILE*  pScriptFile, CONST ARGUMENT*  pArgument);
BearScriptAPI BOOL   findArgumentInScriptFileByIndex(CONST SCRIPT_FILE*  pScriptFile, CONST UINT  iIndex, ARGUMENT*  &pOutput);
BearScriptAPI BOOL   isArgumentInScriptFile(CONST SCRIPT_FILE*  pScriptFile, CONST TCHAR*  szVariableName);
BearScriptAPI BOOL   removeArgumentFromScriptFileByIndex(CONST SCRIPT_FILE*  pScriptFile, CONST UINT  iIndex);
BearScriptAPI VOID   setScriptNameFromPath(SCRIPT_FILE*  pScriptFile);
BearScriptAPI VOID   setPathFromScriptName(SCRIPT_FILE*  pScriptFile);
BearScriptAPI VOID   setScriptName(SCRIPT_FILE*  pScriptFile, CONST TCHAR*  szScriptName);
BearScriptAPI BOOL   verifyScriptFileArgumentName(CONST SCRIPT_FILE*  pScriptFile, CONST TCHAR*  szArgumentName, CONST ARGUMENT*  pExcludeArgument);


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                  SCRIPT  TRANSLATION  
/// /////////////////////////////////////////////////////////////////////////////////////////

// Functions
BearScriptAPI BOOL loadScriptCallCommandTargetScript(CONST SCRIPT_FILE*  pCallingScript, CONST COMMAND* pCommand, HWND  hParentWnd, SCRIPT_FILE* &pOutputScript, ERROR_QUEUE*  pErrorQueue);
OPERATION_RESULT   loadScriptProperties(CONST TCHAR*  szFullPath, HWND  hParentWnd, SCRIPT_FILE*  &pScriptFile, ERROR_QUEUE*  pOutputErrorQueue);
BOOL               translateCommandExpression(COMMAND*  pCommand, CONST PARAMETER_COUNT*  pParameterCount, ERROR_STACK* &pError);
OPERATION_RESULT   translateCommandNode(CONST SCRIPT_FILE* pScriptFile, CONST COMMAND_NODE*  pCommandNode, HWND  hParentWnd, COMMAND*  &pOutput, ERROR_QUEUE*  pErrorQueue);
BOOL               translateCommandNodeParameterNodes(CONST COMMAND_NODE*  pCommandNode, COMMAND*  pOutput, ERROR_STACK*  &pError);
OPERATION_RESULT   translateCommandToString(CONST SCRIPT_FILE*  pScriptFile, COMMAND*  pCommand, HWND  hParentWnd, ERROR_QUEUE*  pErrorQueue);
VOID               translateCommandToStringSpecialCases(CONST SCRIPT_FILE*  pScriptFile, COMMAND*  pCommand, HWND  hParentWnd, ERROR_QUEUE*  pErrorQueue);
BOOL               translateParameterNode(CONST XML_TREE_NODE*  pNode, CONST PARAMETER_SYNTAX  eSyntax, PARAMETER*  &pOutput, ERROR_STACK*  &pError);
BOOL               translateParameterTuple(CONST PARAMETER_NODE_TUPLE*  pNodeTuple, CONST PARAMETER_SYNTAX  eSyntax, CONST COMMAND*  pCommand, PARAMETER*  &pParameter, ERROR_STACK*  &pError);
BOOL               translateParameterToString(CONST SCRIPT_FILE*  pScriptFile, COMMAND*  pCommand, PARAMETER*  pParameter, ERROR_STACK*  &pError);
BOOL               translateReturnObjectToString(CONST SCRIPT_FILE*  pScriptFile, CONST RETURN_OBJECT*  pReturnObject, CONST TCHAR*  &szOutput, ERROR_STACK*  &pError);
OPERATION_RESULT   translateScript(SCRIPT_FILE*  pScriptFile, CONST BOOL  bJustProperties, HWND  hParentWnd, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue);
OPERATION_RESULT   translateScriptFile(SCRIPT_FILE*  pScriptFile, CONST BOOL  bJustProperties, HWND  hParentWnd, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue);
OPERATION_RESULT   translateScriptFileMacros(SCRIPT_FILE*  pScriptFile, HWND  hParentWnd, ERROR_QUEUE*  pErrorQueue);
BOOL               translateScriptFileProperties(SCRIPT_FILE*  pOutput, ERROR_QUEUE*  pErrorQueue);
BOOL               translateXMLToScriptFile(SCRIPT_FILE*  pScriptFile, HWND  hParentWnd, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue);

// Thread operations
BearScriptAPI DWORD         threadprocLoadScriptFile(VOID*  pParameter);

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                  SCRIPT  TRANSLATION  (BASE)
/// /////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
COMMAND_COMPONENT*          createCommandComponent(CONST COMMAND*  pCommand);
VOID                        deleteCommandComponent(COMMAND_COMPONENT*  pCommandComponent);

// Helpers
GAME_VERSION                calculateGameVersionFromEngineVersion(CONST UINT  iEngineVersion);
UINT                        calculateEngineVersionFromGameVersion(CONST GAME_VERSION  eGameVersion);
UINT                        convertSectorCoordinatesToStringID(CONST POINT*  ptSector);
BOOL                        convertStringIDToSectorCoordinates(CONST UINT  iStringID, POINT*  pOutput);
CONST TCHAR*                identifyDataTypeString(CONST DATA_TYPE  eType);
BearScriptAPI GAME_PAGE_ID  identifyGamePageIDFromDataType(CONST DATA_TYPE  eDataType);
PARAMETER_NODE_TYPE         identifyParameterSyntaxType(CONST PARAMETER_SYNTAX  eSyntax);
BOOL                        verifyDataType(CONST DATA_TYPE  eType);

// Functions
BOOL                 calculateParameterSyntaxByIndex(CONST COMMAND*  pCommand, CONST UINT  iParameterIndex, PARAMETER_SYNTAX  &eOutput, ERROR_STACK*  &pError);
BearScriptAPI TCHAR* calculateScriptCallTargetFilePath(CONST TCHAR*  szScriptCallFilePath, CONST COMMAND*  pScriptCallCommand);
BOOL                 convertLabelNumberParameterToLabel(CONST COMMAND_NODE*  pCommandNode, COMMAND*  pCommand, ERROR_STACK*  &pError);
BOOL                 findNextCommandComponent(COMMAND_COMPONENT*  pCommandComponent);
BearScriptAPI BOOL   isGameStringDependencyPresent(CONST COMMAND*  pCommand, ERROR_STACK*  &pError);
BOOL                 parseScriptPropertiesFromBuffer(SCRIPT_FILE*  pScriptFile);
BOOL                 performScriptTypeConversion(CONST PARAMETER*  pParameter, CONST DATA_TYPE  eInput, DATA_TYPE&  eOutput, CONST SCRIPT_VALUE_TYPE  eInputType, CONST SCRIPT_VALUE_TYPE  eOutputType, ERROR_STACK*  &pError);
BOOL                 performScriptValueConversion(SCRIPT_FILE*  pScriptFile, CONST PARAMETER*  pInput, INT&  iOutput, CONST SCRIPT_VALUE_TYPE  eInputType, CONST SCRIPT_VALUE_TYPE  eOutputType, ERROR_STACK*  &pError);
BOOL                 verifyScriptEngineVersion(CONST SCRIPT_FILE*  pScriptFile, ERROR_QUEUE*  pErrorQueue);
VOID                 verifyScriptFileScriptNameFromPath(SCRIPT_FILE*  pScriptFile, ERROR_QUEUE*  pErrorQueue);

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                  SCRIPT  TRANSLATION  (XML)
/// /////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
COMMAND_NODE*          createCommandNode(CONST COMMAND_TYPE  eType, XML_TREE_NODE*  pNode);
PARAMETER_NODE_TUPLE*  createParameterNodeTuple();
VOID                   deleteCommandNode(COMMAND_NODE*  &pNode, BOOL  bIncludeXMLNode);
VOID                   deleteParameterNodeTuple(PARAMETER_NODE_TUPLE*  &pParameterNodeTuple);

// Helpers
UINT  getScriptPropertyNameInteger(CONST XML_TREE_NODE*  pNode);
CONST TCHAR*  getScriptPropertyNameString(CONST XML_TREE_NODE*  pNode);

BOOL                 isScriptBranchNonEmptyArray(CONST XML_TREE_NODE*  pBranch, ERROR_STACK*  &pError);
BOOL                 isScriptBranchValid(CONST XML_TREE_NODE*  pBranch, ERROR_STACK*  &pError);
BOOL                 isScriptCommandIDValid(CONST XML_TREE_NODE*  pNode, ERROR_STACK*  &pError);
BOOL                 isScriptPropertyNodeValid(CONST XML_TREE_NODE*  pStartNode, CONST TCHAR*  szNodeName, XML_TREE_NODE*  &pOutput);
BOOL                 isScriptPropertySValNodeValid(CONST XML_TREE_NODE*  pCodeArray, CONST UINT  iIndex, XML_TREE_NODE*  &pOutput);

// Functions
BOOL                 findNextCommandNode(SCRIPT_TRANSLATOR*  pTranslator, COMMAND_NODE*  &pCommandNode, ERROR_STACK*  &pError);
BOOL                 findNextCommandNodeByType(SCRIPT_TRANSLATOR*  pTranslator, CONST COMMAND_TYPE  eType, COMMAND_NODE*  &pOutput);
BOOL                 findNextParameterNodeTuple(CONST COMMAND_NODE*  pCommandNode, XML_TREE_NODE*  &pCurrentNode, PARAMETER_NODE_TUPLE*  pNodeTuple, ERROR_STACK*  &pError);
BOOL                 findScriptTranslationNodes(SCRIPT_TRANSLATOR*  pTranslator, ERROR_STACK*  &pError);
BOOL                 getCommandNodeCommentID(CONST COMMAND_NODE*  pCommandNode, UINT&  iCommandID, ERROR_STACK*  &pError);
BOOL                 getCommandNodeID(CONST COMMAND_NODE*  pCommandNode, UINT&  iCommandID, ERROR_STACK*  &pError);
BOOL                 getCommandNodeIndex(CONST COMMAND_NODE*  pCommandNode, UINT&  iIndex, ERROR_STACK*  &pError);
BOOL                 getSourceValueInteger(CONST XML_TREE_NODE*  pNode, INT&  iOutput, ERROR_STACK*  &pError);
BOOL                 getSourceValueString(CONST XML_TREE_NODE*  pNode, CONST TCHAR*  &szOutput, ERROR_STACK*  &pError);
BOOL                 getSourceValueType(CONST XML_TREE_NODE*  pNode, SOURCE_VALUE_TYPE&  eOutput, ERROR_STACK*  &pError);
PARAMETER_NODE_TYPE  identifyParameterNodeType(CONST COMMAND*  pCommand, CONST PARAMETER_COUNT*  pParameterCount, CONST UINT  iNodeIndex, CONST UINT  iParameterIndex);
BOOL                 isMatchingAuxiliaryCommand(CONST SCRIPT_TRANSLATOR*  pTranslator, CONST COMMAND_NODE*  pStandardNode, CONST COMMAND_NODE*  pAuxiliaryNode, ERROR_STACK*  &pError);
BOOL                 verifyCommandChildNodeCount(COMMAND*  pCommand, CONST UINT  iChildNodes, ERROR_STACK*  &pError);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                  SCRIPT GENERATION (BASE)
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
BearScriptAPI COMMAND_HASH*  createCommandHash();
PARAMETER_INDEX*             createParameterIndex(CONST UINT  iPhysicalIndex, CONST UINT  iLogicalIndex);
BearScriptAPI VOID           deleteCommandHash(COMMAND_HASH*  &pCommandHash);
VOID                         deleteParameterIndex(PARAMETER_INDEX*  pParameterIndex);

// Helpers
VOID                appendCodeObjectToHash(CONST CODEOBJECT*  pCodeObject, TCHAR*  szHash);
INT                 calculateWareEncoding(CONST GAME_OBJECT*  pGameObject);
DATA_TYPE           identifyDataTypeFromScriptObject(CONST SCRIPT_OBJECT*  pScriptObject);
VOID                setCodeEditLineError(HWND  hCodeEdit, CONST COMMAND*  pCommand);

// Functions
BOOL                appendParameterToXMLTree(SCRIPT_FILE*  pScriptFile, CONST COMMAND*  pCommand, CONST PARAMETER*  pParameter, XML_TREE*  pTree, XML_TREE_NODE*  pParent);
BOOL                findParameterSyntaxByLogicalIndex(CONST COMMAND_SYNTAX*  pSyntax, PARAMETER_INDEX*  pIndex, PARAMETER_SYNTAX&  eOutput);
BOOL                findPostfixParameterIndexByID(CONST COMMAND*  pCommand, CONST UINT  iID, UINT&  iOutput);
BOOL                identifyCommandFromHash(CONST SCRIPT_FILE*  pScriptFile, COMMAND*  pCommand, CONST COMMAND_HASH*  pCommandHash, ERROR_STACK*  &pError);
BOOL                identifyConditionalFromHash(COMMAND*  pCommand, CONST COMMAND_HASH*  pCommandHash, ERROR_STACK*  &pError);
BOOL                identifyExpressionHash(CONST TCHAR*  szHash);
BOOL                isStringConcatenatedLogicalOperator(CONST TCHAR*  szString);
BearScriptAPI VOID  performScriptCommandHashing(CONST TCHAR*  szScriptCommand, COMMAND_HASH*  pCommandHash);
BOOL                verifyParameterDataType(CONST PARAMETER*  pParameter);

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   SCRIPT GENERATION (JUMPS)
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Macros: Creation / Destruction
#define             createJumpStack()               createStack(destructorSimpleObject)
#define             deleteJumpStack(pStack)         deleteStack(pStack)

// Creation / Destruction
JUMP_STACK_ITEM*     createJumpStackItem(COMMAND*  pCommand, COMMAND*  pJumpCommand);
VOID                 deleteJumpStackItem(JUMP_STACK_ITEM*  pItem);

// Helpers
VOID              destroyJumpStackItemByIndex(STACK*  pStack, CONST UINT  iIndex);
BOOL              isIfConditional(CONST CONDITIONAL_ID   eConditional);
BOOL              isWhileConditional(CONST JUMP_STACK_ITEM*  pItem);
VOID              popJumpStack(STACK*  pStack);
VOID              pushJumpStack(STACK*  pStack, COMMAND*  pCommand, COMMAND*  pJumpCommand);
JUMP_STACK_ITEM*  topJumpStack(CONST STACK*  pStack);

// Functions
BOOL              appendEndCommandToScriptFile(HWND  hCodeEdit, SCRIPT_FILE*  pScriptFile, STACK*  pJumpStack, COMMAND*  pEndCommand, ERROR_STACK*  &pError);
BOOL              findLastConditionalInJumpStack(STACK*  pStack, CONST UINT  eType, JUMP_STACK_ITEM*  &pOutput);
BOOL              findLabelInJumpStack(STACK*  pStack, JUMP_STACK_ITEM*  &pOutput);
BOOL              findLabelInJumpStackByName(STACK*  pLabelStack, CONST TCHAR*  szLabelName, JUMP_STACK_ITEM*  &pOutput);
BOOL              generateBranchingCommandLogic(HWND  hCodeEdit, SCRIPT_FILE*  pScriptFile, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue);
BOOL              generateLabelStackItem(STACK*  pLabelStack, COMMAND*  pCommand, ERROR_STACK*  &pError);
BOOL              generateSubroutineCommandLogic(HWND  hCodeEdit, SCRIPT_FILE*  pScriptFile, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue);

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                      SCRIPT GENERATION
/// /////////////////////////////////////////////////////////////////////////////////////////

// Helpers
BOOL                validateExpressionParameters(CONST COMMAND*  pCommand, ERROR_STACK*  &pError);
BOOL                validateScriptProperties(CONST SCRIPT_FILE*  pScriptFile, ERROR_QUEUE*  pErrorQueue);
BOOL                validateSkipIfCommands(CONST SCRIPT_FILE*  pScriptFile, ERROR_QUEUE*  pErrorQueue);
BOOL                validateVariableUsage(CONST SCRIPT_FILE*  pScriptFile, ERROR_QUEUE*  pErrorQueue);

// Functions
BearScriptAPI BOOL  generateCommandFromString(CONST SCRIPT_FILE*  pScriptFile, CONST TCHAR*  szCommandText, CONST UINT  iLineNumber, COMMAND*  &pOutput, ERROR_QUEUE*  pErrorQueue);
BOOL                generateParameterFromCodeObject(CONST SCRIPT_FILE*  pScriptFile, CONST CODEOBJECT*  pCodeObject, CONST PARAMETER_SYNTAX  eSyntax, CONST UINT  iLineNumber, PARAMETER*  &pOutput, ERROR_STACK*  &pError);
BOOL                generateParameters(CONST SCRIPT_FILE*  pScriptFile, COMMAND*  pCommand, ERROR_STACK*  &pError);
BOOL                generateParametersForDefineArray(CONST SCRIPT_FILE*  pScriptFile, COMMAND*  pCommand, CODEOBJECT*  pCodeObject, PARAMETER_INDEX*  pParameterIndex, ERROR_STACK*  &pError);
BOOL                generateParametersForExpression(COMMAND*  pCommand, ERROR_STACK*  &pError);
BOOL                generateParametersForScriptCall(CONST SCRIPT_FILE*  pScriptFile, COMMAND*  pCommand, CODEOBJECT*  pCodeObject, PARAMETER_INDEX*  pParameterIndex, ERROR_STACK*  &pError);
BOOL                generateParametersSpecialCases(CONST SCRIPT_FILE*  pScriptFile, COMMAND*  pCommand, ERROR_STACK*  &pError);
BearScriptAPI BOOL  generateScript(HWND  hCodeEdit, SCRIPT_FILE*  pScriptFile, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue);
BOOL                generateScriptCommands(HWND  hCodeEdit, SCRIPT_FILE*  pScriptFile, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE* pErrorQueue);
BOOL                generateScriptFromOutputTree(SCRIPT_FILE*  pScriptFile);
BOOL                generateOutputTree(HWND  hCodeEdit, SCRIPT_FILE*  pScriptFile, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue);
BOOL                generateOutputTreeDataSection(HWND  hCodeEdit, CONST SCRIPT_FILE*  pScriptFile, XML_TREE*  pTree, XML_SCRIPT_LAYOUT*  pLayout);
BOOL                generateOutputTreeSourceText(HWND  hCodeEdit, CONST SCRIPT_FILE*  pScriptFile, XML_TREE*  pTree, XML_SCRIPT_LAYOUT*  pLayout);
BOOL                generateOutputTreeStandardCommands(SCRIPT_FILE*  pScriptFile, OPERATION_PROGRESS*  pProgress, XML_TREE*  pTree, XML_SCRIPT_LAYOUT*  pLayout);
BOOL                generateOutputTreeAuxiliaryCommands(SCRIPT_FILE*  pScriptFile, OPERATION_PROGRESS*  pProgress, XML_TREE*  pTree, XML_SCRIPT_LAYOUT*  pLayout);

// Thread functions
BearScriptAPI DWORD  threadprocSaveScriptFile(VOID*  pThreadParameter);

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                      SCRIPT VALIDATION
/// /////////////////////////////////////////////////////////////////////////////////////////

// FUnctions
BOOL                  validateScriptFileProperties(CONST SCRIPT_FILE*  pValidationFile, CONST SCRIPT_FILE*  pScriptFile, ERROR_QUEUE*  pErrorQueue);
OPERATION_RESULT      validateScriptFile(CONST SCRIPT_FILE*  pValidationFile, CONST SCRIPT_FILE*  pScriptFile, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue);
OPERATION_RESULT      validateScriptFileByText(CONST SCRIPT_FILE*  pValidationFile, CONST SCRIPT_FILE*  pScriptFile, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue);

// Thread Functions
BearScriptAPI DWORD   threadprocValidateScriptFile(VOID*  pParameter);


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    STRING CONVERSION
/// /////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
STRING_CONVERTER*  createStringConverter(CONST TCHAR*  szString);
VOID               deleteStringConverter(STRING_CONVERTER*  &pStringConverter);

// Helpers
BOOL               hasSpecialPhrases(CONST TCHAR*  szString);

// Functions
BOOL                    findNextSpecialPhrase(STRING_CONVERTER*  pStringConverter);
SPECIAL_CHARACTER_TYPE  identifySpecialPhrase(CONST TCHAR*  szString);
BearScriptAPI BOOL      generateConvertedString(CONST TCHAR*  szInput, CONST UINT  iFlags, TCHAR*  &szOutput);

/// Macros:
//#define           convertScriptStringExternalToDisplay(szInput, szOutput)        generateConvertedString((szInput), SPC_SCRIPT_EXTERNAL_TO_DISPLAY, (szOutput))
//#define           convertScriptStringDisplayToExternal(szInput, szOutput)        generateConvertedString((szInput), SPC_SCRIPT_DISPLAY_TO_EXTERNAL, (szOutput))

//#define           convertLanguageStringExternalToInternal(szInput, szOutput)     generateConvertedString((szInput), SPC_LANGUAGE_EXTERNAL_TO_INTERNAL, (szOutput))
//#define           convertLanguageStringInternalToDisplay(szInput, szOutput)      generateConvertedString((szInput), SPC_LANGUAGE_INTERNAL_TO_DISPLAY, (szOutput))

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                       TEXT STREAM
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
TEXT_STREAM*  createTextStream(CONST UINT  iInitialSize);
VOID          deleteTextStream(TEXT_STREAM*  pTextStream);

// Helpers
TCHAR*        getTextStreamBuffer(CONST TEXT_STREAM*  pTextStream);

// Functions
VOID          appendCharToTextStream(TEXT_STREAM*  pTextStream, CONST TCHAR  chCharacter);
VOID          appendStringToTextStreamf(TEXT_STREAM*  pTextStream, CONST TCHAR*  szFormat, ...);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    THREAD LISTS
/// ////////////////////////////////////////////////////////////////////////////////////////

/// Macros: Creation / Destruction
#define                       deleteCallStack(pCallStack)        deleteStack((STACK*&)pCallStack)
#define                       deleteFunctionCall(pFunctionCall)  utilDeleteObject((FUNCTION_CALL*&)pFunctionCall)

// Creation / Destruction
CALL_STACK*                    createCallStack(CONST DWORD  dwThreadID);
BearScriptAPI CALL_STACK_LIST* createCallStackList();
FUNCTION_CALL*                 createFunctionCall(CONST TCHAR*  szFileName, CONST TCHAR*  szFunction, CONST UINT  iLineNumber);
BearScriptAPI VOID             deleteCallStackList(CALL_STACK_LIST*  &pCallStackList);
BearScriptAPI VOID             destructorCallStack(LPARAM  pCallStack);
BearScriptAPI VOID             destructorFunctionCall(LPARAM  pFunctionCall);

// Helpers
BOOL                           findCallStackByThread(CONST CALL_STACK_LIST*  pCallStackList, CONST DWORD  dwThreadID, CALL_STACK*  &pOutput);

// Functions
BearScriptAPI VOID             destroyTopFunctionCallByThread(CALL_STACK_LIST*  pCallStackList, CONST DWORD  dwThreadID);
BearScriptAPI TCHAR*           generateCallStackStringByThread(CALL_STACK_LIST*  pCallStackList, CONST DWORD  dwThreadID);
BearScriptAPI VOID             pushFunctionCallByThread(CALL_STACK_LIST*  pCallStackList, CONST DWORD  dwThreadID, CONST TCHAR*  szFileName, CONST TCHAR*  szFunction, CONST UINT  iLineNumber);
BearScriptAPI FUNCTION_CALL*   popFunctionCallByThread(CALL_STACK_LIST*  pCallStackList, CONST DWORD  dwThreadID);
BearScriptAPI FUNCTION_CALL*   topFunctionCallByThread(CALL_STACK_LIST*  pCallStackList, CONST DWORD  dwThreadID);

/// Macros: Tracking
#define  TRACK_FUNCTION()     pushFunctionCallByThread(getAppCallStackList(), GetCurrentThreadId(), __SWFILE__, __WFUNCTION__, __LINE__)
#define  END_TRACKING()       destroyTopFunctionCallByThread(getAppCallStackList(), GetCurrentThreadId())

/// Macros: Reporting
#define  GET_LAST_FUNCTION()  topFunctionCallByThread(getAppCallStackList(), GetCurrentThreadId())
#define  FLATTEN_CALL_STACK() generateCallStackStringByThread(getAppCallStackList(), GetCurrentThreadId())

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                         TREES
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
BearScriptAPI AVL_TREE*      createAVLTree(AVL_NODE_EXTRACTOR  pfnExtractValue, DESTRUCTOR  pfnDeleteNode, AVL_TREE_KEY*  pPrimaryKey, AVL_TREE_KEY*  pSecondaryKey, AVL_TREE_KEY*  pTertiaryKey);
AVL_TREE_NODE*               createAVLTreeNode(AVL_TREE*  pTree, AVL_TREE_NODE*  pParent, LPARAM  pData);
BearScriptAPI AVL_TREE_KEY*  createAVLTreeSortKey(CONST AVL_TREE_SORT_KEY  eSorting, CONST AVL_TREE_ORDERING  eDirection);
BearScriptAPI AVL_TREE_KEY*  createAVLTreeSortKeyEx(CONST AVL_TREE_SORT_KEY  eSorting, CONST AVL_TREE_ORDERING  eDirection, CONST AVL_TREE_PROPERTY_TYPE  eType);
BearScriptAPI AVL_TREE*      duplicateAVLTree(AVL_TREE*  pTree, AVL_TREE_KEY*  pNewPrimaryKey, AVL_TREE_KEY*  pNewSecondaryKey, AVL_TREE_KEY*  pNewTertiaryKey);

BearScriptAPI VOID   deleteAVLTree(AVL_TREE*  &pTree);
BearScriptAPI VOID   deleteAVLTreeContents(AVL_TREE*  &pTree);
BearScriptAPI VOID   deleteAVLTreeNode(AVL_TREE*  pTree, AVL_TREE_NODE*  &pNode, CONST BOOL  bContents);
#define             deleteAVLTreeSortKey(eSortKey)      utilDeleteObject(eSortKey)

// Helpers
VOID                 attachAVLTreeNodeToParent(AVL_TREE*  pTree, AVL_TREE_NODE*  pParent, AVL_TREE_NODE*  pNode, CONST AVL_TREE_LINKAGE  eLinkage);
INT                  calculateAVLTreeNodeBalance(CONST AVL_TREE_NODE*  pNode);
UINT                 calculateAVLTreeNodeHeight(AVL_TREE_NODE*  pNode);
VOID                 detatchAVLTreeNode(AVL_TREE_NODE*  pNode);
VOID                 ensureSubTreeIsBalanced(AVL_TREE*  pTree, AVL_TREE_NODE*  pSubTree, CONST AVL_TREE_CHANGE  eCause);
BOOL                 findSubTreeMaximum(AVL_TREE_NODE*  pSubTree, AVL_TREE_NODE*  &pOutput);
BOOL                 findSubTreeMinimum(AVL_TREE_NODE*  pSubTree, AVL_TREE_NODE*  &pOutput);
BearScriptAPI UINT   getTreeNodeCount(CONST AVL_TREE*  pTree);
AVL_TREE_LINKAGE     identifyAVLTreeNodeLinkage(CONST AVL_TREE_NODE*  pNode);
AVL_TREE_PROPERTY_TYPE identifyAVLTreePropertyType(CONST AVL_TREE_SORT_KEY  eProperty);
VOID                 swapAVLTreeNode(AVL_TREE_NODE*  pFirstNode, AVL_TREE_NODE*  pSecondNode);
BearScriptAPI VOID   transferAVLTree(AVL_TREE*  &pSource, AVL_TREE*  &pDestination);

// Functions
COMPARISON_RESULT    compareAVLTreeNodeWithValues(CONST AVL_TREE*  pTree, CONST LPARAM  pNodeData, LPARAM  xValue1, LPARAM  xValue2, LPARAM  xValue3);
BearScriptAPI BOOL   destroyObjectInAVLTreeByIndex(AVL_TREE*  pTree, CONST UINT  iIndex);
BearScriptAPI BOOL   destroyObjectInAVLTreeByValue(AVL_TREE*  pTree, LPARAM  pPrimaryValue, LPARAM  pSecondaryValue);
BearScriptAPI BOOL   findObjectInAVLTreeByValue(CONST AVL_TREE*  pTree, LPARAM  pPrimaryValue, LPARAM  pSecondaryValue, LPARAM&  pOutput, AVL_TREE_NODE*  pCurrentNode = NULL);
BearScriptAPI BOOL   findObjectInAVLTreeByIndex(CONST AVL_TREE*  pTree, CONST UINT  iIndex, LPARAM &pOutput, AVL_TREE_NODE*  pCurrentNode = NULL);
BearScriptAPI BOOL   insertObjectIntoAVLTree(AVL_TREE*  pTree, LPARAM  pNewObject, AVL_TREE_NODE*  pCurrentNode = NULL);
VOID                 performAVLTreeLeftRotation(AVL_TREE*  pTree, AVL_TREE_NODE*  pSubTree);
VOID                 performAVLTreeRightRotation(AVL_TREE*  pTree, AVL_TREE_NODE*  pSubTree);
BearScriptAPI BOOL   removeObjectFromAVLTreeByIndex(AVL_TREE*  pTree, CONST UINT  iIndex, LPARAM&  pOutput, AVL_TREE_NODE*  pCurrentNode = NULL);
BearScriptAPI BOOL   removeObjectFromAVLTreeByValue(AVL_TREE*  pTree, LPARAM  pPrimaryValue, LPARAM  pSecondaryValue, LPARAM&  pOutput, AVL_TREE_NODE*  pCurrentNode = NULL);
VOID                 removeAVLTreePredecessorNode(AVL_TREE*  pTree, AVL_TREE_NODE*  pSubTree, AVL_TREE_NODE*  pCurrentNode = NULL);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                       TREES (ADVANCED)
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation
BearScriptAPI AVL_TREE_GROUP_COUNTER*  createAVLTreeGroupCounter(CONST UINT  iMaxGroups);
BearScriptAPI AVL_TREE_OPERATION*      createAVLTreeOperation(CONST AVL_TREE_FUNCTOR  pfnFunctor, CONST AVL_TREE_TRAVERSAL  eOrder);
BearScriptAPI AVL_TREE_OPERATION*      createAVLTreeOperationEx(CONST AVL_TREE_FUNCTOR  pfnFunctor, CONST AVL_TREE_TRAVERSAL  eOrder, QUEUE*  pErrorQueue, OPERATION_PROGRESS*  pProgress);
BearScriptAPI AVL_TREE_OPERATION*      createAVLTreeThreadedOperation(CONST AVL_TREE_FUNCTOR  pfnFunctor, CONST AVL_TREE_TRAVERSAL  eOrder, OPERATION_PROGRESS*  pProgress, CONST UINT  iNodesPerThread);

// Destruction
BearScriptAPI VOID      deleteAVLTreeGroupCounter(AVL_TREE_GROUP_COUNTER*  &pGroupCounter);
BearScriptAPI VOID      deleteAVLTreeOperation(AVL_TREE_OPERATION*  &pOperation);

// Helpers
BearScriptAPI UINT      getAVLTreeGroupCount(CONST AVL_TREE_GROUP_COUNTER*  pGroupCounter, CONST UINT  iGroup);
BearScriptAPI VOID      incrementAVLTreeGroupCount(AVL_TREE_GROUP_COUNTER*  pGroupCounter, CONST UINT  iGroup);

// Functions
BearScriptAPI TCHAR*    analyseTreeBalance(CONST AVL_TREE_NODE*  pNode, CONST UINT  iDepth = NULL);
BearScriptAPI VOID      performAVLTreeIndexing(AVL_TREE*  pTree);
BearScriptAPI VOID      performAVLTreeGroupCount(CONST AVL_TREE*  pTree, AVL_TREE_GROUP_COUNTER*  pGroupCounter, AVL_TREE_SORT_KEY  eProperty);
BearScriptAPI BOOL      performOperationOnAVLTree(CONST AVL_TREE*  pTree, AVL_TREE_OPERATION*  pOperationData);
BOOL                    performThreadedOperationOnAVLTree(CONST AVL_TREE*  pTree, AVL_TREE_OPERATION*  pOperationData);

// Operations
VOID                    treeprocIndexAVLTreeNode(AVL_TREE_NODE*  pNode, AVL_TREE_OPERATION*  pData);
BearScriptAPI VOID      treeprocReplicateAVLTreeNode(AVL_TREE_NODE*  pNode, AVL_TREE_OPERATION*  pData);
VOID                    treeprocGenerateAVLTreeNodeGroupCounts(AVL_TREE_NODE*  pNode, AVL_TREE_OPERATION*  pOperationData);
DWORD                   threadprocAVLSubTreeOperation(VOID*  pParameter);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                   VARIABLE DEPENDENCIES
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
BearScriptAPI VARIABLE_DEPENDENCY*  createVariableDependency(CONST TCHAR*  szVariableName, CONST VARIABLE_TYPE  eType, CONST VARIABLE_USAGE  eUsage);
BearScriptAPI AVL_TREE*             createVariableDependencyTreeByText();
VOID                                deleteVariableDependencyNode(LPARAM  pVariableDependency);

// Helpers
LPARAM              extractVariableDependencyNode(LPARAM  pNodeData, CONST AVL_TREE_SORT_KEY  eProperty);

// Functions
BearScriptAPI VOID  insertVariableDependencyIntoAVLTree(AVL_TREE*  pTree, CONST TCHAR*  szVariableName, CONST VARIABLE_TYPE  eType, CONST VARIABLE_USAGE  eUsage);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    VARIABLE NAMES
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
BearScriptAPI VARIABLE_NAME*  createVariableName(CONST TCHAR*  szName, CONST UINT  iID);
BearScriptAPI AVL_TREE*       createVariableNameTreeByID();
BearScriptAPI AVL_TREE*       createVariableNameTreeByText();
BearScriptAPI VARIABLE_NAME*  duplicateVariableName(CONST VARIABLE_NAME*  pSource);
VOID                          deleteVariableName(VARIABLE_NAME*  &pVariableName);
BearScriptAPI VOID            deleteVariableNameTreeNode(LPARAM  pVariableName);

// Helpers
BearScriptAPI LPARAM          extractVariableNameTreeNode(LPARAM  pNode, CONST AVL_TREE_SORT_KEY  eProperty);
BearScriptAPI CONST TCHAR*    identifyRawVariableName(CONST TCHAR*  szVariableName);

// Funtions
BearScriptAPI BOOL            insertVariableNameIntoAVLTree(AVL_TREE*  pTree, CONST TCHAR*  szVariable, CONST UINT  iID);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    VIRTUAL FILES
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
FILE_ITEM*          createFileItemFromPhysical(CONST TCHAR*  szFolder, CONST WIN32_FIND_DATA*  pFileData);
FILE_ITEM*          createFileItemFromVirtualFile(CONST VIRTUAL_FILE*  pVirtualFile);
VIRTUAL_FILE*       createVirtualFile(CONST UINT  iCatalogueID, CHAR*  szFilePathA, UINT  iDataOffset, CONST UINT  iPackedSize);
VIRTUAL_FILE*       createVirtualFolder(CHAR*  szFolderPathA);
AVL_TREE*           createVirtualFileTreeByPath();
VOID                deleteFileItem(FILE_ITEM*  &pFileItem);
VOID                deleteFileItemTreeNode(LPARAM  pData);
VOID                deleteVirtualFile(VIRTUAL_FILE*  &pVirtualFile);
VOID                deleteVirtualFileTreeNode(LPARAM  pData);

// Helpers
LPARAM              extractFileItemTreeNode(LPARAM  pNode, CONST AVL_TREE_SORT_KEY  eProperty);
LPARAM              extractVirtualFileTreeNode(LPARAM  pNode, CONST AVL_TREE_SORT_KEY  eProperty);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    VIRTUAL FILE SYSTEM
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
BearScriptAPI AVL_TREE*  createFileItemTree(CONST AVL_TREE_SORT_KEY  eSortKey, CONST AVL_TREE_ORDERING  eDirection);
FILE_SEARCH*             createFileSearch(CONST AVL_TREE_SORT_KEY  eSortKey, CONST AVL_TREE_ORDERING  eDirection);
FILE_SYSTEM*             createFileSystem(CONST TCHAR*  szGameFolder, ERROR_STACK*  &pError);
BearScriptAPI VOID       deleteFileSearch(FILE_SEARCH*  &pFileSearch);
VOID                     deleteFileSystem(FILE_SYSTEM*  &pFileSystem);

// Helpers
BearScriptAPI BOOL  findFileSearchResultByIndex(CONST FILE_SEARCH*  pFileSearch, CONST UINT  iIndex, FILE_ITEM*  &pOutput);
BearScriptAPI BOOL  findNextFileSearchResult(FILE_SEARCH*  pFileSearch, FILE_ITEM*  &pOutput);
BearScriptAPI BOOL  findVirtualFileByPath(CONST FILE_SYSTEM*  pFileSystem, CONST TCHAR*  szFullPath, VIRTUAL_FILE*  &pOutput);
BearScriptAPI UINT  getFileSearchResultCount(CONST FILE_SEARCH*  pFileSearch);
BearScriptAPI BOOL  isCatalogueFilePresent(CONST FILE_SYSTEM*  pFileSystem, CONST TCHAR*  szFullPath);
BearScriptAPI BOOL  isFilePresent(CONST FILE_SYSTEM*  pFileSystem, CONST TCHAR*  szFullPath);
BOOL                isPhysicalFilePresent(TCHAR*  szFullPath, CONST TCHAR*  szAlternateExtension);

// Functions
UINT                         enumerateVirtualFileSystem(FILE_SYSTEM*  pFileSystem, OPERATION_PROGRESS*  pProgress, ERROR_STACK*  &pError);
UINT                         enumerateVirtualFileSystemCatalogues(CONST FILE_SYSTEM*  pFileSystem, CONST GAME_VERSION  eGameVersion);
BearScriptAPI FILE_ITEM_FLAG identifyFileType(CONST FILE_SYSTEM*  pFileSystem, CONST TCHAR*  szFullPath);
OPERATION_RESULT             loadFileSystem(CONST TCHAR*  szGameFolder, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue);
BearScriptAPI UINT           loadGameFileFromFileSystemByPath(CONST FILE_SYSTEM*  pFileSystem, GAME_FILE*  pOutput, CONST TCHAR*  szAlternateExtension, ERROR_QUEUE*  pErrorQueue);
BearScriptAPI UINT           loadRawFileFromFileSystemByPath(CONST FILE_SYSTEM*  pFileSystem, TCHAR*  szFullPath, CONST TCHAR*  szAlternateExtension, RAW_FILE*  &szOutput, ERROR_QUEUE*  pErrorQueue);
BearScriptAPI VOID           performFileSearchSort(FILE_SEARCH*  pFileSearch, CONST AVL_TREE_SORT_KEY  eSortBy, CONST AVL_TREE_ORDERING  eDirection);
BearScriptAPI BOOL           performFileSystemCopy(CONST FILE_SYSTEM*  pFileSystem, CONST TCHAR*  szFileSubPath, ERROR_QUEUE*  pErrorQueue);
BearScriptAPI FILE_SEARCH*   performFileSystemSearch(CONST FILE_SYSTEM*  pFileSystem, CONST TCHAR*  szFolder, CONST FILE_FILTER  eFilter, CONST AVL_TREE_SORT_KEY  eSortBy, CONST AVL_TREE_ORDERING  eDirection, OPERATION_PROGRESS*  pProgress);
UINT                         saveDocumentToFileSystem(CONST TCHAR*  szFullPath, GAME_FILE*  pOutput, ERROR_QUEUE*  pErrorQueue);
UINT                         saveDocumentToFileSystemEx(CONST TCHAR*  szFullPath, GAME_FILE*  pOutput, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue);
UINT                         saveRawFileToFileSystem(CONST TCHAR*  szFullPath, CONST RAW_FILE*  szInput, CONST UINT  iInputLength, ERROR_QUEUE*  pErrorQueue);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                VIRTUAL FILE SYSTEM (BASE)
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
CATALOGUE_FILE*  createCatalogueFile(CONST UINT  iID, CONST TCHAR*  szFullPath);
RAW_FILE*        createRawFileBuffer(CONST UINT  iBytes);
VOID             deleteCatalogueFile(CATALOGUE_FILE*  &pCatalogue);
VOID             destructorCatalogue(LPARAM  pCatalogue);
VOID             deleteRawFileBuffer(RAW_FILE*  &pFileBuffer);

// Helpers
CONST TCHAR*         identifyFileFilterSearchTerm(CONST FILE_FILTER  eFilter);
BOOL                 identifyVirtualSubFolderName(CONST TCHAR*  szSubPath, TCHAR*  szOutput, CONST UINT  iOutputLength);
BearScriptAPI BOOL   isPathSubFolderOf(CONST TCHAR*  szParentFolder, CONST TCHAR*  szPath);
BOOL                 isVirtualFileInExactSubFolder(CONST TCHAR*  szFolderSubPath, CONST TCHAR*  szFileSubPath);

// Functions
FILE_ITEM_FLAG   identifyFileTypeFromBuffer(RAW_FILE*  szFileBuffer, UINT  &iFileSize);
VOID             identifyFileTypeAndProperties(CONST FILE_SYSTEM*  pFileSystem, FILE_ITEM*  pFileItem);
UINT             loadCatalogueFile(CONST TCHAR*  szFullPath, RAW_FILE*  &szOutput);
BOOL             loadRawFileFromUnderlyingFileSystemByPath(CONST TCHAR*  szFullPath, CONST VIRTUAL_FILE*  pVirtualFile, RAW_FILE*  &szOutput, ERROR_QUEUE*  pErrorQueue);
VOID             performCatalogueFileDecryption(CONST TCHAR*  szFileName, RAW_FILE  *szInput, CONST UINT  iInputLength);
UINT             performRawFileDecryption(CONST TCHAR*  szFileName, RAW_FILE*  &szInputOutput, CONST UINT  iInputSize, CONST FILE_ITEM_FLAG  eFileSource, ERROR_QUEUE*  pErrorQueue);
UINT             performGZipFileCompression(CONST TCHAR*  szFileName, RAW_FILE*  szInput, CONST UINT  iInputSize, RAW_FILE*  &szOutput, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue);
UINT             performGZipFileDecompression(RAW_FILE*  szInput, CONST UINT  iInputSize, RAW_FILE*  &szOutput, ERROR_QUEUE*  pErrorQueue);
BOOL             performFileItemFilterMatch(CONST FILE_ITEM*  pFileItem, CONST FILE_FILTER  eFilter);
VOID             performPhysicalFileSystemSearch(CONST FILE_SYSTEM*  pFileSystem, CONST TCHAR*  szFolder, CONST FILE_FILTER  eFilter, OPERATION_PROGRESS*  pProgress, FILE_SEARCH*  pOutput);
VOID             performVirtualFileSystemSearch(CONST FILE_SYSTEM*  pFileSystem, CONST TCHAR*  szFolder, CONST FILE_FILTER  eFilter, OPERATION_PROGRESS*  pProgress, FILE_SEARCH*  pOutput);

// Tree Operations
VOID             treeprocSearchPhysicalFileSystem(AVL_TREE_NODE*  pCurrentNode, AVL_TREE_OPERATION*  pOperationData);
VOID             treeprocSearchVirtualFileSystem(AVL_TREE_NODE*  pCurrentNode, AVL_TREE_OPERATION*  pOperationData);

// Thread Functions
BearScriptAPI DWORD   threadprocFileSystemSearch(VOID*  pParameter);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                          XML GENERATION
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
XML_TREE_NODE*   createSourceValueNodeFromString(XML_TREE_NODE*  pParent, CONST TCHAR*  szValue);
XML_TREE_NODE*   createSourceValueNodeFromInteger(XML_TREE_NODE*  pParent, CONST SOURCE_VALUE_TYPE  eType, CONST INT  iValue);
XML_TREE_NODE*   createXMLTreeNodeFromString(XML_TREE_NODE*  pParent, CONST TCHAR*  szName, CONST TCHAR*  szText);
XML_TREE_NODE*   createXMLTreeNodeFromInteger(XML_TREE_NODE*  pParent, CONST TCHAR*  szName, CONST INT  iValue);
XML_PROPERTY*    createXMLPropertyFromString(CONST TCHAR*  szName, CONST TCHAR*  szValue);
XML_PROPERTY*    createXMLPropertyFromInteger(CONST TCHAR*  szName, CONST INT  iValue);

// Helpers
XML_TREE_NODE*  appendStringNodeToXMLTree(XML_TREE*  pTree, XML_TREE_NODE*  pParent, CONST TCHAR*  szName, CONST TCHAR*  szValue);
XML_TREE_NODE*  appendIntegerNodeToXMLTree(XML_TREE*  pTree, XML_TREE_NODE*  pParent, CONST TCHAR*  szName, CONST INT  iValue);
XML_TREE_NODE*  appendSourceValueStringNodeToXMLTree(XML_TREE*  pTree, XML_TREE_NODE*  pParent, CONST TCHAR*  szValue);
XML_TREE_NODE*  appendSourceValueIntegerNodeToXMLTree(XML_TREE*  pTree, XML_TREE_NODE*  pParent, CONST INT  iValue);
XML_TREE_NODE*  appendSourceValueArrayNodeToXMLTree(XML_TREE*  pTree, XML_TREE_NODE*  pParent, CONST UINT  iSize);
BOOL            findXMLPropertyByIndex(CONST XML_TREE_NODE*  pNode, CONST UINT  iIndex, XML_PROPERTY*  &pOutput);
BOOL            setXMLPropertyInteger(XML_TREE_NODE*  pNode, CONST TCHAR*  szProperty, CONST INT  iValue);

// Functions
BOOL            generateTextStreamFromXMLTree(CONST XML_TREE*  pTree, TEXT_STREAM*  pOutput);
VOID            generateTextStreamFromXMLTreeNode(CONST XML_TREE_NODE*  pCurrentNode, TEXT_STREAM*  pOutput, CONST UINT  iIndentation);


/// ////////////////////////////////////////////////////////////////////////////////////////
///                                          XML PARSER
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
XML_PROPERTY*   createXMLProperty(CONST TCHAR*  szName);
XML_TAG_STACK*  createXMLTagStack();
XML_TOKENISER*  createXMLTokeniser(CONST TCHAR*  szInput);
XML_TREE_NODE*  createXMLTreeNode(XML_TREE_NODE*  pParent, CONST XML_TOKENISER*  pToken);
XML_TREE_NODE*  createXMLTreeRoot();
VOID            deleteXMLProperty(XML_PROPERTY*  &pProperty);
VOID            destructorXMLProperty(LPARAM  pListItem);
VOID            deleteXMLTagStack(XML_TAG_STACK*  &pStack);
VOID            deleteXMLTokeniser(XML_TOKENISER*  &pTokeniser);
VOID            deleteXMLTreeNode(XML_TREE_NODE*  &pNode);
VOID            destructorXMLTreeNode(LPARAM  pItemData);

// (Exported) Creation / Destruction
BearScriptAPI XML_TREE*   createXMLTree();
BearScriptAPI VOID        deleteXMLTree(XML_TREE*  &pTree);

// Helpers
TCHAR*         duplicateNodeText(CONST XML_TREE_NODE*  pNode);
XML_PROPERTY*  getXMLPropertyFromItem(CONST LIST_ITEM*  pListItem);
UINT           getXMLTreeItemCount(CONST XML_TREE*  pTree);
XML_TREE_NODE* getXMLTreeNodeFromItem(CONST LIST_ITEM*  pListItem);
LIST_ITEM*     getFirstXMLPropertyListItem(CONST XML_TREE_NODE*  pNode);
LIST_ITEM*     getFirstXMLTreeNodeListItem(CONST XML_TREE_NODE*  pNode);
BOOL           hasChildren(CONST XML_TREE_NODE*  pNode);
BOOL           hasItems(CONST XML_TAG_STACK*  pStack);
BOOL           hasProperties(CONST XML_TREE_NODE*  pNode);
VOID           pushXMLTagStack(XML_TAG_STACK*  pStack, CONST TCHAR*  szTag, CONST UINT  iTagLength);
VOID           popXMLTagStack(XML_TAG_STACK*  pStack);
CONST TCHAR*   topXMLTagStack(CONST XML_TAG_STACK*  pStack);

// (Exported) Helpers
BearScriptAPI UINT  getXMLNodeCount(CONST XML_TREE_NODE*  pNode);
BearScriptAPI BOOL  isStringNumeric(CONST TCHAR*  szString);
BearScriptAPI BOOL  isStringWhitespace(CONST TCHAR*  szString);

// Functions
VOID    appendChildToXMLTreeNode(XML_TREE*  pTree, XML_TREE_NODE*  pParent, XML_TREE_NODE*  pChild);
VOID    appendPropertyToXMLTreeNode(XML_TREE_NODE*  pNode, XML_PROPERTY*  pProperty);
UINT    calculateCharacterCountInString(CONST TCHAR*  szString, CONST TCHAR  chCharacter);
BOOL    findNextXMLToken(XML_TOKENISER*  &pOutput, ERROR_STACK*  &pError);
BOOL    getXMLPropertyInteger(CONST XML_TREE_NODE*  pNode, CONST TCHAR*  szProperty, INT&  iOutput);
UINT    performXMLCharacterEntityReplacement(TCHAR*  szInput, CONST UINT  iInputLength);
VOID    translateXMLTagIntoNode(CONST TCHAR*  szTag, CONST UINT  iLength, XML_TREE_NODE*  pOutput);

// (Exported) Functions
BearScriptAPI BOOL              findLastXMLTreeNode(CONST XML_TREE_NODE*  pNode, CONST XML_NODE_RELATIONSHIP  eRelationship, XML_TREE_NODE*  &pOutput);
BearScriptAPI BOOL              findNextXMLTreeNode(CONST XML_TREE_NODE*  pNode, CONST XML_NODE_RELATIONSHIP  eRelationship, XML_TREE_NODE*  &pOutput);
BearScriptAPI BOOL              findXMLTreeNodeByIndex(CONST XML_TREE_NODE*  pParentNode, CONST UINT  iIndex, XML_TREE_NODE*  &pOutput);
BearScriptAPI BOOL              findXMLTreeNodeByName(CONST XML_TREE_NODE*  pStartNode, CONST TCHAR*  szName, CONST BOOL  bRecurseChildren, XML_TREE_NODE*  &pOutput);
BearScriptAPI OPERATION_RESULT  generateXMLTree(CONST TCHAR*  szXML, CONST UINT  iInputLength, CONST TCHAR*  szFileName, HWND  hParentWnd, XML_TREE*  &pOutput, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue);
BearScriptAPI BOOL              getXMLPropertyString(CONST XML_TREE_NODE*  pNode, CONST TCHAR*  szProperty, CONST TCHAR*  &szOutput);
BearScriptAPI VOID              traverseXMLTree(XML_TREE_NODE*  pStartNode);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                          TESTING
/// ////////////////////////////////////////////////////////////////////////////////////////

BearScriptAPI VOID  testCompression();
BearScriptAPI VOID  testStrangePCKFile();
