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

/// Function name  : createVariableName
// Description     : Creates a VariableName object, used for converting variable names into unique IDs
// 
// CONST TCHAR*  szName : [in] Name of the variable
// CONST UINT    iID    : [in] Associated ID of the variable
// 
// Return Value   : New VariableName, you are responsible for destroying it
// 
BearScriptAPI
VARIABLE_NAME*  createVariableName(CONST TCHAR*  szName, CONST UINT  iID)
{
   VARIABLE_NAME*  pVariableName;   // Object being created

   // Create new object
   pVariableName = utilCreateEmptyObject(VARIABLE_NAME);

   // Set properties
   pVariableName->szName    = utilDuplicateSimpleString(identifyRawVariableName(szName));
   pVariableName->iID       = iID;
   pVariableName->iCount    = 1;
   pVariableName->eAssigned = VA_UNASSIGNED;

   // Return new object
   return pVariableName;
}


/// Function name  : createVariableNameTreeByID
// Description     : Creates a variables tree sorted by ID
// 
// Return Value   : New AVLTree, you are responsible for destroying it
// 
BearScriptAPI
AVL_TREE*  createVariableNameTreeByID()
{
   // Create tree of VariableNames indexed by name and in alphabetical order
   return createAVLTree(extractVariableNameTreeNode, deleteVariableNameTreeNode, createAVLTreeSortKey(AK_ID, AO_ASCENDING), NULL, NULL);
}


/// Function name  : createVariableNameTreeByText
// Description     : Creates a variables tree sorted by Text
// 
// Return Value   : New AVLTree, you are responsible for destroying it
// 
BearScriptAPI
AVL_TREE*  createVariableNameTreeByText()
{
   // Create tree of VariableNames indexed by name and in alphabetical order
   return createAVLTree(extractVariableNameTreeNode, deleteVariableNameTreeNode, createAVLTreeSortKey(AK_TEXT, AO_DESCENDING), NULL, NULL);
}


/// Function name  : duplicateVariableName
// Description     : Duplicates an existing VariableName - used by CodeEdit for creating deep copies of storage trees
// 
// CONST VARIABLE_NAME*  pSource : [in] VariableName to copy
// 
// Return Value   : New VariableName, you are responsible for destroying it
// 
BearScriptAPI
VARIABLE_NAME*  duplicateVariableName(CONST VARIABLE_NAME*  pSource)
{
   VARIABLE_NAME*  pVariableName;   // Object being created

   // Create new object
   pVariableName = utilCreateEmptyObject(VARIABLE_NAME);

   // Set properties
   pVariableName->szName    = utilDuplicateString(pSource->szName, lstrlen(pSource->szName));
   pVariableName->iID       = pSource->iID;
   pVariableName->iCount    = pSource->iCount;
   pVariableName->eAssigned = pSource->eAssigned;

   // Return new object
   return pVariableName;
}



/// Function name  : deleteVariableName
// Description     : Deletes a VariableName object
// 
// VARIABLE_NAME*  &pVariableName   : [in] VariableName object to destroy
// 
VOID     deleteVariableName(VARIABLE_NAME*  &pVariableName)
{
   // Delete name
   utilDeleteString(pVariableName->szName);

   // Delete calling object
   utilDeleteObject(pVariableName);
}


/// Function name  : deleteVariableNameTreeNode
// Description     : Deletes a VariableName within a tree or list
// 
// LPARAM  pVariableName : [in] Reference to a VariableName pointer
// 
BearScriptAPI
VOID   deleteVariableNameTreeNode(LPARAM  pVariableName)
{
   // Delete node contents
   deleteVariableName((VARIABLE_NAME*&)pVariableName);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : extractVariableNameTreeNode
// Description     : Extract the desired property from a given TreeNode containing a VariableName
// 
// LPARAM                   pNode      : [in] GAME_STRING* : Node data containing a VariableName
// CONST AVL_TREE_SORT_KEY  eProperty  : [in] Property to retrieve
// 
// Return Value   : Value of the specified property or NULL if the property is unsupported
// 
BearScriptAPI
LPARAM  extractVariableNameTreeNode(LPARAM  pNode, CONST AVL_TREE_SORT_KEY  eProperty)
{
   VARIABLE_NAME*  pVariableName;    // Convenience pointer
   LPARAM          xOutput;          // Property value output

   // Prepare
   pVariableName = (VARIABLE_NAME*)pNode;

   // Examine property
   switch (eProperty)
   {
   case AK_TEXT:     xOutput = (LPARAM)pVariableName->szName;       break;
   case AK_COUNT:    xOutput = (LPARAM)pVariableName->iCount;       break;
   case AK_ID:       xOutput = (LPARAM)pVariableName->iID;          break;
   default:          xOutput = NULL;                                break;
   }

   // Return property value
   return xOutput;
}


/// Function name  : identifyRawVariableName
// Description     : Extracts the name of a variable without the dollar sign
// 
// CONST TCHAR*  szVariableName : [in] Variable name
// 
// Return Value   : Raw Variable name, or NULL if name only contains the dollar sign
// 
BearScriptAPI
CONST TCHAR*  identifyRawVariableName(CONST TCHAR*  szVariableName)
{
   return utilFindCharacterNotInSet(szVariableName, "$");
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : insertVariableNameIntoAVLTree
// Description     : Attempts to inserts a VariableName into a VariableNames tree.  If the name is already present then it's 'occurrence count' is incremented.
///                        If the variable is empty or just the dollar sign, it is ignored
// 
// AVL_TREE*     pTree       : [in/out] Tree to add variable to
// CONST TCHAR*  szVariable  : [in]     Name of the variable to insert/increment
// CONST UINT    iID         : [in]     Unique variable ID
// 
// Return Value: New VariableName object if successful, otherwise NULL
//
BearScriptAPI
BOOL   insertVariableNameIntoAVLTree(AVL_TREE*  pTree, CONST TCHAR*  szVariable, CONST UINT  iID)
{
   VARIABLE_NAME*  pVariableName;
   CONST TCHAR*    szRawName;

   // Extract raw variable name
   if (szRawName = identifyRawVariableName(szVariable))
   {
      // Attempt to insert into target tree
      if (findObjectInAVLTreeByValue(pTree, (LPARAM)szRawName, NULL, (LPARAM&)pVariableName))
         /// [EXISTING] Lookup Variable and increase it's 'occurrence count'
         pVariableName->iCount++;
      else
      {
         /// [NEW] Create a VariableName and add it to the tree
         pVariableName = createVariableName(szRawName, iID);
         insertObjectIntoAVLTree(pTree, (LPARAM)pVariableName);
      }
   }

   // Return TRUE if inserted
   return (szRawName != NULL);
}

