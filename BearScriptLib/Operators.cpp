//
// Operators.cpp  :  Functions used by the shunting yard algorithm for identifying operator properties
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createOperatorStack
// Description     : Creates an empty OperatorStack object
// 
// Return Value   : New OperatorStack, you are responsible for destroying it
// 
OPERATOR_STACK*  createOperatorStack()
{
   OPERATOR_STACK*   pStack;     // Stack being created

   // Create new object
   pStack = utilCreateEmptyObject(OPERATOR_STACK);

   // Return new object
   return pStack;
}

/// Function name  : deleteOperatorStack
// Description     : Deletes an operator stack
// 
// OPERATOR_STACK*  &pStack   : [in] Operator stack to delete
// 
VOID    deleteOperatorStack(OPERATOR_STACK*  &pStack)
{
   // Delete calling object
   utilDeleteObject(pStack);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : compareOperatorPrecedence
// Description     : Compares the precedence of two operators, and returns which (if any) is larger
// 
// CONST OPERATOR_TYPE  eOperator1   : [in] First Operator to compare
// CONST OPERATOR_TYPE  eOperator2   : [in] Second operator to compare
// 
// Return Value   : -1 : The precedence of the first operator is less than that of the second
//                   0 : The precedence of both operators is equal
//                   1 : The precedence of the first operator is greater than that of the second
// 
BOOL   compareOperatorPrecedence(CONST OPERATOR_TYPE  eOperator1, CONST OPERATOR_TYPE  eOperator2)
{
   // [CHECK] Return zero if operator precedences are equal
   if (identifyOperatorPrecedence(eOperator1) == identifyOperatorPrecedence(eOperator2))
      return 0;
   
   // Return result
   return (identifyOperatorPrecedence(eOperator1) < identifyOperatorPrecedence(eOperator2) ? -1 : 1);
}


/// Function name  : findOperatorTypeByText
// Description     : Resolves a string containing an operator into its corresponding ID
// 
// CONST TCHAR*    szOperator : [in]  String containing an operator
// OPERATOR_TYPE&  eOutput    : [out] Operation ID if found, otherwise OT_UNRECOGNISED
// 
// Return Value   : TRUE if found, otherwise FALSE
// 
BOOL   findOperatorTypeByText(CONST TCHAR*  szOperator, OPERATOR_TYPE&  eOutput)
{
   SCRIPT_OBJECT*  pScriptObject;      // Object Lookup

   // Prepare
   eOutput = OT_UNRECOGNISED;

   /// Lookup ScriptObject from operator string
   if (findScriptObjectByText(szOperator, pScriptObject))
      // Determine operator from ScriptObject's original game string ID
      eOutput = (OPERATOR_TYPE)pScriptObject->iID;

   // Return TRUE if found
   return (eOutput != OT_UNRECOGNISED);
}


/// Function name  : hasItems
// Description     : Determines whether an OperatorStack contains any operators
// 
// CONST OPERATOR_STACK*  pStack : [in] OperatorStack to test
// 
// Return Value   : TRUE or FALSE
// 
BOOL   hasItems(CONST OPERATOR_STACK*  pStack)
{
   // Return TRUE if stack contains items
   return (pStack->iCount > 0);
}


/// Function name  : isUnaryOperator
// Description     : Determines whether an operator is unary or binary
// 
// CONST OPERATOR_TYPE  eOperator : [in] Operator to examine
// 
// Return Value   : TRUE if unary, FALSE if binary
// 
BOOL   isUnaryOperator(CONST OPERATOR_TYPE  eOperator)
{
   BOOL   bResult;      // Operation result

   // Prepare
   bResult = FALSE;

   // Examine operator
   switch (eOperator)
   {
   /// [UNARY] Return TRUE
   case OT_MINUS:
   case OT_BITWISE_NOT:
   case OT_LOGICAL_NOT:    
      bResult = TRUE;
   }

   // Return result
   return bResult;
}


/// Function name  : isUnaryOperatorOrBracket
// Description     : Determines whether an operator is unary or not
// 
// CONST OPERATOR_TYPE  eOperator : [in] Operator to examine
// 
// Return Value   : TRUE if unary, FALSE if binary
// 
BOOL   isUnaryOperatorOrBracket(CONST OPERATOR_TYPE  eOperator)
{
   BOOL   bResult;      // Operation result

   // Prepare
   bResult = FALSE;

   // Examine operator
   switch (eOperator)
   {
   /// [UNARY/BRACKET] Return TRUE
   case OT_MINUS:
   case OT_BITWISE_NOT:
   case OT_LOGICAL_NOT:    
   case OT_BRACKET_OPEN:
   case OT_BRACKET_CLOSE:
      bResult = TRUE;
   }

   // Return result
   return bResult;
}

/// Function name  : popOperatorStack
// Description     : Removes the top operator from an OperatorStack and returns it
// 
// OPERATOR_STACK*  pStack   : [in] OperatorStack to pop
// 
// Return Value   : Top OperatorStack item, if any, otherwise OT_UNRECOGNISED
// 
OPERATOR_TYPE   popOperatorStack(OPERATOR_STACK*  pStack)
{
   // [CHECK[ Ensure stack is not empty
   if (pStack->iCount == 0)
      return OT_UNRECOGNISED;

   // Pop stack and return item
   return pStack->eItems[--pStack->iCount];
}


/// Function name  : pushOperatorStack
// Description     : Adds a new operator to the top of an OperatorStack
// 
// OPERATOR_STACK*      pStack    : [in] OperatorStack to push
// CONST OPERATOR_TYPE  eOperator : [in] Operator to add to the top of the OperatorStack
// 
VOID     pushOperatorStack(OPERATOR_STACK*  pStack, CONST OPERATOR_TYPE  eOperator)
{
   // [CHECK] Abort if stack is full
   if (pStack->iCount == 32)     // NB: Stack size is 32
      return;

   // Push item onto the stack
   pStack->eItems[pStack->iCount++] = eOperator;
}


/// Function name  : topOperatorStack
// Description     : Return the top operator in an OperatorStack without removing it
// 
// CONST OPERATOR_STACK*  pStack   : [in] OperatorStack to top
// 
// Return Value   : Top OperatorStack item if any, otherwise OT_UNRECOGNISED
// 
OPERATOR_TYPE  topOperatorStack(CONST OPERATOR_STACK*  pStack)
{
   // [CHECK] Ensure stack is not empty
   if (pStack->iCount == 0)
      return OT_UNRECOGNISED;

   // Return top item
   return pStack->eItems[pStack->iCount - 1];
}


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : [GENERATION] identifyOperatorAssociativity
// Description     : [GENERATION] Determines whether an operator is left or right associative
// 
// CONST OPERATOR_TYPE  eOperator : [in] Operator to query
// 
// Return type : Operator associativity

OPERATOR_ASSOCIATIVITY      identifyOperatorAssociativity(CONST OPERATOR_TYPE  eOperator)
{
   switch (eOperator)
   {
   case OT_BITWISE_NOT:
   case OT_MINUS:
   case OT_LOGICAL_NOT:
      return OA_RIGHT;

   case OT_BRACKET_OPEN:
   case OT_BRACKET_CLOSE:
      return OA_NONE;

   default:
      return OA_LEFT;
   }
}


/// Function name  : [GENERATION] identifyOperatorPrecedence
// Description     : [GENERATION] Determines an operator's precedence
//
// CONST OPERATOR_TYPE  eOperator : Operator
// 
// Return type : Operator precedence
//
UINT      identifyOperatorPrecedence(CONST OPERATOR_TYPE  eOperator)
{
   UINT  iPrecedence;

   switch (eOperator)
   {
   case OT_BITWISE_NOT:
   case OT_MINUS:
   case OT_LOGICAL_NOT:
      iPrecedence = 10;
      break;

   case OT_MULTIPLY:
   case OT_DIVIDE:
   case OT_MODULUS:
      iPrecedence = 9;
      break;

   case OT_ADD:
   case OT_SUBTRACT:
      iPrecedence = 8;
      break;

   case OT_GREATER_THAN:
   case OT_LESS_THAN:
   case OT_GREATER_THAN_EQUAL:
   case OT_LESS_THAN_EQUAL:
      iPrecedence = 7;
      break;

   case OT_EQUALITY:
   case OT_EQUALITY_NOT:
      iPrecedence = 6;
      break;

   case OT_BITWISE_AND:
      iPrecedence = 5;
      break;

   case OT_BITWISE_XOR:
      iPrecedence = 4;
      break;

   case OT_BITWISE_OR:
      iPrecedence = 3;
      break;

   case OT_LOGICAL_AND:
      iPrecedence = 2;
      break;

   case OT_LOGICAL_OR:
      iPrecedence = 1;
      break;

   case OT_BRACKET_OPEN:
   case OT_BRACKET_CLOSE:
      iPrecedence = 0;
      break;

   default:
      iPrecedence = NULL;
   }

   // Return precedence
   return iPrecedence;
}


/// Function name  : [GENERATION] identifyOperatorType
// Description     : [GENERATION] Determine the operator type from a string containing an operator
//
// CONST TCHAR*  szOperator : String containing 1 operator
// 
// Return type : operator type if successful or OT_UNRECOGNISED if not

OPERATOR_TYPE    identifyOperatorType(CONST TCHAR*  szOperator)
{
   OPERATOR_TYPE   eType;     // Result
   
   // Prepare
   eType = OT_UNRECOGNISED;

   // Examine first character of operator
   switch (szOperator[0])
   {
   case '(': eType = OT_BRACKET_OPEN;     break;
   case ')': eType = OT_BRACKET_CLOSE;    break;
   case '&': eType = OT_BITWISE_AND;      break;
   case '|': eType = OT_BITWISE_OR;       break;
   case '^': eType = OT_BITWISE_XOR;      break;
   case '~': eType = OT_BITWISE_NOT;      break;
   case '+': eType = OT_ADD;              break;
   case '-': eType = OT_SUBTRACT;         break;
   case '*': eType = OT_MULTIPLY;         break;
   case '/': eType = OT_DIVIDE;           break;
   case '=': eType = OT_EQUALITY;         break;

   // Ambiguously single/double character operators
   case '!': eType = (szOperator[1] == '=' ? OT_EQUALITY_NOT       : OT_LOGICAL_NOT);      break;
   case '>': eType = (szOperator[1] == '=' ? OT_GREATER_THAN_EQUAL : OT_GREATER_THAN);      break;
   case '<': eType = (szOperator[1] == '=' ? OT_LESS_THAN_EQUAL    : OT_LESS_THAN);         break;

   // Phrases
   default:
      if (utilCompareStringN(szOperator, "OR", 2))
         eType = OT_LOGICAL_OR;
      else if (utilCompareStringN(szOperator, "AND", 3))
         eType = OT_LOGICAL_AND;
      else if (utilCompareStringN(szOperator, "MOD", 3))
         eType = OT_MODULUS;
      break;
   }
   
   // [CHECK] Operator should be recognised...
   ASSERT(eType != OT_UNRECOGNISED);
   
   // Return operator ID
   return eType;
}

