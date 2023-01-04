//
// Code Syntax.cpp : Helper functions for objects used to interpret the scripting langugage
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"
#include "RichText Macros.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                  CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////

// Maximum number of possible valid command syntax entry properties
CONST UINT  iMaxProperties = 15;  

// Command group names that I created
CONST TCHAR*  szCommandGroups[COMMAND_GROUP_COUNT] =  
{
   TEXT("ARRAY"),		         // CG_ARRAY
   TEXT("CUSTOM"),		      // CG_CUSTOM
   TEXT("FLEET"),		         // CG_FLEET
   TEXT("FLOW_CONTROL"),		// CG_FLOW_CONTROL
   TEXT("GAME_ENGINE"),		   // CG_GAME_ENGINE
   TEXT("GRAPH"),		         // CG_GRAPH
   TEXT("MACRO"),		         // CG_MACRO
   TEXT("MARINE"),		      // CG_MARINE
   TEXT("MATHS"),		         // CG_MATHS
   TEXT("MERCHANT"),		      // CG_MERCHANT
   TEXT("NON_PLAYER"),		   // CG_NON_PLAYER
   TEXT("OBJECT_ACTION"),		// CG_OBJECT_ACTION
   TEXT("OBJECT_PROPERTY"),	// CG_OBJECT_PROPERTY
   ///TEXT("OBJECT_TRADE"),   // CG_OBJECT_TRADE      REM: Removed
   TEXT("PASSENGER"),		   // CG_PASSENGER
   TEXT("PILOT"),		         // CG_PILOT
   TEXT("PLAYER"),		      // CG_PLAYER
   TEXT("SCRIPT_PROPERTY"),	// CG_SCRIPT_PROPERTY
   TEXT("SHIP_ACTION"),		   // CG_SHIP_ACTION
   TEXT("SHIP_PROPERTY"),		// CG_SHIP_PROPERTY
   TEXT("SHIP_TRADE"),		   // CG_SHIP_TRADE
   TEXT("SHIP_WING"),		   // CG_SHIP_WING
   ///TEXT("STATION_ACTION"),	// CG_STATION_ACTION    REM: Removed
   TEXT("STATION_PROPERTY"),	// CG_STATION_PROPERTY
   TEXT("STATION_TRADE"),		// CG_STATION_TRADE
   TEXT("STOCK_EXCHANGE"),		// CG_STOCK_EXCHANGE
   TEXT("STRING"),		      // CG_STRING
   TEXT("SYSTEM_PROPERTY"),	// CG_SYSTEM_PROPERTY
   TEXT("UNIVERSE_DATA"),		// CG_UNIVERSE_DATA
   TEXT("UNIVERSE_PROPERTY"),	// CG_UNIVERSE_PROPERTY
   TEXT("USER_INTERFACE"),		// CG_USER_INTERFACE
   TEXT("WAR"),               // GC_WAR
   TEXT("WARE_PROPERTY"),		// CG_WARE_PROPERTY
   TEXT("WEAPON_PROPERTY"),	// CG_WEAPON_PROPERTY
   TEXT("HIDDEN")             // CG_HIDDEN
};

// Hard-coded parameter syntax, used only for resolving parameter syntax from the syntax definitions file
CONST TCHAR*  szParameterSyntax[PARAMETER_SYNTAX_COUNT] = 
{
   TEXT("Var"),
   TEXT("Label Name"),
   TEXT("String"),
   TEXT("Number"),
   TEXT("Condition"),
   TEXT("Parameter"),
   TEXT("Label Number"),
   TEXT("Script Name"),
   TEXT("Comment"),
   TEXT("Value"),
   TEXT("Var/Number"),
   TEXT("Var/String"),
   TEXT("Var/Station"),
   TEXT("Var/Ware"),
   TEXT("Var/Sector"),
   TEXT("RetVar"),
   TEXT("Var/Race"),
   TEXT("Var/Station Type"),
   TEXT("Var/Stations Serial"),
   TEXT("Var/Ship Type"),
   TEXT("RefObj"),
   TEXT("Var/Ship"),
   TEXT("Var/Ship/Station"),
   TEXT("Var/Class"),
   TEXT("Var/Transport Class"),
   TEXT("@RetVar/IF"),
   TEXT("RetVar/IF"),
   TEXT("Var/Ship Type/Station Type"),
   TEXT("RetVar/IF/START"),
   TEXT("Relation"),
   TEXT("Expression"),
   TEXT("Object Command"),
   TEXT("Object Signal"),
   TEXT("Object Command/Signal"),
   TEXT("Flight Retcode"),
   TEXT("Var/Ship owned by Player"),
   TEXT("Var/Station owned by Player"),
   TEXT("Var/Station/Carrier owned by Player"),
   TEXT("Var/Ship/Station owned by Player"),
   TEXT("Var/Warpgate"),
   TEXT("Var/Station/Carrier"),
   TEXT("Var/Station/Carrier to dock at"),
   TEXT("Var/All Wares"),
   TEXT("Var/Environment"),
   TEXT("Var/Script Data Type"),
   TEXT("Var/Array"),
   TEXT("Var/Station and Resource"),
   TEXT("Var/Station and Product"),
   TEXT("Var/Station and Ware"),
   TEXT("Var/Homebase and Resource"),
   TEXT("Var/Homebase and Product"),
   TEXT("Var/Homebase and Ware"),
   TEXT("Var/Sector Position"),
   TEXT("Var/Constant"),
   TEXT("Var/Asteroid"),
   TEXT("Var/Flying Ware"),
   TEXT("Var/Jumpdrive Gate"),
   TEXT("Var/Ship and Ware"),
   TEXT("Var/Ware of Ship"),
   TEXT("Var/Quest"),
   TEXT("--NONE--"),
   TEXT("Var/Wing"),
   TEXT("Script Reference Type"),
   TEXT("Var/Boolean"),
   TEXT("--NONE--"),
   TEXT("Var/Wing Command"),
   TEXT("Var/Passenger"),
   TEXT("Var/Ship and Passenger"),
   TEXT("Var/Passenger Of Ship"),
   TEXT("Var/Sector for Jumpdrive"),
   TEXT("Var/Fleet Commander"),
   TEXT("Var/Global Parameter")
};

// Sub-scripts used by the syntax
CONST TCHAR*  szSyntaxSubScripts[5] = { TEXT("º"), TEXT("¹"), TEXT("²"), TEXT("³"), TEXT("ª") };

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                  CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createCommandSyntax
// Description     : Create a new empty command syntax object
//
// Return type : COMMAND_SYNTAX*  
//
COMMAND_SYNTAX*  createCommandSyntax()
{
   COMMAND_SYNTAX*   pNewSyntax;

   // Create object
   pNewSyntax = utilCreateEmptyObject(COMMAND_SYNTAX);

   // Create RichTitle
   //pNewSyntax->pRichTitle = createRichText();

   // Return object
   return pNewSyntax;
}


/// Function name  : createCommandSyntaxTreeByID
// Description     : Creates the game data CommandSyntax Tree sorted by COMMAND-ID
///                                                   NB: This tree owns the CommandSyntax game data 
// Return Value   : New ComandSyntax tree, you are responsible for destroying it
// 
AVL_TREE*  createCommandSyntaxTreeByID()
{
   // Create new STORAGE tree organised by ID then COMPATIBILITY
   return createAVLTree(extractCommandSyntaxTreeNode, deleteCommandSyntaxTreeNode, createAVLTreeSortKey(AK_ID, AO_ASCENDING), createAVLTreeSortKey(AK_VERSION, AO_ASCENDING), NULL);
}


/// Function name  : createCommandSyntaxTreeByHash
// Description     : Creates the game data CommandSyntax Tree sorted by COMMAND HASH
// 
// Return Value   : New ComandSyntax tree, you are responsible for destroying it
// 
AVL_TREE*  createCommandSyntaxTreeByHash()
{
   // Create new STORAGE tree organised by TEXT then COMPATIBILITY
   return createAVLTree(extractCommandSyntaxTreeNode, deleteCommandSyntaxTreeNode, createAVLTreeSortKey(AK_TEXT, AO_ASCENDING), createAVLTreeSortKey(AK_VERSION, AO_ASCENDING), NULL);
}



/// Function name  : duplicateCommandSyntax
// Description     : Creates a duplicate of a CommandSyntax object
// 
// CONST COMMAND_SYNTAX*  pCommandSyntax : [in] CommandSyntax object to copy
// 
// Return Value   : New CommandSyntax object, you are responsible for destroying it
// 
COMMAND_SYNTAX*  duplicateCommandSyntax(CONST COMMAND_SYNTAX*  pCommandSyntax)
{
   COMMAND_SYNTAX*  pNewSyntax;

   // Create empty object
   pNewSyntax = createCommandSyntax();

   // Copy simple values
   utilCopyObject(pNewSyntax, pCommandSyntax, COMMAND_SYNTAX);

   // Duplicate strings
   pNewSyntax->szSuggestion     = utilDuplicateSimpleString(pCommandSyntax->szSuggestion);
   pNewSyntax->szReferenceURL   = utilDuplicateSimpleString(pCommandSyntax->szReferenceURL);
   pNewSyntax->szSyntax         = utilDuplicateSimpleString(pCommandSyntax->szSyntax);
   pNewSyntax->szContent        = utilDuplicateSimpleString(pCommandSyntax->szContent);
   pNewSyntax->szHash           = utilDuplicateSimpleString(pCommandSyntax->szHash);

   // Duplicate RichText
   pNewSyntax->pDisplaySyntax      = duplicateRichText(pCommandSyntax->pDisplaySyntax);
   pNewSyntax->pTooltipSyntax      = duplicateRichText(pCommandSyntax->pTooltipSyntax);
   pNewSyntax->pTooltipDescription = (pNewSyntax->pTooltipDescription ? duplicateRichText(pCommandSyntax->pTooltipDescription) : NULL);

   // Return new object
   return pNewSyntax;
}

/// Function name  : deleteCommandSyntax
// Description     : Destroys a CommandSyntax object
// 
// COMMAND_SYNTAX*  &pCommandSyntax : [in] CommandSyntax object to destroy
//
VOID    deleteCommandSyntax(COMMAND_SYNTAX*  &pCommandSyntax)
{
   // Delete SYNTAX
   utilDeleteString(pCommandSyntax->szSyntax);
   
   // [OPTIONAL] Delete HASH, SUGGESTION and URL
   utilSafeDeleteString(pCommandSyntax->szHash);
   utilSafeDeleteString(pCommandSyntax->szContent);
   utilSafeDeleteString(pCommandSyntax->szSuggestion);
   utilSafeDeleteString(pCommandSyntax->szReferenceURL);

   // [OPTIONAL] Delete DisplaySyntax
   if (pCommandSyntax->pDisplaySyntax)
      deleteRichText(pCommandSyntax->pDisplaySyntax);

   // [OPTIONAL] Delete TooltipSyntax
   if (pCommandSyntax->pTooltipSyntax)
      deleteRichText(pCommandSyntax->pTooltipSyntax);

   // [OPTIONAL] Delete Description
   if (pCommandSyntax->pTooltipDescription)
      deleteRichText(pCommandSyntax->pTooltipDescription);
   
   // Delete calling object
   utilDeleteObject(pCommandSyntax);
}

/// Function name  : deleteCommandSyntaxTreeNode
// Description     : Deletes a CommandSyntax object within an AVLTree Node
// 
// LPARAM  pData : [in] Reference to a CommandSyntax pointer
// 
VOID   deleteCommandSyntaxTreeNode(LPARAM  pData)
{
   // Delete node contents
   deleteCommandSyntax((COMMAND_SYNTAX*&)pData);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : calculateCommandSyntaxGameVersion
// Description     : Determines a command's gameversion
// 
// CONST COMMAND_SYNTAX*  pCommandSyntax  : [in] CommandSyntax object to query
// 
// Return Value : TRUE / FALSE
//
BearScriptAPI
GAME_VERSION   calculateCommandSyntaxGameVersion(CONST COMMAND_SYNTAX*  pCommandSyntax)
{
   GAME_VERSION  eResult;
   
   // Examine compatibility flags
   if (pCommandSyntax->iCompatibility INCLUDES CV_THREAT)
      eResult = GV_THREAT;
   else if (pCommandSyntax->iCompatibility INCLUDES CV_REUNION)
      eResult = GV_REUNION;
   else if (pCommandSyntax->iCompatibility INCLUDES CV_TERRAN_CONFLICT)
      eResult = GV_TERRAN_CONFLICT;
   else if (pCommandSyntax->iCompatibility INCLUDES CV_ALBION_PRELUDE)
      eResult = GV_ALBION_PRELUDE;
   else
      // [ERROR] Return 'Unknown'
      eResult = GV_UNKNOWN;
   
   // Return result
   return eResult;
}


/// Function name  : extractCommandSyntaxTreeNode
// Description     : Extract the desired property from a given TreeNode containing a CommandSyntax object
// 
// LPARAM                   pNode      : [in] COMMAND_SYNTAX* : Node data containing a CommandSyntax
// CONST AVL_TREE_SORT_KEY  eProperty  : [in] Property to retrieve
// 
// Return Value   : Value of the specified property or NULL if the property is unsupported
// 
BearScriptAPI
LPARAM  extractCommandSyntaxTreeNode(LPARAM  pNode, CONST AVL_TREE_SORT_KEY  eProperty)
{
   COMMAND_SYNTAX*  pCommandSyntax;    // Convenience pointer
   LPARAM           xOutput;           // Property value output

   // Prepare
   pCommandSyntax = (COMMAND_SYNTAX*)pNode;

   // Examine property
   switch (eProperty)
   {
   case AK_TEXT:     xOutput = (LPARAM)pCommandSyntax->szHash;          break;      /// NB: 'AK_TEXT' sorts by hash, rather than syntax
   case AK_ID:       xOutput = (LPARAM)pCommandSyntax->iID;             break;
   case AK_GROUP:    xOutput = (LPARAM)pCommandSyntax->eGroup;          break;
   case AK_VERSION:  xOutput = (LPARAM)pCommandSyntax->iVariation;      break;      /// NB: 'AV_VERSION' sorts by Variation rather than compatibility
   default:          xOutput = NULL;                                    break;
   }

   // Return property value
   return xOutput;
}


/// Function name  : findCommandSyntaxByID
// Description     : Find the syntax for a command using the command ID
//
// CONST UINT              iID          : [in]  ID of the command to search for
// CONST GAME_VERSION      eGameVersion : [in]  Version of the game the command must be compatible with
// CONST COMMAND_SYNTAX*  &pOutput      : [out] Command Syntax object, or NULL if not found
// 
// Return type : TRUE if found, FALSE if not
//
BearScriptAPI
BOOL    findCommandSyntaxByID(CONST UINT  iID, CONST GAME_VERSION  eGameVersion, CONST COMMAND_SYNTAX*  &pOutput, ERROR_STACK*  &pError)
{
   CONST COMMAND_SYNTAX  *pFirstVariation,
                         *pSecondVariation;

   // Prepare
   pOutput          = NULL;
   pFirstVariation  = NULL;
   pSecondVariation = NULL;
   
   /// [CHECK] Search for compatible entry under VARIATION ZERO
   if (findObjectInAVLTreeByValue(getGameData()->pCommandTreeByID, iID, 0, (LPARAM&)pFirstVariation) AND isCommandSyntaxCompatible(pFirstVariation, eGameVersion))
      // [FOUND] Return first variation
      pOutput = pFirstVariation;

   /// [CHECK] Search for compatible entry under VARIATION ONE
   else if (findObjectInAVLTreeByValue(getGameData()->pCommandTreeByID, iID, 1, (LPARAM&)pSecondVariation) AND isCommandSyntaxCompatible(pSecondVariation, eGameVersion))
      // [FOUND] Return second variation
      pOutput = pSecondVariation;

   /// [NO ENTRIES] The command is unknown/invalid
   else if (!pFirstVariation AND !pSecondVariation)
      // [ERROR] "Cannot locate a syntax entry for the command with ID %u"
      pError = generateDualError(HERE(IDS_COMMAND_SYNTAX_NOT_FOUND), iID);

   /// [TWO INCOMPATIBLE] Neither of the two commands exists in the specified version
   else if (pFirstVariation AND pSecondVariation)
      // [ERROR] "Neither syntax entry with ID %u are compatible with %s"
      pError = generateDualError(HERE(IDS_COMMAND_SYNTAX_VARIATIONS_INCOMPATIBLE), iID, identifyGameVersionString(eGameVersion));

   /// [ONE INCOMPATIBLE] Command does not exist in the specified version
   else if (pFirstVariation OR pSecondVariation)
      // [ERROR] "The command '%s' (ID:%u) is not compatible with %s"
      pError = generateDualError(HERE(IDS_COMMAND_SYNTAX_INCOMPATIBLE), utilEither(pFirstVariation,pSecondVariation)->szContent, iID, identifyGameVersionString(eGameVersion));
   
   // Return TRUE if a compatible command was found, FALSE if there was an error
   return (pOutput != NULL);
}


/// Function name  : findCommandSyntaxByHash
// Description     : Find the syntax for a specific command by comparing the command hash
// 
// CONST TCHAR*            szSearchHash : [in]  The hash of the command has to search for
// CONST GAME_VERSION      eGameVersion : [in]  Version of the game the command must be compatible with
// CONST COMMAND_SYNTAX*  &pOutput      : [out] The command syntax object, or NULL if no match was found
// ERROR_STACK*           &pError       : [out] Error message if any, otherwise NULL
// 
// Return type : TRUE if found, FALSE if not.
//
BOOL    findCommandSyntaxByHash(CONST TCHAR*  szSearchHash, CONST GAME_VERSION  eGameVersion, CONST COMMAND_SYNTAX*  &pOutput, ERROR_STACK*  &pError)
{
   CONST COMMAND_SYNTAX  *pFirstVariation,
                         *pSecondVariation;

   // Prepare
   pFirstVariation  = NULL;
   pSecondVariation = NULL;

   /// [CHECK] Search for compatible entry under VARIATION ZERO
   if (findObjectInAVLTreeByValue(getGameData()->pCommandTreeByHash, (LPARAM)szSearchHash, 0, (LPARAM&)pFirstVariation) AND isCommandSyntaxCompatible(pFirstVariation, eGameVersion))
      // [FOUND] Return first variation
      pOutput = pFirstVariation;

   /// [CHECK] Search for compatible entry under VARIATION ONE
   else if (findObjectInAVLTreeByValue(getGameData()->pCommandTreeByHash, (LPARAM)szSearchHash, 1, (LPARAM&)pSecondVariation) AND isCommandSyntaxCompatible(pSecondVariation, eGameVersion))
      // [FOUND] Return second variation
      pOutput = pSecondVariation;

   /// [NO ENTRIES] The command is unknown/invalid
   else if (!pFirstVariation AND !pSecondVariation)
      // [ERROR] "Cannot find a corresponding %s command syntax entry from the hash '%s'"
      pError = generateDualError(HERE(IDS_GENERATION_COMMAND_SYNTAX_NOT_FOUND), identifyGameVersionString(eGameVersion), szSearchHash);

   /// [TWO INCOMPATIBLE] Neither of the two commands exists in the specified version
   else if (pFirstVariation AND pSecondVariation)
      // [ERROR] "Neither syntax entry with ID %u are compatible with %s"
      pError = generateDualError(HERE(IDS_COMMAND_SYNTAX_VARIATIONS_INCOMPATIBLE), pFirstVariation->iID, identifyGameVersionString(eGameVersion));

   /// [ONE INCOMPATIBLE] Command does not exist in the specified version
   else if (pFirstVariation OR pSecondVariation)
      // [ERROR] "The command '%s' (ID:%u) is not compatible with %s"
      pError = generateDualError(HERE(IDS_COMMAND_SYNTAX_INCOMPATIBLE), utilEither(pFirstVariation,pSecondVariation)->szContent, utilEither(pFirstVariation,pSecondVariation)->iID, identifyGameVersionString(eGameVersion));
   
   // Return TRUE if a compatible command was found, FALSE if there was an error
   return (pOutput != NULL);
}


/// Function name  : findParameterSyntaxByName
// Description     : Resolves a string containing parameter syntax into its corresponding ID
// 
// CONST TCHAR*       szSyntax  : [in]  String containing parameter syntax
// PARAMETER_SYNTAX&  eOutput   : [out] Syntax if found, otherwise PS_UNDETERMINED
// 
// Return Value   : TRUE if found, otherwise FALSE
// 
BOOL   findParameterSyntaxByName(CONST TCHAR*  szSyntax, PARAMETER_SYNTAX&  eOutput)
{
   SCRIPT_OBJECT*  pScriptObject;      // Object Lookup

   // Prepare
   eOutput = PS_UNDETERMINED;

   /// Lookup ScriptObject from parameter syntax string
   if (findScriptObjectByText(szSyntax, pScriptObject))
      // Determine syntax from ScriptObject's original game string ID
      eOutput = (PARAMETER_SYNTAX)pScriptObject->iID;

   // Return TRUE if found
   return (eOutput != PS_UNDETERMINED);
}


/// Function name  : findParameterSyntaxByPhysicalIndex
// Description     : Retrieve the ParameterSyntax from a CommandSyntax object by index
// 
// CONST COMMAND_SYNTAX*  pSyntax  : [in]  CommandSyntax object
// CONST UINT             iIndex   : [in]  Zero-based physical index of the syntax to retrieve
// PARAMETER_SYNTAX&      eOutput  : [out] Resultant ParameterSyntax
// 
// Return Value   : TRUE if found, FALSE if the index was invalid
// 
BOOL   findParameterSyntaxByPhysicalIndex(CONST COMMAND_SYNTAX*  pSyntax, CONST UINT  iIndex, PARAMETER_SYNTAX&  eOutput)
{
   // [CHECK] Validate index
   if (iIndex >= pSyntax->iParameterCount)
      return FALSE;

   // [SUCCESS] Return desired syntax
   eOutput = pSyntax->eParameters[iIndex];
   return TRUE;
}


/// Function name  : identifyCommandCompatibilityFlags
// Description     : Parses the syntax declaration command compatibilty string into a combination of COMMAND_VERSION flags
// 
// TCHAR*  szCompatilityIDs   : [in] Command compatility string from the syntax declaration file
// 
// Return Value   : Combination of COMMAND_VERSION flags indicating which game version the command is compatible with
// 
UINT  identifyCommandCompatibilityFlags(TCHAR*  szCompatilityIDs)
{
   TCHAR  *szGameVersion,     // Game version ID being processed
          *pIterator;         // Tokeniser data
   UINT    iOutput;           // Compatibility flags

   // Prepare
   iOutput = NULL;

   // Tokenise compatibilty string
   for (szGameVersion = utilTokeniseString(szCompatilityIDs, ",", &pIterator); szGameVersion; szGameVersion = utilGetNextToken(",", &pIterator))
   {
      /// [THE THREAT] Search for X2
      if (utilCompareString(szGameVersion, "X2"))
         iOutput |= CV_THREAT;
      /// [REUNION] Search for X3R
      else if (utilCompareString(szGameVersion, "X3R"))
         iOutput |= CV_REUNION;
      /// [TERRAN CONFLICT] Search for X3TC
      else if (utilCompareString(szGameVersion, "X3TC"))
         iOutput |= CV_TERRAN_CONFLICT;
      /// [ALBION PRELUDE] Search for X3AP
      else if (utilCompareString(szGameVersion, "X3AP"))
         iOutput |= CV_ALBION_PRELUDE;
   }

   // Return result
   return iOutput;
}


/// Function name  : identifyCommandGroup
// Description     : Identify a CommandGroup ID from a string
// 
// CONST TCHAR*  szGroupName : [in] String containing one of the command groups I have defined
// 
// Return type : CommandGroup ID if recongised, otherwise CG_HIDDEN
//
COMMAND_GROUP  identifyCommandGroupFromName(CONST TCHAR*  szGroupName)
{
   /// Iterate through all group IDs
   for (UINT iGroup = FIRST_COMMAND_GROUP; iGroup <= CG_HIDDEN; iGroup++)
   {
      // Compare group name
      if (utilCompareStringVariables(szGroupName, szCommandGroups[iGroup]))
         // [SUCCES] Return group ID
         return (COMMAND_GROUP)iGroup;
   }

   // [ERROR] Unrecognised group
   ASSERT(FALSE);
   return CG_HIDDEN;
}


/// Function name  : identifyParameterSyntaxDefaultValue
// Description     : Supplies the default value for a parameter syntax, used for suggestion text
// 
// CONST PARAMETER_SYNTAX  eSyntax : [in] Parameter syntax
// 
// Return Value   : Default value, or an empty string for RefObj/RetVar
// 
BearScriptAPI
CONST TCHAR*  identifyParameterSyntaxDefaultValue(CONST PARAMETER_SYNTAX  eSyntax)
{
   CONST TCHAR*  szOutput;

   // Examine syntax
   switch (eSyntax)
   {
   // Return/Reference object
   case PS_REFERENCE_OBJECT:
   case PS_RETURN_OBJECT:
   case PS_RETURN_OBJECT_IF:
   case PS_RETURN_OBJECT_IF_START:
   case PS_INTERRUPT_RETURN_OBJECT_IF:    szOutput = TEXT("");                                break;

   // Number
   case PS_VARIABLE_NUMBER:		         
   case PS_NUMBER:		                  szOutput = TEXT("0");                               break;

   // String
   case PS_VARIABLE_STRING:		         
   case PS_STRING:		                  szOutput = TEXT("'String'");                        break;

   // Label
   case PS_LABEL_NAME:		         
   case PS_LABEL_NUMBER:		            szOutput = TEXT("Label");                           break;

   // Simple types
   case PS_ARRAY:		                     szOutput = TEXT("$Array");                          break;
   case PS_VALUE:		                     szOutput = TEXT("$Value");                          break;
   case PS_VARIABLE:		                  szOutput = TEXT("$Variable");                       break;
   case PS_VARIABLE_CONSTANT:		         szOutput = TEXT("[Constant]");                      break;
   case PS_VARIABLE_BOOLEAN:		         szOutput = TEXT("[Boolean]");                       break;
   case PS_VARIABLE_DATATYPE:		         szOutput = TEXT("[DataType]");                      break;
   case PS_SCRIPT_NAME:		               szOutput = TEXT("'ScriptName'");                    break;

   // Ships / Stations
   case PS_VARIABLE_SHIP:		            szOutput = TEXT("$Ship");                           break;
   case PS_VARIABLE_WING:		            szOutput = TEXT("$Wing");                           break;
   case PS_VARIABLE_SHIP_STATION:		   szOutput = TEXT("$ShipOrStation");                  break;
   case PS_VARIABLE_PLAYER_SHIP:		      szOutput = TEXT("$PlayerShip");                     break;
   case PS_VARIABLE_PLAYER_SHIP_STATION:	szOutput = TEXT("$PlayerShipOrStation");            break;
   case PS_VARIABLE_SHIPTYPE:		         szOutput = TEXT("{ShipType}");                      break;
   case PS_VARIABLE_SHIPTYPE_STATIONTYPE:	szOutput = TEXT("{ShipOrStationType}");             break;

   // Stations
   case PS_STATION_CARRIER:		            szOutput = TEXT("$StationOrCarrier");            break;
   case PS_STATION_CARRIERDOCKAT:		      szOutput = TEXT("$StationOrCarrier");            break;
   case PS_VARIABLE_ENVIRONMENT:		         szOutput = TEXT("$CarrierOrSector");             break;
   case PS_VARIABLE_PLAYER_STATION_CARRIER:	szOutput = TEXT("$PlayerStationOrCarrier");      break;
   case PS_VARIABLE_PLAYER_STATION:		      szOutput = TEXT("$PlayerStation");               break;
   case PS_VARIABLE_STATION:		            szOutput = TEXT("{Station}");                    break;
   case PS_VARIABLE_STATIONPRODUCT:		   szOutput = TEXT("{Ware}");                          break;
   case PS_VARIABLE_STATIONRESOURCE:		szOutput = TEXT("{Ware}");                          break;
   case PS_VARIABLE_STATIONSERIAL:		   szOutput = TEXT("[StationSerial]");                 break;
   case PS_VARIABLE_STATIONTYPE:		      szOutput = TEXT("{StationType}");                   break;
   case PS_VARIABLE_STATIONWARE:		      szOutput = TEXT("{Ware}");                          break;
   case PS_VARIABLE_HOMEBASEPRODUCT:		szOutput = TEXT("{Ware}");                          break;
   case PS_VARIABLE_HOMEBASERESOURCE:		szOutput = TEXT("{Ware}");                          break;
   case PS_VARIABLE_HOMEBASEWARE:		   szOutput = TEXT("{Ware}");                          break;

   // Passengers
   case PS_VARIABLE_PASSENGER:		      szOutput = TEXT("$Passenger");                      break;
   case PS_VARIABLE_PASSENGER_OF_SHIP:		szOutput = TEXT("$Passenger");                      break;
   case PS_VARAIBLE_SHIP_AND_PASSENGER:	szOutput = TEXT("$ShipAndPassenger");               break;
   
   // Wares
   case PS_VARIABLE_ALLWARES:		      szOutput = TEXT("{Ware}");                             break;
   case PS_VARIABLE_FLYINGWARE:		   szOutput = TEXT("$FlyingWare");                        break;
   case PS_VARIABLE_WARE:		         szOutput = TEXT("{Ware}");                             break;
   case PS_VARIABLE_WARE_OF_SHIP:		szOutput = TEXT("$Ware");                              break;
   case PS_VARIABLE_SHIP_AND_WARE:		szOutput = TEXT("$ShipAndWare");                       break;

   // Special objects                                                                                                     
   case PS_VARIABLE_ASTEROID:		      szOutput = TEXT("$Asteroid");                          break;
   case PS_VARIABLE_JUMPDRIVE_GATE:		szOutput = TEXT("$JumpGate");                          break;
   case PS_VARIABLE_JUMPDRIVE_SECTOR:	szOutput = TEXT("[Sector]");                           break;
   case PS_VARIABLE_WARPGATE:		      szOutput = TEXT("$JumpGate");                          break;

   // Albion Prelude
   case PS_VARIABLE_FLEET_COMMANDER:	szOutput = TEXT("$FleetCommander");                    break;
   case PS_VARIABLE_GLOBAL_PARAMETER:	szOutput = TEXT("$GlobalParameter");                   break;
   
   // Constants
   case PS_FLIGHTRETCODE:		         szOutput = TEXT("[FlightReturn]");                     break;
   case PS_OBJECTCOMMAND:		         szOutput = TEXT("[Command]");                          break;
   case PS_OBJECTCOMMAND_OBJECTSIGNAL:	szOutput = TEXT("[CommandOrSignal]");                  break;
   case PS_OBJECTSIGNAL:		         szOutput = TEXT("[Signal]");                           break;
   case PS_RELATION:		               szOutput = TEXT("[Relation]");                         break;
   case PS_SECTOR_POSITION:		      szOutput = TEXT("$PositionArray");                     break;
   case PS_VARIABLE_CLASS:		         szOutput = TEXT("[ObjectClass]");                      break;
   case PS_VARIABLE_QUEST:		         szOutput = TEXT("[Quest]");                            break;
   case PS_VARIABLE_RACE:		         szOutput = TEXT("[Race]");                             break;
   case PS_VARIABLE_SECTOR:		      szOutput = TEXT("[Sector]");                           break;
   case PS_VARIABLE_TRANSPORTCLASS:		szOutput = TEXT("[TransportClass]");                   break;
   case PS_VARIABLE_WING_COMMAND:		szOutput = TEXT("[WingCommand]");                      break;
   case PS_SCRIPT_REFERENCE_TYPE:      szOutput = TEXT("[ParameterType]");                    break;

   // Unknown
   default:                            szOutput = TEXT("$Unknown");                           break;
   }

   // Return result
   return szOutput;
}


/// Function name  : identifyParameterSyntaxFromName
// Description     : Manually resolves a string containing ParameterSyntax into it's matching ID
///                      Used only by loadCommandSyntaxTree - used for loading syntax which is not available in previous game versions
// 
// CONST TCHAR*  szSyntax : [in] String containing a parameter syntax in english
// 
// Return Value   : Syntax if found, otherwise PS_UNDETERMINED
// 
PARAMETER_SYNTAX  identifyParameterSyntaxFromName(CONST TCHAR*  szSyntax)
{
   PARAMETER_SYNTAX  eOutput;    // Operation result

   // Prepare
   eOutput = PS_UNDETERMINED;

   /// Iterate through each entry in the ParameterSyntax array
   for (UINT  eSyntax = FIRST_PARAMETER_SYNTAX; (eSyntax <= LAST_PARAMETER_SYNTAX) AND (eOutput == PS_UNDETERMINED); eSyntax++)
      // [CHECK] Does this match the current syntax?
      if (utilCompareStringVariables(szSyntax, szParameterSyntax[eSyntax]))
         /// [FOUND] Set result and abort
         eOutput = (PARAMETER_SYNTAX)eSyntax;

   // Return result
   return eOutput;
}


/// Function name  : identifyParameterSyntaxSubScript
// Description     : Converts parameter syntax indicies and sub-script characters into sub-script literals
// 
// TCHAR  chChar   : [in] Character to test
// 
// Return Value   : Subscript if any, otherwise NULL
// 
CONST TCHAR*  identifyParameterSyntaxSubScript(TCHAR  chChar)
{
   CONST TCHAR*  szOutput;

   // Examine character
   switch (chChar)
   {
   case L'º':
   case 'o':
   case '0':   szOutput = szSyntaxSubScripts[0];   break;

   case L'¹':
   case 'x':
   case '1':   szOutput = szSyntaxSubScripts[1];   break;

   case L'²':
   case 'y':
   case '2':   szOutput = szSyntaxSubScripts[2];   break;

   case L'³':
   case 'z':
   case '3':   szOutput = szSyntaxSubScripts[3];   break;

   case L'ª':
   case 'a':
   case '4':   szOutput = szSyntaxSubScripts[4];   break;
   /// [NOT SUB-SCRIPT] Return NULL
   default:    szOutput = NULL;                    break;
   }

   // Return result
   return szOutput;
}


/// Function name  : isParameterSyntaxValid
// Description     : Ensures a parameter syntax is a valid value
// 
// CONST PARAMETER_SYNTAX  eSyntax : [in] Syntax
// 
// Return Value   : TRUE/FALSE
// 
BOOL  isParameterSyntaxValid(CONST PARAMETER_SYNTAX  eSyntax)
{
   // [CHECK] Ensure syntax is valid
   return (eSyntax >= FIRST_PARAMETER_SYNTAX) AND (eSyntax <= LAST_PARAMETER_SYNTAX) AND (eSyntax != 60) AND (eSyntax != 64);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    SYNTAX  FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : generateCommandSyntaxContentText
// Description     : Generates approximate display text for a command
// 
// CONST COMMAND_SYNTAX*  pCommandSyntax : [in] 
// 
// Return Value   : New string, you are responsible for destroying it
// 
TCHAR*  generateCommandSyntaxContentText(CONST COMMAND_SYNTAX*  pCommandSyntax)
{
   CODEOBJECT*  pCodeObject;          // For parsing syntax string
   TCHAR*       szOutput;             // Operation result

   // Prepare
   pCodeObject = createCodeObject(pCommandSyntax->szSyntax);
   szOutput    = utilCreateEmptyString(LINE_LENGTH);

   /// Split CommandSyntax string into CodeObjects
   while (findNextCodeObject(pCodeObject))
   {
      // Examine CodeObject
      switch (pCodeObject->eClass)
      {
      /// [PARAMETER] Strip
      case CO_VARIABLE:
         break;

      /// [ANYTHING ELSE] Append to the output string
      default:
         StringCchCat(szOutput, LINE_LENGTH, pCodeObject->szText);
         break;
      }
   }

   // Trim leading whitespace and trailing commas
   StrTrim(szOutput, TEXT(" ,"));

   // Return output
   deleteCodeObject(pCodeObject);
   return szOutput;
}


/// Function name  : generateCommandSyntaxHash
// Description     : Creates a hash of the command syntax
// 
// CONST COMMAND_SYNTAX*  pSyntax : [in] CommandSyntax object to create a hash for
// 
// Return Value   : New hash of length HASH_LENGTH, you are responsible for destroying it
// 
TCHAR*   generateCommandSyntaxHash(CONST COMMAND_SYNTAX*  pSyntax)
{
   CODEOBJECT*  pCodeObject;     // CodeObject for tokenising the Command syntax
   TCHAR*       szOutput;        // Resultant hash

   // Prepare
   pCodeObject = createCodeObject(pSyntax->szSyntax);
   szOutput    = utilCreateEmptyString(HASH_LENGTH);

   /// Tokenise the command syntax into CodeObjects
   while (findNextCodeObject(pCodeObject))
   {
      // Examine the CodeObject class
      switch (pCodeObject->eClass)
      {
      /// [OPERATOR] Add these to the hash
      case CO_OPERATOR:
         // [REFERENCE] Skip
         if (utilCompareString(pCodeObject->szText, "->"))
            break;
         // [ASSIGNMENT/DISCARD] Skip
         else if (utilCompareString(pCodeObject->szText, "=") AND (pCodeObject->iIndex == 1 OR pCodeObject->iIndex == 2))
            break;         

         appendCodeObjectToHash(pCodeObject, szOutput);
         break;

      /// [WORD] Add all of these to the hash
      case CO_WORD:
         appendCodeObjectToHash(pCodeObject, szOutput);
         break;

      /// [KEYWORD] Skip 'non-command' keywords that form part of conditionals
      case CO_KEYWORD:
         if (isCodeObjectCommandKeyword(pCodeObject))
            appendCodeObjectToHash(pCodeObject, szOutput);
         break;

      /// [EVERYTHING ELSE] Ignore
      default:
         break;
      }
   }

   // Cleanup and return
   deleteCodeObject(pCodeObject);
   return szOutput;
}


/// Function name  : generateCommandSyntaxSuggestionText
// Description     : Generates display text for command syntax
// 
// CONST COMMAND_SYNTAX*  pCommandSyntax : [in] 
// 
// Return Value   : New string, you are responsible for destroying it
// 
TCHAR*  generateCommandSyntaxSuggestionText(CONST COMMAND_SYNTAX*  pCommandSyntax)
{
   PARAMETER_SYNTAX  eSyntax;
   CODEOBJECT*       pCodeObject;          // For parsing syntax string
   TCHAR*            szOutput;             // Operation result
   UINT              iOutputLength;        // Length of output string buffer

   // Prepare
   pCodeObject = createCodeObject(pCommandSyntax->szSyntax);
   szOutput    = utilCreateEmptyString(iOutputLength = LINE_LENGTH);

   /// Split CommandSyntax string into CodeObjects
   while (findNextCodeObject(pCodeObject))
   {
      // Examine CodeObject
      switch (pCodeObject->eClass)
      {
      /// [PARAMETER] Replace with default value
      case CO_VARIABLE:
         if (findParameterSyntaxByPhysicalIndex(pCommandSyntax, utilConvertCharToInteger(pCodeObject->szText[1]), eSyntax)) // AND !isReturnObject(eSyntax) AND eSyntax != PS_REFERENCE_OBJECT)
            StringCchCat(szOutput, iOutputLength, identifyParameterSyntaxDefaultValue(eSyntax));
         break;

      /// [ANYTHING ELSE] Append to the output string
      default:
         StringCchCat(szOutput, iOutputLength, pCodeObject->szText);
         break;
      }
   }

   // [SPECIAL CASE] Add optional arguments to Script Call suggestion text manually
   /*if (pCommandSyntax->iID == CMD_CALL_SCRIPT_VAR_ARGS)
      StringCchCat(szOutput, iOutputLength, TEXT(" argument¹=$Value  argument²=$Value  argumentª=$Value"));*/

   // Trim leading whitespace
   StrTrim(szOutput, TEXT(" "));

   //[DEBUG]
   //VERBOSE("Generated %s", szOutput);

   // Return output
   deleteCodeObject(pCodeObject);
   return szOutput;
}


/// Function name  : generateCommandSyntaxTitle
// Description     : Generates RichText from a syntax string, with either bold commands or bold parameters
// 
// CONST TCHAR*            szSyntax    : [in]     Syntax source text
// CONST RICH_SYNTAX_TYPE  eType       : [in]     Determines whether text or parameters are emboldened
// RICH_TEXT*             &pOutput     : [out]    New RichText, you are responsible for destroying it
// ERROR_QUEUE*            pErrorQueue : [in/out] Error messages caused by improper source formatting. May already contain errors.
// 
// Return Value   : TRUE if there were no errors in the source, otherwise FALSE
//                    A new RichText object is generated in both cases, you are responsible for destroying it
// 
BOOL  generateCommandSyntaxTitle(CONST COMMAND_SYNTAX*  pSyntax, CONST RICH_SYNTAX_TYPE  eType, RICH_TEXT*  &pOutput, ERROR_QUEUE*  pErrorQueue)
{
   PARAMETER_SYNTAX  eParameterSyntax;
   CODEOBJECT       *pCodeObject;          // For parsing syntax string
   TCHAR            *szSource,             // RichText source code for the title
                    *szParameter;
   CONST TCHAR      *szFirstSubScript,     // First parameter sub-script [optional]
                    *szSecondSubScript;    // Second parameter sub-script [optional]
   BOOL              bBoldState,
                     bResult;

   // Prepare
   szSource    = utilCreateEmptyString(1024);
   szParameter = utilCreateEmptyString(64);
   pCodeObject = createCodeObject(pSyntax->szSyntax);   
   bBoldState  = FALSE;

   // Examine syntax
   switch (pSyntax->iID)
   {
   // [REAL COMMAND] Generate syntax programatically
   default:
      /// Split CommandSyntax string into CodeObjects
      while (findNextCodeObject(pCodeObject))
      {
         // Examine CodeObject
         switch (pCodeObject->eClass)
         {
         /// [PARAMETER] Insert parameter text
         case CO_VARIABLE:
            // Extract Parameter syntax
            if (findParameterSyntaxByPhysicalIndex(pSyntax, utilConvertCharToInteger(pCodeObject->szText[1]), eParameterSyntax))
            {
               // [SPECIAL CASE] Fudge some entries to make them pretty
               switch (eParameterSyntax)
               {
               // [@RetVar/IF] Change to 'RetVar/IF'
               case PS_INTERRUPT_RETURN_OBJECT_IF:  eParameterSyntax = PS_RETURN_OBJECT_IF;  break;
               // [Label Number] Change to 'Label Name'
               case PS_LABEL_NUMBER:                eParameterSyntax = PS_LABEL_NAME;        break;
               }

               // Identify sub-scripts, if any
               szFirstSubScript  = (pCodeObject->szText[2] ? identifyParameterSyntaxSubScript(pCodeObject->szText[2]) : NULL);
               szSecondSubScript = (pCodeObject->szText[3] ? identifyParameterSyntaxSubScript(pCodeObject->szText[3]) : NULL);

               // [CHECK] Are there double sub-scripts?
               if (szFirstSubScript AND szSecondSubScript)
                  /// [DOUBLE SUB-SCRIPT] Insert both sub-scripts
                  StringCchPrintf(szParameter, 64, TEXT("«%s%s%s»"), szParameterSyntax[eParameterSyntax], szFirstSubScript, szSecondSubScript);
               // [CHECK] Is there a single sub-script?
               else if (szFirstSubScript)
                  /// [SUB-SCRIPT] Insert sub-script
                  StringCchPrintf(szParameter, 64, TEXT("«%s%s»"), szParameterSyntax[eParameterSyntax], szFirstSubScript);
               else
                  /// [NO SUB-SCRIPT] Insert without sub-script
                  StringCchPrintf(szParameter, 64, TEXT("«%s»"), szParameterSyntax[eParameterSyntax]);

               /// [BOLD TEXT] Insert parameter without bold
               if (eType == RST_BOLD_TEXT)
               {
                  // [CHECK] Remove Bold
                  if (bBoldState)
                     StringCchCat(szSource, 1024, TEXT("[/b]"));
                  
                  // Add parameter text
                  StringCchCat(szSource, 1024, szParameter);
                  bBoldState = FALSE;
               }
               /// [BOLD PARAMETERS] Embolden parameter only
               else
                  utilStringCchCatf(szSource, 1024, txtBold("%s"), szParameter);
            }
            break;

         /// [WHITESPACE] Display in current state
         case CO_WHITESPACE:
            StringCchCat(szSource, 1024, pCodeObject->szText);
            break;

         /// [SQUARE BRACKETS] Expand for RichText generation
         case CO_OPERATOR:
            switch (pCodeObject->szText[0])
            {
            case '[':  StringCchCat(szSource, 1024, TEXT("\\["));          continue;
            case ']':  StringCchCat(szSource, 1024, TEXT("\\]"));          continue;

            default:   
               // [CHECK] Is this a symbolic or logical operator?
               if (!isStringLogicalOperator(pCodeObject->szText))
               {
                  // [SYMBOL] Output verbatim
                  StringCchCat(szSource, 1024, pCodeObject->szText);
                  continue;
               }
            }
            // [LOGICAL OPERATOR] Fall through...

         /// [WORD/NUMBER] Display in bold
         default:
            /// [BOLD TEXT] Insert words in bold
            if (eType == RST_BOLD_TEXT)
            {
               // [NON-BOLD] Switch to bold
               if (!bBoldState)
                  StringCchCat(szSource, 1024, TEXT("[b]"));

               // Append CodeObject
               StringCchCat(szSource, 1024, pCodeObject->szText);
               bBoldState = TRUE;
            }
            /// [BOLD PARAMETERS] Insert without bold
            else
               StringCchCat(szSource, 1024, pCodeObject->szText);
            break;
         }
      }

      // [CHECK] Close bold tag, if open
      if (bBoldState)
         StringCchCat(szSource, 1024, TEXT("[/b]"));

      // [SPECIAL CASE] Add optional parameters to Script call titles manually
      if (pSyntax->iID == CMD_CALL_SCRIPT_VAR_ARGS AND eType == RST_BOLD_TEXT)
         utilStringCchCatf(szSource, 1024, TEXT(" \\[ ") txtBold("arg1=") TEXT("«%s» ") txtBold("arg2=") TEXT("«%s» ... ") txtBold("argª=") TEXT("«%s» \\]"), szParameterSyntax[PS_VALUE], szParameterSyntax[PS_VALUE], szParameterSyntax[PS_VALUE]);
      else if (pSyntax->iID == CMD_CALL_SCRIPT_VAR_ARGS)
         utilStringCchCatf(szSource, 1024, TEXT(" \\[ arg1=") txtBold("«%s»") TEXT(" arg2=") txtBold("«%s»") TEXT(" ... argª=") txtBold("«%s»") TEXT(" \\]"), szParameterSyntax[PS_VALUE], szParameterSyntax[PS_VALUE], szParameterSyntax[PS_VALUE]);
      break;

   /// [VIRTUAL COMMAND] Manually generate syntax strings
   case CMD_DEFINE_ARRAY:
      if (eType == RST_BOLD_TEXT)
         StringCchPrintf(szSource, 1024, txtBold("dim ") TEXT("«%s»") txtBold(" = ") TEXT("«%s» \\[,«%s» \\] \\[,«%s» \\] ... \\[,«%s» \\]"), szParameterSyntax[PS_ARRAY], szParameterSyntax[PS_VALUE], szParameterSyntax[PS_VALUE], szParameterSyntax[PS_VALUE], szParameterSyntax[PS_VALUE]);
      else 
         StringCchPrintf(szSource, 1024, TEXT("dim ") txtBold("«%s»") TEXT(" = ") txtBold("«%s» \\[,«%s» \\] \\[,«%s» \\] ... \\[,«%s» \\]"), szParameterSyntax[PS_ARRAY], szParameterSyntax[PS_VALUE], szParameterSyntax[PS_VALUE], szParameterSyntax[PS_VALUE], szParameterSyntax[PS_VALUE]);
      break;
   }

   // Generate RichText
   bResult = generateRichTextFromSourceText(szSource, lstrlen(szSource), pOutput, RTT_RICH_TEXT, ST_DISPLAY, pErrorQueue);

   // Cleanup and return result generation result
   deleteCodeObject(pCodeObject);
   utilDeleteStrings(szSource, szParameter);
   return bResult;
}


/// Function name  : insertCommandSyntaxIntoGameData
// Description     : Inserts a CommandSyntax object into the ID and HASH trees
// 
// COMMAND_SYNTAX*  pCommandSyntax : [in/out] Syntax to insert. Variation is alterated if necessary
// 
// Return Value : TRUE if successful, FALSE if object was a duplicate
//
BOOL  insertCommandSyntaxIntoGameData(COMMAND_SYNTAX*  pCommandSyntax)
{
   COMMAND_SYNTAX*  pHashCopy;
   BOOL             bResult;   // Operation result

   // Prepare
   bResult = TRUE;

   /// Attempt to insert into the ID tree
   if (!insertObjectIntoAVLTree(getGameData()->pCommandTreeByID, (LPARAM)pCommandSyntax))
   {
      // [FAILED] Alter variation
      pCommandSyntax->iVariation = 1;

      /// Attempt to insert into the ID tree under alternate variation
      if (!insertObjectIntoAVLTree(getGameData()->pCommandTreeByID, (LPARAM)pCommandSyntax))
         /// [ERROR] Command has more than one variation
         bResult = FALSE;
   }
   
   // [CHECK] Was syntax inserted successfully?
   if (bResult)
   {
      // [SUCCESS] Examine Command
      switch (pCommandSyntax->iID)
      {
      /// [SPECIAL CASE] Don't attempt commands with no hash to the HASH tree
      case CMD_COMMENT:
      case CMD_COMMAND_COMMENT:
      case CMD_ELSE:
      case CMD_EXPRESSION:
      case CMD_NOP:
         break;

      /// [HASH] Insert into the HASH tree
      default:
         // Create a copy of the syntax and reset the variation
         pHashCopy = duplicateCommandSyntax(pCommandSyntax);
         pHashCopy->iVariation = 0;

         /// Attempt to insert into HASH tree
         if (!insertObjectIntoAVLTree(getGameData()->pCommandTreeByHash, (LPARAM)pHashCopy))
         {
            // [FAILED] Alter variation
            pHashCopy->iVariation = 1;

            /// Attempt to insert into HASH tree under alternate version
            if (!insertObjectIntoAVLTree(getGameData()->pCommandTreeByHash, (LPARAM)pHashCopy))
            {
               /// [ERROR] HASH is not unique within variation
               deleteCommandSyntax(pHashCopy);
               bResult = FALSE;
            }
         }
         break;
      }
   }

   // Return result
   return bResult;
}


/// Function name  : isCommandInterruptable
// Description     : Determine whether a command is interruptable from it's syntax
// 
// CONST COMMAND_SYNTAX*  pSyntax   : [in] CommandSyntax object to examine
// 
// Return Value   : TRUE or FALSE
// 
BearScriptAPI
BOOL   isCommandInterruptable(CONST COMMAND_SYNTAX*  pSyntax)
{
   // [CHECK] Return FALSE if the syntax is NULL
   if (pSyntax == NULL)
      return FALSE;

   // Search for the 'interruptable' return object syntax
   for (UINT i = 0; i < pSyntax->iParameterCount; i++)
   {
      switch (pSyntax->eParameters[i])
      {
      case PS_RETURN_OBJECT_IF_START:
      case PS_INTERRUPT_RETURN_OBJECT_IF:
         // [FOUND] - Command is interruptable
         return TRUE;
      }
   }

   // [NOT FOUND] - Command is not interruptable
   return FALSE;
}



/// Function name  : isCommandSyntaxCompatible
// Description     : Checks whether a CommandSyntax object is compatible with a specified game version
// 
// CONST COMMAND_SYNTAX*  pCommandSyntax  : [in] CommandSyntax object to query
// CONST GAME_VERSION     eGameVersion    : [in] GameVersion to test against
// 
// Return Value : TRUE / FALSE
//
BearScriptAPI
BOOL  isCommandSyntaxCompatible(CONST COMMAND_SYNTAX*  pCommandSyntax, CONST GAME_VERSION  eGameVersion)
{
   BOOL  bResult;

   // Examine input version
   switch (eGameVersion)
   {
   /// [THE THREAT] Check for X2 command version flag
   case GV_THREAT:           bResult = (pCommandSyntax->iCompatibility INCLUDES CV_THREAT);           break;

   /// [REUNION] Check for X3 command version flag
   case GV_REUNION:          bResult = (pCommandSyntax->iCompatibility INCLUDES CV_REUNION);          break;

   /// [TERRAN CONFLICT] Check for X3TC command version flag
   case GV_TERRAN_CONFLICT:  bResult = (pCommandSyntax->iCompatibility INCLUDES CV_TERRAN_CONFLICT);  break;

   /// [ALBION PRELUDE] Check for X3AP command version flag
   case GV_ALBION_PRELUDE:   bResult = (pCommandSyntax->iCompatibility INCLUDES CV_ALBION_PRELUDE);   break;

   case GV_UNKNOWN:          bResult = TRUE;   break;
   // [ERROR]
   default: ASSERT(FALSE);   bResult = FALSE;  break;
   }
   
   // Return result
   return bResult;
}


/// Function name  : isCommandReferenceURLPresent
// Description     : Test whether syntax has an MSCI reference entry
// 
// CONST COMMAND_SYNTAX*  pSyntax : [in] Syntax to test
// 
// Return Value   : TRUE or FALSE
// 
BearScriptAPI
BOOL  isCommandReferenceURLPresent(CONST COMMAND_SYNTAX*  pSyntax)
{
   return (pSyntax AND lstrlen(pSyntax->szReferenceURL) != NULL);
}


/// Function name  : loadCommandSyntaxFile
// Description     : Loads the specified syntax file
// 
// GAME_FILE*           pSyntaxFile : [in] Syntax file
// OPERATION_PROGRESS*  pProgress   : [in] 
// ERROR_QUEUE*         pErrorQueue : [in] 
// 
// Return Value   : OR_SUCCESS or OR_FAILURE
// 
OPERATION_RESULT   loadCommandSyntaxFile(GAME_FILE*  pSyntaxFile, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue)
{
   OPERATION_RESULT     eResult = OR_SUCCESS;   // Operation result
   COMMAND_SYNTAX      *pNewSyntax;             // CommandSyntax object being created
   PARAMETER_SYNTAX     eParameterType;         // Convenience pointer for the type of parameter being processed
   ERROR_STACK         *pError;                 // Current error, if any
   TCHAR               *szProperty,             // Current token representing a command property
                       *pIterator,              // Tokeniser data
                       *szProperties[iMaxProperties];    // Properties of the command currently being processed
   UINT                 iPropertyCount,                  // Number of valid properties for the command currently being processed
                        iIndex = 0;

   __try
   {
      // [VERBOSE]
      VERBOSE("Generating CommandSyntax tree from '%s'", identifyGameFileFilename(pSyntaxFile));

      // Prepare
      utilZeroObjectArray(szProperties, CONST TCHAR*, iMaxProperties);
      iPropertyCount = 0;
      pError         = NULL;

      // Tokenise file into lines (each containing a single property)
      for (szProperty = utilTokeniseString(pSyntaxFile->szInputBuffer, "\r\n", &pIterator); szProperty AND eResult == OR_SUCCESS; szProperty = utilGetNextToken("\r\n", &pIterator))
      {
         /// [COMMAND PROPERTY] Add each property to the properties array
         if (!utilCompareStringN(szProperty, "-----", 5))
         {
            ASSERT(iPropertyCount < iMaxProperties);
            szProperties[iPropertyCount++] = szProperty;
         }
         /// [END COMMAND MARKER] Process entire 'properties array' into a new command
         else
         {
            // Create new syntax object
            pNewSyntax = createCommandSyntax();

            /// Extract syntax and ID
            pNewSyntax->iID            = utilSafeConvertStringToInteger(szProperties[CSI_COMMAND_ID]);
            pNewSyntax->szSyntax       = utilDuplicateSimpleString(szProperties[CSI_SYNTAX]);
            pNewSyntax->eGroup         = identifyCommandGroupFromName(szProperties[CSI_GROUP_NAME]); 
            pNewSyntax->szReferenceURL = (utilCompareString(szProperties[CSI_MSCI_LINK], "NONE") ? NULL : utilDuplicateSimpleString(szProperties[CSI_MSCI_LINK]));
            
            // [CUSTOM] Print syntax
            if (!pSyntaxFile->bResourceBased)
               VERBOSE("Loading custom command : '%s'", pNewSyntax->szSyntax);

            // Calculate compatibility 
            pNewSyntax->iCompatibility = identifyCommandCompatibilityFlags(szProperties[CSI_COMPATIBILITY]);
            pNewSyntax->eGameVersion   = calculateCommandSyntaxGameVersion(pNewSyntax);
            
            /// Store parameters (if any)
            for (UINT iProperty = CSI_FIRST_PARAMETER; iProperty < iPropertyCount; iProperty++)
            {
               /// Manually reverse-lookup parameter syntax
               if ((eParameterType = identifyParameterSyntaxFromName(szProperties[iProperty])) == PS_UNDETERMINED)
               {  
                  // [ERROR] The syntax for parameter %u (of %u) of the script command '%s' is '%s', but this could not be resolved using the currently available game strings
                  pushErrorQueue(pErrorQueue, pError = generateDualError(HERE(IDS_SYNTAX_PARAMETER_NOT_FOUND), iProperty, iPropertyCount, pNewSyntax->szContent, szProperties[iProperty]));
                  break;
               }

               // [CHECK] Set appropriate flag if command has a ReturnObject
               if (isReturnObject(eParameterType))
                  pNewSyntax->bMayHaveReturnObject = TRUE;

               /// Store in syntax object
               pNewSyntax->eParameters[iProperty - CSI_FIRST_PARAMETER] = eParameterType;
               pNewSyntax->iParameterCount++;
            }

            // [CUSTOM] Print parameter syntax
            for (UINT  iParam = 0; !pSyntaxFile->bResourceBased AND iParam < pNewSyntax->iParameterCount; iParam++)
               VERBOSE("Command Parameter : '%s'", szProperties[CSI_FIRST_PARAMETER + iParam]);

            /// [SUCCESS] Insert CommandSyntax into ID and HASH trees
            if (!pError)
            {
               // Generate HASH and SUGGESTION TEXT
               pNewSyntax->szHash           = generateCommandSyntaxHash(pNewSyntax);
               pNewSyntax->szContent        = generateCommandSyntaxContentText(pNewSyntax);
               pNewSyntax->szSuggestion = generateCommandSyntaxSuggestionText(pNewSyntax);

               // Generate rich syntax
               generateCommandSyntaxTitle(pNewSyntax, RST_BOLD_TEXT,       pNewSyntax->pDisplaySyntax, pErrorQueue);
               generateCommandSyntaxTitle(pNewSyntax, RST_BOLD_PARAMETERS, pNewSyntax->pTooltipSyntax, pErrorQueue);

               // Insert into game data trees
               if (!insertCommandSyntaxIntoGameData(pNewSyntax))
               {
                  // [FAILED] Duplicate
                  VERBOSE("Error: Unable to add command '%s' due to duplication", pNewSyntax->szSyntax);

                  // [CUSTOM] "Loaded custom script command '%s' %s"
                  if (!pSyntaxFile->bResourceBased)
                     pushErrorQueue(pErrorQueue, generateDualWarning(HERE(IDS_SYNTAX_CUSTOM_COMMAND), pNewSyntax->szSuggestion, TEXT("failed due to non-unique syntax")));

                  // Cleanup
                  deleteCommandSyntax(pNewSyntax);
               }
               // [CUSTOM] "Loaded custom script command '%s' %s"
               else if (!pSyntaxFile->bResourceBased)
                  pushErrorQueue(pErrorQueue, generateDualWarning(HERE(IDS_SYNTAX_CUSTOM_COMMAND), pNewSyntax->szSuggestion, TEXT("successfully")));
            }
            // [ERROR] Abort
            else
            {
               deleteCommandSyntax(pNewSyntax);
               eResult = OR_FAILURE;
            }

            // [PROGRESS] Update operation progress
            if (++iIndex % 10 == 0)
               advanceOperationProgressValue(pProgress);

            // Reset property counter and properties array
            iPropertyCount = 0;
            utilZeroObjectArray(szProperties, CONST TCHAR*, iMaxProperties);

         } // END: if (is end-of-command marker)

      } // END: for (each line/property)
   }
   __except (pushException(pErrorQueue))
   {
      eResult = OR_FAILURE;
   }

   // Cleanup and return result
   return eResult;
}


/// Function name  : loadCommandSyntaxTree
// Description     : Builds the CommandSyntax tree from the resource/custom syntax files
//
// OPERATION_PROGRESS*  pProgress   : [in/out] Operation progress
// ERROR_QUEUE*         pErrorQueue : [out]    Error message, if any
// 
// Operation Stages : ONE
//
// Return type : OR_SUCCESS - CommandSyntax trees were created successfully, if there were any errors the user ignored them
//               OR_FAILURE - CommandSyntax trees were NOT created due to a critical error (corrupt resource library)
//               OR_ABORTED - CommandSyntax trees were NOT created because the user aborted after a non-critical error
//
OPERATION_RESULT   loadCommandSyntaxTree(OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue)
{
   OPERATION_RESULT     eResult = OR_FAILURE; // Operation result
   GAME_FILE           *pSyntaxFile,      // CommandSyntax resource file
                       *pCustomFile;      // Custom CommandSyntax file, if any
   TCHAR               *szCustomPath;     // Path of custom syntax file
   
   __try
   {
      CONSOLE_STAGE();
      CONSOLE("Loading command syntax definitions");

      // [INFO/STAGE] "Loading command syntax for script commands"
      pushErrorQueue(pErrorQueue, generateDualInformation(HERE(IDS_OUTPUT_LOADING_SYNTAX)));
      ASSERT(advanceOperationProgressStage(pProgress) == IDS_PROGRESS_LOADING_COMMAND_SYNTAX);
      updateOperationProgressMaximum(pProgress, 885 / 10);    // Manually define syntax entry count

      // Prepare
      szCustomPath = utilGenerateAppFilePath(TEXT("Custom.Syntax.txt"));
      pSyntaxFile  = createGameFile();

      /// Create commands trees
      getGameData()->pCommandTreeByID   = createCommandSyntaxTreeByID();
      getGameData()->pCommandTreeByHash = createCommandSyntaxTreeByHash();

      // Attempt to syntax definitions file
      if (!loadGameFileFromResource(getResourceInstance(), TEXT("COMMAND_SYNTAX"), pSyntaxFile))
         // No enhancement necessary      // [ERROR] "There was an I/O error while loading the command syntax file from the resource library"
         eResult = OR_FAILURE;            // pushErrorQueue(pErrorQueue, generateDualError(HERE(IDS_SYNTAX_FILE_IO_ERROR)));
      
      else
      {
         /// [RESOURCE] Read main command syntax file from resource library
         eResult = loadCommandSyntaxFile(pSyntaxFile, pProgress, pErrorQueue);

         // [SUCCESS] Check for a custom syntax file
         if (eResult == OR_SUCCESS AND PathFileExists(szCustomPath))
         {
            // [INFO/STAGE] "Loading user generated syntax for custom script commands"
            pushErrorQueue(pErrorQueue, generateDualInformation(HERE(IDS_OUTPUT_LOADING_CUSTOM_SYNTAX)));

            // Attempt to load custom syntax file
            if (loadGameFileFromFileSystemByPath(getFileSystem(), pCustomFile = createGameFileFromAppPath(TEXT("Custom.Syntax.txt")), NULL, pErrorQueue))
               /// [CUSTOM] Read command syntax from custom file
               eResult = loadCommandSyntaxFile(pCustomFile, NULL, pErrorQueue);
            
            // Cleanup
            deleteGameFile(pCustomFile);
         }
      } 

      // [SUCCESS]
      CONSOLE_HEADING("Loaded the syntax for %u script commands and the hashes for %u. (5 Commands have no hash)", getTreeNodeCount(getGameData()->pCommandTreeByID), getTreeNodeCount(getGameData()->pCommandTreeByHash));

      // Cleanup and return result
      utilDeleteString(szCustomPath);
      deleteGameFile(pSyntaxFile);
      return eResult;
   }
   __except (pushException(pErrorQueue))
   {
      EXCEPTION("Unable to the load command syntax");
      return OR_FAILURE;
   }
}



/// Function name  : printMissingSyntax
// Description     : 
// 
BearScriptAPI
VOID  printMissingSyntax()
{
   COMMAND_SYNTAX*  pSyntax;

   performAVLTreeIndexing(getGameData()->pCommandTreeByID);

   for (COMMAND_GROUP  grp = CG_ARRAY; grp < CG_HIDDEN; grp = (COMMAND_GROUP)(grp + 1))
   {
      CONSOLE("Missing %s commands:", loadTempString(IDS_COMMAND_GROUP_ARRAY + grp));

      for (UINT i = 0; findObjectInAVLTreeByIndex(getGameData()->pCommandTreeByID, i, (LPARAM&)pSyntax) ; i++)
      {
         if (pSyntax->eGroup == grp && !pSyntax->pTooltipDescription && !lstrlen(pSyntax->szReferenceURL))
            CONSOLE("  %d - %s", pSyntax->iID, pSyntax->szSyntax);
      }
   }
}


/// Function name  : verifyParameterSyntax
// Description     : Determine if a parameter has the correct type by matching syntax types against data types.
///                                    NOTE: This will need a lot of updating i suspect
//
// CONST DATA_TYPE        eType   : [in] Data type
// CONST PARAMETER_SYNTAX eSyntax : [in] Syntax type
// 
// Return type : TRUE if matches, FALSE if not

BOOL   verifyParameterSyntax(CONST DATA_TYPE  eType, CONST PARAMETER_SYNTAX  eSyntax)
{
   BOOL  bMatch = eSyntax == PS_VALUE;

   // Return TRUE is syntax is PS_VALUE, I assume it means 'any type'
   if (bMatch)
      return TRUE;

   // Otherwise check manually..
   switch (eType)
   {
   // [NULL]
   case DT_NULL:                     /// Provided
      switch (eSyntax)
      {
      case PS_EXPRESSION:               // Expected
      case PS_REFERENCE_OBJECT:         // Expected
      case PS_ARRAY:                    // Expected
      case PS_VARIABLE_RACE:            // Expected
      case PS_VARIABLE_STRING:          // Expected
      case PS_VARIABLE_NUMBER:          // Expected
         bMatch = TRUE;
      }
      break;

   // [UNKNOWN / VARIABLE]
   case DT_UNKNOWN:                   /// Provided
   case DT_VARIABLE:                  /// Provided
      switch (eSyntax)
      {
      case PS_LABEL_NAME:               // Not Allowed
      case PS_LABEL_NUMBER:             // Not Allowed
      case PS_CONDITION:                // Not Allowed
      case PS_PARAMETER:                // Not Allowed
      case PS_COMMENT:                  // Not Allowed
         bMatch = FALSE;
         break;

      default:                           // Expected
         bMatch = TRUE;
         break;
      }
      break;

   // [CONSTANT]
   case DT_CONSTANT:                           /// Provided
      switch (eSyntax)
      {
      case PS_EXPRESSION:                        // Expected
      case PS_NUMBER:                            // Expected
      case PS_VARIABLE_NUMBER:                   // Expected
      case PS_VARIABLE_CONSTANT:                 // Expected
      case PS_VARIABLE_SHIP:                     // Expected
      case PS_VARIABLE_SHIP_STATION:             // Expected
      case PS_VARIABLE_STATION:                  // Expected
      case PS_VARIABLE_PLAYER_SHIP:              // Expected
      case PS_VARIABLE_PLAYER_SHIP_STATION:      // Expected
      case PS_REFERENCE_OBJECT:                  // Expected
      
         bMatch = TRUE;
      }
      break;

   // [INTEGER]
   case DT_INTEGER:                  /// Provided
      switch (eSyntax)
      {
      case PS_EXPRESSION:                 // Expected
      case PS_NUMBER:                     // Expected
      case PS_RETURN_OBJECT:              // Expected
      case PS_RETURN_OBJECT_IF:           // Expected
      case PS_RETURN_OBJECT_IF_START:     // Expected
      case PS_INTERRUPT_RETURN_OBJECT_IF: // Expected
      case PS_VARIABLE_NUMBER:            // Expected
      case PS_VARIABLE:                   // Expected
         
         bMatch = TRUE;
      }
      break;

   // [STRING]
   case DT_STRING:                  /// Provided
      switch (eSyntax)
      {
      case PS_COMMENT:                 // Expected
      case PS_EXPRESSION:              // Expected
      case PS_LABEL_NAME:              // Expected
      case PS_LABEL_NUMBER:            // Expected         // SPECIAL CASE: goto label / gosub, are stored as number but the user must specify them as strings.
      case PS_SCRIPT_NAME:             // Expected
      case PS_STRING:                  // Expected
      case PS_VARIABLE_STRING:         // Expected
         bMatch = TRUE;
      }
      break;

   // [SHIP]
   case DT_SHIP:                              /// Provided
      switch (eSyntax)
      {
      case PS_EXPRESSION:                         // Expected
      case PS_VARIABLE_SHIP:                      // Expected
      case PS_VARIABLE_SHIP_STATION:              // Expected
      case PS_VARIABLE_PLAYER_SHIP:               // Expected
      case PS_VARIABLE_PLAYER_SHIP_STATION:       // Expected
      case PS_VARIABLE_SHIPTYPE:                  // Expected
      case PS_VARIABLE_SHIPTYPE_STATIONTYPE:      // Expected
         bMatch = TRUE;
      }
      break;

   // [STATION]
   case DT_STATION:                           /// Provided
      switch (eSyntax)
      {
      case PS_EXPRESSION:                         // Expected
      case PS_VARIABLE_STATION:                   // Expected
      case PS_VARIABLE_SHIP_STATION:              // Expected
      case PS_VARIABLE_PLAYER_STATION:            // Expected
      case PS_VARIABLE_PLAYER_STATION_CARRIER:    // Expected
      case PS_VARIABLE_PLAYER_SHIP_STATION:       // Expected
      case PS_VARIABLE_STATIONTYPE:               // Expected
      case PS_VARIABLE_SHIPTYPE_STATIONTYPE:      // Expected
         bMatch = TRUE;
      }
      break;

   // [SECTOR]
   case DT_SECTOR:                                 /// Provided
      switch (eSyntax)
      {
      case PS_VARIABLE_SECTOR:                   // Expected
      case PS_EXPRESSION:                        // Expected
         bMatch = TRUE;
      }
      break;

   // [WARE]
   case DT_WARE:                                    /// Provided
      switch (eSyntax)
      {
      case PS_EXPRESSION:                         // Expected
      case PS_VARIABLE_WARE:                      // Expected

      /// NEW: eXscriptor assigns WARE to all of these..
      case PS_VARIABLE_SHIP:                      // Expected
      case PS_VARIABLE_SHIP_STATION:              // Expected
      case PS_VARIABLE_PLAYER_SHIP:               // Expected
      case PS_VARIABLE_PLAYER_SHIP_STATION:       // Expected
      case PS_VARIABLE_SHIPTYPE:                  // Expected
      case PS_VARIABLE_SHIPTYPE_STATIONTYPE:      // Expected
      /// NEW: eXscriptor assigns WARE to all of these..
      case PS_VARIABLE_STATION:                   // Expected
      case PS_VARIABLE_PLAYER_STATION:            // Expected
      case PS_VARIABLE_PLAYER_STATION_CARRIER:    // Expected
      case PS_VARIABLE_STATIONTYPE:               // Expected
      
         bMatch = TRUE;
      }
      break;

   // [RACE]
   case DT_RACE:                                    /// Provided
      switch (eSyntax)
      {
      case PS_EXPRESSION:                        // Expected
      case PS_VARIABLE_RACE:                     // Expected
         bMatch = TRUE;
      }
      break;

   // [STATION SERIAL]
   case DT_STATIONSERIAL:                           /// Provided
      switch (eSyntax)
      {
      case PS_EXPRESSION:                        // Expected
      case PS_VARIABLE_STATIONSERIAL:            // Expected
         bMatch = TRUE;
      }
      break;

   // [OBJECT CLASS]
   case DT_OBJECTCLASS:                              /// Provided
      switch (eSyntax)
      {
      case PS_EXPRESSION:                        // Expected
      case PS_VARIABLE_CLASS:                    // Expected
         bMatch = TRUE;
      }
      break;

   // [TRANSPORT CLASS]
   case DT_TRANSPORTCLASS:                           /// Provided
      switch (eSyntax)
      {
      case PS_EXPRESSION:                        // Expected
      case PS_VARIABLE_TRANSPORTCLASS:           // Expected
         bMatch = TRUE;
      }
      break;

   // [RELATION]
   case DT_RELATION:                                 /// Provided
      switch (eSyntax)
      {
      case PS_EXPRESSION:                        // Expected
      case PS_RELATION:                          // Expected
         bMatch = TRUE;
      }
      break;

   // [OBJECT COMMAND]
   case DT_OBJECTCOMMAND:                           /// Provided
      switch (eSyntax)
      {
      case PS_EXPRESSION:                        // Expected
      case PS_OBJECTCOMMAND:                     // Expected
      case PS_OBJECTCOMMAND_OBJECTSIGNAL:        // Expected
         bMatch = TRUE;
      }
      break;

   // [FLIGHT RETURN]
   case DT_FLIGHTRETURN:                           /// Provided
      switch (eSyntax)
      {
      case PS_EXPRESSION:                        // Expected
      case PS_FLIGHTRETCODE:                     // Expected
         bMatch = TRUE;
      }
      break;

   // [DATATYPE]
   case DT_DATATYPE:                               /// Provided
      switch (eSyntax)
      {
      case PS_EXPRESSION:                        // Expected
      case PS_VARIABLE_DATATYPE:                 // Expected
         bMatch = TRUE;
      }
      break;

   // [ARRAY]
   case DT_ARRAY:                                   /// Provided
      bMatch = eSyntax == PS_ARRAY;              // Expected
      break;

   // [QUEST]
   case DT_QUEST:                                   /// Provided
      switch (eSyntax)
      {
      case PS_EXPRESSION:                        // Expected
      case PS_VARIABLE_QUEST:                    // Expected
         bMatch = TRUE;
      }
      break;

   // [EXPRESSION / OBJECT / OPERATOR]
   case DT_EXPRESSION:                               /// Provided
   case DT_OBJECT:                                   /// Provided
   case DT_OPERATOR:                                 /// Provided
      ASSERT(FALSE)                              // Never Allowed
      break;
   }

   return bMatch;
}


