//
// Insert Argument Dialog.cpp : The 'new argument' dialog for the 'Arguments' document properties
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        DECLARATIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

struct FIND_TEXT_DIALOG
{
   HWND       hDialog;
   DOCUMENT*  pDocument;
};


// Functions
VOID     initFindTextDialog(FIND_TEXT_DIALOG*  pDialogData, HWND  hDialog);

// Message Handlers
BOOL     onFindTextDialogCommand(FIND_TEXT_DIALOG*  pDialogData, CONST UINT  iControlID, CONST UINT  iNotification, HWND  hCtrl);
VOID     onFindTextDialogFindText(FIND_TEXT_DIALOG*  pDialogData, CONST CODE_EDIT_SEARCH_FLAG  eAction);

// Dialog procedure
INT_PTR  dlgprocFindTextDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : displayFindTextDialog
// Description     : Displays the FindText dialog
// 
// HWND  hParentWnd : [in] Parent window
// 
// Return Value   : Window handle if succesful, otherwise NULL
// 
HWND   displayFindTextDialog(HWND  hParentWnd)
{
   FIND_TEXT_DIALOG*  pDialogData = utilCreateObject(FIND_TEXT_DIALOG);

   // [TRACK]
   CONSOLE_ACTION1(getActiveDocumentFileName());

   // Set dialog data
   pDialogData->pDocument = getActiveScriptDocument();

   /// Create FindText dialog
   HWND  hDialog = loadDialog(TEXT("FIND_TEXT_DIALOG"), hParentWnd, dlgprocFindTextDialog, (LPARAM)pDialogData);
   ERROR_CHECK("creating 'FindText' dialog", hDialog);

   // [CHECK] Ensure dialog was created succesfully
   if (hDialog)
   {
      // [SUCCESS] Display dialog
      ShowWindow(hDialog, SW_SHOW);
      UpdateWindow(hDialog);
   }

   // Return window handle
   return hDialog;
}


/// Function name  : initFindTextDialog
// Description     : 
// 
// HWND  hDialog : [in] 
// 
VOID   initFindTextDialog(FIND_TEXT_DIALOG*  pDialogData, HWND  hDialog)
{
   TCHAR    *szNewLine,
            *szSelection;    // Currently selected text

   // Prepare
   pDialogData->hDialog = hDialog;
   SetWindowLong(hDialog, DWL_USER, (LONG)pDialogData);

   // [CAPTION] 
   SetWindowText(pDialogData->hDialog, getAppName());

   // [POSITION] Set last position
   SetWindowPos(pDialogData->hDialog, NULL, getAppPreferencesWindowRect(AW_FIND)->left, getAppPreferencesWindowRect(AW_FIND)->top, NULL, NULL, SWP_NOSIZE WITH SWP_NOZORDER);

   // [CHECK] Is there a current text selection?
   if (szSelection = CodeEdit_GetSelectionText(getActiveScriptCodeEdit()))
   {
      // [CHECK] Truncate at first line break
      if (szNewLine = utilFindCharacterInSet(szSelection, "\r\n"))
         szNewLine[0] = NULL;

      // [SELECTION] Display text currently selected
      SetDlgItemText(pDialogData->hDialog, IDC_FIND_SEARCH_EDIT, szSelection);
      utilDeleteString(szSelection);
   }
   else
      // [NO SELECTION] Display last search text
      SetDlgItemText(pDialogData->hDialog, IDC_FIND_SEARCH_EDIT, getAppPreferences()->szFindText);

   // Prevent EditCtrl from initially selecting text
   SubclassWindow(GetControl(pDialogData->hDialog, IDC_FIND_SEARCH_EDIT), wndprocCustomEditControl);
   
   // [OWNER DRAW]
   utilAddWindowStyle(GetControl(pDialogData->hDialog, IDC_DIALOG_ICON), SS_OWNERDRAW);
   utilAddWindowStyle(GetControl(pDialogData->hDialog, IDC_DIALOG_TITLE), SS_OWNERDRAW);
}


/// Function name  : generateDocumentList
// Description     : Generate list of ScriptDocuments, containing active document first
// 
// DOCUMENT*  pActiveDoc   : [in] Active document
// 
// Return Value   : New list of documents
// 
LIST*  generateDocumentList(DOCUMENT*  pActiveDoc)
{
   HWND   hDocumentsTab = getMainWindowData()->hDocumentsTab;
   LIST*  pOutput       = createList(NULL);
   INT    iInitialIndex;

   // Get Index of current document
   findDocumentIndexByPath(hDocumentsTab, pActiveDoc->szFullPath, iInitialIndex);

   // ITerate through open documents
   for (UINT  i = 0; i < getDocumentCount(); i++)
   {
      UINT       iLogicalIndex = (iInitialIndex + i) % getDocumentCount();
      DOCUMENT*  pDocument;

      // [SCRIPT DOCUMENT] Add to list
      if (findDocumentByIndex(hDocumentsTab, iLogicalIndex, pDocument) && pDocument->eType == DT_SCRIPT)
         appendObjectToList(pOutput, (LPARAM)pDocument);
   }

   // Return document list
   return pOutput;
}

/// Function name  : findNextScriptDocumentMatch
// Description     : Find next match in other script documents, set document to active if found
// 
// FIND_TEXT_DIALOG*  pDialogData      : [in] Dialog data
// SCRIPT_DOCUMENT*   pActiveDocument  : [in] Active document
// CODE_EDIT_SEARCH*  pSearchData      : [in] Search data
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BOOL  findNextScriptDocumentMatch(FIND_TEXT_DIALOG*  pDialogData, SCRIPT_DOCUMENT*   pActiveDocument, CODE_EDIT_SEARCH*  pSearchData)
{
   SCRIPT_DOCUMENT*  pDocument;
   LIST*             pScripts = generateDocumentList(pActiveDocument);
   BOOL              bFound = FALSE;

   // Always search other documents 'from top'
   pSearchData->eFlags ^= CSF_FROM_CARET;
   pSearchData->eFlags |= CSF_FROM_TOP;

   // Search each script document
   for (UINT  iIndex = 1; findListObjectByIndex(pScripts, iIndex, (LPARAM&)pDocument); iIndex++)
   {
      /// [SEARCH] Find/replace and select next match
      if (bFound = CodeEdit_FindText(pDocument->hCodeEdit, pSearchData))
      {
         // [FOUND] Display document
         displayDocument(getMainWindowData()->hDocumentsTab, pDocument);
         break;
      }
   }

   // Cleanup and return result
   deleteList(pScripts);
   return bFound;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onFindTextDialogFindText
// Description     : Invokes the search/replace operation using the current settings
// 
// HWND                         hDialog : [in] Dialog
// CONST CODE_EDIT_SEARCH_FLAG  eAction : [in] Whether to search or replace
// 
VOID  onFindTextDialogFindText(FIND_TEXT_DIALOG*  pDialogData, CONST CODE_EDIT_SEARCH_FLAG  eAction)
{
   CODE_EDIT_SEARCH       oSearchData;
   SCRIPT_DOCUMENT*       pDocument;
   BOOL                   bMatchFound;

   // [CHECK] Ensure document and search phrase exist
   if ((pDocument = getActiveScriptDocument()) AND utilGetDlgItemTextLength(pDialogData->hDialog, IDC_FIND_SEARCH_EDIT))
   {
      // Prepare
      oSearchData.szSearch  = utilGetDlgItemText(pDialogData->hDialog, IDC_FIND_SEARCH_EDIT);
      oSearchData.szReplace = utilGetDlgItemText(pDialogData->hDialog, IDC_FIND_REPLACE_EDIT);

      // [CHECK] Set 'Find from Top' and 'Case sensitive' options
      oSearchData.eFlags  = eAction WITH (IsDlgButtonChecked(pDialogData->hDialog, IDC_FIND_FROM_TOP_CHECK) ? CSF_FROM_TOP : CSF_FROM_CARET);
      oSearchData.eFlags |= (IsDlgButtonChecked(pDialogData->hDialog, IDC_FIND_CASE_SENSITIVE_CHECK) ? CSF_CASE_SENSITIVE : NULL);

      /// [SEARCH] Find/replace and select next match
      bMatchFound = CodeEdit_FindText(pDocument->hCodeEdit, &oSearchData);

      // [ALL DOCUMENTS] Search next document
      if (!bMatchFound && IsDlgButtonChecked(pDialogData->hDialog, IDC_FIND_ALL_DOCUMENTS))
         bMatchFound = findNextScriptDocumentMatch(pDialogData, pDocument, &oSearchData);

      // Display 'no more matches' if no match was found
      utilShowDlgItem(pDialogData->hDialog, IDC_FIND_COMPLETE_STATIC, !bMatchFound);
      // Untick 'Search from top' if a match was found
      CheckDlgButton(pDialogData->hDialog, IDC_FIND_FROM_TOP_CHECK, !bMatchFound);

      // Cleanup
      utilDeleteStrings(oSearchData.szSearch, oSearchData.szReplace);
   }
}

/// Function name  : onFindTextDialogCommand
// Description     : WM_COMMAND processing 
// 
// HWND          hDialog        : [in] Window handle 
// CONST UINT    iControlID     : [in] ID of the control sending the command
// CONST UINT    iNotification  : [in] Notification code
// HWND          hCtrl          : [in] Window handle of the control sending the command
// 
// Return Value   : TRUE if processed, FALSE otherwise
// 
BOOL   onFindTextDialogCommand(FIND_TEXT_DIALOG*  pDialogData, CONST UINT  iControlID, CONST UINT  iNotification, HWND  hCtrl)
{
   TCHAR*   szSearchText;
   BOOL     bResult;

   // Prepare
   bResult = TRUE;

   // Examine sender
   switch (iControlID)
   {
   /// [SEARCH CHANGED] Enable/disable 'Find Next'
   case IDC_FIND_SEARCH_EDIT:
      if (bResult = (iNotification == EN_CHANGE))
         utilEnableDlgItem(pDialogData->hDialog, IDC_FIND_NEXT, utilGetDlgItemTextLength(pDialogData->hDialog, IDC_FIND_SEARCH_EDIT) != 0);
      break;

   /// [REPLACE CHANGED] Enable/disable 'Replace' 
   case IDC_FIND_REPLACE_EDIT:
      if (bResult = (iNotification == EN_CHANGE))
         utilEnableDlgItem(pDialogData->hDialog, IDC_FIND_REPLACE, utilGetDlgItemTextLength(pDialogData->hDialog, IDC_FIND_REPLACE_EDIT) != 0);
      break;

   /// [FIND/REPLACE] Search for next match
   case IDC_FIND_NEXT:     onFindTextDialogFindText(pDialogData, CSF_SEARCH);   break;
   case IDC_FIND_REPLACE:  onFindTextDialogFindText(pDialogData, CSF_REPLACE);  break;

   /// [ALL DOCUMENTS] Ensure 'From Top' is also checked
   case IDC_FIND_ALL_DOCUMENTS:
      utilEnableDlgItem(pDialogData->hDialog, IDC_FIND_FROM_TOP_CHECK, !IsDlgButtonChecked(pDialogData->hDialog, IDC_FIND_ALL_DOCUMENTS));

      if (IsDlgButtonChecked(pDialogData->hDialog, IDC_FIND_ALL_DOCUMENTS))
         CheckDlgButton(pDialogData->hDialog, IDC_FIND_FROM_TOP_CHECK, TRUE);
      break;

   /// [CANCEL] Close dialog and zero window handle
   case IDCANCEL:
      // Save position to preferences
      GetWindowRect(pDialogData->hDialog, getAppPreferencesWindowRect(AW_FIND));

      // Save search to preferences
      setAppPreferencesLastFindText(szSearchText = utilGetDlgItemText(pDialogData->hDialog, IDC_FIND_SEARCH_EDIT));
      utilDeleteString(szSearchText);

      // Close dialog
      EndDialog(pDialogData->hDialog, iControlID);
      getMainWindowData()->hFindTextDlg = NULL;
      break;

   default:
      bResult = FALSE;
      break;
   }

   // Return result
   return bResult;
}


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : dlgprocFindTextDialog
// Description     : Window procedure for the 'Insert Argument' dialog
// 
// 
INT_PTR  dlgprocFindTextDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   FIND_TEXT_DIALOG* pDialogData = (FIND_TEXT_DIALOG*)GetWindowLong(hDialog, DWL_USER);
   BOOL              bResult     = FALSE;

   // Examine message
   switch (iMessage)
   {
   /// [CREATION] -- Setup dialog
   case WM_INITDIALOG:
      // Init dialog
      initFindTextDialog((FIND_TEXT_DIALOG*)lParam, hDialog);
      bResult = TRUE;
      break;

   /// [DESTRUCTION]
   case WM_DESTROY:
      utilDeleteObject(pDialogData);
      SetWindowLong(hDialog, DWL_USER, NULL);
      break;

   /// [COMMAND PROCESSING] -- Process name change, OK and CANCEL
   case WM_COMMAND:
      bResult = onFindTextDialogCommand(pDialogData, LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
      break;

   /// [ENABLED]
   case WM_ENABLE:
      // [CHECK] Ensure main window is in the same state
      if (IsWindowEnabled(getAppWindow()) != wParam)
         EnableWindow(getAppWindow(), wParam);

      // [CHECK] Ensure Properties dialog is in the same state
      if (getMainWindowData()->hPropertiesSheet AND IsWindowEnabled(getMainWindowData()->hPropertiesSheet) != wParam)
         EnableWindow(getMainWindowData()->hPropertiesSheet, wParam);
      break;

   /// [OWNER DRAW]
   case WM_DRAWITEM:
      if (wParam == IDC_DIALOG_ICON)
         bResult = onOwnerDrawStaticIcon(lParam, TEXT("FIND_TEXT_ICON"), 96);
      break;
   }

   // Return result
   return (bResult ? TRUE : dlgprocVistaStyleDialog(hDialog, iMessage, wParam, lParam));
}

