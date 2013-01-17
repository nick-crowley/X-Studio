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

/// Function name  : createImageTree
// Description     : Creates an ImageList that can be accessed via icon ID
// 
// CONST UINT  iIconSize  : [in] Icon size, in pixels
// CONST UINT  iIconCount : [in] Initial size of ImageList
//
// Return Value   : New ImageTree, you are responsible for destroying it
// 
BearScriptAPI
IMAGE_TREE*   createImageTree(CONST UINT  iIconSize, CONST UINT  iIconCount)
{
   IMAGE_TREE*   pImageTree;       // Object being created

   // Create object
   pImageTree = utilCreateEmptyObject(IMAGE_TREE);

   /// Create tree and ImageList
   pImageTree->pIconsByID = createAVLTree(extractImageTreeNode, deleteImageTreeNode, createAVLTreeSortKeyEx(AK_ID, AO_DESCENDING, AP_STRING), NULL, NULL);
   pImageTree->hImageList = ImageList_Create(iIconSize, iIconSize, ILC_COLOR32 WITH ILC_MASK, iIconCount, 2);

   // Return object
   return pImageTree;
}


/// Function name  : createImageTreeItem
// Description     : Creates data for a ImageTreeItem
//
// CONST TCHAR*  szID    : [in] Icon resource ID
// HICON         hHandle : [in] Icon resource handle
// CONST UINT    iIndex  : [in] ImageTree ImageList index
// 
// Return Value   : New ImageTreeItem, you are responsible for destroying it
// 
IMAGE_TREE_ITEM*   createImageTreeItem(CONST TCHAR*  szID, HICON  hHandle, CONST UINT  iIndex)
{
   IMAGE_TREE_ITEM*   pImageTreeItem;       // Object being created

   // Create object
   pImageTreeItem = utilCreateEmptyObject(IMAGE_TREE_ITEM);

   // Set properties
   pImageTreeItem->szID    = szID;
   pImageTreeItem->hHandle = hHandle;
   pImageTreeItem->iIndex  = iIndex;

   // Return object
   return pImageTreeItem;
}



/// Function name  : deleteImageTree
// Description     : Destroys the data for the ImageTree
// 
// IMAGE_TREE*  &pImageTree   : [in] ImageTree to destroy
// 
BearScriptAPI
VOID  deleteImageTree(IMAGE_TREE*  &pImageTree)
{
   // Destroy contents
   deleteAVLTree(pImageTree->pIconsByID);
   ImageList_Destroy(pImageTree->hImageList);

   // Destroy calling object
   utilDeleteObject(pImageTree);
}


/// Function name  : deleteImageTreeNode
// Description     : Destroys an item in an ImageTree
// 
// LPARAM pImageTree : [in] ImageTreeItem to destroy
// 
VOID  deleteImageTreeNode(LPARAM  ImageTreeItem)
{
   // Destroy object
   utilDeleteObject((IMAGE_TREE_ITEM*&)ImageTreeItem);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : extractImageTreeNode
// Description     : Returns the value of the specified property in an AVLTree node containing a ImageTree
// 
// LPARAM                   pObject    : [in] IMAGE_TREE* : TreeNode data containing a ImageTree
// CONST AVL_TREE_SORT_KEY  eProperty  : [in] Property to retrieve
// 
// Return Value   : Desired property value or NULL if the property is unsupported
// 
LPARAM   extractImageTreeNode(LPARAM  pObject, CONST AVL_TREE_SORT_KEY  eProperty)
{
   IMAGE_TREE_ITEM*  pIcon;      // Convenience pointer
   LPARAM            xOutput;    // Property value

   // Prepare
   pIcon = (IMAGE_TREE_ITEM*)pObject;

   // Examine property
   switch (eProperty)
   {
   case AK_ID:       xOutput = (LPARAM)pIcon->szID;   break;
   // [UNSUPPORTED] Error
   default:          xOutput = NULL;  ASSERT(FALSE);  break;
   }

   // Return value
   return xOutput;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : insertImageTreeIcon
// Description     : Adds a new icon to an ImageTree
// 
// IMAGE_TREE*  pImageTree : [in] ImageTree
// CONST TCHAR  szIconID   : [in] Resource ID of the icon to add
// 
BearScriptAPI
VOID  insertImageTreeIcon(IMAGE_TREE*  pImageTree, CONST TCHAR*  szIconID)
{
   IMAGE_TREE_ITEM*  pNewItem;

   // Create new icon
   pNewItem = createImageTreeItem(szIconID, LoadIcon(getResourceInstance(), szIconID), ImageList_GetImageCount(pImageTree->hImageList));

   /// Insert into ImageTree
   if (insertObjectIntoAVLTree(pImageTree->pIconsByID, (LPARAM)pNewItem))
      /// [SUCCESS] Insert into ImageList
      ImageList_AddIcon(pImageTree->hImageList, pNewItem->hHandle);
   else
      // [DUPLICATE] Ignore
      deleteImageTreeNode((LPARAM)pNewItem);
}


/// Function name  : getImageTreeIconIndex
// Description     : Retrieves the ImageList index of an icon within an ImageTree
// 
// CONST IMAGE_TREE*  pImageTree : [in] ImageTree
// CONST TCHAR*       szID       : [in] Resource ID of the icon to query
// 
// Return Value   : Zero-based ImageList index of the specified icon, or NULL if not found
// 
BearScriptAPI
UINT  getImageTreeIconIndex(CONST IMAGE_TREE*  pImageTree, CONST TCHAR*  szID)
{
   IMAGE_TREE_ITEM*  pIcon;         // ImageTree icon
   UINT              iIndex;        // Operation result

   // Prepare
   iIndex = NULL;

   /// Lookup icon by ID
   if (findObjectInAVLTreeByValue(pImageTree->pIconsByID, (LPARAM)szID, NULL, (LPARAM&)pIcon))
      // [FOUND] Set result
      iIndex = pIcon->iIndex;

   // Return index
   return iIndex;
}


/// Function name  : getImageTreeIconHandle
// Description     : Retrieves the icon from within an ImageTree
// 
// CONST IMAGE_TREE*  pImageTree : [in] ImageTree
// CONST TCHAR*       szID       : [in] Resource ID of the icon to query
// 
// Return Value   : Icon handle if found, otherwise NULL
// 
BearScriptAPI
HICON  getImageTreeIconHandle(CONST IMAGE_TREE*  pImageTree, CONST TCHAR*  szID)
{
   IMAGE_TREE_ITEM*  pIcon;        // ImageTree icon
   HICON             hOutput;      // Operation result

   // Prepare
   hOutput = NULL;

   /// Lookup icon by ID
   if (findObjectInAVLTreeByValue(pImageTree->pIconsByID, (LPARAM)szID, NULL, (LPARAM&)pIcon))
      // [FOUND] Set result
      hOutput = pIcon->hHandle;

   // Return index
   return hOutput;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////


