//
// Types.h : Declaration of various structures, enumerations and constants
//

#pragma once

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    GLOBAL CONSTANTS
/// ////////////////////////////////////////////////////////////////////////////////////////

// Define maximum string lengths
#define            LINE_LENGTH               512          // Maximum length of a line (in chars)
#define            HASH_LENGTH               384          // Maximum length of a hash (in chars)
#define            COMPONENT_LENGTH          256          // Max length of variables, enumerations and constants
#define            MAX_STRING           4096         // Max length for a game string

// Define various totals
#define            GAME_TEXT_COLOURS         10           // Number of game text colours
#define            INTERFACE_COLOURS         13           // Number of interface colours
#define            CODEOBJECT_CLASSES        13           // Number of CodeObject classes
#define            LANGUAGES                 7            // Number of Languages
#define            PREFERENCES_PAGES         5            // Number of preferences dialog pages
#define            TUTORIAL_COUNT            (TW_SCRIPT_EDITING + 1)      // Number of tutorial dialogs   (Plus 1 because timer IDs must be 1-based)

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    TYPE DEFINITIONS
/// ////////////////////////////////////////////////////////////////////////////////////////

typedef             QUEUE                   ERROR_QUEUE;

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    APPLICATION
///
///   Holds program states and properties that are not preserved when the program closes
///      
/// ////////////////////////////////////////////////////////////////////////////////////////
struct CALL_STACK_LIST;
struct CONSOLE_DATA;
struct GAME_DATA;
struct FILE_SYSTEM;
struct IMAGE_TREE;

// Indicates whether game data is currently loaded or not
//
enum APPLICATION_STATE { AS_FIRST_RUN,                // First time run  (No game data either)
                         AS_NO_GAME_DATA,             // Game data is not loaded
                         AS_GAME_DATA_LOADED   };     // Game data is loaded

// Defines the types of critical errors that are tracked
//
enum APPLICATION_ERROR  { AE_ACCESS_VIOLATION, AE_DEBUG_ASSERTION, AE_TERMINATION, AE_EXCEPTION, AE_LOAD_GAME_DATA, AE_LOAD_SCRIPT };

// Defines the application registry sub-keys
//
enum APPLICATION_REGISTRY_KEY  { ARK_SESSION, ARK_CURRENT_SCHEME, ARK_SCHEME_LIST, ARK_RECENT_FILES };

// Defines available ImageTree icon sizes
//
enum IMAGE_TREE_SIZE  { ITS_SMALL = 16, 
                        ITS_MEDIUM = 24,
                        ITS_LARGE  = 32 };

// Holds volatile application data
//
struct APPLICATION
{
   // Application data
   HWND              hMainWnd,               // Main window handle
                     hActiveDialog;
   GAME_DATA        *pGameData;              // All loaded game data
   FILE_SYSTEM      *pFileSystem;            // Virtual FileSystem 
   CALL_STACK_LIST  *pCallStackList;         // Per-thread Call stacks
   CONSOLE_DATA     *pConsole;               // Debugging console data
   IMAGE_TREE       *pSmallImageTree,        // Small icons
                    *pMediumImageTree,       // Medium icons
                    *pLargeImageTree;        // Large icons
   WINDOWS_VERSION   eWindowsVersion;        // Windows version

   // Process data
   HINSTANCE         hAppInstance,           // Main thread instance handle
                     hLibraryInstance,       // Logic library instance handle
                     hResourceInstance;      // Resource library instance handle
   DWORD             hHelpFile;              // HTML Help Cookie
   HMODULE           hRichEditDLL;           // RichEdit DLL

   // Application State
   APPLICATION_STATE eState;                 // Application state
   BOOL              bClosing,               // Whether the app is closing
                     bVerbose,               // Whether to output greater detail to the console
                     bCriticalErrors[6];     // Types of critical errors encountered

   // Convenience strings
   TCHAR             szName[128],                  // Application name (Useful to have)
                     szResourceLibrary[MAX_PATH],  // Full path of resource library (useful to have)
                     szGameVersions[5][32];        // Game versions (Used in error message alot, useful to have) 
   // System ImageList icons
   INT               iSystemImageListIcons[8];     // Index of GameVersion and DocumentType icons within the System ImageList
};

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    CALL STACKS
///
///   Represents per-thread call stacks for tracking exceptions
/// ////////////////////////////////////////////////////////////////////////////////////////

// Defines a call within a call stack
//
struct FUNCTION_CALL
{
   CONST TCHAR  *szFunction,     // Name of the function
                *szFileName;     // File containing the function
   UINT          iLine;          // Line of the function
};

// Defines a thread's call stack  (Stack of FunctionCalls)
//
struct CALL_STACK : STACK
{
   DWORD   dwThreadID;           // Thread ID
};

// Lists of CallStacks
//
struct CALL_STACK_LIST : LIST
{
   HANDLE  hMutex;               // Synchronisation
};

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    CODE ARRAY INDICIES
///
///   Defines the purpose of each CodeArray index
/// ////////////////////////////////////////////////////////////////////////////////////////

// Defines the seven elements of the <script> node
//
enum   SCRIPT_NODE_INDEX { SFI_NAME                 = 0,    // Order can't be assured
                           SFI_VERSION              = 1,    // Order can't be assured
                           SFI_ENGINE_VERSION       = 2,    // Order can't be assured
                           SFI_DESCRIPTION          = 3,    // Order can't be assured
                           SFI_ARGUMENTS            = 4,    // Order can't be assured
                           SFI_SOURCE_TEXT          = 5,
                           SFI_CODE_ARRAY           = 6  };

// Defines the purpose of each of the 10 elements of the CodeArray tree
//
enum   CODE_ARRAY_INDEX {  CAI_SCRIPT_NAME          = 0,
                           CAI_ENGINE_VERSION       = 1,
                           CAI_DESCRIPTION          = 2,
                           CAI_VERSION              = 3,
                           CAI_LOADING_FLAG         = 4,
                           CAI_VARIABLES            = 5,
                           CAI_STANDARD_COMMANDS    = 6,
                           CAI_ARGUMENTS            = 7,
                           CAI_AUXILIARY_COMMANDS   = 8,
                           CAI_COMMAND_ID           = 9      };


// Defines important indicies of a ScriptTree of a standard command
//
enum STANDARD_SCRIPT_TREE_INDEX   { SSTI_COMMAND_ID      = 0,
                                    SSTI_FIRST_PARAMETER = 1   };


// Defines important indicies of a ScriptTree of an auxiliary command
//
enum AUXILIARY_SCRIPT_TREE_INDEX  { ASTI_COMMAND_INDEX   = 0,
                                    ASTI_COMMAND_ID      = 1,
                                    ASTI_FIRST_PARAMETER = 2   };

// Defines important indicies of a ScriptTree of a commented (standard or auxiliary) command
enum COMMAND_COMMENT_SCRIPT_TREE_INDEX  { CSTI_COMMAND_INDEX   = 0,
                                          CSTI_COMMAND_ID      = 1,
                                          CSTI_SUB_COMMAND_ID  = 2,
                                          CSTI_FIRST_PARAMETER = 3  };

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                     CODE  OBJECTs
///
///   Used for splitting a line of code (text) into its components. eg. variables, comments etc.
/// ////////////////////////////////////////////////////////////////////////////////////////

//
// Classifies a single character
enum CHARACTER_CLASS   {   CC_WHITESPACE = 0,         // Tabs, spaces, line breaks
                           CC_LETTER     = 1,         // Letters
                           CC_OPERATOR   = 3,         // Everything on the keyboard that isn't one of the other 3 classes
                           CC_NUMBER     = 4      };  // Numbers

// Classifies a Code Object
//
enum CODEOBJECT_CLASS  {   CO_ARGUMENT     = 0,       // 'Arguments'    (Script arguments)
                           CO_WORD         = 1,       // 'Commands'     (Text forming part of a command)
                           CO_COMMENT      = 2,       // 'Comments'
                           CO_ENUMERATION  = 3,       // 'Game Objects'
                           CO_KEYWORD      = 4,       // 'Keywords'     (Branching statements: if, else, skip, while, end, not, for)
                           CO_LABEL        = 5,       // 'Label Names'
                           CO_NUMBER       = 6,       // 'Numbers'
                           CO_OPERATOR     = 7,       // 'Operators'
                           CO_CONSTANT     = 8,       // 'Script Objects'
                           CO_STRING       = 9,       // 'Strings'
                           CO_VARIABLE     = 10,      // 'Variables'
                           CO_NULL         = 11,      // 'Null'
                           CO_WHITESPACE   = 12    }; // Whitespace

// Defines a CodeObject
//
struct  CODEOBJECT
{
   TCHAR*            szText;           // CodeObject text
   UINT              iLength;          // CodeObject length (in characters)
   CODEOBJECT_CLASS  eClass;           // CodeObject class
   UINT              iIndex;           // 1-based index of the current non-whitespace code object. eg. the first word in a line will have an index of 1 no matter how much whitespace preceeds it.

   CONST TCHAR*      szSource;          // The text this code object is tokenising, typically a line of script code
   UINT              iSourceLength,     // Length of source text, in characters
                     iStartCharIndex,   // The character index (into szSource) where current code object begins
                     iEndCharIndex;     // The character index (into szSource) where current code object ends
};

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   COLOUR SCHEME
///
///   Represents the collection of colour settings required to display the script code
///      and language file entries.
/// /////////////////////////////////////////////////////////////////////////////////////////

// GameString/LanguageMessage text colours
enum  GAME_TEXT_COLOUR  { GTC_BLACK, GTC_BLUE, GTC_CYAN, GTC_DEFAULT, GTC_GREEN, GTC_ORANGE, GTC_PURPLE, GTC_RED, GTC_WHITE, GTC_YELLOW };

// Application Interface Colours
enum  INTERFACE_COLOUR  { IC_BLACK, IC_DARK_BLUE, IC_DARK_GREEN, IC_DARK_GREY, IC_DARK_RED, IC_LIGHT_BLUE, IC_LIGHT_GREEN, IC_LIGHT_GREY, IC_LIGHT_RED, IC_ORANGE, IC_PURPLE, IC_YELLOW, IC_WHITE };

// Interface colour scheme
//
struct COLOUR_SCHEME
{
   TCHAR             szName[64],                   // Colour scheme name
                     szFontName[LF_FACESIZE];      // Font name
   UINT              iFontSize;                    // Font size, in points
   BOOL              bFontBold;                    // TRUE if font is in bold

   INTERFACE_COLOUR  eBackgroundColour;            // CodeEdit Background colour
   INTERFACE_COLOUR  eCodeObjectColours[CODEOBJECT_CLASSES]; // CodeObject colours
};

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       CONOSLE
///
///   Debugging console window and log file
/// /////////////////////////////////////////////////////////////////////////////////////////

// Holds the window handle and synchronisation object
//
struct CONSOLE_DATA
{
   HWND               hWnd;               // Window handle
   CRITICAL_SECTION   oCriticalSection;   // Critical section
};

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       COMMAND
///
///   Represents a script command (as found in an X3 game script).  It is used as an intermediary
///      in the conversion process.
/// /////////////////////////////////////////////////////////////////////////////////////////

// Forward declarations
struct LIST;
struct ERROR_STACK;
struct PARAMETER;
struct PARAMETER_ARRAY;
struct RICH_TEXT;
enum CONDITIONAL_ID;
enum GAME_VERSION;
enum OPERATION_RESULT;
enum PARAMETER_SYNTAX;

// COMMAND_GROUP - Defines a command's category
//
enum COMMAND_GROUP  {   CG_ARRAY             = 0,
                        CG_CUSTOM            = 1,
                        CG_FLEET             = 2,
                        CG_FLOW_CONTROL      = 3,
                        CG_GAME_ENGINE       = 4,
                        CG_GRAPH             = 5,
                        CG_MACRO             = 6,
                        CG_MARINE            = 7,
                        CG_MATHS             = 8,
                        CG_MERCHANT          = 9,
                        CG_NON_PLAYER        = 10,
                        CG_OBJECT_ACTION     = 11,
                        CG_OBJECT_PROPERTY   = 12,
                        ///CG_OBJECT_TRADE      = 11,    REM: Removed
                        CG_PASSENGER         = 13,
                        CG_PILOT             = 14,
                        CG_PLAYER            = 15,
                        CG_SCRIPT_PROPERTY   = 16,
                        CG_SHIP_ACTION       = 17,
                        CG_SHIP_PROPERTY     = 18,
                        CG_SHIP_TRADE        = 19,
                        CG_SHIP_WING         = 20,
                        ///CG_STATION_ACTION    = 19,    REM: Removed
                        CG_STATION_PROPERTY  = 21,
                        CG_STATION_TRADE     = 22,
                        CG_STOCK_EXCHANGE    = 23,
                        CG_STRING            = 24,
                        CG_SYSTEM_PROPERTY   = 25,
                        CG_UNIVERSE_DATA     = 26,
                        CG_UNIVERSE_PROPERTY = 27,
                        CG_USER_INTERFACE    = 28,
                        GC_WAR               = 29,
                        CG_WARE_PROPERTY     = 30,
                        CG_WEAPON_PROPERTY   = 31,

                        CG_HIDDEN            = 32 };

// Number of Command groups
//
#define                COMMAND_GROUP_COUNT    33

// Define helpers for iterating over groups
//
#define                FIRST_COMMAND_GROUP_RESOURCE   IDS_COMMAND_GROUP_ARRAY
#define                FIRST_COMMAND_GROUP            CG_ARRAY
#define                LAST_COMMAND_GROUP             CG_WEAPON_PROPERTY

// Indicies of the command properties as stored in CommandSyntax.txt
//
enum  COMMAND_SYNTAX_INDEX  { CSI_GROUP_NAME, 
                              CSI_COMPATIBILITY,
                              CSI_COMMAND_ID, 
                              CSI_MSCI_LINK,
                              CSI_SYNTAX, 
                              CSI_FIRST_PARAMETER, 
                              CSI_SECOND_PARAMETER, 
                              CSI_THIRD_PARAMETER, 
                              CSI_FOURTH_PARAMETER, 
                              CSI_FIFTH_PARAMETER, 
                              CSI_SIXTH_PARAMETER, 
                              CSI_SEVENTH_PARAMETER, 
                              CSI_EIGHTH_PARAMETER };

// Classifies each command into one (or more) of five categories that define how it is processed
//
enum   COMMAND_TYPE    {  CT_STANDARD    = 1,     // Standard commands require no special processing (most commands are standard)
                          CT_AUXILIARY   = 2,     // Auxiliary commands are stored in a separate tree. They are not executable but enhance readability
                          CT_CMD_COMMENT = 4,     // Command comments are standard or auxiliary commands that have been commented out
                          CT_EXPRESSION  = 8,     // Expressions are assignment or conditional commands
                          CT_SCRIPTCALL  = 16,    // Script calls are calls to external scripts
                          CT_VIRTUAL     = 32  }; // Virtual commands represent a group of standard commands

// Identifies what version of the game a command is compatible with
//
enum   COMMAND_VERSION  { CV_THREAT          = 1,        // X2 The Threat
                          CV_REUNION         = 2,        // X3 Reunion
                          CV_TERRAN_CONFLICT = 4,        // X3 Terran Conflict
                          CV_ALBION_PRELUDE  = 8  };     // X3 Albion Prelude

// Define IDs of some important commands, for clarity
//
#define    CMD_NONE                         0
#define    CMD_COMMENT                      1
#define    CMD_NOP                          2
#define    CMD_COMMAND_COMMENT              3
#define    CMD_END                          4
#define    CMD_ELSE                         5
#define    CMD_BREAK                        7
#define    CMD_CONTINUE                     6
#define    CMD_GOTO_LABEL                   100
#define    CMD_DEFINE_LABEL                 101
#define    CMD_CALL_SCRIPT_VAR_ARGS         102
#define    CMD_RETURN                       103
#define    CMD_EXPRESSION                   104
#define    CMD_RANDOM_VALUE_ZERO            106
#define    CMD_RANDOM_VALUE                 110
#define    CMD_INTERRUPT_SCRIPT             111
#define    CMD_HIDDEN_JUMP                  112
#define    CMD_CONNECT_SHIP_SCRIPT          113 
#define    CMD_SET_GLOBAL_SCRIPT_MAP        118
#define    CMD_IS_DATATYPE                  125
#define    CMD_READ_TEXT                    126
#define    CMD_ARRAY_ALLOC                  128
#define    CMD_ARRAY_ACCESS                 129
#define    CMD_ARRAY_ASSIGNMENT             130
#define    CMD_SIZE_OF_ARRAY                131
#define    CMD_GET_NEWS_ARTICLE_STATE       138
#define    CMD_SET_NEWS_ARTICLE_STATE       139
#define    CMD_INCREMENT                    142
#define    CMD_DECREMENT                    143
#define    CMD_STRING_PRINTF                150
#define    CMD_SET_SHIP_UPGRADE_SCRIPT      154
#define    CMD_SET_LOCAL_VARIABLE           155
#define    CMD_GET_LOCAL_VARIABLE           156
#define    CMD_SET_GLOBAL_VARIABLE          157
#define    CMD_GET_GLOBAL_VARIABLE          158
#define    CMD_INTERRUPT_SCRIPT_ARGS        159
#define    CMD_SEND_INCOMING_QUESTION       212
#define    CMD_SEND_INCOMING_QUESTION_EX    213
#define    CMD_DEFINE_ARRAY                 250
#define    CMD_FOR_EACH                     251
#define    CMD_FOR_EACH_COUNTER             252
#define    CMD_FOR_LOOP                     253
#define    CMD_ADD_MENU_ITEM_BYREF          254
#define    CMD_ADD_MENU_INFO_BYREF          255
#define    CMD_ADD_MENU_HEADING_BYREF       256
#define    CMD_WRITE_LOGBOOK                403
#define    CMD_WRITE_LOG                    406
#define    CMD_AVERAGE_PRICE_BY_REF         628
#define    CMD_AVERAGE_PRICE                629
#define    CMD_REGISTER_AL_SCRIPT           1000
#define    CMD_UNREGISTER_AL_SCRIPT         1001
#define    CMD_INTERRUPT_TASK_ARGS          1016
#define    CMD_BEGIN_TASK_ARGS              1017
#define    CMD_SPEAK_TEXT                   1042
#define    CMD_IS_SCRIPT_ON_STACK           1072
#define    CMD_GET_INDEX_OF_ARRAY           1083
#define    CMD_REGISTER_QUEST_SCRIPT        1084
#define    CMD_ADD_EVENT_LISTENER           1088
#define    CMD_DISPLAY_NEWS_ARTICLE         1090
#define    CMD_REGISTER_HOTKEY_1            1100
#define    CMD_REGISTER_GOD_EVENT           1141
#define    CMD_GOTO_SUB                     1167
#define    CMD_END_SUB                      1168
#define    CMD_DELAYED_COMMAND              1185
#define    CMD_CONNECT_WING_SCRIPT          1186
#define    CMD_READ_TEXT_ARRAY              1337
#define    CMD_READ_TEXT_EXISTS             1338
#define    CMD_ADD_SECONDARY_SIGNAL         1339
#define    CMD_SET_WING_UPGRADE_SCRIPT      1381
#define    CMD_SET_GLOBAL_WING_SCRIPT_MAP   1382
#define    CMD_SET_WING_COMMAND             1384
#define    CMD_ADD_WING_SCRIPT              1393
#define    CMD_ADD_WING_SCRIPT_CHECK        1398
#define    CMD_SET_GLOBAL_SECONDARY_MAP     1407
#define    CMD_GET_LOCAL_VARIABLE_KEYS      1414
#define    CMD_CALL_SCRIPT_ARGS             1425
#define    CMD_LAUNCH_SCRIPT_ARGS           1426
#define    CMD_GET_GLOBAL_VARIABLE_KEYS     1436
#define    CMD_ADD_MENU_INFO                1446
#define    CMD_ADD_MENU_ITEM                1447
#define    CMD_ADD_MENU_HEADING             1448
#define    CMD_GET_LOCAL_VARIABLE_REGEX     1450
#define    CMD_GET_GLOBAL_VARIABLE_REGEX    1451
#define    CMD_SET_SHIP_PRELOAD_SCRIPT      1466
#define    CMD_SET_WING_PRELOAD_SCRIPT      1467
#define    CMD_GET_OBJECT_NAME_ARRAY        1496
#define    CMD_REGISTER_HOTKEY_2            1511
#define    CMD_ADD_NEWS_ARTICLE             1531
#define    CMD_ADD_INFO_ARTICLE             1532
#define    CMD_ADD_HISTORY_ARTICLE          1533
#define    CMD_ADD_STORIES_ARTICLE          1534
#define    CMD_ADD_SCRIPT_OPTIONS           1598
#define    CMD_REMOVE_SCRIPT_OPTIONS        1599
#define    CMD_CHECK_SCRIPT_EXISTS_1        1600
#define    CMD_REGISTER_HOTKEY_1_BYREF      1601
#define    CMD_GET_HOTKEY_INDEX_1           1602
#define    CMD_REGISTER_HOTKEY_2_BYREF      1603
#define    CMD_GET_HOTKEY_INDEX_2           1604
#define    CMD_CHECK_SCRIPT_EXISTS_2        1605
#define    CMD_ADD_PLAYER_GRAPH             1608
#define    CMD_REMOVE_PLAYER_GRAPH          1609
#define    CMD_ADD_CLASS_GRAPH              1610
#define    CMD_REMOVE_CLASS_GRAPH           1611
#define    CMD_ADD_OBJECT_GRAPH             1612
#define    CMD_REMOVE_OBJECT_GRAPH          1613

// Defines the syntax of a script command: ID + text + position and type of parameters
//
struct COMMAND_SYNTAX
{
   LONG                 iID;                     // ID of the command
   TCHAR               *szSyntax,                // Syntax string of the command
                       *szHash,                  // Hash of the syntax string
                       *szSuggestionText,        // Text used to insert as suggestion
                       *szReferenceURL;          // MSCI forum URL
                       
   RICH_TEXT           *pDisplaySyntax,          // Syntax as displayed by suggestion box and search dialog
                       *pTooltipSyntax,          // Syntax as displayed in a tooltip
                       *pTooltipDescription;     // Description as displayed in a tooltip

   COMMAND_GROUP        eGroup;                  // Classification
   GAME_VERSION         eGameVersion;            // Game version
   UINT                 iCompatibility,          // Combination of COMMAND_VERSION flags indicating the game versions the command is valid for
                        iVariation;              // HACK: Command variation: 1 or 0 (Usually zero but about 10 commands have an alternate variation under a different game version)

   UINT                 iParameterCount;         // Number of Parameters
   PARAMETER_SYNTAX     eParameters[10];         // List of command's PARAMETER syntax
   BYTE                 bMayHaveReturnObject;    // Whether this command may have a ReturnObject or not
};


// Represents a single script command, including it's text and all it's parameters.  Also contains a link to it's syntax.
//      It is used for all types of script commands during 'translation' and 'generation'. Some properties are only used in certain circumstances
//      
struct COMMAND
{
   // Main
   UINT                   iID;               // Unique Command ID
   CONST COMMAND_SYNTAX*  pSyntax;           // Associated Command syntax

   // Parameters
   PARAMETER_ARRAY  *pParameterArray,        // Parameters array
                    *pInfixParameterArray,   // [EXPRESSIONS] Parameters array sorted (or parsed) into Infix order
                    *pPostfixParameterArray; // [EXPRESSIONS] Parameters array sorted (or parsed) into PostFix order

   // Properties
   OPERATION_RESULT  eResult;                // [TRANSLATION] Translation result
   CONDITIONAL_ID    eConditional;           // [GENERATION] Conditional used, if any
   TCHAR             szBuffer[LINE_LENGTH];  // Input/Output string conversion buffer
   UINT              iIndex,                 // Index of the command within a list or script
                     iReferenceIndex,        // [AUXILIARY] Index of the associated standard command
                     iFlags;                 // Combination of COMMAND_TYPE flags

   // Errors
   ERROR_QUEUE*      pErrorQueue;            // Error messages encountered during translation, if any
   UINT              iLineNumber;            // [ERROR REPORTING] Line number containing the command
};

// Defines the two types of Richtext that can be generated for syntax strings
//
enum  RICH_SYNTAX_TYPE { RST_BOLD_TEXT, RST_BOLD_PARAMETERS };

// Contains basic information about a script command necessary to parse it
//
struct COMMAND_HASH
{
   TCHAR   *szConditionalHash,
           *szCommandHash;
   BOOL     bComment;
};

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    COMMAND COMPONENT
///
///   A COMMAND syntax parsing object for translating COMMANDs to strings
/// ////////////////////////////////////////////////////////////////////////////////////////

enum COMMAND_COMPONENT_TYPE { CCT_COMMAND_TEXT, CCT_PARAMETER_INDEX, CCT_SCRIPT_ARGUMENT };

// A COMMAND syntax string parser for general commands and the special cases of expressions and script calls
// 
struct COMMAND_COMPONENT
{
// public:
   COMMAND_COMPONENT_TYPE  eType;      // CommandComponent type
   TCHAR*                  szText;     // [COMMAND STRING]  CommandComponent contents
   UINT                    iIndex;     // [PARAMETER INDEX] Zero-based parameter index

// private:
   TCHAR    *szSource,           // Command syntax source text
            *szPosition;         // Current position within the source text
   UINT      iFlags,             // Input COMMAND type flags
             iComponent,         // Zero-based index of the current component
             iParameterCount;    // Number of expression parameters
};

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                 DESCRIPTION VARIABLE
///
///   Holds the search/replacement phrases used in populating object descriptions
/// ////////////////////////////////////////////////////////////////////////////////////////

struct  DESCRIPTION_VARIABLE
{
   TCHAR    *szName,      // Search text
            *szValue;     // Replacement text
   UINT      iParameters;       // Number of parameters
   BOOL      //bLiteral,
             //bKeyword,
             bRecursive;
};
 
/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    ERROR HANDLING
///
///   Defines error messages and their severity. Messages are contained in stacks, 
///    representing the source trace.  ErrorStacks are contained in queues, 
///     representing multiple operation errors.
/// ////////////////////////////////////////////////////////////////////////////////////////

// Defines the severity of an error
//
enum ERROR_TYPE        { ET_ERROR       = 0,        // Critical Error
                         ET_WARNING     = 1,        // Non-critical Warning
                         ET_INFORMATION = 2  };     // Information

// Defines how to respond when errors occur
// 
enum ERROR_HANDLING    { EH_ABORT,             // Inform the user of the error and abort
                         EH_IGNORE,            // Ignore all errors and inform the user afterwards
                         EH_PROMPT  };         // Ask the user whether they wish to continue


// Distinguishses between 'message' and 'attachment' ErrorMessages
//
enum ERROR_MESSAGE_TYPE    { MT_MESSAGE, MT_ATTACHMENT };


// Represents an individual error message
//
struct ERROR_MESSAGE
{
   UINT       iID;             // Error ID
   TCHAR      *szMessage,      // Error message
              *szID;
};

// Represents a stack of messages of increasing technical detail
//
struct ERROR_STACK : STACK
{
   ERROR_TYPE      eType;      // Severity of error
};

/// Macro: HERE
// Description: Convenience macro for assembling the current position of a line of source code. It assembles
//                the current position in the file into the parameters taken by the 'createError' function
//
// UINT  nErrorCode  :  The error code, this is tokenised as part of the output
// 
#define      HERE(nErrorCode)      nErrorCode, _T(#nErrorCode), __WFUNCTION__, __SWFILE__, __LINE__

/// ////////////////////////////////////////////////////////////////////////////////////////
///                              FILE DIALOG FILE ITEM
///
///   An item (file) displayed in the file dialog
/// ////////////////////////////////////////////////////////////////////////////////////////
struct SCRIPT_FILE;

// Attribute flags for each file in a FileDialog, indicating whether it is also a shortcut or a folder
// 
enum  FILE_ITEM_FLAG { FIF_NONE         = 0x00000000, 
                       FIF_FOLDER       = 0x00000002, 
                       FIF_SHORTCUT     = 0x00000004,
                       FIF_SYSTEM       = 0x00000008,
                       FIF_PHYSICAL     = 0x00000010,
                       FIF_VIRTUAL      = 0x00000020,

                       FIF_SCRIPT       = 0x00000100,
                       FIF_LANGUAGE     = 0x00000200,
                       FIF_MISSION      = 0x00000400,
                       FIF_UNIVERSE     = 0x00000800,
                       FIF_CONVERSATION = 0x00001000, 
                       FIF_PROJECT      = 0x00002000  };

// The possible filters for FileItem search results
//
enum  FILE_FILTER { FF_SCRIPT_FILES,               // XML/PCK MSCI scripts
                    FF_SCRIPT_PCK_FILES,           // PCK MSCI scripts
                    FF_SCRIPT_XML_FILES,           // XML MSCI scripts
                    FF_LANGUAGE_FILES,             // Language files
                    FF_MISSION_FILES,              // MD scripts
                    FF_PROJECT_FILES,              // Project files
                    FF_DOCUMENT_FILES,             // MSCI/MD scripts or language files
                    FF_ALL_FILES        };         // All files

// Number of filters
#define            FILE_FILTER_COUNT      8

// File dialog display item (a file or folder)
//
struct  FILE_ITEM
{
   TCHAR          szDisplayName[MAX_PATH],      // Shell Display name (text displayed in ListView)
                  szDisplayType[80],            // File type, as string. 
                  szFullPath[MAX_PATH],         // Full path of the item
                  szDescription[128];           // Script description
              
   UINT           iAttributes,         // Whether item is physical, virtual, shortcut, folder, system, script, language, mission etc.
                  iIconIndex,          // System ImageList icon index
                  iOverlayIndex,       // System ImageList overlay icon index
                  iFileSize,           // Size of the file
                  iFileVersion;        // Script version
   GAME_VERSION   eGameVersion;        // Script game version
};


/// Name: extractFileItemProperties
//  Description: Extracts the flags relating to file properties
//
//  UINT  eItemFlags : [in] Combination of FILE_ITEM_FLAGs indicating a file's properies and file-type
//
#define extractFileItemProperties(eItemFlags)     (eItemFlags INCLUDES 0x000000FF)

/// Name: extractFileItemType
//  Description: Extracts the flag which defines the file type
//
//  UINT  eItemFlags : [in] Combination of FILE_ITEM_FLAGs indicating a file's properies and file-type
//
#define extractFileItemType(eItemFlags)           ((FILE_ITEM_FLAG)(eItemFlags INCLUDES 0xFFFFFF00))

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                 GAME CONSTANTS
///
///   Various game constants used throughout the application
/// /////////////////////////////////////////////////////////////////////////////////////////

// The available game languages
//
enum GAME_LANGUAGE    { GL_RUSSIAN = 07,      // 
                        GL_FRENCH  = 33,      // 
                        GL_ITALIAN = 39,      // 
                        GL_SPANISH = 42,      // 
                        GL_ENGLISH = 44,      // 
                        GL_POLISH  = 48,      // 
                        GL_GERMAN  = 49   };  // 
                        

// Defines the game pages containing the definitions for the various game objects used throughout the application
// 
enum GAME_PAGE_ID  {  GPI_INVALID              = 0,
                      GPI_SECTORS              = 7,
                      GPI_STATION_SERIALS      = 12,
                      GPI_SHIPS_STATIONS_WARES = 17,
                      GPI_RELATIONS            = 35,
                      GPI_RACES                = 1266,
                      GPI_TRANSPORT_CLASSES    = 1999,
                      GPI_PARAMETER_TYPES      = 2000,
                      GPI_OPERATORS            = 2001,
                      GPI_CONSTANTS            = 2002,
                      GPI_COMMAND_SYNTAX       = 2003,
                      GPI_OBJECT_CLASSES       = 2006,
                      GPI_CONDITIONALS         = 2007,
                      GPI_OBJECT_COMMANDS      = 2008,
                      GPI_FLIGHT_RETURNS       = 2009,
                      GPI_FORMATIONS           = 2012,
                      GPI_DATA_TYPES           = 2013,
                      GPI_WING_COMMANDS        = 2028   };

// Defines various MainTypes used for defining supported objects
//
enum MAIN_TYPE  { MT_NONE            = 0,    // Used by GAME_DATA_FILE to represent media data files
                  MT_DOCK            = 5,
                  MT_FACTORY         = 6,
                  MT_SHIP            = 7,
                  MT_WARE_LASER      = 8,
                  MT_WARE_SHIELD     = 9,
                  MT_WARE_MISSILE    = 10,
                  MT_WARE_ENERGY     = 11,
                  MT_WARE_NATURAL    = 12,
                  MT_WARE_BIOLOGICAL = 13,
                  MT_WARE_FOODSTUFF  = 14,
                  MT_WARE_MINERAL    = 15,
                  MT_WARE_TECHNOLOGY = 16  };

// Defines the 'Relation' IDs used in MSCI scripts
//
enum RACE_RELATION { RR_FOE          = -1,
                     RR_NEUTRAL      =  0,
                     RR_FRIEND       =  1  };

// Defines the three 'station sizes' used in TDocks and TFactories
// 
enum STATION_SIZE {  SS_NONE              = 1,
                     SS_MEDIUM            = 2,
                     SS_LARGE             = 5,
                     SS_EXTRA_LARGE       = 10,
                     SS_EXTRA_EXTRA_LARGE = 20  };

// Defines the two important categories of 'ship variation' that have no directly associated game string
//
enum SHIP_VARIATION { SV_NONE    = 0,
                      SV_SPECIAL = 20 };

// Defines the Egosoft string IDs used to represent un-defined or special game objects
//
enum UNDEFINED_GAME_OBJECT   { SGOI_UNKNOWN_OBJECT_1 = 9001,         // Used in game as an Unknown Object
                               SGOI_UNKNOWN_OBJECT_2 = 9041,         // Used in game as an Unknown Object
                               SGOI_UNKNOWN_OBJECT_3 = 17206,        // Used in game as an Unknown Object
                               SGOI_UNDEFINED_NAME   = 9999,         // Intentionally undefined ship/station/ware
                               SGOI_UNDEFINED_RACE   = 34000  };     // Intentionally undefined ship/station/ware

// Property indicies for game object names as stored in their TFiles
//
enum GAME_OBJECT_PROPERTY_INDEX
{ 
   GOPI_FILE_VERSION          = 0,        // [HEADER]  File structure version
   GOPI_OBJECT_COUNT          = 1,        // [HEADER]  Number of objects in the TFile
   GOPI_NAME_ID               = 6,        // [ALL]     Name string ID
   GOPI_LASER_OBJECT_ID       = 22,       // [LASER]   Object ID string
   GOPI_MISSILE_OBJECT_ID     = 36,       // [MISSILE] Object ID string
   GOPI_SHIELD_OBJECT_ID      = 20,       // [SHIELD]  Object ID string
   GOPI_WARE_OBJECT_ID        = 16,       // [WARE]    Object ID string
   GOPI_STATION_RACE_ID       = 13,       // [STATION] Race string ID
   GOPI_FACTORY_SIZE_ID       = 17,       // [FACTORY] Factory Size string ID
   GOPI_FACTORY_OBJECT_ID     = 28,       // [FACTORY] Factory Object ID string
   GOPI_DOCK_OBJECT_ID        = 27,       // [DOCK]    Dock Object ID string
   GOPI_SHIP_RACE_ID          = 45,       // [SHIP]    Race string ID
   GOPI_SHIP_VARIATION_ID     = 50,       // [SHIP]    Variation string ID
};


//   UNKNOWN - Bits and pieces I haven't decided where to put yet
// /////////////////////////////////////////////////////////////////////////////////////////

#define      UGC_UNARY_OPERATOR   0x00010000     // Changed from '1'
#define      UGC_LIVE_SECTOR      0x00010000     // Changed from '1'
#define      UGC_LIVE_DATATYPE    0x00010000     // Changed from '1'
#define      UGC_LIVE_VALUE       0x00020000     // Changed from '2'
#define      UGC_LIVE_SCRIPT      0x00000002     // 
//
// Unknown constants used by the game I still don't understand
//
enum  UNKNOWN_GAME_CONSTANTS    {   UGC_SPECIAL_VARIATION = 20    };      // Has some unknown significance but no associated game string

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    GAME DATA
///
///   Holds the processed game data and meta-data relating to it
/// ////////////////////////////////////////////////////////////////////////////////////////
// Forward declarations
struct AVL_TREE;
struct FILE_SYSTEM;
enum DATA_TYPE;

// Defines a data file used for generating an ObjectNames and MediaItems
//
struct GAME_DATA_FILE
{
   CONST TCHAR  *szSubPath,   // Game folder sub-path to the file
                *szAlternateExtension;

   MAIN_TYPE     eMainType;   // [OBJECT NAME] Primary sort key for storing ObjectNames
   DATA_TYPE     eDataType;   // [OBJECT NAME] Defines whether data file holds ships, stations or wares
};

// IDs of the data files used by the application
//
enum GAME_DATA_FILE_INDEX { GFI_DOCKS, 
                            GFI_FACTORIES, 
                            GFI_SHIPS,
                            GFI_LASERS,
                            GFI_SHIELDS,
                            GFI_MISSILES,
                            GFI_WARE_ENERGY,
                            GFI_WARE_NONPLAYER,
                            GFI_WARE_BIOLOGICAL,
                            GFI_WARE_FOODSTUFF,
                            GFI_WARE_MINERAL,
                            GFI_WARE_TECHNOLOGY,
                            GFI_SOUND_EFFECTS,
                            GFI_VIDEO_CLIPS,
                            GFI_SPEECH_CLIPS,
                            GFI_MASTER_LANGUAGE,
                            GFI_MASTER_LANGUAGE_REUNION,    // Should not be used directly
                            GFI_LANGUAGE_FOLDER,
                            GFI_MISSION_FOLDER,
                            GFI_SCRIPT_FOLDER,
                            GFI_CATALOGUE,
                            GFI_CATALOGUE_DATA,
                            GFI_THREAT,
                            GFI_REUNION,
                            GFI_TERRAN_CONFLICT,
                            GFI_ALBION_PRELUDE     };


// Global game data
//
struct GAME_DATA
{
   AVL_TREE         *pCommandTreeByID,          // COMMAND_SYNTAX entries, organised by ID
                    *pCommandTreeByHash,        // COMMAND_SYNTAX entries, organised by command hash

                    *pGameStringsByID,          // GAME_STRING language file entries, organised by PageID then StringID
                    *pGamePagesByID,            // GAME_PAGE language file pages, organised by PageID

                    *pMediaItemsByID,           // MEDIA_ITEM audio/video items, organised by PageID then ID
                    *pMediaPagesByGroup,        // MEDIA_PAGE audio/video pages, organised by Group then PageID

                    *pGameObjectsByMainType,       // GAME_OBJECT T-File entries, organised by Group then ID
                    *pGameObjectsByText,           // GAME_OBJECT T-File entries , organised by Text

                    *pScriptObjectsByGroup,        // SCRIPT_OBJECT compilation strings, organised by Group then ID
                    *pScriptObjectsByText;         // SCRIPT_OBJECT compilation strings, organised by Text
};

// Game versions
//
enum  GAME_VERSION  { GV_THREAT          = 0,         // EngineVersion:  17 -> 24
                      GV_REUNION         = 1,         // EngineVersion:  25 -> 39
                      GV_TERRAN_CONFLICT = 2,         // EngineVersion:  40 -> 44
                      GV_ALBION_PRELUDE  = 3,         // EngineVersion:  45 -> 50 ?
                      GV_UNKNOWN         = 4   };     // Placeholder for an undetermined game data version

// Game folder flags
//
enum GAME_FOLDER_CONTENT  { GMC_NOTHING    = 0,        // No game data
                            GMC_DATA_FILES = 1,        // Data files
                            GMC_EXECUTABLE = 2,        // A game executable
                            GMC_CATALOGUES = 4 };      // Game catalogues

// Represents the contents of any text file stored on disc. Acts as the base for ScriptFiles, LanguageFiles, SpeechFiles and ProjectFiles
// 
struct GAME_FILE
{
   TCHAR      szFullPath[MAX_PATH],    // Full path of the file
             *szInputBuffer,           // UNICODE Input buffer
             *szOutputBuffer;          // UNICODE Output buffer
   BOOL       bResourceBased;          // Indicates the file was loaded from a resource

   UINT       iInputSize,              // Size of input buffer, in characters
              iOutputSize;             // Size of output buffer, in characters
};

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    GAME STRING
///
///   Represents a string loaded from the game data and defines it's unique ID and game version.
///    Also identifies which extremely annoying formatting symbols it contains.
/// ////////////////////////////////////////////////////////////////////////////////////////

// String conversion flags (for converting between formatt states)
//
enum CHARACTER_REPLACEMENT_FLAG    {  CRF_XML_SAFE      = 1,      // Replace characters unsafe for XML with phrases safe for XML
                                      CRF_XML_UNSAFE    = 2,      //    Reverse of above. Cannot be specified with 
                                      CRF_LANG_SAFE     = 4,      // Replace { and [ with \{ and \[
                                      CRF_LANG_UNSAFE   = 8,      // Remove backslashes prefacing brackets
                                      CRF_PRINTF_SAFE   = 16,     // Replace characters unsafe for printf with characters safe for printf
                                      CRF_PRINTF_UNSAFE = 32 };   //    Reverse of above

// String format states
//
enum  STRING_TYPE    {  ST_EXTERNAL,         // External strings have XML Safety phrases and % symbols
                        ST_INTERNAL,         // Internal strings have no XML safety phrases and have ¶ symbols
                        ST_DISPLAY    };     // Display strings have no XML safety phrases and have % symbols

enum  GAME_PAGE_GROUP  { GPG_DATA,
                         GPG_DIALOGUE,
                         GPG_MISC,
                         GPG_QUEST,
                         GPG_BONUS,
                         GPG_MENU,
                         GPG_EDITOR,
                         GPG_NEWS,
                         GPG_PLOT,
                         GPG_NPC,
                         GPG_USER  };

// Game string -- Represents either language file entries or assembled ware names
//
struct GAME_STRING
{
   TCHAR*         szText;       // String contents  (Max length of MAX_STRING)
   UINT           iCount;       // Length of string, in characters

   UINT           iID,          // String ID
                  iPageID;      // Page ID

   STRING_TYPE    eType;        // Whether string is formatted for External, Internal or Display use
   GAME_VERSION   eVersion;     // Version of the game the string is from

   /// NEW:
   BOOL           bModContent,  // Whether string is from a non-Egosoft supplementary language file or not
                  bResolved;    // Indicates sub-strings have been resolved
};

// Game page -- Represents a group of game strings
//
struct GAME_PAGE
{
   TCHAR         *szTitle,          // Page Title
                 *szDescription;    // [optional] Page Description
   UINT           iPageID;          // Page ID
   BOOL           bVoice;           // Whether the page is voiced
};


// Identifies the parameters in a command that hold references to GameStrings
//
struct GAME_STRING_REF
{
   UINT   iPageParameterIndex,      // Index of parameter containing the PageID reference
          iStringParameterIndex;    // Index of parameter containing the StringID reference
};

/// ////////////////////////////////////////////////////////////////////////////////////////
///                               IMAGE TREE
///
///   An ImageList organised by icon resource ID
/// ////////////////////////////////////////////////////////////////////////////////////////

// Holds loaded application icons
//
struct IMAGE_TREE
{
   AVL_TREE*     pIconsByID;     // IconTreeItems organised by ID
   HIMAGELIST    hImageList;     // ImageList containing the icons
};

// Represents an icon in an ImageList
//
struct IMAGE_TREE_ITEM
{
   HICON         hHandle;     // Icon handle
   CONST TCHAR*  szID;        // Resource ID
   UINT          iIndex;      // ImageList index
};

// Defines possible icon drawing states
//
enum  ICON_STATE  { IS_NORMAL, IS_DISABLED, IS_SELECTED };

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    OBJECT NAMES
///
///   Defines the hundreds of constants used by scripts to represent objects or concepts
///    from within the game.  They are classified as 'Game Objects' or 'Script Objects'
///    
/// ////////////////////////////////////////////////////////////////////////////////////////

// Defines the number of game/script object groups
//
#define          GAME_OBJECT_GROUP_COUNT       7
#define          SCRIPT_OBJECT_GROUP_COUNT     14

// Defines whether an ObjectName is a GameObject ("Enumeration") or a ScriptObject ("Constant")
//
enum OBJECT_NAME_TYPE  { ONT_GAME, ONT_SCRIPT };

// Defines the group an ObjectName belongs to   (used as primary sort key)
//
enum OBJECT_NAME_GROUP 
{  
// Game Objects
   ONG_DOCK             = 0,         // Dock
   ONG_FACTORY          = 1,         // Factory
   ONG_LASER            = 2,         // Laser
   ONG_MISSILE          = 3,         // Missile
   ONG_SHIP             = 4,         // Ship
   ONG_SHIELD           = 5,         // Shield
   ONG_WARE             = 6,         // Ware

// Script Objects
   ONG_CONSTANT         = 7,         // Script Constant
   ONG_DATA_TYPE        = 8,         // Variable Data Type
   ONG_FLIGHT_RETURN    = 9,         // Flight Return Code
   ONG_FORMATION        = 10,        // Formation
   ONG_OBJECT_CLASS     = 11,        // Object Class
   ONG_RACE             = 12,        // Race
   ONG_RELATION         = 13,        // Relation
   ONG_PARAMETER_TYPE   = 14,        // Parameter Syntax
   ONG_SECTOR           = 15,        // Sector
   ONG_STATION_SERIAL   = 16,        // Station Serial
   ONG_OBJECT_COMMAND   = 17,        // Object Command
   ONG_WING_COMMAND     = 18,        // Wing Command
   ONG_SIGNAL           = 19,        // Signal
   ONG_TRANSPORT_CLASS  = 20,        // Transport Class
   ONG_OPERATOR         = 21         // Operator
};

// Convenience descriptions for iterating through groups
#define FIRST_GAME_OBJECT_GROUP             ONG_DOCK
#define LAST_GAME_OBJECT_GROUP              ONG_WARE

#define FIRST_SCRIPT_OBJECT_GROUP           ONG_CONSTANT
#define LAST_SCRIPT_OBJECT_GROUP            ONG_TRANSPORT_CLASS

#define FIRST_GAME_OBJECT_GROUP_NAME        IDS_GAME_OBJECT_GROUP_DOCK
#define FIRST_SCRIPT_OBJECT_GROUP_NAME      IDS_SCRIPT_OBJECT_GROUP_CONSTANT

#define FIRST_OBJECTNAME_GROUP_NAME         IDS_GAME_OBJECT_GROUP_DOCK

// Represents a transport class ID
//
enum OBJECT_SIZE   { OS_TINY, OS_SMALL, OS_MEDIUM, OS_LARGE, OS_EXTRA_LARGE, OS_STATION };

// Encapsulates GameObject properties
//
struct OBJECT_PROPERTIES
{
   CONST TCHAR   *szMainType,          // Main type string
                 *szRawName,           // Un-mangled name
                 *szDescription,       // Description
                 *szTransportClass,    // Transport class string
                 *szStationSize,       // Station size string
                 *szManufacturer;      // Owner race string

   UINT           iVolume,             // Unit volume
                  iMinPrice,           // Min trading price
                  iMaxPrice,           // Max trading price
                  iMaxEnergy,          // Max laser or shield energy
                  iMinCargobay,        // Min cargobay volume
                  iMaxCargobay,        // Max cargobay volume
                  iMaxDocked,          // Max number of docked ships
                  iMinSpeed,           // Min speed
                  iMaxSpeed,           // Max speed
                  iMaxHull,            // Max hull energy
                  iMaxShields,         // Max Number of shields
                  iShieldType;         // Sub-type of shield
};


// Represents either a GameObject or ScriptObject
//
struct OBJECT_NAME
{
// Text
   TCHAR*             szText;          // Text
   UINT               iCount;          // Length of the text, in characters

// Identification
   OBJECT_NAME_TYPE   eType;           // Whether object is a game or script object
   OBJECT_NAME_GROUP  eGroup;          // Defines object group during display.  (primary sort key)
   UINT               iID;             // Unique ID within it's group. SubType for GameObjects, StringID for ScriptObjects. (secondary sort key)

// Game Objects
   TCHAR*             szObjectID;      // [GAME OBJECT] Object's ID within the game
   MAIN_TYPE          eMainType;       // [GAME OBJECT] MainType of the ship/station/ware the object represents (Used for generating packed WareIDs)
   //GAME_STRING*       szDescription;   // [GAME OBJECT] GameString containing the object description, or NULL if description was not found
   OBJECT_PROPERTIES  oProperties;     // [GAME OBJECT] Extended properties

// Script Objects
   GAME_VERSION       eVersion;        // [SCRIPT OBJECT] Game version of the string the object represents (Used for name mangling during conflict resolution)
   BOOL               bModContent;     // [SCRIPT OBJECT] Whether object is from a mod or Egosfot   
   RICH_TEXT*         pDescription;    // [SCRIPT OBJECT] Tooltip description
};

// Convenience type definitions for GameObjects and ScriptObjects
typedef  OBJECT_NAME    GAME_OBJECT;
typedef  OBJECT_NAME    SCRIPT_OBJECT;

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    JUMP  GENERATION
///
///   Used in the generation of jump commands and calculation of branching destinations
/// ////////////////////////////////////////////////////////////////////////////////////////

// ReturnObject:
//
//     HI                                       LO
//  ---------------------------------------------------
// :             :                   :                 :
// : RETURN TYPE : DESTINATION INDEX : CONDITIONAL ID  :
// :             :                   :                 :
//  -- 1 byte ---------- 2 bytes --------- 1 byte -----
//

// Identifies how the ReturnObject is handled internally by the game.
//
//
enum     RETURN_TYPE      {   RT_ASSIGNMENT    = 0x00,      // Assign result to a variable
                              RT_DISCARD       = 0x80,      // Discard result
                              RT_JUMP_IF_TRUE  = 0xA0,      // Jump to <destination> if result is TRUE
                              RT_JUMP_IF_FALSE = 0xE0  };   // Jump to <destination> if result is FALSE

// Identifies the branching statements used in the script language. The first 8 IDs are actually encoded in 
//            ReturnObjects whereas the last four are defined by auxiliary commands and used for my coding clarity.
//
enum     CONDITIONAL_ID   {   // Conditional IDs used by the game
                              CI_NONE      = 0,   CI_DISCARD     = 1,
                              CI_START     = 2,
                              CI_IF        = 3,   CI_IF_NOT      = 4, 
                              CI_ELSE_IF   = 5,   CI_ELSE_IF_NOT = 6, 
                              CI_SKIP_IF   = 7,   CI_SKIP_IF_NOT = 8,
                              CI_WHILE     = 9,   CI_WHILE_NOT   = 10,
                              // My custom IDs to represent auxiliary branching commands, jumps, gotos and labels.
                              CI_BREAK     = 11,  CI_CONTINUE    = 12,
                              CI_ELSE      = 13,  CI_END         = 14,
                              CI_LABEL     = 15,  CI_GOTO_LABEL  = 16,
                              CI_GOTO_SUB  = 17,  CI_END_SUB     = 18  };

// Used for searching for conditionals in a JumpStack
//
enum     CONDITIONAL_TYPE  { CT_IF = 1 , CT_WHILE = 2 };

// Defines an item within a 'jump stack' - each item may represent one of the branching statements above.
//  The ID of the statement, the line it occurs on and a link to the ScriptTree node containing it's ReturnObject
//  (which has already been processed into the standard command tree)
//
struct JUMP_STACK_ITEM
{
   CONDITIONAL_ID       eConditional;        // Conditional within the command

   COMMAND             *pCommand,            // Command containing the conditional
                       *pJumpCommand;        // [AUXILIARY] Associated jump command
};


// Placeholders used for processing before jump/variable calculations
#define    EMPTY_JUMP              0xFFFF
#define    EMPTY_VARIABLE          0xFFFF

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    LANGUAGE BUTTON
///
///   Data associated with a Language Entry OLE button
/// ////////////////////////////////////////////////////////////////////////////////////////

// Data associated with each OLE button object, containing it's ID and text.
//
struct  LANGUAGE_BUTTON
{
   TCHAR       *szText,     // Button's Text, as currently displayed as a bitmap
               *szID;       // Button's ID
   HBITMAP      hBitmap;    // Button bitmap;
   IOleObject*  pObject;    // OLE Object
};

/// //////////////////////////////////////////////////////////////////////////////////////////
///                                  LANGUAGE FILE
///
///   Represents an X3 game strings .xml file (or a speech clips definition file)
/// //////////////////////////////////////////////////////////////////////////////////////////

// Defines whether a LanguageFile contains game strings or speech clips
//
enum LANGUAGE_FILE_TYPE  { LFT_STRINGS, LFT_SPEECH, LFT_VARIABLES, LFT_DESCRIPTIONS };

// LANGUAGE_FILE: Used to hold strings, speech items, object descriptions or description variables parsed from XML files with similar formats
//
struct LANGUAGE_FILE : GAME_FILE
{
   LANGUAGE_FILE_TYPE  eType;                 // Identifies whether file contains strings, speech items, object descriptions or description variables

   GAME_LANGUAGE       eLanguage;             // Language ID of the strings within the file
   BOOL                bMasterStrings;        // [STRINGS]      Whether this represents the game's master language file (0001-L0nn.xml)

   AVL_TREE           *pGameStringsByID,      // [STRINGS]      Contains GameStrings by PAGE_ID then STRING_ID
                      *pGamePagesByID,        // [STRINGS]      Contains GamePages by PAGE_ID

                      *pSpeechClipsByID,      // [SPEECH]       Contains MediaItems by PAGE_ID then ITEM_ID
                      *pSpeechPagesByGroup,   // [SPEECH]       Contains MediaPages by GROUP then PAGE_ID

                      *pGameStringsByVersion, // [DESCRIPTIONS] Contains GameStrings by PAGE_ID then STRING_ID
                      *pVariablesByText;      // [VARIABLES]    Contains DescriptionVariables by TEXT
};

// Convenience description for a SpeechFile which temporarily holds speech clip definitions during game data loading
//
typedef LANGUAGE_FILE  SPEECH_FILE;
typedef LANGUAGE_FILE  VARIABLES_FILE;

// Helper object for parsing of language files; it holds page/item properties as they are parsed.
//
struct LANGUAGE_FILE_ITEM
{
   INT           iRawPageID,        // [ALL]      Current page ID possibly prefixed with 30, 35, 38 etc.
                 iItemID;           // [ALL]      Current item ID

   CONST TCHAR  *szTitle,           // [ALL]      Page title
                *szDescription,     // [ALL]      Page description
                *szVoiced;          // [ALL]      Page 'Voiced' property
   INT           iPosition,         // [SPEECH]   Speech clip starting position
                 iLength,           // [SPEECH]   Speech clip length
                 iParameters;       // [VARIABLE] Number of parameters in variable
   CONST TCHAR  *szVariable;        // [VARIABLE] Variable ID
   BOOL          bRecursive;        // [VARIABLE] Indicates the variable must not be recursively populated because it contains it's own ID
};

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    MEDIA ITEM
///
///   Represents either a video clip, a speech clip or a sound effect
/// /////////////////////////////////////////////////////////////////////////////////////////

// Identifies the types of media file available
//
enum MEDIA_ITEM_TYPE  { MIT_SOUND_EFFECT = 0,        // Individual sound effect file
                        MIT_VIDEO_CLIP   = 1,        // Video clip from the main video file
                        MIT_SPEECH_CLIP  = 2  };     // Spoken phrase from the voices language file

// Represents the start or end position of a video clip
//
struct MEDIA_POSITION
{
   UINT     iMinutes,            // Number of minutes
            iSeconds,            // Number of seconds
            iMilliseconds;       // Number of milliseconds
};

// Represents a media file or a clip from the audio/video file banks
//
struct MEDIA_ITEM
{
   // Identification
   UINT            iID,             // ID of the item within it's page
                   iPageID;         // ID of the page containing the item
   TCHAR*          szDescription;   // Description of the item (if any)

   // Properties
   MEDIA_ITEM_TYPE eType;           // Type of media item - SFX, video clip or speech clip

   // Video Clip: Properties
   MEDIA_POSITION  oStart,          // Offset into the video clips movie of the first frame of the video clip
                   oFinish;         // Offset into the video clips movie of the last frame of the video clip

   // Speech Clip: Properties
   UINT            iLength;         // Length of the speech clip, in milliseconds
   __int64         iPosition;       // Offset into the audio file marking the start of the speech clip, in milliseconds
};

// Represents a page of MediaItems
//
struct MEDIA_PAGE
{
   // Identification
   UINT            iPageID;
   TCHAR*          szTitle;

   // Properties
   MEDIA_ITEM_TYPE eType;
};

// Defines the meaning of each property in the sound effect definition file
//
enum  SOUND_EFFECT_INDEX { SEI_PRIORITY           = 0,
                           SEI_VOLUME             = 1,
                           SEI_FLAGS              = 2,
                           SEI_PITCH_MIN          = 3,
                           SEI_PITCH_MAX          = 4,
                           SEI_VOLUME_VARIATION   = 5,
                           SEI_DISTANCE           = 6,
                           SEI_DESCRIPTION        = 7   };

// Defines the meaning of each property in a video clips definition file
//
enum  VIDEO_CLIP_INDEX  { VCI_ID                  = 0, 
                          VCI_STREAM_ID           = 4, 
                          VCI_START_MINUTES       = 5, 
                          VCI_START_SECONDS       = 6, 
                          VCI_START_MILLISECONDS  = 7,
                          VCI_FINISH_MINUTES      = 8, 
                          VCI_FINISH_SECONDS      = 9, 
                          VCI_FINISH_MILLISECONDS = 10,
                          VCI_DESCRIPTION         = 11   };

enum MEDIA_PAGE_ID  { MPI_SPEECH_CLIPS  = 0,
                      MPI_VIDEO_AVATARS = 1,
                      MPI_VIDEO_WARES   = 2  };

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    OPERATOR_TYPE
///
///   For classifying operators, used only in the generation of expressions SCRIPT_TREEs 
/// /////////////////////////////////////////////////////////////////////////////////////////

// Defines each Operator
//
enum  OPERATOR_TYPE
{
   OT_EQUALITY           = 0,
   OT_EQUALITY_NOT       = 1,
   OT_GREATER_THAN       = 2,
   OT_LESS_THAN          = 3,
   OT_GREATER_THAN_EQUAL = 4,
   OT_LESS_THAN_EQUAL    = 5,
   OT_BITWISE_AND        = 6,
   OT_BITWISE_OR         = 7,
   OT_BITWISE_XOR        = 8,
   OT_LOGICAL_AND        = 9,
   OT_LOGICAL_OR         = 10,
   OT_ADD                = 11,
   OT_SUBTRACT           = 12,
   OT_MULTIPLY           = 13,
   OT_DIVIDE             = 14,
   OT_MODULUS            = 15,
   OT_BRACKET_OPEN       = 16,
   OT_BRACKET_CLOSE      = 17,
   OT_BITWISE_NOT        = 18,
   OT_MINUS              = 19,
   OT_LOGICAL_NOT        = 20,
   OT_UNRECOGNISED       = 21
};

// Defines the associativity of an operator
enum   OPERATOR_ASSOCIATIVITY   { OA_NONE = NULL, OA_LEFT = 1, OA_RIGHT = 2 };

// Operator stack used by the SHUNTING YARD algorithm
struct OPERATOR_STACK
{
   OPERATOR_TYPE    eItems[32];
   UINT             iCount;
};

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    PARAMETER
///
///   Represents a value found in the X3 game scripts. Works with the COMMAND structure.
///      Both are intermediaries in the conversion process
/// /////////////////////////////////////////////////////////////////////////////////////////

// DATA_TYPE_FLAGS -- Modifiers added to various types or values
//
#define   DTF_ENCODED           0x20000
#define   DTF_OBJECT_DATA       0x10000
#define   DTF_UNARY_OPERATOR    0x10000

//
// DATA_TYPE -- Defines the type element of a X3 script {type,value} pair
//
enum  DATA_TYPE  {   DT_NULL            = 0,   DT_UNKNOWN        = 1,   DT_VARIABLE      = 2,
                     DT_CONSTANT        = 3,   DT_INTEGER        = 4,   DT_STRING        = 5,
                     DT_SHIP            = 6,   DT_STATION        = 7,   DT_SECTOR        = 8,
                     DT_WARE            = 9,   DT_RACE           = 10,  DT_STATIONSERIAL = 11,
                     DT_OBJECTCLASS     = 12,  DT_TRANSPORTCLASS = 13,  DT_RELATION      = 14,
                     DT_OPERATOR        = 15,  DT_EXPRESSION     = 16,  DT_OBJECT        = 17,
                     DT_OBJECTCOMMAND   = 18,  DT_FLIGHTRETURN   = 19,  DT_DATATYPE      = 20,
                     DT_ARRAY           = 21,  DT_QUEST          = 22,
                     // New in Terran Conflict
                     DT_WING            = 23,  DT_SCRIPTDEF      = 24,  DT_WINGCOMMAND   = 25,
                     DT_PASSENGER       = 26,
                     // Live Types
                     DT_LIVE_CONSTANT  = DT_CONSTANT WITH DTF_ENCODED,
                     DT_LIVE_VARIABLE  = DT_VARIABLE WITH DTF_ENCODED,
                     DT_LIVE_OBJECT    = DT_OBJECT   WITH DTF_OBJECT_DATA,
                     DT_LIVE_SECTOR    = DT_SECTOR   WITH DTF_OBJECT_DATA,
                     DT_LIVE_SHIP      = DT_SHIP     WITH DTF_OBJECT_DATA,
                     DT_LIVE_STATION   = DT_STATION  WITH DTF_OBJECT_DATA,
                     DT_LIVE_WING      = DT_WING     WITH DTF_OBJECT_DATA,
                     DT_UNARY_OPERATOR = DT_OPERATOR WITH UGC_UNARY_OPERATOR    };

// Used to determine which COMMAND list a parameter is stored in
//
enum PARAMETER_TYPE { PT_DEFAULT, PT_INFIX, PT_POSTFIX };

//
// PARAMETER -- Holds a X3 type and it's associated value
//
struct PARAMETER
{
   PARAMETER_SYNTAX   eSyntax;     // Syntax for this parameter
   DATA_TYPE          eType;       // Data type

   INT                iValue;      // Value (as integer)
   TCHAR             *szValue,     // Value (as string)
                     *szBuffer;    // Value formatted for display

   UINT               iLineNumber; // [TRANSLATION] Line number containing the original <sourcevalue> tag
                                   // [GENERATION]  CodeEdit line number containing the script command
   UINT               iID;         // [GENERATION][EXPRESSIONS] Uniquely identifies a PARAMETER within a COMMAND. Always > 0. (used for cross-referencing postfix parameters)
};


// A variable size array that grows as required
struct PARAMETER_ARRAY
{
   UINT         iCount,     // Number of items currently in the array
                iSize;      // Current size of the array
   PARAMETER**  pItems;     // Item data
};


// Defines the positioning of a PARAMETER syntax object within a COMMAND
//
struct PARAMETER_INDEX
{
   UINT  iPhysicalIndex,   // The index by which the PARAMETER is stored, ie. 0, 1, 2, 3, 4, 5 etc. for every PARAMETER
         iLogicalIndex;    // The order in which they're displayed, eg. 2, 1, 3, 0, 4
};

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     OPERATION  DATA
///
///   Used for passing data to worker threads
/// /////////////////////////////////////////////////////////////////////////////////////////
struct FILE_SEARCH;
struct PROJECT_FILE;
struct OPERATION_PROGRESS;
enum AVL_TREE_SORT_KEY;
enum AVL_TREE_ORDERING;

// Identifies a threaded operation
//
enum  OPERATION_TYPE   {  OT_LOAD_GAME_DATA,             // Loading game data

                          OT_LOAD_LANGUAGE_FILE,         // Loading a language file
                          OT_LOAD_SCRIPT_FILE,           // Loading a script
                          OT_LOAD_PROJECT_FILE,          // Loading a project file

                          OT_SAVE_LANGUAGE_FILE,         // Saving a language file
                          OT_SAVE_SCRIPT_FILE,           // Saving a script
                          OT_SAVE_PROJECT_FILE,          // Saving a project file

                          OT_VALIDATE_SCRIPT_FILE,       // Generates/Validates script code
                          
                          OT_SEARCH_FILE_SYSTEM,         // FileDialog file search
                          OT_SEARCH_SCRIPT_CALLS,        // External ScriptCall dependencies search

                          OT_SUBMIT_BUG_REPORT,          // Bug report submission
                          OT_SUBMIT_CORRECTION     };    // Correction submission


// Identifies the possible results of a function or threaded operation
//
enum  OPERATION_RESULT {  OR_SUCCESS = 0,      // Operation succeeded (there may have been errors, but they were ignored by the user)
                          OR_FAILURE = 1,      // Operation failed due to a critical error
                          OR_ABORTED = 2   };  // Operation was aborted by the user (due to errors)
                                               // NB: printOperationResultToOutputDialog() relies on these values being 0, 1 and 2.

// Options dialog tests
//
#define      ODT_NONE                 0
#define      ODT_GENERATION           1
#define      ODT_VALIDATION           2


// Advanced file loading options
// 
struct LOADING_OPTIONS
{
   UINT          iRecursionDepth;       // Recursion depth

   BOOL          eCompilerTest;         // Type of compiler test to perform
   BOOL          bBatchCompilerTest;    // Whether to perform test in batch
   TCHAR         szOriginalPath[MAX_PATH];
};


// The basic data passed to a worker thread. More complex operations extend this object
//
struct OPERATION_DATA
{
   // Operation
   OPERATION_RESULT    eResult;             // Operation result
   OPERATION_TYPE      eType;               // Operation identification
   ERROR_QUEUE*        pErrorQueue;         // Operation error queue

   // Thread
   HANDLE              hThread;             // Thread handle
   HWND                hParentWnd;          // ErrorReporting/Completion notification

   // Tracking
   OPERATION_PROGRESS* pProgress;           // Operation progress
   UINT                iID;                 // Unique operation ID

   // Testing
   UINT                iTestCaseID;         // [TESTING] TestCase number (if the operation is being used as a TestCase)

};

// Worker thread data for load document and save document operations
//
struct DOCUMENT_OPERATION : OPERATION_DATA
{
// Common
   CONST TCHAR*       szFullPath;           //        Convenience pointer to GameFile path
   BOOL               bActivateOnLoad;      // [LOAD] Whether to switch to document once loaded
   LPARAM             pDocument;            // [SAVE] Document being saved
   GAME_FILE*         pGameFile;            //        Operation data
   LOADING_OPTIONS    oAdvanced;            //        Advanced options or testing
   
// Scripts
   HWND               hCodeEdit;            // [SAVE] CodeEdit window handle
};


// Worker thread data for bug and correction submissions
//
struct SUBMISSION_OPERATION : OPERATION_DATA     
{
   TCHAR*             szCorrection;            // Correction submission
};


// Worker thread data for the FileDialog search operation
//
struct SEARCH_OPERATION : OPERATION_DATA
{
   AVL_TREE_SORT_KEY   eSortKey;              // Primary sorting key
   AVL_TREE_ORDERING   eDirection;            // Direction of sorting
   FILE_FILTER         eFilter;               // Desired file filter
   TCHAR               szFolder[MAX_PATH];    // Search folder
   FILE_SEARCH*        pFileSearch;           // Search results (NB: Owned by the FileDialog, not the worker thread)
};


// Worker thread for external script-call searching operation
//
struct SCRIPTCALL_OPERATION : OPERATION_DATA
{
   AVL_TREE*           pCallersTree;               // ScriptDependencies representing each calling script
   TCHAR               szScriptName[MAX_PATH];     // Name of the script to search against
};

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   OPERATION_PROGRESS
///
///   Tracks the progress of worker thread operations, for use by the progress dialog
/// /////////////////////////////////////////////////////////////////////////////////////////

// Represents progress of a stage within an operation
//
struct PROGRESS_STAGE
{
   UINT  iProgress,     // Current progress
         iMaximum,      // Value representing 100% progress
         iResourceID;   // Resource ID of stage description string
};

// Represents the current progress of an operation
//
struct OPERATION_PROGRESS
{
   LIST*            pStages;           // Individual stages
   PROGRESS_STAGE*  pCurrentStage;     // Convenience pointer for the current stage
   UINT             iStageIndex;       // Zero-based index of the current stage
   BOOL             bComplete;         // Indicates whether the operation is complete
};

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                      PREFERENCES
///
///   Defines the various application preferences
/// ////////////////////////////////////////////////////////////////////////////////////////

// The available game languages
//
enum APP_LANGUAGE     { AL_ENGLISH,      // 
                        AL_FRENCH,       // 
                        AL_GERMAN,       // 
                        AL_ITALIAN,      // 
                        AL_POLISH,       // 
                        AL_RUSSIAN,      // 
                        AL_SPANISH,   }; // 

enum INTERFACE_LANGUAGE { IL_ENGLISH,
                          IL_GERMAN };
                          //IL_RUSSIAN   };

// Uniquely Identifies each page in the preferences property sheet
//
enum PREFERENCES_PAGE  { PP_GENERAL,           // General page
                         PP_FOLDERS,           // Folders page
                         PP_APPEARANCE,        // Appearance
                         PP_SYNTAX,            // Syntax page
                         PP_MISC    };         // Miscellaneous page

// Possible results of displaying the preferences dialog to the user
//
enum PREFERENCES_SHEET_RESULT  { PSR_NO_CHANGE              = 0,
                                 PSR_APP_LANGUAGE_CHANGED   = 1,
                                 PSR_GAME_LANGUAGE_CHANGED  = 2,
                                 PSR_GAME_FOLDER_CHANGED    = 4   };

// Indicates the result of attempting to load the application preferences
// 
enum PREFERENCES_STATE { PS_ERROR,          // Preferences for a newer version were present or corrupt
                         PS_SUCCESS,        // Valid Preferences were loaded OK
                         PS_FIRST_RUN,      // No Preferences were found
                         PS_UPGRADED   };   // Preferences from a previous version were over-written
                         
// Various windows used by the application
//
enum APPLICATION_WINDOW { AW_MAIN,                    // Main window
                          AW_PROPERTIES,              // Properties dialog
                          AW_PROJECT,                 // Project dialog
                          AW_BROWSER,                 // Open File dialog
                          AW_FIND,                    // Find dialog
                          AW_DOCUMENT,                // Active Document
                          AW_DOCUMENTS_CTRL,          // Documents tab control
                          AW_OUTPUT,                  // Output dialog
                          AW_PROGRESS,                // Progress dialog
                          AW_SEARCH,                  // Tabbed search dialog
                          AW_TOOLBAR,                 // Toolbar control
                          AW_STATUSBAR    };          // Statusbar control

// Indicates the active SearchResults dialog
//
enum RESULT_TYPE  { RT_COMMANDS, RT_GAME_OBJECTS, RT_SCRIPT_OBJECTS, RT_NONE };  

// Application versions
//
enum APPLICATION_VERSION { AV_BETA_1          = 1,       // Initial release
                           AV_BETA_2          = 2,       // Change of windows and state
                           AV_BETA_2_UPDATE_3 = 3,       // Change from window rectangles to pane ratios
                           AV_BETA_2_UPDATE_4 = 4,       // Implemented APP_LANGUAGE and GAME_LANGUAGE
                           AV_BETA_2_UPDATE_5 = 5,       // Added 'Report string merge warnings'
                           AV_BETA_2_UPDATE_8 = 6,       // [UNRELEASED] Added 'X-Universe Forum Username'
                           AV_BETA_3          = 7,       // Added Transparent PropertiesDlg. Added ProjectsDlg visibilty. Changed window ratios to pixel sizes.
                           AV_BETA_3_UPDATE_1 = 8,       // Added some new tutorial windows
                           AV_BETA_3_UPDATE_4 = 9  };    // Added FindDialog rectangle

// Game data folder state
//
enum GAME_FOLDER_STATE   { GFS_INVALID,         // Game folder does not contain the required game data files
                           GFS_VALID,           // Game folder contains required game data files
                           GFS_INCOMPLETE  };   // Game folder contains required files but the game version cannot be identified

// Defines the available tutorial windows
//
enum TUTORIAL_WINDOW  { TW_GAME_FOLDER = 1, TW_FILE_OPTIONS, TW_GAME_DATA, TW_SCRIPT_COMMANDS, TW_GAME_OBJECTS, TW_OPEN_FILE, TW_SCRIPT_OBJECTS, TW_PROJECTS, TW_SCRIPT_EDITING };

// Defines the type of suggestions displayed in a CodeEdit
//
enum SUGGESTION_TYPE   { CST_NONE, CST_VARIABLE, CST_COMMAND, CST_GAME_OBJECT, CST_SCRIPT_OBJECT };

// Application Preferences - Contains the current values of application wide settings
//
struct PREFERENCES
{
   /// Program States
   GAME_VERSION      eGameVersion;               // Game version of the currently loaded game data
   GAME_FOLDER_STATE eGameFolderState;           // [GFS_VALID]      Game folder contains game data files
                                                 // [GFS_INVALID]    Game folder does not contain game data files
                                                 // [GFS_INCOMPLETE] Used during calculations only, never used outside the preferences dialog

   TCHAR             szLastFolder[MAX_PATH];     // The last folder opened in the file dialog (Guaranteed to have a trailing backslash) 
   FILE_FILTER       eLastFileFilter;            // Last file filter used in the file dialog
   TCHAR             szFindText[MAX_PATH];       // Last search text
   RESULT_TYPE       eSearchDialogTab;           // Current SearchDialog tab
   UINT              eSearchDialogFilters[3];    // Currently selected SearchDialog filters

   BOOL              bOutputDialogVisible,       // Whether the Output dialog is visible
                     bProjectDialogVisible,      // Whether the Project dialog is visible
                     bPropertiesDialogVisible,   // Whether the Properties dialog is visible
                     bSearchDialogVisible,       // Whether the Search dialog is visible
                     bMainWindowMaximised,       // Whether the main window is maximised

                     bTutorialsRemaining[TUTORIAL_COUNT];    // Identifies which tutorial windows have not yet been shown

   /// Window positions
   RECT              rcPropertiesDialog,         // Document properties window rectangle
                     rcFindDialog,               // Find text dialog
                     rcMainWindow;               // Main window rectangle
   SIZE              siFileDialog;               // OpenFile dialog window size

   UINT              iProjectDialogSplit,        // Project dialog workspace pane size
                     iOutputDialogSplit,         // Output dialog workspace pane size
                     iSearchDialogSplit;         // Search results dialog workspace pane size   

   /// Page: General
   APP_LANGUAGE      eAppLanguage;               // Resource library language 
   BOOL              bPreserveSession,           // Re-open the documents from the previous instance when program loads
                     bCodeIndentation,           // Indent CodeEdit text according to syntax
                     bScriptCodeMacros,          // Enable script-code macros
                     bTransparentProperties,     // Transparent properties dialog
                     bTutorialMode,              // Display dialog tutorial windows on first use
                     bUseDoIfSyntax,             // Use the 'do if' syntax instead of 'skip if not'
                     bEditorTooltips,            // Display tooltips in the CodeEdit
                     bSearchResultTooltips,      // Display tooltips in results dialog
                     bSuggestions[5],            // Whether to display suggestions for commands, game objects, script objects and variables
                     bUseSystemDialog;           // Whether to use the system dialog instead of the file browser
   UINT              iEditorTooltipDelay,        // Delay time for CodeEdit tooltips
                     iSearchResultTooltipDelay;  // Delay time for results dialog tooltips

   /// Page: Folders
   GAME_LANGUAGE     eGameLanguage;                 // Language of game data to load
   TCHAR             szGameFolder[MAX_PATH];        // The folder containing the game executable. It is guaranteed to have a trailing backslash.
   BOOL              bReportGameStringOverwrites,   // Produce individual warnings when a supplementary language file entry overwrites another
                     bReportIncompleteGameObjects,  // Produce individual warnings when game objects have missing property strings
                     bReportMissingSubStrings;      // Produce individual warnings when a gamestring substring is missing

   /// Page: Appearance/Syntax
   COLOUR_SCHEME     oColourScheme;              // Current colour scheme containing colour of all display elements

   /// Page: Misc
   BOOL              bSubmitBugReports,          // Submit crash reports containing the console log file
                     bPassiveFTP,                // Whether to use passive FTP or not
                     bAutomaticUpdates;          // Contact the update server and download the latest version
   ERROR_HANDLING    eErrorHandling;             // Defines how to proceed after an error
   TCHAR             szForumUserName[32];        // X-Universe forum username
};

// Dialog data for the Preferences PropertySheet
//
struct PREFERENCES_DATA
{
// Display
   PROPSHEETHEADER   oHeader;                        // Sheet creation data
   PROPSHEETPAGE     oPage[PREFERENCES_PAGES];       // Page creation data
   HWND              hTooltips[PREFERENCES_PAGES];   // Page Tooltips
   BOOL              bInitialised;                   // [HACK] Flag used to re-positioned the dialog before display (Don't get a chance with modal dialogs)

// Data
   PREFERENCES*      pPreferencesCopy;               // Working copy of the application preferences [Dialog does not edit preferences directly]
   LIST*             pColourSchemeList;              // Available colour schemes
   SCRIPT_FILE*      pScriptFile;                    // 'Preview' CodeEdit ScriptFile
};

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                  PROJECT FILE
///
///   Contains the internal data of a project, and represents a project file on disc
/// ////////////////////////////////////////////////////////////////////////////////////////
struct XML_TREE;

// Represents the three types of project file (and the root node, for the dialog treeview)
//
enum  PROJECT_FOLDER    { PF_SCRIPT, PF_LANGUAGE, PF_MISSION, PF_ROOT };

// Represents a project variable
//
struct PROJECT_VARIABLE
{
   TCHAR*      szName;
   UINT        iValue;
};

// Represents a project file stored in disc
//
struct PROJECT_FILE : GAME_FILE
{
   LIST*       pFileList[3];           // List of StoredDocuments representing files in each folder of the project
   AVL_TREE*   pVariablesTree;         // Local/global variables tree
   XML_TREE*   pXMLTree;               // XML-Tree
};


// Used for storing document filenames in the registry
//
struct  STORED_DOCUMENT
{
   FILE_ITEM_FLAG  eType;                  // Document type
   TCHAR           szFullPath[MAX_PATH];   // Full filepath of document
};

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                      RICH TEXT
///
///   Formatted text used for displaying Language file strings, ResultsDialog entries and RichTooltips
/// ////////////////////////////////////////////////////////////////////////////////////////

// Compatibility of a RichEdit message based on the tags it contains
//
enum MESSAGE_COMPATIBILITY { LMC_LOGBOOK, LMC_MESSAGE, LMC_CUSTOM_MENU };

// Alignment of a RichEdit paragraph
//
enum PARAGRAPH_ALIGNMENT   { PA_LEFT = 1, PA_RIGHT = 2, PA_CENTRE = 3, PA_JUSTIFY = 4 };     // NB: Equivilent to PARAFORMAT 'Alignment' constants and RICHTEXT_TAG tags

// Represents the two kinds of player ranking available. Used in the [rank] tag
//
enum RANK_TYPE             { RT_FIGHT, RT_TRADE };

// Defines the type of a RichEdit text item
//
enum RICHITEM_TYPE   { RIT_TEXT = 1, RIT_BUTTON = 2, RIT_IMAGE = 3 };    // NB: Equivilent to EM_SELECTIONTYPE return types SEL_TEXT and SEL_OBJECT

// Defines whether a RichText object is really a LanguageMessage object or just a RichText object
//
enum RICHTEXT_TYPE   { RTT_RICH_TEXT, RTT_LANGUAGE_MESSAGE };

// Represents the result of a comparison between attributes of two rich text characters
//
enum RICHTEXT_FORMATTING   { RTF_FORMATTING_EQUAL, RTF_CHARACTER_CHANGED, RTF_PARAGRAPH_CHANGED, RTF_BOTH_CHANGED };

// Drawing flags returned by the drawRichTextInRectangle function
//
enum RICHTEXT_DRAWING      { RTD_ENTIRE, RTD_PARTIAL };


// Represents formatted text or an OLE button object
//
struct RICH_ITEM
{
   TCHAR               *szText,        // Item text or button caption
                       *szID;          // [BUTTONS] The value identifying the button
   BOOL                 bBold,         // [TEXT]    This text is in bold
                        bUnderline,    // [TEXT]    This text is underlined
                        bItalic;       // [TEXT]    This text is in italics
   GAME_TEXT_COLOUR     eColour;       // [TEXT]    Text colour
   UINT                 iImageType;    // [IMAGE]   IMAGE_BITMAP or IMAGE_ICON

   STRING_TYPE          eTextType;     // Type of string - must be ST_DISPLAY
   RICHITEM_TYPE        eType;         // Type of item, button or text
};

// Represents a paragraph in a rich text message
//
struct RICH_PARAGRAPH
{
   PARAGRAPH_ALIGNMENT  eAlignment;    // Text alignment
   LIST*                pItemList;     // List of text and buttons in this paragraph
};

// A series of paragraphs containing formatted RichText which is used to display language strings and various UI strings
//
struct RICH_TEXT
{ 
   LIST*                  pParagraphList;     // List of paragraphs in the message

   RICHTEXT_TYPE          eType;              // Whether object is a RICH_TEXT object or a LANGUAGE_MESSAGE object
   MESSAGE_COMPATIBILITY  eCompatibility;     // Compatbility of the formatting throughout the message
};

// RichText attributes, used as state information for generating plain text from rich text
// 
struct RICHTEXT_ATTRIBUTES
{
   BOOL                bBold,         // Text is currently in bold
                       bUnderline,    // Text is currently underlined
                       bItalic;       // Text is currently italicised
   GAME_TEXT_COLOUR    eColour;       // Current text colour

   PARAGRAPH_ALIGNMENT  eAlignment;   // Current alignment of the text
};


// A formatted language file entry possibly consisting of various extra properties
// 
struct LANGUAGE_MESSAGE : RICH_TEXT
{
   // [TITLE] and [AUTHOR] Properties
   TCHAR      *szAuthor,               // Author of the message (ST_DISPLAY)
              *szTitle;                // Title of the message (ST_DISPLAY)

   // [RANK] Properties
   BOOL       bCustomRank;             // Whether 'eRankType' and 'szRankTitle' are valid
   RANK_TYPE  eRankType;               // Type of rank specified by 'szRankTitle'
   TCHAR     *szRankTitle;             // Rank title. (ST_DISPLAY)  Exactly what this specifies i'm not sure. Possibly rank required.

   // [ARTICLE] Properties
   BOOL       bArticle;                // Message has an [ARTICLE] tag
   UINT       iArticleState;           // [ARTICLE] state. Whatever that is.

   // [TEXT] Properties
   UINT       iColumnCount,            // Number of columns
              iColumnWidth,            // Width of columns
              iColumnSpacing;          // Spacing between the columns

   BOOL       bCustomWidth,            // Whether 'iColumnWidth' is valid
              bCustomSpacing;          // Whether 'iColumnSpacing' is valid
};


// Stores a position when drawing RichText
//
struct RICHTEXT_POSITION
{
   LIST_ITEM*  pCurrentItem;     // ListItem containing the current item
   UINT        iCharIndex;       // Character offset into the text of the current item
};

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    RICHTEXT TOKENISER
///
///   Parsing object for parsing text containing formatting tags into RichText
/// ////////////////////////////////////////////////////////////////////////////////////////

// Identifies each supported RichEdit tag
//
enum RICHTEXT_TAG   { RTT_NONE,    RTT_LEFT,   RTT_RIGHT,     RTT_CENTRE,    RTT_JUSTIFY,    // Left,Right,Centre+Justify are equivilent to PARAGRAPH_ALIGNMENT and must not be changed.
                      RTT_ARTICLE, RTT_AUTHOR, RTT_TEXT,      RTT_TITLE,     RTT_RANKING, 
                      RTT_BOLD,    RTT_ITALIC, RTT_UNDERLINE, RTT_SELECT,    RTT_IMAGE,
                      RTT_BLACK,   RTT_BLUE,   RTT_CYAN,      RTT_DEFAULT,   RTT_GREEN,   RTT_ORANGE, RTT_PURPLE, RTT_RED, RTT_WHITE, RTT_YELLOW };

// Identifies the type of a RichText token
// 
enum RICHTEXT_TOKEN_CLASS  { RTC_COLOUR_TAG,       // The colour tags, plus RTT_DEFAULT
                             RTC_FORMAT_TAG,       // RTT_BOLD, RTT_ITALIC, RTT_UNDERLINE
                             RTC_ALIGNMENT_TAG,    // RTT_LEFT, RTT_RIGHT, RTT_CENTRE, RTT_JUSTIFY
                             RTC_MESSAGE_TAG,      // RTT_ARTICLE, RTT_AUTHOR, RTT_TEXT, RTT_TITLE, RTT_RANKING
                             RTC_TOOLTIP_TAG,      // RTT_IMAGE,
                             RTC_BUTTON_TAG,       // RTT_SELECT
                             RTC_TEXT };           // Object is Text and therefore not a tag

// Parsing states for parsing RichText message-tag properties
//
enum RICHTEXT_TOKEN_STATE { RTS_PROPERTY_NAME, RTS_COLUMN_COUNT, RTS_COLUMN_WIDTH, RTS_COLUMN_SPACING, RTS_BUTTON_ID, RTS_IMAGE_TYPE, RTS_IMAGE_ID, RTS_RANK_TYPE, RTS_RANK_TITLE, RTS_ARTICLE_STATE };

// Tokeniser for generating RichText
//
struct RICHTEXT_TOKENISER
{
   RICHTEXT_TOKEN_CLASS  eClass;              // Current object's type - either Text or the group of tag
   TCHAR*                szSourceText;        // ST_INTERNAL copy of the source text being parsed
   CONST TCHAR*          szCurrentPosition;   // Current position within the source text

   // Data
   RICHTEXT_TAG          eTag;               // [TAG]  Tag
   TCHAR*                szText;             // [TEXT] Text

   // Attributes
   BOOL                  bClosingTag,        // [TAG]  Whether tag is a closing or opening tag
                         bSelfClosingTag;    // [TAG]  Whether tag is a self closing tag
   UINT                  iCount;             // [TEXT] Length of current text, in characters
};

// Stack item used by RichText Tokeniser
//
struct RICHTEXT_STACK_ITEM
{
   RICHTEXT_TAG          eTag;            // Tag
   RICHTEXT_TOKEN_CLASS  eClass;          // Tag class
};

/// //////////////////////////////////////////////////////////////////////////////////////////
///                                  SCRIPT DEPENDENCY
///
///   Represents an external script call (Used for analysis by the user)
/// //////////////////////////////////////////////////////////////////////////////////////////

// Represents an external script call
//
struct SCRIPT_DEPENDENCY
{
   TCHAR  szScriptName[128];        // Name of the script, without extension
   UINT   iCalls;                   // Number of times the script is called
};


/// ////////////////////////////////////////////////////////////////////////////////////////
///                                     SCRIPT  FILES
///
///   Represents an X3 game script
/// ////////////////////////////////////////////////////////////////////////////////////////
struct XML_TREE;
struct XML_TREE_NODE;
struct XML_TREE_ITERATOR;
struct COMMAND_NODE;

// Helper for generating MSCI Script XML. Defines the useful structural features referenced throughout XML generation
//
struct XML_SCRIPT_LAYOUT
{
   XML_TREE_NODE  *pScriptNode,                // <script> node
                  *pNameNode,                  // <name> node
                  *pDescriptionNode,           // <description> node
                  *pVersionNode,               // <version> node
                  *pEngineVersionNode,         // <engineversion> node
                  *pArgumentsNode,             // <arguments> node
                  *pSourceTextNode,            // <sourcetext> node
                  *pCodeArrayNode,             // <codearray> node
                  *pSignatureNode,             // <signature> node
                  *pLiveDataNode,              // ->  <codearray> sub-node containing the live data flag
                  *pVariableNamesBranch,       // ->  <codearray> sub-node containing the variable names branch
                  *pArgumentsBranch,           // ->  <codearray> sub-node containing the arguments branch
                  *pStandardCommandsBranch,    // ->  <codearray> sub-node containing the standard commands branch
                  *pAuxiliaryCommandsBranch,   // ->  <codearray> sub-node containing the auxiliary commands branch
                  *pCommandIDNode;             // ->  <codearray> sub-node containing the Command ID
};


// Represents an argument variable used in an X3 game script
//
struct ARGUMENT
{
   TCHAR              *szName,            // Name
                      *szDescription;     // Description

   UINT               iID;                // ID
   PARAMETER_SYNTAX   eType;              // Type
};

// Represents two lists of COMMANDs - one for standard commands and the other for auxiliary
//
struct COMMAND_STREAM
{
   LIST    *pStandardList,                // Standard COMMANDs
           *pAuxiliaryList;               // Auxiliary COMMANDs
};

// Used for translating MSCI XML script into a list of translated COMMANDs
// 
struct SCRIPT_TRANSLATOR
{
   // Variables + Arguments list
   LIST              *pVariablesList;          // VariableNames list containing arguments and variables

   // XML-Tree and important nodes
   XML_TREE          *pXMLTree;                // XMLTree storing the contents of parsed the XML file
   XML_SCRIPT_LAYOUT  oLayout;                 // Stores the important nodes within a script

   // Input / Output
   COMMAND_NODE      *pStandardIterator,       // <codearray> standard command nodes iterator
                     *pAuxiliaryIterator;      // <codearray> auxiliary command nodes iterator
   LIST              *pOutputList;             // COMMANDs that have been successfully translated from XML
};


// Used for generating a list of COMMANDS from script edit text
//
struct SCRIPT_GENERATOR
{
   // Variables + Arguments list
   LIST              *pVariablesList;        // VariableNames list containing arguments and variables

   // XMLTree and important nodes
   XML_TREE          *pXMLTree;              // XML Tree used generating the XML file
   XML_SCRIPT_LAYOUT  oLayout;               // Stores the important nodes within a script

   // Input/Output COMMAND lists
   LIST              *pInputList;            // COMMANDs that have been successfully parsed from the CodeEdit but not processed
   COMMAND_STREAM    *pOutputStream;         // COMMANDs that have been processed ready for output to XML

   // Macro iterators
   UINT               iIteratorID;           // Current Macro iterator ID
};

// Defines the two types of operations available to ScriptFiles
// 
enum SCRIPT_FILE_OPERATION  { SFO_TRANSLATION, SFO_GENERATION };

//
// Encapsulates an X3 game script - it holds all it's properties and pointers to the code itself
//
struct SCRIPT_FILE : GAME_FILE
{
   // Script Properties + Arguments
   TCHAR           *szScriptname,            // Name of the script
                   *szDescription,           // Description of the script
                   *szCommandID;             // Command ID the script is implementing
   AVL_TREE        *pArgumentTreeByID;       // Script Arguments as displayed to the user. A separate copy is used for translation and generation

   UINT             iVersion;                // Script version
   GAME_VERSION     eGameVersion;            // Game version of the script
   BOOL             bSignature,              // Whether script is signed by Egosoft or not
                    bLiveData,               // Whether the script contains 'live data'
                    bModifiedOnLoad;         // Whether the script name was changed during loading
   
// Script Conversion
   SCRIPT_TRANSLATOR*  pTranslator;
   SCRIPT_GENERATOR*   pGenerator;
};

// Identifies COMMAND lists in the Generator and Translator
// 
enum COMMAND_LIST    { CL_OUTPUT,            // Commands that were successfully translated or generated
                       CL_INPUT    };        // Commands that have been parsed from XML or the CodeEdit


// Defines whether a value is the MSCI script or internal equivilent 
//       For most values, these are the same thing.
//
enum SCRIPT_VALUE_TYPE  { SVT_INTERNAL,      // Value represents the internal value, usually a GameString ID or enumeration value
                          SVT_EXTERNAL };    // Value represents the value used in an MSCI script


// Defines the types of ReturnObject available
//
enum RETURN_OBJECT_TYPE  { ROT_VARIABLE, ROT_CONDITIONAL };


// Encapsulates a ReturnObject
//
struct RETURN_OBJECT
{
   RETURN_OBJECT_TYPE  eType;       // ReturnObject type - conditional branch or a variable

   // Conditional
   CONDITIONAL_ID  eConditional;    // Conditional branching instruction
   RETURN_TYPE     eReturnType;     // Type of jump (if any)
   UINT            iDestination;    // Destination line number 

   // Variable
   UINT            iVariableID;     // ID of the variable represented by the ReturnValue
};

/// //////////////////////////////////////////////////////////////////////////////////////////
///                                  STRING DEPENDENCY
///
///   Represents a game string references by a command in a script
/// //////////////////////////////////////////////////////////////////////////////////////////

// Represents a game string reference
//
struct STRING_DEPENDENCY
{
   UINT          iPageID,        // GameString page ID
                 iStringID;      // GameString string ID

   GAME_STRING*  pGameString;    // Matching GameString if it exists, otherwise NULL
   RICH_TEXT*    pRichText;      // RichText GameString if appropriate, otherwise NULL
};

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                     SUB-STRINGS
///
///   Represents the various types of special substrings used in GameStrings and iterates through them
/// ////////////////////////////////////////////////////////////////////////////////////////

// Type of substring
//
enum SUBSTRING_TYPE  { SST_TEXT,          // Text
                       SST_COMMENT,       // Comment - enclosed with ( )
                       SST_LOOKUP,        // Lookup in the format {x,y} or {,y}
                       SST_MISSION,       // Mission director string in the format {alice@bob}
                       SST_CONSTANT };    // Constant in the format {x}

// Substring encapsulation and iteration
//
struct SUBSTRING
{
   CONST TCHAR    *szSource,  // Source text containing all the sub-strings
                  *szMarker;  // Current processing position within the source text

   SUBSTRING_TYPE  eType;                       // SubString type
   TCHAR           szText[MAX_STRING];  // SubString text
   UINT            iCount;                      // Length of SubString, in characters
};


/// ////////////////////////////////////////////////////////////////////////////////////////
///                                   STRING CONVERSION
///
///   Converts the special characters in strings for display or export
/// ////////////////////////////////////////////////////////////////////////////////////////

// Identifies the various special phrase conversions that can be requested
//
enum STRING_CONVERSION_FLAG  { SCF_EXPAND_NEWLINE           = 0x000001,  SCF_CONDENSE_NEWLINE         = 0x000002,
                               SCF_EXPAND_APOSTROPHE        = 0x000004,  SCF_CONDENSE_APOSTROPHE      = 0x000008,
                               SCF_EXPAND_AMPERSAND         = 0x000010,  SCF_CONDENSE_AMPERSAND       = 0x000020,
                               SCF_EXPAND_GREATER_THAN      = 0x000040,  SCF_CONDENSE_GREATER_THAN    = 0x000080,
                               SCF_EXPAND_LESS_THAN         = 0x000100,  SCF_CONDENSE_LESS_THAN       = 0x000200, 
                               SCF_EXPAND_QUOTES            = 0x000400,  SCF_CONDENSE_QUOTES          = 0x000800,
                               SCF_EXPAND_PERCENT           = 0x001000,  SCF_CONDENSE_PERCENT         = 0x002000,
                               SCF_EXPAND_ADDITION          = 0x004000,  SCF_CONDENSE_ADDITION        = 0x008000,

                               SCF_EXPAND_SQUARE_BRACKET    = 0x010000,  SCF_CONDENSE_SQUARE_BRACKET  = 0x020000,
                               SCF_EXPAND_CURLY_BRACKET     = 0x040000,  SCF_CONDENSE_CURLY_BRACKET   = 0x080000,
                               SCF_EXPAND_BRACKET           = 0x100000,  SCF_CONDENSE_BRACKET         = 0x200000,

                               SCF_EXPAND_TAB               = 0x400000,  SCF_CONDENSE_TAB             = 0x400000  };

// Identifies the various special phrases available
//
enum SPECIAL_CHARACTER_TYPE {  SCT_TEXT,

                               SCT_NEWLINE,               SCT_EXPANDED_NEWLINE,                  SCT_APOSTROPHE,            SCT_EXPANDED_APOSTROPHE,
                               SCT_AMPERSAND,             SCT_EXPANDED_AMPERSAND,                SCT_GREATER_THAN,          SCT_EXPANDED_GREATER_THAN,
                               SCT_LESS_THAN,             SCT_EXPANDED_LESS_THAN,                SCT_QUOTES,                SCT_EXPANDED_QUOTES,
                               SCT_PERCENT,               SCT_EXPANDED_PERCENT,                  SCT_ADDITION,              SCT_EXPANDED_ADDITION,

                               SCT_SQUARE_BRACKET_OPEN,   SCT_EXPANDED_SQUARE_BRACKET_OPEN,      SCT_SQUARE_BRACKET_CLOSE,  SCT_EXPANDED_SQUARE_BRACKET_CLOSE,
                               SCT_CURLY_BRACKET_OPEN,    SCT_EXPANDED_CURLY_BRACKET_OPEN,       SCT_CURLY_BRACKET_CLOSE,   SCT_EXPANDED_CURLY_BRACKET_CLOSE,
                               SCT_BRACKET_OPEN,          SCT_EXPANDED_BRACKET_OPEN,             SCT_BRACKET_CLOSE,         SCT_EXPANDED_BRACKET_CLOSE,

                               SCT_TAB,                   SCT_EXPANDED_TAB    };


// Handles converting sensitive characters within strings into the safety phrases used for XML output or script display
//
struct STRING_CONVERTER
{
   CONST TCHAR             *szSource,            // Input string
                           *szPosition;          // Input string iterator

   SPECIAL_CHARACTER_TYPE  eType;                // Current phrase type
   TCHAR                   szText[MAX_STRING];   // Current phrase text
   UINT                    iLength;              // Current phrase length, in characters

   BOOL                    bConversionPerformed; // Output flag indicating whether any conversion was performed
   TCHAR                   szOutput[MAX_STRING]; // Output string
   UINT                    iOutputLength;        // Output string length, in characters
};

// Defines commonly used string conversions 
#define  SPC_SCRIPT_DISPLAY_TO_EXTERNAL    (SCF_CONDENSE_APOSTROPHE | SCF_EXPAND_AMPERSAND   | SCF_EXPAND_GREATER_THAN   | SCF_EXPAND_LESS_THAN   | SCF_EXPAND_QUOTES) // WITH SCF_EXPAND_BRACKET)
#define  SPC_SCRIPT_EXTERNAL_TO_DISPLAY    (SCF_EXPAND_APOSTROPHE   | SCF_CONDENSE_AMPERSAND | SCF_CONDENSE_GREATER_THAN | SCF_CONDENSE_LESS_THAN | SCF_CONDENSE_QUOTES) // WITH SCF_CONDENSE_BRACKET)

#define  SPC_LANGUAGE_INTERNAL_TO_EXTERNAL (SCF_EXPAND_APOSTROPHE   | SCF_EXPAND_AMPERSAND   | SCF_EXPAND_GREATER_THAN   | SCF_EXPAND_LESS_THAN   | SCF_EXPAND_QUOTES   | SCF_EXPAND_NEWLINE   | SCF_EXPAND_ADDITION   | SCF_EXPAND_TAB)    // SCF_EXPAND_PERCENT   |
#define  SPC_LANGUAGE_EXTERNAL_TO_INTERNAL (SCF_CONDENSE_APOSTROPHE | SCF_CONDENSE_AMPERSAND | SCF_CONDENSE_GREATER_THAN | SCF_CONDENSE_LESS_THAN | SCF_CONDENSE_QUOTES | SCF_CONDENSE_NEWLINE | SCF_CONDENSE_ADDITION | SCF_CONDENSE_TAB)  // SCF_CONDENSE_PERCENT |

#define  SPC_LANGUAGE_INTERNAL_TO_DISPLAY  (SCF_CONDENSE_SQUARE_BRACKET | SCF_CONDENSE_CURLY_BRACKET | SCF_CONDENSE_BRACKET)
#define  SPC_LANGUAGE_DISPLAY_TO_INTERNAL  (SCF_EXPAND_SQUARE_BRACKET   | SCF_EXPAND_CURLY_BRACKET   | SCF_EXPAND_BRACKET)

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    SUGGESTION RESULT
///
///   Convenience object for extracting search results
/// /////////////////////////////////////////////////////////////////////////////////////////
struct VARIABLE_NAME;

// Used by the CodeEdit for suggestions, and the search dialog for tooltips
//
union SUGGESTION_RESULT
{
   CONST COMMAND_SYNTAX*  asCommandSyntax;
   OBJECT_NAME*           asObjectName;
   VARIABLE_NAME*         asVariableName;
   LPARAM                 asPointer;
};

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                      T - FILES
///
///   Holds details of the TypeFile that should be loaded
/// /////////////////////////////////////////////////////////////////////////////////////////

//
// Represents a T-File
//
struct TFILE
{
   CONST TCHAR*   szFilename;
   UINT           iMainType;
   DATA_TYPE      eType;
};


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     TEXT STREAM
///
///   A text 'streaming' object for building large strings without repeatedly calling strlen(..)
/// /////////////////////////////////////////////////////////////////////////////////////////


// A streaming object used to efficiently append lots of smaller strings to a very large string
//
struct TEXT_STREAM
{
   TCHAR   *szBuffer,         // Text buffer
           *szAssembledText;  // Temporary storage for assembling variable argument input strings

   UINT     iBufferUsed,      // Number of valid characters in the text buffer
            iBufferSize;      // Length of the text buffer, in characters
};

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     TRANSLATION
///
///   Used in the conversion of X3 game scripts to text
/// /////////////////////////////////////////////////////////////////////////////////////////

// Defines the syntax used to define parameters to script commands
//
enum PARAMETER_SYNTAX 
{   
   // Special
   PS_UNDETERMINED                   = 128,     // Added by me for use instead of NULL
   PS_CONDITION                      = 4,       // Unused

   // Scripting
   PS_REFERENCE_OBJECT               = 20,
   PS_RETURN_OBJECT                  = 15,
   PS_RETURN_OBJECT_IF               = 26,
   PS_RETURN_OBJECT_IF_START         = 28,
   PS_INTERRUPT_RETURN_OBJECT_IF     = 25,

   PS_COMMENT                        = 8,
   PS_LABEL_NAME                     = 1,
   PS_LABEL_NUMBER                   = 6,
   PS_SCRIPT_NAME                    = 7,
   PS_VARIABLE_DATATYPE              = 44,
   PS_VARIABLE_ENVIRONMENT           = 43,
   PS_EXPRESSION                     = 30,      // [TRANSLATION] A single node used by infix expressions
                                                // [GENERATION]  Every (non-ReturnObject) parameter in an expression
   PS_PARAMETER                      = 5,       // [TRANSLATION] A node-tuple used by script-call tuples and post-fix expression tuples
                                                // [GENERATION]  ScriptCall argument value
   // Basic Types
   PS_ARRAY                          = 45,
   PS_NUMBER                         = 3,
   PS_STRING                         = 2,
   PS_VALUE                          = 9,
   PS_VARIABLE                       = 0,
   PS_VARIABLE_CONSTANT              = 53,
   PS_VARIABLE_NUMBER                = 10,
   PS_VARIABLE_STRING                = 11,
   PS_VARIABLE_BOOLEAN               = 63,
   
   // Ships / Stations
   PS_VARIABLE_SHIP                  = 21,
   PS_VARIABLE_WING                  = 61,
   PS_VARIABLE_SHIP_STATION          = 22,
   PS_VARIABLE_PLAYER_SHIP           = 35,
   PS_VARIABLE_PLAYER_SHIP_STATION   = 38,
   PS_VARIABLE_SHIPTYPE              = 19,
   PS_VARIABLE_SHIPTYPE_STATIONTYPE  = 27,

   // Stations
   PS_STATION_CARRIER                = 40,
   PS_STATION_CARRIERDOCKAT          = 41,
   PS_VARIABLE_PLAYER_STATION_CARRIER= 37,
   PS_VARIABLE_PLAYER_STATION        = 36,
   PS_VARIABLE_STATION               = 12,
   PS_VARIABLE_STATIONPRODUCT        = 47,
   PS_VARIABLE_STATIONRESOURCE       = 46,
   PS_VARIABLE_STATIONSERIAL         = 18,
   PS_VARIABLE_STATIONTYPE           = 17,
   PS_VARIABLE_STATIONWARE           = 48,
   PS_VARIABLE_HOMEBASEPRODUCT       = 50,
   PS_VARIABLE_HOMEBASERESOURCE      = 49,
   PS_VARIABLE_HOMEBASEWARE          = 51,

   // Passengers
   PS_VARIABLE_PASSENGER             = 66,
   PS_VARIABLE_PASSENGER_OF_SHIP     = 68,
   PS_VARAIBLE_SHIP_AND_PASSENGER    = 67,
   
   // Wares
   PS_VARIABLE_ALLWARES              = 42,
   PS_VARIABLE_FLYINGWARE            = 55,
   PS_VARIABLE_WARE                  = 13,
   PS_VARIABLE_WARE_OF_SHIP          = 58,
   PS_VARIABLE_SHIP_AND_WARE         = 57,

   // Special objects
   PS_VARIABLE_ASTEROID              = 54,
   PS_VARIABLE_JUMPDRIVE_GATE        = 56,
   PS_VARIABLE_JUMPDRIVE_SECTOR      = 69,
   PS_VARIABLE_WARPGATE              = 39,

   // Albion Prelude
   PS_VARIABLE_FLEET_COMMANDER       = 70,
   PS_VARIABLE_GLOBAL_PARAMETER      = 71,
   
   // Constants
   PS_FLIGHTRETCODE                  = 34,
   PS_OBJECTCOMMAND                  = 31,
   PS_OBJECTCOMMAND_OBJECTSIGNAL     = 33,
   PS_OBJECTSIGNAL                   = 32,
   PS_RELATION                       = 29,
   PS_SECTOR_POSITION                = 52,
   PS_VARIABLE_CLASS                 = 23,
   PS_VARIABLE_QUEST                 = 59,
   PS_VARIABLE_RACE                  = 16,
   PS_VARIABLE_SECTOR                = 14,
   PS_VARIABLE_TRANSPORTCLASS        = 24,
   PS_VARIABLE_WING_COMMAND          = 65,
   PS_SCRIPT_REFERENCE_TYPE          = 62
};

// Define helpers for iterating through syntax
//
#define      FIRST_PARAMETER_SYNTAX            PS_VARIABLE
#define      LAST_PARAMETER_SYNTAX             PS_VARIABLE_GLOBAL_PARAMETER
#define      PARAMETER_SYNTAX_COUNT            72

/// ORDERED LIST  of  enum PARAMETER_SYNTAX      
//{   
//   PS_VARIABLE                      = 0,
//   PS_LABEL_NAME                      = 1,
//   PS_STRING                         = 2,
//   PS_NUMBER                         = 3,
//   PS_CONDITION                     = 4,
//   PS_PARAMETER                     = 5,
//   PS_LABEL_NUMBER                  = 6,
//   PS_SCRIPT_NAME                     = 7,
//   PS_COMMENT                        = 8,
//   PS_VALUE                           = 9,
//   PS_VARIABLE_NUMBER               = 10,
//   PS_VARIABLE_STRING               = 11,
//   PS_VARIABLE_STATION               = 12,
//   PS_VARIABLE_WARE                  = 13,
//   PS_VARIABLE_SECTOR               = 14,
//   PS_RETURN_OBJECT                  = 15,
//   PS_VARIABLE_RACE                  = 16,
//   PS_VARIABLE_STATIONTYPE            = 17,
//   PS_VARIABLE_STATIONSERIAL         = 18,
//   PS_VARIABLE_SHIPTYPE               = 19,
//   PS_REFERENCE_OBJECT                = 20,
//   PS_VARIABLE_SHIP                   = 21,
//   PS_VARIABLE_SHIP_STATION         = 22,
//   PS_VARIABLE_CLASS                  = 23,
//   PS_VARIABLE_TRANSPORTCLASS         = 24,
//   PS_INTERRUPT_RETURN_OBJECT_IF      = 25,
//   PS_RETURN_OBJECT_IF               = 26,
//   PS_VARIABLE_SHIPTYPE_STATIONTYPE   = 27,
//   PS_RETURN_OBJECT_IF_START         = 28,
//   PS_RELATION                        = 29,
//   PS_EXPRESSION                     = 30,
//   PS_OBJECTCOMMAND                  = 31,
//   PS_OBJECTSIGNAL                  = 32,
//   PS_OBJECTCOMMAND_OBJECTSIGNAL      = 33,
//   PS_FLIGHTRETCODE                  = 34,
//   PS_VARIABLE_PLAYER_SHIP            = 35,
//   PS_VARIABLE_PLAYER_STATION         = 36,
//   PS_VARIABLE_PLAYER_STATION_CARRIER = 37,
//   PS_VARIABLE_PLAYER_SHIP_STATION   = 38,
//   PS_VARIABLE_WARPGATE               = 39,
//   PS_STATION_CARRIER               = 40,
//   PS_STATION_CARRIERDOCKAT         = 41,
//   PS_VARIABLE_ALLWARES               = 42,
//   PS_VARIABLE_ENVIRONMENT            = 43,
//   PS_VARIABLE_DATATYPE               = 44,
//   PS_ARRAY                           = 45,
//   PS_VARIABLE_STATIONRESOURCE      = 46,
//   PS_VARIABLE_STATIONPRODUCT         = 47,
//   PS_VARIABLE_STATIONWARE            = 48,
//   PS_VARIABLE_HOMEBASERESOURCE      = 49,
//   PS_VARIABLE_HOMEBASEPRODUCT      = 50,
//   PS_VARIABLE_HOMEBASEWARE         = 51,
//   PS_SECTOR_POSITION               = 52,
//   PS_VARIABLE_CONSTANT               = 53,
//   PS_VARIABLE_ASTEROID               = 54,
//   PS_VARIABLE_FLYINGWARE            = 55,
//   PS_VARIABLE_JUMPDRIVE_GATE         = 56,
//   PS_VARIABLE_SHIP_AND_WARE         = 57,
//   PS_VARIABLE_WARE_OF_SHIP         = 58,
//   PS_VARIABLE_QUEST                  = 59
/// Just dicovered I've missed these:
//PS_VARIABLE_WING    = 61
//PS_SCRIPT_REFERENCE_TYPE = 62
//PS_VARIABLE_BOOLEAN = 63
//PS_VARIABLE_WING_COMMAND = 65
//PS_VARIABLE_PASSENGER = 66
//PS_VARAIBLE_SHIP_AND_PASSENGER = 67
//PS_VARIABLE_PASSENGER_OF_SHIP = 68
//PS_VARIABLE_JUMPDRIVE_SECTOR = 69
//};

/// NEW: Albion Prelude
//PS_VARIABLE_FLEET_COMMANDER = 70
//PS_VARIABLE_GLOBAL_PARAMETER = 71


/// ////////////////////////////////////////////////////////////////////////////////////////
///                                 TREE FUNCTORS
///
///  Functors for performing compare and delete operations on parametric datatypes
/// ////////////////////////////////////////////////////////////////////////////////////////
struct AVL_TREE;
struct AVL_TREE_NODE;
struct AVL_TREE_OPERATION;
enum   AVL_TREE_SORT_KEY;
enum   COMPARISON_RESULT;

// Functor for extracting the contents from an AVLTree node 
///               eg:   "LPARAM  extractNodeProperty(LPARAM  pData, CONST AVL_TREE_SORT_KEY  eProperty)"
typedef LPARAM  (*AVL_NODE_EXTRACTOR)(LPARAM, CONST AVL_TREE_SORT_KEY);

// AVL Tree custom operation functor
//
typedef VOID    (*AVL_TREE_FUNCTOR)(AVL_TREE_NODE*, AVL_TREE_OPERATION*);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                     TREES
///
/// A balanced polymorphic AVL binary tree with a limited multi-threaded capability
/// ////////////////////////////////////////////////////////////////////////////////////////

// Node object comparison results
//
enum COMPARISON_RESULT { CR_LESSER_OR_EQUAL = -2, CR_LESSER = -1, CR_EQUAL = 0, CR_GREATER = 1, CR_GREATER_OR_EQUAL = 2 };

// Sort properties available for various Objects.  They may be used by multiple objects in different ways
//
enum AVL_TREE_SORT_KEY { AK_NONE,               // No secondary sort key

                         AK_ID,                 // Individual ID
                         AK_COUNT,              // Count or size
                         AK_VALUE,              // Value
                         AK_GROUP,              // Group or type (integers)
                         AK_MAINTYPE,           // Ware Maintype
                         AK_NAME,               // File name or text ID
                         AK_PAGE_ID,            // Page ID
                         AK_PATH,               // File path
                         AK_TEXT,               // Text or description
                         AK_TYPE,               // Group or type (strings)
                         AK_VERSION  };         // File or command version
                         
// Defines underlying type of sort keys   (ie. how to compare them)
//
enum AVL_TREE_PROPERTY_TYPE { AP_INTEGER, AP_STRING, AP_STRING_CASE  };

// Types of traversal available in custom tree operations
//
enum AVL_TREE_TRAVERSAL { ATT_PREORDER, ATT_INORDER, ATT_POSTORDER };

// Defines the types of sorting available
enum AVL_TREE_ORDERING { AO_ASCENDING, AO_DESCENDING, AO_NONE };     // NONE is used for the lack of a secondary key

// Identifies the type of linkage of a node to it's parent
//
enum AVL_TREE_LINKAGE  { ATL_ROOT,     // Node is tree root
                         ATL_LEFT,     // Left child
                         ATL_RIGHT };  // Right child

// Defines the types of change made to the tree, which determines which balancing rules to follow to rebalance the tree
//
enum AVL_TREE_CHANGE    { AC_INSERTION, AC_REMOVAL };

// Defines a tree sort key
//
struct AVL_TREE_KEY
{
   AVL_TREE_SORT_KEY       eSorting;
   AVL_TREE_ORDERING       eDirection;
   AVL_TREE_PROPERTY_TYPE  eType;
};

// Node in an AVL Tree
//
struct AVL_TREE_NODE
{
   // Children
   AVL_TREE_NODE    *pLeft,             // Left child
                    *pRight,            // Right child
                    *pParent;

   // Node properties
   INT               iBalanceFactor;     // Balance factor of sub-tree rooted at this node
   UINT              iHeight;            // Height of the sub-tree rooted at this node
   UINT              iIndex;             // Sequential index of this node within it's tree
   LPARAM            pData;              // Node data
};


// AVL Tree
//
struct AVL_TREE
{
   // Data
   AVL_TREE_NODE*      pRoot;                      // Root node 
   UINT                iCount;                     // Current number of nodes

   // Properties
   BOOL                bIndexed;                   // Indicates the nodes have been numbered and can be accessed sequentially.
   AVL_TREE_KEY*       pSortKeys[3];
   UINT                iKeyCount;

   // Customisation
   DESTRUCTOR          pfnDeleteNode;              // [optional Destruction function for objects
   AVL_NODE_EXTRACTOR  pfnExtractValue;            // Extraction function for reading node data for comparisons
};


// Holds the data necessary to perform various operations on each node of an AVL Tree
//
struct AVL_TREE_OPERATION
{
   // Properties
   AVL_TREE_FUNCTOR    pfnFunctor;          // Operation function
   AVL_TREE_TRAVERSAL  eOrder;              // Order in which the nodes should be operated upon
   BOOL                bProcessing;         // Processing flag, operation is aborted if this becomes FALSE
   QUEUE*              pErrorQueue;         // Custom error queue, if any
   OPERATION_PROGRESS* pProgress;           // Custom Operation progress, if any

   // Parameters
   BOOL                bResult;             // [OPTIONAL] Operation result
   UINT                iFlags;              // [OPTIONAL] Flags used in the operation
   LPARAM              xFirstInput,         // [OPTIONAL] Input data for the operation
                       xSecondInput,        // [OPTIONAL] Extra input data
                       xThirdInput,         // [OPTIONAL] Extra input data
                       xFourthInput,        // [OPTIONAL] Extra input data
                       xOutput;             // [OPTIONAL] Output data from the operation

   // Input / Output
   CONST AVL_TREE*     pInputTree;          // [OPTIONAL] Input tree
   AVL_TREE*           pOutputTree;         // [OPTIONAL] Output tree

   // Internal data
   LPARAM              xInternalData1,      // [OPTIONAL] Internal data used during the operation
                       xInternalData2;      // [OPTIONAL] Internal data used during the operation

   // Synchronisation
   BOOL                bMultiThreaded;      // Whether operation should be multithreaded
   UINT                iNodesPerThread;     // Minimum number of nodes required per thread
   HANDLE              hProgressMutex,      // Mutex for updating Progress object
                       hOutputMutex;        // Mutex for accessing output tree
};


// Holds the data to control worker threads in a threaded operation upon an AVL Tree
//
struct AVL_TREE_THREAD
{
   AVL_TREE_OPERATION*  pOperationData;      // Data for the actual operation
   AVL_TREE_NODE*       pStartNode;          // Start node for this worker thread
   
   UINT                 iNodeIndex,          // Zero-based index of node currently being processed
                        iThreadCount,        // Total number of threads
                        iThreadIndex;        // Zero-based index of thread
};


// Helper for counting number of nodes in a each group in a tree
//
struct AVL_TREE_GROUP_COUNTER
{
   UINT  *piBuckets,       // Individual node counts for each group
          iBucketCount;    // Number of group counts in the array below
};


// Helper for accessing the contents of nodes in tree functors
//
#define  extractPointerFromTreeNode    extractObjectPointer


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   VARIABLE NAME
///
///   Used to generate IDs for variables from their name during generation
/// /////////////////////////////////////////////////////////////////////////////////////////

// Describes script variable usage
//
enum VARIABLE_ASSIGNMENT  { VA_UNASSIGNED, VA_ASSIGNED, VA_REPORTED };

// Represents a script variable, can be used for converting between name and ID, for counting usage, or for checking usage
//
struct VARIABLE_NAME
{
   TCHAR*               szName;      // Name of the variable (without the $ prefix)
   UINT                 iID,         // Zero-based ID
                        iCount;      // Number of times the variable used in the document
   VARIABLE_ASSIGNMENT  eAssigned;   // Whether variable has been assigned before usage
};

/// //////////////////////////////////////////////////////////////////////////////////////////
///                                  VARIABLE DEPENDENCY
///
///   Represents a local or global variable read or set by a script (Used for analysis by the user)
/// //////////////////////////////////////////////////////////////////////////////////////////

// Defines whether a variable dependency is local or global
//
enum  VARIABLE_TYPE  { VT_LOCAL, VT_GLOBAL };

// Defines the usage of a variable dependency
//
enum  VARIABLE_USAGE  { VU_GET = 1, VU_SET = 2, VU_GET_SET = 3 };

// Represents a local or global variable used in a script
//
struct VARIABLE_DEPENDENCY
{
   TCHAR           szVariableName[128];  // Variable name
   VARIABLE_TYPE   eType;                // Whether variable is local or global
   VARIABLE_USAGE  eUsage;               // Whether variable is read or written, or both
};

/// /////////////////////////////////////////////////////////////////////////////////////////
///                               VIRTUAL FILE DECOMPRESSION
///
///   Objects for representing and decompressing a GZIP file
/// /////////////////////////////////////////////////////////////////////////////////////////

// Identifies supported GZIP compression types. There is only one
//
enum  GZIP_COMPRESSION_TYPE  { GCT_DEFLATE = 8 };      // Algorithm is GZIP

// Identifies which components of a GZIP header are present
//
enum  GZIP_FLAGS  {  GF_TEXT        = 1,        // The content of the file is text. (ignored)
                     GF_HEADER_CRC  = 2,        // Header contains an extra 2-byte CRC
                     GF_EXTRA       = 4,        // Header contains custom data
                     GF_FILENAME    = 8,        // Header contains a filename
                     GF_COMMENT     = 16  };    // Header contains a comment

// Contains GZIP file meta-data
///                 NOTE: The order and type of these fields is important and must not be changed
struct GZIP_HEADER
{
   WORD  iMagicNumber;        // Identification code to identify GZIP files
   BYTE  iCompressionType,    // Compression algoritm ID
         iFlags;              // GZIP header flags
   UINT  tmModification;      // Modification time. (ignored)
   BYTE  iExtendedFlags,      // GZIP auxilliary flags regarding compression strength used.  (ignored)
         iOperatingSystemID;  // ID of the operating system used to create the file. (ignored)
};

// Specifies the attributes of the decompressed gzip data
///                 NOTE: The order and type of these fields is important and must not be changed
struct GZIP_FOOTER
{
   UINT  iFileCRC,         // The CRC of the decompressed data
         iUnPackedSize;    // Length of the decompressed data, in bytes
};

// Represents all the relevant pieces of a GZIP file together
//
struct GZIP_FILE
{
   GZIP_HEADER  oHeader;         // The GZIP file header
   WORD         iExtraSize;      // The amount of custom data, if any (in bytes)
   CONST CHAR  *szFilename,      // The filename following the header, if any
               *szComment;       // The comment following the header, if any
   GZIP_FOOTER  oFooter;         // The GZIP file footer
};

// Defines the magic number used to identify GZIP files
//
#define    GZIP_MAGIC_NUMBER    0x8b1f

// Define size of header and footer
//
#define    GZIP_HEADER_SIZE     10
#define    GZIP_FOOTER_SIZE     8

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   VIRTUAL FILE SYSTEM
///
///   Unified and transparent access to files in the game folder and the game catalogues
/// /////////////////////////////////////////////////////////////////////////////////////////

// Identifies location of catalogue files
//
enum CATALOGUE_LOCATION_FLAG  { CLF_ORIGINAL = 0x0000, CLF_ADDON = 0xF0000 };

// Needs to identify a physical or virtual file
struct VIRTUAL_FILE
{
   UINT      iAttributes;            // NULL for files, FIF_FOLDER for folders
   UINT      iCatalogueID,           // Catalogue number containing the file
             iDataOffset,            // Offset within the matching data file, in bytes
             iPackedSize,            // Size within the matching data file, in bytes
             iUnpackedSize;          // Decompressed size, in bytes

   TCHAR*    szFullPath;             // File path within the game folder, eg.  'types\\bongo.txt'
};

// Describes the catalogue file containing virtual files
//
struct CATALOGUE_FILE
{
   UINT    iCatalogue;
   TCHAR*  szFullPath;
};

// Contains the (virtual and physical) FileItem results of a FileSystem search
//
struct FILE_SEARCH
{
   AVL_TREE     *pResultsTree;         // FileItems representing the results of a search of a FileSystem object
   UINT          iCurrentResult;       // Results tree iterator - represents the index of the 'current' result.
};

// Contains a tree of VirtualFile objects representing all the files in the specified game folder.
// 
struct FILE_SYSTEM
{
   TCHAR*         szGameFolder;         // Path to the game folder with a trailing backslash.
   AVL_TREE*      pFilesTree;           // Tree containing all the virtual files within the game folder
   LIST*          pCatalogueList;       // List of CATALOGUE_FILEs used to generate the VFS
};

// Convenience description for bytes - used to describe file buffers
//
typedef  BYTE          RAW_FILE;

// Identifies byte-ordering marks
//
extern CONST BYTE  iByteOrderingUTF8[3],        // UTF-8 byte ordering header
                   iByteOrderingUTF16_BE[2],    // UTF-16 BigEndian byte ordering header
                   iByteOrderingUTF16_LE[2];    // UTF-16 LittleEndian byte ordering header

// Decryption keys used for virtual files
//
#define   ENCRYPTED_GZIP_KEY        0xc8
#define   ENCRYPTED_PLAINTEXT_KEY   0x33

/// Macro: extractShellIcon
// Description: Extract the system image list icon ID of an icon returned from SHGetFileInfo()
//
#define       extractShellIcon(nIconID)             (nIconID INCLUDES 0x00FFffFF)

/// Macro: extractShellOverlayIcon
// Description: Extract the system image list icon ID of the overlay icon for an icon returned from SHGetFileInfo()
//
#define       extractShellOverlayIcon(nIconID)      ((nIconID INCLUDES 0xFF000000) >> 24)

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       XML PARSER
///
///   A customized XML parser
/// /////////////////////////////////////////////////////////////////////////////////////////

// An XML_TREE_NODE property list item -- a {name,value} pair representing an XML tag property and value
//
struct XML_PROPERTY
{
   TCHAR  szName[24],      // Property name
         *szValue;         // Property value
};

// An item within an XML tree that represents an XML tag
//
struct XML_TREE_NODE
{
   TCHAR          szName[24],       // Original XML tag
                 *szText;           // Inline text of the XML tag (if any)  eg. <bear>A brown bear</bear>
   UINT           iLineNumber,      // LineNumber of the original XML tag. (Used solely for error reporting)
                  iIndex;           // Zero-based Index of the node (with respect to it's parent)

   XML_TREE_NODE *pParent;          // Parent node

   LIST          *pProperties,      // Tag properties (list of XML_PROPERTY*)
                 *pChildren;        // Child nodes (list of XML_TREE_NODE*)
   LIST_ITEM     *pItem;            // ListItem that contains this node (within parent's list of children) 
};

// A tree representing an XML file
//
struct XML_TREE
{
   XML_TREE_NODE*  pRoot;    // Root node
   UINT            iCount;   // Total number of nodes
};

// XML file parsing stack
//
struct XML_TAG_STACK
{
   TCHAR*  pItems[32];     // Stack of tag names
   UINT    iCount;         // Size of stack
};

// Flags indicating the type of an XML token
//
enum XML_TOKEN_TYPE  { XTT_TEXT,                 // Text
                       XTT_WHITESPACE,           // Whitespace
                       XTT_OPENING_TAG,          // Opening tag
                       XTT_CLOSING_TAG,          // Closing tag
                       XTT_SELF_CLOSING_TAG,     // Self-Closing tag
                       XTT_COMMENT_TAG,          // Comment
                       XTT_SCHEMA_TAG  };        // Schema tag

// An XML tokeniser
//
struct XML_TOKENISER
{
// Public:
   TCHAR            szText[MAX_STRING];   // Current token
   UINT             iCount;               // Current token length, in characters
   XML_TOKEN_TYPE   eType;                // Token type
   UINT             iLineNumber;          // Line number containing the token (one-based)

// Private:
   CONST TCHAR     *szSource,             // Input text
                   *szNextToken;          // Current position within the input string (Actually points to the next token)
};

// XML Tag properties parsing state
//
enum XML_TAG_PARSE_STATE  { XTPS_PROPERTY_NAME, XTPS_PROPERTY_VALUE };

// XML Tree Node iterator relationships
enum XML_NODE_RELATIONSHIP  { XNR_CHILD,        // First child
                              XNR_SIBLING,      // Next item in the list containing the node
                              XNR_PARENT  };    // Parent

// An XMLTreeNode that contains a command and has a CommandType to indicate which command branch it belongs to
//
struct COMMAND_NODE
{
   COMMAND_TYPE   eType;      // Type of the command
   XML_TREE_NODE* pNode;      // Node containing the child-nodes that define the command
};

// Two XMLTreeNodes that contain a PARAMETER data type and PARAMETER value in the first and second nodes respectively
//
struct PARAMETER_NODE_TUPLE
{
   XML_TREE_NODE  *pType,     // Node containing the parameter type
                  *pValue;    // Node containing the parameter value
};

// Identifies the meaning of the next (or next two) parameter nodes in a CommandNode
//
enum PARAMETER_NODE_TYPE { PNT_PARAMETER_TUPLE,       // 
                           PNT_SINGLE_PARAMETER,      // 
                           PNT_INFIX_EXPRESSION,      // 
                           PNT_POSTFIX_EXPRESSION,    // 
                           PNT_PARAMETER_COUNT,       // 
                           PNT_INFIX_COUNT,           // 
                           PNT_POSTFIX_COUNT    };    // 

// Identifies the various possible types of <sourcevalue> tag
//
enum SOURCE_VALUE_TYPE { SVT_UNKNOWN,   // Unrecognised or missing value type
                         SVT_ARRAY,     // Array value
                         SVT_STRING,    // String value
                         SVT_INTEGER }; // Integer value

// Represents various parameter counts for translating script CommandNodes.
//
struct PARAMETER_COUNT
{
   UINT    iCount,            // Number of ScriptCall argument or CommandComment parameters
           iPostfixCount,     // Number of Postfix expression parameters
           iInfixCount;       // Number of Infix expression parameters
};


