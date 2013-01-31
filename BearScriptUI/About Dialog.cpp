//
// About Dialog.cpp : About dialog
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



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

BOOL  setDialogStrings(HWND  hDialog, const UINT  iStringID, const UINT  iSkipCount, ...)
{
   CONST TCHAR  *szClasses[] = { WC_STATIC, WC_BUTTON, WC_LINK },      // Supported window classes
                *szCtrlText;        // Text for current control
   TCHAR        *szAllStrings,      // All strings
                *pIterator;         // tokeniser
   TCHAR         szCtrlClass[32];   // Window class of current control
   UINT          iCtrlID,
                 iIndex = 0,
                 iSkipIDs[10];
          
   // Load dialog string
   if ((szAllStrings = utilLoadString(getResourceInstance(), iStringID, 4096)) == NULL)
      return FALSE;

   // Enumerate IDs of controls to skip, if any
   for (va_list  pArgument = utilGetFirstVariableArgument(&iSkipCount); iIndex < iSkipCount AND iIndex < 10; pArgument = utilGetNextVariableArgument(pArgument, UINT))
      iSkipIDs[iIndex++] = utilGetCurrentVariableArgument(pArgument, UINT);
   
   // Extract first text
   szCtrlText = utilTokeniseString(szAllStrings, "$", &pIterator);

   /// Iterate through dialog child windows
   for (HWND  hCtrl = GetFirstChild(hDialog); szCtrlText AND GetClassName(hCtrl, szCtrlClass, 32) AND (iCtrlID = GetWindowID(hCtrl)); hCtrl = GetNextSibling(hCtrl))
   {
      BOOL  bMatch = FALSE,
            bSkip  = FALSE;

      // [CHECK] Ensure control ID isn't in the skip list
      for (UINT  i = 0; !bSkip AND i < iSkipCount; i++)
         if (bSkip = (iCtrlID == iSkipIDs[i]))
            break;

      // [CHECK] Ensure control is: STATIC, BUTTON or LINK
      for (UINT  iClass = 0; !bSkip AND !bMatch AND iClass < 3; iClass++)
         if (bMatch = utilCompareStringVariables(szCtrlClass, szClasses[iClass]))
            break;
      
      // [FAILED] Move to next child
      if (bSkip OR !bMatch)
         continue;

      /// [FOUND] Set control text and extract next string
      SetWindowText(hCtrl, szCtrlText);
      szCtrlText = utilGetNextToken("$", &pIterator);
   }

   // Cleanup and return
   utilDeleteString(szAllStrings);
   return TRUE;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : initAboutDialog
// Description     : Loads the dialog icons and centres the dialog
// 
// HWND  hDialog   : [in] About dialog
// 
// Return Value   : TRUE
// 
BOOL   initAboutDialog(HWND  hDialog)
{
   HICON   hIcon;

   // Set 'Exscriptor' icon
   hIcon = (HICON)LoadImage(getResourceInstance(), TEXT("EXSCRIPTOR_ICON"), IMAGE_ICON, 32, 32, LR_LOADTRANSPARENT);
   Static_SetIcon(GetControl(hDialog, IDC_ABOUT_EXSCRIPTOR_ICON), hIcon);

   // Set 'DoubleShow' icon
   hIcon = (HICON)LoadImage(getResourceInstance(), TEXT("DOUBLESHADOW_ICON"), IMAGE_ICON, 48, 48, LR_LOADTRANSPARENT);
   Static_SetIcon(GetControl(hDialog, IDC_ABOUT_DOUBLESHADOW_ICON), hIcon);

   // [OWNER DRAW]
   utilAddWindowStyle(GetDlgItem(hDialog, IDC_ABOUT_DIALOG_TITLE), SS_OWNERDRAW WITH CSS_CENTRE);
   utilAddWindowStyle(GetDlgItem(hDialog, IDC_ABOUT_DIALOG_VERSION_HEADING), SS_OWNERDRAW WITH CSS_CENTRE);
   utilAddWindowStyle(GetDlgItem(hDialog, IDC_ABOUT_DIALOG_THANKS_HEADING), SS_OWNERDRAW);
   
   // Centre dialog
   utilCentreWindow(getAppWindow(), hDialog);

   /// Localize dialog strings
   setDialogStrings(hDialog, IDS_ABOUT_DIALOG_STRINGS, 4, IDC_ABOUT_DIALOG_ICON, IDC_ABOUT_EXSCRIPTOR_ICON, IDC_ABOUT_DOUBLESHADOW_ICON, IDC_ABOUT_XUNIVERSE_BITMAP);

   // Return TRUE
   return TRUE;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onAboutDialog_Command
// Description     : Handles closing the dialog and the license dialog
// 
// HWND        hDialog       : [in] About dialog
// CONST UINT  iControlID    : [in] ID of the control
// CONST UINT  iNotification : [in] Notification
// 
// Return Value   : TRUE if processed, FALSE otherwise
// 
BOOL  onAboutDialog_Command(HWND  hDialog, CONST UINT  iControlID, CONST UINT  iNotification, HWND  hCtrl)
{
   BOOL   bResult;

   // Prepare
   bResult = FALSE;

   // Examine source
   switch (iControlID)
   {
   /// [LICENSE DIALOG]
   case IDC_ABOUT_LICENSE:
      DialogBox(getResourceInstance(), TEXT("LICENSE_DIALOG"), hDialog, dlgprocLicenseDialog);
      bResult = TRUE;
      break;

   /// [OK/CANCEL] Close dialog
   case IDOK:
   case IDCANCEL:
      EndDialog(hDialog, IDOK);
      bResult = TRUE;
      break;
   }

   // Return result
   return bResult;
}


/// Function name  : onAboutDialog_Destroy
// Description     : Destroys the dialog icons
// 
// HWND  hDialog   : [in] About dialog
// 
// Return Value   : TRUE
// 
BOOL  onAboutDialog_Destroy(HWND  hDialog)
{
   HICON   hIcon;

   // Destroy 'Exscriptor' icon
   hIcon = Static_GetIcon(GetControl(hDialog, IDC_ABOUT_EXSCRIPTOR_ICON));
   DestroyIcon(hIcon);

   // Destroy 'DoubleShow' icon
   hIcon = Static_GetIcon(GetControl(hDialog, IDC_ABOUT_DOUBLESHADOW_ICON));
   DestroyIcon(hIcon);

   // Return TRUE
   return TRUE;
}


/// Function name  : onAboutDialog_LinkClick
// Description     : Launches the specified link
// 
// HWND        hDialog    : [in] About dialog
// CONST UINT  iControlID : [in] Control sending the notification
// NMLINK*     pLinkInfo  : [in] Link notification header
// 
// Return Value   : TRUE
// 
BOOL  onAboutDialog_LinkClick(HWND  hDialog, CONST UINT  iControlID, NMLINK*  pHeader)
{
   LITEM*  pLink;    // Convenience pointer

   // Prepare
   pLink = &pHeader->item;

   // Prepare
   pLink->mask  = LIF_URL WITH LIF_ITEMINDEX;
   pLink->iLink = 0;
   
   /// Retrieve the URL and launch in browser
   SendDlgItemMessage(hDialog, iControlID, LM_GETITEM, NULL, (LPARAM)pLink);
   utilLaunchURL(getAppWindow(), pLink->szUrl, SW_SHOWMAXIMIZED);

   /// Mark link as 'visited'
   pLink->mask  = LIF_STATE WITH LIF_ITEMINDEX;
   pLink->state = LIS_VISITED WITH LIS_ENABLED;
   pLink->stateMask = pLink->state;
   SendDlgItemMessage(hDialog, iControlID, LM_SETITEM, NULL, (LPARAM)pLink);
   
   // Return TRUE
   return TRUE;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : dlgprocAboutBox
// Description     : 'About Box' Dialog window procedure
// 
INT_PTR   dlgprocAboutBox(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   NMHDR*   pHeader;   // SysLink notification header
   BOOL     bResult;   // Operation result

   // Prepare
   bResult = FALSE;

   // Examine message
   switch (iMessage)
   {
   /// [CREATION]
   case WM_INITDIALOG:
      bResult = initAboutDialog(hDialog);
      break;

   /// [DESTRUCTION]
   case WM_DESTROY:
      bResult = onAboutDialog_Destroy(hDialog);
      break;

   /// [COMMAND PROCESSING]
   case WM_COMMAND:
      bResult = onAboutDialog_Command(hDialog, LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
      break;
      
   /// [NOTIFICATION]
   case WM_NOTIFY:
      // Prepare
      pHeader = (NMHDR*)lParam;

      // Examine source control
      switch (pHeader->idFrom)
      {
      case IDC_ABOUT_LINK_EGOSOFT:
      case IDC_ABOUT_LINK_FORUMS:
         // [CLICK] Launch link
         if (pHeader->code == NM_CLICK)
            bResult = onAboutDialog_LinkClick(hDialog, pHeader->idFrom, (NMLINK*)pHeader);         
         break;
      }
      break;

   /// [OWNER DRAW]
   case WM_DRAWITEM:
      switch (wParam)
      {
      case IDC_ABOUT_DIALOG_ICON:            bResult = onOwnerDrawStaticIcon(lParam, szMainWindowClass, 96);    break;
      case IDC_ABOUT_DIALOG_TITLE:           bResult = onOwnerDrawStaticTitle(lParam);                          break;
      case IDC_ABOUT_DIALOG_VERSION_HEADING:
      case IDC_ABOUT_DIALOG_THANKS_HEADING:  bResult = onOwnerDrawStaticHeading(lParam);                        break;
      }
      break;

   /// [VISUAL STYLE]
   default:
      bResult = dlgprocVistaStyleDialog(hDialog, iMessage, wParam, lParam);
      break;
   }

   // Return result
   return bResult;
}


/// Function name  : dlgprocLicenseDialog
// Description     : License dialog procedure
// 
INT_PTR   dlgprocLicenseDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   //NMHDR*  pHeader;

   // Examine message
   switch (iMessage)
   {
   /// [CREATION]
   case WM_INITDIALOG:
      // Use application name for window title
      SetWindowText(hDialog, getAppName());

      // Centre dialog
      utilCentreWindow(getAppWindow(), hDialog);

      // Ensure OK button is at the top of the Z-Order
      BringWindowToTop(GetControl(hDialog, IDOK));
      return TRUE;

   /// [COMMAND PROCESSING]
   case WM_COMMAND:
      switch (LOWORD(wParam))
      {
      // [OK/CANCEL] Close dialog
      case IDOK:
      case IDCANCEL:
         if (HIWORD(wParam) == BN_CLICKED)
         {
            EndDialog(hDialog, IDOK);
            return TRUE;
         }
      }
      break;

   /// [VISUAL STYLES]
   case WM_CTLCOLORDLG:
   case WM_CTLCOLORSTATIC:
   case WM_CTLCOLORBTN:
      return (INT_PTR)onDialog_ControlColour((HDC)wParam, COLOR_WINDOW);

   /// [VISUAL STYLES]
   
      //return (INT_PTR)GetStockObject(WHITE_BRUSH);
   }

   return FALSE;
}

