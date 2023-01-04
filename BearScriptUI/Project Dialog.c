//
// FILE_NAME.cpp : FILE_DESCRIPTION
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        MACROS
/// /////////////////////////////////////////////////////////////////////////////////////////

// OnException: Display in output
#define  ON_EXCEPTION()    displayException(pException);

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createProjectDialog
// Description     : Creates the ProjectDialog 
// 
// HWND   hParentWnd  : [in] Parent window
//
// Return Value   : Window handle of the new window if successful, otherwise NULL
// 
HWND  createProjectDialog(HWND  hParentWnd)
{
   PROJECT_DIALOG_DATA*   pDialogData;       // Dialog data
   HWND                   hDialog;

   /// Create dialog data
   pDialogData = createProjectDialogData();

   /// Create dialog
   hDialog = loadDialog(TEXT("PROJECT_DIALOG"), hParentWnd, dlgprocProjectDialog, (LPARAM)pDialogData);
   ERROR_CHECK("creating project dialog", hDialog);

   // [CHECK] Destroy data if dialog failed to create
   if (!hDialog)
      deleteProjectDialogData(pDialogData);

   // [DEBUG]
   //DEBUG_WINDOW("Project Dialog", hDialog);

   // Return window handle or NULL
   return hDialog;
}


/// Function name  : createProjectDialogData
// Description     : Creates data for a ProjectDialog
// 
// Return Value   : New ProjectDialog data, you are responsible for destroying it
// 
PROJECT_DIALOG_DATA*   createProjectDialogData()
{
   PROJECT_DIALOG_DATA*   pDialogData = utilCreateEmptyObject(PROJECT_DIALOG_DATA);       // Object being created   

   // Return object
   return pDialogData;
}


/// Function name  : deleteProjectDialogData
// Description     : Destroys the data for the ProjectDialog
// 
// PROJECT_DIALOG_DATA*  &pWindowDat  : [in] ProjectDialog data to destroy
// 
VOID  deleteProjectDialogData(PROJECT_DIALOG_DATA*  &pDialogData)
{
   // Destroy calling object
   utilDeleteObject(pDialogData);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : getProjectDialogData
// Description     : Retrieves dialog data for the Project dialog
// 
// HWND  hDialog   : [in] Project dialog
// 
// Return Value   : Project dialog window data
// 
PROJECT_DIALOG_DATA*  getProjectDialogData(HWND  hDialog)
{
   return (PROJECT_DIALOG_DATA*)GetWindowLong(hDialog, DWL_USER);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : getDocumentFromProjectDialogItem
// Description     : Extracts the StoredDocument from a tree item handle. Returns NULL if item is a folder, the root, or doesn't exist
// 
// PROJECT_DIALOG_DATA*  pDialogData : [in] Dialog data
// HTREEITEM             hItem       : [in] Tree item
// 
// Return Value   : StoredDocument if any, otherwise NULL
// 
STORED_DOCUMENT*   getDocumentFromProjectDialogItem(PROJECT_DIALOG_DATA*  pDialogData, HTREEITEM  hItem)
{
   STORED_DOCUMENT*  pDocument;
   TVITEM            oItem;

   // Prepare
   pDocument = NULL;

   // Setup item request
   oItem.mask  = TVIF_PARAM;
   oItem.hItem = hItem;

   /// Lookup tree item
   if (hItem AND TreeView_GetItem(pDialogData->hTreeView, &oItem))
   {
      // [CHECK] Ensure item isn't a folder or the root
      switch (oItem.lParam)
      {
      case PF_ROOT:
      case PF_SCRIPT:
      case PF_LANGUAGE:
      case PF_MISSION:
         pDocument = NULL;
         break;

      default:
         pDocument = (STORED_DOCUMENT*)oItem.lParam;
         break;
      }
   }

   // Return result
   return pDocument;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : addDocumentToProjectDialog
// Description     : Adds a StoredDocument to the relevant folder of the TreeView
// 
// HWND                    hProjectDlg : [in] Project dialog
// CONST STORED_DOCUMENT*  pDocument   : [in] Document
// 
// Return Value   : TRUE 
// 
BOOL   addDocumentToProjectDialog(HWND  hProjectDlg, CONST STORED_DOCUMENT*  pDocument)
{
   PROJECT_DIALOG_DATA*  pDialogData;
   PROJECT_FOLDER  eFolder;
   TVINSERTSTRUCT        oInsertItem;
   HTREEITEM             hNewItem;
   TVITEM*               pItem;

   // [CHECK] Ensure project dialog exists
   ASSERT(IsWindow(hProjectDlg) AND pDocument);

   // Prepare
   pDialogData = getProjectDialogData(hProjectDlg);
   eFolder     = calculateProjectFolderFromFileType(pDocument->eType);

   /// Define item
   pItem            = &oInsertItem.item;
   pItem->mask      = TVIF_TEXT WITH TVIF_IMAGE WITH TVIF_SELECTEDIMAGE WITH TVIF_CHILDREN WITH TVIF_PARAM;
   pItem->cChildren = I_CHILDRENCALLBACK;
   pItem->pszText   = LPSTR_TEXTCALLBACK;
   pItem->iImage    = pItem->iSelectedImage = I_IMAGECALLBACK;
   pItem->lParam    = (LPARAM)pDocument;

   // Define folder
   oInsertItem.hParent      = pDialogData->hFolderNode[eFolder];
   oInsertItem.hInsertAfter = TVI_SORT;

   /// Insert item and expand folder
   hNewItem = TreeView_InsertItem(pDialogData->hTreeView, &oInsertItem);
   TreeView_Expand(pDialogData->hTreeView, pDialogData->hFolderNode[eFolder], TVE_EXPAND);

   // [CHECK] Is the file missing?
   if (!PathFileExists(pDocument->szFullPath))
      /// [MISSING] Add 'missing' overlay icon
      TreeView_SetItemState(pDialogData->hTreeView, hNewItem, INDEXTOOVERLAYMASK(1), TVIS_OVERLAYMASK WITH TVIS_STATEIMAGEMASK);   
   
   // [SUCCESS] Return TRUE
   return TRUE;
}


/// Function name  : initProjectDialog
// Description     : Intialises the window data
// 
// PROJECT_DIALOG_DATA*  pDialogData : [in] Dialog data
// HWND                  hDialog     : [in] Project dialog
// 
VOID  initProjectDialog(PROJECT_DIALOG_DATA*  pDialogData, HWND  hDialog)
{
   /// Store window data
   SetWindowLong(hDialog, DWL_USER, (LONG)pDialogData);
   pDialogData->hDialog  = hDialog;

   // Setup TreeView
   pDialogData->hTreeView = GetControl(hDialog, IDC_PROJECT_TREE);
   TreeView_SetImageList(pDialogData->hTreeView, getAppImageList(ITS_SMALL), TVSIL_NORMAL);
   TreeView_SetImageList(pDialogData->hTreeView, getAppImageList(ITS_SMALL), TVSIL_STATE);

   ImageList_SetOverlayImage(getAppImageList(ITS_SMALL), getAppImageTreeIconIndex(ITS_SMALL, TEXT("INVALID_OVERLAY")), 1);

   /// Populate TreeView
   updateProjectDialog(hDialog);
}


/// Function name  : removeDocumentFromProjectDialog
// Description     : Removes the specified Document from the TreeView
// 
// CONST DOCUMENT*  pDocument   : [in] Dialog data
// 
// Return Value   : TRUE if found, otherwise FALSE
// 
BOOL  removeDocumentFromProjectDialog(CONST DOCUMENT*  pDocument)
{
   PROJECT_DIALOG_DATA*  pDialogData;
   PROJECT_FOLDER        eFolder;
   STORED_DOCUMENT*      pStoredDocument;

   // [CHECK] Ensure dialog exists
   ASSERT(getMainWindowData()->hProjectDlg);

   // Prepare
   pDialogData = getProjectDialogData(getMainWindowData()->hProjectDlg);
   eFolder     = calculateProjectFolderFromDocumentType(pDocument->eType);

   // Manually lookup item in tree
   for (HTREEITEM hCurrentItem = TreeView_GetNextItem(pDialogData->hTreeView, pDialogData->hFolderNode[eFolder], TVGN_CHILD); hCurrentItem; hCurrentItem = TreeView_GetNextItem(pDialogData->hTreeView, hCurrentItem, TVGN_NEXT))
   {
      /// Extract StoredDocument
      pStoredDocument = getDocumentFromProjectDialogItem(pDialogData, hCurrentItem);
      
      // [CHECK]
      if (utilCompareStringVariables(pStoredDocument->szFullPath, getDocumentPath(pDocument)))
      {
         /// [FOUND] Remove item and return TRUE
         TreeView_DeleteItem(pDialogData->hTreeView, hCurrentItem);
         return TRUE;
      }
   }

   // [NOT FOUND] Return FALSE
   return FALSE;
}


/// Function name  : updateProjectDialog
// Description     : Refreshes the Treeview to reflect the current project
// 
// HWND   hDialog     : [in] Project dialog
// 
VOID  updateProjectDialog(HWND  hDialog)
{
   PROJECT_DIALOG_DATA*  pDialogData;   // Dialog data
   STORED_DOCUMENT*      pDocument;     // File list iterator
   TVINSERTSTRUCT        oInsertItem;   // Insert TreeItem wrapper
   TVITEM*               pItem;         // Convenience pointer

   // [CHECK] Ensure dialog exists
   ASSERT(IsWindow(hDialog));

   // Prepare
   pDialogData = getProjectDialogData(hDialog);

   // Define item
   pItem            = &oInsertItem.item;
   pItem->mask      = TVIF_TEXT WITH TVIF_IMAGE WITH TVIF_SELECTEDIMAGE WITH TVIF_CHILDREN WITH TVIF_PARAM;
   pItem->cChildren = I_CHILDRENCALLBACK;
   pItem->pszText   = LPSTR_TEXTCALLBACK;
   pItem->iImage    = I_IMAGECALLBACK;
   pItem->iSelectedImage = pItem->iImage;

   // [CHECK] Are we already displaying items?
   if (pDialogData->hRoot)
      /// Destroy existing contents
      TreeView_DeleteAllItems(pDialogData->hTreeView);

   /// Insert new root
   pItem->lParam       = PF_ROOT;
   oInsertItem.hParent = TVI_ROOT;
   pDialogData->hRoot  = TreeView_InsertItem(pDialogData->hTreeView, &oInsertItem);
   
   /// [PROJECT] Add project files and folders
   if (getActiveProject())
   {
      // Define folders
      oInsertItem.hInsertAfter = TVI_LAST;
      oInsertItem.hParent      = TVI_ROOT; //pDialogData->hRoot;
      
      // Iterate through all folders
      for (PROJECT_FOLDER  eFolder = PF_SCRIPT; eFolder <= PF_MISSION; eFolder = (PROJECT_FOLDER)(eFolder + 1))
      {
         /// Insert folder node
         pItem->lParam = eFolder;
         pDialogData->hFolderNode[eFolder] = TreeView_InsertItem(pDialogData->hTreeView, &oInsertItem);

         // Iterate through all files in the list
         for (LIST_ITEM*  pIterator = getListHead(getActiveProjectFile()->pFileList[eFolder]); pDocument = extractListItemPointer(pIterator, STORED_DOCUMENT); pIterator = pIterator->pNext)
            /// Insert file node
            addDocumentToProjectDialog(pDialogData->hDialog, pDocument);
      }

      // Expand Root
      TreeView_Expand(pDialogData->hTreeView, pDialogData->hRoot, TVE_EXPAND);
   }
}


/// Function name  : updateProjectDialogRoot
// Description     : Refreshes the project name to reflect the current modified status
// 
// HWND  hDialog   : [in] Project dialog
// 
VOID  updateProjectDialogRoot(HWND  hDialog)
{
   PROJECT_DIALOG_DATA*  pDialogData;   // Dialog data
   TVITEM                oItem;         // Convenience pointer

   // [CHECK] Ensure dialog exists
   ASSERT(IsWindow(hDialog));

   // Prepare
   pDialogData = getProjectDialogData(hDialog);

   // Define item
   oItem.mask    = TVIF_TEXT;
   oItem.pszText = LPSTR_TEXTCALLBACK;
   oItem.hItem   = pDialogData->hRoot;
   TreeView_SetItem(pDialogData->hTreeView, &oItem);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onProjectDialog_Close
// Description     : Command the main window to toggle the visibility of the dialog
// 
// PROJECT_DIALOG_DATA*  pDialogData : [in] Window data
//
VOID  onProjectDialog_Close(PROJECT_DIALOG_DATA*  pDialogData)
{
   // Send 'Toggle Output Dialog' to MainWindow
   SendMessage(getAppWindow(), WM_COMMAND, IDM_VIEW_PROJECT_EXPLORER, NULL);
}


/// Function name  : onProjectDialog_ContextMenu
// Description     : Project dialog context menu
// 
// PROJECT_DIALOG_DATA*  pDialogData : [in] Dialog data
// HWND                  hCtrl       : [in] Control the user clicked
// CONST POINT*          ptCursor    : [in] Cursor position in screen co-ordinates
// 
VOID  onProjectDialog_ContextMenu(PROJECT_DIALOG_DATA*  pDialogData, HWND  hCtrl, CONST POINT*  ptCursor)
{
   STORED_DOCUMENT*  pItemDocument;       // StoredDocument associated with the item clicked
   CUSTOM_MENU*      pCustomMenu;         // Custom Popup menu
   HTREEITEM         hSelectedItem;       // Selected tree item (if any)
   DOCUMENT*         pDocument;           // Document iterator
   PROJECT_DOCUMENT* pProject;            // Current project, if any
   BOOL              bIncludeDocs;        // Whether to enable 'IncludeAllDocuments' item

   // [CHECK] Ensure source was the TreeView
   if (GetWindowID(hCtrl) == IDC_PROJECT_TREE)
   {
      // [TRACK]
      CONSOLE_ACTION();

      // Prepare
      pProject      = getActiveProject();
      hSelectedItem = TreeView_GetSelection(pDialogData->hTreeView);
      pItemDocument = getDocumentFromProjectDialogItem(pDialogData, hSelectedItem);
      bIncludeDocs  = FALSE;

      // Create CustomMenu
      pCustomMenu = createCustomMenu(TEXT("DIALOG_MENU"), TRUE, IDM_PROJECT_POPUP);

      /// [REMOVE DOCUMENT] Ensure a document is selected
      EnableMenuItem(pCustomMenu->hPopup, IDM_PROJECT_REMOVE_DOCUMENT, pProject AND pItemDocument ? MF_ENABLED : MF_DISABLED);
      /// [OPEN DOCUMENT] Ensure document is not already open
      EnableMenuItem(pCustomMenu->hPopup, IDM_PROJECT_OPEN_DOCUMENT, pProject AND pItemDocument AND !isDocumentOpen(pItemDocument->szFullPath) ? MF_ENABLED : MF_DISABLED);
      
      // [CHECK] Are any open documents not included in the project?
      for (UINT iIndex = 0; !bIncludeDocs AND findDocumentByIndex(getMainWindowData()->hDocumentsTab, iIndex, pDocument); iIndex++)
         if (!isDocumentInProject(pDocument))
            bIncludeDocs = TRUE;

      /// [INCLUDE ALL DOCUMENTS] 
      EnableMenuItem(pCustomMenu->hPopup, IDM_PROJECT_ADD_ALL_DOCUMENTS, bIncludeDocs ? MF_ENABLED : MF_DISABLED);

      /// [EDIT VARIABLES] Ensure project exists
      EnableMenuItem(pCustomMenu->hPopup, IDM_PROJECT_EDIT_VARIABLES, pProject ? MF_ENABLED : MF_DISABLED);
      
      /// Display the pop-up menu
      TrackPopupMenu(pCustomMenu->hPopup, TPM_LEFTALIGN WITH TPM_TOPALIGN WITH TPM_LEFTBUTTON, ptCursor->x, ptCursor->y, NULL, pDialogData->hDialog, NULL);
      
      // Cleanup
      deleteCustomMenu(pCustomMenu);
   }
}


/// Function name  : onProjectDialog_Command
// Description     : Handles the context menu items
// 
// PROJECT_DIALOG_DATA*  pDialogData   : [in] Dialog data
// CONST UINT            iControl      : [in] Control source
// CONST UINT            iNotification : [in] Notification
// HWND                  hCtrl         : [in] Control
// 
BOOL  onProjectDialog_Command(PROJECT_DIALOG_DATA*  pDialogData, CONST UINT  iControl, CONST UINT  iNotification, HWND  hCtrl)
{
   STORED_DOCUMENT*  pStoredDocument;   // StoredDocument being removed
   DOCUMENT*         pDocument;         // Document iterator
   HTREEITEM         hSelectedItem;     // Selected tree item
   BOOL              bResult;           // Operation result

   // Prepare
   bResult = FALSE;

   // Examine control
   switch (iControl)
   {
   /// [REMOVE SELECTED]
   case IDM_PROJECT_REMOVE_DOCUMENT:
      CONSOLE_MENU(IDM_PROJECT_REMOVE_DOCUMENT);
      // Retrieve item and StoredDocument
      hSelectedItem   = TreeView_GetSelection(pDialogData->hTreeView);
      pStoredDocument = getDocumentFromProjectDialogItem(pDialogData, hSelectedItem);
      debugStoredDocument(pStoredDocument);

      // [CHECK] Ensure both exist
      if (hSelectedItem AND pStoredDocument)
      {
         /// Remove item and StoredDocument
         TreeView_DeleteItem(pDialogData->hTreeView, hSelectedItem);
         removeFileFromProject(getActiveProjectFile(), pStoredDocument);

         // [MODIFIED] Set modified flag
         setProjectModifiedFlag(getActiveProject(), TRUE);
         bResult = TRUE;
      }
      break;

   /// [OPEN SELECTED]
   case IDM_PROJECT_OPEN_DOCUMENT:
      CONSOLE_MENU(IDM_PROJECT_OPEN_DOCUMENT);

      // Retrieve item and StoredDocument
      hSelectedItem   = TreeView_GetSelection(pDialogData->hTreeView);
      pStoredDocument = getDocumentFromProjectDialogItem(pDialogData, hSelectedItem);
      debugStoredDocument(pStoredDocument);
      
      // [CHECK] Ensure both exist
      if (hSelectedItem AND pStoredDocument)
      {
         /// [SUCCESS] Open document
         commandLoadDocument(getMainWindowData(), pStoredDocument->eType, pStoredDocument->szFullPath, TRUE, NULL);
         bResult = TRUE;
      }
      break;

   /// [ADD ALL DOCUMENTS]
   case IDM_PROJECT_ADD_ALL_DOCUMENTS:
      CONSOLE_MENU(IDM_PROJECT_ADD_ALL_DOCUMENTS);

      // Iterate through all documents
      for (UINT iIndex = 0; findDocumentByIndex(getMainWindowData()->hDocumentsTab, iIndex, pDocument); iIndex++)
         // [CHECK] Include current document if possible
         if (!isDocumentInProject(pDocument))
            addDocumentToProject(pDocument);
      break;

   /// [EDIT VARIABLES] Display project variables dialog
   case IDM_PROJECT_EDIT_VARIABLES:
      CONSOLE_MENU(IDM_PROJECT_EDIT_VARIABLES);

      displayProjectVariablesDialog(getAppWindow());
      bResult = TRUE;
      break;
   }

   // Return result
   return bResult;
}

/// Function name  : onProjectDialog_CustomDraw
// Description     : 
// 
// PROJECT_DIALOG_DATA*  pDialogData   : [in] 
// NMTVCUSTOMDRAW*  pMessage   : [in] 
// 
// Return Value   : 
//
/// *** WORK IN PROGRESS ***
/// TEMPORARILY RESCINDED
// 
BOOL  onProjectDialog_CustomDraw(PROJECT_DIALOG_DATA*  pDialogData, NMTVCUSTOMDRAW*  pMessage)
{
   NMCUSTOMDRAW*  pDrawData;
   BOOL           iResult;
   TVITEM  oItem;

   iResult = CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYPOSTERASE | CDRF_NOTIFYPOSTPAINT;  //CDRF_NOTIFYITEMERASE
   pDrawData = &pMessage->nmcd;

   switch (pMessage->nmcd.dwDrawStage)
   {
   /// [START]
   // [Control about to be drawn]
   case CDDS_PREPAINT:
      //VERBOSE("CDDS_PREPAINT");
      iResult = CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYPOSTPAINT;
      break;

   // [Control drawn]
   case CDDS_POSTPAINT:
      //VERBOSE("CDDS_POSTPAINT");
      iResult = CDRF_SKIPPOSTPAINT;
      break;

   // [Item about to be drawn]
   case CDDS_PREPAINT WITH CDDS_ITEM:
      //VERBOSE("CDDS_PREPAINT WITH CDDS_ITEM");
      iResult = CDRF_DODEFAULT | CDRF_NOTIFYPOSTPAINT;
      break;

   // [Item drawn]
   case CDDS_POSTPAINT WITH CDDS_ITEM:
      //VERBOSE("CDDS_POSTPAINT WITH CDDS_ITEM");
      
      {
         pDrawData->rc.left = pDrawData->dwItemSpec;
         SendMessage(pDialogData->hTreeView, TVM_GETITEMRECT, TRUE, (LPARAM)&pDrawData->rc);

         // Prepare
         oItem.pszText = utilCreateEmptyString(64);
         oItem.cchTextMax = 64;
         oItem.mask = TVIF_TEXT WITH TVIF_IMAGE;
         oItem.hItem = (HTREEITEM)pDrawData->dwItemSpec;
         TreeView_GetItem(pDialogData->hTreeView, &oItem);

         RECT  rcNew = pDrawData->rc;

         InflateRect(&pDrawData->rc, +2, NULL);
         pDrawData->rc.right += 1;
         pDrawData->rc.left -= 20;

         
         pDrawData->rc.bottom++;
         // [BACKGROUND]
         if (pDrawData->uItemState INCLUDES CDIS_SELECTED)
            drawCustomSelectionRectangle(pDrawData->hdc, &pDrawData->rc);
         else
            utilFillSysColourRect(pDrawData->hdc, &pDrawData->rc, COLOR_WINDOW);
         pDrawData->rc.bottom--;

         pDrawData->rc.left += 2;
         drawIcon(getAppImageList(ITS_SMALL), oItem.iImage, pDrawData->hdc, pDrawData->rc.left, pDrawData->rc.top, IS_NORMAL);
         
         // [TEXT]
         SetBkMode(pDrawData->hdc, TRANSPARENT);
         //InflateRect(&pDrawData->rc, -2, NULL);
         pDrawData->rc.left += 18;
         DrawText(pDrawData->hdc, oItem.pszText, lstrlen(oItem.pszText), &pDrawData->rc, DT_LEFT);
         SetBkMode(pDrawData->hdc, OPAQUE);

         // Cleanup
         pDrawData->rc = rcNew;
         utilDeleteString(oItem.pszText);
      }

      iResult = CDRF_SKIPDEFAULT | CDRF_SKIPPOSTPAINT;
      break;

   }

   return iResult;
}

/// Function name  : onProjectDialog_Destroy
// Description     : Destroys the window data
// 
// PROJECT_DIALOG_DATA* pDialogData   : [in] Window data
// 
VOID  onProjectDialog_Destroy(PROJECT_DIALOG_DATA*  pDialogData)
{
   // Sever and delete the window data
   SetWindowLong(pDialogData->hDialog, DWL_USER, NULL);
   deleteProjectDialogData(pDialogData);
}


/// Function name  : onProjectDialog_GetMenuItemState
// Description     : Determines whether a toolbar/menu command relating to the document should be enabled or disabled
// 
// PROJECT_DIALOG_DATA*  pDialogData : [in]     Dialog data
// CONST UINT            iCommandID  : [in]     Menu/toolbar Command
// UINT*                 piState     : [in/out] Combination of MF_ENABLED, MF_DISABLED, MF_CHECKED, MF_UNCHECKED
// 
BOOL   onProjectDialog_GetMenuItemState(PROJECT_DIALOG_DATA*  pDialogData, CONST UINT  iCommandID, UINT*  piState)
{
   // Return DISABLED
   *piState = MF_DISABLED;
   SetWindowLong(pDialogData->hDialog, DWL_MSGRESULT, TRUE);

   return TRUE;
}


/// Function name  : onProjectDialog_Moving
// Description     : Prevents the dialog being dragged around by it's caption
// 
// PROJECT_DIALOG_DATA*  pDialogData  : [in]    Dialog data
// RECT*                 pWindowRect  : [in/out] Window rectangle
// 
// Return Value   : TRUE if processed, otherwise FALSE
// 
BOOL  onProjectDialog_Moving(PROJECT_DIALOG_DATA*  pDialogData, RECT*  pWindowRect)
{
   PANE*    pOutputPane;      // Workspace pane containing the dialog
   BOOL     bResult;          // Operation result

   // Prepare
   bResult = TRUE;

   // [CHECK] Lookup workspace pane containing the dialog
   if (pWindowRect AND findWorkspacePaneByHandle(getMainWindowData()->hWorkspace, pDialogData->hDialog, NULL, NULL, pOutputPane))
   {
      /// Force dialog rectangle to be equal to the pane rectangle
      *pWindowRect = pOutputPane->rcPane;

      // Convert to screen co-ordinates
      utilScreenToClientRect(pDialogData->hDialog, pWindowRect);
   }
   else
      // [FAILED] Return FALSE
      bResult = FALSE;

   // Return result
   return bResult;
}


/// Function name  : onProjectDialog_Notify
// Description     : Notification processing
// 
// PROJECT_DIALOG_DATA*  pDialogData : [in]     Dialog data
// NMHDR*                pMessage    : [in/out] Message header
// 
// Return Value   : TRUE if processed, otherwise FALSE
// 
BOOL  onProjectDialog_Notify(PROJECT_DIALOG_DATA*  pDialogData, NMHDR*  pMessage)
{
   TVHITTESTINFO  oHitTest;
   BOOL           bResult = FALSE;

   TRY
   {
      // Examine code
      switch (pMessage->code)
      {
      /// [CUSTOM DRAW]
      case NM_CUSTOMDRAW:
         /*if (pDialogData AND (bResult = onProjectDialog_CustomDraw(pDialogData, (NMTVCUSTOMDRAW*)pMessage)))
         {
            SetWindowLong(pDialogData->hDialog, DWL_MSGRESULT, bResult);
            bResult = TRUE;
         }*/
         break;

      /// [RIGHT-CLICK] Select item, ensuring the context menu is displayed for item beneath cursor, not item currently selected
      case NM_RCLICK:
         // Perform hit-test
         utilGetWindowCursorPos(pDialogData->hTreeView, &oHitTest.pt);
         if (TreeView_HitTest(pDialogData->hTreeView, &oHitTest))
         {
            // [FOUND] Select item beneath the cursor
            TreeView_SelectItem(pDialogData->hTreeView, oHitTest.hItem);
            //VERBOSE("TreeView: NM_RCLICK");
         }
         break;

      /// [DOUBLE CLICK] Open selected document
      case NM_DBLCLK:
         onProjectDialog_Command(pDialogData, IDM_PROJECT_OPEN_DOCUMENT, NULL, NULL);
         break;

      /// [FOCUSED]
      case NM_SETFOCUS:
         CONSOLE("ProjectDialog ListView receiving input focus");
         break;

      /// [REQUEST DATA]
      case TVN_GETDISPINFO:
         onProjectDialog_RequestData(pDialogData, (NMTVDISPINFO*)pMessage);
         bResult = TRUE;
         break;
      }

      // Return result
      return bResult;
   }
   CATCH1("pMessage->code=%d", pMessage->code);
   return FALSE;
}


/// Function name  : onProjectDialog_RequestData
// Description     : 
// 
// PROJECT_DIALOG_DATA*  pDialogData : [in]     Dialog data
// NMTVDISPINFO*         pHeader     : [in/out] Item data
// 
VOID  onProjectDialog_RequestData(PROJECT_DIALOG_DATA*  pDialogData, NMTVDISPINFO*  pHeader)
{
   PROJECT_FOLDER    eFolder;
   STORED_DOCUMENT*  pStoredDoc;
   TVITEM*           pItem;         // Convenience pointer

   TRY
   {
      // Prepare
      pItem = &pHeader->item;

      // Examine node
      switch (eFolder = (PROJECT_FOLDER)pItem->lParam)
      {
      /// [ROOT]
      case PF_ROOT:
         if (pItem->mask INCLUDES TVIF_TEXT)
         {
            if (getActiveProject())
               StringCchPrintf(pItem->pszText, pItem->cchTextMax, isModified(getActiveProject()) ? TEXT("%s *") : TEXT("%s"), getDocumentFileName(getActiveProject()));
            else
               StringCchCopy(pItem->pszText, pItem->cchTextMax, loadTempString(IDS_PROJECT_NOT_LOADED));
         }

         if (pItem->mask INCLUDES (TVIF_IMAGE WITH TVIF_SELECTEDIMAGE))
            pItem->iImage = pItem->iSelectedImage = getAppImageTreeIconIndex(ITS_SMALL, TEXT("NEW_PROJECT_FILE_ICON")); 

         if (pItem->mask INCLUDES TVIF_CHILDREN)
            pItem->cChildren = (getActiveProject() ? 1 : 0);
         break;

      /// [PROJECT FOLDER]
      case PF_SCRIPT:
      case PF_LANGUAGE:
      case PF_MISSION:
         if (pItem->mask INCLUDES TVIF_TEXT)
            StringCchCopy(pItem->pszText, pItem->cchTextMax, loadTempString(IDS_PROJECT_FOLDER_SCRIPT + eFolder));

         if (pItem->mask INCLUDES (TVIF_IMAGE WITH TVIF_SELECTEDIMAGE))
            pItem->iImage = pItem->iSelectedImage = getAppImageTreeIconIndex(ITS_SMALL, TEXT("FOLDERS_ICON"));

         if (pItem->mask INCLUDES TVIF_CHILDREN)
            pItem->cChildren = (getProjectFileCountByFolder(getActiveProjectFile(), eFolder) ? 1 : 0);
         break;

      /// [PROJECT ITEM]
      default:
         // Prepare
         pStoredDoc = (STORED_DOCUMENT*)pItem->lParam;

         if (pItem->mask INCLUDES TVIF_TEXT)
            StringCchCopy(pItem->pszText, pItem->cchTextMax, PathFindFileName(pStoredDoc->szFullPath));

         if (pItem->mask INCLUDES (TVIF_IMAGE WITH TVIF_SELECTEDIMAGE))
         {
            switch (pStoredDoc->eType)
            {
            case FIF_SCRIPT:    pItem->iImage = getAppImageTreeIconIndex(ITS_SMALL, TEXT("NEW_SCRIPT_FILE_ICON"));     break;
            case FIF_LANGUAGE:  pItem->iImage = getAppImageTreeIconIndex(ITS_SMALL, TEXT("NEW_LANGUAGE_FILE_ICON"));   break;
            case FIF_MISSION:   pItem->iImage = getAppImageTreeIconIndex(ITS_SMALL, TEXT("NEW_MISSION_FILE_ICON"));    break;
            }
            pItem->iSelectedImage = pItem->iImage;
         }

         if (pItem->mask INCLUDES TVIF_CHILDREN)
            pItem->cChildren = 0;
         break;
      }
   }
   CATCH2("eFolder=%d  pItem->mask=%d", eFolder, pItem->mask);
}


/// Function name  : onProjectDialog_Resize
// Description     : Stretch the tree control over the entire dialog
// 
// PROJECT_DIALOG_DATA*  pDialogData : [in] Window data
// CONST SIZE*           pNewSize    : [in] New size
// 
VOID  onProjectDialog_Resize(PROJECT_DIALOG_DATA*  pDialogData, CONST SIZE*  pNewSize)
{
   RECT  rcClientRect;     // Dialog client rectangle        
   
   // Prepare
   GetClientRect(pDialogData->hDialog, &rcClientRect);

   /// Stretch TreeView control over entire dialog
   utilSetWindowRect(pDialogData->hTreeView, &rcClientRect, TRUE);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : dlgprocProjectDialog
// Description     : Dialog procedure for the ProjectDialog
// 
// 
INT_PTR  dlgprocProjectDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   PROJECT_DIALOG_DATA*  pDialogData;      // Window data
   POINT                 ptCursor;         // Cursor position
   SIZE                  siWindow;         // Window size
   BOOL                  bResult = TRUE;   // Operation result

   TRY
   {
      // Prepare
      pDialogData = getProjectDialogData(hDialog);

      // Examine message
      switch (iMessage)
      {
      /// [CREATE] Create and store window data
      case WM_INITDIALOG:
         initProjectDialog((PROJECT_DIALOG_DATA*)lParam, hDialog);
         break;

      /// [DESTROY] Cleanup the window data
      case WM_DESTROY:
         onProjectDialog_Destroy(pDialogData);
         break;

      /// [CLOSE] Block message and invoke the main window's "Show/Hide Output" command
      case WM_CLOSE:
         onProjectDialog_Close(pDialogData);
         bResult = FALSE; 
         break;

      /// [COMMAND]
      case WM_COMMAND:
         bResult = onProjectDialog_Command(pDialogData, LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
         break;

      /// [NOTIFICATION] 
      case WM_NOTIFY:
         bResult = onProjectDialog_Notify(pDialogData, (NMHDR*)lParam);
         break;

      /// [CONTEXT MENU]
      case WM_CONTEXTMENU:
         ptCursor.x = GET_X_LPARAM(lParam);
         ptCursor.y = GET_Y_LPARAM(lParam);
         onProjectDialog_ContextMenu(pDialogData, (HWND)wParam, &ptCursor);
         break;

      /// [MENU ITEM HOVER] Forward messages up the chain to the Main window
      case WM_MENUSELECT:
         sendAppMessage(AW_MAIN, WM_MENUSELECT, wParam, lParam);
         break;

      /// [MENU ITEM STATE]
      case UM_GET_MENU_ITEM_STATE:
         bResult = onProjectDialog_GetMenuItemState(pDialogData, wParam, (UINT*)lParam);
         break;

      /// [MOVING] Prevent window from being dragged
      case WM_MOVING:
         bResult = onProjectDialog_Moving(pDialogData, (RECT*)lParam);
         break;

      /// [RECEIVE FOCUS] Focus TreeView
      case WM_SETFOCUS:
         if (pDialogData)
            SetFocus(pDialogData->hTreeView);
         break;

      /// [NON-CLIENT PAINT]
      case WM_NCPAINT:
         onDialog_PaintNonClient(pDialogData->hDialog, (HRGN)wParam);
         break;

      /// [RESIZE] Stretch Treeview
      case WM_SIZE:
         siWindow.cx = LOWORD(lParam);
         siWindow.cy = HIWORD(lParam);
         onProjectDialog_Resize(pDialogData, &siWindow);
         break;

      /// [HELP] Invoke help
      case WM_HELP:
         displayHelp(TEXT("Project_Overview"));
         break;

      /// [CUSTOM MENU]
      case WM_DRAWITEM:    bResult = onWindow_DrawItem((DRAWITEMSTRUCT*)lParam);            break;
      case WM_MEASUREITEM: bResult = onWindow_MeasureItem(hDialog, (MEASUREITEMSTRUCT*)lParam);  break;

      // [UNHANDLED] 
      default:
         bResult = FALSE;
         break;
      }

      // [FOCUS HANDLER]
      updateMainWindowToolBar(iMessage, wParam, lParam);

      // Return return
      return bResult;
   }
   /// [EXCEPTION HANDLER]
   CATCH3("iMessage=%s  wParam=%d  lParam=%d", identifyMessage(iMessage), wParam, lParam);
   return FALSE;
}

