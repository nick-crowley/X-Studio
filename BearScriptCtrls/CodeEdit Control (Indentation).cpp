//
// CodeEdit Indentation (new).cpp : Calculates line indentation
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


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : identifyLineClassFromCommandConditional
// Description     : Classifies a line depending on how it affects code indentation
// 
// CONST UINT            iCommandID   : [in] Command ID
// CONST CONDITIONAL_ID  eConditional : [in] Conditional
// 
// Return Value   : Line class
// 
LINE_CLASS  identifyLineClassFromCommandConditional(CONST UINT  iCommandID, CONST CONDITIONAL_ID  eConditional)
{
   LINE_CLASS   eOutput;    // Operation result

   // Examine command
   switch (iCommandID)
   {
   // [COMMENT/NONE] Return COMMENT
   case CMD_COMMENT:
   case CMD_COMMAND_COMMENT:
   case CMD_NONE:
   case CMD_NOP:
      eOutput = LC_COMMENT_NOP;
      break;

   // [FOR LOOPS] Return IF/WHILE
   case CMD_FOR_EACH_COUNTER:
   case CMD_FOR_EACH:
   case CMD_FOR_LOOP:
      eOutput = LC_IF_WHILE;
      break;

   // [COMMAND] Examine conditional
   default:
      switch (eConditional)
      {
      case CI_NONE:
      case CI_DISCARD:
      case CI_START:
      case CI_BREAK:
      case CI_CONTINUE:
      case CI_LABEL:
      case CI_GOTO_LABEL:
      case CI_GOTO_SUB:
      case CI_END_SUB:        
         eOutput = LC_COMMAND;
         break;

      case CI_IF:
      case CI_IF_NOT:
      case CI_WHILE:
      case CI_WHILE_NOT:      
         eOutput = LC_IF_WHILE;   
         break;

      case CI_SKIP_IF:
      case CI_SKIP_IF_NOT:
         eOutput = LC_SKIP_IF;
         break;

      case CI_ELSE:
      case CI_ELSE_IF:
      case CI_ELSE_IF_NOT:
         eOutput = LC_ELSE_IF;
         break;
      
      case CI_END:
         eOutput = LC_END;
         break;
      }
   }

   // Return result
   return eOutput;
}


/// Function name  : isCodeEditLineIndentedDueToSkipIf
// Description     : Determines whether a line is indented due a previous SKIP-IF command, and returns the SKIP-IF if found
// 
// CONST LIST_ITEM*  pLineItem : [in]  ListItem containing the line to test
// CODE_EDIT_LINE*  &pOutput   : [out] SKIP-IF line data if found, otherwise NULL
// 
// Return Value   : TRUE if input line is indented due to a SKIP-IF, otherwise FALSE
// 
BOOL  isCodeEditLineIndentedDueToSkipIf(CONST LIST_ITEM*  pLineItem, CODE_EDIT_LINE*  &pOutput)
{
   CODE_EDIT_LINE*  pCurrentLine;
   BOOL             bSearching;

   // Prepare
   TRACK_FUNCTION();
   pCurrentLine = extractListItemPointer(pLineItem, CODE_EDIT_LINE);
   pOutput      = NULL;
   bSearching   = TRUE;

   // [CHECK] Ensure input line is not actually SKIP-IF
   if (pCurrentLine->eConditional == CI_SKIP_IF OR pCurrentLine->eConditional == CI_SKIP_IF_NOT)
      return FALSE;

   /// Search PRECEEDING lines (backwards) for SKIP-IF
   for (CONST LIST_ITEM* pIterator = pLineItem->pPrev; bSearching AND (pCurrentLine = extractListItemPointer(pIterator, CODE_EDIT_LINE)); pIterator = pIterator->pPrev)
   {
      // Examine COMMAND ID of the current line
      switch (pCurrentLine->iCommandID)
      {
      /// [COMMENT] Skip these since they don't affect indentation
      case CMD_COMMENT:
      case CMD_COMMAND_COMMENT:
      case CMD_NOP:
      case CMD_NONE:
         continue;
      }

      // [COMMAND] Since SKIP-IF can only indent exactly one COMMAND, finding anything other than SKIP-IF indicates 
      //             that the input line was not indented due to SKIP-IF
      switch (pCurrentLine->eConditional)
      {
      /// [FOUND] Set result to 'SKIP-IF' and abort search
      case CI_SKIP_IF:
      case CI_SKIP_IF_NOT:
         bSearching = FALSE;
         pOutput    = pCurrentLine;
         break;

      /// [NOT-FOUND] Return NULL
      default:
         bSearching = FALSE;
         break;
      }
   }

   // Return TRUE if SKIP-IF was found, otherwise return FALSE
   END_TRACKING();
   return (pOutput != NULL);
}


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : calculateCodeEditIndentationForMultipleLines
// Description     : Calculates the indentation of a specified line and all subsequent lines
// 
// CODE_EDIT_DATA*  pWindowData : [in] Window data
// CONST UINT       iStartLine  : [in] Zero based line number of the line to start from
// 
// Return type : Zero based indentation value
//
VOID   calculateCodeEditIndentationForMultipleLines(CODE_EDIT_DATA*  pWindowData, CONST UINT  iStartLine)
{
   CODE_EDIT_LINE*   pCurrentLine;     // LineData of the line currently being processed
   LIST_ITEM*        pLineIterator;    // LineData iterator
   INT               iIndentation,     // New indentation for the line currently being processed
                     iLineNumber;      // Zero-based line number of the line currently being processed

   // Prepare
   iLineNumber = iStartLine;

   // Iterate through all lines, starting from the input line
   for (findCodeEditLineListItemByIndex(pWindowData, iStartLine, pLineIterator); pLineIterator; pLineIterator = pLineIterator->pNext)
   {
      // Prepare
      pCurrentLine = extractListItemPointer(pLineIterator, CODE_EDIT_LINE);

      /// [FIRST LINE] Indentation must always be zero
      if (pLineIterator->pPrev == NULL)
         iIndentation = 0;

      /// [SUBSEQUENT LINE] Calculate line indentation based on previous lines
      else
      {
         iIndentation = calculateCodeEditIndentationForSingleLine(pCurrentLine, pLineIterator);
         iIndentation = max(0, iIndentation);   // Ensure calculate indentation hasn't become negative due a syntax error, like lots of ENDs without matching IFs
      }

      /// [CHANGED] Save new indentation and redraw line
      if (pCurrentLine->iIndent != iIndentation)
      {
         pCurrentLine->iIndent = iIndentation;
         updateCodeEditLine(pWindowData, iLineNumber, CCF_INDENTATION_CHANGED);
      }

      /// [SUB-ROUTINE] Indent code within sub-routines
      //if (pCurrentLine->iCommandID == CMD_END_SUB)
      //{
      //   CODE_EDIT_LINE*  pSubroutineLine;
      //   LIST_ITEM*       pSubIterator;
      //   UINT             iSubLineNumber;

      //   iSubLineNumber = iLineNumber - 1;

      //   // Iterate backwards through the lines we've just calculated
      //   for (pSubIterator = pLineIterator->pPrev; pSubroutineLine = extractListItemPointer(pSubIterator, CODE_EDIT_LINE); pSubIterator = pSubIterator->pPrev)
      //   {
      //      // [CHECK] Search for a matching label (if any)
      //      if (pSubroutineLine->iCommandID == CMD_DEFINE_LABEL)
      //      {
      //         // Move to the first line of the sub-routine
      //         pSubIterator = pSubIterator->pNext;
      //         iSubLineNumber++;

      //         // [CHECK] Ensure subroutine isn't empty
      //         if ((pSubroutineLine = extractListItemPointer(pSubIterator, CODE_EDIT_LINE)) AND pSubroutineLine->iCommandID != CMD_END_SUB)
      //         {
      //            /// [FOUND] Increase the indentation of the first line of the subroutine
      //            pSubroutineLine->iIndent = 1 + calculateCodeEditIndentationForSingleLine(pSubroutineLine, pSubIterator);
      //            updateCodeEditLine(pWindowData, iSubLineNumber, CCF_INDENTATION_CHANGED);

      //            iSubLineNumber++;

      //            /// Recalculate indentation for the remaining lines of the subroutine
      //            for (pSubIterator = pSubIterator->pNext; (pSubroutineLine = extractListItemPointer(pSubIterator, CODE_EDIT_LINE)) AND (pSubIterator != pLineIterator); pSubIterator = pSubIterator->pNext)
      //            {
      //               pSubroutineLine->iIndent = calculateCodeEditIndentationForSingleLine(pSubroutineLine, pSubIterator);
      //               updateCodeEditLine(pWindowData, iSubLineNumber, CCF_INDENTATION_CHANGED);
      //               iSubLineNumber++;
      //            }
      //         }

      //         // [DONE] Abort..
      //         break;
      //      }
      //      // [CHECK] Abort if another 'end-sub' command is found
      //      else if (pSubroutineLine->iCommandID == CMD_END_SUB)
      //         break;

      //      // Update sub line number
      //      iSubLineNumber--;
      //   }
      //}

      // Update line number iterator
      iLineNumber++;
   }
}


/// Function name  : calculateCodeEditIndentationForSingleLine
// Description     : Calculates the indentation of a single line of code, depending on the previous lines
///                                       NB: Cannot be used on the first line of code
// 
// CONST CODE_EDIT_LINE*  pCurrentLine   : [in] 
// CONST LIST_ITEM*       pLineIterator  : [in] 
// 
// Return Value   : Line indentation, may be negative
// 
INT  calculateCodeEditIndentationForSingleLine(CONST CODE_EDIT_LINE*  pCurrentLine, CONST LIST_ITEM*  pLineIterator)
{
   CODE_EDIT_LINE  *pPreviousLine,      // LineData of the previous line
                   *pSkipIfLine,        // [DUMMY] LineData used for detecting a SKIP-IF causing an indent of the previous line
                   *pSearchLine;        // LineData used for scanning for end of a subroutine
   LINE_CLASS       eCurrentClass,      // Class of the input line
                    ePreviousClass;     // Class of the previous line
   INT              iOutput;            // Operation result

   // [CHECK] Ensure this not the first line
   ASSERT(pLineIterator->pPrev != NULL);

   // Prepare
   pPreviousLine = extractListItemPointer(pLineIterator->pPrev, CODE_EDIT_LINE);

   // Determine class of input line and previous line
   eCurrentClass  = identifyLineClassFromCommandConditional(pCurrentLine->iCommandID,  pCurrentLine->eConditional);
   ePreviousClass = identifyLineClassFromCommandConditional(pPreviousLine->iCommandID, pPreviousLine->eConditional);

   // Examine class
   switch (eCurrentClass)
   {
   /// [COMMENT/NOP]
   case LC_COMMENT_NOP:
      // Examine previous line class
      switch (ePreviousClass)
      {
      // [COMMENT/END] Equal
      case LC_COMMENT_NOP:
      case LC_END:
         iOutput = pPreviousLine->iIndent;
         break;

      // [IF/WHILE/SKIP-IF/ELSE/ELSE-IF] Plus 1
      case LC_IF_WHILE:
      case LC_SKIP_IF:
      case LC_ELSE_IF:
         iOutput = pPreviousLine->iIndent + 1;
         break;

      // [COMMAND] Equal unless previous is indented due to SKIP-IF, then Minus 1
      case LC_COMMAND:
         if (!isCodeEditLineIndentedDueToSkipIf(pLineIterator->pPrev, pSkipIfLine))
            iOutput = pPreviousLine->iIndent;
         else
            iOutput = pPreviousLine->iIndent - 1;
         break;
      }
      break;

   /// [IF/WHILE/SKIP-IF]
   case LC_IF_WHILE:
   case LC_SKIP_IF:
      // Examine previous line class
      switch (ePreviousClass)
      {
      // [SKIP-IF/END] Equal
      case LC_SKIP_IF:
      case LC_END:
         iOutput = pPreviousLine->iIndent;
         break;

      // [IF/WHILE/ELSE/ELSE-IF] Plus 1
      case LC_IF_WHILE:
      case LC_ELSE_IF:
         iOutput = pPreviousLine->iIndent + 1;
         break;

      // [COMMAND/COMMENT/NOP] Equal unless previous is indented due to SKIP-IF, then Minus 1
      case LC_COMMAND:
      case LC_COMMENT_NOP:
         if (!isCodeEditLineIndentedDueToSkipIf(pLineIterator->pPrev, pSkipIfLine))
            iOutput = pPreviousLine->iIndent;
         else
            iOutput = pPreviousLine->iIndent - 1;
         break;
      }
      break;

   /// [ELSE/ELSE-IF/END]
   case LC_ELSE_IF:
   case LC_END:
      // Examine previous line class
      switch (ePreviousClass)
      {
      // [IF/WHILE/SKIP-IF/ELSE/ELSE-IF] Equal
      case LC_IF_WHILE:
      case LC_SKIP_IF:
      case LC_ELSE_IF:
         iOutput = pPreviousLine->iIndent;
         break;

      // [END/COMMENT] Minus 1
      case LC_END:
      case LC_COMMENT_NOP:
         iOutput = pPreviousLine->iIndent - 1;
         break;

      // [COMMAND] Minus one unless previous is indented due to SKIP-IF, then Minus 2
      case LC_COMMAND:
         if (!isCodeEditLineIndentedDueToSkipIf(pLineIterator->pPrev, pSkipIfLine))
            iOutput = pPreviousLine->iIndent - 1;
         else
            iOutput = pPreviousLine->iIndent - 2;
         break;
      }
      break;

   /// [COMMAND]
   case LC_COMMAND:
      // Examine previous line class
      switch (ePreviousClass)
      {
      // [IF/WHILE/SKIP-IF/ELSE/ELSE-IF] Plus 1
      case LC_IF_WHILE:
      case LC_SKIP_IF:
      case LC_ELSE_IF:
         iOutput = pPreviousLine->iIndent + 1;
         break;

      // [END/COMMENT] Equal
      case LC_END:
      case LC_COMMENT_NOP:
         iOutput = pPreviousLine->iIndent;
         break;

      // [COMMAND] Equal unless previous is indented due to SKIP-IF, then Minus 1
      case LC_COMMAND:
         if (!isCodeEditLineIndentedDueToSkipIf(pLineIterator->pPrev, pSkipIfLine))
            iOutput = pPreviousLine->iIndent;
         else
            iOutput = pPreviousLine->iIndent - 1;
         break;
      }
      break;
   }

   /// NEW: Indent subroutine
   // [CHECK] Is line beneath a define label?
   if (pPreviousLine->iCommandID == CMD_DEFINE_LABEL)
   {
      // Search for a matching 'end sub' command, but abort if 'Define Label' is found
      for (CONST LIST_ITEM*  pIterator = pLineIterator; (pSearchLine = extractListItemPointer(pIterator, CODE_EDIT_LINE)) AND pSearchLine->iCommandID != CMD_DEFINE_LABEL; pIterator = pIterator->pNext)
      {
         // [FOUND] Indent current line
         if (pSearchLine->iCommandID == CMD_END_SUB)
         {
            iOutput++;
            break;
         }
      }
   }
   // [END-SUB] Undent current line
   else if (pCurrentLine->iCommandID == CMD_END_SUB)
      iOutput--;

   // Return indentation for input line
   return iOutput;
}


