//
// Scratch Pad.cpp : For testing stuff
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

/// Function name  :  treeprocTestRichEditConversion
// Description     : 
// 
// AVL_TREE_NODE*       pNode      : [in] 
// AVL_TREE_OPERATION*  pOperation : [in] 
// 
VOID   treeprocTestRichEditConversion(AVL_TREE_NODE*  pNode, AVL_TREE_OPERATION*  pOperation)
{
   LANGUAGE_MESSAGE*  pMessage   = NULL;
   GAME_STRING       *pString    = duplicateGameString(extractPointerFromTreeNode(pNode, GAME_STRING)),
                     *pNewString = createGameString(TEXT("empty"), NULL, NULL, ST_INTERNAL);

   /// Generate RichText
   if (!generateLanguageMessageFromGameString(pString, pMessage, NULL))
      VERBOSE("*** Failed to generated RichText for PageID:%d StringID:%d", pString->iPageID, pString->iID);
   else
   {
      // Pass through RichEdit
      setRichEditText((HWND)pOperation->xFirstInput, pMessage, false, GTC_BLACK);
      getRichEditText((HWND)pOperation->xFirstInput, pMessage);

      // TODO: Compare RichText

      /// Generate PlainText
      if (!generatePlainTextFromLanguageMessage(pMessage, pNewString))
         VERBOSE("*** Failed to generated PlainText for PageID:%d StringID:%d", pString->iPageID, pString->iID);

      /// Compare input and output
      else if (!utilCompareStringVariables(pString->szText, pNewString->szText))
      {
         VERBOSE("*** DIFFERENCE in PageID:%d StringID:%d:\r\n" "BEFORE: \r\n" "%s\r\n\r\n" "AFTER: \r\n" "%s\r\n\r\n__", pString->iPageID, pString->iID, pString->szText, pNewString->szText);
         
      }
         
   }

   // Cleanup
   deleteGameString(pString);
   deleteGameString(pNewString);
   deleteLanguageMessage(pMessage);
}

BOOL  testRichTextConversion(HWND  hParent, const AVL_TREE*  pGameStringTree)
{   
   AVL_TREE_OPERATION*  pOperation = createAVLTreeOperation(treeprocTestRichEditConversion, ATT_INORDER);

   // Create RichEdit
   HWND  hRichEdit = CreateWindow(RICHEDIT_CLASS, TEXT("Temporary RichEdit"), WS_CHILD | WS_BORDER | ES_DISABLENOSCROLL | ES_MULTILINE | ES_WANTRETURN | WS_VSCROLL, 0, 0, 200, 100, hParent, (HMENU)42, getAppInstance(), NULL);
   ERROR_CHECK("Creating temporary RichEdit", hRichEdit);

   /// Test each GameString
   pOperation->xFirstInput = (LPARAM)hRichEdit;
   performOperationOnAVLTree(pGameStringTree, pOperation);

   // Cleanup
   deleteAVLTreeOperation(pOperation);
   utilDeleteWindow(hRichEdit);
   return TRUE;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////


