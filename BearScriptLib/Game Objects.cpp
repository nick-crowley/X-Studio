//
// Object Names.cpp  : Parses the Type-Files and generates display names / GameString trees for wares/ships/stations
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////

// Maximum number of properties to read from any T-File object. (TShips entries can have a huge number, but most are irrelevant)
//
CONST UINT           iMaxValidProperties   = 1024;    

// Number of T-Files to process  
CONST UINT           iObjectNameFileCount  = 12;

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                  CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createGameObject
// Description     : Cfreates a GameObject from a combination of SubType:MainType and object text
// 
// CONST MAIN_TYPE  eMainType  : [in] MainType (Used as the primary key)
// CONST UINT       iSubType   : [in] SubType (Used as the secondary key)
// CONST TCHAR*     szText     : [in] Object text
// CONST TCHAR*     szObjectID : [in] Unique ID of the object, as defined by the game
// 
// Return Value   : New GameObject, you are responsible for destroying it
// 
GAME_OBJECT*  createGameObject(CONST MAIN_TYPE  eMainType, CONST UINT  iSubType, CONST TCHAR*  szText, CONST TCHAR*  szObjectID)
{
   GAME_OBJECT*  pGameObject;  // Object being created

   // Prepare
   pGameObject = utilCreateEmptyObject(OBJECT_NAME);
   
   // Set properties
   pGameObject->eType     = ONT_GAME;
   pGameObject->eGroup    = identifyObjectNameGroupFromMainType(eMainType);
   pGameObject->eMainType = eMainType;
   pGameObject->iID       = iSubType;

   // Store text and object ID
   pGameObject->iCount     = lstrlen(szText);
   pGameObject->szText     = utilDuplicateString(szText, pGameObject->iCount);
   pGameObject->szObjectID = utilDuplicateString(szObjectID, lstrlen(szObjectID));

   // Return new object
   return pGameObject;
}


/// Function name  : createGameObjectTreeByMainType
// Description     : Creates the GameObject tree organised by GROUP and then ID
// 
// Return Value   : New AVL Tree, you are responsible for destroying it
// 
AVL_TREE*   createGameObjectTreeByMainType()
{
   // Create storage tree
   return createAVLTree(extractGameObjectTreeNode, deleteObjectNameTreeNode, createAVLTreeSortKey(AK_MAINTYPE, AO_ASCENDING), createAVLTreeSortKey(AK_ID, AO_ASCENDING), NULL);
}


/// Function name  : createGameObjectTreeByText
// Description     : Creates the GameObject reverse-lookup copy tree, organised by TEXT
// 
// Return Value   : New AVL Tree, you are responsible for destroying it
// 
AVL_TREE*   createGameObjectTreeByText()
{
   // Create storage tree
   return createAVLTree(extractGameObjectTreeNode, NULL, createAVLTreeSortKey(AK_TEXT, AO_ASCENDING), NULL, NULL);
}


/// Function name  : createGameObjectTreeForCollisions
// Description     : Creates the GameObject copy tree organised by GROUP, then ID, for temporarily holding collisions
// 
// Return Value   : New AVL Tree, you are responsible for destroying it
// 
AVL_TREE*   createGameObjectTreeForCollisions()
{
   // Create copy tree
   return createAVLTree(extractGameObjectTreeNode, NULL, createAVLTreeSortKey(AK_MAINTYPE, AO_ASCENDING), createAVLTreeSortKey(AK_ID, AO_ASCENDING), NULL);
}


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                         HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : convertStationSizeToStringID
// Description     : Convert the ID used in TFiles for a StationSize to a GameString ID
// 
// CONST UINT  iStationSize : [in] Station Size ID
// 
// Return type : GameString ID
//
UINT  convertStationSizeToStringID(CONST UINT  iStationSize)
{
   switch (iStationSize)
   {
   // [MEDIUM]
   case SS_MEDIUM:
      return 502;
   // [LARGE]
   case SS_LARGE:
      return 503;
   // [EXTRA-LARGE]
   case SS_EXTRA_LARGE:
      return 504;
   // [EXTRA-LARGE]
   case SS_EXTRA_EXTRA_LARGE:
      return 505;
   // [ERROR] - Invalid input
   default:
      //ASSERT(FALSE); 
      return NULL;
   }
}

/// Function name  : convertVariationIDToStringID
// Description     : Convert variation ID used in TFile into it's game string ID 
//
// CONST UINT  iVariationID : [in] Ship Variation ID
// 
// Return type : StringID or NULL
//
UINT  convertVariationIDToStringID(CONST UINT  iVariationID)
{
   // Variation strings start at 10,000
   return iVariationID + 10000;
}


/// Function name  : extractGameObjectTreeNode
// Description     : Returns the value of the specified property in an AVLTree node containing a GameObject
// 
// LPARAM                   pObject    : [in] GAME_OBJECT* : TreeNode data containing a GameObject
// CONST AVL_TREE_SORT_KEY  eProperty  : [in] Property to retrieve
// 
// Return Value   : Desired property value or NULL if the property is unsupported
// 
BearScriptAPI
LPARAM   extractGameObjectTreeNode(LPARAM  pObject, CONST AVL_TREE_SORT_KEY  eProperty)
{
   GAME_OBJECT*  pGameObject;      // Convenience pointer
   LPARAM        xOutput;          // Property value

   // Prepare
   pGameObject = (GAME_OBJECT*)pObject;

   // Examine property
   switch (eProperty)
   {
   case AK_ID:       xOutput = (LPARAM)pGameObject->iID;        break;
   case AK_GROUP:    xOutput = (LPARAM)pGameObject->eGroup;     break;
   case AK_MAINTYPE: xOutput = (LPARAM)pGameObject->eMainType;  break;
   case AK_TEXT:     xOutput = (LPARAM)pGameObject->szText;     break;

   // [UNSUPPORTED] Error
   default:          xOutput = NULL;  ASSERT(FALSE);            break;
   }

   // Return value
   return xOutput;
}


/// Function name  : findGameObjectByIndex
// Description     : Searches for a GameObject at the specified index within the reverse-lookup tree
// 
// CONST UINT     iIndex  : [in]  Zero-based item index
// GAME_OBJECT*  &pOutput : [out] GameObject if found, otherwise NULL
// 
// Return Value : TRUE if the object was found, FALSE otherwise
//
BOOL   findGameObjectByIndex(CONST UINT  iIndex, GAME_OBJECT*  &pOutput)
{
   // [CHECK] Ensure tree is indexed
   ASSERT(getGameData()->pGameObjectsByText->bIndexed);

   // Query tree
   return findObjectInAVLTreeByIndex(getGameData()->pGameObjectsByText, iIndex, (LPARAM&)pOutput);
}


/// Function name  : findGameObjectByText
// Description     : Searches the GameObject reverse-lookup tree for a GameObject with a specified text
// 
// CONST TCHAR*  szSearchString : [in]  Search string
// GAME_OBJECT* &pOutput        : [out] GameObject if found, otherwise NULL
// 
// Return Value : TRUE if found, FALSE if not
//
BearScriptAPI 
BOOL   findGameObjectByText(CONST TCHAR*  szSearchString, GAME_OBJECT*  &pOutput)
{
   // Query names tree using search string
   return findObjectInAVLTreeByValue(getGameData()->pGameObjectsByText, (LPARAM)szSearchString, NULL, (LPARAM&)pOutput);
}


/// Function name  : findGameObjectByWare
// Description     : Searches for a GameObject that matches the specified ware, encoded for use in scripts
// 
// CONST INT      iPackedWareID : [in]  Packed 'Ware' with MainType in HIWORD and SubType in LOWORD.
// GAME_OBJECT*  &pOutput       : [out] GameObject if found, otherwise NULL
// 
// Return Value : TRUE if the object was found, FALSE if not
//
BOOL   findGameObjectByWare(CONST INT  iPackedWareID, GAME_OBJECT*  &pOutput)
{
   UINT   iMainType,   // MainType of the input ware
          iSubType;    // SubType of the input ware

   // Extract maintype and subtype
   iMainType = HIWORD(iPackedWareID);
   iSubType  = LOWORD(iPackedWareID);

   // Query GameObjects by groups tree
   return findObjectInAVLTreeByValue(getGameData()->pGameObjectsByMainType, iMainType, iSubType, (LPARAM&)pOutput);
}


/// Function name  : findMainTypeGameStringByID
// Description     : Lookup the GameString name of a main type 
// 
// CONST MAIN_TYPE  eMainType : [in]  MainType
// GAME_STRING*    &pOutput   : [out] Main type GameString
// 
// Return Value   : TRUE if found, otherwise FALSE
// 
BOOL  findMainTypeGameStringByID(CONST MAIN_TYPE  eMainType, GAME_STRING*  &pOutput)
{
   // Lookup main type
   return findGameStringByID(200 + eMainType, GPI_CONSTANTS, pOutput);
}

/// Function name  : identifyObjectNameGroupFromMainType
// Description     : Identifies a GameObject's group from it's MainType, which is used as the primary sort key
// 
// CONST MAIN_TYPE  eMainType   : [in] MainType to convert
// 
// Return Value   : Game/Script object group
// 
OBJECT_NAME_GROUP  identifyObjectNameGroupFromMainType(CONST MAIN_TYPE  eMainType)
{
   OBJECT_NAME_GROUP  eOutput;      // Operation result

   // Examine MainType
   switch (eMainType)
   {
   case MT_DOCK:              eOutput = ONG_DOCK;        break;
   case MT_FACTORY:           eOutput = ONG_FACTORY;     break;
   case MT_SHIP:              eOutput = ONG_SHIP;        break;
   case MT_WARE_LASER:        eOutput = ONG_LASER;       break;
   case MT_WARE_SHIELD:       eOutput = ONG_SHIELD;      break;
   case MT_WARE_MISSILE:      eOutput = ONG_MISSILE;     break;
   case MT_WARE_ENERGY:
   case MT_WARE_NATURAL:
   case MT_WARE_BIOLOGICAL:
   case MT_WARE_FOODSTUFF:
   case MT_WARE_MINERAL:
   case MT_WARE_TECHNOLOGY:   eOutput = ONG_WARE;        break;
   
   // [UNSUPPORTED] Error
   default:    ASSERT(FALSE); eOutput = ONG_WARE;        break;
   }

   // Return group
   return eOutput;
}


/// Function name  :  identifyMainTypeString
// Description     : Identifies the plural form of the objects being represented by a main type
// 
// CONST MAIN_TYPE  eMainType   : [in] Main type
// 
// Return Value   : String representing the type of objects represented by a main type
// 
CONST TCHAR*  identifyMainTypeString(CONST MAIN_TYPE  eMainType)
{
   CONST TCHAR*  szOutput;

   // Examine object name group
   switch (identifyObjectNameGroupFromMainType(eMainType))
   {
   case ONG_DOCK:    szOutput = TEXT("docks");      break;
   case ONG_FACTORY: szOutput = TEXT("factories");  break;
   case ONG_SHIP:    szOutput = TEXT("ships");      break;
   case ONG_LASER:   szOutput = TEXT("lasers");     break;
   case ONG_SHIELD:  szOutput = TEXT("shields");    break;
   case ONG_MISSILE: szOutput = TEXT("missiles");   break;
   case ONG_WARE:    szOutput = TEXT("wares");      break;
   }

   return szOutput;
}


/// Function name  : identifyTFileFromMainType
// Description     : Retrieves the sub-path of the T-File associated with a given Main Type. 
//                      Used for reporting the errors in GameObject generation from T-Files
// 
// CONST MAIN_TYPE  eMainType : [in] MainType
// 
// Return Value   : Sub-Path of the relevant T-File
// 
CONST TCHAR*  identifyTFileFromMainType(CONST MAIN_TYPE  eMainType)
{
   CONST TCHAR*  szSubPath;

   // [CHECK] Does index relate to a T-File?
   ASSERT(eMainType >= MT_DOCK AND eMainType <= MT_WARE_TECHNOLOGY);

   /// Lookup sub-path
   if (szSubPath = findGameObjectDataFileSubPathByMainType(eMainType, getAppPreferences()->eGameVersion))
      // [FOUND] Return T-File subpath
      szSubPath = PathFindFileName(szSubPath);

   // Return result
   return szSubPath;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : generateGameObjectShipName
// Description     : Generates the proper ship name as displayed within the game, using the NAME, RACE and VARIATION. OBJECT ID may be appended in special cirumstances
// 
// CONST TCHAR*  szNameID      : [in]  Object declaration Name ID property
// CONST TCHAR*  szRaceID      : [in]  Object declaration Race ID property
// CONST TCHAR*  szVariationID : [in]  Object declaration Variation ID property
// CONST TCHAR*  szObjectID    : [in]  Object ID property
// CONST UINT    iLineNumber   : [in]  [ERROR] Line number within the T-file
// ERROR_STACK*  &pError       : [out] [ERROR] Error message if any, otherwise NULL
// 
// Return Value   : New string containing the name of the ship if successful, otherwise NULL. You are responsible for destroying it
//                   Fails if there is a missing declaration property or invalid property value
// 
TCHAR*  generateGameObjectShipName(CONST TCHAR*  szNameID, CONST TCHAR*  szRaceID, CONST TCHAR*  szVariationID, CONST TCHAR*  szObjectID, CONST UINT  iLineNumber, ERROR_STACK*  &pError)
{
   GAME_STRING  *pNameString,        // Lookup GameString for the object's name
                *pRaceString,        // Lookup GameString for the object's race
                *pVariationString;   // Lookup Game for the 'Variation' or 'Station size
   UINT          iNameID,            // For clarity
                 iRaceID,            // For clarity
                 iVariationID;       // For clarity
   TCHAR        *szShipName;         // Operation result
   CONST TCHAR  *szObjectIDTail;     // Last four characters of the ObjectID -- used to identify Pirate ship declarations

   // Prepare
   szShipName  = utilCreateEmptyString(COMPONENT_LENGTH);
   pRaceString = NULL;
   pError      = NULL;

   /// [CHECK] Ensure the RaceID, NameID and VariationID properties are present
   if (!szNameID OR !szRaceID OR !szVariationID)
      // [ERROR] "There are insufficient properties to define the %s on line %u of types definition file '%s'"
      pError = generateDualError(HERE(IDS_TFILE_INSUFFICIENT_PROPERTIES), TEXT("ship"), iLineNumber, identifyTFileFromMainType(MT_SHIP));
   
   else
   {
      // [SUCCESS] Prepare
      iNameID      = utilConvertStringToInteger(szNameID);
      iRaceID      = utilConvertStringToInteger(szRaceID);
      iVariationID = utilConvertStringToInteger(szVariationID);

      // [CHECK] Is this object intentionally un-defined?
      if (iNameID == SGOI_UNDEFINED_NAME OR iNameID == 0 OR iRaceID == SGOI_UNDEFINED_RACE OR iRaceID == 0)
      {
         /// [UNDEFINED] Display all un-defined objects using a standard formatting : "*** UNDEFINED *** (Object ID)"
         findGameStringByID(SGOI_UNDEFINED_NAME, GPI_SHIPS_STATIONS_WARES, pNameString);
         StringCchPrintf(szShipName, COMPONENT_LENGTH, TEXT("%s (%s)"), (pNameString ? pNameString->szText : TEXT("*** UNDEFINED ***")), szObjectID);
      }
      // [CHECK] Is this object intentionally described as an 'Unknown Object'?
      else if (iNameID == SGOI_UNKNOWN_OBJECT_1 OR iNameID == SGOI_UNKNOWN_OBJECT_2 OR iNameID == SGOI_UNKNOWN_OBJECT_3)
      {
         /// [UNKNOWN OBJECT] Display all 'Unknown Objects' using a standard formatting : "Unknown Object (Object ID)"
         findGameStringByID(SGOI_UNKNOWN_OBJECT_1, GPI_SHIPS_STATIONS_WARES, pNameString);
         StringCchPrintf(szShipName, COMPONENT_LENGTH, TEXT("%s (%s)"), (pNameString ? pNameString->szText : TEXT("Unknown Object")), szObjectID);
      }
      else
      {
         // Extract the last four characters of the ObjectID
         szObjectIDTail = utilFindStringRightN(szObjectID, 4);

         // [CHECK] Is the ship defined as a 'Pirate' vessel?
         if (utilCompareString(&szObjectIDTail[2], "_P") OR
             utilCompareString(szObjectIDTail, "_P_1") OR
             utilCompareString(szObjectIDTail, "_P_2") OR
             utilCompareString(szObjectIDTail, "_P_3"))
         {
            /// [PIRATE] Display all Pirate vessels using a standard formatting : "Pirate <Ship name>"
            StringCchCopy(szShipName, COMPONENT_LENGTH, TEXT("Pirate"));
         }
         // [CHECK] Is the ship's RaceID valid?
         else if (findGameStringByID(iRaceID, GPI_RACES, pRaceString))
            /// [RACE] Copy the Race to the ship name
            StringCchCopy(szShipName, COMPONENT_LENGTH, pRaceString->szText);
         else
         {
            // [WARNING] "No matching string for race %s of the ship '%s' on line %u of object definitions file '%s'"
            pError = generateDualWarning(HERE(IDS_TFILE_SHIP_RACE_NOT_FOUND), szRaceID, szObjectID, iLineNumber, identifyTFileFromMainType(MT_SHIP));
            
            /// [UNDEFINED] Display all un-defined objects using a standard formatting : "*** UNDEFINED *** (Object ID)"
            findGameStringByID(SGOI_UNDEFINED_NAME, GPI_SHIPS_STATIONS_WARES, pNameString);
            StringCchPrintf(szShipName, COMPONENT_LENGTH, TEXT("%s (%s)"), (pNameString ? pNameString->szText : TEXT("*** UNDEFINED ***")), szObjectID);
         }
         
         // [CHECK] Abort further processing if the RaceID was invalid
         if (pError == NULL)
         {
            // [CHECK] Is the ship's NameID valid?
            if (!findGameStringByID(iNameID, GPI_SHIPS_STATIONS_WARES, pNameString))
            {
               // [WARNING] "No matching string for name %s of the ship '%s' on line %u of object definitions file '%s'"
               pError = generateDualWarning(HERE(IDS_TFILE_SHIP_NAME_NOT_FOUND), szNameID, szObjectID, iLineNumber, identifyTFileFromMainType(MT_SHIP));
               
               /// [UNDEFINED] Display all un-defined objects using a standard formatting : "*** UNDEFINED *** (Object ID)"
               findGameStringByID(SGOI_UNDEFINED_NAME, GPI_SHIPS_STATIONS_WARES, pNameString);
               StringCchPrintf(szShipName, COMPONENT_LENGTH, TEXT("%s (%s)"), (pNameString ? pNameString->szText : TEXT("*** UNDEFINED ***")), szObjectID);
            }
            // [CHECK] Is the Name prefaced with the Race?
            else if (pRaceString AND utilCompareStringVariablesN(pNameString->szText, pRaceString->szText, pRaceString->iCount))
               /// [RACE PREFIX] Use this as the ship name verbatim
               StringCchCopy(szShipName, COMPONENT_LENGTH, pNameString->szText);
            else
               /// [NAME] Append the Name to the ship name
               utilStringCchCatf(szShipName, COMPONENT_LENGTH, TEXT(" %s"), pNameString->szText);
            
            // [CHECK] Was the NameID valid?  Is the variation ID present and compatible?
            if (!pError AND iVariationID AND iVariationID != UGC_SPECIAL_VARIATION)
            {
               // [CHECK] Is the Variation ID valid?
               if (findGameStringByID(convertVariationIDToStringID(iVariationID), GPI_SHIPS_STATIONS_WARES, pVariationString))
                  /// [VARIATION] Append the Variation to the ship name
                  utilStringCchCatf(szShipName, COMPONENT_LENGTH, TEXT(" %s"), pVariationString->szText);
               else
               {
                  // [WARNING] "No matching string for variation %s of the ship '%s' on line %u of object definitions file '%s'"
                  pError = generateDualWarning(HERE(IDS_TFILE_SHIP_VARIATION_NOT_FOUND), szVariationID, szObjectID, iLineNumber, identifyTFileFromMainType(MT_SHIP));

                  /// [UNDEFINED] Display all un-defined objects using a standard formatting : "*** UNDEFINED *** (Object ID)"
                  findGameStringByID(SGOI_UNDEFINED_NAME, GPI_SHIPS_STATIONS_WARES, pNameString);
                  StringCchPrintf(szShipName, COMPONENT_LENGTH, TEXT("%s (%s)"), (pNameString ? pNameString->szText : TEXT("*** UNDEFINED ***")), szObjectID);
               }
            } // END: Invalid NameID check

         } // END: Invalid RaceID check
         
      } // END: Undefined / Unknown Object check

   } // END: Invalid declaration check

   // [ERROR] Cleanup and return Ship Name / NULL
   if (isError(pError))
      utilDeleteString(szShipName);
   return szShipName;
}



/// Function name  : generateGameObjectStationName
// Description     : Generates the proper station name as displayed within the game, using the NAME, RACE and SIZE. OBJECT ID may be appended in special cirumstances
// 
// CONST TCHAR*    szNameID    : [in]  Object declaration Name ID property
// CONST TCHAR*    szRaceID    : [in]  Object declaration Race ID property
// CONST TCHAR*    szSizeID    : [in]  Object declaration Size ID property  [Ignored for Docks]
// CONST TCHAR*    szObjectID  : [in]  Object ID property
// CONST MAIN_TYPE eMainType   : [in]  Whether object is a Dock or Station
// CONST UINT      iLineNumber : [in]  [ERROR] Line number within the T-file
// ERROR_STACK*   &pError      : [out] [ERROR] Error message if any, otherwise NULL
// 
// Return Value   : New string containing the name of the ship if successful, otherwise NULL. You are responsible for destroying it
//                   Fails if there is a missing declaration property or invalid property value
// 
TCHAR*  generateGameObjectStationName(CONST TCHAR*  szNameID, CONST TCHAR*  szRaceID, CONST TCHAR*  szSizeID, CONST TCHAR*  szObjectID, CONST MAIN_TYPE  eMainType, CONST UINT  iLineNumber, ERROR_STACK*  &pError)
{
   GAME_STRING  *pNameString,        // Lookup for the object's name
                *pRaceString,        // Lookup for the object's race
                *pSizeString;        // [FACTORY] Lookup for the object's size
   UINT          iNameID,            // For clarity
                 iRaceID,            // For clarity
                 iSizeID;            // For clarity
   TCHAR        *szStationName;      // Operation result
   CONST TCHAR  *szCustomRace;       // Used to detect whether stations names are prefixed with the word 'Aldrin' or not

   // Prepare
   szStationName = utilCreateEmptyString(COMPONENT_LENGTH);
   szCustomRace  = NULL;
   pRaceString   = NULL;
   pError        = NULL;

   /// [CHECK] Ensure the RaceID, NameID and VariationID properties are present
   if (!szNameID OR !szRaceID OR !szSizeID)
      // [ERROR] "There are insufficient properties to define the %s on line %u of types definition file '%s'"
      pError = generateDualError(HERE(IDS_TFILE_INSUFFICIENT_PROPERTIES), TEXT("station"), iLineNumber, identifyTFileFromMainType(eMainType));
   
   else
   {
      // [SUCCESS] Prepare
      iNameID = utilConvertStringToInteger(szNameID);
      iRaceID = utilConvertStringToInteger(szRaceID);
      iSizeID = utilConvertStringToInteger(szSizeID);

      // [CHECK] Is this object intentionally un-defined?
      if (iNameID == SGOI_UNDEFINED_NAME OR iRaceID == SGOI_UNDEFINED_RACE OR iNameID == 0)
      {
         /// [UNDEFINED] Display all un-defined objects using a standard formatting : "*** UNDEFINED *** (Object ID)"
         findGameStringByID(SGOI_UNDEFINED_NAME, GPI_SHIPS_STATIONS_WARES, pNameString);
         StringCchPrintf(szStationName, COMPONENT_LENGTH, TEXT("%s (%s)"), (pNameString ? pNameString->szText : TEXT("*** UNDEFINED ***")), szObjectID);
      }
      // [CHECK] Is the station's Name missing?
      else if (!findGameStringByID(iNameID, GPI_SHIPS_STATIONS_WARES, pNameString))
      {
         // [WARNING] "No matching string for name %s of the station '%s' on line %u of object definitions file '%s'"
         pError = generateDualWarning(HERE(IDS_TFILE_STATION_NAME_NOT_FOUND), szNameID, szObjectID, iLineNumber, identifyTFileFromMainType(eMainType));

         /// [UNDEFINED] Display all un-defined objects using a standard formatting : "*** UNDEFINED *** (Object ID)"
         findGameStringByID(SGOI_UNDEFINED_NAME, GPI_SHIPS_STATIONS_WARES, pNameString);
         StringCchPrintf(szStationName, COMPONENT_LENGTH, TEXT("%s (%s)"), (pNameString ? pNameString->szText : TEXT("*** UNDEFINED ***")), szObjectID);
      }
      // [CHECK] Is the station's RaceID valid?
      else if ((iRaceID AND !findGameStringByID(iRaceID, GPI_RACES, pRaceString)) AND !utilFindSubString(szObjectID, "_LC_"))
      {
         // [WARNING] "No matching string for race %s of the station '%s' on line %u of object definitions file '%s'"
         pError = generateDualWarning(HERE(IDS_TFILE_STATION_RACE_NOT_FOUND), szRaceID, szObjectID, iLineNumber, identifyTFileFromMainType(eMainType));
         
         /// [UNDEFINED] Display all un-defined objects using a standard formatting : "*** UNDEFINED *** (Object ID)"
         findGameStringByID(SGOI_UNDEFINED_NAME, GPI_SHIPS_STATIONS_WARES, pNameString);
         StringCchPrintf(szStationName, COMPONENT_LENGTH, TEXT("%s (%s)"), (pNameString ? pNameString->szText : TEXT("*** UNDEFINED ***")), szObjectID);
      }
      else 
      {
         /// [RACE] Extract race and check for 'Aldrin' manually 
         if (utilFindSubString(szObjectID, "_LC_"))
            szCustomRace = TEXT("Aldrin");
         else if (pRaceString)
            szCustomRace = pRaceString->szText;
  
         // [CHECK] Is the Name prefaced with the Race?
         if (!szCustomRace OR utilCompareStringVariablesN(pNameString->szText, szCustomRace, lstrlen(szCustomRace)))
            /// [RACE PREFIXED] Use this as the output verbatim
            StringCchCopy(szStationName, COMPONENT_LENGTH, pNameString->szText);
         else
            /// [JUST NAME] Append the Name to the output
            StringCchPrintf(szStationName, COMPONENT_LENGTH, TEXT("%s %s"), szCustomRace, pNameString->szText);
         
         // [CHECK] Is this object intentionally described as an 'Unknown Object'?
         if (iNameID == SGOI_UNKNOWN_OBJECT_1 OR iNameID == SGOI_UNKNOWN_OBJECT_2 OR iNameID == SGOI_UNKNOWN_OBJECT_3)
         {
            /// [UNKNOWN OBJECT] Display all 'Unknown Objects' using a standard formatting : "<Race> Unknown Object (Object ID)"
            //findGameStringByID(SGOI_UNKNOWN_OBJECT_1, GPI_SHIPS_STATIONS_WARES, pNameString);
            utilStringCchCatf(szStationName, COMPONENT_LENGTH, TEXT(" (%s)"), szObjectID);
         }

         // [FACTORY] Resolve the SIZE description if present
         if (eMainType == MT_FACTORY AND iSizeID != SS_NONE)
         {
            // Attempt to lookup SIZE description
            if (findGameStringByID(convertStationSizeToStringID(iSizeID), GPI_STATION_SERIALS, pSizeString))
               /// [SIZE] Append the SIZE to the output
               utilStringCchCatf(szStationName, COMPONENT_LENGTH, TEXT(" %s"), pSizeString->szText);
            else
            {
               // [WARNING] "No matching string for size %s of the station '%s' on line %u in object definitions file '%s'"
               pError = generateDualWarning(HERE(IDS_TFILE_STATION_SIZE_NOT_FOUND), szSizeID, szObjectID, iLineNumber, identifyTFileFromMainType(eMainType));
               /// [MISSING] Display all un-defined objects using a standard formatting : "*** UNDEFINED *** (Object ID)"
               findGameStringByID(SGOI_UNDEFINED_NAME, GPI_SHIPS_STATIONS_WARES, pNameString);
               StringCchPrintf(szStationName, COMPONENT_LENGTH, TEXT("%s (%s)"), (pNameString ? pNameString->szText : TEXT("*** UNDEFINED ***")), szObjectID);
            }
         }
         
      } // END: Undefined / Unknown Object check

   } // END: Invalid declaration check

   // [ERROR] Cleanup and return Station Name / NULL
   if (isError(pError)) 
      utilDeleteString(szStationName);
   return szStationName;
}


/// Function name  : generateGameObjectWareName
// Description     : Resolves the ware name as displayed within the game
// 
// CONST TCHAR*    szNameID     : [in]  Object declaration Name ID property
// CONST TCHAR*    szObjectID   : [in]  Object ID property
// CONST MAIN_TYPE eMainType    : [in]  Whether ware is a Laser, Missile, or one of the various types of ware
// CONST UINT      iLineNumber  : [in]  [ERROR] Line number within the T-file
// ERROR_STACK*   &pError       : [out] [ERROR] Error message if any, otherwise NULL
// 
// Return Value   : New string containing the name of the ship if successful, otherwise NULL. You are responsible for destroying it
//                   Fails if there is a missing declaration property or invalid property value
// 
TCHAR*  generateGameObjectWareName(CONST TCHAR*  szNameID, CONST TCHAR*  szObjectID, CONST MAIN_TYPE  eMainType, CONST UINT  iLineNumber, ERROR_STACK*  &pError)
{
   GAME_STRING  *pNameString;     // Lookup for the object's name
   UINT          iNameID;         // For clarity
   TCHAR        *szWareName;      // Operation result

   // Prepare
   szWareName = utilCreateEmptyString(COMPONENT_LENGTH);
   pError     = NULL;

   /// [CHECK] Ensure the NameID is present
   if (!szNameID)
      // [ERROR] "There are insufficient properties to define the %s on line %u of types definition file '%s'"
      pError = generateDualError(HERE(IDS_TFILE_INSUFFICIENT_PROPERTIES), TEXT("ware"), iLineNumber, identifyTFileFromMainType(eMainType));
   
   else
   {
      // [SUCCESS] Prepare
      iNameID = utilConvertStringToInteger(szNameID);

      // [CHECK] Is this object intentionally un-defined?
      if (iNameID == SGOI_UNDEFINED_NAME OR iNameID == 0)
      {
         /// [UNDEFINED] Display all un-defined objects using a standard formatting : "*** UNDEFINED *** (Object ID)"
         findGameStringByID(SGOI_UNDEFINED_NAME, GPI_SHIPS_STATIONS_WARES, pNameString);
         StringCchPrintf(szWareName, COMPONENT_LENGTH, TEXT("%s (%s)"), (pNameString ? pNameString->szText : TEXT("*** UNDEFINED ***")), szObjectID);
      }
      // [CHECK] Is the ware's NameID valid?
      else if (!findGameStringByID(iNameID, GPI_SHIPS_STATIONS_WARES, pNameString))
      {
         // [WARNING] "No matching string for name %s of the ware '%s' on line %u of object definitions file '%s'"
         pError = generateDualWarning(HERE(IDS_TFILE_WARE_NAME_NOT_FOUND), szNameID, szObjectID, iLineNumber, identifyTFileFromMainType(eMainType));

         /// [UNDEFINED] Display all un-defined objects using a standard formatting : "*** UNDEFINED *** (Object ID)"
         findGameStringByID(SGOI_UNDEFINED_NAME, GPI_SHIPS_STATIONS_WARES, pNameString);
         StringCchPrintf(szWareName, COMPONENT_LENGTH, TEXT("%s (%s)"), (pNameString ? pNameString->szText : TEXT("*** UNDEFINED ***")), szObjectID);
      }
      else
         /// [NAME] Set the Name as the output
         StringCchCopy(szWareName, COMPONENT_LENGTH, pNameString->szText);

   } // END: Invalid declaration check

   // [CHECK] Only return NULL if error was serious
   if (isError(pError)) 
      utilDeleteString(szWareName);

   // Cleanup and return
   return szWareName;
}


/// Function name  : generateGameObjectsFromTypeFile
// Description     : Generates game/script ObjectNames from an open T-File and adds them to the ObjectName game data.
//
///                     NB: Because these require multiple lookups and the T-Files are prime targets for modification by users,
///                          -> there is an unusually high amount of error checking, making everything quite messy.
// 
// TCHAR*                 szFileBuffer : [in]     Buffer containing T-File text
// CONST GAME_DATA_FILE*  pTypeFile    : [in]     Properties of the T-File being processed
// HWND                   hParentWnd   : [in]     Window that processed WM_PROCESSING_ERROR messages
// ERROR_QUEUE*           pErrorQueue  : [in/out] Error/warning messages, if any.
///                                                                  -> May already contain errors.
// 
// Return type : OR_SUCCESS - The objects in the TFile were parsed succesfully. Any errors were ignored by the user.
//               OR_ABORTED - TFile was PARTIALLY parsed and some objects MAY have been created, but the user aborted after minor errors.
//               OR_FAILURE - Unsupported TFile structure - indicates an old version of X2.  No objects were loaded
// 
OPERATION_RESULT  generateGameObjectsFromTypeFile(TCHAR*  szFileBuffer, CONST TCHAR*  szSubPath, CONST GAME_DATA_FILE*  pTypeFile, HWND  hParentWnd, ERROR_QUEUE*  pOutputQueue)
{
   ERROR_STACK        *pError = NULL;             // Error encountered by name generation due to an invalid/incomplete declaration
   OPERATION_RESULT    eResult = OR_SUCCESS;      // Operation result, defaults to OR_SUCCESS
   AVL_TREE           *pDuplicateObjectsTree;     // Temporary storage for GameObjects with duplicate names
   CONST TCHAR       **aObjectProperties,         // Component properties in the current object declaration
                      *szObjectID;                // Object ID
   GAME_OBJECT        *pGameObject;               // GameObject being generated
   TCHAR              *szObjectName,              // Generated name of the GameObject being generated
                      *szDeclaration = NULL,      // Full text of the current object declaration
                      *szProperty,                // Property currently being parsed (tokenised)
                      *pDeclarationIterator;      // Tokeniser data
   UINT                iCurrentSubType = 0,       // Object subtype, zero-based. Defined by the order in which the objects are declared
                       iCurrentLine = 0,          // One-based line number of the line currently being parsed (Used for error reporting only)
                       iLinePropertyCount;        // Number of properties parsed from the current line
   INT                 iExpectedObjectCount = -1, // Number of objects within the input T-File  (-1 is used to distinguish between an object count of zero, and a missing object count)
                       iActualObjectCount = 0;    // Number of objects loaded
   ERROR_QUEUE        *pIncompleteQueue;          // Errors for objects with missing game strings
   
   __try
   {
      // [INFO] "Loading %s from object type definitions file '%s'"
      pushErrorQueue(pOutputQueue, generateDualInformation(HERE(IDS_OUTPUT_LOADING_TFILE), identifyMainTypeString(pTypeFile->eMainType), szSubPath));

      // Prepare
      pDuplicateObjectsTree = createGameObjectTreeForCollisions();                         // Create a copy tree to temporarily store GameObjects with duplicate names
      aObjectProperties     = utilCreateObjectArray(CONST TCHAR*, iMaxValidProperties);    // Create storage for properties
      pIncompleteQueue      = createErrorQueue();

      /// Tokenise T-File into declarations (lines). Stop if the user aborts
      for (szDeclaration = utilTokeniseString(szFileBuffer, "\r\n", &pDeclarationIterator); szDeclaration AND (eResult == OR_SUCCESS); szDeclaration = utilGetNextToken("\r\n", &pDeclarationIterator))
      {
         // Increment line count
         iCurrentLine++;

         // [COMMENT/EMPTY LINE] Skip these lines
         if (utilCompareStringN(szDeclaration, "//", 2) OR !lstrlen(szDeclaration))
            continue;

         /// Reset properties array
         utilZeroObjectArray(aObjectProperties, CONST TCHAR*, iMaxValidProperties);
         iLinePropertyCount = 0;

         // [SHIP] Extract the object ID from the rear because they're a variable size structure
         if (pTypeFile->eMainType == MT_SHIP AND lstrlen(szDeclaration) > 10) // AND (szObjectID = utilFindSubString(szDeclaration, "SS_SH_")))
            // Remove the trailing semi-colon 
            if (szProperty = utilFindCharacterReverse(szDeclaration, ';'))
            {
               szProperty[0] = NULL;
               // Remove prefixed semi-colon
               if (szProperty = utilFindCharacterReverse(szDeclaration, ';'))
               {
                  szProperty[0] = NULL;
                  szObjectID = (szProperty + 1);
               }
            }

         // Store first property
         aObjectProperties[iLinePropertyCount++] = szDeclaration;

         /// Tokenise declaration into array
         for (szProperty = szDeclaration; szProperty[0] AND (iLinePropertyCount < iMaxValidProperties); szProperty++)
         {
            // [CHECK] Is this the start of a new property?
            if (szProperty[0] == ';')
            {
               // [SUCCESS] Isolate and store property
               szProperty[0] = NULL;
               aObjectProperties[iLinePropertyCount++] = (szProperty + 1);
            }
         }

         // Determine the meaning of the line by the number of properties it contains
         switch (iLinePropertyCount)
         {
         /// [FILE HEADER] Extract the expected object count
         case 2:
         case 3:
            // Store the number of properties for later verification
            if (aObjectProperties[GOPI_OBJECT_COUNT])
               iExpectedObjectCount = utilConvertStringToInteger(aObjectProperties[GOPI_OBJECT_COUNT]);

            // [CHECK] Ensure version is supported
            if (utilSafeConvertStringToInteger(aObjectProperties[GOPI_FILE_VERSION]) <= 14)
            {
               // [FAILURE] "The object type definitions file '%s' is from an unsupported version of %s, please update to the lastest version"
               pushErrorQueue(pOutputQueue, generateDualWarning(HERE(IDS_TFILE_VERSION_UNSUPPORTED), szSubPath, identifyGameVersionString(GV_THREAT)));
               eResult = OR_FAILURE;
            }
            continue;

         case iMaxValidProperties:
            // [WARNING] "The %s on line %u contains over 512 properties, this object is most likely corrupted and has not been loaded"
            generateQueuedWarning(pOutputQueue, HERE(IDS_TFILE_EXCESSIVE_PROPERTIES), identifyMainTypeString(pTypeFile->eMainType), iCurrentLine);
            break;
         
         // [OBJECT DECLARATION] Use various properties to generate a GameObject
         default:
            // [CHECK] Was the header's object count missing?
            if (iCurrentSubType == 0 AND iExpectedObjectCount == -1)
               // [WARNING] "The object count is missing from the header of the object type definition file '%s'"
               generateQueuedWarning(pOutputQueue, HERE(IDS_TFILE_OBJECT_COUNT_NOT_FOUND), szSubPath);
               
            // Determine which of the three methods to use for generating the correct name
            switch (pTypeFile->eDataType)
            {
            /// [SHIP] Generate the ship name from the NAME, RACE, VARIATION and OBJECT ID properties
            case DT_SHIP:
               szObjectName = generateGameObjectShipName(aObjectProperties[GOPI_NAME_ID], aObjectProperties[GOPI_SHIP_RACE_ID], aObjectProperties[GOPI_SHIP_VARIATION_ID], szObjectID, iCurrentLine, pError);
               break;

            /// [STATION] Generate the station name from NAME, RACE and FACTORY SIZE
            case DT_STATION:
               // Extract the ObjectID from a different index for docks and factories
               szObjectID = (TCHAR*)aObjectProperties[pTypeFile->eMainType == MT_FACTORY ? GOPI_FACTORY_OBJECT_ID : GOPI_DOCK_OBJECT_ID];
               // Generate pretty station name
               szObjectName = generateGameObjectStationName(aObjectProperties[GOPI_NAME_ID], aObjectProperties[GOPI_STATION_RACE_ID], aObjectProperties[GOPI_FACTORY_SIZE_ID], szObjectID, pTypeFile->eMainType, iCurrentLine, pError);
               break;

            /// [WARE] Generate ware name from NAME alone
            case DT_WARE:
               // Extract the ObjectID from a appropriate fixed index
               switch (pTypeFile->eMainType)
               {
               case MT_WARE_LASER:   szObjectID = (TCHAR*)aObjectProperties[GOPI_LASER_OBJECT_ID]; break;
               case MT_WARE_SHIELD:  szObjectID = (TCHAR*)aObjectProperties[GOPI_SHIELD_OBJECT_ID]; break;
               case MT_WARE_MISSILE: szObjectID = (TCHAR*)aObjectProperties[GOPI_MISSILE_OBJECT_ID]; break;
               default:              szObjectID = (TCHAR*)aObjectProperties[GOPI_WARE_OBJECT_ID]; break;
               }
               
               // Generate pretty ware name
               szObjectName = generateGameObjectWareName(aObjectProperties[GOPI_NAME_ID], szObjectID, pTypeFile->eMainType, iCurrentLine, pError);
               break;
            }

         } // END: Switch (property count)

         // [ERROR] Store details of incomplete objects
         if (pError)
         {
            pushErrorQueue(pIncompleteQueue, pError);    //getAppPreferences()->bReportIncompleteGameObjects ? pushErrorQueue(pOutputQueue, pError) : deleteErrorStack(pError);
            pError = NULL;
         }

         /// [SUCCESS] Create new GameObject from the generated name
         if (szObjectName)
         {
            // Generate GameObject + Parse properties
            StrTrim(szObjectName, TEXT(" "));
            pGameObject = createGameObject(pTypeFile->eMainType, iCurrentSubType, szObjectName, szObjectID);
            translateGameObjectProperties(pGameObject, aObjectProperties, pOutputQueue);
            
            // Attempt to insert into the game data
            if (!insertGameObjectIntoGameData(pGameObject))
               // [FAILED] Temporarily store in the collisions tree, for later mangling
               insertObjectIntoAVLTree(pDuplicateObjectsTree, (LPARAM)pGameObject);

            // Count successful objects
            iActualObjectCount++;
         }

         // Cleanup object name and increment current SubType
         utilSafeDeleteString(szObjectName);
         iCurrentSubType++;
         
      } // END: for (each line)

      // [CHECK] Were any objects incomplete?
      if (getQueueItemCount(pIncompleteQueue) > 0)
         // [ERROR] "Missing property strings detected in %u %s with object definitions file '%s'"
         generateAttachmentError(pOutputQueue, pIncompleteQueue, generateDualError(HERE(IDS_TFILE_INCOMPLETE_OBJECT_COUNT), getQueueItemCount(pIncompleteQueue), identifyMainTypeString(pTypeFile->eMainType), szSubPath));

      // [DEBUG]
      CONSOLE("Successfully loaded %d %s from the object type definitions file '%s'", iActualObjectCount, identifyMainTypeString(pTypeFile->eMainType), szSubPath);

      /// [SUCCESS] Mangle duplicate names
      if (eResult == OR_SUCCESS)
      {
         // [CHECK] Ensure object count was correct
         if ((iExpectedObjectCount != -1) AND (iCurrentSubType != iExpectedObjectCount))
            // [WARNING] "The object type definitions file '%s' contains %u %s, but the header indicates there should be %u"
            pushErrorQueue(pOutputQueue, generateDualWarning(HERE(IDS_TFILE_OBJECT_COUNT_INCORRECT), szSubPath, identifyMainTypeString(pTypeFile->eMainType), iCurrentSubType, iExpectedObjectCount));

         // Mangle the names of all the objects with duplicate names and insert them into the game data
         CONSOLE("Performing non-unique name mangling of game objects in T-File '%s'", szSubPath);
         insertGameObjectCollisionsIntoGameData(pDuplicateObjectsTree, pOutputQueue);
      }
      else
         /// [FAILURE] Delete the contents of the unprocessed duplicate names tree
         pDuplicateObjectsTree->pfnDeleteNode = deleteObjectNameTreeNode;
      
      // Cleanup,debug and return result
      deleteErrorQueue(pIncompleteQueue);
      deleteAVLTree(pDuplicateObjectsTree);
      utilDeleteObject(aObjectProperties);
      return eResult;
   }
   __except (pushException(pOutputQueue))
   {
      EXCEPTION3("Unable to the load %s from object type definitions file '%s' while processing '%s'", identifyMainTypeString(pTypeFile->eMainType), szSubPath, szDeclaration);
      return OR_FAILURE;
   }
}


/// Function name  : insertGameObjectCollisionsIntoGameData
// Description     : Inserts the GameObjects with duplicate names into the game data
// 
// CONST AVL_TREE*  pDuplicateObjectsTree : [in]     Tree containing the GameObjects that could not be inserted due to conflicting names
// ERROR_QUEUE*     pErrorQueue           : [in/out] Game data worker thread ErrorQueue
// 
VOID  insertGameObjectCollisionsIntoGameData(CONST AVL_TREE*  pDuplicateObjectsTree, ERROR_QUEUE*  pErrorQueue)
{
   AVL_TREE_OPERATION*  pOperationData;

   // [FUNCTION]

   // Create operation data
   pOperationData = createAVLTreeOperationEx(treeprocInsertGameObjectCollisionsIntoGameData, ATT_INORDER, pErrorQueue, NULL);

   /// Mangle conflicts and inset them into the game data
   performOperationOnAVLTree(pDuplicateObjectsTree, pOperationData);

   // Sever ErrorQueue and clean up
   pOperationData->pErrorQueue = NULL;
   deleteAVLTreeOperation(pOperationData);
}


/// Function name  : insertGameObjectIntoGameData
// Description     : Attempts to insert a GameObject into the lookup and reverse-lookup game data trees
// 
// CONST GAME_OBJECT*  pGameObject : [in] GameObject to insert
// 
// Return Value   : TRUE if inserted successfully, FALSE if the name conflicted with an existing object
// 
BOOL  insertGameObjectIntoGameData(CONST GAME_OBJECT*  pGameObject)
{
   BOOL  bResult;

   /// Attempt to insert into the reverse-lookup tree first
   bResult = insertObjectIntoAVLTree(getGameData()->pGameObjectsByText, (LPARAM)pGameObject);

   // [CHECK] Is the GameObject unique?
   if (bResult)
      // [SUCCESS] Insert into the lookup tree
      insertObjectIntoAVLTree(getGameData()->pGameObjectsByMainType, (LPARAM)pGameObject);     // This can't fail because each their IDs defined by file ID and line ID
      
   // Return result
   return bResult;
}


/// Function name  : loadGameObjectTrees
// Description     : Generates the ObjectName game data trees from the relevant game data T-Files.
///                                 NB: ObjectNames are stored using the MainType for primary key and SubType for secondary
// 
// CONST FILE_SYSTEM*  pFileSystem  : [in]     FileSystem object
// HWND                hParentWnd   : [in]     Ignored
// OPERATION_PROGRESS* pProgress    : [in]     Operation progress
// ERROR_QUEUE*        pErrorQueue  : [in/out] Error messages, if any
// 
/// [Operation Stages : ONE]
//
// Return type : OR_SUCCESS - GameObjectName trees were created succesfully. 
//               OR_FAILURE - Some GameObjectName trees may have been created
//
OPERATION_RESULT  loadGameObjectTrees(CONST FILE_SYSTEM*  pFileSystem, HWND  hParentWnd, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue)
{
   OPERATION_RESULT       eResult = OR_SUCCESS; // Operation result
   CONST GAME_DATA_FILE*  pFileInfo;            // Properties of the current TypesFile (T-File)
   GAME_FILE*             pGameFile;            // TFile being processed
   CONST TCHAR*           szSubPath;
   
   __try
   {
      CONSOLE_STAGE();
      
      // [STAGE] Define progress as the number of T-Files processed
      ASSERT(advanceOperationProgressStage(pProgress) == IDS_PROGRESS_LOADING_OBJECT_NAMES);
      updateOperationProgressMaximum(pProgress, iObjectNameFileCount);
      
      /// Create the GameObject trees
      getGameData()->pGameObjectsByMainType = createGameObjectTreeByMainType();
      getGameData()->pGameObjectsByText     = createGameObjectTreeByText();

      /// Load and parse each T-File.  Stop if the user aborts
      for (MAIN_TYPE  eObjectType = MT_DOCK; (eResult == OR_SUCCESS) AND (eObjectType <= MT_WARE_TECHNOLOGY) AND findGameObjectDataFileByMainType(eObjectType, pFileInfo); eObjectType = (MAIN_TYPE)(eObjectType + 1))
      {
         // [INFO/PROGRESS]
         CONSOLE("Loading object definition file '%s'", findGameObjectDataFileSubPathByMainType(eObjectType, getAppPreferences()->eGameVersion));
         advanceOperationProgressValue(pProgress);

         // Prepare
         szSubPath = findGameObjectDataFileSubPathByMainType(eObjectType, getAppPreferences()->eGameVersion);
         pGameFile = createGameFileFromSubPath(szSubPath);      

         /// Load T-File
         if (!loadGameFileFromFileSystemByPath(pFileSystem, pGameFile, pFileInfo->szAlternateExtension, pErrorQueue))
         {
            // No enhancement necessary
            generateOutputTextFromLastError(pErrorQueue);            // [ERROR] "There was an error while loading object properties from the TFile '%s', please check the file exists."
            eResult = OR_FAILURE;                                    // enhanceLastError(pErrorQueue, ERROR_ID(IDS_TFILE_IO_ERROR), szSubPath);
         }
         else
            /// Generate new ObjectNames from current TFile
            eResult = generateGameObjectsFromTypeFile(pGameFile->szInputBuffer, szSubPath, pFileInfo, hParentWnd, pErrorQueue);
         
         // Cleanup
         deleteGameFile(pGameFile);
         CONSOLE(SMALL_PARTITION);
      } 

      CONSOLE_HEADING("Loaded %d Game Objects with %d failures", getTreeNodeCount(getGameData()->pGameObjectsByMainType), getTreeNodeCount(getGameData()->pGameObjectsByMainType) - getTreeNodeCount(getGameData()->pGameObjectsByText));

      /// [SUCCESS] Index the reverse-lookup tree
      if (eResult == OR_SUCCESS)
         performAVLTreeIndexing(getGameData()->pGameObjectsByText);

      // Return operation result
      return eResult;
   }
   __except (pushException(pErrorQueue))
   {
      EXCEPTION1("Unable to load game object names from '%s'", pGameFile ? pGameFile->szFullPath : NULL);
      return OR_FAILURE;
   }
}


/// Function name  : performGameObjectNameMangling
// Description     : Attempts to generate a unique GameObject name by appending its SubType ID to the name
// 
// GAME_OBJECT*  pGameObject   : [in] GameObject to mangle
// 
VOID   performGameObjectNameMangling(GAME_OBJECT*  pGameObject)
{
   UINT   iNewTextLength;

   // Calculate length of new string buffer
   iNewTextLength = (pGameObject->iCount + max(8, lstrlen(pGameObject->szObjectID) + 5));

   // Extend string buffer
   pGameObject->szText = utilExtendString(pGameObject->szText, pGameObject->iCount, iNewTextLength);
   pGameObject->iCount = iNewTextLength;

   // [CHECK] Has input object been mangled before?
   if (utilFindStringRightN(pGameObject->szText, 1)[0] != ')')
      // [NO] Append Object ID  (Trim the 'SS_' prefix)
      utilStringCchCatf(pGameObject->szText, pGameObject->iCount, TEXT(" (%s)"), (utilCompareStringN(pGameObject->szObjectID, "SS_", 3) ? &pGameObject->szObjectID[3] : pGameObject->szObjectID));
   else
      // [YES] Append SubType
      utilStringCchCatf(pGameObject->szText, pGameObject->iCount, TEXT(" (%u)"), pGameObject->iID);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    TREE OPERATIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : treeprocInsertGameObjectCollisionsIntoGameData
// Description     : Mangles the names of GameObjects with duplicate names and inserts them into the game data
// 
// AVL_TREE_NODE*       pCurrentNode   : [in] Node containing a game object with a duplicate name
// AVL_TREE_OPERATION*  pOperationData : [in] Ignored
// 
VOID  treeprocInsertGameObjectCollisionsIntoGameData(AVL_TREE_NODE*  pCurrentNode, AVL_TREE_OPERATION*  pOperationData)
{
   GAME_OBJECT  *pNewObject,        // Convenience pointer
                *pConflictObject;   // GameObject causing the name conflict
   // Prepare
   pNewObject = extractPointerFromTreeNode(pCurrentNode, GAME_OBJECT);

   /// [CHECK] Remove the conflicting object, if we haven't already done so for a previous node
   if (removeObjectFromAVLTreeByValue(getGameData()->pGameObjectsByText, (LPARAM)pNewObject->szText, NULL, (LPARAM&)pConflictObject))
   {
      // Re-Mangle the conflict's name
      performGameObjectNameMangling(pConflictObject);
      insertObjectIntoAVLTree(getGameData()->pGameObjectsByText, (LPARAM)pConflictObject);
   }

   // Mangle the object's name
   performGameObjectNameMangling(pNewObject);

   /// Attempt to insert conflicting object into the reverse-lookup tree  (may fail)
   if (insertObjectIntoAVLTree(getGameData()->pGameObjectsByText, (LPARAM)pNewObject))
      // [SUCCESS] Insert input object into the lookup tree  (Cannot fail, IDs guaranteed to be unique)
      insertObjectIntoAVLTree(getGameData()->pGameObjectsByMainType, (LPARAM)pNewObject);
   else
   {
      // Re-Mangle the object's name
      performGameObjectNameMangling(pNewObject);

      /// Re-attempt to insert conflicting object into the reverse-lookup tree  (Should not fail)
      if (insertObjectIntoAVLTree(getGameData()->pGameObjectsByText, (LPARAM)pNewObject))
         // [SUCCESS] Insert input object into the lookup tree  (Cannot fail, IDs guaranteed to be unique)
         insertObjectIntoAVLTree(getGameData()->pGameObjectsByMainType, (LPARAM)pNewObject);
      else
         // [WARNING] "Could not insert the duplicate GameObject '%s' (%s) from T-File '%s' into the game data"
         generateQueuedWarning(pOperationData->pErrorQueue, HERE(IDS_TFILE_OBJECT_NAME_DUPLICATE), pNewObject->szText, pNewObject->szObjectID, identifyTFileFromMainType(pNewObject->eMainType));
   }
}

