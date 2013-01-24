//
// Rich Text.cpp : Functions for parsing, storing and displaying Rich (formatted) text
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       CONSTANTS / GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

CONST TCHAR*  szAlignmentTags[5] = { TEXT("N/A"),    TEXT("left"),    TEXT("right"), TEXT("center"), TEXT("justify") };
CONST TCHAR*  szColourTags[10]   = { TEXT("black"),  TEXT("blue"),    TEXT("cyan"),  TEXT("N/A"),    TEXT("green"), 
                                     TEXT("orange"), TEXT("magenta"), TEXT("red"),   TEXT("white"),  TEXT("yellow") };

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       CREATION / DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////////////////


/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                            HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : appendRichTextItemToParagraph
// Description     : Add an item to a RichText paragraph's list of items
// 
// RICH_PARAGRAPH*  pParagraph  : [in/out] Paragraph to append
// RICH_ITEM*       pItem       : [in]     Item to be appended to the paragraph, above
// 
BearScriptAPI
VOID  appendRichTextItemToParagraph(RICH_PARAGRAPH*  pParagraph, RICH_ITEM*  pItem)
{
   TCHAR*  szDisplayText;

   // Convert "\n" and "\"" phrases into proper characters
   if (pItem->szText AND generateConvertedString(pItem->szText, SPC_LANGUAGE_INTERNAL_TO_DISPLAY, szDisplayText))
   {
      utilDeleteString(pItem->szText);
      pItem->szText = szDisplayText;
   }

   // Update string type
   pItem->eTextType = ST_DISPLAY;

   // Add item
   appendObjectToList(pParagraph->pItemList, (LPARAM)pItem);
}


/// Function name  : convertRichTextTagToString
// Description     : Converts a tag ID to a string
// 
// RICHTEXT_TAG  eTag   : [in] Tag ID
// 
// Return Value   : String
// 
CONST TCHAR*  convertRichTextTagToString(RICHTEXT_TAG  eTag)
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
   case RTT_YELLOW:  szOutput = TEXT("yellow");   break;

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


/// Function name  : identifyGameTextColourFromRichTextTag
// Description     : Convert a RichText message tag to a game text colour
// 
// CONST RICHTEXT_TAG  eTag  : [in] RichText message tag
// 
// Return Value   : Game text colour, or default if tag is unrecognised
// 
GAME_TEXT_COLOUR  identifyGameTextColourFromRichTextTag(CONST RICHTEXT_TAG  eTag)
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


/// Function name  : setRichTextParagraphAlignment
// Description     : Adjust the alignment of a paragraph in a RichText object
// 
// RICH_TEXT*                 pRichText  : [in] RichText object
// CONST UINT                 iIndex     : [in] Zero-based index of the paragraph to set
// CONST PARAGRAPH_ALIGNMENT  eAlignment : [in] New alignment
// 
BearScriptAPI
VOID  setRichTextParagraphAlignment(RICH_TEXT*  pRichText, CONST UINT  iIndex, CONST PARAGRAPH_ALIGNMENT  eAlignment)
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
// RICHTEXT_TOKENISER*  pObject : [in] A RichText parsing object containing the source string
// 
// Return Value   : TRUE if another tag/phrase was found, FALSE if there are no more tags or phrases
// 
BOOL  findNextRichObject(RICHTEXT_TOKENISER*  pObject)
{
   CONST TCHAR  *szNextObject,    // Character position that delimits the 'next' object after the one currently being processed
                *szNextTag,       // Same as above, used when determining which 'next object' is closer
                *szNextColour;    // Same as above, used when determining which 'next object' is closer

   switch (pObject->szCurrentPosition[0])
   {
   // [CHECK] Any more source string left?
   case NULL:  return FALSE;
   /// Determine whether current object is a tag or text from the first character '[' or '\' or <anything>
   default:    pObject->eClass = RTC_TEXT;         break;
   case '[':   pObject->eClass = RTC_FORMAT_TAG;   break;   // Assign 'format tag' for want of anything better.
   case '\\':  pObject->eClass = utilCompareStringN(pObject->szCurrentPosition, "\\033", 4) ? RTC_COLOUR_TAG : RTC_TEXT;   break;
   }
  
   /// Search for the end of the current object.
   switch (pObject->eClass)
   {
   /// [TEXT] -- Search for '[', '\033' or NULL
   case RTC_TEXT:
      // Find first '[' and '\033'
      szNextTag    = findNextNonEscapedCharacter(pObject->szCurrentPosition, '[');  
      szNextColour = StrStr(pObject->szCurrentPosition, TEXT("\\033"));
      // [BOTH FOUND] Use the closest
      if (szNextTag AND szNextColour)
         szNextObject = min(szNextTag, szNextColour);
      // [NEITHER FOUND] Use the end of the string
      else if (!szNextTag AND !szNextColour)
         szNextObject = pObject->szCurrentPosition + lstrlen(pObject->szCurrentPosition);
      // [ONE FOUND] Use the one found
      else
         szNextObject = (szNextTag ? szNextTag : szNextColour);
      break;

   /// [TAGS] -- Search for ']'
   case RTC_FORMAT_TAG:
      if (szNextObject = findNextNonEscapedCharacter(pObject->szCurrentPosition, ']'))  ///if (szNextObject = utilFindCharacter(pObject->szSource, ']'))
         szNextObject++;
      break;

   /// [COLOUR] -- Length of '\033B' - 5 chars
   case RTC_COLOUR_TAG:
      szNextObject = &pObject->szCurrentPosition[5];
      break;
   }

   // [TAGS] Determine whether it's an opening or closing tag and consume the '/' if present.
   if (pObject->eClass == RTC_FORMAT_TAG)
   {
      if (pObject->bClosingTag = (pObject->szCurrentPosition[1] == '/'))
         pObject->szCurrentPosition++;
      pObject->bSelfClosingTag = (szNextObject[-2] == '/');
   }

   // Extract the object
   utilSafeDeleteString(pObject->szText);
   pObject->szText = utilDuplicateString(pObject->szCurrentPosition, szNextObject - pObject->szCurrentPosition);

   /// Identify the tag
   switch (pObject->eClass)
   {
   /// [TEXT] Zero tag ID and measure string
   case RTC_TEXT:
      pObject->eTag = RTT_NONE;
      break;

   /// [TAG] Identify tag from the string
   case RTC_FORMAT_TAG:  
      // Trim tag and identify
      StrTrim(pObject->szText, TEXT("[/]"));
      pObject->eTag = identifyRichTextTag(pObject->szText);

      // Classify object based on the tag
      switch (pObject->eTag)
      {
      // [ALIGNMENT TAGS]
      case RTT_LEFT:
      case RTT_RIGHT:
      case RTT_CENTRE:
      case RTT_JUSTIFY:    
         pObject->eClass = RTC_ALIGNMENT_TAG;   
         break;
      // [COLOUR TAGS]
      case RTT_BLACK:
      case RTT_BLUE:
      case RTT_CYAN:
      case RTT_GREEN:
      case RTT_ORANGE:
      case RTT_PURPLE:
      case RTT_RED:
      case RTT_WHITE:
      case RTT_YELLOW:
         pObject->eClass = RTC_COLOUR_TAG;
         break;
      // [MESSAGE TAGS]
      case RTT_ARTICLE:
      case RTT_AUTHOR:
      case RTT_TITLE:
      case RTT_TEXT:
      case RTT_RANKING:
         pObject->eClass = RTC_MESSAGE_TAG;
         break;
      // [BUTTON TAG]
      case RTT_SELECT:     
         pObject->eClass = RTC_BUTTON_TAG;   
         break;
      // [IMAGE TAG]
      case RTT_IMAGE:
         pObject->eClass = RTC_TOOLTIP_TAG;   
         break;
      // [PARSE ERROR]
      case RTT_NONE:
         pObject->eClass = RTC_TEXT;
         break;
      }
      break;

   /// [COLOUR] Identify from the letter in the colour tag code
   case RTC_COLOUR_TAG:
      switch (pObject->szText[4])
      {
      case 'A': pObject->bClosingTag = FALSE;  pObject->eTag = RTT_WHITE;    break;
      case 'B': pObject->bClosingTag = FALSE;  pObject->eTag = RTT_BLUE;     break;
      case 'C': pObject->bClosingTag = FALSE;  pObject->eTag = RTT_CYAN;     break;
      case 'G': pObject->bClosingTag = FALSE;  pObject->eTag = RTT_GREEN;    break;
      case 'M': pObject->bClosingTag = FALSE;  pObject->eTag = RTT_PURPLE;   break;
      case 'O': pObject->bClosingTag = FALSE;  pObject->eTag = RTT_ORANGE;   break;
      case 'R': pObject->bClosingTag = FALSE;  pObject->eTag = RTT_RED;      break;
      case 'W': pObject->bClosingTag = FALSE;  pObject->eTag = RTT_WHITE;    break;
      case 'Y': pObject->bClosingTag = FALSE;  pObject->eTag = RTT_YELLOW;   break;
      case 'Z': pObject->bClosingTag = FALSE;  pObject->eTag = RTT_BLACK;    break;
      default:  pObject->bClosingTag = TRUE;   pObject->eTag = RTT_DEFAULT;  break;
      }
      break;
   }

   /// Move the source pointer to the next object
   pObject->szCurrentPosition = szNextObject;
   // Measure object and return
   pObject->iCount = lstrlen(pObject->szText);
   return TRUE;
}



/// Function name  : generatLanguageMessageFromGameString
// Description     : Convenience function for generating LanguageMessages from GameStrings
// 
// CONST GAME_STRING*  pSourceText : [in]  Input string containing RichText formatting tags
// LANGUAGE_MESSAGE*  &pOutput     : [out] New LanguageMessage object, you are responsible for destroying it
// ERROR_QUEUE*        pErrorQueue : [out] Error message, if any, caused by formatting errors
// 
// Return Value   : TRUE if succesful, FALSE if there were parsing errors.  If FALSE the RichText object contains the unformatted plaintext
// 
BearScriptAPI
BOOL  generateLanguageMessageFromGameString(CONST GAME_STRING*  pSourceText, LANGUAGE_MESSAGE*  &pOutput, ERROR_QUEUE*  pErrorQueue)
{
   // Pass to generation function
   return generateRichTextFromSourceText(pSourceText->szText, pSourceText->iCount, pSourceText->eType, (RICH_TEXT*&)pOutput, RTT_LANGUAGE_MESSAGE, pErrorQueue);
}


/// Function name  : generateRichTextFromGameString
// Description     : Convenience function for generating RichText from GameStrings
// 
// CONST GAME_STRING*  pSourceText : [in]  Input string containing RichText formatting tags
// RICH_TEXT*         &pOutput     : [out] New RichText object, you are responsible for destroying it
// ERROR_QUEUE*        pErrorQueue : [out] Error message, if any, caused by formatting errors
// 
// Return Value   : TRUE if succesful, FALSE if there were parsing errors.  If FALSE the RichText object contains the unformatted plaintext
// 
BearScriptAPI
BOOL  generateRichTextFromGameString(CONST GAME_STRING*  pSourceText, RICH_TEXT*  &pOutput, ERROR_QUEUE*  pErrorQueue)
{
   // Pass to generation function
   return generateRichTextFromSourceText(pSourceText->szText, pSourceText->iCount, pSourceText->eType, pOutput, RTT_RICH_TEXT, pErrorQueue);
}


/// Function name  : generateRichTextFromSourceText
// Description     : Parse a string containing language formatting tags into a new RichText or LanguageMessage object
// 
// CONST TCHAR*          szSourceText  : [in]  PlainText containing RichText formatting tags
// CONST UINT            iTextLength   : [in]  Length of plain text above, in characters
// CONST STRING_TYPE     eStringType   : [in]  String type of source-text
// RICH_TEXT*           &pOutput       : [out] New RichText object, you are responsible for destroying it
///                                                -> The String type of the RichText object is always ST_DISPLAY 
// CONST RICHTEXT_TYPE   eObjectType   : [in]  Whether to create a RichText or LanguageMessage object
// ERROR_QUEUE*          pErrorQueue   : [in/out] Error messages, caused by invalid formatting 
// 
// Return Value   : TRUE if parsed succesfully, FALSE otherwise. 
///                 Object is created whether TRUE or FALSE. If FALSE then 'pRichText' contains the unformatted input text
// 
BearScriptAPI
BOOL  generateRichTextFromSourceText(CONST TCHAR*  szSourceText, CONST UINT  iTextLength, CONST STRING_TYPE  eStringType, RICH_TEXT*  &pOutput, CONST RICHTEXT_TYPE  eObjectType, ERROR_QUEUE*  pErrorQueue)
{
   LANGUAGE_MESSAGE*       pLanguageMessage;  // Convenience pointer for accessing RichText object as a LanguageMessage object
   RICHTEXT_TOKENISER*     pTokeniser;        // Parsing object
   RICH_PARAGRAPH*         pParagraph;        // Current paragraph being processed
   RICH_ITEM*              pItem;             // Current item being processed
   STACK*                  pTagStack;            // Parse stack of the currently open formatting tags
   RICHTEXT_STACK_ITEM*    pTopItem;          // Top stack item
   ERROR_STACK*            pError;            // Current error, if any
   BOOL                    bFormattingError;  // Whether any formatting errors occurred  (ErrorStack may have been deleted before return)
   
   // Prepare
   pError = NULL;
   bFormattingError = FALSE;

   /// Create the parsing object, stack and output object
   pTokeniser = createRichTextTokeniser(szSourceText, iTextLength, eStringType);    // Converts string to ST_INTERNAL
   pTagStack  = createStack(destructorSimpleObject);
   pOutput    = createRichText(eObjectType);

   // Setup the initial paragraph and item
   findListObjectByIndex(pOutput->pParagraphList, 0, (LPARAM&)pParagraph);
   pItem = createRichItemText(NULL);

   // Tokenise input string into RichObjects
   while (findNextRichObject(pTokeniser) AND !pError) // Abort on error
   {
      /// [OPENING TAG] Add to the stack - except for 'NONE' and self closing tags (eg. [ranking])
      if (pTokeniser->eClass != RTC_TEXT AND !pTokeniser->bClosingTag AND !pTokeniser->bSelfClosingTag)
         pushObjectOntoStack(pTagStack, (LPARAM)createRichTextStackItem(pTokeniser->eClass, pTokeniser->eTag));
      
      /// [CLOSING TAG] Pop the stack
      else if (pTokeniser->eClass != RTC_TEXT AND pTokeniser->bClosingTag)
      {
         // [CHECK] Ensure there is at least one open tag
         if (!getStackItemCount(pTagStack))
         {  
            // [ERROR] "Unexpected formatting tag [/%s] found in message"
            pError = generateDualError(HERE(IDS_RICHTEXT_EXCESS_CLOSING_TAGS), convertRichTextTagToString(pTokeniser->eTag));
            break;
         }
         else switch (pTokeniser->eClass)
         {
         // [CLOSING FORMATTING TAG] - Remove the appropriate formatting attribute from the current item
         case RTC_FORMAT_TAG:
            pItem->bBold      ^= (pTokeniser->eTag == RTT_BOLD);
            pItem->bItalic    ^= (pTokeniser->eTag == RTT_ITALIC);
            pItem->bUnderline ^= (pTokeniser->eTag == RTT_UNDERLINE);
            break;
         
         // [CLOSING COLOUR TAG] - Revert to the default colour
         case RTC_COLOUR_TAG:
            pItem->eColour = GTC_DEFAULT;
            break;

         // [CLOSING PARAGRAPH TAG] - Reset current paragraph
         case RTC_ALIGNMENT_TAG:
            pParagraph = NULL;
            break;
         }

         // Pop the parse stack
         destroyTopStackObject(pTagStack);
         continue;
      }

      // [CHECK] Has user just closed a paragraph without opening another?
      if (!pParagraph AND pTokeniser->eClass != RTC_ALIGNMENT_TAG)
         // [DEFAULT] Silently create a new left-aligned paragraph 
         appendObjectToList(pOutput->pParagraphList, (LPARAM)(pParagraph = createRichParagraph(PA_LEFT)));
      
      /// Examine the new object
      switch (pTokeniser->eClass)
      {
      /// [MESSAGE PROPERTY TAG] -- Parse message property tag into the message object directly
      case RTC_MESSAGE_TAG:
         // Attempt to parse properties from tag
         if (eObjectType == RTT_LANGUAGE_MESSAGE)
            translateLanguageMessageTag(pTokeniser, (LANGUAGE_MESSAGE*)pOutput, NULL, pErrorQueue);
         else
            // [ERROR] "Unsupported formatting tag [%s] found in toolip message: [article], [author], [title], [text] and [ranking] are not supported"
            pError = generateDualError(HERE(IDS_RICHTEXT_UNSUPPORTED_LANGUAGE_TAG), convertRichTextTagToString(pTokeniser->eTag));
         break;

      /// [ALIGNMENT TAG] - Create a new paragraph, or alter the existing one if it's empty
      case RTC_ALIGNMENT_TAG:
         // [EMPTY PARAGRAPH] Alter alignment of existing paragraph
         if (pParagraph AND !getListItemCount(pParagraph->pItemList))
            pParagraph->eAlignment = (PARAGRAPH_ALIGNMENT)pTokeniser->eTag;
         else
         {  // [EXISTING PARAGRAPH] Create a new paragraph and append to message
            pParagraph = createRichParagraph((PARAGRAPH_ALIGNMENT)pTokeniser->eTag);
            appendObjectToList(pOutput->pParagraphList, (LPARAM)pParagraph);
         }
         break;

      /// [FORMATTING TAG] - Set the current item's attributes
      case RTC_FORMAT_TAG:
         pItem->eType       = RIT_TEXT;
         pItem->bBold      |= (pTokeniser->eTag == RTT_BOLD);
         pItem->bItalic    |= (pTokeniser->eTag == RTT_ITALIC);
         pItem->bUnderline |= (pTokeniser->eTag == RTT_UNDERLINE);
         break;

      /// [COLOUR TAG] - Set the current item's colour
      case RTC_COLOUR_TAG:
         pItem->eType    = RIT_TEXT;
         pItem->eColour  = identifyGameTextColourFromRichTextTag(pTokeniser->eTag);
         break;

      /// [BUTTON TAG] - Parse the tag into the current item directly
      case RTC_BUTTON_TAG:
         // Set type
         pItem->eType = RIT_BUTTON;

         // Parse the ID from the [select] tag
         if (eObjectType == RTT_LANGUAGE_MESSAGE AND translateLanguageMessageTag(pTokeniser, NULL, pItem, pErrorQueue))
         {
            // [CHECK] Ensure ID is not a duplicate
            if (!isRichEditButtonUnique(pOutput, pItem->szID))
               // [ERROR] "Duplicate button ID '%s' found in [select] tag"
               pError = generateDualError(HERE(IDS_RICHTEXT_BUTTON_ID_DUPLICATE), pItem->szID);
         }
         else
            // [ERROR] "Unsupported formatting tag [%s] found in toolip message: [article], [author], [title], [text] and [ranking] are not supported"
            pError = generateDualError(HERE(IDS_RICHTEXT_UNSUPPORTED_LANGUAGE_TAG), convertRichTextTagToString(pTokeniser->eTag));
         break;

      /// [IMAGE TAG] - Parse the tag into the current item directly
      case RTC_TOOLTIP_TAG:
         // Set type
         pItem->eType = RIT_IMAGE;

         // Parse the ID from the [image] tag
         if (eObjectType == RTT_RICH_TEXT)
            translateLanguageMessageTag(pTokeniser, NULL, pItem, pErrorQueue);
         else
            // [ERROR] "Unsupported formatting tag [%s] found in language message: [image] is not supported"
            pError = generateDualError(HERE(IDS_RICHTEXT_UNSUPPORTED_TOOLTIP_TAG), convertRichTextTagToString(pTokeniser->eTag));
         break;

      /// [PLAIN TEXT] -- Add text to the current item then add it to the paragraph
      case RTC_TEXT:
         // [SPECIAL CASE] -- [Author] or [Title] on the stack -- save text directly into the LanguageMessage
         if ((pTopItem = (RICHTEXT_STACK_ITEM*)topStackObject(pTagStack)) AND (pTopItem->eTag == RTT_AUTHOR OR pTopItem->eTag == RTT_TITLE))
         {
            // Prepare
            pLanguageMessage = (LANGUAGE_MESSAGE*)pOutput;

            // Store and convert author/title text
            switch (pTopItem->eTag)
            {
            case RTT_AUTHOR:  
               utilReplaceString(pLanguageMessage->szAuthor, pTokeniser->szText);
               performStringConversion(pLanguageMessage->szAuthor, lstrlen(pLanguageMessage->szAuthor), ST_INTERNAL, ST_DISPLAY);
               break;
            case RTT_TITLE:
               utilReplaceString(pLanguageMessage->szTitle, pTokeniser->szText);
               performStringConversion(pLanguageMessage->szTitle, lstrlen(pLanguageMessage->szTitle), ST_INTERNAL, ST_DISPLAY);
               break;
            }
         }
         // Save item text and add item to the paragraph.
         else
         {
            // Extract current text, convert to ST_DISPLAY and add to current Paragraph
            pItem->szText = utilDuplicateString(pTokeniser->szText, pTokeniser->iCount);
            appendRichTextItemToParagraph(pParagraph, pItem);

            // Create a new item using the 'current' attributes but with no text or ID.
            pItem = createRichItemText(pItem);
         }
         break;
      }
   }

   // Pop any remaining colour tags from the stack
   while ((pTopItem = (RICHTEXT_STACK_ITEM*)topStackObject(pTagStack)) AND pTopItem->eClass == RTC_COLOUR_TAG)
      destroyTopStackObject(pTagStack);

   // [CHECK] Any formatting tags still open?
   if (!pError AND getStackItemCount(pTagStack))
      // [ERROR] "There are unclosed formatting tags remaining in the RichText message"
      pError = generateDualWarning(HERE(IDS_RICHTEXT_OPEN_TAGS_REMAINING));
   
   /// [ERROR] Return the input plaintext
   if (bFormattingError = isError(pError))
   {
      // Delete partially parsed content
      deleteRichText(pOutput);
      pOutput = createRichText(eObjectType);

      // Replace with a single PlainText item
      findListObjectByIndex(pOutput->pParagraphList, 0, (LPARAM&)pParagraph);
      appendRichTextItemToParagraph(pParagraph, createRichItemPlainText(szSourceText, iTextLength));
   }

   // [CHECK] Ensure ErrorQueue is provided
   if (pError AND pErrorQueue)
   {
      attachTextToError(pError, szSourceText);     // Attach the plain-text to the error message
      pushErrorQueue(pErrorQueue, pError);
   }
   else if (pError)
      deleteErrorStack(pError);
   
   // Cleanup and return
   deleteRichItem(pItem);
   deleteRichTextTokeniser(pTokeniser);
   deleteStack(pTagStack);
   return !bFormattingError;
}


/// Function name  : generatePlainTextFromRichText
// Description     : Generate plain text from a LanguageMessage object and store it in a GameString
// 
// CONST LANGUAGE_MESSAGE*  pMessage    : [in]     Language message to convert
// GAME_STRING*             pGameString : [in/out] GameString to receive plain text. String type will be ST_INTERNAL
// 
// Return Value   : TRUE
// 
BearScriptAPI
BOOL  generatePlainTextFromLanguageMessage(CONST LANGUAGE_MESSAGE*  pMessage, GAME_STRING*  pGameString)
{
   RICHTEXT_ATTRIBUTES  oState;       // Current formatting state
   RICH_PARAGRAPH*       pParagraph;   // Paragraph iterator
   RICH_ITEM*            pItem;        // Item iterator
   BOOL                  bDefaultParagraph,  // TRUE if the formatting of the first paragraph is LEFT - ie. what's assumed if there is no initial alignment tag
                         bCustomColumns;     // TRUE if the message uses anything other than a single standard column
   TCHAR*                szOutput;     // New string
   UINT                  iOutputSize;  // Total length of output buffer, in characters
                         //iOutputUsed;  // Current length of output buffer, in characters

   // [CHECK] Validate parameters
   ASSERT(pMessage->eType == RTT_LANGUAGE_MESSAGE);

   // Prepare output
   iOutputSize = 4096;
   szOutput = utilCreateEmptyString(iOutputSize);

   // Setup generator
   utilZeroObject(&oState, RICHTEXT_ATTRIBUTES);
   oState.eAlignment = PA_LEFT;
   oState.eColour    = GTC_DEFAULT;

   /// [ARTICLE] Add this first, if required
   if (pMessage->bArticle)
      StringCchCopy(szOutput, iOutputSize, TEXT("[article/]"));

   /// [AUTHOR and TITLE TAGS] Add these second, if present
   if (pMessage->szAuthor)
      utilStringCchCatf(szOutput, iOutputSize, TEXT("[author]%s[/author]"), pMessage->szAuthor);
   if (pMessage->szTitle)
      utilStringCchCatf(szOutput, iOutputSize, TEXT("[title]%s[/title]"), pMessage->szTitle);
   
   /// [COLUMNS] Add only if any of the three custom column properties are used
   bCustomColumns = (pMessage->iColumnCount > 1 OR pMessage->bCustomWidth OR pMessage->bCustomSpacing);
   if (bCustomColumns)
   {
      StringCchCat(szOutput, iOutputSize, TEXT("[text"));
      // Number of columns
      if (pMessage->iColumnCount > 1)
         utilStringCchCatf(szOutput, iOutputSize, TEXT(" cols='%u'"), pMessage->iColumnCount);
      // Column widths
      if (pMessage->bCustomWidth)
         utilStringCchCatf(szOutput, iOutputSize, TEXT(" colwidth='%u'"), pMessage->iColumnWidth);
      // Column spacings
      if (pMessage->bCustomSpacing)
         utilStringCchCatf(szOutput, iOutputSize, TEXT(" colspacing='%u'"), pMessage->iColumnSpacing);
      // Close tag
      StringCchCat(szOutput, iOutputSize, TEXT("]"));
   }

   /// [RANKING] Add only if a custom rank is present
   if (pMessage->bCustomRank)
      utilStringCchCatf(szOutput, iOutputSize, TEXT("[ranking type='%s' title='%s'/]"), pMessage->eRankType == RT_FIGHT ? TEXT("fight") : TEXT("trade"), pMessage->szRankTitle);
   
   /// Iterate through paragraphs and their items
   for (LIST_ITEM*  pParagraphIterator = getListHead(pMessage->pParagraphList); pParagraph = extractListItemPointer(pParagraphIterator, RICH_PARAGRAPH); pParagraphIterator = pParagraphIterator->pNext)
   {
      // Determine if this paragraph is using the default formatting (left aligned initial paragraph)
      bDefaultParagraph = (!pParagraphIterator->pPrev AND pParagraph->eAlignment == PA_LEFT);

      // Open paragraph tag if necessary
      if (!bDefaultParagraph)
         utilStringCchCatf(szOutput, iOutputSize, TEXT("[%s]"), szAlignmentTags[pParagraph->eAlignment]);

      /// Iterate through items within the paragraph
      for (LIST_ITEM*  pItemIterator = getListHead(pParagraph->pItemList); pItem = extractListItemPointer(pItemIterator, RICH_ITEM); pItemIterator = pItemIterator->pNext)
      {
         // [TEXT]
         if (pItem->eType == RIT_TEXT)
         {
            // Check for changes in the formatting state and open/close the appropriate tags
            if (pItem->bBold != oState.bBold)
               StringCchCat(szOutput, iOutputSize, pItem->bBold > oState.bBold ? TEXT("[b]") : TEXT("[/b]"));
            if (pItem->bItalic != oState.bItalic)
               StringCchCat(szOutput, iOutputSize, pItem->bItalic > oState.bItalic ? TEXT("[i]") : TEXT("[/i]"));
            if (pItem->bUnderline != oState.bUnderline)
               StringCchCat(szOutput, iOutputSize, pItem->bUnderline > oState.bUnderline ? TEXT("[u]") : TEXT("[/u]"));
            
            // Update formatting state
            oState.bBold      = pItem->bBold;
            oState.bItalic    = pItem->bItalic;
            oState.bUnderline = pItem->bUnderline;

            // Check for colour changes
            if (oState.eColour != pItem->eColour)
            {
               // Close the current colour tag and open a new one.  If current colour is default, don't close
               //  the current tag, and if the new colour is default don't open a new one.
               if (oState.eColour != GTC_DEFAULT)
                  utilStringCchCatf(szOutput, iOutputSize, TEXT("[/%s]"), szColourTags[oState.eColour]);
               if (pItem->eColour != GTC_DEFAULT)
                  utilStringCchCatf(szOutput, iOutputSize, TEXT("[%s]"), szColourTags[pItem->eColour]);
               // Update state
               oState.eColour = pItem->eColour;
            }

            UINT  iOutputLength = lstrlen(szOutput);
            /// Insert item text
            StringCchCat(szOutput, 4096, pItem->szText);
            // Convert from DISPLAY to INTERNAL once in the output buffer
            performStringConversion(&szOutput[iOutputLength], 4096 - iOutputLength, ST_DISPLAY, ST_INTERNAL);
         }
         // [BUTTON] - Output as a single line
         else
            utilStringCchCatf(szOutput, iOutputSize, TEXT("[select value='%s']%s[/select]"), pItem->szID, pItem->szText);
      }

      // Close any remaining open formatting tags
      StringCchCat(szOutput, iOutputSize, oState.bBold      ? TEXT("[/b]") : TEXT(""));
      StringCchCat(szOutput, iOutputSize, oState.bItalic    ? TEXT("[/i]") : TEXT(""));
      StringCchCat(szOutput, iOutputSize, oState.bUnderline ? TEXT("[/u]") : TEXT(""));
      oState.bBold = oState.bItalic = oState.bUnderline = FALSE;

      // Close paragraph tag if necessary
      if (!bDefaultParagraph)
         utilStringCchCatf(szOutput, iOutputSize, TEXT("[/%s]"), szAlignmentTags[pParagraph->eAlignment]);
   }

   // Close [TEXT] tag if necessary
   if (bCustomColumns)
      StringCchCat(szOutput, iOutputSize, TEXT("[/text]"));

   // Update existing GameString
   utilDeleteString(pGameString->szText);
   pGameString->iCount = lstrlen(szOutput);

   pGameString->szText = utilDuplicateString(szOutput, pGameString->iCount);
   pGameString->eType  = ST_INTERNAL;

   // Cleanup
   utilDeleteString(szOutput);
   return TRUE;
}


/// Function name  : translateLanguageMessageTag
// Description     : Parse the properties of a Language message RichText tag - [TEXT], [ARTICLE], [RANKING] or [SELECT].
//                     [AUTHOR] and [TITLE] tags are also accepted but ignored.
// 
// CONST RICHTEXT_TOKENISER*  pTokeniser   : [in]               RichObject containing a [TEXT], [ARTICLE], [RANKING] or [SELECT] tag
// LANGUAGE_MESSAGE*          pMessage     : [in/out][optional] LanguageMessage into which the tag should be parsed
// RICH_ITEM*                 pButton      : [in/out][optional] RichText 'button' item into which the tag should be parsed
// ERROR_QUEUE*               pErrorQueue  : [in/out]           Error messages caused by translation, if any. May already contain errors
// 
// Return Value   : TRUE if parsed succesful, FALSE if there were errors
// 
BOOL   translateLanguageMessageTag(CONST RICHTEXT_TOKENISER*  pTokeniser, LANGUAGE_MESSAGE*  pMessage, RICH_ITEM*  pButton, ERROR_QUEUE*  pErrorQueue)
{
   RICHTEXT_TOKEN_STATE   eState;       // Processing state
   ERROR_STACK           *pError;       // Error, if any
   TCHAR                 *szToken,      // Current token
                         *szTokenEnd,   // End position of the current token
                         *szTextCopy;   // Copy of the current RichObject text
   
   // [CHECK] Ensure tag can have properties
   switch (pTokeniser->eTag)
   {
   // [DEFAULT] - Remaining tags don't have properties therefore they don't require parsing
   default:          
      return TRUE;

   case RTT_ARTICLE: pMessage->bArticle    = TRUE;  break;
   case RTT_RANKING: pMessage->bCustomRank = TRUE;  break;
   case RTT_SELECT:  
   case RTT_IMAGE:  
   case RTT_TEXT:    
      break;
   }

   // Prepare
   szTextCopy = utilDuplicateString(pTokeniser->szText, pTokeniser->iCount);
   eState     = RTS_PROPERTY_NAME;
   pError     = NULL;

   /// Consume tag name
   szToken = (szTextCopy + lstrlen(convertRichTextTagToString(pTokeniser->eTag)));

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
            pushErrorQueue(pErrorQueue, pError = generateDualError(HERE(IDS_RICHTEXT_UNKNOWN_TAG), szToken, convertRichTextTagToString(pTokeniser->eTag)));
         
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
      case RTS_RANK_TYPE:      
         pMessage->eRankType = (utilCompareString(szToken, "fight") ? RT_FIGHT : RT_TRADE);  
         break;
      case RTS_RANK_TITLE:     
         pMessage->szRankTitle = utilReplaceString(pMessage->szRankTitle, szToken);
         performStringConversion(pMessage->szRankTitle, lstrlen(pMessage->szRankTitle), ST_INTERNAL, ST_DISPLAY);
         break;

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

