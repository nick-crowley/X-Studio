//
// Custom ComboBox.cpp : OwnerDraw ComboBox with icons and auxiliary item text
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                     MACROS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// OnException: Print to console
#define ON_EXCEPTION()     printException(pException)

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION  /  DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createCustomComboBoxItem
// Description     : Create a custom ComboBox item containing normal text and possibly auxiliary text and item data
// 
// CONST TCHAR*           szText    : [in]           Item text
// CONST TCHAR*           szAuxText : [in][optional] Auxiliary item text (Display on RHS)
// CONST TCHAR*           szIcon    : [in][optional] Resource ID of item icon
// LPARAM                 xItemData : [in][optional] Custom item data
// 
// Return Value   :  New CustomComboBox item data - you are responsible for destroying it
// 
CUSTOM_COMBO_ITEM*  createCustomComboBoxItem(CONST TCHAR*  szText, CONST TCHAR*  szAuxText, CONST TCHAR*  szIcon, LPARAM  xItemData)
{
   CUSTOM_COMBO_ITEM*  pNewItem;   // New item being created

   // Create new item
   pNewItem = utilCreateEmptyObject(CUSTOM_COMBO_ITEM);
   
   /// Set item text and data
   pNewItem->szMainText = utilDuplicateSimpleString(szText);
   pNewItem->szAuxText  = utilSafeDuplicateSimpleString(szAuxText);
   pNewItem->szIconID   = szIcon;
   pNewItem->lParam     = xItemData;
   pNewItem->clColour   = clNullColour;
   pNewItem->hFont      = NULL;

   // Return new data
   return pNewItem;
}


/// Function name  : deleteCustomComboBoxItem
// Description     : Destroy a custom ComboBox item
// 
// CUSTOM_COMBO_ITEM*  &pItem   : [in] Custom ComboBox item to destroy
// 
VOID  deleteCustomComboBoxItem(CUSTOM_COMBO_ITEM*  &pItem)
{
   // Delete strings
   utilDeleteString(pItem->szMainText);
   utilSafeDeleteString(pItem->szAuxText);

   // Delete font
   if (pItem->hFont)
      DeleteFont(pItem->hFont);

   // Delete calling object
   utilDeleteObject(pItem);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                      HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : appendCustomComboBoxItem
// Description     : Append a basic item with no icon to a Custom ComboBox
// 
// HWND          hCtrl     : [in] Custom ComboBox
// CONST TCHAR*  szText    : [in] Item text
// 
// Return Value   : Zero-based index of the new item
// 
ControlsAPI 
INT   appendCustomComboBoxItem(HWND  hCtrl, CONST TCHAR*  szText)
{
   CUSTOM_COMBO_ITEM*  pItemData;   // Item data
   INT                 iIndex;      // New index for appended item

   /// Create basic item data
   pItemData = createCustomComboBoxItem(szText, NULL, NULL, NULL);

   /// Insert item and associate item data
   iIndex = ComboBox_AddString(hCtrl, szText);
   ComboBox_SetItemData(hCtrl, iIndex, pItemData);

   // Return index of new item
   return iIndex;
   //return ComboBox_AddString(hCtrl, pItemData);
}


/// Function name  : appendCustomComboBoxItemEx
// Description     : Append a custom item with an icon and/or auxiliary text to a Custom ComboBox
// 
// HWND          hCtrl     : [in]           Custom ComboBox
// CONST TCHAR*  szText    : [in]           Item text
// CONST TCHAR*  szAuxText : [in][optional] Auxiliary item text (Display on RHS)
// CONST TCHAR*  szIcon    : [in][optional] Resource ID of item icon
// LPARAM        xItemData : [in][optional] Custom item data
// 
// Return Value   : Zero-based index of the new item
// 
ControlsAPI 
INT   appendCustomComboBoxItemEx(HWND  hCtrl, CONST TCHAR*  szText, CONST TCHAR*  szAuxText, CONST TCHAR*  szIcon, LPARAM  xItemData)
{
   CUSTOM_COMBO_ITEM*  pItemData;   // Item data
   INT                 iIndex;      // New index for appended item

   /// Create full item data
   pItemData = createCustomComboBoxItem(szText, szAuxText, szIcon, xItemData);

   /// Insert item and associate item data
   /*iIndex = ComboBox_AddString(hCtrl, szText);
   ComboBox_SetItemData(hCtrl, iIndex, pItemData);*/

   /// [OWNER DATA] Insert item and associate item data
   iIndex = ComboBox_AddString(hCtrl, pItemData);

   // Return index of new item
   return iIndex;
}


/// Function name  : appendCustomComboBoxFontItem
// Description     : Append a basic item with no icon to a Custom ComboBox
// 
// HWND          hCtrl  : [in] Custom ComboBox
// CONST TCHAR*  szText : [in] Item text
// HFONT         hFont  : [in] Item font
// 
// Return Value   : Zero-based index of the new item
// 
ControlsAPI 
INT   appendCustomComboBoxFontItem(HWND  hCtrl, CONST TCHAR*  szText, HFONT  hFont)
{
   CUSTOM_COMBO_ITEM*  pItemData;   // Item data
   INT                 iIndex;      // New index for appended item

   /// Create basic item data
   pItemData = createCustomComboBoxItem(szText, NULL, NULL, NULL);
   pItemData->hFont = hFont;

   /// Insert item and associate item data
   iIndex = ComboBox_AddString(hCtrl, szText);
   ComboBox_SetItemData(hCtrl, iIndex, pItemData);

   // Return index of new item
   return iIndex;
   //return ComboBox_AddString(hCtrl, pItemData);
}


/// Function name  : appendCustomComboBoxImageListItem
// Description     : Append an item with a custom image list and custom data
// 
// HWND          hCtrl      : [in] Custom ComboBox
// CONST TCHAR*  szText     : [in] Item text
// HIMAGELIST    hImageList : [in] Custom ImageList
// CONST UINT    iIconIndex : [in] ImageList icon index
// CONST UINT    iIndent    : [in] Indentation
// LPARAM        xItemData  : [in] Custom item data
// 
// Return Value   : Zero-based index of the new item
// 
ControlsAPI 
INT   appendCustomComboBoxImageListItem(HWND  hCtrl, CONST TCHAR*  szText, HIMAGELIST  hImageList, CONST UINT  iIconIndex, CONST UINT  iIndent, LPARAM  xItemData)
{
   CUSTOM_COMBO_ITEM*  pItemData;   // Item data
   INT                 iIndex;      // New index for appended item

   /// Create full item data
   pItemData = createCustomComboBoxItem(szText, NULL, NULL, xItemData);

   // Set properties
   pItemData->hImageList = hImageList;
   pItemData->iIconIndex = iIconIndex;
   pItemData->iIndent    = iIndent;

   /// Insert item and associate item data
   iIndex = ComboBox_AddString(hCtrl, szText);
   ComboBox_SetItemData(hCtrl, iIndex, pItemData);

   // Return index of new item
   return iIndex;
}


/// Function name  : calculateCustomComboBoxSize
// Description     : Calculates the item height of a custom combobox
// 
// MEASUREITEMSTRUCT*  pMeasureData   : [in] WM_MEASUREITEM message data
// 
BOOL  calculateCustomComboBoxSize(MEASUREITEMSTRUCT*  pMeasureData)
{
   /// All CustomComboBoxes are 24xPixels unless specifically overridden
   pMeasureData->itemHeight = ITS_MEDIUM;
   return TRUE;
}


/// Function name  : compareCustomComboBoxItems
// Description     : Compares two Custom ComboBox items
// 
// const UINT          iControl : [in] Control
// COMPAREITEMSTRUCT*  pData    : [in] Comparison Info
// 
// Return Value   : StrCmpI return codes
// 
ControlsAPI 
BOOL  compareCustomComboBoxItems(const UINT  iControl, COMPAREITEMSTRUCT*  pData)
{
   CUSTOM_COMBO_ITEM  *pItem1 = (CUSTOM_COMBO_ITEM*)pData->itemData1,
                      *pItem2 = (CUSTOM_COMBO_ITEM*)pData->itemData2;

   // Compare main text
   return StrCmpI(pItem1->szMainText, pItem2->szMainText);
}


/// Function name  : getCustomComboBoxItem
// Description     : Retrieves the item data from the specified CustomComboBox item
// 
// HWND        hCtrl   : [in] CustomComboBox handle
// CONST UINT  iIndex  : [in] Zero-based item index
// 
// Return Value   : Item data if found, otherwise NULL
// 
ControlsAPI 
CUSTOM_COMBO_ITEM*  getCustomComboBoxItem(HWND  hCtrl, CONST UINT  iIndex)
{
   // Retrieve item data
   return (CUSTOM_COMBO_ITEM*)ComboBox_GetItemData(hCtrl, iIndex);
}



/// Function name  : getCustomComboBoxItemParam
// Description     : Retrieves the parameter data from the specified CustomComboBox item
// 
// HWND        hCtrl   : [in] CustomComboBox handle
// CONST UINT  iIndex  : [in] Zero-based item index
// 
// Return Value   : Item parameter data, or NULL if not found
// 
ControlsAPI 
LPARAM  getCustomComboBoxItemParam(HWND  hCtrl, CONST UINT  iIndex)
{
   CUSTOM_COMBO_ITEM*  pItemData;   // Item data

   // Retrieve item data
   pItemData = (CUSTOM_COMBO_ITEM*)ComboBox_GetItemData(hCtrl, iIndex);

   // Return item text or NULL
   return (pItemData ? pItemData->lParam : NULL);
}


/// Function name  : getCustomComboBoxItemText
// Description     : Retrieves the text of the specified CustomComboBox item
// 
// HWND        hCtrl   : [in] CustomComboBox handle
// CONST UINT  iIndex  : [in] Zero-based item index
// 
// Return Value   : Item text or NULL if control is not a CustomComboBox
// 
ControlsAPI 
CONST TCHAR*  getCustomComboBoxItemText(HWND  hCtrl, CONST UINT  iIndex)
{
   CUSTOM_COMBO_ITEM*  pItemData;   // Item data

   // Retrieve item data
   pItemData = (CUSTOM_COMBO_ITEM*)ComboBox_GetItemData(hCtrl, iIndex);

   // Return item text or NULL
   return (pItemData ? pItemData->szMainText : NULL);
}


/// Function name  : setCustomComboBoxItemColour
// Description     : Sets the text colour of the specified CustomComboBox item
// 
// HWND            hCtrl        : [in] CustomComboBox handle
// CONST UINT      iIndex       : [in] Zero-based item index
// CONST COLORREF  clTextColour : [in] New text colour
// 
// Return Value   : TRUE if successful, otherwise FALSE
// 
ControlsAPI 
BOOL  setCustomComboBoxItemColour(HWND  hCtrl, CONST UINT  iIndex, CONST COLORREF  clTextColour)
{
   CUSTOM_COMBO_ITEM*  pItemData;   // Item data

   // Retrieve item data
   if (pItemData = (CUSTOM_COMBO_ITEM*)ComboBox_GetItemData(hCtrl, iIndex))
      // [FOUND]
      pItemData->clColour = clTextColour;

   // Return TRUE if found
   return (pItemData != NULL);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                      FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : drawCustomComboBoxItem
// Description     : Perform custom Combo-ListBox item drawing
// 
// DRAWITEMSTRUCT*    pDrawInfo  : [in] OwnerDraw paint data
// 
VOID  drawCustomComboBoxItem(DRAWITEMSTRUCT*  pDrawInfo, CONST CUSTOM_COMBO_ITEM*  pItemData)
{
   DC_STATE*         pPrevState;         // Previous DC state
   IMAGE_TREE_SIZE   eIconSize;          // Icon size
   COLORREF          clTextColour;
   RECT              rcBackgroundRect,   // Background drawing rectangle
                     rcTextRect,         // Text drawing rectangle
                     rcIconRect;         // Icon drawing rectangle
   BOOL              bEditItem;          // Whether the item is being drawn in the combo's edit control
   UINT              iIndentation;
   UINT              iDebugState = 0;

   /// Checkpoints added for linux user having access violations throughout this function
   TRY
   {
      // [CHECK] Ensure OwnerDraw data is for a ComboBox
      ASSERT(pDrawInfo->CtlType == ODT_COMBOBOX);

      // [VERBOSE]
      //VERBOSE("drawCustomComboBoxItem: ID=%d, Index=%d, Action=%d", pDrawInfo->CtlID, pDrawInfo->itemID, pDrawInfo->itemAction);
      iDebugState = 1;

      // Prepare
      bEditItem        = (pDrawInfo->itemState INCLUDES ODS_COMBOBOXEDIT ? TRUE : FALSE);
      eIconSize        = (IMAGE_TREE_SIZE)ComboBox_GetItemHeightEx(pDrawInfo->hwndItem, (bEditItem ? -1 : NULL));
      pPrevState       = utilCreateDeviceContextState(pDrawInfo->hDC);
      rcBackgroundRect = pDrawInfo->rcItem;
      iIndentation     = (bEditItem ? 0 : (eIconSize * pItemData->iIndent));

      // [DEBUG]
      iDebugState = 2;

      // Calculate icon and text rectangles
      SetRect(&rcIconRect, rcBackgroundRect.left + GetSystemMetrics(SM_CXEDGE), rcBackgroundRect.top, rcBackgroundRect.right,                             rcBackgroundRect.bottom);
      SetRect(&rcTextRect, rcIconRect.left + GetSystemMetrics(SM_CXEDGE) * 2,   rcIconRect.top,       rcIconRect.right - GetSystemMetrics(SM_CXEDGE) * 2, rcIconRect.bottom);

      // [ICON/INDENTATION] Shift text to the right
      if (pItemData->szIconID OR pItemData->hImageList)
         rcTextRect.left += eIconSize + iIndentation;

      // Prepare
      utilSetDeviceContextBackgroundMode(pPrevState, TRANSPARENT);
      
      // [CHECK] Is the item selected?
      if (pDrawInfo->itemState INCLUDES ODS_SELECTED)
      {
         /// [HOVER] Draw a light blue hover rectangle for selected items
         iDebugState = 3;
         drawCustomSelectionRectangle(pDrawInfo->hDC, &rcBackgroundRect);
      }
      else
      {
         // [DEFAULT] Draw a white background
         iDebugState = 4;
         utilFillSysColourRect(pDrawInfo->hDC, &pDrawInfo->rcItem, COLOR_WINDOW);   
      }

      /// [ICON] Draw icon (in grey if disabled)
      if (pItemData->szIconID)
      {
         iDebugState = 5;
         drawImageTreeIcon(eIconSize, pItemData->szIconID, pDrawInfo->hDC, rcIconRect.left + iIndentation, rcIconRect.top, pDrawInfo->itemState INCLUDES ODS_DISABLED ? IS_DISABLED : IS_NORMAL);
      }
      /// [IMAGELIST] Draw icon (in grey if disabled
      else if (pItemData->hImageList)
      {
         iDebugState = 6;
         drawIcon(pItemData->hImageList, pItemData->iIconIndex, pDrawInfo->hDC, rcIconRect.left + iIndentation, rcIconRect.top, pDrawInfo->itemState INCLUDES ODS_DISABLED ? IS_DISABLED : IS_NORMAL);
      }

      // [DEBUG]
      iDebugState = 7;

      // Set appropriate colour
      if ((pDrawInfo->itemState & ODS_DISABLED) OR pItemData->clColour == clNullColour)
         clTextColour = getThemeSysColour(TEXT("Window"), pDrawInfo->itemState & ODS_DISABLED ? COLOR_GRAYTEXT : COLOR_WINDOWTEXT);
      else
         clTextColour = pItemData->clColour;

      /// [TEXT] Draw main text on the left (in grey if disabled)
      utilSetDeviceContextFont(pPrevState, pItemData->hFont, clTextColour);
      DrawText(pDrawInfo->hDC, pItemData->szMainText, lstrlen(pItemData->szMainText), &rcTextRect, DT_LEFT WITH DT_VCENTER WITH DT_SINGLELINE);

      // [DEBUG]
      iDebugState = 8;

      /// [AUXILIARY] Draw auxiliary text (except when being displayed in the edit)
      if (pItemData->szAuxText AND !bEditItem)
      {
         SetTextColor(pDrawInfo->hDC, GetSysColor(COLOR_GRAYTEXT));
         DrawText(pDrawInfo->hDC, pItemData->szAuxText, lstrlen(pItemData->szAuxText), &rcTextRect, DT_RIGHT WITH DT_VCENTER WITH DT_SINGLELINE);
      }

      // [DEBUG]
      iDebugState = 9;

      // Cleanup
      utilDeleteDeviceContextState(pPrevState);
   }
   CATCH1("iDebugState=%d", iDebugState);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                   MESSAGE HANDLERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onOwnerDrawCustomComboBox
// Description     : Convenience handler for owner drawn CustomComboBoxes
// 
// DRAWITEMSTRUCT*    pDrawInfo  : [in] WM_DRAWITEM OwnerDraw message data
// 
BOOL  onOwnerDrawCustomComboBox(DRAWITEMSTRUCT*  pDrawInfo)
{
   BOOL   bResult = FALSE;

   TRY
   {
   // [CHECK] Abort if item data isn't provided (not sure why this happens anymore, thought it was when drawing in the edit)
   if (bResult = (pDrawInfo->itemData AND pDrawInfo->itemData != CB_ERR))
      /// Draw item
      drawCustomComboBoxItem(pDrawInfo, (CUSTOM_COMBO_ITEM*)pDrawInfo->itemData);
   }
   CATCH0("");
   
   // Return result
   return bResult;
}

 
/// Function name  : onWindow_CompareComboBoxItems
// Description     : Custom ComboBox comparitor stub
// 
// const UINT          iControl : [in] ComboBox ID
// COMPAREITEMSTRUCT*  pData    : [in] Data
// 
// Return Value   : 0
// 
ControlsAPI
BOOL  onWindow_CompareComboBoxItems(const UINT  iControl, COMPAREITEMSTRUCT*  pData)
{
   return 0;
}


/// Function name  : onWindow_MeasureComboBox
// Description     : Convenience handler for Custom ComboBoxes with custom sizes
// 
// MEASUREITEMSTRUCT*     pMeasureData  : [in] WM_MEASUREITEM message data
// CONST IMAGE_TREE_SIZE  eControlSize  : [in] Size of control
// CONST IMAGE_TREE_SIZE  eDropDownSize : [in] Size of dropdown items
// 
// Return Value   : TRUE if handled, otherwise FALSE
// 
ControlsAPI 
BOOL  onWindow_MeasureComboBox(MEASUREITEMSTRUCT*  pMeasureData, CONST IMAGE_TREE_SIZE  eControlSize, CONST IMAGE_TREE_SIZE  eDropDownSize)
{
   TRY
   {
      // [CHECK] Is control a ComboBox?
      if (pMeasureData->CtlType == ODT_COMBOBOX)
         // Display a small ComboBox with medium Icons
         pMeasureData->itemHeight = (pMeasureData->itemID == -1 ? eControlSize : eDropDownSize);
       
      // Return TRUE if handled
      return (pMeasureData->CtlType == ODT_COMBOBOX);
   }
   CATCH0("");
   return FALSE;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                   WINDOW PROCEDURES
/// ////////////////////////////////////////////////////////////////////////////////////////////////////
