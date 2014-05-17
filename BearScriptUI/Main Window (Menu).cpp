//
// Main Window (Menu).cpp : Handlers for the various menu/toolbar commands 
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          CONSTANTS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// Template used to create new MSCI scripts
//
CONST TCHAR*  szScriptTemplate = TEXT("*************************************************\r\n")
                                 TEXT("* SCRIPT NAME: %s\r\n") 
                                 TEXT("* DESCRIPTION: \r\n") 
                                 TEXT("* \r\n") 
                                 TEXT("* AUTHOR: %s          DATE: %s\r\n") 
                                 TEXT("*************************************************\r\n")
                                 TEXT("\r\n") 
                                 TEXT("return null");

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

const TCHAR*  getMessageString(UINT  iMessage)
{
   const TCHAR*  szOutput;

   switch (iMessage)
   {
      case IDM_EDIT_CUT:            szOutput = TEXT("IDM_EDIT_CUT");             break;
      case IDM_EDIT_COPY:           szOutput = TEXT("IDM_EDIT_COPY");            break;
      case IDM_EDIT_PASTE:          szOutput = TEXT("IDM_EDIT_PASTE");           break;
      case IDM_EDIT_DELETE:         szOutput = TEXT("IDM_EDIT_DELETE");          break;
      case IDM_EDIT_SELECT_ALL:     szOutput = TEXT("IDM_EDIT_SELECT_ALL");      break;
      case IDM_EDIT_UNDO:           szOutput = TEXT("IDM_EDIT_UNDO");            break;
      case IDM_EDIT_REDO:           szOutput = TEXT("IDM_EDIT_REDO");            break;
      case IDM_RICHEDIT_BOLD:       szOutput = TEXT("IDM_RICHEDIT_BOLD");        break;
      case IDM_RICHEDIT_ITALIC:     szOutput = TEXT("IDM_RICHEDIT_ITALIC");      break;
      case IDM_RICHEDIT_UNDERLINE:  szOutput = TEXT("IDM_RICHEDIT_UNDERLINE");   break;
      case IDM_EDIT_COMMENT:        szOutput = TEXT("IDM_EDIT_COMMENT");         break;
      default:                      szOutput = TEXT("Error");                    break;
   }
   return szOutput;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          MESSAGE HANDLERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onMainWindowCommand
// Description     : Menu command routing
// 
// MAIN_WINDOW_DATA*  pWindowData : [in] Main window data
// CONST UINT         iCommandID  : [in] Menu ID of the item chosen
// 
// Return type : TRUE if processed, FALSE to pass to default window proc
//
BOOL  onMainWindowCommand(MAIN_WINDOW_DATA*  pWindowData, CONST UINT  iCommandID)
{
   DOCUMENT*  pActiveDocument;
   BOOL       bResult = TRUE;

   // Prepare

   // Examine command
   switch (iCommandID)
   {
   /// [ACCELERATORS]
   case IDM_CODE_EDIT_VIEW_SUGGESTIONS:      //case IDM_CODE_EDIT_LOOKUP_COMMAND:
      CONSOLE_MENU(IDM_CODE_EDIT_VIEW_SUGGESTIONS);
   
      if (pActiveDocument = getActiveScriptDocument())
         SendMessage(pActiveDocument->hWnd, WM_COMMAND, iCommandID, NULL);
      else
         bResult = FALSE;
      break;

   // [FIND NEXT]
   case IDM_EDIT_FIND_NEXT:
      CONSOLE_MENU(IDM_EDIT_FIND_NEXT);

      // Simulate clicking 'FindNext' in text dialog
      if (pWindowData->hFindTextDlg)
         SendMessage(pWindowData->hFindTextDlg, WM_COMMAND, IDC_FIND_NEXT, NULL);
      break;

   /// [FILE MENU]
   // [FILE: CLOSE DOCUMENT]
   case IDM_FILE_CLOSE:
		CONSOLE_MENU(IDM_FILE_CLOSE);
      onMainWindowFileClose(pWindowData);
      break;

   // [FILE: CLOSE PROJECT]
   case IDM_FILE_CLOSE_PROJECT:
		CONSOLE_MENU(IDM_FILE_CLOSE_PROJECT);
      onMainWindowFileCloseProject(pWindowData);
      break;

   // [FILE: NEW DOCUMENT]
   case IDM_FILE_NEW:
		CONSOLE_MENU(IDM_FILE_NEW);
      onMainWindowFileNewDocument(pWindowData);
      break;

   // [FILE: NEW LANGUAGE FILE]
   case IDM_FILE_NEW_LANGUAGE:
		CONSOLE_MENU(IDM_FILE_NEW_LANGUAGE);
      onMainWindowFileNewLanguageDocument(pWindowData, NULL);
      break;

   // [FILE: NEW SCRIPT FILE]
   case IDM_FILE_NEW_SCRIPT:
		CONSOLE_MENU(IDM_FILE_NEW_SCRIPT);
      onMainWindowFileNewScriptDocument(pWindowData, NULL);
      break;

   // [FILE: NEW PROJECT FILE]
   case IDM_FILE_NEW_PROJECT:
		CONSOLE_MENU(IDM_FILE_NEW_PROJECT);
      onMainWindowFileNewProjectDocument(pWindowData, NULL);
      break;

   // [FILE: OPEN FILE]
   case IDM_FILE_OPEN:
		CONSOLE_MENU(IDM_FILE_OPEN);
      onMainWindowFileOpen(pWindowData, TRUE);
      break;

   // [FILE: SCRIPT BROWSER]
   case IDM_FILE_BROWSE:
		CONSOLE_MENU(IDM_FILE_BROWSE);
      onMainWindowFileOpen(pWindowData, FALSE);
      break;

   // [FILE: SAVE DOCUMENT]
   case IDM_FILE_SAVE:
		CONSOLE_MENU(IDM_FILE_SAVE);
      onMainWindowFileSave(pWindowData);
      break;

   // [FILE: EXPORT PROJECT]
   case IDM_FILE_EXPORT_PROJECT:
		CONSOLE_MENU(IDM_FILE_EXPORT_PROJECT);
      onMainWindowFileExportProject(pWindowData);
      break;

   // [FILE: SAVE DOCUMENT AS]
   case IDM_FILE_SAVE_AS:
		CONSOLE_MENU(IDM_FILE_SAVE_AS);
      onMainWindowFileSaveAs(pWindowData);
      break;

   // [FILE: SAVE DOCUMENT COPY]
   case IDM_FILE_SAVE_ALL:
		CONSOLE_MENU(IDM_FILE_SAVE_ALL);
      onMainWindowFileSaveAll(pWindowData);
      break;

   // [FILE: EXIT]
   case IDM_FILE_EXIT:
		CONSOLE_MENU(IDM_FILE_EXIT);
      onMainWindowFileExit(pWindowData);
      break;

   /// [EDIT MENU]
   // [EDIT: CUT/COPY/PASTE/DELETE/BOLD/ITALIC/UNDERLINE/COMMENT]
   case IDM_EDIT_CUT:         case IDM_EDIT_COPY:        case IDM_EDIT_PASTE:       
   case IDM_EDIT_DELETE:      case IDM_EDIT_UNDO:        case IDM_EDIT_REDO:        
   case IDM_RICHEDIT_BOLD:    case IDM_RICHEDIT_ITALIC:  case IDM_RICHEDIT_UNDERLINE: 
   case IDM_EDIT_SELECT_ALL:  case IDM_EDIT_COMMENT:
      CONSOLE("Menu Cmd: Sending %s to [parent of] input focus window", getMessageString(iCommandID));
      switch (iCommandID)
      {
      // [EDIT: CUT/COPY/PASTE/DELETE] Send to focus window
      case IDM_EDIT_CUT:         SendMessage(GetFocus(), WM_CUT,   NULL, NULL);    break;
      case IDM_EDIT_COPY:        SendMessage(GetFocus(), WM_COPY,  NULL, NULL);    break;
      case IDM_EDIT_PASTE:       SendMessage(GetFocus(), WM_PASTE, NULL, NULL);    break;
      case IDM_EDIT_DELETE:      SendMessage(GetFocus(), WM_CLEAR, NULL, NULL);    break;
      case IDM_EDIT_SELECT_ALL:  SendMessage(GetFocus(), EM_SETSEL, 0, -1);        break;
      case IDM_EDIT_UNDO:        SendMessage(GetFocus(), WM_UNDO, NULL, NULL);     break;
      case IDM_EDIT_REDO:        SendMessage(GetFocus(), EM_REDO, NULL, NULL);     break;
      // [EDIT: BOLD/ITALIC/UNDERLINE/COMMENT] Send to parent of focus window
      case IDM_RICHEDIT_BOLD:
      case IDM_RICHEDIT_ITALIC:
      case IDM_RICHEDIT_UNDERLINE: 
      case IDM_EDIT_COMMENT:     SendMessage(GetParent(GetFocus()), WM_COMMAND, iCommandID, NULL);     break;
      }
      break;

   // [EDIT: FIND] 
   case IDM_EDIT_FIND:
		CONSOLE_MENU(IDM_EDIT_FIND);
      onMainWindowEditFindText(pWindowData);
      break;

   /// [VIEW MENU]
   // [VIEW: COMMAND LIST]
   case IDM_VIEW_COMMAND_LIST:
		CONSOLE_MENU(IDM_VIEW_COMMAND_LIST);
      onMainWindowViewSearchDialog(pWindowData, RT_COMMANDS);
      break;

   // [VIEW: GAME OBJECTS LIST]
   case IDM_VIEW_GAME_OBJECTS_LIST:
		CONSOLE_MENU(IDM_VIEW_GAME_OBJECTS_LIST);
      onMainWindowViewSearchDialog(pWindowData, RT_GAME_OBJECTS);
      break;

   // [VIEW: SCRIPT OBJECTS LIST]
   case IDM_VIEW_SCRIPT_OBJECTS_LIST:
		CONSOLE_MENU(IDM_VIEW_SCRIPT_OBJECTS_LIST);
      onMainWindowViewSearchDialog(pWindowData, RT_SCRIPT_OBJECTS);
      break;

   // [VIEW: OUTPUT WINDOW]
   case IDM_VIEW_COMPILER_OUTPUT:
		CONSOLE_MENU(IDM_VIEW_COMPILER_OUTPUT);
      onMainWindowViewOutputDialog(pWindowData);
      break;

   // [VIEW: PROJECT WINDOW]
   case IDM_VIEW_PROJECT_EXPLORER:
		CONSOLE_MENU(IDM_VIEW_PROJECT_EXPLORER);
      onMainWindowViewProjectDialog(pWindowData);
      break;

   // [VIEW: SCRIPT PROPERTIES]
   case IDM_VIEW_DOCUMENT_PROPERTIES:
		CONSOLE_MENU(IDM_VIEW_DOCUMENT_PROPERTIES);
      onMainWindowViewPropertiesDialog(pWindowData);
      break;

   // [VIEW: APP PREFERENCES]
   case IDM_VIEW_PREFERENCES:
		CONSOLE_MENU(IDM_VIEW_PREFERENCES);
      onMainWindowViewPreferencesDialog(pWindowData);
      break;

   /// [DATA MENU]
   // [DATA: GAME STRINGS]
   case IDM_TOOLS_GAME_STRINGS:
		CONSOLE_MENU(IDM_TOOLS_GAME_STRINGS);
      onMainWindowDataGameStrings(pWindowData);
      break;

   // [DATA: GAME MEDIA]
   case IDM_TOOLS_MEDIA_BROWSER:
		CONSOLE_MENU(IDM_TOOLS_MEDIA_BROWSER);
      onMainWindowDataMediaBrowser(pWindowData);
      break;

   // [DATA: MISSIONS]
   case IDM_TOOLS_MISSION_HIERARCHY:
		CONSOLE_MENU(IDM_TOOLS_MISSION_HIERARCHY);
      onMainWindowDataMissionHierarchy(pWindowData);
      break;

   // [DATA: CONVERSATIONS]
   case IDM_TOOLS_CONVERSATION_BROWSER:
		CONSOLE_MENU(IDM_TOOLS_CONVERSATION_BROWSER);
      onMainWindowDataConversationBrowser(pWindowData);
      break;

   // [DATA: RELOAD]
   case IDM_TOOLS_RELOAD_GAME_DATA:
		CONSOLE_MENU(IDM_TOOLS_RELOAD_GAME_DATA);
      onMainWindowDataReload(pWindowData);
      break;

   /// [WINDOW MENU]
   // [WINDOW: ACTIVATE DOCUMENT]
   default:
      // [DOCUMENT] Display document
      if (iCommandID >= IDM_WINDOW_FIRST_DOCUMENT AND iCommandID <= IDM_WINDOW_LAST_DOCUMENT)
         onMainWindowWindowShowDocument(pWindowData, iCommandID - IDM_WINDOW_FIRST_DOCUMENT);
      // [RECENT FILE] Open file
      else if (iCommandID >= IDM_FILE_RECENT_FIRST AND iCommandID <= IDM_FILE_RECENT_LAST)
         onMainWindowFileOpenRecent(pWindowData, iCommandID - IDM_FILE_RECENT_FIRST);
      // [UNHANDLED] Return FALSE
      else
         bResult = FALSE;
      break;

   // [WINDOW: CLOSE ALL DOCUMENTS]
   case IDM_WINDOW_CLOSE_ALL_DOCUMENTS:
		CONSOLE_MENU(IDM_WINDOW_CLOSE_ALL_DOCUMENTS);
      onMainWindowWindowCloseDocuments(pWindowData, FALSE);
      break;

   // [WINDOW: CLOSE OTHER DOCUMENTS]
   case IDM_WINDOW_CLOSE_OTHER_DOCUMENTS:
		CONSOLE_MENU(IDM_WINDOW_CLOSE_OTHER_DOCUMENTS);
      onMainWindowWindowCloseDocuments(pWindowData, TRUE);
      break;

   // [WINDOW: NEXT DOCUMENT]
   case IDM_WINDOW_NEXT_DOCUMENT:
		CONSOLE_MENU(IDM_WINDOW_NEXT_DOCUMENT);
      displayNextDocument(pWindowData->hDocumentsTab);
      break;

   /// [HELP MENU]
   // [HELP: HELP FILE]
   case IDM_HELP_HELP:
		CONSOLE_MENU(IDM_HELP_HELP);
      onMainWindowHelpFile(pWindowData);
      break;

   // [HELP: CHECK UPDATES]
   case IDM_HELP_UPDATES:
		CONSOLE_MENU(IDM_HELP_UPDATES);
      onMainWindowHelpUpdates(pWindowData);
      break;

   // [HELP: FORUMS]
   case IDM_HELP_FORUMS:
		CONSOLE_MENU(IDM_HELP_FORUMS);
      onMainWindowHelpForums(pWindowData);
      break;

   // [HELP: SUBMIT FILE]
   case IDM_HELP_SUBMIT_FILE:
		CONSOLE_MENU(IDM_HELP_SUBMIT_FILE);
      onMainWindowHelpSubmitFile(pWindowData);
      break;

   // [HELP: ABOUT BOX]
   case IDM_HELP_ABOUT:
		CONSOLE_MENU(IDM_HELP_ABOUT);
      onMainWindowHelpAbout(pWindowData);
      break;

   // [HELP: TUTORIAL]
   case IDM_HELP_TUTORIAL_OPEN_FILE:    case IDM_HELP_TUTORIAL_FILE_OPTIONS:  case IDM_HELP_TUTORIAL_GAME_DATA:
   case IDM_HELP_TUTORIAL_GAME_FOLDER:  case IDM_HELP_TUTORIAL_GAME_OBJECTS:  case IDM_HELP_TUTORIAL_EDITOR:
   case IDM_HELP_TUTORIAL_PROJECTS:     case IDM_HELP_TUTORIAL_COMMANDS:      case IDM_HELP_TUTORIAL_SCRIPT_OBJECTS:
      onMainWindowHelpTutorial(pWindowData, iCommandID);
      break;

   /// [TEST MENU]
   // [TEST: LOAD COMMAND DESCRIPTIONS]
   case IDM_TEST_COMMAND_DESCRIPTIONS:
		CONSOLE_MENU(IDM_TEST_COMMAND_DESCRIPTIONS);
      onMainWindowTestCommandDescriptions(pWindowData);
      break;

   // [TEST: GAME DATA] Run GameData TestCases
   case IDM_TEST_GAME_DATA:
		CONSOLE_MENU(IDM_TEST_GAME_DATA);
      onMainWindowTestGameData(pWindowData);
      break;

   // [TEST: SCRIPT TRANSLATION] Run ScriptTranslation TestCases
   case IDM_TEST_SCRIPT_TRANSLATION:
		CONSOLE_MENU(IDM_TEST_SCRIPT_TRANSLATION);
      onMainWindowTestScriptTranslation(pWindowData);
      break;

   // [TEST: INTERPRET DOCUMENT] Attempt to compile current script
   case IDM_TEST_INTERPRET_DOCUMENT:
		CONSOLE_MENU(IDM_TEST_INTERPRET_DOCUMENT);
      onMainWindowTestScriptGeneration(pWindowData);
      break;

   // [TEST: OPEN EXAMPLE SCRIPT]
   case IDM_TEST_ORIGINAL_SCRIPT:
		CONSOLE_MENU(IDM_TEST_ORIGINAL_SCRIPT);
      onMainWindowTestOriginalScript(pWindowData);
      break;

   // [TEST: OPEN PREVIOUS OUTPUT SCRIPT]
   case IDM_TEST_OUTPUT_SCRIPT:
		CONSOLE_MENU(IDM_TEST_OUTPUT_SCRIPT);
      onMainWindowTestOutputScript(pWindowData);
      break;

   // [TEST: TEST CURRENT CODE] Display ALL loaded wares
   case IDM_TEST_CURRENT_CODE:
		CONSOLE_MENU(IDM_TEST_CURRENT_CODE);
      onMainWindowTestCurrentCode(pWindowData);
      break;

   // [TEST: TEST WARES] Print GameObjectNames to the console
   case IDM_TEST_VALIDATE_XML_SCRIPTS:
		CONSOLE_MENU(IDM_TEST_VALIDATE_XML_SCRIPTS);
      onMainWindowTestValidateUserScripts(pWindowData);
      break;

   // [TEST: CUSTOM TEST CASES] Run whatever TestCase you're debugging right now
   case IDM_TEST_VALIDATE_ALL_SCRIPTS:
		CONSOLE_MENU(IDM_TEST_VALIDATE_ALL_SCRIPTS);
      onMainWindowTestValidateAllScripts(pWindowData);
      break;
   }

   // Return result
   return bResult;
}


/// Function name  : onMainWindowDataGameStrings
// Description     : Creates a new LanguageDocument to display the game strings
// 
// MAIN_WINDOW_DATA*  pWindowData   : [in] Main window data
// 
VOID   onMainWindowDataGameStrings(MAIN_WINDOW_DATA*  pWindowData)
{
   DOCUMENT*       pDocument;
   INT             iDocumentIndex;

   /// [CHECK] Is this file already open?
   if (findDocumentIndexByPath(pWindowData->hDocumentsTab, TEXT("Game Data"), iDocumentIndex))
      // [SUCCESS] Display already open document
      displayDocumentByIndex(pWindowData->hDocumentsTab, iDocumentIndex);
   else
   {
      /// Create Virtual Document without GameFile
      pDocument = createDocumentByType(DT_LANGUAGE, NULL);
      
      // [DISPLAY] Add document and activate
      appendDocument(pWindowData->hDocumentsTab, pDocument);
      displayDocumentByIndex(pWindowData->hDocumentsTab, getDocumentCount() - 1);

      // Update Toolbar
      updateMainWindowToolBar(pWindowData);
   }

}


/// Function name  : onMainWindowDataMediaBrowser
// Description     : Creates a MediaDocument to display the game media
// 
// MAIN_WINDOW_DATA*  pWindowData   : [in] Main window data
// 
VOID  onMainWindowDataMediaBrowser(MAIN_WINDOW_DATA*  pWindowData)
{
   // Create new document and update document properties
   TODO("Re-write Media document code");
   ///addMediaDocumentToDocumentsCtrl(pWindowData->hDocumentsTab);

}


/// Function name  : onMainWindowDataMissionHierarchy
// Description     : 
// 
// MAIN_WINDOW_DATA*  pWindowData : [in] Window data
//
VOID  onMainWindowDataMissionHierarchy(MAIN_WINDOW_DATA*  pWindowData)
{
   // [ERROR] "The feature '%s' has not been implemented"
   displayMessageDialogf(NULL, IDS_GENERAL_NOT_IMPLEMENTED, MDF_OK WITH MDF_ERROR, TEXT("Mission Director Support"));

}


/// Function name  : onMainWindowDataConversationBrowser
// Description     : 
// 
// MAIN_WINDOW_DATA*  pWindowData : [in] Window data
// 
VOID  onMainWindowDataConversationBrowser(MAIN_WINDOW_DATA*  pWindowData)
{
   // [ERROR] "The feature '%s' has not been implemented"
   displayMessageDialogf(NULL, IDS_GENERAL_NOT_IMPLEMENTED, MDF_OK WITH MDF_ERROR, TEXT("Conversation Browser"));

}


/// Function name  : onMainWindowDataReload
// Description     : Reloads the game data
// 
// MAIN_WINDOW_DATA*  pWindowData : [in] Window data
// 
VOID  onMainWindowDataReload(MAIN_WINDOW_DATA*  pWindowData)
{
   // [CHECK] Are there any MODIFIED documents open?
   if (isAnyDocumentModified(pWindowData->hDocumentsTab))
   {
      /// [MODIFIED] "You must save all documents before attempting to reload your game data, would you like to save them now?"
      if (displayMessageDialogf(NULL, IDS_GENERAL_REQUIREMENTS_RELOAD, MDF_YESNO WITH MDF_WARNING) == IDYES)
         // [YES] Save all documents
         onMainWindowFileSaveAll(pWindowData);
   }
   else
      /// [UNMODIFIED] Reload game data
      performMainWindowReInitialisation(pWindowData, NULL);
   
}


/// Function name  : onMainWindowEditFindText
// Description     : Display find text dialog
// 
// MAIN_WINDOW_DATA*  pWindowData : [in] Window data
// 
VOID   onMainWindowEditFindText(MAIN_WINDOW_DATA*  pWindowData)
{
   // [CHECK] Does dialog already exist?
   if (pWindowData->hFindTextDlg)
   {
      // [EXISTS] Focus dialog
      CONSOLE("Focusing existing FindText dialog");
      SetForegroundWindow(pWindowData->hFindTextDlg);
   }
   else
      // [NEW] Create and display dialog
      pWindowData->hFindTextDlg = displayFindTextDialog(pWindowData->hMainWnd);
}


/// Function name  : onMainWindowFileClose
// Description     : Close the current document (if any)
// 
// MAIN_WINDOW_DATA*  pWindowData   : [in] Main window data
// 
VOID   onMainWindowFileClose(MAIN_WINDOW_DATA*  pWindowData)
{
   DOCUMENT*  pFocusedDocument;

   // [CHECK] Ensure there's a focused document
   if (pFocusedDocument = getFocusedDocument(pWindowData))
   {
      // Examine type
      if (pFocusedDocument->eType == DT_PROJECT)
         /// [PROJECT] Attempt to close active project
         closeActiveProject(NULL);
      else
         /// [DOCUMENT] Attempt to close active document
         closeDocumentByIndex(pWindowData->hDocumentsTab, TabCtrl_GetCurSel(pWindowData->hDocumentsTab));
   }

}


/// Function name  : onMainWindowFileCloseProject
// Description     : Close the current project (if any)
// 
// MAIN_WINDOW_DATA*  pWindowData   : [in] Main window data
// 
VOID   onMainWindowFileCloseProject(MAIN_WINDOW_DATA*  pWindowData)
{
   /// Close/save active project
   closeActiveProject(NULL);
   
}


/// Function name  : onMainWindowFileExit
// Description     : Attempts to close the program
// 
// MAIN_WINDOW_DATA*  pWindowData   : [in] Main window data
// 
VOID  onMainWindowFileExit(MAIN_WINDOW_DATA*  pWindowData)
{
   // Invoke CLOSE
   postAppClose(MWS_CLOSING);
}


/// Function name  : onMainWindowFileExportProject
// Description     : Saves all files in the project to a specified folder
// 
// MAIN_WINDOW_DATA*  pWindowData   : [in] Main window data
// 
VOID  onMainWindowFileExportProject(MAIN_WINDOW_DATA*  pWindowData)
{
   // Display 'Export Project' dialog
   if (getActiveProject())
      displayExportProjectDialog(pWindowData, getActiveProject()->pProjectFile);
}


/// Function name  : onMainWindowFileNewDocument
// Description     : Display the 'New Document' dialog
// 
// MAIN_WINDOW_DATA*  pWindowData : [in] Main window data
// 
VOID  onMainWindowFileNewDocument(MAIN_WINDOW_DATA*  pWindowData)
{
   NEW_DOCUMENT_DATA*  pDialogData;

   /// Display 'Insert Document' dialog and ask user for a filename
   if (pDialogData = displayInsertDocumentDialog(pWindowData))
   {
      // Create document of appropriate type
      switch (pDialogData->eNewDocumentType)
      {
      // [SCRIPT/LANGUAGE] Append new document
      case NDT_SCRIPT:      onMainWindowFileNewScriptDocument(pWindowData, pDialogData->szFullPath);     break;
      case NDT_LANGUAGE:    onMainWindowFileNewLanguageDocument(pWindowData, pDialogData->szFullPath);   break;

      // [PROJECT] Replace current project (if any)
      case NDT_PROJECT:     onMainWindowFileNewProjectDocument(pWindowData, pDialogData->szFullPath);    break;

      // [MISSION] Error!
      case NDT_MISSION:     ASSERT(FALSE);   break;
      }

      // Set path
      //setDocumentPath(getActiveDocument() or getLastDocument() etc., pDialogData->szFilename);

      // Cleanup
      deleteInsertDocumentDialogData(pDialogData);
   }
}

/// Function name  : onMainWindowFileNewLanguageDocument
// Description     : Create an empty language document
// 
// MAIN_WINDOW_DATA*  pWindowData : [in] Window data
// CONST TCHAR*       szFullPath  : [in] Full path of document if called via 'New Document' dialog, otherwise NULL
// 
VOID  onMainWindowFileNewLanguageDocument(MAIN_WINDOW_DATA*  pWindowData, CONST TCHAR*  szFullPath)
{
   LANGUAGE_FILE*  pLanguageFile;
   DOCUMENT*       pDocument;

   /// Create LanguageFile with single empty Page
   pLanguageFile = createUserLanguageFile(utilEither(szFullPath, TEXT("Untitled.xml")), getAppPreferences()->eGameLanguage);

   /// Create LanguageDoc
   pDocument = createDocumentByType(DT_LANGUAGE, pLanguageFile);
   
   // [DISPLAY] Add document and activate
   appendDocument(pWindowData->hDocumentsTab, pDocument);
   displayDocumentByIndex(pWindowData->hDocumentsTab, getDocumentCount() - 1);

   // [MODIFY] Set modified and Untitled
   setDocumentModifiedFlag(pDocument, TRUE);
   pDocument->bUntitled = (!szFullPath || utilCompareString(PathFindFileName(szFullPath), "Untitled.xml")
                                       || utilCompareString(PathFindFileName(szFullPath), "Untitled.pck") ? TRUE : FALSE);

   // Update Toolbar
   //updateMainWindowToolBar(pWindowData);
}


/// Function name  : onMainWindowFileNewProjectDocument
// Description     : Create an empty project document
// 
// MAIN_WINDOW_DATA*  pMainWindowDatda : [in] Window data
// CONST TCHAR*       szFullPath       : [in] Full path of project file if called via 'New Document' dialog, otherwise NULL
// 
VOID  onMainWindowFileNewProjectDocument(MAIN_WINDOW_DATA*  pWindowData, CONST TCHAR*  szFullPath)
{
   PROJECT_FILE*      pProjectFile;
   PROJECT_DOCUMENT*  pDocument;

   // [CHECK] Query how user would like to close the existing project, if any
   switch (closeActiveProject(NULL))
   {
   /// [SAVE-CLOSE] Save project
   case DCT_SAVE_CLOSE:
      commandSaveDocument(pWindowData, getActiveProject(), TRUE, NULL);
      break;

   /// [NONE/DISCARDED] Create new project
   case DCT_DISCARD:
      // Create Untitled ProjectFile
      pProjectFile = createProjectFile(utilEither(szFullPath, TEXT("Untitled.xprj")));

      // Create ProjectDocument and set active
      pDocument = (PROJECT_DOCUMENT*)createDocumentByType(DT_PROJECT, pProjectFile);
      setActiveProject(pDocument);

      // [MODIFY] Set modified and Untitled
      setDocumentModifiedFlag(pDocument, TRUE);
      pDocument->bUntitled = (!szFullPath || utilCompareString(PathFindFileName(szFullPath), "Untitled.xprj") ? TRUE : FALSE);

      // Update Toolbar
      //updateMainWindowToolBar(pWindowData);
      break;
   }

   // Cleanup
}


/// Function name  : onMainWindowFileNewScriptDocument
// Description     : Create an empty script document
// 
// MAIN_WINDOW_DATA*  pMainWindowDatda : [in] Window data
// CONST TCHAR*       szFullPath       : [in] Full path of document if called via 'New Document' dialog, otherwise NULL
// 
VOID  onMainWindowFileNewScriptDocument(MAIN_WINDOW_DATA*  pWindowData, CONST TCHAR*  szFullPath)
{
   SCRIPT_FILE  *pScriptFile;
   DOCUMENT     *pDocument;
   TCHAR        *szInitialText,
                *szDate;
   
   CONSOLE_ACTION_BOLD();
   
   // Prepare
   GetDateFormat(LOCALE_USER_DEFAULT, NULL, NULL, TEXT("d MMMM yyyy"), szDate = utilCreateEmptyString(32), 32);

   // Generate empty script text
   szInitialText = utilCreateStringf(512, szScriptTemplate, szFullPath ? PathFindFileName(szFullPath) : TEXT("Untitled"), getAppPreferences()->szForumUserName, szDate);

   /// Create Untitled ScriptFile
   pScriptFile = createScriptFileFromText(szInitialText, utilEither(szFullPath, TEXT("Untitled.xml")));

   /// Create Script document
   pDocument = createDocumentByType(DT_SCRIPT, pScriptFile);
         
   // [DISPLAY] Add document and activate
   appendDocument(pWindowData->hDocumentsTab, pDocument);
   displayDocumentByIndex(pWindowData->hDocumentsTab, getDocumentCount() - 1);

   // [MODIFY]
   setDocumentModifiedFlag(pDocument, TRUE);
   pDocument->bUntitled = (!szFullPath || utilCompareString(PathFindFileName(szFullPath), "Untitled.xml")
                                       || utilCompareString(PathFindFileName(szFullPath), "Untitled.pck") ? TRUE : FALSE);

   // Cleanup and update toolbar
   //updateMainWindowToolBar(pWindowData);
   utilDeleteStrings(szInitialText, szDate);
}


/// Function name  : onMainWindowFileOpen
// Description     : Launches the system OpenFile common dialog and opens the chosen file
//
// HWND  hMainWindow : [in] Window handle of the main BearScript window
// 
VOID   onMainWindowFileOpen(MAIN_WINDOW_DATA*  pWindowData, CONST BOOL  bUseSystemDialog)
{
   FILE_DIALOG_DATA*  pFileDialogData;
   STORED_DOCUMENT*   pStoredFile;

   // Create 'open' dialog data
   pFileDialogData = createFileDialogData(FDT_OPEN, getAppPreferences()->szLastFolder, NULL);

   /// Display system/browser dialog
   if (displayFileDialog(pFileDialogData, pWindowData->hMainWnd, bUseSystemDialog) == IDOK)
   {
      // Iterate through returned filenames
      for (UINT  iIndex = 0; findFileDialogOutputFileByIndex(pFileDialogData, iIndex, pStoredFile); iIndex++)
         /// Load file. Activate the final document.
         commandLoadDocument(pWindowData, pStoredFile->eType, pStoredFile->szFullPath, (iIndex == pFileDialogData->iOutputFiles - 1), &pFileDialogData->oAdvanced);
   }

   // Cleanup
   deleteFileDialogData(pFileDialogData);
}


/// Function name  : onMainWindowFileOpenRecent
// Description     : Opens a file from the 'recent documents' menu
// 
// MAIN_WINDOW_DATA*  pWindowData : [in] Window data
// CONST UINT         iIndex      : [in] Zero-based index of the recent file to open
// 
VOID  onMainWindowFileOpenRecent(MAIN_WINDOW_DATA*  pWindowData, CONST UINT  iIndex)
{
   STORED_DOCUMENT*  pRecentFile;
   LIST*             pRecentFileList;

   CONSOLE_ACTION();
   CONSOLE("Activating MRU document index=%d", iIndex);

   // Prepare
   pRecentFileList = generateRecentDocumentList(pWindowData);

   // [CHECK] Lookup recent file
   if (findListObjectByIndex(pRecentFileList, iIndex, (LPARAM&)pRecentFile))
      // [FOUND] Attempt to open file
      commandLoadDocument(pWindowData, pRecentFile->eType, pRecentFile->szFullPath, TRUE, NULL);

   // Cleanup
   deleteList(pRecentFileList);
}


/// Function name  : onMainWindowFileSave
// Description     : Save the active document or project
// 
// MAIN_WINDOW_DATA*  pWindowData : [in] Main window data
// 
VOID  onMainWindowFileSave(MAIN_WINDOW_DATA*  pWindowData)
{
   DOCUMENT*   pFocusedDocument;      // Focused document or project

   /// Save focused document
   if (pFocusedDocument = getFocusedDocument(pWindowData))
      saveDocument(pFocusedDocument, FALSE, FALSE);
   
}


/// Function name  : onMainWindowFileSaveAs
// Description     : Launches the 'Save As' dialog and saves the active document or project under a new path
// 
// MAIN_WINDOW_DATA*  pWindowData : [in] Main window data
// 
VOID  onMainWindowFileSaveAs(MAIN_WINDOW_DATA*  pWindowData)
{
   DOCUMENT*    pFocusedDocument;    // Focused document or project

   /// Display SaveAs dialog and save document/project under new filename
   if (pFocusedDocument = getFocusedDocument(pWindowData))
      saveDocument(pFocusedDocument, TRUE, FALSE);

}


/// Function name  : onMainWindowFileSaveAll
// Description     : Saves all open documents and the active project
// 
// MAIN_WINDOW_DATA*  pWindowData : [in] Main window data
// 
VOID  onMainWindowFileSaveAll(MAIN_WINDOW_DATA*  pWindowData)
{
   DOCUMENT*   pDocument;     // Document currently being processed

   // Iterate through all open documents
   for (UINT  iIndex = 0; findDocumentByIndex(pWindowData->hDocumentsTab, iIndex, pDocument); iIndex++)
      /// [DOCUMENT] Save document
      saveDocument(pDocument, FALSE, FALSE);

   /// [PROJECT] Save project
   if (pDocument = getActiveProject())
      saveDocument(pDocument, FALSE, FALSE);

}


/// Function name  : onMainWindowHelpAbout
// Description     : Display the about box
// 
// MAIN_WINDOW_DATA*  pWindowData : [in] Main window data
// 
VOID   onMainWindowHelpAbout(MAIN_WINDOW_DATA*  pWindowData)
{
   // Launch the About Dialog
   displayAboutDialog(pWindowData->hMainWnd);
}


/// Function name  : onMainWindowHelpFile
// Description     : 
// 
// MAIN_WINDOW_DATA*  pWindowData : [in] Main window data
// 
VOID  onMainWindowHelpFile(MAIN_WINDOW_DATA*  pWindowData)
{
   // Display Help
   displayHelp(TEXT("XStudio_Welcome"));

}


/// Function name  : onMainWindowHelpForums
// Description     : Launch the forums using the default web browser
// 
// MAIN_WINDOW_DATA*  pWindowData   : [in] Main window data
// 
VOID   onMainWindowHelpForums(MAIN_WINDOW_DATA*  pWindowData)
{
   // Launch forum URL
   utilLaunchURL(pWindowData->hMainWnd, TEXT("http://forum.egosoft.com"), SW_SHOWMAXIMIZED);

}


/// Function name  : onMainWindowHelpSubmitFile
// Description     : Create a new email with the appropriate address and subject
// 
// MAIN_WINDOW_DATA*  pWindowData   : [in] Main window data
// 
VOID   onMainWindowHelpSubmitFile(MAIN_WINDOW_DATA*  pWindowData)
{
   /// Display bug report dialog
   if (displayBugReportDialog(pWindowData->hMainWnd, TRUE) == IDOK)
      // [SUBMIT] Launch the submission thread
      commandSubmitReport(pWindowData, NULL);

}


/// Function name  : onMainWindowHelpTutorial
// Description     : Manually display any of the tutorials
// 
// MAIN_WINDOW_DATA*  pWindowData : [in] Window data
// CONST UINT         iCommandID  : [in] ID of tutorial menu command
// 
VOID  onMainWindowHelpTutorial(MAIN_WINDOW_DATA*  pWindowData, CONST UINT  iCommandID)
{
   TUTORIAL_WINDOW  eTutorial;

   //CONSOLE_ACTION();

   // Examine command ID
   switch (iCommandID)
   {
   case IDM_HELP_TUTORIAL_OPEN_FILE:         eTutorial = TW_OPEN_FILE;         CONSOLE_MENU(IDM_HELP_TUTORIAL_OPEN_FILE);        break;
   case IDM_HELP_TUTORIAL_FILE_OPTIONS:      eTutorial = TW_FILE_OPTIONS;      CONSOLE_MENU(IDM_HELP_TUTORIAL_FILE_OPTIONS);     break;  
   case IDM_HELP_TUTORIAL_GAME_DATA:         eTutorial = TW_GAME_DATA;         CONSOLE_MENU(IDM_HELP_TUTORIAL_GAME_DATA);        break;
   case IDM_HELP_TUTORIAL_GAME_FOLDER:       eTutorial = TW_GAME_FOLDER;       CONSOLE_MENU(IDM_HELP_TUTORIAL_GAME_FOLDER);      break; 
   case IDM_HELP_TUTORIAL_GAME_OBJECTS:      eTutorial = TW_GAME_OBJECTS;      CONSOLE_MENU(IDM_HELP_TUTORIAL_GAME_OBJECTS);     break;  
   case IDM_HELP_TUTORIAL_EDITOR:            eTutorial = TW_SCRIPT_EDITING;    CONSOLE_MENU(IDM_HELP_TUTORIAL_EDITOR);           break;
   case IDM_HELP_TUTORIAL_PROJECTS:          eTutorial = TW_PROJECTS;          CONSOLE_MENU(IDM_HELP_TUTORIAL_PROJECTS);         break;    
   case IDM_HELP_TUTORIAL_COMMANDS:          eTutorial = TW_SCRIPT_COMMANDS;   CONSOLE_MENU(IDM_HELP_TUTORIAL_COMMANDS);         break;  
   case IDM_HELP_TUTORIAL_SCRIPT_OBJECTS:    eTutorial = TW_SCRIPT_OBJECTS;    CONSOLE_MENU(IDM_HELP_TUTORIAL_SCRIPT_OBJECTS);   break;
   default:                                  return;
   }

   /// Force display of tutorial window
   displayTutorialDialog(utilGetTopWindow(getAppWindow()), eTutorial, TRUE);

}

/// Function name  : onMainWindowHelpUpdates
// Description     : 
// 
// MAIN_WINDOW_DATA*  pWindowData   : [in] 
// 
VOID  onMainWindowHelpUpdates(MAIN_WINDOW_DATA*  pWindowData)
{
   //// [ERROR] "The feature '%s' has not been implemented"
   //displayMessageDialogf(NULL, IDS_GENERAL_NOT_IMPLEMENTED, MDF_OK WITH MDF_ERROR, TEXT("Automatic Updates"));

   launchOperation(pWindowData, createUpdateOperation());

}
 

/// Function name  : onMainWindowViewOutputDialog
// Description     : Toggle display of the output dialog
// 
// MAIN_WINDOW_DATA*  pWindowData  : [in] Window data
// 
VOID    onMainWindowViewOutputDialog(MAIN_WINDOW_DATA*  pWindowData)
{
   /// Toggle output dialog
   displayOutputDialog(pWindowData, !pWindowData->hOutputDlg);

}


/// Function name  : onMainWindowViewPreferencesDialog
// Description     : Display the preferences dialog to the user and (possibly) load game data because of it.
// 
// MAIN_WINDOW_DATA*  pWindowData  : [in] Main window data
// 
VOID   onMainWindowViewPreferencesDialog(MAIN_WINDOW_DATA*  pWindowData)
{
   UINT    iPreferencesChanged;    // Flag indicating what (if any) important preferences have changed

   /// Display preferences dialog
   iPreferencesChanged = displayPreferencesDialog(pWindowData->hMainWnd, PP_GENERAL);

}


/// Function name  : onMainWindowViewProjectDialog
// Description     : Toggle display of the project dialog
// 
// MAIN_WINDOW_DATA*  pWindowData : [in] Main window data
// 
VOID  onMainWindowViewProjectDialog(MAIN_WINDOW_DATA*  pWindowData)
{
   /// Toggle project dialog
   displayProjectDialog(pWindowData, !pWindowData->hProjectDlg);

}


/// Function name  : onMainWindowViewPropertiesDialog
// Description     : Display or hide the document properties dialog
// 
// MAIN_WINDOW_DATA*  pWindowData  : [in] Main window data
// 
VOID   onMainWindowViewPropertiesDialog(MAIN_WINDOW_DATA*  pWindowData)
{
   /// Toggle properties window
   displayPropertiesDialog(pWindowData, !pWindowData->hPropertiesSheet);

}


/// Function name  : onMainWindowViewSearchDialog
// Description     : Either toggles the search results dialog, or changes the tab
// 
// MAIN_WINDOW_DATA*  pWindowData : [in] Main window data
// CONST RESULT_TYPE  eDialog     : [in] Tab to display
// 
VOID    onMainWindowViewSearchDialog(MAIN_WINDOW_DATA*  pWindowData, CONST RESULT_TYPE  eDialog)
{
   // Change to the desired tab, unless tab is already displayed -- then dialog is hidden
   displaySearchDialog(pWindowData, eDialog);

}


/// Configuration  : _TESTING
/// Function name  : onMainWindowTestCaseBegin
// Description     : Initiate the specified TestCase
// 
// MAIN_WINDOW_DATA*      pWindowData : [in] Main window data
// CONST TEST_CASE_TYPE  eType           : [in] Type of TestCase to run
// CONST UINT            iTestCaseID     : [in] ID of the TestCase to run
// 
#ifdef _TESTING
VOID  onMainWindowTestCaseBegin(MAIN_WINDOW_DATA*  pWindowData, CONST TEST_CASE_TYPE  eType, CONST UINT  iTestCaseID)
{
   switch (eType)
   {
   // [GAME DATA]
   case TCT_GAME_DATA:
      runGameDataTestCase(pWindowData, iTestCaseID);
      break;

   // [SCRIPT TRANSLATION]
   case TCT_SCRIPT_TRANSLATION:
      runScriptTranslationTestCase(pWindowData, iTestCaseID);
      break;

   // [ERROR]
   default:
      ASSERT(FALSE);
   }
}
#endif

/// Function name  : onMainWindowTestCommandDescriptions
// Description     : Load the command descriptions file for editing
// 
// MAIN_WINDOW_DATA*  pWindowData   : [in] 
// 
// Return Value   : 
// 
BOOL   onMainWindowTestCommandDescriptions(MAIN_WINDOW_DATA*  pWindowData)
{
   /*LANGUAGE_FILE*  pLanguageFile;
   OPERATION_DATA*    pOperationData;

   pLanguageFile = createLanguageFile(TEXT("Command-Descriptions.XML")); 

   pOperationData = commandLoadLanguageFile(pLanguageFile);
   createProgressDialog(pWindowData, pOperationData);*/

   // Test output dialog
   //testOutputDialog(pWindowData->hOutputDlg);

   // [ERROR] "The feature '%s' has not been implemented"
   displayMessageDialogf(NULL, IDS_GENERAL_NOT_IMPLEMENTED, MDF_OK WITH MDF_INFORMATION, TEXT("Test command Descriptions"));
   
   return TRUE;
}


/// Function name  : onMainWindowTestCurrentCode
// Description     : Test whatever you're debugging right now
// 
// MAIN_WINDOW_DATA*  pWindowData : [in] Main window data
// 
VOID  onMainWindowTestCurrentCode(MAIN_WINDOW_DATA*  pWindowData)
{
   //SCRIPT_DOCUMENT*  pDocument;

   /*if (pDocument = getActiveScriptDocument())
      CodeEdit_FindText(pDocument->hCodeEdit, CSF_FROM_CARET, TEXT("script"));*/

   //SCRIPT_OPERATION*  pOperationData;

   //// [QUESTION] "Would you like to search for all external scripts that depend upon '%s'?"
   //if (displayMessageDialogf(NULL, IDS_GENERAL_CONFIRM_DEPENDENCY_SEARCH, MDF_YESNO WITH MDF_QUESTION, TEXT("ignored")) == IDYES)
   //{
   //   // [VERBOSE]
   //   CONSOLE_COMMAND();

   //   // [SUCCESS] Create operation data
   //   pOperationData = createScriptOperationData(OT_SEARCH_SCRIPT_CONTENT, NULL, CMD_SIZE_OF_ARRAY);

   //   /// Launch search thread
   //   launchOperation(getMainWindowData(), pOperationData);
   //}

   TRY
   {
      debugAccessViolation();
   }
   CATCH
   {
      displayException(pException);
   }
}


/// Function name  : onMainWindowTestGameData
// Description     : Initiate the GameData batch of test cases
// 
// MAIN_WINDOW_DATA*  pWindowData : [in] Main window data
// 
// Return Value   : TRUE
// 
BOOL   onMainWindowTestGameData(MAIN_WINDOW_DATA*  pWindowData)
{
   // Initiate GameData TestCase number 1
   PostMessage(pWindowData->hMainWnd, UM_TESTCASE_BEGIN, TCT_GAME_DATA, 1);

   return TRUE;
}


/// Function name  : onMainWindowTestScriptGeneration
// Description     : Initiate the ScriptTranslation batch of test cases
// 
// MAIN_WINDOW_DATA*  pWindowData : [in] Main window data
// 
// Return Value   : TRUE
// 
BOOL  onMainWindowTestScriptGeneration(MAIN_WINDOW_DATA*  pWindowData)
{
   SCRIPT_DOCUMENT*  pDocument;
   ERROR_QUEUE*      pErrorQueue;

   // Prepare
   pErrorQueue = createErrorQueue();
 
   // Get active document
   if (getActiveDocument() AND getActiveDocument()->eType == DT_SCRIPT)
   {
      pDocument = (SCRIPT_DOCUMENT*)getActiveDocument();
      generateScript(pDocument->hCodeEdit, pDocument->pScriptFile, NULL, pErrorQueue);
   }

   // Cleanup and return
   deleteErrorQueue(pErrorQueue);
   return TRUE;
}


/// Function name  : onMainWindowTestScriptTranslation
// Description     : Initiate the ScriptTranslation batch of test cases
// 
// MAIN_WINDOW_DATA*  pWindowData : [in] Main window data
// 
// Return Value   : TRUE
// 
BOOL  onMainWindowTestScriptTranslation(MAIN_WINDOW_DATA*  pWindowData)
{
   // Initiate GameData TestCase number 50
   PostMessage(pWindowData->hMainWnd, UM_TESTCASE_BEGIN, TCT_SCRIPT_TRANSLATION, TC_MISSING_SCRIPT_TAG);
   return TRUE;
}


/// Function name  : onMainWindowTestOriginalScript
// Description     : Load an original script to verify the translation process
// 
// MAIN_WINDOW_DATA*  pWindowData : [in] Main window data
// 
// Return Value   : TRUE
// 
BOOL   onMainWindowTestOriginalScript(MAIN_WINDOW_DATA*  pWindowData)
{
   commandLoadDocument(pWindowData, FIF_SCRIPT, TEXT("C:\\temp\\plugin.piracy.lib.logic.xml"), TRUE, NULL);
   return TRUE;
}

/// Function name  : onMainWindowTestOutputScript
// Description     : Load a previously compiled script to verify the compiling process
// 
// MAIN_WINDOW_DATA*  pWindowData : [in] Main window data
// 
// Return Value   : TRUE
// 
BOOL   onMainWindowTestOutputScript(MAIN_WINDOW_DATA*  pWindowData)
{
   commandLoadDocument(pWindowData, FIF_SCRIPT, TEXT("C:\\Output.Test.xml"), TRUE, NULL);
   return TRUE;
}


/// Function name  : onMainWindowTestValidateAllScripts
// Description     : Validates all scripts in the scripts folder
// 
// MAIN_WINDOW_DATA*  pWindowData : [in] Main window data
// 
VOID  onMainWindowTestValidateAllScripts(MAIN_WINDOW_DATA*  pWindowData)
{
   performScriptValidationBatchTest(pWindowData, FF_SCRIPT_FILES);
}


/// Function name  : onMainWindowTestValidateUserScripts
// Description     : Validates all XML files in the scripts folder
// 
// MAIN_WINDOW_DATA*  pWindowData : [in] Main window data
// 
VOID   onMainWindowTestValidateUserScripts(MAIN_WINDOW_DATA*  pWindowData)
{
   performScriptValidationBatchTest(pWindowData, FF_SCRIPT_XML_FILES);
}



/// Function name  : onMainWindowWindowCloseDocuments
// Description     : Close all documents
// 
// MAIN_WINDOW_DATA*  pWindowData : [in] Main window data
// CONST BOOL         bExcludeActive  : [in] Whether to leave the active document open
// 
VOID  onMainWindowWindowCloseDocuments(MAIN_WINDOW_DATA*  pWindowData, CONST BOOL  bExcludeActive)
{
   /// Remove all documents [except active]
   closeAllDocuments(pWindowData->hDocumentsTab, bExcludeActive);

}


/// Function name  : onMainWindowWindowShowDocument
// Description     : Switch to the specified document
// 
// MAIN_WINDOW_DATA*  pWindowData : [in] Main window data
// CONST UINT         iIndex          : [in] Zero based document index
// 
VOID  onMainWindowWindowShowDocument(MAIN_WINDOW_DATA*  pWindowData, CONST UINT  iIndex)
{
   CONSOLE_ACTION();
   CONSOLE("Activating document index=%d", iIndex);

   /// Display desired document
   displayDocumentByIndex(pWindowData->hDocumentsTab, iIndex);

}

