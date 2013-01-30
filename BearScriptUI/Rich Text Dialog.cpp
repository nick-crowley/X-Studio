//
// Rich Text Dialog.cpp : A RichText manipulation dialog consisting of a RichEdit control and a formatting toolbar.
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                           CONSTANTS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

CONST UINT   iToolBarButtonCount    = 17,    // Number of buttons and separators in the toolbar
             iToolBarSeparatorCount = 4,     // Number of separators in the toolbar
             iMessageTextSize       = 8,     // Point size of message text
             iMessageTitleSize      = 11;    // Point size of message titles

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       CREATION / DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createRichTextDialogToolBar
// Description     : Creates the RichText dialog's toolbar
// 
// LANGUAGE_DOCUMENT*  pDocument : [in] Language document window data
// HWND                hDialog   : [in] Window handle of the RichText dialog
// 
// Return Value   : Window handle of the toolbar or NULL if unsuccesful
// 
BOOL   createRichTextDialogToolBar(LANGUAGE_DOCUMENT*  pDocument, HWND  hDialog)
{
   TBBUTTON      oButtonData[iToolBarButtonCount];    // Array of button properties
   TBADDBITMAP   oToolbarData;                        // Identifies the toolbar bitmap

   /// Create ToolBar
   pDocument->hToolBar = CreateWindowW(TOOLBARCLASSNAME, NULL, (WS_CHILD WITH WS_VISIBLE) WITH (TBSTYLE_FLAT WITH TBSTYLE_TRANSPARENT WITH TBSTYLE_TOOLTIPS) WITH CCS_NODIVIDER , 
                                       0,0,0,0, hDialog, (HMENU)IDC_LANGUAGE_TOOLBAR, getAppInstance(), NULL);
   ERROR_CHECK("creating language document toolbar", pDocument->hToolBar);
   if (!pDocument->hToolBar)
      return FALSE;

   // Add 24x24 BItmap ImageStrip
   oToolbarData.hInst = getResourceInstance();
   oToolbarData.nID = IDB_LANGUAGE_TOOLBAR;
   SendMessage(pDocument->hToolBar, TB_SETBITMAPSIZE, NULL, MAKE_LONG(24,24));
   SendMessage(pDocument->hToolBar, TB_ADDBITMAP, iToolBarButtonCount - iToolBarSeparatorCount, (LPARAM)&oToolbarData);

   // Define button array
   utilZeroObjectArray(oButtonData, TBBUTTON, iToolBarButtonCount);
   for (UINT i = 0, iBitmap = 0; i < iToolBarButtonCount; i++)
   {
      oButtonData[i].iString   = NULL; 
      // Assign command ID. disable all buttons.
      oButtonData[i].idCommand = identifyRichTextDialogToolBarCommandID(i);
      oButtonData[i].fsState   = NULL;
      // Assign the next bitmap index to buttons, skip it for separators
      oButtonData[i].iBitmap = (oButtonData[i].idCommand > 0 ? iBitmap++ : NULL);
      oButtonData[i].fsStyle = (oButtonData[i].idCommand > 0 ? BTNS_BUTTON : BTNS_SEP);
   }

   // Add buttons
   SendMessage(pDocument->hToolBar, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), NULL);
   SendMessage(pDocument->hToolBar, TB_ADDBUTTONS, iToolBarButtonCount, (LPARAM)&oButtonData);
   // Resize toolbar
   SendMessage(pDocument->hToolBar, TB_AUTOSIZE, NULL, NULL);
   return TRUE;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                           HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : identifyGameTextColourFromColourMenuID
// Description     : Convert a colour menu item ID into it's equivilent colour ID
// 
// CONST UINT  iCommandID   : [in] Command ID of the colour menu item
// 
// Return Value   : Game text colour or DEFAULT if unrecognised
// 
GAME_TEXT_COLOUR  identifyTextColourFromCommandID(CONST UINT  iCommandID)
{
   GAME_TEXT_COLOUR  eOutput;

   switch (iCommandID)
   {
   case IDM_COLOUR_BLACK:     eOutput = GTC_BLACK;    break;
   case IDM_COLOUR_BLUE:      eOutput = GTC_BLUE;     break;
   case IDM_COLOUR_CYAN:      eOutput = GTC_CYAN;     break;
   case IDM_COLOUR_GREEN:     eOutput = GTC_GREEN;    break;
   case IDM_COLOUR_ORANGE:    eOutput = GTC_ORANGE;   break;
   case IDM_COLOUR_PURPLE:    eOutput = GTC_PURPLE;   break;
   case IDM_COLOUR_RED:       eOutput = GTC_RED;      break;
   case IDM_COLOUR_WHITE:     eOutput = GTC_WHITE;    break;
   case IDM_COLOUR_YELLOW:    eOutput = GTC_YELLOW;   break;
   default:                   eOutput = GTC_DEFAULT;  break;
   }
   
   return eOutput;
}


/// Function name  : identifyRichTextDialogToolBarCommandID
// Description     : Determine the command ID of a RichText dialog toolbar button from it's position
// 
// CONST UINT  iIndex   [in] Zero based position of the button
// 
// Return Value   : Command ID or NULL if the index represents a separator
// 
UINT  identifyRichTextDialogToolBarCommandID(CONST UINT  iIndex)
{
   static UINT iButtonCommands[iToolBarButtonCount] = {
                IDM_EDIT_CUT,       IDM_EDIT_COPY,       IDM_EDIT_PASTE,         IDM_EDIT_DELETE,
          NULL, IDM_RICHEDIT_BOLD,  IDM_RICHEDIT_ITALIC, IDM_RICHEDIT_UNDERLINE, 
          NULL, IDM_RICHEDIT_COLOUR, 
          NULL, IDM_RICHEDIT_LEFT,  IDM_RICHEDIT_CENTRE, IDM_RICHEDIT_RIGHT,     IDM_RICHEDIT_JUSTIFY,
          NULL, IDM_RICHEDIT_BUTTON };

   // [CHECK] Index is valid
   ASSERT(iIndex < iToolBarButtonCount);
   return iButtonCommands[iIndex];
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : getRichTextDialogButtonCount
// Description     : Retrieves the button count for the current message
// 
// LANGUAGE_DOCUMENT*  pDocument : [in] Language Document
// 
// Return Value   : Button Count
// 
UINT  getRichTextDialogButtonCount(LANGUAGE_DOCUMENT*  pDocument)
{
   IRichEditOle*  pRichEdit;
   UINT           iCount = 0;

   // Count number of OLE objects
   if (RichEdit_GetOLEInterface(pDocument->hRichEdit, &pRichEdit))
   {
      iCount = pRichEdit->GetObjectCount();
      utilReleaseInterface(pRichEdit);
   }

   // Return count
   return iCount;
}


/// Function name  : initRichTextDialog
// Description     : Initialise the toolbar and RichEdit control in a RichText dialog
// 
// LANGUAGE_DOCUMENT*  pDocument  : [in] Language document hosting the RichText dialog
// HWND                hDialog    : [in] RichText dialog window handle
// 
// Return Value   : TRUE
// 
BOOL  initRichTextDialog(LANGUAGE_DOCUMENT*  pDocument, HWND  hDialog)
{
   CHARFORMAT  oCharFormat;

   // Store language document as window data
   SetWindowLong(hDialog, DWL_USER, (LONG)pDocument);

   /// Create Toolbar
   createRichTextDialogToolBar(pDocument, hDialog);

   // Save RichEdit control convenience handle
   pDocument->hRichEdit = GetDlgItem(hDialog, IDC_LANGUAGE_EDIT);

   // Set default formatting
   oCharFormat.cbSize      = sizeof(CHARFORMAT);
   oCharFormat.dwMask      = CFM_COLOR WITH CFM_FACE;
   oCharFormat.crTextColor = getGameTextColour(GTC_DEFAULT);
   StringCchCopy(oCharFormat.szFaceName, LF_FACESIZE, TEXT("Arial"));
   RichEdit_SetCharFormat(pDocument->hRichEdit, SCF_DEFAULT, &oCharFormat);

   /// Setup RichEdit control
   RichEdit_SetBackgroundColour(pDocument->hRichEdit, FALSE, RGB(22,31,46));
   RichEdit_SetEventMask(pDocument->hRichEdit, ENM_UPDATE WITH ENM_SELCHANGE WITH ENM_CHANGE); 
   RichEdit_SetFontSize(pDocument->hRichEdit, 14);
   Edit_SetReadOnly(pDocument->hRichEdit, pDocument->bVirtual);

   // Clear RichEdit
   SetWindowText(pDocument->hRichEdit, TEXT(""));

   /// Create ButtonData tree
   pDocument->pButtonsByID = createLanguageButtonTreeByID();

   /// Create RichEdit-Callback
   pDocument->pOleCallback = new RichEditCallback(hDialog);
   pDocument->pOleCallback->QueryInterface(IID_IRichEditOleCallback, (IInterface*)&pDocument->pOleCallback);
   RichEdit_SetOLECallback(pDocument->hRichEdit, pDocument->pOleCallback);

   // Enable/disable appropriate toolbar buttons
   updateRichTextDialogToolBar(pDocument, hDialog);

   return TRUE;
}


/// Function name  : performRichEditFormatCommand
// Description     : Perform one of the formatting commands available from the toolbar
// 
// LANGUAGE_DOCUMENT*  pDocument : [in] Language document hosting the RichText dialog
// HWND                hDialog   : [in] Window handle of the RichText dialog
// CONST UINT          iCommand  : [in] Command ID of the desired toolbar command
// 
// Return Value   : TRUE
// 
BOOL performRichEditFormatCommand(LANGUAGE_DOCUMENT*  pDocument, HWND  hDialog, CONST UINT  iCommand)
{
   CHARFORMAT   oCharacterFormat;   // Current Text attributes
   PARAFORMAT   oParagraphFormat;   // Current Paragraph attributes
   UINT         iProperty;          // Text or paragraph property to alter

   /// Determine from the CommandID which text/paragraph property to alter
   switch (iCommand)
   {
   // [FORMATTING]
   case IDM_RICHEDIT_BOLD:       iProperty = CFE_BOLD;         break;
   case IDM_RICHEDIT_ITALIC:     iProperty = CFE_ITALIC;       break;
   case IDM_RICHEDIT_UNDERLINE:  iProperty = CFE_UNDERLINE;    break;
   // [ALIGNMENT]
   case IDM_RICHEDIT_LEFT:       iProperty = PA_LEFT;          break;
   case IDM_RICHEDIT_CENTRE:     iProperty = PA_CENTRE;        break;
   case IDM_RICHEDIT_RIGHT:      iProperty = PA_RIGHT;         break;
   case IDM_RICHEDIT_JUSTIFY:    iProperty = PA_JUSTIFY;       break;
   // [COLOUR]
   default:                      iProperty = CFM_COLOR;        break;
   // [CLIPBOARD]
   case IDM_EDIT_CUT:        return SendMessage(pDocument->hRichEdit, WM_CUT,   NULL, NULL);
   case IDM_EDIT_COPY:       return SendMessage(pDocument->hRichEdit, WM_COPY,  NULL, NULL);
   case IDM_EDIT_PASTE:      return SendMessage(pDocument->hRichEdit, WM_PASTE, NULL, NULL);
   case IDM_EDIT_DELETE:     return SendMessage(pDocument->hRichEdit, WM_CLEAR, NULL, NULL);
   }

   /// Alter the specified property
   switch (iCommand)
   {
   /// [BOLD/ITALIC/UNDERLINE] - Toggle the specified text attribute
   /// [COLOUR]                - Set the specified colour
   default:
      // Prepare
      utilZeroObject(&oCharacterFormat, CHARFORMAT);
      oCharacterFormat.cbSize = sizeof(CHARFORMAT);

      // Add specified property to current attributes mask
      SendMessage(pDocument->hRichEdit, EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM)&oCharacterFormat);
      oCharacterFormat.dwMask |= iProperty;

      // Set property
      switch (iCommand)
      {
      // [BOLD/ITALIC/UNDERLINE] Flip the presence of the desired property bit
      case IDM_RICHEDIT_BOLD:
      case IDM_RICHEDIT_ITALIC:
      case IDM_RICHEDIT_UNDERLINE:
         oCharacterFormat.dwEffects ^= iProperty;
         break;

      // [COLOUR] Set the correct text colour
      default:
         oCharacterFormat.crTextColor = getGameTextColour(identifyTextColourFromCommandID(iCommand));
         break;
      }
      // Save new attributes
      RichEdit_SetCharFormat(pDocument->hRichEdit, SCF_SELECTION, &oCharacterFormat);
      break;

   /// [LEFT/CENTRE/RIGHT/JUSTIFY] - Change the current paragraph alignment
   case IDM_RICHEDIT_LEFT:
   case IDM_RICHEDIT_CENTRE:
   case IDM_RICHEDIT_RIGHT:
   case IDM_RICHEDIT_JUSTIFY:
      // Prepare
      utilZeroObject(&oParagraphFormat, PARAFORMAT);
      oParagraphFormat.cbSize = sizeof(PARAFORMAT);

      // Set the new alignment of the current paragraph
      oParagraphFormat.dwMask     = PFM_ALIGNMENT;
      oParagraphFormat.wAlignment = iProperty;
      RichEdit_SetParagraphFormat(pDocument->hRichEdit, &oParagraphFormat);
      break;
   }

   return TRUE;
}


/// Function name  : updateRichTextDialogToolBar
// Description     : Enable or Disable the edit dialog's toolbar buttons based on various properties
// 
// LANGUAGE_DOCUMENT*  pDocument : [in] Language document window data
// HWND                hDialog   : [in] Edit dialog window handle
// 
// Return Value   : TRUE
// 
BOOL  updateRichTextDialogToolBar(LANGUAGE_DOCUMENT*  pDocument, HWND  hDialog)
{
   BOOL         bButtonState,      // New state for the button being processed
                bButtonCheck,
                bEditHasFocus;     // TRUE if the edit has input focus
   CHARRANGE    oSelection;        // Current text selection
   CHARFORMAT   oCharacterFormat;  // Text attributes
   PARAFORMAT   oParagraphFormat;  // Paragraph attributes

   // Prepare
   bEditHasFocus = (GetFocus() == pDocument->hRichEdit);

   /// [STATE] Enable/Disable buttons 
   for (UINT iButton = IDM_EDIT_CUT; iButton <= IDM_RICHEDIT_BUTTON; iButton++)
   {
      // [DEFAULT] Ensure we have the input focus + doc isn't game data
      bButtonState = bEditHasFocus AND !pDocument->bVirtual;

      switch (iButton)
      {
      // [SEPARATOR] - Skip
      case NULL:  
         continue;

      /// [CUT/COPY/DELETE] - Ensure there is a selection
      case IDM_EDIT_CUT:
      case IDM_EDIT_COPY:
      case IDM_EDIT_DELETE:
         SendMessage(pDocument->hRichEdit, EM_EXGETSEL, NULL, (LPARAM)&oSelection);
         bButtonState &= (oSelection.cpMin != oSelection.cpMax);
         break;

      /// [PASTE] - Ensure clipboard contains text
      case IDM_EDIT_PASTE:
         bButtonState &= SendMessage(pDocument->hRichEdit, EM_CANPASTE, CF_UNICODETEXT, NULL) WITH SendMessage(pDocument->hRichEdit, EM_CANPASTE, CF_TEXT, NULL);
         break;
      }

      Toolbar_EnableButton(pDocument->hToolBar, iButton, bButtonState);

      // [DISABLED] Ensure unchecked, otherwise the button is improperly greyed
      if (!bButtonState)
         Toolbar_CheckButton(pDocument->hToolBar, iButton, FALSE);
   }

   /// [CHECK] Check/Uncheck buttons
   if (bEditHasFocus)
   {
      // Get current paragraph alignment
      oParagraphFormat.cbSize = sizeof(PARAFORMAT);
      oParagraphFormat.dwMask = PFM_ALIGNMENT;
      SendMessage(pDocument->hRichEdit, EM_GETPARAFORMAT, SCF_SELECTION, (LPARAM)&oParagraphFormat);

      /// [PARAGRAPH ALIGNMENT] 
      for (UINT iButton = IDM_RICHEDIT_LEFT; iButton <= IDM_RICHEDIT_JUSTIFY; iButton++)
      {
         // Determine whether the alignment matches the current button's command ID, then check/uncheck it appropriately
         switch (iButton)
         {
         case IDM_RICHEDIT_LEFT:    bButtonCheck = oParagraphFormat.wAlignment == PA_LEFT;     break;
         case IDM_RICHEDIT_CENTRE:  bButtonCheck = oParagraphFormat.wAlignment == PA_CENTRE;   break;
         case IDM_RICHEDIT_RIGHT:   bButtonCheck = oParagraphFormat.wAlignment == PA_RIGHT;    break;
         case IDM_RICHEDIT_JUSTIFY: bButtonCheck = oParagraphFormat.wAlignment == PA_JUSTIFY;  break;
         }
         SendMessage(pDocument->hToolBar, TB_CHECKBUTTON, iButton, bButtonCheck);
      }

      // Get current Bold/Italic/Underline state
      oCharacterFormat.cbSize = sizeof(CHARFORMAT);
      oCharacterFormat.dwMask = CFM_BOLD WITH CFM_ITALIC WITH CFM_UNDERLINE;
      SendMessage(pDocument->hRichEdit, EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM)&oCharacterFormat);

      /// [TEXT FORMATTING] 
      for (UINT iButton = IDM_RICHEDIT_BOLD; iButton <= IDM_RICHEDIT_UNDERLINE; iButton++)
      {
         // Check each button that matches the text's attributes
         switch (iButton)
         {
         case IDM_RICHEDIT_BOLD:      bButtonCheck = oCharacterFormat.dwEffects INCLUDES CFE_BOLD;       break;
         case IDM_RICHEDIT_ITALIC:    bButtonCheck = oCharacterFormat.dwEffects INCLUDES CFE_ITALIC;     break;
         case IDM_RICHEDIT_UNDERLINE: bButtonCheck = oCharacterFormat.dwEffects INCLUDES CFE_UNDERLINE;  break;
         }
         SendMessage(pDocument->hToolBar, TB_CHECKBUTTON, iButton, bButtonCheck);
      }
   }

   return TRUE;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         MESSAGE HANDLERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

VOID  onRichTextDialog_FormatButton(LANGUAGE_DOCUMENT*  pDocument, const GAME_TEXT_COLOUR  eColour)
{
   LANGUAGE_BUTTON*  pNewButton;

   /// Modify button within text selection
   if (modifyButtonInRichEditByPosition(pDocument->hRichEdit, LOWORD(Edit_GetSel(pDocument->hRichEdit)), eColour, pNewButton))
      // [SUCCESS] Store data
      insertObjectIntoAVLTree(pDocument->pButtonsByID, (LPARAM)pNewButton);
}

/// Function name  : onRichTextDialog_Command
// Description     : RichText dialog WM_COMMAND processing
// 
// LANGUAGE_DOCUMENT*  pDocument       : [in] Language document hosting the RichText dialog
// HWND                hDialog         : [in] RichText dialog window handle
// CONST UINT          iControlID      : [in] ID of the control/menu item/accelerator
// CONST UINT          iNotification   : [in] Notification code
// 
// Return Value   : TRUE if processed, FALSE otherwise
// 
BOOL  onRichTextDialog_Command(LANGUAGE_DOCUMENT*  pDocument, HWND  hDialog, CONST UINT  iControlID, CONST UINT  iNotification)
{
   BOOL  bResult = FALSE;

   TRACK_FUNCTION();
   switch (iControlID)
   {
   /// [COLOUR POPUP MENU]
   case IDM_COLOUR_BLACK:
   case IDM_COLOUR_BLUE:
   case IDM_COLOUR_CYAN:
   case IDM_COLOUR_DEFAULT:
   case IDM_COLOUR_GREEN:
   case IDM_COLOUR_ORANGE:
   case IDM_COLOUR_PURPLE:
   case IDM_COLOUR_RED:
   case IDM_COLOUR_WHITE:
   case IDM_COLOUR_YELLOW:
      // [BUTTON] Change button text colour
      if (SendMessage(pDocument->hRichEdit, EM_SELECTIONTYPE, NULL, NULL) == SEL_OBJECT)
      {  
         onRichTextDialog_FormatButton(pDocument, identifyTextColourFromCommandID(iControlID));
         break;
      }
      // Fall through...

   /// [ALIGNMENT / FORMATTING]
   case IDM_RICHEDIT_LEFT:
   case IDM_RICHEDIT_CENTRE:
   case IDM_RICHEDIT_RIGHT:
   case IDM_RICHEDIT_JUSTIFY:
   case IDM_RICHEDIT_BOLD:
   case IDM_RICHEDIT_ITALIC:
   case IDM_RICHEDIT_UNDERLINE:
   /// [CLIPBOARD]
   case IDM_EDIT_CUT:    
   case IDM_EDIT_COPY:   
   case IDM_EDIT_PASTE:  
   case IDM_EDIT_DELETE: 
      bResult = performRichEditFormatCommand(pDocument, hDialog, iControlID);
      break;

   /// [COLOUR] - Display colouring menu
   case IDM_RICHEDIT_COLOUR:
      bResult = onRichTextDialog_ContextMenu(pDocument, hDialog, IDM_COLOUR_POPUP);
      break;

   /// [INSERT BUTTON] - Create button bitmap and Insert as OLE object
   case IDM_RICHEDIT_BUTTON:
      bResult = onRichTextDialog_InsertButton(pDocument);
      break;

   // [EDIT NOTIFICATIONS]
   case IDC_LANGUAGE_EDIT:
      switch (iNotification)
      {
      /// [TEXT CHANGED] Notify document + properties
      case EN_CHANGE:
         sendDocumentPropertyUpdated(AW_DOCUMENTS_CTRL, iControlID);
         sendDocumentUpdated(AW_PROPERTIES); 
         // Fall through..

      /// [TEXT/FOCUS CHANGED] Update toolbar
      case EN_SETFOCUS:
      case EN_KILLFOCUS:
         updateRichTextDialogToolBar(pDocument, hDialog);
         break;
      }
      break;
   }

   END_TRACKING();
   return bResult;
}


/// Function name  : onRichTextDialog_ContextMenu
// Description     : Display the colour/edit popup menu
//
// HWND        hDialog     : [in] Window handle of the parent window for the popup menu
// CONST UINT  iMenuIndex  : [in] Sub-menu index of the desired popup menu  : IDM_RICHEDIT_POPUP  or  IDM_COLOUR_POPUP
// 
// Return Value   : TRUE
// 
BOOL  onRichTextDialog_ContextMenu(LANGUAGE_DOCUMENT*  pDocument, HWND  hDialog, CONST UINT  iMenuIndex)
{
   CUSTOM_MENU*  pCustomMenu;    // Custom Popup menu
   POINT         ptCursor;       // Cursor position

   // Create desired custom popup menu
   pCustomMenu = createCustomMenu(TEXT("LANGUAGE_MENU"), TRUE, iMenuIndex);

   /// Display context menu
   GetCursorPos(&ptCursor);
   TrackPopupMenu(pCustomMenu->hPopup, TPM_RIGHTALIGN WITH TPM_TOPALIGN WITH TPM_LEFTBUTTON, ptCursor.x, ptCursor.y, NULL, hDialog, NULL);

   // Cleanup and return
   deleteCustomMenu(pCustomMenu);
   return TRUE;
}


/// Function name  : onRichTextDialog_Destroy
// Description     : Cleanup the RichText dialog controls and RichEdit OLE interface
// 
// LANGUAGE_DOCUMENT*  pDocument  : [in] Language document hosting the RichText dialog
// HWND                hDialog    : [in] RichText dialog window handle
// 
// Return Value   : TRUE
// 
BOOL  onRichTextDialog_Destroy(LANGUAGE_DOCUMENT*  pDocument, HWND  hDialog)
{
   IRichEditOle*  pRichEdit;  // RichEdit control OLE interface
   REOBJECT       oImage;     // RichEdit control OLE object attributes

   TRACK_FUNCTION();

   /// Destroy ToolBar + it's child combo
   if (pDocument->hToolBar)
      utilDeleteWindow(pDocument->hToolBar);
   
   // Get RichEdit OLE interface
   RichEdit_GetOLEInterface(pDocument->hRichEdit, &pRichEdit);

   /// Destroy any OLE objects within the document (Ripped from MSDN)
   for (INT iIndex = pRichEdit->GetObjectCount() - 1; iIndex >= 0; iIndex--)
   {
      // Prepare
      utilZeroObject(&oImage, REOBJECT);
      oImage.cbStruct = sizeof(REOBJECT);

      // Lookup object
      if (pRichEdit->GetObject(iIndex, &oImage, REO_GETOBJ_POLEOBJ) == S_OK)
      {
         // Deactivate if necessary
         if (oImage.dwFlags INCLUDES REO_INPLACEACTIVE)
            pRichEdit->InPlaceDeactivate();

         // Close
         if (oImage.dwFlags INCLUDES REO_OPEN)
            oImage.poleobj->Close(OLECLOSE_NOSAVE);

         // Cleanup
         utilReleaseInterface(oImage.poleobj);
      }
   }
   
   /// Destroy any remaining button data
   deleteAVLTree(pDocument->pButtonsByID);

   // Release interfaces
   utilReleaseInterface(pRichEdit);
   utilReleaseInterface(pDocument->pOleCallback);
   END_TRACKING();
   return TRUE;
}


/// Function name  : onRichTextDialog_DestroyButton
// Description     : Called when a Languagebutton is being destroyed by the RichEdit
// 
// LANGUAGE_DOCUMENT*  pDocument : [in] Document
// IOleObject*         pObject   : [in] Button being destroyed
// 
// Return Value   : 
// 
BOOL  onRichTextDialog_DestroyButton(LANGUAGE_DOCUMENT*  pDocument, IOleObject*  pObject)
{
   LANGUAGE_BUTTON*  pButton;

   // Lookup + destroy associated button data
   if (findLanguageButtonByObject(pDocument->pButtonsByID, pObject, pButton))
      destroyObjectInAVLTreeByValue(pDocument->pButtonsByID, (LPARAM)pButton->szID, NULL);

   return TRUE;
}

/// Function name  : onRichTextDialog_GetMenuItemState
// Description     : Determines whether a toolbar/menu command relating to the document should be enabled or disabled
// 
// LANGUAGE_DOCUMENT*  pDocument   : [in]     Document
// CONST UINT          iCommandID  : [in]     Menu/toolbar Command
// UINT*               piState     : [in/out] Combination of MF_ENABLED, MF_DISABLED, MF_CHECKED, MF_UNCHECKED
// 
BOOL   onRichTextDialog_GetMenuItemState(LANGUAGE_DOCUMENT*  pDocument, CONST UINT  iCommandID, UINT*  piState)
{
   UINT  iSelection;

   // [TRACKING]
   TRACK_FUNCTION();

   // Examine command
   switch (iCommandID)
   {
   /// [UNDO/REDO] Query RichEdit
   case IDM_EDIT_REDO:  *piState = (RichEdit_CanRedo(pDocument->hRichEdit) ? MF_ENABLED : MF_DISABLED);   break;
   case IDM_EDIT_UNDO:  *piState = (Edit_CanUndo(pDocument->hRichEdit)     ? MF_ENABLED : MF_DISABLED);   break;  

   /// [CUT/COPY/DELETE] Requires a text selection
   case IDM_EDIT_CUT:
   case IDM_EDIT_COPY:
   case IDM_EDIT_DELETE:
      iSelection = Edit_GetSel(pDocument->hRichEdit);
      *piState   = (LOWORD(iSelection) == HIWORD(iSelection) ? MF_ENABLED : MF_DISABLED);
      break;

   /// [PASTE/SELECT ALL] Always enabled
   case IDM_EDIT_PASTE:
   case IDM_EDIT_SELECT_ALL:
      *piState = MF_ENABLED;
      break;

   // [COMMENT] Unsupported
   default:
      *piState = MF_DISABLED;
      break;
   }

   END_TRACKING();
   return TRUE;
}

/// Function name  : onRichTextDialog_InsertButton
// Description     : Inserts a new OLE Button Picture 
// 
// LANGUAGE_DOCUMENT*  pDocument : [in] Lanaguage document hosting the RichText dialog
// 
// Return Value   : TRUE if successful, FALSE if not
// 
BOOL  onRichTextDialog_InsertButton(LANGUAGE_DOCUMENT*  pDocument)
{
   LANGUAGE_BUTTON*  pButton;

   // [CHECK] Ensure button ID is unique
   if (!validateLanguageButtonID(pDocument, TEXT("NewID")))
      return FALSE;

   // [CHECK] Attempt to insert new button + store data
   if (pButton = insertRichEditButton(pDocument->hRichEdit, TEXT("NewID"), TEXT("New Button"), GTC_DEFAULT))
      insertObjectIntoAVLTree(pDocument->pButtonsByID, (LPARAM)pButton);

   // Return TRUE if successful
   return pButton != NULL;
}


/// Function name  : onRichTextDialog_Notify
// Description     : WM_NOTIFY processing
// 
// LANGUAGE_DOCUMENT*  pDocument : [in] Lanaguage document hosting the RichText dialog
// HWND                hDialog   : [in] RichText dialog window handle
// NMHDR*              pMessage  : [in] WM_NOTIFY message data
// 
// Return Value   : TRUE if processed, FALSE otherwise
// 
BOOL   onRichTextDialog_Notify(LANGUAGE_DOCUMENT*  pDocument, HWND  hDialog, NMHDR*  pMessage)
{
   BOOL  bResult = FALSE;

   TRACK_FUNCTION();
   switch (pMessage->code)
   {
   // [TOOLBAR REQUESTING TOOLTIP]
   case TTN_GETDISPINFO:
      bResult = onTooltipRequestText((NMTTDISPINFO*)pMessage);
      break;

   // [EDIT SELECTION CHANGED/CARET MOVED]
   case EN_SELCHANGE:
      bResult = updateRichTextDialogToolBar(pDocument, hDialog);
      break;
   }

   END_TRACKING();
   return bResult;
}


/// Function name  : onMainWindowPaint
// Description     : Paint the area next to the toolbar
// 
// MAIN_WINDOW_DATA*  pWindowData : [in] Window data
// PAINTSTRUCT*       pPaintInfo  : [in] Paint data
// 
VOID    onRichTextDialog_Paint(LANGUAGE_DOCUMENT*  pDocument, PAINTSTRUCT  *pPaintInfo)
{
   RECT   rcClient;    // MainWindow client rectangle

   // Prepare
   GetClientRect(pDocument->hRichTextDialog, &rcClient);

   /// Draw sunken edge along the top
   //DrawEdge(pPaintInfo->hdc, &rcClient, BDR_SUNKENOUTER, BF_TOP WITH BF_ADJUST);
   
   /// Colour remainder of the window
   utilFillSysColourRect(pPaintInfo->hdc, &rcClient, COLOR_BTNFACE);
}


/// Function name  : onRichTextDialog_Resize
// Description     : Stretches the RichEdit control across the non-toolbar parts of the client area
// 
// LANGUAGE_DOCUMENT*  pDocument : [in] Language document hosting the RichText dialog
// HWND                hDialog   : [in] RichText dialog window handle
// CONST UINT          iWidth    : [in] New dialog width, in pixels
// CONST UINT          iHeight   : [in] New dialog height, in pixels
// 
/// Return Value   : TRUE
// 
BOOL  onRichTextDialog_Resize(LANGUAGE_DOCUMENT*  pDocument, HWND  hDialog, CONST UINT  iWidth, CONST UINT  iHeight)
{
   RECT  rcClient,      // Dialog client rectangle
         rcToolBar;     // Toolbar client rectangle
   SIZE  siClient,      // Size of client rectangle
         siToolBar;     // Size of toolbar rectangle

   // [CHECK] Ignore call made during dialog creation
   if (pDocument == NULL)
      return FALSE;

   /// Resize Toolbar to window's width
   SendMessage(pDocument->hToolBar, WM_SIZE, NULL, MAKE_LONG(iWidth, iHeight));

   // Get toolbar rectangle
   GetClientRect(pDocument->hToolBar, &rcToolBar);
   utilConvertRectangleToSize(&rcToolBar, &siToolBar);

   // Get client rectangle
   GetClientRect(hDialog, &rcClient);
   rcClient.top += siToolBar.cy;
   utilConvertRectangleToSize(&rcClient, &siClient);

   /// Stretch Edit control across non-toolbar client area
   MoveWindow(pDocument->hRichEdit, rcClient.left, rcClient.top, siClient.cx, siClient.cy, TRUE);
   
   // Re-format the RichEdit control's text
   //updateRichEditFormatting(pDocument, hDialog);

   return TRUE;
}


/// Function name  : dlgprocRichTextDialog
// Description     : RichText dialog's window procedure
// 
// 
INT_PTR CALLBACK  dlgprocRichTextDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   LANGUAGE_DOCUMENT*  pDocument;   // Language document data
   ERROR_STACK*        pException;
   PAINTSTRUCT         oPaintData;
   BOOL                bResult = FALSE;

   // Get window data
   TRACK_FUNCTION();
   pDocument = (LANGUAGE_DOCUMENT*)GetWindowLong(hDialog, DWL_USER);

   /// [GUARD BLOCK]
   __try
   {
      switch (iMessage)
      {
      /// [CREATION]
      case WM_INITDIALOG:
         bResult = initRichTextDialog((LANGUAGE_DOCUMENT*)lParam, hDialog);
         break;

      /// [DESTRUCTION]
      case WM_DESTROY:
         bResult = onRichTextDialog_Destroy(pDocument, hDialog);
         break;

      /// [COMMANDS]
      case WM_COMMAND:
         bResult = onRichTextDialog_Command(pDocument, hDialog, LOWORD(wParam), HIWORD(wParam));
         break;

      /// [NOTIFICATION]
      case WM_NOTIFY:
         bResult = onRichTextDialog_Notify(pDocument, hDialog, (NMHDR*)lParam);
         break;

      /// [CONTEXT MENU] - Display RichEdit popup
      case WM_CONTEXTMENU:
         if (pDocument->hRichEdit == (HWND)wParam)
            bResult = onRichTextDialog_ContextMenu(pDocument, hDialog, IDM_RICHEDIT_POPUP);
         break;

      /// [MENU HOVER] -- Display description in status bar
      case WM_MENUSELECT:
         bResult = utilReflectMessage(hDialog, iMessage, wParam, lParam);
         break;

      /// [MENU ITEM STATE]
      case UM_GET_MENU_ITEM_STATE:
         bResult = onRichTextDialog_GetMenuItemState(pDocument, wParam, (UINT*)lParam);
         break;

      /// [CUSTOM MENU] -- Draw custom menu
      case WM_DRAWITEM:    onWindow_DrawItem((DRAWITEMSTRUCT*)lParam);                  break;
      case WM_MEASUREITEM: onWindow_MeasureItem(hDialog, (MEASUREITEMSTRUCT*)lParam);   break;

      /// [PAINT]
      case WM_PAINT:    
         BeginPaint(hDialog, &oPaintData);
         onRichTextDialog_Paint(pDocument, &oPaintData); 
         EndPaint(hDialog, &oPaintData);
         break;

      /// [RESIZING]
      case WM_SIZE:
         bResult = onRichTextDialog_Resize(pDocument, hDialog, LOWORD(lParam), HIWORD(lParam));
         break;

      /// [VISUAL STYLES]
      case WM_CTLCOLORDLG:
      case WM_CTLCOLORSTATIC:
         bResult = (BOOL)onDialog_ControlColour((HDC)wParam, IsThemeActive() ? COLOR_WINDOW : COLOR_BTNFACE);
         break;

      // [OBJECT DESTROYED]
      case UN_OBJECT_DESTROYED:
         bResult = onRichTextDialog_DestroyButton(pDocument, (IOleObject*)lParam);
         break;
      }

      // [FOCUS HANDLER]
      updateMainWindowToolBar(iMessage, wParam, lParam);
   }
   /// [EXCEPTION HANDLER]
   __except (generateExceptionError(GetExceptionInformation(), pException))
   {
      // [ERROR] "An unidentified and unexpected critical error has occurred in the rich edit dialog"
      enhanceError(pException, ERROR_ID(IDS_EXCEPTION_RICHTEXT_DIALOG));
      displayException(pException);
   }

   END_TRACKING();
   return (INT_PTR)bResult;
}

