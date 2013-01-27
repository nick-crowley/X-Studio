//
// Group ListView Control.cpp : Virtual grouped ListView control. All messages to the control should use logical item indicies.
//                               All notifications from the control use physical item indiciates, convertable using via GroupedListView_PhysicalToLogical(..)
//                               The only exception to this is LVN_GETDISPINFO during CustomDraw.
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                   DECLARATIONS
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
GROUPED_LISTVIEW*  createWindowData();
VOID               deleteWindowData(GROUPED_LISTVIEW*  &pWindowData);
VOID               deleteGroup(LISTVIEW_GROUP*  &pNewGroup);
VOID               destructorGroup(LPARAM  pListViewGroup);

// Helpers
WNDPROC            getBaseWindowProc(HWND  hCtrl);
GROUPED_LISTVIEW*  getWindowData(HWND  hCtrl);
LRESULT            sendBaseMessage(HWND  hCtrl, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);

// Functions
UINT               calculateLogicalCount(GROUPED_LISTVIEW*  pWindowData);
BOOL               calculateLogicalIndex(GROUPED_LISTVIEW*  pWindowData, const INT  iPhysicalIndex, LISTVIEW_ITEM*  pOutput);
UINT               calculatePhysicalCount(GROUPED_LISTVIEW*  pWindowData);
INT                calculatePhysicalIndex(GROUPED_LISTVIEW*  pWindowData, const INT  iLogicalIndex);
BOOL               drawHeaderGradient(HDC  hDC, const RECT*  pItemRect, const COLORREF  clForeground, const COLORREF  clBackground);
BOOL               findGroupByID(GROUPED_LISTVIEW*  pWindowData, CONST UINT  iID, LISTVIEW_GROUP*  &pOutput);

// Message Handlers
BOOL               onGroupedListView_AddGroup(GROUPED_LISTVIEW*  pWindowData, LISTVIEW_GROUP*  pNewGroup);
GROUPED_LISTVIEW*  onGroupedListView_Create(HWND  hCtrl);
BOOL               onGroupedListView_CustomDrawTooltip(GROUPED_LISTVIEW*  pWindowData, HWND  hTooltip, NMHDR*  pHeader);
VOID               onGroupedListView_Destroy(GROUPED_LISTVIEW*  &pWindowData);
VOID               onGroupedListView_EmptyGroups(GROUPED_LISTVIEW*  pWindowData);
VOID               onGroupedListView_RemoveGroups(GROUPED_LISTVIEW*  pWindowData);
BOOL               onGroupedListView_SetGroupCount(GROUPED_LISTVIEW*  pWindowData, CONST UINT  iGroupID, CONST UINT  iCount);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                  CONSTANTS / GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////

const COLORREF  clLightBlue = RGB(50,151,255);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                  CREATION / DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createWindowData
// Description     : Creates window data for a GroupedListView
// 
// Return Value   : New window data for GroupedListView, you are responsible for destroying it
// 
GROUPED_LISTVIEW*   createWindowData()
{
   GROUPED_LISTVIEW*   pWindowData;       // Object being created

   // Create object
   pWindowData = utilCreateEmptyObject(GROUPED_LISTVIEW);

   // Set properties
   pWindowData->pGroupList = createList(destructorGroup);

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



/// Function name  : deleteWindowData
// Description     : Destroys the data for the GroupedListView
// 
// GROUPED_LISTVIEW*  &pWindowData : [in] GroupedListView data to destroy
// 
VOID  deleteWindowData(GROUPED_LISTVIEW*  &pWindowData)
{
   // Destroy contents
   deleteList(pWindowData->pGroupList);

   // Destroy calling object
   utilDeleteObject(pWindowData);
}


/// Function name  : deleteGroup
// Description     : Destroys the data for the GroupedListView group
// 
// LISTVIEW_GROUP*  &pNewGroup : [in] GroupedListView group to destroy
// 
VOID  deleteGroup(LISTVIEW_GROUP*  &pNewGroup)
{
   // Destroy contents
   utilDeleteString(pNewGroup->szName);

   // Destroy calling object
   utilDeleteObject(pNewGroup);
}


/// Function name  : destructorGroup
// Description     : Destroys a GroupedListView group stored in a list
// 
// LPARAM  pListViewGroup : [in] GroupedListView group
// 
VOID  destructorGroup(LPARAM  pListViewGroup)
{
   // Destroy group data
   deleteGroup((LISTVIEW_GROUP*&)pListViewGroup);
}


/// ////////////////////////////////////////////////////////////////////////////////////////
///                                       HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : getBaseWindowProc
// Description     : Retrieve the address of the window procedure for the ListView base
// 
// HWND  hCtrl : [in] Grouped ListView window handle
// 
// Return Value   : Window procedure of the system ListView
// 
WNDPROC  getBaseWindowProc(HWND  hCtrl)
{
   return wndprocCustomListView;
}


/// Function name  : getWindowData
// Description     : Retrieve window data for the Grouped ListView
// 
// HWND  hCtrl : [in] Grouped ListView window handle
// 
// Return type : Grouped ListView window data
//
GROUPED_LISTVIEW*  getWindowData(HWND  hCtrl)
{
   // Retrieve window data from the second 32-bit value
   return (GROUPED_LISTVIEW*)GetWindowLong(hCtrl, 4);
}

/// Function name  : sendBaseMessage
// Description     : Sends a message directly to the base window proc, bypassing the conversion performed of ListView messages in the group listview window proc
// 
// Return Value   : Message result
// 
LRESULT  sendBaseMessage(HWND  hCtrl, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   return CallWindowProc(getBaseWindowProc(hCtrl), hCtrl, iMessage, wParam, lParam);
}

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : calculateLogicalCount
// Description     : Returns number of items in each group
// 
// GROUPED_LISTVIEW*  pWindowData : [in] Window data
// 
// Return Value   : Current logical item count
// 
UINT   calculateLogicalCount(GROUPED_LISTVIEW*  pWindowData)
{
   LISTVIEW_GROUP*  pGroup;      // Group data iterator
   UINT             iCount = 0;  // Operation result

   // Iterate through each group
   for (LIST_ITEM*  pIterator = getListHead(pWindowData->pGroupList); pGroup = extractListItemPointer(pIterator, LISTVIEW_GROUP); pIterator = pIterator->pNext)
   {
      // Ensure group is not empty (ie. not visible)
      if (pGroup->iCount > 0)
         iCount += pGroup->iCount;
   }

   // Return logical count
   return iCount;
}


/// Function name  : calculateLogicalIndex
// Description     : Retrieves the logical index, item type and group of the item at the specified physical index
// 
// GROUPED_LISTVIEW*  pWindowData    : [in]     Window data
// const UINT         iPhysicalIndex : [in]     Physical index
// LISTVIEW_ITEM*     pItem          : [in/out] Logical Item data
// 
// Return Value   : TRUE if logical index is an item, FALSE if a header or invalid
// 
BOOL  calculateLogicalIndex(GROUPED_LISTVIEW*  pWindowData, const INT  iPhysicalIndex, LISTVIEW_ITEM*  pOutput)
{
   LISTVIEW_GROUP*  pGroup;                 // Group iterator
   INT              iPhysicalBoundary = 0,  // Physical index of first item in current group
                    iLogicalBoundary = 0;   // Logical index of first item in current group

   // Prepare
   utilZeroObject(pOutput, LISTVIEW_ITEM);
   pOutput->iLogicalIndex = -1;

   // [CHECK] Return FALSE if index is invalid
   if (iPhysicalIndex < 0)
      return FALSE;

   // Iterate through each group
   for (LIST_ITEM*  pIterator = getListHead(pWindowData->pGroupList); pGroup = extractListItemPointer(pIterator, LISTVIEW_GROUP); pIterator = pIterator->pNext)
   {
      // Ensure group is not empty (ie. not visible)
      if (pGroup->iCount > 0)
      {
         // [CHECK] Is input index within this group?
         if (iPhysicalIndex < (iPhysicalBoundary + 3 + (INT)pGroup->iCount))
         {
            /// [SUCCESS] Store current group
            pOutput->pGroup        = pGroup;
            pOutput->iLogicalIndex = -1;

            // Identify item type
            switch (iPhysicalIndex - iPhysicalBoundary)
            {
            case 0:
            case 2:  pOutput->eType = GLVIT_BLANK;         break;
            case 1:  pOutput->eType = GLVIT_GROUPHEADER;   break;  
            default: pOutput->eType = GLVIT_ITEM;          break;
            }

            /// [ITEM] Return: Logical boundary + Logical offset
            if (pOutput->eType == GLVIT_ITEM)
               pOutput->iLogicalIndex = iLogicalBoundary + (iPhysicalIndex - (iPhysicalBoundary + 3));

            /// [SUCCESS] Return TRUE
            return TRUE;
         }

         // Update physical + logical boundary 
         iPhysicalBoundary += pGroup->iCount + 3;
         iLogicalBoundary += pGroup->iCount;
      }
   }

   /// [FAILED] Return FALSE
   return FALSE;
}


/// Function name  : calculatePhysicalCount
// Description     : Returns number of items in the listview
// 
// GROUPED_LISTVIEW*  pWindowData : [in] Window data
// 
// Return Value   : Current physical item count
// 
UINT   calculatePhysicalCount(GROUPED_LISTVIEW*  pWindowData)
{
   LISTVIEW_GROUP*  pGroup;      // Group data iterator
   UINT             iCount = 0;  // Operation result

   // Iterate through each group
   for (LIST_ITEM*  pIterator = getListHead(pWindowData->pGroupList); pGroup = extractListItemPointer(pIterator, LISTVIEW_GROUP); pIterator = pIterator->pNext)
   {
      // Ensure group is not empty (ie. not visible)
      if (pGroup->iCount > 0)
         /// Allocate three items per group header
         iCount += (pGroup->iCount + 3);
   }

   /// Return physical count
   return iCount;
}


/// Function name  : calculatePhysicalIndex
// Description     : Converts Logical -> Physical
// 
// GROUPED_LISTVIEW*  pWindowData   : [in] Window data
// const UINT         iLogicalIndex : [in] Logical index
// 
// Return Value   : Physical index if valid, -1 otherwise
// 
INT  calculatePhysicalIndex(GROUPED_LISTVIEW*  pWindowData, const INT  iLogicalIndex)
{
   LISTVIEW_GROUP*  pGroup;                 // Group iterator
   INT              iPhysicalBoundary = 0,  // Physical index of first item in current group
                    iLogicalBoundary = 0;   // Logical index of first item in current group

   // [CHECK] Return -1 if index is invalid
   if (iLogicalIndex < 0)
      return -1;

   // Iterate through each group
   for (LIST_ITEM*  pIterator = getListHead(pWindowData->pGroupList); pGroup = extractListItemPointer(pIterator, LISTVIEW_GROUP); pIterator = pIterator->pNext)
   {
      // Ensure group is not empty (ie. not visible)
      if (pGroup->iCount > 0)
      {
         // [CHECK] Is input index within this group?
         if (iLogicalIndex < (iLogicalBoundary + (INT)pGroup->iCount))
            /// [SUCCESS] Return: physical boundary + logical offset
            return (iPhysicalBoundary + 3) + (iLogicalIndex - iLogicalBoundary);

         // Update physical + logical boundary 
         iPhysicalBoundary += pGroup->iCount + 3;
         iLogicalBoundary += pGroup->iCount;
      }
   }

   /// [FAILED] Return -1
   return -1;
}


/// Function name  : drawHeaderGradient
// Description     : Draws an line beneath a Grouped ListView heading using a colour gradient. 
// 
// HDC          hDC       : [in] ListView device Context
// CONST RECT*  pItemRect : [in] Bounding rectangle of the item. The line will be drawn at the bottom of this rectangle
// 
// Return type : TRUE if succesful, FALSE otherwise 
//
BOOL   drawHeaderGradient(HDC  hDC, const RECT*  pItemRect, const COLORREF  clForeground, const COLORREF  clBackground)
{
   TRIVERTEX        oVertex[2] ;
   GRADIENT_RECT    oGradientRect;

   // Define left as Light Blue (No transparency)
   oVertex[0].x      = pItemRect->left;
   oVertex[0].y      = pItemRect->bottom - 2;
   oVertex[0].Red    = GetRValue(clForeground) << 8;
   oVertex[0].Green  = GetGValue(clForeground) << 8;      // Light Blue -> RGB(50, 151, 255)
   oVertex[0].Blue   = GetBValue(clForeground) << 8;
   oVertex[0].Alpha  = 0xff00;

   // Define right as background colour  (Full transparency)
   oVertex[1].x      = pItemRect->left + (pItemRect->right - pItemRect->left) / 2;
   oVertex[1].y      = pItemRect->bottom; 
   oVertex[1].Red    = GetRValue(clBackground) << 8;
   oVertex[1].Green  = GetGValue(clBackground) << 8;
   oVertex[1].Blue   = GetBValue(clBackground) << 8;
   oVertex[1].Alpha  = 0x0000;

   // Draw shaded rectangle
   oGradientRect.UpperLeft  = 0;
   oGradientRect.LowerRight = 1;
   return GradientFill(hDC, oVertex, 2, &oGradientRect, 1, GRADIENT_FILL_RECT_H);   
}


/// Function name  : findGroupByID
// Description     : Lookup the data for a group by it's unique ID
// 
// GROUPED_LISTVIEW*  pWindowData : [in] Window data
// CONST UINT         iID         : [in] ID of the group to search for
// LISTVIEW_GROUP*   &pOutput     : [out] Desired group if found, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BOOL  findGroupByID(GROUPED_LISTVIEW*  pWindowData, CONST UINT  iID, LISTVIEW_GROUP*  &pOutput)
{
   // Iterate through list
   for (LIST_ITEM*  pIterator = getListHead(pWindowData->pGroupList); pOutput = extractListItemPointer(pIterator, LISTVIEW_GROUP); pIterator = pIterator->pNext) 
   {
      // Find matching group ID
      if (pOutput->iID == iID)
         break;
   }

   // Return TRUE if found
   return (pOutput != NULL);
}

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE  HANDLERS
/// ////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onCustomDraw_GroupedListView
// Description     : Handles the NM_CUSTOMDRAW notification for a GroupedListView parent window and sets the appropriate message result flags
// 
// HWND             hParentWnd   : [in] Parent window of the GroupedListView that received the NM_CUSTOMDRAW notification
// HWND             hCtrl        : [in] Grouped ListView window handle
// NMLVCUSTOMDRAW*  pMessageData : [in] NM_CUSTOMDRAW message data
// 
ControlsAPI 
BOOL   onCustomDraw_GroupedListView(HWND  hParentWnd, HWND  hCtrl, NMLVCUSTOMDRAW*  pMessageData)
{
   GROUPED_LISTVIEW*  pWindowData;      // Grouped ListView window data
   NMCUSTOMDRAW*      pDrawingData;     // Convenience pointer
   DC_STATE*          pPrevState;       // DC State
   LISTVIEW_ITEM      oItemData;        // GroupedListView item data
   RECT               rcHeading,        // Heading text rectangle
                      rcItem;
   BOOL               bResult,
                      bEnabled;

   // Prepare
   pWindowData  = getWindowData(hCtrl);
   pDrawingData = &pMessageData->nmcd;
   bResult      = CDRF_DODEFAULT;

   /// Examine current draw stage
   switch (pDrawingData->dwDrawStage)
   {
   /// [CDDS_PREPAINT] Request Custom Draw
   case CDDS_PREPAINT:
      bResult = CDRF_NOTIFYITEMDRAW WITH CDRF_NOTIFYPOSTPAINT;
      break;

   /// [POST-PAINT] Skip focus rectangle
   case CDDS_POSTPAINT:
   case CDDS_ITEMPOSTPAINT:
      bResult = DWL_MSGRESULT, CDRF_SKIPPOSTPAINT;
      break;

   /// [CDDS_ITEMPREPAINT] Draw header, a gap, or an item
   case CDDS_ITEMPREPAINT:   
      // [CHECK] ignore items displayed while heading is being dragged
      if (pDrawingData->dwItemSpec != ListView_GetTopIndex(hCtrl) - 1)
      {
         // Prepare
         pPrevState = utilCreateDeviceContextState(pDrawingData->hdc);
         
         /// Lookup logical index 
         if (calculateLogicalIndex(pWindowData, pDrawingData->dwItemSpec, &oItemData))
         {
            // Retrieve bounding rectangle
            rcItem.left = LVIR_BOUNDS;
            sendBaseMessage(pWindowData->hCtrl, LVM_GETITEMRECT, pDrawingData->dwItemSpec, (LPARAM)&rcItem);

            // Examine item type
            switch (oItemData.eType)
            {
            /// [HEADER GAP] - Draw a blank item
            case GLVIT_BLANK:
               FillRect(pPrevState->hDC, &rcItem, GetSysColorBrush(IsWindowEnabled(hCtrl) ? COLOR_WINDOW : COLOR_BTNFACE));
               break;

            /// [HEADER] - Draw a bold header with a gradient underline
            case GLVIT_GROUPHEADER:
               // Prepare
               rcHeading       = rcItem;
               rcHeading.left += (2 * GetSystemMetrics(SM_CXEDGE));
               bEnabled        = IsWindowEnabled(hCtrl);

               // Draw group name in bold 
               utilSetDeviceContextFont(pPrevState, pWindowData->hHeaderFont, GetSysColor(bEnabled ? COLOR_WINDOWTEXT : COLOR_GRAYTEXT));
               DrawText(pPrevState->hDC, oItemData.pGroup->szName, lstrlen(oItemData.pGroup->szName), &rcHeading, DT_LEFT WITH DT_SINGLELINE WITH DT_WORD_ELLIPSIS);

               // Draw blue gradient line beneath
               drawHeaderGradient(pPrevState->hDC, &rcItem, (bEnabled ? clLightBlue : GetSysColor(COLOR_GRAYTEXT)), GetSysColor(bEnabled ? COLOR_WINDOW : COLOR_BTNFACE));
               break;

            /// [ITEM] Draw Item  [Provide Logical index]
            case GLVIT_ITEM:
               pDrawingData->dwItemSpec = oItemData.iLogicalIndex;
               drawCustomListViewItem(hParentWnd, hCtrl, pMessageData);

               //VERBOSE("Drawing item: Item=%u, SubItem=%u, State=%u", pItemData->iPhysicalIndex, pMessageData->iSubItem, pMessageData->nmcd.uItemState);
               break;
            }
         }

         // Cleanup
         utilDeleteDeviceContextState(pPrevState);
      }
      
      // Return 'Draw Nothing'
      bResult = CDRF_SKIPDEFAULT;
      break;
   }

   return bResult;
}


/// Function name  : onGroupedListView_AddGroup
// Description     : Adds a new group to the ListView
// 
// GROUPED_LISTVIEW*  pWindowData : [in] Window data
// LISTVIEW_GROUP*    pNewGroup   : [in] Group data
///                                       This is now owned by the GroupedListView and must not be destroyed by the caller!
// 
// Return Value   : TRUE if the group was added successfully, FALSE if the ID conflicted with an existing group
// 
BOOL   onGroupedListView_AddGroup(GROUPED_LISTVIEW*  pWindowData, LISTVIEW_GROUP*  pNewGroup)
{
   LISTVIEW_GROUP*  pConflictingGroup;    // Group with a conflicting ID, if any

   // [CHECK] Return FALSE if group ID is not unique
   if (findGroupByID(pWindowData, pNewGroup->iID, pConflictingGroup))
      return FALSE;

   // Add group to group list
   appendItemToList(pWindowData->pGroupList, createListItem((LPARAM)pNewGroup));

   // [SUCCESS] Return TRUE
   return TRUE;
}


/// Function name  : onGroupedListView_Create
// Description     : Creates and initialises the window data for the control
// 
// HWND  hCtrl   : [in] ListView window handle
// 
// Return Value : New window data, you are responsible for destroying it
//
GROUPED_LISTVIEW*  onGroupedListView_Create(HWND  hCtrl)
{
   GROUPED_LISTVIEW*  pWindowData = createWindowData();   // New window data
   HDC                hDC         = GetDC(hCtrl);         // ListView DC

   /// Store window handle
   pWindowData->hCtrl = hCtrl;
   SetWindowLong(hCtrl, 4, (LONG)pWindowData);

   // Store base window procedure
   pWindowData->pfnBaseWindowProc = getBaseWindowProc(hCtrl);

   /// Create a bold version of the window font for drawing header text
   SelectObject(hDC, GetStockObject(ANSI_VAR_FONT));
   pWindowData->hHeaderFont = utilDuplicateFont(hDC, TRUE, FALSE, FALSE);

   // Cleanup and return window data
   ReleaseDC(hCtrl, hDC);
   return pWindowData;
}


/// Function name  : onGroupedListView_CustomDrawTooltip
// Description     : Requests CustomTooltip data from the parent window, then performs tooltip CustomDraw
// 
// GROUPED_LISTVIEW*  pWindowData : [in] Window data
// HWND               hTooltip    : [in] Tooltip
// NMHDR*             pHeader     : [in] Tooltip CustomDraw header
// 
// Return Value   : CustomDraw result flag if handled, otherwise FALSE
// 
BOOL  onGroupedListView_CustomDrawTooltip(GROUPED_LISTVIEW*  pWindowData, HWND  hTooltip, NMHDR*  pHeader)
{
   NMLVGETINFOTIP  oDataRequest;    // Custom data request
   LVHITTESTINFO   oHitTest;        // ListView hit test
   LISTVIEW_ITEM   oItem;
   BOOL            iResultCode;

   // Prepare
   utilZeroObject(&oDataRequest, NMLVGETINFOTIP);
   utilGetWindowCursorPos(pWindowData->hCtrl, &oHitTest.pt);
   
   /// Calculate logical index
   if (calculateLogicalIndex(pWindowData, ListView_HitTest(pWindowData->hCtrl, &oHitTest), &oItem) AND oItem.eType == GLVIT_ITEM)
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


/// Function name  : onGroupedListView_Destroy
// Description     : Delete associated window data
// 
// GROUPED_LISTVIEW*  &pWindowData : [in] Grouped ListView window data
// 
VOID   onGroupedListView_Destroy(GROUPED_LISTVIEW*  &pWindowData)
{
   // Delete header text font
   DeleteObject(pWindowData->hHeaderFont);

   // Sever window data
   SetWindowLong(pWindowData->hCtrl, 4, NULL);

   // Free window data
   deleteWindowData(pWindowData);
}


/// Function name  : onGroupedListView_EmptyGroups
// Description     : Empties all the groups in the GroupedListView
// 
// GROUPED_LISTVIEW*  pWindowData : [in] Window data
// 
VOID   onGroupedListView_EmptyGroups(GROUPED_LISTVIEW*  pWindowData)
{
   LISTVIEW_GROUP*  pCurrentGroup;     // Group iterator

   // Iterate through all groups
   for (LIST_ITEM*  pIterator = getListHead(pWindowData->pGroupList); pCurrentGroup = extractListItemPointer(pIterator, LISTVIEW_GROUP); pIterator = pIterator->pNext)
      /// Set GroupCount to zero
      pCurrentGroup->iCount = 0;

   /// Set ListView count to zero
   sendBaseMessage(pWindowData->hCtrl, LVM_SETITEMCOUNT, 0, NULL);   // Send message directly to the base window procedure (it's blocked by our implementation)
}


/// Function name  : onGroupedListView_RemoveGroups
// Description     : Removes all groups from the ListView
// 
// GROUPED_LISTVIEW*  pWindowData : [in] Window data
// 
VOID   onGroupedListView_RemoveGroups(GROUPED_LISTVIEW*  pWindowData)
{
   /// Re-create groups list
   deleteList(pWindowData->pGroupList);
   pWindowData->pGroupList = createList(destructorGroup);
}


/// Function name  : onGroupedListView_SetGroupCount
// Description     : Defines the item count for a specified group
// 
// GROUPED_LISTVIEW*  pWindowData : [in] Window data
// CONST UINT         iGroupID    : [in] ID of the group
// CONST UINT         iCount      : [in] Number of items in the group
// 
// Return Value   : TRUE if successful, FALSE if the group does not exist
// 
BOOL   onGroupedListView_SetGroupCount(GROUPED_LISTVIEW*  pWindowData, CONST UINT  iGroupID, CONST UINT  iCount)
{
   LISTVIEW_GROUP*  pGroup;      // Group to be modified

   // Lookup group
   if (!findGroupByID(pWindowData, iGroupID, pGroup))
      // [GROUP NOT FOUND] Return FALSE
      return FALSE;

   // Set new count
   pGroup->iCount = iCount;

   // Calculate new physical item count
   return sendBaseMessage(pWindowData->hCtrl, LVM_SETITEMCOUNT, calculatePhysicalCount(pWindowData), NULL);    // Send directly to base proc to prevent blocking
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
   GROUPED_LISTVIEW*  pWindowData = getWindowData(hCtrl); 
   LISTVIEW_ITEM      oItem;
   LVHITTESTINFO*     pHitTest;
   NMHDR*             pHeader;

   // Examine message
   switch (iMessage)
   {
   /// [CREATE] -- Create window data
   case WM_CREATE:
      pWindowData = onGroupedListView_Create(hCtrl);  // Pass to base window proc
      break;   

   /// [DESTROY] -- Destroy window data
   case WM_DESTROY:
      onGroupedListView_Destroy(pWindowData);   // Pass to base window proc
      break;

   /// [NOTIFICATION]
   case WM_NOTIFY:
      // Prepare
      pHeader = (NMHDR*)lParam;

      // [CUSTOM DRAW TOOLTIP] Ensure it's not from the header control
      if (pHeader->code == NM_CUSTOMDRAW AND pHeader->hwndFrom == ListView_GetToolTips(pWindowData->hCtrl))
         return onGroupedListView_CustomDrawTooltip(pWindowData, pHeader->hwndFrom, pHeader);
      break;

   // [CUSTOM MESSAGES]
   // -----------------

   /// [ADD GROUP] - Defines a new group
   case UM_ADD_LISTVIEW_GROUP:
      return onGroupedListView_AddGroup(pWindowData, (LISTVIEW_GROUP*)lParam);

   /// [EMPTY GROUPS] Reset count of all groups to zero
   case UM_EMPTY_LISTVIEW_GROUPS:
      onGroupedListView_EmptyGroups(pWindowData);
      return 0;

   /// [PHYSICAL -> LOGICAL]
   case UM_PHYSICAL_TO_LOGICAL:
      calculateLogicalIndex(pWindowData, wParam, &oItem);
      return oItem.iLogicalIndex;

   /// [REMOVE GROUPS] - Deletes all groups
   case UM_REMOVE_LISTVIEW_GROUPS:
      onGroupedListView_RemoveGroups(pWindowData);
      return 0;

   /// [SET GROUP COUNT] - Defines the number of items in a group
   case UM_SET_LISTVIEW_GROUP_COUNT:
      return onGroupedListView_SetGroupCount(pWindowData, wParam, lParam);


   // [LISTVIEW MESSAGES]
   // -------------------

   /// [GET ITEM COUNT] Return logical item count
   case LVM_GETITEMCOUNT:
      return calculateLogicalCount(pWindowData);

   /// [SET ITEM COUNT] Block 
   case LVM_SETITEMCOUNT:
      return 0;

   /// [STATE/RECT/VISIBILITY] Pass to base: Convert logical -> physical
   case LVM_ENSUREVISIBLE:
   case LVM_GETITEMRECT:
   case LVM_GETSUBITEMRECT:
   case LVM_GETITEMSTATE:
   case LVM_SETITEMSTATE:
      wParam = calculatePhysicalIndex(pWindowData, wParam);
      break;   // Base to base

   /// [GET NEXT ITEM] Pass to base: Convert input logical -> physical. Convert output physical -> logical
   case LVM_GETNEXTITEM:
      wParam = calculatePhysicalIndex(pWindowData, wParam);
      calculateLogicalIndex(pWindowData, sendBaseMessage(pWindowData->hCtrl, iMessage, wParam, lParam), &oItem);
      return oItem.iLogicalIndex;

   /// [HIT-TEST] Convert output physical -> logical
   case LVM_HITTEST:
      pHitTest = (LVHITTESTINFO*)lParam;

      if (sendBaseMessage(pWindowData->hCtrl, iMessage, wParam, lParam) != -1)
      {  // [HIT] Convert to logical index
         calculateLogicalIndex(pWindowData, pHitTest->iItem, &oItem);
         pHitTest->iItem = oItem.iLogicalIndex;
      }

      // Return item index or -1
      return pHitTest->iItem;
   }

   // Pass to base
   return sendBaseMessage(hCtrl, iMessage, wParam, lParam);
}

