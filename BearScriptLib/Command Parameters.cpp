//
// Parameters.cpp  :  ____
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                 CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createParameterFromInteger
// Description     : Creates a new PARAMETER from an integer
// 
// CONST INT               iValue      : [in] Value
// CONST DATA_TYPE         eType       : [in] Data type
// CONST PARAMETER_SYNTAX  eSyntax     : [in] Syntax for the new parameter
// CONST UINT              iLineNumber : [in] Line number in the XML file containing the original <sourcevalue> tag
// 
// Return Value   : New PARAMETER, you are responsible for destroying it
// 
PARAMETER*  createParameterFromInteger(CONST INT  iValue, CONST DATA_TYPE  eType, CONST PARAMETER_SYNTAX  eSyntax, CONST UINT  iLineNumber)
{
   PARAMETER*  pNewParameter;

   // Create new object
   pNewParameter = utilCreateEmptyObject(PARAMETER);

   // Set properties
   pNewParameter->iValue      = iValue;
   pNewParameter->eSyntax     = eSyntax;
   pNewParameter->eType       = eType;
   pNewParameter->iLineNumber = iLineNumber;

   // Return new object
   return pNewParameter;
}


/// Function name  : createParameterFromString
// Description     : Creates a new PARAMETER from a string
// 
// CONST TCHAR*            szValue     : [in] 
// CONST PARAMETER_SYNTAX  eSyntax     : [in] Syntax for the new parameter
// CONST UINT              iLineNumber : [in] Line number in the XML file containing the original <sourcevalue> tag
// 
// Return Value   : New PARAMETER, you are responsible for destroying it
// 
PARAMETER*  createParameterFromString(CONST TCHAR*  szValue, CONST PARAMETER_SYNTAX  eSyntax, CONST UINT  iLineNumber)
{
   PARAMETER*  pNewParameter;

   // Create new object
   pNewParameter = utilCreateEmptyObject(PARAMETER);

   // Set properties
   pNewParameter->szValue     = utilDuplicateSimpleString(szValue);
   pNewParameter->eSyntax     = eSyntax;
   pNewParameter->eType       = DT_STRING;
   pNewParameter->iLineNumber = iLineNumber;

   // Return new object
   return pNewParameter;
}


/// Function name  : createParameterFromVariable
// Description     : Creates a new PARAMETER for a variable
// 
// CONST TCHAR*            szName      : [in] Variable name
// CONST PARAMETER_SYNTAX  eSyntax     : [in] Syntax for the new parameter
// CONST UINT              iLineNumber : [in] Line number in the XML file containing the original <sourcevalue> tag
// 
// Return Value   : New PARAMETER, you are responsible for destroying it
// 
PARAMETER*  createParameterFromVariable(CONST TCHAR*  szName, CONST PARAMETER_SYNTAX  eSyntax, CONST UINT  iLineNumber)
{
   PARAMETER*  pNewParameter;

   // Create new object
   pNewParameter = utilCreateEmptyObject(PARAMETER);

   // Set properties
   pNewParameter->szValue     = utilDuplicateSimpleString(szName);
   pNewParameter->eSyntax     = eSyntax;
   pNewParameter->eType       = DT_VARIABLE;
   pNewParameter->iLineNumber = iLineNumber;

   // Return new object
   return pNewParameter;
}

/// Function name  : deleteParameter
// Description     : Destroy a PARAMETER object
// 
// PARAMETER*  &pParameter : [in] Parameter to destroy
// 
VOID  deleteParameter(PARAMETER*  &pParameter)
{
   // Delete string value and/or buffer (if any)
   utilSafeDeleteStrings(pParameter->szValue, pParameter->szBuffer);
 
   // Delete calling object
   utilDeleteObject(pParameter);
}


/// Function name  : destructorParameter
// Description     : Destroy a PARAMETER list ListItem
// 
// LPARAM    pParameter : [in] ListItem data (PARAMETER*)
// 
VOID  destructorParameter(LPARAM  pParameter)
{
   deleteParameter((PARAMETER*&)pParameter);
}


/// Function name  : duplicateParameter
// Description     : Create a new PARAMETER from an existing PARAMETER
// 
// CONST PARAMETER*  pExistingParameter   : [in] Existing parameter to copy
// 
// Return Value   : New PARAMETER object, you are responsible for destroying it
// 
PARAMETER*  duplicateParameter(CONST PARAMETER*  pExistingParameter)
{
   PARAMETER*  pNewParameter;

   // Create new object
   pNewParameter = utilCreateEmptyObject(PARAMETER);

   // Copy properties
   utilCopyObject(pNewParameter, pExistingParameter, PARAMETER);

   // [STRING/BUFFER] Duplicate string values
   pNewParameter->szValue  = utilSafeDuplicateSimpleString(pExistingParameter->szValue);
   pNewParameter->szBuffer = utilSafeDuplicateSimpleString(pExistingParameter->szBuffer);

   // Return new object
   return pNewParameter;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                         HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : appendParameterToCommand
// Description     : Add a PARAMETER to either of a COMMAND's Paramter arrays
// 
// COMMAND*               pCommand   : [in] Command containing the parameter arrays
// PARAMETER*             pParameter : [in] Parameter to append
// CONST PARAMETER_TYPE   eType      : [in] Indicates which array to append
// 
VOID   appendParameterToCommand(COMMAND*  pCommand, PARAMETER*  pParameter, CONST PARAMETER_TYPE  eType)
{
   // Determine which array to append from the ParameterType
   switch (eType)
   {
   case PT_DEFAULT:  appendParameterToArray(pCommand->pParameterArray, pParameter);          break;
   case PT_INFIX:    appendParameterToArray(pCommand->pInfixParameterArray, pParameter);     break;
   case PT_POSTFIX:  appendParameterToArray(pCommand->pPostfixParameterArray, pParameter);   break;
   }
}


/// Function name  : findIntegerParameterInCommandByIndex
// Description     : Locate an integer parameter in a command's default parameter list
// 
// CONST COMMAND*   pCommand : [in]     COMMAND object to search
// CONST UINT       iIndex   : [in]     Syntax index of the parameter to retrieve
// INT             &iOutput  : [in/out] Integer value if found, otherwise zero
// 
// Return Value   : TRUE if parameter was found, FALSE otherwise
// 
BearScriptAPI
BOOL  findIntegerParameterInCommandByIndex(CONST COMMAND* pCommand, CONST UINT  iIndex, INT  &iOutput)
{
   PARAMETER*  pParameter;
   BOOL        bResult;

   // Prepare
   iOutput = 0;

   // Lookup Integer Parameter
   if (bResult = (pCommand AND findParameterInCommandByIndex(pCommand, PT_DEFAULT, iIndex, pParameter) AND pParameter->eType == DT_INTEGER))
      /// [FOUND] Set result
      iOutput = pParameter->iValue;

   // Return result
   return bResult;
}


/// Function name  : findParameterInCommandByIndex
// Description     : Locate a parameter in one of a command's parameter lists
// 
// CONST COMMAND*        pCommand : [in]  COMMAND object to search
// CONST PARAMETER_TYPE  eType    : [in]  Identifies whether to retrieve the DEFAULT or POSTFIX parameter arrays
// CONST UINT            iIndex   : [in]  Zero-based array index to retrieve
// PARAMETER*            &pOutput : [out] Resultant Parameter if any, otherwise NULL
// 
// Return Value   : TRUE if parameter was found, FALSE otherwise
// 
BearScriptAPI
BOOL  findParameterInCommandByIndex(CONST COMMAND* pCommand, CONST PARAMETER_TYPE  eType, CONST UINT  iIndex, PARAMETER*  &pOutput)
{
   // Prepare
   pOutput = NULL;

   // Search appropriate array
   switch (eType)
   {
   case PT_DEFAULT: findParameterInArray(pCommand->pParameterArray, iIndex, pOutput);         break;
   case PT_INFIX:   findParameterInArray(pCommand->pInfixParameterArray, iIndex, pOutput);    break;
   case PT_POSTFIX: findParameterInArray(pCommand->pPostfixParameterArray, iIndex, pOutput);  break;
   }
   
   // Return TRUE if parameter was found
   return (pOutput != NULL);
}


/// Function name  : findReturnObjectParameterInCommand
// Description     : Finds the PARAMETER in a COMMAND that contains the ReturnObject
// 
// CONST COMMAND*  pCommand : [in]  COMMAND to search
// PARAMETER*     &pOutput  : [out] ReturnObject PARAMETER if found, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BearScriptAPI
BOOL  findReturnObjectParameterInCommand(CONST COMMAND*  pCommand, PARAMETER*  &pOutput)
{
   PARAMETER*  pCurrentParameter;
   UINT        iParameterIndex;      // ParameterArray iterator

   // Prepare
   iParameterIndex = 0;
   pOutput         = NULL;

   /// Search through COMMAND's default ParameterArray for a PARAMETER with 'Return Object' syntax
   while (!pOutput AND findParameterInCommandByIndex(pCommand, PT_DEFAULT, iParameterIndex++, pCurrentParameter))
   {
      if (isReturnObject(pCurrentParameter->eSyntax))
         // [FOUND] Return current Parameter
         pOutput = pCurrentParameter;
   }

   // Return TRUE if parameter was found
   return (pOutput != NULL);
}


/// Function name  : findReturnVariableParameterInCommand
// Description     : Finds the name of the return variable, if any
// 
// CONST COMMAND*  pCommand : [in]  COMMAND to search
// CONST TCHAR*   &pOutput  : [out] Name of the return variable, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BearScriptAPI
BOOL  findReturnVariableParameterInCommand(CONST COMMAND*  pCommand, CONST TCHAR*  &szOutput)
{
   PARAMETER*  pParameter;

   // Prepare
   szOutput = NULL;

   /// [CHECK] Lookup ReturnObject and ensure it's a variable
   if (findReturnObjectParameterInCommand(pCommand, pParameter) AND isVariableParameter(pParameter))
      // [FOUND] Extract variable name
      szOutput = identifyRawVariableName(pParameter->szValue);

   // Return TRUE if found
   return szOutput != NULL;
}


/// Function name  : findScriptCallParameterInCommand
// Description     : Retrieves the Parameter containing the script name
// 
// const COMMAND*  pCommand : [in] ScriptCall command
// PARAMETER*&     pOutput  : [out] ScriptCall parameter, if found.  NB: May be a string or a variable
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BearScriptAPI
BOOL  findScriptCallParameterInCommand(const COMMAND*  pCommand, PARAMETER*&  pOutput)
{
   // Prepare
   pOutput = NULL;

   // Extract the script name PARAMETER from the input COMMAND
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
      findParameterInCommandByIndex(pCommand, PT_DEFAULT, 0, pOutput);
      break;

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
      findParameterInCommandByIndex(pCommand, PT_DEFAULT, 1, pOutput);  
      break;

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
      findParameterInCommandByIndex(pCommand, PT_DEFAULT, 2, pOutput);  
      break;

   /// [SCRIPTNAME IS PARAMETER THREE]
   case CMD_ADD_EVENT_LISTENER:                 // "$0 add event listener: quest/event=$1 objevent=$2 script=$3"
   case CMD_SET_GLOBAL_SECONDARY_MAP:           // "global secondary signal map: add signal=$0 race=$1 class=$2 script=$3 prio=$4 name=$5"

   case CMD_REGISTER_HOTKEY_1_BYREF:            // "$0 register hotkey: page=$1 id=$2, to call script $3"
   case CMD_REGISTER_HOTKEY_2_BYREF:            // "$0 register hotkey: page=$1 id=$2, to call script name $3"
      findParameterInCommandByIndex(pCommand, PT_DEFAULT, 3, pOutput);
      break;
   }

   // Return TRUE if found
   return pOutput != NULL;
}



// rename CMD_LAUNCH_SCRIPT_ARGS  CMD_LAUNCH_NAMED_SCRIPT

/// Function name  : findStringParameterInCommandByIndex
// Description     : Locate a string parameter in a command's default parameter list
// 
// CONST COMMAND*   pCommand : [in]  COMMAND object to search
// CONST UINT       iIndex   : [in]  Syntax index of the parameter to retrieve
// CONST TCHAR*    &szOutput : [out] String value if found, otherwise NULL
// 
// Return Value   : TRUE if string was found, FALSE otherwise
// 
BearScriptAPI
BOOL  findStringParameterInCommandByIndex(CONST COMMAND* pCommand, CONST UINT  iIndex, CONST TCHAR* &szOutput)
{
   PARAMETER*  pParameter;

   // Prepare
   szOutput = NULL;

   // Lookup string Parameter
   if (pCommand AND findParameterInCommandByIndex(pCommand, PT_DEFAULT, iIndex, pParameter) AND pParameter->eType == DT_STRING)
      /// [FOUND] Set result
      szOutput = pParameter->szValue;
   
   // Return TRUE if found
   return (szOutput != NULL);
}


/// Function name  : findVariableParameterInCommandByIndex
// Description     : Locate a variable parameter in a command's default parameter list
// 
// CONST COMMAND*   pCommand : [in]  COMMAND object to search
// CONST UINT       iIndex   : [in]  Syntax index of the parameter to retrieve
// CONST TCHAR*    &szOutput : [out] Variable name if found, without leading $ (otherwise NULL)
// 
// Return Value   : TRUE if variable was found, FALSE otherwise
// 
BearScriptAPI
BOOL  findVariableParameterInCommandByIndex(CONST COMMAND* pCommand, CONST UINT  iIndex, CONST TCHAR* &szOutput)
{
   PARAMETER*  pParameter;

   // Prepare
   szOutput = NULL;

   // Lookup string Parameter
   if (pCommand AND findParameterInCommandByIndex(pCommand, PT_DEFAULT, iIndex, pParameter) AND isVariableParameter(pParameter))
      /// [FOUND] Set result
      szOutput = identifyRawVariableName(pParameter->szValue);
   
   // Return TRUE if found
   return (szOutput != NULL);
}


/// Function name  : getCommandParameterCount
// Description     : Retrieves the parameter count of one of a COMMAND's parameter lists
// 
// CONST COMMAND*        pCommand : [in] Command
// CONST PARAMETER_TYPE  eType    : [in] Parameter array type
// 
// Return Value   : Number of parameters
// 
BearScriptAPI
UINT  getCommandParameterCount(CONST COMMAND*  pCommand, CONST PARAMETER_TYPE  eType)
{
   UINT  iCount;

   // Search appropriate array
   switch (eType)
   {
   case PT_DEFAULT: iCount = getArrayItemCount(pCommand->pParameterArray);         break;
   case PT_INFIX:   iCount = getArrayItemCount(pCommand->pInfixParameterArray);    break;
   case PT_POSTFIX: iCount = getArrayItemCount(pCommand->pPostfixParameterArray);  break;
   }
   
   // Return TRUE if parameter was found
   return iCount;
}


/// Function name  : generateParameterFromMissingWare
// Description     : Generates an appropriate PARAMETER by decoding the 'missing ware' format
// 
// CONST TCHAR*  szParameterText : [in]  Encoded 'missing ware' in the format {<MainType string>@<SubType>} 
// PARAMETER*   &pOutput         : [out] New DT_WARE PARAMETER if successful, otherwise NULL
// 
// Return Value   : TRUE if successful, otherwise FALSE
// 
BOOL  generateParameterFromMissingWare(CONST TCHAR*  szParameterText, CONST PARAMETER_SYNTAX  eSyntax, CONST UINT  iLineNumber, PARAMETER*  &pOutput)
{
   SCRIPT_OBJECT*  pScriptObject;      // ScriptObject containing the MainType string
   CONST TCHAR*    szSubType;          // Encoded SubType as string
   MAIN_TYPE       eMainType;          // MainType
   TCHAR*          szMainType;         // Encoded MainType as string
   UINT            iSubType;           // SubType

   // Prepare
   pOutput = NULL;
   
   // [CHECK] Search for delimiter
   if (szSubType = utilFindCharacter(szParameterText, '@'))
   {
      // Extract MainType string
      szMainType = utilDuplicateSubString(szParameterText, lstrlen(szParameterText), 0, (szSubType - szParameterText));

      // [CHECK] Lookup MainType
      if (findScriptObjectByText(szMainType, pScriptObject))
      {
         // [FOUND] Calculate MainType and SubType
         eMainType = (MAIN_TYPE)(pScriptObject->iID - 200);
         iSubType  = utilConvertStringToInteger(&szSubType[1]);

         /// [SUCCESS] Encode ware
         pOutput = createParameterFromInteger(MAKE_LONG(iSubType, eMainType), DT_WARE, eSyntax, iLineNumber);
      }

      // Cleanup
      utilDeleteString(szMainType);
   }

   // Return TRUE if PARAMETER was generated
   return (pOutput != NULL);
}

/// Function name  : generateParameterGameStringRefs
// Description     : Identifies whether a Command is GameString dependent and the parameter indicies of the string/pages
// 
// CONST COMMAND*  pCommand : [in] Command to test
// 
// Return Value   : List containing GameStringReferences if successful, otherwise NULL
// 
BearScriptAPI
LIST*   generateParameterGameStringRefs(CONST COMMAND*  pCommand)
{
   LIST*   pOutputList;

   // Prepare
   pOutputList = createList(destructorSimpleObject);

   // Examine Command ID
   switch (pCommand->iID)
   {
   /// [SINGLE ENTRY] {Page,String} = {0, 1}
   case CMD_SET_NEWS_ARTICLE_STATE:             // "set state of news article page=$0 id=$1 to $2"
   case CMD_WRITE_LOGBOOK:                      // "write to player logbook: printf: pageid=$0 textid=$1, $2, $3, $4, $5, $6"
   case CMD_WRITE_LOG:                          // "write to log file $0  append=$1  printf: pageid=$2 textid=$3, $4, $5, $6, $7, $8"
      appendObjectToList(pOutputList, (LPARAM)createGameStringReference(0, 1));
      break;

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
      appendObjectToList(pOutputList, (LPARAM)createGameStringReference(1, 2));
      break;

   /// [SINGLE ENTRY] {Page,String} = {2, 3}
   case CMD_REMOVE_CLASS_GRAPH:                 // "remove class graph: class=$0 script=$1 group pageid=$2 group textid=$3"
   case CMD_REMOVE_OBJECT_GRAPH:                // "$0 remove object graph: script=$1 group pageid=$2 group textid=$3"
      appendObjectToList(pOutputList, (LPARAM)createGameStringReference(2, 3));
      break;

   /// [SINGLE ENTRY] {Page,String} = {3, 4} 
   case CMD_ADD_WING_SCRIPT:                    // "$0 add wing additional ship command: id=$1 script=$2 pageid=$3 textid=$4"
   case CMD_ADD_WING_SCRIPT_CHECK:              // "$0 add wing additional ship command: id=$1 script=$2 pageid=$3 textid=$4 checkscript=$5"
      appendObjectToList(pOutputList, (LPARAM)createGameStringReference(3, 4));
      break;

   /// [DUAL ENTRIES] {Page,String} = {1, 2} and {3, 4}
   case CMD_ADD_NEWS_ARTICLE:                   // "add encyclopedia custom article: News, id=$0, title page=$1, title id=$2, text page=$3, text id=$4, timeout=$5"
   case CMD_ADD_INFO_ARTICLE:                   // "add encyclopedia custom article: Information, id=$0, title page=$1, title id=$2, text page=$3, text id=$4, timeout=$5"
   case CMD_ADD_HISTORY_ARTICLE:                // "add encyclopedia custom article: History, id=$0, title page=$1, title id=$2, text page=$3, text id=$4, timeout=$5"
   case CMD_ADD_STORIES_ARTICLE:                // "add encyclopedia custom article: Stories, id=$0, title page=$1, title id=$2, text page=$3, text id=$4, timeout=$5"
   case CMD_ADD_PLAYER_GRAPH:                   // "add player graph: script=$0 pageid=$1 textid=$2 group pageid=$3 group textid=$4 subgroup=$5"
      appendObjectToList(pOutputList, (LPARAM)createGameStringReference(1, 2));
      appendObjectToList(pOutputList, (LPARAM)createGameStringReference(3, 4));
      break;

   /// [DUAL ENTRIES] {Page,String} = {2, 3} and {4, 5}
   case CMD_ADD_CLASS_GRAPH:                    // "add class graph: class=$0 script=$1 pageid=$2 textid=$3 group pageid=$4 group textid=$5 subgroup=$6"
   case CMD_ADD_OBJECT_GRAPH:                   // "$0 add object graph: script=$1 pageid=$2 textid=$3 group pageid=$4 group textid=$5 subgroup=$6"
      appendObjectToList(pOutputList, (LPARAM)createGameStringReference(2, 3));
      appendObjectToList(pOutputList, (LPARAM)createGameStringReference(4, 5));
      break;

   /// [VARIABLE ENTRIES] {Page,String} = {1,2} -> {1,3}
   case CMD_READ_TEXT_ARRAY:                    // "$0 read text: page id=$1, from $2 to $3 to array, include empty=$4"
      appendObjectToList(pOutputList, (LPARAM)createGameStringReference(1, 2));
      appendObjectToList(pOutputList, (LPARAM)createGameStringReference(1, 3));
      break;
   }

   // [CHECK] Destroy list if no references were found
   if (!getListItemCount(pOutputList))
      deleteList(pOutputList);

   // Return list or NULL
   return pOutputList;
}


/// Function name  : identifyParameterSyntaxString
// Description     : Retrieve the string version of a ParameterSyntax object
// 
// CONST PARAMETER_SYNTAX  eParameterSyntax : [in] ParameterSyntax value to convert to string
// 
// Return Value   : String representing the ParameterSyntax or "Unknown" if not found
// 
CONST TCHAR*  identifyParameterSyntaxString(CONST PARAMETER_SYNTAX  eParameterSyntax)
{
   GAME_STRING*  pOutput;

   // Lookup GameString
   if (findGameStringByID((UINT)eParameterSyntax, identifyGamePageIDFromDataType(DT_SCRIPTDEF), pOutput))
      return pOutput->szText;
   
   // [NOT FOUND] Return 'Unknown'
   return TEXT("Unknown");
}


/// Function name  : insertParameterIntoCommand
// Description     : Stores a PARAMETER in a COMMAND at a specified physical index
// 
// COMMAND*                pCommand   : [in/out] Command to attach parameter to
// CONST PARAMETER_INDEX*  pIndex     : [in]     ParameterIndex containing the PHYSICAL index to store Parameter at
// PARAMETER*              pParameter : [in]     Parameter to be attached
// 
VOID   insertParameterIntoCommand(COMMAND*  pCommand, CONST PARAMETER_INDEX*  pIndex, PARAMETER*  pParameter)
{
   // Insert Parameter into the COMMAND's  DEFAULT  parameter array
   insertParameterIntoArray(pCommand->pParameterArray, pIndex->iPhysicalIndex, pParameter);
}


/// Function name  : isReferenceObject
// Description     : 
// 
// CONST PARAMETER*  pParameter : [in] 
// 
// Return Value   : TRUE/FALSE
// 
BOOL  isReferenceObject(CONST PARAMETER*  pParameter)
{
   return pParameter->eSyntax == PS_REFERENCE_OBJECT;
}


/// Function name  : isReturnVariableInCommand
// Description     : Checks whether a command contains a specific Return Variable
// 
// CONST COMMAND*  pCommand   : [in] Command to search
// CONST TCHAR*    szVariable : [in] Desired Variable Name
// 
// Return Value   : TRUE/FALSE
// 
BearScriptAPI
BOOL  isReturnVariableInCommand(CONST COMMAND*  pCommand, CONST TCHAR*  szVariable)
{
   PARAMETER*  pReturnObject;

   // [CHECK] Ensure ReturnObject is present, is a variable, and variable names match
   return findReturnObjectParameterInCommand(pCommand, pReturnObject) AND pReturnObject->szValue AND utilCompareStringVariables(identifyRawVariableName(szVariable), identifyRawVariableName(pReturnObject->szValue));
}


/// Function name  : isOperatorParameterInCommandAtIndex
// Description     : Checks whether a certain operator is at a specific index
// 
// CONST COMMAND*       pCommand  : [in] COMMAND to search
// CONST UINT           iIndex    : [in] Zero-based index to search
// CONST OPERATOR_TYPE  eOperator : [in] Operator to search for
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BOOL  isOperatorParameterInCommandAtIndex(CONST COMMAND*  pCommand, CONST UINT  iIndex, CONST OPERATOR_TYPE  eOperator)
{
   PARAMETER*  pParameter;

   /// [CHECK] Ensure operator is present and has the desired type
   return findParameterInCommandByIndex(pCommand, PT_DEFAULT, iIndex, pParameter) AND pParameter->eType == DT_OPERATOR AND pParameter->iValue == eOperator;
}


/// Function name  : isVariableParameterInCommandAtIndex
// Description     : Checks whether a certain variable is at a specific index
// 
// CONST COMMAND*  pCommand : [in] COMMAND to search
// CONST UINT      iIndex   : [in] Zero-based index to search
// CONST TCHAR*    pOutput  : [in] Name of the variable
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BOOL  isParameterTypeInCommandAtIndex(CONST COMMAND*  pCommand, CONST UINT  iIndex, CONST DATA_TYPE  eType)
{
   PARAMETER*  pParameter;

   /// [CHECK] Ensure Parameter is present and has the desired type
   return findParameterInCommandByIndex(pCommand, PT_DEFAULT, iIndex, pParameter) AND pParameter->eType == eType;
}


/// Function name  : isVariableParameter
// Description     : Determines whether a PARAMETER contains a normal variable or ReturnVariable
// 
// CONST PARAMETER*  pParameter : [in] Parameter to text
// 
// Return Value   : TRUE/FALSE
// 
BOOL  isVariableParameter(CONST PARAMETER*  pParameter)
{
   // [CHECK] Check for the 'variable' type or an encoded ReturnVariable
   return pParameter->eType == DT_VARIABLE OR (isReturnObject(pParameter->eSyntax) AND identifyReturnObjectType(pParameter->iValue) == ROT_VARIABLE);
}


/// Function name  : isVariableParameterInCommand
// Description     : Checks whether a certain variable is at a specific index
// 
// CONST COMMAND*  pCommand : [in] COMMAND to search
// CONST UINT      iIndex   : [in] Zero-based index to search
// CONST TCHAR*    pOutput  : [in] Name of the variable
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BOOL  isVariableParameterInCommand(CONST COMMAND*  pCommand, CONST TCHAR*  szVariableName)
{
   PARAMETER*  pParameter;
   BOOL        bResult;

   // Prepare
   bResult = FALSE;

   // Iterate through through Parameters until match is found
   for (UINT  iIndex = 0; !bResult AND findParameterInCommandByIndex(pCommand, PT_DEFAULT, iIndex, pParameter); iIndex++)
      /// [CHECK] Is this a matching variable?
      bResult = isVariableParameter(pParameter) AND utilCompareStringVariables(identifyRawVariableName(szVariableName), identifyRawVariableName(pParameter->szValue));

   // Return result
   return bResult;
}


/// Function name  : isVariableParameterInCommandAtIndex
// Description     : Checks whether a certain variable is at a specific index
// 
// CONST COMMAND*  pCommand : [in] COMMAND to search
// CONST UINT      iIndex   : [in] Zero-based index to search
// CONST TCHAR*    pOutput  : [in] Name of the variable
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BOOL  isVariableParameterInCommandAtIndex(CONST COMMAND*  pCommand, CONST UINT  iIndex, CONST TCHAR*  szVariableName)
{
   PARAMETER*  pParameter;
   BOOL        bResult;

   // Prepare
   bResult = FALSE;

   /// [CHECK] Is there a Variable/ReturnVariable at the specified index?
   if (findParameterInCommandByIndex(pCommand, PT_DEFAULT, iIndex, pParameter) AND isVariableParameter(pParameter))
      // [SUCCESS] Compare variable names
      bResult = utilCompareStringVariables(identifyRawVariableName(szVariableName), identifyRawVariableName(pParameter->szValue));
   
   // Return result
   return bResult;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////


