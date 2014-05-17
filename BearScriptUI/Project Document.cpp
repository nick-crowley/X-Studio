//
// Project.cpp : Project files
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        MACROS
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : calculateProjectFolderFromDocumentType
// Description     : Converts a DocumentType into a ProjectFolderIndex
// 
// CONST DOCUMENT_TYPE  eType   : [in] DocumentType
// 
// Return Value   : ProjectFolderIndex
// 
PROJECT_FOLDER    calculateProjectFolderFromDocumentType(CONST DOCUMENT_TYPE  eType)
{
   PROJECT_FOLDER    eOutput;

   // Examine type
   switch (eType)
   {
   case DT_SCRIPT:      eOutput = PF_SCRIPT;      break;
   case DT_LANGUAGE:    eOutput = PF_LANGUAGE;    break;
   case DT_MISSION:     eOutput = PF_MISSION;     break;
   case DT_MEDIA:       ASSERT(FALSE);             break;
   }

   // Return output
   return eOutput;
}


/// Function name  : getActiveProject
// Description     : Retrieves the current project document
// 
// Return Value   : Current project, if any, otherwise NULL
// 
PROJECT_DOCUMENT*  getActiveProject()
{
   // [CHECK] Ensure main window exists
   ASSERT(getAppWindow());

   // Return current project or NULL
   return getMainWindowData()->pProject;
}


/// Function name  : getActiveProjectFile
// Description     : Retrieves the current project document's ProjectFile
// 
// Return Value   : Current ProjectFile, if any, otherwise NULL
// 
PROJECT_FILE*  getActiveProjectFile()
{
   return getActiveProject() ? getActiveProject()->pProjectFile : NULL;
}

/// Function name  : getActiveProjectFileName
// Description     : Retrieves the current project's FileName
// 
// Return Value   : Filename / NULL
// 
const TCHAR*  getActiveProjectFileName()
{
   return getDocumentFileName(getActiveProject());
}

/// Function name  : isDocumentInProject
// Description     : Determines whether a document is in the project
// 
// CONST DOCUMENT*  pDocument : [in] Document
// 
// Return Value   : TRUE/FALSE
// 
BOOL   isDocumentInProject(CONST DOCUMENT*  pDocument)
{
   STORED_DOCUMENT*  pDummy;

   /// Ensure project already exists, then Lookup item and return result
   return getActiveProject() AND findDocumentInProjectFileByPath(getActiveProjectFile(), getDocumentPath(pDocument), calculateProjectFolderFromDocumentType(pDocument->eType), pDummy);
}


/// Function name  : setProjectModifiedFlag
// Description     : Sets or Clears the modified flag of a project
// 
// PROJECT*    pProject   : [in] Project
// CONST BOOL  bModified  : [in] TRUE/FALSE
// 
VOID  setProjectModifiedFlag(PROJECT_DOCUMENT*  pProject, CONST BOOL  bModified)
{
   // [CHECK] Do nothing if flag already set
   if (isModified(pProject) != bModified)
   {
      /// Set flag and update main window title
      pProject->bModified = bModified;
      updateMainWindowTitle(getMainWindowData());

      // Update project dialog
      if (getMainWindowData()->hProjectDlg)
         updateProjectDialogRoot(getMainWindowData()->hProjectDlg);
   }
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : addDocumentToProject
// Description     : Adds a new document to the current project
// 
// CONST DOCUMENT*  pDocument : [in] Document to add
// 
// Return Value   : TRUE if successfully added, otherwise FALSE
// 
BOOL   addDocumentToProject(CONST DOCUMENT*  pDocument)
{
   STORED_DOCUMENT*   pStoredDocument;
   FILE_ITEM_FLAG     eFileType;

   // Prepare
   eFileType = calculateFileTypeFromDocumentType(pDocument->eType);

   // [CHECK] Return FALSE if project doesn't exist or document already present
   if (!getActiveProject() OR isDocumentInProject(pDocument))
      return FALSE;   

   /// Create new StoredDocument and append to appropriate file list
   pStoredDocument = createStoredDocument(eFileType, getDocumentPath(pDocument));
   addDocumentToProjectFile(getActiveProjectFile(), pStoredDocument);

   // [MODIFIED]
   setProjectModifiedFlag(getActiveProject(), TRUE);

   // [PROJECT DIALOG] Add to project dialog, if visible
   if (getMainWindowData()->hProjectDlg)
      addDocumentToProjectDialog(getMainWindowData()->hProjectDlg, pStoredDocument);

   // [SUCCESS] Return TRUE
   return TRUE;
}


/// Function name  : closeProject
// Description     : Closes the active project, saving it if necessary
// 
// CONST TCHAR*  szReplacementProject : [in] Full path of file to open instead
//
// Return Value   : DCT_SAVE_CLOSE : The project is being SaveClosed
//                  DCT_DISCARD    : The project was Closed
//                  DCT_ABORT      : The user wishes to abort closing the project
// 
CLOSURE_TYPE  closeActiveProject(CONST TCHAR*  szReplacementProject)
{
   PROJECT_DOCUMENT*  pProject;     // Active project
   CLOSURE_TYPE       eResult;      // Project closure type

   // Prepare
   eResult = DCT_DISCARD;

   // [CHECK] Ensure active project exists
   if (pProject = getActiveProject())
   {
      // [REPLACEMENT] Store path in document
      if (szReplacementProject)
         StringCchCopy(pProject->szReplacementPath, MAX_PATH, szReplacementProject);

      /// [CHECK] Determine how to close the document
      switch (eResult = closeDocument(pProject))
      {
      // [SAVE/DISCARD] Save/destroy project and return TRUE
      case DCT_SAVE_CLOSE:  commandSaveDocument(getMainWindowData(), pProject, TRUE, NULL);  break;
      case DCT_DISCARD:     deleteDocument((DOCUMENT*&)pProject);  setActiveProject(NULL);   break;
      }
   }

   // [CHECK] Is the app closing?
   if (isAppClosing() AND !pProject)
      postAppClose(MWS_PROJECT_CLOSED);   /// [EVENT] Fire UM_MAIN_WINDOW_CLOSING with state MWS_PROJECT_CLOSED

   // Return closure type
   return eResult;
}


/// Function name  : removeDocumentFromProject
// Description     : Removes a document from the project and the project dialog
// 
// CONST DOCUMENT*  pDocument : [in] Document to remove
// 
// Return Value   : TRUE if found, otherwise FALSE
// 
BOOL  removeDocumentFromProject(CONST DOCUMENT*  pDocument)
{   
   STORED_DOCUMENT*  pStoredDocument;  // Associated StoredDocument to search for
   PROJECT_FOLDER    eFolder;          // Project folder
   PROJECT_FILE*     pProjectFile;     // Convenience pointer

   // Prepare
   pProjectFile = getActiveProjectFile();
   eFolder      = calculateProjectFolderFromDocumentType(pDocument->eType);

   /// [CHECK] Lookup document in project
   if (!pProjectFile OR !findDocumentInProjectFileByPath(pProjectFile, getDocumentPath(pDocument), eFolder, pStoredDocument))
      // [FAILED] No project or document not in project
      return FALSE;

   // [MODIFIED] Set modified flag
   setProjectModifiedFlag(getActiveProject(), TRUE);
   
   // [PROJECT DIALOG] Remove from dialog
   if (getMainWindowData()->hProjectDlg)
      removeDocumentFromProjectDialog(pDocument); 
   
   /// Remove from project
   return removeFileFromProject(pProjectFile, pStoredDocument);
}


/// Function name  : setActiveProject
// Description     : Sets or clears the active project 
// 
// PROJECT*   pProject : [in] New project, or NULL to remove
//
VOID  setActiveProject(PROJECT_DOCUMENT*  pNewProject)
{
   MAIN_WINDOW_DATA*  pWindowData;

   // [CHECK] Ensure we're not trying to replace an existing project
   ASSERT((pNewProject AND !getActiveProject()) OR (!pNewProject AND getActiveProject()));

   // Prepare
   pWindowData = getMainWindowData();

   /// Set active project
   pWindowData->pProject = pNewProject;

   // Update main window title
   updateMainWindowTitle(pWindowData);

   // [PROJECT] Set/Remove dialog
   if (pNewProject)
      pNewProject->hWnd = pWindowData->hProjectDlg;

   // [DIALOG] Update dialog to reflect new project
   if (pWindowData->hProjectDlg)
      updateProjectDialog(pWindowData->hProjectDlg);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onProject_LoadComplete
// Description     : Creates the project document and sets as the active project
// 
// DOCUMENT_OPERATION*  pOperationData   : [in] 
// 
VOID  onProject_LoadComplete(DOCUMENT_OPERATION*  pOperationData)
{
   PROJECT_DOCUMENT*  pProject;    // New document

   // [VERBOSE]
   CONSOLE_EVENT1(PathFindFileName(pOperationData->szFullPath));
   debugDocumentOperationData(pOperationData);

   /// [SUCCESS] Create new project and set active
   if (isOperationSuccessful(pOperationData))
   {
      // [DEBUG]
      debugProjectFile((PROJECT_FILE*)pOperationData->pGameFile);

      // Create project
      pProject = (PROJECT_DOCUMENT*)createDocumentByType(DT_PROJECT, (PROJECT_FILE*)pOperationData->pGameFile);
      setActiveProject(pProject);

      // [RECENT] Add project to MRU
      addDocumentToRecentDocumentList(getMainWindowData(), pProject);

      // Sever GameFile to prevent destruction
      pOperationData->pGameFile = NULL;
   }
   /// [FAILED] Flag a critical error
   else
      setAppError(AE_LOAD_SCRIPT);     // [CRITICAL] Translation failed
}


/// Function name  : onProject_SaveComplete
// Description     : Saves the project, and closes it if required
// 
// DOCUMENT_OPERATION*  pOperationData   : [in] 
// 
VOID  onProject_SaveComplete(DOCUMENT_OPERATION*  pOperationData)
{
   PROJECT_DOCUMENT*    pDocument;      // Project

   // [VERBOSE]
   CONSOLE_EVENT1(PathFindFileName(pOperationData->szFullPath));
   debugDocumentOperationData(pOperationData);

   // Prepare
   pDocument = (PROJECT_DOCUMENT*)pOperationData->pDocument;

   // [CHECK] Was the save successful?
   if (isOperationSuccessful(pOperationData))
   {
      /// [UN-MODIFIED] Update flag
      setProjectModifiedFlag(pDocument, FALSE);

      // [CHECK] Should project close after a successful save?
      if (pDocument->bClosing)
      {
         /// [CLOSE] Close project
         closeActiveProject(NULL);

         // [CHECK] Is the app closing down?
         if (isAppClosing())
            /// [EVENT] Fire UM_MAIN_WINDOW_CLOSING with state MWS_PROJECT_CLOSED
            postAppClose(MWS_PROJECT_CLOSED);

         /// [REPLACEMENT] Load replacement project
         else if (lstrlen(pDocument->szReplacementPath))
            commandLoadDocument(getMainWindowData(), FIF_PROJECT, pDocument->szReplacementPath, TRUE, NULL);
      }
   }
   // [CHECK] Should the app be closed after a successful save?
   else if (isAppClosing())
      /// [FAILED] Abort the app closing sequence
      setAppClosing(FALSE);

   // Sever GameFile to prevent destruction
   pOperationData->pGameFile = NULL;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////


