// 
// BearScriptUI.h  -- Main header for the BearScript user interface
//

#pragma once

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                        MACROS
/// ////////////////////////////////////////////////////////////////////////////////////////

/// Macro: TESTING_ONLY
// Description: Ensures a piece of code only exists in the 'Testing' builds
//
#ifdef _TESTING
   #define TESTING_ONLY(xExpression)   xExpression
#else
   #define TESTING_ONLY(xExpression)   
#endif

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                   CONSTANTS / GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////

// Window Classes
extern CONST TCHAR*    szDocumentsCtrlClass;       
extern CONST TCHAR*    szMainWindowClass;         
extern CONST TCHAR*    szOperationPoolClass;      
extern CONST TCHAR*    szArgumentsLabelComboClass;
extern CONST TCHAR*    szFileDialogEditClass;
extern CONST TCHAR*    szArgumentsLabelEditClass; 
extern CONST TCHAR*    szSplashWindowClass;       
extern CONST TCHAR*    szScriptDocumentClass; 
extern CONST TCHAR*    szLanguageDocumentClass; 
extern CONST TCHAR*    szLanguageIcons[LANGUAGES];

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    ABOUT DIALOG
/// ////////////////////////////////////////////////////////////////////////////////////////

// Functions
BOOL  displayAboutDialog(HWND  hParent);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    AUTO-UPDATE DIALOG
/// ////////////////////////////////////////////////////////////////////////////////////////

// Functions
BOOL   displayUpdateDialog(HWND  hParent, UPDATE_OPERATION*  pOperationData);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                  BUG REPORT DIALOG
/// ////////////////////////////////////////////////////////////////////////////////////////

// Functions
BOOL     displayBugReportDialog(HWND  hParentWnd, CONST BOOL  bManualInvoke);
VOID     initBugReportDialog(HWND  hDialog, CONST BOOL  bManualInvoke);

// Message Handlers
BOOL     onBugReportDialogCommand(HWND  hDialog, CONST UINT  iControlID, CONST UINT  iNotification);
VOID     onBugReportDialogOK(HWND  hDialog);
VOID     onBugReportDialogViewReport(HWND  hDialog);

// Dialog procedure
INT_PTR  dlgprocBugReportDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);


/// ////////////////////////////////////////////////////////////////////////////////////////
///                                  CORRECTION DIALOG
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
CORRECTION_DIALOG_DATA*   createCorrectionDialogData(CONST RESULT_TYPE  eType, SUGGESTION_RESULT  xResult);
VOID                      deleteCorrectionDialogData(CORRECTION_DIALOG_DATA*  &pDialogData);

// Helpers

// Functions
VOID     displayCorrectionDialog(HWND  hParentWnd, CONST RESULT_TYPE  eType, SUGGESTION_RESULT  xResult);
VOID     initCorrectionDialog(HWND  hDialog, CORRECTION_DIALOG_DATA*  pDialogData);

// Message Handlers
BOOL     onCorrectionDialog_Command(CORRECTION_DIALOG_DATA*  pDialogData, CONST UINT  iControlID, CONST UINT  iNotification);
VOID     onCorrectionDialog_OK(CORRECTION_DIALOG_DATA*  pDialogData);

// Window Procedure
INT_PTR  dlgprocCorrectionDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                     DEBUGGING
/// ////////////////////////////////////////////////////////////////////////////////////////

// Helpers
VOID     debugDocument(const DOCUMENT*  pDocument);
VOID     debugDocumentOperationData(const DOCUMENT_OPERATION*  pOperation);
VOID     debugScriptDocument(const SCRIPT_DOCUMENT*   pDocument);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                               DEPENDENCY DIALOG
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction

// Helpers

// Functions
VOID     displayDependencyDialog(HWND  hParent, SCRIPT_OPERATION*  pOperation);
BOOL     initDependencyDialog(DEPENDENCY_DIALOG_DATA*  pDialogData, HWND  hDialog);

// Message Handlers
BOOL     onDependencyDialog_Command(DEPENDENCY_DIALOG_DATA*  pDialogData, CONST UINT  iControlID, HWND  hCtrl);
VOID     onDependencyDialog_ContextMenu(DEPENDENCY_DIALOG_DATA*  pDialogData, HWND  hListView, CONST POINT*  ptCursor);
VOID     onDependencyDialog_ColumnClick(DEPENDENCY_DIALOG_DATA*  pDialogData, NMLISTVIEW*  pHeader);
VOID     onDependencyDialog_Destroy(DEPENDENCY_DIALOG_DATA*  pDialogData);
VOID     onDependencyDialog_RequestData(DEPENDENCY_DIALOG_DATA*  pDialogData, NMLVDISPINFO*  pMessageData);

// Window Procedure
INT_PTR  dlgprocDependencyDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    DOCUMENT
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
DOCUMENT*         createDocumentByType(CONST DOCUMENT_TYPE  eDocumentType, GAME_FILE*  pGameFile);
VOID              deleteDocument(DOCUMENT*  &pDocument);
VOID              destructorDocument(LPARAM  pDocument);

// Helpers
FILE_ITEM_FLAG    calculateFileTypeFromDocumentType(CONST DOCUMENT_TYPE  eDocumentType);
CONST TCHAR*      getDocumentFileName(CONST DOCUMENT*  pDocument);
CONST TCHAR*      getDocumentPath(CONST DOCUMENT*  pDocument);
CONST TCHAR*      identifyDocumentTypeIcon(CONST DOCUMENT_TYPE  eType);
CONST TCHAR*      identifyDocumentTypeString(CONST DOCUMENT_TYPE  eType);
BOOL              isModified(CONST DOCUMENT*  pDocument);
VOID              setDocumentModifiedFlag(DOCUMENT*  pDocument, CONST BOOL  bModified);
VOID              setDocumentPath(DOCUMENT*  pDocument, CONST TCHAR*  szPath);

// Functions
CLOSURE_TYPE      closeDocument(DOCUMENT*  pDocument);
BOOL              saveDocument(DOCUMENT*  pDocument, CONST BOOL  bSaveAs, CONST BOOL  bSaveClose);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                 DOCUMENTS CONTROL
/// ////////////////////////////////////////////////////////////////////////////////////////

// Construction / Destruction
HWND             createDocumentsControl(HWND  hParentWnd, CONST RECT*  pRect, CONST UINT  iControlID);
DOCUMENTS_DATA*  createDocumentsControlData(HWND  hTabCtrl);
VOID             deleteDocumentsControlData(DOCUMENTS_DATA*  &pDocumentsData);

// Helpers
BOOL                findDocumentByIndex(HWND  hTabCtrl, CONST UINT  iIndex, DOCUMENT*  &pOutput);
BOOL                findDocumentIndexByPath(HWND  hTabCtrl, CONST TCHAR*  szFullPath, INT&  iOutput);
DOCUMENT*           getActiveDocument();
const TCHAR*        getActiveDocumentFileName();
LANGUAGE_DOCUMENT*  getActiveLanguageDocument();
HWND                getActiveScriptCodeEdit();
SCRIPT_DOCUMENT*    getActiveScriptDocument();
DOCUMENTS_DATA*     getDocumentsControlData(HWND  hCtrl);
UINT                getDocumentCount();
BOOL                isAnyDocumentModified(HWND  hTabCtrl);
BOOL                isDocumentOpen(CONST TCHAR*  szFullPath);
VOID                setActiveDocument(DOCUMENTS_DATA*  pWindowData, DOCUMENT*  pDocument);

// Functions
VOID     appendDocument(HWND  hTabCtrl, DOCUMENT*  pDocument);
BOOL     closeAllDocuments(HWND  hTabCtrl, CONST BOOL  bExcludeActiveDocument);
BOOL     closeDocumentByIndex(HWND  hTabCtrl, CONST UINT  iIndex);
VOID     displayDocument(HWND  hTabCtrl, DOCUMENT*  pDocument);
VOID     displayDocumentByIndex(HWND  hTabCtrl, CONST UINT  iIndex);
VOID     displayNextDocument(HWND  hTabCtrl);
VOID     removeDocument(DOCUMENTS_DATA*  pWindowData, DOCUMENT*  pDocument, CONST UINT  iIndex);
VOID     updateDocumentSize(DOCUMENTS_DATA*  pWindowData, DOCUMENT*  pDocument);
VOID     updateDocumentTitle(DOCUMENT*  pDocument);

// Message Handlers
BOOL     onDocumentsControl_ContextMenu(DOCUMENTS_DATA*  pWindowData, CONST POINT*  ptClick);
VOID     onDocumentsControl_Create(DOCUMENTS_DATA*  pWindowData);
VOID     onDocumentsControl_Destroy(DOCUMENTS_DATA*  &pWindowData);
VOID     onDocumentsControl_DocumentUpdated(DOCUMENTS_DATA*  pWindowData);
VOID     onDocumentsControl_DocumentPropertyUpdated(DOCUMENTS_DATA*  pWindowData, CONST UINT  iControlID);
VOID     onDocumentsControl_DocumentSwitched(DOCUMENTS_DATA*  pWindowData, DOCUMENT*  pNewDocument);
VOID     onDocumentsControl_LoadComplete(DOCUMENTS_DATA*  pWindowData, CONST DOCUMENT_TYPE  eType, DOCUMENT_OPERATION*  pOperationData);
VOID     onDocumentsControl_MiddleClick(DOCUMENTS_DATA*  pWindowData, CONST POINT  ptClick);
BOOL     onDocumentsControl_Notify(DOCUMENTS_DATA*  pWindowData, CONST UINT  iControlID, NMHDR*  pMessage);
VOID     onDocumentsControl_OperationComplete(DOCUMENTS_DATA*  pWindowData, DOCUMENT_OPERATION*  pOperationData);
VOID     onDocumentsControl_PreferencesChanged(DOCUMENTS_DATA*  pWindowData);
VOID     onDocumentsControl_RequestTooltip(DOCUMENTS_DATA*  pWindowData, NMTTDISPINFO*  pHeader);
VOID     onDocumentsControl_SaveComplete(DOCUMENTS_DATA*  pWindowData, CONST DOCUMENT_TYPE  eType, DOCUMENT_OPERATION*  pOperationData);
VOID     onDocumentsControl_ValidationComplete(DOCUMENTS_DATA*  pWindowData, CONST DOCUMENT_TYPE  eType, DOCUMENT_OPERATION*  pOperationData);

// Window procedure
LRESULT  wndprocDocumentsCtrl(HWND  hCtrl, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                 EXPORT PROJECT DIALOG
/// ////////////////////////////////////////////////////////////////////////////////////////

// Functions
BOOL     displayExportProjectDialog(MAIN_WINDOW_DATA*  pMainWindowData, PROJECT_FILE*  pProject);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                     FILE DIALOG (EVENTS)
/// ////////////////////////////////////////////////////////////////////////////////////////

// Message Handlers
VOID     onFileDialogFilter_Changed(FILE_DIALOG_DATA*  pDialogData);
VOID     onFileDialogFilename_Changed(FILE_DIALOG_DATA*  pDialogData);
VOID     onFileDialogFilename_GetFocus(FILE_DIALOG_DATA*  pDialogData);
VOID     onFileDialogFilename_LostFocus(FILE_DIALOG_DATA*  pDialogData);
VOID     onFileDialogList_ColumnClick(FILE_DIALOG_DATA*  pDialogData, CONST UINT  iColumnIndex);
VOID     onFileDialogList_DoubleClick(FILE_DIALOG_DATA*  pDialogData, CONST UINT  iItem);
VOID     onFileDialogList_SelectionChanged(FILE_DIALOG_DATA*  pDialogData);
VOID     onFileDialogJumpBar_Click(FILE_DIALOG_DATA*  pDialogData, CONST UINT  iIndex);
VOID     onFileDialogNamespace_Changed(FILE_DIALOG_DATA*  pDialogData);
VOID     onFileDialogOK_Click(FILE_DIALOG_DATA*  pDialogData);
VOID     onFileDialogOptions_Click(FILE_DIALOG_DATA*  pDialogData);
VOID     onFileDialogRefresh_Click(FILE_DIALOG_DATA*  pDialogData);
VOID     onFileDialogTraverseUp_Click(FILE_DIALOG_DATA*  pDialogData);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                     FILE DIALOG (UI)
/// ////////////////////////////////////////////////////////////////////////////////////////

// Functions
BOOL     displayFileDialog(FILE_DIALOG_DATA*  pDialogData, HWND  hParentWnd, CONST BOOL  bSystemDialog);
VOID     initFileDialog(FILE_DIALOG_DATA*  pDialogData, HWND  hDialog);
VOID     initFileDialogControls(FILE_DIALOG_DATA*  pDialogData);
BOOL     initFileDialogToolbar(FILE_DIALOG_DATA*  pDialogData);

// Message Handlers
BOOL     onFileDialog_Command(FILE_DIALOG_DATA*  pDialogData, CONST UINT  iControlID, CONST UINT  iNotification, HWND  hCtrl);
BOOL     onFileDialog_CustomDrawJumpItem(FILE_DIALOG_DATA*  pDialogData, JUMPBAR_ITEM*  pButtonData, NMCUSTOMDRAW*  pDrawData);
BOOL     onFileDialog_CustomDrawListView(FILE_DIALOG_DATA*  pDialogData, NMLVCUSTOMDRAW*  pListItemData);
VOID     onFileDialog_Destroy(FILE_DIALOG_DATA*  pDialogData);
VOID     onFileDialog_OperationComplete(FILE_DIALOG_DATA*  pDialogData, SEARCH_OPERATION*  pOperationData);
BOOL     onFileDialog_Notify(FILE_DIALOG_DATA*  pDialogData, NMHDR*  pMessage);
VOID     onFileDialog_Paint(FILE_DIALOG_DATA*  pDialogData, PAINTSTRUCT*  pPaintData);
VOID     onFileDialog_Resize(FILE_DIALOG_DATA*  pDialogData, CONST UINT  iWidth, CONST UINT  iHeight);
VOID     onFileDialog_ResizeColumns(FILE_DIALOG_DATA*  pDialogData, CONST RECT*  pListViewRect);
VOID     onFileDialog_RequestData(FILE_DIALOG_DATA*  pDialogData, NMLVDISPINFO*  pItemInfo);
VOID     onFileDialog_Timer(FILE_DIALOG_DATA*  pDialogData);

// Dialog procedure
INT_PTR  dlgprocFileDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    FILE DIALOG (BASE)
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
FILE_DIALOG_DATA*  createFileDialogData(CONST FILE_DIALOG_TYPE  eType, CONST TCHAR*  szInitialFolder, CONST TCHAR*  szInitialFileName);
JUMPBAR_ITEM*      createJumpBarItem(HWND  hCtrl, CONST TCHAR*  szPath, CONST TCHAR*  szDisplayName, HIMAGELIST  hImageList, CONST UINT  iIconIndex);
JUMPBAR_ITEM*      createJumpBarItemFromLocation(HWND  hCtrl, CONST UINT  iCLSID, CONST TCHAR*  szPath);
VOID               deleteFileDialogData(FILE_DIALOG_DATA*  pDialogData);
VOID               deleteJumpBarItem(JUMPBAR_ITEM*  &pItem);
VOID               deleteFileDialogNamespace(FILE_DIALOG_DATA*  pDialogData);

// Helpers
AVL_TREE_SORT_KEY  convertFileDialogSortColumnToSortKey(CONST UINT  iSortColumnID);
BOOL               findFileDialogOutputFileByIndex(CONST FILE_DIALOG_DATA*  pDialogData, CONST UINT  iIndex, STORED_DOCUMENT*  &pOutput);
FILE_DIALOG_DATA*  getFileDialogData(HWND  hDialog);
HICON              getGameVersionIconHandle(CONST GAME_VERSION  eVersion);
CONST TCHAR*       identifyDefaultFileExtension(CONST FILE_FILTER  eFilter);

// Functions
VOID              appendFileDialogNamespaceFolder(FILE_DIALOG_DATA*  pDialogData, CONST UINT  iCLSID, CONST TCHAR*  szPath, CONST UINT  iIndent);
VOID              appendFileDialogNamespaceTraversal(FILE_DIALOG_DATA*  pDialogData, CONST TCHAR*  szParent, UINT  iInitialIndent);
TRAVERSAL_PARENT  calculateFileDialogTraversalParent(FILE_DIALOG_DATA*  pDialogData);
FILE_ITEM_FLAG    calculateFileTypeFromFileFilter(CONST FILE_FILTER  eFilter);
HRESULT           calculateFileDialogShortcutTarget(TCHAR*  szShortcutPath);
BOOL              initFileDialogJumpBarLocations(FILE_DIALOG_DATA*  pDialogData);
VOID              performFileDialogFolderChange(FILE_DIALOG_DATA*  pDialogData, CONST TCHAR*  szNewFolder);
BOOL              performFileDialogInputValidation(FILE_DIALOG_DATA*  pDialogData, CONST TCHAR*  szFullPath);
VOID              appendFileDialogOutputFile(FILE_DIALOG_DATA*  pDialogData, CONST TCHAR*  szFullPath, CONST FILE_ITEM_FLAG  eFileType);
VOID              setFileDialogUpdateState(FILE_DIALOG_DATA*  pDialogData, CONST BOOL  bUpdating);
VOID              updateFileDialog(FILE_DIALOG_DATA*  pDialogData, CONST FILE_DIALOG_SEARCH  eCommand);
VOID              updateFileDialogNamespace(FILE_DIALOG_DATA*  pDialogData);


/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    FILE DIALOG (BASE)
/// ////////////////////////////////////////////////////////////////////////////////////////

VOID   displayFileDialogSuggestions(FILE_DIALOG_DATA*  pDialogData, CONST BOOL  bShow);
VOID   updateFileDialogSuggestionPosition(FILE_DIALOG_DATA*  pDialogData);
VOID   updateFileDialogSuggestions(FILE_DIALOG_DATA*  pDialogData);
VOID   updateFileDialogSuggestionResults(FILE_DIALOG_DATA*  pDialogData);

VOID   onFileDialog_InsertSuggestion(FILE_DIALOG_DATA*  pDialogData, CONST INT  iItemIndex);
BOOL   onFileDialog_SuggestionNotify(FILE_DIALOG_DATA*  pDialogData, NMHDR*  pMessage);
VOID   onFileDialog_RequestSuggestion(FILE_DIALOG_DATA*  pDialogData, NMLVDISPINFO*  pMessage);

VOID   treeprocBuildFileDialogSuggestionTree(AVL_TREE_NODE*  pCurrentNode, AVL_TREE_OPERATION*  pOperationData);

LRESULT  wndprocFileDialogFilenameEdit(HWND  hCtrl, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                 FILE DIALOG (OPTIONS)
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
OPTIONS_DIALOG_DATA*   createFileOptionsDialogData(CONST LOADING_OPTIONS*  pExistingOptions);
VOID                   deleteFileOptionsDialogData(OPTIONS_DIALOG_DATA*  &pDialogData);

// Functions
OPTIONS_DIALOG_DATA*   displayFileOptionsDialog(HWND  hParentWnd, CONST LOADING_OPTIONS*  pExistingOptions);
BOOL                   initFileOptionsDialog(OPTIONS_DIALOG_DATA*  pDialogData, HWND  hDialog);
VOID                   updateFileOptionsDialogRecursionDepth(OPTIONS_DIALOG_DATA*  pDialogData, CONST UINT  iDepth);

// Message Handlers
VOID  onFileOptionsDialog_AdjustDepth(OPTIONS_DIALOG_DATA*  pDialogData, CONST UINT  eScrollType, CONST UINT  iAmount);
BOOL  onFileOptionsDialog_Command(OPTIONS_DIALOG_DATA*  pDialogData, CONST UINT  iControlID, CONST UINT  iNotification, HWND  hCtrl);
VOID  onFileOptionsDialog_OK(OPTIONS_DIALOG_DATA*  pDialogData);


// Dialog procedure
INT_PTR   dlgprocFileOptionsDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                 FIND TEXT DIALOG
/// ////////////////////////////////////////////////////////////////////////////////////////

// Functions
HWND     displayFindTextDialog(HWND  hParentWnd);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                 FIRST RUN DIALOG
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
APP_LANGUAGE   displayFirstRunDialog(HWND  hParentWnd);

// Functions
BOOL      initFirstRunDialog(HWND  hDialog);

// Message Handlers
BOOL      onFirstRunDialog_Close(HWND  hDialog);

// Window Procedure
INT_PTR   dlgprocFirstRunDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                 GAME VERSION DIALOG
/// ////////////////////////////////////////////////////////////////////////////////////////

// Functions
GAME_VERSION  displayGameVersionDialog(HWND  hParent, CONST BOOL  bOnlyTCAndAP);
BOOL          initGameVersionDialog(HWND  hDialog, CONST BOOL  bOnlyTCAndAP);

// Message Handlers
BOOL          onGameVersionDialog_Command(HWND  hDialog, CONST UINT  iControl, CONST UINT  iNotification);

// Window procedure
INT_PTR       dlgprocGameVersionDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                               INSERT ARGUMENT DIALOG
/// ////////////////////////////////////////////////////////////////////////////////////////

// Functions
BOOL      displayInsertArgumentDialog(SCRIPT_FILE*  pScriptFile, HWND  hParentWnd);
BOOL      initInsertArgumentDialog(CONST SCRIPT_FILE*  pScriptFile, HWND  hDialog, HWND  hTooltip);
VOID      performParameterSyntaxComboBoxPopulation(HWND  hCtrl, CONST PARAMETER_SYNTAX  eInitialSelection);

// Message Handlers
BOOL      onInsertArgumentDialogCommand(SCRIPT_FILE*  pScriptFile, HWND  hDialog, CONST UINT  iControlID, CONST UINT  iNotification, HWND  hCtrl);

// Dialog Procedure
INT_PTR   dlgprocInsertArgumentDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                               INSERT STRING / PAGE DIALOG
/// ////////////////////////////////////////////////////////////////////////////////////////

// Functions
GAME_PAGE*    displayInsertPageDialog(LANGUAGE_DOCUMENT*  pDocument, GAME_PAGE*  pPageToEdit, HWND  hParentWnd);
GAME_STRING*  displayInsertStringDialog(LANGUAGE_DOCUMENT*  pDocument, HWND  hParentWnd);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                               INSERT VARIABLE DIALOG
/// ////////////////////////////////////////////////////////////////////////////////////////

// Functions
BOOL     displayInsertVariableDialog(PROJECT_FILE*  pProjectFile, HWND  hParentWnd);
BOOL     initInsertVariableDialog(CONST PROJECT_FILE*  pProjectFile, HWND  hDialog, HWND  hTooltip);

// Message Handlers
BOOL     onInsertVariableDialogCommand(PROJECT_FILE*  pProjectFile, HWND  hDialog, CONST UINT  iControlID, CONST UINT  iNotification, HWND  hCtrl);

// Dialog Procedure
INT_PTR  dlgprocInsertVariableDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                LANGUAGE DOCUMENT (BASE)
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
BOOL       createLanguageDialogControls(LANGUAGE_DOCUMENT*  pDocument);

// Helpers
BOOL       findLanguageDocumentGameStringByIndex(LANGUAGE_DOCUMENT*  pDocument, CONST UINT  iIndex, GAME_STRING* &pOutput);
BOOL       findLanguageDocumentGamePageByID(LANGUAGE_DOCUMENT*  pDocument, CONST UINT  iPageID, GAME_PAGE* &pOutput);
BOOL       findLanguageDocumentGamePageByIndex(LANGUAGE_DOCUMENT*  pDocument, CONST UINT  iIndex, GAME_PAGE* &pOutput);
AVL_TREE*  getLanguageDocumentGameStringTree(LANGUAGE_DOCUMENT*  pDocument);
AVL_TREE*  getLanguageDocumentGamePageTree(LANGUAGE_DOCUMENT*  pDocument);

// Functions
VOID       deleteLanguageDocumentGamePage(LANGUAGE_DOCUMENT*  pDocument, GAME_PAGE*  pPage);
VOID       deleteLanguageDocumentGameString(LANGUAGE_DOCUMENT*  pDocument, GAME_STRING*  &pString);
AVL_TREE*  generateLanguagePageStringsTree(LANGUAGE_DOCUMENT*  pDocument, CONST GAME_PAGE*  pGamePage);
UINT       identifyLanguagePageStringNextID(LANGUAGE_DOCUMENT*  pDocument);
BOOL       insertLanguageDocumentGamePage(LANGUAGE_DOCUMENT*  pDocument, GAME_PAGE*  pGamePage);
BOOL       insertLanguageDocumentGameString(LANGUAGE_DOCUMENT*  pDocument, GAME_STRING*  pGameString);
VOID       modifyLanguageDocumentGamePageID(LANGUAGE_DOCUMENT*  pDocument, GAME_PAGE*  pOldPage, GAME_PAGE*  pNewPage);
VOID       modifyLanguageDocumentGameStringID(LANGUAGE_DOCUMENT*  pDocument, GAME_STRING*  pString, const UINT  iNewID);
VOID       updateLanguageDocumentPageGroups(LANGUAGE_DOCUMENT*  pDocument);
VOID       treeprocDeleteGameStringPageID(AVL_TREE_NODE*  pNode, AVL_TREE_OPERATION*  pData);
VOID       treeprocFindFreeGameStringID(AVL_TREE_NODE*  pNode, AVL_TREE_OPERATION*  pData);
VOID       treeprocGeneratePageStrings(AVL_TREE_NODE*  pNode, AVL_TREE_OPERATION*  pOperationData);
VOID       treeprocModifyGameStringPageID(AVL_TREE_NODE*  pNode, AVL_TREE_OPERATION*  pData);
BOOL       validateLanguageButtonID(LANGUAGE_DOCUMENT*  pDocument, CONST TCHAR*  szID);
BOOL       validateLanguagePageStringID(LANGUAGE_DOCUMENT*  pDocument, const UINT  iStringID);

// Message Handlers
VOID       onLanguageDocument_Create(LANGUAGE_DOCUMENT*  pDocument, HWND  hWnd);
VOID       onLanguageDocument_ContextMenu(LANGUAGE_DOCUMENT*  pDocument, CONST POINT*  ptCursor, HWND  hCtrl);
BOOL       onLanguageDocument_Command(LANGUAGE_DOCUMENT*  pDocument, CONST UINT  iControlID, CONST UINT  iNotification, HWND  hControl);
VOID       onLanguageDocument_Destroy(LANGUAGE_DOCUMENT*  pDocument);
BOOL       onLanguageDocument_Notify(LANGUAGE_DOCUMENT*  pDocument, NMHDR*  pMessage);
BOOL       onLanguageDocument_RequestData(LANGUAGE_DOCUMENT*  pDocument, CONST UINT  iControlID, NMLVDISPINFO*  pHeader);
VOID       onLanguageDocument_Resize(LANGUAGE_DOCUMENT*  pDocument, CONST SIZE*  pNewSize);
VOID       onLanguageDocument_SaveComplete(LANGUAGE_DOCUMENT*  pDocument, DOCUMENT_OPERATION*  pOperationData);

// Window proc
LRESULT    wndprocLanguageDocument(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                LANGUAGE DOCUMENT (UI)
/// ////////////////////////////////////////////////////////////////////////////////////////

// Functions
VOID  displayLanguageDocumentGameString(LANGUAGE_DOCUMENT*  pDocument, const GAME_STRING*  pString);
VOID  treeprocResolveGameStringIndex(AVL_TREE_NODE*  pNode, AVL_TREE_OPERATION*  pData);

// Message Handlers
VOID  onLanguageDocument_DeletePage(LANGUAGE_DOCUMENT*  pDocument, GAME_PAGE*  pPage);
VOID  onLanguageDocument_DeleteString(LANGUAGE_DOCUMENT*  pDocument, GAME_STRING*  pString);
VOID  onLanguageDocument_EditPage(LANGUAGE_DOCUMENT*  pDocument, GAME_PAGE*  pOldPage, GAME_PAGE*  pNewPage);
VOID  onLanguageDocument_EditString(LANGUAGE_DOCUMENT*  pDocument, GAME_STRING*  pString);
VOID  onLanguageDocument_EditStringBegin(LANGUAGE_DOCUMENT*  pDocument, NMLVDISPINFO*  pHeader);
BOOL  onLanguageDocument_EditStringEnd(LANGUAGE_DOCUMENT*  pDocument, NMLVDISPINFO*  pHeader);
BOOL  onLanguageDocument_GetMenuItemState(LANGUAGE_DOCUMENT*  pDocument, CONST UINT  iCommandID, UINT*  piState);
VOID  onLanguageDocument_Help(LANGUAGE_DOCUMENT*  pDocument, CONST HELPINFO*  pRequest);
VOID  onLanguageDocument_InsertPage(LANGUAGE_DOCUMENT*  pDocument, GAME_PAGE*  pNewPage);
VOID  onLanguageDocument_InsertString(LANGUAGE_DOCUMENT*  pDocument, GAME_STRING*  pNewString);
VOID  onLanguageDocument_PageSelectionChanged(LANGUAGE_DOCUMENT*  pDocument, CONST INT  iItem, CONST BOOL  bSelected);
VOID  onLanguageDocument_PropertyChanged(LANGUAGE_DOCUMENT*  pDocument, CONST UINT  iControlID);
VOID  onLanguageDocument_StringSelectionChanged(LANGUAGE_DOCUMENT*  pDocument, CONST INT  iItem, CONST BOOL  bSelected);
VOID  onLanguageDocument_ViewFormattingError(LANGUAGE_DOCUMENT*  pDocument);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                     LOCALISATION
/// ////////////////////////////////////////////////////////////////////////////////////////

// Functions
const TCHAR*  findNextPackedString(const TCHAR*  szString);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                        MAIN
/// ////////////////////////////////////////////////////////////////////////////////////////

// Helpers
MAIN_WINDOW_DATA*  getMainWindowData();
HACCEL             setMainWindowAccelerators(MAIN_WINDOW_DATA*  pWindowData, const TCHAR*  szID);

// Functions
BOOL     displayApplication(INT  nCmdShow, ERROR_STACK*  &pError);
BOOL     displayLanguageDialog(ERROR_STACK*  &pError);
BOOL     displayHelp(CONST TCHAR*  szPageName);
BOOL     initApplication(ERROR_STACK*  &pError);
VOID     initApplicationLogFile();
BOOL     parseCommandLineSwitches();
BOOL     registerAppWindowClasses(ERROR_STACK*  &pError);
VOID     unregisterAppWindowClasses();
BOOL     verifyInitialGameFolderState();

// Entry Point
INT      wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, TCHAR* lpCmdLine, INT nCmdShow);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    MAIN WINDOW (BASE)
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
HWND               createMainWindow();
BOOL               createMainWindowControls(MAIN_WINDOW_DATA*  pWindowData);
HWND               createMainWindowToolBar(MAIN_WINDOW_DATA*  pWindowData);
MAIN_WINDOW_DATA*  createMainWindowData();
VOID               deleteMainWindowData(MAIN_WINDOW_DATA*  pWindowData);

// Helpers
VOID               displayError(ERROR_STACK*  &pError);
VOID               displayException(ERROR_STACK*  &pException);
DOCUMENT*          getFocusedDocument(MAIN_WINDOW_DATA*  pWindowData);
MAIN_WINDOW_DATA*  getMainWindowData(HWND  hMainWnd);
VOID               getMainWindowClientRect(HWND  hMainWnd, RECT*  pOutput);
UINT               identifyMainWindowCommandByIndex(CONST UINT  iIndex);

// Functions
VOID     setMainWindowState(CONST APPLICATION_STATE  eState);
VOID     setMainWindowStatusBarTextf(CONST UINT  iPaneID, CONST UINT  iMessageID, ...);
VOID     updateMainWindowToolBar(MAIN_WINDOW_DATA*  pWindowData);
VOID     updateMainWindowToolBar(UINT  iMessage, WPARAM  wParam, LPARAM  lParam);

// Message Handlers
VOID     onMainWindowCreate(HWND  hMainWnd, CONST CREATESTRUCT*  pCreationData);
VOID     onMainWindowDestroy(MAIN_WINDOW_DATA*  &pWindowData);
VOID     onMainWindowGetMinimumSize(MAIN_WINDOW_DATA*  pWindowData, MINMAXINFO*  pWindowLimits);
VOID     onMainWindowMenuInitialise(MAIN_WINDOW_DATA*  pWindowData, HMENU  hPopupMenu, CONST UINT  iIndex, CONST BOOL  bSystemMenu);
VOID     onMainWindowMenuHover(MAIN_WINDOW_DATA* pWindowData, HMENU  hMenu, CONST UINT  iMenuID, CONST UINT  iFlags);
VOID     onMainWindowNotify(MAIN_WINDOW_DATA*  pWindowData, NMHDR*  pMessage);
VOID     onMainWindowPaint(MAIN_WINDOW_DATA*  pWindowData, PAINTSTRUCT  *pPaintInfo);
VOID     onMainWindowReceiveFocus(MAIN_WINDOW_DATA*  pWindowData);
VOID     onMainWindowResize(MAIN_WINDOW_DATA*  pWindowData, CONST UINT  iWidth, CONST UINT  iHeight);
VOID     onMainWindowThemeChanged(MAIN_WINDOW_DATA*  pWindowData);
VOID     onMainWindowToolbarClick(MAIN_WINDOW_DATA*  pWindowData, NMMOUSE*  pMessage);
VOID     onMainWindowToolbarHover(MAIN_WINDOW_DATA*  pWindowData, NMTBHOTITEM*  pHeader);
VOID     onMainWindowToolbarTooltip(MAIN_WINDOW_DATA*  pWindowData, NMTTDISPINFO*  pHeader);
BOOL     onMainWindowSystemCommand(MAIN_WINDOW_DATA*  pWindowData, CONST UINT  iCommandID);

// Window procedure
LRESULT  wndprocMainWindow(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);


/// ////////////////////////////////////////////////////////////////////////////////////////
///                               MAIN WINDOW (UI)
/// ////////////////////////////////////////////////////////////////////////////////////////

// Helpers

// Functions
VOID   addDocumentToRecentDocumentList(MAIN_WINDOW_DATA*  pWindowData, CONST DOCUMENT*  pDocument);
BOOL   commandLoadGameData(MAIN_WINDOW_DATA*  pWindowData, CONST UINT  iTestCaseID);
BOOL   commandLoadDocument(MAIN_WINDOW_DATA*  pWindowData, CONST FILE_ITEM_FLAG  eFileType, CONST TCHAR*  szFullPath, CONST BOOL  bSetActive, CONST LOADING_OPTIONS*  pOptions);
BOOL   commandLoadScriptDependency(MAIN_WINDOW_DATA*  pWindowData, const TCHAR*  szFolder, const SCRIPT_DEPENDENCY*  pDependency, CONST BOOL  bSetActive, CONST LOADING_OPTIONS*  pOptions);
BOOL   commandSaveDocument(MAIN_WINDOW_DATA*  pWindowData, DOCUMENT*  pDocument, CONST BOOL  bCloseDocument, CONST LOADING_OPTIONS*  pOptions);
BOOL   commandSubmitReport(MAIN_WINDOW_DATA*  pWindowData, CONST TCHAR*  szCorrection);
VOID   displayOperationStatus(OPERATION_DATA*  pOperation);
BOOL   displayOutputDialog(MAIN_WINDOW_DATA*  pWindowData, CONST BOOL  bShow);
BOOL   displayProjectDialog(MAIN_WINDOW_DATA*  pWindowData, CONST BOOL  bShow);
BOOL   displaySearchDialog(MAIN_WINDOW_DATA*  pWindowData, CONST RESULT_TYPE  eDialogID);
LIST*  generateRecentDocumentList(MAIN_WINDOW_DATA*  pWindowData);
BOOL   identifyMainWindowCommandStateByID(MAIN_WINDOW_DATA*  pWindowData, CONST UINT  iCommandID);
VOID   loadDocumentSession(MAIN_WINDOW_DATA*  pWindowData);
VOID   performMainWindowReInitialisation(MAIN_WINDOW_DATA*  pWindowData, CONST UINT  iTestCaseID);
VOID   setMainWindowTutorialTimer(MAIN_WINDOW_DATA*  pWindowData, CONST TUTORIAL_WINDOW  eTutorial, CONST BOOL  bInitiate);
VOID   storeDocumentSession(MAIN_WINDOW_DATA*  pWindowData);
VOID   updateMainWindowTitle(MAIN_WINDOW_DATA*  pWindowData);

// Message Handlers
VOID   onMainWindow_ChildException(MAIN_WINDOW_DATA*  pWindowData, HWND  hSourceWnd, ERROR_STACK*  pException);
VOID   onMainWindow_Close(MAIN_WINDOW_DATA*  pWindowData, CONST MAIN_WINDOW_STATE  eState);
VOID   onMainWindow_DocumentPropertyUpdated(MAIN_WINDOW_DATA*  pWindowData, CONST UINT  iControlID);
VOID   onMainWindow_DocumentSwitched(MAIN_WINDOW_DATA*  pWindowData, DOCUMENT*  pNewDocument);
VOID   onMainWindow_DocumentUpdated(MAIN_WINDOW_DATA*  pWindowData);
VOID   onMainWindow_OperationComplete(MAIN_WINDOW_DATA*  pWindowData, OPERATION_DATA*  pOperationData);
VOID   onMainWindow_PreferencesChanged(MAIN_WINDOW_DATA*  pWindowData);
VOID   onMainWindow_ShellDropFiles(MAIN_WINDOW_DATA*  pWindowData, HDROP  hDrop);
VOID   onMainWindow_TutorialTimer(MAIN_WINDOW_DATA*  pWindowData, CONST TUTORIAL_WINDOW  eTutorial);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                               MAIN WINDOW (MENU)
/// ////////////////////////////////////////////////////////////////////////////////////////

// Functions

// Message Handlers
BOOL   onMainWindowCommand(MAIN_WINDOW_DATA*  pMainWindowData, CONST UINT  iCommandID);
VOID   onMainWindowDataGameStrings(MAIN_WINDOW_DATA*  pMainWindowData);
VOID   onMainWindowDataMediaBrowser(MAIN_WINDOW_DATA*  pMainWindowData);
VOID   onMainWindowDataMissionHierarchy(MAIN_WINDOW_DATA*  pMainWindowData);
VOID   onMainWindowDataConversationBrowser(MAIN_WINDOW_DATA*  pMainWindowData);
VOID   onMainWindowDataReload(MAIN_WINDOW_DATA*  pWindowData);
VOID   onMainWindowEditFindText(MAIN_WINDOW_DATA*  pWindowData);
VOID   onMainWindowFileClose(MAIN_WINDOW_DATA*  pMainWindowData);
VOID   onMainWindowFileCloseProject(MAIN_WINDOW_DATA*  pWindowData);
VOID   onMainWindowFileExit(MAIN_WINDOW_DATA*  pMainWindowData);
VOID   onMainWindowFileExportProject(MAIN_WINDOW_DATA*  pWindowData);
VOID   onMainWindowFileNewDocument(MAIN_WINDOW_DATA*  pMainWindowData);
VOID   onMainWindowFileNewLanguageDocument(MAIN_WINDOW_DATA*  pMainWindowData, CONST TCHAR*  szFullPath);
VOID   onMainWindowFileNewProjectDocument(MAIN_WINDOW_DATA*  pWindowData, CONST TCHAR*  szFullPath);
VOID   onMainWindowFileNewScriptDocument(MAIN_WINDOW_DATA*  pMainWindowData, CONST TCHAR*  szFullPath);
VOID   onMainWindowFileOpen(MAIN_WINDOW_DATA*  pMainWindowData, CONST BOOL  bUseSystemDialog);
VOID   onMainWindowFileOpenRecent(MAIN_WINDOW_DATA*  pWindowData, CONST UINT  iIndex);
VOID   onMainWindowFileSave(MAIN_WINDOW_DATA*  pMainWindowData);
VOID   onMainWindowFileSaveAs(MAIN_WINDOW_DATA*  pMainWindowData);
VOID   onMainWindowFileSaveAll(MAIN_WINDOW_DATA*  pMainWindowData);
VOID   onMainWindowHelpAbout(MAIN_WINDOW_DATA*  pMainWindowData);
VOID   onMainWindowHelpFile(MAIN_WINDOW_DATA*  pMainWindowData);
VOID   onMainWindowHelpForums(MAIN_WINDOW_DATA*  pMainWindowData);
VOID   onMainWindowHelpSubmitFile(MAIN_WINDOW_DATA*  pMainWindowData);
VOID   onMainWindowHelpTutorial(MAIN_WINDOW_DATA*  pWindowData, CONST UINT  iCommandID);
VOID   onMainWindowHelpUpdates(MAIN_WINDOW_DATA*  pMainWindowData);
VOID   onMainWindowViewPreferencesDialog(MAIN_WINDOW_DATA*  pMainWindowData);
VOID   onMainWindowViewOutputDialog(MAIN_WINDOW_DATA*  pMainWindowData);
VOID   onMainWindowViewProjectDialog(MAIN_WINDOW_DATA*  pMainWindowData);
VOID   onMainWindowViewPropertiesDialog(MAIN_WINDOW_DATA*  pMainWindowData);
VOID   onMainWindowViewSearchDialog(MAIN_WINDOW_DATA*  pMainWindowData, CONST RESULT_TYPE  eDialog);
BOOL   onMainWindowTestCommandDescriptions(MAIN_WINDOW_DATA*  pMainWindowData);
VOID   onMainWindowTestCurrentCode(MAIN_WINDOW_DATA*  pMainWindowData);
BOOL   onMainWindowTestGameData(MAIN_WINDOW_DATA*  pMainWindowData);
BOOL   onMainWindowTestScriptGeneration(MAIN_WINDOW_DATA*  pMainWindowData);
BOOL   onMainWindowTestScriptTranslation(MAIN_WINDOW_DATA*  pMainWindowData);
BOOL   onMainWindowTestOriginalScript(MAIN_WINDOW_DATA*  pMainWindowData);
BOOL   onMainWindowTestOutputScript(MAIN_WINDOW_DATA*  pMainWindowData);
VOID   onMainWindowTestValidateAllScripts(MAIN_WINDOW_DATA*  pMainWindowData);
VOID   onMainWindowTestValidateUserScripts(MAIN_WINDOW_DATA*  pMainWindowData);
VOID   onMainWindowWindowCloseDocuments(MAIN_WINDOW_DATA*  pMainWindowData, CONST BOOL  bExcludeActive);
VOID   onMainWindowWindowShowDocument(MAIN_WINDOW_DATA*  pMainWindowData, CONST UINT  iIndex);

#ifdef _TESTING
// (Testing) Message Handlers
VOID   onMainWindowTestCaseBegin(MAIN_WINDOW_DATA*  pMainWindowData, CONST TEST_CASE_TYPE  eType, CONST UINT  iTestCaseID);
#endif


/// ////////////////////////////////////////////////////////////////////////////////////////
///                                 MAIN WINDOW (TESTING)
/// ////////////////////////////////////////////////////////////////////////////////////////

BOOL  commandScriptValidationBatchTest(MAIN_WINDOW_DATA*  pWindowData, CONST BATCH_TEST_COMMAND  eCommand);
VOID  performScriptValidationBatchTest(MAIN_WINDOW_DATA*  pWindowData, CONST FILE_FILTER  eScriptType);


/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    MEDIA DOCUMENT
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
MEDIA_DOCUMENT*   createMediaDocument(HWND  hParentWnd);
AVL_TREE*         createMediaDocumentItemsTree();

// Helpers
MEDIA_DOCUMENT*   getMediaDocumentData(HWND  hDialog);

// Functions
VOID      displayMediaDocumentPages(MEDIA_DOCUMENT*  pDocument);
VOID      displayMediaDocumentPageItems(MEDIA_DOCUMENT*  pDocument, CONST UINT  iIndex);
BOOL      findMediaDocumentItemByIndex(MEDIA_DOCUMENT*  pDocument, CONST UINT iIndex, MEDIA_ITEM*  &pOutput);
BOOL      initMediaDocument(MEDIA_DOCUMENT*  pDocument, HWND  hDialog);
BOOL      initMediaDocumentControls(MEDIA_DOCUMENT*  pDocument);

// Message Handlers
BOOL      onMediaDocumentCommand(MEDIA_DOCUMENT*  pDocument, HWND  hDialog, CONST UINT  iControlID, CONST UINT  iNotification, HWND  hCtrl);
VOID      onMediaDocumentDestroy(MEDIA_DOCUMENT*  pDocument, HWND  hDialog);
BOOL      onMediaDocumentNotify(MEDIA_DOCUMENT*  pDocument, HWND  hDialog, NMHDR*  pMessage);
VOID      onMediaDocumentResize(MEDIA_DOCUMENT*  pDocument, HWND  hDocumentsCtrl);
VOID      onMediaDocumentRequestData(MEDIA_DOCUMENT*  pDocument, CONST UINT  iControlID, NMLVDISPINFO*  pOutput);
VOID      onMediaDocumentSelectionChange(MEDIA_DOCUMENT*  pDocument, CONST UINT  iControlID, CONST UINT  iItem, CONST BOOL  bSelected);

// Dialog procedure
INT_PTR   dlgprocMediaDocument(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    MESSAGES
/// ////////////////////////////////////////////////////////////////////////////////////////

// Helpers
HWND           getAppWindowHandle(MAIN_WINDOW_DATA*  pMainWindowData, CONST APPLICATION_WINDOW  eAppWindow);

// Functions
VOID           postAppClose(CONST MAIN_WINDOW_STATE  eState);
VOID           postAppMessage(CONST APPLICATION_WINDOW  eAppWindow, CONST UINT  iMessage, WPARAM  wParam, LPARAM  lParam);
VOID           postScriptCallSearchComplete(CONST APPLICATION_WINDOW  eAppWindow, AVL_TREE*  pCallersTree);
UINT           sendAppMessage(CONST APPLICATION_WINDOW  eAppWindow, CONST UINT  iMessage, WPARAM  wParam, LPARAM  lParam);
VOID           sendDocumentSwitched(CONST APPLICATION_WINDOW  eWindow, DOCUMENT*  pNewDocument);
VOID           sendDocumentUpdated(CONST APPLICATION_WINDOW  eWindow);
VOID           sendDocumentPropertyUpdated(CONST APPLICATION_WINDOW  eWindow, CONST UINT  iControlID);
VOID           sendOperationComplete(CONST APPLICATION_WINDOW  eWindow, OPERATION_DATA*  pOperationData);
VOID           sendDocumentOperationComplete(HWND  hDocumentWnd, DOCUMENT_OPERATION*  pOperationData);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                MESSAGE DIALOG
/// ////////////////////////////////////////////////////////////////////////////////////////

// Construction / Destructions
MESSAGE_DIALOG_DATA*  createMessageDialogData(CONST TCHAR*  szMessage, CONST TCHAR*  szTitle, CONST UINT  iAttributes, CONST ERROR_STACK*  pErrorStack, CONST ERROR_QUEUE*  pErrorQueue, va_list  pArguments);
VOID                  deleteMessageDialogData(MESSAGE_DIALOG_DATA*  &pDialogData);

// Helpers
VOID                  calculateMessageDialogToolbarPosition(MESSAGE_DIALOG_DATA*  pDialogData, HWND  hDialog, RECT*  pToolbarRect);
UINT                  convertSystemFlagsToMessageDialogFlags(CONST UINT  iSystemFlags);
MESSAGE_DIALOG_DATA*  getMessageDialogData(HWND  hDialog);

// Functions
UINT    displayErrorMessageDialog(HWND  hParentWnd, CONST ERROR_STACK*  pError, CONST TCHAR*  szTitle, CONST UINT  iButtons);
VOID    displayErrorQueueDialog(HWND  hParentWnd, CONST ERROR_QUEUE*  pErrorQueue, CONST TCHAR*  szTitle);
UINT    displayMessageDialogf(HWND  hParentWnd, CONST UINT  iMessageID, CONST UINT  iAttributes, ...);
VOID    displayMessageDialogButton(MESSAGE_DIALOG_DATA*  pDialogData, CONST UINT  iControlID, CONST UINT  iNewID, CONST TCHAR*  szText, CONST BOOL  bEnabled, CONST BOOL  bVisible);
BOOL    initMessageDialog(MESSAGE_DIALOG_DATA*  pDialogData, HWND  hDialog);
BOOL    initMessageDialogToolbar(MESSAGE_DIALOG_DATA*  pDialogData, HWND  hDialog);
VOID    updateMessageDialog(MESSAGE_DIALOG_DATA*  pDialogData);

// Message Handelrs
VOID    onMessageDialogAdjustDetailLevel(MESSAGE_DIALOG_DATA*  pDialogData, CONST BOOL  bGreater);
BOOL    onMessageDialogCommand(MESSAGE_DIALOG_DATA*  pDialogData, CONST UINT  iControlID, CONST UINT  iNotification, HWND  hCtrl);
VOID    onMessageDialogContextMenu(MESSAGE_DIALOG_DATA*  pDialogData);
VOID    onMessageDialogCopyMessage(MESSAGE_DIALOG_DATA*  pDialogData);
VOID    onMessageDialogDestroy(MESSAGE_DIALOG_DATA*  pDialogData);
VOID    onMessageDialogDisplayAttachment(MESSAGE_DIALOG_DATA*  pDialogData, CONST UINT  iMenuItemID);
BOOL    onMessageDialogDrawItem(MESSAGE_DIALOG_DATA*  pDialogData, CONST UINT  iControlID, DRAWITEMSTRUCT*  pDrawInfo);
VOID    onMessageDialogNextError(MESSAGE_DIALOG_DATA*  pDialogData);
BOOL    onMessageDialogNotify(MESSAGE_DIALOG_DATA*  pDialogData, CONST UINT  iControlID, NMHDR*  pMessage);

// Window procedures
INT_PTR dlgprocMessageDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);
INT_PTR dlgprocAttachmentDialog(HWND  hDialog, UINT  message, WPARAM  wParam, LPARAM  lParam);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                               MEDIA PLAYER DIALOG
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
HWND     createMediaPlayerDialog(HWND  hParentWnd);
HRESULT  createMediaPlayerDialogInterfaces(MEDIA_PLAYER_DATA*  pDialogData, HWND  hDialog);

// Helpers
MEDIA_PLAYER_DATA*  getMediaPlayerDialogData(HWND  hDialog);

// Functions
VOID     calculateMediaItemInReferenceTime(CONST MEDIA_ITEM*  pItem, REFERENCE_TIME*  piStart, REFERENCE_TIME*  piFinish);
BOOL     drawMediaPlayerButtonControl(MEDIA_PLAYER_DATA*  pDialogData, HWND  hDialog, CONST UINT  iControlID, NMCUSTOMDRAW*  pDrawData);
VOID     drawMediaPlayerProgressCtrl(HDC  hDC, CONST RECT*  pDrawRect, HWND  hCtrl, MEDIA_PLAYER_DATA*  pDialogData);
BOOL     initMediaPlayerDialog(MEDIA_PLAYER_DATA*  pDialogData, HWND  hDialog);
HRESULT  setMediaPlayerDialogPlaybackPositions(MEDIA_PLAYER_DATA*  pDialogData, HWND  hDialog);
VOID     setMediaPlayerDialogPlaybackState(MEDIA_PLAYER_DATA*  pDialogData, HWND  hDialog, CONST BOOL  bPlaying);
VOID     setMediaPlayerDialogVideoPosition(MEDIA_PLAYER_DATA*  pDialogData, HWND  hDialog);
BOOL     performMediaPlayerDialogSoundEffectHack(CONST TCHAR*  szFileSubPath);
BOOL     updateMediaPlayerDialogControls(MEDIA_PLAYER_DATA*  pDialogData, HWND  hDialog);
HRESULT  updateMediaPlayerDialogInterfaces(MEDIA_PLAYER_DATA*  pDialogData, HWND  hDialog, MEDIA_ITEM*  pItem);

// Message Handlers
BOOL     onMediaPlayerDialogCommand(MEDIA_PLAYER_DATA*  pDialogData, HWND  hDialog, CONST UINT  iControlID, CONST UINT  iNotification, HWND  hCtrl);
VOID     onMediaPlayerDialogDestroy(MEDIA_PLAYER_DATA*  pDialogData, HWND  hDialog);
BOOL     onMediaPlayerDialogCommandLoad(MEDIA_PLAYER_DATA*  pDialogData, HWND  hDialog, MEDIA_ITEM*  pItem);
BOOL     onMediaPlayerDialogCommandPause(MEDIA_PLAYER_DATA*  pDialogData, HWND  hDialog);
BOOL     onMediaPlayerDialogCommandPlay(MEDIA_PLAYER_DATA*  pDialogData, HWND  hDialog);
BOOL     onMediaPlayerDialogNotify(MEDIA_PLAYER_DATA*  pDialogData, HWND  hDialog, NMHDR*  pMessage);
BOOL     onMediaPlayerDialogPlaybackComplete(MEDIA_PLAYER_DATA*  pDialogData, HWND  hDialog);
BOOL     onMediaPlayerDialogPaint(MEDIA_PLAYER_DATA*  pDialogData, HWND  hDialog);
VOID     onMediaPlayerDialogResize(MEDIA_PLAYER_DATA*  pDialogData, HWND  hDialog, CONST UINT  iWidth, CONST UINT  iHeight);

INT_PTR  dlgprocMediaPlayerDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);
LRESULT  wndprocMediaPlayerDialogProgressCtrl(HWND  hCtrl, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                               MODAL DIALOG STACK
/// ////////////////////////////////////////////////////////////////////////////////////////

/// Macros
#define      createModalWindowStack      createStack
#define      deleteModalWindowStack      deleteStack

// Functions
HWND    topModalWindowStack();
VOID    pushModalWindowStack(HWND  hNewWnd);
VOID    popModalWindowStack();

/// ////////////////////////////////////////////////////////////////////////////////////////
///                               NEW DOCUMENT DIALOG
/// ////////////////////////////////////////////////////////////////////////////////////////

// Functions
NEW_DOCUMENT_DATA*  displayInsertDocumentDialog(MAIN_WINDOW_DATA*  pMainWindowData);
VOID                deleteInsertDocumentDialogData(NEW_DOCUMENT_DATA*  &pData);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                 OPERATION POOL
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
OPERATION_POOL*   createOperationPool();
HWND              createOperationPoolCtrl(OPERATION_POOL*  pOperationPool, HWND  hParentWnd);
VOID              deleteOperationPool(OPERATION_POOL*  &pOperationPool);

// Helpers
BOOL              findOperationDataByIndex(CONST OPERATION_POOL*  pOperationPool, CONST UINT  iIndex, OPERATION_DATA*  &pOutput);
UINT              getOperationCount(CONST OPERATION_POOL*  pOperationPool);
BOOL              isOperationPoolEmpty(CONST OPERATION_POOL*  pOperationPool);
VOID  setOperationPoolBatchMode(OPERATION_POOL*  pOperationPool, CONST BOOL  bEnabled);
VOID  setOperationPoolBatchProgress(OPERATION_POOL*  pOperationPool, CONST UINT  iProgress);


// Functions 
OPERATION_DATA*   getCurrentOperationData(CONST OPERATION_POOL*  pOperationPool);
UINT              getCurrentOperationProgress(CONST OPERATION_POOL*  pOperationPool);
UINT              getCurrentOperationStageID(CONST OPERATION_POOL*  pOperationPool);
UINT              identifyOperationStatusMessageID(CONST OPERATION_DATA*  pOperation);
VOID              insertOperationIntoOperationPool(OPERATION_POOL*  pOperationPool, OPERATION_DATA*  pOperationData);
BOOL              launchOperation(MAIN_WINDOW_DATA*  pMainWindowData, OPERATION_DATA*  pOperationData);
VOID              removeOperationFromOperationPool(OPERATION_POOL*  pOperationPool, OPERATION_DATA*  &pOperationData);

// Message Handlers
VOID              onOperationPoolDestroy(OPERATION_POOL*  pOperationPool);
VOID              onOperationPoolOperationComplete(OPERATION_POOL*  pOperationPool, OPERATION_DATA*  pOperationData);
ERROR_HANDLING    onOperationPoolProcessingError(OPERATION_POOL*  pOperationPool, CONST UINT  eButtons, CONST ERROR_STACK*  pError);

// Window procedure
LRESULT           wndprocOperationPool(HWND  hCtrl, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                  OUTPUT DIALOG
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
HWND                createOutputDialog(OUTPUT_DIALOG_DATA*  pDialogData, HWND  hParentWnd);
OUTPUT_DIALOG_DATA* createOutputDialogData();
OUTPUT_DIALOG_ITEM* createOutputDialogItem(CONST TCHAR*  szText, CONST TCHAR*  szIconID, CONST OPERATION_DATA*  pOperation, CONST UINT  iOperationIndex, CONST ERROR_STACK*  pError);
VOID                deleteOutputDialogData(OUTPUT_DIALOG_DATA*  pDialogData);
VOID                deleteOutputDialogItem(OUTPUT_DIALOG_ITEM*  &pItem);
VOID                deleteOutputDialogTreeNode(LPARAM  pOutputDialogItem);

// Helpers
UINT                calculateOutputDialogOperationID(HWND  hDialog);
LPARAM              extractOutputDialogTreeNode(LPARAM  pItem, CONST AVL_TREE_SORT_KEY  eProperty);
BOOL                findOutputDialogItem(OUTPUT_DIALOG_DATA*  pDialogData, CONST UINT  iIndex, OUTPUT_DIALOG_ITEM*  &pOutput);
CONST TCHAR*        identifyOutputDialogIconID(CONST OUTPUT_DIALOG_ICON  eIcon);
OUTPUT_DIALOG_DATA* getOutputDialogData(HWND  hDialog);

// Functions
VOID    clearOutputDialogText(HWND  hDialog);
VOID    initOutputDialog(OUTPUT_DIALOG_DATA*  pDialogData);
VOID    insertOutputDialogItem(HWND  hDialog, OUTPUT_DIALOG_ITEM*  pNewItem);
VOID    printErrorToOutputDialog(CONST ERROR_STACK*  pError);
VOID    printMessageToOutputDialogf(CONST OUTPUT_DIALOG_ICON  eIcon, CONST UINT  iMessageID, ...);
VOID    printOperationStateToOutputDialogf(OPERATION_DATA*  pOperation, CONST UINT  iMessageID, ...);
VOID    printOperationErrorToOutputDialog(CONST OPERATION_DATA*  pOperation, CONST UINT  iItemIndex, CONST ERROR_STACK*  pError, CONST BOOL  bAddToConsole);
VOID    updateOutputDialogList(OUTPUT_DIALOG_DATA*  pDialogData);

// Message Handlers
VOID    onOutputDialogClose(OUTPUT_DIALOG_DATA*  pDialogData);
BOOL    onOutputDialogCommand(OUTPUT_DIALOG_DATA*  pDialogData, CONST UINT  iControlID, CONST UINT  iNotification);
VOID    onOutputDialogContextMenu(OUTPUT_DIALOG_DATA*  pDialogData, HWND  hCtrl, POINT*  ptCursor);
BOOL    onOutputDialogDoubleClick(OUTPUT_DIALOG_DATA*  pDialogData, NMITEMACTIVATE*  pClickData);
BOOL    onOutputDialogGetMenuItemState(OUTPUT_DIALOG_DATA*  pDialogData, CONST UINT  iCommandID, UINT*  piState);
BOOL    onOutputDialogMoving(OUTPUT_DIALOG_DATA*  pDialogData, RECT*  pWindowRect);
BOOL    onOutputDialogNotify(OUTPUT_DIALOG_DATA*  pDialogData, CONST UINT  iControlID, NMHDR*  pMessage);
VOID    onOutputDialogResize(OUTPUT_DIALOG_DATA*  pDialogData, CONST SIZE*  pNewSize);
VOID    onOutputDialogRequestData(OUTPUT_DIALOG_DATA*  pDialogData, LVITEM*  pOutput);

// Dialog procedure
INT_PTR dlgprocOutputDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                  PROJECT DOCUMENT
/// ////////////////////////////////////////////////////////////////////////////////////////

// Helpers
PROJECT_FOLDER     calculateProjectFolderFromDocumentType(CONST DOCUMENT_TYPE  eType);
PROJECT_DOCUMENT*  getActiveProject();
PROJECT_FILE*      getActiveProjectFile();
const TCHAR*       getActiveProjectFileName();
BOOL               isDocumentInProject(CONST DOCUMENT*  pDocument);
VOID               setProjectModifiedFlag(PROJECT_DOCUMENT*  pProject, CONST BOOL  bModified);

// Functions
BOOL          addDocumentToProject(CONST DOCUMENT*  pDocument);
CLOSURE_TYPE  closeActiveProject(CONST TCHAR*  szReplacementProject);
BOOL          removeDocumentFromProject(CONST DOCUMENT*  pDocument);
VOID          setActiveProject(PROJECT_DOCUMENT*  pNewProject);

// Message Handlers
VOID          onProject_LoadComplete(DOCUMENT_OPERATION*  pOperationData);
VOID          onProject_SaveComplete(DOCUMENT_OPERATION*  pOperationData);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                  PROJECT DIALOG
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
HWND                   createProjectDialog(HWND  hParentWnd);
PROJECT_DIALOG_DATA*   createProjectDialogData();
VOID                   deleteProjectDialogData(PROJECT_DIALOG_DATA*  &pDialogData);

// Helpers
PROJECT_DIALOG_DATA*   getProjectDialogData(HWND  hDialog);
STORED_DOCUMENT*       getDocumentFromProjectDialogItem(PROJECT_DIALOG_DATA*  pDialogData, HTREEITEM  hItem);

// Functions
BOOL     addDocumentToProjectDialog(HWND  hProjectDlg, CONST STORED_DOCUMENT*  pDocument);
VOID     initProjectDialog(PROJECT_DIALOG_DATA*  pDialogData, HWND  hDialog);
BOOL     removeDocumentFromProjectDialog(CONST DOCUMENT*  pDocument);
VOID     updateProjectDialog(HWND  hDialog);
VOID     updateProjectDialogRoot(HWND  hDialog);

// Message Handlers
VOID     onProjectDialog_Close(PROJECT_DIALOG_DATA*  pDialogData);
VOID     onProjectDialog_ContextMenu(PROJECT_DIALOG_DATA*  pDialogData, HWND  hCtrl, CONST POINT*  ptCursor);
BOOL     onProjectDialog_Command(PROJECT_DIALOG_DATA*  pDialogData, CONST UINT  iControl, CONST UINT  iNotification, HWND  hCtrl);
BOOL     onProjectDialog_CustomDraw(PROJECT_DIALOG_DATA*  pDialogData, NMTVCUSTOMDRAW*  pMessage);
VOID     onProjectDialog_Destroy(PROJECT_DIALOG_DATA*  pDialogData);
BOOL     onProjectDialog_GetMenuItemState(PROJECT_DIALOG_DATA*  pDialogData, CONST UINT  iCommandID, UINT*  piState);
BOOL     onProjectDialog_Moving(PROJECT_DIALOG_DATA*  pDialogData, RECT*  pWindowRect);
BOOL     onProjectDialog_Notify(PROJECT_DIALOG_DATA*  pDialogData, NMHDR*  pMessage);
VOID     onProjectDialog_RequestData(PROJECT_DIALOG_DATA*  pDialogData, NMTVDISPINFO*  pHeader);
VOID     onProjectDialog_Resize(PROJECT_DIALOG_DATA*  pDialogData, CONST SIZE*  pNewSize);

// Window Procedure
INT_PTR  dlgprocProjectDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                               PROJECT VARIABLES DIALOG
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction

// Helpers

// Functions
BOOL     displayProjectVariablesDialog(HWND  hParent);
BOOL     initProjectVariablesDialog(HWND  hDialog);

// Message Handlers
VOID     onProjectVariablesDialog_AddVariable(HWND  hDialog, HWND  hListView);
BOOL     onProjectVariablesDialog_Command(HWND  hDialog, CONST UINT  iControlID, HWND  hCtrl);
VOID     onProjectVariablesDialog_ContextMenu(HWND  hDialog, HWND  hListView, CONST POINT*  ptCursor);
VOID     onProjectVariablesDialog_RemoveVariable(HWND  hDialog, HWND  hListView);
VOID     onProjectVariablesDialog_RequestData(HWND  hDialog, LVITEM&  oOutput);

// Window Procedure
INT_PTR  dlgprocProjectVariablesDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                               PREFERENCES DIALOG (BASE)
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
PREFERENCES_DATA*  createPreferencesData(CONST PREFERENCES*  pAppPreferences);
HWND               createPreferencesTooltips(HWND  hDialog, CONST PREFERENCES_PAGE  ePage);
VOID               deletePreferencesData(PREFERENCES_DATA*  pDialogData);

// Helpers
PREFERENCES_DATA*  getPreferencesData(HWND  hDialog);

// Functions
VOID     performPreferencesFolderBrowse(PREFERENCES_DATA*  pDialogData, HWND  hDialog, CONST UINT  iDestinationControl);
VOID     performPreferencesFolderValidation(PREFERENCES_DATA*  pDialogData, HWND  hDialog, CONST UINT  iComboControlID);

// Message Handlers
INT      onPreferencesDialog_Close(HWND  hParentWnd, PREFERENCES*  pNewPreferences);
VOID     onPreferencesPage_Help(PREFERENCES_DATA*  pDialogData, CONST PREFERENCES_PAGE  ePage, CONST HELPINFO*  pRequest);
BOOL     onPreferencesPage_Notify(PREFERENCES_DATA*  pDialogData, HWND  hDialog, NMHDR*  pMessage, CONST PREFERENCES_PAGE  ePage);

// Callbacks
INT      callbackPreferencesDialog(HWND  hSheet, UINT  iMessage, LPARAM  lParam);
UINT     callbackPreferencesPage(HWND  hPage, UINT  iMessage, PROPSHEETPAGE*  pPageData);

// Window procedure
INT_PTR  dlgprocPreferencesPage(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam, CONST PREFERENCES_PAGE  ePage);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                            PREFERENCES PROPERTY SHEET (UI)
/// ////////////////////////////////////////////////////////////////////////////////////////

// Helpers
INT      callbackPreferencesFontEnumeration(CONST ENUMLOGFONTEX*  pFontData, CONST NEWTEXTMETRICEX*  pFontSize, DWORD  dwType, LPARAM  lParam);
VOID     populateAppearancePageFontSizesCombo(PREFERENCES_DATA*  pDialogData, HWND  hDialog);

// Functions
BOOL     displayPreferencesDialog(HWND  hParentWnd, CONST PREFERENCES_PAGE  ePage);
BOOL     initPreferencesPage(PREFERENCES_DATA*  pDialogData, HWND  hDialog, CONST PREFERENCES_PAGE  ePage);
BOOL     initPreferencesAppearancePage(PREFERENCES_DATA*  pDialogData, HWND  hDialog);
BOOL     initPreferencesFolderPage(PREFERENCES_DATA*  pDialogData, HWND  hDialog);
BOOL     initPreferencesGeneralPage(PREFERENCES_DATA*  pDialogData, HWND  hDialog);
BOOL     initPreferencesMiscPage(PREFERENCES_DATA*  pDialogData, HWND  hDialog);
BOOL     initPreferencesSyntaxPage(PREFERENCES_DATA*  pDialogData, HWND  hDialog);
VOID     updatePreferencesFoldersPageState(CONST PREFERENCES*  pPreferencesCopy, HWND  hPage, CONST UINT  iIconControlID);
VOID     updatePreferencesGeneralPageSliderValue(PREFERENCES_DATA*  pDialogData, HWND  hPage, CONST UINT  iControlID, UINT  iDelay);

// Message Handlers
BOOL     onPreferencesAppearancePage_Command(PREFERENCES_DATA*  pDialogData, HWND  hDialog, CONST UINT  iControlID, CONST UINT  iNotification, HWND  hCtrl);
BOOL     onPreferencesFoldersPage_Command(PREFERENCES_DATA*  pDialogData, HWND  hDialog, CONST UINT  iControlID, CONST UINT  iNotification);
VOID     onPreferencesGeneralPage_AdjustDelay(PREFERENCES_DATA*  pDialogData, HWND  hPage, CONST UINT  iControlID, CONST UINT  eScrollType, CONST UINT  iAmount);
BOOL     onPreferencesPage_Hide(PREFERENCES_DATA*  pDialogData, HWND  hDialog, CONST PREFERENCES_PAGE  ePage);
BOOL     onPreferencesPage_Show(PREFERENCES_DATA*  pDialogData, HWND  hDialog, CONST PREFERENCES_PAGE  ePage);
BOOL     onPreferencesSyntaxPage_Command(PREFERENCES_DATA*  pDialogData, HWND  hDialog, CONST UINT  iControlID, CONST UINT  iNotification);
BOOL     onPreferencesSyntaxPage_Notify(PREFERENCES_DATA*  pDialogData, HWND  hPage, NMHDR*  pHeader);

// Window procedures
INT_PTR  dlgprocPreferencesAppearancePage(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);
INT_PTR  dlgprocPreferencesFoldersPage(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);
INT_PTR  dlgprocPreferencesGeneralPage(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);
INT_PTR  dlgprocPreferencesMiscPage(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);
INT_PTR  dlgprocPreferencesSyntaxPage(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                           PROPERTIES DIALOG (BASE)
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
HWND              createPropertiesDialog(MAIN_WINDOW_DATA*  pMainWindowData);
PROPERTIES_DATA*  createPropertiesDialogData();
VOID              createPropertiesDialogPageData(PROPERTIES_DATA*  pPropertiesData, PROPSHEETPAGE*  pPageData, CONST PROPERTY_PAGE  ePage);
HIMAGELIST  createPropertiesDialogImageList(PROPERTY_DIALOG_TYPE  eType);
VOID              deletePropertiesDialogData(PROPERTIES_DATA*  &pDialogData);
VOID              destroyPropertiesDialog(MAIN_WINDOW_DATA*  pMainWindowData);

// Helpers
VOID                 displayPropertiesDialog(MAIN_WINDOW_DATA*  pMainWindowData, CONST BOOL  bShow);
PROPERTY_PAGE        getPropertiesDialogCurrentPageID(PROPERTIES_DATA*  pPropertiesData);
PROPERTIES_DATA*     getPropertiesDialogData(HWND  hDialog);
PROPERTY_DIALOG_TYPE identifyPropertiesDialogType(CONST DOCUMENT*  pDocument);
UINT                 identifyPropertiesDialogPageCount(CONST PROPERTY_DIALOG_TYPE  eType);
BOOL                 isPropertiesDialogAlreadyClosed(MAIN_WINDOW_DATA*  pMainWindowData);

// Functions
VOID  initPropertiesDialogPage(HWND  hPage, CONST PROPERTY_PAGE  ePage, CONST PROPSHEETPAGE*  pPageData);
VOID  initPropertiesDialogPageControls(PROPERTIES_DATA*  pSheetData, HWND  hPage, CONST PROPERTY_PAGE  ePage);
VOID  initPropertiesDialogPageListView(HWND  hListView, const LISTVIEW_COLUMNS*  pColumnData);
HWND  initPropertiesDialogPageTooltips(PROPERTIES_DATA*  pSheetData, HWND  hPage, CONST PROPERTY_PAGE  ePage);

// Message Handlers
VOID  onPropertiesDialog_Activate(HWND  hSheet, CONST UINT  iFlags);
INT   onPropertiesDialog_Create(HWND  hSheet, UINT  iMessage, LPARAM  lParam);
VOID  onPropertiesDialog_DocumentSwitched(PROPERTIES_DATA*  pPropertiesData, DOCUMENT*  pNewDocument);
VOID  onPropertiesDialog_Help(PROPERTIES_DATA*  pSheetData, PROPERTY_PAGE  ePage, CONST HELPINFO*  pRequest);
BOOL  onPropertiesDialog_Notify(PROPERTIES_DATA*  pSheetData, HWND  hPage, PROPERTY_PAGE  ePage, NMHDR*  pMessageHeader);
VOID  onPropertiesDialog_PageHide(PROPERTIES_DATA*  pSheetData, HWND  hPage, CONST PROPERTY_PAGE  ePage);
VOID  onPropertiesDialog_PageShow(PROPERTIES_DATA*  pSheetData, HWND  hPage, CONST PROPERTY_PAGE  ePage);

// Window procedures
INT_PTR  dlgprocPropertiesBlankPage(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);
INT_PTR  dlgprocPropertiesPage(HWND  hPage, UINT  iMessage, WPARAM  wParam, LPARAM  lParam, PROPERTY_PAGE  ePage);
INT_PTR  dlgprocPropertiesSheet(HWND  hSheet, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);


/// ////////////////////////////////////////////////////////////////////////////////////////
///                        PROPERTIES DIALOG (SCRIPT)
/// ////////////////////////////////////////////////////////////////////////////////////////

/// Page: General
// Message Handlers
BOOL   onGeneralPage_CommandS(PROPERTIES_DATA*  pSheetData, HWND  hPage, CONST UINT  iControlID, CONST UINT  iNotification, HWND  hCtrl);
BOOL   onGeneralPage_DrawItem(DRAWITEMSTRUCT*  pDrawInfo);
VOID   onGeneralPage_PropertyChanged(PROPERTIES_DATA*  pSheetData, HWND  hPage, CONST UINT  iControlID);
VOID   onScriptPage_Show(PROPERTIES_DATA*  pSheetData, HWND  hPage, CONST PROPERTY_PAGE  ePage);

/// Page: Arguments
// Message Handlers
BOOL   onArgumentPage_ContextMenu(SCRIPT_DOCUMENT*  pDocument, HWND  hCtrl, CONST POINT  ptCursor);
BOOL   onArgumentPage_Command(SCRIPT_DOCUMENT*  pDocument, HWND  hPage, const UINT  iControl, const UINT  iNotification, HWND  hCtrl);
BOOL   onArgumentPage_CustomDraw(SCRIPT_DOCUMENT*  pDocument, HWND  hPage, CONST UINT  iControlID, NMHDR*  pHeader);
VOID   onArgumentPage_DeleteArgument(SCRIPT_DOCUMENT*  pDocument, HWND  hDialog);
VOID   onArgumentPage_EditArgument(SCRIPT_DOCUMENT*  pDocument, HWND  hListView);
VOID   onArgumentPage_EditBegin(SCRIPT_DOCUMENT*  pDocument, NMLVDISPINFO*  pHeader);
BOOL   onArgumentPage_EditEnd(SCRIPT_DOCUMENT*  pDocument, NMLVDISPINFO*  pHeader);
VOID   onArgumentPage_InsertArgument(SCRIPT_DOCUMENT*  pDocument, HWND  hDialog);
BOOL   onArgumentPage_ItemChanged(SCRIPT_DOCUMENT*  pActiveDocument, HWND  hDialog, NMLISTVIEW*  pItemData);
BOOL   onArgumentPage_Notify(SCRIPT_DOCUMENT*  pDocument, HWND  hDialog, NMHDR*  pMessage);
BOOL   onArgumentPage_RequestData(CONST SCRIPT_DOCUMENT*  pDocument, HWND  hDialog, NMLVDISPINFO*  pMessageData);

/// Pages: Dependencies/Strings/Variables
// Functions
VOID   updateScriptDependenciesPage_List(PROPERTIES_DATA*  pSheetData, HWND  hPage);
VOID   updateScriptStringsPage_List(PROPERTIES_DATA*  pSheetData, HWND  hPage);
VOID   updateScriptVariablesPage_List(PROPERTIES_DATA*  pSheetData, HWND  hPage);

// Message Handlers
BOOL   onDependenciesPage_ContextMenu(SCRIPT_DOCUMENT*  pDocument, HWND  hCtrl, CONST UINT  iCursorX, CONST UINT  iCursorY);
VOID   onDependenciesPage_LoadSelectedScripts(HWND  hPage, SCRIPT_FILE*  pScriptFile, AVL_TREE*  pDependenciesTree);
VOID   onDependenciesPage_PerformSearch(SCRIPT_DOCUMENT*  pDocument, HWND  hPage);
VOID   onDependenciesPage_RequestData(PROPERTIES_DATA*  pSheetData, HWND  hPage, NMLVDISPINFO*  pMessageData);
VOID   onStringsPage_RequestData(PROPERTIES_DATA*  pSheetData, HWND  hPage, NMLVDISPINFO*  pMessageData);
VOID   onVariablesPage_RequestData(PROPERTIES_DATA*  pSheetData, HWND  hPage, NMLVDISPINFO*  pMessageData);

// Dialog procedures
INT_PTR   dlgprocArgumentPage(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);
INT_PTR   dlgprocGeneralPageS(HWND  hPage, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);
INT_PTR   dlgprocDependenciesPage(HWND  hPage, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);
INT_PTR   dlgprocStringsPage(HWND  hPage, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);
INT_PTR   dlgprocVariablesPage(HWND  hPage, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                        PROPERTIES DIALOG (LANGUAGE)
/// ////////////////////////////////////////////////////////////////////////////////////////

// Helpers
VOID                   enablePropertiesDialogControls(HWND  hPage, const BOOL  bEnable);
LANGUAGE_MESSAGE*      getLanguageMessage(PROPERTIES_DATA*  pSheetData);

/// Page: General
// Functions
VOID    displayColumnPageSliderText(HWND  hPage, CONST UINT  iControlID, CONST UINT  iValue);

// Handlers
VOID    onLanguagePage_Show(PROPERTIES_DATA*  pSheetData, HWND  hPage, CONST PROPERTY_PAGE  ePage);
BOOL    onGeneralPage_CommandL(PROPERTIES_DATA*  pSheetData, HWND  hDialog, CONST UINT  iControl, CONST UINT  iNotification, HWND  hCtrl);

/// Page: Buttons
// Message Handlers
//BOOL    onButtonPage_Command(LANGUAGE_DOCUMENT*  pDocument, HWND  hDialog, CONST UINT  iControl, CONST UINT  iNotification, HWND  hCtrl);
BOOL    onButtonPage_ContextMenu(PROPERTIES_DATA*  pSheetData, HWND  hPage, HWND  hCtrl, CONST POINT*  ptCursor);
VOID    onButtonPage_LabelEditBegin(PROPERTIES_DATA*  pSheetData, HWND  hPage, NMLVDISPINFO*  pLabelData);
VOID    onButtonPage_LabelEditEnd(PROPERTIES_DATA*  pSheetData, HWND  hPage, NMLVDISPINFO*  pLabelData);
BOOL    onButtonPage_Notification(PROPERTIES_DATA*  pSheetData, HWND  hPage, NMHDR*  pMessage);
VOID    onButtonPage_RequestData(PROPERTIES_DATA*  pSheetData, HWND  hPage, LVITEM&  oOutput);

/// Page: Columns
// Message Handlers
BOOL    onColumnsPage_Command(PROPERTIES_DATA*  pSheetData, HWND  hPage, CONST UINT  iControl, CONST UINT  iNotification, HWND  hCtrl);
BOOL    onColumnsPage_DrawGroup(PROPERTIES_DATA*  pSheetData, HWND  hPage, DRAWITEMSTRUCT*  pDrawData);
BOOL    onColumnsPage_DrawRadio(PROPERTIES_DATA*  pSheetData, HWND  hPage, NMCUSTOMDRAW*  pDrawData);
BOOL    onColumnsPage_Scroll(PROPERTIES_DATA*  pSheetData, HWND  hPage, CONST UINT  iScrollType, UINT  iPosition, HWND  hCtrl);

/// Page: Special
// Message Handlers
BOOL    onSpecialPage_Command(PROPERTIES_DATA*  pSheetData, HWND  hDialog, CONST UINT  iControl, CONST UINT  iNotification, HWND  hCtrl);

// Dialog procedures
INT_PTR    dlgprocButtonPage(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);
INT_PTR    dlgprocColumnsPage(HWND  hPage, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);
INT_PTR    dlgprocGeneralPageL(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);
INT_PTR    dlgprocSpecialPage(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                PROGRESS DIALOG
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
PROGRESS_DATA*  createProgressDialogData(OPERATION_POOL*  pOperationPool);
VOID            deleteProgressDialogData(PROGRESS_DATA*  &pProgressData);

// Helpers
PROGRESS_DATA*  getProgressDialogData(HWND  hDialog);

// Functions
HWND     displayProgressDialog(MAIN_WINDOW_DATA*  pMainWindowData);
BOOL     initProgressDialog(PROGRESS_DATA*  pDialogData);
VOID     setProgressDialogProgressValue(PROGRESS_DATA*  pDialogData, CONST UINT  iCurrentProgress);
VOID     updateProgressDialogStageDescription(PROGRESS_DATA*  pDialogData);
VOID     updateProgressDialogTitle(PROGRESS_DATA*  pDialogData);

// Message Handlers
VOID     onProgressDialogDestroy(PROGRESS_DATA*  pDialogData);
VOID     onProgressDialogTimer(PROGRESS_DATA*  pDialogData);

// Window Procedre
INT_PTR  dlgprocProgressDlg(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                RICH TEXT DIALOG
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
BOOL    createRichTextDialogToolBar(LANGUAGE_DOCUMENT*  pDocument, HWND  hDialog);

// Helpers
UINT   identifyRichTextDialogToolBarCommandID(CONST UINT  iIndex);

// Functions 
UINT   getRichTextDialogButtonCount(LANGUAGE_DOCUMENT*  pDocument);
BOOL   initRichTextDialog(LANGUAGE_DOCUMENT*  pDocument, HWND  hDialog);
BOOL   performRichEditFormatCommand(LANGUAGE_DOCUMENT*  pDocument, HWND  hDialog, CONST UINT  iCommand);
BOOL   updateRichTextDialogToolBar(LANGUAGE_DOCUMENT*  pDocument, HWND  hDialog);

// Message Handlers
BOOL   onRichTextDialog_Command(LANGUAGE_DOCUMENT*  pDocument, HWND  hDialog, CONST UINT  iControlID, CONST UINT  iNotification);
BOOL   onRichTextDialog_ContextMenu(LANGUAGE_DOCUMENT*  pDocument, HWND  hDialog, CONST UINT  iMenuIndex);
BOOL   onRichTextDialog_Destroy(LANGUAGE_DOCUMENT*  pDocument, HWND  hDialog);
BOOL   onRichTextDialog_DestroyButton(LANGUAGE_DOCUMENT*  pDocument, IOleObject*  pObject);
BOOL   onRichTextDialog_GetMenuItemState(LANGUAGE_DOCUMENT*  pDocument, CONST UINT  iCommandID, UINT*  piState);
BOOL   onRichTextDialog_InsertButton(LANGUAGE_DOCUMENT*  pDocument);
BOOL   onRichTextDialog_Notify(LANGUAGE_DOCUMENT*  pDocument, HWND  hDialog, NMHDR*  pMessage);
BOOL   onRichTextDialog_Resize(LANGUAGE_DOCUMENT*  pDocument, HWND  hDialog, CONST UINT  iWidth, CONST UINT  iHeight);

INT_PTR CALLBACK  dlgprocRichTextDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                SOURCETEXT DIALOG
/// ////////////////////////////////////////////////////////////////////////////////////////

// Functions
BOOL  displaySourceTextDialog(LANGUAGE_DOCUMENT*  pDocument, GAME_STRING*  pTargetString, HWND  hParentWnd);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                SCRIPT DOCUMENTS
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction


// Functions
VOID    updateScriptDocumentLabelsCombo(SCRIPT_DOCUMENT*  pDocument);
VOID    updateScriptDocumentVariablesCombo(SCRIPT_DOCUMENT*  pDocument);

// Message Handlers
VOID    onScriptDocumentContextMenu(SCRIPT_DOCUMENT*  pDocument, HWND  hDialog, CONST POINT*  ptCursor, HWND  hCtrl);
BOOL    onScriptDocumentCommand(SCRIPT_DOCUMENT*  pDocument, CONST UINT  iControlID, CONST UINT  iNotification);
VOID    onScriptDocumentCreate(SCRIPT_DOCUMENT*  pDocument, HWND  hWnd);
VOID    onScriptDocumentDestroy(SCRIPT_DOCUMENT*  pDocument);
BOOL    onScriptDocumentGetMenuItemState(SCRIPT_DOCUMENT*  pDocument, CONST UINT  iCommandID, UINT*  piState);
VOID    onScriptDocumentGotoLabel(SCRIPT_DOCUMENT*  pDocument);
VOID    onScriptDocumentHelp(SCRIPT_DOCUMENT*  pDocument, HWND  hDialog, CONST HELPINFO*  pRequest);
VOID    onScriptDocumentLoadComplete(SCRIPT_DOCUMENT*  pDocument, DOCUMENT_OPERATION*  pOperationData);
VOID    onScriptDocumentLookupCommand(SCRIPT_DOCUMENT*  pDocument);
VOID    onScriptDocumentLoseFocus(SCRIPT_DOCUMENT*  pDocument, HWND  hDialog);
VOID    onScriptDocumentOpenTargetScript(SCRIPT_DOCUMENT*  pDocument);
BOOL    onScriptDocumentNotify(SCRIPT_DOCUMENT*  pDocument, NMHDR*  pMessage);
VOID    onScriptDocumentPreferencesChanged(SCRIPT_DOCUMENT*  pDocument);
VOID    onScriptDocumentReceiveFocus(SCRIPT_DOCUMENT*  pDocument, HWND  hDialog);
VOID    onScriptDocumentResize(SCRIPT_DOCUMENT*  pDocument, CONST SIZE*  pNewSize);
VOID    onScriptDocumentSaveComplete(SCRIPT_DOCUMENT*  pDocument, DOCUMENT_OPERATION*  pOperationData);
VOID    onScriptDocumentScopeChange(SCRIPT_DOCUMENT*  pDocument, CODE_EDIT_LABEL*  pNewScope);
VOID    onScriptDocumentSelectionChange(SCRIPT_DOCUMENT*  pDocument);
VOID    onScriptDocumentTextChange(SCRIPT_DOCUMENT*  pDocument);
VOID    onScriptDocumentPropertyChanged(SCRIPT_DOCUMENT*  pDocument, CONST UINT  iProperty);
VOID    onScriptDocumentValidationComplete(SCRIPT_DOCUMENT*  pDocument, DOCUMENT_OPERATION*  pOperationData);
VOID    onScriptDocumentViewLanguageString(SCRIPT_DOCUMENT*  pDocument);
VOID    onScriptDocumentViewError(SCRIPT_DOCUMENT*  pDocument);

// Window procedure
LRESULT  wndprocScriptDocument(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                  SEARCH RESULTS DIALOG
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
HWND                  createResultsDialog(HWND  hParentWnd, CONST RESULT_TYPE  eType);
RESULTS_DIALOG_DATA*  createResultsDialogData(CONST RESULT_TYPE  eType);
AVL_TREE*             createResultsDialogTree(const RESULT_TYPE  eType);
VOID                  deleteResultsDialogData(RESULTS_DIALOG_DATA*  &pDialogData);

// Helpers
RESULTS_DIALOG_DATA*  getResultsDialogData(HWND  hDialog);
UINT                  identifyResultsDialogFilter(HWND  hDialog);

// Functions
BOOL   findResultsDialogItemByIndex(RESULTS_DIALOG_DATA*  pDialogData, CONST INT  iLogicalIndex, SUGGESTION_RESULT&  xOutput);
VOID   initResultsDialog(RESULTS_DIALOG_DATA*  pDialogData, HWND  hDialog);
VOID   initResultsDialogControls(RESULTS_DIALOG_DATA*  pDialogData);
VOID   performResultsDialogQuery(RESULTS_DIALOG_DATA*  pDialogData, CONST TCHAR*  szSearchTerm, CONST UINT  iGroupFilter, CONST BOOL  bCompatibleOnly);
VOID   updateResultsDialog(HWND  hDialog);

// Message Handlers
BOOL   onResultsDialog_Command(RESULTS_DIALOG_DATA*  pWindowData, CONST UINT  iControlID, CONST UINT  iNotification, HWND  hCtrl);
VOID   onResultsDialog_ContextMenu(RESULTS_DIALOG_DATA*  pDialogData, HWND  hCtrl, POINT*  ptCursor);
VOID   onResultsDialog_Destroy(RESULTS_DIALOG_DATA*  pWindowData);
VOID   onResultsDialog_LookupCommand(RESULTS_DIALOG_DATA*  pDialogData);
BOOL   onResultsDialog_MeasureItem(RESULTS_DIALOG_DATA*  pDialogData, MEASUREITEMSTRUCT*  pMessage);
BOOL   onResultsDialog_Notify(RESULTS_DIALOG_DATA*  pWindowData, NMHDR*  pMessage);
VOID   onResultsDialog_InsertResult(RESULTS_DIALOG_DATA*  pWindowData, CONST SUGGESTION_RESULT  xResult);
VOID   onResultsDialog_PreferencesChanged(RESULTS_DIALOG_DATA*  pDialogData);
VOID   onResultsDialog_RequestData(RESULTS_DIALOG_DATA*  pDialogData, NMLVDISPINFO*  pHeader);
BOOL   onResultsDialog_RequestTooltipData(RESULTS_DIALOG_DATA*  pDialogData, HWND  hListView, NMLVGETINFOTIP*  pHeader);
VOID   onResultsDialog_Resize(RESULTS_DIALOG_DATA*  pWindowData, CONST SIZE*  pDialogSize);
VOID   onResultsDialog_Show(RESULTS_DIALOG_DATA*  pDialogData);
VOID   onResultsDialog_SearchContent(RESULTS_DIALOG_DATA*  pDialogData);
VOID   onResultsDialog_SubmitCorrection(RESULTS_DIALOG_DATA*  pDialogData);
VOID   onResultsDialog_Timer(RESULTS_DIALOG_DATA*  pWindowData, CONST UINT  iTimerID);

// Tree Operations
VOID      treeprocSearchGameData(AVL_TREE_NODE*  pNode, AVL_TREE_OPERATION*  pOperationData);

// Window procedure
INT_PTR   dlgprocResultsDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                  SEARCH TAB DIALOG
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
HWND                 createSearchDialog(HWND  hParentWnd, CONST RESULT_TYPE  eInitialDialog);
SEARCH_DIALOG_DATA*  createSearchDialogData(CONST RESULT_TYPE  eInitialDialog);
VOID                 deleteSearchDialogData(SEARCH_DIALOG_DATA*  &pDialogData);

// Helpers
SEARCH_DIALOG_DATA*  getSearchDialogData(HWND  hDialog);
RESULT_TYPE          identifyActiveResultsDialog(HWND  hSearchDialog);
HWND                 identifyActiveResultsDialogHandle(HWND  hSearchDialog);
UINT                 identifyResultsDialogFilterByType(HWND  hSearchDialog, CONST RESULT_TYPE  eDialog);

// Functions
BOOL               setActiveResultsDialogByType(HWND  hSearchDialog, CONST RESULT_TYPE  eDialog);
BOOL               initSearchDialog(SEARCH_DIALOG_DATA*  pDialogData, HWND  hDialog, HWND  hFocusCtrl);

// Message Handlers
VOID               onSearchDialog_Destroy(SEARCH_DIALOG_DATA*  pDialogData);
VOID               onSearchDialog_DocumentSwitched(SEARCH_DIALOG_DATA*  pDialogData, DOCUMENT*  pNewDocument);
VOID               onSearchDialog_DocumentPropertyUpdated(SEARCH_DIALOG_DATA*  pDialogData, CONST UINT  iControlID);
VOID               onSearchDialog_SelectionChange(SEARCH_DIALOG_DATA*  pDialogData);
VOID               onSearchDialog_Resize(SEARCH_DIALOG_DATA*  pDialogData, CONST SIZE*  pNewSize);

// Window Procedure
INT_PTR            dlgprocSearchDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                      SPLASH WINDOW
/// ////////////////////////////////////////////////////////////////////////////////////////

// Functions
HWND     displaySplashWindow(MAIN_WINDOW_DATA*  pMainWindowData);
VOID     drawSplashWindow(SPLASH_WINDOW_DATA*  pWindowData);

// Message Handlers
VOID     onSplashWindowCreate(SPLASH_WINDOW_DATA*  pWindowData, HWND  hWnd);
VOID     onSplashWindowDestroy(SPLASH_WINDOW_DATA*  pWindowData);
VOID     onSplashWindowTimer(SPLASH_WINDOW_DATA*  pWindowData);

// Window procedure
LRESULT  wndprocSplashWindow(HWND  hWnd, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                      TESTING
/// ////////////////////////////////////////////////////////////////////////////////////////

// Helpers

// Functions
BOOL   testErrorMessageDialog();
BOOL   testMessageDialog();
BOOL   testOutputDialog(HWND  hOutputDialog);

//
BOOL  testRichTextConversion(HWND  hParent, const AVL_TREE*  pGameStringTree);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                      TEST CASES
/// ////////////////////////////////////////////////////////////////////////////////////////
#ifdef _TESTING

// Helpers
BOOL    checkCommandTranslation(CONST LIST* pCommandList, CONST UINT  iIndex, CONST TCHAR*  szTranslation);
BOOL    checkErrorQueueMessages(CONST ERROR_QUEUE*  pErrorQueue, CONST UINT  iTestCaseID, CONST UINT  iMessageCount, ...);
BOOL    hasErrorStackMessage(CONST ERROR_STACK*  pError, CONST UINT  iMessageID);
BOOL    hasErrorQueueMessage(CONST ERROR_QUEUE*  pErrorQueue, CONST UINT  iMessageID);
TCHAR*  generateTestCaseFolder(CONST UINT  iTestCaseID);

// Functions
BOOL            findNextTestCaseID(CONST UINT  iTestCaseID, UINT&  iOutput);
TEST_CASE_TYPE  identifyTestCaseType(CONST UINT  iTestCaseID);
VOID    runGameDataTestCase(MAIN_WINDOW_DATA*  pMainWindowData, CONST UINT  iTestCaseID);
VOID    runScriptTranslationTestCase(MAIN_WINDOW_DATA*  pMainWindowData, CONST UINT  iTestCaseID);
BOOL    verifyGameDataTestCase(CONST UINT  iTestCaseID, CONST OPERATION_DATA*  pOperationData);
BOOL    verifyScriptTranslationTestCase(CONST UINT  iTestCaseID, CONST OPERATION_DATA*  pOperationData);

// Message Handlers
BOOL    onTestCaseComplete(MAIN_WINDOW_DATA*  pMainWindowData, CONST OPERATION_DATA*  pOperationData);

#endif
/// ////////////////////////////////////////////////////////////////////////////////////////
///                                   TOOLTIP WINDOWS
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
HWND     createTooltipWindow(HWND  hParentWnd);

// Helpers
UINT     getTooltipControlID(NMTTDISPINFO*  pData);

// Functions
VOID     addTooltipTextToControl(HWND  hTooltip, HWND  hParentWnd, CONST UINT  iControlID);
BOOL     onTooltipRequestText(NMTTDISPINFO*  pData);
BOOL     onTooltipRequestSpecifiedText(NMTTDISPINFO*  pData, CONST UINT  iResourceID);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                   TUTORIAL DIALOG
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction


// Helpers


// Functions
BOOL      displayTutorialDialog(HWND  hParentWnd, CONST TUTORIAL_WINDOW  eDialogID, CONST BOOL  bForceDisplay);
BOOL      initTutorialDialog(HWND  hDialog);

// Dialog procedure
INT_PTR   dlgprocTutorialDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);


// ////////////////////////////////////////////////////////////////////////////////////////
//                                          TESTING
// ////////////////////////////////////////////////////////////////////////////////////////




