//
// Code Objects.cpp : Tokenises script code into components
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       CONSTANTS / GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////



/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       CREATION / DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  :   createCodeObject
// Description     : Initialise a script code tokenising object
//
// CONST TCHAR*  szLine : [in] Text of line to tokenise
//
// Return type : New empty code object, you are responsible for destroying it
//
BearScriptAPI 
CODEOBJECT*      createCodeObject(CONST TCHAR*  szLine)
{
   CODEOBJECT*  pNewObject = utilCreateEmptyObject(CODEOBJECT);

   // Set source pointer
   pNewObject->szSource = szLine;                           
   pNewObject->iSourceLength = lstrlen(pNewObject->szSource);

   // Create phrase buffer
   pNewObject->szText = utilCreateEmptyString(1024);      

   return pNewObject;
}

/// Function name  :   deleteCodeObject
// Description     : Deletes a CODEOBJECT
// 
// CODEOBJECT*  &pCodeObject : Reference to a CODEOBJECT pointer
// 
BearScriptAPI 
VOID         deleteCodeObject(CODEOBJECT*  &pCodeObject)
{
   // Free phrase buffer
   utilDeleteString(pCodeObject->szText);

   // Destroy code object
   utilDeleteObject(pCodeObject);
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : isCodeObjectCommandKeyword
// Description     : Determines whether a CodeObject of type CO_KEYWORD is a conditional or a command
///                                             NB: The 'else' command has to be classed as a conditional
// 
// CONST CODEOBJECT*  pCodeObject : [in] CodeObject to test.
// 
// Return Value   : TRUE for commands, FALSE for keywords.  
// 
BOOL   isCodeObjectCommandKeyword(CONST CODEOBJECT*  pCodeObject)
{
   static CONST TCHAR* szKeywords[11] = { TEXT("end"),       TEXT("break"),  
                                          TEXT("endsub"),    TEXT("continue"),
                                          TEXT("gosub"),     TEXT("return"),  
                                          TEXT("goto"),      TEXT("for"),
                                          TEXT("label"),     
                                          TEXT("each"),   }; //   TEXT("step")  };

   // [CHECK] Ensure CodeObject is a keyword
   ASSERT(pCodeObject->eClass == CO_KEYWORD);

   /// Iterate through keyword array
   for (UINT i = 0; i < 10; i++)
      if (utilCompareStringVariables(pCodeObject->szText, szKeywords[i]))
         // [FOUND] Return TRUE
         return TRUE;

   // [NOT FOUND] Return FALSE
   return FALSE;
}


/// Function name  : isStringLogicalOperator
// Description     : Determine whether a string is a logical operator
// 
// CONST TCHAR*  szString : [in] String to test
// 
// Return Value   : TRUE or FALSE
// 
BearScriptAPI
BOOL  isStringLogicalOperator(CONST TCHAR*  szString)
{
   static CONST TCHAR*  szLogicalOperators[3] = { TEXT("and"), 
                                                  TEXT("or"), 
                                                  TEXT("mod")  };

   /// Iterate through operators array
   for (UINT i = 0; i < 3; i++)
      if (StrCmpI(szString, szLogicalOperators[i]) == 0)
         // [FOUND] Return TRUE
         return TRUE;

   // [NOT FOUND] Return FALSE
   return FALSE;
}



/// Function name  : isStringKeyword
// Description     : Determine whether a string is a reserved word
// 
// CONST TCHAR*  szString : [in] Word to test
// CONST UINT    iIndex   : [in] CodeObject index
// 
// Return Value   : TRUE or FALSE
// 
BOOL  isStringKeyword(CONST TCHAR*  szString, CONST UINT  iIndex)
{
   // Keywords that can only be used as first or second word on line
   static CONST TCHAR*  szFirst[15] = { TEXT("while"), TEXT("skip"),   TEXT("do"),    TEXT("else"),  TEXT("end"), TEXT("endsub"), TEXT("break"), TEXT("continue"), TEXT("gosub"), 
                                        TEXT("goto"),  TEXT("return"), TEXT("start"), TEXT("label"), TEXT("for"), TEXT("if")  };
   static CONST TCHAR*  szSecond[4] = { TEXT("if"),    TEXT("not"),    TEXT("label"), TEXT("each")   };
   
   switch (iIndex)
   {
   /// [FIRST WORD] Iterate through keyword array
   case 1:
      for (UINT iWord = 0; iWord < 15; iWord++)
         // [CHECK] Return TRUE if keyword matches
         if (utilCompareStringVariables(szString, szFirst[iWord]))
            return TRUE;
      break;

   /// [SECOND WORD] Only 'if', 'not' and 'label' are allowed here
   case 2:
      for (UINT iWord = 0; iWord < 4; iWord++)
         // [CHECK] Return TRUE if keyword matches
         if (utilCompareStringVariables(szString, szSecond[iWord]))
            return TRUE;
      break;
      break;

   /// [THIRD WORD] Only 'not' is allowed here
   case 3:
      if (utilCompareStringVariables(szString, TEXT("not")))
         return TRUE;
      break;

   /*/// [SEVENTH WORD] Only 'step' is allowed here
   case 7:
      if (utilCompareStringVariables(szString, TEXT("step")))
         return TRUE;
      break;*/
   }

   // [NOT FOUND] Return FALSE
   return FALSE;
}


/// Function name  : identifyArrayIndex
// Description     : Determine whether the text at a CODEOBJECT's current position is a constant or an array index
//                        (both of use the same operator)
// 
// CODEOBJECT*  pData : Valid Code object positioned at the opening bracket of a constant.
// 
// Return type : TRUE if text before ']' is entirely NUMBER, WHITESPACE or VARIABLE.   FALSE if not.
//
BearScriptAPI 
BOOL   identifyArrayIndex(CODEOBJECT*  pData)
{
   BOOL   bNumericConstant,
          bVariableConstant;

   // Set initial values.  Default to numeric
   bNumericConstant = TRUE;
   bVariableConstant   = FALSE;

   // Begin searching after the '[' character
   for (UINT i = pData->iEndCharIndex + 1; i < pData->iSourceLength; i++)
      switch (identifyCharacterClass(pData->szSource[i]))
      {
      // [LETTER] -- Classify as 'not numeric'
      case CC_LETTER:
         bNumericConstant = FALSE;
         continue;

      // [OPERATOR]
      case CC_OPERATOR:
         switch (pData->szSource[i])
         {
         // [VARIABLE] -- '$' Detected
         case '$':
            bVariableConstant = TRUE;
            continue;
         
         // [END OF CONSTANT]  --  return result
         case ']':
            return bVariableConstant OR bNumericConstant ? TRUE : FALSE;

         // [OPERATOR] -- Indicates neither number nor variable
         default:
            bVariableConstant = bNumericConstant = FALSE;
            continue;
         }

      // [NUMBER / WHITESPACE] -- Do nothing
      default:
         continue;
      }
   
   // No End of Constant found, return current result.
   return bVariableConstant OR bNumericConstant ? TRUE : FALSE;
}


/// Function name  : identifyCharacterClass
// Description     : Categorise a character into one of a series of classes
//
// CONST TCHAR  cCharacter : Character
// 
// Return type : Character Class
//
BearScriptAPI 
CHARACTER_CLASS    identifyCharacterClass(CONST TCHAR  cCharacter)
{
   CHARACTER_CLASS    eClass;

   // TODO: We could use 'IsCharAlphaNumeric()' here to isolate all symbols from characters
   //       Alternatively we could identify whitespace, numbers, characters and some operators manually, and consider anything else an operator.

   switch (cCharacter)
   {
   case ' ':
   case '\t':
   case '\r':
      eClass = CC_WHITESPACE;
      break;

   case '0':
   case '1':
   case '2':
   case '3':
   case '4':
   case '5':
   case '6':
   case '7':
   case '8':
   case '9':
      eClass = CC_NUMBER;
      break;

   case '*':   // Arithmetic operators
   case '/':   
   case '+':
   case '-':
   case '(':
   case ')':
   case '<':   // Boolean operators
   case '>':
   case '=':   
   case '&':   // Bitwise operators
   case '|':
   case '^':
   case '!':
   case '~':
   case '[':   // Script operators
   case ']':
   case '{':
   case '}':
   case '$':
   case '\'':
   case '@':   
   case ':':
   case ',':
   case '"':   // Remaining unrelated operators available on british keyboard
   case '#':
   case '£':
   case '%':
   case '\\':
   case '`':
   case '¬':
      eClass = CC_OPERATOR;
      break;

   case '_':   /// HACK: I've decided to make underscore a letter so constants with underscores in tooltips are treated as whole words
   default:
      eClass = CC_LETTER;
      break;
   }

   return eClass;
}


/// Function name  : identifyConditionalID
// Description     : Identifies the conditional, if any, of a script command
// 
// CONST TCHAR*  szLine : [in] Script command to identify the conditional for
// 
// Return type : Conditional if present, otherwise CI_NONE
//
BearScriptAPI 
CONDITIONAL_ID   identifyConditionalID(CONST TCHAR*  szLine)
{
   CODEOBJECT*       pCodeObject;         // CodeObject
   CONDITIONAL_ID    eResult;             // Operation result
   TCHAR*            szHash;              // Hash of the first three keywords in the input string
   BOOL              bLabelDetected;      // Whether a label CodeObject was detected or not. Used because the 'define-label' commands generates no CO_KEYWORD CodeObjects

   // Prepare
   pCodeObject    = createCodeObject(szLine);
   szHash         = utilCreateEmptyString(HASH_LENGTH);
   bLabelDetected = FALSE;

   /// Search the first three CodeObjects for keywords
   while (findNextCodeObject(pCodeObject) AND pCodeObject->iIndex <= 3)
   {
      // [KEYWORD] Add to the hash
      if (pCodeObject->eClass == CO_KEYWORD)
         appendCodeObjectToHash(pCodeObject, szHash);

      // [LABEL] Set label flag
      else if (pCodeObject->eClass == CO_LABEL)
         bLabelDetected = TRUE;
   }

   /// Identify conditional from hash
   // [DEFINE LABEL, NONE]
   if (!lstrlen(szHash))
      eResult = (bLabelDetected ? CI_LABEL : CI_NONE);
   
   // [IF] 
   else if (utilCompareString(szHash, "if"))
      eResult = CI_IF;
   else if (utilCompareString(szHash, "ifnot"))
      eResult = CI_IF_NOT;

   // [SKIP-IF, SKIP-IF-NOT]
   else if (utilCompareString(szHash, "skipif"))
      eResult = CI_SKIP_IF;
   else if (utilCompareString(szHash, "skipifnot"))
      eResult = CI_SKIP_IF_NOT;

   // [ELSE, ELSE-IF, ELSE-IF-NOT]
   else if (utilCompareString(szHash, "else"))
      eResult = CI_ELSE;
   else if (utilCompareString(szHash, "elseif"))
      eResult = CI_ELSE_IF;
   else if (utilCompareString(szHash, "elseifnot"))
      eResult = CI_ELSE_IF_NOT;

   // [WHILE, WHILE-NOT]
   else if (utilCompareString(szHash, "while"))
      eResult = CI_WHILE;
   else if (utilCompareString(szHash, "whilenot"))
      eResult = CI_WHILE_NOT;

   // [BREAK/CONTINUE/END/RETURN]
   else if (utilCompareString(szHash, "break"))
      eResult = CI_BREAK;
   else if (utilCompareString(szHash, "continue"))
      eResult = CI_CONTINUE;
   else if (utilCompareString(szHash, "end"))
      eResult = CI_END;
   else if (utilCompareString(szHash, "return"))
      eResult = CI_NONE;

   // [END-SUB/GOTO-LABEL/GOSUB/START]
   else if (utilCompareString(szHash, "endsub"))
      eResult = CI_END_SUB;
   else if (utilCompareString(szHash, "gotolabel"))
      eResult = CI_GOTO_LABEL;
   else if (utilCompareString(szHash, "gosub"))
      eResult = CI_GOTO_SUB;
   else if (utilCompareString(szHash, "start"))
      eResult = CI_START;

   // [DEFAULT]
   else
      eResult = CI_NONE;

   // Cleanup and return result
   deleteCodeObject(pCodeObject);
   utilDeleteString(szHash);
   return eResult;
}


/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : findNextCodeObject
// Description     : Tokenise a line of script code into it's various components (called 'CodeObjects')
//
// CODEOBJECT*  pData : [in/out] Current 'code object'
// 
// Return type : TRUE if there was another code object, FALSE if not.
//
BearScriptAPI 
BOOL   findNextCodeObject(CODEOBJECT*  pData)
{
   CHARACTER_CLASS   eInitialClass,           // Character class of the first letter in phrase
                     eCurrentClass;           // Character class of the current letter in phrase
   TCHAR             chInitial,               // First letter in phrase
                     chCurrent;               // Current letter in phrase
   BOOL              bConstantIsArrayIndex;   // [CONSTANTS] TRUE if the constant is a number or a variable
   UINT              i;                       // Phrase iterator

   // Check if there is no more string left to check
   if (pData->iEndCharIndex >= pData->iSourceLength)
      return FALSE;

   /// Get INITIAL CHARACTER and INITIAL CHARACTER CLASS
   chInitial     = pData->szSource[pData->iEndCharIndex];
   eInitialClass = identifyCharacterClass(chInitial);

   /// Determine INITIAL CODEOBJECT CLASS
   switch (eInitialClass)
   {
   case CC_WHITESPACE:  pData->eClass = CO_WHITESPACE;   break;
   case CC_LETTER:      pData->eClass = CO_WORD;         break;
   case CC_OPERATOR:    pData->eClass = CO_OPERATOR;     break;
   case CC_NUMBER:      pData->eClass = CO_NUMBER;       break;
   }

   // [OPERATOR] Attempt to identify directly
   if (pData->eClass == CO_OPERATOR)
      switch (chInitial)
      {
      case '\'':   pData->eClass = CO_STRING;        break;
      case '[':    pData->eClass = CO_CONSTANT;      break;
      case '{':    pData->eClass = CO_ENUMERATION;   break;
      case '$':    pData->eClass = CO_VARIABLE;      break;
      }
   
   // [SPECIAL CASE] CONSTANTS -- Is text between [ ] operators a NUMBER, VARIABLE or WHITESPACE?
   if (pData->eClass == CO_CONSTANT)
      bConstantIsArrayIndex = identifyArrayIndex(pData);

   /// Examine each subsequent character
   for (i = pData->iEndCharIndex + 1; i < pData->iSourceLength; i++)
   {
      // Classify current character
      chCurrent     = pData->szSource[i];
      eCurrentClass = identifyCharacterClass(chCurrent);

      switch (pData->eClass)
      {
      /// [WHITESPACE]  --  Allow only whitespace characters
      case CO_WHITESPACE:
         if (eCurrentClass == CC_WHITESPACE)
            continue;
         break;

      // [WORD CLASS]
      case CO_WORD:
         /// [WORDS]  --  Allow letters and numbers in words
         if (eCurrentClass == CC_NUMBER OR eCurrentClass == CC_LETTER)
            continue;
         /// [LABELS]  --  Require ':' to be the last character on the line
         else if (chCurrent == ':' AND pData->iSourceLength == i + 1)
            pData->eClass = (pData->iIndex <= 3 ? CO_LABEL : CO_WORD);      // Return LABEL or WORD
         break;

      /// [VARIABLES]  --  Allow alphanumeric characters and the '.' character
      case CO_VARIABLE:
         if (eCurrentClass == CC_LETTER OR eCurrentClass == CC_NUMBER OR chCurrent == '.')
            continue;
         break;

      // [OPERATOR CLASS]
      case CO_OPERATOR:
         switch (chInitial)
         {
         /// [OPERATORS]  --  Return single character operators individually
         default:
            break;

         /// [COMMENT/MULTIPLY OPERATOR]  --  Classify as one or the other based on whether this is the first CodeObject for the line, or not.
         case '*':
            if (pData->iIndex == 0)  
            {
               pData->eClass = CO_COMMENT;         // Class entire line as COMMENT if this is the first CodeObject.
               continue;   
            }
            break;            // Return as multiply operator.

         /// [DOUBLE CHARACTER OPERATORS]  --  Identify as single or double character operators, and return.
         case '!':
         case '<':
         case '>':
         case '=':
            if (chCurrent == '=')
               i++;
            break;

         // [SUBTRACT OPERATOR]
         case '-':
            switch (eCurrentClass)
            {
            /// [MINUS NUMBERS]  --  Change class to NUMBER and continue searching
            case CC_NUMBER:
               pData->eClass = CO_NUMBER;
               continue;

            /// [REFERENCE OPERATOR]  --  Return reference operator or subtraction operator
            case CC_OPERATOR:
               if (chCurrent == '>')
                  i++;      // Include this character in codeobject
               break;
            }
            break;
         }
         break;

      /// [NUMBERS]  --  Allow only numbers
      case CO_NUMBER:
         if (eCurrentClass == CC_NUMBER)
            continue;
         break;

      /// [STRINGS]  --  Allow all characters until another apostrophe is found
      case CO_STRING:
         if (chCurrent != '\'')
            continue;
         i++;      // Return and include this character in codeobject
         break;

      /// [COMMENTS]
      case CO_COMMENT:
         continue;         // Nothing interrupts comments

      // [CONSTANT CLASS]
      case CO_CONSTANT:
         /// [ARRAY INDEX]  --  Return '[' or ']' character as an OPERATOR
         if (bConstantIsArrayIndex)
            pData->eClass = CO_OPERATOR;

         // [CHECK] Abort if enumeration is an open bracket followed by a space, equals or reference operator
         else if (i == (pData->iEndCharIndex + 1) AND (chCurrent == ' ' OR chCurrent == '=' OR utilCompareStringN(&pData->szSource[i], "->", 2)))
            break;

         /// [CONSTANTS]  --  Allow all characters until ']' is found
         else if (chCurrent != ']')
            continue;
         else
            i++;      // Return and include this character in codeobject
         break;

      /// [ENUMERATIONS]  --  Allow all characters until '}' is found
      case CO_ENUMERATION:
         // [CHECK] Abort if enumeration is an open bracket followed by a space, equals or reference operator
         if (i == (pData->iEndCharIndex + 1) AND (chCurrent == ' ' OR chCurrent == '=' OR utilCompareStringN(&pData->szSource[i], "->", 2)))
            break;
         // 
         else if (chCurrent != '}') 
            continue;
        
         i++;      // Return and include this character in codeobject
         break;
      } // END - switch 'code object class'

      // Stop searching if we reach here
      break;
   } // END - for 'each character in source'

   /// [EXTRACT TEXT]  --  Set CodeObject index, phrase position and copy phrase text.
   pData->iStartCharIndex = pData->iEndCharIndex;
   pData->iEndCharIndex   = i;
   pData->iLength         = i - pData->iStartCharIndex;

   // Copy new phrase to text buffer
   StringCchCopyN(pData->szText, 1024, &pData->szSource[pData->iStartCharIndex], pData->iLength);

   switch (pData->eClass)
   {
   /// [WORD] Check for reserved words
   case CO_WORD:
      // [NULL]
      if (utilCompareString(pData->szText, "null"))
         pData->eClass = CO_NULL;

      // [KEYWORD]
      else if (isStringKeyword(pData->szText, pData->iIndex + 1))
         pData->eClass = CO_KEYWORD;

      // [OPERATOR]
      else if (isStringLogicalOperator(pData->szText))
         pData->eClass = CO_OPERATOR;
      
      // Fall through...

   /// [NON-WHITESPACE] Give each non-whitespace CodeObject an index.
   default:
      pData->iIndex++;
      break;

   /// [WHITESPACE] Do not give an index
   case CO_WHITESPACE:
      break;
   }
   
   // Return TRUE to indicate valid CodeObject
   return TRUE;
}



/// Function name  : findNextCodeObjectWithArguments
// Description     : Functions exactly like findNextCodeObject(..) except it returns Argument CodeObjects where appropriate, instead of plain Variable CodeObjects
// 
// CODEOBJECT*         pCodeObject : [in/out]       Valid CodeObject
// CONST SCRIPT_FILE*  pScriptFile : [in][optional] ScriptFile containing the ARGUMENTs. If this is not specified then this functions acts like the standard 'findNextCodeObject'
// 
// Return Value   : TRUE if another was found, FALSE otherwise
// 
BearScriptAPI
BOOL  findNextCodeObjectWithArguments(CODEOBJECT*  pCodeObject, CONST SCRIPT_FILE*  pScriptFile)
{
   BOOL       bResult;        // Operation result

   /// Find next CodeObject
   if (bResult = findNextCodeObject(pCodeObject))
   {
      // [CHECK] Is this variable actually an argument?
      if (pCodeObject->eClass == CO_VARIABLE AND isArgumentInScriptFile(pScriptFile, pCodeObject->szText))
         /// [FOUND] Change class to ARGUMENT
         pCodeObject->eClass = CO_ARGUMENT;
   }
      
   // Return TRUE if any CodeObject was found
   return bResult;
}


/// Function name  : findCodeObjectByCharacterIndex
// Description     : Retrieves the CodeObject at the specified character index
// 
// CODEOBJECT*  pCodeObject : [in/out] CodeObject to operate on
// CONST UINT   iIndex      : [in]     Zero-based CodeObject index to retrieve
// 
// Return Value   : TRUE if found, otherwise FALSE
// 
BearScriptAPI
BOOL   findCodeObjectByCharacterIndex(CODEOBJECT*  pCodeObject, CONST UINT  iIndex)
{
   // Iterate through CodeObjects
   while (findNextCodeObject(pCodeObject))
   {
      // [CHECK] Is character within this codeobject?
      if (iIndex >= pCodeObject->iStartCharIndex AND iIndex < pCodeObject->iEndCharIndex)
         // [FOUND] Return TRUE
         return TRUE;
   }
   
   // [NOT FOUND] Return FALSE
   return FALSE;
}



/// Function name  : findNextParameterCodeObject
// Description     : Returns the CodeObject that can represent a parameter
// 
// CODEOBJECT*  pCodeObject    : [in] Valid CodeObject
// CONST BOOL   bSkipOperators : [in] If FALSE then CO_OPERATOR CodeObjects will be returned, if TRUE they will not
// 
// Return Value   : TRUE if found, FALSE otherwise
// 
BOOL  findNextParameterCodeObject(CODEOBJECT*  pCodeObject, CONST BOOL  bSkipOperators)
{
   // Iterate through remaining CodeObjects
   while (findNextCodeObject(pCodeObject))
   {
      // Determine whether this codeobject can represent a parameter
      switch (pCodeObject->eClass)
      {
      /// [WORD, KEYWORD, WHITESPACE] These cannot represent parameters
      case CO_KEYWORD:
      case CO_WORD:
      case CO_WHITESPACE:
         break;

      /// [OPERATOR] Only return these if desired  (ie. for expression generation)
      case CO_OPERATOR:
         if (!bSkipOperators)
            // [FOUND] Return TRUE
            return TRUE;
         break;

      /// [EVERYTHING ELSE] These can all represent parameters
      default:
         // [FOUND] Return TRUE
         return TRUE;
      }
   }

   // [NOT FOUND] Return FALSE
   return FALSE;
}


/// Function name  : findNextScriptCallArgumentCodeObject
// Description     : Returns the CodeObject representing a script-call argument
// 
// CODEOBJECT*     pCodeObject    : [in/out] Valid CodeObject, contains the argument value on return
// CONST COMMAND*  pCommand       : in]      Command being generated, used for error reporting
// TCHAR*         &szArgumentName : [out]    New string containing the name of the argument. You are responsible for destroying it
// ERROR_STACK*   &pError         : [out]    New error containing syntax errors, if any. You are responsible for destroying it
// 
// Return Value   : TRUE if found and there were no errors, otherwise FALSE
// 
BOOL  findNextScriptCallArgumentCodeObject(CODEOBJECT*  pCodeObject, CONST COMMAND*  pCommand, TCHAR*  &szArgumentName, ERROR_STACK*  &pError)
{
   BOOL   bAssignmentFound;      // Indicates the assignment operator has been found

   // Prepare
   szArgumentName   = NULL;
   bAssignmentFound = FALSE;
   pError           = NULL;

   // Iterate through remaining CodeObjects
   while (findNextCodeObject(pCodeObject))
   {
      // Determine whether this codeobject can represent a parameter
      switch (pCodeObject->eClass)
      {
      /// [COMMENT/LABEL] Error
      case CO_COMMENT:
      case CO_LABEL:
         break;

      /// [WHITESPACE] Skip
      case CO_WHITESPACE:
         continue;

      /// [OPERATOR] Ensure the assignment operator is present
      case CO_OPERATOR:
         // [COLON] Allow the colon operator
         if (utilCompareString(pCodeObject->szText, ":"))
            continue;

         // [CHECK] Ensure assignment operator is present and alone
         else if (bAssignmentFound = utilCompareString(pCodeObject->szText, "="))
            continue;

         // [UNKNOWN OPERATOR] Abort and return FALSE
         break;

      /// [ARGUMENT NAME or VALUE] Allow reserved words as argument names
      default:
         // [CHECK] Ensure argument name hasn't already been found
         if (!bAssignmentFound AND !szArgumentName)
         {
            // [SUCCESS] Store name and continue searching
            szArgumentName = utilDuplicateSimpleString(pCodeObject->szText);
            continue;
         }
         else if (!bAssignmentFound)
            // [ERROR] "Missing assignment operator for argument '%s' of the script call command on line %u : '%s'"
            pError = generateDualError(HERE(IDS_GENERATION_SCRIPT_CALL_ASSIGNMENT_MISSING), (szArgumentName ? szArgumentName : TEXT("<undetermined>")), pCommand->iLineNumber, pCommand->szBuffer);

         // Abort search
         break;
      }

      // [FOUND/ERROR] Abort search
      break;
   }

   // [CHECK] Ensure argument name and assignment operator are present
   if (!szArgumentName OR !bAssignmentFound)
      // [FAILED] Cleanup argument name
      utilSafeDeleteString(szArgumentName);

   // Return TRUE if operator and name were found
   return (bAssignmentFound AND szArgumentName);
}
