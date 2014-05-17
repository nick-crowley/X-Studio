//
// Script Translation (XML).cpp : Provides helper functions for traversing the script XML
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

/// Function name  : createCommandNode
// Description     : Create a new CommandNode
// 
// CONST COMMAND_TYPE  eType : [in] Whether command is standard or auxiliary
// XML_TREE_NODE*      pNode : [in] Node contents
// 
// Return Value   : New CommandNode, you are responsible for destroying it
// 
COMMAND_NODE*  createCommandNode(CONST COMMAND_TYPE  eType, XML_TREE_NODE*  pNode)
{
   COMMAND_NODE*  pNewNode;

   // Create new node
   pNewNode = utilCreateEmptyObject(COMMAND_NODE);

   // Set properites
   pNewNode->eType = eType;
   pNewNode->pNode = pNode;

   // Return new node
   return pNewNode;
}


/// Function name  : createParameterNodeTuple
// Description     : Creates a ParameterNodeTuple object
// 
// Return Value   : New ParameterNodeTuple object, you are responsible for destroying it
// 
PARAMETER_NODE_TUPLE*  createParameterNodeTuple()
{
   PARAMETER_NODE_TUPLE*  pTuple;

   // Create new object
   pTuple = utilCreateEmptyObject(PARAMETER_NODE_TUPLE);

   // Return new object
   return pTuple;
}


/// Function name  : deleteCommandNode
// Description     : Destroy a CommandNode
// 
// COMMAND_NODE*  &pNode           : [in] CommandNode to destroy
// BOOL            bIncludeXMLNode : [in] Whether to destroy the underlying XMLTreeNode too
// 
VOID  deleteCommandNode(COMMAND_NODE*  &pNode, BOOL  bIncludeXMLNode)
{
   // Delete underlying node
   if (bIncludeXMLNode)
      deleteXMLTreeNode(pNode->pNode);

   // Delete calling object
   utilDeleteObject(pNode);
}


/// Function name  : deleteParameterNodeTuple
// Description     : Deletes a ParameterNodeTuple object
// 
// PARAMETER_NODE_TUPLE*  &pParameterNodeTuple : [in] ParameterNodeTuple object to destroy
// 
VOID   deleteParameterNodeTuple(PARAMETER_NODE_TUPLE*  &pParameterNodeTuple)
{
   // Delete calling object
   utilDeleteObject(pParameterNodeTuple);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

   /// Function name  : isScriptBranchNonEmptyArray
// Description     : Checks whether a node represents a non-empty array
// 
// CONST XML_TREE_NODE*  pBranch  : [in]  Node to test
// ERROR_STACK*         &pError   : [out] Error message if any, otherwise NULL
// 
// Return Value   : TRUE or FALSE
// 
BOOL  isScriptBranchNonEmptyArray(CONST XML_TREE_NODE*  pBranch, ERROR_STACK*  &pError)
{
   SOURCE_VALUE_TYPE   eNodeType;   // Node type of the variables branch node

   // Prepare
   pError = NULL;

   // [CHECK] Ensure node is a non-empty array
   return getSourceValueType(pBranch, eNodeType, pError) AND (eNodeType == SVT_ARRAY) AND (getXMLNodeCount(pBranch) > 0);
}


/// Function name  : isScriptBranchValid
// Description     : Determines whether node represents an empty or non-empty branch  ie. Integer zero or non-empty array
// 
// CONST XML_TREE_NODE*  pBranch  : [in]  Node to test
// ERROR_STACK*         &pError   : [out] Error message if any, otherwise NULL
// 
// Return Value   : TRUE or FALSE
// 
BOOL  isScriptBranchValid(CONST XML_TREE_NODE*  pBranch, ERROR_STACK*  &pError)
{
   SOURCE_VALUE_TYPE   eNodeType;   // Node type of the variables branch node
   INT                 iNodeValue;
   BOOL                bResult;     // Operation result

   // Prepare
   pError = NULL;

   /// Determine node type
   if (bResult = getSourceValueType(pBranch, eNodeType, pError))
   {
      switch (eNodeType)
      {
      /// [ARRAY] Ensure branch is not empty
      case SVT_ARRAY:      bResult = (getXMLNodeCount(pBranch) > 0);        break;
      /// [INTEGER] Ensure branch is empty
      case SVT_INTEGER:    bResult = (getSourceValueInteger(pBranch, iNodeValue, pError) AND iNodeValue == 0);   break;
      /// [STRING/UNKNOWN] Invalid
      case SVT_STRING:
      case SVT_UNKNOWN:    bResult = FALSE;     break;
      }
   }

   // Return result
   return bResult;
}


/// Function name  : isScriptCommandIDValid
// Description     : Determines whether a script's command ID node is valid
// 
// CONST XML_TREE_NODE*  pNode    : [in]  Node to test
// ERROR_STACK*         &pError   : [out] Error message if any, otherwise NULL
// 
// Return Value   : TRUE or FALSE
// 
BOOL  isScriptCommandIDValid(CONST XML_TREE_NODE*  pNode, ERROR_STACK*  &pError)
{
   SOURCE_VALUE_TYPE   eNodeType;   // Node type of the variables branch node
   CONST TCHAR*        szNodeValue;
   BOOL                bResult;     // Operation result

   // Prepare
   pError = NULL;

   /// Determine node type
   if (bResult = getSourceValueType(pNode, eNodeType, pError))
   {
      switch (eNodeType)
      {
      /// [STRING] Ensure node is not empty
      case SVT_STRING:     bResult = (getSourceValueString(pNode, szNodeValue, pError) AND lstrlen(szNodeValue));   break;
      /// [INTEGER] Valid
      case SVT_INTEGER:    bResult = TRUE;      break;
      /// [ARRAY/UNKNOWN] Invalid
      case SVT_ARRAY:
      case SVT_UNKNOWN:    bResult = FALSE;     break;
      }
   }

   // Return result
   return bResult;
}

/// Function name  : isScriptPropertyNodeNonEmpty
// Description     : Locates a child of the <script> node and ensures it exists
// 
// CONST XML_TREE_NODE*  pStartNode : [in]  The <script> node
// CONST TCHAR*          szNodeName : [in]  Name of property node to locate
// XML_TREE_NODE*       &pStartNode : [out] Property node if found, otherwise NULL
// 
// Return Value   : TRUE/FALSE
// 
BOOL  isScriptPropertyNodeValid(CONST XML_TREE_NODE*  pStartNode, CONST TCHAR*  szNodeName, XML_TREE_NODE*  &pOutput)
{
   /// [CHECK] Ensure presence of node.  FIX: No longer ensures the node has text
   if (!findXMLTreeNodeByName(pStartNode, szNodeName, FALSE, pOutput)) // OR !lstrlen(pOutput->szText))
      // [FAILED]
      pOutput = NULL;

   // Return TRUE if succesful
   return (pOutput != NULL);
}


/// Function name  : isScriptPropertyNodeNonEmpty
// Description     : Locates a child of the <codearray> node and ensures it has a non-empty 'val' property
// 
// CONST XML_TREE_NODE*  pStartNode : [in]  The <codearray> node
// CONST UINT            iIndex     : [in]  Zero-based index of the <codearray> node to locate
// XML_TREE_NODE*       &pStartNode : [out] <sval> node if found, otherwise NULL
// 
// Return Value   : TRUE/FALSE
// 
BOOL  isScriptPropertySValNodeValid(CONST XML_TREE_NODE*  pCodeArray, CONST UINT  iIndex, XML_TREE_NODE*  &pOutput)
{
   CONST TCHAR*  szProperty;

   /// [CHECK] Lookup 'sval' node and ensure 'val' property has text
   if (!findXMLTreeNodeByIndex(pCodeArray, iIndex, pOutput) OR !getXMLPropertyString(pOutput, TEXT("val"), szProperty) OR !lstrlen(szProperty))
      pOutput = NULL;

   // Return TRUE if successful
   return (pOutput != NULL);
}


UINT  getScriptPropertyNameInteger(CONST XML_TREE_NODE*  pNode)
{
   CONST TCHAR*  szOutput;

   // [CHECK] Is this an <sourceval> node?
   if (!utilCompareString(pNode->szName, "sval"))
      /// [NAMED] Return node text, if any
      szOutput = (lstrlen(pNode->szText) ? pNode->szText : NULL);

   /// [SVAL] Return value of 'val' property, if any
   else if (!getXMLPropertyString(pNode, TEXT("val"), szOutput) OR !lstrlen(szOutput))
      szOutput = NULL;

   // Return integer, if possible
   return utilSafeConvertStringToInteger(szOutput);
}


CONST TCHAR*  getScriptPropertyNameString(CONST XML_TREE_NODE*  pNode)
{
   CONST TCHAR*  szOutput;

   // [CHECK] Is this an <sourceval> node?
   if (!utilCompareString(pNode->szName, "sval"))
      /// [NAMED] Return node text, if any
      szOutput = (lstrlen(pNode->szText) ? pNode->szText : NULL);

   /// [SVAL] Return value of 'val' property, if any
   else if (!getXMLPropertyString(pNode, TEXT("val"), szOutput) OR !lstrlen(szOutput))
      szOutput = NULL;

   // Return text
   return szOutput;
}


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : findScriptTranslationNodes
// Description     : Finds the required nodes for script translation
// 
// SCRIPT_TRANSLATOR*  pTranslator : [in] Script translator
// ERROR_STACK*       &pError      : [out] Error message if any, otherwise NULL
// 
// Return Value   : TRUE if successful, otherwise FALSE
// 
BOOL   findScriptTranslationNodes(SCRIPT_TRANSLATOR*  pTranslator, ERROR_STACK*  &pError)
{
   XML_SCRIPT_LAYOUT* pLayout;            // Convenience pointer
   XML_TREE_NODE*     pCommandNode;       // First node in the standard/auxiliary command branches

   // [VERBOSE]
   //VERBOSE("Locating crucial XML-Tree nodes");

   // Prepare
   pLayout = &pTranslator->oLayout;
   pError  = NULL;

   // [CHECK] Ensure XMLTree exists
   if (!pTranslator->pXMLTree OR !getXMLNodeCount(pTranslator->pXMLTree->pRoot))
      // [ERROR] "The underlying XMLTree is missing or empty"
      pError = generateDualError(HERE(IDS_SCRIPT_TREE_EMPTY));
   
   /// [SCRIPT] Find the <script> node, from ROOT
   else if (!findXMLTreeNodeByName(pTranslator->pXMLTree->pRoot, TEXT("script"), TRUE, pLayout->pScriptNode))
      // [ERROR] "Cannot find the mandatory script XML tag <%s>"
      pError = generateDualError(HERE(IDS_SCRIPT_TAG_MISSING), TEXT("script"));

#ifdef BUG_FIX
   /// [CODEARRAY] Find the <codearray> node, from <script>
   else if (!findXMLTreeNodeByIndex(pLayout->pScriptNode, SFI_CODE_ARRAY, pLayout->pCodeArrayNode))
#endif
   /// [CODEARRAY] Find the <codearray> node
   else if (!findXMLTreeNodeByName(pLayout->pScriptNode, TEXT("codearray"), FALSE, pLayout->pCodeArrayNode))   // [FIX] BUG:1022 - 'Albion Prelude scripts with the new <sourceplaintext> tag cannot be not loaded'
      // [ERROR] "Cannot find the mandatory script XML tag <%s>"
      pError = generateDualError(HERE(IDS_SCRIPT_TAG_MISSING), TEXT("codearray"));

   // Replace <codearray> with its first <sval> child, and ensure has 10 children
   else if (!findXMLTreeNodeByIndex(pLayout->pCodeArrayNode, 0, pLayout->pCodeArrayNode) OR getXMLNodeCount(pLayout->pCodeArrayNode) != 10)
      // [ERROR] "The <codearray> tag has %u child nodes, instead of the expected 10"
      pError = generateDualError(HERE(IDS_CODEARRAY_INVALID), getXMLNodeCount(pLayout->pCodeArrayNode));


   /// [NAME] Find the <name> node.  Use <codearray> if missing.
   else if (!isScriptPropertyNodeValid(pLayout->pScriptNode, TEXT("name"), pLayout->pNameNode) AND !isScriptPropertySValNodeValid(pLayout->pCodeArrayNode, CAI_SCRIPT_NAME, pLayout->pNameNode))
      // [ERROR] "Cannot find the %s in either the <%s> XML tag or the equivilent <codearray> element"
      pError = generateDualError(HERE(IDS_SCRIPT_PROPERTY_TAG_MISSING), TEXT("script name"), TEXT("name"));

   /// [DESCRIPTION] Find the <description> node.  Use <codearray> if missing
   else if (!isScriptPropertyNodeValid(pLayout->pScriptNode, TEXT("description"), pLayout->pDescriptionNode) AND !isScriptPropertySValNodeValid(pLayout->pCodeArrayNode, CAI_DESCRIPTION, pLayout->pDescriptionNode))
      // [ERROR] "Cannot find the %s in either the <%s> XML tag or the equivilent <codearray> element"
      pError = generateDualError(HERE(IDS_SCRIPT_PROPERTY_TAG_MISSING), TEXT("description"), TEXT("description"));

   /// [VERSION] Find the <version> node.  Use <codearray> if missing
   else if (!isScriptPropertyNodeValid(pLayout->pScriptNode, TEXT("version"), pLayout->pVersionNode) AND !isScriptPropertySValNodeValid(pLayout->pCodeArrayNode, CAI_VERSION, pLayout->pVersionNode))
      // [ERROR] "Cannot find the %s in either the <%s> XML tag or the equivilent <codearray> element"
      pError = generateDualError(HERE(IDS_SCRIPT_PROPERTY_TAG_MISSING), TEXT("version"), TEXT("version"));

   /// [ENGINE-VERSION] Find the <engineversion> node.  Use <codearray> if missing
   else if (!isScriptPropertyNodeValid(pLayout->pScriptNode, TEXT("engineversion"), pLayout->pEngineVersionNode) AND !isScriptPropertySValNodeValid(pLayout->pCodeArrayNode, CAI_ENGINE_VERSION, pLayout->pEngineVersionNode))
      // [ERROR] "Cannot find the %s in either the <%s> XML tag or the equivilent <codearray> element"
      pError = generateDualError(HERE(IDS_SCRIPT_PROPERTY_TAG_MISSING), TEXT("script game version"), TEXT("engineversion"));

   /// [ARGUMENTS] Find the <arguments> node
   else if (!findXMLTreeNodeByName(pLayout->pScriptNode, TEXT("arguments"), FALSE, pLayout->pArgumentsNode) AND !findXMLTreeNodeByIndex(pLayout->pCodeArrayNode, CAI_ARGUMENTS, pLayout->pArgumentsNode))
      // [ERROR] "Cannot find the %s in either the <%s> XML tag or the equivilent <codearray> element"
      pError = generateDualError(HERE(IDS_SCRIPT_PROPERTY_TAG_MISSING), TEXT("script arguments"), TEXT("arguments"));


   /// [LIVE DATA] Find <live data> node within the <codearray> node
   else if (!findXMLTreeNodeByIndex(pLayout->pCodeArrayNode, CAI_LOADING_FLAG, pLayout->pLiveDataNode))
      // [ERROR] "Could not read the live script data property from element %u of the <codearray> structure"
      pError = generateDualError(HERE(IDS_CODEARRAY_LIVE_DATA_MISSING), CAI_LOADING_FLAG);

   /// [COMMAND ID] Find <commandID> node within the <codearray> node
   else if (!findXMLTreeNodeByIndex(pLayout->pCodeArrayNode, CAI_COMMAND_ID, pLayout->pCommandIDNode))
      // [ERROR] "Could not read the Command ID property from element %u of the <codearray> structure"
      pError = generateDualError(HERE(IDS_CODEARRAY_COMMAND_ID_MISSING), CAI_COMMAND_ID);
   
   // [CHECK] Ensure <commandID> node is valid
   else if (!isScriptCommandIDValid(pLayout->pCommandIDNode, pError))
      // [ERROR] "The Command ID property of the <codearray> structure is invalid"
      pError = generateSafeDualError(pError, HERE(IDS_CODEARRAY_COMMAND_ID_INVALID));
   
   
   /// [VARIABLES BRANCH] Find <variables> branch within the <codearray> node
   else if (!findXMLTreeNodeByIndex(pLayout->pCodeArrayNode, CAI_VARIABLES, pLayout->pVariableNamesBranch))
      // [ERROR] "Cannot find the variables branch in element %u the <codearray> node on line %u"
      pError = generateDualError(HERE(IDS_CODEARRAY_VARIABLES_BRANCH_MISSING), CAI_VARIABLES, pLayout->pCodeArrayNode->iLineNumber);
   
   // Ensure branch is valid
   else if (!isScriptBranchValid(pLayout->pVariableNamesBranch, pError))
      // [ERROR] "The variables branch of the <codearray> node on line %u is present but invalid"
      pError = generateSafeDualError(pError, HERE(IDS_CODEARRAY_VARIABLES_BRANCH_INVALID), pLayout->pVariableNamesBranch->iLineNumber);
   
   /// [STANDARD BRANCH] Find <standard commands> branch within <codearray> node
   else if (!findXMLTreeNodeByIndex(pLayout->pCodeArrayNode, CAI_STANDARD_COMMANDS, pLayout->pStandardCommandsBranch))
      // [ERROR] "Cannot find the standard commands branch in element %u of the <codearray> node on line %u"
      pError = generateDualError(HERE(IDS_CODEARRAY_STANDARD_BRANCH_MISSING), CAI_STANDARD_COMMANDS, pLayout->pCodeArrayNode->iLineNumber);
   
   // Ensure branch is not empty
   else if (!isScriptBranchNonEmptyArray(pLayout->pStandardCommandsBranch, pError))
      // [ERROR] "The standard commands branch of the <codearray> node on line %u is present but invalid"
      pError = generateSafeDualError(pError, HERE(IDS_CODEARRAY_STANDARD_BRANCH_INVALID), pLayout->pStandardCommandsBranch->iLineNumber);

   /// [AUXILIARY BRANCH] Find <auxiliary commands> branch within the <codearray> node
   else if (!findXMLTreeNodeByIndex(pLayout->pCodeArrayNode, CAI_AUXILIARY_COMMANDS, pLayout->pAuxiliaryCommandsBranch))
      // [ERROR] "Cannot find the auxiliary commands branch in element %u of the <codearray> node on line %u"
      pError = generateDualError(HERE(IDS_CODEARRAY_AUXILIARY_BRANCH_MISSING), CAI_AUXILIARY_COMMANDS, pLayout->pCodeArrayNode->iLineNumber);
   
   // Ensure branch is valid
   else if (!isScriptBranchValid(pLayout->pVariableNamesBranch, pError))
      // [ERROR] "The auxiliary commands branch of the <codearray> node on line %u is present but invalid"
      pError = generateSafeDualError(pError, HERE(IDS_CODEARRAY_AUXILIARY_BRANCH_INVALID), pLayout->pAuxiliaryCommandsBranch->iLineNumber);
   
   else 
   {
      // [ARGUMENTS] Find the codeArray arguments branch
      findXMLTreeNodeByIndex(pLayout->pCodeArrayNode, CAI_ARGUMENTS, pLayout->pArgumentsBranch);

      // [SIGNATURE] Find the <signature> node (if any)
      findXMLTreeNodeByName(pLayout->pScriptNode, TEXT("signature"), FALSE, pLayout->pSignatureNode);

      /// Initialise Standard iterator to the first command
      findNextXMLTreeNode(pLayout->pStandardCommandsBranch, XNR_CHILD, pCommandNode);
      pTranslator->pStandardIterator = createCommandNode(CT_STANDARD, pCommandNode);

      /// Initialise Auxiliary iterator if required
      if (findNextXMLTreeNode(pLayout->pAuxiliaryCommandsBranch, XNR_CHILD, pCommandNode))
         pTranslator->pAuxiliaryIterator = createCommandNode(CT_AUXILIARY, pCommandNode);
   }

   // Return TRUE if there were no errors
   return (pError == NULL);
}



/// Function name  : findNextCommandNode
// Description     : Create a new CommandNode containing the next appropriate node from the auxiliary or standard
//                    commands branch.
// 
// SCRIPT_TRANSLATOR*  pTranslator   : [in/out] ScriptFile translator
// COMMAND_NODE*       pCommandNode  : [out]    New CommandNode containing the resultant node, if found, otherwise NULL. You are responsible for destroying it.
// ERROR_STACK*       &pError        : [out]    New error message, if any, otherwise NULL. You are responsible for destroying it
//  
// Return Value   : TRUE if node was found, FALSE if no more are available (or there was an error)
// 
BOOL  findNextCommandNode(SCRIPT_TRANSLATOR*  pTranslator, COMMAND_NODE*  &pOutput, ERROR_STACK*  &pError)
{
   // Prepare
   pOutput = NULL;
   pError  = NULL;

   // [CHECK] Return FALSE when there are no more standard nodes
   //if (pScriptFile->pStandardIterator == NULL)
   //   return FALSE;

   /// Determine whether the current auxiliary command (if any are left) is associated with the current standard command
   if (pTranslator->pAuxiliaryIterator AND isMatchingAuxiliaryCommand(pTranslator, pTranslator->pStandardIterator, pTranslator->pAuxiliaryIterator, pError))
   {
      // Output the current auxiliary node and then advance the auxiliary iterator
      pOutput = pTranslator->pAuxiliaryIterator;
      findNextCommandNodeByType(pTranslator, CT_AUXILIARY, pTranslator->pAuxiliaryIterator);
   }   
   /// Otherwise return the current standard command (if any are left and there were no errors checking the auxiliary command)
   else if (!pError AND pTranslator->pStandardIterator)
   {
      // Output the current standard node then advance the standard iterator
      pOutput = pTranslator->pStandardIterator;
      findNextCommandNodeByType(pTranslator, CT_STANDARD, pTranslator->pStandardIterator);
   }
   
   // Return TRUE if a node was returned without error
   return (pOutput != NULL AND pError == NULL);
}


/// Function name  : findNextCommandNodeByType
// Description     : Create a new CommandNode from the next XMLTreeNode in the standard/auxiliary command branch.
//                     If no more nodes are available from that branch then the return is FALSE.
// 
// SCRIPT_TRANSLATOR*  pTranslator : [in/out] ScriptFile translator
// CONST COMMAND_TYPE  eType       : [in]     Type of command to retrieve
// COMMAND_NODE*      &pOutput     : [out]    New CommandNode if successful, otherwise NULL. You are responsible for destroying it
// 
// Return Value   : TRUE if successful, FALSE otherwise
// 
BOOL  findNextCommandNodeByType(SCRIPT_TRANSLATOR*  pTranslator, CONST COMMAND_TYPE  eType, COMMAND_NODE*  &pOutput)
{
   XML_TREE_NODE    *pCurrentNode,  // Current node, as an XMLTreeNode
                    *pNextNode;     // Next tree node, if found
   // Prepare
   // [REM] pOutput = NULL;   // The 'output' node is the iterator itself during a call from findNextCommandNode

   // Set the start node, determined by the iterator for the appropriate command type
   switch (eType)
   {
   case CT_AUXILIARY:  pCurrentNode = pTranslator->pAuxiliaryIterator->pNode; break;
   case CT_STANDARD:   pCurrentNode = pTranslator->pStandardIterator->pNode;  break;
   }

   /// Create a new CommandNode from the next node (if any)
   if (pCurrentNode AND findNextXMLTreeNode(pCurrentNode, XNR_SIBLING, pNextNode))
      pOutput = createCommandNode(eType, pNextNode);
   else
      pOutput = NULL;

   // Return TRUE if node is found
   return (pOutput != NULL);
}


/// Function name  : findNextParameterNodeTuple
// Description     : Find the next node tuple from within a CommandNode
///                     NOTE: This reads the type from the indicated by current position and the value from the
///                             subsequent node
// 
// CONST COMMAND_NODE*  pCommandNode : [in]     CommandNode containing the node-tuples, only used for error reporting
// XML_TREE_NODE*       pCurrentNode : [in/out] First node of the parameter tuple
// COMMAND_NODE_TUPLE*  pNodeTuple   : [in/out] Data to hold the parameter tuple
// ERROR_STACK*        &pError       : [out]    New error message if any, otherwise NULL. You are responsible for destroying it
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BOOL  findNextParameterNodeTuple(CONST COMMAND_NODE*  pCommandNode, XML_TREE_NODE*  &pCurrentNode, PARAMETER_NODE_TUPLE*  pNodeTuple, ERROR_STACK*  &pError)
{
   // [CHECK] Ensure first node is valid
   ASSERT(pCurrentNode != NULL);

   // Prepare
   pError = NULL;
   utilZeroObject(pNodeTuple, PARAMETER_NODE_TUPLE);

   /// Store the first node (TYPE)
   pNodeTuple->pType = pCurrentNode;
   
   /// Find the second node (VALUE)
   if (!findNextXMLTreeNode(pCurrentNode, XNR_SIBLING, pNodeTuple->pValue))
   {  // [ERROR] "There are insufficient <sourcevalue> tags to define a parameter tuple for the CommandNode created from the <sourcevalue> tag on line %u"
      pError = generateDualError(HERE(IDS_SCRIPT_PARAMETER_TUPLE_INVALID), pCommandNode->pNode->iLineNumber);
   }
   else  // Change the 'CurrentNode' to the second node
      pCurrentNode = pNodeTuple->pValue;

   // Return TRUE if there were no errors
   return (pError == NULL);
}



/// Function name  : getCommandNodeCommentID
// Description     : Retrieve the CommandID of the original command from a CommandNode representing a command comment
// 
// CONST COMMAND_NODE*  pCommandNode : [in]  CommandNode representing a command comment
// UINT                &iCommandID   : [out] ID of the command
// ERROR_STACK*        &pError       : [out] Error if any, otherwise NULL
// 
// Return Value   : TRUE if successful, FALSE otherwise
// 
BOOL  getCommandNodeCommentID(CONST COMMAND_NODE*  pCommandNode, UINT&  iCommandID, ERROR_STACK*  &pError)
{
   XML_TREE_NODE*  pIDNode;     // SubNode containing the command ID

   // [CHECK]
   ASSERT(pCommandNode->eType == CT_AUXILIARY);

   // Prepare
   pError = NULL;

   /// Retrieve node containing the ID then extract it
   if (!findXMLTreeNodeByIndex(pCommandNode->pNode, CSTI_SUB_COMMAND_ID, pIDNode) OR 
       !getSourceValueInteger(pIDNode, (INT&)iCommandID, pError))
      // [ERROR] "Cannot locate the original command ID of the commented command"
      pError = generateSafeDualError(pError, HERE(IDS_SCRIPT_COMMAND_COMMENT_ID_MISSING));

   // Return TRUE if there were no errors
   return (pError == NULL);
}


/// Function name  : getCommandNodeID
// Description     : Retrieve the CommandID from a CommandNode representing a standard or auxiliary command
// 
// CONST COMMAND_NODE*  pCommandNode : [in]  CommandNode representing a standard or auxiliary command
// UINT                &iCommandID   : [out] ID of the command
// ERROR_STACK*        &pError       : [out] Error if any, otherwise NULL
// 
// Return Value   : TRUE if successful, FALSE otherwise
// 
BOOL  getCommandNodeID(CONST COMMAND_NODE*  pCommandNode, UINT&  iCommandID, ERROR_STACK*  &pError)
{
   XML_TREE_NODE*  pIDNode;     // SubNode containing the ID
   UINT            iIndex;      // Index of subnode, above

   // Prepare
   pError = NULL;
   iIndex = (pCommandNode->eType == CT_STANDARD ? SSTI_COMMAND_ID : ASTI_COMMAND_ID);

   /// Retrieve node containing the ID then extract it
   if (!findXMLTreeNodeByIndex(pCommandNode->pNode, iIndex, pIDNode) OR 
       !getSourceValueInteger(pIDNode, (INT&)iCommandID, pError))
      // [ERROR] "Cannot locate the ID of the command from child node %u of %u"
      pError = generateSafeDualError(pError, HERE(IDS_SCRIPT_COMMAND_ID_MISSING), iIndex, getXMLNodeCount(pCommandNode->pNode));
   
   // Return TRUE if there were no errors
   return (pError == NULL);
}


/// Function name  : getCommandNodeIndex
// Description     : Retrieve the index of the standard command associated with an auxiliary command
// 
// CONST COMMAND_NODE*  pCommandNode : [in]  CommandNode representing an Auxiliary command
// UINT                &iIndex       : [out] Desired index
// ERROR_STACK*        &pError       : [out] Error if any, otherwise NULL
// 
// Return Value   : TRUE if successful, FALSE if there were errors
// 
BOOL  getCommandNodeIndex(CONST COMMAND_NODE*  pCommandNode, UINT&  iIndex, ERROR_STACK*  &pError)
{
   XML_TREE_NODE*  pIndexNode;     // SubNode containing the index

   // [CHECK] Ensure node is an Auxiliary command
   ASSERT(pCommandNode->eType == CT_AUXILIARY);

   // Prepare
   pError = NULL;

   /// Retrieve node containing the index then extract it
   if (!findXMLTreeNodeByIndex(pCommandNode->pNode, ASTI_COMMAND_INDEX, pIndexNode)
       OR !getSourceValueInteger(pIndexNode, (INT&)iIndex, pError))
      // [ERROR] "Cannot determine the reference index of the auxiliary command in the <sourcevalue> node on line %u from child node #%u"
      pError = generateSafeDualError(pError, HERE(IDS_SCRIPT_COMMAND_INDEX_MISSING), pCommandNode->pNode->iLineNumber, iIndex + 1);
   
   // Return TRUE if there were no errors
   return (pError == NULL);
}


/// Function name  : getSourceValueInteger
// Description     : Retrieve an integer from an XMLTreeNode representing a <sourcevalue> tag
// 
// CONST XML_TREE_NODE*  pNode   : [in]     
// INT                  &iOutput : [in/out] 
// ERROR_STACK*         &pError  : [out]    
// 
// Return Value   : TRUE if successful, FALSE if there were errors
// 
BOOL  getSourceValueInteger(CONST XML_TREE_NODE*  pNode, INT&  iOutput, ERROR_STACK*  &pError)
{
   CONST TCHAR*  szType;   // Node type

   // Prepare
   pError = NULL;

   // [CHECK] Ensure node is an integer
   if (!getXMLPropertyString(pNode, TEXT("type"), szType) OR !utilCompareString(szType, "int"))
      // [ERROR] "Cannot retrieve the integer from the <sourcevalue> node on line %u because it's type is '%s' instead of 'int'""
      pError = generateDualError(HERE(IDS_SCRIPT_SOURCEVALUE_NOT_INTEGER), pNode->iLineNumber, szType);
   
   /// Retrieve integer value
   else if (!getXMLPropertyInteger(pNode, TEXT("val"), iOutput))
      // [ERROR] "Cannot retrieve the value of the <sourcevalue> node on line %u because the 'value' property is missing""
      pError = generateDualError(HERE(IDS_SCRIPT_SOURCEVALUE_NOT_FOUND), pNode->iLineNumber);
   
   // Return TRUE if there were no errors
   return (pError == NULL);
}


/// Function name  : getSourceValueString
// Description     : Retrieve a string from an XMLTreeNode representing a <sourcevalue> tag
// 
// CONST XML_TREE_NODE*  pNode    : [in]     
// CONST TCHAR*         &szOutput : [in/out] 
// ERROR_STACK*         &pError   : [out]    
// 
// Return Value   : TRUE if successful, FALSE if there were errors
// 
BOOL  getSourceValueString(CONST XML_TREE_NODE*  pNode, CONST TCHAR*  &szOutput, ERROR_STACK*  &pError)
{
   CONST TCHAR*  szType;   // Node type

   // Prepare
   pError = NULL;

   // [CHECK] Ensure node is a string
   if (!getXMLPropertyString(pNode, TEXT("type"), szType) OR !utilCompareString(szType, "string"))
      // [ERROR] "Cannot retrieve the string from the <sourcevalue> node on line %u because it's type is '%s' instead of 'string'""
      pError = generateDualError(HERE(IDS_SCRIPT_SOURCEVALUE_NOT_STRING), pNode->iLineNumber, szType);
   
   /// Retrieve string
   else if (!getXMLPropertyString(pNode, TEXT("val"), szOutput))
      // [ERROR] "Cannot retrieve the value of the <sourcevalue> node on line %u because the 'value' property is missing""
      pError = generateDualError(HERE(IDS_SCRIPT_SOURCEVALUE_NOT_FOUND), pNode->iLineNumber);
   
   // Return TRUE if there were no errors
   return (pError == NULL);
}


/// Function name  : getSourceValueType
// Description     : Determine the type of value contained within a <sourcevalue> node
// 
// CONST XML_TREE_NODE*  pNode   : [in]  Node to examine
// SOURCE_VALUE_TYPE    &eOutput : [out] Value type identifier, or SVT_UNKNOWN if undetermined
// ERROR_STACK*         &pError  : [out] New error message, if any, otherwise NULL
// 
// Return Value   : TRUE if successful, FALSE if type was missing or unrecognised
// 
BOOL  getSourceValueType(CONST XML_TREE_NODE*  pNode, SOURCE_VALUE_TYPE&  eOutput, ERROR_STACK*  &pError)
{
   CONST TCHAR*  szType;   // Node type string

   // Prepare
   pError  = NULL;
   eOutput = SVT_UNKNOWN;

   // Determine node type
   if (!getXMLPropertyString(pNode, TEXT("type"), szType))
      // [ERROR] "Cannot retrieve the value type identifier from the <sourcevalue> node on line %u"
      pError = generateDualError(HERE(IDS_SCRIPT_SOURCEVALUE_TYPE_MISSING), pNode->iLineNumber);
   
   // [STRING]
   else if (utilCompareString(szType, "string"))
      eOutput = SVT_STRING;
   
   // [INTEGER]
   else if (utilCompareString(szType, "int"))
      eOutput = SVT_INTEGER;
   
   // [ARRAY]
   else if (utilCompareString(szType, "array"))
      eOutput = SVT_ARRAY;

   else
      // [ERROR] "Unrecognised value type identifier in the <sourcevalue> node on line %u"
      pError = generateDualError(HERE(IDS_SCRIPT_SOURCEVALUE_TYPE_INVALID), pNode->iLineNumber);
   
   // Return FALSE if there were errors (the type was missing/unrecognised)
   return (pError == NULL);
}


/// Function name  : identifyParameterNodeType
// Description     : Identify the meaning of the current child node of CommandNode (ie. whether it's a tuple, a 
//                      parameter count or a single node eg. comment text or return object)
// 
// CONST COMMAND*          pCommand        : [in] COMMAND for the CommandNode being translated
// CONST PARAMETER_COUNT*  pParameterCount : [in] Parameter counts for default, infix and postfix
// CONST UINT              iNodeIndex      : [in] Zero based index of the current child node
// CONST UINT              iParameterIndex : [in] Zero based index of the parameter being processed
// 
// Return Value   : Meaning of the next child node
// 
PARAMETER_NODE_TYPE  identifyParameterNodeType(CONST COMMAND*  pCommand, CONST PARAMETER_COUNT*  pParameterCount, CONST UINT  iNodeIndex, CONST UINT  iParameterIndex)
{
   PARAMETER_NODE_TYPE   eOutput;              // Node meaning
   PARAMETER_SYNTAX      eParameterSyntax;     // Output syntax

   switch (pCommand->iID)
   {
   /// [EXPRESSION] ReturnObject, ParameterCount, Parameter 0, ... , Parameter n, Parameter Count, Parameter 0, ... , Parameter n
   case CMD_EXPRESSION:
      /// [COMMAND COMMENT EXPRESSIONS] ReturnObject followed by Parameter count and parameter tuples
      if (pCommand->iFlags INCLUDES CT_CMD_COMMENT)
      {
         switch (iNodeIndex)
         {
         // [RETURN-OBJECT] Single node
         case CSTI_FIRST_PARAMETER:      eOutput = PNT_SINGLE_PARAMETER;  break;
         // [PARAMETER COUNT] Single node
         case CSTI_FIRST_PARAMETER + 1:  eOutput = PNT_PARAMETER_COUNT;   break;
         // [PARAMETER] Node tuples
         default:                        eOutput = PNT_PARAMETER_TUPLE;   break;
         }
      }
      /// [NORMAL EXPRESSIONS] Split into postfix and infix parameters
      else 
      {
         switch (iNodeIndex)
         {
         // [RETURN-OBJECT]
         case SSTI_FIRST_PARAMETER:      eOutput = PNT_SINGLE_PARAMETER;  break;
         // [POST-FIX COUNT]
         case SSTI_FIRST_PARAMETER + 1:  eOutput = PNT_POSTFIX_COUNT;     break;

         default:
            // [POST-FIX PARAMETER]
            if (iParameterIndex < pParameterCount->iPostfixCount + 1)   // +1 because first parameter is ParameterCount
               eOutput = PNT_POSTFIX_EXPRESSION;
            // [INFIX COUNT]
            else if (iNodeIndex == 3 + (2 * pParameterCount->iPostfixCount))   // +3 for commandID, returnObject + postfix count. 2 for each postfix tuple
               eOutput = PNT_INFIX_COUNT;
            // [IN-FIX PARAMETER]
            else
               eOutput = PNT_INFIX_EXPRESSION;
            break;
         }
      }
      break;

   /// [SCRIPT CALL] ScriptName, ReturnObject, ReferenceObject, Argument Count, Argument 0, ... , Argument n
   case CMD_CALL_SCRIPT_VAR_ARGS:
      /// [COMMAND COMMENT SCRIPT-CALLS] These have the 'associated command' and 'base command ID' nodes, thereby offsetting each index by two
      if (pCommand->iFlags INCLUDES CT_CMD_COMMENT)
         switch (iNodeIndex)
         {
         // [SCRIPT-NAME, RETURN-OBJECT]
         case CSTI_FIRST_PARAMETER:      // ScriptName
         case CSTI_FIRST_PARAMETER + 1:  // ReturnObject
            eOutput = PNT_SINGLE_PARAMETER; 
            break;

         // [REFERENCE-OBJECT, PARAMETERS]
         case CSTI_FIRST_PARAMETER + 2:  // ReferenceObject
         default:                        // Subsequent script parameters
            eOutput = PNT_PARAMETER_TUPLE; 
            break;

         // [ARGUMENT COUNT]
         case CSTI_FIRST_PARAMETER + 4:  // Argument Count
            eOutput = PNT_PARAMETER_COUNT; 
            break;
         }
      /// [STANDARD SCRIPT-CALLS]
      else 
         switch (iNodeIndex)
         {
         // [SCRIPT-NAME, RETURN-OBJECT]
         case SSTI_FIRST_PARAMETER:      // ScriptName
         case SSTI_FIRST_PARAMETER + 1:  // ReturnObject
            eOutput = PNT_SINGLE_PARAMETER; 
            break;

         // [REFERENCE-OBJECT, PARAMETERS]
         case SSTI_FIRST_PARAMETER + 2:  // ReferenceObject
         default:                        // Subsequent script parameters
            eOutput = PNT_PARAMETER_TUPLE; 
            break;

         // [ARGUMENT COUNT]
         case SSTI_FIRST_PARAMETER + 4:  // Argument Count
            eOutput = PNT_PARAMETER_COUNT; 
            break;
         }
      
      break;

   /// [DEFAULT] Determine from the command syntax whether the next parameter uses one or two nodes
   default:
      if (!findParameterSyntaxByPhysicalIndex(pCommand->pSyntax, iParameterIndex, eParameterSyntax))
      {  // [ERROR] Syntax should always be found for normal commands
         ASSERT(FALSE);
         eOutput = PNT_SINGLE_PARAMETER;
      }
      else 
         // Determine whether syntax requires 1 node or 2
         eOutput = identifyParameterSyntaxType(eParameterSyntax);
      break;
   }

   // Return syntax
   return  eOutput;
}


/// Function name  : isMatchingAuxiliaryCommand
// Description     : Determines whether an auxiliary command is associated with the specified standard command
// 
// CONST SCRIPT_TRANSLATOR*  pTranslator  : [in]  ScriptFile translator
// CONST COMMAND_NODE*  pStandardNode  : [in]  Standard command to test against
// CONST COMMAND_NODE*  pAuxiliaryNode : [in]  Auxiliary command to test
// ERROR_STACK*        &pError         : [out] New error message if any, otherwise NULL
//  
// Return Value   : TRUE if the commands match, FALSE if not or there was an error
// 
BOOL  isMatchingAuxiliaryCommand(CONST SCRIPT_TRANSLATOR*  pTranslator, CONST COMMAND_NODE*  pStandardNode, CONST COMMAND_NODE*  pAuxiliaryNode, ERROR_STACK*  &pError)
{
   UINT  iIndex;     // Index of the standard command associated with the input auxiliary command
   
   // [CHECK] Ensure nodes are the correct type
   ASSERT((pStandardNode ? pStandardNode->eType == CT_STANDARD : TRUE) AND pAuxiliaryNode->eType == CT_AUXILIARY);

   // Prepare
   pError = NULL;

   // Attempt to extract the index from the auxiliary command node
   if (!getCommandNodeIndex(pAuxiliaryNode, iIndex, pError))
      return FALSE;     // No need to further enhance the error

   // [SPECIAL CASE] Allow auxiliary commands following the last standard command
   if (pStandardNode == NULL AND iIndex == getXMLNodeCount(pTranslator->oLayout.pStandardCommandsBranch))
      return TRUE;

   /// Compare the auxiliary command's index with the standard command's index
   return (iIndex == pStandardNode->pNode->iIndex);
}


/// Function name  : verifyCommandChildNodeCount
// Description     : Check the number of XMLTreeNodes in a CommandNode is appropriate for the specified COMMAND
// 
// CONST COMMAND*  pCommand    : [in] COMMAND to verify
// CONST UINT      iChildNodes : [in] Number of child nodes
// ERROR_STACK*   &pError      : [out] New error message, if any, otherwise NULL. You are responsible for destroying it
// 
// Return Value   : TRUE if verified, FALSE if not. If FALSE then pError will be valid.
// 
BOOL  verifyCommandChildNodeCount(COMMAND*  pCommand, CONST UINT  iChildNodes, ERROR_STACK*  &pError)
{
   //CONST COMMAND_SYNTAX*  pOldSyntax;     // Old command syntax
   PARAMETER_SYNTAX       eSyntax;        // Syntax of the current parameter being calculated
   UINT                   iCorrectCount;  // Correct number of nodes

   // Prepare
   iCorrectCount = NULL;
   pError        = NULL;

   // Examine command ID
   switch (pCommand->iID)
   {
   /// [EXPRESSION / SCRIPT CALL] Can't calculate the appropriate number of nodes for these
   case CMD_EXPRESSION:
   case CMD_CALL_SCRIPT_VAR_ARGS:
      return TRUE;

   /// [DELAYED COMMAND] Manually check the appropriate counts - two variations
   case CMD_DELAYED_COMMAND:
      if (iChildNodes == 13 OR iChildNodes == 15)        // 13 = 4 argument version.  15 = 5 argument version, however I never parse the 5th argument, for simplicity's sake
         return TRUE;
      break;

   /// [SET WING COMMAND] Manually check (ignore) for two hidden arguments
   case CMD_SET_WING_COMMAND:
      if (iChildNodes == 9 OR iChildNodes == 13)         // 9 = syntax version, 13 = two extra hidden parameters
         return TRUE;
      break;

   /// [GET OBJECT NAME ARRAY] Ignore last solo node
   case CMD_GET_OBJECT_NAME_ARRAY:
      if (iChildNodes == 4 OR iChildNodes == 5)          // 4 = syntax version, 5 = extra hidden node
         return TRUE;
      break;
   }
   
   // [STANDARD] Set initial Count to ONE for the CommandID
   if (isCommandType(pCommand, CT_STANDARD))
      iCorrectCount = 1;
   
   // [COMMAND COMMENTS] Set initial Count to THREE for the CommandID + ReferenceIndex + Original Command ID
   else if (isCommandType(pCommand, CT_CMD_COMMENT))
      iCorrectCount = 3;

   // [AUXILIARY] Set initial Count to TWO for the CommandID + ReferenceIndex
   else
      iCorrectCount = 2;

   /// Caclulate correct total by determining whether each parameter requires 1 or 2 nodes
   for (UINT iIndex = 0; findParameterSyntaxByPhysicalIndex(pCommand->pSyntax, iIndex, eSyntax); iIndex++)
   {
      switch (identifyParameterSyntaxType(eSyntax))
      {
      case PNT_SINGLE_PARAMETER:    iCorrectCount++;      break;
      case PNT_PARAMETER_TUPLE:     iCorrectCount += 2;   break;
      }
   }

   /// [CHECK] Verify correct node count
   if (iCorrectCount != iChildNodes)
   {
      // [SPECIAL CASE] Is this a trading command compiled using the older syntax, which doesn't have "exclude array"?
      if ((iCorrectCount == iChildNodes + 2) AND isCommandTradingSearch(pCommand) AND findCommandSyntaxByID(pCommand->iID, GV_REUNION, pCommand->pSyntax, pError))
         /// [OLD VERSION] Attempt to verify using the old syntax
         return verifyCommandChildNodeCount(pCommand, iChildNodes, pError);
      
      // [ERROR] "The command should have %u child nodes but %u were detected"
      pError = generateDualError(HERE(IDS_SCRIPT_COMMAND_NODE_COUNT_INCORRECT), iCorrectCount, iChildNodes);
   }
   
   // Return TRUE if there was no error (ie. verified successfully)
   return (pError == NULL);
}

