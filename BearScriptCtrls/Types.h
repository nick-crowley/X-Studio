//
// Types.h : Declaration of various structures, enumerations and constants
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#pragma once

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                CONSTANTS / GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////

// Identifies a direction
//
enum DIRECTION { HORIZONTAL, VERTICAL };

// ////////////////////////////////////////////////////////////////////////////////////////
//                                   CODE EDIT DATA (NEW)
//
//   Contains the text and formatting data for the new CodeEdit control
// ////////////////////////////////////////////////////////////////////////////////////////
struct CUSTOM_TOOLTIP;

// Defines available FindText options 
//
enum CODE_EDIT_SEARCH_FLAG  { CSF_FROM_TOP = 1, CSF_FROM_CARET = 2, CSF_CASE_SENSITIVE = 4, CSF_SEARCH = 8, CSF_REPLACE = 16 };

// Defines a Find & Replace operation
//
struct CODE_EDIT_SEARCH
{
   TCHAR  *szSearch,
          *szReplace;
   UINT    eFlags;
};

// Defines the position of a character within the CodeEdit
//
struct CODE_EDIT_LOCATION
{
   UINT  iIndex,        // Zero based character index
         iLine;         // Zero based line number
};

// Defines the position of the caret in a CodeEdit
//
struct CODE_EDIT_CARET
{
   CODE_EDIT_LOCATION  oLocation;            // Location of the caret
   LIST_ITEM*          pCharacterItem;       // [OPTIONAL] ListItem containing the CharacterData of the character at the caret.
                                             //             If the caret is positioned at the end of a line, this will be NULL.
};

// Defines the presence and the origin of the current text selection
//
struct CODE_EDIT_SELECTION
{
   BOOL                bExists;        // Whether a selection currently exists or not
   CODE_EDIT_LOCATION  oOrigin;        // Location of the selection origin.  The other end of the selection is always the caret location
};

// Defines each character in a CodeEdit line of text
//
struct CODE_EDIT_CHARACTER
{
   TCHAR         chCharacter;       // Character being represented
   BOOL          bSelected;         // Selection state of character
   
};

// Defines a line of CodeEdit text
//
struct CODE_EDIT_LINE
{
   // Basic properties
   UINT           iCommandID;        // Command ID
   BOOL           bInterrupt;        // Whether the line contains an interruptable command
   CONDITIONAL_ID eConditional;      // Conditional phrase
   UINT           iIndent;           // Line Indentation
   ERROR_TYPE     eSeverity;         // Whether the error(s) associated with the line represent a warning or an error

   // Internal Data
   LIST*          pCharacterList;    // Line text stored as a list of characters
   COMMAND*       pCommand;          // COMMAND generated from the current contents of the line
   ERROR_QUEUE*   pErrorQueue;       // [OPTIONAL] Errors associated with this line by translation or generation. This is not the same as the error queue in 
                                     //             -> the COMMAND, which is ignored because it is updated on every key press or change
};

// Identifies the location of a label within the CodeEdit
//
struct CODE_EDIT_LABEL
{
   TCHAR    szName[128];      // Label name
   UINT     iLineNumber;      // Zero-based line number
};

// Undo item types
enum UNDO_ITEM_TYPE  { UIT_ADDITION, UIT_DELETION };

// Represents an item in the undo/redo stacks
//
struct CODE_EDIT_UNDO_ITEM
{
   TCHAR*              szText;      // Text of item being added/removed
   CODE_EDIT_LOCATION  oStart,      // Location where item starts
                       oFinish;     // Location where item ends
   UNDO_ITEM_TYPE      eType;       // Whether item was added or removed
};

// Provides undo/redo capabilities by storing recent changes to the text
//
struct CODE_EDIT_UNDO
{
   CODE_EDIT_UNDO_ITEM  oBuffer;                // Properties for the working buffer
   TCHAR                szBuffer[LINE_LENGTH];  // Working buffer for characters currently being typed/removed
   STACK               *pUndoStack,             // Undo stack
                       *pRedoStack;             // Redo stack
};

// Represents the auto-complete ListView
//
struct CODE_EDIT_SUGGESTION
{
   HWND                 hCtrl;            // Window handle of the Auto-Complete ListView
   SUGGESTION_TYPE      eType;            // Current suggestion type
   CODE_EDIT_LOCATION   ptOrigin;         // Identifies the first character of the suggestion

   AVL_TREE            *pResultsTree;     // Current results
};

// Holds all the data for a CodeEdit
//
struct CODE_EDIT_DATA
{
   // Main data
   HWND                 hWnd;              // Window handle of the CodeEdit control
   LIST*                pLineList;         // Control text stored by line

   // Display/Scrolling data
   SIZE                 siCharacterSize,   // Defines Average Character Size (cx) and Line Height (cy)
                        siPageSize;        // Defines the number of characters (cx) and lines (cy) in a page, given the current font and size of the CodeEdit
   CODE_EDIT_LOCATION   ptFirstCharacter,  // Defines the first visible character in average character widths (x) and lines (y) respectively
                        ptLastCharacter;   // Defines the scroll limits in in average character widths (x) and lines (y) respectively 
   UINT                 iLongestLine;      // Length in pixels of the longest line in the CodeEdit

   // Caret and Selection tracking
   CODE_EDIT_CARET      oCaret;            // Current position of the caret
   CODE_EDIT_SELECTION  oSelection;        // Contains the origin of the current text selection

   // Event generation
   BOOL                 bEventsEnabled;    // Whether control sends events to the parent or not (Allows events to be disabled during loading)

   // External data
   SCRIPT_FILE*         pScriptFile;       // Datastructure representing the script
   CONST PREFERENCES*   pPreferences;      // Current application preferences, defines the colour scheme and indentation options
   CONST COLOUR_SCHEME* pColourScheme;     // Current colour scheme, defined by preferences

   // Suggestions
   CODE_EDIT_SUGGESTION oSuggestion;       // Auto-Complete Suggestions
   
   // Tooltips
   HWND                 hTooltip;          // Tooltip window handle
   BOOL                 bTooltipVisible;   // Tooltip display state
   CUSTOM_TOOLTIP*      pTooltipData;      // CustomTooltip display data
   POINT                ptCursor;          // Last Cursor position - used to block spurious WM_MOUSEMOVE messages

   // Undo/Redo
   CODE_EDIT_UNDO       oUndo;             // Undo/Redo stacks and buffer

   // Scope
   CODE_EDIT_LABEL*     pCurrentScope;     // Current scope (last label)
};


// Defines a contiguous block of characters with the same graphical properties in a CodeEdit
//
struct CODE_FRAGMENT
{
   // Input Data
   CONST CODE_EDIT_LINE*  pLineData;               // LineData for the line being parsed
   CONST COLOUR_SCHEME*   pColourScheme;           // Current colour scheme
   CONST SCRIPT_FILE*     pScriptFile;             // ScriptFile of the document, used for identifying arguments
   LIST_ITEM*             pCharacterIterator;      // CharacterData iterator for the LineData
   CODEOBJECT*            pCodeObject;             // CodeObject to parse the input text
   TCHAR*                 szSource;                // Line text converted into an array

   // Fragment properties
   TCHAR*                 szText;                  // Current fragment text
                                                   ///   NB: Only the first 'iLength' characters are valid, the remainder belong to the next fragment
   UINT                   iLength;                 // Length of the current fragment text
   COLORREF               clColour;                // Colour of the current fragment, based on current preferences
   BOOL                   bSelected;               // Whether the fragment is selected or not
};


// Flags used for defining various drawing rectangles
//
enum CODE_EDIT_DISPLAY_RECTANGLE   { CDR_LINE_NUMBER_INDICATOR,   // Column containing the line number
                                     CDR_INTERRUPT_INDICATOR,     // Gap containing nothing or the interrupt indicator
                                     CDR_INDENTATION_GAP,         // Indentation gap according to the surrounding script commands
                                     CDR_LINE_TEXT,               // Script command text
                                     CDR_LINE_GAP,                // Gap between the final character and the right edge of the client area
                                     CDR_PAGE_GAP,                // [HIT-TEST] Gap beneath last line of code
                                     CDR_NONE };                  // [HIT-TEST] Not within CodeEdit

// Types of squiggle
//
enum CODE_EDIT_SQUIGGLE            { CST_ERROR, CST_WARNING, CST_VIRTUAL };

// Convenience descriptor for HitTest results
//
typedef CODE_EDIT_DISPLAY_RECTANGLE  CODE_EDIT_AREA;

// Flags used to identify when text data has changed
//
enum CODE_EDIT_CHANGE_FLAG { CCF_TEXT_CHANGED         = 1,     // The text of a line has changed
                             CCF_SELECTION_CHANGED    = 2,     // The selection of a character(s) in a line has changed
                             CCF_ERROR_CHANGED        = 4,     // The error queue on a line has changed
                             CCF_INDENTATION_CHANGED  = 8,     // The indentation of a line has changed
                             CCF_LINES_CHANGED        = 16,    // The number of lines has changed
                             CCF_SCOPE_CHANGED        = 32 };  // The current scope has changed


// Classifies lines based on their affect on code indentation
//
enum LINE_CLASS   { LC_COMMAND,        // Any command not prefaced with a conditional
                    LC_COMMENT_NOP,    // Comment, command comment or an empty line
                    LC_IF_WHILE,       // IF, IF-NOT, WHILE or WHILE-NOT
                    LC_SKIP_IF,        // SKIP-IF or SKIP-IF-NOT
                    LC_ELSE_IF,        // ELSE, ELSE-IF or ELSE-IF-NOT
                    LC_END };          // END

// Define Line Feed ('\n') convenience constant
#define            VK_LINE_FEED       0x0A

// ////////////////////////////////////////////////////////////////////////////////////////
//                                   CODE EDIT ITERATOR
//
//   An iterator for traversing the text within the CodeEdit
// ////////////////////////////////////////////////////////////////////////////////////////

// Defines the two types of iterator available
//
enum CODE_EDIT_ITERATOR_TYPE  { CIT_SINGLE_LINE,      // Traverses all the characters in a line of text
                                CIT_MULTI_LINE   };   // Traverses between any two locations within the text. They need not be on different lines.

// Contains the iterator data
//
struct CODE_EDIT_ITERATOR
{
   // Internal
   CODE_EDIT_ITERATOR_TYPE  eType;                 //              Whether iterator is single or multi-line
   CONST CODE_EDIT_DATA    *pWindowData;           // [MULTI-LINE] Window data
   CODE_EDIT_LOCATION       oStart,                // [MULTI-LINE] Location of first character
                            oFinish;               // [MULTI-LINE] Location following last character
   LIST_ITEM               *pCharacterIterator,    //              CharacterData iterator
                           *pLineIterator;         // [MULTI-LINE] LineData iterator
   //BOOL                     bFirstCall;
                         
   // Current position
   TCHAR                    chCharacter;           //              Current character
   //CODE_EDIT_CHARACTER     *pCharacter;            //              CharacterData for the current character
   //CONST CODE_EDIT_LINE    *pLine;                 //              LineData for the current line
   CODE_EDIT_LOCATION       oLocation;             //              Location of the current character
                                                   ///               NB: Only the character index is valid in a single line iterator
};

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                  CUSTOM DRAWING
///
///   Custom drawing for buttons, comboboxes, listviews and menus
///      
/// ////////////////////////////////////////////////////////////////////////////////////////

// Holds the data necessary for drawing a custom icon button
// 
struct CUSTOM_BUTTON
{
   IMAGE_TREE_SIZE  eImageSize;       // ImageTree size to use for icons
   UINT             iIconIndex;       // Zero based index of the icon within the imagelist
   BOOL             bRightAligned;    // Whether button is displayed to the right of the text, instead of the left
};

// Represents data necessary to draw a custom ComboBox item
//
struct CUSTOM_COMBO_ITEM
{
   TCHAR           *szMainText,     // Text displayed in black on the left hand side
                   *szAuxText;      // Tetx displayed in grey on the right hand side
   CONST TCHAR     *szIconID;       // Resource ID of icon
   UINT             iIndent;        // Indentation 

   LPARAM           lParam;         // [OPTIONAL] Custom item data 
   HFONT            hFont;          // [OPTIONAL] Custom item font
   TCHAR            szFont[LF_FACESIZE];
   COLORREF         clColour;       // [OPTIONAL] Custom text colour
   HIMAGELIST       hImageList;     // [OPTIONAL] Custom ImageList
   UINT             iIconIndex;     // [OPTIONAL] Custom ImageList icon index
};

// Defines item comparitor function
//
typedef  BOOL  (*COMPARE_COMBO_PROC)(UINT, COMPAREITEMSTRUCT*);

// Defines a custom menu
//
struct CUSTOM_MENU
{
   HMENU       hMenuBar,         // MenuBar containing the popup
               hPopup;           // Popup menu
};

// Custom menu item data required for drawing
//
struct CUSTOM_MENU_ITEM
{
   TCHAR*      szText;           // Item text
   UINT        iItemID,          // Command ID of the item
               iIconIndex;       // ImageTree Icon index
   BOOL        bIsPopup,         // Whether item is a popup menu item
               bIsSeparator,     // Whether item is a separator
               bIsHeading,       // Whether item is a top level menu heading
               bDefault;         // Whether item should be displayed in bold
};


// RichText custom Tooltip data
//
struct CUSTOM_TOOLTIP
{
   RESULT_TYPE        eType;        // Object type
   SUGGESTION_RESULT  xObject;      // Object
};


// Convenience object for initialising a Report mode ListView
//
struct LISTVIEW_COLUMNS
{
   UINT     iColumnCount,           // Number of columns
            iColumnResourceID,      // Resource ID of the string defining first column name  (Strings must have sequential IDs after that)
            iColumnWidths[5];       // Width of columns, in pixels
};

// Custom ListView item states
#define       LVIS_ENABLED           0
#define       LVIS_DISABLED          1

// Custom ListView item flags
#define       LVIF_PLAINTEXT         0x00000000      // Plaintext (default)
#define       LVIF_RICHTEXT          0x00000020      // Indicates lParam contains a RichText object
#define       LVIF_DESTROYTEXT       0x00000040      // Indicates the custom ListView should destroy the RichText object once it has been drawn
#define       LVIF_COLOUR_TEXT       0x00000080      // Indicates lParam contains a COLORREF specifying custom text colour

// Custom ListView notification ID
#define       LVIP_CUSTOM_DRAW       0x12345678      // Indiciates LVN_GETDISPINFO has been sent by a CustomDrawn ListView, not the system

// Custom Static text alignment styles  (Existing styles corrupted by SS_OWNERDRAW)
#define       CSS_CENTRE             0x00000400
#define       CSS_RIGHT              0x00000800

// Custom ListView Custom Tooltip message ID
#define       LVGIT_CUSTOM           0x12345678

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                   GROUPED LIST VIEW
///
///  A virtual ListView that displays grouped data with fancy XP style headings
/// ////////////////////////////////////////////////////////////////////////////////////////

// Defines the size and name of each group
// 
struct  LISTVIEW_GROUP
{
   TCHAR*  szName;      // Group name
   UINT    iCount,      // Number of items in the group
           iID;         // Application defined ID for the group
};

// Associated with a Grouped ListView window, it contains the group data
//
struct  GROUPED_LISTVIEW
{
   HWND      hCtrl;                 // ListView
   LIST*     pGroupList;            // Group data
   WNDPROC   pfnBaseWindowProc;     // No longer used
   HFONT     hHeaderFont;           // Font used to display header text
};

// Identifies items as either a real item or part of a header
//
enum  LISTVIEW_INDEX_TYPE  { GLVIT_BLANK, GLVIT_GROUPHEADER, GLVIT_ITEM };


// Holds the position/properties of a Grouped ListView item.
//
struct  LISTVIEW_ITEM
{
   LISTVIEW_INDEX_TYPE  eType;               // Index type - either an item or part of a header
   UINT                 iLogicalIndex,       // Logical index  (Item number)
                        iPhysicalIndex;      // Physical index  (ListView item index)
   LISTVIEW_GROUP      *pGroup;              // Group to which item belongs
};

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                 LABEL QUEUE
///
///   Stores the name and locations of labels in a ScriptDocument for display in the Labels ComboBox
/// ////////////////////////////////////////////////////////////////////////////////////////

// Whether a label is actually a procedure or not
//
enum LABEL_TYPE  { LT_LABEL, LT_PROCEDURE };

// Defines a script document label -- contains the name and position of the label
//
struct  LABEL_QUEUE_ITEM
{
   TCHAR              szName[128];    // Name of the label
   UINT               iLineNumber;    // Zero-based line number of the label
   LABEL_TYPE         eType;          // Label type
};

// Queue of label items
//
struct  LABEL_QUEUE
{
   INT                iCount;          // Number of labels in the queue
   LABEL_QUEUE_ITEM*  eData[64];       // Queue data
};

/// ////////////////////////////////////////////////////////////////////////////////////////
///                               LISTVIEW LABEL EDITING
///
///   Custom ListView item editing allowing for sub-items to be edited as well as items and
///    optionally to use a combo box instead of an edit control
/// ////////////////////////////////////////////////////////////////////////////////////////

// Defines the type of control used to edit a ListView item
//
enum  LISTVIEW_LABEL  { LVLT_EDIT = 0xffff0001, LVLT_COMBOBOX = 0xffff0002 };

// Stores the necessary properties to enable ListView in-place item editing
//
struct  LISTVIEW_LABEL_DATA
{
   HWND                 hParent;      // Parent window containing the ListView

   HWND                 hCtrl;        // Control used for item editing
   LISTVIEW_LABEL  eCtrlType;    // Type of control used for item editing
   WNDPROC              wndprocCtrl;  // Original window procedure of the control used for item editing

   INT                  iItem,        // Zero-based Item being edited
                        iSubItem;     // 1-based Sub-item being edited. Zero indicates the main item.

   LPARAM               pParameter;   // [Optional] Parameter data used for passing data to the label control
};


// Notification data used by the modified label notifications - LVN_BEGINLABELEDIT and LVN_ENDLABELEDIT
//
struct  LISTVIEW_LABEL_NOTIFICATION
{
   NMHDR                oMessage;     // Message header containing the ListView notification, window handle and ID.

   HWND                 hCtrl;        // Control sending the nofication
   LISTVIEW_LABEL  eCtrlType;    // Type of control sending the notification

   INT                  iItem,        // Zero-based Item being edited
                        iSubItem;     // 1-based Sub-item being edited. Zero indicates the main item.

   TCHAR*               szNewText;    // New text the user has entered, or NULL if user pressed escape.
   INT                  iSelection;   // New combobox selection of -1 if user pressed escape.
};

// Convenience description matching other ListView notification structures
typedef    LISTVIEW_LABEL_NOTIFICATION    NMLVLABELINFO;
#define   LVCN_BEGINLABELEDIT            LVN_BEGINLABELEDIT
#define   LVCN_ENDLABELEDIT              LVN_ENDLABELEDIT

// ID of the control used for in-place listview item editing
#define   IDC_LISTVIEW_LABEL_CTRL        100 

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    RICH TOOLTIP DATA
///
///   
/// ////////////////////////////////////////////////////////////////////////////////////////

struct RICH_TOOLTIP_DATA
{
   CONST RICH_TEXT  *szTitle,    // Title text for the RichTooltip
                    *szText;     // The main block of text used in a RichTooltip
   UINT              iTipID,     // User defined ID uniquely identifying the content within the tip
                     iLinkID;    // Unique ID used to generate the link displayed at the bottom of the tip

   HWND              hParent,    // Window handle of the RichTooltip's parent window
                     hLinkCtrl;  // Internet link control

   HFONT             hTitleFont, // Font used to display the Tooltip title
                     hTextFont;  // Font used to display the tooltip text

   BOOL              bSelfDestructMode; // 
};



/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    WORKSPACE
///
///   Represents a pluggable scaling window system
/// ////////////////////////////////////////////////////////////////////////////////////////

// Identifies how to position a new pane compared to an existing pane
//
enum SIDE { LEFT, RIGHT, TOP, BOTTOM };
  
// Identifies a pane type, used for debugging only
//
enum PANE_TYPE { PT_WINDOW, PT_SPLITTER };

// Defines extended pane properties
//
struct PANE_PROPERTIES
{
   BOOL      bFixed;            // Whether border is fixed or adjustable
   UINT      iInitialSize;      // Initial size in pixels
   FLOAT     fInitialRatio;     // Initial ratio
   RECT      rcBorder;          // Internal border
};

// Identifies a rectangle within the workspace tree, may be empty (a splitter) or contain a window
//
struct PANE
{
// Common
   PANE_TYPE eType;         // Whether window or splitter [DEBUGGING]
   RECT      rcPane;        // Pane rectangle (in workspace client co-ordinates)
   PANE*     pParent;       // Parent pane (NULL if root)
   BOOL      bNew;          // Whether window has just been inserted, and not moved for the first time

// Window Panes
   HWND      hWnd;          // Window handle
   TCHAR     szName[64];    // Window title [DEBUGGING]
   RECT      rcBorder;      // Internal border (if any)
   
// Splitter Panes
   BOOL      bFixed;        // Whether splitter is adjustable or fixed
   SIDE      eFixedSide;    // Splitter pane which is fixed (if any)
   UINT      iFixedSize;    // Size of the fixed splitter pane (if any) in pixels
   FLOAT     fRatio;        // Ratio of left:right
   DIRECTION eSplit;        // Arrangement of windows (not orientation of splitter bar)
   PANE*     pChildren[2];  // Child panes, if any
};


// Holds the data required to perform a search of the pane tree 
//
struct PANE_SEARCH
{
   UINT   iCount,          // Number of windows to search for
          iMatches;        // Number of windows found
   HWND   hWindows[3];     // Handles of the windows to search for

   PANE*  pOutput;         // Search result
};


// Workspace window
struct WORKSPACE_DATA
{
   HWND     hWnd,           // Workspace control
            hBaseWnd;       // Identifies the base window
   HBRUSH   hBackground;    // System colour index for the background (splitter bars)

   RECT     rcWorkspace;    // Workspace rectangle in parent client co-ordinates
   PANE    *pBasePane,      // Pane tree
           *pDragPane;      // Splitter pane currently being dragged, if any
};

