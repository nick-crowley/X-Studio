// 
// Messages.h  -- Global header containing stuff used by all projects
//
///      Messages prefixed with 'UN_' are notifications, 'UM_' indicate commands or messages with return values

#pragma once

#include "Test Cases.h"

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                 CONSTANTS / GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////

#define     UM_XSTUDIO                                              (WM_USER + 300)

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                     GENERIC
/// ////////////////////////////////////////////////////////////////////////////////////////

/// Name: UM_CONSOLE
// Description: Prints text to the debugging console
//
// WPARAM : NULL
// LPARAM : TCHAR* : New string to print to the console. This is owned by the Console and must not be destroyed. 
//                   It must be CONSOLE_LENGTH characters long.
//
#define     UM_CONSOLE                                              (UM_XSTUDIO)

/// Name: UM_SET_WINDOW_DATA
// Description: Pass a window it's window data
//
// WPARAM : NULL
// LPARAM : Window data pointer
//
#define     UM_SET_WINDOW_DATA                                      (UM_XSTUDIO + 1)

/// Name: UM_TESTCASE_BEGIN
// Description: Inform a window to start the specified TestCase
//
// WPARAM : TEST_CASE_TYPE : Which group the TestCase belongs to
// LPARAM : UINT           : TestCase number
//
#define     UM_TESTCASE_BEGIN                                       (UM_XSTUDIO + 2)

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    MAIN WINDOW
/// ////////////////////////////////////////////////////////////////////////////////////////

/// Name: UN_PREFERENCES_CHANGED
// Description: Notifies a window that the application preferences have changed and it should refresh itself
///                    NOTE: This is sent directly to each document and CodeEdit, so it cannot use the sendAppMessage technique
//
// WPARAM : NULL
// LPARAM : NULL
//
#define     UN_PREFERENCES_CHANGED                                  (UM_XSTUDIO + 5)
//
#define     Preferences_Changed(hWnd)                               SendMessage(hWnd, UN_PREFERENCES_CHANGED, NULL, NULL)


/// Name: UN_APP_STATE_CHANGED
// Description: Indicates the game data state has changed
//
// WPARAM : APPLICATION_STATE : [in] New application state
// LPARAM : NULL
//
#define     UN_APP_STATE_CHANGED                                    (UM_XSTUDIO + 6)


/// Name: UM_MAIN_WINDOW_CLOSING
// Description: Indicates the main window is closing
//
// WPARAM : MAIN_WINDOW_STATE : [in] Current state of the destruction process
// LPARAM : NULL
//
#define     UM_MAIN_WINDOW_CLOSING                                  (UM_XSTUDIO + 7)

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    OPERATIONS
/// ////////////////////////////////////////////////////////////////////////////////////////

/// Name: UM_PROCESSING_ERROR
// Description: Notify the user of a processing error and request how to proceed
//
// WPARAM : MESSAGE_DIALOG_FLAG : [in] Flag indicating which buttons to display
// LPARAM : ERROR_STACK*        : [in] Error message to be displayed
//
// Returns : EH_IGNORE or EH_ABORT
//
#define     UM_PROCESSING_ERROR                                     (UM_XSTUDIO + 10)

/// Name: UN_OPERATION_COMPLETE
// Description: Indicates an operation running in another thread has completed
//
// WPARAM : NULL
// LPARAM : OPERATION_DATA* : [in] Operation data
//
#define     UN_OPERATION_COMPLETE                                   (UM_XSTUDIO + 11) 

/// Name: UN_SCRIPTCALL_OPERATION_COMPLETE
// Description: Indicates a search for external script-calls has completed
//
// WPARAM : NULL
// LPARAM : AVL_TREE* : [in] ScriptDepedency tree
//
#define     UN_SCRIPTCALL_OPERATION_COMPLETE                        (UM_XSTUDIO + 12) 


/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    DOCUMENTS
/// ////////////////////////////////////////////////////////////////////////////////////////

/// Name: UN_DOCUMENT_SWITCHED
// Description: Indicates the active document has changed.
///                              The original document (if any) is guaranteed to still exist when this message is received
//
// WPARAM : NULL
// LPARAM : DOCUMENT* : [in] New document
//
#define     UN_DOCUMENT_SWITCHED                                    (UM_XSTUDIO + 15) 

/// Name: UN_DOCUMENT_UPDATED
// Description: Indicates the active document data has changed
//
#define     UN_DOCUMENT_UPDATED                                     (UM_XSTUDIO + 16) 

/// Name: UN_PROPERTY_UPDATED
// Description: Indicates a property of the active document has changed 
//
// WPARAM : UINT : [in] Control ID of the property that has changed
// LPARAM : NULL
//
#define     UN_PROPERTY_UPDATED                                     (UM_XSTUDIO + 17) 

/// Name: UM_GET_MENU_ITEM_STATE
// Description: Queries the parent of the focus window for the correct state of a menu command
//
// WPARAM : UINT : [in] ID of the menu/toolbar command to query the state of
// LPARAM : NULL
//
// Return Value : Combination of MF_ENABLED, MF_DISABLED, MF_CHECKED, MF_UNCHECKED
//
#define     UM_GET_MENU_ITEM_STATE                                   (UM_XSTUDIO + 18)

#define     Window_GetMenuItemState(hWnd, iCommandID, piState)       SendMessage(hWnd, UM_GET_MENU_ITEM_STATE, iCommandID, (LPARAM)(UINT*)(piState))

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                  SCRIPT DOCUMENT
/// ////////////////////////////////////////////////////////////////////////////////////////

/// Name: UM_GET_SCRIPT_VERSION
// Description: Retrieves the GameVersion of the target script document
//
// WPARAM : NULL
// LPARAM : GAME_VERSION* : [in/out] Pointer to GameVersion object that retrieves the game version of the current script
//
// Return Value : TRUE if document is a ScriptDocument, otherwise FALSE
//
#define     UM_GET_SCRIPT_VERSION                                    (UM_XSTUDIO + 19)
//
#define     Document_GetScriptVersion(hWnd, pVersion)                SendMessage(hWnd, UM_GET_SCRIPT_VERSION, NULL, (LPARAM)(pVersion))

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                  LANGUAGE DOCUMENT
/// ////////////////////////////////////////////////////////////////////////////////////////

/// Name: UN_OBJECT_DESTROYED
// Description: Notification that a language button is being destroyed
//
// LPARAM : IOleObject* : [in] OLE interface of object being destroyed
//
#define     UN_OBJECT_DESTROYED                                      (UM_XSTUDIO + 20)

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                     CODE EDIT
/// ////////////////////////////////////////////////////////////////////////////////////////

/// Name: UM_CLEAR_LINE_ERRORS
// Description: Removes all the errors from a CodeEdit
//
#define     UM_CLEAR_LINE_ERRORS                                     (UM_XSTUDIO + 23)
//
#define     CodeEdit_ClearLineErrors(hCtrl)                          SendMessage(hCtrl, UM_CLEAR_LINE_ERRORS, NULL, NULL)


/// Name: UM_COMMENT_SELECTION
// Description: Comments or uncomments the lines within the current text selection
//
// WPARAM : BOOL : [in] TRUE to comment selection, FALSE to uncomment
// LPARAM : NULL
//
#define     UM_COMMENT_SELECTION                                     (UM_XSTUDIO + 24)
//
#define     CodeEdit_CommentSelection(hWnd, bComment)                SendMessage(hWnd, UM_COMMENT_SELECTION, bComment, NULL)


/// Name: UM_FIND_TEXT
// Description: Searches for text within the CodeEdit and selects it, if found
//
// LPARAM : CODE_EDIT_SEARCH* : [in] Search data
//
// Return : TRUE if found, FALSE otherwise
//
#define     UM_FIND_TEXT                                             (UM_XSTUDIO + 25)
//
#define     CodeEdit_FindText(hCtrl,pSearchData)                     (BOOL)SendMessage(hCtrl, UM_FIND_TEXT, NULL, (LPARAM)(pSearchData))


/// Name: UM_GET_CARET_LINE_COMMAND
// Description: Retrieve the COMMAND associated with the line containing the caret
//
// Return : COMMAND* : The COMMAND associated with the line containing the caret
//
#define     UM_GET_CARET_LINE_COMMAND                                (UM_XSTUDIO + 26)
//
#define     CodeEdit_GetCaretLineCommand(hCtrl)                      (COMMAND*)SendMessage(hCtrl, UM_GET_CARET_LINE_COMMAND, NULL, NULL)


/// Name: UM_GET_CARET_COMMAND
// Description: Retrieve the ErrorQueue (if any) associated with the line containing the caret
//
// Return : ERROR_QUEUE* : The ErrorQueue associated with the line containing the caret, or NULL if there is none
//
#define     UM_GET_CARET_LINE_ERROR                                  (UM_XSTUDIO + 27)
//
#define     CodeEdit_GetCaretLineError(hCtrl)                        (ERROR_QUEUE*)SendMessage(hCtrl, UM_GET_CARET_LINE_ERROR, NULL, NULL)


/// Name: UM_GET_CARET_GAME_STRING
// Description: Retrieves the GameString referenced by the command containing the caret
//
// LPARAM : PROJECT_FILE* : [in] ProjectFile
//
// Return : GAME_STRING* if found, otherwise NULL
//
#define     UM_GET_CARET_GAME_STRING                                (UM_XSTUDIO + 51)
//
#define     CodeEdit_GetCaretGameString(hCtrl,pProject)             (GAME_STRING*)SendMessage(hCtrl, UM_GET_CARET_GAME_STRING, NULL, (LPARAM)(pProject))


/// Name: UM_GET_CARET_LOCATION
// Description: Retrieves the current location of the caret within the CodeEdit
//
// WPARAM : NULL
// LPARAM : CODE_EDIT_LOCATION* : [in/out] Location of the caret
//
// Return : TRUE
//
#define     UM_GET_CARET_LOCATION                                    (UM_XSTUDIO + 28)
//
#define     CodeEdit_GetCaretLocation(hCtrl,pLocation)               SendMessage(hCtrl, UM_GET_CARET_LOCATION, NULL, (LPARAM)(pLocation))


/// Name: UM_GET_COMMENT_QUALITY
// Description: Retrieves the quality score of the CodeEdit commenting
//
// Return : Quality score between 0 and 100
//
#define     UM_GET_COMMENT_QUALITY                                   (UM_XSTUDIO + 29)
//
#define     CodeEdit_GetCommentQuality(hCtrl)                        SendMessage(hCtrl, UM_GET_COMMENT_QUALITY, NULL, NULL)


/// Name: UM_GET_LABEL_QUEUE
// Description: Retrieves a queue containing label positions
//
// Return : New Queue containing CodeEditLabels, you are responsible for destroying it
//
#define     UM_GET_LABEL_QUEUE                                       (UM_XSTUDIO + 30)
//
#define     CodeEdit_GetLabelQueue(hCtrl)                            (QUEUE*)SendMessage(hCtrl, UM_GET_LABEL_QUEUE, NULL, NULL)


/// Name: UM_GET_SCRIPT_DEPENDENCY_TREE
// Description: Retrieves the ScriptDependencies tree from a CodeEdit which describes the number of external script calls
//
// Return : New AVLTree containing ScriptDependencies, you are responsible for destroying it
//
#define     UM_GET_SCRIPT_DEPENDENCY_TREE                            (UM_XSTUDIO + 31)
//
#define     CodeEdit_GetScriptDependencyTree(hCtrl)                  (AVL_TREE*)SendMessage(hCtrl, UM_GET_SCRIPT_DEPENDENCY_TREE, NULL, NULL)


/// Name: UM_GET_STRING_DEPENDENCY_TREE
// Description: Retrieves the StringDependencies tree from a CodeEdit which describes the GameStrings references in the script
//
// LPARAM : PROJECT_FILE* : [in] [optional] Project file containing active project variables
//
// Return : New AVLTree containing StringDependencies, you are responsible for destroying it
//
#define     UM_GET_STRING_DEPENDENCY_TREE                            (UM_XSTUDIO + 32)
//
#define     CodeEdit_GetStringDependencyTree(hCtrl, pProjectFile)    (AVL_TREE*)SendMessage(hCtrl, UM_GET_STRING_DEPENDENCY_TREE, NULL, (LPARAM)(pProjectFile))


/// Name: UM_GET_VARIABLE_DEPENDENCY_TREE
// Description: Retrieves the VariableDependencies tree from a CodeEdit which describes the local/global variable usage
//
// Return : New AVLTree containing VariableDependencies, you are responsible for destroying it
//
#define     UM_GET_VARIABLE_DEPENDENCY_TREE                          (UM_XSTUDIO + 33)
//
#define     CodeEdit_GetVariableDependencyTree(hCtrl)                (AVL_TREE*)SendMessage(hCtrl, UM_GET_VARIABLE_DEPENDENCY_TREE, NULL, NULL)


/// Name: UM_GET_VARIABLE_NAME_TREE
// Description: Retrieves the variable tree from a CodeEdit which describes the number of times each variable is used
//
// Return : New AVLTree containing VariableNames, you are responsible for destroying it
//
#define     UM_GET_VARIABLE_NAME_TREE                                (UM_XSTUDIO + 34)
//
#define     CodeEdit_GetVariableNameTree(hCtrl)                      (AVL_TREE*)SendMessage(hCtrl, UM_GET_VARIABLE_NAME_TREE, NULL, NULL)


/// Name: UM_GET_LINE_INDENTATION
// Description: Retrieve the indentation of a line of CodeEdit text
//
// WPARAM : UINT : [in] Zero based Line Number
//
// Return : Zero-based Indentation factor
//
#define     UM_GET_LINE_INDENTATION                                  (UM_XSTUDIO + 35)
//
#define     CodeEdit_GetLineIndentation(hCtrl, iLine)                (UINT)SendMessage(hCtrl, UM_GET_LINE_INDENTATION, (WPARAM)(iLine), NULL)


/// Name: UM_GET_LINE_TEXT 
// Description: Retrieve a line of CodeEdit text
//
// WPARAM : UINT : [in] Zero based Line Number
//
// Return : New string containing line text, you are responsible for destroying it
//
#define     UM_GET_LINE_TEXT                                         (UM_XSTUDIO + 36)
//
#define     CodeEdit_GetLineText(hCtrl, iLine)                       (TCHAR*)SendMessage(hCtrl, UM_GET_LINE_TEXT, (WPARAM)(iLine), NULL)

/// Name: UM_GET_SELECTION_TEXT 
// Description: Retrieve the selected CodeEdit text, if any
//
// Return : New string containing line text, if any, otherwise NULL
//
#define     UM_GET_SELECTION_TEXT                                    (UM_XSTUDIO + 37)
//
#define     CodeEdit_GetSelectionText(hCtrl)                         (TCHAR*)SendMessage(hCtrl, UM_GET_SELECTION_TEXT, NULL, NULL)


/// Name: UM_HAS_SELECTION
// Description: Determines whether a CodeEdit has a text selection or not
//
// Return : TRUE or FALSE
//
#define     UM_HAS_SELECTION                                         (UM_XSTUDIO + 38)
//
#define     CodeEdit_HasSelection(hCtrl)                             (BOOL)SendMessage(hCtrl, UM_HAS_SELECTION, NULL, NULL)


/// Name: UM_HAS_SUGGESTIONS
// Description: Determines whether the object under the caret can have suggestions or not
//
// Return : TRUE or FALSE
//
#define     UM_HAS_SUGGESTIONS                                       (UM_XSTUDIO + 39)
//
#define     CodeEdit_HasSuggestions(hCtrl)                           (BOOL)SendMessage(hCtrl, UM_HAS_SUGGESTIONS, NULL, NULL)


/// Name: UM_INSERT_TEXT_AT_CARET
// Description: Inserts the specified text into the CodeEdit at the position of the caret. 
//                The text should already have the appropriate brackets, since no meta-data is passed.
//
// WPARAM : NULL
// LPARAM : CONST TCHAR* : [in] Text to insert. This is owned by the caller and must not be deleted
//
#define     UM_INSERT_TEXT_AT_CARET                                  (UM_XSTUDIO + 40)
//
#define     CodeEdit_InsertTextAtCaret(hCtrl, szText)                SendMessage(hCtrl, UM_INSERT_TEXT_AT_CARET, NULL, (LPARAM)(szText))


/// Name: UM_HAS_SUGGESTIONS
// Description: Determines whether the object under the caret can have suggestions or not
//
// Return : TRUE or FALSE
//
#define     UM_IS_SELECTION_COMMENTED                                (UM_XSTUDIO + 41)
//
#define     CodeEdit_IsSelectionCommented(hCtrl)                     (BOOL)SendMessage(hCtrl, UM_IS_SELECTION_COMMENTED, NULL, NULL)


/// Name: UM_SCROLL_TO_LOCATION
// Description: Scroll the CodeEdit to a given position
//
// WPARAM : UINT : [in] Zero-based Character Index
// LPARAM : UINT : [in] Zero-based Line Number
//
#define     UM_SCROLL_TO_LOCATION                                    (UM_XSTUDIO + 42)
//
#define     CodeEdit_ScrollToLocation(hCtrl, iChar, iLine)           SendMessage(hCtrl, UM_SCROLL_TO_LOCATION, (WPARAM)(iChar), (LPARAM)(iLine))


/// Name: UM_SELECT_ALL
// Description: Determines whether the object under the caret can have suggestions or not
//
// Return : TRUE or FALSE
//
#define     UM_SELECT_ALL                                            (UM_XSTUDIO + 43)
//
#define     CodeEdit_SelectAll(hCtrl)                                (BOOL)SendMessage(hCtrl, UM_SELECT_ALL, NULL, NULL)


/// Name: UM_SET_LINE_ERROR
// Description: Inform a CodeEdit the specified line caused compiler error(s)/warning(s)
//
// WPARAM (LO) : UINT               : [in] Zero based Line Number
// WPARAM (HO) : ERROR_TYPE         : [in] Whether error(s) represent an error or a warning
// LPARAM      : CONST ERROR_QUEUE* : [in] ErrorQueue containing the error(s)
//
#define     UM_SET_LINE_ERROR                                        (UM_XSTUDIO + 44)
//
#define     CodeEdit_SetLineError(hCtrl, iLine, eType, pErrorQueue)  SendMessage(hCtrl, UM_SET_LINE_ERROR, (WPARAM)MAKE_LONG(iLine,eType), (LPARAM)(ERROR_QUEUE*)(pErrorQueue))


/// Name: UM_SET_PREFERENCES
// Description: Passes a Preferences object to a CodeEdit to define it's colour scheme
//
// WPARAM : NULL
// LPARAM : CONST PREFERENCES* : [in] Preferences object
//
#define     UM_SET_PREFERENCES                                       (UM_XSTUDIO + 45)
//
#define     CodeEdit_SetPreferences(hCtrl, pPreferences)             SendMessage(hCtrl, UM_SET_PREFERENCES, NULL, (LPARAM)(PREFERENCES*)(pPreferences))


/// Name: UM_SET_SCRIPT_FILE
// Description: Replace the contents of an empty CodeEdit with a ScriptFile's 'translated' COMMAND list
//
// WPARAM : NULL
// LPARAM : CONST SCRIPT_FILE* : [in] ScriptFile to display (THIS MUST NOT BE DELETED)
//
#define     UM_SET_SCRIPT_FILE                                       (UM_XSTUDIO + 46)
//
#define     CodeEdit_SetScriptFile(hCtrl, pScriptFile)               SendMessage(hCtrl, UM_SET_SCRIPT_FILE, NULL, (LPARAM)(SCRIPT_FILE*)(pScriptFile))


/// Name: UM_SHOW_SUGGESTIONS
// Description: Displays the SuggestionList on the line beneath the caret
//
#define     UM_SHOW_SUGGESTIONS                                      (UM_XSTUDIO + 47)
//
#define     CodeEdit_ShowSuggestions(hCtrl, iFlags)                  SendMessage(hCtrl, UM_SHOW_SUGGESTIONS, NULL, NULL)


/// Name: UN_CODE_EDIT_CHANGED
// Description: Informs the parent of a CodeEdit that it's contents have changed
//
// WPARAM : UINT : [in] CCF_SELECTION_CHANGED : The selection has changed
//                 [in] CCF_TEXT_CHANGED      : The text has changed 
//                 [in] CCF_SCOPE_CHANGED     : The current scope has changed. (lParam contains scope label)
//
// LPARAM : NULL             : [in] CCF_SELECTION_CHANGED and CCF_TEXT_CHANGED
//        : CODE_EDIT_LABEL* : [in] CCF_SCOPE_CHANGED : New scope
//
#define     UN_CODE_EDIT_CHANGED                                     (UM_XSTUDIO + 48)

/// Name: UN_CODE_EDIT_EXCEPTION
// Description: Informs the MainWindow an exception has occurred in the CodeEdit
//
// WPARAM : HWND         : [in] CodeEdit window
// LPARAM : ERROR_STACK* : [in] Error message
//
#define     UN_CODE_EDIT_EXCEPTION                                   (UM_XSTUDIO + 49)


/// Name: UN_CODE_EDIT_REQUEST_ARGUMENTS
// Description: Requests the parent insert default arguments for a script-call command
//
// LPARAM : COMMAND* : [in] COMMAND containing the script-call
//
#define     UN_CODE_EDIT_REQUEST_ARGUMENTS                           (UM_XSTUDIO + 50)

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                     GROUPED LIST VIEW
/// ////////////////////////////////////////////////////////////////////////////////////////

/// Name: UM_ADD_LISTVIEW_GROUP
// Description: Adds a new group to a GroupedListView
//
// WPARAM : NULL
// LPARAM : LISTVIEW_GROUP* : [in] Defines the group ID and name.  (The count is ignored)
//
#define     UM_ADD_LISTVIEW_GROUP                                    (UM_XSTUDIO + 55)
//
#define     GroupedListView_AddGroup(hCtrl, pGroupData)              SendMessage(hCtrl, UM_ADD_LISTVIEW_GROUP, NULL, (LPARAM)(pGroupData))


/// Name: UM_EMPTY_LISTVIEW_GROUPS
// Description: Empties but does not delete all groups in a GroupedListView
//
#define     UM_EMPTY_LISTVIEW_GROUPS                                 (UM_XSTUDIO + 56)
//
#define     GroupedListView_EmptyGroups(hCtrl)                       SendMessage(hCtrl, UM_EMPTY_LISTVIEW_GROUPS, NULL, NULL)


/// Name: UM_PHYSICAL_TO_LOGICAL
// Description: Converts a physical index into a logical onde
//
#define     UM_PHYSICAL_TO_LOGICAL                                   (UM_XSTUDIO + 57)
//
#define     GroupedListView_PhysicalToLogical(hCtrl, iPhysicalIndex) SendMessage(hCtrl, UM_PHYSICAL_TO_LOGICAL, iPhysicalIndex, NULL)


/// Name: UM_REMOVE_LISTVIEW_GROUPS
// Description: Deletes all groups in a GroupedListView
//
#define     UM_REMOVE_LISTVIEW_GROUPS                                (UM_XSTUDIO + 58)
//
#define     GroupedListView_RemoveGroups(hCtrl)                      SendMessage(hCtrl, UM_REMOVE_LISTVIEW_GROUPS, NULL, NULL)


/// Name: UM_SET_LISTVIEW_GROUP_COUNT
// Description: Defines the number of items in a GroupedListView group
//
// WPARAM : UINT : [in] ID of the group
// LPARAM : UINT : [in] Number of items in the group
//          
#define     UM_SET_LISTVIEW_GROUP_COUNT                              (UM_XSTUDIO + 59)
//
#define     GroupedListView_SetGroupCount(hCtrl, iGroupID, iCount)   SendMessage(hCtrl, UM_SET_LISTVIEW_GROUP_COUNT, (WPARAM)(iGroupID), (LPARAM)(iCount))

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                     MEDIA DOCUMENT
/// ////////////////////////////////////////////////////////////////////////////////////////

/// Name: UM_LOAD_MEDIA_ITEM
// Description: Tells the Media Player dialog to load and display the specified media item but not play it
//
// WPARAM : NULL
// LPARAM : Pointer to the MediaItem to load.  This is owned by the caller and should not be destroyed.
//
#define     UM_LOAD_MEDIA_ITEM            (UM_XSTUDIO + 60)

/// Name: UM_PLAY_MEDIA_ITEM
// Description: Tells the Media Player dialog to play the specified media item
//
// WPARAM : NULL
// LPARAM : Pointer to the MediaItem to play.  This is owned by the caller and should not be destroyed.
//
#define     UM_PLAY_MEDIA_ITEM            (UM_XSTUDIO + 61)

/// Name: UM_INIT_MEDIA_PLAYER
// Description: Passes dialog data to the media player
//
// WPARAM : NULL
// LPARAM : Dialog data
//
#define     UM_INIT_MEDIA_PLAYER            (UM_XSTUDIO + 62)

/// Name: UM_MEDIA_EVENT
// Description: Tells the Media Player dialog a DirectShow graph filter event has occurred
//
// WPARAM : NULL
// LPARAM : Custom ID, Ignored.
//
#define     UM_MEDIA_EVENT                (WM_APP + 2)




