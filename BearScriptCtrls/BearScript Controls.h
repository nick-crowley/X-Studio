//
// BearScript Controls.h  --  Header for the BearScript control library
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#pragma once

#include "Types.h"

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                        MACROS
/// ////////////////////////////////////////////////////////////////////////////////////////

#ifdef _ControlsDLL
   #define ControlsAPI __declspec(dllexport)
#else
   #define ControlsAPI __declspec(dllimport)
#endif

// Convenience descriptions of WM_COMMAND message sources
//
enum COMMAND_SOURCE  { CS_MENU_ITEM = 0, CS_ACCELERATOR = 1 };


/// ////////////////////////////////////////////////////////////////////////////////////////
///                                   CONSTANTS / GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////

// Window class name
extern ControlsAPI CONST TCHAR*  szCodeEditClass;
extern CONST TCHAR*              szCommentRatioCtrl;
extern ControlsAPI CONST TCHAR*  szGroupedListViewClass;
extern CONST TCHAR*              szMessageHeaderClass;
extern CONST TCHAR*              szWorkspaceClass;          

// Control library instance
extern ControlsAPI HINSTANCE     hControlsInstance;

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                   CODE EDIT (ADVANCED)
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
CODE_EDIT_LABEL*  createCodeEditLabel(CONST TCHAR*  szName, CONST UINT  iLineNumber);
ControlsAPI VOID  deleteCodeEditLabel(CODE_EDIT_LABEL*  &pCodeEditLabel);
ControlsAPI VOID  destructorCodeEditLabel(LPARAM  pCodeEditLabel);

// Helpers
COMMAND*          getCodeEditCaretLineCommand(CODE_EDIT_DATA*  pWindowData);
ERROR_QUEUE*      getCodeEditCaretLineError(CODE_EDIT_DATA*  pWindowData);
UINT              getCodeEditLineIndentation(CODE_EDIT_DATA*   pWindowData, CONST UINT  iLine);
BOOL              isCodeEditLineWhitespace(CONST CODE_EDIT_LINE*  pLineData);

// Functions
UINT             calculateCodeEditCommentQuality(CODE_EDIT_DATA*  pWindowData);
VOID             displayCodeEditTooltip(CODE_EDIT_DATA*  pWindowData, CONST BOOL  bShow, CONST UINT  iLine, CONST CODEOBJECT*  pCodeObject);
QUEUE*           generateCodeEditLabelQueue(CODE_EDIT_DATA*  pWindowData);
AVL_TREE*        generateCodeEditScriptDependencyTree(CODE_EDIT_DATA*  pWindowData);
AVL_TREE*        generateCodeEditStringDependencyTree(CODE_EDIT_DATA*  pWindowData, CONST PROJECT_FILE*  pProjectFile);
CUSTOM_TOOLTIP*  generateCodeEditTooltipData(CODE_EDIT_DATA*  pWindowData, CONST UINT  iLine, CONST CODEOBJECT*  pCodeObject);
AVL_TREE*        generateCodeEditVariableDependencyTree(CODE_EDIT_DATA*  pWindowData);
AVL_TREE*        generateCodeEditVariableNamesTree(CODE_EDIT_DATA*  pWindowData);
GAME_STRING*     getCodeEditCaretGameString(CODE_EDIT_DATA*  pWindowData, CONST PROJECT_FILE*  pProjectFile);
BOOL             isCodeEditSelectionCommented(CODE_EDIT_DATA*  pWindowData);
VOID             performCodeEditSelectionCommenting(CODE_EDIT_DATA*  pWindowData, CONST BOOL  bComment);
BOOL             resolveCodeEditIntegerParameter(CODE_EDIT_DATA*  pWindowData, LIST_ITEM*  pTargetLine, CONST UINT  iParameterIndex, CONST PROJECT_FILE*  pProjectFile, UINT&  iOutput);
VOID             updateCodeEditScope(CODE_EDIT_DATA*  pWindowData);

// Message Handlers
VOID    onCodeEditClearLineErrors(CODE_EDIT_DATA*  pWindowData);
VOID    onCodeEditClipboardCopy(CONST CODE_EDIT_DATA*  pWindowData);
VOID    onCodeEditClipboardCut(CODE_EDIT_DATA*  pWindowData);
VOID    onCodeEditClipboardPaste(CODE_EDIT_DATA*  pWindowData);
BOOL    onCodeEditFindText(CODE_EDIT_DATA*  pWindowData, CONST TCHAR*  szSearchText, CONST TCHAR*  szReplaceText, CONST UINT  eFlags);
VOID    onCodeEditInsertTextAtCaret(CODE_EDIT_DATA*  pWindowData, CONST TCHAR*  szText);
VOID    onCodeEditScriptCallOperator(CODE_EDIT_DATA*  pWindowData, CODE_EDIT_LINE*  pLineData);
VOID    onCodeEditScrollToLocation(CODE_EDIT_DATA*  pWindowData, CONST CODE_EDIT_LOCATION*  pTarget);
VOID    onCodeEditSelectAll(CODE_EDIT_DATA*  pWindowData);
VOID    onCodeEditSetLineError(CODE_EDIT_DATA*  pWindowData, CONST UINT  iLineNumber, CONST ERROR_TYPE  eType, CONST ERROR_QUEUE*  pErrorQueue);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                  CODE EDIT (BASE)
/// ////////////////////////////////////////////////////////////////////////////////////////

// Helpers
VOID    calculateCodeEditFinalCharacterLocation(CONST CODE_EDIT_DATA*  pWindowData, CODE_EDIT_LOCATION*  pOutput);
VOID    calculateCodeEditPageSize(CODE_EDIT_DATA*  pWindowData);
VOID    calculateCodeEditScrollBarLimits(CODE_EDIT_DATA*  pWindowData);
VOID    drawCodeEditTextStream(HDC  hDC, CONST TCHAR*  szText, CONST UINT  iLength, RECT*  pDrawRect);
VOID    enableCodeEditEvents(CODE_EDIT_DATA*  pWindowData, CONST BOOL  bEnable);
UINT    getCodeEditLineCount(CONST CODE_EDIT_DATA*  pWindowData);
UINT    getCodeEditLineLength(CONST CODE_EDIT_LINE*  pLineData);
UINT    getCodeEditLineLengthByIndex(CONST CODE_EDIT_DATA*  pWindowData, CONST UINT  iLineNumber);
BOOL    hasCodeEditSelection(CONST CODE_EDIT_DATA*  pWindowData);

// Functions
VOID    calculateCodeEditDisplayRectangle(HDC  hDC, CONST CODE_EDIT_LINE*  pLineData, CONST CODE_EDIT_DISPLAY_RECTANGLE  eRectangle, CONST RECT&  rcLineRect, RECT*  pOutputRect);
VOID    calculateCodeEditLineRectangle(CONST CODE_EDIT_DATA*  pWindowData, UINT  iLineNumber, RECT*  pOutput);
BOOL    calculateCodeEditLocationFromPoint(CONST CODE_EDIT_DATA*  pWindowData, CONST POINT*  ptPoint, CODE_EDIT_LOCATION*  pOutput);
VOID    calculateCodeEditMaximumLineLength(CODE_EDIT_DATA*  pWindowData, CONST UINT  iLineNumber, CONST CODE_EDIT_LINE*  pLineData);
BOOL    calculateCodeEditPointFromLocation(CONST CODE_EDIT_DATA*  pWindowData, CONST CODE_EDIT_LOCATION*  ptLocation, POINT*  ptOutput);
UINT    calculateCodeEditSelectionLength(CONST CODE_EDIT_DATA*  pWindowData);
COMPARISON_RESULT     compareCodeEditLocations(CONST CODE_EDIT_LOCATION&  oFirst, CONST CODE_EDIT_LOCATION&  oSecond);
BOOL    compareCodeEditLocationsEx(CONST CODE_EDIT_LOCATION&  oFirst, CONST CODE_EDIT_LOCATION&  oSecond, CONST COMPARISON_RESULT  eDesiredResult);
VOID    drawCodeEditLine(HDC  hDC, CONST COLOUR_SCHEME*  pColourScheme, CONST SCRIPT_FILE*  pScriptFile, CONST CODE_EDIT_LINE*  pLineData, CONST UINT  iLineNumber, CONST RECT&  rcLineRect);
VOID    drawCodeEditLineSquiggle(HDC  hDC, CONST CODE_EDIT_SQUIGGLE  eSquiggle, CONST RECT&  rcTextRect);
VOID    ensureCodeEditLocationIsVisible(CODE_EDIT_DATA*  pWindowData, CONST CODE_EDIT_LOCATION*  ptLocation);
TCHAR*  generateCodeEditLineText(CONST CODE_EDIT_LINE*  pLineData);
TCHAR*  generateCodeEditSelectionText(CONST CODE_EDIT_DATA*  pWindowData);
VOID    invalidateCodeEditLine(CODE_EDIT_DATA*  pWindowData, CONST UINT  iLineNumber);
BOOL    isCodeEditLocationSelected(CODE_EDIT_DATA*  pWindowData, CONST CODE_EDIT_LOCATION&  oLocation);
VOID    setCodeEditCaretLocation(CODE_EDIT_DATA*  pWindowData, CONST CODE_EDIT_LOCATION*  ptLocation);
VOID    setCodeEditPreferences(CODE_EDIT_DATA*  pWindowData, CONST PREFERENCES*  pPreferences);
VOID    setCodeEditSelection(CODE_EDIT_DATA*  pWindowData, CONST CODE_EDIT_LOCATION&  oLocation1, CONST CODE_EDIT_LOCATION&  oLocation2, CONST BOOL  bState);
CODE_EDIT_AREA        performCodeEditHitTest(CODE_EDIT_DATA*  pWindowData, CONST POINT*  ptCursor, CODE_EDIT_LOCATION*  pOutputLocation);
VOID    updateCodeEditCaretPosition(CODE_EDIT_DATA*  pWindowData);
VOID    updateCodeEditLine(CODE_EDIT_DATA*  pWindowData, CONST UINT  iLineNumber, CONST UINT  iUpdateFlags);
VOID    updateCodeEditScrollBarLimits(CODE_EDIT_DATA*  pWindowData);
VOID    updateCodeEditSelection(CODE_EDIT_DATA*  pWindowData, CONST CODE_EDIT_LOCATION&  oOldCaret);
VOID    updateCodeEditLineCommand(CONST CODE_EDIT_DATA*  pWindowData, CODE_EDIT_LINE*  pLineData, CONST COMMAND*  pExistingCommand);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                   CODE EDIT  (CONTROL)
/// ////////////////////////////////////////////////////////////////////////////////////////

// Helpers
CODE_EDIT_DATA*  getCodeEditData(HWND  hCodeEdit);

// Message Handlers
VOID      onCodeEditCharacterPress(CODE_EDIT_DATA*  pWindowData, CONST TCHAR  chCharacter);
BOOL      onCodeEditCommand(CODE_EDIT_DATA*  pWindowData, CONST UINT  iCommandID, CONST UINT  iSource, HWND  hCtrl);
VOID      onCodeEditCreate(HWND  hWnd);
VOID      onCodeEditDestroy(CODE_EDIT_DATA*  pWindowData);
VOID      onCodeEditKeyDown(CODE_EDIT_DATA*  pWindowData, CONST UINT  iVirtualKey);
LRESULT   onCodeEditInterfaceMessage(CODE_EDIT_DATA*  pWindowData, CONST UINT  iMessage, WPARAM  wParam, LPARAM  lParam);
VOID      onCodeEditLButtonDoubleClick(CODE_EDIT_DATA*  pWindowData, CONST POINT*  ptClick, CONST UINT  iButtons);
VOID      onCodeEditLButtonDown(CODE_EDIT_DATA*  pWindowData, CONST POINT*  ptClick, CONST UINT  iButtons);
VOID      onCodeEditLButtonUp(CODE_EDIT_DATA*  pWindowData, CONST POINT*  ptClick, CONST UINT  iButtons);
VOID      onCodeEditLoseFocus(CODE_EDIT_DATA*  pWindowData, HWND  hDestinationWnd);
VOID      onCodeEditMouseMove(CODE_EDIT_DATA*  pWindowData, POINT  ptCursor, CONST UINT  iButtons);
VOID      onCodeEditMouseWheel(CODE_EDIT_DATA*  pWindowData, CONST INT  iWheelDelta);
VOID      onCodeEditRButtonDown(CODE_EDIT_DATA*  pWindowData, CONST POINT*  ptClick, CONST UINT  iButtons);
VOID      onCodeEditReceiveFocus(CODE_EDIT_DATA*  pWindowData, HWND  hSourceWnd);
LRESULT   onCodeEditNotification(CODE_EDIT_DATA*  pWindowData, CONST UINT  iControlID, NMHDR*  pHeader);
VOID      onCodeEditPaint(CODE_EDIT_DATA*  pWindowData);
VOID      onCodeEditPaintNonClient(CODE_EDIT_DATA*  pWindowData, HRGN  hUpdateRgn);
VOID      onCodeEditResize(CODE_EDIT_DATA*  pWindowData, CONST UINT  iWidth, CONST UINT  iHeight);
VOID      onCodeEditScroll(CODE_EDIT_DATA*  pWindowData, CONST UINT  iScrollType, CONST UINT  iDragPosition, CONST UINT  iDirection);

// Window procedure
LRESULT   wndprocCodeEdit(HWND  hWnd, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                   CODE EDIT (DATA)
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
CODE_EDIT_CHARACTER*  createCodeEditCharacter(CONST TCHAR  chCharacter);
CODE_EDIT_DATA*       createCodeEditData(HWND  hWnd);
CODE_EDIT_LINE*       createCodeEditLine();
CODE_EDIT_LINE*       createCodeEditLineFromCommand(CONST CODE_EDIT_DATA*  pWindowData, CONST COMMAND*  pCommand);
CODE_FRAGMENT*        createCodeFragment(CONST CODE_EDIT_LINE*  pLineData, CONST COLOUR_SCHEME*  pColourScheme, CONST SCRIPT_FILE*  pScriptFile);
VOID     deleteCodeEditCharacter(CODE_EDIT_CHARACTER*  &pCharacterData);
VOID     destructorCodeEditCharacter(LPARAM  pCharacterData);
VOID     deleteCodeEditData(CODE_EDIT_DATA*  &pWindowData);
VOID     deleteCodeEditLine(CODE_EDIT_LINE*  &pLineData);
VOID     destructorCodeEditLine(LPARAM  pLineData);
VOID     deleteCodeFragment(CODE_FRAGMENT*  &pCodeFragment);

// Helpers
VOID     appendCodeEditCharacterToLine(CODE_EDIT_LINE* pLineData, CONST TCHAR  chCharacter);

// Functions
BOOL     findCodeEditCharacterDataByIndex(CONST CODE_EDIT_LINE*  pLineData, CONST UINT  iIndex, CODE_EDIT_CHARACTER*  &pOutput);
BOOL     findCodeEditCharacterListItemByIndex(CONST CODE_EDIT_LINE*  pLineData, CONST UINT  iIndex, LIST_ITEM*  &pOutput);
BOOL     findCodeEditLineDataByIndex(CONST CODE_EDIT_DATA*  pWindowData, CONST UINT  iLineNumber, CODE_EDIT_LINE*  &pOutput);
BOOL     findCodeEditLineListItemByIndex(CONST CODE_EDIT_DATA*  pWindowData, CONST UINT  iIndex, LIST_ITEM*  &pOutput);
BOOL     findNextCodeFragment(CODE_FRAGMENT*  pCodeFragment);
UINT     findNextCodeEditWord(CONST CODE_EDIT_LINE*  pLineData, CONST UINT  iCurrentIndex);
UINT     findPrevCodeEditWord(CONST CODE_EDIT_LINE*  pLineData, CONST UINT  iCurrentIndex);
VOID     insertCodeEditCharacterByIndex(CODE_EDIT_LINE*  pLineData, CONST UINT  iIndex, CONST TCHAR  chCharacter);
VOID     insertCodeEditLineByIndex(CODE_EDIT_DATA*  pWindowData, CONST UINT  iIndex, CODE_EDIT_LINE*  pLineData);
BOOL     removeCodeEditCharacterByIndex(CODE_EDIT_LINE*  pLineData, CONST UINT  iIndex);
VOID     removeCodeEditLineByIndex(CODE_EDIT_DATA*  pWindowData, CONST UINT  iIndex);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                   CODE EDIT (ITERATOR)
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
CODE_EDIT_ITERATOR*  createCodeEditSingleLineIterator(CONST CODE_EDIT_LINE*  pLineData);
CODE_EDIT_ITERATOR*  createCodeEditMultipleLineIterator(CONST CODE_EDIT_DATA*  pWindowData, CONST CODE_EDIT_LOCATION&  oLocation1, CONST CODE_EDIT_LOCATION&  oLocation2);
VOID                 deleteCodeEditIterator(CODE_EDIT_ITERATOR*  &pIterator);

// Helpers

// Functions
BOOL      findNextCodeEditCharacter(CODE_EDIT_ITERATOR*  pIterator);
//VOID    removeCodeEditIteratorCharacters(CODE_EDIT_ITERATOR*  pIterator);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                 CODE EDIT (IDENTATION)
/// ////////////////////////////////////////////////////////////////////////////////////////

// Helpers
LINE_CLASS  identifyLineClassFromCommandConditional(CONST UINT  iCommandID, CONST CONDITIONAL_ID  eConditional);
BOOL        isCodeEditLineIndentedDueToSkipIf(CONST LIST_ITEM*  pLineItem, CODE_EDIT_LINE*  &pOutput);

// Functions
VOID    calculateCodeEditIndentationForMultipleLines(CODE_EDIT_DATA*  pWindowData, CONST UINT  iStartLine);
INT     calculateCodeEditIndentationForSingleLine(CONST CODE_EDIT_LINE*  pCurrentLine, CONST LIST_ITEM*  pLineIterator);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                   CODE EDIT (SUGGESTIONS)
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
BOOL    createCodeEditSuggestionList(CODE_EDIT_DATA*  pWindowData);
VOID    destroyCodeEditSuggestionList(CODE_EDIT_DATA*  pWindowData);

// Helpers
BOOL    findCodeEditSuggestionByIndex(CODE_EDIT_DATA*  pWindowData, CONST UINT  iIndex, CONST TCHAR*  &szOutput);
BOOL    isCodeObjectSuggestionType(CONST CODEOBJECT*  pCodeObject, CONST SUGGESTION_TYPE  eType);

// Functions
VOID            calculateCodeEditSuggestionOrigin(CODE_EDIT_DATA*  pWindowData, CONST UINT  iLine, CONST UINT  iCharIndex);
UINT            findCodeEditSuggestionByText(CODE_EDIT_DATA*  pWindowData);
AVL_TREE*       generateCodeEditSuggestionTree(CODE_EDIT_DATA*  pWindowData);
SUGGESTION_TYPE identifyCodeEditSuggestionTypeAtLocation(CODE_EDIT_DATA*  pWindowData, CONST UINT  iLine, CONST UINT  iCharIndex);
BOOL            insertCodeEditSuggestionAtCaret(CODE_EDIT_DATA*  pWindowData);
VOID            updateCodeEditSuggestionList(CODE_EDIT_DATA*  pWindowData, CONST UINT  iCharIndex, BOOL  bKeyPress);
VOID            updateCodeEditSuggestionListSize(CODE_EDIT_DATA*  pWindowData);

// Message Handlers
VOID    onCodeEditSuggestionItemRequest(CODE_EDIT_DATA*  pWindowData, CONST UINT  iLogicalIndex, LVITEM*  pItemData);
BOOL    onCodeEditSuggestionKeyDown(CODE_EDIT_DATA*  pWindowData, CONST UINT  iVirtualKey, CONST BOOL  bKeyPress, CONST BOOL  bProcessed);
VOID    onCodeEditSuggestionListKeyDown(CODE_EDIT_DATA*  pWindowData, CONST UINT  iVirtualKey);

// Tree operations
VOID    treeprocBuildCodeEditSuggestionTree(AVL_TREE_NODE*  pCurrentNode, AVL_TREE_OPERATION*  pOperationData);
VOID    treeprocFindCodeEditSuggestionResult(AVL_TREE_NODE*  pCurrentNode, AVL_TREE_OPERATION*  pOperationData);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                  CODE EDIT (TEXT)
/// ////////////////////////////////////////////////////////////////////////////////////////

// Functions
VOID    insertCodeEditCharacterAtCaret(CODE_EDIT_DATA*  pWindowData, CONST TCHAR  chCharacter);
VOID    insertCodeEditScriptFile(CODE_EDIT_DATA*  pWindowData, SCRIPT_FILE*  pScriptFile);
VOID    insertCodeEditTextAtCaret(CODE_EDIT_DATA*  pWindowData, CONST TCHAR*  szText, CONST BOOL bEnableUndo);
VOID    performCodeEditCaretCommand(CODE_EDIT_DATA*  pWindowData, CONST UINT  iMovement, CONST BOOL  bExtendSelection);
VOID    removeCodeEditCharacterAtCaret(CODE_EDIT_DATA*  pWindowData, CONST UINT  iVirtualKey);
VOID    removeCodeEditSelection(CODE_EDIT_DATA*  pWindowData);
VOID    removeCodeEditSelectionText(CODE_EDIT_DATA*  pWindowData, CONST BOOL  bEnableUndo);


/// ////////////////////////////////////////////////////////////////////////////////////////
///                                  CODE EDIT (UNDO)
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
CODE_EDIT_UNDO_ITEM*  createCodeEditUndoItem(CONST TCHAR*  szText, CONST UNDO_ITEM_TYPE  eType, CONST CODE_EDIT_LOCATION*  pStart, CONST CODE_EDIT_LOCATION*  pFinish);
VOID                  deleteCodeEditUndoItem(CODE_EDIT_UNDO_ITEM*  &pCodeEditUndoItem);
VOID                  destructorCodeEditUndoItem(LPARAM  pCodeEditUndoItem);

// Functions
VOID                  performCodeEditUndoAction(CODE_EDIT_DATA*  pWindowData, CODE_EDIT_UNDO_ITEM*  pAction);
VOID                  pushStringToCodeEditUndoStack(CODE_EDIT_DATA*  pWindowData, CONST TCHAR*  szText, CONST UNDO_ITEM_TYPE  eType, CONST CODE_EDIT_LOCATION*  pStart, CONST CODE_EDIT_LOCATION*  pFinish);
VOID                  pushCharacterToCodeEditUndoStack(CODE_EDIT_DATA*  pWindowData, CONST TCHAR  szCharacter, CONST UNDO_ITEM_TYPE  eType, CONST CODE_EDIT_LOCATION*  pStart);

// Message Handlers
VOID                  onCodeEditUndoCommitTimer(CODE_EDIT_DATA*  pWindowData);
VOID                  onCodeEditRedoLastAction(CODE_EDIT_DATA*  pWindowData);
VOID                  onCodeEditUndoLastAction(CODE_EDIT_DATA*  pWindowData);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    COMMENT RATIO CONTROL
/// ////////////////////////////////////////////////////////////////////////////////////////

// FUnctions
COLORREF  identifyCommentRatioColour(CONST UINT  iPercentage);
BOOL      drawGradientBar(HDC  hDC, CONST SIZE*  pSize, CONST COLORREF  clColour);

// Message Handlers
BOOL      onCommentRatioCtrlPaint(HWND  hCtrl, PAINTSTRUCT*  pPaintData, UINT  iPercentage);
LRESULT   wndprocCommentRatioCtrl(HWND  hCtrl, UINT iMessage, WPARAM  wParam, LPARAM  lParam);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                  CUSTOM BUTTON
/// ////////////////////////////////////////////////////////////////////////////////////////

// Helpers
ControlsAPI 
PUSHBUTTONSTATES  identifyCustomButtonState(HWND  hCtrl, CONST UINT  iCustomDrawState);

// Functions
VOID              drawCustomButton(NMCUSTOMDRAW*  pDrawInfo, CONST CUSTOM_BUTTON*  pButtonData, CONST PUSHBUTTONSTATES  eButtonState);

// Message Handlers
ControlsAPI VOID  onCustomDrawButton(HWND  hDialog, NMHDR*  pMessageHeader, CONST IMAGE_TREE_SIZE  eSize, CONST TCHAR*  szIconID, CONST BOOL  bLeftAligned);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                  CUSTOM COMBOBOX
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
CUSTOM_COMBO_ITEM* createCustomComboBoxItem(CONST TCHAR*  szText, CONST TCHAR*  szAuxText, CONST TCHAR*  szIcon, LPARAM  xItemData);
VOID               deleteCustomComboBoxItem(CUSTOM_COMBO_ITEM*  &pItem);

// Helpers
ControlsAPI INT            appendCustomComboBoxItem(HWND  hCtrl, CONST TCHAR*  szText);
ControlsAPI INT            appendCustomComboBoxItemEx(HWND  hCtrl, CONST TCHAR*  szText, CONST TCHAR*  szAuxText, CONST TCHAR*  szIcon, LPARAM  xItemData);
ControlsAPI INT            appendCustomComboBoxFontItem(HWND  hCtrl, CONST TCHAR*  szText, HFONT  hFont);
ControlsAPI INT            appendCustomComboBoxImageListItem(HWND  hCtrl, CONST TCHAR*  szText, HIMAGELIST  hImageList, CONST UINT  iIconIndex, CONST UINT  iIndent, LPARAM  xItemData);
BOOL                       calculateCustomComboBoxSize(MEASUREITEMSTRUCT*  pMeasureData);
ControlsAPI CUSTOM_COMBO_ITEM*  getCustomComboBoxItem(HWND  hCtrl, CONST UINT  iIndex);
ControlsAPI LPARAM         getCustomComboBoxItemParam(HWND  hCtrl, CONST UINT  iIndex);
ControlsAPI CONST TCHAR*   getCustomComboBoxItemText(HWND  hCtrl, CONST UINT  iIndex);
ControlsAPI BOOL           setCustomComboBoxItemColour(HWND  hCtrl, CONST UINT  iIndex, CONST COLORREF  clTextColour);

// Functions
VOID     drawCustomComboBoxItem(DRAWITEMSTRUCT*  pDrawInfo, CONST CUSTOM_COMBO_ITEM*  pItemData);

// Message Handlers
BOOL              onOwnerDrawCustomComboBox(DRAWITEMSTRUCT*  pDrawInfo);
ControlsAPI BOOL  onWindow_MeasureComboBox(MEASUREITEMSTRUCT*  pMeasureData, CONST IMAGE_TREE_SIZE  eControlSize, CONST IMAGE_TREE_SIZE  eDropDownSize);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                  CUSTOM DRAWING
/// ////////////////////////////////////////////////////////////////////////////////////////

// Helpers
VOID                 drawGradientRect(HDC  hDC, CONST RECT*  pRect, CONST COLORREF  clTopColour, CONST COLORREF  clBottomColour, CONST DIRECTION  eDirection);
ControlsAPI HBRUSH   getDialogBackgroundBrush();
ControlsAPI COLORREF getDialogBackgroundColour();

// Functions
ControlsAPI BOOL     drawIcon(HIMAGELIST  hImageList, CONST UINT  iIconIndex, HDC  hDC, CONST UINT  iX, CONST UINT  iY, CONST ICON_STATE  eState);
ControlsAPI BOOL     drawImageTreeIcon(CONST IMAGE_TREE_SIZE  eSize, CONST TCHAR*  szIconID, HDC  hDC, CONST UINT  iX, CONST UINT  iY, CONST ICON_STATE  eState);
ControlsAPI BOOL     drawBlendedIcon(HDC  hDC, HICON  hIcon, CONST UINT  iIconSize, CONST POINT*  ptOutput, CONST COLORREF  clBlendColour, CONST FLOAT  fBlending);
ControlsAPI VOID     drawShadedBar(HDC  hDC, CONST RECT*  pDrawRect, CONST COLORREF  clTopColour, CONST COLORREF  clMiddleColour, CONST COLORREF  clBottomColour, CONST COLORREF  clBorderColour, CONST DIRECTION  eDirection);
ControlsAPI VOID     drawCustomBackgroundRectangle(HDC  hDC, CONST RECT*  pDrawRect);
ControlsAPI VOID     drawCustomHoverRectangle(HDC  hDC, CONST RECT*  pDrawRect);
ControlsAPI VOID     drawCustomSelectionRectangle(HDC  hDC, CONST RECT*  pDrawRect);
ControlsAPI VOID     drawShadedRoundRectangle(HDC  hDC, CONST RECT*  pDrawRect, CONST COLORREF  clTop, CONST COLORREF  clMiddle, CONST COLORREF  clBottom, CONST COLORREF  clBorder, CONST UINT  iWidthPadding);

// Message Handlers
ControlsAPI HBRUSH   onDialog_DrawBackground(HDC  hDC, HWND  hWnd);
ControlsAPI BOOL     onWindow_DeleteItem(DELETEITEMSTRUCT*  pDeleteData);
ControlsAPI BOOL     onWindow_DrawItem(DRAWITEMSTRUCT*  pDrawData);
ControlsAPI BOOL     onWindow_EraseBackground(HWND  hDialog, HDC  hDC, CONST UINT  iControlID);
ControlsAPI BOOL     onWindow_MeasureItem(HWND  hMenuParent, MEASUREITEMSTRUCT*  pMeasureData);
ControlsAPI VOID     onWindow_PaintNonClient(HWND  hWnd, HRGN  hUpdateRegion);

// Window procedure
ControlsAPI INT_PTR  dlgprocVistaStyleDialog(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                  CUSTOM EDIT
/// ////////////////////////////////////////////////////////////////////////////////////////

// Window procedure
ControlsAPI LRESULT  wndprocCustomEditControl(HWND  hWnd, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                  CUSTOM STATIC
/// ////////////////////////////////////////////////////////////////////////////////////////

// Message Handlers
ControlsAPI BOOL     onOwnerDrawStaticImage(DRAWITEMSTRUCT*  pDrawData, CONST TCHAR*  szResource, CONST UINT  iType, CONST UINT  iWidth, CONST UINT  iHeight);
ControlsAPI BOOL     onOwnerDrawStaticText(DRAWITEMSTRUCT*  pDrawData, CONST TCHAR*  szFont, CONST UINT  iPointSize, CONST COLORREF  clText, CONST UINT  iBackgroundColour, CONST BOOL  bBold);

ControlsAPI BOOL     onOwnerDrawStaticTitle(LPARAM  lParam);
ControlsAPI BOOL     onOwnerDrawStaticHeading(LPARAM  lParam);

/// Macro: onOwnerDrawStaticTitle
//  Description: Convenience macro for drawing a Vista-Style dialog title
//
// LPARAM  pDrawData : DRAWITEMSTRUCT* : [in] OwnerDraw data
//
//#define onOwnerDrawStaticTitle(pDrawData)                onOwnerDrawStaticText((DRAWITEMSTRUCT*)(pDrawData), TEXT("Segoe UI Light"), 14, RGB(42,84,153), COLOR_WINDOW, FALSE);
//#define onOwnerDrawStaticTitle(pDrawData)                onOwnerDrawStaticText((DRAWITEMSTRUCT*)(pDrawData), TEXT("Tahoma"), 14, RGB(42,84,153), COLOR_WINDOW, FALSE);

/// Macro: onOwnerDrawStaticHeading
//  Description: Convenience macro for drawing a Vista-Style dialog sub-heading
//
// LPARAM  pDrawData : DRAWITEMSTRUCT* : [in] OwnerDraw data
//
//#define onOwnerDrawStaticHeading(pDrawData)              onOwnerDrawStaticText((DRAWITEMSTRUCT*)(pDrawData), TEXT("Segoe UI Bold"), 9, GetSysColor(COLOR_WINDOWTEXT), COLOR_WINDOW, FALSE); 
//#define onOwnerDrawStaticHeading(pDrawData)              onOwnerDrawStaticText((DRAWITEMSTRUCT*)(pDrawData), TEXT("MS Shell Dlg 2"), 9, GetSysColor(COLOR_WINDOWTEXT), COLOR_WINDOW, TRUE); 

/// Macro: onOwnerDrawStaticIcon
//  Description: Convenience macro for owner-drawing an icon
//
// LPARAM (DRAWITEMSTRUCT*)  pDrawData : [in] OwnerDraw data
// CONST TCHAR*              szIcon    : [in] Icon Resource ID
// CONST UINT                iSize     : [in] Icon size
//
#define onOwnerDrawStaticIcon(pDrawData, szIcon, iSize)             onOwnerDrawStaticImage((DRAWITEMSTRUCT*)(pDrawData), szIcon, IMAGE_ICON, iSize, iSize)

/// Macro: onOwnerDrawStaticBitmap
//  Description: Convenience macro for owner-drawing a bitmap
//
// LPARAM (DRAWITEMSTRUCT*)  pDrawData : [in] OwnerDraw data
// CONST TCHAR*              szBitmap  : [in] Bitmap Resource ID
// CONST UINT                iWidth    : [in] Bitmap width
// CONST UINT                iHeight   : [in] Bitmap Height
//
#define onOwnerDrawStaticBitmap(pDrawData,szBitmap,iWidth,iHeight)  onOwnerDrawStaticImage((DRAWITEMSTRUCT*)(pDrawData), szBitmap, IMAGE_BITMAP, iWidth, iHeight)

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                  CUSTOM COMBOBOX
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
ControlsAPI CUSTOM_TOOLTIP*   createCustomTooltipData(CONST RESULT_TYPE  eType, SUGGESTION_RESULT  xObject);
ControlsAPI VOID              deleteCustomTooltipData(CUSTOM_TOOLTIP*  &pCustomTooltip);

// Functions
ControlsAPI VOID     drawSimpleCustomTooltip(HDC  hDC, RECT*  pOutputRect, CONST UINT  iWidth, CONST TCHAR*  szTitleIcon, CONST RICH_TEXT*  pTitleText, CONST RICH_TEXT*  pDescriptionText, CONST BOOL  bCalculation);

// Message Handlers
ControlsAPI BOOL     onCustomDrawTooltip(HWND  hTooltip, NMTTCUSTOMDRAW*  pHeader, CONST CUSTOM_TOOLTIP*  pTooltipData);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                  CUSTOM LISTVIEW
/// ////////////////////////////////////////////////////////////////////////////////////////

// Helpers
ControlsAPI VOID     initReportModeListView(HWND  hListView, CONST LISTVIEW_COLUMNS*  pListViewData, CONST BOOL  bEnableHover);

// Functions
ControlsAPI VOID     drawCustomListViewItem(HWND  hParent, HWND  hListView, NMLVCUSTOMDRAW*  pHeader);
ControlsAPI BOOL     editCustomListViewItem(HWND  hListView, const UINT  iItem, const UINT  iSubItem, const LISTVIEW_LABEL  eCtrlType);

// Message Handlers
ControlsAPI UINT     onCustomDrawListView(HWND  hParent, HWND  hListView, NMLVCUSTOMDRAW*  pHeader);
ControlsAPI BOOL     onCustomListViewNotify(HWND  hParent, BOOL  bIsDialog, CONST UINT  iListViewID, NMHDR*  pMessage);

// Window Proc
ControlsAPI LRESULT  wndprocCustomListView(HWND  hWnd, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);
LRESULT              wndprocCustomListViewLabel(HWND  hCtrl, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);



/// ////////////////////////////////////////////////////////////////////////////////////////
///                                  CUSTOM MENUS
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
ControlsAPI CUSTOM_MENU*   createCustomMenu(CONST TCHAR*  szMenuResource, CONST BOOL  bPopupMenu, CONST UINT  iSubMenuIndex);
CUSTOM_MENU_ITEM*          createCustomMenuItem(CONST UINT  iItemID, CONST UINT  iIconIndex, CONST BOOL  bIsHeading, CONST BOOL  bIsPopup, CONST BOOL  bIsSeparator);
ControlsAPI VOID           deleteCustomMenu(CUSTOM_MENU*  &pCustomMenu);
VOID                       deleteCustomMenuItemData(CUSTOM_MENU_ITEM*  &pCustomMenuItem);

// Helpers
ControlsAPI CUSTOM_MENU_ITEM*  getCustomMenuItemData(HMENU  hMenu, CONST UINT  iItem, CONST BOOL  bByPosition);

// Functions
ControlsAPI BOOL   appendCustomMenuItem(HMENU  hMenu, CONST UINT  iItemID, CONST UINT  iIconIndex, CONST UINT  iState, CONST TCHAR*  szText);
ControlsAPI BOOL   calculateCustomMenuItemSize(HWND  hParentWnd, MEASUREITEMSTRUCT*  pMeasureData);
ControlsAPI VOID   convertCustomMenuToSystemMenu(HMENU  hCustomMenu);
ControlsAPI VOID   convertSystemMenuToCustomMenu(HMENU  hSystemMenu, CONST BOOL  bPopupMenu);
VOID               drawCustomMenu(DRAWITEMSTRUCT*  pDrawData, CONST CUSTOM_MENU_ITEM*  pItemData);
VOID               drawCustomMenuItem(DRAWITEMSTRUCT*  pDrawData, CONST CUSTOM_MENU_ITEM*  pItemData);
VOID               drawCustomMenuHeading(DRAWITEMSTRUCT*  pDrawData, CONST CUSTOM_MENU_ITEM*  pItemData);
ControlsAPI BOOL   emptyCustomMenu(HMENU  hMenu);
ControlsAPI UINT   identifyCustomMenuIconByID(CONST UINT  iCommandID);
ControlsAPI BOOL   removeCustomMenuItem(HMENU  hMenu, CONST UINT  iItem, CONST BOOL  bByPosition);
ControlsAPI BOOL   setCustomMenuItemText(HMENU  hMenu, CONST UINT  iItem, CONST BOOL  bByPosition, CONST TCHAR*  szFormat, ...);

// Message Handlers
ControlsAPI BOOL   onOwnerDrawCustomMenu(DRAWITEMSTRUCT*  pDrawData);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                               CUSTOM RICH EDIT
/// ////////////////////////////////////////////////////////////////////////////////////////

// Functions
ControlsAPI BOOL              findButtonInRichEditByIndex(HWND  hRichEdit, CONST UINT  iIndex, LANGUAGE_BUTTON* &pOutput);
ControlsAPI BOOL              getRichEditText(HWND  hRichEdit, RICH_TEXT*  pMessage);
ControlsAPI LANGUAGE_BUTTON*  insertRichEditButton(HWND  hRichEdit, CONST TCHAR*  szID, CONST TCHAR*  szText);
ControlsAPI BOOL              modifyButtonInRichEditByIndex(HWND  hRichEdit, const UINT  iIndex, const TCHAR*  szNewText, LANGUAGE_BUTTON*& pOutput);
ControlsAPI BOOL              removeButtonFromRichEditByIndex(HWND  hRichEdit, const UINT  iIndex);
ControlsAPI VOID              setRichEditText(HWND  hRichEdit, CONST RICH_TEXT*  pMessage, const bool  bSkipButtons, CONST GAME_TEXT_COLOUR  eBackground);

// Window Procedures
ControlsAPI LRESULT           wndprocCustomRichEditControl(HWND  hWnd, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    GROUPED LISTVIEW 
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation/Destruction
ControlsAPI LISTVIEW_GROUP*  createGroupedListViewGroup(CONST UINT  iGroupID, CONST UINT  iGroupNameResourceID);

// Handlers
ControlsAPI BOOL             onCustomDraw_GroupedListView(HWND  hParentWnd, HWND  hCtrl, NMLVCUSTOMDRAW*  pMessageData);

// Window procedure
LRESULT CALLBACK             wndprocGroupedListView(HWND  hCtrl, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                    LANGUAGE OLE
///
///   Implementation of OLE intefaces for the buttons displayed in the language message RichEdit
/// ////////////////////////////////////////////////////////////////////////////////////////

/// Implmentation of IRichEditOleCallback
//
class ControlsAPI RichEditCallback : IRichEditOleCallback
{
public:
   RichEditCallback(HWND  hParent) : m_hParent(hParent), m_iRefCount(0)  {};
   ~RichEditCallback() {};

   // IUnknown
   OLE_Declaration(HRESULT) QueryInterface(REFIID  iid, IInterface*  pInterface);
   OLE_Declaration(ULONG)   AddRef(VOID);
   OLE_Declaration(ULONG)   Release(VOID);

   // IRichEditOleCallback
   OLE_Declaration(HRESULT) GetContextMenu(WORD  iSelectionType, IOleObject*  piObject, CHARRANGE*  pSelection, HMENU*  pOutput);
   OLE_Declaration(HRESULT) DeleteObject(IOleObject* pOleObject);
   OLE_Declaration(HRESULT) QueryAcceptData(IDataObject* pDataObject, CLIPFORMAT* lpcfFormat, DWORD reco, BOOL fReally, HGLOBAL hMetaPict)  { return S_OK; };
	OLE_Declaration(HRESULT) QueryInsertObject(CLSID* pCLSID, IStorage* pStorage, LONG cp);

   // Not Implemented
   OLE_Declaration(HRESULT) ContextSensitiveHelp(BOOL fEnterMode)                                                                            { return E_NOTIMPL; };
	OLE_Declaration(HRESULT) GetClipboardData(CHARRANGE* lpchrg, DWORD reco, IDataObject** ppDataObject)                                      { return E_NOTIMPL; };
   OLE_Declaration(HRESULT) GetDragDropEffect(BOOL fDrag, DWORD grfKeyState, DWORD* pdwEffect)                                               { return E_NOTIMPL; };
   OLE_Declaration(HRESULT) GetInPlaceContext(IOleInPlaceFrame** ppFrame, IOleInPlaceUIWindow** ppDoc, LPOLEINPLACEFRAMEINFO lpFrameInfo)    { return E_NOTIMPL; };
   OLE_Declaration(HRESULT) GetNewStorage(IStorage** ppStorage)                                                                              { return E_NOTIMPL; };
	OLE_Declaration(HRESULT) ShowContainerUI(BOOL fShow)                                                                                      { return E_NOTIMPL; };
   
private:
   INT   m_iRefCount;
   HWND  m_hParent;
};

/// Implementation of IDataObject - Based on code by Hani Atassi  (atassi@arabteam2000.com)
//
class ControlsAPI RichEditImage : IDataObject
{
public:
	RichEditImage();
	~RichEditImage();

   // Methods
	IOleObject*               Create(IOleClientSite*  pOleClientSite, IStorage*  pStorage);

	// IUnknown
   OLE_Declaration(HRESULT)  QueryInterface(REFIID  iID, IInterface*  pInterface);
	OLE_Declaration(ULONG)    AddRef(VOID);
	OLE_Declaration(ULONG)    Release(VOID);
   
   // IDataObject
   VOID                      SetBitmap(HBITMAP  hBitmap);
   OLE_Declaration(HRESULT)  GetData(FORMATETC*  pFormat, STGMEDIUM*  pOutput);
   OLE_Declaration(HRESULT)  SetData(FORMATETC*  pFormat, STGMEDIUM*  pData, BOOL  bDataOwner);
   OLE_Declaration(HRESULT)  DAdvise(FORMATETC*  pFormat, DWORD  dwFlags, IAdviseSink*  pOutput, DWORD*  pdwConnectionID);
   OLE_Declaration(HRESULT)  DUnadvise(DWORD dwConnectionID);

	// Not Implemented
	OLE_Declaration(HRESULT)  EnumFormatEtc(DWORD  dwDirection , IEnumFORMATETC**  ppenumFormatEtc )      { return E_NOTIMPL; };
   OLE_Declaration(HRESULT)  EnumDAdvise(IEnumSTATDATA **ppenumAdvise)                                   { return E_NOTIMPL; };
   OLE_Declaration(HRESULT)  GetCanonicalFormatEtc(FORMATETC*  pformatectIn ,FORMATETC* pformatetcOut )  { return E_NOTIMPL; };
   OLE_Declaration(HRESULT)  GetDataHere(FORMATETC* pformatetc, STGMEDIUM*  pmedium)                     { return E_NOTIMPL; };
	OLE_Declaration(HRESULT)  QueryGetData(FORMATETC*  pformatetc )                                       { return E_NOTIMPL; };

private:
   // Data
	INT	        m_iRefCount;
	BOOL	        m_bDataOwner;
	STGMEDIUM     m_oData;
   FORMATETC     m_oFormat;

   // Events
   IDataAdviseHolder*  m_pEvents;
};

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                        MAIN
/// ////////////////////////////////////////////////////////////////////////////////////////

// Helpers
ControlsAPI HINSTANCE  getControlsInstance();

// Functions
ControlsAPI BOOL       registerControlsWindowClasses(ERROR_STACK*  &pError);
ControlsAPI VOID       unregisterControlsWindowClasses();

// Entry Point
BOOL APIENTRY          DllMain(HMODULE  hModule, DWORD  dwPurpose, LPVOID  lpReserved);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                 RICH TEXT (DRAWING)
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction


// Helpers
ControlsAPI COLORREF  calculateVisibleRichTextColour(CONST GAME_TEXT_COLOUR  eColour, CONST GAME_TEXT_COLOUR  eBackground);
GAME_TEXT_COLOUR      identifyGameTextColourFromRGB(CONST COLORREF  clColour);

// Functions
ControlsAPI VOID   drawRichText(HDC  hDC, CONST RICH_TEXT*  pRichText, RECT*  pTargetRect, CONST UINT  iDrawFlags);
ControlsAPI VOID   drawRichTextInSingleLine(HDC  hDC, RECT  rcDrawRect, RICH_TEXT*  pRichText, const BOOL  bDisabled, CONST GAME_TEXT_COLOUR  eBackground);
RICHTEXT_DRAWING   drawRichTextItemInRect(HDC  hDC, CONST TCHAR*  szText, CONST RECT*  pItemRect, UINT*  piCharsOutput, LONG*  piTextWidth, CONST BOOL  bMeasurement);
VOID               drawRichTextItemsInLine(HDC  hDC, CONST RICHTEXT_POSITION*  pStartPos, CONST RECT*  pLineRect, RICHTEXT_POSITION*  pEndPos, INT*  piRemainingWidth, INT*  piLineHeight, CONST BOOL  bMeasurement);
ControlsAPI VOID   drawLanguageMessageInSingleLine(HDC  hDC, RECT  rcDrawRect, LANGUAGE_MESSAGE*  pMessage, const BOOL  bDisabled, CONST GAME_TEXT_COLOUR  eBackground);

/// ////////////////////////////////////////////////////////////////////////////////////////
///                                     WORKSPACE_DATA
/// ////////////////////////////////////////////////////////////////////////////////////////

// Creation / Destruction
PANE*              createSplitterPane(PANE*  pLeftChild, PANE*  pRightChild, CONST RECT  rcPane, CONST DIRECTION  eSplit, CONST BOOL  bFixed);
PANE*              createWindowPane(HWND  hWorkspace, HWND  hWnd, CONST RECT*  pBorderRect);
ControlsAPI HWND   createWorkspace(HWND  hParentWnd, CONST RECT*  pParentRect, HWND  hBaseWnd, CONST UINT  clBackground);
WORKSPACE_DATA*    createWorkspaceData(HWND  hBaseWnd, CONST RECT*  pWorkspaceRect, CONST UINT  clBackground);
VOID               deletePane(PANE*  &pPane);
VOID               deleteWorkspaceData(WORKSPACE_DATA*  &pWindowData);

// Helpers
VOID               calculateSplitterBarRectangle(CONST PANE*  pSplitter, RECT*  pOutput);
BOOL               findSplitterBarPaneAtPoint(PANE*  pCurrentPane, CONST POINT*  ptCursor, PANE*  &pOutput);
ControlsAPI BOOL   findWorkspacePaneByHandle(HWND  hWorkspace, HWND  hFirstWindow, HWND  hSecondWindow, HWND  hThirdWindow, PANE*  &pOutput);
WORKSPACE_DATA*    getWorkspaceWindowData(HWND  hWorkspace);
BOOL               performWorkspacePaneSearch(PANE*  pPane, PANE_SEARCH*  pSearch);
VOID               setWorkspaceClippingRegion(PANE*  pPane, HDC  hDC);
ControlsAPI VOID   setWorkspacePaneProperties(PANE_PROPERTIES*  pProperties, CONST BOOL  bFixed, CONST UINT  iInitialSize, CONST FLOAT  fInitialRatio);
VOID               setWorkspacePaneRedraw(PANE*  pPane, CONST BOOL  bRedraw);
VOID               transferWorkspacePaneChild(WORKSPACE_DATA*  pWindowData, CONST PANE*  pOldChild, PANE*  pParent, PANE*  pNewChild);

// Functions
VOID               drawWorkspaceSplitterBar(HDC  hDC, CONST UINT  iSysColour, PANE*  pPane);
UINT               getWorkspacePaneCount(CONST PANE*  pTargetPane);
ControlsAPI UINT   getWorkspaceWindowSize(HWND  hWorkspace, HWND  hWnd);
ControlsAPI BOOL   insertWorkspaceWindow(HWND  hWorkspace, PANE*  pTargetPane, HWND  hWnd, CONST SIDE  eSide, CONST PANE_PROPERTIES*  pProperties);
ControlsAPI BOOL   removeWorkspaceWindow(HWND  hWorkspace, HWND  hWnd);
VOID               resizeWorkspacePaneRectangles(HDWP  hWindowPositions, PANE*  pTargetPane, CONST BOOL  bMoved);
VOID               resizeWorkspacePane(PANE*  pTargetPane);

// Message Handlers
VOID  onWorkspaceCreate(WORKSPACE_DATA*  pWindowData, HWND  hWnd);
VOID  onWorkspaceDestroy(WORKSPACE_DATA*  pWindowData);
VOID  onWorkspaceMouseDown(WORKSPACE_DATA*  pWindowData, CONST POINT*  ptCursor);
VOID  onWorkspaceMouseMove(WORKSPACE_DATA*  pWindowData, CONST POINT*  ptCursor);
VOID  onWorkspaceMouseUp(WORKSPACE_DATA*  pWindowData, CONST POINT*  ptCursor);
VOID  onWorkspacePaint(WORKSPACE_DATA*  pWindowData, HDC  hDC, CONST RECT*  pUpdateRect);
VOID  onWorkspaceResize(WORKSPACE_DATA*  pWindowData, CONST SIZE*  pNewSize);


// Window Procedure
LRESULT  wndprocWorkspace(HWND  hWnd, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);






