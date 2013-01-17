//
// Drawing.cpp : Helper functions for drawing
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

/// Function name  : utilCreateDeviceContextState
// Description     : Creates an empty DeviceContextState
// 
// HDC  hDC   : [in] Device Context to encapsulate
// 
// Return Value   : New DeviceContextState, you are responsible for destroying it
// 
UtilityAPI 
DC_STATE*  utilCreateDeviceContextState(HDC  hDC)
{
   DC_STATE*   pDeviceContextState;

   // Create object
   pDeviceContextState = utilCreateEmptyObject(DC_STATE);

   // Store DC
   pDeviceContextState->hDC = hDC;

   // Return object
   return pDeviceContextState;
}


/// Function name  : utilCreateImageList
// Description     : Convenience function for creating icon ImageLists
// 
// HINSTANCE    hInstance   : [in] Instance containing the icons
// CONST UINT   iIconSize   : [in] Icon dimensions
// CONST UINT   iIconCount  : [in] Number of icons to add
// CONST CHAR*  ...         : [in] Resource IDs of the icons to add
///                                 Resource IDs must be specified as ANSI strings for convenience
// 
// Return Value   : New ImageList, you are responsible for destroying it
// 
UtilityAPI 
HIMAGELIST  utilCreateImageList(HINSTANCE  hInstance, CONST UINT  iIconSize, CONST UINT  iIconCount, ...)
{
   HIMAGELIST  hImageList;    // ImageList being created
   va_list     pArguments;    // VariableArguments pointer
   HICON       hIcon;         // Icon being loaded
   
   // Prepare
   pArguments = utilGetFirstVariableArgument(&iIconCount);
   
   /// Create ImageList
   hImageList = ImageList_Create(iIconSize, iIconSize, ILC_MASK WITH ILC_COLOR32, iIconCount, 1);

   /// Iterate through icon resource IDs
   for (UINT iArgument = 0; iArgument < iIconCount; iArgument++)
   {
      /// Load icon and insert into ImageList
      hIcon = LoadIconA(hInstance, utilGetCurrentVariableArgument(pArguments, CONST CHAR*));
      ImageList_AddIcon(hImageList, hIcon);

      // Advance to next resource ID
      pArguments = utilGetNextVariableArgument(pArguments, TCHAR*);
   }

   // Return new ImageList
   return hImageList;
}


/// Function name  : utilDeleteDeviceContextState
// Description     : Destroys a DeviceContextState, reverting any remaining changes in the process
// 
// DC_STATE*  &pPreviousState   : [in] DeviceContext state to revert and destroy
// 
UtilityAPI 
VOID  utilDeleteDeviceContextState(DC_STATE*  &pPreviousState)
{
   // Restore DC properties
   utilRestoreDeviceContext(pPreviousState);

   // Destroy calling object
   utilDeleteObject(pPreviousState);
}


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : utilRestoreDeviceContext
// Description     : Reverts a DeviceContext to the state it was in when the object was created
// 
// DC_STATE*  pPrevState   : [in] DeviceContext state
// 
UtilityAPI 
VOID  utilRestoreDeviceContext(DC_STATE*  pPrevState)
{
   // [BACKGROUND MODE]
   if (pPrevState->eState INCLUDES DCC_BACKGROUND_MODE)
   {
      SetBkMode(pPrevState->hDC, pPrevState->iOldBackgroundMode);
      pPrevState->eState ^= DCC_BACKGROUND_MODE;
   }

   // [BACKGROUND COLOUR]
   if (pPrevState->eState INCLUDES DCC_BACKGROUND_COLOUR)
   {
      SetBkColor(pPrevState->hDC, pPrevState->clOldBackgroundColour);
      pPrevState->eState ^= DCC_BACKGROUND_COLOUR;
   }

   // [BITMAP]
   if (pPrevState->eState INCLUDES DCC_BITMAP)
   {
      SelectBitmap(pPrevState->hDC, pPrevState->hOldBitmap);
      pPrevState->eState ^= DCC_BITMAP;
   }

   // [BRUSH]
   if (pPrevState->eState INCLUDES DCC_BRUSH)
   {
      SelectBrush(pPrevState->hDC, pPrevState->hOldBrush);
      pPrevState->eState ^= DCC_BRUSH;
   }

   // [FONT]
   if (pPrevState->eState INCLUDES DCC_FONT)
   {
      SelectFont(pPrevState->hDC, pPrevState->hOldFont);
      pPrevState->eState ^= DCC_FONT;
   }

   // [PEN]
   if (pPrevState->eState INCLUDES DCC_PEN)
   {
      SelectPen(pPrevState->hDC, pPrevState->hOldPen);
      pPrevState->eState ^= DCC_PEN;
   }

   // [TEXT COLOUR]
   if (pPrevState->eState INCLUDES DCC_TEXT_COLOUR)
   {
      SetTextColor(pPrevState->hDC, pPrevState->clOldTextColour);
      pPrevState->eState ^= DCC_TEXT_COLOUR;
   }
}


/// Function name  : utilSetDeviceContextBackgroundColour
// Description     : Change a DC's background drawing mode
// 
// DC_STATE*       pPrevState   : [in] DeviceContextState
// CONST COLORREF  clBackground : [in] New background mode
//
UtilityAPI 
VOID  utilSetDeviceContextBackgroundColour(DC_STATE*  pPrevState, CONST COLORREF  clBackground)
{
   /// [COLOUR] Change colour and update state
   pPrevState->clOldBackgroundColour = SetBkColor(pPrevState->hDC, clBackground);
   pPrevState->eState               |= DCC_BACKGROUND_COLOUR;
}


/// Function name  : utilSetDeviceContextBackgroundMode
// Description     : Change a DC's background drawing mode
// 
// DC_STATE*  pPrevState : [in] DeviceContextState
// CONST UINT  iMode     : [in] New background mode
//
UtilityAPI 
VOID  utilSetDeviceContextBackgroundMode(DC_STATE*  pPrevState, CONST UINT  iMode)
{
   /// [BACKGROUND] Change mode and update state
   pPrevState->iOldBackgroundMode = SetBkMode(pPrevState->hDC, iMode);
   pPrevState->eState            |= DCC_BACKGROUND_MODE;
}



/// Function name  : utilSetDeviceContextBitmap
// Description     : Selects a bitmap into a device context
// 
// DC_STATE*  pPrevState : [in] DeviceContextState
// HBITMAP    hBitmap    : [in] New bitmap to apply
//
UtilityAPI 
VOID  utilSetDeviceContextBitmap(DC_STATE*  pPrevState, HBITMAP  hBitmap)
{
   /// [BITMAP] Change mode and update state
   pPrevState->hOldBitmap = SelectBitmap(pPrevState->hDC, hBitmap);
   pPrevState->eState    |= DCC_BITMAP;
}


/// Function name  : utilSetDeviceContextBrush
// Description     : Selects a brush into a device context
// 
// DC_STATE*  pPrevState : [in] DeviceContextState
// HBRUSH     hBrush     : [in] New brush to select
//
UtilityAPI 
VOID  utilSetDeviceContextBrush(DC_STATE*  pPrevState, HBRUSH  hBrush)
{
   /// [BRUSH] Change mode and update state
   pPrevState->hOldBrush = SelectBrush(pPrevState->hDC, hBrush);
   pPrevState->eState   |= DCC_BRUSH;
}


/// Function name  : utilSetDeviceContextFont
// Description     : Change the font and text colour in a DeviceContext
// 
// DC_STATE*  pPrevState : [in] device context state
// HFONT      hFont      : [in] [opional] New font to apply
// COLORREF   clColour   : [in] New text colour to display
// 
UtilityAPI 
VOID  utilSetDeviceContextFont(DC_STATE*  pPrevState, HFONT  hFont, COLORREF  clColour)
{
   /// [FONT] Change font and update flags
   if (hFont)
   {
      pPrevState->hOldFont = SelectFont(pPrevState->hDC, hFont);
      pPrevState->eState  |= DCC_FONT;
   }

   /// [TEXT COLOUR] Change colour and update flags
   pPrevState->clOldTextColour = SetTextColor(pPrevState->hDC, clColour);
   pPrevState->eState         |= DCC_TEXT_COLOUR;
}


/// Function name  : utilSetDeviceContextPen
// Description     : Selects a pen into a device context
// 
// DC_STATE*  pPrevState : [in] DeviceContextState
// HPEN       hPen       : [in] New pen to apply
//
UtilityAPI 
VOID  utilSetDeviceContextPen(DC_STATE*  pPrevState, HPEN  hPen)
{
   /// [PEN] Change mode and update state
   pPrevState->hOldPen  = SelectPen(pPrevState->hDC, hPen);
   pPrevState->eState  |= DCC_PEN;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////



