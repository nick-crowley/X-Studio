//
// Script Generation.cpp : Provides the high-level MSCI script generation functions
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

// NOTES: Need to verify the syntax of the Command 514 : "START $0 command $1 : arg1=$2, arg2=$3, arg3=$4, arg4=$5"
//             -> Why is there START in the syntax, and why is there no return object?  Determine whether command can only be called asychronously
//                      upon another object....
//

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       MACROS
/// /////////////////////////////////////////////////////////////////////////////////////////

// OnException: Print ScriptFile
#define  ON_EXCEPTION()     debugScriptFile(pScriptFile);

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : generateCommandFromString
// Description     : Creates a new COMMAND from a specified script command
// 
// CONST SCRIPT_FILE*  pScriptFile   : [in]     ScriptFile containing the COMMAND, used for converting variable names to IDs
// CONST TCHAR*        szCommandText : [in]     Script command text
// CONST UINT          iLineNumber   : [in]     Zero based line number of the script command
// COMMAND*           &pOutput       : [out]    New COMMAND whether successful or not. You are responsible for destroying it
// ERROR_QUEUE*        pErrorQueue   : [in/out] Output error queue
// 
// Return Value   : TRUE if succesful, FALSE otherwise
// 
BearScriptAPI
BOOL  generateCommandFromString(CONST SCRIPT_FILE*  pScriptFile, CONST TCHAR*  szCommandText, CONST UINT  iLineNumber, COMMAND*  &pOutput, ERROR_QUEUE*  pErrorQueue)
{ 
   COMMAND_HASH*  pCommandHash;     // Contains the hashes of the command text
   ERROR_STACK*   pError;           // Operation error, if any
   
   // Prepare
   pCommandHash = createCommandHash();
   pOutput      = createCommandFromText(szCommandText, iLineNumber);
   pError       = NULL;
   
   /// [HASH COMMAND] Ascertain the basic info about the command
   performScriptCommandHashing(szCommandText, pCommandHash);

   /// [IDENTIFY CONDITIONAL] Identify the conditional from it's hash
   if (!identifyConditionalFromHash(pOutput, pCommandHash, pError))
   {
      // [ERROR] "Unrecognised conditional in script command on line %u : '%s'"
      enhanceError(pError, ERROR_ID(IDS_GENERATION_CONDITIONAL_UNRECOGNISED), pOutput->iLineNumber + 1, pOutput->szBuffer);
      pushCommandAndOutputQueues(pError, pErrorQueue, pOutput->pErrorQueue, pError->eType);
   }
   /// [IDENTIFY COMMAND] Identify the command from it's hash
   else if (!identifyCommandFromHash(pScriptFile, pOutput, pCommandHash, pError))
   {
      // [ERROR] "Unrecognised or incompatible script command on line %u : '%s'"
      enhanceError(pError, ERROR_ID(IDS_GENERATION_COMMAND_SYNTAX_INVALID), pOutput->iLineNumber + 1, pOutput->szBuffer);
      pushCommandAndOutputQueues(pError, pErrorQueue, pOutput->pErrorQueue, pError->eType);
   }
   else
   {
      /// [COMMAND VERSION] Produce a warning if this command isn't available in the current GameVersion
      //if (pOutput->pSyntax->eGameVersion > pScriptFile->eGameVersion)
      //{
      //   // [WARNING] "The script command on line %u is not compatible with %s : '%s'"
      //   pError = generateDualWarning(HERE(IDS_GENERATION_COMMAND_INCOMPATIBLE), pOutput->iLineNumber + 1, identifyGameVersionString(pScriptFile->eGameVersion), pOutput->szBuffer);
      //   pushCommandAndOutputQueues(pError, pErrorQueue, pOutput->pErrorQueue, ET_WARNING);
      //   // Don't return FALSE for this warning
      //   pError = NULL;
      //}

      /// [PARSE PARAMETERS] Parse and verify the script command parameters
      if (!generateParameters(pScriptFile, pOutput, pError))
         // No enhancement necessary, all errors include cause + line + command text
         pushCommandAndOutputQueues(pError, pErrorQueue, pOutput->pErrorQueue, pError->eType);

      /// [SPECIAL CASES] Fudge certain COMMAND parameters
      else if (!generateParametersSpecialCases(pScriptFile, pOutput, pError))
         // No enhancement necessary, all errors include cause + line + command text
         pushCommandAndOutputQueues(pError, pErrorQueue, pOutput->pErrorQueue, pError->eType);
   }
   
   // Cleanup and return TRUE if there was no error
   deleteCommandHash(pCommandHash);
   return (pError == NULL);
}


/// Function name  : generateParameterFromCodeObject
// Description     : Create a new PARAMETER from the current text (and class) of a CodeObject
// 
// CONST SCRIPT_FILE* pScriptFile : [in]  ScriptFile containing the PARAMETER, used for converting variable names to IDs
// CONST CODEOBJECT*  pCodeObject : [in]  CodeObject containing the phrase to be converted into a new PARAMETER
// CONST UINT         iLineNumber : [in]  Zero-based Line number of the line containing the command [Used for error reporting only]
// PARAMETER*        &pOutput     : [out] New PARAMETER if succesful, otherwise NULL
// ERROR_STACK*      &pError      : [out] Error message, if unsuccessful
// 
// Return Value   : TRUE if the PARAMETER was created successfully, otherwise FALSE
// 
BOOL   generateParameterFromCodeObject(CONST SCRIPT_FILE*  pScriptFile, CONST CODEOBJECT*  pCodeObject, CONST PARAMETER_SYNTAX  eSyntax, CONST UINT  iLineNumber, PARAMETER*  &pOutput, ERROR_STACK*  &pError)
{
   GAME_OBJECT*    pGameEnumeration;   // Used for resolving enumerations
   SCRIPT_OBJECT*  pGameConstant;      // Used for resolving constants
   OPERATOR_TYPE   eOperator;          // Used for resolving operators
   TCHAR*          szParameterText;    // Used for extracting the relevant portion of the input CodeObject

   // [CHECK] Ensure output and error do not exist
   ASSERT(pOutput == NULL AND pError == NULL);

   // Prepare
   szParameterText = NULL;
   
   /// Determine new PARAMETER datatype based upon CodeObject character class
   switch (pCodeObject->eClass)
   {
   /// [NULL] Create an empty integer PARAMETER
   case CO_NULL:
      pOutput = createParameterFromInteger(NULL, DT_NULL, eSyntax, iLineNumber);
      break;

   /// [STRING] Extract string without the quotes
   case CO_STRING:
      // Extract the variable name
      szParameterText = utilDuplicateSubString(pCodeObject->szText, pCodeObject->iLength, 1, pCodeObject->iLength - 2);
      // Create PARAMETER
      pOutput = createParameterFromString(szParameterText, eSyntax, iLineNumber);
      break;

   /// [VARIABLE] Extract the variable name
   case CO_VARIABLE:
   case CO_ARGUMENT:
      // Extract the variable name
      szParameterText = utilDuplicateSimpleString(identifyRawVariableName(pCodeObject->szText));

      // [CHECK] Ensure variable has a name
      if (!lstrlen(szParameterText))
         // [ERROR] "Missing variable name in the script command on line %u : '%s'"
         pError = generateDualError(HERE(IDS_GENERATION_VARIABLE_ZERO_LENGTH), iLineNumber, pCodeObject->szSource);
      else
      {
         // [SUCCESS] Resolve VariableID into a 'DT_VARIABLE' PARAMETER
         pOutput = createParameterFromInteger(calculateGeneratorVariableIDFromName(pScriptFile->pGenerator, szParameterText), DT_VARIABLE, eSyntax, iLineNumber);
         // Also store variable name, used for XML output
         pOutput->szValue = utilDuplicateString(pCodeObject->szText, pCodeObject->iLength);
      }
      break;

   /// [COMMENT] Extract the comment following the * commenting operator
   case CO_COMMENT:
      // Extract the comment text
      szParameterText = utilDuplicateSimpleString(pCodeObject->szText[1] == ' ' ? &pCodeObject->szText[2] : &pCodeObject->szText[1]);   //   SubString(pCodeObject->szText, pCodeObject->iLength, 2, pCodeObject->iLength - 2);
      // Create PARAMETER
      pOutput = createParameterFromString(szParameterText, eSyntax, iLineNumber);
      break;

   /// [NUMBER] Convert string to a number
   case CO_NUMBER:
      // [CHECK] Ensure user hasn't supplied a number for the label name
      if (eSyntax != PS_LABEL_NUMBER)
      {
         pOutput = createParameterFromInteger(utilConvertStringToInteger(pCodeObject->szText), DT_INTEGER, eSyntax, iLineNumber);
         break;
      }
      // Fall through...

   /// [LABEL] Remove the labelling operator
   case CO_LABEL:
      // Create PARAMETER with syntax of Label NAME instead of NUMBER.  (External vs. Internal)
      pOutput = createParameterFromString(pCodeObject->szText, PS_LABEL_NAME, iLineNumber);
      break;

   /// [CONSTANT] Reverse lookup as a GAME STRING   (Race / Object Class / Command / FlightReturn / Relation / DataType / TransportClass / Sector / StationSerial)
   case CO_CONSTANT:
      // Remove brackets
      szParameterText = utilDuplicateSubString(pCodeObject->szText, pCodeObject->iLength, 1, pCodeObject->iLength - 2);

      // Reverse lookup constant in the game data
      if (!findScriptObjectByText(szParameterText, pGameConstant) OR pGameConstant->eGroup == ONG_OPERATOR)
         // [ERROR] "Unknown script object '%s' in the script command on line %u : '%s'"
         pError = generateDualError(HERE(IDS_GENERATION_CONSTANT_NOT_FOUND), pCodeObject->szText, iLineNumber, pCodeObject->szSource);
      
      // [FOUND] Create a PARAMETER of the appropriate type from the ScriptObject
      else 
         pOutput = createParameterFromInteger((INT)pGameConstant->iID, identifyDataTypeFromScriptObject(pGameConstant), eSyntax, iLineNumber);
      break;

   /// [ENUMERATION] Reverse lookup the text as a GAME OBJECT NAME, then determine the data type from the page where it was found
   case CO_ENUMERATION:
      // Extract enumeration text from the display brackets
      szParameterText = utilDuplicateSubString(pCodeObject->szText, pCodeObject->iLength, 1, pCodeObject->iLength - 2);

      // Reverse lookup enumeration in the game data
      if (!findGameObjectByText(szParameterText, pGameEnumeration))
      {
         // [CHECK] Is this an encoded missing ware?
         if (!utilFindCharacter(szParameterText, '@') OR !generateParameterFromMissingWare(szParameterText, eSyntax, iLineNumber, pOutput))
            // [ERROR] "Unknown game object '%s' in the script command on line %u : '%s'"
            pError = generateDualError(HERE(IDS_GENERATION_ENUMERATION_NOT_FOUND), pCodeObject->szText, iLineNumber, pCodeObject->szSource);
      }
      // [FOUND] Create a DT_WARE PARAMETER and encode the MainType and SubType as required by the MSCI script format
      else
         pOutput = createParameterFromInteger(calculateWareEncoding(pGameEnumeration), DT_WARE, eSyntax, iLineNumber);
      break;

   /// [OPERATOR] Reverse lookup as a GAME STRING
   case CO_OPERATOR:
      // Reverse lookup operator in the game data
      if (!findOperatorTypeByText(pCodeObject->szText, eOperator))
         // [ERROR] "Unknown operator '%s' in the script comamnd on line %u : '%s'"
         pError = generateDualError(HERE(IDS_GENERATION_OPERATOR_NOT_FOUND), pCodeObject->szText, iLineNumber, pCodeObject->szSource);
      
      // [FOUND] Create a DT_OPERATOR PARAMETER
      else 
         pOutput = createParameterFromInteger(eOperator, DT_OPERATOR, eSyntax, iLineNumber);
      break;
   
   /// [INVALID] Error - cannot create a PARAMETER from these
   default:
      // [ERROR] "Unable to generate a parameter from the CodeObject '%s' on line %u"
      pError = generateDualError(HERE(IDS_GENERATION_CODEOBJECT_ILLEGAL), pCodeObject->szText, iLineNumber);
      break;
   }

   // [SPECIAL CASE]

   /// [VERIFICATION] Attempt to verify the parameter using the syntax
   if (pOutput AND !verifyParameterDataType(pOutput))
   { 
      // [ERROR] "'%s' is not a valid value for the %s parameter in the script command on line %u : '%s'"
      pError = generateDualError(HERE(IDS_GENERATION_PARAMETER_INVALID), pCodeObject->szText, identifyParameterSyntaxString(eSyntax), iLineNumber, pCodeObject->szSource);
      // Cleanup
      deleteParameter(pOutput);
   }
   else if (pOutput)
      // [SUCCESS] Copy text to parameter buffer for macro generation
      pOutput->szBuffer = utilDuplicateSimpleString(pCodeObject->szText);

   /// [EXTERNALISE VALUES] Convert values here...

   // Cleanup and return TRUE if the PARAMETER was created successfully
   utilSafeDeleteString(szParameterText);
   return (pOutput != NULL);
}



/// Function name  : generateParameters
// Description     : Parses the parameters in a script command into a PARAMETER list
// 
// CONST SCRIPT_FILE*  pScriptFile : [in]     ScriptFile containing the COMMAND, used for converting variable names to IDs
// COMMAND*            pCommand    : [in/out] COMMAND containing the script command to generate PARAMETERs for
// ERROR_STACK*       &pError      : [out]    New error, if any. You are responsible for destroying it
// 
// Return Value   : TRUE if succesful, FALSE otherwise
// 
BOOL  generateParameters(CONST SCRIPT_FILE*  pScriptFile, COMMAND*  pCommand, ERROR_STACK*  &pError)
{
   CODEOBJECT*       pCodeObject;         // CodeObject for parsing script command parameters
   RETURN_OBJECT*    pReturnObject;       // ReturnObject being created
   PARAMETER_INDEX*  pParameterIndex;     // Handles parameter index conversion
   PARAMETER_SYNTAX  eSyntax;             // Syntax of the parameter currently being processed
   PARAMETER*        pParameter;          // Parameter currently being processed
   
   TCHAR             chPreviousChar;      // Character preceeding a minus operator in an expression

   // Prepare
   pCodeObject     = createCodeObject(pCommand->szBuffer);
   pParameterIndex = createParameterIndex(NULL, 0);
   pReturnObject   = NULL;

   /// Iterate through the expected parameters defined by the command syntax
   while (!pError AND findParameterSyntaxByLogicalIndex(pCommand->pSyntax, pParameterIndex, eSyntax))
   {
      // Prepare
      pParameter = NULL;

      // Examine the syntax of the current parameter
      switch (eSyntax)
      {
      /// [RETURN OBJECT] Encode the conditional, return variable or lack of return variable into a PARAMETER
      case PS_RETURN_OBJECT:
      case PS_RETURN_OBJECT_IF:
      case PS_RETURN_OBJECT_IF_START:
      case PS_INTERRUPT_RETURN_OBJECT_IF:
         /// [CONDITIONAL] Create a 'CONDITIONAL' ReturnObject
         if (pCommand->eConditional != CI_NONE)
            pReturnObject = createReturnObjectConditional(pCommand->eConditional);  
            
         // Retrieve the next 'parameter' code object
         else if (!findNextParameterCodeObject(pCodeObject, FALSE))
            // [ERROR] "Cannot find the return object in the script command on line %u : '%s'"
            pError = generateDualError(HERE(IDS_GENERATION_RETURN_OBJECT_MISSING), pCommand->iLineNumber, pCommand->szBuffer);
         
         /// [DISCARD OPERATOR] Create a 'DISCARD' ReturnObject
         else if (utilCompareString(pCodeObject->szText, "="))
            // [DEFAULT] Create 'discard' return object
            pReturnObject = createReturnObjectConditional(CI_DISCARD);

         /// [RETURN VARIABLE] Create a 'VARIABLE' ReturnObject 
         else switch (pCodeObject->eClass)
         {
         // [ARGUMENT/VARIABLE] Resolve variable ID and generate ReturnObject
         case CO_ARGUMENT:
         case CO_VARIABLE:
            pReturnObject = createReturnObjectVariable(calculateGeneratorVariableIDFromName(pScriptFile->pGenerator, identifyRawVariableName(pCodeObject->szText)));
            break;

         // [OTHER] Can't be used as a return variable
         default:
            // [ERROR] "Cannot use '%s' as the return object for the script command on line %u : '%s'"
            pError = generateDualError(HERE(IDS_GENERATION_RETURN_OBJECT_INVALID), pCodeObject->szText, pCommand->iLineNumber, pCommand->szBuffer);
            break;
         }
         
         /// [SUCCESS] Generate PARAMETER from the ReturnObject and save it
         if (!pError AND pReturnObject)
         {
            // Encode ReturnObject data into a PARAMETER then add it to the COMMAND
            pParameter = createParameterFromInteger(calculateReturnObjectEncoding(pReturnObject), DT_INTEGER, eSyntax, pCommand->iLineNumber);
            insertParameterIntoCommand(pCommand, pParameterIndex, pParameter);

            // [RETURN VARIABLES] Save the variable name in the PARAMETER and consume the assignment operator.
            //                    This is used for command comment variables, CodeEdit variable tree building and general debugging
            if (pReturnObject->eType == ROT_VARIABLE)
            {
               // Store variable name (without the '$')
               utilSafeDeleteString(pParameter->szValue);
               pParameter->szValue = utilDuplicateSimpleString(identifyRawVariableName(pCodeObject->szText));

               // [CHECK] Ensure the assignment operator is present, and then consume it
               if (!findNextParameterCodeObject(pCodeObject, FALSE) OR !utilCompareString(pCodeObject->szText, "="))
                  // [ERROR] "Cannot find the assignment operator in the script command on line %u : '%s'"
                  pError = generateDualError(HERE(IDS_GENERATION_ASSIGNMENT_OPERATOR_MISSING), pCommand->iLineNumber, pCommand->szBuffer);
            }

            // Cleanup
            deleteReturnObject(pReturnObject);
         }
         break;

      /// [REFERENCE OBJECT] These must be a constant or variable and be followed by the -> operator
      case PS_REFERENCE_OBJECT:
         if (!findNextParameterCodeObject(pCodeObject, FALSE))
            // [ERROR] "Cannot find the reference object in the script command on line %u : '%s'"
            pError = generateDualError(HERE(IDS_GENERATION_REFERENCE_OBJECT_MISSING), pCommand->iLineNumber, pCommand->szBuffer);
         
         else switch (pCodeObject->eClass)
         {
         /// [NULL, VARIABLE, CONSTANT] Attempt to generate a PARAMETER
         case CO_NULL:
         case CO_VARIABLE:
         case CO_CONSTANT:
            if (generateParameterFromCodeObject(pScriptFile, pCodeObject, eSyntax, pCommand->iLineNumber, pParameter, pError))
            {
               // Save in COMMAND
               insertParameterIntoCommand(pCommand, pParameterIndex, pParameter);

               // [CHECK] Ensure the reference operator is present
               if (!findNextParameterCodeObject(pCodeObject, FALSE) OR !utilCompareString(pCodeObject->szText, "->"))
                  // [ERROR] "Cannot find the reference operator in the script command on line %u : '%s'"
                  pError = generateDualError(HERE(IDS_GENERATION_REFERENCE_OPERATOR_MISSING), pCommand->iLineNumber, pCommand->szBuffer);
            }
            //else
               // [ERROR] No enhancement needed
            break;

         /// [EVERYTHING ELSE] Invalid
         default:
            // [ERROR] "Cannot use '%s' as a reference object for the script command on line %u : '%s'"
            pError = generateDualError(HERE(IDS_GENERATION_REFERENCE_OBJECT_INVALID), pCodeObject->szText, pCommand->iLineNumber, pCommand->szBuffer);
            break;
         }
         break;

      /// [EXPRESSION] Generate PARAMETERS from the remaining operators and parameters
      case PS_EXPRESSION:
         // Iterate through remaining operators and parameters
         while (!pError AND findNextParameterCodeObject(pCodeObject, FALSE))
         {
            // Attempt to generate PARAMETER from CodeObject
            if (generateParameterFromCodeObject(pScriptFile, pCodeObject, eSyntax, pCommand->iLineNumber, pParameter, pError))
            {
               // [SPECIAL CASE] Manually convert the subtraction operator into the minus operator if it directly preceeds a variable or brackets.  eg. '-$iVariable', '-(5 + 2)'
               if (pParameter->eType == DT_OPERATOR AND pParameter->iValue == OT_SUBTRACT)
               {
                  // Prepare
                  chPreviousChar = pCodeObject->szSource[pCodeObject->iEndCharIndex];

                  // [CHECK] Does operator preceed a variation or brackets?
                  if (chPreviousChar == '$' OR chPreviousChar == '(')
                     pParameter->iValue = OT_MINUS;
               }

               // Save in COMMAND
               insertParameterIntoCommand(pCommand, pParameterIndex, pParameter);

               // Manually increment the PHYSICAL (output) INDEX and reset PARAMETER
               pParameterIndex->iPhysicalIndex++;
               pParameter = NULL;
            }
            //else
               // [ERROR] No enhancement needed
         }
         break;

      /// [STANDARD PARAMETER] Generate a PARAMETER from the next non-operator CodeObject
      default:
         if (!findNextParameterCodeObject(pCodeObject, TRUE))
            // [ERROR] "Cannot find parameter %u of %u in the script command on line %u : '%s'"
            pError = generateDualError(HERE(IDS_GENERATION_PARAMETER_MISSING), pParameterIndex->iLogicalIndex + 1, pCommand->pSyntax->iParameterCount, pCommand->iLineNumber, pCommand->szBuffer);
         
         // Attempt to generate PARAMETER from CodeObject
         else if (generateParameterFromCodeObject(pScriptFile, pCodeObject, eSyntax, pCommand->iLineNumber, pParameter, pError)) 
            // Save in COMMAND
            insertParameterIntoCommand(pCommand, pParameterIndex, pParameter);     
         //else
            // [ERROR] No enhancement needed
         break;

      } // END: switch (current syntax)
      
      // Move to the next parameter
      pParameterIndex->iLogicalIndex++;
   }

   // [CHECK] Ensure generation was successful
   if (!pError)
   {
      // [VARIABLE ARGUMENTS] Parse commands with parameters in a non-uniform quantity and format
      switch (pCommand->iID)
      {
      /// [SCRIPT CALL] Parse any remaining non-operator and non-word CodeObjects
      case CMD_CALL_SCRIPT_VAR_ARGS:
         generateParametersForScriptCall(pScriptFile, pCommand, pCodeObject, pParameterIndex, pError);
         break;

      /// [DEFINE ARRAY] Parse any remaining non-operator CodeObjects
      case CMD_DEFINE_ARRAY:
         generateParametersForDefineArray(pScriptFile, pCommand, pCodeObject, pParameterIndex, pError);
         break;
         
      /// [EXPRESSION] Generate a PostFix list of PARAMETERS
      case CMD_EXPRESSION:
         if (!validateExpressionParameters(pCommand, pError) OR !generateParametersForExpression(pCommand, pError))
            generateOutputTextFromError(pError);
         break;

      // [DEFAULT] Ensure no variable arguments exist
      default:
         if (findNextParameterCodeObject(pCodeObject, TRUE)) 
            // [ERROR] "Unexpected '%s' found while processing script command on line %u : '%s'"
            pError = generateDualError(HERE(IDS_GENERATION_COMMAND_TEXT_UNEXPECTED), pCodeObject->szText, pCommand->iLineNumber, pCommand->szBuffer);
         break;
      }
   }
   
   // Cleanup and return TRUE if there was no error
   deleteCodeObject(pCodeObject);
   deleteParameterIndex(pParameterIndex);
   return (pError == NULL);
}


/// Function name  : generateParametersForDefineArray
// Description     : Generates the PARAMETERs for the virtual 'define array' command
// 
// CONST SCRIPT_FILE*  pScriptFile     : [in]     ScriptFile
// COMMAND*            pCommand        : [in/out] Command to store new parameters in
// CODEOBJECT*         pCodeObject     : [in/out] CodeObject initialised to the last syntax parameter
// PARAMETER_INDEX*    pParameterIndex : [in/out] ParameterIndex initialised to the last syntax parameter
// ERROR_STACK*       &pError          : [out]    New error, if any, otherwise NULL
// 
// Return Value   : TRUE if successful, FALSE if there was an error
// 
BOOL  generateParametersForDefineArray(CONST SCRIPT_FILE*  pScriptFile, COMMAND*  pCommand, CODEOBJECT*  pCodeObject, PARAMETER_INDEX*  pParameterIndex, ERROR_STACK*  &pError)
{
   PARAMETER*  pParameter;          // Parameter currently being processed
   BOOL        bOperatorState;      // Whether searching for an operator or parameter

   // [CHECK] Ensure command is 'define array'
   ASSERT(isCommandID(pCommand, CMD_DEFINE_ARRAY));

   // Prepare
   bOperatorState = TRUE;
   pError         = NULL;

   // Set the initial PHYSICAL index manually (in case there's an error with the first argument)
   pParameterIndex->iPhysicalIndex = 2;

   // Iterate through remaining operators and parameters
   while (!pError AND findNextParameterCodeObject(pCodeObject, FALSE))
   {
      // Prepare
      pParameter = NULL;

      // Examine CodeObject
      switch (pCodeObject->eClass)
      {
      /// [OPERATOR]
      case CO_OPERATOR:
         // [CHECK] Ensure this is the array element operator
         if (utilCompareString(pCodeObject->szText, ","))
         {
            // [CHECK] Are we expecting the operator?
            if (bOperatorState)
               // [SUCCESS] Move to parameter state
               bOperatorState = FALSE;
            else
               // [ERROR] "Unexpected array element operator ',' detected in array definition on line %d : '%s'"
               pError = generateDualError(HERE(IDS_GENERATION_ARRAY_UNEXPECTED_COMMA), pCommand->iLineNumber, pCommand->szBuffer);
         }
         else
            // [ERROR] "Unexpected operator '%s' detected in array definition on line %d : '%s'"
            pError = generateDualError(HERE(IDS_GENERATION_ARRAY_UNEXPECTED_OPERATOR), pCodeObject->szText, pCommand->iLineNumber, pCommand->szBuffer);
         break;

      /// [PARAMETER] Generate PARAMETER
      default:
         // [CHECK] Are we expecting a parameter?
         if (bOperatorState)
            // [ERROR] "Unexpected array element '%s' detected in array definition on line %d : '%s'"
            pError = generateDualError(HERE(IDS_GENERATION_ARRAY_UNEXPECTED_PARAMETER), pCodeObject->szText, pCommand->iLineNumber, pCommand->szBuffer);
         
         // Attempt to generate PARAMETER from CodeObject
         else if (generateParameterFromCodeObject(pScriptFile, pCodeObject, PS_VALUE, pCommand->iLineNumber, pParameter, pError))
         {
            // Set the PHYSICAL INDEX manually using the logical index as a reference
            pParameterIndex->iPhysicalIndex = pParameterIndex->iLogicalIndex++;
            // Save in COMMAND and revert to the operator state
            insertParameterIntoCommand(pCommand, pParameterIndex, pParameter);
            bOperatorState = TRUE;
         }
         //else
            // [ERROR] No enhancement needed
         break;

      case CO_LABEL:
      case CO_COMMENT:
         // [ERROR] "Unexpected %s '%s' detected in array definition on line %d : '%s'"
         pError = generateDualError(HERE(IDS_GENERATION_ARRAY_UNEXPECTED_CODEOBJECT), pCodeObject->eClass == CO_LABEL ? TEXT("label") : TEXT("comment"), pCodeObject->szText, pCommand->iLineNumber, pCommand->szBuffer);
         break;
      }
   }

   // Return TRUE if successful
   return (pError == NULL);
}


/// Function name  : generateParametersForExpression
// Description     : Builds a COMMAND's postfix expression PARAMETER list using the SHUNTING YARD algorithm 
//
// COMMAND*       pCommand  : [in/out] COMMAND of type CT_EXPRESSION containing a default PARMAETER list
// ERROR_STACK*  &pError    : [out]    Error message, if any
// 
// Return type : TRUE if successul, FALSE if there's an error
//
BOOL   generateParametersForExpression(COMMAND*  pCommand, ERROR_STACK*  &pError)
{
   OPERATOR_STACK*       pOperatorStack;        // [ShuntingYard] Operator stack
   OPERATOR_TYPE         eCurrentOperator;      // [ShuntingYard] Current operator
   PARAMETER*            pParameter;            // Parameter currently being processed
   UINT                  iCurrentParameter;     // Zero-based index of the PARAMETER (from the default list) currently being processed
   BOOL                  bMatchingBracket;

   TCHAR*                szOutput;              /// DEBUG: Output buffer
   //GAME_STRING*          pLookupString;         /// DEBUG: Lookup buffer

   // [CHECK] Ensure COMMAND is an EXPRESSION
   ASSERT(isCommandType(pCommand, CT_EXPRESSION) AND isCommandID(pCommand, CMD_EXPRESSION));

   // Prepare
   pOperatorStack    = createOperatorStack();
   szOutput          = utilCreateEmptyString(LINE_LENGTH);
   iCurrentParameter = 1;     // Start with the first non-ReturnObject parameter
   pError            = NULL;

   /// Iterate through the existing parameter list
   while (findParameterInCommandByIndex(pCommand, PT_DEFAULT, iCurrentParameter++, pParameter))
   {
      // Set Parameter ID
      pParameter->iID = iCurrentParameter;

      // Examine Parameter syntax
      switch (pParameter->eType)
      {
      /// [OPERAND] Add to POSTFIX list
      default:
         appendParameterToCommand(pCommand, duplicateParameter(pParameter), PT_POSTFIX);

         // [DEBUG] Print to output
         //utilStringCchCatf(szOutput, LINE_LENGTH, TEXT("%s "), pParameter->szValue);
         break;

      /// [OPERATOR] Add/Remove from the stack depending on whether unary/binary
      case DT_OPERATOR:
         // Extract operator from parameter and set as the 'current'
         switch (eCurrentOperator = (OPERATOR_TYPE)pParameter->iValue)
         {
         /// [OPEN BRACKET / UNARY OPERATOR] Add to the stack
         case OT_BRACKET_OPEN:
         case OT_BITWISE_NOT:
         case OT_LOGICAL_NOT:
         case OT_MINUS:
            pushOperatorStack(pOperatorStack, eCurrentOperator);
            break;

         /// [CLOSING BRACKET / BINARY OPERATORS] Unwind stack until an OPENING BRACKET or OPERATOR OF LOWER PRECEDENCE is found
         default:
            // Prepare
            bMatchingBracket = FALSE;

            // Examine the stack
            while (hasItems(pOperatorStack))
            {
               // [MATCHING OPENING BRACKET] Pop the opening bracket and stop
               if (topOperatorStack(pOperatorStack) == OT_BRACKET_OPEN AND eCurrentOperator == OT_BRACKET_CLOSE)
               {
                  popOperatorStack(pOperatorStack);   // Discard brackets for postfix
                  bMatchingBracket = TRUE;
                  break;
               }
               // [OPERATOR OF LOWER PRECEDENCE] Just stop  
               else if (identifyOperatorAssociativity(eCurrentOperator) == OA_LEFT  AND compareOperatorPrecedence(topOperatorStack(pOperatorStack), eCurrentOperator) < 0     // LEFT ASSOCIATIVE: Lower precedence
                     OR identifyOperatorAssociativity(eCurrentOperator) == OA_RIGHT AND compareOperatorPrecedence(topOperatorStack(pOperatorStack), eCurrentOperator) <= 0)   // RIGHT ASSOCIATIVE: Lower or equal precedence
                     break;

               // [DEBUG] Add operator to the output buffer
               /*findGameStringByID(topOperatorStack(pOperatorStack), GPI_OPERATORS, pLookupString);
               utilStringCchCatf(szOutput, LINE_LENGTH, TEXT("%s "), pLookupString->szText);*/

               /// Remove top operator and convert to a PARAMETER
               pParameter = createParameterFromInteger(popOperatorStack(pOperatorStack), DT_OPERATOR, PS_EXPRESSION, pCommand->iLineNumber);
               // Copy to POSTFIX list
               appendParameterToCommand(pCommand, pParameter, PT_POSTFIX);
            }

            // [OPERATOR] Add current operator to the stack
            if (eCurrentOperator != OT_BRACKET_CLOSE)
               pushOperatorStack(pOperatorStack, eCurrentOperator);

            // [BRACKET] Ensure matching bracket was found
            else if (!bMatchingBracket)
               // [ERROR] "There are brackets missing from the expression on line %u : '%s'"
               pError = generateDualError(HERE(IDS_GENERATION_EXPRESSION_BRACKET_MISSING), pCommand->szBuffer);
            
         }  // END -- switch 'current operator'

      } // END -- switch 'parameter datatype'

   } // END -- for each 'parameter'

   /// [SPECIAL CASE] 
   // [APPEND OPERATOR] If the first non-return object parameter is the addition operator, and this is the only operator in the stack - convert to the 'Append String' operator
   if (pOperatorStack->iCount == 1 AND getCommandParameterCount(pCommand, PT_DEFAULT) == 3 AND isOperatorParameterInCommandAtIndex(pCommand, 1, OT_ADD))
   {   
      // Do not add anything to the PostFix parameter list
      // TODO("Produce a warning here that the addition operator has been interpreted as the append operator");
   }
   /// [NON-EMPTY OPERATOR STACK] Pop remaining items off in PostFix order
   else while (!pError AND hasItems(pOperatorStack))
   {
      // Pop next operator
      switch (eCurrentOperator = popOperatorStack(pOperatorStack))
      {
      /// [OPERATOR] Generate a new PARAMETER and add it to the PostFix list
      default:
         // Generate new PARAMTER and add to the POSTFIX list
         pParameter = createParameterFromInteger(eCurrentOperator, DT_OPERATOR, PS_EXPRESSION, pCommand->iLineNumber);
         appendParameterToCommand(pCommand, pParameter, PT_POSTFIX);
         
         // [DEBUG] Append operator to the output buffer
         /*findGameStringByID(eCurrentOperator ,GPI_OPERATORS, pLookupString);
         utilStringCchCatf(szOutput, LINE_LENGTH, TEXT("%s "), pLookupString->szText);*/
         break;

      /// [BRACKETS] Indicates a missing brackets
      case OT_BRACKET_OPEN:
      case OT_BRACKET_CLOSE:
         // [ERROR] "There are brackets missing from the expression on line %u : '%s'"
         pError = generateDualError(HERE(IDS_GENERATION_EXPRESSION_BRACKET_MISSING), pCommand->szBuffer);
         break;
      }
   }

   // [DEBUG] Print output
   //CONSOLE("Input: %s    Output: %s", pCommand->szBuffer, szOutput);

   // Cleanup and return TRUE if there were no errors
   utilDeleteString(szOutput);
   deleteOperatorStack(pOperatorStack);
   return (pError == NULL);
}


/// Function name  : generateParametersForScriptCall
// Description     : Generates the argument PARAMETERs for the 'call script' command
// 
// CONST SCRIPT_FILE*  pScriptFile     : [in]     ScriptFile
// COMMAND*            pCommand        : [in/out] Command to store new parameters in
// CODEOBJECT*         pCodeObject     : [in/out] CodeObject initialised to the last syntax parameter
// PARAMETER_INDEX*    pParameterIndex : [in/out] ParameterIndex initialised to the last syntax parameter
// ERROR_STACK*       &pError          : [out]    New error, if any, otherwise NULL
// 
// Return Value   : TRUE if successful, FALSE if there was an error
// 
BOOL  generateParametersForScriptCall(CONST SCRIPT_FILE*  pScriptFile, COMMAND*  pCommand, CODEOBJECT*  pCodeObject, PARAMETER_INDEX*  pParameterIndex, ERROR_STACK*  &pError)
{
   PARAMETER*  pParameter;          // Parameter currently being processed
   TCHAR*      szArgumentName;      // Name of the ScriptCall argument being processed

   // [CHECK] Ensure command is 'call script'
   ASSERT(isCommandID(pCommand, CMD_CALL_SCRIPT_VAR_ARGS));

   // Prepare
   pError = NULL;

   // Set the initial PHYSICAL index manually (in case there's an error with the first argument)
   pParameterIndex->iPhysicalIndex = 3;

   // Iterate through remaining operators and parameters
   while (!pError AND findNextScriptCallArgumentCodeObject(pCodeObject, pCommand, szArgumentName, pError))
   {
      // Prepare
      pParameter = NULL;

      // Attempt to generate PARAMETER from CodeObject
      if (!generateParameterFromCodeObject(pScriptFile, pCodeObject, PS_PARAMETER, pCommand->iLineNumber, pParameter, pError))
         // [ERROR] "Syntax error in argument '%s' of the script call command on line %u : '%s'"
         enhanceError(pError, ERROR_ID(IDS_GENERATION_SCRIPT_CALL_INVALID), szArgumentName, pCommand->iLineNumber, pCommand->szBuffer);
      else
      {
         // Set the PHYSICAL INDEX manually using the logical index as a reference
         pParameterIndex->iPhysicalIndex = pParameterIndex->iLogicalIndex++;
         // Save in COMMAND
         insertParameterIntoCommand(pCommand, pParameterIndex, pParameter);
      }

      // Cleanup
      utilDeleteString(szArgumentName);
   }

   // Return TRUE if successful
   return (pError == NULL);
}



#define    COMMAND_DESTROY_THIS    34

/// Function name  : generateParametersSpecialCases
// Description     : Fudge certain COMMANDs in various ways
///                    Adjusts the parameters in commands using '± 1' syntax
// 
// CONST SCRIPT_FILE*  pScriptFile : [in]     ScriptFile containing the COMMAND
// COMMAND*            pCommand    : [in/out] COMMAND object containing a generated COMMAND
// ERROR_STACK*       &pError      : [out]    Error message if any, otherwise NULL
// 
// Return Value : TRUE if there were no errors, otherwise FALSE
//
BOOL  generateParametersSpecialCases(CONST SCRIPT_FILE*  pScriptFile, COMMAND*  pCommand, ERROR_STACK*  &pError)
{
   PARAMETER*  pParameter;    // Parameter being altered

   // Prepare
   pError = NULL;

   // Examine command
   switch (pCommand->iID)
   {
   /// [INTERNAL] "$0 random value from zero to $1"
   /// [EXTERNAL] "$0 random value from zero to $1 - 1"
   //  [ACTION] Add 1 to parameter $1
   case CMD_RANDOM_VALUE_ZERO:
      // Lookup target parameter
      if (!findParameterInCommandByIndex(pCommand, PT_DEFAULT, 1, pParameter))
         // [WARNING] "Could not perform special case processing on parameter %u of the command on line %u"
         pError = generateDualWarning(HERE(IDS_GENERATION_SPECIAL_CASE_PARAMETER_NOT_FOUND), 2, pCommand->iLineNumber);

      // [CHECK] Don't attempt to increment constants or variables
      else if (pParameter->eType == DT_INTEGER)
         pParameter->iValue++;
      break;

   /// [INTERNAL] "$0 random value between $1 and $2"
   /// [EXTERNAL] "$0 random value between $1 and $2 - 1"
   //  [ACTION] Add 1 to parameter $2
   case CMD_RANDOM_VALUE:
      // Lookup target parameter
      if (!findParameterInCommandByIndex(pCommand, PT_DEFAULT, 2, pParameter))
         // [WARNING] "Could not perform special case processing on parameter %u of the command on line %u"
         pError = generateDualWarning(HERE(IDS_GENERATION_SPECIAL_CASE_PARAMETER_NOT_FOUND), 3, pCommand->iLineNumber);

      // [CHECK] Don't attempt to increment constants or variables
      else if (pParameter->eType == DT_INTEGER)
         pParameter->iValue++;
      break;

   /// [INTERNAL] "$0 get index of $2 in array $1 offset=$3"
   /// [EXTERNAL] "$0 get index of $2 in array $1 offset=$3 + 1"
   //  [ACTION] Sbutract 1 from parameter $3
   case CMD_GET_INDEX_OF_ARRAY:
      // Lookup target parameter
      if (!findParameterInCommandByIndex(pCommand, PT_DEFAULT, 3, pParameter))
         // [WARNING] "Could not perform special case processing on parameter %u of the command on line %u"
         pError = generateDualWarning(HERE(IDS_GENERATION_SPECIAL_CASE_PARAMETER_NOT_FOUND), 4, pCommand->iLineNumber);

      // [CHECK] Don't attempt to subtract from constants or variables
      else if (pParameter->eType == DT_INTEGER)
         pParameter->iValue--;
      break;

   /// [DELAYED COMMAND] Add a hidden NULL parameter
   case CMD_DELAYED_COMMAND:
      // Lookup command parameter
      if (!findParameterInCommandByIndex(pCommand, PT_DEFAULT, 1, pParameter))
         // [WARNING] "Could not perform special case processing on parameter %u of the command on line %u"
         pError = generateDualWarning(HERE(IDS_GENERATION_SPECIAL_CASE_PARAMETER_NOT_FOUND), 1, pCommand->iLineNumber);
      
      else if (pParameter->iValue == COMMAND_DESTROY_THIS)
         appendParameterToCommand(pCommand, createParameterFromInteger(NULL, DT_NULL, PS_VALUE, pCommand->iLineNumber), PT_DEFAULT);
      break;

   /// [SET WING COMMAND] Add two hidden NULL parameters
   case CMD_SET_WING_COMMAND:
      appendParameterToCommand(pCommand, createParameterFromInteger(NULL, DT_NULL, PS_VALUE, pCommand->iLineNumber), PT_DEFAULT);
      appendParameterToCommand(pCommand, createParameterFromInteger(NULL, DT_NULL, PS_VALUE, pCommand->iLineNumber), PT_DEFAULT);
      break;
   }

   // Return TRUE if there were no errors
   return (pError == NULL);
}


/// Function name  : generateScript
// Description     : Performs the entire conversion from text file to MSCI script XML
// 
// HWND                 hCodeEdit    : [in]     Window handle of the CodeEdit window containing the script commands to parse
// SCRIPT_FILE*         pScriptFile  : [in/out] ScriptFile containing the script properties and the output buffer to store the XML in
// OPERATION_PROGRESS*  pProgress    : [in]     Progress tracking object
// ERROR_QUEUE*         pErrorQueue  : [in]     Output error queue - must be empty.
// 
// Return Value   : TRUE if there were no errors, otherwise FALSE
// 
BearScriptAPI
BOOL  generateScript(HWND  hCodeEdit, SCRIPT_FILE*  pScriptFile, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue)
{
   ARGUMENT*  pArgument;      // ScriptFile ARGUMENT list iterator

   __try
   {
      // [TRACKING]
      CONSOLE_STAGE();

      // [INFO/STAGE] "Checking syntax of commands in '%s' and their compatbility with %s"
      VERBOSE("Generating script file '%s'", pScriptFile->szFullPath);
      pushErrorQueue(pErrorQueue, generateDualInformation(HERE(IDS_OUTPUT_PARSING_SCRIPT_COMMANDS), identifyScriptName(pScriptFile), identifyGameVersionString(pScriptFile->eGameVersion)));
      updateOperationProgressMaximum(pProgress, Edit_GetLineCount(hCodeEdit));

      // Empty any previous generation data
      initScriptFileGenerator(pScriptFile);   

      /// [ENUMERATE ARGUMENTS] Add each of the ScriptFile ARGUMENTS to the VariableName list
      for (UINT iIndex = 0; findArgumentInScriptFileByIndex(pScriptFile, iIndex, pArgument); iIndex++)
         // Append to Generator's Arguments+Variables list
         appendArgumentNameToGenerator(pScriptFile->pGenerator, pArgument->szName);

      /// [VALIDATE PROPERTIES] Ensure necessary properties are present and valid
      if (!validateScriptProperties(pScriptFile, pErrorQueue))
         // [ERROR] "Compilation aborted due to missing or invalid script properties"
         generateQueuedWarning(pErrorQueue, HERE(IDS_GENERATION_ABORT_PROPERTIES));

      /// [PARSE/VALIDATE COMMANDS] Parse script commands into COMMAND objects and add to the SCRIPT_FILE list
      else if (!generateScriptCommands(hCodeEdit, pScriptFile, pProgress, pErrorQueue))
         // [ERROR] "Compilation aborted due to syntax errors in script commands"
         generateQueuedWarning(pErrorQueue, HERE(IDS_GENERATION_ABORT_SYNTAX));

      /// [BRANCHING LOGIC] Insert jump information into the branching commands.  (Also splits 'translated' command list into the standard and auxiliary lists)
      else if (!generateBranchingCommandLogic(hCodeEdit, pScriptFile, pErrorQueue))
         // [ERROR] "Compilation aborted due to logical errors in conditional script commands"
         generateQueuedWarning(pErrorQueue, HERE(IDS_GENERATION_ABORT_CONDITIONALS));

      /// [SUBROUTINE LOGIC] Insert jump information into label and subroutine commands
      else if (!generateSubroutineCommandLogic(hCodeEdit, pScriptFile, pErrorQueue))
         // [ERROR] "Compilation aborted due to logical errors in sub-routine script commands"
         generateQueuedWarning(pErrorQueue, HERE(IDS_GENERATION_ABORT_SUBROUTINES));

      /// [BUILD XML TREE] Build the COMMAND lists and the SCRIPT_FILE properties into an XML tree
      else if (!generateOutputTree(hCodeEdit, pScriptFile, pProgress, pErrorQueue))
         // [ERROR] "Compilation aborted due to errors generating the script tree"
         generateQueuedWarning(pErrorQueue, HERE(IDS_GENERATION_ABORT_XML_TREE));

      /// [FLATTEN] Convert the XML tree to a string
      else if (!generateScriptFromOutputTree(pScriptFile))
         // [ERROR] "Compilation aborted due to errors generating XML code from the script tree"
         generateQueuedWarning(pErrorQueue, HERE(IDS_GENERATION_ABORT_FLATTEN_XML));
      
      // Return TRUE if there were no errors (warnings are acceptable)
      return (identifyErrorQueueType(pErrorQueue) != ET_ERROR);
   }
   PUSH_CATCH0(pErrorQueue, "");
   return FALSE;
}

/// Function name  : generateCustomMenuMacro
// Description     : Generates the physical commands necessary to perform the 'add custom menu' macro
// 
// SCRIPT_FILE*    pScriptFile     : [in/out] ScriptFile
// CONST COMMAND*  pVirtualCommand : [in]     Macro command to expand
// ERROR_QUEUE*    pErrorQueue     : [in/out] Error queue
// 
// Return Value   : TRUE/FALSE
// 
BOOL  generateCustomMenuMacro(SCRIPT_FILE*  pScriptFile, CONST COMMAND*  pVirtualCommand, ERROR_QUEUE*  pErrorQueue)
{
   CONST TCHAR  *szReturnVariable,     // Return variable
                *szArrayVariable;      // Array variable
   ERROR_STACK  *pError;
   PARAMETER    *pMenuPageID,           // Parameter, re-used
                *pMenuStringID,           // For loop start range
                *pMenuReturnID;          // For loop finish range
   COMMAND      *pCommand;             // Command being processed
   TCHAR        *szCommandText;        // Text of new command being generated                
   BOOL          bResult;

   // Prepare
   bResult = TRUE;
   pError  = NULL;

   // add custom menu item to array $0: page=$1x id=$2y returnvalue=$3

   // [CHECK] Lookup array variable
   if (!findVariableParameterInCommandByIndex(pVirtualCommand, 0, szArrayVariable)) 
      // [ERROR] "Missing custom menu array variable in the 'add custom menu ...' macro on line %u : '%s'"
      pError = generateDualError(HERE(IDS_GENERATION_FOREACH_ARRAY_MISSING), pVirtualCommand->iLineNumber + 1, pVirtualCommand->szBuffer);

   // [CHECK] Lookup string/page ID parameters
   else if (!findParameterInCommandByIndex(pVirtualCommand, PT_DEFAULT, 1, pMenuPageID) OR !findParameterInCommandByIndex(pVirtualCommand, PT_DEFAULT, 2, pMenuStringID))
      // [ERROR] "Missing page ID or string ID in the 'add custom menu ...' macro on line %u : '%s'"
      pError = generateDualError(HERE(IDS_GENERATION_FOREACH_ITERATOR_MISSING), pVirtualCommand->iLineNumber + 1, pVirtualCommand->szBuffer);

   // [CHECK] Lookup return variable 
   else if (isCommandID(pVirtualCommand, CMD_ADD_MENU_ITEM_BYREF) AND !findParameterInCommandByIndex(pVirtualCommand, PT_DEFAULT, 3, pMenuReturnID))
      // [ERROR] "Missing item ID in the 'add custom menu item' macro on line %u : '%s'"
      pError = generateDualError(HERE(IDS_GENERATION_FOREACH_COUNTER_MISSING), pVirtualCommand->iLineNumber + 1, pVirtualCommand->szBuffer);

   else
   {
      // Prepare
      szReturnVariable = TEXT("XStudio.Hidden");
      szCommandText    = utilCreateEmptyString(LINE_LENGTH);

      // Generate two commands
      for (UINT iSubCommand = 1; bResult AND iSubCommand <= 2; iSubCommand++)
      {
         /// [READ TEXT] Create '$0 read text: page=$1x id=$2y' command
         if (iSubCommand == 1)
            StringCchPrintf(szCommandText, LINE_LENGTH, TEXT("$%s = read text: page=%s id=%s"), szReturnVariable, pMenuPageID->szBuffer, pMenuStringID->szBuffer);

         /// [ITEM] Create 'add custom menu item' command
         else if (pVirtualCommand->iID == CMD_ADD_MENU_ITEM_BYREF)
            StringCchPrintf(szCommandText, LINE_LENGTH, TEXT("add custom menu item to array $%s: text=$%s returnvalue=%s"), szArrayVariable, szReturnVariable, pMenuReturnID->szBuffer);

         /// [HEADING/INFO-LINE] Create 'add custom menu heading/info-line' command
         else if (pVirtualCommand->iID == CMD_ADD_MENU_INFO_BYREF)
            StringCchPrintf(szCommandText, LINE_LENGTH, TEXT("add custom menu info line to array $%s: text=$%s"), szArrayVariable, szReturnVariable);
         else
            StringCchPrintf(szCommandText, LINE_LENGTH, TEXT("add custom menu heading to array $%s: title=$%s"), szArrayVariable, szReturnVariable);

         /// Attempt to generate COMMAND
         if (bResult = generateCommandFromString(pScriptFile, szCommandText, pVirtualCommand->iLineNumber, pCommand, pErrorQueue))
            // [SUCCESS] Add to Generator Input
            appendCommandToGenerator(pScriptFile->pGenerator, CL_INPUT, pCommand);
         else
            // [FAILURE] Cleanup and abort.. (No enhancement necessary)
            deleteCommand(pCommand);
      }

      // Cleanup
      utilDeleteString(szCommandText);
   }

   // [ERROR] Add to macro + output queue and return FALSE
   if (pError)
      pushCommandAndOutputQueues(pError, pErrorQueue, pVirtualCommand->pErrorQueue, ET_ERROR);
   
   // Return TRUE if successul
   return !pError AND bResult;
}

/// Function name  : generateDefineArrayMacro
// Description     : Generates the physical commands necessary to perform the 'dim' macro
// 
// SCRIPT_FILE*    pScriptFile     : [in/out] ScriptFile
// CONST COMMAND*  pVirtualCommand : [in]     Macro command to expand
// ERROR_QUEUE*    pErrorQueue     : [in/out] Error queue
// 
// Return Value   : TRUE/FALSE
// 
BOOL  generateDefineArrayMacro(SCRIPT_FILE*  pScriptFile, CONST COMMAND*  pVirtualCommand, ERROR_QUEUE*  pErrorQueue)
{
   CONST TCHAR  *szArrayVariable;      // Array variable
   ERROR_STACK  *pError;
   PARAMETER    *pElement;           // Parameter, re-used
   COMMAND      *pCommand;             // Command being processed
   TCHAR        *szCommandText;        // Text of new command being generated                
   UINT          iElementCount;        // DefineArray element count
   BOOL          bResult;

   // Prepare
   bResult = TRUE;
   pError  = NULL;

   // [CHECK] Lookup return variable
   if (!findReturnVariableParameterInCommand(pVirtualCommand, szArrayVariable))
      // [ERROR] "Missing return variable in the array definition macro on line %u : '%s'"
      pError = generateDualError(HERE(IDS_GENERATION_ARRAY_RETURN_VARIABLE_MISSING), pVirtualCommand->iLineNumber + 1, pVirtualCommand->szBuffer);

   else
   {
      // Prepare
      iElementCount = getCommandParameterCount(pVirtualCommand, PT_DEFAULT) - 1;
      szCommandText = utilCreateEmptyString(LINE_LENGTH);

      // Generate commands
      for (UINT iSubCommand = 0; bResult AND iSubCommand <= iElementCount; iSubCommand++)
      {
         switch (iSubCommand)
         {
         /// [ALLOCATION]
         case 0:  
            StringCchPrintf(szCommandText, LINE_LENGTH, TEXT("$%s = array alloc: size=%d"), szArrayVariable, iElementCount); 
            break;

         /// [ASSIGNMENT]
         default: 
            findParameterInCommandByIndex(pVirtualCommand, PT_DEFAULT, iSubCommand, pElement);
            StringCchPrintf(szCommandText, LINE_LENGTH, TEXT("$%s[%d] = %s"), szArrayVariable, iSubCommand - 1, pElement->szBuffer);
            break;
         }
      
         /// Attempt to generate command
         if (bResult = generateCommandFromString(pScriptFile, szCommandText, pVirtualCommand->iLineNumber, pCommand, pErrorQueue))
            // [SUCCESS] Add to Generator Input
            appendCommandToGenerator(pScriptFile->pGenerator, CL_INPUT, pCommand);
         else
            // [FAILURE] Cleanup and abort.. (No enhancement necessary)
            deleteCommand(pCommand);
      }

      // Cleanup
      utilDeleteString(szCommandText);
   }

   // [ERROR] Add to macro + output queue and return FALSE
   if (pError)
      pushCommandAndOutputQueues(pError, pErrorQueue, pVirtualCommand->pErrorQueue, ET_ERROR);
   
   // Return TRUE if successul
   return !pError AND bResult;
}

/// Function name  : generateForEachMacro
// Description     : Generates the physical commands necessary to perform the 'for each' macro
// 
// SCRIPT_FILE*    pScriptFile     : [in/out] ScriptFile
// CONST COMMAND*  pVirtualCommand : [in]     Macro command to expand
// ERROR_QUEUE*    pErrorQueue     : [in/out] Error queue
// 
// Return Value   : TRUE/FALSE
// 
BOOL  generateForEachMacro(SCRIPT_FILE*  pScriptFile, CONST COMMAND*  pVirtualCommand, ERROR_QUEUE*  pErrorQueue)
{
   CONST TCHAR  *szReturnVariable,     // Return variable
                *szArrayVariable,      // Array variable
                *szIteratorVariable;
   ERROR_STACK  *pError;
   COMMAND      *pCommand;             // Command being processed
   TCHAR        *szCommandText;        // Text of new command being generated                
   BOOL          bResult;

   // Prepare
   bResult = TRUE;
   pError  = NULL;

   /// [CHECK] Lookup RetVar
   if (!findVariableParameterInCommandByIndex(pVirtualCommand, 0, szReturnVariable))
      // [ERROR] "Missing iterator variable in the 'for each' macro on line %u : '%s'"
      pError = generateDualError(HERE(IDS_GENERATION_FOREACH_ITERATOR_MISSING), pVirtualCommand->iLineNumber + 1, pVirtualCommand->szBuffer);

   /// [CHECK] Lookup Array
   else if (!findVariableParameterInCommandByIndex(pVirtualCommand, 1, szArrayVariable)) 
      // [ERROR] "Missing array variable in the 'for each' macro on line %u : '%s'"
      pError = generateDualError(HERE(IDS_GENERATION_FOREACH_ARRAY_MISSING), pVirtualCommand->iLineNumber + 1, pVirtualCommand->szBuffer);

   /// [CHECK] Lookup Iterator
   else if (isCommandID(pVirtualCommand, CMD_FOR_EACH_COUNTER) AND !findVariableParameterInCommandByIndex(pVirtualCommand, 2, szIteratorVariable))
      // [ERROR] "Missing counter variable in the 'for each ... using counter' macro on line %u : '%s'"
      pError = generateDualError(HERE(IDS_GENERATION_FOREACH_COUNTER_MISSING), pVirtualCommand->iLineNumber + 1, pVirtualCommand->szBuffer);

   else
   {
      // Prepare
      szCommandText = utilCreateEmptyString(LINE_LENGTH);

      // [FOR-EACH] Generate hidden iterator name
      if (isCommandID(pVirtualCommand, CMD_FOR_EACH))
         szIteratorVariable = utilCreateStringf(64, TEXT("XS.Iterator%d"), ++pScriptFile->pGenerator->iIteratorID);  // [FIX]  calculateGeneratorForEachCommandCount(pScriptFile->pGenerator) + 1

      // Generate four commands
      for (UINT iSubCommand = 1; bResult AND iSubCommand <= 4; iSubCommand++)
      {
         switch (iSubCommand)
         {
         /// [INITIALISE] Create '$iterator.1 = size of array $Array' command
         case 1: StringCchPrintf(szCommandText, LINE_LENGTH, TEXT("$%s = size of array $%s"), szIteratorVariable, szArrayVariable);           break;
         /// [LOOP] Create 'while $iterator.1' command
         case 2: StringCchPrintf(szCommandText, LINE_LENGTH, TEXT("while $%s"), szIteratorVariable);                                          break;
         /// [DECREASE] Create 'dec $iterator.1' command
         case 3: StringCchPrintf(szCommandText, LINE_LENGTH, TEXT("dec $%s"), szIteratorVariable);                                            break;
         /// [ASSIGN] Create '$item = $Array[$iterator.1]' command
         case 4: StringCchPrintf(szCommandText, LINE_LENGTH, TEXT("$%s = $%s[$%s]"), szReturnVariable, szArrayVariable, szIteratorVariable);  break;
         }

         /// Attempt to generate COMMAND
         if (bResult = generateCommandFromString(pScriptFile, szCommandText, pVirtualCommand->iLineNumber, pCommand, pErrorQueue))
            // [SUCCESS] Add to Generator Input
            appendCommandToGenerator(pScriptFile->pGenerator, CL_INPUT, pCommand);
         else
            // [FAILURE] Cleanup and abort.. (No enhancement necessary)
            deleteCommand(pCommand);
      }

      // [FOR-EACH] Cleanup iterator name
      if (isCommandID(pVirtualCommand, CMD_FOR_EACH))
         utilDeleteString((TCHAR*&)szIteratorVariable);

      // Cleanup
      utilDeleteString(szCommandText);
   }

   // [ERROR] Add to macro + output queue and return FALSE
   if (pError)
      pushCommandAndOutputQueues(pError, pErrorQueue, pVirtualCommand->pErrorQueue, ET_ERROR);
   
   // Return TRUE if successul
   return !pError AND bResult;
}

/// Function name  : generateForLoopMacro
// Description     : Generates the physical commands necessary to perform the 'for loop' macro
// 
// SCRIPT_FILE*    pScriptFile     : [in/out] ScriptFile
// CONST COMMAND*  pVirtualCommand : [in]     Macro command to expand
// ERROR_QUEUE*    pErrorQueue     : [in/out] Error queue
// 
// Return Value   : TRUE/FALSE
// 
BOOL  generateForLoopMacro(SCRIPT_FILE*  pScriptFile, CONST COMMAND*  pVirtualCommand, ERROR_QUEUE*  pErrorQueue)
{
   CONST TCHAR  *szLoopVariable;       // For loop variable
   PARAMETER    *pLoopStart,           // For loop start range
                *pLoopFinish;          // For loop finish range
   ERROR_STACK  *pError;
   COMMAND      *pCommand;             // Command being processed
   TCHAR        *szCommandText;        // Text of new command being generated                
   INT           iLoopStep;            // For loop stepping value
   BOOL          bResult;

   // Prepare
   bResult = TRUE;
   pError  = NULL;

   // [CHECK] Lookup loop iterator
   if (!findVariableParameterInCommandByIndex(pVirtualCommand, 0, szLoopVariable))
      // [ERROR] "Missing loop variable in the 'for' macro on line %u : '%s'"
      pError = generateDualError(HERE(IDS_GENERATION_MACRO_LOOP_ITERATOR_MISSING), pVirtualCommand->iLineNumber + 1, pVirtualCommand->szBuffer);

   // [CHECK] Lookup loop 'range' numbers/variables
   else if (!findParameterInCommandByIndex(pVirtualCommand, PT_DEFAULT, 1, pLoopStart) OR !findParameterInCommandByIndex(pVirtualCommand, PT_DEFAULT, 2, pLoopFinish))
      // [ERROR] "Missing range variable or number in the 'for' macro on line %u : '%s'"
      pError = generateDualError(HERE(IDS_GENERATION_MACRO_LOOP_RANGE_MISSING), pVirtualCommand->iLineNumber + 1, pVirtualCommand->szBuffer);

   // [CHECK] Lookup loop 'step' 
   else if (!findIntegerParameterInCommandByIndex(pVirtualCommand, 3, iLoopStep))
      // [ERROR] "Missing loop step in the 'for' macro on line %u : '%s'"
      pError = generateDualError(HERE(IDS_GENERATION_MACRO_LOOP_STEP_MISSING), pVirtualCommand->iLineNumber + 1, pVirtualCommand->szBuffer);

   // [CHECK] Ensure 'step' is non-zero
   else if (iLoopStep == 0)
      // [ERROR] "Invalid loop step in the 'for' macro on line %u : '%s'"
      pError = generateDualError(HERE(IDS_GENERATION_MACRO_LOOP_STEP_ZERO), pVirtualCommand->iLineNumber + 1, pVirtualCommand->szBuffer);

   // [CHECK] Ensure 'range' is logical
   else if (pLoopStart->eType == DT_INTEGER AND pLoopFinish->eType == DT_INTEGER AND ((pLoopStart->iValue <= pLoopFinish->iValue AND iLoopStep < 0) OR (pLoopStart->iValue >= pLoopFinish->iValue AND iLoopStep > 0)))
      // [ERROR] "Invalid loop range %d to %d (step %d) in the 'for' macro on line %u : '%s'"
      pError = generateDualError(HERE(IDS_GENERATION_MACRO_LOOP_RANGE_INVALID), pLoopStart->iValue, pLoopFinish->iValue, iLoopStep, pVirtualCommand->iLineNumber + 1, pVirtualCommand->szBuffer);

   // [SUCCESS] Generate three commands
   else 
   {
      // Prepare
      szCommandText = utilCreateEmptyString(LINE_LENGTH);

      for (UINT iSubCommand = 1; bResult AND iSubCommand <= 3; iSubCommand++)
      {
         switch (iSubCommand)
         {
         /// [INITIALISE] Create '$iterator = initial ± step' command
         case 1: StringCchPrintf(szCommandText, LINE_LENGTH, TEXT("$%s = %s %s %d"), szLoopVariable, pLoopStart->szBuffer, iLoopStep > 0 ? TEXT("-") : TEXT("+"), utilCalculateMagnitude(iLoopStep)); break;
         /// [CONDITIONAL] Create 'while $iterator <|> count' command
         case 2: StringCchPrintf(szCommandText, LINE_LENGTH, TEXT("while $%s %s %s"), szLoopVariable, iLoopStep > 0 ? TEXT("<") : TEXT(">"), pLoopFinish->szBuffer);                                  break;
         /// [COUNTER] Create '$iterator = $iterator ± step' command
         case 3: StringCchPrintf(szCommandText, LINE_LENGTH, TEXT("$%s = $%s %s %d"), szLoopVariable, szLoopVariable, iLoopStep > 0 ? TEXT("+") : TEXT("-"), utilCalculateMagnitude(iLoopStep));      break;
         }

         /// Attempt to generate COMMAND
         if (bResult = generateCommandFromString(pScriptFile, szCommandText, pVirtualCommand->iLineNumber, pCommand, pErrorQueue))
            // [SUCCESS] Add to Generator Input
            appendCommandToGenerator(pScriptFile->pGenerator, CL_INPUT, pCommand);
         else
            // [FAILURE] Cleanup and abort.. (No enhancement necessary)
            deleteCommand(pCommand);
      }

      // Cleanup
      utilDeleteString(szCommandText);
   }

   // [ERROR] Add to macro + output queue and return FALSE
   if (pError)
      pushCommandAndOutputQueues(pError, pErrorQueue, pVirtualCommand->pErrorQueue, ET_ERROR);
   
   // Return TRUE if successul
   return !pError AND bResult;
}

/// Function name  : generateScriptMacroCommands
// Description     : Generates and inserts the appropriate standard COMMANDs from a virtual COMMAND
// 
// SCRIPT_FILE*    pScriptFile     : [in/out] ScriptFile to insert standard COMMANDs into
// COMMAND*        pVirtualCommand : [in/out] Virtual COMMAND to generate standard commands from
// ERROR_QUEUE*    pErrorQueue     : [in/out] ErrorQueue
// 
// Return Value   : TRUE if successful, FALSE if there was an error
// 
BOOL  generateScriptMacroCommands(SCRIPT_FILE*  pScriptFile, COMMAND*  pVirtualCommand, ERROR_QUEUE*  pErrorQueue)
{
   ERROR_STACK*  pError;    // Error
   BOOL          bResult;   // Operation result

   // Prepare
   bResult = TRUE;
   pError  = NULL;

   // [CHECK] Ensure command isn't within a skip-if block
   if (isLastGeneratedCommandIndentedBySkipIf(pScriptFile->pGenerator))
   {
      // [ERROR] "Cannot use the macro command '%s' on line %u within a 'skip/do if' conditional"  
      pError = generateDualError(HERE(IDS_GENERATION_VIRTUAL_SKIP_IF), pVirtualCommand->szBuffer, pVirtualCommand->iLineNumber + 1);
      pushCommandAndOutputQueues(pError, pErrorQueue, pVirtualCommand->pErrorQueue, ET_ERROR);
   }
   // Examine command ID
   else switch (pVirtualCommand->iID)
   {
   /// [DEFINE ARRAY] 'dim $Array = $c, $d, $e, $f, $g, $h'
   case CMD_DEFINE_ARRAY:
      bResult = generateDefineArrayMacro(pScriptFile, pVirtualCommand, pErrorQueue);
      break;

   /// [FOR EACH IN ARRAY] 'for each $item in array $Array [using counter $iterator]'
   case CMD_FOR_EACH:
   case CMD_FOR_EACH_COUNTER:
      bResult = generateForEachMacro(pScriptFile, pVirtualCommand, pErrorQueue);
      break;

   /// [FOR LOOP]  'for $item = 0 to 8 step 1'
   case CMD_FOR_LOOP:
      bResult = generateForLoopMacro(pScriptFile, pVirtualCommand, pErrorQueue);
      break;

   /// [READ CUSTOM MENU xxxx] 
   case CMD_ADD_MENU_ITEM_BYREF:     // add custom menu item to array $0: page=$1x id=$2y returnvalue=$3
   case CMD_ADD_MENU_INFO_BYREF:     // add custom menu info line to array $0: page=$1x id=$2y
   case CMD_ADD_MENU_HEADING_BYREF:  // add custom menu heading to array $0: page=$1x id=$2y
      bResult = generateCustomMenuMacro(pScriptFile, pVirtualCommand, pErrorQueue);
      break;
   }

   // Return result
   return !pError AND bResult;
}



/// Function name  : generateScriptCommands
// Description     : Extract the script commands from a CodeEdit control and parse them into COMMAND objects.
//                      These are then stored in the ScriptFile.
// 
// HWND                 hCodeEdit     : [in]  CodeEdit containing the text of each command
// SCRIPT_FILE*         pScriptFile   : [out] ScriptFile containing the script properties and output list
// OPERATION_PROGRESS*  pProgress     : [in]  Progress tracker
// ERROR_QUEUE*         pErrorQueue   : [in]  Error messages, if any
// 
// Return Value   : TRUE if the COMMANDs were parsed successfully, FALSE if there were errors
// 
BOOL   generateScriptCommands(HWND  hCodeEdit, SCRIPT_FILE*  pScriptFile, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE* pErrorQueue)
{
   TCHAR*       szLine = NULL;     // Text of the script command currently being processed
   UINT         iCommandCount;     // Number of lines in the CodeEdit
   COMMAND*     pCommand = NULL;   // COMMAND of the script command currently being processed

   __try
   {
      // Clear any previous errors and determine line count
      CodeEdit_ClearLineErrors(hCodeEdit);
      iCommandCount = Edit_GetLineCount(hCodeEdit);

      /// Attempt to parse each script command into a new COMMAND object
      for (UINT iLine = 0; iLine < iCommandCount; iLine++)
      {
         // Prepare
         pCommand = NULL;
         szLine   = CodeEdit_GetLineText(hCodeEdit, iLine);

         // [PROGRESS] Update progress object, if present
         if (pProgress)
            updateOperationProgressValue(pProgress, iLine);

         /// Attempt to parse script command into a COMMAND object with PARAMETERS
         if (generateCommandFromString(pScriptFile, szLine, iLine, pCommand, pErrorQueue))
         {
            // [CHECK] Is command a macro?
            if (!isCommandType(pCommand, CT_VIRTUAL))
               /// [PHYSICAL] Add directly to GENERATION INPUT
               appendCommandToGenerator(pScriptFile->pGenerator, CL_INPUT, pCommand);
            
            else
            {
               /// [MACROS] Generate equivilent standard commands directly into GENERATION INPUT
               if (!generateScriptMacroCommands(pScriptFile, pCommand, pErrorQueue))
                  // [FAILURE] Inform CodeEdit this line contains an error
                  setCodeEditLineError(hCodeEdit, pCommand);

               // Destroy virtual command
               deleteCommand(pCommand);
            }
         }
         else
         {  // [FAILURE] Inform CodeEdit this line contains an error
            setCodeEditLineError(hCodeEdit, pCommand);
            deleteCommand(pCommand);
         }

         // Cleanup
         utilDeleteString(szLine);
      }

      /// [SKIP-IF] Ensure no command uses a recursive Skip-If
      if (validateSkipIfCommands(pScriptFile, pErrorQueue))
         /// [VAR USAGE] Ensure all variables are assigned before use
         validateVariableUsage(pScriptFile, pErrorQueue);

      // Return TRUE if there were no errors (warnings are acceptable)
      return (identifyErrorQueueType(pErrorQueue) != ET_ERROR);
   }
   PUSH_CATCH1(pErrorQueue, "szLine=%s", szLine);
   debugCommand(pCommand);
   return FALSE;
}


/// Function name  : validateExpressionParameters
// Description     : 
// 
// CONST COMMAND*  pCommand : [in] 
// ERROR_STACK*  &pError : [in] 
// 
// Return Value   : 
// 
BOOL  validateExpressionParameters(CONST COMMAND*  pCommand, ERROR_STACK*  &pError)
{
   PARAMETER  *pNextParameter,
              *pParameter;

   // Prepare
   pError = NULL;

   // [CHECK] Ensure at least one parameter was found
   if (getCommandParameterCount(pCommand, PT_DEFAULT) < 2)
      // [ERROR] "Missing assignment value in the expression command on line %u : '%s'"
      pError = generateDualError(HERE(IDS_GENERATION_EXPRESSION_ASSIGNMENT_MISSING), pCommand->iLineNumber, pCommand->szBuffer);

   else
   {
      // Iterate through parameters
      for (UINT  iIndex = 1; !pError AND findParameterInCommandByIndex(pCommand, PT_DEFAULT, iIndex, pParameter); iIndex++)
      {
         // Get next parameter, if any
         findParameterInCommandByIndex(pCommand, PT_DEFAULT, iIndex + 1, pNextParameter);
      
         // Examine
         switch (pParameter->eType)
         {
         /// [OBJECTS] Allow binary operators and opening brackets
         default:
            // [CHECK] Allow no operators
            if (pNextParameter)
            {
               // [CHECK] Ensure followed by an operator
               if (pNextParameter->eType != DT_OPERATOR)
                  // [ERROR] "Unexpected value '%s' following '%s' in the expression command on line %u : '%s'"
                  pError = generateDualError(HERE(IDS_GENERATION_EXPRESSION_OPERAND_INVALID), pNextParameter->szBuffer, pParameter->szBuffer, pCommand->iLineNumber, pCommand->szBuffer);

               else if (isUnaryOperator((OPERATOR_TYPE)pNextParameter->iValue) OR pNextParameter->iValue == OT_BRACKET_OPEN)
                  // [ERROR] "Unexpected operator '%s' detected following '%s' in the expression command on line %u : '%s'"
                  pError = generateDualError(HERE(IDS_GENERATION_EXPRESSION_OPERATOR_INVALID), pNextParameter->szBuffer, pParameter->szBuffer, pCommand->iLineNumber, pCommand->szBuffer);
            }
            break;

         ///// [VARIABLE/NUMBER] Allow only binary operators
         //case DT_VARIABLE:
         //case DT_INTEGER:
         //   // [CHECK] Allow no operators
         //   if (pNextParameter)
         //   {
         //      // [CHECK] Ensure followed by an operator
         //      if (pNextParameter->eType != DT_OPERATOR)
         //         // [ERROR] "Missing operator following '%s' in the expression command on line %u : '%s'"
         //         pError = generateDualError(HERE(IDS_GENERATION_EXPRESSION_OPERATOR_MISSING), pParameter->szBuffer, pCommand->iLineNumber, pCommand->szBuffer);

         //      // [CHECK] Ensure followed by binary operator
         //      else switch (pNextParameter->iValue)
         //      {
         //      case OT_BRACKET_OPEN:
         //      case OT_LOGICAL_NOT:
         //      case OT_BITWISE_NOT:
         //      case OT_MINUS:
         //         // [ERROR] "Unexpected operator '%s' detected following '%s' in the expression command on line %u : '%s'"
         //         pError = generateDualError(HERE(IDS_GENERATION_EXPRESSION_OPERATOR_INVALID), pParameter->szBuffer, pCommand->iLineNumber, pCommand->szBuffer);
         //      }
         //   }
         //   break;

         /// [OPERATOR] Ensure operand is valid
         case DT_OPERATOR:
            // Examine operator
            switch (pParameter->iValue)
            {
            /// [CLOSING BRACKET] Ensure followed by a binary operator (or closing bracket)
            case OT_BRACKET_CLOSE:
               // Allow nothing
               if (pNextParameter)
               {
                  // [CHECK] Ensure operator is present
                  if (pNextParameter->eType != DT_OPERATOR)
                     // [ERROR] "Missing operator following '%s' in the expression command on line %u : '%s'"
                     pError = generateDualError(HERE(IDS_GENERATION_EXPRESSION_OPERATOR_MISSING), pParameter->szBuffer, pCommand->iLineNumber, pCommand->szBuffer);

                  // [CHECK] Allow closing brackets and binary operators
                  else switch (pNextParameter->iValue)
                  {
                  case OT_BRACKET_OPEN:
                  case OT_LOGICAL_NOT:
                  case OT_BITWISE_NOT:
                  case OT_MINUS:
                     // [ERROR] "Unexpected operator '%s' detected following '%s' in the expression command on line %u : '%s'"
                     pError = generateDualError(HERE(IDS_GENERATION_EXPRESSION_OPERATOR_INVALID), pNextParameter->szBuffer, pParameter->szBuffer, pCommand->iLineNumber, pCommand->szBuffer);
                  }
               }
               break;

            /// [OPENING BRACKET] Ensure followed by operand or unary operator (or opening bracket)
            case OT_BRACKET_OPEN:
               // Ensure operand/operator is present
               if (!pNextParameter) 
                  // [ERROR] "Missing value following '%s' in the expression command on line %u : '%s'"
                  pError = generateDualError(HERE(IDS_GENERATION_EXPRESSION_OPERAND_MISSING), pParameter->szBuffer, pCommand->iLineNumber, pCommand->szBuffer);

               // [CHECK] Allow operands
               else if (pNextParameter->eType == DT_OPERATOR)
                  // [OPERATOR] Ensure operator isn't binary
                  switch (pNextParameter->iValue)
                  {
                  case OT_BRACKET_OPEN:
                  case OT_LOGICAL_NOT:
                  case OT_BITWISE_NOT:
                  case OT_MINUS:
                     break;

                  default:
                     // [ERROR] "Unexpected operator '%s' detected following '%s' in the expression command on line %u : '%s'"
                     pError = generateDualError(HERE(IDS_GENERATION_EXPRESSION_OPERATOR_INVALID), pNextParameter->szBuffer, pParameter->szBuffer, pCommand->iLineNumber, pCommand->szBuffer);
                  }
               break;

            /// [UNARY OPERATOR] Ensure followed by operand (or opening bracket)
            case OT_MINUS:
            case OT_LOGICAL_NOT:
            case OT_BITWISE_NOT:
               // [CHECK] Ensure operand isn't missing 
               if (!pNextParameter) 
                  // [ERROR] "Missing value following '%s' in the expression command on line %u : '%s'"
                  pError = generateDualError(HERE(IDS_GENERATION_EXPRESSION_OPERAND_MISSING), pParameter->szBuffer, pCommand->iLineNumber, pCommand->szBuffer);

               // [CHECK] Ensure operand is integer, variable or opening bracket
               else switch (pNextParameter->eType)
               {
               case DT_OPERATOR:
                  if (pNextParameter->iValue != OT_BRACKET_OPEN)
                     // [ERROR] "Unexpected operator '%s' detected following '%s' in the expression command on line %u : '%s'"
                     pError = generateDualError(HERE(IDS_GENERATION_EXPRESSION_OPERATOR_INVALID), pNextParameter->szBuffer, pParameter->szBuffer, pCommand->iLineNumber, pCommand->szBuffer);
                  break;

               case DT_INTEGER:
               case DT_VARIABLE:
               case DT_CONSTANT:
                  break;

               default:
                  // [ERROR] "Illegal operation attempted on '%s' in the expression command on line %u : '%s'"
                  pError = generateDualError(HERE(IDS_GENERATION_EXPRESSION_UNARY_INVALID), pNextParameter->szBuffer, pCommand->iLineNumber, pCommand->szBuffer);
                  break;
               }
               break;

            /// [BINARY OPERATOR] Ensure followed by operand or unary operator (or opening bracket)
            default:
               // [CHECK] Ensure operand isn't missing
               if (!pNextParameter) 
                  // [ERROR] "Missing value following '%s' in the expression command on line %u : '%s'"
                  pError = generateDualError(HERE(IDS_GENERATION_EXPRESSION_OPERAND_MISSING), pParameter->szBuffer, pCommand->iLineNumber, pCommand->szBuffer);

               // [CHECK] Allow all operands
               else if (pNextParameter->eType == DT_OPERATOR)
                  // [OPERATOR] Allow unary operators an open bracket
                  switch (pNextParameter->iValue)
                  {
                  case OT_BRACKET_OPEN:
                  case OT_LOGICAL_NOT:
                  case OT_BITWISE_NOT:
                  case OT_MINUS:
                     break;

                  /*case OT_LOGICAL_AND:
                  case OT_LOGICAL_OR:*/

                  default:
                     // [ERROR] "Unexpected operator '%s' detected following '%s' in the expression command on line %u : '%s'"
                     pError = generateDualError(HERE(IDS_GENERATION_EXPRESSION_OPERATOR_INVALID), pNextParameter->szBuffer, pParameter->szBuffer, pCommand->iLineNumber, pCommand->szBuffer);
                  }
               break;
            }
            break;
         }
      }
   }

   // Return TRUE if succesful
   return (pError == NULL);
}

/// Function name  : validateScriptProperties
// Description     : Validates the ScriptFile properties
// 
// CONST SCRIPT_FILE*  pScriptFile : [in] ScriptFile
// ERROR_QUEUE*        pErrorQueue : [in/out] ErrorQueue
// 
// Return Value   : TRUE if successful or warnings, FALSE on error
// 
BOOL  validateScriptProperties(CONST SCRIPT_FILE*  pScriptFile, ERROR_QUEUE*  pErrorQueue)
{
   SCRIPT_OBJECT*  pCommandNameLookup = NULL;
   GAME_STRING*    pCommandIDLookup = NULL;
   ERROR_STACK*    pError = NULL;

   __try
   {
      /// [SCRIPT NAME]
      if (!lstrlen(pScriptFile->szScriptname))
         // [ERROR] "This script has no name"
         pError = generateDualWarning(HERE(IDS_GENERATION_SCRIPTNAME_MISSING));

      /// [COMMAND ID] Ensure ID matches known command
      else if (isStringNumeric(pScriptFile->szCommandID) AND !findGameStringByID(utilConvertStringToInteger(pScriptFile->szCommandID), GPI_OBJECT_COMMANDS, pCommandIDLookup))
         // [WARNING] "This script implements an unknown ship or station command with ID %s"
         pError = generateDualWarning(HERE(IDS_GENERATION_COMMAND_UNRECOGNISED), pScriptFile->szCommandID);

      /// [COMMAND NAME] Ensure name matches known command
      else if (lstrlen(pScriptFile->szCommandID) AND !findScriptObjectByText(pScriptFile->szCommandID, pCommandNameLookup))
         // [WARNING] "This script implements an unknown ship or station command with ID %s", 
         pError = generateDualWarning(HERE(IDS_GENERATION_COMMAND_UNRECOGNISED), pScriptFile->szCommandID);
      
      // Return TRUE if successful or only warnings
      return !isError(pError);
   }
   PUSH_CATCH0(pErrorQueue, "");
   debugGameString(pCommandIDLookup);
   debugObjectName(pCommandNameLookup);
   return FALSE;
}


/// Function name  : validateSkipIfCommands
// Description     : Ensures no commands recursively attempt to use the skip-if conditional
// 
// SCRIPT_FILE*  pScriptFile : [in]     ScriptFile containing generated commands
// ERROR_QUEUE*  pErrorQueue : [in/out] Error Queue
// 
// Return Value   : TRUE if successful, FALSE if one or more commands recursively use the skip-if conditional
// 
BOOL  validateSkipIfCommands(CONST SCRIPT_FILE*  pScriptFile, ERROR_QUEUE*  pErrorQueue)
{
   ERROR_STACK   *pError;
   COMMAND       *pCommand,
                 *pPrevSkipIf;

   // Prepare
   pPrevSkipIf = NULL;
   pError      = NULL;
   
   // Iterate through all commands
   for (UINT  iLine = 0; findCommandInGeneratorInput(pScriptFile->pGenerator, iLine, pCommand); iLine++)
   {
      switch (pCommand->iID)
      {
      /// [COMMENT/EMPTY] Ignore
      case CMD_COMMENT:
      case CMD_COMMAND_COMMENT:
      case CMD_NOP:
      case CMD_NONE:
         break;

      /// [COMMAND]
      default:
         // [SKIP-IF] Are we within a skip-if bracket?
         if (pPrevSkipIf AND (isIfConditional(pCommand->eConditional) OR isSkipIfConditional(pCommand->eConditional)))
            // [ERROR] "Cannot use 'skip/do if' on the command on line %u due to previous 'skip/do if' on line %u"
            pushErrorQueue(pErrorQueue, pError = generateDualError(HERE(IDS_GENERATION_RECURSIVE_SKIP_IF), iLine + 1, pPrevSkipIf->iLineNumber + 1));
            
         // [CHECK] Store or reset 'previous skip-if' marker
         pPrevSkipIf = (isSkipIfConditional(pCommand->eConditional) ? pCommand : NULL);
         break;
      }
   }

   // Return TRUE if there were no errors
   return (pError == NULL);
}


/// Function name  : validateVariableUsage
// Description     : Ensures all variables are assigned before usage
// 
// SCRIPT_FILE*  pScriptFile : [in]     ScriptFile containing generated commands
// ERROR_QUEUE*  pErrorQueue : [in/out] Error queue
// 
// Return Value   : TRUE if successful, FALSE if one or variables was used before being assigned
// 
BOOL  validateVariableUsage(CONST SCRIPT_FILE*  pScriptFile, ERROR_QUEUE*  pErrorQueue)
{
   ERROR_STACK   *pError;
   COMMAND       *pCommand;
   PARAMETER     *pParameter;
   VARIABLE_NAME *pVariableName;
   AVL_TREE      *pVariablesTree;

   // Prepare
   pVariablesTree = createVariableNameTreeByText(); 
   pError         = NULL;

   // Prepare VariableNames tree
   for (UINT  iIndex = 0; findVariableNameInGeneratorByIndex(pScriptFile->pGenerator, iIndex, pVariableName); iIndex++)
   {
      // Generate copy and calculate initial usage
      pVariableName            = duplicateVariableName(pVariableName);
      pVariableName->eAssigned = (isArgumentInScriptFile(pScriptFile, pVariableName->szName) ? VA_ASSIGNED : VA_UNASSIGNED);
      // Insert into temporary tree
      insertObjectIntoAVLTree(pVariablesTree, (LPARAM)pVariableName);
   }
   
   // Iterate through all commands
   for (UINT  iLine = 0; findCommandInGeneratorInput(pScriptFile->pGenerator, iLine, pCommand); iLine++)
   {
      // Examine command
      switch (pCommand->iID)
      {
      /// [COMMAND] Check all variables
      default:
         // Iterate through all parameters
         for (UINT  iIndex = 0; findParameterInCommandByIndex(pCommand, PT_DEFAULT, iIndex, pParameter); iIndex++)
         {
            // [CHECK] Is this a variable or return-variable?
            if (pParameter->eType == DT_VARIABLE OR (isReturnObject(pParameter->eSyntax) AND pParameter->eType == DT_INTEGER AND identifyReturnObjectType(pParameter->iValue) == ROT_VARIABLE))
            {
               // [SUCCESS] Extract variable by name
               if (!findObjectInAVLTreeByValue(pVariablesTree, (LPARAM)identifyRawVariableName(pParameter->szValue), NULL, (LPARAM&)pVariableName))
                  VERBOSE("ERROR: validateVariableUsage : Variable '%s' is missing", pParameter->szValue);
               ASSERT(pVariableName);
            
               // [CHECK] Is this the ReturnObject?
               if (isReturnObject(pParameter->eSyntax) AND pVariableName->eAssigned == VA_UNASSIGNED)
                  /// [ASSIGNMENT] Mark as assigned
                  pVariableName->eAssigned = VA_ASSIGNED;
               
               /// [USAGE] Ensure variable has already been assigned
               else if (pVariableName->eAssigned == VA_UNASSIGNED)
               {
                  // [WARNING] "The variable '$%s' on line %u is being used without being assigned"  
                  pushErrorQueue(pErrorQueue, generateDualWarning(HERE(IDS_GENERATION_UNASSIGNED_VARIABLE), pVariableName->szName, pCommand->iLineNumber + 1));
                  pVariableName->eAssigned = VA_REPORTED;
               }
            }
         }
         break;

      /// [COMMENT/EMPTY] Ignore
      case CMD_COMMENT:
      case CMD_COMMAND_COMMENT:
      case CMD_NOP:
      case CMD_NONE:
         break;
      }
   }

   // Return TRUE if there were no errors
   deleteAVLTree(pVariablesTree);
   return (pError == NULL);
}
