//
// About Dialog.cpp : About dialog
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        MACROS
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  :  findNextPackedString
// Description     : Finds the next string in a string of null terminated packed strings, ending with a double null
// 
// const TCHAR*  szString : [in] 
// 
// Return Value   : Next string, if any
// 
const TCHAR*  findNextPackedString(const TCHAR*  szString)
{
   return szString[0] ? &szString[lstrlen(szString)+1] : &szString[0];
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : setDialogStrings
// Description     : Localises the controls in a dialog
// 
// HWND        hDialog    : [in] Dialog
// const UINT  iStringID  : [in] Resource ID of dialog strings
// const UINT  iSkipCount : [in] Number of control IDs
// const UINT  ...        : [in] Control IDs to skip
// 
// Return Value   : TRUE/FALSE
//
/// RESCINDED:
//
//BOOL  setDialogStrings(HWND  hDialog, const UINT  iStringID, const UINT  iSkipCount, ...)
//{
//   CONST TCHAR  *szClasses[] = { WC_STATIC, WC_BUTTON, WC_LINK },      // Supported window classes
//                *szCtrlText;        // Text for current control
//   TCHAR        *szDlgStrings;      // All strings
//   TCHAR         szCtrlClass[32];   // Window class of current control
//   UINT          iCtrlID,
//                 iIndex = 0,
//                 iSkipIDs[10];
//          
//   /// Load dialog strings
//   if ((szCtrlText = szDlgStrings = loadString(iStringID, 4096)) == NULL)
//      return FALSE;
//
//   /// Enumerate IDs of controls to skip, if any
//   for (va_list  pArgument = utilGetFirstVariableArgument(&iSkipCount); iIndex < iSkipCount AND iIndex < 10; pArgument = utilGetNextVariableArgument(pArgument, UINT))
//      iSkipIDs[iIndex++] = utilGetCurrentVariableArgument(pArgument, UINT);
//   
//   // Iterate through dialog child windows
//   for (HWND  hCtrl = GetFirstChild(hDialog); szCtrlText AND GetClassName(hCtrl, szCtrlClass, 32) AND (iCtrlID = GetWindowID(hCtrl)); hCtrl = GetNextSibling(hCtrl))
//   {
//      BOOL  bMatch = FALSE,
//            bSkip  = FALSE;
//
//      // [CHECK] Ensure control ID isn't in the skip list
//      for (UINT  i = 0; !bSkip AND i < iSkipCount; i++)
//         bSkip = (iCtrlID == iSkipIDs[i]);
//
//      // [CHECK] Ensure control is: STATIC, BUTTON or LINK
//      for (UINT  iClass = 0; !bSkip AND !bMatch AND iClass < 3; iClass++)
//         bMatch = utilCompareStringVariables(szCtrlClass, szClasses[iClass]);
//      
//      // [FAILED] Move to next child
//      if (bSkip OR !bMatch)
//         continue;
//
//      /// [FOUND] Set control text. Extract next string
//      SetWindowText(hCtrl, szCtrlText);
//      szCtrlText = findNextPackedString(szCtrlText);
//   }
//
//   // Cleanup and return
//   utilDeleteString(szDlgStrings);
//   return TRUE;
//}
//
/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     WINDOW PROCEDURE
/// /////////////////////////////////////////////////////////////////////////////////////////
