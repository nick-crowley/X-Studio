//
// Properties Dialog General (Language).cpp : The general properties page for a language document
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


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : calculateLanguageMessageCompatibility
// Description     : Determine which display methods a LanguageMessage is compatible with from the custom formatting it uses.
// 
// CONST LANGUAGE_MESSAGE*  pMessage   : [in] LanguageMessage to examine
// 
// Return Value   : LMC_LOGBOOK     : Logbook
//                  LMC_MESSAGE     : Logbook + 'Incoming Message'
//                  LMC_CUSTOM_MENU : Logbook + 'Incoming Message' + CustomMenu
// 
MESSAGE_COMPATIBILITY  calculateLanguageMessageCompatibility(CONST LANGUAGE_MESSAGE*  pMessage)
{
   MESSAGE_COMPATIBILITY  eResult;      // Resultant compatbility
   RICH_PARAGRAPH*        pParagraph;   // LanguageMessage paragraphs iterator
   RICH_ITEM*        pItem;        // LanguageMessage items iterator

   // [RANK], [ARTICLE] and [TEXT] tags are incompatible with the CustomMenu
   // [AUTHOR], [TITLE] and [SELECT] are too.
   /// TODO: Determine exactly what is compatible logbooks and 'incoming messages'.  Possibly alter the possible combinations

   eResult = LMC_CUSTOM_MENU;

   // [CHECK] Output 'MESSAGE' if [ARTICLE], [AUTHOR], [TITLE], [RANK] or [TEXT] tags are present
   if (pMessage->bArticle OR pMessage->bCustomRank OR pMessage->szAuthor OR pMessage->szTitle OR
      pMessage->iColumnCount > 1 OR pMessage->bCustomSpacing OR pMessage->bCustomWidth)
      eResult = LMC_MESSAGE;

   // [CHECK] Output 'MESSAGE' if [SELECT] tags are present
   for (LIST_ITEM*  pParagraphIterator = getListHead(pMessage->pParagraphList); pParagraph = extractListItemPointer(pParagraphIterator, RICH_PARAGRAPH); pParagraphIterator = pParagraphIterator->pNext)
   {
      for (LIST_ITEM*  pItemIterator = getListHead(pParagraph->pItemList); pItem = extractListItemPointer(pItemIterator, RICH_ITEM); pItemIterator = pItemIterator->pNext)
         if (pItem->eType == RIT_BUTTON)
            eResult = LMC_MESSAGE;
   }
      
   // Return result
   return eResult;
}


/// Function name  : performLanguageMessageCompatibilityChange
// Description     : Remove customisations that are incompatible with the specified compatibility mode
// 
// LANGUAGE_MESSAGE*            pMessage       : [in/out] LanguageMessage to alter
// CONST MESSAGE_COMPATIBILITY  eCompatibility : [in]     Desired compatibility
// 
VOID  performLanguageMessageCompatibilityChange(LANGUAGE_MESSAGE*  pMessage, CONST MESSAGE_COMPATIBILITY  eCompatibility)
{
   switch (eCompatibility)
   {
   // [LOGBOOK / MESSAGE] - At present everything is compatible with both of these
   case LMC_LOGBOOK: /// TODO: Determine what's compatible with logbook messages and what isn't.
   case LMC_MESSAGE: /// TODO: Determine what's compatible with 'incoming messages' and what isn't.
      break;

   /// [CUSTOM MENU] - Strip.. everything really.
   case LMC_CUSTOM_MENU:
      // [TITLE], [AUTHOR], [RANK] and [ARTICLE]
      utilSafeDeleteStrings(pMessage->szAuthor, pMessage->szTitle);
      utilSafeDeleteString(pMessage->szRankTitle);
      pMessage->bArticle = FALSE;
      pMessage->bCustomRank = FALSE;
      // [TEXT]
      pMessage->iColumnCount = 1;
      pMessage->bCustomWidth = FALSE;
      pMessage->bCustomSpacing = FALSE;
      // [SELECT] -- Remove button items..
      /// TODO: Remove buttons
      break;
   }

   // Update message compatiblity
   pMessage->eCompatibility = eCompatibility;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : onGeneralPageCommand
// Description     : WM_COMMAND processing for the 'General' properties page
// 
// LANGUAGE_DOCUMENT* pDocument      : [in] Language Document data
// HWND               hDialog        : [in] 'General' properties dialog page window handle
// CONST UINT         iControl       : [in] ID of the control sending the command
// CONST UINT         iNotification  : [in] Notification being sent
// HWND               hCtrl          : [in] Window handle fo the control sending the command
// 
// Return Value   : TRUE if processed, FALSE otherwise
// 
BOOL    onGeneralPageCommand(LANGUAGE_DOCUMENT*  pDocument, HWND  hDialog, CONST UINT  iControl, CONST UINT  iNotification, HWND  hCtrl)
{
   MESSAGE_COMPATIBILITY  eCompatibility;
   LANGUAGE_MESSAGE*     &pCurrentMessage = pDocument->pCurrentMessage;

   switch (iControl)
   {
   /// [AUTHOR] -- Update current message
   case IDC_AUTHOR_EDIT:
      if (iNotification == EN_UPDATE)
      {
         utilSafeDeleteString(pCurrentMessage->szAuthor);
         pCurrentMessage->szAuthor = utilGetWindowText(hCtrl);
         return TRUE;
      }
      break;

   /// [TITLE] -- Update current message
   case IDC_TITLE_EDIT:
      if (iNotification == EN_UPDATE)
      {
         utilSafeDeleteString(pCurrentMessage->szTitle);
         pCurrentMessage->szTitle = utilGetWindowText(hCtrl);
         return TRUE;
      }
      break;

   /// [COMPATIBILITY] - Strip the message of customisations incompatible with the current selection
   case IDC_COMPATIBILITY_COMBO:
      if (iNotification == CBN_SELCHANGE)
      {
         eCompatibility = (MESSAGE_COMPATIBILITY)SendDlgItemMessage(hDialog, IDC_COMPATIBILITY_COMBO, CB_GETCURSEL, NULL, NULL);
         performLanguageMessageCompatibilityChange(pCurrentMessage, eCompatibility);
         return TRUE;
      }
      break;
   }

   return FALSE;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : dlgprocGeneralPageLanguage
// Description     : Window procedure for the 'General' script property page
//
// 
INT_PTR   dlgprocGeneralPageLanguage(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   PROPERTIES_DATA*  pDialogData;

   // Get dialog data
   pDialogData = getPropertiesDialogData(hDialog);

   // Intercept messages
   switch (iMessage)
   {
   case WM_COMMAND:
      if (onGeneralPageCommand(pDialogData->pLanguageDocument, hDialog, LOWORD(wParam), HIWORD(wParam), (HWND)lParam))
         return TRUE;
      break;
   }

   // Pass to common handlers
   return dlgprocPropertiesPage(hDialog, iMessage, wParam, lParam, PP_LANGUAGE_GENERAL);
}

