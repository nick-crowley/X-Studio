//
// Virtual File System.cpp : Translation layer for accessing the physical and virtual file systems transparently with a single system.
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          MACROS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                   CONSTANTS / GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// Defines the byte ordering marks present in some files to define whether they're UNICODE or not
//
CONST BYTE  iByteOrderingUTF8[3]     = { 0xEF, 0xBB, 0xBF },    // UTF-8 byte ordering header
            iByteOrderingUTF16_BE[2] = { 0xFE, 0xFF },          // UTF-16 BigEndian byte ordering header
            iByteOrderingUTF16_LE[2] = { 0xFF, 0xFE };          // UTF-16 LittleEndian byte ordering header

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createFileItemTree
// Description     : Creates an FileItem tree sorted any supported property, in ascending order
// 
// CONST AVL_TREE_SORT_KEY  eSortKey   : [in] Property to sort the results by
// CONST AVL_TREE_ORDERING  eDirection : [in] Direction of sorting
//
// Return Value   : New AVLTree, you are responsible for destroying it
// 
BearScriptAPI
AVL_TREE*  createFileItemTree(CONST AVL_TREE_SORT_KEY  eSortKey, CONST AVL_TREE_ORDERING  eDirection)
{
   return createAVLTree(extractFileItemTreeNode, deleteFileItemTreeNode, createAVLTreeSortKey(AK_GROUP, AO_DESCENDING), createAVLTreeSortKey(eSortKey, eDirection), (eSortKey != AK_PATH ? createAVLTreeSortKey(AK_PATH, AO_DESCENDING) : NULL));
}


/// Function name  : createFileSearch
// Description     : Creates a new (empty) FileSearch object
// 
// CONST AVL_TREE_SORT_KEY  eSortKey   : [in] Property to sort the results by
// CONST AVL_TREE_ORDERING  eDirection : [in] Direction of sorting
// 
// Return Value   : New FileSearch object, you are responsible for destroying it
// 
FILE_SEARCH*  createFileSearch(CONST AVL_TREE_SORT_KEY  eSortKey, CONST AVL_TREE_ORDERING  eDirection)
{
   FILE_SEARCH*  pFileSearch;  // Create a new file search object (which is a convenience description for a binary tree)

   // Create new FileSearch object
   pFileSearch = utilCreateEmptyObject(FILE_SEARCH);

   // Create results tree and set initial result
   pFileSearch->pResultsTree   = createFileItemTree(eSortKey, eDirection);
   pFileSearch->iCurrentResult = 0;

   // Return new object
   return pFileSearch;
}


/// Function name  : createFileSystem
// Description     : Create the VirtualFileSystem object and store the game folder
// 
// CONST TCHAR*  szGameFolder  : [in]  Game folder path
// ERROR_STACK*  &pError       : [out] Error, if any
// 
// Return Value   : If successful a New VirtualFileSystem object, which you are responsible for destroying.
//                  If unsuccessful then NULL
// 
FILE_SYSTEM*  createFileSystem(CONST TCHAR*  szGameFolder, ERROR_STACK*  &pError)
{
   FILE_SYSTEM*  pNewFileSystem;

   // Prepare
   pNewFileSystem = NULL;
   pError         = NULL;

   // [CHECK] Game folder exists
   if (!PathFileExists(szGameFolder))
      // [ERROR] The specified game folder '%s' could not be found
      pError = generateDualError(HERE(IDS_VFS_GAME_FOLDER_NOT_FOUND), szGameFolder);
    
   else
   {
      // [SUCCESS] Create object
      pNewFileSystem = utilCreateEmptyObject(FILE_SYSTEM);

      // Set Game Folder
      pNewFileSystem->szGameFolder = utilDuplicatePath(szGameFolder);
      PathAddBackslash(pNewFileSystem->szGameFolder);

      // Create CatalogueList and VFS tree
      pNewFileSystem->pCatalogueList = createList(destructorCatalogue);
      pNewFileSystem->pFilesTree     = createVirtualFileTreeByPath();
   }

   // Return file system, or NULL
   return pNewFileSystem;
}



/// Function name  : deleteFileSearch
// Description     : Destroys a FileSearch object and any results it contains
// 
// FILE_SEARCH*  &pFileSearch   : [in] FileSearch object to destroy
// 
BearScriptAPI
VOID deleteFileSearch(FILE_SEARCH*  &pFileSearch)
{
   // Delete results tree
   deleteAVLTree(pFileSearch->pResultsTree);
   // Delete calling object
   utilDeleteObject(pFileSearch);
}


/// Function name  : deleteFileSystem
// Description     : Destroy a virtual file system object
// 
// FILE_SYSTEM*  &pFileSystem   : [in] Virtual file system object to destroy
// 
VOID  deleteFileSystem(FILE_SYSTEM*  &pFileSystem)
{
   // Delete game folder
   utilDeleteString(pFileSystem->szGameFolder);

   // Delete catalogue list
   deleteList(pFileSystem->pCatalogueList);

   // Delete Virtual files tree
   deleteAVLTree(pFileSystem->pFilesTree);
   
   // Delete calling object
   utilDeleteObject(pFileSystem);
}


/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : findFileSearchResultByIndex
// Description     : Provides index-based access to a FileSearch's results tree
// 
// CONST FILE_SEARCH*  pFileSearch  : [in] FileSearch containing the results
// CONST UINT          iIndex       : [in] Zero based index to search for
// FILE_ITEM*         &pOutput      : [in] Associated FileItem if found, otherwise NULL
// 
// Return Value   : TRUE if succesful, FALSE if the index was invalid or no search has been performed
// 
BearScriptAPI
BOOL   findFileSearchResultByIndex(CONST FILE_SEARCH*  pFileSearch, CONST UINT  iIndex, FILE_ITEM*  &pOutput)
{
   // Query tree
   return findObjectInAVLTreeByIndex(pFileSearch->pResultsTree, iIndex, (LPARAM&)pOutput);
}


/// Function name  : findNextFileSearchResult
// Description     : Encapsulates iterating through a FileSearch results tree
// 
// FILE_SYSTEM*  pFileSystem  : [in]  FileSearch containing the search results
// FILE_ITEM*   &pOutput      : [out] FileItem search result if any, otherwise NULL
// 
// Return Value   : TRUE if there are more files, FALSE otherwise
// 
BearScriptAPI
BOOL   findNextFileSearchResult(FILE_SEARCH*  pFileSearch, FILE_ITEM*  &pOutput)
{
   // Prepare
   pOutput = NULL;

   // [CHECK] Return FALSE if there are no more results remaining
   if (pFileSearch->iCurrentResult >= pFileSearch->pResultsTree->iCount)
      return FALSE;

   // Query tree
   return findObjectInAVLTreeByIndex(pFileSearch->pResultsTree, pFileSearch->iCurrentResult++, (LPARAM&)pOutput);
}


/// Function name  : findVirtualFileByPath
// Description     : Search the VFS tree for the VirtualFile associated with a specified FilePath
// 
// CONST FILE_SYSTEM* pFileSystem  : [in]  The VirtualFileSystem to search
// CONST TCHAR*       szFullPath   : [in]  The FilePath to search for
// VIRTUAL_FILE*     &pOutput      : [out] The resultant VirtualFile if found, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BearScriptAPI 
BOOL   findVirtualFileByPath(CONST FILE_SYSTEM*  pFileSystem, CONST TCHAR*  szFullPath, VIRTUAL_FILE*  &pOutput)
{
   TCHAR*   szSearchTerm;

   // Prepare
   pOutput      = NULL;
   szSearchTerm = utilCreateEmptyPath();
   StringCchCopy(szSearchTerm, MAX_PATH, szFullPath);

   // [CHECK] Remove the game folder prefix from the search path, if present
   if (isPathSubFolderOf(pFileSystem->szGameFolder, szFullPath))
   {
      StringCchCopy(szSearchTerm, MAX_PATH, &szFullPath[lstrlen(pFileSystem->szGameFolder)]);
      PathRemoveBackslash(szSearchTerm);
   }

   // [CHECK] Replace '.txt' and '.xml' extensions with '.pck'
   if (utilCompareString(PathFindExtension(szFullPath), ".txt") OR utilCompareString(PathFindExtension(szFullPath), ".xml"))
      PathRenameExtension(szSearchTerm, TEXT(".pck"));

   // Query VirtualFileSystem tree
   findObjectInAVLTreeByValue(pFileSystem->pFilesTree, (LPARAM)szSearchTerm, NULL, (LPARAM&)pOutput);

   // Cleanup and return
   utilDeleteString(szSearchTerm);
   return (pOutput != NULL);
}


/// Function name  : getFileSystemResultCount
// Description     : Retrieve the number of results within a FileSearch object
// 
// CONST FILE_SEARCH*  pFileSearch  : [in] FileSearch object containing the results
// 
// Return Value   : Number of results
// 
BearScriptAPI
UINT   getFileSearchResultCount(CONST FILE_SEARCH*  pFileSearch)
{
   return pFileSearch->pResultsTree->iCount;
}


/// Function name  : isCatalogueFilePresent
// Description     : Search the VFS for the CatalogueFile with a specified FilePath
// 
// CONST FILE_SYSTEM* pFileSystem  : [in]  FileSystem
// CONST TCHAR*       szFullPath   : [in]  FilePath to search for
// VIRTUAL_FILE*     &pOutput      : [out] Matching CatalogueFile if found, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BearScriptAPI 
BOOL   isCatalogueFilePresent(CONST FILE_SYSTEM*  pFileSystem, CONST TCHAR*  szFullPath)
{
   CATALOGUE_FILE*  pCatalogueFile;    // CatalogueFile being processed
   TCHAR*           szTargetPath;      // Copy of input path, used for renaming '.dat' searches
   BOOL             bResult;           // Search result

   // Prepare
   szTargetPath = utilDuplicatePath(szFullPath);
   bResult      = FALSE;

   // [CHECK] Are we searching for a data file?
   if (utilCompareString(PathFindExtension(szTargetPath), ".dat"))
      /// [DATA FILE] Search for a catalogue instead -- only catalogue files are actually stored
      PathRenameExtension(szTargetPath, TEXT(".cat"));

   /// Iterate through all catalogues
   for (LIST_ITEM*  pIterator = getListHead(pFileSystem->pCatalogueList); pIterator; pIterator = pIterator->pNext)
   {      
      // Prepare
      pCatalogueFile = extractListItemPointer(pIterator, CATALOGUE_FILE);

      // [CHECK] Compare paths...
      if (bResult = utilCompareStringVariables(szTargetPath, pCatalogueFile->szFullPath))
         // [FOUND] Abort..
         break;
   }

   // Cleanup and return result
   utilDeleteString(szTargetPath);
   return bResult;
}


/// Function name  : isFilePresent
// Description     : Determines whether a file exists in either VFS or physical file systems
///                           NOTE: The VFS is checked first
// 
// CONST FILE_SYSTEM*  pFileSystem  : [in] FileSystem object
// CONST TCHAR*        szFullPath   : [in] Path to search for
// 
// Return Value   : TRUE/FALSE
// 
BearScriptAPI
BOOL   isFilePresent(CONST FILE_SYSTEM*  pFileSystem, CONST TCHAR*  szFullPath)
{
   VIRTUAL_FILE*  pDummyFile;       // Result of VFS search, ignored
   BOOL           bResult;          // Operation result

   // Prepare
   bResult = FALSE;

   /// [CHECK] Is path within the VFS?
   if (isPathSubFolderOf(pFileSystem->szGameFolder, szFullPath))
      // [SUCCESS] Check whether file is present in VFS
      bResult = findVirtualFileByPath(pFileSystem, szFullPath, pDummyFile);
   
   /// [NOT FOUND] Check the physical file system too
   if (!bResult)
      bResult = PathFileExists(szFullPath);

   // Return result
   return bResult;
}


/// Function name  : isPhysicalFilePresent
// Description     : Checks whether a game data file is present as an extracted file, either in PCK or TXT/XML format.
// 
// TCHAR*        szFullPath            : [in/out] Full path of the file to query
///                                                The extension of this path may be altered
// CONST TCHAR*  szAlternateExtension  : [in]     Alternate file extension to search under, including the full stop.
// 
// Return Value   : TRUE / FALSE
// 
BOOL  isPhysicalFilePresent(TCHAR*  szFullPath, CONST TCHAR*  szAlternateExtension)
{
   BOOL  bResult;    // Operation result

   // Prepare
   bResult = TRUE;

   /// [CHECK] Is file present?
   if (!PathFileExists(szFullPath))
   {
      // [FAILED] Attempt to locate under alternate extension
      PathRenameExtension(szFullPath, szAlternateExtension);

      /// [CHECK] Is file present under alternate extension?
      bResult = PathFileExists(szFullPath);
   }

   // Return result
   return bResult;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : enumerateVirtualFileSystem
// Description     : Reads all the catalogues in the game folder (specified by the VFS) and builds the tree
//                     of the files they contain.
// 
// FILE_SYSTEM*         pFileSystem  : [in]     VirtualFileSystem object containing the game folder
// OPERATION_PROGRESS*  pProgress    : [in/out] Operation progress
// ERROR_STACK*        &pError       : [out]    Error, if any
// 
// Return Value   : Number of files discovered  
// 
UINT   enumerateVirtualFileSystem(FILE_SYSTEM*  pFileSystem, OPERATION_PROGRESS*  pProgress, ERROR_STACK*  &pError)
{
   VIRTUAL_FILE    *pVirtualFile;     // New Virtual File/Folder being created
   CATALOGUE_FILE  *pCatalogue;       // Properties of the catalogue currently being processed
   RAW_FILE        *szCatalogue;      // Contents of the current catalogue
   CHAR            *szSubPath,        // Subpath of the file currently being processed
                   *pIterator,        // Tokeniser data
                   *szMarker,         // Pointer to the position of the current entry containing the file size
                   *szLastSubFolder;  // SubFolder of the last file processed, used to manually detect folders since they are not individually defined in the catalogues
   UINT             iFileCount,
                    iFilePosition,    // Location (in bytes) within the Data file marking the start of the file currently being processed
                    iFileSize;        // Packed size of the file currently being processed, in bytes
   
   // [VERBOSE]
   VERBOSE("Enumerating game catalogues in folder '%s", pFileSystem->szGameFolder);

   /// Enumerate the appropriate catalogues files to load in ascending order
   enumerateVirtualFileSystemCatalogues(pFileSystem, getAppPreferences()->eGameVersion);

   // [PROGRESS] Define progress as the number of catalogues to be loaded
   updateOperationProgressMaximum(pProgress, max(1, getListItemCount(pFileSystem->pCatalogueList)));
   
   /// Iterate BACKWARDS through the catalogues in the game folder
   for (UINT iCatalogue = getListItemCount(pFileSystem->pCatalogueList); (iCatalogue > 0) AND findListObjectByIndex(pFileSystem->pCatalogueList, (iCatalogue - 1), (LPARAM&)pCatalogue); iCatalogue--)
   {
      // Prepare
      iFileCount      = 0;
      iFilePosition   = 0;
      szLastSubFolder = utilCreateEmptyStringA(MAX_PATH);

      // [PROGRESS]
      advanceOperationProgressValue(pProgress);

      /// Attempt to load the catalogue
      if (!loadCatalogueFile(pCatalogue->szFullPath, szCatalogue))
         /// [ERROR] Eeek!
         ASSERT(FALSE);
      
      // Skip the first token (dat name)
      utilTokeniseStringA(szCatalogue, "\n", &pIterator);

      /// Iterate through all the filename/filesize pairs in the catalogue
      for (szSubPath = utilGetNextTokenA("\n", &pIterator); szSubPath; szSubPath = utilGetNextTokenA("\n", &pIterator))
      {
         // Sever the FileSize from the token and convert to integer
         szMarker    = utilFindCharacterReverseA(szSubPath, ' ');
         szMarker[0] = NULL;
         iFileSize   = utilConvertStringToIntegerA(szMarker + 1);

         // Convert the forward slashes in the FilePath to backslashes
         while (szMarker = utilFindCharacterA(szSubPath, '/'))
            szMarker[0] = '\\';

         /// Create new VFS File
         pVirtualFile = createVirtualFile(pCatalogue->iCatalogue, szSubPath, iFilePosition, iFileSize);

         /// Attempt to add current file to FileSystem
         if (insertObjectIntoAVLTree(pFileSystem->pFilesTree, (LPARAM)pVirtualFile))
            // [SUCCESS] Count file for debugging
            iFileCount++;
         else
            // [DUPLICATE] File already exists in a higher numbered catalogue
            deleteVirtualFile(pVirtualFile);            

         // [CHECK] Is this file within a sub-folder?
         if (szMarker = utilFindCharacterReverseA(szSubPath, '\\'))
         {
            // Remove the filename from the SubPath
            szMarker[0] = NULL;

            // [CHECK] Have we never seen this folder before?
            if (!szLastSubFolder[0] OR !utilCompareStringVariablesA(szSubPath, szLastSubFolder))
            {
               /// Create a new VFS Folder
               pVirtualFile = createVirtualFolder(szSubPath);

               /// Attempt to add folder to FileSystem
               if (!insertObjectIntoAVLTree(pFileSystem->pFilesTree, (LPARAM)pVirtualFile))
                  // [DUPLICATE] Folder has already been added, either in this catalogue or a higher numbered catalogue
                  deleteVirtualFile(pVirtualFile);

               // Update 'Last SubFolder'
               StringCchCopyA(szLastSubFolder, MAX_PATH, szSubPath);
            }
         }

         // Calculate position of next file (within data file)
         iFilePosition += iFileSize;
      }

      // [VERBOSE]
      VERBOSE("Loaded %u virtual files from catalogue '%s'", iFileCount, &pCatalogue->szFullPath[lstrlen(pFileSystem->szGameFolder)]);
      
      // Cleanup
      deleteRawFileBuffer(szCatalogue);
      utilDeleteStringA(szLastSubFolder);
   }

   // [CHECK] Generate a warning if no files were found
   if (!getTreeNodeCount(pFileSystem->pFilesTree))
      // [WARNING] "No catalogues were found in the game folder '%s'"
      pError = generateDualWarning(HERE(IDS_VFS_CATALOGUES_NOT_FOUND), pFileSystem->szGameFolder);
   
   // Cleanup and return
   return getTreeNodeCount(pFileSystem->pFilesTree);
}


/// Function name  : enumerateVirtualFileSystemCatalogues
// Description     : Builds the VFS list of CATALOGUE_FILEs indicating which catalogues exist
// 
// CONST FILE_SYSTEM*  pFileSystem   : [in] File system
// CONST GAME_VERSION  eGameVersion  : [in] GameVersion of the catalogues to search for
// 
// Return Value   : Number of catalogues found
// 
UINT    enumerateVirtualFileSystemCatalogues(CONST FILE_SYSTEM*  pFileSystem, CONST GAME_VERSION  eGameVersion)
{
   CONST UINT  iMaxCatalogueCount = 99;      // Maximum possible number of catalogues
   UINT        iCatalogueLimit;              // Number of catalogues to search for in base folder
   TCHAR*      szFullPath;                   // FilePath of the catalogue currently being processed
   BOOL        bCatalogueExists;             // Whether current catalogue exists

   // Prepare
   bCatalogueExists = TRUE;
   szFullPath       = NULL;

   // [ALBION PRELUDE] Only load first 13 catalogues
   iCatalogueLimit = (eGameVersion == GV_ALBION_PRELUDE ? 13 : iMaxCatalogueCount);

   /// [ALL VERSIONS] Iterate through potential catalogue IDs 
   for (UINT iCatalogue = 1; bCatalogueExists AND iCatalogue <= iCatalogueLimit; iCatalogue++)
   {
      // Generate path into 'base' folder
      szFullPath = generateGameCatalogueFilePath(GFI_CATALOGUE, pFileSystem->szGameFolder, iCatalogue, GV_THREAT);

      // [CHECK] Does the catalogue exist?
      if (bCatalogueExists = PathFileExists(szFullPath))
      {
         // [SUCCESS] Add to catalogue list
         appendObjectToList(pFileSystem->pCatalogueList, (LPARAM)createCatalogueFile(iCatalogue, szFullPath));
         VERBOSE("Base catalogue detected: '%s'", szFullPath);
      }

      // Cleanup
      utilDeleteString(szFullPath);
   }

   /// [ALBION-PRELUDE] Iterate through potential catalogue IDs in the 'addon' folder
   if (eGameVersion == GV_ALBION_PRELUDE)
   {
      // Iterate through potential catalogue IDs
      for (UINT iCatalogue = 1; bCatalogueExists AND iCatalogue <= iMaxCatalogueCount; iCatalogue++)
      {
         // Generate path into 'addon' folder
         szFullPath = generateGameCatalogueFilePath(GFI_CATALOGUE, pFileSystem->szGameFolder, iCatalogue, GV_ALBION_PRELUDE);

         // [CHECK] Does the catalogue exist?
         if (bCatalogueExists = PathFileExists(szFullPath))
         {
            // [SUCCESS] Add to catalogue list with 'ADDON' flag
            appendObjectToList(pFileSystem->pCatalogueList, (LPARAM)createCatalogueFile(iCatalogue WITH CLF_ADDON, szFullPath));
            VERBOSE("Addon catalogue detected: '%s'", szFullPath);
         }

         // Cleanup
         utilDeleteString(szFullPath);
      }
   }

   // Return list count
   return getListItemCount(pFileSystem->pCatalogueList);
}


/// Function name  : identifyFileType
// Description     : Determine the file-type of a file from it's contents
// 
// CONST FILE_SYSTEM*  pFileSystem : [in] VirtualFileSystem object
// CONST TCHAR*        szFullPath  : [in] FilePath of the file to examine
//
// Return Type : Specified file type, or FIF_NONE if type could not be determined
// 
BearScriptAPI
FILE_ITEM_FLAG  identifyFileType(CONST FILE_SYSTEM*  pFileSystem, CONST TCHAR*  szFullPath)
{
   FILE_ITEM_FLAG  eResult;          // Operation result
   ERROR_QUEUE*    pErrorQueue;      // IO Error message, if any
   CONST TCHAR*    szExtension;      // Extension of the input file
   RAW_FILE*       szFileBuffer;     // Buffer to contain file
   TCHAR*          szFilePathCopy;   // Non-const copy of filepath
   UINT            iFileSize;        // Size of file, in bytes
   
   // Prepare
   szFilePathCopy = utilDuplicateSimpleString(szFullPath);
   szExtension    = PathFindExtension(szFullPath);
   eResult        = FIF_NONE;

   // [CHECK] Ensure file is XML or PCK
   if (utilCompareString(szExtension, ".pck") OR utilCompareString(szExtension, ".xml") OR utilCompareString(szExtension, ".xprj"))
   {
      // Prepare
      pErrorQueue = createErrorQueue();

      // Load file
      if (iFileSize = loadRawFileFromFileSystemByPath(pFileSystem, szFilePathCopy, NULL, szFileBuffer, pErrorQueue))  
      {
         /// Identify file type from content
         eResult = identifyFileTypeFromBuffer(szFileBuffer, iFileSize);
         deleteRawFileBuffer(szFileBuffer);
      }

      // Cleanup
      deleteErrorQueue(pErrorQueue);
   }

   // Cleanup and return
   utilDeleteString(szFilePathCopy);
   return eResult;
}


/// Function name  : loadFileSystem
// Description     : Create and load the VirtualFileSystem from the catalogues in the game folder
// 
// CONST TCHAR*         szGameFolder  : [in]     Game folder containing the catalogues. Must have a trailing backslash
// OPERATION_PROGRESS*  pProgress     : [in/out] Operation progress
// ERROR_QUEUE*         pErrorQueue   : [in/out] Error queue to hold any errors encountered
// 
// Return Value : OR_SUCCESS - FileSystem was created succesfully, even if zero virtual files were found
//                OR_FAILURE - FileSystem was NOT created because the 'game folder' does not exist
// 
OPERATION_RESULT  loadFileSystem(CONST TCHAR*  szGameFolder, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue)
{
   ERROR_STACK*      pError      = NULL;         // Error, if any
   FILE_SYSTEM*      pFileSystem = NULL;         // New FileSystem
   OPERATION_RESULT  eResult     = OR_SUCCESS;   // Operation result

   __try
   {
      CONSOLE_STAGE();

      // [CHECK] Input folder has a trailing backslash
      ASSERT(lstrlen(szGameFolder) AND szGameFolder[lstrlen(szGameFolder) - 1] == '\\');

      // [INFO/PROGRESS] "Searching for catalogues and data files"
      pushErrorQueue(pErrorQueue, generateDualInformation(HERE(IDS_OUTPUT_LOADING_FILE_SYSTEM)));
      ASSERT(getOperationProgressStageID(pProgress) == IDS_PROGRESS_LOADING_CATALOGUES);

      /// Create Virtual File System
      if ((pFileSystem = createFileSystem(szGameFolder, pError)) == NULL)
      {
         /// [CRITICAL ERROR] "There was an error while creating the virtual file system from the catalogues in the game folder '%s'"
         enhanceError(pError, ERROR_ID(IDS_VFS_ERROR), szGameFolder);
         eResult = OR_FAILURE;  
      }
      else
      {
         // [SUCCESS] Store global pointer
         setFileSystem(pFileSystem);
         
         /// Populate the FileSystem
         if (!enumerateVirtualFileSystem(pFileSystem, pProgress, pError))
            // [WARNING] "There was an error while creating the virtual file system from the catalogues in the game folder '%s'"
            enhanceWarning(pError, ERROR_ID(IDS_VFS_ERROR), szGameFolder);
            
         // [VERBOSE]
         CONSOLE_HEADING("Loaded %u virtual files from %u game catalogues", getTreeNodeCount(pFileSystem->pFilesTree), getListItemCount(pFileSystem->pCatalogueList));
      }

      // [ERROR]
      if (pError)
         pushErrorQueue(pErrorQueue, pError);

      // Cleanup and return result
      return eResult;
   }
   __except (pushException(pErrorQueue))
   {
      EXCEPTION1("Unable to load the virtual file system from '%s'", szGameFolder);
      return OR_FAILURE;
   }
}


/// Function name  : loadGameFileFromFileSystemByPath
// Description     : Fills a GameFile object using the contents of a text file specified by a full file path
// 
// CONST FILE_SYSTEM* pFileSystem  : [in]  VirtualFileSystem object
// GAME_FILE*         pOutput      : [in]  'szFullPath' contains the path of the file load
///                                  [out] 'szInputBuffer' contains UNICODE version of the file
// CONST TCHAR*       szAltEx      : [in]  Alternate file extension
// ERROR_QUEUE*       pErrorQueue  : [out] Error message, if any
// 
// Return Value   : Length of the document in characters, or NULL if unsuccesful
// 
BearScriptAPI 
UINT   loadGameFileFromFileSystemByPath(CONST FILE_SYSTEM*  pFileSystem, GAME_FILE*  pOutput, CONST TCHAR*  szAlternateExtension, ERROR_QUEUE*  pErrorQueue)
{
   RAW_FILE*  szFileBuffer;  // ANSI file input buffer

   __try
   {
      // [CHECK] Ensure GameFile is empty
      ASSERT(!hasInputData(pOutput));

      /// Attempt to load file in ANSI
      if (pOutput->iInputSize = loadRawFileFromFileSystemByPath(pFileSystem, pOutput->szFullPath, szAlternateExtension, szFileBuffer, pErrorQueue))
      {
         /// Convert to UNICODE. Update file-size in case any characters were dropped
         pOutput->szInputBuffer = utilTranslateStringToUNICODEEx((CHAR*)szFileBuffer, pOutput->iInputSize);
         deleteRawFileBuffer(szFileBuffer);
      }

      // Return number of characters read
      return pOutput->iInputSize;
   }
   __except (pushException(pErrorQueue))
   {
      EXCEPTION2("Unable to load game file '%s' (.%s)", pOutput ? pOutput->szFullPath : NULL, szAlternateExtension);
      return 0;
   }
}


/// Function name  : loadRawFileFromFileSystemByPath
// Description     : Load the specified file transparently from either the virtual or the physical file system
///                    -> The physical file system is given priority if the file exists as both virtual and physical
///                    -> The file will be decrypted and/or decompressed appropriately
// 
// CONST FILE_SYSTEM*  pFileSystem           : [in]           VirtualFileSystem object specifying the game folder
// TCHAR*              szFullPath            : [in/out]       Full FilePath of the file to load
///                                                                     If an alternate extension is provided, and loaded, the filepath will be altered to reflect this
// CONST CHAR*         szAlternateExtension  : [in][optional] Alternate file extension to search under
// RAW_FILE*          &szOutput              : [out]          New ANSI byte buffer containing the uncompressed file contents if succesful, otherwise NULL
// ERROR_QUEUE*        pErrorQueue           : [in/out]       Error message, if any
// 
// Return Value   : Length of the output buffer in bytes if successful, otherwise NULL
// 
BearScriptAPI
UINT   loadRawFileFromFileSystemByPath(CONST FILE_SYSTEM*  pFileSystem, TCHAR*  szFullPath, CONST TCHAR*  szAlternateExtension, RAW_FILE*  &szOutput, ERROR_QUEUE*  pErrorQueue)
{
   VIRTUAL_FILE  *pVirtualFile;        // [VIRTUAL]   VirtualFile associated with the file
   TCHAR         *szDataFilePath,      // [VIRTUAL]   FilePath of the data file containing the file
                 *szAlternatePath;     // [ALTERNATE] Alternate file path
   RAW_FILE      *szStrippedOutput;    //             Used for stripping the byte ordering mark from decrypted files that have them
   UINT           iBytesRead;          //             Number of bytes read or decompressed
   
   __try
   {
      // Prepare
      szAlternatePath = NULL;
      pVirtualFile    = NULL;
      szOutput        = NULL;
      iBytesRead      = NULL;

      // [ALTERNATE] Generate alternate path
      if (szAlternateExtension)
      {
         szAlternatePath = utilDuplicatePath(szFullPath);
         PathRenameExtension(szAlternatePath, szAlternateExtension);
      }

      // [CHECK] Is file present?
      if (PathFileExists(szFullPath))
         /// [PHYSICAL] Load the file from the physical filesystem
         iBytesRead = loadRawFileFromUnderlyingFileSystemByPath(szFullPath, NULL, szOutput, pErrorQueue);

      // [CHECK] Is the alternate file present?
      else if (szAlternateExtension AND PathFileExists(szAlternatePath))
      {
         // Rename input path to reflect alternate extension
         PathRenameExtension(szFullPath, szAlternateExtension);      // [FIX] BUG:1032 'The filename in the output dialog and VERBOSE doesn't always reflect the alternate extension of the file, if that was loaded instead'

         /// [ALTERNATE PHYSICAL] Load the alternate file from the physical filesystem
         iBytesRead = loadRawFileFromUnderlyingFileSystemByPath(szAlternatePath, NULL, szOutput, pErrorQueue);
      }
      // [CHECK] Is the file in the Virtual File System?
      else if (isPathSubFolderOf(pFileSystem->szGameFolder, szFullPath) AND findVirtualFileByPath(pFileSystem, szFullPath, pVirtualFile))
      {
         // Generate data file path 
         szDataFilePath = generateGameCatalogueFilePath(GFI_CATALOGUE_DATA, pFileSystem->szGameFolder, LOWORD(pVirtualFile->iCatalogueID), (pVirtualFile->iCatalogueID INCLUDES CLF_ADDON ? GV_ALBION_PRELUDE : GV_THREAT));

         /// [VIRTUAL] Load the file from the appropriate .DAT file
         iBytesRead = loadRawFileFromUnderlyingFileSystemByPath(szDataFilePath, pVirtualFile, szOutput, pErrorQueue);
         utilDeleteString(szDataFilePath);
      }
      // [CHECK] Is the alternate file in the Virtual File System?
      else if (isPathSubFolderOf(pFileSystem->szGameFolder, szFullPath) AND szAlternateExtension AND findVirtualFileByPath(pFileSystem, szAlternatePath, pVirtualFile))
      {
         // Generate data file path and modified input path to reflect alternate extension
         szDataFilePath = generateGameCatalogueFilePath(GFI_CATALOGUE_DATA, pFileSystem->szGameFolder, LOWORD(pVirtualFile->iCatalogueID), (pVirtualFile->iCatalogueID INCLUDES CLF_ADDON ? GV_ALBION_PRELUDE : GV_THREAT));
         PathRenameExtension(szFullPath, szAlternateExtension);      // [FIX] BUG:1032 'The filename in the output dialog and VERBOSE doesn't always reflect the alternate extension of the file, if that was loaded instead'

         /// [ALTERNATE VIRTUAL] Load the file from the appropriate .DAT file
         iBytesRead = loadRawFileFromUnderlyingFileSystemByPath(szDataFilePath, pVirtualFile, szOutput, pErrorQueue);
         utilDeleteString(szDataFilePath);
      }
      else if (szAlternateExtension)
         /// [ALTERNATE NOT FOUND] "The file '%s' (or %s) could not be found in physical file system or the game catalogues"
         pushErrorQueue(pErrorQueue, generateDualError(HERE(IDS_LOAD_ALTERNATE_FILE_NOT_FOUND), szFullPath, szAlternateExtension));
      else
         /// [NOT FOUND] "The file '%s' could not be found in physical file system or the game catalogues"
         pushErrorQueue(pErrorQueue, generateDualError(HERE(IDS_LOAD_FILE_NOT_FOUND), szFullPath));
      
      // [SUCCESS] Decrypt/decompress file as appropriate
      if (iBytesRead > 0)
      {
         /// [DECODE] Decrypt/decompress and return number new file size
         iBytesRead = performRawFileDecryption(PathFindFileName(szFullPath), szOutput, iBytesRead, (pVirtualFile ? FIF_VIRTUAL : FIF_PHYSICAL), pErrorQueue);

         /// [CHECK] Does decrypted file start with a UTF-16 byte ordering mark?
         if (utilCompareMemory(szOutput, iByteOrderingUTF16_BE, 2) OR utilCompareMemory(szOutput, iByteOrderingUTF16_LE, 2))
         {
            // [ERROR] Cannot read the file '%s' because it is using the currently unsupported UTF-16 byte ordering 0x%02X, 0x%02X
            pushErrorQueue(pErrorQueue, generateDualError(HERE(IDS_FILE_UTF16_UNSUPPORTED), szFullPath, (UINT)szOutput[0], (UINT)szOutput[1]));

            // Cleanup and return ZERO
            deleteRawFileBuffer(szOutput);
            iBytesRead = 0;
         }
         /// [CHECK] Does the decrypted file start a UTF-8 byte ordering mark?
         else if (utilCompareMemory(szOutput, iByteOrderingUTF8, 3))
         {
            // Reduce filesize
            iBytesRead -= 3;

            // Strip BOM by re-allocating the output buffer
            szStrippedOutput = createRawFileBuffer(iBytesRead);
            utilCopyMemory(szStrippedOutput, &szOutput[3], iBytesRead);

            // Replace output buffer
            deleteRawFileBuffer(szOutput);
            szOutput = szStrippedOutput;
         }
      }

      // Cleanup and return number of bytes read/decompressed
      utilSafeDeleteString(szAlternatePath);
      return iBytesRead;
   }
   __except (pushException(pErrorQueue))
   {
      EXCEPTION2("Unable to load the file '%s' (.%s)", szFullPath, szAlternateExtension);
      return 0;
   }
}


/// Function name  : performFileSearchSort
// Description     : Sorts the results of a FileSearch
// 
// FILE_SEARCH*             pFileSearch : [in/out] FileSearch to resort
// CONST AVL_TREE_SORT_KEY  eSortBy     : [in]     New sort key
// CONST AVL_TREE_ORDERING  eDirection  : [in] Direction of sorting
// 
BearScriptAPI
VOID  performFileSearchSort(FILE_SEARCH*  pFileSearch, CONST AVL_TREE_SORT_KEY  eSortBy, CONST AVL_TREE_ORDERING  eDirection)
{
   AVL_TREE_OPERATION*  pReplicationOperation;     // Replication operation data

   // [CHECK] Ensure new sort key is different
   if (pFileSearch->pResultsTree->pSortKeys[0]->eSorting != eSortBy OR pFileSearch->pResultsTree->pSortKeys[0]->eDirection != eDirection)
   {
      // Prepare
      pReplicationOperation = createAVLTreeOperation(treeprocReplicateAVLTreeNode, ATT_INORDER);

      // Create new tree to hold re-sorted files
      pReplicationOperation->pOutputTree = createFileItemTree(eSortBy, eDirection);

      /// Copy files to new tree and index it
      performOperationOnAVLTree(pFileSearch->pResultsTree, pReplicationOperation);
      performAVLTreeIndexing(pReplicationOperation->pOutputTree);

      // Sever and destroy original results tree
      pFileSearch->pResultsTree->pfnDeleteNode = NULL;
      deleteAVLTree(pFileSearch->pResultsTree);

      /// Attach new files
      pFileSearch->pResultsTree   = pReplicationOperation->pOutputTree;
      pFileSearch->iCurrentResult = 0;

      // Cleanup
      deleteAVLTreeOperation(pReplicationOperation);
   }
}


/// Function name  : performFileSystemCopy
// Description     : Copy a file from the VFS to it's equivilent game-sub path in the physical file system
// 
// CONST FILE_SYSTEM*   pFileSystem   : [in]  VirtualFileSystem
// CONST TCHAR*         szFileSubPath : [in]  Virtual file game sub-path
// ERROR_QUEUE*         pErrorQueue   : [out] Error message, if any
// 
// Return Value   : TRUE if succesful, FALSE otherwise
// 
BearScriptAPI
BOOL   performFileSystemCopy(CONST FILE_SYSTEM*  pFileSystem, CONST TCHAR*  szFileSubPath, ERROR_QUEUE*  pErrorQueue)
{
   TCHAR*   szFullPath;       // Full uutput file path
   UINT     iFileSize;        // Length of file
   RAW_FILE*    szFileBuffer;     // Buffer containing the file

   // Generate full file path
   szFullPath = utilCreateEmptyPath();
   StringCchPrintf(szFullPath, MAX_PATH, TEXT("%s%s"), pFileSystem->szGameFolder, szFileSubPath);

   /// [CHECK] Only bother extracting the file if it doesn't already exist in the physical FS
   if (PathFileExists(szFullPath) == FALSE)
   // Extract the file from the VFS
      if (iFileSize = loadRawFileFromFileSystemByPath(pFileSystem, szFullPath, NULL, szFileBuffer, pErrorQueue))
         // Save to the physical FS
         saveRawFileToFileSystem(szFullPath, szFileBuffer, iFileSize, pErrorQueue);
   
   // Cleanup and return
   utilDeleteString(szFullPath);
   return !hasErrors(pErrorQueue);
}


/// Function name  : performFileSystemSearch
// Description     : Searches the physical and virtual FileSystems for files matching the specified path and filter,
//                    and outputs both of them to a new FileSearch object.
// 
// CONST FILE_SYSTEM*       pFileSystem  : [in/out]            FileSystem containing the 'files' and 'results' tree
// CONST TCHAR*             szFolder     : [in]                Specified path to search. If not a part of the game folder, no virtual results are returned.
// CONST FILE_FILTER        eFilter      : [in]                Filter to filter results by
// CONST AVL_TREE_ORDERING  eDirection   : [in]                Direction of sorting
// OPERATION_PROGRESS*      pProgress    : [in/out] [optional] Progress tracking object
// 
// Return Value   : New FileSearch object containing the file results (if any)
// 
BearScriptAPI
FILE_SEARCH*   performFileSystemSearch(CONST FILE_SYSTEM*  pFileSystem, CONST TCHAR*  szFolder, CONST FILE_FILTER  eFilter, CONST AVL_TREE_SORT_KEY  eSortBy, CONST AVL_TREE_ORDERING  eDirection, OPERATION_PROGRESS*  pProgress)
{
   FILE_SEARCH*   pSearchResults;     // FileSearch object
  
   // Prepare

   /// Create new FileSearch to hold results
   pSearchResults = createFileSearch(eSortBy, eDirection);

   /// [PHYSICAL] Search for all physical files first
   performPhysicalFileSystemSearch(pFileSystem, szFolder, eFilter, pProgress, pSearchResults);

   // [CHECK] Ensure path is within the VFS
   if (isPathSubFolderOf(pFileSystem->szGameFolder, szFolder))
   {
      // [STAGE] Move to the 'Virtual Search' stage
      if (pProgress)
         ASSERT(advanceOperationProgressStage(pProgress) == IDS_PROGRESS_SEARCHING_VIRTUAL);

      /// [VIRTUAL] Populate any remaining matches from the VFS
      performVirtualFileSystemSearch(pFileSystem, szFolder, eFilter, pProgress, pSearchResults);
   }
   
   /// Index search results tree
   performAVLTreeIndexing(pSearchResults->pResultsTree);

   // Cleanup and return result
   return pSearchResults;
}


/// Function name  : saveDocumentToFileSystem
// Description     : Save a GameFile document prepared by one of the generation functions to a physical file
// 
// CONST TCHAR*  szFullPath  : [in]  Full filepath of the location to save the file
// GAME_FILE*    pOutput     : [in]  GameFile with a valid output buffer to be saved to disk
// ERROR_QUEUE*  pErrorQueue : [out] Error message, if any
// 
// Return Value   : Number of bytes written
// 
UINT  saveDocumentToFileSystem(CONST TCHAR*  szFullPath, GAME_FILE*  pOutput, ERROR_QUEUE*  pErrorQueue)
{
   RAW_FILE*  pFileBuffer;     // ANSI buffer to hold the input document
   UINT       iBytesWritten;   // Number of bytes written to disk

   __try
   {
      // Prepare
      iBytesWritten = NULL;

      // [CHECK] Output buffer is not empty
      ASSERT(hasOutputData(pOutput));

      /// Attempt to convert output buffer to ANSI
      if (pFileBuffer = (RAW_FILE*)utilTranslateStringToANSI(pOutput->szOutputBuffer, pOutput->iOutputSize))
         /// [SUCCESS] Output file as ANSI
         iBytesWritten = saveRawFileToFileSystem(szFullPath, pFileBuffer, pOutput->iOutputSize, pErrorQueue);
      else
         // [ERROR] "Unable to convert document contents from UNICODE to ANSI"
         generateQueuedError(pErrorQueue, HERE(IDS_UNICODE_CONVERSION_ANSI_FAILED));

      // Cleanup and return
      deleteRawFileBuffer(pFileBuffer);
      return iBytesWritten;
   }
   PUSH_CATCH(pErrorQueue)
   {
      EXCEPTION1("szFullPath=%s", szFullPath);
      return 0;
   }
}


/// Function name  : saveDocumentToFileSystemEx
// Description     : Saves a GameFile to disc. If the extension is .PCK, the file will be compressed
// 
// CONST TCHAR*         szFullPath  : [in]     Full filepath of the location to save the file
// GAME_FILE*           pOutput     : [in]     GameFile with a valid output buffer to be saved to disk
// OPERATION_PROGRESS*  pProgress   : [in/out] Progress tracker with stage already set. (Only used during compression)
// ERROR_QUEUE*         pErrorQueue : [out]    Error message, if any
// 
// Return Value   : Number of bytes written
// 
UINT  saveDocumentToFileSystemEx(CONST TCHAR*  szFullPath, GAME_FILE*  pOutput, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue)
{
   RAW_FILE  *pUncompressedFile,     // ANSI version of document output buffer
             *pCompressedFile;       // Compressed ANSI version of document output buffer
   TCHAR     *szFileName;            // Filename within the PCK archive
   UINT       iOutputLength,         // Length of (Uncompressed/Compressed) ANSI output buffer
              iBytesWritten;         // Number of bytes written to disc
              
   __try
   {
      // [CHECK] Output buffer is not empty
      ASSERT(hasOutputData(pOutput));

      // Prepare
      pUncompressedFile = NULL;
      pCompressedFile   = NULL;
      iBytesWritten     = NULL;

      // Set initial length
      iOutputLength = pOutput->iOutputSize;

      /// Generate ANSI copy of document output buffer
      if (pUncompressedFile = (RAW_FILE*)utilTranslateStringToANSI(pOutput->szOutputBuffer, iOutputLength))
      {
         // [CHECK] Are we saving as PCK?
         if (utilCompareString(PathFindExtension(szFullPath), ".pck"))
         {
            // Generate filename (without extension)
            szFileName = utilDuplicateSimpleString(PathFindFileName(szFullPath));
            utilFindCharacterReverse(szFileName, '.')[0] = NULL;

            /// [PCK] Generate compressed copy of ANSI buffer
            iOutputLength = performGZipFileCompression(szFileName, pUncompressedFile, iOutputLength, pCompressedFile, pProgress, pErrorQueue);

            // Cleanup
            utilDeleteString(szFileName);
         }

         // [CHECK] Was compression successful?
         if (iOutputLength)
            /// [SUCCESS] Output compressed/uncompressed file as ANSI
            iBytesWritten = saveRawFileToFileSystem(szFullPath, utilEither(pCompressedFile, pUncompressedFile), iOutputLength, pErrorQueue);
      }
      else
         // [ERROR] "Unable to convert document contents from UNICODE to ANSI"
         generateQueuedError(pErrorQueue, HERE(IDS_UNICODE_CONVERSION_ANSI_FAILED));

      // Cleanup
      if (pCompressedFile)
         deleteRawFileBuffer(pCompressedFile);
      deleteRawFileBuffer(pUncompressedFile);
      // Return number of bytes written
      return iBytesWritten;
   }
   PUSH_CATCH(pErrorQueue)
   {
      EXCEPTION1("szFullPath=%s", szFullPath);
      debugGameFile(pOutput);
      return NULL;
   }
}

/// Function name  : saveRawFileToFileSystem
// Description     : Create a new physical file from an ANSI file buffer
// 
// CONST TCHAR*    szFullPath    : [in]  Full filepath of the target file
// CONST RAW_FILE* szInput       : [in]  ANSI file buffer containing the file
// CONST UINT      iInputLength  : [in]  Length of file buffer, in bytes
// ERROR_QUEUE*    pErrorQueue   : [out] Error message, if any 
// 
// Return Value   : Number of bytes written
// 
UINT  saveRawFileToFileSystem(CONST TCHAR*  szFullPath, CONST RAW_FILE*  szInput, CONST UINT  iInputLength, ERROR_QUEUE*  pErrorQueue)
{
   HANDLE   hFile;            // File handle
   BOOL     bResult;          // Whether write operation was successful or not 
   UINT     iBytesWritten;    // Number of bytes written
   TCHAR*   szSystemError;    // GetLastError() result, if any
   
   // Prepare
   iBytesWritten = NULL;

   /// Attempt to create/overwrite the target file
   if ((hFile = CreateFile(szFullPath, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)) != INVALID_HANDLE_VALUE)
   {
      /// Output the buffer and close the file
      bResult = WriteFile(hFile, szInput, iInputLength, (DWORD*)&iBytesWritten, NULL);
      utilCloseHandle(hFile);
   }

   // [ERROR] Convert the system error to an error queue message
   if (hFile == INVALID_HANDLE_VALUE OR !bResult)
   {
      // Get system error
      szSystemError = utilCreateEmptyString(ERROR_LENGTH);
      FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), NULL, szSystemError, ERROR_LENGTH, NULL);

      // [CHECK] Remove new-lines from system errors
      if (utilFindCharacterInSet(szSystemError, "\r\n"))
         utilFindCharacterInSet(szSystemError, "\r\n")[0] = NULL;

      // [ERROR] "There was an IO error while saving '%s', the error is '%s'"
      generateQueuedError(pErrorQueue, HERE(IDS_SAVE_FILE_SYSTEM_ERROR), szFullPath, szSystemError);
      // Cleanup
      utilDeleteString(szSystemError);
   }

   // Return number of bytes written
   return iBytesWritten;
}






