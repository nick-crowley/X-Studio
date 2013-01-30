//
// Colours.cpp : Handle the colours available in the application
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       CONSTANTS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// LanguageMessage Colours
BearScriptAPI CONST COLORREF  clTextColours[GAME_TEXT_COLOURS] = 
{  
   RGB(0,0,0),       RGB(0,0,255),     RGB(42,168,200),  RGB(200,200,200),     // Black, Blue, Cyan, Default
   RGB(13,191,25),   RGB(255,150,20),  RGB(210,20,255),  RGB(204,0,0),         // Green, Orange, Purple, Red
   RGB(233,233,233), RGB(255,255,255), RGB(215,215,0)                          // Silver, White, Yellow
};

/// Tooltip Colours
BearScriptAPI CONST COLORREF  clTooltipColours[GAME_TEXT_COLOURS] = 
{  
   RGB(0,0,0),       RGB(25,55,242),   RGB(42,168,200),  RGB(200,200,200),     // Black, Blue, Cyan, Default
   RGB(13,191,25),   RGB(213,145,28),  RGB(200,36,167),  RGB(180,37,37),       // Green, Orange, Purple, Red 
   RGB(233,233,233), RGB(255,255,255), RGB(180,180,0)                          // Silver, White, Yellow
};

/// CodeEdit Colours
BearScriptAPI CONST COLORREF  clInterfaceColours[INTERFACE_COLOURS] = 
{  
   RGB(0,0,0),                                                                  // Black
   RGB(0,0,255),     RGB(0,115,0),     RGB(70,70,60),    RGB(140,0,0),          // D-Blue, D-Green, D-Grey, D-Red
   RGB(0,230,240),   RGB(0,255,0),     RGB(175,175,175), RGB(255,0,0),          // L-blue, L-green, L-grey, L-red
   RGB(255,150,20),  RGB(210,20,255),  RGB(255,255,0),   RGB(255,255,255)       // Orange, Purple, Yellow, White
};

/// Colour Schemes
CONST COLOUR_SCHEME    oDefaultScheme[4] = 
{ 
   /// Default colour scheme -- BBC Micro style
   TEXT("X-Studio Standard (Default)"),   TEXT("Tahoma"),     10, FALSE, IC_BLACK, 
   //  Arguments   Commands   Comments     GameObjects     Keywords     Labels       Numbers   Operators  ScriptObjs   Strings       Variables        Null       Whitespace
     { IC_ORANGE, IC_WHITE, IC_LIGHT_GREY, IC_LIGHT_BLUE, IC_DARK_BLUE, IC_PURPLE, IC_LIGHT_RED, IC_WHITE, IC_YELLOW, IC_LIGHT_RED, IC_LIGHT_GREEN, IC_LIGHT_GREEN, IC_BLACK },

   /// Exscriptor Emulation -- Black background, few colours
   TEXT("eXscriptor Emulation"),          TEXT("Courier New"), 8, FALSE, IC_BLACK,
   //     Arguments     Commands   Comments    Gameobject  Keywords   Labels       Numbers    Operators ScriptObjs    Strings       Variables        Null        Whitespace
     { IC_LIGHT_GREEN, IC_WHITE, IC_LIGHT_GREY, IC_YELLOW, IC_WHITE, IC_WHITE, IC_LIGHT_BLUE, IC_WHITE, IC_YELLOW, IC_LIGHT_BLUE, IC_LIGHT_GREEN, IC_LIGHT_GREEN, IC_BLACK },

   /// MSCI Emulation -- Black background, almost no colours
   TEXT("M.S.C.I. Emulation"),            TEXT("Arial"),      10, FALSE, IC_BLACK,
   //    Arguments     Commands   Comments      GameObject Keywords    Labels    Numbers  Operators  ScriptObjs  Strings     Variables         Null        Whitespace
     { IC_LIGHT_GREEN, IC_WHITE, IC_LIGHT_GREY, IC_YELLOW, IC_WHITE, IC_YELLOW, IC_YELLOW, IC_WHITE, IC_YELLOW, IC_YELLOW, IC_LIGHT_GREEN, IC_LIGHT_GREEN, IC_BLACK },

   /// Windows Scheme -- Colours that contrast a white background
   TEXT("Visual Studio Style"),           TEXT("Arial"),      10, FALSE, IC_WHITE,
   //  Arguments   Commands   Comments      GameObjs      Keywords     Labels      Numbers     Operators  ScriptObjs      Strings       Variables        Null      Whitespace
     { IC_ORANGE, IC_BLACK, IC_DARK_GREY, IC_LIGHT_BLUE, IC_DARK_BLUE, IC_PURPLE, IC_LIGHT_RED, IC_BLACK, IC_LIGHT_BLUE, IC_LIGHT_RED, IC_DARK_GREEN, IC_DARK_GREEN, IC_WHITE }
};

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                      CREATION / DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createColourScheme
// Description     : Creates a ColourScheme object
// 
// Return Value   : New ColourScheme object, you are responsible for destroying it
// 
COLOUR_SCHEME*  createColourScheme()
{
   // Create empty object
   return utilCreateEmptyObject(COLOUR_SCHEME);
}

/// Function name  : deleteColourScheme
// Description     : Destroys a ColourScheme object
// 
// COLOUR_SCHEME*  &pColourScheme   : [in] ColourScheme object to destroy
// 
VOID  deleteColourScheme(COLOUR_SCHEME*  &pColourScheme)
{
   // Delete scheme
   utilDeleteObject(pColourScheme);
}


/// Function name  : destructorColourScheme
// Description     : Deletes a ColourScheme object within a list
// 
// LPARAM  pColourSchemeListItem : [in] References to a ColourScheme pointer
// 
BearScriptAPI
VOID  destructorColourScheme(LPARAM  pColourSchemeListItem)
{
   deleteColourScheme((COLOUR_SCHEME*&)pColourSchemeListItem);
}


/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                      HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : getGameTextColour
// Description     : Retrieve the RGB colour associated with a specified GameText colour ID
// 
// CONST GAME_TEXT_COLOUR  eColour   : [in] GameText colour ID
// 
// Return Value   : RGB colour
// 
BearScriptAPI
COLORREF  getGameTextColour(CONST GAME_TEXT_COLOUR  eColour)
{
   ASSERT((UINT)eColour < GAME_TEXT_COLOURS);

   return clTextColours[eColour];
}


/// Function name  : getInterfaceColour
// Description     : Retrieve the RGB colour associated with the specified Interface Colour ID
// 
// CONST INTERFACE_COLOUR  eColour   : [in] Interface Colour ID
// 
// Return Value   : RGB colour
// 
BearScriptAPI
COLORREF  getInterfaceColour(CONST INTERFACE_COLOUR  eColour)
{
   ASSERT((UINT)eColour < INTERFACE_COLOURS);

   return clInterfaceColours[eColour];
}


/// Function name  : getTooltipColour
// Description     : Retrieve the RGB colour associated with a specified GameText colour ID
// 
// CONST GAME_TEXT_COLOUR  eColour   : [in] GameText colour ID
// 
// Return Value   : RGB colour
// 
BearScriptAPI
COLORREF  getTooltipColour(CONST GAME_TEXT_COLOUR  eColour)
{
   ASSERT((UINT)eColour < GAME_TEXT_COLOURS);

   return clTooltipColours[eColour];
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                      FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : generateColourSchemeListFromRegistry
// Description     : Enumerates the ColourSchemes in the app registry key and builds them into a list
// 
// Return Value   : List containing zero or more ColourScheme objects
// 
BearScriptAPI
LIST*  generateColourSchemeListFromRegistry()
{
   COLOUR_SCHEME*  pScheme;            // Scheme currently being generated
   TCHAR*          szSubKey;           // Name of sub-key containing the current scheme
   LIST*           pSchemeList;        // Operation result
   HKEY            hSchemeKey;         // Application registry key

   // Prepare
   pSchemeList = createList(destructorColourScheme);
   szSubKey    = utilCreateEmptyString(32);

   /// Iterate through up to 10 colour scheme sub-keys
   for (UINT  iIndex = 0; iIndex < 10; iIndex++)
   {
      // Prepare
      StringCchPrintf(szSubKey, 32, TEXT("%s\\Scheme%u"), getAppRegistrySubKey(ARK_SCHEME_LIST), iIndex);

      // Attempt to open sub-key
      if (hSchemeKey = utilRegistryOpenAppSubKey(getAppRegistryKey(), szSubKey))
      {
         /// Generate new ColourScheme from current sub-key
         if (pScheme = generateColourSchemeFromRegistry(hSchemeKey))
            // [SUCCESS] Add to output list
            appendObjectToList(pSchemeList, (LPARAM)pScheme);

         // Cleanup
         utilRegistryCloseKey(hSchemeKey);
      }
   }

   // Return new list
   utilDeleteString(szSubKey);
   return pSchemeList;
}


/// Function name  : generateColourSchemeFromRegistry
// Description     : Generates a ColourScheme object from a scheme saved in the registry
// 
// HKEY   hSchemeKey : [in] Subkey containing the scheme, already open
// 
// Return Value   : New ColourScheme, you are responsible for destroying it
// 
COLOUR_SCHEME*   generateColourSchemeFromRegistry(HKEY   hSchemeKey)
{
   COLOUR_SCHEME*  pScheme;      // Scheme being created

   // [CHECK] Ensure key exists
   ASSERT(hSchemeKey);

   // Prepare
   pScheme  = createColourScheme();

   /// [NAMES]
   utilRegistryReadString(hSchemeKey, TEXT("szName"), pScheme->szName, 64);
   utilRegistryReadString(hSchemeKey, TEXT("szFontName"), pScheme->szFontName, LF_FACESIZE);

   /// [PROPERTIES]
   utilRegistryReadNumber(hSchemeKey, TEXT("iFontSize"), &pScheme->iFontSize);
   utilRegistryReadNumber(hSchemeKey, TEXT("bFontBold"), &pScheme->bFontBold);
   
   /// [COLOURS]
   utilRegistryReadNumber(hSchemeKey, TEXT("eBackgroundColour"), &pScheme->eBackgroundColour);
   utilRegistryReadObjectArray(hSchemeKey, TEXT("eCodeObjectColours"), pScheme->eCodeObjectColours, INTERFACE_COLOUR, CODEOBJECT_CLASSES);

   // Return scheme
   return pScheme;
}


/// Function name  : saveColourSchemeToRegistry
// Description     : Stores a ColourScheme in the desired key of the registry
// 
// HKEY                  hSchemeKey : [in] Subkey to contain the scheme, already open
// CONST COLOUR_SCHEME*  pScheme    : [in] ColourScheme to save
// 
VOID  saveColourSchemeToRegistry(HKEY  hSchemeKey, CONST COLOUR_SCHEME*  pScheme)
{
   // [CHECK] Ensure key exists
   ASSERT(hSchemeKey);

   // [NAMES]
   utilRegistryWriteString(hSchemeKey, TEXT("szName"), pScheme->szName);
   utilRegistryWriteString(hSchemeKey, TEXT("szFontName"), pScheme->szFontName);

   // [PROPERTIES]
   utilRegistryWriteNumber(hSchemeKey, TEXT("iFontSize"), pScheme->iFontSize);
   utilRegistryWriteNumber(hSchemeKey, TEXT("bFontBold"), pScheme->bFontBold);
   
   // [COLOURS]
   utilRegistryWriteNumber(hSchemeKey, TEXT("eBackgroundColour"), pScheme->eBackgroundColour);
   utilRegistryWriteObjectArray(hSchemeKey, TEXT("eCodeObjectColours"), pScheme->eCodeObjectColours, INTERFACE_COLOUR, CODEOBJECT_CLASSES);
}


