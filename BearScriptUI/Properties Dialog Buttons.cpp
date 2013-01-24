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

/// Function name  : onButtonPage_ContextMenu
// Description     : Display the 'Buttons' page listview context menu
// 
// LANGUAGE_DOCUMENT*  pDocument : [in] Language document data
// HWND                hDialog   : [in] 'Buttons' properties page window handle
// HWND                hCtrl     : [in] Window handle of the control that was right-clicked
// CONST POINT*        ptCursor  : [in] Cursor position, in screen co-ords
// 
// Return Value   : TRUE/FALSE
// 
BOOL    onButtonPage_ContextMenu(PROPERTIES_DATA*  pSheetData, HWND  hPage, HWND  hCtrl, CONST POINT*  ptCursor)
{
   CUSTOM_MENU*   pCustomMenu;    // Custom Popup menu
   LVHITTESTINFO  oHitTest;       // ListView hittest data

   /// [BUTTON LIST] Display the editing context menu
   if (GetDlgCtrlID(hCtrl) == IDC_BUTTONS_LIST)
   {
      // Identify the sub-item 
      oHitTest.pt = *ptCursor;
      ScreenToClient(hCtrl, &oHitTest.pt);
      ListView_SubItemHitTest(hCtrl, &oHitTest);
      
      // Create appropriate menu
      pCustomMenu = createCustomMenu(TEXT("LANGUAGE_MENU"), TRUE, IDM_BUTTON_POPUP);
      EnableMenuItem(pCustomMenu->hPopup, IDM_BUTTON_EDIT,   ListView_GetSelected(hCtrl) != -1 ? MF_ENABLED : MF_DISABLED);
      EnableMenuItem(pCustomMenu->hPopup, IDM_BUTTON_DELETE, ListView_GetSelected(hCtrl) != -1 ? MF_ENABLED : MF_DISABLED);

      // Display context menu
      switch (TrackPopupMenu(pCustomMenu->hPopup, TPM_TOPALIGN WITH TPM_LEFTALIGN WITH TPM_RETURNCMD, ptCursor->x, ptCursor->y, NULL, hPage, NULL))
      {
      /// [EDIT BUTTON]
      case IDM_BUTTON_EDIT:   editCustomListViewItem(hCtrl, oHitTest.iItem, oHitTest.iSubItem, LVLT_EDIT);                break;
      /// [DELETE BUTTON]
      case IDM_BUTTON_DELETE: removeButtonFromRichEditByIndex(pSheetData->pLanguageDocument->hRichEdit, oHitTest.iItem);    
                              ListView_SetItemCount(hCtrl, getRichTextDialogButtonCount(pSheetData->pLanguageDocument));  break;
      }

      // Cleanup
      deleteCustomMenu(pCustomMenu);
      return TRUE;
   }

   return FALSE;
}


/// Function name  : onButtonPage_LabelEditBegin
// Description     : Displays the button ID/Text in the Edit label
// 
// PROPERTIES_DATA*  pSheetData : [in] Sheet data
// HWND              hPage      : [in] 'Button' page 
// NMLVDISPINFO*     pLabelData : [in] Label notification
// 
VOID  onButtonPage_LabelEditBegin(PROPERTIES_DATA*  pSheetData, HWND  hPage, NMLVDISPINFO*  pLabelData)
{
   LANGUAGE_BUTTON*  pButton;

   // [CHECK] Item should be valid
   ASSERT(pLabelData->item.iItem != -1);

   /// Lookup LanguageButton
   if (findButtonInRichEditByIndex(pSheetData->pLanguageDocument->hRichEdit, pLabelData->item.iItem, pButton))
   {
      /// Provide ID/Text
      switch (pLabelData->item.iSubItem)
      {
      case BUTTON_COLUMN_ID:     SetWindowText(pLabelData->hdr.hwndFrom, pButton->szID);    break;
      case BUTTON_COLUMN_TEXT:   SetWindowText(pLabelData->hdr.hwndFrom, pButton->szText);  break;
      }
   }
}


/// Function name  : onButtonPage_LabelEditEnd
// Description     : Validates/Saves new button ID/text
// 
// PROPERTIES_DATA*  pSheetData : [in] Sheet data
// HWND              hPage      : [in] 'Button' page 
// NMLVDISPINFO*     pLabelData : [in] Label notification
// 
VOID  onButtonPage_LabelEditEnd(PROPERTIES_DATA*  pSheetData, HWND  hPage, NMLVDISPINFO*  pLabelData)
{
   LANGUAGE_DOCUMENT* pDocument = pSheetData->pLanguageDocument;
   LANGUAGE_BUTTON   *pButton,
                     *pNewButton;

   // Lookup language button
   if (findButtonInRichEditByIndex(pDocument->hRichEdit, pLabelData->item.iItem, pButton))
   {
      TCHAR*  szValue = utilGetWindowText(pLabelData->hdr.hwndFrom);
      BOOL    bAccept = TRUE;

      switch (pLabelData->item.iSubItem)
      {
      /// [TEXT] Ensure text isn't empty, then delete the OLE button and generate a new one
      case BUTTON_COLUMN_TEXT:
         if (bAccept = lstrlen(szValue))
         {
            // Replace desired button + Store resultant button data
            if (modifyButtonInRichEditByIndex(pDocument->hRichEdit, pLabelData->item.iItem, szValue, pNewButton))
               insertObjectIntoAVLTree(pDocument->pButtonsByID, (LPARAM)pNewButton);   
         }
         break;

      /// [ID] Ensure ID is valid, then replace
      case BUTTON_COLUMN_ID: 
         if (bAccept = validateLanguageButtonID(pDocument, szValue))
            utilReplaceString(pButton->szID, szValue);
         break;
      }

      // Return validation result
      utilDeleteString(szValue);
      SetWindowLong(hPage, DWL_MSGRESULT, bAccept);
   }
}


/// Function name  : onButtonPage_Notification
// Description     : Performs 'Buttons' page notification processing
// 
// LANGUAGE_DOCUMENT*  pDocument  : [in] Language Document data
// HWND                hDialog    : [in] 'Buttons' properties page window handle
// NMHDR*              pMessage   : [in] WM_NOTIFY message data
// 
// Return Value   : TRUE if processed, FALSE otherwise
// 
BOOL   onButtonPage_Notification(PROPERTIES_DATA*  pSheetData, HWND  hPage, NMHDR*  pMessage)
{
   /// [BUTTON LIST] 
   if (pMessage->idFrom == IDC_BUTTONS_LIST)
   {
      switch (pMessage->code)
      {
      // [REQUEST ITEM DATA]
      case LVN_GETDISPINFO:
         onButtonPage_RequestData(pSheetData, hPage, ((NMLVDISPINFO*)pMessage)->item);
         return TRUE;

      // [BEGIN EDIT]
      case LVN_BEGINLABELEDIT:
         onButtonPage_LabelEditBegin(pSheetData, hPage, (NMLVDISPINFO*)pMessage);
         return TRUE;

      // [END EDIT]
      case LVN_ENDLABELEDIT:
         onButtonPage_LabelEditEnd(pSheetData, hPage, (NMLVDISPINFO*)pMessage);
         return TRUE;
      }
   }

   return FALSE;
}


/// Function name  : onButtonPage_RequestData
// Description     : Supply 'buttons' page Listview item data
// 
// LANGUAGE_DOCUMENT*  pDocument : [in]     Language document data
// HWND                hDialog   : [in]     'Buttons' properties page window handle
// NMLVDISPINFO*       pOutput   : [in/out] Item + subitem data being requested
// 
VOID  onButtonPage_RequestData(PROPERTIES_DATA*  pSheetData, HWND  hPage, LVITEM&  oOutput)
{
   LANGUAGE_BUTTON*  pButtonData;     // Data associated with the OLE button object
   
   /// [IMAGE] Supply calculator image
   if (oOutput.mask INCLUDES LVIF_IMAGE)
      oOutput.iImage = getAppImageTreeIconIndex(ITS_SMALL, TEXT("VARIABLE_ICON"));

   // [INDENT] None
   if (oOutput.mask INCLUDES LVIF_INDENT)
      oOutput.iIndent = NULL;

   /// [TEXT] Supply text/id from the data associated with the OLE object
   if (oOutput.mask INCLUDES LVIF_TEXT)
   {
      // Lookup button data in RichEdit control
      if (findButtonInRichEditByIndex(pSheetData->pLanguageDocument->hRichEdit, oOutput.iItem, pButtonData))
      {
         // Supply appropriate property
         switch (oOutput.iSubItem)
         {
         case BUTTON_COLUMN_ID:   StringCchCopy(oOutput.pszText, oOutput.cchTextMax, pButtonData->szID);    break;
         case BUTTON_COLUMN_TEXT: StringCchCopy(oOutput.pszText, oOutput.cchTextMax, pButtonData->szText);  break;
         }
      }
      else
         StringCchCopy(oOutput.pszText, oOutput.cchTextMax, TEXT("[Not Found]"));
   }
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
   /// [NOTIFICATIONS]
   case WM_NOTIFY:
      if (onButtonPage_Notification(pDialogData, hDialog, (NMHDR*)lParam))
         return TRUE;
      break;

   /// [CONTEXT MENU]
   case WM_CONTEXTMENU:
      ptCursor.x = LOWORD(lParam);
      ptCursor.y = HIWORD(lParam);
      return onButtonPage_ContextMenu(pDialogData, hDialog, (HWND)wParam, &ptCursor);
   }

   // Pass to common handlers
   return dlgprocPropertiesPage(hDialog, iMessage, wParam, lParam, PP_LANGUAGE_BUTTON);
}

