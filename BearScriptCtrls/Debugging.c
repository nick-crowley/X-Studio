//
// Debugging.cpp : Debugging functions for the controls library
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                 CONSTRUCTION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       METHODS
/// /////////////////////////////////////////////////////////////////////////////////////////



VOID  debugCodeEditData(const CODE_EDIT_DATA*  pWindowData)
{
   // Caret
   if (!pWindowData)
      CONSOLE("Window data is null");
   else
   {
      debugList(pWindowData->pLineList);
      CONSOLE("Selection: bExists=%d  oOrigin=(%dx, %dy)", pWindowData->oSelection.bExists, pWindowData->oSelection.oOrigin.iIndex, pWindowData->oSelection.oOrigin.iLine);
      CONSOLE("Caret: pCharItem=%x  oLocation=(%dx, %dy)", pWindowData->oCaret.pCharacterItem, pWindowData->oCaret.oLocation.iIndex, pWindowData->oCaret.oLocation.iLine);

      // Display Scope, Suggestion, Undo, ScriptFile
      debugCodeEditLabel(pWindowData->pCurrentScope);
      debugCodeEditSuggestion(&pWindowData->oSuggestion);
      debugCodeEditUndo(&pWindowData->oUndo);
      debugScriptFile(pWindowData->pScriptFile);
   }
}


ControlsAPI
VOID  debugCodeEditLabel(const CODE_EDIT_LABEL*  pLabel)
{
   !pLabel ? CONSOLE("Label is null") : CONSOLE("Label '%s': iLine=%d", pLabel->szName, pLabel->iLineNumber);
}


VOID  debugCodeEditSuggestion(const CODE_EDIT_SUGGESTION*  pSuggestion)
{
   CONSOLE("hCtrl=%d  eType=%d  ptOrigin=(%dx,%dy)", pSuggestion->hCtrl, pSuggestion->eType, pSuggestion->ptOrigin.iIndex, pSuggestion->ptOrigin.iLine);
   debugTree(pSuggestion->pResultsTree);
}


VOID  debugCodeEditUndo(const CODE_EDIT_UNDO*  pUndo)
{
   if (!pUndo)
      CONSOLE("Undo is null");
   else
   {
      debugCodeEditUndoItem(&pUndo->oBuffer);
      CONSOLE("Undo Buffer: '%s'", pUndo->szBuffer);
      debugStack(pUndo->pUndoStack);
      debugStack(pUndo->pRedoStack);
   }
}

VOID  debugCodeEditUndoItem(const CODE_EDIT_UNDO_ITEM*  pItem)
{
   if (!pItem)
      CONSOLE("UndoItem is null");
   else
      CONSOLE("UndoItem '%s' eType=%d  oStart=(%dx,%dy)  oFinish=(%dx,%dy)", pItem->szText, pItem->eType, 
               pItem->oStart.iIndex, pItem->oStart.iLine, pItem->oFinish.iIndex, pItem->oFinish.iLine);
}

