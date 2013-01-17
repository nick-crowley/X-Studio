//
// Commands (Macros).cpp : COMMAND macro functions
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

/// Function name  : isCommandAddCustomMenuInfoHeading
// Description     : Checks for either a 'add custom menu info' or 'add custom menu heading' command
// 
//     INFO: add custom menu info line to array $0: text=$1          HEADING: add custom menu heading to array $0: title=$1
// 
// CONST COMMAND*  pCommand       : [in]  Add custom menu Command
// CONST TCHAR*    szTextVariable : [in]  Name of text variable
// CONST TCHAR*   &szMenuVariable : [out] Name of menu variable
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BOOL  isCommandAddCustomMenuInfoHeading(CONST COMMAND*  pCommand, CONST TCHAR*  szTextVariable, CONST TCHAR*  &szMenuVariable)
{
   // Prepare
   szMenuVariable = NULL;

   // [CHECK] Ensure command and text variable are correct
   if ((isCommandID(pCommand, CMD_ADD_CUSTOM_MENU_INFO) OR isCommandID(pCommand, CMD_ADD_CUSTOM_MENU_HEADING)) AND isVariableParameterInCommandAtIndex(pCommand, 1, szTextVariable))
      // [SUCCESS] Extract Menu Variable 
      findVariableParameterInCommandByIndex(pCommand, 0, szMenuVariable);

   // Return TRUE if found
   return (szMenuVariable != NULL);
}


/// Function name  : isCommandAddCustomMenuItem
// Description     : Checks for an 'add custom menu item to array $menu: text=$text returnvalue='anything' command
// 
// CONST COMMAND*  pCommand       : [in]  Add custom menu Command
// CONST TCHAR*    szTextVariable : [in]  Name of text variable
// CONST TCHAR*   &szMenuVariable : [out] Name of menu variable
// PARAMETER*     &pItemID        : [out] Item ID parameter
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BOOL  isCommandAddCustomMenuItem(CONST COMMAND*  pCommand, CONST TCHAR*  szTextVariable, CONST TCHAR*  &szMenuVariable, PARAMETER*  &pItemID)
{
   // [CHECK] Ensure command has correct text variable. Extract Menu Variable and ReturnID Parameter.
   if (!isCommandID(pCommand, CMD_ADD_CUSTOM_MENU_ITEM) OR !isVariableParameterInCommandAtIndex(pCommand, 1, szTextVariable) OR 
       !findVariableParameterInCommandByIndex(pCommand, 0, szMenuVariable) OR !findParameterInCommandByIndex(pCommand, PT_DEFAULT, 2, pItemID))
   {
      // [FAILED] Ensure outputs are NULL
      szMenuVariable = NULL;
      pItemID        = NULL;
   }

   // Return TRUE if found
   return (pItemID != NULL);
}


/// Function name  : isCommandArrayAccess
// Description     : Checks for an array element access command '$item = $Array[$iterator.1]' and returns the return variable
// 
// CONST COMMAND*  pCommand          : [in]  Command to test
// CONST TCHAR*    szArrayVariable   : [in]  Name of the array variable
// CONST TCHAR*    szElementVariable : [in]  Name of the element variable
// CONST TCHAR*   &szReturnVariable  : [out] Name of the return variable
// 
// Return Value   : TRUE if found, otherwise FALSE
// 
BOOL  isCommandArrayAccess(CONST COMMAND*  pCommand, CONST TCHAR*  szArrayVariable, CONST TCHAR*  szElementVariable, CONST TCHAR*  &szReturnVariable)
{
   // Prepare
   szReturnVariable = NULL;

   /// [CHECK] Ensure array and element variables match
   if (isCommandID(pCommand, CMD_ARRAY_ACCESS) AND isVariableParameterInCommandAtIndex(pCommand, 1, szArrayVariable) AND isVariableParameterInCommandAtIndex(pCommand, 2, szElementVariable))
      // [SUCCESS] Extract name of return variable
      findReturnVariableParameterInCommand(pCommand, szReturnVariable);

   // Return TRUE if found
   return (szReturnVariable != NULL);
}


/// Function name  : isCommandArrayAssignment
// Description     : Checks for an array allocation command '$Array = alloc array: size=9' and returns the array variable and array size
// 
// CONST COMMAND*  pCommand        : [in]  Command to test
// CONST TCHAR*   &szArrayVariable : [out] Name of the array variable
// CONST UINT     &iArraySize      : [out] Array size
// 
// Return Value   : TRUE if found, otherwise FALSE
// 
BOOL  isCommandArrayAllocation(CONST COMMAND*  pCommand, CONST TCHAR*  &szArrayVariable, INT&  iArraySize)
{
   // Prepare
   szArrayVariable = NULL;
   iArraySize      = 0;

   /// [CHECK] Extract array variable and element count
   return isCommandID(pCommand, CMD_ARRAY_ALLOC) AND findReturnVariableParameterInCommand(pCommand, szArrayVariable) AND findIntegerParameterInCommandByIndex(pCommand, 1, iArraySize);
}


/// Function name  : isCommandArrayAssignment
// Description     : Checks for an array assignment command '$Array[3] = $assignment' and returns the assignment parameter
// 
// CONST COMMAND*  pCommand        : [in]  Command to test
// CONST TCHAR*    szArrayVariable : [in]  Name of the array variable
// CONST UINT      iElement        : [in]  Zero-based element index
// PARAMETER*     &pAssignment     : [out] Assignment parameter if found, otherwise NULL
// 
// Return Value   : TRUE if found, otherwise FALSE
// 
BOOL  isCommandArrayAssignment(CONST COMMAND*  pCommand, CONST TCHAR*  szArrayVariable, CONST UINT  iElement, PARAMETER*  &pAssignment)
{
   INT  iParameterElement;       // Actual element index

   // Prepare
   pAssignment = NULL;

   /// [CHECK] Ensure command has correct array variable and element index
   if (isCommandID(pCommand, CMD_ARRAY_ASSIGNMENT) AND isVariableParameterInCommandAtIndex(pCommand, 0, szArrayVariable) AND findIntegerParameterInCommandByIndex(pCommand, 1, iParameterElement) AND iParameterElement == iElement)
      // [SUCCESS] Extract assignment parameter
      findParameterInCommandByIndex(pCommand, PT_DEFAULT, 2, pAssignment);

   // Return TRUE if found
   return (pAssignment != NULL);
}


/// Function name  : isCommandArrayCount
// Description     : Checks for an array size command '$count = size of array $array' and returns the count and array variables
// 
// CONST COMMAND*  pCommand        : [in]  Command to test
// CONST TCHAR*   &szCountVariable : [out] Name of the element count variable
// CONST TCHAR*   &szArrayVariable : [out] Name of the array variable
// 
// Return Value   : TRUE if found, otherwise FALSE
// 
BOOL  isCommandArrayCount(CONST COMMAND*  pCommand, CONST TCHAR*  &szCountVariable, CONST TCHAR*  &szArrayVariable)
{
   // Prepare
   szCountVariable = szArrayVariable = NULL;

   /// [CHECK] Extract array variable and element count variable
   return isCommandID(pCommand, CMD_SIZE_OF_ARRAY) AND findReturnVariableParameterInCommand(pCommand, szCountVariable) AND findVariableParameterInCommandByIndex(pCommand, 1, szArrayVariable);
}


/// Function name  : isCommandGotoLabel
// Description     : Checks whether a command is 'goto label' or 'gosub' and returns the label anme
// 
// CONST COMMAND*  pCommand    : [in]  Command to test
// CONST TCHAR*   &szLabelName : [out] Label name
// 
// Return Value   : TRUE if found, otherwise FALSE
// 
BearScriptAPI
BOOL  isCommandGotoLabel(CONST COMMAND*  pCommand, CONST TCHAR*  &szLabelName)
{
   // Prepare
   szLabelName = NULL;

   /// [CHECK] Ensure command is correct and extract label name
   return (isCommandID(pCommand, CMD_GOTO_LABEL) OR isCommandID(pCommand, CMD_GOTO_SUB)) AND findStringParameterInCommandByIndex(pCommand, 0, szLabelName);
}


/// Function name  : isCommandReadText
// Description     : Checks for a read text command '$text = read text: page=$Page.ID id=216' and returns the ReturnVariable, stringID param and pageID param
// 
// CONST COMMAND*  pCommand         : [in]  ReadText Command 
// CONST TCHAR*   &szReturnVariable : [out] Name of the return variable
// PARAMETER*     &pStringID        : [out] String ID parameter
// PARAMETER*     &pPageID          : [out] Page ID parameter
// 
// Return Value   : TRUE if found, otherwise FALSE
// 
BOOL  isCommandReadText(CONST COMMAND*  pCommand, CONST TCHAR*  &szReturnVariable, PARAMETER*  &pPageID, PARAMETER*  &pStringID)
{
   /// [CHECK] Extract array variable and element count variable
   if (!isCommandID(pCommand, CMD_READ_TEXT) OR !findReturnVariableParameterInCommand(pCommand, szReturnVariable) OR
       !findParameterInCommandByIndex(pCommand, PT_DEFAULT, 1, pPageID) OR !findParameterInCommandByIndex(pCommand, PT_DEFAULT, 2, pStringID))
   {
      // [FAILED] Set all outputs to NULL
      pStringID = pPageID = NULL;
      szReturnVariable    = NULL;
   }

   // Return TRUE if found
   return (pPageID != NULL);
}


/// Function name  : isExpressionSimpleLoopConditional
// Description     : Check whether a command is an expression in the format 'while $iterator'
// 
// CONST COMMAND*  pCommand       : [in] Command 
// CONST TCHAR*    szIteratorName : [in] Assignment/conditional variable
// 
// Return Value   : TRUE or FALSE
// 
BOOL  isExpressionSimpleLoopConditional(CONST COMMAND*  pCommand, CONST TCHAR*  szIteratorName)
{
   // [CHECK] Ensure command is an expression with the desired conditional and assignment variable
   return isCommandID(pCommand, CMD_EXPRESSION) AND pCommand->eConditional == CI_WHILE AND getCommandParameterCount(pCommand, PT_DEFAULT) == 2 AND isVariableParameterInCommandAtIndex(pCommand, 1, szIteratorName);
}


/// Function name  : isExpressionRangedLoopConditional
// Description     : Check whether a command is an expression in the format 'while $iterator < count' or 'while $iterator > count'
// 
// CONST COMMAND*       pCommand       : [in]  Expression Command 
// CONST TCHAR*         szIteratorName : [in]  Name of Iterator variable
// CONST OPERATOR_TYPE  eOperator      : [in]  Whether to search for less-than or greater-than
// PARAMETER*          &pInitialValue  : [out] Final loop value
// 
// Return Value   : TRUE or FALSE
// 
BOOL  isExpressionRangedLoopConditional(CONST COMMAND*  pCommand, CONST TCHAR*  szIteratorName, CONST OPERATOR_TYPE  eOperator, PARAMETER*  &pFinalValue)
{
   // Prepare
   pFinalValue = NULL;

   // [CHECK] Ensure expression has three parameters, a 'while' conditional and a 'less-than/greater-than' operator:  'while $iterator <|> count'
   if (isCommandID(pCommand, CMD_EXPRESSION) AND getCommandParameterCount(pCommand, PT_DEFAULT) == 4 AND pCommand->eConditional == CI_WHILE AND isOperatorParameterInCommandAtIndex(pCommand, 2, eOperator))
      // [CHECK] Ensure iterator is correct, and 'count' is integer/variable
      if (isVariableParameterInCommandAtIndex(pCommand, 1, szIteratorName) AND (isParameterTypeInCommandAtIndex(pCommand, 3, DT_INTEGER) OR isParameterTypeInCommandAtIndex(pCommand, 3, DT_VARIABLE)))
         /// [SUCCESS] Extract 'count' parameter
         findParameterInCommandByIndex(pCommand, PT_DEFAULT, 3, pFinalValue);
   
   // Return TRUE if found
   return pFinalValue != NULL;
}

/// Function name  : isExpressionRangedLoopConditional
// Description     : Check whether a command is an expression in the format '$iterator = $iterator ± step'
// 
// CONST COMMAND*  pCommand       : [in] Expression Command 
// CONST TCHAR*    szIteratorName : [in] Name of Iterator variable
// CONST UINT      iLoopStep      : [in] Loop step
// 
// Return Value   : TRUE or FALSE
// 
BOOL  isExpressionLoopCounter(CONST COMMAND*  pCommand, CONST TCHAR*  szIteratorName, CONST INT  iLoopStep)
{
   INT   iStepParameter;      // Value of 'step' parameter
   BOOL  bResult;

   // [CHECK] Ensure expression has four components and the correct operator
   if (bResult = isCommandID(pCommand, CMD_EXPRESSION) AND getCommandParameterCount(pCommand, PT_DEFAULT) == 4 AND isOperatorParameterInCommandAtIndex(pCommand, 2, iLoopStep > 0 ? OT_ADD : OT_SUBTRACT))
      // [CHECK] Ensure iterator is present and correct
      if (bResult = isVariableParameterInCommandAtIndex(pCommand, 0, szIteratorName) AND isVariableParameterInCommandAtIndex(pCommand, 1, szIteratorName) AND findIntegerParameterInCommandByIndex(pCommand, 3, iStepParameter))
         // [CHECK] Ensure step value is correct  (Parameter value is always positive since operator determines increment/decrement)
         bResult = (iLoopStep > 0 AND iStepParameter == iLoopStep) OR (iLoopStep < 0 AND iStepParameter == -iLoopStep);
         
   // Return result
   return bResult;
}


/// Function name  : isExpressionLoopInitialisation
// Description     : Check whether an expression is of the format : '$iterator = initial ± step'
// 
// CONST COMMAND*  pCommand       : [in]  Expression Command 
// CONST TCHAR*   &szIteratorName : [out] Iterator variable
// PARAMETER*     &pInitialValue  : [out] Initial loop value
// INT            &iStepValue     : [out] Step value
// 
// Return Value   : TRUE or FALSE
// 
BOOL  isExpressionLoopInitialisation(CONST COMMAND*  pCommand, CONST TCHAR*  &szIteratorName, PARAMETER*  &pInitialValue, INT&  iStepValue)
{
   BOOL   bResult;

   // [CHECK] Ensure expression has three parameters and a return value:  '$iterator = initial ± step'
   if (bResult = isCommandID(pCommand, CMD_EXPRESSION) AND getCommandParameterCount(pCommand, PT_DEFAULT) == 4 AND findReturnVariableParameterInCommand(pCommand, szIteratorName))
   {
      // [CHECK] Ensure 'step' is integer and 'initial' is integer/variable
      if (bResult = isParameterTypeInCommandAtIndex(pCommand, 3, DT_INTEGER) AND (isParameterTypeInCommandAtIndex(pCommand, 1, DT_VARIABLE) OR isParameterTypeInCommandAtIndex(pCommand, 1, DT_INTEGER)))
      {
         // [CHECK] Ensure operator is add or subtract
         if (bResult = isOperatorParameterInCommandAtIndex(pCommand, 2, OT_ADD) OR isOperatorParameterInCommandAtIndex(pCommand, 2, OT_SUBTRACT))
         {
            /// [SUCCESS] Extract 'step' and 'initial'
            findParameterInCommandByIndex(pCommand, PT_DEFAULT, 1, pInitialValue);
            findIntegerParameterInCommandByIndex(pCommand, 3, iStepValue);

            // [ADDITION] Indicates loop 'step' is negative, since this is just initialising the loop
            if (isOperatorParameterInCommandAtIndex(pCommand, 2, OT_ADD))
               iStepValue *= -1;
         }
      }
   }

   // [FAILED] Ensure all outputs are zero
   if (!bResult)
   {
      szIteratorName = NULL;
      pInitialValue  = NULL;
      iStepValue     = NULL;
   }

   // Return result
   return bResult;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////


