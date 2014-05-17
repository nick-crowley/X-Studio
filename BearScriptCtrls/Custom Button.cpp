//
// Custom Button.cpp : CustomDraw button with an icon, text and a themed appearance
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
///                                      HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : identifyCustomButtonState
// Description     : Identify a button's state as a VisualStyles 'Parts and States' value
// 
// HWND        hCtrl            : [in] Window handle of the button
// CONST UINT  iCustomDrawState : [in] Button state provided by NM_CUSTOMDRAW notification
// 
// Return Value   : Visual Styles button state
// 
ControlsAPI
PUSHBUTTONSTATES  identifyCustomButtonState(HWND  hCtrl, CONST UINT  iCustomDrawState)
{
   PUSHBUTTONSTATES  eButtonState;     // Button state

   /// Determine the button state from various sources
   // [DISABLED] 
   if (!IsWindowEnabled(hCtrl))
      eButtonState = PBS_DISABLED;
   // [PRESSED]
   else if (SendMessage(hCtrl, BM_GETSTATE, 0, 0) INCLUDES BST_PUSHED)
      eButtonState = PBS_PRESSED;
   // [HOT]
   else if (iCustomDrawState INCLUDES CDIS_HOT)
      eButtonState = PBS_HOT;
   // [DEFAULT]
   else if (GetWindowLong(hCtrl, GWL_STYLE) INCLUDES BS_DEFPUSHBUTTON)
      eButtonState = PBS_DEFAULTED;
   // [NORMAL]
   else
      eButtonState = PBS_NORMAL;

   // Return
   return eButtonState;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                      FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : drawCustomButton
// Description     : Draws a CustomButton as part of the CustomDraw cycle
//
// NMCUSTOMDRAW*           pDrawInfo   : [in] CustomDraw data
// CONST CUSTOM_BUTTON*    pButtonData : [in] CustomButton data
// CONST PUSHBUTTONSTATES  eState      : [in] Current button state
// 
VOID   drawCustomButton(NMCUSTOMDRAW*  pDrawData, CONST CUSTOM_BUTTON*  pButtonData, CONST PUSHBUTTONSTATES  eButtonState)
{
   HIMAGELIST  hTempImageList;   // HACK: Used for drawing shrunked, blended, transparent icons
   HTHEME      hButtonTheme;     // Button theme handle
   HWND        hCtrl;            // Button window handle
   TCHAR*      szText;           // Button text
   RECT        rcButtonClient,   // Button client rectangle
               rcText,           // Text drawing rectangle
               rcIcon,           // Icon drawing rectangle
               rcContent;        // Button drawing rectangle
   SIZE        siContent;        // Size of button drawing rectangle
   UINT        iIconSize,        // Actual icon drawing size (may be shrunk to fit)
               iIconGap;         // Gap between left/top of bottom and the icon

   TRY
   {
      // Prepare
      hCtrl        = pDrawData->hdr.hwndFrom;
      szText       = utilGetWindowText(hCtrl);
      hButtonTheme = OpenThemeData(hCtrl, TEXT("BUTTON"));
      GetClientRect(hCtrl, &rcButtonClient);
      
      /// [CONTENT] Retrieve or estimate content rectangle
      if (hButtonTheme)
         GetThemeBackgroundContentRect(hButtonTheme, pDrawData->hdc, BP_PUSHBUTTON, eButtonState, &rcButtonClient, &rcContent);
      else
         SetRect(&rcContent, rcButtonClient.left + 2, rcButtonClient.top + 2, rcButtonClient.right - 2, rcButtonClient.bottom - 2);
      utilConvertRectangleToSize(&rcContent, &siContent);

      // [PRESSED] Offset rectangle
      if (eButtonState == PBS_PRESSED)
         OffsetRect(&rcContent, 1, 1);

      // [CHECK] Shrink icon if smaller than the button
      iIconSize = min(siContent.cy, pButtonData->eImageSize);
      iIconGap  = (siContent.cy - iIconSize) / 2;

      // [LEFT ALIGNED]
      if (!pButtonData->bRightAligned)
      {
         /// [ICON RECT] Centre icon vertically. Position equal distance from the left and top edges
         utilSetRectangle(&rcIcon, rcContent.left + iIconGap, rcContent.top + iIconGap, iIconSize, iIconSize);
         /// [TEXT RECT] Position to the right of the button
         SetRect(&rcText, rcIcon.right - GetSystemMetrics(SM_CXEDGE), rcContent.top, rcContent.right, rcContent.bottom);
      }
      // [RIGHT ALIGNED]
      else
      {
         /// [ICON RECT] Centre icon vertically. Position equal distance from the top and right edges
         utilSetRectangle(&rcIcon, rcContent.left + (siContent.cx - iIconSize - iIconGap), rcContent.top + iIconGap, iIconSize, iIconSize);
         /// [TEXT RECT] Position to the left of the button
         SetRect(&rcText, rcContent.left + GetSystemMetrics(SM_CXEDGE), rcContent.top, rcIcon.left, rcContent.bottom);
      }

      // [CHECK] Are we drawing themed?
      if (hButtonTheme)
         /// [THEME BACKGROUND] Use visual styles to draw the background and appropriate edges for the current state
         DrawThemeBackground(hButtonTheme, pDrawData->hdc, BP_PUSHBUTTON, eButtonState, &rcButtonClient, NULL);

      /// [STANDARD BACKGROUND] Draw basic button
      else switch (eButtonState)
      {
      default:           DrawFrameControl(pDrawData->hdc, &rcButtonClient, DFC_BUTTON, DFCS_BUTTONPUSH);                      break;
      case PBS_PRESSED:  DrawFrameControl(pDrawData->hdc, &rcButtonClient, DFC_BUTTON, DFCS_BUTTONPUSH WITH DFCS_PUSHED);     break;
      case PBS_DISABLED: DrawFrameControl(pDrawData->hdc, &rcButtonClient, DFC_BUTTON, DFCS_BUTTONPUSH WITH DFCS_INACTIVE);   break;
      case PBS_HOT:      DrawFrameControl(pDrawData->hdc, &rcButtonClient, DFC_BUTTON, DFCS_BUTTONPUSH WITH DFCS_HOT);        break;
      }

      // [CHECK] Is the button disabled?
      if (eButtonState != PBS_DISABLED)
         /// [ICON] Draw icon transparently and shrink if necessary
         DrawIconEx(pDrawData->hdc, rcIcon.left, rcIcon.top, ImageList_GetIcon(getAppImageList(pButtonData->eImageSize), pButtonData->iIconIndex, ILD_TRANSPARENT), iIconSize, iIconSize, NULL, NULL, DI_NORMAL);
      else
      {
         // Copy icon into a new ImageList to resizing the icon as necessary
         hTempImageList = ImageList_Create(iIconSize, iIconSize, ILC_COLOR32 WITH ILC_MASK, 1, 1);
         ImageList_AddIcon(hTempImageList, ImageList_GetIcon(getAppImageList(pButtonData->eImageSize), pButtonData->iIconIndex, ILD_TRANSPARENT));

         /// [ICON] Draw blended icon
         drawIcon(hTempImageList, 0, pDrawData->hdc, rcIcon.left, rcIcon.top, IS_DISABLED);
         ImageList_Destroy(hTempImageList);
      }

      // [CHECK] Are we drawing themed?
      if (lstrlen(szText) AND hButtonTheme)
         /// [THEME TEXT] Draw text using visual styles
         DrawThemeText(hButtonTheme, pDrawData->hdc, BP_PUSHBUTTON, eButtonState, szText, lstrlen(szText), DT_SINGLELINE WITH DT_VCENTER WITH DT_CENTER, 0, &rcText);
      else if (lstrlen(szText))
         /// [STANDARD TEXT] Draw text 
         DrawText(pDrawData->hdc, szText, lstrlen(szText), &rcText, DT_SINGLELINE WITH DT_VCENTER WITH DT_CENTER);

      // Cleanup and return
      CloseThemeData(hButtonTheme);
      utilDeleteString(szText);
   }
   CATCH0("");
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                   MESSAGE HANDLERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onCustomDrawButton
// Description     : CustomButton NM_CUSTOMDRAW convenience handler.
// 
// HWND                   hDialog       : [in] Window handle of the dialog containing the button
// NMHDR* (NMCUSTOMDRAW*) pMessageData  : [in] NM_CUSTOMDRAW notification data
// CONST IMAGE_TREE_SIZE  eSize         : [in] Icon size
// CONST THCAR*           szIconID      : [in] Resource ID of the desired icon
// CONST BOOL             bLeftAligned  : [in] Whether to display icon on the left, or the right
// 
ControlsAPI 
VOID  onCustomDrawButton(HWND  hDialog, NMHDR*  pMessageHeader, CONST IMAGE_TREE_SIZE  eSize, CONST TCHAR*  szIconID, CONST BOOL  bLeftAligned)
{
   PUSHBUTTONSTATES   eButtonState;    // Current Button state
   CUSTOM_BUTTON      oButtonData;     // CustomButton drawing data
   NMCUSTOMDRAW*      pDrawInfo;       // CustomDraw notification data

   // Prepare
   pDrawInfo = (NMCUSTOMDRAW*)pMessageHeader;

   // Examine draw state
   switch (pDrawInfo->dwDrawStage)
   {
   /// [INITIAL STAGE] Perform all drawing manually
   case CDDS_PREERASE:
      // Determine current button state
      eButtonState = identifyCustomButtonState(pMessageHeader->hwndFrom, pDrawInfo->uItemState);

      // Populate button data
      oButtonData.eImageSize    = eSize;
      oButtonData.iIconIndex    = getAppImageTreeIconIndex(eSize, szIconID);
      oButtonData.bRightAligned = !bLeftAligned;

      /// Perform custom button drawing
      drawCustomButton(pDrawInfo, &oButtonData, eButtonState);

      // Tell the system not to draw this button
      SetWindowLong(hDialog, DWL_MSGRESULT, CDRF_SKIPDEFAULT);
      break;
   }
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                   WINDOW PROCEDURES
/// ////////////////////////////////////////////////////////////////////////////////////////////////////
