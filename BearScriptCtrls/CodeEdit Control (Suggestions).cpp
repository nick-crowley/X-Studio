//
// CodeEdit (Suggestions).cpp : FILE_DESCRIPTION
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 


#include "stdafx.h"

// onException: Pass to MainWindow for display
#define  ON_EXCEPTION()         debugCodeEditData(pWindowData);  SendMessage(getAppWindow(), UN_CODE_EDIT_EXCEPTION, NULL, (LPARAM)pException);

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////

// Defines the different ListView sizes for each suggestion type
//
CONST SIZE  siListViewSizes[CST_SCRIPT_OBJECT + 1] = 
{
   {   0,   0 },     // CST_NONE
   { 260, 180 },     // CST_VARIABLE
   { 560, 180 },     // CST_COMMAND
   { 350, 180 },     // CST_GAME_OBJECT
   { 350, 180 },     // CST_SCRIPT_OBJECT
};

CONST UINT  iTypeColumnWidths[CST_SCRIPT_OBJECT + 1] = 
{
   NULL,   // CST_NONE
   70,     // CST_VARIABLE
   NULL,   // CST_COMMAND
   70,     // CST_GAME_OBJECT
   120,    // CST_SCRIPT_OBJECT
};

// Column IDs
#define    OBJECT_COLUMN_NAME                 0
#define    OBJECT_COLUMN_TYPE                 1

static const TCHAR*  szSuggestionTypes[5] = { TEXT("None"), TEXT("Variables"), TEXT("Commands"), TEXT("Game Objects"), TEXT("Script Objects"),  };

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createCodeEditSuggestionList
// Description     : Creates the Suggestion ListView at the suggestion origin
// 
// CODE_EDIT_DATA*        pWindowData  : [in] Window data
// CONST SUGGESTION_TYPE  eType        : [in] Type of suggestions to display
// 
// Return Value   : TRUE if successful, otherwise FALSE
// 
BOOL   createCodeEditSuggestionList(CODE_EDIT_DATA*  pWindowData)
{
   LISTVIEW_COLUMNS      oListView = { 1, IDS_AUTOCOMPLETE_COLUMN_NAME, 400, iTypeColumnWidths[pWindowData->oSuggestion.eType], NULL, NULL, NULL };
   CODE_EDIT_SUGGESTION* pSuggestion;         // Convenience pointer
   POINT                 ptOrigin;            // Position of the Suggestion Origin
   
   // [CHECK] Ensure control doesn't exist and there are results to be displayed
   ASSERT(!pWindowData->oSuggestion.hCtrl AND getTreeNodeCount(pWindowData->oSuggestion.pResultsTree));

   // Prepare
   pSuggestion = &pWindowData->oSuggestion;
   
   // calculate co-ordinates of the Suggestion Origin
   if (calculateCodeEditPointFromLocation(pWindowData, &pSuggestion->ptOrigin, &ptOrigin))
   {
      // [TOOLTIP] Destroy tooltip, if present
      displayCodeEditTooltip(pWindowData, FALSE, NULL, NULL);

      /// Create invisible ListView beneath the caret
      pSuggestion->hCtrl = CreateWindow(WC_LISTVIEW, NULL, LVS_OWNERDATA WITH LVS_SHOWSELALWAYS WITH LVS_REPORT WITH LVS_SINGLESEL WITH LVS_NOCOLUMNHEADER WITH WS_CHILD WITH WS_VSCROLL,
                                        ptOrigin.x, ptOrigin.y + pWindowData->siCharacterSize.cy + 1, CW_USEDEFAULT, CW_USEDEFAULT, 
                                        pWindowData->hWnd, (HMENU)IDC_SUGGESTION_LIST, getControlsInstance(), NULL);  

      SILENT_ERROR_CHECK("creating suggestion ListView", pSuggestion->hCtrl);

      // [CHECK] Ensure control was created successfully
      if (pSuggestion->hCtrl)
      {
         CONSOLE("Displaying '%s' suggestions for char %d line %d", szSuggestionTypes[pSuggestion->eType], pSuggestion->ptOrigin.iIndex, pSuggestion->ptOrigin.iLine);

         // [COLUMNS] Display one column for commands, two columns for everything else
         oListView.iColumnCount = (pSuggestion->eType != CST_COMMAND ? 2 : 1);
         
         // Setup ListView
         initReportModeListView(pSuggestion->hCtrl, &oListView, FALSE);
         
         // Set item count and resize ListView to fit
         ListView_SetItemCount(pSuggestion->hCtrl, getTreeNodeCount(pSuggestion->pResultsTree));
         updateCodeEditSuggestionListSize(pWindowData);

         // Display 
         ShowWindow(pSuggestion->hCtrl, SW_SHOW);
      }
   }
   
   // Return TRUE if successfully created
   return (pSuggestion->hCtrl != NULL);
}


/// Function name  : destroyCodeEditSuggestionList
// Description     : Hides the Suggestion ListView and destroys the results tree
// 
// CODE_EDIT_DATA*  pWindowData   : [in] Window data
// 
VOID  destroyCodeEditSuggestionList(CODE_EDIT_DATA*  pWindowData)
{
   CODE_EDIT_SUGGESTION*   pSuggestion;         // Convenience pointer
   HWND                    hCtrl;               // Used to prevent function being executed twice

   // Prepare
   pSuggestion = &pWindowData->oSuggestion;

   /// [CHECK] Ensure control DOES exist
   if (pSuggestion->hCtrl)
   {
      //CONSOLE("Destroying Suggestions ListView");

      // Preserve the window handle
      hCtrl = pSuggestion->hCtrl;      /// Prevent re-entry from LoseFocus handler upon destruction
      pSuggestion->hCtrl = NULL;

      // Destroy the control and results tree
      utilDeleteWindow(hCtrl);      
      deleteAVLTree(pSuggestion->pResultsTree);

      // Zero the suggestion type
      pSuggestion->eType = CST_NONE;
   }
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : findCodeEditSuggestionByIndex
// Description     : Searches for the text of an item within the Suggestion results tree
// 
// CODE_EDIT_DATA*   pWindowData : [in]  Window data
// CONST UINT        iIndex      : [in]  Zero-based logical item index
// CONST TCHAR*     &szOutput    : [out] Item text if found, otherwise NULL
// 
// Return Value   : TRUE if found, otherwise FALSE
// 
BOOL   findCodeEditSuggestionByIndex(CODE_EDIT_DATA*  pWindowData, CONST UINT  iIndex, CONST TCHAR*  &szOutput)
{
   CODE_EDIT_SUGGESTION*   pSuggestion;      // Convenience pointer
   SUGGESTION_RESULT       eResult;          // Command, GameObject, ScriptObject or VariableName in the Suggestion Results tree

   // Prepare
   szOutput    = NULL;
   pSuggestion = &pWindowData->oSuggestion;

   /// [SUCCESS] Attempt to lookup item
   if (pSuggestion->pResultsTree AND findObjectInAVLTreeByIndex(pSuggestion->pResultsTree, iIndex, eResult.asPointer))
   {
      // [SUCCESS] Extract text
      switch (pSuggestion->eType)
      {
      case CST_GAME_OBJECT:      
      case CST_SCRIPT_OBJECT:    szOutput = eResult.asObjectName->szText;              break;
      case CST_VARIABLE:         szOutput = eResult.asVariableName->szName;            break;
      case CST_COMMAND:          szOutput = eResult.asCommandSyntax->szSuggestion; break;
      }
   }

   // Return TRUE if found, otherwise FALSE
   return (szOutput != NULL);
}


/// Function name  : isCodeObjectSuggestionType
// Description     : 
// 
// CONST CODEOBJECT*      pCodeObject : [in] 
// CONST SUGGESTION_TYPE  eType       : [in] 
// 
// Return Value   : 
// 
BOOL  isCodeObjectSuggestionType(CONST CODEOBJECT*  pCodeObject, CONST SUGGESTION_TYPE  eType)
{
   BOOL  bResult;

   /// Compare Suggestion type and CodeObject class
   switch (pCodeObject->eClass)
   {
   // [ENUMERATION] Suggest GameObjects
   case CO_ENUMERATION:    bResult = (eType == CST_GAME_OBJECT);     break;
   // [CONSTANT] Suggest ScriptObjects
   case CO_CONSTANT:       bResult = (eType == CST_SCRIPT_OBJECT);   break;
   // [VARIABLE] Suggest Variables
   case CO_NULL:
   case CO_VARIABLE:       bResult = (eType == CST_VARIABLE);        break;
   // [WORD/WHITESPACE] Suggest Commands
   case CO_WORD:
   case CO_KEYWORD:
   case CO_OPERATOR:
   case CO_WHITESPACE:     bResult = (eType == CST_COMMAND);         break;
   // [OTHER] Return FALSE
   default:                bResult = FALSE;                          break;
   }

   // Return result
   return bResult;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : calculateCodeEditSuggestionOrigin
// Description     : Calculates and stores the position of the SuggestionOrigin for any suggestion type
// 
// CODE_EDIT_DATA*  pWindowData : [in] Window data
// CONST UINT       iLine       : [in] Line containing the suggestion
// CONST UINT       iCharIndex  : [in] Character containing the CodeObject representing the suggestion
// 
VOID  calculateCodeEditSuggestionOrigin(CODE_EDIT_DATA*  pWindowData, CONST UINT  iLine, CONST UINT  iCharIndex)
{
   CODE_EDIT_SUGGESTION*  pSuggestion;    // Convenience pointer
   CODE_EDIT_LINE*        pLineData;      // Line data
   CONST TCHAR*           szMarker;       // Position of the reference operator, if any
   CODEOBJECT*            pCodeObject;    // CodeObject
   TCHAR*                 szLineText;     // Line text
   BOOL                   bSearching;     // Search flag

   // Prepare
   pSuggestion = &pWindowData->oSuggestion;

   // Get line text
   findCodeEditLineDataByIndex(pWindowData, iLine, pLineData);
   szLineText = generateCodeEditLineText(pLineData);
   
   /// [DEFAULT] Position at start of line
   pSuggestion->ptOrigin.iIndex = 0;
   pSuggestion->ptOrigin.iLine  = iLine;

   // [CHECK] Ensure line has text
   if (lstrlen(szLineText))
   {
      // Create CodeObject from line data
      pCodeObject = createCodeObject(szLineText);

      // Examine suggestion type
      switch (pSuggestion->eType)
      {
      /// [NON-COMMAND] Position at start of CodeObject
      default:
         // Lookup CodeObject at character index
         if (findCodeObjectByCharacterIndex(pCodeObject, iCharIndex))
            // [FOUND] Position at start of CodeObject
            pSuggestion->ptOrigin.iIndex = pCodeObject->iStartCharIndex;
         else
            // [END-OF-LINE] Position at last character
            pSuggestion->ptOrigin.iIndex = max(0, lstrlen(szLineText) - 1);
         break;

      /// [COMMAND] Position after (in order): reference operator, assignment operator, last keyword, start of line
      case CST_COMMAND:
         // Prepare
         bSearching = TRUE;

         /// [REFERENCE OPERATOR] Position after reference operator
         if (szMarker = utilFindSubString(szLineText, "->"))
            pSuggestion->ptOrigin.iIndex = (szMarker - szLineText + 2);

         // Iterate through CodeObjects
         else while (bSearching AND findNextCodeObject(pCodeObject))
         {
            // Examine CodeObject
            switch (pCodeObject->eClass)
            {
            /// [KEYWORD] Position after last keyword
            case CO_KEYWORD:
               // [CONDITIONAL or START] Position after keyword
               if (utilCompareString(pCodeObject->szText, "if") OR utilCompareString(pCodeObject->szText, "not") OR utilCompareString(pCodeObject->szText, "start"))
                  pSuggestion->ptOrigin.iIndex = pCodeObject->iEndCharIndex; 
               break;
            
            /// [ASSIGNMENT OPERATOR] Position after assignment operator
            case CO_OPERATOR:
               if (pCodeObject->iIndex == 2 AND utilCompareString(pCodeObject->szText, "="))
               {
                  pSuggestion->ptOrigin.iIndex = pCodeObject->iEndCharIndex;
                  bSearching = FALSE;
               }
               break;
            }
         }
         break;
      }

      // Cleanup
      deleteCodeObject(pCodeObject);
   }

   //VERBOSE("calculateCodeEditSuggestionOrigin : %u", pSuggestion->ptOrigin.iIndex);
   
   // Cleanup
   utilDeleteString(szLineText);
}


/// Function name  : findCodeEditSuggestionByText
// Description     : Finds the suggestion result that best matches the CodeObject/CommandHash at the suggestion origin
// 
// CODE_EDIT_DATA*  pWindowData   : [in] Window data
// 
// Return Value : Suggestion results tree
//
UINT   findCodeEditSuggestionByText(CODE_EDIT_DATA*  pWindowData)
{
   CODE_EDIT_SUGGESTION  *pSuggestion;           // Convenience pointer
   AVL_TREE_OPERATION    *pOperationData;        // Build query operation data
   CODE_EDIT_LINE        *pLineData;             // Data for the line containing the suggestion
   TCHAR                 *szLineText;            // Text for the line containing the suggestion
   CODEOBJECT            *pCodeObject;           // [NON-COMMAND] CodeObject at the suggesiton origin
   COMMAND_HASH          *pCommandHash;          // [COMMAND]     Hash of the line text
   UINT                   iResultIndex;          // Operation result

   // [CHECK] Ensure suggestion type is valid and line data exists
   ASSERT(pWindowData->oSuggestion.eType != CST_NONE AND findCodeEditLineDataByIndex(pWindowData, pWindowData->oSuggestion.ptOrigin.iLine, pLineData));

   // Prepare
   pSuggestion  = &pWindowData->oSuggestion;
   iResultIndex = NULL;

   // Create operation
   pOperationData = createAVLTreeOperation(treeprocFindCodeEditSuggestionResult, ATT_INORDER);
   pOperationData->xFirstInput = (LPARAM)pSuggestion->eType;

   // Generate line text
   findCodeEditLineDataByIndex(pWindowData, pSuggestion->ptOrigin.iLine, pLineData);
   szLineText = generateCodeEditLineText(pLineData);

   // Create empty tree of appropriate type
   switch (pSuggestion->eType)
   {
   /// [COMMAND] Pass the partial command hash
   case CST_COMMAND:
      // Prepare
      pCommandHash = createCommandHash();

      // Hash line text
      performScriptCommandHashing(szLineText, pCommandHash);
      pOperationData->xThirdInput = (LPARAM)pCommandHash->szCommandHash;

      // Perform search
      iResultIndex = (performOperationOnAVLTree(pWindowData->oSuggestion.pResultsTree, pOperationData) ? pOperationData->xOutput : -1);

      // Cleanup
      deleteCommandHash(pCommandHash);
      break;

   /// [REMAINDER] Pass the codeobject at the origin
   default:
      // Generate CodeObject
      pCodeObject = createCodeObject(szLineText);

      // [CHECK] Lookup CodeObject at the origin
      ASSERT(findCodeObjectByCharacterIndex(pCodeObject, pSuggestion->ptOrigin.iIndex));
      pOperationData->xSecondInput = (LPARAM)pCodeObject;

      // Perform search
      iResultIndex = (performOperationOnAVLTree(pWindowData->oSuggestion.pResultsTree, pOperationData) ? pOperationData->xOutput : -1);

      // Cleanup
      deleteCodeObject(pCodeObject);
      break;
   }

   // Cleanup and return index
   utilDeleteString(szLineText);
   deleteAVLTreeOperation(pOperationData);
   return iResultIndex;
}


/// Function name  : generateCodeEditSuggestionTree
// Description     : Builds the Suggestion results tree from the CodeObject at the caret
// 
// CODE_EDIT_DATA*  pWindowData   : [in] Window data
// 
// Return Value : Suggestion results tree
//
AVL_TREE*   generateCodeEditSuggestionTree(CODE_EDIT_DATA*  pWindowData)
{
   CODE_EDIT_SUGGESTION *pSuggestion;           // Convenience pointer
   AVL_TREE_OPERATION   *pOperationData;        // Build query operation data
   CODE_EDIT_LINE       *pLineData;             // Data for the line containing the suggestion origin
   VARIABLE_NAME        *pVariableName;         // Variable beneath the cursor
   TCHAR                *szLineText;            // Text for the line containing the suggestion origin
   CODEOBJECT           *pCodeObject;           // Used for parsing the text of the line containing the suggestion origin
   AVL_TREE             *pOutput,               // Operation result
                        *pVariableNamesTree;    // [VARIABLE] Temporary VariableNames tree generated by the CodeEdit, used as the source for the search results tree

   // [CHECK] Ensure suggestion type is valid and line data exists
   ASSERT(pWindowData->oSuggestion.eType != CST_NONE AND findCodeEditLineDataByIndex(pWindowData, pWindowData->oSuggestion.ptOrigin.iLine, pLineData));

   // Prepare
   pSuggestion = &pWindowData->oSuggestion;

   // Create operation
   pOperationData = createAVLTreeOperation(treeprocBuildCodeEditSuggestionTree, ATT_INORDER);
   pOperationData->xFirstInput = (LPARAM)pSuggestion->eType;
   pOperationData->xSecondInput = (LPARAM)pWindowData->pScriptFile;

   /// Create output tree
   switch (pSuggestion->eType)
   {
   // [COMMAND] Create CommandSyntax copy tree
   case CST_COMMAND:       pOperationData->pOutputTree = createAVLTree(extractCommandSyntaxTreeNode, NULL, createAVLTreeSortKey(AK_TEXT, AO_DESCENDING), NULL, NULL);    break;
   // [SCRIPT OBJECT] Create GameObject copy tree
   case CST_GAME_OBJECT:   pOperationData->pOutputTree = createAVLTree(extractGameObjectTreeNode, NULL, createAVLTreeSortKey(AK_TEXT, AO_DESCENDING), NULL, NULL);       break;
   // [SCRIPT OBJECT] Create ScriptObject copy tree
   case CST_SCRIPT_OBJECT: pOperationData->pOutputTree = createAVLTree(extractScriptObjectTreeNode, NULL, createAVLTreeSortKey(AK_TEXT, AO_DESCENDING), NULL, NULL);     break;
   // [VARIABLE] Create VariableNames STORAGE tree
   case CST_VARIABLE:      pOperationData->pOutputTree = createAVLTree(extractVariableNameTreeNode,  deleteVariableNameTreeNode, createAVLTreeSortKey(AK_TEXT, AO_DESCENDING), NULL, NULL);    break;
   }

   // Perform operation
   switch (pSuggestion->eType)
   {
   /// [COMMAND/GAME-OBJECT/SCRIPT-OBJECT] Build results from game data
   case CST_COMMAND:       performOperationOnAVLTree(getGameData()->pCommandTreeByID, pOperationData);            break;
   case CST_GAME_OBJECT:   performOperationOnAVLTree(getGameData()->pGameObjectsByMainType, pOperationData);      break;
   case CST_SCRIPT_OBJECT: performOperationOnAVLTree(getGameData()->pScriptObjectsByGroup, pOperationData);       break;

   /// [VARIABLE] Build results from the VariableNames tree generated by the CodeEdit
   case CST_VARIABLE:
      // Generate VariableNames tree
      pVariableNamesTree = generateCodeEditVariableNamesTree(pWindowData);

      // Generate line text at Origin
      findCodeEditLineDataByIndex(pWindowData, pSuggestion->ptOrigin.iLine, pLineData);
      szLineText  = generateCodeEditLineText(pLineData);
      
      // Lookup the CodeObject at Origin
      pCodeObject = createCodeObject(szLineText);
      findCodeObjectByCharacterIndex(pCodeObject, pSuggestion->ptOrigin.iIndex);

      // [CHECK] Is the variable beneath the cursor unique?
      if (findObjectInAVLTreeByValue(pVariableNamesTree, (LPARAM)identifyRawVariableName(pCodeObject->szText), NULL, (LPARAM&)pVariableName) AND pVariableName->iCount == 1)
         /// [HACK] Do not include the partial name of variables as they're typed
         destroyObjectInAVLTreeByValue(pVariableNamesTree, (LPARAM)identifyRawVariableName(pCodeObject->szText), NULL);

      // Populate results
      performOperationOnAVLTree(pVariableNamesTree, pOperationData);

      // Cleanup
      deleteAVLTree(pVariableNamesTree);
      deleteCodeObject(pCodeObject);
      utilDeleteString(szLineText);
      break;
   }

   /// Index and extract results tree
   performAVLTreeIndexing(pOutput = pOperationData->pOutputTree);

   // Cleanup and return tree
   deleteAVLTreeOperation(pOperationData);
   return pOutput;
}


/// Function name  : identifyCodeEditSuggestionTypeAtLocation
// Description     : Identifes the type of suggestions associated with the object at the character immediately preceeding the caret
// 
// CODE_EDIT_DATA*  pWindowData : [in] Window data
// 
// Return Value   : Types of suggestions to display (if any)
// 
SUGGESTION_TYPE  identifyCodeEditSuggestionTypeAtLocation(CODE_EDIT_DATA*  pWindowData, CONST UINT  iLine, CONST UINT  iCharIndex)
{
   SUGGESTION_TYPE       eOutput;          // Operation Result
   CODEOBJECT*           pCodeObject;      // Used for parsing the line containing the caret
   CODE_EDIT_LINE*       pLineData;        // Data for the line containing the caret
   TCHAR*                szLineText;       // Text of the line containing the caret

   // [CHECK] Ensure character index is valid
   ASSERT((INT)iCharIndex >= 0);

   // Prepare
   eOutput = CST_NONE;

   // Lookup data for the line containing the caret
   if (findCodeEditLineDataByIndex(pWindowData, iLine, pLineData))
   {
      // Generate a CodeObject from the line text
      szLineText  = generateCodeEditLineText(pLineData);
      pCodeObject = createCodeObject(szLineText);

      /// [EMPTY LINE] Suggest Commands
      if (szLineText[0] == NULL)
         eOutput = CST_COMMAND;

      /// [CHECK] Identify the CodeObject at the specified character
      else if (findCodeObjectByCharacterIndex(pCodeObject, (iCharIndex < (UINT)lstrlen(szLineText) ? iCharIndex : iCharIndex - 1)))
      {
         /// [FOUND] Determine suggestion type from CodeObject
         switch (pCodeObject->eClass)
         {
         // [ENUMERATION] Suggest GameObjects
         case CO_ENUMERATION:    eOutput = CST_GAME_OBJECT;     break;
         // [CONSTANT] Suggest ScriptObjects
         case CO_CONSTANT:       eOutput = CST_SCRIPT_OBJECT;   break;
         // [VARIABLE] Suggest Variables
         case CO_NULL:
         case CO_VARIABLE:       eOutput = CST_VARIABLE;        break;
         // [WORD/WHITESPACE] Suggest Commands
         case CO_WORD:
         case CO_KEYWORD:
         case CO_OPERATOR:
         case CO_WHITESPACE:     eOutput = CST_COMMAND;         break;
         }
      }
      
      // Cleanup
      deleteCodeObject(pCodeObject);
      utilDeleteString(szLineText);
   }

   // Return result
   return eOutput;
}


/// Function name  : insertCodeEditSuggestionAtCaret
// Description     : Inserts the currently selected suggestion into the CodeEdit
// 
// CODE_EDIT_DATA*  pWindowData   : [in] Window data
// 
// Return Value   : TRUE if successful, otherwise FALSE
// 
BOOL  insertCodeEditSuggestionAtCaret(CODE_EDIT_DATA*  pWindowData)
{
   CODE_EDIT_SUGGESTION *pSuggestion;      // Convenience pointer
   CODE_EDIT_LOCATION   *pCaretLocation;   // Convenience pointer
   CODE_EDIT_LINE       *pLineData;        // Data for the line containing the caret
   CODEOBJECT           *pCodeObject;      // Used for parsing the line containing the caret
   TCHAR                *szLineText,       // Text of the line containing the caret
                        *szOutput;         // Suggestion formatted for display
   CONST TCHAR          *szSuggestion;     // Suggestion text to insert
   UINT                  iRemovalLength;/*,
                         iRemovalPosition;*/

   // Prepare
   pSuggestion    = &pWindowData->oSuggestion;
   pCaretLocation = &pWindowData->oCaret.oLocation;
   szOutput       = utilCreateEmptyString(COMPONENT_LENGTH);

   // [CHECK] Lookup suggestion text
   if (getTreeNodeCount(pSuggestion->pResultsTree) AND findCodeEditSuggestionByIndex(pWindowData, ListView_GetNextItem(pSuggestion->hCtrl, -1, LVNI_SELECTED WITH LVNI_ALL), szSuggestion))
   {
      // Prepare
      findCodeEditLineDataByIndex(pWindowData, pCaretLocation->iLine, pLineData);
      szLineText     = generateCodeEditLineText(pLineData);
      iRemovalLength = 0;
      
      // Delete any selected text
      removeCodeEditSelectionText(pWindowData, TRUE);

      // [COMMAND] Recalculate suggestion origin, as user may have typed 
      if (pSuggestion->eType == CST_COMMAND)
         calculateCodeEditSuggestionOrigin(pWindowData, pWindowData->oCaret.oLocation.iLine, pWindowData->oCaret.oLocation.iIndex);

      // Move caret to the suggestion origin
      setCodeEditCaretLocation(pWindowData, &pSuggestion->ptOrigin);

      /// [COMMAND] Replace everything following origin
      if (pSuggestion->eType == CST_COMMAND)
      {
         /// Delete remaining line text
         while (removeCodeEditCharacterByIndex(pLineData, pSuggestion->ptOrigin.iIndex))
            {}

         // Insert verbatim
         StringCchPrintf(szOutput, COMPONENT_LENGTH, TEXT(" %s"), szSuggestion);
         insertCodeEditTextAtCaret(pWindowData, szOutput, TRUE);
      }
      /// [NON-COMMAND] Replace CodeObject only
      else
      {
         // Prepare
         pCodeObject = createCodeObject(szLineText);
         
         // Lookup CodeObject length
         if (findCodeObjectByCharacterIndex(pCodeObject, pSuggestion->ptOrigin.iIndex) AND isCodeObjectSuggestionType(pCodeObject, pSuggestion->eType))
            iRemovalLength = pCodeObject->iLength;

         /// Delete CodeObject only
         for (UINT iChar = 0; iChar < iRemovalLength; iChar++)
            removeCodeEditCharacterByIndex(pLineData, pSuggestion->ptOrigin.iIndex);

         // Determine how to display suggestion
         switch (pSuggestion->eType)
         {
         case CST_GAME_OBJECT:   StringCchPrintf(szOutput, COMPONENT_LENGTH, TEXT("{%s}"), szSuggestion);     break;
         case CST_SCRIPT_OBJECT: StringCchPrintf(szOutput, COMPONENT_LENGTH, TEXT("[%s]"), szSuggestion);     break;
         case CST_VARIABLE:      StringCchPrintf(szOutput, COMPONENT_LENGTH, TEXT("$%s"), szSuggestion);      break;
         }

         /// Insert suggestion within brackets
         insertCodeEditTextAtCaret(pWindowData, szOutput, TRUE);

         // Cleanup
         deleteCodeObject(pCodeObject);
      }

      // [TRACK]
      VERBOSE("Inserting %s suggestion '%s' at char %d line %d", szSuggestionTypes[pSuggestion->eType], szOutput, pSuggestion->ptOrigin.iIndex, pSuggestion->ptOrigin.iLine);
         
      // Cleanup
      utilDeleteString(szLineText);
   }

   // Cleanup and return TRUE
   utilDeleteString(szOutput);
   return TRUE;
}


/// Function name  : updateCodeEditSuggestionList
// Description     : Shows, Hides or Updates the SuggestionList to reflect the current caret position
// 
// CODE_EDIT_DATA*  pWindowData   : [in] Window data
// 
VOID  updateCodeEditSuggestionList(CODE_EDIT_DATA*  pWindowData, CONST UINT  iCharIndex, BOOL  bKeyPress)
{
   CODE_EDIT_SUGGESTION*  pSuggestion;      // Convenience pointer
   SUGGESTION_TYPE        eNewType;         // Type of suggestions appropriate to the new position of the caret
   UINT                   iSuggestionIndex; // Index of selected suggestion

   ASSERT((INT)iCharIndex >= 0);

   // Prepare
   pSuggestion = &pWindowData->oSuggestion;

   /// Identify the appropriate type of suggestions to display
   eNewType = identifyCodeEditSuggestionTypeAtLocation(pWindowData, pWindowData->oCaret.oLocation.iLine, iCharIndex);
 
   // Examine new type
   switch (eNewType)
   {
   /// [NONE] Destroy control
   case CST_NONE:
      if (pSuggestion->hCtrl)
         destroyCodeEditSuggestionList(pWindowData);
      break;

   /// [SUGGESTIONS] Display or update appropriately
   default:
      // [CHECK] Are we creating new suggestions or changing their type?
      if (!pSuggestion->hCtrl OR eNewType != pSuggestion->eType)
      {
         /// [CHANGE/CREATE] Destroy existing ListView, if present
         if (pSuggestion->hCtrl)
            destroyCodeEditSuggestionList(pWindowData);

         // [CHECK] Is user typing a disabled suggestion category?
         if (bKeyPress AND !getAppPreferences()->bSuggestions[eNewType])
         {
            /// [DISABLE] Display nothing
            pSuggestion->eType = CST_NONE;
            return;
         }

         /// Update type and calculate origin
         pSuggestion->eType = eNewType;
         calculateCodeEditSuggestionOrigin(pWindowData, pWindowData->oCaret.oLocation.iLine, iCharIndex);

         // Create results
         pSuggestion->pResultsTree = generateCodeEditSuggestionTree(pWindowData);
         
         /// [CHECK] Display ListView at the origin
         if (getTreeNodeCount(pSuggestion->pResultsTree))
            createCodeEditSuggestionList(pWindowData);
         else
         {
            /// [FAILED] Cleanup and display nothing
            deleteAVLTree(pSuggestion->pResultsTree);
            pSuggestion->eType = CST_NONE;
         }
      }
      else 
      {
         // Refresh results
         deleteAVLTree(pSuggestion->pResultsTree);
         pSuggestion->pResultsTree = generateCodeEditSuggestionTree(pWindowData);

         // [CHECK] Are there any results?
         if (getTreeNodeCount(pSuggestion->pResultsTree))
         {
            /// [SUCCESS] Update ListView count and size
            ListView_SetItemCount(pSuggestion->hCtrl, getTreeNodeCount(pSuggestion->pResultsTree));
            updateCodeEditSuggestionListSize(pWindowData);
         }
         else
            // [FAILED] Destroy ListView
            destroyCodeEditSuggestionList(pWindowData);
      }

      // [CHECK] Are we displaying results?
      if (pSuggestion->hCtrl)
      {
         /// Find index of best match
         iSuggestionIndex = findCodeEditSuggestionByText(pWindowData);
  
         // Select best match
         ListView_SetItemState(pSuggestion->hCtrl, iSuggestionIndex, LVIS_SELECTED WITH LVIS_FOCUSED, LVIS_SELECTED WITH LVIS_FOCUSED);
         ListView_EnsureVisible(pSuggestion->hCtrl, iSuggestionIndex, FALSE);
      }
      break;
   }
}


/// Function name  : updateCodeEditSuggestionListSize
// Description     : Shrinks ListView to fit visible items
// 
// CODE_EDIT_DATA*  pWindowData : [in] Window data
// 
VOID  updateCodeEditSuggestionListSize(CODE_EDIT_DATA*  pWindowData)
{
   CODE_EDIT_SUGGESTION*   pSuggestion;         // Convenience pointer
   UINT                    iVisibleItems,       // Number of items to display
                           iAvailableWidth;        // Column width, accounting for scroll-bar
   POINT                   ptOrigin;            // Suggestion origin co-ordinates
   RECT                    rcItem,              // Item rectangle
                           rcClient,            // Client rectangle
                           rcListView;          // ListView rectangle
   SIZE                    siItem,              // Size of Item rectangle
                           siListView;          // Size of ListView rectangle

   // Prepare
   pSuggestion = &pWindowData->oSuggestion;
   GetClientRect(pWindowData->hWnd, &rcClient);

   // [CHECK] Ensure ListView has items
   ASSERT(getTreeNodeCount(pSuggestion->pResultsTree));

   // Calculate item height
   ListView_GetItemRect(pSuggestion->hCtrl, 0, &rcItem, LVIR_BOUNDS);
   utilConvertRectangleToSize(&rcItem, &siItem);

   // Get default ListView size
   siListView    = siListViewSizes[pWindowData->oSuggestion.eType];

   // [CALC] Reduce ListView height if we're displaying very few items
   iVisibleItems = utilSmallerOrEqual((INT)getTreeNodeCount(pSuggestion->pResultsTree), siListView.cy / siItem.cy);
   siListView.cy = (iVisibleItems * siItem.cy);

   // Generate ListView rectangle
   calculateCodeEditPointFromLocation(pWindowData, &pWindowData->oSuggestion.ptOrigin, &ptOrigin);
   utilSetRectangle(&rcListView, ptOrigin.x, (ptOrigin.y + pWindowData->siCharacterSize.cy + 1), siListView.cx, siListView.cy);

   // [CHECK] Shift ListView left if clipped on the right
   if (rcListView.right > rcClient.right)
      OffsetRect(&rcListView, -(rcListView.right - rcClient.right + 10), 0);

   // [CHECK] Is ListView clipped at the bottom?
   if (rcListView.bottom > rcClient.bottom)
   {
      // Move ListView to above the line
      OffsetRect(&rcListView, 0, -(pWindowData->siCharacterSize.cy + 1 + siListView.cy));

      //// [CHECK] Is ListView clipped at the top?
      //if (rcListView.top < rcClient.top)
      //{
      //   rcListView.top = max(rcListView.top, rcClient.top);
      //}
   }

   /// [LISTVIEW] Resize and reposition ListView
   SetWindowPos(pSuggestion->hCtrl, NULL, rcListView.left, rcListView.top, siListView.cx, siListView.cy, SWP_NOZORDER);

   /// [COLUMN] Stretch column over entire width, excluding scroll bar.
   iAvailableWidth = (getTreeNodeCount(pSuggestion->pResultsTree) > iVisibleItems ? siListView.cx - GetSystemMetrics(SM_CXHSCROLL) : siListView.cx);

   if (pSuggestion->eType == CST_COMMAND)
      ListView_SetColumnWidth(pSuggestion->hCtrl, OBJECT_COLUMN_NAME, iAvailableWidth);
   else
   {
      ListView_SetColumnWidth(pSuggestion->hCtrl, OBJECT_COLUMN_NAME, iAvailableWidth - iTypeColumnWidths[pSuggestion->eType]);
      ListView_SetColumnWidth(pSuggestion->hCtrl, OBJECT_COLUMN_TYPE, iTypeColumnWidths[pSuggestion->eType]);
   }
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onCodeEditSuggestionItemRequest
// Description     : Provides the text for the Suggestion virtual ListView
// 
// CODE_EDIT_DATA*  pWindowData    : [in]     Window data
// CONST UINT       iLogicalIndex  : [in]     Logical item index of the string to retrieve
// LVITEM*          pItemData      : [in/out] Item data
// 
VOID   onCodeEditSuggestionItemRequest(CODE_EDIT_DATA*  pWindowData, CONST UINT  iLogicalIndex, LVITEM*  pItemData)
{
   CODE_EDIT_SUGGESTION*  pSuggestion;
   SUGGESTION_RESULT      xResult;
   CONST TCHAR*           szIcon;

   // Prepare
   pSuggestion = &pWindowData->oSuggestion;
   szIcon      = NULL;

   /// Lookup suggestion result
   if (findObjectInAVLTreeByIndex(pSuggestion->pResultsTree, iLogicalIndex, xResult.asPointer))
   {
      // [SUCCESS] Examine type
      switch (pSuggestion->eType)
      {
      /// [COMMAND] Return RichText and icon
      case CST_COMMAND:
         pItemData->lParam = (LPARAM)xResult.asCommandSyntax->pDisplaySyntax;
         pItemData->mask  |= LVIF_RICHTEXT;
         szIcon            = TEXT("FUNCTION_ICON");
         break;

      /// [GAME/SCRIPT OBJECT] Provide text and icon
      case CST_GAME_OBJECT:
      case CST_SCRIPT_OBJECT:
         if (pItemData->iSubItem == OBJECT_COLUMN_NAME)
         {
            if (pItemData->mask INCLUDES LVIF_TEXT)
               StringCchCopy(pItemData->pszText, pItemData->cchTextMax, xResult.asObjectName->szText);

            szIcon = (pSuggestion->eType == CST_GAME_OBJECT ? TEXT("GAME_OBJECT_ICON") : TEXT("SCRIPT_OBJECT_ICON"));
         }
         else
         {
            // Lookup group name
            if (pItemData->mask INCLUDES LVIF_TEXT)
               loadString(FIRST_OBJECTNAME_GROUP_NAME + xResult.asObjectName->eGroup, pItemData->pszText, pItemData->cchTextMax);
            
            // Right-aligned grey text
            pItemData->cColumns = DT_RIGHT;
            pItemData->mask    |= LVIF_COLOUR_TEXT;
            pItemData->lParam   = (LPARAM)GetSysColor(COLOR_GRAYTEXT);
         }
         break;

      /// [VARIABLE] Provide text and icon
      case CST_VARIABLE:
         if (pItemData->iSubItem == OBJECT_COLUMN_NAME)
         {
            // [TEXT] Extract sole variable name
            if (pItemData->mask INCLUDES LVIF_TEXT)
               StringCchCopy(pItemData->pszText, pItemData->cchTextMax, identifyRawVariableName(xResult.asVariableName->szName));

            szIcon = TEXT("VARIABLE_ICON");
         }
         else
         {
            // [TEXT] Supply 'Variable' or 'Argument'
            if (pItemData->mask INCLUDES LVIF_TEXT)
               StringCchCopy(pItemData->pszText, pItemData->cchTextMax, !isArgumentInScriptFile(pWindowData->pScriptFile, xResult.asVariableName->szName) ? TEXT("Variable") : TEXT("Argument"));

            // Right-aligned grey text
            pItemData->cColumns = DT_RIGHT;
            pItemData->mask    |= LVIF_COLOUR_TEXT;
            pItemData->lParam   = (LPARAM)GetSysColor(COLOR_GRAYTEXT);
         }
         break;
      }

      /// Lookup icon
      if (szIcon)
         pItemData->iImage = getAppImageTreeIconIndex(ITS_SMALL, szIcon);
   }
   else if (pItemData->mask INCLUDES LVIF_TEXT)
      // [ERROR] Identify missing item
      StringCchPrintf(pItemData->pszText, pItemData->cchTextMax, TEXT("ERROR: Unknown Item %u of %u"), iLogicalIndex, getTreeNodeCount(pSuggestion->pResultsTree));
   else
      // [ERROR] Provide fixed string
      pItemData->pszText = TEXT("ERROR: Unknown item");
}


/// Function name  : onCodeEditSuggestionKeyDown
// Description     : Manages the creation and destruction of the Suggestion ListView
// 
// CODE_EDIT_DATA*  pWindowData  : [in] Window data
// CONST UINT       iVirtualKey  : [in] Virtual Key Code of the key being pressed
// CONST BOOL       bKeyPress    : [in] TRUE indicates a key press, FALSE indicates a character message
// CONST BOOL       bProcessed   : [in] Whether the key has already been processed or not
// 
// Return Value: TRUE if the key press was handled successfully, FALSE if it should be passed to the base window
//
BOOL  onCodeEditSuggestionKeyDown(CODE_EDIT_DATA*  pWindowData, CONST UINT  iVirtualKey, CONST BOOL  bKeyPress, CONST BOOL  bProcessed)
{
   CODE_EDIT_SUGGESTION*   pSuggestion;         // Convenience pointer
   BOOL                    bControlPressed,     // Whether user is holding CTRL
                           bPassToBase;         // Whether the key press should be passed to the CodeEdit afterwards

   // Prepare
   pSuggestion     = &pWindowData->oSuggestion;
   bControlPressed = HIBYTE(GetKeyState(VK_CONTROL));
   bPassToBase     = TRUE;

   /// [CONTROL] Destroy List
   if (bControlPressed AND pSuggestion->hCtrl)
      destroyCodeEditSuggestionList(pWindowData);

   /// [KEY PRESS] Examine key-press
   else if (bKeyPress) switch (iVirtualKey)
   {
   /// [LEFT/RIGHT/HOME/END] Update List
   case VK_LEFT:
   case VK_RIGHT:
   case VK_HOME:
   case VK_END:
      // Update suggestions after caret has moved
      if (bProcessed AND pSuggestion->hCtrl)
         updateCodeEditSuggestionList(pWindowData, pWindowData->oCaret.oLocation.iIndex, TRUE);
      break;

   /// [BACKSPACE/DELETE] Update List
   case VK_BACK:
   case VK_DELETE:
      // Update suggestions after characters have been removed
      if (bProcessed AND pSuggestion->hCtrl)
         updateCodeEditSuggestionList(pWindowData, max(0, (INT)pWindowData->oCaret.oLocation.iIndex - 1), TRUE);
      break;

   /// [UP/DOWN,PAGE-UP/PAGE-DOWN] Pass to List
   case VK_UP:
   case VK_DOWN:
   case VK_PRIOR:
   case VK_NEXT:
      // Handled by SuggestionList
      if (!bProcessed AND pSuggestion->hCtrl)
      {
         SendMessage(pWindowData->oSuggestion.hCtrl, WM_KEYDOWN, iVirtualKey, NULL);
         bPassToBase = FALSE;
      }
      break;

   /// [ESCAPE] Destroy List.
   case VK_ESCAPE:
      // Destroy List
      if (!bProcessed AND pSuggestion->hCtrl)
      {
         destroyCodeEditSuggestionList(pWindowData);
         bPassToBase = FALSE;
      }
      break;
   }
   
   /// [CHARACTER MESSAGE] Examine character
   else switch (iVirtualKey)
   {
   /// [CHARACTER] Update List
   default:
      // Create/Update suggestions after characters have been added
      if (bProcessed)
         updateCodeEditSuggestionList(pWindowData, max(0, (INT)pWindowData->oCaret.oLocation.iIndex - 1), TRUE);
      break;

   /// [RETURN] Destroy list
   case VK_RETURN:
      // Destroy List
      if (bProcessed AND pSuggestion->hCtrl)
         destroyCodeEditSuggestionList(pWindowData);
      break;

   /// [TAB] Insert Suggestion. Destroy List.
   case VK_TAB:
      // [CHECK] Insert suggestion before key is processed
      if (!bProcessed AND pSuggestion->hCtrl)
      {
         // Insert suggestion and destroy control
         insertCodeEditSuggestionAtCaret(pWindowData);
         destroyCodeEditSuggestionList(pWindowData);
         bPassToBase = FALSE;
      }
      break;
   }
   

   /// Return TRUE to block key, FALSE to base to CodeEdit
   return !bPassToBase;
}


/// Function name  : onCodeEditSuggestionListKeyDown
// Description     : Converts key-presses in the ListView into simulated key-presses
// 
// CODE_EDIT_DATA*  pWindowData : [in] Window data
// CONST UINT       iVirtualKey : [in] Virtual Key Code of the key being pressed
// 
VOID   onCodeEditSuggestionListKeyDown(CODE_EDIT_DATA*  pWindowData, CONST UINT  iVirtualKey)
{
   // Examine key press
   switch (iVirtualKey)
   {
   /// [DELETE/ESCAPE/BACKSPACE] Simulate key-press
   case VK_BACK:
   case VK_DELETE:
   case VK_ESCAPE:
      if (!onCodeEditSuggestionKeyDown(pWindowData, iVirtualKey, TRUE, FALSE))
         onCodeEditSuggestionKeyDown(pWindowData, iVirtualKey, TRUE, TRUE);
      break;

   /// [TAB] Similate character message
   case VK_TAB:
      if (!onCodeEditSuggestionKeyDown(pWindowData, iVirtualKey, FALSE, FALSE))
         onCodeEditSuggestionKeyDown(pWindowData, iVirtualKey, FALSE, TRUE);
      break;

   /// [RETURN] Insert suggestion
   case VK_RETURN:
   case VK_LINE_FEED:
      // Insert suggestion and destroy control
      insertCodeEditSuggestionAtCaret(pWindowData);
      destroyCodeEditSuggestionList(pWindowData);
      break;
   }
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    WINDOW PROCEDURES
/// /////////////////////////////////////////////////////////////////////////////////////////

///// Function name  : wndprocCodeEditSuggestionList
//// Description     : Sub-classed SuggestionList window procedure
//// 
//// HWND  hCtrl   : [in] Suggestion ListView
//// 
//LRESULT  wndprocCodeEditSuggestionList(HWND  hCtrl, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
//{
//   NMITEMACTIVATE  oItemClick;
//   LVHITTESTINFO   oHitTest;
//   ERROR_STACK*    pException;
//   WNDCLASS        oBaseClass;
//   LRESULT         iResult;
//   BOOL            bPassToBase;
//
//   // [TRACK]
//   TRACK_FUNCTION();
//   bPassToBase = TRUE;
//   iResult     = 0;
//
//   /// [GUARD BLOCK]
//   __try
//   {
//      // [CHECK] Ensure window exists
//      ASSERT(IsWindow(hCtrl));
//
//      // Examine message
//      switch (iMessage)
//      {
//      /// [REQUEST KEYS] Ensure TAB and RETURN are passed to the CodeEdit
//      case WM_GETDLGCODE:
//         iResult = DLGC_WANTALLKEYS;
//         bPassToBase = FALSE;
//         break;
//
//      /// [CHARACTER] Copy this message to the CodeEdit
//      case WM_CHAR:
//         utilReflectMessage(hCtrl, iMessage, wParam, lParam);
//         break;
//
//      case WM_LBUTTONDBLCLK:
//         bPassToBase = FALSE;
//         break;
//
//      case WM_LBUTTONUP:
//         utilZeroObject(&oHitTest, LVHITTESTINFO);
//         utilZeroObject(&oItemClick, NMITEMACTIVATE);
//         
//         // Prepare
//         oHitTest.pt.x = LOWORD(lParam);
//         oHitTest.pt.y = HIWORD(lParam);
//
//         // [CHECK] Is cursor over an item?
//         if (ListView_HitTest(hCtrl, &oHitTest) != -1)
//         {
//            oItemClick.iItem = oHitTest.iItem;
//            oItemClick.hdr.hwndFrom = hCtrl;
//            oItemClick.hdr.code     = NM_DBLCLK;
//            oItemClick.hdr.idFrom   = GetDlgCtrlID(hCtrl);
//
//            SendMessage(GetParent(hCtrl), WM_NOTIFY, GetDlgCtrlID(hCtrl), (LPARAM)&oItemClick);
//         }
//         bPassToBase = FALSE;
//         break;
//      }
//
//      // [CHECK] Should we pass to ListView?
//      if (bPassToBase)
//      {
//         // [BASE] Call ListView window procedure
//         GetClassInfo(getAppInstance(), WC_LISTVIEW, &oBaseClass);
//         iResult = CallWindowProc(oBaseClass.lpfnWndProc, hCtrl, iMessage, wParam, lParam);
//      }
//   }
//   /// [EXCEPTION HANDLER]
//   __except (generateExceptionError(GetExceptionInformation(), pException))
//   {
//      // [ERROR] "An unidentified and unexpected critical error has occurred in the code window of script '%s'"
//      enhanceError(pException, ERROR_ID(IDS_EXCEPTION_CODE_EDIT), TEXT("Suggestion List"));
//      
//      /// Inform main window
//      SendMessage(getAppWindow(), UN_CODE_EDIT_EXCEPTION, (WPARAM)hCtrl, (LPARAM)pException);
//   }
//
//   // Cleanup and return result
//   END_TRACKING();
//   return iResult;
//}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    TREE  OPERATIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : treeprocBuildCodeEditSuggestionTree
// Description     : Builds the results tree for the current suggestion
// 
// AVL_TREE_NODE*       pCurrentNode   : [in] Current node
// AVL_TREE_OPERATION*  pOperationData : [in] Operation data
// 
///                         xFirstInput  : SUGGESTION_TYPE : [in]            Type of suggestion tree being built
///                         xSecondInput : SCRIPT_FILE*    : [in] [COMMANDS] ScriptFile 
// 
VOID  treeprocBuildCodeEditSuggestionTree(AVL_TREE_NODE*  pCurrentNode, AVL_TREE_OPERATION*  pOperationData)
{
   SUGGESTION_RESULT  xNode;               // Node contents
   SUGGESTION_TYPE    eSuggestionType;     // Type of the current node
   SCRIPT_FILE*       pScriptFile;

   // Extract parameters
   eSuggestionType = (SUGGESTION_TYPE)pOperationData->xFirstInput;
   xNode.asPointer = pCurrentNode->pData;
   
   // [CHECK] Exclude certain commands/objects
   switch (eSuggestionType)
   {
   /// [COMMAND] Exclude 'Hidden' commands
   case CST_COMMAND:
      pScriptFile = (SCRIPT_FILE*)pOperationData->xSecondInput;

      // Exclude hidden, ensure compatible.  (Allow hidden 'return' command)
      if ((xNode.asCommandSyntax->eGroup != CG_HIDDEN AND isCommandSyntaxCompatible(xNode.asCommandSyntax, pScriptFile->eGameVersion))
          OR xNode.asCommandSyntax->iID == CMD_RETURN)
         // [SUCCESS] Insert into the results tree
         insertObjectIntoAVLTree(pOperationData->pOutputTree, pCurrentNode->pData);
      break;

   /// [GAME OBJECT] Include all GameObjects
   case CST_GAME_OBJECT:
      // [SUCCESS] Insert into the results tree
      insertObjectIntoAVLTree(pOperationData->pOutputTree, pCurrentNode->pData);
      break;

   /// [SCRIPT OBJECT] Exclude 'Operators' from the results
   case CST_SCRIPT_OBJECT:
      switch (xNode.asObjectName->eGroup)
      {
      case ONG_PARAMETER_TYPE:
         switch (xNode.asObjectName->iID)
         {
         // [INTERNAL TYPES] Block
         case PS_COMMENT:
         case PS_CONDITION:      
         case PS_EXPRESSION:
         case PS_INTERRUPT_RETURN_OBJECT_IF:
         case PS_LABEL_NAME:
         case PS_LABEL_NUMBER:
         case PS_PARAMETER:
         case PS_REFERENCE_OBJECT:
         case PS_RETURN_OBJECT:
         case PS_RETURN_OBJECT_IF:
         case PS_RETURN_OBJECT_IF_START:
            break;

         // [REMINADER] Insert
         default:   
            insertObjectIntoAVLTree(pOperationData->pOutputTree, pCurrentNode->pData);
         }
         break;

      // [OPERATOR] Block all
      case ONG_OPERATOR:   
         break;

      // [REMINADER] Insert all
      default: 
         insertObjectIntoAVLTree(pOperationData->pOutputTree, pCurrentNode->pData);
      }
      break;

   /// [VARIABLE] Perform a deep copy since the source tree is not persistent
   case CST_VARIABLE:
      // [SUCCESS] COPY into the results tree
      insertObjectIntoAVLTree(pOperationData->pOutputTree, (LPARAM)duplicateVariableName(xNode.asVariableName));
      break;
   }
}


/// Function name  : treeprocFindCodeEditSuggestionResult
// Description     : Finds the most appropriate suggestion result
// 
// AVL_TREE_NODE*       pCurrentNode   : [in] Current node
// AVL_TREE_OPERATION*  pOperationData : [in] Operation data
// 
///                         xFirstInput  : SUGGESTION_TYPE : [in]               Type of suggestion tree being built
///                         xSecondInput : CODEOBJECT*     : [in] [OBJECT NAME] CodeObject being examined
///                         xThirdInput  : CONST TCHAR*    : [in] [COMMANDS]    Hash of the line being examined
///                         xOutput      : UINT            : [out]              Matching index
// 
VOID  treeprocFindCodeEditSuggestionResult(AVL_TREE_NODE*  pCurrentNode, AVL_TREE_OPERATION*  pOperationData)
{
   SUGGESTION_RESULT  xNode;               // Node contents
   SUGGESTION_TYPE    eSuggestionType;     // Type of the current node
   CONST TCHAR       *szPartialHash,
                     *szNodeText;          // Text of the node
   TCHAR             *szSearchTerm;        // Text to search for
   CODEOBJECT        *pCodeObject;         // CodeObject containing the search term for non-command suggestions
   
   // Extract parameters
   eSuggestionType = (SUGGESTION_TYPE)pOperationData->xFirstInput;
   pCodeObject     = (CODEOBJECT*)pOperationData->xSecondInput;
   szPartialHash   = (CONST TCHAR*)pOperationData->xThirdInput;
   xNode.asPointer = pCurrentNode->pData;
   
   // Determine search term
   switch (eSuggestionType)
   {
   /// [COMMAND] Extract the command hash generated from the current line text
   case CST_COMMAND:
      // Search using the partial hash
      szSearchTerm = utilDuplicateSimpleString(szPartialHash);
      
      // Compare against the command hash
      szNodeText = xNode.asCommandSyntax->szHash;
      break;

   /// [GAME/SCRIPT OBJECT] Exclude the bracket operators from the search term
   case CST_GAME_OBJECT:      
   case CST_SCRIPT_OBJECT:
      // [CHECK] Does CodeObject have a closing bracket?
      if (utilFindCharacterReverse(pCodeObject->szText, ']') OR utilFindCharacterReverse(pCodeObject->szText, '}'))
         // [OPENING and CLOSING] Strip opening and closing bracket
         szSearchTerm = utilDuplicateSubString(pCodeObject->szText, pCodeObject->iLength, 1, pCodeObject->iLength - 2);
      else
         // [OPENING] Strip opening bracket
         szSearchTerm = utilDuplicateSimpleString(&pCodeObject->szText[1]);

      // Compare against object text
      szNodeText = xNode.asObjectName->szText;
      break;

   /// [VARIABLE] Exclude the $ operator from the search term
   case CST_VARIABLE:
      // Strip $ operator
      szSearchTerm = utilDuplicateSimpleString(identifyRawVariableName(pCodeObject->szText));

      // Compare against variable name
      szNodeText = xNode.asVariableName->szName; 
      break;
   }

   UINT iSearchTermLength = lstrlen(szSearchTerm);
   UINT iMatchingChars = 0;

   /// [COMPARE] Determine number of matching characters
   for (UINT iIndex = 0; iIndex < iSearchTermLength; iIndex++)
      if (szNodeText[iIndex] AND (ChrCmpI(szSearchTerm[iIndex], szNodeText[iIndex]) == 0))
         iMatchingChars++;

   // [CHECK] Is this the best match found so far?
   if (iMatchingChars > (UINT)pOperationData->xInternalData1)
   {
      // [SUCCESS] Save this index
      pOperationData->xOutput = pOperationData->xInternalData2;
      pOperationData->bResult = TRUE;

      // Store match size
      pOperationData->xInternalData1 = iMatchingChars;
   }
   
   // Increment item index
   pOperationData->xInternalData2++;

   ///// [COMPARE] If there is a SearchTerm, check node text matches
   //if (StrCmpNI(szNodeText, szSearchTerm, lstrlen(szSearchTerm)) <= 0)
   //{
   //   // [FOUND] Set result and abort search
   //   pOperationData->bProcessing = FALSE;
   //   pOperationData->bResult     = TRUE;
   //}
   //else
   //   // [FAILED] Increment search index
   //   pOperationData->xOutput++;

   // Cleanup
   utilDeleteString(szSearchTerm);
}

