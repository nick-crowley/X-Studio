//
// Variable Dependency Tree.cpp : Represents the local/global variables in a MSCI script
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

/// Function name  : createVariableDependency
// Description     : Create a new VariableDependency object
// 
// CONST TCHAR*          szVariableName : [in] Name of the variable
// CONST VARIABLE_TYPE   eType          : [in] Whether variable is local or global
// CONST VARIABLE_USAGE  eUsage         : [in] Whether variable is being read or written
// 
// Return Value   : New dependency object, you are responsible for destroying it
// 
BearScriptAPI 
VARIABLE_DEPENDENCY*  createVariableDependency(CONST TCHAR*  szVariableName, CONST VARIABLE_TYPE  eType, CONST VARIABLE_USAGE  eUsage)
{
   VARIABLE_DEPENDENCY*  pVariableDependency;

   // Create new dependency
   pVariableDependency = utilCreateEmptyObject(VARIABLE_DEPENDENCY);

   // Set properties
   StringCchCopy(pVariableDependency->szVariableName, 128, szVariableName);
   pVariableDependency->eType  = eType;
   pVariableDependency->eUsage = eUsage;

   // Return
   return pVariableDependency;
}


/// Function name  : createVariableDependencyTreeByText
// Description     : Creates a VariableDependency tree sorted by script name
// 
// Return Value   : New AVLTree, you are responsible for destroying it
// 
BearScriptAPI
AVL_TREE*  createVariableDependencyTreeByText()
{
   // Create tree of VariableDependency sorted by TYPE and then NAME
   return createAVLTree(extractVariableDependencyNode, deleteVariableDependencyNode, createAVLTreeSortKey(AK_TEXT, AO_DESCENDING), createAVLTreeSortKeyEx(AK_TYPE, AO_ASCENDING, AP_INTEGER), NULL);
}


/// Function name  : deleteVariableDependencyNode
// Description     : Deletes a VariableDependency within an AVLTree Node
// 
// LPARAM  pVariableDependency : VARIABLE_DEPENDENCY*& : [in] Reference to a VariableDependency to destroy
// 
VOID   deleteVariableDependencyNode(LPARAM  pVariableDependency)
{
   // Delete node contents
   utilDeleteObject((VARIABLE_DEPENDENCY*&)pVariableDependency);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : extractVariableDependencyNode
// Description     : Extract the desired property from a given TreeNode containing a VariableDependency
// 
// LPARAM                   pNodeData  : [in] VARIABLE_DEPENDENCY* : Node data representing a VariableDependency
// CONST AVL_TREE_SORT_KEY  eProperty  : [in] Property to retrieve
// 
// Return Value   : Value of the specified property or NULL if the property is unsupported
// 
LPARAM  extractVariableDependencyNode(LPARAM  pNodeData, CONST AVL_TREE_SORT_KEY  eProperty)
{
   VARIABLE_DEPENDENCY*  pVariableDependency;    // Convenience pointer
   LPARAM                xOutput;              // Property value output

   // Prepare
   pVariableDependency = (VARIABLE_DEPENDENCY*)pNodeData;

   // Examine property
   switch (eProperty)
   {
   case AK_TEXT:     xOutput = (LPARAM)pVariableDependency->szVariableName;   break;
   case AK_TYPE:     xOutput = (LPARAM)pVariableDependency->eType;            break;
   default:          xOutput = NULL;                                          break;
   }

   // Return property value
   return xOutput;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : insertVariableDependencyIntoAVLTree
// Description     : Attempts to inserts a VariableDependency into a VariableNames tree.
//                    If the name is already present then it's 'occurrence count' is incremented
// 
// AVL_TREE*     pTree        : [in/out] Tree to add script name to
// CONST TCHAR*  szScriptName : [in]     Name of the script name to insert/increment
//
BearScriptAPI
VOID  insertVariableDependencyIntoAVLTree(AVL_TREE*  pTree, CONST TCHAR*  szVariableName, CONST VARIABLE_TYPE  eType, CONST VARIABLE_USAGE  eUsage)
{
   VARIABLE_DEPENDENCY*  pVariableDependency;

   // [CHECK] Check whether this variable already exists
   if (findObjectInAVLTreeByValue(pTree, (LPARAM)szVariableName, eType, (LPARAM&)pVariableDependency))
      /// [SUCCESS] Update the usage to 'get/set' if necessary
      pVariableDependency->eUsage = (VARIABLE_USAGE)(pVariableDependency->eUsage WITH eUsage);
   
   /// [FAILURE] Insert new VariableDependency into tree
   else
      insertObjectIntoAVLTree(pTree, (LPARAM)createVariableDependency(szVariableName, eType, eUsage));
}

