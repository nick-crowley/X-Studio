//
// File Dialog.cpp : Open / Close file dialog
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        CONSTANTS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// REPLACE USING SHGetIconOverlayIndex
// Fixed indicies into the system image list containing the 'shared' and 'shortcut' overlay icons
#define          SYSTEM_ICON_ID_SHARED         0
#define          SYSTEM_ICON_ID_SHORTCUT       1

// ListView Column IDs
#define          FILE_COLUMN_NAME              0
#define          FILE_COLUMN_DESCRIPTION       1
#define          FILE_COLUMN_VERSION           2
#define          FILE_COLUMN_TYPE              3
#define          FILE_COLUMN_SIZE              4

// Number of columns
#define          FILE_COLUMNS_COUNT            5

// Minimum Dialog size
CONST SIZE        iMinimumDialogSize             = { 740, 570 };

// JumpBar properties
CONST UINT        iJumpBarItemCount              = 6,     // Number of items
                  iJumpBarIconSize               = 32;    // Size of icons, in pixels

// Toolbar
CONST UINT        iToolBarButtonCount            = 2;

// Colour used for highlighting. Cannot extract from visual styles, no matter how hard i try!
CONST COLORREF    clSortColumnBackground         = RGB(247,247,247),
                  clJumpItemBackground           = RGB(247,247,247);

// Text Colour used to display virtual files/folders
CONST COLORREF    clVirtualFile                  = RGB(0,0,255),
                  clVirtualPlaceholder           = RGB(174,186,225);

// Filters displayed for the system OpenFile dialog
//
CONST TCHAR*  szSystemDialogFilter = TEXT("All Supported Documents (*.xml, *.pck, *.xprj)\0") TEXT("*.PCK;*.XML;*.XPRJ\0")
                                     TEXT("Project Files (*.xprj)\0") TEXT("*.XPRJ\0")
                                     TEXT("Compressed Files (*.pck)\0") TEXT("*.PCK\0")
                                     TEXT("Uncompressed Files (*.xml)\0") TEXT("*.XML\0")
                                     TEXT("All Files (*.*)\0") TEXT("*.*\0\0");

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION  /  DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : displayFileDialog
// Description     : Displays a modal Open File dialog
//
// FILE_DIALOG_DATA*  pDialogData   : [in/out] Dialog data
// HWND               hParentWnd    : [in]     Parent window
// CONST BOOL         bSystemDialog : [in]     TRUE to use system dialog, FALSE to use browser
// 
// Return type : IDOK or IDCANCEL
//
BOOL    displayFileDialog(FILE_DIALOG_DATA*  pDialogData, HWND  hParentWnd, CONST BOOL  bSystemDialog)
{
   OPENFILENAME  oDialogData;    // System dialog data
   CONST TCHAR*  szFolder;
   TCHAR*        szFullPath;
   BOOL          bResult;        // Operation result
   
   // [CHECK] Are we displaying the system dialog?
   if (!bSystemDialog)
   {
      /// Display browser dialog
      bResult = (INT_PTR)DialogBoxParam(getResourceInstance(), TEXT("FILE_DIALOG"), hParentWnd, dlgprocFileDialog, (LPARAM)pDialogData);

      // [CHECK] Ensure dialog was created successfully
      ERROR_CHECK("displaying FileBrowser dialog", bResult != -1);
   }
   else
   {
      // Prepare
      utilZeroObject(&oDialogData, OPENFILENAME);

      // Define properties
      oDialogData.lStructSize     = sizeof(OPENFILENAME);
      oDialogData.hwndOwner       = hParentWnd;
      oDialogData.lpstrTitle      = (pDialogData->eType == FDT_OPEN ? TEXT("Open File...") : TEXT("Save File As..."));
      oDialogData.lpstrFilter     = szSystemDialogFilter;
      oDialogData.lpstrFile       = utilCreateEmptyString(4096);
      oDialogData.lpstrDefExt     = TEXT("xml");
      oDialogData.nMaxFile        = 4096;
      oDialogData.lpstrInitialDir = (lstrlen(getAppPreferences()->szLastFolder) ? getAppPreferences()->szLastFolder : getAppPreferences()->szGameFolder);
      oDialogData.Flags           = OFN_ENABLESIZING WITH OFN_EXPLORER WITH OFN_HIDEREADONLY WITH OFN_NONETWORKBUTTON;

      // [CHECK] Are we opening or saving?
      if (pDialogData->eType == FDT_OPEN)
         // [OPEN] Allow multiple selection, ensure files exist
         oDialogData.Flags |= OFN_ALLOWMULTISELECT WITH OFN_FILEMUSTEXIST WITH OFN_PATHMUSTEXIST;
      else
      {
         // [SAVE] Warn on overwrite, supply initial filename
         oDialogData.Flags |= OFN_OVERWRITEPROMPT;
         StringCchCopy(oDialogData.lpstrFile, 4096, pDialogData->szFileName);
      }

      /// Display system dialog
      if (bResult = (pDialogData->eType == FDT_OPEN ? GetOpenFileName(&oDialogData) : GetSaveFileName(&oDialogData)))
      {
         // Prepare
         szFolder = oDialogData.lpstrFile;
         bResult  = IDOK;

         // [SINGLE FILE] Extract single filename
         if (oDialogData.nFileOffset < lstrlen(szFolder))
            appendFileDialogOutputFile(pDialogData, szFolder, identifyFileType(getFileSystem(), szFolder));

         // [MULTIPLE FILES] Parse returned filenames
         else for (CONST TCHAR*  szFileName = szFolder + lstrlen(szFolder) + 1; szFileName[0]; szFileName += lstrlen(szFileName) + 1)
         {
            // Generate full filepath
            szFullPath = utilDuplicatePath(szFolder);
            PathAppend(szFullPath, szFileName);

            // Add to DialogData
            appendFileDialogOutputFile(pDialogData, szFullPath, identifyFileType(getFileSystem(), szFullPath));
            utilDeleteString(szFullPath);
         }
      }
      else
      {
         // [FAILED]
         consolePrintLastErrorEx(CommDlgExtendedError(), TEXT("displaying System OpenFile dialog"), __WFUNCTION__, __WFILE__, __LINE__);
         bResult = IDCANCEL;
      }

      // Cleanup
      utilDeleteString(oDialogData.lpstrFile);
   }

   // Return dialog result
   return bResult;
}


/// Function name  : initFileDialog
// Description     : Initialise the controls in the file dialog
//
// FILE_DIALOG_DATA*  pDialogData : [in] File dialog data
// 
VOID  initFileDialog(FILE_DIALOG_DATA*  pDialogData, HWND  hDialog)
{
   // Store window data
   SetWindowLong(pDialogData->hDialog = hDialog, DWL_USER, (LONG)pDialogData);

   // Create convenience pointers
   pDialogData->hListView      = GetDlgItem(pDialogData->hDialog, IDC_FILE_LIST);
   pDialogData->hFileEdit      = GetDlgItem(pDialogData->hDialog, IDC_FILENAME_EDIT);
   pDialogData->hFilterCombo   = GetDlgItem(pDialogData->hDialog, IDC_FILETYPE_COMBO);
   pDialogData->hLocationCombo = GetDlgItem(pDialogData->hDialog, IDC_LOCATION_COMBO);

   /// Setup dialog controls
   initFileDialogControls(pDialogData);
   initFileDialogToolbar(pDialogData);
   
   /// Initialise Jumpbar locations
   initFileDialogJumpBarLocations(pDialogData);

   // Set appropriate title and icon
   SetWindowText(pDialogData->hDialog, pDialogData->eType == FDT_OPEN ? TEXT("Open Existing Document") : TEXT("Save Document As..."));
   SetClassLong(pDialogData->hDialog, GCL_HICON, (LONG)LoadIcon(getAppInstance(), szMainWindowClass));

   //SetClassLong(pDialogData->hDialog, GCL_STYLE, CS_NOCLOSE WITH GetClassLong(pDialogData->hDialog, GCL_STYLE));

   // Resize dialog to most appropriate size and center within main window
   SetWindowPos(pDialogData->hDialog, NULL, NULL, NULL, getAppPreferencesWindowSize(AW_BROWSER)->cx, getAppPreferencesWindowSize(AW_BROWSER)->cy, SWP_NOMOVE WITH SWP_NOZORDER);
   utilCentreWindow(getAppWindow(), pDialogData->hDialog);

   /// Display current folder contents
   pDialogData->bSortAscending = TRUE;
   updateFileDialog(pDialogData, FDS_SEARCH); 
}


/// Function name  : initFileDialogControls
// Description     : Setup the controls in the FileDialog
//
// FILE_DIALOG_DATA*  pDialogData : [in] DileDialog window data
// 
VOID  initFileDialogControls(FILE_DIALOG_DATA*  pDialogData)
{
   LISTVIEW_COLUMNS  oListView                        = { 5, IDS_FILE_DIALOG_COLUMN_NAME, 200, 180, 50, 100, 60 };
   CONST TCHAR*      szFilterIcons[FILE_FILTER_COUNT] = { TEXT("NEW_SCRIPT_FILE_ICON"), TEXT("NEW_SCRIPT_FILE_ICON"), TEXT("NEW_SCRIPT_FILE_ICON"), TEXT("NEW_LANGUAGE_FILE_ICON"), TEXT("NEW_MISSION_FILE_ICON"), TEXT("NEW_PROJECT_FILE_ICON"), TEXT("NEW_FILE_ICON"), TEXT("ALL_FILES_ICON") };
   SHFILEINFO        oShellInfo;      // Used to get the system image list   
   TCHAR*            szFilterText;    // 'File Type' filter string
   
   /// [LISTVIEW]
   initReportModeListView(pDialogData->hListView, &oListView, TRUE);
   ListView_SetExtendedListViewStyleEx(pDialogData->hListView, LVS_EX_FULLROWSELECT, NULL);

   /// [SAVE DIALOG] Disable multiple selections
   if (pDialogData->eType == FDT_SAVE)
      utilRemoveWindowStyle(pDialogData->hListView, LVS_SINGLESEL);

   // Highlight NAME column
   ListView_SetSelectedColumn(pDialogData->hListView, FILE_COLUMN_NAME);

   /// [FILTER COMBO] Add filter strings
   for (UINT iFilter = 0; iFilter < FILE_FILTER_COUNT; iFilter++)
   {
      // Load and insert item text
      szFilterText = utilLoadString(getResourceInstance(), IDS_FILE_DIALOG_FILTER_SCRIPT_FILES + iFilter, 64);
      appendCustomComboBoxItemEx(pDialogData->hFilterCombo, szFilterText, NULL, szFilterIcons[iFilter], NULL);
      // Cleanup
      utilDeleteString(szFilterText);
   }
   
   // Select first filter
   pDialogData->eFilter = getAppPreferences()->eLastFileFilter;
   ComboBox_SetCurSel(pDialogData->hFilterCombo, pDialogData->eFilter);

   /// [FILE-NAME] Set initial filename (if any)
   if (pDialogData->eType == FDT_SAVE)
      SetDlgItemText(pDialogData->hDialog, IDC_FILENAME_EDIT, pDialogData->szFileName);

   // Set font
   SetWindowFont(pDialogData->hFileEdit, GetWindowFont(pDialogData->hDialog), FALSE);

   /// [SYSTEM IMAGELIST] Retrieve system image list and assign to ListView and ComboBox
   pDialogData->hImageList = (HIMAGELIST)SHGetFileInfo(TEXT("*.xml"), FILE_ATTRIBUTE_NORMAL, &oShellInfo, sizeof(SHFILEINFO), SHGFI_USEFILEATTRIBUTES WITH SHGFI_SMALLICON WITH SHGFI_ICON WITH SHGFI_SYSICONINDEX);
   ERROR_CHECK("Retrieving system ImageList", pDialogData->hImageList);
   DestroyIcon(oShellInfo.hIcon);

   // Setup overlay icons  
   ImageList_SetOverlayImage(pDialogData->hImageList, SYSTEM_ICON_ID_SHARED,   SYSTEM_ICON_ID_SHARED + 1);
   ImageList_SetOverlayImage(pDialogData->hImageList, SYSTEM_ICON_ID_SHORTCUT, SYSTEM_ICON_ID_SHORTCUT + 1);

   /// TODO: **** USE SHGetIconOverlayIndex INSTEAD ***
   //INT  iShareImageIndex = SHGetIconOverlayIndex(NULL, IDO_SHGIOI_SHARE);
   //INT  iLinkImageIndex = SHGetIconOverlayIndex(NULL, IDO_SHGIOI_LINK);

   // Assign
   ListView_SetImageList(pDialogData->hListView, pDialogData->hImageList, LVSIL_SMALL);

   // [DEBUG]
   /*INT  iWidth, iHeight;
   ImageList_GetIconSize(pDialogData->hImageList, &iWidth, &iHeight);
   VERBOSE("System ImageList contains %u icons of size %u pixels", ImageList_GetImageCount(pDialogData->hImageList), iWidth, iHeight);*/
}


/// Function name  : initFileDialogToolbar
// Description     : Create the folder controls toolbar
// 
// FILE_DIALOG_DATA*  pDialogData  : [in] FileDialog window data
// 
// Return Value   : TRUE if succesful, FALSE otherwise
// 
BOOL  initFileDialogToolbar(FILE_DIALOG_DATA*  pDialogData)
{
   TBADDBITMAP   oToolbarData;                     // Bitmap data for the new toolbar
   TBBUTTON      oButtonData[iToolBarButtonCount]; // Button data for the new toolbar
   RECT          rcListViewRect,    // Files listview rectangle
                 rcComboRect,       // Locations combobox rectangle
                 rcToolbarRect;     // Toolbar rectangle
   UINT          iButtonHeight;     // Height of a toolbar button, in pixels
   HWND          hCtrl;             // New toolbar window handle

   /// Create ToolBar
   hCtrl = CreateWindowEx(NULL, TOOLBARCLASSNAME, NULL, WS_CHILD WITH WS_VISIBLE WITH TBSTYLE_FLAT WITH TBSTYLE_TRANSPARENT WITH TBSTYLE_TOOLTIPS WITH CCS_NOPARENTALIGN WITH CCS_NOMOVEY WITH CCS_NOMOVEX WITH CCS_NORESIZE WITH CCS_NODIVIDER , 
                          0,0,0,0, pDialogData->hDialog, (HMENU)IDC_FILE_TOOLBAR, getResourceInstance(), NULL);
   pDialogData->hToolbar = hCtrl;
   if (pDialogData->hToolbar == NULL)
      return FALSE;

   // Define bitmaps
   oToolbarData.hInst = getResourceInstance();
   oToolbarData.nID   = IDB_FILE_TOOLBAR;

   // Add bitmap
   SendMessage(hCtrl, TB_SETBITMAPSIZE, NULL, MAKE_LONG(24,24));
   SendMessage(hCtrl, TB_ADDBITMAP, iToolBarButtonCount, (LPARAM)&oToolbarData);

   /// Define buttons
   utilZeroObjectArray(oButtonData, TBBUTTON, iToolBarButtonCount);
   for (UINT i = 0, iBitmap = 0; i < iToolBarButtonCount; i++)
   {
      oButtonData[i].iString = NULL; 
      oButtonData[i].fsState = TBSTATE_ENABLED;
      oButtonData[i].fsStyle = BTNS_BUTTON;
      oButtonData[i].iBitmap = i;
   }

   // Set command IDs
   oButtonData[0].idCommand = IDC_LOCATION_UP;
   oButtonData[1].idCommand = IDC_LOCATION_REFRESH;

   // Add buttons
   SendMessage(hCtrl, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), NULL);
   SendMessage(hCtrl, TB_ADDBUTTONS, iToolBarButtonCount, (LPARAM)&oButtonData);
   // Resize toolbar
   SendMessage(hCtrl, TB_AUTOSIZE, NULL, NULL);

   /// Reposition Toolbar to the right of the 'Browse Folder' combo
   // Get rectangles of the combo and listview
   utilGetDlgItemRect(pDialogData->hDialog, IDC_FILE_LIST, &rcListViewRect);
   utilGetDlgItemRect(pDialogData->hDialog, IDC_LOCATION_COMBO, &rcComboRect);
   // Calculate rectangle of RHS of the combo and above the ListView, then centre it vertically
   SetRect(&rcToolbarRect, rcComboRect.right + 16, 0, rcListViewRect.right, rcListViewRect.top);
   iButtonHeight = HIWORD(SendMessage(hCtrl, TB_GETBUTTONSIZE, NULL, NULL));
   rcToolbarRect.top = (rcListViewRect.top - iButtonHeight) / 2;

   // Set position and return
   utilSetDlgItemRect(pDialogData->hDialog, IDC_FILE_TOOLBAR, &rcToolbarRect, TRUE);
   return TRUE;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                      MESSAGE HANDLERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onFileDialog_Command
// Description     : Process WM_COMMAND notifications
//
// FILE_DIALOG_DATA*  pDialogData   : [in] File dialog data
// CONST UINT         iControlID    : [in] ID of the control sending the notification
// CONST UINT         iNotification : [in] Notification code
// HWND               hCtrl         : [in] Window handle of the control sending the message (if any)
// 
// Return type : TRUE if processed, FALSE if not
//
BOOL  onFileDialog_Command(FILE_DIALOG_DATA*  pDialogData, CONST UINT  iControlID, CONST UINT  iNotification, HWND  hCtrl)
{
   BOOL   bResult;
 
   // [TRACK]
   TRACK_FUNCTION();

   // Prepare
   bResult = TRUE;

   // Examine control ID
   switch (iControlID)
   {
   /// [OK] Attempt to accept the currently selected file and return IDOK
   case IDOK:
      onFileDialogOK_Click(pDialogData);
      break;

   /// [CANCEL] Close the dialog box and return IDCANCEL
   case IDCANCEL:
      // End dialog
      EndDialog(pDialogData->hDialog, IDCANCEL);
      break;

   /// [OPTIONS] Display options dialog
   case IDC_FILE_OPTIONS:
      onFileDialogOptions_Click(pDialogData);
      break;

   /// [FILENAME EDIT]
   case IDC_FILENAME_EDIT:
      // Examine notification
      switch (iNotification)
      {
      case EN_UPDATE:      onFileDialogFilename_Changed(pDialogData);      break;
      case EN_SETFOCUS:    onFileDialogFilename_GetFocus(pDialogData);     break;
      case EN_KILLFOCUS:   onFileDialogFilename_LostFocus(pDialogData);    break;
      default:             bResult = FALSE;
      }
      break;

   /// [LOCATION CHANGE] Display new folder
   case IDC_LOCATION_COMBO:
      switch (iNotification)
      {
      case CBN_SELCHANGE:  onFileDialogNamespace_Changed(pDialogData);     break;
      default:             bResult = FALSE;  break;
      }
      break;

   /// [FILTER CHANGE] Display new file types
   case IDC_FILETYPE_COMBO:
      switch (iNotification)
      {
      case CBN_SELCHANGE:  onFileDialogFilter_Changed(pDialogData);        break;
      default:             bResult = FALSE;  break;
      }
      break;

   /// [FOLDER JUMP] Change folder to new location
   case IDC_LOCATION_JUMP1:
   case IDC_LOCATION_JUMP2:
   case IDC_LOCATION_JUMP3:
   case IDC_LOCATION_JUMP4:
   case IDC_LOCATION_JUMP5:
   case IDC_LOCATION_JUMP6:
      onFileDialogJumpBar_Click(pDialogData, iControlID - IDC_LOCATION_JUMP1);
      break;

   /// [TRAVERSE UP] Change folder
   case IDC_LOCATION_UP:
      onFileDialogTraverseUp_Click(pDialogData);
      break;

   /// [REFRESH] Update file list
   case IDC_LOCATION_REFRESH:
      onFileDialogRefresh_Click(pDialogData);
      break;

   // [UNHANDLED]
   default:
      bResult = FALSE;
      break;
   }

   // Cleanup and return result
   END_TRACKING();
   return bResult;
}


/// Function name  : onFileDialog_CustomDrawJumpItem
// Description     : Draw one of the jump bar item buttons
// 
// FILE_DIALOG_DATA*  pDialogData   : [in] FileDialog window data
// JUMPBAR_ITEM*      pJumpItem     : [in] JumpBar item data
// NMCUSTOMDRAW*      pDrawData     : [in] WM_NOTIFY CustomDraw data
// 
// Return Value   : TRUE if processed, FALSE otherwise
// 
BOOL  onFileDialog_CustomDrawJumpItem(FILE_DIALOG_DATA*  pDialogData, JUMPBAR_ITEM*  pJumpItem, NMCUSTOMDRAW*  pDrawData)
{
   DC_STATE*  pPrevState;         // Previous DeviceContext state
   HTHEME     hTheme;             // Toolbar theme handle
   RECT       rcClientRect,       // Button's client rectangle
              rcTextRect;         // Text drawing rectangle
   POINT      ptIconPosition;     // Icon draw position
   SIZE       siClientSize,       // Size of the button's client rectangle
              siTextSize;         // Size of the text's drawing rectangle
   HBRUSH     hBackgroundBrush;   // Brush used for drawing the background of an empty button
   UINT       iButtonState;       // Current state of the button as a windows Theme State enum
   BOOL       bResult;            // Operation Result

   // Prepare
   TRACK_FUNCTION();
   bResult = FALSE;
   
   /// [PRE-PAINT] Perform all drawing manually
   if (pDrawData->dwDrawStage == CDDS_PREPAINT)
   {
      // Prepare
      hTheme           = OpenThemeData(pJumpItem->hCtrl, TEXT("TOOLBAR"));
      hBackgroundBrush = CreateSolidBrush(clJumpItemBackground);
      pPrevState       = utilCreateDeviceContextState(pDrawData->hdc);
      iButtonState     = identifyCustomButtonState(pJumpItem->hCtrl, pDrawData->uItemState);
      bResult          = TRUE;
      
      // Get client rectangle
      GetClientRect(pJumpItem->hCtrl, &rcClientRect);
      utilConvertRectangleToSize(&rcClientRect, &siClientSize);

      /// [ICON POSITION] Position in the centre of the button
      ptIconPosition.x = rcClientRect.left + (siClientSize.cx - iJumpBarIconSize) / 2;
      ptIconPosition.y = rcClientRect.top  + (siClientSize.cy - iJumpBarIconSize) / 2;
      // Shift upwards by half the height of the text
      GetTextExtentPoint32(pDrawData->hdc, pJumpItem->szText, lstrlen(pJumpItem->szText), &siTextSize);
      ptIconPosition.y -= (siTextSize.cy / 2);

      // [CONTENT] Get content rectangle
      if (hTheme)
         GetThemeBackgroundContentRect(hTheme, pPrevState->hDC, TP_BUTTON, NULL, &rcClientRect, &rcTextRect);
      else
         SetRect(&rcTextRect, rcClientRect.left + 2, rcClientRect.top + 2, rcClientRect.right - 2, rcClientRect.bottom - 2);

      /// [TEXT POSITION] Position beneath the icon
      rcTextRect.top    = ptIconPosition.y + iJumpBarIconSize;
      rcTextRect.bottom = rcTextRect.top + siTextSize.cy;
      InflateRect(&rcTextRect, -2, 0);

      /// [BACKGROUND] -- Draw the default state manually and use visual styles for the hot and pressed states
      switch (iButtonState)
      {
      // [DEFAULT] Draw a sunken box
      case TS_DISABLED:
      case TS_NORMAL:
         // Draw background
         FillRect(pPrevState->hDC, &rcClientRect, hBackgroundBrush);

         // To create the illusion of a box draw 'top' and 'bottom' edges only for buttons at the top + bottom.
         switch (GetDlgCtrlID(pJumpItem->hCtrl))
         {
         case IDC_LOCATION_JUMP1:   DrawEdge(pPrevState->hDC, &rcClientRect, EDGE_SUNKEN, BF_LEFT WITH BF_RIGHT WITH BF_TOP);      break;
         case IDC_LOCATION_JUMP6:   DrawEdge(pPrevState->hDC, &rcClientRect, EDGE_SUNKEN, BF_LEFT WITH BF_RIGHT WITH BF_BOTTOM);   break;
         default:                   DrawEdge(pPrevState->hDC, &rcClientRect, EDGE_SUNKEN, BF_LEFT WITH BF_RIGHT);               break;
         }
         break;
      
      // [HOT/PRESSED] Use visual styles to do all the drawing
      default:
         if (hTheme)
            DrawThemeBackground(hTheme, pPrevState->hDC, TP_BUTTON, iButtonState, &rcClientRect, NULL);
         else 
            DrawFrameControl(pPrevState->hDC, &rcClientRect, DFC_BUTTON, DFCS_BUTTONPUSH WITH (iButtonState == TS_PRESSED ? DFCS_PUSHED : DFCS_HOT));
         break;
      }
      
      // Setup device context
      utilSetDeviceContextBackgroundMode(pPrevState, TRANSPARENT);
      utilSetDeviceContextFont(pPrevState, NULL, GetSysColor(iButtonState == TS_DISABLED ? COLOR_GRAYTEXT : COLOR_WINDOWTEXT));

      /// [ICON] Draw Icon in normal/disabled mode
      drawIcon(pJumpItem->hImageList, pJumpItem->iIconIndex, pPrevState->hDC, ptIconPosition.x, ptIconPosition.y, iButtonState != TS_DISABLED ? IS_NORMAL : IS_DISABLED);

      /// [TEXT] Draw Text in normal/disabled mode
      DrawText(pPrevState->hDC, pJumpItem->szText, lstrlen(pJumpItem->szText), &rcTextRect, DT_CENTER WITH DT_SINGLELINE WITH DT_END_ELLIPSIS);
      
      /// [MESSAGE RESULT] Inform system not to paint
      SetWindowLong(pDialogData->hDialog, DWL_MSGRESULT, CDRF_SKIPDEFAULT);

      // Cleanup
      utilDeleteDeviceContextState(pPrevState);
      DeleteObject(hBackgroundBrush);
      CloseThemeData(hTheme);
   }
 
   // Return result
   END_TRACKING();
   return bResult;
}


/// Function name  : onFileDialog_CustomDrawListView
// Description     : Draw 'Please Wait' instead of the ListView items while the ListView is updating, and draw
//                    any virtual files or folders in blue instead of black.
// 
// FILE_DIALOG_DATA*  pDialogData : [in] File dialog data
// NMLVCUSTOMDRAW*    pDrawInfo   : [in] Custom draw data
// 
// Return Type : TRUE if processed, FALSE otherwise
// 
BOOL   onFileDialog_CustomDrawListView(FILE_DIALOG_DATA*  pDialogData, NMLVCUSTOMDRAW*  pListViewData)
{
   //CONST TCHAR    *szNoDescription = TEXT("[No Description]"),       // Text of the description placeholder
   //               *szNoVersion     = TEXT("[None]"),                 // Text of the version placeholder
   //               *szPlaceholder;                                    // Pointer to either of the above
   NMCUSTOMDRAW   *pDrawData;          // Convenience pointer for the custom draw data
   //FILE_ITEM      *pFileItem;          // FileItem associated with the current ListView item
   TCHAR          *szUpdateText;       // Wait string, displayed when listview is updating
   RECT            rcClientRect;       // Rectangle of listView's client area
   BOOL            bResult;            // Operation Result;

   // Prepare
   TRACK_FUNCTION();
   pDrawData     = &pListViewData->nmcd;
   //szPlaceholder = NULL;
   bResult       = FALSE;

   // Examine draw stage
   switch (pDrawData->dwDrawStage)
   {
   /// [NORMAL MODE - ITEM PAINT]
   case CDDS_ITEMPREPAINT:
      // [NON-UPDATING] Perform custom drawing
      drawCustomListViewItem(pDialogData->hDialog, pDialogData->hListView, pListViewData);
      SetWindowLong(pDialogData->hDialog, DWL_MSGRESULT, CDRF_SKIPDEFAULT);

      //// Request per-subitem notification
      //SetWindowLong(pDialogData->hDialog, DWL_MSGRESULT, CDRF_DODEFAULT WITH CDRF_NOTIFYSUBITEMDRAW);
      bResult = TRUE;
      break;

   ///// [NORMAL MODE - SUBITEM PAINT]
   //case CDDS_ITEMPREPAINT WITH CDDS_SUBITEM:
   //   // Lookup associated FileItem 
   //   if (findFileSearchResultByIndex(pDialogData->pFileSearch, pDrawData->dwItemSpec, pFileItem))
   //   {
   //      // Examine sub-item
   //      switch (pListViewData->iSubItem)
   //      {
   //      /// [NAME, TYPE and SIZE] - Set text colour of virtual items to blue
   //      case FILE_COLUMN_NAME:
   //      case FILE_COLUMN_TYPE:
   //      case FILE_COLUMN_SIZE:
   //         // Set text colour to blue or black
   //         pListViewData->clrText = (pFileItem->iAttributes INCLUDES FIF_VIRTUAL ? clVirtualFile : GetSysColor(COLOR_WINDOWTEXT));
   //         // Tell ListView to draw item
   //         SetWindowLong(pDialogData->hDialog, DWL_MSGRESULT, CDRF_DODEFAULT);
   //         break;
   //      
   //      /// [DESCRIPTION, VERSION] - Draw a grey placeholder if none is present
   //      case FILE_COLUMN_DESCRIPTION:
   //      case FILE_COLUMN_VERSION:
   //         // [FOLDER] Don't add a placeholder to folder items
   //         if (pFileItem->iAttributes INCLUDES FIF_FOLDER)
   //            SetWindowLong(pDialogData->hDialog, DWL_MSGRESULT, CDRF_DODEFAULT);
   //         else 
   //         {
   //            /// Determine whether to draw a placeholder
   //            switch (pListViewData->iSubItem)
   //            {
   //            case FILE_COLUMN_DESCRIPTION: szPlaceholder = (lstrlen(pFileItem->szDescription) ? NULL : szNoDescription);  break;
   //            case FILE_COLUMN_VERSION:     szPlaceholder = (pFileItem->iFileVersion           ? NULL : szNoVersion);      break;
   //            }

   //            // [CHECK] Are we drawing a placeholder?
   //            if (szPlaceholder)
   //            {
   //               // Calculate text rectangle
   //               ListView_GetSubItemRect(pDialogData->hListView, pDrawData->dwItemSpec, pListViewData->iSubItem, LVIR_LABEL, &pListViewData->rcText);
   //               InflateRect(&pListViewData->rcText, GetSystemMetrics(SM_CXEDGE) * -3, NULL);

   //               // Set text colour to dark blue or grey
   //               SetTextColor(pDrawData->hdc, (pFileItem->iAttributes INCLUDES FIF_VIRTUAL ? clVirtualPlaceholder : GetSysColor(COLOR_GRAYTEXT)));

   //               // [TEXT] Draw placeholder text 
   //               DrawText(pDrawData->hdc, szPlaceholder, lstrlen(szPlaceholder), &pListViewData->rcText, DT_LEFT WITH DT_END_ELLIPSIS);
   //            }

   //            // Inform ListView either to paint normally or not at all
   //            SetWindowLong(pDialogData->hDialog, DWL_MSGRESULT, szPlaceholder ? CDRF_SKIPDEFAULT : CDRF_DODEFAULT);
   //         }
   //      }
   //   }
   //   bResult = TRUE;
   //   break;

   /// [UPDATING MODE]
   case CDDS_PREPAINT:
      // [CHECK] Ensure we're in the updating state
      if (pDialogData->bIsUpdating)
      {
         // Prepare         
         GetClientRect(pDialogData->hListView, &rcClientRect);

         // [CHECK] Is a search operation running yet?
         if (pDialogData->pSearchOperation)
         {
            // [SUCCESS] Generate progress string
            szUpdateText = utilLoadString(getResourceInstance(), getOperationProgressStageID(pDialogData->pSearchOperation->pProgress), 96);
            utilStringCchCatf(szUpdateText, 96, TEXT(" %u%%"), calculateOperationProgressStagePercentage(pDialogData->pSearchOperation->pProgress) / 100);
         }
         else
            // [FAILED] Load update string
            szUpdateText = utilLoadString(getResourceInstance(), IDS_FILE_DIALOG_UPDATING, 96);

         /// [BACKGROUND] Draw the client region in white except for the sort column rectangle
         FillRect(pDrawData->hdc, &rcClientRect, GetSysColorBrush(COLOR_WINDOW));
         /// [TEXT] Draw the update status text
         DrawText(pDrawData->hdc, szUpdateText, lstrlen(szUpdateText), &rcClientRect, DT_CENTER WITH DT_VCENTER WITH DT_SINGLELINE);

         // Request no system painting 
         SetWindowLong(pDialogData->hDialog, DWL_MSGRESULT, CDRF_SKIPDEFAULT);

         // Cleanup and return
         utilDeleteString(szUpdateText);
      }
      else
         // [NON-UPDATING] Request per-item custom draw.
         SetWindowLong(pDialogData->hDialog, DWL_MSGRESULT, CDRF_NOTIFYITEMDRAW);
      
      bResult = TRUE;
      break;
   }

   // Cleanup and return
   END_TRACKING();
   return bResult;

   /// [OLD CODE: COLOURED SORT COLUMN] This is most of the old column highlighting code - it would be nice to bring it back at some point
   //rgnClientRegion = CreateRectRgn(rcClientRect.left, rcClientRect.top, rcClientRect.right, rcClientRect.bottom);
   //rgnSortRegion   = CreateRectRgn(rcSortRect.left, rcSortRect.top, rcSortRect.right, rcSortRect.bottom);
   //hHighlightBrush = CreateSolidBrush(clSortColumnBackground);
   //FillRgn(pDrawData->hdc, rgnSortRegion, GetSysColorBrush(COLOR_INFOBK));
   //CombineRgn(rgnClientRegion, rgnClientRegion, rgnSortRegion, RGN_DIFF);
   //FillRgn(pDrawData->hdc, rgnClientRegion, GetSysColorBrush(COLOR_WINDOW));
   //DeleteObject(hHighlightBrush);
}



/// Function name  : onFileDialog_Destroy
// Description     : Preserves state and destroys dialog data
// 
// FILE_DIALOG_DATA*  pDialogData : [in] File dialog data
// 
VOID  onFileDialog_Destroy(FILE_DIALOG_DATA*  pDialogData)
{
   // [TRACK]
   TRACK_FUNCTION();

   /// Save current folder
   setAppPreferencesLastFolder(pDialogData->szFolder, pDialogData->eFilter);

   /// Save dialog size
   utilGetWindowSize(pDialogData->hDialog, getAppPreferencesWindowSize(AW_BROWSER));

   // Remove and destroy locations data
   deleteFileDialogNamespace(pDialogData);

   // Destroy toolbar
   utilDeleteWindow(pDialogData->hToolbar);

   /*/// [MODAL WINDOW] Remove window from stack
   popModalWindowStack();*/

   // Zero but do not destroy the system ImageList handle
   pDialogData->hImageList = NULL;
   
   // Cleanup
   END_TRACKING();
}


/// Function name  : onFileDialog_GetMinMaxSize
// Description     : Set the minimum dialog size
// 
// FILE_DIALOG_DATA*  pDialogData   : [in]     FileDialog window data
// MINMAXINFO*        pWindowSize   : [in/out] Minimum window size data
// 
VOID  onFileDialog_GetMinMaxSize(FILE_DIALOG_DATA*  pDialogData, MINMAXINFO*  pWindowSize)
{
   POINT  ptMinimumSize = { 418, 264 };      // Minimum dialog size, in dialog units

   // Set minimum size in pixels by converting from DLUs
   utilConvertDialogUnitsToPixels(pDialogData->hDialog, &ptMinimumSize, &pWindowSize->ptMinTrackSize);

   /// HACK: Specify minimum size manually because DLU conversion doesn't give the results I was expecting
   pWindowSize->ptMinTrackSize.x = iMinimumDialogSize.cx;
   pWindowSize->ptMinTrackSize.y = iMinimumDialogSize.cy;
}


/// Function name  : onFileDialog_Notify
// Description     : WM_NOTIFY message processing
// 
// FILE_DIALOG_DATA*  pDialogData  : [in] File dialog data
// NMHDR*             pMessage     : [in] WM_NOTIFY message data
// 
// Return Value : TRUE if handled, otherwise FALSE
//
BOOL  onFileDialog_Notify(FILE_DIALOG_DATA*  pDialogData, NMHDR*  pMessage)
{
   NMITEMACTIVATE    *pClickInfo;         // NM_DBLCLICK data
   NMLISTVIEW        *pColumnInfo;        // ListView column data
   JUMPBAR_ITEM      *pJumpItem;          // JumpBar item convenience pointer
   BOOL               bResult;            // Operation result
   
   // [TRACK]
   TRACK_FUNCTION();

   // Prepare
   bResult = FALSE;

   // [CHECK]
   if (pMessage->hwndFrom == pDialogData->hSuggestionList)
      bResult = onFileDialog_SuggestionNotify(pDialogData, pMessage);

   // Examine notification
   else switch (pMessage->code)
   {
   /// [FILE DOUBLE CLICK] - Get item details
   case NM_DBLCLK:
      // Prepare
      pClickInfo = (NMITEMACTIVATE*)pMessage;

      // [CHECK] User clicked an item
      if (pClickInfo->iItem != -1)
      {
         onFileDialogList_DoubleClick(pDialogData, pClickInfo->iItem);
         bResult = TRUE;
      }
      break;

   /// [FILE LIST SELECTION CHANGED]
   case LVN_ITEMCHANGED:
   case LVN_ODSTATECHANGED:
      // Prepare
      pClickInfo = (NMITEMACTIVATE*)pMessage;

      // [CHECK] Has selection changed?
      if (utilIncludes(pClickInfo->uNewState, LVIS_SELECTED) OR utilIncludes(pClickInfo->uOldState, LVIS_SELECTED))
         // [SUCCESS] Enable/Disable filename
         onFileDialogList_SelectionChanged(pDialogData);

      // Refresh item
      onCustomListViewNotify(pDialogData->hDialog, TRUE, IDC_FILE_LIST, pMessage);
      bResult = TRUE;
      break;

   /// [FILE LIST REQUEST DATA]
   case LVN_GETDISPINFO:
      onFileDialog_RequestData(pDialogData, (NMLVDISPINFO*)pMessage);
      bResult = TRUE;
      break;

   /// [SORT COLUMN CLICKED]
   case LVN_COLUMNCLICK:
      // Prepare
      pColumnInfo = (NMLISTVIEW*)pMessage;
      // Identify column index
      onFileDialogList_ColumnClick(pDialogData, pColumnInfo->iSubItem);
      bResult = TRUE;
      break;

   /// [CUSTOM DRAW]
   case NM_CUSTOMDRAW:
      // [FILE LIST]
      if (pMessage->idFrom == IDC_FILE_LIST)
         bResult = onFileDialog_CustomDrawListView(pDialogData, (NMLVCUSTOMDRAW*)pMessage);
      
      // [JUMP ITEMS]
      else if (pMessage->idFrom >= IDC_LOCATION_JUMP1 AND pMessage->idFrom <= IDC_LOCATION_JUMP6)
      {
         // Lookup associated jump bar item
         pJumpItem = pDialogData->pJumpBarItems[pMessage->idFrom - IDC_LOCATION_JUMP1];
         bResult = onFileDialog_CustomDrawJumpItem(pDialogData, pJumpItem, (NMCUSTOMDRAW*)pMessage);
      }
      break;

   /// [HOTTRACK] Handle ListView hover notifications
   default:
      bResult = onCustomListViewNotify(pDialogData->hDialog, TRUE, IDC_FILE_LIST, pMessage);
      break;
   }

   // Cleanup and return result
   END_TRACKING();
   return bResult;
}


/// Function name  : onFileDialog_OperationComplete
// Description     : Notifies the file dialog that a file search has completed
// 
// FILE_DIALOG_DATA*  pDialogData    : [in] Dialog data
// SEARCH_OPERATION*  pOperationData : [in] FileSearch operation data
// 
VOID  onFileDialog_OperationComplete(FILE_DIALOG_DATA*  pDialogData, SEARCH_OPERATION*  pOperationData)
{
   // Extract FileSearch
   pDialogData->pFileSearch = pOperationData->pFileSearch;

   // Remove progress timer
   KillTimer(pDialogData->hDialog, FILE_SEARCH_TIMER_ID);

   /// Re-create namespace
   updateFileDialogNamespace(pDialogData);

   // Remove "Please Wait..." and display new ListView contents
   setFileDialogUpdateState(pDialogData, FALSE);

   // [TUTORIAL] Display tutorial window
   setMainWindowTutorialTimer(getMainWindowData(), TW_OPEN_FILE, TRUE); 

   // Cleanup
   pDialogData->pSearchOperation = NULL;
   deleteOperationData((OPERATION_DATA*&)pOperationData);
}


/// Function name  : onFileDialog_Paint
// Description     : Draw the size box in the bottom right corner
// 
// FILE_DIALOG_DATA*    pDialogData : [in] File Dialog data
// PAINTSTRUCT*         pPaintData  : [in] Paint data
// 
VOID  onFileDialog_Paint(FILE_DIALOG_DATA*  pDialogData, PAINTSTRUCT*  pPaintData)
{
   RECT  rcClient,       // Client rect
         rcSizeBox;      // Bounding rectangle of the size box in bottom right hand corner

   // Prepare
   GetClientRect(pDialogData->hDialog, &rcClient);

   /// [BACKGROUND]
   utilFillSysColourRect(pPaintData->hdc, &rcClient, COLOR_WINDOW);

   // Calculate sizing grip rectangle
   SetRect(&rcSizeBox, rcClient.right - GetSystemMetrics(SM_CXVSCROLL), rcClient.bottom - GetSystemMetrics(SM_CYHSCROLL), rcClient.right, rcClient.bottom);
   
   /// [SIZE GRIP] Draw the sizing grip
   //DrawFrameControl(pPaintData->hdc, &rcSizeBox, DFC_SCROLL, DFCS_SCROLLSIZEGRIP);

   /// [HACK] Redraw statics and buttons
   InvalidateDlgItem(pDialogData->hDialog, IDC_FILENAME_STATIC, FALSE);
   InvalidateDlgItem(pDialogData->hDialog, IDC_FILETYPE_STATIC, FALSE);
   InvalidateDlgItem(pDialogData->hDialog, IDOK, FALSE);
   InvalidateDlgItem(pDialogData->hDialog, IDCANCEL, FALSE);
}


/// Function name  : onFileDialog_Resize
// Description     : Resize the file dialog and it's child controls
// 
// FILE_DIALOG_DATA*  pDialogData : [in] File dialog data
// CONST UINT         iWidth      : [in] New width, in pixels
// CONST UINT         iHeight     : [in] New height, in pixels
// 
VOID  onFileDialog_Resize(FILE_DIALOG_DATA*  pDialogData, CONST UINT  iWidth, CONST UINT  iHeight)
{
   RECT  rcListView,       // ListView rectangle, in dialog client co-ordinates
         rcJumpBarItem,    // First JumpBar item rectangle, in dialog client co-ordinates
         rcClient,         // Dialog client rectangle
         rcControl,        // Working rectangle used for resizing individual controls
         rcLowerControls;  // Bounding rectangle of the 'lower controls area', in dialog client co-ordinates
   UINT  iBorderSize,      // Width/Height of the spacing between all the controls
         iEditHeight;      // Height of an edit control
   HDWP  hWindowPositions; // Multiple window positions

   // Prepare
   GetClientRect(pDialogData->hDialog, &rcClient);
   utilGetDlgItemRect(pDialogData->hDialog, IDC_FILE_LIST, &rcListView);
   utilGetDlgItemRect(pDialogData->hDialog, IDC_LOCATION_JUMP1, &rcJumpBarItem);

   // Calculate border size
   iBorderSize = GetSystemMetrics(SM_CXEDGE) * 3;
   // Calculate edit height
   utilGetDlgItemRect(pDialogData->hDialog, IDC_FILENAME_EDIT, &rcControl);
   iEditHeight = rcControl.bottom - rcControl.top;

   /// [CALCULATE] - Calculate the rectangles for positioning the controls
   // Calculate size of the 'lower controls area' from the edge of the dialog and the RHS of the jump bar
   rcLowerControls.left   = rcJumpBarItem.right + iBorderSize;
   rcLowerControls.right  = rcClient.right  - iBorderSize;
   rcLowerControls.bottom = rcClient.bottom - iBorderSize;
   rcLowerControls.top    = rcLowerControls.bottom - iEditHeight - iBorderSize - iEditHeight;

   // Adjust the right and bottom edges of the ListView rectangle
   rcListView.right  = rcClient.right - iBorderSize;
   rcListView.bottom = rcLowerControls.top - iBorderSize;

   /// [POSITION] - Use the calculated rectangles to position the various controls
   hWindowPositions = BeginDeferWindowPos(7);

   // [LISTVIEW] - Position the control to using the adjusted listview rectangle
   utilDeferDlgItemRect(hWindowPositions, pDialogData->hDialog, IDC_FILE_LIST, &rcListView, TRUE, TRUE);

   // [OK BUTTON] - Position in the top right of the 'lower controls area'
   utilSetRectangle(&rcControl, rcLowerControls.right - 75, rcLowerControls.top, 75, iEditHeight);
   utilDeferDlgItemRect(hWindowPositions, pDialogData->hDialog, IDOK, &rcControl, TRUE, TRUE);

   // [CANCEL BUTTON] - Position in the bottom right of the 'lower controls area'
   utilSetRectangle(&rcControl, rcLowerControls.right - 75, rcLowerControls.bottom - iEditHeight, 75, iEditHeight);
   utilDeferDlgItemRect(hWindowPositions, pDialogData->hDialog, IDCANCEL, &rcControl, TRUE, TRUE);

   // [FILENAME LABEL] - Position in the top left of the 'lower controls area'
   utilSetRectangle(&rcControl, rcLowerControls.left, rcLowerControls.top, 60, iEditHeight);
   utilDeferDlgItemRect(hWindowPositions, pDialogData->hDialog, IDC_FILENAME_STATIC, &rcControl, TRUE, TRUE);

   // [FILETYPE LABEL] - Position in the bottom left of the 'lower controls area'
   utilSetRectangle(&rcControl, rcLowerControls.left, rcLowerControls.bottom - iEditHeight, 60, iEditHeight);
   utilDeferDlgItemRect(hWindowPositions, pDialogData->hDialog, IDC_FILETYPE_STATIC, &rcControl, TRUE, TRUE);

   // [FILENAME COMBO] - Position in the centre of the 'top row' of the 'lower controls area'
   SetRect(&rcControl, rcLowerControls.left  + 60 + iBorderSize, rcLowerControls.top, 
                       rcLowerControls.right - 75 - iBorderSize, rcLowerControls.top + iEditHeight);
   utilDeferDlgItemRect(hWindowPositions, pDialogData->hDialog, IDC_FILENAME_EDIT, &rcControl, TRUE, TRUE);

   // [FILETYPE COMBO] - Position in the centre of the 'bottom row' of the 'lower controls area'
   SetRect(&rcControl, rcLowerControls.left  + 60 + iBorderSize, rcLowerControls.bottom - iEditHeight, 
                       rcLowerControls.right - 75 - iBorderSize, rcLowerControls.bottom);
   utilDeferDlgItemRect(hWindowPositions, pDialogData->hDialog, IDC_FILETYPE_COMBO, &rcControl, TRUE, TRUE);
   SendDlgItemMessage(pDialogData->hDialog, IDC_FILETYPE_COMBO, CB_SETMINVISIBLE, 6, NULL);

   // Position windows
   EndDeferWindowPos(hWindowPositions);
   
   /// [LISTVIEW COLUMNS]
   onFileDialog_ResizeColumns(pDialogData, &rcListView);
}


/// Function name  : onFileDialog_ResizeColumns
// Description     : Resize the file dialog ListView columns
// 
// FILE_DIALOG_DATA*  pDialogData : [in] File dialog data
// CONST UINT         iWidth      : [in] New width, in pixels
// CONST UINT         iHeight     : [in] New height, in pixels
// 
VOID  onFileDialog_ResizeColumns(FILE_DIALOG_DATA*  pDialogData, CONST RECT*  pListViewRect)
{
   UINT    iColumnWidths[FILE_COLUMNS_COUNT] = { 230, 180, 50, 100, 60 },   // ListView Column widths: Name, Description, Version, Type, Size  
           iFixedColumnsWidth,     // Total width of fixed columns: version, type, size
           iListViewWidth;         // Width of ListView minus the scrollbar

   // Calculate widths
   iListViewWidth     = (pListViewRect->right - pListViewRect->left) - GetSystemMetrics(SM_CXHSCROLL) - 10;    // [HACK] -10 to create border to reduce appearance of horz bar while resizing
   iFixedColumnsWidth = iListViewWidth - (iColumnWidths[2] + iColumnWidths[3] + iColumnWidths[4]);

   // Resize all columns
   for (UINT  iColumn = 0; iColumn < FILE_COLUMNS_COUNT; iColumn++)
      ListView_SetColumnWidth(GetControl(pDialogData->hDialog,IDC_FILE_LIST), iColumn, iColumn <= 1 ? max(50, iFixedColumnsWidth / 2) : iColumnWidths[iColumn]);
}


/// Function name  : onFileDialog_RequestData
// Description     : Virtual listview requesting data
//
// FILE_DIALOG_DATA* pDialogData : [in] FileDialog window data
// NMLVDISPINFO*     pItemInfo   : [in/out] Object containing item to get info for
// 
VOID  onFileDialog_RequestData(FILE_DIALOG_DATA*  pDialogData, NMLVDISPINFO*  pItemInfo)
{
   LVITEM&     oOutput = pItemInfo->item;   // Conveience pointer for output data
   FILE_ITEM*  pFileItem;                   // Associated FileItem for the specified item
   
   // [CHECK] Abort is dialog is updating
   if (pDialogData->bIsUpdating)
      return;

   // Prepare
   TRACK_FUNCTION();

   // Lookup FileItem
   if (findFileSearchResultByIndex(pDialogData->pFileSearch, oOutput.iItem, pFileItem))
   {
      // Return appropriate data
      switch (oOutput.iSubItem)
      {
      case FILE_COLUMN_NAME:
      case FILE_COLUMN_TYPE:
      case FILE_COLUMN_SIZE:
         // Re-examine
         switch (oOutput.iSubItem)
         {
         /// [FILE NAME] Supply text and icon
         case FILE_COLUMN_NAME:
            if (oOutput.mask INCLUDES LVIF_TEXT)
               StringCchCopy(oOutput.pszText, oOutput.cchTextMax, pFileItem->szDisplayName);

            oOutput.iImage = pFileItem->iIconIndex;
            break;

         /// [FILE TYPE] Supply text
         case FILE_COLUMN_TYPE:
            if (oOutput.mask INCLUDES LVIF_TEXT)
               StringCchCopy(oOutput.pszText, oOutput.cchTextMax, pFileItem->szDisplayType);
            break;

         /// [FILE SIZE] Supply text for files
         case FILE_COLUMN_SIZE:
            if (utilIncludes(oOutput.mask, LVIF_TEXT) AND utilExcludes(pFileItem->iAttributes, FIF_FOLDER))
               // [FILES] Format into kilobytes
               StrFormatByteSize(pFileItem->iFileSize, oOutput.pszText, oOutput.cchTextMax);
            break;
         }

         // [VIRTUAL] Display in blue
         if (pFileItem->iAttributes INCLUDES FIF_VIRTUAL)
         {
            oOutput.lParam = (LPARAM)clVirtualFile; // (pFileItem->iAttributes INCLUDES FIF_VIRTUAL ? clVirtualFile : GetSysColor(COLOR_WINDOWTEXT));
            oOutput.mask  |= LVIF_COLOUR_TEXT;
         }
         break;
     
      /// [DESCRIPTION]
      case FILE_COLUMN_DESCRIPTION:
         // [CHECK] Display nothing for folders
         if (utilExcludes(pFileItem->iAttributes, FIF_FOLDER))
         {
            // [DESCRIPTION] Display description, if any, otherwise placeholder
            if (lstrlen(pFileItem->szDescription))
            {
               if (oOutput.mask INCLUDES LVIF_TEXT)
                  StringCchCopy(oOutput.pszText, oOutput.cchTextMax, pFileItem->szDescription);
            }
            else
            {
               // [MISSING] Display placeholder
               if (oOutput.mask INCLUDES LVIF_TEXT)
                  StringCchCopy(oOutput.pszText, oOutput.cchTextMax, TEXT("[No Description]"));

               // [COLOUR] 
               oOutput.lParam = (LPARAM)(pFileItem->iAttributes INCLUDES FIF_VIRTUAL ? clVirtualPlaceholder : GetSysColor(COLOR_GRAYTEXT));
               oOutput.mask  |= LVIF_COLOUR_TEXT;
            }
         }
         break;

      /// [SCRIPT VERSION]
      case FILE_COLUMN_VERSION:
         // [CHECK] Display nothing for folders
         if (utilExcludes(pFileItem->iAttributes, FIF_FOLDER))
         {
            // [VERSION] Display version
            if (pFileItem->iFileVersion)
            {
               if (oOutput.mask INCLUDES LVIF_TEXT)
                  utilConvertIntegerToString(oOutput.pszText, pFileItem->iFileVersion);
            }
            else
            {
               // [MISSING] Display placeholder
               if (oOutput.mask INCLUDES LVIF_TEXT)
                  StringCchCopy(oOutput.pszText, oOutput.cchTextMax, TEXT("[None]"));

               // [COLOUR] 
               oOutput.lParam = (LPARAM)(pFileItem->iAttributes INCLUDES FIF_VIRTUAL ? clVirtualPlaceholder : GetSysColor(COLOR_GRAYTEXT));
               oOutput.mask  |= LVIF_COLOUR_TEXT;
            }
         }
         break;
      }
   }
   else if (oOutput.mask INCLUDES LVIF_TEXT)
      // [ERROR] Identify missing item
      StringCchPrintf(oOutput.pszText, oOutput.cchTextMax, TEXT("ERROR: Unknown Item %u of %u"), oOutput.iItem, getFileSearchResultCount(pDialogData->pFileSearch));
   else
      // [ERROR] Provide fixed string
      oOutput.pszText = TEXT("ERROR: Unknown item");

   // Cleanup
   END_TRACKING();
}


/// Function name  : onFileDialog_Timer
// Description     : Refreshes the ListView during updates to report progress
// 
// FILE_DIALOG_DATA*  pDialogData   : [in] File dialog data
// 
VOID  onFileDialog_Timer(FILE_DIALOG_DATA*  pDialogData)
{
   // Force refresh of disabled ListView
   InvalidateRect(pDialogData->hListView, NULL, TRUE);
}


/// Function name  : dlgprocFileDialog
// Description     : File dialog dialog procedure
//
INT_PTR CALLBACK   dlgprocFileDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   FILE_DIALOG_DATA*  pDialogData;   // FileDialog window data
   PAINTSTRUCT        oPaintData;    // System painting data
   ERROR_STACK*       pError;        // Exception error
   BOOL               bResult;       // Operation result
   TRACK_FUNCTION();

   // Prepare
   bResult = TRUE;

   /// [GUARD BLOCK]
   __try
   {
      // Get window data
      pDialogData = getFileDialogData(hDialog);

      switch (iMessage)
      {
      /// [INIT DIALOG] - Store input data and initialise dialog
      case WM_INITDIALOG:
         // Manually extract window data
         pDialogData = (FILE_DIALOG_DATA*)lParam;
         // Init dialog
         initFileDialog(pDialogData, hDialog);
         break;

      /// [DESTRUCTION] Preserve state and destroy dialog
      case WM_DESTROY:
         onFileDialog_Destroy(pDialogData);
         break;

      // [OPERATION COMPLETE]
      case UN_OPERATION_COMPLETE:
         onFileDialog_OperationComplete(pDialogData, (SEARCH_OPERATION*)lParam);
         break;
         
      /// [COMMAND PROCESSING]
      case WM_COMMAND:
         bResult = onFileDialog_Command(pDialogData, LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
         break;

      /// [NOTIFICATION]
      case WM_NOTIFY:
         bResult = onFileDialog_Notify(pDialogData, (NMHDR*)lParam);
         break;

      /// [HELP]
      case WM_HELP:
         bResult = displayHelp(TEXT("Browser_Catalogues"));
         break;

         /// [PROGRESS TIMER]
      case WM_TIMER:
         onFileDialog_Timer(pDialogData);
         break;

      /// [SIZING] -- Resize the child controls along with the dialog
      case WM_SIZE:
         onFileDialog_Resize(pDialogData, LOWORD(lParam), HIWORD(lParam));
         break;

      /// [SIZING] - Prevent the dialog being shunk beyond a certain minimum size
      case WM_GETMINMAXINFO:
         onFileDialog_GetMinMaxSize(pDialogData, (MINMAXINFO*)lParam);
         break;

      /// [PAINT] -- Draw the size box
      case WM_PAINT:
         BeginPaint(hDialog, &oPaintData);
         onFileDialog_Paint(pDialogData, &oPaintData);
         EndPaint(hDialog, &oPaintData);
         break;

      // [OWNER DRAW] -- Draw ComboBoxes
      case WM_DRAWITEM:    bResult = onWindow_DrawItem((DRAWITEMSTRUCT*)lParam);           break;
      case WM_DELETEITEM:  bResult = onWindow_DeleteItem((DELETEITEMSTRUCT*)lParam);       break;
      case WM_MEASUREITEM: bResult = onWindow_MeasureComboBox((MEASUREITEMSTRUCT*)lParam, ITS_SMALL, ITS_SMALL);     break;

      // [VISUAL STYLES] -- Use a white background
      case WM_CTLCOLORDLG:
      case WM_CTLCOLORSTATIC:
         bResult = (INT_PTR)GetSysColorBrush(COLOR_WINDOW);
         break;

      // [UNHANDLED] Return FALSE
      default:
         bResult = FALSE;
         break;
      }
   }
   /// [EXCEPTION HANDLER]
   __except (generateExceptionError(GetExceptionInformation(), pError))
   {
      // [ERROR] "An unidentified and unexpected critical error has occurred in the open file window"
      enhanceError(pError, ERROR_ID(IDS_EXCEPTION_FILE_DIALOG));
      displayException(pError);
   }

   // Return result
   END_TRACKING();
   return bResult;
}



