//
// Message Crackers.h : Custom message crackers not provided by the Windows API
//

#pragma once

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Macro: utilSetListViewColumn
// Description: Fill a LVCOLUMN structure from a co-ordinate plus a width and height
//
// CONST LVCOLUMN* pColumn  : [in/out] ListView column structure
// CONST UINT      nSubItem : [in]     Column SubItem
// CONST TCHAR*    szText   : [in]     Column text
// CONST UINT      iWidth   : [in]     Column width
//
#define   utilSetListViewColumn(pColumn,nSubItem,szText,iWidth)         { (pColumn)->mask = LVCF_WIDTH WITH LVCF_TEXT WITH LVCF_SUBITEM;   \
                                                                           (pColumn)->cx = (INT)(iWidth); (pColumn)->pszText = (TCHAR*)(szText); (pColumn)->iSubItem = (INT)(nSubItem); }

/// Macro: utilSetWindowID
// Description: Changes the ID of a window
//
// HWND        hWnd  : [in] Window handle
// CONST UINT  iID   : [in] New window ID
//
#define   utilSetWindowID(hWnd, iID)                                    SetWindowLong(hWnd, GWL_ID, iID)


/// Macro: CallDialogProc
// Description: Calls a dialog procedure
//
// DLGPROC     pfnDialogProc : [in] Dialog procedure
// HWND        hDialog       : [in] Dialog handle
// ...
//
#define   CallDialogProc(pfnDialogProc,hDialog,iMessage,wParam,lParam)  CallWindowProc((WNDPROC)pfnDialogProc, hDialog, iMessage, wParam, lParam)


/// Macro: GetControl
// Description: Convenience descriptor for 'GetDlgItem'
//
#define   GetControl                                                    GetDlgItem


/// Macro: InvalidateDlgItem
// Description: Invalidates a dialog control
//
// HWND        hWnd       : [in] Window handle
// UINT        iControlID : [in] Control ID
// CONST BOOL  bErase     : [in] Whether to also erase the background
//
#define   InvalidateDlgItem(hDialog, iControlID, bErase)                InvalidateRect(GetControl(hDialog, iControlID), NULL, bErase)


/// Macro: InvalidateWindow
// Description: Invalidates a window
//
// HWND        hWnd   : [in] Window handle
// CONST BOOL  bErase : [in] Whether to also erase the background
//
#define   InvalidateWindow(hWnd, bErase)                                InvalidateRect(hWnd, NULL, bErase)


/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       MESSAGE CRACKERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// ////////////////////////////////////////////////////////////////////////////////////////////////////
//                                       BUTTON CONTROL
// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Macro: Button_SetIcon
//  Description: Sets a button's icon
//
//  HWND   hCtrl  : [in] Button window handle
//  HICON  hIcon  : [in] Icon handle
//
//  Return Value : Previous icon handle, or NULL
//
#define   Button_SetIcon(hCtrl, hIcon)                               (HICON)SendMessage(hCtrl, BM_SETIMAGE, IMAGE_ICON, (LPARAM)(hIcon))

/// Macro: Button_SetBitmap
//  Description: Sets a button's bitmaps
//
//  HWND     hCtrl  : [in] Button window handle
//  HBITMAP  hIcon  : [in] Bitmap handle
//
//  Return Value : Previous bitmap handle, or NULL
//
#define   Button_SetBitmap(hCtrl, hBitmap)                           (HBITMAP)SendMessage(hCtrl, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)(hBitmap))


/// Macro: ComboBox_GetItemHeightEx
//  Description: Retrieves the height of a ComboBox item or the edit box
//
//  HWND  hCtrl : [in] Button window handle
//  UINT  iItem : [in] -1 for the edit box, 0 for items in a Fixed OwnerDraw ComboBox, Zero-based index of the item in a Variable OwnerDraw comboBox
//
//  Return Value : Previous bitmap handle, or NULL
//
#define    ComboBox_GetItemHeightEx(hCtrl, iItem)                    SendMessage(hCtrl, CB_GETITEMHEIGHT, (iItem), NULL)


// ////////////////////////////////////////////////////////////////////////////////////////////////////
//                                       EDIT CONTROL
// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Macro: Edit_GetSelText
//  Description: Retrieves selected text
//
//  HWND   hCtrl    : [in] Edit window handle
//  TCHAR* szBuffer : [in] Buffer to hold text
//
//  Return Value : Number of chars copied
//
#define    Edit_GetSelText(hCtrl, szBuffer)                          SendMessage(hCtrl, EM_GETSELTEXT, NULL, (LPARAM)szBuffer)

/// Macro: Edit_GetSelEx
//  Description: Retrieves selection position
//
//  HWND   hCtrl  : [in] Edit window handle
//  ULONG* pStart : [in] Index of start of selection
//  ULONG* pEnd   : [in] Index of end of selection
//
//  Return Value : pStart and pEnd packed together
//
#define    Edit_GetSelEx(hCtrl, pStart, pEnd)                        SendMessage(hCtrl, EM_GETSEL, (WPARAM)(pStart), (LPARAM)(pEnd))

// ////////////////////////////////////////////////////////////////////////////////////////////////////
//                                         LISTVIEW
// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Macro: ListView_GetSelected
//  Description: Retrieves index of first selected item
//
//  HWND   hCtrl  : [in] ListView
//
//  Return Value : Item Index, or -1
//
#define    ListView_GetSelected(hCtrl)                               ListView_GetNextItem(hCtrl, -1, LVNI_ALL | LVNI_SELECTED)

// ////////////////////////////////////////////////////////////////////////////////////////////////////
//                                       RICH EDIT CONTROL
// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Macro: RichEdit_CanRedo
//  Description: Checks whether control can Redo
//
//  HWND  hCtrl   : [in] RichEdit window handle
//
// Return Value: TRUE/FALSE
//
#define    RichEdit_CanRedo(hCtrl)                                   SendMessage(hCtrl, EM_CANREDO, NULL, NULL)

/// Macro: RichEdit_FindTextEx
//  Description: Finds RichEdit text
//
//  HWND         hCtrl   : [in] RichEdit window handle
//  UINT         iFlags  : [in] Flags
//  FINDTEXTEX*  pData   : [in] Search data
//
//  Return Value : Character index or -1
//
#define RichEdit_FindTextEx(hCtrl, iFlags, pData)                    SendMessage(hCtrl, EM_FINDTEXTEX, iFlags, (LPARAM)(FINDTEXTEX*)(pData))

/// Macro: RichEdit_GetCharFormat
//  Description: Gets current character format
//
//  HWND         hCtrl   : [in] RichEdit window handle
//  UINT         iFlags  : [in] Flags
//  CHARFORMAT*  pFormat : [in] Formatting characteristics
//
//  Return Value : Value of dwMask memeber
//
#define    RichEdit_GetCharFormat(hCtrl, iFlags, pFormat)            SendMessage(hCtrl, EM_GETCHARFORMAT, iFlags, (LPARAM)(CHARFORMAT*)(pFormat))

/// Macro: RichEdit_GetEventMask
//  Description: Gets the notifications sent by a RichEdit
//
//  HWND   hCtrl : [in] RichEdit window handle
//
//  Return Value : Previous event mask
//
#define    RichEdit_GetEventMask(hCtrl)                              SendMessage(hCtrl, EM_GETEVENTMASK, NULL, NULL)

/// Macro: RichEdit_GetOLEInterface
//  Description: Retrieves OLE Interface
//
//  HWND           hCtrl      : [in] RichEdit window handle
//  IRichEditOle*  pInterface : [in] IRichEditOle* Interface
//
//  Return Value : TRUE/FALSE
//
#define    RichEdit_GetOLEInterface(hCtrl, pInterface)               SendMessage(hCtrl, EM_GETOLEINTERFACE, NULL, (LPARAM)(IRichEditOle*)(pInterface))

/// Macro: RichEdit_GetParagraphFormat
//  Description: Gets current paragraph format
//
//  HWND         hCtrl   : [in] RichEdit window handle
//  PARAFORMAT*  pFormat : [in] Formatting characteristics
//
//  Return Value : Value of dwMask memeber
//
#define    RichEdit_GetParagraphFormat(hCtrl, pFormat)               SendMessage(hCtrl, EM_GETPARAFORMAT, NULL, (LPARAM)(PARAFORMAT*)(pFormat))

/// Macro: RichEdit_HideSelection
//  Description: Shows/Hides the current selection
//
//  HWND   hCtrl : [in] RichEdit window handle
//  BOOL   bHide : [in] TRUE to hide, FALSE to reveal
//
#define    RichEdit_HideSelection(hCtrl, bHide)                       SendMessage(hCtrl, EM_HIDESELECTION, bHide, NULL)

/// Macro: RichEdit_SetBackgroundColour
//  Description: Sets background colour
//
//  HWND      hCtrl    : [in] RichEdit window handle
//  BOOL      bSystem  : [in] TRUE to use system default, FALSE to use custom
//  COLORREF  clColour : [in] Custom colour
//
//  Return Value : Previous colour
//
#define    RichEdit_SetBackgroundColour(hCtrl, bSystem, clColour)    SendMessage(hCtrl, EM_SETBKGNDCOLOR, bSystem, (LPARAM)(clColour))

/// Macro: RichEdit_SetCharFormat
//  Description: Sets current character format
//
//  HWND         hCtrl   : [in] RichEdit window handle
//  UINT         iFlags  : [in] Flags
//  CHARFORMAT*  pFormat : [in] Formatting characteristics
//
//  Return Value : TRUE/FALSE
//
#define    RichEdit_SetCharFormat(hCtrl, iFlags, pFormat)            SendMessage(hCtrl, EM_SETCHARFORMAT, iFlags, (LPARAM)(CHARFORMAT*)(pFormat))

/// Macro: RichEdit_SetEventMask
//  Description: Sets the notifications sent by a RichEdit
//
//  HWND   hCtrl : [in] RichEdit window handle
//  UINT   iMask : [in] Event mask
//
//  Return Value : Previous event mask
//
#define    RichEdit_SetEventMask(hCtrl, iMask)                       SendMessage(hCtrl, EM_SETEVENTMASK, NULL, (LPARAM)(iMask))

/// Macro: RichEdit_SetFontSize
//  Description: Sets the font size
//
//  HWND   hCtrl   : [in] RichEdit window handle
//  UINT   iPoints : [in] Font size
//
//  Return Value : TRUE/FALSE
//
#define    RichEdit_SetFontSize(hCtrl, iPoints)                      SendMessage(hCtrl, EM_SETFONTSIZE, iPoints, NULL)

/// Macro: RichEdit_SetOLECallback
//  Description: Sets OLE Callback interface
//
//  HWND                   hCtrl      : [in] RichEdit window handle
//  IRichEditOleCallback*  pInterface : [in] IRichEditCallback* Interface
//
//  Return Value : TRUE/FALSE
//
#define    RichEdit_SetOLECallback(hCtrl, pInterface)               SendMessage(hCtrl, EM_SETOLECALLBACK, NULL, (LPARAM)(IRichEditOleCallback*)(pInterface))

/// Macro: RichEdit_SetParagraphFormat
//  Description: Sets current paragraph format
//
//  HWND         hCtrl   : [in] RichEdit window handle
//  PARAFORMAT*  pFormat : [in] Formatting characteristics
//
//  Return Value : TRUE/FALSE
//
#define    RichEdit_SetParagraphFormat(hCtrl, pFormat)               SendMessage(hCtrl, EM_SETPARAFORMAT, NULL, (LPARAM)(PARAFORMAT*)(pFormat))

// ////////////////////////////////////////////////////////////////////////////////////////////////////
//                                       TOOLBAR CONTROL
// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Macro: Toolbar_AutoSize
//  Description: Causes a toolbar to be resized
//
//  HWND  hCtrl  : [in] Toolbar window handle
//
#define    Toolbar_AutoSize(hWnd)                                    SendMessage(hWnd, TB_AUTOSIZE, NULL, NULL)


/// Macro: Toolbar_AddBitmap
//  Description: Adds one or more images to the list of button images available for a toolbar. 
//
//  HWND         hCtrl        : [in] Toolbar window handle
//  UINT         iBitmapCount : [in] Number of bitmaps
//  TBADDBITMAP* pBitmapData  : [in] Bitmap data
//
//  Return Value : Index of first image if successful, otherwise -1
//
#define    Toolbar_AddBitmap(hWnd, iBitmapCount, pBitmapData)        SendMessage(hWnd, TB_ADDBITMAP, iBitmapCount, (LPARAM)(TBADDBITMAP*)pBitmapData)


/// Macro: Toolbar_AddButtons
//  Description: Adds one or more buttons to a toolbar.
//
//  HWND       hCtrl        : [in] Toolbar window handle
//  UINT       iButtonCount : [in] Number of buttons
//  TBBUTTON*  pButtonData  : [in] Button data
//
//  Return Value : TRUE is successful, FALSE otherwise
//
#define    Toolbar_AddButtons(hWnd, iButtonCount, pButtonData)       SendMessage(hWnd, TB_ADDBUTTONS, iButtonCount, (LPARAM)(TBBUTTON*)pButtonData)


/// Macro: Toolbar_ButtonStructSize
//  Description: Sets the size of the TBBUTTON structure
//
//  HWND  hCtrl  : [in] Toolbar window handle
//
#define    Toolbar_ButtonStructSize(hWnd)                            SendMessage(hWnd, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), NULL)


/// Macro: Toolbar_CheckButton
//  Description: Checks or unchecks a toolbar button
//
//  HWND  hCtrl      : [in] Toolbar window handle
//  UINT  iButtonID  : [in] Command ID of the button to check/uncheck
//  BOOL  bEnabled   : [in] TRUE to check, FALSE to uncheck
//
//  Return Value : TRUE is successful, FALSE otherwise
//
#define    Toolbar_CheckButton(hWnd, iButtonID, bChecked)            SendMessage(hWnd, TB_CHECKBUTTON, iButtonID, bChecked)


/// Macro: Toolbar_EnableButton
//  Description: Enables or disables a toolbar button
//
//  HWND  hCtrl      : [in] Toolbar window handle
//  UINT  iButtonID  : [in] Command ID of the button to enable/disable
//  BOOL  bEnabled   : [in] TRUE to enable, FALSE to disable
//
//  Return Value : TRUE is successful, FALSE otherwise
//
#define    Toolbar_EnableButton(hWnd, iButtonID, bEnabled)           SendMessage(hWnd, TB_ENABLEBUTTON, iButtonID, bEnabled)


/// Macro: Toolbar_SetBitmapSize
//  Description: Sets the dimensions of the bitmaps for each buttons
//
//  HWND  hCtrl   : [in] Toolbar window handle
//  UINT  iWidth  : [in] Button Bitmap width, in pixels
//  UINT  iHeight : [in] Button Bitmap height, in pixels
//
//  Return Value : TRUE is successful, FALSE otherwise
//
#define    Toolbar_SetBitmapSize(hWnd, iWidth, iHeight)              SendMessage(hWnd, TB_SETBITMAPSIZE, NULL, MAKE_LONG(iWidth, iHeight))

// ////////////////////////////////////////////////////////////////////////////////////////////////////
//                                       TOOLTIP CONTROL
// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Macro: Tooltip_Activate
//  Description: Enables/Disabled a tooltip
//
//  HWND  hCtrl    : [in] Tooltip window handle
//  BOOL  bEnabled : [in] TRUE / FALSE
//
#define    Tooltip_Activate(hWnd, bEnabled)                          SendMessage(hWnd, TTM_ACTIVATE, bEnabled, NULL)


/// Macro: Tooltip_AdjustRect
//  Description: Converts between background and content tooltip rectangle s
//
//  HWND  hCtrl                : [in] Tooltip window handle
//  BOOL  bContentToBackground : [in] TRUE to convert Content->Background, FALSE to convert Background->Content
//  RECT* pRect                : [in] Rectangle to convert
//
//  Return Value : TRUE if succesful, otherwise FALSE
//
#define    Tooltip_AdjustRect(hWnd, bContentToBackground, pRect)     SendMessage(hWnd, TTM_ADJUSTRECT, bContentToBackground, (LPARAM)(pRect))


/// Macro: Tooltip_SetDisplayTime
//  Description: Sets one of a tooltip's display tiumers
//
//  HWND  hCtrl   : [in] Tooltip window handle
//  ENUM  eTime   : [in] Type of timer to set : TTDT_AUTOPOP, TTDT_INITIAL, TTDT_RESHOW or TTDT_AUTOMATIC
//  UINT  iDelay  : [in] Delay time in milliseconds  (Maximum is 32,768)
//
//  Return Value : TRUE if succesful, otherwise FALSE
//
#define    Tooltip_SetDisplayTime(hWnd, eTime, iDelay)               SendMessage(hWnd, TTM_SETDELAYTIME, eTime, iDelay)


// ////////////////////////////////////////////////////////////////////////////////////////////////////
//                                       TEMPLATE
// ////////////////////////////////////////////////////////////////////////////////////////////////////


/// Macro: 
//  Description: 
//
//  HWND  hCtrl  : [in]  window handle
//               : [in] 
//
//  Return Value : TRUE if succesful, otherwise FALSE
//
