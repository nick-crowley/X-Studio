//
// Script Generation (Jumps).cpp  :  Functions for creating 'Jump' commands and determining their destination to create
//                                     the illusion of branching logic.
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       MACROS
/// /////////////////////////////////////////////////////////////////////////////////////////

// OnException: Print ScriptFile
#define  ON_EXCEPTION()     debugScriptFile(pScriptFile);

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                 CONSTRUCTION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createJumpStackItem
// Description     : Creates a new JumpStack item
// 
// COMMAND*  pCommand     : [in]            Command containing the branching command
// COMMAND*  pJumpCommand : [in] [optional] Jump command associated with the branching command
// 
// Return Value   : New JumpStackItem, you are responsible for destroying it
// 
JUMP_STACK_ITEM*  createJumpStackItem(COMMAND*  pCommand, COMMAND*  pJumpCommand)
{
   JUMP_STACK_ITEM*  pItem;

   // Create new item
   pItem = utilCreateEmptyObject(JUMP_STACK_ITEM);

   // Set properties
   pItem->eConditional = pCommand->eConditional;
   pItem->pCommand     = pCommand;
   pItem->pJumpCommand = pJumpCommand;

   // [COMMAND KEYWORDS] Manually set the conditional for command keywords
   switch (pCommand->iID)
   {
   // Branching Commands
   case CMD_BREAK:         pItem->eConditional = CI_BREAK;      break;
   case CMD_CONTINUE:      pItem->eConditional = CI_CONTINUE;   break;
   case CMD_ELSE:          pItem->eConditional = CI_ELSE;       break;
   case CMD_END:           pItem->eConditional = CI_END;        break;
   // Subroutine Commands
   case CMD_DEFINE_LABEL:  pItem->eConditional = CI_LABEL;      break;
   case CMD_GOTO_LABEL:    pItem->eConditional = CI_GOTO_LABEL; break;
   case CMD_GOTO_SUB:      pItem->eConditional = CI_GOTO_SUB;   break;
   case CMD_END_SUB:       pItem->eConditional = CI_END_SUB;    break;
   }

   // Return new item
   return pItem;
}


/// Function name  : deleteJumpStackItem
// Description     : Deletes a JumpStackItem
// 
// JUMP_STACK_ITEM*  pItem   : [in] JumpStackItem to destroy
// 
VOID     deleteJumpStackItem(JUMP_STACK_ITEM*  pItem)
{
   // Delete calling object
   utilDeleteObject(pItem);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                          HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : destroyJumpStackItemByIndex
// Description     : Destroy a specified item in a jump stack
// 
// STACK*      pStack : [in] JumpStack to modify
// CONST UINT  iIndex : [in] Index of the item to remove
// 
VOID   destroyJumpStackItemByIndex(STACK*  pStack, CONST UINT  iIndex)
{
   STACK_ITEM*  pStackItem;

   // Lookup and remove item
   if (removeItemFromListByIndex(pStack, iIndex, pStackItem))
      // [FOUND] Delete item
      deleteStackItem(pStack, pStackItem, TRUE);
}


/// Function name  : isIfConditional
// Description     : Determines whether a conditional is 'IF' or not
// 
// CONST CONDITIONAL_ID   eConditional : [in] Conditional to test
// 
// Return Value   : TRUE or FALSE
// 
BOOL  isIfConditional(CONST CONDITIONAL_ID   eConditional)
{
   return (eConditional == CI_IF OR eConditional == CI_IF_NOT);
}



/// Function name  : isSkipIfConditional
// Description     : Determines whether a conditional is 'SKIP-IF' or not
// 
// CONST CONDITIONAL_ID   eConditional : [in] Conditional to test
// 
// Return Value   : TRUE or FALSE
// 
BOOL  isSkipIfConditional(CONST CONDITIONAL_ID   eConditional)
{
   return (eConditional == CI_SKIP_IF OR eConditional == CI_SKIP_IF_NOT);
}


/// Function name  : isWhileConditional
// Description     : Determines whether a conditional within a JumpStackItem is a 'WHILE'
// 
// CONST JUMP_STACK_ITEM*  pItem: [in] JumpStack item containing the conditional to test
// 
// Return Value   : TRUE or FALSE
// 
BOOL  isWhileConditional(CONST JUMP_STACK_ITEM*   pItem)
{
   return (pItem->eConditional == CI_WHILE OR pItem->eConditional == CI_WHILE_NOT);
}


/// Function name  : popJumpStack
// Description     : Pops and destroys the top stack item
// 
// STACK*    pStack : [in] JumpStack
// 
VOID   popJumpStack(STACK*  pStack)
{
   JUMP_STACK_ITEM*  pStackItem;

   // [CHECK] Pop stack
   if (pStackItem = (JUMP_STACK_ITEM*)popObjectFromStack(pStack))
      // [FOUND] Destroy item
      deleteJumpStackItem(pStackItem);
}


/// Function name  : popJumpStack
// Description     : Pops and destroys the top stack item
// 
// STACK*   pStack       : [in] JumpStack
// COMMAND* pCommand     : [in] Target command
// COMMAND* pJumpCommand : [in] Associated hidden Jump command
// 
VOID   pushJumpStack(STACK*  pStack, COMMAND*  pCommand, COMMAND*  pJumpCommand)
{
   JUMP_STACK_ITEM*  pStackItem;

   // Create new item
   pStackItem = createJumpStackItem(pCommand, pJumpCommand);

   // Add to stack
   pushObjectOntoStack(pStack, (LPARAM)pStackItem);
}


/// Function name  : topJumpStack
// Description     : Retrieves the top jumpstack item
// 
// STACK*   pStack : [in] JumpStack 
// 
JUMP_STACK_ITEM*   topJumpStack(CONST STACK*  pStack)
{
   // Return top item, if any
   return (JUMP_STACK_ITEM*)topStackObject(pStack);
}


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                          FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : appendEndCommandToScriptFile
// Description     : Remove conditional commands from the JumpStack, setting their branching 
//                      information, until the parent 'if' or 'while' is found.
// 
// HWND           hCodeEdit   : [in]  Window handle of the CodeEdit containing the command
// SCRIPT_FILE*   pScriptFile : [in]  ScriptFile containing the commands
// STACK*         pJumpStack  : [in]  JumpStack containing the parent conditional and any sub-conditionals or loop commands
// COMMAND*       pEndCommand : [in]  COMMAND containing 'END'
// ERROR_STACK*  &pError      : [out] New error if any, otherwise NULL
// 
// Return Value   : TRUE if successful, FALSE if there was an error
// 
BOOL  appendEndCommandToScriptFile(HWND  hCodeEdit, SCRIPT_FILE*  pScriptFile, STACK*  pJumpStack, COMMAND*  pEndCommand, ERROR_STACK*  &pError)
{
   JUMP_STACK_ITEM      *pMatchingConditional, // JumpStack item representing the last if/while conditional 
                        *pLastWhile,           // JumpStack item representing the last while conditional (if any)
                        *pCurrentItem;         // JumpStack item currently being processed
   COMMAND              *pEndElseIfMarker;     // The COMMAND containing the last 'end' or 'else-if' command
   BOOL                  bSearching;           // Processing flag

   // [CHECK] Ensure command is 'END'
   ASSERT(isCommandID(pEndCommand, CMD_END));

   // Prepare
   pEndElseIfMarker   = pEndCommand;
   bSearching         = TRUE;
   pError             = NULL;

   /// Locate that last IF or WHILE conditional
   if (!findLastConditionalInJumpStack(pJumpStack, CT_IF WITH CT_WHILE, pMatchingConditional))
   {
      // [ERROR] "Cannot use 'end' on line %u without a matching 'if' or 'while' conditional"
      pushCommandAndCodeEditErrorQueue(pEndCommand, hCodeEdit, pError = generateDualError(HERE(IDS_GENERATION_UNEXPECTED_END), pEndCommand->iLineNumber + 1));
      // Delete COMMAND without deleting error
      popErrorQueue(pEndCommand->pErrorQueue);
      deleteCommand(pEndCommand);
   }
   else 
   {
      /// [WHILE] Add hidden jump before 'END' that targets 'WHILE'
      if (isWhileConditional(pMatchingConditional))
         appendCommandToGenerator(pScriptFile->pGenerator, CL_OUTPUT, createHiddenJumpCommand(pMatchingConditional->pCommand->iIndex));     // Tell 'JUMP' to target 'WHILE'
      
      /// [END] Add command to output
      appendCommandToGenerator(pScriptFile->pGenerator, CL_OUTPUT, pEndCommand);  
      
      // [IF]    Set the jump destination of all commands (except BREAK) between 'IF' and 'END'    (BREAK's target cannot be determined until we handle the 'END' that matches the 'WHILE')
      // [WHILE] Set the jump destination of all commands between 'WHILE' and 'END'      
      for (UINT  iIndex = getStackItemCount(pJumpStack) - 1; !pError AND bSearching AND findListObjectByIndex(pJumpStack, iIndex, (LPARAM&)pCurrentItem); iIndex--) 
      {
         // Set the jump target for each command
         switch (pCurrentItem->eConditional)
         {
         /// [IF/WHILE] Target END/ELSE-IF and stop searching
         case CI_IF:
         case CI_IF_NOT:
         case CI_WHILE:
         case CI_WHILE_NOT:
            setCommandJumpDestination(pCurrentItem->pCommand, pEndElseIfMarker);
            bSearching = FALSE;
            break;

         /// [BREAK] Target END - Unless we're inside a nested 'IF'
         case CI_BREAK:
            // [CHECK] Ensure 'WHILE' is somewhere on the stack
            if (!findLastConditionalInJumpStack(pJumpStack, CT_WHILE, pLastWhile))
            {
               // [ERROR] "Cannot use 'break' on line %u outside of a while loop"
               pushCommandAndCodeEditErrorQueue(pCurrentItem->pCommand, hCodeEdit, pError = generateDualError(HERE(IDS_GENERATION_UNEXPECTED_BREAK), pCurrentItem->pCommand->iLineNumber + 1));
               break;
            }

            // [IF CONDITIONAL] We are inside a nested 'IF' conditional -- do not update the 'BREAK' jump target
            if (!isWhileConditional(pMatchingConditional))
               continue;   // Prevents this item being removed from the stack
            
            // Target jump -> 'END'
            setCommandJumpDestination(pCurrentItem->pJumpCommand, pEndCommand);
            break;

         /// [CONTINUE] Target WHILE
         case CI_CONTINUE: 
            // [CHECK] Ensure 'WHILE' is somewhere on the stack
            if (!findLastConditionalInJumpStack(pJumpStack, CT_WHILE, pLastWhile))
            {
               // [ERROR] "Cannot use 'continue' on line %u outside of a while loop"
               pushCommandAndCodeEditErrorQueue(pCurrentItem->pCommand, hCodeEdit, pError = generateDualError(HERE(IDS_GENERATION_UNEXPECTED_CONTINUE), pCurrentItem->pCommand->iLineNumber + 1));
               break;
            }
            
            // Target jump -> WHILE
            setCommandJumpDestination(pCurrentItem->pJumpCommand, pLastWhile->pCommand);
            break;

         /// [ELSE] Target END/ELSE-IF and replace marker
         case CI_ELSE:
            // [CHECK] Ensure conditional isn't 'WHILE'
            if (isWhileConditional(pMatchingConditional))
            {
               // [ERROR] "Cannot use 'else' on line %u outside of an 'if' conditional"
               pushCommandAndCodeEditErrorQueue(pCurrentItem->pCommand, hCodeEdit, pError = generateDualError(HERE(IDS_GENERATION_UNEXPECTED_ELSE), pCurrentItem->pCommand->iLineNumber + 1));
               break;
            }

            // Target Jump -> END/ELSE-IF
            setCommandJumpDestination(pCurrentItem->pJumpCommand, pEndElseIfMarker);
            pEndElseIfMarker = pCurrentItem->pCommand;      // Update ELSE-IF marker
            break;

         /// [ELSE-IF] Target END/ELSE-IF and replace marker
         case CI_ELSE_IF:
         case CI_ELSE_IF_NOT:
            // [CHECK] Ensure conditional isn't 'WHILE'
            if (isWhileConditional(pMatchingConditional))
            {
               // [ERROR] "Cannot use 'else-if' on line %u outside of an 'if' conditional"
               pushCommandAndCodeEditErrorQueue(pCurrentItem->pCommand, hCodeEdit, pError = generateDualError(HERE(IDS_GENERATION_UNEXPECTED_ELSE_IF), pCurrentItem->pCommand->iLineNumber + 1));
               break;
            }

            // Target Command -> END/ELSE-IF
            setCommandJumpDestination(pCurrentItem->pCommand, pEndElseIfMarker);
            // Target Jump Command -> END
            setCommandJumpDestination(pCurrentItem->pJumpCommand, pEndCommand);      /// [VALIDATION_FIX]
            // Update ELSE-IF marker
            pEndElseIfMarker = pCurrentItem->pCommand;      
            break;

         } // END: switch 'current item conditional'

         // Remove current item from the stack and delete it
         destroyJumpStackItemByIndex(pJumpStack, iIndex);

      } // END: for each 'jump stack item'

   } // END: if 'stack contains IF or WHILE'

   // Cleanup and return TRUE if there was no error
   return (pError == NULL);
}


/// Function name  : findLastConditionalInJumpStack
// Description     : Search a JumpStack for an 'IF' and/or 'WHILE' item 
// 
// STACK*        pStack  : [in]  JumpStack to search
// CONST UINT         eType   : [in]  Combination of CONDITION_TYPE flags indicating which type of conditional to search for
// JUMP_STACK_ITEM*  &pOutput : [out] JumpStackItem if found, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BOOL    findLastConditionalInJumpStack(STACK*  pStack, CONST UINT  eType, JUMP_STACK_ITEM*  &pOutput)
{
   JUMP_STACK_ITEM*   pItem;     // Item currently being examined

   // Prepare
   pOutput = NULL;

   /// Search backwards through stack
   for (LIST_ITEM*  pIterator = getListTail(pStack); !pOutput AND (pItem = extractListItemPointer(pIterator, JUMP_STACK_ITEM)); pIterator = pIterator->pPrev)
   {
      // Check conditional for a match
      switch (pItem->eConditional)
      {
      /// [IF] Return item if we're searching for 'IF'
      case CI_IF:
      case CI_IF_NOT:
         if (eType INCLUDES CT_IF)
            pOutput = pItem;
         break;

      /// [WHILE] Return item if we're searching for 'WHILE'
      case CI_WHILE:
      case CI_WHILE_NOT:
         if (eType INCLUDES CT_WHILE)
            pOutput = pItem;
         break;
      }
   }

   // Cleanup and return TRUE if item was found
   return (pOutput != NULL);
}


/// Function name  : findLabelInJumpStack
// Description     : Search a JumpStack for a 'define label' command
// 
// STACK*        pStack        : [in]  JumpStack to search
// JUMP_STACK_ITEM*  &pOutput       : [out] JumpStackItem if found, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BOOL    findLabelInJumpStack(STACK*  pStack, JUMP_STACK_ITEM*  &pOutput)
{
   JUMP_STACK_ITEM*   pItem;     // Item currently being examined

   // Prepare
   pOutput = NULL;

   /// Search backwards through stack
   for (LIST_ITEM*  pIterator = getListTail(pStack); !pOutput AND (pItem = extractListItemPointer(pIterator, JUMP_STACK_ITEM)); pIterator = pIterator->pPrev)
   {
      // Check conditional for a match
      if (pItem->eConditional == CI_LABEL)
         // [FOUND] Set result and abort
         pOutput = pItem;
   }

   // Cleanup and return TRUE if item was found
   return (pOutput != NULL);
}


/// Function name  : findLabelInJumpStackByName
// Description     : Finds the item in a JumpStack of 'define label' commands with the specified label name
// 
// STACK*        pLabelStack  : [in]  JumpStack containing the 'define label' commands
// CONST TCHAR*       szLabelName  : [in]  Name of the label to search for
// JUMP_STACK_ITEM*  &pOutput      : [out] JumpStack item containing the matching 'define label' command if found, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BOOL  findLabelInJumpStackByName(STACK*  pLabelStack, CONST TCHAR*  szLabelName, JUMP_STACK_ITEM*  &pOutput)
{
   PARAMETER*         pLabelParameter;      // Label name Parameter of the COMMAND currently being processed
   JUMP_STACK_ITEM*   pItem;                // Item currently being examined

   // Prepare
   pOutput = NULL;

   /// Search forwards through stack
   for (LIST_ITEM*  pIterator = getListHead(pLabelStack); !pOutput AND (pItem = extractListItemPointer(pIterator, JUMP_STACK_ITEM)); pIterator = pIterator->pNext)
   {
      // Extract PARAMETER containing the label name
      findParameterInCommandByIndex(pItem->pCommand, PT_DEFAULT, 0, pLabelParameter);
      ASSERT(pLabelParameter);

      // [CHECK] Determine whether label names match
      if (utilCompareStringVariables(pLabelParameter->szValue, szLabelName))
         // [FOUND] Set result and abort
         pOutput = pItem;
   }

   // Cleanup and return TRUE if item was found
   return (pOutput != NULL);
}


/// Function name  : findNextJumpStackItem
// Description     : Searches forwards through a JumpStack for the next item
// 
// JUMP_STACK_ITERATOR*  pIterator  : [in/out] JumpStack iterator
// JUMP_STACK_ITEM*     &pOutput    : [out]    Item if found, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BOOL  findNextJumpStackItem(STACK*  pStack, STACK_ITEM*  &pIterator, JUMP_STACK_ITEM*  &pOutput)
{
   // [CHECK] Does iterator exist?
   if (!pIterator)
      // [FIRST CALL] Return first item
      pIterator = getListHead(pStack);
   else
      // [SUBSEQUENT CALL] Return next item (if any)
      pIterator = pIterator->pNext;

   // Set result
   pOutput = extractListItemPointer(pIterator, JUMP_STACK_ITEM);

   // Return TRUE if found
   return (pOutput != NULL);
}


/// Function name  : findPrevJumpStackItem
// Description     : Searches backwards through a JumpStack for the previous item
// 
// JUMP_STACK_ITERATOR*  pIterator  : [in/out] JumpStack iterator
// JUMP_STACK_ITEM*     &pOutput    : [out]    Item if found, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BOOL  findPrevJumpStackItem(STACK*  pStack, STACK_ITEM*  &pIterator, JUMP_STACK_ITEM*  &pOutput)
{
   // [CHECK] Does iterator exist?
   if (!pIterator)
      // [FIRST CALL] Return last item
      pIterator = getListTail(pStack);
   else
      // [SUBSEQUENT CALL] Return previous item (if any)
      pIterator = pIterator->pPrev;

   // Set result
   pOutput = extractListItemPointer(pIterator, JUMP_STACK_ITEM);

   // Return TRUE if found
   return (pOutput != NULL);
}


/// Function name  : generateBranchingCommandLogic
// Description     : Generates the branching information for a list of successfully parsed COMMANDs, 
//                      then splits them into STANDARD and AUXILIARY lists
// 
// HWND                 hCodeEdit   : [in]     Window handle of the CodeEdit containing the commands
// SCRIPT_FILE*         pScriptFile : [in/out] Contains the COMMANDS to be translated on input and the generated commands on output
// ERROR_QUEUE*         pErrorQueue : [in/out] Empty error queue
// 
// Return Value   : TRUE if successful, FALSE If there were errors
// 
BOOL   generateBranchingCommandLogic(HWND  hCodeEdit, SCRIPT_FILE*  pScriptFile, ERROR_QUEUE*  pErrorQueue)
{
   COMMAND          *pSourceCommand,   // Read-Only 'source' copy of the COMMAND currently being processed
                    *pCommand,         // COMMAND currently being processed
                    *pJumpCommand;     // Used for creating 'hidden jump' COMMANDs
   ERROR_STACK      *pError;           // Operation error, if any
   STACK            *pJumpStack = NULL;// For storing branching commands
   JUMP_STACK_ITEM  *pJumpItem;

   __try
   {
      // [INFO] "Compiling commands in '%s' for %s"     
      VERBOSE("Generating branching logic for '%s'", identifyScriptName(pScriptFile));
      pushErrorQueue(pErrorQueue, generateDualInformation(HERE(IDS_OUTPUT_GENERATING_BRANCHING_LOGIC), identifyScriptName(pScriptFile), identifyGameVersionString(pScriptFile->eGameVersion)));

      // Prepare
      pError     = NULL;
      pJumpStack = createJumpStack();

      /// Iterate through GENERATOR INPUT list
      for (UINT iIndex = 0; findCommandInGeneratorInput(pScriptFile->pGenerator, iIndex, pSourceCommand); iIndex++)
      {
         // Duplicate command for insertion into other command lists
         pCommand = duplicateCommand(pSourceCommand);

         //VERBOSE("BRANCHING: Index %d : Command 0x%08x : '%s'", iIndex, pCommand, pCommand->szBuffer);

         // Examine command
         switch (pCommand->iID)
         {
         // [COMMAND] Examine conditional to determine whether a hidden jump is necessary
         default:
            switch (pCommand->eConditional)
            {
            /// [NO CONDITIONAL/START/LABEL/SUBROUTINE] Add COMMAND -> OUTPUT
            case CI_NONE:
            case CI_LABEL:
            case CI_START:
            case CI_GOTO_LABEL:
            case CI_GOTO_SUB:
            case CI_END_SUB:
               appendCommandToGenerator(pScriptFile->pGenerator, CL_OUTPUT, pCommand);
               break;

            /// [VARIABLE/IF/WHILE] Add COMMAND -> OUTPUT + STACK
            case CI_IF:
            case CI_IF_NOT:
            case CI_WHILE:
            case CI_WHILE_NOT:
               appendCommandToGenerator(pScriptFile->pGenerator, CL_OUTPUT, pCommand);
               pushJumpStack(pJumpStack, pCommand, NULL);
               break;

            /// [ELSE-IF] Add COMMAND + JUMP -> OUTPUT and STACK
            case CI_ELSE_IF:
            case CI_ELSE_IF_NOT:
               // Add hidden jump and command to output
               appendCommandToGenerator(pScriptFile->pGenerator, CL_OUTPUT, pJumpCommand = createHiddenJumpCommand(EMPTY_JUMP));
               appendCommandToGenerator(pScriptFile->pGenerator, CL_OUTPUT, pCommand);
               // Add both to JumpStack
               pushJumpStack(pJumpStack, pCommand, pJumpCommand);
               break;

            /// [SKIP-IF] Add COMMAND -> OUTPUT
            case CI_SKIP_IF:
            case CI_SKIP_IF_NOT:
               // Add command to output
               appendCommandToGenerator(pScriptFile->pGenerator, CL_OUTPUT, pCommand);
               // Target the next line manually
               setCommandJumpDestinationByIndex(pCommand, pCommand->iIndex + 2);    /// [VALIDATION_FIX]
               break;

            /// [????]
            default:
               // [ERROR]
               ASSERT(FALSE);
            }
            break;

         /// [ELSE / BREAK / CONTINUE] Add COMMAND + JUMP -> OUTPUT and STACK
         case CMD_ELSE:
         case CMD_BREAK:
         case CMD_CONTINUE:
            // Add hidden jump and command to output
            appendCommandToGenerator(pScriptFile->pGenerator, CL_OUTPUT, pJumpCommand = createHiddenJumpCommand(EMPTY_JUMP));
            appendCommandToGenerator(pScriptFile->pGenerator, CL_OUTPUT, pCommand);
            // Add to JumpStack
            pushJumpStack(pJumpStack, pCommand, pJumpCommand);
            break;

         /// [END] Examine the JumpStack for a matching conditional, and calculate their jump destinations
         case CMD_END:
            if (!appendEndCommandToScriptFile(hCodeEdit, pScriptFile, pJumpStack, pCommand, pError))
            {
               // [ERROR] Invalid placement of a branching command.  No enhancement necessary. 
               generateOutputTextFromError(pError);
               pushErrorQueue(pErrorQueue, duplicateErrorStack(pError));      // Add to output error queue manually - appendEndCommandToScriptFile distributes it to the correct COMMAND and informs the CodeEdit.
            }
            break;
         }
      }

      /// [CHECK] Any conditionals left in the stack indicate a missing 'END' command
      while (pJumpItem = topJumpStack(pJumpStack))
      {
         // [ERROR] "Missing 'end' command for the conditional command on line %u"
         pError = generateDualError(HERE(IDS_GENERATION_UNCLOSED_CONDITIONAL), pJumpItem->pCommand->iLineNumber);
         // Add to output queues and CodeEdit
         pushCommandAndOutputQueues(pError, pErrorQueue, pJumpItem->pCommand->pErrorQueue, ET_ERROR);
         setCodeEditLineError(hCodeEdit, pJumpItem->pCommand);

         // Destroy item
         popJumpStack(pJumpStack);
      }

      /// [CHECK] Ensure at least one command was generated 
      if (!findCommandInGeneratorOutput(pScriptFile->pGenerator, CT_STANDARD, 0, pCommand))
         // [ERROR] "Cannot compile a script with zero standard commands"
         pushErrorQueue(pErrorQueue, pError = generateDualError(HERE(IDS_GENERATION_NO_COMMANDS_FOUND)) );

      /// [CHECK] Ensure last command is RETURN or ENDSUB
      else if (!findLastCommandInGeneratorOutput(pScriptFile->pGenerator, pCommand) OR !(isCommandID(pCommand, CMD_RETURN) OR isCommandID(pCommand, CMD_END_SUB)) )
         // [ERROR] "The last command in any script must always be 'return' or 'end'"
         pushErrorQueue(pErrorQueue, pError = generateDualError(HERE(IDS_GENERATION_FINAL_RETURN_MISSING)) );

      // [DEBUG]
      /*VERBOSE("------------------------------");
      for (UINT  iIndex = 0; findCommandInGeneratorOutput(pScriptFile->pGenerator, CT_STANDARD, iIndex, pCommand); iIndex++)
         VERBOSE("VALIDATION: STANDARD (Index %d) : Command 0x%08x : '%s'", iIndex, pCommand, pCommand->szBuffer);

      VERBOSE("------------------------------");
      for (UINT  iIndex = 0; findCommandInGeneratorOutput(pScriptFile->pGenerator, CT_AUXILIARY, iIndex, pCommand); iIndex++)
         VERBOSE("VALIDATION: AUXILIARY (Index %d) : Command 0x%08x : '%s'", iIndex, pCommand, pCommand->szBuffer);*/

      // Cleanup and return TRUE if there were no errors
      deleteJumpStack(pJumpStack);
      return (pError == NULL);
   }
   PUSH_CATCH0(pErrorQueue, "");
   debugCommand(pCommand);
   debugJumpStack(pJumpStack);
   return FALSE;
}




/// Function name  : generateSubroutineCommandLogic
// Description     : Generates the jump information in label and sub-routine commands. Also verifies 'endsub' commands are legitimately placed
// 
// HWND                 hCodeEdit   : [in]     Window handle of the CodeEdit containing the commands
// SCRIPT_FILE*         pScriptFile : [in/out] Contains the COMMANDS to generate subroutine logic for
// ERROR_QUEUE*         pErrorQueue : [in/out] Empty error queue
// 
// Return Value   : TRUE if successful, FALSE If there were errors
// 
BOOL  validateSubroutineLogic(HWND  hCodeEdit, SCRIPT_FILE*  pScriptFile, ERROR_QUEUE*  pErrorQueue)
{
   JUMP_STACK_ITEM  *pItem;             // Stack item iterator
   STACK            *pSubroutineStack;  // Stores current 'LABEL' commands for sub-routine syntax verification
   COMMAND          *pCommand = NULL;   // 'Standard generated' Command list iterator
   ERROR_STACK      *pError = NULL;     // Operation error, if any
   
   __try
   {
      // [DEBUG] 
      VERBOSE("Validating subroutine branching logic for '%s'", identifyScriptName(pScriptFile));

      // Prepare
      pSubroutineStack = createJumpStack();
      pError           = NULL;

      /// [VERIFY SUBROUTINES] Ensure each 'endsub' command is matched with a preceeding 'define label' command
      // Iterate through generator INPUT list
      for (UINT iLine = 0; findCommandInGeneratorInput(pScriptFile->pGenerator, iLine, pCommand); iLine++)
      {
         // Prepare
         pError = NULL;

         // Examine conditional
         switch (pCommand->eConditional)
         {
         /// [DEFINE LABEL] Add to Stack 
         case CI_LABEL:
            pushJumpStack(pSubroutineStack, pCommand, NULL);
            break;

         /// [IF/SKIP-IF] Add to Stack
         case CI_IF:
         case CI_IF_NOT:
         case CI_SKIP_IF:
         case CI_SKIP_IF_NOT:
            pushJumpStack(pSubroutineStack, pCommand, NULL);
            break;

         /// [COMMAND] Remove 'SKIP-IF', if present
         default:
            if (topJumpStack(pSubroutineStack) AND isSkipIfConditional(topJumpStack(pSubroutineStack)->eConditional))
               popJumpStack(pSubroutineStack);
            break;

         /// [END] Remove last 'IF' from the stack
         case CI_END:
            // Search backwards through the stack
            for (UINT  iIndex = getStackItemCount(pSubroutineStack) - 1; findListObjectByIndex(pSubroutineStack, iIndex, (LPARAM&)pItem); iIndex--) 
            {
               // [CHECK] Search for IF/IF-NOT
               if (isIfConditional(pItem->eConditional))
               {
                  // [FOUND] Remove item and abort search
                  destroyJumpStackItemByIndex(pSubroutineStack, iIndex);
                  break;
               }
            }
            break;

         /// [ENDSUB] Check stack for a matching label
         case CI_END_SUB:
            /// [EMPTY STACK] Error - ENDSUB without a LABEL definition
            if (!topJumpStack(pSubroutineStack))
               // [ERROR] "Cannot use 'endsub' on line %u without a previous subroutine definition"
               pError = generateDualError(HERE(IDS_GENERATION_UNEXPECTED_END_SUB), pCommand->iLineNumber + 1);
            
            // Examine the conditional of the last item in the stack
            else switch (topJumpStack(pSubroutineStack)->eConditional)
            {
            /// [IF] ENDSUB is within an IF conditional - ignore it
            case CI_IF:
            case CI_IF_NOT:
               break;

            /// [SKIP-IF] ENDSUB is within a SKIP-IF conditional - ignore it  (And remove Skip-If)
            case CI_SKIP_IF:
            case CI_SKIP_IF_NOT:
               popJumpStack(pSubroutineStack);
               break;

            /// [LABEL] ENDSUB follows a LABEL, remove label from the stack
            case CI_LABEL:
               popJumpStack(pSubroutineStack);
               break;
            }
            break;
         }

         // [ERROR] Add to error queues and inform CodeEdit
         if (pError)
         {
            pushCommandAndOutputQueues(pError, pErrorQueue, pCommand->pErrorQueue, ET_ERROR);
            setCodeEditLineError(hCodeEdit, pCommand);
         }
      }

      // Cleanup and return TRUE if there were no errors
      deleteJumpStack(pSubroutineStack);
      return (pError == NULL);
   }
   PUSH_CATCH0(pErrorQueue, "");
   debugCommand(pCommand);
   debugJumpStack(pSubroutineStack);
   return FALSE;
}


/// Function name  : generateSubroutineCommandLogic
// Description     : Generates the jump information in goto label/sub-routine commands.
// 
// HWND                 hCodeEdit   : [in]     CodeEdit
// SCRIPT_FILE*         pScriptFile : [in/out] Scriptfile containing generated commands
// ERROR_QUEUE*         pErrorQueue : [in/out] ErrorQueue
// 
// Return Value   : TRUE if successful, FALSE If there were errors
// 
BOOL  generateSubroutineCommandLogic(HWND  hCodeEdit, SCRIPT_FILE*  pScriptFile, ERROR_QUEUE*  pErrorQueue)
{
   STACK        *pLabelStack;       // Stores all 'LABEL' commands for jump destination generation
   COMMAND      *pCommand = NULL;   // 'Standard generated' Command list iterator
   ERROR_STACK  *pError = NULL;     // Operation error, if any
   
   __try
   {
      /// [VALIDATE] Validate label/gosub placement
      if (!validateSubroutineLogic(hCodeEdit, pScriptFile, pErrorQueue))
         return FALSE;

      // Prepare
      pLabelStack = createJumpStack();

      // Populate label stack
      for (UINT iIndex = 0; findCommandInGeneratorOutput(pScriptFile->pGenerator, CT_STANDARD, iIndex, pCommand); iIndex++)
      {
         JUMP_STACK_ITEM  *pConflict;     // Item data for conflicting label 
         CONST TCHAR      *szLabel,       // Name of label being defined
                          *szConflict;    // Name of conflicting label 

         /// [DEFINE LABEL] Ensure unique and add to stack
         if (isCommandID(pCommand, CMD_DEFINE_LABEL))
         {
            // [CHECK] Lookup label name and ensure unique
            if (findStringParameterInCommandByIndex(pCommand, 0, szLabel) AND !findLabelInJumpStackByName(pLabelStack, szLabel, pConflict))
               pushJumpStack(pLabelStack, pCommand, NULL);
            else
            {
               // [ERROR] "The label '%s' on line %u is a duplicate of the label '%s' on line %u"
               findStringParameterInCommandByIndex(pConflict->pCommand, 0, szConflict);
               pError = generateDualError(HERE(IDS_GENERATION_DUPLICATE_LABEL_NAME), szLabel, (pCommand->iLineNumber + 1), szConflict, (pConflict->pCommand->iLineNumber + 1));
               // Add to error queues and inform CodeEdit
               pushCommandAndOutputQueues(pError, pErrorQueue, pCommand->pErrorQueue, ET_ERROR);
               setCodeEditLineError(hCodeEdit, pCommand);
            }
         }
      }
         
      /// [VERIFY LABEL NAMES] Verify labels and convert the their names into line numbers
      for (UINT iIndex = 0; findCommandInGeneratorOutput(pScriptFile->pGenerator, CT_STANDARD, iIndex, pCommand); iIndex++)
      {
         JUMP_STACK_ITEM*  pTarget;       // Item data for target label
         CONST TCHAR*      szTarget;      // Name of target label
         PARAMETER*        pReference;    // Parameter in 'goto' command referencing the target label

         // Examine command
         switch (pCommand->iID)
         {
         /// [GOTO LABEL/GOSUB] Change the label name into a jump destination - if target label exists
         case CMD_GOTO_LABEL:
         case CMD_GOTO_SUB:
            /// Check that the label exists
            if (!findStringParameterInCommandByIndex(pCommand, 0, szTarget) OR !findLabelInJumpStackByName(pLabelStack, szTarget, pTarget))
            {
               // [ERROR] "The label '%s' specified by the command on line %u does not exist"
               pError = generateDualError(HERE(IDS_GENERATION_LABEL_NOT_FOUND), szTarget, pCommand->iLineNumber + 1);
               // Add to error queues and inform CodeEdit
               pushCommandAndOutputQueues(pError, pErrorQueue, pCommand->pErrorQueue, ET_ERROR);
               setCodeEditLineError(hCodeEdit, pCommand);
            }
            /// [SUCCESS] Convert label name into a jump destination
            else
            {
               findParameterInCommandByIndex(pCommand, PT_DEFAULT, 0, pReference);
               pReference->eSyntax = PS_LABEL_NUMBER;
               pReference->eType   = DT_INTEGER;
               pReference->iValue  = pTarget->pCommand->iIndex;
            }
            break;
         }
      }

      // Cleanup and return TRUE if there were no errors
      deleteJumpStack(pLabelStack);
      return (pError == NULL);
   }
   PUSH_CATCH0(pErrorQueue, "");
   debugCommand(pCommand);
   debugJumpStack(pLabelStack);
   return FALSE;
}

