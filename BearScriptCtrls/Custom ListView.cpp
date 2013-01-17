//
// Custom ListView.cpp : Custom draw ListView with a VisualStudio-esque appearance
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION  /  DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////////////////


/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                     CONSTANTS/ GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// Colours
COLORREF  clLightGrey = RGB(247,247,247);    // ListView sort column

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                      HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : initReportModeListView
// Description     : Intiailises a 'Report' mode ListView : Adds columns, an ImageList and the 'FullRowSelection' style
// 
// HWND                     hListView     : [in] ListView window handle
// CONST LISTVIEW_COLUMNS*  pListViewData : [in] Data used to setup the listView
// CONST BOOL               bEnableHover  : [in] Enables the 'hover' effect on items 
///                                                  This option requires you pass a window's WM_NOTIFY messages to onCustomListViewNotify(..) 
// 
ControlsAPI 
VOID  initReportModeListView(HWND  hListView, CONST LISTVIEW_COLUMNS*  pListViewData, CONST BOOL  bEnableHover)
{
   LVCOLUMN  oColumn;     // Column data

   /// Set Report mode
   ListView_SetView(hListView, LV_VIEW_DETAILS);

   // Iterate through desired columns
   for (UINT iColumn = 0; iColumn < pListViewData->iColumnCount; iColumn++)
   {
      /// Define and insert column
      utilSetListViewColumn(&oColumn, iColumn, utilLoadString(getResourceInstance(), (iColumn + pListViewData->iColumnResourceID), 48), pListViewData->iColumnWidths[iColumn]);
      ListView_InsertColumn(hListView, iColumn, &oColumn);

      // Cleanup
      utilDeleteString(oColumn.pszText);
   }

   /// Add (Shared) ImageList and 'Full Row Selection' style
   utilAddWindowStyle(hListView, LVS_SHAREIMAGELISTS);
   ListView_SetExtendedListViewStyleEx(hListView, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
   ListView_SetImageList(hListView, getAppImageList(ITS_SMALL), LVSIL_SMALL);

   // [HOVER] Add 'Track selection' style and set hot tracking timeout to inifinity
   if (bEnableHover AND getAppWindowsVersion() >= WINDOWS_VISTA)     /// Disable on WinXP
   {
      ListView_SetExtendedListViewStyleEx(hListView, LVS_EX_TRACKSELECT, LVS_EX_TRACKSELECT);
      ListView_SetHoverTime(hListView, -2);
   }
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                      FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : drawCustomListViewItem
// Description     : Performs ListView custom draw for a single item in report mode
///                                    This function does not set the CustomDraw result code
// 
// HWND             hParent   : [in] Parent window that provides item data
// HWND             hListView : [in] ListView window
// NMLVCUSTOMDRAW*  pHeader   : [in] CustomDraw header
// 
ControlsAPI
VOID  drawCustomListViewItem(HWND  hParent, HWND  hListView, NMLVCUSTOMDRAW*  pHeader)
{
   NMCUSTOMDRAW*  pDrawData;
   HIMAGELIST     hImageList;       // ListView ImageList, if any
   NMLVDISPINFO   oDataRequest;     // WM_NOTIFY item data request
   COLORREF       clOldColour;      // Previous text colour
   LVITEM*        pItem;            // Convenience pointer
   RECT           rcSubItem;        // Current item/sub-item rectangle
   UINT           iBackgroundMode,  // Previous background drawing mode
                  eItemState,       // Item state
                  iColumnCount,     // Number of ListView columns
                  iSortColumn;      // Index of sorted column, if any
   HBRUSH         hColumnBrush,     // Column background brush
                  hSortBrush;       // Sort column background brush

   // Prepare
   pDrawData = &pHeader->nmcd;

   // [WIN XP] Manually lookup item rectangle
   if (getAppWindowsVersion() < WINDOWS_VISTA)
      ListView_GetItemRect(hListView, pDrawData->dwItemSpec, &pDrawData->rc, LVIR_BOUNDS);

   // [VISTA] Skip drawing if rectangle is empty
   else if (!pDrawData->rc.right OR !pDrawData->rc.bottom)
      return;

   // Setup WM_NOTIFY message
   utilZeroObject(&oDataRequest, NMLVDISPINFO);
   oDataRequest.hdr.code     = LVN_GETDISPINFO;
   oDataRequest.hdr.idFrom   = GetWindowID(hListView);
   oDataRequest.hdr.hwndFrom = hListView;

   // Setup item data request
   pItem             = &oDataRequest.item;
   pItem->pszText    = utilCreateEmptyString(512);
   pItem->cchTextMax = 512;

   // [CHECK] Get column count and ImageList
   iColumnCount  = Header_GetItemCount(ListView_GetHeader(hListView));
   hImageList    = ListView_GetImageList(hListView, LVSIL_SMALL);

   // Determine item state 
   eItemState = (ListView_GetItemState(hListView, pDrawData->dwItemSpec, LVIS_SELECTED) ? CDIS_SELECTED : NULL);

   // [HOT TRACKING]  (Use 'focus' state as 'hot')
   if (ListView_GetExtendedListViewStyle(hListView) INCLUDES LVS_EX_TRACKSELECT)
      eItemState |= (pDrawData->uItemState INCLUDES CDIS_FOCUS ? CDIS_HOT : NULL);

   // [COLUMNS] Create background brushes
   hSortBrush   = CreateSolidBrush(clLightGrey);
   iSortColumn  = ListView_GetSelectedColumn(hListView);
   hColumnBrush = CreateSolidBrush(ListView_GetBkColor(hListView));
   
   // Prepare
   iBackgroundMode = SetBkMode(pDrawData->hdc, TRANSPARENT);

   /// Iterate through all sub-items
   for (UINT  iColumn = 0; iColumn < iColumnCount; iColumn++)
   {
      // Prepare
      pItem->mask       = LVIF_TEXT WITH LVIF_IMAGE WITH LVIF_PARAM WITH LVIF_INDENT;
      pItem->iItem      = pHeader->nmcd.dwItemSpec;
      pItem->iSubItem   = iColumn;
      pItem->cColumns   = DT_LEFT;
      pItem->pszText[0] = NULL;

      // Manually retrieve item data  (BUGFIX: ListView_GetItem wasn't retrieving the LPARAM value)
      SendMessage(hParent, WM_NOTIFY, oDataRequest.hdr.idFrom, (LPARAM)&oDataRequest);

      // Examine subitem
      switch (pItem->iSubItem)
      {
      /// [ITEM] Draw icon and text
      case 0:
         // [CHECK] Are we using a custom state?
         if (pItem->mask INCLUDES LVIF_STATE)
            // [CHECK] Determine whether item is disabled
            eItemState |= (pItem->state == LVIS_DISABLED ? CDIS_DISABLED : NULL);

         /// [INDENT] Offset our drawing rectangle
         pDrawData->rc.left += pItem->iIndent * GetSystemMetrics(SM_CXSMICON);

         // [FULLROW] Highlight/erase entire item
         if (utilIncludes(ListView_GetExtendedListViewStyle(hListView), LVS_EX_FULLROWSELECT) OR utilExcludes(eItemState, CDIS_SELECTED WITH CDIS_HOT))
            rcSubItem = pDrawData->rc;
         else
         {  // [¬FULLROW] Highlight/erase only label
            ListView_GetItemRect(hListView, pItem->iItem, &rcSubItem, LVIR_SELECTBOUNDS);
            rcSubItem.left -= GetSystemMetrics(SM_CXFIXEDFRAME);
         }

         /// [BACKGROUND] Draw highlight/erase label or entire row
         switch (eItemState)
         {
         case CDIS_SELECTED:
         case CDIS_SELECTED WITH CDIS_HOT:   drawCustomSelectionRectangle(pDrawData->hdc, &rcSubItem);         break;
         case CDIS_HOT:                      drawCustomHoverRectangle(pDrawData->hdc, &rcSubItem);             break;
         default:                            FillRect(pDrawData->hdc, &rcSubItem, iColumn == iSortColumn ? hSortBrush : hColumnBrush);  break;
         }

         // [¬FULLROW] Erase remainder of column
         if (utilExcludes(ListView_GetExtendedListViewStyle(hListView), LVS_EX_FULLROWSELECT))
         {
            rcSubItem.left  = rcSubItem.right;
            rcSubItem.right = ListView_GetColumnWidth(hListView, 0);
            FillRect(pDrawData->hdc, &rcSubItem, iColumn == iSortColumn ? hSortBrush : hColumnBrush);
         }

         // [CHECK] Are we drawing an icon?
         if (hImageList)
         {
            // Retrieve icon rectangle
            ListView_GetItemRect(hListView, pItem->iItem, &rcSubItem, LVIR_ICON);

            /// [ICON] Draw icon 
            drawIcon(hImageList, LOWORD(pItem->iImage), pDrawData->hdc, rcSubItem.left, rcSubItem.top, utilIncludes(eItemState, CDIS_SELECTED WITH CDIS_HOT) AND utilExcludes(eItemState, CDIS_DISABLED) ? IS_SELECTED : IS_NORMAL);

            /// [OVERLAY]
            if (HIWORD(pItem->iImage))
              drawIcon(hImageList, HIWORD(pItem->iImage), pDrawData->hdc, rcSubItem.left, rcSubItem.top, utilIncludes(eItemState, CDIS_SELECTED WITH CDIS_HOT) AND utilExcludes(eItemState, CDIS_DISABLED) ? IS_SELECTED : IS_NORMAL);
         }

         // Retrieve text rectangle
         ListView_GetItemRect(hListView, pItem->iItem, &rcSubItem, LVIR_LABEL);
         InflateRect(&rcSubItem, -GetSystemMetrics(SM_CXFIXEDFRAME), 0);
         break;

      /// [SUBITEM] Calculate sub-item rectangle
      default:
         // Retrieve sub-item rectangle
         ListView_GetSubItemRect(hListView, pItem->iItem, pItem->iSubItem, LVIR_BOUNDS, &rcSubItem);

         // [¬FULLROW] Erase sub-item item background
         if (utilExcludes(ListView_GetExtendedListViewStyle(hListView), LVS_EX_FULLROWSELECT))
            FillRect(pDrawData->hdc, &rcSubItem, iColumn == iSortColumn ? hSortBrush : hColumnBrush);

         // Add borders
         InflateRect(&rcSubItem, -2 * GetSystemMetrics(SM_CXFIXEDFRAME), 0);
         break;
      }

      /// [COLOUR] Set text colour
      if (pItem->mask INCLUDES LVIF_COLOUR_TEXT)
         clOldColour = SetTextColor(pDrawData->hdc, (COLORREF)pItem->lParam);

      // [CHECK] Is RichText specified?
      if (pItem->lParam AND utilIncludes(pItem->mask, LVIF_RICHTEXT))
      {
         /// [RICH-TEXT] Draw RichText, and optionally destroy
         drawRichTextInSingleLine(pDrawData->hdc, rcSubItem, (RICH_TEXT*)pItem->lParam, GTC_WHITE);

         // [CHECK] Should the RichText be destroyed?
         if (utilIncludes(pItem->mask, LVIF_DESTROYTEXT))
            deleteRichText((RICH_TEXT*&)pItem->lParam);
      }
      else
         /// [PLAINTEXT] Draw PlaintText
         DrawText(pDrawData->hdc, pItem->pszText, lstrlen(pItem->pszText), &rcSubItem, pItem->cColumns WITH DT_VCENTER WITH DT_SINGLELINE WITH DT_WORD_ELLIPSIS WITH DT_NOPREFIX);

      // Restore DC colour
      if (pItem->mask INCLUDES LVIF_COLOUR_TEXT)
         SetTextColor(pDrawData->hdc, clOldColour);
   }

   // Restore DC
   iBackgroundMode = SetBkMode(pDrawData->hdc, iBackgroundMode);

   // Cleanup
   DeleteBrush(hSortBrush);
   DeleteBrush(hColumnBrush);
   utilDeleteString(pItem->pszText);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                   MESSAGE HANDLERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onCustomDrawListView
// Description     : Performs Custmom ListView drawing.
///                                    Cannot be used with the 'Always show selection' style
// 
// HWND             hWnd      : [in] Window or Dialog handle
// HWND             hListView : [in] Window or Dialog handle
// NMLVCUSTOMDRAW*  pHeader   : [in] ListView CustomDraw message header
// 
// Return Value   : Custom draw notification code
// 
ControlsAPI
UINT  onCustomDrawListView(HWND  hParent, HWND  hListView, NMLVCUSTOMDRAW*  pHeader)
{
   UINT   iResultCode;      // CustomDraw notification result code
                  
   // Prepare
   iResultCode = NULL;

   // Examine current draw stage
   switch (pHeader->nmcd.dwDrawStage)
   {
   /// [PRE-PAINT] Request per-item drawing
   case CDDS_PREPAINT:
      iResultCode = CDRF_NOTIFYITEMDRAW WITH CDRF_NOTIFYPOSTPAINT;
      break;

   /// [POST-PAINT] Skip focus rectangle
   case CDDS_POSTPAINT:
      iResultCode = CDRF_SKIPPOSTPAINT;
      break;

   /// [ITEM PRE-PAINT] Draw all items and sub-items at once
   case CDDS_PREPAINT WITH CDDS_ITEM:
      drawCustomListViewItem(hParent, hListView, pHeader);
      iResultCode = CDRF_SKIPDEFAULT;
      break;
   }

   // Return drawing notification code
   return iResultCode;
}


/// Function name  : onCustomListViewNotify
// Description     : Convenience handler for implementing the hacked hot-tracking of custom drawn ListViews
// 
// HWND        hParent     : [in] Dialog or window handle
// BOOL        bIsDialog   : [in] Whether parent is a dialog or not
// CONST UINT  iListViewID : [in] Control ID of the ListView
// NMHDR*      pMessage    : [in] WM_NOTIFY message data
// 
// Return Value   : TRUE if processed, otherwise FALSE
///                        If bIsDialog is TRUE, the result is stored using SetWindowLong
// 
ControlsAPI
BOOL  onCustomListViewNotify(HWND  hParent, BOOL  bIsDialog, CONST UINT  iListViewID, NMHDR*  pMessage)
{
   NMLVODSTATECHANGE*  pStateChange;
   NMLISTVIEW*         pItemData;
   BOOL                bResult;

   // Prepare
   bResult = FALSE;

   // Examine notification
   switch (pMessage->code)
   {
   /// [CUSTOM DRAW]
   case NM_CUSTOMDRAW:
      if (pMessage->idFrom == iListViewID)
         bResult = onCustomDrawListView(hParent, pMessage->hwndFrom, (NMLVCUSTOMDRAW*)pMessage);
      break;

   /// [HOT TRACK] HACK: Add the 'focus' style to the hot item - drawn as hot
   case LVN_HOTTRACK:
      if (pMessage->idFrom == iListViewID AND utilIncludes(ListView_GetExtendedListViewStyle(pMessage->hwndFrom), LVS_EX_TRACKSELECT))
      {
         // Prepare
         pItemData = (NMLISTVIEW*)pMessage;

         if (utilExcludes(ListView_GetExtendedListViewStyle(pMessage->hwndFrom), LVS_EX_FULLROWSELECT))
            // [NORMAL] Focus only when hovering over subitem zero
            ListView_SetItemState(pMessage->hwndFrom, pItemData->iItem, (pItemData->iSubItem == 0 ? LVIS_FOCUSED : NULL), LVIS_FOCUSED)
         else
            // [FULLROW] Add focus style
            ListView_SetItemState(pMessage->hwndFrom, pItemData->iItem, LVIS_FOCUSED, LVIS_FOCUSED)
         
         // Block hot-track activation
         pItemData->iItem = -1;
         bResult = TRUE;
      }
      break;

   /// [ITEM (UN)FOCUSED or (UN)SELECTED] HACK: Refresh item that is no longer 'hot'
   case LVN_ITEMCHANGED:
      // [CHECK] Ensure message if from ListView
      if (pMessage->idFrom == iListViewID AND utilIncludes(ListView_GetExtendedListViewStyle(pMessage->hwndFrom), LVS_EX_TRACKSELECT))
      {
         // Prepare
         pItemData = (NMLISTVIEW*)pMessage;

         // [CHECK] Has item just lost or gained focus?
         if (utilIncludes(pItemData->uNewState, LVIS_FOCUSED WITH LVIS_SELECTED) OR utilIncludes(pItemData->uOldState, LVIS_FOCUSED WITH LVIS_SELECTED))
         {
            /// [CHANGED] Redraw item
            ListView_RedrawItems(pMessage->hwndFrom, pItemData->iItem, pItemData->iItem);
            bResult = TRUE;
         }
      }
      break;

   case LVN_ODSTATECHANGED:
      // [CHECK] Ensure message if from ListView
      if (pMessage->idFrom == iListViewID AND utilIncludes(ListView_GetExtendedListViewStyle(pMessage->hwndFrom), LVS_EX_TRACKSELECT))
      {
         // Prepare
         pStateChange = (NMLVODSTATECHANGE*)pMessage;

         // [CHECK] Has item just lost or gained focus?
         if (utilIncludes(pStateChange->uNewState, LVIS_FOCUSED WITH LVIS_SELECTED) OR utilIncludes(pStateChange->uOldState, LVIS_FOCUSED WITH LVIS_SELECTED))
         {
            /// [CHANGED] Redraw item
            ListView_RedrawItems(pMessage->hwndFrom, pStateChange->iFrom, pStateChange->iTo);
            bResult = TRUE;
         }
      }
      break;
   }

   // [CHECK] Store dialog result
   if (bIsDialog AND bResult)
      SetWindowLong(hParent, DWL_MSGRESULT, bResult);

   // Return result
   return bResult;
}


/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                   OLD CODE
/// ////////////////////////////////////////////////////////////////////////////////////////////////////


/// Function name  : onCustomListViewItemEdit
// Description     : Initiate custom ListView label editing by creating a control over a specified sub-item
// 
// LISTVIEW_LABEL_DATA*  pLabelData  : [in] Label Data containing the item/subitem and the type of control to create
// HWND                  hListView   : [in] Window handle of the ListView containing the item
// 
// Return Value   : TRUE
// 
ControlsAPI 
BOOL   onCustomListViewItemEdit(LISTVIEW_LABEL_DATA*  pLabelData, HWND  hListView)
{
   //NMLVLABELINFO  oNotification;    // Notification data for setting up the new control
   //RECT           rcSubItemRect;    // Sub-item rectangle
   //SIZE           siSubItemSize;    // Size of the sub-item rectangle
   //
   //// [CHECK] Shouldn't be here if no item was selected
   //if (pLabelData->iItem == -1)
   //   return TRUE;

   ///// [ITEM/SUBITEM] Get the bounding rectangle for the element to be edited
   //rcSubItemRect.left = LVIR_LABEL;
   //rcSubItemRect.top  = pLabelData->iSubItem;
   //SendMessage(hListView, pLabelData->iSubItem > 0 ? LVM_GETSUBITEMRECT : LVM_GETITEMRECT, pLabelData->iItem, (LPARAM)&rcSubItemRect);
   //
   ///// Create appropriate label editing control
   //switch (pLabelData->eCtrlType)
   //{
   //case LVLT_EDIT:
   //   pLabelData->hCtrl = CreateWindowEx(NULL, WC_EDIT, NULL, WS_CHILD WITH WS_BORDER WITH ES_WANTRETURN WITH ES_AUTOHSCROLL, 0, 0, CW_USEDEFAULT, CW_USEDEFAULT, hListView, (HMENU)IDC_LISTVIEW_LABEL_CTRL, getAppInstance(), NULL);
   //   break;

   //case LVLT_COMBOBOX:
   //   pLabelData->hCtrl = CreateWindowEx(NULL, WC_COMBOBOX, NULL, WS_CHILD WITH WS_BORDER WITH WS_VSCROLL WITH CBS_DROPDOWNLIST WITH CBS_SORT, 0, 0, CW_USEDEFAULT, CW_USEDEFAULT, hListView, (HMENU)IDC_LISTVIEW_LABEL_CTRL, getAppInstance(), NULL);
   //   SendMessage(pLabelData->hCtrl, CB_SETDROPPEDWIDTH, 200, NULL);
   //   SendMessage(pLabelData->hCtrl, CB_SETMINVISIBLE, 8, NULL);
   //   break;
   //}
   //
   //// Subclass new control and pass label data
   //pLabelData->wndprocCtrl = (WNDPROC)SetWindowLong(pLabelData->hCtrl, GWL_WNDPROC, (LONG)wndprocCustomListViewLabelCtrl);
   //SendMessage(pLabelData->hCtrl, UM_SET_WINDOW_DATA, NULL, (LPARAM)pLabelData);

   ///// Send a modified 'BeginLabelEdit' notification. This allows user to fill ComboBox items or edit ctrl text.
   //oNotification.oMessage.code     = LVCN_BEGINLABELEDIT;
   //oNotification.oMessage.hwndFrom = hListView;
   //oNotification.oMessage.idFrom   = GetDlgCtrlID(hListView);
   //// Pass control handle and item/subitem
   //oNotification.eCtrlType = pLabelData->eCtrlType;
   //oNotification.hCtrl     = pLabelData->hCtrl;
   //oNotification.iItem     = pLabelData->iItem;
   //oNotification.iSubItem  = pLabelData->iSubItem;
   //// Send to the parent of the ListView
   //pLabelData->hParent = GetParent(hListView);
   //SendMessage(pLabelData->hParent, WM_NOTIFY, oNotification.oMessage.idFrom, (LPARAM)&oNotification);

   ///// Reposition control
   //utilConvertRectangleToSize(&rcSubItemRect, &siSubItemSize);
   //siSubItemSize.cy += 4;
   //MoveWindow(pLabelData->hCtrl, rcSubItemRect.left, rcSubItemRect.top, siSubItemSize.cx, siSubItemSize.cy, TRUE);
   //SetWindowPos(pLabelData->hCtrl, HWND_TOPMOST, rcSubItemRect.left, rcSubItemRect.top, siSubItemSize.cx, siSubItemSize.cy, SWP_NOMOVE WITH SWP_NOSIZE);

   //// Assign normal font, show control and set focus
   //SetWindowFont(pLabelData->hCtrl, GetStockObject(ANSI_VAR_FONT), FALSE);
   //ShowWindow(pLabelData->hCtrl, SW_SHOW);
   //SetFocus(pLabelData->hCtrl);

   return TRUE;
}


/// Function name  : wndprocCustomListViewLabelCtrl
// Description     : Sub-classed window procedure for the edit/combobox used in custom listview item editing
// 
// 
LRESULT  wndprocCustomListViewLabelCtrl(HWND  hCtrl, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   static LISTVIEW_LABEL_DATA*  pLabelData = NULL;   // ListView label editing data
   //NMLVLABELINFO                oNotification;       // Notification message data
   //
   //switch (iMessage)
   //{
   ///// [INPUT DATA] -- Store the LABEL_DATA for the current edit
   //case UM_SET_WINDOW_DATA:
   //   pLabelData = (LISTVIEW_LABEL_DATA*)lParam;
   //   return 0;

   ///// [DESTROY WINDOW] -- Zero input LABEL_DATA
   //case WM_DESTROY:
   //   CallWindowProc(pLabelData->wndprocCtrl, hCtrl, iMessage, wParam, lParam);
   //   SetWindowLong(hCtrl, GWL_WNDPROC, (LONG)pLabelData->wndprocCtrl);
   //   pLabelData = NULL;
   //   return 0;

   ///// [ENTER or ESCAPE KEY] -- Pass current text / selected item to the ListView's parent
   //case WM_KEYDOWN:
   //   switch (wParam)
   //   {
   //   // [ENTER] - Check user has entered text
   //   case VK_RETURN:
   //      if (pLabelData->eCtrlType == LVLT_EDIT AND GetWindowTextLength(hCtrl) == 0)
   //         break;
   //      
   //   // [ESCAPE] - Return NULL instead of window text or -1 instead of item index
   //   case VK_ESCAPE:
   //      // Setup a modified 'EndLabelEdit' notification
   //      oNotification.oMessage.code     = LVCN_ENDLABELEDIT;
   //      oNotification.oMessage.hwndFrom = GetParent(hCtrl);
   //      oNotification.oMessage.idFrom   = GetDlgCtrlID(oNotification.oMessage.hwndFrom);
   //      // Pass the item/subitem and control type
   //      oNotification.iItem     = pLabelData->iItem;
   //      oNotification.iSubItem  = pLabelData->iSubItem;
   //      oNotification.eCtrlType = pLabelData->eCtrlType;
   //      // Pass the current text / currently selected item.  If user pressed ESCAPE, pass NULL or -1.
   //      switch (pLabelData->eCtrlType)
   //      {
   //      case LVLT_EDIT:     oNotification.szNewText  = (wParam == VK_RETURN ? utilGetWindowText(hCtrl) : NULL);           break;            
   //      case LVLT_COMBOBOX: oNotification.iSelection = (wParam == VK_RETURN ? SendMessage(hCtrl, CB_GETCURSEL, NULL, NULL) : -1);  break;
   //      }
   //      
   //      // Send notification to the parent of the ListView
   //      SendMessage(pLabelData->hParent, WM_NOTIFY, IDC_LISTVIEW_LABEL_CTRL, (LPARAM)&oNotification);
   //      // Destroy label text (if any)
   //      if (pLabelData->eCtrlType == LVLT_EDIT AND oNotification.szNewText)
   //         utilDeleteString(oNotification.szNewText);
   //      // Destroy control by setting the focus back to the ListView
   //      SetFocus(GetParent(hCtrl));
   //      return 0;
   //   }
   //   break;

   ///// [LOST FOCUS] -- Destroy control and (For ComboBoxes only) notify the parent of the current selection
   //case WM_KILLFOCUS:
   //   if (pLabelData->eCtrlType == LVLT_COMBOBOX)
   //   {
   //      // Setup a modified 'EndLabelEdit' notification
   //      oNotification.oMessage.code     = LVCN_ENDLABELEDIT;
   //      oNotification.oMessage.idFrom   = IDC_LISTVIEW_LABEL_CTRL;
   //      oNotification.oMessage.hwndFrom = hCtrl;
   //      // Pass the item/subitem
   //      oNotification.iItem      = pLabelData->iItem;
   //      oNotification.iSubItem   = pLabelData->iSubItem;
   //      // Pass the current selection index
   //      oNotification.eCtrlType  = LVLT_COMBOBOX;
   //      oNotification.iSelection = SendMessage(hCtrl, CB_GETCURSEL, NULL, NULL);
   //      // Send notification to the parent of the ListView
   //      SendMessage(pLabelData->hParent, WM_NOTIFY, IDC_LISTVIEW_LABEL_CTRL, (LPARAM)&oNotification);
   //   }
   //   DestroyWindow(hCtrl);
   //   return 0;

   ///// [KEYBOARD QUERY] -- Request all keyboard input
   //case WM_GETDLGCODE:
   //   return DLGC_WANTALLKEYS;
   //}

   // Pass to the control's base window proc
   return CallWindowProc(pLabelData->wndprocCtrl, hCtrl, iMessage, wParam, lParam);
}
