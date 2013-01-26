//
// GameStrings (old).cpp : Old string conversion stuff
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        MACROS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Name: findPhraseInString
//  Description: Convenience macro for searching a specified position in a string for a substring
// 
// CONST TCHAR*  szPhrase         : [in] Phrase to search for
// CONST UINT    nPhraseLength    : [in] Length of phrase to search for
// CONST TCHAR*  szString         : [in] String to search
// CONST UINT    nPhraseStartChar : [in] Character index of the position to search at
// 
// Return Type : TRUE if position contains the specified string, FALSE otherwise
//
#define    findPhraseInString(szPhrase,nPhraseLength,szString,nPhraseStartChar) (StrCmpN(&szString[nPhraseStartChar], szPhrase, nPhraseLength) == 0)

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

/// Function name  : performCharacterReplacement
// Description     : Alters the character composition of a string - replacing certain characters or phrases with
//                    other characters or phrases.  The game uses many symbols which cannot be safely used within
//                    an XML file, and conversely there are characters in the XML files which cannot be safely used
//                    within a printf style function.  This function allows for subsitution of these characters.
// 
// TCHAR*      szInputBuffer : [in] input string buffer containing string to operate on
// CONST UINT  iBufferLength : [in] length of string buffer.
// CONST UINT  iFlags        : [in] Combination of the following flags:
//
//             CRF_XML_SAFE      : Replaces characters unsafe for XML with their egosoft phrase equivilent
//             CRF_XML_UNSAFE    : Replaces egosoft phrases safe for XML with their character equivilent
//
//             CRF_LANG_SAFE     : Prefaces square brackets and curly brackets with a backslash
//             CRF_LANG_UNSAFE   : Removes backslashes prefacing square and curly brackets
//
//             CRF_PRINTF_SAFE   : Replace % with ¶
//             CRF_PRINTF_UNSAFE : Replace ¶ with %
// 
// Return type : FALSE if flags were invalid
//
BOOL      performCharacterReplacement(TCHAR*  szInputBuffer, CONST UINT  iBufferLength, CONST UINT  iFlags)
{
   TCHAR    *szOutputBuffer,      // String used for output
            *szSourceBuffer;      // String used as input
   UINT     iDestinationChar,     // The character position where inserts into the output buffer should be made
            iCurrentChar;         // Current character position
   
   /// Validate flags
   if ((iFlags == NULL) OR (iFlags INCLUDES CRF_PRINTF_SAFE) AND (iFlags INCLUDES CRF_PRINTF_UNSAFE) OR (iFlags INCLUDES CRF_XML_SAFE) AND (iFlags INCLUDES CRF_XML_UNSAFE))
      return FALSE;

   /// Work from a backup of the input buffer
   szSourceBuffer = utilDuplicateString(szInputBuffer, iBufferLength);
   utilZeroString(szInputBuffer, iBufferLength);

   szOutputBuffer = szInputBuffer;
   
   /// Copy source string to output, char by char.  Abort if we reach a NULL or the end of source or destination string
   for (iCurrentChar = 0, iDestinationChar = 0;    
         iCurrentChar < iBufferLength AND iDestinationChar < iBufferLength AND szSourceBuffer[iCurrentChar]; 
          iCurrentChar++, iDestinationChar++)
   {
      // Examine current character
      switch (szSourceBuffer[iCurrentChar])
      {
      // Non-relevant character
      default:
         szOutputBuffer[iDestinationChar] = szSourceBuffer[iCurrentChar];
         continue;

      /// REDUCE XML SAFETY PHRASES: "\053", "\(" or "\)"
      case '\\':
         // [XML]
         if (iFlags INCLUDES CRF_XML_UNSAFE)
         {
            // Replace "\(" with "("
            if (findPhraseInString(TEXT("\\("), 2, szSourceBuffer, iCurrentChar))
            {
               szOutputBuffer[iDestinationChar] = '(';
               iCurrentChar++;
            }
            // Replace "\)" with ")"
            else if (findPhraseInString(TEXT("\\)"), 2, szSourceBuffer, iCurrentChar))
            {
               szOutputBuffer[iDestinationChar] = ')';
               iCurrentChar++;
            }
            // Replace "\053" with "+"
            else if (findPhraseInString(TEXT("\\053"), 4, szSourceBuffer, iCurrentChar))
            {
               szOutputBuffer[iDestinationChar] = '+';
               iCurrentChar += 3;      // Consume extra source characters
            }
            // Copy direct
            else
               szOutputBuffer[iDestinationChar] = szSourceBuffer[iCurrentChar];
         }
         // [LANGUAGE]
         else if (iFlags INCLUDES CRF_LANG_UNSAFE)
         {
            // Replace "\[" with "["
            if (findPhraseInString(TEXT("\\["), 2, szSourceBuffer, iCurrentChar))
            {
               szOutputBuffer[iDestinationChar] = '[';
               iCurrentChar++;
            }
            // Replace "\]" with "]"
            else if (findPhraseInString(TEXT("\\]"), 2, szSourceBuffer, iCurrentChar))
            {
               szOutputBuffer[iDestinationChar] = ']';
               iCurrentChar++;
            }
            // Replace "\{" with "{"
            if (findPhraseInString(TEXT("\\{"), 2, szSourceBuffer, iCurrentChar))
            {
               szOutputBuffer[iDestinationChar] = '{';
               iCurrentChar++;
            }
            // Replace "\}" with "}"
            else if (findPhraseInString(TEXT("\\}"), 2, szSourceBuffer, iCurrentChar))
            {
               szOutputBuffer[iDestinationChar] = '}';
               iCurrentChar++;
            }
            // Replace "\n" with a newline characters
            else if (findPhraseInString(TEXT("\\n"), 2, szSourceBuffer, iCurrentChar))
            {
               szOutputBuffer[iDestinationChar] = '\n';
               iCurrentChar++;
            }
         }
         // Copy direct
         else
            szOutputBuffer[iDestinationChar] = szSourceBuffer[iCurrentChar];
         continue;

      /// EXPAND XML CHARACTER: "&"
      /// REDUCE XML SAFETY PHRASES: "&amp;", "&gt;", "&lt;" or "&quot;"
      case '&':
         // Replace "&" with safe equivilient "&amp;"
         if (iFlags INCLUDES CRF_XML_SAFE)
         {
            StringCchCat(szOutputBuffer, iBufferLength, TEXT("&amp;"));
            iDestinationChar += 4;      // Consume extra destination characters
         }
         // Replace "&amp;", "&gt;", "&lt;" or "&quot;" with their non-safe equivilients
         else if (iFlags INCLUDES CRF_XML_UNSAFE)
         {
            // Replace "&amp;" with "&"
            if (findPhraseInString(TEXT("&amp;"), 5, szSourceBuffer, iCurrentChar))
            {
               szOutputBuffer[iDestinationChar] = '&';
               iCurrentChar += 4;      // Consume extra source characters
            }
            // Replace "&gt;" with ">"
            else if (findPhraseInString(TEXT("&gt;"), 4, szSourceBuffer, iCurrentChar))
            {
               szOutputBuffer[iDestinationChar] = '>';
               iCurrentChar += 3;      // Consume extra source characters
            }
            // Replace "&lt;" with "<"
            else if (findPhraseInString(TEXT("&lt;"), 4, szSourceBuffer, iCurrentChar))
            {
               szOutputBuffer[iDestinationChar] = '<';
               iCurrentChar += 3;      // Consume extra source characters
            }
            // Replace "&quot;" with """
            else if (findPhraseInString(TEXT("&quot;"), 6, szSourceBuffer, iCurrentChar))
            {
               szOutputBuffer[iDestinationChar] = '"';
               iCurrentChar += 5;      // Consume extra source characters
            }
            // Copy lone ampersand directly
            else
               szOutputBuffer[iDestinationChar] = szSourceBuffer[iCurrentChar];
         }
         else
            szOutputBuffer[iDestinationChar] = szSourceBuffer[iCurrentChar];
         continue;

      /// EXPAND XML CHARACTERS: 
      case '+':
         if (iFlags INCLUDES CRF_XML_SAFE)
         {
            StringCchCat(szOutputBuffer, iBufferLength, TEXT("\\053"));
            iDestinationChar += 3;         // Consume extra destination characters
         }
         else
            szOutputBuffer[iDestinationChar] = szSourceBuffer[iCurrentChar];
         continue;

      case '>':
         if (iFlags INCLUDES CRF_XML_SAFE)
         {
            StringCchCat(szOutputBuffer, iBufferLength, TEXT("&gt;"));
            iDestinationChar += 3;         // Consume extra destination characters
         }
         else
            szOutputBuffer[iDestinationChar] = szSourceBuffer[iCurrentChar];
         continue;

      case '<':
         if (iFlags INCLUDES CRF_XML_SAFE)
         {
            StringCchCat(szOutputBuffer, iBufferLength, TEXT("&lt;"));
            iDestinationChar += 3;         // Consume extra destination characters
         }
         else
            szOutputBuffer[iDestinationChar] = szSourceBuffer[iCurrentChar];
         continue;

      case '"':
         if (iFlags INCLUDES CRF_XML_SAFE)
         {
            StringCchCat(szOutputBuffer, iBufferLength, TEXT("&quot;"));
            iDestinationChar += 5;         // Consume extra destination characters
         }
         else
            szOutputBuffer[iDestinationChar] = szSourceBuffer[iCurrentChar];
         continue;

      case '(':
         if (iFlags INCLUDES CRF_XML_SAFE)
         {
            StringCchCat(szOutputBuffer, iBufferLength, TEXT("\\("));
            iDestinationChar++;         // Consume extra destination characters
         }
         else
            szOutputBuffer[iDestinationChar] = szSourceBuffer[iCurrentChar];
         continue;

      case ')':
         if (iFlags INCLUDES CRF_XML_SAFE)
         {
            StringCchCat(szOutputBuffer, iBufferLength, TEXT("\\)"));
            iDestinationChar++;         // Consume extra destination characters
         }
         else
            szOutputBuffer[iDestinationChar] = szSourceBuffer[iCurrentChar];
         continue;

      /// [EXPAND LANGUAGE CHARACTERs]
      case '[':
         if (iFlags INCLUDES CRF_LANG_SAFE)
         {
            StringCchCat(szOutputBuffer, iBufferLength, TEXT("\\["));
            iDestinationChar++;         // Consume extra destination characters
         }
         else
            szOutputBuffer[iDestinationChar] = szSourceBuffer[iCurrentChar];
         continue;

      case ']':
         if (iFlags INCLUDES CRF_LANG_SAFE)
         {
            StringCchCat(szOutputBuffer, iBufferLength, TEXT("\\]"));
            iDestinationChar++;         // Consume extra destination characters
         }
         else
            szOutputBuffer[iDestinationChar] = szSourceBuffer[iCurrentChar];
         continue;

      case '{':
         if (iFlags INCLUDES CRF_LANG_SAFE)
         {
            StringCchCat(szOutputBuffer, iBufferLength, TEXT("\\{"));
            iDestinationChar++;         // Consume extra destination characters
         }
         else
            szOutputBuffer[iDestinationChar] = szSourceBuffer[iCurrentChar];
         continue;

      case '}':
         if (iFlags INCLUDES CRF_LANG_SAFE)
         {
            StringCchCat(szOutputBuffer, iBufferLength, TEXT("\\}"));
            iDestinationChar++;         // Consume extra destination characters
         }
         else
            szOutputBuffer[iDestinationChar] = szSourceBuffer[iCurrentChar];
         continue;

      case '\r':
         if (iFlags INCLUDES CRF_LANG_SAFE)
         {
            StringCchCat(szOutputBuffer, iBufferLength, TEXT("\\n"));
            iDestinationChar++;         // Consume extra destination characters
         }
         else
            szOutputBuffer[iDestinationChar] = szSourceBuffer[iCurrentChar];
         continue;

      /// [PRINTF CHARACTERS]
      case '%':
         /// CHANGE PERCENTAGE TO AN UNLIKELY CHARACTER
         szOutputBuffer[iDestinationChar] = (iFlags INCLUDES CRF_PRINTF_SAFE ? 0x00B6 : szSourceBuffer[iCurrentChar]);
         continue;
            
      case 0x00B6:   // 0x00B6 == ¶
         /// CHANGE UNLIKELY CHARACTER TO A PERCENTAGE
         szOutputBuffer[iDestinationChar] = (iFlags INCLUDES CRF_PRINTF_UNSAFE ? '%' : szSourceBuffer[iCurrentChar]);
         continue;
      }
   }
   
   /// Cleanup appropriate buffers
   //if (iFlags INCLUDES (CRF_XML_UNSAFE WITH CRF_XML_SAFE))
   utilDeleteString(szSourceBuffer);

   // Zero remaining destination buffer characters (if any)
   if (iDestinationChar < iBufferLength)
      utilZeroString(&szOutputBuffer[iDestinationChar], iBufferLength - iDestinationChar);
      
   return TRUE;
}


/// Function name  : performStringConversion
// Description     : Formats a string for a specific environment - file, memory or display
// 
// TCHAR*             szBuffer      : [in/out] String to convert
// CONST UINT         iBufferLength : [in] Length of string buffer
// CONST STRING_TYPE  eType         : [in] String type to convert to
// 
// Return type : FALSE if eType is invalid
//
/// RESCINDED: Kept for reference
//
BearScriptAPI
BOOL    performStringConversion(TCHAR*  szBuffer, CONST UINT  iBufferLength, CONST STRING_TYPE  eFromType, CONST STRING_TYPE  eToType)
{
   UINT  iFlags = NULL;
  
   TODO("DO NOT USE");
   return FALSE;

   switch (eFromType)
   {
   /// [FROM EXTERNAL TO INTERNAL] -- Convert LF, ', &, <, >, ", %  but not [,],{,}   WARNING: Length will be increased
   case ST_EXTERNAL:
      // [CHECK] Can't convert directly from external <-> display
      ASSERT(eToType != ST_DISPLAY);
      iFlags = CRF_XML_UNSAFE WITH CRF_PRINTF_SAFE;
      break;

   /// [FROM INTERNAL]
   case ST_INTERNAL:
      switch (eToType)
      {
      // [TO EXTERNAL] -- Create XML safety phrases and convert the P symbol
      case ST_EXTERNAL:
         iFlags = CRF_XML_SAFE WITH CRF_PRINTF_UNSAFE;
         break;
      // [TO DISPLAY] -- Convert language brackets and P symbol
      case ST_DISPLAY:
         iFlags = CRF_PRINTF_UNSAFE WITH CRF_LANG_UNSAFE;
         break;
      }
      break;

   /// [FROM DISPLAY TO INTERNAL] -- Convert []. {}, % but not LF, ', &, <, >, "
   case ST_DISPLAY:
      // [CHECK] Can't convert directly from display <-> external
      ASSERT(eToType != ST_EXTERNAL);
      iFlags = CRF_LANG_SAFE WITH CRF_PRINTF_SAFE;
      break;
   }

   return performCharacterReplacement(szBuffer, iBufferLength, iFlags);
}

