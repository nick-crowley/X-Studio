//
// CodeEdit Control (new).cpp : The 'high level' functionality of the CodeEdit, referred to as the 'CodeEdit' to separate the two clearly
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createCodeEditLabel
// Description     : Creates a CodeEditLabel
// 
// CONST TCHAR*  szName      : [in] Label name
// CONST UINT    iLineNumber : [in] Zero-based line number
// 
// Return Value   : New CodeEditLabel, you are responsible for destroying it
// 
CODE_EDIT_LABEL*  createCodeEditLabel(CONST TCHAR*  szName, CONST UINT  iLineNumber)
{
   CODE_EDIT_LABEL*  pNewLabel;

   // Create object
   pNewLabel = utilCreateEmptyObject(CODE_EDIT_LABEL);

   // Set properties
   StringCchCopy(pNewLabel->szName, 128, szName);
   pNewLabel->iLineNumber = iLineNumber;

   // Return new object
   return pNewLabel;
}


/// Function name  : deleteCodeEditLabel
// Description     : Destroys a CodeEditLabel
// 
// CODE_EDIT_LABEL*  &pCodeEditLabel : [in] Label to destroy
// 
ControlsAPI
VOID  deleteCodeEditLabel(CODE_EDIT_LABEL*  &pCodeEditLabel)
{
   // Delete calling object
   utilDeleteObject(pCodeEditLabel);
}


/// Function name  : destructorCodeEditLabel
// Description     : Destroys a CodeEditLabel within a List/Queue
// 
// LPARAM  pCodeEditLabel : [in] QueueItem containing a CodeEditLabel pointer
// 
ControlsAPI
VOID  destructorCodeEditLabel(LPARAM  pCodeEditLabel)
{
   // Convert input item and delete
   deleteCodeEditLabel((CODE_EDIT_LABEL*&)pCodeEditLabel);
}


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : getCodeEditCaretLineCommand
// Description     : Retrieves the COMMAND from the line containing the caret
// 
// CODE_EDIT_DATA*  pWindowData : [in] Window data
// 
// Return Value   : COMMAND associated with the line containing the caret
// 
COMMAND*  getCodeEditCaretLineCommand(CODE_EDIT_DATA*  pWindowData)
{
   CODE_EDIT_LINE*   pLineData;

   // [TRACK]
   TRACK_FUNCTION();

   // Lookup line data
   findCodeEditLineDataByIndex(pWindowData, pWindowData->oCaret.oLocation.iLine, pLineData);

   // [CHECK] Ensure COMMAND exists
   ASSERT(pLineData AND pLineData->pCommand);
   
   // Return COMMAND
   END_TRACKING();
   return pLineData->pCommand;
}


/// Function name  : getCodeEditCaretLineError
// Description     : Retrieve the ErrorQueue associated with the line containing the caret
// 
// CODE_EDIT_DATA*  pWindowData : [in] Window data
// 
// Return Value   : ErrorQueue associated with the line containing the caret, or NULL if line contains no errors
// 
ERROR_QUEUE*  getCodeEditCaretLineError(CODE_EDIT_DATA*  pWindowData)
{
   CODE_EDIT_LINE*    pLineData;

   // [TRACK]
   TRACK_FUNCTION();

   // Lookup line data
   findCodeEditLineDataByIndex(pWindowData, pWindowData->oCaret.oLocation.iLine, pLineData);
   ASSERT(pLineData);

   // Return ErrorQueue if any, otherwise NULL
   END_TRACKING();
   return pLineData->pErrorQueue;
}


/// Function name  : getCodeEditLineIndentation
// Description     : Retrieves the indentation for the specified line
// 
// CODE_EDIT_DATA*   pWindowData : [in] Window data
// CONST UINT        iLine       : [in] Zero-based line number
// 
// Return Value   : Line indentation if found, otherwise NULL
// 
UINT  getCodeEditLineIndentation(CODE_EDIT_DATA*   pWindowData, CONST UINT  iLine)
{
   CODE_EDIT_LINE*  pLineData;
   UINT             iOutput;

   // [TRACK]
   TRACK_FUNCTION();

   // Prepare
   iOutput = NULL;

   /// Lookup line data
   if (findCodeEditLineDataByIndex(pWindowData, iLine, pLineData))
      // [FOUND] Return indentation
      iOutput = pLineData->iIndent;

   // Return indentation
   END_TRACKING();
   return iOutput;
}


/// Function name  : isCodeEditLineWhitespace
// Description     : Checks whether a line in a CodeEdit is whitespace/empty
// 
// CONST CODE_EDIT_LINE*  pLineData : [in] Line to test
// 
// Return Value   : TRUE if line is whitespace or empty, otherwise FALSE
// 
BOOL  isCodeEditLineWhitespace(CONST CODE_EDIT_LINE*  pLineData)
{
   TCHAR*  szLineText;
   BOOL    bResult;

   // [CHECK] Is string whitespace?
   bResult = isStringWhitespace(szLineText = generateCodeEditLineText(pLineData));

   // Cleanup and return result
   utilDeleteString(szLineText);
   return bResult;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : calculateCodeEditCommentQuality
// Description     : Determines the quality of commenting in the CodeEdit by comparing the number of
//                      -> commands and comments.  A quality of 100% represents 40% of lines or more 
//                      -> contain comments.
// 
// CODE_EDIT_DATA*  pWindowData : [in] Window data
// 
// Return Value   : Number between 0 and 100
// 
UINT    calculateCodeEditCommentQuality(CODE_EDIT_DATA*  pWindowData)
{
   CODE_EDIT_LINE*  pLineData;         // LineData for the line currently being processed
   LIST_ITEM*       pLineIterator;     // LineData iterator
   FLOAT            fOutput;           // Precisely calculated output
   UINT             iCommentCount,     // Number of comments in the CodeEdit
                    iIdealMaximum;     // Number of comments required to quality for 100% quality

   // [TRACK]
   TRACK_FUNCTION();

   // [CHECK] CodeEdit must never have zero lines
   ASSERT(getCodeEditLineCount(pWindowData) > 0);

   // Prepare
   iCommentCount = 0;

   /// Iterate through all lines in the control
   for (findCodeEditLineListItemByIndex(pWindowData, 0, pLineIterator); pLineIterator; pLineIterator = pLineIterator->pNext)
   {
      // Prepare
      pLineData = extractListItemPointer(pLineIterator, CODE_EDIT_LINE);

      // Examine COMMAND ID
      switch (pLineData->iCommandID)
      {
      /// [COMMENT] Increment comment count
      case CMD_COMMENT:
      case CMD_COMMAND_COMMENT:
         iCommentCount++;
         break;
      }
   }

   // Calculate ideal maximum as 30% (Ensure it's never zero)
   iIdealMaximum = max(utilCalculatePercentage(getCodeEditLineCount(pWindowData), 30), 1);

   // Calculate quality
   fOutput = (100.0f * (FLOAT)iCommentCount / (FLOAT)iIdealMaximum);

   // Return quality score. Limit to 100%
   END_TRACKING();
   return (INT)min(fOutput, 100.0f);
}



/// Function name  : displayCodeEditTooltip
// Description     : Displays the tooltip relevant to a CodeObject, if any, or hides the existing Tooltip
// 
// CODE_EDIT_DATA*      pWindowData : [in]        Window data
// CONST BOOL           bShow       : [in]        Whether to show or hide
// CONST UINT           iLine       : [in] [SHOW] Line number
// CONST CODEOBJECT*    pCodeObject : [in] [SHOW] CodeObject to display a tooltip for
// 
VOID  displayCodeEditTooltip(CODE_EDIT_DATA*  pWindowData, CONST BOOL  bShow, CONST UINT  iLine, CONST CODEOBJECT*  pCodeObject)
{
   CODE_EDIT_LOCATION  oLocation;
   TOOLINFO            oTooltipData;
   POINT               ptCodeObject[2];

   // [TRACK]
   TRACK_FUNCTION();

   // [CHECK] Do nothing if state is correct.  Don't display tooltips and suggestions together
   if (bShow != pWindowData->bTooltipVisible AND !pWindowData->oSuggestion.hCtrl)
   {
      // Prepare
      utilZeroObject(&oTooltipData, TOOLINFO);

      // Setup request
      oTooltipData.cbSize   = sizeof(TOOLINFO);
      oTooltipData.hwnd     = pWindowData->hWnd;
      oTooltipData.uFlags   = TTF_TRACK;
      oTooltipData.uId      = IDC_CODE_EDIT_TOOLTIP;
      
      // [SHOW] Position tooltip
      if (bShow)
      {
         /// Create tooltip data from CodeObject
         pWindowData->pTooltipData = generateCodeEditTooltipData(pWindowData, iLine, pCodeObject);

         // [CHECK] Does this CodeObject have a tooltip?
         if (pWindowData->pTooltipData)
         {
            // Calculate position of first CodeObject char
            oLocation.iLine = iLine;
            oLocation.iIndex = pCodeObject->iStartCharIndex;
            calculateCodeEditPointFromLocation(pWindowData, &oLocation, &ptCodeObject[0]);

            // Calculate position of last CodeObject char
            oLocation.iIndex = pCodeObject->iEndCharIndex;
            calculateCodeEditPointFromLocation(pWindowData, &oLocation, &ptCodeObject[1]);

            // Define tool around CodeEdit rectangle
            oTooltipData.lpszText = LPSTR_TEXTCALLBACK;
            SetRect(&oTooltipData.rect, ptCodeObject[0].x, ptCodeObject[0].y, ptCodeObject[1].x, ptCodeObject[1].y += pWindowData->siCharacterSize.cy);
            
            /// Update tooltip state
            pWindowData->bTooltipVisible = TRUE;

            // Add new tool
            SendMessage(pWindowData->hTooltip, TTM_ADDTOOL, NULL, (LPARAM)&oTooltipData);
            
            /// Position tooltip
            ClientToScreen(pWindowData->hWnd, &ptCodeObject[1]);
            SendMessage(pWindowData->hTooltip, TTM_TRACKPOSITION, NULL, MAKE_LONG(ptCodeObject[1].x, ptCodeObject[1].y));

            /// Show/Hide tooltip
            SendMessage(pWindowData->hTooltip, TTM_TRACKACTIVATE, pWindowData->bTooltipVisible, (LPARAM)&oTooltipData);

            //VERBOSE("CODE-EDIT: Showing Tooltip!  %u", GetTickCount());
         }
      }
      // [HIDE]
      else 
      {
         //VERBOSE("CODE-EDIT: Hiding Tooltip!  %u", GetTickCount());

         /// Update tooltip state
         pWindowData->bTooltipVisible = FALSE;

         /// Hide tooltip
         SendMessage(pWindowData->hTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM)&oTooltipData);

         /// Delete tool and data
         SendMessage(pWindowData->hTooltip, TTM_DELTOOL, NULL, (LPARAM)&oTooltipData);
         deleteCustomTooltipData(pWindowData->pTooltipData);
      }
   }

   // [TRACK]
   END_TRACKING();
}


/// Function name  : generateCodeEditLabelQueue
// Description     : Generates a Queue containing the names and locations of all the 'define label' commands in the CodeEdit
// 
// CODE_EDIT_DATA*  pWindowData : [in] Window data
// 
// Return Value   : New Queue containing CodeEditLabels. You are responsible for destroying it
// 
QUEUE*  generateCodeEditLabelQueue(CODE_EDIT_DATA*  pWindowData)
{
   CODE_EDIT_LINE*     pLineData;         // LineData for the line currently being processed
   LIST_ITEM*          pLineIterator;     // LineData iterator
   CONST TCHAR*        szLabelName;       // Name of current label
   QUEUE*              pLabelQueue;       // Operation result
   UINT                iLineNumber;       // Line number of the line currently being processed

   // [TRACK]
   TRACK_FUNCTION();

   // Prepare
   pLabelQueue = createQueue(destructorCodeEditLabel);
   iLineNumber = 0;

   /// Iterate through all lines in the control
   for (findCodeEditLineListItemByIndex(pWindowData, 0, pLineIterator); pLineData = extractListItemPointer(pLineIterator, CODE_EDIT_LINE); pLineIterator = pLineIterator->pNext)
   {
      // [CHECK] Ensure COMMAND exists
      ASSERT(pLineData->pCommand);

      /// [DEFINE LABEL] Attempt to extract label name
      if (pLineData->iCommandID == CMD_DEFINE_LABEL AND findStringParameterInCommandByIndex(pLineData->pCommand, 0, szLabelName))
         /// [SUCCESS] Generate new CodeEditLabel
         pushLastQueueObject(pLabelQueue, (LPARAM)createCodeEditLabel(szLabelName, iLineNumber));
      
      // Advance line number iterator
      iLineNumber++;
   }

   // Return new LabelQueue
   END_TRACKING();
   return pLabelQueue;
}


/// Function name  : generateCodeEditScriptDependencyTree
// Description     : Creates a ScriptDependencies tree from the current commands
// 
// CODE_EDIT_DATA*  pWindowData   : [in] Window data
// 
// Return Value   : New ScriptDependencies tree, you are responsible for destroying it
// 
AVL_TREE*  generateCodeEditScriptDependencyTree(CODE_EDIT_DATA*  pWindowData)
{
   CODE_EDIT_LINE*     pLineData;        // LineData for the line currently being processed
   LIST_ITEM*          pLineIterator;    // LineData iterator
   CONST TCHAR*        szScriptName;     // Name of the current script call
   AVL_TREE*           pOutputTree;      // Tree containing one ScriptDependencies for each unique external script
   
   // [TRACK]
   TRACK_FUNCTION();

   // Prepare
   pOutputTree = createScriptDependencyTreeByText();

   /// Iterate through all lines in the control
   for (findCodeEditLineListItemByIndex(pWindowData, 0, pLineIterator); pLineData = extractListItemPointer(pLineIterator, CODE_EDIT_LINE); pLineIterator = pLineIterator->pNext)
   {
      // [CHECK] Ensure command exists
      ASSERT(pLineData->pCommand);

      /// Lookup ScriptName
      if (findScriptCallTargetInCommand(pLineData->pCommand, szScriptName))
         // Insert into ScriptDependencies tree
         insertScriptDependencyIntoAVLTree(pOutputTree, szScriptName);   
   }

   // Return indexed ScriptDependencies Tree
   performAVLTreeIndexing(pOutputTree);
   END_TRACKING();
   return pOutputTree;
}



/// Function name  : generateCodeEditStringDependencyTree
// Description     : Creates a StringDependencies tree from the current commands
// 
// CODE_EDIT_DATA*      pWindowData   : [in] Window data
// CONST PROJECT_FILE*  pProjectFile  : [in] Project file
// 
// Return Value   : New StringDependencies tree, you are responsible for destroying it
// 
AVL_TREE*  generateCodeEditStringDependencyTree(CODE_EDIT_DATA*  pWindowData, CONST PROJECT_FILE*  pProjectFile)
{
   GAME_STRING_REFERENCE *pReference;         // Current GameString reference
   CODE_EDIT_LINE        *pLineData;          // LineData for the current line
   LIST_ITEM             *pIterator;          // Line iterator
   AVL_TREE              *pOutputTree;        // Output tree of ScriptDependencies
   LIST                  *pReferenceList;     // List of GameString references
   UINT                   iStringID,          // First/only String ID of current reference
                          iLastStringID,      // [ARRAY] Last string ID of current only reference
                          iPageID;            // PageID of current reference

   // Prepare
   TRACK_FUNCTION();
   pOutputTree = createStringDependencyTreeByPageID();

   /// Iterate through all lines in the control
   for (findCodeEditLineListItemByIndex(pWindowData, 0, pIterator); pLineData = extractListItemPointer(pIterator, CODE_EDIT_LINE); pIterator = pIterator->pNext)
   {
      // [CHECK] Ensure command exists
      ASSERT(pLineData->pCommand);

      // Lookup the GameString references
      if (pReferenceList = generateCommandGameStringReferences(pLineData->pCommand))
      {
         // Iterate through the references
         for (UINT  iIndex = 0; findListObjectByIndex(pReferenceList, iIndex, (LPARAM&)pReference); iIndex++)
         {
            // [CHECK] Resolve page ID and string ID
            if (!resolveCodeEditIntegerParameter(pWindowData, pIterator, pReference->iPageParameterIndex, pProjectFile, iPageID) OR
                !resolveCodeEditIntegerParameter(pWindowData, pIterator, pReference->iStringParameterIndex, pProjectFile, iStringID))
                continue;
            
            /// [STATIC REFERENCES] Generate single dependency
            if (pLineData->iCommandID != CMD_READ_TEXT_ARRAY)
               insertStringDependencyIntoAVLTree(pOutputTree, iPageID, iStringID);

            /// [REFERENCE ARRAY] Generate a dependency for each string in array
            else
            {
               // Prepare
               findListObjectByIndex(pReferenceList, 1, (LPARAM&)pReference);

               // [CHECK] Lookup ID of last string ID
               if (resolveCodeEditIntegerParameter(pWindowData, pIterator, pReference->iStringParameterIndex, pProjectFile, iLastStringID))
               {
                  // Generate new dependency for each string ID
                  for (UINT  iID = iStringID; iID <= iLastStringID; iID++)
                     insertStringDependencyIntoAVLTree(pOutputTree, iPageID, iID);
               }

               break;
            }
         }

         // Cleanup
         deleteList(pReferenceList);
      }
   }

   // Return indexed StringDependencies Tree
   performAVLTreeIndexing(pOutputTree);
   END_TRACKING();
   return pOutputTree;
}



/// Function name  : generateCodeEditTooltipData
// Description     : Generates CustomTooltip data for the specified CodeObject
// 
// CODE_EDIT_DATA*    pWindowData : [in] Window data
// CONST UINT         iLine       : [in] Line nunmber
// CONST CODEOBJECT*  pCodeObject : [in] CodeObject to generate data for
// 
// Return Value   : Returns CustomTooltip data if CodeObject is a command, GameObject or ScriptObject - otherwise it returns NULL
// 
CUSTOM_TOOLTIP*  generateCodeEditTooltipData(CODE_EDIT_DATA*  pWindowData, CONST UINT  iLine, CONST CODEOBJECT*  pCodeObject)
{
   SUGGESTION_RESULT  xObject;
   CODE_EDIT_LINE*    pLineData;
   RESULT_TYPE        eOutputType;
   TCHAR*             szObjectName;

   // [TRACK]
   TRACK_FUNCTION();

   // Prepare
   eOutputType = RT_NONE;
   
   // Examine CodeObject
   switch (pCodeObject->eClass)
   {
   default:
      // Lookup line data
      findCodeEditLineDataByIndex(pWindowData, iLine, pLineData);

      // [CHECK] Ensure COMMAND exists
      ASSERT(pLineData AND pLineData->pCommand);

      // [CHECK] Exclude hidden and un-identified commands
      if (!isCommandID(pLineData->pCommand, CMD_NONE) AND pLineData->pCommand->pSyntax->eGroup != CG_HIDDEN)
      {
         xObject.asCommandSyntax = pLineData->pCommand->pSyntax;
         eOutputType = RT_COMMANDS;
      }
      break;

   case CO_ENUMERATION: 
      // Extract text
      szObjectName = utilDuplicateSubString(pCodeObject->szText, pCodeObject->iLength, 1, pCodeObject->iLength - 2);

      // [CHECK] Lookup GameObject
      if (findGameObjectByText(szObjectName, xObject.asObjectName))
         eOutputType = RT_GAME_OBJECTS;

      // Cleanup
      utilDeleteString(szObjectName);
      break;

   case CO_CONSTANT: 
      // Extract text
      szObjectName = utilDuplicateSubString(pCodeObject->szText, pCodeObject->iLength, 1, pCodeObject->iLength - 2);

      // [CHECK] Lookup GameObject
      if (findScriptObjectByText(szObjectName, xObject.asObjectName))
         eOutputType = RT_SCRIPT_OBJECTS;

      // Cleanup
      utilDeleteString(szObjectName);
      break;
   }

   /// Create data or return NULL
   END_TRACKING();
   return (eOutputType == RT_NONE ? NULL : createCustomTooltipData(eOutputType, xObject));
}



/// Function name  : generateCodeEditVariableDependencyTree
// Description     : Creates a VariableDependencies tree from the current commands
// 
// CODE_EDIT_DATA*  pWindowData : [in] Window data
// 
// Return Value   : New ScriptDependencies tree, you are responsible for destroying it
// 
AVL_TREE*  generateCodeEditVariableDependencyTree(CODE_EDIT_DATA*  pWindowData)
{
   CODE_EDIT_LINE*     pLineData;               // LineData for the line currently being processed
   LIST_ITEM*          pLineIterator;           // LineData iterator
   CONST TCHAR*        szVariableName;          // Variable name
   AVL_TREE*           pOutputTree;             // Tree containing one VariableDependency for each unique local/global variable
   UINT                iVariableNameIndex;      // Index of the PARAMETER in the current COMMAND that contains the variable name
   VARIABLE_TYPE       eVariableType;           // Whether variable is local or global
   VARIABLE_USAGE      eVariableUsage;          // Whether variable is read or write

   // [TRACK]
   TRACK_FUNCTION();

   // Prepare
   pOutputTree = createVariableDependencyTreeByText();

   /// Iterate through all lines in the control
   for (findCodeEditLineListItemByIndex(pWindowData, 0, pLineIterator); pLineData = extractListItemPointer(pLineIterator, CODE_EDIT_LINE); pLineIterator = pLineIterator->pNext)
   {
      // [CHECK] Ensure command exists
      ASSERT(pLineData->pCommand);

      /// Determine parameter index containing Variable name
      switch (pLineData->iCommandID)
      {
      // [DEFAULT] Ignore command
      default:
         continue;

      // [PARAMETER ZERO]
      case CMD_GET_GLOBAL_VARIABLE:         // "$1 get global variable: name=$0"
      case CMD_SET_GLOBAL_VARIABLE:         // "set global variable: name=$0 value=$1"
         iVariableNameIndex = 0;
         break;

      // [PARAMETER ONE]
      case CMD_GET_LOCAL_VARIABLE:          // "$2 $0 get local variable: name=$1"
      case CMD_SET_LOCAL_VARIABLE:          // "$0 set local variable: name=$1 value=$2"
      case CMD_GET_GLOBAL_VARIABLE_KEYS:    // "$0 get all global variable keys, starting with=$1"
      case CMD_GET_GLOBAL_VARIABLE_REG_EXP: // "$0 get global variables: regular expression=$1"
         iVariableNameIndex = 1;
         break;

      // [PARAMETER TWO]
      case CMD_GET_LOCAL_VARIABLE_KEYS:     // "$1 $0 get all local variable keys, starting with=$2"
      case CMD_GET_LOCAL_VARIABLE_REG_EXP:  // "$1 $0 get local variables: regular expression=$2"
         iVariableNameIndex = 2;
         break;
      }

      /// [USAGE] Determine whether variable is Local or Global
      switch (pLineData->iCommandID)
      {
      case CMD_GET_LOCAL_VARIABLE:  case CMD_SET_LOCAL_VARIABLE:   eVariableType = VT_LOCAL;      break;
      default:                                                     eVariableType = VT_GLOBAL;     break;
      }

      /// [TYPE] Determine whether variable is Read or Write
      switch (pLineData->iCommandID)
      {
      case CMD_SET_LOCAL_VARIABLE:          
      case CMD_SET_GLOBAL_VARIABLE:   eVariableUsage = VU_SET;    break;
      default:                        eVariableUsage = VU_GET;    break;
      }

      /// Lookup VariableName PARAMETER  (Also ensure it's specified as a string, not a variable)
      if (findStringParameterInCommandByIndex(pLineData->pCommand, iVariableNameIndex, szVariableName))
          // Insert into VariableDependencies tree
          insertVariableDependencyIntoAVLTree(pOutputTree, szVariableName, eVariableType, eVariableUsage);      
   }

   // Return indexed VariableDependencies Tree
   performAVLTreeIndexing(pOutputTree);
   END_TRACKING();
   return pOutputTree;
}


/// Function name  : generateCodeEditVariableNamesTree
// Description     : Creates an AVLTree of VariableNames sorted alphabetically, and calculates how many times each occurs
// 
// CODE_EDIT_DATA*  pWindowData : [in] Window data
// 
// Return Value   : New AVLTree, you are responsible for destroying it
// 
AVL_TREE*  generateCodeEditVariableNamesTree(CODE_EDIT_DATA*  pWindowData)
{
   CODE_EDIT_LINE*  pLineData;         // LineData for the line currently being examined
   LIST_ITEM*       pLineIterator;     // LineData iterator
   PARAMETER*       pParameter;        // PARAMETER within the COMMAND currently being examined
   AVL_TREE*        pOutputTree;       // Operation result
   UINT             iLineNumber;       // Line number of the line currently being examined

   // [TRACK]
   TRACK_FUNCTION();

   // Prepare
   pOutputTree = createVariableNameTreeByText();
   iLineNumber = 0;

   /// Iterate through all lines in the control
   for (findCodeEditLineListItemByIndex(pWindowData, 0, pLineIterator); pLineData = extractListItemPointer(pLineIterator, CODE_EDIT_LINE); pLineIterator = pLineIterator->pNext)
   {
      // [CHECK] Ensure COMMAND exists
      ASSERT(pLineData->pCommand);

      /// Iterate through all PARAMETERS in the current COMMAND
      for (UINT  iParameterIndex = 0; findParameterInCommandByIndex(pLineData->pCommand, PT_DEFAULT, iParameterIndex, pParameter); iParameterIndex++)
      {
         // [CHECK] Is this a variable or ReturnObject?
         if (lstrlen(pParameter->szValue) AND (pParameter->eType == DT_VARIABLE OR isReturnObject(pParameter->eSyntax)))
            /// [VARIABLE/RETURN-OBJECT] Add to output tree using an incremental ID
            insertVariableNameIntoAVLTree(pOutputTree, pParameter->szValue, getTreeNodeCount(pOutputTree));
      }

      // Advance line number iterator
      iLineNumber++;
   }

   /// Ensure new tree is indexed before returning it
   performAVLTreeIndexing(pOutputTree);
   END_TRACKING();
   return pOutputTree;
}


/// Function name  : isCodeEditSelectionCommented
// Description     : Returns TRUE if the first line in the selection is commented, otherwise FALSE
// 
// CODE_EDIT_DATA*  pWindowData : [in] Window data
// 
// Return Value   : TRUE if first line is commented, FALSE if not or there is no selection
// 
BOOL  isCodeEditSelectionCommented(CODE_EDIT_DATA*  pWindowData)
{
   CODE_EDIT_CHARACTER*  pCharacter;      // Character being processed
   CODE_EDIT_LINE*       pLineData;       // Data of first line in selection
   BOOL                  bComment;        // Whether first line is commented or not
   UINT                  iFirstLine;      // Number of first line in selection

   // Prepare
   bComment = FALSE;

   // [CHECK] Ensure selection exists
   if (hasCodeEditSelection(pWindowData))
   {
      // Prepare
      iFirstLine = min(pWindowData->oSelection.oOrigin.iLine, pWindowData->oCaret.oLocation.iLine);

      /// Lookup first line in the selection
      if (findCodeEditLineDataByIndex(pWindowData, iFirstLine, pLineData))
      {
         /// Search through characters until definitive answer is found
         for (UINT  iChar = 0; pLineData AND !bComment AND findCodeEditCharacterDataByIndex(pLineData, iChar, pCharacter); iChar++)
         {
            // Examine character
            switch (pCharacter->chCharacter)
            {
            // [COMMENT] 
            case '*':  
               bComment = TRUE;
               break;

            // [NOT COMMENT]
            default: 
               pLineData = NULL;  
               break;

            // [WHITESPACE] Ignore
            case ' ':
               continue;
            }
         }
      }
   }

   // Return result
   return bComment;
}


/// Function name  : performCodeEditSelectionCommenting
// Description     : Comments or uncomments the selected lines
// 
// CODE_EDIT_DATA*  pWindowData : [in] Window data
// CONST BOOL       bComment    : [in] TRUE to comment, FALSE to uncomment
// 
VOID   performCodeEditSelectionCommenting(CODE_EDIT_DATA*  pWindowData, CONST BOOL  bComment)
{
   CODE_EDIT_CHARACTER* pFirstCharacter;   // First character on the line being processed
   CODE_EDIT_LOCATION   oCaretLocation;    // Updated caret location
   CODE_EDIT_LINE*      pLineData;         // Data for line being processed
   LIST_ITEM*           pIterator;         // Line data iterator
   UINT                 iCurrentLine,      // Number of line being processed
                        iLastLine;         // Line number of last line in the selection

   // [VERBOSE]
   TRACK_FUNCTION();
   VERBOSE_LIB_COMMAND();

   // Prepare
   oCaretLocation = pWindowData->oCaret.oLocation;

   // [CHECK] Ensure selection exists
   if (hasCodeEditSelection(pWindowData))
   {
      // Determine start/end lines
      iCurrentLine = min(pWindowData->oSelection.oOrigin.iLine, oCaretLocation.iLine);
      iLastLine    = max(pWindowData->oSelection.oOrigin.iLine, oCaretLocation.iLine);
   
      /// Iterate through all lines in the selection
      for (findCodeEditLineListItemByIndex(pWindowData, iCurrentLine, pIterator); (iCurrentLine <= iLastLine) AND (pLineData = extractListItemPointer(pIterator, CODE_EDIT_LINE)); pIterator = pIterator->pNext)
      {
         // [CHECK] Lookup first character
         if (findCodeEditCharacterDataByIndex(pLineData, 0, pFirstCharacter) AND !isCodeEditLineWhitespace(pLineData))
         {
            /// [CHECK] Is this an uncommented line that needs commenting?
            if (pFirstCharacter->chCharacter != '*' AND bComment)
            {
               // [SUCCESS] Insert '* ' at the start of the line
               insertCodeEditCharacterByIndex(pLineData, 0, '*');
               insertCodeEditCharacterByIndex(pLineData, 1, ' ');

               // [CARET] Adjust caret if appropriate
               if (iCurrentLine == oCaretLocation.iLine AND oCaretLocation.iIndex > 0)
                  oCaretLocation.iIndex += 2;
            }
            /// [CHECK] Is this a commented line that needs uncommenting?
            else if (pFirstCharacter->chCharacter == '*' AND !bComment)
            {
               // [SUCCESS] Remove all comment operators and whitespace from the start of the line
               while (findCodeEditCharacterDataByIndex(pLineData, 0, pFirstCharacter) AND (pFirstCharacter->chCharacter == '*' OR pFirstCharacter->chCharacter == ' '))
               {
                  // [WHITESPACE] Remove any whitespace
                  removeCodeEditCharacterByIndex(pLineData, 0);

                  // [CARET] Adjust caret if appropriate
                  if (iCurrentLine == oCaretLocation.iLine AND oCaretLocation.iIndex > 0)
                     oCaretLocation.iIndex--;
               }
            }

            // [CHECK] Have we moved the caret?
            if (iCurrentLine == oCaretLocation.iLine AND pWindowData->oCaret.oLocation.iIndex != oCaretLocation.iIndex)
               /// [CARET] Store new caret position
               setCodeEditCaretLocation(pWindowData, &oCaretLocation);

            // [COMMENT] Add selection state to the characters we've just added
            if (bComment)
               setCodeEditSelection(pWindowData, pWindowData->oCaret.oLocation, pWindowData->oSelection.oOrigin, TRUE);

            /// [EVENT] Text has changed
            updateCodeEditLine(pWindowData, iCurrentLine, CCF_TEXT_CHANGED);
         }

         // Move to the next line
         iCurrentLine++;
      }
   }

   // Cleanup
   END_TRACKING();
}


/// Function name  : resolveCodeEditIntegerParameter
// Description     : Resolves the integer value of a parameter by searching for the last assignment, if necessary, and resolving the value of a local/global variable, if necessary.
// 
// CODE_EDIT_DATA*      pWindowData     : [in]           Window data
// LIST_ITEM*           pTargetLine     : [in]           Item containing line data for the target line
// CONST UINT           iParameterIndex : [in]           Zero-based index of the parameter to resolve
// CONST PROJECT_FILE*  pProjectFile    : [in][optional] ProjectFile containing local/global variables
// UINT&                iOutput         : [in/out]       Parameter value
// 
// Return Value   : TRUE if resolved otherwise FALSE
// 
BOOL  resolveCodeEditIntegerParameter(CODE_EDIT_DATA*  pWindowData, LIST_ITEM*  pTargetLine, CONST UINT  iParameterIndex, CONST PROJECT_FILE*  pProjectFile, UINT&  iOutput)
{
   PROJECT_VARIABLE *pProjectVariable;    // Project variable
   CODE_EDIT_LINE   *pLineData;           // Line data
   CONST TCHAR      *szTargetVariable,    // Variable being resolved
                    *szGlobalVariable;    // Global/Local variable that target variable has been assigned to
   UINT              iNameParameter;      // ParameterIndex of the Global/Local variable name
   BOOL              bSearching,          // Search flag
                     bResult;             // Operation result

   // Prepare
   TRACK_FUNCTION();
   pLineData = extractListItemPointer(pTargetLine, CODE_EDIT_LINE);
   bResult   = FALSE;

   /// [INTEGER] Return value directly
   if (findIntegerParameterInCommandByIndex(pLineData->pCommand, iParameterIndex, (INT&)iOutput))
      bResult = TRUE;

   /// [VARIABLE] Attempt to resolve value
   else if (findVariableParameterInCommandByIndex(pLineData->pCommand, iParameterIndex, szTargetVariable))
   {
      // Prepare
      bSearching = TRUE;

      /// Iterate backwards through the commands
      for (LIST_ITEM*  pIterator = pTargetLine->pPrev; bSearching AND (pLineData = extractListItemPointer(pIterator, CODE_EDIT_LINE)); pIterator = pIterator->pPrev)
      {
         // Examine command
         switch (pLineData->iCommandID)
         {
         /// [EXPRESSION] Check for format '$iPageID = 5555'
         case CMD_EXPRESSION:
            // [CHECK] Ensure this is an assignment of our variable
            if (getCommandParameterCount(pLineData->pCommand, PT_DEFAULT) == 2 AND isReturnVariableInCommand(pLineData->pCommand, szTargetVariable))
            {
               /// [FOUND] Extract integer value if possible, otherwise fail
               bResult    = findIntegerParameterInCommandByIndex(pLineData->pCommand, 1, (INT&)iOutput);
               bSearching = FALSE;
            }  
            break;

         /// [LOCAL/GLOBAL] Use ProjectFile to resolve value
         case CMD_GET_GLOBAL_VARIABLE:         // "$1 get global variable: name=$0" 
         case CMD_GET_LOCAL_VARIABLE:          // "$2 $0 get local variable: name=$1" 
            // [CHECK] Ensure ReturnObject is the correct variable
            if (isReturnVariableInCommand(pLineData->pCommand, szTargetVariable))
            {
               // Prepare
               iNameParameter = (pLineData->iCommandID == CMD_GET_GLOBAL_VARIABLE ? 0 : 1);

               // [CHECK] Ensure Global variable name is present and can be resolved
               if (bResult = pProjectFile AND findStringParameterInCommandByIndex(pLineData->pCommand, iNameParameter, szGlobalVariable) AND findVariableInProjectFileByName(pProjectFile, szGlobalVariable, pProjectVariable))
                  /// [SUCCESS] Return global variable value
                  iOutput = pProjectVariable->iValue;

               // [ABORT]
               bSearching = FALSE;
            }
            break;
         }
      }
   }

   // Return result
   END_TRACKING();
   return bResult;
}

/// Function name  : updateCodeEditScope
// Description     : Searches for the first label preceeding the caret and updates the parent if it has changed
// 
// CODE_EDIT_DATA*  pWindowData : [in] Window data
// 
VOID  updateCodeEditScope(CODE_EDIT_DATA*  pWindowData)
{
   CODE_EDIT_LABEL*    pNewScope;         // Current scope
   CODE_EDIT_LINE*     pLineData;         // LineData for the line currently being processed
   LIST_ITEM*          pLineIterator;     // LineData iterator
   CONST TCHAR*        szLabelName;       // Name of label being processed
   UINT                iLineNumber;       // Line number of the line currently being processed

   // Prepare
   TRACK_FUNCTION();
   iLineNumber = pWindowData->oCaret.oLocation.iLine;
   pNewScope   = NULL;

   // [CHECK] Do not update scope without game data
   if (getAppState() == AS_GAME_DATA_LOADED)
   {
      /// Iterate through all lines in the control
      for (findCodeEditLineListItemByIndex(pWindowData, iLineNumber, pLineIterator); pLineData = extractListItemPointer(pLineIterator, CODE_EDIT_LINE); pLineIterator = pLineIterator->pPrev)
      {
         // [CHECK] Ensure COMMAND exists
         ASSERT(pLineData->pCommand);

         // [CHECK] Is this a label?
         if (pLineData->iCommandID == CMD_DEFINE_LABEL AND findStringParameterInCommandByIndex(pLineData->pCommand, 0, szLabelName))
         {
            /// [FOUND] Create new label and abort
            pNewScope = createCodeEditLabel(szLabelName, iLineNumber);
            break;
         }
         
         // [NOT FOUND] Keep searching
         iLineNumber--;
      }

      /// [GLOBAL SCOPE] Use a label with no name
      if (!pNewScope)
         pNewScope = createCodeEditLabel(TEXT(""), 0);
      
      // [CHECK] Has scope changed?
      if (!utilCompareStringVariables(pWindowData->pCurrentScope->szName, pNewScope->szName) OR pWindowData->pCurrentScope->iLineNumber != pNewScope->iLineNumber)
      {
         /// [CHANGED] Update current scope
         deleteCodeEditLabel(pWindowData->pCurrentScope);
         pWindowData->pCurrentScope = pNewScope;

         // [CHECK] Ensure events aren't disabled
         if (pWindowData->bEventsEnabled)
            /// [EVENT] Inform parent
            SendMessage(GetParent(pWindowData->hWnd), UN_CODE_EDIT_CHANGED, CCF_SCOPE_CHANGED, (LPARAM)pNewScope);
      }
      else
         // [UNCHANGED] Cleanup
         deleteCodeEditLabel(pNewScope);
   }
   
   // [TRACK]
   END_TRACKING();
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                      MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onCodeEditClearLineErrors
// Description     : Clears the errors from every line in the CodeEdit
// 
// CODE_EDIT_DATA*  pWindowData : [in] Window data
// 
VOID  onCodeEditClearLineErrors(CODE_EDIT_DATA*  pWindowData)
{
   CODE_EDIT_LINE*  pLineData;        // LineData for the line being processed
   LIST_ITEM*       pLineIterator;    // Line data iterator
   UINT             iCurrentLine;     // Line number of the line being processed

   // Prepare
   TRACK_FUNCTION();
   iCurrentLine = 0;

   /// Iterate through all lines
   for (findCodeEditLineListItemByIndex(pWindowData, 0, pLineIterator); pLineIterator; pLineIterator = pLineIterator->pNext)
   {
      // Extract LineData
      pLineData = extractListItemPointer(pLineIterator, CODE_EDIT_LINE);

      /// Delete ErrorQueue and redraw line
      if (pLineData->pErrorQueue)
      {
         deleteErrorQueue(pLineData->pErrorQueue);
         updateCodeEditLine(pWindowData, iCurrentLine, CCF_ERROR_CHANGED);
      }

      // Advance line counter
      iCurrentLine++;
   }

   // [TRACK]
   END_TRACKING();
}


/// Function name  : onCodeEditClipboardCopy
// Description     : Copy the currently selected text (if any) to the clipboard
// 
// CONST CODE_EDIT_DATA*  pWindowData : [in] Window data
// 
VOID  onCodeEditClipboardCopy(CONST CODE_EDIT_DATA*  pWindowData)
{
   HGLOBAL  hOutputHandle;       // Handle to the clipboard memory
   TCHAR   *szOutputBuffer,      // Clipboard memory buffer
           *szSelectionText;     // Currently selected text
   UINT     iLength;             // Length of selection
   
   // [TRACK]
   TRACK_FUNCTION();

   // [CHECK] Ensure a selection exists
   if (hasCodeEditSelection(pWindowData))
   {
      // Prepare
      iLength = calculateCodeEditSelectionLength(pWindowData);

      /// Allocate clipboard memory
      hOutputHandle = GlobalAlloc(GMEM_MOVEABLE, (iLength + 1) * sizeof(TCHAR));

      // Attempt to open the clipboard
      if (hOutputHandle AND OpenClipboard(pWindowData->hWnd))
      {
         // Prepare
         EmptyClipboard();
         szOutputBuffer = (TCHAR*)GlobalLock(hOutputHandle);

         /// Copy selection text to the clipboard buffer
         szSelectionText = generateCodeEditSelectionText(pWindowData);
         StringCchCopy(szOutputBuffer, iLength + 1, szSelectionText);

         /// Transfer ownership to the clipboard
         GlobalUnlock(hOutputHandle);
         SetClipboardData(CF_UNICODETEXT, hOutputHandle);

         // Cleanup
         CloseClipboard();
         utilDeleteString(szSelectionText);
      }
      // [ERROR] Free clipboard memory
      else
         GlobalFree(hOutputHandle);
   }

   // [TRACK]
   END_TRACKING();
}


/// Function name  : onCodeEditClipboardCut
// Description     : Cut the currently selected text (if any) to the clipboard
// 
// CODE_EDIT_DATA*  pWindowData : [in] Window data
// 
VOID  onCodeEditClipboardCut(CODE_EDIT_DATA*  pWindowData)
{
   // [TRACK]
   TRACK_FUNCTION();

   // [CHECK] Ensure a selection exists
   if (hasCodeEditSelection(pWindowData))
   {
      /// Copy text
      onCodeEditClipboardCopy(pWindowData);

      /// Remove text
      removeCodeEditSelectionText(pWindowData, TRUE);
   }

   // [TRACK]
   END_TRACKING();
}


/// Function name  : onCodeEditClipboardPaste
// Description     : Paste the text on the clipboard (if any) at the position of the caret
// 
// CODE_EDIT_DATA*  pWindowData : [in] Window data
// 
VOID  onCodeEditClipboardPaste(CODE_EDIT_DATA*  pWindowData)
{
   HANDLE  hDataHandle;          // Clipboard memory
   TCHAR*  szClipboardText;      // Clipboard text buffer

   // [TRACK]
   TRACK_FUNCTION();

   /// [SELECTION] Delete current selection, if any
   if (hasCodeEditSelection(pWindowData))
      removeCodeEditSelectionText(pWindowData, TRUE);

   // Open Clipboard
   if (OpenClipboard(pWindowData->hWnd))
   {
      // Get clipboard memory
      if (hDataHandle = GetClipboardData(CF_UNICODETEXT))
      {
         // Get clipboard buffer
         szClipboardText = (TCHAR*)GlobalLock(hDataHandle);

         /// Insert into CodeEdit one character at a time
         insertCodeEditTextAtCaret(pWindowData, szClipboardText, TRUE);

         // Cleanup
         GlobalUnlock(hDataHandle);
      }

      // Cleanup
      CloseClipboard();
   }

   // [TRACK]
   END_TRACKING();
}


/// Function name  : onCodeEditFindText
// Description     : Searches for a phrase following the caret and selects it
// 
// CODE_EDIT_DATA*  pWindowData : [in] Window data
// CONST TCHAR*     szText      : [in] Text to find
// 
// Return Value : TRUE if found, otherwise FALSE
// 
BOOL  onCodeEditFindText(CODE_EDIT_DATA*  pWindowData, CONST TCHAR*  szSearchText, CONST TCHAR*  szReplaceText, CONST UINT  eFlags)
{
   CODE_EDIT_LOCATION  oNewCaretLocation; // New Caret location
   CODE_EDIT_LINE*     pLineData;         // LineData for the line being processed
   LIST_ITEM*          pLineIterator;     // Line data iterator
   TCHAR*              szLineText;
   CONST TCHAR        *szResult,
                      *szStartPos;
   UINT                iCurrentLine;      // Line number of the line being processed
   BOOL                bFound;

   // Prepare
   TRACK_FUNCTION();
   VERBOSE_LIB_COMMAND();
   iCurrentLine = (eFlags INCLUDES CSF_FROM_CARET ? pWindowData->oCaret.oLocation.iLine : 0);
   bFound       = FALSE;

   // [CHECK] Are we performing a replacement?
   if (utilIncludes(eFlags, CSF_REPLACE) AND lstrlen(szReplaceText) AND hasCodeEditSelection(pWindowData))
   {
      /// [REPLACE] Replace selected text
      removeCodeEditSelectionText(pWindowData, TRUE);
      insertCodeEditTextAtCaret(pWindowData, szReplaceText, TRUE);
   }

   /// Iterate through all lines following the caret
   for (findCodeEditLineListItemByIndex(pWindowData, iCurrentLine, pLineIterator); !bFound AND (pLineData = extractListItemPointer(pLineIterator, CODE_EDIT_LINE)); pLineIterator = pLineIterator->pNext)
   {
      // Get line text
      szLineText = generateCodeEditLineText(pLineData);

      // [CHECK] Search entire line unless we're searching from the caret  (and this line contains the caret)
      szStartPos = (utilExcludes(eFlags, CSF_FROM_CARET) OR iCurrentLine != pWindowData->oCaret.oLocation.iLine ? szLineText : &szLineText[pWindowData->oCaret.oLocation.iIndex]);

      /// Search line for desired text
      szResult = (utilIncludes(eFlags, CSF_CASE_SENSITIVE) ? utilFindSubStringVariable(szStartPos, szSearchText) : utilFindSubStringVariableI(szStartPos, szSearchText));
      
      // [CHECK] Was text found?
      if (szResult)
      {
         // Cancel current selection
         removeCodeEditSelection(pWindowData);

         // Calculate selection origin
         pWindowData->oSelection.oOrigin.iLine  = iCurrentLine;
         pWindowData->oSelection.oOrigin.iIndex = (szResult - szLineText);

         // Calculate caret position
         oNewCaretLocation         = pWindowData->oSelection.oOrigin;
         oNewCaretLocation.iIndex += lstrlen(szSearchText);

         /// [FOUND] Move caret and select text
         setCodeEditCaretLocation(pWindowData, &oNewCaretLocation);
         setCodeEditSelection(pWindowData, pWindowData->oSelection.oOrigin, oNewCaretLocation, TRUE);

         // [SEARCH] Scroll result into view
         ensureCodeEditLocationIsVisible(pWindowData, &oNewCaretLocation);

         // Abort search
         bFound = TRUE;
      }

      // Advance line counter
      iCurrentLine++;
      utilDeleteString(szLineText);
   }

   // [TRACK]
   END_TRACKING();
   return bFound;
}


/// Function name  : onCodeEditInsertTextAtCaret
// Description     : Inserts a search result and ensures it's visible
// 
// CODE_EDIT_DATA*  pWindowData    : [in] Window data
// CONST TCHAR*     szSearchResult : [in] Search result
// 
VOID  onCodeEditInsertTextAtCaret(CODE_EDIT_DATA*  pWindowData, CONST TCHAR*  szText)
{
   // Insert text and scroll to position
   insertCodeEditTextAtCaret(pWindowData, szText, TRUE);
   ensureCodeEditLocationIsVisible(pWindowData, &pWindowData->oCaret.oLocation);
}


/// Function name  : onCodeEditScriptCallOperator
// Description     : Requests script-call arguments from the parent if appropriate
// 
// CODE_EDIT_DATA*  pWindowData : [in] Window data
// CODE_EDIT_LINE*  pLineData   : [in] Line containing the caret
// 
VOID  onCodeEditScriptCallOperator(CODE_EDIT_DATA*  pWindowData, CODE_EDIT_LINE*  pLineData)
{
   CODE_EDIT_LOCATION*  pCaretLocation;
   TCHAR*               szLineText;

   // Prepare
   pCaretLocation = &pWindowData->oCaret.oLocation;
   szLineText     = generateCodeEditLineText(pLineData);

   // [CHECK] Ensure there is no text (ie. arguments) following the ':' operator
   if (pCaretLocation->iIndex >= (UINT)lstrlen(szLineText) OR isStringWhitespace(&szLineText[pCaretLocation->iIndex]))
      // [SUCCESS] Request parent insert script-call arguments
      SendMessage(GetParent(pWindowData->hWnd), UN_CODE_EDIT_REQUEST_ARGUMENTS, NULL, (LPARAM)pLineData->pCommand);
   
   // Cleanup
   utilDeleteString(szLineText);
}


/// Function name  : onCodeEditScrollToLocation
// Description     : Scrolls the CodeEdit to a given line and character, and moves the caret there as well.
//                      -> This is not used internally by the CodeEdit, but a handler for UM_SCROLL_TO_LOCATION
// 
// CODE_EDIT_DATA*           pWindowData : [in] Window Data
// CONST CODE_EDIT_LOCATION*  pTarget     : [in] Location to scroll to
// 
VOID  onCodeEditScrollToLocation(CODE_EDIT_DATA*  pWindowData, CONST CODE_EDIT_LOCATION*  pTarget)
{
   // [TRACK]
   TRACK_FUNCTION();

   // Attempt to position on first line
   onCodeEditScroll(pWindowData, SB_THUMBTRACK, pTarget->iLine, SB_VERT);

   // Ensure character index is visible
   ensureCodeEditLocationIsVisible(pWindowData, pTarget);

   // Move caret to target location
   setCodeEditCaretLocation(pWindowData, pTarget);

   // [TRACK]
   END_TRACKING();
}


/// Function name  : onCodeEditSelectAll
// Description     : Selects all characters in the CodeEdit and moves caret to final character
// 
// CODE_EDIT_DATA*  pWindowData : [in] Window data
// 
VOID  onCodeEditSelectAll(CODE_EDIT_DATA*  pWindowData)
{
   CODE_EDIT_LOCATION  oCaretLocation;

   // Set selection origin to first character
   pWindowData->oSelection.oOrigin.iLine  = 0;
   pWindowData->oSelection.oOrigin.iIndex = 0;

   // Set caret to last character
   calculateCodeEditFinalCharacterLocation(pWindowData, &oCaretLocation);
   setCodeEditCaretLocation(pWindowData, &oCaretLocation);

   // Select all characters between
   setCodeEditSelection(pWindowData, pWindowData->oSelection.oOrigin, pWindowData->oCaret.oLocation, TRUE);
}


/// Function name  : onCodeEditSetLineError
// Description     : Assigns a new ErrorQueue to a line of CodeEdit text
// 
// CODE_EDIT_DATA*     pWindowData  : [in] Window data
// CONST UINT          iLineNumber   : [in] Zero-based line number
// CONST ERROR_QUEUE*  pErrorQueue   : [in] Error to assign to line
// 
VOID  onCodeEditSetLineError(CODE_EDIT_DATA*  pWindowData, CONST UINT  iLineNumber, CONST ERROR_TYPE  eType, CONST ERROR_QUEUE*  pErrorQueue)
{
   CODE_EDIT_LINE*  pLineData;

   // [TRACK]
   TRACK_FUNCTION();

   // Lookup LineData
   if (findCodeEditLineDataByIndex(pWindowData, iLineNumber, pLineData))
   {
      // Delete existing error, if any
      if (pLineData->pErrorQueue)
         deleteErrorQueue(pLineData->pErrorQueue);

      /// Copy new error
      pLineData->pErrorQueue = duplicateErrorQueue(pErrorQueue);
      pLineData->eSeverity   = eType;

      // Redraw
      updateCodeEditLine(pWindowData, iLineNumber, CCF_ERROR_CHANGED);
   }

   // [TRACK]
   END_TRACKING();
}

