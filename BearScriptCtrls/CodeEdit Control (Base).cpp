//
// CodeEdit Base (new).cpp : Provides a variety of back-end functions for the CodeEdit
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : calculateCodeEditFinalCharacterLocation
// Description     : Determines the location of the last character in a CodeEdit
// 
// CONST CODE_EDIT_DATA*  pWindowData : [in]  Window data
// CODE_EDIT_LOCATION*     pOutput     : [out] Location of the final character
// 
VOID  calculateCodeEditFinalCharacterLocation(CONST CODE_EDIT_DATA*  pWindowData, CODE_EDIT_LOCATION*  pOutput)
{
   // Return zero-based line count
   pOutput->iLine  = (getCodeEditLineCount(pWindowData) - 1);

   // Return one-based character index (this is used for positioning the caret after the last character, rather than before)
   pOutput->iIndex = getCodeEditLineLengthByIndex(pWindowData, pOutput->iLine);
}


/// Function name  : calculateCodeEditPageSize
// Description     : Calculates the size of a page based on the width of a character and the height of a line
// 
// CODE_EDIT_DATA*  pWindowData : [in] CodeEdit Window data
// 
VOID  calculateCodeEditPageSize(CODE_EDIT_DATA*  pWindowData)
{
   RECT   rcClientRect;      // Client rectangle

   // Prepare
   GetClientRect(pWindowData->hWnd, &rcClientRect);
   utilConvertRectangleToSize(&rcClientRect, &pWindowData->siPageSize);

   /// Calculate size of client area in characters ie. the 'page' size
   pWindowData->siPageSize.cx /= pWindowData->siCharacterSize.cx;
   pWindowData->siPageSize.cy /= pWindowData->siCharacterSize.cy;
}


/// Function name  : calculateCodeEditScrollBarLimits
// Description     : Calculates the maximum extent for the scroll-bars, based on the text in the CodeEdit
// 
// CODE_EDIT_DATA*  pWindowData : [in] CodeEdit window data
// 
VOID  calculateCodeEditScrollBarLimits(CODE_EDIT_DATA*  pWindowData)
{
   // [TRACK]
   TRACK_FUNCTION();

   /// [HORIZONTAL] Length of longest line converted into average character widths + plus 40 widths
   pWindowData->ptLastCharacter.iIndex = (pWindowData->iLongestLine / pWindowData->siCharacterSize.cx) + 40;

   /// [VERTICAL] Number of lines + half a page
   pWindowData->ptLastCharacter.iLine = getCodeEditLineCount(pWindowData) + (pWindowData->siPageSize.cy / 2);

   // [TRACK]
   END_TRACKING();
}


/// Function name  : drawCodeEditTextStream
// Description     : Draws text to a specified Device Context and consumes an equal sized piece of
//                      the specified drawing rectangle
// 
// HDC           hDC         : [in]     Device context to draw to
// CONST TCHAR*  szText      : [in]     Text to draw
// CONST UINT    iLength     : [in]     Length of the text, in characters
// RECT*         pDrawRect   : [in/out] Drawing rectangle
// 
VOID  drawCodeEditTextStream(HDC  hDC, CONST TCHAR*  szText, CONST UINT  iLength, RECT*  pDrawRect)
{
   SIZE  siTextSize;    // Size of the text being drawn

   // Draw text
   DrawText(hDC, szText, iLength, pDrawRect, DT_LEFT WITH DT_EXPANDTABS WITH DT_SINGLELINE WITH DT_NOPREFIX);

   // Offset drawing rectangle
   GetTextExtentPoint32(hDC, szText, iLength, &siTextSize);
   pDrawRect->left += siTextSize.cx;
}


/// Function name  : enableCodeEditEvents
// Description     : Turns events posted to the parent on or off
// 
// CODE_EDIT_DATA*  pWindowData : [in] CodeEdit window data
// CONST BOOL       bEnable     : [in] TRUE to enable events, FALSE to disable them
// 
VOID  enableCodeEditEvents(CODE_EDIT_DATA*  pWindowData, CONST BOOL  bEnable)
{
   // Update property
   pWindowData->bEventsEnabled = bEnable;
}


/// Function name  : getCodeEditLength
// Description     : Retrieves the size of the buffer required to hold entire CodeEdit text
// 
// const CODE_EDIT_DATA*  pWindowData   : [in] Window data
// 
// Return Value   : Total length of text, in characters
// 
UINT  getCodeEditLength(const CODE_EDIT_DATA*  pWindowData)
{
   CODE_EDIT_LINE*  pLineData;
   LIST_ITEM*       pIterator;
   UINT             iLength = 0;

   // Return the length of each line plus two for CRLF
   for (findCodeEditLineListItemByIndex(pWindowData, 0, pIterator); pLineData = extractListItemPointer(pIterator, CODE_EDIT_LINE); pIterator = pIterator->pNext)
      iLength += getCodeEditLineLength(pLineData) + 2;

   // Return length
   return iLength;
}


/// Function name  : getCodeEditLineCount
// Description     : Retrieves the current number of lines in a CodeEdit control
// 
// CONST CODE_EDIT_DATA*  pWindowData : [in] CodeEdit window data
// 
// Return Value   : Number of lines
// 
UINT  getCodeEditLineCount(CONST CODE_EDIT_DATA*  pWindowData)
{
   return getListItemCount(pWindowData->pLineList);
}


/// Function name  : getCodeEditLineLength
// Description     : Retrieves the length of a line of text in a CodeEdit control
// 
// CONST CODE_EDIT_LINE*  pLineData : [in] CodeEdit line data
// 
// Return Value   : Length of line, in characters
// 
UINT  getCodeEditLineLength(CONST CODE_EDIT_LINE*  pLineData)
{
   return getListItemCount(pLineData->pCharacterList);
}


/// Function name  : getCodeEditLineLengthByIndex
// Description     : Retrieves the length of a line of text in a CodeEdit control
// 
// CONST CODE_EDIT_DATA*  pWindowData   : [in] Window data
// CONST UINT              iLineNumber   : [in] Zero-based line number of the line to query
// 
// Return Value   : Line length, in characters. If line does not exist then NULL.
// 
UINT  getCodeEditLineLengthByIndex(CONST CODE_EDIT_DATA*  pWindowData, CONST UINT  iLineNumber)
{
   CODE_EDIT_LINE*  pLineData;
   UINT             iLength;

   // Prepare
   TRACK_FUNCTION();
   iLength = NULL;
   
   // Calculate result from LineData if found
   if (findCodeEditLineDataByIndex(pWindowData, iLineNumber, pLineData))
      iLength = getCodeEditLineLength(pLineData);

   // Return length
   END_TRACKING();
   return iLength;
}

/// Function name  : getCodeEditText
// Description     : Retrieves the entire text of the window.  Handler for WM_GETWINDOWTEXT
// 
// const CODE_EDIT_DATA*  pWindowData : [in]     Window data
// TCHAR*                 szBuffer    : [in/out] Buffer
// const INT              iLength     : [in]     Length of buffer
// 
// Return Value   : Number of characters copied, excluding NULL 
// 
UINT  getCodeEditText(const CODE_EDIT_DATA*  pWindowData, TCHAR*  szBuffer, const UINT  iLength)
{
   CODE_EDIT_LINE* pLineData;
   TEXT_STREAM*    pStream;
   LIST_ITEM*      pIterator;
   TCHAR*          szLineText,
                   szIndent[32];
         
   // Prepare
   pStream = createTextStream(getCodeEditLength(pWindowData));

   // Iterate through all lines
   for (findCodeEditLineListItemByIndex(pWindowData, 0, pIterator); pStream->iBufferUsed < iLength AND (pLineData = extractListItemPointer(pIterator, CODE_EDIT_LINE)); pIterator = pIterator->pNext)
   {
      // [INDENT]
      if (pLineData->iIndent)
      {
         StringCchCopy(szIndent, 1 + min(pLineData->iIndent, 31), TEXT("                               "));
         appendStringToTextStream(pStream, szIndent);
      }

      /// Copy each line and add a trailing CRLF
      appendStringToTextStream(pStream, szLineText = generateCodeEditLineText(pLineData));
      appendStringToTextStream(pStream, TEXT("\r\n"));
      utilDeleteString(szLineText);
   }

   /// Copy to output buffer
   StringCchCopy(szBuffer, iLength, pStream->szBuffer);

   // Cleanup and return number of chars copied
   deleteTextStream(pStream);
   return min(iLength - 1, pStream->iBufferUsed);
}


/// Function name  : hasCodeEditSelection
// Description     : Determines whether any text selection exists
// 
// CONST CODE_EDIT_DATA*  pWindowData : [in] Window data
// 
// Return Value   : TRUE or FALSE
// 
BOOL  hasCodeEditSelection(CONST CODE_EDIT_DATA*  pWindowData)
{
   // Return result
   return pWindowData->oSelection.bExists;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////


/// Function name  : calculateCodeEditDisplayRectangle
// Description     : Calculates the drawing rectangles for the various components of a line of text
// 
// HDC                                 hDC          : [in]            Device context to draw text
// CONST CODE_EDIT_LINE*               pLineData    : [in] [optional] Line data for the line, or NULL to draw an illusionary line beneath the main body of text
// CONST CODE_EDIT_DISPLAY_RECTANGLE   eRectangle   : [in]            Rectangle to retrieve
// CONST RECT&                         rcLineRect   : [in]            Bounding rectangle of the entire line
// RECT*                               pOutputRect  : [out]           Resultant drawing rectangle
// 
VOID  calculateCodeEditDisplayRectangle(HDC  hDC, CONST CODE_EDIT_LINE*  pLineData, CONST CODE_EDIT_DISPLAY_RECTANGLE  eRectangle, CONST RECT&  rcLineRect, RECT*  pOutputRect)
{
   SIZE   siLineSize,         // Size of the line rectangle
          siTextSize;         // Defines the size of a block of text
   TCHAR  szIndicator[8],     // Used for measuring indicator text
         *szLineText;         // Used for measuring the text of an entire line

   // Prepare
   TRACK_FUNCTION();
   utilConvertRectangleToSize(&rcLineRect, &siLineSize);

   // Examine required constant
   switch (eRectangle)
   {
   /// [LINE NUMBER INDICATOR] The rectangle containing the line number and a border on the RHS
   case CDR_LINE_NUMBER_INDICATOR:
      // Measure size of a five digit line number
      StringCchCopy(szIndicator, 8, TEXT("000000"));
      GetTextExtentPoint32(hDC, szIndicator, 6, &siTextSize);

      // Output a truncated LHS of line rectangle
      utilSetRectangle(pOutputRect, rcLineRect.left, rcLineRect.top, siTextSize.cx, siLineSize.cy);
      break;

   /// [INTERRUPT INDICATOR] The space containing the '@' symbol or a gap
   case CDR_INTERRUPT_INDICATOR:
      // Measure size of 3 interrupt indicators
      StringCchCopy(szIndicator, 8, TEXT("@y"));
      GetTextExtentPoint32(hDC, szIndicator, 2, &siTextSize);

      // Create interrupt indicator rectangle next to the line number indicator
      calculateCodeEditDisplayRectangle(hDC, pLineData, CDR_LINE_NUMBER_INDICATOR, rcLineRect, pOutputRect);
      utilSetRectangle(pOutputRect, pOutputRect->right, rcLineRect.top, siTextSize.cx, siLineSize.cy);
      break;

   /// [INDENTATION GAP] The gap that indents different lines according to their syntax
   case CDR_INDENTATION_GAP:
      // Create indentation gap rectangle next to the interrupt indicator
      calculateCodeEditDisplayRectangle(hDC, pLineData, CDR_INTERRUPT_INDICATOR, rcLineRect, pOutputRect);
      utilSetRectangle(pOutputRect, pOutputRect->right, rcLineRect.top, (20 * pLineData->iIndent), siLineSize.cy);
      break;

   /// [LINE TEXT] The rectangle defining the text of the line
   case CDR_LINE_TEXT:
      // Measure the line text (if any)
      if (getCodeEditLineLength(pLineData) > 0)
      {
         szLineText = generateCodeEditLineText(pLineData);
         GetTextExtentPoint32(hDC, szLineText, getCodeEditLineLength(pLineData), &siTextSize);
         utilDeleteString(szLineText);
      }
      else
         siTextSize.cx = NULL;

      // Create text rectangle next to the indentation gap
      calculateCodeEditDisplayRectangle(hDC, pLineData, CDR_INDENTATION_GAP, rcLineRect, pOutputRect);
      utilSetRectangle(pOutputRect, pOutputRect->right, rcLineRect.top, siTextSize.cx, siLineSize.cy);
      break;

   /// [LINE GAP] The rectangle defining the gap between the text and the RHS of the window
   case CDR_LINE_GAP:
      // Create line gap rectangle next to the text rectangle
      calculateCodeEditDisplayRectangle(hDC, pLineData, CDR_LINE_TEXT, rcLineRect, pOutputRect);
      SetRect(pOutputRect, pOutputRect->right, rcLineRect.top, rcLineRect.right, rcLineRect.bottom);
      break;
   }

   // [TRACK]
   END_TRACKING();
}


/// Function name  : calculateCodeEditLineRectangle
// Description     : Calculates the bounding rectangle of a line IN CLIENT CO-ORDINATES
//
// CONST CODE_EDIT_DATA*  pWindowData  : [in]  CodeEdit window data
// UINT                   iLineNumber  : [in]  Zero based line number
// RECT*                  pOutput      : [out] Resultant Line rectangle. LEFT/TOP/BOTTOM may be negative
// 
VOID  calculateCodeEditLineRectangle(CONST CODE_EDIT_DATA*  pWindowData, UINT  iLineNumber, RECT*  pOutput)
{
   RECT   rcClientRect;    // Client rectangle
   POINT  ptDrawOrigin;    // Top-left corner of the first visible line, in client co-ordinates

   // [TRACK]
   TRACK_FUNCTION();

   // Prepare
   GetClientRect(pWindowData->hWnd, &rcClientRect);

   /// Determine the origin of the first visible line, based on the current horizontal and vertical scroll position
   ptDrawOrigin.x = -((INT)pWindowData->ptFirstCharacter.iIndex * pWindowData->siCharacterSize.cx);
   ptDrawOrigin.y = ((INT)iLineNumber - pWindowData->ptFirstCharacter.iLine) * pWindowData->siCharacterSize.cy;

   // Create line rectangle from the origin above
   SetRect(pOutput, ptDrawOrigin.x, ptDrawOrigin.y, rcClientRect.right, ptDrawOrigin.y + pWindowData->siCharacterSize.cy);

   // [TRACK]
   END_TRACKING();
}


/// Function name  : calculateCodeEditLocationFromPoint
// Description     : Determines the a logical line number and character index, from a point in client co-ordinates
//                      
// CONST CODE_EDIT_DATA*  pWindowData  : [in]  CodeEdit window data
// CONST POINT*            ptPoint     : [in]  The position to convert, in client co-ordinates pixels
// CODE_EDIT_LOCATION*     pOutput     : [out] The line number and character index if found, otherwise {0,0}
// 
// Return Value   : TRUE if location was found, FALSE if location is in the gap beneath the last line of script code
// 
BOOL  calculateCodeEditLocationFromPoint(CONST CODE_EDIT_DATA*  pWindowData, CONST POINT*  ptPoint, CODE_EDIT_LOCATION*  pOutput)
{
   CODE_EDIT_LINE*  pLineData;       // Line data for the line at the position
   RECT             rcClientRect,    // Client rectangle
                    rcLineRect,      // Bounding line rectangle in client co-ordinates
                    rcTextRect;      // Line text rectangle in client co-ordinates
   SIZE             siTextSize;      // [IGNORED] Size of the text between LHS and cursor
   TCHAR*           szLineText;      // Text of the line at the input location
   BOOL             bResult;         // Operation result
   HDC              hDC;             // CodeEdit window DC for calculating text size

   // Prepare
   TRACK_FUNCTION();
   hDC = GetDC(pWindowData->hWnd);
   GetClientRect(pWindowData->hWnd, &rcClientRect);
   
   /// [LINE NUMBER] Return first visible line + lines between point and TOP
   pOutput->iLine = pWindowData->ptFirstCharacter.iLine + (ptPoint->y / pWindowData->siCharacterSize.cy);

   // Lookup line data for the input location
   if (bResult = findCodeEditLineDataByIndex(pWindowData, pOutput->iLine, pLineData))
   {
      // Get text rectangle
      calculateCodeEditLineRectangle(pWindowData, pOutput->iLine, &rcLineRect);
      calculateCodeEditDisplayRectangle(hDC, pLineData, CDR_LINE_TEXT, rcLineRect, &rcTextRect);

      /// [CLICK PRECEEDS TEXT] Return character index ZERO
      if (ptPoint->x < rcTextRect.left)
         pOutput->iIndex = 0;

      /// [CLICK FOLLOWS TEXT] Return last character index
      else if (ptPoint->x > rcTextRect.right)
         pOutput->iIndex = getCodeEditLineLength(pLineData);

      /// [CLICK ON CHARACTER] Return number of characters between the start of text and the point
      else
      {
         szLineText = generateCodeEditLineText(pLineData);
         GetTextExtentExPoint(hDC, szLineText, getCodeEditLineLength(pLineData), ptPoint->x - rcTextRect.left, (INT*)&pOutput->iIndex, NULL, &siTextSize);
         utilDeleteString(szLineText);
      }
   }
   // [NOT FOUND] Set result to {0,0}
   else
      pOutput->iIndex = pOutput->iLine = NULL;
   
   // Cleanup and return result
   ReleaseDC(pWindowData->hWnd, hDC);
   END_TRACKING();
   return bResult;
}


/// Function name  : calculateCodeEditMaximumLineLength
// Description     : Calculates the length of the longest line in pixels, and updates the scrollbars as necessary
// 
// CODE_EDIT_DATA*       pWindowData : [in] Window data
// CONST UINT             iLineNumber : [in] Zero-based line number of the line to test
// CONST CODE_EDIT_LINE*  pLineData   : [in] LineData for the line
// 
VOID  calculateCodeEditMaximumLineLength(CODE_EDIT_DATA*  pWindowData, CONST UINT  iLineNumber, CONST CODE_EDIT_LINE*  pLineData)
{
   RECT  rcLineRect,    // Line rectangle for the input line
         rcTextRect;    // Text rectangle for the input line
   HDC   hDC;           // Destination device context

   // Prepare
   TRACK_FUNCTION();
   hDC = GetDC(pWindowData->hWnd);

   /// Get text rectangle to input line, which defines it's length in pixels
   calculateCodeEditLineRectangle(pWindowData, iLineNumber, &rcLineRect);
   calculateCodeEditDisplayRectangle(hDC, pLineData, CDR_LINE_TEXT, rcLineRect, &rcTextRect);

   // Determine whether longest line has been exceeded
   if (pWindowData->iLongestLine < (UINT)(rcTextRect.right)) // - rcTextRect.left))
   {
      /// Update longest line
      pWindowData->iLongestLine = (UINT)(rcTextRect.right); // - rcTextRect.left);

      /// Recalculate scrollbar ranges
      updateCodeEditScrollBarLimits(pWindowData);
   }

   // Cleanup
   ReleaseDC(pWindowData->hWnd, hDC);
   END_TRACKING();
}


/// Function name  : calculateCodeEditPointFromLocation
// Description     : Determines a point in client co-ordinates, from a logical line number and character index
// 
// CONST CODE_EDIT_DATA*      pWindowData : [in]  CodeEdit window data
// CONST CODE_EDIT_LOCATION*  ptLocation  : [in]  Line number and character index to convert
///                                                -> The character index is defined as : 0 <= CharIndex < Line Length
// POINT*                     ptOutput    : [out] Client co-ordinates of the line number and character index.
// 
// Return Value   : TRUE if point is on a valid line, otherwise FALSE
// 
BOOL  calculateCodeEditPointFromLocation(CONST CODE_EDIT_DATA*  pWindowData, CONST CODE_EDIT_LOCATION*  ptLocation, POINT*  ptOutput)
{
   CODE_EDIT_LINE*   pLineData;     // Line data for the input line
   TCHAR*            szLineText;    // Text of the input line
   SIZE              siTextSize;    // Size of the text of the input line
   RECT              rcLineRect,    // Line rectangle, in client co-ordinates
                     rcTextRect;    // Line text rectangle, in client co-ordinates
   BOOL              bResult;       // Operation result
   HDC               hDC;           // CodeEdit window DC

   // [TRACK]
   TRACK_FUNCTION();

   // Lookup line data for the input line
   if (bResult = findCodeEditLineDataByIndex(pWindowData, ptLocation->iLine, pLineData))
   {
      // Prepare
      hDC        = GetDC(pWindowData->hWnd);
      szLineText = generateCodeEditLineText(pLineData);

      // Retrieve text rectangle for the input line
      calculateCodeEditLineRectangle(pWindowData, ptLocation->iLine, &rcLineRect);
      calculateCodeEditDisplayRectangle(hDC, pLineData, CDR_LINE_TEXT, rcLineRect, &rcTextRect);

      /// [VERTICAL] Top of the line rectangle
      ptOutput->y = rcLineRect.top;

      /// [HORIZONTAL] Left side of the desired character   (Return the position of final character if requested index is invalid)
      GetTextExtentPoint32(hDC, szLineText, min((UINT)ptLocation->iIndex, getCodeEditLineLength(pLineData)), &siTextSize);  
      ptOutput->x = rcTextRect.left + siTextSize.cx;

      // Cleanup
      ReleaseDC(pWindowData->hWnd, hDC);
      utilDeleteString(szLineText);
   }
   // [NOT FOUND] Set result to {0,0}
   else
      ptOutput->x = ptOutput->y = NULL;

   // Return result
   END_TRACKING();
   return bResult;
}


/// Function name  : calculateCodeEditSelectionLength
// Description     : Calculates the length of the currently selected text within the CodeEdit
// 
// CONST CODE_EDIT_DATA*  pWindowData : [in] Window data
// 
// Return Value   : Length of selection in characters (including CRLFs, excluding NULL terminator)
// 
UINT  calculateCodeEditSelectionLength(CONST CODE_EDIT_DATA*  pWindowData)
{
   CODE_EDIT_ITERATOR*   pIterator;   // CodeEdit text iterator
   UINT                  iLength;     // Operation result

   // Prepare
   TRACK_FUNCTION();
   iLength = NULL;

   // [CHECK] Ensure selection exists
   if (hasCodeEditSelection(pWindowData))
   {
      // Create text iterator
      pIterator = createCodeEditMultipleLineIterator(pWindowData, pWindowData->oSelection.oOrigin, pWindowData->oCaret.oLocation);

      /// Count through all characters within selection
      for (iLength = 0; findNextCodeEditCharacter(pIterator); iLength++)
         // [LINE BREAK] Add an extra character for a CRLF (for a total of 2)
         if (pIterator->chCharacter == '\r')
            iLength++;
      
      // Cleanup 
      deleteCodeEditIterator(pIterator);
   }

   // Return selection length, or NULL
   END_TRACKING();
   return iLength;
}


/// Function name  : compareCodeEditLocations
// Description     : Compares two locations
// 
// CONST CODE_EDIT_LOCATION&  oFirst  : [in] Location to compare
// CONST CODE_EDIT_LOCATION&  oSecond : [in] Location to compare against
// 
// Return Value   : AC_LESSER  : The first location preceeds the second location
//                  AC_EQUAL   : The locations are equal
//                  AC_GREATER : The first location follows the second location
// 
COMPARISON_RESULT  compareCodeEditLocations(CONST CODE_EDIT_LOCATION&  oFirst, CONST CODE_EDIT_LOCATION&  oSecond)
{
   COMPARISON_RESULT  eOutput;       // Comparison result

   // [SEPARATE LINES]
   if (oFirst.iLine != oSecond.iLine)
      eOutput = (oFirst.iLine < oSecond.iLine ? CR_LESSER : CR_GREATER);

   // [SAME LINE]
   else if (oFirst.iIndex != oSecond.iIndex)
      eOutput = (oFirst.iIndex < oSecond.iIndex ? CR_LESSER : CR_GREATER);

   // [EQUAL]
   else
      eOutput = CR_EQUAL;

   // Return result
   return eOutput;
}


/// Function name  : compareCodeEditLocationsEx
// Description     : Perform aggregate comparison on two CodeEdit locations, such as <= or >=
// 
// CONST CODE_EDIT_LOCATION&  oFirst         : [in] Location to compare
// CONST CODE_EDIT_LOCATION&  oSecond        : [in] Location to compare against
// CONST COMPARISON_RESULT    eDesiredResult : [in] Comparison to perform
// 
// Return Value   : TRUE or FALSE
// 
BOOL  compareCodeEditLocationsEx(CONST CODE_EDIT_LOCATION&  oFirst, CONST CODE_EDIT_LOCATION&  oSecond, CONST COMPARISON_RESULT  eDesiredResult)
{
   COMPARISON_RESULT  eComparison;     // Comparison result
   BOOL               bResult;         // Operation result

   // Perform comparisons
   eComparison = compareCodeEditLocations(oFirst, oSecond);

   // Examine desired result
   switch (eDesiredResult)
   {
   case CR_LESSER_OR_EQUAL:    
      bResult = (eComparison == CR_LESSER OR eComparison == CR_EQUAL);   
      break;

   case CR_GREATER_OR_EQUAL:
      bResult = (eComparison == CR_GREATER OR eComparison == CR_EQUAL);   
      break;

   case CR_LESSER:             
   case CR_EQUAL:     
   case CR_GREATER: 
      bResult = (eComparison == eDesiredResult);
      break;
   }

   // Return result
   return bResult;
}


/// Function name  : drawCodeEditLine
// Description     : Draws all the components of a line of CodeEdit text -- the line number and interrupt indicators, 
//                      the 'interrupt indicator gap', the indentation gap, the line text, the remaining line gap, and
//                      the warning/error squiggle
// 
// HDC                    hDC           : [in]           Device context to draw to
// CONST COLOUR_SCHEME*   pColourScheme : [in]           Current colour scheme, if any 
// CONST SCRIPT_FILE*     pScriptFile   : [in]           ScriptFile of the document, used for recognising arguments
// CONST CODE_EDIT_LINE*  pLineData     : [in][optional] Line data of the line being drawn or NULL to draw an empty line (with a line number)
// CONST UINT             iLineNumber   : [in]           Zero based line number of the line to draw
// CONST RECT&            rcLineRect    : [in]           Bounding rectangle of the entire line
// 
VOID  drawCodeEditLine(HDC  hDC, CONST COLOUR_SCHEME*  pColourScheme, CONST SCRIPT_FILE*  pScriptFile, CONST CODE_EDIT_LINE*  pLineData, CONST UINT  iLineNumber, CONST RECT&  rcLineRect)
{
   CODE_FRAGMENT*   pCodeFragment;     // CodeFragment for drawing contiguous blocks of text with similar graphical properties
   RECT             rcDrawRect,        // Drawing rectangle for the item currently being drawn
                    rcLineNumber;      // Drawing rectangle for Line number digits
   SIZE             siNumberDigit;     // Drawing metrics for the character '0'
   HBRUSH           hBackgroundBrush;  // Background brush
   TCHAR            szIndicator[8];    // Temporary buffer for formatting the line number and interrupt indicator

   // Prepare
   TRACK_FUNCTION();
   hBackgroundBrush = CreateSolidBrush(getInterfaceColour(pColourScheme->eBackgroundColour));
   SetBkMode(hDC, TRANSPARENT);

   // Prepare line number
   calculateCodeEditDisplayRectangle(hDC, pLineData, CDR_LINE_NUMBER_INDICATOR, rcLineRect, &rcDrawRect);
   StringCchPrintf(szIndicator, 8, TEXT("%u"), iLineNumber + 1);

   /// Draw line number background
   FillRect(hDC, &rcDrawRect, GetSysColorBrush(COLOR_WINDOW));
   DrawEdge(hDC, &rcDrawRect, EDGE_RAISED, BF_RIGHT);

   // Generate rectangle of line number digits
   GetTextExtentPoint32(hDC, TEXT("0"), 1, &siNumberDigit);
   SetRect(&rcLineNumber, rcDrawRect.left, rcDrawRect.top, rcDrawRect.right - siNumberDigit.cx, rcDrawRect.bottom);

   /// Draw line number  (in a system default colours)
   SetBkColor(hDC, GetSysColor(COLOR_WINDOW));
   SetTextColor(hDC, GetSysColor(COLOR_WINDOWTEXT));
   DrawText(hDC, szIndicator, lstrlen(szIndicator), &rcLineNumber, DT_RIGHT WITH DT_SINGLELINE);
   
   /// [SCRIPT COMMAND] Draw interrupt, indentation, text and squiggle
   if (pLineData)
   {
      // Prepare
      pCodeFragment = createCodeFragment(pLineData, pColourScheme, pScriptFile);

      // Draw interrupt gap
      calculateCodeEditDisplayRectangle(hDC, pLineData, CDR_INTERRUPT_INDICATOR, rcLineRect, &rcDrawRect);
      FillRect(hDC, &rcDrawRect, hBackgroundBrush);

      /// Draw interrupt indicator  (in light blue)
      if (pLineData->bInterrupt)
      {
         SetTextColor(hDC, getInterfaceColour(IC_LIGHT_BLUE));
         SetBkColor(hDC, getInterfaceColour(pColourScheme->eBackgroundColour));
         DrawText(hDC, TEXT("@"), 1, &rcDrawRect, DT_CENTER WITH DT_SINGLELINE);
      }
      
      /// Draw line indentation gap
      calculateCodeEditDisplayRectangle(hDC, pLineData, CDR_INDENTATION_GAP, rcLineRect, &rcDrawRect);
      FillRect(hDC, &rcDrawRect, hBackgroundBrush);

      // Prepare line text
      calculateCodeEditDisplayRectangle(hDC, pLineData, CDR_LINE_TEXT, rcLineRect, &rcDrawRect);
      SetBkMode(hDC, OPAQUE);

      /// Iterate through code fragments
      while (findNextCodeFragment(pCodeFragment))
      {
         // Set text and background colour
         if (pCodeFragment->bSelected)
         {
            SetTextColor(hDC, GetSysColor(COLOR_HIGHLIGHTTEXT));
            SetBkColor(hDC, GetSysColor(COLOR_HIGHLIGHT));
         }
         else
         {
            SetTextColor(hDC, pCodeFragment->clColour);
            SetBkColor(hDC, getInterfaceColour(pColourScheme->eBackgroundColour));
         }

         // Draw fragment
         drawCodeEditTextStream(hDC, pCodeFragment->szText, pCodeFragment->iLength, &rcDrawRect);
      }

      /// [LINE ERROR] Draw appropriate squiggle
      if (pLineData->pErrorQueue) // OR (pLineData->pCommand AND isCommandType(pLineData->pCommand, CT_VIRTUAL)))  /// REM: Blue underline for macros was too much information
      {
         // Prepare text rectangle
         calculateCodeEditDisplayRectangle(hDC, pLineData, CDR_LINE_TEXT, rcLineRect, &rcDrawRect);
         SetBkColor(hDC, getInterfaceColour(pColourScheme->eBackgroundColour));

         // Draw squiggle
         //if (pLineData->pErrorQueue)
         drawCodeEditLineSquiggle(hDC, pLineData->eSeverity == ET_WARNING ? CST_WARNING : CST_ERROR, rcDrawRect);
         /*else
            drawCodeEditLineSquiggle(hDC, CST_VIRTUAL, rcDrawRect);*/
      }

      /// Draw line gap
      calculateCodeEditDisplayRectangle(hDC, pLineData, CDR_LINE_GAP, rcLineRect, &rcDrawRect);
      FillRect(hDC, &rcDrawRect, hBackgroundBrush);

      // Cleanup
      deleteCodeFragment(pCodeFragment);
   }
   /// [FAKE LINE] Draw background only
   else
   {
      SetRect(&rcDrawRect, rcDrawRect.right, rcDrawRect.top, rcLineRect.right, rcDrawRect.bottom);
      FillRect(hDC, &rcDrawRect, hBackgroundBrush);
   }
   
   // Cleanup
   DeleteObject(hBackgroundBrush);
   END_TRACKING();
}


/// Function name  : drawCodeEditLineSquiggle
// Description     : Draws the squiggle beneath a line of CodeEdit text to indicate an error or a warning
// 
// HDC               hDC        : [in] Destination device context
// CONST ERROR_TYPE  eSeverity  : [in] Whether to draw an error or warning squiggle
// CONST RECT&       rcTextRect : [in] Text rectangle
// 
VOID  drawCodeEditLineSquiggle(HDC  hDC, CONST CODE_EDIT_SQUIGGLE  eSquiggle, CONST RECT&  rcTextRect)
{
   static COLORREF  clColours[3] = { RGB(200,0,3), RGB(228,211,7), RGB(138,184,253) };

   HBITMAP     hBitmap;       // Squiggle bitmap
   HBRUSH      hBrush;        // Squiggle pattern brush
   RECT        rcDrawRect;    // Squiggle drawing rectangle

   // Prepare
   TRACK_FUNCTION();
   hBitmap = LoadBitmap(getResourceInstance(), TEXT("SQUIGGLE_PATTERN"));
   hBrush  = CreatePatternBrush(hBitmap);

   // Calculate drawing rectangle
   SetRect(&rcDrawRect, rcTextRect.left, rcTextRect.bottom - 3, rcTextRect.right, rcTextRect.bottom);

   // Set squiggle colour
   SetTextColor(hDC, clColours[eSquiggle]); // eSeverity == ET_ERROR ? );

   /// Ensure pattern is painted correctly by matching it's origin with the drawing rectangle
   SelectObject(hDC, hBrush);
   SetBrushOrgEx(hDC, rcTextRect.left, rcTextRect.bottom - 1, NULL);

   /// Draw squiggle
   FillRect(hDC, &rcDrawRect, hBrush);

   // Cleanup   
   SelectObject(hDC, GetStockObject(WHITE_BRUSH));
   DeleteObject(hBrush);
   DeleteObject(hBitmap);
   END_TRACKING();
}


/// Function name  : drawCodeEditLineSquiggle
// Description     : Draws the squiggle beneath a line of CodeEdit text to indicate an error or a warning
// 
// HDC               hDC        : [in] Destination device context
// CONST ERROR_TYPE  eSeverity  : [in] Whether to draw an error or warning squiggle
// CONST RECT&       rcTextRect : [in] Text rectangle
// 
//VOID  drawCodeEditLineSquiggle(HDC  hDC, CONST ERROR_TYPE  eSeverity, CONST RECT&  rcTextRect)
//{
//   BLENDFUNCTION  oBlendData;
//   HBITMAP     hBitmap;       // Squiggle bitmap
//   HBRUSH      hBrush;        // Squiggle pattern brush
//   RECT        rcDrawRect,    // Squiggle drawing rectangle
//               rcSquiggleRect;
//
//   // Prepare
//   TRACK_FUNCTION();
//   hBitmap = LoadBitmap(getResourceInstance(), TEXT("SQUIGGLE_PATTERN"));
//   hBrush  = CreatePatternBrush(hBitmap);
//
//   // Create memory DC
//   HDC      hMemoryDC     = CreateCompatibleDC(hDC);
//   HBITMAP  hMemoryBitmap = CreateCompatibleBitmap(hDC, rcTextRect.right, rcTextRect.bottom);
//   HBITMAP  hOldBitmap = SelectBitmap(hMemoryDC, hMemoryBitmap);
//
//   // Calculate drawing rectangle
//   SetRect(&rcDrawRect, rcTextRect.left, rcTextRect.bottom - 3, rcTextRect.right, rcTextRect.bottom);
//
//   // Set squiggle colour
//   SetTextColor(hMemoryDC, eSeverity == ET_ERROR ? RGB(255,0,0) : RGB(255,255,0));
//
//   /// Ensure pattern is painted correctly by matching it's origin with the drawing rectangle
//   SelectObject(hMemoryDC, hBrush);
//   SetBrushOrgEx(hMemoryDC, rcTextRect.left, rcTextRect.bottom - 1, NULL);
//
//   /// Draw squiggle
//   //FillRect(hMemoryDC, &rcDrawRect, hBrush);
//   utilFillSysColourRect(hMemoryDC, &rcTextRect, COLOR_BTNFACE);
//
//   // Setup blending
//   utilZeroObject(&oBlendData, BLENDFUNCTION);
//   oBlendData.SourceConstantAlpha = 128;
//   oBlendData.BlendOp = AC_SRC_OVER;
//
//   /// AlphaBlend icon with blend colour
//   AlphaBlend(hDC, rcTextRect.left, rcTextRect.top, rcTextRect.right - rcTextRect.left, rcTextRect.bottom - rcTextRect.top, hMemoryDC, rcTextRect.top, rcTextRect.left, rcTextRect.right - rcTextRect.left, rcTextRect.bottom - rcTextRect.top, oBlendData);
//
//   // Cleanup   
//   SelectObject(hMemoryDC, GetStockObject(WHITE_BRUSH));
//   SelectObject(hMemoryDC, hOldBitmap);
//   DeleteObject(hBrush);
//   DeleteObject(hBitmap);
//   DeleteObject(hMemoryBitmap);
//   DeleteDC(hMemoryDC);
//   END_TRACKING();
//}

/// Function name  : ensureCodeEditLocationIsVisible
// Description     : Determine whether a given location is visible and scroll to it if not. 
///                      NB: Doesn't move the caret.
// 
// CODE_EDIT_DATA*           pWindowData : [in] Window data
// CONST CODE_EDIT_LOCATION*  ptLocation  : [in] Zero-based line number to ensure is visible
// 
VOID  ensureCodeEditLocationIsVisible(CODE_EDIT_DATA*  pWindowData, CONST CODE_EDIT_LOCATION*  ptLocation)
{
   POINT  ptPoint;            // Input location in client co-ordinates
   UINT   iHorizontalTarget;  // Input character index converted into horizontal scroll units (average character widths)

   // [TRACK]
   TRACK_FUNCTION();

   // Since the horizontal scroll units are average character widths and the desired location is in character
   //  -> indicies, they are not equal.  Convert the index into pixels, and that into average character widths

   // Determine location in client co-ordinates
   if (calculateCodeEditPointFromLocation(pWindowData, ptLocation, &ptPoint))
   {
      // Determine target index in average character widths
      iHorizontalTarget = pWindowData->ptFirstCharacter.iIndex + (ptPoint.x / pWindowData->siCharacterSize.cx);

      /// [CHARACTER BEFORE] Make equivilent index the first visible
      if ((iHorizontalTarget - 1) < pWindowData->ptFirstCharacter.iIndex)
         // Scroll to the character
         onCodeEditScroll(pWindowData, SB_THUMBTRACK, (iHorizontalTarget - 1), SB_HORZ);

      /// [CHARACTER AFTER] Make equivilent index the last visible
      else if (iHorizontalTarget >= pWindowData->ptFirstCharacter.iIndex + (pWindowData->siPageSize.cx - 1))
         // Scroll to one page before the character
         onCodeEditScroll(pWindowData, SB_THUMBTRACK, (iHorizontalTarget - (pWindowData->siPageSize.cx - 1)), SB_HORZ);
   }
   // else
   //    do nothing, target location couldn't contain the caret anyway.


   /// [LINE ABOVE] Make input line the first visible
   if (ptLocation->iLine < pWindowData->ptFirstCharacter.iLine)
      // Scroll precisely to the desired line
      onCodeEditScroll(pWindowData, SB_THUMBTRACK, ptLocation->iLine, SB_VERT);

   /// [LINE BELOW] Make input line the last visible
   else if (ptLocation->iLine >= pWindowData->ptFirstCharacter.iLine + pWindowData->siPageSize.cy)
      // Scroll to one page above the line
      onCodeEditScroll(pWindowData, SB_THUMBTRACK, (ptLocation->iLine - pWindowData->siPageSize.cy + 1), SB_VERT);

   // [TRACK]
   END_TRACKING();
}


/// Function name  : generateCodeEditLineText
// Description     : Generates a new string from a line's character data
// 
// CONST CODE_EDIT_LINE*  pLineData : [in] LineData of the line containing the text to generate a string from
// 
// Return Value   : New NULL terminated string, you are responsible for destroying it
// 
TCHAR*  generateCodeEditLineText(CONST CODE_EDIT_LINE*  pLineData)
{
   CODE_EDIT_ITERATOR*   pIterator;    // CodeEdit text iterator
   TCHAR*                szOutput;     // Output string

   // Prepare
   TRACK_FUNCTION();
   pIterator = createCodeEditSingleLineIterator(pLineData);
   szOutput  = utilCreateEmptyString(getCodeEditLineLength(pLineData) + 1);      // Naturally null terminated

   /// Iterate through CharacterData and output string simultaneously
   for (TCHAR*  szIterator = szOutput; findNextCodeEditCharacter(pIterator); szIterator++)
      // Copy current character to output string
      szIterator[0] = pIterator->chCharacter;

   // Cleanup and return Null terminated string
   deleteCodeEditIterator(pIterator);
   END_TRACKING();
   return szOutput;
}


/// Function name  : generateCodeEditSelectionText
// Description     : Generates a string containing the currently selected text
// 
// CONST CODE_EDIT_DATA*  pWindowData   : [in] Window data
// 
// Return Value   : New NULL terminated string containing the selected text, you are responsible for destroying it
//                   -> If no text is selected then the result is NULL
// 
TCHAR*  generateCodeEditSelectionText(CONST CODE_EDIT_DATA*  pWindowData)
{
   CODE_EDIT_ITERATOR*   pIterator;    // CodeEdit text iterator
   TCHAR*                szOutput;     // Output string

   // Prepare
   TRACK_FUNCTION();
   szOutput = NULL;

   // [CHECK] Ensure selection exists
   if (hasCodeEditSelection(pWindowData))
   {
      // Prepare
      pIterator = createCodeEditMultipleLineIterator(pWindowData, pWindowData->oSelection.oOrigin, pWindowData->oCaret.oLocation);
      szOutput  = utilCreateEmptyString(calculateCodeEditSelectionLength(pWindowData) + 1);     // Naturally null terminated

      /// Iterate through CharacterData and output string simultaneously
      for (TCHAR*  szIterator = szOutput; findNextCodeEditCharacter(pIterator); szIterator++)
      {
         // Copy character to output string
         szIterator[0] = pIterator->chCharacter;

         // [LINE BREAK] Add CRLF
         if (pIterator->chCharacter == '\r')
         {
            szIterator[1] = '\n';
            szIterator++;
         }
      }

      // Cleanup
      deleteCodeEditIterator(pIterator);
   }

   // Return null terminated output string
   END_TRACKING();
   return szOutput;
}



/// Function name  : invalidateCodeEditLine
// Description     : Invalidate a specific line in the CodeEdit
// 
// CODE_EDIT_DATA*  pWindowData : [in] Window data
// CONST UINT        iLineNumber : [in] Zero-based line number of the line to update
// 
VOID  invalidateCodeEditLine(CODE_EDIT_DATA*  pWindowData, CONST UINT  iLineNumber)
{
   RECT   rcLineRect;      // Line rectangle of input line

   // [TRACK]
   TRACK_FUNCTION();

   // Calculate line rectangle
   calculateCodeEditLineRectangle(pWindowData, iLineNumber, &rcLineRect);

   // Add to update rectangle
   InvalidateRect(pWindowData->hWnd, &rcLineRect, FALSE);

   // [TRACK]
   END_TRACKING();
}

/// Function name  : isCodeEditLocationSelected
// Description     : Determines whether a CodeEdit location is selected or not
// 
// CODE_EDIT_DATA*           pWindowData : [in] Window data
// CONST CODE_EDIT_LOCATION&  oLocation   : [in] Location to test
// 
// Return Value   : TRUE if location within selection, FALSE if not or if there is no selection
// 
BOOL  isCodeEditLocationSelected(CODE_EDIT_DATA*  pWindowData, CONST CODE_EDIT_LOCATION&  oLocation)
{
   CODE_EDIT_LOCATION  oStart,       // Start location of the current selection
                       oFinish;      // End location of the current selection
   COMPARISON_RESULT   eComparison;  // Origin/caret comparison result
   BOOL                bResult;      // Operation result

   // Prepare
   TRACK_FUNCTION();
   bResult = FALSE;

   // [CHECK] Ensure there is a selection
   if (hasCodeEditSelection(pWindowData))
   {
      // Determine whether CARET preceeds ORIGIN or vice versa
      switch (eComparison = compareCodeEditLocations(pWindowData->oCaret.oLocation, pWindowData->oSelection.oOrigin))
      {
      case CR_LESSER:    oStart = pWindowData->oCaret.oLocation;     oFinish = pWindowData->oSelection.oOrigin;    break;
      case CR_GREATER:   oStart = pWindowData->oSelection.oOrigin;   oFinish = pWindowData->oCaret.oLocation;      break;
      }

      // [CHECK] Ensure selection isn't empty (shouldn't happen - selection is empty but also flagged as existent)
      if (eComparison != CR_EQUAL)
         // Return TRUE if location is inside selection
         bResult = compareCodeEditLocationsEx(oStart, oLocation, CR_LESSER_OR_EQUAL) AND compareCodeEditLocationsEx(oLocation, oFinish, CR_LESSER);
   }

   // Return result
   END_TRACKING();
   return bResult;
}


/// Function name  : setCodeEditCaretLocation
// Description     : Positions the caret at the specified location.  If the location is invalid then the caret is not moved.
// 
// CODE_EDIT_DATA*           pWindowData : [in] Window data
// CONST CODE_EDIT_LOCATION*  ptLocation  : [in] New location of the caret
// 
VOID   setCodeEditCaretLocation(CODE_EDIT_DATA*  pWindowData, CONST CODE_EDIT_LOCATION*  ptLocation)
{
   CODE_EDIT_LINE*  pLineData;            // LineData for the specified location
   LIST_ITEM*       pCharacterListItem;   // ListItem containing the CharacterData for the specified location

   // [TRACK]
   TRACK_FUNCTION();

   /// Locate LineData for the specified location
   if (findCodeEditLineDataByIndex(pWindowData, ptLocation->iLine, pLineData))
   {
      /// Locate ListItem containing CharacterData for the specified location. Also allow the caret
      ///   to be placed after the final character in the line, and use NULL for the ListItem.
      if (findCodeEditCharacterListItemByIndex(pLineData, ptLocation->iIndex, pCharacterListItem)
          OR ptLocation->iIndex == getCodeEditLineLength(pLineData))
      {
         // Save new position
         pWindowData->oCaret.pCharacterItem = pCharacterListItem;
         pWindowData->oCaret.oLocation      = (*ptLocation);
         
         // Move caret to new position
         updateCodeEditCaretPosition(pWindowData);
      }

      // [CHECK] Do not update scope during createCodeEditData(..)
      if (pLineData->pCommand)
         /// Update current scope
         updateCodeEditScope(pWindowData);
   }

   // [TRACK]
   END_TRACKING();
}


/// Function name  : setCodeEditPreferences
// Description     : Updates the CodeEdit in response to new preferences
// 
// CODE_EDIT_DATA*     pWindowData   : [in] Window data
// CONST PREFERENCES*  pPreferences  : [in] New preferences object
// 
VOID   setCodeEditPreferences(CODE_EDIT_DATA*  pWindowData, CONST PREFERENCES*  pPreferences)
{
   // [TRACK]
   TRACK_FUNCTION();

   /// Store preferences object and update ColourScheme convenience pointer
   pWindowData->pPreferences  = pPreferences;
   pWindowData->pColourScheme = &pPreferences->oColourScheme;

   // [EVENT] Preferences Changed
   onCodeEditPreferencesChanged(pWindowData);
   END_TRACKING();
}


/// Function name  : setCodeEditSelection
// Description     : Adds or removes the selection from a block of text defined by two locations
// 
// CODE_EDIT_DATA*            pWindowData : [in/out] Window data containing caret and selection
// CONST CODE_EDIT_LOCATION&  oLocation1  : [in]     First location, may be the start or the end
// CONST CODE_EDIT_LOCATION&  oLocation2  : [in]     Second location, may be the start or the end
// CONST BOOL                 bState      : [in]     TRUE to add selection, FALSE to remove it
// 
VOID  setCodeEditSelection(CODE_EDIT_DATA*  pWindowData, CONST CODE_EDIT_LOCATION&  oLocation1, CONST CODE_EDIT_LOCATION&  oLocation2, CONST BOOL  bState)
{
   CODE_EDIT_LOCATION  oStart,               // Location of first character in the selection
                       oFinish;              // Location of last character in the selection
   COMPARISON_RESULT   eComparison;          // Comparison result
   CODE_EDIT_LINE     *pLineData;            // LineData for the line currently being processed
   LIST_ITEM          *pLineIterator,        // LineData iterator
                      *pCharacterIterator;   // CharacterData iterator
   UINT                iCurrentLine,         // Line number of the line currently being processed
                       iCurrentIndex;        // Character index of the character currently being processed

   // [TRACK]
   TRACK_FUNCTION();

   // Ensure our start point actually preceeds our end point
   switch (eComparison = compareCodeEditLocations(oLocation1, oLocation2))
   {
   case CR_LESSER:    oStart = oLocation1;   oFinish = oLocation2;      break;
   case CR_GREATER:   oStart = oLocation2;   oFinish = oLocation1;      break;
   }

   // [CHECK] Ensure selection isn't empty
   if (eComparison != CR_EQUAL)
   {
      // [ADD SELECTION] Set the 'Has Selection' flag
      if (bState)
         pWindowData->oSelection.bExists = TRUE;

      /// Iterate through all lines from START to FINISH
      for (findCodeEditLineListItemByIndex(pWindowData, iCurrentLine = oStart.iLine, pLineIterator); pLineIterator AND iCurrentLine <= oFinish.iLine; pLineIterator = pLineIterator->pNext)
      {
         // Prepare
         pLineData = extractListItemPointer(pLineIterator, CODE_EDIT_LINE);

         /// [SINGLE LINE SELECTION] Select characters from START to FINISH
         if (oStart.iLine == oFinish.iLine)
         {
            // Iterate through all characters between START and FINISH
            for (findCodeEditCharacterListItemByIndex(pLineData, iCurrentIndex = oStart.iIndex, pCharacterIterator); pCharacterIterator AND iCurrentIndex < oFinish.iIndex; pCharacterIterator = pCharacterIterator->pNext)
            {
               // Set selection state
               extractListItemPointer(pCharacterIterator, CODE_EDIT_CHARACTER)->bSelected = bState;
               // Move to the next character
               iCurrentIndex++;
            }
         }
         /// [FIRST LINE] Select characters from START to LINE-END
         else if (iCurrentLine == oStart.iLine)
         {
            // Iterate through all characters between START and END OF LINE
            for (findCodeEditCharacterListItemByIndex(pLineData, iCurrentIndex = oStart.iIndex, pCharacterIterator); pCharacterIterator; pCharacterIterator = pCharacterIterator->pNext)
               // Set selection state
               extractListItemPointer(pCharacterIterator, CODE_EDIT_CHARACTER)->bSelected = bState;
         }
         /// [MIDDLE LINE] Select ENTIRE LINE
         else if (iCurrentLine != oFinish.iLine)
         {
            // Iterate through all characters
            for (findCodeEditCharacterListItemByIndex(pLineData, 0, pCharacterIterator); pCharacterIterator; pCharacterIterator = pCharacterIterator->pNext)
               // Set selection state
               extractListItemPointer(pCharacterIterator, CODE_EDIT_CHARACTER)->bSelected = bState;
         }
         /// [FINISH LINE] Select from LINE-START to FINISH
         else
         {
            // Iterate through all characters between START and END OF LINE
            for (findCodeEditCharacterListItemByIndex(pLineData, iCurrentIndex = 0, pCharacterIterator); pCharacterIterator AND iCurrentIndex < oFinish.iIndex; pCharacterIterator = pCharacterIterator->pNext)
            {
               // Set selection state
               extractListItemPointer(pCharacterIterator, CODE_EDIT_CHARACTER)->bSelected = bState;
               // Move to the next character
               iCurrentIndex++;
            }
         }

         // [EVENT] Selection has changed
         updateCodeEditLine(pWindowData, iCurrentLine, CCF_SELECTION_CHANGED);

         /// Advance to the next line
         iCurrentLine++;
      }
   }

   // [TRACK]
   END_TRACKING();
}



/// Function name  : performCodeEditHitTest
// Description     : Performs a CodeEdit hit-test
// 
// CODE_EDIT_DATA*  pWindowData : [in] Window data
// CONST POINT*     ptCursor    : [in] Position in client co-ordinates
// 
// Return Value   : HitTest area code
// 
CODE_EDIT_AREA  performCodeEditHitTest(CODE_EDIT_DATA*  pWindowData, CONST POINT*  ptCursor, CODE_EDIT_LOCATION*  pOutputLocation)
{
   CODE_EDIT_LOCATION  oLocation;      // Location beneath the cursor
   CODE_EDIT_LINE*     pLineData;      // Line data
   CODE_EDIT_AREA      eOutput;        // HitTest result
   RECT                rcArea,         // drawing area currently being tested
                       rcLine;         // Line rectangle
   HDC                 hDC;            // CodeEdit DC

   // [TRACK]
   TRACK_FUNCTION();

   // Lookup client area
   GetClientRect(pWindowData->hWnd, &rcArea);

   // [CHECK] Is point within the CodeEdit?
   if (!PtInRect(&rcArea, *ptCursor))
      /// [NONE] Point not within CodeEdit
      eOutput = CDR_NONE;

   // [CHECK] Is the point beneath the last line of text?
   else if (!calculateCodeEditLocationFromPoint(pWindowData, ptCursor, &oLocation))
      /// [PAGE-GAP] Point below edit text
      eOutput = CDR_PAGE_GAP;
   
   else
   {
      // Lookup line data
      findCodeEditLineDataByIndex(pWindowData, oLocation.iLine, pLineData);

      // [CHECK] Is point within line?
      if (oLocation.iIndex > 0 AND oLocation.iIndex < getCodeEditLineLength(pLineData))
         /// [LINE-TEXT] 
         eOutput = CDR_LINE_TEXT;

      // [RIGHT] Check whether position is over line gap
      else 
      {
         // Prepare
         hDC = GetDC(pWindowData->hWnd);

         // Lookup line rectangle
         calculateCodeEditLineRectangle(pWindowData, oLocation.iLine, &rcLine);

         // [CHECK] Is point to the right of the text?
         if (oLocation.iIndex == getCodeEditLineLength(pLineData))
         {
            // Lookup line text rectangle
            calculateCodeEditDisplayRectangle(hDC, pLineData, CDR_LINE_TEXT, rcLine, &rcArea);

            /// [LINE TEXT/GAP] Point within LINE-TEXT or LINE-GAP
            eOutput = (ptCursor->x > rcArea.right ? CDR_LINE_GAP : CDR_LINE_TEXT);
         }
         // [CHECK] Is point to the left of the text?
         else 
         {
            // Lookup indentation gap
            calculateCodeEditDisplayRectangle(hDC, pLineData, CDR_INDENTATION_GAP, rcLine, &rcArea);

            // [CHECK] Is point to the right of the indentation gap?
            if (ptCursor->x > rcArea.right)
               /// [LINE-TEXT] 
               eOutput = CDR_LINE_TEXT;

            // [CHECK] Is point within indendation gap?
            else if (ptCursor->x >= rcArea.left)
               /// [INDENTATION GAP] 
               eOutput = CDR_INDENTATION_GAP;

            else
            {
               // Lookup line number rect
               calculateCodeEditDisplayRectangle(hDC, pLineData, CDR_LINE_NUMBER_INDICATOR, rcLine, &rcArea);

               /// [LINE NUMBER / INTERRUPT]
               eOutput = (PtInRect(&rcArea, *ptCursor) ? CDR_LINE_NUMBER_INDICATOR : CDR_INTERRUPT_INDICATOR);
            }
         }

         // Cleanup
         ReleaseDC(pWindowData->hWnd, hDC);
      }

      /// [SUCCESS] Set output
      if (pOutputLocation)
         *pOutputLocation = oLocation;
   }

   // Return result
   END_TRACKING();
   return eOutput;
}



/// Function name  : updateCodeEditCaretPosition
// Description     : Moves the caret to the client co-ordinates defined by the caret's current location
// 
// CODE_EDIT_DATA*  pWindowData   : [in] Window data containing the caret's current location
// 
VOID   updateCodeEditCaretPosition(CODE_EDIT_DATA*  pWindowData)
{
   POINT   ptCaretPosition;      // New caret position in client co-ordinates

   // [TRACK]
   TRACK_FUNCTION();

   // Calculate new client co-ordinates of the current caret location
   calculateCodeEditPointFromLocation(pWindowData, &pWindowData->oCaret.oLocation, &ptCaretPosition);

   // Move caret to the co-ordinates
   SetCaretPos(ptCaretPosition.x, ptCaretPosition.y);
   
   // [TRACK]
   END_TRACKING();
}


/// Function name  : updateCodeEditScrollBarLimits
// Description     : Sets the maximum position of the horizontal and vertical scrollbars
//
// CODE_EDIT_DATA*  pWindowData : [in] Window data
// 
VOID    updateCodeEditScrollBarLimits(CODE_EDIT_DATA*  pWindowData)
{
   SCROLLINFO  oScrollBarInfo;    // Holds Scrollbar ranges

   // [TRACK]
   TRACK_FUNCTION();

   // Prepare
   oScrollBarInfo.cbSize = sizeof(SCROLLINFO);
   oScrollBarInfo.fMask  = SIF_RANGE WITH SIF_PAGE WITH SIF_DISABLENOSCROLL;
   oScrollBarInfo.nMin   = 0;

   // Re-calculate limits
   calculateCodeEditScrollBarLimits(pWindowData);
   
   // [HORIZONTAL] 
   oScrollBarInfo.nMax  = pWindowData->ptLastCharacter.iIndex; 
   oScrollBarInfo.nPage = pWindowData->siPageSize.cx;

   // [CHECK] Ensure page size is always less than the maximum if the left edge of the window isn't completely visible
   if (pWindowData->ptFirstCharacter.iIndex > 0)
      oScrollBarInfo.nPage = min(oScrollBarInfo.nPage, oScrollBarInfo.nMax - pWindowData->ptFirstCharacter.iIndex);

   SetScrollInfo(pWindowData->hWnd, SB_HORZ, &oScrollBarInfo, TRUE);

   // [VERTICAL] 
   oScrollBarInfo.nMax  = pWindowData->ptLastCharacter.iLine;
   oScrollBarInfo.nPage = pWindowData->siPageSize.cy;

   SetScrollInfo(pWindowData->hWnd, SB_VERT, &oScrollBarInfo, TRUE);

   // [TRACK]
   END_TRACKING();
}


/// Function name  : updateCodeEditSelection
// Description     : Updates the text selection after the caret has been moved
// 
// CODE_EDIT_DATA*           pWindowData : [in] Window data
// CONST CODE_EDIT_LOCATION&  oOldCaret   : [in] Previous position of the caret
// 
VOID  updateCodeEditSelection(CODE_EDIT_DATA*  pWindowData, CONST CODE_EDIT_LOCATION&  oOldCaret)
{
   CODE_EDIT_LOCATION  oNewCaret,      // Convenience reference for the current location of the caret
                       oOrigin;        // Convenience reference for the origin of the current selection

   // [TRACK]
   TRACK_FUNCTION();

   // Prepare
   oNewCaret = pWindowData->oCaret.oLocation;
   oOrigin   = pWindowData->oSelection.oOrigin;

   /// [CASE: EMPTY] Selection is empty
   ///                -> Remove selection between OldCaret and Origin
   //
   //   STATE  :  NewCaret == Origin
   //
   if (compareCodeEditLocations(oNewCaret, oOrigin) == CR_EQUAL)

      setCodeEditSelection(pWindowData, oOldCaret, oOrigin, FALSE);

   /// [CASE: EXTENSION] Selection has been extended away from the origin
   ///                    -> Add selection between OldCaret and NewCaret
   //
   //   STATE  :  NewCaret < OldCaret   AND   OldCaret <= Origin
   //   MIRROR :  NewCaret > OldCaret   AND   OldCaret >= Origin
   //
   else if (compareCodeEditLocations(oNewCaret, oOldCaret) == CR_LESSER  AND compareCodeEditLocationsEx(oOldCaret, oOrigin, CR_LESSER_OR_EQUAL) OR
            compareCodeEditLocations(oNewCaret, oOldCaret) == CR_GREATER AND compareCodeEditLocationsEx(oOldCaret, oOrigin, CR_GREATER_OR_EQUAL))

      setCodeEditSelection(pWindowData, oOldCaret, oNewCaret, TRUE);

   /// [CASE: REDUCTION] Selection has been reduced toward the origin
   ///                    -> Remove selection between OldCaret and NewCaret
   //
   //   STATE  :  NewCaret > OldCaret   AND   NewCaret <  Origin
   //   MIRROR :  NewCaret < OldCaret   AND   NewCaret >  Origin
   //
   else if (compareCodeEditLocations(oNewCaret, oOldCaret) == CR_GREATER AND compareCodeEditLocations(oNewCaret, oOrigin) == CR_LESSER OR
            compareCodeEditLocations(oNewCaret, oOldCaret) == CR_LESSER  AND compareCodeEditLocations(oNewCaret, oOrigin) == CR_GREATER)

      setCodeEditSelection(pWindowData, oOldCaret, oNewCaret, FALSE);

   /// [CASE: REVERSE] Selection has been 'flipped' around the origin
   ///                  -> Remove selection between OldCaret and Origin
   ///                  -> Add selection between NewCaret and Origin
   //
   //   STATE  :  OldCaret < Origin   AND   NewCaret > Origin
   //   MIRROR :  OldCaret > Origin   AND   NewCaret < Origin
   //
   else if (compareCodeEditLocations(oOldCaret, oOrigin) == CR_LESSER  AND compareCodeEditLocations(oNewCaret, oOrigin) == CR_GREATER OR
            compareCodeEditLocations(oOldCaret, oOrigin) == CR_GREATER AND compareCodeEditLocations(oNewCaret, oOrigin) == CR_LESSER)
   {
      setCodeEditSelection(pWindowData, oOldCaret, oOrigin, FALSE);
      setCodeEditSelection(pWindowData, oNewCaret, oOrigin, TRUE);
   }

   // [TRACK]
   END_TRACKING();
}



/// Function name  : updateCodeEditLine
// Description     : Informs the CodeEdit of a change in it's text, selection or line count and updates itself accordingly
// 
// CODE_EDIT_DATA*  pWindowData  : [in] Window data
// CONST UINT        iLineNumber  : [in] Zero-based line number identifying the source of the change
// CONST UINT        iUpdateFlags : [in] Combination of CODE_EDIT_CHANGE_FLAGs identifying the type(s) of change
// 
VOID  updateCodeEditLine(CODE_EDIT_DATA*  pWindowData, CONST UINT  iLineNumber, CONST UINT  iUpdateFlags)
{
   CODE_EDIT_LINE*  pLineData;         // LineData for the input line
   UINT             iEventFlags;

   // Prepare
   TRACK_FUNCTION();
   iEventFlags = iUpdateFlags INCLUDES (CCF_TEXT_CHANGED WITH CCF_SELECTION_CHANGED);

   /// [SELECTION] Repaint line and inform main window of the change, so it can update the toolbar
   if (iUpdateFlags INCLUDES CCF_SELECTION_CHANGED) // BUG: Main window Toolbar isn't updating after a selection change
      // Invalidate line
      invalidateCodeEditLine(pWindowData, iLineNumber);
   
   /// [ERROR/INDENTATION] Repaint line
   if (iUpdateFlags INCLUDES CCF_ERROR_CHANGED OR iUpdateFlags INCLUDES CCF_INDENTATION_CHANGED)
      // Invalidate line
      invalidateCodeEditLine(pWindowData, iLineNumber);

   /// [TEXT] Recalculate CommandID, conditional and indentation.  Repaint affected lines.
   if (iUpdateFlags INCLUDES CCF_TEXT_CHANGED)
   {
      // Prepare
      findCodeEditLineDataByIndex(pWindowData, iLineNumber, pLineData);

      // Clear any error previously associated with this line as the text has now changed
      if (pLineData->pErrorQueue)
         deleteErrorQueue(pLineData->pErrorQueue);

      // Update the length of the longest line, if appropriate
      calculateCodeEditMaximumLineLength(pWindowData, iLineNumber, pLineData);

      /// Update COMMAND and extract ID, conditional and interruptability
      updateCodeEditLineCommand(pWindowData, pLineData, NULL);

      /// Recalculate line indentation, and invalidate those that have changed
      calculateCodeEditIndentationForMultipleLines(pWindowData, iLineNumber);

      // Repaint caret and current line
      updateCodeEditCaretPosition(pWindowData);
      invalidateCodeEditLine(pWindowData, iLineNumber);
   }

   /// [EVENT] Inform the parent window that the text and/or selection has changed
   if (pWindowData->bEventsEnabled)
      SendMessage(GetParent(pWindowData->hWnd), UN_CODE_EDIT_CHANGED, iEventFlags, NULL);

   /// [LINE COUNT] Recalculate scrollbar ranges, repaint all lines following the input line
   if (iUpdateFlags INCLUDES CCF_LINES_CHANGED)
   {
      // Recalculate scrollbar ranges
      updateCodeEditScrollBarLimits(pWindowData);

      /// Repaint current and subsequent lines
      for (UINT iCurrentLine = iLineNumber; iCurrentLine < (pWindowData->ptFirstCharacter.iLine + pWindowData->siPageSize.cy); iCurrentLine++)
         // Invalidate line
         invalidateCodeEditLine(pWindowData, iCurrentLine);
   }

   // Redraw window
   UpdateWindow(pWindowData->hWnd);

   // [TRACK]
   END_TRACKING();
}


/// Function name  : updateCodeEditLineCommand
// Description     : Generates a COMMAND from the text within LineData, stores it, and extracts the relevant properties
// 
// CONST CODE_EDIT_DATA*  pWindowData      : [in]            Window data
// CODE_EDIT_LINE*        pLineData        : [in/out]        LineData to save COMMAND and properties into
// CONST COMMAND*         pExistingCommand : [in] [optional] Existing COMMAND to duplicate. If this NULL then a new COMMAND is generated from the line text
// 
VOID  updateCodeEditLineCommand(CONST CODE_EDIT_DATA*  pWindowData, CODE_EDIT_LINE*  pLineData, CONST COMMAND*  pExistingCommand)
{
   ERROR_STACK*  pException;     // Exception
   TCHAR*        szLineText;     // Text of the input line

   // Prepare
   TRACK_FUNCTION();

   // [CHECK] Do not attempt to generate if we have no game data 
   if (getAppState() == AS_GAME_DATA_LOADED)        // Happens in the 'Preview' CodeEdit in the preferences dialog
   {
      // Prepare
      szLineText = generateCodeEditLineText(pLineData);

      // Delete any existing COMMAND
      if (pLineData->pCommand)
         deleteCommand(pLineData->pCommand);

      __try
      {
         /// [INPUT COMMAND] Duplicate the input COMMAND
         if (pExistingCommand)
            pLineData->pCommand = duplicateCommand(pExistingCommand);
         
         /// [GENERATE COMMAND] Generate a new COMMAND from the current line text
         else
         {
            // Ensure line has an error queue
            if (pLineData->pErrorQueue)
               deleteErrorQueue(pLineData->pErrorQueue);
            pLineData->pErrorQueue = createErrorQueue();

            /// Reset existing generation data
            initScriptFileGenerator(pWindowData->pScriptFile);
            
            // Generate new COMMAND from the current line text
            generateCommandFromString(pWindowData->pScriptFile, szLineText, NULL, pLineData->pCommand, pLineData->pErrorQueue);

            // [CHECK] Determine severity of the errors
            if (hasErrors(pLineData->pErrorQueue))
               // [SUCCESS] Determine severity indicator
               pLineData->eSeverity = identifyErrorQueueType(pLineData->pErrorQueue);
            else
               // [FAILURE] Remove the ErrorQueue to indicate there were no errors
               deleteErrorQueue(pLineData->pErrorQueue);
         }
      }
      __except (generateExceptionError(GetExceptionInformation(), pException))
      {
         // [ERROR] "An unidentified and unexpected critical error has occurred while compiling line '%s' of script '%s'"
         enhanceError(pException, ERROR_ID(IDS_EXCEPTION_UPDATE_LINE_COMMAND), szLineText, pWindowData->pScriptFile->szScriptname);
         SendMessage(getAppWindow(), UN_CODE_EDIT_EXCEPTION, (WPARAM)pWindowData->hWnd, (LPARAM)pException);

         // [CHECK] Ensure COMMAND exists
         if (!pLineData->pCommand)
         {
            pLineData->pCommand      = createCommandFromText(szLineText, NULL);
            pLineData->pCommand->iID = CMD_NOP;
         }
      }

      /// Store conditional
      pLineData->eConditional = pLineData->pCommand->eConditional;

      /// Store command ID.  Extract special conditionals manually.
      switch (pLineData->iCommandID = pLineData->pCommand->iID)
      {
      case CMD_ELSE:          pLineData->eConditional = CI_ELSE;        break;
      case CMD_BREAK:         pLineData->eConditional = CI_BREAK;       break;
      case CMD_CONTINUE:      pLineData->eConditional = CI_CONTINUE;    break;
      case CMD_END:           pLineData->eConditional = CI_END;         break;
      case CMD_END_SUB:       pLineData->eConditional = CI_END_SUB;     break;
      case CMD_DEFINE_LABEL:  pLineData->eConditional = CI_LABEL;       break;
      }

      // Determine whether COMMAND is interruptable
      if (!isCommandType(pLineData->pCommand, CT_CMD_COMMENT))
         pLineData->bInterrupt = isCommandInterruptable(pLineData->pCommand->pSyntax);

      // [COMMAND COMMENTS] Set the Command ID manually and remove any conditional
      else
      {
         pLineData->iCommandID   = CMD_COMMENT;
         pLineData->eConditional = CI_NONE;
         pLineData->bInterrupt   = FALSE;
      }

      // Cleanup
      utilDeleteString(szLineText);
   }

   // [TRACK]
   END_TRACKING();
}


