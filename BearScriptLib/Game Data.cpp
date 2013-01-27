//
// Game Data.cpp : Functions for creating/destroying the game data
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       CONSTANTS / GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// Number of game data files
CONST UINT            iGameDataFileCount   = 26;     // Total number of data files

// The data files for generating ObjectNames and MediaItems
//
CONST GAME_DATA_FILE  oGameDataFiles[iGameDataFileCount] = 
{      
   // T-Files
   TEXT("addon\\types\\TDocks.pck"),      TEXT(".txt"),   MT_DOCK,            DT_STATION,
   TEXT("addon\\types\\TFactories.pck"),  TEXT(".txt"),   MT_FACTORY,         DT_STATION,
   TEXT("addon\\types\\TShips.pck"),      TEXT(".txt"),   MT_SHIP,            DT_SHIP,
   TEXT("addon\\types\\TLaser.pck"),      TEXT(".txt"),   MT_WARE_LASER,      DT_WARE,
   TEXT("addon\\types\\TShields.pck"),    TEXT(".txt"),   MT_WARE_SHIELD,     DT_WARE,
   TEXT("addon\\types\\TMissiles.pck"),   TEXT(".txt"),   MT_WARE_MISSILE,    DT_WARE,
   TEXT("addon\\types\\TWareE.pck"),      TEXT(".txt"),   MT_WARE_ENERGY,     DT_WARE,
   TEXT("addon\\types\\TWareN.pck"),      TEXT(".txt"),   MT_WARE_NATURAL,    DT_WARE,
   TEXT("addon\\types\\TWareB.pck"),      TEXT(".txt"),   MT_WARE_BIOLOGICAL, DT_WARE,
   TEXT("addon\\types\\TWareF.pck"),      TEXT(".txt"),   MT_WARE_FOODSTUFF,  DT_WARE,
   TEXT("addon\\types\\TWareM.pck"),      TEXT(".txt"),   MT_WARE_MINERAL,    DT_WARE,
   TEXT("addon\\types\\TWareT.pck"),      TEXT(".txt"),   MT_WARE_TECHNOLOGY, DT_WARE,

   // Sound/Video files
   TEXT("addon\\types\\sounds.pck"),      TEXT(".txt"),   MT_NONE,            DT_NULL,    
   TEXT("addon\\types\\videos.pck"),      TEXT(".txt"),   MT_NONE,            DT_NULL, 

   // Speech file
   TEXT("addon\\mov\\000%02u.pck"),       TEXT(".xml"),   MT_NONE,            DT_NULL, 

   // Language files
   TEXT("addon\\t\\0001-L0%02u.pck"),     TEXT(".xml"),   MT_NONE,            DT_NULL,       // GV_TERRAN_CONFLICT, GV_ALBION_PRELUDE
   TEXT("addon\\t\\%02u0001.pck"),        TEXT(".xml"),   MT_NONE,            DT_NULL,       // GV_REUNION

   // Folders
   TEXT("addon\\t\\"),                    NULL,           MT_NONE,            DT_NULL,
   TEXT("addon\\director\\"),             NULL,           MT_NONE,            DT_NULL,
   TEXT("addon\\scripts\\"),              NULL,           MT_NONE,            DT_NULL,

   // Catalogue files
   TEXT("addon\\%02u.cat"),               NULL,           MT_NONE,            DT_NULL,
   TEXT("addon\\%02u.dat"),               NULL,           MT_NONE,            DT_NULL,

   // Executables
   TEXT("X2.exe"),                        NULL,           MT_NONE,            DT_NULL,
   TEXT("X3.exe"),                        NULL,           MT_NONE,            DT_NULL,
   TEXT("X3TC.exe"),                      NULL,           MT_NONE,            DT_NULL,
   TEXT("X3AP.exe"),                      NULL,           MT_NONE,            DT_NULL
};


/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       CREATION / DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : destroyGameData
// Description     : Destroy any loaded GameData (if it exists) and the VirtualFileSystem, if that exists.
// 
BearScriptAPI
VOID  destroyGameData()
{
   FILE_SYSTEM*  pFileSystem;

   /// [COMMAND SYNTAX (by ID)]
   if (getGameData()->pCommandTreeByID)
      deleteAVLTree(getGameData()->pCommandTreeByID);
   
   // [COMMAND SYNTAX (by Hash)]
   if (getGameData()->pCommandTreeByHash)
      deleteAVLTree(getGameData()->pCommandTreeByHash);

   /// [GAME OBJECTS (by Group)]
   if (getGameData()->pGameObjectsByMainType)
      deleteAVLTree(getGameData()->pGameObjectsByMainType);

   // [GAME OBJECT (by Name)]
   if (getGameData()->pGameObjectsByText)
      deleteAVLTree(getGameData()->pGameObjectsByText);

   /// [SCRIPT OBJECTS (by Group)]
   if (getGameData()->pScriptObjectsByGroup)
      deleteAVLTree(getGameData()->pScriptObjectsByGroup);

   // [SCRIPT OBJECTS (by Name)]
   if (getGameData()->pScriptObjectsByText)
      deleteAVLTree(getGameData()->pScriptObjectsByText);

   /// [GAME STRINGS (by ID)]
   if (getGameData()->pGameStringsByID)
      deleteAVLTree(getGameData()->pGameStringsByID);

   // [GAME STRING PAGES (by ID)]
   if (getGameData()->pGamePagesByID)
      deleteAVLTree(getGameData()->pGamePagesByID);

   /// [MEDIA ITEMS (by ID)]
   if (getGameData()->pMediaItemsByID)
      deleteAVLTree(getGameData()->pMediaItemsByID);

   // [MEDIA PAGES (by ID)]
   if (getGameData()->pMediaPagesByGroup)
      deleteAVLTree(getGameData()->pMediaPagesByGroup);

   /// [VIRTUAL FILE SYSTEM]
   if (pFileSystem = getFileSystem())
   {
      deleteFileSystem(pFileSystem);
      setFileSystem(NULL);
   }

   // Zero any remaining handles
   utilZeroObject(getGameData(), GAME_DATA);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                             HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : convertAppLanguageToGameLanguage
// Description     : Converts an language ID used by the application into the value used by the game
// 
// CONST APP_LANGUAGE  eLanguage   : [in] Application language ID
// 
// Return Value   : Game language ID
// 
BearScriptAPI
GAME_LANGUAGE  convertAppLanguageToGameLanguage(CONST APP_LANGUAGE  eLanguage)
{
   GAME_LANGUAGE  eOutput;      // Operation result

   // Examine language
   switch (eLanguage)
   {
   default:  VERBOSE(BUG "Unknown AppLanguage"); // Fall through
   case AL_ENGLISH:   eOutput = GL_ENGLISH;  break;
   case AL_FRENCH:    eOutput = GL_FRENCH;   break;
   case AL_GERMAN:    eOutput = GL_GERMAN;   break;
   case AL_ITALIAN:   eOutput = GL_ITALIAN;  break;
   case AL_POLISH:    eOutput = GL_POLISH;   break;
   case AL_RUSSIAN:   eOutput = GL_RUSSIAN;  break;
   case AL_SPANISH:   eOutput = GL_SPANISH;  break;
   }

   // Return result
   return eOutput;
}

/// Function name  : convertInterfaceLanguageToAppLanguage
// Description     : Converts an App language to an interface language
// 
// CONST INTERFACE_LANGUAGE  eLanguage : [in] App language ID 
// 
// Return Value   : interface language ID
// 
BearScriptAPI
INTERFACE_LANGUAGE  convertAppLanguageToInterfaceLanguage(CONST APP_LANGUAGE  eLanguage)
{
   INTERFACE_LANGUAGE  eOutput;      // Operation result

   // Examine language
   switch (eLanguage)
   {
   default:  VERBOSE(BUG "Unknown AppLanguage"); // Fall through
   case AL_ENGLISH:   eOutput = IL_ENGLISH;  break;
   case AL_GERMAN:    eOutput = IL_GERMAN;   break;
   //case IL_RUSSIAN:   eOutput = AL_RUSSIAN;  break;
   }

   // Return result
   return eOutput;
}

/// Function name  : convertGameLanguageToAppLanguage
// Description     : Converts an language ID used by the game into the value used by the application
// 
// CONST GAME_LANGUAGE  eLanguage : [in] Game language ID
// 
// Return Value   : Application language ID
// 
BearScriptAPI
APP_LANGUAGE  convertGameLanguageToAppLanguage(CONST GAME_LANGUAGE  eLanguage)
{
   APP_LANGUAGE  eOutput;      // Operation result

   // Examine language
   switch (eLanguage)
   {
   default:  VERBOSE(BUG "Unknown GameLanguage"); // Fall through
   case GL_ENGLISH:   eOutput = AL_ENGLISH;  break;
   case GL_FRENCH:    eOutput = AL_FRENCH;   break;
   case GL_GERMAN:    eOutput = AL_GERMAN;   break;
   case GL_ITALIAN:   eOutput = AL_ITALIAN;  break;
   case GL_POLISH:    eOutput = AL_POLISH;   break;
   case GL_RUSSIAN:   eOutput = AL_RUSSIAN;  break;
   case GL_SPANISH:   eOutput = AL_SPANISH;  break;
   }

   // Return result
   return eOutput;
}



/// Function name  : convertInterfaceLanguageToAppLanguage
// Description     : Converts an interface language (commonly a ComboBox item index) into the application language ID
// 
// CONST INTERFACE_LANGUAGE  eLanguage : [in] Interface language ID  (commonly a ComboBox item index)
// 
// Return Value   : Application language ID
// 
BearScriptAPI
APP_LANGUAGE  convertInterfaceLanguageToAppLanguage(CONST INTERFACE_LANGUAGE  eLanguage)
{
   APP_LANGUAGE  eOutput;      // Operation result

   // Examine language
   switch (eLanguage)
   {
   default:  VERBOSE(BUG "Unknown InterfaceLanguage"); // Fall through
   case IL_ENGLISH:   eOutput = AL_ENGLISH;  break;
   case IL_GERMAN:    eOutput = AL_GERMAN;   break;
   //case IL_RUSSIAN:   eOutput = AL_RUSSIAN;  break;
   }

   // Return result
   return eOutput;
}


/// Function name  : findGameDataFileByIndex
// Description     : Finds the properties of a file used to define game data
// 
// CONST UINT        iIndex   : [in]  Zero based file index, as defined by their order in the oGameDataFiles array
// GAME_DATA_FILE*  &pOutput  : [out] File properties if found, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BOOL  findGameDataFileByIndex(CONST UINT  iIndex, CONST GAME_DATA_FILE*  &pOutput)
{
   // Prepare
   pOutput = NULL;

   // Lookup game data file object
   if (iIndex < iGameDataFileCount)
      pOutput = &oGameDataFiles[iIndex];

   // Return TRUE if found
   return (pOutput != NULL); 
}


/// Function name  : findGameDataFileSubPathByID
// Description     : Finds the sub-path of a game data file
// 
// CONST UINT          eGameDataFile : [in] Data file index
// CONST GAME_VERSION* eGameVersion  : [in] Game version
// 
// Return Value   : Subpath of the specified game data file
// 
CONST TCHAR*  findGameDataFileSubPathByID(CONST GAME_DATA_FILE_INDEX  eGameDataFile, CONST GAME_VERSION  eGameVersion)
{
   GAME_DATA_FILE_INDEX   eActualIndex;
   CONST GAME_DATA_FILE*  pFileInfo;
   CONST TCHAR*           szOutput;

   // [CHECK] This function can only be used for data file, catalogue and folder paths
   ASSERT(eGameDataFile <= GFI_CATALOGUE_DATA);       // [FIX] BUG:1028 'Unable to resolve the game data version even executable is present'

   // Prepare
   eActualIndex = eGameDataFile;
   szOutput     = NULL;

   // [CHECK] Automatically return the Reunion version of the MasterLanguage path, if appropriate
   if (eGameDataFile == GFI_MASTER_LANGUAGE AND eGameVersion <= GV_REUNION)
      eActualIndex = GFI_MASTER_LANGUAGE_REUNION;

   /// Lookup GameDataFile and retrieve sub-path
   if (findGameDataFileByIndex(eActualIndex, pFileInfo))
      // [SUCCESS] Strip the 'addon' folder from older versions
      szOutput = (eGameVersion == GV_ALBION_PRELUDE ? pFileInfo->szSubPath : &pFileInfo->szSubPath[6]);

   // Return result
   return szOutput;
}


/// Function name  : findGameObjectDataFileByMainType
// Description     : Finds the properties of a T-File
// 
// CONST UINT        iIndex   : [in]  Zero based T-File index, as defined by their order in the oGameDataFiles array
// GAME_DATA_FILE*  &pOutput  : [out] File properties if found, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BOOL  findGameObjectDataFileByMainType(CONST MAIN_TYPE  eMainType, CONST GAME_DATA_FILE*  &pOutput)
{
   // [CHECK] Does index relate to a T-File?
   ASSERT(eMainType >= MT_DOCK AND eMainType <= MT_WARE_TECHNOLOGY);

   // Lookup TFile info
   return findGameDataFileByIndex((eMainType - MT_DOCK), pOutput);
}


/// Function name  : findGameObjectDataFileSubPathByMainType
// Description     : Finds the properties of a T-File
// 
// CONST UINT        iIndex   : [in]  Zero based T-File index, as defined by their order in the oGameDataFiles array
// GAME_DATA_FILE*  &pOutput  : [out] File properties if found, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
CONST TCHAR*  findGameObjectDataFileSubPathByMainType(CONST MAIN_TYPE  eMainType, CONST GAME_VERSION  eGameVersion)
{
   // [CHECK] Does index relate to a T-File?
   ASSERT(eMainType >= MT_DOCK AND eMainType <= MT_WARE_TECHNOLOGY);

   // Lookup subpath
   return findGameDataFileSubPathByID((GAME_DATA_FILE_INDEX)(eMainType - MT_DOCK), eGameVersion);
}


/// Function name  : generateGameCatalogueFilePath
// Description     : Generates the path to a catalogue file
// 
// CONST GAME_DATA_FILE_INDEX  eGameDataFile : [in] Game data file index
// CONST TCHAR*                szGameFolder  : [in] Game folder
// CONST UINT                  iCatalogue    : [in] ID of the catalogue
// CONST GAME_VERSION          eGameVersion  : [in] Game version to indicate the correct path
// 
// Return Value   : New string containing the path to the desired catalogue
// 
TCHAR*  generateGameCatalogueFilePath(CONST GAME_DATA_FILE_INDEX  eGameDataFile, CONST TCHAR*  szGameFolder, CONST UINT  iCatalogue, CONST GAME_VERSION  eGameVersion)
{
   TCHAR*  szOutput;

   // [CHECK] Ensure file index is valid
   ASSERT(eGameDataFile == GFI_CATALOGUE OR eGameDataFile == GFI_CATALOGUE_DATA);

   // Prepare
   szOutput = utilCreateEmptyPath(); 

   // Prepend with game folder
   StringCchCopy(szOutput, MAX_PATH, szGameFolder);
   // Append and format catalogue path
   utilStringCchCatf(szOutput, MAX_PATH, findGameDataFileSubPathByID(eGameDataFile, eGameVersion), iCatalogue);

   // Return output
   return szOutput;
}


/// Function name  : generateGameDataFilePath
// Description     : Generates the full path to a T-File, Sound/VideoClip definitions or Folder path
// 
// CONST GAME_DATA_FILE_INDEX  eGameDataFile : [in] Game data file index
// CONST TCHAR*                szGameFolder  : [in] Game folder
// CONST GAME_VERSION          eGameVersion  : [in] Game version to indicate the correct path
// 
// Return Value   : New string containing the path to the desired catalogue
// 
BearScriptAPI
TCHAR*  generateGameDataFilePath(CONST GAME_DATA_FILE_INDEX  eGameDataFile, CONST TCHAR*  szGameFolder, CONST GAME_VERSION  eGameVersion)
{
   // [CHECK] Can only be used for T-Files, Sound/VideoClips and FolderPaths
   ASSERT(eGameDataFile <= GFI_SCRIPT_FOLDER AND eGameDataFile != GFI_SPEECH_CLIPS AND eGameDataFile != GFI_MASTER_LANGUAGE);

   // Generate full path
   return utilCreateStringf(MAX_PATH, TEXT("%s%s"), szGameFolder, findGameDataFileSubPathByID(eGameDataFile, eGameVersion));
}


/// Function name  : generateGameExecutableFilePath
// Description     : Generates the full-path to a game executable
// 
// CONST TCHAR*          szGameFolder  : [in] game folder
// CONST GAME_VERSION    eGameVersion  : [in] Game version to indicate the correct path
// 
// Return Value   : New string containing the path to the desired catalogue
// 
BearScriptAPI 
TCHAR*  generateGameExecutableFilePath(CONST TCHAR*  szGameFolder, CONST GAME_VERSION  eGameVersion)
{
   CONST TCHAR*  szExecutableName;

   // [CHECK] Ensure version is not UNKNOWN
   ASSERT(eGameVersion != GV_UNKNOWN);

   /// Examine GameVersion
   switch (eGameVersion)
   {
   case GV_THREAT:           szExecutableName = oGameDataFiles[GFI_THREAT].szSubPath;           break;      // [FIX] BUG:1028 'Unable to resolve the game data version even executable is present'
   case GV_REUNION:          szExecutableName = oGameDataFiles[GFI_REUNION].szSubPath;          break;
   case GV_TERRAN_CONFLICT:  szExecutableName = oGameDataFiles[GFI_TERRAN_CONFLICT].szSubPath;  break;
   case GV_ALBION_PRELUDE:   szExecutableName = oGameDataFiles[GFI_ALBION_PRELUDE].szSubPath;   break;
   }

   /// Generate and return output
   return utilCreateStringf(MAX_PATH, TEXT("%s%s"), szGameFolder, szExecutableName);
}


/// Function name  : generateGameLanguageFilePath
// Description     : Generates the full path to a language/speech data file
// 
// CONST GAME_DATA_FILE_INDEX  eGameDataFile : [in] Game data file index
// CONST TCHAR*                szGameFolder  : [in] game folder
// CONST GAME_VERSION          eGameVersion  : [in] Game version to indicate the correct path
// CONST GAME_LANGUAGE         eLanguage     : [in] Desired language
// 
// Return Value   : New string containing the path to the desired catalogue
// 
TCHAR*  generateGameLanguageFilePath(CONST GAME_DATA_FILE_INDEX  eGameDataFile, CONST TCHAR*  szGameFolder, CONST GAME_VERSION  eGameVersion, CONST GAME_LANGUAGE  eLanguage)
{
   GAME_DATA_FILE_INDEX   eActualIndex;
   TCHAR                 *szOutput,
                         *szSubPath;

   // [CHECK] Ensure file index is valid
   ASSERT(eGameDataFile == GFI_SPEECH_CLIPS OR eGameDataFile == GFI_MASTER_LANGUAGE);

   // Prepare
   eActualIndex = eGameDataFile;

   // [CHECK] Automatically return the Reunion version of the MasterLanguage path, if appropriate
   if (eGameDataFile == GFI_MASTER_LANGUAGE AND eGameVersion <= GV_REUNION)
      eActualIndex = GFI_MASTER_LANGUAGE_REUNION;

   // Generate sub-path
   switch (eGameDataFile)
   {
   case GFI_MASTER_LANGUAGE_REUNION:
   case GFI_MASTER_LANGUAGE:
   case GFI_SPEECH_CLIPS:
      // Generate subpath
      szSubPath = generateGameLanguageFileSubPath(eActualIndex, eGameVersion, eLanguage);

      // Generate full path
      szOutput = utilCreateStringf(MAX_PATH, TEXT("%s%s"), szGameFolder, szSubPath);

      // Cleanup
      utilDeleteString(szSubPath);
      break;
   }

   // Return path
   return szOutput;
}


/// Function name  : generateGameLanguageFileSubPath
// Description     : Generates the sub-path of a language/speech data file  (Can also be used for the language folder)
// 
// CONST GAME_DATA_FILE_INDEX  eGameDataFile : [in] Game data file index
// CONST GAME_VERSION          eGameVersion  : [in] Game version to indicate the correct path
// CONST GAME_LANGUAGE         eLanguage     : [in] Desired language
// 
// Return Value   : New string containing the path to the desired catalogue
// 
TCHAR*  generateGameLanguageFileSubPath(CONST GAME_DATA_FILE_INDEX  eGameDataFile, CONST GAME_VERSION  eGameVersion, CONST GAME_LANGUAGE  eLanguage)
{
   CONST GAME_DATA_FILE  *pFileInfo;
   GAME_DATA_FILE_INDEX   eActualIndex;
   TCHAR                 *szOutput;
   CONST TCHAR           *szSubPath;

   // [CHECK] 
   ASSERT(eGameDataFile >= GFI_SPEECH_CLIPS AND eGameDataFile <= GFI_SCRIPT_FOLDER);

   // Prepare
   eActualIndex = eGameDataFile;
   szOutput     = NULL;

   // [CHECK] Automatically return the Reunion version of the MasterLanguage path, if appropriate
   if (eGameDataFile == GFI_MASTER_LANGUAGE AND eGameVersion <= GV_REUNION)
      eActualIndex = GFI_MASTER_LANGUAGE_REUNION;

   /// Determine sub-path
   if (findGameDataFileByIndex(eActualIndex, pFileInfo))
   {
      // Strip the 'addon' folder from older versions
      szSubPath = (eGameVersion == GV_ALBION_PRELUDE ? pFileInfo->szSubPath : &pFileInfo->szSubPath[6]);
   
      /// Generate language specific string
      switch (eGameDataFile)
      {
      // [MASTER-LANGUAGE, SPEECH] "addon\\t\\00001-L0%2u.pck" or "addon\\mov\\000%2u.pck"
      case GFI_MASTER_LANGUAGE_REUNION:
      case GFI_MASTER_LANGUAGE:
      case GFI_SPEECH_CLIPS:
         szOutput = utilCreateStringf(MAX_PATH, szSubPath, eLanguage);
         break;

      // [LANGUAGE FOLDER] "addon\\t\\"
      case GFI_LANGUAGE_FOLDER:
         szOutput = utilCreateStringf(MAX_PATH, szSubPath);
         break;
      }
   }

   // Return path, if found
   return szOutput;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                            FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : calculateGameFolderState
// Description     : Determines whether the specified folder contains game data or not
// 
// CONST TCHAR*   szGameFolder : [in]  Folder to query (must contain a trailing backslash)
// GAME_VERSION&  eVersion     : [out] GameVersion if possible, otherwise GV_UNKNOWN
// 
// Return Value   : GFS_INVALID    : Folder contains no game data, eOutput is GV_UNKNOWN
//                  GFS_VALID      : Folder contains game data and eOutput identifies the game version
//                  GFS_INCOMPLETE : Folder contains game data but game version could not be determined. eOutput is GV_UNKNOWN
// 
BearScriptAPI
GAME_FOLDER_STATE  calculateGameFolderState(CONST TCHAR*  szGameFolder, GAME_VERSION&  eVersion)
{
   GAME_FOLDER_STATE  eResult;         // operation result
   UINT               iContentFlags;   // Flags indicating the contents of the input folder

   // [CHECK] Game folder must have a trailing backslash
   ASSERT(!lstrlen(szGameFolder) OR szGameFolder[lstrlen(szGameFolder)-1] == '\\');

   // Prepare
   eResult  = GFS_INVALID;
   eVersion = GV_UNKNOWN;

   // [CHECK] Initially check the path even exists
   if (PathFileExists(szGameFolder))
   {
      // Analyse folder contents
      iContentFlags = identifyGameFolderContents(szGameFolder, eVersion);

      /// [DATA FILES or CATALOGUES] 
      if (iContentFlags INCLUDES (GMC_DATA_FILES WITH GMC_CATALOGUES))
      {
         /// [SINGLE EXECUTABLE] Return VALID + VERSION
         if ((iContentFlags INCLUDES GMC_EXECUTABLE) AND eVersion != GV_UNKNOWN)
            eResult = GFS_VALID;
         
         /// [X3TC+X3AP EXECUTABLES] Return INCOMPLETE + X3:AP
         else if (iContentFlags INCLUDES GMC_EXECUTABLE)
         {
            eResult  = GFS_INCOMPLETE;
            eVersion = GV_ALBION_PRELUDE;
         }
         /// [NO EXECUTABLE] Return INCOMPLETE + UNKNOWN
         else
            eResult = GFS_INCOMPLETE;
      }
      /// [NO DATA FILES / NO CATALOGUES] Return INVALID + UNKNOWN
      else
         eResult = GFS_INVALID;
   }

   // Return result
   return eResult;
}


/// Function name  : identifyGameFolderContents
// Description     : Analyses the contents of a specified folder for the presence of the necessary game files
// 
// CONST TCHAR*   szGameFolder : [in]  Path to verify (must contain a trailing backslash)
// GAME_VERSION&  eVersion     : [out] If the result contains GMC_EXECUTABLE then this indicates the game version
// 
// Return Value   : Combination of the following flags:
//
//                   GMC_NOTHING    : No game data was found
//                   GMC_DATA_FILES : All the necessary extracted game data files are present
//                   GMC_EXECUTABLE : An executable is present (eVersion : Indicates the game version. GV_UNKNOWN indicates X3TC+X3AP)
//                   GMC_CATALOUGES : Catalogues are present
// 
UINT   identifyGameFolderContents(CONST TCHAR*  szGameFolder, GAME_VERSION&  eVersion)
{
   TCHAR   *szFullPath,              // Path of the file currently being tested
           *szFullPath2;             // Path of the file currently being tested
   UINT     iOutputFlags,            // Operation Result
            iCurrentFile,            // Iterator for generating filenames
            bResult;
   
   // [CHECK] Game folder must have a trailing backslash
   ASSERT(!lstrlen(szGameFolder) OR szGameFolder[lstrlen(szGameFolder)-1] == '\\');

   // Prepare
   iOutputFlags = GMC_DATA_FILES;
   eVersion     = GV_UNKNOWN;

   /// [EXECUTABLES] Search for game executables
   for (iCurrentFile = GV_ALBION_PRELUDE; (INT)iCurrentFile >= GV_THREAT; iCurrentFile--)
   {
      // [CHECK] Does executable exist?
      if (PathFileExists(szFullPath = generateGameExecutableFilePath(szGameFolder, (GAME_VERSION)iCurrentFile)))
      {
         // [CHECK] Did we just detect X3TC and X3AP?
         if (iCurrentFile == GV_TERRAN_CONFLICT AND eVersion == GV_ALBION_PRELUDE)
            // [X3TC + X3AP] Add 'executable' flag but return UNKNOWN game version
            eVersion = GV_UNKNOWN;
         else
         {
            /// [FOUND] Add 'executable' flag, identify the GameVersion, Abort..
            iOutputFlags |= GMC_EXECUTABLE;
            eVersion      = (GAME_VERSION)iCurrentFile;
         }
      } 

      // Cleanup
      utilDeleteString(szFullPath);
   }

   /// [DATA FILES] Search for extracted data files
   for (bResult = TRUE, iCurrentFile = GFI_DOCKS; bResult AND (iCurrentFile <= GFI_MASTER_LANGUAGE); iCurrentFile++)
   {
      // Examine file ID
      switch (iCurrentFile)
      {
      /// [TYPE FILES]
      default:                     
         // Generate data file path
         szFullPath = generateGameDataFilePath((GAME_DATA_FILE_INDEX)iCurrentFile, szGameFolder, eVersion);
         // Compare against TXT
         bResult = isPhysicalFilePresent(szFullPath, TEXT(".txt")); 
         break;

      /// [SPEECH/LANGUAGE FILES]
      case GFI_SPEECH_CLIPS:
      case GFI_MASTER_LANGUAGE:
         // Generate language specific data file path
         szFullPath = generateGameLanguageFilePath((GAME_DATA_FILE_INDEX)iCurrentFile, szGameFolder, eVersion, getAppPreferences()->eGameLanguage);
         // Compare against XML
         bResult = isPhysicalFilePresent(szFullPath, TEXT(".xml")); 
         break;
      }

      // [CHECK] Was file missing?
      if (!bResult)
         /// [NOT FOUND] Remove 'Data Files' flag
         iOutputFlags ^= GMC_DATA_FILES;

      // Cleanup
      utilDeleteString(szFullPath);
   }

   /// [CATALOGUES] Search for game catalogues (X2 has 5, TC has 12, AP has 13, presumably X3R has something between)
   for (bResult = TRUE, iCurrentFile = 1; bResult AND (iCurrentFile <= 13); iCurrentFile++)
   {
      // Generate catalogue path
      szFullPath  = generateGameCatalogueFilePath(GFI_CATALOGUE, szGameFolder, iCurrentFile, eVersion);
      szFullPath2 = generateGameCatalogueFilePath(GFI_CATALOGUE_DATA, szGameFolder, iCurrentFile, eVersion);

      // [CHECK] Does catalogue and data file exist?
      bResult = (PathFileExists(szFullPath) AND PathFileExists(szFullPath2));
      
      // Cleanup
      utilDeleteStrings(szFullPath, szFullPath2);
   }

   /// [AT LEAST ONE CATALOGUE] Add 'Catalogues' flag
   if (iCurrentFile > 1)
      iOutputFlags |= GMC_CATALOGUES;
   
   // Return result
   return iOutputFlags;
}


/// Function name  : identifyGameVersionIconID
// Description     : Identifies the resource ID of the icon matching the game version
// 
// CONST GAME_VERSION  eGameVersion : [in] GameVersion
// 
// Return Value   : Resource ID of the game version icon
// 
BearScriptAPI
CONST TCHAR*  identifyGameVersionIconID(CONST GAME_VERSION  eGameVersion)
{
   CONST TCHAR*  szOutput;

   switch (eGameVersion)
   {
   case GV_THREAT:            szOutput = TEXT("THREAT_ICON");           break;
   case GV_REUNION:           szOutput = TEXT("REUNION_ICON");          break;
   case GV_TERRAN_CONFLICT:   szOutput = TEXT("TERRAN_CONFLICT_ICON");  break;
   case GV_ALBION_PRELUDE:    szOutput = TEXT("ALBION_PRELUDE_ICON");   break;
   }

   return szOutput;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         THREAD OPERATIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : [THREAD] threadprocLoadGameData
// Description     : Loads all the game data into various trees ie. strings, object names, commands and command descriptions.
//
// VOID*  pParameter : [in] Operation data
// 
// Return type : Ignored
// Operation Stages : SIX
//
// Operation Result : OR_SUCCESS - GameData was loaded successfully. If there were non-critical errors they were
//                                   ignored by the user. Each of the game data trees is guaranteed to exist, but
//                                   their contents could be anything.
//                    OR_FAILURE - No GameData was loaded. No VFS was created. Critical errors were encountered
//                                   that prevented the loading of game data (eg. missing files). Any partially
//                                   loaded game data has been destroyed. Game data trees are guaranteed NOT to exist.
//                    OR_ABORTED - No GameData was loaded. No VFS was created. Minor errors led to the user aborting
//                                   the loading of game data. Any partially loaded game data has been destroyed.
//                                   Game Data trees are guaranteed NOT to exist.
//
BearScriptAPI
DWORD   threadprocLoadGameData(VOID*  pParameter)
{
   OPERATION_DATA*   pOperationData;   // Input data
   OPERATION_RESULT  eResult;          // Operation result
   
   // [DEBUGGING]
   TRACK_FUNCTION();
   SET_THREAD_NAME("Load Game Data");
   VERBOSE_LIB_COMMAND();
   setThreadLanguage(getAppPreferences()->eAppLanguage);

   // [CHECK] Ensure parameter exists
   ASSERT(pParameter);

   // Prepare
   pOperationData = (OPERATION_DATA*)pParameter;
   eResult        = OR_SUCCESS;

   /// [GUARD BLOCK]
   __try
   {
      for (UINT  iIndex = 0; (iIndex < 6) AND (eResult == OR_SUCCESS); iIndex++)
      {
         switch (iIndex)
         {
         /// Create VIRTUAL FILE SYSTEM
         case 0:  eResult = loadFileSystem(getAppPreferences()->szGameFolder, pOperationData->pProgress, pOperationData->pErrorQueue);                    break;
         /// Load GAME STRINGS
         case 1:  eResult = loadGameStringTrees(getFileSystem(), pOperationData->hParentWnd, pOperationData->pProgress, pOperationData->pErrorQueue);     break;
         /// Load OBJECT NAMES
         case 2:  eResult = loadGameObjectTrees(getFileSystem(), pOperationData->hParentWnd, pOperationData->pProgress, pOperationData->pErrorQueue);     break;
         /// Load MEDIA ITEMS
         case 3:  eResult = loadMediaItemTrees(getFileSystem(), pOperationData->hParentWnd, pOperationData->pProgress, pOperationData->pErrorQueue);      break;
         /// Load COMMAND SYNTAX
         case 4:  eResult = loadCommandSyntaxTree(pOperationData->hParentWnd, pOperationData->pProgress, pOperationData->pErrorQueue);                    break;
         /// Load OBJECT DESCRIPTIONS
         case 5:  eResult = loadObjectDescriptions(pOperationData->pProgress, pOperationData->hParentWnd, pOperationData->pErrorQueue);                   break;
         }
      }
   }
   /// [EXCEPTION HANDLER]
   __except (generateQueuedExceptionError(GetExceptionInformation(), pOperationData->pErrorQueue))
   {
      // [FAILURE] "An unidentified and unexpected critical error has occurred while loading the game data from '%s'"
      enhanceLastError(pOperationData->pErrorQueue, ERROR_ID(IDS_EXCEPTION_LOAD_GAME_DATA), getAppPreferences()->szGameFolder);
      eResult = OR_FAILURE;
   }
   
   /// [ABORT/ERROR/EXCEPTION] Destroy any loaded game data
   if (eResult != OR_SUCCESS)
   {
      // [FAILED] Set critical error flag
      if (eResult == OR_FAILURE)
         setAppError(AE_LOAD_GAME_DATA);
      
      // Cleanup
      destroyGameData();
   }

   // [DEBUG] Separate previous output from further output for claritfy
   VERBOSE_THREAD_COMPLETE("GAME DATA WORKER THREAD COMPLETED");

   // Cleanup and return
   END_TRACKING();
   closeThreadOperation(pOperationData, eResult);
   return THREAD_RETURN;
}
