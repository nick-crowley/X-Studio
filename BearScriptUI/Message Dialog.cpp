//
// Message Dialog.cpp : A versatile dialog for displaying simple messages (from resource library) with appropriate
//                         icons and buttons.  It can also display stacks of error messages, and allow the user to
//                         browse through them.
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MACROS
/// /////////////////////////////////////////////////////////////////////////////////////////

// onException: Display 
#define  ON_EXCEPTION()    displayException(pException);

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                   CONSTANTS / GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// Number of Toolbar buttons
CONST UINT  iToolBarButtonCount = 4;

// Toolbar button Commands
CONST UINT  iButtonCommands[iToolBarButtonCount] = 
{ 
   IDC_MESSAGE_COPY, IDC_MESSAGE_ATTACHMENTS, IDC_MESSAGE_LESS, IDC_MESSAGE_MORE 
};

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTRUCTION / DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createMessageDialogData
// Description     : Creates message dialog data
// 
// CONST UINT          iMessageID   : [in] [MESSAGE] String ID of the message to display
// CONST UINT          iTitleID     : [in]           String ID of Dialog title
// CONST UINT          iAttributes  : [in]           MESSAGE_DIALOG_FLAGs indicating which buttons to display.  For message dialogs this should also indicate the icon to display
// CONST ERROR_STACK*  pErrorStack  : [in] [ERROR]   Error message to display
// CONST ERROR_QUEUE*  pErrorQueue  : [in] [ERROR]   Queue of error messages to display
// va_list             pArguments   : [in] [MESSAGE] Optional arguments for message
// 
// Return Value   : New error message dialog data, you are responsible for destroying it
// 
MESSAGE_DIALOG_DATA*  createMessageDialogData(CONST TCHAR*  szMessage, CONST TCHAR*  szTitle, CONST UINT  iAttributes, CONST ERROR_STACK*  pErrorStack, CONST ERROR_QUEUE*  pErrorQueue, va_list  pArguments)
{
   MESSAGE_DIALOG_DATA*  pDialogData;     // MessageDialog data

   // Create object
   pDialogData = utilCreateEmptyObject(MESSAGE_DIALOG_DATA);

   /// Set properties
   pDialogData->szTitle     = utilDuplicateSimpleString(szTitle);
   pDialogData->iAttributes = iAttributes;
   pDialogData->pErrorStack = pErrorStack;

   /// [QUEUE] Extract first error
   if (pDialogData->pErrorQueue = pErrorQueue)
   {
      // Lookup first error
      findErrorStackByIndex(pErrorQueue, 0, (ERROR_STACK*&)pDialogData->pErrorStack);
      ASSERT(pDialogData->pErrorStack);
   }

   /// [STACK/QUEUE] Extract first message
   if (pDialogData->pErrorStack)
   {
      // Lookup first message
      findErrorMessageByLogicalIndex(pDialogData->pErrorStack, 0, MT_MESSAGE, pDialogData->pErrorMessage);
      ASSERT(pDialogData->pErrorMessage);
      CONSOLE("Error %s text: '%s'", pDialogData->pErrorMessage->szID, pDialogData->pErrorMessage->szMessage);
   }
   /// [MESSAGE] Assemble message
   else if (szMessage)
   {
      // Load formatting string and assemble
      pDialogData->szSimpleMessage = utilCreateStringVf(ERROR_LENGTH, szMessage, pArguments);
      CONSOLE("Message: '%s'", pDialogData->szSimpleMessage);
   }
   
   /// [ICON] Set icon from Error, if available, otherwise from message attributes
   switch (pDialogData->pErrorStack ? pDialogData->pErrorStack->eType : extractIconFlags(iAttributes))
   {
   // [ERROR]
   case ET_ERROR:
   case MDF_ERROR:       pDialogData->szIcon = TEXT("ERROR_ICON");        break;
   // [WARNING]
   case ET_WARNING:
   case MDF_WARNING:     pDialogData->szIcon = TEXT("WARNING_ICON");      break;
   // [INFORMATION]
   case ET_INFORMATION:
   case MDF_INFORMATION: pDialogData->szIcon = TEXT("INFORMATION_ICON");  break;
   // [QUESTION]
   default:
   case MDF_QUESTION:    pDialogData->szIcon = TEXT("HELP_ICON");         break;
   }

   // Return new data
   return pDialogData;
}


/// Function name  : deleteMessageDialogData
// Description     : Delete message dialog data
// 
// MESSAGE_DIALOG_DATA*  &pDialogData   : [in] Message/Error dialog data to destroy
// 
VOID  deleteMessageDialogData(MESSAGE_DIALOG_DATA*  &pDialogData)
{
   // Delete strings
   utilSafeDeleteStrings(pDialogData->szSimpleMessage, pDialogData->szTitle);

   // Delete calling object
   utilDeleteObject(pDialogData);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : calculateMessageDialogToolbarPosition
// Description     : Calculate the position of the toolbar
// 
// MESSAGE_DIALOG_DATA*  pDialogData  : [in]  MessageDialog data
// HWND                  hDialog      : [in]  MessageDialog window handle
// RECT*                 pToolbarRect : [out] Toolbar rectangle in dialog client co-ordinates
// 
VOID  calculateMessageDialogToolbarPosition(MESSAGE_DIALOG_DATA*  pDialogData, HWND  hDialog, RECT*  pToolbarRect)
{
   RECT    rcClient;
   SIZE    siButtonSize;         // Convenience Toolbar button dimensions
   UINT    iButtonDimensions,    // Packed toolbar button dimensions
           iGapSize;

   // Prepare
   GetClientRect(hDialog, &rcClient);

   // Determine button size
   iButtonDimensions = SendMessage(pDialogData->hToolbar, TB_GETBUTTONSIZE, NULL, NULL);
   siButtonSize.cx = LOWORD(iButtonDimensions);
   siButtonSize.cy = HIWORD(iButtonDimensions);

   // Position in bottom left corner
   iGapSize = (siButtonSize.cy / 2);
   utilSetRectangle(pToolbarRect, iGapSize, rcClient.bottom - siButtonSize.cy - iGapSize, (iToolBarButtonCount * siButtonSize.cx), siButtonSize.cy);
}



/// Function name  : convertSystemFlagsToMessageDialogFlags
// Description     : Converts MessageBox flags to message dialog flags
// 
// CONST UINT  iSystemFlags   : [in] MessageBox flags
// 
// Return Value   : Message dialog flags
// 
UINT   convertSystemFlagsToMessageDialogFlags(CONST UINT  iSystemFlags)
{
   UINT  iOutput;

   // Prepare
   iOutput = NULL;

   // [BUTTONS]
   switch (extractButtonFlags(iSystemFlags))
   {
   case MB_OK:          iOutput = MDF_OK;           break;
   case MB_YESNO:       iOutput = MDF_YESNO;        break;
   case MB_OKCANCEL:    iOutput = MDF_OKCANCEL;     break;
   case MB_YESNOCANCEL: iOutput = MDF_YESNOCANCEL;  break;
   }

   // [ICONS]
   switch (extractIconFlags(iSystemFlags))
   {
   case MB_ICONERROR:       iOutput = MDF_ERROR;        break;
   case MB_ICONWARNING:     iOutput = MDF_WARNING;      break;
   case MB_ICONQUESTION:    iOutput = MDF_QUESTION;     break;
   case MB_ICONINFORMATION: iOutput = MDF_INFORMATION;  break;
   }

   // Return flags
   return iOutput;
}


/// Function name  : getMessageDialogData
// Description     : Retrieves the Message Dialog window data
// 
// HWND   hDialog  : [in] MessageDialog window handle
// 
// Return Value   : Messagedialog window data
// 
MESSAGE_DIALOG_DATA*  getMessageDialogData(HWND  hDialog)
{
   return (MESSAGE_DIALOG_DATA*)GetWindowLong(hDialog, DWL_USER);
}


/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : displayErrorMessageDialog
// Description     : Display an 'Error Message' dialog displaying the contents of an error stack of messages + attachments
///                                              NB: This function CAN be used when resources are not loaded
// 
// HWND                hParentWnd  : [in] Parent window for the dialog
// CONST ERROR_STACK*  pError      : [in] Error containing the messages/attachments to display
// CONST TCHAR*        szTitle     : [in] Dialog title, or Resource ID of dialog title
// CONST UINT          iButtons    : [in] MESSAGE_DIALOG_FLAGs specifying the desired buttons only
// 
// Return Value   : ID of the button the user pressed: IDOK, IDCANCEL, IDYES or IDNO
// 
UINT  displayErrorMessageDialog(HWND  hParentWnd, CONST ERROR_STACK*  pError, CONST TCHAR*  szTitle, CONST UINT  iButtons)
{
   MESSAGE_DIALOG_DATA*   pDialogData;
   TCHAR*                 szBasicMessage;
   UINT                   iResult;

   CONSOLE_COMMAND();

   // [CHECK] If no parent is provided, default to the window at the top of the Z-order
   if (!hParentWnd)
      hParentWnd = GetTopWindow(getAppWindow());

   /// [RESOURCES LOADED] Display error dialog
   if (getResourceInstance())
   {
      // Create error dialog data
      pDialogData = createMessageDialogData(NULL, szTitle, extractButtonFlags(iButtons), pError, NULL, NULL);

      // Display dialog and return result
      iResult = showDialog(TEXT("MESSAGE_DIALOG"), hParentWnd, dlgprocMessageDialog, (LPARAM)pDialogData);
   }
   /// [FAILURE] Display text without using any components from the resource library
   else
   {
      // Flatten error into a simple string and display system message box
      iResult = MessageBox(hParentWnd, szBasicMessage = generateFlattenedErrorStackText(pError), getAppName(), MB_OK WITH MB_ICONERROR);

      // Cleanup
      utilDeleteString(szBasicMessage);
   }

   // Return result
   return iResult;
}


/// Function name  : displayErrorQueueDialog
// Description     : Display an error message dialog for each error within an error queue. The icon is determined by each error. No button options are available - OK is always displayed.
///                                              NB: Cannot be used when resources are not loaded
// 
// HWND                hParentWnd  : [in] Parent window for the dialog
// CONST ERROR_QUEUE*  pErrorQueue : [in] Queue of errors to display
// CONST TCHAR*        szTitle     : [in] Dialog title, or Resource ID of dialog title
// 
VOID  displayErrorQueueDialog(HWND  hParentWnd, CONST ERROR_QUEUE*  pErrorQueue, CONST TCHAR*  szTitle)
{
   MESSAGE_DIALOG_DATA*   pDialogData;

   CONSOLE_COMMAND();

   // [CHECK] Cannot be used when resources are not loaded
   ASSERT(getResourceInstance());

   // [CHECK] If no parent is provided, default to the window at the top of the Z-order
   if (!hParentWnd)
      hParentWnd = GetTopWindow(getAppWindow());

   // Create ErrorQueue dialog data
   pDialogData = createMessageDialogData(NULL, szTitle, MDF_OK, NULL, pErrorQueue, NULL);

   // Display dialog and ignore result
   showDialog(TEXT("MESSAGE_DIALOG"), hParentWnd, dlgprocMessageDialog, (LPARAM)pDialogData);
}


/// Function name  : displayMessageDialogf
// Description     : Display a message dialog containing a formatted string
///                                              NB: Cannot be used when resources are not loaded  
// 
// HWND          hParentWnd  : [in] Parent window
// CONST UINT    iMessageID  : [in] ID of string containing title followed by message
// CONST UINT    iAttributes : [in] Combination of MESSAGE_DIALOG_FLAG values specifying the icon and button arrangement
// ...             ...       : [in]   ... 
// 
// Return Value   : ID of the button clicked: IDOK, IDCANCEL, IDYES or IDNO
//  
UINT  displayMessageDialogf(HWND  hParentWnd, CONST UINT  iMessageID, CONST UINT  iAttributes, ...)
{
   MESSAGE_DIALOG_DATA*  pDialogData;     // Messagedialog data
   const TCHAR*          szTitle = loadTempString(iMessageID);

   // [TRACK]
   CONSOLE_COMMAND();

   // [CHECK] Cannot be used when resources are not loaded
   ASSERT(getResourceInstance());

   /// Create message dialog data
   pDialogData = createMessageDialogData(findNextPackedString(szTitle), szTitle, iAttributes, NULL, NULL, utilGetFirstVariableArgument(&iAttributes));

   /// Display dialog and return result
   return (UINT)showDialog(TEXT("MESSAGE_DIALOG"), utilEither(hParentWnd, GetTopWindow(getAppWindow())), dlgprocMessageDialog, (LPARAM)pDialogData);
}


/// Function name  : displayMessageDialogButton
// Description     : Sets the text, ID, visibility and enabled state of a button
// 
// MESSAGE_DIALOG_DATA*  pDialogData : [in] Dialog data
// CONST UINT            iControlID  : [in] Button control ID
// CONST UINT            iNewID      : [in] New control ID
// CONST TCHAR*          szText      : [in] New text
// CONST BOOL            bEnabled    : [in] Whether to enable/disable button
// CONST BOOL            bVisible    : [in] Whether to show/hide button
// 
VOID  displayMessageDialogButton(MESSAGE_DIALOG_DATA*  pDialogData, CONST UINT  iControlID, CONST UINT  iNewID, CONST TCHAR*  szText, CONST BOOL  bEnabled, CONST BOOL  bVisible)
{
   // [TEXT] Change the control text
   SetDlgItemText(pDialogData->hDialog, iControlID, szText);

   // [ENABLED/VISIBLE] Set enabled and visible states
   utilEnableDlgItem(pDialogData->hDialog, iControlID, bEnabled);
   utilShowDlgItem(pDialogData->hDialog, iControlID, bVisible);

   // [ID] Change the control ID
   utilSetDlgItemID(pDialogData->hDialog, iControlID, iNewID);
}


/// Function name  : initMessageDialog
// Description     : Initialises the message dialog controls -- text, icon and buttons
// 
// MESSAGE_DIALOG_DATA*  pDialogData  : [in] MessageDialog data
// HWND                  hDialog      : [in] MessageDialog window handle
// 
// Return Value   : TRUE if succesful, FALSE if creation of the toolbar failed 
// 
BOOL  initMessageDialog(MESSAGE_DIALOG_DATA*  pDialogData, HWND  hDialog)
{
   TCHAR*  szTitle;

   // Store dialog data
   SetWindowLong(hDialog, DWL_USER, (LONG)pDialogData);
   pDialogData->hDialog = hDialog;

   /*/// [MODAL WINDOW]
   pushModalWindowStack(hDialog);*/

   /// Create toolbar
   if (!initMessageDialogToolbar(pDialogData, hDialog))
      return FALSE;

   // [OWNER DRAW]
   utilAddWindowStyle(GetControl(hDialog, IDC_DIALOG_TITLE), SS_OWNERDRAW WITH CSS_CENTRE);
   /*utilAddWindowStyle(GetControl(hDialog, IDC_DIALOG_HEADING_1), SS_OWNERDRAW);
   utilAddWindowStyle(GetControl(hDialog, IDC_DIALOG_HEADING_2), SS_OWNERDRAW);*/

   /// [CHECK] Set dialog title from resource or string
   if (pDialogData->szTitle AND IS_INTRESOURCE(pDialogData->szTitle))
      // [RESOURCE] Load dialog title
      SetDlgItemText(hDialog, IDC_DIALOG_TITLE, loadTempString((UINT)pDialogData->szTitle));
   else
      // [STRING or MISSING] Display string or placeholder
      SetDlgItemText(hDialog, IDC_DIALOG_TITLE, utilEither(pDialogData->szTitle, TEXT("Unspecified Title")));

   /// [CAPTION] Set dialog caption
   utilSetWindowTextf(hDialog, TEXT("%s - %s"), getAppName(), szTitle = utilGetDlgItemText(hDialog, IDC_DIALOG_TITLE));
   utilDeleteString(szTitle);

   /// Display appropriate buttons
   switch (extractButtonFlags(pDialogData->iAttributes))
   {
   // [OK]
   case MDF_OK:
      // [CHECK] Are we displaying a message dialog?
      if (!pDialogData->pErrorQueue)
         // [MESSAGE] Display 'OK'
         displayMessageDialogButton(pDialogData, IDC_MESSAGE_BUTTON_3, IDOK, TEXT("&OK"), TRUE, TRUE);
      else
      {  // [QUEUE] Display 'OK' and 'NEXT' 
         displayMessageDialogButton(pDialogData, IDC_MESSAGE_BUTTON_2, IDOK,       TEXT("&OK"),   TRUE, TRUE);
         displayMessageDialogButton(pDialogData, IDC_MESSAGE_BUTTON_3, IDCONTINUE, TEXT("Next"), TRUE, TRUE);
      }   
      break;

   // [YES and NO]
   case MDF_YESNO:
      displayMessageDialogButton(pDialogData, IDC_MESSAGE_BUTTON_2, IDYES, TEXT("&Yes"), TRUE, TRUE);
      displayMessageDialogButton(pDialogData, IDC_MESSAGE_BUTTON_3, IDNO,  TEXT("&No"),  TRUE, TRUE);
      break;

   // [OK and CANCEL]
   case MDF_OKCANCEL:
      displayMessageDialogButton(pDialogData, IDC_MESSAGE_BUTTON_2, IDOK,     TEXT("&OK"),     TRUE, TRUE);
      displayMessageDialogButton(pDialogData, IDC_MESSAGE_BUTTON_3, IDCANCEL, TEXT("&Cancel"), TRUE, TRUE);
      break;

   // [YES, NO and CANCEL]
   case MDF_YESNOCANCEL:
      displayMessageDialogButton(pDialogData, IDC_MESSAGE_BUTTON_1, IDYES,    TEXT("&Yes"),    TRUE, TRUE);
      displayMessageDialogButton(pDialogData, IDC_MESSAGE_BUTTON_2, IDNO,     TEXT("&No"),     TRUE, TRUE);
      displayMessageDialogButton(pDialogData, IDC_MESSAGE_BUTTON_3, IDCANCEL, TEXT("&Cancel"), TRUE, TRUE);
      break;
   }

   /// Display message, update toolbar
   updateMessageDialog(pDialogData);

   // [SUCCESS] Return TRUE
   return TRUE;
}


/// Function name  : initMessageDialogToolbar
// Description     : Create the message dialog toolbar
// 
// MESSAGE_DIALOG_DATA*  pDialogData  : [in] MessageDialog data
// HWND                  hDialog      : [in] MessageDialog window handle
// 
// Return Value   : TRUE if succesful, FALSE if toolbar was not created succesfully
// 
BOOL   initMessageDialogToolbar(MESSAGE_DIALOG_DATA*  pDialogData, HWND  hDialog)
{
   TBBUTTON      oButtonData[iToolBarButtonCount]; // Button data for the new toolbar
   TBADDBITMAP   oToolbarData;                     // Bitmap data for the new toolbar
   RECT          rcToolbarRect;                    // Rectangle of the toolbar, in dialog client co-ordinates
   DWORD         dwToolbarStyles;                  // Toolbar styles

   // Prepare
   dwToolbarStyles = (WS_CHILD WITH WS_VISIBLE) WITH (TBSTYLE_FLAT WITH TBSTYLE_TRANSPARENT WITH TBSTYLE_TOOLTIPS WITH CCS_NOPARENTALIGN WITH CCS_NOMOVEY WITH CCS_NOMOVEX WITH CCS_NORESIZE WITH CCS_NODIVIDER);
   
   /// Create ToolBar
   pDialogData->hToolbar = CreateWindowEx(NULL, TOOLBARCLASSNAME, NULL, dwToolbarStyles, 0,0, 0,0, hDialog, (HMENU)IDC_MESSAGE_TOOLBAR, getResourceInstance(), NULL);
   ERROR_CHECK("creating message dialog toolbar", pDialogData->hToolbar);
   
   // [CHECK] Ensure toolbar was created successfully
   if (pDialogData->hToolbar)
   {
      // Define bitmaps
      oToolbarData.hInst = getResourceInstance();
      oToolbarData.nID   = IDB_MESSAGE_TOOLBAR;

      // Add bitmap
      Toolbar_SetBitmapSize(pDialogData->hToolbar, 24, 24);
      Toolbar_AddBitmap(pDialogData->hToolbar, iToolBarButtonCount, &oToolbarData);

      /// Define buttons
      utilZeroObjectArray(oButtonData, TBBUTTON, iToolBarButtonCount);
      for (UINT i = 0, iBitmap = 0; i < iToolBarButtonCount; i++)
      {
         oButtonData[i].iBitmap   = i;
         oButtonData[i].fsState   = TBSTATE_ENABLED;
         oButtonData[i].fsStyle   = BTNS_BUTTON;
         oButtonData[i].idCommand = iButtonCommands[i];
         oButtonData[i].iString   = iButtonCommands[i];
      }

      // Add buttons
      Toolbar_ButtonStructSize(pDialogData->hToolbar);
      Toolbar_AddButtons(pDialogData->hToolbar, iToolBarButtonCount, &oButtonData);

      // Resize toolbar
      Toolbar_AutoSize(pDialogData->hToolbar);

      /// Position toolbar beneath dialog icon
      calculateMessageDialogToolbarPosition(pDialogData, hDialog, &rcToolbarRect);
      utilSetDlgItemRect(hDialog, IDC_MESSAGE_TOOLBAR, &rcToolbarRect, TRUE);
   }

   // Return TRUE if successful
   return (pDialogData->hToolbar != NULL);
}


/// Function name  : updateMessageDialog
// Description     : Updates the dialog text, buttons and toolbar to reflect the current state
// 
// MESSAGE_DIALOG_DATA*  pDialogData   : [in] Dialog data
// 
VOID  updateMessageDialog(MESSAGE_DIALOG_DATA*  pDialogData)
{
   CONST ERROR_MESSAGE*  pDummy;       // Used for testing for the presence of messages in the ErrorStack
   LIST_ITEM*            pStackItem;   // ListItem wrapper for the current ErrorStack

   // [CHECK] Is this an error dialog?
   if (pDialogData->pErrorMessage)
   {
      /// [ERROR] Display message
      SetDlgItemText(pDialogData->hDialog, IDC_MESSAGE_TEXT_STATIC, pDialogData->pErrorMessage->szMessage);

      // [CODE] Display code, if any
      SetDlgItemText(pDialogData->hDialog, IDC_MESSAGE_CODE_STATIC, (lstrlen(pDialogData->pErrorMessage->szID) ? pDialogData->pErrorMessage->szID : TEXT("None")));
      //utilShowDlgItem(pDialogData->hDialog, IDC_DIALOG_HEADING_2, lstrlen(pDialogData->pErrorMessage->szID) ? TRUE : FALSE);
   }
   else  
   {
      /// [MESSAGE] Lookup and display message
      SetDlgItemText(pDialogData->hDialog, IDC_MESSAGE_TEXT_STATIC, pDialogData->szSimpleMessage);
      SetDlgItemText(pDialogData->hDialog, IDC_MESSAGE_CODE_STATIC, TEXT("Not appliciable"));
      //utilShowDlgItem(pDialogData->hDialog, IDC_DIALOG_HEADING_2, FALSE);
   }

   // [CHECK] Is this an ErrorQueue dialog?
   if (pDialogData->pErrorQueue)
      /// [NEXT] Disable button if Stack is the last in the queue
      utilEnableDlgItem(pDialogData->hDialog, IDCONTINUE, findListItemByValue(pDialogData->pErrorQueue, (LPARAM)pDialogData->pErrorStack, pStackItem) AND pStackItem->pNext);

   /// [TOOLBAR] Enable/disable toolbar buttons
   Toolbar_EnableButton(pDialogData->hToolbar, IDC_MESSAGE_COPY, TRUE);

   // [ATTACHMENTS] Check for any attachments
   Toolbar_EnableButton(pDialogData->hToolbar, IDC_MESSAGE_ATTACHMENTS, findErrorMessageByLogicalIndex(pDialogData->pErrorStack, 0, MT_ATTACHMENT, pDummy));
   // [LESSER/GREATER] Check for adjacent messages
   Toolbar_EnableButton(pDialogData->hToolbar, IDC_MESSAGE_LESS, findErrorMessageByLogicalIndex(pDialogData->pErrorStack, pDialogData->iStackIndex - 1, MT_MESSAGE, pDummy));
   Toolbar_EnableButton(pDialogData->hToolbar, IDC_MESSAGE_MORE, findErrorMessageByLogicalIndex(pDialogData->pErrorStack, pDialogData->iStackIndex + 1, MT_MESSAGE, pDummy));
}


/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        MESSAGE HANDLERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onMessageDialogAdjustDetailLevel
// Description     : Display the next message in the error stack
// 
// MESSAGE_DIALOG_DATA*  pDialogData   : [in] MessageDialog data
// CONST BOOL            bGreater      : [in] TRUE for greater detail, FALSE for less detail
// 
VOID  onMessageDialogAdjustDetailLevel(MESSAGE_DIALOG_DATA*  pDialogData, CONST BOOL  bGreater)
{
   // Move to the next/prev ErrorStack
   findErrorMessageByLogicalIndex(pDialogData->pErrorStack, (bGreater ? ++pDialogData->iStackIndex : --pDialogData->iStackIndex), MT_MESSAGE, pDialogData->pErrorMessage);

   // Display new message
   updateMessageDialog(pDialogData);
}


/// Function name  : onMessageDialogCommand
// Description     : WM_COMMAND processing
// 
// MESSAGE_DIALOG_DATA*  pDialogData   : [in] MessageDialog data
// HWND                  hDialog       : [in] MessageDialog window handle
// CONST UINT            iControlID    : [in] ID of the control sending the notification
// CONST UINT            iNotification : [in] Notification code
// HWND                  hCtrl         : [in] Window handle of the control sending the notification
// 
// Return Value   : TRUE if processed, FALSE otherwise
// 
BOOL  onMessageDialogCommand(MESSAGE_DIALOG_DATA*  pDialogData, CONST UINT  iControlID, CONST UINT  iNotification, HWND  hCtrl)
{
   static const TCHAR*  szButtonNames[] = { NULL, TEXT("OK"), TEXT("Cancel"), NULL, NULL, NULL, TEXT("Yes"), TEXT("No") };

   switch (iControlID)
   {
   /// [YES/NO, OK/CANCEL] - Close dialog and return button ID
   case IDOK:
   case IDCANCEL:
   case IDYES:
   case IDNO:
      CONSOLE("Message dialog dismissed with button '%s'", szButtonNames[iControlID]);

      // [CHECK] Has user closed a YES/NO dialog via SC_CLOSE?
      if ((pDialogData->iAttributes INCLUDES MDF_YESNO) AND iControlID == IDCANCEL)
         // [SPECIAL CASE] Return IDNO
         EndDialog(pDialogData->hDialog, IDNO);
      else 
         // Return button ID
         EndDialog(pDialogData->hDialog, iControlID);
      return TRUE;

   /// [NEXT ERROR]
   case IDCONTINUE:
      onMessageDialogNextError(pDialogData);
      return TRUE;

   /// [COPY MESSAGE]
   case IDC_MESSAGE_COPY:
      onMessageDialogCopyMessage(pDialogData);
      return TRUE;

   /// [DISPLAY ATTACHMENTS MENU]
   case IDC_MESSAGE_ATTACHMENTS:
      onMessageDialogContextMenu(pDialogData);
      return TRUE;

   /// [DISPLAY PREVIOUS MESSAGE]
   case IDC_MESSAGE_LESS:
      onMessageDialogAdjustDetailLevel(pDialogData, FALSE);
      return TRUE;

   /// [DISPLAY NEXT MESSAGE]
   case IDC_MESSAGE_MORE:
      onMessageDialogAdjustDetailLevel(pDialogData, TRUE);
      return TRUE;

   /*case IDM_FIRST_ATTACHMENT:
   case IDM_FIRST_ATTACHMENT+1:
   case IDM_FIRST_ATTACHMENT+2:
   case IDM_FIRST_ATTACHMENT+3:
   case IDM_FIRST_ATTACHMENT+4:
   case IDM_FIRST_ATTACHMENT+5:*/
   /// [ATTACHMENT MENU ITEMS]
   default:
      if (iControlID >= IDM_FIRST_ATTACHMENT AND iControlID <= IDM_FIRST_ATTACHMENT + 10)
      {
         onMessageDialogDisplayAttachment(pDialogData, iControlID);
         return TRUE;
      }
      break;
   }

   return FALSE;
}


/// Function name  : onMessageDialogContextMenu
// Description     : Display the attachments context menu
// 
// MESSAGE_DIALOG_DATA*  pDialogData   : [in] MessageDialog data
// 
VOID  onMessageDialogContextMenu(MESSAGE_DIALOG_DATA*  pDialogData)
{
   CONST ERROR_MESSAGE*  pErrorMessage;      // Attachment error message currently being processed
   HMENU                 hAttachmentMenu;    // Popup menu being assembled
   CONST TCHAR*          szItemText;         // Text for the menu item currently being created
   POINT                 ptCursor;           // Position to the display the menu at

   // Create Popup menu
   hAttachmentMenu = CreatePopupMenu();

   /// Iterate through attachment messages (if any)
   for (UINT iMessage = 0; findErrorMessageByLogicalIndex(pDialogData->pErrorStack, iMessage, MT_ATTACHMENT, pErrorMessage); iMessage++)
   {
      // Convert Error ID into a string
      switch (pErrorMessage->iID)      
      {   
         case IDS_ERROR_APPEND_COMMAND:      szItemText = TEXT("COMMAND Object");      break;
         case IDS_ERROR_APPEND_XML:          szItemText = TEXT("XML Extract");         break;
         case IDS_ERROR_APPEND_TEXT:         szItemText = TEXT("Output Text");          break;
         default: ASSERT(FALSE);             szItemText = TEXT("Error");               break;
      }

      /// Append item to menu
      AppendMenu(hAttachmentMenu, MF_STRING WITH MF_ENABLED, IDM_FIRST_ATTACHMENT + iMessage, szItemText);
   }

   // Convert
   convertSystemMenuToCustomMenu(hAttachmentMenu, TRUE);

   /// Display popup menu
   GetCursorPos(&ptCursor);
   TrackPopupMenu(hAttachmentMenu, TPM_LEFTALIGN WITH TPM_LEFTBUTTON, ptCursor.x, ptCursor.y, NULL, pDialogData->hDialog, NULL);

   // Cleanup and return
   convertCustomMenuToSystemMenu(hAttachmentMenu);
   DestroyMenu(hAttachmentMenu);
}


/// Function name  : onMessageDialogCopyMessage
// Description     : Copy all the messages in the error stack (plus attachments) to the clipboard
// 
// MESSAGE_DIALOG_DATA*  pDialogData   : [in] MessageDialog data
// HWND                  hDialog       : [in] MessageDialog window handle
// 
VOID  onMessageDialogCopyMessage(MESSAGE_DIALOG_DATA*  pDialogData)
{
   TCHAR*   szOutput;            // Buffer containing the text to be copied. For error messages this contains every message in the stack.
   UINT     iOutputLength;       // Length of the contents of the buffer above, in characters
   HANDLE   hClipboardMemory;    // Clipboard memory handle
   TCHAR*   szClipboardBuffer;   // Clipboard memory buffer

   if (OpenClipboard(pDialogData->hDialog))
   {
      // [CHECK] Is this an error dialog?
      if (pDialogData->pErrorStack)
         /// [ERROR MESSAGE] - Flatten the error stack into a single buffer
         szOutput = generateFlattenedErrorStackText(pDialogData->pErrorStack);
      else
         /// [MESSAGE] - Copy the current message text
         szOutput = utilGetDlgItemText(pDialogData->hDialog, IDC_MESSAGE_TEXT_STATIC);
      
      // Measure output
      iOutputLength = lstrlen(szOutput);
      
      /// Allocate global memory output buffer
      hClipboardMemory = GlobalAlloc(GMEM_MOVEABLE, (iOutputLength + 1) * sizeof(TCHAR));

      // Copy output to the clipboard buffer
      szClipboardBuffer = (TCHAR*)GlobalLock(hClipboardMemory);
      StringCchCopy(szClipboardBuffer, iOutputLength + 1, szOutput);
      // Ensure null termination
      szClipboardBuffer[iOutputLength] = NULL;
      
      /// Transfer to clipboard
      GlobalUnlock(hClipboardMemory);
      SetClipboardData(CF_UNICODETEXT, hClipboardMemory);

      // Cleanup and return
      utilDeleteString(szOutput);
      CloseClipboard();
   }
}


/// Function name  : onMessageDialogDestroy
// Description     : Destroy the dialog data
// 
// MESSAGE_DIALOG_DATA*  pDialogData  : [in] MessageDialog data
// HWND                  hDialog      : [in] MessageDialog window handle
// 
VOID  onMessageDialogDestroy(MESSAGE_DIALOG_DATA*  pDialogData)
{
   /*/// [MODAL WINDOW]
   popModalWindowStack();*/

   // Sever and delete dialog data
   SetWindowLong(pDialogData->hDialog, DWL_USER, NULL);
   deleteMessageDialogData(pDialogData);
}


/// Function name  : onMessageDialogDisplayAttachment
// Description     : Display the attachment specified by the menu item
// 
// MESSAGE_DIALOG_DATA*  pDialogData  : [in] MessageDialog data
// HWND                  hDialog      : [in] MessageDialog window handle
// CONST UINT            iMenuItemID  : [in] ID of the menu item
// 
VOID  onMessageDialogDisplayAttachment(MESSAGE_DIALOG_DATA*  pDialogData, CONST UINT  iMenuItemID)
{
   CONST ERROR_MESSAGE*  pAttachmentMessage;    // Attachment message associated with the menu item
   UINT                  iAttachmentIndex;      // Logical index of the associated attachment message
   
   // [CHECK] Ensure we're displaying an error message, not a normal message
   ASSERT(pDialogData->pErrorStack);

   /// Lookup the attachment message
   iAttachmentIndex = (iMenuItemID - IDM_FIRST_ATTACHMENT);
   findErrorMessageByLogicalIndex(pDialogData->pErrorStack, iAttachmentIndex, MT_ATTACHMENT, pAttachmentMessage);

   // [CHECK] The attachment should always be found
   ASSERT(pAttachmentMessage);

   /// Display attachment
   showDialog(TEXT("ATTACHMENT_DIALOG"), pDialogData->hDialog, dlgprocAttachmentDialog, (LPARAM)pAttachmentMessage);
}


/// Function name  : onMessageDialogDrawItem
// Description     : Draws the message text centred both vertically and horizontally
// 
// MESSAGE_DIALOG_DATA*  pDialogData  : [in] MessageDialog data
// HWND                  hDialog      : [in] MessageDialog window handle
// CONST UINT            iControlID   : [in] ID of the control requesting drawing
// DRAWITEMSTRUCT*       pDrawInfo    : [in] OwnerDraw WM_DRAWITEM data
// 
// Return Value   : TRUE if processed, FALSE otherwise
// 
BOOL  onMessageDialogDrawItem(MESSAGE_DIALOG_DATA*  pDialogData, CONST UINT  iControlID, DRAWITEMSTRUCT*  pDrawInfo)
{
   BOOL    bResult;

   // Prepare
   bResult = FALSE;

   // Examine source
   switch (iControlID)
   {
   /// [ICON/MENU]
   case IDC_DIALOG_ICON:   bResult = onOwnerDrawStaticIcon(pDrawInfo, pDialogData->szIcon, 72);    break;
   case NULL:              bResult = onOwnerDrawCustomMenu(pDrawInfo);                             break;
   }      

   // Return result
   return bResult;
}


/// Function name  : onMessageDialogNextError
// Description     : Display the next stack of the error messages (if any) in the dialog's error queue (if any)
// 
// MESSAGE_DIALOG_DATA*  pDialogData   : [in] MessageDialog data
// HWND                  hDialog       : [in] MessageDialog window handle
// 
VOID  onMessageDialogNextError(MESSAGE_DIALOG_DATA*  pDialogData)
{
   LIST_ITEM*  pStackItem;

   // [CHECK] Ensure this an ErrorQueue dialog
   ASSERT(pDialogData->pErrorQueue);

   // Lookup current ErrorStack
   findListItemByValue(pDialogData->pErrorQueue, (LPARAM)pDialogData->pErrorStack, pStackItem);
   
   // Move to next ErrorStack
   ASSERT(pStackItem->pNext);
   pDialogData->pErrorStack = extractListItemPointer(pStackItem, ERROR_STACK);

   // Lookup first message in new stack
   findErrorMessageByLogicalIndex(pDialogData->pErrorStack, 0, MT_MESSAGE, pDialogData->pErrorMessage);

   // Update dialog
   updateMessageDialog(pDialogData);
}


/// Function name  : onMessageDialogNotify
// Description     : WM_NOTIFY processing
// 
// MESSAGE_DIALOG_DATA*  pDialogData   : [in] MessageDialog data
// HWND                  hDialog       : [in] MessageDialog window handle
// CONST UINT            iControlID    : [in] ID of the control sending the notification
// NMHDR*                pMessage      : [in] WM_NOTIFY notification data
// 
// Return Value   : TRUE if processed, FALSE otherwise
// 
BOOL  onMessageDialogNotify(MESSAGE_DIALOG_DATA*  pDialogData, CONST UINT  iControlID, NMHDR*  pMessage)
{
   BOOL  bResult;

   // Prepare
   bResult = FALSE;

   // Examine notification
   switch (pMessage->code)
   {
   /// [TOOLBAR TOOLTIPS] Retrieve tooltip text
   case TTN_GETDISPINFO:
      // [CHECK] Ensure resource library is loaded
      if (getResourceInstance())
         bResult = onTooltipRequestText((NMTTDISPINFO*)pMessage);
      break;
   }

   // Return result
   return bResult;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                    DIALOG PROCEDURES
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : dlgprocMessageDialog
// Description     : Message dialog window procedure
// 
INT_PTR  dlgprocMessageDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   MESSAGE_DIALOG_DATA*  pDialogData;
   BOOL                  bResult;

   TRY
   {
      // Prepare
      pDialogData = getMessageDialogData(hDialog);
      bResult     = FALSE;

      // Examine message
      switch (iMessage)
      {
      /// [CREATION]
      case WM_INITDIALOG:
         bResult = initMessageDialog((MESSAGE_DIALOG_DATA*)lParam, hDialog);
         break;

      /// [DESTRUCTION]
      case WM_DESTROY:
         onMessageDialogDestroy(pDialogData);
         bResult = TRUE;
         break;

      /// [COMMAND PROCESSING]
      case WM_COMMAND:
         bResult = onMessageDialogCommand(pDialogData, LOWORD(wParam), HIWORD(wParam), (HWND)lParam);
         break;

      /// [NOTIFICATION]
      case WM_NOTIFY:
         bResult = onMessageDialogNotify(pDialogData, wParam, (NMHDR*)lParam);
         break;

      /// [OWNER DRAW]
      case WM_DRAWITEM:     bResult = onMessageDialogDrawItem(pDialogData, wParam, (DRAWITEMSTRUCT*)lParam);   break;
      case WM_MEASUREITEM:  bResult = onWindow_MeasureItem(hDialog, (MEASUREITEMSTRUCT*)lParam);               break;

      /// [HELP]
      case WM_HELP:
         bResult = displayHelp(TEXT("Window_Output"));
         break;
      }

      // Return TRUE, or pass to Vista handler
      return (bResult ? TRUE : dlgprocVistaStyleDialog(hDialog, iMessage, wParam, lParam));
   }
   /// [EXCEPTION HANDLER]
   CATCH3("iMessage=%s  wParam=%d  lParam=%d", identifyMessage(iMessage), wParam, lParam);
   return dlgprocVistaStyleDialog(hDialog, iMessage, wParam, lParam);
}


/// Function name  : dlgprocAttachmentDialog
// Description     : Window procedure for the new attachment dialog
// 
//
INT_PTR   dlgprocAttachmentDialog(HWND  hDialog, UINT  message, WPARAM  wParam, LPARAM  lParam)
{
   CONST ERROR_MESSAGE*  pMessage;

   switch (message)
   {
   /// [CREATION] -- Display attachment message
   case WM_INITDIALOG:
      // Extract input data
      pMessage = (CONST ERROR_MESSAGE*)lParam;
      // Display attachment text
      SetDlgItemText(hDialog, IDC_ATTACHMENTS, pMessage->szMessage);
      // Remove the selection from the attachment edit control
      SetFocus(GetDlgItem(hDialog, IDC_ATTACHMENTS));
      SendDlgItemMessage(hDialog, IDC_ATTACHMENTS, EM_SETSEL, -1, NULL);
      return FALSE;

   /// [COMMAND PROCESSING] -- Close dialog
   case WM_COMMAND:
      switch (LOWORD(wParam))
      {
      // [OK / CLOSE BUTTON]
      case IDOK:
      case IDCANCEL:
         EndDialog(hDialog, LOWORD(wParam));
         return TRUE;
      }
      break;
   
   /// [VISUAL STYLES]
   case WM_CTLCOLORDLG:
   case WM_CTLCOLORSTATIC:
      return (BOOL)onDialog_ControlColour((HDC)wParam, COLOR_WINDOW);

   /// [VISUAL STYLES] 
   case WM_CTLCOLORBTN:
      return (INT_PTR)GetStockObject(WHITE_BRUSH);
   }

   return FALSE;
}

