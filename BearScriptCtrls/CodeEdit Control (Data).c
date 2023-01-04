//
// CodeEdit Data (new).cpp : Handles the data used to hold the CodeEdit text
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

// onException: Pass to MainWindow for display
#define  ON_EXCEPTION()         debugCodeEditData(pWindowData);  SendMessage(getAppWindow(), UN_CODE_EDIT_EXCEPTION, NULL, (LPARAM)pException);

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createCodeEditCharacter
// Description     : Creates a new character object for a CodeEdit line object
// 
// CONST TCHAR  chCharacter : [in] Character to represent
// 
// Return Value   : New CodeEditCharacter object, you are responsible for destroying it
// 
CODE_EDIT_CHARACTER*  createCodeEditCharacter(CONST TCHAR  chCharacter)
{
   CODE_EDIT_CHARACTER*  pCharacterData;

   // Create object
   pCharacterData = utilCreateEmptyObject(CODE_EDIT_CHARACTER);

   // Set properties
   pCharacterData->chCharacter = chCharacter;

   // Return object
   return pCharacterData;
}


/// Function name  : createCodeEditData
// Description     : Creates window data for a CodeEdit control
// 
// HWND  hWnd   : [in] Window handle of the CodeEdit control
// 
// Return Value   : New CodeEditData object, you are responsible for destroying it
// 
CODE_EDIT_DATA*  createCodeEditData(HWND  hWnd)
{
   CODE_EDIT_DATA*     pWindowData;
   CODE_EDIT_LOCATION  ptInitialCaretPosition;

   // Create object
   pWindowData = utilCreateEmptyObject(CODE_EDIT_DATA);

   // Set initial properties
   pWindowData->hWnd         = hWnd;
   pWindowData->iLongestLine = 64;        // Set to something ridiculously small to represent an empty CodeEdit
   
   // Use global preferences by default
   setCodeEditPreferences(pWindowData, getAppPreferences());

   // Create line data and an initial empty line
   pWindowData->pLineList = createList(destructorCodeEditLine);
   insertCodeEditLineByIndex(pWindowData, 0, createCodeEditLine());

   // [GLOBAL SCOPE] Set initial scope
   pWindowData->pCurrentScope = createCodeEditLabel(TEXT(""), 0);

   // [UNDO] Create undo stack
   pWindowData->oUndo.pUndoStack = createStack(destructorCodeEditUndoItem);
   pWindowData->oUndo.pRedoStack = createStack(destructorCodeEditUndoItem);

   // Position caret at {0,0} 
   ptInitialCaretPosition.iLine = ptInitialCaretPosition.iIndex = 0;
   setCodeEditCaretLocation(pWindowData, &ptInitialCaretPosition);

   // Return object
   return pWindowData;
}


/// Function name  : createCodeEditLine
// Description     : Creates an empty line data object
// 
// Return Value   : New CodeEditLine object, you are responsible for destroying it
// 
CODE_EDIT_LINE*  createCodeEditLine()
{
   CODE_EDIT_LINE*  pLineData;

   // Create object
   pLineData = utilCreateEmptyObject(CODE_EDIT_LINE);

   // Create character data
   pLineData->pCharacterList = createList(destructorCodeEditCharacter);

   // Set initial command properties
   pLineData->iCommandID   = CMD_NOP;
   pLineData->eConditional = CI_NONE;
   pLineData->bInterrupt   = FALSE;

   // Set initial indentation to 0
   pLineData->iIndent = 0;

   // Return object
   return pLineData;
}


/// Function name  : createCodeEditLineFromCommand
// Description     : Creates a new line data object containing the text and properties of a COMMAND object
// 
// CONST CODE_EDIT_DATA*  pWindowData : [in] Window Data
// CONST COMMAND*         pCommand    : [in] COMMAND containing the data to be displayed by the line
// 
// Return Value   : New CodeEditLine object, you are responsible for destroying it
// 
CODE_EDIT_LINE*  createCodeEditLineFromCommand(CONST CODE_EDIT_DATA*  pWindowData, CONST COMMAND*  pCommand)
{
   CODE_EDIT_LINE*  pLineData;

   TRY
   {
      // Create empty line
      pLineData = createCodeEditLine();

      /// Append input text
      for (UINT iIndex = 0; pCommand->szBuffer[iIndex] ;iIndex++)
         appendCodeEditCharacterToLine(pLineData, pCommand->szBuffer[iIndex]);

      /// Duplicate translation errors, if any
      if (hasErrors(pCommand->pErrorQueue))
      {
         pLineData->pErrorQueue = duplicateErrorQueue(pCommand->pErrorQueue);
         pLineData->eSeverity   = identifyErrorQueueType(pCommand->pErrorQueue);
      }

      /// Duplicate input COMMAND and extract the relevant properties
      updateCodeEditLineCommand(pWindowData, pLineData, pCommand);
      
      // Return object
      return pLineData;
   }
   CATCH0("");
   debugCommand(pCommand);

   // Return empty line
   return createCodeEditLine();
}


/// Function name  : createCodeFragment
// Description     : Creates a CodeFragment object, for drawing contiguous blocks of text in a CodeEdit
// 
// CONST CODE_EDIT_LINE*  pLineData     : [in] LineData of the line to draw
// CONST COLOUR_SCHEME*   pColourScheme : [in] Current colour scheme
// CONST SCRIPT_FILE*     pScriptFile   : [in] ScriptFile
// 
// Return Value   : New CodeFragment object, you are responsible for destroying it
// 
CODE_FRAGMENT*  createCodeFragment(CONST CODE_EDIT_LINE*  pLineData, CONST COLOUR_SCHEME*  pColourScheme, CONST SCRIPT_FILE*  pScriptFile)
{
   CODE_FRAGMENT*  pCodeFragment;

   // Create object
   pCodeFragment = utilCreateEmptyObject(CODE_FRAGMENT);

   // Set properties
   pCodeFragment->pLineData     = pLineData;
   pCodeFragment->pColourScheme = pColourScheme;
   pCodeFragment->pScriptFile   = pScriptFile;

   // Setup CodeObject
   pCodeFragment->szSource    = generateCodeEditLineText(pLineData);
   pCodeFragment->pCodeObject = createCodeObject(pCodeFragment->szSource);

   // Initialise character data iterator
   findCodeEditCharacterListItemByIndex(pLineData, 0, pCodeFragment->pCharacterIterator);

   // Return object
   return pCodeFragment;
}


/// Function name  : deleteCodeEditCharacter
// Description     : Destroys a CodeEditCharacter object
// 
// CODE_EDIT_CHARACTER*  &pCharacterData : [in] object to destroy
// 
VOID   deleteCodeEditCharacter(CODE_EDIT_CHARACTER*  &pCharacterData)
{
   // Delete calling object
   utilDeleteObject(pCharacterData);
}


/// Function name  : destructorCodeEditCharacter
// Description     : Destroys a CodeEditCharacter object within a ListItem
// 
// LPARAM  pCharacterData : [in] Pointer to a CodeEditCharacter object
// 
VOID  destructorCodeEditCharacter(LPARAM  pCharacterData)
{
   // Delete list item
   deleteCodeEditCharacter((CODE_EDIT_CHARACTER*&)pCharacterData);
}


/// Function name  : deleteCodeEditData
// Description     : Destroys window data for a CodeEdit control
// 
// CODE_EDIT_DATA*  &pWindowData   : [in] Window data to destroy
// 
VOID  deleteCodeEditData(CODE_EDIT_DATA*  &pWindowData)
{
   HFONT  hCodeFont;
   HDC    hDC;

   // Delete font
   hDC       = GetDC(pWindowData->hWnd);
   hCodeFont = SelectFont(hDC, GetStockObject(ANSI_FIXED_FONT));
   DeleteObject(hCodeFont);

   // Delete line data
   deleteList(pWindowData->pLineList);

   // Delete scope
   deleteCodeEditLabel(pWindowData->pCurrentScope);

   // Delete undo stack
   deleteStack(pWindowData->oUndo.pUndoStack);
   deleteStack(pWindowData->oUndo.pRedoStack);

   // Cleanup and Delete calling object
   ReleaseDC(pWindowData->hWnd, hDC);
   utilDeleteObject(pWindowData);
}


/// Function name  : deleteCodeEditLine
// Description     : Destroys CodeEdit line data
// 
// CODE_EDIT_LINE*  &pLineData   : [in] Line data to destroy
// 
VOID  deleteCodeEditLine(CODE_EDIT_LINE*  &pLineData)
{
   // Delete character list
   deleteList(pLineData->pCharacterList);

   // Delete error queue, if any
   if (pLineData->pErrorQueue)
      deleteErrorQueue(pLineData->pErrorQueue);

   // Delete COMMAND, if any
   if (pLineData->pCommand)
      deleteCommand(pLineData->pCommand);

   // Delete calling object
   utilDeleteObject(pLineData);
}


/// Function name  : destructorCodeEditLine
// Description     : Destroys CodeEdit line data stored in a ListItem
// 
// LPARAM  pLineData   : [in] Pointer to a CodeEditLine object
// 
VOID  destructorCodeEditLine(LPARAM  pLineData)
{
   // Delete list item
   deleteCodeEditLine((CODE_EDIT_LINE*&)pLineData);
}


/// Function name  : deleteCodeFragment
// Description     : Destroys a CodeFragment and the CodeObject within
// 
// CODE_FRAGMENT*  &pCodeFragment : [in] CodeFragment to destroy 
// 
VOID  deleteCodeFragment(CODE_FRAGMENT*  &pCodeFragment)
{
   // Delete source text
   utilDeleteString(pCodeFragment->szSource);

   // Delete CodeObject
   deleteCodeObject(pCodeFragment->pCodeObject);

   // Delete calling object
   utilDeleteObject(pCodeFragment);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : appendCodeEditCharacterToLine
// Description     : Appends a new character to a line of CodeEdit text
// 
// CODE_EDIT_LINE* pLineData   : [in] Line to be appended
// CONST TCHAR     chCharacter : [in] Character to append
// 
VOID  appendCodeEditCharacterToLine(CODE_EDIT_LINE* pLineData, CONST TCHAR  chCharacter)
{
   // Insert character at the end of the line
   insertCodeEditCharacterByIndex(pLineData, getCodeEditLineLength(pLineData), chCharacter);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : findCodeEditCharacterDataByIndex
// Description     : Search for Character data in a specified line, by index
// 
// CONST CODE_EDIT_LINE*  pLineData  : [in]  Line data containing the character data
// CONST UINT             iIndex     : [in]  Zero based character index
// CODE_EDIT_CHARACTER*  &pOutput    : [out] Character data if found, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BOOL  findCodeEditCharacterDataByIndex(CONST CODE_EDIT_LINE*  pLineData, CONST UINT  iIndex, CODE_EDIT_CHARACTER*  &pOutput)
{
   LIST_ITEM*  pListItem;

   // Prepare
   pOutput = NULL;

   // Lookup list item
   if (findListItemByIndex(pLineData->pCharacterList, iIndex, pListItem))
      pOutput = extractListItemPointer(pListItem, CODE_EDIT_CHARACTER);

   // Return TRUE if item was found
   return (pOutput != NULL);
}


/// Function name  : findCodeEditCharacterListItemByIndex
// Description     : Search for the list item containing the desired character data, by index
// 
// CONST CODE_EDIT_LINE*  pLineData : [in]  LineData to search
// CONST UINT             iIndex    : [in]  Zero based character index
// LIST_ITEM*            &pOutput   : [out] ListItem containing the desired character data
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BOOL  findCodeEditCharacterListItemByIndex(CONST CODE_EDIT_LINE*  pLineData, CONST UINT  iIndex, LIST_ITEM*  &pOutput)
{
   // Lookup list item
   return findListItemByIndex(pLineData->pCharacterList, iIndex, pOutput);
}


/// Function name  : findCodeEditLineDataByIndex
// Description     : Searches for line data within a CodeEdit, by Line number
// 
// CONST CODE_EDIT_DATA*  pWindowData   : [in]  CodeEdit window data to search
// CONST UINT              iLineNumber   : [in]  Zero based line number
// CODE_EDIT_LINE*        &pOutput       : [out] Line data if found, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BOOL  findCodeEditLineDataByIndex(CONST CODE_EDIT_DATA*  pWindowData, CONST UINT  iLineNumber, CODE_EDIT_LINE*  &pOutput)
{
   LIST_ITEM*  pListItem;

   // Prepare
   pOutput = NULL;

   // Lookup list item
   if (findListItemByIndex(pWindowData->pLineList, iLineNumber, pListItem))
      pOutput = extractListItemPointer(pListItem, CODE_EDIT_LINE);

   // Return TRUE if item was found
   return (pOutput != NULL);
}


/// Function name  : findCodeEditLineListItemByIndex
// Description     : Searches for the list item containing the desired line data within a CodeEdit
// 
// CONST CODE_EDIT_DATA*  pWindowData : [in]  Window data
// CONST UINT              iIndex      : [in]  Zero based line number
// LIST_ITEM*             &pOutput     : [out] Line data if found, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BOOL  findCodeEditLineListItemByIndex(CONST CODE_EDIT_DATA*  pWindowData, CONST UINT  iIndex, LIST_ITEM*  &pOutput)
{
   // Lookup list item
   return findListItemByIndex(pWindowData->pLineList, iIndex, pOutput);
}



/// Function name  : findNextCodeFragment
// Description     : Retrieve the next 'fragment' of script text that has contiguous graphical properties. 
//                      ie. text colour and selection state
// 
// CODE_FRAGMENT*  pCodeFragment : [in/out] CodeFragment object
// 
// Return Value   : TRUE if found, FALSE if no more fragments remain
// 
BOOL  findNextCodeFragment(CODE_FRAGMENT*  pCodeFragment)
{
   CODEOBJECT*&   pCodeObject          = pCodeFragment->pCodeObject;             // Convenience reference for the CodeObject within the CodeFragment
   LIST_ITEM*&    pIterator            = pCodeFragment->pCharacterIterator;      // Convenience reference for the CharacterData iterator within the CodeFragment
   BOOL           bFragmentRemaining;                                            // Whether or not there is still unprocessed text remaining from the previous CodeFragment

   // Determine whether previous fragment was partial or complete
   bFragmentRemaining = (pCodeFragment->iLength != pCodeObject->iLength);

   /// [FRAGMENT REMAINING] Move to the next fragment within current CodeObject
   if (bFragmentRemaining)
   {
      // Move to the next CodeFragment
      pCodeFragment->szText += pCodeFragment->iLength;
      pCodeFragment->iLength = lstrlen(pCodeFragment->szText);

      // [CHECK] Have we consumed all the fragment yet?
      if (pCodeFragment->iLength == 0)
         bFragmentRemaining = FALSE;
   }

   /// [FRAGMENT COMPLETE] Create new fragment from the next CodeObject
   if (!bFragmentRemaining)
   {
      // [CHECK] Have we consumed all the CodeObjects yet?
      if (!findNextCodeObjectWithArguments(pCodeObject, pCodeFragment->pScriptFile))
         /// [NOT FOUND] Return FALSE
         return FALSE;

      // Update CodeFragment text
      pCodeFragment->szText   = &pCodeObject->szText[0];
      pCodeFragment->iLength  = pCodeObject->iLength;

      // [COLOUR SCHEME] Calculate fragment colour
      pCodeFragment->clColour = getInterfaceColour(pCodeFragment->pColourScheme->eCodeObjectColours[pCodeObject->eClass]);
   }

   // Determine initial selection state
   pCodeFragment->bSelected = extractListItemPointer(pIterator, CODE_EDIT_CHARACTER)->bSelected;

   /// Ensure selection state is contiguous
   for (UINT iCharIndex = 0; iCharIndex < pCodeFragment->iLength; iCharIndex++)
   {
      // Ensure current character matches the CodeFragment
      if (pCodeFragment->bSelected == extractListItemPointer(pIterator, CODE_EDIT_CHARACTER)->bSelected)
         pIterator = pIterator->pNext;

      /// [PARTIAL FRAGMENT] Set fragment length (also aborts search)
      else
      {
         pCodeFragment->iLength = iCharIndex;
         break;
      }
   }

   /// [FOUND] Return TRUE
   return TRUE;
}


/// Function name  : findNextCodeEditWord
// Description     : Return character index of the next word or end of line
// 
// CONST CODE_EDIT_LINE*  pLineData     : [in] LineData for the line containing the word
// CONST UINT             iCurrentIndex : [in] Character index to search from
// 
// Return Value   : character index of the next word or end of line
// 
UINT  findNextCodeEditWord(CONST CODE_EDIT_LINE*  pLineData, CONST UINT  iCurrentIndex)
{
   CODEOBJECT*  pCodeObject;
   CONST TCHAR* szMarker;
   TCHAR*       szLineText;
   BOOL         bFound;
   UINT         iResult;

   // Prepare
   bFound     = FALSE;
   szLineText = generateCodeEditLineText(pLineData);

   // [CHECK]
   if (FALSE)
   {
      // Create CodeObject from input line
      pCodeObject = createCodeObject(szLineText);
      iResult     = getCodeEditLineLength(pLineData);
      
      /// Iterate through CodeObjects
      while (!bFound AND findNextCodeObject(pCodeObject))
      {
         // [WHITESPACE] Skip Whitespace
         if (pCodeObject->eClass == CO_WHITESPACE)
            continue;
         
         /// Abort when we reach a CodeObject who's start follows the input index
         if (pCodeObject->iStartCharIndex > iCurrentIndex)
         {
            bFound  = TRUE;
            iResult = pCodeObject->iStartCharIndex;
         }
      }

      // Cleanup
      deleteCodeObject(pCodeObject);
   }
   /// [NEW METHOD] Search for special character
   else if (szMarker = utilFindCharacterInSet(&szLineText[iCurrentIndex], " .=*-[](){}:<>"))
      iResult = szMarker - szLineText + 1;

   // [EOF]
   else
      iResult = getCodeEditLineLength(pLineData);

   // Cleanup and Return index of next CodeObject / end of line
   utilDeleteString(szLineText);
   return iResult;
}


/// Function name  : findPrevCodeEditWord
// Description     : Return character index of the previous word or beginning of the line
// 
// CONST CODE_EDIT_LINE*  pLineData     : [in] LineData for the line containing the word
// CONST UINT             iCurrentIndex : [in] Character index to search from
// 
// Return Value   : character index of the previous word or beginning of the line
// 
UINT  findPrevCodeEditWord(CONST CODE_EDIT_LINE*  pLineData, CONST UINT  iCurrentIndex)
{
   CODEOBJECT*  pCodeObject;
   CONST TCHAR* szMarker;
   TCHAR*       szLineText;
   BOOL         bFound;
   UINT         iLastStartIndex;

   // Prepare
   iLastStartIndex = 0;
   szLineText      = generateCodeEditLineText(pLineData);
   bFound          = FALSE;
   
   if (FALSE)
   {
      // Create CodeObject from input line
      pCodeObject = createCodeObject(szLineText);
      
      /// Iterate through CodeObjects
      while (!bFound AND findNextCodeObject(pCodeObject))
      {
         // [WHITESPACE] Skip Whitespace
         if (pCodeObject->eClass == CO_WHITESPACE)
            continue;
         
         /// Abort when we reach the CodeObject at or following the input index
         if (pCodeObject->iStartCharIndex >= iCurrentIndex)
            bFound = TRUE;
         else
            // Save current start index
            iLastStartIndex = pCodeObject->iStartCharIndex;
      }

      // Cleanup
      deleteCodeObject(pCodeObject);
   }
   /// [NEW METHOD] Search for special character
   else 
   {
      // Truncate and reverse string
      szLineText[iCurrentIndex] = NULL;
      utilReverseString(szLineText);

      // Search for previous marker
      if (szMarker = utilFindCharacterInSet(szLineText, " .=*-[](){}:<>"))
         iLastStartIndex = lstrlen(szLineText) - (szMarker - szLineText + 1);
   }

   // Return index of prev CodeObject / start of line
   utilDeleteString(szLineText);
   return max(0, iLastStartIndex); //(bFound ? iLastStartIndex : 0);
}


/// Function name  : insertCodeEditCharacterByIndex
// Description     : Creates CodeEdit character data from a character and inserts it into CodeEdit line data at the specified position
// 
// CODE_EDIT_LINE*  pLineData   : [in/out] Line to insert the character into
// CONST UINT       iIndex      : [in]     Zero-based character index. If a character exists here, it will be shifted RIGHT
// CONST TCHAR      chCharacter : [in]     Character to insert
// 
VOID  insertCodeEditCharacterByIndex(CODE_EDIT_LINE*  pLineData, CONST UINT  iIndex, CONST TCHAR  chCharacter)
{
   // [CHECK] Ensure index is valid
   ASSERT(iIndex <= getCodeEditLineLength(pLineData));

   // Insert at specified index
   insertObjectIntoListByIndex(pLineData->pCharacterList, iIndex, (LPARAM)createCodeEditCharacter(chCharacter));
}


/// Function name  : insertCodeEditLineByIndex
// Description     : Inserts line data into a CodeEdit control at the specified position
// 
// CODE_EDIT_DATA*  pWindowData : [in/out] Window data
// CONST UINT        iIndex      : [in]     Zero-based line number of the new line. If a line already exists here, it will be shifted DOWN.
// CODE_EDIT_LINE*   pLineData   : [in]     Line data of the line to insert
// 
VOID  insertCodeEditLineByIndex(CODE_EDIT_DATA*  pWindowData, CONST UINT  iIndex, CODE_EDIT_LINE*  pLineData)
{
   // [CHECK] Ensure index is valid
   ASSERT(iIndex <= getCodeEditLineCount(pWindowData));

   /// Insert at specified index
   insertObjectIntoListByIndex(pWindowData->pLineList, iIndex, (LPARAM)pLineData);
}


/// Function name  : removeCodeEditCharacterByIndex
// Description     : Locates and destroys the character data for a specific character in a CodeEdit control
// 
// CODE_EDIT_LINE*  pLineData : [in/out] Line data for the line containing the character to destroy
// CONST UINT       iIndex    : [in]     Zero-based index of the character to destroy
// 
// Return Value : TRUE if found, otherwise FALSE
//
BOOL  removeCodeEditCharacterByIndex(CODE_EDIT_LINE*  pLineData, CONST UINT  iIndex)
{
   LIST_ITEM*  pTargetItem;      // ListItem wrapper
   BOOL        bResult;          // Return

   // [CHECK] Ensure character exists
   //ASSERT(findListItemByIndex(pLineData->pCharacterList, iIndex, pTargetItem));

   /// Remove character from list
   if (bResult = removeItemFromListByIndex(pLineData->pCharacterList, iIndex, pTargetItem))
      // Delete item
      deleteListItem(pLineData->pCharacterList, pTargetItem, TRUE);
   
   // Return result
   return bResult;
}


/// Function name  : removeCodeEditLineByIndex
// Description     : Locates and destroys the line data for a specific line in a CodeEdit control
// 
// CODE_EDIT_DATA*  pWindowData : [in/out] Window data
// CONST UINT        iIndex      : [in]     Zero-based index of the line to destroy
// 
VOID  removeCodeEditLineByIndex(CODE_EDIT_DATA*  pWindowData, CONST UINT  iIndex)
{
   LIST_ITEM*  pTargetItem;      // ListItem wrapper

   // [CHECK] Ensure line exists
   ASSERT(findListItemByIndex(pWindowData->pLineList, iIndex, pTargetItem));

   /// Find desired line
   if (findListItemByIndex(pWindowData->pLineList, iIndex, pTargetItem))
   {
      /// Remove line from list
      removeItemFromList(pWindowData->pLineList, pTargetItem);

      // Delete line data and ListItem
      deleteListItem(pWindowData->pLineList, pTargetItem, TRUE);
   }
}
