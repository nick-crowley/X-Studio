//
// Text Stream.cpp : Stream object for repeatedly appending very long strings in an efficient manner
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                      GLOBALS / CONSTANTS
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : createTextStream
// Description     : Creates a TextStream object
// 
// CONST UINT  iInitialSize   : [in] Intial length, in characters
// 
// Return Value   : New TextStream object, you are responsible for destroying it
// 
TEXT_STREAM*  createTextStream(CONST UINT  iInitialSize)
{
   TEXT_STREAM*   pTextStream;

   // Create object
   pTextStream = utilCreateEmptyObject(TEXT_STREAM);

   // Set properties
   pTextStream->szBuffer        = utilCreateEmptyString(iInitialSize);
   pTextStream->szAssembledText = utilCreateEmptyString(MAX_STRING);
   pTextStream->iBufferSize     = iInitialSize;

   // Return object
   return pTextStream;
}


/// Function name  : deleteTextStream
// Description     : Destroys a textStream object
// 
// TEXT_STREAM*  pTextStream   : [in] TextStream object to destroy
// 
VOID   deleteTextStream(TEXT_STREAM*  pTextStream)
{
   // Delete buffers
   utilDeleteStrings(pTextStream->szBuffer, pTextStream->szAssembledText);

   // Delete calling object
   utilDeleteObject(pTextStream);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                          HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : getTextStreamBuffer
// Description     : Retrieves a pointer to the next un-used character in a TextStream object's text buffer
// 
// CONST TEXT_STREAM*  pTextStream : [in] TextStream object to query
// 
// Return Value   : Next un-used character in the buffer
// 
TCHAR*  getTextStreamBuffer(CONST TEXT_STREAM*  pTextStream)
{
   // Return the position of the next unused character
   return &pTextStream->szBuffer[pTextStream->iBufferUsed];
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                          FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : appendStringToTextStream
// Description     : Appends a string to a TextStream object
//
// TEXT_STREAM*  pTextStream : [in/out] TextStream to append
// CONST TCHAR*  szText      : [in]     Text
// 
VOID  appendStringToTextStream(TEXT_STREAM*  pTextStream, CONST TCHAR*  szText)
{
   // Extend stream if buffer more than 85% full
   if (pTextStream->iBufferUsed > utilCalculatePercentage(pTextStream->iBufferSize, 85))
   {
      pTextStream->iBufferSize *= 2;
      pTextStream->szBuffer = utilExtendString(pTextStream->szBuffer, pTextStream->iBufferUsed, pTextStream->iBufferSize);
   }

   /// Append input text to existing buffer
   StringCchCopy(getTextStreamBuffer(pTextStream), pTextStream->iBufferSize - pTextStream->iBufferUsed, szText);

   // Increment current position
   pTextStream->iBufferUsed += lstrlen(szText);
}


/// Function name  : appendStringToTextStreamf
// Description     : Appends a formatted string to a TextStream object
///                                             NB: New strings are limited to an expanded size of MAX_STRING characters
// 
// TEXT_STREAM*  pTextStream : [in/out]       TextStream to append
// CONST TCHAR*  szFormat    : [in]           Printf-style formatting string
// ...           ...         : [in][optional] Arguments
// 
VOID  appendStringToTextStreamf(TEXT_STREAM*  pTextStream, CONST TCHAR*  szFormat, ...)
{
   va_list  pArguments;       // Variable argument pointer

   // Prepare
   pArguments = utilGetFirstVariableArgument(&szFormat);

   /// Assemble input text
   StringCchVPrintf(pTextStream->szAssembledText, MAX_STRING, szFormat, pArguments);

   // Extend stream if buffer more than 85% full
   if (pTextStream->iBufferUsed > utilCalculatePercentage(pTextStream->iBufferSize, 85))
   {
      pTextStream->iBufferSize *= 2;
      pTextStream->szBuffer = utilExtendString(pTextStream->szBuffer, pTextStream->iBufferUsed, pTextStream->iBufferSize);
   }

   /// Append input text to existing buffer
   StringCchCopy(getTextStreamBuffer(pTextStream), pTextStream->iBufferSize - pTextStream->iBufferUsed, pTextStream->szAssembledText);

   // Increment current position
   pTextStream->iBufferUsed += lstrlen(pTextStream->szAssembledText);
}


/// Function name  : appendCharToTextStream
// Description     : Appends a character to a TextStream object
///                                             NB: New strings are limited to an expanded size of MAX_STRING characters
// 
// TEXT_STREAM*  pTextStream : [in/out] TextStream to be appended
// CONST TCHAR   chCharacter : [in]     Character to append
// 
VOID  appendCharToTextStream(TEXT_STREAM*  pTextStream, CONST TCHAR  chCharacter)
{
   // Append character to text stream
   getTextStreamBuffer(pTextStream)[0] = chCharacter;

   // Increment stream position
   pTextStream->iBufferUsed++;
}

