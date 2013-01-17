//
// FILE_NAME.cpp : FILE_DESCRIPTION
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        MACROS
/// /////////////////////////////////////////////////////////////////////////////////////////

#define VALIDATE_BY_TEXT

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////

CONST UINT  iErrorThreshold = 15;

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : validateScriptFileProperties
// Description     : Validates the properties, arguments and variables of two ScriptFiles
// 
// CONST SCRIPT_FILE*  pValidationFile : [in]     Validation script
// CONST SCRIPT_FILE*  pScriptFile     : [in]     Original script
// ERROR_QUEUE*        pErrorQueue     : [in/out] Error queue
// 
// Return Value   : TRUE
// 
BOOL  validateScriptFileProperties(CONST SCRIPT_FILE*  pValidationFile, CONST SCRIPT_FILE*  pScriptFile, ERROR_QUEUE*  pErrorQueue)
{
   VARIABLE_NAME  *pVariable1,      // Variable from the validation script
                  *pVariable2;      // Variable from the original script
   ARGUMENT       *pArgument1,      // Argument from the validation script
                  *pArgument2;      // Argument from the original script

   // [TRACK]
   TRACK_FUNCTION();

   /// [SCRIPT-NAME] 
   if (!utilCompareStringVariables(pValidationFile->szScriptname, pScriptFile->szScriptname))
      // [WARNING] "The %s property '%s' does not match the original '%s'"
      generateQueuedWarning(pErrorQueue, HERE(IDS_VALIDATION_STRING_PROPERTY_MISMATCH), TEXT("script name"), pValidationFile->szScriptname, pScriptFile->szScriptname);

   /// [DESCRIPTION] 
   if (!utilCompareStringVariables(pValidationFile->szDescription, pScriptFile->szDescription))
      // [WARNING] "The %s property '%s' does not match the original '%s'"
      generateQueuedWarning(pErrorQueue, HERE(IDS_VALIDATION_STRING_PROPERTY_MISMATCH), TEXT("description"), pValidationFile->szDescription, pScriptFile->szDescription);

   /// [COMMAND ID]
   if (lstrlen(pValidationFile->szCommandID) AND pScriptFile->szCommandID AND !utilCompareStringVariables(pValidationFile->szCommandID, pScriptFile->szCommandID))
      // [WARNING] "The %s property '%s' does not match the original '%s'"
      generateQueuedWarning(pErrorQueue, HERE(IDS_VALIDATION_STRING_PROPERTY_MISMATCH), TEXT("command ID"), pValidationFile->szCommandID, pScriptFile->szCommandID);

   /// [GAME VERSION]
   if (pValidationFile->eGameVersion != pScriptFile->eGameVersion)
      // [WARNING] "The %s property '%s' does not match the original '%s'"
      generateQueuedWarning(pErrorQueue, HERE(IDS_VALIDATION_STRING_PROPERTY_MISMATCH), TEXT("game version"), identifyGameVersionString(pValidationFile->eGameVersion), identifyGameVersionString(pScriptFile->eGameVersion));

   ///// [SIGNATURE]
   //if (pValidationFile->bSignature != pScriptFile->bSignature)
   //   // [WARNING] "The %s property '%s' does not match the original '%s'"
   //   generateQueuedWarning(pErrorQueue, HERE(IDS_VALIDATION_STRING_PROPERTY_MISMATCH), TEXT("signature"), pValidationFile->bSignature ? TEXT("Signed") : TEXT("Unsigned"), pScriptFile->bSignature ? TEXT("Signed") : TEXT("Unsigned"));

   /// [VERSION]
   if (pValidationFile->iVersion != pScriptFile->iVersion)
      // [WARNING] "The %s property '%d' does not match the original '%d'"
      generateQueuedWarning(pErrorQueue, HERE(IDS_VALIDATION_INTEGER_PROPERTY_MISMATCH), TEXT("version"), pValidationFile->iVersion, pScriptFile->iVersion);

   /// [ARGUMENT COUNT]
   if (getTreeNodeCount(pValidationFile->pArgumentTreeByID) != getTreeNodeCount(pScriptFile->pArgumentTreeByID))
      // [ERROR] "There are %d arguments in the validation file but %d in the original"
      generateQueuedError(pErrorQueue, HERE(IDS_VALIDATION_ARGUMENT_COUNT_MISMATCH), TEXT("version"), getTreeNodeCount(pValidationFile->pArgumentTreeByID), getTreeNodeCount(pScriptFile->pArgumentTreeByID));

   /// [ARGUMENTS]
   for (UINT iIndex = 0; findArgumentInScriptFileByIndex(pValidationFile, iIndex, pArgument1) AND findArgumentInScriptFileByIndex(pScriptFile, iIndex, pArgument2); iIndex++)
   {
      /// [ID]
      if (pArgument1->iID != pArgument2->iID)
         // [WARNING] "The %s property of %s %d is '%d', which does not match the original '%d'"
         generateQueuedWarning(pErrorQueue, HERE(IDS_VALIDATION_VARIABLE_INTEGER_MISMATCH), TEXT("ID"), TEXT("argument"), iIndex, pArgument1->iID, pArgument2->iID);
      /// [NAME]
      if (!utilCompareStringVariables(pArgument1->szName, pArgument2->szName))
         // [WARNING] "The %s property of %s %d is '%s', which does not match the original '%s'"
         generateQueuedWarning(pErrorQueue, HERE(IDS_VALIDATION_VARIABLE_STRING_MISMATCH), TEXT("name"), TEXT("argument"), iIndex, pArgument1->szName, pArgument2->szName);
      /// [DESCRIPTION]
      if (!utilCompareStringVariables(pArgument1->szDescription, pArgument2->szDescription))
         // [WARNING] "The %s property of %s %d is '%s', which does not match the original '%s'"
         generateQueuedWarning(pErrorQueue, HERE(IDS_VALIDATION_VARIABLE_STRING_MISMATCH), TEXT("description"), TEXT("argument"), iIndex, pArgument1->szDescription, pArgument2->szDescription);
      /// [TYPE]
      if (pArgument1->eType != pArgument2->eType)
         // [WARNING] "The %s property of %s %d is '%s', which does not match the original '%s'"
         generateQueuedWarning(pErrorQueue, HERE(IDS_VALIDATION_VARIABLE_STRING_MISMATCH), TEXT("syntax"), TEXT("argument"), iIndex, szParameterSyntax[pArgument1->eType], szParameterSyntax[pArgument2->eType]);
   }

   /// [VARIABLES]
   for (UINT iIndex = 0; findVariableNameInTranslatorByIndex(pValidationFile->pTranslator, iIndex, pVariable1) AND findVariableNameInTranslatorByIndex(pScriptFile->pTranslator, iIndex, pVariable2); iIndex++)
   {
      /// [NAME]
      if (!utilCompareStringVariables(pVariable1->szName, pVariable2->szName))
         // [WARNING] "The %s property of %s %d is '%s', which does not match the original '%s'"
         generateQueuedWarning(pErrorQueue, HERE(IDS_VALIDATION_VARIABLE_STRING_MISMATCH), TEXT("name"), TEXT("variable"), iIndex, pVariable1->szName, pVariable2->szName);
      /// [ID]
      if (pVariable1->iID != pVariable2->iID)
         // [WARNING] "The %s property of %s %d is '%d', which does not match the original '%d'"
         generateQueuedWarning(pErrorQueue, HERE(IDS_VALIDATION_VARIABLE_INTEGER_MISMATCH), TEXT("ID"), TEXT("variable"), iIndex, pVariable1->iID, pVariable2->iID);
   }

   // Return TRUE
   END_TRACKING();
   return TRUE;
}



/// Function name  : validateScriptCodeArrayCommandIDNodes
// Description     : Validates the command ID node, which can be a string or a GameString ID
// 
// CONST XML_TREE_NODE*  pValidation : [in]     Validation node
// CONST XML_TREE_NODE*  pOriginal   : [in]     Original node
// ERROR_QUEUE*          pErrorQueue : [in/out] Error queue
// 
// Return Value   : TRUE if number and content of all nodes match, otherwise FALSE
// 
BOOL  validateScriptCodeArrayCommandIDNodes(CONST XML_TREE_NODE*  pValidation, CONST XML_TREE_NODE*  pOriginal, ERROR_QUEUE*  pErrorQueue)
{
   CONST TCHAR   *szType1,       // Type of the validation node
                 *szType2,       // Type of the original node
                 *szValue1,      // Value of the validation node
                 *szValue2;      // Value of the original node
   GAME_STRING   *pCommandName;  // Resolved command name
   BOOL           bResult;       // Validation result
   UINT           iCommandID;    // Command ID

   // [CHECK] Ensure both nodes exist
   ASSERT(pValidation AND pOriginal);

   // Prepare
   bResult = TRUE;

   // Extract types and values
   ASSERT(getXMLPropertyString(pValidation, TEXT("type"), szType1));
   ASSERT(getXMLPropertyString(pValidation, TEXT("val"), szValue1));
   ASSERT(getXMLPropertyString(pOriginal, TEXT("type"), szType2));
   ASSERT(getXMLPropertyString(pOriginal, TEXT("val"), szValue2));

   // [CHECK] Are the two nodes different?
   if (!utilCompareStringVariables(szType1, szType2) OR !utilCompareStringVariables(szValue1, szValue2))
   {
      /// [CHECK] Ensure types are different
      if (utilCompareStringVariables(szType1, szType2))
      {
         // [ERROR] "The %s of the %s %s node (Index:%d) on line %u is '%s', which does not match the original '%s' on line %u"
         generateQueuedError(pErrorQueue, HERE(IDS_VALIDATION_ARRAY_STRING_MISMATCH), TEXT("value"), TEXT("codearray"), TEXT("command ID"), pValidation->iIndex, pValidation->iLineNumber, szValue1, szValue2, pOriginal->iLineNumber);
         bResult = FALSE;  // [FAILED] Same type, different value
      }
      else
      {
         // Extract the ID of the command
         iCommandID = utilConvertStringToInteger( utilCompareString(szType1, "int") ? szValue1 : szValue2 );
         
         /// Lookup command ID
         if (!findGameStringByID(iCommandID, GPI_OBJECT_COMMANDS, pCommandName))
         {
            // [ERROR] "Cannot find a object command with the ID '%s'"
            generateQueuedError(pErrorQueue, HERE(IDS_VALIDATION_COMMAND_ID_NOT_FOUND), (utilCompareString(szType1, "int") ? szValue1 : szValue2));
            bResult = FALSE;  // [FAILED] Command Name not found
         }
         /// [CHECK] Compare command name to the string node
         else if (!utilCompareStringVariables(pCommandName->szText, utilCompareString(szType1, "string") ? szValue1 : szValue2))
         {
            // [ERROR] "The %s of the %s %s node (Index:%d) on line %u is '%s', which does not match the original '%s' on line %u"
            generateQueuedError(pErrorQueue, HERE(IDS_VALIDATION_ARRAY_STRING_MISMATCH), TEXT("resolved name"), TEXT("object"), TEXT("command"), pValidation->iIndex, pValidation->iLineNumber, pCommandName->szText, (utilCompareString(szType1, "string") ? szValue1 : szValue2), pOriginal->iLineNumber);
            bResult = FALSE;     // [FAILED] Resolved command names are different
         }
      }
   }

   // Return result
   return bResult;
}



/// Function name  : validateScriptNodes
// Description     : Validates the contents of two nodes
// 
// CONST XML_TREE_NODE*  pValidation : [in]     Validation node
// CONST XML_TREE_NODE*  pOriginal   : [in]     Original node
// CONST TCHAR*          szArrayType : [in]     Name of the array containing these nodes, eg. standard command, varaibles branch
// CONST TCHAR*          szNodeType  : [in]     Name of these nodes, eg. parameter, declaration
// ERROR_QUEUE*          pErrorQueue : [in/out] Error queue
// 
// Return Value   : TRUE if number and content of all nodes match, otherwise FALSE
// 
BOOL  validateScriptNodes(CONST XML_TREE_NODE*  pValidation, CONST XML_TREE_NODE*  pOriginal, CONST TCHAR*  szArrayType, CONST TCHAR*  szNodeType, ERROR_QUEUE*  pErrorQueue)
{
   CONST TCHAR   *szType,        // Type of the current node
                 *szValue1,      // Value of the validation node
                 *szValue2;      // Value of the original node
   BOOL           bResult;       // Validation result

   // [CHECK] Ensure both nodes exist
   ASSERT(pValidation AND pOriginal);

   // Prepare
   bResult = TRUE;

   // Extract types
   ASSERT(getXMLPropertyString(pValidation, TEXT("type"), szType));
   ASSERT(getXMLPropertyString(pOriginal, TEXT("type"), szValue2));

   /// [TYPE] Compare types as strings
   if (!utilCompareStringVariables(szType, szValue2))
   {
      // [ERROR] "The %s of the %s %s node (Index:%d) on line %u is '%s', which does not match the original '%s' on line %u"
      generateQueuedError(pErrorQueue, HERE(IDS_VALIDATION_ARRAY_STRING_MISMATCH), TEXT("type"), szArrayType, szNodeType, pValidation->iIndex, pValidation->iLineNumber, szType, szValue2, pOriginal->iLineNumber);
      bResult = FALSE;
   }

   // Extract values
   ASSERT(getXMLPropertyString(pValidation, TEXT("val"), szValue1));
   ASSERT(getXMLPropertyString(pOriginal, TEXT("val"), szValue2));

   /// [VALUE] Compare values as strings
   if (!utilCompareStringVariables(szValue1, szValue2))
   {
      // [CHECK] Output as strings or integers
      if (utilCompareString(szType, "string"))
         // [ERROR] "The %s of the %s %s node (Index:%d) on line %u is '%s', which does not match the original '%s' on line %u"
         generateQueuedError(pErrorQueue, HERE(IDS_VALIDATION_ARRAY_STRING_MISMATCH), TEXT("value"), szArrayType, szNodeType, pValidation->iIndex, pValidation->iLineNumber, szValue1, szValue2, pOriginal->iLineNumber);
      else
         // [ERROR] "The %s of the %s %s node (Index:%d) on line %u is 0x%X, which does not match the original 0x%X on line %u"
         generateQueuedError(pErrorQueue, HERE(IDS_VALIDATION_ARRAY_INTEGER_MISMATCH), TEXT("value"), szArrayType, szNodeType, pValidation->iIndex, pValidation->iLineNumber, utilConvertStringToInteger(szValue1), utilConvertStringToInteger(szValue2), pOriginal->iLineNumber);

      // [FAILED] Return FALSE
      bResult = FALSE;
   }

   // Return result
   return bResult;
}


/// Function name  : validateScriptArrayNodes
// Description     : Validates the contents of two array nodes
// 
// CONST XML_TREE_NODE*  pValidation : [in]     Validation node
// CONST XML_TREE_NODE*  pOriginal   : [in]     Original node
// CONST TCHAR*          szArrayType : [in]     Name of the array, eg. standard command, varaibles branch
// CONST TCHAR*          szNodeType  : [in]     Name of each node, eg. parameter, declaration
// ERROR_QUEUE*          pErrorQueue : [in/out] Error queue
// 
// Return Value   : TRUE if number and content of all nodes match, otherwise FALSE
// 
BOOL  validateScriptArrayNodes(CONST XML_TREE_NODE*  pValidation, CONST XML_TREE_NODE*  pOriginal, CONST TCHAR*  szArrayType, CONST TCHAR*  szNodeType, ERROR_QUEUE*  pErrorQueue)
{
   XML_TREE_NODE   *pValidationIterator,     // Child Node iterator for validation node
                   *pOriginalIterator;       // Child Node iterator for original node
   BOOL             bResult;                 // Validation result

   // [TRACK]
   TRACK_FUNCTION();

   // [CHECK] Ensure both nodes exist
   ASSERT(pValidation AND pOriginal);

   // Prepare
   bResult = TRUE;

   /// [COUNT]
   if (getXMLNodeCount(pValidation) != getXMLNodeCount(pOriginal))
   {
      // [ERROR] "There are %d %s nodes in the %s on line %u of the validation file, but %d in the original"
      generateQueuedError(pErrorQueue, HERE(IDS_VALIDATION_ARRAY_COUNT_MISMATCH), getXMLNodeCount(pValidation), szNodeType, szArrayType, pValidation->iLineNumber, getXMLNodeCount(pOriginal));
      bResult = FALSE;
   }

   /// [CONTENT] Iterate through child nodes
   for (findNextXMLTreeNode(pValidation, XNR_CHILD, pValidationIterator), findNextXMLTreeNode(pOriginal, XNR_CHILD, pOriginalIterator); 
         pValidationIterator AND pOriginalIterator; 
            findNextXMLTreeNode(pValidationIterator, XNR_SIBLING, pValidationIterator), findNextXMLTreeNode(pOriginalIterator, XNR_SIBLING, pOriginalIterator))
   {
      // [CHECK] Validate node
      if (!validateScriptNodes(pValidationIterator, pOriginalIterator, szArrayType, szNodeType, pErrorQueue))
         // [FAILED] Set result to FALSE
         bResult = FALSE;
   }
   
   // Return result
   END_TRACKING();
   return bResult;
}


/// Function name  : validateScriptCommandNodes
// Description     : Validates the contents of two array nodes
// 
// CONST XML_TREE_NODE*  pValidationCommand : [in]     Validation command node
// CONST XML_TREE_NODE*  pOriginalCommand   : [in]     Original command node
// CONST COMMAND_TYPE    eType              : [in]     Whether standard or auxiliary command
// CONST XML_TREE_NODE*  pStdCommandBranch  : [in]     Node containing the standard commands branch
// CONST GAME_VERSION    eScriptVersion     : [in]     Game version of both scripts
// CONST UINT            iLineNumber        : [in]     Zero-based logical line number of this command within the script
// ERROR_QUEUE*          pErrorQueue        : [in/out] Error queue
// 
// Return Value   : TRUE if number and content of all nodes match, otherwise FALSE
// 
BOOL  validateScriptCommandNodes(XML_TREE_NODE*  pValidationCommand, XML_TREE_NODE*  pOriginalCommand, CONST COMMAND_TYPE  eType, CONST XML_TREE_NODE*  pStdCommandBranch, CONST GAME_VERSION  eScriptVersion, CONST UINT  iLineNumber, ERROR_QUEUE*  pErrorQueue)
{
   CONST COMMAND_SYNTAX *pSyntax,                  // Syntax for the command being processed
                        *pReferenceSyntax;         // [AUXILIARY] Syntax for the associated standard command
   XML_TREE_NODE        *pValidationIterator,      // Parameter node iterator for the validation command
                        *pOriginalIterator;        // Parameter node iterator for the original command
   COMMAND_NODE         *pCommandNode;             // Used for extracting command information when errors occur
   ERROR_STACK          *pError;                   // 
   UINT                  iCommandID,               // ID of the command failing validation
                         iReferenceIndex;          // [AUXILIARY] ID of the associated standard command
   BOOL                  bResult;

   // [TRACK]
   TRACK_FUNCTION();

   // [CHECK] Ensure both nodes exist
   ASSERT(pValidationCommand AND pOriginalCommand);

   // Prepare
   bResult = TRUE;

   /// [COUNT]
   if (getXMLNodeCount(pValidationCommand) != getXMLNodeCount(pOriginalCommand))
   {
      // [ERROR] "There are %d %s nodes in the %s on line %u of the validation file, but %d in the original"
      generateQueuedError(pErrorQueue, HERE(IDS_VALIDATION_ARRAY_COUNT_MISMATCH), getXMLNodeCount(pValidationCommand), TEXT("parameter"), (eType == CT_STANDARD ? TEXT("standard command") : TEXT("auxiliary command")), pValidationCommand->iLineNumber, getXMLNodeCount(pOriginalCommand));
      bResult = FALSE;
   }

   /// [CONTENT] Iterate through child nodes
   for (findNextXMLTreeNode(pValidationCommand, XNR_CHILD, pValidationIterator), findNextXMLTreeNode(pOriginalCommand, XNR_CHILD, pOriginalIterator); 
         pValidationIterator AND pOriginalIterator; 
            findNextXMLTreeNode(pValidationIterator, XNR_SIBLING, pValidationIterator), findNextXMLTreeNode(pOriginalIterator, XNR_SIBLING, pOriginalIterator))
   {
      // [CHECK] Validate node
      if (!validateScriptNodes(pValidationIterator, pOriginalIterator, identifyCommandTypeString(eType), TEXT("parameter"), pErrorQueue))
         // [FAILED] Set result to FALSE
         bResult = FALSE;
   }

   // [ERROR]
   if (!bResult)
   {
      // Prepare
      pCommandNode = createCommandNode(eType, (XML_TREE_NODE*)pValidationCommand);

      // [CHECK] Lookup syntax for this command
      getCommandNodeID(pCommandNode, iCommandID, pError);
      findCommandSyntaxByID(iCommandID, eScriptVersion, pSyntax, pError);
      ASSERT(pSyntax);

      switch (eType)
      {
      /// [STANDARD]
      case CT_STANDARD:
         // [ERROR] "Error in the standard command '%s' on line %d"
         enhanceLastError(pErrorQueue, ERROR_ID(IDS_VALIDATION_STD_COMMAND_ERROR), (pSyntax->iID != CMD_EXPRESSION ? pSyntax->szSyntax : TEXT("<expression>")), iLineNumber);
         generateOutputTextFromLastError(pErrorQueue);
         break;

      /// [AUXILIARY]
      case CT_AUXILIARY:
         // [CHECK] Lookup associated standard command  (Re-use the CommandNode)
         if (getCommandNodeIndex(pCommandNode, iReferenceIndex, pError) AND findXMLTreeNodeByIndex(pStdCommandBranch, iReferenceIndex, pCommandNode->pNode))
         {
            // Change type
            pCommandNode->eType = CT_STANDARD;

            // [CHECK] Lookup syntax for this command
            getCommandNodeID(pCommandNode, iCommandID, pError);
            findCommandSyntaxByID(iCommandID, eScriptVersion, pReferenceSyntax, pError);
            ASSERT(pReferenceSyntax);
            
            // [ERROR] "Error in the auxiliary command '%s' on line %d, targetting standard command '%s' at index %d"
            enhanceLastError(pErrorQueue, ERROR_ID(IDS_VALIDATION_AUX_COMMAND_ERROR), pSyntax->szSyntax, iLineNumber, (pReferenceSyntax->iID != CMD_EXPRESSION ? pReferenceSyntax->szSyntax : TEXT("<expression>")), iReferenceIndex);
            generateOutputTextFromLastError(pErrorQueue);
         }
         break;
      }

      // Cleanup
      deleteCommandNode(pCommandNode, FALSE);
   }
   
   // Return result
   END_TRACKING();
   return bResult;
}


/// Function name  : validateScriptFileCommands
// Description     : Validates the commands in two scripts
// 
// CONST SCRIPT_TRANSLATOR*  pValidation     : [in]     Validation script
// CONST SCRIPT_TRANSLATOR*  pOriginal       : [in]     Original script
// CONST GAME_VERSION        eScriptVersion  : [in]     Game version of the script
// OPERATION_PROGRESS*       pProgress       : [in]     Progress tracker
// ERROR_QUEUE*              pErrorQueue     : [in/out] Error queue
// 
// Return Value   : OR_SUCCESS - Zero errors
//                  OR_FAILURE - One or more errors
// 
OPERATION_RESULT  validateScriptFileCommands(SCRIPT_TRANSLATOR*  pValidation, SCRIPT_TRANSLATOR*  pOriginal, CONST GAME_VERSION  eScriptVersion, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue)
{
   CONST XML_SCRIPT_LAYOUT  *pValidationLayout,          // Key nodes in the validation script
                            *pOriginalLayout;            // Key nodes in the original script
   COMMAND_NODE             *pValidationCommandNode,     // Command currently being checked from the validation script
                            *pOriginalCommandNode;       // Command currently being checked from the original script
   ERROR_STACK              *pError;                     // Error
   OPERATION_RESULT          eResult;                    // Result
   UINT                      iLineNumber,                // Line number of the commands being checked
                             iCommandID,                 // ID of the command being checked
                             iFailureCount;              // Failure count
   // [TRACK]
   TRACK_FUNCTION();

   // Prepare
   pValidationLayout = &pValidation->oLayout;
   pOriginalLayout   = &pOriginal->oLayout;
   eResult           = OR_SUCCESS;
   iFailureCount     = 0;
   iLineNumber       = 0;

   // [PROGRESS] Define progress as number of standard+auxiliary commands
   updateOperationProgressMaximum(pProgress, getXMLNodeCount(pValidationLayout->pStandardCommandsBranch) + getXMLNodeCount(pValidationLayout->pAuxiliaryCommandsBranch));

   /// [STANDARD COUNT]
   if (getXMLNodeCount(pValidationLayout->pStandardCommandsBranch) != getXMLNodeCount(pOriginalLayout->pStandardCommandsBranch))
      // [ERROR] "There are %d %s commands in the validation file but %d in the original"
      generateQueuedError(pErrorQueue, HERE(IDS_VALIDATION_COMMAND_COUNT_MISMATCH), getXMLNodeCount(pValidationLayout->pStandardCommandsBranch), TEXT("standard"), getXMLNodeCount(pOriginalLayout->pStandardCommandsBranch));

   /// [AUXILIARY COUNT]
   if (getXMLNodeCount(pValidationLayout->pAuxiliaryCommandsBranch) != getXMLNodeCount(pOriginalLayout->pAuxiliaryCommandsBranch))
      // [ERROR] "There are %d %s commands in the validation file but %d in the original"
      generateQueuedError(pErrorQueue, HERE(IDS_VALIDATION_COMMAND_COUNT_MISMATCH), getXMLNodeCount(pValidationLayout->pAuxiliaryCommandsBranch), TEXT("auxiliary"), getXMLNodeCount(pOriginalLayout->pAuxiliaryCommandsBranch));

   
   // Prepare
   pValidation->pAuxiliaryIterator = NULL;
   pOriginal->pAuxiliaryIterator   = NULL;

   // Destroy existing standard iterators
   if (pValidation->pStandardIterator)
      deleteCommandNode(pValidation->pStandardIterator, FALSE);
   if (pOriginal->pStandardIterator)
      deleteCommandNode(pOriginal->pStandardIterator, FALSE);
   // Destroy existing auxiliary iterators
   if (pValidation->pAuxiliaryIterator)
      deleteCommandNode(pValidation->pAuxiliaryIterator, FALSE);
   if (pOriginal->pAuxiliaryIterator)
      deleteCommandNode(pOriginal->pAuxiliaryIterator, FALSE);

   /// [COMMAND ITERATOR] Re-Initialise Standard iterators
   pValidation->pStandardIterator = createCommandNode(CT_STANDARD, NULL);
   findNextXMLTreeNode(pValidationLayout->pStandardCommandsBranch, XNR_CHILD, pValidation->pStandardIterator->pNode);

   pOriginal->pStandardIterator = createCommandNode(CT_STANDARD, NULL);
   findNextXMLTreeNode(pOriginalLayout->pStandardCommandsBranch, XNR_CHILD, pOriginal->pStandardIterator->pNode);

   /// [COMMAND ITERATOR] Re-Initialise Auxiliary iterators
   if (getXMLNodeCount(pValidationLayout->pAuxiliaryCommandsBranch))
   {
      pValidation->pAuxiliaryIterator = createCommandNode(CT_AUXILIARY, NULL);
      findNextXMLTreeNode(pValidationLayout->pAuxiliaryCommandsBranch, XNR_CHILD, pValidation->pAuxiliaryIterator->pNode);

      pOriginal->pAuxiliaryIterator = createCommandNode(CT_AUXILIARY, NULL);
      findNextXMLTreeNode(pOriginalLayout->pAuxiliaryCommandsBranch, XNR_CHILD, pOriginal->pAuxiliaryIterator->pNode);
   }

   /// [COMMANDS] Iterate through commands in display order
   while ((iFailureCount < iErrorThreshold) AND findNextCommandNode(pValidation, pValidationCommandNode, pError) AND findNextCommandNode(pOriginal, pOriginalCommandNode, pError))
   {
      // Lookup command ID
      getCommandNodeID(pOriginalCommandNode, iCommandID, pError);

      // Examine command ID
      switch (iCommandID)
      {
      // [COMMAND COMMENT] Skip validation
      case CMD_COMMAND_COMMENT:
      case CMD_DELAYED_COMMAND:
      case CMD_IS_DATATYPE:
         break;

      default:
         // [CHECK] Compare command nodes
         if (!validateScriptCommandNodes(pValidationCommandNode->pNode, pOriginalCommandNode->pNode, pValidationCommandNode->eType, pValidation->oLayout.pStandardCommandsBranch, eScriptVersion, iLineNumber + 1, pErrorQueue))
            // [FAILED] Increment failure count
            iFailureCount++;

         // [PROGRESS]
         advanceOperationProgressValue(pProgress);
         break;
      }

      // [CHECK] Do not increment the line count for hidden commands
      if (iCommandID != CMD_HIDDEN_JUMP)
         iLineNumber++;

      // Cleanup
      deleteCommandNode(pValidationCommandNode, FALSE);
      deleteCommandNode(pOriginalCommandNode, FALSE);
   }

   // Return result
   END_TRACKING();
   return (iFailureCount == 0 ? OR_SUCCESS : OR_FAILURE);
}


/// Function name  : validateScriptFileCodeArray
// Description     : Validates the codearray variables and properties
// 
// CONST XML_SCRIPT_LAYOUT*  pValidationLayout : [in]     Validation script layout
// CONST XML_SCRIPT_LAYOUT*  pOriginalLayout   : [in]     Original script layout
// ERROR_QUEUE*              pErrorQueue       : [in/out] Error queue
// 
// Return Value   : OR_SUCCESS - Zero errors
//                  OR_FAILURE - One or more errors
// 
OPERATION_RESULT  validateScriptFileCodeArray(CONST XML_SCRIPT_LAYOUT*  pValidationLayout, CONST XML_SCRIPT_LAYOUT*  pOriginalLayout, ERROR_QUEUE*  pErrorQueue)
{
   XML_TREE_NODE     *pValidationIterator,      // Validation script node iterator
                     *pOriginalIterator,        // Original script node iterator
                     *pValidationNode,          // Node being compared from the validation script
                     *pOriginalNode;            // Node being compared from the original script
   OPERATION_RESULT   eResult;                  // Result
   CONST TCHAR       *szVariableName;           // Name of variable being processed
   AVL_TREE          *pOriginalVariables;       // Tree of variables names within original script
   SUGGESTION_RESULT  xTreeItem;                // Used for checking presence of variables
   UINT               iIndex,                   // Current node index
                      iFailureCount;            // Failure count
   // [TRACK]
   TRACK_FUNCTION();

   // Prepare
   pOriginalVariables = createVariableNameTreeByText();
   eResult            = OR_SUCCESS;
   iFailureCount      = 0;
   iIndex             = 0;

   /// [VARIABLES] Iterate through original variables
   for (findNextXMLTreeNode(pOriginalLayout->pVariableNamesBranch, XNR_CHILD, pOriginalIterator); pOriginalIterator; findNextXMLTreeNode(pOriginalIterator, XNR_SIBLING, pOriginalIterator))
   {
      // Add original variable to tree
      getXMLPropertyString(pOriginalIterator, TEXT("val"), szVariableName);
      insertVariableNameIntoAVLTree(pOriginalVariables, szVariableName, NULL);
   }

   /// [VARIABLES: CHECK] Ensure validation variables match orginal
   for (findNextXMLTreeNode(pValidationLayout->pVariableNamesBranch, XNR_CHILD, pValidationIterator); (iFailureCount < iErrorThreshold) AND pValidationIterator; findNextXMLTreeNode(pValidationIterator, XNR_SIBLING, pValidationIterator))
   {
      // Prepare
      getXMLPropertyString(pValidationIterator, TEXT("val"), szVariableName);

      // [CHECK] Ensure variable is present
      if (!findObjectInAVLTreeByValue(pOriginalVariables, (LPARAM)szVariableName, NULL, xTreeItem.asPointer))
      {
         // [ERROR] "Codearray variable mismatch - '%s' was detected in the %s script but not in the %s script"
         generateQueuedError(pErrorQueue, HERE(IDS_VALIDATION_VARIABLE_MISSING), szVariableName, TEXT("validation"), TEXT("original"));
         iFailureCount++;
      }
   }

   // Index variables tree
   performAVLTreeIndexing(pOriginalVariables);

   /// [VARIABLES: CHECK] Ensure original variables match validation
   for (iIndex = 0; (iFailureCount < iErrorThreshold) AND findObjectInAVLTreeByIndex(pOriginalVariables, iIndex, xTreeItem.asPointer); iIndex++)
   {
      // Iterate through all validation variables
      for (findNextXMLTreeNode(pValidationLayout->pVariableNamesBranch, XNR_CHILD, pValidationIterator); pValidationIterator; findNextXMLTreeNode(pValidationIterator, XNR_SIBLING, pValidationIterator))
      {
         // Prepare
         getXMLPropertyString(pValidationIterator, TEXT("val"), szVariableName);

         // [CHECK] Abort search if variable matches
         if (utilCompareStringVariables(szVariableName, xTreeItem.asVariableName->szName))
            break;
      }

      // [CHECK] Was the variable found?
      if (!pValidationIterator)
      {
         // [ERROR] "Codearray variable mismatch - '%s' was detected in the %s script but not in the %s script"
         generateQueuedError(pErrorQueue, HERE(IDS_VALIDATION_VARIABLE_MISSING), xTreeItem.asVariableName->szName, TEXT("original"), TEXT("validation"));
         iFailureCount++;
      }
   }

   /// [VARIABLE COUNT] Check variable count matches
   if (getXMLNodeCount(pOriginalLayout->pVariableNamesBranch) != getXMLNodeCount(pValidationLayout->pVariableNamesBranch))
   {
      // [CHECK] Are any variables missing?
      if (iFailureCount++)
         // [ERROR] "There are %d %s nodes in the %s on line %u of the validation file, but %d in the original"
         generateQueuedError(pErrorQueue, HERE(IDS_VALIDATION_ARRAY_COUNT_MISMATCH), getXMLNodeCount(pValidationLayout->pVariableNamesBranch), TEXT("declaration"), TEXT("variables codearray branch"), pValidationLayout->pVariableNamesBranch->iLineNumber, getXMLNodeCount(pOriginalLayout->pVariableNamesBranch));
      else
      {
         // [ERROR] "This script contains one or more variable names that are distinguished only by case; this is not supported in X-Studio, variable names must be unique."
         generateQueuedError(pErrorQueue, HERE(IDS_VALIDATION_VARIABLES_CASE_INSENSITIVE));
         eResult = OR_ABORTED;
      }
   }
   
   /// [NAME] 
   if (!utilCompareStringVariables(pValidationLayout->pNameNode->szText, pOriginalLayout->pNameNode->szText))
      // [ERROR] "The %s property is '%s', which does not match the original: '%s'"
      generateQueuedError(pErrorQueue, HERE(IDS_VALIDATION_STRING_PROPERTY_MISMATCH), TEXT("name node"), pValidationLayout->pNameNode->szText, pOriginalLayout->pNameNode->szText);
   
   /// [DESCRIPTION] 
   if (lstrlen(pValidationLayout->pDescriptionNode->szText) AND pOriginalLayout->pDescriptionNode->szText AND
       !utilCompareStringVariables(pValidationLayout->pDescriptionNode->szText, pOriginalLayout->pDescriptionNode->szText))
      // [ERROR] "The %s property is '%s', which does not match the original: '%s'"
      generateQueuedError(pErrorQueue, HERE(IDS_VALIDATION_STRING_PROPERTY_MISMATCH), TEXT("description node"), pValidationLayout->pDescriptionNode->szText, pOriginalLayout->pDescriptionNode->szText);
   
   /// [CODEARRAY:NAME]
   findXMLTreeNodeByIndex(pValidationLayout->pCodeArrayNode, CAI_SCRIPT_NAME, pValidationNode);
   findXMLTreeNodeByIndex(pOriginalLayout->pCodeArrayNode,   CAI_SCRIPT_NAME, pOriginalNode);

   if (!validateScriptNodes(pValidationNode, pOriginalNode, TEXT("codearray"), TEXT("scriptname"), pErrorQueue))
      iFailureCount++;

   /// [CODEARRAY:NAME]
   findXMLTreeNodeByIndex(pValidationLayout->pCodeArrayNode, CAI_DESCRIPTION, pValidationNode);
   findXMLTreeNodeByIndex(pOriginalLayout->pCodeArrayNode,   CAI_DESCRIPTION, pOriginalNode);

   if (!validateScriptNodes(pValidationNode, pOriginalNode, TEXT("codearray"), TEXT("description"), pErrorQueue))
      iFailureCount++;

   /// [CODEARRAY:LOADING FLAG]
   findXMLTreeNodeByIndex(pValidationLayout->pCodeArrayNode, CAI_LOADING_FLAG, pValidationNode);
   findXMLTreeNodeByIndex(pOriginalLayout->pCodeArrayNode,   CAI_LOADING_FLAG, pOriginalNode);

   if (!validateScriptNodes(pValidationNode, pOriginalNode, TEXT("codearray"), TEXT("live data"), pErrorQueue))
      iFailureCount++;

   /// [CODEARRAY:COMMAND ID]
   findXMLTreeNodeByIndex(pValidationLayout->pCodeArrayNode, CAI_COMMAND_ID, pValidationNode);
   findXMLTreeNodeByIndex(pOriginalLayout->pCodeArrayNode,   CAI_COMMAND_ID, pOriginalNode);
   
   if (!validateScriptCodeArrayCommandIDNodes(pValidationNode, pOriginalNode, pErrorQueue))
      iFailureCount++;

   // Cleanup and return result
   deleteAVLTree(pOriginalVariables);
   END_TRACKING();
   return (eResult == OR_ABORTED ? OR_ABORTED : (iFailureCount == 0 ? OR_SUCCESS : OR_FAILURE));
}


/// Function name  : validateScriptFileProperties
// Description     : Validates the properties, arguments and variables of two ScriptFiles
// 
// CONST SCRIPT_FILE*  pValidationFile : [in]     Validation script
// CONST SCRIPT_FILE*  pScriptFile     : [in]     Original script
// OPERATION_PROGRESS* pProgress       : [in]     Progress tracker
// ERROR_QUEUE*        pErrorQueue     : [in/out] Error queue
// 
// Return Value   : OR_SUCCESS - Zero errors
//                  OR_FAILURE - One or more errors
// 
OPERATION_RESULT  validateScriptFile(CONST SCRIPT_FILE*  pValidationFile, CONST SCRIPT_FILE*  pScriptFile, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue)
{
   OPERATION_RESULT  eResult;

   // [TRACK]
   TRACK_FUNCTION();

   // Prepare
   eResult = OR_SUCCESS;

   /// [INFO] Validate properties : "Validating the properties of %s script '%s'"
   pushErrorQueue(pErrorQueue, generateDualInformation(HERE(IDS_OUTPUT_VALIDATING_SCRIPT_PROPERTIES), identifyGameVersionString(pValidationFile->eGameVersion), identifyGameFileFilename(pValidationFile)));
   validateScriptFileProperties(pValidationFile, pScriptFile, pErrorQueue);

   /// [INFO] Validate CodeArray : "Validating the CodeArray of %s script '%s'"
   pushErrorQueue(pErrorQueue, generateDualInformation(HERE(IDS_OUTPUT_VALIDATING_SCRIPT_CODEARRAY), identifyGameVersionString(pValidationFile->eGameVersion), identifyGameFileFilename(pValidationFile)));
   eResult = validateScriptFileCodeArray(&pValidationFile->pTranslator->oLayout, &pScriptFile->pTranslator->oLayout, pErrorQueue);

   // [SUCCESS] Validate commands
   if (eResult == OR_SUCCESS)
   {
      /// [INFO] Validate Commands : "Validating the commands within %s script '%s'"
      pushErrorQueue(pErrorQueue, generateDualInformation(HERE(IDS_OUTPUT_VALIDATING_SCRIPT_COMMANDS), identifyGameVersionString(pValidationFile->eGameVersion), identifyGameFileFilename(pValidationFile)));
      eResult = validateScriptFileCommands(pValidationFile->pTranslator, pScriptFile->pTranslator, pValidationFile->eGameVersion, pProgress, pErrorQueue);
   }
   // [ABORTED] Cannot perform validation on this script, due to an error in design.
   else if (eResult == OR_ABORTED)
      // Return SUCCESS to continue validation -- Errors have been generated containin the relevant information.
      eResult = OR_SUCCESS;
   
   // Return result
   END_TRACKING();
   return eResult;
}

/// Function name  : validateScriptFileByText
// Description     : Compares the text of each command in two ScriptFiles
// 
// CONST SCRIPT_FILE*  pValidationFile : [in]     Validation script
// CONST SCRIPT_FILE*  pScriptFile     : [in]     Original script
// OPERATION_PROGRESS* pProgress       : [in]     Progress tracker
// ERROR_QUEUE*        pErrorQueue     : [in/out] Error queue
// 
// Return Value   : OR_SUCCESS - Zero errors
//                  OR_FAILURE - One or more errors
// 
OPERATION_RESULT  validateScriptFileByText(CONST SCRIPT_FILE*  pValidationFile, CONST SCRIPT_FILE*  pScriptFile, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue)
{
   ERROR_STACK*  pError;
   COMMAND  *pOriginalCmd,
            *pValidationCmd;
   UINT      iFailureCount;

   // Prepare
   TRACK_FUNCTION();
   iFailureCount = 0;

   /// [INFO/PROGRESS] Define progress as number of commands.  "Validating the commands within %s script '%s'"
   pushErrorQueue(pErrorQueue, generateDualInformation(HERE(IDS_OUTPUT_VALIDATING_SCRIPT_COMMANDS), identifyGameVersionString(pValidationFile->eGameVersion), identifyGameFileFilename(pValidationFile)));
   updateOperationProgressMaximum(pProgress, getScriptTranslatorOutputCount(pScriptFile->pTranslator));

   /// [COMMAND COUNT]
   if (getScriptTranslatorOutputCount(pValidationFile->pTranslator) != getScriptTranslatorOutputCount(pScriptFile->pTranslator))
   {
      // [ERROR] "There are %d %s commands in the validation file but %d in the original"
      generateQueuedError(pErrorQueue, HERE(IDS_VALIDATION_COMMAND_COUNT_MISMATCH), getScriptTranslatorOutputCount(pValidationFile->pTranslator), TEXT("translated"), getScriptTranslatorOutputCount(pScriptFile->pTranslator));
      iFailureCount++;
   }
   
   /// [COMMAND TEXT]
   for (UINT  iIndex = 0; (iFailureCount < iErrorThreshold) AND findCommandInTranslatorOutput(pScriptFile->pTranslator, iIndex, pOriginalCmd) AND findCommandInTranslatorOutput(pValidationFile->pTranslator, iIndex, pValidationCmd); iIndex++)
   {
      switch (pOriginalCmd->iID)
      {
      //// [COMMENTS] Skip because apostrophes may be different
      //case CMD_COMMENT:
      //case CMD_COMMAND_COMMENT:
      // [SCRIPT CALL] Skip because argument names cannot be properly resolves in the 'Code.Validation' folder when the necessary script hasn't be validated yet
      case CMD_SCRIPT_CALL:
         continue;

      default:
         // [CHECK] Compare command text
         if (!utilCompareStringVariables(pOriginalCmd->szBuffer, pValidationCmd->szBuffer))
         {
            // [COMMENTS] Skip comments with apostrophes in
            if ((pOriginalCmd->iID == CMD_COMMENT OR pOriginalCmd->iID == CMD_COMMAND_COMMENT OR isCommandType(pOriginalCmd, CT_CMD_COMMENT)) AND utilFindCharacterInSet(pOriginalCmd->szBuffer, "'`"))
               continue;
            
            // [ERROR] "Difference in command on line %d, original: '%s' validation: '%s'"
            pushErrorQueue(pErrorQueue, pError = generateDualError(HERE(IDS_VALIDATION_COMMAND_TEXT_MISMATCH), iIndex + 1, pOriginalCmd->szBuffer, pValidationCmd->szBuffer));
            attachTextToError(pError, topErrorStack(pError)->szMessage);
            iFailureCount++;
         }
         break;
      }
   }

   // Return result
   END_TRACKING();
   return (iFailureCount == 0 ? OR_SUCCESS : OR_FAILURE);
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
DWORD   threadprocValidateScriptFile(VOID*  pParameter)
{
   DOCUMENT_OPERATION  *pOperationData;      // Convenience pointer
   OPERATION_RESULT     eResult;             // Operation result, defaults to SUCCESS
   SCRIPT_FILE         *pScriptFile,         // Convenience pointer
                       *pValidationFile;     //
   TCHAR               *szValidationPath,    //
                       *szScriptFolder;      //

   // [TRACKING]
   TRACK_FUNCTION();
   VERBOSE_LIB_COMMAND();
   SET_THREAD_NAME("Script Validation");

   // [CHECK] Ensure parameter exists
   ASSERT(pParameter);
   
   // Prepare
   pOperationData = (DOCUMENT_OPERATION*)pParameter;
   pScriptFile    = (SCRIPT_FILE*)pOperationData->pGameFile;
   eResult        = OR_FAILURE;

   /// Generate validation path
   szValidationPath = utilCreateStringf(MAX_PATH, TEXT("%sCode.Validation\\%s"), szScriptFolder = utilDuplicateFolderPath(pOperationData->szFullPath), PathFindFileName(pOperationData->szFullPath));
   PathRenameExtension(szValidationPath, TEXT(".xml"));

   /// Create validation file
   pValidationFile = createScriptFileByOperation(SFO_TRANSLATION, szValidationPath);

   // [STAGE] Set parsing stage
   ASSERT(getOperationProgressStageID(pOperationData->pProgress) == IDS_PROGRESS_PARSING_SCRIPT);

   /// [GUARD BLOCK]
   __try
   {
      // [INFO] "Searching for %s script '%s'..."
      pushErrorQueue(pOperationData->pErrorQueue, generateDualInformation(HERE(IDS_OUTPUT_LOADING_VALIDATION_SCRIPT), TEXT("original"), PathFindFileName(pOperationData->szFullPath)));
      VERBOSE_SMALL_PARTITION();

      /// [LOAD] Load original script into ScriptFile
      if (!loadGameFileFromFileSystemByPath(getFileSystem(), pScriptFile, NULL, pOperationData->pErrorQueue))
         // [ERROR] "The MSCI script '%s' is unavailable or could not be accessed"
         enhanceLastError(pOperationData->pErrorQueue, ERROR_ID(IDS_SCRIPT_LOAD_IO_ERROR), pOperationData->szFullPath);
      else 
      {
         // [INFO] "Searching for %s script '%s'..."
         pushErrorQueue(pOperationData->pErrorQueue, generateDualInformation(HERE(IDS_OUTPUT_LOADING_VALIDATION_SCRIPT), TEXT("validation"), PathFindFileName(pValidationFile->szFullPath)));
         VERBOSE_SMALL_PARTITION();

         /// [LOAD] Load validation script into ScriptFile
         if (!loadGameFileFromFileSystemByPath(getFileSystem(), pValidationFile, NULL, pOperationData->pErrorQueue))
            // [ERROR] "The MSCI script '%s' is unavailable or could not be accessed"
            enhanceLastError(pOperationData->pErrorQueue, ERROR_ID(IDS_SCRIPT_LOAD_IO_ERROR), pValidationFile->szFullPath);
         
         /// [TRANSLATE] Translate original script
         else if ((eResult = translateScript(pScriptFile, FALSE, pOperationData->hParentWnd, pOperationData->pProgress, pOperationData->pErrorQueue)) != OR_SUCCESS)
            // [ERROR] "Unable the translate the %s script '%s'"
            generateQueuedError(pOperationData->pErrorQueue, HERE(IDS_VALIDATION_TRANSLATION_FAILED), TEXT("original"), identifyGameFileFilename(pScriptFile));
         else 
         {
            // [PROGRESS] Repeat parsing stage
            ASSERT(advanceOperationProgressStage(pOperationData->pProgress) == IDS_PROGRESS_PARSING_SCRIPT);

            /// [TRANSLATE] Translate validation script
            if ((eResult = translateScript(pValidationFile, FALSE, pOperationData->hParentWnd, pOperationData->pProgress, pOperationData->pErrorQueue)) != OR_SUCCESS)
               // [ERROR] "Unable the translate the %s script '%s'"
               generateQueuedError(pOperationData->pErrorQueue, HERE(IDS_VALIDATION_TRANSLATION_FAILED), TEXT("validation"), identifyGameFileFilename(pValidationFile));
            else
            {
               // [PROGRESS] Advance to validation stage
               ASSERT(advanceOperationProgressStage(pOperationData->pProgress) == IDS_PROGRESS_VALIDATING_SCRIPT);
               
#ifndef VALIDATE_BY_TEXT
               /// [VALIDATE] Compare scripts code arrays
               eResult = validateScriptFile(pValidationFile, pScriptFile, pOperationData->pProgress, pOperationData->pErrorQueue);
#else
               /// [VALIDATE] Compare scripts by text
               eResult = validateScriptFileByText(pValidationFile, pScriptFile, pOperationData->pProgress, pOperationData->pErrorQueue);
#endif
            }
         }
      }
   }
   /// [EXCEPTION HANDLER]
   __except (generateQueuedExceptionError(GetExceptionInformation(), pOperationData->pErrorQueue))
   {
      // [ERROR] "An unidentified and unexpected critical error has occurred while validate the script '%s'"
      enhanceLastError(pOperationData->pErrorQueue, ERROR_ID(IDS_EXCEPTION_VALIDATE_SCRIPT_FILE), pOperationData->szFullPath);
      
      // [FAILURE]
      eResult = OR_FAILURE;
   }

   // Cleanup
   deleteScriptFile(pValidationFile);
   utilDeleteStrings(szValidationPath, szScriptFolder);

   // Return result
   VERBOSE_THREAD_COMPLETE("SCRIPT VALIDATION WORKER THREAD COMPLETED");
   closeThreadOperation(pOperationData, eResult);
   END_TRACKING();
   return THREAD_RETURN;
}
