//
// Language Document Base.cpp : Implements the creation and display of language document windows
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       CONSTANTS / GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

#define       COLUMN_ID           0      // Page ID column 
#define       COLUMN_TEXT         1      // Text column
#define       COLUMN_TITLE        1      // Title column 
#define       COLUMN_DESCRIPTION  2      // Description column

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       CREATION / DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createLanguageDialogControls
// Description     : Creates the Workspace and ListViews
// 
// LANGUAGE_DOCUMENT*  pDocument : [in] LanguaegDocument
// 
// Return Value   : TRUE if successful, otherwise FALSE
// 
BOOL   createLanguageDialogControls(LANGUAGE_DOCUMENT*  pDocument)
{
   DWORD             dwListStyle     = WS_BORDER WITH WS_CHILD WITH WS_TABSTOP WITH (LVS_OWNERDATA WITH LVS_SHOWSELALWAYS WITH LVS_REPORT WITH LVS_SINGLESEL WITH LVS_NOSORTHEADER);
   LISTVIEW_COLUMNS  oPageListView   = { 3, IDS_LANGUAGE_STRING_COLUMN_ID, 60, 160, 100,  NULL, NULL },
                     oStringListView = { 2, IDS_LANGUAGE_STRING_COLUMN_ID, 70, 600, NULL, NULL, NULL };
   PANE_PROPERTIES   oPaneData;       // New pane properties
   PANE*             pTargetPane;     // Workspace pane being targetted for a split
   RECT              rcClient;        // Dialog client rectangle
   SIZE              siClient;        // Size of dialog rectangle
                     
   // Prepare
   TRACK_FUNCTION();
   GetClientRect(pDocument->hWnd, &rcClient);
   utilConvertRectangleToSize(&rcClient, &siClient);

   /// [PAGE LIST] Create ListView
   pDocument->hPageList = CreateWindow(szGroupedListViewClass, TEXT("GamePage List"), dwListStyle, 0, 0, siClient.cx, siClient.cy, pDocument->hWnd, (HMENU)IDC_PAGE_LIST, getAppInstance(), NULL);
   ERROR_CHECK("Creating GamePage listview", pDocument->hPageList);
   initReportModeListView(pDocument->hPageList, &oPageListView, FALSE);
   ListView_SetBkColor(pDocument->hPageList, getThemeSysColour(TEXT("TAB"), COLOR_WINDOW));

   // Define ListView groups
   for (UINT  iGroup = 0; iGroup < LANGUAGE_GROUP_COUNT; iGroup++)
      GroupedListView_AddGroup(pDocument->hPageList, createGroupedListViewGroup(iGroup, IDS_FIRST_LANGUAGE_GROUP + iGroup));
   
   /// [STRING LIST] Create ListView
   pDocument->hStringList = CreateWindow(WC_LISTVIEW, TEXT("GameString List"), dwListStyle, 0, 0, siClient.cx, siClient.cy, pDocument->hWnd, (HMENU)IDC_STRING_LIST, getAppInstance(), NULL);
   ERROR_CHECK("Creating GameString listview", pDocument->hStringList);
   initReportModeListView(pDocument->hStringList, &oStringListView, FALSE);
   SubclassWindow(pDocument->hStringList, wndprocCustomListView);
   ListView_SetBkColor(pDocument->hStringList, getThemeSysColour(TEXT("TAB"), COLOR_WINDOW));

   /// [RICH EDIT] Create RichEdit Dialog
   pDocument->hRichTextDialog = CreateDialogParam(getResourceInstance(), TEXT("RICHTEXT_DIALOG"), pDocument->hWnd, dlgprocRichTextDialog, (LPARAM)pDocument);
   ERROR_CHECK("Creating RichEdit Dialog", pDocument->hRichTextDialog);

   /// [WORKSPACE]
   pDocument->hWorkspace = createWorkspace(pDocument->hWnd, &rcClient, pDocument->hPageList, getTabThemeColour());

   // Add GameString | GamePage split
   if (findWorkspacePaneByHandle(pDocument->hWorkspace, pDocument->hPageList, NULL, NULL, pTargetPane))
   {
      setWorkspacePaneProperties(&oPaneData, FALSE, NULL, 0.3f);
      insertWorkspaceWindow(pDocument->hWorkspace, pTargetPane, pDocument->hStringList, RIGHT, &oPaneData);

      // Add GameString -- RichText split
      if (findWorkspacePaneByHandle(pDocument->hWorkspace, pDocument->hStringList, NULL, NULL, pTargetPane))
      {
         setWorkspacePaneProperties(&oPaneData, FALSE, NULL, 0.6f);
         insertWorkspaceWindow(pDocument->hWorkspace, pTargetPane, pDocument->hRichTextDialog, BOTTOM, &oPaneData);
      }
   }

   // [TRACK]
   END_TRACKING();
   return pDocument->hWorkspace AND pDocument->hPageList AND pDocument->hStringList AND pDocument->hRichEdit;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                            HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : findLanguageDocumentGameStringByIndex
// Description     : Finds in a string within the current Page
// 
// LANGUAGE_DOCUMENT*  pDocument : [in]  Language document data
// CONST UINT          iIndex    : [in]  Index of the desired GameString
// GAME_STRING*       &pOutput   : [out] GameString, if found
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BOOL  findLanguageDocumentGameStringByIndex(LANGUAGE_DOCUMENT*  pDocument, CONST UINT  iIndex, GAME_STRING* &pOutput)
{
   // Query tree
   return findObjectInAVLTreeByIndex(pDocument->pPageStringsByID, iIndex, (LPARAM&)pOutput);
}


/// Function name  : findLanguageDocumentGamePageByID
// Description     : Find the page within the underlying LanguageFile
// 
// LANGUAGE_DOCUMENT*  pDocument : [in]  Language document data
// CONST UINT          iIndex    : [in]  PageID of the desired GameString
// GAME_PAGE*         &pOutput   : [out] GamePage, if found
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BOOL  findLanguageDocumentGamePageByID(LANGUAGE_DOCUMENT*  pDocument, CONST UINT  iPageID, GAME_PAGE* &pOutput)
{
   // Query tree
   return findObjectInAVLTreeByValue(getLanguageDocumentGamePageTree(pDocument), iPageID, NULL, (LPARAM&)pOutput);
}


/// Function name  : findLanguageDocumentGamePageByIndex
// Description     : Find the page within the underlying LanguageFile
// 
// LANGUAGE_DOCUMENT*  pDocument : [in]  Language document data
// CONST UINT          iIndex    : [in]  Index of the desired GameString
// GAME_PAGE*         &pOutput   : [out] GamePage, if found
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BOOL  findLanguageDocumentGamePageByIndex(LANGUAGE_DOCUMENT*  pDocument, CONST UINT  iIndex, GAME_PAGE* &pOutput)
{
   // Query tree
   return findObjectInAVLTreeByIndex(getLanguageDocumentGamePageTree(pDocument), iIndex, (LPARAM&)pOutput);
}


/// Function name  : getLanguageDocumentGameStringTree
// Description     : Retrieve the entire GameStrings tree for a language document. If the document is displaying
//                    the main language file then this returns the global game data strings tree.
// 
// LANGUAGE_DOCUMENT*  pDocument  : [in] Language document data
// 
// Return Value   : GameStrings tree ordered by ID
// 
AVL_TREE*   getLanguageDocumentGameStringTree(LANGUAGE_DOCUMENT*  pDocument)
{
   return pDocument->pLanguageFile ? pDocument->pLanguageFile->pGameStringsByID : getGameData()->pGameStringsByID;
}

/// Function name  : getLanguageDocumentGamePageTree
// Description     : Retrieve the GamePages tree for a language document. If the document is displaying the main
//                    language file then this returns the global game data GamePages tree.
// 
// LANGUAGE_DOCUMENT*  pDocument   : [in] Language document data
// 
// Return Value   : GamePages tree ordered by ID
// 
AVL_TREE*   getLanguageDocumentGamePageTree(LANGUAGE_DOCUMENT*  pDocument)
{
   return pDocument->pLanguageFile ? pDocument->pLanguageFile->pGamePagesByID : getGameData()->pGamePagesByID;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                            FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : deleteLanguageDocumentGamePage
// Description     : Destroys the specified GamePage and all strings with it's PageID
// 
// LANGUAGE_DOCUMENT*  pDocument : [in]     Document
// GAME_PAGE*          pPage     : [in/out] GamePage to destroy
// 
VOID  deleteLanguageDocumentGamePage(LANGUAGE_DOCUMENT*  pDocument, GAME_PAGE*  pPage)
{
   AVL_TREE_OPERATION*  pOperation   = createAVLTreeOperation(treeprocDeleteGameStringPageID, ATT_INORDER);
   AVL_TREE*            pPageStrings = generateLanguagePageStringsTree(pDocument, pPage);

   /// Delete all strings with input PageID
   pOperation->pOutputTree = getLanguageDocumentGameStringTree(pDocument);
   performOperationOnAVLTree(pPageStrings, pOperation);

   /// Delete GamePage
   destroyObjectInAVLTreeByValue(getLanguageDocumentGamePageTree(pDocument), pPage->iPageID, NULL);

   // Re-Index Page tree
   performAVLTreeIndexing(getLanguageDocumentGamePageTree(pDocument));

   // Update PageList Count
   /// REM: ListView_SetItemCount(pDocument->hPageList, getTreeNodeCount(getLanguageDocumentGamePageTree(pDocument)));
   updateLanguageDocumentPageGroups(pDocument);

   // Cleanup
   deleteAVLTree(pPageStrings);
   deleteAVLTreeOperation(pOperation);
}


/// Function name  : deleteLanguageDocumentGameString
// Description     : Destroys the specified GameString
// 
// LANGUAGE_DOCUMENT*  pDocument : [in]     Document
// GAME_STRING*        pString   : [in/out] GameString to destroy
// 
VOID  deleteLanguageDocumentGameString(LANGUAGE_DOCUMENT*  pDocument, GAME_STRING*  &pString)
{
   /// Delete string 
   removeObjectFromAVLTreeByValue(pDocument->pPageStringsByID, pString->iID, NULL, (LPARAM&)pString);
   destroyObjectInAVLTreeByValue(getLanguageDocumentGameStringTree(pDocument), pString->iPageID, pString->iID);
   pString = NULL;

   /// Update ListView
   performAVLTreeIndexing(pDocument->pPageStringsByID);
   ListView_SetItemCount(pDocument->hStringList, getTreeNodeCount(pDocument->pPageStringsByID));
}


/// Function name  : generateLanguagePageStringsTree
// Description     : Create a GameString tree containing only strings for a specified page
// 
// LANGUAGE_DOCUMENT*  pDocument : [in] Document
// CONST GAME_PAGE*    pGamePage : [in] Page
// 
// Return Value   : New AVL tree, you are responsible for destroying it
// 
AVL_TREE*   generateLanguagePageStringsTree(LANGUAGE_DOCUMENT*  pDocument, CONST GAME_PAGE*  pGamePage)
{
   AVL_TREE_OPERATION*  pOperation  = createAVLTreeOperation(treeprocGeneratePageStrings, ATT_INORDER);
   AVL_TREE*            pOutputTree = createAVLTree(extractGameStringTreeNode, NULL, createAVLTreeSortKey(AK_ID, AO_ASCENDING), NULL, NULL);    // Shallow Copy

   // Prepare
   pOperation->xFirstInput = pGamePage->iPageID;
   pOperation->pOutputTree = pOutputTree;

   /// Copy strings with matching PageID, Index resultant tree
   performOperationOnAVLTree(getLanguageDocumentGameStringTree(pDocument), pOperation);
   performAVLTreeIndexing(pOutputTree);
      
   // Cleanup and return tree
   deleteAVLTreeOperation(pOperation);
   return pOutputTree;
}



/// Function name  : identifyLanguagePageStringNextID
// Description     : Calculates the next ID available in the current page
// 
// LANGUAGE_DOCUMENT*  pDocument : [in] Document
// 
// Return Value   : Next ID
// 
UINT  identifyLanguagePageStringNextID(LANGUAGE_DOCUMENT*  pDocument)
{
   AVL_TREE_OPERATION*  pOperation   = createAVLTreeOperation(treeprocFindFreeGameStringID, ATT_INORDER);
   UINT                 iResult;

   // [CHECK] Ensure PageStrings exist
   ASSERT(pDocument->pPageStringsByID);

   /// Calculate next availble ID
   iResult = performOperationOnAVLTree(pDocument->pPageStringsByID, pOperation);

   // Cleanup
   deleteAVLTreeOperation(pOperation);
   return iResult;
}


/// Function name  : insertLanguageDocumentGamePage
// Description     : Add a new GamePage to a Language document's LanguageFile and GamePage ListView
// 
// LANGUAGE_DOCUMENT*  pDocument  : [in] Langage document
// GAME_PAGE*          pGamePage  : [in] New GamePage to append. This is now owned by the document and must not be deleted
// 
// Return Value   : TRUE
// 
BOOL  insertLanguageDocumentGamePage(LANGUAGE_DOCUMENT*  pDocument, GAME_PAGE*  pGamePage)
{
   // [CHECK] Ensure document isn't virtual
   ASSERT(!pDocument->bVirtual);

   // Insert into LanguageFile/GameData GamePage tree and re-index it
   insertObjectIntoAVLTree(getLanguageDocumentGamePageTree(pDocument), (LPARAM)pGamePage);
   performAVLTreeIndexing(getLanguageDocumentGamePageTree(pDocument));

   // Update the GamePage ListView item count
   ///REM: ListView_SetItemCount(pDocument->hPageList, getLanguageDocumentGamePageTree(pDocument)->iCount);
   updateLanguageDocumentPageGroups(pDocument);

   return TRUE;
}


/// Function name  : insertLanguageDocumentGameString
// Description     : Insert a new GameString into the current page
// 
// LANGUAGE_DOCUMENT*  pDocument   : [in] Language document 
// GAME_STRING*        pGameString : [in] GameString to insert
// 
// Return Value   : TRUE
// 
BOOL  insertLanguageDocumentGameString(LANGUAGE_DOCUMENT*  pDocument, GAME_STRING*  pGameString)
{
   // [CHECK] Ensure document isn't virtual
   ASSERT(!pDocument->bVirtual);

   /// Insert into underlying GameString tree
   insertObjectIntoAVLTree(getLanguageDocumentGameStringTree(pDocument), (LPARAM)pGameString);

   // Re-create current page strings
   deleteAVLTree(pDocument->pPageStringsByID);
   pDocument->pPageStringsByID = generateLanguagePageStringsTree(pDocument, pDocument->pCurrentPage);

   /// Update the string ListView
   ListView_SetItemCount(pDocument->hStringList, getTreeNodeCount(pDocument->pPageStringsByID));

   // Return TRUE
   return TRUE;
}


/// Function name  : modifyLanguageDocumentGamePageID
// Description     : Changes the PageID of all strings within a specified page.  If successful, the input Page is destroyed
// 
// LANGUAGE_DOCUMENT*  pDocument : [in]     Document
// GAME_PAGE*          pOldPage  : [in/out] Current Page
// GAME_PAGE*          pNewPage  : [in]     New Page to store strings under
// 
VOID  modifyLanguageDocumentGamePageID(LANGUAGE_DOCUMENT*  pDocument, GAME_PAGE*  pOldPage, GAME_PAGE*  pNewPage)
{
   AVL_TREE_OPERATION*  pOperation   = createAVLTreeOperation(treeprocModifyGameStringPageID, ATT_INORDER);
   AVL_TREE*            pPageStrings = generateLanguagePageStringsTree(pDocument, pOldPage);

   // Prepare
   pOperation->xFirstInput = pNewPage->iPageID;
   pOperation->pOutputTree = getLanguageDocumentGameStringTree(pDocument);

   /// Modify PageID of all strings within target page
   if (performOperationOnAVLTree(pPageStrings, pOperation) == 0)
      // [ALL MOVED] Delete old Page
      destroyObjectInAVLTreeByValue(getLanguageDocumentGamePageTree(pDocument), pOldPage->iPageID, NULL);

   /// Insert new Page into List and Tree
   insertLanguageDocumentGamePage(pDocument, pNewPage);  // Updates PageList Count / Re-indexes PageList tree

   // Cleanup
   deleteAVLTree(pPageStrings);
   deleteAVLTreeOperation(pOperation);
}


/// Function name  : modifyLanguageDocumentGameStringID
// Description     : Changes the StringID of a string in the current page
// 
// LANGUAGE_DOCUMENT*  pDocument : [in] Document
// GAME_STRING*        pString   : [in] Target String
// const UINT          iNewID    : [in] New ID for string
// 
VOID  modifyLanguageDocumentGameStringID(LANGUAGE_DOCUMENT*  pDocument, GAME_STRING*  pString, const UINT  iNewID)
{
   // [CHECK] Ensure desired ID is available
   if (!validateLanguagePageStringID(pDocument, iNewID))
      return;

   /// Re-Insert string under new ID
   removeObjectFromAVLTreeByValue(getLanguageDocumentGameStringTree(pDocument), pString->iPageID, pString->iID, (LPARAM&)pString);
   pString->iID = iNewID;
   insertObjectIntoAVLTree(getLanguageDocumentGameStringTree(pDocument), (LPARAM)pString);

   // Re-create current page strings
   deleteAVLTree(pDocument->pPageStringsByID);
   pDocument->pPageStringsByID = generateLanguagePageStringsTree(pDocument, pDocument->pCurrentPage);

   /// Redraw current page
   ListView_RedrawItems(pDocument->hStringList, 0, getTreeNodeCount(pDocument->pPageStringsByID));
}


/// Function name  : updateLanguageDocumentPageGroups
// Description     : Refreshes the size of each group in the Grouped PageList
// 
// LANGUAGE_DOCUMENT*  pDocument   : [in] Document
// 
VOID  updateLanguageDocumentPageGroups(LANGUAGE_DOCUMENT*  pDocument)
{
   AVL_TREE_GROUP_COUNTER*  pGroupCounter = createAVLTreeGroupCounter(LANGUAGE_GROUP_COUNT);

   // Count each 
   performAVLTreeGroupCount(getLanguageDocumentGamePageTree(pDocument), pGroupCounter, AK_GROUP);

   /// Set new group sizes
   for (UINT iGroupID = 0; iGroupID < LANGUAGE_GROUP_COUNT; iGroupID++)
      GroupedListView_SetGroupCount(pDocument->hPageList, iGroupID, getAVLTreeGroupCount(pGroupCounter, iGroupID));

   // Cleanup
   deleteAVLTreeGroupCounter(pGroupCounter);
}


/// Function name  : treeprocDeleteGameStringPageID
// Description     : Destroys the equivilent GameString in the source tree
// 
// AVL_TREE_NODE*       pNode   : [in] GameString in copy tree
// AVL_TREE_OPERATION*  pData   : [in] pOutputTree : source tree
// 
VOID  treeprocDeleteGameStringPageID(AVL_TREE_NODE*  pNode, AVL_TREE_OPERATION*  pData)
{
   GAME_STRING*  pString = extractPointerFromTreeNode(pNode, GAME_STRING);
   
   /// Destroy string
   destroyObjectInAVLTreeByValue(pData->pOutputTree, pString->iPageID, pString->iID);
}


/// Function name  : treeprocFindFreeGameStringID
// Description     : Finds the next available StringID
// 
// AVL_TREE_NODE*       pNode   : [in] GameString in copy tree
// AVL_TREE_OPERATION*  pData   : [in] Not used
// 
VOID  treeprocFindFreeGameStringID(AVL_TREE_NODE*  pNode, AVL_TREE_OPERATION*  pData)
{
   GAME_STRING*  pString = extractPointerFromTreeNode(pNode, GAME_STRING);

   // [CHECK] Calculate next available ID
   if (pString->iID >= (UINT)pData->xOutput)
      pData->bResult = pString->iID + 1;
}

/// Function name  : treeprocGeneratePageStrings
// Description     : Performs a shallow copy of GameStrings with specific PageID 
// 
// AVL_TREE_NODE*       pNode          : [in/out] GameString node
// AVL_TREE_OPERATION*  pOperationData : [in]     xFirstInput -- Page ID to copy
//                                                pOutputTree -- Destination tree
//
VOID   treeprocGeneratePageStrings(AVL_TREE_NODE*  pNode, AVL_TREE_OPERATION*  pOperationData)
{
   GAME_STRING*   pGameString = extractPointerFromTreeNode(pNode, GAME_STRING);

   /// Compare PageID against the input PageID
   if (pGameString->iPageID == pOperationData->xFirstInput)
      // [SUCCESS] Insert into destination tree
      insertObjectIntoAVLTree(pOperationData->pOutputTree, (LPARAM&)pGameString);
}
/// Function name  : treeprocModifyGameStringPageID
// Description     : Re-Inserts a GameString under a new PageID
// 
// AVL_TREE_NODE*       pNode   : [in] GameString in copy tree
// AVL_TREE_OPERATION*  pData   : [in] xFirstInput : new PageID
//                                     pOutputTree : source tree
// 
VOID  treeprocModifyGameStringPageID(AVL_TREE_NODE*  pNode, AVL_TREE_OPERATION*  pData)
{
   GAME_STRING*  pString    = extractPointerFromTreeNode(pNode, GAME_STRING);
   UINT          iNewPageID = pData->xFirstInput;

   // Create copy with new PageID
   GAME_STRING*  pNewString = createGameString(pString->szText, pString->iID, iNewPageID, pString->eType);
   
   /// Attempt to insert copy
   if (insertObjectIntoAVLTree(pData->pOutputTree, (LPARAM)pNewString))
      // [SUCCESS] Remove original
      destroyObjectInAVLTreeByValue(pData->pOutputTree, pString->iPageID, pString->iID);
   else
   {
      // [FAILED] Delete copy
      deleteGameString(pNewString);
      pData->bResult++;
   }
}


/// Function name  : validateLanguageButtonID
// Description     : Validates a potential button ID
// 
// LANGUAGE_DOCUMENT*  pDocument : [in] Document
// CONST TCHAR*        szID      : [in] ID to test
// 
// Return Value   : TRUE if available, FALSE if taken/invalid
// 
BOOL  validateLanguageButtonID(LANGUAGE_DOCUMENT*  pDocument, CONST TCHAR*  szID)
{
   LANGUAGE_BUTTON*  pButton;

   // [TODO] Validate ID string
   if (!lstrlen(szID) OR utilFindCharacterInSet(szID, "`¬!\"£$%^&*()_[]@#~?<>;,/|\\"))
      return FALSE;

   // [CHECK] Ensure no button with ID exists
   return !findObjectInAVLTreeByValue(pDocument->pButtonsByID, (LPARAM)szID, NULL, (LPARAM&)pButton);
}


/// Function name  : validateLanguagePageStringID
// Description     : Determines whether a string ID for the current page is available
// 
// LANGUAGE_DOCUMENT*  pDocument : [in]  Language document data
// const UINT          iStringID : [in]  String ID
// 
// Return Value   : TRUE if available, FALSE if taken
// 
BOOL  validateLanguagePageStringID(LANGUAGE_DOCUMENT*  pDocument, const UINT  iStringID)
{
   GAME_STRING*  pConflict;

   // Query tree
   return !findObjectInAVLTreeByValue(getLanguageDocumentGameStringTree(pDocument), pDocument->pCurrentPage->iPageID, iStringID, (LPARAM&)pConflict);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         MESSAGE HANDLERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onLanguageDocument_ContextMenu
// Description     : Display the Language document popup menu
// 
// SCRIPT_DOCUMENT*  pDocument  : [in] ScriptDocument window data
// CONST POINT*      ptCursor   : [in] Cursor position in screen co-ordinates
// HWND              hCtrl      : [in] Control sending the message
// 
VOID   onLanguageDocument_ContextMenu(LANGUAGE_DOCUMENT*  pDocument, CONST POINT*  ptCursor, HWND  hCtrl)
{
   UINT            iMenuIDs[] = { IDM_GAMESTRING_INSERT, IDM_GAMESTRING_DELETE, IDM_GAMESTRING_EDIT, IDM_GAMESTRING_VIEW_ERROR };
   CUSTOM_MENU*    pCustomMenu;
   CONST TCHAR*    szItemText;
   LVHITTESTINFO&  oHitTest         = pDocument->oStringClick;
   BOOL            bStringSelection = ListView_GetSelected(pDocument->hStringList) != -1,
                   bPageSelection   = ListView_GetSelected(pDocument->hPageList) != -1,
                   bGameData        = pDocument->bVirtual;
   
   // Prepare
   TRACK_FUNCTION();
   VERBOSE_LIB_COMMAND();
   
   // Examine control 
   switch (GetWindowID(hCtrl)) 
   {
   /// [PAGE/STRING LIST]
   case IDC_PAGE_LIST:
   case IDC_STRING_LIST:
      /// Create CustomMenu
      pCustomMenu = createCustomMenu(TEXT("LANGUAGE_MENU"), TRUE, GetWindowID(hCtrl) == IDC_PAGE_LIST ? IDM_GAMEPAGE_POPUP : IDM_GAMESTRING_POPUP);

      // Examine source
      if (GetWindowID(hCtrl) == IDC_PAGE_LIST)
      {
         /// [EDIT/DELETE PAGE] Require selection
         EnableMenuItem(pCustomMenu->hPopup, IDM_GAMEPAGE_EDIT,   !bGameData AND bPageSelection ? MF_ENABLED : MF_DISABLED);
         EnableMenuItem(pCustomMenu->hPopup, IDM_GAMEPAGE_DELETE, !bGameData AND bPageSelection ? MF_ENABLED : MF_DISABLED);
         /// [INSERT PAGE] Ensure not game data
         EnableMenuItem(pCustomMenu->hPopup, IDM_GAMEPAGE_INSERT, !bGameData                    ? MF_ENABLED : MF_DISABLED);
      }
      else
      {
         // Perform sub-item HitTest
         oHitTest.pt = *ptCursor;
         ScreenToClient(pDocument->hStringList, &oHitTest.pt);
         ListView_SubItemHitTest(pDocument->hStringList, &oHitTest);

         /// Enable/Disable menu items
         for (UINT  i = 0, bEnabled = 0; i < 4; i++)
         {
            switch (iMenuIDs[i])
            {
            case IDM_GAMESTRING_VIEW_ERROR:  bEnabled = pDocument->bFormattingError;            break;   // Require formatting error
            case IDM_GAMESTRING_INSERT:      bEnabled = bPageSelection   AND !bGameData;        break;   // Require current page
            case IDM_GAMESTRING_DELETE:      bEnabled = bStringSelection AND !bGameData;        break;   // Require current string
            case IDM_GAMESTRING_EDIT:        bEnabled = bStringSelection AND (oHitTest.iSubItem == COLUMN_TEXT OR !bGameData);     break; // Prevent editing of game data string IDs
            }
            EnableMenuItem(pCustomMenu->hPopup, iMenuIDs[i], bEnabled ? MF_ENABLED : MF_DISABLED);  
         }

         /// [EDIT] Display 'Edit ID/Edit Formatting/View Formatting'
         szItemText = (oHitTest.iSubItem == COLUMN_ID ? TEXT("Edit ID") : (bGameData ? TEXT("View Formatting") : TEXT("Edit Formatting")));
         setCustomMenuItemText(pCustomMenu->hPopup, IDM_GAMESTRING_EDIT, FALSE, szItemText);

         /// [PROPERTIES] Check/Uncheck appropriately
         CheckMenuItem(pCustomMenu->hPopup, IDM_GAMESTRING_PROPERTIES, (getMainWindowData()->hPropertiesSheet ? MF_CHECKED : MF_UNCHECKED));
      }
      
      // Display the pop-up menu
      TrackPopupMenu(pCustomMenu->hPopup, TPM_LEFTALIGN WITH TPM_TOPALIGN WITH TPM_LEFTBUTTON, ptCursor->x, ptCursor->y, NULL, pDocument->hWnd, NULL);
      deleteCustomMenu(pCustomMenu);
   }

   // [TRACK]
   END_TRACKING();
}


/// Function name  : onLanguageDocument_Command
// Description     : Language Document menu item processing
//
// LANGUAGE_DOCUMENT*  pDocument     : [in] Language document
// CONST UINT          iControlID    : [in] Source control
// CONST UINT          iNotification : [in] Notifcation code
// HWND                hControl      : [in] Control window handle
// 
// Return type : TRUE if processed, FALSE otherwise
//
BOOL   onLanguageDocument_Command(LANGUAGE_DOCUMENT*  pDocument, CONST UINT  iControlID, CONST UINT  iNotification, HWND  hControl)
{
   GAME_PAGE*  pNewPage;
   BOOL        bResult;

   // Prepare
   bResult = TRUE;

   // Examine source
   switch (iControlID)
   {
   // [PAGE CHANGED]
   case IDM_GAMEPAGE_EDIT:
   case IDM_GAMEPAGE_INSERT:
   case IDM_GAMEPAGE_DELETE:
      switch (iControlID)
      {
      /// [EDIT PAGE] Edit selected Page
      case IDM_GAMEPAGE_EDIT:
         if (pNewPage = displayInsertPageDialog(pDocument, pDocument->pCurrentPage, getAppWindow()))
         {
            onLanguageDocument_EditPage(pDocument, pDocument->pCurrentPage, pNewPage);
            break;
         }
         // [ABORTED] Do not modify document
         return TRUE;

      /// [INSERT PAGE] Create/Insert new Page
      case IDM_GAMEPAGE_INSERT:
         if (pNewPage = displayInsertPageDialog(pDocument, NULL, getAppWindow()))
         {
            onLanguageDocument_InsertPage(pDocument, pNewPage);
            break;
         }
         // [ABORTED] Do not modify document
         return TRUE;

      /// [DELETE PAGE]
      case IDM_GAMEPAGE_DELETE:
         onLanguageDocument_DeletePage(pDocument, pDocument->pCurrentPage);
         break;
      }

      // [DOCUMENT CHANGED]
      sendDocumentUpdated(AW_DOCUMENTS_CTRL);
      break;

   // [STRING]
   case IDM_GAMESTRING_INSERT:
   case IDM_GAMESTRING_EDIT:
   case IDM_GAMESTRING_DELETE:
      switch (iControlID)
      {
      /// [INSERT STRING]
      case IDM_GAMESTRING_INSERT:
         onLanguageDocument_InsertString(pDocument, pDocument->pCurrentPage->iPageID);
         break;

      /// [EDIT STRING]
      case IDM_GAMESTRING_EDIT:
         onLanguageDocument_EditString(pDocument, pDocument->pCurrentString);
         return TRUE;      // Document modified only if edit is committed

      /// [DELETE STRING]
      case IDM_GAMESTRING_DELETE:
         onLanguageDocument_DeleteString(pDocument, pDocument->pCurrentString);
         break;
      }

      // [DOCUMENT CHANGED]
      sendDocumentUpdated(AW_DOCUMENTS_CTRL);
      break;

   /// [FORMATTING ERROR]
   case IDM_GAMESTRING_VIEW_ERROR:
      onLanguageDocument_ViewFormattingError(pDocument);
      break;

   /// [PROPERTIES] -- Invoke the document properties dialog
   case IDM_GAMESTRING_PROPERTIES:
      sendAppMessage(AW_MAIN, WM_COMMAND, IDM_VIEW_DOCUMENT_PROPERTIES, NULL);
      break;

   default:
      return FALSE;
   }

   // Return result
   return bResult;
}


/// Function name  : onLanguageDocument_Create
// Description     : Initialises the LanguageDocument controls and the LanguageDocument convenience pointers.
// 
// LANGUAGE_DOCUMENT*  pDocument : [in] LanguageDocument data
// HWND                hWnd      : [in] Document window handle
// 
VOID   onLanguageDocument_Create(LANGUAGE_DOCUMENT*  pDocument, HWND  hWnd)
{  
   // [TRACK]
   TRACK_FUNCTION();

   /// Associate data with dialog
   SetWindowLong(hWnd, 0, (LONG)pDocument);
   pDocument->hWnd = hWnd;

   // Create document convenience pointer
   pDocument->pLanguageFile = (LANGUAGE_FILE*)pDocument->pGameFile;
   pDocument->pFormatErrors = createErrorQueue();

   /// Create child windows
   if (createLanguageDialogControls(pDocument))
   {
      // Ensure LanguageFile's GamePages are indexed
      performAVLTreeIndexing(getLanguageDocumentGamePageTree(pDocument));

      /// Display Pages
      updateLanguageDocumentPageGroups(pDocument);
      ListView_SetItemState(pDocument->hPageList, 0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);    // Causes strings to be generated + displayed

      /// [DEBUG] Test RichText algorithms
      //testRichTextConversion(pDocument->hWnd, getLanguageDocumentGameStringTree(pDocument));

      // Show child windows
      ShowWindow(pDocument->hPageList, SW_SHOWNORMAL);
      ShowWindow(pDocument->hStringList, SW_SHOWNORMAL);
      ShowWindow(pDocument->hRichEdit, SW_SHOWNORMAL);
   }

   // [TRACK]
   END_TRACKING();
}


/// Function name  : onLanguageDocument_Destroy
// Description     : Cleanup the language document's data
// 
// LANGUAGE_DOCUMENT*  pDocument : [in] Language document data
// 
VOID   onLanguageDocument_Destroy(LANGUAGE_DOCUMENT*  pDocument)
{
   // [TRACK]
   TRACK_FUNCTION();

   /// [GAMESTRINGS] Delete GameStrings tree
   if (pDocument->pPageStringsByID)
      deleteAVLTree(pDocument->pPageStringsByID);

   /// [MESSAGE] Delete current message, if any
   if (pDocument->pCurrentMessage)
      deleteLanguageMessage(pDocument->pCurrentMessage);
   
   /// [CONTROLS] Destroy workspace
   utilDeleteWindow(pDocument->hWorkspace);

   // ErrorQueue
   deleteErrorQueue(pDocument->pFormatErrors);
   
   // [TRACK]
   END_TRACKING();
}


/// Function name  : onLanguageDocument_Notify
// Description     : Language document dialog WM_NOTIFY processing
// 
// LANGUAGE_DOCUMENT*  pDocument  : [in] Language document window data
// NMHDR*              pMessage   : [in] WM_NOTIFY message data
// 
// Return Value   : TRUE if message processed, FALSE if not
// 
BOOL   onLanguageDocument_Notify(LANGUAGE_DOCUMENT*  pDocument, NMHDR*  pMessage)
{
   NMLISTVIEW*   pListItem;
   BOOL          bResult = TRUE;

   // Examine source
   TRACK_FUNCTION();
   switch (pMessage->idFrom)
   {
   case IDC_STRING_LIST:
   case IDC_PAGE_LIST:
      // Examine notification
      switch (pMessage->code)
      {
      /// [REQUEST DATA]
      case LVN_GETDISPINFO:
         onLanguageDocument_RequestData(pDocument, pMessage->idFrom, (NMLVDISPINFO*)pMessage);
         break;

      /// [ITEM CHANGED]
      case LVN_ITEMCHANGED:
         pListItem = (NMLISTVIEW*)pMessage;

         // [SELECTION CHANGED]
         if ((pListItem->uNewState & LVIS_SELECTED) OR (pListItem->uOldState & LVIS_SELECTED))
         {
            if (pMessage->idFrom == IDC_PAGE_LIST)
               onLanguageDocument_PageSelectionChanged(pDocument, pListItem->iItem, pListItem->uNewState & LVIS_SELECTED);
            else
               onLanguageDocument_StringSelectionChanged(pDocument, pListItem->iItem, pListItem->uNewState & LVIS_SELECTED);
         }
         break;

      /// [EDIT STRING ID]
      case LVN_BEGINLABELEDIT:
         onLanguageDocument_EditStringBegin(pDocument, (NMLVDISPINFO*)pMessage);
         break;

      /// [EDIT STRING ID]
      case LVN_ENDLABELEDIT:
         bResult = onLanguageDocument_EditStringEnd(pDocument, (NMLVDISPINFO*)pMessage);
         break;

      /// [CUSTOM DRAW] Pass to CustomListView / GroupedListView handler
      case NM_CUSTOMDRAW:
         bResult = (pMessage->idFrom == IDC_STRING_LIST ? onCustomDrawListView(pDocument->hWnd, pMessage->hwndFrom, (NMLVCUSTOMDRAW*)pMessage) 
                                                        : onCustomDraw_GroupedListView(pDocument->hWnd, pMessage->hwndFrom, (NMLVCUSTOMDRAW*)pMessage));
         break;

      // [UNHANDLED]
      default:
         bResult = FALSE;
         break;
      }
      break;

   // [UNHANDLED]
   default:
      bResult = FALSE;
      break;
   }
   
   // Return result
   END_TRACKING();
   return bResult;
}

BOOL  generateLanguageDisplayString(const GAME_STRING*  pSource, LANGUAGE_MESSAGE*&  pOutput)
{
   SUBSTRING*   pSubString = createSubString(pSource->szText);
   TCHAR*       szDisplay  = utilCreateEmptyString(1024);
   BOOL         bResult;

   /// Generate 'Display' source with comments removed
   while (findNextSubStringSimple(pSource, pSubString))
   {
      // Examine type
      switch (pSubString->eType)
      {
      /// [MISSION, TEXT] Append to output
      case SST_MISSION:
      case SST_LOOKUP:
      case SST_TEXT:
         StringCchCat(szDisplay, 1024, pSubString->szText);  
         break;

      /// [COMMENTS] Ignore
      case SST_COMMENT:
         // [SPECIAL CASE] Allow 'opening bracket' operator
         if (utilCompareString(pSubString->szText, "("))
            StringCchCat(szDisplay, 1024, pSubString->szText);  
         break;
      }
   }

   /// Generate RichText 'Display' source
   //generateConvertedString(szDisplay, SCF_CONDENSE_BRACKET|SCF_CONDENSE_CURLY_BRACKET, szConverted);
   bResult = generateRichTextFromSourceText(szDisplay, lstrlen(szDisplay), (RICH_TEXT*&)pOutput, RTT_LANGUAGE_MESSAGE, ST_DISPLAY, NULL);

   // Cleanup and return RichText
   deleteSubString(pSubString);
   utilDeleteString(szDisplay);
   //utilSafeDeleteString(szConverted);
   return bResult;
}

/// Function name  : onLanguageDocument_RequestData
// Description     : Supply item data for the GamePages ListView
//
// LANGUAGE_DOCUMENT*  pDocument  : [in]     Language document data
// CONST UINT          iControlID : [in]     ID of the control requesting data (Always IDC_PAGE_LIST)
// NMLVDISPINFO*       pHeader    : [in/out] Item to display / Item data
// 
// Return type : TRUE  
//
BOOL  onLanguageDocument_RequestData(LANGUAGE_DOCUMENT*  pDocument, CONST UINT  iControlID, NMLVDISPINFO*  pHeader)
{
   LANGUAGE_MESSAGE* pMessage;
   GAME_STRING*      pGameString = NULL;
   GAME_PAGE*        pGamePage   = NULL;
   LVITEM&           oOutput     = pHeader->item;

   // Prepare
   TRACK_FUNCTION();

   // Examine
   switch (iControlID)
   {
   /// [GAME PAGE]
   case IDC_PAGE_LIST:
      // [CHECK] Lookup item
      if (findLanguageDocumentGamePageByIndex(pDocument, oOutput.iItem, pGamePage))
      {
         // [IMAGE] Provide Voiced/NotVoiced icon
         if (oOutput.mask INCLUDES LVIF_IMAGE)
            oOutput.iImage = getAppImageTreeIconIndex(ITS_SMALL, pGamePage->bVoice ? TEXT("VOICED_YES_ICON") : TEXT("VOICED_NO_ICON"));

         // [TEXT] Provide appropriate text
         if (oOutput.mask INCLUDES LVIF_TEXT) 
         {  /// [PAGE ID] -- Supply the PageID and determine the icon from whether the page is voiced or not
            if (oOutput.iSubItem == COLUMN_ID)
               StringCchPrintf(oOutput.pszText, oOutput.cchTextMax, TEXT("%u"), pGamePage->iPageID);
            else
            {  /// [TITLE/DESCRIPTION] Supply text or use placeholder
               CONST TCHAR*  szText = (oOutput.iSubItem == COLUMN_TITLE ? pGamePage->szTitle : pGamePage->szDescription);

               // [CHECK] Supply text
               if (lstrlen(szText))
                  StringCchCopyConvert(oOutput.pszText, oOutput.cchTextMax, SPC_LANGUAGE_INTERNAL_TO_DISPLAY, szText);
               else
               {  // [FAILED] Display placeholder in grey
                  StringCchCopy(oOutput.pszText, oOutput.cchTextMax, TEXT("[None]"));
                  oOutput.lParam  = (LPARAM)GetSysColor(COLOR_GRAYTEXT);
                  oOutput.mask   |= LVIF_COLOUR_TEXT;
               }
            }
         }
      }
      break;

   /// [GAME STRING]
   case IDC_STRING_LIST:
      // [CHECK] Lookup item
      if (findLanguageDocumentGameStringByIndex(pDocument, oOutput.iItem, pGameString))
      {
         // [IMAGE] Provide string's GameVersion icon
         if (oOutput.mask INCLUDES LVIF_IMAGE)
            oOutput.iImage = getAppImageTreeIconIndex(ITS_SMALL, identifyGameVersionIconID(pGameString->eVersion));

         // [TEXT] Provide column text
         if (oOutput.mask INCLUDES LVIF_TEXT AND (oOutput.lParam == LVIP_CUSTOM_DRAW))    // [FIX] Ignore LVN_GETDISPINFO that does not originate from CustomDraw handler (Received from unknown window once per click)
         {
            // Attempt to generate RichText from source 
            if (!generateLanguageDisplayString(pGameString, pMessage))
            {  // [ERROR] Display text in red
               oOutput.lParam  = RGB(255,0,0); 
               oOutput.mask   |= LVIF_COLOUR_TEXT;
            }
            
            /// [STRING ID] 
            if (oOutput.iSubItem == COLUMN_ID)
               StringCchPrintf(oOutput.pszText, oOutput.cchTextMax, TEXT("%u"), pGameString->iID);
            
            /// [VALID TEXT] Supply and pass ownership to ListView
            else if (utilExcludes(oOutput.mask, LVIF_COLOUR_TEXT))
            {  
               oOutput.lParam = (LPARAM)pMessage;
               oOutput.mask  |= LVIF_RICHTEXT | LVIF_DESTROYTEXT;
            }
            else
               /// [INVALID TEXT] Output verbatim
               StringCchCopy(oOutput.pszText, oOutput.cchTextMax, pGameString->szText);

            // Cleanup
            if (utilExcludes(oOutput.mask, LVIF_DESTROYTEXT))
               deleteLanguageMessage(pMessage);
         }
      }
      break;
   }

   /// [ERROR] Provide error string
   if (!pGameString AND !pGamePage)
   {
      if (oOutput.mask INCLUDES LVIF_TEXT)
         StringCchPrintf(oOutput.pszText, oOutput.cchTextMax, TEXT("ERROR: Item %d not found"), oOutput.iItem);
      else
         oOutput.pszText = TEXT("ERROR: Item not found");
   }

   END_TRACKING();
   return TRUE;
}



/// Function name  : onLanguageDocument_Resize
// Description     : Resizes a language document and repositions it's child windows
// 
// LANGUAGE_DOCUMENT* pDocument : [in] Language document
// CONST SIZE*        pNewSize  : [in] New window size
// 
VOID   onLanguageDocument_Resize(LANGUAGE_DOCUMENT*  pDocument, CONST SIZE*  pNewSize)
{
   // [TRACK]
   TRACK_FUNCTION();

   // Resize workspace
   SetWindowPos(pDocument->hWorkspace, NULL, NULL, NULL, pNewSize->cx, pNewSize->cy, SWP_NOMOVE WITH SWP_NOZORDER);

   // Stretch StringList 'Text' column over entire width
   ListView_SetColumnWidth(pDocument->hStringList, COLUMN_TEXT, LVSCW_AUTOSIZE_USEHEADER);

   // [TRACK]
   END_TRACKING();
}


/// Function name  : onScriptDocumentSaveComplete
// Description     : Marks the document as 'un-modified' if successful
// 
// LANGUAGE_DOCUMENT*   pDocument      : [in] Language document
// DOCUMENT_OPERATION*  pOperationData : [in] Generation Operation Data
// 
VOID   onLanguageDocument_SaveComplete(LANGUAGE_DOCUMENT*  pDocument, DOCUMENT_OPERATION*  pOperationData)
{
   // [TRACK]
   TRACK_FUNCTION();

   // [SUCCESSFUL] Document saved successfully
   if (isOperationSuccessful(pOperationData))
   {
      /// [UN-MODIFIED] Remove 'modified' flag
      setDocumentModifiedFlag(pDocument, FALSE);
   }
   
   // [TRACK]
   END_TRACKING();
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         WINDOW PROCEDURE
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : dlgprocLanguageDocument
// Description     : Language document dialog procedure
//
//
LRESULT   wndprocLanguageDocument(HWND  hWnd, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   DOCUMENT_OPERATION* pOperationData;
   LANGUAGE_DOCUMENT*  pDocument;
   CREATESTRUCT*       pCreationData;
   ERROR_STACK*        pException;
   LRESULT             iResult;
   POINT               ptCursor;
   SIZE                siWindow;

   __try
   {
      // Get document data
      TRACK_FUNCTION();
      pDocument = (LANGUAGE_DOCUMENT*)GetWindowLong(hWnd, 0);
      iResult   = 0;

      switch (iMessage)
      {
      /// [CREATION]
      case WM_CREATE:
         // Prepare
         pCreationData = (CREATESTRUCT*)lParam;
         pDocument     = (LANGUAGE_DOCUMENT*)pCreationData->lpCreateParams;

         // Init dialog
         onLanguageDocument_Create(pDocument, hWnd);
         break;

      /// [DESTRUCTION] 
      case WM_DESTROY:
         onLanguageDocument_Destroy(pDocument);
         break;

      /// [COMMAND PROCESSING]
      case WM_COMMAND:
         if (!onLanguageDocument_Command(pDocument, LOWORD(wParam), HIWORD(wParam), (HWND)lParam))
            // [UNHANDLED] Call default window proc
            iResult = DefWindowProc(hWnd, iMessage, wParam, lParam);
         break;

      /// [NOTIFICATION]
      case WM_NOTIFY:
         if ((iResult = onLanguageDocument_Notify(pDocument, (NMHDR*)lParam)) == FALSE)
            // [UNHANDLED] Call default window proc
            iResult = DefWindowProc(hWnd, iMessage, wParam, lParam);
         break;

      /// [RESIZE] - Resize tab to the size of the tab control
      case WM_SIZE:
         // Prepare
         siWindow.cx = LOWORD(lParam);
         siWindow.cy = HIWORD(lParam);
         // Resize
         onLanguageDocument_Resize(pDocument, &siWindow);
         break;

      /// [CONTEXT MENU]
      case WM_CONTEXTMENU:
         ptCursor.x = GET_X_LPARAM(lParam);
         ptCursor.y = GET_Y_LPARAM(lParam);
         onLanguageDocument_ContextMenu(pDocument, &ptCursor, (HWND)wParam);
         break;

      /// [MENU ITEM HOVER] Forward messages from CodeEdit up the chain to the Main window
      case WM_MENUSELECT:
         sendAppMessage(AW_MAIN, WM_MENUSELECT, wParam, lParam);
         break;

      /// [GET MENU CMD STATE]
      case UM_GET_MENU_ITEM_STATE:
         iResult = onLanguageDocument_GetMenuItemState(pDocument, wParam, (UINT*)lParam);
         break;

      case WM_SETFOCUS:
         SetFocus(pDocument->hPageList);
         break;

      /// [CUSTOM MENU/CUSTOM COMBO]
      case WM_DRAWITEM:    onWindow_DrawItem((DRAWITEMSTRUCT*)lParam);                  break;
      case WM_MEASUREITEM: onWindow_MeasureItem(hWnd, (MEASUREITEMSTRUCT*)lParam);      break;
      case WM_DELETEITEM:  onWindow_DeleteItem((DELETEITEMSTRUCT*)lParam);              break;

      /// [THEME CHANGED]
      case WM_THEMECHANGED:  
         ListView_SetBkColor(pDocument->hPageList, getThemeSysColour(TEXT("TAB"), COLOR_WINDOW));
         ListView_SetBkColor(pDocument->hStringList, getThemeSysColour(TEXT("TAB"), COLOR_WINDOW));
         setWorkspaceBackgroundColour(pDocument->hWorkspace, getTabThemeColour());  
         break;

      /// [DOCUMENT PROPERTY CHANGED]
      case UN_PROPERTY_UPDATED:
         onLanguageDocument_PropertyChanged(pDocument, wParam);
         break;

      /// [DOCUMENT SAVED]
      case UN_OPERATION_COMPLETE:
         pOperationData = (DOCUMENT_OPERATION*)lParam;

         // [SAVE]
         if (pOperationData->eType == OT_SAVE_LANGUAGE_FILE)
            onLanguageDocument_SaveComplete(pDocument, pOperationData);
         break;

      // [UNHANDLED]
      default:
         iResult = DefWindowProc(hWnd, iMessage, wParam, lParam);
         break;
      }

      // [FOCUS HANDLER]
      updateMainWindowToolBar(iMessage, wParam, lParam);
   }
   /// [EXCEPTION HANDLER]
   __except (generateExceptionError(GetExceptionInformation(), pException))
   {
      // [ERROR] "An unidentified and unexpected critical error has occurred in the document object of language file '%s'"
      enhanceError(pException, ERROR_ID(IDS_EXCEPTION_LANGUAGE_DOCUMENT), (pDocument ? getDocumentFileName(pDocument) : TEXT("Unknown")));
      displayException(pException);
   }
   
   // Return result
   END_TRACKING();
   return iResult;
}

