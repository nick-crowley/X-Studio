//
// Virtual Files.cpp : Handles FileItems and VirtualFiles
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

/// Function name  : createCatalogueFile
// Description     : Creates a CatalogueFile
// 
// CONST UINT    iID        : [in] Catalogue ID
// CONST TCHAR*  szFullPath : [in] Full path to the catalogue
// 
// Return Value   : New CatalogueFile, you are responsible for destroying it
// 
CATALOGUE_FILE*  createCatalogueFile(CONST UINT  iID, CONST TCHAR*  szFullPath)
{
   CATALOGUE_FILE*  pCatalogue;

   // Create object
   pCatalogue = utilCreateObject(CATALOGUE_FILE);

   // Delete path
   pCatalogue->szFullPath = utilDuplicateSimpleString(szFullPath);
   pCatalogue->iCatalogue = iID;

   // Return object
   return pCatalogue;
}


/// Function name  : createFileItemFromPhysical
// Description     : Create a new FileItem from the result of a physical file search
// 
// CONST TCHAR*      szFolder   : [in] Folder containing the file. Must have a trailing backslash.
// WIN32_FIND_DATA*  pFileData  : [in] Physical file search result data
// 
// Return Value   : New FileItem object, you are responsible for destroying it
// 
FILE_ITEM*   createFileItemFromPhysical(CONST TCHAR*  szFolder, CONST WIN32_FIND_DATA*  pFileData)
{
   SHFILEINFO    oShellInfo;   // Shell data for the item
   FILE_ITEM*    pOutput;      // New FileItem being created

   // Prepare
   pOutput = utilCreateEmptyObject(FILE_ITEM);

   // Build item's full path
   StringCchPrintf(pOutput->szFullPath, MAX_PATH, TEXT("%s%s"), szFolder, pFileData->cFileName);

   /// Retrieve main icon index, overlay icon index and file type string
   SHGetFileInfo(pOutput->szFullPath, NULL, &oShellInfo, sizeof(SHFILEINFO), SHGFI_TYPENAME WITH SHGFI_ICON WITH SHGFI_SMALLICON WITH SHGFI_SYSICONINDEX WITH SHGFI_OVERLAYINDEX);      //SHGFI_ATTRIBUTES
   if (oShellInfo.hIcon)
      DestroyIcon(oShellInfo.hIcon);

   // Set DISPLAY NAME and TYPE
   StringCchCopy(pOutput->szDisplayName, MAX_PATH, pFileData->cFileName);
   StringCchCopy(pOutput->szDisplayType, 80, oShellInfo.szTypeName);
   
   // Set FILE SIZE, ICON
   pOutput->iFileSize  = (UINT)pFileData->nFileSizeLow;
   pOutput->iIconIndex = oShellInfo.iIcon;

   // Convert ATTRIBUTES
   pOutput->iAttributes = FIF_PHYSICAL;
   pOutput->eGameVersion = GV_UNKNOWN;

   // Set SHORTCUT attribute. Remove the '.lnk' from shortcut display names
   if (utilCompareString(PathFindExtension(pOutput->szFullPath), ".lnk"))
   {
      pOutput->iAttributes |= FIF_SHORTCUT;
      pOutput->iIconIndex   = NULL;
      PathRemoveExtension(pOutput->szDisplayName);
   }
   // Set FOLDER and SYSTEM attributes
   if (pFileData->dwFileAttributes INCLUDES FILE_ATTRIBUTE_DIRECTORY)
      pOutput->iAttributes |= FIF_FOLDER;
   if (pFileData->dwFileAttributes INCLUDES FILE_ATTRIBUTE_SYSTEM)
      pOutput->iAttributes |= FIF_SYSTEM;

   return pOutput;
}


/// Function name  : createFileItemFromVirtualFile
// Description     : Create a new FileItem object from a VirtualFile object
// 
// CONST VIRTUAL_FILE*  pVirtualFile  : [in] VirtualFile object representing a file in the game catalogues
// 
// Return Value   : New FileItem object, you are responsible for destroying it
// 
FILE_ITEM*   createFileItemFromVirtualFile(CONST VIRTUAL_FILE*  pVirtualFile)
{
   SHFILEINFO    oShellInfo;   // Shell data for the item
   FILE_ITEM*    pOutput;      // New FileItem being created

   // Prepare
   pOutput = utilCreateEmptyObject(FILE_ITEM);

   // Use the game folder to build the FULL PATH.  Use the file/folder name as the DISPLAY NAME
   StringCchPrintf(pOutput->szFullPath, MAX_PATH, TEXT("%s%s"), getAppPreferences()->szGameFolder, pVirtualFile->szFullPath);
   StringCchCopy(pOutput->szDisplayName, MAX_PATH, PathFindFileName(pOutput->szFullPath));

   // [FOLDER] Lookup TYPE + ICON of a pre-defined folder
   if (pVirtualFile->iAttributes INCLUDES FIF_FOLDER)
      SHGetFileInfo(pOutput->szDisplayName, FILE_ATTRIBUTE_DIRECTORY, &oShellInfo, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX WITH SHGFI_SMALLICON WITH SHGFI_USEFILEATTRIBUTES WITH SHGFI_TYPENAME);
   // [FILE] Lookup TYPE + ICON using extension
   else
      SHGetFileInfo(PathFindExtension(pOutput->szDisplayName), FILE_ATTRIBUTE_NORMAL, &oShellInfo, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX WITH SHGFI_SMALLICON WITH SHGFI_USEFILEATTRIBUTES WITH SHGFI_TYPENAME);

   // Store DISPLAY TYPE and ICON
   StringCchCopy(pOutput->szDisplayType, 80, oShellInfo.szTypeName);
   pOutput->iIconIndex    = extractShellIcon(oShellInfo.iIcon);
   pOutput->iOverlayIndex = extractShellOverlayIcon(oShellInfo.iIcon);

   // Set GameVersion
   pOutput->eGameVersion = GV_UNKNOWN;

   // Store FILE SIZE and add VIRTUAL flag
   pOutput->iFileSize   = pVirtualFile->iPackedSize;
   pOutput->iAttributes = pVirtualFile->iAttributes WITH FIF_VIRTUAL;

   // Return new object
   return pOutput;
}


/// Function name  : createRawFileBuffer
// Description     : Create an empty BYTE array for use as a file buffer
// 
// CONST UINT  iBytes : [in] Number of bytes to allocate
// 
// Return Value   : New raw file buffer, you are responsible for destroying it
// 
RAW_FILE*  createRawFileBuffer(CONST UINT  iBytes)
{
   RAW_FILE*  pNewBuffer;

   // Create (and erase) new BYTE buffer
   pNewBuffer = utilCreateEmptyObjectArray(BYTE, iBytes);

   return pNewBuffer;
}


/// Function name  : createVirtualFile
// Description     : Create a new VirtualFile object representing a virtual file within a game catalogue
// 
// CONST UINT  iCatalogueID : [in] Catalogue number identifying the data file containing the file
// CHAR*       szFilePathA  : [in] SubPath of file
// UINT        iDataOffset  : [in] Offset into the data file marking the position of the file, in bytes
// CONST UINT  iPackedSize  : [in] Length of the file (within the data file), in bytes
// 
// Return Value   : New VirtualFile object, you are responsible for destroying it
// 
VIRTUAL_FILE*  createVirtualFile(CONST UINT  iCatalogueID, CHAR*  szFilePathA, UINT  iDataOffset, CONST UINT  iPackedSize)
{
   VIRTUAL_FILE*  pNewFile;

   // Create new file
   pNewFile = utilCreateEmptyObject(VIRTUAL_FILE);

   // Set properties
   pNewFile->iCatalogueID = iCatalogueID;
   pNewFile->iDataOffset  = iDataOffset;
   pNewFile->iPackedSize  = iPackedSize;
   pNewFile->szFullPath   = utilTranslateStringToUNICODE(szFilePathA, lstrlenA(szFilePathA));

   // Return new file
   return pNewFile;
}

/// Function name  : createVirtualFolder
// Description     : Create a new VirtualFile object representing a virtual folder within a game catalogue
// 
// CHAR*  szFolderPathA : [in] SubPath of folder
// 
// Return Value   : New VirtualFile object, you are responsible for destroying it
// 
VIRTUAL_FILE*  createVirtualFolder(CHAR*  szFolderPathA)
{
   VIRTUAL_FILE*  pNewFolder;

   // Create new folder
   pNewFolder = utilCreateEmptyObject(VIRTUAL_FILE);

   // Set properties
   pNewFolder->szFullPath  = utilTranslateStringToUNICODE(szFolderPathA, lstrlenA(szFolderPathA));
   pNewFolder->iAttributes = FIF_FOLDER;

   // Return new folder
   return pNewFolder;
}


/// Function name  : createVirtualFileTreeByPageID
// Description     : Creates an VirtualFile tree sorted by file path in ascending order
// 
// Return Value   : New AVLTree, you are responsible for destroying it
// 
AVL_TREE*  createVirtualFileTreeByPath()
{
   return createAVLTree(extractVirtualFileTreeNode, deleteVirtualFileTreeNode, createAVLTreeSortKey(AK_PATH, AO_ASCENDING), NULL, NULL);
}


/// Function name  : deleteCatalogueFile
// Description     : Destroys a CatalogueFile
// 
// CATALOGUE_FILE*  &pCatalogue   : [in] CatalogueFile
// 
VOID  deleteCatalogueFile(CATALOGUE_FILE*  &pCatalogue)
{
   // Delete path
   utilDeleteString(pCatalogue->szFullPath);

   // Delete calling object
   utilDeleteObject(pCatalogue);
}

/// Function name  : destructorCatalogue
// Description     : Destroys a CatalogueFile stored in a list
// 
// LPARAM  pCatalogue   : [in] CatalogueFile
// 
VOID  destructorCatalogue(LPARAM  pCatalogue)
{
   // Delete item
   deleteCatalogueFile((CATALOGUE_FILE*&)pCatalogue);
}


/// Function name  : deleteFileItem
// Description     : Delete a FileItem object
// 
// FILE_ITEM*  pFileItem   : [in] 
// 
VOID   deleteFileItem(FILE_ITEM*  &pFileItem)
{
   // Delete calling object
   utilDeleteObject(pFileItem);
}


/// Function name  : deleteFileItemTreeNode
// Description     : Deletes a FileItem within an AVLTree Node
// 
// LPARAM  pData : [in] Reference to a FileItem pointer
// 
VOID   deleteFileItemTreeNode(LPARAM  pData)
{
   // Delete node contents
   deleteFileItem((FILE_ITEM*&)pData);
}

/// Function name  : deleteRawFileBuffer
// Description     : Destroys a raw file buffer
// 
// RAW_FILE*  &pFileBuffer   : [in] Raw file buffer to delete
// 
VOID  deleteRawFileBuffer(RAW_FILE*  &pFileBuffer)
{
   // Delete calling object
   utilDeleteObject(pFileBuffer);
}


/// Function name  : deleteVirtualFile
// Description     : Destroy a VirtualFile
// 
// VIRTUAL_FILE*  &pVirtualFile   : [in] VirtualFile object to destroy
// 
VOID  deleteVirtualFile(VIRTUAL_FILE*  &pVirtualFile)
{
   // Delete file path
   utilDeleteString(pVirtualFile->szFullPath);

   // Delete calling object
   utilDeleteObject(pVirtualFile);
}


/// Function name  : deleteVirtualFileTreeNode
// Description     : Deletes a VirtualFile within an AVLTree Node
// 
// LPARAM  pData : [in] Reference to a VirtualFile pointer
// 
VOID   deleteVirtualFileTreeNode(LPARAM  pData)
{
   // Delete node contents
   deleteVirtualFile((VIRTUAL_FILE*&)pData);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : extractFileItemTreeNode
// Description     : Extract the desired property from a given TreeNode containing a FileItem
// 
// LPARAM                   pNode      : [in] FILE_ITEM* : Node data containing a FileItem
// CONST AVL_TREE_SORT_KEY  eProperty  : [in] Property to retrieve
// 
// Return Value   : Value of the specified property or NULL if the property is unsupported
// 
LPARAM  extractFileItemTreeNode(LPARAM  pNode, CONST AVL_TREE_SORT_KEY  eProperty)
{
   FILE_ITEM*  pFileItem;    // Convenience pointer
   LPARAM      xOutput;      // Property value output

   // Prepare
   pFileItem = (FILE_ITEM*)pNode;

   // Examine property
   switch (eProperty)
   {
   // Internal
   case AK_PATH:     xOutput = (LPARAM)pFileItem->szFullPath;      break;
   case AK_GROUP:    xOutput = (LPARAM)(pFileItem->iAttributes INCLUDES FIF_FOLDER);      break;

   // File dialog
   case AK_NAME:     xOutput = (LPARAM)pFileItem->szDisplayName;   break;
   case AK_TEXT:     xOutput = (LPARAM)pFileItem->szDescription;   break;
   case AK_TYPE:     xOutput = (LPARAM)pFileItem->szDisplayType;   break;
   case AK_VERSION:  xOutput = (LPARAM)pFileItem->iFileVersion;    break;
   case AK_COUNT:    xOutput = (LPARAM)pFileItem->iFileSize;       break;
   
   // [ERROR]
   default:          xOutput = NULL; ASSERT(FALSE);                break;
   }

   // Return property value
   return xOutput;
}


/// Function name  : extractVirtualFileTreeNode
// Description     : Extract the desired property from a given TreeNode containing a VirtualFile
// 
// LPARAM                   pNode      : [in] VIRTUAL_FILE* : Node data containing a VirtualFile
// CONST AVL_TREE_SORT_KEY  eProperty  : [in] Property to retrieve
// 
// Return Value   : Value of the specified property or NULL if the property is unsupported
// 
LPARAM  extractVirtualFileTreeNode(LPARAM  pNode, CONST AVL_TREE_SORT_KEY  eProperty)
{
   VIRTUAL_FILE*  pVirtualFile;    // Convenience pointer
   LPARAM         xOutput;        // Property value output

   // Prepare
   pVirtualFile = (VIRTUAL_FILE*)pNode;

   // Examine property
   switch (eProperty)
   {
   case AK_PATH:     xOutput = (LPARAM)pVirtualFile->szFullPath;    break;
   default:          xOutput = NULL;                                break;
   }

   // Return property value
   return xOutput;
}



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////


