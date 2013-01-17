//
// Document Property Sheet UI.cpp : Implementation of the UI actions of the document properties dialog
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"


/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          MESSAGE HANDLERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onGeneralPageCommandScript
// Description     : WM_COMMAND processing for the 'General' script properties page
// 
// SCRIPT_DOCUMENT* pDocument      : [in] ScriptDocument 
// HWND             hDialog        : [in] 'General' page window handle
// CONST UINT       iControl       : [in] ID of the control sending the command
// CONST UINT       iNotification  : [in] Notification being sent
// HWND             hCtrl          : [in] Window handle fo the control sending the command
// 
// Return Value   : TRUE if processed, FALSE otherwise
// 
BOOL   onGeneralPageCommandScript(SCRIPT_DOCUMENT*  pDocument, HWND  hPage, CONST UINT  iControlID, CONST UINT  iNotification, HWND  hCtrl)
{
   GAME_STRING*  pCommandLookup;
   TCHAR*        szCommandID;
   BOOL          bResult;

   // Prepare
   bResult = FALSE;

   // Examine control ID
   switch (iControlID)
   {
   /// [SCRIPT PROPERTY] 
   case IDC_SCRIPT_NAME:
   case IDC_SCRIPT_DESCRIPTION:
   case IDC_SCRIPT_COMMAND:
   case IDC_SCRIPT_VERSION:
   case IDC_SCRIPT_ENGINE_VERSION:
   case IDC_SCRIPT_FOLDER:
      // Examine notification
      switch (iNotification)
      {
      /// [VALUE CHANGED] Update appropriate property
      case EN_CHANGE:
      case CBN_SELCHANGE:
         // [CHECK] Ensure notification was caused by user input
         if (hCtrl == GetFocus())
         {
            onGeneralPagePropertyChanged(pDocument, hPage, iControlID);
            bResult = TRUE;
         }
         break;

      /// [LOST FOCUS] Ensure script name isn't empty
      case EN_KILLFOCUS:
         // [CHECK] Change ScriptName to 'Untitiled' if empty
         if (iControlID == IDC_SCRIPT_NAME AND !utilGetDlgItemTextLength(hPage, IDC_SCRIPT_NAME))
         {
            SetDlgItemText(hPage, IDC_SCRIPT_NAME, TEXT("Untitled"));
            onGeneralPagePropertyChanged(pDocument, hPage, IDC_SCRIPT_NAME);
         }
         // [CHECK] Resolve CommandID if numeric
         else if (iControlID == IDC_SCRIPT_COMMAND AND utilGetDlgItemTextLength(hPage, IDC_SCRIPT_COMMAND))
         {
            // [CHECK] Has user entered a recognised, numeric command ID?
            if (isStringNumeric(szCommandID = utilGetDlgItemText(hPage, IDC_SCRIPT_COMMAND)) AND findGameStringByID(utilConvertStringToInteger(szCommandID), GPI_OBJECT_COMMANDS, pCommandLookup))
               // [SUCCESS] Resolve ID
               SetDlgItemText(hPage, IDC_SCRIPT_COMMAND, pCommandLookup->szText);

            // Cleanup
            utilDeleteString(szCommandID);
         }
         break;
      }
      break;
   }

   // Return result
   return bResult;
}


/// Function name  : onGeneralPageDrawItem
// Description     : Draws the script signature icon
// 
// DRAWITEMSTRUCT*  pDrawInfo : [in] Draw info
// 
// Return Value   : TRUE
// 
BOOL  onGeneralPageDrawItem(DRAWITEMSTRUCT*  pDrawInfo)
{
   RECT  rcIcon;

   // [CHECK] Ensure item is the signature icon
   if (pDrawInfo->CtlID != IDC_SCRIPT_SIGNATURE)
      return FALSE;

   // Prepare
   GetClientRect(pDrawInfo->hwndItem, &rcIcon);

   // Erase icon
   FillRect(pDrawInfo->hDC, &rcIcon, getDialogBackgroundBrush());
   
   /// [ICON] Draw in colour if enabled, greyed if disabled
   drawImageTreeIcon(ITS_MEDIUM, TEXT("SIGNED_ICON"), pDrawInfo->hDC, rcIcon.left, rcIcon.top, IsWindowEnabled(pDrawInfo->hwndItem) ? IS_NORMAL : IS_DISABLED);
                    
   // [SUCCESS] Return TRUE
   return TRUE;
}


/// Function name  : onGeneralPagePropertyChanged
// Description     : Saves the new values to the ScriptFile and notifies the document
// 
// SCRIPT_DOCUMENT*  pDocument  : [in] Script Document
// HWND              hDialog    : [in] Window handle of the Properties page
// CONST UINT        iControlID : [in] ID of the control whoose value has changed
// 
VOID   onGeneralPagePropertyChanged(SCRIPT_DOCUMENT*  pDocument, HWND  hPage, CONST UINT  iControlID)
{
   SCRIPT_FILE*  pScriptFile;    // Convenience pointer
   TCHAR*        szNewValue;     // New property value
   
   // Prepare
   pScriptFile = pDocument->pScriptFile;

   // Get new property value
   szNewValue = utilGetWindowText(GetDlgItem(hPage, iControlID));

   // Determine which property has changed
   switch (iControlID)
   {
   /// [SCRIPT NAME] Update ScriptFile's name and update path to match
   case IDC_SCRIPT_NAME:
      setScriptName(pScriptFile, szNewValue);
      setPathFromScriptName(pScriptFile);
      break;

   /// [DESCRIPTION] Update ScriptFile's Description
   case IDC_SCRIPT_DESCRIPTION:
      utilReplaceString(pScriptFile->szDescription, szNewValue);
      break;

   /// [COMMAND] Update ScriptFile's CommandID
   case IDC_SCRIPT_COMMAND:
      utilReplaceString(pScriptFile->szCommandID, szNewValue);
      break;

   /// [VERSION] Update ScriptFile's version
   case IDC_SCRIPT_VERSION:
      pScriptFile->iVersion = (lstrlen(szNewValue) > 0 ? utilConvertStringToInteger(szNewValue) : 0);
      break;

   /// [ENGINE-VERSION] Update ScriptFile's EngineVersion
   case IDC_SCRIPT_ENGINE_VERSION:
      pScriptFile->eGameVersion = (GAME_VERSION)SendDlgItemMessage(hPage, IDC_SCRIPT_ENGINE_VERSION, CB_GETCURSEL, NULL, NULL);
      break;

   // [PATH] Can't be changed in this manner
   //case IDC_SCRIPT_FOLDER:
   }

   // [EVENT] Notify DocumentsCtrl that a property has changed
   sendDocumentPropertyUpdated(AW_DOCUMENTS_CTRL, iControlID);

   // Cleanup
   utilDeleteString(szNewValue);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       DIALOG PROCEDURES
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : dlgprocGeneralPageScript
// Description     : Window procedure for the 'General' properties dialog page
//
// 
INT_PTR   dlgprocGeneralPageScript(HWND  hPage, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   PROPERTIES_DATA*  pSheetData;    // Property sheet dialog data

   // Get properties dialog data
   pSheetData = getPropertiesDialogData(hPage);

   // Examine message
   switch (iMessage)
   {
   /// [COMMAND]
   case WM_COMMAND:
      return onGeneralPageCommandScript(pSheetData->pScriptDocument, hPage, LOWORD(wParam), HIWORD(wParam), (HWND)lParam);

   /// [CUSTOM COMBOBOX] Override default size
   case WM_MEASUREITEM:
      return onWindow_MeasureComboBox((MEASUREITEMSTRUCT*)lParam, ITS_SMALL, ITS_MEDIUM);

   /// [SIGNATURE] Draw icon
   case WM_DRAWITEM:
      if (onGeneralPageDrawItem((DRAWITEMSTRUCT*)lParam))
         return TRUE;
      break;
   }

   /// Pass to base
   return dlgprocPropertiesPage(hPage, iMessage, wParam, lParam, PP_SCRIPT_GENERAL);
}

