//
// Game Files.cpp : Functions regarding the loading of game data files
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       CONSTANTS / GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////



/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       CREATION / DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createGameFile
// Description     : Create an empty GameFile
// 
// Return Value   : New GameFile object, you are responsible for destroying it
// 
BearScriptAPI
GAME_FILE*   createGameFile()
{
   GAME_FILE*  pGameFile = utilCreateEmptyObject(GAME_FILE);

   // Return object
   return pGameFile;
}


/// Function name  : createGameFileFromAppPath
// Description     : Create a new GameFile object
// 
// CONST TCHAR*  szFileName : [in] Filename
//
// Return Value   : New GameFile object, you are responsible for destroying it
// 
BearScriptAPI
GAME_FILE*   createGameFileFromAppPath(CONST TCHAR*  szFileName)
{
   GAME_FILE*  pGameFile;
   TCHAR*      szCustomPath;

   // Prepare
   pGameFile = utilCreateEmptyObject(GAME_FILE);

   /// Set path
   setGameFilePath(pGameFile, szCustomPath = utilGenerateAppFilePath(szFileName));

   // Return object
   utilDeleteString(szCustomPath);
   return pGameFile;
}


/// Function name  : createGameFileFromSubPath
// Description     : Create a new GameFile object
// 
// CONST TCHAR*  szSubPath : [in] Subpath
//
// Return Value   : New GameFile object, you are responsible for destroying it
// 
BearScriptAPI
GAME_FILE*   createGameFileFromSubPath(CONST TCHAR*  szSubPath)
{
   GAME_FILE*  pGameFile = utilCreateEmptyObject(GAME_FILE);

   /// Generate full path from sub-path
   setGameFilePath(pGameFile, TEXT("%s%s"), getFileSystem()->szGameFolder, szSubPath);

   // Return object
   return pGameFile;
}


/// Function name  : deleteGameFile
// Description     : Delete an existing GameFile object
// 
// GAME_FILE*  pGameFile   : [in] The GameFile object to delete
// 
BearScriptAPI
VOID  deleteGameFile(GAME_FILE*  &pGameFile)
{
   // Delete string buffers
   utilSafeDeleteStrings(pGameFile->szInputBuffer, pGameFile->szOutputBuffer);
   
   // Delete calling object
   utilDeleteObject(pGameFile);
}


/// Function name  : deleteGameFileIOBuffers
// Description     : Delete the file input and output buffers within a GameFile object but not the object itself.
// 
// GAME_FILE*  pGameFile : [in] GameFile object who's input and output buffers will be destroyed
///                                                   NOTE: This does *NOT* delete the calling object
BearScriptAPI 
VOID      deleteGameFileIOBuffers(GAME_FILE*  pGameFile)
{
   // Buffers
   utilSafeDeleteStrings(pGameFile->szInputBuffer, pGameFile->szOutputBuffer);

   // File sizes
   pGameFile->iInputSize  = NULL;
   pGameFile->iOutputSize = NULL;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                             HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : hasInputData
// Description     : Determines whether a GameFile has data in it's input buffer
// 
// CONST GAME_FILE*  pGameFile : [in] GameFile
// 
// Return Value   : TRUE/FALSE
// 
BearScriptAPI
BOOL  hasInputData(CONST GAME_FILE*  pGameFile)
{
   return (pGameFile->szInputBuffer AND pGameFile->szInputBuffer[0] AND pGameFile->iInputSize);
}


/// Function name  : hasOutputData
// Description     : Determines whether a GameFile has data in it's output buffer
// 
// CONST GAME_FILE*  pGameFile : [in] GameFile
// 
// Return Value   : TRUE/FALSE
// 
BearScriptAPI
BOOL  hasOutputData(CONST GAME_FILE*  pGameFile)
{
   return (pGameFile->szOutputBuffer AND pGameFile->szOutputBuffer[0] AND pGameFile->iOutputSize);
}


/// Function name  : identifyGameFileFilename
// Description     : Retrieve the filename from a GameFile's path
// 
// CONST GAME_FILE*  pGameFile : [in] GameFile to query
// 
// Return Value   : Pointer to the name of the string. This must not be deleted.
// 
BearScriptAPI
CONST TCHAR*  identifyGameFileFilename(CONST GAME_FILE*  pGameFile)
{
   return (pGameFile ? PathFindFileName(pGameFile->szFullPath) : TEXT("Unknown"));
}


/// Function name  : identifyGameFileFolder
// Description     : Retrieve the folder from a GameFile's path
// 
// CONST GAME_FILE*  pGameFile : [in]  GameFile to query
// TCHAR*           &szOutput  : [out] New string containing the path if any, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE if path only contained a filename
// 
BearScriptAPI
BOOL  identifyGameFileFolder(CONST GAME_FILE*  pGameFile, TCHAR*  &szOutput)
{
   CONST TCHAR*  szFilename;

   // Prepare
   szOutput = utilCreateEmptyPath();

   // Get filename portion
   szFilename = PathFindFileName(pGameFile->szFullPath);
   
   /// Extract non-filename portion  (ie. folder)
   StringCchCopyN(szOutput, MAX_PATH, pGameFile->szFullPath, szFilename - pGameFile->szFullPath);

   /// [CHECK] Does filepath contain a folder?
   if (!lstrlen(szOutput))
      // [FAILED] Set result to NULL
      utilDeleteString(szOutput);

   // Return TRUE if found
   return (szOutput != NULL);
}


/// Function name  : setGameFilePath
// Description     : Sets the path of a game file
// 
// GAME_FILE*    pGameFile : [in/out] GameFile
// CONST TCHAR*  szNewPath : [in]     New path
// 
BearScriptAPI
VOID  setGameFilePath(GAME_FILE*  pGameFile, CONST TCHAR*  szNewPath, ...)
{
   va_list  pArguments = utilGetFirstVariableArgument(&szNewPath);

   // Update path
   StringCchVPrintf(pGameFile->szFullPath, MAX_PATH, szNewPath, pArguments);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : loadGameFileFromResource
// Description     : Fills a GameFile object from a binary resource
// 
// HINSTANCE     hInstance      : [in]     Handle of the module containing the resource
// CONST TCHAR*  szResourceName : [in]     Name of the resource
// GAME_FILE*    pOutput        : [in/out] Empty GameFile object to fill
// 
// Return Value   : Number of characters copied
// 
UINT   loadGameFileFromResource(HINSTANCE  hInstance, CONST TCHAR*  szResourceName, GAME_FILE*  pOutput)
{
   // [VERBOSE]
   //VERBOSE("Loading resource %s", szResourceName);

   // [CHECK] Ensure GameFile is empty
   ASSERT(!hasInputData(pOutput));

   // Create new input buffer from resource
   pOutput->iInputSize = utilLoadResourceFile(hInstance, szResourceName, pOutput->szInputBuffer);

   // [CHECK] Ensure resource was loaded successfully
   ERROR_CHECK("loading binary resource", pOutput->iInputSize);

   // Set properties
   pOutput->bResourceBased = TRUE;
   setGameFilePath(pOutput, szResourceName);

   // Return number of characters loaded
   return pOutput->iInputSize;
}



