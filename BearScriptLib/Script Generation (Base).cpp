//
// Script Generation (Base).cpp : Provides the helper functions for MSCI script generation
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createCommandHash
// Description     : Creates a CommandHash object
// 
// Return Value   : New CommandHash object, you are responsible for destroying it
// 
BearScriptAPI
COMMAND_HASH*  createCommandHash()
{
   COMMAND_HASH*  pCommandHash;    // Object being created

   // Create new object
   pCommandHash = utilCreateEmptyObject(COMMAND_HASH);

   // Create properties
   pCommandHash->szCommandHash     = utilCreateEmptyString(HASH_LENGTH);
   pCommandHash->szConditionalHash = utilCreateEmptyString(HASH_LENGTH);

   // Return new object
   return pCommandHash;
}


/// Function name  : createParameterIndex
// Description     : Create a new ParameterIndex object, used for converting between the physical and logical indicies of COMMAND's PARAMETERS
// 
// CONST UINT  iPhysicalIndex  : [in] Zero-based physical index (Display Index)
// CONST UINT  iLogicalIndex   : [in] Zero-based logical index (ScriptNode index)
// 
// Return Value   : New ParameterIndex object, you are responsible for destroying it
// 
PARAMETER_INDEX*  createParameterIndex(CONST UINT  iPhysicalIndex, CONST UINT  iLogicalIndex)
{
   PARAMETER_INDEX*   pParameterIndex; // Parameter being processed

   // Create new object
   pParameterIndex = utilCreateEmptyObject(PARAMETER_INDEX);

   // Set properties
   pParameterIndex->iLogicalIndex  = iLogicalIndex;
   pParameterIndex->iPhysicalIndex = iPhysicalIndex;

   // Return result
   return pParameterIndex;
}


/// Function name  : deleteCommandHash
// Description     : Deletes a CommandHash object
// 
// COMMAND_HASH*  &pCommandHash   : [in] CommandHash object to destroy
// 
BearScriptAPI
VOID  deleteCommandHash(COMMAND_HASH*  &pCommandHash)
{
   // Free strings
   utilDeleteStrings(pCommandHash->szCommandHash, pCommandHash->szConditionalHash);

   // Delete calling object
   utilDeleteObject(pCommandHash);
}


/// Function name  : deleteParameterIndex
// Description     : Deletes a ParameterIndex object
// 
// PARAMETER_INDEX*  pParameterIndex   : [in] ParameterIndex object to delete 
// 
VOID  deleteParameterIndex(PARAMETER_INDEX*  pParameterIndex)
{
   // Delete calling object
   utilDeleteObject(pParameterIndex);
}


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////


/// Function name  : appendCodeObjectToHash
// Description     : Append the current CodeObject to the specified hash
// 
// CONST CODEOBJECT*  pCodeObject : [in]     CodeObject containing the text to append
// TCHAR*             szHash      : [in/out] Hash that CodeObject will be appended to (Must be HASH_LENGTH long)
// 
VOID   appendCodeObjectToHash(CONST CODEOBJECT*  pCodeObject, TCHAR*  szHash)
{
   // Append text to the specified hash
   StringCchCat(szHash, HASH_LENGTH, pCodeObject->szText);
}

/// Function name  : calculateWareEncoding
// Description     : Encodes a MainType and SubType into the format used in MSCI scripts
// 
// CONST MAIN_TYPE  eMainType  : [in] Category of the ware
// CONST UINT       iSubType   : [in] Individual ware identifier
// 
// Return Value   : Packed 32-bit WareID -- SubType in LOWORD, MainType in HIWORD.
// 
INT   calculateWareEncoding(CONST GAME_OBJECT*  pGameObject)
{
   return MAKE_LONG(pGameObject->iID, pGameObject->eMainType);
}


/// Function name  : identifyDataTypeFromGamePageID
// Description     : Identifies the underlying datatype of the object represented by a ScriptObject
// 
// CONST SCRIPT_OBJECT*  pScriptObject : [in] ScriptObject to query
// 
// Return Value   : Associated datatype
// 
DATA_TYPE   identifyDataTypeFromScriptObject(CONST SCRIPT_OBJECT*  pScriptObject)
{
   DATA_TYPE  eOutput;      // Operation result

   // Convert common page IDs into associated data types
   switch (pScriptObject->eGroup)
   {
   case ONG_CONSTANT:         eOutput = DT_CONSTANT;         break;    // Constant
   ///case ONG_FORMATION:        eOutput = DT_CONSTANT;         break;    // Formation       REM: Don't exist
   case ONG_DATA_TYPE:        eOutput = DT_DATATYPE;         break;    // Variable Data Type
   case ONG_FLIGHT_RETURN:    eOutput = DT_FLIGHTRETURN;     break;    // Flight Return Code
   case ONG_OBJECT_CLASS:     eOutput = DT_OBJECTCLASS;      break;    // Object Class
   case ONG_RACE:             eOutput = DT_RACE;             break;    // Race
   case ONG_RELATION:         eOutput = DT_RELATION;         break;    // Relation
   case ONG_PARAMETER_TYPE:   eOutput = DT_SCRIPTDEF;        break;    // Parameter Syntax
   case ONG_SECTOR:           eOutput = DT_SECTOR;           break;    // Sector
   case ONG_STATION_SERIAL:   eOutput = DT_STATIONSERIAL;    break;    // Station Serial
   case ONG_SIGNAL:                                                    // Signal
   case ONG_OBJECT_COMMAND:   eOutput = DT_OBJECTCOMMAND;    break;    // Object Command
   case ONG_WING_COMMAND:     eOutput = DT_WINGCOMMAND;      break;    // Wing Command
   case ONG_TRANSPORT_CLASS:  eOutput = DT_TRANSPORTCLASS;   break;    // Transport class

   // [REMAINDER] ERROR: These are not ScriptObjects, they have no equivilient datatype and should be resolved in this way
   default:    ASSERT(FALSE); eOutput = DT_NULL;               break;
   }

   // Return result
   return eOutput;
}


/// Function name  : setCodeEditLineError
// Description     : Highlight the CodeEdit line that contains the source text of a COMMAND with an error or warning underline, if appropriate
// 
// HWND            hCodeEdit : [in] Window handle of the Script/CodeEdit
// CONST COMMAND*  pCommand    : [in] COMMAND object to examine
// 
VOID   setCodeEditLineError(HWND  hCodeEdit, CONST COMMAND*  pCommand)
{
   ERROR_TYPE    eSeverity;      // Whether error is a warning or not

   // [CHECK] Ensure COMMAND has errors
   if (hasErrors(pCommand->pErrorQueue))
   {
      // Determine severity
      eSeverity = identifyErrorQueueType(pCommand->pErrorQueue);

      // Inform CodeEdit of error(s)
      CodeEdit_SetLineError(hCodeEdit, pCommand->iLineNumber, eSeverity, pCommand->pErrorQueue);
   }
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : appendParameterToXMLTree
// Description     : Converts a PARAMETER into XMLTree nodes and adds them an XMLTree
///                     NB: This converts 'internal' values within a PARAMETER to their appropriate 'external' counterparts
// 
// CONST COMMAND*    pCommand   : [in]     COMMAND containing the input PARAMETER
// CONST PARAMETER*  pParameter : [in]     PARAMETER to convert into tree nodes
// XML_TREE*         pTree      : [in/out] Tree containing the parent node
// XML_TREE_NODE*    pParent    : [in/out] Parent node to append the new node to
// 
// Return Value   : TRUE
// 
BOOL  appendParameterToXMLTree(SCRIPT_FILE*  pScriptFile, CONST COMMAND*  pCommand, CONST PARAMETER*  pParameter, XML_TREE*  pTree, XML_TREE_NODE*  pParent)
{
   ERROR_STACK* pError;           // Dummy error for 'performScriptConversion..', which doesn't return errors on generation
   TCHAR*       szOutputString;   // Parameter string value with XML safety phrases encoded
   DATA_TYPE    eOutputType;      // Parameter type properly encoded for MSCI scripts
   INT          iOutputValue;     // Parameter value properly encoded for MSCI scripts

   // Examine syntax
   switch (pParameter->eSyntax)
   {
   /// [PURE VARIABLE] Output SINGLE INTEGER or SINGLE STRING  (Used by 'inc' and 'dec' only)
   case PS_VARIABLE: 
      // [NORMAL] Output as variable ID
      if (!isCommandType(pCommand, CT_CMD_COMMENT)) 
         appendSourceValueIntegerNodeToXMLTree(pTree, pParent, pParameter->iValue);
      // [COMMAND COMMENT] Output as variable name
      else
         appendSourceValueStringNodeToXMLTree(pTree, pParent, pParameter->szValue);
      break;

   /// [RETURN OBJECT] Output SINGLE INTEGER
   case PS_RETURN_OBJECT:
   case PS_RETURN_OBJECT_IF:
   case PS_RETURN_OBJECT_IF_START:
   case PS_INTERRUPT_RETURN_OBJECT_IF:
      appendSourceValueIntegerNodeToXMLTree(pTree, pParent, pParameter->iValue);
      break;

   /// [SCRIPT-NAME/LABEL-NAME] Output SINGLE STRING
   case PS_SCRIPT_NAME:
   case PS_LABEL_NAME:
      appendSourceValueStringNodeToXMLTree(pTree, pParent, pParameter->szValue);
      break;

   /// [COMMENT] Output as SINGLE EXTERNAL STRING
   case PS_COMMENT: 
      // Convert string to external, if necessary
      generateConvertedString(pParameter->szValue, SPC_SCRIPT_DISPLAY_TO_EXTERNAL, szOutputString);
      // Output string
      appendSourceValueStringNodeToXMLTree(pTree, pParent, szOutputString ? szOutputString : pParameter->szValue);
      // Cleanup
      utilSafeDeleteString(szOutputString);
      break;

   /// [LABEL-NUMBER] Output as SINGLE INTEGER or SINGLE STRING
   case PS_LABEL_NUMBER:
      // [STANDARD] Output as Line Number
      if (!isCommandType(pCommand, CT_CMD_COMMENT))
         appendSourceValueIntegerNodeToXMLTree(pTree, pParent, pParameter->iValue);
      // [COMMAND COMMENT] Output as a Label Name
      else
         appendSourceValueStringNodeToXMLTree(pTree, pParent, pParameter->szValue);
      break;

   /// [DEFAULT] Output as TUPLE
   default:
      // [TYPE] Convert to external if appropriate and output as SINGLE INTEGER
      performScriptTypeConversion(pParameter, pParameter->eType, eOutputType, SVT_INTERNAL, SVT_EXTERNAL, pError);
      appendSourceValueIntegerNodeToXMLTree(pTree, pParent, eOutputType);

      // [VALUE] Convert to external if appropriate, and output as an string/integer
      if (pParameter->eType == DT_STRING)
      {
         // [STRING] Convert to external, if necessary
         generateConvertedString(pParameter->szValue, SPC_SCRIPT_DISPLAY_TO_EXTERNAL, szOutputString);
         appendSourceValueStringNodeToXMLTree(pTree, pParent, utilEither(szOutputString, pParameter->szValue));
         // Cleanup
         utilSafeDeleteString(szOutputString);
      }
      else
      {
         //PARAMETER*  pVariableParameter;
         //ARGUMENT*   pArgument;

         //// [CHECK] $0 is datatyp[$1] == $2
         //if (pCommand->iID == 125 AND pParameter->eType == DT_DATATYPE AND (pParameter->iValue == DT_SHIP OR pParameter->iValue == DT_STATION OR pParameter->iValue == DT_OBJECT))
         //{
         //   // [CHECK] Lookup parameter
         //   if (findParameterInCommandByIndex(pCommand, PT_DEFAULT, 1, pVariableParameter) AND pVariableParameter->eType == DT_VARIABLE)
         //   {
         //      // Iterate through the ScriptFile ARGUMENTS
         //      for (UINT iIndex = 0; findArgumentInScriptFileByIndex(pScriptFile, iIndex, pArgument); iIndex++)
         //      {
         //         if (utilCompareStringVariables(&pVariableParameter->szValue[1], pArgument->szName))
         //         {  // [FOUND] Output without conversion
         //            appendSourceValueIntegerNodeToXMLTree(pTree, pParent, pParameter->iValue);
         //            return TRUE;
         //         }
         //      }
         //   }
         //}
         
         // [INTEGER] Convert to external, if necessary
         performScriptValueConversion(pScriptFile, pParameter, iOutputValue, SVT_INTERNAL, SVT_EXTERNAL, pError);
         appendSourceValueIntegerNodeToXMLTree(pTree, pParent, iOutputValue);
      }
   }

   // Return TRUE
   return TRUE;
}


/// Function name  : findParameterSyntaxByLogicalIndex
// Description     : Locate a COMMAND's PARAMETER_SYNTAX by logical index. Also calculates it's physical index.
// 
// CONST COMMAND_SYNTAX*  pSyntax  : [in]     CommandSyntax object to query
// PARAMETER_INDEX*       pIndex   : [in/out] ParameterIndex containing the LOGICAL INDEX to search for.
//                                              If successful this will Contain the associated PHYSICAL INDEX on return
// PARAMETER_SYNTAX&      eOutput  : [out]    Desired ParameterSyntax if found, otherwise PS_UNDETERMINED
// 
// Return Value   : TRUE if found, FALSE otherwise
//
//             $1 $0 script command $3 blah $2
//    -------------------------------------------
//    Logical   0  1                2       3
//    Physical  1  0                3       2
// 
BOOL   findParameterSyntaxByLogicalIndex(CONST COMMAND_SYNTAX*  pSyntax, PARAMETER_INDEX*  pIndex, PARAMETER_SYNTAX&  eOutput)
{
   CODEOBJECT*   pCodeObject;    // CodeObject for tokenising the syntax string
   UINT          iLogicalIndex;  // logical index iterator

   // Prepare
   pCodeObject   = createCodeObject(pSyntax->szSyntax);
   eOutput       = PS_UNDETERMINED;
   iLogicalIndex = 0;

   /// Iterate through the insertion points in their logical order until the syntax is found or we run out of codeobjects
   while (eOutput == PS_UNDETERMINED AND findNextCodeObject(pCodeObject))
   { 
      switch (pCodeObject->eClass)
      {
      /// [INSERTION POINT] It's number represents it's PHYSICAL INDEX
      case CO_VARIABLE:
         // Skip each insertion point until we find the one with the desired logical index
         if (iLogicalIndex++ == pIndex->iLogicalIndex)
         {  
            // Set the physical index using the insertion point
            pIndex->iPhysicalIndex = utilConvertCharToInteger(pCodeObject->szText[1]);

            /// [FOUND] Lookup the desired parameter syntax using the PHYSICAL INDEX
            findParameterSyntaxByPhysicalIndex(pSyntax, pIndex->iPhysicalIndex, eOutput);
         }
         break;

      /// [COMMENT] SPECIAL CASE: Manually return the data - the insertion points are skipped due to the comment operator
      case CO_COMMENT:
         if (pIndex->iLogicalIndex == 0)
         {
            // Return 'COMMENT' at PHYSICAL INDEX zero
            eOutput = PS_COMMENT;
            pIndex->iPhysicalIndex = 0;
         }
         break;
      }
   }

   // Cleanup and return result
   deleteCodeObject(pCodeObject);
   return (eOutput != PS_UNDETERMINED);
}



/// Function name  : findPostfixParameterIndexByID
// Description     : Determine the index of a PARAMETER within a COMMAND's postfix array, using it's unique parameter ID
// 
// CONST COMMAND*  pCommand  : [in]  COMMAND containing the PARAMETER
// CONST UINT      iID       : [in]  ID of the PARAMETER to search for
// UINT&           iOutput   : [out] Index of the desired PARAMETER if found, otherwise NULL
// 
// Return Value   : TRUE if found, otherwise FALSE
// 
BOOL  findPostfixParameterIndexByID(CONST COMMAND*  pCommand, CONST UINT  iID, UINT&  iOutput)
{
   PARAMETER*   pParameter;      // PARAMETER array iterator
   BOOL         bFound;          // Search processing flag

   // Prepare
   iOutput = NULL;
   bFound  = FALSE;

   /// Iterate through input COMMAND's Postfix PARAMETER array
   for (UINT iIndex = 0; !bFound AND findParameterInCommandByIndex(pCommand, PT_POSTFIX, iIndex, pParameter); iIndex++)
      if (bFound = (pParameter->iID == iID))
         // [FOUND] Set output to parameter index and stop searching
         iOutput = iIndex;
      
   // Return TRUE if PARAMETER was located
   return bFound;
}

/// Function name  : isExpressionConditional
// Description     : Checks whether a conditional is compatible with an expression
// 
// const CONDITIONAL_ID  eConditional   : [in] Conditional to test
// 
// Return Value   : TRUE/FALSE
// 
BOOL  isExpressionConditional(const CONDITIONAL_ID  eConditional)
{
   switch (eConditional)
   {
   // [IF/WHILE]
   default:
      return TRUE;

   // [EVERYTHING ELSE]
   case CI_START:
   case CI_BREAK:
   case CI_CONTINUE:
   case CI_ELSE:
   case CI_END:
   case CI_LABEL:
   case CI_GOTO_LABEL:
   case CI_GOTO_SUB:
   case CI_END_SUB:
      return FALSE;
   }
}

/// Function name  : identifyCommandFromHash
// Description     : Set the ID and syntax of a COMMAND from a hash
// 
// CONST SCRIPT_FILE*   pScriptFile  : [in]     ScriptFile containing the COMMAND 
// COMMAND*             pCommand     : [in/out] COMMAND to set the ID and syntax for (Must contain the command text in it's buffer)
// CONST COMMAND_HASH*  pCommandHash : [in]     CommandHash containing basic command info and hashes
// ERROR_STACK*        &pError       : [out]    Error message, if any
// 
// Return Value   : TRUE if succesful, FALSE otherwise
// 
BOOL  identifyCommandFromHash(CONST SCRIPT_FILE*  pScriptFile, COMMAND*  pCommand, CONST COMMAND_HASH*  pCommandHash, ERROR_STACK*  &pError)
{
   // Prepare
   pError = NULL;

   // [COMMENT] -- 'COMMENTS' are previously identified by the CommandHash
   if (pCommandHash->bComment)
      pCommand->iID = CMD_COMMENT;

   // [ELSE] -- 'ELSE' has no command hash but a conditional of 'else'
   //else if (!lstrlen(pCommandHash->szCommandHash) AND pCommand->eConditional == CI_ELSE)
   else if (pCommand->eConditional == CI_ELSE)
      pCommand->iID = CMD_ELSE;

   // [NOP] -- 'NOP' cannot be identified by a hash
   else if (!lstrlen(pCommand->szBuffer) OR isStringWhitespace(pCommand->szBuffer))
      pCommand->iID = CMD_NOP;
      
   // [DEFINE LABEL] -- 'label:' has a command hash of entirely operators but is not an expression
   else if (utilCompareString(pCommandHash->szCommandHash, ":"))
      pCommand->iID = CMD_DEFINE_LABEL;

   // [SCRIPT CALL] -- Can only be identified by the start of the command hash as the ending depends on the script arguments
   else if (utilCompareStringN(pCommandHash->szCommandHash, "callscript:", 11))
      pCommand->iID = CMD_CALL_SCRIPT_VAR_ARGS;

   // [DIM ARRAY] -- Can only be identified by the start of the command hash as the ending depends on the script arguments
   else if (utilCompareStringN(pCommandHash->szCommandHash, "dim", 3))
      pCommand->iID = CMD_DEFINE_ARRAY;
   
   // [EXPRESSION] -- Identified by having a hash that contains only operators
   else if (isExpressionConditional(pCommand->eConditional) AND identifyExpressionHash(pCommandHash->szCommandHash))   
      pCommand->iID = CMD_EXPRESSION;

   /// [PRE-IDENTIFIED] Lookup syntax from the ID
   if (pCommand->iID != CMD_NONE)
      findCommandSyntaxByID(pCommand->iID, pScriptFile->eGameVersion, pCommand->pSyntax, pError);
   
   /// [UNIDENTIFIED] Resolve syntax from the hash 
   else if (findCommandSyntaxByHash(pCommandHash->szCommandHash, pScriptFile->eGameVersion, pCommand->pSyntax, pError))
      // [SUCCESS] Set ID using the syntax
      pCommand->iID = pCommand->pSyntax->iID; 

   /// [COMMAND TYPE] -- Determine the Command type from it's ID
   switch (pCommand->iID)
   {
   // [AUXILIARY]
   case CMD_COMMENT:
   case CMD_NOP:
   case CMD_COMMAND_COMMENT:      // TODO: Implement this
   case CMD_END:
   case CMD_ELSE:
   case CMD_BREAK:
   case CMD_CONTINUE:               pCommand->iFlags = CT_AUXILIARY;                       break;
   // [SCRIPT-CALL]
   case CMD_CALL_SCRIPT_VAR_ARGS:   pCommand->iFlags = CT_STANDARD WITH CT_SCRIPTCALL;     break;
   // [EXPRESSION]
   case CMD_EXPRESSION:             pCommand->iFlags = CT_STANDARD WITH CT_EXPRESSION;     break;
   // [STANDARD]
   default:                         pCommand->iFlags = CT_STANDARD;                        break;
   }

   // [SCRIPT-CALL]
   if (isCommandScriptCall(pCommand))
      pCommand->iFlags |= CT_SCRIPTCALL;

   // [MACRO] Add the 'virtual' flag
   if (pCommand->pSyntax AND pCommand->pSyntax->eGroup == CG_MACRO) 
      pCommand->iFlags |= CT_VIRTUAL;
   
   // Return TRUE if there were no errors
   return (pError == NULL);
}


/// Function name  : identifyConditionalFromHash
// Description     : Identify a command's conditional from it's hash, and store the result in a COMMAND
// 
// COMMAND*             pCommand     : [in/out] COMMAND to store the result in
// CONST COMMAND_HASH*  pCommandHash : [in]     CommandHAsh containing the basic properties of the command
// ERROR_STACK*        &pError       : [out]    Error message if any, you are responsible for destroying it
// 
// Return Value   : TRUE if successful, FALSE otherwise
// 
BOOL   identifyConditionalFromHash(COMMAND*  pCommand, CONST COMMAND_HASH*  pCommandHash, ERROR_STACK*  &pError)
{
   CONDITIONAL_ID    eResult;    // Operation result

   // Prepare
   pError  = NULL;
   eResult = CI_NONE;

   // [EMPTY HASH, COMMENT] Skip checks
   if (!lstrlen(pCommandHash->szConditionalHash) OR pCommandHash->bComment)
      eResult = CI_NONE;
   
   // [IF] 
   else if (utilCompareString(pCommandHash->szConditionalHash, "if"))
      eResult = CI_IF;
   else if (utilCompareString(pCommandHash->szConditionalHash, "ifnot"))
      eResult = CI_IF_NOT;

   // [SKIP-IF, SKIP-IF-NOT]
   else if (utilCompareString(pCommandHash->szConditionalHash, "skipif"))
      eResult = CI_SKIP_IF;
   else if (utilCompareString(pCommandHash->szConditionalHash, "skipifnot") OR utilCompareString(pCommandHash->szConditionalHash, "doif"))
      eResult = CI_SKIP_IF_NOT;

   // [ELSE, ELSE-IF, ELSE-IF-NOT]
   else if (utilCompareString(pCommandHash->szConditionalHash, "else"))
      eResult = CI_ELSE;
   else if (utilCompareString(pCommandHash->szConditionalHash, "elseif"))
      eResult = CI_ELSE_IF;
   else if (utilCompareString(pCommandHash->szConditionalHash, "elseifnot"))
      eResult = CI_ELSE_IF_NOT;

   // [WHILE, WHILE-NOT]
   else if (utilCompareString(pCommandHash->szConditionalHash, "while"))
      eResult = CI_WHILE;
   else if (utilCompareString(pCommandHash->szConditionalHash, "whilenot"))
      eResult = CI_WHILE_NOT;

   // [START]
   else if (utilCompareString(pCommandHash->szConditionalHash, "start"))
      eResult = CI_START;

   // [ERROR] "Cannot identify the conditional from the hash '%s'"
   else
      pError = generateDualError(HERE(IDS_GENERATION_CONDITIONAL_HASH_UNRECOGNISED), pCommandHash->szConditionalHash);
   
   // Set result and return TRUE if there was no error
   pCommand->eConditional = eResult;
   return (pError == NULL);
}


/// Function name  : identifyExpressionHash
// Description     : Determine if a command is an expression using the hash and a full COMMAND object
// 
// CONST TCHAR*  szHash : user command hash
// 
// Return type : TRUE if an expression, FALSE if not
//
BOOL   identifyExpressionHash(CONST TCHAR*  szHash)
{
   CODEOBJECT*  pCodeObject;     // CodeObject for tokenising the input hash
   BOOL         bIsExpression;   // Operation result
   
   // Create CodeObject and set default for an empty hash to TRUE
   pCodeObject   = createCodeObject(szHash);
   bIsExpression = TRUE;
   
   // Check each CodeObject in hash is an operator
   while (findNextCodeObject(pCodeObject) AND bIsExpression)
   {
      switch (pCodeObject->eClass)
      {
      // [OPERATOR] -- Don't allow '[' and ']' since if they're present they indicate an array index - and arrays can't be used in expressions.
      case CO_OPERATOR:
         if (pCodeObject->szText[0] == '[' OR pCodeObject->szText[0] == ']')
            bIsExpression = FALSE;
         continue;

      // [COMMENT] -- Happens when the hash begins with a '*'.  Recursively analyse the comment text to see if it contains operators
      case CO_COMMENT:
         if (pCodeObject->iLength > 1)
            bIsExpression = identifyExpressionHash(&pCodeObject->szText[1]);
         continue;

      // [WORD] -- Check whether the word is a series of logical operators, eg. ANDOR: formed from '$a AND $b OR $c'
      case CO_WORD:
         bIsExpression = isStringConcatenatedLogicalOperator(pCodeObject->szText);
         continue;

      // [DEFAULT] -- Not an expression
      default:
         bIsExpression = FALSE;
         continue;
      }
   }
   
   // Cleanup and return
   deleteCodeObject(pCodeObject);
   return bIsExpression;
}



/// Function name  : isStringConcatenatedLogicalOperator
// Description     : Determines whether a string is a series of logical operators squashed together
// 
// CONST TCHAR*  szString   : [in] String to test
// 
// Return Value   : TRUE or FALSE
// 
BOOL  isStringConcatenatedLogicalOperator(CONST TCHAR*  szString)
{
   UINT    iIndex,            // Current character index
           iInputLength;      // Length of the input string, in characters

   // Prepare
   iIndex       = 0;
   iInputLength = lstrlen(szString);

   // [CHECK] Return FALSE for empty string
   if (!lstrlen(szString))
      return FALSE;
   
   /// Examine each 2/3 letter phrase
   while (iIndex < iInputLength)
   {
      // [THREE LETTER PHRASE]
      if (utilCompareStringN(&szString[iIndex], "and", 3) OR utilCompareStringN(&szString[iIndex], "mod", 3))
         iIndex += 3;
      // [TWO LETTER PHRASE]
      else if (utilCompareStringN(&szString[iIndex], "or", 2))
         iIndex += 2;
      else
         break;
   }
   
   // Return TRUE if entire phrase was searched successfully 
   return (iIndex == iInputLength);
}


/// Function name  : performScriptCommandHashing
// Description     : Generates two hashes - one for a script command and another for it's conditional. Also 
//                      detects whether the command is commented or not
// 
// CONST TCHAR*    szScriptCommand : [in]     Script command to hash
// COMMAND_HASH*   pCommandHash    : [in/out] CommandHash to hold the results
// 
BearScriptAPI
VOID   performScriptCommandHashing(CONST TCHAR*  szScriptCommand, COMMAND_HASH*  pCommandHash)
{
   CODEOBJECT*  pCodeObject;     // CodeObject for tokenising the script command

   // Prepare
   pCodeObject = createCodeObject(szScriptCommand);

   /// Tokenise the command into CodeObjects
   while (findNextCodeObject(pCodeObject))
   {
      // Examine the CodeObject class
      switch (pCodeObject->eClass)
      {
      /// [OPERATOR] Add most of these to the COMMAND hash
      case CO_OPERATOR:
         // [REFERENCE] Skip
         if (utilCompareString(pCodeObject->szText, "->"))
            break;
         // [ASSIGNMENT/DISCARD] Skip
         else if (utilCompareString(pCodeObject->szText, "=") AND (pCodeObject->iIndex == 1 OR pCodeObject->iIndex == 2))
            break;         

         appendCodeObjectToHash(pCodeObject, pCommandHash->szCommandHash);
         break;

      /// [WORD] Add all of these to the COMMAND hash
      case CO_WORD:
         appendCodeObjectToHash(pCodeObject, pCommandHash->szCommandHash);
         break;

      /// [KEYWORD] Add 'pure' conditionals to the CONDITIONAL hash, and 'fakes' like 'continue' to the COMMAND hash
      case CO_KEYWORD:
         appendCodeObjectToHash(pCodeObject, isCodeObjectCommandKeyword(pCodeObject) ? pCommandHash->szCommandHash : pCommandHash->szConditionalHash);
         break;

      /// [COMMENT] Recursively analyse the contents of the comment to identify command comments
      case CO_COMMENT:
         // [COMMENT] Recurse into the comment and attempt to hash the 'commented command', if any
         if (!pCommandHash->bComment)
         {
            pCommandHash->bComment = TRUE;
            performScriptCommandHashing(&pCodeObject->szText[1], pCommandHash);
         }
         // [ALREADY DETECTED] Add to the COMMAND hash
         else
            appendCodeObjectToHash(pCodeObject, pCommandHash->szCommandHash);
         break;
      }
   }

   // Cleanup and return
   deleteCodeObject(pCodeObject);
}


/// Function name  : verifyParameterDataType
// Description     : Checks whether a PARAMETER's datatype is valid, based on it's syntax.
//
//                      A PARAMETER's datatype is set manually by generateParameterFromCodeObject(), for most types it's determined
//                      by the GamePage the string was found in.  For basic types it's manually.  DT_CONSTANT is not the same as a CodeObject
//                      constant - it indicates an entry from within GPI_CONSTANTS (2002) containing [TRUE], [FALSE], Quest.Continued, Wing.Blue etc.
// 
// CONST PARMETER*  pParameter : [in]  Parmeter to verify
// 
// Return Value   : TRUE if successful, FALSE otherwise
// 
BOOL   verifyParameterDataType(CONST PARAMETER*  pParameter)
{
   // [NULL] NULL can be used for pretty much anything, except maybe the return object
   if (pParameter->eType == DT_NULL)
      return !isReturnObject(pParameter->eSyntax);

   /// Determine whether datatype is valid for the syntax
   switch (pParameter->eSyntax)
   {
   // [UNDETERMINED] should never reach here
   default:
	case PS_UNDETERMINED:
      ASSERT(FALSE);
      return FALSE;

   // [UNIVERSAL] These can be any datatype
   case PS_VALUE:
   case PS_EXPRESSION:
   case PS_PARAMETER: 
      return TRUE;


   /// [PURE] Require a specific datatype
   

   // [FLIGHT RETURN] Stored using their own datatypes
	case PS_FLIGHTRETCODE:
      return pParameter->eType == DT_FLIGHTRETURN OR pParameter->eType == DT_VARIABLE;

   // [LABEL NAME, SCRIPT NAME, STRING, COMMENT] Always held as strings
	case PS_LABEL_NAME:
	case PS_SCRIPT_NAME:
   case PS_COMMENT:
   case PS_STRING:
      return pParameter->eType == DT_STRING;

   // [NUMBER] Integers only
   case PS_NUMBER:
      return pParameter->eType == DT_INTEGER;

   // [REFERENCE OBJECT] Constants and Variables only
	case PS_REFERENCE_OBJECT:
      return pParameter->eType == DT_VARIABLE OR pParameter->eType == DT_CONSTANT;
   
   // [RELATIONS] Stored using their own datatypes
	case PS_RELATION:
      return pParameter->eType == DT_RELATION OR pParameter->eType == DT_VARIABLE;

   // [RETURN OBJECT] Always held as integers
	case PS_RETURN_OBJECT:
	case PS_RETURN_OBJECT_IF:
	case PS_RETURN_OBJECT_IF_START:
	case PS_INTERRUPT_RETURN_OBJECT_IF:
      return pParameter->eType == DT_INTEGER;

   // [SCRIPT REFERENCE TYPE]
   case PS_SCRIPT_REFERENCE_TYPE:
      return pParameter->eType == DT_SCRIPTDEF OR pParameter->eType == DT_VARIABLE;

   // [SECTOR POSITION] What about DT_ARRAY?
	case PS_SECTOR_POSITION:
      return pParameter->eType == DT_VARIABLE;

   // [VARIABLE]
	case PS_VARIABLE:
      return pParameter->eType == DT_VARIABLE;



   /// [MIXED] Can be the appropriate datatype or a variable
   // [ARRAY / VARIABLE]
	case PS_ARRAY:
      return pParameter->eType == DT_VARIABLE;

   // [BOOLEAN / VARIABLE]
   case PS_VARIABLE_BOOLEAN:  /// Compatible with: [TRUE] and [FALSE]
      return pParameter->eType == DT_CONSTANT OR pParameter->eType == DT_VARIABLE;

   // [CONSTANT / VARIABLE]
	case PS_VARIABLE_CONSTANT:
      return pParameter->eType == DT_CONSTANT OR pParameter->eType == DT_VARIABLE;

   // [COMMAND / SIGNAL] 
	case PS_OBJECTCOMMAND:     
	case PS_OBJECTCOMMAND_OBJECTSIGNAL:
	case PS_OBJECTSIGNAL:
      return pParameter->eType == DT_OBJECTCOMMAND OR pParameter->eType == DT_VARIABLE;

   // [DATATYPE / VARIABLE]
	case PS_VARIABLE_DATATYPE:
      return pParameter->eType == DT_DATATYPE OR pParameter->eType == DT_VARIABLE;

   // [ENVIRONMENT / VARIABLE] 
   case PS_VARIABLE_ENVIRONMENT: /// Compatible with: [ENVIRONMENT]
      return pParameter->eType == DT_CONSTANT OR pParameter->eType == DT_VARIABLE;

   // [GLOBAL PARAMETER] Stored as string
   case PS_VARIABLE_GLOBAL_PARAMETER:     /// NEW: Work in progress
      return pParameter->eType == DT_STRING OR pParameter->eType == DT_VARIABLE;

   // [NUMBER / VARIABLE] Integer, Constant or Variable
   case PS_VARIABLE_NUMBER:      /// Compatible with: [TRUE], [FALSE], [MAX]?
      return pParameter->eType == DT_INTEGER OR pParameter->eType == DT_CONSTANT OR pParameter->eType == DT_VARIABLE;

   // [OBJECT CLASS / VARIABLE] Stored using it's own datatypes
	case PS_VARIABLE_CLASS:
      return pParameter->eType == DT_OBJECTCLASS OR pParameter->eType == DT_VARIABLE; // OR pParameter->eType == DT_CONSTANT;

   // [PASSENGER / VARIABLE] What about DT_PASSENGER?
   case PS_VARIABLE_PASSENGER:
   case PS_VARIABLE_PASSENGER_OF_SHIP:
      return pParameter->eType == DT_VARIABLE;

   // [QUEST / VARIABLE]      
   case PS_VARIABLE_QUEST:    /// Compatible with: Quest.xxx constants
      return pParameter->eType == DT_VARIABLE OR pParameter->eType == DT_QUEST OR pParameter->eType == DT_CONSTANT;

   // [RACE / VARIABLE]
   case PS_VARIABLE_RACE:     /// Possibly related: [OWNER]
      return pParameter->eType == DT_VARIABLE OR pParameter->eType == DT_RACE OR pParameter->eType == DT_CONSTANT;

   // [SECTOR / VARIABLE] 
   case PS_VARIABLE_SECTOR:   /// Compatible with: [SECTOR]
      return pParameter->eType == DT_VARIABLE OR pParameter->eType == DT_SECTOR OR pParameter->eType == DT_CONSTANT;

   // [SHIP TYPE / STATION TYPE]
   case PS_VARIABLE_SHIPTYPE:
	case PS_VARIABLE_SHIPTYPE_STATIONTYPE:
   case PS_VARIABLE_STATIONTYPE:
      return pParameter->eType == DT_OBJECTCLASS OR pParameter->eType == DT_VARIABLE OR pParameter->eType == DT_WARE;

   // [SHIP / STATION / VARIABLE] Ship, station, constant or variable
   case PS_VARIABLE_SHIP:              /// Compatible with: [SHIP]
   case PS_VARIABLE_PLAYER_SHIP:
	case PS_VARIABLE_SHIP_STATION:
   case PS_VARIABLE_PLAYER_SHIP_STATION:
	case PS_STATION_CARRIER:
	case PS_STATION_CARRIERDOCKAT:
   case PS_VARIABLE_PLAYER_STATION_CARRIER:
	case PS_VARIABLE_PLAYER_STATION:
	case PS_VARIABLE_STATION:
	case PS_VARIABLE_STATIONPRODUCT:
	case PS_VARIABLE_STATIONRESOURCE:
	   return pParameter->eType == DT_SHIP OR pParameter->eType == DT_STATION OR pParameter->eType == DT_CONSTANT OR 
             pParameter->eType == DT_WARE OR pParameter->eType == DT_VARIABLE;

   // [STATION SERIAL / VARAIBLE]
   case PS_VARIABLE_STATIONSERIAL:
      return pParameter->eType == DT_VARIABLE OR pParameter->eType == DT_STATIONSERIAL;

   // [STRING / VARIABLE] Variable, string or constant
   case PS_VARIABLE_STRING:
      return pParameter->eType == DT_STRING OR pParameter->eType == DT_VARIABLE OR pParameter->eType == DT_CONSTANT;

   // [TRANSPORT CLASS / VARIABLE]
	case PS_VARIABLE_TRANSPORTCLASS:
      return pParameter->eType == DT_VARIABLE OR pParameter->eType == DT_TRANSPORTCLASS; 

   // [UNIVERSE OBJECTS]
	case PS_VARIABLE_ASTEROID:
	case PS_VARIABLE_WARPGATE:
      return pParameter->eType == DT_VARIABLE;

   // [WARE / VARIABLE]
	case PS_VARIABLE_STATIONWARE:
	case PS_VARIABLE_HOMEBASEPRODUCT:
	case PS_VARIABLE_HOMEBASERESOURCE:
	case PS_VARIABLE_HOMEBASEWARE:
   case PS_VARIABLE_ALLWARES:
	case PS_VARIABLE_FLYINGWARE:
	case PS_VARIABLE_WARE:
	case PS_VARIABLE_WARE_OF_SHIP:
      return pParameter->eType == DT_WARE OR pParameter->eType == DT_VARIABLE;

   // [WING / VARIABLE]
   case PS_VARIABLE_WING:     /// Compatible with Wing.xxx constants
      return pParameter->eType == DT_CONSTANT OR pParameter->eType == DT_VARIABLE;
   
   // [WING COMMAND / VARIABLE]
   case PS_VARIABLE_WING_COMMAND:
      return pParameter->eType == DT_WINGCOMMAND OR pParameter->eType == DT_VARIABLE;



   /// [UNKNOWN] Either these aren't used, or I don't know how to use them yet
   // [SHIP AND WARE/PASSENGER] 
	case PS_VARIABLE_SHIP_AND_WARE:
   case PS_VARAIBLE_SHIP_AND_PASSENGER:
   // [JUMP-DRIVE GATE/SECTOR] 
   case PS_VARIABLE_JUMPDRIVE_GATE:
   case PS_VARIABLE_JUMPDRIVE_SECTOR:

      ASSERT(FALSE);
      return FALSE;

   
   // [FLEET COMMANDER] Unknown
   case PS_VARIABLE_FLEET_COMMANDER:      /// NEW: Work in progress
      ASSERT(FALSE);
      return FALSE;

   /// [INVALID] 
   // [CONDITION] Internal to the compiler
	case PS_CONDITION:  
      ASSERT(pParameter->eSyntax != PS_CONDITION);
      return FALSE;

   // [LABEL NUMBER] These are external, you can't specify a label number manually
   case PS_LABEL_NUMBER:
      ASSERT(pParameter->eSyntax != PS_LABEL_NUMBER);
      return FALSE;
   }
}



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                      THREAD FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : threadprocSaveScriptFile
// Description     : Fill a ScriptFile using the text in a CodeEdit and save it to disk as an X3 game script file
// 
// VOID*  pParameter : [in] OPERATION_DATA* : Operation data
//
///                              pScriptFile : [in] ScriptFile containing the script properties
///                              hCodeEdit : [in] Window handle of the CodeEdit containing the script text
///                              szFullPath  : [in] New/Existing Filepath to attempt to save the output into
//
// Return type : 0 if successful
// 
BearScriptAPI
DWORD    threadprocSaveScriptFile(VOID*  pParameter)
{
   DOCUMENT_OPERATION*  pOperationData;         // Operation data
   OPERATION_RESULT     eResult = OR_SUCCESS;   // Operation result
   SCRIPT_FILE*         pScriptFile = NULL;     // Convenience pointer

   // [TRACKING]
   SET_THREAD_NAME("Script Generation");
   setThreadLanguage(getAppPreferences()->eAppLanguage);

   __try
   {
      // [CHECK] Ensure parameter exists
      ASSERT(pParameter);
      
      // Prepare
      pOperationData = (DOCUMENT_OPERATION*)pParameter;
      pScriptFile    = (SCRIPT_FILE*)pOperationData->pGameFile;

      // [STAGE] Set parsing stage
      ASSERT(getOperationProgressStageID(pOperationData->pProgress) == IDS_PROGRESS_PARSING_COMMANDS);

      /// [GENERATION] Parse and Compile all commands. Generate XML tree. Flatten into output buffer.
      if (!generateScript(pOperationData->hCodeEdit, pScriptFile, pOperationData->pProgress, pOperationData->pErrorQueue))
         // [ERROR] No further enhancement necessary
         eResult = OR_FAILURE;
      
      /// [PCK] Compress file before saving
      else if (utilCompareString(PathFindExtension(pScriptFile->szFullPath), ".pck"))
      {
         // [INFO/STAGE] "Writing compressed MSCI script '%s' to disc"
         VERBOSE("Saving script file '%s'", pOperationData->szFullPath);
         pushErrorQueue(pOperationData->pErrorQueue, generateDualInformation(HERE(IDS_OUTPUT_SAVING_SCRIPT_PCK), identifyScriptName(pScriptFile)));
         ASSERT(advanceOperationProgressStage(pOperationData->pProgress) == IDS_PROGRESS_COMPRESSING_SCRIPT);

         /// [COMPRESS] Compress file and save to disc
         if (!saveDocumentToFileSystemEx(pOperationData->szFullPath, pScriptFile, pOperationData->pProgress, pOperationData->pErrorQueue))
            // No enhancement necessary      // [ERROR] "There was an error while saving or compressing the MSCI script '%s', the file was not saved"
            eResult = OR_FAILURE;            // enhanceLastError(pOperationData->pErrorQueue, ERROR_ID(IDS_SCRIPT_SAVE_PCK_IO_ERROR), pOperationData->szFullPath); 
      }
      /// [XML] Output to disc without compression
      else
      {
         // [INFO] "Writing MSCI script '%s' to disc"
         VERBOSE("Saving script file '%s'", pOperationData->szFullPath);
         pushErrorQueue(pOperationData->pErrorQueue, generateDualInformation(HERE(IDS_OUTPUT_SAVING_SCRIPT_XML), identifyScriptName(pScriptFile)));

         /// [SAVE] Save file to disc
         if (!saveDocumentToFileSystem(pOperationData->szFullPath, pScriptFile, pOperationData->pErrorQueue))
            // No enhancement necessary      // [ERROR] "There was an error while saving the MSCI script '%s', the file was not saved"
            eResult = OR_FAILURE;            //enhanceLastError(pOperationData->pErrorQueue, ERROR_ID(IDS_SCRIPT_SAVE_IO_ERROR), pOperationData->szFullPath); 
      }
   }
   PUSH_CATCH(pOperationData->pErrorQueue)
   {
      eResult = OR_FAILURE;
   }

   // Cleanup and return result
   CONSOLE_COMPLETE("SCRIPT GENERATION", eResult);
   closeThreadOperation(pOperationData, eResult);
   return  THREAD_RETURN;
}

