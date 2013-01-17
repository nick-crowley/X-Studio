//
// Game Object Properties.cpp : Extended properties for GameObjects
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        MACROS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

#define     GOPI_VOLUME               0
#define     GOPI_RELATIVE_VALUE       1
#define     GOPI_PRICE_MODIFIER_1     2
#define     GOPI_PRICE_MODIFIER_2     3
#define     GOPI_TRANSPORT_CLASS      4
#define     GOPI_RELATIVE_PL_VALUE    5

#define     GOPI_LASER_ENERGY         10
#define     GOPI_SHIELD_ENERGY        8


#define     GOPI_SHIP_SHIELD_TYPE     22
#define     GOPI_SHIP_MAX_SHIELDS     23
#define     GOPI_SHIP_CARGO_MIN       28
#define     GOPI_SHIP_CARGO_MAX       29
#define     GOPI_SHIP_WARE_LIST       30
#define     GOPI_SHIP_DOCKING_SLOTS   43
#define     GOPI_SHIP_MAX_CARGO_SIZE  44
#define     GOPI_SHIP_RACE            45
#define     GOPI_SHIP_HULL_STRENGTH   46
#define     GOPI_SHIP_VARIATION       50
#define     GOPI_SHIP_TURRET_COUNT    53
#define     GOPI_SHIP_GUN_GROUP_COUNT 54 + (4 * GOPI_SHIP_TURRET_COUNT)

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

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

BOOL  translateGameObjectProperties(GAME_OBJECT*  pGameObject, CONST TCHAR**  aProperties, ERROR_QUEUE*  pErrorQueue)
{
   GAME_STRING  *pLookupString;
   STATION_SIZE  eStationSize;
   UINT          iCommonOffset,
                 iCurrentProperty,   // [SHIP] 
                 iTurretCount,       // [SHIP] 
                 iGunCount,          // [SHIP] 
                 iWeaponCount,       // [SHIP] 
                 iRelativeValue,
                 iAveragePrice,
                 iPriceModifier1,
                 iPriceModifier2;

   OBJECT_PROPERTIES  oRawValues;

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
      // Start at 'Turret Count'
      iCurrentProperty = GOPI_SHIP_TURRET_COUNT;

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
      iCommonOffset = iCurrentProperty;

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
      break;
   }

   /// Extract common properties
   // Store Volume
   pGameObject->oProperties.iVolume = utilSafeConvertStringToInteger(aProperties[iCommonOffset + GOPI_VOLUME]);

   // [NON-SHIP] Extract container size
   if (pGameObject->eMainType != MT_SHIP)
      oRawValues.szTransportClass = aProperties[iCommonOffset + GOPI_TRANSPORT_CLASS];

   // Extract price components
   iPriceModifier1 = utilSafeConvertStringToInteger(aProperties[iCommonOffset + GOPI_PRICE_MODIFIER_1]);
   iPriceModifier2 = utilSafeConvertStringToInteger(aProperties[iCommonOffset + GOPI_PRICE_MODIFIER_2]);
   iRelativeValue  = utilSafeConvertStringToInteger(aProperties[iCommonOffset + GOPI_RELATIVE_VALUE]);

   // Calculate average price
   iAveragePrice = (iRelativeValue * iPriceModifiers[pGameObject->eMainType]);
   
   /// Resolve properties
   // [NAME] 
   if (findGameStringByID(utilSafeConvertStringToInteger(aProperties[GOPI_NAME_ID]), GPI_SHIPS_STATIONS_WARES, pLookupString))
      pGameObject->oProperties.szRawName = pLookupString->szText;
   // [MISSING] Use *** UNDEFINED *** instead
   else if (findGameStringByID(SGOI_UNDEFINED_NAME, GPI_SHIPS_STATIONS_WARES, pLookupString))
      pGameObject->oProperties.szRawName = pLookupString->szText;

   // [DESCRIPTION] 
   if (findGameStringByID(1 + utilSafeConvertStringToInteger(aProperties[GOPI_NAME_ID]), GPI_SHIPS_STATIONS_WARES, pLookupString))
      pGameObject->oProperties.szDescription = pLookupString->szText;

   // [MAINTYPE]
   if (findMainTypeGameStringByID(pGameObject->eMainType, pLookupString))
      pGameObject->oProperties.szMainType = pLookupString->szText;

   // [TRANSPORT CLASS]
   if (oRawValues.szTransportClass AND findGameStringByID(10 + utilSafeConvertStringToInteger(oRawValues.szTransportClass), GPI_TRANSPORT_CLASSES, pLookupString))
      pGameObject->oProperties.szTransportClass = pLookupString->szText;

   // [STATION SIZE]
   if (oRawValues.szStationSize AND eStationSize != SS_NONE AND findGameStringByID(convertStationSizeToStringID(eStationSize), GPI_STATION_SERIALS, pLookupString))
      pGameObject->oProperties.szStationSize = pLookupString->szText;

   // [MANUFACTURER]
   if (oRawValues.szManufacturer AND findGameStringByID(utilSafeConvertStringToInteger(oRawValues.szManufacturer), GPI_RACES, pLookupString))
      pGameObject->oProperties.szManufacturer = pLookupString->szText;

   /// Calculate price
   pGameObject->oProperties.iMinPrice = iAveragePrice - utilCalculatePercentage(iAveragePrice, iPriceModifier1);
   pGameObject->oProperties.iMaxPrice = iAveragePrice + utilCalculatePercentage(iAveragePrice, iPriceModifier1);
   
   // Return TRUE
   return TRUE;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////


