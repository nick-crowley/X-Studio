//
// Rich Text.cpp : Functions for parsing, storing and displaying Rich (formatted) text
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          DECLARATIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

// Helpers
CONST TCHAR*         getCompatibleColourTag(const LANGUAGE_MESSAGE*  pMessage, const GAME_TEXT_COLOUR  eColour, const bool  bOpening);
COMPATIBILITY        calculateMessageCompatibility(const LANGUAGE_MESSAGE*  pMessage, const TCHAR*  szSourceText);
CONST TCHAR*         convertTagToString(RICHTEXT_TAG  eTag);
UINT                 getRichItemCount(const RICH_PARAGRAPH*  pParagraph);
UINT                 getParagraphCount(const RICH_TEXT*  pRichText);
RICHTEXT_TAG         identifyColourTagFromChar(const TCHAR  chChar);
GAME_TEXT_COLOUR     identifyColourFromTag(CONST RICHTEXT_TAG  eTag);
RICHTEXT_TAG         identifyRichTextTag(CONST TCHAR*  szTag);

// Functions
BOOL                 findNextRichObject(RICHTEXT_TOKENISER*  pToken, ERROR_STACK*  &pError);
BOOL                 translateLanguageMessageTag(CONST RICHTEXT_TOKENISER*  pTokeniser, LANGUAGE_MESSAGE*  pMessage, RICH_ITEM*  pButton, ERROR_STACK*  &pError);

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       CONSTANTS / GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

CONST TCHAR*  szAlignmentTags[5] = { TEXT("N/A"),    TEXT("left"),    TEXT("right"), TEXT("center"), TEXT("justify") };
CONST TCHAR*  szColourTags[10]   = { TEXT("black"),  TEXT("blue"),    TEXT("cyan"),  TEXT("N/A"),    TEXT("green"), 
                                     TEXT("orange"), TEXT("magenta"), TEXT("red"),   TEXT("white"),  TEXT("yellow") };

/// Macro: topTagStackItem
#define  topTagStackItem(pStack)    ((RICHTEXT_STACK_ITEM*)topStackObject(pStack))

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                            HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : getCompatibleColourTag
// Description     : Returns a \033X or [blue] style colour tag 
// 
// const LANGUAGE_MESSAGE   pMessage  : [in] Message type
// const GAME_TEXT_COLOUR   eColour   : [in] Colour
// const bool               bOpening  : [in] Opening/Closing tag
// 
CONST TCHAR*  getCompatibleColourTag(const LANGUAGE_MESSAGE*  pMessage, const GAME_TEXT_COLOUR  eColour, const bool  bOpening)
{
   const TCHAR*  szOutput;
   
   /// [CUSTOM MENU] Use \033X format tags
   if (pMessage->eCompatibility == LMC_CUSTOM_MENU) 
   {
      switch (eColour)
      {
      case GTC_BLACK:   szOutput = TEXT("\\033Z");    break;
      case GTC_BLUE:    szOutput = TEXT("\\033B");    break;
      case GTC_CYAN:    szOutput = TEXT("\\033C");    break;
      case GTC_GREEN:   szOutput = TEXT("\\033G");    break;
      case GTC_ORANGE:  szOutput = TEXT("\\033O");    break;
      case GTC_PURPLE:  szOutput = TEXT("\\033M");    break;
      case GTC_RED:     szOutput = TEXT("\\033R");    break;
      case GTC_SILVER:  szOutput = TEXT("\\033A");    break;
      case GTC_WHITE:   szOutput = TEXT("\\033W");    break;
      case GTC_YELLOW:  szOutput = TEXT("\\033Y");    break;
      default:          szOutput = TEXT("\\033X");    break;
      }
   }
   /// [LOGBOOK/MESSAGE] Use BB Code format tags
   else 
   {
      switch (eColour)
      {
      case GTC_BLACK:   szOutput = bOpening ? TEXT("[black]")   : TEXT("[/black]");    break;
      case GTC_BLUE:    szOutput = bOpening ? TEXT("[blue]")    : TEXT("[/blue]");     break;
      case GTC_CYAN:    szOutput = bOpening ? TEXT("[cyan]")    : TEXT("[/cyan]");     break;
      case GTC_GREEN:   szOutput = bOpening ? TEXT("[green]")   : TEXT("[/green]");    break;
      case GTC_ORANGE:  szOutput = bOpening ? TEXT("[orange]")  : TEXT("[/orange]");   break;
      case GTC_PURPLE:  szOutput = bOpening ? TEXT("[magenta]") : TEXT("[/magenta]");  break;
      case GTC_RED:     szOutput = bOpening ? TEXT("[red]")     : TEXT("[/red]");      break;
      case GTC_SILVER:  szOutput = bOpening ? TEXT("[silver]")  : TEXT("[/silver]");   break;
      case GTC_WHITE:   szOutput = bOpening ? TEXT("[white]")   : TEXT("[/white]");    break;
      case GTC_YELLOW:  szOutput = bOpening ? TEXT("[yellow]")  : TEXT("[/yellow]");   break;
      default:          szOutput = TEXT("");
      }
   }

   // Return tag
   return szOutput;
}


/// Function name  : appendRichTextItem
// Description     : Add an item to a RichText paragraph's list of items
// 
// RICH_PARAGRAPH*  pParagraph  : [in/out] Paragraph to append
// RICH_ITEM*       pItem       : [in]     Item to be appended to the paragraph, above
// 
BearScriptAPI
VOID  appendRichTextItem(RICH_PARAGRAPH*  pParagraph, RICH_ITEM*  pItem)
{
   // Add item
   appendObjectToList(pParagraph->pItemList, (LPARAM)pItem);
}


/// Function name  : appendRichTextParagraph
// Description     : Appends a paragraph to RichText
// 
// RICH_TEXT*       pRichText  : [in/out] RichText
// RICH_PARAGRAPH*  pParagraph : [in]     Paragraph
// 
BearScriptAPI
VOID  appendRichTextParagraph(RICH_TEXT*  pRichText, RICH_PARAGRAPH*  pParagraph)
{
   appendObjectToList(pRichText->pParagraphList, (LPARAM)pParagraph);
}


/// Function name  : calculateMessageCompatibility
// Description     : Determine which display methods a LanguageMessage is compatible with from the custom formatting it uses.
// 
// const TCHAR*  szSourceText : [in] Source Text
// 
// Return Value   : LMC_LOGBOOK or LMC_CUSTOM_MENU
// 
COMPATIBILITY  calculateMessageCompatibility(const LANGUAGE_MESSAGE*  pMessage, const TCHAR*  szSourceText)
{
   RICH_PARAGRAPH*  pParagraph;   // LanguageMessage paragraphs iterator
   RICH_ITEM*       pItem;        // LanguageMessage items iterator
   
   /// LOGBOOK: Check for [ARTICLE] or [RANK] or [TEXT] or [AUTHOR] or [TITLE]
   if (pMessage->bArticle OR pMessage->bCustomRank OR pMessage->iColumnCount > 1 OR pMessage->bCustomSpacing OR pMessage->bCustomWidth OR lstrlen(pMessage->szAuthor) OR lstrlen(pMessage->szTitle))
      return LMC_LOGBOOK;

   /// LOGBOOK: Check for [SELECT]
   for (LIST_ITEM*  pParagraphIterator = getListHead(pMessage->pParagraphList); pParagraph = extractListItemPointer(pParagraphIterator, RICH_PARAGRAPH); pParagraphIterator = pParagraphIterator->pNext)
   {
      for (LIST_ITEM*  pItemIterator = getListHead(pParagraph->pItemList); pItem = extractListItemPointer(pItemIterator, RICH_ITEM); pItemIterator = pItemIterator->pNext)
         if (pItem->eType == RIT_BUTTON)
            return LMC_LOGBOOK;
   }

   /// CUSTOM MENU: Check for \033 tags
   return utilFindSubString(szSourceText, "\\033") ? LMC_CUSTOM_MENU : LMC_LOGBOOK;
}


/// Function name  : convertTagToString
// Description     : Converts a tag ID to a string
// 
// RICHTEXT_TAG  eTag   : [in] Tag ID
// 
// Return Value   : String
// 
CONST TCHAR*  convertTagToString(RICHTEXT_TAG  eTag)
{
   CONST TCHAR*  szOutput;

   // Examine tag
   switch (eTag)
   {
   case RTT_BOLD:      szOutput = TEXT("b");      break;
   case RTT_UNDERLINE: szOutput = TEXT("u");      break;
   case RTT_ITALIC:    szOutput = TEXT("i");      break;

   case RTT_ARTICLE: szOutput = TEXT("article");  break;
   case RTT_AUTHOR:  szOutput = TEXT("author");   break;
   case RTT_TITLE:   szOutput = TEXT("title");    break;
   case RTT_TEXT:    szOutput = TEXT("text");     break;
   case RTT_RANKING: szOutput = TEXT("ranking");  break;

   case RTT_BLUE:    szOutput = TEXT("blue");     break;
   case RTT_CYAN:    szOutput = TEXT("cyan");     break;
   case RTT_GREEN:   szOutput = TEXT("green");    break;
   case RTT_PURPLE:  szOutput = TEXT("magenta");  break;
   case RTT_ORANGE:  szOutput = TEXT("orange");   break;
   case RTT_RED:     szOutput = TEXT("red");      break;
   case RTT_WHITE:   szOutput = TEXT("white");    break;
   case RTT_SILVER:  szOutput = TEXT("silver");   break;
   case RTT_YELLOW:  szOutput = TEXT("yellow");   break;
   case RTT_DEFAULT: szOutput = TEXT("\\033X");   break;

   case RTT_LEFT:    szOutput = TEXT("left");     break;
   case RTT_RIGHT:   szOutput = TEXT("right");    break;
   case RTT_CENTRE:  szOutput = TEXT("center");   break;
   case RTT_JUSTIFY: szOutput = TEXT("justify");  break;

   case RTT_SELECT:  szOutput = TEXT("select");   break;
   case RTT_IMAGE:   szOutput = TEXT("image");    break;
   case RTT_NONE:    szOutput = TEXT("none");     break;

   default:          szOutput = TEXT("unknown");  break;
   }

   // Return output
   return szOutput;
}



/// Function name  : getRichItemCount
// Description     : Retrieve item count
// 
// const RICH_PARAGRAPH*  pParagraph  : [in/out] Paragraph to append
// 
UINT  getRichItemCount(const RICH_PARAGRAPH*  pParagraph)
{
   return getListItemCount(pParagraph->pItemList);
}


/// Function name  : getParagraphCount
// Description     : Retrieve paragraph count
// 
// const RICH_PARAGRAPH*  pParagraph  : [in/out] Paragraph to append
// 
UINT  getParagraphCount(const RICH_TEXT*  pRichText)
{
   return getListItemCount(pRichText->pParagraphList);
}

/// Function name  : identifyColourFromTag
// Description     : Identifies the equivilent colour tag from the letter of \033X tags
// 
// CONST RICHTEXT_TAG  eTag  : [in] Colour character identifier
// 
// Return Value   : Colour tag
// 
RICHTEXT_TAG  identifyColourTagFromChar(const TCHAR  chChar)
{
   RICHTEXT_TAG  eOutput;

   switch (chChar)
   {
   case 'W':  eOutput = RTT_WHITE;    break;
   case 'A':  eOutput = RTT_SILVER;   break;
   case 'B':  eOutput = RTT_BLUE;     break;
   case 'C':  eOutput = RTT_CYAN;     break;
   case 'G':  eOutput = RTT_GREEN;    break;
   case 'M':  eOutput = RTT_PURPLE;   break;
   case 'O':  eOutput = RTT_ORANGE;   break;
   case 'R':  eOutput = RTT_RED;      break;
   case 'Y':  eOutput = RTT_YELLOW;   break;
   case 'Z':  eOutput = RTT_BLACK;    break;
   default:   eOutput = RTT_DEFAULT;  break;
   }

   return eOutput;
}


/// Function name  : identifyColourFromTag
// Description     : Convert a RichText message tag to a game text colour
// 
// CONST RICHTEXT_TAG  eTag  : [in] RichText message tag
// 
// Return Value   : Game text colour, or default if tag is unrecognised
// 
GAME_TEXT_COLOUR  identifyColourFromTag(CONST RICHTEXT_TAG  eTag)
{
   GAME_TEXT_COLOUR  eOutput;

   switch (eTag)
   {
   case RTT_BLACK:   eOutput = GTC_BLACK;    break;
   case RTT_BLUE:    eOutput = GTC_BLUE;     break;
   case RTT_CYAN:    eOutput = GTC_CYAN;     break;
   case RTT_DEFAULT: eOutput = GTC_DEFAULT;  break;
   case RTT_GREEN:   eOutput = GTC_GREEN;    break;
   case RTT_ORANGE:  eOutput = GTC_ORANGE;   break;
   case RTT_PURPLE:  eOutput = GTC_PURPLE;   break;
   case RTT_RED:     eOutput = GTC_RED;      break;
   case RTT_WHITE:   eOutput = GTC_WHITE;    break;
   case RTT_YELLOW:  eOutput = GTC_YELLOW;   break;
   case RTT_SILVER:  eOutput = GTC_SILVER;   break;
   default:          eOutput = GTC_DEFAULT;  break;
   }

   return eOutput;
}


/// Function name  : identifyRichTextTag
// Description     : Identify the specified RichText message tag
// 
// CONST TCHAR*  szTag : [in] String containing a RichText message tag
// 
// Return Value   : RichText message tag ID or RTT_NONE if the string matches no tag
// 
RICHTEXT_TAG  identifyRichTextTag(CONST TCHAR*  szTag)
{
   if (utilCompareString(szTag, "b"))                 return RTT_BOLD;
   else if (utilCompareString(szTag, "u"))            return RTT_UNDERLINE;
   else if (utilCompareString(szTag, "i"))            return RTT_ITALIC;

   else if (utilCompareString(szTag, "article"))      return RTT_ARTICLE;
   else if (utilCompareString(szTag, "author"))       return RTT_AUTHOR;
   else if (utilCompareString(szTag, "title"))        return RTT_TITLE;
   else if (utilCompareStringN(szTag, "text", 4))     return RTT_TEXT;
   else if (utilCompareStringN(szTag, "ranking", 7))  return RTT_RANKING;

   else if (utilCompareString(szTag, "blue"))         return RTT_BLUE;
   else if (utilCompareString(szTag, "cyan"))         return RTT_CYAN;
   else if (utilCompareString(szTag, "green"))        return RTT_GREEN;
   else if (utilCompareString(szTag, "magenta"))      return RTT_PURPLE;
   else if (utilCompareString(szTag, "orange"))       return RTT_ORANGE;
   else if (utilCompareString(szTag, "red"))          return RTT_RED;
   else if (utilCompareString(szTag, "silver"))       return RTT_SILVER;
   else if (utilCompareString(szTag, "white"))        return RTT_WHITE;
   else if (utilCompareString(szTag, "yellow"))       return RTT_YELLOW;

   else if (utilCompareString(szTag, "left"))         return RTT_LEFT;
   else if (utilCompareString(szTag, "right"))        return RTT_RIGHT;
   else if (utilCompareString(szTag, "center"))       return RTT_CENTRE;
   else if (utilCompareString(szTag, "justify"))      return RTT_JUSTIFY;

   else if (utilCompareStringN(szTag, "image", 5))    return RTT_IMAGE;

   else if (utilCompareStringN(szTag, "select", 6))   return RTT_SELECT;

   else return RTT_NONE;
}


/// Function name  : isRichEditButtonUnique
// Description     : Checks whether an ID is available
// 
// CONST RICH_TEXT*  pRichText : [in] RichText
// CONST TCHAR*      szID      : [in] ID to check
// 
// Return Value   : TRUE if ID is available, otherwise FALSE
// 
BearScriptAPI 
BOOL  isRichEditButtonUnique(CONST RICH_TEXT*  pRichText, CONST TCHAR*  szID)
{
   RICH_PARAGRAPH*  pParagraph;
   RICH_ITEM*       pItem;

   // Iterate through paragraphs
   for (LIST_ITEM*  pParagraphIterator = getListHead(pRichText->pParagraphList); pParagraph = extractListItemPointer(pParagraphIterator, RICH_PARAGRAPH); pParagraphIterator = pParagraphIterator->pNext)
      // Iterate through items
      for (LIST_ITEM*  pItemIterator = getListHead(pParagraph->pItemList); pItem = extractListItemPointer(pItemIterator, RICH_ITEM); pItemIterator = pItemIterator->pNext)
      {
         /// [BUTTON] Ensure Button ID is not present
         if (pItem->eType == RIT_BUTTON AND utilCompareStringVariables(pItem->szID, szID))
            return FALSE;
      }

   // [NOT FOUND] Return TRUE
   return TRUE;
}


/// Function name  : setParagraphAlignment
// Description     : Adjust the alignment of a paragraph in a RichText object
// 
// RICH_TEXT*                 pRichText  : [in] RichText object
// CONST UINT                 iIndex     : [in] Zero-based index of the paragraph to set
// CONST PARAGRAPH_ALIGNMENT  eAlignment : [in] New alignment
// 
BearScriptAPI
VOID  setParagraphAlignment(RICH_TEXT*  pRichText, CONST UINT  iIndex, CONST PARAGRAPH_ALIGNMENT  eAlignment)
{
   RICH_PARAGRAPH*  pParagraph;

   // [CHECK] Lookup paragraph
   if (findListObjectByIndex(pRichText->pParagraphList, iIndex, (LPARAM&)pParagraph))
      // [FOUND] Set alignment
      pParagraph->eAlignment = eAlignment;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                            FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : findNextRichObject
// Description     : Find and return the next tag or text phrase in a string containing X3 RichText formatting tags
// 
// RICHTEXT_TOKENISER*  pToken : [in]  A RichText parsing object containing the source string
// ERROR_STACK*        &pError  : [out] Error, if any
// 
// Return Value   : TRUE if another tag/phrase was found, FALSE if there are no more tags or phrases
// 
BOOL  findNextRichObject(RICHTEXT_TOKENISER*  pToken, ERROR_STACK*  &pError)
{
   CONST TCHAR  *szNextObject,    // Character position that delimits the 'next' object after the one currently being processed
                *szNextTag,       // Same as above, used when determining which 'next object' is closer
                *szNextColour;    // Same as above, used when determining which 'next object' is closer

   // Prepare
   pError = NULL;

   switch (pToken->szCurrentPosition[0])
   {
   // [CHECK] Any more source string left?
   case NULL:  return FALSE;
   /// Determine whether current object is a tag or text from the first character '[' or '\' or <anything>
   default:    pToken->eClass = RTC_TEXT;         break;
   case '[':   pToken->eClass = RTC_FORMAT_TAG;   break;   // Assign 'format tag' for want of anything better.
   case '\\':  pToken->eClass = utilCompareStringN(pToken->szCurrentPosition, "\\033", 4) ? RTC_COLOUR_TAG : RTC_TEXT;   break;
   }
  
   /// Search for the end of the current object.
   switch (pToken->eClass)
   {
   /// [TEXT] -- Search for '[', '\033' or NULL
   case RTC_TEXT:
      // Find first '[' and '\033'
      szNextTag    = findNextNonEscapedCharacter(pToken->szCurrentPosition, '[');  
      szNextColour = StrStr(pToken->szCurrentPosition, TEXT("\\033"));
      // [BOTH FOUND] Use the closest
      if (szNextTag AND szNextColour)
         szNextObject = min(szNextTag, szNextColour);
      // [NEITHER FOUND] Use the end of the string
      else if (!szNextTag AND !szNextColour)
         szNextObject = pToken->szCurrentPosition + lstrlen(pToken->szCurrentPosition);
      // [ONE FOUND] Use the one found
      else
         szNextObject = (szNextTag ? szNextTag : szNextColour);
      break;

   /// [TAGS] -- Search for ']'
   case RTC_FORMAT_TAG:
      // Attempt to position after closing bracket
      if (szNextObject = findNextNonEscapedCharacter(pToken->szCurrentPosition, ']'))
         szNextObject++;   
      else 
      {  // [ERROR] "Missing closing bracket in formatting tag '%s'"
         pError = generateDualError(HERE(IDS_RICHTEXT_TAG_INCOMPLETE), pToken->szCurrentPosition);
         return FALSE;
      }
      break;

   /// [COLOUR] -- Length of '\033B' - 5 chars
   case RTC_COLOUR_TAG:
      szNextObject = &pToken->szCurrentPosition[5];
      break;
   }

   // [TAGS] Determine whether it's an opening or closing tag and consume the '/' if present.
   if (pToken->eClass == RTC_FORMAT_TAG)
   {
      if (pToken->bClosingTag = (pToken->szCurrentPosition[1] == '/'))
         pToken->szCurrentPosition++;
      pToken->bSelfClosingTag = (szNextObject - pToken->szCurrentPosition >= 2 ? szNextObject[-2] == '/' : FALSE);
   }

   // Extract the object
   utilSafeDeleteString(pToken->szText);
   pToken->szText = utilDuplicateString(pToken->szCurrentPosition, szNextObject - pToken->szCurrentPosition);

   /// Identify the tag
   switch (pToken->eClass)
   {
   /// [TEXT] Zero tag ID and measure string
   case RTC_TEXT:
      pToken->eTag = RTT_NONE;
      break;

   /// [TAG] Identify tag from the string
   case RTC_FORMAT_TAG:  
      // Trim tag and identify
      StrTrim(pToken->szText, TEXT("[/]"));
      pToken->eTag = identifyRichTextTag(pToken->szText);

      // Classify object based on the tag
      switch (pToken->eTag)
      {
      // [ALIGNMENT TAGS]
      case RTT_LEFT:
      case RTT_RIGHT:
      case RTT_CENTRE:
      case RTT_JUSTIFY:    
         pToken->eClass = RTC_ALIGNMENT_TAG;   
         break;
      // [COLOUR TAGS]
      case RTT_BLACK:
      case RTT_BLUE:
      case RTT_CYAN:
      case RTT_GREEN:
      case RTT_ORANGE:
      case RTT_PURPLE:
      case RTT_RED:
      case RTT_SILVER:
      case RTT_WHITE:
      case RTT_YELLOW:
         pToken->eClass = RTC_COLOUR_TAG;
         break;
      // [MESSAGE TAGS]
      case RTT_ARTICLE:
      case RTT_AUTHOR:
      case RTT_TITLE:
      case RTT_TEXT:
      case RTT_RANKING:
         pToken->eClass = RTC_MESSAGE_TAG;
         break;
      // [BUTTON TAG]
      case RTT_SELECT:     
         pToken->eClass = RTC_BUTTON_TAG;   
         break;
      // [IMAGE TAG]
      case RTT_IMAGE:
         pToken->eClass = RTC_TOOLTIP_TAG;   
         break;

      // [ERROR] "Unrecognised tag [%s] detected"
      case RTT_NONE:
         pError = generateDualError(HERE(IDS_RICHTEXT_UNKNOWN_TAG), pToken->szText);
         pToken->eClass = RTC_TEXT;

         // Re-Extract the object, but as text
         /*utilSafeDeleteString(pToken->szText);
         pToken->szText = utilDuplicateString(pToken->szCurrentPosition, szNextObject - pToken->szCurrentPosition);
         pToken->eClass = RTC_TEXT;
         pToken->eTag = RTT_NONE;*/
         break;
      }
      break;

   /// [COLOUR] Identify from the letter in the colour tag code
   case RTC_COLOUR_TAG:
      pToken->eTag        = identifyColourTagFromChar(pToken->szText[4]);
      pToken->bClosingTag = (pToken->eTag == RTT_DEFAULT);  
      break;
   }

   /// Calculate token index and length
   pToken->iPosition = (pToken->szCurrentPosition - pToken->szSourceText);
   pToken->iCount    = lstrlen(pToken->szText);

   /// Move the source pointer to the next object
   pToken->szCurrentPosition = szNextObject;
   
   // Return TRUE if successful
   return (pError == NULL);
}




/// Function name  : performMessageCompatibilityChange
// Description     : Remove customisations that are incompatible with the specified compatibility mode
// 
// LANGUAGE_MESSAGE*    pMessage       : [in/out] LanguageMessage to alter
// CONST COMPATIBILITY  eCompatibility : [in]     Desired compatibility
// 
VOID  performMessageCompatibilityChange(LANGUAGE_MESSAGE*  pMessage, CONST COMPATIBILITY  eCompatibility)
{
   switch (eCompatibility)
   {
   // [LOGBOOK / MESSAGE] - At present everything is compatible with both of these
   case LMC_LOGBOOK: /// TODO: Determine what's compatible with logbook messages and what isn't.
   //case LMC_MESSAGE: /// TODO: Determine what's compatible with 'incoming messages' and what isn't.
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



/// Function name  : generateMessageFromGameString
// Description     : Convenience function for generating LanguageMessages from GameStrings
// 
// CONST GAME_STRING*  pSourceText : [in]          Input string containing RichText formatting tags
// LANGUAGE_MESSAGE*  &pOutput     : [out]         New LanguageMessage object, you are responsible for destroying it
// ERROR_QUEUE*        pErrorQueue : [in/out][opt] Error messages, caused by invalid formatting 
// 
// Return Value   : TRUE if succesful, FALSE if there were parsing errors.  If FALSE the RichText object contains the unformatted plaintext
// 
BearScriptAPI
BOOL  generateMessageFromGameString(CONST GAME_STRING*  pSourceText, LANGUAGE_MESSAGE*  &pOutput, ERROR_QUEUE*  pErrorQueue)
{
   BOOL  bResult;

   // [CHECK] Ensure input is internal
   ASSERT(pSourceText->eType == ST_INTERNAL);

   // Pass to generation function
   if (bResult = generateRichTextFromSourceText(pSourceText->szText, pSourceText->iCount, (RICH_TEXT*&)pOutput, RTT_LANGUAGE_MESSAGE, ST_INTERNAL, pErrorQueue))
      // [SUCCESS] Calculate compatibility
      pOutput->eCompatibility = calculateMessageCompatibility(pOutput, pSourceText->szText);
   
   // Return result
   return bResult;
}


/// Function name  : generateRichTextFromGameString
// Description     : Convenience function for generating RichText from GameStrings
// 
// CONST GAME_STRING*  pSourceText : [in]          Input string containing RichText formatting tags
// RICH_TEXT*         &pOutput     : [out]         New RichText object, you are responsible for destroying it
// ERROR_QUEUE*        pErrorQueue : [in/out][opt] Error messages, caused by invalid formatting 
// 
// Return Value   : TRUE if succesful, FALSE if there were parsing errors.  If FALSE the RichText object contains the unformatted plaintext
// 
BearScriptAPI
BOOL  generateRichTextFromGameString(CONST GAME_STRING*  pSourceText, RICH_TEXT*  &pOutput, ERROR_QUEUE*  pErrorQueue)
{
   // [CHECK] Ensure input is internal
   ASSERT(pSourceText->eType == ST_INTERNAL);

   // Pass to generation function
   return generateRichTextFromSourceText(pSourceText->szText, pSourceText->iCount, pOutput, RTT_RICH_TEXT, ST_DISPLAY, pErrorQueue);
}


/// Function name  : generateRichTextFromSourceText
// Description     : Parse a string containing language formatting tags into a new RichText or LanguageMessage object
// 
// CONST TCHAR*          szSourceText  : [in]          PlainText containing RichText formatting tags
// CONST UINT            iTextLength   : [in]          Length of plain text above, in characters
///                                                      -> The String type of input must be ST_INTERNAL
// RICH_TEXT*           &pOutput       : [out]         New RichText object, you are responsible for destroying it
// CONST STRING_TYPE     eOutputType   : [in]          String type of output: ST_DISPLAY / ST_INTERNAL
// ERROR_QUEUE*          pErrorQueue   : [in/out][opt] Error messages, caused by invalid formatting 
// 
// Return Value   : TRUE if parsed succesfully, FALSE otherwise. 
///                 Object is created whether TRUE or FALSE. If FALSE then 'pRichText' contains the unformatted input text
// 
BearScriptAPI
BOOL  generateRichTextFromSourceText(CONST TCHAR*  szSourceText, CONST UINT  iTextLength, RICH_TEXT*  &pOutput, RICHTEXT_TYPE  eType, const STRING_TYPE  eOutputType, ERROR_QUEUE*  pErrorQueue)
{
   LANGUAGE_MESSAGE*       pMessage;          // Convenience pointer
   RICHTEXT_TOKENISER*     pToken;            // Parsing object
   RICH_PARAGRAPH*         pParagraph;        // Current paragraph being processed
   RICH_ITEM*              pItem;             // Current item being processed
   STACK*                  pTagStack;         // Parse stack of the currently open formatting tags
   RICHTEXT_STACK_ITEM*    pTopItem;          // Top stack item
   ERROR_STACK*            pError = NULL;     // Current error, if any
   BOOL                    bFormattingError;  // Whether any formatting errors occurred  (ErrorStack may have been deleted before return)

   // [CHECK] Ensure output type is ST_DISPLAY or ST_INTERNAL
   ASSERT(eOutputType != ST_EXTERNAL);

   // Create the parsing object, stack and output object
   pToken    = createRichTextTokeniser(szSourceText, iTextLength, ST_INTERNAL);    // Must be ST_INTERNAL
   pTagStack = createStack(destructorSimpleObject);
   pOutput   = createRichText(eType);
   pMessage  = (LANGUAGE_MESSAGE*)pOutput;

   // Setup the initial paragraph and item
   findListObjectByIndex(pOutput->pParagraphList, 0, (LPARAM&)pParagraph);
   pItem = createRichItemText(NULL);

   // Tokenise input string into RichObjects
   while (!pError AND findNextRichObject(pToken, pError)) // Abort on error
   {
      // Prepare
      pTopItem = topTagStackItem(pTagStack);

      /// [TEXT] Save text in item or object
      if (pToken->eClass == RTC_TEXT)
      {
         /// [AUTHOR/TITLE] Save as property, not item text
         if (pTopItem AND (pTopItem->eTag == RTT_AUTHOR OR pTopItem->eTag == RTT_TITLE))
         {
            if (pOutput->eType == RTT_LANGUAGE_MESSAGE)
            {
               // Store/convert property text
               switch (pTopItem->eTag)
               {
               case RTT_AUTHOR:  replaceStringConvert(pMessage->szAuthor, SPC_LANGUAGE_INTERNAL_TO_DISPLAY, pToken->szText);  break;
               case RTT_TITLE:   replaceStringConvert(pMessage->szTitle, SPC_LANGUAGE_INTERNAL_TO_DISPLAY, pToken->szText);   break;
               }
            }
            else
               // [ERROR] "Unsupported formatting tag [%s] found in message: [article], [author], [title], [text] and [ranking] are not supported"
               pError = generateDualError(HERE(IDS_RICHTEXT_UNSUPPORTED_LANGUAGE_TAG), convertTagToString(pToken->eTag));
         }
         else
            /// [TEXT] Save in item
            pItem->szText = utilDuplicateString(pToken->szText, pToken->iCount);
      }
      // [CHECK] Ensure no tags are present in the Author/Title tag
      else if (pTopItem AND (pTopItem->eTag == RTT_AUTHOR OR pTopItem->eTag == RTT_TITLE) AND (pToken->eClass == RTC_FORMAT_TAG OR pToken->eClass == RTC_COLOUR_TAG OR pToken->eClass == RTC_BUTTON_TAG))
         // [ERROR] "Using formatting tags within the [author] or [title] tags is not supported by X-Studio"
         pError = generateDualError(HERE(IDS_RICHTEXT_UNSUPPORTED_RICH_AUTHOR));
      
      /// [OPENING TAG] Set item or object properties
      else if (!pToken->bClosingTag)
      {
         // Push stack
         if (!pToken->bSelfClosingTag)       // Ignore self closing tags
            pushObjectOntoStack(pTagStack, (LPARAM)createRichTextStackItem(pToken->eClass, pToken->eTag));

         /// Commit previous item, if any
         if (lstrlen(pItem->szText))
         {
            appendRichTextItem(pParagraph, pItem);
            pItem = createRichItemText(pItem);
         }

         // Set item properties
         switch (pToken->eClass)
         {
         /// [ALIGNMENT TAG] Set alignment or start new paragraph.  Reset attributes
         case RTC_ALIGNMENT_TAG:
            if (getListItemCount(pParagraph->pItemList) == 0)
               pParagraph->eAlignment = (PARAGRAPH_ALIGNMENT)pToken->eTag;
            else
               appendRichTextParagraph(pOutput, pParagraph = createRichParagraph((PARAGRAPH_ALIGNMENT)pToken->eTag));

            // Reset item attributes
            pItem->bBold   = pItem->bItalic = pItem->bUnderline = FALSE;
            pItem->eColour = GTC_DEFAULT;
            break;

         /// [FORMATTING TAG] Add to the item's attributes
         case RTC_FORMAT_TAG:
            pItem->bBold      |= (pToken->eTag == RTT_BOLD);
            pItem->bItalic    |= (pToken->eTag == RTT_ITALIC);
            pItem->bUnderline |= (pToken->eTag == RTT_UNDERLINE);
            break;

         /// [COLOUR TAG] Set the item's colour
         case RTC_COLOUR_TAG:
            pItem->eColour = identifyColourFromTag(pToken->eTag);
            break;

         /// [BUTTON TAG] Parse tag into the item
         case RTC_BUTTON_TAG:
            pItem->eType   = RIT_BUTTON; 
            pItem->eColour = GTC_DEFAULT;

            // [CHECK] Ensure buttons are allowed
            if (pOutput->eType != RTT_LANGUAGE_MESSAGE)
               // [ERROR] "Unsupported formatting tag [%s] found in message: [article], [author], [title], [text] and [ranking] are not supported"
               pError = generateDualError(HERE(IDS_RICHTEXT_UNSUPPORTED_LANGUAGE_TAG), convertTagToString(pToken->eTag));
            
            // [CHECK] Parse + Validate ID
            else if (translateLanguageMessageTag(pToken, NULL, pItem, pError) AND !isRichEditButtonUnique(pOutput, pItem->szID))
               // [ERROR] "Duplicate button ID '%s' found in [select] tag"
               pError = generateDualError(HERE(IDS_RICHTEXT_BUTTON_ID_DUPLICATE), pItem->szID);
            break;

         /// [SPECIAL TAG] Parse tag into the LanguageMessage object
         case RTC_MESSAGE_TAG:
            if (pOutput->eType == RTT_LANGUAGE_MESSAGE)
               translateLanguageMessageTag(pToken, pMessage, NULL, pError);
            else
               // [ERROR] "Unsupported formatting tag [%s] found in toolip message: [article], [author], [title], [text] and [ranking] are not supported"
               pError = generateDualError(HERE(IDS_RICHTEXT_UNSUPPORTED_LANGUAGE_TAG), convertTagToString(pToken->eTag));
            break;
         }
      }
      /// [CLOSING TAG] Commit item + clear attributes
      else if (pToken->bClosingTag)
      {
         // [CHECK] Ensure there is at least one open tag
         if (!getStackItemCount(pTagStack) AND pToken->eTag != RTT_DEFAULT)     // Allow excess \033X tags, they have no effect
            // [ERROR] "Unexpected formatting tag [/%s] found in message"
            pError = generateDualError(HERE(IDS_RICHTEXT_EXCESS_CLOSING_TAGS), convertTagToString(pToken->eTag));
         else 
         {
            /// Commit item, if any
            if (lstrlen(pItem->szText))   // Closing nested tags in sequence will produce an empty item
            {
               appendRichTextItem(pParagraph, pItem);
               pItem = createRichItemText(pItem);
            }

            // Revert state appropriately
            switch (pToken->eClass)
            {
            /// [FORMATTING TAG] - Remove the appropriate formatting attribute from the current item
            case RTC_FORMAT_TAG:
               pItem->bBold      ^= (pToken->eTag == RTT_BOLD);
               pItem->bItalic    ^= (pToken->eTag == RTT_ITALIC);
               pItem->bUnderline ^= (pToken->eTag == RTT_UNDERLINE);
               break;
            
            /// [COLOUR TAG] - Revert to the default colour
            case RTC_COLOUR_TAG:
               pItem->eColour = GTC_DEFAULT;
               break;

            /// [PARAGRAPH TAG] Reset item attributes
            case RTC_ALIGNMENT_TAG:
               pItem->bBold   = pItem->bItalic = pItem->bUnderline = FALSE;
               pItem->eColour = GTC_DEFAULT;
               break;
            }

            // Pop the parse stack
            destroyTopStackObject(pTagStack);
         }

      } // END: if (closing tag)

   } // END: while (findNextObject)

   // [CHECK] Ensure message doesn't just contain an opening/closing colour/formatting tag -- otherwise they'll be erased
   if (!pError AND iTextLength AND getParagraphCount(pOutput) == 1 AND getRichItemCount(pParagraph) == 0 AND (pToken->eClass == RTC_FORMAT_TAG OR pToken->eClass == RTC_COLOUR_TAG))
      // [ERROR] "Messages containing only orphan formatting tags are not supported by X-Studio"
      pError = generateDualError(HERE(IDS_RICHTEXT_UNSUPPORTED_ORPHAN_TAG));

   /// Commit final item (if exists)
   lstrlen(pItem->szText) ? appendRichTextItem(pParagraph, pItem) : deleteRichItem(pItem);

   // Pop any remaining colour tags from the stack
   while ((pTopItem = topTagStackItem(pTagStack)) AND pTopItem->eClass == RTC_COLOUR_TAG)
      destroyTopStackObject(pTagStack);
   
   /// [ERROR] Return the input plaintext
   if (bFormattingError = (pError != NULL))
   {
      // Delete partially parsed content
      deleteRichText(pOutput);
      pOutput = createRichText(eType);

      // Replace with a single PlainText item
      findListObjectByIndex(pOutput->pParagraphList, 0, (LPARAM&)pParagraph);
      appendRichTextItem(pParagraph, createRichItemPlainText(szSourceText, iTextLength));

      // Attach plain-text to message, then Append/Destroy error appropriately
      attachTextToError(pError, szSourceText); 
      pErrorQueue ? pushErrorQueue(pErrorQueue, pError) : deleteErrorStack(pError);
   }
   /// [SUCCESS] Convert to ST_DISPLAY, if required
   else if (eOutputType == ST_DISPLAY)
      convertRichEditText(pOutput, ST_DISPLAY);

   // [CHECK] Ensure all tags were closed
   if (getStackItemCount(pTagStack) AND pErrorQueue)
      /// [WARNING] "There is an unclosed [%s] formatting tag remaining in the RichText message"
      pushErrorQueue(pErrorQueue, generateDualWarning(HERE(IDS_RICHTEXT_OPEN_TAGS_REMAINING), convertTagToString(topTagStackItem(pTagStack)->eTag)) );
   
   // Cleanup and return
   deleteRichTextTokeniser(pToken);
   deleteStack(pTagStack);
   return !bFormattingError;
}


/// Function name  : generateSourceTextFromRichText
// Description     : Generate plain text from a LanguageMessage object and store it in a GameString
// 
// CONST LANGUAGE_MESSAGE*  pMessage    : [in]     Language message to convert
// GAME_STRING*             pGameString : [in/out] GameString to receive plain text. String type will be ST_INTERNAL
// 
// Return Value   : TRUE
// 
BearScriptAPI
BOOL  generateSourceTextFromRichText(CONST LANGUAGE_MESSAGE*  pMessage, GAME_STRING*  pOutput)
{
   RICHTEXT_ATTRIBUTES   oState;                 // Current formatting state
   RICH_PARAGRAPH*       pParagraph;             // Paragraph iterator
   RICH_ITEM*            pItem;                  // Item iterator
   BOOL                  bDefaultParagraph,      // TRUE if the formatting of the first paragraph is LEFT - ie. what's assumed if there is no initial alignment tag
                         bTextTag = FALSE;       // TRUE if the message uses anything other than a single standard column
                         
   TEXT_STREAM*          pStream;                // Output stream

   // [CHECK] Validate parameters
   ASSERT(pMessage->eType == RTT_LANGUAGE_MESSAGE AND pOutput);

   // Prepare 
   utilZeroObject(&oState, RICHTEXT_ATTRIBUTES);
   oState.eAlignment = PA_LEFT;
   oState.eColour    = GTC_DEFAULT;
   pStream           = createTextStream(8 * 1024);

   // [LOGBOOK] Insert extended tags
   //if (pMessage->eCompatibility == LMC_LOGBOOK)
   {
      /// [ARTICLE] Add this first, if required
      if (pMessage->bArticle)
         appendStringToTextStream(pStream, TEXT("[article/]"));

      /// [RANKING] Add only if a custom rank is present
      if (pMessage->bCustomRank AND lstrlen(pMessage->szRankTitle))
         appendStringToTextStreamf(pStream, TEXT("[ranking type='%s' title='%s'/]"), pMessage->eRankType == RT_FIGHT ? TEXT("fight") : TEXT("trade"), pMessage->szRankTitle);
      else if (pMessage->bCustomRank)
         appendStringToTextStreamf(pStream, TEXT("[ranking type='%s'/]"), pMessage->eRankType == RT_FIGHT ? TEXT("fight") : TEXT("trade"));

      /// [AUTHOR and TITLE TAGS] Add these second, if present
      if (lstrlen(pMessage->szAuthor))
         appendStringToTextStreamf(pStream, TEXT("[author]%s[/author]"), pMessage->szAuthor);
      if (lstrlen(pMessage->szTitle))
         appendStringToTextStreamf(pStream, TEXT("[title]%s[/title]"), pMessage->szTitle);
      
      /// [COLUMNS] Add only if any of the three custom column properties are used
      if (bTextTag = (pMessage->iColumnCount > 1 OR pMessage->bCustomWidth OR pMessage->bCustomSpacing))
      {
         appendStringToTextStreamf(pStream, pMessage->iColumnCount > 1 ? TEXT("[text cols='%u'") : TEXT("[text"), pMessage->iColumnCount);

         /// [HACK]
         if (pMessage->iColumnCount == 0 AND extractListItemPointer(getListHead(pMessage->pParagraphList), RICH_PARAGRAPH)->eAlignment == PA_JUSTIFY)
            appendStringToTextStream(pStream, TEXT(" cols='1'"));

         // Column widths
         if (pMessage->bCustomWidth)
            appendStringToTextStreamf(pStream, TEXT(" colwidth='%u'"), pMessage->iColumnWidth);
         // Column spacings
         if (pMessage->bCustomSpacing)
            appendStringToTextStreamf(pStream, TEXT(" colspacing='%u'"), pMessage->iColumnSpacing);
         // Close tag
         appendStringToTextStream(pStream, TEXT("]"));
      }
      /*else if (bTextTag = lstrlen(pMessage->szAuthor) OR lstrlen(pMessage->szTitle))
         appendStringToTextStream(pStream, TEXT("[text]"));*/
   }
   
   /// Iterate through paragraphs and their items
   for (LIST_ITEM*  pParagraphIterator = getListHead(pMessage->pParagraphList); pParagraph = extractListItemPointer(pParagraphIterator, RICH_PARAGRAPH); pParagraphIterator = pParagraphIterator->pNext)
   {
      // Determine if this paragraph is using the default formatting (left aligned initial paragraph)
      bDefaultParagraph = (!pParagraphIterator->pPrev AND pParagraph->eAlignment == PA_LEFT);

      // [PARA] Open paragraph tag if necessary
      if (!bDefaultParagraph)
         appendStringToTextStreamf(pStream, TEXT("[%s]"), szAlignmentTags[pParagraph->eAlignment]);

      /// Iterate through items within the paragraph
      for (LIST_ITEM*  pItemIterator = getListHead(pParagraph->pItemList); pItem = extractListItemPointer(pItemIterator, RICH_ITEM); pItemIterator = pItemIterator->pNext)
      {
         // [TEXT]
         if (pItem->eType == RIT_TEXT)
         {
            // [FORMAT] Check for changes in the formatting state and open/close the appropriate tags
            if (pItem->bBold != oState.bBold)
               appendStringToTextStream(pStream, pItem->bBold > oState.bBold ? TEXT("[b]") : TEXT("[/b]"));
            if (pItem->bItalic != oState.bItalic)
               appendStringToTextStream(pStream, pItem->bItalic > oState.bItalic ? TEXT("[i]") : TEXT("[/i]"));
            if (pItem->bUnderline != oState.bUnderline)
               appendStringToTextStream(pStream, pItem->bUnderline > oState.bUnderline ? TEXT("[u]") : TEXT("[/u]"));
            
            // Update formatting state
            oState.bBold      = pItem->bBold;
            oState.bItalic    = pItem->bItalic;
            oState.bUnderline = pItem->bUnderline;

            // [COLOUR] Check for colour changes
            if (oState.eColour != pItem->eColour)
            {
               // [LOGBOOK] Close current tag and open new one
               if (pMessage->eCompatibility == LMC_LOGBOOK)
               {
                  // If current colour is default, don't close it
                  if (oState.eColour != GTC_DEFAULT)
                     appendStringToTextStream(pStream, getCompatibleColourTag(pMessage, oState.eColour, false)); 
                  // If new colour is default don't open it
                  if (pItem->eColour != GTC_DEFAULT)
                     appendStringToTextStream(pStream, getCompatibleColourTag(pMessage, pItem->eColour, true)); 
               }
               else
                  // [CUSTOM MENU] Switch colour mode
                  appendStringToTextStream(pStream, getCompatibleColourTag(pMessage, pItem->eColour, false)); 

               // Update state
               oState.eColour = pItem->eColour;
            }

            /// Insert item text
            appendStringToTextStream(pStream, pItem->szText);

            // [Editor Text is now ST_INTERNAL] generateConvertedString(pItem->szText, SPC_RICHTEXT_DISPLAY_TO_INTERNAL, szConverted);    // Convert DISPLAY->INTERNAL
            //appendStringToTextStream(pStream, utilEither(szConverted, pItem->szText));
            //utilSafeDeleteString(szConverted);
         }
         /// [BUTTON] Generate button with/without ID and/or coloured/non-coloured
         else 
         {  
            appendStringToTextStreamf(pStream, lstrlen(pItem->szID) ? TEXT("[select value='%s']") : TEXT("[select]"), pItem->szID);
            appendStringToTextStreamf(pStream, TEXT("%s%s%s[/select]"), pItem->eColour != GTC_DEFAULT ? getCompatibleColourTag(pMessage, pItem->eColour, true) : TEXT(""),
                                                                        pItem->szText, pItem->eColour != GTC_DEFAULT ? getCompatibleColourTag(pMessage, pItem->eColour, false) : TEXT(""));
         }
      }

      // [FORMAT] Close any remaining open formatting tags
      appendStringToTextStream(pStream, oState.bBold      ? TEXT("[/b]") : TEXT(""));
      appendStringToTextStream(pStream, oState.bItalic    ? TEXT("[/i]") : TEXT(""));
      appendStringToTextStream(pStream, oState.bUnderline ? TEXT("[/u]") : TEXT(""));
      oState.bBold = oState.bItalic = oState.bUnderline = FALSE;

      // [PARA] Close paragraph tag if necessary
      if (!bDefaultParagraph)
         appendStringToTextStreamf(pStream, TEXT("[/%s]"), szAlignmentTags[pParagraph->eAlignment]);
   }

   // [COLOUR] Close/Reset colour tag if neccesary
   if (oState.eColour != GTC_DEFAULT)                                      // Pass 'Close colour' for logbook, and 'Switch Default' for CustomMenu
      appendStringToTextStream(pStream, getCompatibleColourTag(pMessage, (pMessage->eCompatibility == LMC_LOGBOOK ? oState.eColour : GTC_DEFAULT), false)); 

   // [TEXT] Close tag
   if (bTextTag)
      appendStringToTextStream(pStream, TEXT("[/text]"));

   /// Update existing GameString
   updateGameStringText(pOutput, pStream->szBuffer);
   pOutput->eType  = ST_INTERNAL;

   // Cleanup
   deleteTextStream(pStream);
   return TRUE;
}


/// Function name  : translateLanguageMessageTag
// Description     : Parse the properties of a Language message RichText tag - [TEXT], [ARTICLE], [RANKING] or [SELECT].
//                     [AUTHOR] and [TITLE] tags are also accepted but ignored.
// 
// CONST RICHTEXT_TOKENISER*  pTokeniser   : [in]               RichObject containing a [TEXT], [ARTICLE], [RANKING] or [SELECT] tag
// LANGUAGE_MESSAGE*          pMessage     : [in/out][optional] LanguageMessage into which the tag should be parsed
// RICH_ITEM*                 pButton      : [in/out][optional] RichText 'button' item into which the tag should be parsed
// ERROR_STACK*               pError       : [out]              New error containing parsing error, otherwise NULL
// 
// Return Value   : TRUE if parsed succesful, FALSE if there were errors
// 
BOOL   translateLanguageMessageTag(CONST RICHTEXT_TOKENISER*  pTokeniser, LANGUAGE_MESSAGE*  pMessage, RICH_ITEM*  pButton, ERROR_STACK*  &pError)
{
   RICHTEXT_TOKEN_STATE   eState;       // Processing state
   TCHAR                 *szToken,      // Current token
                         *szTokenEnd,   // End position of the current token
                         *szTextCopy;   // Copy of the current RichObject text
                         
   // Examine tag
   switch (pTokeniser->eTag)
   {
   // [CUSTOM] Set message flag when detected
   case RTT_ARTICLE: pMessage->bArticle    = TRUE;  break;
   case RTT_RANKING: pMessage->bCustomRank = TRUE;  break;
   // [DEFAULT] Process normally
   case RTT_SELECT:  
   case RTT_IMAGE:  
   case RTT_TEXT:    break;
   // [NO PROPERTIES] No processing required
   default:          return TRUE;
   }

   // Prepare
   szTextCopy = utilDuplicateString(pTokeniser->szText, pTokeniser->iCount);
   eState     = RTS_PROPERTY_NAME;
   pError     = NULL;

   /// Consume tag name
   szToken = (szTextCopy + lstrlen(convertTagToString(pTokeniser->eTag)));

   // Tokenise tag manually since the tokeniser can't handle empty strings.  Search for value delimiters
   while ((szTokenEnd = utilFindCharacterInSet(szToken, "'\"")) AND !pError)  // Abort on errors
   {
      // Null terminate token
      szTokenEnd[0] = NULL;

      switch (eState)
      {
      /// [PROPERTY NAME] -- Trim property name and then identify it.
      case RTS_PROPERTY_NAME:
         StrTrim(szToken, TEXT(" ="));
         // [SELECT] properties
         if (utilCompareString(szToken, "value"))
            eState = RTS_BUTTON_ID;

         // [IMAGE] properties
         else if (utilCompareString(szToken, "imgtype"))
            eState = RTS_IMAGE_TYPE;
         else if (utilCompareString(szToken, "id"))
            eState = RTS_IMAGE_ID;

         // [TEXT] properties
         else if (utilCompareString(szToken, "cols"))
            eState = RTS_COLUMN_COUNT;
         else if (utilCompareString(szToken, "colwidth"))
            eState = RTS_COLUMN_WIDTH;
         else if (utilCompareString(szToken, "colspacing"))
            eState = RTS_COLUMN_SPACING;

         // [RANK] properties
         else if (utilCompareString(szToken, "type"))
            eState = RTS_RANK_TYPE;
         else if (utilCompareString(szToken, "title"))
            eState = RTS_RANK_TITLE;

         // [ARTICLE] properties
         else if (utilCompareString(szToken, "state"))
            eState = RTS_ARTICLE_STATE;
         else
            /// [ERROR] "Unknown property '%s' detected while translating formatting tag [%s]"
            pError = generateDualError(HERE(IDS_RICHTEXT_UNKNOWN_TAG_PROPERTY), szToken, convertTagToString(pTokeniser->eTag));
         
         // Move token to character after null terminator we inserted
         szToken = &szTokenEnd[1];
         continue;

      /// [IMAGE PROPERTIES]
      case RTS_IMAGE_ID:       pButton->szID       = utilDuplicateString(szToken, lstrlen(szToken));  break;
      case RTS_IMAGE_TYPE:     pButton->iImageType = (utilCompareString(szToken, "IMAGE_BITMAP") ? IMAGE_BITMAP : IMAGE_ICON);  break;

      /// [BUTTON PROPERTIES]
      case RTS_BUTTON_ID:      pButton->szID = utilDuplicateString(szToken, lstrlen(szToken));  break;

      /// [TEXT PROPERTIES]
      case RTS_COLUMN_COUNT:   pMessage->iColumnCount   = utilConvertStringToInteger(szToken);  break;
      case RTS_COLUMN_WIDTH:   pMessage->iColumnWidth   = utilConvertStringToInteger(szToken);  pMessage->bCustomWidth = TRUE;    break;
      case RTS_COLUMN_SPACING: pMessage->iColumnSpacing = utilConvertStringToInteger(szToken);  pMessage->bCustomSpacing = TRUE;  break;

      /// [RANKING PROPERTIES]
      case RTS_RANK_TYPE:      pMessage->eRankType = (utilCompareString(szToken, "fight") ? RT_FIGHT : RT_TRADE);      break;
      case RTS_RANK_TITLE:     replaceStringConvert(pMessage->szRankTitle, SPC_LANGUAGE_INTERNAL_TO_DISPLAY, szToken); break;

      /// [ARTICLE PROPERTIES]
      case RTS_ARTICLE_STATE:  
         pMessage->iArticleState = utilConvertStringToInteger(szToken);
         break;
      }

      // Reset to searching for a property name and update the token pointer.
      eState = RTS_PROPERTY_NAME;
      szToken = &szTokenEnd[1];
   }

   // Cleanup and return TRUE if there were no errors
   utilDeleteString(szTextCopy);
   return (pError == NULL);
}

