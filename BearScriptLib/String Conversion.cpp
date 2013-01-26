//
// String Conversion.cpp : For converting the special characters in strings
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////

// Defines the various special phrases available
//
CONST TCHAR*    szConversionPhrases[31] = { NULL,                                          // SCT_TEXT

                               TEXT("\n"), TEXT("\\n"),   TEXT("'"),  TEXT("`"),           // SCT_NEWLINE,      SCT_EXPANDED_NEWLINE,      SCT_APOSTROPHE,      SCT_EXPANDED_APOSTROPHE,
                               TEXT("&"),  TEXT("&amp;"), TEXT(">"),  TEXT("&gt;"),        // SCT_AMPERSAND,    SCT_EXPANDED_AMPERSAND,    SCT_GREATER_THAN,    SCT_EXPANDED_GREATER_THAN,
                               TEXT("<"),  TEXT("&lt;"),  TEXT("\""), TEXT("&quot;"),      // SCT_LESS_THAN,    SCT_EXPANDED_LESS_THAN,    SCT_QUOTES,          SCT_EXPANDED_QUOTES,
                               TEXT("%"),  TEXT("¶"),     TEXT("+"),  TEXT("\\053"),       // SCT_PERCENT,      SCT_EXPANDED_PERCENT,      SCT_ADDITION,        SCT_EXPANDED_ADDITION,

                               TEXT("["),  TEXT("\\["),   TEXT("]"),  TEXT("\\]"),         // SCT_SQUARE_BRACKET_OPEN,   SCT_EXPANDED_SQUARE_BRACKET_OPEN,      SCT_SQUARE_BRACKET_CLOSE,  SCT_EXPANDED_SQUARE_BRACKET_CLOSE,
                               TEXT("{"),  TEXT("\\{"),   TEXT("}"),  TEXT("\\}"),         // SCT_CURLY_BRACKET_OPEN,    SCT_EXPANDED_CURLY_BRACKET_OPEN,       SCT_CURLY_BRACKET_CLOSE,   SCT_EXPANDED_CURLY_BRACKET_CLOSE,
                               TEXT("("),  TEXT("\\("),   TEXT(")"),  TEXT("\\)"),         // SCT_BRACKET_OPEN,          SCT_EXPANDED_BRACKET_OPEN,             SCT_BRACKET_CLOSE,         SCT_EXPANDED_BRACKET_CLOSE
                               TEXT("\t"), TEXT("\\t")   };


// Identifies the conversion flag that needs to be present to convert the correlated special phrase in 'szConversionPhrases'
//
CONST STRING_CONVERSION_FLAG  iConversionFlags[31] = { (STRING_CONVERSION_FLAG)NULL,      // placeholder for SCT_TEXT so that the array indicies align

                               SCF_EXPAND_NEWLINE,        SCF_CONDENSE_NEWLINE,        SCF_EXPAND_APOSTROPHE,     SCF_CONDENSE_APOSTROPHE,
                               SCF_EXPAND_AMPERSAND,      SCF_CONDENSE_AMPERSAND,      SCF_EXPAND_GREATER_THAN,   SCF_CONDENSE_GREATER_THAN,
                               SCF_EXPAND_LESS_THAN,      SCF_CONDENSE_LESS_THAN,      SCF_EXPAND_QUOTES,         SCF_CONDENSE_QUOTES,
                               SCF_EXPAND_PERCENT,        SCF_CONDENSE_PERCENT,        SCF_EXPAND_ADDITION,       SCF_CONDENSE_ADDITION,

                               // These flags are repeated because EXPAND and CONDENSE apply to opening and closing brackets
                               SCF_EXPAND_SQUARE_BRACKET, SCF_CONDENSE_SQUARE_BRACKET, SCF_EXPAND_SQUARE_BRACKET, SCF_CONDENSE_SQUARE_BRACKET,
                               SCF_EXPAND_CURLY_BRACKET,  SCF_CONDENSE_CURLY_BRACKET,  SCF_EXPAND_CURLY_BRACKET,  SCF_CONDENSE_CURLY_BRACKET, 
                               SCF_EXPAND_BRACKET,        SCF_CONDENSE_BRACKET,        SCF_EXPAND_BRACKET,        SCF_CONDENSE_BRACKET,

                               SCF_EXPAND_TAB,            SCF_CONDENSE_TAB    };

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createStringConverter
// Description     : Creates a string conversion object, for replacing sensitive characters with various equivilents
// 
// CONST TCHAR*  szString : [in] String to convert
// 
// Return Value   : New string conversion object, you are responsible for destroying it
// 
STRING_CONVERTER*  createStringConverter(CONST TCHAR*  szString)
{
   STRING_CONVERTER*  pStringConverter;

   // Create object
   pStringConverter = utilCreateEmptyObject(STRING_CONVERTER);

   // Set properties
   pStringConverter->szSource   = szString;
   pStringConverter->szPosition = szString;

   // Return new object
   return pStringConverter;
}


/// Function name  : deleteStringConverter
// Description     : Destroys a string conversion object
// 
// STRING_CONVERTER*  &pStringConverter : [in] String conversion object to destroy
// 
VOID  deleteStringConverter(STRING_CONVERTER*  &pStringConverter)
{
   // Delete calling object
   utilDeleteObject(pStringConverter);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : hasSpecialPhrases
// Description     : Checks a string for the presence of special phrases
// 
// CONST TCHAR*  szString : [in] String to test
// 
// Return Value   : TRUE or FALSE
// 
BOOL     hasSpecialPhrases(CONST TCHAR*  szString)
{
   CONST TCHAR*  szIterator;   // Input string iterator
   BOOL          bResult;      // Operation result

   // Prepare
   bResult = FALSE;

   /// Check each character for special phrases
   for (szIterator = szString; szIterator[0]; szIterator++)
      if (identifySpecialPhrase(szIterator) != SCT_TEXT)
      {
         bResult = TRUE;
         break;
      }

   // Return result
   return bResult;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : findNextSpecialPhrase
// Description     : Iterates through a source string and returns the next special phrase
// 
// STRING_CONVERTER*  pStringConverter : [in/out] Conversion object containing the source string on input and the current phrase on output
// 
// Return Value   : TRUE if any phrase is found, FALSE if no more phrases exist
// 
BOOL  findNextSpecialPhrase(STRING_CONVERTER*  pStringConverter)
{
   CONST TCHAR*  szPhraseStart;

   // [CHECK] Return FALSE when no more input string is available
   if (pStringConverter->szPosition[0] == NULL)
      return FALSE;

   // Prepare
   szPhraseStart = pStringConverter->szPosition;

   /// Identify the current phrase
   pStringConverter->eType = identifySpecialPhrase(szPhraseStart);

   /// [SPECIAL PHRASE] Copy to phrase buffer
   if (pStringConverter->eType != SCT_TEXT)
   {
      // Copy phrase text
      StringCchCopy(pStringConverter->szText, MAX_STRING, szConversionPhrases[pStringConverter->eType]);
      // Calculate phrase length
      pStringConverter->iLength = lstrlen(pStringConverter->szText);
      
      // Move iterator to the start of the next phrase
      pStringConverter->szPosition += pStringConverter->iLength;
   }

   /// [TEXT] Identify where the text ends then copy to phrase buffer
   else for (pStringConverter->szPosition = szPhraseStart + 1; ; pStringConverter->szPosition++)
      // Check for the end of the source string or current phrase
      if (pStringConverter->szPosition[0] == NULL OR identifySpecialPhrase(pStringConverter->szPosition) != SCT_TEXT)
      {
         // Calculate length and extract phrase
         pStringConverter->iLength = (pStringConverter->szPosition - szPhraseStart);
         StringCchCopyN(pStringConverter->szText, MAX_STRING, szPhraseStart, pStringConverter->iLength);
         break;
      }
      
   // Return TRUE
   return TRUE;
}


/// Function name  : generateConvertedString
// Description     : Examines a string for specific phrases and allocates a new string with alternatives in their place
// 
// CONST TCHAR*  szInput  : [in]  String to examine
// CONST UINT    iFlags   : [in]  Combination of STRING_CONVERSION_FLAGS indicating which phrases to replace
// TCHAR*       &szOutput : [out] New string if a conversion was performed, otherwise NULL
// 
// Return Value   : TRUE if converted, FALSE if none of the desired phrases were found
// 
BearScriptAPI
BOOL   generateConvertedString(CONST TCHAR*  szInput, CONST UINT  iFlags, TCHAR*  &szOutput)
{
   STRING_CONVERTER*        pCurrentPhrase;       // string conversion object
   SPECIAL_CHARACTER_TYPE   eReplacementPhrase;   // Alternate special phrase for the phrase being replaced
   
   // Prepare
   pCurrentPhrase = createStringConverter(szInput);
   szOutput       = NULL;

   /// Search for special phrases in input string
   while (findNextSpecialPhrase(pCurrentPhrase))
   {
      // Examine current phrase type
      switch (pCurrentPhrase->eType)
      {
      /// [SPECIAL PHRASE] Replace with it's alternative if the conversion flag for this type of phrase has been specified
      default:
         // Determine whether expand/condense flag was specified for this phrase type
         if (iFlags INCLUDES iConversionFlags[pCurrentPhrase->eType])
         {
            // Determine replacement phrase                    NB: CONDENSED phrase IDs are ODD, while EXPANDED phrase IDs are EVEN
            if ((INT)pCurrentPhrase->eType % 2 == 1)
               // [CONDENSED] Replace with Expanded
               eReplacementPhrase = (SPECIAL_CHARACTER_TYPE)(pCurrentPhrase->eType + 1);      // Map Condensed->Expanded: Increase ID by 1
            else  
               // [EXPANDED] Replace with Condensed
               eReplacementPhrase = (SPECIAL_CHARACTER_TYPE)(pCurrentPhrase->eType - 1);      // Map Expanded->Condensed: Decrease ID by 1

            // Output expanded/condensed alternative
            StringCchCat(pCurrentPhrase->szOutput, MAX_STRING, szConversionPhrases[eReplacementPhrase]);
            pCurrentPhrase->iOutputLength += lstrlen(szConversionPhrases[eReplacementPhrase]);

            // Set the 'any conversion performed' flag
            pCurrentPhrase->bConversionPerformed = TRUE;
            break;
         }
         // else Fall through...

      /// [TEXT] Add current phrase to output 
      case SCT_TEXT:
         StringCchCat(pCurrentPhrase->szOutput, MAX_STRING, pCurrentPhrase->szText);
         pCurrentPhrase->iOutputLength += pCurrentPhrase->iLength;
         break;
      }
   }

   // Set output only if necessary
   if (pCurrentPhrase->bConversionPerformed)
      szOutput = utilDuplicateString(pCurrentPhrase->szOutput, pCurrentPhrase->iOutputLength);

   // Cleanup and return TRUE if conversion was performed
   deleteStringConverter(pCurrentPhrase);
   return (szOutput != NULL);
}


/// Function name  : identifySpecialPhrase
// Description     : Identifies whether a string starts with a special phrase and if so, which one
// 
// CONST TCHAR*  szString   : [in] String to examine
// 
// Return Value   : Special phrase ID, or SCT_TEXT if it doesn't start with a special phrase
// 
SPECIAL_CHARACTER_TYPE  identifySpecialPhrase(CONST TCHAR*  szString)
{
   SPECIAL_CHARACTER_TYPE   eOutput;      // Identification result

   // Examine first letter
   switch (szString[0])
   {
   /// [TEXT]
   default:    eOutput = SCT_TEXT;                  break;

   /// [SINGLE LETTER SPECIAL PHRASE]
   case '(':   eOutput = SCT_BRACKET_OPEN;          break;
   case ')':   eOutput = SCT_BRACKET_CLOSE;         break;
   case '{':   eOutput = SCT_CURLY_BRACKET_OPEN;    break;
   case '}':   eOutput = SCT_CURLY_BRACKET_CLOSE;   break;
   case '[':   eOutput = SCT_SQUARE_BRACKET_OPEN;   break;
   case ']':   eOutput = SCT_SQUARE_BRACKET_CLOSE;  break;

   case '%':   eOutput = SCT_PERCENT;               break;
   case '¶':   eOutput = SCT_EXPANDED_PERCENT;      break;
   case '<':   eOutput = SCT_LESS_THAN;             break;
   case '>':   eOutput = SCT_GREATER_THAN;          break;
   case '\'':  eOutput = SCT_APOSTROPHE;            break;
   case '`':   eOutput = SCT_EXPANDED_APOSTROPHE;   break;
   case '"':   eOutput = SCT_QUOTES;                break;
   case '+':   eOutput = SCT_ADDITION;              break;
   case '\n':  eOutput = SCT_NEWLINE;               break;
   case '\t':  eOutput = SCT_TAB;                   break;

   // [INDICATES POSSIBLE SPECIAL PHRASE]
   case '&':
      /// [XML SAFETY PHRASES]
      if (utilCompareStringN(szString, "&lt;", 4))
         eOutput = SCT_EXPANDED_LESS_THAN;
      else if (utilCompareStringN(szString, "&gt;", 4))
         eOutput = SCT_EXPANDED_GREATER_THAN;
      else if (utilCompareStringN(szString, "&amp;", 5))
         eOutput = SCT_EXPANDED_AMPERSAND;
      else if (utilCompareStringN(szString, "&quot;", 6))
         eOutput = SCT_EXPANDED_QUOTES;
      else
         /// [SINGLE AMPERSAND]
         eOutput = SCT_AMPERSAND;
      break;

   // [INDICATES POSSIBLE SPECIAL PHRASE]
   case '\\':
      switch (szString[1])
      {
      /// [EXPANDED NEWLINE OR BRACKET]
      case 'n':  eOutput = SCT_EXPANDED_NEWLINE;               break;
      case 't':  eOutput = SCT_EXPANDED_TAB;                   break;
      case '(':  eOutput = SCT_EXPANDED_BRACKET_OPEN;          break;  
      case ')':  eOutput = SCT_EXPANDED_BRACKET_CLOSE;         break;  
      case '[':  eOutput = SCT_EXPANDED_SQUARE_BRACKET_OPEN;   break;  
      case ']':  eOutput = SCT_EXPANDED_SQUARE_BRACKET_CLOSE;  break;  
      case '{':  eOutput = SCT_EXPANDED_CURLY_BRACKET_OPEN;    break;  
      case '}':  eOutput = SCT_EXPANDED_CURLY_BRACKET_CLOSE;   break;
      
      // [SINGLE BACKSLASH]
      default:   eOutput = SCT_TEXT;                           break;
      }

      /// [ADDITION]
      if (utilCompareStringN(szString, "\\053", 4))
         eOutput = SCT_EXPANDED_ADDITION;
      break;
   }

   // Return result
   return eOutput;
}



