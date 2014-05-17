//
// Arguments.cpp : Script arguments
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       CREATION / DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createArgumentFromNode
// Description     : Create a new Argument from a script node
// 
// CONST XML_TREE_NODE*      pArgumentNode  : [in]  <argument> node -OR- <codearray> argument node
// CONST XML_SCRIPT_LAYOUT*  pLayout        : [in]  Script layout 
// CONST UINT                iIndex         : [in]  Argument index
// ERROR_STACK*             &pError         : [out] New Error message, if any, otherwise NULL
// 
// Return Value   : New Argument if successful, otherwise NULL
// 
ARGUMENT*  createArgumentFromNode(CONST XML_TREE_NODE*  pArgumentNode, CONST XML_SCRIPT_LAYOUT*  pLayout, CONST UINT  iIndex, ERROR_STACK*  &pError)
{
   XML_TREE_NODE    *pArgumentBranchNode,   // CodeArray node that is equivilent to <argument> node
                    *pDescriptionNode,      // CodeArray subnode containing the argument description
                    *pNameNode,             // CodeArray variables branch subnode containing the argument name
                    *pTypeNode;             // CodeArray subnode containing the argument type 
   ARGUMENT         *pNewArgument;          // Argument being created
   CONST TCHAR      *szName = 0,            // Argument name
                    *szDescription = 0;     // Argument description
   //PARAMETER_SYNTAX  eType = PS_VARIABLE;   // Argument type

   // Create new argument
   pNewArgument = utilCreateEmptyObject(ARGUMENT);
   
   /// [ARGUMENT NODE] Extract name, index, description properties
   if (utilCompareString(pArgumentNode->szName, "argument"))
   {
      // Extract name/description/index from <argument> node
      if (!getXMLPropertyInteger(pArgumentNode, TEXT("index"), (INT&)pNewArgument->iID) OR
          !getXMLPropertyString(pArgumentNode, TEXT("name"), szName) OR
          !getXMLPropertyString(pArgumentNode, TEXT("desc"), szDescription))
          // [ERROR] "The script argument '%s' (index %d) is missing one or more property"
          pError = generateDualError(HERE(IDS_SCRIPT_ARGUMENT_CORRUPT), szName, iIndex+1);

      // Extract type from <codearray> branch node
      else if (!findXMLTreeNodeByIndex(pLayout->pArgumentsBranch, iIndex, pArgumentBranchNode) OR 
               !findXMLTreeNodeByIndex(pArgumentBranchNode, 0, pTypeNode) OR 
               !getXMLPropertyInteger(pTypeNode, TEXT("val"), (INT&)pNewArgument->eType))
         // [ERROR] "The script argument '%s' (index %d) has no type indicator in the codearray"
         pError = generateDualError(HERE(IDS_SCRIPT_ARGUMENT_CODEARRAY_CORRUPT), szName, iIndex+1);
   }
   /// [CODEARRAY BRANCH NODE] Extract type/description from node
   else
   {
      // Extract type from first subnode
      if (!findXMLTreeNodeByIndex(pArgumentNode, 0, pTypeNode) OR !getXMLPropertyInteger(pTypeNode, TEXT("val"), (INT&)pNewArgument->eType))
         // [ERROR] "The script argument '%s' (index %d) has no type indicator in the codearray"
         pError = generateDualError(HERE(IDS_SCRIPT_ARGUMENT_CODEARRAY_CORRUPT), szName, iIndex+1);

      else 
      {
         // Extract description (if any) from second subnode
         if (!findXMLTreeNodeByIndex(pArgumentNode, 1, pDescriptionNode) OR !getXMLPropertyString(pDescriptionNode, TEXT("val"), szDescription))
            szDescription = TEXT("");

         // Lookup name in separate variables branch
         if (!findXMLTreeNodeByIndex(pLayout->pVariableNamesBranch, iIndex, pNameNode) OR !getXMLPropertyString(pNameNode, TEXT("val"), szName))
            // [ERROR] "The script argument with index %d has no associated variable name in the codearray"
            pError = generateDualError(HERE(IDS_SCRIPT_ARGUMENT_NAME_MISSING), iIndex+1);

         // Set ID from index
         pNewArgument->iID = iIndex+1;
      }
   }
      
   /// [CHECK] Ensure parameter syntax is valid
   if (!pError && !isParameterSyntaxValid(pNewArgument->eType))
      // [ERROR] "The script argument '%s' (index %d) uses an unknown data type with ID '%d'"
      pError = generateDualError(HERE(IDS_SCRIPT_ARGUMENT_TYPE_INVALID), szName, iIndex+1, pNewArgument->eType);

   // [SUCCESS] Set properties
   if (!pError)
   {
      pNewArgument->szName        = utilDuplicateSimpleString(szName);
      pNewArgument->szDescription = utilDuplicateSimpleString(szDescription);
   }
   else
      deleteArgument(pNewArgument);

   // Return argument or NULL
   return pNewArgument;
}


/// Function name  : createArgumentFromName
// Description     : Creates an empty ARGUMENT from NAME and DESCRIPTION, used by the document property pages when generating a new argument
//
// CONST TCHAR*  szName        : [in] Argument name
// CONST TCHAR*  szDescription : [in] Argument description
// 
// Return type : new ARGUMENT, you are responsible for destroying it
BearScriptAPI
ARGUMENT*   createArgumentFromName(CONST TCHAR*  szName, CONST TCHAR*  szDescription)
{
   ARGUMENT*   pNewArgument;

   // Create new argument
   pNewArgument = utilCreateEmptyObject(ARGUMENT);

   /// Set NAME and DESCRIPTION
   pNewArgument->szName        = utilDuplicateSimpleString(szName);
   pNewArgument->szDescription = utilDuplicateSimpleString(szDescription);

   // Return new object
   return pNewArgument;
}


/// Function name  : createArgumentTreeByID
// Description     : Creates an ARGUMENT tree sorted by ID
// 
// Return Value   : New AVLTree, you are responsible for destroying it
// 
AVL_TREE*  createArgumentTreeByID()
{
   // Create a tree of Arguments organised by ID
   return createAVLTree(extractArgumentTreeNode, deleteArgumentTreeNode, createAVLTreeSortKey(AK_ID, AO_ASCENDING), NULL, NULL);
}


/// Function name  : deleteArgument
// Description     : Destroys an ARGUMENT object
// 
// ARGUMENT*  &pArgument : [in] ARGUMENT to destroy
//
VOID   deleteArgument(ARGUMENT*  &pArgument)
{
   // Delete strings
   utilSafeDeleteStrings(pArgument->szName, pArgument->szDescription);

   // Delete calling object
   utilDeleteObject(pArgument);
}


/// Function name  : deleteArgumentTreeNode
// Description     : Deletes a Argument within an AVLTree Node
// 
// LPARAM  pData : [in] Reference to a Argument pointer
// 
VOID   deleteArgumentTreeNode(LPARAM  pData)
{
   // Delete node contents
   deleteArgument((ARGUMENT*&)pData);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                             HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : extractArgumentTreeNode
// Description     : Extract the desired property from a given TreeNode containing an Argument
// 
// LPARAM                   pNode      : [in] ARGUMENT* : Node data containing an Argument
// CONST AVL_TREE_SORT_KEY  eProperty  : [in] Property to retrieve
// 
// Return Value   : Value of the specified property or NULL if the property is unsupported
// 
LPARAM  extractArgumentTreeNode(LPARAM  pNode, CONST AVL_TREE_SORT_KEY  eProperty)
{
   ARGUMENT*  pArgument;    // Convenience pointer
   LPARAM     xOutput;          // Property value output

   // Prepare
   pArgument = (ARGUMENT*)pNode;

   // Examine property
   switch (eProperty)
   {
   case AK_ID:       xOutput = (LPARAM)pArgument->iID;          break;
   case AK_TEXT:     xOutput = (LPARAM)pArgument->szName;       break;
   default:          xOutput = NULL;                            break;
   }

   // Return property value
   return xOutput;
}


/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                             FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////
