//
// Properties Dialog Columns.cpp : The Column adjustment properties page for a language document
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        MACROS
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////

// 'Columns' Page 'Number of Columns' Custom Radio button colours
CONST COLORREF  clRadioButtonSelected     = GetSysColor(COLOR_GRAYTEXT), // RGB(0,70,213),
                clRadioButtonHot          = GetSysColor(COLOR_HIGHLIGHT); // RGB(18,149,204);

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////


/// Function name  : displayColumnPageSliderText
// Description     : Format the specified column width or spacing and assign to the specified static control
// 
// HWND        hPage      : [in] 'Columns' page window handle
// CONST UINT  iControlID : [in] ID of the destination static control
// CONST UINT  iValue     : [in] Value to format
// 
VOID  displayColumnPageSliderText(HWND  hPage, CONST UINT  iControlID, CONST UINT  iValue)
{
   TCHAR  szText[32];

   // Assemble output text
   StringCchPrintf(szText, 32, iControlID == IDC_COLUMN_WIDTH_STATIC ? TEXT("%u pixels wide") : TEXT("%u pixels apart"), iValue);

   // Output
   SetWindowText(GetDlgItem(hPage, iControlID), szText);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////


/// Function name  : onColumnsPage_Command
// Description     : WM_COMMAND processing for the 'Columns' properties page
// 
// PROPERTIES_DATA*   pSheetData     : [in] Properties sheet data
// HWND               hPage          : [in] 'Columns' properties dialog page window handle
// CONST UINT         iControl       : [in] ID of the control sending the command
// CONST UINT         iNotification  : [in] Notification being sent
// HWND               hCtrl          : [in] Window handle fo the control sending the command
// 
// Return Value   : TRUE if processed, FALSE otherwise
// 
BOOL    onColumnsPage_Command(PROPERTIES_DATA*  pSheetData, HWND  hPage, CONST UINT  iControl, CONST UINT  iNotification, HWND  hCtrl)
{
   LANGUAGE_MESSAGE*  pCurrentMessage = pSheetData->pLanguageDocument->pCurrentMessage;    // Convenience pointer for current language message;
   BOOL               bResult = TRUE;

   TRACK_FUNCTION();
   switch (iControl)
   {
   /// [COLUMN COUNT] -- Update the message
   case IDC_COLUMN_ONE_RADIO:  
   case IDC_COLUMN_TWO_RADIO:  
   case IDC_COLUMN_THREE_RADIO:
   case IDC_COLUMN_WIDTH_CHECK:
   case IDC_COLUMN_SPACING_CHECK:
      switch (iControl)
      {
      /// [COLUMN COUNT] -- Update the message
      case IDC_COLUMN_ONE_RADIO:    pCurrentMessage->iColumnCount = 1;   break;
      case IDC_COLUMN_TWO_RADIO:    pCurrentMessage->iColumnCount = 2;   break;
      case IDC_COLUMN_THREE_RADIO:  pCurrentMessage->iColumnCount = 3;   break;

      /// [CUSTOM WIDTH] -- Update message then enable/disable width controls
      case IDC_COLUMN_WIDTH_CHECK:
         pCurrentMessage->bCustomWidth = IsDlgButtonChecked(hPage, IDC_COLUMN_WIDTH_CHECK);
         EnableWindow(GetDlgItem(hPage, IDC_COLUMN_WIDTH_SLIDER), pCurrentMessage->bCustomWidth);
         EnableWindow(GetDlgItem(hPage, IDC_COLUMN_WIDTH_STATIC), pCurrentMessage->bCustomWidth);
         break;

      /// [CUSTOM SPACING] -- Update message then enable/disable width controls
      case IDC_COLUMN_SPACING_CHECK:
         pCurrentMessage->bCustomSpacing = IsDlgButtonChecked(hPage, IDC_COLUMN_SPACING_CHECK);
         EnableWindow(GetDlgItem(hPage, IDC_COLUMN_SPACING_SLIDER), pCurrentMessage->bCustomSpacing);
         EnableWindow(GetDlgItem(hPage, IDC_COLUMN_SPACING_STATIC), pCurrentMessage->bCustomSpacing);
         break;
      }

      // [BY USER] Notify document of property change
      if (pSheetData->bRefreshing)
         sendDocumentPropertyUpdated(AW_DOCUMENTS_CTRL, iControl);
      break;

   // [UNHANDLED]
   default:
      bResult = FALSE;
      break;
   }

   END_TRACKING();
   return bResult;
}


/// Function name  : onColumnsPage_DrawGroup
// Description     : Custom draw the 'Number of Columns' radio buttons
// 
// PROPERTIES_DATA*    pSheetData : [in] Properties sheet data
// HWND                hPage      : [in] Window handle of the 'Columns' properties dialog page
// HIMAGELIST          hImageList : [in] ImageList containing the large column selection icons
// NMCUSTOMDRAW*       pDrawData  : [in] WM_NOTIFY custom draw data
// 
// Return Value   : TRUE
// 
BOOL  onColumnsPage_DrawGroup(PROPERTIES_DATA*  pSheetData, HWND  hPage, DRAWITEMSTRUCT*  pDrawData)
{
   HTHEME  hTheme;
   RECT    rcCheck;
   UINT    iCheckID;
   INT     iDCState;

   // Prepare
   iDCState = SaveDC(pDrawData->hDC);

   // Get drawing rectangle of relevant CheckBox
   iCheckID = (pDrawData->CtlID == IDC_COLUMN_WIDTH_GROUP ? IDC_COLUMN_WIDTH_CHECK : IDC_COLUMN_SPACING_CHECK);
   GetWindowRect(GetControl(hPage, iCheckID), &rcCheck);

   // Clip the CheckBox
   utilScreenToClientRect(pDrawData->hwndItem, &rcCheck);
   ExcludeClipRect(pDrawData->hDC, rcCheck.left, rcCheck.top, rcCheck.right, rcCheck.bottom);

   /// [THEME]
   if (hTheme = OpenThemeData(pDrawData->hwndItem, TEXT("BUTTON")))
   {
      DrawThemeBackground(hTheme, pDrawData->hDC, BP_GROUPBOX, GBS_NORMAL, &pDrawData->rcItem, NULL);
      CloseThemeData(hTheme);
   }
   else
      /// [NON-THEME]
      DrawEdge(pDrawData->hDC, &pDrawData->rcItem, EDGE_ETCHED, BF_RECT);

   RestoreDC(pDrawData->hDC, iDCState);
   return TRUE;
}


/// Function name  : onColumnsPage_DrawRadio
// Description     : Custom draw the 'Number of Columns' radio buttons
// 
// PROPERTIES_DATA*    pSheetData : [in] Properties sheet data
// HWND                hPage      : [in] Window handle of the 'Columns' properties dialog page
// HIMAGELIST          hImageList : [in] ImageList containing the large column selection icons
// NMCUSTOMDRAW*       pDrawData  : [in] WM_NOTIFY custom draw data
// 
// Return Value   : TRUE
// 
BOOL  onColumnsPage_DrawRadio(PROPERTIES_DATA*  pSheetData, HWND  hPage, DRAWITEMSTRUCT*  pDrawData)
{
   CONST UINT iIconSize = 48;
   DC_STATE   oPrevState;         // Device context state
   POINT      ptIcon;             // Position of the icon within the draw rectangle
   BOOL       bIsChecked,         // Whether control is checked
              bIsHot;             // Whether cursor is over the control
   SIZE       siDrawSize;         // Size of the drawing rectangle
   HPEN       hPen;               // Pen used for drawing the coloured rectangled
   TCHAR      szText[16];         // Control's text, displayed beneath the icon

   
   // Prepare
   utilConvertRectangleToSize(&pDrawData->rcItem, &siDrawSize);
   GetWindowText(pDrawData->hwndItem, szText, 16);
   
   // Calculate icon position
   ptIcon.x = (siDrawSize.cx - iIconSize) / 2;
   ptIcon.y = (siDrawSize.cy - iIconSize) / 2;

   /// [ICON] Draw appropriate icon
   ImageList_Draw(pSheetData->hColumnIcons, pDrawData->CtlID - IDC_COLUMN_ONE_RADIO, pDrawData->hDC, ptIcon.x, ptIcon.x, ILD_NORMAL);
   
   // Determine state
   bIsHot     = (pDrawData->itemState INCLUDES CDIS_HOT);
   bIsChecked = IsDlgButtonChecked(hPage, pDrawData->CtlID);

   if (bIsHot OR bIsChecked)
   {
      // Create appropriately coloured pen
      hPen = CreatePen(PS_SOLID, 3, (bIsHot ? clRadioButtonHot : clRadioButtonSelected));
      oPrevState.hOldBrush = SelectBrush(pDrawData->hDC, GetStockObject(NULL_BRUSH));
      oPrevState.hOldPen   = SelectPen(pDrawData->hDC, hPen);

      /// [BORDER] Draw border 
      Rectangle(pDrawData->hDC, pDrawData->rcItem.left, pDrawData->rcItem.top, pDrawData->rcItem.right, pDrawData->rcItem.bottom);

      // Cleanup
      SelectPen(pDrawData->hDC, oPrevState.hOldPen);
      SelectBrush(pDrawData->hDC, oPrevState.hOldBrush);
      DeleteObject(hPen);
   }

   /// [TEXT] Draw window text beneath icon
   pDrawData->rcItem.top += ptIcon.x + iIconSize;
   DrawText(pDrawData->hDC, szText, lstrlen(szText), &pDrawData->rcItem, DT_CENTER);
   return TRUE;
}


/// Function name  : onColumnsPage_DrawRadio
// Description     : Custom draw the 'Number of Columns' radio buttons
// 
// PROPERTIES_DATA*    pSheetData : [in] Properties sheet data
// HWND                hPage      : [in] Window handle of the 'Columns' properties dialog page
// HIMAGELIST          hImageList : [in] ImageList containing the large column selection icons
// NMCUSTOMDRAW*       pDrawData  : [in] WM_NOTIFY custom draw data
// 
// Return Value   : TRUE
// 
BOOL  onColumnsPage_DrawRadio(PROPERTIES_DATA*  pSheetData, HWND  hPage, NMCUSTOMDRAW*  pDrawData)
{
   CONST UINT iIconSize = 48;
   DC_STATE   oPrevState;         // Device context state
   POINT      ptIcon;             // Position of the icon within the draw rectangle
   BOOL       bIsChecked,         // Whether control is checked
              bIsHot;             // Whether cursor is over the control
   SIZE       siDrawSize;         // Size of the drawing rectangle
   HPEN       hPen;               // Pen used for drawing the coloured rectangled
   TCHAR      szText[16];         // Control's text, displayed beneath the icon

   switch (pDrawData->dwDrawStage)
   {
   case CDDS_PREERASE:
   case CDDS_PREPAINT:
      // Prepare
      utilConvertRectangleToSize(&pDrawData->rc, &siDrawSize);
      GetWindowText(pDrawData->hdr.hwndFrom, szText, 16);
      
      // Calculate icon position
      ptIcon.x = (siDrawSize.cx - iIconSize) / 2;
      ptIcon.y = (siDrawSize.cy - iIconSize) / 2;

      /// [ICON] Draw appropriate icon
      ImageList_Draw(pSheetData->hColumnIcons, pDrawData->hdr.idFrom - IDC_COLUMN_ONE_RADIO, pDrawData->hdc, ptIcon.x, ptIcon.x, ILD_NORMAL);
      
      // Determine state
      bIsHot     = (pDrawData->uItemState INCLUDES CDIS_HOT);
      bIsChecked = IsDlgButtonChecked(hPage, pDrawData->hdr.idFrom);

      if (bIsHot OR bIsChecked)
      {
         // Create appropriately coloured pen
         hPen = CreatePen(PS_SOLID, 3, (bIsHot ? clRadioButtonHot : clRadioButtonSelected));
         oPrevState.hOldBrush = SelectBrush(pDrawData->hdc, GetStockObject(NULL_BRUSH));
         oPrevState.hOldPen   = SelectPen(pDrawData->hdc, hPen);

         /// [BORDER] Draw border 
         Rectangle(pDrawData->hdc, pDrawData->rc.left, pDrawData->rc.top, pDrawData->rc.right, pDrawData->rc.bottom);

         // Cleanup
         SelectPen(pDrawData->hdc, oPrevState.hOldPen);
         SelectBrush(pDrawData->hdc, oPrevState.hOldBrush);
         DeleteObject(hPen);
      }

      /// [TEXT] Draw window text beneath icon
      pDrawData->rc.top += ptIcon.x + iIconSize;
      DrawText(pDrawData->hdc, szText, lstrlen(szText), &pDrawData->rc, DT_CENTER);
      
      // Prevent system from painting
      SetWindowLong(hPage, DWL_MSGRESULT, CDRF_SKIPDEFAULT);
      break;
   }
   
   return TRUE;
}


/// Function name  : onColumnsPage_Scroll
// Description     : Extract the current position from the trackbar and display in it's static control
// 
// PROPERTIES_DATA*  pSheetData   : [in] Properties sheet data
// HWND              hPage        : [in] 'Columns' property dialog page window handle
// CONST UINT        iScrollType  : [in] Type of scrolling to perform
// UINT              iPosition    : [in] [DRAGGING] Current drag position
// HWND              hCtrl        : [in] Scrollbar control window handle
// 
// Return Value   : TRUE
// 
BOOL  onColumnsPage_Scroll(PROPERTIES_DATA*  pSheetData, HWND  hPage, CONST UINT  iScrollType, UINT  iPosition, HWND  hCtrl)
{
   UINT  iSliderID,     // ID of the slider control being scrolled
         iStaticID;     // ID of the related static which will display formatted position of the slider

   // Prepare
   iSliderID = GetDlgCtrlID(hCtrl);

   /// Determine slider position  
   switch (iScrollType)
   {
   // [DRAG] 
   case SB_THUMBTRACK:
   case SB_THUMBPOSITION:  break;
   // [NON-DRAG MOVEMENT]
   default:                iPosition = SendDlgItemMessage(hPage, iSliderID, TBM_GETPOS, NULL, NULL);  break;
   }

   /// Update current message
   switch (iSliderID)
   {
   // [WIDTH]
   case IDC_COLUMN_WIDTH_SLIDER:
      iStaticID = IDC_COLUMN_WIDTH_STATIC;
      getLanguageMessage(pSheetData)->iColumnWidth = iPosition;
      break;

   // [SPACING]
   case IDC_COLUMN_SPACING_SLIDER:
      iStaticID = IDC_COLUMN_SPACING_STATIC;
      getLanguageMessage(pSheetData)->iColumnSpacing = iPosition;
      break;
   }

   // [BY USER] Notify document of change
   if (!pSheetData->bRefreshing)
      sendDocumentPropertyUpdated(AW_DOCUMENTS_CTRL, iSliderID);

   /// Format and display position value
   displayColumnPageSliderText(hPage, iStaticID, iPosition);
   return TRUE;
}


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : dlgprocColumnsPage
// Description     : Window procedure for the 'General' script property page
//
// 
INT_PTR   dlgprocColumnsPage(HWND  hPage, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   PROPERTIES_DATA*  pDialogData;
   NMHDR*            pHeader;
   BOOL              bResult = FALSE;
   TRACK_FUNCTION();

   // Get dialog data
   pDialogData = getPropertiesDialogData(hPage);
   
   switch (iMessage)
   {
   /// [SLIDER] 
   case WM_HSCROLL:
      bResult = onColumnsPage_Scroll(pDialogData, hPage, LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
      break;

   /// [COMMANDS]
   case WM_COMMAND:
      bResult = onColumnsPage_Command(pDialogData, hPage, LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
      break;

   /// [OWNER DRAW GROUP-BOX]
   case WM_DRAWITEM:
      switch (wParam)
      {
      case IDC_COLUMN_WIDTH_GROUP:
      case IDC_COLUMN_SPACING_GROUP:  bResult = onColumnsPage_DrawGroup(pDialogData, hPage, (DRAWITEMSTRUCT*)lParam);   break;
      case IDC_COLUMN_ONE_RADIO:
      case IDC_COLUMN_TWO_RADIO:
      case IDC_COLUMN_THREE_RADIO:    bResult = onColumnsPage_DrawRadio(pDialogData, hPage, (DRAWITEMSTRUCT*)lParam);   break;
      }
      break;

   /// [CUSTOM DRAW RADIO-BUTTON]
   case WM_NOTIFY:
      pHeader = (NMHDR*)lParam;

      // [CUSTOM DRAW] - Only custom draw the 'Number of Columns' radio buttons
      if (pHeader->code == NM_CUSTOMDRAW)
      {
         switch (pHeader->idFrom)
         {
         case IDC_COLUMN_ONE_RADIO:
         case IDC_COLUMN_TWO_RADIO:
         case IDC_COLUMN_THREE_RADIO:
            bResult = onColumnsPage_DrawRadio(pDialogData, hPage, (NMCUSTOMDRAW*)pHeader);
            break;
         }
      }
      break;
   }

   END_TRACKING();
   return (bResult ? bResult : dlgprocPropertiesPage(hPage, iMessage, wParam, lParam, PP_LANGUAGE_COLUMNS));
}
