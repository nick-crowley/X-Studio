//
// Export Project Dialog.cpp : The 'export project' dialog
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"
#include "XZip.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    DECLARATIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

// Creation/Destruction
EXPORT_PROJECT_DATA*  createExportProjectDialogData(PROJECT_FILE*  pProject);
VOID                  deleteExportProjectDialogData(EXPORT_PROJECT_DATA*  &pData);

// Helpers
EXPORT_PROJECT_DATA*  getExportProjectDialogData(HWND  hDialog);

// Functions
BOOL     initExportProjectDialog(EXPORT_PROJECT_DATA*  pDialogData, HWND  hDialog);
VOID     updateExportProjectDialog(EXPORT_PROJECT_DATA*  pDialogData);

// Message Handler
VOID     onExportProjectDialog_Browse(EXPORT_PROJECT_DATA*  pDialogData);
BOOL     onExportProjectDialog_Command(EXPORT_PROJECT_DATA*  pDialogData, CONST UINT  iControlID, CONST UINT  iNotification, HWND  hCtrl);
VOID     onExportProjectDialog_OK(EXPORT_PROJECT_DATA*  pDialogData);

// Window Procedure
INT_PTR  dlgprocExportProjectDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createExportProjectDialogData
// Description     : Create 'Export Project' dialog data
// 
// Return Value   : New dialog data, you are responsible for destroying it
// 
EXPORT_PROJECT_DATA*  createExportProjectDialogData(PROJECT_FILE*  pProject)
{
   EXPORT_PROJECT_DATA*  pNewData = utilCreateEmptyObject(EXPORT_PROJECT_DATA);

   // Store project
   pNewData->pProject = pProject;

   // Filename: Use last filename, if any, otherwise project name
   if (lstrlen(getAppPreferences()->szBackupFileName))
      StringCchCopy(pNewData->szFileName, MAX_PATH, getAppPreferences()->szBackupFileName);
   else
   {
      TCHAR  szFileName[64];
      // Generate project filename without extension. Format into filename string
      StringCchCopy(szFileName, 64, PathFindFileName(pProject->szFullPath));
      PathRemoveExtension(szFileName);
      StringCchPrintf(pNewData->szFileName, MAX_PATH, TEXT("%s v1.zip"), szFileName);
   }

   // Folder: Use last folder if any, otherwise game folder
   const TCHAR* szFolder = lstrlen(getAppPreferences()->szBackupFolder) ? getAppPreferences()->szBackupFolder : getAppPreferences()->szGameFolder;
   StringCchCopy(pNewData->szFolder, MAX_PATH, szFolder);
   
   // Return object
   return pNewData;
}


/// Function name  : deleteExportProjectDialogData
// Description     : Delete 'Export Project' dialog data
// 
// EXPORT_PROJECT_DATA*  &pData   : [in] Dialog data
// 
VOID  deleteExportProjectDialogData(EXPORT_PROJECT_DATA*  &pData)
{
   // Delete tooltip
   utilDeleteWindow(pData->hTooltip);

   // Delete calling object
   utilDeleteObject(pData);
}


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////


/// Function name  : getExportProjectDialogData
// Description     : Retrieve dialog data from a 'Export Project' dialog
// 
// HWND  hDialog   : [in] 'Export Project' dialog window handle
// 
// Return Value   : Dialog data
// 
EXPORT_PROJECT_DATA*  getExportProjectDialogData(HWND  hDialog)
{
   return (EXPORT_PROJECT_DATA*)GetWindowLong(hDialog, DWL_USER);
}


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : displayExportProjectDialog
// Description     : Display the 'Export Project' dialog to query the type of document to create
// 
// MAIN_WINDOW_DATA*  pMainWindowData   : [in] Main window data
// 
// Return Value   : TRUE/FALSE
// 
BOOL   displayExportProjectDialog(MAIN_WINDOW_DATA*  pMainWindowData, PROJECT_FILE*  pProject)
{
   EXPORT_PROJECT_DATA*  pDialogData;
   
   // Create dialog data
   pDialogData = createExportProjectDialogData(pProject);

   // Display dialog
   if (showDialog(TEXT("EXPORT_PROJECT_DIALOG"), pMainWindowData->hMainWnd, dlgprocExportProjectDialog, (LPARAM)pDialogData))
      return TRUE;

   // Error
   deleteExportProjectDialogData(pDialogData);
   return FALSE;
}


/// Function name  : initExportProjectDialog
// Description     : Initialise the dialog 
// 
// CONST EXPORT_PROJECT_DATA*  pDialogData : [in] Dialog data
// HWND                        hDialog     : [in] Dialog handle
// 
// Return Value   : TRUE
// 
BOOL  initExportProjectDialog(EXPORT_PROJECT_DATA*  pDialogData, HWND  hDialog)
{
   // Store document data
   SetWindowLong(hDialog, DWL_USER, (LONG)pDialogData);
   pDialogData->hDialog = hDialog;

   // Create tooltips
   pDialogData->hTooltip = createTooltipWindow(hDialog);
   /*addTooltipTextToControl(pDialogData->hTooltip, hDialog, IDC_EXPORT_FILE_RADIO);
   addTooltipTextToControl(pDialogData->hTooltip, hDialog, IDC_EXPORT_ZIP_RADIO);
   addTooltipTextToControl(pDialogData->hTooltip, hDialog, IDC_EXPORT_FILENAME_EDIT);
   addTooltipTextToControl(pDialogData->hTooltip, hDialog, IDC_EXPORT_FOLDER_EDIT);*/

   // [OWNER DRAW]
   utilAddWindowStyle(GetControl(hDialog, IDC_DIALOG_TITLE), SS_OWNERDRAW);
   utilAddWindowStyle(GetControl(hDialog, IDC_DIALOG_HEADING_1), SS_OWNERDRAW);
   utilAddWindowStyle(GetControl(hDialog, IDC_DIALOG_HEADING_2), SS_OWNERDRAW);

   // Display initial file/folder
   SetDlgItemText(hDialog, IDC_EXPORT_FILENAME_EDIT, pDialogData->szFileName);
   SetDlgItemText(hDialog, IDC_EXPORT_FOLDER_EDIT, pDialogData->szFolder);

   // Check 'ZIP' by default
   CheckRadioButton(hDialog, IDC_EXPORT_FILE_RADIO, IDC_EXPORT_ZIP_RADIO, IDC_EXPORT_ZIP_RADIO);
   SetFocus(GetDlgItem(hDialog, IDC_EXPORT_FILENAME_EDIT));

   // Centre dialog + Update state
   utilCentreWindow(getAppWindow(), hDialog);
   updateExportProjectDialog(pDialogData);

   // Return TRUE
   pDialogData->bInitialised = TRUE;
   return TRUE;
}


/// Function name  : updateExportProjectDialog
// Description     : Enable/disable the OK button
// 
// EXPORT_PROJECT_DATA*  pDialogData  : [in] New Document dialog data
// 
VOID   updateExportProjectDialog(EXPORT_PROJECT_DATA*  pDialogData)
{
   TCHAR   *szFileName,       // Current filename
           *szFolder;         // Current folder

   // Prepare
   szFileName = utilGetDlgItemPath(pDialogData->hDialog, IDC_EXPORT_FILENAME_EDIT);
   szFolder   = utilGetDlgItemPath(pDialogData->hDialog, IDC_EXPORT_FOLDER_EDIT);

   // Update dialog data
   StringCchCopy(pDialogData->szFileName, MAX_PATH, szFileName);
   StringCchCopy(pDialogData->szFolder, MAX_PATH, szFolder);

   // Enable/Disable 'Filename' edit when 'ZIP' is selected
   utilEnableDlgItem(pDialogData->hDialog, IDC_EXPORT_FILENAME_EDIT, IsDlgButtonChecked(pDialogData->hDialog, IDC_EXPORT_ZIP_RADIO));

   // Enable/Disable OK button when folder path exists  (Zip files require a filename also)
   BOOL bEnabled = !IsDlgButtonChecked(pDialogData->hDialog, IDC_EXPORT_ZIP_RADIO) ? PathFileExists(szFolder) : lstrlen(szFileName) AND PathFileExists(szFolder);
   utilEnableDlgItem(pDialogData->hDialog, IDOK, bEnabled);

   // Cleanup
   utilDeleteStrings(szFileName, szFolder);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onExportProjectDialog_Browse
// Description     : Browse for folder
// 
// EXPORT_PROJECT_DATA*  pDialogData : [in] Dialog data
// 
VOID  onExportProjectDialog_Browse(EXPORT_PROJECT_DATA*  pDialogData)
{
   BROWSEINFO   oBrowseData;      // Properties for the browse window
   ITEMIDLIST  *pFolderObject;    // The IDList of the folder the user selects
                     
   // Prepare
   utilZeroObject(&oBrowseData, BROWSEINFO);

   // Setup dialog: "Select destination folder for project files" 
   oBrowseData.lpszTitle = loadTempString(IDS_GENERAL_PROJECT_EXPORT_BROWSE);
   oBrowseData.hwndOwner = pDialogData->hDialog;
   oBrowseData.ulFlags   = BIF_NEWDIALOGSTYLE | BIF_RETURNONLYFSDIRS;

   /// Query user for a folder
   if (pFolderObject = SHBrowseForFolder(&oBrowseData))
   {
      // [SUCCESS] Resolve path
      SHGetPathFromIDList(pFolderObject, pDialogData->szFolder);
      PathAddBackslash(pDialogData->szFolder);

      // Display path
      SetDlgItemText(pDialogData->hDialog, IDC_EXPORT_FOLDER_EDIT, pDialogData->szFolder);

      // Cleanup
      CoTaskMemFree(pFolderObject);
   }
}


/// Function name  : onExportProjectDialog_Command
// Description     : Enables/Disable the OK button in response to change of document type, filename and path.  Also generates dialog return values
// 
// EXPORT_PROJECT_DATA*  pDialogData    : [in] Dialog data
// HWND                hDialog        : [in] Window handle of the 'New Document dialog
// CONST UINT          iControlID     : [in] ID of the control sending the command
// CONST UINT          iNotification  : [in] Notification code
// HWND                hCtrl          : [in] Control sending the commanmd
// 
// Return Value   : TRUE if processed, FALSE otherwise
// 
BOOL   onExportProjectDialog_Command(EXPORT_PROJECT_DATA*  pDialogData, CONST UINT  iControlID, CONST UINT  iNotification, HWND  hCtrl)
{
   // Examine button
   switch (iControlID)
   {
   /// [FILE/FOLDER] Enable 'OK' button when file/folder are valid
   case IDC_EXPORT_FILENAME_EDIT:
   case IDC_EXPORT_FOLDER_EDIT:
      if (iNotification == EN_CHANGE && pDialogData->bInitialised)
         updateExportProjectDialog(pDialogData);
      break;

   /// [FILE/ZIP] Enable/Disable the 'Filename' edit
   case IDC_EXPORT_FILE_RADIO:
   case IDC_EXPORT_ZIP_RADIO:
      updateExportProjectDialog(pDialogData);
      break;

   /// [BROWSE] Browse for folder
   case IDC_EXPORT_FOLDER_BROWSE:
      onExportProjectDialog_Browse(pDialogData);
      return TRUE;

   /// [OK] Export files
   case IDOK:
      onExportProjectDialog_OK(pDialogData);
      EndDialog(pDialogData->hDialog, IDOK);
      return TRUE;

   /// [CANCEL] Return NULL
   case IDCANCEL:
      EndDialog(pDialogData->hDialog, IDCANCEL);
      return TRUE;
   }

   // [UNHANDLED] Return FALSE
   return FALSE;
}



/// Function name  : onExportProjectDialog_OK
// Description     : Exports the project files to a ZIP or folder
// 
// EXPORT_PROJECT_DATA*  pDialogData   : [in] Dialog data
// 
VOID  onExportProjectDialog_OK(EXPORT_PROJECT_DATA*  pDialogData)
{
   STORED_DOCUMENT   oProjectDoc,
                    *pDocument;
   TCHAR            *szDestination;
   UINT              iCopied = 0;
   LIST*             pDocumentsToSave;
   const TCHAR*      szFormat;
   UINT              iResult;
   CHAR              szError[256];

   // Generate list of documents to export
   pDocumentsToSave = createList(NULL);
   szDestination = utilCreateEmptyPath();
   
   // Iterate through MSCI scripts
   for (UINT iIndex = 0; findDocumentInProjectFileByIndex(pDialogData->pProject, PF_SCRIPT, iIndex, pDocument); iIndex++)
      appendItemToList(pDocumentsToSave, createListItem((LPARAM)pDocument));

   // Iterate through language files
   for (UINT iIndex = 0; findDocumentInProjectFileByIndex(pDialogData->pProject, PF_LANGUAGE, iIndex, pDocument); iIndex++)
      appendItemToList(pDocumentsToSave, createListItem((LPARAM)pDocument));

   // Manually insert project file
   oProjectDoc.eType = FIF_PROJECT;
   StringCchCopy(oProjectDoc.szFullPath, MAX_PATH, pDialogData->pProject->szFullPath);
   appendItemToList(pDocumentsToSave, createListItem((LPARAM)&oProjectDoc));

   // ZIP: Export all files into a ZIP file
   if (IsDlgButtonChecked(pDialogData->hDialog, IDC_EXPORT_ZIP_RADIO))
   {
      VERBOSE("Exporting project files to ZIP: %s %s", pDialogData->szFolder, pDialogData->szFileName);
      HZIP hZip;

      // Generate ZIP filename
      PathRemoveBackslash(pDialogData->szFolder);
      StringCchPrintf(szDestination, MAX_PATH, TEXT("%s\\%s"), pDialogData->szFolder, pDialogData->szFileName);

      // [EXISTS] "The file '%s' already exists in the target folder, overwrite?"
      if (!PathFileExists(szDestination) || displayMessageDialogf(pDialogData->hDialog, IDS_GENERAL_PROJECT_EXPORT_FILE_EXISTS, MDF_YESNO WITH MDF_QUESTION, szDestination) == IDYES)
      {
         // Create zip file
         if ((hZip = CreateZip(szDestination, 0, ZIP_FILENAME)) == NULL)
            VERBOSE("Unable to create ZIP File. hZip=%d", hZip);
         else 
         {  
            // Iterate through documents
            for (UINT iIndex = 0; findListObjectByIndex(pDocumentsToSave, iIndex, (LPARAM&)pDocument); iIndex++)
            {
               // Select folder
               switch (extractFileItemType(pDocument->eType))
               {
               case FIF_SCRIPT:    szFormat = TEXT("scripts\\%s");   break;
               case FIF_LANGUAGE:  szFormat = TEXT("t\\%s");         break;
               case FIF_PROJECT:   szFormat = TEXT("%s");            break;
               default:           continue;
               }
               // Generate path
               StringCchPrintf(szDestination, MAX_PATH, szFormat, PathFindFileName(pDocument->szFullPath));
               VERBOSE("Compressing file: '%s' as '%s'", pDocument->szFullPath, szDestination);

               // Add file to ZIP
               if ((iResult=ZipAdd(hZip, szDestination, pDocument->szFullPath, 0, ZIP_FILENAME)) == ZR_OK)
                  iCopied++;
               else
               {
                  FormatZipMessage(iResult, szError, 256);
                  VERBOSE(szError);
               }

            }

            // Close file
            CloseZip(hZip);
         }
      }
   }
   else
   {
      VERBOSE("Exporting project files to folder: %s", pDialogData->szFolder);
      PathRemoveBackslash(pDialogData->szFolder);

      // FILES: Export all files into a target folder
      for (UINT iIndex = 0; findListObjectByIndex(pDocumentsToSave, iIndex, (LPARAM&)pDocument); iIndex++)
      {
         // Generate appropriate path
         switch (extractFileItemType(pDocument->eType))
         {
         case FIF_SCRIPT:    szFormat = TEXT("%s\\scripts\\");   break;
         case FIF_LANGUAGE:  szFormat = TEXT("%s\\t\\");         break;
         case FIF_PROJECT:   szFormat = TEXT("%s\\");            break;
         default:           continue;
         }

         // Ensure folder exists
         StringCchPrintf(szDestination, MAX_PATH, szFormat, pDialogData->szFolder);
         CreateDirectory(szDestination, NULL);

         // Generate target path 
         StringCchCat(szDestination, MAX_PATH, PathFindFileName(pDocument->szFullPath));

         // Copy file but do not overwrite if exists
         if (CopyFile(pDocument->szFullPath, szDestination, TRUE))
            iCopied++;
         else
         {
            // [QUESTION] "The file '%s' already exists in the target folder, overwrite?"
            if (displayMessageDialogf(pDialogData->hDialog, IDS_GENERAL_PROJECT_EXPORT_FILE_EXISTS, MDF_YESNO WITH MDF_QUESTION, PathFindFileName(pDocument->szFullPath)) == IDYES)
               iCopied += CopyFile(pDocument->szFullPath, szDestination, FALSE);
         }
      }
   }

   // Store file/folder in preferences
   PathAddBackslash(pDialogData->szFolder);
   setAppPreferencesBackupFolder(pDialogData->szFolder);
   setAppPreferencesBackupFileName(pDialogData->szFileName);
   PathRemoveBackslash(pDialogData->szFolder);

   // [SUCCESS] "Successfully exported %d project files to '%s'"
   if (iCopied == getListItemCount(pDocumentsToSave))
      displayMessageDialogf(pDialogData->hDialog, IDS_GENERAL_PROJECT_EXPORT_SUCCESS, MDF_OK WITH MDF_INFORMATION, iCopied, pDialogData->szFolder);
   else
      // [FAILURE] "Only %d of %d project files were successfully exported to '%s'"
      displayMessageDialogf(pDialogData->hDialog, IDS_GENERAL_PROJECT_EXPORT_FAILED, MDF_OK WITH MDF_INFORMATION, iCopied, getListItemCount(pDocumentsToSave), pDialogData->szFolder);   

   // Cleanup
   deleteList(pDocumentsToSave);
   utilDeleteString(szDestination);
}



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : dlgprocExportProjectDialog
// Description     : Window procedure for the 'Insert Argument' dialog
// 
// 
INT_PTR  dlgprocExportProjectDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   EXPORT_PROJECT_DATA*  pDialogData = getExportProjectDialogData(hDialog);
   BOOL                  bResult = FALSE;

   // Examine message
   switch (iMessage)
   {
   /// [CREATION] -- Init dialog
   case WM_INITDIALOG:
      bResult = initExportProjectDialog((EXPORT_PROJECT_DATA*)lParam, hDialog);
      break;

   /// [DESTRUCTION] -- Cleanup
   case WM_DESTROY:
      deleteExportProjectDialogData(pDialogData);
      break;

   /// [COMMAND] - Process text/selection change
   case WM_COMMAND:
      bResult = onExportProjectDialog_Command(pDialogData, LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
      break;

   /// [OWNER DRAW]
   case WM_DRAWITEM:
      switch (wParam)
      {
      case IDC_DIALOG_ICON:       bResult = onOwnerDrawStaticBitmap(lParam, TEXT("EXPORT_PROJECT_BITMAP"), 96, 256);  break;
      case IDC_DIALOG_TITLE:      bResult = onOwnerDrawStaticTitle(lParam);      break;
      case IDC_DIALOG_HEADING_1:  bResult = onOwnerDrawStaticHeading(lParam);    break;
      case IDC_DIALOG_HEADING_2:  bResult = onOwnerDrawStaticHeading(lParam);    break;
      }
      break;
   }

   // Return result
   return (bResult ? TRUE : dlgprocVistaStyleDialog(hDialog, iMessage, wParam, lParam));
}


