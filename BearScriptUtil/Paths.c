//
// Paths.cpp : Utility functions regarding file paths
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

/// Function name  : utilDuplicatePath
// Description     : Duplicates a path string, ensuring the output is of MAX_PATH chars
// 
// CONST TCHAR*  szSource   : [in] Path to duplicate
// 
// Return Value   : New Path of length MAX_PATH, you are responsible for destroying it
// 
UtilityAPI
TCHAR*  utilDuplicatePath(CONST TCHAR*  szSource)
{
   TCHAR*  szOutput; //

   // Prepare
   szOutput = utilCreateEmptyPath();

   // Copy strings
   StringCchCopy(szOutput, MAX_PATH, szSource);

   // Return result
   return szOutput;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : utilDuplicateFolderPath
// Description     : Generates a new path containing only the folder portion of the input path.
// 
// CONST TCHAR*  szFullPath : [in] Full filepath containing a folder and a filename
// 
// Return Value   : New path containing folder portion of input path, you are responsible for destroying it
///                                       NOTE: Path is MAX_PATH and guaranteed to have a trailing backslash 
///                                             If input had no folder, only a filename, output will be empty
// 
UtilityAPI
TCHAR*  utilDuplicateFolderPath(CONST TCHAR*  szFullPath)
{
   TCHAR*  szOutput;

   // Copy input path
   szOutput = utilDuplicatePath(szFullPath);

   // Null terminate after last backslash
   PathFindFileName(szOutput)[0] = NULL;

   // Return folder portion
   return szOutput;
}


/// Function name  : utilGenerateAppFilePath
// Description     : Generates the full path of a file in the folder containing the app executable
// 
// CONST TCHAR*  szFileName : [in] Filename
// 
// Return Value   : New path containing full path to filename
///                                 String is always MAX_PATH chars and you are responsible for destroying it
// 
UtilityAPI
TCHAR*  utilGenerateAppFilePath(CONST TCHAR*  szFileName)
{
   TCHAR   *szModulePath,  // Full path of the EXE
           *szOutput;      // Path being generated

   // Prpeare
   szModulePath = utilCreateEmptyPath();
   GetModuleFileName(NULL, szModulePath, MAX_PATH);

   // Strip module name and append FileName
   szOutput = utilDuplicateFolderPath(szModulePath);
   PathAppend(szOutput, szFileName);

   // Cleanup and return path
   utilDeleteString(szModulePath);
   return szOutput;
}


/// Function name  : utilRenameFilePath
// Description     : Duplicates a given path, but changes the filename
// 
// CONST TCHAR*  szFullPath    : [in] Full file path containing folder and file
// CONST TCHAR*  szNewFileName : [in] New filename
// 
// Return Value   : New path containing the path of 'szFullPath' but the filename of 'szNewFileName'. 
///                                 String is always MAX_PATH chars and you are responsible for destroying it
// 
UtilityAPI
TCHAR*  utilRenameFilePath(CONST TCHAR*  szFullPath, CONST TCHAR*  szNewFileName)
{
   TCHAR*  szOutput;    // Operation result

   // Create new path containing just the folder name
   szOutput = utilDuplicateFolderPath(szFullPath);

   // Append new filename
   StringCchCat(szOutput, MAX_PATH, szNewFileName);

   // Return result
   return szOutput;
}
