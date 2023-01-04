//
// Script Translation (Base).cpp : Provides the helper functions for translating scripts
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

/// Function name  : createCommandComponent
// Description     : Create a new CommandComponent object for parsing command syntax
// 
// CONST COMMAND*  pCommand : [in] COMMAND containing the syntax to parse
// 
// Return Value   : New CommandComponent object, you are responsible for destroying it
// 
COMMAND_COMPONENT*  createCommandComponent(CONST COMMAND*  pCommand)
{
   COMMAND_COMPONENT*  pCommandComponent;    // New command component being created

   // Create new object
   pCommandComponent = utilCreateEmptyObject(COMMAND_COMPONENT);

   // Copy syntax for tokenising
   pCommandComponent->szSource   = utilDuplicateString(pCommand->pSyntax->szSyntax, lstrlen(pCommand->pSyntax->szSyntax));
   pCommandComponent->szPosition = pCommandComponent->szSource;

   // Set properties
   pCommandComponent->iParameterCount = pCommand->pParameterArray->iCount;
   pCommandComponent->iFlags          = pCommand->iFlags;
   //pCommandComponent->iIndex          = -1;

   // Return new object
   return pCommandComponent;
}


/// Function name  : deleteCommandComponent
// Description     : Destroy a CommandComponent object
// 
// COMMAND_COMPONENT*  pCommandComponent   : [in] CommandComponent object to destroy
// 
VOID  deleteCommandComponent(COMMAND_COMPONENT*  pCommandComponent)
{
   // Delete syntax string
   utilDeleteString(pCommandComponent->szSource);

   // Delete current component, if any
   utilSafeDeleteString(pCommandComponent->szText);

   // Delete calling object
   utilDeleteObject(pCommandComponent);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : calculateGameVersionFromEngineVersion
// Description     : Converts an EngineVersion into a GameVersion
// 
// CONST UINT  iEngineVersion   : [in] EngineVersion
// 
// Return Value   : GameVersion
// 
GAME_VERSION  calculateGameVersionFromEngineVersion(CONST UINT  iEngineVersion)
{
   GAME_VERSION  eOutput;  // Operation result

   // Examine engine version
   if (iEngineVersion <= 24)
      eOutput = GV_THREAT;

   else if (iEngineVersion <= 39)
      eOutput = GV_REUNION;

   else if (iEngineVersion <= 44)
      eOutput = GV_TERRAN_CONFLICT;

   else
      eOutput = GV_ALBION_PRELUDE;

   // Return result
   return eOutput;
}


/// Function name  : calculateEngineVersionFromGameVersion
// Description     : Converts a GameVersion into an EngineVersion 
// 
// CONST GAME_VERSION  eGameVersion : [in] Game version
// 
// Return Value   : EngineVersion
// 
UINT  calculateEngineVersionFromGameVersion(CONST GAME_VERSION  eGameVersion)
{
   UINT  iOutput;  // Operation result

   // Examine Game version
   switch (eGameVersion)
   {
   case GV_THREAT:           iOutput = 24;   break;
   case GV_REUNION:          iOutput = 32;   break;
   case GV_TERRAN_CONFLICT:  iOutput = 44;   break;
   case GV_ALBION_PRELUDE:   iOutput = 50;   break;
   }

   // Return EngineVersion
   return iOutput;
}



/// Function name  : convertSectorCoordinatesToStringID
// Description     : Convert zero-based sector co-ordinates into the string ID of the sector name
// 
// CONST POINT*  ptSector : [in] Zero-based sector co-ordinates
// 
// Return Value   : String ID of sector name  (which uses ONE-based co-ordinates)
// 
UINT   convertSectorCoordinatesToStringID(CONST POINTS*  ptSector)
{
   TCHAR  szSector[16];    // SectorID as a string

   // Assemble one-based co-ordinates into a string, prefaced with '102'
   StringCchPrintf(szSector, 16, TEXT("102%02d%02d"), ptSector->x + 1, ptSector->y + 1);

   // Convert to integer
   return utilConvertStringToInteger(szSector);
}


/// Function name  : convertStringIDToSectorCoordinates
// Description     : Converts a sector string ID into zero-based co-ordinates
// 
// CONST UINT  iStringID : [in]     String ID of sector (containing ONE-based sector co-ordinates)
// POINT*      pOutput   : [in/out] Point containing ZERO-based sector co-ordinates
// 
// Return Value   : TRUE if successful, FALSE if the sector StringID was invalid
// 
BOOL   convertStringIDToSectorCoordinates(CONST UINT  iStringID, POINTS*  pOutput)
{
   TCHAR   szSectorID[8],     // SectorID as a string
           szCoordinate[3];   // X or Y co-ordinate as a string

   // Prepare
   utilZeroString(szCoordinate, 3);
   
   // Convert to string
   StringCchPrintf(szSectorID, 8, TEXT("%d"), iStringID);

   // [CHECK] Ensure string is a sector ID.  ie. 102xxxx
   if (!utilCompareStringN(szSectorID, "102", 3) OR lstrlen(szSectorID) != 7)
      return FALSE;

   // Extract X co-ordinate
   StringCchCopyN(szCoordinate, 3, &szSectorID[3], 2);
   pOutput->x = utilConvertStringToInteger(szCoordinate) - 1;     // Convert from 1-based to 0-based

   // Extract y co-ordinate
   StringCchCopyN(szCoordinate, 3, &szSectorID[5], 2);
   pOutput->y = utilConvertStringToInteger(szCoordinate) - 1;     // Convert from 1-based to 0-based

   // Return TRUE
   return TRUE;
}



/// Function name  : identifyDataTypeString
// Description     : Retrieve the string representation of a data type
// 
// CONST DATA_TYPE  eType : [in] DataType to retrieve string for
// 
// Return Value   : Constant string, this must not be deleted.
// 
CONST TCHAR*  identifyDataTypeString(CONST DATA_TYPE  eType)
{
   GAME_STRING*  pOutput;
   
   // Lookup data type string
   return findGameStringByID(eType, GPI_DATA_TYPES, pOutput) ? pOutput->szText : TEXT("Unknown");
}


/// Function name  : identifyGamePageIDFromDataType
// Description     : Gets ID of the table containing the game strings for a particular data type
// 
// CONST DATA_TYPE  eDataType : Data type with an associated game string table
// 
// Return type : table ID
BearScriptAPI
GAME_PAGE_ID    identifyGamePageIDFromDataType(CONST DATA_TYPE  eDataType)
{   
   GAME_PAGE_ID  eOutput;

   switch (eDataType)
   {
   case DT_CONSTANT:          eOutput = GPI_CONSTANTS;             break;
   case DT_SHIP:
   case DT_STATION:
   case DT_WARE:              eOutput = GPI_SHIPS_STATIONS_WARES;  break;
   case DT_SECTOR:            eOutput = GPI_SECTORS;               break;
   case DT_RACE:              eOutput = GPI_RACES;                 break;
   case DT_STATIONSERIAL:     eOutput = GPI_STATION_SERIALS;       break;
   case DT_OBJECTCLASS:       eOutput = GPI_OBJECT_CLASSES;        break;
   case DT_TRANSPORTCLASS:    eOutput = GPI_TRANSPORT_CLASSES;     break;
   case DT_RELATION:          eOutput = GPI_RELATIONS;             break;
   case DT_OPERATOR:          eOutput = GPI_OPERATORS;             break;
   case DT_OBJECTCOMMAND:     eOutput = GPI_OBJECT_COMMANDS;       break;
   case DT_FLIGHTRETURN:      eOutput = GPI_FLIGHT_RETURNS;        break;
   case DT_DATATYPE:          eOutput = GPI_DATA_TYPES;            break;
   case DT_SCRIPTDEF:         eOutput = GPI_PARAMETER_TYPES;    break;
   case DT_WINGCOMMAND:       eOutput = GPI_WING_COMMANDS;         break;

   // [UNSUPPORTED] No lookup table for the following types: DT_NULL, DT_UNKNOWN, DT_VARIABLE, DT_INTEGER, DT_STRING, DT_EXPRESSION, DT_OBJECT, DT_ARRAY, DT_QUEST
   default:                   eOutput = GPI_INVALID;               break;
   }

   return eOutput;
}


/// Function name  : identifyParameterSyntaxType
// Description     : Determine whether a parameter of a given syntax will be stored as one node or two
// 
// CONST PARAMETER_SYNTAX  eSyntax   : [in] Syntax to test
// 
// Return Value   : SINGLE_PARAMETER for parameters without a type node, PARAMETER_TUPLE for parameters stored as tuples
// 
PARAMETER_NODE_TYPE  identifyParameterSyntaxType(CONST PARAMETER_SYNTAX  eSyntax)
{
   switch (eSyntax)
   {
   /// [RETURN-OBJECT and SPECIAL CASES] Parameter value stored in a single node
   case PS_COMMENT:        // Comments are defined by a single node
   case PS_SCRIPT_NAME:    // Script names are defined by a single node
   case PS_VARIABLE:       // Variable ID used only by inc/dec commands
   case PS_LABEL_NUMBER:   // Label numbers are defined by a single node
   case PS_LABEL_NAME:     // Label names are defined by a single node
   case PS_RETURN_OBJECT:
   case PS_RETURN_OBJECT_IF:
   case PS_RETURN_OBJECT_IF_START:
   case PS_INTERRUPT_RETURN_OBJECT_IF:
      return  PNT_SINGLE_PARAMETER;
   }

   /// [NORMAL] Defined by a node tuple
   return  PNT_PARAMETER_TUPLE;
}


/// Function name  : isVariableMacroIterator
// Description     : Checks whether a variable name is an X-Studio hidden iterator
// 
// const TCHAR*  szVariable : [in] Variable name
// 
// Return Value   : TRUE/FALSE
// 
BOOL  isVariableMacroIterator(const TCHAR*  szVariable)
{
   return utilCompareStringN(szVariable, "XS.Iterator", 11);
}


/// Function name  : verifyDataType
// Description     : Ensure a DataType enumeration (read from a script) is within it's property limits. 
// 
// CONST DATA_TYPE  eType : [in] DataType to verify
// 
// Return Value   : TRUE or FALSE
// 
BOOL  verifyDataType(CONST DATA_TYPE  eType)
{
   // Check for the normal data types
   if ((INT)eType >= DT_NULL  AND  (INT)eType <= DT_PASSENGER)
      return TRUE;   // [FOUND] Return TRUE

   // Check for the live data types
   else switch (eType)
   {
      case DT_LIVE_CONSTANT:
      case DT_LIVE_VARIABLE:
      case DT_LIVE_OBJECT:
      case DT_LIVE_SECTOR:
      case DT_LIVE_SHIP:
      case DT_LIVE_STATION:
      case DT_LIVE_WING:
      case DT_UNARY_OPERATOR:
         return TRUE;    // [FOUND] Return TRUE
   }

   return FALSE; // [NOT FOUND] Return FALSE
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : calculateParameterSyntaxByIndex
// Description     : Determine the appropriate parameter syntax for any command
// 
// CONST COMMAND*     pCommand        : [in]  Command to determine parameter syntax for
// CONST UINT         iParameterIndex : [in]  Zero-based index of the parameter to retrieve syntax for
// PARAMETER_SYNTAX  &eOutput         : [out] Parameter syntax
// ERROR_STACK*      &pError          : [out] New error message if any, otherwise NULL. You are responsible for destroying it
// 
// Return Value   : TRUE if successful, FALSE if there was an error
// 
BOOL  calculateParameterSyntaxByIndex(CONST COMMAND*  pCommand, CONST UINT  iParameterIndex, PARAMETER_SYNTAX  &eOutput, ERROR_STACK*  &pError)
{
   // Prepare
   eOutput = PS_UNDETERMINED;
   pError  = NULL;

   /// [EXPRESSION] After the return object all syntax should be 'expression'
   if (isCommandID(pCommand, CMD_EXPRESSION) AND  iParameterIndex > 0)
      eOutput = PS_EXPRESSION;

   /// [SCRIPT CALL] Use the 'PARAMETER' syntax to indicate script-call arguments
   else if (isCommandID(pCommand, CMD_CALL_SCRIPT_VAR_ARGS) AND iParameterIndex > 2)
      eOutput = PS_PARAMETER;

   /// [DELAYED COMMAND] Return FALSE, thereby aborting parsing without causing an error
   else if (isCommandID(pCommand, CMD_DELAYED_COMMAND) AND iParameterIndex == 6)
      return FALSE; 

   /// [SET WING COMMAND] Return FALSE, thereby aborting parsing without causing an error
   else if (isCommandID(pCommand, CMD_SET_WING_COMMAND) AND iParameterIndex == 4)
      return FALSE; 

   /// [GET OBJECT NAME ARRAY] Return FALSE, thereby aborting parsing without causing an error
   else if (isCommandID(pCommand, CMD_GET_OBJECT_NAME_ARRAY) AND iParameterIndex == 2)
      // Return FALSE, thereby aborting parsing without causing an error
      return FALSE; 

   /// [DEFAULT] Lookup the appropriate syntax
   else if (!findParameterSyntaxByPhysicalIndex(pCommand->pSyntax, iParameterIndex, eOutput))
      // [ERROR] "Cannot locate the syntax entry for parameter %u of %u"
      pError = generateDualError(HERE(IDS_SCRIPT_PARAMETER_SYNTAX_MISSING), iParameterIndex + 1, pCommand->pSyntax->iParameterCount);
   
   // Return TRUE if there were no errors
   return  (pError == NULL);
}


/// Function name  : convertLabelNumberParameterToLabel
// Description     : Convert a LabelNumber PARAMETER within a COMMAND into a corresponding LabelName PARAMETER
// 
// CONST COMMAND_NODE*  pCommandNode : [in]  CommandNode containing the command that uses a label number parameter
// COMMAND*             pCommand     : [in]  COMMAND containing a LabelNumber PARAMETER
// ERROR_STACK*        &pError       : [out] New error message if any, otherwise NULL
// 
// Return Value   : TRUE if successful, FALSE if there were errors
// 
BOOL  convertLabelNumberParameterToLabel(CONST COMMAND_NODE*  pCommandNode, COMMAND*  pCommand, ERROR_STACK*  &pError)
{
   XML_TREE_NODE *pIterator,           // CommandNode iterator for iterating to the node containing the label name
                 *pLabelNameNode;      // CommandNode containing the desired label name
   PARAMETER     *pLabelNumber,        // Input COMMAND's label number PARAMETER
                 *pLabelName;          // New label name PARAMETER that will replace the existing label number PARAMETER

   // [CHECK] Can only be run on GoSub and Goto commands
   ASSERT(pCommand->iID == CMD_GOTO_SUB OR pCommand->iID == CMD_GOTO_LABEL);
   // [CHECK] Command should have only one parameter
   ASSERT(pCommand->pParameterArray->iCount == 1);

   /// Extract the label number parameter
   if (!findParameterInCommandByIndex(pCommand, PT_DEFAULT, 0, pLabelNumber)                       // [CHECK] Ensure PARAMETER is present
       OR pLabelNumber->eSyntax != PS_LABEL_NUMBER                                                 // [CHECK] Ensure PARAMETER is a PS_LABEL_NUMBER
       OR (!isCommandType(pCommand, CT_CMD_COMMENT) AND pLabelNumber->eType != DT_INTEGER))        // [CHECK] Ensure non-command comment label numbers are infact numbers
   {
      // [ERROR] "The destination of the jump command on line %u cannot be determined, it may be missing or invalid"
      pError = generateDualError(HERE(IDS_SCRIPT_LABEL_NUMBER_INVALID), pCommandNode->pNode->iLineNumber);
   }
   /// [COMMAND COMMENTS] These are already stored as label names, just update the syntax to reflect this
   else if (isCommandType(pCommand, CT_CMD_COMMENT) AND pLabelNumber->eType == DT_STRING)
      pLabelNumber->eSyntax = PS_LABEL_NAME;

   else
   {
      /// [COMMAND PRECEEDS LABEL] Search forwards for the TreeNode identified by the label number
      if (pCommandNode->pNode->iIndex < (UINT)pLabelNumber->iValue)
         for (findNextXMLTreeNode(pCommandNode->pNode, XNR_SIBLING, pIterator); pIterator AND pIterator->iIndex != pLabelNumber->iValue; findNextXMLTreeNode(pIterator, XNR_SIBLING, pIterator))
            continue;
      /// [COMMAND FOLLOWS LABEL] Search backwards for the TreeNode identified by the label number
      else
         for (findLastXMLTreeNode(pCommandNode->pNode, XNR_SIBLING, pIterator); pIterator AND pIterator->iIndex != pLabelNumber->iValue; findLastXMLTreeNode(pIterator, XNR_SIBLING, pIterator))
            continue;
      
      // [NOT FOUND] Return NULL
      if (!pIterator)
      {
         // [ERROR] "The Jump command on line %u identifies a destination Label on line %u which cannot be found"
         pError = generateDualError(HERE(IDS_SCRIPT_LABEL_NUMBER_NOT_FOUND), pCommandNode->pNode->iLineNumber, pLabelNumber->iValue);
      }
      // Extract the parameter node containing the label name
      else if (!findXMLTreeNodeByIndex(pIterator, SSTI_FIRST_PARAMETER, pLabelNameNode))
      {
         // [ERROR] "The name of the label command on line %u cannot be determined, it may be missing or invalid"
         pError = generateDualError(HERE(IDS_SCRIPT_LABEL_COMMAND_INVALID), pIterator->iLineNumber);
      }
      /// Translate into a PARAMETER
      else if (!translateParameterNode(pLabelNameNode, PS_LABEL_NAME, pLabelName, pError))
      {
         // [ERROR] "Cannot translate the label name of the Label command on line %u into a new PARAMETER object"
         pError = generateDualError(HERE(IDS_SCRIPT_LABEL_TRANSLATION_FAILED), pLabelNameNode->iLineNumber);
      }
      /// Replace LabelNumber PARAMETER with the LabelName PARAMETER
      else
      {
         // Lookup Parameter containing the LabelNumber
         findParameterInCommandByIndex(pCommand, PT_DEFAULT, 0, pLabelNumber);
         // Copy it's LineNumber to the Label NAME parameter
         pLabelName->iLineNumber = pLabelNumber->iLineNumber;
         insertParameterIntoArray(pCommand->pParameterArray, 0, pLabelName);
         // Delete the severed LabelNumber parameter
         deleteParameter(pLabelNumber);
      }
   }
   
   // Return TRUE if there were no errors
   return (pError == NULL);
}


/// Function name  : findGameStringDependency
// Description     : Retrieves the first available GameString dependency from a Command
// 
// CONST COMMAND*  pCommand : [in]  COMMAND containing a command that references a game string
// GAME_STRING*   &pOutput  : [out] GameString if found, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BearScriptAPI
BOOL  findGameStringDependency(CONST COMMAND*  pCommand, GAME_STRING*  &pOutput)
{
   GAME_STRING_REF *pReference;         // Current Ref
   LIST            *pReferenceList;     // COMMAND's GameString references
   INT              iPageID,            // Current Ref PageID
                    iStringID;          // Current Ref StringID

   // Prepare
   pOutput = NULL;

   /// Lookup the Parameter Indicies containing PageID/StringID
   if (pReferenceList = generateParameterGameStringRefs(pCommand))
   {
      // Iterate through the references (Abort on errors)
      for (UINT  iIndex = 0; findListObjectByIndex(pReferenceList, iIndex, (LPARAM&)pReference); iIndex++)
      {
         // Locate the PARAMETERS
         if (!findIntegerParameterInCommandByIndex(pCommand, pReference->iPageParameterIndex, iPageID) OR
             !findIntegerParameterInCommandByIndex(pCommand, pReference->iStringParameterIndex, iStringID))
             continue;

         // [CHECK] Abort if both are zero, this often happens in commands with multiple references
         if (iPageID == 0 AND iStringID == 0)
            continue;

         /// Return the first GameString found
         if (findGameStringByID(iStringID, iPageID, pOutput))
            break;
      }

      // Cleanup
      deleteList(pReferenceList);
   }

   // Return TRUE if found
   return (pOutput != NULL);
}


/// Function name  : findNextCommandComponent
// Description     : Returns the next component of a COMMAND's syntax string
// 
// COMMAND_COMPONENT*  pCommandComponent : [in] CommandComponent parsing object
// 
// Return Value   : TRUE if a component was found, FALSE otherwise
// 
BOOL  findNextCommandComponent(COMMAND_COMPONENT*  pCommandComponent)
{
   TCHAR*  szTextEnd;   // Marks the end of the current CommandText result

   /// [EXPRESSION] Return the next parameter index (if any)
   if (pCommandComponent->iFlags INCLUDES CT_EXPRESSION)
   {
      // [CHECK] Return FALSE when index indicates no more parameters remain
      if (pCommandComponent->iIndex == pCommandComponent->iParameterCount - 1)
         return FALSE;

      switch (pCommandComponent->iComponent++)
      {
      // [RETURN-OBJECT] Return parameter index 0
      case 0:
         pCommandComponent->eType  = CCT_PARAMETER_INDEX;
         pCommandComponent->iIndex = 0;
         break;

      // [EQUALS SIGN] Return ' =' instead of a parameter
      case 1:
         utilReplaceString(pCommandComponent->szText, TEXT(" "));
         pCommandComponent->eType = CCT_COMMAND_TEXT;
         break;

      // [PARAMETER] Return next parameter index
      default:
         pCommandComponent->eType = CCT_PARAMETER_INDEX;
         pCommandComponent->iIndex++;
         break;
      }
   }
   /// [START OF SCRIPT-CALL ARGUMENTS] Return remaining Script Arguments
   else if (!pCommandComponent->szPosition AND (pCommandComponent->iFlags INCLUDES CT_SCRIPTCALL))
   {
      // [CHECK] Return FALSE when no script arguments remain
      if (pCommandComponent->iComponent == pCommandComponent->iParameterCount)   // Component is one-based in effect
         return FALSE;

      // Set type and index
      pCommandComponent->eType  = CCT_SCRIPT_ARGUMENT;
      pCommandComponent->iIndex = pCommandComponent->iComponent;  // Index of first argument should be 3

      // Update component count
      pCommandComponent->iComponent++;
   }
   /// [NO MORE INSERTION MARKERS] Return FALSE 
   else if (!pCommandComponent->szPosition)
      return FALSE;  // Return FALSE when no more string/arguments are remaining
   
   // Examine first character of the syntax iterator
   else switch(pCommandComponent->szPosition[0])
   {
   /// [INSERTION MARKER] Return the specified parameter index
   case '$':
      // Extract index and move position iterator
      pCommandComponent->iIndex     = utilConvertCharToInteger(pCommandComponent->szPosition[1]);
      pCommandComponent->szPosition = utilFindCharacterNotInSet(pCommandComponent->szPosition, "$0123456789aoxyzªº°¹²³"); // (pCommandComponent->szPosition[2] ? &pCommandComponent->szPosition[2] : NULL);

      // Set component type
      pCommandComponent->eType = CCT_PARAMETER_INDEX;
      pCommandComponent->iComponent++;
      break;

   /// [COMMAND TEXT] Return the current section of command text
   default:
      // Delete previous component
      utilSafeDeleteString(pCommandComponent->szText);

      // [NOT LAST COMPONENT] Return the portion of the syntax string preceeding the next insertion marker
      if (szTextEnd = utilFindCharacter(pCommandComponent->szPosition, '$'))
      {
         pCommandComponent->szText = utilDuplicateString(pCommandComponent->szPosition, szTextEnd - pCommandComponent->szPosition);
         pCommandComponent->szPosition = szTextEnd;
      }
      // [LAST COMPONENT] Return the remaining syntax string
      else
      {
         pCommandComponent->szText = utilReplaceString(pCommandComponent->szText, pCommandComponent->szPosition);
         pCommandComponent->szPosition = NULL;
      }

      // Set component type
      pCommandComponent->eType = CCT_COMMAND_TEXT;
      break;
   }
   
   // Update current component and return TRUE
   return TRUE;
}


/// Function name  : parseScriptPropertiesFromBuffer
// Description     : Manually parses the name, description and version properties from a buffer containing the first
//                     1024 bytes of MSCI script XML.  This is a quick and dirty alternative to fully parsing the
//                     entire XML tree for data that's easily available in the first six or seven lines.
// 
// SCRIPT_FILE*  pScriptFile  : [in] ScriptFile containing MSCI script XML 
// 
// Return Value   : TRUE if all name, decription and version were found, otherwise FALSE
// 
BOOL   parseScriptPropertiesFromBuffer(SCRIPT_FILE*  pScriptFile)
{
   TCHAR  *szStartMarker,        // Position marker for the start of the property being processed
          *szEndMarker;          // Position marker for the end of the property being processed
   BOOL    bFoundName,           // 'Name' property search result
           bFoundDescription,    // 'Description' property search result
           bFoundVersion,        // 'Version' property search result
           bFoundGameVersion;    // 'EngineVersion' property search result

   // Prepare
   bFoundName = bFoundDescription = bFoundVersion = FALSE;

   /// [NAME] Search for <name> and </name> markers
   if ((szStartMarker = utilFindSubStringI(pScriptFile->szInputBuffer, "<name>")) AND (szEndMarker = utilFindSubStringI(szStartMarker, "</name>")))
   {
      // Extract whatever's between them
      szStartMarker += lstrlenA("<name>");
      pScriptFile->szScriptname = utilDuplicateString(szStartMarker, szEndMarker - szStartMarker);
      // Set result
      bFoundName = TRUE;
   }

   /// [DESCRIPTION] Search for <description> and </description> markers
   if ((szStartMarker = utilFindSubStringI(pScriptFile->szInputBuffer, "<description>")) AND (szEndMarker = utilFindSubStringI(szStartMarker, "</description>")))
   {
      // Extract whatever's between them
      szStartMarker += lstrlenA("<description>");
      pScriptFile->szDescription = utilDuplicateString(szStartMarker, szEndMarker - szStartMarker);
      // Set result
      bFoundDescription = TRUE;
   }

   /// [ENGINE-VERSION] Search for the <engineversion> marker
   if (szStartMarker = utilFindSubStringI(pScriptFile->szInputBuffer, "<engineversion>"))
   {
      // Extract and convert whatever's between them
      szStartMarker += lstrlenA("<engineversion>");
      pScriptFile->eGameVersion = calculateGameVersionFromEngineVersion(utilConvertStringToInteger(szStartMarker));
      // Set result
      bFoundGameVersion = TRUE;
   }

   /// [VERSION] Search for the <version> marker
   if (szStartMarker = utilFindSubStringI(pScriptFile->szInputBuffer, "<version>"))
   {
      // Extract and convert whatever's between them
      szStartMarker += lstrlenA("<version>");
      pScriptFile->iVersion = utilConvertStringToInteger(szStartMarker);
      // Set result
      bFoundVersion = TRUE;
   }

   // Return TRUE if all three were found
   return (bFoundName AND bFoundDescription AND bFoundVersion AND bFoundGameVersion);
}


/// Function name  : performScriptValueConversion
// Description     : Converts a DataType from internal to external (or vice versa)
// 
// CONST PARAMETER*         pParameter  : [in][optional] Parameter containing Internal DataType to convert to external
// CONST DATA_TYPE          eInput      : [in][optional] External DataType to convert to internal
// DATA_TYPE&               eOutput     : [out] Output datatype as internal or external
// CONST SCRIPT_VALUE_TYPE  eInputType  : [in]  Whether the input datatype is internal or external
// CONST SCRIPT_VALUE_TYPE  eOutputType : [in]  Whether the output datatype should be internal or external
// ERROR_STACK*            &pError      : [out] New error if any, otherwise NULL
// 
// Return Value   : TRUE if successful, FALSE otherwise
// 
BOOL  performScriptTypeConversion(CONST PARAMETER*  pParameter, CONST DATA_TYPE  eInput, DATA_TYPE&  eOutput, CONST SCRIPT_VALUE_TYPE  eInputType, CONST SCRIPT_VALUE_TYPE  eOutputType, ERROR_STACK*  &pError)
{
   // [CHECK] Ensure types are different
   ASSERT(eInputType != eOutputType);
   
   // Prepare
   pError  = NULL;
   eOutput = DT_NULL;

   switch (eInputType)
   {
   /// [STRING ID -> EXTERNAL] Convert some DataTypes to their 'live' equivilents
   case SVT_INTERNAL:
      switch (pParameter->eType)
      {
         // [VARIABLE / CONSTANT] Change the type to LIVE VALUE
         case DT_VARIABLE:  eOutput = DT_LIVE_VARIABLE;   break;
         case DT_CONSTANT:  eOutput = DT_LIVE_CONSTANT;   break;

         // [SECTOR] Change the type to LIVE SECTOR
         case DT_SECTOR:    eOutput = DT_LIVE_SECTOR;     break;

         // [OPERATOR] Change the type to UNARY OPERATOR (if appropriate)
         /*case DT_OPERATOR:
            switch (pParameter->iValue)
            {
            case OT_BITWISE_NOT:
            case OT_LOGICAL_NOT:
            case OT_MINUS: 
               eOutput = DT_UNARY_OPERATOR;  
               break;
            }
            break;*/

         // [OBJECT/SHIP/STATION] Change to LIVE_OBJECT/LIVE_SHIP/LIVE_STATION
         case DT_OBJECT:
         case DT_SHIP:
         case DT_STATION:
            TODO("Add new LiveObject, LiveShip and LiveStation types");
            break;

         // [DEFAULT] Do not alter datatype
         default:   
            eOutput = eInput;   
            break;
      }
      break;

   /// [SCRIPT VALUE -> STRING ID] Convert external types to 'basic' types
   case SVT_EXTERNAL:
      //// Check input type is valid in the first place
      //if (!verifyDataType(eInput))
      //   // [ERROR] "Cannot create a valid DataType from the external value 0x%04X:%04X"
      //   pError = generateDualError(HERE(IDS_TRANSLATION_DATATYPE_INVALID), HIWORD(eInput), LOWORD(eInput));
      //
      //// Convert 'live' DataTypes to their basic equivilents
      //else switch (eInput)
      //{
      //case DT_LIVE_CONSTANT:   eOutput = DT_CONSTANT;   break;
      //case DT_LIVE_VARIABLE:   eOutput = DT_VARIABLE;   break;
      //case DT_LIVE_OBJECT:     eOutput = DT_OBJECT;     break;
      //case DT_LIVE_SECTOR:     eOutput = DT_SECTOR;     break;
      //case DT_LIVE_SHIP:       eOutput = DT_SHIP;       break;
      //case DT_LIVE_STATION:    eOutput = DT_STATION;    break;
      //case DT_LIVE_WING:       eOutput = DT_WING;       break;
      //case DT_UNARY_OPERATOR:  eOutput = DT_OPERATOR;   break;
      //default:                 eOutput = eInput;        break;
      //}

      // Remove any encoding -- still unknown what the encoding does
      eOutput = (DATA_TYPE)LOWORD(eInput);

      // [CHECK] Ensure result is valid
      if (!verifyDataType(eOutput))
         // [ERROR] "Cannot create a valid DataType from the external value 0x%04X:%04X"
         pError = generateDualError(HERE(IDS_TRANSLATION_DATATYPE_INVALID), HIWORD(eInput), LOWORD(eInput));
      break;
   }

   // Return TRUE if there were no errors
   return (pError == NULL);
}


/// Function name  : performScriptValueConversion
// Description     : Converts a value from a Script Value to a String ID (and vice versa)
// 
// CONST PARAMETER*         pInput      : [in]     Parameter containing the value to convert (also indicates it's type)
// INT                     &iOutput     : [out]    Resultant value
// BOOL                    &bLiveData   : [in/out] This will be set to TRUE if the value indicates the script contains 'live data'
// CONST SCRIPT_VALUE_TYPE  eInputType  : [in]     Current value type
// CONST SCRIPT_VALUE_TYPE  eOutputType : [in]     Desired value type
// ERROR_STACK*            &pError      : [out]    New error if any, otherwise NULL
// 
// Return Value   : TRUE if successful, FALSE otherwise
// 
BOOL  performScriptValueConversion(SCRIPT_FILE*  pScriptFile, CONST PARAMETER*  pInput, INT&  iOutput, CONST SCRIPT_VALUE_TYPE  eInputType, CONST SCRIPT_VALUE_TYPE  eOutputType, ERROR_STACK*  &pError)
{
   GAME_STRING*  pParameterType;         // GameString associated with the DataType of the input parameter, used for error reporting only
   POINTS        ptSector;               // Converted sector co-ordinates

   // Prepare
   pError  = NULL;
   iOutput = NULL;

   // [CHECK] Ensure types are different
   ASSERT(eInputType != eOutputType);

   // Examine input type
   switch (eInputType)
   {
   /// [INTERNAL -> EXTERNAL]
   case SVT_INTERNAL:
      switch (pInput->eType)
      {
      /// [OPERATOR] Add the 'unary operator' flag to the three unary operators
      case DT_OPERATOR:
         switch (pInput->iValue)
         {
         case OT_MINUS: 
         case OT_BITWISE_NOT:
         case OT_LOGICAL_NOT:  iOutput = pInput->iValue WITH UGC_UNARY_OPERATOR;    break;
         default:              iOutput = pInput->iValue;                            break;
         }
         break;

      /// [STATION SERIAL] -- Subtract 100 from the string ID
      case DT_STATIONSERIAL:
         iOutput = pInput->iValue - 100;
         break;

      /// [RELATION] -- Map {1102423,1102424,1102422} into {-1,0,1}
      case DT_RELATION:
         switch (pInput->iValue)
         {
         case 1102423:   iOutput = RR_FOE;      break;   
         case 1102424:   iOutput = RR_NEUTRAL;  break;   
         case 1102422:   iOutput = RR_FRIEND;   break;   

         default: ASSERT(FALSE);  // [ERROR] Should never happen
         }
         break;

      /// [DATATYPE] -- Add unknown flag
      case DT_DATATYPE:
         // Change certain values
         switch (iOutput = pInput->iValue)
         {
         case DT_CONSTANT:   iOutput = DT_LIVE_CONSTANT;   break;
         case DT_VARIABLE:   iOutput = DT_LIVE_VARIABLE;   break;
         case DT_SECTOR:     iOutput = DT_LIVE_SECTOR;     break;

         case DT_OBJECT:     iOutput = DT_LIVE_OBJECT;     break;
         case DT_SHIP:       iOutput = DT_LIVE_SHIP;       break;
         case DT_STATION:    iOutput = DT_LIVE_STATION;    break;
         case DT_WING:       iOutput = DT_LIVE_WING;       break;
         }
         break;

      /// [VARIOUS] -- No conversion is required
      case DT_NULL:
      case DT_RACE:
      case DT_WARE:
      case DT_CONSTANT:
      case DT_INTEGER:
      case DT_STRING:
      case DT_VARIABLE:
      case DT_SCRIPTDEF:
      case DT_FLIGHTRETURN:
      case DT_TRANSPORTCLASS:
      case DT_OBJECTCLASS:
      case DT_OBJECTCOMMAND:
      case DT_WINGCOMMAND:
         iOutput = pInput->iValue;
         break;

      /// [SECTOR] Convert ID into co-ordinates.  Set 'LiveData' flag
      case DT_SECTOR:
         // Convert GameString ID into co-ordinates and pack into return value
         convertStringIDToSectorCoordinates(pInput->iValue, &ptSector);
         iOutput = MAKE_LONG(ptSector.x, ptSector.y);
   
         // [LIVE DATA] Indicates a sector has been specified literally   [VALIDATION_FIX]
         if (pScriptFile)
            pScriptFile->bLiveData = TRUE;
         break;

      /// [ARRAY/EXPRESSION/QUEST/UNKNOWN/PASSENGER] -- Unsupported
      default: 
         ASSERT(FALSE); // [ERROR] - should never happen
      }
      break;

   /// [EXTERNAL -> INTERNAL] Convert external value into the string ID associated with it
   case SVT_EXTERNAL:
      switch (pInput->eType)
      {
      /// [SECTOR] -- Unpack co-ordinates and convert into a number with the format 102xxyy
      case DT_SECTOR:
         // Convert packed sector co-ordinates into a GameString ID
         utilSetPoint(&ptSector, LOWORD(pInput->iValue), HIWORD(pInput->iValue));
         iOutput = convertSectorCoordinatesToStringID(&ptSector);
         break;

      /// [STATION SERIAL] -- Add 100 to the script value
      case DT_STATIONSERIAL:
         iOutput = pInput->iValue + 100;
         break;

      /// [RELATION] -- Map {-1,0,1} into {1102423,1102424,1102422}
      case DT_RELATION:
         switch (pInput->iValue)
         {
         case RR_FOE:      iOutput = 1102423;   break;   // [FOE]
         case RR_NEUTRAL:  iOutput = 1102424;   break;   // [NEUTRAL]
         case RR_FRIEND:   iOutput = 1102422;   break;   // [FRIEND]
         default:
            // [ERROR] "Cannot convert the unrecognised race relation ID '%d' into a game string ID"
            pError = generateDualError(HERE(IDS_TRANSLATION_RELATION_INVALID), pInput->iValue);
         }
         break;

      /// [OPERATOR] -- Remove the UNARY OPERATOR bit flag, if present
      case DT_OPERATOR:
         iOutput = LOWORD(pInput->iValue);
         break;

      /// [DATATYPE] -- Remove any encoded/object-data modifiers to create a 'basic' DataType
      case DT_DATATYPE:
         performScriptTypeConversion(pInput, (DATA_TYPE)pInput->iValue, (DATA_TYPE&)iOutput, SVT_EXTERNAL, SVT_INTERNAL, pError);
         break;

      /// [CONSTANT] -- Remove the object-data modifier if present
      case DT_CONSTANT:
         iOutput = LOWORD(pInput->iValue);
         break;

      /// [VARIOUS] -- No conversion is required
      case DT_RACE:
      case DT_OBJECTCLASS:
      case DT_TRANSPORTCLASS:
      case DT_OBJECTCOMMAND:
      case DT_FLIGHTRETURN:
      case DT_SCRIPTDEF:
      case DT_WINGCOMMAND:
         iOutput = pInput->iValue;
         break;

      /// [ARRAY/EXPRESSION/QUEST/OBJECT/UNKNOWN/WING/PASSENGER] -- Unsupported
      default:
         // Convert data type to string
         findGameStringByID(pInput->eType, identifyGamePageIDFromDataType(DT_DATATYPE), pParameterType);
         // [ERROR] "Cannot translate parameter because it is using an unrecognised or unsupported data type '%s'"
         pError = generateDualError(HERE(IDS_TRANSLATION_DATATYPE_UNSUPPORTED), pParameterType ? pParameterType->szText : TEXT("unknown"));
         break;
      }
   }

   // Return TRUE if there were no errors
   return (pError == NULL);
}


/// Function name  : verifyGameStringDependencies
// Description     : Generates a warning for every missing GameString dependency
// 
// CONST COMMAND*  pCommand    : [in]     COMMAND containing a command that references a game string
// ERROR_QUEUE*    pErrorQueue : [in/out] New error if any, otherwise NULL
// 
// Return Value   : TRUE if all are present, otherwise FALSE
// 
BOOL  verifyGameStringDependencies(COMMAND*  pCommand, ERROR_QUEUE*  pErrorQueue)
{
   GAME_STRING_REF *pReference;         // COMMAND's GameString references
   ERROR_STACK     *pError = NULL;
   GAME_STRING     *pTargetString;      // Dummy GameString for testing retrieval
   LIST            *pReferenceList;     // Reference list
   INT              iPageID,
                    iStringID;

   /// Lookup the COMMAND's Page and String parameter indicies
   if (pReferenceList = generateParameterGameStringRefs(pCommand))
   {
      // Iterate through the references
      for (UINT  iIndex = 0; findListObjectByIndex(pReferenceList, iIndex, (LPARAM&)pReference); iIndex++)
      {
         /// Locate the PARAMETERS
         if (!findIntegerParameterInCommandByIndex(pCommand, pReference->iPageParameterIndex, iPageID) OR
             !findIntegerParameterInCommandByIndex(pCommand, pReference->iStringParameterIndex, iStringID))
             continue;

         // [CHECK] Abort if both are zero, this often happens in commands with multiple references
         if (iPageID == 0 AND iStringID == 0)
            continue;

         /// [CHECK] Lookup the specified GameString
         if (!findGameStringByID(iStringID, iPageID, pTargetString))
         {
            // [WARNING] "Cannot find the GameString %u:%u referenced by the command '%s' on line %u"
            pError = generateDualWarning(HERE(IDS_TRANSLATION_GAME_STRING_REFERENCE_NOT_FOUND), iPageID, iStringID, pCommand->szBuffer, pCommand->iLineNumber);
            pushCommandAndOutputQueues(pError, pErrorQueue, pCommand->pErrorQueue, ET_WARNING);
         }
      }

      // Cleanup
      deleteList(pReferenceList);
   }

   // Return TRUE if there were no errors
   return (pError == NULL);
}


/// Function name  : verifyScriptEngineVersion
// Description     : Determines whether a ScriptFile's engineVersion matches the currently selected game version
// 
// CONST SCRIPT_FILE*  pScriptFile : [in]     ScriptFile containing the EngineVersion to test
// ERROR_QUEUE*        pErrorQueue : [in/out] Error queue to fill with a warning if engine version is inappropriate
// 
// Return Value   : TRUE if verification successful, FALSE if it fails
// 
BOOL  verifyScriptEngineVersion(CONST SCRIPT_FILE*  pScriptFile, ERROR_QUEUE*  pErrorQueue)
{
   // [CHECK] Is the script for a later game version than the current GameData?
   if (pScriptFile->eGameVersion > getAppPreferences()->eGameVersion)
   {
      /// [WARNING] "This script is written for %s whereas your current game data is for %s"
      pushErrorQueue(pErrorQueue, generateDualWarning(HERE(IDS_SCRIPT_ENGINE_VERSION_HIGHER), identifyGameVersionString(pScriptFile->eGameVersion), identifyGameVersionString(getAppPreferences()->eGameVersion)));
      generateOutputTextFromLastError(pErrorQueue);
      return FALSE;
   }

   // [SUCCESS] Return TRUE
   return TRUE;
}


/// Function name  : verifyScriptFileScriptNameFromPath
// Description     : Ensure ScriptName matches Filename, and generate a warning if not
// 
// SCRIPT_FILE*  pScriptFile   : [in/out] ScriptFile to verify
// ERROR_QUEUE*  pErrorQueue   : [in/out] Error queue, may already contain errors
// 
VOID  verifyScriptFileScriptNameFromPath(SCRIPT_FILE*  pScriptFile, ERROR_QUEUE*  pErrorQueue)
{
   TCHAR  *szOriginalScriptName,    // Current script name
          *szScriptFileName;        // Filename of the script

   // Extract Filename without extension
   szScriptFileName = utilDuplicateSimpleString(PathFindFileName(pScriptFile->szFullPath));
   PathRemoveExtension(szScriptFileName);

   // [CHECK] Does ScriptName match the FileName?
   if (!utilCompareStringVariables(szScriptFileName, pScriptFile->szScriptname))
   {
      /// [FAILED] Change the NAME to match the PATH
      szOriginalScriptName = utilDuplicateSimpleString(pScriptFile->szScriptname);
      setScriptNameFromPath(pScriptFile);

      // [MODIFIED] Set 'modified on load' flag
      pScriptFile->bModifiedOnLoad = TRUE;

      // [WARNING] "The name of the script '%s' has been changed to '%s' to match the file name"
      pushErrorQueue(pErrorQueue, generateDualWarning(HERE(IDS_SCRIPT_NAME_MISMATCH), szOriginalScriptName, pScriptFile->szScriptname));

      // Cleanup
      utilDeleteString(szOriginalScriptName);
   }

   // Cleanup
   utilDeleteString(szScriptFileName);
}


