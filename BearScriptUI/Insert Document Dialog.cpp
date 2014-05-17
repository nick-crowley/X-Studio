//
// Insert Document Dialog.cpp : 'Insert Document' dialog
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"


// Creation / Destruction
NEW_DOCUMENT_DATA*  createInsertDocumentDialogData();

// Helpers
NEW_DOCUMENT_DATA*  getInsertDocumentDialogData(HWND  hDialog);

// Functions
VOID                updateInsertDocumentDialog(NEW_DOCUMENT_DATA*  pDialogData);
BOOL                initInsertDocumentDialog(NEW_DOCUMENT_DATA*  pDialogData, HWND  hDialog);

// Message Handlers
BOOL     onInsertDocumentDialog_Command(NEW_DOCUMENT_DATA*  pDialogData, CONST UINT  iControlID, CONST UINT  iNotification, HWND  hCtrl);
VOID     onInsertDocumentDialog_DoubleClick(NEW_DOCUMENT_DATA*  pDialogData, NMITEMACTIVATE*  pMessage);
BOOL     onInsertDocumentDialog_Notify(NEW_DOCUMENT_DATA*  pDialogData, NMHDR*  pMessage);
VOID     onInsertDocumentDialog_OK(NEW_DOCUMENT_DATA*  pDialogData);
VOID     onInsertDocumentDialog_RequestData(NEW_DOCUMENT_DATA*  pDialogData, NMLVDISPINFO*  pHeader);
VOID     onInsertDocumentDialog_SelectionChange(NEW_DOCUMENT_DATA*  pDialogData, NMLISTVIEW*  pListView);

// Window procedure
INT_PTR  dlgprocInsertDocumentDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       CONSTANTS / GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

CONST UINT  iDocumentTypeCount     = 3;

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                      CREATION / DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createInsertDocumentDialogData
// Description     : Create 'Insert Document' dialog data
// 
// Return Value   : New dialog data, you are responsible for destroying it
// 
NEW_DOCUMENT_DATA*  createInsertDocumentDialogData()
{
   NEW_DOCUMENT_DATA*  pNewData = utilCreateEmptyObject(NEW_DOCUMENT_DATA);

   /// No properties to create
   
   // Return object
   return pNewData;
}


/// Function name  : deleteInsertDocumentDialogData
// Description     : Delete 'Insert Document' dialog data
// 
// NEW_DOCUMENT_DATA*  &pData   : [in] Dialog data
// 
VOID  deleteInsertDocumentDialogData(NEW_DOCUMENT_DATA*  &pData)
{
   // [OPTIONAL] Delete path 
   utilSafeDeleteString(pData->szFullPath);

   // Delete calling object
   utilDeleteObject(pData);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                            HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : getInsertDocumentDialogData
// Description     : Retrieve dialog data from a 'Insert Document' dialog
// 
// HWND  hDialog   : [in] 'Insert Document' dialog window handle
// 
// Return Value   : Dialog data
// 
NEW_DOCUMENT_DATA*  getInsertDocumentDialogData(HWND  hDialog)
{
   return (NEW_DOCUMENT_DATA*)GetWindowLong(hDialog, DWL_USER);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                           FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : displayInsertDocumentDialog
// Description     : Display the 'Insert Document' dialog to query the type of document to create
// 
// MAIN_WINDOW_DATA*  pMainWindowData   : [in] Main window data
// 
// Return Value   : New dialog data if successful, otherwise NULL
// 
NEW_DOCUMENT_DATA*   displayInsertDocumentDialog(MAIN_WINDOW_DATA*  pMainWindowData)
{
   NEW_DOCUMENT_DATA*  pDialogData;
   
   // Create New Document data
   pDialogData = createInsertDocumentDialogData();

   // Display dialog
   if (!showDialog(TEXT("INSERT_DOCUMENT_DIALOG"), pMainWindowData->hMainWnd, dlgprocInsertDocumentDialog, (LPARAM)pDialogData))
      deleteInsertDocumentDialogData(pDialogData);

   // Return dialog data or NULL
   return pDialogData;
}


/// Function name  : initInsertDocumentDialog
// Description     : Initialise the dialog
// 
// NEW_DOCUMENT_DATA*  pDialogData  : [in] Dialog data
// HWND                hDialog      : [in] InsertDocument dialog
// 
// Return Value   : TRUE
// 
BOOL   initInsertDocumentDialog(NEW_DOCUMENT_DATA*  pDialogData, HWND  hDialog)
{
   HIMAGELIST  hImageList;       // ImageList of the ListView
   TCHAR      *szProgramFolder,  // Program folder
              *szScriptFolder;   // Scripts folder

   /// Store document data
   SetWindowLong(hDialog, DWL_USER, (LONG)pDialogData);
   pDialogData->hDialog = hDialog;
   
   // Prepare
   pDialogData->hListView = GetDlgItem(pDialogData->hDialog, IDC_DOCUMENT_LIST);
   szProgramFolder        = utilCreateEmptyPath();
   
   /// Assign custom 32x32 ListView ImageList
   hImageList = utilCreateImageList(getResourceInstance(), 32, 4, "NEW_SCRIPT_FILE_ICON", "NEW_LANGUAGE_FILE_ICON", "NEW_MISSION_FILE_ICON", "NEW_PROJECT_FILE_ICON");
   ListView_SetImageList(pDialogData->hListView, hImageList, LVSIL_NORMAL);

   /// Populate document types
   ListView_SetItemCount(pDialogData->hListView, 4);

   // Retrieve 'scripts' folder, and program folder
   szScriptFolder = generateGameDataFilePath(GFI_SCRIPT_FOLDER, getAppPreferences()->szGameFolder, getAppPreferences()->eGameVersion);
   GetModuleFileName(NULL, szProgramFolder, MAX_PATH);
   utilFindCharacterReverse(szProgramFolder, '\\')[1] = NULL;
   PathAddBackslash(szProgramFolder);

   /// Populate folders
   ComboBox_AddString(GetControl(pDialogData->hDialog, IDC_DOCUMENT_LOCATION_COMBO), szScriptFolder);
   ComboBox_AddString(GetControl(pDialogData->hDialog, IDC_DOCUMENT_LOCATION_COMBO), getAppPreferences()->szGameFolder);
   ComboBox_AddString(GetControl(pDialogData->hDialog, IDC_DOCUMENT_LOCATION_COMBO), szProgramFolder);

   /// Set initial filename and path
   SetDlgItemText(pDialogData->hDialog, IDC_DOCUMENT_FILENAME_EDIT, TEXT("Untitled.xml"));
   ComboBox_SetCurSel(GetControl(pDialogData->hDialog, IDC_DOCUMENT_LOCATION_COMBO), 0);
     
   // [OWNER DRAW]
   utilAddWindowStyle(GetControl(hDialog, IDC_DOCUMENT_DIALOG_ICON),     SS_OWNERDRAW);
   utilAddWindowStyle(GetControl(hDialog, IDC_DOCUMENT_TITLE_STATIC),    SS_OWNERDRAW);
   utilAddWindowStyle(GetControl(hDialog, IDC_DOCUMENT_TYPE_STATIC),     SS_OWNERDRAW);
   utilAddWindowStyle(GetControl(hDialog, IDC_DOCUMENT_LOCATION_STATIC), SS_OWNERDRAW);

   // Centre dialog
   utilCentreWindow(getAppWindow(), hDialog);

   // Display description and disable OK button
   SetDlgItemText(pDialogData->hDialog, IDC_DOCUMENT_DESCRIPTION_STATIC, TEXT("None"));
   updateInsertDocumentDialog(pDialogData);

   // Initially Select 'MSCI Script'
   ListView_SetItemState(pDialogData->hListView, 0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);

   // Cleanup and return TRUE
   utilDeleteStrings(szProgramFolder, szScriptFolder);
   return TRUE;
}


/// Function name  : updateInsertDocumentDialog
// Description     : Enable/disable the OK button
// 
// NEW_DOCUMENT_DATA*  pDialogData  : [in] New Document dialog data
// 
VOID   updateInsertDocumentDialog(NEW_DOCUMENT_DATA*  pDialogData)
{
   TCHAR   *szFileName,       // Current filename
           *szFolder;         // Current folder
   BOOL     bOKEnabled;       // Whether to enable/disable OK button

   // Prepare
   szFileName = utilGetDlgItemPath(pDialogData->hDialog, IDC_DOCUMENT_FILENAME_EDIT);
   szFolder   = utilGetDlgItemPath(pDialogData->hDialog, IDC_DOCUMENT_LOCATION_COMBO);

   // Examine selection
   switch (ListView_GetNextItem(pDialogData->hListView, -1, LVNI_SELECTED))
   {
   /// [SCRIPT/LANGUAGE/PROJECT] Allow
   case NDT_PROJECT:
   case NDT_LANGUAGE:
   case NDT_SCRIPT:     bOKEnabled = TRUE;  break;
   /// [MISSION] Deny
   default:             bOKEnabled = FALSE; break;
   }
   
   // [CHECK] Ensure filename/folder are valid
   if (bOKEnabled)
      bOKEnabled = lstrlen(szFileName) AND PathFileExists(szFolder);

   // Enable/Disable OK button and cleanup
   utilEnableDlgItem(pDialogData->hDialog, IDOK, bOKEnabled);
   utilDeleteStrings(szFileName, szFolder);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          MESSAGE HANDLERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onInsertDocumentDialog_Command
// Description     : Enables/Disable the OK button in response to change of document type, filename and path.  Also generates dialog return values
// 
// NEW_DOCUMENT_DATA*  pDialogData    : [in] Dialog data
// HWND                hDialog        : [in] Window handle of the 'New Document dialog
// CONST UINT          iControlID     : [in] ID of the control sending the command
// CONST UINT          iNotification  : [in] Notification code
// HWND                hCtrl          : [in] Control sending the commanmd
// 
// Return Value   : TRUE if processed, FALSE otherwise
// 
BOOL   onInsertDocumentDialog_Command(NEW_DOCUMENT_DATA*  pDialogData, CONST UINT  iControlID, CONST UINT  iNotification, HWND  hCtrl)
{
   // Examine button
   switch (iControlID)
   {
   /// [FILENAME]
   case IDC_DOCUMENT_FILENAME_EDIT:
      if (iNotification == EN_CHANGE)
         // Disable the 'OK' button if Filename is empty
         updateInsertDocumentDialog(pDialogData);
      break;

   /// [PATH]
   case IDC_DOCUMENT_LOCATION_COMBO:
      if (iNotification == CBN_EDITCHANGE)
         // Disable the 'OK' button if path does not exist
         updateInsertDocumentDialog(pDialogData);
      break;

   /// [OK] Store path + selection and return dialog data
   case IDOK:
      onInsertDocumentDialog_OK(pDialogData);
      return TRUE;

   /// [CANCEL] -- Return NULL
   case IDCANCEL:
      EndDialog(pDialogData->hDialog, NULL);
      return TRUE;
   }

   // [UNHANDLED] Return FALSE
   return FALSE;
}


/// Function name  : onInsertDocumentDialog_DoubleClick
// Description     : Emulates clicking the OK button
// 
// NEW_DOCUMENT_DATA*  pDialogData : [in] Dialog data
// NMITEMACTIVATE*     pMessage    : [in] Ignored
// 
VOID  onInsertDocumentDialog_DoubleClick(NEW_DOCUMENT_DATA*  pDialogData, NMITEMACTIVATE*  pMessage)
{
   // [CHECK] Are current values valid?
   if (IsWindowEnabled(GetControl(pDialogData->hDialog, IDOK)))
      /// [SUCCESS] Accept current values
      onInsertDocumentDialog_OK(pDialogData);
}


/// Function name  : onInsertDocumentDialog_Notify
// Description     : Displays the description for the newly selected document type
// 
// NEW_DOCUMENT_DATA*  pDialogData  : [in] 'Insert Document' dialog data
// NMHDR*              pMessage     : [in] WM_NOTIFY message header
// 
// Return Value   : TRUE if processed, FALSE otherwise
// 
BOOL   onInsertDocumentDialog_Notify(NEW_DOCUMENT_DATA*  pDialogData, NMHDR*  pMessage)
{
   // Examine notification
   switch (pMessage->idFrom)
   {
   /// [DOCUMENT LIST]
   case IDC_DOCUMENT_LIST:
      switch (pMessage->code)
      {
      /// [DOUBLE-CLICK]
      case NM_DBLCLK:
         onInsertDocumentDialog_DoubleClick(pDialogData, (NMITEMACTIVATE*)pMessage);
         return TRUE;

      /// [REQUEST DATA]
      case LVN_GETDISPINFO:
         onInsertDocumentDialog_RequestData(pDialogData, (NMLVDISPINFO*)pMessage);
         return TRUE;

      /// [SELECTION CHANGED]
      case LVN_ITEMCHANGED:
         // [SELECTION CHANGED] Update description and OK button
         onInsertDocumentDialog_SelectionChange(pDialogData, (NMLISTVIEW*)pMessage);
         return TRUE;
      }
      break;
   }

   // [UNHANDLED] return FALSE
   return FALSE;
}


/// Function name  : onInsertDocumentDialog_OK
// Description     : Assembles output path and stores result
// 
// NEW_DOCUMENT_DATA*  pDialogData   : [in] Dialog data
// 
VOID  onInsertDocumentDialog_OK(NEW_DOCUMENT_DATA*  pDialogData)
{
   TCHAR  *szFileName;     // Current filename

   // Prepare
   pDialogData->szFullPath = utilGetDlgItemPath(pDialogData->hDialog, IDC_DOCUMENT_LOCATION_COMBO);
   szFileName              = utilGetDlgItemPath(pDialogData->hDialog, IDC_DOCUMENT_FILENAME_EDIT);
   
   /// Store OutputPath and DocumentType
   PathAppend(pDialogData->szFullPath, szFileName);
   pDialogData->eNewDocumentType = (NEW_DOCUMENT_TYPE)ListView_GetNextItem(pDialogData->hListView, -1, LVNI_SELECTED);
   
   // Cleanup and return dialog data
   utilDeleteString(szFileName);
   EndDialog(pDialogData->hDialog, (INT_PTR)pDialogData);
}


/// Function name  : onInsertDocumentDialog_RequestData
// Description     : Supplies virtual ListView data
// 
// NEW_DOCUMENT_DATA*  pDialogData : [in] Dialog data
// NMLVDISPINFO*       pHeader     : [in] ListView message header
// 
VOID  onInsertDocumentDialog_RequestData(NEW_DOCUMENT_DATA*  pDialogData, NMLVDISPINFO*  pHeader)
{
   LVITEM*  pItem;      // Convenience pointer

   // Prepare
   pItem = &pHeader->item;

   // [TEXT]
   if (pItem->mask INCLUDES LVIF_TEXT)
      loadString(IDS_DOCUMENT_TYPE_SCRIPT + pItem->iItem, pItem->pszText, pItem->cchTextMax);

   // [ICON]
   if (pItem->mask INCLUDES LVIF_IMAGE)
      pItem->iImage = pItem->iItem;
}


/// Function name  : onInsertDocumentDialog_SelectionChange
// Description     : Updates the document type description and filename extension
// 
// NEW_DOCUMENT_DATA*  pDialogData  : [in] Dialog data
// CONST UINT          iNewItem     : [in] Item index
// 
VOID  onInsertDocumentDialog_SelectionChange(NEW_DOCUMENT_DATA*  pDialogData, NMLISTVIEW*  pListView)
{
   CONST TCHAR*  szDefaultExtension;   // Default filetype extension
   TCHAR*        szFileName;           // Current filename
   INT           iSelection;           // Index of selected item

   // Prepare
   iSelection = ListView_GetNextItem(pDialogData->hListView, -1, LVNI_ALL WITH LVNI_SELECTED);
   szFileName = utilGetDlgItemPath(pDialogData->hDialog, IDC_DOCUMENT_FILENAME_EDIT);

   // [CHECK] Display appropriate document description
   if (iSelection != -1)
      /// [SELECTION] Display new document type description
      SetDlgItemText(pDialogData->hDialog, IDC_DOCUMENT_DESCRIPTION_STATIC, loadTempString(IDS_DOCUMENT_DESCRIPTION_SCRIPT+iSelection));
   else
      /// [NO SELECTION] Erase description
      SetDlgItemText(pDialogData->hDialog, IDC_DOCUMENT_DESCRIPTION_STATIC, TEXT("None"));

   // Determine document extension
   switch (iSelection)
   {
   // [PROJECT] Use .xprj extension
   case NDT_PROJECT: szDefaultExtension = TEXT(".xprj");      break;
   // [DEFAULT] Use .xml extension
   default:          szDefaultExtension = TEXT(".xml");       break;
   }

   // [CHECK] Does filename exist?
   if (lstrlen(szFileName))
      /// [FILENAME] Change extension
      PathRenameExtension(szFileName, szDefaultExtension);
   else
      /// [EMPTY] Replace with fixed string
      StringCchPrintf(szFileName, MAX_PATH, TEXT("Untitled%s"), szDefaultExtension);
   
   // Set filename and cleanup
   SetDlgItemText(pDialogData->hDialog, IDC_DOCUMENT_FILENAME_EDIT, szFileName);
   utilDeleteString(szFileName);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         MESSAGE HANDLERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : dlgprocInsertDocumentDialog
// Description     : 'Insert Document' dialog window procedure
// 
// 
INT_PTR  dlgprocInsertDocumentDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   NEW_DOCUMENT_DATA*  pDialogData;
   BOOL                bResult;

   // Get dialog data
   pDialogData = getInsertDocumentDialogData(hDialog);
   bResult     = FALSE;

   switch (iMessage)
   {
   /// [CREATION] -- Intialise controls
   case WM_INITDIALOG:
      bResult = initInsertDocumentDialog((NEW_DOCUMENT_DATA*)lParam, hDialog);
      break;

   /// [COMMAND PROCESSING]
   case WM_COMMAND:
      bResult = onInsertDocumentDialog_Command(pDialogData, LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
      break;

   /// [NOTIFICATIONS]
   case WM_NOTIFY:
      bResult = onInsertDocumentDialog_Notify(pDialogData, (NMHDR*)lParam);
      break;

   /// [OWNER DRAW]
   case WM_DRAWITEM:
      switch (wParam)
      {
      case IDC_DOCUMENT_DIALOG_ICON:      bResult = onOwnerDrawStaticIcon(lParam, TEXT("NEW_FILE_ICON"), 96);    break;
      case IDC_DOCUMENT_TITLE_STATIC:     bResult = onOwnerDrawStaticTitle(lParam);      break;
      case IDC_DOCUMENT_TYPE_STATIC:      bResult = onOwnerDrawStaticHeading(lParam);    break;
      case IDC_DOCUMENT_LOCATION_STATIC:  bResult = onOwnerDrawStaticHeading(lParam);    break;
      }
      break;
   }

   // Return result
   return (bResult ? TRUE : dlgprocVistaStyleDialog(hDialog, iMessage, wParam, lParam));
}