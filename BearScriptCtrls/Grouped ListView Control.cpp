//
// Group ListView Control.cpp : A grouped virtual ListView control
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"


/// ////////////////////////////////////////////////////////////////////////////////////////
///                                  CREATION / DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createGroupedListViewData
// Description     : Creates window data for a GroupedListView
// 
// Return Value   : New window data for GroupedListView, you are responsible for destroying it
// 
GROUPED_LISTVIEW_DATA*   createGroupedListViewData()
{
   GROUPED_LISTVIEW_DATA*   pWindowData;       // Object being created

   // Create object
   pWindowData = utilCreateEmptyObject(GROUPED_LISTVIEW_DATA);

   // Set properties
   pWindowData->pGroupList = createList(destructorGroupedListViewGroup);

   // Return object
   return pWindowData;
}


/// Function name  : createGroupedListViewGroup
// Description     : Creates a GroupedListView group with an initial item count of zero
// 
// CONST UINT  iGroupID             : [in] Unique ID of the group
// CONST UINT  iGroupNameResourceID : [in] ResourceID of the string containing the name of the group
// 
// Return Value   : New LISTVIEW_GROUP, you are responsible for destroying it
// 
ControlsAPI
LISTVIEW_GROUP*   createGroupedListViewGroup(CONST UINT  iGroupID, CONST UINT  iGroupNameResourceID)
{
   LISTVIEW_GROUP*   pNewGroup;     // Group being created

   // Create object
   pNewGroup = utilCreateEmptyObject(LISTVIEW_GROUP);

   // Set properties
   pNewGroup->iID    = iGroupID;
   pNewGroup->szName = utilLoadString(getResourceInstance(), iGroupNameResourceID, 96);

   /// NOTE: Item count is always set to ZERO

   // Return new object
   return pNewGroup;
}



/// Function name  : createGroupedListViewItem
// Description     : Creates an empty GroupedListViewItem specially for requesting item data
// 
// CONST UINT  iPhysicalIndex : [in] Zero-based physical item index
// CONST UINT  iBufferLength  : [in] Length of the text buffer it will contain
// 
// Return Value   : New LISTVIEW_ITEM, you are responsible for destroying it
// 
LISTVIEW_ITEM*  createGroupedListViewItem(CONST UINT  iPhysicalIndex, CONST UINT  iBufferLength)
{
   LISTVIEW_ITEM*   pListViewItem;       // Object being created

   // Create object
   pListViewItem = utilCreateEmptyObject(LISTVIEW_ITEM);

   // Create text buffer
   pListViewItem->szText         = utilCreateEmptyString(iBufferLength);
   pListViewItem->iTextMax       = iBufferLength;

   // Set properties
   pListViewItem->iPhysicalIndex = iPhysicalIndex;
   pListViewItem->iMask          = (LVIF_TEXT WITH LVIF_IMAGE WITH LVIF_STATE);

   // Return object
   return pListViewItem;
}



/// Function name  : deleteGroupedListViewData
// Description     : Destroys the data for the GroupedListView
// 
// GROUPED_LISTVIEW_DATA*  &pWindowData : [in] GroupedListView data to destroy
// 
VOID  deleteGroupedListViewData(GROUPED_LISTVIEW_DATA*  &pWindowData)
{
   // Destroy contents
   deleteList(pWindowData->pGroupList);

   // Destroy calling object
   utilDeleteObject(pWindowData);
}


/// Function name  : deleteGroupedListViewGroup
// Description     : Destroys the data for the GroupedListView group
// 
// LISTVIEW_GROUP*  &pNewGroup : [in] GroupedListView group to destroy
// 
VOID  deleteGroupedListViewGroup(LISTVIEW_GROUP*  &pNewGroup)
{
   // Destroy contents
   utilDeleteString(pNewGroup->szName);

   // Destroy calling object
   utilDeleteObject(pNewGroup);
}


/// Function name  : destructorGroupedListViewGroup
// Description     : Destroys a GroupedListView group stored in a list
// 
// LPARAM  pListViewGroup : [in] GroupedListView group
// 
VOID  destructorGroupedListViewGroup(LPARAM  pListViewGroup)
{
   // Destroy group data
   deleteGroupedListViewGroup((LISTVIEW_GROUP*&)pListViewGroup);
}


/// Function name  : deleteGroupedListViewItem
// Description     : Destroys the data for the GroupedListViewItem
// 
// LISTVIEW_ITEM*  &pListViewItem : [in] GroupedListViewItem to destroy
// 
VOID  deleteGroupedListViewItem(LISTVIEW_ITEM*  &pListViewItem)
{
   // Destroy text buffer
   utilDeleteString(pListViewItem->szText);

   /// Do *NOT* destroy RichText buffer

   // Destroy calling object
   utilDeleteObject(pListViewItem);
}

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                       HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : calculateGroupedListViewPhysicalItemCount
// Description     : Calculates the number of items required to display the items in the currently non-empty groups
// 
// GROUPED_LISTVIEW_DATA*  pWindowData : [in] Window data
// 
// Return Value   : Current physical item count
// 
UINT   calculateGroupedListViewPhysicalItemCount(GROUPED_LISTVIEW_DATA*  pWindowData)
{
   LISTVIEW_GROUP*  pCurrentGroup;     // Group data iterator
   UINT             iCount;            // Operation result

   // Prepare
   iCount = 0;

   /// Iterate through each group
   for (LIST_ITEM*  pIterator = getListHead(pWindowData->pGroupList); pIterator; pIterator = pIterator->pNext)
   {
      // Prepare
      pCurrentGroup = extractListItemPointer(pIterator, LISTVIEW_GROUP);

      // Ensure group is not empty (ie. not visible)
      if (pCurrentGroup->iCount > 0)
         /// Allocate three items per group header
         iCount += (pCurrentGroup->iCount + 3);
   }

   /// Return physical count
   return iCount;
}


/// Function name  : findGroupedListViewGroupByID
// Description     : Lookup the data for a group by it's unique ID
// 
// GROUPED_LISTVIEW_DATA*  pWindowData : [in] Window data
// CONST UINT              iID         : [in] ID of the group to search for
// LISTVIEW_GROUP*        &pOutput     : [out] Desired group if found, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BOOL  findGroupedListViewGroupByID(GROUPED_LISTVIEW_DATA*  pWindowData, CONST UINT  iID, LISTVIEW_GROUP*  &pOutput)
{
   LISTVIEW_GROUP*  pCurrentGroup;     // Group iterator

   // Prepare
   pOutput = NULL;

   // Iterate through list
   for (LIST_ITEM*  pIterator = getListHead(pWindowData->pGroupList); pIterator; pIterator = pIterator->pNext)
   {
      // Extract group
      pCurrentGroup = extractListItemPointer(pIterator, LISTVIEW_GROUP);

      // Check whether the IDs match
      if (pCurrentGroup->iID == iID)
      {
         // [SUCCESS] Set result and abort
         pOutput = pCurrentGroup;
         break;
      }
   }  

   // Return result
   return (pOutput != NULL);
}



/// Function name  : getGroupedListViewBaseWindowProc
// Description     : Retrieve the address of the window procedure for the ListView base
// 
// HWND  hCtrl : [in] Grouped ListView window handle
// 
// Return Value   : Window procedure of the system ListView
// 
WNDPROC  getGroupedListViewBaseWindowProc(HWND  hCtrl)
{
   WNDCLASS   oBaseClass;

   // Lookup window class
   GetClassInfo(NULL, WC_LISTVIEW, &oBaseClass);

   // Return window proc
   return oBaseClass.lpfnWndProc;
}


/// Function name  : getGroupedListViewData
// Description     : Retrieve window data for the Grouped ListView
// 
// HWND  hCtrl : [in] Grouped ListView window handle
// 
// Return type : Grouped ListView window data
//
GROUPED_LISTVIEW_DATA*  getGroupedListViewData(HWND  hCtrl)
{
   // Retrieve window data from the second 32-bit value
   return (GROUPED_LISTVIEW_DATA*)GetWindowLong(hCtrl, 4);
}

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : calculateGroupedListViewLogicalIndex
// Description     : Retrieves the logical index, item type and group of the item at the specified physical index
// 
// GROUPED_LISTVIEW_DATA*  pWindowData   : [in]  Window data
// LISTVIEW_ITEM*          pItem         : [in]  Item containing the physical index to lookup
///                                        [out] Logical index, Group data, Item Type
// 
// Return Value   : TRUE if successful, FALSE if the physical index was invalid
// 
BOOL   calculateGroupedListViewLogicalIndex(GROUPED_LISTVIEW_DATA*  pWindowData, LISTVIEW_ITEM*  pItem)
{
   LISTVIEW_GROUP*  pCurrentGroup;                    // Group data iterator
   UINT             iPhysicalGroupBoundaryIndex,      // Physical index of the first item in the current group
                    iPhysicalItemIndexWithinGroup,    // Physical index of the target item WITHIN THE CURRENT GROUP  (may exceed the group size, thereby indicating it's not actually in the current group)
                    iPhysicalGroupSize;               // Number of physical items required to display the current group

   // Prepare
   iPhysicalGroupBoundaryIndex = 0;
   pItem->iLogicalIndex        = 0;
   pItem->iGroupPhysicalIndex  = 0;

   /// Iterate through groups
   for (LIST_ITEM*  pGroupIterator = getListHead(pWindowData->pGroupList); pGroupIterator; pGroupIterator = pGroupIterator->pNext)
   {
      // Prepare
      pCurrentGroup = extractListItemPointer(pGroupIterator, LISTVIEW_GROUP);

      // Skip empty groups
      if (pCurrentGroup->iCount > 0)
      {
         // Calculate number of physical items in the group
         iPhysicalGroupSize = (pCurrentGroup->iCount + 3);

         /// Calculate what the physical index of the target index would be, if it were contained in the current group
         iPhysicalItemIndexWithinGroup = (pItem->iPhysicalIndex - iPhysicalGroupBoundaryIndex);

         // Check whether target index is within the current group
         if (iPhysicalItemIndexWithinGroup < iPhysicalGroupSize)
         {
            // Store physical index of group
            pItem->iGroupPhysicalIndex = iPhysicalGroupBoundaryIndex;

            /// Determine item type from the physical index, then calculate the logical index
            switch (iPhysicalItemIndexWithinGroup)
            {
            // [HEADER SPACING] Define item type and set logical index to NULL
            case 0:
            case 2:  
               pItem->eType         = GLVIT_BLANK;      
               pItem->iLogicalIndex = (UINT)-1;
               break;

            // [HEADER TITLE] Define item type and set logical index to NULL
            case 1:
               pItem->eType         = GLVIT_GROUPHEADER;      
               pItem->iLogicalIndex = (UINT)-1;
               break;

            // [ITEM] Define item type and logical index
            default:
               pItem->eType         = GLVIT_ITEM;      
               pItem->iLogicalIndex += (iPhysicalItemIndexWithinGroup - 3);
               break;
            }

            /// [SUCCESS] Store Group and Return TRUE
            pItem->pGroup = pCurrentGroup;
            return TRUE;
         }

         // Calculate initial index for the next group
         iPhysicalGroupBoundaryIndex += iPhysicalGroupSize;

         // Re-calculate the 'running total' of logical items encountered thus far, so that the final logical index can be determined
         pItem->iLogicalIndex += pCurrentGroup->iCount;
      }
   }

   /// [ITEM NOT FOUND] Set logical index to -1 and Return FALSE
   pItem->iLogicalIndex = (UINT)-1;
   return FALSE;
}


/// Function name  : convertGroupedListViewPhysicalIndex
// Description     : Calculates the logical index of an ITEM within a GroupedListView. If the item is a heading or a spacer, it returns -1.
// 
// HWND        hCtrl          : [in] GroupedListView control window handle
// CONST UINT  iPhysicalIndex : [in] Physical index
// 
// Return Value   : Logical index for items, -1 for headings and spacers
// 
ControlsAPI
INT   convertGroupedListViewPhysicalIndex(HWND  hCtrl, CONST UINT  iPhysicalIndex)
{
   GROUPED_LISTVIEW_DATA*   pWindowData;     // Window data
   LISTVIEW_ITEM            oItem;           // Item data

   // Prepare
   utilZeroObject(&oItem, LISTVIEW_ITEM);

   // Set indicies
   oItem.iPhysicalIndex = iPhysicalIndex;
   oItem.iLogicalIndex  = (UINT)-1;
   
   /// [CHECK] Lookup window data
   if ((iPhysicalIndex != -1) AND (pWindowData = getGroupedListViewData(hCtrl)))
      /// Convert index
      calculateGroupedListViewLogicalIndex(pWindowData, &oItem);
   
   // Return logical index, if any
   return (INT)oItem.iLogicalIndex;
}


/// Function name  : drawGroupedListViewGradientLine
// Description     : Draws an line beneath a Grouped ListView heading using a colour gradient. 
// 
// HDC          hDC       : [in] ListView device Context
// CONST RECT*  pItemRect : [in] Bounding rectangle of the item. The line will be drawn at the bottom of this rectangle
// 
// Return type : TRUE if succesful, FALSE otherwise 
//
BOOL   drawGroupedListViewGradientLine(HDC  hDC, CONST RECT*  pItemRect)
{
   TRIVERTEX        oVertex[2] ;
   GRADIENT_RECT    oGradientRect;

   // Shamelessly stolen from MSDN...
   //
   oVertex[0].x      = pItemRect->left;
   oVertex[0].y      = pItemRect->bottom - 2;
   oVertex[0].Red    = 0x3200;
   oVertex[0].Green  = 0x9700;
   oVertex[0].Blue   = 0xff00;
   oVertex[0].Alpha  = 0xff00;

   oVertex[1].x      = pItemRect->left + (pItemRect->right - pItemRect->left) / 2;
   oVertex[1].y      = pItemRect->bottom; 
   oVertex[1].Red    = GetRValue(GetSysColor(COLOR_WINDOW)) << 8;
   oVertex[1].Green  = GetGValue(GetSysColor(COLOR_WINDOW)) << 8;
   oVertex[1].Blue   = GetBValue(GetSysColor(COLOR_WINDOW)) << 8;
   oVertex[1].Alpha  = 0x0000;

   oGradientRect.UpperLeft  = 0;
   oGradientRect.LowerRight = 1;

   return GradientFill(hDC, oVertex, 2, &oGradientRect, 1, GRADIENT_FILL_RECT_H);   
}


/// ////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE  HANDLERS
/// ////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onGroupedListViewAddGroup
// Description     : Adds a new group to the ListView
// 
// GROUPED_LISTVIEW_DATA*  pWindowData : [in] Window data
// LISTVIEW_GROUP*         pNewGroup   : [in] Group data
///                                       This is now owned by the GroupedListView and must not be destroyed by the caller!
// 
// Return Value   : TRUE if the group was added successfully, FALSE if the ID conflicted with an existing group
// 
BOOL   onGroupedListViewAddGroup(GROUPED_LISTVIEW_DATA*  pWindowData, LISTVIEW_GROUP*  pNewGroup)
{
   LISTVIEW_GROUP*  pConflictingGroup;    // Group with a conflicting ID, if any

   // [CHECK] Return FALSE if group ID is not unique
   if (findGroupedListViewGroupByID(pWindowData, pNewGroup->iID, pConflictingGroup))
      return FALSE;

   // Add group to group list
   appendItemToList(pWindowData->pGroupList, createListItem((LPARAM)pNewGroup));

   // [SUCCESS] Return TRUE
   return TRUE;
}


/// Function name  : onGroupedListViewCreate
// Description     : Creates and initialises the window data for the control
// 
// HWND  hCtrl   : [in] ListView window handle
// 
// Return Value : New window data, you are responsible for destroying it
//
GROUPED_LISTVIEW_DATA*  onGroupedListViewCreate(HWND  hCtrl)
{
   GROUPED_LISTVIEW_DATA*  pWindowData;         // New window data
   HDC                     hDC;                 // ListView DC

   // Prepare
   hDC = GetDC(hCtrl);

   /// Create window data
   pWindowData = createGroupedListViewData();

   /// Store window handle
   pWindowData->hCtrl = hCtrl;
   SetWindowLong(hCtrl, 4, (LONG)pWindowData);

   // Store base window procedure
   pWindowData->pfnBaseWindowProc = getGroupedListViewBaseWindowProc(hCtrl);

   /// Create a bold version of the window font for drawing header text
   SelectObject(hDC, GetStockObject(ANSI_VAR_FONT));
   pWindowData->hHeaderFont = utilDuplicateFont(hDC, TRUE, FALSE, FALSE);

   // Cleanup and return window data
   ReleaseDC(hCtrl, hDC);
   return pWindowData;
}


/// Function name  : onGroupedListViewCustomDraw
// Description     : Handles the NM_CUSTOMDRAW notification for a GroupedListView parent window and sets the appropriate message result flags
// 
// HWND             hParentWnd   : [in] Parent window of the GroupedListView that received the NM_CUSTOMDRAW notification
// HWND             hCtrl        : [in] Grouped ListView window handle
// NMLVCUSTOMDRAW*  pMessageData : [in] NM_CUSTOMDRAW message data
// 
ControlsAPI 
BOOL   onGroupedListViewCustomDraw(HWND  hParentWnd, HWND  hCtrl, NMLVCUSTOMDRAW*  pMessageData)
{
   GROUPED_LISTVIEW_DATA*  pWindowData;      // Grouped ListView window data
   NMCUSTOMDRAW*           pDrawingData;     // Convenience pointer
   DC_STATE*               pPrevState;       // DC State
   LISTVIEW_ITEM*          pItemData;        // GroupedListView item data
   RECT                    rcHeading;        // Heading text rectangle
   BOOL                    bResult;

   // Prepare
   pWindowData  = getGroupedListViewData(hCtrl);
   pDrawingData = &pMessageData->nmcd;
   bResult = TRUE;

   /// Examine current draw stage
   switch (pDrawingData->dwDrawStage)
   {
   /// [CDDS_PREPAINT] Request Custom Draw
   case CDDS_PREPAINT:
      SetWindowLong(hParentWnd, DWL_MSGRESULT, CDRF_NOTIFYITEMDRAW WITH CDRF_NOTIFYPOSTPAINT);
      break;

   /// [POST-PAINT] Skip focus rectangle
   case CDDS_POSTPAINT:
   case CDDS_ITEMPOSTPAINT:
      SetWindowLong(hParentWnd, DWL_MSGRESULT, CDRF_SKIPPOSTPAINT);
      break;

   /// [CDDS_ITEMPREPAINT] Draw header, a gap, or an item
   case CDDS_ITEMPREPAINT:   
      // Prepare
      pItemData  = createGroupedListViewItem(pDrawingData->dwItemSpec, 512);
      pPrevState = utilCreateDeviceContextState(pDrawingData->hdc);

      // [CHECK] Never draw the item beneath the header (this only happens when dragging headers)
      if (pItemData->iPhysicalIndex != ListView_GetTopIndex(hCtrl) - 1)
      {
         /// Attempt to convert physical item index into a logical one
         if (calculateGroupedListViewLogicalIndex(pWindowData, pItemData))
         {
            // Retrieve bounding rectangle
            ListView_GetItemRect(pWindowData->hCtrl, pItemData->iPhysicalIndex, &pItemData->rcRect, LVIR_BOUNDS);

            // Examine item type
            switch (pItemData->eType)
            {
            /// [HEADER GAP] - Draw a blank item
            case GLVIT_BLANK:
               FillRect(pPrevState->hDC, &pItemData->rcRect, GetSysColorBrush(COLOR_WINDOW));
               break;

            /// [HEADER] - Draw a bold header with a gradient underline
            case GLVIT_GROUPHEADER:
               // Switch to a bold font
               utilSetDeviceContextFont(pPrevState, pWindowData->hHeaderFont, GetSysColor(COLOR_WINDOWTEXT));
               
               // [CALC]
               rcHeading       = pItemData->rcRect;
               rcHeading.left += (2 * GetSystemMetrics(SM_CXEDGE));

               // Draw group name in bold with gradient line beneath
               DrawText(pPrevState->hDC, pItemData->pGroup->szName, lstrlen(pItemData->pGroup->szName), &rcHeading, DT_LEFT WITH DT_SINGLELINE WITH DT_WORD_ELLIPSIS);
               drawGroupedListViewGradientLine(pPrevState->hDC, &pItemData->rcRect);
               break;

            /// [ITEM] - Request data from parent and draw item
            case GLVIT_ITEM:
               //VERBOSE("Drawing item: Item=%u, SubItem=%u, State=%u", pItemData->iPhysicalIndex, pMessageData->iSubItem, pMessageData->nmcd.uItemState);
               drawCustomListViewItem(hParentWnd, hCtrl, pMessageData);
               break;
            }
         }
      }
      
      // Cleanup and Notify system we've performed drawing
      utilDeleteDeviceContextState(pPrevState);
      deleteGroupedListViewItem(pItemData);
      SetWindowLong(hParentWnd, DWL_MSGRESULT, CDRF_SKIPDEFAULT);
      break;

   default:
      bResult = FALSE;
      break;
   }

   return bResult;
}


/// Function name  : onGroupedListViewCustomDrawTooltip
// Description     : Requests CustomTooltip data from the parent window, then performs tooltip CustomDraw
// 
// GROUPED_LISTVIEW_DATA*  pWindowData : [in] Window data
// HWND                    hTooltip    : [in] Tooltip
// NMHDR*                  pHeader     : [in] Tooltip CustomDraw header
// 
// Return Value   : CustomDraw result flag if handled, otherwise FALSE
// 
BOOL  onGroupedListViewCustomDrawTooltip(GROUPED_LISTVIEW_DATA*  pWindowData, HWND  hTooltip, NMHDR*  pHeader)
{
   NMLVGETINFOTIP  oDataRequest;    // Custom data request
   LVHITTESTINFO   oHitTest;        // ListView hit test
   LISTVIEW_ITEM   oItem;           // Used for converting physical index -> logical index
   BOOL            iResultCode;

   // Prepare
   utilZeroObject(&oItem, LISTVIEW_ITEM);
   utilZeroObject(&oDataRequest, NMLVGETINFOTIP);

   /// Perform ListView hit-test
   utilGetWindowCursorPos(pWindowData->hCtrl, &oHitTest.pt);
   oItem.iPhysicalIndex = ListView_HitTest(pWindowData->hCtrl, &oHitTest);
   
   /// Convert index from physical -> logical 
   if (oItem.iPhysicalIndex != -1 AND calculateGroupedListViewLogicalIndex(pWindowData, &oItem))
   {
      // Setup RichTooltip data request
      oDataRequest.hdr.code     = LVN_GETINFOTIP;
      oDataRequest.hdr.hwndFrom = hTooltip;
      oDataRequest.hdr.idFrom   = GetWindowID(pWindowData->hCtrl);
      oDataRequest.dwFlags      = LVGIT_CUSTOM;
      oDataRequest.iItem        = oItem.iLogicalIndex;

      /// Request data from parent
      if (SendMessage(GetParent(pWindowData->hCtrl), WM_NOTIFY, oDataRequest.hdr.idFrom, (LPARAM)&oDataRequest))
      {
         /// [SUCCESS] Custom Draw Tooltip 
         iResultCode = onCustomDrawTooltip(hTooltip, (NMTTCUSTOMDRAW*)pHeader, (CUSTOM_TOOLTIP*)oDataRequest.lParam);

         // Cleanup and return result code
         deleteCustomTooltipData((CUSTOM_TOOLTIP*&)oDataRequest.lParam);
         return iResultCode;
      }
   }

   // [FAILED] Invalid index
   return FALSE;
}


/// Function name  : onGroupedListViewDestroy
// Description     : Delete associated window data
// 
// GROUPED_LISTVIEW_DATA*  &pWindowData : [in] Grouped ListView window data
// 
VOID   onGroupedListViewDestroy(GROUPED_LISTVIEW_DATA*  &pWindowData)
{
   // Delete header text font
   DeleteObject(pWindowData->hHeaderFont);

   // Sever window data
   SetWindowLong(pWindowData->hCtrl, 4, NULL);

   // Free window data
   deleteGroupedListViewData(pWindowData);
}



/// Function name  : onGroupedListViewEmptyGroups
// Description     : Empties all the groups in the GroupedListView
// 
// GROUPED_LISTVIEW_DATA*  pWindowData : [in] Window data
// 
VOID   onGroupedListViewEmptyGroups(GROUPED_LISTVIEW_DATA*  pWindowData)
{
   LISTVIEW_GROUP*  pCurrentGroup;     // Group iterator

   // Iterate through all groups
   for (LIST_ITEM*  pIterator = getListHead(pWindowData->pGroupList); pCurrentGroup = extractListItemPointer(pIterator, LISTVIEW_GROUP); pIterator = pIterator->pNext)
      /// Set GroupCount to zero
      pCurrentGroup->iCount = 0;

   /// Set ListView count to zero
   CallWindowProc(getGroupedListViewBaseWindowProc(pWindowData->hCtrl), pWindowData->hCtrl, LVM_SETITEMCOUNT, 0, NULL);   // Send message directly to the base window procedure (it's blocked by our implementation)
}


/// Function name  : onGroupedListViewRemoveGroups
// Description     : Removes all groups from the ListView
// 
// GROUPED_LISTVIEW_DATA*  pWindowData : [in] Window data
// 
VOID   onGroupedListViewRemoveGroups(GROUPED_LISTVIEW_DATA*  pWindowData)
{
   /// Re-create groups list
   deleteList(pWindowData->pGroupList);
   pWindowData->pGroupList = createList(destructorGroupedListViewGroup);
}


/// Function name  : onGroupedListViewSetGroupCount
// Description     : Defines the item count for a specified group
// 
// GROUPED_LISTVIEW_DATA*  pWindowData : [in] Window data
// CONST UINT              iGroupID    : [in] ID of the group
// CONST UINT              iCount      : [in] Number of items in the group
// 
// Return Value   : TRUE if successful, FALSE if the group does not exist
// 
BOOL   onGroupedListViewSetGroupCount(GROUPED_LISTVIEW_DATA*  pWindowData, CONST UINT  iGroupID, CONST UINT  iCount)
{
   LISTVIEW_GROUP*  pGroup;              // Group to be modified
   UINT             iPhysicalItemCount;  // New physical item count

   // Lookup group
   if (!findGroupedListViewGroupByID(pWindowData, iGroupID, pGroup))
      // [GROUP NOT FOUND] Return FALSE
      return FALSE;

   // Set new count
   pGroup->iCount = iCount;

   // Calculate new physical item count
   iPhysicalItemCount = calculateGroupedListViewPhysicalItemCount(pWindowData);
   
   // Send message directly to the base window procedure (it's blocked by our implementation)
   CallWindowProc(getGroupedListViewBaseWindowProc(pWindowData->hCtrl), pWindowData->hCtrl, LVM_SETITEMCOUNT, iPhysicalItemCount, NULL);   

   // [SUCCESS] Return TRUE
   return TRUE;
}



/// Function name  : onGroupedListViewSetItemCount
// Description     : Increase a ListView item count to account for the lines containing group headers
// 
// GROUPED_LISTVIEW_DATA*  pWindowData : [in] Grouped ListView window data
// CONST UINT              iItemCount  : [in] Item count
// 
// Return type : New item count
//
UINT   onGroupedListViewSetItemCount(GROUPED_LISTVIEW_DATA*  pWindowData, CONST UINT  iItemCount)
{
   ASSERT(FALSE);
   // Use 3 lines per group header
   return iItemCount + (pWindowData->iCount * 3);
}

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                  WINDOW PROCEDURE
/// ////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : wndprocGroupedListView
// Description     : Grouped ListView window procedure
//
//
LRESULT   wndprocGroupedListView(HWND  hCtrl, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   GROUPED_LISTVIEW_DATA*  pWindowData; 
   WNDCLASS                oBaseClass;
   NMHDR*                  pHeader;
   /*BOOL                    bPassToBase;
   LRESULT                 iResult;*/
   
   // Get window data
   pWindowData = getGroupedListViewData(hCtrl);
   /*bPassToBase = TRUE;
   iResult     = 0;*/

   // Lookup base window class
   GetClassInfo(NULL, WC_LISTVIEW, &oBaseClass);

   // Examine message
   switch (iMessage)
   {
   /// [CREATE] -- Create window data
   case WM_CREATE:
      // Create window data
      pWindowData = onGroupedListViewCreate(hCtrl);
      // Pass to base window proc
      break;   

   /// [DESTROY] -- Destroy window data
   case WM_DESTROY:
      onGroupedListViewDestroy(pWindowData);
      // Pass to base window proc
      break;

   /// [NOTIFICATION]
   case WM_NOTIFY:
      // Prepare
      pHeader = (NMHDR*)lParam;

      // [CHECK] Ensure CustomDraw is not from the ListView header
      if (pHeader->code == NM_CUSTOMDRAW AND pHeader->hwndFrom == ListView_GetToolTips(pWindowData->hCtrl))
         // [CUSTOM DRAW TOOLTIP]
         return onGroupedListViewCustomDrawTooltip(pWindowData, pHeader->hwndFrom, pHeader);

      //else if (pHeader->code == LVN_ITEMCHANGED)
      break;

   // [ADD GROUP] - Defines a new group
   case UM_ADD_LISTVIEW_GROUP:
      return onGroupedListViewAddGroup(pWindowData, (LISTVIEW_GROUP*)lParam);

   // [EMPTY GROUPS] Reset count of all groups to zero
   case UM_EMPTY_LISTVIEW_GROUPS:
      onGroupedListViewEmptyGroups(pWindowData);
      return 0;

   // [REMOVE GROUPS] - Deletes all groups
   case UM_REMOVE_LISTVIEW_GROUPS:
      onGroupedListViewRemoveGroups(pWindowData);
      return 0;

   // [SET GROUP COUNT] - Defines the number of items in a group
   case UM_SET_LISTVIEW_GROUP_COUNT:
      return onGroupedListViewSetGroupCount(pWindowData, wParam, lParam);

   // [SET ITEM COUNT] -- Alter item count to account for group headings, then pass to ListView base
   case LVM_SETITEMCOUNT:
      ASSERT(FALSE);
      wParam = onGroupedListViewSetItemCount(pWindowData, wParam);
      break;
   }

   // Pass to base
   return CallWindowProc(oBaseClass.lpfnWndProc, hCtrl, iMessage, wParam, lParam);
}

