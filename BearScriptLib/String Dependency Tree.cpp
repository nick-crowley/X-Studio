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

/// Function name  : createStringDependency
// Description     : Create a new StringDependency object
// 
// CONST UINT  iPageID   : [in] GameString page ID
// CONST UINT  iStringID : [in] GameString string ID
// 
// Return Value   : New dependency object, you are responsible for destroying it
// 
BearScriptAPI 
STRING_DEPENDENCY*  createStringDependency(CONST UINT  iPageID, CONST UINT  iStringID)
{
   STRING_DEPENDENCY*  pStringDependency;

   // Create new dependency
   pStringDependency = utilCreateEmptyObject(STRING_DEPENDENCY);

   // Set properties
   pStringDependency->iPageID   = iPageID;
   pStringDependency->iStringID = iStringID;

   // Return
   return pStringDependency;
}


/// Function name  : createStringDependencyTreeByPageID
// Description     : Creates a StringDependency tree sorted by Page ID and String ID
// 
// Return Value   : New AVLTree, you are responsible for destroying it
// 
BearScriptAPI
AVL_TREE*  createStringDependencyTreeByPageID()
{
   // Create tree of StringDependency sorted by TYPE and then NAME
   return createAVLTree(extractStringDependencyNode, deleteStringDependencyNode, createAVLTreeSortKey(AK_PAGE_ID, AO_DESCENDING), createAVLTreeSortKey(AK_ID, AO_ASCENDING), NULL);
}


/// Function name  : deleteStringDependencyNode
// Description     : Deletes a StringDependency within an AVLTree Node
// 
// LPARAM  pStringDependency : STRING_DEPENDENCY*& : [in] Reference to a StringDependency to destroy
// 
VOID   deleteStringDependencyNode(LPARAM  pStringDependency)
{
   // Delete node contents
   utilDeleteObject((STRING_DEPENDENCY*&)pStringDependency);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : extractStringDependencyNode
// Description     : Extract the desired property from a given TreeNode containing a StringDependency
// 
// LPARAM                   pNodeData  : [in] STRING_DEPENDENCY* : Node data representing a StringDependency
// CONST AVL_TREE_SORT_KEY  eProperty  : [in] Property to retrieve
// 
// Return Value   : Value of the specified property or NULL if the property is unsupported
// 
LPARAM  extractStringDependencyNode(LPARAM  pNodeData, CONST AVL_TREE_SORT_KEY  eProperty)
{
   STRING_DEPENDENCY*  pStringDependency;    // Convenience pointer
   LPARAM                xOutput;              // Property value output

   // Prepare
   pStringDependency = (STRING_DEPENDENCY*)pNodeData;

   // Examine property
   switch (eProperty)
   {
   case AK_PAGE_ID:  xOutput = (LPARAM)pStringDependency->iPageID;    break;
   case AK_ID:       xOutput = (LPARAM)pStringDependency->iStringID;  break;
   default:          xOutput = NULL;                                  break;
   }

   // Return property value
   return xOutput;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : insertStringDependencyIntoAVLTree
// Description     : Attempts to inserts a StringDependency into a StringDependencies tree
//                    If the string is already present then no action is taken
// 
// AVL_TREE*   pTree     : [in/out] Tree to add script name to
// CONST UINT  iPageID   : [in]     GameString page ID
// CONST UINT  iStringID : [in]     GameString string ID
//
BearScriptAPI
VOID  insertStringDependencyIntoAVLTree(AVL_TREE*  pTree, CONST UINT  iPageID, CONST UINT  iStringID)
{
   STRING_DEPENDENCY*  pStringDependency;

   // [CHECK] Ensure string does not already exist
   if (!findObjectInAVLTreeByValue(pTree, iPageID, iStringID, (LPARAM&)pStringDependency))
   {
      /// [SUCCESS] Generate new StringDependency 
      pStringDependency = createStringDependency(iPageID, iStringID);

      // Attempt to resolve GameString
      findGameStringByID(iStringID, iPageID, pStringDependency->pGameString);

      // Add to tree
      insertObjectIntoAVLTree(pTree, (LPARAM)pStringDependency);
   }
}

