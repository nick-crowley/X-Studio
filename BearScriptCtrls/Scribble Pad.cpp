//
// Scribble Pad.cpp : Testing grounds
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 
#include "stdafx.h"
/// /////////////////////////////////////////////////////////////////////////////////////////
///                                      DECLARATIONS
/// /////////////////////////////////////////////////////////////////////////////////////////
/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////
/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////
/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////
/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       MODIFIED
/// /////////////////////////////////////////////////////////////////////////////////////////
/// Function name  : removeCodeEditSelectionText9
// Description     : Deletes the current text selection, repositions the caret and invalidates the affected lines
// 
// CODE_EDIT_DATA*  pWindowData   : [in] Window data
// 
/// UNUSED: Implementing removing characters with the iterator is too weird at the moment, im tired.
//
//VOID   removeCodeEditSelectionText9(CODE_EDIT_DATA*  pWindowData)
//{
//   CODE_EDIT_ITERATOR*  pIterator;        // Character data iterator
//   
//   // [CHECK] Ensure selection exists
//   if (!hasCodeEditSelection(pWindowData))
//      return;
//
//   // Prepare
//   pIterator = createCodeEditMultipleLineIterator(pWindowData, pWindowData->oSelection.oOrigin, pWindowData->oCaret.oLocation);
//
//   /// Delete all characters within selection
//   //removeCodeEditIteratorCharacters(pIterator);
//
//   // [SINGLE LINE] Redraw line
//   if (pIterator->oStart.iLine == pIterator->oFinish.iLine)
//      updateCodeEditSingleLine(pWindowData, pIterator->oStart.iLine);
//
//   // [MULTIPLE LINES] Redraw initial line and all below it
//   else
//      updateCodeEditMultipleLines(pWindowData, pIterator->oStart.iLine);
//      
//   /// Move the caret to the selection origin
//   setCodeEditCaretLocation(pWindowData, &pWindowData->oSelection.oOrigin);
//
//   /// Manually remove selection flag
//   pWindowData->oSelection.bExists = FALSE;   
//
//   // Cleanup
//   deleteCodeEditIterator(pIterator);
//}
