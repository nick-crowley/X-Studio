//
// Commands.cpp  :  ____
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                              CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createCommandFromNode
// Description     : Creates a new COMMAND object from a CommandNode and ensures it has a valid ID, type and index.
// 
// CONST SCRIPT_FILE*   pScriptFile  : [in]  ScriptFile containing the CommandNode
// CONST COMMAND_NODE*  pCommandNode : [in]  CommandNode containing the data as an XMLTreeNode
// ERROR_STACK*        &pError       : [out] New error message if any, otherwise NULL. You are responsible for destroying it.
// 
// Return Value   : New COMMAND whether successful or not. You are responsible for destroying it.
// 
COMMAND*   createCommandFromNode(CONST SCRIPT_FILE*  pScriptFile, CONST COMMAND_NODE*  pCommandNode, ERROR_STACK*  &pError)
{
   COMMAND*  pCommand;  // New Command being created

   // Prepare
   pError = NULL;

   // Create new command
   pCommand = utilCreateEmptyObject(COMMAND);

   // Set properties
   pCommand->iFlags      = pCommandNode->eType;
   pCommand->iLineNumber = pCommandNode->pNode->iLineNumber;
   pCommand->pErrorQueue = createErrorQueue();

   // Create PARAMETER arrays
   pCommand->pParameterArray        = createParameterArray(10);
   pCommand->pPostfixParameterArray = createParameterArray(10);
   pCommand->pInfixParameterArray   = createParameterArray(10);

   /// Extract the CommandID (Standard+Auxiliary) and ReferenceIndex (Auxiliary only)
   if (!getCommandNodeID(pCommandNode, pCommand->iID, pError) OR (pCommandNode->eType == CT_AUXILIARY AND !getCommandNodeIndex(pCommandNode, pCommand->iReferenceIndex, pError)))
      // [ERROR] "Cannot locate the command ID and reference index necessary to define the command"
      pError = generateDualError(HERE(IDS_SCRIPT_COMMAND_ID_INDEX_MISSING));
   
   /// Lookup the CommandSyntax
   else if (findCommandSyntaxByID(pCommand->iID, pScriptFile->eGameVersion, pCommand->pSyntax, pError))
   {
      // [SUCCESS] Check for command comments
      if (isCommandID(pCommand, CMD_COMMAND_COMMENT))
      {
         /// [COMMAND COMMENT] Set type flag and Replace the CommandComment ID + syntax with the ID + syntax of the original (commented) command
         pCommand->iFlags |= CT_CMD_COMMENT;

         // Lookup the original command ID and syntax
         if (!getCommandNodeCommentID(pCommandNode, pCommand->iID, pError) OR !findCommandSyntaxByID(pCommand->iID, pScriptFile->eGameVersion, pCommand->pSyntax, pError))
            // [ERROR] "Either the original command (ID:%03u) contained within the commented command cannot be found or is unrecognised"
            enhanceError(pError, HERE(IDS_SCRIPT_COMMAND_COMMENT_INVALID), pCommand->iID);
      }

      /// [EXPRESSION] Add the 'Expression' command type flag
      if (isCommandID(pCommand, CMD_EXPRESSION))
         pCommand->iFlags |= CT_EXPRESSION;

      /// [SCRIPT-CALL] Add the 'ScriptCall' command type flag
      else if (isCommandScriptCall(pCommand))
         pCommand->iFlags |= CT_SCRIPTCALL;
   }
   // else
   //    [FAILED] No enhancement necessary

   /// Set result to SUCCESS if there were no errors (ie. a recognised command syntax)
   pCommand->eResult = (pError == NULL ? OR_SUCCESS : OR_FAILURE);

   // Return new command
   return pCommand;
}


/// Function name  : createCommandFromText
// Description     : Create an empty COMMAND object to represent a script command
// 
// CONST TCHAR*  szCommandText : [in] Text of the script command
// CONST UINT    iLineNumber   : [in] Zero-based line number holding the COMMAND [Used for error reporting only]
// 
// Return Value   : New COMMAND object, you are responsible for destroying it
// 
BearScriptAPI
COMMAND*  createCommandFromText(CONST TCHAR*  szCommandText, CONST UINT  iLineNumber)
{
   COMMAND*    pCommand;      // Command being created

   // Create new COMMAND
   pCommand = utilCreateEmptyObject(COMMAND);

   // Create error queue
   pCommand->pErrorQueue = createErrorQueue();

   // Create PARAMETER arrays
   pCommand->pParameterArray        = createParameterArray(10);
   pCommand->pPostfixParameterArray = createParameterArray(10);
   pCommand->pInfixParameterArray   = createParameterArray(10);

   // Set properties
   StringCchCopy(pCommand->szBuffer, LINE_LENGTH, szCommandText);
   pCommand->iLineNumber  = iLineNumber;
   pCommand->eConditional = CI_NONE;

   // Return new object
   return pCommand;
}

/// Function name  : createCustomCommandf
// Description     : Create a COMMAND with the specified text and ID, used for creating virtual commands
// 
// CONST UINT    iID           : [in] Command ID
// CONST UINT    iFlags        : [in] Command type flags
// CONST UINT    iLineNumber   : [in] Zero-based line number for error reporting
// CONST TCHAR*  szCommandText : [in] Formatting string of command text
// ...           ...           : [in] Formatting arguments
// 
// Return Value   : New COMMAND object, you are responsible for destroying it
// 
COMMAND*  createCustomCommandf(CONST UINT  iID, CONST UINT  iFlags, CONST UINT  iLineNumber, CONST TCHAR*  szFormat, ...)
{
   ERROR_STACK*  pError;
   COMMAND*      pCommand;
   TCHAR*        szCommandText;

   // Prepare
   szCommandText = utilCreateStringVf(LINE_LENGTH, szFormat, utilGetFirstVariableArgument(&szFormat));

   /// Create command
   pCommand         = createCommandFromText(szCommandText, iLineNumber);
   pCommand->iFlags = CT_STANDARD;
   
   // [CHECK] Lookup syntax
   if (findCommandSyntaxByID(iID, GV_THREAT, pCommand->pSyntax, pError))
   {
      /// [SUCCESS] Set properties
      pCommand->iID    = iID;
      pCommand->iFlags = iFlags;
   }
   else
   {
      // [FAILED] Print error to console
      consolePrintError(pError);
      deleteErrorStack(pError);
   }

   // Cleanup and return command
   utilDeleteString(szCommandText);
   return pCommand;
}


/// Function name  : createHiddenJumpCommand
// Description     : Creates a new COMMAND representing CMD_HIDDEN_JUMP, 
// 
// CONST UINT  iJumpDestination   : [in] 
// 
// Return Value   : 
// 
COMMAND*  createHiddenJumpCommand(CONST UINT  iJumpDestination)
{
   PARAMETER_INDEX*  pIndex;        // Parameter Indexing information
   COMMAND*          pCommand;      // New Command being created
   PARAMETER*        pParameter;    // PARAMETER containing the jump destination parameter

   // Create new COMMAND (with no line number)
   pCommand         = createCommandFromText(TEXT("hidden jump"), NULL);
   pCommand->iID    = CMD_HIDDEN_JUMP;
   pCommand->iFlags = CT_STANDARD;

   // Set jump destination
   pParameter = createParameterFromInteger(iJumpDestination, DT_INTEGER, PS_LABEL_NUMBER, NULL);

   // Attach Parameter to COMMAND at index zero
   pIndex = createParameterIndex(0, 0);
   insertParameterIntoCommand(pCommand, pIndex, pParameter);

   // Cleanup and return new Command
   deleteParameterIndex(pIndex);
   return pCommand;
}


/// Function name  : duplicateCommand
// Description     : Create a duplicate of a COMMAND and all it's PARAMETERS
// 
// CONST COMMAND*  pCommand   : [in] Command to duplicate
// 
// Return Value   : New COMMAND, you are responsible for destroying it
// 
BearScriptAPI 
COMMAND*  duplicateCommand(CONST COMMAND*  pCommand)
{
   COMMAND*  pCopy;

   // Create new COMMAND
   pCopy = utilCreateEmptyObject(COMMAND);

   // Copy all properties
   utilCopyObject(pCopy, pCommand, COMMAND);

   // Duplicate error queue
   pCopy->pErrorQueue = duplicateErrorQueue(pCommand->pErrorQueue);

   // Duplicate ParameterArrays
   pCopy->pParameterArray        = duplicateParameterArray(pCommand->pParameterArray);
   pCopy->pInfixParameterArray   = duplicateParameterArray(pCommand->pInfixParameterArray);
   pCopy->pPostfixParameterArray = duplicateParameterArray(pCommand->pPostfixParameterArray);

   // Return new object
   return pCopy;
}


/// Function name  : deleteCommand
// Description     : Destroy a COMMAND object
// 
// COMMAND*  &pCommand : [in] COMMAND to destroy
// 
BearScriptAPI
VOID   deleteCommand(COMMAND*  &pCommand)
{
   // Delete PARAMETER arrays
   deleteParameterArray(pCommand->pParameterArray);
   deleteParameterArray(pCommand->pInfixParameterArray);
   deleteParameterArray(pCommand->pPostfixParameterArray);

   // Delete error
   //if (pCommand->pError)
   //   deleteErrorStack(pCommand->pError);

   // Delete error queue
   deleteErrorQueue(pCommand->pErrorQueue);

   // Delete calling object
   utilDeleteObject(pCommand);
}


/// Function name  : destructorCommand
// Description     : Destroy a COMMAND list ListItem
// 
// LPARAM    pArgument : [in] ListItem data (COMMAND*)
// 
VOID  destructorCommand(LPARAM  pCommand)
{
   deleteCommand((COMMAND*&)pCommand);
}


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                         HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : identifyCommandTypeString
// Description     : Get the string representation of a command type
// 
// CONST COMMAND_TYPE  eType   : [in] Command Type to convert
// 
// Return Value   : String version of the command type
// 
BearScriptAPI
CONST TCHAR*  identifyCommandTypeString(CONST COMMAND_TYPE  eType)
{
   return ((UINT)eType INCLUDES CT_STANDARD ? TEXT("standard") : TEXT("auxiliary"));
}


/// Function name  : isCommandGameStringDependent
// Description     : Determine whether a command references a game string from it's ID
// 
// CONST COMMAND*  pCommand : [in] Command to test
// 
// Return Value   : TRUE or FALSE
// 
BearScriptAPI 
BOOL  isCommandGameStringDependent(CONST COMMAND*  pCommand)
{
   // Examine command ID
   switch (pCommand->iID)
   {
   /// [SINGLE ENTRY] {Page,String} = {0, 1}
   case CMD_SET_NEWS_ARTICLE_STATE:             // "set state of news article page=$0 id=$1 to $2"
   case CMD_WRITE_LOGBOOK:                      // "write to player logbook: printf: pageid=$0 textid=$1, $2, $3, $4, $5, $6"
   case CMD_WRITE_LOG:                          // "write to log file $0  append=$1  printf: pageid=$2 textid=$3, $4, $5, $6, $7, $8"
   /// [SINGLE ENTRY] {Page,String} = {1, 2}
   case CMD_READ_TEXT:                          // "$0 read text: page=$1 id=$2"
   case CMD_READ_TEXT_EXISTS:                   // "$0 read text: page id=$1, id=$2 exists"
   case CMD_SPEAK_TEXT:                         // "$0 speak text: page=$1 id=$2 priority=$3"
   case CMD_STRING_PRINTF:                      // "$0 sprintf: pageid=$1 textid=$2, $3, $4, $5, $6, $7"
   case CMD_DISPLAY_NEWS_ARTICLE:               // "$0 display news article: page=$1 newsid=$2 occurrences=$3 maxoffertime=$4 maxtime=$5 placeholder:race1=$6 race2=$7 customarray=$8"
   case CMD_GET_NEWS_ARTICLE_STATE:             // "$0 state of news article: page=$1 id=$2"
   case CMD_ADD_SCRIPT_OPTIONS:                 // "add script options: script=$0 pageid=$1 textid=$2"
   case CMD_REMOVE_PLAYER_GRAPH:                // "remove player graph: script=$0 group pageid=$1 group textid=$2"
   case CMD_REGISTER_HOTKEY_1_BYREF:            // "$0 register hotkey: page=$1 id=$2, to call script $3"
   case CMD_REGISTER_HOTKEY_2_BYREF:            // "$0 register hotkey: page=$1 id=$2, to call script name $3"
   case CMD_ADD_MENU_ITEM_BYREF:                // "add custom menu item to array $0: page=$1x id=$2y returnvalue=$3"
   case CMD_ADD_MENU_INFO_BYREF:                // "add custom menu info line to array $0: page=$1x id=$2y"
   case CMD_ADD_MENU_HEADING_BYREF:             // "add custom menu heading to array $0: page=$1x id=$2y"
   /// [SINGLE ENTRY] {Page,String} = {2, 3}
   case CMD_REMOVE_CLASS_GRAPH:                 // "remove class graph: class=$0 script=$1 group pageid=$2 group textid=$3"
   case CMD_REMOVE_OBJECT_GRAPH:                // "$0 remove object graph: script=$1 group pageid=$2 group textid=$3"
   /// [SINGLE ENTRY] {Page,String} = {3, 4} 
   case CMD_ADD_WING_SCRIPT:                    // "$0 add wing additional ship command: id=$1 script=$2 pageid=$3 textid=$4"
   case CMD_ADD_WING_SCRIPT_CHECK:              // "$0 add wing additional ship command: id=$1 script=$2 pageid=$3 textid=$4 checkscript=$5"
   /// [DUAL ENTRIES] {Page,String} = {1, 2} and {3, 4}
   case CMD_ADD_NEWS_ARTICLE:                   // "add encyclopedia custom article: News, id=$0, title page=$1, title id=$2, text page=$3, text id=$4, timeout=$5"
   case CMD_ADD_INFO_ARTICLE:                   // "add encyclopedia custom article: Information, id=$0, title page=$1, title id=$2, text page=$3, text id=$4, timeout=$5"
   case CMD_ADD_HISTORY_ARTICLE:                // "add encyclopedia custom article: History, id=$0, title page=$1, title id=$2, text page=$3, text id=$4, timeout=$5"
   case CMD_ADD_STORIES_ARTICLE:                // "add encyclopedia custom article: Stories, id=$0, title page=$1, title id=$2, text page=$3, text id=$4, timeout=$5"
   case CMD_ADD_PLAYER_GRAPH:                   // "add player graph: script=$0 pageid=$1 textid=$2 group pageid=$3 group textid=$4 subgroup=$5"
   /// [DUAL ENTRIES] {Page,String} = {2, 3} and {4, 5}
   case CMD_ADD_CLASS_GRAPH:                    // "add class graph: class=$0 script=$1 pageid=$2 textid=$3 group pageid=$4 group textid=$5 subgroup=$6"
   case CMD_ADD_OBJECT_GRAPH:                   // "$0 add object graph: script=$1 pageid=$2 textid=$3 group pageid=$4 group textid=$5 subgroup=$6"
   /// [VARIABLE ENTRIES] {Page,String} = {1,2} -> {1,3}
   case CMD_READ_TEXT_ARRAY:                    // "$0 read text: page id=$1, from $2 to $3 to array, include empty=$4"
      // Return TRUE
      return TRUE;
   }

   // Return FALSE
   return FALSE;
}


/// Function name  : isCommandID
// Description     : Test a COMMAND for a specified ID
// 
// CONST COMMAND*  pCommand : [in] COMMAND to test
// CONST UINT      iID      : [in] Command ID
// 
// Return Value   : TRUE or FALSE
// 
BearScriptAPI
BOOL  isCommandComment(CONST COMMAND*  pCommand)
{
   switch (pCommand->iID)
   {
   case CMD_COMMENT:
   case CMD_COMMAND_COMMENT:
   case CMD_NOP:
      return TRUE;
   }

   return FALSE;
}


/// Function name  : isCommandScriptCall
// Description     : Identifies the ScriptCall commands
// 
// CONST COMMAND*  pCommand : [in] COMMAND to test
// 
// Return Value   : TRUE or FALSE
// 
BearScriptAPI
BOOL  isCommandScriptCall(CONST COMMAND*  pCommand)
{
   switch (pCommand->iID)
   {
   /// [SCRIPTNAME IS PARAMETER ZERO]
   case CMD_CALL_SCRIPT_VAR_ARGS:               // "$1 $2 call script $0 :"
   case CMD_REGISTER_AL_SCRIPT:                 // "al engine: register script=$0"
   case CMD_REGISTER_QUEST_SCRIPT:              // "register quest script $0 instance multiple=$1"
   case CMD_REGISTER_GOD_EVENT:                 // "register god event: script=$0 mask=$1"
   case CMD_UNREGISTER_AL_SCRIPT:               // "al engine: unregister script $0"
   case CMD_ADD_SCRIPT_OPTIONS:                 // "add script options: script=$0 pageid=$1 textid=$2"  
   case CMD_ADD_PLAYER_GRAPH:                   // "add player graph: script=$0 pageid=$1 textid=$2 group pageid=$3 group textid=$4 subgroup=$5"
   case CMD_REMOVE_PLAYER_GRAPH:                // "remove player graph: script=$0 group pageid=$1 group textid=$2"
   case CMD_REMOVE_SCRIPT_OPTIONS:              // "remove script options: script=$0"  

   /// [SCRIPTNAME IS PARAMETER ONE]
   case CMD_BEGIN_TASK_ARGS:                    // "$0 begin task $2 with script $1 and priority $3: arg1=$4o arg2=$5x arg3=$6y arg4=$7z arg5=$8a"
   case CMD_INTERRUPT_SCRIPT:                   // "$0 interrupt with script $1 and prio $2"
   case CMD_INTERRUPT_SCRIPT_ARGS:              // "$0 interrupt with script $1 and prio $2: arg1=$3 arg2=$4 arg3=$5 arg4=$6"
   case CMD_INTERRUPT_TASK_ARGS:                // "$0 interrupt task $2 with script $1 and prio $3: arg1=$4 arg2=$5 arg3=$6 arg4=$7"
   case CMD_SET_GLOBAL_SCRIPT_MAP:              // "global script map: set: key=$0, class=$3, race=$4, script=$1, prio=$2"
   case CMD_SET_SHIP_PRELOAD_SCRIPT:            // "set ship command preload script: command=$0 script=$1"
   case CMD_SET_WING_PRELOAD_SCRIPT:            // "set wing command preload script: wing command=$0 script=$1"
   case CMD_ADD_CLASS_GRAPH:                    // "add class graph: class=$0 script=$1 pageid=$2 textid=$3 group pageid=$4 group textid=$5 subgroup=$6"
   case CMD_ADD_OBJECT_GRAPH:                   // "$0 add object graph: script=$1 pageid=$2 textid=$3 group pageid=$4 group textid=$5 subgroup=$6"
   case CMD_REMOVE_CLASS_GRAPH:                 // "remove class graph: class=$0 script=$1 group pageid=$2 group textid=$3"
   case CMD_REMOVE_OBJECT_GRAPH:                // "$0 remove object graph: script=$1 group pageid=$2 group textid=$3"
   case CMD_GET_HOTKEY_INDEX_1:                 // "$0 get hotkey index, script: $1"
   case CMD_GET_HOTKEY_INDEX_2:                 // "$0 get hotkey index, script name: $1"
   case CMD_CHECK_SCRIPT_EXISTS_1:              // "$0 does script exist: $1"
   case CMD_CHECK_SCRIPT_EXISTS_2:              // "$0 does script name exist: $1"
   case CMD_SEND_INCOMING_QUESTION:             // "send incoming question $0 to player: callback=$1"
   case CMD_SEND_INCOMING_QUESTION_EX:          // "send incoming question $0 to player: callback=$1 flags=$2"

   /// [SCRIPTNAME IS PARAMETER TWO]
   case CMD_CALL_SCRIPT_ARGS:                   // "$1 $0 call named script: script=$2, $3, $4, $5, $6, $7"
   case CMD_LAUNCH_SCRIPT_ARGS:                 // "$0 launch named script: task=$1 scriptname=$2 prio=$3, $4, $5, $6, $7, $8"
   case CMD_IS_SCRIPT_ON_STACK:                 // "$1 $0 is script $2 on stack of task=$3"
   case CMD_CONNECT_SHIP_SCRIPT:                // "$0 connect ship command/signal $1 to script $2 with prio $3"
   case CMD_CONNECT_WING_SCRIPT:                // "$0 connect wing command/signal $1 to script $2 with prio $3"
   case CMD_SET_SHIP_UPGRADE_SCRIPT:            // "set script command upgrade: command=$0  upgrade=$1  script=$2"
   case CMD_SET_WING_UPGRADE_SCRIPT:            // "set script command upgrade: wing command=$0 upgrade=$1 script=$2"
   case CMD_SET_GLOBAL_WING_SCRIPT_MAP:         // "global script map for wings: key=$0 race=$1 script=$2 prio=$3"
   case CMD_ADD_SECONDARY_SIGNAL:               // "$0 add secondary signal: signal=$1, script=$2, prio=$3, name=$4"
   case CMD_ADD_WING_SCRIPT:                    // "$0 add wing additional ship command: id=$1 script=$2 pageid=$3 textid=$4"
   case CMD_ADD_WING_SCRIPT_CHECK:              // "$0 add wing additional ship command: id=$1 script=$2 pageid=$3 textid=$4 checkscript=$5"
   case CMD_REGISTER_HOTKEY_1:                  // "$0 register hotkey $1 to call script $2"
   case CMD_REGISTER_HOTKEY_2:                  // "$0 register hotkey $1 to call script name $2" 

   /// [SCRIPTNAME IS PARAMETER THREE]
   case CMD_ADD_EVENT_LISTENER:                 // "$0 add event listener: quest/event=$1 objevent=$2 script=$3"
   case CMD_SET_GLOBAL_SECONDARY_MAP:           // "global secondary signal map: add signal=$0 race=$1 class=$2 script=$3 prio=$4 name=$5"
   case CMD_REGISTER_HOTKEY_1_BYREF:            // "$0 register hotkey: page=$1 id=$2, to call script $3"
   case CMD_REGISTER_HOTKEY_2_BYREF:            // "$0 register hotkey: page=$1 id=$2, to call script name $3"
      return TRUE;

   default:
      return FALSE;
   }
}

/// Function name  : isCommandTradingSearch
// Description     : Check whether COMMANd is one of the many 'find station with best xxx' trading commands
// 
// CONST COMMAND*  pCommand : [in] COMMAND to test
// 
// Return Value   : TRUE or FALSE
// 
BearScriptAPI
BOOL  isCommandTradingSearch(CONST COMMAND*  pCommand)
{
   switch (pCommand->iID)
   {
   case 606:
   case 607:
   case 608:
   case 609:
   case 663:
   case 664:
   case 1002:
   case 1003:
   case 1004:
   case 1005:
   case 1006:
   case 1007:
   case 1169:
   case 1170:
   case 1171:
   case 1176:
   case 1177:
   case 1178:
      return TRUE;
   }

   return FALSE;
}


/// Function name  : isCommandID
// Description     : Test a COMMAND for a specified ID
// 
// CONST COMMAND*  pCommand : [in] COMMAND to test
// CONST UINT      iID      : [in] Command ID
// 
// Return Value   : TRUE or FALSE
// 
BearScriptAPI
BOOL  isCommandID(CONST COMMAND*  pCommand, CONST UINT  iID)
{
   return (pCommand->iID == iID);
}


/// Function name  : isCommandType
// Description     : Test a COMMAND for a specified command type flag
// 
// CONST COMMAND*      pCommand : [in] COMMAND to test
// CONST COMMAND_TYPE  eType    : [in] CommandType to test for
// 
// Return Value   : TRUE or FALSE
// 
BearScriptAPI
BOOL  isCommandType(CONST COMMAND*  pCommand, CONST COMMAND_TYPE  eType)
{
   return (pCommand->iFlags INCLUDES eType);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : findScriptCallTargetInCommand
// Description     : Returns the name of the script called by a scriptcall command
// 
// CONST COMMAND*  pCommand : [in] Command containing a script-call command
// CONST TCHAR*   &szOutput : [out] ScriptName if found, otherwise NULL
// 
// Return Value   : TRUE if found, otherwise FALSE
// 
BearScriptAPI
BOOL   findScriptCallTargetInCommand(CONST COMMAND*  pCommand, CONST TCHAR*  &szOutput)
{
   PARAMETER*  pScriptName;

   // Prepare
   szOutput = NULL;

   /// Search Command for ScriptName Parameter
   if (findScriptCallParameterInCommand(pCommand, pScriptName) AND pScriptName->eType == DT_STRING)
      // [FOUND] Set result
      szOutput = pScriptName->szValue;

   // Return TRUE if found
   return szOutput != NULL;
}


/// Function name  : setCommandJumpDestination
// Description     : Target any branching COMMAND at another COMMAND
// 
// COMMAND*        pCommand  : [in/out] COMMAND to set the jump destination of
// CONST COMMAND*  pTarget   : [in]     COMMAND to target
// 
VOID  setCommandJumpDestination(COMMAND*  pCommand, CONST COMMAND*  pTarget)
{
   UINT   iTargetIndex;

   // Determine whether to target the target command, or it's associated command
   iTargetIndex = (isCommandType(pTarget, CT_AUXILIARY) ? pTarget->iReferenceIndex : pTarget->iIndex);

   // Set COMMAND's jump destination
   setCommandJumpDestinationByIndex(pCommand, iTargetIndex);
}


/// Function name  : setCommandJumpDestinationByIndex
// Description     : Target any branching COMMAND at a specific COMMAND index
// 
// COMMAND*    pCommand  : [in/out] COMMAND to set the jump destination of
// CONST UINT  iIndex    : [in]     Zero based index of the target COMMAND
// 
VOID  setCommandJumpDestinationByIndex(COMMAND*  pCommand, CONST UINT  iIndex)
{
   PARAMETER*      pParameter;       // Parameter being processed
   RETURN_OBJECT*  pReturnObject;    // ReturnObject to encapsulate the parameter being processed
   
   // Examine Command ID
   switch (pCommand->iID)
   {
   /// [JUMP COMMAND] Update jump destination PARAMETER
   case CMD_HIDDEN_JUMP:
      ASSERT(findParameterInCommandByIndex(pCommand, PT_DEFAULT, 0, pParameter));

      // Locate PARAMETER containing the jump destination
      if (findParameterInCommandByIndex(pCommand, PT_DEFAULT, 0, pParameter))      // Error checking not required, Parameter was generated manually
         pParameter->iValue = iIndex;
      break;

   /// [NON-JUMP COMMAND] Update the ReturnObject within the COMMAND
   default:
      ASSERT(findReturnObjectParameterInCommand(pCommand, pParameter));

      // Locate PARAMETER containing the ReturnObject
      if (findReturnObjectParameterInCommand(pCommand, pParameter))      // Error checking not required, command already verified
      {
         // Unpack PARAMETER into a ReturnObject
         pReturnObject = createReturnObjectFromInteger(pParameter->iValue);

         // [CHECK] Ensure ReturnObject is a Conditional
         ASSERT(pReturnObject->eType == ROT_CONDITIONAL);

         // Update destination and re-pack PARAMETER
         pReturnObject->iDestination = iIndex;
         pParameter->iValue          = calculateReturnObjectEncoding(pReturnObject);

         // [DEBUG]
         ASSERT((pParameter->iValue INCLUDES 0x00ffff00) != 0x00ffff00);

         // Cleanup
         deleteReturnObject(pReturnObject);
      }
      break;
   }
}

