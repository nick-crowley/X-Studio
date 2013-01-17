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

#define     UNDO_COMMIT_TICKER        2

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createCodeEditUndoItem
// Description     : Creates data for a CodeEditUndoItem
// 
// CONST TCHAR*               szText    : [in] Item text
// CONST UNDO_ITEM_TYPE       eType     : [in] Whether item was inserted or deleted
// CONST CODE_EDIT_LOCATION*  pStart    : [in] Item start
// CONST CODE_EDIT_LOCATION*  pFinish   : [in] Item finish
//
// Return Value   : New CodeEditUndoItem, you are responsible for destroying it
// 
CODE_EDIT_UNDO_ITEM*   createCodeEditUndoItem(CONST TCHAR*  szText, CONST UNDO_ITEM_TYPE  eType, CONST CODE_EDIT_LOCATION*  pStart, CONST CODE_EDIT_LOCATION*  pFinish)
{
   CODE_EDIT_UNDO_ITEM*   pCodeEditUndoItem;       // Object being created

   // Create object
   pCodeEditUndoItem = utilCreateEmptyObject(CODE_EDIT_UNDO_ITEM);

   // Set properties
   pCodeEditUndoItem->szText  = utilSafeDuplicateSimpleString(szText);
   pCodeEditUndoItem->eType   = eType;
   pCodeEditUndoItem->oStart  = (*pStart);
   pCodeEditUndoItem->oFinish = (*pFinish);

   // Return object
   return pCodeEditUndoItem;
}


/// Function name  : deleteCodeEditUndoItem
// Description     : Destroys the data for theCodeEditUndoItem
// 
// CODE_EDIT_UNDO_ITEM*  &pCodeEditUndoItem   : [in] CodeEditUndoItem to destroy
// 
VOID  deleteCodeEditUndoItem(CODE_EDIT_UNDO_ITEM*  &pCodeEditUndoItem)
{
   // [OPTIONAL] Destroy text
   utilSafeDeleteString(pCodeEditUndoItem->szText);

   // Destroy calling object
   utilDeleteObject(pCodeEditUndoItem);
}


/// Function name  : destructorCodeEditUndoItem
// Description     : CodeEditUndoItem queue item destructor
// 
// LPARAM  pCodeEditUndoItem : [in] CodeEditUndoItem to destroy
// 
VOID  destructorCodeEditUndoItem(LPARAM  pCodeEditUndoItem)
{
   // Destroy calling object
   deleteCodeEditUndoItem((CODE_EDIT_UNDO_ITEM*&)pCodeEditUndoItem);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : performCodeEditUndoAction
// Description     : Performs the textual addition or removal
// 
// CODE_EDIT_DATA*       pWindowData : [in] Window data
// CODE_EDIT_UNDO_ITEM*  pAction     : [in] Item data
// 
VOID  performCodeEditUndoAction(CODE_EDIT_DATA*  pWindowData, CODE_EDIT_UNDO_ITEM*  pAction)
{
   CODE_EDIT_UNDO&  oUndo = pWindowData->oUndo;

   // [DEBUGGING]
   //VERBOSE("Performing undo action %d between (%d:%d) and (%d:%d) : '%s'", pAction->eType, pAction->oStart.iLine, pAction->oStart.iIndex, pAction->oFinish.iLine, pAction->oFinish.iIndex, pAction->szText);

   // Cancel any selection
   removeCodeEditSelection(pWindowData);

   /// Position caret at start
   setCodeEditCaretLocation(pWindowData, &pAction->oStart);

   /// [ADDITION] Remove text that was added
   if (pAction->eType == UIT_ADDITION)
   {
      // Select text that was added
      pWindowData->oSelection.oOrigin = pAction->oFinish;
      setCodeEditSelection(pWindowData, pAction->oStart, pAction->oFinish, TRUE);
      // Delete text
      removeCodeEditSelectionText(pWindowData, FALSE);
      removeCodeEditSelection(pWindowData);
   }
   /// [REMOVAL] Insert text that was removed
   else
      // Insert text and original position
      insertCodeEditTextAtCaret(pWindowData, pAction->szText, FALSE);
}


/// Function name  : pushStringToCodeEditUndoStack
// Description     : Adds a new string to the undo stack
// 
// CODE_EDIT_DATA*            pWindowData : [in] Window data
// CONST TCHAR*               szText      : [in] Item text
// CONST UNDO_ITEM_TYPE       eType       : [in] Whether item was inserted or deleted
// CONST CODE_EDIT_LOCATION*  pLocation   : [in] Item location
// 
VOID  pushStringToCodeEditUndoStack(CODE_EDIT_DATA*  pWindowData, CONST TCHAR*  szText, CONST UNDO_ITEM_TYPE  eType, CONST CODE_EDIT_LOCATION*  pStart, CONST CODE_EDIT_LOCATION*  pFinish)
{
   CODE_EDIT_UNDO_ITEM*  pBottomItem;
   CODE_EDIT_UNDO&       oUndo = pWindowData->oUndo;

   /// Add new item to undo stack 
   pushObjectOntoStack(oUndo.pUndoStack, (LPARAM)createCodeEditUndoItem(szText, eType, pStart, pFinish));

   // [DEBUGGING]
   //CODE_EDIT_UNDO_ITEM*  pAction = (CODE_EDIT_UNDO_ITEM*)topStackObject(oUndo.pUndoStack);
   //VERBOSE("Storing undo action %d between (%d:%d) and (%d:%d) : '%s'", pAction->eType, pAction->oStart.iLine, pAction->oStart.iIndex, pAction->oFinish.iLine, pAction->oFinish.iIndex, pAction->szText);

   /// Empty Redo stack
   deleteListContents(oUndo.pRedoStack);

   // [CHECK] Ensure stack isn't full
   if (getStackItemCount(oUndo.pUndoStack) > 32)
   {
      // [FULL] Destroy item at bottom of the stack
      pBottomItem = (CODE_EDIT_UNDO_ITEM*)popObjectFromStackBottom(oUndo.pUndoStack);
      deleteCodeEditUndoItem(pBottomItem);
   }
}


/// Function name  : pushCharacterToCodeEditUndoStack
// Description     : Adds a Character to the undo buffer, and initiates the commit timer
// 
// CODE_EDIT_DATA*            pWindowData : [in] Window data
// CONST TCHAR                szCharacter : [in] Character
// CONST UNDO_ITEM_TYPE       eType       : [in] Addition or Removal
// CONST CODE_EDIT_LOCATION*  pStart      : [in] Position of character
// 
VOID  pushCharacterToCodeEditUndoStack(CODE_EDIT_DATA*  pWindowData, CONST TCHAR  szCharacter, CONST UNDO_ITEM_TYPE  eType, CONST CODE_EDIT_LOCATION*  pStart)
{
   CODE_EDIT_UNDO&     oUndo     = pWindowData->oUndo;
   CODE_EDIT_LOCATION  oFinish   = (*pStart);
   TCHAR               szText[2] = { szCharacter, NULL };

   /// Empty Redo stack
   deleteListContents(oUndo.pRedoStack);

   // Calculate finish position
   oFinish.iIndex++;

   // [NEW-LINE ADDED] Set finish to the start of the next line
   if (eType == UIT_ADDITION AND szCharacter == '\r')
   {
      oFinish.iLine++;
      oFinish.iIndex = 0;
   }

   // [CHECK] Has buffer type changed?
   if (lstrlen(oUndo.szBuffer) AND oUndo.oBuffer.eType != eType)
   {
      /// [CHANGED] Commit current contents to UndoStack
      pushStringToCodeEditUndoStack(pWindowData, oUndo.szBuffer, oUndo.oBuffer.eType, &oUndo.oBuffer.oStart, &oUndo.oBuffer.oFinish);
      oUndo.szBuffer[0] = NULL;
   }

   // [CHECK] Should we start a new buffer?
   if (!lstrlen(oUndo.szBuffer))
   {
      /// [INITIALISE] Store type and position
      oUndo.oBuffer.eType   = eType;
      oUndo.oBuffer.oStart  = *pStart;
      oUndo.oBuffer.oFinish = oFinish;

      // Setup buffer
      StringCchCopy(oUndo.szBuffer, LINE_LENGTH, szText);
   }
   // [CHECK] Has user typed a character?
   else if (eType == UIT_ADDITION)
   {
      /// [CHARACTER] Add character and set new finish position
      StringCchCat(oUndo.szBuffer, LINE_LENGTH, szText);
      oUndo.oBuffer.oFinish = oFinish;
   }
   // [CHECK] Has user pressed delete?
   else if (compareCodeEditLocations(oUndo.oBuffer.oStart, *pStart) == CR_EQUAL)
   {
      /// [DELETE] Add character without adjusting position
      StringCchCat(oUndo.szBuffer, LINE_LENGTH, szText);
   }
   else
   {
      // Backup buffer
      TCHAR* szTextCopy = utilDuplicateSimpleString(oUndo.szBuffer);

      /// [BACKSPACE] Add character in reverse order
      StringCchCopy(oUndo.szBuffer, LINE_LENGTH, szText);
      StringCchCat(oUndo.szBuffer, LINE_LENGTH, szTextCopy);
      oUndo.oBuffer.oStart = (*pStart);

      // Cleanup
      utilDeleteString(szTextCopy);
   }

   // Set/Reset commit timer
   SetTimer(pWindowData->hWnd, UNDO_COMMIT_TICKER, 500, NULL);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onCodeEditUndoCommitTimer
// Description     : Commits the contents of the current undo buffer into a new undo stack item
// 
// CODE_EDIT_DATA*  pWindowData : [in] Window data
// 
VOID  onCodeEditUndoCommitTimer(CODE_EDIT_DATA*  pWindowData)
{
   CODE_EDIT_UNDO&   oUndo = pWindowData->oUndo;

   // Kill undo timer
   KillTimer(pWindowData->hWnd, UNDO_COMMIT_TICKER);

   /// Commit current contents to UndoStack
   pushStringToCodeEditUndoStack(pWindowData, oUndo.szBuffer, oUndo.oBuffer.eType, &oUndo.oBuffer.oStart, &oUndo.oBuffer.oFinish);
   oUndo.szBuffer[0] = NULL;
}


/// Function name  : onCodeEditRedoLastAction
// Description     : Performs the top action in the redo stack, and adds it to the undo stack
// 
// CODE_EDIT_DATA*  pWindowData : [in] Window data
// 
VOID  onCodeEditRedoLastAction(CODE_EDIT_DATA*  pWindowData)
{
   CODE_EDIT_UNDO&       oUndo = pWindowData->oUndo;
   CODE_EDIT_UNDO_ITEM*  pLastAction;

   // [CHECK] Extract last redo action
   if (pLastAction = (CODE_EDIT_UNDO_ITEM*)popObjectFromStack(oUndo.pRedoStack))
   {
      /// Perform last action 
      performCodeEditUndoAction(pWindowData, pLastAction);

      // Transfer to Undo queue
      pLastAction->eType = (pLastAction->eType == UIT_ADDITION ? UIT_DELETION : UIT_ADDITION);
      pushObjectOntoStack(oUndo.pUndoStack, (LPARAM)pLastAction);
   }
}


/// Function name  : onCodeEditUndoLastAction
// Description     : Performs the top action in the undo stack, and adds it to the redo stack
// 
// CODE_EDIT_DATA*  pWindowData : [in] window data
// 
VOID  onCodeEditUndoLastAction(CODE_EDIT_DATA*  pWindowData)
{
   CODE_EDIT_UNDO&       oUndo = pWindowData->oUndo;
   CODE_EDIT_UNDO_ITEM*  pLastAction;

   // [CHECK] Extract last undo action
   if (pLastAction = (CODE_EDIT_UNDO_ITEM*)popObjectFromStack(oUndo.pUndoStack))
   {
      /// Perform last action
      performCodeEditUndoAction(pWindowData, pLastAction);

      // Transfer to redo queue
      pLastAction->eType = (pLastAction->eType == UIT_ADDITION ? UIT_DELETION : UIT_ADDITION);
      pushObjectOntoStack(oUndo.pRedoStack, (LPARAM)pLastAction);
   }
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////


