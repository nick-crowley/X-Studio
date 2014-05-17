//
// User Interface.cpp : Windowing utility functions
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : utilCreateFont
// Description     : Create a new font with the specified attributes
// 
// CONST TCHAR* szName         : [in] Font name
// CONST UINT   iLogicalHeight : [in] Logical font height
// CONST BOOL   bBold          : [in] TRUE for bold
// CONST BOOL   bItalic        : [in] TRUE for italic
// CONST BOOL   bUnderline     : [in] TRUE for underline
// 
// Return Value   : New font, you are responsible for destroying it
// 
UtilityAPI
HFONT   utilCreateFont(HDC  hDC, CONST TCHAR*  szName, CONST UINT  iPointSize, CONST BOOL  bBold, CONST BOOL  bItalic, CONST BOOL  bUnderline)
{
   return CreateFont(utilCalculateFontHeight(hDC, iPointSize), NULL, NULL, NULL, bBold ? FW_BOLD : NULL, bItalic, bUnderline, NULL, NULL, OUT_TT_PRECIS, NULL, PROOF_QUALITY, NULL, szName);
}


/// Function name  : utilCreateFontEx
// Description     : Create a new font with the specified attributes
// 
// CONST TCHAR* szName         : [in] Font name
// CONST UINT   iLogicalHeight : [in] Logical font height
// CONST BOOL   bBold          : [in] TRUE for bold
// CONST BOOL   bItalic        : [in] TRUE for italic
// CONST BOOL   bUnderline     : [in] TRUE for underline
// 
// Return Value   : New font, you are responsible for destroying it
// 
UtilityAPI
HFONT   utilCreateFontEx(HWND  hWnd, CONST TCHAR*  szName, CONST UINT  iPointSize, CONST BOOL  bBold, CONST BOOL  bItalic, CONST BOOL  bUnderline)
{
   HFONT  hFont;
   UINT   iWeight = (!bBold ? NULL : (bBold > TRUE ? bBold : FW_BOLD));
   HDC    hDC;

   /// Create font
   hFont = CreateFont(utilCalculateFontHeight(hDC = GetDC(hWnd), iPointSize), NULL, NULL, NULL, iWeight, bItalic, bUnderline, NULL, NULL, OUT_TT_PRECIS, NULL, PROOF_QUALITY, NULL, szName);

   // Cleanup and return font
   ReleaseDC(hWnd, hDC);
   return hFont;
}


/// Function name  : utilDuplicateFont
// Description     : Create a modified version of an existing window's current font
// 
// HDC         hDC        : [in] Target window's device context
// CONST BOOL  bBold      : [in] TRUE for bold
// CONST BOOL  bItalic    : [in] TRUE for italic
// CONST BOOL  bUnderline : [in] TRUE for underline
// 
// Return Value   : New font, you are responsible for destroying it
// 
UtilityAPI
HFONT   utilDuplicateFont(HDC  hDC, CONST BOOL  bBold, CONST BOOL  bItalic, CONST BOOL  bUnderline)
{
   TEXTMETRIC  oTextMetrics;
   TCHAR       szFontName[LF_FACESIZE];
   UINT        iWeight = (!bBold ? NULL : (bBold > TRUE ? bBold : FW_BOLD));

   // Extract font name and height
   GetTextMetrics(hDC, &oTextMetrics);
   GetTextFace(hDC, LF_FACESIZE, szFontName);

   // Create font
   return CreateFont(oTextMetrics.tmHeight, NULL, NULL, NULL, iWeight, bItalic, bUnderline, NULL, NULL, OUT_TT_PRECIS, NULL, PROOF_QUALITY, NULL, szFontName);
}


/// Function name  : utilDuplicateFontEx
// Description     : Create a modified version of an existing window's current font
// 
// HDC           hDC        : [in] Target window's device context
// CONST TCHAR*  szFontName : [in] Desired font name
// CONST BOOL    bBold      : [in] TRUE for bold
// CONST BOOL    bItalic    : [in] TRUE for italic
// CONST BOOL    bUnderline : [in] TRUE for underline
// 
// Return Value   : New font, you are responsible for destroying it
// 
UtilityAPI
HFONT   utilDuplicateFontEx(HDC  hDC, CONST TCHAR*  szFontName, CONST BOOL  bBold, CONST BOOL  bItalic, CONST BOOL  bUnderline)
{
   TEXTMETRIC  oTextMetrics;
   UINT        iWeight = (!bBold ? NULL : (bBold > TRUE ? bBold : FW_BOLD));

   // Lookup text metrics
   GetTextMetrics(hDC, &oTextMetrics);

   // Return duplicate
   return CreateFont(oTextMetrics.tmHeight, NULL, NULL, NULL, iWeight, bItalic, bUnderline, NULL, NULL, OUT_TT_PRECIS, NULL, PROOF_QUALITY, NULL, szFontName);
}


/// Function name  : utilDeleteGDIObject
// Description     : Destroy a GDI object and zero it's handle
// 
// HGDIOBJ&  hObject   : [in] GDI Object
// 
UtilityAPI
VOID    utilDeleteGDIObject(HGDIOBJ&  hObject)
{
   DeleteObject(hObject);
   hObject = NULL;
}

/// Function name  : utilDeleteWindow
// Description     : Destroy a window and zero the window handle
// 
// HWND&  hWnd : [in] Reference to a window handle to be destroyed
// 
UtilityAPI
VOID    utilDeleteWindow(HWND&  hWnd)
{
   DestroyWindow(hWnd);
   hWnd = NULL;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : utilAddWindowStyle
// Description     : Add new window style(s) to a window
// 
// HWND   hWnd    : [in] Window handle of the window to alter
// DWORD  dwStyle : [in] Window style(s) to add
// 
// Return Value   : Original window style
// 
UtilityAPI
LONG    utilAddWindowStyle(HWND  hWnd, DWORD  dwStyle)
{
   return SetWindowLong(hWnd, GWL_STYLE, (GetWindowLong(hWnd, GWL_STYLE) WITH dwStyle));
}


/// Function name  : utilAddWindowStyleEx
// Description     : Add extended window style(s) to a window
// 
// HWND   hWnd    : [in] Window handle of the window to alter
// DWORD  dwStyle : [in] Window style(s) to add
// 
// Return Value   : Original window style
// 
UtilityAPI
LONG    utilAddWindowStyleEx(HWND  hWnd, DWORD  dwStyleEx)
{
   return SetWindowLong(hWnd, GWL_EXSTYLE, (GetWindowLong(hWnd, GWL_EXSTYLE) WITH dwStyleEx));
}


/// Function name  : utilAdjustDlgItemFont
// Description     : Adjusts the font of a dialog item.  Can also create an entirely new font, if required
// 
// HWND          hDialog    : [in]           Dialog containing the control
// CONST UINT    iControlID : [in]           Resource ID of control
// CONST TCHAR*  szFontName : [in][optional] New font name
// CONST UINT    iPointSize : [in][optional] New font size
// CONST BOOL    bBold      : [in]           Create bold font
// CONST BOOL    bItalic    : [in]           Create italicised font
// CONST BOOL    bUnderline : [in]           Created underling font
// 
// Return Value   : New font that was created
// 
UtilityAPI 
HFONT  utilAdjustDlgItemFont(HWND  hDialog, CONST UINT  iControlID, CONST TCHAR*  szFontName, CONST UINT  iPointSize, CONST BOOL  bBold, CONST BOOL  bItalic, CONST BOOL  bUnderline)
{
   HFONT  hNewFont;     // Font being created
   HWND   hWnd;         // Target Window
   HDC    hDC;          // Target's DC

   // Prepare
   hWnd = GetControl(hDialog, iControlID);
   hDC  = GetDC(hWnd);

   // [CHECK] Are we creating a new font?
   if (szFontName)
      /// [NAME] Create new font
      hNewFont = utilCreateFont(hDC, szFontName, iPointSize, bBold, bItalic, bUnderline);
   else
      /// [ADJUST] Adjust existing font
      hNewFont = utilDuplicateFont(hDC, bBold, bItalic, bUnderline);

   /// Set new window font
   SetWindowFont(hWnd, hNewFont, TRUE);

   // Cleanup and return font
   ReleaseDC(hWnd, hDC);
   return hNewFont;
}


/// Function name  : utilCalculateFontHeight
// Description     : Calculate the height of a font in logical units from the point size
// 
// HDC         hDC         : [in] Device context font will be displayed using
// CONST UINT  iPointSize  : [in] Desired point size
// 
// Return Value   : Logical height
// 
UtilityAPI
INT   utilCalculateFontHeight(HDC  hDC, CONST UINT  iPointSize)
{
   return -MulDiv(iPointSize, GetDeviceCaps(hDC, LOGPIXELSY), 72);  // Shamelessly ripped from MSDN
}


/// Function name  : utilCentreRectangle
// Description     : Centre a rectangle within another rectangle
// 
// CONST RECT*  pParentRect : [in]     Outer rectangle containing the child rectangle
// RECT*        pChildRect  : [in/out] Rectangle to centre
// CONST UINT   iFlags      : [in]     UCR_HORIZONTAL to centre horizontally
//                                     UCR_VERTICAL to centre vertically
// 
UtilityAPI
VOID  utilCentreRectangle(CONST RECT*  pParentRect, RECT*  pChildRect, CONST UINT  iFlags)
{
   SIZE  siChildSize,      // Size of child rectangle
         siParentSize;     // Size of parent rectangle

   // Get convenience sizes for child and parent rectangles
   utilConvertRectangleToSize(pChildRect, &siChildSize);
   utilConvertRectangleToSize(pParentRect, &siParentSize);

   // Reposition child rectangle horizontally and ensure original size
   if (iFlags INCLUDES UCR_HORIZONTAL)
   {
      pChildRect->left  = pParentRect->left + ((siParentSize.cx - siChildSize.cx) / 2);      // [FIX] BUG:1031 'Progress dialog is not centering itself within main window'
      pChildRect->right = pChildRect->left + siChildSize.cx;
   }

   // Reposition child rectangle vertically and ensure original size
   if (iFlags INCLUDES UCR_VERTICAL)
   {
      pChildRect->top    = pParentRect->top + ((siParentSize.cy - siChildSize.cy) / 2);      // [FIX] BUG:1031 'Progress dialog is not centering itself within main window'
      pChildRect->bottom = pChildRect->top + siChildSize.cy;
   }
}


/// Function name  : utilCentreWindow
// Description     : Centres a window within another window
// 
// HWND  hParentWnd  : [in] Outer window
// HWND  hChildWnd   : [in] Window to centre
// 
UtilityAPI
VOID  utilCentreWindow(HWND  hParentWnd, HWND  hChildWnd)
{
   RECT   rcParent,     // Parent window rectangle
          rcChild;      // Child window rectangle

   // Get window rectangles
   GetWindowRect(hParentWnd, &rcParent);
   GetWindowRect(hChildWnd, &rcChild);

   // Center child rectangle
   utilCentreRectangle(&rcParent, &rcChild, UCR_HORIZONTAL WITH UCR_VERTICAL);

   // Reposition child
   utilSetWindowRect(hChildWnd, &rcChild, TRUE);

}


/// Function name  : utilClientToScreenRect
// Description     : Converts a client rectangle into a window rectangle for the desired window
// 
// HWND         hWnd        : [in] Target Window
// CONST RECT*  pWindowRect : [in] Rectangle containing client co-ordinates
// 
UtilityAPI
VOID  utilClientToScreenRect(HWND  hWnd, RECT*  pClientRect)
{
   // Convert rectangle as two points
   ClientToScreen(hWnd, (POINT*)pClientRect);
   ClientToScreen(hWnd, (POINT*)&pClientRect->right);
}


/// Function name  : utilConvertDialogUnitsToPixels
// Description     : Convert dialog units for a specified dialog into pixels
// 
// HWND          hDialog       : [in]  Window handle of the dialog
// CONST POINT*  ptDialogUnits : [in]  Co-ordinates in dialog units
// POINT*        ptPixels      : [out] Equivilent co-ordinates in pixels
// 
UtilityAPI
VOID  utilConvertDialogUnitsToPixels(HWND  hDialog, CONST POINT*  ptDialogUnits, POINT*  ptPixels)
{
   TEXTMETRIC  oTextMetrics;  // Text attributes
   float       fPixelX,       // Amount of horizontal pixels
               fPixelY;       // Amount of Vertical
   HDC         hDC;           // Dialog's Device Context

   // Get Text metrics for the dialog
   hDC = GetDC(hDialog);
   GetTextMetrics(hDC, &oTextMetrics);

   // Convert DLUs to pixels
   fPixelX = (float)ptDialogUnits->x * (float)oTextMetrics.tmAveCharWidth / 4.0f;
   fPixelY = (float)ptDialogUnits->y * (float)oTextMetrics.tmHeight / 8.0f;

   // Output results as integers
   ptPixels->x = (INT)fPixelX;
   ptPixels->y = (INT)fPixelY;

   // Cleanup
   ReleaseDC(hDialog, hDC);
}


/// Function name  : utilDeferClientRect
// Description     : Adjust the size and position of a window to the dimensions of a specified rectangle
// 
// HDWP         hWindowPos
// HWND         hDialog    : [in] window handle
// CONST RECT*  pRect      : [in] Rectangle specifying size and position in dialog client co-ordinates
// CONST BOOL   bMove      : [in] Whether window is to be moved
// CONST BOOL   bResize    : [in] Whether window is to be resized
// 
UtilityAPI
VOID  utilDeferClientRect(HDWP  hWindowPos, HWND  hWnd, CONST RECT*  pRect, CONST BOOL  bMove, CONST BOOL  bResize)
{
   UINT  iFlags = SWP_NOZORDER;

   // Set flags
   if (!bMove)
      iFlags |= SWP_NOMOVE;
   if (!bResize)
      iFlags |= SWP_NOSIZE;

   // Move window
   DeferWindowPos(hWindowPos, hWnd, NULL, pRect->left, pRect->top, pRect->right - pRect->left, pRect->bottom - pRect->top, iFlags);
}


/// Function name  : utilDeferDlgItemRect
// Description     : Adjust the size and position of a control on a dialog to the dimensions of a specified rectangle
// 
// HDWP         hWindowPos
// HWND         hDialog    : [in] Dialog window handle
// CONST UINT   iControlID : [in] ID of the control
// CONST RECT*  pRect      : [in] Rectangle specifying size and position in dialog client co-ordinates
// CONST BOOL   bMove      : [in] Whether window is to be moved
// CONST BOOL   bResize    : [in] Whether window is to be resized
// 
UtilityAPI
VOID  utilDeferDlgItemRect(HDWP  hWindowPos, HWND  hDialog, CONST UINT  iControlID, CONST RECT*  pRect, CONST BOOL  bMove, CONST BOOL  bResize)
{
   UINT  iFlags = SWP_NOZORDER;

   // Set flags
   if (!bMove)
      iFlags |= SWP_NOMOVE;
   if (!bResize)
      iFlags |= SWP_NOSIZE;

   // Move window
   DeferWindowPos(hWindowPos, GetControl(hDialog, iControlID), NULL, pRect->left, pRect->top, pRect->right - pRect->left, pRect->bottom - pRect->top, iFlags);
}


/// Function name  : utilEnableDragNDrop
// Description     : Enables drag'n'drop to a window from explorer, which is blocked in Windows Vista because Explorer doesn't have admin rights
//                    
// HWND  hWnd   : [in] Window to enable drag'n'drop for
// 
// Return Value   : TRUE If successful, otherwise FALSE
// 
UtilityAPI 
BOOL   utilEnableDragNDrop(HWND  hWnd)
{
   CHANGE_WINDOWS_MESSAGE_FILTER_EX  pfnChangeWindowsMessageFilterEx;      // ChangeWindowMessageFilterEx() function pointer
   BOOL                              bResult;                              // Operation result

   // Prepare
   bResult = FALSE;

   // Manually extract the pointer to the ChangeWindowMessageFilterEx() function
   pfnChangeWindowsMessageFilterEx = (CHANGE_WINDOWS_MESSAGE_FILTER_EX)GetProcAddress(GetModuleHandle(TEXT("User32.dll")), "ChangeWindowMessageFilterEx");

   /// [DRAG N DROP] Change the window's message filter to allow drag'n'drop in windows vista
   if (pfnChangeWindowsMessageFilterEx)
   {
      bResult  = (*pfnChangeWindowsMessageFilterEx)(hWnd, WM_DROPFILES,      MSGFLT_ALLOW, NULL);
      bResult &= (*pfnChangeWindowsMessageFilterEx)(hWnd, WM_COPYDATA,       MSGFLT_ALLOW, NULL);
      bResult &= (*pfnChangeWindowsMessageFilterEx)(hWnd, WM_COPYGLOBALDATA, MSGFLT_ALLOW, NULL);
   }

   // Return result
   return bResult;
}


/// Function name  : utilEnableDlgItem
// Description     : Enable/Disable a dialog box control
// 
// HWND        hWnd        : [in] Window handle of target window
// CONST UINT  iControlID  : [in] ID of the child control
// CONST BOOL  bEnable     : [in] TRUE to enable control, FALSE to disable
// 
// Return Value   : New string, may be empty
// 
UtilityAPI
BOOL   utilEnableDlgItem(HWND  hDialog, CONST UINT  iControlID, CONST BOOL  bEnable)
{
   // Enable dialog control
   return EnableWindow(GetDlgItem(hDialog, iControlID), bEnable);
}


/// Function name  : utilEnableDlgItems
// Description     : Enables/Disables specified controls in a dialog
// 
// HWND        hDialog   : [in] Dialog
// const BOOL  bEnable : [in] TRUE/FALSE
// const UINT  iCount  : [in] Control count
// const UINT  ....    : [in] Control IDs
// 
UtilityAPI
VOID  utilEnableDlgItems(HWND  hDialog, const BOOL  bEnable, const UINT  iCount, ...)
{
   va_list  pArguments = utilGetFirstVariableArgument(&iCount);

   for (UINT  i = 0; i < iCount; i++)
   {
      EnableWindow(GetDlgItem(hDialog, utilGetCurrentVariableArgument(pArguments, UINT)), bEnable);
      pArguments = utilGetNextVariableArgument(pArguments, UINT);
   }
}


/// Function name  : utilGetDlgItemPath
// Description     : Generate a new string of MAX_PATH chars from a window's text
// 
// HWND        hWnd        : [in] Window handle of target window
// CONST UINT  iControlID  : [in] ID of the child control
// 
// Return Value   : New string, may be empty
// 
UtilityAPI
TCHAR*   utilGetDlgItemPath(HWND  hDialog, CONST UINT  iControlID)
{
   TCHAR*  szWindowText;      // Window text

   // Create null terminated buffer of MAX_PATH
   szWindowText = utilCreateEmptyString(MAX_PATH);

   // Get window text and return
   GetWindowText(GetControl(hDialog, iControlID), szWindowText, MAX_PATH);
   return szWindowText;
}


/// Function name  : utilGetDlgItemRect
// Description     : Get the rectangle of a dialog control in the dialog's client co-ordinates
// 
// HWND        hDialog      : [in]  Dialog window handle
// CONST UINT  iControlID   : [in]  ID of the control
// RECT*       pClientRect  : [out] Rectangle of the control in dialog client co-ordinates
// 
UtilityAPI
VOID  utilGetDlgItemRect(HWND  hDialog, CONST UINT  iControlID, RECT*  pClientRect)
{
   /// Get window rectangle and convert to dialog client co-ordinates
   GetWindowRect(GetControl(hDialog, iControlID), pClientRect);
   utilScreenToClientRect(hDialog, pClientRect);
}


/// Function name  : utilGetDlgItemText
// Description     : Create a new string from a window's text
// 
// HWND        hWnd        : [in] Window handle of target window
// CONST UINT  iControlID  : [in] ID of the child control
// 
// Return Value   : New string, may be empty
// 
UtilityAPI
TCHAR*   utilGetDlgItemText(HWND  hDialog, CONST UINT  iControlID)
{
   return utilGetWindowText(GetDlgItem(hDialog, iControlID));
}


/// Function name  :  utilGetDlgItemTextLength
// Description     : Retrieve the length of the window text in a dialog item
// 
// HWND        hDialog     : [in] Target dialog
// CONST UINT  iControlID  : [in] ID of the child control
// 
// Return Value   : Text length, in characters
// 
UtilityAPI
UINT   utilGetDlgItemTextLength(HWND  hDialog, CONST UINT  iControlID)
{
   // Retrieve the length of the window text of the desired dialog item
   return GetWindowTextLength(GetDlgItem(hDialog, iControlID));
}


/// Function name  : utilGetTopWindow
// Description     : Returns the top popup window, if any, otherwise the input window
// 
// HWND  hWnd : [in] Target Window
// 
// Return Value   : Window handle of top window
// 
UtilityAPI
HWND  utilGetTopWindow(HWND  hWnd)
{
   // Return popup if any, otherwise input window
   return utilEither(GetWindow(hWnd, GW_ENABLEDPOPUP), hWnd);
}


/// Function name  : utilGetWindowCursorPos
// Description     : Retrieve the cursor position in window client co=ordinates
// 
// HWND    hWnd      : [in]     Target window
// POINT*  ptCursor  : [in/out] Receives position of cursor
// 
// Return Value   : TRUE if successful, otherwise FALSE
// 
UtilityAPI
BOOL  utilGetWindowCursorPos(HWND  hWnd, POINT*  ptCursor)
{
   return GetCursorPos(ptCursor) AND ScreenToClient(hWnd, ptCursor);
}


/// Function name  : utilGetWindowHeight
// Description     : Retrieve the height of a window's client rectangle
// 
// HWND   hWnd  : [in] Target window
// 
UtilityAPI
UINT   utilGetWindowHeight(HWND  hWnd)
{
   RECT  rcWindow;

   // Retrieve client rectangle
   GetClientRect(hWnd, &rcWindow);
   
   // Return width
   return (rcWindow.bottom - rcWindow.top);
}


/// Function name  : utilGetWindowInt
// Description     : Retrieve window text as an integer
// 
// HWND   hWnd  : [in] Target window
// 
UtilityAPI
UINT  utilGetWindowInt(HWND  hWnd)
{
   TCHAR  szText[16];
   
   return GetWindowText(hWnd, szText, 16) ? utilConvertStringToInteger(szText) : 0;
}

/// Function name  : utilGetWindowSize
// Description     : Retrieve the size of a window
// 
// HWND   hWnd  : [in] Target window
// SIZE*  pSize : [in/out] Window size
// 
UtilityAPI
VOID   utilGetWindowSize(HWND  hWnd, SIZE*  pSize)
{
   RECT  rcWindow;

   // Retrieve window rectangle and measure
   GetClientRect(hWnd, &rcWindow);
   utilConvertRectangleToSize(&rcWindow, pSize);
}


/// Function name  : utilGetWindowText
// Description     : Create a new string from a window's text
// 
// HWND  hWnd   : [in] Target window
// 
// Return Value   : New string, may be empty
// 
UtilityAPI
TCHAR*   utilGetWindowText(HWND  hWnd)
{
   TCHAR*  szWindowText;      // Window text
   UINT    iTextLength;       // Window text length

   // Create null terminated buffer large enough to hold text
   iTextLength  = (GetWindowTextLength(hWnd) + 1);
   szWindowText = utilCreateEmptyString(iTextLength);

   // Get window text and return
   GetWindowText(hWnd, szWindowText, iTextLength);
   return szWindowText;
}


/// Function name  : utilGetWindowWidth
// Description     : Retrieve the width of a window's client rectangle
// 
// HWND   hWnd  : [in] Target window
// 
UtilityAPI
UINT   utilGetWindowWidth(HWND  hWnd)
{
   RECT  rcWindow;

   // Retrieve client rectangle
   GetClientRect(hWnd, &rcWindow);
   
   // Return width
   return (rcWindow.right - rcWindow.left);
}


/// Function name  : utilLaunchURL
// Description     : Launches an URL in a web-browser
// 
// HWND          hParentWnd : [in] Parent window 
// CONST TCHAR*  szURL      : [in] URL to display
// CONST INT     iShowCmd   : [in] WM_SHOW flags
// 
// Return Value   : Various - see ShellExecute
// 
UtilityAPI
BOOL  utilLaunchURL(HWND  hParentWnd, CONST TCHAR*  szURL, CONST INT  iShowCmd)
{
   // Launch forum URL
   return (BOOL)ShellExecute(hParentWnd, TEXT("open"), szURL, NULL, NULL, iShowCmd);
}


/// Function name  : utilReflectMessage
// Description     : 'Reflect' a message back to a window's parent
// 
// HWND          hWnd     : [in] Window handle of the window that received the message
// CONST UINT    iMessage : [in] Message
// CONST WPARAM  wParam   : [in] Parameter
// CONST LPARAM  lParam   : [in] Parameter
// 
// Return Value   : Results of the parent window handling the message
// 
UtilityAPI
BOOL  utilReflectMessage(HWND  hWnd, CONST UINT  iMessage, CONST WPARAM  wParam, CONST LPARAM  lParam)
{
   return SendMessage(GetParent(hWnd), iMessage, wParam, lParam);
}


/// Function name  : utilRemoveWindowStyle
// Description     : Remove a window style(s) from a window
// 
// HWND   hWnd    : [in] Window handle of the window to alter
// DWORD  dwStyle : [in] Window style(s) to add
// 
// Return Value   : Original window style
// 
UtilityAPI
LONG    utilRemoveWindowStyle(HWND  hWnd, DWORD  dwStyle)
{
   return SetWindowLong(hWnd, GWL_STYLE, (GetWindowLong(hWnd, GWL_STYLE) ^ dwStyle));
}


/// Function name  : utilRemoveWindowStyleEx
// Description     : Remove extended window style(s) from a window
// 
// HWND   hWnd    : [in] Window handle of the window to alter
// DWORD  dwStyle : [in] Window style(s) to add
// 
// Return Value   : Original window style
// 
UtilityAPI
LONG    utilRemoveWindowStyleEx(HWND  hWnd, DWORD  dwStyleEx)
{
   return SetWindowLong(hWnd, GWL_EXSTYLE, (GetWindowLong(hWnd, GWL_EXSTYLE) ^ dwStyleEx));
}


/// Function name  : utilReplaceWindowTextString
// Description     : Replace an existing string buffer with the text of a specified window
// 
// TCHAR*  &szString : [in/out] Reference to string buffer to replace
// HWND     hWnd     : [in]     Window containing the replacement text
// 
// Return Value   : Newly allocated string
// 
UtilityAPI
TCHAR*  utilReplaceWindowTextString(TCHAR*  &szString, HWND  hWnd)
{
   // Delete existing string
   utilSafeDeleteString(szString);
   // Replace with text of specified window and return input string
   szString = utilGetWindowText(hWnd);
   return szString;
}


/// Function name  : utilSafeDeleteWindow
// Description     : Checks whether the specified window exists before destroying it and zero-ing the window handle
// 
// HWND  &hWnd : [in] Window to destroy
// 
UtilityAPI
VOID  utilSafeDeleteWindow(HWND  &hWnd)
{
   // [CHECK] Only delete the window if the handle is valid
   if (hWnd AND IsWindow(hWnd))
      utilDeleteWindow(hWnd);
}


/// Function name  : utilScreenToClientRect
// Description     : Converts a window rectangle into a client rectangle for the desired window
// 
// HWND         hWnd        : [in] Target Window
// CONST RECT*  pWindowRect : [in] Rectangle containing window co-ordinates
// 
UtilityAPI
VOID  utilScreenToClientRect(HWND  hWnd, RECT*  pWindowRect)
{
   // Convert rectangle as two points
   ScreenToClient(hWnd, (POINT*)pWindowRect);
   ScreenToClient(hWnd, (POINT*)&pWindowRect->right);
}

/// Function name  : utilSetDlgItemFocus
// Description     : Sets the focus to a control on a dialog
// 
// HWND        hDialog     : [in] Dialog handle
// CONST UINT  iControlID  : [in] Dialog control ID
// 
UtilityAPI
VOID  utilSetDlgItemFocus(HWND  hDialog, CONST UINT  iControlID)
{
   // Set focus to specified window
   SetFocus(GetDlgItem(hDialog, iControlID));
}


/// Function name  : utilSetDlgItemID
// Description     : Sets the ID of control in a dialog
// 
// HWND        hDialog     : [in] Dialog handle
// CONST UINT  iControlID  : [in] Dialog control ID
// CONST UINT  iNewID      : [in] New control ID
// 
UtilityAPI
VOID  utilSetDlgItemID(HWND  hDialog, CONST UINT  iControlID, CONST UINT  iNewID)
{
   // Set ID of window
   SetWindowLong(GetDlgItem(hDialog, iControlID), GWL_ID, iNewID);
}


/// Function name  : utilSetDlgItemRect
// Description     : Adjust the size and position of a control on a dialog to the dimensions of a specified rectangle
// 
// HWND         hDialog    : [in] Dialog window handle
// CONST UINT   iControlID : [in] ID of the control
// CONST RECT*  pRect      : [in] Rectangle specifying size and position in dialog client co-ordinates
// CONST BOOL   bRepaint   : [in] TRUE to repaint the control afterwards
// 
UtilityAPI
VOID  utilSetDlgItemRect(HWND  hDialog, CONST UINT  iControlID, CONST RECT*  pRect, CONST BOOL  bRepaint)
{
   // Move window
   MoveWindow(GetDlgItem(hDialog, iControlID), pRect->left, pRect->top, pRect->right - pRect->left, pRect->bottom - pRect->top, bRepaint);
}



/// Function name  : utilSetWindowInt
// Description     : Set a window's text to the string representation of a number
// 
// HWND       hWnd      : [in] Window handle of destination window
// CONST INT  iInteger  : [in] Signed integer to display
// 
UtilityAPI
VOID  utilSetWindowInt(HWND  hWnd, CONST INT  iInteger)
{
   TCHAR  szText[16];

   // Convert to a string and set window text
   StringCchPrintf(szText, 16, TEXT("%d"), iInteger);
   SetWindowText(hWnd, szText);
}


/// Function name   : utilSetWindowProgressState
/// Windows Version : Windows 7
// Description      : Sets the state of a progress bar in a window's taskbar button
// 
// HWND        hWnd   : [in] Target window
// CONST UINT  eState : [in] New taskbar button progress bar state
// 
UtilityAPI
BOOL  utilSetWindowProgressState(HWND  hWnd, CONST UINT  eState)
{
   ITaskbarList3*  pTaskBar;     // Shell interface
   HRESULT         hResult;      // Result code

   /// Attempt to create ITaskBar3 Interface
   if (SUCCEEDED(hResult = CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_ITaskbarList3, (IInterface*)&pTaskBar)))
   {
      // Prepare
      pTaskBar->AddRef();

      // Initialise interface
      if (SUCCEEDED(hResult = pTaskBar->HrInit()))
         /// [SUCCESS] Set window progress
         hResult = pTaskBar->SetProgressState(hWnd, (TBPFLAG)eState);
      
      // Cleanup
      utilReleaseInterface(pTaskBar);
   }

   // Return result
   return SUCCEEDED(hResult);
}


/// Function name   : utilSetWindowProgressValue
/// Windows Version : Windows 7
// Description      : Displays a progress bar in a window's taskbar button, and sets the current progress
// 
// HWND        hWnd      : [in] Target window
// CONST INT   iProgress : [in] Current progress
// CONST INT   iMaximum  : [in] Maximum progress
// 
UtilityAPI
BOOL  utilSetWindowProgressValue(HWND  hWnd, CONST INT  iProgress, CONST INT  iMaximum)
{
   ITaskbarList3*  pTaskBar;     // Shell interface
   HRESULT         hResult;      // Result code

   /// Attempt to create ITaskBar3 Interface
   if (SUCCEEDED(hResult = CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_ITaskbarList3, (IInterface*)&pTaskBar)))
   {
      // Prepare
      pTaskBar->AddRef();

      // Initialise interface
      if (SUCCEEDED(hResult = pTaskBar->HrInit()))
         /// [SUCCESS] Set window progress
         hResult = pTaskBar->SetProgressValue(hWnd, iProgress, iMaximum);
      
      // Cleanup
      utilReleaseInterface(pTaskBar);
   }

   // Return result
   return SUCCEEDED(hResult);
}


/// Function name  : utilSetWindowRect
// Description     : Move and/or resize a window from co-ordinates in a rectangle
// 
// HWND         hWnd         : [in] Window to move and/or resize
// CONST RECT*  pWindowRect  : [in] Specifies the new position and size for the window. If window is a child then these are in client co-ords.
// CONST BOOL   bRepaint     : [in] Whether to repaint the window afterwards
// 
UtilityAPI
VOID  utilSetWindowRect(HWND  hWnd, CONST RECT*  pWindowRect, CONST BOOL  bRepaint)
{
   // [CHECK] Ensure window is a child window
   //ASSERT(!hWnd OR GetParent(hWnd));

   // Move window
   MoveWindow(hWnd, pWindowRect->left, pWindowRect->top, pWindowRect->right - pWindowRect->left, pWindowRect->bottom - pWindowRect->top, bRepaint);
}


/// Function name  : utilSetWindowTextf
// Description     : Sets window text using a formatted string
// 
// HWND          hWnd     : [in] Window handle
// CONST TCHAR*  szFormat : [in] Formatting string
// ...            ...     : [in] Arguments
// 
UtilityAPI
VOID  utilSetWindowTextf(HWND  hWnd, CONST TCHAR*  szFormat, ...)
{
   TCHAR*   szAssembled;   // Window text
   va_list  pArguments;

   // Prepare
   pArguments = utilGetFirstVariableArgument(&szFormat);

   // Generate and set window text
   szAssembled = utilCreateStringVf(256, szFormat, pArguments);
   SetWindowText(hWnd, szAssembled);

   // Cleanup
   utilDeleteString(szAssembled);
}


/// Function name  : utilSendNotifyMessage
// Description     : Send a custom WM_NOTIFY message
// 
// HWND        hWnd        : [in] Handle of the destination window
// CONST UINT  iMessage    : [in] Notification message
// HWND        hCtrl       : [in] Window handle of the source window
// CONST UINT  iControlID  : [in] Control ID of the source window
// 
// Return Value   : Result of the message processing handler
// 
UtilityAPI
LRESULT  utilSendNotifyMessage(HWND  hWnd, CONST UINT  iMessage, HWND  hCtrl, CONST UINT  iControlID)
{
   NMHDR  oHeader;

   // Assemble WM_NOTIFY header
   oHeader.code     = iMessage;
   oHeader.hwndFrom = hCtrl;
   oHeader.idFrom   = iControlID;

   return SendMessage(hWnd, WM_NOTIFY, iControlID, (LPARAM)&oHeader);
}

/// Function name  : utilShowDlgItem
// Description     : Show or hide a control on a dialog
// 
// HWND        hDialog    : [in] Dialog window handle
// CONST UINT  iControlID : [in] ID of the control
// CONST BOOL  bShow      : [in] TRUE to show, FALSE to hide
// 
UtilityAPI
VOID  utilShowDlgItem(HWND  hDialog, CONST UINT  iControlID, CONST BOOL  bShow)
{
   // Display/Hide dialog control
   ShowWindow(GetDlgItem(hDialog, iControlID), bShow ? SW_SHOW : SW_HIDE);
}

/// Function name  : utilTrackMouseEvent
// Description     : Convenience wrapper for tracking mouse events
// 
// HWND   hCtrl     : [in] Window to track
// DWORD  dwFlags   : [in] 'TrackMouseEvent' API function flags : TME_LEAVE, TME_HOVER, TME_CANCEL, TME_NONCLIENT
// DWORD  dwTimeout : [in] Timeout before message occurs, in milliseconds
// 
// Return Value   : TRUE if succesful, FALSE if there was an error
// 
UtilityAPI
BOOL  utilTrackMouseEvent(HWND  hCtrl, DWORD  dwFlags, DWORD  dwTimeout)
{
   TRACKMOUSEEVENT   oMouseEvent;      // Mouse event tracking data

   oMouseEvent.cbSize      = sizeof(TRACKMOUSEEVENT);
   oMouseEvent.dwFlags     = dwFlags;
   oMouseEvent.hwndTrack   = hCtrl;
   oMouseEvent.dwHoverTime = dwTimeout;

   return TrackMouseEvent(&oMouseEvent);
}

