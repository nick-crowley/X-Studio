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



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////


/// Function name  : generateScriptFromOutputTree
// Description     : Generates and fills a ScriptFile's output buffer from the contents of it's output tree
// 
// SCRIPT_FILE*  pScriptFile : [in/out] ScriptFile containing the output tree representing the MSCI script
// ERROR_QUEUE*  pErrorQueue : [in/out] Error messages, if any
// 
// Return Value   : TRUE
// 
BOOL  generateScriptFromOutputTree(SCRIPT_FILE*  pScriptFile)
{
   TEXT_STREAM*  pOutputStream;     // XML output stream

   // [VERBOSE]
   TRACK_FUNCTION();
   VERBOSE("Performing recursive tree flattening for script '%s'", identifyScriptName(pScriptFile));

   // Create stream object
   pOutputStream = createTextStream(32 * 1024);

   /// Add schema tags
   appendStringToTextStreamf(pOutputStream, TEXT("<?xml version=\"1.0\" standalone=\"yes\"?>\r\n"));
   appendStringToTextStreamf(pOutputStream, TEXT("<?xml-stylesheet href=\"x2script.xsl\" type=\"text/xsl\"?>\r\n"));

   /// Flatten XMLTree
   generateTextStreamFromXMLTree(pScriptFile->pGenerator->pXMLTree, pOutputStream);

   /// Copy output to ScriptFile
   pScriptFile->szOutputBuffer = utilDuplicateString(pOutputStream->szBuffer, pOutputStream->iBufferUsed);
   pScriptFile->iOutputSize    = pOutputStream->iBufferUsed;

   // Cleanup and return TRUE
   deleteTextStream(pOutputStream);
   END_TRACKING();
   return TRUE;
}


/// Function name  : generateOutputTreeDataSection
// Description     : Builds the non-executable portions of the XMLTree representing an MSCI script, such as script properties, script arguments 
//                      and the text displayed when the script is opening using a web-browser
// 
// HWND                hCodeEdit   : [in]     CodeEdit window handle
// CONST SCRIPT_FILE*  pScriptFile : [in]     ScriptFile containing the script properties and arguments
// XML_TREE*           pTree       : [in/out] XMLTree representing the MSCI script
// XML_SCRIPT_LAYOUT*  pLayout     : [in/out] Helper object containing pointers to the important structural elements of the XMLTree
// 
// Return Value   : TRUE
// 
BOOL  generateOutputTreeDataSection(HWND  hCodeEdit, CONST SCRIPT_FILE*  pScriptFile, XML_TREE*  pTree, XML_SCRIPT_LAYOUT*  pLayout)
{
   XML_TREE_NODE*  pNode;           // Argument-node currently being createds
   GAME_STRING*    pLookupString;   // Lookup string for resolving argument-type
   ARGUMENT*       pArgument;       // Argument currently being processed

   // [VERBOSE]
   TRACK_FUNCTION();
   VERBOSE("Generating XML-Tree data section for script '%s'", identifyScriptName(pScriptFile));

   /// Create new node for each script property
   appendStringNodeToXMLTree(pTree, pLayout->pScriptNode, TEXT("name"),           pScriptFile->szScriptname);
   appendIntegerNodeToXMLTree(pTree, pLayout->pScriptNode, TEXT("version"),       pScriptFile->iVersion);
   appendIntegerNodeToXMLTree(pTree, pLayout->pScriptNode, TEXT("engineversion"), (INT)calculateEngineVersionFromGameVersion(pScriptFile->eGameVersion));
   appendStringNodeToXMLTree(pTree, pLayout->pScriptNode, TEXT("description"),    pScriptFile->szDescription);

   // Create <arguments> nodes
   pLayout->pArgumentsNode = appendStringNodeToXMLTree(pTree, pLayout->pScriptNode, TEXT("arguments"), NULL);

   /// Iterate through the ScriptFile's ARGUMENT list
   for (UINT iIndex = 0; findArgumentInScriptFileByIndex(pScriptFile, iIndex, pArgument); iIndex++)
   {
      // Create <argument> node and retrieve string for argument's parameter syntax
      pNode = createXMLTreeNodeFromString(pLayout->pArgumentsNode, TEXT("argument"), NULL);
      findGameStringByID(pArgument->eType, GPI_PARAMETER_TYPES, pLookupString);

      // Add properties
      appendPropertyToXMLTreeNode(pNode, createXMLPropertyFromInteger(TEXT("index"), iIndex + 1));
      appendPropertyToXMLTreeNode(pNode, createXMLPropertyFromString(TEXT("name"), pArgument->szName));
      appendPropertyToXMLTreeNode(pNode, createXMLPropertyFromString(TEXT("type"), pLookupString->szText)); 
      appendPropertyToXMLTreeNode(pNode, createXMLPropertyFromString(TEXT("desc"), pArgument->szDescription));
   
      /// Attach <argument> to <arguments> node
      appendChildToXMLTreeNode(pTree, pLayout->pArgumentsNode, pNode);
   }

   /// Generate <sourcetext> node
   generateOutputTreeSourceText(hCodeEdit, pScriptFile, pTree, pLayout);

   // Return TRUE
   END_TRACKING();
   return TRUE;
}


/// Function name  : generateOutputTreeSourceText
// Description     : Builds the non-executable portions of the XMLTree representing an MSCI script, such as script properties, script arguments 
//                      and the text displayed when the script is opening using a web-browser
// 
// HWND                hCodeEdit   : [in]     CodeEdit window handle
// CONST SCRIPT_FILE*  pScriptFile : [in]     ScriptFile containing the script properties and arguments
// XML_TREE*           pTree       : [in/out] XMLTree representing the MSCI script
// XML_SCRIPT_LAYOUT*  pLayout     : [in/out] Helper object containing pointers to the important structural elements of the XMLTree
// 
// Return Value   : TRUE
// 
BOOL  generateOutputTreeSourceText(HWND  hCodeEdit, CONST SCRIPT_FILE*  pScriptFile, XML_TREE*  pTree, XML_SCRIPT_LAYOUT*  pLayout)
{
   XML_TREE_NODE*  pLineNode;       // <line> node currently being created
   CODEOBJECT*     pCodeObject;
   COMMAND*        pCommand;        // Command currently being processed
   UINT            iIndentation;    // Indentation of current line
   TCHAR          *szLineText,      // 'External' copy of current line text
                  *szIndicator;     // Line indentation string
   CONST TCHAR    *szScriptCall;

   // [VERBOSE]
   TRACK_FUNCTION();
   VERBOSE("Generating XML-Tree <sourcetext> node for script '%s'", identifyScriptName(pScriptFile));

   // Prepare
   szIndicator = utilCreateEmptyString(256);

   // Create <sourcetext> node
   pLayout->pSourceTextNode = appendStringNodeToXMLTree(pTree, pLayout->pScriptNode, TEXT("sourcetext"), NULL);

   /// Iterate through the INPUT list
   for (UINT iLine = 0; findCommandInGeneratorInput(pScriptFile->pGenerator, iLine, pCommand); iLine++)  
   {
      // Create <line> node
      pLineNode = createXMLTreeNodeFromString(pLayout->pSourceTextNode, TEXT("line"), NULL);

      /// [LINE NUMBER] Add linenr="###" property
      StringCchPrintf(szIndicator, 256, TEXT("%03u"), iLine + 1);
      appendPropertyToXMLTreeNode(pLineNode, createXMLPropertyFromString(TEXT("linenr"), szIndicator));

      // Lookup line identation
      iIndentation   = CodeEdit_GetLineIndentation(hCodeEdit, iLine);
      szIndicator[0] = NULL;

      // Generate indentation indicator text
      for (UINT  i = 0; (i < iIndentation) AND (i < 42); i++)
         StringCchCat(szIndicator, 256, TEXT("&#160;"));
      
      /// [INDENTATION] Add indent="xxx" property
      appendPropertyToXMLTreeNode(pLineNode, createXMLPropertyFromString(TEXT("indent"), szIndicator));

      /// [INTERRUPTABLE] Add interruptable="@" property
      if (isCommandInterruptable(pCommand->pSyntax))
         appendPropertyToXMLTreeNode(pLineNode, createXMLPropertyFromString(TEXT("interruptable"), TEXT("@")));

      // Parse line text
      pCodeObject = createCodeObject(pCommand->szBuffer);

      /// Generate <text> and <var> tags
      while (findNextCodeObject(pCodeObject))
      {
         // Generate external version of CodeObject
         generateConvertedString(pCodeObject->szText, SPC_SCRIPT_DISPLAY_TO_EXTERNAL, szLineText);

         // Examine CodeObject
         switch (pCodeObject->eClass)
         {
         /// [VARIABLE/CONSTANT/LABEL/NUMBER] Output <var>
         case CO_NULL:
         case CO_LABEL:
         case CO_VARIABLE:
         case CO_NUMBER:
         case CO_CONSTANT:
         case CO_ENUMERATION:
            appendChildToXMLTreeNode(pTree, pLineNode, createXMLTreeNodeFromString(pLineNode, TEXT("var"), utilEither(szLineText, pCodeObject->szText)));
            break;

         /// [TEXT/WHITESPACE/OPERATOR] Output <text>
         case CO_KEYWORD:
         case CO_OPERATOR:
         case CO_WHITESPACE:
         case CO_WORD:
            appendChildToXMLTreeNode(pTree, pLineNode, createXMLTreeNodeFromString(pLineNode, TEXT("text"), utilEither(szLineText, pCodeObject->szText)));
            break;

         /// [COMMENT] Output as <comment>
         case CO_COMMENT:
            appendChildToXMLTreeNode(pTree, pLineNode, createXMLTreeNodeFromString(pLineNode, TEXT("comment"), utilEither(szLineText, pCodeObject->szText)));
            break;

         /// [STRING] Output as <text> or <call>
         case CO_STRING:
            // [CHECK] Is this command a ScriptCall?  Is this string the script-name?
            if (isCommandType(pCommand, CT_SCRIPTCALL) AND pCodeObject->iLength > 2 AND findScriptCallTargetInCommand(pCommand, szScriptCall) AND utilCompareStringVariablesN(&pCodeObject->szText[1], szScriptCall, pCodeObject->iLength - 2))
               /// [CALL] Output script name
               appendChildToXMLTreeNode(pTree, pLineNode, createXMLTreeNodeFromString(pLineNode, TEXT("call"), szScriptCall));
            else
               // [FAILED] Output as text
               appendChildToXMLTreeNode(pTree, pLineNode, createXMLTreeNodeFromString(pLineNode, TEXT("text"), utilEither(szLineText, pCodeObject->szText)));
            break;
         }

         // Cleanup
         utilSafeDeleteString(szLineText);
      }

      /// Attach <line> to <sourcetext> node
      appendChildToXMLTreeNode(pTree, pLayout->pSourceTextNode, pLineNode);

      // Cleanup
      deleteCodeObject(pCodeObject);
   }

   // Return TRUE
   utilDeleteString(szIndicator);
   END_TRACKING();
   return TRUE;
}


/// Function name  : generateOutputTreeStandardCommands
// Description     : Builds the 'Standard Commands' CodeArray branch from a ScriptFile's list of successfully generated standard commands
// 
// CONST SCRIPT_FILE*  pScriptFile : [in]     ScriptFile containing the list of successfully generated standard commands
// OPERATION_PROGRESS* pProgress   : [in/out] Progress tracking object
// XML_TREE*           pTree       : [in/out] XMLTree representing the MSCI script
// XML_SCRIPT_LAYOUT*  pLayout     : [in/out] Helper object containing pointers to the important structural elements of the XMLTree
// 
// Return Value   : TRUE
// 
BOOL  generateOutputTreeStandardCommands(SCRIPT_FILE*  pScriptFile, OPERATION_PROGRESS*  pProgress, XML_TREE*  pTree, XML_SCRIPT_LAYOUT*  pLayout)
{
   XML_TREE_NODE*  pCommandNode;    // Array node containing the command's 'parameter nodes'
   PARAMETER*      pParameter;      // Command PARAMETER list iterator
   COMMAND*        pCommand;        // Command being created
   UINT            iInfixIndex;     // 
   
   // [VERBOSE]
   TRACK_FUNCTION();
   VERBOSE("Generating XML-Tree standard commands branch for script '%s'", identifyScriptName(pScriptFile));

   // Create STANDARD COMMANDS branch
   pLayout->pStandardCommandsBranch = appendSourceValueArrayNodeToXMLTree(pTree, pLayout->pCodeArrayNode, getListItemCount(pScriptFile->pGenerator->pOutputStream->pStandardList));
   
   /// Iterate through STANDARD OUTPUT list
   for (UINT iIndex = 0; findCommandInGeneratorOutput(pScriptFile->pGenerator, CT_STANDARD, iIndex, pCommand); iIndex++)
   {
      // Update progress object, if present
      if (pProgress)
         updateOperationProgressValue(pProgress, iIndex);

      // Create 'command array' node to contain the command, but specify the size later on
      pCommandNode = appendSourceValueArrayNodeToXMLTree(pTree, pLayout->pStandardCommandsBranch, NULL);
         
      // Add Command ID node
      appendSourceValueIntegerNodeToXMLTree(pTree, pCommandNode, pCommand->iID);

      // Examine COMMAND
      switch (pCommand->iID)
      {
      /// [NORMAL] Output each PARAMETER as a single node or a node tuple
      default:
         // Iterate through each PARAMETER and add to XML Tree
         for (UINT  iSubIndex = 0; findParameterInCommandByIndex(pCommand, PT_DEFAULT, iSubIndex, pParameter); iSubIndex++)
            appendParameterToXMLTree(pScriptFile, pCommand, pParameter, pTree, pCommandNode);
         break;

      /// [SCRIPT-CALL] -- Normal parameters followed by an argument count and a sequence of argument tuples
      case CMD_SCRIPT_CALL:
         // Iterate through first THREE PARAMETERs and add to XML Tree
         for (UINT  iSubIndex = 0; iSubIndex < 3 AND findParameterInCommandByIndex(pCommand, PT_DEFAULT, iSubIndex, pParameter); iSubIndex++)
            appendParameterToXMLTree(pScriptFile, pCommand, pParameter, pTree, pCommandNode);
         
         // Add argument count in single node
         appendSourceValueIntegerNodeToXMLTree(pTree, pCommandNode, pCommand->pParameterArray->iCount - 3);

         // [ARGUMENTS] Iterate through remaining PARAMETERs (if any) and add to XML tree
         for (UINT  iSubIndex = 3; findParameterInCommandByIndex(pCommand, PT_DEFAULT, iSubIndex, pParameter); iSubIndex++)
            appendParameterToXMLTree(pScriptFile, pCommand, pParameter, pTree, pCommandNode);
         break;

      /// [EXPRESSION] -- THESE WILL BE BY FAR THE MOST COMPLEX
      case CMD_EXPRESSION:
         // [RETURN OBJECT] Output as a single node
         findParameterInCommandByIndex(pCommand, PT_DEFAULT, 0, pParameter);
         appendParameterToXMLTree(pScriptFile, pCommand, pParameter, pTree, pCommandNode);

         // [POSTFIX PARAMETER COUNT] Single node
         appendSourceValueIntegerNodeToXMLTree(pTree, pCommandNode, getArrayItemCount(pCommand->pPostfixParameterArray));
         
         // [POSTFIX PARAMETERS] Node tuples
         for (UINT  iSubIndex = 0; findParameterInCommandByIndex(pCommand, PT_POSTFIX, iSubIndex, pParameter); iSubIndex++)
            appendParameterToXMLTree(pScriptFile, pCommand, pParameter, pTree, pCommandNode);
         
         // [INFIX PARAMETER COUNT] Single node
         appendSourceValueIntegerNodeToXMLTree(pTree, pCommandNode, getArrayItemCount(pCommand->pParameterArray) - 1); 

         // [INFIX PARAMETERS] Single nodes : Negative values are indicies into the PostFix array, positive values are operators
         for (UINT  iSubIndex = 1; findParameterInCommandByIndex(pCommand, PT_DEFAULT, iSubIndex, pParameter); iSubIndex++)         // Item 0 is the ReturnObject
         {
            // [UNARY OPERATOR] Output value and 'Unary operator' bit-flag
            if (pParameter->eType == DT_OPERATOR AND isUnaryOperator((OPERATOR_TYPE)pParameter->iValue))
               appendSourceValueIntegerNodeToXMLTree(pTree, pCommandNode, pParameter->iValue WITH UGC_UNARY_OPERATOR);
            
            // [BINARY OPERATOR] Output value only
            else if (pParameter->eType == DT_OPERATOR)
               appendSourceValueIntegerNodeToXMLTree(pTree, pCommandNode, pParameter->iValue);

            // [OPERAND] Output an a negative 1-based index into the POSTFIX array
            else if (findPostfixParameterIndexByID(pCommand, pParameter->iID, iInfixIndex))
               appendSourceValueIntegerNodeToXMLTree(pTree, pCommandNode, -(INT)iInfixIndex - 1);
         }
         break;
      }

      // Set the 'array size' property of the 'Command node' 
      setXMLPropertyInteger(pCommandNode, TEXT("size"), getXMLNodeCount(pCommandNode));
   }

   // Return TRUE
   END_TRACKING();
   return TRUE;
}


/// Function name  : generateOutputTreeAuxiliaryCommands
// Description     : Builds the 'Auxiliary Commands' CodeArray branch from a ScriptFile's list of successfully generated auxiliary commands
// 
// CONST SCRIPT_FILE*  pScriptFile : [in]     ScriptFile containing the list of successfully generated auxiliary commands
// OPERATION_PROGRESS* pProgress   : [in/out] Progress tracking object
// XML_TREE*           pTree       : [in/out] XMLTree representing the MSCI script
// XML_SCRIPT_LAYOUT*  pLayout     : [in/out] Helper object containing pointers to the important structural elements of the XMLTree
// 
// Return Value   : TRUE
// 
BOOL  generateOutputTreeAuxiliaryCommands(SCRIPT_FILE*  pScriptFile, OPERATION_PROGRESS*  pProgress, XML_TREE*  pTree, XML_SCRIPT_LAYOUT*  pLayout)
{
   XML_TREE_NODE*  pCommandNode;  // Array node containing the command's '
   PARAMETER*      pParameter;    // Command PARAMETER list iterator
   COMMAND*        pCommand;      // Command being created
   
   // [VERBOSE]
   TRACK_FUNCTION();
   VERBOSE("Generating XML-Tree auxiliary commands branch for script '%s'", identifyScriptName(pScriptFile));

   // Create AUXILIARY COMMANDS branch
   pLayout->pAuxiliaryCommandsBranch = appendSourceValueArrayNodeToXMLTree(pTree, pLayout->pCodeArrayNode, getListItemCount(pScriptFile->pGenerator->pOutputStream->pAuxiliaryList));
   
   /// Iterate through AUXILIARY OUTPUT 
   for (UINT iIndex = 0; findCommandInGeneratorOutput(pScriptFile->pGenerator, CT_AUXILIARY, iIndex, pCommand); iIndex++)
   {
      // Update progress object, if present
      if (pProgress)
         updateOperationProgressValue(pProgress, iIndex + getListItemCount(pScriptFile->pGenerator->pOutputStream->pStandardList));

      // Create 'command array' node to contain the command, but specify the size later on
      pCommandNode = appendSourceValueArrayNodeToXMLTree(pTree, pLayout->pAuxiliaryCommandsBranch, NULL);
         
      /// Add Reference index 
      appendSourceValueIntegerNodeToXMLTree(pTree, pCommandNode, pCommand->iReferenceIndex);
     
      // [COMMAND COMMENT] Manually specify the 'COMMAND COMMENT' CommandID
      if (isCommandType(pCommand, CT_CMD_COMMENT))
         appendSourceValueIntegerNodeToXMLTree(pTree, pCommandNode, CMD_COMMAND_COMMENT);
      
      /// Add Command ID
      appendSourceValueIntegerNodeToXMLTree(pTree, pCommandNode, pCommand->iID);
      
      /// Iterate through each PARAMETER and add to XML Tree
      for (UINT  iSubIndex = 0; findParameterInCommandByIndex(pCommand, PT_DEFAULT, iSubIndex, pParameter); iSubIndex++)
         // Convert parameter to the correct node(s) type based on syntax and whether command is a CommandComment
         appendParameterToXMLTree(pScriptFile, pCommand, pParameter, pTree, pCommandNode);

      // Set the 'array size' property of the 'Command node' 
      setXMLPropertyInteger(pCommandNode, TEXT("size"), getXMLNodeCount(pCommandNode));
   }

   // Return TRUE
   END_TRACKING();
   return TRUE;
}

/// Function name  : generateOutputTree
// Description     : Generates the ScriptFile XMLTree from the list of successfully generated COMMANDs
// 
// CONST SCRIPT_FILE*  pScriptFile : [in/out] ScriptFile containing the COMMANDS to generate the tree from
// OPERATION_PROGRESS* pProgress   : [in/out] Progress object
// ERROR_QUEUE*        pErrorQueue : [in/out] Empty error queue
// 
// Return Value   : TRUE
// 
BOOL  generateOutputTree(HWND  hCodeEdit, SCRIPT_FILE*  pScriptFile, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue)
{
   XML_TREE           *pTree;              // Convenience pointer for tree being created
   XML_SCRIPT_LAYOUT  *pLayout;            // Convenience pointer for layout
   XML_TREE_NODE      *pArgumentNode;      // Node within the Arguments code-array branch containing the argument currently being processed
   VARIABLE_NAME      *pVariableName;      // ScriptFile's VARIABLE_NAME list iterator
   ARGUMENT           *pArgument;          // ScriptFile's ARGUMENT list iterator

   // Prepare
   TRACK_FUNCTION();
   pTree   = pScriptFile->pGenerator->pXMLTree = createXMLTree();
   pLayout = &pScriptFile->pGenerator->oLayout;
   utilZeroObject(pLayout, XML_SCRIPT_LAYOUT);

   // [INFO] "Generating XML for MSCI script '%s'"
   VERBOSE("Generating XML-Tree for script '%s'", identifyScriptName(pScriptFile));
   pushErrorQueue(pErrorQueue, generateDualInformation(HERE(IDS_OUTPUT_GENERATING_XML_TREE), identifyScriptName(pScriptFile)));

   // [PROGRESS] Set new stage and define the maximum as the total number of commands
   if (pProgress)
   {
      ASSERT(advanceOperationProgressStage(pProgress) == IDS_PROGRESS_GENERATING_SCRIPT);
      updateOperationProgressMaximum(pProgress, getListItemCount(pScriptFile->pGenerator->pOutputStream->pStandardList) + getListItemCount(pScriptFile->pGenerator->pOutputStream->pAuxiliaryList));
   }

   // Create <script> node
   pLayout->pScriptNode = appendStringNodeToXMLTree(pTree, pTree->pRoot, TEXT("script"), NULL);

   /// [NON-EXECUTABLE] Add script properties and arguments in XML tags
   generateOutputTreeDataSection(hCodeEdit, pScriptFile, pTree, pLayout);
   
   /// [CODE-ARRAY] Add script properties as CodeArray elements
   pLayout->pCodeArrayNode = appendStringNodeToXMLTree(pTree, pLayout->pScriptNode, TEXT("codearray"), NULL);    // Create <codearray>
   pLayout->pCodeArrayNode = appendSourceValueArrayNodeToXMLTree(pTree, pLayout->pCodeArrayNode, 10);            // Create 10 element <sval> within

   // Add CodeArray script properties (Name, Version, Description, EngineVersion, LiveData)
   appendSourceValueStringNodeToXMLTree(pTree,  pLayout->pCodeArrayNode, pScriptFile->szScriptname);
   appendSourceValueIntegerNodeToXMLTree(pTree, pLayout->pCodeArrayNode, calculateEngineVersionFromGameVersion(pScriptFile->eGameVersion));
   appendSourceValueStringNodeToXMLTree(pTree,  pLayout->pCodeArrayNode, pScriptFile->szDescription);
   appendSourceValueIntegerNodeToXMLTree(pTree, pLayout->pCodeArrayNode, pScriptFile->iVersion);
   appendSourceValueIntegerNodeToXMLTree(pTree, pLayout->pCodeArrayNode, pScriptFile->bLiveData ? UGC_LIVE_SCRIPT : NULL);

   /// [VARIABLES] Output each variable (without the $) in ascending ID order
   if (getScriptGeneratorVariablesCount(pScriptFile->pGenerator) > 0)
      pLayout->pVariableNamesBranch = appendSourceValueArrayNodeToXMLTree(pTree, pLayout->pCodeArrayNode, getScriptGeneratorVariablesCount(pScriptFile->pGenerator));
   else
      // [NONE] Output NULL
      pLayout->pVariableNamesBranch = appendSourceValueIntegerNodeToXMLTree(pTree, pLayout->pCodeArrayNode, NULL);
   
   // Iterate through the Generator's Arguments + Variables list
   for (UINT iIndex = 0; findVariableNameInGeneratorByIndex(pScriptFile->pGenerator, iIndex, pVariableName); iIndex++)
      // Add to the Variables Branch
      appendSourceValueStringNodeToXMLTree(pTree, pLayout->pVariableNamesBranch, pVariableName->szName);
   
   /// [STANDARD COMMANDS] Convert each command in the STANDARD GENERATED list into nodes
   generateOutputTreeStandardCommands(pScriptFile, pProgress, pTree, pLayout);

   /// [ARGUMENTS] Add each argument (with the $) in ascending ID order
   if (getScriptFileArgumentCount(pScriptFile) > 0)
      pLayout->pArgumentsBranch = appendSourceValueArrayNodeToXMLTree(pTree, pLayout->pCodeArrayNode, getScriptFileArgumentCount(pScriptFile));
   else
      // [NONE] Output NULL
      pLayout->pArgumentsBranch = appendSourceValueIntegerNodeToXMLTree(pTree, pLayout->pCodeArrayNode, NULL);

   // Add each of the ScriptFile's Arguments to the Arguments Branch
   for (UINT iIndex = 0; findArgumentInScriptFileByIndex(pScriptFile, iIndex, pArgument); iIndex++)
   {
      // Create 'argument node'
      pArgumentNode = appendSourceValueArrayNodeToXMLTree(pTree, pLayout->pArgumentsBranch, 2);
      
      // Output raw ParameterType and Argument Description
      appendSourceValueIntegerNodeToXMLTree(pTree, pArgumentNode, pArgument->eType);
      appendSourceValueStringNodeToXMLTree(pTree, pArgumentNode, pArgument->szDescription);
   }

   /// [AUXILIARY COMMANDS] Convert each command in the AUXILIARY GENERATED list into nodes
   generateOutputTreeAuxiliaryCommands(pScriptFile, pProgress, pTree, pLayout);

   /// [COMMAND ID] Output as a string if present, otherwise NULL
   if (lstrlen(pScriptFile->szCommandID) AND isStringNumeric(pScriptFile->szCommandID))
      // [STRING] Output command name
      appendSourceValueStringNodeToXMLTree(pTree, pLayout->pCodeArrayNode, pScriptFile->szCommandID);
   else
      // [NUMBER/NONE] Output command ID
      appendSourceValueIntegerNodeToXMLTree(pTree, pLayout->pCodeArrayNode, lstrlen(pScriptFile->szCommandID) ? utilConvertStringToInteger(pScriptFile->szCommandID) : NULL);
   
   // Return TRUE
   END_TRACKING();
   return TRUE;
}



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////


