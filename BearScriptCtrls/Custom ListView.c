//
// Custom ListView.cpp : Custom draw ListView with a VisualStudio-esque appearance
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          MACROS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// OnException: Print to console
#define ON_EXCEPTION()     printException(pException)

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                     CONSTANTS/ GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// Colours
COLORREF  clLightGrey  = RGB(247,247,247);    // ListView sort column

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
      utilSetListViewColumn(&oColumn, iColumn, loadString((iColumn + pListViewData->iColumnResourceID), 48), pListViewData->iColumnWidths[iColumn]);
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

   /// Set themed background colour
   ListView_SetBkColor(hListView, getThemeSysColour(TEXT("TAB"), COLOR_WINDOW));
}



/// Function name  : setMissingListViewItem
// Description     : Returns a stock phrase indicate the item ID is invalid
// 
// LVITEM*     pItem  : [in/out] Item
// const UINT  iCount : [in] Item count
// 
ControlsAPI
VOID  setMissingListViewItem(LVITEM*  pItem, const UINT  iCount)
{
   if (pItem->mask INCLUDES LVIF_TEXT)
      // [FAILED] "Error item %d of %d not found"
      StringCchPrintf(pItem->pszText, pItem->cchTextMax, TEXT("(Error: Item %d of %d not found)"), pItem->iItem, iCount);
   else
      pItem->pszText = TEXT("ERROR: Item not found");
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                      FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

ICON_STATE  calculateIconState(UINT  eItemState, const BOOL  bCtrlEnabled)
{
   if (!bCtrlEnabled)
      return IS_DISABLED;
   else
      return utilIncludes(eItemState, CDIS_SELECTED WITH CDIS_HOT) AND utilExcludes(eItemState, CDIS_DISABLED) ? IS_SELECTED : IS_NORMAL;
}

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
   BOOL           bCtrlEnabled;
   HBRUSH         hColumnBrush,     // Column background brush
                  hSortBrush;       // Sort column background brush
   COLORREF          clBackground;  // Background colour
   GAME_TEXT_COLOUR  eBackground;   // Background colour broadly converted to white or black

   TRY
   {
      // Prepare
      pDrawData   = &pHeader->nmcd;
      clOldColour = clNullColour;
      bCtrlEnabled = IsWindowEnabled(hListView);

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
      else
         eItemState |= (ListView_GetItemState(hListView, pDrawData->dwItemSpec, LVIS_CUT) ? CDIS_HOT : NULL);

      // [LISTVIEW DISABLED] Draw all items as 'Disabled'
      if (!IsWindowEnabled(hListView))
         eItemState = CDIS_DISABLED;

      // Determine background colour
      clBackground = bCtrlEnabled ? ListView_GetBkColor(hListView) : GetSysColor(COLOR_BTNFACE);
      eBackground  = (GetRValue(clBackground) > 128 AND GetGValue(clBackground) > 128 AND GetBValue(clBackground) > 128 ? GTC_WHITE : GTC_BLACK);

      // [COLUMNS] Create background brushes
      //hSortBrush   = CreateSolidBrush(clLightGrey);
      hSortBrush = CreateSolidBrush(IsThemeActive() ? getThemeColour(TEXT("Listview"), LVP_LISTSORTEDDETAIL, 0, TMT_EDGEHIGHLIGHTCOLOR, 0) : clLightGrey); //   getThemeSysColourBrush(TEXT("Window"), COLOR_INFOBK);
      hColumnBrush = CreateSolidBrush(clBackground);
      iSortColumn  = ListView_GetSelectedColumn(hListView);
      iBackgroundMode = SetBkMode(pDrawData->hdc, TRANSPARENT);

      /// Iterate through all sub-items
      for (UINT  iColumn = 0; iColumn < iColumnCount; iColumn++)
      {
         // Prepare
         pItem->mask       = LVIF_TEXT WITH LVIF_PARAM WITH (iColumn == 0 ? LVIF_INDENT WITH LVIF_IMAGE : NULL);
         pItem->iItem      = pHeader->nmcd.dwItemSpec;
         pItem->iSubItem   = iColumn;
         pItem->cColumns   = DT_LEFT;
         pItem->pszText[0] = NULL;
         pItem->lParam     = LVIP_CUSTOM_DRAW;     // Indicate to handler we're accepting enhanced mask flags: LVIF_PLAINTEXT, LVIF_RICHTEXT, LVIF_DESTROYTEXT, LVIF_COLOUR_TEXT
         pItem->iIndent    = 0;

         // Manually retrieve item data  (BUGFIX: ListView_GetItem wasn't retrieving the LPARAM value)
         SendMessage(hParent, WM_NOTIFY, oDataRequest.hdr.idFrom, (LPARAM)&oDataRequest);

         // Examine subitem
         switch (pItem->iSubItem)
         {
         /// [ITEM] Draw icon and text
         case 0:
            // [DISABLED ITEM] Determine whether item is disabled
            if (pItem->mask INCLUDES LVIF_STATE)
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
               drawIcon(hImageList, LOWORD(pItem->iImage), pDrawData->hdc, rcSubItem.left, rcSubItem.top, calculateIconState(eItemState, bCtrlEnabled));

               /// [OVERLAY]
               if (HIWORD(pItem->iImage))
                 drawIcon(hImageList, HIWORD(pItem->iImage), pDrawData->hdc, rcSubItem.left, rcSubItem.top, calculateIconState(eItemState, bCtrlEnabled));
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

         /// [DISABLED/COLOURED] Set text colour
         if ((pItem->mask & LVIF_COLOUR_TEXT) OR !bCtrlEnabled)
            clOldColour = SetTextColor(pDrawData->hdc, (!bCtrlEnabled ? GetSysColor(COLOR_GRAYTEXT) : (COLORREF)pItem->lParam));
         else
            clOldColour = SetTextColor(pDrawData->hdc, getThemeSysColour(TEXT("TAB"), COLOR_WINDOWTEXT));

         // [CHECK] Is RichText specified?
         if (pItem->lParam AND utilIncludes(pItem->mask, LVIF_RICHTEXT))
         {
            RICH_TEXT*  pRichText = (RICH_TEXT*)pItem->lParam;

            /// [RICH-TEXT] Draw RichText, and optionally destroy
            if (pRichText->eType == RTT_LANGUAGE_MESSAGE)
               drawLanguageMessageInSingleLine(pDrawData->hdc, rcSubItem, (LANGUAGE_MESSAGE*)pRichText, !IsWindowEnabled(hListView), eBackground);
            else
               drawRichTextInSingleLine(pDrawData->hdc, rcSubItem, pRichText, !IsWindowEnabled(hListView), eBackground);

            // [CHECK] Should the RichText be destroyed?
            if (utilIncludes(pItem->mask, LVIF_DESTROYTEXT))
            {
               deleteRichText(pRichText);
               pItem->lParam = NULL;
            }
         }
         else
            /// [PLAINTEXT] Draw PlaintText
            DrawText(pDrawData->hdc, pItem->pszText, lstrlen(pItem->pszText), &rcSubItem, pItem->cColumns WITH DT_VCENTER WITH DT_SINGLELINE WITH DT_WORD_ELLIPSIS WITH DT_NOPREFIX);

         // Restore DC colour
         if (clOldColour != clNullColour)
            SetTextColor(pDrawData->hdc, clOldColour);
      }

      // Restore DC
      iBackgroundMode = SetBkMode(pDrawData->hdc, iBackgroundMode);

      // Cleanup
      DeleteBrush(hSortBrush);
      DeleteBrush(hColumnBrush);
      utilDeleteString(pItem->pszText);
   }
   CATCH0("");
}


 
/// Function name  : editCustomListViewItem
// Description     : Initiate custom ListView label editing
///                        Creates + positions the control, which requests population from the parent before this function returns
// 
// HWND                  hListView : [in] ListView
// const UINT            iItem     : [in] Item
// const UINT            iSubItem  : [in] SubItem
// const LISTVIEW_LABEL  eCtrlType : [in] LVLT_EDIT or LVLT_COMBOBOX
// 
// Return Value   : TRUE/FALSE
// 
ControlsAPI
BOOL   editCustomListViewItem(HWND  hListView, const UINT  iItem, const UINT  iSubItem, const LISTVIEW_LABEL  eCtrlType)
{
   return editCustomListViewItemEx(hListView, iItem, iSubItem, eCtrlType, NULL, onWindow_CompareComboBoxItems);
}


/// Function name  : editCustomListViewItem
// Description     : Initiate custom ListView label editing
///                        Creates + positions the control, which requests population from the parent before this function returns
// 
// HWND                  hListView     : [in] ListView
// const UINT            iItem         : [in] Item
// const UINT            iSubItem      : [in] SubItem
// const LISTVIEW_LABEL  eCtrlType     : [in] LVLT_EDIT or LVLT_COMBOBOX
// const DWORD           dwCustomStyle : [in] Additional window styles
// COMPARE_COMBO_PROC    pfnComparitor : [in] Item comparitor function
// 
// Return Value   : TRUE/FALSE
// 
ControlsAPI 
BOOL   editCustomListViewItemEx(HWND  hListView, const UINT  iItem, const UINT  iSubItem, const LISTVIEW_LABEL  eCtrlType, const DWORD  dwCustomStyle, COMPARE_COMBO_PROC  pfnComparitor)
{
   LVITEM  oItem;    // Notification data for setting up the new control
   RECT    rcItem;    // Sub-item rectangle
   SIZE    siItem;    // Size of the sub-item rectangle
   HWND    hCtrl;
   
   // Prepare
   utilZeroObject(&oItem, LVITEM);

   // Store item data
   oItem.iItem    = iItem;
   oItem.iSubItem = iSubItem;
   oItem.lParam   = eCtrlType;
   oItem.piColFmt = (INT*)pfnComparitor;

   // Get the SubItem rectangle
   rcItem.left = LVIR_LABEL;
   rcItem.top  = iSubItem;
   SendMessage(hListView, (iSubItem ? LVM_GETSUBITEMRECT : LVM_GETITEMRECT), iItem, (LPARAM)&rcItem);
   utilConvertRectangleToSize(&rcItem, &siItem);
   
   /// Create control
   switch (eCtrlType)
   {
   // [EDIT] 
   case LVLT_EDIT:
      hCtrl = CreateWindowEx(NULL, WC_EDIT, NULL, WS_CHILD WITH WS_BORDER WITH ES_WANTRETURN WITH ES_AUTOHSCROLL | dwCustomStyle, 0, 0, CW_USEDEFAULT, CW_USEDEFAULT, hListView, (HMENU)IDC_LISTVIEW_LABEL_CTRL, getAppInstance(), NULL);
      break;

   // [COMBO] 
   case LVLT_COMBOBOX:
      hCtrl = CreateWindowEx(NULL, WC_COMBOBOX, NULL, WS_CHILD WITH WS_BORDER WITH WS_VSCROLL WITH CBS_DROPDOWNLIST WITH CBS_SORT | dwCustomStyle, 0, 0, CW_USEDEFAULT, CW_USEDEFAULT, hListView, (HMENU)IDC_LISTVIEW_LABEL_CTRL, getAppInstance(), NULL);
      SendMessage(hCtrl, CB_SETDROPPEDWIDTH, 200, NULL);
      SendMessage(hCtrl, CB_SETMINVISIBLE, 8, NULL);
      break;
   }

   // [CHECK] Ensure window was created
   if (!hCtrl)
      return FALSE;

   /// Pass label data. Command parent to populate control
   SubclassWindow(hCtrl, wndprocCustomListViewLabel);
   SendMessage(hCtrl, WM_USER, GetWindowID(hListView), (LPARAM)&oItem);

   // Show + Focus
   MoveWindow(hCtrl, rcItem.left, rcItem.top, siItem.cx, siItem.cy + 4, FALSE);
   SetWindowPos(hCtrl, HWND_TOPMOST, NULL, NULL, NULL, NULL, SWP_NOMOVE | SWP_NOSIZE);
   SetWindowFont(hCtrl, GetStockObject(ANSI_VAR_FONT), FALSE);
   ShowWindow(hCtrl, SW_SHOWNORMAL);
   SetFocus(hCtrl);
   return TRUE;
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
///                                   WINDOW PROCEDURES
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : wndprocCustomListView
// Description     : Provides 'Hot' ListView items
// 
ControlsAPI
LRESULT  wndprocCustomListView(HWND  hWnd, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   LVHITTESTINFO  oHitTest;
   WNDCLASS       oBaseClass;
   BOOL           bResult = FALSE;
   /*static HWND    hPrevFocus = NULL;*/

   switch (iMessage)
   {
   /// [MOUSE MOVE] Assign Hot Item + Track Mouse
   case WM_MOUSEMOVE:
      // Get focus
      /*if (GetFocus() != hWnd)
         hPrevFocus = SetFocus(hWnd);*/

      // Prepare
      utilTrackMouseEvent(hWnd, TME_LEAVE, 0);
      utilGetWindowCursorPos(hWnd, &oHitTest.pt);

      // [NOWHERE/HEADER] Clear focus, if any
      if (ListView_HitTest(hWnd, &oHitTest) == -1)
         ListView_SetItemState(hWnd, -1, NULL, LVIS_CUT)

      // [NEW ITEM] Move focus status to new item
      else if (!ListView_GetItemState(hWnd, oHitTest.iItem, LVIS_CUT))
      {
         ListView_SetItemState(hWnd, -1, NULL, LVIS_CUT)
         ListView_SetItemState(hWnd, oHitTest.iItem, LVIS_CUT, LVIS_CUT);
      }
      
      //CONSOLE("** Current Hot Item = %d", ListView_GetNextItem(hWnd, -1, LVNI_CUT));
      break;

   /// [MOUSE LEAVE] Cancel mouse tracking + Hot Item
   case WM_MOUSELEAVE:
      utilTrackMouseEvent(hWnd, TME_LEAVE | TME_CANCEL, 0);

      // Remove 'Hot' status from current hot item
      ListView_SetItemState(hWnd, -1, NULL, LVIS_CUT);

      //// Return focus
      //SetFocus(hPrevFocus);
      break;

   /// [CUSTOM COMBOBOX]
   case WM_DRAWITEM:  
   case WM_DELETEITEM: 
   case WM_COMPAREITEM:
   case WM_MEASUREITEM:
      switch (iMessage)
      {
      case WM_DRAWITEM:     bResult = onWindow_DrawItem((DRAWITEMSTRUCT*)lParam);                                  break;
      case WM_DELETEITEM:   bResult = onWindow_DeleteItem((DELETEITEMSTRUCT*)lParam);                              break;
      case WM_MEASUREITEM:  bResult = onWindow_MeasureComboBox((MEASUREITEMSTRUCT*)lParam, ITS_SMALL, ITS_SMALL);  break;

      case WM_COMPAREITEM:  return SendMessage(GetDlgItem(hWnd, wParam), WM_COMPAREITEM, wParam, lParam);          break;
      }

      if (bResult)
         return 0;
   }

   // Pass to base
   GetClassInfo(NULL, WC_LISTVIEW, &oBaseClass);
   return CallWindowProc(oBaseClass.lpfnWndProc, hWnd, iMessage, wParam, lParam);
}


/// Function name  : wndprocCustomListViewLabel
// Description     : Edit/ComboBox Custom ListView Label control
// 
LRESULT  wndprocCustomListViewLabel(HWND  hCtrl, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   static NMLVDISPINFO  oLabelData;
   COMPARE_COMBO_PROC   pfnCompareItem;
   WNDCLASS             oBaseClass;
   
   switch (iMessage)
   {
   /// [CREATE] Store the label data
   case WM_USER:
      // Copy item data
      oLabelData.item = *(LVITEM*)lParam;

      // Request ** ListView's Parent ** populate control
      oLabelData.hdr.code     = LVN_BEGINLABELEDIT;
      oLabelData.hdr.idFrom   = wParam;         /// Listview WindowID
      oLabelData.hdr.hwndFrom = hCtrl;          /// Label Handle
      SendMessage(GetParent(GetParent(hCtrl)), WM_NOTIFY, oLabelData.hdr.idFrom, (LPARAM)&oLabelData);

      // Prepare final notification
      oLabelData.hdr.code = LVN_ENDLABELEDIT;
      return 0;
      
   /// [LOST FOCUS] Destroy without notification
   case WM_KILLFOCUS:
      DestroyWindow(hCtrl);
      return 0;

   /// [COMBO-BOX CHANGED] Inform ListView *Parent* + Destroy window
   case WM_COMMAND:
      if (HIWORD(wParam) == CBN_SELCHANGE)
      {
         // [REQUEST] Request ListView *Parent* validate data
         if (SendMessage(GetParent(GetParent(hCtrl)), WM_NOTIFY, oLabelData.hdr.idFrom, (LPARAM)&oLabelData))
            // [VALID] Focus ListView
            SetFocus(GetParent(hCtrl));
         else // [INVALID] Beep!
            MessageBeep(MB_ICONERROR);
         return 0;
      }
      break;

   // [KEY PRESS]
   case WM_KEYDOWN:
      /// [ESCAPE] Destroy without notification
      if (wParam == VK_ESCAPE)
      {
         SetFocus(GetParent(hCtrl));
         return 0;
      }
      /// [ENTER] Inform ListView *Parent* + Destroy window
      else if (wParam == VK_RETURN AND oLabelData.item.lParam == LVLT_EDIT)
      {
         // [REQUEST] Request ListView *Parent* validate data
         if (SendMessage(GetParent(GetParent(hCtrl)), WM_NOTIFY, oLabelData.hdr.idFrom, (LPARAM)&oLabelData))
            // [VALID] Focus ListView
            SetFocus(GetParent(hCtrl));
         else // [INVALID] Beep!
            MessageBeep(MB_ICONERROR);
         return 0;
      }
      break;

   /// [KEYBOARD QUERY] -- Request all keyboard input
   case WM_GETDLGCODE:
      return DLGC_WANTALLKEYS;

   /// [COMPARE ITEM (Reflected)]
   case WM_COMPAREITEM:  
      pfnCompareItem = (COMPARE_COMBO_PROC)oLabelData.item.piColFmt;
      return (*pfnCompareItem)(wParam, (COMPAREITEMSTRUCT*)lParam);
   }

   // Pass to edit/combo base proc
   GetClassInfo(NULL, (TCHAR*)GetClassLong(hCtrl, GCW_ATOM), &oBaseClass);
   return CallWindowProc(oBaseClass.lpfnWndProc, hCtrl, iMessage, wParam, lParam);
}
