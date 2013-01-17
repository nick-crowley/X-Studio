//
// Custom Tooltips.cpp : Custom RichText Tooltips
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

// Error message displayed when a Tooltip description is missing 
CONST TCHAR*      szErrorSource      = TEXT("[red][b]ERROR:[/b][/red] Tooltip title or description not found");

// Tooltip dimensions
CONST INT         iTooltipBorder = 8;           // Tooltip icon border

// Tooltip icon size
IMAGE_TREE_SIZE   eTooltipIconSize   = ITS_LARGE; 

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createCustomTooltipData
// Description     : Creates Custom Tooltip data
// 
// CONST RESULT_TYPE  eType   : [in] Type of object
// SUGGESTION_RESULT  xObject : [in] Object data
// 
// Return Value   : New data, you are responsible for destroying it
// 
ControlsAPI
CUSTOM_TOOLTIP*   createCustomTooltipData(CONST RESULT_TYPE  eType, SUGGESTION_RESULT  xObject)
{
   CUSTOM_TOOLTIP*  pCustomTooltip = utilCreateEmptyObject(CUSTOM_TOOLTIP);

   // Set properties
   pCustomTooltip->eType   = eType;
   pCustomTooltip->xObject = xObject;

   // Return object
   return pCustomTooltip;
}


/// Function name  : deleteCustomTooltipData
// Description     : Destroys Custom Tooltip data
// 
// CUSTOM_TOOLTIP*  &pCustomTooltip   : [in] Custom Tooltip data
// 
ControlsAPI
VOID  deleteCustomTooltipData(CUSTOM_TOOLTIP*  &pCustomTooltip)
{
   // Delete calling object
   utilDeleteObject(pCustomTooltip);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////


/// Function name  : drawComplexCustomTooltip
// Description     : Draws a simple tooltip with an icon, title and description
// 
// HDC               hDC              : [in]     Target DC
// RECT*             pOutputRect      : [in/out] Drawing/Calculation content rectangle of the tooltip
// CONST TCHAR*      szTitleIcon      : [in]     Resource ID of an icon within the LARGE application ImageTree
// CONST RICH_TEXT*  pTitleText       : [in]     Title text
// CONST RICH_TEXT*  pDescriptionText : [in]     Description text
// CONST BOOL        bCalculation     : [in]     Whether to only calculate the necessary drawing rectangle
// 
ControlsAPI
VOID   drawComplexCustomTooltip(HDC  hDC, RECT*  pOutputRect, CONST UINT  iWidth, CONST TCHAR*  szTitleIcon, CONST RICH_TEXT*  pTitleText, CONST RICH_TEXT*  pBasicText, CONST RICH_TEXT*  pAdvancedText, CONST RICH_TEXT*  pDescriptionText, CONST BOOL  bCalculation)
{
   RECT        rcTitle,          // Title rectangle
               rcDescription,    // Description rectangle
               rcBasic,         // Custom data
               rcAdvanced;
   INT         iPrevMode,        //
               iIconOffset;      // Vertical Offset for icon, used to centre vertically
               //iTitleBottom;


   /// [CALCULATION]
   if (bCalculation)
   {
      // [TITLE/DESCRIPTION] Define initial rectangle manually
      SetRect(&rcTitle, 0, 0, iWidth, eTooltipIconSize);
      rcDescription = rcTitle;

      // [BASIC/ADVANCED] Define as half-width
      SetRect(&rcBasic, 0, 0, iWidth / 2, eTooltipIconSize);
      rcAdvanced = rcBasic;

      /// [TITLE] Calculate title rectangle
      rcTitle.left += (eTooltipIconSize + iTooltipBorder);
      drawRichText(hDC, pTitleText, &rcTitle, DT_CALCRECT);

      // [CHECK] Ensure minimum height of title
      rcTitle.bottom = max(rcTitle.top + eTooltipIconSize, rcTitle.bottom);

      /// [BASIC/ADVANCED] Calculate rectangles
      drawRichText(hDC, pBasicText, &rcBasic, DT_CALCRECT);
      if (pAdvancedText)
         drawRichText(hDC, pAdvancedText, &rcAdvanced, DT_CALCRECT);

      /// [DESCRIPTION] Calculate description rectangle
      drawRichText(hDC, pDescriptionText, &rcDescription, DT_CALCRECT);

      // [OUTPUT] Combine rectangles
      SetRect(pOutputRect, 0, 0, iWidth, rcTitle.bottom + iTooltipBorder + max(rcBasic.bottom, rcAdvanced.bottom) + iTooltipBorder + rcDescription.bottom); 
      InflateRect(pOutputRect, 0, (INT)iTooltipBorder);
   }
   /// [DRAWING]
   else
   {
      // Prepare
      iPrevMode = SetBkMode(hDC, TRANSPARENT);

      // [BORDER] Contract rectangle to create top/bottom borders
      InflateRect(pOutputRect, 0, -(INT)iTooltipBorder);
      rcTitle = rcBasic = rcAdvanced = rcDescription = *pOutputRect;

      // [TITLE] Re-calculate title rectangle
      rcTitle.left += (eTooltipIconSize + iTooltipBorder);
      drawRichText(hDC, pTitleText, &rcTitle, DT_CALCRECT);
      
      // [CALC] Ensure title height >= icon height.  Vertically centre icon
      rcTitle.bottom = max(rcTitle.top + eTooltipIconSize, rcTitle.bottom);
      iIconOffset = (rcTitle.bottom - rcTitle.top - eTooltipIconSize) / 2;

      /// [ICON/TITLE] Draw appropriate icon and title
      drawImageTreeIcon(eTooltipIconSize, szTitleIcon, hDC, pOutputRect->left, rcTitle.top + iIconOffset, IS_NORMAL);
      drawRichText(hDC, pTitleText, &rcTitle, DT_VCENTER WITH DT_CENTER);

      // [CALC] Position properties beneath title. Split basic/advanced rectangles
      rcBasic.top = rcAdvanced.top = pOutputRect->top + max(eTooltipIconSize, rcTitle.bottom - rcTitle.top) + iTooltipBorder;
      rcBasic.right   -= (pOutputRect->right - pOutputRect->left) / 2;
      rcAdvanced.left += (pOutputRect->right - pOutputRect->left) / 2;

      /// [BASIC] Draw properties
      drawRichText(hDC, pBasicText, &rcBasic, NULL);
      if (pAdvancedText)
         drawRichText(hDC, pAdvancedText, &rcAdvanced, NULL);

      // [CALC] Position description beneath properties
      rcDescription.top = (pAdvancedText ? max(rcBasic.bottom, rcAdvanced.bottom) : rcBasic.bottom) + iTooltipBorder;

      /// [DESCRIPTION] Draw description
      drawRichText(hDC, pDescriptionText, &rcDescription, NULL);

      // Cleanup
      SetBkMode(hDC, iPrevMode);
   }

}


/// Function name  : drawSimpleCustomTooltip
// Description     : Draws a simple tooltip with an icon, title and description
// 
// HDC               hDC              : [in]     Target DC
// RECT*             pOutputRect      : [in/out] Drawing/Calculation content rectangle of the tooltip
// CONST TCHAR*      szTitleIcon      : [in]     Resource ID of an icon within the LARGE application ImageTree
// CONST RICH_TEXT*  pTitleText       : [in]     Title text
// CONST RICH_TEXT*  pDescriptionText : [in]     Description text
// CONST BOOL        bCalculation     : [in]     Whether to only calculate the necessary drawing rectangle
// 
ControlsAPI
VOID   drawSimpleCustomTooltip(HDC  hDC, RECT*  pOutputRect, CONST UINT  iWidth, CONST TCHAR*  szTitleIcon, CONST RICH_TEXT*  pTitleText, CONST RICH_TEXT*  pDescriptionText, CONST BOOL  bCalculation)
{
   RICH_TEXT*  pErrorMessage;    // Error message displayed when a tooltip is missing
   RECT        rcTitle,          // Title rectangle
               rcDescription;    // Description rectangle
   INT         iPrevMode,        //
               iIconOffset;      // Vertical Offset for icon, used to centre vertically

   // Prepare
   pErrorMessage = NULL;

   // [CHECK] Ensure title and description exist
   if (!pTitleText OR !pDescriptionText)
      generateRichTextFromSourceText(szErrorSource, lstrlen(szErrorSource), ST_INTERNAL, pErrorMessage, RTT_RICH_TEXT, NULL);

   /// [CALCULATION]
   if (bCalculation)
   {
      // [TITLE] Define initial rectangle manually  (Fixed at 600 pixels wide)
      SetRect(&rcTitle, 0, 0, iWidth, eTooltipIconSize);
      rcDescription = rcTitle;

      /// [TITLE] Calculate title rectangle
      rcTitle.left += (eTooltipIconSize + iTooltipBorder);
      drawRichText(hDC, (pTitleText ? pTitleText : pErrorMessage), &rcTitle, DT_CALCRECT);

      // [CHECK] Ensure minimum height of title
      rcTitle.bottom = max(rcTitle.top + eTooltipIconSize, rcTitle.bottom);

      /// [DESCRIPTION] Calculate description rectangle
      drawRichText(hDC, (pDescriptionText ? pDescriptionText : pErrorMessage), &rcDescription, DT_CALCRECT);

      // [OUTPUT] Combine rectangles
      SetRect(pOutputRect, 0, 0, iWidth, rcTitle.bottom + rcDescription.bottom + (3 * iTooltipBorder));  // 3 Gaps: top/bottom edge and split between title/description
   }
   /// [DRAWING]
   else
   {
      // Prepare
      iPrevMode = SetBkMode(hDC, TRANSPARENT);

      // [BORDER] Contract rectangle to create top/bottom borders
      InflateRect(pOutputRect, 0, -(INT)iTooltipBorder);
      rcTitle = rcDescription = *pOutputRect;

      // [TITLE] Re-calculate title rectangle
      rcTitle.left += (eTooltipIconSize + iTooltipBorder);
      drawRichText(hDC, (pTitleText ? pTitleText : pErrorMessage), &rcTitle, DT_CALCRECT);
      
      // [CALC] Ensure title height >= icon height.  Vertically centre icon
      rcTitle.bottom = max(rcTitle.top + eTooltipIconSize, rcTitle.bottom);
      iIconOffset = (rcTitle.bottom - rcTitle.top - eTooltipIconSize) / 2;

      /// [ICON/TITLE] Draw appropriate icon and title
      drawImageTreeIcon(eTooltipIconSize, szTitleIcon, hDC, pOutputRect->left, rcTitle.top + iIconOffset, IS_NORMAL);
      drawRichText(hDC, (pTitleText ? pTitleText : pErrorMessage), &rcTitle, DT_VCENTER WITH DT_CENTER);

      // [DESCRIPTION] Position beneath title/icon, whichever if lowest
      rcDescription.top += max(eTooltipIconSize, rcTitle.bottom - rcTitle.top) + iTooltipBorder;

      /// [DESCRIPTION] Draw description
      drawRichText(hDC, (pDescriptionText ? pDescriptionText : pErrorMessage), &rcDescription, NULL);

      // Cleanup
      SetBkMode(hDC, iPrevMode);
   }

   // Cleanup
   if (pErrorMessage)
      deleteRichText(pErrorMessage);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////

CONST TCHAR*  szNoCommandDescription = TEXT("No description has been written for this command yet");

VOID  drawCommandSyntaxCustomTooltip(HDC  hDC, RECT*  pDrawRect, CONST COMMAND_SYNTAX*  pSyntax, CONST BOOL  bCalculation)
{
   RICH_TEXT    *pRichDescription;  // Placeholder description

   // Prepare
   pRichDescription = NULL;

   // [CHECK] Has we written a description yet?
   if (!pSyntax->pTooltipDescription)
      // [FAILED] Generate a placeholder
      generateRichTextFromSourceText(szNoCommandDescription, lstrlen(szNoCommandDescription), ST_INTERNAL, pRichDescription, RTT_RICH_TEXT, NULL); 

   /// [DRAW] Draw simple tooltip
   drawSimpleCustomTooltip(hDC, pDrawRect, 500, identifyGameVersionIconID(pSyntax->eGameVersion), pSyntax->pTooltipSyntax, utilEither(pSyntax->pTooltipDescription, pRichDescription), bCalculation);

   // Cleanup
   if (pRichDescription)
      deleteRichText(pRichDescription);
}



CONST TCHAR*  szNoGameObjectDescription = TEXT("This object has no description");

VOID  drawGameObjectCustomTooltip(HDC  hDC, RECT*  pDrawRect, CONST OBJECT_NAME*  pGameObject, CONST BOOL  bCalculation)
{
   CONST TCHAR  *szIcon;            // Resource ID of icon
   RICH_TEXT    *pRichDescription,  // RichText description
                *pRichBasic,
                *pRichAdvanced,
                *pRichTitle;        // RichText title
   TCHAR        *szSource;          // SourceText for generating title

   // Prepare
   szSource = utilCreateEmptyString(1024);
   pRichAdvanced = NULL;

   // [ICON] Use a fixed icon
   szIcon = TEXT("GAME_OBJECT_ICON");

   /// [TITLE] Generate title containing NAME, ID, MAINTYPE and SUBTYPE
   StringCchPrintf(szSource, 1024, TEXT("[center][b]%s[/b]\n") TEXT("%s\n\n") TEXT("[b]ID:[/b] %s\t\t\t") TEXT("[b]SubType[/b]: %u[/center]"), utilEither(pGameObject->oProperties.szRawName, pGameObject->szText), pGameObject->oProperties.szMainType, pGameObject->szObjectID, pGameObject->iID);
   generateRichTextFromSourceText(szSource, lstrlen(szSource), ST_INTERNAL, pRichTitle, RTT_RICH_TEXT, NULL);

   /// [COMMON PROPERTIES]
   StringCchPrintf(szSource, 1024, TEXT("[b][u]Properties:[/u][/b]\n\n") TEXT("[b]Trading Price[/b]: %u - %u cr\n") TEXT("[b]Unit Volume[/b]: %u units\n") TEXT("[b]Containers[/b]: %s"), pGameObject->oProperties.iMinPrice, pGameObject->oProperties.iMaxPrice, pGameObject->oProperties.iVolume, pGameObject->oProperties.szTransportClass);
   generateRichTextFromSourceText(szSource, lstrlen(szSource), ST_INTERNAL, pRichBasic, RTT_RICH_TEXT, NULL);

   // Examine GameObject type
   switch (pGameObject->eMainType)
   {
   case MT_SHIP:
      StringCchPrintf(szSource, 1024, TEXT("[right]\n\n") TEXT("%u - %u m/s : [b]Ship Speed[/b]\n") TEXT("%u - %u units : [b]Cargobay Volume[/b]\n") TEXT("%u ships : [b]Docking Ports[/b]\n") TEXT("%s : [b]Manufacturer[/b][/right]"), pGameObject->oProperties.iMaxSpeed, pGameObject->oProperties.iMinSpeed, pGameObject->oProperties.iMinCargobay, pGameObject->oProperties.iMaxCargobay, pGameObject->oProperties.iMaxDocked, pGameObject->oProperties.szManufacturer);
      generateRichTextFromSourceText(szSource, lstrlen(szSource), ST_INTERNAL, pRichAdvanced, RTT_RICH_TEXT, NULL);
      break;

   case MT_DOCK:
   case MT_FACTORY:
      break;

   case MT_WARE_LASER:
   case MT_WARE_SHIELD:
      StringCchPrintf(szSource, 1024, TEXT("[right]\n\n") TEXT("%u joules : [b]Total Energy[/b][/right]"), pGameObject->oProperties.iMaxEnergy);
      generateRichTextFromSourceText(szSource, lstrlen(szSource), ST_INTERNAL, pRichAdvanced, RTT_RICH_TEXT, NULL);
      break;
   }

   /// [DESCRIPTION] Generate description
   StringCchPrintf(szSource, 1024, TEXT("[b][u]Description:[/u][/b]\n\n") TEXT("%s"), pGameObject->oProperties.szDescription ? pGameObject->oProperties.szDescription : szNoGameObjectDescription);
   generateRichTextFromSourceText(szSource, lstrlen(szSource), ST_INTERNAL, pRichDescription, RTT_RICH_TEXT, NULL);     //TEXT("[center]%s[/center]")

   /// [DRAW] Draw complex tooltip
   drawComplexCustomTooltip(hDC, pDrawRect, 400, szIcon, pRichTitle, pRichBasic, pRichAdvanced, pRichDescription, bCalculation);

   // Cleanup
   if (pRichAdvanced)
      deleteRichText(pRichAdvanced);

   // Cleanup
   deleteRichText(pRichTitle);
   deleteRichText(pRichBasic);
   deleteRichText(pRichDescription);
   utilDeleteString(szSource);
}


CONST TCHAR*  szNoScriptObjectDescription = TEXT("No further details are available");

VOID  drawScriptObjectCustomTooltip(HDC  hDC, RECT*  pDrawRect, CONST OBJECT_NAME*  pScriptObject, CONST BOOL  bCalculation)
{
   CONST TCHAR  *szIcon;            // Resource ID of icon
   RICH_TEXT    *pRichDescription,  // RichText placeholder description, used if object has no description
                *pRichTitle;        // RichText title
   TCHAR        *szSource;          // SourceText for generating title

   // Prepare
   pRichDescription = NULL;

   /// [ICON] Use GameVersion or ModContent icon
   szIcon = (pScriptObject->bModContent ? TEXT("SCRIPT_OBJECT_ICON") : identifyGameVersionIconID(pScriptObject->eVersion));

   /// [TITLE] Create Bold version of the object name
   szSource = utilCreateStringf(256, TEXT("[b]%s[/b]"), pScriptObject->szText);
   generateRichTextFromSourceText(szSource, lstrlen(szSource), ST_INTERNAL, pRichTitle, RTT_RICH_TEXT, NULL);

   // [CHECK] Does object have a description?
   if (!pScriptObject->pDescription)
      /// [NO DESCRIPTION] Use fixed string
      generateRichTextFromSourceText(szNoScriptObjectDescription, lstrlen(szNoScriptObjectDescription), ST_INTERNAL, pRichDescription, RTT_RICH_TEXT, NULL);

   /// [DRAW] Draw simple tooltip
   drawSimpleCustomTooltip(hDC, pDrawRect, 300, szIcon, pRichTitle, utilEither(pScriptObject->pDescription, pRichDescription), bCalculation);

   // [OPTIONAL] Delete placeholder description
   if (pRichDescription)
      deleteRichText(pRichDescription);

   // Cleanup
   deleteRichText(pRichTitle);
   utilDeleteString(szSource);
}


/// Function name  : onCustomDrawTooltip
// Description     : Convenience handler for CustomDraw Tooltips
// 
// HWND                   hTooltip     : [in] Tooltip window
// NMTTCUSTOMDRAW*        pHeader      : [in] Tooltip CustomDraw message header
// CONST CUSTOM_TOOLTIP*  pTooltipData : [in] Custom Tooltip data
// 
// Return Value   : Custom draw result code
// 
ControlsAPI
BOOL  onCustomDrawTooltip(HWND  hTooltip, NMTTCUSTOMDRAW*  pHeader, CONST CUSTOM_TOOLTIP*  pTooltipData)
{
   NMCUSTOMDRAW*    pDrawData;        // Convenience pointer
   HFONT            hTooltipFont,     // Tooltip font
                    hOldFont;
   RECT             rcBackground;     // Tooltip Background rectangle

   // Prepare
   pDrawData = &pHeader->nmcd;

   // Create font
   hOldFont = SelectFont(pDrawData->hdc, hTooltipFont = utilCreateFont(pDrawData->hdc, TEXT("MS Shell Dlg 2"), 8, FALSE, FALSE, FALSE));

   // [CHECK] Are we calculating or drawing?
   if (pHeader->uDrawFlags INCLUDES DT_CALCRECT)
   {
      /// Perform calculation
      switch (pTooltipData->eType)
      {
      case RT_COMMANDS:          drawCommandSyntaxCustomTooltip(pDrawData->hdc, &pDrawData->rc, pTooltipData->xObject.asCommandSyntax, TRUE);      break;
      case RT_GAME_OBJECTS:      drawGameObjectCustomTooltip(pDrawData->hdc, &pDrawData->rc, pTooltipData->xObject.asObjectName, TRUE);            break;
      case RT_SCRIPT_OBJECTS:    drawScriptObjectCustomTooltip(pDrawData->hdc, &pDrawData->rc, pTooltipData->xObject.asObjectName, TRUE);          break;
      }

      // Convert content rectangle to background rectangle
      Tooltip_AdjustRect(hTooltip, TRUE, &pDrawData->rc);
   }
   else
   {
      // Prepare
      GetClientRect(hTooltip, &rcBackground);

      // [BACKGROUND] Draw shaded tooltip background
      drawCustomBackgroundRectangle(pDrawData->hdc, &rcBackground);

      // Convert background rectangle to content rectangle
      Tooltip_AdjustRect(hTooltip, FALSE, &pDrawData->rc);

      /// Perform calculation
      switch (pTooltipData->eType)
      {
      case RT_COMMANDS:          drawCommandSyntaxCustomTooltip(pDrawData->hdc, &pDrawData->rc, pTooltipData->xObject.asCommandSyntax, FALSE);     break;
      case RT_GAME_OBJECTS:      drawGameObjectCustomTooltip(pDrawData->hdc, &pDrawData->rc, pTooltipData->xObject.asObjectName, FALSE);           break;
      case RT_SCRIPT_OBJECTS:    drawScriptObjectCustomTooltip(pDrawData->hdc, &pDrawData->rc, pTooltipData->xObject.asObjectName, FALSE);         break;
      }      
   }

   // Cleanup
   SelectFont(pDrawData->hdc, hOldFont);
   DeleteFont(hTooltipFont);

   // Return 'Notify CustomDraw' for calculations, and return 'Skip drawing' for drawings
   return (pHeader->uDrawFlags INCLUDES DT_CALCRECT ? CDRF_NOTIFYITEMDRAW : CDRF_SKIPDEFAULT);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////
