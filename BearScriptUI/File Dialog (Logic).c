//
// File Dialog (Base).cpp : Logic behind the FileDialog
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          MACROS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        CONSTANTS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// ListView Column IDs
#define       FILE_COLUMN_NAME              0
#define       FILE_COLUMN_DESCRIPTION       1
#define       FILE_COLUMN_VERSION           2
#define       FILE_COLUMN_TYPE              3
#define       FILE_COLUMN_SIZE              4

// Number of jumpbar buttons
CONST UINT     iJumpBarItemCount           = 6;

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                     CREATION / DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createFileDialogData
// Description     : Creates data for the FileDialog which specified which type of dialog to create
// 
// CONST FILE_DIALOG_TYPE  eType             : [in]           The type of FileDialog to create
// CONST TCHAR*            szInitialFolder   : [in][optional] The initial folder to display (NULL will display the desktop)
// CONST TCHAR*            szInitialFileName : [in][optional] The initial filename to display (NULL will display no filename)
// 
// Return Value   : New FileDialog data, you are responsible for destroying it
// 
FILE_DIALOG_DATA*  createFileDialogData(CONST FILE_DIALOG_TYPE  eType, CONST TCHAR*  szInitialFolder, CONST TCHAR*  szInitialFileName)
{
   FILE_DIALOG_DATA*  pDialogData;     // New dialog data

   // Create new data
   pDialogData = utilCreateEmptyObject(FILE_DIALOG_DATA);

   // Set properties
   pDialogData->eType = eType;

   // Create StoredDocument output list
   pDialogData->pOutputFileList = createList(destructorStoredDocument);
   
   /// [CHECK] Determine the initial folder
   if (lstrlen(szInitialFolder))
      // [LAST FOLDER] Use the last used folder
      StringCchCopy(pDialogData->szFolder, MAX_PATH, szInitialFolder);
   else if (lstrlen(getAppPreferences()->szGameFolder))
      // [GAME FOLDER] Use the game folder if no initial folder is provided
      StringCchCopy(pDialogData->szFolder, MAX_PATH, getAppPreferences()->szGameFolder);
   else
      // [DESKTOP] Use the desktop when neither are provided
      SHGetSpecialFolderPath(NULL, pDialogData->szFolder, CSIDL_DESKTOPDIRECTORY, FALSE);

   /// Ensure initial folder has trailing backslash
   PathAddBackslash(pDialogData->szFolder);

   /// [OPTIONAL] Set the inital filename
   if (szInitialFileName)
      StringCchCopy(pDialogData->szFileName, MAX_PATH, szInitialFileName);

   // Return new object
   return pDialogData;
}


/// Function name  : createJumpBarItem
// Description     : Create a new JumpBar item by manually specifying all the properties of a JumpBar item
//                     This is used for creating buttons for the GameFolder and the ScriptFolder
// 
// HWND          hCtrl          : [in] Window handle of the control associated with the item
// CONST TCHAR*  szPath         : [in] Full destination path
// CONST TCHAR*  szDisplayName  : [in] Display name
// HICON         hIcon          : [in] Display icon
// 
// Return Value   : New JumpBar item, you are responsible for destroying it
// 
JUMPBAR_ITEM*  createJumpBarItem(HWND  hCtrl, CONST TCHAR*  szPath, CONST TCHAR*  szDisplayName, HIMAGELIST  hImageList, CONST UINT  iIconIndex)
{
   JUMPBAR_ITEM*  pNewItem;      // New jumpbar item

   /// Create JumpBar item
   pNewItem = utilCreateEmptyObject(JUMPBAR_ITEM);

   // Set properties
   pNewItem->hCtrl      = hCtrl;
   pNewItem->szText     = utilDuplicateSimpleString(szDisplayName);
   pNewItem->szPath     = utilDuplicateSimpleString(szPath);
   pNewItem->iIconIndex = iIconIndex;
   pNewItem->hImageList = hImageList;
   pNewItem->bValid     = TRUE;

   // Return
   return pNewItem;
}

/// Function name  : createJumpBarItemFromLocation
// Description     : Create a new JumpBar item from a special folder CLSID or path
// 
// HWND         hCtrl   : [in] Button control represented by this JumpItem
// CONST UINT   iCLSID  : [in][optional] CLSID of the folder to represent. *Only used if szPath is NULL*
// CONST TCHAR* szPath  : [in][optional] Path of the folder to represent. *If specified then iCLSID is ignored*
// 
// Return Value   : New JumpBar item, you are responsible for destroying it
// 
JUMPBAR_ITEM*  createJumpBarItemFromLocation(HWND  hCtrl, CONST UINT  iCLSID, CONST TCHAR*  szPath)
{
   JUMPBAR_ITEM*  pOutput;           // New item
   SHFILEINFO     oShellData;        // Shell data for the current folder
   ITEMIDLIST*    pIDList;           // IDList for the current folder
   BOOL           bVirtualFolder;    // TRUE if CLSID represents a virtual item in the namespace (eg. my computer, printers)
   
   // Prepare
   utilZeroObject(&oShellData, SHFILEINFO);

   /// Create JumpBar item
   pOutput         = utilCreateEmptyObject(JUMPBAR_ITEM);
   pOutput->szPath = utilCreateEmptyPath();
   pOutput->szText = utilCreateEmptyPath();
   pOutput->hCtrl  = hCtrl;

   /// [PATH] Store path and use it to create an IDList.
   if (szPath)
   {
      StringCchCopy(pOutput->szPath, MAX_PATH, szPath);
      pIDList = SHSimpleIDListFromPath(pOutput->szPath);
   }
   /// [CLSID] Convert CLSID -> IDList -> Path
   else
   {
      SHGetSpecialFolderLocation(NULL, iCLSID, &pIDList);
      SHGetPathFromIDList(pIDList, pOutput->szPath);
   }

   // Convert IDList -> Display Name
   SHGetFileInfo((CONST TCHAR*)pIDList, NULL, &oShellData, sizeof(SHFILEINFO), SHGFI_PIDL WITH SHGFI_DISPLAYNAME);
   StringCchCopy(pOutput->szText, MAX_PATH, oShellData.szDisplayName);
   
   // Determine whether folder exists or is a virtual component of the namespace. eg. My Computer
   bVirtualFolder = (lstrlen(pOutput->szPath) == 0);

   // [VIRTUAL] Convert IDList -> Icon
   if (bVirtualFolder)
   {
      pOutput->hImageList = (HIMAGELIST)SHGetFileInfo((CONST TCHAR*)pIDList, NULL, &oShellData, sizeof(SHFILEINFO), SHGFI_PIDL WITH SHGFI_ICON WITH SHGFI_LARGEICON WITH SHGFI_SYSICONINDEX);
      pOutput->iIconIndex = oShellData.iIcon;
      // Cleanup
      DestroyIcon(oShellData.hIcon);
   }
   // [NON-VIRTUAL] Convert Path -> Icon
   else
   {
      PathAddBackslash(pOutput->szPath);
      pOutput->hImageList = (HIMAGELIST)SHGetFileInfo(pOutput->szPath, NULL, &oShellData, sizeof(SHFILEINFO), SHGFI_ICON WITH SHGFI_LARGEICON WITH SHGFI_SYSICONINDEX);
      pOutput->iIconIndex = oShellData.iIcon;
      // Cleanup
      DestroyIcon(oShellData.hIcon);
   }
   
   // [CHECK] Disable button for network and windows virtual folders
   pOutput->bValid = !PathIsNetworkPath(pOutput->szPath) AND !bVirtualFolder;

   // Cleanup and return
   CoTaskMemFree(pIDList);
   return pOutput;
}


/// Function name  : deleteFileDialogData
// Description     : Delete a FileDialog data object
// 
// FILE_DIALOG_DATA*  pDialogData   : [in] Existing FileDialog object to destroy
// 
VOID  deleteFileDialogData(FILE_DIALOG_DATA*  pDialogData)
{
   /// Destroy FileSearch
   if (pDialogData->pFileSearch)
      deleteFileSearch(pDialogData->pFileSearch);

   /// Delete SuggestionTree
   if (pDialogData->pSuggestionTree)
      deleteAVLTree(pDialogData->pSuggestionTree);

   /// Destroy JumpBar item data
   for (UINT iItem = 0; iItem < iJumpBarItemCount; iItem++)
      if (pDialogData->pJumpBarItems[iItem])
         deleteJumpBarItem(pDialogData->pJumpBarItems[iItem]);

   /// Destroy output list
   deleteList(pDialogData->pOutputFileList);

   // Delete calling object
   utilDeleteObject(pDialogData);
}


/// Function name  : deleteJumpBarItem
// Description     : Deletes an existing JumpBar item
// 
// JUMPBAR_ITEM*  &pItem   : [in] JumpBar item to delete
// 
VOID  deleteJumpBarItem(JUMPBAR_ITEM*  &pItem)
{
   // Delete strings
   utilDeleteStrings(pItem->szText, pItem->szPath);

   // Delete calling object
   utilDeleteObject(pItem);
}


/// Function name  : deleteFileDialogNamespace
// Description     : Erase the contents of the locations combo (which stores paths as item data)
// 
// FILE_DIALOG_DATA*  pDialogData  : [in] 
// 
VOID   deleteFileDialogNamespace(FILE_DIALOG_DATA*  pDialogData)
{
   TCHAR*   szPath;
   INT      iItemCount;

   // Prepare
   iItemCount = ComboBox_GetCount(pDialogData->hLocationCombo);

   // Delete the path stored as parameter data in each item
   for (INT iItem = 0; iItem < iItemCount; iItem++)
   {
      // [CHECK] Does item have a path?  ('My Computer' will have no path)
      if (szPath = (TCHAR*)getCustomComboBoxItemParam(pDialogData->hLocationCombo, iItem))
         // [FOUND] Delete item
         utilDeleteString(szPath);
   }

   // Remove all items
   ComboBox_ResetContent(pDialogData->hLocationCombo);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                           HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : convertFileDialogSortColumnToSortKey
// Description     : Convert the column ID into the matching binary tree sort key
// 
// CONST UINT  iSortColumnID : [in] Zero based column index
// 
// Return Value   : Equivilent AVL tree sort key
// 
AVL_TREE_SORT_KEY  convertFileDialogSortColumnToSortKey(CONST UINT  iSortColumnID)
{
   AVL_TREE_SORT_KEY     eSortKey;    // Output sort key
 
   // Convert selected column into appropriate sort key
   switch (iSortColumnID)
   {
   case FILE_COLUMN_NAME:          eSortKey = AK_PATH;         break;
   case FILE_COLUMN_DESCRIPTION:   eSortKey = AK_TEXT;         break;
   case FILE_COLUMN_VERSION:       eSortKey = AK_VERSION;      break;
   case FILE_COLUMN_TYPE:          eSortKey = AK_TYPE;        break;
   case FILE_COLUMN_SIZE:          eSortKey = AK_COUNT;        break;
   }

   return eSortKey;
}


/// Function name  : findFileDialogOutputFileByIndex
// Description     : Convenience access to output list of FileDialog
// 
// CONST FILE_DIALOG_DATA*  pDialogData : [in]  Dialog data
// CONST UINT               iIndex      : [in]  Zero based file index
// STORED_DOCUMENT*        &pOutput     : [out] StoredDocument representing file
// 
// Return Value   : TRUE if found, otherwise FALSE
// 
BOOL  findFileDialogOutputFileByIndex(CONST FILE_DIALOG_DATA*  pDialogData, CONST UINT  iIndex, STORED_DOCUMENT*  &pOutput)
{
   return findListObjectByIndex(pDialogData->pOutputFileList, iIndex, (LPARAM&)pOutput);
}


/// Function name  : getFileDialogData
// Description     : Retrieve FileDialog data assocaited with the window
// 
// HWND  hDialog   : [in] FileDialog window handle
// 
// Return Value   : FileDialog window data
// 
FILE_DIALOG_DATA*  getFileDialogData(HWND  hDialog)
{
   return (FILE_DIALOG_DATA*)GetWindowLong(hDialog, DWL_USER);
}


/// Function name  : getGameVersionIconHandle
// Description     : Loads the appropriate game version icon and returns the handle
// 
// CONST GAME_VERSION  eVersion : [in] GameVersion to retrieve the icon for
// 
// Return Value   : Icon resource handle
// 
HICON  getGameVersionIconHandle(CONST GAME_VERSION  eVersion)
{
   CONST TCHAR*   szIconName;    // Icon resource name
   
   // Examine game version
   switch (eVersion)
   {
   case GV_THREAT:            szIconName = TEXT("THREAT_ICON");           break;
   case GV_REUNION:           szIconName = TEXT("REUNION_ICON");          break;
   case GV_TERRAN_CONFLICT:   szIconName = TEXT("TERRAN_CONFLICT_ICON");  break;
   case GV_ALBION_PRELUDE:    szIconName = TEXT("ALBION_PRELUDE_ICON");   break;
   case GV_UNKNOWN:           ASSERT(FALSE);
   }

   // Return resource
   return LoadIcon(getResourceInstance(), szIconName);
}


/// Function name  : identifyDefaultFileExtension
// Description     : Identifies the default extension related to a specific file filter  (except 'All Files')
// 
// CONST FILE_FILTER  eFilter : [in] FileFilter
// 
// Return Value   : Default file extension, in '.abc' format
// 
CONST TCHAR*  identifyDefaultFileExtension(CONST FILE_FILTER  eFilter)
{
   CONST TCHAR*  szOutput;

   // Examine filter
   switch (eFilter)
   {
   /// [SCRIPT/XML-SCRIPT/LANGUAGE/MISSION/DOCUMENT] All default to .xml
   case FF_SCRIPT_FILES:      
   case FF_SCRIPT_XML_FILES:  
   case FF_LANGUAGE_FILES:    
   case FF_MISSION_FILES:     
   case FF_DOCUMENT_FILES:    szOutput = TEXT(".xml");      break;

   /// [PCK-SCRIPT] Only one to default to .pck
   case FF_SCRIPT_PCK_FILES:  szOutput = TEXT(".pck");      break;

   /// [PROJECTS] Uses .xprj
   case FF_PROJECT_FILES:     szOutput = TEXT(".xprj");     break;

   /// [ALL FILES] Error: Has no default extension
   case FF_ALL_FILES:         
      ASSERT(FALSE); 
      break;
   }

   // Return extension
   return szOutput;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : appendFileDialogNamespaceTraversal
// Description     : Adds the sub-folders between the specified 'parent' folder and the file dialog's current folder
//                    to the end of the Locations combo box with increased indentation for every item.
// 
// FILE_DIALOG_DATA*  pDialogData    : [in] FileDialog window data
// CONST TCHAR*       szParent       : [in] 'Parent' of the first sub-folder to be added.  ie. This folder is not added but it's children are.
// UINT               iInitialIndent : [in] Indentation for the first item.
// 
VOID  appendFileDialogNamespaceTraversal(FILE_DIALOG_DATA*  pDialogData, CONST TCHAR*  szParent, UINT  iInitialIndent)
{
   TCHAR  *szFolderCopy,   // Copy of the 'current folder'
          *szMarker;       // String iterator, marks the position of backslashes
   INT     iReplaced;      // Number of backslashes replaced in the copy of the 'current folder'

   // Copy the current folder to an empty buffer
   szFolderCopy = utilCreateEmptyPath();
   StringCchCopy(szFolderCopy, MAX_PATH, pDialogData->szFolder);

   /// Replace all the backslashes with NULL terminators
   iReplaced = 0;
   while (szMarker = utilFindCharacterReverse(szFolderCopy, '\\'))
   {
      szMarker[0] = NULL;
      iReplaced++;
   }

   /// Replace the backslashes one at a time. Add the resultant folder if its a child of the current folder.
   szMarker = szFolderCopy;
   while (iReplaced-- > 0)
   {
      if (isPathSubFolderOf(szParent, szMarker))
         appendFileDialogNamespaceFolder(pDialogData, NULL, szMarker, iInitialIndent++);
      // Replace a backslash
      szMarker[lstrlen(szMarker)] = '\\';
   }

   // Cleanup
   utilDeleteString(szFolderCopy);
}


/// Function name  : appendFileDialogNamespaceFolder
// Description     : Append a location defined by either a CLSID or a path to the locations combo box
// 
// FILE_DIALOG_DATA*  pDialogData : [in]           FileDialog data
// CONST UINT         iCLSID      : [in][optional] CLSID of the location to add. (Ignored if szPath is specified)
// CONST TCHAR*       szPath      : [in][optional] Full path of the location to add. (If specified CLSID is ignored)
// CONST UINT         iIndent     : [in]           Indent for the new item
// 
VOID  appendFileDialogNamespaceFolder(FILE_DIALOG_DATA*  pDialogData, CONST UINT  iCLSID, CONST TCHAR*  szPath, CONST UINT  iIndent)
{
   SHFILEINFO        oShellInfo;        // Shell data used for retrieving the appropriate file type and icon
   TCHAR*            szFolderPath;      // Full path of the folder being inserted
   ITEMIDLIST*       pIDList;           // IDList of the folder being inserted (Used for My Computer, which is system-virtual)
   BOOL              bVirtualFolder;    // Whether folder is virtual
   INT               iNewItemIndex;     // Zero-based index of the new combobox item, used to selecting the 'current' folder

   // Prepare
   bVirtualFolder = FALSE;
   szFolderPath   = utilCreateEmptyPath();
   utilZeroObject(&oShellInfo, SHFILEINFO);

   /// [MY COMPUTER] Determine IDList from CLSID then get DISPLAY NAME and ICON
   if (iCLSID == CSIDL_DRIVES)
   {
      SHGetSpecialFolderLocation(NULL, iCLSID, &pIDList);
      SHGetFileInfo((LPCWSTR)pIDList, NULL, &oShellInfo, sizeof(SHFILEINFO), SHGFI_PIDL WITH SHGFI_SYSICONINDEX WITH SHGFI_SMALLICON WITH SHGFI_DISPLAYNAME);
      // Free IDList and path -- 'My Computer' stores NULL instead of a valid path
      CoTaskMemFree(pIDList);
   }
   /// [CLSID] Determine PATH from CLSID, then get DISPLAY NAME and ICON
   else if (szPath == NULL)
   {
      // Resolve PATH, DISPLAY-NAME and ICON
      SHGetSpecialFolderPath(NULL, szFolderPath, iCLSID, FALSE);
      SHGetFileInfo(szFolderPath, NULL, &oShellInfo, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX WITH SHGFI_SMALLICON WITH SHGFI_DISPLAYNAME);
      PathAddBackslash(szFolderPath);
   }
   /// [ANY FOLDER]
   else
   {
      // Ensure a trailing backslash
      StringCchCopy(szFolderPath, MAX_PATH, szPath);
      PathAddBackslash(szFolderPath);

      /// [GAME or VIRTUAL FOLDER] Specify DISPLAY NAME and ICON manually
      if (!PathIsRoot(szFolderPath) AND (utilCompareStringVariables(szFolderPath, getFileSystem()->szGameFolder) OR !PathFileExists(szFolderPath)))
      {
         // Prepare
         bVirtualFolder = TRUE;

         // Use relevant game icon from the system imagelist
         oShellInfo.iIcon = getSystemImageListGameVersionIcon(getAppPreferences()->eGameVersion);
         
         // [CHECK] Game Folder / Virtual Folder?
         if (utilCompareStringVariables(szFolderPath, getFileSystem()->szGameFolder))
            /// [GAME FOLDER] Display '<Game Version> VFS'
            StringCchPrintf(oShellInfo.szDisplayName, MAX_PATH, TEXT("%s VFS"), identifyGameVersionString(getAppPreferences()->eGameVersion));
         else
         {
            /// [VIRTUAL FOLDER] Extract folder name from path
            StringCchCopy(oShellInfo.szDisplayName, MAX_PATH, PathFindFileName(szFolderPath));
            PathRemoveBackslash(oShellInfo.szDisplayName);
         }
      }
      /// [PHYSICAL FOLDER] Retrieve DISPLAY NAME and ICON
      else
         SHGetFileInfo(szFolderPath, NULL, &oShellInfo, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX WITH SHGFI_SMALLICON WITH SHGFI_DISPLAYNAME);
   }

   // [CHECK] No icon handles should be being returned
   ASSERT(oShellInfo.hIcon == NULL);
  
   //oNewItem.iOverlay       = extractShellOverlayIcon(oShellInfo.iIcon);

   /// Append to the namespace Combo. Store it's combobox index
   iNewItemIndex = appendCustomComboBoxImageListItem(pDialogData->hLocationCombo, oShellInfo.szDisplayName, pDialogData->hImageList, extractShellIcon(oShellInfo.iIcon), iIndent, (LPARAM)szFolderPath);

   // [VIRTUAL] Display in blue
   if (bVirtualFolder)
      setCustomComboBoxItemColour(pDialogData->hLocationCombo, iNewItemIndex, RGB(0,0,255));

   // [CURRENT FOLDER] Select this item if it's the 'current folder'
   if (utilCompareStringVariables(szFolderPath, pDialogData->szFolder))
      ComboBox_SetCurSel(pDialogData->hLocationCombo, iNewItemIndex);
}


/// Function name  : calculateFileDialogTraversalParent
// Description     : Determine under which item the 'current folder traversal' should be placed
// 
// FILE_DIALOG_DATA*  pDialogData  : [in] FileDialog window data

// 
// Return Value   : Value indicating which item the traversal should be placed under, or TP_NONE if for some reason
//                   the current folder should not be displayed under any of the usual places. 
// 
TRAVERSAL_PARENT  calculateFileDialogTraversalParent(FILE_DIALOG_DATA*  pDialogData)
{
   TCHAR*           szSpecialFolder;
   TRAVERSAL_PARENT eOutput;

   // Prepare
   szSpecialFolder = utilCreateEmptyPath();
   eOutput         = TP_NONE;

   // Generate 'Desktop' path
   SHGetSpecialFolderPath(NULL, szSpecialFolder, CSIDL_DESKTOPDIRECTORY, FALSE);

   /// [CHECK] Is 'Current Folder' a child of the desktop?
   if (isPathSubFolderOf(szSpecialFolder, pDialogData->szFolder))
      eOutput = TP_DESKTOP;
   else
   {
      // Generate 'My Documents' path
      SHGetSpecialFolderPath(NULL, szSpecialFolder, CSIDL_MYDOCUMENTS, FALSE);

      /// [CHECK] Is 'Current Folder' a child of My Documents?
      if (isPathSubFolderOf(szSpecialFolder, pDialogData->szFolder))
         eOutput = TP_DOCUMENTS;

      // Iterate through all possible drive letters
      else for (INT iDrive = 0; eOutput == TP_NONE AND iDrive < 25; iDrive++)
      {
         // Generate Root path
         szSpecialFolder[0] = NULL;
         PathBuildRoot(szSpecialFolder, iDrive);

         /// [CHECK] Is 'Current Folder' a child of a drive?
         if (PathIsSameRoot(szSpecialFolder, pDialogData->szFolder))
            eOutput = TP_DRIVE;
      }
   }

   // Cleanup and return
   utilDeleteString(szSpecialFolder);
   return eOutput;
}


/// Function name  : calculateFileTypeFromFileFilter
// Description     : Calculates the file type from the file filter
// 
// CONST FILE_FILTER  eFilter   : [in] File filter
// 
// Return Value   : Matching FileType, if any, otherwise FIF_NONE
// 
FILE_ITEM_FLAG  calculateFileTypeFromFileFilter(CONST FILE_FILTER  eFilter)
{
   FILE_ITEM_FLAG    eOutput;

   // Examine file
   switch (eFilter)
   {
   /// [SCRIPTS]
   case FF_SCRIPT_FILES:
   case FF_SCRIPT_PCK_FILES:
   case FF_SCRIPT_XML_FILES:  eOutput = FIF_SCRIPT;      break;
   /// [LANGUAGE FILES]
   case FF_LANGUAGE_FILES:    eOutput = FIF_LANGUAGE;    break;
   /// [MISSION FILES]
   case FF_MISSION_FILES:     eOutput = FIF_MISSION;     break;
   // [UNKOWN]
   default:                   eOutput = FIF_NONE;        break;
   }

   // Return result
   return eOutput;
}


/// Function name  : calculateFileDialogShortcutTarget
// Description     : Resolve the path of the target file/folder indicated by a shortcut file
//
// TCHAR*  szShortcutPath : [in/out] The full path of a shortcut file to resolve
//                                   This will be replaced with the full path of it's target
// 
// Return type : Postive if succesful, negative if there was an error
//
HRESULT  calculateFileDialogShortcutTarget(TCHAR*  szShortcutPath)
{
   IPersistFile*  pPersistFile;    // IPersistFile interface
   IShellLink*    pShellLink;      // IShellLink interface
   TCHAR*         szTargetPath;    // Full path of resolved target
   HRESULT        hResult;         // Operation result

   // Prepare
   szTargetPath = utilCreateEmptyPath();
   pShellLink   = NULL;
   pPersistFile = NULL;

   // Get IShellLink interface
   hResult = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (IInterface*)&pShellLink);

   // Get IPersistFile interface
   if (SUCCEEDED(hResult))
      hResult = pShellLink->QueryInterface(IID_IPersistFile, (IInterface*)&pPersistFile);

   // Load link
   if (SUCCEEDED(hResult))
      hResult = pPersistFile->Load(szShortcutPath, STGM_READ);

   // Resolve link
   if (SUCCEEDED(hResult))
      hResult = pShellLink->Resolve(NULL, SLR_ANY_MATCH);

   if (SUCCEEDED(hResult))
   {
      // Extract resolved path
      hResult = pShellLink->GetPath(szTargetPath, MAX_PATH, NULL, NULL);
      // Overwrite input path with resolved path
      if (SUCCEEDED(hResult))
         StringCchCopy(szShortcutPath, MAX_PATH, szTargetPath);
   }
   
   // Cleanup and return
   utilDeleteString(szTargetPath);
   utilReleaseInterface(pPersistFile);
   utilReleaseInterface(pShellLink);
   return hResult;
}


/// Function name  : initFileDialogJumpBarLocations
// Description     : Determine the user's 'my places' folders, retrieve their name, path and large icon.
// 
// FILE_DIALOG_DATA*  pDialogData : [in] File dialog window data
// 
// Return Type : TRUE
//
BOOL  initFileDialogJumpBarLocations(FILE_DIALOG_DATA*  pDialogData)
{
   CONST UINT   iJumpBarCLSIDs[2] = { CSIDL_DESKTOP, CSIDL_MYDOCUMENTS };     // CLSID of desktop and MyDocuments
   TCHAR       *szFolderPath[4];        // Paths of the game subfolders
   HWND         hCtrl;                  // Button associated with the current JumpBarItem
   
   // Generate folder paths
   szFolderPath[0] = generateGameDataFilePath(GFI_LANGUAGE_FOLDER, getAppPreferences()->szGameFolder, getAppPreferences()->eGameVersion);
   szFolderPath[1] = generateGameDataFilePath(GFI_SCRIPT_FOLDER, getAppPreferences()->szGameFolder, getAppPreferences()->eGameVersion);
   szFolderPath[2] = generateGameDataFilePath(GFI_MISSION_FOLDER, getAppPreferences()->szGameFolder, getAppPreferences()->eGameVersion);

   // Generate GameFolder path
   szFolderPath[3] = utilDuplicatePath(getAppPreferences()->szGameFolder);
   if (getAppPreferences()->eGameVersion == GV_ALBION_PRELUDE)
      StringCchCat(szFolderPath[3], MAX_PATH, TEXT("addon\\"));

   // Create 'Desktop' and 'My Docs' from their CLSID
   for (UINT iJumpItem = 0; iJumpItem < 2; iJumpItem++)
   {
      // Get button control window handle
      hCtrl = GetDlgItem(pDialogData->hDialog, IDC_LOCATION_JUMP1 + iJumpItem);
      
      /// Generate JumpBarItem from CLSID
      pDialogData->pJumpBarItems[iJumpItem] = createJumpBarItemFromLocation(hCtrl, iJumpBarCLSIDs[iJumpItem], NULL);
   }

   /// [GAME FOLDER] Manually define the game folder button
   hCtrl = GetDlgItem(pDialogData->hDialog, IDC_LOCATION_JUMP3);
   pDialogData->pJumpBarItems[2] = createJumpBarItem(hCtrl, szFolderPath[3], TEXT("Game Folder"), getAppImageList(ITS_LARGE), getAppImageTreeIconIndex(ITS_LARGE, identifyGameVersionIconID(getAppPreferences()->eGameVersion)));

   /// [LANGUAGE FOLDER] Manually define the language folder button
   hCtrl = GetDlgItem(pDialogData->hDialog, IDC_LOCATION_JUMP4);
   pDialogData->pJumpBarItems[3] = createJumpBarItem(hCtrl, szFolderPath[0], TEXT("Language Folder"), getAppImageList(ITS_LARGE), getAppImageTreeIconIndex(ITS_LARGE, TEXT("LANGUAGE_FOLDER_ICON")));
   
   /// [SCRIPT FOLDER] Manually define the script folder button
   hCtrl = GetDlgItem(pDialogData->hDialog, IDC_LOCATION_JUMP5);
   pDialogData->pJumpBarItems[4] = createJumpBarItem(hCtrl, szFolderPath[1], TEXT("Scripts Folder"), getAppImageList(ITS_LARGE), getAppImageTreeIconIndex(ITS_LARGE, TEXT("SCRIPT_FOLDER_ICON")));
   
   /// [MISSION FOLDER] Manually define the mission folder button
   hCtrl = GetDlgItem(pDialogData->hDialog, IDC_LOCATION_JUMP6);
   pDialogData->pJumpBarItems[5] = createJumpBarItem(hCtrl, szFolderPath[2], TEXT("Director Folder"), getAppImageList(ITS_LARGE), getAppImageTreeIconIndex(ITS_LARGE, TEXT("MISSION_FOLDER_ICON")));
   
   // [CHECK] Disable each folder button if sub-folder doesn't exist
   for (UINT iFolder = 0; iFolder < 3; iFolder++)
      pDialogData->pJumpBarItems[iFolder+3]->bValid = isFilePresent(getFileSystem(), szFolderPath[iFolder]);

   // Cleanup and return
   utilDeleteStrings(szFolderPath[0], szFolderPath[1], szFolderPath[2]);
   utilDeleteString(szFolderPath[3]);
   return TRUE;
}


/// Function name  : performFileDialogFolderChange
// Description     : Ensures the new folder has a trailing backslash and updates the files ListView
// 
// FILE_DIALOG_DATA*    pDialogData : [in/out] FileDialog window data
// CONST TCHAR*         szNewFolder : [in]     New folder to display
// 
VOID   performFileDialogFolderChange(FILE_DIALOG_DATA*  pDialogData, CONST TCHAR*  szNewFolder)
{
   /// Update current folder
   StringCchCopy(pDialogData->szFolder, MAX_PATH, szNewFolder);
   PathAddBackslash(pDialogData->szFolder);

   // [HACK] Display current folder in the locations bar
   SetDlgItemText(pDialogData->hDialog, IDC_LOCATION_COMBO, pDialogData->szFolder);

   // [OPEN] Erase 'Filename' edit
   if (pDialogData->eType == FDT_OPEN)
      SetWindowText(pDialogData->hFileEdit, TEXT(""));

   /// Display folder contents
   updateFileDialog(pDialogData, FDS_SEARCH);
}


/// Function name  : performFileDialogInputValidation
// Description     : Determine how to respond to an input path
// 
// FILE_DIALOG_DATA*  pDialogData : [in] Dialog data
// CONST TCHAR*       szFullPath  : [in] Full File path
// 
// Return Value : TRUE if file was accepted and dialog should be closed, otherwise FALSE
//
BOOL   performFileDialogInputValidation(FILE_DIALOG_DATA*  pDialogData, CONST TCHAR*  szInputPath)
{
   VIRTUAL_FILE*  pVirtualFile;  // VirtualFile associated with the input path
   TCHAR*         szFullPath;    // Copy of input path
   BOOL           bResult;       // Operation result

   // Prepare
   szFullPath = utilDuplicatePath(szInputPath);
   bResult    = TRUE;

   // Ensure output file list is empty
   deleteListContents(pDialogData->pOutputFileList);

   /// [PHYSICAL FOLDER] Display folder
   if (PathIsDirectory(szFullPath))
   {
      // Display folder and return 'stay open'
      performFileDialogFolderChange(pDialogData, szFullPath);
      bResult = FALSE;
   }
   /// [PHYSICAL SHORTCUT] Resolve shortcut
   else if (utilCompareString(PathFindExtension(szFullPath), ".lnk"))
   {
      calculateFileDialogShortcutTarget(szFullPath);
      bResult = performFileDialogInputValidation(pDialogData, szFullPath);
   }
   /// [PHYSICAL FILE] Determine whether to accept the file 
   else if (PathFileExists(szFullPath))
   {
      // [QUESTION] "The file '%s' already exists, would you like to overwrite it?"
      if (pDialogData->eType == FDT_SAVE AND displayMessageDialogf(NULL, IDS_GENERAL_OVERWRITE_PHYSICAL_FILE, MDF_YESNO WITH MDF_QUESTION, PathFindFileName(szFullPath)) == IDNO)
         // [NO] Return 'stay open'
         bResult = FALSE;     

      // [YES] Store file and close dialog
      else
         appendFileDialogOutputFile(pDialogData, szFullPath, identifyFileType(getFileSystem(), szFullPath));
   }
   /// [CHECK] Try and locate a matching virtual file or folder
   else if (findVirtualFileByPath(getFileSystem(), szFullPath, pVirtualFile))
   {
      /// [VIRTUAL FOLDER] Display folder
      if (pVirtualFile->iAttributes INCLUDES FIF_FOLDER)
      {
         // Display folder and return 'stay open'
         performFileDialogFolderChange(pDialogData, szFullPath);
         bResult = FALSE;
      }
      /// [VIRTUAL FILE (OPEN)] Store file and close dialog
      else if (pDialogData->eType == FDT_OPEN)
         appendFileDialogOutputFile(pDialogData, szFullPath, identifyFileType(getFileSystem(), szFullPath));
         
      /// [VIRTUAL FILE (SAVE)] Error: Cannot overwrite virtual files
      else
      {
         // [ERROR] "The file '%s' is part of the virtual file system and cannot be overwritten"
         displayMessageDialogf(NULL, IDS_GENERAL_OVERWRITE_VIRTUAL_FILE, MDF_OK WITH MDF_ERROR, PathFindFileName(szFullPath));
         bResult = FALSE;
      }
   }
   /// [NEW FILE (OPEN)] Error: File does not exist
   else if (pDialogData->eType == FDT_OPEN)
   {
      // [ERROR] "The file '%s' cannot be opened because it does not exist"
      displayMessageDialogf(NULL, IDS_GENERAL_FILE_NOT_FOUND, MDF_OK WITH MDF_ERROR, PathFindFileName(szFullPath));
      bResult = FALSE;
   }
   /// [NEW FILE (SAVE)] Store file and close dialog
   else
   {
      // [CHECK] Has the user omitted the file extension?
      if (pDialogData->eFilter != FF_ALL_FILES AND !utilCompareString(PathFindExtension(szFullPath), ".pck") AND !utilCompareString(PathFindExtension(szFullPath), ".xml") AND !utilCompareString(PathFindExtension(szFullPath), ".xprj"))
         /// [NO EXTENSION] Add the default extension
         StringCchCat(szFullPath, MAX_PATH, identifyDefaultFileExtension(pDialogData->eFilter));

      // Store file and close dialog
      appendFileDialogOutputFile(pDialogData, szFullPath, calculateFileTypeFromFileFilter(pDialogData->eFilter));
   }

   // Cleanup and return result
   utilDeleteString(szFullPath);
   return bResult;
}


/// Function name  : appendFileDialogOutputFile
// Description     : Set the path and type of the dialog's output file
// 
// FILE_DIALOG_DATA*     pDialogData  : [in] FileDialog window data
// CONST TCHAR*          szFullPath   : [in] Full path of output file
// CONST FILE_ITEM_FLAG  eFileType    : [in] Output file type
// 
VOID  appendFileDialogOutputFile(FILE_DIALOG_DATA*  pDialogData, CONST TCHAR*  szFullPath, CONST FILE_ITEM_FLAG  eFileType)
{
   /// Add new StoredDocument to output list
   appendObjectToList(pDialogData->pOutputFileList, (LPARAM)createStoredDocument(eFileType, szFullPath));

   // Update output file count
   pDialogData->iOutputFiles = getListItemCount(pDialogData->pOutputFileList);
}


/// Function name  : setFileDialogUpdateState
// Description     : Shows or hides the 'Loading' screen
// 
// FILE_DIALOG_DATA*  pDialogData : [in] Window data
// CONST BOOL         bUpdating   : [in] Whether to show or hide the 'loading' text
// 
VOID  setFileDialogUpdateState(FILE_DIALOG_DATA*  pDialogData, CONST BOOL  bUpdating)
{
   /// [UPDATING] Disable controls and display 'Loading'
   if (pDialogData->bIsUpdating = bUpdating)
   {
      // [LISTVIEW] Disable and empty
      ListView_SetItemCount(pDialogData->hListView, NULL);
      utilEnableDlgItem(pDialogData->hDialog, IDC_FILE_LIST, FALSE);

      // [FILENAME,FILTER,OPTIONS,NAMESPACE,OK] Disable all
      utilEnableDlgItem(pDialogData->hDialog, IDC_FILENAME_EDIT, FALSE);
      utilEnableDlgItem(pDialogData->hDialog, IDC_FILETYPE_COMBO, FALSE);
      utilEnableDlgItem(pDialogData->hDialog, IDC_LOCATION_COMBO, FALSE);
      utilEnableDlgItem(pDialogData->hDialog, IDC_FILE_OPTIONS, FALSE);
      utilEnableDlgItem(pDialogData->hDialog, IDOK, FALSE);
      utilEnableDlgItem(pDialogData->hDialog, IDCANCEL, FALSE);
      
      // [TOOLBAR] Disable all buttons
      SendMessage(pDialogData->hToolbar, TB_ENABLEBUTTON, IDC_LOCATION_UP, FALSE);
      SendMessage(pDialogData->hToolbar, TB_ENABLEBUTTON, IDC_LOCATION_REFRESH, FALSE);

      // [JUMPBAR] Disable all 
      for (UINT  iItem = 0; iItem < iJumpBarItemCount; iItem++)
         EnableWindow(pDialogData->pJumpBarItems[iItem]->hCtrl, FALSE);
   }
   /// [NORMAL] Enable (most) controls and display content
   else
   {
      // [LISTVIEW] Enable and Populate
      ListView_SetItemCount(pDialogData->hListView, getFileSearchResultCount(pDialogData->pFileSearch));
      utilEnableDlgItem(pDialogData->hDialog, IDC_FILE_LIST, TRUE);

      // [TOOLBAR] Enable all (except when root)
      SendMessage(pDialogData->hToolbar, TB_ENABLEBUTTON, IDC_LOCATION_UP, !PathIsRoot(pDialogData->szFolder));
      SendMessage(pDialogData->hToolbar, TB_ENABLEBUTTON, IDC_LOCATION_REFRESH, TRUE);

      // [FILENAME,FILTER,OPTIONS,NAMESPACE] Enable all
      utilEnableDlgItem(pDialogData->hDialog, IDC_FILENAME_EDIT, TRUE);
      utilEnableDlgItem(pDialogData->hDialog, IDC_FILETYPE_COMBO, TRUE);
      utilEnableDlgItem(pDialogData->hDialog, IDC_LOCATION_COMBO, TRUE);
      utilEnableDlgItem(pDialogData->hDialog, IDC_FILE_OPTIONS, TRUE);
      utilEnableDlgItem(pDialogData->hDialog, IDCANCEL, TRUE);

      // [OK] Enable if filename exists
      utilEnableDlgItem(pDialogData->hDialog, IDOK, utilGetDlgItemTextLength(pDialogData->hDialog, IDC_FILENAME_EDIT) > 0);

      // [JUMP-BAR] Only enable items that weren't disabled in the first place
      for (UINT  iItem = 0; iItem < iJumpBarItemCount; iItem++)
         if (pDialogData->pJumpBarItems[iItem]->bValid)
            EnableWindow(pDialogData->pJumpBarItems[iItem]->hCtrl, TRUE);
   }

   // Refresh ListView
   UpdateWindow(pDialogData->hListView);
}


/// Function name  : updateFileDialog
// Description     : Erase any existing ListView contents and fill with the contents of the 'current folder'.
//                    Recalculate the locations to also match the 'current folder'
//
// FILE_DIALOG_DATA*         pDialogData : [in] File dialog data
// CONST FILE_DIALOG_SEARCH  eCommand    : [in] Whether to search for new results or sort existing results
// 
VOID  updateFileDialog(FILE_DIALOG_DATA*  pDialogData, CONST FILE_DIALOG_SEARCH  eCommand)
{
   SEARCH_OPERATION*  pOperationData;     // Convenience pointer

   /// Display an empty listView with "Please Wait.."
   setFileDialogUpdateState(pDialogData, TRUE);

   // Examine command
   switch (eCommand)
   {
   /// [SEARCH] Create new search results from current folder and update namespace
   case FDS_SEARCH:
      // Create and store operation data
      pOperationData = pDialogData->pSearchOperation = createSearchOperationData();

      // Initialise a FileSearch operation
      pOperationData->hParentWnd  = pDialogData->hDialog;
      pOperationData->eFilter     = pDialogData->eFilter;
      pOperationData->eSortKey    = convertFileDialogSortColumnToSortKey(pDialogData->iSortColumn);
      pOperationData->eDirection  = (pDialogData->bSortAscending ? AO_DESCENDING : AO_ASCENDING);
      pOperationData->pFileSearch = pDialogData->pFileSearch;
      StringCchCopy(pOperationData->szFolder, MAX_PATH, pDialogData->szFolder);
      
      /// Launch FileSearch thread
      launchThreadedOperation(threadprocFileSystemSearch, pOperationData);

      // Create progress report timer
      SetTimer(pDialogData->hDialog, FILE_SEARCH_TIMER_ID, 100, NULL);
      break;

   /// [SORT] Sort existing search results only
   case FDS_SORT:
      // Re-sort existing results
      performFileSearchSort(pDialogData->pFileSearch, convertFileDialogSortColumnToSortKey(pDialogData->iSortColumn), (pDialogData->bSortAscending ? AO_DESCENDING : AO_ASCENDING));

      // Remove "Please Wait..." and display new ListView contents
      setFileDialogUpdateState(pDialogData, FALSE);
      break;
   }
}


/// Function name  : updateFileDialogNamespace
// Description     : Refresh the locations combo box with new data based on the current FileDialog folder
// 
// FILE_DIALOG_DATA*  pDialogData : [in] FileDialog window data
// 
VOID   updateFileDialogNamespace(FILE_DIALOG_DATA*  pDialogData)
{
   FILE_ITEM        *pFileItem;           // Search result from the desktop file/folder search
   FILE_SEARCH      *pFileSearch;         // FileSearch object representing all the files on the user's desktop
   TRAVERSAL_PARENT  eTraversalParent;    // Indicates under which item the current folder should be placed
   TCHAR            *szSpecialFolder,     // Buffer for the folder path of a special windows folder
                    *szDrive;             // Logical Drive strings iterator

   // Prepare
   szSpecialFolder = utilCreateEmptyPath();

   // Remove all previous entries
   deleteFileDialogNamespace(pDialogData);

   /// Determine whether to display traversal under desktop, my documents, relevant drive or nowhere.
   eTraversalParent = calculateFileDialogTraversalParent(pDialogData);

   /// Add Desktop and My Documents
   appendFileDialogNamespaceFolder(pDialogData, CSIDL_DESKTOPDIRECTORY, NULL, 0);
   appendFileDialogNamespaceFolder(pDialogData, CSIDL_MYDOCUMENTS, NULL, 1);

   // [CHECK] Is this is a child of 'My Documents'?
   if (eTraversalParent == TP_DOCUMENTS)
      // [SUCCESS] Add traversal
      appendFileDialogNamespaceTraversal(pDialogData, szSpecialFolder, 2);
       
   /// Add My Computer
   appendFileDialogNamespaceFolder(pDialogData, CSIDL_DRIVES, NULL, 1);

   // Lookup drive strings
   GetLogicalDriveStrings(MAX_PATH, szSpecialFolder);

   /// Iterate through drives
   for (szDrive = szSpecialFolder; szDrive[0]; szDrive = &szDrive[lstrlen(szDrive)+1])
   {
      // Add drive
      appendFileDialogNamespaceFolder(pDialogData, NULL, szDrive, 2);
      // [CHECK] Is this is a child of the drive?
      if (eTraversalParent == TP_DRIVE AND PathIsSameRoot(pDialogData->szFolder, szDrive))
         // [SUCCESS] Add traversal
         appendFileDialogNamespaceTraversal(pDialogData, szDrive, 3);
   }

   // Get desktop folder (with a trailing backslash)
   SHGetSpecialFolderPath(NULL, szSpecialFolder, CSIDL_DESKTOPDIRECTORY, FALSE);
   PathAddBackslash(szSpecialFolder);

   // Search for all items on the Desktop
   pFileSearch = performFileSystemSearch(getFileSystem(), szSpecialFolder, FF_ALL_FILES, AK_PATH, AO_DESCENDING, NULL);

   /// Iterate through desktop folders
   while (findNextFileSearchResult(pFileSearch, pFileItem))
   {
      // [CHECK] Ensure item is a folder
      if (pFileItem->iAttributes INCLUDES FIF_FOLDER)
      {
         // Add folder
         appendFileDialogNamespaceFolder(pDialogData, NULL, pFileItem->szFullPath, 1);
         // [CHECK] Is this is a child of the desktop?
         if (eTraversalParent == TP_DESKTOP AND isPathSubFolderOf(pFileItem->szFullPath, pDialogData->szFolder))
            // [SUCCESS] Add traversal
            appendFileDialogNamespaceTraversal(pDialogData, pFileItem->szFullPath, 2);
      }
   }

   // Cleanup
   deleteFileSearch(pFileSearch);
   utilDeleteString(szSpecialFolder);
}



