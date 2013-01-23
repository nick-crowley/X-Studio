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

#define       PAGE_COLUMN_ID           0      // Page ID column 
#define       PAGE_COLUMN_TITLE        1      // Title column 
#define       PAGE_COLUMN_DESCRIPTION  2      // Description column

#define       STRING_COLUMN_ID         0      // String ID column
#define       STRING_COLUMN_TEXT       1      // Text column

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
   DWORD             dwListStyle = WS_BORDER WITH WS_CHILD WITH WS_TABSTOP WITH (LVS_OWNERDATA WITH LVS_SHOWSELALWAYS WITH LVS_REPORT WITH LVS_SINGLESEL WITH LVS_NOSORTHEADER);
   LISTVIEW_COLUMNS  oPageListView   = { 3, IDS_LANGUAGE_PAGE_COLUMN_ID,   60, 160, 100,  NULL, NULL };
   LISTVIEW_COLUMNS  oStringListView = { 2, IDS_LANGUAGE_STRING_COLUMN_ID, 70, 600, NULL, NULL, NULL };
   PANE_PROPERTIES   oPaneData;       // New pane properties
   PANE*             pTargetPane;     // Workspace pane being targetted for a split
   RECT              rcClient;        // Dialog client rectangle
   SIZE              siClient;        // Size of dialog rectangle
                     
   // Prepare
   TRACK_FUNCTION();
   GetClientRect(pDocument->hWnd, &rcClient);
   utilConvertRectangleToSize(&rcClient, &siClient);

   /// [PAGE LIST] Create ListView
   pDocument->hPageList = CreateWindow(WC_LISTVIEW, TEXT("GamePage List"), dwListStyle, 0, 0, siClient.cx, siClient.cy, pDocument->hWnd, (HMENU)IDC_PAGE_LIST, getAppInstance(), NULL);
   ERROR_CHECK("Creating GamePage listview", pDocument->hPageList);
   initReportModeListView(pDocument->hPageList, &oPageListView, TRUE);

   /// [STRING LIST] Create ListView
   pDocument->hStringList = CreateWindow(WC_LISTVIEW, TEXT("GameString List"), dwListStyle, 0, 0, siClient.cx, siClient.cy, pDocument->hWnd, (HMENU)IDC_STRING_LIST, getAppInstance(), NULL);
   ERROR_CHECK("Creating GameString listview", pDocument->hStringList);
   initReportModeListView(pDocument->hStringList, &oStringListView, TRUE);

   ///// [RICH EDIT] Create RichEdit
   /*pDocument->hRichEdit = CreateWindow(RICHEDIT_CLASS, TEXT("Richedit"), WS_BORDER | WS_CHILD | WS_TABSTOP, 0, 0, siClient.cx, siClient.cy, pDocument->hWnd, (HMENU)IDC_EDIT_DIALOG, getAppInstance(), NULL);
   ERROR_CHECK("Creating RichEdit", pDocument->hRichEdit);
   SetWindowFont(pDocument->hRichEdit, GetStockFont(ANSI_VAR_FONT), FALSE);*/

   /// [RICH EDIT] Create RichEdit Dialog
   pDocument->hRichTextDialog = CreateDialogParam(getResourceInstance(), TEXT("RICHTEXT_DIALOG"), pDocument->hWnd, dlgprocRichTextDialog, (LPARAM)pDocument);
   ERROR_CHECK("Creating RichEdit Dialog", pDocument->hRichTextDialog);

   /// [WORKSPACE]
   pDocument->hWorkspace = createWorkspace(pDocument->hWnd, &rcClient, pDocument->hPageList, IsThemeActive() ? COLOR_WINDOW : COLOR_BTNFACE);

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
// Description     : Find the GameString object for an item in the currently visible page of game strings
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
// Description     : Find the GamePage object for an item a language document's list of game string pages
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
// Description     : Find the GamePage object for an item a language document's list of game string pages
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
   AVL_TREE_OPERATION*  pOperation   = createAVLTreeOperation(treeprocDeleteGameStringPageID, ATT_PREORDER);
   AVL_TREE*            pPageStrings = generateLanguagePageStringsTree(pDocument, pPage);

   /// Delete all strings with input PageID
   pOperation->pOutputTree = getLanguageDocumentGameStringTree(pDocument);
   performOperationOnAVLTree(pPageStrings, pOperation);

   /// Delete GamePage
   destroyObjectInAVLTreeByValue(getLanguageDocumentGamePageTree(pDocument), pPage->iPageID, NULL);

   // Re-Index affected trees
   performAVLTreeIndexing(getLanguageDocumentGamePageTree(pDocument));
   performAVLTreeIndexing(getLanguageDocumentGameStringTree(pDocument));

   // Update PageList Count
   ListView_SetItemCount(pDocument->hPageList, getTreeNodeCount(getLanguageDocumentGamePageTree(pDocument)));

   // Cleanup
   deleteAVLTree(pPageStrings);
   deleteAVLTreeOperation(pOperation);
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
   AVL_TREE*  pOutputTree;

   // Create shallow copy
   pOutputTree = createAVLTree(extractGameStringTreeNode, NULL, createAVLTreeSortKey(AK_ID, AO_ASCENDING), NULL, NULL);

   /// Replicate strings with correct PageID 
   performBinaryTreeStringPageReplication(getLanguageDocumentGameStringTree(pDocument), pGamePage->iPageID, pOutputTree);
      
   // Index and return tree
   performAVLTreeIndexing(pOutputTree);
   return pOutputTree;
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
   // Insert into LanguageFile/GameData GamePage tree and re-index it
   insertObjectIntoAVLTree(getLanguageDocumentGamePageTree(pDocument), (LPARAM)pGamePage);
   performAVLTreeIndexing(getLanguageDocumentGamePageTree(pDocument));

   // Update the GamePage ListView item count
   ListView_SetItemCount(pDocument->hPageList, getLanguageDocumentGamePageTree(pDocument)->iCount);

   return TRUE;
}


/// Function name  : insertLanguageDocumentGameString
// Description     : Insert a new GameString into the GameString tree using the page ID of the current GamePage.
// 
// LANGUAGE_DOCUMENT*  pDocument   : [in] Language document data
// GAME_STRING*        pGameString : [in] GameString to insert
// 
// Return Value   : TRUE
// 
BOOL  insertLanguageDocumentGameString(LANGUAGE_DOCUMENT*  pDocument, GAME_STRING*  pGameString)
{
   AVL_TREE*  pGameStringTree;

   // Prepare
   pGameStringTree = getLanguageDocumentGameStringTree(pDocument);

   // Insert into LanguageFile/GameData GameString tree and re-index it
   insertObjectIntoAVLTree(pGameStringTree, (LPARAM)pGameString);
   performAVLTreeIndexing(pGameStringTree);

   // Update the string List count
   ListView_SetItemCount(pDocument->hStringList, getTreeNodeCount(pGameStringTree));

   // Return TRUE
   return TRUE;
}


/// Function name  : moveLanguageDocumentPageStrings
// Description     : Changes the PageID of all strings within a specified page.  If successful, the input Page is destroyed
// 
// LANGUAGE_DOCUMENT*  pDocument : [in]     Document
// GAME_PAGE*          pOldPage  : [in/out] Current Page
// GAME_PAGE*          pNewPage  : [in]     New Page to store strings under
// 
VOID  moveLanguageDocumentPageStrings(LANGUAGE_DOCUMENT*  pDocument, GAME_PAGE*  pOldPage, GAME_PAGE*  pNewPage)
{
   AVL_TREE_OPERATION*  pOperation   = createAVLTreeOperation(treeprocModifyGameStringPageID, ATT_PREORDER);
   AVL_TREE*            pPageStrings = generateLanguagePageStringsTree(pDocument, pOldPage);

   // Prepare
   pOperation->xFirstInput = pNewPage->iPageID;
   pOperation->pOutputTree = getLanguageDocumentGameStringTree(pDocument);

   /// Move those strings with unique IDs into the new Page
   if (performOperationOnAVLTree(pPageStrings, pOperation) == 0)
      // [ALL MOVED] Delete old Page
      destroyObjectInAVLTreeByValue(getLanguageDocumentGamePageTree(pDocument), pOldPage->iPageID, NULL);

   // Re-index strings tree
   performAVLTreeIndexing(getLanguageDocumentGameStringTree(pDocument));
   
   /// Insert new Page 
   insertLanguageDocumentGamePage(pDocument, pNewPage);  // Updates PageList Count / Re-indexes PageList tree

   // Cleanup
   deleteAVLTree(pPageStrings);
   deleteAVLTreeOperation(pOperation);
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
   CUSTOM_MENU*    pCustomMenu;       // Popup CustomMenu
   GAME_PAGE*      pGamePage;
   UINT            iSelection,
                   iControlID,
                   iVoicedIcon;

   // Prepare
   TRACK_FUNCTION();
   VERBOSE_LIB_COMMAND();
   
   // Examine control 
   switch (iControlID = GetWindowID(hCtrl)) 
   {
   /// [PAGE/STRING LIST]
   case IDC_PAGE_LIST:
   case IDC_STRING_LIST:
      // Prepare
      iSelection = ListView_GetNextItem(hCtrl, -1, LVNI_SELECTED);

      /// Create CustomMenu
      pCustomMenu = createCustomMenu(TEXT("LANGUAGE_MENU"), TRUE, iControlID == IDC_PAGE_LIST ? IDM_GAMEPAGE_POPUP : IDM_GAMESTRING_POPUP);

      // Examine source
      if (iControlID == IDC_PAGE_LIST)
      {
         /// [EDIT/DELETE PAGE] Require selection
         EnableMenuItem(pCustomMenu->hPopup, IDM_GAMEPAGE_EDIT,   iSelection != -1 ? MF_ENABLED : MF_DISABLED);
         EnableMenuItem(pCustomMenu->hPopup, IDM_GAMEPAGE_DELETE, iSelection != -1 ? MF_ENABLED : MF_DISABLED);

         /// [PAGE VOICED] Set Icon + Check
         if (iSelection != -1 AND findLanguageDocumentGamePageByIndex(pDocument, iSelection, pGamePage))
         {
            // Lookup and set Icon
            iVoicedIcon = getAppImageTreeIconIndex(ITS_MEDIUM, pGamePage->bVoice ? TEXT("VOICED_YES_ICON") : TEXT("VOICED_NO_ICON"));
            getCustomMenuItemData(pCustomMenu->hPopup, IDM_GAMEPAGE_VOICED, FALSE)->iIconIndex = iVoicedIcon;
            // Check item
            CheckMenuItem(pCustomMenu->hPopup, IDM_GAMEPAGE_VOICED, pGamePage->bVoice ? MF_CHECKED : MF_UNCHECKED);
         }
      }
      else
      {
         /// [EDIT/DELETE STRING] Require selection
         EnableMenuItem(pCustomMenu->hPopup, IDM_GAMESTRING_EDIT,   iSelection != -1 ? MF_ENABLED : MF_DISABLED);
         EnableMenuItem(pCustomMenu->hPopup, IDM_GAMESTRING_DELETE, iSelection != -1 ? MF_ENABLED : MF_DISABLED);

         // [VIEW ERROR] TODO

         /// [PROPERTIES] Check based on whether the window is open or not
         CheckMenuItem(pCustomMenu->hPopup, IDM_GAMESTRING_PROPERTIES, (getMainWindowData()->hPropertiesSheet ? MF_CHECKED : MF_UNCHECKED));
      }
      
      // Display the pop-up menu
      TrackPopupMenu(pCustomMenu->hPopup, TPM_LEFTALIGN WITH TPM_TOPALIGN WITH TPM_LEFTBUTTON, ptCursor->x, ptCursor->y, NULL, pDocument->hWnd, NULL);
      
      // Cleanup
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
   BOOL  bResult;

   // Prepare
   bResult = TRUE;

   // Examine source
   switch (iControlID)
   {
   /// [EDIT PAGE] Edit selected Page
   case IDM_GAMEPAGE_EDIT:
      if (pNewPage = displayInsertPageDialog(pDocument, pDocument->pCurrentPage, getAppWindow()))
      {
         onLanguageDocument_EditPage(pDocument, pDocument->pCurrentPage, pNewPage);
         sendDocumentPropertyUpdated(AW_DOCUMENTS_CTRL, IDC_PAGE_LIST);
      }
      break;

   /// [INSERT PAGE] Create/Insert new Page
   case IDM_GAMEPAGE_INSERT:
      if (pNewPage = displayInsertPageDialog(pDocument, NULL, getAppWindow()))
      {
         onLanguageDocument_InsertPage(pDocument, pNewPage);
         sendDocumentPropertyUpdated(AW_DOCUMENTS_CTRL, IDC_PAGE_LIST);
      }
      break;

   /// [DELETE PAGE]
   case IDM_GAMEPAGE_DELETE:
      onLanguageDocument_DeletePage(pDocument, pDocument->pCurrentPage);
      sendDocumentPropertyUpdated(AW_DOCUMENTS_CTRL, IDC_PAGE_LIST);
      break;

   //// [GAME PAGE VOICED] -- Toggle the 'voiced flag'
   //case IDM_GAMEPAGE_VOICED:
   //   onLanguageDocument_GamePageVoiceToggle(pDocument);
   //   break;

   ///// [GAME STRINGS]
   //// [GAME STRING INSERT] -- 
   //case IDM_GAMESTRING_INSERT:
   //   onLanguageDocument_GameStringInsert(pDocument);
   //   break;

   //// [GAME STRING DELETE] -- Not yet supported
   //case IDM_GAMESTRING_DELETE:
   //   MessageBox(pDocument->hWnd, TEXT("Requires removing items from a binary tree"), TEXT("None"), MB_OK);
   //   break;

   //// [GAME STRING EDIT] -- Edit current GameString
   //case IDM_GAMESTRING_EDIT:
   //   //pDocument->oLabelData.iSubItem = 0;
   //   //ListView_EditLabel(pDocument->hStringList, pDocument->oLabelData.iItem);
   //   onCustomListViewItemEdit(&pDocument->oLabelData, pDocument->hStringList);
   //   break;

   //// [GAME STRING VIEW ERROR] -- Display error dialog
   //case IDM_GAMESTRING_VIEW_ERROR:
   //   onLanguageDocument_GameStringViewError(pDocument);
   //   break;

   //// [GAME STRING PROPERTIES] -- Invoke the document properties dialog
   //case IDM_GAMESTRING_PROPERTIES:
   //   utilReflectMessage(pDocument->hWnd, WM_COMMAND, MAKE_LONG(IDM_VIEW_DOCUMENT_PROPERTIES, 0), NULL);
   //   break;

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
      ListView_SetItemCount(pDocument->hPageList, getTreeNodeCount(getLanguageDocumentGamePageTree(pDocument)));
      ListView_SetItemState(pDocument->hPageList, 0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);    // Causes strings to be generated + displayed

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
   BOOL          bResult;

   // Prepare
   TRACK_FUNCTION();
   bResult = FALSE;

   // Examine source
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
         bResult = TRUE;
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

         //onLanguageDocument_SelectionChanged(pDocument, pMessage->idFrom, (NMLISTVIEW*)pMessage);

         // Pass to CustomDraw handler
         bResult = onCustomListViewNotify(pDocument->hWnd, FALSE, pMessage->idFrom, pMessage);
         break;

      /// [CUSTOM DRAW/HOVER]
      default:
         bResult = onCustomListViewNotify(pDocument->hWnd, FALSE, pMessage->idFrom, pMessage);
         break;
      }
      break;
   }
   
   // Return result
   END_TRACKING();
   return bResult;
}


/// Function name  : onLanguageDocument_PropertyChanged
// Description     : Refresh current string when data changes
//
// LANGUAGE_DOCUMENT*  pDocument  : [in] Document 
// CONST UINT          iControlID : [in] ID of the control responsible for the change
// 
VOID  onLanguageDocument_PropertyChanged(LANGUAGE_DOCUMENT*  pDocument, CONST UINT  iControlID)
{
   INT  iSelected;

   switch (iControlID)
   {
   case IDC_AUTHOR_EDIT:
   case IDC_TITLE_EDIT:
   case IDC_LANGUAGE_EDIT:
      // [NON-VIRTUAL] Update current string from current message
      if (!pDocument->bVirtual)
      {
         getRichEditText(pDocument->hRichEdit, pDocument->pCurrentMessage);
         generatePlainTextFromLanguageMessage(pDocument->pCurrentMessage, pDocument->pCurrentString);
      }

      // Redraw current string
      iSelected = ListView_GetNextItem(pDocument->hStringList, -1, LVNI_ALL | LVNI_SELECTED);
      ListView_RedrawItems(pDocument->hStringList, iSelected, iSelected);

      // [TEXT-CHANGED] Refresh Properties
      if (iControlID == IDC_LANGUAGE_EDIT)
         sendDocumentUpdated(AW_PROPERTIES);
      break;
   }
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
   ERROR_QUEUE*  pErrorQueue;
   GAME_PAGE*    pGamePage;
   GAME_STRING*  pGameString;
   LVITEM&       oOutput = pHeader->item;
   TCHAR*        szConverted = NULL;

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
            switch (oOutput.iSubItem)
            {
            /// [PAGE ID] -- Supply the PageID and determine the icon from whether the page is voiced or not
            case PAGE_COLUMN_ID:
               StringCchPrintf(oOutput.pszText, oOutput.cchTextMax, TEXT("%u"), pGamePage->iPageID);
               break;

            /// [TITLE/DESCRIPTION] Supply text or use placeholder
            case PAGE_COLUMN_TITLE:
            case PAGE_COLUMN_DESCRIPTION:
               CONST TCHAR*  szText = (oOutput.iSubItem == PAGE_COLUMN_TITLE ? pGamePage->szTitle : pGamePage->szDescription);

               // [CHECK] Ensure text is present
               if (lstrlen(szText))
               {
                  // Convert + Display text
                  generateConvertedString(szText, SPC_LANGUAGE_INTERNAL_TO_DISPLAY, szConverted);
                  StringCchCopy(oOutput.pszText, oOutput.cchTextMax, utilEither(szConverted, szText));
                  utilSafeDeleteString(szConverted);
               }
               else
               {
                  // [FAILED] Display placeholder in grey
                  StringCchCopy(oOutput.pszText, oOutput.cchTextMax, TEXT("[None]"));
                  oOutput.lParam  = (LPARAM)GetSysColor(COLOR_GRAYTEXT);
                  oOutput.mask   |= LVIF_COLOUR_TEXT;
               }
               break;
            }
      }
      /// [ERROR] 
      else if (oOutput.mask INCLUDES LVIF_TEXT)
         StringCchPrintf(oOutput.pszText, oOutput.cchTextMax, TEXT("ERROR: Item %d not found"), oOutput.iItem);
      else
         oOutput.pszText = TEXT("ERROR: Item not found");
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
         if (oOutput.mask INCLUDES LVIF_TEXT)
            switch (oOutput.iSubItem)
            {
            /// [ID] 
            case STRING_COLUMN_ID:
               StringCchPrintf(oOutput.pszText, oOutput.cchTextMax, TEXT("%u"), pGameString->iID);
               break;
            
            /// [TEXT] -- Supply the title or description
            case STRING_COLUMN_TEXT:
               LANGUAGE_MESSAGE*  pMessage;

               // [CUSTOM DRAW] Attempt to generate RichText from source         // [FIX] Do not provide RichText when LVN_GETDISPINFO does not originate from CustomDraw handler (Received from unknown window once per click)
               if (generateLanguageMessageFromGameString(pGameString, pMessage, pErrorQueue = createErrorQueue()) AND oOutput.lParam == LVIP_CUSTOM_DRAW)   
               {
                  // [RICH-TEXT] Mark RichText for destruction
                  oOutput.lParam = (LPARAM)pMessage;
                  oOutput.mask |= LVIF_RICHTEXT | LVIF_DESTROYTEXT;
               }
               else
               {
                  // [FORMATTING ERROR] Display raw text
                  generateConvertedString(pGameString->szText, SPC_LANGUAGE_INTERNAL_TO_DISPLAY, szConverted);
                  StringCchCopy(oOutput.pszText, oOutput.cchTextMax, utilEither(szConverted, pGameString->szText));
                  // Cleanup
                  utilSafeDeleteString(szConverted);
                  deleteLanguageMessage(pMessage);
               }

               // Cleanup
               deleteErrorQueue(pErrorQueue);
               break;
            }
      }
      /// [ERROR] 
      else if (oOutput.mask INCLUDES LVIF_TEXT)
         StringCchPrintf(oOutput.pszText, oOutput.cchTextMax, TEXT("ERROR: Item %d not found"), oOutput.iItem);
      else
         oOutput.pszText = TEXT("ERROR: Item not found");
      break;
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

   // [TRACK]
   END_TRACKING();
}

/// Function name  : onLanguageDocument_PageSelectionChanged
// Description     : Displays new page strings
// 
// LANGUAGE_DOCUMENT*  pDocument : [in] Document
// CONST INT           iItem     : [in] Item index  (Only valid when bSelected == TRUE)
// CONST BOOL          bSelected : [in] Whether selected or de-selected
// 
VOID   onLanguageDocument_PageSelectionChanged(LANGUAGE_DOCUMENT*  pDocument, CONST INT  iItem, CONST BOOL  bSelected)
{
   if (bSelected)
   {
      // [CHECK] Ensure Page + PageStrings do not exist
      ASSERT(!pDocument->pCurrentPage AND !pDocument->pPageStringsByID);

      // Generate strings for new page
      findLanguageDocumentGamePageByIndex(pDocument, iItem, pDocument->pCurrentPage);
      pDocument->pPageStringsByID = generateLanguagePageStringsTree(pDocument, pDocument->pCurrentPage);

      /// Display contents + select first
      ListView_SetItemCount(pDocument->hStringList, getTreeNodeCount(pDocument->pPageStringsByID));
      ListView_SetItemState(pDocument->hStringList, 0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
   }
   else
   {
      // [CHECK] Ensure Page + PageStrings exist
      ASSERT(pDocument->pCurrentPage AND pDocument->pPageStringsByID);

      // Deselect current string, if any
      ListView_SetItemState(pDocument->hStringList, -1, NULL, LVIS_SELECTED);
      ListView_SetItemCount(pDocument->hStringList, 0);

      // Delete existing page contents
      deleteAVLTree(pDocument->pPageStringsByID);
      pDocument->pCurrentPage = NULL;
   }
}

/// Function name  : onLanguageDocument_StringSelectionChanged
// Description     : Displays new string
// 
// LANGUAGE_DOCUMENT*  pDocument : [in] Document
// CONST INT           iItem     : [in] Item index  (Only valid when bSelected == TRUE)
// CONST BOOL          bSelected : [in] Whether selected or de-selected
// 
VOID   onLanguageDocument_StringSelectionChanged(LANGUAGE_DOCUMENT*  pDocument, CONST INT  iItem, CONST BOOL  bSelected)
{
   IRichEditOle*  pRichEdit;  // RichEdit control OLE interface
   REOBJECT       oImage;     // RichEdit control OLE object attributes

   // Disable EN_CHANGE notifications
   INT  iOldMask = RichEdit_SetEventMask(pDocument->hRichEdit, RichEdit_GetEventMask(pDocument->hRichEdit) ^ ENM_CHANGE);

   /// [SELECTED] Display string in RichEdit
   if (bSelected)
   {
      // Lookup desired string
      findLanguageDocumentGameStringByIndex(pDocument, iItem, pDocument->pCurrentString);
     
      // Reset message/errors
      if (pDocument->pCurrentMessage)
      {
         deleteLanguageMessage(pDocument->pCurrentMessage);
         clearErrorQueue(pDocument->pFormatErrors);
      }

      // Generate RichText + display
      if (pDocument->bFormattingError = !generateLanguageMessageFromGameString(pDocument->pCurrentString, pDocument->pCurrentMessage, pDocument->pFormatErrors))
         SetWindowText(pDocument->hRichEdit, TEXT(""));
      else
      {
         // Prepare
         utilZeroObject(&oImage, REOBJECT);
         oImage.cbStruct = sizeof(REOBJECT);
         RichEdit_GetOLEInterface(pDocument->hRichEdit, &pRichEdit);

         // Insert text + buttons
         setRichEditText(pDocument->hRichEdit, pDocument->pCurrentMessage, GTC_BLACK);
         Edit_SetModify(pDocument->hRichEdit, FALSE);

         // Store button data
         for (INT iIndex = 0; pRichEdit->GetObject(iIndex, &oImage, REO_GETOBJ_POLEOBJ) == S_OK; iIndex--)
            insertObjectIntoAVLTree(pDocument->pButtonsByID, (LPARAM)(LANGUAGE_BUTTON*)oImage.dwUser);
         
         // Cleanup
         utilReleaseInterface(pRichEdit);
      }

      // Enable/Disable window by result
      EnableWindow(pDocument->hRichEdit, !pDocument->bFormattingError);

      // [UPDATED] Refresh properties
      sendDocumentUpdated(AW_PROPERTIES);
   }
   /// [UNSELECTED] Update string from RichEdit
   else
   {
      // [MODIFIED + NON-VIRTUAL] Generate new source-text from RichEdit
      if (!pDocument->bVirtual AND Edit_GetModify(pDocument->hRichEdit))
      {
         getRichEditText(pDocument->hRichEdit, pDocument->pCurrentMessage);
         generatePlainTextFromLanguageMessage(pDocument->pCurrentMessage, pDocument->pCurrentString);
      }

      // Disable/Clear RichEdit
      SetWindowText(pDocument->hRichEdit, TEXT(""));
      EnableWindow(pDocument->hRichEdit, FALSE);
   }

   // Re-Enable EN_CHANGE
   RichEdit_SetEventMask(pDocument->hRichEdit, iOldMask);
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
         if (iResult = onLanguageDocument_Notify(pDocument, (NMHDR*)lParam))
            break;

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

      /// [CUSTOM MENU/CUSTOM COMBO]
      case WM_DRAWITEM:    onWindow_DrawItem((DRAWITEMSTRUCT*)lParam);                  break;
      case WM_MEASUREITEM: onWindow_MeasureItem(hWnd, (MEASUREITEMSTRUCT*)lParam);      break;
      case WM_DELETEITEM:  onWindow_DeleteItem((DELETEITEMSTRUCT*)lParam);              break;

      /// [DOCUMENT PROPERTY CHANGED]
      case UN_PROPERTY_UPDATED:
         onLanguageDocument_PropertyChanged(pDocument, wParam);
         break;

      // [UNHANDLED]
      default:
         iResult = DefWindowProc(hWnd, iMessage, wParam, lParam);
         break;
      }
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

