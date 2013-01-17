//
// Document Property Sheet UI.cpp : Implementation of the UI actions of the document properties dialog
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       CONSTANTS / GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// 'Buttons' Page Column IDs
#define         BUTTON_COLUMN_ID          0
#define         BUTTON_COLUMN_TEXT        1

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                            HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////


/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                           FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////


/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        MESSAGE HANDLERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onButtonPageCommand
// Description     : WM_COMMAND processing for the 'Buttons' properties page
// 
// LANGUAGE_DOCUMENT* pDocument      : [in] Language Document data
// HWND               hDialog        : [in] 'Buttons' properties dialog page window handle
// CONST UINT         iControl       : [in] ID of the control sending the command
// CONST UINT         iNotification  : [in] Notification being sent
// HWND               hCtrl          : [in] Window handle fo the control sending the command
// 
// Return Value   : TRUE if processed, FALSE otherwise
// 
BOOL    onButtonPageCommand(LANGUAGE_DOCUMENT*  pDocument, HWND  hDialog, CONST UINT  iControl, CONST UINT  iNotification, HWND  hCtrl)
{
   switch (iControl)
   {
   /// [EDIT SELECTED BUTTON]
   case IDM_BUTTON_EDIT:
      return onCustomListViewItemEdit(&pDocument->oLabelData, GetDlgItem(hDialog, IDC_BUTTONS_LIST));

   /// [REMOVE EXISTING BUTTON]
   case IDM_BUTTON_DELETE:
      MessageBox(hDialog, TEXT("Deleting buttons not yet implemented"), TEXT("Info"), MB_OK);
      return TRUE;
   }
   return FALSE;
}



/// Function name  : onButtonPageContextMenu
// Description     : Display the 'Buttons' page listview context menu
// 
// LANGUAGE_DOCUMENT*  pDocument : [in] Language document data
// HWND                hDialog   : [in] 'Buttons' properties page window handle
// HWND                hCtrl     : [in] Window handle of the control that was right-clicked
// CONST POINT*        ptCursor  : [in] Cursor position, in screen co-ords
// 
// Return Value   : TRUE
// 
BOOL    onButtonPageContextMenu(LANGUAGE_DOCUMENT*  pDocument, HWND  hDialog, HWND  hCtrl, CONST POINT*  ptCursor)
{
   CUSTOM_MENU*   pCustomMenu;    // Custom Popup menu
   LVHITTESTINFO  oHitTest;          // ListView hittest data
   UINT           iItem;             // ListView index of the item clicked

   /// [BUTTON LIST] Display the editing context menu
   if (GetDlgCtrlID(hCtrl) == IDC_BUTTONS_LIST)
   {
      // Find the currently selected item 
      iItem = ListView_GetNextItem(hCtrl, -1, LVNI_ALL WITH LVNI_SELECTED);
      // [CHECK] Abort if user hasn't selected an item (all button menu items require a selected item)
      if (iItem == -1)
      {
         pDocument->oLabelData.iItem = pDocument->oLabelData.iSubItem = -1;
         return TRUE;
      }

      /// Identify the sub-item clicked (if any)
      oHitTest.pt = *ptCursor;
      ScreenToClient(hCtrl, &oHitTest.pt);

      // Determine and store Item + SubItem
      ListView_SubItemHitTest(hCtrl, &oHitTest);
      pDocument->oLabelData.iItem    = iItem;
      pDocument->oLabelData.iSubItem = oHitTest.iSubItem;
      
      // Create Button Popup CustomMenu
      pCustomMenu = createCustomMenu(TEXT("LANGUAGE_MENU"), TRUE, IDM_BUTTON_POPUP);

      /// Display context menu
      TrackPopupMenu(pCustomMenu->hPopup, TPM_TOPALIGN WITH TPM_LEFTALIGN, ptCursor->x, ptCursor->y, NULL, GetParent(hCtrl), NULL);

      // Cleanup
      deleteCustomMenu(pCustomMenu);
   }

   return TRUE;
}

/// Function name  : onButtonPageLabelEditBegin
// Description     : Copy the specified language button property into the label editing control
// 
// LANGUAGE_DOCUMENT*  pDocument   : [in] Language document data
// HWND                hDialog     : [in] 'Button' properties page window handle
// NMLVLABELINFO*      pLabelData  : [in] Custom label editing notification data
// 
// Return Value   : TRUE
// 
BOOL  onButtonPageLabelEditBegin(LANGUAGE_DOCUMENT*  pDocument, HWND  hDialog, NMLVLABELINFO*  pLabelData)
{
   LANGUAGE_BUTTON*  pLanguageButton;

   // [CHECK] Item should be valid
   ASSERT(pLabelData->iItem != -1);

   // Get specified language button
   if (findLanguageButtonInRichEditByIndex(pDocument->hRichEdit, pLabelData->iItem, pLanguageButton))
   {
      // Fill label with appropriate property
      switch (pLabelData->iSubItem)
      {
      case BUTTON_COLUMN_ID:     SetWindowText(pLabelData->hCtrl, pLanguageButton->szID);    break;
      case BUTTON_COLUMN_TEXT:   SetWindowText(pLabelData->hCtrl, pLanguageButton->szText);  break;
      }
   }

   return TRUE;
}

/// Function name  : onButtonPageLabelEditEnd
// Description     : Save the contents of the label editing control to the appropriate language button property
// 
// LANGUAGE_DOCUMENT*  pDocument   : [in] Language document data
// HWND                hDialog     : [in] 'Button' properties page window handle
// NMLVLABELINFO*      pLabelData  : [in] Custom label editing notification data
// 
// Return Value   : TRUE
// 
BOOL  onButtonPageLabelEditEnd(LANGUAGE_DOCUMENT*  pDocument, HWND  hDialog, NMLVLABELINFO*  pLabelData)
{
   LANGUAGE_BUTTON*  pLanguageButton;

   // [CHECK] User didn't cancel editing
   if (pLabelData->szNewText == NULL)
      return TRUE;

   // Get specified language button
   if (findLanguageButtonInRichEditByIndex(pDocument->hRichEdit, pLabelData->iItem, pLanguageButton))
   {
      // Update appropriate property
      switch (pLabelData->iSubItem)
      {
      case BUTTON_COLUMN_ID:     utilReplaceString(pLanguageButton->szID,   pLabelData->szNewText);    break;
      case BUTTON_COLUMN_TEXT:   utilReplaceString(pLanguageButton->szText, pLabelData->szNewText);    break;
      }
   }

   return TRUE;
}


/// Function name  : onButtonPageNotification
// Description     : Performs 'Buttons' page notification processing
// 
// LANGUAGE_DOCUMENT*  pDocument  : [in] Language Document data
// HWND                hDialog    : [in] 'Buttons' properties page window handle
// NMHDR*              pMessage   : [in] WM_NOTIFY message data
// 
// Return Value   : TRUE if processed, FALSE otherwise
// 
BOOL   onButtonPageNotification(LANGUAGE_DOCUMENT*  pDocument, HWND  hDialog, NMHDR*  pMessage)
{
   /// [BUTTON LIST] 
   if (pMessage->idFrom == IDC_BUTTONS_LIST)
   {
      switch (pMessage->code)
      {
      // [REQUEST ITEM DATA]
      case LVN_GETDISPINFO:
         return onButtonPageRequestData(pDocument, hDialog, (NMLVDISPINFO*)pMessage);

      // [LABEL EDITING BEGIN]
      case LVCN_BEGINLABELEDIT:
         return onButtonPageLabelEditBegin(pDocument, hDialog, (NMLVLABELINFO*)pMessage);

      // [LABEL EDITING END]
      case LVCN_ENDLABELEDIT:
         return onButtonPageLabelEditEnd(pDocument, hDialog, (NMLVLABELINFO*)pMessage);
      }
   }

   return FALSE;
}


/// Function name  : onButtonPageRequestData
// Description     : Supply 'buttons' page Listview item data
// 
// LANGUAGE_DOCUMENT*  pDocument : [in]     Language document data
// HWND                hDialog   : [in]     'Buttons' properties page window handle
// NMLVDISPINFO*       pOutput   : [in/out] Item + subitem data being requested
// 
// Return Value   : TRUE
// 
BOOL  onButtonPageRequestData(LANGUAGE_DOCUMENT*  pDocument, HWND  hDialog, NMLVDISPINFO*  pOutput)
{
   LANGUAGE_BUTTON*  pButtonData;     // Data associated with the OLE button object
   
   /// [IMAGE] Supply none
   if (pOutput->item.mask INCLUDES LVIF_IMAGE)
      pOutput->item.iImage = NULL;

   /// [INDENT] Supply none
   if (pOutput->item.mask INCLUDES LVIF_INDENT)
      pOutput->item.iIndent = NULL;

   /// [TEXT] Supply text/id from the data associated with the OLE object
   if (pOutput->item.mask INCLUDES LVIF_TEXT)
   {
      // Lookup button data in RichEdit control
      if (findLanguageButtonInRichEditByIndex(pDocument->hRichEdit, pOutput->item.iItem, pButtonData))
      {
         // Supply appropriate property
         switch (pOutput->item.iSubItem)
         {
         case BUTTON_COLUMN_ID:   StringCchCopy(pOutput->item.pszText, pOutput->item.cchTextMax, pButtonData->szID);    break;
         case BUTTON_COLUMN_TEXT: StringCchCopy(pOutput->item.pszText, pOutput->item.cchTextMax, pButtonData->szText);  break;
         }
      }
      else
         StringCchCopy(pOutput->item.pszText, pOutput->item.cchTextMax, TEXT("[Not Found]"));
   }

   return TRUE;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                LANGUAGE PAGE  WINDOW PROCEDURES
/// ////////////////////////////////////////////////////////////////////////////////////////////////////


/// Function name  : dlgprocButtonPage
// Description     : Window procedure for the 'General' script property page
//
// 
INT_PTR   dlgprocButtonPage(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   PROPERTIES_DATA*  pDialogData;
   POINT             ptCursor;

   // Get dialog data
   pDialogData = getPropertiesDialogData(hDialog);

   // Intercept messages
   switch (iMessage)
   {
   /// [COMMANDS]
   case WM_COMMAND:
      if (onButtonPageCommand((LANGUAGE_DOCUMENT*)pDialogData->pDocument, hDialog, LOWORD(wParam), HIWORD(wParam), (HWND)lParam))
         return TRUE;
      break;

   /// [NOTIFICATIONS]
   case WM_NOTIFY:
      if (onButtonPageNotification((LANGUAGE_DOCUMENT*)pDialogData->pDocument, hDialog, (NMHDR*)lParam))
         return TRUE;
      break;

   /// [CONTEXT MENU]
   case WM_CONTEXTMENU:
      ptCursor.x = LOWORD(lParam);
      ptCursor.y = HIWORD(lParam);
      return onButtonPageContextMenu((LANGUAGE_DOCUMENT*)pDialogData->pDocument, hDialog, (HWND)wParam, &ptCursor);

   // [CUSTOM MENU DRAWING]
   case WM_DRAWITEM:
      return onOwnerDrawCustomMenu((DRAWITEMSTRUCT*)lParam);
   // [CUSTOM MENU MEASURING]
   case WM_MEASUREITEM:
      calculateCustomMenuItemSize(hDialog, (MEASUREITEMSTRUCT*)lParam);
      return TRUE;
   }

   // Pass to common handlers
   return dlgprocPropertiesPage(hDialog, iMessage, wParam, lParam, PP_LANGUAGE_BUTTON);
}

