//
// Variable Names Tree.cpp : Counts the variables used within a document and assigns them each a unique ID
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

/// Function name  : createProjectVariable
// Description     : Creates a ProjectVariable object, used for converting variable names into unique IDs
// 
// CONST TCHAR*  szName : [in] Name of the variable
// CONST UINT    iValue : [in] Value
// 
// Return Value   : New ProjectVariable, you are responsible for destroying it
// 
BearScriptAPI
PROJECT_VARIABLE*  createProjectVariable(CONST TCHAR*  szName, CONST UINT  iValue)
{
   PROJECT_VARIABLE*  pProjectVariable;   // Object being created

   // Create new object
   pProjectVariable = utilCreateEmptyObject(PROJECT_VARIABLE);

   // Set properties
   pProjectVariable->szName = utilDuplicateSimpleString(szName);
   pProjectVariable->iValue = iValue;

   // Return new object
   return pProjectVariable;
}


/// Function name  : createProjectVariableTreeByText
// Description     : Creates a variables tree sorted by Text
// 
// Return Value   : New AVLTree, you are responsible for destroying it
// 
BearScriptAPI
AVL_TREE*  createProjectVariableTreeByText()
{
   // Create tree of ProjectVariables indexed by name and in alphabetical order
   return createAVLTree(extractProjectVariableTreeNode, deleteProjectVariableTreeNode, createAVLTreeSortKey(AK_TEXT, AO_DESCENDING), NULL, NULL);
}


/// Function name  : duplicateProjectVariable
// Description     : Duplicates an existing ProjectVariable - used by CodeEdit for creating deep copies of storage trees
// 
// CONST PROJECT_VARIABLE*  pSource : [in] ProjectVariable to copy
// 
// Return Value   : New ProjectVariable, you are responsible for destroying it
// 
BearScriptAPI
PROJECT_VARIABLE*  duplicateProjectVariable(CONST PROJECT_VARIABLE*  pSource)
{
   PROJECT_VARIABLE*  pProjectVariable;   // Object being created

   // Create new object
   pProjectVariable = utilCreateEmptyObject(PROJECT_VARIABLE);

   // Set properties
   pProjectVariable->szName = utilDuplicateSimpleString(pSource->szName);
   pProjectVariable->iValue = pSource->iValue;
   
   // Return new object
   return pProjectVariable;
}



/// Function name  : deleteProjectVariable
// Description     : Deletes a ProjectVariable object
// 
// PROJECT_VARIABLE*  &pProjectVariable   : [in] ProjectVariable object to destroy
//
BearScriptAPI
VOID     deleteProjectVariable(PROJECT_VARIABLE*  &pProjectVariable)
{
   // Delete name
   utilDeleteString(pProjectVariable->szName);

   // Delete calling object
   utilDeleteObject(pProjectVariable);
}


/// Function name  : deleteProjectVariableTreeNode
// Description     : Deletes a ProjectVariable within a tree or list
// 
// LPARAM  pProjectVariable : [in] Reference to a ProjectVariable pointer
// 
BearScriptAPI
VOID   deleteProjectVariableTreeNode(LPARAM  pProjectVariable)
{
   // Delete node contents
   deleteProjectVariable((PROJECT_VARIABLE*&)pProjectVariable);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : extractProjectVariableTreeNode
// Description     : Extract the desired property from a given TreeNode containing a ProjectVariable
// 
// LPARAM                   pNode      : [in] GAME_STRING* : Node data containing a ProjectVariable
// CONST AVL_TREE_SORT_KEY  eProperty  : [in] Property to retrieve
// 
// Return Value   : Value of the specified property or NULL if the property is unsupported
// 
BearScriptAPI
LPARAM  extractProjectVariableTreeNode(LPARAM  pNode, CONST AVL_TREE_SORT_KEY  eProperty)
{
   PROJECT_VARIABLE*  pProjectVariable;    // Convenience pointer
   LPARAM             xOutput;             // Property value output

   // Prepare
   pProjectVariable = (PROJECT_VARIABLE*)pNode;

   // Examine property
   switch (eProperty)
   {
   case AK_TEXT:     xOutput = (LPARAM)pProjectVariable->szName;       break;
   case AK_VALUE:    xOutput = (LPARAM)pProjectVariable->iValue;       break;
   default:          xOutput = NULL;                                   break;
   }

   // Return property value
   return xOutput;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

