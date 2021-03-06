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

// OnException: Print to console
#define ON_EXCEPTION()     printException(pException)

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////

// Error message displayed when a Tooltip description is missing 
CONST TCHAR*      szErrorSource    = TEXT("[red][b]ERROR:[/b][/red] Tooltip title or description not found");

// Tooltip icon border
CONST INT         iTooltipBorder   = 8;        

// Tooltip icon size
IMAGE_TREE_SIZE   eTooltipIconSize = ITS_LARGE; 

// Placeholders
CONST TCHAR   *szNoCommandDescription      = TEXT("No description has been written for this command yet"),
              *szNoGameObjectDescription   = TEXT("This object has no description"),
              *szNoScriptObjectDescription = TEXT("No further details are available");
               
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

/// Function name  :  formatCurrency
// Description     : Formats a number for display, optionally with a currency symbol
// 
// const UINT    iValue   : [in] Value to format
// TCHAR*        szOutput : [in] Buffer
// UINT          iLength  : [in] Buffer length
// const TCHAR*  szSymbol : [in] Currency symbol  (May be empty string, but must not be NULL)
// 
// Return Value   : Input buffer
// 
const TCHAR*  formatCurrency(const UINT  iValue, TCHAR*  szOutput, UINT  iLength, const TCHAR*  szSymbol)
{
   TCHAR         szValue[32];
   CURRENCYFMT   oFormat = { 0, FALSE, 3, TEXT(""), TEXT(","), 0, 0, (TCHAR*)szSymbol }; //TEXT("€₵")};

   // Format
   StringCchPrintf(szValue, 32, TEXT("%u"), iValue);
   GetCurrencyFormat(LOCALE_INVARIANT, NULL, szValue, &oFormat, szOutput, iLength);

   // Return buffer
   return szOutput;
}

// Helpers for formatting Numbers and Prices
const TCHAR*  formatNumber(const UINT  iNumber, TCHAR*  szOutput, UINT  iLength) { return formatCurrency(iNumber, szOutput, iLength, TEXT("")); };
const TCHAR*  formatPrice(const UINT  iPrice, TCHAR*  szOutput, UINT  iLength)   { return formatCurrency(iPrice, szOutput, iLength, TEXT("₵")); };


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

   TRY
   {
      /// [CALCULATION]
      if (bCalculation)
      {
         // [TITLE/DESCRIPTION] Define initial rectangle manually
         SetRect(&rcTitle, 0, 0, iWidth, eTooltipIconSize);
         rcAdvanced = rcBasic = rcDescription = rcTitle;

         // [BASIC/ADVANCED] Define as half width 
         //SetRect(&rcBasic, 0, 0, iWidth / 2, eTooltipIconSize);
         //rcAdvanced = rcBasic;

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
         /*rcBasic.right   -= (pOutputRect->right - pOutputRect->left) / 2;
         rcAdvanced.left += (pOutputRect->right - pOutputRect->left) / 2;*/

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
   CATCH0("");
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

   TRY
   {
      // Prepare
      pErrorMessage = NULL;

      // [CHECK] Ensure title and description exist
      if (!pTitleText OR !pDescriptionText)
         generateRichTextFromSourceText(szErrorSource, lstrlen(szErrorSource), pErrorMessage, RTT_RICH_TEXT, ST_DISPLAY, NULL);

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
   CATCH0("");
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : drawCommandSyntaxCustomTooltip
// Description     : Draws/Calculates a CommandSyntax tooltip
// 
// HDC                    hDC          : [in]     Target DC
// RECT*                  pDrawRect    : [in/out] Draw/Calc rectangle
// CONST COMMAND_SYNTAX*  pSyntax      : [in]     Syntax
// CONST BOOL             bCalculation : [in]     TRUE to only perform calculation
// 
VOID  drawCommandSyntaxCustomTooltip(HDC  hDC, RECT*  pDrawRect, CONST COMMAND_SYNTAX*  pSyntax, CONST BOOL  bCalculation)
{
   RICH_TEXT*    pRichDescription = NULL;  // Placeholder description

   TRY
   {
      // [CHECK] Has we written a description yet?
      if (!pSyntax->pTooltipDescription)
         // [FAILED] Generate a placeholder
         generateRichTextFromSourceText(szNoCommandDescription, lstrlen(szNoCommandDescription), pRichDescription, RTT_RICH_TEXT, ST_DISPLAY, NULL); 

      /// [DRAW] Draw simple tooltip
      drawSimpleCustomTooltip(hDC, pDrawRect, 500, identifyGameVersionIconID(pSyntax->eGameVersion), pSyntax->pTooltipSyntax, utilEither(pSyntax->pTooltipDescription, pRichDescription), bCalculation);

      // Cleanup
      if (pRichDescription)
         deleteRichText(pRichDescription);
   }
   CATCH0("");
}


/// Function name  : drawGameObjectCustomTooltip
// Description     : Draws/Calculates a GameObject tooltip
// 
// HDC                 hDC          : [in]     Target DC
// RECT*               pDrawRect    : [in/out] Draw/Calc rectangle
// CONST OBJECT_NAME*  pGameObject  : [in]     GameObject to draw
// CONST BOOL          bCalculation : [in]     TRUE to only perform calculation
// 
VOID  drawGameObjectCustomTooltip(HDC  hDC, RECT*  pDrawRect, CONST OBJECT_NAME*  pGameObject, CONST BOOL  bCalculation)
{
   CONST TCHAR  *szIcon;            // Resource ID of icon
   RICH_TEXT    *pRichDescription,  // RichText description
                *pRichBasic,
                *pRichAdvanced,
                *pRichTitle;        // RichText title
   TCHAR        *szSource;          // SourceText for generating title
   TCHAR         szMin[32], szMax[32];

   TRY
   {
      const OBJECT_PROPERTIES&  oProperties = pGameObject->oProperties;

      // Prepare
      pRichAdvanced = NULL;
      szSource = utilCreateEmptyString(1024);
      szIcon   = TEXT("GAME_OBJECT_ICON");

      /// [TITLE] Generate title containing NAME, ID, MAINTYPE and SUBTYPE
      StringCchPrintf(szSource, 1024,   TEXT("[center]") TEXT("[b]%s[/b]\n"), utilEither(oProperties.szRawName, pGameObject->szText));
      utilStringCchCatf(szSource, 1024, TEXT("%s\n\n"),              oProperties.szMainType);
      utilStringCchCatf(szSource, 1024, TEXT("[b]ID:[/b] %s\t\t\t"), pGameObject->szObjectID);
      utilStringCchCatf(szSource, 1024, TEXT("[b]SubType[/b]: %u") TEXT("[/center]"), pGameObject->iID);

      generateRichTextFromSourceText(szSource, lstrlen(szSource), pRichTitle, RTT_RICH_TEXT, ST_DISPLAY, NULL);
      
      /// [COMMON PROPERTIES]
      StringCchPrintf(szSource, 1024,   TEXT("[b][u]Properties:[/u][/b]\n\n"));
      utilStringCchCatf(szSource, 1024, oProperties.iMinPrice OR oProperties.iMaxPrice ? TEXT("[b]Trading Price[/b]: %s - %s\n") : TEXT("[b]Trading Price[/b]: Unknown\n"), formatPrice(oProperties.iMinPrice, szMin, 32), formatPrice(oProperties.iMaxPrice, szMax, 32));
      utilStringCchCatf(szSource, 1024, TEXT("[b]Unit Volume[/b]: %s units\n"), formatNumber(oProperties.iVolume, szMin, 32));
      utilStringCchCatf(szSource, 1024, TEXT("[b]Containers[/b]: %s"),          oProperties.szTransportClass);
      if (oProperties.szClass) 
         utilStringCchCatf(szSource, 1024, TEXT("\n[b]Class[/b]: %s"), oProperties.szClass);

      generateRichTextFromSourceText(szSource, lstrlen(szSource), pRichBasic, RTT_RICH_TEXT, ST_DISPLAY, NULL);

      // Examine GameObject type
      switch (pGameObject->eMainType)
      {
      // [SHIPS] Display SPEED, CARGO-VOLUME, MANUFACTURER
      case MT_SHIP:
         StringCchPrintf(szSource, 1024,   TEXT("[right]\n\n"));
         utilStringCchCatf(szSource, 1024, TEXT("%s - %s m/s : [b]Ship Speed[/b]\n"), formatNumber(oProperties.iMinSpeed, szMin, 32), formatNumber(oProperties.iMaxSpeed, szMax, 32));
         utilStringCchCatf(szSource, 1024, TEXT("%s - %s units : [b]Cargobay Volume[/b]\n"), formatNumber(oProperties.iMinCargobay, szMin, 32), formatNumber(oProperties.iMaxCargobay, szMax, 32));
         if (oProperties.iMaxDocked) 
            utilStringCchCatf(szSource, 1024, TEXT("%s ships : [b]Docking Ports[/b]\n"), formatNumber(oProperties.iMaxDocked, szMin, 32));
         utilStringCchCatf(szSource, 1024, TEXT("%s : [b]Manufacturer[/b]") TEXT("[/right]"), oProperties.szManufacturer);

         generateRichTextFromSourceText(szSource, lstrlen(szSource), pRichAdvanced, RTT_RICH_TEXT, ST_DISPLAY, NULL);
         break;

      // [DOCK/FACTORY]
      case MT_DOCK:
      case MT_FACTORY:
         break;

      // [SHIELD/LASER] Display Energy
      case MT_WARE_LASER:
      case MT_WARE_SHIELD:
         StringCchPrintf(szSource, 1024, TEXT("[right]\n\n") TEXT("%s joules : [b]Total Energy[/b]") TEXT("[/right]"), formatNumber(oProperties.iMaxEnergy, szMin, 32));
         generateRichTextFromSourceText(szSource, lstrlen(szSource), pRichAdvanced, RTT_RICH_TEXT, ST_DISPLAY, NULL);
         break;
      }

      /// [DESCRIPTION] Generate description
      StringCchPrintf(szSource, 1024, TEXT("[b][u]Description:[/u][/b]\n\n") TEXT("%s"), utilEither(oProperties.szDescription, szNoGameObjectDescription));
      generateRichTextFromSourceText(szSource, lstrlen(szSource), pRichDescription, RTT_RICH_TEXT, ST_DISPLAY, NULL);     //TEXT("[center]%s[/center]")

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
   CATCH0("");
}



/// Function name  : drawScriptObjectCustomTooltip
// Description     : Draws/Calculates a ScriptObject tooltip
// 
// HDC                 hDC           : [in]     Target DC
// RECT*               pDrawRect     : [in/out] Draw/Calc rectangle
// CONST OBJECT_NAME*  pScriptObject : [in]     ScriptObject to draw
// CONST BOOL          bCalculation  : [in]     TRUE to only perform calculation
// 
VOID  drawScriptObjectCustomTooltip(HDC  hDC, RECT*  pDrawRect, CONST OBJECT_NAME*  pScriptObject, CONST BOOL  bCalculation)
{
   CONST TCHAR  *szIcon;            // Resource ID of icon
   RICH_TEXT    *pRichDescription,  // RichText placeholder description, used if object has no description
                *pRichTitle;        // RichText title
   TCHAR        *szSource;          // SourceText for generating title

   TRY
   {
      // Prepare
      pRichDescription = NULL;

      /// [ICON] Use GameVersion or ModContent icon
      szIcon = (pScriptObject->bModContent ? TEXT("SCRIPT_OBJECT_ICON") : identifyGameVersionIconID(pScriptObject->eVersion));

      /// [TITLE] Create Bold version of the object name
      szSource = utilCreateStringf(256, TEXT("[b]%s[/b]"), pScriptObject->szText);
      generateRichTextFromSourceText(szSource, lstrlen(szSource), pRichTitle, RTT_RICH_TEXT, ST_DISPLAY, NULL);

      // [CHECK] Does object have a description?
      if (!pScriptObject->pDescription)
         /// [NO DESCRIPTION] Use fixed string
         generateRichTextFromSourceText(szNoScriptObjectDescription, lstrlen(szNoScriptObjectDescription), pRichDescription, RTT_RICH_TEXT, ST_DISPLAY, NULL);

      /// [DRAW] Draw simple tooltip
      drawSimpleCustomTooltip(hDC, pDrawRect, 300, szIcon, pRichTitle, utilEither(pScriptObject->pDescription, pRichDescription), bCalculation);

      // [OPTIONAL] Delete placeholder description
      if (pRichDescription)
         deleteRichText(pRichDescription);

      // Cleanup
      deleteRichText(pRichTitle);
      utilDeleteString(szSource);
   }
   CATCH0("");
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

   TRY
   {
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
   CATCH0("");
   return CDRF_SKIPDEFAULT;
}

