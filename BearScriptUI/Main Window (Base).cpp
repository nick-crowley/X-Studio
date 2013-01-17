//
// Main Window (Base).cpp : Manages the low-level operations of the main window
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                     CONSTANTS / GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// Toolbar button count
CONST UINT  iToolBarButtonCount  = 24;

// Menu Sub-Menu indicies
#define  MAINWINDOW_FILE_MENU_INDEX     0
#define  MAINWINDOW_EDIT_MENU_INDEX     1
#define  MAINWINDOW_VIEW_MENU_INDEX     2
#define  MAINWINDOW_TOOLS_MENU_INDEX    3
#define  MAINWINDOW_WINDOW_MENU_INDEX   5
#define  MAINWINDOW_HELP_MENU_INDEX     6

#define  MAINWINDOW_RECENT_SUBMENU_INDEX  10

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION  /  DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createMainWindow
// Description     : Creates the main BearScript window and it's child windows
// 
// Return type : Window handle to the new window
//
HWND   createMainWindow()
{
   MAIN_WINDOW_DATA*   pWindowData;   // Main window data
   RECT                rcWindowRect;
   SIZE                siWindowSize;
   HWND                hWnd;              // Window handle
   
   // [VERBOSE]
   VERBOSE_LIB_COMMAND();
   TRACK_FUNCTION();

   // Create main window data
   pWindowData = createMainWindowData();

   // [CHECK] Was window previous maximised?
   if (getAppPreferences()->bMainWindowMaximised)
      // [MAXIMISED] Size to desktop
      GetWindowRect(GetDesktopWindow(), &rcWindowRect);
   else
      // [NORMAL] Size from previous
      rcWindowRect = *getAppPreferencesWindowRect(AW_MAIN);

   // Calculate size
   utilConvertRectangleToSize(&rcWindowRect, &siWindowSize);

   /// Create main window. Pass window data
   hWnd = CreateWindowEx(NULL, szMainWindowClass, getAppName(), WS_OVERLAPPEDWINDOW WITH WS_CLIPCHILDREN, rcWindowRect.left, rcWindowRect.top, siWindowSize.cx, siWindowSize.cy, NULL, NULL, getAppInstance(), (VOID*)pWindowData);
   ERROR_CHECK("creating main window", hWnd);

   // [CHECK] Ensure window was created successfully
   if (!hWnd)
      // [FAILED] Cleanup
      deleteMainWindowData(pWindowData);
   else
   {
      // [WINDOWS VISTA] Change the window's message filter to allow drag'n'drop
      if (getAppWindowsVersion() >= WINDOWS_VISTA)
         utilEnableDragNDrop(hWnd);

      /*/// Add main window to the modal window stack
      pushModalWindowStack(hWnd);*/

      // [TUTORIAL] Delay display 'Game Folder' tutorial
      setMainWindowTutorialTimer(pWindowData, TW_GAME_FOLDER, TRUE);
   }

   /// Return handle if successful, otherwise NULL
   END_TRACKING();
   return hWnd;
}


/// Function name  : createMainWindowControls
// Description     : Creates the child windows of the main window
//
// MAIN_WINDOW_DATA*  pWindowData : [in] Main window data
// 
// Return type : TRUE if all windows created, otherwise FALSE
//
BOOL     createMainWindowControls(MAIN_WINDOW_DATA*  pWindowData)
{
   RECT    rcClientRect;         // Main window client rectangle which takes account of statusbars and the toolbar
   INT     iStatusBarWidths[2];
   
   // [TRACK]
   TRACK_FUNCTION();

   /// [CUSTOM MENU] Create CustomMenu and attach to the window
   pWindowData->pCustomMenu = createCustomMenu(szMainWindowClass, FALSE, NULL);
   SetMenu(pWindowData->hMainWnd, pWindowData->pCustomMenu->hMenuBar);

   /// [TOOLBAR]
   pWindowData->hToolBar = createMainWindowToolBar(pWindowData);

   // Get client area
   getMainWindowClientRect(pWindowData->hMainWnd, &rcClientRect);
   iStatusBarWidths[0] = (rcClientRect.right - 200);
   iStatusBarWidths[1] = -1;
   
   /// [STATUSBAR] Create StatusBar and display welcome message
   pWindowData->hStatusBar = CreateStatusWindow(WS_CHILD WITH WS_VISIBLE WITH CCS_BOTTOM WITH SBARS_SIZEGRIP, TEXT(""), pWindowData->hMainWnd, IDC_STATUS_BAR);
   ERROR_CHECK("creating status bar", pWindowData->hStatusBar);

   // Create parts
   SendMessage(pWindowData->hStatusBar, SB_SETPARTS, 2, (LPARAM)iStatusBarWidths);
   setMainWindowStatusBarTextf(0, IDS_MAIN_STATUS_WELCOME);
   setMainWindowStatusBarTextf(1, IDS_MAIN_STATUS_NO_DATA_LOADED);

   /// [OPERATION POOL]
   pWindowData->hOperationPoolCtrl = createOperationPoolCtrl(pWindowData->pOperationPool, pWindowData->hStatusBar);

   // Calculate updated main window client area
   getMainWindowClientRect(pWindowData->hMainWnd, &rcClientRect);

   /// [DOCUMENTS CONTROL] Create the documents control
   pWindowData->hDocumentsTab = createDocumentsControl(pWindowData->hMainWnd, &rcClientRect, IDC_DOCUMENTS_TAB);

   /// [WORKSPACE] Create the workspace control
   pWindowData->hWorkspace = createWorkspace(pWindowData->hMainWnd, &rcClientRect, pWindowData->hDocumentsTab, COLOR_BTNFACE);

   // Return TRUE if all windows created successfully
   END_TRACKING();
   return pWindowData->pCustomMenu->hMenuBar AND pWindowData->hToolBar AND pWindowData->hStatusBar AND
          pWindowData->hOperationPoolCtrl AND pWindowData->hWorkspace AND pWindowData->hDocumentsTab;
}


/// Function name  : createMainWindowToolBar
// Description     : Creates the toolbar for the main window
//
// HWND  hParentWnd : [in] Main window handle
// 
// Return type : Toolbar window handle
//
HWND    createMainWindowToolBar(MAIN_WINDOW_DATA*  pWindowData)
{
   TBADDBITMAP   oToolbarData;      // Source bitmap data
   TBBUTTON*     pButtonData;       // Button data
   HWND          hCtrl;             // Output window handle

   // [TRACK]
   TRACK_FUNCTION();

   // Prepare
   pButtonData = utilCreateObjectArray(TBBUTTON, iToolBarButtonCount);

   // Define number of toolbar buttons
   pWindowData->iToolBarButtonCount    = iToolBarButtonCount;
   pWindowData->iToolBarSeparatorCount = 4;

   /// Create ToolBar
   hCtrl = CreateWindowEx(NULL, TOOLBARCLASSNAME, NULL, TBSTYLE_FLAT WITH TBSTYLE_TRANSPARENT WITH TBSTYLE_TOOLTIPS WITH WS_CHILD WITH WS_VISIBLE, 0,0, 0,0, 
                          pWindowData->hMainWnd, (HMENU)IDC_MAIN_TOOLBAR, getResourceInstance(), NULL);
   ERROR_CHECK("creating main toolbar", hCtrl);

   // [CHECK] Ensure Toolbar was created successfully
   if (hCtrl)
   {
      // Define source bitmap
      oToolbarData.hInst = getResourceInstance();
      oToolbarData.nID   = IDB_MAIN_TOOLBAR;

      /// Define number and dimensions of individual images
      SendMessage(hCtrl, TB_SETBITMAPSIZE, NULL, MAKE_LONG(32,32));
      SendMessage(hCtrl, TB_ADDBITMAP, (iToolBarButtonCount - pWindowData->iToolBarSeparatorCount), (LPARAM)&oToolbarData);

      /// Define buttons
      for (UINT iButton = 0, iBitmap = 0; iButton < iToolBarButtonCount; iButton++)
      {
         // Prepare
         utilZeroObject(&pButtonData[iButton], TBBUTTON);

         // Define button ID
         pButtonData[iButton].fsState   = TBSTATE_ENABLED;
         pButtonData[iButton].idCommand = identifyMainWindowCommandByIndex(iButton);
         
         // [BUTTON]
         if (pButtonData[iButton].idCommand)
         {
            pButtonData[iButton].iBitmap = iBitmap++;
            pButtonData[iButton].fsStyle = BTNS_BUTTON;
         }
         // [SEPARATOR]
         else
            pButtonData[iButton].fsStyle = BTNS_SEP;
      }

      /// Pass buttons to toolbar
      SendMessage(hCtrl, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), NULL);
      SendMessage(hCtrl, TB_ADDBUTTONS, iToolBarButtonCount, (LPARAM)pButtonData);
      // Resize toolbar
      SendMessage(hCtrl, TB_AUTOSIZE, NULL, NULL);
   }

   // Cleanup and return toolbar handle (or NULL)
   utilDeleteObject(pButtonData);
   END_TRACKING();
   return hCtrl;
}


/// Function name  : createMainWindowData
// Description     : Create the window data for the main application window
// 
// Return Value   : New main window data, you are responsible for destroying it
// 
MAIN_WINDOW_DATA*  createMainWindowData()
{
   MAIN_WINDOW_DATA*  pWindowData;

   // [TRACK]
   TRACK_FUNCTION();

   // Create new object
   pWindowData = utilCreateEmptyObject(MAIN_WINDOW_DATA);

   // Create output dialog data
   pWindowData->pOutputDlgData = createOutputDialogData();

   // Create Operation Pool
   pWindowData->pOperationPool = createOperationPool();

   // Create properties data
   pWindowData->pPropertiesSheetData = createPropertiesDialogData();

   // Create window stack
   //pWindowData->pWindowStack = createModalWindowStack(NULL);

   // Load main window accelerators
   pWindowData->hAccelerators = LoadAccelerators(getResourceInstance(), szMainWindowClass);

   // Return new window data
   END_TRACKING();
   return pWindowData;
}

/// Function name  : deleteMainWindowData
// Description     : Delete the main window data
// 
// MAIN_WINDOW_DATA*  pWindowData   : [in] Window handle of the main window
// 
VOID  deleteMainWindowData(MAIN_WINDOW_DATA*  pWindowData)
{
   // Delete output dialog data
   deleteOutputDialogData(pWindowData->pOutputDlgData);

   /// Delete documents data  REMOVED: Now handled by onMainWindowDestroy(.)
   //deleteDocumentsControlData(pWindowData->pDocumentsData);

   // Destroy accelerator table
   DestroyAcceleratorTable(pWindowData->hAccelerators);

   // Delete calling object
   utilDeleteObject(pWindowData);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                           HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : displayException
// Description     : Displays and then destroys an exception error
// 
// ERROR_STACK*  &pException : [in] Exception error
// 
VOID  displayException(ERROR_STACK*  &pException)
{
   // Generate output message
   //generateOutputTextFromError(pException);

   // [CHECK] Does app window exist yet?
   if (getAppWindow())
   {
      /// [SUCCESS] Display in OutputDialog and MessageDialog
      printErrorToOutputDialog(pException);
      //displayErrorMessageDialog(NULL, pException, MAKEINTRESOURCE(IDS_TITLE_EXCEPTION), MDF_OK WITH MDF_ERROR);
   }
   else
      /// [FAILED] Print to console
      consolePrintError(pException);

   // Cleanup
   deleteErrorStack(pException);
}


/// Function name  : getFocusedDocument
// Description     : Retrieves the Project or Document with the keyboard focus
// 
// MAIN_WINDOW_DATA*  pWindowData : [in] Window data
// 
// Return Value   : ActiveProject if focused, otherwise ActiveDocument. If neither exist then NULL.
// 
DOCUMENT*  getFocusedDocument(MAIN_WINDOW_DATA*  pWindowData)
{
   DOCUMENT  *pDocument,
             *pProject,
             *pActive;

   // Prepare
   pActive   = NULL;
   pProject  = getActiveProject();
   pDocument = getActiveDocument();

   /// [BOTH EXIST] Return ActiveProject if project window has keyboard focus, otherwise the ActiveDocument
   if (pProject AND pWindowData->hProjectDlg AND pDocument)
      pActive = (GetFocus() == getProjectDialogData(pWindowData->hProjectDlg)->hTreeView ? pProject : pDocument);

   /// [DOCUMENT or PROJECT] Return whichever exists
   else if (pDocument OR (pProject AND pWindowData->hProjectDlg))
      pActive = utilEither(pDocument, pProject);

   // Return active document if any, otherwise NULL
   return pActive;
}


/// Function name  : getMainWindowData
// Description     : Retrieve data associated with the main window
// 
// HWND  hMainWnd : [in] Window handle of main BearScript window
// 
// Return type : MAIN_WINDOW_DATA*  
//
MAIN_WINDOW_DATA*  getMainWindowData(HWND  hMainWnd)
{
   return (hMainWnd ? (MAIN_WINDOW_DATA*)GetWindowLong(hMainWnd, 0) : NULL);
}


/// Function name  : getMainWindowClientRect
// Description     : Get the client area of the main BearScript window
//
// HWND   hMainWnd : [in]  Window handle of main BearScript window
// RECT*  pOutput  : [out] Client rectangle
// 
VOID     getMainWindowClientRect(HWND  hMainWnd, RECT*  pOutput)
{
   MAIN_WINDOW_DATA*  pWindowData;
   RECT               rcControl;

   // Get window data and set true client rectangle
   pWindowData = getMainWindowData(hMainWnd);
   GetClientRect(pWindowData->hMainWnd, pOutput);

   // Subtract Toolbar height
   GetClientRect(pWindowData->hToolBar, &rcControl);
   pOutput->top += rcControl.bottom + 2;   // TODO: What is causing this gap of two pixels?

   // Subtract StatusBar height
   GetClientRect(pWindowData->hStatusBar, &rcControl);
   pOutput->bottom -= (rcControl.bottom - rcControl.top);
}


/// Function name  : identifyMainWindowCommandByIndex
// Description     : Identify the command ID of a given button index in the main window toolbar
// 
// CONST UINT  iIndex : [in] Zero based Button index
// 
// Return Value   : Command ID
// 
UINT  identifyMainWindowCommandByIndex(CONST UINT  iIndex)
{
   static UINT   iButtonCommands[iToolBarButtonCount] = 
   {  
      // New, Open, Save, <Separator>  
      // Cut, Copy, Paste, <Separator>
      IDM_FILE_NEW,            IDM_FILE_OPEN,               IDM_FILE_SAVE,                 NULL, 
      IDM_EDIT_CUT,            IDM_EDIT_COPY,               IDM_EDIT_PASTE,                NULL, 

      // Find, <Separator>
      IDM_EDIT_FIND,           NULL, 

      // GameObjects, ScriptObjects, CommandList, OutputDialog, DocumentProperties, <Separator>
      // GameStrings, MediaBrowser, MissionHierarchy, ConversationBrowser, <Separator>
      IDM_VIEW_COMMAND_LIST,   IDM_VIEW_GAME_OBJECTS_LIST,  IDM_VIEW_SCRIPT_OBJECTS_LIST,  IDM_VIEW_COMPILER_OUTPUT,        IDM_VIEW_PROJECT_EXPLORER,  IDM_VIEW_DOCUMENT_PROPERTIES,   NULL, 
      IDM_TOOLS_GAME_STRINGS,  IDM_TOOLS_MEDIA_BROWSER,     IDM_TOOLS_MISSION_HIERARCHY,   IDM_TOOLS_CONVERSATION_BROWSER,  NULL, 

      // Help, Forums
      IDM_HELP_HELP,           IDM_HELP_FORUMS   
   }; 

   // Return
   return iButtonCommands[iIndex];
}


/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : setMainWindowState
// Description     : Changes the application state. Also updates the status bar and program icon to match
// 
// CONST APPLICATION_STATE  eState : [in] New application state
// 
VOID  setMainWindowState(CONST APPLICATION_STATE  eState)
{
   /// Update application state
   setAppState(eState);

   // Examine state
   switch (eState)
   {
   /// [GAME DATA] Display version in the status bar and change the program icon
   case AS_GAME_DATA_LOADED:
      // [STATUS BAR] "Loaded: %s"
      setMainWindowStatusBarTextf(1, IDS_MAIN_STATUS_GAME_DATA_LOADED, identifyGameVersionString(getAppPreferences()->eGameVersion));
      // Change window icon to match game version
      //REM: SetClassLong(getAppWindow(), GCL_HICON, (LONG)getGameVersionIconHandle(getAppPreferences()->eGameVersion));
      break;

   /// [NO GAME DATA] Display fixed string in the status bar and change the program icon
   case AS_NO_GAME_DATA:
   case AS_FIRST_RUN:
      // [STATUS BAR] "No Game Data Loaded"
      setMainWindowStatusBarTextf(1, IDS_MAIN_STATUS_NO_DATA_LOADED, identifyGameVersionString(getAppPreferences()->eGameVersion));
      // Change back to program icon
      //REM: SetClassLong(getAppWindow(), GCL_HICON, (LONG)LoadIcon(getAppInstance(), szMainWindowClass));
      break;      
   }

   // Update toolbar
   updateMainWindowToolBar(getMainWindowData());

   /// [EVENT] Inform SearchDialog
   sendAppMessage(AW_SEARCH, UN_APP_STATE_CHANGED, eState, NULL);
}

/// Function name  : setMainWindowStatusBarTextf
// Description     : Display a formatted string resource in the main window status bar
// 
// CONST UINT  iPaneID     : [in] Zero for left pane, one for right pane
// CONST UINT  iMessageID  : [in] Resource ID of the formattting string, or NULL to erase the current text
// ...         ...         : [in] Formatting arguments
// 
VOID  setMainWindowStatusBarTextf(CONST UINT  iPaneID, CONST UINT  iMessageID, ...)
{
   va_list   pArguments;      // Variable argument list
   TCHAR    *szFormat,        // Formatting string
            *szOutput;        // Formatted text for the new item

   // Determine whether we're displaying or removing a string
   if (iMessageID)
   {
      // Prepare
      pArguments = utilGetFirstVariableArgument(&iMessageID);
      szOutput   = utilCreateEmptyString(256);
      
      /// Load and format string
      szFormat = utilLoadString(getResourceInstance(), iMessageID, 256);
      StringCchVPrintf(szOutput, 256, szFormat, pArguments);

      /// Display string
      SendMessage(getMainWindowData()->hStatusBar, SB_SETTEXT, iPaneID, (LPARAM)szOutput);

      // Cleanup
      utilDeleteStrings(szFormat, szOutput);
   }
   else
      // Erase current text
      SendMessage(getMainWindowData()->hStatusBar, SB_SETTEXT, iPaneID, (LPARAM)TEXT(""));
}


/// Function name  : updateMainWindowToolBar
// Description     : Check or uncheck the matching toolbar buttons for the currently visible tool windows
// 
// MAIN_WINDOW_DATA*  pWindowData   : [in] Window data for BearScript main window
// 
VOID  updateMainWindowToolBar(MAIN_WINDOW_DATA*  pWindowData)
{
   UINT  iCommandState,    // State of the command currently being processed
         iCommandID;       // ID of the command currently being processed

   /// Iterate through each button command
   for (UINT iCurrentButton = 0; iCurrentButton < pWindowData->iToolBarButtonCount; iCurrentButton++)
   {
      /// [COMMAND EXISTS] Update button state depending on current program state
      if (iCommandID = identifyMainWindowCommandByIndex(iCurrentButton))
      {
         // Lookup current state
         iCommandState = identifyMainWindowCommandStateByID(pWindowData, iCommandID);

         // Set state and check
         SendMessage(pWindowData->hToolBar, TB_ENABLEBUTTON, iCommandID, (iCommandState INCLUDES MF_DISABLED ? FALSE : TRUE));
         SendMessage(pWindowData->hToolBar, TB_CHECKBUTTON,  iCommandID, (iCommandState INCLUDES MF_CHECKED ? TRUE : FALSE));
      }
   }
}


/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        MESSAGE HANDLERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onMainWindowCreate
// Description     : Create the debugging console
//
// HWND                 hMainWnd      : [in] Window handle of main window
// CONST CREATESTRUCT*  pCreationData : [in] System window creation data
// 
VOID     onMainWindowCreate(HWND  hMainWnd, CONST CREATESTRUCT*  pCreationData)
{
   MAIN_WINDOW_DATA*  pWindowData;

   // [TRACK]
   TRACK_FUNCTION();

   // Extract window data
   pWindowData = (MAIN_WINDOW_DATA*)pCreationData->lpCreateParams;
   pWindowData->hMainWnd = hMainWnd;

   // Store global window handle
   SetWindowLong(hMainWnd, 0, (LONG)pWindowData);   
   setAppWindow(hMainWnd);

   /// Create console window
   attachConsole(hMainWnd);

   /// Create child controls
   createMainWindowControls(pWindowData);

   /// [STATE] Set application state to 'NO GAME DATA' or 'FIRST RUN'
   setMainWindowState(getAppState());

   // Enable drag'n'drop
   DragAcceptFiles(hMainWnd, TRUE);

   /// [SEARCH] Display the search dialog and update it, if previously visible
   if (getAppPreferences()->bSearchDialogVisible)
      displaySearchDialog(pWindowData, getAppPreferences()->eSearchDialogTab);
   
   /// [OUTPUT] Display the output dialog, if previously visible
   if (getAppPreferences()->bOutputDialogVisible)
      displayOutputDialog(pWindowData, TRUE);

   /// [PROJECT] Display the project dialog
   if (getAppPreferences()->bProjectDialogVisible)
      displayProjectDialog(pWindowData, TRUE);

   // [TRACK]
   END_TRACKING();
}


/// Function name  : onMainWindowDestroy
// Description     : Destroy child windows, window data and the console
// 
// MAIN_WINDOW_DATA*  &pWindowData : [in] Main window data
// 
VOID     onMainWindowDestroy(MAIN_WINDOW_DATA*  &pWindowData)
{
   // [TRACK]
   TRACK_FUNCTION();

   /// Destroy Properties dialog (if present)
   if (pWindowData->hPropertiesSheet)
      destroyPropertiesDialog(pWindowData);
   // Delete Properties dialog data
   deletePropertiesDialogData(pWindowData->pPropertiesSheetData);

   /// Destroy workspace
   utilDeleteWindow(pWindowData->hWorkspace);
   pWindowData->hDocumentsTab = pWindowData->hOutputDlg = pWindowData->hSearchTabDlg = NULL;

   /// Destroy both the operation pool and it's window simultaneously
   deleteOperationPool(pWindowData->pOperationPool);
   pWindowData->hOperationPoolCtrl = NULL;

   /*/// [MODAL WINDOW] Remove window from stack and destroy stack
   popModalWindowStack();
   deleteModalWindowStack(pWindowData->pWindowStack);*/

   // Destroy Status bar
   utilDeleteWindow(pWindowData->hStatusBar);

   // Destroy toolbar
   utilDeleteWindow(pWindowData->hToolBar);

   // Sever and destroy main menu
   SetMenu(pWindowData->hMainWnd, NULL);
   deleteCustomMenu(pWindowData->pCustomMenu);

   // Sever and delete window data
   SetWindowLong(pWindowData->hMainWnd, 0, NULL);
   deleteMainWindowData(pWindowData);

   // Destroy console and terminate
   deleteConsole();
   PostQuitMessage(0);

   // [TRACK]
   END_TRACKING();
}


/// Function name  : onMainWindowGetMinimumSize
// Description     : Prevent the window from being resized to anything less than the toolbar width
//                    
// MAIN_WINDOW_DATA*  pWindowData   : [in] Main window data
// MINMAXINFO*        pWindowLimits : [in] Window size limits object
// 
VOID  onMainWindowGetMinimumSize(MAIN_WINDOW_DATA*  pWindowData, MINMAXINFO*  pWindowLimits)
{
   SIZE  siToolBarSize;
   UINT  iToolBarPadding;

   // [WINDOW EXISTS] - Bound to the toolbar
   if (pWindowData)
   {
      SendMessage(pWindowData->hToolBar, TB_GETMAXSIZE, NULL, (LPARAM)&siToolBarSize);
      iToolBarPadding = SendMessage(pWindowData->hToolBar, TB_GETPADDING, NULL, NULL);

      // Prevent window becoming smaller than the toolbar
      pWindowLimits->ptMinTrackSize.x = siToolBarSize.cx + (pWindowData->iToolBarSeparatorCount * LOWORD(iToolBarPadding));
      pWindowLimits->ptMinTrackSize.y = utilCalculatePercentage(pWindowLimits->ptMinTrackSize.x, 80);
   }
   // [CREATION] - Set to 850x650
   else
   {
      pWindowLimits->ptMinTrackSize.x = 850;
      pWindowLimits->ptMinTrackSize.y = 650;
   }
}


/// Function name  : onMainWindowMenuHover
// Description     : Display the string resource associated with the specified menu item in the status bar.
// 
// MAIN_WINDOW_DATA* pWindowData : [in] Main window data
// HMENU             hMenu       : [in] Handle to the menu
// CONST UINT        iMenuID     : [in] Either ID of the menu item or the index of the sub-menu
// CONST UINT        iFlags      : [in] Indicates item properties
// 
VOID  onMainWindowMenuHover(MAIN_WINDOW_DATA* pWindowData, HMENU  hMenu, CONST UINT  iMenuID, CONST UINT  iFlags)
{
   CUSTOM_MENU_ITEM*  pItemData;      // Item data for a 'recent' menu item
   DOCUMENT*          pDocument;      // Document associated with 'Window' menu item

   // [TRACK]
   TRACK_FUNCTION();

   /// [MENU CLOSED or OPENED] Display nothing
   if ((iFlags == 0xFFFF AND hMenu == NULL) OR (iFlags INCLUDES MF_POPUP))
      setMainWindowStatusBarTextf(0, IDS_MAIN_STATUS_READY);
   
   /// [DOCUMENT MENU] Use document name in description
   else if (iMenuID >= IDM_WINDOW_FIRST_DOCUMENT AND iMenuID <= IDM_WINDOW_LAST_DOCUMENT)
   {
      // Get data for document represented by this item
      findDocumentByIndex(pWindowData->hDocumentsTab, (iMenuID - IDM_WINDOW_FIRST_DOCUMENT), pDocument);
      ASSERT(pDocument);

      // [MESSAGE] "Switch to document '%s'"
      setMainWindowStatusBarTextf(0, IDM_WINDOW_FIRST_DOCUMENT, getDocumentFileName(pDocument));
   }
   /// [RECENT MENU] Use document name in description
   else if (iMenuID >= IDM_FILE_RECENT_FIRST AND iMenuID <= IDM_FILE_RECENT_LAST)
   {
      // Get item data 
      pItemData = getCustomMenuItemData(hMenu, iMenuID, FALSE);
      ASSERT(pItemData);

      // [MESSAGE] "Open recent file '%s'"
      setMainWindowStatusBarTextf(0, IDM_FILE_RECENT_FIRST, pItemData->szText);
   }
   /// [STANDARD MENU ITEM] Display appropriate description
   else
      setMainWindowStatusBarTextf(0, iMenuID);

   // [TRACK]
   END_TRACKING();
}


/// Function name  : onMainWindowMenuInitialise
// Description     : Enables/Disables menu items appropriately. Also modifies the 'window' menu to display list of documents
//
// MAIN_WINDOW_DATA* pWindowData : [in] Data for the main BearScript window
// HMENU             hPopupMenu  : [in] The popup window menu
// CONST UINT        iIndex      : [in] Index of the menu clicked
// CONST BOOL        bSystemMenu : [in] Indicates whether 'hPopupMenu' is the system menu
// 
VOID    onMainWindowMenuInitialise(MAIN_WINDOW_DATA*  pWindowData, HMENU  hPopupMenu, CONST UINT  iIndex, CONST BOOL  bSystemMenu)
{
   CUSTOM_MENU_ITEM*  pMenuItemData;      // Custom menu data
   STORED_DOCUMENT*   pRecentFile;
   DOCUMENT*          pDocument;          // Document iterator, or Document/Project with the keyboard focus
   LIST*              pRecentFilesList;
   UINT               iCommandState,      // State of the menu item currently being processed
                      iCommandID,         // ID of the menu item currently being processed
                      iMenuItemIcon;
   BOOL               bShowProject;       // Whether to display Project or Document menu items

   // [TRACK]
   TRACK_FUNCTION();

   // Examine index
   switch (iIndex)
   {
   /// [FILE MENU] Adjust the text of Save/SaveAs
   case MAINWINDOW_FILE_MENU_INDEX:
      // [CHECK] Ensure this isn't the 'New Document' submenu popup
      if (hPopupMenu == GetSubMenu(pWindowData->pCustomMenu->hMenuBar, MAINWINDOW_FILE_MENU_INDEX))
      {
         // Prepare
         pDocument    = getFocusedDocument(pWindowData);
         bShowProject = (pDocument AND pDocument->eType == DT_PROJECT);

         pMenuItemData = getCustomMenuItemData(hPopupMenu, IDM_FILE_CLOSE, FALSE);
         utilReplaceString(pMenuItemData->szText, bShowProject ? TEXT("Close Project\tCtrl+W") : TEXT("Close Document\tCtrl+W"));

         pMenuItemData = getCustomMenuItemData(hPopupMenu, IDM_FILE_SAVE, FALSE);
         utilReplaceString(pMenuItemData->szText, bShowProject ? TEXT("Save Project\tCtrl+S") : TEXT("Save Document\tCtrl+S"));

         pMenuItemData = getCustomMenuItemData(hPopupMenu, IDM_FILE_SAVE_AS, FALSE);
         utilReplaceString(pMenuItemData->szText, bShowProject ? TEXT("Save Project As") : TEXT("Save Document As"));
      }
      break;

   /// [RECENT POPUP] Build the recent documents menu
   case MAINWINDOW_RECENT_SUBMENU_INDEX:
      // Delete existing custom menu data
      emptyCustomMenu(hPopupMenu);

      // [CHECK] Generate RecentFiles list
      if (getListItemCount(pRecentFilesList = generateRecentDocumentList(pWindowData)) == 0)
         // [EMPTY] Append placeholder
         appendCustomMenuItem(hPopupMenu, IDM_FILE_RECENT_EMPTY, 0, MF_DISABLED WITH MF_GRAYED, TEXT("No Recent Files"));

      /// [NON-EMPTY] Build RecentFiles menu
      else for (UINT  iIndex = 0; findListObjectByIndex(pRecentFilesList, iIndex, (LPARAM&)pRecentFile); iIndex++)
      {
         // Set icon based on document type
         switch (pRecentFile->eType)
         {
         case FIF_SCRIPT:   iMenuItemIcon = identifyCustomMenuIconByID(IDM_FILE_NEW_SCRIPT);    break;
         case FIF_LANGUAGE: iMenuItemIcon = identifyCustomMenuIconByID(IDM_FILE_NEW_LANGUAGE);  break;
         case FIF_MISSION:  iMenuItemIcon = identifyCustomMenuIconByID(IDM_FILE_NEW_MISSION);   break;
         case FIF_PROJECT:  iMenuItemIcon = identifyCustomMenuIconByID(IDM_FILE_NEW_PROJECT);   break;
         }

         // Append menu item
         appendCustomMenuItem(hPopupMenu, IDM_FILE_RECENT_FIRST + iIndex, iMenuItemIcon, MF_ENABLED, pRecentFile->szFullPath);
      }

      // Cleanup
      deleteList(pRecentFilesList);
      break;
   
   /// [WINDOW MENU] Build the documents menu list
   case MAINWINDOW_WINDOW_MENU_INDEX:
      // [CHECK] Ensure this isn't the 'Display Tutorials' submenu popup
      if (hPopupMenu == GetSubMenu(pWindowData->pCustomMenu->hMenuBar, MAINWINDOW_WINDOW_MENU_INDEX))
      {
         // Delete existing custom menu data
         convertCustomMenuToSystemMenu(hPopupMenu);

         /// Remove any existing document items
         while (GetMenuItemCount(hPopupMenu) > 2)
            DeleteMenu(hPopupMenu, 2, MF_BYPOSITION);

         // HACK: Create a new separator, otherwise it gets converted into an item by either create or delete custom menu
         AppendMenu(hPopupMenu, MF_SEPARATOR, NULL, NULL);

         /// Build new documents list
         for (UINT iDocument = 0; findDocumentByIndex(pWindowData->hDocumentsTab, iDocument, pDocument); iDocument++)
            // Add new item containing the document's title
            AppendMenu(hPopupMenu, MF_STRING WITH MF_ENABLED, IDM_WINDOW_FIRST_DOCUMENT + iDocument, getDocumentFileName(pDocument));
         
         // Convert back to a custom menu
         convertSystemMenuToCustomMenu(hPopupMenu, TRUE);

         /// Set the appropriate custom menu icon for each document
         for (UINT iDocument = 0; findDocumentByIndex(pWindowData->hDocumentsTab, iDocument, pDocument); iDocument++)
         {
            // Lookup item data
            pMenuItemData = getCustomMenuItemData(hPopupMenu, iDocument + 3, TRUE);    // Menu contains two previous items plus the separator

            // Set icon based on document type
            switch (pDocument->eType)
            {
            case DT_SCRIPT:   pMenuItemData->iIconIndex = identifyCustomMenuIconByID(IDM_FILE_NEW_SCRIPT);        break;
            case DT_LANGUAGE: pMenuItemData->iIconIndex = identifyCustomMenuIconByID(IDM_FILE_NEW_LANGUAGE);      break;
            case DT_MEDIA:    pMenuItemData->iIconIndex = identifyCustomMenuIconByID(IDM_TOOLS_MEDIA_BROWSER);    break;
            }
         }
      }
      break;
   }
   
   /// Set appropriate state for each item in the menu
   for (INT  iCurrentItem = 0; iCurrentItem < GetMenuItemCount(hPopupMenu); iCurrentItem++)
   {
      // Lookup Command ID
      switch (iCommandID = GetMenuItemID(hPopupMenu, iCurrentItem))
      {
      // [ITEM] Check/Disable appropriately
      default:
         // Determine appropriate state for this command
         iCommandState = identifyMainWindowCommandStateByID(pWindowData, iCommandID);

         // Set item state and check
         EnableMenuItem(hPopupMenu, iCommandID, (iCommandState INCLUDES MF_DISABLED ? MF_DISABLED WITH MF_GRAYED : MF_ENABLED));
         CheckMenuItem(hPopupMenu, iCommandID, (iCommandState INCLUDES MF_CHECKED));
         break;

      // [POP-UP or SEPARATOR] Skip
      case NULL:
      case -1:
         break;
      }
   }

   // [TRACK]
   END_TRACKING();
}


/// Function name  : onMainWindowNotify
// Description     : WM_NOTIFY handler
//
// MAIN_WINDOW_DATA*  pWindowData : [in] Main window data
// NMHDR*             pMessage    : [in] WM_NOTIFY header
// 
VOID   onMainWindowNotify(MAIN_WINDOW_DATA*  pWindowData, NMHDR*  pMessage)
{
   // [TRACK]
   TRACK_FUNCTION();

   // Examine notification
   switch (pMessage->code)
   {
   /// [NEW DOCUMENT SELECTION] Display the document representing the newly selected tab
   case TCN_SELCHANGE:
      // [CHECK] Ensure index is valid
      ASSERT(TabCtrl_GetCurSel(pWindowData->hDocumentsTab) != -1);

      /// Display desired document
      displayDocumentByIndex(pWindowData->hDocumentsTab, TabCtrl_GetCurSel(pWindowData->hDocumentsTab));
      break;
   
   /// [TOOLBAR CLICK] - HACK: Convert click data into WM_COMMAND messages manually...
   case NM_CLICK:
      onMainWindowToolbarClick(pWindowData, (NMMOUSE*)pMessage);
      break;

   /// [TOOLBAR TOOLIP] - Load appropriate tooltip
   case TTN_GETDISPINFO:
      onMainWindowToolbarTooltip(pWindowData, (NMTTDISPINFO*)pMessage);
      break;

   /// [TOOLBAR HOVER] - Display tooltip in the status bar
   case TBN_HOTITEMCHANGE:
      onMainWindowToolbarHover(pWindowData, (NMTBHOTITEM*)pMessage);
      break;
   }

   // [TRACK]
   END_TRACKING();
}



/// Function name  : onMainWindowPaint
// Description     : Paint the area next to the toolbar
// 
// MAIN_WINDOW_DATA*  pWindowData : [in] Window data
// PAINTSTRUCT*       pPaintInfo  : [in] Paint data
// 
VOID    onMainWindowPaint(MAIN_WINDOW_DATA*  pWindowData, PAINTSTRUCT  *pPaintInfo)
{
   RECT   rcClient;    // MainWindow client rectangle

   // Prepare
   GetClientRect(pWindowData->hMainWnd, &rcClient);

   /// Draw sunken edge along the top
   DrawEdge(pPaintInfo->hdc, &rcClient, BDR_SUNKENOUTER, BF_TOP WITH BF_ADJUST);
   
   /// Colour remainder of the window
   utilFillSysColourRect(pPaintInfo->hdc, &rcClient, COLOR_BTNFACE);
}


/// Function name  : onMainWindowReceiveFocus
// Description     : Switch focus to the documents tab control
//
// MAIN_WINDOW_DATA*  pWindowData : [in] Main window data
// 
VOID     onMainWindowReceiveFocus(MAIN_WINDOW_DATA*  pWindowData)
{
   DOCUMENT*  pDocument;

   pDocument = getActiveDocument();

   if (pDocument)
      SetFocus(pDocument->hWnd);
}


/// Function name  : onMainWindowResize
// Description     : Resize the documents tab control to fit the new size
// 
// MAIN_WINDOW_DATA*  pWindowData : [in] Main window data
// CONST UINT         iWidth      : [in] New width, in pixels
// CONST UINT         iHeight     : [in] New height, in pixels
// 
VOID      onMainWindowResize(MAIN_WINDOW_DATA*  pWindowData, CONST UINT  iWidth, CONST UINT  iHeight)
{
   RECT  rcClientRect;           // Client rectangle
   SIZE  siClientSize;           // Size of client rectangle
   INT   iStatusBarWidths[2];    // StatusBar pane widths

   // Get real client size
   getMainWindowClientRect(pWindowData->hMainWnd, &rcClientRect);
   utilConvertRectangleToSize(&rcClientRect, &siClientSize);

   /// [WORKSPACE] Resize workspace
   rcClientRect.left += 2;
   rcClientRect.bottom--;
   utilSetClientRect(pWindowData->hWorkspace, &rcClientRect, TRUE);

   /// Resize status bar
   SendMessage(pWindowData->hStatusBar, WM_SIZE, NULL, MAKE_LONG(iWidth, siClientSize.cy));
   
   // Resize parts
   iStatusBarWidths[0] = (rcClientRect.right - 200);
   iStatusBarWidths[1] = -1;
   SendMessage(pWindowData->hStatusBar, SB_SETPARTS, 2, (LPARAM)iStatusBarWidths); 
}


/// Function name  : onMainWindowToolbarClick
// Description     : Converts toolbar clicks into commands
// 
// MAIN_WINDOW_DATA*  pWindowData : [in] Main window data
// NMMOUSE*           pMessage    : [in] NM_CLICK header
// 
VOID  onMainWindowToolbarClick(MAIN_WINDOW_DATA*  pWindowData, NMMOUSE*  pMessage)
{
   UINT  iCommandID;

   // [TRACK]
   TRACK_FUNCTION();

   /// Extract ID of the command
   iCommandID = pMessage->dwItemSpec;

   /// Launch command
   onMainWindowCommand(pWindowData, iCommandID);

   // [TRACK]
   END_TRACKING();
}



/// Function name  : onMainWindowToolbarHover
// Description     : Displays the command description in the status bar
// 
// MAIN_WINDOW_DATA*  pWindowData : [in] Main window data
// NMTBHOTITEM *      pHeader     : [in] TBN_HOTITEMCHANGE notification data
// 
VOID  onMainWindowToolbarHover(MAIN_WINDOW_DATA*  pWindowData, NMTBHOTITEM*  pHeader)
{
   /// [MOUSE HOVER]
   if (pHeader->dwFlags INCLUDES HICF_ENTERING)
   {
      // [ENABLED] Display command description
      if ((identifyMainWindowCommandStateByID(pWindowData, pHeader->idNew) INCLUDES MF_DISABLED) == FALSE)
         setMainWindowStatusBarTextf(0, pHeader->idNew);
   }
 
   /// [MOUSE LEAVE]
   else if (pHeader->dwFlags INCLUDES HICF_LEAVING)
      // Display nothing
      setMainWindowStatusBarTextf(0, IDS_MAIN_STATUS_READY);
}


/// Function name  : onMainWindowToolbarTooltip
// Description     : Fills requested tooltip information
// 
// MAIN_WINDOW_DATA*  pWindowData : [in] 
// NMTTDISPINFO*      pHeader     : [in] 
// 
VOID  onMainWindowToolbarTooltip(MAIN_WINDOW_DATA*  pWindowData, NMTTDISPINFO*  pHeader)
{
   // [CHECK] Ensure item isn't a separator
   if (pHeader->hdr.idFrom != NULL)
      /// [SUCCESS] Retrieve Tooltip 
      onTooltipRequestText(pHeader);
}


/// Function name  : onMainWindowSystemCommand
// Description     : Show or hide the debugging console window
// 
// MAIN_WINDOW_DATA*  pWindowData : [in] Main window data
// CONST UINT         iCommandID  : [in] Menu ID of the item chosen
// 
// Return type : TRUE if processed, FALSE to pass to default window proc
//
BOOL  onMainWindowSystemCommand(MAIN_WINDOW_DATA*  pWindowData, CONST UINT  iCommandID)
{
   BOOL   bResult;

   // Prepare
   TRACK_FUNCTION();
   bResult = TRUE;

   // Examine message
   switch (iCommandID)
   {
   /// [SHOW/HIDE] Flip console visibility
   case IDM_SHOW_HIDE_CONSOLE:
      showConsole(!isConsoleVisible());
      break;

   /// [CLOSE] Close main window and submit a bug report if appropriate
   case SC_CLOSE:
      postAppClose(MWS_CLOSING);
      break;

   // [UNHANDLED] Return FALSE
   default:
      bResult = FALSE;
      break;
   }

   // Return result
   END_TRACKING();
   return bResult;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          WINDOW PROCEDURE
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : wndprocMainWindow
// Description     : Window procedure for the main window
// 
LRESULT  wndprocMainWindow(HWND  hWnd, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   MAIN_WINDOW_DATA*  pWindowData;      // Window data
   PAINTSTRUCT        oPaintInfo;       // System paint data
   ERROR_STACK*       pException;       // Exception error
   UINT               iResult;          // Operation result

   // [TRACK]
   TRACK_FUNCTION();

   // Prepare
   pWindowData = getMainWindowData(hWnd);
   iResult = 0;

   /// [GUARD BLOCK]
   __try
   {
      // Examine message
      switch (iMessage)
      {
      /// [CREATE] Create console
      case WM_CREATE:
         onMainWindowCreate(hWnd, (CREATESTRUCT*)lParam);
         break;

      /// [DESTROY] Destroy console
      case WM_DESTROY:
         onMainWindowDestroy(pWindowData);
         break;

      /// [COMMANDS]
      case WM_COMMAND:      
         // [MENU/ACCELERATOR COMMANDS]
         if (lParam == NULL)
            if (!onMainWindowCommand(pWindowData, LOWORD(wParam)))
               iResult = DefWindowProc(hWnd, iMessage, wParam, lParam);
         break;
      
      /// [SYSTEM COMMANDS] Handle Console and Close manually
      case WM_SYSCOMMAND:
         // Extract menu item ID from wParam by stripping internal system flags
         if (onMainWindowSystemCommand(pWindowData, wParam INCLUDES 0xFFF0) == FALSE)
            iResult = DefWindowProc(hWnd, iMessage, wParam, lParam);   
         break;

      /// [NOTIFICATION] 
      case WM_NOTIFY:
         onMainWindowNotify(pWindowData, (NMHDR*)lParam);
         break;

      /// [PAINT] 
      case WM_PAINT:
         BeginPaint(hWnd, &oPaintInfo);
		   onMainWindowPaint(pWindowData, &oPaintInfo);
         EndPaint(hWnd, &oPaintInfo);
         break;

      /// [ENABLED]
      case WM_ENABLE:
         // [CHECK] Ensure properties dialog is in the same state
         if (pWindowData->hPropertiesSheet AND IsWindowEnabled(pWindowData->hPropertiesSheet) != wParam)
            EnableWindow(pWindowData->hPropertiesSheet, wParam);

         // [CHECK] Ensure FindText dialog is in the same state
         if (pWindowData->hFindTextDlg AND IsWindowEnabled(pWindowData->hFindTextDlg) != wParam)
            EnableWindow(pWindowData->hFindTextDlg, wParam);
         break;

      /// [SIZE] 
      case WM_SIZE:
         onMainWindowResize(pWindowData, LOWORD(lParam), HIWORD(lParam));
         break;

      /// [MINIMUM WINDOW SIZE]
      case WM_GETMINMAXINFO:
         onMainWindowGetMinimumSize(pWindowData, (MINMAXINFO*)lParam);   
         break;

      /// [TIMER] Display tutorial windows
      case WM_TIMER:
         onMainWindow_TutorialTimer(pWindowData, (TUTORIAL_WINDOW)wParam);
         break;

      /// [FOCUS] Shift focus to active document
      case WM_SETFOCUS:
         onMainWindowReceiveFocus(pWindowData);
         break;

      /// [MENUS] 
      case WM_INITMENUPOPUP: onMainWindowMenuInitialise(pWindowData, (HMENU)wParam, LOWORD(lParam), HIWORD(lParam));  break;   // Item click -- Modify documents menu
      case WM_MENUSELECT:    onMainWindowMenuHover(pWindowData, (HMENU)lParam, LOWORD(wParam), HIWORD(wParam));       break;   // Item hover -- display description in status bar

      /// [CUSTOM MENU]
      case WM_DRAWITEM:      onWindow_DrawItem((DRAWITEMSTRUCT*)lParam);              break;
      case WM_MEASUREITEM:   onWindow_MeasureItem(hWnd, (MEASUREITEMSTRUCT*)lParam);  break;

      // [DOCUMENT UPDATED]
      case UN_DOCUMENT_UPDATED:
         onMainWindow_DocumentUpdated(pWindowData);
         break;

      // [DOCUMENT SWITCHED]
      case UN_DOCUMENT_SWITCHED:
         onMainWindow_DocumentSwitched(pWindowData, (DOCUMENT*)lParam);
         break;

      // [DOCUMENT PROPERTY UPDATED]
      case UN_PROPERTY_UPDATED:
         onMainWindow_DocumentPropertyUpdated(pWindowData, wParam);
         break;

      // [DRAG 'N' DROP]
      case WM_DROPFILES:
         onMainWindow_ShellDropFiles(pWindowData, (HDROP)wParam);
         break;

      // [PREFERENCES CHANGED]
      case UN_PREFERENCES_CHANGED:
         onMainWindow_PreferencesChanged(pWindowData);
         break;

      // [WINDOW CLOSING]
      case UM_MAIN_WINDOW_CLOSING:
         onMainWindow_Close(pWindowData, (MAIN_WINDOW_STATE)wParam);
         break;

      // [OPERATION COMPLETE] 
      case UN_OPERATION_COMPLETE:
         onMainWindow_OperationComplete(pWindowData, (OPERATION_DATA*)lParam);
         break;

      // [CODE-EDIT EXCEPTION]
      case UN_CODE_EDIT_EXCEPTION:
         displayException((ERROR_STACK*&)lParam);
         //onMainWindow_ChildException(pWindowData, (HWND)wParam, (ERROR_STACK*)lParam);
         break;

      // [UNHANDLED] 
      default:
         iResult = DefWindowProc(hWnd, iMessage, wParam, lParam);
         break;

   #ifdef _TESTING
      /// [TESTING] - Validate TestCase result
      case UM_TESTCASE_BEGIN:
         onMainWindowTestCaseBegin(pWindowData, (TEST_CASE_TYPE)wParam, lParam);
         break;
   #endif
      }
   }
   /// [EXCEPTION HANDLER]
   __except (generateExceptionError(GetExceptionInformation(), pException))
   {
      // [ERROR] "An unidentified and unexpected critical error has occurred in the application window"
      enhanceError(pException, ERROR_ID(IDS_EXCEPTION_MAIN_WINDOW));
      displayException(pException);
      // [HACK] Ensure main window is re-enabled  (Incase exception happens during threaded operation)
      EnableWindow(getAppWindow(), TRUE);
   }

   // Return result
   END_TRACKING();
   return iResult;
}

