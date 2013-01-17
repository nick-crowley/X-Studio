//
// RichText Macros.h : RichText helper macros
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        MACROS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Macro: szTag
// Description: Surrounds text with a tag
// 
// CONST TCHAR*  szTag  : [in] Tag name
// CONST TCHAR*  szText : [in] Text
//
#define    txtTag(szTag, szText)                           TEXT("[") TEXT(szTag) TEXT("]") TEXT(szText) TEXT("[/") TEXT(szTag) TEXT("]")

/// Macro: txtDoubleTag
// Description: Surrounds text with two tags
// 
// CONST TCHAR*  szTagInner  : [in] Inner Tag name
// CONST TCHAR*  szTagOuter  : [in] Outer Tag name
// CONST TCHAR*  szText      : [in] Text
//
#define    txtDoubleTag(szTagInner, szTagOuter, szText)    TEXT("[") TEXT(szTagOuter) TEXT("]") TEXT("[") TEXT(szTagInner) TEXT("]") TEXT(szText) TEXT("[/") TEXT(szTagInner) TEXT("]") TEXT("[/") TEXT(szTagOuter) TEXT("]")

/// Macro: txtBold
// Description: Emboldens text
// 
// CONST TCHAR*  szText : [in] Text
//
#define    txtBold(szText)                                 txtTag("b", szText)

/// Macro: txtColour
// Description: Changes text colour
// 
// CONST TCHAR*  szColour : [in] Text colour
// CONST TCHAR*  szText   : [in] Text
//
#define    txtColour(szColour, szText)                     txtTag(szColour, szText)

/// Macro: txtItalic
// Description: Italicises text
// 
// CONST TCHAR*  szText : [in] Text
//
#define    txtItalic(szText)                               txtTag("i", szText)

/// Macro: txtHeading
// Description: Generates a emboldened heading
// 
// CONST TCHAR*  szText : [in] Heading text
//
#define    txtHeading(szText)                              txtDoubleTag("b", "u", szText) 

/// Macro: txtLineBreak
// Description: Generates a emboldened heading surrounded by line-breaks
// 
#define    txtLineBreak                                    TEXT("\n\n") 


