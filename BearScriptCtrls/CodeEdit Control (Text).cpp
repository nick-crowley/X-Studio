//
// CodeEdit Text (new).cpp : Handles the addition and removal of text from the CodeEdit
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : insertCodeEditCharacterAtCaret
// Description     : Inserts a character or new-line at the position of the caret
// 
// CODE_EDIT_DATA*  pWindowData : [in] Window data
// CONST TCHAR       chCharacter : [in] Character to insert
// 
VOID  insertCodeEditCharacterAtCaret(CODE_EDIT_DATA*  pWindowData, CONST TCHAR  chCharacter)
{
   CODE_EDIT_LOCATION   oCaretLocation;    // Existing/New caret location
   CODE_EDIT_ITERATOR  *pIterator;         // Character iterator
   CODE_EDIT_LINE      *pLineData,         // LineData for the current line
                       *pNewLineData;      // LineData for the new line

   // Prepare
   TRACK_FUNCTION();
   oCaretLocation = pWindowData->oCaret.oLocation;

   // Examine character
   switch (chCharacter)
   {
   /// [CHARACTER] Insert character at caret and move caret right by one character
   default:
      // Insert character at the caret
      findCodeEditLineDataByIndex(pWindowData, oCaretLocation.iLine, pLineData);
      insertCodeEditCharacterByIndex(pLineData, oCaretLocation.iIndex, chCharacter);

      // [UNDO] Add character to undo stack
      pushCharacterToCodeEditUndoStack(pWindowData, chCharacter, UIT_ADDITION, &oCaretLocation);

      // Move caret one character to the right
      oCaretLocation.iIndex++;
      setCodeEditCaretLocation(pWindowData, &oCaretLocation);

      // [EVENT] Text has changed
      updateCodeEditLine(pWindowData, oCaretLocation.iLine, CCF_TEXT_CHANGED);

      // [SCRIPT ARGUMENTS] Insert script-call arguments when user types ':'
      if (chCharacter == ':' AND isCommandID(pLineData->pCommand, CMD_CALL_SCRIPT_VAR_ARGS))
         onCodeEditScriptCallOperator(pWindowData, pLineData);
      break;

   /// [TAB] Convert to four spaces
   case VK_TAB:
      insertCodeEditTextAtCaret(pWindowData, TEXT("    "), TRUE);
      break;

   /// [ENTER] Add a new line on the line following the caret, move the caret to it's start
   case VK_RETURN:
      // Insert empty line beneath caret
      insertCodeEditLineByIndex(pWindowData, oCaretLocation.iLine + 1, pNewLineData = createCodeEditLine());

      // [UNDO] Add newline to undo stack
      pushCharacterToCodeEditUndoStack(pWindowData, '\r', UIT_ADDITION, &oCaretLocation);

      /// [NON-FINAL CHARACTER] Copy (and delete) characters following the caret to the new line
      if (pWindowData->oCaret.pCharacterItem)
      {
         // Lookup LineData for the caret line and create an iterator
         findCodeEditLineDataByIndex(pWindowData, oCaretLocation.iLine, pLineData);
         pIterator = createCodeEditSingleLineIterator(pLineData);

         // Iterate through all characters on the caret line
         while (findNextCodeEditCharacter(pIterator))
            // Skip characters preceeding the caret
            if (pIterator->oLocation.iIndex >= oCaretLocation.iIndex)
               // Append character to new line
               appendCodeEditCharacterToLine(pNewLineData, pIterator->chCharacter);

         // Iterate backwards through characters following the caret
         for (INT iCharIndex = (INT)getCodeEditLineLength(pLineData) - 1; iCharIndex >= (INT)oCaretLocation.iIndex; iCharIndex--)
            // Remove character from line
            removeCodeEditCharacterByIndex(pLineData, iCharIndex);

         // Cleanup
         deleteCodeEditIterator(pIterator);
      }

      // Generate a COMMAND for new line so the indentation can be properly calculated (the caret line will re-generated during the update below)
      updateCodeEditLineCommand(pWindowData, pNewLineData, NULL);

      // [EVENT] Text and line count has changed
      updateCodeEditLine(pWindowData, oCaretLocation.iLine, CCF_TEXT_CHANGED WITH CCF_LINES_CHANGED);

      // Move caret to the start of the new line, now that's indentation has been calculated by the event above
      oCaretLocation.iIndex = 0;
      oCaretLocation.iLine++;
      setCodeEditCaretLocation(pWindowData, &oCaretLocation);
      break;
   }

   // [TRACK]
   END_TRACKING();
}


/// Function name  : insertCodeEditScriptFile
// Description     : Fills a newly created CodeEdit window with the COMMANDs stored in a ScriptFile's 'translated' COMMAND list
// 
// CODE_EDIT_DATA*    pWindowData  : [in/out] Window data
// CONST SCRIPT_FILE*  pScriptFile  : [in]     ScriptFile containing the COMMANDs to insert
// 
VOID  insertCodeEditScriptFile(CODE_EDIT_DATA*  pWindowData, SCRIPT_FILE*  pScriptFile)
{
   CODE_EDIT_LINE*  pCurrentLine;      // Line data for the COMMAND currently being processed
   COMMAND*         pCommand;          // ScriptFile 'translated' COMMAND list iterator
   TCHAR*           szInputText;

   // [TRACK]
   TRACK_FUNCTION();

   // [CHECK] CodeEdit should only have one line
   ASSERT(getCodeEditLineCount(pWindowData) == 1);

   // Disable posting events to the parent
   enableCodeEditEvents(pWindowData, FALSE);

   /// Store ScriptFile
   pWindowData->pScriptFile = pScriptFile;

   /// [HAS TRANSLATOR] Indicates the ScriptFile contains translated commands
   if (pScriptFile->pTranslator)
   {
      // Remove empty first line
      removeCodeEditLineByIndex(pWindowData, 0);

      // Iterate through TRANSLATED OUTPUT
      for (UINT  iIndex = 0; findCommandInTranslatorOutput(pScriptFile->pTranslator, iIndex, pCommand); iIndex++)
      {
         /// Generate a new line from each COMMAND
         pCurrentLine = createCodeEditLineFromCommand(pWindowData, pCommand);
         insertCodeEditLineByIndex(pWindowData, iIndex, pCurrentLine);

         // Update maximum line length if necessary
         calculateCodeEditMaximumLineLength(pWindowData, iIndex, pCurrentLine);
      }
   }
   /// [HAS INPUT BUFFER] Indicates the ScriptFile represents a New Document / Text document
   else if (pScriptFile->szInputBuffer)
   {
      // [TEXT] Insert text at caret
      insertCodeEditTextAtCaret(pWindowData, szInputText = utilDuplicateSimpleString(pScriptFile->szInputBuffer), FALSE);     // InputBuffer will be deleted if Generate is initialised
      utilDeleteString(szInputText);
   }
   
   /// Empty all translation data and prepare for generation
   initScriptFileGenerator(pScriptFile);

   // [EVENT] Text and number of lines has changed
   updateCodeEditLine(pWindowData, 0, CCF_TEXT_CHANGED WITH CCF_LINES_CHANGED);

   // Re-enable posting events to the parent
   enableCodeEditEvents(pWindowData, TRUE);

   // [CHECK] Do not update scope without game data
   if (getAppState() == AS_GAME_DATA_LOADED)
      // [EVENT] Update current scope
      updateCodeEditScope(pWindowData);

   // [TRACK]
   END_TRACKING();
}


/// Function name  : insertCodeEditTextAtCaret
// Description     : Inserts a block of text at the caret, generates a COMMAND for each new line added and then updates the CodeEdit
// 
// CODE_EDIT_DATA*  pWindowData : [in] Window data
// CONST TCHAR*      szText      : [in] Null terminated string to insert
// 
VOID  insertCodeEditTextAtCaret(CODE_EDIT_DATA*  pWindowData, CONST TCHAR*  szText, CONST BOOL bEnableUndo)
{
   CODE_EDIT_LOCATION   oCaretLocation,      // Convenience copy of the initial caret location
                        oNewCaretLocation;
   CODE_EDIT_LINE*      pCurrentLineData;    // LineData for the line currently being processed
   LIST*                pSplitCharacters;    // Temporary storage for characters on the input line following the caret
   UINT                 iInitialLine,        // Line number of the line initially containing the caret
                        iCurrentLine;        // Line number of the currently being processed
   // Prepare
   TRACK_FUNCTION();
   pSplitCharacters = createList(NULL);
   oCaretLocation   = pWindowData->oCaret.oLocation;
   iCurrentLine     = oCaretLocation.iLine;
   iInitialLine     = oCaretLocation.iLine;

   // Lookup LineData for the line containing the caret
   findCodeEditLineDataByIndex(pWindowData, iInitialLine, pCurrentLineData);

   /// Temporarily remove any characters following the caret (if any)
   insertListIntoListByIndex(pSplitCharacters, 0, pCurrentLineData->pCharacterList, oCaretLocation.iIndex);

   // Iterate through input string
   for (CONST TCHAR*  szIterator = szText; szIterator[0]; szIterator++)
   {
      // Examine character
      switch (szIterator[0])
      {
      /// [CHARACTER] Append to current line
      default:
         appendCodeEditCharacterToLine(pCurrentLineData, szIterator[0]);
         break;

      /// [TAB] Append four spaces to current line
      case VK_TAB:
         for (UINT iTabSize = 0; iTabSize < 4; iTabSize++)
            appendCodeEditCharacterToLine(pCurrentLineData, ' ');
         break;

      /// [ENTER] Add new line and set as the 'current line'
      case VK_RETURN:
         // [MULTIPLE-NEW-LINES] Generate a COMMAND for each new line, so the indentation is properly calculated during the update
         if (iCurrentLine != iInitialLine)
            updateCodeEditLineCommand(pWindowData, pCurrentLineData, NULL);

         // Insert new line beneath current. Advance line number iterator. Set current line to new line.
         insertCodeEditLineByIndex(pWindowData, ++iCurrentLine, pCurrentLineData = createCodeEditLine());
         break;

      // [LINE-FEED] Drop character
      case VK_LINE_FEED:
         break;
      }
   }

   // Calculate new Caret position as last inserted character
   oNewCaretLocation.iLine  = iCurrentLine;
   oNewCaretLocation.iIndex = getCodeEditLineLength(pCurrentLineData);

   /// [UNDO] Add to undo queue
   if (bEnableUndo)
      pushStringToCodeEditUndoStack(pWindowData, szText, UIT_ADDITION, &oCaretLocation, &oNewCaretLocation);

   /// Re-attach characters previously removed
   insertListIntoListByIndex(pCurrentLineData->pCharacterList, oNewCaretLocation.iIndex, pSplitCharacters, 0);

   // Move caret (now that character list has been updated)
   setCodeEditCaretLocation(pWindowData, &oNewCaretLocation);

   // [FINAL NEW LINE] Generate a COMMAND for the last new line added (not possible above) so the indentation is properly calculated during the update
   if (iCurrentLine != iInitialLine)
      updateCodeEditLineCommand(pWindowData, pCurrentLineData, NULL);

   /// [EVENT] Text has changed (possibly line count too)
   updateCodeEditLine(pWindowData, iInitialLine, (iInitialLine == iCurrentLine ? CCF_TEXT_CHANGED : CCF_TEXT_CHANGED WITH CCF_LINES_CHANGED));

   // Cleanup
   deleteList(pSplitCharacters);
   END_TRACKING();
}


/// Function name  : performCodeEditCaretCommand
// Description     : Implements the keyboard interface for moving the caret. 
//                      eg. "move to next word", "move to word above" etc.
// 
// CODE_EDIT_DATA*  pWindowData       : [in] Window data
// CONST UINT        eMovement         : [in] Virtual key representing UP, DOWN, LEFT or RIGHT
//                                            -OR- Menu command ID defining WORD-LEFT, WORD-RIGHT, LINE-UP or LINE-DOWN
// CONST BOOL        bExtendSelection  : [in] Whether the SHIFT key is pressed or not
// 
VOID  performCodeEditCaretCommand(CODE_EDIT_DATA*  pWindowData, CONST UINT  eMovement, CONST BOOL  bExtendSelection)
{
   LIST_ITEM*           pCaretCharacter;     // Current caret character item
   CODE_EDIT_LOCATION   oNewCaretLocation,   // New position for the caret, depending on the movement requested
                        oOldCaretLocation;   // Existing caret location
   CODE_EDIT_LINE*      pLineData;           // LineData for the line preceeding/following the line being examined
   POINT                ptCaretPoint;        // Current caret position, in client co-ordinates
   BOOL                 bWordCommand;        // Whether command is move to next/prev word

   // Prepare
   TRACK_FUNCTION();
   oOldCaretLocation = oNewCaretLocation = pWindowData->oCaret.oLocation;
   pCaretCharacter   = pWindowData->oCaret.pCharacterItem;
   bWordCommand      = FALSE;

   // [REMOVE SELECTION] Cancel current selection if we're not extending it.
   if (!bExtendSelection)
      removeCodeEditSelection(pWindowData);
   
   // Examine type of movement required
   switch (eMovement)
   {
   /// [CHARACTER LEFT] Move LEFT one character, or to the END of the PREVIOUS LINE
   case VK_LEFT:
      // [NON-INITIAL CHARACTER] Move left one character
      if (oNewCaretLocation.iIndex > 0)
         oNewCaretLocation.iIndex--;
         
      // [NON-INITIAL LINE] Move to the end of the previous line
      else if (findCodeEditLineDataByIndex(pWindowData, oNewCaretLocation.iLine - 1, pLineData))
      {
         oNewCaretLocation.iLine--;
         oNewCaretLocation.iIndex = getCodeEditLineLength(pLineData);
      }
      break;

   /// [CHARACTER RIGHT] Move RIGHT one character, or to the START of the NEXT LINE
   case VK_RIGHT:
      // [NON-FINAL CHARACTER] Move right one character
      if (pCaretCharacter)
         oNewCaretLocation.iIndex++;

      // [NON-FINAL LINE] Move to the start of the next line
      else if (findCodeEditLineDataByIndex(pWindowData, oNewCaretLocation.iLine + 1, pLineData))
      {
         oNewCaretLocation.iLine++;
         oNewCaretLocation.iIndex = 0;
      }
      break;

   /// [CHARACTER UP] Move VERTICALLY upwards, if possible, possibly to a radically different character index
   case VK_UP:
      // [CHECK] Ensure an upward move is possible
      if (findCodeEditLineDataByIndex(pWindowData, oNewCaretLocation.iLine, pLineData))
      {
         // Convert current caret location into client co-ordinates
         calculateCodeEditPointFromLocation(pWindowData, &oNewCaretLocation, &ptCaretPoint);

         // Move position vertically upwards by one line
         ptCaretPoint.y -= pWindowData->siCharacterSize.cy;

         // Convert client co-ordinates back into a location
         calculateCodeEditLocationFromPoint(pWindowData, &ptCaretPoint, &oNewCaretLocation);
      }
      break;

   /// [CHARACTER UP] Move VERTICALLY downwards, if possible, possibly to a radically different character index
   case VK_DOWN:
      // [CHECK] Ensure an downward move is possible
      if (findCodeEditLineDataByIndex(pWindowData, oNewCaretLocation.iLine, pLineData))
      {
         // Convert current caret location into client co-ordinates
         calculateCodeEditPointFromLocation(pWindowData, &oNewCaretLocation, &ptCaretPoint);

         // Move position vertically upwards by one line
         ptCaretPoint.y += pWindowData->siCharacterSize.cy;

         // Convert client co-ordinates back into a location
         if (!calculateCodeEditLocationFromPoint(pWindowData, &ptCaretPoint, &oNewCaretLocation))
            calculateCodeEditFinalCharacterLocation(pWindowData, &oNewCaretLocation);
      }
      break;

   /// [WORD LEFT] Move to the previous non-whitespace CodeObject, or the end of the previous line
   case IDM_EDIT_WORD_LEFT:
      // [INITIAL CHARACTER] Move to the end of the previous line
      if (oNewCaretLocation.iIndex == 0)
      {
         performCodeEditCaretCommand(pWindowData, VK_LEFT, bExtendSelection);
         bWordCommand = TRUE;
      }
      // [NON-INITIAL CHARACTER] Move to the previous CodeObject
      else
      {
         findCodeEditLineDataByIndex(pWindowData, oNewCaretLocation.iLine, pLineData);
         oNewCaretLocation.iIndex = findPrevCodeEditWord(pLineData, oNewCaretLocation.iIndex);
      }
      break;

   /// [WORD RIGHT] Move to the next non-whitespace CodeObject, or the start of the next line
   case IDM_EDIT_WORD_RIGHT:
      // [FINAL CHARACTER] Move to the end of the previous line
      if (!pCaretCharacter)
      {
         performCodeEditCaretCommand(pWindowData, VK_RIGHT, bExtendSelection);
         bWordCommand = TRUE;
      }
      // [NON-FINAL CHARACTER] Move to the next CodeObject
      else
      {
         findCodeEditLineDataByIndex(pWindowData, oNewCaretLocation.iLine, pLineData);
         oNewCaretLocation.iIndex = findNextCodeEditWord(pLineData, oNewCaretLocation.iIndex);
      }
      break;
   }

   // [CHECK] Ensure this isn't a recursive call
   if (!bWordCommand)
   {
      /// Move caret to it's new position
      setCodeEditCaretLocation(pWindowData, &oNewCaretLocation);

      // [EXTEND SELECTION] Update text selection to reflect new caret position
      if (bExtendSelection)
         updateCodeEditSelection(pWindowData, oOldCaretLocation);
   }

   // [TRACK]
   END_TRACKING();
}


/// Function name  : removeCodeEditCharacterAtCaret
// Description     : Deletes the character preceeding or following the caret, depending on whether DELETE or
//                    -> BACKSPACE was pressed
// 
// CODE_EDIT_DATA*  pWindowData   : [in] Window data
// CONST UINT        iVirtualKey   : [in] Must be VK_DELETE or VK_CLEAR
// 
VOID  removeCodeEditCharacterAtCaret(CODE_EDIT_DATA*  pWindowData, CONST UINT  iVirtualKey)
{
   CODE_EDIT_CHARACTER *pTargetChar;
   CODE_EDIT_CARET      oCaret;                 // Convenience copy of the current caret position
   CODE_EDIT_ITERATOR  *pIterator;              // Character data iterator
   CODE_EDIT_LINE      *pCurrentLineData,       // LineData for the line containing the caret
                       *pFollowingLineData,     // LineData for the line following the line containing the caret
                       *pPreceedingLineData;    // LineData for the line preceeding the line that contains the caret

   // [TRACK]
   TRACK_FUNCTION();

   // [CHECK] Ensure key is DELETE or BACKSPACE
   ASSERT(iVirtualKey == VK_DELETE OR iVirtualKey == VK_BACK);

   // Prepare
   oCaret = pWindowData->oCaret;

   // Lookup LineData for line containing CARET
   findCodeEditLineDataByIndex(pWindowData, oCaret.oLocation.iLine, pCurrentLineData);

   /// Examine input key
   switch (iVirtualKey)
   {
   // [DELETE]
   case VK_DELETE:
      /// [DELETE FIRST/MIDDLE CHARACTER] Delete the next character, do not move the caret
      if (oCaret.pCharacterItem)
      {
         // [UNDO] Add target character to undo stack
         findCodeEditCharacterDataByIndex(pCurrentLineData, oCaret.oLocation.iIndex, pTargetChar);
         pushCharacterToCodeEditUndoStack(pWindowData, pTargetChar->chCharacter, UIT_DELETION, &oCaret.oLocation);

         // Delete the next character 
         removeCodeEditCharacterByIndex(pCurrentLineData, oCaret.oLocation.iIndex);
         // [EVENT] Text has changed
         updateCodeEditLine(pWindowData, oCaret.oLocation.iLine, CCF_TEXT_CHANGED);
      }
      /// [DELETE LAST CHARACTER] Join the following line (if any) to the end of the current line. Do not move caret.
      else if (findCodeEditLineDataByIndex(pWindowData, oCaret.oLocation.iLine + 1, pFollowingLineData))
      {
         // Create iterator for following line
         pIterator = createCodeEditSingleLineIterator(pFollowingLineData);

         // [UNDO] Add newline character to undo stack
         pushCharacterToCodeEditUndoStack(pWindowData, '\r', UIT_DELETION, &oCaret.oLocation);

         /// Iterate through all characters on the following line
         while (findNextCodeEditCharacter(pIterator))
            // Append to the current line
            appendCodeEditCharacterToLine(pCurrentLineData, pIterator->chCharacter);

         /// Delete the following line
         removeCodeEditLineByIndex(pWindowData, oCaret.oLocation.iLine + 1);

         // [EVENT] Text and line count has changed
         updateCodeEditLine(pWindowData, oCaret.oLocation.iLine, CCF_TEXT_CHANGED WITH CCF_LINES_CHANGED);
         deleteCodeEditIterator(pIterator);
      }
      break;

   // [BACKSPACE]
   case VK_BACK:
      /// [BACKSPACE MIDDLE/LAST CHARACTER] Delete the previous character and move the caret backwards one character
      if (oCaret.oLocation.iIndex > 0)
      {
         // [UNDO] Move caret back one character and Add character to undo stack
         findCodeEditCharacterDataByIndex(pCurrentLineData, --oCaret.oLocation.iIndex, pTargetChar);
         pushCharacterToCodeEditUndoStack(pWindowData, pTargetChar->chCharacter, UIT_DELETION, &oCaret.oLocation);

         // Delete preceeding character
         removeCodeEditCharacterByIndex(pCurrentLineData, oCaret.oLocation.iIndex);

         // [EVENT] Text has changed
         updateCodeEditLine(pWindowData, oCaret.oLocation.iLine, CCF_TEXT_CHANGED);
      }
      /// [BACKSPACE FIRST CHARACTER] Join the current line to the end of the previous line (if any). Move caret to between new and existing text
      else if (findCodeEditLineDataByIndex(pWindowData, oCaret.oLocation.iLine - 1, pPreceedingLineData))
      {
         // Create iterator the current line
         pIterator = createCodeEditSingleLineIterator(pCurrentLineData);

         // Position caret at the join between the lines
         oCaret.oLocation.iLine--;
         oCaret.oLocation.iIndex = getCodeEditLineLength(pPreceedingLineData);

         // [UNDO] Add newline character to undo stack
         pushCharacterToCodeEditUndoStack(pWindowData, '\r', UIT_DELETION, &oCaret.oLocation);

         /// Iterate through all characters on the current line
         while (findNextCodeEditCharacter(pIterator))
            // Append to the line above
            appendCodeEditCharacterToLine(pPreceedingLineData, pIterator->chCharacter);

         /// Delete the current line
         removeCodeEditLineByIndex(pWindowData, oCaret.oLocation.iLine + 1);  /// NB: Caret is now on the preceeding line, so current is now 'caret + 1'

         // [EVENT] Text and line count has changed
         updateCodeEditLine(pWindowData, oCaret.oLocation.iLine, CCF_TEXT_CHANGED WITH CCF_LINES_CHANGED);

         // Cleanup
         deleteCodeEditIterator(pIterator);
      }
      break;
   }

   // Update caret location
   setCodeEditCaretLocation(pWindowData, &oCaret.oLocation);
   END_TRACKING();
}



/// Function name  : removeCodeEditSelection
// Description     : Removes any existing selection from the CodeEdit
// 
// CODE_EDIT_DATA*  pWindowData   : [in] Window data
// 
VOID   removeCodeEditSelection(CODE_EDIT_DATA*  pWindowData)
{
   // [TRACK]
   TRACK_FUNCTION();

   // [CHECK] Ensure selection exists
   if (hasCodeEditSelection(pWindowData))
   {
      // Remove selection between CARET and ORIGIN
      setCodeEditSelection(pWindowData, pWindowData->oSelection.oOrigin, pWindowData->oCaret.oLocation, FALSE);

      // Update selection flag
      pWindowData->oSelection.bExists = FALSE;
   }

   // [TRACK]
   END_TRACKING();
}


/// Function name  : removeCodeEditSelectionText
// Description     : Deletes the current text selection, repositions the caret and invalidates the affected lines
// 
// CODE_EDIT_DATA*  pWindowData   : [in] Window data
// 
VOID   removeCodeEditSelectionText(CODE_EDIT_DATA*  pWindowData, CONST BOOL  bEnableUndo)
{
   CODE_EDIT_LOCATION  oStart,               // Location of first character in the selection
                       oFinish;              // Location of last character in the selection
   COMPARISON_RESULT   eComparison;          // Location comparison result
   CODE_EDIT_LINE     *pLineData,            // LineData for the line currently being processed
                      *pJoinLineData;        // LineData for the final line in a multi-line selection, used for joining them after deletion process
   LIST_ITEM          *pCharacterIterator;   // CharacterData iterator used for joining the first and last lines of a multi-line selection
   TCHAR              *szSelection;          // [UNDO] Selection text before deletion
   INT                 iCurrentLine,         // Line number of the line currently being processed
                       iCurrentCharIndex;    // Character index of the character currently being deleted

   // [TRACK]
   TRACK_FUNCTION();

   // [CHECK] Ensure selection exists
   if (hasCodeEditSelection(pWindowData))
   {
      /// Determine whether CARET preceeds ORIGIN or vice versa
      switch (eComparison = compareCodeEditLocations(pWindowData->oSelection.oOrigin, pWindowData->oCaret.oLocation))
      {
      case CR_LESSER:    oStart = pWindowData->oSelection.oOrigin;   oFinish = pWindowData->oCaret.oLocation;      break;
      case CR_GREATER:   oStart = pWindowData->oCaret.oLocation;     oFinish = pWindowData->oSelection.oOrigin;    break;
      // [EQUAL] User has pressed SHIFT + DELETE, causing a new empty selection - cancel it
      case CR_EQUAL:     removeCodeEditSelection(pWindowData);                                                     break;
      }

      // [CHECK] Ensure we haven't already removed the selection
      if (eComparison != CR_EQUAL)
      {
         // Prepare
         iCurrentLine = oFinish.iLine;

         // [UNDO] Add to undo queue
         if (bEnableUndo)
         {
            pushStringToCodeEditUndoStack(pWindowData, szSelection = generateCodeEditSelectionText(pWindowData), UIT_DELETION, &oStart, &oFinish);
            utilDeleteString(szSelection);
         }

         /// Iterate backwards through all lines from FINISH to START
         for (findCodeEditLineDataByIndex(pWindowData, oFinish.iLine, pLineData); iCurrentLine >= (INT)oStart.iLine; findCodeEditLineDataByIndex(pWindowData, --iCurrentLine, pLineData))
         {
            /// [SINGLE LINE SELECTION] Delete characters between FINISH and START.  Redraw line
            if (oStart.iLine == oFinish.iLine)
            {
               // Iterate backwards through all characters between FINISH and START
               for (iCurrentCharIndex = (INT)oFinish.iIndex - 1; iCurrentCharIndex >= (INT)oStart.iIndex; iCurrentCharIndex--)
                  // Delete current character
                  removeCodeEditCharacterByIndex(pLineData, iCurrentCharIndex);

               // [EVENT] Text has changed
               updateCodeEditLine(pWindowData, iCurrentLine, CCF_TEXT_CHANGED);
            }
            /// [LAST LINE] Delete all characters preceeding FINISH
            else if (iCurrentLine == oFinish.iLine)
            {
               // Iterate backwards through all characters between FINISH and the beginning of the line
               for (iCurrentCharIndex = (INT)oFinish.iIndex - 1; iCurrentCharIndex >= 0; iCurrentCharIndex--)
                  // Delete current character
                  removeCodeEditCharacterByIndex(pLineData, iCurrentCharIndex);
            }
            /// [MIDDLE LINE] Delete entire line
            else if (iCurrentLine != oStart.iLine)
               removeCodeEditLineByIndex(pWindowData, iCurrentLine);
               
            /// [FIRST LINE] Delete all characters following START
            else
            {
               // Iterate backwards through all characters between the end of the line and START
               for (iCurrentCharIndex = getCodeEditLineLength(pLineData) - 1; iCurrentCharIndex >= (INT)oStart.iIndex; iCurrentCharIndex--)
                  // Delete current character
                  removeCodeEditCharacterByIndex(pLineData, iCurrentCharIndex);
            }
         }

         /// [MULTI-LINE SELECTION] Join the first and last lines into a single line
         if (oStart.iLine != oFinish.iLine)
         {
            // Recalculate the position of FINISH
            oFinish.iLine = oStart.iLine + 1;

            // Lookup LineData for START and FINISH
            findCodeEditLineDataByIndex(pWindowData, oStart.iLine,  pLineData);        
            findCodeEditLineDataByIndex(pWindowData, oFinish.iLine, pJoinLineData);

            // Iterate forwards through all characters on FINISH
            for (findCodeEditCharacterListItemByIndex(pJoinLineData, 0, pCharacterIterator); pCharacterIterator; pCharacterIterator = pCharacterIterator->pNext)
               // Append each character to START
               appendCodeEditCharacterToLine(pLineData, extractListItemPointer(pCharacterIterator, CODE_EDIT_CHARACTER)->chCharacter);
            
            // Delete FINISH
            removeCodeEditLineByIndex(pWindowData, oFinish.iLine);

            // [EVENT] Text and line count has changed
            updateCodeEditLine(pWindowData, oStart.iLine, CCF_TEXT_CHANGED WITH CCF_LINES_CHANGED);
         }

         // Move the caret to the START
         setCodeEditCaretLocation(pWindowData, &oStart);

         /// Manually remove selection flag
         pWindowData->oSelection.bExists = FALSE;
      }
   }

   // [TRACK]
   END_TRACKING();
}

