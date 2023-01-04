//
// Language Button Tree.cpp : 
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       CONSTANTS / GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

HBITMAP    createLanguageButtonBitmap(HWND  hRichEdit, CONST TCHAR*  szText, const GAME_TEXT_COLOUR  eColour);
VOID       deleteLanguageButtonNode(LPARAM  pLanguageButton);
LPARAM     extractLanguageButtonNode(LPARAM  pNodeData, CONST AVL_TREE_SORT_KEY  eProperty);
VOID       treeprocFindButtonByObject(AVL_TREE_NODE*  pCurrentNode, AVL_TREE_OPERATION*  pData);

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       CREATION / DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createLanguageButton
// Description     : Create new language button data, containing button text and ID
// 
// CONST TCHAR*  szText : [in] Button text
// CONST TCHAR*  szID   : [in] Button ID
// 
// Return Value   : New language button data, you are responsible for destroying it
// 
BearScriptAPI
LANGUAGE_BUTTON*  createLanguageButton(HWND  hRichEdit, CONST TCHAR*  szText, CONST TCHAR*  szID, const GAME_TEXT_COLOUR  eColour)
{
   LANGUAGE_BUTTON*  pButton;   // New button data

   // Create new button data
   pButton = utilCreateEmptyObject(LANGUAGE_BUTTON);

   // Set Text+ID and create Bitmap
   pButton->szText  = utilDuplicateSimpleString(szText);
   pButton->szID    = utilDuplicateSimpleString(szID);
   pButton->eColour = eColour;
   pButton->hBitmap = createLanguageButtonBitmap(hRichEdit, szText, eColour);

   // Return
   return pButton;
}

/// Function name  : createLanguageButtonBitmap
// Description     : Create a custom bitmap with the specified text for display in a rich edit control
// 
// HWND              hRichEditCtrl : [in] Window handle of a RichEdit control
// LANGUAGE_BUTTON*  szObjectText  : [in] Button data
// 
// Return Value  : Handle of the button's bitmap
// 
HBITMAP  createLanguageButtonBitmap(HWND  hRichEdit, CONST TCHAR*  szText, const GAME_TEXT_COLOUR  eColour)
{
   DC_STATE oState;
   HBITMAP  hNewBitmap;       // New bitmap
   RECT     rcButton;         // Drawing rectangle
   HDC      hMemoryDC,        // Memory DC for creating bitmap
            hDC;              // RichEdit control's device context
   
   // Prepare
   hMemoryDC = CreateCompatibleDC(hDC = GetDC(hRichEdit));
   hNewBitmap = LoadBitmap(getResourceInstance(), TEXT("RICHTEXT_BUTTON"));

   // Setup DC
   oState.hOldBitmap = SelectBitmap(hMemoryDC, hNewBitmap);
   oState.hOldFont   = SelectFont(hMemoryDC, GetStockObject(ANSI_VAR_FONT));
   SetBkMode(hMemoryDC, TRANSPARENT);
   SetTextColor(hMemoryDC, getGameTextColour(eColour));
   
   // Draw button text onto Bitmap
   utilSetRectangle(&rcButton, 0, 0, 160, 19);
   DrawText(hMemoryDC, szText, lstrlen(szText), &rcButton, DT_CENTER WITH DT_VCENTER WITH DT_SINGLELINE WITH DT_END_ELLIPSIS);

   // Cleanup + Extract bitmap
   SelectBitmap(hMemoryDC, oState.hOldBitmap);
   SelectFont(hMemoryDC, oState.hOldFont);
   DeleteDC(hMemoryDC);

   // Return without destroying new bitmap
   ReleaseDC(hRichEdit, hDC);
   return hNewBitmap;
}

/// Function name  : createLanguageButtonTreeByID
// Description     : Creates a language button tree sorted by ID
// 
// Return Value   : New AVLTree, you are responsible for destroying it
// 
BearScriptAPI
AVL_TREE*  createLanguageButtonTreeByID()
{
   return createAVLTree(extractLanguageButtonNode, deleteLanguageButtonNode, createAVLTreeSortKeyEx(AK_ID, AO_DESCENDING, AP_STRING), NULL, NULL);
}


/// Function name  : deleteLanguageButton
// Description     : Delete existing language button data
// 
// LANGUAGE_BUTTON*  pData   : [in] Language button data to delete
//
BearScriptAPI
VOID  deleteLanguageButton(LANGUAGE_BUTTON*  pButton)
{
   // Delete strings and bitmap
   utilDeleteStrings(pButton->szText, pButton->szID);
   DeleteBitmap(pButton->hBitmap);

   // Delete calling object
   utilDeleteObject(pButton);
}


/// Function name  : deleteLanguageButtonNode
// Description     : Deletes a LanguageButton within an AVLTree Node
// 
// LPARAM  LanguageButton : LANGUAGE_BUTTON*& : [in] Reference to a LanguageButton to destroy
// 
VOID   deleteLanguageButtonNode(LPARAM  pLanguageButton)
{
   // Delete node contents
   deleteLanguageButton((LANGUAGE_BUTTON*&)pLanguageButton);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                            HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : extractLanguageButtonNode
// Description     : Extract the desired property from a given TreeNode containing a LanguageButton
// 
// LPARAM                   pNodeData  : [in] LANGUAGE_BUTTON* : Node data representing a LanguageButton
// CONST AVL_TREE_SORT_KEY  eProperty  : [in] Property to retrieve
// 
// Return Value   : Value of the specified property or NULL if the property is unsupported
// 
LPARAM  extractLanguageButtonNode(LPARAM  pNodeData, CONST AVL_TREE_SORT_KEY  eProperty)
{
   LANGUAGE_BUTTON*  pLanguageButton;    // Convenience pointer
   LPARAM            xOutput;            // Property value output

   // Prepare
   pLanguageButton = (LANGUAGE_BUTTON*)pNodeData;

   // Examine property
   switch (eProperty)
   {
   case AK_TEXT:     xOutput = (LPARAM)pLanguageButton->szText;   break;
   case AK_ID:       xOutput = (LPARAM)pLanguageButton->szID;     break;
   default:          xOutput = NULL;                              break;
   }

   // Return property value
   return xOutput;
}


/// Function name  : findLanguageButtonByObject
// Description     : Finds a LanguageButton in a tree by it's OLE pointer
// 
// AVL_TREE*         pTree   : [in] Tree to search
// IOLEObject*       pObject : [in] OLE Object pointer
// LANGUAGE_BUTTON* &pOutput : [out] LanguageButton, if found
// 
// Return Value   : TRUE if found, otherwise FALSE
// 
BearScriptAPI
BOOL   findLanguageButtonByObject(AVL_TREE*  pTree, IOleObject*  pObject, LANGUAGE_BUTTON* &pOutput)
{
   AVL_TREE_OPERATION*  pOperation = createAVLTreeOperation(treeprocFindButtonByObject, ATT_INORDER);

   // Setup operation
   pOperation->xFirstInput = (LPARAM)pObject;

   // Perform operation
   pOutput = (performOperationOnAVLTree(pTree, pOperation) ? (LANGUAGE_BUTTON*)pOperation->xOutput : NULL);

   // Return TRUE if found
   deleteAVLTreeOperation(pOperation);
   return pOutput != NULL;
}


/// Function name  : treeprocFindButtonByObject
// Description     : Finds a LanguageButton by it's OLE Object pointer
// 
// AVL_TREE_NODE*       pCurrentNode   : [in] Node to output
// AVL_TREE_OPERATION*  pOperationData : [in] xFirstInput - Desired OLE Object
///                                           xOutput     - Desired LanguageButton, if found
//
VOID   treeprocFindButtonByObject(AVL_TREE_NODE*  pCurrentNode, AVL_TREE_OPERATION*  pData)
{
   LANGUAGE_BUTTON*  pButton;
   IOleObject*       pObject;

   // Prepare 
   pObject = (IOleObject*)pData->xFirstInput;
   pButton = extractPointerFromTreeNode(pCurrentNode, LANGUAGE_BUTTON);

   // [CHECK] Compare OLEObject pointer
   if (pData->bResult = (pButton->pObject == pObject))
      // [FOUND] Store object
      pData->xOutput = (LPARAM)pButton;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                            FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

