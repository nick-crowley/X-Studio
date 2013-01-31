//
// Script Translation.cpp : Provides the high-level functions for MSCI script translation
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : loadScriptCallCommandTargetScript
// Description     : Loads the properties of the target of a script-call COMMAND into a new ScriptFile
// 
// CONST SCRIPT_FILE*  pCallingScript : [in]     The script containing the script-call COMMAND being processed
// CONST COMMAND*      pCommand       : [in]     The script-call COMMAND being processed
// HWND                hParentWnd     : [in]     The parent window responsible for error handling
// SCRIPT_FILE*       &pOutputScript  : [out]    New ScriptFile containing only the properties of the target script, if successful, otherwise NULL
// ERROR_QUEUE*        pErrorQueue    : [in/out] Error message if any, may already contain errors.
// 
// Return Value : TRUE if successful, FALSE otherwise
// 
BearScriptAPI 
BOOL  loadScriptCallCommandTargetScript(CONST SCRIPT_FILE*  pCallingScript, CONST COMMAND* pCommand, HWND  hParentWnd, SCRIPT_FILE* &pOutputScript, ERROR_QUEUE*  pErrorQueue)
{
   TCHAR*    szTargetScriptPath;    // Full path of the target script

   // Prepare
   pOutputScript = NULL;

   /// Generate full path of target script (Script must be in the same folder)
   szTargetScriptPath = calculateScriptCallTargetFilePath(pCallingScript->szFullPath, pCommand);

   /// Load the target script properties into a new ScriptFile
   if (szTargetScriptPath AND loadScriptProperties(szTargetScriptPath, hParentWnd, pOutputScript, pErrorQueue) != OR_SUCCESS)
      // [ERROR] Destroy output ScriptFile
      deleteScriptFile(pOutputScript);

   // Cleanup and return TRUE if successful
   utilSafeDeleteString(szTargetScriptPath);
   return (pOutputScript != NULL);
}


/// Function name  : loadScriptProperties
// Description     : Loads the properties of the specified script into a new ScriptFile
// 
// CONST TCHAR*   szFullPath   : [in]  Full file path of the script to load
// HWND           hParentWnd   : [in]  Parent window responsible for error handling
// SCRIPT_FILE*  &pScriptFile  : [out] New ScriptFile containing the properties of the specified script, if successful. 
///                                                            You are responsible for destroying it.
// ERROR_QUEUE*   pErrorQueue  : [in]  Error messages, if any. May already contain errors.
// 
// Return Value   : OR_SUCCESS : The script properties were loaded successfully, any errors encountered were ignored by the user
//                  OR_FAILURE : The script was partially loaded but I/O or structural errors prevented it from being loaded successfully
//                  OR_ABORTED : The script was partially loaded but the user aborted after a minor error
// 
OPERATION_RESULT  loadScriptProperties(CONST TCHAR*  szFullPath, HWND  hParentWnd, SCRIPT_FILE*  &pScriptFile, ERROR_QUEUE*  pOutputErrorQueue)
{
   OPERATION_RESULT  eResult;                // Operation result, defaults to SUCCESS
   ERROR_QUEUE*      pTranslationErrors;     // Errors encountered during script translation
   ERROR_STACK*      pError;                 // Used for transferring errors between the translation and output error queues
   TCHAR*            szScriptName;           // Name of the target script (Only used for I/O error reporting)
   
   // Prepare
   pTranslationErrors = createErrorQueue();

   // Create ScriptFile and set PATH
   pScriptFile = createScriptFileByOperation(SFO_TRANSLATION, szFullPath);

   // [vERBOSE]
   VERBOSE_SMALL_PARTITION();
   VERBOSE("Loading external script call '%s'", identifyScriptName(pScriptFile));

   /// [LOAD] Load external script into ScriptFile buffer
   if (!loadGameFileFromFileSystemByPath(getFileSystem(), pScriptFile, TEXT(".xml"), pOutputErrorQueue))
   {
      // Create string containing only the script name (no folder, no extension)
      szScriptName = utilDuplicateSimpleString(PathFindFileName(szFullPath));
      utilFindCharacterReverse(szScriptName, '.')[0] = NULL;   // Script is guaranteed to have .pck on the end

      // [OUTPUT-ERROR] "The external MSCI script '%s' is unavailable or could not be accessed"
      enhanceLastWarning(pOutputErrorQueue, ERROR_ID(IDS_SCRIPT_CALL_LOAD_IO_ERROR), szScriptName);
      generateOutputTextFromLastError(pOutputErrorQueue);

      // [FAILURE]
      eResult = OR_FAILURE;

      // Cleanup
      utilDeleteString(szScriptName);
   }
   else
   {
      /// [SUCCESS] Read the script and extract the properties
      eResult = translateScript(pScriptFile, TRUE, hParentWnd, NULL, pTranslationErrors);

      // [ERRORS] Change any external script translation errors into warnings for the calling script's error queue
      while (pError = popErrorQueue(pTranslationErrors))
      {
         // [WARNING] "Minor errors were detected in the external script '%s' which may have affected translation"
         enhanceWarning(pError, ERROR_ID(IDS_TRANSLATION_SCRIPT_TARGET_MINOR_ERRORS), pScriptFile->szScriptname);
         generateOutputTextFromError(pError);
         // Add to output queue
         pushErrorQueue(pOutputErrorQueue, pError);
      }
   }

   // Cleanup and return result
   deleteErrorQueue(pTranslationErrors);
   return eResult;
}


/// Function name  : translateCommandExpression
// Description     : Build the default parameter list in an Expression command from the infix and postfix lists
// 
// COMMAND*                pCommand        : [in/out] Command containing the infix/postfix lists
// CONST PARAMETER_COUNT*  pParameterCount : [in]     Parameter count object containing the infix/postfix counts
// ERROR_STACK*           &pError          : [out]    New Error message if any, otherwise NULL
// 
// Return Value   : TRUE if successful, FALSE if there were errors
// 
BOOL  translateCommandExpression(COMMAND*  pCommand, CONST PARAMETER_COUNT*  pParameterCount, ERROR_STACK* &pError)
{
   PARAMETER  *pIterator,           // Infix parameter list iterator
              *pPostfixParameter;   // PostFix parameter

   // [CHECK] Ensure command is an expression with only 1 existing parameter (ReturnObject)
   ASSERT(pCommand->iID == CMD_EXPRESSION AND pCommand->pParameterArray->iCount == 1);
   // [CHECK] Ensure command is not a command comment, as expression parameters are already saved in infix order
   ASSERT(!isCommandType(pCommand, CT_CMD_COMMENT));

   // Prepare
   pError = NULL;

   // [CHECK] Ensure Infix parameter count is correct
   if (pParameterCount->iInfixCount != pCommand->pInfixParameterArray->iCount)
   {
      // [ERROR] "The expression command should have %u infix parameters but only %u were detected"
      pError = generateDualError(HERE(IDS_SCRIPT_EXPRESSION_INFIX_COUNT_MISMATCH), pParameterCount->iInfixCount, pCommand->pInfixParameterArray->iCount);
   }
   // [CHECK] Ensure postfix parameter count is correct
   else if (pParameterCount->iPostfixCount != pCommand->pPostfixParameterArray->iCount)
   {
      // [ERROR] "The expression command should have %u postfix parameters but only %u were detected"
      pError = generateDualError(HERE(IDS_SCRIPT_EXPRESSION_POSTFIX_COUNT_MISMATCH), pParameterCount->iInfixCount, pCommand->pPostfixParameterArray->iCount);
   }
   else
   {
      // PostFix array contains only normal parameters
      // Infix array contains DT_EXPRESSION and DT_OPERATOR parameters, where the value of the DT_EXPRESSION ones are indicies into the POSTFIX array
      
      /// Iterate through 'Infix' parameter list creating the 'default' parameter list item by item.
      for (UINT  i = 0; findParameterInCommandByIndex(pCommand, PT_INFIX, i, pIterator) AND !pError; i++)
      {
         switch (pIterator->eType)
         {
         /// [EXPRESSION] Copy matching item from PostFix list
         case DT_EXPRESSION:
            // [CHECK] Index must be negative
            ASSERT(pIterator->iValue < 0  AND  pIterator->eSyntax == PS_EXPRESSION);

            // Lookup item in PostFix array   (Convert negative one-based index to a positive zero-based index)
            if (!findParameterInCommandByIndex(pCommand, PT_POSTFIX, -pIterator->iValue - 1, pPostfixParameter))
            {
               // [ERROR] "The parameter defined by the <sourcevalue> tag on line %u references a parameter index '%d' that cannot be found in the expression command's postfix parameter list"
               pError = generateDualError(HERE(IDS_SCRIPT_EXPRESSION_POSTFIX_NOT_FOUND), pIterator->iLineNumber, pIterator->iValue);
            }
            else
               // Copy to default array
               appendParameterToCommand(pCommand, duplicateParameter(pPostfixParameter), PT_DEFAULT);
            break;

         /// [OPERATOR] Copy to default array verbatim
         case DT_OPERATOR:
            appendParameterToCommand(pCommand, duplicateParameter(pIterator), PT_DEFAULT);
            break;
         }
      }
   }

   // Return TRUE if there were no errors
   return (pError == NULL);
}


/// Function name  : translateCommandNode
// Description     : Create a new COMMAND from a CommandNode
// 
// SCRIPT_FILE*   pScriptFile  : [in]     ScriptFile containing the CommandNode
// COMMAND_NODE*  pCommandNode : [in]     CommandNode containing the command data as an XMLTreeNode
// HWND           hParentWnd   : [in]     Parent window for displaying error messages
// COMMAND*      &pOutput      : [out]    New COMMAND whether successful or not
// ERROR_QUEUE*   pErrorQueue  : [in/out] Error messages encountered, if any. May already contain errors.
// 
// Return Value : OR_SUCCESS - The CommandNode and parameters were translated, there may have been errors but they were ignored
//                OR_FAILURE - The CommandNode was translated but the parameters were not (due to errors)
//                OR_ABORTED - The CommandNode was partially translated but the user aborted due to errors
// 
OPERATION_RESULT  translateCommandNode(CONST SCRIPT_FILE*  pScriptFile, CONST COMMAND_NODE*  pCommandNode, HWND  hParentWnd, COMMAND*  &pOutput, ERROR_QUEUE*  pErrorQueue)
{
   ERROR_STACK*  pError;           // Critical error encountered during the pre-translation activities, if any
   UINT          iChildNodeCount;  // Number of child-nodes expected in the CommandNode being processed

   // Prepare
   pError          = NULL;
   iChildNodeCount = NULL;

   /// Create basic COMMAND from CommandNode
   pOutput = createCommandFromNode(pScriptFile, pCommandNode, pError);

   // NEW: Change LineNumber to internal?
   pOutput->iLineNumber = getListItemCount(pScriptFile->pTranslator->pOutputList) + 1;

   // [SUCCESS] Use the syntax in the COMMAND to interpret the command nodes
   if (pOutput->eResult == OR_SUCCESS)
   {
      /// [CHECK] Ensure all child nodes are present
      if (!getXMLPropertyInteger(pCommandNode->pNode, TEXT("size"), (INT&)iChildNodeCount) OR iChildNodeCount != getXMLNodeCount(pCommandNode->pNode))
         // [CRITICAL ERROR] "Either the <sourcevalue> tag on line %u has the incorrect number of child tags or the number of child tags is not defined. %u child tags were detected but %u were declared"
         pError = generateDualError(HERE(IDS_SCRIPT_SOURCEVALUE_SIZE_MISMATCH), pCommandNode->pNode->iLineNumber, getXMLNodeCount(pCommandNode->pNode), iChildNodeCount);
      
      /// [CHECK] Ensure actual number of child nodes is appropriate
      else if (!verifyCommandChildNodeCount(pOutput, iChildNodeCount, pError))
         // [CRITICAL ERROR] "The command has an inappropriate number of child <sourcevalue> tags"
         enhanceError(pError, ERROR_ID(IDS_SCRIPT_COMMAND_NODE_COUNT_MISMATCH));
      
      /// Read parameter nodes into unverified PARAMETERs
      else if (!translateCommandNodeParameterNodes(pCommandNode, pOutput, pError))
         // [ERROR] "There was an error while parsing one or more of the command's parameter nodes"
         enhanceError(pError, ERROR_ID(IDS_SCRIPT_PARAMETER_PARSING_FAILED));

      // Convert any label number parameters into label names
      else if ((isCommandID(pOutput, CMD_GOTO_SUB) OR isCommandID(pOutput, CMD_GOTO_LABEL)) AND !convertLabelNumberParameterToLabel(pCommandNode, pOutput, pError))
         // [ERROR] "There was an error while resolving the destination of the command's label or subroutine"
         enhanceError(pError, ERROR_ID(IDS_SCRIPT_PARAMETER_RESOLUTION_FAILED));
   }

   /// [CRITICAL ERROR] Do not attempt to translate to string
   if (pError)
   {
      // [CHECK] Was the syntax identified?
      if (pOutput->pSyntax)
         // [SYNTAX FOUND] Display the suggestion text
         StringCchCopy(pOutput->szBuffer, LINE_LENGTH, pOutput->pSyntax->szSuggestionText);
      else
         // [UNRECOGNISED COMMAND] Display a placeholder
         StringCchCopy(pOutput->szBuffer, LINE_LENGTH, TEXT("<Unrecognised Command>"));

      // [ERROR] "Could not translate the %s script command '%s' (ID:%03u) on line %u (XML line %u)"
      enhanceError(pError, ERROR_ID(IDS_SCRIPT_COMMAND_TRANSLATION_FAILED), identifyCommandTypeString(pCommandNode->eType), pOutput->szBuffer, pOutput->iID, pOutput->iLineNumber, pCommandNode->pNode->iLineNumber);
      // Create output attachment and add to output and command error queues
      pushCommandAndOutputQueues(pError, pErrorQueue, pOutput->pErrorQueue, ET_ERROR);
      // [QUESTION] "A minor error was detected while translating the script '%s', would you like to continue processing?"
      //         -> Return FAILURE for this COMMAND if user chooses to continue. (Otherwise return ABORTED)
      pOutput->eResult = (displayOperationError(hParentWnd, pError, ERROR_ID(IDS_SCRIPT_TRANSLATION_MINOR_ERROR), identifyScriptName(pScriptFile)) == EH_IGNORE ? OR_FAILURE : OR_ABORTED);
   }
   /// [SUCCESS] Attempt to translate the COMMAND and parameters to a string
   else 
      (pOutput->eResult = translateCommandToString(pScriptFile, pOutput, hParentWnd, pErrorQueue));
   //{
   //// [SUCCESS] Generate a warning if COMMAND is incompatible with the script version
   //case OR_SUCCESS:
   //   // [CHECK] Does command GameVersion exceed scriptFile GameVersion?
   //   if (pOutput->pSyntax->eGameVersion > pScriptFile->eGameVersion)
   //   {
   //      // [WARNING] "The script command on line %u is not compatible with %s : '%s'"
   //      pError = generateDualWarning(HERE(IDS_SCRIPT_COMMAND_INCOMPATIBLE), pOutput->iLineNumber, identifyGameVersionString(pScriptFile->eGameVersion), pOutput->szBuffer);
   //      pushCommandAndOutputQueues(pError, pErrorQueue, pOutput->pErrorQueue, ET_WARNING);
   //   }
   //   break;
   //}
   
   // Return translation result
   return pOutput->eResult;
}


/// Function name  : translateCommandNodeParameterNodes
// Description     : Create COMMAND's PARAMETER list from the nodes within a CommandNode
// 
// COMMAND_NODE*  pCommandNode : [in]     CommandNode containing the subnodes
// COMMAND*       pOutput      : [in/out] COMMAND to add the PARAMETERs to
// ERROR_STACK*  &pError       : [out]    New error message if any, otherwise NULL. You are responsible for destroying it
// 
// Return Value   : TRUE if successful, FALSE otherwise
// 
BOOL  translateCommandNodeParameterNodes(CONST COMMAND_NODE*  pCommandNode, COMMAND*  pOutput, ERROR_STACK*  &pError)
{
   PARAMETER_NODE_TYPE    eCurrentNodeType;
   PARAMETER_NODE_TUPLE*  pParameterNodeTuple;  // Node tuple representing the PARAMETER currently being processed
   PARAMETER_COUNT        oParameterCount;      // Holds the parameter counts for expressions/script-calls/cmd-comments
   PARAMETER_SYNTAX       eParameterSyntax;     // Syntax for the parameter currently being processed
   XML_TREE_NODE*         pNodeIterator;        // Node iterator for iterating through a CommandNode's child nodes
   PARAMETER*             pParameter;           // Parameter currently being created
   UINT                   iParameterIndex,      // Zero-based index of the current parameter being processed
                          iInitialIndex;        // Zero-based index of the first parameter child node 
   // Prepare
   utilZeroObject(&oParameterCount, PARAMETER_COUNT);
   pParameterNodeTuple = createParameterNodeTuple();
   pError              = NULL;
   iParameterIndex     = NULL;

   // Determine starting index
   if (isCommandType(pOutput, CT_CMD_COMMENT))
      iInitialIndex = CSTI_FIRST_PARAMETER;
   else
      iInitialIndex = (pCommandNode->eType == CT_STANDARD ? SSTI_FIRST_PARAMETER : ASTI_FIRST_PARAMETER);

   /// Iterate through the relevant child parameter nodes 
   for (findXMLTreeNodeByIndex(pCommandNode->pNode, iInitialIndex, pNodeIterator); pNodeIterator AND !pError; findNextXMLTreeNode(pNodeIterator, XNR_SIBLING, pNodeIterator)) 
   {
      // Prepare
      pParameter = NULL;

      // Determine the syntax for this parameter (For ScriptCalls and Expressions this must be extrapolated)
      if (!calculateParameterSyntaxByIndex(pOutput, iParameterIndex, eParameterSyntax, pError))
         break; // Abort.. No further enhancement necessary

      // Determine how many nodes to read and how to interpret them
      switch (eCurrentNodeType = identifyParameterNodeType(pOutput, &oParameterCount, pNodeIterator->iIndex, iParameterIndex))
      {
      /// [PARAMETER TUPLE] Create a PARAMETER from a ParameterNodeTuple created from the current+next two nodes 
      case PNT_PARAMETER_TUPLE:
      case PNT_POSTFIX_EXPRESSION:
         // Read the current and next nodes into a ParameterNodeTuple object
         if (!findNextParameterNodeTuple(pCommandNode, pNodeIterator, pParameterNodeTuple, pError))
            // [ERROR] "Could not retrieve the next parameter tuple for the current command node"
            enhanceError(pError, ERROR_ID(IDS_SCRIPT_PARAMETER_TUPLE_MISSING));
         
         // Create a new PARAMETER from the ParameterNodeTuple
         else if (!translateParameterTuple(pParameterNodeTuple, eParameterSyntax, pOutput, pParameter, pError))
            // [ERROR] "Cannot translate the '%s' parameter (%u of %u) defined by the <sourcevalue> tag on line %u"
            enhanceError(pError, ERROR_ID(IDS_SCRIPT_PARAMETER_NODE_TRANSLATION_FAILED), identifyParameterSyntaxString(eParameterSyntax), iParameterIndex+1, pOutput->pSyntax->iParameterCount+1, pParameterNodeTuple->pType->iLineNumber);

         else 
         {  // [SUCCESS] Add PARAMETER to appropriate array within COMMAND 
            appendParameterToCommand(pOutput, pParameter, eCurrentNodeType == PNT_POSTFIX_EXPRESSION ? PT_POSTFIX : PT_DEFAULT);
            iParameterIndex++;
         }
         break;

      /// [SINGLE PARAMETER] Create a new PARAMETER from the node and a very specific parameter syntax
      case PNT_SINGLE_PARAMETER:
      case PNT_INFIX_EXPRESSION:
         if (!translateParameterNode(pNodeIterator, eParameterSyntax, pParameter, pError))
            // [ERROR] "Cannot translate the '%s' parameter (%u of %u) defined by the <sourcevalue> tag on line %u"
            enhanceError(pError, ERROR_ID(IDS_SCRIPT_PARAMETER_NODE_TRANSLATION_FAILED), identifyParameterSyntaxString(eParameterSyntax), iParameterIndex+1, pOutput->pSyntax->iParameterCount+1, pNodeIterator->iLineNumber);
         else
         {  // [SUCCESS] Add PARAMETER to appropriate array within COMMAND
            appendParameterToCommand(pOutput, pParameter, eCurrentNodeType == PNT_INFIX_EXPRESSION ? PT_INFIX : PT_DEFAULT);
            iParameterIndex++;
         }
         break;

      /// [PARAMETER COUNT] Extract the ScriptCall/CommandComment parameter count
      case PNT_PARAMETER_COUNT:
         if (!getXMLPropertyInteger(pNodeIterator, TEXT("val"), (INT&)oParameterCount.iCount))
            // [ERROR] "Cannot determine the number of %s parameters from the <sourcevalue> tag on line %u"
            enhanceError(pError, ERROR_ID(IDS_SCRIPT_PARAMETER_COUNT_MISSING), TEXT("command"), pNodeIterator->iLineNumber);
         break;
      /// [INFIX COUNT] Extract the Infix Expression components parameter count
      case PNT_INFIX_COUNT:
         if (!getXMLPropertyInteger(pNodeIterator, TEXT("val"), (INT&)oParameterCount.iInfixCount))
            // [ERROR] "Cannot determine the number of %s parameters from the <sourcevalue> tag on line %u"
            enhanceError(pError, ERROR_ID(IDS_SCRIPT_PARAMETER_COUNT_MISSING), TEXT("infix expression"), pNodeIterator->iLineNumber);
         break;
      /// [POSTFIX COUNT] Extract the Postfix Expression components parameter count
      case PNT_POSTFIX_COUNT:
         if (!getXMLPropertyInteger(pNodeIterator, TEXT("val"), (INT&)oParameterCount.iPostfixCount))
            // [ERROR] "Cannot determine the number of %s parameters from the <sourcevalue> tag on line %u"
            enhanceError(pError, ERROR_ID(IDS_SCRIPT_PARAMETER_COUNT_MISSING), TEXT("postfix expression"), pNodeIterator->iLineNumber);
         break;
      }
   }

   // [EXPRESSIONS] Combine the parameter lists
   if (isCommandID(pOutput, CMD_EXPRESSION) AND !isCommandType(pOutput, CT_CMD_COMMENT))
      translateCommandExpression(pOutput, &oParameterCount, pError);

   // Cleanup and return TRUE if there were no errors
   deleteParameterNodeTuple(pParameterNodeTuple);
   return (pError == NULL);
}


/// Function name  : translateCommandToString
// Description     : Translate a COMMAND to string using the new method of a CommandComponent object
// 
// CONST SCRIPT_FILE*  pScriptFile : [in]     ScriptFile containing the COMMAND
// COMMAND*            pCommand    : [in]     COMMAND object to convert to a string
// HWND                hParentWnd  : [in]     Parent window handle
// ERROR_QUEUE*        pErrorQueue : [in/out] Error queue, may already contain errors
// 
// Return Value   : OR_SUCCESS : The COMMAND was translated successfully, there may have been errors but they were ignored
//                  OR_ABORTED : The COMMAND was partially translated but translation was aborted by the user due to errors
// 
OPERATION_RESULT  translateCommandToString(CONST SCRIPT_FILE*  pScriptFile, COMMAND*  pCommand, HWND  hParentWnd, ERROR_QUEUE*  pErrorQueue)
{
   COMMAND_COMPONENT*  pCommandComponent;       // CommandComponent object for parsing command syntax
   //TCHAR*              szParameter;             // String translation of the Parameter currently being processed
   PARAMETER*          pParameter;              // COMMAND's Parameter list iterator
   SCRIPT_FILE*        pExternalScript;         // [SCRIPT-CALL] Contains the target of a script-call command
   ARGUMENT*           pExternalScriptArgument; // [SCRIPT-CALL] The current argument of a script-call command
   ERROR_QUEUE*        pExternalErrorQueue;     // [SCRIPT-CALL] Errors encountered while translating the target of a script-call commands
   ERROR_STACK*        pError;                  // Error (if any) encountered while translating current parameter
   

   // [NOP COMMAND] No translation necessary
   if (pCommand->iID == CMD_NOP)
      return OR_SUCCESS;

   /// [SPECIAL CASES] Fudge certain commands in various ways
   translateCommandToStringSpecialCases(pScriptFile, pCommand, hParentWnd, pErrorQueue);

   /// [COMPATIBILITY] Is this command incompatible?
   if (!isCommandSyntaxCompatible(pCommand->pSyntax, pScriptFile->eGameVersion))
   {
      // [ERROR] "Incompatible %s command detected on line %u : '%s'"
      pError = generateDualError(HERE(IDS_SCRIPT_COMMAND_INCOMPATIBLE), identifyGameVersionString(pCommand->pSyntax->eGameVersion), pCommand->iLineNumber, pCommand->pSyntax->szSyntax);
      pushCommandAndOutputQueues(pError, pErrorQueue, pCommand->pErrorQueue, ET_ERROR);
   }

   // Prepare
   pCommandComponent = createCommandComponent(pCommand);
   pExternalScript   = NULL;
   pError            = NULL;

   /// [COMMAND COMMENT] Preface command with '*'
   if (isCommandType(pCommand, CT_CMD_COMMENT))
      StringCchCopy(pCommand->szBuffer, LINE_LENGTH, TEXT("* "));

   /// [SCRIPT CALL] Load the properties of the target script into a new ScriptFile
   if (isCommandType(pCommand, CT_SCRIPTCALL))
   {
      // Load the properties into a new ScriptFile with an empty Errorqueue
      loadScriptCallCommandTargetScript(pScriptFile, pCommand, hParentWnd, pExternalScript, (pExternalErrorQueue = createErrorQueue()));

      // Transfer any errors into the COMMAND and output queue
      while (pError = popErrorQueue(pExternalErrorQueue))
         pushCommandAndOutputQueues(pError, pErrorQueue, pCommand->pErrorQueue, pError->eType);    /// [FIX] BUG:1029 'Missing external scripts and any translation errors they created are not displayed in the CodeEdit after loading'

      // Cleanup
      deleteErrorQueue(pExternalErrorQueue);
   }

   /// Iterate through the available command components (stop if user aborts after an error)
   while (pCommand->eResult != OR_ABORTED AND findNextCommandComponent(pCommandComponent))
   {
      // Prepare
      //szParameter = NULL;
      pError      = NULL;

      // Examine component type
      switch (pCommandComponent->eType)
      {
      /// [COMMAND TEXT] Add to output buffer
      case CCT_COMMAND_TEXT:
         StringCchCat(pCommand->szBuffer, LINE_LENGTH, pCommandComponent->szText);
         break;

      /// [PARAMETER INDEX / SCRIPT ARGUMENT] Add specified parameter to output buffer
      case CCT_PARAMETER_INDEX:
      case CCT_SCRIPT_ARGUMENT:
         // Lookup parameter at the specified index in the COMMAND
         if (!findParameterInCommandByIndex(pCommand, PT_DEFAULT, pCommandComponent->iIndex, pParameter))
            // [ERROR] "The Command object does not have a parameter with index %u as indicated by it's syntax"
            pError = generateDualError(HERE(IDS_SCRIPT_COMMAND_PARAMETER_NOT_FOUND), pCommandComponent->iIndex);
         
         // Convert PARAMETER to a string
         else if (!translateParameterToString(pScriptFile, pCommand, pParameter, pError))
            // [ERROR] "Cannot translate the %s parameter (index %u) with a unionised value of 0x%04X:%04x or '%s'"
            enhanceError(pError, ERROR_ID(IDS_TRANSLATION_PARAMETER_FAILED), identifyDataTypeString(pParameter->eType), pCommandComponent->iIndex, pParameter->iValue, pParameter->szValue);
         
         /// [SCRIPT ARGUMENT] Add argument name in front
         else if (pCommandComponent->eType == CCT_SCRIPT_ARGUMENT)
         {
            // Preface parameter with the argument name obtained from the target script
            if (pExternalScript AND findArgumentInScriptFileByIndex(pExternalScript, pCommandComponent->iIndex - 3, pExternalScriptArgument))
               utilStringCchCatf(pCommand->szBuffer, LINE_LENGTH, TEXT(" %s=%s"), pExternalScriptArgument->szName, pParameter->szBuffer);
            else
               // [NOT FOUND] If the target script was not found, has errors, or doesn't have as many arguments
               //               as the caller believes, then preface with a generic argument index
               utilStringCchCatf(pCommand->szBuffer, LINE_LENGTH, TEXT(" argument%u=%s"), pCommandComponent->iIndex - 2, pParameter->szBuffer); // First argument starts at index 3
         }
         /// [PARAMETER] Add appropriate spacing in front and behind
         else switch (pParameter->eType)
         {
         // [OPERATOR] Adjust spacing to display correctly
         case DT_OPERATOR:
            switch (pParameter->iValue)
            {
            // [UNARY] Output alone
            case OT_MINUS:
            case OT_LOGICAL_NOT:
            case OT_BITWISE_NOT:
            case OT_BRACKET_OPEN:   
            case OT_BRACKET_CLOSE:  utilStringCchCatf(pCommand->szBuffer, LINE_LENGTH, TEXT("%s"), pParameter->szBuffer);   break;
            // [BINARY] Output with spaces
            default:                utilStringCchCatf(pCommand->szBuffer, LINE_LENGTH, TEXT(" %s "), pParameter->szBuffer); break;
            }
            break;

         // [NON-OPERATOR] Print without any spaces
         default:
            StringCchCat(pCommand->szBuffer, LINE_LENGTH, pParameter->szBuffer);
            break;
         }

         // Cleanup
         //utilSafeDeleteString(szParameter);
         break;
      } // END: switch 'current component type'

      /// [ERROR] Determine whether to continue...
      if (pError)
      {
         // [ERROR] "Could not translate the '%s' parameter (%u of %u) of the script command '%s' (ID:%03u) on line %u"
         enhanceError(pError, ERROR_ID(IDS_SCRIPT_PARAMETER_TRANSLATION_FAILED), identifyDataTypeString(pParameter ? pParameter->eType : DT_UNKNOWN), pCommandComponent->iIndex, pCommand->pParameterArray->iCount, pCommand->szBuffer, pCommand->iID, pCommand->iLineNumber);
         // Create output attachment and add to output and command error queues
         pushCommandAndOutputQueues(pError, pErrorQueue, pCommand->pErrorQueue, ET_WARNING);
         // [QUESTION] "A minor error was detected while translating the script '%s', would you like to continue processing?"
         //         -> Return SUCCESS for this COMMAND is user chooses to continue. (Otherwise return ABORTED)
         pCommand->eResult = (displayOperationError(hParentWnd, pError, ERROR_ID(IDS_SCRIPT_TRANSLATION_MINOR_ERROR), identifyScriptName(pScriptFile)) == EH_IGNORE ? OR_SUCCESS : OR_ABORTED);
      }
   } // END: while 'more Command Components'

   /// [GAME STRING REFERENCE] Check the specified game string exists
   if (isCommandGameStringDependent(pCommand))
      verifyGameStringDependencies(pCommand, pErrorQueue);

   // Cleanup and return result
   if (pExternalScript)
      deleteScriptFile(pExternalScript);
   deleteCommandComponent(pCommandComponent);
   return pCommand->eResult;
}



/// Function name  : translateCommandToStringSpecialCases
// Description     : Fudge certain COMMANDs in various ways
///                    Adjusts the parameters in commands using '± 1' syntax
// 
// CONST SCRIPT_FILE*  pScriptFile : [in]     ScriptFile containing the COMMAND
// COMMAND*            pCommand    : [in/out] COMMAND object containing a translated COMMAND
// HWND                hParentWnd  : [in]     Parent window handle
// ERROR_QUEUE*        pErrorQueue : [in/out] Error queue, may already contain errors
// 
VOID  translateCommandToStringSpecialCases(CONST SCRIPT_FILE*  pScriptFile, COMMAND*  pCommand, HWND  hParentWnd, ERROR_QUEUE*  pErrorQueue)
{
   PARAMETER*  pParameter;    // Parameter being altered

   // Examine command
   switch (pCommand->iID)
   {
   /// [EXTERNAL] "$0 random value from zero to $1 - 1"
   /// [INTERNAL] "$0 random value from zero to $1"
   //  [ACTION] Subtract 1 from parameter $1
   case CMD_RANDOM_VALUE_ZERO:
      // Lookup target parameter
      if (!findParameterInCommandByIndex(pCommand, PT_DEFAULT, 1, pParameter))
         // [WARNING] "Could not perform special case processing on parameter %u of the command on line %u"
         pushErrorQueue(pErrorQueue, generateDualWarning(HERE(IDS_TRANSLATION_SPECIAL_CASE_PARAMETER_NOT_FOUND), 2, pCommand->iLineNumber));

      // [CHECK] Don't attempt to subtract from constants or variables
      else if (pParameter->eType == DT_INTEGER)
         pParameter->iValue--;
      break;

   /// [EXTERNAL] "$0 random value between $1 and $2 - 1"
   /// [INTERNAL] "$0 random value between $1 and $2"
   //  [ACTION] Subtract 1 from parameter $2
   case CMD_RANDOM_VALUE:
      // Lookup target parameter
      if (!findParameterInCommandByIndex(pCommand, PT_DEFAULT, 2, pParameter))
         // [WARNING] "Could not perform special case processing on parameter %u of the command on line %u"
         pushErrorQueue(pErrorQueue, generateDualWarning(HERE(IDS_TRANSLATION_SPECIAL_CASE_PARAMETER_NOT_FOUND), 3, pCommand->iLineNumber));

      // [CHECK] Don't attempt to subtract from constants or variables
      else if (pParameter->eType == DT_INTEGER)
         pParameter->iValue--;
      break;

   /// [EXTERNAL] "$0 get index of $2 in array $1 offset=$3 + 1"
   /// [INTERNAL] "$0 get index of $2 in array $1 offset=$3"
   //  [ACTION] Add 1 to parameter $3
   case CMD_GET_INDEX_OF_ARRAY:
      // Lookup target parameter
      if (!findParameterInCommandByIndex(pCommand, PT_DEFAULT, 3, pParameter))
         // [WARNING] "Could not perform special case processing on parameter %u of the command on line %u"
         pushErrorQueue(pErrorQueue, generateDualWarning(HERE(IDS_TRANSLATION_SPECIAL_CASE_PARAMETER_NOT_FOUND), 4, pCommand->iLineNumber));

      // [CHECK] Don't attempt to increment constants or variables
      else if (pParameter->eType == DT_INTEGER)
         pParameter->iValue++;
      break;

   /// [DELAYED COMMAND] Warn user this command may have hidden parameter
   case CMD_DELAYED_COMMAND:
      // [WARNING] "The 'delayed command' script command on line %u may have a hidden parameter that has been deleted"
      pushCommandAndOutputQueues(generateDualWarning(HERE(IDS_TRANSLATION_DELAYED_COMMAND_DETECTED), pCommand->iLineNumber), pErrorQueue, pCommand->pErrorQueue, ET_WARNING);
      break;
   
   /// [SET WING COMMAND] Warn user this command may have two hidden parameters
   case CMD_SET_WING_COMMAND:
      // [WARNING] "The 'set wing command' script command on line %u may have two hidden parameters that have been deleted"
      pushCommandAndOutputQueues(generateDualWarning(HERE(IDS_TRANSLATION_SET_WING_COMMAND_DETECTED), pCommand->iLineNumber), pErrorQueue, pCommand->pErrorQueue, ET_WARNING);
      break;
   }
}


/// Function name  : translateParameterNode
// Description     : Creates a new PARAMETER from an XMLTreeNode and a parameter syntax entry
// 
// CONST XML_TREE_NODE*    pNode   : [in]  Node to create
// CONST PARAMETER_SYNTAX  eSyntax : [in]  Syntax to use to interpret the node
// PARAMETER*             &pOutput : [out] New PARAMETER if successful, otherwise NULL. You are responsible for destroying it
// ERROR_STACK*           &pError  : [out] New Error is unsuccessful, otherwise NULL. You are responsible for destroying it
// 
// Return Value   : TRUE if successful, FALSE otherwise
// 
BOOL  translateParameterNode(CONST XML_TREE_NODE*  pNode, CONST PARAMETER_SYNTAX  eSyntax, PARAMETER*  &pOutput, ERROR_STACK*  &pError)
{
   SOURCE_VALUE_TYPE eType;         // Type of the value contained within the node
   CONST TCHAR*      szValue;       // [STRING VALUES] Node value
   TCHAR*            szSafeValue;   // [STRING VALUES] Node value converted to an internal string
   INT               iValue;        // [INTEGER VALUES] Node value
   
   // Prepare
   pOutput = NULL;

   switch (eSyntax)
   {
   /// [INFIX EXPRESSION PARAMETER] Create a DT_EXPRESSION or DT_OPERATOR parameter
   case PS_EXPRESSION:  
      // Extract integer, determine from sign whether an operator or postfix parameter index, create parameter
      if (getXMLPropertyInteger(pNode, TEXT("val"), iValue))
      {
         if (iValue < 0)
            // [EXPRESSION] Store the negative value
            pOutput = createParameterFromInteger(iValue, DT_EXPRESSION, eSyntax, pNode->iLineNumber);
         else
            // [OPERATOR] Store the operator ID without the unary bit flag stored in the HO word
            pOutput = createParameterFromInteger(LOWORD(iValue), DT_OPERATOR, eSyntax, pNode->iLineNumber);
      }
      else
      {  // [ERROR] "Cannot read the integer value from the parameter tuple defined by the <sourcevalue> tag on line %u"
         pError = generateDualError(HERE(IDS_SCRIPT_PARAMETER_NODE_INTEGER_INVALID), pNode->iLineNumber);
      }
      break;

   /// [RETURN OBJECT] Create a DT_VARIABLE or DT_STRING parameter containing the ReturnObject, VariableID or
   ///                  Variable name depending on whether the node is a command comment or not
   case PS_VARIABLE:  // Used by inc/dec command
   case PS_LABEL_NUMBER:
   case PS_RETURN_OBJECT:
   case PS_RETURN_OBJECT_IF:
   case PS_RETURN_OBJECT_IF_START:
   case PS_INTERRUPT_RETURN_OBJECT_IF:
      // Determine type
      if (!getSourceValueType(pNode, eType, pError) OR eType == SVT_ARRAY)
      {
         // [ERROR] "Unsupported or unrecognised <sourcevalue> tag type detected in the parameter node defined on line %u"
         pError = generateDualError(HERE(IDS_SCRIPT_PARAMETER_NODE_TYPE_INVALID), pNode->iLineNumber);
      }
      else switch (eType)
      {
      /// [VARIABLE NAME] Create a DT_STRING parameter containing the variable name
      case SVT_STRING:
         if (getSourceValueString(pNode, szValue, pError))
         {
            // Convert string to internal and create PARAMETER
            pOutput = createParameterFromString(generateConvertedString(szValue, SPC_SCRIPT_EXTERNAL_TO_DISPLAY, szSafeValue) ? szSafeValue : szValue, eSyntax, pNode->iLineNumber);
            utilSafeDeleteString(szSafeValue);
         }
         else
         {  // [ERROR] "Cannot read the string value from the parameter node defined by the <sourcevalue> tag on line %u"
            pError = generateDualError(HERE(IDS_SCRIPT_PARAMETER_NODE_STRING_INVALID), pNode->iLineNumber);
         }
         break;

      /// [RETURN OBJECT or VARIABLE ID] Create a DT_VARIABLE parameter containing the integer
      /// [LABEL NUMBER] Create a DT_INTEGER parameter containing the target index
      case SVT_INTEGER:
         if (getSourceValueInteger(pNode, iValue, pError))
            pOutput = createParameterFromInteger(iValue, eSyntax != PS_LABEL_NUMBER ? DT_VARIABLE : DT_INTEGER, eSyntax, pNode->iLineNumber);
         else
         {  // [ERROR] "Cannot read the integer value from the parameter tuple defined by the <sourcevalue> tag on line %u"
            pError = generateDualError(HERE(IDS_SCRIPT_PARAMETER_NODE_INTEGER_INVALID), pNode->iLineNumber);
         }
         break;
      }
      break;

   /// [COMMENT/SCRIPTNAME] Create a DT_STRING parameter
   case PS_COMMENT:
   case PS_SCRIPT_NAME:
   case PS_LABEL_NAME:
      // Extract string and create DT_STRING parameter
      if (getXMLPropertyString(pNode, TEXT("val"), szValue))
      {
         // Convert string to internal and create PARAMETER
         pOutput = createParameterFromString(generateConvertedString(szValue, SPC_SCRIPT_EXTERNAL_TO_DISPLAY, szSafeValue) ? szSafeValue : szValue, eSyntax, pNode->iLineNumber);
         utilSafeDeleteString(szSafeValue);
      }
      else
      {  // [ERROR] "Cannot read the string value from the parameter defined by the <sourcevalue> tag on line %u"
         pError = generateDualError(HERE(IDS_SCRIPT_PARAMETER_NODE_STRING_INVALID), pNode->iLineNumber);
      }
      break;

   // [ERROR]
   default:
      ASSERT(FALSE);
      break;
   }
   
   // Return TRUE if parameter was created
   return (pOutput != NULL);
}


/// Function name  : translateParameterTuple
// Description     : Creates a new PARAMETER from a ParameterNodeTuple
// 
// CONST PARAMETER_NODE_TUPLE*  pParameterNodeTuple : [in]  Node Tuple to create PARAMETER from
// CONST PARAMETER_SYNTAX       eSyntax             : [in]  Syntax of the PARAMETER
// CONST COMMAND*               pCommand            : [in]  COMMAND currently being processed
// PARAMETER*                  &pParameter          : [out] New PARAMETER if successful, otherwise NULL. You are responsible for destroying it
// ERROR_STACK*                &pError              : [out] New error message if unsuccessful, otherwise NULL. You are responsible for destroying it
// 
// Return Value   : TRUE if successful, FALSE otherwise
// 
BOOL  translateParameterTuple(CONST PARAMETER_NODE_TUPLE*  pNodeTuple, CONST PARAMETER_SYNTAX  eSyntax, CONST COMMAND*  pCommand, PARAMETER*  &pOutput, ERROR_STACK*  &pError)
{
   DATA_TYPE    eType;         // Data type of parameter being created
   CONST TCHAR* szValue;       // Parameter value (as string)
   TCHAR*       szSafeValue;   // Parameter value (as string) converted to an internal string
   INT          iValue;        // Parameter value (as integer)

   /// Read data type
   if (!getXMLPropertyInteger(pNodeTuple->pType, TEXT("val"), iValue) OR !performScriptTypeConversion(NULL, (DATA_TYPE)iValue, eType, SVT_EXTERNAL, SVT_INTERNAL, pError))
      // [ERROR] "The data type defined by the parameter <sourcevalue> tag on line %u is an unrecognised or unsupported value of '%d'"
      pError = generateSafeDualError(pError, HERE(IDS_SCRIPT_PARAMETER_TUPLE_TYPE_INVALID), pNodeTuple->pType->iLineNumber, LOWORD(iValue));
   
   // Examine data type
   else switch (eType)
   {
   /// [STRING] Create PARAMETER from string value
   case DT_STRING:
      if (getXMLPropertyString(pNodeTuple->pValue, TEXT("val"), szValue))
      {
         // Convert string to INTERNAL and create a PARAMETER
         pOutput = createParameterFromString(generateConvertedString(szValue, SPC_SCRIPT_EXTERNAL_TO_DISPLAY, szSafeValue) ? szSafeValue : szValue, eSyntax, pNodeTuple->pValue->iLineNumber);
         utilSafeDeleteString(szSafeValue);
      }
      else
         // [ERROR] "Cannot read the string value from the parameter tuple defined by the <sourcevalue> tag on line %u"
         pError = generateDualError(HERE(IDS_SCRIPT_PARAMETER_TUPLE_STRING_INVALID), pNodeTuple->pValue->iLineNumber);
      break;

   /// [VARIABLE] Create PARAMETER from either a string or integer value
   case DT_VARIABLE:
      // [STANDARD] Create PARAMETER containing variable ID
      if (!isCommandType(pCommand, CT_CMD_COMMENT) AND getXMLPropertyInteger(pNodeTuple->pValue, TEXT("val"), iValue))
         pOutput = createParameterFromInteger(iValue, DT_VARIABLE, eSyntax, pNodeTuple->pValue->iLineNumber);
         
      // [COMMAND COMMENT] Create PARAMETER containing variable name
      else if (isCommandType(pCommand, CT_CMD_COMMENT) AND getXMLPropertyString(pNodeTuple->pValue, TEXT("val"), szValue))
         // Create parameter of type DT_VARIABLE but with a string
         pOutput = createParameterFromVariable(szValue, eSyntax, pNodeTuple->pValue->iLineNumber);  // STRINGS with a syntax of VARIABLE are printed as such in translateParameterToString()

      else
         // [ERROR] "Cannot read the string value from the parameter tuple defined by the <sourcevalue> tag on line %u"
         pError = generateDualError(HERE(IDS_SCRIPT_PARAMETER_TUPLE_STRING_INVALID), pNodeTuple->pValue->iLineNumber);
      break;

   /// [DEFAULT] Create PARAMETER from integer value
   default:
      if (getXMLPropertyInteger(pNodeTuple->pValue, TEXT("val"), iValue))
         pOutput = createParameterFromInteger(iValue, eType, eSyntax, pNodeTuple->pValue->iLineNumber);

      else
         // [ERROR] "Cannot read the integer value from the parameter tuple defined by the <sourcevalue> tag on line %u"
         pError = generateDualError(HERE(IDS_SCRIPT_PARAMETER_TUPLE_INTEGER_INVALID), pNodeTuple->pValue->iLineNumber);
      break;
   }
   
   // Return TRUE if there were no errors
   return (pError == NULL);
}


/// Function name  : translateParameterToString
// Description     : Generates the string version of the input PARAMETER
// 
// SCRIPT_FILE*      pScriptFile     : [in]     ScriptFile containing the COMMAND that contains the PARAMETER
// CONST PARAMETER*  pParameter      : [in/out] Parameter to convert into a string.
///                                               -> If this contains a variable ID, it is resolved and stored in the PARAMETER, for use by the CodeEdit VariableTree generator
// COMMAND*          pCommand        : [in/out] COMMAND containing the PARAMETER to convert
// TCHAR*           &szOutput        : [out]    New string representing the parameter
// ERROR_STACK*     &pError          : [out]    New error message, if any, otherwise NULL
// 
// Return Value   : TRUE if successful, FALSE otherwise
// 
BOOL  translateParameterToString(CONST SCRIPT_FILE*  pScriptFile, COMMAND*  pCommand, PARAMETER*  pParameter, ERROR_STACK*  &pError)
{
   VARIABLE_NAME  *pVariableName;     // VariableName lookup
   GAME_OBJECT    *pGameObject;       // GameObject lookup
   GAME_STRING    *pGameString;       // GameString lookup
   CONST TCHAR    *szReturnObject;    // ReturnObject text
   RETURN_OBJECT  *pReturnObject;     // Used for decoding the ReturnObject
   SCRIPT_OBJECT  *pScriptObject;     // ScriptObject lookup
   UINT            iStringID;         // ID of the GameString representing the parameter value

   // Prepare
   //szOutput = utilCreateEmptyString(COMPONENT_LENGTH);
   pError   = NULL;

   // Examine syntax
   switch (pParameter->eSyntax)
   {
   /// [RETURN-OBJECT] Convert to a variable name or conditional appropriately
   case PS_RETURN_OBJECT:
   case PS_RETURN_OBJECT_IF:
   case PS_RETURN_OBJECT_IF_START:
   case PS_INTERRUPT_RETURN_OBJECT_IF:
      /// [RETURN VARIABLE NAME] Output with '=' on the end
      if (pParameter->eType == DT_STRING OR pParameter->szValue != NULL)
         pParameter->szBuffer = utilCreateStringf(lstrlen(pParameter->szValue) + 8, TEXT("$%s ="), pParameter->szValue);
      else
      {
         // Create ReturnObject
         pReturnObject = createReturnObjectFromInteger(pParameter->iValue);

         // [SPECIAL CASE] No variables and return object is zero, assume 'discard'
         if (!pParameter->iValue AND !getScriptTranslatorVariablesCount(pScriptFile->pTranslator))
            pParameter->szBuffer = utilDuplicateSimpleString(TEXT("="));       /// [VALIDATION_FIX] 

         // Convert to string
         else if (!translateReturnObjectToString(pScriptFile, pReturnObject, szReturnObject, pError))
         {
            // [ERROR] "Could not translate the ReturnObject variable with ID %u"
            if (pReturnObject->eType == ROT_VARIABLE)
               enhanceError(pError, ERROR_ID(IDS_RETURN_OBJECT_VARIABLE_TRANSLATION_FAILED), pReturnObject->iVariableID);
            else  // [ERROR] "Could not translate the conditional ReturnObject with conditional %u:%u and destination %u"
               enhanceError(pError, ERROR_ID(IDS_RETURN_OBJECT_CONDITIONAL_TRANSLATION_FAILED), pReturnObject->eReturnType, pReturnObject->eConditional, pReturnObject->iDestination);
         }
         /// [RETURN VARIABLES] Output with an '=' on the end
         else if (pReturnObject->eType == ROT_VARIABLE)
         {
            // Store variable and generate RetVar syntax
            pParameter->szBuffer = utilCreateStringf(lstrlen(szReturnObject) + 8, TEXT("$%s ="), szReturnObject);
            pParameter->szValue = utilDuplicateSimpleString(szReturnObject);
         }
         /// [CONDITIONALS] Output alone and save conditonal in COMMAND
         else
         {
            // Store without trailing spaces
            pParameter->szBuffer = utilDuplicateSimpleString(szReturnObject);
            StrTrim(pParameter->szBuffer, TEXT(" ")); 
            // Save conditional in COMMAND
            pCommand->eConditional = pReturnObject->eConditional;
         }
         
         // Cleanup
         deleteReturnObject(pReturnObject);
      }
      break;

   /// [NORMAL PARAMETER] Convert to a string and display appropriately
   default:
      switch (pParameter->eType)
      {
      /// [NULL/INTEGER] -- Easy conversion
      case DT_NULL:    pParameter->szBuffer = utilDuplicateSimpleString(!isReferenceObject(pParameter) ? TEXT("null") : TEXT("null->"));                 break;
      case DT_INTEGER: pParameter->szBuffer = utilCreateStringf(32, TEXT("%d"), pParameter->iValue);   break;

      /// [STRING] -- Enclose in appropriate apostrophes
      case DT_STRING:
         switch (pParameter->eSyntax)
         {
         // [COMMENT or LABEL NAME] Output alone
         case PS_COMMENT:
         case PS_LABEL_NAME: pParameter->szBuffer = utilDuplicateSimpleString(pParameter->szValue);                break;
         // [VARIABLE NAME] Preface with '$'
         case PS_VARIABLE:   pParameter->szBuffer = utilCreateStringf(lstrlen(pParameter->szValue) + 2, TEXT("$%s"), pParameter->szValue);   break;
         // [STRING] Output in apostrophes
         default:            pParameter->szBuffer = utilCreateStringf(lstrlen(pParameter->szValue) + 4, TEXT("'%s'"), pParameter->szValue);  break;
         }
         break;

      /// [SHIP/STATION] -- Error
      case DT_SHIP:     TODO("Unable to translate parameter types defined as DT_SHIP");      break;
      case DT_STATION:  TODO("Unable to translate parameter types defined as DT_STATION");   break;

      /// [WARE] -- Lookup ObjectName and enclose in appropriate brackets
      case DT_WARE:
         if (!findGameObjectByWare(pParameter->iValue, pGameObject))
         {  
            // [ERROR] "Unrecognised or invalid ship/station/ware defined by MainType %u and SubType %u"
            pError = generateDualError(HERE(IDS_TRANSLATION_WARE_INVALID), HIWORD(pParameter->iValue), LOWORD(pParameter->iValue));

            // [CHECK] Attempt to produce encoded ware
            if (findMainTypeGameStringByID((MAIN_TYPE)HIWORD(pParameter->iValue), pGameString))
               // [SUCCESS] Generate 'encoded' placeholder
               pParameter->szBuffer = utilCreateStringf(lstrlen(pGameString->szText) + 16, TEXT("{%s@%u}"), pGameString->szText, LOWORD(pParameter->iValue));
            else
               // [FAILED] Insert Placeholder
               pParameter->szBuffer = utilDuplicateSimpleString(TEXT("<Unrecognised Ware>"));
         }
         else
            // [SUCCESS] Insert GameObject
            pParameter->szBuffer = utilCreateStringf(lstrlen(pGameObject->szText) + 4, TEXT("{%s}"), pGameObject->szText);
         break;

      /// [VARIABLE ID] -- Lookup name in <codearray> and preface with dollar sign
      case DT_VARIABLE:
         // [COMMAND COMMENT] Variables are already strings
         if (pParameter->szValue)
            pParameter->szBuffer = utilCreateStringf(lstrlen(pParameter->szValue) + 8, !isReferenceObject(pParameter) ? TEXT("$%s") : TEXT("$%s->"), pParameter->szValue);

         // Lookup variable ID in the Translator and display appropriately
         else if (!findVariableNameInTranslatorByIndex(pScriptFile->pTranslator, pParameter->iValue, pVariableName))
         {  
            // [FAILED] "Cannot find a matching variable with ID %u"
            pError = generateDualError(HERE(IDS_TRANSLATION_VARIABLE_INVALID), pParameter->iValue);
            pParameter->szBuffer = utilDuplicateSimpleString(TEXT("<Unrecognised Variable>"));
         }
         else
         {
            // [SUCCESS] Store variable name and generate display text
            pParameter->szValue  = utilDuplicateSimpleString(pVariableName->szName);
            pParameter->szBuffer = utilCreateStringf(lstrlen(pVariableName->szName) + 8, !isReferenceObject(pParameter) ? TEXT("$%s") : TEXT("$%s->"), pVariableName->szName);
         }
         break;

      /// [REAMINDER] -- Convert external value to internal, find matching GameString and display appropriately
      default:
         // Convert script values from external to internal
         if (!performScriptValueConversion(NULL, pParameter, (INT&)iStringID, SVT_EXTERNAL, SVT_INTERNAL, pError))
         {
            // [FAILED] "Cannot convert the external %s parameter value %d to an internal parameter value"
            enhanceError(pError, ERROR_ID(IDS_TRANSLATION_TYPE_CONVERSION_FAILED), identifyDataTypeString(pParameter->eType), pParameter->iValue);
            pParameter->szBuffer = utilDuplicateSimpleString(TEXT("<Unrecognised Parameter>")); 
         }
         // [CHECK] Lookup matching GameString
         else if (!findGameStringByID(iStringID, identifyGamePageIDFromDataType(pParameter->eType), pGameString))
         {  
            // [FAILED] "Cannot find a matching game string with ID %u for the %s parameter with value %u:%u"
            pError = generateDualError(HERE(IDS_TRANSLATION_PARAMETER_INVALID), iStringID, identifyDataTypeString(pParameter->eType), HIWORD(pParameter->iValue), LOWORD(pParameter->iValue));
            pParameter->szBuffer = utilDuplicateSimpleString(TEXT("<Unrecognised Parameter>")); 
         }
         // [OPERATOR] These have no ScriptObjects
         else if (pParameter->eType == DT_OPERATOR) 
            // Convert from INTERNAL->DISPLAY
            replaceStringConvert(pParameter->szBuffer, SPC_LANGUAGE_INTERNAL_TO_DISPLAY, pGameString->szText);
         else
         {
            // Lookup ScriptObject
            findScriptObjectByGroup(identifyObjectNameGroupFromGameString(pGameString), pGameString->iID, pScriptObject);
            ASSERT(pScriptObject != NULL);

            // Display within brackets
            pParameter->szBuffer = utilCreateStringf(lstrlen(pScriptObject->szText) + 8, !isReferenceObject(pParameter) ? TEXT("[%s]") : TEXT("[%s]->"), pScriptObject->szText);
         }
         break;
      }
   }

   //// [REFERENCE OBJECT] Append Reference operator
   //if (pParameter->eSyntax == PS_REFERENCE_OBJECT AND !pError)
   //   StringCchCat(szOutput, COMPONENT_LENGTH, TEXT("->"));
   
   //return (pError == NULL);
   return TRUE;
}


/// Function name  : translateReturnObjectToString
// Description     : Convert a ReturnObject to a string
// 
// SCRIPT_FILE*          pScriptFile   : [in]  ScriptFile containing the COMMAND that contains the ReturnObject
// CONST RETURN_OBJECT*  pReturnObject : [in]  ReturnObject to convert into a string
// CONST TCHAR*         &szOutput      : [out] Const string containing the variable name / conditional
// ERROR_STACK*         &pError        : [out] New error message, if any, otherwise NULL. You are responsible for destroying it
// 
// Return Value   : TRUE if successful, FALSE otherwise
// 
BOOL  translateReturnObjectToString(CONST SCRIPT_FILE*  pScriptFile, CONST RETURN_OBJECT*  pReturnObject, CONST TCHAR*  &szOutput, ERROR_STACK*  &pError)
{
   VARIABLE_NAME*  pVariableName;   // Self explanatory
   GAME_STRING*    pConditional;    // GameString containing the conditional (if any)

   // Prepare
   szOutput = NULL;

   switch (pReturnObject->eType)
   {
   /// [CONDITIONAL] Lookup conditional string
   case ROT_CONDITIONAL:
      // [SKIP-IF] Alter depending on preferences
      if (pReturnObject->eConditional == CI_SKIP_IF_NOT)
         szOutput = (getAppPreferences()->bUseDoIfSyntax ? TEXT("do if") : TEXT("skip if not"));

      // [REMAINDER] Lookup conditional
      else if (findGameStringByID(pReturnObject->eConditional, GPI_CONDITIONALS, pConditional))
         // [SUCCESS] Return conditional text
         szOutput = pConditional->szText;
      else
         // [ERROR] "Cannot find the conditional branching operation defined by the ID '%u'"
         pError = generateDualError(HERE(IDS_RETURN_OBJECT_CONDITIONAL_INVALID), pReturnObject->eConditional);
      break;

   /// [VARIABLE] Lookup variable name
   case ROT_VARIABLE:
      if (findVariableNameInTranslatorByIndex(pScriptFile->pTranslator, pReturnObject->iVariableID, pVariableName))
         // [SUCCESS] Return variable name
         szOutput = pVariableName->szName;
      else
         // [ERROR] "Cannot find the variable with ID %u in the <codearray> variables branch (which contains %u variable names)"
         pError = generateDualError(HERE(IDS_RETURN_OBJECT_VARIABLE_INVALID), pReturnObject->iVariableID, getXMLNodeCount(pScriptFile->pTranslator->oLayout.pVariableNamesBranch));
      break;
   }

   // Return TRUE if there were no errors
   return (pError == NULL);
}


/// Function name  : translateScript
// Description     : Translates a MSCI script into a SCRIPT-FILE object
// 
// SCRIPT_FILE*         pScriptFile     : [in]               ScriptFile with an input buffer full of MSCI XML
// CONST BOOL           bJustProperties : [in]               Whether to translate the entire script or just extract the properties
// HWND                 hParentWnd      : [in]               Parent window used to display error messages
// OPERATION_PROGRESS*  pProgress       : [in/out][optional] Operation progress object, if feedback is required
// ERROR_QUEUE*         pErrorQueue     : [in/out]           Error messages detected during translation, if any
// 
// Return Value   : OR_SUCCESS : The script and as many commands as possible were translated successfully. There may have been errors but they were ignored by the user.
//                  OR_FAILURE : The script could not be translated due to critical errors in the structure or script properties
//                  OR_ABORTED : The script was partially translated but the user aborted after minor errors
// 
OPERATION_RESULT  translateScript(SCRIPT_FILE*  pScriptFile, CONST BOOL  bJustProperties, HWND  hParentWnd, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue)
{
   OPERATION_RESULT   eResult;        // Operation result
   ERROR_STACK*       pError;         // Operation error
   
   // [TRACKING]
   TRACK_FUNCTION();
   VERBOSE_THREAD_COMMAND();

   // Prepare
   eResult = OR_SUCCESS;
   pError  = NULL;

   // [INFO] "Translating XML from file '%s' into an MSCI script"
   VERBOSE("Translating script file '%s' (JustProperties=%s)", identifyScriptName(pScriptFile), bJustProperties ? TEXT("True") : TEXT("False"));
   if (!bJustProperties)
      pushErrorQueue(pErrorQueue, generateDualInformation(HERE(IDS_OUTPUT_PARSING_SCRIPT_XML), identifyScriptName(pScriptFile)));

   /// [XML] Build XML Tree and locate the key nodes
   if (!translateXMLToScriptFile(pScriptFile, hParentWnd, pProgress, pErrorQueue))
      // [FAILED] Return FAILURE
      eResult = OR_FAILURE;
      
   // [SUCCESS]
   else 
   {
      // [INFO] "Translating properties from MSCI script '%s'"
      VERBOSE("Translating script file properties '%s'", identifyScriptName(pScriptFile));
      if (!bJustProperties)
         pushErrorQueue(pErrorQueue, generateDualInformation(HERE(IDS_OUTPUT_TRANSLATING_SCRIPT_PROPERTIES), identifyScriptName(pScriptFile)));

      /// [PROPERTIES] Extract properties from the XML-Tree, populate the ArgumentsTree and VariablesList
      if (!translateScriptFileProperties(pScriptFile, pErrorQueue))
      {
         // [ERROR] "One or more of the mandatory extended script properties are missing or invalid"
         enhanceLastError(pErrorQueue, ERROR_ID(IDS_SCRIPT_PROPERTY_MISSING));
         generateOutputTextFromLastError(pErrorQueue);
         
         // [FAILED] Return FAILURE
         eResult = OR_FAILURE;
      }
      /// [COMMANDS]
      else if (eResult = translateScriptFile(pScriptFile, bJustProperties, hParentWnd, pProgress, pErrorQueue))
      {
         // [ERROR] "The MSCI script '%s' contains errors that have prevented translation"
         enhanceLastError(pErrorQueue, ERROR_ID(IDS_SCRIPT_TRANSLATION_FAILED), identifyScriptName(pScriptFile));
         generateOutputTextFromLastError(pErrorQueue);
      }
   }
   
   // Cleanup and return result
   END_TRACKING();
   return  eResult;
} 


/// Function name  : translateScriptFile
// Description     : Translates a ScriptFile containing just XML the XML in a ScriptFile input buffer into a COMMAND list and script properties
// 
// SCRIPT_FILE*         pScriptFile     : [in]               ScriptFile with an input buffer full of MSCI XML
// CONST BOOL           bJustProperties : [in]               Whether to translate the entire script or just extract the properties
// HWND                 hParentWnd      : [in]               Parent window used to display error messages
// OPERATION_PROGRESS*  pProgress       : [in/out][optional] Operation progress object, if feedback is required
// ERROR_QUEUE*         pErrorQueue     : [in/out]           Error messages detected during translation, if any
// 
// Return Value   : OR_SUCCESS : As many commands as possible were translated successfully. There may have been errors but they were ignored by the user.
//                  OR_FAILURE : Not all commands were translated due to corrupted node structure
//                  OR_ABORTED : Some commands were partially translated but the user aborted due to minor errors
// 
OPERATION_RESULT  translateScriptFile(SCRIPT_FILE*  pScriptFile, CONST BOOL  bJustProperties, HWND  hParentWnd, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue)
{
   OPERATION_RESULT   eResult;        // Operation result
   COMMAND_NODE*      pCommandNode;   // 'Command node' currently being processed
   COMMAND*           pCommand;       // COMMAND generated from the current 'Command node'
   ERROR_STACK*       pError;         // Operation error, caused by structural errors in the XML
   
   // Prepare
   eResult = OR_SUCCESS;

   // [INFO] "Translating commands from %s script '%s'"
   VERBOSE("Verifying script meta-data of '%s'", identifyScriptName(pScriptFile));
   if (!bJustProperties)
      pushErrorQueue(pErrorQueue, generateDualInformation(HERE(IDS_OUTPUT_TRANSLATING_SCRIPT_COMMANDS), identifyGameVersionString(pScriptFile->eGameVersion), identifyScriptName(pScriptFile)));

   // Generate a warning if EngineVersion is un-supported
   verifyScriptEngineVersion(pScriptFile, pErrorQueue);

   // Ensure ScriptName matches Filename, and generate a warning if not
   verifyScriptFileScriptNameFromPath(pScriptFile, pErrorQueue);

   // [PROGRESS] Set new stage and define the maximum as the total number of commands
   if (pProgress)
   {
      ASSERT(advanceOperationProgressStage(pProgress) == IDS_PROGRESS_TRANSLATING_SCRIPT);
      updateOperationProgressMaximum(pProgress, getXMLNodeCount(pScriptFile->pTranslator->oLayout.pStandardCommandsBranch) + getXMLNodeCount(pScriptFile->pTranslator->oLayout.pAuxiliaryCommandsBranch));
   }

   /// Process Standard and Auxiliary commands
   if (!bJustProperties)
   {
      // [VERBOSE]
      VERBOSE("Translating standard and auxiliary command branches of '%s'", identifyScriptName(pScriptFile));

      // Iterate through the standard and auxiliary commands in their proper order
      while (findNextCommandNode(pScriptFile->pTranslator, pCommandNode, pError) AND eResult != OR_ABORTED)
      {
         /// Attempt to translate XML node into a COMMAND
         switch (eResult = translateCommandNode(pScriptFile, pCommandNode, hParentWnd, pCommand, pErrorQueue))
         {
         // [SUCCESS or FAILURE] Add COMMAND to the output queue
         case OR_SUCCESS:
         case OR_FAILURE:
            /// [NORMAL] Add to TRANSLATED OUTPUT
            if (!isCommandID(pCommand, CMD_HIDDEN_JUMP))
               appendCommandToTranslator(pScriptFile->pTranslator, pCommand);
            else
               // [HIDDEN JUMP] Do not add these to the COMMAND list
               deleteCommand(pCommand);
            break;
         }

         // [PROGRESS] Update progress object
         if (pProgress)
            advanceOperationProgressValue(pProgress);
         
         // Cleanup
         deleteCommandNode(pCommandNode, FALSE);
      }

      // An error here indicates findNextCommandNode(..) returned an error
      if (pError)
      {  
         // [ERROR] "There was an error retrieving the next command node from the <codearray> around line %u"
         enhanceError(pError, ERROR_ID(IDS_SCRIPT_COMMAND_STRUCTURE_INVALID), pCommandNode AND pCommandNode->pNode ? pCommandNode->pNode->iLineNumber : NULL);
         generateOutputTextFromError(pError);
         pushErrorQueue(pErrorQueue, pError);
         // Return FAILURE
         eResult = OR_FAILURE;
      }
      /// [SUCCESS] Convert appropriate translated commands into macro commands
      else if (getAppPreferences()->bScriptCodeMacros)
         eResult = translateScriptFileMacros(pScriptFile, hParentWnd, pErrorQueue);
   }

   // Return result
   return eResult;
}


VOID   translateReadCustomMenuItemMacro(SCRIPT_FILE*  pScriptFile, CONST COMMAND*  pCommand, CONST UINT  iIndex, ERROR_QUEUE*  pErrorQueue)
{
   CONST TCHAR  *szMenuVariable,          // Custom Menu array
                *szTextVariable;          // Item text variable
   TCHAR        *szCommandText;
   PARAMETER    *pPageID,                 // Page ID parameter
                *pStringID,               // String ID parameter
                *pItemID;                 // Item ID parameter
   COMMAND      *pSubCommand;             // Custom menu command

   // [CHECK] Lookup return variable, pageID and stringID    '$text = read text: page=$Page.ID id=216'
   if (isCommandReadText(pCommand, szTextVariable, pPageID, pStringID) AND findCommandInTranslatorOutput(pScriptFile->pTranslator, iIndex + 1, pSubCommand)) 
   {
      // [ADD CUSTOM MENU ITEM] Lookup array variable and return value:  'add custom menu item to array $0: text=$1 returnvalue=$2'
      if (isCommandAddCustomMenuItem(pSubCommand, szTextVariable, szMenuVariable, pItemID))
      {
         /// Generate command text:  'add custom menu item to array $0: page=$1 id=$2 returnvalue=$3'
         szCommandText = utilCreateStringf(LINE_LENGTH, TEXT("add custom menu item to array $%s: page=%s id=%s returnvalue=%s"), szMenuVariable, pPageID->szBuffer, pStringID->szBuffer, pItemID->szBuffer);
         
         /// Replace 'read text' and 'add custom menu item' commands with a macro command
         destroyCommandsInTranslatorOutputByIndex(pScriptFile->pTranslator, iIndex, 2);
         insertVirtualCommandToTranslator(pScriptFile->pTranslator, iIndex, createCustomCommandf(CMD_ADD_MENU_ITEM_BYREF, CT_STANDARD WITH CT_VIRTUAL, iIndex, szCommandText));

         // Cleanup
         utilDeleteString(szCommandText);
      }
      // [ADD CUSTOM MENU INFO/HEADING] Lookup array variable:  'add custom menu info/heading to array $0: text=$1'
      else if (isCommandAddCustomMenuInfoHeading(pSubCommand, szTextVariable, szMenuVariable))
      {
         /// Generate command text:  'add custom menu info/heading to array $0: page=$1 id=$2'
         szCommandText = utilCreateStringf(LINE_LENGTH, TEXT("add custom menu %s to array $%s: page=%s id=%s"), isCommandID(pSubCommand, CMD_ADD_MENU_INFO) ? TEXT("info line") : TEXT("heading"), 
                                                                                                                szMenuVariable, pPageID->szBuffer, pStringID->szBuffer);
         /// Replace 'read text' and 'add custom menu' commands with a macro command
         destroyCommandsInTranslatorOutputByIndex(pScriptFile->pTranslator, iIndex, 2);
         insertVirtualCommandToTranslator(pScriptFile->pTranslator, iIndex, createCustomCommandf(isCommandID(pSubCommand, CMD_ADD_MENU_INFO) ? CMD_ADD_MENU_INFO_BYREF : CMD_ADD_MENU_HEADING_BYREF, 
                                                                                                 CT_STANDARD WITH CT_VIRTUAL, iIndex, szCommandText));
         // Cleanup
         utilDeleteString(szCommandText);
      }
   }
}

/// Function name  : translateScriptFileMacros
// Description     : Replaces translated commands with macros, where appropriate
// 
// SCRIPT_FILE*  pScriptFile : [in]     Script file
// HWND          hParentWnd  : [in]     Parent window
// ERROR_QUEUE*  pErrorQueue : [in/out] ErrorQueue
// 
// Return Value   : OR_SUCCESS
// 
OPERATION_RESULT  translateScriptFileMacros(SCRIPT_FILE*  pScriptFile, HWND  hParentWnd, ERROR_QUEUE*  pErrorQueue)
{
   CONST TCHAR  *szArrayVariable,         // Name of the array variable
                *szReturnVariable,        // Name of the return variable
                *szIteratorVariable;      // Name of the array iterator variable
   TCHAR        *szCommandText;           // Buffer used for generating new macro COMMAND text
   PARAMETER    *pParameter,              // Parameter of command being examined - re-used.
                *pLoopInitial,            // 'For loop' range start Number/Variable Parameter
                *pLoopFinal;              // 'For loop' range finish Number/Variable Parameter
   COMMAND      *pCommand,                // First command in block of commands being examined
                *pSubCommand;             // subsequent command in block of commands being examined
   BOOL          bMatchFound,             // TRUE if necessary command to form a macro were found
                 bWithinSkipIf;           // Whether within a Skip-if brace or not
   UINT          iIndex;                  // Zero-based Index of current command
   INT           iLoopStep,               // 'For loop' stepping value
                 iArraySize;              // Size of array being processed

   // Prepare
   szCommandText = utilCreateEmptyString(LINE_LENGTH);
   bWithinSkipIf = FALSE;
   iIndex        = 0;

   // Iterate through decompiled commands
   for (UINT  iIndex = 0; findCommandInTranslatorOutput(pScriptFile->pTranslator, iIndex, pCommand); iIndex++)
   {
      // [CHECK] Is this a standard command within a skip-if brace?
      if (!isCommandComment(pCommand) AND bWithinSkipIf)
      {
         // [WITHIN SKIP-IF] Skip this command
         bWithinSkipIf = FALSE;
         continue;
      }
      else
         // [CHECK] Update marker
         bWithinSkipIf = (pCommand->eConditional == CI_SKIP_IF OR pCommand->eConditional == CI_SKIP_IF_NOT);

      // Examine command
      switch (pCommand->iID)
      {
      /// [CUSTOM MENU ITEM]
      case CMD_READ_TEXT:
         translateReadCustomMenuItemMacro(pScriptFile, pCommand, iIndex, pErrorQueue);
         break;

      /// [DEFINE ARRAY] Convert '$array = alloc array: size=$size' and a variable number of '$array[n] = $element' commands into 'define array' macro
      case CMD_ARRAY_ALLOC:   
         // Prepare
         bMatchFound = TRUE;

         // [CHECK] Lookup return variable and array size      '$Array = array alloc: size=6'
         if (isCommandArrayAllocation(pCommand, szArrayVariable, iArraySize) AND iArraySize > 0)
         {
            // Prepare
            StringCchPrintf(szCommandText, LINE_LENGTH, TEXT("dim $%s ="), szArrayVariable);

            // Iterate through next <arraysize> commands
            for (INT  iSubCommand = 1; bMatchFound AND iSubCommand <= iArraySize AND findCommandInTranslatorOutput(pScriptFile->pTranslator, iIndex + iSubCommand, pSubCommand); iSubCommand++)
            {
               /// [CHECK] Ensure array assignment has correct return variable and correct index   '$Array[3] = $element'
               if (bMatchFound = isCommandArrayAssignment(pSubCommand, szArrayVariable, iSubCommand - 1, pParameter))
                  // [SUCCESS] Append assignment parameter to new command
                  utilStringCchCatf(szCommandText, LINE_LENGTH, iSubCommand != iArraySize ? TEXT(" %s,") : TEXT(" %s"), pParameter->szBuffer);
            }

            // [CHECK] Ensure we found the necessary commands
            if (bMatchFound)
            {
               /// Replace 'alloc array' and 'element access' commands with a macro command
               destroyCommandsInTranslatorOutputByIndex(pScriptFile->pTranslator, iIndex, iArraySize + 1);
               insertVirtualCommandToTranslator(pScriptFile->pTranslator, iIndex, createCustomCommandf(CMD_DEFINE_ARRAY, CT_STANDARD WITH CT_VIRTUAL, iIndex, szCommandText));
            }
         }
         break;

      /// [FOR EACH IN ARRAY] Convert '$iterator = size of array $array', 'while $iterator', 'dec $iterator' and '$retvar = $array[$iterator]' into the 'for each $retvar in array $array' macro
      case CMD_SIZE_OF_ARRAY:
         // Prepare
         szArrayVariable = szReturnVariable = szIteratorVariable = NULL;
         bMatchFound     = TRUE;

         // [CHECK] Lookup array variable and return variable 
         if (isCommandArrayCount(pCommand, szIteratorVariable, szArrayVariable))
         {
            // Iterate through the next three commands
            for (UINT  iSubCommand = 1; bMatchFound AND iSubCommand <= 3 AND findCommandInTranslatorOutput(pScriptFile->pTranslator, iIndex + iSubCommand, pSubCommand); iSubCommand++)
            {
               switch (iSubCommand)
               {
               // [CHECK] Ensure next command is 'while $iterator'
               case 1: bMatchFound = isExpressionSimpleLoopConditional(pSubCommand, szIteratorVariable);   break;
               // [CHECK] Ensure next command is 'dec $iterator'
               case 2: bMatchFound = isCommandID(pSubCommand, CMD_DECREMENT) AND isVariableParameterInCommandAtIndex(pSubCommand, 0, szIteratorVariable);  break;
               // [CHECK] Ensure next command is '$item = $Array[$iterator.1]'
               case 3: bMatchFound = isCommandArrayAccess(pSubCommand, szArrayVariable, szIteratorVariable, szReturnVariable);                             break;
               }
            }

            // [CHECK] Ensure we found the necessary commands
            if (bMatchFound)
            {
               // [FOR EACH USING COUNTER] Use counter macro if Iterator isn't in format XS.Iteratorxx
               if (!isVariableMacroIterator(szIteratorVariable))
                  StringCchPrintf(szCommandText, LINE_LENGTH, TEXT("for each $%s in array $%s using counter $%s"), szReturnVariable, szArrayVariable, szIteratorVariable);
               // [FOR EACH] Hide iterator if not required
               else
                  StringCchPrintf(szCommandText, LINE_LENGTH, TEXT("for each $%s in array $%s"), szReturnVariable, szArrayVariable);

               /// Replace 'size of array', 'while', 'dec' and 'array assignment' with a 'for each' macro
               destroyCommandsInTranslatorOutputByIndex(pScriptFile->pTranslator, iIndex, 4);
               insertVirtualCommandToTranslator(pScriptFile->pTranslator, iIndex, createCustomCommandf(CMD_FOR_EACH, CT_STANDARD WITH CT_VIRTUAL, iIndex, szCommandText));
            }
         }
         break;

      /// [FOR LOOP] Convert '$iterator = initial - step', 'while $iterator < count' and '$iterator = $iterator + step' into a 'for $iterator = $a to $b step +n' macro
      ///            Convert '$iterator = initial + step', 'while $iterator > count' and '$iterator = $iterator - step' into a 'for $iterator = $a to $b step -n' macro
      case CMD_EXPRESSION:
         // Prepare
         szReturnVariable = szIteratorVariable = NULL;
         bMatchFound      = TRUE;

         // [CHECK] Check for '$iterator = initial ± step', then extract 'iterator', 'initial value' and 'step'
         if (isExpressionLoopInitialisation(pCommand, szIteratorVariable, pLoopInitial, iLoopStep)) 
         {
            // [SUCCESS] Iterate through the next two commands
            for (UINT  iSubCommand = 1; bMatchFound AND iSubCommand <= 2 AND findCommandInTranslatorOutput(pScriptFile->pTranslator, iIndex + iSubCommand, pSubCommand); iSubCommand++)
            {
               switch (iSubCommand)
               {
               // [CHECK] Ensure next command is 'while $iterator <|> final'
               case 1: bMatchFound = isExpressionRangedLoopConditional(pSubCommand, szIteratorVariable, (iLoopStep > 0 ? OT_LESS_THAN : OT_GREATER_THAN), pLoopFinal); break;
               // [CHECK] Ensure next command is '$iterator = $iterator ± 1'
               case 2: bMatchFound = isExpressionLoopCounter(pSubCommand, szIteratorVariable, iLoopStep);  break;
               }
            }

            // [CHECK] Ensure we found the necessary commands
            if (bMatchFound)
            {
               // Generate command text
               StringCchPrintf(szCommandText, LINE_LENGTH, TEXT("for $%s = %s to %s step %d"), szIteratorVariable, pLoopInitial->szBuffer, pLoopFinal->szBuffer, iLoopStep);

               /// Replace '$iterator = initial ± step', 'while $iterator < count' and 'inc/dec $iterator' with a 'for loop' macro
               destroyCommandsInTranslatorOutputByIndex(pScriptFile->pTranslator, iIndex, 3);
               insertVirtualCommandToTranslator(pScriptFile->pTranslator, iIndex, createCustomCommandf(CMD_FOR_LOOP, CT_STANDARD WITH CT_VIRTUAL, iIndex, szCommandText));
            }
         }
         break;
      }
   }

   // Return SUCCESS
   utilDeleteString(szCommandText);
   return OR_SUCCESS;
}



/// Function name  : translateScriptFileProperties
// Description     : Extract the script properties from the XMLTree within a ScriptFile (into the ScriptFile itself)
// 
// SCRIPT_FILE*  pOutput  : [in/out] ScriptFile to save the properties into
// ERROR_STACK*  pError   : [out]    Error message if any, otherwise NULL
// 
// Return Value   : TRUE if successful, FALSE if one of the properties is missing or invalid
// 
BOOL  translateScriptFileProperties(SCRIPT_FILE*  pScriptFile, ERROR_QUEUE*  pErrorQueue)
{
   XML_SCRIPT_LAYOUT  *pLayout;                   // Convenience pointer
   XML_TREE_NODE      *pArgumentNode,             // <argument> tag node defining an argument
                      *pVariableNode;             // <codearray> node defining a Variable
   ERROR_STACK        *pError;                    // Operation error
   SOURCE_VALUE_TYPE   eNodeType;                 // Whether command ID node is string or integer
   CONST TCHAR        *szCommandID,               // Command override ID of the input script
                      *szVariableName;            // Variable 
   GAME_STRING        *pCommandLookup;
   ARGUMENT           *pArgument;                 // Argument currently being created
   UINT                iCommandID;
   
   // Prepare
   pError      = NULL;
   szCommandID = NULL;
   pLayout     = &pScriptFile->pTranslator->oLayout;

   /// [SUCCESS] Extract properties
   pScriptFile->szScriptname   = utilDuplicateSimpleString(getScriptPropertyNameString(pLayout->pNameNode));
   pScriptFile->szDescription  = utilDuplicateSimpleString(getScriptPropertyNameString(pLayout->pDescriptionNode));
   pScriptFile->iVersion       = getScriptPropertyNameInteger(pLayout->pVersionNode);
   pScriptFile->eGameVersion   = calculateGameVersionFromEngineVersion(getScriptPropertyNameInteger(pLayout->pEngineVersionNode));
   pScriptFile->bLiveData      = getScriptPropertyNameInteger(pLayout->pLiveDataNode);
   pScriptFile->bSignature     = (pLayout->pSignatureNode != NULL);

   /// [COMMAND ID] Determine whether command ID is a string or an integer
   if (getSourceValueType(pLayout->pCommandIDNode, eNodeType, pError) AND eNodeType == SVT_STRING)
      // [STRING] Store Command name
      pScriptFile->szCommandID = utilDuplicateSimpleString(getScriptPropertyNameString(pLayout->pCommandIDNode));

   // [INTEGER/NULL] Lookup Command name or use empty string
   else if (findGameStringByID(iCommandID = getScriptPropertyNameInteger(pLayout->pCommandIDNode), GPI_OBJECT_COMMANDS, pCommandLookup))
      pScriptFile->szCommandID = utilDuplicateSimpleString(iCommandID ? pCommandLookup->szText : TEXT(""));

   else
   {  // [WARNING] "This script implements a command with ID %d that could not be found in your game data"
      pushErrorQueue(pErrorQueue, generateDualWarning(HERE(IDS_SCRIPT_COMMAND_ID_NOT_FOUND), iCommandID));
      pScriptFile->szCommandID = utilDuplicateSimpleString(getScriptPropertyNameString(pLayout->pCommandIDNode)); //utilCreateStringf(TEXT("%d"), getScriptPropertyNameInteger(pLayout->pCommandIDNode));
   }

   // [VERBOSE]
   VERBOSE("Enumerating script arguments and variables of '%s'", identifyScriptName(pScriptFile));

   // Iterate through ARGUMENTS branch
   for (UINT iIndex = 0; !pError AND findXMLTreeNodeByIndex(pLayout->pArgumentsNode, iIndex, pArgumentNode); iIndex++)
   {
      /// Create new ARGUMENT in SCRIPT-FILE
      if (pArgument = createArgumentFromNode(pArgumentNode, iIndex, pError))
         appendArgumentToScriptFile(pScriptFile, pArgument);
      else
         pushErrorQueue(pErrorQueue, pError);
   }

   // Iterate through VARIABLES branch
   for (UINT iIndex = 0; !pError AND findXMLTreeNodeByIndex(pLayout->pVariableNamesBranch, iIndex, pVariableNode); iIndex++)
   {
      /// Create new VARIABLE-NAME in TRANSLATOR
      if (getXMLPropertyString(pVariableNode, TEXT("val"), szVariableName))
         // Add VariableName with a sequential ID
         appendVariableNameToTranslator(pScriptFile->pTranslator, szVariableName);
   }
      
   // Return TRUE if successful, otherwise FALSE
   return (pError == NULL);
}


/// Function name  : translateXMLToScriptFile
// Description     : Generates an XML-Tree, stores the position of the important nodes and initialises the 'command node' iterators.
// 
// SCRIPT_FILE*         pScriptFile     : [in]               ScriptFile with an input buffer full of MSCI XML
// HWND                 hParentWnd      : [in]               Parent window used to display error messages
// OPERATION_PROGRESS*  pProgress       : [in/out][optional] Operation progress object, if feedback is required
// ERROR_QUEUE*         pErrorQueue     : [in/out]           Error messages detected during translation, if any
// 
// Return Value   : TRUE if the XML was parsed successfully and the critical nodes were present, otherwise FALSE
// 
BOOL  translateXMLToScriptFile(SCRIPT_FILE*  pScriptFile, HWND  hParentWnd, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue)
{
   ERROR_STACK  *pError;         // Error encountered, if any
   BOOL          bResult;        // Operation result

   // [CHECK] Script file input buffer exists
   ASSERT(hasInputData(pScriptFile));
   
   // Prepare
   pError  = NULL;
   bResult = TRUE;

   /// [GENERATE TREE] Parse XML into an XML Tree
   if (generateXMLTree(pScriptFile->szInputBuffer, pScriptFile->iInputSize, identifyScriptName(pScriptFile), hParentWnd, pScriptFile->pTranslator->pXMLTree, pProgress, pErrorQueue) != OR_SUCCESS)
   {
      // [ERROR] "One or more syntax errors were discovered during processing that have prevented translation being attempted"
      enhanceLastError(pErrorQueue, ERROR_ID(IDS_SCRIPT_XML_ERROR));
      generateOutputTextFromLastError(pErrorQueue);

      // [CRITICAL ERROR] Return FALSE
      bResult = FALSE;
   }
   /// [LOCATE NODES] Find the <script> and <codearray> nodes, the standard/auxiliary/variables branches and the command iterators
   else if (!findScriptTranslationNodes(pScriptFile->pTranslator, pError))
   {
      // [ERROR] "Critical XML tags in the script are missing or invalid"
      enhanceError(pError, ERROR_ID(IDS_SCRIPT_STRUCTURE_INVALID));
      generateOutputTextFromError(pError);
      pushErrorQueue(pErrorQueue, pError);

      // [CRITICAL ERROR] Return FALSE
      bResult = FALSE;
   }

   // Return result
   return bResult;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       THREAD OPERATIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : threadprocLoadScriptFile
// Description     : Parse a script into a new ScriptFile object and return it
// 
// VOID*  pParameters   : OPERATION_DATA structure containing:
//                        ->   szFullPath : FilePath of the target file to open
//
// Return type : Ignored
// Operation Stages : TWO (Parsing, Translating)
//
// Operation Result : OR_SUCCESS - The script was loaded successfully, any minor errors were ignored by the user
//                    OR_FAILURE - The script was NOT loaded due to critical errors that prevented translation.
//                    OR_ABORTED - The script was NOT loaded because the user aborted after minor errors
// 
BearScriptAPI
DWORD   threadprocLoadScriptFile(VOID*  pParameter)
{
   DOCUMENT_OPERATION*  pOperationData;   // Convenience pointer
   SCRIPT_FILE*         pScriptFile;      // Convenience pointer
   OPERATION_RESULT     eResult;          // Operation result, defaults to SUCCESS

   // [TRACKING]
   TRACK_FUNCTION();
   VERBOSE_LIB_COMMAND();
   SET_THREAD_NAME("Script Translation");
   setThreadLanguage(getAppPreferences()->eAppLanguage);

   // [CHECK] Ensure parameter exists
   ASSERT(pParameter);
   
   // Prepare
   pOperationData = (DOCUMENT_OPERATION*)pParameter;
   pScriptFile    = (SCRIPT_FILE*)pOperationData->pGameFile;
   eResult        = OR_SUCCESS;

   // [STAGE] Set parsing stage
   ASSERT(getOperationProgressStageID(pOperationData->pProgress) == IDS_PROGRESS_PARSING_SCRIPT);

   /// [GUARD BLOCK]
   __try
   {
      // [INFO] "Loading XML from file '%s'"
      pushErrorQueue(pOperationData->pErrorQueue, generateDualInformation(HERE(IDS_OUTPUT_LOADING_SCRIPT_XML), PathFindFileName(pOperationData->szFullPath)));
      VERBOSE_SMALL_PARTITION();

      /// [LOAD] Load XML into ScriptFile buffer
      if (!loadGameFileFromFileSystemByPath(getFileSystem(), pScriptFile, NULL, pOperationData->pErrorQueue))
      {
         // [ERROR] "The MSCI script '%s' is unavailable or could not be accessed"
         enhanceLastError(pOperationData->pErrorQueue, ERROR_ID(IDS_SCRIPT_LOAD_IO_ERROR), pOperationData->szFullPath);
         eResult = OR_FAILURE;
      }
      else
         /// [SUCCESS] Attempt to translate XML into a ScriptFile
         eResult = translateScript(pScriptFile, FALSE, pOperationData->hParentWnd, pOperationData->pProgress, pOperationData->pErrorQueue);
   }
   /// [EXCEPTION HANDLER]
   __except (generateQueuedExceptionError(GetExceptionInformation(), pOperationData->pErrorQueue))
   {
      // [ERROR] "An unidentified and unexpected critical error has occurred while translating the script '%s'"
      enhanceLastError(pOperationData->pErrorQueue, ERROR_ID(IDS_EXCEPTION_LOAD_SCRIPT_FILE), pOperationData->szFullPath);
      
      // [FAILURE]
      eResult = OR_FAILURE;
   }

   // [DEBUG] Separate previous output from further output for claritfy
   VERBOSE_THREAD_COMPLETE("SCRIPT TRANSLATION WORKER THREAD COMPLETED");
   
   // Cleanup and return result
   closeThreadOperation(pOperationData, eResult);
   END_TRACKING();
   return  THREAD_RETURN;
}
