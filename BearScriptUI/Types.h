//
// Types.h : Declaration of various structures, enumerations and constants
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#pragma once

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////

// Declaration
typedef        STACK               MODAL_WINDOW_STACK;

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                   BATCH TESTING
///
///   Contains a FileSearch used for iterating through files during batch testing
/// ////////////////////////////////////////////////////////////////////////////////////////

// Available batch test commands
//
enum BATCH_TEST_COMMAND    { BTC_NEXT_FILE, BTC_STOP };

// Batch test data
//
struct BATCH_TEST
{
   FILE_SEARCH*  pFileSearch;        // Batch files to perform test upon
};

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                 CORRECTION DIALOG
///
///   Encapsulates the object being corrected and the correction text
/// ////////////////////////////////////////////////////////////////////////////////////////

// Correction dialog data
//
struct CORRECTION_DIALOG_DATA
{
   HWND               hDialog;
   TCHAR*             szCorrection;
   RESULT_TYPE        eObjectType;
   SUGGESTION_RESULT  xResult;
};

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    DOCUMENT
///
///   Represent a document window and contains the necessary data to display it.
///      
/// ////////////////////////////////////////////////////////////////////////////////////////

// Identifies the result of attempting to close a document
//
enum  CLOSURE_TYPE  { DCT_DISCARD, DCT_SAVE_CLOSE, DCT_ABORT };

// Types of documents
//
enum  DOCUMENT_TYPE  { DT_SCRIPT, DT_LANGUAGE, DT_MEDIA, DT_MISSION, DT_PROJECT };

// A document window displayed within a tab control
//
struct  DOCUMENT
{
// Properties
   DOCUMENT_TYPE   eType;                 // Document type
   GAME_FILE*      pGameFile;             // Universal pointer to document data  [Data is always a ScriptFile, LanguageFile or ProjectFile]
   HWND            hWnd;                  // Document window
   BOOL            bModified,             // Indicates document has been modified since last save
                   bUntitled,             // Indicates document is new and has never been saved
                   bVirtual,              // Indicates document is virtual and cannot be saved
                   bClosing;              // Indicates document should be closed once saving is complete
   CONST TCHAR*    szFullPath;            // Current path of document. May not reflect name on disc if document has been renamed.
};

struct  PROJECT_DOCUMENT : DOCUMENT
{
   PROJECT_FILE*   pProjectFile;                  // Underlying project file
   TCHAR           szReplacementPath[MAX_PATH];   // FullPath of replacement project, used when switching projects
};

/* DERIVED TYPES:
-----------------

struct  SCRIPT_DOCUMENT : DOCUMENT
{
   SCRIPT_FILE*         pScriptFile;         // Data to display

   HWND                 hCodeEdit,           // Text Edit control window handle
                        hLabelsCombo,        // Label ComboBox control window handle
                        hVariablesCombo;     // Variables ComboBox control window handle

   HIMAGELIST           hImageList;          // Icons for the ComboBoxes

   LISTVIEW_LABEL_DATA  oLabelData;          // In-Place Label editing data for arguments property page
};

struct  LANGUAGE_DOCUMENT : DOCUMENT
{
   HWND           hPageList,           // Pages listview window handle
                  hStringList,         // Strings listview (in the strings dialog) window handle
                  hRichTextDialog,     // RichText dialog window handle
                  hRichEdit,           // RichText dialog's RichEdit control window handle
                  hToolBar;            // RichText dialog's Toolbar window handle
   HIMAGELIST     hImageList;          // ListView icons image list

   LISTVIEW_LABEL_DATA oLabelData;         // In-Place Label editing data
   UINT               iCurrentPageID;     // ID of the current page of the strings currently being displayed

   BOOL               bFormattingError;   // Indicates the current message contains a formatting error(s) [Affects how it is displayed]
   LANGUAGE_MESSAGE*  pCurrentMessage;    // Message currently being displayed/edited
   LANGUAGE_FILE*     pLanguageFile;      // Document data

   AVL_TREE*          pCurrentGameStringsByID;   // Strings in the currently selected page

   RichEditCallback*  pOleCallback;      // RichEdit OLE callback interface
};

struct  MEDIA_DOCUMENT : DOCUMENT
{
   HWND                 hGroupList,      // Media Groups listview
                        hItemList,       // Media Items listview
                        hItemDialog;     // Media player dialog
   HIMAGELIST           hImageList;      // ImageList used by both ListViews

   LISTVIEW_LABEL_DATA  oLabelData;      // In-Place Label editing data
   UINT                 iCurrentGroupID; // ID of the current group being displayed

   AVL_TREE*            pCurrentMediaItemsByID;    // Items in the currently selected group
};
*/

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                DOCUMENTS CONTROL DATA
///
///   Holds the active document and the original window procedure
///      
/// ////////////////////////////////////////////////////////////////////////////////////////

// Holds all the data for the documents control
//
struct  DOCUMENTS_DATA
{
   // Control
   HWND       hTabCtrl;               // Window handle
   WNDPROC    pfnBaseWindowProc;      // Base window proc of the tab control
   HFONT      hTitleFont;             // Font used for displaying titles on tabs
   HIMAGELIST hImageList;             // ImageList for Tab control

   // Documents
   LIST*      pDocumentList;          // Document list
   DOCUMENT*  pActiveDocument;        // Current document being displayed, or NULL if no document is displayed

   /// NEW
   UINT iOldIndex;
};



/// ////////////////////////////////////////////////////////////////////////////////////////
///                                      FILE DIALOG
///
///  A customized Open/Save File dialog that scans game scripts and displays their properties to the user
/// ////////////////////////////////////////////////////////////////////////////////////////

// Defines under which 'namespace' folder the current folder traversal should be placed
//
enum TRAVERSAL_PARENT { TP_NONE, TP_DESKTOP, TP_DOCUMENTS, TP_DRIVE };

// The types of file dialog available
//
enum  FILE_DIALOG_TYPE  { FDT_OPEN, 
                          FDT_SAVE };

// The two types of file search operations used by the dialog
//
enum  FILE_DIALOG_SEARCH     { FDS_SEARCH,         // Search current folder for new results
                               FDS_SORT     };     // Sort results from a previous search

// Progress report timer
#define      FILE_SEARCH_TIMER_ID     42

// FilDialog 'My Places' bar item
//
struct  JUMPBAR_ITEM
{
   TCHAR      *szText,       // Shell display name of the folder
              *szPath;       // Full path of the folder
   HWND        hCtrl;        // Handle of the button the item represents
   BOOL        bValid;       // Whether button is valid or not
   HIMAGELIST  hImageList;   // ImageList containing the icon
   UINT        iIconIndex;   // Icon's ImageList icon index
};


// FileDialog window data
//
struct  FILE_DIALOG_DATA
{
   // Dialog
   HWND              hDialog,                 // Dialog window handle
                     hListView,               // ListView window handle
                     hFileEdit,               // Filename edit window handle
                     hFilterCombo,            // File Filters combobox window handle
                     hLocationCombo,          // Folder locations combobox window handle
                     hSuggestionList,         // Auto-complete ListView
                     hToolbar;                // Folder controls toolbar (Parent, Refresh)
   JUMPBAR_ITEM*     pJumpBarItems[6];        // JumpBar item properties
   HIMAGELIST        hImageList;              // Handle to the system image list

   // File Search
   BOOL              bIsUpdating;             // TRUE when files are being added to the ListView
   UINT              iSortColumn;             // The current property (column) that files are sorted by
   BOOL              bSortAscending;          // Direction of sorting, above
   FILE_FILTER       eFilter;                 // The current filter of files being displayed
   FILE_DIALOG_TYPE  eType;                   // Whether the dialog is an 'open' or 'save' dialog
   AVL_TREE*         pSuggestionTree;         // Auto-complete list
   FILE_SEARCH*      pFileSearch;             // Search results for the current folder
   SEARCH_OPERATION* pSearchOperation;        // Exists only during a search operation

   // Input/Output
   TCHAR             szFileName[MAX_PATH];    // [OUTPUT] Full path of the file chosen by the user
   TCHAR             szFolder[MAX_PATH];      // [INPUT/OUTPUT] Initial/Final display directory

   /// NEW:
   LIST*             pOutputFileList;         // [OUTPUT] List of StoredDocuments containing selected files
   UINT              iOutputFiles;            // [OUTPUT] Number of files in output list
   LOADING_OPTIONS   oAdvanced;               // [OUTPUT] Advanced loading options
};


// File options dialog
// 
struct OPTIONS_DIALOG_DATA
{
   HWND             hDialog;            // Dialog

   BOOL             bScriptCalls,       // Open all script-calls on load
                    bRecursiveCalls,    // Recursively open all script-calls on load
                    bCompilerTest;      // Perform compiler test

   LOADING_OPTIONS  oOptions;           // Options
};

/// ////////////////////////////////////////////////////////////////////////////////////////
///                               LANGUAGE DOCUMENT
///
///   Data for the properties PropertySheet dialog
/// ////////////////////////////////////////////////////////////////////////////////////////

// Data for a language file document window
// 
struct  LANGUAGE_DOCUMENT : DOCUMENT
{
   HWND                 hWorkspace,          // Workspace
                        hPageList,           // Pages listview window handle
                        hStringList,         // Strings listview (in the strings dialog) window handle
                        hRichTextDialog,     // RichText dialog window handle
                        hRichEdit,           // RichText dialog's RichEdit control window handle
                        hToolBar;            // RichText dialog's Toolbar window handle

   //LISTVIEW_LABEL_DATA  oLabelData;         // In-Place Label editing data
   LVHITTESTINFO        oStringClick;
   GAME_PAGE*           pCurrentPage;       // Currently selected GamePage
   GAME_STRING*         pCurrentString;     // Currently selected GameString

   BOOL                 bFormattingError;   // Indicates the current message contains a formatting error(s) [Affects how it is displayed]
   LANGUAGE_MESSAGE*    pCurrentMessage;    // Message currently being displayed in the RichText dialog
   ERROR_QUEUE*         pFormatErrors;      // Formatting errors for current message
   LANGUAGE_FILE*       pLanguageFile;      // Document data

   AVL_TREE*            pPageStringsByID;   // Strings in the currently selected page

   RichEditCallback*    pOleCallback;       // [DIALOG] RichEdit callback interface
   AVL_TREE*            pButtonsByID;       // [DIALOG] Buttons in current message
};

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    MAIN WINDOW DATA
///
///   Data for the main window containing the window handles of all it's child windows
///      
/// ////////////////////////////////////////////////////////////////////////////////////////
struct DOCUMENTS_DATA;
struct PROPERTIES_DATA;
struct OUTPUT_DIALOG_DATA;
struct OPERATION_POOL;
struct PROJECT;

// Window data for the main window containing the handles of it's child windows
// 
struct  MAIN_WINDOW_DATA
{
   // Window handles
   HWND                 hMainWnd,                  // Main window
                        hWorkspace,                // Workspace control
                        hStatusBar,                // Status bar 
                        hSplashWindow,             // Splash window
                        hToolBar,                  // Toolbar
                        // Controls
                        hDocumentsTab,             // Documents control 
                        hSearchTabDlg,             // Tabbed search dialog
                        hOperationPoolCtrl,        // Operations pool control
                        // Dialogs
                        hFindTextDlg,              // Text search dialog
                        hOutputDlg,                // Compiler output dialog
                        hProjectDlg,               // Project dialog
                        hProgressDlg,              // Progress dialog [or NULL]
                        hPropertiesSheet,          // Properties PropertySheet [or NULL]
                        hRichTextDlg;              // Active RichText dialog [or NULL]
   HACCEL               hAccelerators;             // Accelerator Table
   CUSTOM_MENU*         pCustomMenu;               // Main menu

   // Window data
   OPERATION_POOL*      pOperationPool;         // Operations Pool
   MODAL_WINDOW_STACK*  pWindowStack;           // Modal Window stack
   OUTPUT_DIALOG_DATA*  pOutputDlgData;         // Output dialog window data
   PROPERTIES_DATA*     pPropertiesSheetData;   // Properties PropertySheet window data.     NB: This is here because of the issues surrounding storing of dialog data in a property sheet
   UINT                 iToolBarButtonCount,    // Number of Toolbar items (buttons plus separators)
                        iToolBarSeparatorCount; // Number of ToolBar separators
   
   // Project
   PROJECT_DOCUMENT*    pProject;        // Current project (if any)
};

// Closing states
//
enum  MAIN_WINDOW_STATE
{
   MWS_CLOSING,            // User closing window
   MWS_DOCUMENTS_CLOSED,   // All Documents have been closed
   MWS_PROJECT_CLOSED,     // Project has been closed
   MWS_REPORT_SUBMITTED    // BugReport has been submitted (if appropriate)
};

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                   MEDIA DOCUMENT
///
///   Data for the 'Media Document' audio/video browser
/// ////////////////////////////////////////////////////////////////////////////////////////

// Data for the media browser document
//
struct  MEDIA_DOCUMENT : DOCUMENT
{
   HWND                 hGroupList,      // Media Groups listview
                        hItemList,       // Media Items listview
                        hItemDialog;     // Media player dialog
   HIMAGELIST           hImageList;      // ImageList used by both ListViews

   LISTVIEW_LABEL_DATA  oLabelData;      // In-Place Label editing data
   UINT                 iCurrentGroupID; // ID of the current group being displayed

   AVL_TREE*            pCurrentMediaItemsByID;    // Items in the currently selected group
};

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                 MEDIA DIALOG DATA
///
///   Data for the 'Media Player' dialog
/// ////////////////////////////////////////////////////////////////////////////////////////

struct  MEDIA_PLAYER_DATA
{
   IGraphBuilder*          pGraph;                 // Filter graph manager interface
   IVMRWindowlessControl*  pVideoCtrl;             // Windowless rendering interface
   IMediaControl*          pController;            // Filter graph control interface
   IMediaSeeking*          pSeeking;               // Filter graph seeking interface
   IMediaEventEx*          pEvents;                // DirectShow events interface

   MEDIA_ITEM*             pCurrentItem;           // Currently loaded/playing MediaItem
   REFERENCE_TIME          timeDuration,           // Length of current item (in reference time)
                           timeStart;              // Start offset of current item (in reference time)
   BOOL                    bPlaying;               // Whether the item is playing or not.

   HIMAGELIST              hButtonIcons;           // Player dialog button icons
   HBRUSH                  hPlayerBackground;      // Brush for painting the player dialog background
   WNDPROC                 wndprocProgressCtrl;    // Player dialog progress bar original window procedure
};

/// ////////////////////////////////////////////////////////////////////////////////////////
///                               MESSAGE DIALOG DATA
///
///   Data for the MessageBox replacement dialog - MessageDialog
/// ////////////////////////////////////////////////////////////////////////////////////////

// Defines the available button and icon flags
//
enum MESSAGE_DIALOG_FLAG { MDF_OK          = 0x01,    // Just an OK button
                           MDF_OKCANCEL    = 0x02,    // OK and CANCEL button
                           MDF_YESNO       = 0x04,    // YES and NO button
                           MDF_YESNOCANCEL = 0x08,    // YES, NO and CANCEL
                           MDF_WARNING     = 0x10,    // Warning icon
                           MDF_ERROR       = 0x20,    // Error icon
                           MDF_QUESTION    = 0x30,    // Question icon
                           MDF_INFORMATION = 0x40 };  // Information icon

// Message dialog data
//
struct MESSAGE_DIALOG_DATA
{
   // [COMMON]
   HWND                 hDialog,          //           Dialog window handle
                        hToolbar;         //           Toolbar window handle
   UINT                 iAttributes,      //           Button and icon flags
                        iStackIndex;      // [ERROR]   Zero-based LOGICAL index of the current ErrorMessage being displayed
   CONST TCHAR         *szIcon,           //           Resource ID of dialog icon
                       *szTitle;          //           String ID of dialog title
   TCHAR*               szSimpleMessage;  // [MESSAGE] Assembled message text

   /// NEW:
   CONST ERROR_STACK*   pErrorStack;      // [ERROR]   Current error stack
   CONST ERROR_QUEUE*   pErrorQueue;      // [QUEUE]   Error Queue
   CONST ERROR_MESSAGE* pErrorMessage;    // [ERROR]   Current message within the current error stack
};

// Convenience macros for extracting button/icon flags alone
//
#define    extractButtonFlags(iFlags)     (iFlags INCLUDES 0x0000000F)
#define    extractIconFlags(iFlags)       (iFlags INCLUDES 0x000000F0)

/// ////////////////////////////////////////////////////////////////////////////////////////
///                               NEW DOCUMENT DIALOG
///
///   Data for the 'New Document' dialog
/// ////////////////////////////////////////////////////////////////////////////////////////

enum  NEW_DOCUMENT_TYPE    { NDT_SCRIPT, NDT_LANGUAGE, NDT_MISSION, NDT_PROJECT };

struct NEW_DOCUMENT_DATA
{
   HWND               hDialog,            // Dialog window handle
                      hListView;          // ListView window handle

   NEW_DOCUMENT_TYPE  eNewDocumentType;   // [OUTPUT] Document type
   TCHAR*             szFullPath;         // [OUTPUT] Document file path
   
};

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                  OPERATION POOL
///
///   Provides tracking and encapsulation of the currently executable threaded operations
///      
/// ////////////////////////////////////////////////////////////////////////////////////////

// Window data for the operation pool control
//
struct OPERATION_POOL
{
   LIST*    pOperationList;           // List of currently executing operations
   HWND     hWnd;                     // Window handle
   WNDPROC  wndprocProgressBar;       // Original window procedure of the base progress bar control

   BOOL     bBatchOperation;          // Indicates a batch operation is in progress, overriding the normal progress reporting mechanism
   UINT     iBatchProgress;           // Batch operation progress       0 <= progress < 10,000
};

/// ////////////////////////////////////////////////////////////////////////////////////////
///                               OUTPUT DIALOG
///
///   Data for the 'Output' dialog
/// ////////////////////////////////////////////////////////////////////////////////////////

enum OUTPUT_DIALOG_ICON    { ODI_OPERATION, ODI_INFORMATION, ODI_WARNING, ODI_ERROR, ODI_SUCCESS, ODI_FAILURE };

// Data for each item in the OutputDialog
//
struct OUTPUT_DIALOG_ITEM
{
   TCHAR*         szText;           //             Item text
   UINT           iIconIndex;       //             Item icon
   UINT           iIndent,          //             Indentation level
                  iOperationID,     // [OPERATION] Item operation
                  iOperationIndex;  // [OPERATION] Index within operation messages
   OPERATION_TYPE eOperationType;   // [OPERATION] Operation type
   ERROR_STACK*   pError;           // [ERROR]     Error associated with the item
};

// Dialog data for the OutputDialog
//
struct OUTPUT_DIALOG_DATA
{
   // Dialog
   HWND          hDialog,              // Dialog window handle
                 hListView;            // ListView window handle

   // Data
   AVL_TREE*     pItemTreeByOperation; // Item data
   UINT          iLastOperationID;     // Last operation ID
};

/// ////////////////////////////////////////////////////////////////////////////////////////
///                              OPERATION_PROGRESS DIALOG DATA
///
///   Data for the progress dialog
/// ////////////////////////////////////////////////////////////////////////////////////////

// Window data for the progress dialog
//
struct PROGRESS_DATA
{
   OPERATION_POOL*    pOperationPool;      // Convenience pointer to the OperationPool
   UINT               iCurrentStageID;     // Resource ID of the description of the currently executing operation
   HICON              hIcon;               // Handle for the dialog graphic
   HWND               hDialog;             // Window handle of the progress dialog
};

/// ////////////////////////////////////////////////////////////////////////////////////////
///                               PROPERTIES DIALOG
///
///   Data for the properties PropertySheet dialog
/// ////////////////////////////////////////////////////////////////////////////////////////
struct SCRIPT_DOCUMENT;

// Defines the IDs of available 'Document Properties' PropertyPages
//
enum PROPERTY_PAGE { PP_SCRIPT_GENERAL,   PP_SCRIPT_ARGUMENTS, PP_SCRIPT_DEPENDENCIES, PP_SCRIPT_VARIABLES, PP_SCRIPT_STRINGS,
                     PP_LANGUAGE_GENERAL, PP_LANGUAGE_COLUMNS, PP_LANGUAGE_BUTTON, PP_LANGUAGE_SPECIAL,
                     PP_NO_DOCUMENT };

// Identifies the type of pages currently being displayed
//
enum PROPERTY_DIALOG_TYPE  { PDT_NO_DOCUMENT,            // The 'no document' placeholder page
                             PDT_SCRIPT_DOCUMENT,        // The script document pages
                             PDT_LANGUAGE_DOCUMENT };    // The language document pages

// Defines the common actions required of every property page
//
enum PROPERTY_PAGE_TASK  { PPT_INITIALISE,      // Initalise controls
                           PPT_SHOW,            // Fill controls with data
                           PPT_HIDE,            // Save control values to data
                           PPT_TOOLTIPS };      // Create tooltips

// Used to define attributes of a PropertyPage for 'Document Properties' and the 'Preferences' property sheets.
//
struct PROPERTY_PAGE_DEFINITION
{
   DLGPROC       dlgProc;               // Window procedure
   CONST TCHAR  *szTemplateResource,   // Dialog template
                *szIconResource;       // Icon name
   UINT          iTitleResource;        // StringID of the page title
};

// Define the total number of available property pages
#define     PROPERTY_PAGE_COUNT          10

// Dialog data associated with the document properties PropertySheet
//
struct PROPERTIES_DATA
{
   // Sheet data
   PROPERTY_DIALOG_TYPE  eType;                             // Type of pages currently being displayed
   HWND                  hSheetDlg,                         // Window handle for the properties sheet
                         hTooltips[PROPERTY_PAGE_COUNT];    // Tooltip window handles
   BOOL                  bRefreshing;                       // TRUE while controls are being refreshed by the dialog
   
   DLGPROC               dlgprocSheetBase;                  // Base dialog procedure for the sub-classed properties sheet

   // Script Documents
   SCRIPT_DOCUMENT*      pScriptDocument;                   // Document for script pages
   AVL_TREE             *pScriptDependecies,                // Holds ScriptDependencies for the current script document
                        *pStringDependencies,               // Holds StringDependencies for the current script document
                        *pVariableDependencies;             // Holds variableDependencies for the current script document

   // Language Documents
   LANGUAGE_DOCUMENT*    pLanguageDocument;                 // Document for language pages
   HIMAGELIST            hColumnIcons;                      // Large icons for the custom radio buttons on 'Columns' page
};

/// ////////////////////////////////////////////////////////////////////////////////////////
///                             PROJECT DIALOG DATA
///
///   Data for the projects dialog
/// ////////////////////////////////////////////////////////////////////////////////////////


// Project Dialog Data
struct PROJECT_DIALOG_DATA
{
   HWND        hDialog,          // Project dialog
               hTreeView;        // TreeView

   HTREEITEM   hRoot,            // TreeView root node
               hFolderNode[3];   // TreeView folder node for script files, language files and mission files
};

/// ////////////////////////////////////////////////////////////////////////////////////////
///                              RESULTS DIALOG DATA
///
///   Dialog data for the 'search results' dialog
/// ////////////////////////////////////////////////////////////////////////////////////////


struct RESULTS_DIALOG_DATA
{
// Dialog
   RESULT_TYPE   eType;                    // Type of dialog to display: Command list, Game objects, Script object
   HWND          hDialog,                  // Dialog window handle
                 hListView,
                 hFilterCombo,
                 hTooltip,                 // Tooltip control for the dialog
                 hRichTooltip;             // ListView Rich Tooltips

// Results data
   AVL_TREE*     pResultsTree;             // Current search results  [COPY TREE]
   GAME_VERSION  eGameVersion;             // GameVersion of the current document
   UINT          iFirstGroupResourceID,    // Resource ID of the first group name
                 iFirstGroupID,            // ID of the first group
                 iLastGroupID;             // Number of groups
};


/// ////////////////////////////////////////////////////////////////////////////////////////
///                              SEARCH DIALOG DATA
///
///   Dialog data for the Search Dialog  (which hosts the Results dialogs)
/// ////////////////////////////////////////////////////////////////////////////////////////

struct SEARCH_DIALOG_DATA
{
   HWND          hDialog,            // Dialog handle
                 hTabCtrl,           // Tab control
                 hChildren[3];       // ResultsDialog child dialogs
                                     //  -> Accessed via RT_COMMANDS, RT_GAME_OBJECTS and RT_SCRIPT_OBJECTS
   RESULT_TYPE   eActiveDialog;      // ID of the currently active tab/dialog
   HIMAGELIST    hImageList;         // Tab control ImageList 
   HFONT         hTabFont;           // Bold font for the tab titles
};


/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    SCRIPT DOCUMENT
///
///   Window data for the script document dialog
/// ////////////////////////////////////////////////////////////////////////////////////////

// Data for a Script file document window
// 
struct  SCRIPT_DOCUMENT : DOCUMENT
{
   SCRIPT_FILE*         pScriptFile;         // Data to display

   HWND                 hWorkspace,          // Workspace
                        hCodeEdit,           // Text Edit control window handle
                        hLabelsCombo,        // Label ComboBox control window handle
                        hVariablesCombo;     // Variables ComboBox control window handle

   LISTVIEW_LABEL_DATA  oLabelData;          // In-Place Label editing data for arguments property page
};

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    SPLASH WINDOW DATA
///
///   Window data for the splash window
/// ////////////////////////////////////////////////////////////////////////////////////////

struct SPLASH_WINDOW_DATA
{
   HWND      hWnd;

   HBITMAP   hLogoBitmap;            // Splash window bitmap
   HPEN      hProgressPen;           // Green pen used for drawing the progress bar
   HFONT     hProgressFont;          // Progress stage font
};



