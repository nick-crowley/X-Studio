//
// FILE_NAME.cpp : FILE_DESCRIPTION
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        OVERVIEW
/// /////////////////////////////////////////////////////////////////////////////////////////

// The workspace is a binary tree of rectangle spaces. Each node in the tree represents a rectangle, and has zero or two children.
// Each child node represents a subdivision of it's parent rectangle. Leaves of the tree contain windows, non-leaves are splitter
// nodes that exist to define the split ratio and arrangement of it's children.

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////

// Half the width of a splitter bar
CONST UINT  iBorderWidth = 3;

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createSplitterPane
// Description     : Creates a pane representing a split between two windows
// 
// PANE*            pLeftChild   : [in] Left/Top child pane
// PANE*            pRightChild  : [in] Right/Bottom child pane
// CONST RECT       rcPane       : [in] Entire Pane rectangle
// CONST DIRECTION  eSplit       : [in] Whether to arrange windows vertically or horizontally
// CONST BOOL       bFixed       : [in] Whethet splitter is fixed or adjustable
// 
// Return Value   : New splitter pane, you are responsible for destroying it
// 
PANE*  createSplitterPane(PANE*  pLeftChild, PANE*  pRightChild, CONST RECT  rcPane, CONST DIRECTION  eSplit, CONST BOOL  bFixed)
{
   // Create new object
   PANE*  pSplitter = utilCreateEmptyObject(PANE);
   
   /// Set properties
   pSplitter->eType  = PT_SPLITTER;
   pSplitter->rcPane = rcPane;
   pSplitter->eSplit = eSplit;
   pSplitter->bFixed = bFixed;

   /// Attach children
   pSplitter->pChildren[LEFT]  = pLeftChild;
   pSplitter->pChildren[RIGHT] = pRightChild;

   /// Update parents
   pLeftChild->pParent  = pSplitter;
   pRightChild->pParent = pSplitter;
   
   // Return pane
   return pSplitter;
}


/// Function name  : createWindowPane
// Description     : Creates a pane containing a window
// 
// HWND   hWorkspace  : [in] Workspace
// HWND   hWnd        : [in] Window handle
// 
// Return Value   : New window pane, you are responsible for destroying it
// 
PANE*  createWindowPane(HWND  hWorkspace, HWND  hWnd, CONST RECT*  pBorderRect)
{
   // Create new object
   PANE*  pWindow = utilCreateEmptyObject(PANE);

   /// Set properties
   pWindow->hWnd  = hWnd;
   pWindow->eType = PT_WINDOW;
   pWindow->bNew  = TRUE;

   // [BORDER] Set internal border, if any
   if (pBorderRect)
      pWindow->rcBorder = *pBorderRect;

   /// Take ownership of window
   SetParent(hWnd, hWorkspace);

   // [DEBUG] Get window name
   GetWindowText(hWnd, pWindow->szName, 64);

   // Return pane
   return pWindow;
}


/// Function name  : createWorkspace
// Description     : Creates the workspace window and attaches a 'base' window
// 
// HWND         hParentWnd   : [in] Parent window
// CONST RECT*  pParentRect  : [in] Workspace rectangle in parent client co-ordinates
// HWND         hBaseWnd     : [in] 'Base' window
// CONST UINT   clBackground : [in] System colour index for the background (splitter bars)
// 
// Return Value   : Workspace window if successful, otherwise NULL
// 
ControlsAPI
HWND  createWorkspace(HWND  hParentWnd, CONST RECT*  pParentRect, HWND  hBaseWnd, CONST COLORREF  clBackground)
{
   WORKSPACE_DATA*  pWindowData;    // Window data
   SIZE             siParentSize;   // Parent rectangle size
   HWND             hWnd;           // Workspace window handle
   
   // Prepare
   utilConvertRectangleToSize(pParentRect, &siParentSize);

   /// Create window data
   pWindowData = createWorkspaceData(hBaseWnd, pParentRect, clBackground);

   /// Create window, pass window data as parameter
   hWnd = CreateWindowEx(NULL, szWorkspaceClass, TEXT("Workspace"), WS_CHILD, pParentRect->left, pParentRect->top, siParentSize.cx, siParentSize.cy, hParentWnd, NULL, getControlsInstance(), (VOID*)pWindowData);
   ERROR_CHECK("creating workspace", hWnd);

   // [CHECK] Display window
   if (hWnd)
   {
      // Display window
      ShowWindow(hWnd, SW_SHOWNORMAL);
      UpdateWindow(hWnd);

      // [DEBUG]
      DEBUG_WINDOW("Workspace", hWnd);
   }

   // Return window handle
   return hWnd;
}


/// Function name  : createWorkspaceData
// Description     : Creates workspace window data
// 
// HWND         hBaseWnd       : [in] The 'base' window
// CONST RECT*  pWorkspaceRect : [in] Workspace rectangle, in parent client co-ordinates
// CONST UINT   clBackground   : [in] System colour index for the background (splitter bars)
// 
// Return Value   : New window data, you are responsible for destroying it
// 
WORKSPACE_DATA*  createWorkspaceData(HWND  hBaseWnd, CONST RECT*  pWorkspaceRect, CONST COLORREF  clBackground)
{
   // Create new object
   WORKSPACE_DATA*  pWindowData = utilCreateEmptyObject(WORKSPACE_DATA);

   /// Store base window and workspace rectangle
   pWindowData->hBaseWnd    = hBaseWnd;
   pWindowData->rcWorkspace = (*pWorkspaceRect);
   pWindowData->hBackground = CreateSolidBrush(clBackground);

   // Return window data
   return pWindowData;
}


/// Function name  : deletePane
// Description     : Deletes a workspace pane
// 
// PANE*  &pPane   : [in] Pane to destroy
// 
VOID  deletePane(PANE*  &pPane)
{
   /// [LEFT] Delete left sub-panes
   if (pPane->pChildren[LEFT])
      deletePane(pPane->pChildren[LEFT]);

   /// [RIGHT] Delete right sub-panes
   if (pPane->pChildren[RIGHT])
      deletePane(pPane->pChildren[RIGHT]);

   /// Destroy pane window
   utilDeleteWindow(pPane->hWnd);

   // Delete pane object
   utilDeleteObject(pPane);
}

/// Function name  : deleteWorkspaceData
// Description     : Deletes workspace window data and any connected panes 
// 
// WORKSPACE_DATA*  &pWindowData : [in] Window data
// 
VOID  deleteWorkspaceData(WORKSPACE_DATA*  &pWindowData)
{
   /// Delete panes
   deletePane(pWindowData->pBasePane);

   // Delete workspace
   DeleteBrush(pWindowData->hBackground);
   utilDeleteObject(pWindowData);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : calculateSplitterBarRectangle
// Description     : Calculates the splitter bar rectangle of a splitter pane
// 
// CONST PANE*  pSplitter : [in]     Splitter pane
// RECT*        pOutput   : [in/out] Splitter bar rectangle
// 
VOID  calculateSplitterBarRectangle(CONST PANE*  pSplitter, RECT*  pOutput)
{
   RECT *pFirstChildRect,     // Pane rectangle of left/top child
        *pSecondChildRect;    // Pane rectangle of right/bottom child

   // [CHECK] Ensure pane is a splitter
   if (pSplitter->eType == PT_SPLITTER)
   {
      // Prepare
      pFirstChildRect  = &pSplitter->pChildren[LEFT]->rcPane;
      pSecondChildRect = &pSplitter->pChildren[RIGHT]->rcPane;

      // Calculate gap rectangle
      switch (pSplitter->eSplit)
      {
      case HORIZONTAL:  SetRect(pOutput, pFirstChildRect->right, pFirstChildRect->top, pSecondChildRect->left, pSecondChildRect->bottom);   break;
      case VERTICAL:    SetRect(pOutput, pFirstChildRect->left, pFirstChildRect->bottom, pSecondChildRect->right, pSecondChildRect->top);   break;
      }
   }
}


/// Function name  : findSplitterBarPaneAtPoint
// Description     : Checks whether the mouse is over the gap between splitter child windows, and returns the pane if so
// 
// PANE*         pCurrentPane : [in]  Pane currently being examined. This should be the root on initial call
// CONST POINT*  ptCursor     : [in]  Cursor position to test, in workspace client co-ordinates
// PANE*        &pOutput      : [out] Desired pane if any, otherwise NULL
// 
// Return Value   : TRUE if found, otherwise FALSE
// 
BOOL  findSplitterBarPaneAtPoint(PANE*  pCurrentPane, CONST POINT*  ptCursor, PANE*  &pOutput)
{
   RECT  rcSplitterGap;       // Splitter bar rectangle generated from the gap between child panes   

   // Prepare
   pOutput = NULL;

   // [CHECK] Ensure pane is a splitter
   if (pCurrentPane->eType == PT_SPLITTER)
   {
      // Calculate gap rectangle
      calculateSplitterBarRectangle(pCurrentPane, &rcSplitterGap);
      
      // [CHECK] Determine result
      if (PtInRect(&rcSplitterGap, *ptCursor))
         /// [SUCCESS] Set result
         pOutput = pCurrentPane;
   }

   /// [CHECK] Recurse left (if necessary)
   if (!pOutput AND pCurrentPane->pChildren[LEFT])
      findSplitterBarPaneAtPoint(pCurrentPane->pChildren[LEFT], ptCursor, pOutput);

   /// [CHECK] Recurse right (if necessary)
   if (!pOutput AND pCurrentPane->pChildren[RIGHT])
      findSplitterBarPaneAtPoint(pCurrentPane->pChildren[RIGHT], ptCursor, pOutput);

   // Return TRUE if found
   return (pOutput != NULL);
}


/// Function name  : findWorkspacePaneByHandle
// Description     : Retrieves the pane containing the window(s)
// 
// HWND    hWorkspace     : [in]           Workspace
// HWND    hFirstWindow   : [in]           First window
// HWND    hSecondWindow  : [in][optional] Second window
// HWND    hThirdWindow   : [in][optional] Third window
// PANE*  &pOutput        : [out]          Pane if found, otherwise NULL
// 
// Return Value   : TRUE if found, otherwise FALSE
// 
ControlsAPI
BOOL  findWorkspacePaneByHandle(HWND  hWorkspace, HWND  hFirstWindow, HWND  hSecondWindow, HWND  hThirdWindow, PANE*  &pOutput)
{
   WORKSPACE_DATA*  pWindowData;
   PANE_SEARCH      oSearchData;

   // [CHECK]
   ASSERT(hFirstWindow);

   // Prepare
   pWindowData = getWorkspaceWindowData(hWorkspace);
   utilZeroObject(&oSearchData, PANE_SEARCH);
   
   // Setup search
   oSearchData.hWindows[0] = hFirstWindow;
   oSearchData.hWindows[1] = (hSecondWindow ? hSecondWindow : hThirdWindow);
   oSearchData.hWindows[2] = (hSecondWindow AND hThirdWindow ? hThirdWindow : NULL);

   // Count window handles
   for (UINT iWindow = 0; iWindow < 3; iWindow++)
   {
      if (oSearchData.hWindows[iWindow])
         oSearchData.iCount++;
   }

   /// Perform search from base pane
   performWorkspacePaneSearch(pWindowData->pBasePane, &oSearchData);
      
   // Set result (or NULL) and return TRUE if found
   return (pOutput = oSearchData.pOutput) != NULL;
}


/// Function name  : getWorkspaceWindowData
// Description     : Retrieves workspace window data
// 
// HWND  hWorkspace : [in] Workspace
// 
// Return Value   : Window data
// 
WORKSPACE_DATA*   getWorkspaceWindowData(HWND  hWorkspace)
{
   return (WORKSPACE_DATA*)GetWindowLong(hWorkspace, 0);
}


/// Function name  : performWorkspacePaneSearch
// Description     : Recursively searches the workspace for a single window or the lowest splitter containing two or more windows
// 
// PANE*         pPane   : [in]     Current pane (should be base pane on first call)
// PANE_SEARCH*  pSearch : [in/out] Search data. Target pane is returned in 'pOutput'
// 
// Return Value   : TRUE if found, otherwise FALSE
// 
BOOL  performWorkspacePaneSearch(PANE*  pPane, PANE_SEARCH*  pSearch)
{
   // Iterate through target windows
   for (UINT  i = 0; i < pSearch->iCount; i++)
      /// [CHECK] Is current pane a target window?
      if (pPane->hWnd == pSearch->hWindows[i])
      {
         /// [SINGLE WINDOW] Return this window pane
         if (pSearch->iCount == 1)
            pSearch->pOutput = pPane;
         
         // [SUCCESS] Increment match count.  Return TRUE if pane was found.
         pSearch->iMatches++;
         return (pSearch->pOutput != NULL);
      }
   
   /// [RECURSE] Recurse into left child
   if (pPane->pChildren[LEFT])
      performWorkspacePaneSearch(pPane->pChildren[LEFT], pSearch);

   /// [RECURSE] Recurse into right child
   if (pPane->pChildren[RIGHT])
      performWorkspacePaneSearch(pPane->pChildren[RIGHT], pSearch);
   
   // [CHECK] Is this the immediate nearest parent of all target windows?
   if (!pSearch->pOutput AND pSearch->iMatches == pSearch->iCount)
      /// [MULTIPLE WINDOWS] Return this splitter pane
      pSearch->pOutput = pPane;

   // Return TRUE if all windows were found
   return (pSearch->pOutput != NULL);
}


/// Function name  : setWorkspaceBackgroundColour
// Description     : Changes the workspace background colour
// 
// HWND            hWorkspace   : [in] Workspace
// const COLORREF  clBackground : [in] Colour
// 
ControlsAPI 
VOID  setWorkspaceBackgroundColour(HWND  hWorkspace, const COLORREF  clBackground)
{
   WORKSPACE_DATA*   pWindowData = getWorkspaceWindowData(hWorkspace);

   /// Replace existing background brush
   if (pWindowData)
   {
      DeleteBrush(pWindowData->hBackground);
      pWindowData->hBackground = CreateSolidBrush(clBackground);
   }
}


/// Function name  : setWorkspaceClippingRegion
// Description     : Adds all panes in the workspace to a Device Context's clipping region
// 
// PANE*  pPane : [in] Current pane. Should be base pane on first call
// HDC    hDC   : [in] Painting Device Context
// 
VOID  setWorkspaceClippingRegion(PANE*  pPane, HDC  hDC)
{
   // [CHECK] Is pane a window pane?
   if (pPane->hWnd)
      /// Add to clipping rectangle
      ExcludeClipRect(hDC, pPane->rcPane.left, pPane->rcPane.top, pPane->rcPane.right, pPane->rcPane.bottom);

   /// [CHECK] Recurse left if possible
   if (pPane->pChildren[LEFT])
      setWorkspaceClippingRegion(pPane->pChildren[LEFT], hDC);

   /// [CHECK] Recurse right if possible
   if (pPane->pChildren[RIGHT])
      setWorkspaceClippingRegion(pPane->pChildren[RIGHT], hDC);
}


/// Function name  : setWorkspacePaneProperties
// Description     : Convenience function for assembling pane properties
// 
// PANE_PROPERTIES*  pProperties   : [in/out]       Properties object
// CONST BOOL        bFixed        : [in]           Whether split is fixed or adjustable
// CONST UINT        iInitialSize  : [in][optional] Initial size in pixels. Must be provided is fInitialRatio is NULL
// CONST FLOAT       fInitialRatio : [in][optional] Initial ratio between 0 and 1. Must be provided if iInitialSize is NULL
// 
ControlsAPI
VOID  setWorkspacePaneProperties(PANE_PROPERTIES*  pProperties, CONST BOOL  bFixed, CONST UINT  iInitialSize, CONST FLOAT  fInitialRatio)
{
   // [CHECK] Ensure size or ratio is provided
   ASSERT(iInitialSize OR fInitialRatio);

   // Prepare
   utilZeroObject(pProperties, PANE_PROPERTIES);

   // Set properties
   pProperties->bFixed        = bFixed;
   pProperties->iInitialSize  = iInitialSize;
   pProperties->fInitialRatio = fInitialRatio;
}


/// Function name  : setWorkspacePaneRedraw
// Description     : Enables/Disables window redrawing on all window panes in the workspace
// 
// PANE*       pPane   : [in] Current pane, should be the base pane on first call
// CONST BOOL  bRedraw : [in] Whether to enable/disable redraw
// 
VOID  setWorkspacePaneRedraw(PANE*  pPane, CONST BOOL  bRedraw)
{
   // [CHECK] Is pane a window pane?
   if (pPane->hWnd)
   {
      /// Enable/Disable window pane
      SetWindowRedraw(pPane->hWnd, bRedraw);

      // [ENABLE] Update window
      if (bRedraw)
         utilRedrawWindow(pPane->hWnd);
   }

   /// Set left/right child redraw
   if (pPane->pChildren[LEFT])
      setWorkspacePaneRedraw(pPane->pChildren[LEFT], bRedraw);

   if (pPane->pChildren[RIGHT])
      setWorkspacePaneRedraw(pPane->pChildren[RIGHT], bRedraw);
}


/// Function name  : transferWorkspacePaneChild
// Description     : Attaches a child pane to a different parent (or the root)
// 
// WORKSPACE_DATA*  pWindowData : [in] Window data
// CONST PANE*      pOldChild   : [in] Current child
// PANE*            pParent     : [in] Parent to attach to
// PANE*            pNewChild   : [in] New child for the parent
// 
VOID  transferWorkspacePaneChild(WORKSPACE_DATA*  pWindowData, CONST PANE*  pOldChild, PANE*  pParent, PANE*  pNewChild)
{
   SIDE  eLinkage;      // Existing child <-> parent linkage

   /// [ROOT] Update workspace root
   if (!pParent)
   {
      pWindowData->pBasePane = pNewChild;
      pNewChild->pParent     = NULL;
   }
   /// [NOT ROOT] Attach new pane <-> new parent
   else
   {
      // [CHECK] Determine whether to add as left or right child
      eLinkage = (pParent->pChildren[LEFT] == pOldChild ? LEFT : RIGHT);

      // Attach new child <-> parent
      pNewChild->pParent           = pParent;
      pParent->pChildren[eLinkage] = pNewChild;
   }
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : setWorkspaceClippingRegion
// Description     : Adds all panes in the workspace to a Device Context's clipping region
// 
// HDC         hDC        : [in] Painting Device Context
// CONST UINT  iSysColour : [in] System colour index
// PANE*       pPane      : [in] Current pane. Should be base pane on first call
// 
/// RESCINDED: In favour of clipping child windows and blanketing window
//
VOID  drawWorkspaceSplitterBar(HDC  hDC, CONST UINT  iSysColour, PANE*  pPane)
{
   RECT  rcSplitterBar;

   // [CHECK] Is this a splitter pane?
   if (pPane->eType == PT_SPLITTER)
   {
      // Prepare
      calculateSplitterBarRectangle(pPane, &rcSplitterBar);

      /// Draw splitter bar using background colour
      utilFillSysColourRect(hDC, &rcSplitterBar, iSysColour);
   }

   /// [CHECK] Recurse left if possible
   if (pPane->pChildren[LEFT])
      drawWorkspaceSplitterBar(hDC, iSysColour, pPane->pChildren[LEFT]);

   /// [CHECK] Recurse right if possible
   if (pPane->pChildren[RIGHT])
      drawWorkspaceSplitterBar(hDC, iSysColour, pPane->pChildren[RIGHT]);
}


/// Function name  : getWorkspacePaneCount
// Description     : Calculates the number of panes within a workspace pane
// 
// CONST PANE*  pTargetPane : [in] Target pane
// 
// Return Value   : 1-based number of panes
// 
UINT  getWorkspacePaneCount(CONST PANE*  pTargetPane)
{
   UINT  iCount;

   // Prepare
   iCount = 1;
   
   // [LEFT] Recurse into left child
   if (pTargetPane->pChildren[LEFT])
      iCount += getWorkspacePaneCount(pTargetPane->pChildren[LEFT]);

   // [RIGHT] Recurse into right child
   if (pTargetPane->pChildren[RIGHT])
      iCount += getWorkspacePaneCount(pTargetPane->pChildren[RIGHT]);

   // Return count
   return iCount;
}


/// Function name  : getWorkspaceWindowSize
// Description     : Retrieves the current split ratio of a workspace window, in pixels
// 
// HWND  hWorkspace  : [in] Workspace
// HWND  hWnd        : [in] Window to measure
// 
// Return Value   : Split window ratio in pixels if window is found, otherwise NULL
// 
ControlsAPI 
UINT  getWorkspaceWindowSize(HWND  hWorkspace, HWND  hWnd)
{
   WORKSPACE_DATA  *pWindowData;    // Window data
   PANE            *pWindowPane,    // Pane containing the window
                   *pSplitterPane;  // Parent pane containing the splitter
   FLOAT            fSplitterRatio; // Size ratio for the window pane
   SIZE             siSplitterPane; // Size of the splitter pane
   UINT             iOutput;        // Window size
   
   // Prepare
   pWindowData = getWorkspaceWindowData(hWorkspace);
   iOutput     = NULL;

   /// [CHECK] Search for window pane.  Ensure window is not the base.
   if (pWindowData->pBasePane->hWnd != hWnd AND findWorkspacePaneByHandle(hWorkspace, hWnd, NULL, NULL, pWindowPane))
   {
      // Prepare
      pSplitterPane = pWindowPane->pParent;
      utilConvertRectangleToSize(&pSplitterPane->rcPane, &siSplitterPane);

      // Determine window pane ratio
      fSplitterRatio = (pWindowPane == pSplitterPane->pChildren[LEFT] ? pSplitterPane->fRatio : 1.0f - pSplitterPane->fRatio);

      /// Convert ratio into pixels
      switch (pSplitterPane->eSplit)
      {
      case HORIZONTAL:  iOutput = (UINT)(fSplitterRatio * (FLOAT)siSplitterPane.cx);  break;
      case VERTICAL:    iOutput = (UINT)(fSplitterRatio * (FLOAT)siSplitterPane.cy);  break;
      }
   }

   // Return size
   return iOutput;
}


/// Function name  : insertWorkspaceWindow
// Description     : Inserts a window into the workspace
// 
// HWND                    hWorkspace   : [in] Workspace
// PANE*                   pTargetPane  : [in] Existing pane to split
// HWND                    hWnd         : [in] New window handle
// CONST SIDE              eSide        : [in] Side to add the new window to
// CONST PANE_PROPERTIES*  pProperties  : [in] Size of the new window, in pixels
//
// Return Value : TRUE if workspace and target pane exist, otherwise FALSE
// 
ControlsAPI 
BOOL  insertWorkspaceWindow(HWND  hWorkspace, PANE*  pTargetPane, HWND  hWnd, CONST SIDE  eSide, CONST PANE_PROPERTIES*  pProperties)
{
   WORKSPACE_DATA*  pWindowData;    // Window data
   PANE            *pSplitterPane,  // New Splitter pane to hold both window panes
                   *pWindowPane,    // New window pane
                   *pTargetParent;  // Preserves the parent of the target pane (altered by createSplitterPane(..))
   SIZE             siSplitterPane; // Size of the splitter pane

   // [CHECK] Ensure window is invisible
   ASSERT(IsWindow(hWnd) AND !IsWindowVisible(hWnd));

   // [CHECK] Ensure Workspace and target exist
   if (!pTargetPane OR !hWorkspace)
   {
      CONSOLE_ERROR("Workspace pane not found");
      return FALSE;
   }

   // Prepare
   pWindowData = getWorkspaceWindowData(hWorkspace);
   utilConvertRectangleToSize(&pTargetPane->rcPane, &siSplitterPane);

   // Preserve parent
   pTargetParent = pTargetPane->pParent;

   /// Create new window pane
   pWindowPane = createWindowPane(pWindowData->hWnd, hWnd, &pProperties->rcBorder);

   /// Create new splitter pane containing both panes as children
   switch (eSide)
   {
   // [LEFT/TOP] Window in the 'side' child
   case LEFT:   pSplitterPane = createSplitterPane(pWindowPane, pTargetPane, pTargetPane->rcPane, HORIZONTAL, pProperties->bFixed);  break;
   case TOP:    pSplitterPane = createSplitterPane(pWindowPane, pTargetPane, pTargetPane->rcPane, VERTICAL, pProperties->bFixed);    break;
   // [TOP/BOTTOM] Window in the 'opposite' child. Window size is inverted
   case RIGHT:  pSplitterPane = createSplitterPane(pTargetPane, pWindowPane, pTargetPane->rcPane, HORIZONTAL, pProperties->bFixed);  break;
   case BOTTOM: pSplitterPane = createSplitterPane(pTargetPane, pWindowPane, pTargetPane->rcPane, VERTICAL, pProperties->bFixed);    break;
   }   

   /// [FIXED SIZE] Store side and size
   if (pProperties->bFixed AND pProperties->iInitialSize)
   {
      pSplitterPane->iFixedSize = pProperties->iInitialSize;
      pSplitterPane->eFixedSide = eSide;
   }
   /// [RATIO] Store initial ratio
   else if (pProperties->fInitialRatio)
      pSplitterPane->fRatio = max(0.05f, min(pProperties->fInitialRatio, 0.95f));
   
   /// [ADJUSTABLE] Calculate initial ratio
   else 
   {
      switch (eSide)
      {
      case LEFT:   pSplitterPane->fRatio = (FLOAT)pProperties->iInitialSize / (FLOAT)siSplitterPane.cx;          break;
      case TOP:    pSplitterPane->fRatio = (FLOAT)pProperties->iInitialSize / (FLOAT)siSplitterPane.cy;          break;
      case RIGHT:  pSplitterPane->fRatio = 1.0f - (FLOAT)pProperties->iInitialSize / (FLOAT)siSplitterPane.cx;   break;
      case BOTTOM: pSplitterPane->fRatio = 1.0f - (FLOAT)pProperties->iInitialSize / (FLOAT)siSplitterPane.cy;   break;
      }
      // [CHECK] Ensure splitter ratio
      pSplitterPane->fRatio = max(0.05f, min(pSplitterPane->fRatio, 0.95f));
   }

   /// Attach splitter to parent
   transferWorkspacePaneChild(pWindowData, pTargetPane, pTargetParent, pSplitterPane);

   // Resize children according to ratio
   resizeWorkspacePane(pSplitterPane);

   // Show window
   ShowWindow(hWnd, SW_SHOWNORMAL);
   UpdateWindow(hWnd);

   // [DEBUG]
   DEBUG_WINDOW_VAR(pWindowPane->szName, hWnd);

   // [SUCCESS] Return TRUE
   return TRUE;
}

 
/// Function name  : removeWorkspaceWindow
// Description     : Removes and destroys a window in the workspace
// 
// HWND  hWorkspace : [in] Workspace
// HWND  hWnd       : [in] Window to be removed
// 
// Return Value   : TRUE if successful, FALSE if the window was not found or is the only remaining window
// 
ControlsAPI
BOOL  removeWorkspaceWindow(HWND  hWorkspace, HWND  hWnd)
{
   WORKSPACE_DATA  *pWindowData;    // Window data
   PANE            *pTargetPane,    // Target pane to remove
                   *pSplitter,      // Splitter pane being destroyed
                   *pOppositePane;  // Pane opposite the target pane
   SIDE             eOpposite;      // Identifies the opposite pane

   // [CHECK] Ensure Workspace and target exist
   if (!hWorkspace OR !hWnd)
      return FALSE;

   // Prepare
   pWindowData = getWorkspaceWindowData(hWorkspace);

   /// [CHECK] Lookup target pane and ensure it's not the root
   if (findWorkspacePaneByHandle(hWorkspace, hWnd, NULL, NULL, pTargetPane) AND (pSplitter = pTargetPane->pParent))
   {
      // Identify opposite pane
      eOpposite     = (pSplitter->pChildren[LEFT] == pTargetPane ? RIGHT : LEFT);
      pOppositePane = pSplitter->pChildren[eOpposite];

      /// Attach opposite pane to the splitter's parent  (or root)
      transferWorkspacePaneChild(pWindowData, pSplitter, pSplitter->pParent, pOppositePane);

      // Resize opposite pane to cover splitter
      pOppositePane->rcPane = pSplitter->rcPane;
      resizeWorkspacePane(pOppositePane);

      /// Sever oppposite pane, delete target + splitter
      pSplitter->pChildren[eOpposite] = NULL;
      deletePane(pSplitter);
      
      // [SUCCESS] Return TRUE
      return TRUE;
   }

   // [FAILED] Window not found / Window was root
   CONSOLE_ERROR("Workspace pane not found");
   return FALSE;
}


/// Function name  : resizeWorkspacePaneRectangles
// Description     : Resizes the child windows of a pane and any of it's descendants
///                          Update the rectangle of the pane before calling 
// 
// PANE*  pTargetPane   : [in] Pane to resize
// 
VOID   resizeWorkspacePane(PANE*  pTargetPane)
{
   HDWP  hWindowPositions;

   // Prepare
   hWindowPositions = BeginDeferWindowPos(getWorkspacePaneCount(pTargetPane));

   /// Recalculate pane and descendants positions
   resizeWorkspacePaneRectangles(hWindowPositions, pTargetPane, TRUE);

   // Perform batch resize
   EndDeferWindowPos(hWindowPositions);
}


/// Function name  : resizeWorkspacePaneRectangles
// Description     : Resizes the child windows of a pane and any of it's descendants
///                          Update the rectangle of the pane before calling 
// 
// HDWP   hWindowPositions : [in] Multiple window positioning
// PANE*  pTargetPane      : [in] Pane to resize
// 
VOID  resizeWorkspacePaneRectangles(HDWP  hWindowPositions, PANE*  pTargetPane, CONST BOOL  bMoved)
{
   PANE*  pChild;          // Child being updated
   SIZE   siTargetPane;    // Size of the target pane
   POINT  ptOld;
   
   // Prepare
   utilConvertRectangleToSize(&pTargetPane->rcPane, &siTargetPane);

   // [LEFT CHILD] Recalculate child's rectangle from pane and ratio
   if (pChild = pTargetPane->pChildren[LEFT])
   {
      // Copy rectangle from pane
      utilConvertRectangleToPoint(&pChild->rcPane, &ptOld);
      pChild->rcPane = pTargetPane->rcPane;
      
      // [CHECK] Is one pane a fixed size?
      if (pTargetPane->bFixed AND pTargetPane->iFixedSize)
      {
         /// [FIXED] Recalculate left/top pane using the fixed size (Always ensure border is external so fixed size is exact)
         switch (pTargetPane->eFixedSide)
         {
         case LEFT:    pChild->rcPane.right  -= (siTargetPane.cx - pTargetPane->iFixedSize);    break;
         case RIGHT:   pChild->rcPane.right  -= (pTargetPane->iFixedSize + iBorderWidth * 2);   break;
         case TOP:     pChild->rcPane.bottom -= (siTargetPane.cy - pTargetPane->iFixedSize);    break;
         case BOTTOM:  pChild->rcPane.bottom -= (pTargetPane->iFixedSize + iBorderWidth * 2);   break;
         }
      }
      /// [RATIO] Recalculate left/top pane using the splitter ratio
      else switch (pTargetPane->eSplit)
      {
      case HORIZONTAL: pChild->rcPane.right  -= (LONG)((1.0f - pTargetPane->fRatio) * siTargetPane.cx) + iBorderWidth;   break;
      case VERTICAL:   pChild->rcPane.bottom -= (LONG)((1.0f - pTargetPane->fRatio) * siTargetPane.cy) + iBorderWidth;   break;
      }

      // [INTERNAL BORDER] Shrink pane
      if (pChild->rcBorder.left OR pChild->rcBorder.top OR pChild->rcBorder.right OR pChild->rcBorder.bottom)
         SetRect(&pChild->rcPane, pChild->rcPane.left + pChild->rcBorder.left, pChild->rcPane.top + pChild->rcBorder.top, pChild->rcPane.right - pChild->rcBorder.right, pChild->rcPane.bottom - pChild->rcBorder.bottom);

      /// [RECURSE] Recursively update any child panes
      resizeWorkspacePaneRectangles(hWindowPositions, pChild, pChild->bNew OR (pChild->rcPane.left != ptOld.x) OR (pChild->rcPane.top != ptOld.y));
   }

   // [RIGHT CHILD] Recalculate child's rectangle from pane and ratio
   if (pChild = pTargetPane->pChildren[RIGHT])
   {
      // Copy rectangle from pane
      utilConvertRectangleToPoint(&pChild->rcPane, &ptOld);
      pChild->rcPane = pTargetPane->rcPane;

      // [CHECK] Is one pane a fixed size?
      if (pTargetPane->bFixed AND pTargetPane->iFixedSize)
      {
         /// [FIXED] Recalculate right/bottom pane using the fixed size (Always ensure border is external so fixed size is exact)
         switch (pTargetPane->eFixedSide)
         {
         case LEFT:    pChild->rcPane.left += (pTargetPane->iFixedSize + iBorderWidth * 2);  break;
         case RIGHT:   pChild->rcPane.left += (siTargetPane.cx - pTargetPane->iFixedSize);   break;
         case TOP:     pChild->rcPane.top  += (pTargetPane->iFixedSize + iBorderWidth * 2);  break;
         case BOTTOM:  pChild->rcPane.top  += (siTargetPane.cy - pTargetPane->iFixedSize);   break;
         }
      }
      /// [RATIO] Recalculate right/bottom pane using the splitter ratio
      else switch (pTargetPane->eSplit)
      {
      case HORIZONTAL: pChild->rcPane.left += (LONG)(pTargetPane->fRatio * siTargetPane.cx) + iBorderWidth;   break;
      case VERTICAL:   pChild->rcPane.top  += (LONG)(pTargetPane->fRatio * siTargetPane.cy) + iBorderWidth;   break;
      }

      // [INTERNAL BORDER] Shrink pane
      if (pChild->rcBorder.left OR pChild->rcBorder.top OR pChild->rcBorder.right OR pChild->rcBorder.bottom)
         SetRect(&pChild->rcPane, pChild->rcPane.left + pChild->rcBorder.left, pChild->rcPane.top + pChild->rcBorder.top, pChild->rcPane.right - pChild->rcBorder.right, pChild->rcPane.bottom - pChild->rcBorder.bottom);

      /// [RECURSE] Recursively update any child panes
      resizeWorkspacePaneRectangles(hWindowPositions, pChild, pChild->bNew OR (pChild->rcPane.left != ptOld.x) OR (pChild->rcPane.top != ptOld.y));
   }

   /// [WINDOW] Resize window
   utilDeferClientRect(hWindowPositions, pTargetPane->hWnd, &pTargetPane->rcPane, bMoved, TRUE);
   pTargetPane->bNew = FALSE;
}


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onWorkspaceCreate
// Description     : Intialises the window data
// 
// WORKSPACE_DATA*  pWindowData : [in] Window data
// HWND             hWnd        : [in] Workspace
// 
VOID  onWorkspaceCreate(WORKSPACE_DATA*  pWindowData, HWND  hWnd)
{
   PANE*  pBasePane;    // Convenience pointer

   /// Store window handle and data
   SetWindowLong(hWnd, 0, (LONG)pWindowData);
   pWindowData->hWnd = hWnd;

   /// Create base pane and take ownership
   pBasePane = createWindowPane(pWindowData->hWnd, pWindowData->hBaseWnd, NULL);
   
   /// Stretch base pane over entire workspace
   GetClientRect(pWindowData->hWnd, &pBasePane->rcPane);
   utilSetWindowRect(pBasePane->hWnd, &pBasePane->rcPane, TRUE);

   // Store base pane
   pWindowData->pBasePane = pBasePane;
}


/// Function name  : onWorkspaceDestroy
// Description     : Destroys the window data
// 
// WORKSPACE_DATA* pWindowData   : [in] Window data
// 
VOID  onWorkspaceDestroy(WORKSPACE_DATA*  pWindowData)
{
   // Sever and delete the window data
   SetWindowLong(pWindowData->hWnd, 0, NULL);
   deleteWorkspaceData(pWindowData);
}


/// Function name  : onWorkspaceMouseDown
// Description     : Sets the appropriate position
// 
// WORKSPACE_DATA*  pWindowData  : [in] Window data
// CONST POINT*     ptCursor     : [in] Cursor position
// 
VOID  onWorkspaceMouseDown(WORKSPACE_DATA*  pWindowData, CONST POINT*  ptCursor)
{
   PANE*  pSplitterPane;

   // [CHECK] Is the mouse over an adjustable splitter bar?
   if (findSplitterBarPaneAtPoint(pWindowData->pBasePane, ptCursor, pSplitterPane) AND !pSplitterPane->bFixed)
   {
      /// [SUCCESS] Capture cursor and store dragging pane
      SetCapture(pWindowData->hWnd);
      pWindowData->pDragPane = pSplitterPane;
   }
}


/// Function name  : onWorkspaceMouseMove
// Description     : Sets the appropriate position
// 
// WORKSPACE_DATA*  pWindowData  : [in] Window data
// CONST POINT*     ptCursor     : [in] Cursor position
// 
VOID  onWorkspaceMouseMove(WORKSPACE_DATA*  pWindowData, CONST POINT*  ptCursor)
{
   PANE  *pDragPane,
         *pSplitterPane;
   RECT  *pFirstPaneRect;
   SIZE   siSplitterPane;

   /// [DRAGGING] Recalculate the split ratio and resize the child panes
   if (GetCapture() == pWindowData->hWnd)
   {
      // Prepare
      pDragPane      = pWindowData->pDragPane;
      pFirstPaneRect = &pDragPane->pChildren[LEFT]->rcPane;
      utilConvertRectangleToSize(&pDragPane->rcPane, &siSplitterPane);

      /// Recalculate the split ratio
      switch (pDragPane->eSplit)
      {
      case HORIZONTAL: pDragPane->fRatio = (FLOAT)(ptCursor->x - pFirstPaneRect->left) / (FLOAT)siSplitterPane.cx;  break;
      case VERTICAL:   pDragPane->fRatio = (FLOAT)(ptCursor->y - pFirstPaneRect->top) / (FLOAT)siSplitterPane.cy;   break;
      }

      // [CHECK] Enforce limits
      pDragPane->fRatio = max(0.05f, min(0.95f, pDragPane->fRatio));

      /// Resize panes
      resizeWorkspacePane(pDragPane);
   }
   // [CHECK] Is the mouse over an adjustable splitter bar?
   else if (findSplitterBarPaneAtPoint(pWindowData->pBasePane, ptCursor, pSplitterPane) AND !pSplitterPane->bFixed)
      /// [HOVERING] Set appropriate cursor
      SetCursor(LoadCursor(NULL, pSplitterPane->eSplit == HORIZONTAL ? IDC_SIZEWE : IDC_SIZENS));
}


/// Function name  : onWorkspaceMouseMove
// Description     : Sets the appropriate position
// 
// WORKSPACE_DATA*  pWindowData  : [in] Window data
// CONST POINT*     ptCursor     : [in] Cursor position
// 
VOID  onWorkspaceMouseUp(WORKSPACE_DATA*  pWindowData, CONST POINT*  ptCursor)
{
   /// [DRAGGING] Release mouse capture
   if (GetCapture() == pWindowData->hWnd)
   {
      ReleaseCapture();
      pWindowData->pDragPane = NULL;
   }
}


/// Function name  : onWorkspacePaint
// Description     : Paints the workspace splitter bars
// 
// WORKSPACE_DATA*  pWindowData  : [in] Window data
// HDC              hDC          : [in] Destination Device Context
// CONST RECT*      pUpdateRect  : [in] Update rectangle
// 
VOID  onWorkspacePaint(WORKSPACE_DATA*  pWindowData, HDC  hDC, CONST RECT*  pUpdateRect)
{
   // Clip child windows
   setWorkspaceClippingRegion(pWindowData->pBasePane, hDC);

   // Draw splitter bars
   //drawWorkspaceSplitterBar(hDC, pWindowData->clBackground, pWindowData->pBasePane);

   // Blanket entire window
   FillRect(hDC, pUpdateRect, pWindowData->hBackground);
}


/// Function name  : onWorkspaceResize
// Description     : Resizes all panes in the workspace
// 
// WORKSPACE_DATA*  pWindowData  : [in] Window data
// CONST SIZE*      pNewSize     : [in] New window size
// 
VOID  onWorkspaceResize(WORKSPACE_DATA*  pWindowData, CONST SIZE*  pNewSize)
{
   PANE*  pBasePane = pWindowData->pBasePane;   // Convenience pointer

   /// Update base pane rectangle
   pBasePane->rcPane.right  = pBasePane->rcPane.left + pNewSize->cx;
   pBasePane->rcPane.bottom = pBasePane->rcPane.top + pNewSize->cy;

   // Update workspace rectangle
   utilSetRectangle(&pWindowData->rcWorkspace, pWindowData->rcWorkspace.left, pWindowData->rcWorkspace.top, pNewSize->cx, pNewSize->cy);

   // Disable redrawing
   //setWorkspacePaneRedraw(pWindowData->pBasePane, FALSE);

   /// Update pane tree
   resizeWorkspacePane(pBasePane);

   // Enable redrawing
   //setWorkspacePaneRedraw(pWindowData->pBasePane, TRUE);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : wndprocWorkspace
// Description     : Window procedure for the Workspace
// 
// 
LRESULT  wndprocWorkspace(HWND  hWnd, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   CREATESTRUCT*     pCreationData;     // Creation data
   WORKSPACE_DATA*   pWindowData;       // Window data
   PAINTSTRUCT       oPaintData;        // Painting data
   POINT             ptCursor;          // Cursor position
   SIZE              siWindowSize;      // New window data

   // Prepare
   pWindowData = getWorkspaceWindowData(hWnd);

   // Examine message
   switch (iMessage)
   {
   /// [CREATE] Store window data
   case WM_CREATE:
      // Extract window data manually
      pCreationData = (CREATESTRUCT*)lParam; 
      pWindowData   = (WORKSPACE_DATA*)pCreationData->lpCreateParams;
      // Initialise window
      onWorkspaceCreate(pWindowData, hWnd);
      break;

   /// [DESTROY] Cleanup the window data
   case WM_DESTROY:
      onWorkspaceDestroy(pWindowData);
      break;

   /// [COMMANDS/NOTIFICATIONS] Pass to the parent of the workspace
   case WM_COMMAND:
   case WM_NOTIFY:
   case WM_CONTEXTMENU:
      return utilReflectMessage(hWnd, iMessage, wParam, lParam);

   /// [RESIZE] Resize all panes
   case WM_SIZE:
      siWindowSize.cx = LOWORD(lParam);
      siWindowSize.cy = HIWORD(lParam);
      onWorkspaceResize(pWindowData, &siWindowSize);
      break;

   /// [MOUSE MOVE] Set appropriate sizing cursor
   case WM_MOUSEMOVE:
      ptCursor.x = GET_X_LPARAM(lParam);
      ptCursor.y = GET_Y_LPARAM(lParam);
      onWorkspaceMouseMove(pWindowData, &ptCursor);
      break;

   /// [MOUSE DOWN] Set mouse capture
   case WM_LBUTTONDOWN:
      ptCursor.x = GET_X_LPARAM(lParam);
      ptCursor.y = GET_Y_LPARAM(lParam);
      onWorkspaceMouseDown(pWindowData, &ptCursor);
      break;

   /// [MOUSE UP] Release mouse capture
   case WM_LBUTTONUP:
      ptCursor.x = GET_X_LPARAM(lParam);
      ptCursor.y = GET_Y_LPARAM(lParam);
      onWorkspaceMouseUp(pWindowData, &ptCursor);
      break;

   /// [PAINT] Draw splitter bars
   case WM_PAINT:
      BeginPaint(pWindowData->hWnd, &oPaintData);
      onWorkspacePaint(pWindowData, oPaintData.hdc, &oPaintData.rcPaint);
      EndPaint(pWindowData->hWnd, &oPaintData);
      break;

   // [UNHANDLED]
   default:
      // [SYSTEM] Handle to default proc
      if (iMessage < WM_USER)
         return DefWindowProc(hWnd, iMessage, wParam, lParam);
      else
         // [USER] Pass to parent
         return utilReflectMessage(hWnd, iMessage, wParam, lParam);
   }

   // [HANDLED]
   return 0;
}



