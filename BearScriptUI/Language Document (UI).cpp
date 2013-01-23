//
// Insert Argument Dialog.cpp : The 'new argument' dialog for the 'Arguments' document properties
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


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////


VOID  onLanguageDocument_DeletePage(LANGUAGE_DOCUMENT*  pDocument, GAME_PAGE*  pPage)
{
   // Hide/Destroy Document PageStrings
   INT  iSelection = ListView_GetNextItem(pDocument->hPageList, -1, LVNI_ALL | LVNI_SELECTED);
   ListView_SetItemState(pDocument->hPageList, -1, NULL, LVIS_SELECTED);

   /// Delete all strings with input PageID
   deleteLanguageDocumentGamePage(pDocument, pPage);

   // Re-create Document PageStrings
   ListView_SetItemState(pDocument->hPageList, min(ListView_GetItemCount(pDocument->hPageList), iSelection), LVIS_SELECTED, LVIS_SELECTED);
}


VOID  onLanguageDocument_EditPage(LANGUAGE_DOCUMENT*  pDocument, GAME_PAGE*  pOldPage, GAME_PAGE*  pNewPage)
{
   // Prepare
   INT  iSelected = ListView_GetSelected(pDocument->hPageList);

   /// [SAME ID] Update properties
   if (pOldPage->iPageID == pNewPage->iPageID)
   {
      // Update Values
      utilReplaceString(pOldPage->szTitle,       pNewPage->szTitle);
      utilReplaceString(pOldPage->szDescription, pNewPage->szDescription);
      pOldPage->bVoice = pNewPage->bVoice;

      // Cleanup
      deleteGamePage(pNewPage);
   }
   /// [NEW ID] Change ID + Properties
   else
   {
      // Hide/Destroy Document PageStrings
      ListView_SetItemState(pDocument->hPageList, -1, NULL, LVIS_SELECTED);

      /// Change PageID of all strings within page
      moveLanguageDocumentPageStrings(pDocument, pOldPage, pNewPage);
      
      // Create/Show document PageStrings
      ListView_SetItemState(pDocument->hPageList, iSelected, LVIS_SELECTED, LVIS_SELECTED);
   }

   // Redraw current page
   ListView_RedrawItems(pDocument->hPageList, iSelected, iSelected);
}


VOID  onLanguageDocument_InsertPage(LANGUAGE_DOCUMENT*  pDocument, GAME_PAGE*  pNewPage)
{
   // Hide/Destroy document PageStrings
   INT  iSelection = ListView_GetSelected(pDocument->hPageList);
   ListView_SetItemState(pDocument->hPageList, -1, NULL, LVIS_SELECTED);

   /// Insert new GamePage + Refresh ListView
   insertLanguageDocumentGamePage(pDocument, pNewPage);

   // Create/Show document PageStrings
   ListView_SetItemState(pDocument->hPageList, iSelection, LVIS_SELECTED, LVIS_SELECTED);
}