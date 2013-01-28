//
// Game Strings.cpp  :  Functions for searching the collection of game strings
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                         MACROS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                  CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////

// Page ID version identifiers
CONST UINT  iPageVersionValues[4] = 
{ 
   0,          // GV_THREAT
   300000,     // GV_REUNION
   350000,     // GV_TERRAN_CONFLICT
   380000,     // GV_ALBION_PRELUDE
};

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createGameString
// Description     : Creates a new GameString from a string and it's properties
// 
// CONST TCHAR*       szText      : [in] Text for the new string
// CONST UINT         iStringID   : [in] ID of the new string
// CONST UINT         iPageID     : [in] ID of the page containing the new string
// CONST STRING_TYPE  eType       : [in] String type - internal, external or display
// 
// Return Value   : New GameString - you are responsible for destroying it
// 
BearScriptAPI
GAME_STRING*  createGameString(CONST TCHAR*  szText, CONST UINT  iStringID, CONST UINT  iPageID, CONST STRING_TYPE  eType)
{
   GAME_STRING*  pNewString = utilCreateEmptyObject(GAME_STRING);

   // Identify version from Page ID
   for (GAME_VERSION  eVersion = GV_ALBION_PRELUDE; eVersion >= GV_THREAT; eVersion = (GAME_VERSION)(eVersion - 1))
   {
      // [CHECK] Is version identification present?
      if (iPageID >= iPageVersionValues[eVersion])
      {
         // [FOUND] Set version and normalise page ID
         pNewString->eVersion = eVersion;
         pNewString->iPageID  = (iPageID - iPageVersionValues[eVersion]);

         // Set properties
         pNewString->iID    = iStringID;
         pNewString->eType  = eType;

         // Measure and copy string
         pNewString->iCount = lstrlen(szText);
         pNewString->szText = utilDuplicateString(szText, pNewString->iCount);
         break;
      }
   }

   // Return string
   return pNewString;
}


/// Function name  : createGameStringReference
// Description     : Creates data for a GameStringReference
// 
// CONST UINT  iPageParameterIndex   : [in] Index of parameter containing the Page ID
// CONST UINT  iStringParameterIndex : [in] Index of parameter containing the String ID
//
// Return Value   : New GameStringReference, you are responsible for destroying it
// 
GAME_STRING_REF*   createGameStringReference(CONST UINT  iPageParameterIndex, CONST UINT  iStringParameterIndex)
{
   GAME_STRING_REF*   pReference;       // Object being created

   // Create object
   pReference = utilCreateEmptyObject(GAME_STRING_REF);

   // Set properties
   pReference->iPageParameterIndex   = iPageParameterIndex;
   pReference->iStringParameterIndex = iStringParameterIndex;

   // Return object
   return pReference;
}


/// Function name  : createSubString
// Description     : Create a new SubString iteration object
// 
// CONST TCHAR*  szSourceText : [in] SourceText to iterate through
// 
// Return Value   : New SubString object, you are responsible for destroying it
// 
SUBSTRING*  createSubString(CONST TCHAR*  szSourceText)
{
   SUBSTRING*  pNewSubString;

   // Create empty sub-string object
   pNewSubString = utilCreateEmptyObject(SUBSTRING);

   // Set properties
   pNewSubString->szSource = szSourceText;
   pNewSubString->szMarker = szSourceText;

   // Return new
   return pNewSubString;
}


/// Function name  : createGamePage
// Description     : Create a new GamePage object
// 
// CONST UINT    iPageID       : [in] ID of the page
// CONST TCHAR*  szTitle       : [in] Title of the page
// CONST TCHAR*  szDescription : [in] Description of the page
// CONST BOOL    bVoiced       : [in] Whether the strings in the page are voiced within the game
// 
// Return Value   : New GamePage object, you are responsible for destroying it
// 
BearScriptAPI
GAME_PAGE*  createGamePage(CONST UINT  iPageID, CONST TCHAR*  szTitle, CONST TCHAR*  szDescription, CONST BOOL  bVoiced)
{
   GAME_PAGE*  pNewPage;

   // Create empty GamePage
   pNewPage = utilCreateEmptyObject(GAME_PAGE);

   // Identify version
   for (GAME_VERSION  eVersion = GV_ALBION_PRELUDE; eVersion >= GV_THREAT; eVersion = (GAME_VERSION)(eVersion - 1))
   {
      // [CHECK] Does version match?
      if (iPageID >= iPageVersionValues[eVersion])
      {
         /// [FOUND] Normalise Page ID 
         pNewPage->iPageID = (iPageID - iPageVersionValues[eVersion]);

         // Set properties
         pNewPage->szDescription = (szDescription ? utilDuplicateSimpleString(szDescription) : NULL);
         pNewPage->szTitle       = utilDuplicateSimpleString(szTitle);
         pNewPage->bVoice        = bVoiced;
         break;
      }
   }

   // Return new GamePage
   return pNewPage;
}


/// Function name  : duplicateGameString
// Description     : Duplicates a GameString
// 
// const GAME_STRING*  pString   : [in] Old string
// 
// Return Value   : New string
// 
BearScriptAPI
GAME_STRING*  duplicateGameString(const GAME_STRING*  pString)
{
   GAME_STRING*  pNewString = utilCreateEmptyObject(GAME_STRING);

   // Duplicate content
   utilCopyObject(pNewString, pString, GAME_STRING);
   pNewString->szText = utilDuplicateString(pString->szText, pString->iCount);
   
   // Return
   return pNewString;
}


/// Function name  : deleteGameString
// Description     : Deletes a GAME_STRING object
//
// GAME_STRING*  &pGameString : [in] Game string objec to destroy
// 
BearScriptAPI
VOID  deleteGameString(GAME_STRING*  &pGameString)
{
   // Delete text
   if (pGameString->szText)
      utilDeleteString(pGameString->szText);

   // Delete calling object
   utilDeleteObject(pGameString);
}


/// Function name  : deleteGamePage
// Description     : Delete a GamePage object
// 
// GAME_PAGE*  &pGamePage   : [in] GamePage object to delete
// 
BearScriptAPI
VOID  deleteGamePage(GAME_PAGE*  &pGamePage)
{
   // Delete title and description
   utilSafeDeleteStrings(pGamePage->szTitle, pGamePage->szDescription);

   // Delete calling object
   utilDeleteObject(pGamePage);
}

/// Function name  : deleteSubString
// Description     : Deletes a SubString object
// 
// SUBSTRING*  &pSubString   : [in] SubString object to delete
// 
VOID  deleteSubString(SUBSTRING*  &pSubString)
{
   // Delete calling object
   utilDeleteObject(pSubString);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                          HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : appendGameStringText
// Description     : Append formatted text to a GameString
// 
// GAME_STRING*  pGameString : [in/out] GameString to be modified
// CONST TCHAR*  szFormat    : [in]     Formatting string defining text to append
// ...           ...         : [in]     Parameters for above string
// 
VOID  appendGameStringText(GAME_STRING*  pGameString, CONST TCHAR*  szFormat, ...)
{
   va_list  pArguments;                // Variable arguments
   TCHAR*   szAppendedText,            // Formatted version of the input text
           *szReplacementString;       // Replacement for existing game string text - containing original text plus the new text

   // Prepare
   pArguments          = utilGetFirstVariableArgument(&szFormat);
   szAppendedText      = utilCreateEmptyString(32);
   szReplacementString = utilCreateEmptyString(pGameString->iCount + 32);

   // Format input text
   StringCchVPrintf(szAppendedText, 32, szFormat, pArguments);

   // Create replacement from existing text + input text
   utilStringCchCatf(szReplacementString, pGameString->iCount + 32, TEXT("%s %s"), pGameString->szText, szAppendedText);
   updateGameStringText(pGameString, szReplacementString);
   
   // Cleanup
   utilDeleteStrings(szAppendedText, szReplacementString);
}

/// Function name  : calculateOutputPageID
// Description     : Generates the original PageID using game version
// 
// const UINT          iPageID   : [in] PageID
// const GAME_VERSION  eVersion  : [in] Game version.  Must not be GV_UNKNOWN
// 
// Return Value   : Original PageID
// 
UINT  calculateOutputPageID(const UINT  iPageID, const GAME_VERSION  eVersion)
{
   return iPageID + iPageVersionValues[eVersion];
}

/// Function name  : findGameStringByID
// Description     : Find a game string from the game data strings tree by it's string and page IDs
//
// CONST UINT     iStringID : [in]  ID of the string to search for
// CONST UINT     iPageID   : [in]  ID of the page containing the string to search for
// GAME_STRING*  &pOutput   : [out] Resultant game string object, or NULL If not found
// 
// Return type : TRUE if found, FALSE If not
//
BearScriptAPI
BOOL    findGameStringByID(CONST UINT  iStringID, CONST UINT  iPageID, GAME_STRING*  &pOutput)
{
   // Query game strings tree
   return findObjectInAVLTreeByValue(getGameData()->pGameStringsByID, iPageID, iStringID, (LPARAM&)pOutput);
}


/// Function name  :  findGameStringInTreeByID
// Description     : Search for a GameString in a specified binary tree
// 
// CONST AVL_TREE*  pTree     : [in] Binary tree to search
// CONST UINT       iStringID : [in] ID of the string to search for
// CONST UINT       iPageID   : [in] ID of the page containing the string
// GAME_STRING*    &pOutput   : [out] Resultant GameString, if found
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BearScriptAPI
BOOL   findGameStringInTreeByID(CONST AVL_TREE*  pTree, CONST UINT  iStringID, CONST UINT  iPageID, GAME_STRING*  &pOutput)
{
   // Query tree
   return findObjectInAVLTreeByValue(pTree, iPageID, iStringID, (LPARAM&)pOutput);
}


/// Function name  : findGamePageInTreeByID
// Description     : Find a GamePage in a specified tree by it's ID
// 
// CONST BINARY_TREE*  pTree    : [in]  The GamePage tree to search
// CONST UINT          iPageID  : [in]  The ID of the Page to search for
// GAME_PAGE*         &pOutput  : [out] Resultant GamePage, if found
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BearScriptAPI
BOOL   findGamePageInTreeByID(CONST AVL_TREE*  pTree, CONST UINT  iPageID, GAME_PAGE*  &pOutput)
{
   // [CHECK] Ensure tree contains game pages
   //ASSERT(pTree->eType == AT_GAME_PAGE);

   // Query tree
   return findObjectInAVLTreeByValue(pTree, iPageID, NULL, (LPARAM&)pOutput);
}


/// Function name  : updateGameStringText
// Description     : Change the text of a GameString
// 
// GAME_STRING*  pGameString : [in/out] GameString to modify
// CONST TCHAR*  szNewText   : [in]     New GameString text
// 
BearScriptAPI
VOID  updateGameStringText(GAME_STRING*  pGameString, CONST TCHAR*  szNewText)
{
   // Delete existing string
   utilDeleteString(pGameString->szText);
   // Replace with new
   pGameString->iCount = lstrlen(szNewText);
   pGameString->szText = utilDuplicateString(szNewText, pGameString->iCount);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                          FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : findGameStringBySubString
// Description     : Lookup the GameString specified by the sub-string and copy it to an output buffer
// 
// CONST TCHAR*  szSubString    : [in]  Lookup SubString in the format {x,y}
// CONST UINT    iLength        : [in]  Length of substring, in characters
// CONST UINT    iDefaultPageID : [in]  PageID of the GameString containing the sub-string. This is used for
//                                        SubStrings that don't provide a page ID
// GAME_STRING* &pOutput        : [out] Resultant GameString if found, otherwise NULL
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BOOL  findGameStringBySubString(CONST TCHAR*  szSubString, CONST UINT  iLength, CONST UINT  iDefaultPageID, GAME_STRING*  &pOutput)
{
   UINT  iStringID,   // StringID component of the substring
         iPageID;     // PageID component of the substring   
         

   // [CHECK] Substring is enclosed in curly brackets
   ASSERT(szSubString[0] == '{' AND szSubString[iLength-1] == '}');

   /// Convert IDs to integers
   // [NO PAGE PROVIDED] Use the default page ID
   if (szSubString[1] == ',')
   {
      iStringID = utilConvertStringToInteger(&szSubString[2]);
      iPageID   = iDefaultPageID;
   }
   // [COMMA DELIMITED] Extract page first, then string
   else if (utilFindCharacter(szSubString, ','))
   {
      iPageID   = utilConvertStringToInteger(&szSubString[1]);
      iStringID = utilConvertStringToInteger(utilFindCharacter(szSubString, ',') + 1);
   }
   // [SINGLE VALUE] Assume it's a string ID and use the default page ID
   else
   {
      iStringID = utilConvertStringToInteger(&szSubString[1]);
      iPageID   = iDefaultPageID;
   }

   // [CHECK] Normalise Page ID
   for (GAME_VERSION  eVersion = GV_ALBION_PRELUDE; eVersion > GV_THREAT; eVersion = (GAME_VERSION)(eVersion - 1))
      // [CHECK] Is version identification present?
      if (iPageID >= iPageVersionValues[eVersion])
      {
         iPageID -= iPageVersionValues[eVersion];
         break;
      }

   /// Lookup substring and return result
   return findGameStringByID(iStringID, iPageID, pOutput);
}


/// Function name  : findNextNonEscapedCharacter
// Description     : Find the next special character in a string that is not preceeded by a backslash.
// 
// CONST TCHAR*  szString    : [in] String to search
// CONST TCHAR   chCharacter : [in] Any character, but usually an opening or closing square bracket
// 
// Return Value   : String where character begins, or NULL if no such character was found
// 
CONST TCHAR*  findNextNonEscapedCharacter(CONST TCHAR*  szString, CONST TCHAR  chCharacter)
{
   CONST TCHAR*  szOutput;

   // Search through all the matching characters in the string
   for (szOutput = utilFindCharacter(szString, chCharacter); szOutput; szOutput = utilFindCharacter(szOutput+1, chCharacter))
   {
      // If resultant character is the first in the string or it isn't preceeded by a backslash - return it.
      if (szOutput - szString == 0 OR szOutput[-1] != '\\')
         break;
   }

   return szOutput;
}

/// Function name  : findNextNonEscapedCharacters
// Description     : Search for the next character occurence of any character in a specified string that is
//                     not preceeded by a backslash.
// 
// CONST TCHAR*  szString      : [in] String to search
// CONST TCHAR*  szCharacters  : [in] String containing the characters to search for
// 
// Return Value   : Position if found, otherwise NULL
// 
CONST TCHAR*  findNextNonEscapedCharacters(CONST TCHAR*  szString, CONST TCHAR*  szCharacters)
{
   CONST TCHAR*  szOutput;

   // Search through all the matching characters in the string
   for (szOutput = StrPBrk(szString, szCharacters); szOutput; szOutput = StrPBrk(szOutput+1, szCharacters))
   {
      // [CHECK] Is first character a match?  Is a subsequent character preceeded by a backslash?
      if (szOutput == szString OR szOutput[-1] != '\\')
         /// [FOUND] Return current position
         return szOutput;  
   }

   /// [NOT FOUND] Return NULL
   return NULL;
}


/// Function name  : findNextSubString
// Description     : Searches for the next SubString in a GameString
// 
// CONST GAME_STRING*  pGameString    : [in]     GameString currently being resolved
// SUBSTRING*          pSubString     : [in/out] Contains the resolved SubString on return, if successful
// STACK*              pHistoryStack  : [in/out] Represents the 'chain' of GameStrings being resolved
// ERROR_QUEUE*        pErrorQueue    : [in/out] ErrorQueue, may already contain errors
// 
// Return Value   : TRUE if substring was found, FALSE if there were no more substrings
// 
BOOL   findNextSubString(CONST GAME_STRING*  pGameString, SUBSTRING*  pSubString, UINT&  iMissingCount, STACK*  pHistoryStack, ERROR_QUEUE*  pErrorQueue)
{
   CONST TCHAR*  szEndMarker;    // Marks the position of a matching closing bracket
   GAME_STRING*  pLookupString;  // Sub-string lookup string
   TCHAR*        szPreview;      // GameString preview text, for error reporting

   // Prepare
   pSubString->szText[0] = NULL;

   // [CHECK] Is there no more string?
   if (!pSubString->szMarker OR !pSubString->szMarker[0])
      return FALSE;

   // Determine sub-string type from first character
   switch (pSubString->szMarker[0])
   {
   /// [TEXT or NORMAL BRACKET] - Indicates TEXT or COMMENT substring
   default: 
      // [COMMENT] Search for a matching closing bracket (or EOF)
      if (pSubString->szMarker[0] == '(')
      {
         // Calculate the size. (Include closing bracket, if any)
         if (szEndMarker = findNextNonEscapedCharacter(pSubString->szMarker, ')'))
            pSubString->iCount = (szEndMarker - pSubString->szMarker) + 1;
         // Set type
         pSubString->eType = SST_COMMENT;
      }
      // [TEXT] Search for any opening brackets (or EOF)
      else
      {
         // Calculate the size.  (Exclude opening bracket)
         if (szEndMarker = findNextNonEscapedCharacters(pSubString->szMarker, TEXT("({")))
            pSubString->iCount = (szEndMarker - pSubString->szMarker);
         // Set type
         pSubString->eType = SST_TEXT;
      }

      // Determine the sub-string length (if no matching bracket was found)
      if (szEndMarker == NULL)
         pSubString->iCount = lstrlen(pSubString->szMarker);

      /// Extract string to the output buffer and update the marker
      StringCchCopyN(pSubString->szText, MAX_STRING, pSubString->szMarker, pSubString->iCount);
      pSubString->szMarker += pSubString->iCount;
      break;

   /// [CURLY BRACKET] - Indicates LOOKUP or MISSION
   case '{':
      // Identify the *matching* closing curly bracket (sub-strings may be nested)
      if (szEndMarker = findSubStringEndMarker(pSubString->szMarker))
      {
         // Extend 'end' marker to cover the closing curly bracket
         szEndMarker += 1;

         // [CHECK] Is this a lookup or a MD variable?
         if (isSubStringLookup(pSubString->szMarker, szEndMarker - pSubString->szMarker))
         {
            /// [LOOKUP] Lookup the GameString 
            if (findGameStringBySubString(pSubString->szMarker, szEndMarker - pSubString->szMarker, pGameString->iPageID, pLookupString))
            {
               // [CHECK] Are substrings within GameString resolved?
               if (pLookupString->bResolved)
               {
                  // [RESOLVED] Insert verbatim
                  StringCchCopy(pSubString->szText, MAX_STRING, pLookupString->szText);
                  pSubString->iCount = pLookupString->iCount;
               }
               else
               {
                  // [UNRESOLVED] Recursively resolve sub-strings
                  performGameStringResolution(pLookupString, pSubString->szText, MAX_STRING, pHistoryStack, pErrorQueue);
                  pSubString->iCount = lstrlen(pSubString->szText);
               }
            }
            else
            {  
               /// [NOT FOUND] GameString lookup not found (return the sub-string tag)
               pSubString->iCount = (szEndMarker - pSubString->szMarker);
               StringCchCopyN(pSubString->szText, MAX_STRING, pSubString->szMarker, pSubString->iCount);

               // Report number of missing strings
               iMissingCount++;

               // [CHECK] Is individual reporting enabled?
               if (getAppPreferences()->bReportMissingSubStrings)
               {
                  // [WARNING] "Missing sub-string %s detected in page %u, string %u : '%s'"
                  pushErrorQueue(pErrorQueue, generateDualWarning(HERE(IDS_GAME_STRING_SUBSTRING_NOT_FOUND), pSubString->szText, pGameString->iPageID, pGameString->iID, szPreview = generateGameStringPreview(pGameString, 96)));
                  utilDeleteString(szPreview);
               }
            }
            // Set the type
            pSubString->eType  = SST_LOOKUP;
         }
         else
         {
            /// [MISSION] Copy verbatim
            pSubString->iCount = (szEndMarker - pSubString->szMarker);
            StringCchCopyN(pSubString->szText, MAX_STRING, pSubString->szMarker, pSubString->iCount);
            // Set the type
            pSubString->eType = SST_MISSION;
         }
         // Position the marker beyond the closing bracket
         pSubString->szMarker = szEndMarker;
      }
      else
      {
         /// [ERROR] No matching curly bracket
         //StringCchCopy(pSubString->szText, MAX_STRING, TEXT("{Error, no matching bracket}"));
         //pSubString->iCount = lstrlen(pSubString->szText);

         //// [ERROR] "Unclosed sub-string marker detected in page %u, string %u : '%s'"
         //pushErrorQueue(pErrorQueue, generateDualWarning(HERE(IDS_GAME_STRING_SUBSTRING_CORRUPT), pGameString->iPageID, pGameString->iID, szPreview = generateGameStringPreview(pGameString, 96)));
         //utilDeleteString(szPreview);

         /// [VALIDATION_FIX] Copy string verbatim. They're often used as characters in the MARS scripts without escapes.
         StringCchCopy(pSubString->szText, MAX_STRING, pSubString->szMarker);
         pSubString->iCount = lstrlen(pSubString->szText);

         // Return 'TEXT' and ensure further calls return FALSE
         pSubString->eType    = SST_TEXT;
         pSubString->szMarker = NULL;
      }
      break;
   }

   // [FOUND] Return TRUE
   return TRUE;
}


/// Function name  : findSubStringEndMarker
// Description     : Search the matching end bracket to a sub-string, which may contain nested substrings
// 
// CONST TCHAR*  szInput : [in] Sub-string, including opening curly bracket
// 
// Return Value   : Position of the closing curly bracket, or NULL if no matching curly bracket was found
// 
CONST TCHAR*  findSubStringEndMarker(CONST TCHAR*  szInput)
{
   CONST TCHAR*  szIterator;     // Iterator
   INT           iOpenBrackets;  // Number of currently open brackets

   // Prepare
   iOpenBrackets = 0;

   // [CHECK] First character must be an opening curly bracket
   ASSERT(szInput[0] == '{');

   // Iterate through input characters
   for (szIterator = szInput; szIterator[0]; szIterator++)
   {
      switch (szIterator[0])
      {
      // [OPEN] Increment counter
      case '{':  
         iOpenBrackets++;  
         break;

      // [CLOSE] Decrement counter
      case '}':  
         if (--iOpenBrackets == 0)
            // [FOUND] Return current character
            return szIterator;
      }
   }

   // [NOT FOUND] Return NULL
   return NULL;
}


/// Function name  : generateGameStringPreview
// Description     : Generates a preview of a GameString
// 
// CONST GAME_STRING*  pGameString    : [in] GameString
// CONST UINT          iPreviewLength : [in] Length of preview, in characters
// 
// Return Value   : New string, you are responsible for destroying it
// 
TCHAR*   generateGameStringPreview(CONST GAME_STRING*  pGameString, CONST UINT  iPreviewLength)
{
   TCHAR*  szPreview;

   // Extract a portion of the GameString text
   szPreview = utilDuplicateSubString(pGameString->szText, pGameString->iCount, 0, iPreviewLength);
   
   // [TRUNCATED] Add ellipsis
   if (lstrlen(szPreview) >= (INT)(iPreviewLength - 1))
      szPreview[iPreviewLength - 1] = L'…';
         
   // Return preview
   return szPreview;
}


/// Function name  : generateInternalGameStringPreview
// Description     : Generates a preview of a GameString
// 
// CONST GAME_STRING*  pGameString    : [in] GameString
// CONST UINT          iPreviewLength : [in] Length of preview, in characters
// 
// Return Value   : New string, you are responsible for destroying it
// 
TCHAR*   generateInternalGameStringPreview(CONST GAME_STRING*  pGameString, CONST UINT  iPreviewLength)
{
   TCHAR  *szPreview,      // Preview
          *szConverted;    // Preview of type ST_INTERNAL

   // Extract a portion of the GameString text
   szPreview = generateGameStringPreview(pGameString, iPreviewLength);

   // Convert string to internal
   if (generateConvertedString(szPreview, SPC_LANGUAGE_EXTERNAL_TO_INTERNAL, szConverted))
      utilDeleteString(szPreview);
   
   // Return original/converted string
   return utilEither(szPreview, szConverted);
}



/// Function name  : isSubStringLookup
// Description     : Determine whether a sub-string enclosed in curly braces is a mission director sub-string
//                     or a lookup substring constisting of a string ID and (optional) page ID
// 
// CONST TCHAR*  szSubString : [in] SubString to search
// CONST UINT    iLength     : [in] Length of the substring in characters
// 
// Return Value   : TRUE or FALSE
// 
BOOL  isSubStringLookup(CONST TCHAR*  szSubString, CONST UINT  iLength)
{
   BOOL  bHasLetters,   // Stirng has letters
         bHasComma;     // String has a comma

   // Prepare
   bHasLetters = bHasComma = FALSE;

   // Examine substring characters
   for (UINT i = 1; i < iLength AND !bHasLetters; i++)
   {
      switch (szSubString[i])
      {
      // [COMMA] Indicates a lookup or MD variable
      case ',': 
         bHasComma = TRUE;
         continue;

      // [CLOSING BRACKET, SPACE, NUMBERS] Ignore
      case '}':     
      case ' ':
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
         continue;

      // [LETTERS or RECURSIVE OPENING BRACKETS] Indicates an MD variable
      case '{': 
      default:
         bHasLetters = TRUE;
         break;
      }
   }

   // Return TRUE if string contains no letters and a comma
   return !bHasLetters AND bHasComma;
}


/// Function name  : performGameStringResolution
// Description     : Expands sub-strings within the source-text and strips out any comments
// 
// CONST GAME_STRING*  pGameString    : [in]     GameString currently being resolved
// TCHAR*              szOutput       : [in/out] Output buffer 
// CONST UINT          iOutputLength  : [in]     Length of output buffer, in characters
// STACK*              pHistoryStack  : [in/out] Represents the 'chain' of GameStrings being resolved
// ERROR_QUEUE*        pErrorQueue    : [in/out] ErrorQueue, may already contain errors
// 
// Return Value : Number of sub-strings that could not be resolved
//
UINT  performGameStringResolution(CONST GAME_STRING*  pGameString, TCHAR*  szOutput, CONST UINT  iOutputLength, STACK*  pHistoryStack, ERROR_QUEUE*  pErrorQueue)
{
   SUBSTRING*    pSubString;           // SubString iterator
   GAME_STRING*  pSourceString;        // First game string in the list
   TCHAR*        szPreview;            // Preview text for error reporting
   UINT          iMissingSubstrings;

   // Prepare
   iMissingSubstrings = 0;

   /// [CHECK] Ensure sub-string is not self-referencial
   if (isValueInList(pHistoryStack, (LPARAM)pGameString))
   {
      // Lookup initial GameString
      findListObjectByIndex(pHistoryStack, 0, (LPARAM&)pSourceString);

      // [ERROR] "Circular sub-string references detected in page %u, string %u : '%s'"
      pushErrorQueue(pErrorQueue, generateDualError(HERE(IDS_GAME_STRING_SUBSTRING_RECURSIVE), pSourceString->iPageID, pSourceString->iID, szPreview = generateGameStringPreview(pGameString, 96)));
      utilDeleteString(szPreview);
      return FALSE;
   }
   
   // Prepare
   pSubString = createSubString(pGameString->szText);

   /// [STACK] Add GameString to history stack
   pushObjectOntoStack(pHistoryStack, (LPARAM)pGameString);

   /// Iterate through sub-strings
   while (findNextSubString(pGameString, pSubString, iMissingSubstrings, pHistoryStack, pErrorQueue))
   {
      // Examine type
      switch (pSubString->eType)
      {
      /// [MISSION, TEXT] Append to output
      case SST_MISSION:
      case SST_LOOKUP:
      case SST_TEXT:
         StringCchCat(szOutput, MAX_STRING, pSubString->szText);  
         break;

      /// [COMMENTS] Ignore
      case SST_COMMENT:
         // [SPECIAL CASE] Allow 'opening bracket' operator
         if (utilCompareString(pSubString->szText, "("))
            StringCchCat(szOutput, MAX_STRING, pSubString->szText);  
         break;
      }
   }
   
   // [STACK] Remove GameString from processing stack
   popObjectFromStack(pHistoryStack);

   // Cleanup
   deleteSubString(pSubString);
   return iMissingSubstrings;
}


/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                        TREE OPERATIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : treeprocConvertGameStringToInternal
// Description     : Convert a GameStrings binary tree node from external to internal
// 
// BINARY_TREE_NODE*            pNode            : [in] Current tree node
// BINARY_TREE_OPERATION_DATA*  pOperationData   : [in] xFirstInput : Conversion flags
// 
VOID    treeprocConvertGameStringToInternal(AVL_TREE_NODE*  pNode, AVL_TREE_OPERATION*  pOperationData)
{
   GAME_STRING*  pGameString;    // Convenience pointer
   TCHAR*        szConverted;    // Convert string, if any

   // Prepare
   pGameString = extractPointerFromTreeNode(pNode, GAME_STRING);

   // [CHECK] Is string already 'Internal'?
   if (pGameString->eType != ST_INTERNAL)
   {
      /// Create converted string
      if (generateConvertedString(pGameString->szText, pOperationData->xFirstInput, szConverted))
      {
         /// Replace text buffer
         utilDeleteString(pGameString->szText);
         pGameString->szText = szConverted;

         // Re-Calculate length
         pGameString->iCount = lstrlen(pGameString->szText);
      }

      // Update string type
      pGameString->eType = ST_INTERNAL;
   }
}






