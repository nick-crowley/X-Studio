//
// FILE_NAME.cpp : FILE_DESCRIPTION
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

// The idea for the polymorphic document should be common duties:
// -> Initialisation
// -> Destruction
// -> Resizing

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        MACROS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////

CONST TCHAR*  szDocumentTypes[5] = 
{
   TEXT("script"),
   TEXT("language"),
   TEXT("media"),
   TEXT("mission"),
   TEXT("project")
};

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

LANGUAGE_DOCUMENT*  createLanguageDocument(LANGUAGE_FILE*  pLanguageFile)
{
   LANGUAGE_DOCUMENT*  pDocument = utilCreateEmptyObject(LANGUAGE_DOCUMENT);

   // Set properties
   pDocument->eType         = DT_LANGUAGE;
   pDocument->pLanguageFile = pLanguageFile;
   pDocument->pGameFile     = pLanguageFile;

   // Return document
   return pDocument;
}

SCRIPT_DOCUMENT*  createScriptDocument(SCRIPT_FILE*  pScriptFile)
{
   SCRIPT_DOCUMENT*  pDocument = utilCreateEmptyObject(SCRIPT_DOCUMENT);

   // Set properties
   pDocument->eType       = DT_SCRIPT;
   pDocument->pScriptFile = pScriptFile;
   pDocument->pGameFile   = pScriptFile;
   
   // Return document
   return pDocument;
}

MEDIA_DOCUMENT*  createMediaDocument()
{
   MEDIA_DOCUMENT*  pDocument = utilCreateEmptyObject(MEDIA_DOCUMENT);

   // Set properties
   pDocument->eType = DT_MEDIA;

   // Return document
   return pDocument;
}

PROJECT_DOCUMENT*  createProjectDocument(PROJECT_FILE*  pProjectFile)
{
   PROJECT_DOCUMENT*  pDocument = utilCreateEmptyObject(PROJECT_DOCUMENT);

   // Set properties
   pDocument->eType        = DT_PROJECT;
   pDocument->pProjectFile = pProjectFile;
   pDocument->pGameFile    = pProjectFile;

   // Return document
   return pDocument;
}

/// Function name  : createDocumentByType
// Description     : Universal method for creating any type of document
// 
// CONST DOCUMENT_TYPE  eType       : [in]            Type of document to create
// GAME_FILE*           pGameFile   : [in] [optional] Underlying document data, if appropriate. Not required for media documents
// 
// Return Value   : New document object if successful, otherwise NULL
// 
DOCUMENT*  createDocumentByType(CONST DOCUMENT_TYPE  eDocumentType, GAME_FILE*  pGameFile)
{
   CONST TCHAR  *szWindowTitles[]  = { TEXT("Script Document"), TEXT("Language Document"), TEXT("Media Document") },   // Dialog templates
                *szWindowClasses[] = { szScriptDocumentClass,   szLanguageDocumentClass,   NULL                   };   // Window procedures
   DOCUMENT     *pDocument;     // Document being created
   
   // [VERBOSE]
   VERBOSE_LIB_COMMAND();

   /// Create object
   switch (eDocumentType)
   {
   case DT_SCRIPT:   pDocument = createScriptDocument((SCRIPT_FILE*)pGameFile);      break;
   case DT_LANGUAGE: pDocument = createLanguageDocument((LANGUAGE_FILE*)pGameFile);  break;
   case DT_PROJECT:  pDocument = createProjectDocument((PROJECT_FILE*)pGameFile);    break;
   case DT_MEDIA:    pDocument = createMediaDocument();                              break;
   }

   // Set common properties
   pDocument->szFullPath = (pGameFile ? pGameFile->szFullPath : TEXT("Game Data"));
   pDocument->bVirtual   = !pGameFile;

   // [CHECK] Do not create a document window for projects
   if (eDocumentType != DT_PROJECT)
   {
      /// Create (invisible) document window. Pass document as parameter. Documents control will resize before display.
      pDocument->hWnd = CreateWindowEx(NULL, szWindowClasses[eDocumentType], szWindowTitles[eDocumentType], WS_CHILD, 0, 0, 400, 300, getMainWindowData()->hDocumentsTab, NULL, getAppInstance(), (VOID*)pDocument);

      // [DEBUG]
      ERROR_CHECK("creating document window", pDocument->hWnd);
      DEBUG_WINDOW("Document", pDocument->hWnd);

      // [CHECK] Ensure dialog was created succesfully
      if (!pDocument->hWnd)
         /// [FAILED] Return NULL if dialog could not be created
         deleteDocument(pDocument);
   }
   
   // Return new document (or NULL)
   return pDocument;
}


/// Function name  : createLanguageDocument
// Description     : Create a new LANGUAGE_DOCUMENT
// 
// HWND            hParentWnd    : [in] Window handle of the parent window (the documents control)
// LANGUAGE_FILE*  pLanguageFile : [in] Language file containing the game strings to display, or NULL to use
//                                      the main language strings loaded as game data
//
// Return type : New LANGUAGE_DOCUMENT object, you are responsible for deleting it  
//
/// RESCINDED: Kept for information purposes when I re-write the document code
//
LANGUAGE_DOCUMENT*  createLanguageDocument2(HWND  hParentWnd, LANGUAGE_FILE*  pLanguageFile)
{
   //LANGUAGE_DOCUMENT*  pNewDocument;   // New document being created
   //LVITEM              oItem;          // ListView item data for selecting items
   //HWND                hDialog;        // New dialog window handle

   ///// Create document window
   //hDialog = CreateDialog(getResourceInstance(), TEXT("LANGUAGE_DOCUMENT"), hParentWnd, dlgprocLanguageDocument);
   //ERROR_CHECK("creating language document window", hDialog);
   //
   //// [FAILED] Return NULL
   //if (hDialog == NULL)
   //   return NULL;

   ///// Create new LANGUAGE_DOCUMENT object
   //pNewDocument                      = utilCreateEmptyObject(LANGUAGE_DOCUMENT);
   //pNewDocument->pLanguageFile       = pLanguageFile;
   //pNewDocument->pCurrentMessage     = createLanguageMessage();
   //pNewDocument->eType               = DT_LANGUAGE;
   //pNewDocument->hWnd                = hDialog;
   //pNewDocument->bVirtual           = (pLanguageFile == NULL);
   //pNewDocument->oLabelData.iItem    = -1;
   //pNewDocument->oLabelData.iSubItem = -1;
   //
   //// Set title and associate data with window handle
   //utilReplaceString(pNewDocument->szTitle, pLanguageFile == NULL ? TEXT("Game Strings") : PathFindFileName(pLanguageFile->szFullPath));
   //SetWindowLong(pNewDocument->hWnd, DWL_USER, (LONG)pNewDocument);

   ///// Create RichText sub-dialog
   //pNewDocument->hRichTextDialog = CreateDialogParam(getResourceInstance(), TEXT("DOCUMENT_LANGUAGE_RICHTEXT_DIALOG"), pNewDocument->hWnd, dlgprocRichTextDialog, (LPARAM)pNewDocument);
   //ERROR_CHECK("creating RichText editing dialog", pNewDocument->hRichTextDialog);

   //// Save convenience pointers to the child controls
   //pNewDocument->hPageList    = GetDlgItem(pNewDocument->hWnd, IDC_PAGE_LIST);
   //pNewDocument->hStringList  = GetDlgItem(pNewDocument->hWnd, IDC_STRING_LIST);

   ///// Initalise ListView columns + styles
   //initLanguageDocument(pNewDocument);

   //// List all GamePages in tree and all GameStrings in first GamePage
   //ListView_SetItemCount(pNewDocument->hPageList, getLanguageDocumentGamePageTree(pNewDocument)->iCount);
   //displayLanguageDocumentGamePage(pNewDocument, 0);
   //// Display first string in RichEdit dialog
   //displayLanguageDocumentGameString(pNewDocument, 0);

   //// Select first GamePage and first GameString
   //oItem.state = oItem.stateMask = LVIS_SELECTED;
   //SendMessage(pNewDocument->hPageList, LVM_SETITEMSTATE, 0, (LPARAM)&oItem);
   //SendMessage(pNewDocument->hStringList, LVM_SETITEMSTATE, 0, (LPARAM)&oItem);

   //// Display and Return
   //ShowWindow(pNewDocument->hRichTextDialog, SW_SHOW);
   //UpdateWindow(pNewDocument->hRichTextDialog);
   return NULL;
}


/// Function name  : deleteDocument
// Description     : Universal method of destroying any type of document
// 
// DOCUMENT*  &pDocument  : [in] Document object to be destroyed
// 
VOID  deleteDocument(DOCUMENT*  &pDocument)
{
   ERROR_STACK*  pException;

   __try
   {
      // [TRACK]
      TRACK_FUNCTION();

      /// [OPTIONAL] Delete document dialog
      if (pDocument->eType != DT_PROJECT)
         utilSafeDeleteWindow(pDocument->hWnd);    // NOTE: Does not exist if a) failed during creation  b) Project file

      /// Delete document specific GameFile
      switch (pDocument->eType)
      {
      // [SCRIPT] 
      case DT_SCRIPT:
         if (pDocument->pGameFile)
            deleteScriptFile((SCRIPT_FILE*&)pDocument->pGameFile);
         break;

      // [LANGUAGE]
      case DT_LANGUAGE:
         if (pDocument->pGameFile)
            deleteLanguageFile((LANGUAGE_FILE*&)pDocument->pGameFile);
         break;

      // [PROJECT] 
      case DT_PROJECT:
         if (pDocument->pGameFile)
            deleteProjectFile((PROJECT_FILE*&)pDocument->pGameFile);
         break;

      // [MEDIA]
      case DT_MEDIA:
         break;
      }

      // Delete calling object
      utilDeleteObject(pDocument);

      // [TRACK]
      END_TRACKING();
   }
   __except (generateExceptionError(GetExceptionInformation(), pException))
   {
      // [ERROR] "An unidentified and unexpected critical error has occurred while destroying the %s document '%s'"
      enhanceError(pException, ERROR_ID(IDS_EXCEPTION_DELETE_DOCUMENT), szDocumentTypes[pDocument->eType], identifyGameFileFilename(pDocument->pGameFile));
      displayException(pException);
      // Cleanup
      pDocument = NULL;
   }
}


/// Function name  : destructorDocument
// Description     : Destroys a DOCUMENT within a list of documents
// 
// LPARAM  pDocument   : [in] DOCUMENT to destroy
// 
VOID  destructorDocument(LPARAM  pDocument)
{
   // Destroy document dialog and object
   deleteDocument((DOCUMENT*&)pDocument);
}


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : calculateFileTypeFromDocumentType
// Description     : Calculates the FileItemFlag corresponding to a DocumentType
// 
// CONST DOCUMENT_TYPE  eDocumentType   : [in] DocumentType to convert
// 
// Return Value   : Corresponding FileItemFlag
// 
FILE_ITEM_FLAG  calculateFileTypeFromDocumentType(CONST DOCUMENT_TYPE  eDocumentType)
{
   FILE_ITEM_FLAG  eOutput;

   // Examine flag indicating
   switch (eDocumentType)
   {
   case DT_SCRIPT:      eOutput = FIF_SCRIPT;      break;
   case DT_LANGUAGE:    eOutput = FIF_LANGUAGE;    break;
   case DT_MISSION:     eOutput = FIF_MISSION;     break;
   case DT_PROJECT:     eOutput = FIF_PROJECT;     break;
   }

   // Return result
   return eOutput;
}


/// Function name  : getDocumentFileName
// Description     : Returns the filename of a document
// 
// CONST DOCUMENT*  pDocument   : [in] Document
// 
// Return Value   : Document filename
// 
CONST TCHAR*  getDocumentFileName(CONST DOCUMENT*  pDocument)
{
   return PathFindFileName(pDocument->szFullPath);
}



/// Function name  : getDocumentPath
// Description     : Returns the full path of a document
// 
// CONST DOCUMENT*  pDocument   : [in] Document
// 
// Return Value   : Full path
// 
CONST TCHAR*  getDocumentPath(CONST DOCUMENT*  pDocument)
{
   return pDocument->szFullPath;
}


/// Function name  : identifyDocumentTypeIcon
// Description     : Retrieves the resource ID of the appropriate document icon
// 
// CONST DOCUMENT_TYPE  eType : [in] Document type
// 
// Return Value   : Icon ID
// 
CONST TCHAR*  identifyDocumentTypeIcon(CONST DOCUMENT_TYPE  eType)
{
   CONST TCHAR*  szOutput;

   // Examine type
   switch (eType)
   {
   case DT_SCRIPT:    szOutput = TEXT("NEW_SCRIPT_FILE_ICON");     break;
   case DT_LANGUAGE:  szOutput = TEXT("NEW_LANGUAGE_FILE_ICON");   break;
   case DT_MEDIA:     szOutput = TEXT("OPEN_SAMPLES_ICON");        break;
   case DT_MISSION:   szOutput = TEXT("NEW_MISSION_FILE_ICON");    break;
   }

   // Return icon
   return szOutput;
}


/// Function name  : isModified
// Description     : Checks whether a document is modified
// 
// CONST DOCUMENT*  pDocument : [in] Document
// 
// Return Value   : TRUE/FALSE
// 
BOOL  isModified(CONST DOCUMENT*  pDocument)
{
   return pDocument AND pDocument->bModified; 
}



/// Function name  : setDocumentModifiedFlag
// Description     : Sets a document's 'modified' flag and updates the tab title
// 
// DOCUMENT*        pDocument : [in/out] Document to modify
// CONST BOOL       bModified : [in]     TRUE to add flag, FALSE to remove it
// 
VOID   setDocumentModifiedFlag(DOCUMENT*  pDocument, CONST BOOL  bModified)
{
   // [CHECK] Do nothing if modified flag already matches
   if (isModified(pDocument) != bModified)
   {
      /// Update modified flag and title
      pDocument->bModified = bModified;
      updateDocumentTitle(pDocument);
   }
}


/// Function name  : setDocumentPath
// Description     : Modifies a Document's path, and updates the title
// 
// DOCUMENT*   pDocument : [in] Target Document
// 
VOID   setDocumentPath(DOCUMENT*  pDocument, CONST TCHAR*  szPath)
{
   // [CHECK] Has the path changed
   if (!utilCompareStringVariables(pDocument->szFullPath, szPath))
   {
      /// Change path and add 'Modified'
      setGameFilePath(pDocument->pGameFile, szPath);
      pDocument->bModified = TRUE;

      // Update title
      updateDocumentTitle(pDocument);
   }
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : closeDocument
// Description     : Determines how a document should be closed, asking the user if they wish to save their changes if necessary
// 
// DOCUMENT*  pDocument : [in] Document to close
// 
// Return Value   : DCT_SAVE_CLOSE : The document should be saved
//                  DCT_DISCARD    : The document should be destroyed
//                  DCT_ABORT      : The document should be left open and the closing sequence should be aborted
// 
CLOSURE_TYPE   closeDocument(DOCUMENT*  pDocument)
{
   FILE_DIALOG_DATA*  pFileDialogData;    // SaveAs dialog data
   STORED_DOCUMENT*   pOutputFile;        // SaveAs dialog output filepath
   CLOSURE_TYPE       eResult;            // Operation result
   TCHAR*             szInitialFolder;    // Initial folder to display in the 'save as' dialog
   CONST TCHAR*       szMessageTitle;     // StringID of MessageBox message
   UINT               iMessageID;         // MessageBox title

   // [TRACK]
   TRACK_FUNCTION();
   VERBOSE_LIB_COMMAND();

   // Prepare
   eResult = DCT_DISCARD;
   
   // [CHECK] Is document modified?  (And not virtual)
   if (!pDocument->bVirtual AND isModified(pDocument))
   {
      // Prepare
      iMessageID     = (pDocument->eType != DT_PROJECT ? IDS_GENERAL_DOCUMENT_CLOSE_CONFIRMATION : IDS_GENERAL_PROJECT_CLOSE_CONFIRMATION);
      szMessageTitle = (pDocument->eType != DT_PROJECT ? TEXT("Save Document Changes?") : TEXT("Save Project Changes?"));

      // [QUESTION] "The document/project '%s' has been modified, do you want to save your changes?"
      switch (displayMessageDialogf(NULL, iMessageID, szMessageTitle, MDF_YESNOCANCEL WITH MDF_QUESTION, getDocumentFileName(pDocument)))
      {
      /// [CANCEL] Return ABORT
      case IDCANCEL: 
         eResult = DCT_ABORT;
         break;

      /// [SAVE CHANGES] Return SAVE-CLOSE
      case IDYES: 
         // Prepare
         szInitialFolder = NULL;
         eResult         = DCT_SAVE_CLOSE;

         // [CHECK] Has the document ever been saved?
         if (pDocument->bUntitled)
         {
            /// [UNTITLED] Create OpenFile dialog data.  Display document folder if available, otherwise game folder
            pFileDialogData = createFileDialogData(FDT_SAVE, (identifyGameFileFolder(pDocument->pGameFile, szInitialFolder) ? szInitialFolder : getAppPreferences()->szLastFolder), identifyGameFileFilename(pDocument->pGameFile));
            
            // Display 'Save As' dialog
            if (displayFileDialog(pFileDialogData, getAppWindow(), getAppPreferences()->bUseSystemDialog) == IDOK)
            {
               // Prepare
               findListObjectByIndex(pFileDialogData->pOutputFileList, 0, (LPARAM&)pOutputFile);

               // Update document path
               setDocumentPath(pDocument, pOutputFile->szFullPath);
               pDocument->bUntitled = FALSE;

               // [SCRIPT] Update Script name
               if (pDocument->eType == DT_SCRIPT)
                  setScriptNameFromPath((SCRIPT_FILE*)pDocument->pGameFile);
            }
            /// [SAVE-AS CANCELLED] Return ABORT
            else
               eResult = DCT_ABORT;

            // Cleanup
            utilSafeDeleteString(szInitialFolder);
            deleteFileDialogData(pFileDialogData);
         }
         break;
      }
   }
   
   // [ABORTED] Cancel app closing sequence, if any
   if (eResult == DCT_ABORT)
      setAppClosing(FALSE);

   // Return result
   END_TRACKING();
   return eResult;
}


/// Function name  :  saveDocument
// Description     : 
// 
// DOCUMENT*   pDocument  : [in] 
// CONST BOOL  bSaveAs    : [in] 
// CONST BOOL  bSaveClose : [in] 
// 
// Return Value   : 
// 
BOOL   saveDocument(DOCUMENT*  pDocument, CONST BOOL  bSaveAs, CONST BOOL  bSaveClose)
{
   FILE_DIALOG_DATA*  pFileDialogData;    // SaveAs dialog data
   STORED_DOCUMENT*   pOutputFile;        // New document path
   TCHAR*             szInitialFolder;    // Initial SaveAs dialog folder
   BOOL               bResult;            // Operation result

   // Prepare
   TRACK_FUNCTION();
   VERBOSE_LIB_COMMAND();
   szInitialFolder = NULL;
   bResult         = FALSE;

   // [CHECK] Is document modified?
   if (!pDocument->bVirtual AND (isModified(pDocument) OR bSaveAs))
   {
      // [CHECK] Has the document ever been saved?
      if (pDocument->bUntitled OR bSaveAs)
      {
         /// [UNTITLED] Create OpenFile dialog data.  Display document folder if available, otherwise game folder
         pFileDialogData = createFileDialogData(FDT_SAVE, (identifyGameFileFolder(pDocument->pGameFile, szInitialFolder) ? szInitialFolder : getAppPreferences()->szLastFolder), identifyGameFileFilename(pDocument->pGameFile));
         
         // Display 'Save As' dialog
         if (displayFileDialog(pFileDialogData, getAppWindow(), getAppPreferences()->bUseSystemDialog) == IDOK)
         {
            // Prepare
            findListObjectByIndex(pFileDialogData->pOutputFileList, 0, (LPARAM&)pOutputFile);

            // Update document path
            setDocumentPath(pDocument, pOutputFile->szFullPath);
            pDocument->bUntitled = FALSE;

            // [SCRIPT] Update Script name
            if (pDocument->eType == DT_SCRIPT)
               setScriptNameFromPath((SCRIPT_FILE*)pDocument->pGameFile);

            /// [EVENT] Update properties dialog
            sendDocumentUpdated(AW_PROPERTIES);

            /// [SAVE] Save or Save-close document
            bResult = commandSaveDocument(getMainWindowData(), pDocument, bSaveClose, NULL);
         }

         // Cleanup
         utilSafeDeleteString(szInitialFolder);
         deleteFileDialogData(pFileDialogData);
      }
      else
         /// [SAVE] Save or Save-close document
         bResult = commandSaveDocument(getMainWindowData(), pDocument, bSaveClose, NULL);
   }

   // Return FALSE if document was not saved
   END_TRACKING();
   return bResult;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////


