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
// CONST XML_TREE_NODE*  pNode   : [in]  <argument> node
// ERROR_STACK*         &pError  : [out] New Error message, if any, otherwise NULL
// 
// Return Value   : New Argument if successful, otherwise NULL
// 
ARGUMENT*  createArgumentFromNode(CONST XML_TREE_NODE*  pArgumentNode, CONST UINT  iIndex, ERROR_STACK*  &pError)
{
   XML_TREE_NODE    *pNameNode,
                    *pTypeNode;
   ARGUMENT         *pNewArgument;      // Argument being created
   CONST TCHAR      *szName,            // Argument name
                    *szType,            // Argument type
                    *szDescription;     // Argument description
   PARAMETER_SYNTAX  eType;

   // Create new argument
   pNewArgument = utilCreateEmptyObject(ARGUMENT);

   /// [ARGUMENT NODE] Extract name, type, index and description properties
   if (utilCompareString(pArgumentNode->szName, "argument"))
   {
      // Extract argument properties
      getXMLPropertyInteger(pArgumentNode, TEXT("index"), (INT&)pNewArgument->iID);
      getXMLPropertyString(pArgumentNode, TEXT("name"), szName);
      getXMLPropertyString(pArgumentNode, TEXT("type"), szType);
      getXMLPropertyString(pArgumentNode, TEXT("desc"), szDescription);

      /// Set properties
      pNewArgument->szName        = utilDuplicateSimpleString(szName);
      pNewArgument->szDescription = utilDuplicateSimpleString(szDescription);

      // [HACK] Some X3:R scripts specify the 'number' syntax in german: 'Nummer' instead of 'Number'
      if (utilCompareString(szType, "Nummer"))
         pNewArgument->eType = PS_NUMBER;

      /// [CHECK] Lookup argument type
      else if (!findParameterSyntaxByName(szType, pNewArgument->eType))
      {  
         // [ERROR] "The script argument '%s' uses an unknown data type '%s'"
         pError = generateDualError(HERE(IDS_SCRIPT_ARGUMENT_TYPE_NAME_INVALID), szName, szType);
         deleteArgument(pNewArgument);
      }
   }
   /// [CODEARRAY NODE] Extract name and type
   else if (!findXMLTreeNodeByIndex(pArgumentNode, 1, pNameNode) OR !findXMLTreeNodeByIndex(pArgumentNode, 0, pTypeNode) OR !getXMLPropertyString(pNameNode, TEXT("val"), szName) OR !getXMLPropertyInteger(pTypeNode, TEXT("val"), (INT&)eType))
   {
      // [ERROR] "The script argument with index '%d' is missing one or more property %s"
      pError = generateDualError(HERE(IDS_SCRIPT_ARGUMENT_CORRUPT), iIndex, pNameNode AND pTypeNode ? TEXT("values") : TEXT("nodes"));
      deleteArgument(pNewArgument);
   }
   // [CHECK] Ensure parameter syntax is valid
   else if (!isParameterSyntaxValid(eType))
   {  
      // [ERROR] "The script argument with index %d uses an unknown data type with ID %d"
      pError = generateDualError(HERE(IDS_SCRIPT_ARGUMENT_TYPE_ID_INVALID), iIndex, eType);
      deleteArgument(pNewArgument);
   }
   else
   {
      /// Set properties
      pNewArgument->szName        = utilDuplicateSimpleString(szName);
      pNewArgument->szDescription = utilDuplicateSimpleString(NULL);
      pNewArgument->iID           = iIndex;
      pNewArgument->eType         = eType;
   }

   // Return new argument (or NULL if lookup failed)
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
