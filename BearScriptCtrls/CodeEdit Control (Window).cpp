//
// CodeEdit (new).cpp : Attempt to re-write the devilishly beloved CodeEdit control
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

///  TODO LIST:    Steal the NC painting function for the box between the scroll bars. Maybe draw a X3TC icon.
///            

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS/GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////

#define     UNDO_COMMIT_TICKER        2

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : getCodeEditData
// Description     : Retrieves the window data previously associated with a CodeEdit control
// 
// HWND  hCodeEdit : [in] Window handle of a CodeEdit control
// 
// Return Value   : CodeEdit window data or NULL if none is found
// 
CODE_EDIT_DATA*   getCodeEditData(HWND  hCodeEdit)
{
   // Retrieve from first four bytes of window handle storage
   return (CODE_EDIT_DATA*)GetWindowLong(hCodeEdit, 0);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onCodeEditCharacterPress
// Description     : Processes the insertion of text into the CodeEdit
// 
// CODE_EDIT_DATA*  pWindowData : [in] Window data
// CONST TCHAR      chCharacter : [in] Character to insert
// 
VOID  onCodeEditCharacterPress(CODE_EDIT_DATA*  pWindowData, CONST TCHAR  chCharacter)
{
   BOOL   bControlPressed;    // Whether CTRL was pressed

   // Prepare
   TRACK_FUNCTION();
   bControlPressed = HIBYTE(GetKeyState(VK_CONTROL));

   /// [CHECK] Allow only letters, numbers and operators
   if (chCharacter == VK_RETURN OR chCharacter == VK_TAB OR (chCharacter >= 32 AND chCharacter <= 126))
   {
      // [TOOLTIP] Destroy Tooltip
      if (pWindowData->bTooltipVisible) 
         displayCodeEditTooltip(pWindowData, FALSE, NULL, NULL);

      // [PRE-PROCESS] Determine whether SuggestionList should process this instead
      if (!onCodeEditSuggestionKeyDown(pWindowData, chCharacter, FALSE, FALSE))
      {
         /// [SELECTION] Delete selection text
         if (hasCodeEditSelection(pWindowData))
            removeCodeEditSelectionText(pWindowData, TRUE);

         /// Insert character at the caret
         insertCodeEditCharacterAtCaret(pWindowData, chCharacter);

         // Ensure location is visible
         ensureCodeEditLocationIsVisible(pWindowData, &pWindowData->oCaret.oLocation);

         // [POST-PROCESS] Inform SuggestionList
         onCodeEditSuggestionKeyDown(pWindowData, chCharacter, FALSE, TRUE);
      }
   }

   // [TRACK]
   END_TRACKING();
}


/// Function name  : onCodeEditCommand
// Description     : Dispatches menu item commands
//
// CODE_EDIT_DATA*  pWindowData  : [in] Window data
// CONST UINT       iCommandID   : [in] ID of the Menu/Accelerator item sending the message
// CONST UINT       iSource      : [in] Whether menu or accelerator sent the message
// HWND             hCtrl        : [in] Handle of the child window sending the notification
// 
// Return type : TRUE if processed, FALSE otherwise
//
BOOL   onCodeEditCommand(CODE_EDIT_DATA*  pWindowData, CONST UINT  iCommandID, CONST UINT  iSource, HWND  hCtrl)
{
   /*switch (iCommandID)
   {
   /// [VIEW SUGGESTIONS]
   case IDM_CODE_EDIT_VIEW_SUGGESTIONS:
      updateCodeEditSuggestionList(pWindowData);
      return TRUE;
   }*/

   // [UNHANDLED] Return FALSE
   return FALSE;
}


/// Function name  : onCodeEditCreate
// Description     : Creates window data for a CodeEdit control and associates with the window handle
// 
// HWND  hWnd   : [in] Window handle of the control
// 
VOID  onCodeEditCreate(HWND  hWnd)
{
   CODE_EDIT_DATA*  pWindowData;

   // [TRACK]
   TRACK_FUNCTION();

   // Create window data
   pWindowData = createCodeEditData(hWnd);

   // Store in first four bytes of window handle storage
   SetWindowLong(hWnd, 0, (LONG)pWindowData);

   /// NEW: Create tooltip
   pWindowData->hTooltip = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL, WS_POPUP WITH TTS_ALWAYSTIP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hWnd, NULL, getAppInstance(), NULL);
   ERROR_CHECK("creating CodeEdit tooltip", pWindowData->hTooltip);     //  WITH TTF_SUBCLASS  WITH TTF_TRANSPARENT

   //if (pWindowData->hTooltip)
   //{
   //   //SetWindowPos(pWindowData->hTooltip, HWND_TOPMOST,0, 0, 0, 0, SWP_NOMOVE WITH SWP_NOSIZE WITH SWP_NOACTIVATE);     //
   //   SendMessage(pWindowData->hTooltip, TTM_SETDELAYTIME, TTDT_AUTOPOP, 30 * 1000);
   //   SendMessage(pWindowData->hTooltip, TTM_SETDELAYTIME, TTDT_INITIAL, 5000);
   //}

   // Disable updates
   SetWindowRedraw(hWnd, FALSE);

   // [TRACK]
   END_TRACKING();
}



/// Function name  : onCodeEditDestroy
// Description     : Disconnects and destroys CodeEdit window data from it's window handle
// 
// CODE_EDIT_DATA*  pWindowData : [in] Window data to destroy
// 
VOID  onCodeEditDestroy(CODE_EDIT_DATA*  pWindowData)
{
   // [TRACK]
   TRACK_FUNCTION();

   /// Destroy Tooltip
   utilDeleteWindow(pWindowData->hTooltip);

   // Disconnect from window handle storage
   SetWindowLong(pWindowData->hWnd, 0, NULL);

   // Destroy window data
   deleteCodeEditData(pWindowData);

   // [TRACK]
   END_TRACKING();
}


/// Function name  : onCodeEditKeyDown
// Description     : Implements the keyboard interface for the various directional keys and the DELETE/BACKSPACE keys
// 
// CODE_EDIT_DATA*  pWindowData   : [in] Window data
// CONST UINT        iVirtualKey   : [in] Virtual Key Code of the key being pressed
// 
VOID  onCodeEditKeyDown(CODE_EDIT_DATA*  pWindowData, CONST UINT  iVirtualKey)
{
   CODE_EDIT_LOCATION  oOldCaretLocation,    // Existing caret location
                       oNewCaretLocation;    // New caret location
   POINT               ptCaretPosition;      // Caret position in client co-ordinates
   BOOL                bExtendSelection,     // Whether SHIFT is pressed
                       bSpecialAction;       // Whether CONTROL is pressed

   // [TRACK]
   TRACK_FUNCTION();

   // [SHIFT PRESSED] Extend/Create a text selection with this action
   bExtendSelection = HIBYTE(GetKeyState(VK_SHIFT));
   bSpecialAction   = HIBYTE(GetKeyState(VK_CONTROL));

   // [PRE-PROCESS] Determine whether SuggestionList should process this instead
   if (!onCodeEditSuggestionKeyDown(pWindowData, iVirtualKey, TRUE, FALSE))
   {
      // [TOOLTIP] Destroy Tooltip
      if (pWindowData->bTooltipVisible) 
         displayCodeEditTooltip(pWindowData, FALSE, NULL, NULL);

      // [CHECK] Create new selection if SHIFT is pressed
      if (bExtendSelection AND !hasCodeEditSelection(pWindowData))
      {
         pWindowData->oSelection.oOrigin = pWindowData->oCaret.oLocation;
         pWindowData->oSelection.bExists = TRUE;
      }

      // Examine key code
      switch (iVirtualKey)
      {
      /// [DIRECTION KEY] Perform one of several different actions depending on whether CONTROL is pressed
      case VK_UP:
      case VK_DOWN:
      case VK_LEFT:
      case VK_RIGHT:
         // [CONTROL] Perform corresponding special action
         if (bSpecialAction)
         {
            switch (iVirtualKey)
            {
            // [CONTROL + LEFT/RIGHT] Move the next/previous word
            case VK_LEFT:     performCodeEditCaretCommand(pWindowData, IDM_EDIT_WORD_LEFT,  bExtendSelection);    break;
            case VK_RIGHT:    performCodeEditCaretCommand(pWindowData, IDM_EDIT_WORD_RIGHT, bExtendSelection);    break;
            // [CONTROL + UP/DOWN] Scroll up or down a line
            case VK_UP:       onCodeEditScroll(pWindowData, SB_LINEUP,   NULL, SB_VERT);                         break;
            case VK_DOWN:     onCodeEditScroll(pWindowData, SB_LINEDOWN, NULL, SB_VERT);                         break;
            }
         }
         // [NORMAL] Move caret in specified direction
         else
            performCodeEditCaretCommand(pWindowData, iVirtualKey, bExtendSelection);

         // Ensure location is visible
         ensureCodeEditLocationIsVisible(pWindowData, &pWindowData->oCaret.oLocation);
         break;

      /// [PAGE UP/DOWN] Scroll an entire page and attempt to move the caret to an equivilent position in the new page
      case VK_NEXT:
      case VK_PRIOR:
         // [CONTROL] Return without doing anything
         if (bSpecialAction)
            break;

         // [NO SHIFT] Cancel current selection
         if (!bExtendSelection)
            removeCodeEditSelection(pWindowData);

         // Calculate caret position in client co-ordinates
         calculateCodeEditPointFromLocation(pWindowData, &pWindowData->oCaret.oLocation, &ptCaretPosition);

         // Scroll a page
         onCodeEditScroll(pWindowData, (iVirtualKey == VK_NEXT ? SB_PAGEDOWN : SB_PAGEUP), NULL, SB_VERT);

         // Calculate equivilent caret position. If none exists then use the end of the text
         if (!calculateCodeEditLocationFromPoint(pWindowData, &ptCaretPosition, &oNewCaretLocation))
            calculateCodeEditFinalCharacterLocation(pWindowData, &oNewCaretLocation);

         // Reposition caret
         oOldCaretLocation = pWindowData->oCaret.oLocation;
         setCodeEditCaretLocation(pWindowData, &oNewCaretLocation);

         // [SHIFT] Update selection
         if (bExtendSelection)
            updateCodeEditSelection(pWindowData, oOldCaretLocation);
         break;

      /// [HOME/END] Performs different actions depending on whether SHIFT and/or CONTROL are pressed
      case VK_HOME:
      case VK_END:
         // Prepare
         oNewCaretLocation = pWindowData->oCaret.oLocation;

         // [NO SHIFT] Cancel current selection
         if (!bExtendSelection)
            removeCodeEditSelection(pWindowData);

         // [CONTROL] Scroll to the first or last page and move the caret to the first or last character in the edit
         if (bSpecialAction)
         {
            // Scroll to the first or last page
            onCodeEditScroll(pWindowData, (iVirtualKey == VK_HOME ? SB_TOP : SB_BOTTOM), NULL, SB_VERT);

            // [HOME] Move caret to the first character in the edit
            if (iVirtualKey == VK_HOME)
               oNewCaretLocation.iLine = oNewCaretLocation.iIndex = 0;
            // [END] Move the caret to the last character in the edit
            else
               calculateCodeEditFinalCharacterLocation(pWindowData, &oNewCaretLocation);
         }
         // [HOME] Move the caret to the first character on the line
         else if (iVirtualKey == VK_HOME)
            oNewCaretLocation.iIndex = 0;
         // [END] Move the caret to the last character on the line
         else
            oNewCaretLocation.iIndex = getCodeEditLineLengthByIndex(pWindowData, oNewCaretLocation.iLine);
            
         // Reposition caret
         oOldCaretLocation = pWindowData->oCaret.oLocation;
         setCodeEditCaretLocation(pWindowData, &oNewCaretLocation);      

         // [SHIFT] Update selection
         if (bExtendSelection)
            updateCodeEditSelection(pWindowData, oOldCaretLocation);

         // Ensure location is visible
         ensureCodeEditLocationIsVisible(pWindowData, &pWindowData->oCaret.oLocation);
         break;

      /// [DELETE/BACKSPACE] Delete the current selection or the next/prev character
      case VK_BACK:
      case VK_DELETE:
         // [SELECTION] Delete selection text
         if (hasCodeEditSelection(pWindowData))
            removeCodeEditSelectionText(pWindowData, TRUE);
         // [NO SELECTION] Delete next/prev character
         else
            removeCodeEditCharacterAtCaret(pWindowData, iVirtualKey);
         break;
      }

      // [POST-PROCESS] Inform the SuggestionList a key has been processed
      onCodeEditSuggestionKeyDown(pWindowData, iVirtualKey, TRUE, TRUE);
   }

   // [TRACK]
   END_TRACKING();
}



/// Function name  : onCodeEditInterfaceMessage
// Description     : Handles a series of simple messages for interacting with the CodeEdit
// 
// CODE_EDIT_DATA*  pWindowData : [in] Window data
// CONST UINT        iMessage    : [in] Message to handle
// 
// Return Value   : Depends on the message, but NULL indicates failure or that the message was unhandled
// 
LRESULT  onCodeEditInterfaceMessage(CODE_EDIT_DATA*  pWindowData, CONST UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   CODE_EDIT_LOCATION*  pLocation;
   CODE_EDIT_LINE*  pLineData;         // LineData for the line being examined
   LRESULT          xResult;           // Operation result
   UINT             iLineNumber;       // Convenience description for an input parameter
   
   // Prepare
   TRACK_FUNCTION();
   xResult = NULL;

   // Examine message
   switch (iMessage)
   {
   /// Message: UM_GET_CARET_LOCATION
   //
   // lParam : CODE_EDIT_LOCATION* : [in/out] CodeEdit Caret location
   //
   case UM_GET_CARET_LOCATION:
      // Prepare
      pLocation = (CODE_EDIT_LOCATION*)lParam;

      // Copy current caret location and return TRUE
      (*pLocation) = pWindowData->oCaret.oLocation;
      xResult      = TRUE;
      break;

   /// Message: UM_GET_LINE_TEXT
   //
   // wParam : UINT : [in] Zero based line number 
   //
   case UM_GET_LINE_TEXT:
      // Prepare
      iLineNumber = (UINT)wParam;

      // Lookup line data
      if (findCodeEditLineDataByIndex(pWindowData, iLineNumber, pLineData))
         // Generate output text
         xResult = (LRESULT)generateCodeEditLineText(pLineData);
      break;

   /// Message: EM_GETLINECOUNT
   //
   case EM_GETLINECOUNT:
      // Return line count
      xResult = (LRESULT)getCodeEditLineCount(pWindowData);
      break;

   /// Message: EM_LINELENGTH
   //
   // wParam : UINT : [in] Zero based line number 
   //
   case EM_LINELENGTH:
      // Prepare
      iLineNumber = (UINT)wParam;

      // Lookup line data
      if (findCodeEditLineDataByIndex(pWindowData, iLineNumber, pLineData))
         // Return line text
         xResult = (LRESULT)getCodeEditLineLength(pLineData);
      break;

   /// Message: EM_SETSEL
   //
   // wParam : UINT : [in] Zero based start index
   // lParam : UINT : [in] Zero based end index
   //
   case EM_SETSEL:
      // [SELECT ALL] Invoke manually
      if (wParam == 0 AND lParam == -1)
         onCodeEditSelectAll(pWindowData);
      break;


   /// Message: EM_CANUNDO and EM_CANREDO
   case EM_CANUNDO:  xResult = (getStackItemCount(pWindowData->oUndo.pUndoStack) > 0 ? TRUE : FALSE); break;
   case EM_CANREDO:  xResult = (getStackItemCount(pWindowData->oUndo.pRedoStack) > 0 ? TRUE : FALSE); break;

   // [UNSUPPORTED] Return NULL
   default:
      break;
   }

   // Return result
   END_TRACKING();
   return xResult;
}


/// Function name  : onCodeEditLButtonDoubleClick
// Description     : Selects the word at the caret
// 
// CODE_EDIT_DATA*  pWindowData : [in] Window data
// CONST POINT*      ptClick     : [in] Client co-ordinates of the click
// CONST UINT        iButtons    : [in] Button flags
// 
VOID  onCodeEditLButtonDoubleClick(CODE_EDIT_DATA*  pWindowData, CONST POINT*  ptClick, CONST UINT  iButtons)
{
   CODE_EDIT_LOCATION   oWordStart,    // Location defining the start of the word
                        oWordEnd;      // Location defining the end of the word
   CODE_EDIT_LINE*      pLineData;     // LineData for the line beneath the click (at the caret)
   
   // Prepare
   TRACK_FUNCTION();
   oWordStart = oWordEnd = pWindowData->oCaret.oLocation;

   // Lookup line data
   if (findCodeEditLineDataByIndex(pWindowData, pWindowData->oCaret.oLocation.iLine, pLineData))
   {
      /// Cancel current selection
      removeCodeEditSelection(pWindowData);

      // Determine the location of the start+end of the word beneath the caret
      oWordStart.iIndex = findPrevCodeEditWord(pLineData, pWindowData->oCaret.oLocation.iIndex);
      oWordEnd.iIndex   = findNextCodeEditWord(pLineData, pWindowData->oCaret.oLocation.iIndex);

      /// Define the origin as the start, and move the caret to the end
      pWindowData->oSelection.oOrigin = oWordStart;
      setCodeEditCaretLocation(pWindowData, &oWordEnd);

      /// Display selection
      updateCodeEditSelection(pWindowData, oWordStart);
   }

   // [TRACK]
   END_TRACKING();
}


/// Function name  : onCodeEditLButtonDown
// Description     : Moves the caret to the cursor and extends the current selection if SHIFT is pressed
// 
// CODE_EDIT_DATA*  pWindowData : [in] Window data
// CONST POINT*      ptClick     : [in] Client co-ordinates of the click
// CONST UINT        iButtons    : [in] Button flags
// 
VOID  onCodeEditLButtonDown(CODE_EDIT_DATA*  pWindowData, CONST POINT*  ptClick, CONST UINT  iButtons)
{
   CODE_EDIT_LOCATION  ptClickLocation,         // Location of the input click
                       oOldCaretLocation;       // Original location of the caret
   BOOL                bExtendSelection;        // Whether the SHIFT key is pressed or not

   // Prepare
   TRACK_FUNCTION();
   bExtendSelection = (iButtons INCLUDES MK_SHIFT);

   // [SUGGESTIONS] Hide suggestions
   destroyCodeEditSuggestionList(pWindowData);

   /// Set focus to the CodeEdit and capture the mouse
   SetFocus(pWindowData->hWnd);
   SetCapture(pWindowData->hWnd);

   // Convert click into a valid location, if possible
   if (!calculateCodeEditLocationFromPoint(pWindowData, ptClick, &ptClickLocation))
      // [CLICK BENEATH TEXT] Use the final character
      calculateCodeEditFinalCharacterLocation(pWindowData, &ptClickLocation);
   
   /// [NEW SELECTION] Cancel current selection
   if (!bExtendSelection)
      removeCodeEditSelection(pWindowData);

   // Save caret location
   oOldCaretLocation = pWindowData->oCaret.oLocation;

   /// Move the caret to the cursor
   setCodeEditCaretLocation(pWindowData, &ptClickLocation);

   /// [NEW SELECTION] Define a new selection origin
   if (!bExtendSelection)
      pWindowData->oSelection.oOrigin = ptClickLocation;

   /// [EXTEND SELECTION] Update selection to reflect new caret position
   else
      updateCodeEditSelection(pWindowData, oOldCaretLocation);
   
   // [TRACK]
   END_TRACKING();
}


/// Function name  : onCodeEditLButtonDown
// Description     : Releases the mouse capture
// 
// CODE_EDIT_DATA*  pWindowData : [in] Window data
// CONST POINT*     ptClick     : [in] Client co-ordinates of the click
// CONST UINT       iButtons    : [in] Button flags
// 
VOID  onCodeEditLButtonUp(CODE_EDIT_DATA*  pWindowData, CONST POINT*  ptClick, CONST UINT  iButtons)
{
   /// Release mouse
   if (GetCapture() == pWindowData->hWnd)
      ReleaseCapture();
}


/// Function name  : onCodeEditMouseHover
// Description     : Displays a tooltip at the co-ordinates
// 
// CODE_EDIT_DATA*  pWindowData : [in] Window data
// CONST POINT*      ptCursor    : [in] Location of the mouse in client co-ordinates
// CONST UINT        iButtons    : [in] Button flags indicating which keys and mouse buttons were presesd
// 
VOID  onCodeEditMouseHover(CODE_EDIT_DATA*  pWindowData, CONST POINT*  ptCursor, CONST UINT  iButtons)
{
   CODE_EDIT_LOCATION  oHoverLocation;            // Location of the cursor
   SUGGESTION_TYPE     eObjectType;
   CODE_EDIT_LINE*     pLineData;
   CODEOBJECT*         pCodeObject;
   TCHAR*              szLineText;

   // Prepare
   TRACK_FUNCTION();
   eObjectType = CST_NONE;

   //VERBOSE("CODE-EDIT: WM_MOUSEHOVER received");

   // Cancel hover request
   utilTrackMouseEvent(pWindowData->hWnd, TME_HOVER WITH TME_CANCEL, NULL);

   /// [CHECK] Have we hovered over code?
   if (performCodeEditHitTest(pWindowData, ptCursor, &oHoverLocation) == CDR_LINE_TEXT)
   {
      // [SUCCESS] Lookup line data
      findCodeEditLineDataByIndex(pWindowData, oHoverLocation.iLine, pLineData);

      /// Generate a CodeObject from the line text
      szLineText  = generateCodeEditLineText(pLineData);
      pCodeObject = createCodeObject(szLineText);

      /// [CHECK] Determine the CodeObject preceeding the cursor (if possible)
      if (szLineText[0] AND findCodeObjectByCharacterIndex(pCodeObject, oHoverLocation.iIndex ? oHoverLocation.iIndex - 1 : 0))
      {
         // [FOUND] Determine suggestion type from CodeObject
         switch (pCodeObject->eClass)
         {
         case CO_ENUMERATION:    eObjectType = CST_GAME_OBJECT;     break;
         case CO_CONSTANT:       eObjectType = CST_SCRIPT_OBJECT;   break;
         default:                eObjectType = CST_COMMAND;         break;
         }

         //VERBOSE("CODE-EDIT: CodeObject '%s' identified for tooltip", pCodeObject->szText);

         /// Display tooltip
         displayCodeEditTooltip(pWindowData, TRUE, oHoverLocation.iLine, pCodeObject);
      }

      // Cleanup
      deleteCodeObject(pCodeObject);
      utilDeleteString(szLineText);
   }

   // [TRACK]
   END_TRACKING();
}



/// Function name  : onCodeEditMouseMove
// Description     : Alters the text selection and the position of the caret if the LEFT button is being held
// 
// CODE_EDIT_DATA*  pWindowData : [in] Window data
// CONST POINT*      ptCursor    : [in] Location of the mouse in client co-ordinates
// CONST UINT        iButtons    : [in] Button flags indicating which keys and mouse buttons were presesd
// 
VOID  onCodeEditMouseMove(CODE_EDIT_DATA*  pWindowData, POINT  ptCursor, CONST UINT  iButtons)
{
   CODE_EDIT_LOCATION  oCursorLocation,            // Location of the cursor
                       oPreviousCaretLocation;     // Existing location of the caret

   // [TRACK]
   TRACK_FUNCTION();

   /// [LEFT BUTTON HELD] Extend the selection if the left button is pressed
   if (GetCapture() == pWindowData->hWnd)
   {
      // Convert cursor co-ordinates into a valid location
      if (!calculateCodeEditLocationFromPoint(pWindowData, &ptCursor, &oCursorLocation))
         // [FAILED] Use final character
         calculateCodeEditFinalCharacterLocation(pWindowData, &oCursorLocation);
    
      // [CHECK] Do nothing if the cursor hasn't moved to a different character
      if (compareCodeEditLocations(oCursorLocation, pWindowData->oCaret.oLocation) != CR_EQUAL)
      {
         // Store current caret location
         oPreviousCaretLocation = pWindowData->oCaret.oLocation;

         /// Move the caret to cursor location
         setCodeEditCaretLocation(pWindowData, &oCursorLocation);

         /// Update selection
         updateCodeEditSelection(pWindowData, oPreviousCaretLocation);

         // [CURSOR ON TOP LINE] Scroll window upwards
         if (pWindowData->oCaret.oLocation.iLine == pWindowData->ptFirstCharacter.iLine)
            onCodeEditScroll(pWindowData, SB_LINEUP, NULL, SB_VERT);

         // [CURSOR ON BOTTOM LINE] Scroll window downwards
         else if (pWindowData->oCaret.oLocation.iLine >= (pWindowData->ptFirstCharacter.iLine + pWindowData->siPageSize.cy - 1))
            onCodeEditScroll(pWindowData, SB_LINEDOWN, NULL, SB_VERT);
      }
   }
   /// [NO BUTTONS and NO SUGGESTIONS]
   else if (utilExcludes(iButtons, MK_LBUTTON WITH MK_RBUTTON WITH MK_MBUTTON) AND !pWindowData->oSuggestion.hCtrl AND pWindowData->pPreferences->bEditorTooltips)
      // Request mouse hover
      utilTrackMouseEvent(pWindowData->hWnd, TME_HOVER, 1000 * pWindowData->pPreferences->iEditorTooltipDelay);
   
   // [TOOLTIP] Destroy Tooltip
   if (pWindowData->bTooltipVisible) 
      displayCodeEditTooltip(pWindowData, FALSE, NULL, NULL);

   // [TRACK]
   END_TRACKING();
}


/// Function name  : onCodeEditMouseWheel
// Description     : Scrolls the window (or suggestion list) after a mouse scroll
// 
// CODE_EDIT_DATA*  pWindowData : [in] Window data
// CONST INT        iWheelDelta : [in] Amount of scrolling required, in multiples of system constant WHEEL_DELTA
// 
VOID  onCodeEditMouseWheel(CODE_EDIT_DATA*  pWindowData, CONST INT  iWheelDelta)
{
   INT    iLineDelta;      // Number of lines to scroll up/down
   UINT   eScrollCommand;  // Scrolling constant to send to the CodeEdit/SuggestionList

   // [TRACK]
   TRACK_FUNCTION();

   // Determine number of lines to scroll
   iLineDelta = (iWheelDelta / WHEEL_DELTA);

   /// Determine direction of scroll by positive/negative value
   eScrollCommand = (iLineDelta > 0 ? SB_LINEUP : SB_LINEDOWN);

#ifdef BUG_FIX
   /// Determine direction of scroll by positive/negative value
   if (iLineDelta > 0)
      // [POSITIVE] Determine amount of scroll by magnitude of delta
      eScrollCommand = (iLineDelta > 1 ? SB_PAGEUP : SB_LINEUP);
   else
      // [NEGATIVE] Determine amount of scroll by magnitude of delta
      eScrollCommand = (iLineDelta < -1 ? SB_PAGEDOWN : SB_LINEDOWN);
#endif

   // Repeat the command as necessary if the user did some kind of mega scroll      // [FIX] BUG:016 'Scrolling the mouse wheel very fast causes the script editor to 'jump''
   for (INT iRepetitions = utilCalculateMagnitude(iLineDelta); iRepetitions >= 0; iRepetitions--)
   {
      /// Determine where to send the command
      if (!pWindowData->oSuggestion.hCtrl)
         // [CODE-EDIT] Command the window to scroll directly
         onCodeEditScroll(pWindowData, eScrollCommand, NULL, SB_VERT);
      else
         // [SUGGESTION-LIST] Pass the list a WM_VSCROLL message
         SendMessage(pWindowData->oSuggestion.hCtrl, WM_VSCROLL, eScrollCommand, NULL);
   }

   // [CHECK] Is the Tooltip visible?
   if (pWindowData->bTooltipVisible) 
      // [TOOLTIP] Destroy Tooltip
      displayCodeEditTooltip(pWindowData, FALSE, NULL, NULL);

   // [TRACK]
   END_TRACKING();
}


/// Function name  : onCodeEditRButtonDown
// Description     : Moves the caret to the cursor, unless over a selection
// 
// CODE_EDIT_DATA*  pWindowData : [in] Window data
// CONST POINT*      ptClick     : [in] Client co-ordinates of the click
// CONST UINT        iButtons    : [in] Button flags
// 
VOID  onCodeEditRButtonDown(CODE_EDIT_DATA*  pWindowData, CONST POINT*  ptClick, CONST UINT  iButtons)
{
   CODE_EDIT_LOCATION  oClickLocation;       // Location of the input click

   // [TRACK]
   TRACK_FUNCTION();

   // [SUGGESTIONS] Hide suggestions
   destroyCodeEditSuggestionList(pWindowData);

   /// Set focus to the CodeEdit
   SetFocus(pWindowData->hWnd);

   // Convert click into a valid location, if possible
   if (!calculateCodeEditLocationFromPoint(pWindowData, ptClick, &oClickLocation))
      // [CLICK BENEATH TEXT] Use the final character
      calculateCodeEditFinalCharacterLocation(pWindowData, &oClickLocation);
   
   /// [OUTSIDE SELECTION] Cancel selection and move caret to the cursor
   if (!isCodeEditLocationSelected(pWindowData, oClickLocation))
   {
      // Cancel current selection
      removeCodeEditSelection(pWindowData);

      // Move the caret to the cursor
      setCodeEditCaretLocation(pWindowData, &oClickLocation);

      // Remove selection between CARET and ORIGIN
      setCodeEditSelection(pWindowData, pWindowData->oSelection.oOrigin, pWindowData->oCaret.oLocation, FALSE);
   }

   // [TRACK]
   END_TRACKING();
}


/// Function name  : onCodeEditLoseFocus
// Description     : Hides the caret
// 
// CODE_EDIT_DATA*  pWindowData     : [in] Window data
// HWND              hDestinationWnd : [in] Handle of the window receiving the focus
// 
VOID  onCodeEditLoseFocus(CODE_EDIT_DATA*  pWindowData, HWND  hDestinationWnd)
{
   // [TRACK]
   TRACK_FUNCTION();

   /// [CHECK] Ensure we're not losing focus to the suggestion control
   if (pWindowData AND hDestinationWnd != pWindowData->oSuggestion.hCtrl)  // [HACK] Sometimes received after window data is destroyed! 
   {
      // Destroy caret
      DestroyCaret();

      // Hide tooltip
      displayCodeEditTooltip(pWindowData, FALSE, NULL, NULL);

      // Destroy suggestion ListView and data tree
      destroyCodeEditSuggestionList(pWindowData);
   }

   // Send EN_KILLFOCUS
   if (pWindowData)
      SendMessage(GetParent(pWindowData->hWnd), WM_COMMAND, MAKE_LONG(GetWindowID(pWindowData->hWnd), EN_KILLFOCUS), (LPARAM)pWindowData->hWnd);

   // [TRACK]
   END_TRACKING();
}


/// Function name  : onCodeEditReceiveFocus
// Description     : Displays the caret
// 
// CODE_EDIT_DATA*  pWindowData  : [in] Window data   (May not exist - This is sometimes received during destruction, probably because the document automatically sets the focus here)
// HWND             hSourceWnd   : [in] Handle of the window losing the focus
// 
VOID  onCodeEditReceiveFocus(CODE_EDIT_DATA*  pWindowData, HWND  hSourceWnd)
{
   // [TRACK]
   TRACK_FUNCTION();
   
   /// [CHECK] Ensure we're not receiving focus from the Suggestion control
   if (pWindowData AND hSourceWnd != pWindowData->oSuggestion.hCtrl AND hSourceWnd != NULL)
   {
      // Create Caret
      CreateCaret(pWindowData->hWnd, NULL, 1, pWindowData->siCharacterSize.cy);

      // Position and display
      updateCodeEditCaretPosition(pWindowData);
      ShowCaret(pWindowData->hWnd);
   }

   // Send EN_SETFOCUS
   if (pWindowData)
      SendMessage(GetParent(pWindowData->hWnd), WM_COMMAND, MAKE_LONG(GetWindowID(pWindowData->hWnd), EN_SETFOCUS), (LPARAM)pWindowData->hWnd);

   // [TRACK]
   END_TRACKING();
}


/// Function name  : onCodeEditNotification
// Description     : Handles notifications from the suggestion ListView
// 
// CODE_EDIT_DATA*  pWindowData  : [in]     Window data
// CONST UINT       iControlID   : [in]     Source control ID
// NMHDR*           pHeader      : [in/out] Message data
// 
// Return Value : Message result
//
LRESULT  onCodeEditNotification(CODE_EDIT_DATA*  pWindowData, CONST UINT  iControlID, NMHDR*  pHeader)
{
   //NMLVKEYDOWN*        pKeyPress;
   NMLVDISPINFO*       pDataRequest;      // LVN_GETDISPINFO header
   NMLVCUSTOMDRAW*     pCustomListView;   // ListView NM_CUSTOMDRAW header
   LRESULT             iResult;           // Message Result
   BOOL                bHandled;

   // Prepare
   TRACK_FUNCTION();
   bHandled = FALSE;
   iResult  = 0;

   /// [SUGGESTION LISTVIEW]
   if (pHeader->hwndFrom == pWindowData->oSuggestion.hCtrl)
   {
      // Prepare
      pDataRequest     = (NMLVDISPINFO*)pHeader;
      pCustomListView  = (NMLVCUSTOMDRAW*)pHeader;

      // Examine notification
      switch (pHeader->code)
      {
      /// [DOUBLE CLICK] Simulate the action of pressing TAB
      case NM_DBLCLK:
         onCodeEditSuggestionListKeyDown(pWindowData, VK_TAB);
         bHandled = TRUE;
         break;

      /// [CUSTOM DRAW]
      case NM_CUSTOMDRAW:
         iResult  = onCustomDrawListView(pWindowData->hWnd, pHeader->hwndFrom, pCustomListView);
         bHandled = TRUE;
         break;

      ///// [KEY PRESS] 
      //case LVN_KEYDOWN:
      //   pKeyPress = (NMLVKEYDOWN*)pHeader;
      //   // 
      //   onCodeEditSuggestionListKeyDown(pWindowData, pKeyPress->wVKey);
      //   bHandled = TRUE;
      //   break;

      /// [LOST FOCUS] Destroy ListView 
      case NM_KILLFOCUS:
         // [CHECK] Destroy caret if CodeEdit did not receive the focus
         if (GetFocus() != pWindowData->hWnd)
            DestroyCaret();

         // Destroy ListView
         destroyCodeEditSuggestionList(pWindowData);
         bHandled = TRUE;
         break;

      /// [REQUEST DATA] Provide item data
      case LVN_GETDISPINFO:
         onCodeEditSuggestionItemRequest(pWindowData, pDataRequest->item.iItem, &pDataRequest->item);
         bHandled = TRUE;
         break;
      }
   }
   /// [TOOLTIP]
   else if (pHeader->hwndFrom == pWindowData->hTooltip)
   {
      // Examine notification
      switch (pHeader->code)
      {
      /// [CUSTOM DRAW]
      case NM_CUSTOMDRAW:
         //VERBOSE("CODE-EDIT: Custom drawing Tooltip");

         // Perform CustomDraw
         iResult = onCustomDrawTooltip(pHeader->hwndFrom, (NMTTCUSTOMDRAW*)pHeader, pWindowData->pTooltipData);
         bHandled = TRUE;
         break;

      /// [REQUEST TEXT]
      case TTN_GETDISPINFO:
         //VERBOSE("CODE-EDIT: TTN_GETDISPINFO received");
         StringCchCopy(((NMTTDISPINFO*)pHeader)->szText, 80, TEXT("Dummy Tooltip text"));
         bHandled = TRUE;
         break;
      }
   }

   // Return result or pass to base
   END_TRACKING();
   return (bHandled ? iResult : DefWindowProc(pWindowData->hWnd, WM_NOTIFY, iControlID, (LPARAM)pHeader));
}

/// Function name  : onCodeEditPaint
// Description     : Paints the invalidated lines in a CodeEdit
// 
// CODE_EDIT_DATA*  pWindowData   : [in] Window data
// 
VOID  onCodeEditPaint(CODE_EDIT_DATA*  pWindowData)
{
   PAINTSTRUCT   oPaintData;        // System painting data
   LIST_ITEM*    pIterator;         // CodeEdit line data iterator
   UINT          iLineNumber;       // Line number of the line currently being painted
   RECT          rcLineRect,        // Bounding rectangle of the line currently being painted, in client co-ordinates
                 rcClientRect,      // CodeEdit window client rectangle
                 rcDummyRect;       // Used for testing the intersection of the line and update rectangles
   HRGN          rgnClippingArea;   // Clipping region for the window, which excludes the 3D border
   HDC           hDC;               // Destination device context

   // [TRACK]
   TRACK_FUNCTION();

   // Prepare
   GetClientRect(pWindowData->hWnd, &rcClientRect);
   iLineNumber = pWindowData->ptFirstCharacter.iLine;

   /// Draw a 3D border around the control
   hDC = BeginPaint(pWindowData->hWnd, &oPaintData);
   DrawEdge(hDC, &rcClientRect, EDGE_ETCHED, BF_TOPLEFT);

   // Define clipping region within the border
   rgnClippingArea = CreateRectRgn(rcClientRect.left + 1, rcClientRect.top + 1, rcClientRect.right, rcClientRect.bottom);
   SelectClipRgn(hDC, rgnClippingArea);

   // Determine the bounding rectangle of the first visible line
   calculateCodeEditLineRectangle(pWindowData, iLineNumber, &rcLineRect);

   /// Iterate through visible lines of script code
   for (findCodeEditLineListItemByIndex(pWindowData, iLineNumber, pIterator); pIterator AND rcLineRect.top <= rcClientRect.bottom; pIterator = pIterator->pNext)
   {
      // Determine whether line intersects the update region
      if (IntersectRect(&rcDummyRect, &rcLineRect, &oPaintData.rcPaint))
         // Draw current line
         drawCodeEditLine(hDC, pWindowData->pColourScheme, pWindowData->pScriptFile, extractListItemPointer(pIterator, CODE_EDIT_LINE), iLineNumber, rcLineRect);
      
      // Advance to the next line
      OffsetRect(&rcLineRect, NULL, pWindowData->siCharacterSize.cy);
      iLineNumber++;
   }

   /// Iterate through any remaining 'lines' at the end of the document
   while (rcLineRect.top <= rcClientRect.bottom)
   {
      // Determine whether line intersects the update region
      if (IntersectRect(&rcDummyRect, &rcLineRect, &oPaintData.rcPaint))
         // Draw an empty line
         drawCodeEditLine(hDC, pWindowData->pColourScheme, NULL, NULL, iLineNumber, rcLineRect);

      // Advance to the next line
      OffsetRect(&rcLineRect, NULL, pWindowData->siCharacterSize.cy);
      iLineNumber++;
   }

   // Cleanup
   SelectClipRgn(hDC, NULL);
   DeleteObject(rgnClippingArea);
   EndPaint(pWindowData->hWnd, &oPaintData);
   END_TRACKING();
}


/// Function name  : onCodeEditPaintNonClient
// Description     : Paints the scroll bar in the CodeEdit with a border
// 
// CODE_EDIT_DATA*  pWindowData   : [in] Window data
// 
/// IGNORED: I can't be bothered drawing the scrollbar just to add a border
//
VOID  onCodeEditPaintNonClient(CODE_EDIT_DATA*  pWindowData, HRGN  hUpdateRgn)
{
   //HTHEME  hTheme;
   RECT    rcWindow;
   HDC     hDC;

   // [TRACK]
   TRACK_FUNCTION();
  
   // Prepare
   hDC = GetWindowDC(pWindowData->hWnd);
  
   // Get window rectangle
   GetWindowRect(pWindowData->hWnd, &rcWindow);
   utilScreenToClientRect(pWindowData->hWnd, &rcWindow);

   // Draw bottom right edge
   DrawEdge(hDC, &rcWindow, EDGE_SUNKEN, BF_BOTTOMRIGHT);

   // Cleanup
   ReleaseDC(pWindowData->hWnd, hDC);
   END_TRACKING();
}


/// Function name  : onCodeEditResize
// Description     : Recalculates the size of a page and adjusts the scrollbars to match
// 
// CODE_EDIT_DATA*  pWindowData : [in] Window data
// CONST UINT        iWidth      : [in] New window width, in pixels
// CONST UINT        iHeight     : [in] New window height, in pixels
// 
VOID  onCodeEditResize(CODE_EDIT_DATA*  pWindowData, CONST UINT  iWidth, CONST UINT  iHeight)
{
   // [TRACK]
   TRACK_FUNCTION();

   // Recalculate the size of a page
   calculateCodeEditPageSize(pWindowData);

   // Adjust scrollbars to reflect new page size
   updateCodeEditScrollBarLimits(pWindowData);

   //// [CHECK] Have we enlarged window enough to display entire width?
   //if (pWindowData->siPageSize.cy >= pWindowData->ptLastCharacter.iLine)
   //   onCodeEditScroll(pWindowData, SB_THUMBTRACK, 0, SB_HORZ);

   // [TRACK]
   END_TRACKING();
}


/// Function name  : onCodeEditScroll
// Description     : Updates the drawing origin to create the illusion of movement, according to the amount of scrolling requested
// 
// CODE_EDIT_DATA*  pWindowData   : [in]           Window data
// CONST UINT        iScrollType   : [in]           Type of scrolling being performed
// CONST UINT        iDragPosition : [in][optional] Custom dragging position
// CONST UINT        iDirection    : [in]           SB_VERT or SB_HORZ
// 
VOID  onCodeEditScroll(CODE_EDIT_DATA*  pWindowData, CONST UINT  iScrollType, CONST UINT  iDragPosition, CONST UINT  iDirection)
{
   INT   iNewPosition,        // New horizontal/vertical scroll position
         iMaximumPosition;    // Maximum horizontal/vertical scroll position

   // [TRACK]
   TRACK_FUNCTION();

   // [TOOLTIP/SUGGESTION] Hide both
   displayCodeEditTooltip(pWindowData, FALSE, NULL, NULL);
   destroyCodeEditSuggestionList(pWindowData);
   
   // Determine current horizontal/vertical position
   switch (iDirection)
   {
   case SB_HORZ:   iNewPosition = pWindowData->ptFirstCharacter.iIndex;    break;
   case SB_VERT:   iNewPosition = pWindowData->ptFirstCharacter.iLine;     break;
   }

   // Calcuate maximum position  (last character - page)
   iMaximumPosition = (iDirection == SB_HORZ ? (pWindowData->ptLastCharacter.iIndex - pWindowData->siPageSize.cx) : (pWindowData->ptLastCharacter.iLine - pWindowData->siPageSize.cy));

   // [CHECK] Ensure position is not negative
   iMaximumPosition = max(0, iMaximumPosition);

   // Calculate new position based on scroll type
   switch (iScrollType)
   {
   /// [LINE UP] - Scroll up (left) a single line (character)
   case SB_LINEUP:      iNewPosition--;        break;
   /// [LINE DOWN] - Scroll down (right) a single line (character)
   case SB_LINEDOWN:    iNewPosition++;        break;

   /// [TOP] - Scroll to the top (far left)
   case SB_TOP:         iNewPosition = 0;      break;
   /// [BOTTOM] - Scroll to the bottom (far right)
   case SB_BOTTOM:      iNewPosition = (iDirection == SB_VERT ? pWindowData->ptLastCharacter.iLine : pWindowData->ptLastCharacter.iIndex);     break;

   /// [PAGE UP] - Scroll up (left) a single page
   case SB_PAGEUP:      iNewPosition -= (iDirection == SB_VERT ? pWindowData->siPageSize.cy : pWindowData->siPageSize.cx);            break;
   /// [PAGE DOWN] - Scroll down (right) a single page
   case SB_PAGEDOWN:    iNewPosition += (iDirection == SB_VERT ? pWindowData->siPageSize.cy : pWindowData->siPageSize.cx);            break;

   /// [DRAG] - Scroll to wherever the user chooses
   case SB_THUMBTRACK:
      iNewPosition = iDragPosition;

      // Re-Calcuate maximum position based on the annoying fact that SB_THUMBTRACK 'helpfully' scrolls between min and (max-page) instead of just min->max
      iMaximumPosition = (iDirection == SB_HORZ ? pWindowData->ptLastCharacter.iIndex : pWindowData->ptLastCharacter.iLine);
      break;
   }

   // Re-examine scroll type
   switch (iScrollType)
   {
   /// [END-DRAG] - Ignore these to avoid 'skipping'
   case SB_THUMBPOSITION:
   case SB_ENDSCROLL:
      break;

   /// [REMAINING] Scroll window
   default:
      // Ensure new position is not negative
      iNewPosition = max(0, iNewPosition);

      // Ensure it's not out of range
      iNewPosition = (iNewPosition <= iMaximumPosition ? iNewPosition : iMaximumPosition);

      // Update internal position
      switch (iDirection)
      {
      case SB_HORZ:  pWindowData->ptFirstCharacter.iIndex = iNewPosition;     break;
      case SB_VERT:  pWindowData->ptFirstCharacter.iLine  = iNewPosition;     break;
      }

      /// Set ScrollBar to new position
      SetScrollPos(pWindowData->hWnd, iDirection, iNewPosition, TRUE);

      /// Re-diplay the caret at existing location (but new client co-ordinates)
      updateCodeEditCaretPosition(pWindowData);

      // Redraw window contents
      InvalidateRect(pWindowData->hWnd, NULL, FALSE);
      break;
   }

   // [TRACK]
   END_TRACKING();
}


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : wndprocCodeEdit
// Description     : CodeEdit window procedure
// 
// 
LRESULT   wndprocCodeEdit(HWND  hWnd, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{ 
   CODE_EDIT_LOCATION  oLocation;        // Location of a request to scroll to specific location
   CODE_EDIT_SEARCH*   pSearchData;      // Find & Replace search data
   CODE_EDIT_DATA*     pWindowData;      // Window data
   ERROR_STACK*        pError;           // Exception error, if any
   POINT               ptClick;          // Position of a mouse click, in client co-ordinates
   LRESULT             iResult;

   // [TRACK]
   TRACK_FUNCTION();

   /// [GUARD BLOCK]
   __try
   {
      // Prepare
      pWindowData = getCodeEditData(hWnd);
      iResult     = 0;

      // Examine message
      switch (iMessage)
      {
      /// [SYSTEM]
      // [CREATE] 
      case WM_CREATE:
         onCodeEditCreate(hWnd);
         break;

      // [DESTROY]
      case WM_DESTROY:
         onCodeEditDestroy(pWindowData);
         break;

      // [COMMAND]
      case WM_COMMAND:
         if (!onCodeEditCommand(pWindowData, LOWORD(wParam), HIWORD(wParam), (HWND)lParam))
            // Pass back to system for processing
            iResult = DefWindowProc(hWnd, iMessage, wParam, lParam);
         break;

      // [NOTIFICATION]
      case WM_NOTIFY:
         iResult = onCodeEditNotification(pWindowData, wParam, (NMHDR*)lParam);
         break;

      // [PAINT]
      case WM_PAINT:
         onCodeEditPaint(pWindowData);
         break;

      // [NON-CLIENT PAINT]
      /*case WM_NCPAINT:
         DefWindowProc(hWnd, iMessage, wParam, lParam);
         onCodeEditPaintNonClient(pWindowData, (HRGN)wParam);
         break;*/

      // [RESIZE]
      case WM_SIZE:
         onCodeEditResize(pWindowData, LOWORD(lParam), HIWORD(lParam));
         break;

      // [TIMER]
      case WM_TIMER:
         if (wParam == UNDO_COMMIT_TICKER)
            onCodeEditUndoCommitTimer(pWindowData);
         break;

      /// [APPLICATION DEFINED]
      // [CLEAR ALL ERRORS]
      case UM_CLEAR_LINE_ERRORS:
         onCodeEditClearLineErrors(pWindowData);
         break;

      // [COMMENT SELECTION]
      case UM_COMMENT_SELECTION:
         performCodeEditSelectionCommenting(pWindowData, (BOOL)wParam);
         break;

      // [FIND TEXT]
      case UM_FIND_TEXT:
         pSearchData = (CODE_EDIT_SEARCH*)lParam;
         iResult = (LRESULT)onCodeEditFindText(pWindowData, pSearchData->szSearch, pSearchData->szReplace, pSearchData->eFlags);
         break;

      // [GET CARET LINE COMMAND]
      case UM_GET_CARET_LINE_COMMAND:
         iResult = (LRESULT)getCodeEditCaretLineCommand(pWindowData);
         break;

      // [GET CARET LINE ERROR]
      case UM_GET_CARET_LINE_ERROR:
         iResult = (LRESULT)getCodeEditCaretLineError(pWindowData);
         break;

      // [GET CARET GAME STRING]
      case UM_GET_CARET_GAME_STRING:
         iResult = (LRESULT)getCodeEditCaretGameString(pWindowData, (PROJECT_FILE*)lParam);
         break;

      // [GET COMMENT QUALITY]
      case UM_GET_COMMENT_QUALITY:
         iResult = calculateCodeEditCommentQuality(pWindowData);
         break;

      // [GET LABEL QUEUE]
      case UM_GET_LABEL_QUEUE:
         iResult = (LRESULT)generateCodeEditLabelQueue(pWindowData);
         break;

      // [GET LINE INDENTATION]
      case UM_GET_LINE_INDENTATION:
         iResult = (LRESULT)getCodeEditLineIndentation(pWindowData, wParam);
         break;

      // [GET SCRIPT DEPENDENCIES TREE]
      case UM_GET_SCRIPT_DEPENDENCY_TREE:
         iResult = (LRESULT)generateCodeEditScriptDependencyTree(pWindowData);
         break;

      // [GET STRING DEPENDENCIES TREE]
      case UM_GET_STRING_DEPENDENCY_TREE:
         iResult = (LRESULT)generateCodeEditStringDependencyTree(pWindowData, (PROJECT_FILE*)lParam);
         break;

      // [GET VARIABLE DEPENDENCIES TREE]
      case UM_GET_VARIABLE_DEPENDENCY_TREE:
         iResult = (LRESULT)generateCodeEditVariableDependencyTree(pWindowData);
         break;

      // [GET VARIABLES TREE]
      case UM_GET_VARIABLE_NAME_TREE:
         iResult = (LRESULT)generateCodeEditVariableNamesTree(pWindowData);
         break;

      // [UM_GET_SELECTION_TEXT]
      case UM_GET_SELECTION_TEXT:
         iResult = (LRESULT)generateCodeEditSelectionText(pWindowData);
         break;

      // [HAS SELECTION]
      case UM_HAS_SELECTION:
         iResult = (LRESULT)hasCodeEditSelection(pWindowData);
         break;

      // [HAS SUGGESTIONS]
      case UM_HAS_SUGGESTIONS:
         iResult = (identifyCodeEditSuggestionTypeAtLocation(pWindowData, pWindowData->oCaret.oLocation.iLine, pWindowData->oCaret.oLocation.iIndex) != CST_NONE);
         break;

      // [INSERT SEARCH RESULT]
      case UM_INSERT_TEXT_AT_CARET:
         onCodeEditInsertTextAtCaret(pWindowData, (CONST TCHAR*)lParam);
         break;

      // [IS SELECITON COMMENTED]
      case UM_IS_SELECTION_COMMENTED:
         iResult = (LRESULT)isCodeEditSelectionCommented(pWindowData);
         break;

      // [LOAD SCRIPT FILE]
      case UM_SET_SCRIPT_FILE:
         insertCodeEditScriptFile(pWindowData, (SCRIPT_FILE*)lParam);
         break;

      // [SELECT ALL]
      case UM_SELECT_ALL:
         onCodeEditSelectAll(pWindowData);
         break;

      // [SET LINE ERROR]
      case UM_SET_LINE_ERROR:
         onCodeEditSetLineError(pWindowData, LOWORD(wParam), (ERROR_TYPE)HIWORD(wParam), (CONST ERROR_QUEUE*)lParam);
         break;

      // [SET PREFERENCES]
      case UM_SET_PREFERENCES:
         setCodeEditPreferences(pWindowData, (CONST PREFERENCES*)lParam);
         break;

      // [SCROLL TO LOCATION]
      case UM_SCROLL_TO_LOCATION:
         oLocation.iIndex = wParam;
         oLocation.iLine  = lParam;
         onCodeEditScrollToLocation(pWindowData, &oLocation);
         break;

      // [SHOW SUGGESTIONS]
      case UM_SHOW_SUGGESTIONS:
         updateCodeEditSuggestionList(pWindowData, pWindowData->oCaret.oLocation.iIndex, FALSE);
         break;

      /// [TEXT RELATED MESSAGES]
      case EM_GETLINECOUNT:
      case EM_LINELENGTH:
      case EM_SETSEL:
      case EM_CANUNDO:
      case EM_CANREDO:
      case UM_GET_CARET_LOCATION:
      case UM_GET_LINE_TEXT:
         iResult = onCodeEditInterfaceMessage(pWindowData, iMessage, wParam, lParam);
         break;

      /// [CLIPBOARD]
      // [CUT]
      case WM_CUT:
         onCodeEditClipboardCut(pWindowData);
         break;

      // [COPY]
      case WM_COPY:
         onCodeEditClipboardCopy(pWindowData);
         break;

      // [PASTE]
      case WM_PASTE:
         onCodeEditClipboardPaste(pWindowData);
         break;

      /// [UNDO/REDO]
      case WM_UNDO:
      case EM_UNDO:  onCodeEditUndoLastAction(pWindowData); break;
      case EM_REDO:  onCodeEditRedoLastAction(pWindowData); break;

      /// [KEYBOARD and MOUSE]
      // [LEFT CLICK DOWN]
      case WM_LBUTTONDOWN:
         ptClick.x = LOWORD(lParam);
         ptClick.y = HIWORD(lParam);
         onCodeEditLButtonDown(pWindowData, &ptClick, wParam);
         // Pass back to system to generate double-click messages
         iResult = DefWindowProc(hWnd, iMessage, wParam, lParam);
         break;

      // [LEFT CLICK UP]
      case WM_LBUTTONUP:
         ptClick.x = GET_X_LPARAM(lParam);
         ptClick.y = GET_Y_LPARAM(lParam);
         onCodeEditLButtonUp(pWindowData, &ptClick, wParam);
         break;

      // [LEFT DOUBLE-CLICK]
      case WM_LBUTTONDBLCLK:
         ptClick.x = GET_X_LPARAM(lParam);
         ptClick.y = GET_Y_LPARAM(lParam);
         onCodeEditLButtonDoubleClick(pWindowData, &ptClick, wParam);
         break;

      // [MOUSE HOVER]
      case WM_MOUSEHOVER:
         ptClick.x = GET_X_LPARAM(lParam);
         ptClick.y = GET_Y_LPARAM(lParam);
         onCodeEditMouseHover(pWindowData, &ptClick, wParam);
         break;

      // [MOUSE MOVE]
      case WM_MOUSEMOVE:
         ptClick.x = GET_X_LPARAM(lParam);
         ptClick.y = GET_Y_LPARAM(lParam);

         // [CHECK] Ignore spurious WM_MOUSEMOVE messages
         if (ptClick.x != pWindowData->ptCursor.x OR ptClick.y != pWindowData->ptCursor.y)
         {
            pWindowData->ptCursor = ptClick;
            onCodeEditMouseMove(pWindowData, ptClick, wParam);
         }
         break;

      // [RIGHT CLICK DOWN]
      case WM_RBUTTONDOWN:
         ptClick.x = GET_X_LPARAM(lParam);
         ptClick.y = GET_Y_LPARAM(lParam);
         onCodeEditRButtonDown(pWindowData, &ptClick, wParam);
         // Pass back to system to generate double-click messages
         iResult = DefWindowProc(hWnd, iMessage, wParam, lParam);
         break;

      // [CHARACTER INPUT]
      case WM_CHAR:
         onCodeEditCharacterPress(pWindowData, wParam);
         break;

      // [KEY DOWN]
      case WM_KEYDOWN:
         onCodeEditKeyDown(pWindowData, wParam);
         break;

      // [REQUEST KEYBOARD INPUT]
      case WM_GETDLGCODE:
         iResult = DLGC_WANTALLKEYS;
         break;

      //// [TIMER] (Rescinded)
      //case WM_TIMER:
      //   if (!onCodeEditTimer(pWindowData, wParam))
      //      iResult = DefWindowProc(hWnd, iMessage, wParam, lParam);
      //   break;

      /// [SCROLLING and FOCUS]
      // [VERTICAL SCROLL]
      case WM_VSCROLL:
         onCodeEditScroll(pWindowData, LOWORD(wParam), HIWORD(wParam), SB_VERT);
         break;

      // [HORIZONTAL SCROLL]
      case WM_HSCROLL:
         onCodeEditScroll(pWindowData, LOWORD(wParam), HIWORD(wParam), SB_HORZ);
         break;

      // [MOUSE WHEEL]
      case WM_MOUSEWHEEL:
         onCodeEditMouseWheel(pWindowData, GET_WHEEL_DELTA_WPARAM(wParam));
         break;

      // [RECEIVE FOCUS]
      case WM_SETFOCUS:
         onCodeEditReceiveFocus(pWindowData, (HWND)wParam);
         break;

      // [LOSE FOCUS]
      case WM_KILLFOCUS:
         onCodeEditLoseFocus(pWindowData, (HWND)wParam);
         break;

      /// [UNSUPPORTED]
      default:
         iResult = DefWindowProc(hWnd, iMessage, wParam, lParam);
         break;
      }
   }
   /// [EXCEPTION HANDLER]
   __except (generateExceptionError(GetExceptionInformation(), pError))
   {
      // [ERROR] "An unidentified and unexpected critical error has occurred in the code window of script '%s'"
      enhanceError(pError, ERROR_ID(IDS_EXCEPTION_CODE_EDIT), (pWindowData AND pWindowData->pScriptFile) ? pWindowData->pScriptFile->szScriptname : TEXT("Unknown"));
      SendMessage(getAppWindow(), UN_CODE_EDIT_EXCEPTION, (WPARAM)hWnd, (LPARAM)pError);
   }

   // Return result
   END_TRACKING();
   return iResult;
}


