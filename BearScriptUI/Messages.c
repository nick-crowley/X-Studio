//
// Messages.cpp : Handles the dispatching of messages between various windows
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
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : getAppWindowHandle
// Description     : Reolves the window handle from a window ID
// 
// MAIN_WINDOW_DATA*         pMainWindowData : [in] Main window data
// CONST APPLICATION_WINDOW  eAppWindow      : [in] ID of the window to resolve
// 
// Return Value   : Window handle if successful, otherwise NULL
// 
HWND  getAppWindowHandle(MAIN_WINDOW_DATA*  pMainWindowData, CONST APPLICATION_WINDOW  eAppWindow)
{
   HWND  hWnd;

   /// Determine window handle of window
   switch (eAppWindow)
   {
   case AW_MAIN:              hWnd = pMainWindowData->hMainWnd;             break;
   case AW_PROPERTIES:        hWnd = pMainWindowData->hPropertiesSheet;     break;
   case AW_OUTPUT:            hWnd = pMainWindowData->hOutputDlg;           break;
   case AW_TOOLBAR:           hWnd = pMainWindowData->hToolBar;             break;
   case AW_SEARCH:            hWnd = pMainWindowData->hSearchTabDlg;        break;
   case AW_STATUSBAR:         hWnd = pMainWindowData->hStatusBar;           break;
   case AW_DOCUMENT:          hWnd = (getActiveDocument() ? getActiveDocument()->hWnd : NULL);     break;
   case AW_DOCUMENTS_CTRL:    hWnd = pMainWindowData->hDocumentsTab;        break;
   default:                   hWnd = NULL;                                  break;
   }

   // Return handle
   return hWnd;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : postAppClose
// Description     : Commands the main window to close
// 
// CONST MAIN_WINDOW_STATE  eState : [in] Current closure state
// 
VOID  postAppClose(CONST MAIN_WINDOW_STATE  eState)
{
   // Send WM_CLOSE to the main window
   postAppMessage(AW_MAIN, UM_MAIN_WINDOW_CLOSING, eState, NULL);
}


/// Function name  : postAppMessage
// Description     : Facilitates communication between the various windows in the application
// 
// CONST APPLICATION_WINDOW  eAppWindow : [in] ID of the destination window
// CONST UINT                iMessage   : [in] Message to send
// WPARAM                    wParam     : [in] First parameter
// LPARAM                    lParam     : [in] Second parameter
// 
VOID  postAppMessage(CONST APPLICATION_WINDOW  eAppWindow, CONST UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   HWND    hDestination;       // Window handle of the destination window

   /// Determine window handle of destination window
   if (hDestination = getAppWindowHandle(getMainWindowData(), eAppWindow))
      // [SUCCESS] Post message
      PostMessage(hDestination, iMessage, wParam, lParam);
}


/// Function name  : postScriptCallSearchComplete
// Description     : Informs the properties window a script-call search has completed
// 
// CONST APPLICATION_WINDOW  eAppWindow   : [in] ID of the destination window
// AVL_TREE*                 pCallersTree : [in] ScriptDependency tree
// 
VOID  postScriptCallSearchComplete(CONST APPLICATION_WINDOW  eAppWindow, AVL_TREE*  pCallersTree)
{
   HWND    hDestination;       // Window handle of the destination window

   /// Determine window handle of destination window
   if (hDestination = getAppWindowHandle(getMainWindowData(), eAppWindow))
      // [SUCCESS] Post message
      PostMessage(hDestination, UN_SCRIPTCALL_OPERATION_COMPLETE, NULL, (LPARAM)pCallersTree);
}


/// Function name  : sendAppMessage
// Description     : Facilitates communication between the various windows in the application
// 
// CONST APPLICATION_WINDOW  eAppWindow : [in] ID of the destination window
// CONST UINT                iMessage   : [in] Message to send
// WPARAM                    wParam     : [in] First parameter
// LPARAM                    lParam     : [in] Second parameter
// 
// Return Value   : Return value of message, if the window exists, otherwise NULL
// 
UINT  sendAppMessage(CONST APPLICATION_WINDOW  eAppWindow, CONST UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   HWND    hDestination;       // Window handle of the destination window

   /// Determine window handle of destination window
   hDestination = getAppWindowHandle(getMainWindowData(), eAppWindow);

   /// Send message
   return (hDestination ? SendMessage(hDestination, iMessage, wParam, lParam) : NULL);
}


/// Function name  : sendDocumentSwitched
// Description     : Notify a window that there is a new active document
// 
// CONST APPLICATION_WINDOW  eWindow      : [in] Destination window
// DOCUMENT*                 pNewDocument : [in] New active document if any, otherwise NULL
// 
VOID  sendDocumentSwitched(CONST APPLICATION_WINDOW  eWindow, DOCUMENT*  pNewDocument)
{
   // Send UN_DOCUMENT_SWITCHED to the window
   sendAppMessage(eWindow, UN_DOCUMENT_SWITCHED, NULL, (LPARAM)pNewDocument);
}


/// Function name  : sendDocumentUpdated
// Description     : Notify a window that the active document has been updated
// 
// CONST APPLICATION_WINDOW  eWindow : [in] Destination window
// 
VOID  sendDocumentUpdated(CONST APPLICATION_WINDOW  eWindow)
{
   // Send UN_DOCUMENT_UPDATED to the window
   sendAppMessage(eWindow, UN_DOCUMENT_UPDATED, NULL, NULL);
}


/// Function name  : sendDocumentPropertyUpdated
// Description     : Notify a window that a property of the active document has been updated
// 
// CONST APPLICATION_WINDOW  eWindow      : [in] Destination window
// CONST UINT                iControlID   : [in] ID of the property page control that has changed
// 
VOID  sendDocumentPropertyUpdated(CONST APPLICATION_WINDOW  eWindow, CONST UINT  iControlID)
{
   // Send UN_PROPERTY_UPDATED to the window and pass the control ID
   sendAppMessage(eWindow, UN_PROPERTY_UPDATED, iControlID, NULL);
}


/// Function name  : sendDocumentOperationComplete
// Description     : Notify a document that a load or save operation has been completed
// 
// HWND                 hDocumentWnd    : [in] Destination document window
// DOCUMENT_OPERATION*  pOperationData  : [in] Data for the completed operation
// 
VOID  sendDocumentOperationComplete(HWND  hDocumentWnd, DOCUMENT_OPERATION*  pOperationData)
{
   // Send a UN_OPERATION_COMPLETE notification
   SendMessage(hDocumentWnd, UN_OPERATION_COMPLETE, NULL, (LPARAM)pOperationData);
}


/// Function name  : sendOperationComplete
// Description     : Notify a window that a threaded operation has completed
// 
// CONST APPLICATION_WINDOW  eWindow         : [in] Destination window
// OPERATION_DATA*           pOperationData  : [in] Data for the recently completed operation
// 
VOID  sendOperationComplete(CONST APPLICATION_WINDOW  eWindow, OPERATION_DATA*  pOperationData)
{
   // Send a UN_OPERATION_COMPLETE notification
   sendAppMessage(eWindow, UN_OPERATION_COMPLETE, NULL, (LPARAM)pOperationData);
}

