//
// FILE_NAME.cpp : FILE_DESCRIPTION
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

// Update.xml URL
static const TCHAR*  szUpdateURL = TEXT("http://x-studio.fbnz.de//Update.xml");     

// Max update.xml file size
static const UINT    MAX_BUFFER  = 32768;       

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

TCHAR*  translateUpdateFile(CHAR*  szFile, DWORD&  iLength)
{
   // [CHECK] Skip UTF-8 BOM if present
   if (utilCompareMemory(szFile, iByteOrderingUTF8, 3))
   {
      szFile += 3;
      iLength -= 3;
   }

   /// Generate UNICODE equivilent
   return utilTranslateStringToUNICODE(szFile, iLength);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////


BOOL  downloadUpdate(UPDATE_OPERATION*  pOperationData, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue)
{
   XML_TREE_NODE  *pDataNode,                // Update property/details node
                  *pReleaseNode;             // Latest release node
   ERROR_STACK    *pError      = NULL;       // Error, if any
   HINTERNET       hConnection = NULL,       // Connection
                   hFile       = NULL;       // Update.xml remote file handle
   XML_TREE*       pXMLTree    = NULL;       // Update.xml XML tree
   CHAR*           szBufferA   = NULL;       // I/O buffer (ansi)
   TCHAR          *szBufferW   = NULL,       // I/O buffer (unicode)
                  *szError     = NULL;       // Error
   DWORD           iBytesRead;               // BytesRead
   INT             iLatestVersion;           // Latest version

   /// Open connection
   if (!(hConnection = InternetOpen(getAppName(), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, NULL)))
      // [ERROR] "Unable to open internet connection - %s"
      pushErrorQueue(pErrorQueue, pError = generateDualError(HERE(IDS_UPDATE_NO_CONNECTION), szError = getLastSystemError()) );
   
   /// Connect to server
   else if (!(hFile = InternetOpenUrl(hConnection, szUpdateURL, NULL, NULL, NULL, NULL)))
      // [ERROR] "Unable to connect to update server - %s"
      pushErrorQueue(pErrorQueue, pError = generateDualError(HERE(IDS_UPDATE_CONNECTION_FAILED), szError = getLastSystemError()) );
   
   /// Read Update.txt
   else if (!InternetReadFile(hFile, szBufferA = utilCreateEmptyStringA(MAX_BUFFER), MAX_BUFFER, &iBytesRead))
      // [ERROR] "Unable to download update file - %s"
      pushErrorQueue(pErrorQueue, pError = generateDualError(HERE(IDS_UPDATE_DOWNLOAD_FAILED), szError = getLastSystemError()) );

   /// Parse Update.txt
   else if (generateXMLTree(szBufferW = translateUpdateFile(szBufferA, iBytesRead), iBytesRead, TEXT("Update.xml"), NULL, pXMLTree, pProgress, pErrorQueue) == OR_SUCCESS)
   {
      // Extract latest release node
      if (findXMLTreeNodeByName(pXMLTree->pRoot, TEXT("release"), TRUE, pReleaseNode) AND getXMLPropertyInteger(pReleaseNode, TEXT("version"), iLatestVersion))
      {
         // [CHECK] Is new verison available?
         if (pOperationData->bAvailable = iLatestVersion > getAppVersion())
         {
            // [NAME]
            if (findXMLTreeNodeByName(pReleaseNode, TEXT("name"), FALSE, pDataNode))
               pOperationData->szName = utilDuplicateSimpleString(pDataNode->szText);

            // [DATE]
            if (findXMLTreeNodeByName(pReleaseNode, TEXT("date"), FALSE, pDataNode))
               pOperationData->szDate = utilDuplicateSimpleString(pDataNode->szText);

            // [DESCRIPTION]
            if (findXMLTreeNodeByName(pReleaseNode, TEXT("description"), FALSE, pDataNode))
               generateRichTextFromSourceText(pDataNode->szText, lstrlen(pDataNode->szText), pOperationData->pDescription, RTT_RICH_TEXT, ST_DISPLAY, NULL);
               
            // [URL]
            if (findXMLTreeNodeByName(pReleaseNode, TEXT("download"), FALSE, pDataNode))
               pOperationData->szURL = utilDuplicateSimpleString(pDataNode->szText);
         }
      }      
   }

   // Cleanup
   InternetCloseHandle(hFile);
   InternetCloseHandle(hConnection);
   utilSafeDeleteString(szError);
   utilDeleteStringA(szBufferA);
   utilDeleteString(szBufferW);
   deleteXMLTree(pXMLTree);
   return (pError == NULL);
}



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     THREAD FUNCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : threadprocAutomaticUpdate
// Description     : Downloads update XML file
// 
// VOID*  pParameters   : UPDATE_OPERATION* : [in] Ignored
//
// Return type : Ignored
//
// Operation Stages : ONE
//
/// Operation Result : OR_SUCCESS - Update file downloaded successfully 
///                    OR_FAILURE - Unable to download file
//                     OR_ABORTED - Never
// 
BearScriptAPI
DWORD   threadprocAutomaticUpdate(VOID*  pParameter)
{
   UPDATE_OPERATION*   pOperationData = (UPDATE_OPERATION*)pParameter;      // Convenience pointer
   OPERATION_RESULT    eResult = OR_FAILURE;             

   __try
   {
      // [DEBUGGING]
      CONSOLE_COMMAND_BOLD();
      SET_THREAD_NAME("Automatic Update");
      setThreadLanguage(getAppPreferences()->eAppLanguage);

      // [INFO] "Checking for updates..."
      generateQueuedInformation(pOperationData->pErrorQueue, HERE(IDS_OUTPUT_DOWNLOADING_UPDATE));
      ASSERT(getOperationProgressStageID(pOperationData->pProgress) == IDS_PROGRESS_UPDATE_DOWNLOADING);

      /// Check for updates
      eResult = (downloadUpdate(pOperationData, pOperationData->pProgress, pOperationData->pErrorQueue) ? OR_SUCCESS : OR_FAILURE);
   }
   PUSH_CATCH(pOperationData->pErrorQueue)
   {
      // [FAILURE]
      eResult = OR_FAILURE;
   }

   // Cleanup and return result
   CONSOLE_COMPLETE("AUTOMATIC UPDATE", eResult);
   closeThreadOperation(pOperationData, eResult);
   return THREAD_RETURN;
}



