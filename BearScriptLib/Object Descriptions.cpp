//
// Object Descriptions.cpp : RichText command descriptions
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"
#include "RichText Macros.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                          MACROS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                  CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////

struct OBJECT_COMMAND
{
   const TCHAR*  szName;
   UINT          iBase;
};

// ObjectCommand group names + ID ranges
static OBJECT_COMMAND  oObjectCommands[10] = 
{
   {TEXT("NAV"), 200},    {TEXT("FIGHT"), 300},   {TEXT("TRADE"), 400},  {TEXT("SPECIAL"), 500},  {TEXT("PIRACY"), 600}, 
   {TEXT("CUSTOM"), 700}, {TEXT("GENERAL"), 800}, {TEXT("TURRET"), 900}, {TEXT("STATION"), 1100}, {TEXT("SHIP"), 1200}  
};

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                  CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createDescriptionVariable
// Description     : 
// 
// CONST TCHAR*  szName      : [in] 
// CONST TCHAR*  szValue     : [in] 
// CONST UINT    iParameters : [in] 
// 
// Return Value   : 
// 
DESCRIPTION_VARIABLE*  createDescriptionVariable(CONST TCHAR*  szName, CONST TCHAR*  szValue, CONST UINT  iParameters, CONST BOOL  bRecursive)
{
   DESCRIPTION_VARIABLE*  pVariable = utilCreateEmptyObject(DESCRIPTION_VARIABLE);

   // Set properties
   pVariable->szName      = utilDuplicateSimpleString(szName);
   pVariable->szValue     = utilDuplicateSimpleString(szValue);
   pVariable->bRecursive  = bRecursive;
   pVariable->iParameters = iParameters;

   // Return object
   return pVariable;
}


/// Function name  : createDescriptionVariableTreeByName
// Description     : Creates an DescriptionVariable tree sorted by name in ascending order
// 
// Return Value   : New AVLTree, you are responsible for destroying it
//  
AVL_TREE*  createDescriptionVariableTreeByName()
{
   return createAVLTree(extractDescriptionVariableTreeNode, deleteDescriptionVariableTreeNode, createAVLTreeSortKeyEx(AK_NAME, AO_DESCENDING, AP_STRING_CASE), NULL, NULL);
}


/// Function name  : deleteDescriptionVariable
// Description     : 
// 
// DESCRIPTION_VARIABLE*  &pVariable : [in] 
// 
VOID  deleteDescriptionVariable(DESCRIPTION_VARIABLE*  &pVariable)
{
   utilDeleteStrings(pVariable->szName, pVariable->szValue);

   utilDeleteObject(pVariable);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       HELPERS 
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : deleteDescriptionVariableTreeNode
// Description     : Deletes a DescriptionVariable within an AVLTree Node
// 
// LPARAM  pVariable : [in] Reference to a DescriptionVariable pointer
// 
VOID   deleteDescriptionVariableTreeNode(LPARAM  pVariable)
{
   // Delete node contents
   deleteDescriptionVariable((DESCRIPTION_VARIABLE*&)pVariable);
}


/// Function name  : extractDescriptionVariableTreeNode
// Description     : Extract the desired property from a given TreeNode containing a DescriptionVariable
// 
// LPARAM                   pNode      : [in] MEDIA_ITEM* : Node data containing a DescriptionVariable
// CONST AVL_TREE_SORT_KEY  eProperty  : [in] Property to retrieve
// 
// Return Value   : Value of the specified property or NULL if the property is unsupported
// 
LPARAM  extractDescriptionVariableTreeNode(LPARAM  pNode, CONST AVL_TREE_SORT_KEY  eProperty)
{
   DESCRIPTION_VARIABLE*  pVariable;    // Convenience pointer
   LPARAM                xOutput;        // Property value output

   // Prepare
   pVariable = (DESCRIPTION_VARIABLE*)pNode;

   // Examine property
   switch (eProperty)
   {
   case AK_NAME:  xOutput = (LPARAM)pVariable->szName;       break;
   case AK_TEXT:  xOutput = (LPARAM)pVariable->szValue;      break;
   default:       xOutput = NULL;                            break;
   }

   // Return property value
   return xOutput;
}


/// Function name  : insertDescriptionIntoVariablesFile
// Description     : Inserts a new DescriptionVariable into a VariablesFile
// 
// VARIABLES_FILE*  pVariablesFile  : [in] Variables File
// CONST TCHAR*     szName          : [in] Variable name
// CONST TCHAR*     szValue         : [in] Variable value
// CONST UINT       iParameterCount : [in] 
// ERROR_STACK*    &pError          : [out] Error if any
// 
// Return Value   : TRUE if successful, FALSE if a duplicate
// 
BOOL   insertDescriptionIntoVariablesFile(VARIABLES_FILE*  pVariablesFile, CONST TCHAR*  szName, CONST TCHAR*  szValue, CONST UINT  iParameterCount, CONST BOOL  bRecursive, ERROR_STACK*  &pError)
{
   DESCRIPTION_VARIABLE    *pNewVariable;           // GameString being inserted

   // Prepare
   pError = NULL;

   // Create external GameString
   pNewVariable = createDescriptionVariable(szName, szValue, iParameterCount, bRecursive);

   /// Attempt to insert language file's string tree
   if (!insertObjectIntoAVLTree(pVariablesFile->pVariablesByText, (LPARAM)pNewVariable))
   {
      /// [WARNING] "Unable to add the duplicate description variable '%s'"
      pError = generateDualWarning(HERE(IDS_DESCRIPTION_VARIABLE_DUPLICATE), szName);

      // Cleanup
      deleteDescriptionVariable(pNewVariable);
   }

   // Return TRUE if there was no error
   return (pError == NULL);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                      FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : generatePopulatedDescriptionSource
// Description     : Populates CommandSyntax description source text with parameter names
// 
// CONST COMMAND_SYNTAX*  pSyntax       : [in] CommandSyntax
// CONST TCHAR*           szInputSource : [in] Description source text
// CONST UINT             iInputLength  : [in] Length of source text
// 
// Return Value   : New description source-text, you are responsible for destroying it
// 
TCHAR*  generatePopulatedDescriptionSource(CONST TCHAR*  szSourceText, CONST UINT  iSourceLength, CONST AVL_TREE*  pVariablesTree, CONST COMMAND_SYNTAX*  pSyntax, CONST UINT  iPageID, CONST UINT  iStringID, ERROR_QUEUE*  pErrorQueue)
{
   CONST UINT             iMaxParameters = 6;            //           Maximum number of parameters a command can have
   DESCRIPTION_VARIABLE  *pVariable;                     //           DescriptionVariable
   CODEOBJECT            *pCodeObject;                   //           Parses the input source-text
   CONST TCHAR           *szPunctuation;                 // [KEYWORD] Punctuation following the keyword (if any)
   TCHAR                 *szOutput,                      //           Operation result
                         *szKeyword,                     //           Variable ID  (or text)
                         *szCommand,                     // [COMMAND] Entire command text, including parameters
                         *szFirstParameter,              // [COMMAND] Remaining Command parameters
                         *szParameters[iMaxParameters],  // [COMMAND] Command parameters
                         *szIterator,                    // [COMMAND] Parameters tokeniser
                         *szAssembledTemplate,           // [COMMAND] Unpopulated variable source-text with parameters inserted
                         *szPopulatedVariable;           //           Populated variable source-text
   UINT                   iOutputLength,                 //           Length of output buffer
                          iParameterCount;               // [COMMAND] Number of parameters parsed

   // Prepare
   pCodeObject = createCodeObject(szSourceText);
   szOutput    = utilCreateEmptyString(iOutputLength = 4096);
   
   /// Split CommandSyntax string into CodeObjects
   while (findNextCodeObject(pCodeObject))
   {
      // Examine CodeObject
      switch (pCodeObject->eClass)
      {
      /// [PARAMETER SYNTAX / VARIABLE NAME]
      case CO_VARIABLE:
         populateDescriptionSyntax(pCodeObject->szText, szOutput, iOutputLength, pSyntax, pErrorQueue);
         break;

      /// [KEYWORD / TEXT] Highlight common words with a uniform formatting
      case CO_LABEL:
      case CO_NULL:
      case CO_WORD:
         // Extract keyword without punctuation
         szPunctuation = utilFindCharacterInSet(pCodeObject->szText, ",.:;!?");
         szKeyword     = utilDuplicateString(pCodeObject->szText, (szPunctuation ? szPunctuation - pCodeObject->szText : pCodeObject->iLength));

         /// [CHECK] Lookup description variable
         if (findObjectInAVLTreeByValue(pVariablesTree, (LPARAM)szKeyword, NULL, (LPARAM&)pVariable))
         {
            // [CHECK] Ensure variable can be recursively populated
            if (pVariable->bRecursive)
            {
               // [SUCCESS] Populate source-text
               szPopulatedVariable = generatePopulatedDescriptionSource(pVariable->szValue, lstrlen(pVariable->szValue), pVariablesTree, pSyntax, iPageID, iStringID, pErrorQueue);

               // Append populated source-text
               StringCchCat(szOutput, iOutputLength, szPopulatedVariable);
               utilDeleteString(szPopulatedVariable);
            }
            else
               // [FAILED] Output variable without population
               StringCchCat(szOutput, iOutputLength, pVariable->szValue);

            // [PUNCTUATION] Append any remaining punctuation
            if (szPunctuation)
               StringCchCat(szOutput, iOutputLength, szPunctuation);
            
         }
         else
            /// [TEXT] Output verbatim
            StringCchCat(szOutput, iOutputLength, pCodeObject->szText);

         // Cleanup
         utilDeleteString(szKeyword);
         break;

      /// [COMMAND]
      case CO_ENUMERATION:
         // Prepare
         utilZeroObjectArray(szParameters, CONST TCHAR*, iMaxParameters);
         iParameterCount = 0;

         // Extract command
         szCommand = utilDuplicateString(&pCodeObject->szText[1], pCodeObject->iLength - 2);

         // [CHECK] Does this command have parameters?
         if (szFirstParameter = utilFindCharacter(szCommand, ':'))
         {
            // Destroy ':' operator
            szFirstParameter[0] = NULL;

            // [CHECK] Is this a HEADING?
            if (utilCompareString(szCommand, "HEADING"))
            {
               // [HEADING] Assume any commas in the parameter are just commas, and not parameter operators
               szFirstParameter++;  // Consume ':' operator
               iParameterCount  = 1;
            }
            else
            {
               // [COMMAND] Extract first parameter
               szFirstParameter = utilTokeniseString(&szFirstParameter[1], ",", &szIterator);
               iParameterCount  = 1;

               /// Tokenise remaining parameters
               while (szIterator[0] AND iParameterCount < iMaxParameters)
                  szParameters[iParameterCount++] = utilGetNextToken(",", &szIterator);
            }
         }

         /// Lookup DescriptionVariable
         if (findObjectInAVLTreeByValue(pVariablesTree, (LPARAM)szCommand, NULL, (LPARAM&)pVariable))
         {
            // [CHECK] Ensure parameter count is correct
            if (pVariable->iParameters != iParameterCount)
               // [ERROR] "The description variable '%s' in the description for the object in page %u with ID:%u has %u parameters where %u were expected"
               pushErrorQueue(pErrorQueue, generateDualError(HERE(IDS_DESCRIPTION_VARIABLE_PARAMETER_COUNT), szCommand, iPageID, iStringID, iParameterCount, pVariable->iParameters));
            else
            {
               // [SUCCESS] Insert parameters into template
               szAssembledTemplate = utilCreateStringf(2048, pVariable->szValue, szFirstParameter, szParameters[1], szParameters[2], szParameters[3], szParameters[4], szParameters[5]);

               // Populate template
               szPopulatedVariable = generatePopulatedDescriptionSource(szAssembledTemplate, lstrlen(szAssembledTemplate), pVariablesTree, pSyntax, iPageID, iStringID, pErrorQueue);

               // Append populated source-text
               StringCchCat(szOutput, iOutputLength, szPopulatedVariable);

               // Cleanup
               utilDeleteStrings(szAssembledTemplate, szPopulatedVariable);
            }
         }
         else
            // [ERROR] "Unknown description variable '%s' detected in the description for object in page %u with ID:%u"
            pushErrorQueue(pErrorQueue, generateDualError(HERE(IDS_DESCRIPTION_VARIABLE_UNKNOWN), szCommand, iPageID, iStringID));
         
         // Cleanup
         utilDeleteString(szCommand);
         break;

      /// [REMAINDER] Copy without change
      default:
         StringCchCat(szOutput, iOutputLength, pCodeObject->szText);
         break;
      }
   }

   // Cleanup and return new source
   deleteCodeObject(pCodeObject);
   return szOutput;
}


/// Function name  : populateDescriptionSyntax
// Description     : 
// 
// CONST COMMAND_SYNTAX*  pSyntax       : [in] 
// TCHAR*                 szText        : [in] 
// TCHAR*                 szOutput      : [in] 
// CONST UINT             iOutputLength : [in] 
// 
VOID  populateDescriptionSyntax(CONST TCHAR*  szText, TCHAR*  szOutput, CONST UINT  iOutputLength, CONST COMMAND_SYNTAX*  pSyntax, ERROR_QUEUE*  pErrorQueue)
{
   PARAMETER_SYNTAX  eParameterSyntax;    // Syntax of the current parameter
   CONST TCHAR      *szFirstSubScript,    // First parameter sub-script [optional]
                    *szSecondSubScript;   // Second parameter sub-script [optional]

   // [CHECK] Output verbatim if this is a ScriptObject
   if (!pSyntax)
      StringCchCat(szOutput, iOutputLength, szText);

   // Examine character following the $ symbol
   else switch (szText[1])
   {
   /// [PARAMETER] Generate and insert parameter text
   case '0': case '2': case '4': case '6': case '8':
   case '1': case '3': case '5': case '7': case '9':
      // Extract Parameter syntax
      if (findParameterSyntaxByPhysicalIndex(pSyntax, utilConvertCharToInteger(szText[1]), eParameterSyntax))
      {
         /// [SPECIAL CASE] Fudge some entries to make them pretty
         switch (eParameterSyntax)
         {
         // [@RetVar/IF] Change to 'RetVar/IF'
         case PS_INTERRUPT_RETURN_OBJECT_IF:  eParameterSyntax = PS_RETURN_OBJECT_IF;  break;
         // [Label Number] Change to 'Label Name'
         case PS_LABEL_NUMBER:                eParameterSyntax = PS_LABEL_NAME;        break;
         }

         // Identify sub-scripts, if any
         szFirstSubScript  = (szText[2] ? identifyParameterSyntaxSubScript(szText[2]) : NULL);
         szSecondSubScript = (szText[3] ? identifyParameterSyntaxSubScript(szText[3]) : NULL);

         // [CHECK] Are there double sub-scripts?
         if (szFirstSubScript AND szSecondSubScript)
            /// [DOUBLE SUB-SCRIPT] Insert both sub-scripts  (and any appended punctuation)
            utilStringCchCatf(szOutput, iOutputLength, txtBold("«%s%s%s»") TEXT("%s"), szParameterSyntax[eParameterSyntax], szFirstSubScript, szSecondSubScript, &szText[4]);
         // [CHECK] Is there a single sub-script?
         else if (szFirstSubScript)
            /// [SUB-SCRIPT] Insert sub-script  (and any appended punctuation)
            utilStringCchCatf(szOutput, iOutputLength, txtBold("«%s%s»") TEXT("%s"), szParameterSyntax[eParameterSyntax], szFirstSubScript, &szText[3]);
         else
            /// [NO SUB-SCRIPT] Insert without sub-script  (and any appended punctuation)
            utilStringCchCatf(szOutput, iOutputLength, txtBold("«%s»") TEXT("%s"), szParameterSyntax[eParameterSyntax], &szText[2]);
      }
      else
         // [ERROR] "An unknown parameter ID '%s' was detected in the description for command ID:%u with syntax '%s'"
         pushErrorQueue(pErrorQueue, generateDualError(HERE(IDS_DESCRIPTION_PARAMETER_UNKNOWN), szText, pSyntax->iID, pSyntax->szSyntax));
      break;

   /// [VARIABLE NAME] Insert verbatim
   default:
      StringCchCat(szOutput, iOutputLength, szText);
      break;
   }
}

/// Function name  : loadObjectDescriptions
// Description     : Load the object descriptions LanguageFile, generates RichText descriptions and assigns them to objects
// 
// OPERATION_PROGRESS*  pProgress   : [in/out] Operation progress
// HWND                 hParentWnd  : [in]     Parent window of any error dialogs that are displayed
// ERROR_QUEUE*         pErrorQueue : [in/out] Error messages, if any
// 
// Return Value : OR_SUCCESS - Command descriptions were loaded successfully, if there were any minor errors the user ignored them
//                OR_ABORTED - Command descriptions were NOT loaded due to the user aborting after minor errors
//                OR_FAILURE - Command descriptions were NOT loaded due to a critical error. (corrupt XML file or resource library)
// 
OPERATION_RESULT  loadObjectDescriptions(OPERATION_PROGRESS*  pProgress, HWND  hParentWnd,  ERROR_QUEUE*  pErrorQueue)
{
   AVL_TREE_OPERATION*  pOperationData;         // Generation operation data
   LANGUAGE_FILE*       pLanguageFile;          // Temporary LanguageFile to parse the CommandDescriptions.xml into
   VARIABLES_FILE*      pVariablesFile;         //
   ERROR_STACK*         pError  = NULL;         // Current error, if any
   OPERATION_RESULT     eResult = OR_SUCCESS;   // Operation result

   __try
   {
      CONSOLE_STAGE();
      CONSOLE("Loading object descriptions");

      // [INFO/STAGE] "Loading object descriptions"  (Define progress as percentage)
      pushErrorQueue(pErrorQueue, generateDualInformation(HERE(IDS_PROGRESS_LOADING_COMMAND_DESCRIPTIONS)));
      ASSERT(advanceOperationProgressStage(pProgress) == IDS_PROGRESS_LOADING_COMMAND_DESCRIPTIONS);
      updateOperationProgressMaximum(pProgress, 100);
      
      // [CHECK] Ensure Command syntax tree exists
      ASSERT(getGameData()->pCommandTreeByID AND getGameData()->pCommandTreeByID->iCount > 0);

      // Prepare
      pLanguageFile  = createLanguageFile(LFT_DESCRIPTIONS, TEXT("OBJECT_DESCRIPTIONS"), FALSE);
      pVariablesFile = createLanguageFile(LFT_VARIABLES, TEXT("DESCRIPTION_VARIABLES"), FALSE);

      /// Load the command descriptions from the resource library
      if (!loadGameFileFromResource(getResourceInstance(), TEXT("OBJECT_DESCRIPTIONS"), pLanguageFile))
      {
         // [ERROR] "There was an unspecified error while loading the command descriptions file from the resource library. This could indicate the resource library is corrupt."
         pushErrorQueue(pErrorQueue, pError = generateDualError(HERE(IDS_DESCRIPTION_FILE_IO_ERROR)));
         generateOutputTextFromError(pError);
         eResult = OR_FAILURE;
      }
      /// Load the description variables from the resource library
      else if (!loadGameFileFromResource(getResourceInstance(), TEXT("DESCRIPTION_VARIABLES"), pVariablesFile))
      {
         // [ERROR] "There was an unspecified error while loading the description variables file from the resource library. This could indicate the resource library is corrupt."
         pushErrorQueue(pErrorQueue, pError = generateDualError(HERE(IDS_VARIABLES_FILE_IO_ERROR)));
         generateOutputTextFromError(pError);
         eResult = OR_FAILURE;
      }
      else
      {
         // [PROGRESS] Set progress to 10%
         updateOperationProgressValue(pProgress, 10);

         /// Read descriptions XML file
         if (eResult = loadLanguageFile(NULL, pLanguageFile, FALSE, hParentWnd, NULL, pErrorQueue))
            // [ERROR] "The command descriptions file could not be translated due to syntax errors in the XML. This could indicate a corrupted resource library."
            enhanceLastError(pErrorQueue, ERROR_ID(IDS_DESCRIPTION_FILE_TRANSLATION_FAILED));

         /// Read description variables XML file
         else if (eResult = loadLanguageFile(NULL, pVariablesFile, FALSE, hParentWnd, NULL, pErrorQueue))
            // [ERROR] "The description variables file could not be translated due to syntax errors in the XML. This could indicate a corrupted resource library."
            enhanceLastError(pErrorQueue, ERROR_ID(IDS_VARIABLES_FILE_TRANSLATION_FAILED));
         else
         {
            // [VERBOSE]
            CONSOLE_HEADING("Loaded source text for %u object descriptions and %u description variables", getTreeNodeCount(pLanguageFile->pGameStringsByVersion), getTreeNodeCount(pVariablesFile->pVariablesByText));

            // [PROGRESS] Redefine progress as the number of nodes processed, plus 20%
            updateOperationProgressMaximum(pProgress, utilCalculatePercentage(getTreeNodeCount(pLanguageFile->pGameStringsByVersion), 120));
            // [HACK] Manually set progress to 24%.  (Progress will be displayed as 24%, and tree operation will update progress smoothly to 100%)
            updateOperationProgressValue(pProgress, pProgress->pCurrentStage->iMaximum - getTreeNodeCount(pLanguageFile->pGameStringsByVersion)); 

            /// Generate and assign descriptions
            CONSOLE("Generating RichText object descriptions");

            // Create operation data. Attach Progress + DescriptionVariables tree
            pOperationData = createAVLTreeOperationEx(treeprocGenerateObjectDescription, ATT_INORDER, pErrorQueue, pProgress);
            pOperationData->xFirstInput = (LPARAM)pVariablesFile->pVariablesByText;
            performOperationOnAVLTree(pLanguageFile->pGameStringsByVersion, pOperationData);
            
            // [DEBUG] 
            CONSOLE_HEADING("Successfully generated %u object descriptions, with %d discarded", pOperationData->xOutput, pOperationData->xOutput2);

            // [DISCARDS] Generate warning if descriptions were discarded
            if (pOperationData->xOutput2 > 0)
               // [WARNING] "Discarded %d description tooltips for commands/objects incompatible with your '%s' game data"
               generateQueuedWarning(pOperationData->pErrorQueue, HERE(IDS_DESCRIPTION_DISCARD_COUNT), pOperationData->xOutput2, identifyGameVersionString(getAppPreferences()->eGameVersion));

            // Cleanup and sever ErrorQueue
            pOperationData->pErrorQueue = NULL;
            deleteAVLTreeOperation(pOperationData);

            /// Generate descriptions for Object/Wing commands
            performOperationOnAVLTree(getGameData()->pScriptObjectsByGroup, pOperationData = createAVLTreeOperation(treeprocGenerateObjectCommandDescriptions, ATT_INORDER));

            // Cleanup
            deleteAVLTreeOperation(pOperationData);
         }
      }

      // Cleanup and return
      deleteLanguageFile(pLanguageFile);
      deleteLanguageFile(pVariablesFile);
      return eResult;
   }
   __except (pushException(pErrorQueue))
   {
      EXCEPTION("Unable to the load object descriptions");
      return OR_FAILURE;
   }
}


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    TREE OPERATIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : treeprocConvertDescriptionVariableType
// Description     : Convert a DescriptionVariable tree node from external to internal
// 
// AVL_TREE_NODE*       pNode            : [in] Current tree node
// AVL_TREE_OPERATION*  pOperationData   : [in] xFirstInput - Conversion flags
// 
VOID    treeprocConvertDescriptionVariableType(AVL_TREE_NODE*  pNode, AVL_TREE_OPERATION*  pOperationData)
{
   DESCRIPTION_VARIABLE  *pVariable;     // Convenience pointer
   TCHAR                 *szConverted;   // Convert string, if any

   // Prepare
   pVariable = extractPointerFromTreeNode(pNode, DESCRIPTION_VARIABLE);

   // [CHECK] Did string require conversion?
   if (generateConvertedString(pVariable->szValue, pOperationData->xFirstInput, szConverted))
   {
      /// [SUCCESS] Replace text buffer
      utilDeleteString(pVariable->szValue);
      pVariable->szValue = szConverted;
   }
}


/// Function name  : treeprocGenerateObjectCommandDescriptions
// Description     : Generates descriptions for overridden object/wing commands
// 
// AVL_TREE_NODE*       pNode          : [in] ScriptObject
// AVL_TREE_OPERATION*  pOperationData : [in] Ignored
// 
VOID  treeprocGenerateObjectCommandDescriptions(AVL_TREE_NODE*  pNode, AVL_TREE_OPERATION*  pOperationData)
{
   OBJECT_NAME*  pScriptObject = extractPointerFromTreeNode(pNode, OBJECT_NAME);
   TCHAR        *szCommand,
                *szSource;

   switch (pScriptObject->eGroup)
   {
   case ONG_OBJECT_COMMAND:
   case ONG_WING_COMMAND:
      // [CHECK] Ignore standard commands with low IDs
      if (pScriptObject->iID < oObjectCommands[0].iBase)
         return;

      // Examine all groups
      for (UINT  i = 0; (pScriptObject->eGroup == ONG_OBJECT_COMMAND ? i < 10 : i < 7); i++)
      {
         // [CHECK] Find correct group
         if (pScriptObject->iID >= oObjectCommands[i].iBase AND pScriptObject->iID < oObjectCommands[i].iBase + 64)
         {
            // Generate original command name
            szCommand = utilCreateStringf(128, pScriptObject->eGroup == ONG_OBJECT_COMMAND ? TEXT("COMMAND_TYPE_%s_%d") : TEXT("COMMAND_WING_TYPE_%s_%d"), oObjectCommands[i].szName, pScriptObject->iID - oObjectCommands[i].iBase);

            // Compare new/original command names
            if (!utilCompareStringVariables(szCommand, pScriptObject->szText))
            {
               // [CHANGED]
               szSource = utilCreateStringf(256, TEXT("This command is assigned to slot:\n[center][red][b]%s[/b][/red][/center]"), szCommand);
               generateRichTextFromSourceText(szSource, 256, pScriptObject->pDescription, RTT_RICH_TEXT, ST_DISPLAY, NULL);
               utilDeleteString(szSource);
            }

            utilDeleteString(szCommand);
            break;
         }
      }
      break;
   }
}


/// Function name  : treeprocGenerateObjectDescription
// Description     : Generates RichText from the current node and assigns it to an object
// 
// AVL_TREE_NODE*       pCurrentNode   : [in] GameString
// AVL_TREE_OPERATION*  pOperationData : [in] Operation data:
///                                               xFirstInput : AVL_TREE* : [in]  VariablesTree
///                                               xOutput     : UINT      : [out] Number of descriptions successfully generated
///                                               xOutput2    : UINT      : [out] Number of incompatible descriptions that were discarded
// 
VOID  treeprocGenerateObjectDescription(AVL_TREE_NODE*  pCurrentNode, AVL_TREE_OPERATION*  pOperationData)
{
   SUGGESTION_RESULT  xObject;          // Object wrapper
   ERROR_STACK*       pError;           // Operation error
   GAME_STRING*       pSourceString;    // Convenience pointer
   RICH_TEXT*         pDescription;     // Object description, generated from node source-text
   AVL_TREE*          pVariablesTree;   // DescriptionVariables tree
   TCHAR*             szGroupName;      // Error reporting
   TCHAR*             szPopulatedSource;// Command description source with parameter names filled in
   BOOL               bResult;          // Indicates whether they were any formatting errors in the node source text

   // Prepare
   pSourceString  = extractPointerFromTreeNode(pCurrentNode, GAME_STRING);
   pVariablesTree = (AVL_TREE*)pOperationData->xFirstInput;
   pDescription   = NULL;
   pError         = NULL;
   bResult        = FALSE;

   //DEBUG: VERBOSE("[DESCRIPTION] Generating page %u, string %u", pSourceString->iPageID, pSourceString->iID);

   // Determine object type from page ID
   switch (pSourceString->iPageID)
   {
   /// [COMMAND] Populate parameters, generate description and store in syntax
   case GPI_COMMAND_SYNTAX:
      // Lookup command syntax
      if (!findCommandSyntaxByID(pSourceString->iID, pSourceString->eVersion, xObject.asCommandSyntax, pError))   
         // [WARNING] "Discarding description tooltip for incompatible %s command with ID:%u"
         pOperationData->xOutput2++;      //enhanceWarning(pError, ERROR_ID(IDS_DESCRIPTION_SYNTAX_NOT_FOUND), identifyGameVersionString(pSourceString->eVersion), pSourceString->iID);

      // [CHECK] Ensure syntax has not been repeated
      else if (xObject.asCommandSyntax->pTooltipDescription)
         // [WARNING] "Discarding duplicate description tooltip for %s command '%s'"
         pError = generateDualWarning(HERE(IDS_DESCRIPTION_SYNTAX_DUPLICATE), identifyGameVersionString(pSourceString->eVersion), xObject.asCommandSyntax->szContent);
      
      else
      {
         // Generate populated source text
         szPopulatedSource = generatePopulatedDescriptionSource(pSourceString->szText, pSourceString->iCount, pVariablesTree, xObject.asCommandSyntax, pSourceString->iPageID, pSourceString->iID, pOperationData->pErrorQueue);

         /// Generate RichText from source
         if (bResult = generateRichTextFromSourceText(szPopulatedSource, lstrlen(szPopulatedSource), pDescription, RTT_RICH_TEXT, ST_DISPLAY, pOperationData->pErrorQueue))
            const_cast<COMMAND_SYNTAX*>(xObject.asCommandSyntax)->pTooltipDescription = pDescription;    /// [HACK] Remove 'const' - this is the only time Syntax is edited post-creation
         else
            // [WARNING] "Formatting error detected in description tooltip for %s command '%s'"
            pError = generateDualWarning(HERE(IDS_DESCRIPTION_SYNTAX_FORMAT_ERROR), identifyGameVersionString(pSourceString->eVersion), xObject.asCommandSyntax->szContent);

         // Cleanup
         utilDeleteString(szPopulatedSource);
      }
      break;

   /// [SCRIPT OBJECT] Store as description
   case GPI_CONSTANTS:
   case GPI_DATA_TYPES:
   case GPI_FLIGHT_RETURNS:
   case GPI_OBJECT_CLASSES:
   case GPI_RACES:
   case GPI_RELATIONS:
   case GPI_SECTORS:
   case GPI_TRANSPORT_CLASSES:
      // Prepare
      szGroupName = NULL;

      /// Lookup ScriptObject
      if (!findScriptObjectByGroup(identifyObjectNameGroupFromGameString(pSourceString), pSourceString->iID, xObject.asObjectName))
         // [WARNING] "Discarding description tooltip for incompatible '%s' script object with ID:%u"
         pOperationData->xOutput2++;      //pError = generateDualWarning(HERE(IDS_DESCRIPTION_OBJECT_NOT_FOUND), szGroupName = generateScriptObjectGroupString(identifyObjectNameGroupFromGameString(pSourceString)), pSourceString->iID);
      
      // [CHECK] Ensure description not already present
      else if (xObject.asObjectName->pDescription)
         // [WARNING] "Discarding duplicate description tooltip for '%s' script object '%s' with ID:%u"
         pError = generateDualWarning(HERE(IDS_DESCRIPTION_OBJECT_DUPLICATE), szGroupName = generateScriptObjectGroupString(identifyObjectNameGroupFromGameString(pSourceString)), xObject.asObjectName->szText, pSourceString->iID);
         
      else
      {
         /// Population description
         szPopulatedSource = generatePopulatedDescriptionSource(pSourceString->szText, pSourceString->iCount, pVariablesTree, NULL, pSourceString->iPageID, pSourceString->iID, pOperationData->pErrorQueue);

         /// Generate description from source text
         if (bResult = generateRichTextFromSourceText(szPopulatedSource, lstrlen(szPopulatedSource), pDescription, RTT_RICH_TEXT, ST_DISPLAY, pOperationData->pErrorQueue))
            // [SUCCESS] Store description
            xObject.asObjectName->pDescription = pDescription;
         else
            // [WARNING] "Formatting error detected in description tooltip for '%s' script object '%s' with ID:%u"
            pError = generateDualWarning(HERE(IDS_DESCRIPTION_OBJECT_FORMAT_ERROR), szGroupName = generateScriptObjectGroupString(identifyObjectNameGroupFromGameString(pSourceString)), xObject.asObjectName->szText, pSourceString->iID);
            
         // Cleanup
         utilDeleteString(szPopulatedSource);
      }
      // Cleanup
      utilSafeDeleteString(szGroupName);
      break;

   /// [UNSUPPORTED] Error
   default:
      ASSERT(FALSE);
      break;
   }
   
   // [CHECK] Formatting errors / Object not found
   if (!bResult OR pError)
   {
      // [OBJECT-NOT-FOUND] Add to operation error queue
      if (pError)
         pushErrorQueue(pOperationData->pErrorQueue, pError);

      // Cleanup
      if (pDescription)
         deleteRichText(pDescription);
   }
   else
      /// [SUCCESS] Update output count
      pOperationData->xOutput++;
}


