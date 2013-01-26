//
// Media Document.cpp : Implements the creation and display of the media browser document window
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       CONSTANTS / GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

#define       GROUP_COLUMN_ID           0      // Column index of the 'Group ID' column of the GamePage list
#define       GROUP_COLUMN_TITLE        1      // Column index of the 'Title' column of the GamePage list

#define       ITEM_COLUMN_ID            0      // Column index of the 'Item ID' column of the MediaItem list
#define       ITEM_COLUMN_DESCRIPTION   1      // Column index of the 'Description' column of the MediaItem list
#define       ITEM_COLUMN_DURATION      2      // Column index of the 'Duration' column of the MediaItem list

#define       MEDIA_PAGE_TYPES          3

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       CREATION / DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createMediaDocument
// Description     : Create a new 'media document' object and window
// 
// HWND  hParentWnd   : [in] Parent window for the document window
// 
// Return Value   : New MEDIA_DOCUMENT object if succesful, otherwise NULL
// 
MEDIA_DOCUMENT*   createMediaDocument(HWND  hParentWnd)
{
   MEDIA_DOCUMENT*  pNewDocument;

   /// Create document object and window
   pNewDocument = utilCreateEmptyObject(MEDIA_DOCUMENT);
   pNewDocument->hWnd = CreateDialogParam(getResourceInstance(), TEXT("MEDIA_DOCUMENT"), hParentWnd, dlgprocMediaDocument, (LPARAM)pNewDocument);

   // [CHECK] Abort if window creation failed
   if (pNewDocument->hWnd == NULL)
   {
      ERROR_CHECK("creating media document window", FALSE);
      utilDeleteObject(pNewDocument);
      return NULL;
   }

   // Set properties
   pNewDocument->eType               = DT_MEDIA;
   ///REM: pNewDocument->szTitle             = utilDuplicateString(TEXT("Game Media"), 10);
   pNewDocument->oLabelData.iItem    = -1;
   pNewDocument->oLabelData.iSubItem = -1;

   // Return new document
   return pNewDocument;
}


/// Function name  : createMediaDocumentItemsTree
// Description     : Creates the binary tree storing the items for the currently selected page
// 
// Return Value   : New MediaItems tree, you are responsible for destroying it
// 
AVL_TREE*  createMediaDocumentItemsTree()
{
   return createAVLTree(extractMediaItemTreeNode, NULL, createAVLTreeSortKey(AK_ID, AO_ASCENDING), NULL, NULL);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                            HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : getMediaDocumentData
// Description     : Retrieve the MediaDocument data associated with a MediaDocument window
// 
// HWND  hDialog   : [in] MediaDocument window handle
// 
// Return Value   : Media document window data
// 
MEDIA_DOCUMENT*  getMediaDocumentData(HWND  hDialog)
{
   return (MEDIA_DOCUMENT*)GetWindowLong(hDialog, DWL_USER);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                           FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : calculateMediaDocumentControlRect
// Description     : Calculate the position of a control in a media or language document based on it's current size
// 
// LANGUAGE_DOCUMENT* pDocument   : [in]  Language Document data
// CONST RECT*        pDocumentRect : [in]  Document dialog client rectangle
// CONST UINT         iControlID  : [in]  ID of a control in the document window who's position is to be calculated
// RECT*              pOutput     : [out] Output rectangle for the control
// 
VOID      calculateMediaDocumentControlRect(MEDIA_DOCUMENT*  pDocument, CONST RECT*  pDocumentRect, CONST UINT iControlID, RECT*  pOutput)
{
   SIZE         siDocumentSize,    // Size of the document's dialog
                siLeftPane,    // Size of the LHS 'pane' rectangle
                siRightPane,   // Size of the RHS 'pane' rectangle
                siControlsSize;  // Size of the player controls
   RECT         rcRightPane,   // Right hand side 'pane' rectangle
                rcControlsRect;  // Rectangle of the player controls
   MEDIA_ITEM*  pMediaItem;
   UINT         iBorderSize;

   // Determine size of document
   utilConvertRectangleToSize(pDocumentRect, &siDocumentSize);

   // Calculate border size and 'player controls' height
   iBorderSize = 2 * GetSystemMetrics(SM_CXEDGE);
   utilGetDlgItemRect(pDocument->hItemDialog, IDC_MEDIA_PLAY, &rcControlsRect);
   utilConvertRectangleToSize(&rcControlsRect, &siControlsSize);

   /// LEFT PANE: Calculate left side pane
   if (iControlID == IDC_GROUP_LIST)
   {
      // Calculate width as 25%
      siLeftPane = siDocumentSize;
      siLeftPane.cx = utilCalculatePercentage(siDocumentSize.cx, 25);
      // Output
      utilSetRectangle(pOutput, pDocumentRect->left, pDocumentRect->top, siLeftPane.cx, siLeftPane.cy);
   }
   else
   {
      rcRightPane = *pDocumentRect;
      // Stretch RHS over remaining 75%
      rcRightPane.left += utilCalculatePercentage(siDocumentSize.cx, 25) + iBorderSize;
      utilConvertRectangleToSize(&rcRightPane, &siRightPane);

      // Determine current page type by examine first MediaItem in the items list
      findMediaDocumentItemByIndex(pDocument, 0, pMediaItem);
      ASSERT(pMediaItem);

      /// [ITEM LIST]: Calculate top portion of existing right pane
      *pOutput = rcRightPane;
      switch (iControlID)
      {
      case IDC_ITEM_LIST:
         switch (pMediaItem->eType)
         {
         // [SOUND/SPEECH] -- Create rectangle covering entire RHS minus the controls
         case MIT_SOUND_EFFECT:
         case MIT_SPEECH_CLIP:
            // Define as bottom 40 pixels of RHS pane
            pOutput->bottom -= siControlsSize.cy;
            break;

         // [VIDEO] -- Use top 50% of pane
         case MIT_VIDEO_CLIP:
            pOutput->bottom -= (siRightPane.cy / 2);
            break;
         }
         // Add border gap to the top half
         pOutput->bottom -= iBorderSize;
         break;
      
      /// [PLAYER DIALOG] Calculate bottom-right pane
      case IDC_ITEM_DIALOG:
         switch (pMediaItem->eType)
         {
         // [SOUND/SPEECH] -- Create rectangle just high enough to display player controls
         case MIT_SOUND_EFFECT:
         case MIT_SPEECH_CLIP:
            // Define as bottom 40 pixels of RHS pane
            pOutput->top = (pOutput->bottom - siControlsSize.cy);
            break;

         // [VIDEO] -- Use bottom 50% of pane
         case MIT_VIDEO_CLIP:
            pOutput->top += (siRightPane.cy / 2);
            break;
         }
         break;
      }
   }
}


/// Function name  : displayMediaDocumentPages
// Description     : 
// 
// MEDIA_DOCUMENT*  pDocument   : [in] 
// 
VOID  displayMediaDocumentPages(MEDIA_DOCUMENT*  pDocument)
{
   UINT            iGroupCounts[MEDIA_PAGE_TYPES];
   CONST TCHAR*    szGroupNames[MEDIA_PAGE_TYPES] = { TEXT("Sound Effects"), TEXT("Video Clips"), TEXT("Speech Clips") };
   LISTVIEW_GROUP  oGroup;

   // Count MediaPage types
   utilZeroObjectArray(iGroupCounts, UINT, MEDIA_PAGE_TYPES);
   performBinaryTreeMediaPageTypeCount(getGameData()->pMediaPagesByGroup, iGroupCounts, MEDIA_PAGE_TYPES);

   /// Create one Group for each MediaPage type
   for (UINT i = 0; i < MEDIA_PAGE_TYPES; i++)
   {
      // Define group
      oGroup.iID    = i;
      oGroup.iCount = iGroupCounts[i];
      oGroup.szName = utilLoadString(getResourceInstance(), IDS_MEDIA_TYPE_SOUND_EFFECT + i, 64);
      // Add to Grouped ListView
      /// REMOVED: addGroupedListViewGroup(pDocument->hGroupList, &oGroup);
      utilDeleteString(oGroup.szName);
   }

   /// Set ListView count and select first item
   ListView_SetItemCount(pDocument->hGroupList, getGameData()->pMediaPagesByGroup->iCount);
   ListView_SetItemState(pDocument->hGroupList, 0, LVIS_SELECTED, LVIS_SELECTED);
}


/// Function name  : displayMediaDocumentPageItems
// Description     : Rebuild and display a new MediaItems tree based on the given group ID
// 
// MEDIA_DOCUMENT*  pDocument  : [in] MediaDocument data
// CONST UINT       iGroupID   : [in] ListView Index of the MediaPage defining the group to display
// 
VOID  displayMediaDocumentPageItems(MEDIA_DOCUMENT*  pDocument, CONST UINT  iIndex)
{
   MEDIA_PAGE*  pMediaPage;      // MediaPage associated with the selected index
   MEDIA_ITEM*  pMediaItem;      // MediaItem associated with the first item in the MediaPage above

   // Delete existing tree, if any
   if (pDocument->pCurrentMediaItemsByID)
      deleteAVLTree(pDocument->pCurrentMediaItemsByID);

   // Lookup selected MediaPage 
   findMediaPageByIndex(iIndex, pMediaPage);
   ASSERT(pMediaPage);

   /// Create new tree based on current group ID
   pDocument->iCurrentGroupID = pMediaPage->iPageID;
   pDocument->pCurrentMediaItemsByID = createMediaDocumentItemsTree();

   /// Popuplate new media items tree
   performBinaryTreeMediaPageReplication(getGameData()->pMediaItemsByID, pMediaPage->eType, pMediaPage->iPageID, pDocument->pCurrentMediaItemsByID);
   performAVLTreeIndexing(pDocument->pCurrentMediaItemsByID);

   /// Update ListView
   ListView_SetItemCount(pDocument->hItemList, pDocument->pCurrentMediaItemsByID->iCount);

   // Select and load first item
   if (pDocument->pCurrentMediaItemsByID->iCount > 0)
   {
      // Select first item
      ListView_SetItemState(pDocument->hItemList, -1, NULL,          LVIS_SELECTED);
      ListView_SetItemState(pDocument->hItemList,  0, LVIS_SELECTED, LVIS_SELECTED);

      // Find associated MediaItem
      findMediaDocumentItemByIndex(pDocument, 0, pMediaItem);
      ASSERT(pMediaItem);

      // Order player dialog to load the selected item
      SendMessage(pDocument->hItemDialog, UM_LOAD_MEDIA_ITEM, NULL, (LPARAM)pMediaItem);
   }
}


/// Function name  : findMediaItemByIndex
// Description     : Find a media item in the current group being displayed in a media document
// 
// MEDIA_DOCUMENT*  pDocument : [in] Media Document data
// CONST UINT    iIndex       : [in]  Zero-based Index of the media item to find
// MEDIA_ITEM*  &pOutput      : [out] Resultant media item if found, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BOOL  findMediaDocumentItemByIndex(MEDIA_DOCUMENT*  pDocument, CONST UINT iIndex, MEDIA_ITEM*  &pOutput)
{
   // Search MediaDocument 'current group' tree
   return findObjectInAVLTreeByIndex(pDocument->pCurrentMediaItemsByID, iIndex, (LPARAM&)pOutput);
}


/// Function name  : initMediaDocument
// Description     : Initialise a new 'Media Document' window
// 
// MEDIA_DOCUMENT*  pDocument : [in] Media Document data
// HWND             hDialog   : [in] Window handle of the document window
// 
// Return Value   : TRUE
// 
BOOL   initMediaDocument(MEDIA_DOCUMENT*  pDocument, HWND  hDialog)
{
   // Store window handle and document data
   pDocument->hWnd = hDialog;
   SetWindowLong(hDialog, DWL_USER, (LONG)pDocument);

   // Store child window convenience pointers
   pDocument->hGroupList = GetDlgItem(hDialog, IDC_GROUP_LIST);
   pDocument->hItemList  = GetDlgItem(hDialog, IDC_ITEM_LIST);

   // Create document controls 
   initMediaDocumentControls(pDocument);

   // Create player dialog
   pDocument->hItemDialog = createMediaPlayerDialog(hDialog);
   ShowWindow(pDocument->hItemDialog, SW_SHOW);
   UpdateWindow(pDocument->hItemDialog);
   
   /// Display pages and items from the first page
   displayMediaDocumentPages(pDocument);
   displayMediaDocumentPageItems(pDocument, 0);
   return TRUE;
}


/// Function name  : initMediaDocumentControls
// Description     : Initialise the MediaDocument ListViews
// 
// MEDIA_DOCUMENT*  pDocument  : [in] MediaDocument window data
// 
// Return Value   : TRUE
// 
BOOL   initMediaDocumentControls(MEDIA_DOCUMENT*  pDocument)
{
   UINT      iColumnWidths[2][3] = { 60,200,NULL,  70,400,70 };
   LVCOLUMN  oColumn;

   // Create ImageList
   pDocument->hImageList = ImageList_Create(16, 16, ILC_COLOR32|ILC_MASK, 3, 1);
   ImageList_AddIcon(pDocument->hImageList, LoadIcon(getResourceInstance(), TEXT("SOUND_EFFECT_ICON")));
   ImageList_AddIcon(pDocument->hImageList, LoadIcon(getResourceInstance(), TEXT("VIDEO_CLIP_ICON")));
   ImageList_AddIcon(pDocument->hImageList, LoadIcon(getResourceInstance(), TEXT("VOICED_YES_ICON")));

   /// Initialise ListViews
   ///REMOVED:  initGroupedListView(pDocument->hGroupList);
   // Set Styles
   SendMessage(pDocument->hGroupList, LVM_SETVIEW, LV_VIEW_DETAILS, NULL);
   SendMessage(pDocument->hGroupList, LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
   SendMessage(pDocument->hItemList,  LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
   // Assign ImageLists
   SendMessage(pDocument->hGroupList, LVM_SETIMAGELIST, LVSIL_SMALL, (LPARAM)pDocument->hImageList);
   SendMessage(pDocument->hItemList,  LVM_SETIMAGELIST, LVSIL_SMALL, (LPARAM)pDocument->hImageList);

   /// Create 'Groups' columns
   for (UINT iColumn = GROUP_COLUMN_ID; iColumn <= GROUP_COLUMN_TITLE; iColumn++)
   {
      // Define and Insert column
      utilSetListViewColumn(&oColumn, iColumn, utilLoadString(getResourceInstance(), IDS_MEDIA_GROUP_COLUMN_ID + iColumn, 64), iColumnWidths[0][iColumn]);
      ListView_InsertColumn(pDocument->hGroupList, oColumn.iSubItem, &oColumn);
      // Cleanup
      utilDeleteString(oColumn.pszText);
   }

   /// Create 'Items' columns
   for (UINT iColumn = ITEM_COLUMN_ID; iColumn <= ITEM_COLUMN_DURATION; iColumn++)
   {
      // Define and Insert column
      utilSetListViewColumn(&oColumn, iColumn, utilLoadString(getResourceInstance(), IDS_MEDIA_ITEM_COLUMN_ID + iColumn, 64), iColumnWidths[1][iColumn]);
      ListView_InsertColumn(pDocument->hItemList, oColumn.iSubItem, &oColumn);
      // Cleanup
      utilDeleteString(oColumn.pszText);
   }

   return TRUE;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         MESSAGE HANDLERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onMediaDocumentCommand
// Description     : Performs MediaDocument command processing
// 
// MEDIA_DOCUMENT*  pDocument      : [in] MediaDocument window data
// HWND             hDialog        : [in] Window handle of the MediaDocument window
// CONST UINT       iControlID     : [in] ID of the control sending the notification
// CONST UINT       iNotification  : [in] [CONTROLS] Notification code
// HWND             hCtrl          : [in] [CONTROLS] Window handle of the control sending the noficaition
// 
// Return Value   : TRUE if processed, FALSE otherwise
// 
BOOL  onMediaDocumentCommand(MEDIA_DOCUMENT*  pDocument, HWND  hDialog, CONST UINT  iControlID, CONST UINT  iNotification, HWND  hCtrl)
{
   return FALSE;
}

/// Function name  : onMediaDocumentDestroy
// Description     : Destroy a media document window and object
// 
// MEDIA_DOCUMENT*  pDocument : [in] MediaDocument window data
// HWND             hDialog   : [in] MediaDocument window handle
// 
VOID  onMediaDocumentDestroy(MEDIA_DOCUMENT*  pDocument, HWND  hDialog)
{
   /// Destroy player dialog
   utilDeleteWindow(pDocument->hItemDialog);

   /// Delete MediaItems tree
   deleteAVLTree(pDocument->pCurrentMediaItemsByID);

   // Delete ListView ImageList
   ImageList_Destroy(pDocument->hImageList);
   // Delete document title
   ///REM: utilDeleteString(pDocument->szTitle);

   /// Delete document data      BUG: Destroyed by removeDocumentFromDocumentsCtrl() since destroying here seems to cause problems
   //utilDeleteObject(pDocument);
   //SetWindowLong(hDialog, DWL_USER, NULL);

   /// TODO: ARE WE SURE THIS CAUSES A BUG?  IT'S FUCKING WEIRD DELETING DOCUMENT FAR AWAY...
}


/// Function name  : onMediaDocumentNotify
// Description     : Handles MediaDocument notifications
// 
// MEDIA_DOCUMENT*  pDocument : [in] MediaDocument window data
// HWND             hDialog   : [in] Window handle of the media document window
// NMHDR*           pMessage  : [in] WM_NOTIFY message data
// 
// Return Value   : TRUE if processed, FALSE otherwise
// 
BOOL  onMediaDocumentNotify(MEDIA_DOCUMENT*  pDocument, HWND  hDialog, NMHDR*  pMessage)
{
   NMLISTVIEW*  pListView;
   ///INT          iCustomDrawFlag;

   switch (pMessage->code)
   {
   /// [LISTVIEW REQUEST DATA]
   case LVN_GETDISPINFO:
      onMediaDocumentRequestData(pDocument, pMessage->idFrom, (NMLVDISPINFO*)pMessage);
      return TRUE;
      
   /// [LISTVIEW SELECTION CHANGED]
   case LVN_ITEMCHANGED:
      pListView = (NMLISTVIEW*)pMessage;
      if (pListView->iItem != -1)
      {
         // Track the 'focus' state instead of the 'selection' state because it's easiest that way.
         onMediaDocumentSelectionChange(pDocument, pMessage->idFrom, pListView->iItem, pListView->uNewState INCLUDES LVIS_FOCUSED ? TRUE : FALSE);
         return TRUE;
      }
      break;

   /// [GROUPED LISTVIEW CUSTOM DRAW]
   case NM_CUSTOMDRAW:
      if (pMessage->idFrom == IDC_GROUP_LIST)
      {
         /// REMOVED: iCustomDrawFlag = onGroupedListViewCustomDraw(pDocument->hGroupList, (NMLVCUSTOMDRAW*)pMessage);
         /// REMOVED: SetWindowLong(hDialog, DWL_MSGRESULT, iCustomDrawFlag);
         return TRUE;
      }
      break;
   }

   return FALSE;
}

/// Function name  : onMediaDocumentResize
// Description     : Resize the document controls
// 
// MEDIA_DOCUMENT*  pDocument      : [in] Document window data
// HWND             hDocumentsCtrl : [in] Documents Tab Control window handle
// 
VOID  onMediaDocumentResize(MEDIA_DOCUMENT*  pDocument, HWND  hDocumentsCtrl)
{
   RECT    rcDisplayRect,      // Display rectangle for the document (within the documents control)
           rcClientRect,       // Client rectangle for the document
           rcCtrlRect;         // Rectangle for each control, in document client co-ordinates
   SIZE    siDisplaySize,      // Size of the display rectangle
           siCtrlSize;         // Size of the control rectangle
   
   // Get current 'display rectangle' from documents control
   GetClientRect(hDocumentsCtrl, &rcDisplayRect);
   SendMessage(hDocumentsCtrl, TCM_ADJUSTRECT, FALSE, (LPARAM)&rcDisplayRect);
   utilConvertRectangleToSize(&rcDisplayRect, &siDisplaySize);

   /// Stretch document over entire display area
   MoveWindow(pDocument->hWnd, rcDisplayRect.left, rcDisplayRect.top, siDisplaySize.cx, siDisplaySize.cy, FALSE);

   // Get resultant client rectangle, deflate to create a border
   GetClientRect(pDocument->hWnd, &rcClientRect);
   InflateRect(&rcClientRect, -GetSystemMetrics(SM_CXDLGFRAME), -GetSystemMetrics(SM_CYDLGFRAME));

   /// Resize Group List
   calculateMediaDocumentControlRect(pDocument, &rcClientRect, IDC_GROUP_LIST, &rcCtrlRect);
   utilConvertRectangleToSize(&rcCtrlRect, &siCtrlSize);
   MoveWindow(pDocument->hGroupList, rcCtrlRect.left, rcCtrlRect.top, siCtrlSize.cx, siCtrlSize.cy, FALSE);

   /// Resize String List
   calculateMediaDocumentControlRect(pDocument, &rcClientRect, IDC_ITEM_LIST, &rcCtrlRect);
   utilConvertRectangleToSize(&rcCtrlRect, &siCtrlSize);
   MoveWindow(pDocument->hItemList, rcCtrlRect.left, rcCtrlRect.top, siCtrlSize.cx, siCtrlSize.cy, FALSE);
   
   /// Resize RichText Dialog
   calculateMediaDocumentControlRect(pDocument, &rcClientRect, IDC_ITEM_DIALOG, &rcCtrlRect);
   utilConvertRectangleToSize(&rcCtrlRect, &siCtrlSize);
   MoveWindow(pDocument->hItemDialog, rcCtrlRect.left, rcCtrlRect.top, siCtrlSize.cx, siCtrlSize.cy, FALSE);

   InvalidateRect(pDocument->hWnd, NULL, TRUE);
}


/// Function name  : onMediaDocumentRequestData
// Description     : Supply item/subitem text for the specified ListView item
// 
// MEDIA_DOCUMENT*  pDocument   : [in]     MediaDocument data
// CONST UINT       iControlID  : [in]     ID of the ListView control requesting data
// NMLVDISPINFO*    pOutput     : [in/out] Output data
// 
VOID  onMediaDocumentRequestData(MEDIA_DOCUMENT*  pDocument, CONST UINT  iControlID, NMLVDISPINFO*  pOutput)
{
   MEDIA_PAGE*   pMediaPage;   // MediaPage object associated with the requested group index
   MEDIA_ITEM*   pMediaItem;   // MediaItem object associated with the requested item index
   GAME_STRING*  pGameString;  // GameString containing descriptions for speech MediaItems
   GAME_PAGE*    pGamePage;    // GamePage containing names of speech MediaPages
   TCHAR*        szConverted;

   // Default to returning plaintext
   pOutput->item.lParam = NULL;

   switch (iControlID)
   {
   /// [GROUP LIST]
   case IDC_GROUP_LIST:
      // Lookup associated MediaPage object
      findMediaPageByIndex(pOutput->item.iItem, pMediaPage);
      ASSERT(pMediaPage);

      // Examine sub-item
      switch (pOutput->item.iSubItem)
      {
      /// [GROUP ID] Supply text, image and indent
      case GROUP_COLUMN_ID:  
         switch (pMediaPage->eType)
         {
         // [SPEECH PAGE] -- Supply Page ID
         case MIT_SPEECH_CLIP: 
            ASSERT(pOutput->item.cchTextMax == 256);
            StringCchPrintf(pOutput->item.pszText, pOutput->item.cchTextMax, TEXT("%u"), pMediaPage->iPageID); 
            break;
         // [SOUND/VIDEO PAGE] -- Supply 'N/A'
         case MIT_SOUND_EFFECT:
         case MIT_VIDEO_CLIP:
            //StringCchCopy(pOutput->item.pszText, pOutput->item.cchTextMax, TEXT("None"));
            pOutput->item.pszText[0] = NULL;
            break;
         }
         pOutput->item.iImage = pMediaPage->eType;
         break;

      /// [GROUP TITLE] Supply text
      case GROUP_COLUMN_TITLE:
         switch (pMediaPage->eType)
         {
         // [SPEECH PAGE] -- Lookup the associated GamePage object in the game data
         case MIT_SPEECH_CLIP:
            findGamePageInTreeByID(getGameData()->pGamePagesByID, pMediaPage->iPageID, pGamePage);
            ASSERT(pGamePage);
            StringCchCopy(pOutput->item.pszText, pOutput->item.cchTextMax, pGamePage->szTitle);
            break;

         // [SOUND/VIDEO PAGE] -- Use custom title given by me during loading
         case MIT_SOUND_EFFECT:
         case MIT_VIDEO_CLIP:
            StringCchCopy(pOutput->item.pszText, pOutput->item.cchTextMax, pMediaPage->szTitle); 
            break;
         }
         break;
      }
      break;

   case IDC_ITEM_LIST:
      // Lookup associated MediaItem object
      findMediaDocumentItemByIndex(pDocument, pOutput->item.iItem, pMediaItem);
      ASSERT(pMediaItem);

      // Examine sub-item
      switch (pOutput->item.iSubItem)
      {
      /// [ITEM ID] Supply text, image and indent
      case ITEM_COLUMN_ID:  
         if (pOutput->item.mask INCLUDES LVIF_TEXT)
            StringCchPrintf(pOutput->item.pszText, pOutput->item.cchTextMax, TEXT("%u"), pMediaItem->iID); 
         if (pOutput->item.mask INCLUDES LVIF_IMAGE)
            pOutput->item.iImage = pMediaItem->eType;
         if (pOutput->item.mask INCLUDES LVIF_INDENT)
            pOutput->item.iIndent = NULL;
         break;

      /// [ITEM DESCRIPTION] Supply text
      case ITEM_COLUMN_DESCRIPTION:
         switch (pMediaItem->eType)
         {
         // [SPEECH ITEM] -- Lookup associated GameString containing the item's description
         case MIT_SPEECH_CLIP:
            if (findGameStringByID(pMediaItem->iID, pMediaItem->iPageID, pGameString))
            {
               generateConvertedString(pGameString->szText, SPC_LANGUAGE_INTERNAL_TO_DISPLAY, szConverted);
               StringCchCopy(pOutput->item.pszText, pOutput->item.cchTextMax, utilEither(szConverted, pGameString->szText));
               // Cleanup
               utilSafeDeleteString(szConverted);
            }
            else
               StringCchCopy(pOutput->item.pszText, pOutput->item.cchTextMax, TEXT("[None]")); 
            break;

         // [SOUND EFFECT / VIDEO CLIP] -- Use the description given in the sounds/videos.txt file
         case MIT_SOUND_EFFECT:
         case MIT_VIDEO_CLIP:
            StringCchCopy(pOutput->item.pszText, pOutput->item.cchTextMax, pMediaItem->szDescription); 
            break;
         }
         break;

      /// [ITEM DURATION] Calculate and supply duration text
      case ITEM_COLUMN_DURATION:
         formatMediaItemDuration(calculateMediaItemDuration(pMediaItem), pOutput->item.pszText, pOutput->item.cchTextMax);
         break;
      }
      break;
   }
}


/// Function name  : onMediaDocumentSelectionChange
// Description     : Called when the selection in either ListView changes
// 
// MEDIA_DOCUMENT*  pDocument  : [in] MediaDocument window data
// CONST UINT       iControlID : [in] ID of the ListView sending the notification
// CONST UINT       iItem      : [in] Zero-based listview item index of the newly selected (or unselected) item
// CONST BOOL       bSelected  : [in] Whether the item has been selected or unselected
// 
VOID  onMediaDocumentSelectionChange(MEDIA_DOCUMENT*  pDocument, CONST UINT  iControlID, CONST UINT  iItem, CONST BOOL  bSelected)
{
   ///LISTVIEW_ITEM   oGroupedItem;     // Properties of a GroupedListView item, for converting item index
   MEDIA_ITEM*             pMediaItem;

   switch (iControlID)
   {
   /// [GROUP LIST] -- Rebuild the current items tree
   case IDC_GROUP_LIST:
      if (bSelected)
      {
         // Convert physical item number to a logical item number
         /// REMOVED: getGroupedListViewItemProperties(pDocument->hGroupList, iItem, &oGroupedItem);

         // Re-build MediaItems tree using ID of the newly selected GamePage
         /// REM: if (oGroupedItem.eType == GLVIT_ITEM)
         /// REM: displayMediaDocumentPageItems(pDocument, oGroupedItem.iLogicalIndex);

         // Resize the item list and player dialog
         onMediaDocumentResize(pDocument, GetParent(pDocument->hWnd));
      }
      break;

   /// [ITEM LIST] -- Adjust Player dialog
   case IDC_ITEM_LIST:
      if (bSelected)
      {
         // Find associated MediaItem
         findMediaDocumentItemByIndex(pDocument, iItem, pMediaItem);
         ASSERT(pMediaItem);

         // Order player dialog to load the selected item
         SendMessage(pDocument->hItemDialog, UM_LOAD_MEDIA_ITEM, NULL, (LPARAM)pMediaItem);
      }
      break;
   }
}

/// Function name  : dlgprocMediaDocument
// Description     : MediaDocument window dialog procedure
// 
INT_PTR  dlgprocMediaDocument(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   MEDIA_DOCUMENT*  pDocument;

   // Get document window data
   pDocument = getMediaDocumentData(hDialog);

   switch (iMessage)
   {
   /// [CREATION] -- Initialise the document data and window
   case WM_INITDIALOG:
      // Extract input data and initialise document
      return initMediaDocument((MEDIA_DOCUMENT*)lParam, hDialog);

   /// [DESTRUCTION] -- Destroy the document data and window
   case WM_DESTROY:
      onMediaDocumentDestroy(pDocument, hDialog);
      return TRUE;

   // [COMMANDS]
   case WM_COMMAND:
      if (onMediaDocumentCommand(pDocument, hDialog, LOWORD(wParam), HIWORD(wParam), (HWND)lParam))
         return TRUE;
      break;

   // [NOTIFICAITON]
   case WM_NOTIFY:
      if (onMediaDocumentNotify(pDocument, hDialog, (NMHDR*)lParam))
         return TRUE;
      break;

   /// [RESIZING]
   case WM_SIZE:
      onMediaDocumentResize(pDocument, GetParent(hDialog));
      return TRUE;

   // [VISUAL STYLES]
   case WM_CTLCOLORDLG:
      return (INT_PTR)GetStockObject(WHITE_BRUSH);
   }

   return FALSE;
}


