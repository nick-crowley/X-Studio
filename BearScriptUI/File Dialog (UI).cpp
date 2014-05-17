//
// FILE_NAME.cpp : FILE_DESCRIPTION
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onFileDialogFilter_Changed
// Description     : Redisplay the current folder using the currently selected filter
//
// FILE_DIALOG_DATA*  pDialogData : [in] File dialog data
// 
VOID  onFileDialogFilter_Changed(FILE_DIALOG_DATA*  pDialogData)
{
   // Update the current filter
   pDialogData->eFilter = (FILE_FILTER)ComboBox_GetCurSel(pDialogData->hFilterCombo);

   // [OPEN] Erase 'Filename' edit
   if (pDialogData->eType == FDT_OPEN)
      SetWindowText(pDialogData->hFileEdit, TEXT(""));

   // Re-display the current folder
   updateFileDialog(pDialogData, FDS_SEARCH);
}


/// Function name  : onFileDialogFilename_Changed
// Description     : Enable 'OK' once user has entered a filename
//
// FILE_DIALOG_DATA*  pDialogData : [in] File dialog data
// 
VOID  onFileDialogFilename_Changed(FILE_DIALOG_DATA*  pDialogData)
{
   // Enable 'OK' if a filename is present
   utilEnableDlgItem(pDialogData->hDialog, IDOK, (GetWindowTextLength(pDialogData->hFileEdit) > 0));

   // Update suggestions
   //updateFileDialogSuggestions(pDialogData);
}


/// Function name  : onFileDialogFilename_GetFocus
// Description     : Erases the filename edit if multiple files are selected
//
// FILE_DIALOG_DATA*  pDialogData : [in] File dialog data
// 
VOID  onFileDialogFilename_GetFocus(FILE_DIALOG_DATA*  pDialogData)
{
   // [CHECK] Are multiple files selected?
   if (ListView_GetSelectedCount(pDialogData->hListView) > 1)
      /// [MULTIPLE-SELECTION] Erase filename
      SetWindowText(pDialogData->hFileEdit, TEXT(""));
}


/// Function name  : onFileDialogFilename_LostFocus
// Description     : Destroys suggestions
//
// FILE_DIALOG_DATA*  pDialogData : [in] File dialog data
// 
VOID  onFileDialogFilename_LostFocus(FILE_DIALOG_DATA*  pDialogData)
{
   // [CHECK] Ensure focus is on FileName or SuggestionList
   if (GetFocus() != pDialogData->hFileEdit AND GetFocus() != pDialogData->hSuggestionList)
      // [FAILED] Destroy list
      displayFileDialogSuggestions(pDialogData, FALSE);
}



/// Function name  : onFileDialogList_ColumnClick
// Description     : Change the sort order and re-display
//
// FILE_DIALOG_DATA*  pDialogData  : [in] File dialog data
// CONST UINT         iColumnIndex : [in] Column index of the column clicked
// 
VOID  onFileDialogList_ColumnClick(FILE_DIALOG_DATA*  pDialogData, CONST UINT  iColumnIndex)
{
   /// [CHECK] Is this already the sort column?
   if (pDialogData->iSortColumn == iColumnIndex)
      // [SUCCESS] Reverse the current sorting direction
      pDialogData->bSortAscending = !pDialogData->bSortAscending;
   else
      // [FAILURE] Set new sort column
      pDialogData->iSortColumn = iColumnIndex;
   
   // Highlight column to indicate sorting
   ListView_SetSelectedColumn(pDialogData->hListView, iColumnIndex);

   // [OPEN] Erase 'Filename' edit
   if (pDialogData->eType == FDT_OPEN)
      SetWindowText(pDialogData->hFileEdit, TEXT(""));

   /// Re-sort existing results
   updateFileDialog(pDialogData, FDS_SORT);
}


/// Function name  : onFileDialogList_DoubleClick
// Description     : Returns the file represented by the item double-clicked, unless the item is a folder - then
//                     it's contents are displayed instead
// 
// FILE_DIALOG_DATA* pDialogData : [in] File dialog data
// CONST UINT        iItem       : [in] Zero-based Item number clicked
// 
VOID  onFileDialogList_DoubleClick(FILE_DIALOG_DATA*  pDialogData, CONST UINT  iItem)
{
   FILE_ITEM*  pFileItem;    // FileItem associated with the currently selected item

   // Lookup associated FileItem
   findFileSearchResultByIndex(pDialogData->pFileSearch, iItem, pFileItem);
   ASSERT(pFileItem);

   /// Accept path, reject path, or change folder
   if (performFileDialogInputValidation(pDialogData, pFileItem->szFullPath))
      // [ACCEPT] Close dialog and return
      EndDialog(pDialogData->hDialog, IDOK);
}


/// Function name  : onFileDialogList_SelectionChanged
// Description     : Sets the currently selected file
// 
// FILE_DIALOG_DATA* pDialogData : [in] File dialog data
// 
VOID  onFileDialogList_SelectionChanged(FILE_DIALOG_DATA*  pDialogData)
{
   FILE_ITEM*  pCurrentFile;      // FileItem associated with the item
   UINT        iSelectionCount;   // Number of selected items
   INT         iItemIndex;        // Index of the item being processed

   // Prepare
   iItemIndex = -1;

   // Examine selected item count
   switch (iSelectionCount = ListView_GetSelectedCount(pDialogData->hListView))
   {
   /// [ERASED SELECTION] Erase 'Filename' edit
   case 0:
      SetWindowText(pDialogData->hFileEdit, TEXT(""));
      break;

   /// [SINGLE SELECTION] Display item in 'FileName' edit
   case 1:
      // Get selected item index
      iItemIndex = ListView_GetNextItem(pDialogData->hListView, iItemIndex, LVNI_ALL WITH LVNI_SELECTED);

      // Lookup associated file item
      findFileSearchResultByIndex(pDialogData->pFileSearch, iItemIndex, pCurrentFile);
      ASSERT(pCurrentFile);

      // [CHECK] Ensure item isn't a folder
      if (~pCurrentFile->iAttributes INCLUDES FIF_FOLDER)
         /// [SUCCESS] Display item name in the 'FileName' edit
         SetWindowText(pDialogData->hFileEdit, pCurrentFile->szDisplayName);
      break;
   
   /// [MULTIPLE SELECTION] Display placeholder 'FileName' edit
   default:
      SetWindowText(pDialogData->hFileEdit, TEXT("[Multiple Files]"));
      break;
   }

   // Disable the filename edit for multiple selections
   EnableWindow(pDialogData->hFileEdit, (iSelectionCount <= 1));
}


/// Function name  : onFileDialogJumpBar_Click
// Description     : Change the current folder to the appropriate jump-bar destination
// 
// FILE_DIALOG_DATA*  pDialogData  : [in] FileDialog window data
// CONST UINT         iIndex       : [in] Zero-based index of the jump bar button clicked
// 
VOID  onFileDialogJumpBar_Click(FILE_DIALOG_DATA*  pDialogData, CONST UINT  iIndex)
{
   TCHAR*  szJumpFolder;      // Path specified by the JumpBar item clicked

   // Extract destination folder from associated JumpBar item
   szJumpFolder = pDialogData->pJumpBarItems[iIndex]->szPath;

   // Change display folder
   performFileDialogFolderChange(pDialogData, szJumpFolder);
}


/// Function name  : onFileDialogNamespace_Changed
// Description     : Change the current folder to that specfied by the newly selected location
// 
// FILE_DIALOG_DATA*  pDialogData : [in] File dialog data
// 
VOID  onFileDialogNamespace_Changed(FILE_DIALOG_DATA*  pDialogData)
{
   TCHAR*          szPath;

   // Extract path from current item
   szPath = (TCHAR*)getCustomComboBoxItemParam(pDialogData->hLocationCombo, ComboBox_GetCurSel(pDialogData->hLocationCombo));

   // [CHECK] Does location have a path?
   if (lstrlen(szPath))
      /// [SUCCESS] Change to folder
      performFileDialogFolderChange(pDialogData, szPath);
   else
      /// [VIRTUAL LOCATION] Not currently supported
      displayMessageDialogf(NULL, IDS_GENERAL_NOT_IMPLEMENTED, MDF_ERROR WITH MDF_OK, TEXT("Browsing Virtual Namespace Folders"));
}


/// Function name  : onFileDialogOK_Click
// Description     : Validates the current contents of the filename edit: Accept path, Reject path or change folder
// 
// FILE_DIALOG_DATA*  pDialogData  : [in] FileDialog window data
// 
VOID  onFileDialogOK_Click(FILE_DIALOG_DATA*  pDialogData)
{
   FILE_ITEM  *pCurrentFile;     //
   TCHAR      *szInputPath,      // Input filename or path
              *szFullPath;       // Full filepath of the input filename
   INT         iItemIndex,       //
               iSelectionCount;

   // Determine number of selected files
   iSelectionCount = ListView_GetSelectedCount(pDialogData->hListView);

   /// [ZERO/SINGLE SELECTION] Validate filename edit path
   if (iSelectionCount <= 1)
   {
      // Prepare
      szInputPath = utilGetDlgItemText(pDialogData->hDialog, IDC_FILENAME_EDIT);

      // [CHECK] Is input a filename or a path?
      if (utilCompareStringN(&szInputPath[1], ":\\", 2)) 
         /// [PATH] Use entire path
         szFullPath = utilDuplicatePath(szInputPath);
      else
         /// [FILENAME] Generate entire path
         szFullPath = utilCreateStringf(MAX_PATH, TEXT("%s%s"), pDialogData->szFolder, szInputPath);

      // [CHECK] Determine whether to accept input and close the dialog
      if (performFileDialogInputValidation(pDialogData, szFullPath))
         /// [SUCCESS] Close the dialog
         EndDialog(pDialogData->hDialog, IDOK);

      // Cleanup
      utilDeleteStrings(szInputPath, szFullPath);
   }
   /// [MULTIPLE SELECTION] Extract all necessary data from the FileSearch
   else
   {
      // Prepare
      iItemIndex = -1;

      // Ensure output file list is empty
      deleteListContents(pDialogData->pOutputFileList);

      // Iterate through selected files
      while ((iItemIndex = ListView_GetNextItem(pDialogData->hListView, iItemIndex, LVNI_ALL WITH LVNI_SELECTED)) != -1)
      {
         // Lookup FileItem
         findFileSearchResultByIndex(pDialogData->pFileSearch, iItemIndex, pCurrentFile);

         // Add to output list
         appendFileDialogOutputFile(pDialogData, pCurrentFile->szFullPath, extractFileItemType(pCurrentFile->iAttributes));
      }

      /// Close dialog
      EndDialog(pDialogData->hDialog, IDOK);
   }
}


/// Function name  : onFileDialogOptions_Click
// Description     : Display the options dialog
// 
// FILE_DIALOG_DATA*  pDialogData : [in] File dialog data
// 
VOID  onFileDialogOptions_Click(FILE_DIALOG_DATA*  pDialogData)
{
   OPTIONS_DIALOG_DATA*  pOptionsDialogData;    // Options dialog data

   /// Display Options dialog
   if (pOptionsDialogData = displayFileOptionsDialog(pDialogData->hDialog, &pDialogData->oAdvanced))
   {
      // [CHANGED] Replace existing data
      pDialogData->oAdvanced = pOptionsDialogData->oOptions;
      deleteFileOptionsDialogData(pOptionsDialogData);
   }
}



/// Function name  : onFileDialogRefresh_Click
// Description     : Refreshes the current file search
// 
// FILE_DIALOG_DATA*  pDialogData : [in] File dialog data
// 
VOID  onFileDialogRefresh_Click(FILE_DIALOG_DATA*  pDialogData)
{
   // [OPEN] Erase 'Filename' edit
   if (pDialogData->eType == FDT_OPEN)
      SetWindowText(pDialogData->hFileEdit, TEXT(""));

   // Perform current filesearch again
   updateFileDialog(pDialogData, FDS_SEARCH);
}


/// Function name  : onFileDialogTraverseUp_Click
// Description     : Change the folder to the current folder's parent
// 
// FILE_DIALOG_DATA*  pDialogData : [in] File dialog data
// 
VOID  onFileDialogTraverseUp_Click(FILE_DIALOG_DATA*  pDialogData)
{
   TCHAR*  szParentFolder;    // Parent folder

   // [CHECK] Should never be the root folder
   ASSERT(PathIsRoot(pDialogData->szFolder) == FALSE);

   // Retrieve parent folder
   PathRemoveBackslash(pDialogData->szFolder);
   szParentFolder = utilDuplicateFolderPath(pDialogData->szFolder);

   /// Switch to parent folder
   performFileDialogFolderChange(pDialogData, szParentFolder);

   // Cleanup
   utilDeleteString(szParentFolder);
}
