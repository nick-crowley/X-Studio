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
CONST COLORREF  clRadioButtonSelected     = RGB(0,70,213),
                clRadioButtonHot          = RGB(18,149,204);

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////


/// Function name  : displayColumnPageSliderText
// Description     : Format the specified column width or spacing and assign to the specified static control
// 
// HWND        hDialog    : [in] 'Columns' page window handle
// CONST UINT  iControlID : [in] ID of the destination static control
// CONST UINT  iValue     : [in] Value to format
// 
VOID  displayColumnPageSliderText(HWND  hDialog, CONST UINT  iControlID, CONST UINT  iValue)
{
   TCHAR  szText[32];

   // Assemble output text
   StringCchPrintf(szText, 32, TEXT("%u pixels"), iValue);

   // Output
   SetWindowText(GetDlgItem(hDialog, iControlID), szText);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////


/// Function name  : onColumnsPageCommand
// Description     : WM_COMMAND processing for the 'Columns' properties page
// 
// LANGUAGE_DOCUMENT* pDocument      : [in] Language Document data
// HWND               hDialog        : [in] 'Columns' properties dialog page window handle
// CONST UINT         iControl       : [in] ID of the control sending the command
// CONST UINT         iNotification  : [in] Notification being sent
// HWND               hCtrl          : [in] Window handle fo the control sending the command
// 
// Return Value   : TRUE if processed, FALSE otherwise
// 
BOOL    onColumnsPageCommand(LANGUAGE_DOCUMENT*  pDocument, HWND  hDialog, CONST UINT  iControl, CONST UINT  iNotification, HWND  hCtrl)
{
   LANGUAGE_MESSAGE*  &pCurrentMessage = pDocument->pCurrentMessage;    // Convenience pointer for current language message

   switch (iControl)
   {
   /// [COLUMN COUNT] -- Update the message
   case IDC_COLUMN_ONE_RADIO:    pCurrentMessage->iColumnCount = 1;   break;
   case IDC_COLUMN_TWO_RADIO:    pCurrentMessage->iColumnCount = 2;   break;
   case IDC_COLUMN_THREE_RADIO:  pCurrentMessage->iColumnCount = 3;   break;

   /// [CUSTOM WIDTH] -- Update message then enable/disable width controls
   case IDC_COLUMN_WIDTH_CHECK:
      pCurrentMessage->bCustomWidth = IsDlgButtonChecked(hDialog, IDC_COLUMN_WIDTH_CHECK);
      EnableWindow(GetDlgItem(hDialog, IDC_COLUMN_WIDTH_SLIDER), pCurrentMessage->bCustomWidth);
      EnableWindow(GetDlgItem(hDialog, IDC_COLUMN_WIDTH_STATIC), pCurrentMessage->bCustomWidth);
      break;

   /// [CUSTOM SPACING] -- Update message then enable/disable width controls
   case IDC_COLUMN_SPACING_CHECK:
      pCurrentMessage->bCustomSpacing = IsDlgButtonChecked(hDialog, IDC_COLUMN_SPACING_CHECK);
      EnableWindow(GetDlgItem(hDialog, IDC_COLUMN_SPACING_SLIDER), pCurrentMessage->bCustomSpacing);
      EnableWindow(GetDlgItem(hDialog, IDC_COLUMN_SPACING_STATIC), pCurrentMessage->bCustomSpacing);
      break;

   default:
      return FALSE;
   }

   return TRUE;
}




/// Function name  : onColumnsPageCustomDraw
// Description     : Custom draw the 'Number of Columns' radio buttons
// 
// LANGUAGE_DOCUMENT*  pDocument  : [in] Language Document data
// HWND                hDialog    : [in] Window handle of the 'Columns' properties dialog page
// HIMAGELIST          hImageList : [in] ImageList containing the large column selection icons
// NMCUSTOMDRAW*       pDrawData  : [in] WM_NOTIFY custom draw data
// 
// Return Value   : TRUE
// 
BOOL  onColumnsPageCustomDraw(LANGUAGE_DOCUMENT*  pDocument, HWND  hDialog, HIMAGELIST  hImageList, NMCUSTOMDRAW*  pDrawData)
{
   //DEVICE_CONTEXT_STATE  oPrevState;         // Device context state
   //COLORREF              clBorderColour;     // Colour of the border around the icon
   //POINT                 ptIcon;             // Position of the icon within the draw rectangle
   //BOOL                  bIsChecked,         // Whether control is checked
   //                      bIsHot;             // Whether cursor is over the control
   //SIZE                  siDrawSize;         // Size of the drawing rectangle
   //HPEN                  hPen;               // Pen used for drawing the coloured rectangled
   //TCHAR                 szText[16];         // Control's text, displayed beneath the icon

   //switch (pDrawData->dwDrawStage)
   //{
   //case CDDS_PREERASE:
   //   // Prepare
   //   utilConvertRectangleToSize(&pDrawData->rc, &siDrawSize);
   //   
   //   // Calculate icon position
   //   ptIcon.x = (siDrawSize.cx / 2) - 24;
   //   ptIcon.y = (siDrawSize.cy / 2) - 24;

   //   /// [ICON] Draw appropriate icon
   //   ImageList_Draw(hImageList, pDrawData->hdr.idFrom - IDC_COLUMN_ONE_RADIO, pDrawData->hdc, ptIcon.x, ptIcon.x, ILD_NORMAL);
   //   
   //   /// [BORDER] Draw appropriate border (or none)
   //   bIsHot     = (pDrawData->uItemState INCLUDES CDIS_HOT);
   //   bIsChecked = IsDlgButtonChecked(hDialog, pDrawData->hdr.idFrom);

   //   if (bIsHot OR bIsChecked)
   //   {
   //      // Create appropriately coloured pen
   //      clBorderColour = (bIsHot ? clRadioButtonHot : clRadioButtonSelected);
   //      hPen = CreatePen(PS_SOLID, 3, clBorderColour);
   //      
   //      // Draw border using appropriate border and (lack of) fill colour
   //      oPrevState.hPen = (HPEN)SelectObject(pDrawData->hdc, hPen);
   //      oPrevState.hBrush = (HBRUSH)SelectObject(pDrawData->hdc, GetStockObject(NULL_BRUSH));
   //      Rectangle(pDrawData->hdc, pDrawData->rc.left, pDrawData->rc.top, pDrawData->rc.right, pDrawData->rc.bottom);

   //      // Cleanup
   //      SelectObject(pDrawData->hdc, oPrevState.hPen);
   //      SelectObject(pDrawData->hdc, oPrevState.hBrush);
   //      DeleteObject(hPen);
   //   }

   //   /// [TEXT] Draw window text beneath icon
   //   pDrawData->rc.top += ptIcon.x + 48;
   //   GetWindowText(pDrawData->hdr.hwndFrom, szText, 16);
   //   DrawText(pDrawData->hdc, szText, lstrlen(szText), &pDrawData->rc, DT_CENTER);
   //   
   //   // Prevent system from painting
   //   SetWindowLong(hDialog, DWL_MSGRESULT, CDRF_SKIPDEFAULT);
   //   break;
   //}
   
   return TRUE;
}


/// Function name  : onColumnsPageScrollHorizontal
// Description     : Extract the current position from the trackbar and display in it's static control
// 
// LANGUAGE_DOCUMENT*  pDocument    : [in] LanguageDocument data
// HWND                hDialog      : [in] 'Columns' property dialog page window handle
// CONST UINT          iScrollType  : [in] Type of scrolling to perform
// UINT                iPosition    : [in] [DRAGGING] Current drag position
// HWND                hCtrl        : [in] Scrollbar control window handle
// 
// Return Value   : TRUE
// 
BOOL  onColumnsPageScrollHorizontal(LANGUAGE_DOCUMENT*  pDocument, HWND  hDialog, CONST UINT  iScrollType, UINT  iPosition, HWND  hCtrl)
{
   UINT  iSliderID,     // ID of the slider control being scrolled
         iStaticID;     // ID of the related static which will display formatted position of the slider

   /// Determine slider ID and position
   iSliderID = GetDlgCtrlID(hCtrl);
   switch (iScrollType)
   {
   case SB_THUMBTRACK:
   case SB_THUMBPOSITION:
      break;

   // [NON-DRAG MOVEMENT] - Determine position manually
   default:
      iPosition = SendDlgItemMessage(hDialog, iSliderID, TBM_GETPOS, NULL, NULL);
      break;
   }

   /// Update current message
   switch (iSliderID)
   {
   // [WIDTH]
   case IDC_COLUMN_WIDTH_SLIDER:
      iStaticID = IDC_COLUMN_WIDTH_STATIC;
      pDocument->pCurrentMessage->iColumnWidth = iPosition;
      break;

   // [SPACING]
   case IDC_COLUMN_SPACING_SLIDER:
      iStaticID = IDC_COLUMN_SPACING_STATIC;
      pDocument->pCurrentMessage->iColumnSpacing = iPosition;
      break;
   }

   /// Format and display position value
   displayColumnPageSliderText(hDialog, iStaticID, iPosition);
   return TRUE;
}


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////



/// Function name  : dlgprocColumnsPage
// Description     : Window procedure for the 'General' script property page
//
// 
INT_PTR   dlgprocColumnsPage(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   PROPERTIES_DATA*  pDialogData;
   NMHDR*            pHeader;

   // Get dialog data
   pDialogData = getPropertiesDialogData(hDialog);
   
   switch (iMessage)
   {
   /// [TRACKBAR MOVEMENT] 
   case WM_HSCROLL:
      return onColumnsPageScrollHorizontal((LANGUAGE_DOCUMENT*)pDialogData->pDocument, hDialog, LOWORD(wParam), HIWORD(wParam), (HWND)lParam);

   // [COMMAND PROCESSING]
   case WM_COMMAND:
      if (onColumnsPageCommand((LANGUAGE_DOCUMENT*)pDialogData->pDocument, hDialog, LOWORD(wParam), HIWORD(wParam), (HWND)lParam))
         return TRUE;
      break;

   /// [NOTIFICATION]
   case WM_NOTIFY:
      pHeader = (NMHDR*)lParam;
      // [CUSTOM DRAW] - Only custom draw the 'Number of Columns' radio buttons
      if (pHeader->code == NM_CUSTOMDRAW AND pHeader->idFrom >= IDC_COLUMN_ONE_RADIO AND pHeader->idFrom <= IDC_COLUMN_THREE_RADIO)
         return onColumnsPageCustomDraw((LANGUAGE_DOCUMENT*)pDialogData->pDocument, hDialog, pDialogData->hColumnIcons, (NMCUSTOMDRAW*)pHeader);
      break;
   }

   return dlgprocPropertiesPage(hDialog, iMessage, wParam, lParam, PP_LANGUAGE_COLUMNS);
}
