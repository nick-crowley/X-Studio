//
// Script Files.cpp : Functions related to the SCRIPT_FILE object
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       CREATION / DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createScriptFileByOperation
// Description     : creates an empty ScriptFile for TRANSLATION or GENERATION
// 
// Return type : New ScriptFile, you are responsible for destroying it
//
BearScriptAPI 
SCRIPT_FILE*   createScriptFileByOperation(CONST SCRIPT_FILE_OPERATION  eOperation, CONST TCHAR*  szFullPath)
{
   SCRIPT_FILE*  pScriptFile;

   // Create new SCRIPT_FILE
   pScriptFile = utilCreateEmptyObject(SCRIPT_FILE);

   // Create ARGUMENT tree
   pScriptFile->pArgumentTreeByID = createArgumentTreeByID();

   // Create Translator/Generator
   switch (eOperation)
   {
   case SFO_TRANSLATION:  pScriptFile->pTranslator = createScriptTranslator();   break;
   case SFO_GENERATION:   pScriptFile->pGenerator  = createScriptGenerator();    break;
   }

   // Set Path
   setGameFilePath(pScriptFile, szFullPath);

   // Return new object
   return pScriptFile;
}


/// Function name  : createScriptFileFromFile
// Description     : Create a new ScriptFile from an open file
// 
// CONST TCHAR*  szFullPath    : [in] File path
// CONST BYTE*   szInputBuffer : [in] ANSI file buffer
// CONST UINT    iInputSize    : [in] Length of above buffer, in bytes
// 
// Return Value   : New ScriptFile, you are responsible for destroying it
// 
BearScriptAPI
SCRIPT_FILE*  createScriptFileFromFile(CONST TCHAR*  szFullPath, CONST BYTE*  szInputBuffer, CONST UINT  iInputSize)
{
   SCRIPT_FILE*  pNewScriptFile;

   // Create object
   pNewScriptFile = createScriptFileByOperation(SFO_TRANSLATION, szFullPath);

   // Set properties
   pNewScriptFile->iInputSize = iInputSize;

   // Create UNICODE copy of input buffer (Also updates the buffer size if characters were dropped in translation)
   pNewScriptFile->szInputBuffer = utilTranslateStringToUNICODEEx((CONST CHAR*)szInputBuffer, pNewScriptFile->iInputSize);

   // Return new
   return pNewScriptFile;
}


/// Function name  : createScriptFileFromText
// Description     : Creates an empty ScriptFile used for the underlying data of a new ScriptDocument
// 
// Return Value   : New ScriptFile, you are responsible for destroying it
// 
BearScriptAPI
SCRIPT_FILE*  createScriptFileFromText(CONST TCHAR*  szText, CONST TCHAR*  szFullPath)
{
   SCRIPT_FILE*  pScriptFile;

   // Create 'GENERATION' ScriptFile
   pScriptFile = createScriptFileByOperation(SFO_GENERATION, szFullPath);

   // Set SCRIPT-NAME and FILE-PATH
   setScriptNameFromPath(pScriptFile);

   // Initialise DESCRIPTION and COMMAND-ID
   utilReplaceString(pScriptFile->szDescription, TEXT(""));
   utilReplaceString(pScriptFile->szCommandID, TEXT(""));
   
   // Initialise versions
   pScriptFile->iVersion     = 1;
   pScriptFile->eGameVersion = getAppPreferences()->eGameVersion;

   // [TEXT] Copy text, if any
   if (szText)
      pScriptFile->szInputBuffer = utilDuplicateSimpleString(szText);
   
   // Return new object
   return pScriptFile;
}


/// Function name  : deleteScriptFile
// Description     : Deletes a ScriptFile and it's contents
//
// SCRIPT_FILE*  &pScriptFile : [in] ScriptFile to delete
//
BearScriptAPI
VOID   deleteScriptFile(SCRIPT_FILE*  &pScriptFile)
{
   // Delete I/O buffers
   deleteGameFileIOBuffers(pScriptFile);
   
   // Delete generator
   if (pScriptFile->pGenerator)
      deleteScriptGenerator(pScriptFile->pGenerator);
      
   // Delete translator
   if (pScriptFile->pTranslator)
      deleteScriptTranslator(pScriptFile->pTranslator);

   // Delete ScriptName, Filename and description
   utilSafeDeleteString(pScriptFile->szScriptname);
   utilSafeDeleteString(pScriptFile->szDescription);
   utilSafeDeleteString(pScriptFile->szCommandID);

   // Delete Argument tree
   deleteAVLTree(pScriptFile->pArgumentTreeByID);   

   // Delete calling object
   utilDeleteObject(pScriptFile);
}



/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                           HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : getScriptFileArgumentCount
// Description     : Returns the number of ARGUMENTS in a ScriptFile
// 
// CONST SCRIPT_FILE*  pScriptFile   : [in] ScriptFile to query
// 
// Return Value   : Number of ARGUMENTS contained in the ScriptFile
// 
BearScriptAPI
UINT  getScriptFileArgumentCount(CONST SCRIPT_FILE*  pScriptFile)
{
   return getTreeNodeCount(pScriptFile->pArgumentTreeByID);
}


/// Function name  : identifyScriptName
// Description     : Retrieve the name of the script from a ScriptFile.
// 
// CONST SCRIPT_FILE*  pScriptFile : [in] ScriptFile containing either a filename or a scriptname
// 
// Return Value   : Pointer to the name of the string. This must not be deleted.
// 
BearScriptAPI
CONST TCHAR*  identifyScriptName(CONST SCRIPT_FILE*  pScriptFile)
{
   // [CHECK] Ensure it has either a path or a name
   ASSERT(lstrlen(pScriptFile->szFullPath) OR lstrlen(pScriptFile->szScriptname));

   // Use name for preference, otherwise extract the name from the full path
   if (lstrlen(pScriptFile->szScriptname))
      return pScriptFile->szScriptname;
   else
      return PathFindFileName(pScriptFile->szFullPath);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : appendArgumentToScriptFile
// Description     : Add a new ARGUMENT to a ScriptFile's ARGUMENT list
// 
// SCRIPT_FILE*     pScriptFile : [in] ScriptFile to be appened
// CONST ARGUMENT*  pArgument   : [in] ARGUMENT to append to ScriptFile's ARGUMENT list
// 
BearScriptAPI
VOID  appendArgumentToScriptFile(SCRIPT_FILE*  pScriptFile, CONST ARGUMENT*  pArgument)
{
   // Insert into Argument tree
   insertObjectIntoAVLTree(pScriptFile->pArgumentTreeByID, (LPARAM)pArgument);

   // Re-Index Argument tree
   performAVLTreeIndexing(pScriptFile->pArgumentTreeByID);
}


/// Function name  : findArgumentInScriptFileByIndex
// Description     : Retrieve an ARGUMENT from a ScriptFile's Argument list by it's index
// 
// CONST SCRIPT_FILE*  pScriptFile : [in]  ScriptFile to search
// CONST UINT          iIndex      : [in]  Zero based index of the argument to retrieve
// ARGUMENT*          &pOutput     : [out] Argument if found, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BearScriptAPI
BOOL  findArgumentInScriptFileByIndex(CONST SCRIPT_FILE*  pScriptFile, CONST UINT  iIndex, ARGUMENT*  &pOutput)
{
   // Prepare
   pOutput = NULL;

   // Search ARGUMENT tree
   return findObjectInAVLTreeByIndex(pScriptFile->pArgumentTreeByID, iIndex, (LPARAM&)pOutput);
}

/// Function name  : generateScriptFileArgumentList
// Description     : Flattens script file arguments tree into an ordered list
// 
// CONST SCRIPT_FILE*  pScriptFile   : [in] Script file
// 
// Return Value   : New list, you are responsible for destroying it
// 
LIST*  generateScriptFileArgumentList(CONST SCRIPT_FILE*  pScriptFile)
{
   LIST*  pList = createList(NULL);
   ARGUMENT*  pArgument;

   // Flatten tree into a list
   for (UINT i = 0; findObjectInAVLTreeByIndex(pScriptFile->pArgumentTreeByID, i, (LPARAM&)pArgument); i++)
      appendObjectToList(pList, (LPARAM)pArgument);

   // Return list
   return pList;
}

/// Function name  : generateScriptFileArgumentTree
// Description     : Generates a script file tree from an ordered list of arguments
// 
// LIST*  pArgumentList : [in] Ordered argument list
// 
// Return Value   : New tree, you are responsible for destroying it
// 
AVL_TREE*  generateScriptFileArgumentTree(LIST*  pArgumentList)
{
   AVL_TREE*  pTree = createArgumentTreeByID();
   ARGUMENT*  pArgument;

   // Populate tree
   for (UINT i = 0; findListObjectByIndex(pArgumentList, i, (LPARAM&)pArgument); i++)
   {
      pArgument->iID = i;
      insertObjectIntoAVLTree(pTree, (LPARAM)pArgument);
   }

   // Return
   return pTree;
}

/// Function name  : isArgumentInScriptFile
// Description     : Checks whether a variable name is an argument in a ScriptFile
// 
// CONST SCRIPT_FILE*  pScriptFile    : [in] ScriptFile
// CONST TCHAR*        szVariableName : [in] Name of variable, with or without $ operator
// 
// Return Value   : TRUE if found, otherwise FALSE
// 
BearScriptAPI
BOOL  isArgumentInScriptFile(CONST SCRIPT_FILE*  pScriptFile, CONST TCHAR*  szVariableName)
{
   CONST TCHAR*  szRawName;      // Name of variable without $ operator
   ARGUMENT*     pArgument;      // Argument iterator
   BOOL          bResult;        // Result

   // Prepare
   bResult = FALSE;

   // Identify name without $ operator
   szRawName = (szVariableName[0] == '$' ? &szVariableName[1] : &szVariableName[0]);

   /// Iterate through arguments
   for (UINT iIndex = 0; !bResult AND findArgumentInScriptFileByIndex(pScriptFile, iIndex, pArgument); iIndex++)
   {
      // [CHECK] Is this a match?
      if (utilCompareStringVariables(szRawName, pArgument->szName))
         /// [FOUND] Return TRUE
         bResult = TRUE;
   }

   // Return result
   return bResult;
}

/// Function name  : removeArgumentFromScriptFileByIndex
// Description     : Delete an ARGUMENT from a ScriptFile's argument list, by index
// 
// CONST SCRIPT_FILE*  pScriptFile : [in] ScriptFile containing the argument to delete
// CONST UINT          iIndex      : [in] Zero based index of the argument to delete
// 
// Return Value   : TRUE if argument was deleted, FALSE if it was not found
// 
BearScriptAPI
BOOL  removeArgumentFromScriptFileByIndex(CONST SCRIPT_FILE*  pScriptFile, CONST UINT  iIndex)
{
   BOOL       bResult;        // Operation result

   // Attempt to remove object from tree
   if (bResult = destroyObjectInAVLTreeByIndex(pScriptFile->pArgumentTreeByID, iIndex))
      // [SUCCESS] Re-index tree
      performAVLTreeIndexing(pScriptFile->pArgumentTreeByID);

   // Return result
   return bResult;
}


/// Function name  : replaceScriptFileArguments
// Description     : Replaces the arguments tree of a script file
// 
// SCRIPT_FILE*  pScriptFile   : [in] Script file
// AVL_TREE*  pNewArguments   : [in] New tree
// 
VOID  replaceScriptFileArguments(SCRIPT_FILE*  pScriptFile, AVL_TREE*  pNewArguments)
{
   // Delete tree but not arguments
   pScriptFile->pArgumentTreeByID->pfnDeleteNode = NULL;
   deleteAVLTree(pScriptFile->pArgumentTreeByID);

   // Replace + re-index
   pScriptFile->pArgumentTreeByID = pNewArguments;
   performAVLTreeIndexing(pScriptFile->pArgumentTreeByID);
}


/// Function name  : findArgumentInScriptFileByIndex
// Description     : Retrieve an ARGUMENT from a ScriptFile's Argument list by it's index
// 
// CONST SCRIPT_FILE*  pScriptFile : [in]  ScriptFile to search
// CONST UINT          iIndex      : [in]  Zero based index of the argument to retrieve
// ARGUMENT*          &pOutput     : [out] Argument if found, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BearScriptAPI
BOOL  reorderScriptFileArgument(SCRIPT_FILE*  pScriptFile, CONST UINT  iIndex, CONST BOOL  iOffset)
{
   LIST_ITEM* pItem;
   LIST*      pList;

   // Validate index
   if (iOffset == 1 && (iIndex >= getScriptFileArgumentCount(pScriptFile)-1))
      return FALSE;

   // Validate index
   else if (iOffset == -1 && (iIndex == 0 || iIndex >= getScriptFileArgumentCount(pScriptFile)))
      return FALSE;

   // Perform re-ordering
   pList = generateScriptFileArgumentList(pScriptFile);

   // Remove/Re-insert object at different index
   removeItemFromListByIndex(pList, iIndex, pItem);
   insertObjectIntoListByIndex(pList, iIndex + iOffset, pItem->pData);

   // Re-create arguments tree
   replaceScriptFileArguments(pScriptFile, generateScriptFileArgumentTree(pList));

   // Cleanup
   deleteListItem(pList, pItem, FALSE);
   deleteList(pList);
   return TRUE;
}

/// Function name  : setScriptNameFromPath
// Description     : Updates the ScriptName to match the file path
// 
// SCRIPT_FILE*  pScriptFile : [in/out] ScriptFile
// 
VOID   setScriptNameFromPath(SCRIPT_FILE*  pScriptFile)
{
   // Use FILE-NAME (without extension) for the SCRIPT-NAME
   utilReplaceString(pScriptFile->szScriptname, PathFindFileName(pScriptFile->szFullPath));
   PathRemoveExtension(pScriptFile->szScriptname);
}


/// Function name  : setPathFromScriptName
// Description     : Renames the file portion of a ScriptFile path to reflect the script name
// 
// SCRIPT_FILE*  pScriptFile     : [in/out] ScriptFile
// 
VOID   setPathFromScriptName(SCRIPT_FILE*  pScriptFile)
{
   TCHAR  *szFolder,                // Folder portion of the path. If not empty, guaranteed to have a trailing backslash
          *szOriginalExtension;     // Current filename extension

   // Extract folder and original extension
   szFolder = utilDuplicateFolderPath(pScriptFile->szFullPath);
   szOriginalExtension = utilDuplicateSimpleString(PathFindExtension(pScriptFile->szFullPath));

   /// Set new path
   setGameFilePath(pScriptFile, TEXT("%s%s%s"), szFolder, pScriptFile->szScriptname, szOriginalExtension);

   // Cleanup
   utilDeleteStrings(szFolder, szOriginalExtension);
}


/// Function name  : setScriptName
// Description     : Adjusts a ScriptFile's script name
// 
// SCRIPT_FILE*  pScriptFile  : [in/out] ScriptFile
// CONST TCHAR*  szScriptName : [in]     New script name
// 
VOID   setScriptName(SCRIPT_FILE*  pScriptFile, CONST TCHAR*  szScriptName)
{
   // Update SCRIPT-NAME
   utilReplaceString(pScriptFile->szScriptname, szScriptName);
}


/// Function name  : verifyScriptFileArgumentName
// Description     : Verifies a ScriptFile argument name is unique and contains no unsupported symbols
// 
// CONST SCRIPT_FILE*  pScriptFile      : [in]           ScriptFile containing the Arguments to check
// CONST TCHAR*        szArgumentName   : [in]           Name to verify
// CONST ARGUMENT*     pExcludeArgument : [in][optional] ScriptFile Argument to exclude from the check (Used when updating the name of an existing argument)
// 
// Return Value   : TRUE if name is unique and contains no unsupported symbols, otherwise FALSE
// 
BearScriptAPI 
BOOL  verifyScriptFileArgumentName(CONST SCRIPT_FILE*  pScriptFile, CONST TCHAR*  szArgumentName, CONST ARGUMENT*  pExcludeArgument)
{
   ARGUMENT*  pArgument;      // ScriptFile ARGUMENT iterator
   BOOL       bResult;        // Verification result

   // [CHECK] Ensure name is not empty or has perculiar symbols
   bResult = lstrlen(szArgumentName) AND !StrPBrk(szArgumentName, TEXT("|!`¬£$%^&*()[]{}-_=+:;@'#~<>,/? \"\\"));

   /// Iterate through ScriptFile's ARGUMENTs
   for (UINT  iArgument = 0; bResult AND findArgumentInScriptFileByIndex(pScriptFile, iArgument, pArgument); iArgument++)
   {
      // [CHECK] Exclude the input Argument (if any) from checking
      if (pArgument == pExcludeArgument)
         continue;

      // [CHECK] Ensure name is unique
      bResult = !utilCompareStringVariables(szArgumentName, pArgument->szName);
   }

   // Return result
   return bResult;
}

