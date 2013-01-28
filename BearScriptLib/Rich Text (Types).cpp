//
// Rich Text (Types).cpp : Creation/Destruction of the many objects used in RichText generation
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       CREATION / DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createRichParagraph
// Description     : Create a new RichText paragraph
// 
// PARAGRAPH_ALIGNMENT  eAlignment : [in] Paragraph alignment
// 
// Return Value   : New RichText paragraph, you are responsible for destroying it
// 
BearScriptAPI 
RICH_PARAGRAPH*  createRichParagraph(PARAGRAPH_ALIGNMENT  eAlignment)
{
   // Create object
   RICH_PARAGRAPH*  pNewParagraph = utilCreateEmptyObject(RICH_PARAGRAPH);

   // Set properties
   pNewParagraph->eAlignment = eAlignment;
   pNewParagraph->pItemList  = createList(destructorRichItem);
   
   // Return new paragraph
   return pNewParagraph;
}


/// Function name  : createRichItemButton
// Description     : Create a new RichText button item
// 
// CONST TCHAR*  szText : [in] Button text
// CONST TCHAR*  szID   : [in] Button ID
// 
// Return Value   : New button item, you are responsible for destroying it
// 
BearScriptAPI
RICH_ITEM*  createRichItemButton(CONST TCHAR*  szText, CONST TCHAR*  szID)
{
   RICH_ITEM*  pNewItem;

   // Create new button item
   pNewItem         = utilCreateEmptyObject(RICH_ITEM);
   pNewItem->eType  = RIT_BUTTON;

   // Set attributes
   pNewItem->szText = utilDuplicateSimpleString(szText);
   pNewItem->szID   = utilDuplicateSimpleString(szID);

   // Return
   return pNewItem;
}


/// Function name  : createRichItemPlainText
// Description     : Create a new 'text' RichText item containing unformatted text. This is used when there is a parsing error.
// 
// CONST TCHAR*     szText      : [in] Plaintext
// CONST UINT       iTextLength : [in] Length of plaintext, in characters
// 
// Return Value   : New RichText item, you are responsisble for destroying it
// 
RICH_ITEM*  createRichItemPlainText(CONST TCHAR*  szPlainText, CONST UINT  iTextLength)
{
   // Create object
   RICH_ITEM*  pNewItem = utilCreateEmptyObject(RICH_ITEM);

   // Set properties
   pNewItem->eType   = RIT_TEXT;
   pNewItem->szText  = utilDuplicateString(szPlainText, iTextLength);
   pNewItem->eColour = GTC_DEFAULT;

   // return new item
   return pNewItem;
}


/// Function name  : createRichItemText
// Description     : Create an empty RichText item of type 'text', optionally copying the attributes of an existing item.
// 
// CONST RICH_ITEM*  pExistingItem   : [in] [optional] Existing item from which to copy the attributes
// 
// Return Value   : New RichTextItem, you are reponsible for destroying it
// 
RICH_ITEM*  createRichItemText(CONST RICH_ITEM*  pExistingItem)
{
   RICH_ITEM*  pNewItem;

   // Create an empty 'text' object
   pNewItem = utilCreateEmptyObject(RICH_ITEM);
   pNewItem->eType = RIT_TEXT;

   // [FROM EXISTING] Copy attributes
   if (pExistingItem)
   {
      pNewItem->bBold      = pExistingItem->bBold;
      pNewItem->bItalic    = pExistingItem->bItalic;
      pNewItem->bUnderline = pExistingItem->bUnderline;
      pNewItem->eColour    = pExistingItem->eColour;
   }
   // [EMPTY] Default to no bold/italic/underline and the default colour
   else
      pNewItem->eColour = GTC_DEFAULT;

   return pNewItem;
}

/// Function name  : createRichItemTextFromPhrase
// Description     : Create a new 'text' RichText item using specified attributes and text from a RichEdit control
// 
// HWND                   hRichEdit     : [in] RichEdit control window handle
// CONST UINT             iCharIndex    : [in] Character index of text to be extracted from RichEdit control
// CONST UINT             iPhraseLength : [in] Length of text to extract, in characters
// RICHTEXT_ATTRIBUTES*   pState        : [in] Attributes for the new RichText item
// 
// Return Value   : New RichText item, you are responsisble for destroying it
// 
BearScriptAPI
RICH_ITEM*  createRichItemTextFromPhrase(HWND  hRichEdit, const RICHTEXT_PHRASE*  pPhrase)
{
   RICH_ITEM*  pNewItem = utilCreateEmptyObject(RICH_ITEM);
   UINT        iLength  = pPhrase->iEnd - pPhrase->iStart;

   // Set attributes
   pNewItem->eType      = RIT_TEXT;
   pNewItem->eTextType  = ST_DISPLAY;
   pNewItem->szText     = utilCreateEmptyString(iLength + 1);
   pNewItem->bBold      = pPhrase->oState.bBold;
   pNewItem->bItalic    = pPhrase->oState.bItalic;
   pNewItem->bUnderline = pPhrase->oState.bUnderline;
   pNewItem->eColour    = pPhrase->oState.eColour;

   // Extract desired text from RichEdit control
   Edit_SetSel(hRichEdit, pPhrase->iStart, pPhrase->iEnd);
   Edit_GetSelText(hRichEdit, pNewItem->szText);   

   // Ensure null termination and return
   pNewItem->szText[iLength] = NULL;
   return pNewItem;
}


/// Function name  : createRichText
// Description     : Create a new RichText message with a single left-aligned paragraph
// 
// Return Value   : New RichText object, you are responsible for destroying it
// 
BearScriptAPI
RICH_TEXT*   createRichText(CONST RICHTEXT_TYPE  eType)
{
   RICH_TEXT*  pRichText = (eType == RTT_RICH_TEXT ? utilCreateEmptyObject(RICH_TEXT) : utilCreateEmptyObject(LANGUAGE_MESSAGE));     // New LanguageMessage object

   // Set properties
   pRichText->eType          = eType;
   pRichText->pParagraphList = createList(destructorRichParagraph);

   // Create empty left-aligned paragraph
   appendObjectToList(pRichText->pParagraphList, (LPARAM)createRichParagraph(PA_LEFT));

   // Return new object
   return pRichText;
}


/// Function name  : createRichTextStackItem
// Description     : Creates a RichObject stack item
// 
// RICHTEXT_TOKEN_CLASS   eClass : [in] RichObject Class 
// RICHTEXT_TAG           eTag   : [in] RichText Tag 
// 
// Return Value   : New stack item
// 
RICHTEXT_STACK_ITEM*  createRichTextStackItem(RICHTEXT_TOKEN_CLASS  eClass, RICHTEXT_TAG  eTag)
{
   RICHTEXT_STACK_ITEM*  pStackItem = utilCreateEmptyObject(RICHTEXT_STACK_ITEM);

   // Set properties
   pStackItem->eClass = eClass;
   pStackItem->eTag   = eTag;

   // Return item
   return pStackItem;
}


/// Function name  : createRichTextTokeniser
// Description     : Creates a new RichText parsing object from sourcetext containing RichText tags
// 
// CONST TCHAR*       szSourceText  : [in] Source text containing RichText formatting tags
// CONST UINT         iTextLength   : [in] Length of plain text above, in characters
// CONST STRING_TYPE  eStringType   : [in] String type of plaintext
// 
// Return Value   : New empty RichText parsing object, you are responsible for deleting it
// 
RICHTEXT_TOKENISER*  createRichTextTokeniser(CONST TCHAR*  szSourceText, CONST UINT  iTextLength, CONST STRING_TYPE  eStringType)
{
   // Create new object
   RICHTEXT_TOKENISER*  pRichObject = utilCreateEmptyObject(RICHTEXT_TOKENISER);
   
   // Convert input string
   switch (eStringType)
   {
   case ST_EXTERNAL:    generateConvertedString(szSourceText, SPC_LANGUAGE_EXTERNAL_TO_INTERNAL, pRichObject->szSourceText);      break;
   case ST_DISPLAY:     ASSERT(FALSE);    // Assert for now. Define new flags for display -> internal
   }

   // [CHECK] Duplicate input string if no conversion was required
   if (!pRichObject->szSourceText)
      pRichObject->szSourceText = utilDuplicateString(szSourceText, iTextLength);

   // Set properties
   pRichObject->szCurrentPosition = pRichObject->szSourceText;

   // Return object
   return pRichObject;
}


/// Function name  : duplicateRichItem
// Description     : Duplicates a RichItem
// 
// CONST RICH_ITEM*  pOriginal : [in] Item to copy
// 
// Return Value   : New RichItem, you are responsible for destroying it
// 
RICH_ITEM*  duplicateRichItem(CONST RICH_ITEM*  pOriginal)
{
   RICH_ITEM*  pDuplicate = utilCreateEmptyObject(RICH_ITEM);

   // Copy properties
   utilCopyObject(pDuplicate, pOriginal, RICH_ITEM);

   // Copy strings
   pDuplicate->szText = (pOriginal->szText ? utilDuplicateSimpleString(pOriginal->szText) : NULL);
   pDuplicate->szID   = (pOriginal->szID   ? utilDuplicateSimpleString(pOriginal->szID) : NULL);

   // Return
   return pDuplicate;
}


/// Function name  : duplicateRichParagraph
// Description     : Duplicates a RichText paragraph
// 
// CONST RICH_PARAGRAPH*  pOriginal : [in] Paragraph to duplicate
// 
// Return Value   : New RichText paragraph, you are responsible for destroying it
//  
RICH_PARAGRAPH*  duplicateRichParagraph(CONST RICH_PARAGRAPH*  pOriginal)
{
   RICH_PARAGRAPH*  pDuplicate;
   RICH_ITEM*       pItem;

   // Duplicate object
   pDuplicate = createRichParagraph(pOriginal->eAlignment);

   // Iterate through items
   for (LIST_ITEM*  pIterator = getListHead(pOriginal->pItemList); pItem = extractListItemPointer(pIterator, RICH_ITEM); pIterator = pIterator->pNext)
      // Duplicate item
      appendObjectToList(pDuplicate->pItemList, (LPARAM)duplicateRichItem(pItem));
   
   // Return new paragraph
   return pDuplicate;
}


/// Function name  : duplicateRichText
// Description     : Duplicates a RichText object
// 
// CONST RICH_TEXT*  pOriginal : [in] RichText to duplicate
// 
// Return Value   : New RichText object, you are responsible for destroying it
// 
BearScriptAPI 
RICH_TEXT*  duplicateRichText(CONST RICH_TEXT*  pOriginal)
{
   RICH_PARAGRAPH*  pParagraph;
   RICH_TEXT*        pDuplicate;

   // [CHECK] Ensure object is not a LanguageMessage
   ASSERT(pOriginal->eType != RTT_LANGUAGE_MESSAGE);     // Not yet implemented

   // Create object
   pDuplicate = utilCreateEmptyObject(RICH_TEXT);

   // Copy basic properties
   utilCopyObject(pDuplicate, pOriginal, RICH_TEXT);

   // Create empty paragraph list
   pDuplicate->pParagraphList = createList(destructorRichParagraph);

   // Iterate through paragraphs
   for (LIST_ITEM*  pIterator = getListHead(pOriginal->pParagraphList); pParagraph = extractListItemPointer(pIterator, RICH_PARAGRAPH); pIterator = pIterator->pNext)
      // Duplicate paragraph
      appendObjectToList(pDuplicate->pParagraphList, (LPARAM)duplicateRichParagraph(pParagraph));
   
   // Return new paragraph
   return pDuplicate;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                         DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : deleteRichItem
// Description     : Delete a list of RichText items
// 
// RICH_ITEM*  &pItem : [in] RichText item to delete
// 
BearScriptAPI
VOID  deleteRichItem(RICH_ITEM*  &pItem)
{
   // Delete strings, if they exist
   utilSafeDeleteStrings(pItem->szText, pItem->szID);

   // Delete calling object
   utilDeleteObject(pItem);
}


/// Function name  : deleteRichParagraph
// Description     : Delete a list of RichText paragraphs
// 
// RICH_PARAGRAPH*  &pParagraph : [in] RichText paragraph to delete
// 
BearScriptAPI
VOID  deleteRichParagraph(RICH_PARAGRAPH*  &pParagraph)
{
   // Delete items
   deleteList(pParagraph->pItemList);

   // Delete calling object
   utilDeleteObject(pParagraph);
}


/// Function name  : deleteRichText
// Description     : Delete a RichText/LanguageMessage object
// 
// RICH_TEXT*  &pRichText : [in] RichText object to delete
// 
BearScriptAPI
VOID  deleteRichText(RICH_TEXT*  &pRichText)
{
   LANGUAGE_MESSAGE*  pLanguageMessage;

   if (pRichText->eType == RTT_LANGUAGE_MESSAGE)
   {
      // Prepare
      pLanguageMessage = (LANGUAGE_MESSAGE*)pRichText;

      // [OPTIONAL] Delete strings
      utilSafeDeleteStrings(pLanguageMessage->szAuthor, pLanguageMessage->szTitle);
      utilSafeDeleteString(pLanguageMessage->szRankTitle);
   }

   // Delete paragraphs
   deleteList(pRichText->pParagraphList);

   // Delete calling object
   utilDeleteObject(pRichText);
}


/// Function name  : deleteRichTextTokeniser
// Description     : Delete a RichText object and it's current buffer (if any)
// 
// RICHTEXT_TOKENISER*  &pObject : [in] Language object to delete
// 
VOID  deleteRichTextTokeniser(RICHTEXT_TOKENISER*  &pObject)
{
   // Delete current object's text (if any)
   utilSafeDeleteString(pObject->szText);

   // Delete source string
   utilDeleteString(pObject->szSourceText);

   // Delete calling object
   utilDeleteObject(pObject);
}


/// Function name  : destructorRichItem
// Description     : Destroys a RichItem in a list
// 
// LPARAM  pItem : [in] RichItem
// 
VOID  destructorRichItem(LPARAM  pItem)
{
   deleteRichItem((RICH_ITEM*&)pItem);
}


/// Function name  : destructorRichParagraph
// Description     : Detroys a RichParagraph in a list item
// 
// LPARAM  pParagraph : [in] RichParagraph
// 
VOID  destructorRichParagraph(LPARAM  pParagraph)
{
   deleteRichParagraph((RICH_PARAGRAPH*&)pParagraph);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                            FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////


