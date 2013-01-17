//
// Testing.cpp : Initial testing script
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                              HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////


/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                            FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : testErrorMessageDialog
// Description     : Test the dispay or ErrorStacks and ErrorQueues
// 
// Return Value   : TRUE
// 
BOOL  testErrorMessageDialog()
{
   //ERROR_STACK*  pErrorStack;
   //ERROR_QUEUE*  pErrorQueue;

   //// Create example ErrorStack
   //pErrorStack = generateError(HERE(IDS_SCRIPT_COMMAND_COMMENT_INVALID));
   //enhanceError(pErrorStack, IDS_SCRIPT_COMMAND_COMMENT_INVALID, 1, TEXT("Dummy"));
   //enhanceError(pErrorStack, IDS_SCRIPT_COMMAND_COMMENT_INVALID, 1, 2);
   //enhanceError(pErrorStack, IDS_SCRIPT_COMMAND_COMMENT_INVALID, 42, TEXT("blah"), TEXT("blah"));
   //enhanceError(pErrorStack, IDS_SCRIPT_COMMAND_COMMENT_INVALID, TEXT("blah"));
   //enhanceError(pErrorStack, IDS_ERROR_APPEND_TEXT, TEXT("blah blah blah blah blah blah blah blah"));
   //enhanceError(pErrorStack, IDS_ERROR_APPEND_TEXT, TEXT("blah blah blah blah blah blah blah blah"));

   ///// Test MessageDialog with an ErrorStack
   //displayErrorMessageDialog(NULL, pErrorStack, NULL, MDF_YESNO WITH MDF_ERROR);

   //// Create example ErrorQueue
   //pErrorQueue = createErrorQueue();
   //pushErrorQueue(pErrorQueue, pErrorStack);

   //pErrorStack = generateError(HERE(IDS_SCRIPT_COMMAND_COMMENT_INVALID));
   //enhanceError(pErrorStack, IDS_SCRIPT_COMMAND_COMMENT_INVALID, 55, TEXT("Another"));
   //enhanceError(pErrorStack, IDS_SCRIPT_COMMAND_COMMENT_INVALID, 42, 42);
   //enhanceError(pErrorStack, IDS_SCRIPT_COMMAND_COMMENT_INVALID, 22, TEXT("Mr Bear"), TEXT("Mrs Bear"));
   //enhanceError(pErrorStack, IDS_SCRIPT_COMMAND_COMMENT_INVALID, TEXT("Something else"));
   //enhanceError(pErrorStack, IDS_ERROR_APPEND_TEXT, TEXT("The quick brown fox jumped over the hungry bear"));
   //enhanceError(pErrorStack, IDS_ERROR_APPEND_TEXT, TEXT("There once was a man with no face who ate bacon"));
   //pushErrorQueue(pErrorQueue, pErrorStack);
   //
   ///// Test MessageDialog with an ErrorQueue
   //displayErrorQueueDialog(getAppWindow(), pErrorQueue, NULL, MDF_YESNO WITH MDF_ERROR);

   //// Cleanup and return
   //deleteErrorQueue(pErrorQueue);
   return TRUE;
}


/// Function name  : testMessageDialog
// Description     : Test each of the icons and button arrangements in a MessageDialog
// 
// Return Value   : TRUE
// 
BOOL  testMessageDialog()
{
   displayMessageDialogf(NULL, IDS_SCRIPT_COMMAND_COMMENT_INVALID, NULL, MDF_OK WITH MDF_INFORMATION);
   displayMessageDialogf(NULL, IDS_SCRIPT_COMMAND_COMMENT_INVALID, NULL, MDF_OK WITH MDF_QUESTION);
   displayMessageDialogf(NULL, IDS_SCRIPT_COMMAND_COMMENT_INVALID, NULL, MDF_OK WITH MDF_WARNING);
   displayMessageDialogf(NULL, IDS_SCRIPT_COMMAND_COMMENT_INVALID, NULL, MDF_OK WITH MDF_ERROR);

   displayMessageDialogf(NULL, IDS_SCRIPT_COMMAND_COMMENT_INVALID, NULL, MDF_YESNO WITH MDF_INFORMATION);
   displayMessageDialogf(NULL, IDS_SCRIPT_COMMAND_COMMENT_INVALID, NULL, MDF_YESNO WITH MDF_QUESTION);
   displayMessageDialogf(NULL, IDS_SCRIPT_COMMAND_COMMENT_INVALID, NULL, MDF_YESNO WITH MDF_WARNING);
   displayMessageDialogf(NULL, IDS_SCRIPT_COMMAND_COMMENT_INVALID, NULL, MDF_YESNO WITH MDF_ERROR);

   displayMessageDialogf(NULL, IDS_FEATURE_NOT_IMPLEMENTED, MAKEINTRESOURCE(IDS_TITLE_NOT_IMPLEMENTED), MDF_OK WITH MDF_INFORMATION, TEXT("Command descriptions"));
   return TRUE;
}


/// Function name  : testOutputDialog
// Description     : Test the 'Output Dialog'
// 
// HWND  hOutputDialog   : [in] Output dialog window handle
// 
// Return Value   : TRUE if passed, FALSE if failed
// 
BOOL  testOutputDialog(HWND  hOutputDialog)
{
   CONST TCHAR*  szTestPhrase[3] = { TEXT("The quick brown fox jumped over the lazy dog"), 
                                     TEXT("The quick brown fox jumped over the lazy dog"), 
                                     TEXT("Holy fucking cow there's a bear in my soup") };
   //// Test text output
   /*for (UINT i = 0; i < 3; i++)
      appendTextToOutputDialog(hOutputDialog, szTestPhrase[i], lstrlen(szTestPhrase[i]));*/

   // Test formatting string output
   printMessageToOutputDialogf(ODI_INFORMATION, IDS_SCRIPT_COMMAND_COMMENT_INVALID, TEXT("Bears!"), TEXT("Bears!"), 42, TEXT("c:\\Bears.exe"));
   printMessageToOutputDialogf(ODI_INFORMATION, IDS_SCRIPT_COMMAND_COMMENT_INVALID, TEXT("Bears!"));
   printMessageToOutputDialogf(ODI_INFORMATION, IDS_SCRIPT_COMMAND_COMMENT_INVALID, 42, 21, 6);

   return TRUE;
}
