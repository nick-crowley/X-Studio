//
// Types.h : Declaration of various structures, enumerations and constants
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#pragma once

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS
/// ////////////////////////////////////////////////////////////////////////////////////////

#define            ERROR_LENGTH              4096         // Max length for an error

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    GENERIC
/// ////////////////////////////////////////////////////////////////////////////////////////

// Functor for destroying the contents of a list or tree
///         eg:   "VOID  deleteListOrTreeObject(LPARAM  pData)"
//
typedef VOID    (*DESTRUCTOR)(LPARAM);

/// Name: extractObjectPointer
// Description: Convenience macro for extracting an object pointer stored as an LPARAM
//
#define        extractObjectPointer(pContainer, xType)     ((pContainer) ? (xType*)((pContainer)->pData) : NULL)

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    EXCEPTION STRING
///
///   String wrapper used for passing strings to an exception handler
/// ////////////////////////////////////////////////////////////////////////////////////////

// Convenience object for storing details of a failed debugging assertion
//
struct EXCEPTION_STRING
{
   CONST TCHAR   *szAssertion,      // Assertion text
                 *szFunction,       // Function
                 *szFileName;           // File
   UINT           iLine;            // Line number
};

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    DEVICE CONTEXT STATE
///
///   Used for restoring a DC to it's original state after drawing
/// ////////////////////////////////////////////////////////////////////////////////////////

// Describes the current contents of a DC State 
//
enum DEVICE_CONTEXT_CONTENTS  { DCC_NONE              = 0x0000,
                                DCC_BACKGROUND_COLOUR = 0x0001,
                                DCC_BACKGROUND_MODE   = 0x0002,
                                DCC_TEXT_COLOUR       = 0x0004,
                                DCC_BITMAP            = 0x0008,
                                DCC_BRUSH             = 0x0010,
                                DCC_FONT              = 0x0020,
                                DCC_PEN               = 0x0040  };

// Convenience object for storing original DC attributes while drawing
//
struct DEVICE_CONTEXT_STATE
{
   // Main
   HDC       hDC;
   UINT      eState;

   // Data
   COLORREF  clOldTextColour,
             clOldBackgroundColour;
   INT       iOldBackgroundMode;
   HFONT     hOldFont;
   HPEN      hOldPen;
   HBRUSH    hOldBrush;
   HBITMAP   hOldBitmap;
};

// Convenience description
typedef DEVICE_CONTEXT_STATE  DC_STATE;


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       LIST
///
///   A simple double-linked list with the ability to hold any data type
/// /////////////////////////////////////////////////////////////////////////////////////////

// Item within a linked list
//
struct LIST_ITEM
{
   LPARAM       pData;      // Item data
   LIST_ITEM   *pNext,      // Next item in the list
               *pPrev;      // Previous item in the list
};

// Double-Linked List
//
struct LIST
{
   LIST_ITEM   *pHead,            // First item in the list or NULL if list is empty
               *pTail;            // Last item in the list or NULL if list is empty
   UINT         iCount;           // Number of items in list
   DESTRUCTOR   pfnDeleteItem;    // [OPTIONAL]  Functor for deleting item data
};

// List iterator
//
struct LIST_ITERATOR
{
   LIST_ITEM*   pCurrentItem;     // Current iterator position
   UINT         iIndex;           // Zero-based index of the current item
};

/// Name: extractListItemPointer
// Description: Convenience macro for extracting an object pointer from a ListItem
//
#define        extractListItemPointer       extractObjectPointer

/// ////////////////////////////////////////////////////////////////////////////////////////
///                              MANUAL FUNCTION IMPORTS
///
///   Functors used for manually importing NT6 Windows API functions into an NT5 build environment
/// ////////////////////////////////////////////////////////////////////////////////////////

// ChangeWindowMessageFilterEx() - Adjusts the message filter to allow drag'n'drop functionality
//
typedef   BOOL                  (*CHANGE_WINDOWS_MESSAGE_FILTER_EX)(HWND, UINT, DWORD, CHANGEFILTERSTRUCT*);
#define  WM_COPYGLOBALDATA     0x49 

// FileIconInit() - Initialises the per-process sytem imagelist
typedef   BOOL                  (*FILE_ICON_INIT)(BOOL);

// Shell_GetCachedImageIndex() - Adds an icon to the system ImageList
typedef   INT                   (*GET_CACHED_IMAGE_INDEX)(CONST TCHAR*, INT, UINT);


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       QUEUES
///
///   A FIFO queue made from a linked list
/// /////////////////////////////////////////////////////////////////////////////////////////

// Base the queue entirely on pseudonyms to avoid having to write any real code
//    Should also allow you manipulate the queue in special circumstances
//
typedef  LIST                  QUEUE;
typedef  LIST_ITEM             QUEUE_ITEM;

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       STACKS
///
///   A FILO stack made from a linked list
/// /////////////////////////////////////////////////////////////////////////////////////////

// Base the queue entirely on pseudonyms to avoid having to write any real code
//    Should also allow you manipulate the queue in special circumstances
//
typedef  LIST                  STACK;
typedef  LIST_ITEM             STACK_ITEM;

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       THREADS
///
///   Helper for naming threads
/// /////////////////////////////////////////////////////////////////////////////////////////

#pragma pack(push,8)
// Custom MS-VC debugger exception data used for naming threads    (Code taken directly from MSDN article 'SetThreadName')
//
struct THREAD_NAME
{
   DWORD       dwType;     // Must be 0x1000.
   CONST CHAR* szName;     // Pointer to name (in user addr space).
   DWORD       dwThreadID; // Thread ID (-1=caller thread).
   DWORD       dwFlags;    // Reserved for future use, must be zero.
} ;
#pragma pack(pop)

// Custom exception used by the MS-VC debugger     (Code taken directly from MSDN article 'SetThreadName')
#define    MS_VC_EXCEPTION    0x406D1388

// Thread return code
#define    THREAD_RETURN      0

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       USER INTERFACE
///
///   
/// /////////////////////////////////////////////////////////////////////////////////////////

// Flags for 'utilCentreRectangle'
enum CENTRE_RECTANGLE_FLAG  { UCR_HORIZONTAL = 1, UCR_VERTICAL = 2 };

// Defines the possible operating system versions
//
enum WINDOWS_VERSION  { WINDOWS_2000, WINDOWS_XP, WINDOWS_2003, WINDOWS_VISTA, WINDOWS_7, WINDOWS_FUTURE };

