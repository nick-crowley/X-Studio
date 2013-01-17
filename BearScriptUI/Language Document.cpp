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

/// Function name  : createLanguageDocumentGameStringsTree
// Description     : Create a GameString tree containing only strings for a specified page
// 
// LANGUAGE_DOCUMENT*  pDocument : [in] Language document data
// 
// Return Value   : New Binary tree, you are responsible for destroying it
// 
AVL_TREE*   createLanguageDocumentGameStringsTree(LANGUAGE_DOCUMENT*   pDocument)
{
   AVL_TREE*  pOutputTree;

   // Create COPY TREE
   pOutputTree = createAVLTree(extractGameStringTreeNode, NULL, createAVLTreeSortKey(AK_ID, AO_ASCENDING), NULL, NULL);

   // [CHECK] Ensure there is a current page
   if (pDocument->pCurrentPage)
      /// [SUCCESS] Copy strings with desired PageID 
      performBinaryTreeStringPageReplication(getLanguageDocumentGameStringTree(pDocument), pDocument->pCurrentPage->iPageID, pOutputTree);
      
   // Index and return tree
   performAVLTreeIndexing(pOutputTree);
   return pOutputTree;
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
   return findObjectInAVLTreeByIndex(pDocument->pCurrentGameStringsByID, iIndex, (LPARAM&)pOutput);
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

   // Update the GameString ListView item count
   ListView_SetItemCount(pDocument->hStringList, getTreeNodeCount(pGameStringTree));

   // Return TRUE
   return TRUE;
}
/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                            FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  :  createLanguageDialogControls
// Description     : 
// 
// LANGUAGE_DOCUMENT*  pDocument : [in] 
// 
// Return Value   : 
// 
BOOL   createLanguageDialogControls(LANGUAGE_DOCUMENT*  pDocument)
{
   LISTVIEW_COLUMNS  oPageListView   = { 3, IDS_LANGUAGE_PAGE_COLUMN_ID,   60, 100, 100,  NULL, NULL };
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
   pDocument->hPageList = CreateWindow(WC_LISTVIEW, TEXT("GamePage List"), WS_CHILD WITH WS_TABSTOP WITH LVS_SHOWSELALWAYS WITH LVS_REPORT WITH LVS_OWNERDATA WITH LVS_SINGLESEL, 0, 0, 
                                                                           siClient.cx, siClient.cy, pDocument->hWnd, (HMENU)IDC_PAGE_LIST, getAppInstance(), NULL);
   ERROR_CHECK("Creating GamePage listview", pDocument->hPageList);
   initReportModeListView(pDocument->hPageList, &oPageListView, TRUE);

   /// [STRING LIST] Create ListView
   pDocument->hStringList = CreateWindow(WC_LISTVIEW, TEXT("GameString List"), WS_CHILD WITH WS_TABSTOP WITH LVS_SHOWSELALWAYS WITH LVS_REPORT WITH LVS_OWNERDATA WITH LVS_SINGLESEL, 0, 0, 
                                                                                siClient.cx, siClient.cy, pDocument->hWnd, (HMENU)IDC_STRING_LIST, getAppInstance(), NULL);
   ERROR_CHECK("Creating GameString listview", pDocument->hStringList);
   initReportModeListView(pDocument->hStringList, &oStringListView, TRUE);

   /// [WORKSPACE]
   pDocument->hWorkspace = createWorkspace(pDocument->hWnd, &rcClient, pDocument->hPageList, IsThemeActive() ? COLOR_WINDOW : COLOR_BTNFACE);

   // Search for GamePage pane
   if (findWorkspacePaneByHandle(pDocument->hWorkspace, pDocument->hPageList, NULL, NULL, pTargetPane))
   {
      /// Add GameString list in horizontal split with the GamePage list
      setWorkspacePaneProperties(&oPaneData, FALSE, 320, NULL);
      insertWorkspaceWindow(pDocument->hWorkspace, pTargetPane, pDocument->hStringList, RIGHT, &oPaneData);
   }

   // [TRACK]
   END_TRACKING();
   return pDocument->hWorkspace AND pDocument->hPageList AND pDocument->hStringList;
}


/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         MESSAGE HANDLERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onLanguageDocumentContextMenu
// Description     : Display the Language document popup menu
// 
// SCRIPT_DOCUMENT*  pDocument  : [in] ScriptDocument window data
// CONST POINT*      ptCursor   : [in] Cursor position in screen co-ordinates
// HWND              hCtrl      : [in] Control sending the message
// 
VOID   onLanguageDocumentContextMenu(LANGUAGE_DOCUMENT*  pDocument, CONST POINT*  ptCursor, HWND  hCtrl)
{
   CUSTOM_MENU*    pCustomMenu;       // Popup CustomMenu
   GAME_PAGE*      pGamePage;
   UINT            iSelection,
                   iControlID;

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

         /// [PAGE VOICED]
         if (iSelection != -1 AND findLanguageDocumentGamePageByIndex(pDocument, iSelection, pGamePage))
            CheckMenuItem(pCustomMenu->hPopup, IDM_GAMEPAGE_VOICED, pGamePage->bVoice);
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


/// Function name  : onLanguageDocumentCommand
// Description     : Language Document menu item processing
//
// LANGUAGE_DOCUMENT*  pDocument     : [in] Language document
// CONST UINT          iControlID    : [in] Source control
// CONST UINT          iNotification : [in] Notifcation code
// HWND                hControl      : [in] Control window handle
// 
// Return type : TRUE if processed, FALSE otherwise
//
BOOL   onLanguageDocumentCommand(LANGUAGE_DOCUMENT*  pDocument, CONST UINT  iControlID, CONST UINT  iNotification, HWND  hControl)
{
   BOOL  bResult;

   // Prepare
   bResult = TRUE;

   //// Examine source
   //switch (iControlID)
   //{
   ///// [GAME PAGES]
   //// [GAME PAGE EDIT] -- Edit the currently selected GamePage
   //case IDM_GAMEPAGE_EDIT:
   //   onCustomListViewItemEdit(&pDocument->oLabelData, pDocument->hPageList);
   //   //onLanguageDocumentGamePageEdit(pDocument, pDocument->hPageList);
   //   break;

   //// [GAME PAGE ADD] -- Add a new item to the GamePage tree
   //case IDM_GAMEPAGE_INSERT:
   //   onLanguageDocumentGamePageInsert(pDocument);
   //   break;

   //// [GAME PAGE REMOVE] -- Not yet supported
   //case IDM_GAMEPAGE_DELETE:
   //   MessageBox(pDocument->hWnd, TEXT("Requires removing items from a binary tree"), TEXT("None"), MB_OK);
   //   break;

   //// [GAME PAGE VOICED] -- Toggle the 'voiced flag'
   //case IDM_GAMEPAGE_VOICED:
   //   onLanguageDocumentGamePageVoiceToggle(pDocument);
   //   break;

   ///// [GAME STRINGS]
   //// [GAME STRING INSERT] -- 
   //case IDM_GAMESTRING_INSERT:
   //   onLanguageDocumentGameStringInsert(pDocument);
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
   //   onLanguageDocumentGameStringViewError(pDocument);
   //   break;

   //// [GAME STRING PROPERTIES] -- Invoke the document properties dialog
   //case IDM_GAMESTRING_PROPERTIES:
   //   utilReflectMessage(pDocument->hWnd, WM_COMMAND, MAKE_LONG(IDM_VIEW_DOCUMENT_PROPERTIES, 0), NULL);
   //   break;

   //default:
   //   return FALSE;
   //}

   // Return result
   return bResult;
}


/// Function name  : onLanguageDocumentCreate
// Description     : Initialises the LanguageDocument controls and the LanguageDocument convenience pointers.
// 
// LANGUAGE_DOCUMENT*  pDocument : [in] LanguageDocument data
// HWND                hWnd      : [in] Document window handle
// 
VOID   onLanguageDocumentCreate(LANGUAGE_DOCUMENT*  pDocument, HWND  hWnd)
{  
   // [TRACK]
   TRACK_FUNCTION();

   /// Associate data with dialog
   SetWindowLong(hWnd, 0, (LONG)pDocument);
   pDocument->hWnd = hWnd;

   // Create document convenience pointer
   pDocument->pLanguageFile = (LANGUAGE_FILE*)pDocument->pGameFile;

   /// Create child windows
   if (createLanguageDialogControls(pDocument))
   {
      // Set GamePage count
      ListView_SetItemCount(pDocument->hPageList, getTreeNodeCount(getLanguageDocumentGamePageTree(pDocument)));
      performAVLTreeIndexing(getLanguageDocumentGamePageTree(pDocument));

      VERBOSE("LANGUAGE_FILE: There are %d pages", getTreeNodeCount(getLanguageDocumentGamePageTree(pDocument)));

      // [CHECK] Lookup first page
      if (findLanguageDocumentGamePageByIndex(pDocument, 0, pDocument->pCurrentPage))
      {
         /// [FOUND] Generate and display page strings
         pDocument->pCurrentGameStringsByID = createLanguageDocumentGameStringsTree(pDocument);
         ListView_SetItemCount(pDocument->hStringList, getTreeNodeCount(pDocument->pCurrentGameStringsByID));

         VERBOSE("LANGUAGE_FILE: There are %d strings", getTreeNodeCount(pDocument->pCurrentGameStringsByID));
      }

      // Show child windows
      ShowWindow(pDocument->hPageList, SW_SHOWNORMAL);
      ShowWindow(pDocument->hStringList, SW_SHOWNORMAL);
   }

   // [TRACK]
   END_TRACKING();
}


/// Function name  : onLanguageDocumentDestroy
// Description     : Cleanup the language document's data
// 
// LANGUAGE_DOCUMENT*  pDocument : [in] Language document data
// 
VOID   onLanguageDocumentDestroy(LANGUAGE_DOCUMENT*  pDocument)
{
   // [TRACK]
   TRACK_FUNCTION();

   /// [GAMESTRINGS] Delete GameStrings tree
   if (pDocument->pCurrentGameStringsByID)
      deleteAVLTree(pDocument->pCurrentGameStringsByID);

   /// [MESSAGE] Delete current message, if any
   if (pDocument->pCurrentMessage)
      deleteLanguageMessage(pDocument->pCurrentMessage);
   
   // Destroy workspace
   utilDeleteWindow(pDocument->hWorkspace);

   // [TRACK]
   END_TRACKING();
}


/// Function name  : onLanguageDocumentNotify
// Description     : Language document dialog WM_NOTIFY processing
// 
// LANGUAGE_DOCUMENT*  pDocument  : [in] Language document window data
// NMHDR*              pMessage   : [in] WM_NOTIFY message data
// 
// Return Value   : TRUE if message processed, FALSE if not
// 
BOOL   onLanguageDocumentNotify(LANGUAGE_DOCUMENT*  pDocument, NMHDR*  pMessage)
{
   //NMLISTVIEW*   pListInfo;
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
         onLanguageDocumentRequestData(pDocument, pMessage->idFrom, (NMLVDISPINFO*)pMessage);
         bResult = TRUE;
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



/// Function name  : onLanguageDocumentRequestData
// Description     : Supply item data for the GamePages ListView
//
// LANGUAGE_DOCUMENT*  pDocument  : [in]     Language document data
// CONST UINT          iControlID : [in]     ID of the control requesting data (Always IDC_PAGE_LIST)
// NMLVDISPINFO*       pHeader    : [in/out] Item to display / Item data
// 
// Return type : TRUE  
//
BOOL  onLanguageDocumentRequestData(LANGUAGE_DOCUMENT*  pDocument, CONST UINT  iControlID, NMLVDISPINFO*  pHeader)
{
   LVITEM&       oOutput = pHeader->item;
   GAME_PAGE*    pGamePage;
   GAME_STRING*  pGameString;
   TCHAR*        szConverted;

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
         // Examine column
         switch (oOutput.iSubItem)
         {
         /// [PAGE ID] -- Supply the PageID and determine the icon from whether the page is voiced or not
         case PAGE_COLUMN_ID:
            if (oOutput.mask INCLUDES LVIF_TEXT)
               StringCchPrintf(oOutput.pszText, oOutput.cchTextMax, TEXT("%u"), pGamePage->iPageID);
            if (oOutput.mask INCLUDES LVIF_IMAGE)
               oOutput.iImage = pGamePage->bVoice;
            if (oOutput.mask INCLUDES LVIF_INDENT)
               oOutput.iIndent = 0;
            break;
         
         /// [TITLE] Supply title
         case PAGE_COLUMN_TITLE:
            if (utilIncludes(oOutput.mask, LVIF_TEXT))
            {
               // [CHECK] Is title present?
               if (lstrlen(pGamePage->szTitle))
               {
                  // [SUCCESS] Display converted title
                  generateConvertedString(pGamePage->szTitle, SPC_LANGUAGE_INTERNAL_TO_DISPLAY, szConverted);
                  StringCchCopy(oOutput.pszText, oOutput.cchTextMax, utilEither(szConverted, pGamePage->szTitle));
                  // Cleanup
                  utilSafeDeleteString(szConverted);
               }
               else
               {
                  // [FAILED] Display placeholder in grey
                  StringCchCopy(oOutput.pszText, oOutput.cchTextMax, TEXT("[No Title]"));
                  oOutput.lParam  = (LPARAM)GetSysColor(COLOR_GRAYTEXT);
                  oOutput.mask   |= LVIF_COLOUR_TEXT;
               }
            }
            break;

         /// [DESCRIPTION] Supply description
         case PAGE_COLUMN_DESCRIPTION:
            if (utilIncludes(oOutput.mask, LVIF_TEXT))
            {
               // [CHECK] Ensure description exists
               if (lstrlen(pGamePage->szDescription))
               {
                  // Display converted description
                  generateConvertedString(pGamePage->szDescription, SPC_LANGUAGE_INTERNAL_TO_DISPLAY, szConverted);
                  StringCchCopy(oOutput.pszText, oOutput.cchTextMax, utilEither(szConverted, pGamePage->szDescription));
                  // Cleanup
                  utilSafeDeleteString(szConverted);
               }
               else
               {
                  // [FAILED] Display placeholder in grey
                  StringCchCopy(oOutput.pszText, oOutput.cchTextMax, TEXT("[No Description]"));
                  oOutput.lParam  = (LPARAM)GetSysColor(COLOR_GRAYTEXT);
                  oOutput.mask   |= LVIF_COLOUR_TEXT;
               }
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
         // Examine column
         switch (oOutput.iSubItem)
         {
         /// [STRING ID] Supply the StringID and determine the icon 
         case STRING_COLUMN_ID:
            if (oOutput.mask INCLUDES LVIF_TEXT)
               StringCchPrintf(oOutput.pszText, oOutput.cchTextMax, TEXT("%u"), pGameString->iID);

            if (oOutput.mask INCLUDES LVIF_IMAGE)
               oOutput.iImage = getAppImageTreeIconIndex(ITS_SMALL, identifyGameVersionIconID(pGameString->eVersion));
            break;
         
         /// [TEXT] -- Supply the title or description
         case STRING_COLUMN_TEXT:
            if (oOutput.mask INCLUDES LVIF_TEXT)
            {
               //// [TEXT] Provide RichText
               //pItem->lParam = (LPARAM)xResult.asCommandSyntax->pDisplaySyntax;
               //pItem->mask  |= LVIF_RICHTEXT;

               StringCchCopy(oOutput.pszText, oOutput.cchTextMax, pGameString->szText);
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
   }

   END_TRACKING();
   return TRUE;
}


/// Function name  : onLanguageDocumentResize
// Description     : Resizes a language document and repositions it's child windows
// 
// LANGUAGE_DOCUMENT* pDocument : [in] Language document
// CONST SIZE*        pNewSize  : [in] New window size
// 
VOID   onLanguageDocumentResize(LANGUAGE_DOCUMENT*  pDocument, CONST SIZE*  pNewSize)
{
   // [TRACK]
   TRACK_FUNCTION();

   // Resize workspace
   SetWindowPos(pDocument->hWorkspace, NULL, NULL, NULL, pNewSize->cx, pNewSize->cy, SWP_NOMOVE WITH SWP_NOZORDER);

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
         onLanguageDocumentCreate(pDocument, hWnd);
         break;

      /// [DESTRUCTION] 
      case WM_DESTROY:
         onLanguageDocumentDestroy(pDocument);
         break;

      /// [COMMAND PROCESSING]
      case WM_COMMAND:
         if (!onLanguageDocumentCommand(pDocument, LOWORD(wParam), HIWORD(wParam), (HWND)lParam))
            // [UNHANDLED] Call default window proc
            iResult = DefWindowProc(hWnd, iMessage, wParam, lParam);
         break;

      /// [NOTIFICATION]
      case WM_NOTIFY:
         iResult = onLanguageDocumentNotify(pDocument, (NMHDR*)lParam);

         if (!iResult)
            // [UNHANDLED] Call default window proc
            iResult = DefWindowProc(hWnd, iMessage, wParam, lParam);
         break;

      /// [RESIZE] - Resize tab to the size of the tab control
      case WM_SIZE:
         // Prepare
         siWindow.cx = LOWORD(lParam);
         siWindow.cy = HIWORD(lParam);
         // Resize
         onLanguageDocumentResize(pDocument, &siWindow);
         break;

      /// [CONTEXT MENU]
      case WM_CONTEXTMENU:
         ptCursor.x = GET_X_LPARAM(lParam);
         ptCursor.y = GET_Y_LPARAM(lParam);
         onLanguageDocumentContextMenu(pDocument, &ptCursor, (HWND)wParam);
         break;

      /// [MENU ITEM HOVER] Forward messages from CodeEdit up the chain to the Main window
      case WM_MENUSELECT:
         sendAppMessage(AW_MAIN, WM_MENUSELECT, wParam, lParam);
         break;

      /// [CUSTOM MENU/CUSTOM COMBO]
      case WM_DRAWITEM:    onWindow_DrawItem((DRAWITEMSTRUCT*)lParam);                  break;
      case WM_MEASUREITEM: onWindow_MeasureItem(hWnd, (MEASUREITEMSTRUCT*)lParam);      break;
      case WM_DELETEITEM:  onWindow_DeleteItem((DELETEITEMSTRUCT*)lParam);              break;

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

