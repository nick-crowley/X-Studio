//
// CodeEdit Iterator (new).cpp : Iterator for CodeEdit text
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

/// Function name  : createCodeEditSingleLineIterator
// Description     : Creates a CodeEdit iterator that iterates through all the characters in a line
// 
// CONST CODE_EDIT_LINE*  pLineData : [in] LineData of the line to iterate through
// 
// Return Value   : New CodeEdit iterator, you are responsible for destroying it
// 
CODE_EDIT_ITERATOR*  createCodeEditSingleLineIterator(CONST CODE_EDIT_LINE*  pLineData)
{
   CODE_EDIT_ITERATOR*  pIterator;

   // Create new object
   pIterator = utilCreateEmptyObject(CODE_EDIT_ITERATOR);

   // Set properties
   pIterator->eType  = CIT_SINGLE_LINE;

   // Initialise CharacterData iterator
   findCodeEditCharacterListItemByIndex(pLineData, 0, pIterator->pCharacterIterator);

   // Set initial position
   pIterator->oLocation.iIndex = -1;

   // Return object
   return pIterator;
}


/// Function name  : createCodeEditMultipleLineIterator
// Description     : Creates a CodeEdit iterator for iterating through the characters between any two locations in the text
//
// CONST CODE_EDIT_DATA*      pWindowData  : [in] Window data
// CONST CODE_EDIT_LOCATION&  oLocation1   : [in] First location, can be specified in any order
// CONST CODE_EDIT_LOCATION&  oLocation2   : [in] Second location, can be specified in any order
// 
// Return Value   : New CodeEdit iterator, you are responsible for destroying it
// 
CODE_EDIT_ITERATOR*  createCodeEditMultipleLineIterator(CONST CODE_EDIT_DATA*  pWindowData, CONST CODE_EDIT_LOCATION&  oLocation1, CONST CODE_EDIT_LOCATION&  oLocation2)
{
   CODE_EDIT_ITERATOR*  pIterator;

   // Create new object
   pIterator = utilCreateEmptyObject(CODE_EDIT_ITERATOR);

   // Determine whether CARET preceeds ORIGIN or vice versa
   switch (compareCodeEditLocations(oLocation1, oLocation2))
   {
   case CR_LESSER:    pIterator->oStart = oLocation1;   pIterator->oFinish = oLocation2;    break;
   case CR_GREATER:   pIterator->oStart = oLocation2;   pIterator->oFinish = oLocation1;    break;
   }

   // Set properties
   pIterator->eType       = CIT_MULTI_LINE;
   pIterator->pWindowData = pWindowData;
   pIterator->oLocation   = pIterator->oStart;
   
   // [CHECK] Initialise line iterator
   if (findCodeEditLineListItemByIndex(pWindowData, pIterator->oStart.iLine, pIterator->pLineIterator))
      // [SUCCES] Initialise character iterator
      findCodeEditCharacterListItemByIndex(extractListItemPointer(pIterator->pLineIterator, CODE_EDIT_LINE), pIterator->oStart.iIndex, pIterator->pCharacterIterator);

   // Prepare initial character index
   pIterator->oLocation.iIndex--;

   // Return object
   return pIterator;
}

/// Function name  : deleteCodeEditIterator
// Description     : Destroys a CodeEdit iterator
// 
// CODE_EDIT_ITERATOR*  &pIterator : [in] CodeEdit iterator to destroy
// 
VOID  deleteCodeEditIterator(CODE_EDIT_ITERATOR*  &pIterator)
{
   // Delete calling object
   utilDeleteObject(pIterator);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : findNextCodeEditCharacter
// Description     : Returns the next character in the CodeEdit
///                      NB: Single line iterator does not return CRLF
//
// CODE_EDIT_ITERATOR*  pIterator : [in/out] CodeEdit iterator
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BOOL  findNextCodeEditCharacter(CODE_EDIT_ITERATOR*  pIterator)
{
   /// [SINGLE LINE] Return the current character and advance to the next, or return FALSE
   if (pIterator->eType == CIT_SINGLE_LINE)
   {
      // [CHECK] Abort when no characters remain
      if (!pIterator->pCharacterIterator)
         return FALSE;

      // [CHARACTER] Set current character
      pIterator->chCharacter = extractListItemPointer(pIterator->pCharacterIterator, CODE_EDIT_CHARACTER)->chCharacter;
      pIterator->oLocation.iIndex++;

      // Advance iterator and return TRUE
      pIterator->pCharacterIterator = pIterator->pCharacterIterator->pNext;
      return TRUE;
   }
   /// [MULTI-LINE] 
   else
   {
      // [CHECK] Ensure we haven't already finished
      if (!pIterator->pLineIterator OR compareCodeEditLocations(pIterator->oLocation, pIterator->oFinish) == CR_EQUAL)
         return FALSE;
      
      // [CHARACTER] Return current character
      else if (pIterator->pCharacterIterator)
      {
         // Set current character
         pIterator->chCharacter = extractListItemPointer(pIterator->pCharacterIterator, CODE_EDIT_CHARACTER)->chCharacter;
         pIterator->oLocation.iIndex++;

         // Advance iterator
         pIterator->pCharacterIterator = pIterator->pCharacterIterator->pNext;
      }
      // [END-OF-LINE] Return carriage return
      else if (pIterator->chCharacter != '\r')
      {
         // Set carriage return
         pIterator->chCharacter = '\r';
         pIterator->oLocation.iIndex++;
      }
      // [NEXT LINE] Return first character of next line
      else if (pIterator->pLineIterator = pIterator->pLineIterator->pNext)
      {
         // Update line
         pIterator->oLocation.iLine++;

         // Re-initialise character iterator
         if (findCodeEditCharacterListItemByIndex(extractListItemPointer(pIterator->pLineIterator, CODE_EDIT_LINE), 0, pIterator->pCharacterIterator))
         {
            // [SUCCESS] Return first character
            pIterator->oLocation.iIndex = -1;
            return findNextCodeEditCharacter(pIterator);
         }
         else
         {
            // [EMPTY LINE] Return CRLF again
            pIterator->chCharacter      = '\r';
            pIterator->oLocation.iIndex = 0;
         }
      }
      // [FINAL LINE] Return FALSE without updating position
      else
         return FALSE;

      // [CHECK] Return TRUE if we haven't reached the finish
      return (compareCodeEditLocations(pIterator->oLocation, pIterator->oFinish) != CR_EQUAL);
   }
}

