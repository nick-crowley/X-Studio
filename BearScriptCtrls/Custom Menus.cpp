//
// Custom Menus.cpp : Owner-drawn menus in a Visual studio style
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                   CONSTANTS / GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

CONST UINT  iCustomMenuItemHeight      = 32,       // Height of a custom menu item
            iCustomMenuSeparatorHeight = 3,        // Height of a custom menu separator
            iCustomMenuGutterWidth     = GetSystemMetrics(SM_CXICON),       // The width of the grey square on the left of an item
            iCustomMenuEdgeWidth       = GetSystemMetrics(SM_CXEDGE) * 2,   // The distance between the grey square and the text of an item
            iCustomMenuTextOffset      = GetSystemMetrics(SM_CXICON) + 
                                         GetSystemMetrics(SM_CXEDGE) * 4,   // The distance between the left menu edge and the first character of the menu item
            iCustomMenuIconSize        = ITS_MEDIUM;


CONST COLORREF  clLightBlue = RGB(50,105,200),     // Used by CustomMenu
                clDarkBlue  = RGB(190,210,240),    // Used by CustomMenu
                clWhite     = RGB(255,255,255);

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION  /  DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createCustomMenu
// Description     : Creates data for a CustomMenu
// 
// CONST TCHAR*  szMenuResource : [in]         Resource ID of the menu
// CONST BOOL    bPopupMenu     : [in]         Whether this is a popup or top-level menu
// CONST UINT    iSubMenuIndex  : [in] [POPUP] Popup menu index
//
// Return Value   : New CustomMenu, you are responsible for destroying it
// 
ControlsAPI
CUSTOM_MENU*   createCustomMenu(CONST TCHAR*  szMenuResource, CONST BOOL  bPopupMenu, CONST UINT  iSubMenuIndex)
{
   CUSTOM_MENU*   pCustomMenu;       // Object being created

   // Create object
   pCustomMenu = utilCreateEmptyObject(CUSTOM_MENU);

   /// Load MenuBar
   pCustomMenu->hMenuBar = LoadMenu(getResourceInstance(), szMenuResource);
   ERROR_CHECK("creating custom menu", pCustomMenu->hMenuBar);

   // Lookup Popup menu
   pCustomMenu->hPopup = (bPopupMenu ? GetSubMenu(pCustomMenu->hMenuBar, iSubMenuIndex) : NULL);

   /// Generate custom menu data for MenuBar or Popup
   convertSystemMenuToCustomMenu(bPopupMenu ? pCustomMenu->hPopup : pCustomMenu->hMenuBar, bPopupMenu);

   // Return object
   return pCustomMenu;
}


/// Function name  : createCustomMenuItem
// Description     : Creates item data for a custom menu
// 
// CONST UINT  iItemID      : [in] Menu item ID
// CONST UINT  iIconIndex   : [in] Icon index
// CONST BOOL  bIsHeading   : [in] Whether item is a top-level menu heading
// CONST BOOL  bIsPopup     : [in] Whether item is a popup menu item
// CONST BOOL  bIsSeparator : [in] Whether item is a separator
// 
// Return Value   : New custom menu item data
// 
CUSTOM_MENU_ITEM*  createCustomMenuItem(CONST UINT  iItemID, CONST UINT  iIconIndex, CONST BOOL  bIsHeading, CONST BOOL  bIsPopup, CONST BOOL  bIsSeparator)
{
   CUSTOM_MENU_ITEM*  pMenuItem;

   // Create object
   pMenuItem = utilCreateEmptyObject(CUSTOM_MENU_ITEM);

   // Set properties
   pMenuItem->iItemID      = iItemID;
   pMenuItem->iIconIndex   = iIconIndex;
   pMenuItem->bIsHeading   = bIsHeading;
   pMenuItem->bIsPopup     = bIsPopup;
   pMenuItem->bIsSeparator = bIsSeparator;

   // Return object
   return pMenuItem;
}


/// Function name  : deleteCustomMenu
// Description     : Destroys the data for the CustomMenu
// 
// CUSTOM_MENU*  &pCustomMenu   : [in] CustomMenu to destroy
// 
ControlsAPI
VOID  deleteCustomMenu(CUSTOM_MENU*  &pCustomMenu)
{
   /// Delete custom menu data for MenuBar or Popup
   convertCustomMenuToSystemMenu(pCustomMenu->hPopup ? pCustomMenu->hPopup : pCustomMenu->hMenuBar);

   /// Unload menu
   DestroyMenu(pCustomMenu->hMenuBar);

   // Destroy calling object
   utilDeleteObject(pCustomMenu);
}


/// Function name  : deleteCustomMenuItemData
// Description     : Destroys CustomMenuItem data
// 
// CUSTOM_MENU_ITEM*  &pCustomMenuItem   : [in] CustomMenuItem to destroy
// 
VOID  deleteCustomMenuItemData(CUSTOM_MENU_ITEM*  &pCustomMenuItem)
{
   // [OPTIONAL] Delete text (separators have no text)
   utilSafeDeleteString(pCustomMenuItem->szText);

   // Delete calling object
   utilDeleteObject(pCustomMenuItem);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                      HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : getCustomMenuItemData
// Description     : Get the CustomMenu data stored with a CustomMenu menu item
// 
// HMENU       hMenu       : [in] Custom menu handle
// CONST UINT  iItem       : [in] Menu item identifier - either a command ID or the the zero-based postiion
// CONST BOOL  bByPosition : [in] TRUE if 'iItem' is a position, FALSE for a Command ID
// 
// Return Value   : CustomMenu item data
// 
ControlsAPI 
CUSTOM_MENU_ITEM*  getCustomMenuItemData(HMENU  hMenu, CONST UINT  iItem, CONST BOOL  bByPosition)
{
   MENUITEMINFO    oItemAttributes;   // Existing item attributes

   // Prepare
   utilZeroObject(&oItemAttributes, MENUITEMINFO);
   oItemAttributes.cbSize = sizeof(MENUITEMINFO);
   oItemAttributes.fMask  = MIIM_DATA;
   
   // Query menu item and return the stored item data
   GetMenuItemInfo(hMenu, iItem, bByPosition, &oItemAttributes);

   // Return stored item data
   ASSERT(oItemAttributes.dwItemData);
   return (CUSTOM_MENU_ITEM*)oItemAttributes.dwItemData;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                      FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : appendCustomMenuItem
// Description     : Appends a new item to a custom menu
// 
// HMENU         hMenu      : [in] Custom Menu handle
// CONST UINT    iItemID    : [in] Menu item ID
// CONST UINT    iIconIndex : [in] Zero-based icon index
// CONST TCHAR*  szText     : [in] Item text
// 
// Return Value   : TRUE if succesful, otherwise FALSE
// 
ControlsAPI
BOOL  appendCustomMenuItem(HMENU  hMenu, CONST UINT  iItemID, CONST UINT  iIconIndex, CONST UINT  iState, CONST TCHAR*  szText)
{
   CUSTOM_MENU_ITEM*  pMenuItem;     // Custom item data

   /// Create custom menu item data
   pMenuItem = createCustomMenuItem(iItemID, iIconIndex, FALSE, FALSE, FALSE);
   pMenuItem->szText = utilDuplicateSimpleString(szText);

   // [CHECK] Append to input menu
   if (!AppendMenu(hMenu, MF_OWNERDRAW WITH iState, iItemID, (CONST TCHAR*)pMenuItem))
      // [FAILED] Cleanup
      deleteCustomMenuItemData(pMenuItem);

   // Return TRUE if successful
   return (pMenuItem != NULL);
}


/// Function name  : calculateCustomMenuItemSize
// Description     : Calculate the width and height of a custom menu item
// 
// HWND                hParentWnd    : [in]  Window handle of the window that owns the menu
// MEASUREITEMSTRUCT*  pMeasureData  : [out] Menu item size
//
ControlsAPI 
BOOL   calculateCustomMenuItemSize(HWND  hParentWnd, MEASUREITEMSTRUCT*  pMeasureData)
{
   CUSTOM_MENU_ITEM*  pItemData;    // Custom menu item data
   HFONT              hOldFont;     //
   SIZE               siTextSize;   // Size of the item text
   RECT               rcTextRect;   // Rectangle of the item text
   HDC                hDC;          // Parent window's device context
   BOOL               bResult;

   // Prepare
   TRACK_FUNCTION();
   bResult = FALSE;

   // [CHECK] Requested item to measure is a menu
   if (pMeasureData->CtlType == ODT_MENU AND pMeasureData->itemData)
   {
      // Get custom item data
      pItemData = (CUSTOM_MENU_ITEM*)pMeasureData->itemData;
      bResult   = TRUE;

      /// [ITEMS / HEADINGS]- Measure the text (using the parent's DC)
      if (!pItemData->bIsSeparator)
      {
         // Prepare
         hDC      = GetDC(hParentWnd);
         hOldFont = SelectFont(hDC, utilCreateFont(hDC, TEXT("MS Shell Dlg 2"), 9, FALSE, FALSE, FALSE));

         // Measure Text
         DrawText(hDC, pItemData->szText, lstrlen(pItemData->szText), &rcTextRect, DT_LEFT WITH DT_SINGLELINE WITH DT_VCENTER WITH DT_EXPANDTABS WITH DT_CALCRECT);
         utilConvertRectangleToSize(&rcTextRect, &siTextSize);
         
         // Output size.  Append the 'text offset' for non-heading items
         pMeasureData->itemWidth  = siTextSize.cx + (pItemData->bIsHeading ? 0 : iCustomMenuTextOffset);
         pMeasureData->itemHeight = iCustomMenuItemHeight;

         // Release
         DeleteFont(SelectFont(hDC, hOldFont));
         ReleaseDC(hParentWnd, hDC);
      }
      /// [SEPARATORS] - Return a fixed size of 32x3 pixels
      else
      {
         pMeasureData->itemWidth  = 32;
         pMeasureData->itemHeight = iCustomMenuSeparatorHeight;
      }
   }

   // [TRACK]
   END_TRACKING();
   return bResult;
}


/// Function name  : convertCustomMenuToSystemMenu
// Description     : Deletes the stored item data associated with each menu item in a custom menu. Despite the
//                     name it does not destroy the menu handle.
// 
// HMENU  hCustomMenu : [in] Menu handle of the custom menu who's data is to be deleted
// 
ControlsAPI 
VOID    convertCustomMenuToSystemMenu(HMENU  hCustomMenu)
{
   CUSTOM_MENU_ITEM*  pCustomData;     // Menu Item data
   MENUITEMINFO       oItemData;       // Menu item attributes

   // Prepare
   utilZeroObject(&oItemData, MENUITEMINFO);
   oItemData.fMask  = MIIM_DATA WITH MIIM_SUBMENU;
   oItemData.cbSize = sizeof(MENUITEMINFO);

   /// Iterate through menu items
   for (INT  iItemIndex = 0; iItemIndex < GetMenuItemCount(hCustomMenu); iItemIndex++)
   {
      // Get item data
      GetMenuItemInfo(hCustomMenu, iItemIndex, TRUE, &oItemData);
      pCustomData = (CUSTOM_MENU_ITEM*)oItemData.dwItemData;
      
      /// [POP-UP ITEMS] Recurse into item
      if (oItemData.hSubMenu)
         convertCustomMenuToSystemMenu(oItemData.hSubMenu);
      
      // Sever item data
      oItemData.hSubMenu   = NULL;
      oItemData.dwItemData = NULL;
      SetMenuItemInfo(hCustomMenu, iItemIndex, TRUE, &oItemData);

      // Delete item data
      deleteCustomMenuItemData(pCustomData);
   }
}


/// Function name  : convertSystemMenuToCustomMenu
// Description     : Create a custom menu from an existing menu by adding the owner draw flag to each item (and
//                    the items of any pop-up menus).  This also associates custom data with each item so the
//                    menu must later be destroyed with convertCustomMenuToSystemMenu(..)
// 
// HMENU       hSystemMenu : [in] Handle of the menu to convert
// CONST BOOL  bPopupMenu  : [in] Whether this is a popup menu
///                                       Top level menus must pass FALSE so their headings are painted correctly
// 
ControlsAPI 
VOID    convertSystemMenuToCustomMenu(HMENU  hSystemMenu, CONST BOOL  bPopupMenu)
{
   CUSTOM_MENU_ITEM*  pCustomData;     // Custom item data
   MENUITEMINFO       oItemData;       // Existing item attributes

   // Prepare
   utilZeroObject(&oItemData, MENUITEMINFO);
   oItemData.cbSize = sizeof(MENUITEMINFO);
   oItemData.fMask  = MIIM_FTYPE WITH MIIM_SUBMENU WITH MIIM_ID;

   // Iterate through items
   for (INT  iItemIndex = 0; iItemIndex < GetMenuItemCount(hSystemMenu); iItemIndex++)
   {
      // Get menu item attributes
      GetMenuItemInfo(hSystemMenu, iItemIndex, TRUE, &oItemData);

      // Create custom menu item
      pCustomData = createCustomMenuItem(oItemData.wID, identifyCustomMenuIconByID(oItemData.wID), !bPopupMenu, oItemData.hSubMenu != NULL, oItemData.fType INCLUDES MFT_SEPARATOR ? TRUE : FALSE);
      
      // [ITEM] Set item string
      if (!pCustomData->bIsSeparator)
      {
         pCustomData->szText = utilCreateString(64);
         GetMenuString(hSystemMenu, iItemIndex, pCustomData->szText, 64, MF_BYPOSITION);
      }
      
      /// Store item data and add owner draw flag
      ModifyMenu(hSystemMenu, iItemIndex, MF_BYPOSITION WITH MF_OWNERDRAW, pCustomData->iItemID, (TCHAR*)pCustomData);

      // [POP-UP MENU] - Recurse into menu item
      if (pCustomData->bIsPopup)
         convertSystemMenuToCustomMenu(oItemData.hSubMenu, TRUE);
   }
}


/// Function name  : drawCustomMenuItem
// Description     : Draws an item from a custom menu: separator, heading or item
// 
// DRAWITEMSTRUCT*    pDrawData   : [in] OwnerDraw paint data
// CUSTOM_MENU_ITEM*  pItemData   : [in] CustomMenu item data
//
VOID   drawCustomMenu(DRAWITEMSTRUCT*  pDrawData, CONST CUSTOM_MENU_ITEM*  pItemData)
{
   RECT       rcDrawRect;            // Draw rectangle

   // Prepare
   TRACK_FUNCTION();
   rcDrawRect = pDrawData->rcItem;

   // [CHECK] Owner Draw Menus only
   ASSERT(pDrawData->CtlType == ODT_MENU);

   /// [BACKGROUND] Draw a light grey square followed by a dark grey line followed by a white box
   // Draw a light grey square on the left
   rcDrawRect.right = rcDrawRect.left + iCustomMenuGutterWidth;
   utilFillSysColourRect(pDrawData->hDC, &rcDrawRect, COLOR_BTNFACE);

   // Draw a vertical dark grey line one pixel wide
   rcDrawRect.left = rcDrawRect.right - 1;
   utilFillStockRect(pDrawData->hDC, &rcDrawRect, LTGRAY_BRUSH);

   // Draw remaining item background in white
   rcDrawRect       = pDrawData->rcItem;
   rcDrawRect.left += iCustomMenuGutterWidth;
   utilFillStockRect(pDrawData->hDC, &rcDrawRect, WHITE_BRUSH);

   /// [NORMAL MENU ITEM] - Draw a full custom menu item
   if (!pItemData->bIsHeading AND !pItemData->bIsSeparator)
      drawCustomMenuItem(pDrawData, pItemData);

   /// [HEADINGS] -- Draw a coloured box with the heading title inside
   else if (pItemData->bIsHeading AND !pItemData->bIsSeparator)
      drawCustomMenuHeading(pDrawData, pItemData);
   
   /// [SEPARATORS] -- Draw a light grey line a single pixel high
   else
   {
      // Calculate draw rect
      rcDrawRect        = pDrawData->rcItem;
      rcDrawRect.left  += iCustomMenuTextOffset;
      rcDrawRect.right -= iCustomMenuEdgeWidth;
      rcDrawRect.bottom = rcDrawRect.top + 1; 

      // Draw as a light grey line
      FillRect(pDrawData->hDC, &rcDrawRect, (HBRUSH)GetStockObject(LTGRAY_BRUSH));
   }

   // Cleanup
   END_TRACKING();
}


/// Function name  : drawCustomMenuItem
// Description     : Draw a visual-studio style custom menu item
// 
// DRAWITEMSTRUCT*    pDrawData   : [in] OwnerDraw paint data
// CUSTOM_MENU_ITEM*  pItemData   : [in] CustomMenu item data
//
VOID   drawCustomMenuItem(DRAWITEMSTRUCT*  pDrawData, CONST CUSTOM_MENU_ITEM*  pItemData)
{
   DC_STATE  *pPrevState;            // Previous DC state
   HPEN       hBluePen;              // Pen used for drawing menu heading outline
   HBRUSH     hBlueBrush;            // Fill colour for menu headings
   RECT       rcDrawRect;            // Draw rectangle
   HFONT      hBoldFont;             // Bold font used for default items
   TCHAR     *szItemText,            // Working copy of the menu text to be tokenised
             *szItemToken,           // Tokenised string of menu item text
             *pIterator;             // Tokeniser data

   // [CHECK] Owner Draw Menus only
   ASSERT(pDrawData->CtlType == ODT_MENU);

   // Prepare
   SetRect(&rcDrawRect, pDrawData->rcItem.left + iCustomMenuGutterWidth + 1, pDrawData->rcItem.top, pDrawData->rcItem.right, pDrawData->rcItem.bottom);

   // Prepare a light blue brush and a dark blue pen
   hBluePen    = CreatePen(PS_SOLID, 1, clLightBlue);
   hBlueBrush  = CreateSolidBrush(clDarkBlue);
   hBoldFont   = utilDuplicateFont(pDrawData->hDC, TRUE, FALSE, FALSE);
   pPrevState  = utilCreateDeviceContextState(pDrawData->hDC);

   // [CHECK]
   if (!hBoldFont)
      hBoldFont = utilDuplicateFontEx(pDrawData->hDC, TEXT("MS Shell Dlg 2"), TRUE, FALSE, FALSE);

   /// [HOT ITEMS that aren't DISABLED] Draw a shaded light blue box
   if ((pDrawData->itemState INCLUDES ODS_SELECTED) AND !(pDrawData->itemState INCLUDES ODS_DISABLED))
      drawCustomSelectionRectangle(pDrawData->hDC, &rcDrawRect);

   // Calculate centred icon rectangle
   utilSetRectangle(&rcDrawRect, pDrawData->rcItem.left, pDrawData->rcItem.top, iCustomMenuIconSize, iCustomMenuIconSize);
   OffsetRect(&rcDrawRect, iCustomMenuEdgeWidth, iCustomMenuEdgeWidth);

   /// [CHECK MARK] Draw a light blue box
   if (pDrawData->itemState INCLUDES ODS_CHECKED)
   {
      // Contract drawing rectangle and activate the dark blue pen
      InflateRect(&rcDrawRect, 2, 2);
      utilSetDeviceContextPen(pPrevState, hBluePen);
      utilSetDeviceContextBrush(pPrevState, hBlueBrush);

      // [SELECTED] Draw with a white interior instead
      if (pDrawData->itemState INCLUDES ODS_SELECTED)
         SetDCBrushColor(pDrawData->hDC, clWhite);
      
      // Draw rounded rectangle
      RoundRect(pDrawData->hDC, rcDrawRect.left, rcDrawRect.top, rcDrawRect.right, rcDrawRect.bottom, 2, 2);
      InflateRect(&rcDrawRect, -2, -2);
   }

   /// [ICON] Draw an icon with appropriate shading if item is selected / disabled
   if (pDrawData->itemState INCLUDES ODS_DISABLED)
      // [DISABLED] Draw disabled
      drawIcon(getAppImageList(ITS_MEDIUM), pItemData->iIconIndex, pDrawData->hDC, rcDrawRect.left, rcDrawRect.top, IS_DISABLED);

   else if (!pItemData->bIsPopup)
      // [ITEM] Draw normal or selected
      drawIcon(getAppImageList(ITS_MEDIUM), pItemData->iIconIndex, pDrawData->hDC, rcDrawRect.left, rcDrawRect.top, pDrawData->itemState INCLUDES ODS_SELECTED ? IS_SELECTED : IS_NORMAL);
   
   // Prepare
   utilSetDeviceContextBackgroundMode(pPrevState, TRANSPARENT);
   utilSetDeviceContextFont(pPrevState, (pItemData->bDefault ? hBoldFont : NULL), GetSysColor(pDrawData->itemState INCLUDES ODS_DISABLED ? COLOR_GRAYTEXT : COLOR_WINDOWTEXT));

   // Calculate drawing rectangle
   rcDrawRect               = pDrawData->rcItem;
   pDrawData->rcItem.left  += iCustomMenuTextOffset;
   pDrawData->rcItem.right -= iCustomMenuEdgeWidth;

   // Retrieve item text
   szItemText  = utilDuplicateString(pItemData->szText, lstrlen(pItemData->szText));
   szItemToken = utilTokeniseString(szItemText, "\t", &pIterator);

   /// [TEXT] Draw the menu item text on the LHS 
   DrawText(pDrawData->hDC, szItemToken, lstrlen(szItemToken), &pDrawData->rcItem, DT_LEFT WITH DT_SINGLELINE WITH DT_VCENTER);

   // [CHECK] Is there an accelerator?
   if (szItemToken = utilGetNextToken("\t", &pIterator))
      /// [ACCELERATOR] Draw the accelerator on the RHS
      DrawText(pDrawData->hDC, szItemToken, lstrlen(szItemToken), &pDrawData->rcItem, DT_RIGHT WITH DT_SINGLELINE WITH DT_VCENTER);      

   // Cleanup
   utilDeleteString(szItemText);
   utilDeleteDeviceContextState(pPrevState);
   DeletePen(hBluePen);
   DeleteFont(hBoldFont);
   //DeleteFont(hNormalFont);
   DeleteBrush(hBlueBrush);
}


/// Function name  : drawCustomMenuItem
// Description     : Draw a visual-studio style custom menu item
// 
// DRAWITEMSTRUCT*    pDrawData   : [in] OwnerDraw paint data
// CUSTOM_MENU_ITEM*  pItemData   : [in] CustomMenu item data
//
VOID   drawCustomMenuHeading(DRAWITEMSTRUCT*  pDrawData, CONST CUSTOM_MENU_ITEM*  pItemData)
{
   DC_STATE*  pPrevState;            // Previous DC state
   HPEN       hBluePen;              // Pen used for drawing menu heading outline
   HBRUSH     hBlueBrush;            // Fill colour for menu headings
   RECT       rcDrawRect;            // Draw rectangle

   // [CHECK] Owner Draw Menus only
   ASSERT(pDrawData->CtlType == ODT_MENU);

   // Prepare a light blue brush and a dark blue pen
   hBluePen   = CreatePen(PS_SOLID, 1, clLightBlue);
   hBlueBrush = CreateSolidBrush(clDarkBlue);
   pPrevState = utilCreateDeviceContextState(pDrawData->hDC);

   // Prepare
   rcDrawRect = pDrawData->rcItem;

   /// [BACKGROUND] -- Draw a two coloured rounded rectangles, or just a grey background
   if ((pDrawData->itemState INCLUDES ODS_SELECTED) OR (pDrawData->itemState INCLUDES ODS_HOTLIGHT))
   {
      // Setup DC
      utilSetDeviceContextPen(pPrevState, GetStockPen(NULL_PEN));
      utilSetDeviceContextBrush(pPrevState, GetStockBrush(WHITE_BRUSH));

      // Draw white rounded rectangle with no border
      RoundRect(pDrawData->hDC, rcDrawRect.left, rcDrawRect.top, rcDrawRect.right, rcDrawRect.bottom, 2, 2);

      // Contract drawing rectangle and activate the dark blue pen
      InflateRect(&rcDrawRect, -1, -1);
      utilSetDeviceContextPen(pPrevState, hBluePen);

      // [HOT] Draw rounded rectangle with dark blue border and light blue fill colour.
      if (pDrawData->itemState INCLUDES ODS_SELECTED)
         RoundRect(pDrawData->hDC, rcDrawRect.left, rcDrawRect.top, rcDrawRect.right, rcDrawRect.bottom, 2, 2);
      
      // [SELECTED] Draw rounded rectangle with dark blue border and white fill colour.
      else
      {
         utilSetDeviceContextBrush(pPrevState, hBlueBrush);
         RoundRect(pDrawData->hDC, rcDrawRect.left, rcDrawRect.top, rcDrawRect.right, rcDrawRect.bottom, 2, 2);
      }
   }
   else
      FillRect(pDrawData->hDC, &rcDrawRect, GetSysColorBrush(COLOR_BTNFACE));

   /// [TEXT] Draw text
   utilSetDeviceContextBackgroundMode(pPrevState, TRANSPARENT);
   DrawText(pDrawData->hDC, pItemData->szText, lstrlen(pItemData->szText), &rcDrawRect, DT_CENTER WITH DT_SINGLELINE WITH DT_VCENTER);
   
   // Cleanup
   utilDeleteDeviceContextState(pPrevState);
   DeleteBrush(hBlueBrush);
   DeletePen(hBluePen);
}


/// Function name  : emptyCustomMenu
// Description     : Removes and destroys all custom/standard items from a menu
// 
// HMENU   hMenu  : [in] Custom menu handle
// 
// Return Value   : TRUE if successful, otherwise FALSE
// 
ControlsAPI 
BOOL  emptyCustomMenu(HMENU  hMenu)
{
   CUSTOM_MENU_ITEM*  pItem;        // Item data associated with the target item
   BOOL               bResult;      // Operation result

   // Prepare
   bResult = TRUE;

   /// Iterate through all menu items
   while (GetMenuItemCount(hMenu))
   {
      // [CHECK] Lookup custom menu item data
      if (pItem = getCustomMenuItemData(hMenu, 0, TRUE))
         // [FOUND] Destroy item data
         deleteCustomMenuItemData(pItem);

      /// Destroy menu item
      bResult = DeleteMenu(hMenu, 0, MF_BYPOSITION);
   }

   // Return result
   return bResult;
}


/// Function name  : identifyCustomMenuIconByID
// Description     : Identify the appropriate icon for a menu item
// 
// CONST UINT  iCommandID   : [in] Command ID for the menu item
// 
// Return Value   : Icon index (into the menu item's ImageList)
// 
ControlsAPI 
UINT  identifyCustomMenuIconByID(CONST UINT  iCommandID)
{
   CONST TCHAR*  szIconID;

   /// Determine icon ID
   switch (iCommandID)
   {
   // [DEBUG]
   default:                                  szIconID = TEXT("MISSING_ICON");             break;

   /// [MAIN WINDOW]
   case IDM_FILE_NEW:                        szIconID = TEXT("NEW_FILE_ICON");            break;  
   case IDM_FILE_NEW_SCRIPT:                 szIconID = TEXT("NEW_SCRIPT_FILE_ICON");     break;
   case IDM_FILE_NEW_LANGUAGE:               szIconID = TEXT("NEW_LANGUAGE_FILE_ICON");   break;
   case IDM_FILE_NEW_MISSION:                szIconID = TEXT("NEW_MISSION_FILE_ICON");    break;
   case IDM_FILE_NEW_PROJECT:                szIconID = TEXT("NEW_PROJECT_FILE_ICON");    break;
   case IDM_FILE_OPEN:                       szIconID = TEXT("OPEN_FILE_ICON");           break;
   case IDM_FILE_BROWSE:                     szIconID = TEXT("OPEN_FILE_ICON");           break;
   case IDM_FILE_CLOSE:                      szIconID = TEXT("CLOSE_DOCUMENT_ICON");      break;
   case IDM_FILE_CLOSE_PROJECT:              szIconID = TEXT("CLOSE_PROJECT_ICON");       break;
   case IDM_FILE_SAVE:                       szIconID = TEXT("SAVE_FILE_ICON");           break;
   case IDM_FILE_EXPORT_PROJECT:             szIconID = TEXT("SAVE_PROJECT_ICON");        break;
   case IDM_FILE_SAVE_AS:                    szIconID = TEXT("SAVE_FILE_AS_ICON");        break;
   case IDM_FILE_SAVE_ALL:                   szIconID = TEXT("SAVE_ALL_FILES_ICON");      break;
   case IDM_FILE_EXIT:                       szIconID = TEXT("QUIT_ICON");                break;

   case IDM_EDIT_COPY:                       szIconID = TEXT("COPY_ICON");                break;
   case IDM_EDIT_CUT:                        szIconID = TEXT("CUT_ICON");                 break;
   case IDM_EDIT_PASTE:                      szIconID = TEXT("PASTE_ICON");               break;
   case IDM_EDIT_DELETE:                     szIconID = TEXT("DELETE_ICON");              break;
   case IDM_EDIT_FIND:                       szIconID = TEXT("FIND_ICON");                break;
   case IDM_EDIT_SELECT_ALL:                 szIconID = TEXT("SELECT_ALL_ICON");          break; 
   case IDM_EDIT_COMMENT:                    szIconID = TEXT("COMMENT_ICON");             break; 
   case IDM_EDIT_UNDO:                       szIconID = TEXT("UNDO_ICON");                break;
   case IDM_EDIT_REDO:                       szIconID = TEXT("REDO_ICON");                break;

   case IDM_VIEW_COMPILER_OUTPUT:            szIconID = TEXT("OUTPUT_WINDOW_ICON");       break;
   case IDM_VIEW_COMMAND_LIST:               szIconID = TEXT("COMMAND_LIST_ICON");        break;
   case IDM_VIEW_GAME_OBJECTS_LIST:          szIconID = TEXT("GAME_OBJECT_ICON");         break;
   case IDM_VIEW_SCRIPT_OBJECTS_LIST:        szIconID = TEXT("SCRIPT_OBJECT_ICON");       break;
   case IDM_VIEW_DOCUMENT_PROPERTIES:        szIconID = TEXT("PROPERTIES_ICON");          break;
   case IDM_VIEW_PROJECT_EXPLORER:           szIconID = TEXT("PROJECT_EXPLORER_ICON");    break;
   case IDM_VIEW_PREFERENCES:                szIconID = TEXT("PREFERENCES_ICON");         break;
   
   case IDM_TOOLS_GAME_STRINGS:              szIconID = TEXT("NEW_LANGUAGE_FILE_ICON");   break;   
   case IDM_TOOLS_MEDIA_BROWSER:             szIconID = TEXT("OPEN_SAMPLES_ICON");        break;
   case IDM_TOOLS_MISSION_HIERARCHY:         szIconID = TEXT("MISSING_ICON");             break;   // Not Implemented
   case IDM_TOOLS_CONVERSATION_BROWSER:      szIconID = TEXT("MISSING_ICON");             break;   // Not Implemented
   case IDM_TOOLS_RELOAD_GAME_DATA:          szIconID = TEXT("REFRESH_ICON");             break;
   
   case IDM_WINDOW_CLOSE_ALL_DOCUMENTS:      szIconID = TEXT("CLOSE_ALL_DOCUMENTS_ICON"); break;
   case IDM_WINDOW_CLOSE_OTHER_DOCUMENTS:    szIconID = TEXT("CLOSE_DOCUMENT_ICON");      break;

   case IDM_HELP_FORUMS:                     szIconID = TEXT("FORUMS_ICON");              break;
   case IDM_HELP_UPDATES:                    szIconID = TEXT("UPDATE_ICON");              break;
   case IDM_HELP_SUBMIT_FILE:                szIconID = TEXT("SUBMIT_REPORT_ICON");       break;
   case IDM_HELP_HELP:                       szIconID = TEXT("HELP_ICON");                break;
   case IDM_HELP_ABOUT:                      szIconID = TEXT("ABOUT_ICON");               break;
   case IDM_HELP_TUTORIAL_OPEN_FILE:         szIconID = TEXT("OPEN_FILE_ICON");           break;
   case IDM_HELP_TUTORIAL_FILE_OPTIONS:      szIconID = TEXT("HELP_ICON");                break;
   case IDM_HELP_TUTORIAL_GAME_DATA:         szIconID = TEXT("PREFERENCES_ICON");         break;
   case IDM_HELP_TUTORIAL_GAME_FOLDER:       szIconID = TEXT("PREFERENCES_ICON");         break;
   case IDM_HELP_TUTORIAL_GAME_OBJECTS:      szIconID = TEXT("GAME_OBJECT_ICON");         break;
   case IDM_HELP_TUTORIAL_EDITOR:            szIconID = TEXT("NEW_SCRIPT_FILE_ICON");     break;
   case IDM_HELP_TUTORIAL_PROJECTS:          szIconID = TEXT("PROJECT_EXPLORER_ICON");    break;
   case IDM_HELP_TUTORIAL_COMMANDS:          szIconID = TEXT("COMMAND_LIST_ICON");        break;
   case IDM_HELP_TUTORIAL_SCRIPT_OBJECTS:    szIconID = TEXT("SCRIPT_OBJECT_ICON");       break;
   
   /// [OUTPUT DIALOG]
   case IDM_OUTPUT_CLEAR:                    szIconID = TEXT("DELETE_ICON");              break;

   /// [PROJECT DIALOG]
   case IDM_PROJECT_REMOVE_DOCUMENT:         szIconID = TEXT("REMOVE_PROJECT_FILE_ICON"); break;
   case IDM_PROJECT_OPEN_DOCUMENT:           szIconID = TEXT("OPEN_FILE_ICON");           break;
   case IDM_PROJECT_VARIABLES_ADD:           szIconID = TEXT("ADD_VARIABLE_ICON");        break;
   case IDM_PROJECT_VARIABLES_REMOVE:        szIconID = TEXT("REMOVE_VARIABLE_ICON");     break;
   case IDM_PROJECT_ADD_ALL_DOCUMENTS:       szIconID = TEXT("INSERT_PROJECT_FILE_ICON"); break;
   case IDM_PROJECT_EDIT_VARIABLES:          szIconID = TEXT("VARIABLE_ICON");            break;

   /// [RESULTS DIALOG]
   case IDM_RESULTS_INSERT_RESULT:           szIconID = TEXT("INSERT_RESULT_ICON");       break;
   case IDM_RESULTS_MSCI_LOOKUP:             szIconID = TEXT("HELP_ICON");                break;
   case IDM_RESULTS_SUBMIT_CORRECTION:       szIconID = TEXT("SUBMIT_REPORT_ICON");       break;

   /// [DOCUMENTS CONTROL]
   case IDM_DOCUMENT_ADD_PROJECT:            szIconID = TEXT("INSERT_PROJECT_FILE_ICON"); break;
   case IDM_DOCUMENT_REMOVE_PROJECT:         szIconID = TEXT("REMOVE_PROJECT_FILE_ICON"); break;
      
   /// [CODE EDIT]
   /*case IDM_CODE_EDIT_CUT:                   szIconID = TEXT("CUT_ICON");                 break;
   case IDM_CODE_EDIT_COPY:                  szIconID = TEXT("COPY_ICON");                break;
   case IDM_CODE_EDIT_PASTE:                 szIconID = TEXT("PASTE_ICON");               break;
   case IDM_CODE_EDIT_DELETE:                szIconID = TEXT("DELETE_ICON");              break;*/
   case IDM_CODE_EDIT_VIEW_SUGGESTIONS:      szIconID = TEXT("GAME_OBJECT_ICON");         break;
   case IDM_CODE_EDIT_LOOKUP_COMMAND:        szIconID = TEXT("HELP_ICON");                break;
   case IDM_CODE_EDIT_OPEN_TARGET_SCRIPT:    szIconID = TEXT("SCRIPT_DEPENDENCY_ICON");   break;
   case IDM_CODE_EDIT_GOTO_LABEL:            szIconID = TEXT("FUNCTION_ICON");            break;
   case IDM_CODE_EDIT_VIEW_LANGUAGE_STRING:  szIconID = TEXT("EDIT_FORMATTING_ICON");     break;
   case IDM_CODE_EDIT_VIEW_ERROR:            szIconID = TEXT("ERROR_ICON");               break;
   case IDM_CODE_EDIT_VIEW_WARNING:          szIconID = TEXT("WARNING_ICON");             break;
   case IDM_CODE_EDIT_PROPERTIES:            szIconID = TEXT("PROPERTIES_ICON");          break;

   /// [LANGUAGE: RICHEDIT COLOURS]
   case IDM_COLOUR_BLACK:                    szIconID = TEXT("BLACK_ICON");               break;
   case IDM_COLOUR_BLUE:                     szIconID = TEXT("BLUE_ICON");                break;
   case IDM_COLOUR_CYAN:                     szIconID = TEXT("CYAN_ICON");                break;
   case IDM_COLOUR_DEFAULT:                  szIconID = TEXT("GREY_ICON");               break;
   case IDM_COLOUR_GREEN:                    szIconID = TEXT("GREEN_ICON");               break;
   case IDM_COLOUR_ORANGE:                   szIconID = TEXT("ORANGE_ICON");              break;
   case IDM_COLOUR_PURPLE:                   szIconID = TEXT("PURPLE_ICON");              break;
   case IDM_COLOUR_RED:                      szIconID = TEXT("RED_ICON");                 break;
   case IDM_COLOUR_WHITE:                    szIconID = TEXT("WHITE_ICON");               break;
   case IDM_COLOUR_YELLOW:                   szIconID = TEXT("YELLOW_ICON");              break;

   /// [LANGUAGE: GAMEPAGES]
   case IDM_GAMEPAGE_INSERT:                 szIconID = TEXT("INSERT_PAGE_ICON");         break;
   case IDM_GAMEPAGE_EDIT:                   szIconID = TEXT("EDIT_PAGE_ICON");           break;
   case IDM_GAMEPAGE_DELETE:                 szIconID = TEXT("DELETE_PAGE_ICON");         break;
   case IDM_GAMEPAGE_VOICED:                 szIconID = TEXT("VOICED_YES_ICON");          break;    // SPECIAL: Must be specified manually: 'VOICED_YES_ICON' or 'VOICED_NO_ICON'

   /// [LANGUAGE: GAMESTRINGS]
   case IDM_GAMESTRING_INSERT:               szIconID = TEXT("INSERT_PAGE_ICON");         break;
   case IDM_GAMESTRING_EDIT:                 szIconID = TEXT("EDIT_FORMATTING_ICON");     break;
   case IDM_GAMESTRING_DELETE:               szIconID = TEXT("DELETE_PAGE_ICON");         break;
   case IDM_GAMESTRING_VIEW_ERROR:           szIconID = TEXT("FORMATTING_ERROR_ICON");    break;
   case IDM_GAMESTRING_PROPERTIES:           szIconID = TEXT("PROPERTIES_ICON");          break;

   /// [PROPERTIES: SCRIPT ARGUMENTS]
   case IDM_ARGUMENT_INSERT:                 szIconID = TEXT("INSERT_PAGE_ICON");         break;
   case IDM_ARGUMENT_EDIT:                   szIconID = TEXT("EDIT_PAGE_ICON");           break;
   case IDM_ARGUMENT_DELETE:                 szIconID = TEXT("DELETE_PAGE_ICON");         break;

   /// [PROPERTIES: DEPENDENCIES]
   case IDM_DEPENDENCIES_LOAD:               szIconID = TEXT("OPEN_FILE_ICON");           break;
   case IDM_DEPENDENCIES_REFRESH:            
   case IDM_VARIABLES_REFRESH:               
   case IDM_STRINGS_REFRESH:                 szIconID = TEXT("REFRESH_ICON");             break;

   /// [PROPERTIES: LANGUAGE BUTTONS]
   case IDM_BUTTON_EDIT:                     szIconID = TEXT("EDIT_PAGE_ICON");           break;
   case IDM_BUTTON_DELETE:                   szIconID = TEXT("DELETE_PAGE_ICON");         break;

   /// [MESSAGE DIALOG]
   case IDM_FIRST_ATTACHMENT:                szIconID = TEXT("ATTACHMENT_ICON");          break;
   case IDM_FIRST_ATTACHMENT+1:              szIconID = TEXT("ATTACHMENT_ICON");          break;
   case IDM_FIRST_ATTACHMENT+2:              szIconID = TEXT("ATTACHMENT_ICON");          break;
   case IDM_FIRST_ATTACHMENT+3:              szIconID = TEXT("ATTACHMENT_ICON");          break;
   case IDM_FIRST_ATTACHMENT+4:              szIconID = TEXT("ATTACHMENT_ICON");          break;
   case IDM_FIRST_ATTACHMENT+5:              szIconID = TEXT("ATTACHMENT_ICON");          break;
   }

   /// Resolve resource ID to ImageList index
   return getImageTreeIconIndex(getAppImageTree(ITS_MEDIUM), szIconID);
}


/// Function name  : removeCustomMenuItem
// Description     : Removes and destroys an item from a custom menu
// 
// HMENU       hMenu       : [in] Custom menu handle
// CONST UINT  iItem       : [in] Menu item identifier - either a command ID or the the zero-based postiion
// CONST BOOL  bByPosition : [in] TRUE if 'iItem' is a position, FALSE for a Command ID
// 
// Return Value   : TRUE if successful, otherwise FALSE
// 
ControlsAPI 
BOOL  removeCustomMenuItem(HMENU  hMenu, CONST UINT  iItem, CONST BOOL  bByPosition)
{
   CUSTOM_MENU_ITEM*  pItem;        // Item data associated with the target item
   BOOL               bResult;      // Operation result

   // Prepare
   bResult = FALSE;

   // Lookup associated CustomMenuItem for the specified item
   if (pItem = getCustomMenuItemData(hMenu, iItem, bByPosition))
   {
      /// Destroy CustomMenuItem data
      deleteCustomMenuItemData(pItem);

      /// Destroy menu item
      bResult = DeleteMenu(hMenu, iItem, bByPosition);
   }

   // Return result
   return bResult;
}


/// Function name  : setCustomMenuItemText
// Description     : Removes and destroys an item from a custom menu
// 
// HMENU         hMenu       : [in] Custom menu handle
// CONST UINT    iItem       : [in] Menu item identifier - either a command ID or the the zero-based postiion
// CONST BOOL    bByPosition : [in] TRUE if 'iItem' is a position, FALSE for a Command ID
// CONST TCHAR*  szFormat    : [in] Text formatting string
// ....          ....        : [in] Formatting arguments
// 
// Return Value   : TRUE if successful, otherwise FALSE
// 
ControlsAPI 
BOOL  setCustomMenuItemText(HMENU  hMenu, CONST UINT  iItem, CONST BOOL  bByPosition, CONST TCHAR*  szFormat, ...)
{
   CUSTOM_MENU_ITEM*  pItem;        // Item data associated with the target item

   // [CHECK] Lookup data for the specified item
   if (pItem = getCustomMenuItemData(hMenu, iItem, bByPosition))
   {
      /// [FOUND] Replace text with formatted string
      utilDeleteString(pItem->szText);
      pItem->szText = utilCreateStringVf(128, szFormat, utilGetFirstVariableArgument(&szFormat));
   }

   // Return TRUE if successful
   return (pItem != NULL);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                   MESSAGE HANDLERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onOwnerDrawCustomMenu
// Description     : Convenience OwnerDraw handler for drawing custom menus
// 
// DRAWITEMSTRUCT*  pDrawData   : [in] OwnerDraw paint data
// 
// Returns : TRUE if drawn, otherwise FALSE
//
ControlsAPI 
BOOL   onOwnerDrawCustomMenu(DRAWITEMSTRUCT*  pDrawData)
{
   BOOL   bResult;

   // Prepare
   TRACK_FUNCTION();
   bResult = FALSE;

   // [CHECK] Ensure OwnerDraw data is for a menu
   if (pDrawData->CtlType == ODT_MENU AND pDrawData->itemData != NULL)
   {
      /// Draw item
      drawCustomMenu(pDrawData, (CUSTOM_MENU_ITEM*)pDrawData->itemData);
      bResult = TRUE;
   }
   
   END_TRACKING();
   return bResult;
}





