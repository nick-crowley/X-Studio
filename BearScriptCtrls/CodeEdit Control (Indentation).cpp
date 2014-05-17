//
// CodeEdit Indentation (new).cpp : Calculates line indentation
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"
#include <set>

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////

// Helpers
LINE_CLASS  identifyLineClass(CONST UINT  iCommandID, CONST CONDITIONAL_ID  eConditional);
BOOL        isLineIndentedDueToSkipIf(CONST LIST_ITEM*  pLineItem, CODE_EDIT_LINE*  &pOutput);

// Functions
INT     calculateLineIndentation(CONST CODE_EDIT_LINE*  pCurrentLine, CONST LIST_ITEM*  pLineIterator);

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/*
public string[]  GetLines(XStringLibrary  lib, XTypeLibrary  types, int  indent)
{
   if (lib==null) throw new ArgumentNullException("lib");
   if (types==null) throw new ArgumentNullException("types");

   // Prepare
   var lines = new List<string>(StdCommands.Count+AuxCommands.Count); 
   var branch = new Stack<XBranchLogic>();
   
   // Examine commands
   foreach (var c in Commands)
   {
      // Push/pop branching logic from stack
      var logic = c.BranchLogic;
      switch (logic)
      {
      case XBranchLogic.Break:
      case XBranchLogic.Continue: 
         branch.Push(logic); 
         break;

      case XBranchLogic.Else:    
      case XBranchLogic.End:      
         branch.Pop();       
         break;
      }

      // Resolve/Indent/Append command text
      lines.Add(new string(' ', indent*branch.Count) + c.Lookup(lib, types));

      // Re-Examine stack
      switch (logic)
      {
      case XBranchLogic.If:
      case XBranchLogic.Else:
         branch.Push(logic);
         break;

      case XBranchLogic.Break:
      case XBranchLogic.Continue:
         branch.Pop();
         break;

      case XBranchLogic.SkipIf:
         branch.Push(logic); 
         continue;
      }

      // Pop 'SkipIf' after next standard command
      if (branch.Count > 0 && c.Syntax.Type == XCommandType.Standard && branch.Peek() == XBranchLogic.SkipIf)
         branch.Pop();
   }

   // Return formatted commands
   return lines.ToArray();
}
*/

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : identifyLineClass
// Description     : Classifies a line depending on how it affects code indentation
// 
// CONST UINT            iCommandID   : [in] Command ID
// CONST CONDITIONAL_ID  eConditional : [in] Conditional
// 
// Return Value   : Line class
// 
LINE_CLASS  identifyLineClass(CONST UINT  iCommandID, CONST CONDITIONAL_ID  eConditional)
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


/// Function name  : isLineIndentedDueToSkipIf
// Description     : Determines whether a line is indented due a previous SKIP-IF command, and returns the SKIP-IF if found
// 
// CONST LIST_ITEM*  pLineItem : [in]  ListItem containing the line to test
// CODE_EDIT_LINE*  &pOutput   : [out] SKIP-IF line data if found, otherwise NULL
// 
// Return Value   : TRUE if input line is indented due to a SKIP-IF, otherwise FALSE
// 
BOOL  isLineIndentedDueToSkipIf(CONST LIST_ITEM*  pLineItem, CODE_EDIT_LINE*  &pOutput)
{
   CODE_EDIT_LINE*  pCurrentLine;
   BOOL             bSearching;

   // Prepare
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
   return (pOutput != NULL);
}


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : calculateSubroutineIndentation
// Description     : Calculates indentation for all lines within a subroutine
// 
// CODE_EDIT_DATA*  pWindowData : [in] Window data
// std::set<int>    updates     : [in/out] Line numbers to be redrawn
// CONST UINT       iEndSub     : [in] Line number of 'endsub' command
// LIST_ITEM*       pEndSub     : [in] Line iterator for 'endsub' command
// 
VOID   calculateSubroutineIndentation(CODE_EDIT_DATA*  pWindowData, std::set<int>  updates, CONST UINT  iEndSub, LIST_ITEM*  pEndSub)
{
   CODE_EDIT_LINE*  pLine;
   LIST_ITEM*       pIterator;
   UINT             iLine;

   // Prepare
   iLine = iEndSub;

   // Search backwards for matching label
   for (pIterator = pEndSub->pPrev; pLine = extractListItemPointer(pIterator, CODE_EDIT_LINE); pIterator = pIterator->pPrev)
   {
      iLine--;

      // [CHECK] Define label?
      if (pLine->iCommandID == CMD_DEFINE_LABEL)
      {
         // Move to the first line of the sub-routine
         pIterator = pIterator->pNext;
         pLine = extractListItemPointer(pIterator, CODE_EDIT_LINE);
         iLine++;

         // [CHECK] Ensure subroutine isn't empty
         if (pLine->iCommandID != CMD_END_SUB)
         {
            // Indent +1 first line 
            pLine->iIndent = 1 + calculateLineIndentation(pLine, pIterator);
            updates.insert(iLine); 

            /// Iterate thru remaining subroutine  [excluding 'endsub']
            for (pIterator = pIterator->pNext; (pLine = extractListItemPointer(pIterator, CODE_EDIT_LINE)) AND (pIterator != pEndSub); pIterator = pIterator->pNext)
            {
               iLine++;

               // Recalculate indent + defer redraw
               pLine->iIndent = calculateLineIndentation(pLine, pIterator);
               updates.insert(iLine); 
            }
         }

         // [DONE] 
         break;
      }
   }
}

/// Function name  : calculateCodeEditIndentationForMultipleLines
// Description     : Calculates the indentation of a specified line and all subsequent lines
// 
// CODE_EDIT_DATA*  pWindowData : [in] Window data
// CONST UINT       iStartLine  : [in] Zero based line number of the line to start from
// 
// Return type : Zero based indentation value
//
VOID   calculateCodeEditIndentation(CODE_EDIT_DATA*  pWindowData, CONST UINT  iStartLine)
{
   CODE_EDIT_LINE*   pCurrentLine;     // LineData of the line currently being processed
   LIST_ITEM*        pLineIterator;    // LineData iterator
   INT               iIndentation,     // New indentation for the line currently being processed
                     iLineNumber;      // Zero-based line number of the line currently being processed
   std::set<int>     updates;          // HACK: Line numbers with modified indentation

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
         iIndentation = calculateLineIndentation(pCurrentLine, pLineIterator);
         iIndentation = max(0, iIndentation);   // Ensure calculate indentation hasn't become negative due a syntax error, like lots of ENDs without matching IFs
      }

      /// [CHANGED] Save new indentation and defer line update
      if (pCurrentLine->iIndent != iIndentation)
      {
         pCurrentLine->iIndent = iIndentation;
         updates.insert(iLineNumber); 
      }

      /// [SUB-ROUTINE] Indent code within sub-routines
      if (pCurrentLine->iCommandID == CMD_END_SUB && pCurrentLine->iIndent == 0)    // Ensure this is the final 'endsub', not within if/skip-if
         calculateSubroutineIndentation(pWindowData, updates, iLineNumber, pLineIterator);

      // Update line number iterator
      iLineNumber++;
   }

   // Redraw affected lines
   for (std::set<int>::const_iterator  it = updates.begin(); it != updates.end(); ++it)
      updateCodeEditLine(pWindowData, *it, CCF_INDENTATION_CHANGED);
}


/// Function name  : calculateLineIndentation
// Description     : Calculates the indentation of a single line of code, depending on the previous lines
///                                       NB: Cannot be used on the first line of code
// 
// CONST CODE_EDIT_LINE*  pCurrentLine   : [in] 
// CONST LIST_ITEM*       pLineIterator  : [in] 
// 
// Return Value   : Line indentation, may be negative
// 
INT  calculateLineIndentation(CONST CODE_EDIT_LINE*  pCurrentLine, CONST LIST_ITEM*  pLineIterator)
{
   CODE_EDIT_LINE  *pPreviousLine,      // LineData of the previous line
                   *pSkipIfLine;        // [DUMMY] LineData used for detecting a SKIP-IF causing an indent of the previous line
   LINE_CLASS       eCurrentClass,      // Class of the input line
                    ePreviousClass;     // Class of the previous line
   INT              newIndent;            // Operation result

   // [CHECK] Ensure this not the first line
   ASSERT(pLineIterator->pPrev != NULL);

   // Prepare
   pPreviousLine = extractListItemPointer(pLineIterator->pPrev, CODE_EDIT_LINE);

   // Determine class of input line and previous line
   eCurrentClass  = identifyLineClass(pCurrentLine->iCommandID,  pCurrentLine->eConditional);
   ePreviousClass = identifyLineClass(pPreviousLine->iCommandID, pPreviousLine->eConditional);

   // Examine line
   switch (eCurrentClass)
   {
   /// [COMMENT/NOP]
   case LC_COMMENT_NOP:
      // Examine previous 
      switch (ePreviousClass)
      {
      // [COMMENT/END] Equal
      case LC_COMMENT_NOP:
      case LC_END:
         newIndent = pPreviousLine->iIndent;
         break;

      // [IF/WHILE/SKIP-IF/ELSE/ELSE-IF] Plus 1
      case LC_IF_WHILE:
      case LC_SKIP_IF:
      case LC_ELSE_IF:
         newIndent = pPreviousLine->iIndent + 1;
         break;

      // [COMMAND] Equal unless previous is indented due to SKIP-IF, then Minus 1
      case LC_COMMAND:
         if (!isLineIndentedDueToSkipIf(pLineIterator->pPrev, pSkipIfLine))
            newIndent = pPreviousLine->iIndent;
         else
            newIndent = pPreviousLine->iIndent - 1;
         break;
      }
      break;

   /// [IF/WHILE/SKIP-IF]
   case LC_IF_WHILE:
   case LC_SKIP_IF:
      // Examine previous 
      switch (ePreviousClass)
      {
      // [SKIP-IF/END] Equal
      case LC_SKIP_IF:
      case LC_END:
         newIndent = pPreviousLine->iIndent;
         break;

      // [IF/WHILE/ELSE/ELSE-IF] Plus 1
      case LC_IF_WHILE:
      case LC_ELSE_IF:
         newIndent = pPreviousLine->iIndent + 1;
         break;

      // [COMMAND/COMMENT/NOP] Equal unless previous is indented due to SKIP-IF, then Minus 1
      case LC_COMMAND:
      case LC_COMMENT_NOP:
         if (!isLineIndentedDueToSkipIf(pLineIterator->pPrev, pSkipIfLine))
            newIndent = pPreviousLine->iIndent;
         else
            newIndent = pPreviousLine->iIndent - 1;
         break;
      }
      break;

   /// [ELSE/ELSE-IF/END]
   case LC_ELSE_IF:
   case LC_END:
      // Examine previous 
      switch (ePreviousClass)
      {
      // [IF/WHILE/SKIP-IF/ELSE/ELSE-IF] Equal
      case LC_IF_WHILE:
      case LC_SKIP_IF:
      case LC_ELSE_IF:
         newIndent = pPreviousLine->iIndent;
         break;

      // [END/COMMENT] Minus 1
      case LC_END:
      case LC_COMMENT_NOP:
         newIndent = pPreviousLine->iIndent - 1;
         break;

      // [COMMAND] Minus one unless previous is indented due to SKIP-IF, then Minus 2
      case LC_COMMAND:
         if (!isLineIndentedDueToSkipIf(pLineIterator->pPrev, pSkipIfLine))
            newIndent = pPreviousLine->iIndent - 1;
         else
            newIndent = pPreviousLine->iIndent - 2;
         break;
      }
      break;

   /// [COMMAND]
   case LC_COMMAND:
      // Examine previous 
      switch (ePreviousClass)
      {
      // [IF/WHILE/SKIP-IF/ELSE/ELSE-IF] Plus 1
      case LC_IF_WHILE:
      case LC_SKIP_IF:
      case LC_ELSE_IF:
         newIndent = pPreviousLine->iIndent + 1;
         break;

      // [END/COMMENT] Equal
      case LC_END:
      case LC_COMMENT_NOP:
         newIndent = pPreviousLine->iIndent;
         break;

      // [COMMAND] Equal unless previous is indented due to SKIP-IF, then Minus 1
      case LC_COMMAND:
         if (!isLineIndentedDueToSkipIf(pLineIterator->pPrev, pSkipIfLine))
            newIndent = pPreviousLine->iIndent;
         else
            newIndent = pPreviousLine->iIndent - 1;
         break;
      }
      break;
   }

   // Return indentation for input line
   return newIndent;
}


