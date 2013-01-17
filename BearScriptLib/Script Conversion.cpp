//
// Script Conversion.cpp : Helper objects for translating and generating scripts
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

/// Function name  : createCommandStream
// Description     : Creates a CommandStream
// 
// Return Value   : New CommandStream, you are responsible for destroying it
// 
COMMAND_STREAM*   createCommandStream()
{
   COMMAND_STREAM*  pCommandStream;

   // Prepare
   pCommandStream = utilCreateEmptyObject(COMMAND_STREAM);

   // Create STANDARD and AUXILIARY COMMAND lists
   pCommandStream->pStandardList  = createList(destructorCommand);
   pCommandStream->pAuxiliaryList = createList(destructorCommand);

   // Return new object
   return pCommandStream;
}


/// Function name  : createScriptGenerator
// Description     : Creates a Generator object for converting MSCI scripts into lists of COMMAND objects
// 
// Return type : New ScriptGenerator object, you are responsible for destroying it
//
SCRIPT_GENERATOR*   createScriptGenerator()
{
   SCRIPT_GENERATOR*  pScriptGenerator;

   // Prepare
   pScriptGenerator = utilCreateEmptyObject(SCRIPT_GENERATOR);

   // Create Arguments/Variables list
   pScriptGenerator->pVariablesList = createList(deleteVariableNameTreeNode);

   // Create INPUT and OUTPUT
   pScriptGenerator->pInputList    = createList(destructorCommand);
   pScriptGenerator->pOutputStream = createCommandStream();

   // Return new object
   return pScriptGenerator;
}


/// Function name  : createScriptTranslator
// Description     : Creates a Translator object for converting MSCI scripts into lists of COMMAND objects
// 
// Return type : New ScriptTranslator object, you are responsible for destroying it
//
SCRIPT_TRANSLATOR*   createScriptTranslator()
{
   SCRIPT_TRANSLATOR*  pScriptTranslator;

   // Prepare
   pScriptTranslator = utilCreateEmptyObject(SCRIPT_TRANSLATOR);

   // Create Arguments/Variables list
   pScriptTranslator->pVariablesList = createList(deleteVariableNameTreeNode);

   // Create OUTPUT
   pScriptTranslator->pOutputList = createList(destructorCommand);

   // Return new object
   return pScriptTranslator;
}


/// Function name  : deleteCommandStream
// Description     : Destroys a CommandStream
// 
// COMMAND_STREAM*  &pCommandStream   : [in] CommandStream to destroy
// 
VOID  deleteCommandStream(COMMAND_STREAM*  &pCommandStream)
{
   // Delete lists
   deleteList(pCommandStream->pStandardList);
   deleteList(pCommandStream->pAuxiliaryList);

   // Delete calling object
   utilDeleteObject(pCommandStream);
}


/// Function name  : deleteScriptGenerator
// Description     : Destroys a ScriptGenerator
// 
// SCRIPT_GENERATOR*  &ScriptGenerator   : [in] ScriptGenerator to destroy
// 
VOID  deleteScriptGenerator(SCRIPT_GENERATOR*  &pGenerator)
{
   // Delete Arguments/Variables list
   deleteList(pGenerator->pVariablesList);

   // Delete XML Tree
   if (pGenerator->pXMLTree)
      deleteXMLTree(pGenerator->pXMLTree);

   // Zero Layout
   utilZeroObject(&pGenerator->oLayout, XML_SCRIPT_LAYOUT);

   // Delete INPUT and OUTPUT
   deleteList(pGenerator->pInputList);
   deleteCommandStream(pGenerator->pOutputStream);
   
   // Delete Generator
   utilDeleteObject(pGenerator);
}


/// Function name  : deleteScriptTranslator
// Description     : Destroys a ScriptTranslator
// 
// SCRIPT_TRANSLATOR*  &ScriptTranslator   : [in] ScriptTranslator to destroy
// 
VOID  deleteScriptTranslator(SCRIPT_TRANSLATOR*  &pTranslator)
{
   // Delete Arguments/Variables list
   deleteList(pTranslator->pVariablesList);

   // Delete XML Tree
   if (pTranslator->pXMLTree)
      deleteXMLTree(pTranslator->pXMLTree);

   // Delete standard iterator
   if (pTranslator->pStandardIterator)
      deleteCommandNode(pTranslator->pStandardIterator, FALSE);

   // Delete auxiliary iterator
   if (pTranslator->pAuxiliaryIterator)
      deleteCommandNode(pTranslator->pAuxiliaryIterator, FALSE);

   // Zero Layout
   utilZeroObject(&pTranslator->oLayout, XML_SCRIPT_LAYOUT);

   // Delete OUTPUT
   deleteList(pTranslator->pOutputList);
   
   // Delete Translator
   utilDeleteObject(pTranslator);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : appendArgumentNameToGenerator
// Description     : Adds a new argument name to a ScriptGenerator's list of arguments+variables and calculates the correct ID
///                              Argument names must be unique
// 
// SCRIPT_GENERATOR*  pGenerator     : [in] ScriptGenerator
// CONST TCHAR*       szVariableName : [in] Name of the argument to add
// 
VOID  appendArgumentNameToGenerator(SCRIPT_GENERATOR*  pGenerator, CONST TCHAR*  szArgumentName)
{
   VARIABLE_NAME*  pVariableName;   // VariableName being created

   /// Create new VariableName with a zero-based ID
   pVariableName = createVariableName(szArgumentName, getListItemCount(pGenerator->pVariablesList));

   // Append to Generators's VariableNames list
   appendObjectToList(pGenerator->pVariablesList, (LPARAM)pVariableName);
}


/// Function name  : appendCommandToGenerator
// Description     : Add a COMMAND to a generator's INPUT or OUTPUT list.
// 
// SCRIPT_FILE*        pScriptFile : [in] ScriptFile to be appened
// CONST COMMAND_LIST  eList       : [in] INPUT/OUTPUT list ID
// COMMAND*            pCommand    : [in] COMMAND to be appended to the ScriptFile
// 
VOID   appendCommandToGenerator(SCRIPT_GENERATOR*  pGenerator, CONST COMMAND_LIST  eList, COMMAND*  pCommand)
{
   // Add COMMAND to the appropriate list
   switch (eList)
   {
   /// [INPUT] Add to INPUT LIST
   case CL_INPUT: 
      // Assign a sequential zero-based index
      pCommand->iIndex = getListItemCount(pGenerator->pInputList);

      // Add to INPUT LIST
      appendObjectToList(pGenerator->pInputList, (LPARAM)pCommand);
      break;

   /// [OUTPUT] Add to STANDARD/AUXILIARY OUTPUT
   case CL_OUTPUT: 
      // [STANDARD] 
      if (isCommandType(pCommand, CT_STANDARD))
      {
         // Assign a sequential zero-based index
         pCommand->iIndex = getListItemCount(pGenerator->pOutputStream->pStandardList);

         /// Add to STANDARD OUT
         appendObjectToList(pGenerator->pOutputStream->pStandardList, (LPARAM)pCommand);
      }
      // [AUXILIARY] 
      else
      {
         // Assign a sequential zero-based index
         pCommand->iIndex = getListItemCount(pGenerator->pOutputStream->pAuxiliaryList);

         /// Add to AUXILIARY OUT
         appendObjectToList(pGenerator->pOutputStream->pAuxiliaryList, (LPARAM)pCommand);

         /// Associate COMMAND with the next standard command
         pCommand->iReferenceIndex = getListItemCount(pGenerator->pOutputStream->pStandardList);
      }
      break;
   }
}


/// Function name  : appendCommandToTranslator
// Description     : Adds a COMMAND to a ScriptTranslator's COMMAND output
// 
// SCRIPT_TRANSLATOR*  pTranslator : [in] ScriptTranslator
// COMMAND*            pCommand    : [in] COMMAND to append
// 
VOID   appendCommandToTranslator(SCRIPT_TRANSLATOR*  pTranslator, COMMAND*  pCommand)
{
   /// Assign a sequential zero-based index
   pCommand->iIndex = getListItemCount(pTranslator->pOutputList);

   /// Add to OUTPUT
   appendObjectToList(pTranslator->pOutputList, (LPARAM)pCommand);
}


/// Function name  : appendVariableNameToTranslator
// Description     : Adds a new argument/variable name to a ScriptTranslator' list of arguments+variables and calculates the correct ID
///                              Argument/Variable Names must be unique
// 
// SCRIPT_TRANSLATOR*  pTranslator     : [in] ScriptTranslator
// CONST TCHAR*        szVariableName  : [in] Name of the argument/variable to add
// 
VOID  appendVariableNameToTranslator(SCRIPT_TRANSLATOR*  pTranslator, CONST TCHAR*  szVariableName)
{
   VARIABLE_NAME*  pVariableName;   // VariableName being created

   /// Create new VariableName with a zero-based ID
   pVariableName = createVariableName(szVariableName, getListItemCount(pTranslator->pVariablesList));

   // Append to Translator's VariableNames list
   appendObjectToList(pTranslator->pVariablesList, (LPARAM)pVariableName);
}


/// Function name  : calculateGeneratorForEachCommandCount
// Description     : Counts the number of existing 'for each' commands already generated. This is used for assigning unique iterator variable names
// 
// SCRIPT_GENERATOR*  pGenerator : [in] ScriptGenerator
// 
// Return Value   : Zero-based count
// 
UINT  calculateGeneratorForEachCommandCount(SCRIPT_GENERATOR*  pGenerator)
{
   COMMAND*  pCommand;
   UINT      iCount = 0;

   /// Iterate through standard commands
   for (LIST_ITEM*  pIterator = getListHead(pGenerator->pOutputStream->pStandardList); pCommand = extractListItemPointer(pIterator, COMMAND); pIterator = pIterator->pNext)
   {
      // [FOR EACH IN ARRAY] Add to count
      if (isCommandID(pCommand, CMD_FOR_EACH))
         iCount++;
   }

   // Return result
   return iCount;
}


/// Function name  : calculateGeneratorVariableIDFromName
// Description     : Returns the ID of the argument/variable in a ScriptGenerator with a given name.
//                    -> If the argument/variable does not already exist, it is created
// 
// SCRIPT_GENERATOR*  pGenerator  : [in] ScriptGenerator to search
// CONST TCHAR*       szVariable  : [in] Name of the argument/variable to search for
// 
// Return Value   : One-based Variable ID
// 
UINT  calculateGeneratorVariableIDFromName(SCRIPT_GENERATOR*  pGenerator, CONST TCHAR*  szVariable)
{
   VARIABLE_NAME*  pVariableName;      // Pre-existing VariableName or newly created VariableName

   /// Iterate through existing VariableNames for a match
   for (LIST_ITEM*  pIterator = getListHead(pGenerator->pVariablesList); pIterator; pIterator = pIterator->pNext)
   {
      // Prepare
      pVariableName = extractListItemPointer(pIterator, VARIABLE_NAME);

      // [CHECK] Does current VariableName match?
      if (utilCompareStringVariables(pVariableName->szName, szVariable))
         /// [FOUND] Return associated ID
         return pVariableName->iID;
   }

   /// [NOT FOUND] Create a new VariableName with a zero-based ID
   pVariableName = createVariableName(szVariable, getListItemCount(pGenerator->pVariablesList));

   // Append to Generators's VariableNames list
   appendObjectToList(pGenerator->pVariablesList, (LPARAM)pVariableName);

   /// [FOUND] Return ID of new VariableName
   return pVariableName->iID;
}



/// Function name  : destroyCommandInTranslatorOutputByIndex
// Description     : Destroy a COMMAND in a translator OUTPUT list
// 
// CONST SCRIPT_TRANSLATOR*  pTranslator : [in]  Script Translator
// CONST UINT                iIndex      : [in]  Zero based index of the COMMAND to retrieve
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BOOL  destroyCommandInTranslatorOutputByIndex(CONST SCRIPT_TRANSLATOR*  pTranslator, CONST UINT  iIndex)
{
   // Return TRUE if COMMAND was found and destroyed
   return destroyListItemByIndex(pTranslator->pOutputList, iIndex);
}


/// Function name  : destroyCommandInTranslatorOutputByIndex
// Description     : Destroy a COMMAND in a translator OUTPUT list
// 
// CONST SCRIPT_TRANSLATOR*  pTranslator : [in] Script Translator
// CONST UINT                iIndex      : [in] Zero based index of the COMMAND to retrieve
// CONST UINT                iCount      : [in] Number of COMMANDS to destroy
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BOOL  destroyCommandsInTranslatorOutputByIndex(CONST SCRIPT_TRANSLATOR*  pTranslator, CONST UINT  iIndex, CONST UINT  iCount)
{
   BOOL  bResult = TRUE;

   /// Delete <count> COMMANDS at the target index
   for (UINT  i = 0; bResult AND i < iCount; i++)
      bResult = destroyCommandInTranslatorOutputByIndex(pTranslator, iIndex);

   // Return result
   return bResult;
}


/// Function name  : findCommandInGeneratorInput
// Description     : Lookup a COMMAND in a generator INPUT list
// 
// CONST SCRIPT_GENERATOR*  pGenerator : [in]  Script generator
// CONST UINT               iIndex     : [in]  Zero based index of the COMMAND to retrieve
// COMMAND*                &pOutput    : [out] Resultant COMMAND if found, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BOOL  findCommandInGeneratorInput(CONST SCRIPT_GENERATOR*  pGenerator, CONST UINT  iIndex, COMMAND*  &pOutput)
{
   /// Lookup COMMAND and return result
   return findListObjectByIndex(pGenerator->pInputList, iIndex, (LPARAM&)pOutput);
}


/// Function name  : findCommandInTranslatorOutput
// Description     : Lookup a COMMAND in a translator OUTPUT list
// 
// CONST SCRIPT_TRANSLATOR*  pTranslator : [in]  Script Translator
// CONST UINT                iIndex      : [in]  Zero based index of the COMMAND to retrieve
// COMMAND*                 &pOutput     : [out] Resultant COMMAND if found, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BearScriptAPI
BOOL  findCommandInTranslatorOutput(CONST SCRIPT_TRANSLATOR*  pTranslator, CONST UINT  iIndex, COMMAND*  &pOutput)
{
   /// Lookup COMMAND and return result
   return findListObjectByIndex(pTranslator->pOutputList, iIndex, (LPARAM&)pOutput);
}


/// Function name  : findCommandInGeneratorOutput
// Description     : Lookup a COMMAND in a generators STANDARD OUT or AUXILIARY OUT
// 
// CONST SCRIPT_GENERATOR*  pGenerator : [in]  Script generator
// CONST COMMAND_TYPE       eType      : [in]  Standard or Auxiliary
// CONST UINT               iIndex     : [in]  Zero based index of the COMMAND to retrieve
// COMMAND*                &pOutput    : [out] Resultant COMMAND if found, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BearScriptAPI
BOOL  findCommandInGeneratorOutput(CONST SCRIPT_GENERATOR*  pGenerator, CONST COMMAND_TYPE  eType, CONST UINT  iIndex, COMMAND*  &pOutput)
{
   // Prepare
   pOutput = NULL;

   switch (eType)
   {
   /// [STANDARD] Search the STANDARD OUTPUT
   case CT_STANDARD:  findListObjectByIndex(pGenerator->pOutputStream->pStandardList, iIndex, (LPARAM&)pOutput);   break;
   /// [AUXILIARY] Search the AUXILIARY OUTPUT
   case CT_AUXILIARY: findListObjectByIndex(pGenerator->pOutputStream->pAuxiliaryList, iIndex, (LPARAM&)pOutput);  break;
   }
   
   // Return TRUE if COMMAND was found
   return (pOutput != NULL);
}


/// Function name  : findVariableNameInGeneratorByIndex
// Description     : Locate a VariableName within a ScriptGenerator by ZERO-BASED index
// 
// SCRIPT_TRANSLATOR*  pTranslator : [in]  Script Generator
// CONST UINT          iIndex      : [in]  Zero-based index of the VariableName to search for
// VARIABLE_NAME*     &pOutput     : [out] Resultant variable if found, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BOOL  findVariableNameInGeneratorByIndex(CONST SCRIPT_GENERATOR*  pGenerator, CONST UINT  iIndex, VARIABLE_NAME*  &pOutput)
{
   // [CHECK] Ensure Generator exists
   ASSERT(pGenerator);

   /// Lookup Variable name
   return findListObjectByIndex(pGenerator->pVariablesList, iIndex, (LPARAM&)pOutput);
}


/// Function name  : findVariableNameInTranslatorByIndex
// Description     : Locate a VariableName within a ScriptTranslator by ONE-BASED index
// 
// SCRIPT_TRANSLATOR*  pTranslator : [in]  Script Translator
// CONST UINT          iIndex      : [in]  One-based index of the VariableName to search for
// VARIABLE_NAME*     &pOutput     : [out] Resultant variable if found, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BOOL  findVariableNameInTranslatorByIndex(CONST SCRIPT_TRANSLATOR*  pTranslator, CONST UINT  iIndex, VARIABLE_NAME*  &pOutput)
{
   // [CHECK] Ensure Translator exists and index is not zero
   ASSERT(pTranslator);

   /// Lookup Variable name
   return findListObjectByIndex(pTranslator->pVariablesList, iIndex, (LPARAM&)pOutput);
}


/// Function name  : getScriptGeneratorVariablesCount
// Description     : Retrieves the number of arguments+variables in the ScriptGenerator's variables list
// 
// CONST SCRIPT_GENERATOR*  pGenerator   : [in] ScriptpGenerator to query
// 
// Return Value   : Number of arguments+variables in the generator's list
// 
UINT  getScriptGeneratorVariablesCount(CONST SCRIPT_GENERATOR*  pGenerator)
{
   // [CHECK] Ensure generator exists
   ASSERT(pGenerator);

   // Return length of VariableNames list
   return getListItemCount(pGenerator->pVariablesList);
}


/// Function name  : getScriptTranslatorOutputCount
// Description     : Retrieves the number of commands in the script translator output
// 
// CONST SCRIPT_TRANSLATOR*  pTranslator : [in] ScriptTranslator to query
// 
// Return Value   : Number of commands in the translator output
// 
UINT  getScriptTranslatorOutputCount(CONST SCRIPT_TRANSLATOR*  pTranslator)
{
   // [CHECK] Ensure translator exists
   ASSERT(pTranslator);

   // Return length of VariableNames list
   return getListItemCount(pTranslator->pOutputList);
}


/// Function name  : getScriptTranslatorVariablesCount
// Description     : Retrieves the number of arguments+variables in the ScriptGenerator's variables list
// 
// CONST SCRIPT_TRANSLATOR*  pTranslator : [in] ScriptTranslator to query
// 
// Return Value   : Number of arguments+variables in the translator's list
// 
UINT  getScriptTranslatorVariablesCount(CONST SCRIPT_TRANSLATOR*  pTranslator)
{
   // [CHECK] Ensure translator exists
   ASSERT(pTranslator);

   // Return length of VariableNames list
   return getListItemCount(pTranslator->pVariablesList);
}


/// Function name  : initScriptFileGenerator
// Description     : Destroys the ScriptFile TRANSLATOR/GENERATOR and prepares a new GENERATOR
// 
// SCRIPT_FILE*   pScriptFile  : [in] ScriptFile
// 
BearScriptAPI
VOID   initScriptFileGenerator(SCRIPT_FILE*  pScriptFile)
{
   // Delete I/O buffers
   deleteGameFileIOBuffers(pScriptFile);

   // Delete translator
   if (pScriptFile->pTranslator)
      deleteScriptTranslator(pScriptFile->pTranslator);

   // Delete previous generator, if any
   if (pScriptFile->pGenerator)
      deleteScriptGenerator(pScriptFile->pGenerator);

   /// Create Generator
   pScriptFile->pGenerator = createScriptGenerator();
}


/// Function name  : insertVirtualCommandToTranslator
// Description     : Inserts a virtual COMMAND into a ScriptTranslator's output list at a specific index
// 
// SCRIPT_TRANSLATOR*  pTranslator : [in] ScriptTranslator
// CONST UINT          iIndex      : [in] Zero-based index
// COMMAND*            pCommand    : [in] COMMAND to append
// 
VOID   insertVirtualCommandToTranslator(SCRIPT_TRANSLATOR*  pTranslator, CONST UINT  iIndex, COMMAND*  pCommand)
{
   /// Assign input index
   pCommand->iIndex = iIndex;

   /// Insert into OUTPUT list
   insertObjectIntoListByIndex(pTranslator->pOutputList, iIndex, (LPARAM)pCommand);
}

/// Function name  : isLastGeneratedCommandIndentedBySkipIf
// Description     : Determines whether the last generated standard command has a skip-if/do-if conditional
// 
// CONST SCRIPT_GENERATOR*  pGenerator : [in] ScriptFile generator
// 
// Return Value   : TRUE / FALSE
// 
BOOL  isLastGeneratedCommandIndentedBySkipIf(CONST SCRIPT_GENERATOR*  pGenerator)
{
   COMMAND*  pCommand;
   BOOL      bResult;

   // Prepare
   bResult = FALSE;

   /// Search backwards for the last standard command
   for (LIST_ITEM*  pIterator = getListTail(pGenerator->pInputList); pCommand = extractListItemPointer(pIterator, COMMAND); pIterator = pIterator->pPrev)
   {
      // [NON-COMMENT] Check for presence of skip/do-if
      if (!isCommandComment(pCommand))
      {
         bResult = (pCommand->eConditional == CI_SKIP_IF OR pCommand->eConditional == CI_SKIP_IF_NOT);
         break;
      }
   }

   // Return result
   return bResult;
}

/// Function name  : isLastGeneratedCommandIndentedBySkipIf
// Description     : Determines whether the last translated standard command has a skip-if/do-if conditional
// 
// CONST SCRIPT_TRANSLATOR*  pTranslator : [in] ScriptFile translator
// 
// Return Value   : TRUE / FALSE
// 
BOOL  isLastTranslatedCommandIndentedBySkipIf(CONST SCRIPT_TRANSLATOR*  pTranslator, CONST LIST_ITEM*  pTargetCommand)
{
   COMMAND*  pCommand;
   BOOL      bResult;

   // Prepare
   bResult = FALSE;

   /// Search backwards for the last standard command
   for (LIST_ITEM*  pIterator = pTargetCommand->pPrev; pCommand = extractListItemPointer(pIterator, COMMAND); pIterator = pIterator->pPrev)
   {
      // [STANDARD] Check for presence of skip/do-if
      if (isCommandType(pCommand, CT_STANDARD))
      {
         bResult = (pCommand->eConditional == CI_SKIP_IF OR pCommand->eConditional == CI_SKIP_IF_NOT);
         break;
      }
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


