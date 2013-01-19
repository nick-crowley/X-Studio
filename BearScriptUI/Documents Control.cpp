//
// Documents Control (new).cpp : FILE_DESCRIPTION
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

/// Function name  : createDocumentsControl
// Description     : Create a new Documents control
// 
// HWND             hParentWnd     : [in] Parent window for the documents tab control
// CONST RECT*      pRect          : [in] Bounding rectangle for the new tab control
// CONST UINT       iControlID     : [in] Control ID for the documents tab control
// 
// Return type : Window handle of the new tab control, if successful, otherwise NULL
//
HWND   createDocumentsControl(HWND  hParentWnd, CONST RECT*  pRect, CONST UINT  iControlID)
{
   DOCUMENTS_DATA*  pWindowData;       // Window data
   SIZE             siCtrlSize;        // Size of the new control
   RECT             rcCtrlRect;        // Rect of new control
   HWND             hTabCtrl;          // Convenience pointer
   
   // [TRACK]
   TRACK_FUNCTION();

   // Prepare
   rcCtrlRect = *pRect;

   // Reduce size of target rectangle to create border effect
   InflateRect(&rcCtrlRect, -2, -2);
   utilConvertRectangleToSize(&rcCtrlRect, &siCtrlSize);

   /// Create DocumentsControl (Also creates/stores window data)
   hTabCtrl = CreateWindow(szDocumentsCtrlClass, TEXT("Documents Control"), WS_CHILD WITH WS_TABSTOP WITH TCS_TABS WITH TCS_MULTILINE WITH TCS_TOOLTIPS WITH WS_CLIPCHILDREN, //  TCS_SINGLELINE 
                           rcCtrlRect.left, rcCtrlRect.top, siCtrlSize.cy, siCtrlSize.cy, 
                           hParentWnd, (HMENU)iControlID, getAppInstance(), NULL);

   // [DEBUG]
   ERROR_CHECK("creating documents control", hTabCtrl);

   // [CHECK] Ensure control was created successfully
   if (hTabCtrl)
   {
      // Prepare
      pWindowData = getDocumentsControlData(hTabCtrl);

      // Set font and ImageList
      TabCtrl_SetImageList(hTabCtrl, pWindowData->hImageList);
      SetWindowFont(hTabCtrl, pWindowData->hTitleFont, FALSE);

      /// Display window
      UpdateWindow(hTabCtrl);
      ShowWindow(hTabCtrl, SW_SHOW);

      // [DEBUG]
      DEBUG_WINDOW("Documents Control", hTabCtrl);
   }

   // Return window handle or NULL
   END_TRACKING();
   return hTabCtrl;
}


/// Function name  : createDocumentsControlData
// Description     : Creates the underlying data for the documents control
// 
// Return Value   : New documents control data
// 
DOCUMENTS_DATA*  createDocumentsControlData(HWND  hTabCtrl)
{
   DOCUMENTS_DATA*  pCtrlData;

   // Create new object
   pCtrlData = utilCreateEmptyObject(DOCUMENTS_DATA);

   // Set properties
   pCtrlData->hTabCtrl = hTabCtrl;

   // Create document list
   pCtrlData->pDocumentList = createList(destructorDocument);

   // Return new object
   return pCtrlData;
}


/// Function name  : deleteDocumentsControlData
// Description     : Destroys documents control data
// 
// DOCUMENTS_DATA*  &pDocumentsData   : [in] Documents data
// 
VOID  deleteDocumentsControlData(DOCUMENTS_DATA*  &pDocumentsData)
{
   // [TRACK]
   TRACK_FUNCTION();

   // Delete Documents list
   deleteList(pDocumentsData->pDocumentList);

   // Destroy ImageList and title font
   ImageList_Destroy(pDocumentsData->hImageList);
   DeleteFont(pDocumentsData->hTitleFont);

   // Delete calling object
   utilDeleteObject(pDocumentsData);

   // [TRACK]
   END_TRACKING();
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : findDocumentByIndex
// Description     : Lookup a DOCUMENT by index
// 
// HWND         hTabCtrl : [in]  Document control handle
// CONST UINT   iIndex   : [in]  Zero-based index of the document to be retrieved
// DOCUMENT*   &pOutput  : [out] Resultant DOCUMENT if found, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BOOL  findDocumentByIndex(HWND  hTabCtrl, CONST UINT  iIndex, DOCUMENT*  &pOutput)
{
   DOCUMENTS_DATA*  pWindowData;    // Window data

   // Prepare
   pWindowData = getDocumentsControlData(hTabCtrl);

   // Lookup document
   return findListObjectByIndex(pWindowData->pDocumentList, iIndex, (LPARAM&)pOutput);
}


/// Function name  : findDocumentIndexByPath
// Description     : Determines the index of the specified document
// 
// HWND          hTabCtrl     : [in]  Documents control
// CONST TCHAR*  szFullPath   : [in]  Full path of the desired document
// INT&          iOutput      : [out] Zero-based index if found, otherwise -1
// 
// Return Value   : TRUE if found, otherwise FALSE
// 
BOOL   findDocumentIndexByPath(HWND  hTabCtrl, CONST TCHAR*  szFullPath, INT&  iOutput)
{
   DOCUMENTS_DATA* pWindowData;    // Window data
   LIST_ITERATOR*  pIterator;      // List iterator
   DOCUMENT*       pDocument;      // List iterator

   // [TRACK]
   TRACK_FUNCTION();

   // Prepare
   pWindowData = getDocumentsControlData(hTabCtrl);
   iOutput     = -1;
   
   /// Iterate through document list
   for (pIterator = createListIterator(pWindowData->pDocumentList); getCurrentItem(pIterator, (LPARAM&)pDocument); moveNextItem(pIterator))
   {
      // [CHECK] Compare paths
      if (utilCompareStringVariables(getDocumentPath(pDocument), szFullPath))
      {
         /// [FOUND] Set result and abort
         iOutput = pIterator->iIndex;
         break;
      }
   }

   // Cleanup and return TRUE if found
   deleteListIterator(pIterator);
   END_TRACKING();
   return (iOutput != -1);
}


/// Function name  : findDocumentIndexByValue
// Description     : Determines the index of the specified document
// 
// DOCUMENTS_DATA*  pDocumentsData  : [in]  Window data
// CONST DOCUMENT*  pTargetDocument : [in]  Document to retrieve the index for
// INT&             iOutput         : [out] Zero-based index if found, otherwise -1
// 
// Return Value   : TRUE if found, otherwise FALSE
// 
BOOL   findDocumentIndexByValue(DOCUMENTS_DATA*  pWindowData, CONST DOCUMENT*  pTargetDocument, INT&  iOutput)
{
   LIST_ITERATOR*  pIterator;             // List iterator
   DOCUMENT*       pCurrentDocument;      // List iterator

   // [TRACK]
   TRACK_FUNCTION();

   // Prepare
   iOutput = -1;

   /// Iterate through document list
   for (pIterator = createListIterator(pWindowData->pDocumentList); getCurrentItem(pIterator, (LPARAM&)pCurrentDocument); moveNextItem(pIterator))
   {
      // [CHECK] Compare current item
      if (pCurrentDocument == pTargetDocument)
      {
         /// [FOUND] Set result and abort
         iOutput = pIterator->iIndex;
         break;
      }
   }

   // Cleanup and return TRUE if found
   deleteListIterator(pIterator);
   END_TRACKING();
   return (iOutput != -1);
}


/// Function name  : getActiveDocument
// Description     : Retrieve the data for the active document
//
// Return type : Active document if any, otherwise NULL
//
DOCUMENT*   getActiveDocument()
{
   DOCUMENTS_DATA*  pWindowData;

   // Prepare
   pWindowData = getDocumentsControlData(getMainWindowData()->hDocumentsTab);

   // Return active document
   return (pWindowData ? pWindowData->pActiveDocument : NULL);
}


/// Function name  : getActiveLanguageDocument
// Description     : Retrieve active LanguageDocument
//
// Return type : Active Language document if any, otherwise NULL
//
LANGUAGE_DOCUMENT*   getActiveLanguageDocument()
{
   DOCUMENTS_DATA*  pWindowData;
   DOCUMENT*        pDocument;

   // Prepare
   pWindowData = getDocumentsControlData(getMainWindowData()->hDocumentsTab);
   pDocument   = pWindowData->pActiveDocument;

   // Return LanguageDocument or NULL
   return (pDocument AND pDocument->eType == DT_LANGUAGE ? (LANGUAGE_DOCUMENT*)pDocument : NULL);
}


/// Function name  : getActiveScriptCodeEdit
// Description     : Active CodeEdit
// 
// Return Value   : 
// 
HWND  getActiveScriptCodeEdit()
{
   SCRIPT_DOCUMENT*  pScriptDocument;

   // [CHECK] Lookup ScriptDocument
   pScriptDocument = getActiveScriptDocument();

   // Return CodeEdit
   return (pScriptDocument ? pScriptDocument->hCodeEdit : NULL);
}


/// Function name  : getActiveScriptDocument
// Description     : Retrieve active ScriptDocument
//
// Return type : Active ScriptDocument if any, otherwise NULL
//
SCRIPT_DOCUMENT*   getActiveScriptDocument()
{
   DOCUMENTS_DATA*  pWindowData;
   DOCUMENT*        pDocument;

   // Prepare
   pWindowData = getDocumentsControlData(getMainWindowData()->hDocumentsTab);
   pDocument   = pWindowData->pActiveDocument;

   // Return ScriptDocument or NULL
   return (pDocument AND pDocument->eType == DT_SCRIPT ? (SCRIPT_DOCUMENT*)pDocument : NULL);
}


/// Function name  : getDocumentCount
// Description     : Retrieves the active document, if any
// 
// DOCUMENTS_DATA*  pDocumentsData   : [in] Window data
// 
// Return Value   : Currently active DOCUMENT if any, otherwise NULL
// 
UINT  getDocumentCount()
{
   DOCUMENTS_DATA*  pWindowData;

   // Prepare
   pWindowData = getDocumentsControlData(getMainWindowData()->hDocumentsTab);

   // Return document list item count
   return getListItemCount(pWindowData->pDocumentList);
}


/// Function name  : getDocumentsControlData
// Description     : Retrieve documents control window data
// 
// HWND  hTabCtrl : [in] Documents control
// 
// Return Value   : Documents control window data
// 
DOCUMENTS_DATA*  getDocumentsControlData(HWND  hCtrl)
{
   return (hCtrl ? (DOCUMENTS_DATA*)GetWindowLong(hCtrl, sizeof(LONG)) : NULL);
}


/// Function name  : isAnyDocumentModified
// Description     : Determines whether any document is modified
// 
// HWND  hTabCtrl : [in] Documents control
//
// Return Value   : TRUE if there is at least one modified document, otherwise FALSE
// 
BOOL   isAnyDocumentModified(HWND  hTabCtrl)
{
   DOCUMENTS_DATA*  pWindowData;     // Window data
   DOCUMENT*        pDocument;       // Current document
   BOOL             bModified;
   
   // Prepare
   pWindowData = getDocumentsControlData(hTabCtrl);
   bModified   = FALSE;
   
   /// [DOCUMENTS] Iterate through document list
   for (LIST_ITEM*  pIterator = getListHead(pWindowData->pDocumentList); !bModified AND (pDocument = extractListItemPointer(pIterator, DOCUMENT)); pIterator = pIterator->pNext)
      // [CHECK] Abort if document is modified...
      if (isModified(pDocument))
         bModified = TRUE;

   /// [PROJECT] Check project, if any
   if (!bModified)
      bModified = isModified(getActiveProject());
   
   // Return state
   return bModified;
}


/// Function name  : isDocumentOpen
// Description     : Checks whther a file is already is already open
// 
// CONST TCHAR*  szFullPath : [in] Full path of file to test
// 
// Return Value   : TRUE if open, otherwise FALSE
// 
BOOL   isDocumentOpen(CONST TCHAR*  szFullPath)
{
   INT  iDummyIndex;

   return findDocumentIndexByPath(getMainWindowData()->hDocumentsTab, szFullPath, iDummyIndex);
}


/// Function name  : setActiveDocument
// Description     : Updates the 'ActiveDocument' and displays the specified document
// 
// DOCUMENTS_DATA*  pWindowData : [in] Window data
// DOCUMENT*        pDocument   : [in] Target document
// 
VOID  setActiveDocument(DOCUMENTS_DATA*  pWindowData, DOCUMENT*  pDocument)
{
   // [TRACK]
   TRACK_FUNCTION();

   // Replace ActiveDocument
   pWindowData->pActiveDocument = pDocument;

   // [CHECK] Are we switching to a Document or NULL?
   if (pDocument)
   {
      /// [DOCUMENT] Resize document appropriately
      updateDocumentSize(pWindowData, pDocument);

      /// Display new document and set focus
      ShowWindow(pDocument->hWnd, SW_SHOWNORMAL);
      UpdateWindow(pDocument->hWnd);
      SetFocus(pDocument->hWnd);

      // [DEBUG]
      DEBUG_WINDOW("ActiveDocument", pDocument->hWnd);
   }

   /// [EVENT] Document switched
   sendDocumentSwitched(AW_DOCUMENTS_CTRL, pDocument);

   // [TRACK]
   END_TRACKING();
}


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : appendDocument
// Description     : Appends a Document to the documents control
// 
// HWND       hTabCtrl  : [in] Tab control
// DOCUMENT*  pDocument : [in] Document to insert
// 
VOID  appendDocument(HWND  hTabCtrl, DOCUMENT*  pDocument)
{
   DOCUMENTS_DATA*  pWindowData;    // Documents control
   TCITEM           oTabItem;       // New Tab data 
   SIZE             siTabCtrl;      // Size of the tab control
   
   // [TRACK]
   TRACK_FUNCTION();

   // Prepare
   pWindowData = getDocumentsControlData(hTabCtrl);

   // [CHECK] Ensure document count == tab count
   ASSERT(getDocumentCount() == TabCtrl_GetItemCount(hTabCtrl));

   // Define tab
   oTabItem.mask    = TCIF_TEXT WITH TCIF_IMAGE;
   oTabItem.pszText = (TCHAR*)getDocumentFileName(pDocument);
   oTabItem.iImage  = pDocument->eType;

   /// Add DOCUMENT to document list
   appendObjectToList(pWindowData->pDocumentList, (LPARAM)pDocument);

   /// Append new tab, but don't initially select
   TabCtrl_InsertItem(hTabCtrl, getDocumentCount() - 1, (LPARAM)&oTabItem);

   // [CHECK] Is this the first document?
   if (getDocumentCount() == 1)
      /// [SUCCESS] Set as ActiveDocument
      setActiveDocument(pWindowData, pDocument);
   else
   {
      // [FAILURE] Resize the active document in case a new row of tabs has been opened
      utilGetWindowSize(hTabCtrl, &siTabCtrl);
      PostMessage(hTabCtrl, WM_SIZE, NULL, MAKE_LONG(siTabCtrl.cx, siTabCtrl.cy));    // BUG_FIX: Using updateDocumentSize(pWindowData, pDocument) didn't work, TabCtrl must need to update something internal first...
   }

   // [TRACK]
   END_TRACKING();
}


/// Function name  : closeAllDocuments
// Description     : Attempts to close/save all open documents
// 
// CONST BOOL  bExcludeActiveDocument : [in] Whether to leave the active document open 
// 
BOOL   closeAllDocuments(HWND  hTabCtrl, CONST BOOL  bExcludeActive)
{
   DOCUMENTS_DATA*    pWindowData;     // Documents data
   DOCUMENT*          pDocument;       // Document list iterator
   UINT               iDocumentCount;  // Document count

   // [TRACK]
   TRACK_FUNCTION();

   // Prepare  
   pWindowData = getDocumentsControlData(hTabCtrl);

   // [CHECK] Ensure there are documents to close
   if (iDocumentCount = getDocumentCount())
   {
      // Iterate through all open documents
      for (INT  iIndex = (iDocumentCount - 1); findDocumentByIndex(hTabCtrl, iIndex, pDocument); iIndex--)
      {
         // [CHECK] Skip the active document, if appropriate
         if (bExcludeActive AND pDocument == getActiveDocument())
            continue;
         
         /// [CHECK] Attempt to save or discard document
         if (!closeDocumentByIndex(hTabCtrl, iIndex))
            // [CANCELLED] Abort 
            break;
      }
   }

   // Recalculate document count
   iDocumentCount = getDocumentCount();

   // [CLOSING] Have all documents been closed?
   if (isAppClosing() AND !iDocumentCount)
      /// [EVENT] Fire UM_MAIN_WINDOW_CLOSING with state MWS_DOCUMENTS_CLOSED
      postAppClose(MWS_DOCUMENTS_CLOSED);

   // Return TRUE if all documents were closed
   END_TRACKING();
   return (iDocumentCount == 0);
}


/// Function name  : closeDocumentByIndex
// Description     : Immediately closes a document or initiates a save-then-close operation on a document
// 
// HWND        hTabCtrl : [in] Document control handle
// CONST UINT  iIndex   : [in] Zero based index of the document
// 
// Return Value : TRUE if document was closed or save-with-close, otherwise FALSE
//
BOOL  closeDocumentByIndex(HWND  hTabCtrl, CONST UINT  iIndex)
{
   DOCUMENTS_DATA*  pWindowData;    // Window data
   DOCUMENT*        pDocument;      // Target document
   CLOSURE_TYPE     eResult;        // Document closure type
   
   // Prepare
   TRACK_FUNCTION();
   pWindowData = getDocumentsControlData(hTabCtrl);
   eResult     = DCT_DISCARD;

   // [CHECK] Lookup associated Document object
   if (findDocumentByIndex(hTabCtrl, iIndex, pDocument))
   {
      /// [CHECK] Determine how to close the document
      switch (eResult = closeDocument(pDocument))
      {
      // [SAVE/DISCARD] Save/destroy document and return TRUE
      case DCT_SAVE_CLOSE:  commandSaveDocument(getMainWindowData(), pDocument, TRUE, NULL);  break;
      case DCT_DISCARD:     removeDocument(pWindowData, pDocument, iIndex);                   break;
      }
   }

   // Return TRUE if closing sequence was not aborted
   END_TRACKING();
   return (eResult != DCT_ABORT);
}


/// Function name  : displayDocumentByIndex
// Description     : Changes the active tab and displays the appropriate document
// 
// HWND        hTabCtrl : [in] Document control handle
// CONST UINT  iIndex   : [in] Zero based index of the document to display
// 
VOID  displayDocumentByIndex(HWND  hTabCtrl, CONST UINT  iIndex)
{
   DOCUMENTS_DATA*  pWindowData;    // Window data
   DOCUMENT*        pDocument;      // Target Document

   // [TRACK]
   TRACK_FUNCTION();

   // Prepare
   pWindowData  = getDocumentsControlData(hTabCtrl);

   // Lookup document and ensure its not already active
   if (findDocumentByIndex(hTabCtrl, iIndex, pDocument) AND pDocument != getActiveDocument())
   {
      // [SUCCESS] Hide current active document (if any)
      if (getActiveDocument())
         ShowWindow(getActiveDocument()->hWnd, SW_HIDE);

      /// Ensure correct tab is selected
      TabCtrl_SetCurSel(pWindowData->hTabCtrl, iIndex);     // Already active if user has changed tab manually

      /// Display new document
      setActiveDocument(pWindowData, pDocument);
   }

   // [TRACK]
   END_TRACKING();
}


/// Function name  : displayNextDocument
// Description     : Switches to the next document
// 
// HWND        hTabCtrl : [in] Document control handle
// CONST UINT  iIndex   : [in] Zero based index of the document to display
// 
VOID  displayNextDocument(HWND  hTabCtrl)
{
   UINT   iDocumentCount,
          iNextIndex;

   // Prepare
   TRACK_FUNCTION();
   iDocumentCount = getDocumentCount();

   // [CHECK]
   if (iDocumentCount > 1)
   {
      // Prepare
      iNextIndex = TabCtrl_GetCurSel(hTabCtrl) + 1;
      
      // [LAST DOCUMENT] Display first document
      if (iNextIndex >= iDocumentCount)
         iNextIndex = 0;

      // Display next document
      displayDocumentByIndex(hTabCtrl, iNextIndex);
   }

   // [TRACK]
   END_TRACKING();
}


/// Function name  : removeDocument
// Description     : Removes the destroys the specified document. Should not be called directly, except by 'closeDocumentByIndex'
// 
// DOCUMENTS_DATA*  pWindowData : [in] Documents window data
// DOCUMENT*        pDocument   : [in] Target document
// CONST UINT       iIndex      : [in] Index of target document
// 
VOID  removeDocument(DOCUMENTS_DATA*  pWindowData, DOCUMENT*  pDocument, CONST UINT  iIndex)
{
   UINT   iReplacementIndex;    // Index of document to display instead

   // [TRACK]
   TRACK_FUNCTION();

   // [CHECK] Are we closing the ActiveDocument?
   if (pDocument == getActiveDocument())
   {
      // [CHECK] Is there a replacement for the ActiveDocument?
      if (getDocumentCount() > 1)
      {
         // [SUCCESS] Display document on the right, otherwise one on the left
         iReplacementIndex = (iIndex == getDocumentCount() - 1 ? (iIndex - 1) : (iIndex + 1));
         displayDocumentByIndex(pWindowData->hTabCtrl, iReplacementIndex);       /// [EVENT] Fires UM_DOCUMENT_SWITCHED
      }
      else
         // [FAILED] Reset active document
         setActiveDocument(pWindowData, NULL);     /// [EVENT] Fires UM_DOCUMENT_SWITCHED
   }

   /// Remove tab and Destroy Document
   TabCtrl_DeleteItem(pWindowData->hTabCtrl, iIndex);        // [WARNING] TabCtrl_DeleteItem causes document to be erased but not invalidated
   removeObjectFromListByValue(pWindowData->pDocumentList, (LPARAM)pDocument);
   deleteDocument(pDocument);

   // [CHECK] Is there an active document?
   if (getActiveDocument())
      // [ACTIVE DOCUMENT] Resize in case a row of tabs has been removed
      updateDocumentSize(pWindowData, getActiveDocument());

   // [TRACK]
   END_TRACKING();
}


/// Function name  : updateDocumentSize
// Description     : Resizes a document to reflect current Documents control size
// 
// DOCUMENTS_DATA*  pDocumentsData  : [in] Documents data
// DOCUMENT*        pDocument       : [in] Target Document
// 
VOID  updateDocumentSize(DOCUMENTS_DATA*  pWindowData, DOCUMENT*  pDocument)
{
   POINT  ptDocument;    // Top-Left co-ordinate of document rectangle
   RECT   rcDocument;    // Document rectangle
   SIZE   siDocument;    // Size of rectangle

   // Calculate document rectangle
   GetWindowRect(pWindowData->hTabCtrl, &rcDocument);
   TabCtrl_AdjustRect(pWindowData->hTabCtrl, FALSE, &rcDocument);

   // Shrink to create border
   InflateRect(&rcDocument, -6, -6);

   // Convert to client co-ordinates
   utilConvertRectangleToPoint(&rcDocument, &ptDocument);
   ScreenToClient(pWindowData->hTabCtrl, &ptDocument);

   /// Resize document window
   utilConvertRectangleToSize(&rcDocument, &siDocument);
   SetWindowPos(pDocument->hWnd, NULL, ptDocument.x, ptDocument.y, siDocument.cx, siDocument.cy, SWP_NOZORDER);
}


/// Function name  : updateDocumentTitle
// Description     : Updates the document's tab to reflect it's current title and modified status
// 
// DOCUMENT*   pDocument : [in] Target Document
// 
VOID    updateDocumentTitle(DOCUMENT*  pDocument)
{
   DOCUMENTS_DATA*  pWindowData;   // Window data
   TCITEM           oTabData;      // Document Tab data 
   INT              iIndex;        // Document index

   // [TRACK]
   TRACK_FUNCTION();

   // Prepare
   pWindowData      = getDocumentsControlData(getMainWindowData()->hDocumentsTab);
   oTabData.pszText = utilCreateEmptyString(128);
   oTabData.mask    = TCIF_TEXT;

   /// [CHECK] Is the document modified?
   if (isModified(pDocument))
      // [MODIFIED] Append an asterix to the filename
      StringCchPrintf(oTabData.pszText, 128, TEXT("%s *"), getDocumentFileName(pDocument));
   else
      // [UN-MODIFED] Use filename
      StringCchCopy(oTabData.pszText, 128, getDocumentFileName(pDocument));
      
   /// Display new title
   findDocumentIndexByValue(pWindowData, pDocument, iIndex);
   TabCtrl_SetItem(pWindowData->hTabCtrl, iIndex, &oTabData);

   // Refresh document window  (Document is erased but not invalidated by TabCtrl_SetItem)
   utilRedrawWindow(pDocument->hWnd);
      
   // Cleanup
   utilDeleteString(oTabData.pszText);
   END_TRACKING();
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onDocumentsControl_ContextMenu
// Description     : Displays the tab context menu and implements it's commands
// 
// DOCUMENTS_DATA*  pWindowData  : [in] Documents data
// CONST POINT*     ptClick      : [in] Cursor position (in screen co-ordinates)
// 
BOOL  onDocumentsControl_ContextMenu(DOCUMENTS_DATA*  pWindowData, CONST POINT*  ptClick)
{
   TCHITTESTINFO  oHitTest;            // Document tab HitTest
   CUSTOM_MENU*   pCustomMenu;         // Custom menu
   DOCUMENT*      pDocument;           // Document that was clicked, if any
   INT            iDocumentIndex;      // Index of the document that was clicked

   // [TRACK]
   TRACK_FUNCTION();

   // Prepare
   oHitTest.pt    = *ptClick;
   oHitTest.flags = TCHT_ONITEM;

   /// [CHECK] Has the user clicked a tab heading?
   ScreenToClient(pWindowData->hTabCtrl, &oHitTest.pt);
   iDocumentIndex = TabCtrl_HitTest(pWindowData->hTabCtrl, &oHitTest);

   // Lookup document (Will fails if user did not click a tab heading)
   if (findDocumentByIndex(pWindowData->hTabCtrl, iDocumentIndex, pDocument))
   {
      // Generate custom menu
      pCustomMenu = createCustomMenu(TEXT("DIALOG_MENU"), TRUE, IDM_DOCUMENT_POPUP);

      /// Enable/Disable menu items
      EnableMenuItem(pCustomMenu->hPopup, IDM_FILE_SAVE,                    isModified(pDocument) ? MF_ENABLED : MF_DISABLED);
      EnableMenuItem(pCustomMenu->hPopup, IDM_WINDOW_CLOSE_OTHER_DOCUMENTS, getDocumentCount() > 1 ? MF_ENABLED : MF_DISABLED);

      // Enable/Disable project items
      EnableMenuItem(pCustomMenu->hPopup, IDM_DOCUMENT_ADD_PROJECT,    getActiveProject() AND !isDocumentInProject(pDocument) ? MF_ENABLED : MF_DISABLED);
      EnableMenuItem(pCustomMenu->hPopup, IDM_DOCUMENT_REMOVE_PROJECT, getActiveProject() AND isDocumentInProject(pDocument)  ? MF_ENABLED : MF_DISABLED);

      /// Display context menu and return choice immediately
      switch (TrackPopupMenu(pCustomMenu->hPopup, TPM_RETURNCMD WITH TPM_TOPALIGN WITH TPM_LEFTALIGN, ptClick->x, ptClick->y, NULL, pWindowData->hTabCtrl, NULL))
      {
      // [SAVE/CLOSE/CLOSE-OTHER] Perform relevant command
      case IDM_FILE_SAVE:                     commandSaveDocument(getMainWindowData(), pDocument, FALSE, NULL);  break;
      case IDM_FILE_CLOSE:                    closeDocumentByIndex(pWindowData->hTabCtrl, iDocumentIndex);       break;
      case IDM_WINDOW_CLOSE_OTHER_DOCUMENTS:  closeAllDocuments(pWindowData->hTabCtrl, TRUE);                    break;

      // [ADD/REMOVE PROJECT] TODO
      case IDM_DOCUMENT_ADD_PROJECT:          addDocumentToProject(pDocument);         break;
      case IDM_DOCUMENT_REMOVE_PROJECT:       removeDocumentFromProject(pDocument);    break;
         break;
      }

      // Cleanup
      deleteCustomMenu(pCustomMenu);
   }
   
   // Return TRUE if handled, otherwise FALSE
   END_TRACKING();
   return (iDocumentIndex != -1);
}


/// Function name  : onDocumentsControl_Create
// Description     : Stores and initialises the window data
// 
// DOCUMENTS_DATA*  pWindowData  : [in] Documents data
// 
VOID  onDocumentsControl_Create(DOCUMENTS_DATA*  pWindowData)
{
   WNDCLASS    oBaseClass;    // Window class of base Tab control
   HDC         hDC;           // Control DC

   // [TRACK]
   TRACK_FUNCTION();

   // Prepare
   hDC = GetDC(pWindowData->hTabCtrl);

   // Store window data
   SetWindowLong(pWindowData->hTabCtrl, sizeof(LONG), (LONG)pWindowData);

   // Store base window procedure
   GetClassInfo(NULL, WC_TABCONTROL, &oBaseClass);
   pWindowData->pfnBaseWindowProc = oBaseClass.lpfnWndProc;

   /// Create 'bold' tab font
   pWindowData->hTitleFont = utilCreateFont(hDC, TEXT("MS Sans Serif"), 9, TRUE, FALSE, FALSE);

   /// Create 32-bit colour replacement tab ImageList
   pWindowData->hImageList = utilCreateImageList(getResourceInstance(), 20, 3, "NEW_SCRIPT_FILE_ICON", "NEW_LANGUAGE_FILE_ICON", "OPEN_SAMPLES_ICON");

   // Cleanup
   ReleaseDC(pWindowData->hTabCtrl, hDC);

   // [TRACK]
   END_TRACKING();
}

/// Function name  : onDocumentsControl_Destroy
// Description     : Destroys window data and any remaining documents
// 
// DOCUMENTS_DATA*  pWindowData  : [in] Documents data
// 
VOID  onDocumentsControl_Destroy(DOCUMENTS_DATA*  &pWindowData)
{
   ERROR_STACK*  pException;

   // [TRACK]
   TRACK_FUNCTION();

   /// [GUARD BLOCK]
   __try
   {
      // [CHECK] Ensure there are no documents
      ASSERT(!getDocumentCount());

      // Release window font
      SetWindowFont(pWindowData->hTabCtrl, NULL, FALSE);

      /// Sever and destroy window data
      SetWindowLong(pWindowData->hTabCtrl, sizeof(DOCUMENTS_DATA*), NULL);
      deleteDocumentsControlData(pWindowData);
   }
   /// [EXCEPTION HANDLER]
   __except (generateExceptionError(GetExceptionInformation(), pException))
   {
      // [ERROR] "An unidentified and unexpected critical error has occurred while closing the documents window"
      enhanceError(pException, ERROR_ID(IDS_EXCEPTION_DESTROY_DOCUMENTS_CTRL));
      displayException(pException);

      // Sever window data
      SetWindowLong(pWindowData->hTabCtrl, sizeof(DOCUMENTS_DATA*), NULL);
   }

   // [TRACK]
   END_TRACKING();
}


/// Function name  : onDocumentsControl_DocumentUpdated
// Description     : Marks the active document as 'modified' when its contents change, refreshes the properties dialog + toolbar
// 
// DOCUMENTS_DATA*  pWindowData  : [in] Documents data
// 
VOID  onDocumentsControl_DocumentUpdated(DOCUMENTS_DATA*  pWindowData)
{
   DOCUMENT*   pDocument;     // Active document

   // [TRACK]
   TRACK_FUNCTION();

   // [CHECK] Ensure active document exists
   ASSERT(getActiveDocument());

   // Prepare
   pDocument = getActiveDocument();
   
   /// [NON-VIRTUAL] Set the 'modified' flag and update title
   if (!pDocument->bVirtual)
      setDocumentModifiedFlag(pDocument, TRUE);

   /// Inform MainWindow and PropertiesSheet
   sendDocumentUpdated(AW_PROPERTIES);
   sendDocumentUpdated(AW_MAIN);

   // [TRACK]
   END_TRACKING();
}


/// Function name  : onMainWindow_DocumentPropertyUpdated
// Description     : Marks the active document as 'modified' when a property changes
//
// DOCUMENTS_DATA*  pWindowData  : [in] Documents data
// CONST UINT       iControlID   : [in] ID of the control representing the property that has changed
// 
VOID  onDocumentsControl_DocumentPropertyUpdated(DOCUMENTS_DATA*  pWindowData, CONST UINT  iControlID)
{
   DOCUMENT*   pDocument;     // Active document

   // [TRACK]
   TRACK_FUNCTION();

   // [CHECK] Ensure active document exists
   ASSERT(getActiveDocument());

   // Prepare
   pDocument = getActiveDocument();

   // [CHECK] Ensure document is modified
   if (!isModified(pDocument) OR iControlID == IDC_SCRIPT_NAME) // [SPECIAL CASE] Always refresh title when script name changes
   {
      /// [SUCCESS] Set modified and redraw the document title
      pDocument->bModified = TRUE;
      updateDocumentTitle(pDocument);
   }
   
   /// Inform MainWindow, SearchWindow and Document
   sendDocumentPropertyUpdated(AW_MAIN, iControlID);  
   sendDocumentPropertyUpdated(AW_SEARCH, iControlID); 
   sendDocumentPropertyUpdated(AW_DOCUMENT, iControlID); 

   // [TRACK]
   END_TRACKING();
}


/// Function name  : onDocumentsControl_DocumentSwitched
// Description     : Informs various windows that the active document has been switched
// 
// DOCUMENTS_DATA*  pWindowData  : [in] Documents data
// DOCUMENT*        pNewDocument : [in] New active document if any, otherwise NULL
// 
VOID  onDocumentsControl_DocumentSwitched(DOCUMENTS_DATA*  pWindowData, DOCUMENT*  pNewDocument)
{
   // [TRACK]
   TRACK_FUNCTION();

   /// Inform PropertiesDialog, MainWindow and SearchWindow
   sendDocumentSwitched(AW_PROPERTIES, pNewDocument);
   sendDocumentSwitched(AW_MAIN, pNewDocument);  
   sendDocumentSwitched(AW_SEARCH, pNewDocument); 

   // [TRACK]
   END_TRACKING();
}


/// Function name  : onDocumentsControl_LoadComplete
// Description     : Creates a new document and displays a tutorial
// 
// DOCUMENTS_DATA*      pWindowData    : [in] Documents data
// DOCUMENT_OPERATION*  pOperationData : [in] Data for the completed operation
// 
VOID  onDocumentsControl_LoadComplete(DOCUMENTS_DATA*  pWindowData, CONST DOCUMENT_TYPE  eType, DOCUMENT_OPERATION*  pOperationData)
{
   DOCUMENT*  pDocument;    // New document

   // [VERBOSE]
   TRACK_FUNCTION();
   VERBOSE_LIB_EVENT();

   // [CHECK] Was the operation successful?
   if (isOperationSuccessful(pOperationData))
   {
      /// [SUCCESS] Create new document from GameFile
      switch (eType)
      {
      case DT_SCRIPT:   pDocument = createDocumentByType(eType, (SCRIPT_FILE*)pOperationData->pGameFile);    break;
      case DT_LANGUAGE: pDocument = createDocumentByType(eType, (LANGUAGE_FILE*)pOperationData->pGameFile);  break;
      }

      /// Add document to control
      appendDocument(pWindowData->hTabCtrl, pDocument);

      // [RECENT] Add document to MRU
      addDocumentToRecentDocumentList(getMainWindowData(), pDocument);

      // [ACTIVATE] Should the document be activated?
      if (pOperationData->bActivateOnLoad)
         displayDocumentByIndex(pWindowData->hTabCtrl, getDocumentCount() - 1);

      // [MODIFIED] 
      /*if (eType == DT_SCRIPT AND ((SCRIPT_FILE*)pOperationData->pGameFile)->bModifiedOnLoad)
         setDocumentModifiedFlag(pDocument, TRUE);*/

      /// [TUTORIAL] Delay-Display relevant tutorial
      switch (eType)
      {
      case DT_SCRIPT:     setMainWindowTutorialTimer(getMainWindowData(), TW_SCRIPT_EDITING, TRUE);  break;
      //case DT_LANGUAGE:   TODO("Language file tutorial");   break;
      }

      /// Inform document load is complete
      sendDocumentOperationComplete(pDocument->hWnd, pOperationData);

      // Sever GameFile to prevent destruction
      pOperationData->pGameFile = NULL;
   }
   /// [FAILED] Flag a critical error
   else
   {
      // [CHECK] Ensure this isn't a "file not found" error
      if (!findErrorStackByID(pOperationData->pErrorQueue, IDS_LOAD_FILE_NOT_FOUND) AND !findErrorStackByID(pOperationData->pErrorQueue, IDS_LOAD_ALTERNATE_FILE_NOT_FOUND))
         // [ERROR] Flag this as a critical error
         setAppError(AE_LOAD_SCRIPT);

      // [CHECK] Are we batch testing?
      if (pOperationData->oAdvanced.bBatchCompilerTest)
         /// [BATCH TEST] Abort batch testing
         commandScriptValidationBatchTest(getMainWindowData(), BTC_STOP);
   }

   // [TRACK]
   END_TRACKING();
}


/// Function name  : onDocumentsControl_MiddleClick
// Description     : Checks whether the user middle clicked a tab and closes the associated document
// 
// DOCUMENTS_DATA*  pWindowData  : [in] Documents data
// CONST POINT      ptClick      : [in] Position of click, in client co-ordinates
// 
VOID  onDocumentsControl_MiddleClick(DOCUMENTS_DATA*  pWindowData, CONST POINT  ptClick)
{
   TCHITTESTINFO     oHitTest;      // Hit test properties
   INT               iTabIndex;     // Zero-based index of the tab that was clicked

   // [TRACK]
   TRACK_FUNCTION();

   // Prepare
   oHitTest.pt = ptClick;

   // Determine the location of the click
   iTabIndex = TabCtrl_HitTest(pWindowData->hTabCtrl, &oHitTest);

   // [CHECK] Has the user middle clicked on a tab?
   if (oHitTest.flags INCLUDES TCHT_ONITEM)
      /// [SUCCESS] Close (and possibly save) specified document
      closeDocumentByIndex(pWindowData->hTabCtrl, iTabIndex);

   // [TRACK]
   END_TRACKING();
}


/// Function name  : onDocumentsControl_Notify
// Description     : Tooltip notifications
// 
// DOCUMENTS_DATA*  pWindowData : [in] Window data
// CONST UINT       iControlID  : [in] ID of Control sending notification
// NMHDR*           pMessage    : [in] Header
// 
// Return Value   : TRUE if handled, otherwise FALSE
// 
BOOL  onDocumentsControl_Notify(DOCUMENTS_DATA*  pWindowData, CONST UINT  iControlID, NMHDR*  pMessage)
{
   BOOL  bResult;

   // Prepare
   bResult = FALSE;

   // [TOOLTIP]
   if (pMessage->code == TTN_GETDISPINFO)
   {
      onDocumentsControl_RequestTooltip(pWindowData, (NMTTDISPINFO*)pMessage);
      bResult = TRUE;
   }

   return bResult;
}

/// Function name  : onDocumentsControl_OperationComplete
// Description     : Creates or destroys the document object after a load or save operation
// 
// DOCUMENTS_DATA*      pWindowData    : [in] Documents data
// DOCUMENT_OPERATION*  pOperationData : [in] Data for the completed operation
// 
VOID  onDocumentsControl_OperationComplete(DOCUMENTS_DATA*  pWindowData, DOCUMENT_OPERATION*  pOperationData)
{
   // [TRACK]
   TRACK_FUNCTION();

   // Examine operation
   switch (pOperationData->eType)
   {
   /// [LOAD DOCUMENT] Pass to load handler
   case OT_LOAD_SCRIPT_FILE:     onDocumentsControl_LoadComplete(pWindowData, DT_SCRIPT,   pOperationData);        break;
   case OT_LOAD_LANGUAGE_FILE:   onDocumentsControl_LoadComplete(pWindowData, DT_LANGUAGE, pOperationData);        break;
   
   /// [SAVE DOCUMENT] Pass to save handler
   case OT_SAVE_SCRIPT_FILE:     onDocumentsControl_SaveComplete(pWindowData, DT_SCRIPT,   pOperationData);        break;
   case OT_SAVE_LANGUAGE_FILE:   onDocumentsControl_SaveComplete(pWindowData, DT_LANGUAGE, pOperationData);        break;

   /// [VALIDATE] Pass to validation handler
   case OT_VALIDATE_SCRIPT_FILE: onDocumentsControl_ValidationComplete(pWindowData, DT_SCRIPT, pOperationData);    break;

   /// [PROJECTS] Pass to project handler
   case OT_LOAD_PROJECT_FILE:    onProject_LoadComplete(pOperationData);      break;
   case OT_SAVE_PROJECT_FILE:    onProject_SaveComplete(pOperationData);      break;
   }

   // [TRACK]
   END_TRACKING();
}


/// Function name  : onDocumentsControl_PreferencesChanged
// Description     : Inform each document that the app preferences have changed
// 
// DOCUMENTS_DATA*  pWindowData   : [in] Window data
// 
VOID  onDocumentsControl_PreferencesChanged(DOCUMENTS_DATA*  pWindowData)
{
   DOCUMENT*  pDocument;      // Document iterator

   /// Inform each document
   for (UINT iIndex = 0; findDocumentByIndex(pWindowData->hTabCtrl, iIndex, pDocument); iIndex++)
      // Inform document window of change
      Preferences_Changed(pDocument->hWnd);
}


/// Function name  : onDocumentsControl_RequestTooltip
// Description     : Provides the full path of the document when user hovers over a tab
// 
// DOCUMENTS_DATA*  pWindowData : [in] Window data
// NMTTDISPINFO*    pHeader     : [in] Tooltip request header
//
VOID  onDocumentsControl_RequestTooltip(DOCUMENTS_DATA*  pWindowData, NMTTDISPINFO*  pHeader)
{
   static TCHAR  szTooltip[MAX_PATH];     // Static tooltip buffer

   TCHITTESTINFO  oHitTest;      // Hit-test
   DOCUMENT*      pDocument;     // Document beneath cursor
   UINT           iIndex;        // Tab/Document index

   // [TRACK]
   TRACK_FUNCTION();

   // Perform hit-test
   utilGetWindowCursorPos(pWindowData->hTabCtrl, &oHitTest.pt);
   iIndex = TabCtrl_HitTest(pWindowData->hTabCtrl, &oHitTest);

   // [CHECK] Ensure cursor is hovered over a tab
   if (findDocumentByIndex(pWindowData->hTabCtrl, iIndex, pDocument) AND getDocumentPath(pDocument))
   {
      // [SUCCESS] Return document path in static tooltip buffer
      StringCchCopy(szTooltip, MAX_PATH, getDocumentPath(pDocument));
      pHeader->lpszText = szTooltip;
   }

   // [TRACK]
   END_TRACKING();
}


/// Function name  : onDocumentsControl_SaveComplete
// Description     : Closes the document if appropriate
// 
// DOCUMENTS_DATA*      pWindowData    : [in] Documents data
// DOCUMENT_OPERATION*  pOperationData : [in] Data for the completed operation
// 
VOID  onDocumentsControl_SaveComplete(DOCUMENTS_DATA*  pWindowData, CONST DOCUMENT_TYPE  eType, DOCUMENT_OPERATION*  pOperationData)
{
   DOCUMENT*  pDocument;      // Target document
   INT        iIndex;         // Document index

   // [VERBOSE]
   TRACK_FUNCTION();
   VERBOSE_LIB_EVENT();

   // Prepare
   pDocument = (DOCUMENT*)pOperationData->pDocument;

   /// Inform document save is complete
   sendDocumentOperationComplete(pDocument->hWnd, pOperationData);

   // [CHECK] Should project close after a successful save?
   if (pDocument->bClosing AND isOperationSuccessful(pOperationData))
   {
      /// [DESTROY] Destroy document
      findDocumentIndexByValue(pWindowData, pDocument, iIndex);
      removeDocument(pWindowData, pDocument, iIndex);

      // [CHECK] Is the app closing? Are all documents closed?
      if (isAppClosing() AND !getDocumentCount())
         /// [EVENT] Fire UM_MAIN_WINDOW_CLOSING with state MWS_DOCUMENTS_CLOSED
         postAppClose(MWS_DOCUMENTS_CLOSED);    
   }
   // [CHECK] Is app closing?  Did save fail?
   else if (isAppClosing() AND !isOperationSuccessful(pOperationData))
      /// [FAILED] Abort the app closing sequence
      setAppClosing(FALSE);

   // Sever GameFile to prevent destruction
   pOperationData->pGameFile = NULL;

   // [TRACK]
   END_TRACKING();
}


/// Function name  : onDocumentsControl_ValidationComplete
// Description     : Moves onto the next batch file, or displays the one that failed
// 
// DOCUMENTS_DATA*      pWindowData    : [in] Documents data
// DOCUMENT_OPERATION*  pOperationData : [in] Data for the completed operation
// 
VOID  onDocumentsControl_ValidationComplete(DOCUMENTS_DATA*  pWindowData, CONST DOCUMENT_TYPE  eType, DOCUMENT_OPERATION*  pOperationData)
{
   SCRIPT_FILE*  pScriptFile;      // Convenience pointer
   DOCUMENT*     pDocument;        // Document created on failure
   
   // [VERBOSE]
   TRACK_FUNCTION();
   VERBOSE_LIB_EVENT();

   // [CHECK] Was the operation succeed?
   if (isOperationSuccessful(pOperationData))
      /// [SUCCESS] Pass to document handler - document does not actually exist
      onScriptDocumentValidationComplete(NULL, pOperationData);
   
   /// [FAILED] Display document
   else
   {
      // Prepare
      pScriptFile = (SCRIPT_FILE*)pOperationData->pGameFile;

      // [CHECK] Are we batch testing?
      if (pOperationData->oAdvanced.bBatchCompilerTest)
         /// [BATCH TESTING] Abort batch testing
         commandScriptValidationBatchTest(getMainWindowData(), BTC_STOP);

      // [CHECK] Do nothing if script was not translated
      if (pScriptFile->szScriptname)
      {
         // Create document
         switch (eType)
         {
         case DT_SCRIPT:   pDocument = createDocumentByType(eType, (SCRIPT_FILE*)pOperationData->pGameFile);    break;
         case DT_LANGUAGE: pDocument = createDocumentByType(eType, (LANGUAGE_FILE*)pOperationData->pGameFile);  break;
         }

         /// Add document to control
         appendDocument(pWindowData->hTabCtrl, pDocument);

         // [ACTIVATE] 
         displayDocumentByIndex(pWindowData->hTabCtrl, getDocumentCount() - 1);

         // Inform document validation is complete
         sendDocumentOperationComplete(pDocument->hWnd, pOperationData);

         // Sever GameFile to prevent destruction
         pOperationData->pGameFile = NULL;
      }
   }

   // [TRACK]
   END_TRACKING();
}


//VOID  onDocumentsControl_MouseMove(DOCUMENTS_DATA*  pWindowData, CONST POINT* ptClick)
//{
//}
//
//VOID  onDocumentsControl_LeftClickDown(DOCUMENTS_DATA*  pWindowData, CONST POINT* ptClick)
//{
//   TCHITTESTINFO  oHitTest;      // Hit-test
//   DOCUMENT*      pDocument;     // Document beneath cursor
//   //UINT           iIndex;        // Tab/Document index
//
//   // Perform hit-test
//   utilGetWindowCursorPos(pWindowData->hTabCtrl, &oHitTest.pt);
//   pWindowData->iOldIndex = TabCtrl_HitTest(pWindowData->hTabCtrl, &oHitTest);
//
//   // [CHECK] Ensure cursor is hovered over a tab
//   if (findDocumentByIndex(pWindowData->hTabCtrl, pWindowData->iOldIndex, pDocument))
//      SetCapture(pWindowData->hTabCtrl);
//}
//
//VOID  onDocumentsControl_LeftClickUp(DOCUMENTS_DATA*  pWindowData, CONST POINT* ptClick)
//{
//   TCHITTESTINFO  oHitTest;      // Hit-test
//   DOCUMENT*      pDocument;     // Document beneath cursor
//   UINT           iIndex;        // Tab/Document index
//
//   // [CHECK]
//   if (GetCapture() == pWindowData->hTabCtrl)
//   {
//      // Perform hit-test
//      utilGetWindowCursorPos(pWindowData->hTabCtrl, &oHitTest.pt);
//      iIndex = TabCtrl_HitTest(pWindowData->hTabCtrl, &oHitTest);
//
//      // [CHECK] Ensure cursor is hovered over a tab
//      if (findDocumentByIndex(pWindowData->hTabCtrl, iIndex, pDocument))
//      {
//      }
//   }
//}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : wndprocDocumentsCtrl
// Description     : Documents control window procedure
//  
LRESULT  wndprocDocumentsCtrl(HWND  hCtrl, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   DOCUMENTS_DATA*  pWindowData;        // Window data
   ERROR_STACK*     pException;         // Exception error
   DOCUMENT*        pActiveDocument;    // Active document
   WNDCLASS         oBaseClass;         // Window class of base Tab control
   POINT            ptClick;            // Click location
   BOOL             bPassToBase;        // Whether to pass message to base tab control
#ifdef BULLWINKLE
   RECT             rcClient;
#endif

   // Prepare
   TRACK_FUNCTION();
   bPassToBase = TRUE;

   /// [GUARD BLOCK]
   __try
   {
      // Prepare
      pWindowData = getDocumentsControlData(hCtrl);

      // Examine message
      switch (iMessage)
      {
      /// [CREATE] Create window data
      case WM_CREATE:
         // Create window data
         pWindowData = createDocumentsControlData(hCtrl);
         onDocumentsControl_Create(pWindowData);
         break;

      /// [DESTROY] Destroy window data
      case WM_DESTROY:
         onDocumentsControl_Destroy(pWindowData);
         break;

      /// [NOTIFICATION]
      case WM_NOTIFY:
         bPassToBase = !onDocumentsControl_Notify(pWindowData, wParam, (NMHDR*)lParam);
         break;

      // [DOCUMENT UPDATED]
      case UN_DOCUMENT_UPDATED:
         onDocumentsControl_DocumentUpdated(pWindowData);
         // Don't pass to base
         bPassToBase = FALSE;
         break;

      // [DOCUMENT SWITCHED]
      case UN_DOCUMENT_SWITCHED:
         onDocumentsControl_DocumentSwitched(pWindowData, (DOCUMENT*)lParam);
         // Don't pass to base
         bPassToBase = FALSE;
         break;

      // [DOCUMENT PROPERTY UPDATED]
      case UN_PROPERTY_UPDATED:
         onDocumentsControl_DocumentPropertyUpdated(pWindowData, wParam);
         // Don't pass to base
         bPassToBase = FALSE;
         break;

      // [OPERATION COMPLETE]
      case UN_OPERATION_COMPLETE:
         onDocumentsControl_OperationComplete(pWindowData, (DOCUMENT_OPERATION*)lParam);
         // Don't pass to base
         bPassToBase = FALSE;
         break;

      // [PREFERENCES CHANGED]
      case UN_PREFERENCES_CHANGED:
         onDocumentsControl_PreferencesChanged(pWindowData);
         // Don't pass to base
         bPassToBase = FALSE;
         break;

      /// [CONTEXT MENU] Display tab context menu
      case WM_CONTEXTMENU:
         // Prepare
         ptClick.x = GET_X_LPARAM(lParam);
         ptClick.y = GET_Y_LPARAM(lParam);
         // Display context menu
         bPassToBase = !onDocumentsControl_ContextMenu(pWindowData, &ptClick);
         break;

      /// [MENU ITEM HOVER] Forward messages up the chain to the Main window
      case WM_MENUSELECT:
         sendAppMessage(AW_MAIN, WM_MENUSELECT, wParam, lParam);
         break;

      /// [MIDDLE CLICK] Close the associated document if user middle-clicks a document tab
      case WM_MBUTTONUP:
         // Prepare
         ptClick.x = GET_X_LPARAM(lParam);
         ptClick.y = GET_Y_LPARAM(lParam);
         // Close document if user clicked tab
         onDocumentsControl_MiddleClick(pWindowData, ptClick);
         break;

      /*case WM_MOUSEMOVE:
         SetCursor(LoadCursor(NULL, IDC_ARROW));
         break;*/

      /*/// [MOUSE MOVE] Set appropriate sizing cursor
      case WM_MOUSEMOVE:
         ptClick.x = GET_X_LPARAM(lParam);
         ptClick.y = GET_Y_LPARAM(lParam);
         onDocumentsControl_MouseMove(pWindowData, &ptClick);
         break;

      /// [MOUSE DOWN] Set mouse capture
      case WM_LBUTTONDOWN:
         ptClick.x = GET_X_LPARAM(lParam);
         ptClick.y = GET_Y_LPARAM(lParam);
         onDocumentsControl_LeftClickDown(pWindowData, &ptClick);
         break;

      /// [MOUSE UP] Release mouse capture
      case WM_LBUTTONUP:
         ptClick.x = GET_X_LPARAM(lParam);
         ptClick.y = GET_Y_LPARAM(lParam);
         onDocumentsControl_LeftClickUp(pWindowData, &ptClick);
         break;*/

      /// [RE-SIZE] Resize active document
      case WM_SIZE:
         // [CHECK] Is there an active document?
         if (pActiveDocument = getActiveDocument())
         {
            // [SUCCESS] Resize control then active document
            CallWindowProc(pWindowData->pfnBaseWindowProc, hCtrl, iMessage, wParam, lParam);
            updateDocumentSize(pWindowData, pActiveDocument);
            // Don't pass to base
            bPassToBase = FALSE;
         }
         break;

      /// [CUSTOM MENU]
      case WM_DRAWITEM:    bPassToBase = !onWindow_DrawItem((DRAWITEMSTRUCT*)lParam);                   break;
      case WM_MEASUREITEM: bPassToBase = !onWindow_MeasureItem(hCtrl, (MEASUREITEMSTRUCT*)lParam);      break;

#ifdef BULLWINKLE
      /// [ERASE BACKGROUND]
      case WM_ERASEBKGND:
         GetClientRect(hCtrl, &rcClient);
         bPassToBase = !FillRect((HDC)wParam, &rcClient, GetSysColorBrush(COLOR_3DFACE));
         break;
#endif
      }
   }
   /// [EXCEPTION HANDLER]
   __except (generateExceptionError(GetExceptionInformation(), pException))
   {
      // [ERROR] "An unidentified and unexpected critical error has occurred in the documents container window"
      enhanceError(pException, ERROR_ID(IDS_EXCEPTION_DOCUMENTS_WINDOW));
      displayException(pException);
   }

   // [CHECK] Pass to base? 
   if (bPassToBase)
      GetClassInfo(NULL, WC_TABCONTROL, &oBaseClass);
    
   /// Return TRUE if handled here, otherwise pass to base
   END_TRACKING();
   return (bPassToBase ? CallWindowProc(oBaseClass.lpfnWndProc, hCtrl, iMessage, wParam, lParam) : 0);
}

