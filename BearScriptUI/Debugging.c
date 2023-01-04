//
// Debugging.cpp : Debugging functions for the UI
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       CONSTANTS / GLOBALS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////


/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////


VOID  debugDocument(const DOCUMENT*  pDocument)
{
   if (!pDocument)
      CONSOLE("Document is null");
   else
      CONSOLE("Document '%s' : eType=%s  bModified=%d  bVirtual=%d  bUntitled=%d  bClosing=%d  GameFile=0x%08x", pDocument->szFullPath, identifyDocumentTypeString(pDocument->eType), 
               pDocument->bModified, pDocument->bVirtual, pDocument->bUntitled, pDocument->bClosing, pDocument->pGameFile);
}


VOID  debugDocumentOperationData(const DOCUMENT_OPERATION*  pOperation)
{
   if (!pOperation)
      CONSOLE("Document Operation is null");
   else
      CONSOLE("Document Operation '%s': bActivateOnLoad=%d", pOperation->szFullPath, pOperation->bActivateOnLoad);
      
   // Operation + Document
   debugOperationData(pOperation);
   debugDocument(pOperation ? (DOCUMENT*)pOperation->pDocument : NULL);
}


/// Function name  : debugScriptDocument
// Description     : 
// 
// const SCRIPT_DOCUMENT*   pDocument   : [in] 
// 
VOID  debugScriptDocument(const SCRIPT_DOCUMENT*   pDocument)
{
   // Document + ScriptFile
   debugDocument(pDocument);
   debugScriptFile(pDocument->pScriptFile);
}