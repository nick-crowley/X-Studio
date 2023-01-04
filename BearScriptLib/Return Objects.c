//
// Return Objects.cpp  :  Functions to alter and interpret Return Object IDs
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                 CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createReturnObjectConditional
// Description     : Creates a new ReturnObject representing a branching conditional command
// 
// CONST CONDITIONAL_ID  eConditional : [in] Conditional for the ReturnObject to represent
// 
// Return Value   : New ReturnObject, you are responsible for destroying it
// 
RETURN_OBJECT*   createReturnObjectConditional(CONST CONDITIONAL_ID  eConditional)
{
   RETURN_OBJECT*  pReturnObject;

   // Create conditional ReturnObject
   pReturnObject        = utilCreateEmptyObject(RETURN_OBJECT);
   pReturnObject->eType = ROT_CONDITIONAL;

   // Set properties
   pReturnObject->eConditional = eConditional;
   pReturnObject->eReturnType  = identifyReturnTypeFromConditional(eConditional);

   // Set destination
   switch (eConditional)
   {
   case CI_DISCARD:
   case CI_START:    pReturnObject->iDestination = NULL;         break;
   default:          pReturnObject->iDestination = EMPTY_JUMP;   break;
   }

   // Return new object
   return pReturnObject;
}


/// Function name  : createReturnObjectFromInteger
// Description     : Create a new ReturnObject from an packed integer
// 
// CONST INT  iReturnValue : [in] Packed integer containing the variable ID or conditional information
// 
// Return Value   : New ReturnObject, you are responsible for destroying it
// 
RETURN_OBJECT*   createReturnObjectFromInteger(CONST INT  iReturnValue)
{
   RETURN_OBJECT*  pReturnObject;

   // Create new object
   pReturnObject = utilCreateEmptyObject(RETURN_OBJECT);
   
   // Determine type
   pReturnObject->eType = identifyReturnObjectType(iReturnValue);

   // Set properties
   switch (pReturnObject->eType)
   {
   // [CONDITIONAL] Extract packed properties
   case ROT_CONDITIONAL:
      pReturnObject->iDestination = (iReturnValue INCLUDES 0x00ffff00) >> 8;
      pReturnObject->eConditional = (CONDITIONAL_ID)(iReturnValue INCLUDES 0x000000ff);
      pReturnObject->eReturnType  = (RETURN_TYPE)((iReturnValue INCLUDES 0xff000000) >> 24);
      break;

   // [VARIABLE] Save the variable ID
   case ROT_VARIABLE:
      pReturnObject->iVariableID  = iReturnValue;
      break;
   }

   // Return new object
   return pReturnObject;
}


/// Function name : createReturnObjectVariable
// Description    : Creates a ReturnObject representing a variable assignment
// 
// CONST UINT  iVariableID : [in] Zero-based index of the variable to represent
// 
// Return Value : New RETURN_OBJECT, you are responsible for destroying it
// 
RETURN_OBJECT*   createReturnObjectVariable(CONST UINT  iVariableID)
{
   RETURN_OBJECT*  pReturnObject;   // ReturnObject being created

   // Create variable ReturnObject
   pReturnObject        = utilCreateEmptyObject(RETURN_OBJECT);
   pReturnObject->eType = ROT_VARIABLE;

   // Set properties
   pReturnObject->iVariableID = iVariableID;

   // Return new object
   return pReturnObject;
}


/// Function name  : deleteReturnObject
// Description     : Delete a ReturnObject object
// 
// RETURN_OBJECT*  &pReturnObject   : [in] ReturnObject ot delete
// 
VOID   deleteReturnObject(RETURN_OBJECT*  &pReturnObject)
{
   // Delete calling object
   utilDeleteObject(pReturnObject);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : calculateReturnObjectEncoding
// Description     : Encodes the data in a ReturnObject into the format used in MSCI scripts
//
// CONST RETURN_OBJECT*  pReturnObject : [in] ReturnObject containing the ReturnType, JumpDestination and Conditional to encode into an integer
// 
// Return type : Packed 32-bit ReturnObject 
//
INT     calculateReturnObjectEncoding(CONST RETURN_OBJECT*  pReturnObject)
{
   switch (pReturnObject->eType)
   {
   // [VARIABLE] Return the zero-based variable ID
   case ROT_VARIABLE:
      return pReturnObject->iVariableID;

   // [CONDITIONAL] Encode the jump and conditional information
   case ROT_CONDITIONAL:
      // RETURN_TYPE in the H.O. byte, DESTINATION in the middle word, CONDITIONAL in the L.O. byte
      return ((INT)pReturnObject->eReturnType << 24) WITH (pReturnObject->iDestination << 8) WITH (INT)pReturnObject->eConditional;
   }

   return NULL;
}


/// Function name  : identifyReturnObjectType
// Description     : Determines whether a ReturnObject encoded as an integer represents a variable or a conditional
// 
// CONST INT  iReturnValue : [in] Packed ReturnObject to test
// 
// Return Value   : ROT_CONDITIONAL or ROT_VARIABLE
// 
RETURN_OBJECT_TYPE   identifyReturnObjectType(CONST INT  iReturnValue)
{
   return (iReturnValue < 0 ? ROT_CONDITIONAL : ROT_VARIABLE);
}


/// Function name  : isReturnObject
// Description     : Determines if parameter syntax represents a ReturnObject or not
// 
// CONST PARAMETER_SYNTAX  eSyntax : Syntax to check
// 
// Return type : TRUE if ReturnObject, FALSE if not
BearScriptAPI
BOOL   isReturnObject(CONST PARAMETER_SYNTAX  eSyntax)
{
   switch (eSyntax)
   {
   case PS_RETURN_OBJECT:
   case PS_RETURN_OBJECT_IF:
   case PS_RETURN_OBJECT_IF_START:
   case PS_INTERRUPT_RETURN_OBJECT_IF:
      return TRUE;

   default:
      return FALSE;
   }
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : identifyReturnTypeFromConditional
// Description     : Identifies the associated ReturnType for the specified conditional
// 
// CONST CONDITIONAL_ID   eConditional : [in] Conditional ID to determine the return type for
// 
// Return Value   : ReturnType associated with the specified conditional id
// 
RETURN_TYPE    identifyReturnTypeFromConditional(CONST CONDITIONAL_ID   eConditional)
{
   RETURN_TYPE    eOutput;    // Resultant ReturnType for the input conditional id

   // Prepare
   eOutput = RT_DISCARD;

   // Determine type from conditional
   switch (eConditional)
   {
   // [POSITIVES] Jump if positive
   case CI_IF:
   case CI_WHILE:
   case CI_ELSE_IF:
   case CI_SKIP_IF_NOT:    
      eOutput = RT_JUMP_IF_TRUE;    
      break;

   // [NEGATIVES] Jump if negative
   case CI_IF_NOT:
   case CI_WHILE_NOT:
   case CI_SKIP_IF:
   case CI_ELSE_IF_NOT:    
      eOutput = RT_JUMP_IF_FALSE;   
      break;

   // [START] Class this as 'discard' presumably, since the calls are asynchronous
   default:                
      eOutput = RT_DISCARD;         
      break;
   }

   // Return type
   return eOutput;
}





