//
// Script Objects.cpp  :  Represents the constants used in scripts
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     CONSTANTS / GLOBAL
/// /////////////////////////////////////////////////////////////////////////////////////////

CONST TCHAR*  szVersionInitials[4] = { TEXT("X2"), TEXT("X3R"), TEXT("X3TC"), TEXT("X3AP") };

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createScriptObject
// Description     : Creates a new ScriptObject from an existing GameString
// 
// CONST GAME_STRING*  pGameString : [in] GameString used to generate object. PageID must one of the ocmpilation string pages.
// 
// Return Value   : New ScriptObject, you are responsible for destroying it
// 
SCRIPT_OBJECT*  createScriptObject(CONST GAME_STRING*  pGameString)
{
   SCRIPT_OBJECT*  pScriptObject;  // Object being created

   // [CHECK] Ensure GameString is already 'INTERNAL'
   ASSERT(pGameString->eType == ST_INTERNAL);

   // Prepare
   pScriptObject = utilCreateEmptyObject(OBJECT_NAME);
   
   // [CHECK] Ensure text has been provided
   if (!lstrlen(pGameString->szText))
      /// [FAILED] Insert an error string
      pScriptObject->szText = utilCreateStringf(32, TEXT("MISSING_NAME_%u"), pGameString->iID);

   /// [SUCCESS] Convert GameString to DISPLAY, if necessary
   else 
      replaceStringConvert(pScriptObject->szText, SPC_LANGUAGE_INTERNAL_TO_DISPLAY, pGameString->szText);

   // Set properties
   pScriptObject->eType       = ONT_SCRIPT;
   pScriptObject->eGroup      = identifyObjectNameGroupFromGameString(pGameString);
   pScriptObject->iID         = pGameString->iID;
   pScriptObject->eVersion    = pGameString->eVersion;
   pScriptObject->bModContent = pGameString->bModContent;
   pScriptObject->iCount      = lstrlen(pScriptObject->szText);

   // Return new object
   return pScriptObject;
}


/// Function name  : createScriptObjectTreeByGroup
// Description     : Creates the ScriptObject tree organised by GROUP and then ID
// 
// Return Value   : New AVL Tree, you are responsible for destroying it
// 
AVL_TREE*   createScriptObjectTreeByGroup()
{
   // Create storage tree
   return createAVLTree(extractScriptObjectTreeNode, deleteObjectNameTreeNode, createAVLTreeSortKey(AK_GROUP, AO_ASCENDING), createAVLTreeSortKey(AK_ID, AO_ASCENDING), NULL);
}


/// Function name  : createScriptObjectTreeByText
// Description     : Creates the ScriptObject reverse-lookup copy tree, organised by TEXT
// 
// Return Value   : New AVL Tree, you are responsible for destroying it
// 
AVL_TREE*   createScriptObjectTreeByText()
{
   // Create storage tree
   return createAVLTree(extractScriptObjectTreeNode, NULL, createAVLTreeSortKey(AK_TEXT, AO_ASCENDING), NULL, NULL);
}

/// Function name  : createScriptObjectTreeForCollisions
// Description     : Creates a ScriptObject copy tree organised by GROUP, then ID, for temporarily storing collisions
// 
// Return Value   : New AVL Tree, you are responsible for destroying it
// 
AVL_TREE*   createScriptObjectTreeForCollisions()
{
   // Create copy tree
   return createAVLTree(extractScriptObjectTreeNode, NULL, createAVLTreeSortKey(AK_GROUP, AO_ASCENDING), createAVLTreeSortKey(AK_ID, AO_ASCENDING), NULL);
}

/// Function name  : deleteObjectName
// Description     : Deletes either a GameObject or ScriptObject
// 
// OBJECT_NAME*  pObjectName : [in] GameObject/ScriptObject to destroy
// 
VOID  deleteObjectName(OBJECT_NAME*  pObjectName)
{
   // Delete text
   utilDeleteString(pObjectName->szText);

   // Delete object ID
   utilSafeDeleteString(pObjectName->szObjectID);

   // [OPTIONAL] Delete description
   if (pObjectName->pDescription)
      deleteRichText(pObjectName->pDescription);

   // Delete calling object
   utilDeleteObject(pObjectName);
}


/// Function name  : deleteObjectNameTreeNode
// Description     : Destroys a Game/ScriptObject stored within an AVLTree node
// 
// LPARAM  pObject   : [in] Node containing an OBJECT_NAME
// 
VOID  deleteObjectNameTreeNode(LPARAM  pObject)
{
   // Delete calling object
   deleteObjectName((OBJECT_NAME*&)pObject);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                          HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : extractScriptObjectTreeNode
// Description     : Returns the value of the specified property in an AVLTree node containing a ScriptObject
// 
// LPARAM                   pObject    : [in] SCRIPT_OBJECT* : TreeNode data containing a ScriptObject
// CONST AVL_TREE_SORT_KEY  eProperty  : [in] Property to retrieve
// 
// Return Value   : Desired property value or NULL if the property is unsupported
// 
BearScriptAPI
LPARAM   extractScriptObjectTreeNode(LPARAM  pObject, CONST AVL_TREE_SORT_KEY  eProperty)
{
   SCRIPT_OBJECT*  pScriptObject;      // Convenience pointer
   LPARAM          xOutput;            // Property value

   // Prepare
   pScriptObject = (SCRIPT_OBJECT*)pObject;

   // Examine property
   switch (eProperty)
   {
   case AK_ID:       xOutput = (LPARAM)pScriptObject->iID;        break;
   case AK_GROUP:    xOutput = (LPARAM)pScriptObject->eGroup;     break;
   case AK_TEXT:     xOutput = (LPARAM)pScriptObject->szText;     break;

   // [UNSUPPORTED] Error
   default:          xOutput = NULL;  ASSERT(FALSE);            break;
   }

   // Return value
   return xOutput;
}



/// Function name  : findScriptObjectByGroup
// Description     : Retrieves a ScriptObject with a specified index within a group
// 
// CONST OBJECT_NAME_GROUP  eGroup  : [in]  Object name group
// CONST UINT               iID     : [in]  Underlying GameString ID
// SCRIPT_OBJECT*          &pOutput : [out] ScriptObject if found, otherwise NULL
// 
// Return Value : TRUE if found, FALSE if not
//
BOOL   findScriptObjectByGroup(CONST OBJECT_NAME_GROUP  eGroup, CONST UINT  iID, SCRIPT_OBJECT*  &pOutput)
{
   // Query group tree 
   return findObjectInAVLTreeByValue(getGameData()->pScriptObjectsByGroup, eGroup, iID, (LPARAM&)pOutput);
}



/// Function name  : findScriptObjectByText
// Description     : Searches the ScriptObject reverse-lookup tree for a ScriptObject with a specified text
// 
// CONST TCHAR*    szSearchString : [in]  Search string
// SCRIPT_OBJECT* &pOutput        : [out] ScriptObject if found, otherwise NULL
// 
// Return Value : TRUE if found, FALSE if not
//
BearScriptAPI 
BOOL   findScriptObjectByText(CONST TCHAR*  szSearchString, SCRIPT_OBJECT*  &pOutput)
{
   // Query names tree using search string
   return findObjectInAVLTreeByValue(getGameData()->pScriptObjectsByText, (LPARAM)szSearchString, NULL, (LPARAM&)pOutput);
}


/// Function name  : identifyObjectNameGroupFromGameString
// Description     : Determines a ScriptObject's group from the page of the string it represents
// 
// CONST UINT  iPageID   : [in] PageID of the GameString represented by the ScriptObject
// 
// Return Value   : Game/Script object group
// 
OBJECT_NAME_GROUP  identifyObjectNameGroupFromGameString(CONST GAME_STRING*  pGameString)
{
   OBJECT_NAME_GROUP  eOutput;

   // Examine page
   switch (pGameString->iPageID)
   {
   case GPI_SECTORS:                eOutput = ONG_SECTOR;               break;
   case GPI_STATION_SERIALS:        eOutput = ONG_STATION_SERIAL;       break;
   case GPI_RELATIONS:              eOutput = ONG_RELATION;             break;
   case GPI_RACES:                  eOutput = ONG_RACE;                 break;
   case GPI_TRANSPORT_CLASSES:      eOutput = ONG_TRANSPORT_CLASS;      break;
   case GPI_PARAMETER_TYPES:        eOutput = ONG_PARAMETER_TYPE;       break;
   case GPI_CONSTANTS:              eOutput = ONG_CONSTANT;             break; 
   case GPI_OBJECT_CLASSES:         eOutput = ONG_OBJECT_CLASS;         break;
   case GPI_WING_COMMANDS:          eOutput = ONG_WING_COMMAND;         break;
   case GPI_FLIGHT_RETURNS:         eOutput = ONG_FLIGHT_RETURN;        break;
   ///case GPI_FORMATIONS:             eOutput = ONG_FORMATION;            break;      REM: Don't exist
   case GPI_DATA_TYPES:             eOutput = ONG_DATA_TYPE;            break;
   case GPI_OPERATORS:              eOutput = ONG_OPERATOR;             break;

   // [OBJECT COMMANDS] Split this page into two groups
   case GPI_OBJECT_COMMANDS: 
      if (pGameString->iID >= 1000 AND pGameString->iID < 1100)     
         eOutput = ONG_SIGNAL;      // Also possibly pGameString->iID < 200 
      else
         eOutput = ONG_OBJECT_COMMAND;
      break;

   // [UNSUPPORTED] Error
   default:    ASSERT(FALSE);       eOutput = ONG_DOCK;
   }

   // Return group
   return eOutput;
}


/// Function name  : generateScriptObjectGroupString
// Description     : Generates the name of a ScriptObject group
// 
// CONST OBJECT_NAME_GROUP  eGroup   : [in] ScriptObject group
// 
// Return Value   : New string, you are responsible for destroying it
// 
TCHAR*  generateScriptObjectGroupString(CONST OBJECT_NAME_GROUP  eGroup)
{
   // Load, create and return appropriate string
   return loadString((IDS_SCRIPT_OBJECT_GROUP_CONSTANT + (eGroup - ONG_CONSTANT)), 64);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                          FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : loadScriptObjectsTrees
// Description     : Builds the two ScriptObject game data trees from the existing string game data
// 
// OPERATION_PROGRESS*  pProgress   : [in/out] Progress tracker, with stage and maximum already set
// ERROR_QUEUE*         pErrorQueue : [in/out] ErrorQueue from the game data loading worker thread
///                                           May already contain errors
// 
BOOL  loadScriptObjectsTrees(OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue)
{
   AVL_TREE_OPERATION  *pGenerationOperation,      // Operation data for extract SCriptObjects from the game data
                       *pMergeOperation;           // Operation data for holding the ScriptObjects that require name mangling
   AVL_TREE            *pCollisionsTree;           // Copy tree holding the ScriptObjects with non-unique names

   __try
   {
      // [TRACKING]
      CONSOLE_STAGE();
      CONSOLE("Generating Script Objects from language strings");

      // [INFO] "Generating Script Objects from loaded language strings"
      pushErrorQueue(pErrorQueue, generateDualInformation(HERE(IDS_OUTPUT_GENERATING_SCRIPT_OBJECTS)));
      
      // [CHECK] Ensure ScriptObject trees doesn't exist
      ASSERT(!getGameData()->pScriptObjectsByGroup AND !getGameData()->pScriptObjectsByText);

      /// Create ScriptObject trees
      getGameData()->pScriptObjectsByGroup = createScriptObjectTreeByGroup();
      getGameData()->pScriptObjectsByText  = createScriptObjectTreeByText();
      pCollisionsTree                      = createScriptObjectTreeForCollisions();

      // Generate
      CONSOLE("Generating unique ScriptObjects");

      /// Generate ScriptObjects from GameStrings tree. Store duplicates in the Collisions tree
      pGenerationOperation              = createAVLTreeOperationEx(treeprocBuildScriptObjectTrees, ATT_INORDER, NULL, pProgress);
      pGenerationOperation->pOutputTree = pCollisionsTree;   
      performOperationOnAVLTree(getGameData()->pGameStringsByID, pGenerationOperation);

      // Merge
      CONSOLE("Merging ScriptObject collisions into game data");

      /// Manipulate the Collision tree entries to make them unique, then re-insert them.
      pMergeOperation = createAVLTreeOperationEx(treeprocMergeScriptObjectCollisions, ATT_INORDER, pErrorQueue, NULL);
      performOperationOnAVLTree(pCollisionsTree, pMergeOperation);

      // [SUCCESS]
      CONSOLE_HEADING("Generated %u unique ScriptObjects with %d failures", getTreeNodeCount(getGameData()->pScriptObjectsByGroup), getTreeNodeCount(getGameData()->pScriptObjectsByGroup) - getTreeNodeCount(getGameData()->pScriptObjectsByText));

      // Cleanup
      deleteAVLTree(pCollisionsTree);
      pMergeOperation->pErrorQueue = NULL;
      deleteAVLTreeOperation(pMergeOperation);
      deleteAVLTreeOperation(pGenerationOperation);
      return TRUE;
   }
   __except (pushException(pErrorQueue))
   {
      EXCEPTION("Unable to generate script objects");
      return FALSE;
   }
}


/// Function name  : performScriptObjectNameMangling
// Description     : Alters ScriptObject text to ensure it is unique 
//
///                      This is performed differently depending on its group. Sectors have co-ordinates appended, whereas
///                      -> object classes have the game version appended etc. 
// 
// SCRIPT_OBJECT*  pScriptObject : [in/out] ScriptObject to mangle
// 
VOID  performScriptObjectNameMangling(SCRIPT_OBJECT*  pScriptObject)
{
   POINTS   ptCoordinates;    // Sector co-ords
   TCHAR   *szNewText,
           *szGroup;

   // Prepare
   szNewText = NULL;

   // Determine whether name mangling is required
   switch (pScriptObject->eGroup)
   {
   /// [OBJECT CLASS] Append the game version in brackets
   case ONG_OBJECT_CLASS:
      szNewText = utilCreateStringf(pScriptObject->iCount + 16, TEXT("%s (%s)"), pScriptObject->szText, szVersionInitials[pScriptObject->eVersion]);  
      break;

   /// [OBJECT COMMAND] Append the ID in brackets
   case ONG_STATION_SERIAL:
   case ONG_OBJECT_COMMAND:
   case ONG_WING_COMMAND:
   case ONG_SIGNAL:
      szNewText = utilCreateStringf(pScriptObject->iCount + 16, TEXT("%s (%d)"), pScriptObject->szText, pScriptObject->iID);
      break;

   /// [RACE] Append type in brackets
   case ONG_RACE:
   case GPI_RELATIONS:
      szNewText = utilCreateStringf(pScriptObject->iCount + 32, TEXT("%s (%s)"), pScriptObject->szText, szGroup = generateScriptObjectGroupString(pScriptObject->eGroup));
      utilDeleteString(szGroup);
      break;

   /// [SECTOR] Append the co-ordinates in brackets
   case ONG_SECTOR:
      // Calculate sector co-ordinates and update ScriptObject
      convertStringIDToSectorCoordinates(pScriptObject->iID, &ptCoordinates);
      szNewText = utilCreateStringf(pScriptObject->iCount + 16, TEXT("%s (%d, %d)"), pScriptObject->szText, ptCoordinates.x, ptCoordinates.y);
      break;
   }

   // [CHANGED] Update object
   if (szNewText)
   {
      utilDeleteString(pScriptObject->szText);
      pScriptObject->szText = szNewText;
      pScriptObject->iCount = lstrlen(pScriptObject->szText);
   }
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                          TREE OPERATIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : treeprocBuildScriptObjectTrees
// Description     : Inserts the GameStrings used in compilation into the ScriptObjects tree.  If not unique they're placed in a collisions tree
// 
// AVL_TREE_NODE*       pCurrentNode   : [in/out] Game data GameString node
// AVL_TREE_OPERATION*  pOperationData : [in/out] Operation data:
///                                                pOutputTree : [in/out] Collisions tree, holding the ScriptObjecvts that could not be inserted
// 
VOID   treeprocBuildScriptObjectTrees(AVL_TREE_NODE*  pCurrentNode, AVL_TREE_OPERATION*  pOperationData)
{
   GAME_STRING*   pGameString     = extractPointerFromTreeNode(pCurrentNode, GAME_STRING);      // GameString being processed
   AVL_TREE*      pCollisionsTree = pOperationData->pOutputTree;                                // Collisions tree
   SCRIPT_OBJECT* pScriptObject   = NULL;                                                       // ScriptObject being created, if any

   // Only copy strings from the GamePages that potentially contain script objects
   switch (pGameString->iPageID)
   {
   /// [CONSTANTS] Include all
   case GPI_CONSTANTS:
      pScriptObject = createScriptObject(pGameString);

      // [SPECIAL CASE] Insert 'old' [THIS] into the ID tree, so it can be resolved, but do not include it in the reverse-lookup tree
      if (pGameString->iID == 0)
      {
         insertObjectIntoAVLTree(getGameData()->pScriptObjectsByGroup, (LPARAM)pScriptObject);
         return;
      }
      break;

   /// [OPERATORS] Exclude the minus operator, this is detected by findNextCodeObject(..)
   case GPI_OPERATORS:
      if (pGameString->iID != OT_MINUS)
         pScriptObject = createScriptObject(pGameString);
      break;

   /// [RACES] Exclude the race abbreviations
   case GPI_RACES:
      if (pGameString->iID < 200)
         pScriptObject = createScriptObject(pGameString);
      break;

   /// [RELATIONS] Include only 'Friend', 'Foe' and 'Neutral'
   case GPI_RELATIONS:
      switch (pGameString->iID)
      {
      // [FRIEND/FOE/NEUTRAL]
      case 1102422:
      case 1102423:
      case 1102424:
         pScriptObject = createScriptObject(pGameString);
      }
      break;

   /// [SECTOR] Skip 6 digit sector names with IDs 20xxx and 30xxx, I'm not sure what they are
   case GPI_SECTORS:
      if (pGameString->iID >= 1020000)
         pScriptObject = createScriptObject(pGameString);
      break;
   
   /// [STATION SERIALS] Exclude the S,M,L,XL,XXL ship/station name modifiers
   case GPI_STATION_SERIALS:
      if (pGameString->iID < 500)
         pScriptObject = createScriptObject(pGameString);
      break;

   /// [TRANSPORT CLASS] Only include the abbreviated versions
   case GPI_TRANSPORT_CLASSES:
      if (pGameString->iID < 10)
         pScriptObject = createScriptObject(pGameString);
      break;

   /// [DATA TYPE / FLIGHT RETURN / FORMATION / PARAMETER-TYPE / OBJECT CLASS / OBJECT COMMAND / WING COMMAND] Include all
   case GPI_DATA_TYPES:
   case GPI_FLIGHT_RETURNS:
   //case GPI_FORMATIONS:     REM: Don't exist
   case GPI_OBJECT_CLASSES:
   case GPI_OBJECT_COMMANDS:
   case GPI_PARAMETER_TYPES:
   case GPI_WING_COMMANDS:
      pScriptObject = createScriptObject(pGameString);
      break;
   }

   // [GENERATED] Attempt to insert into game data
   if (pScriptObject)
   {
      // [CHECK] Attempt to insert into text tree first
      if (insertObjectIntoAVLTree(getGameData()->pScriptObjectsByText, (LPARAM)pScriptObject))
         /// [SUCCESS] Insert into group tree (always unique)
         insertObjectIntoAVLTree(getGameData()->pScriptObjectsByGroup, (LPARAM)pScriptObject);
      else
         /// [FAILURE] Insert into collisions tree
         insertObjectIntoAVLTree(pCollisionsTree, (LPARAM)pScriptObject);
   }
}

/// Function name  : treeprocBuildScriptObjectTrees
// Description     : Builds the ScriptObjects trees by scanning the game string data for relevant strings
// 
// AVL_TREE_NODE*       pCurrentNode   : [in/out] Node containing GameString currently being examined - this may be modified to make it unique
// AVL_TREE_OPERATION*  pOperationData : [in/out] Operation Data
///                                                   xFirstInput : [in/out] Collisions tree, holding the ScriptObjects that could not be inserted
// 
VOID   treeprocMergeScriptObjectCollisions(AVL_TREE_NODE*  pCurrentNode, AVL_TREE_OPERATION*  pOperationData)
{
   SCRIPT_OBJECT  *pNewObject = NULL,        // Convenience pointer
                  *pConflictObject = NULL;   // ScriptObject causing the name conflict
   TCHAR          *szGroupName;              // The name of the group containing the conflict  [ERROR REPORTING]

   __try
   {
      // Prepare
      pNewObject = extractPointerFromTreeNode(pCurrentNode, SCRIPT_OBJECT);

      /// [CHECK] Remove and mangle the conflicting object, if we haven't already done so for a previous node
      if (removeObjectFromAVLTreeByValue(getGameData()->pScriptObjectsByText, (LPARAM)pNewObject->szText, NULL, (LPARAM&)pConflictObject))  // NB: Since GROUP tree does not organise by text, there is no need to remove it both from trees
      {
         // [FOUND] Mangle the conflicting object's name
         performScriptObjectNameMangling(pConflictObject);

         /// Attempt to re-insert conflicting object into the reverse-lookup tree  (Should not fail)
         if (!insertObjectIntoAVLTree(getGameData()->pScriptObjectsByText, (LPARAM)pConflictObject))
         {
            // [WARNING] "Could not insert the duplicate script object '%s' (ID %u) from group '%s' into the game data"
            szGroupName = generateScriptObjectGroupString(pConflictObject->eGroup);
            pushErrorQueue(pOperationData->pErrorQueue, generateDualWarning(HERE(IDS_SCRIPT_OBJECT_DUPLICATE), pConflictObject->szText, pConflictObject->iID, szGroupName));

            // [FAILED] Destroy remaining object in GROUP tree
            destroyObjectInAVLTreeByValue(getGameData()->pScriptObjectsByGroup, pConflictObject->eGroup, pConflictObject->iID);
            utilDeleteString(szGroupName);
         }
      }

      /// Mangle the input object's name
      performScriptObjectNameMangling(pNewObject);

      /// Attempt to insert input object into the TEXT tree  (Should not fail)
      if (!insertObjectIntoAVLTree(getGameData()->pScriptObjectsByText, (LPARAM)pNewObject))
      {
         // [WARNING] "Could not insert the duplicate script object '%s' (ID %u) from group '%s' into the game data"
         szGroupName = generateScriptObjectGroupString(pNewObject->eGroup);
         pushErrorQueue(pOperationData->pErrorQueue, generateDualWarning(HERE(IDS_SCRIPT_OBJECT_DUPLICATE), pNewObject->szText, pNewObject->iID, szGroupName));

         // Cleanup
         utilDeleteString(szGroupName);
         deleteScriptObject(pNewObject);
      }
      else
         // [SUCCESS] Insert input object into the lookup tree  (Cannot fail, IDs guaranteed to be unique)
         insertObjectIntoAVLTree(getGameData()->pScriptObjectsByGroup, (LPARAM)pNewObject);
   }
   PUSH_CATCH(pOperationData->pErrorQueue)
   {
      EXCEPTION("Unable to merge one of the two following script objects:");
      debugObjectName(pNewObject);
      debugObjectName(pConflictObject);
   }
}

