//
// Game Object Properties.cpp : Extended properties for GameObjects
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        DECLARATIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

struct OBJECT_CLASS
{
   const TCHAR*  szClass;
   const UINT    iStringID;
};

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////

#define     GOPI_VOLUME               0
#define     GOPI_RELATIVE_VALUE       1
#define     GOPI_PRICE_MODIFIER_1     2
#define     GOPI_PRICE_MODIFIER_2     3
#define     GOPI_TRANSPORT_CLASS      4
#define     GOPI_RELATIVE_PL_VALUE    5

#define     GOPI_LASER_ENERGY         10
#define     GOPI_SHIELD_ENERGY        8

#define     GOPI_SHIP_SPEED           7
#define     GOPI_SHIP_SHIELD_TYPE     22
#define     GOPI_SHIP_MAX_SHIELDS     23
#define     GOPI_SHIP_SPEED_EXTS      26
#define     GOPI_SHIP_CARGO_MIN       28
#define     GOPI_SHIP_CARGO_MAX       29
#define     GOPI_SHIP_WARE_LIST       30
#define     GOPI_SHIP_DOCKING_SLOTS   43
#define     GOPI_SHIP_MAX_CARGO_SIZE  44
#define     GOPI_SHIP_RACE            45
#define     GOPI_SHIP_HULL_STRENGTH   46
#define     GOPI_SHIP_VARIATION       50
#define     GOPI_SHIP_CLASS           52
#define     GOPI_SHIP_TURRET_COUNT    53
#define     GOPI_SHIP_GUN_GROUP_COUNT 54 + (4 * GOPI_SHIP_TURRET_COUNT)

// Individual modifiers for types based on category 
CONST UINT  iPriceModifiers[MT_WARE_TECHNOLOGY+1] = 
{
   NULL, NULL, NULL, NULL, NULL, 
   NULL, // MT_DOCK
   64,   // MT_FACTORY         
   122,  // MT_SHIP            
   64,   // MT_WARE_LASER      
   64,   // MT_WARE_SHIELD     
   28,   // MT_WARE_MISSILE    
   4,    // MT_WARE_ENERGY     
   NULL, // MT_WARE_NATURAL    
   5,    // MT_WARE_BIOLOGICAL 
   12,   // MT_WARE_FOODSTUFF  
   5,    // MT_WARE_MINERAL    
   28,   // MT_WARE_TECHNOLOGY 
};

// StringIDs associated with each ObjClass
static OBJECT_CLASS  szShipClasses[27] = 
{ 
   {TEXT("OBJ_SHIP_M0"), 2020}, {TEXT("OBJ_SHIP_M1"), 2021}, {TEXT("OBJ_SHIP_M2"), 2022}, {TEXT("OBJ_SHIP_M3"), 2023}, {TEXT("OBJ_SHIP_M4"), 2024}, {TEXT("OBJ_SHIP_M5"), 2025}, 
   {TEXT("OBJ_SHIP_M6"), 2026}, {TEXT("OBJ_SHIP_M7"), 2027}, {TEXT("OBJ_SHIP_M8"), 2142}, {TEXT("OBJ_SHIP_TP"), 2030}, {TEXT("OBJ_SHIP_TS"), 2031}, {TEXT("OBJ_SHIP_TM"), 2141}, 
   {TEXT("OBJ_SHIP_TL"), 2032}, {TEXT("OBJ_SHIP_TS_PIRATE"), 2035}, {TEXT("OBJ_SHIP_TL_PIRATE"), 2036}, {TEXT("OBJ_SHIP_GONER"), 2039}, {TEXT("OBJ_SHIP_UFO"), 2068}, 
   {TEXT("OBJ_SHIP_ASTRONAUT"), 2064}, {TEXT("OBJ_SATELLITE"), 2065}, {TEXT("OBJ_SATELLITE2"), 2080}, {TEXT("OBJ_BEACON"), 2060}, {TEXT("OBJ_MINE"), 2066}, 
   {TEXT("OBJ_ORBITALLASER"), 2011}, {TEXT("OBJ_FIGHTDRONE"), 2067}, {TEXT("OBJ_FREIGHTDRONE"), 2092}, {TEXT("OBJ_SPACEFLY"), 2070}, {TEXT("OBJ_SHIP_KHAAKCLUSTER"), 2131} 
};

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////


/// Function name  : identifyCommonShipOffset
// Description     : Identifies the offset at which the common properties begin in a ship declaration
// 
// const TCHAR**  aProperties   : [in] 
// 
// Return Value   : 
// 
UINT  identifyCommonShipOffset(const TCHAR**  aProperties)
{
   UINT  iCurrentProperty = GOPI_SHIP_TURRET_COUNT,      // Start at 'Turret Count'
         iTurretCount, iGunCount, iWeaponCount; 

   // Extract and consume Turret Count
   iTurretCount = utilSafeConvertStringToInteger(aProperties[GOPI_SHIP_TURRET_COUNT]);
   iCurrentProperty++;

   // Advance through turrets
   iCurrentProperty += (4 * iTurretCount);

   // Extract and consume gun count
   iGunCount = utilSafeConvertStringToInteger(aProperties[iCurrentProperty]);
   iCurrentProperty++;

   // Advance through guns (if any)
   for (UINT  iGun = 0; iGun < iGunCount; iGun++)
   {
      // Advance to 'Weapon Count'
      iCurrentProperty += 3;

      // Extract and consume 'Weapon Count'
      iWeaponCount = utilSafeConvertStringToInteger(aProperties[iCurrentProperty]);
      iCurrentProperty++;

      // Consume weapon properties and advance to either: the next gun OR 'Volume'
      iCurrentProperty += (6 * iWeaponCount);
   }

   // Set common offset
   return iCurrentProperty;
}



VOID  calculateGameObjectPrice(GAME_OBJECT*  pGameObject, const UINT  iCommonOffset, const TCHAR** aProperties)
{
   UINT  iRelativeValue,  iAveragePrice,
         iPriceModifier1, iPriceModifier2;

   // Extract price components
   iPriceModifier1 = utilSafeConvertStringToInteger(aProperties[iCommonOffset + GOPI_PRICE_MODIFIER_1]);
   iPriceModifier2 = utilSafeConvertStringToInteger(aProperties[iCommonOffset + GOPI_PRICE_MODIFIER_2]);
   iRelativeValue  = utilSafeConvertStringToInteger(aProperties[iCommonOffset + GOPI_RELATIVE_VALUE]);

   // Calculate average price
   iAveragePrice = (iRelativeValue * iPriceModifiers[pGameObject->eMainType]);

   /// Calculate price
   pGameObject->oProperties.iMinPrice = iAveragePrice - utilCalculatePercentage(iAveragePrice, iPriceModifier1);
   pGameObject->oProperties.iMaxPrice = iAveragePrice + utilCalculatePercentage(iAveragePrice, iPriceModifier1);
}


VOID  calculateGameObjectSpeed(GAME_OBJECT*  pGameObject, const TCHAR** aProperties)
{
   UINT  iMin = utilSafeConvertStringToInteger(aProperties[GOPI_SHIP_SPEED]) / 500,
         iMax = iMin + (iMin * utilSafeConvertStringToInteger(aProperties[GOPI_SHIP_SPEED_EXTS]) / 10);

   // Set properties
   pGameObject->oProperties.iMinSpeed = iMin;
   pGameObject->oProperties.iMaxSpeed = iMax;
}


const TCHAR*  identifyGameObjectName(const TCHAR*  szNameID)
{
   GAME_STRING*  pLookup;

   // Lookup name / placeholder
   if (!findGameStringByID(utilSafeConvertStringToInteger(szNameID), GPI_SHIPS_STATIONS_WARES, pLookup))
      findGameStringByID(SGOI_UNDEFINED_NAME, GPI_SHIPS_STATIONS_WARES, pLookup);
      
   // Return name / NULL
   return pLookup ? pLookup->szText : NULL;
}


const TCHAR*  identifyGameObjectDescription(const TCHAR*  szNameID)
{
   GAME_STRING*  pLookup;

   // Return description / NULL
   findGameStringByID(1 + utilSafeConvertStringToInteger(szNameID), GPI_SHIPS_STATIONS_WARES, pLookup);
   return pLookup ? pLookup->szText : NULL;
}


const TCHAR*  identifyGameObjectMainType(const MAIN_TYPE  eMainType)
{
   GAME_STRING*  pLookup;

   // Return string / NULL
   findMainTypeGameStringByID(eMainType, pLookup);
   return pLookup ? pLookup->szText : NULL;
}


const TCHAR*  identifyGameObjectTransportClass(const TCHAR*  szClassID)
{
   GAME_STRING*  pLookup;

   // Return string / NULL
   findGameStringByID(10 + utilSafeConvertStringToInteger(szClassID), GPI_TRANSPORT_CLASSES, pLookup);
   return pLookup ? pLookup->szText : NULL;
}


const TCHAR*  identifyGameObjectStationSize(const STATION_SIZE  eSize)
{
   GAME_STRING*  pLookup;

   // Return string / NULL
   findGameStringByID(convertStationSizeToStringID(eSize), GPI_STATION_SERIALS, pLookup);
   return pLookup ? pLookup->szText : NULL;
}


const TCHAR*  identifyGameObjectManufacturer(const TCHAR*  szRace)
{
   GAME_STRING*  pLookup;

   // Return string / NULL
   findGameStringByID(utilSafeConvertStringToInteger(szRace), GPI_RACES, pLookup);
   return pLookup ? pLookup->szText : NULL;
}

   
const TCHAR*  identifyGameObjectShipClass(const TCHAR*  szType)
{
   GAME_STRING*  pLookup = NULL;

   for (UINT  iIndex = 0; iIndex < 27; iIndex++)
   {
      // Compare type
      if (utilCompareStringVariables(szShipClasses[iIndex].szClass, szType))
      {
         // [FOUND] Lookup matching name
         findGameStringByID(szShipClasses[iIndex].iStringID, GPI_OBJECT_CLASSES, pLookup);
         break;
      }
   }
   
   // Return class name / Null
   return pLookup ? pLookup->szText : NULL;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

BOOL  translateGameObjectProperties(GAME_OBJECT*  pGameObject, CONST TCHAR**  aProperties, ERROR_QUEUE*  pErrorQueue)
{
   STATION_SIZE       eStationSize;
   OBJECT_PROPERTIES  oRawValues;
   UINT               iCommonOffset;

   // Prepare
   utilZeroObject(&oRawValues, OBJECT_PROPERTIES);

   // Examine type
   switch (pGameObject->eMainType)
   {
   // []
   case MT_DOCK: 
      // Set common offset
      iCommonOffset = 18;

      // Extract individual properties
      oRawValues.szManufacturer = aProperties[GOPI_STATION_RACE_ID];
      break;

   case MT_FACTORY:        
      // Set common offset
      iCommonOffset = 19;

      // Extract individual properties
      oRawValues.szManufacturer = aProperties[GOPI_STATION_RACE_ID];
      oRawValues.szStationSize  = aProperties[GOPI_FACTORY_SIZE_ID];
      eStationSize = (STATION_SIZE)utilConvertStringToInteger(oRawValues.szStationSize);
      break;

   case MT_WARE_LASER:     
      // Set common offset
      iCommonOffset = 13;

      // Extract individual properties
      pGameObject->oProperties.iMaxEnergy = utilConvertStringToInteger(aProperties[GOPI_LASER_ENERGY]);
      break;

   case MT_WARE_SHIELD:    
      // Set common offset
      iCommonOffset = 11;

      // Extract individual properties
      pGameObject->oProperties.iMaxEnergy = utilConvertStringToInteger(aProperties[GOPI_SHIELD_ENERGY]);
      break;

   case MT_WARE_MISSILE:   
      iCommonOffset = 27;     
      break;

   /// [WARE]
   default:
      // Set common offset
      iCommonOffset = 7;
      break;

   /// [SHIP]
   case MT_SHIP:
      // Calculate common offset
      iCommonOffset = identifyCommonShipOffset(aProperties);

      /// Extract individual properties
      oRawValues.szManufacturer   = aProperties[GOPI_SHIP_RACE_ID];
      oRawValues.szTransportClass = aProperties[GOPI_SHIP_MAX_CARGO_SIZE];

      // Store hull and cargobay size
      pGameObject->oProperties.iMaxHull     = utilSafeConvertStringToInteger(aProperties[GOPI_SHIP_HULL_STRENGTH]);
      pGameObject->oProperties.iMaxShields  = utilSafeConvertStringToInteger(aProperties[GOPI_SHIP_MAX_SHIELDS]);
      pGameObject->oProperties.iShieldType  = utilSafeConvertStringToInteger(aProperties[GOPI_SHIP_SHIELD_TYPE]);
      pGameObject->oProperties.iMaxDocked   = utilSafeConvertStringToInteger(aProperties[GOPI_SHIP_DOCKING_SLOTS]);
      pGameObject->oProperties.iMinCargobay = utilSafeConvertStringToInteger(aProperties[GOPI_SHIP_CARGO_MIN]);
      pGameObject->oProperties.iMaxCargobay = utilSafeConvertStringToInteger(aProperties[GOPI_SHIP_CARGO_MAX]);

      /// Identify class + Calculate speed
      pGameObject->oProperties.szClass = identifyGameObjectShipClass(aProperties[GOPI_SHIP_CLASS]);
      calculateGameObjectSpeed(pGameObject, aProperties);
      break;
   }

   /// Calculate price
   calculateGameObjectPrice(pGameObject, iCommonOffset, aProperties);

   /// Resolve properties
   pGameObject->oProperties.szRawName     = identifyGameObjectName(aProperties[GOPI_NAME_ID]);
   pGameObject->oProperties.szDescription = identifyGameObjectDescription(aProperties[GOPI_NAME_ID]);
   pGameObject->oProperties.szMainType    = identifyGameObjectMainType(pGameObject->eMainType);
   pGameObject->oProperties.iVolume       = utilSafeConvertStringToInteger(aProperties[iCommonOffset + GOPI_VOLUME]);

   // [TRANSPORT CLASS]
   oRawValues.szTransportClass = (pGameObject->eMainType == MT_SHIP ? aProperties[GOPI_SHIP_MAX_CARGO_SIZE] : aProperties[iCommonOffset + GOPI_TRANSPORT_CLASS]);
   pGameObject->oProperties.szTransportClass = identifyGameObjectTransportClass(oRawValues.szTransportClass);

   // [STATION SIZE]
   if (oRawValues.szStationSize AND eStationSize != SS_NONE)
      pGameObject->oProperties.szStationSize = identifyGameObjectStationSize(eStationSize);

   // [MANUFACTURER]
   if (oRawValues.szManufacturer)
      pGameObject->oProperties.szManufacturer = identifyGameObjectManufacturer(oRawValues.szManufacturer);
   
   // Return TRUE
   return TRUE;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////


