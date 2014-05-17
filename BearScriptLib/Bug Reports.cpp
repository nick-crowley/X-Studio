//
// Bug Reports.cpp : Bug submission thread
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

// Defines the size of the chunks in which submissions are uploaded
//
#define  UPLOAD_CHUNK_SIZE   (16 * 1024)

// Defines the server address and credentials
//
CONST TCHAR   *szServerHostName = TEXT("x-studio.fbnz.de"),
              *szServerUserName = TEXT("x-studio-bugs@fbnz.de"),
              *szServerPassword = TEXT("thereisnobear");

//#error X-Studio username/password has been intentionally removed.  Please see X-Universe member 'ScRaT_GER' regarding hosting details

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                        HELPERS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : generateBugReportFileName
// Description     : Generates a filename containing a timestamp
// 
// Return Value   : New filename string, you are responsible for destroying it
// 
TCHAR*   generateBugReportFileName(CONST SUBMISSION_OPERATION*  pOperationData)
{
   CONST TCHAR*   szFileName;     // Type of file
   SYSTEMTIME     oCurrentTime;   // Current time in GMT
   TCHAR*         szOutput;       // Complete filename

   // Preppare
   szOutput = utilCreateEmptyString(96);
   GetSystemTime(&oCurrentTime);
   
   // Generate filename
   szFileName = (pOperationData->eType == OT_SUBMIT_BUG_REPORT ? TEXT("Report") : TEXT("Correction"));
   StringCchPrintf(szOutput, 96, TEXT("%s.(%u-%02u-%02u).(%02uh.%02um.%02us).zip"), szFileName, oCurrentTime.wYear, oCurrentTime.wMonth, oCurrentTime.wDay, oCurrentTime.wHour, oCurrentTime.wMinute, oCurrentTime.wSecond);

   // Return result
   return szOutput;
}


/// Function name  : getBugReportServerResponse
// Description     : Retrieves the FTP server response
// 
// Return Value   : New Error string
// 
TCHAR*   getBugReportServerResponse()
{
   TCHAR*  szError = utilCreateEmptyString(256);
   DWORD   iErrorCode, iErrorLength = 256;
   
   // Get server response
   InternetGetLastResponseInfo(&iErrorCode, szError, &iErrorLength);
   return szError;
}


/// Function name  : getLastSystemError
// Description     : Formats the result of GetLastError()
// 
// Return Value   : New Error string
// 
TCHAR*   getLastSystemError()
{
   DWORD   dwError = GetLastError();
   TCHAR*  szError = utilCreateEmptyString(256);

   // Get system error
   FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwError, NULL, szError, 256, NULL);
   return szError;
}


/// Function name  : identifySubmissionOperationString
// Description     : Returns appropriate description string for operation
// 
// SUBMISSION_OPERATION*  pOperationData : [in] Submission operation data
// 
// Return Value   : Returns 'Bug report' or 'Correction'
// 
CONST TCHAR*  identifySubmissionOperationString(SUBMISSION_OPERATION*  pOperationData)
{
   return (pOperationData->eType == OT_SUBMIT_BUG_REPORT ? TEXT("bug report") : TEXT("correction"));
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : cleanSubmissionForumUserName
// Description     : Ensures than an X-Universe forum name doesn't contain any characters that the FTP server would reject
// 
// TCHAR*      szUserName     : [in] Buffer containing the username to clean
// CONST UINT  iBufferLength  : [in] Length of buffer
// 
BearScriptAPI
VOID   cleanSubmissionForumUserName(TCHAR*  szUserName, CONST UINT  iBufferLength)
{
   TCHAR*   szInputString;

   // Prepare
   szInputString = utilCreateStringf(32, TEXT("%s"), szUserName);
   utilZeroString(szUserName, iBufferLength);

   /// Iterate through input string
   for (UINT iInput = 0, iOutput = 0; iInput < iBufferLength; iInput++)
   {
      // [CHECK] Is the character alpha-numeric?
      if (IsCharAlphaNumeric(szInputString[iInput]))
         // [SUCCESS] Copy to output string
         szUserName[iOutput++] = szInputString[iInput];
   }

   // Cleanup
   utilDeleteString(szInputString);
}


/// Function name  : performBugReportCompression
// Description     : Compresses the 'Console.log' file into a GZip file
///                                 File is assumed to be in the application folder
// 
// SUBMISSION_OPERATION*  pOperationData : [in]  Operation data
// RAW_FILE*             &szOutput       : [out] New RawFile containing the GZip file
// 
// Return Value   : Length of compressed buffer, in bytes
// 
UINT  performBugReportCompression(SUBMISSION_OPERATION*  pOperationData, RAW_FILE*  &szOutput)
{
   TCHAR        *szApplicationPath,       // Full path of the application EXE
                *szConsolePath,           // Full path of 'Console.log' file
                *szFileName;              // Name of file within archive
   RAW_FILE     *szInput;                 // 'Console.log' file, uncompressed
   UINT          iFileSize;               // Size of 'Console.log' file, uncompressed
   
   
   // Prepare
   szApplicationPath  = utilCreateEmptyPath();
   szConsolePath      = utilCreateEmptyPath();
   szOutput           = NULL;

   // Get full path of 'Console.log'
   GetModuleFileName(NULL, szApplicationPath, MAX_PATH);
   szConsolePath = utilRenameFilePath(szApplicationPath, TEXT("Console.log"));

   // Generate archive filename
   szFileName = generateBugReportFileName(pOperationData);
   PathRenameExtension(szFileName, TEXT(".log"));

   /// Attempt to load console.log
   if ((iFileSize = loadRawFileFromFileSystemByPath(getFileSystem(), szConsolePath, NULL, szInput, pOperationData->pErrorQueue)) == NULL)
      // [ERROR] "The bug report '%s' is unavailable or could not be accessed"
      pushErrorQueue(pOperationData->pErrorQueue, generateDualError(HERE(IDS_SUBMISSION_LOADING_FAILED), szConsolePath));
   
   /// [SUCCESS] Attempt to compress console.log
   else if ((iFileSize = performGZipFileCompression(szFileName, szInput, iFileSize, szOutput, pOperationData->pProgress, pOperationData->pErrorQueue)) == NULL)
      // [ERROR] "There was an error during the preparation of the %s '%s'"
      pushErrorQueue(pOperationData->pErrorQueue, generateDualError(HERE(IDS_SUBMISSION_COMPRESSION_FAILED), identifySubmissionOperationString(pOperationData), szConsolePath));
   
   // Cleanup + return compressed size
   deleteRawFileBuffer(szInput);
   utilDeleteStrings(szApplicationPath, szConsolePath, szFileName);
   return iFileSize;
}


/// Function name  : performCorrectionReportCompression
// Description     : Compresses the contents of the correction into a new compressed RawFile
// 
// SUBMISSION_OPERATION*  pOperationData : [in]  Operation data
// RAW_FILE*             &szOutput       : [out] New RawFile containing the GZip file
//
// Return Value   : Length of compressed buffer, in bytes
// 
UINT  performCorrectionReportCompression(SUBMISSION_OPERATION*  pOperationData, RAW_FILE*  &szOutput)
{
   TCHAR*  szFileName;          // Name of file within archive
   UINT    iFileSize;           // Size of compressed buffer
   CHAR*   szCorrectionA;       // ANSI submission
   UINT    iCorrectionLength;   // Length of ANSI submission
   
   
   // Prepare
   szOutput  = NULL;
   iFileSize = NULL;

   // Convert correction to ANSI
   iCorrectionLength = lstrlen(pOperationData->szCorrection);
   szCorrectionA     = utilTranslateStringToANSI(pOperationData->szCorrection, iCorrectionLength, 115);

   // Generate archive filename
   szFileName = generateBugReportFileName(pOperationData);
   PathRenameExtension(szFileName, TEXT(".txt"));

   // [CHECK] Ensure conversion succeeded
   if (szCorrectionA)
      /// Attempt to compress submission buffer
      iFileSize = performGZipFileCompression(szFileName, (BYTE*)szCorrectionA, iCorrectionLength, szOutput, pOperationData->pProgress, pOperationData->pErrorQueue);

   // [CHECK] Abort on failure
   if (!szCorrectionA OR !iFileSize)
      // [ERROR] "There was an error during the preparation of the %s '%s'"
      pushErrorQueue(pOperationData->pErrorQueue, generateDualError(HERE(IDS_SUBMISSION_COMPRESSION_FAILED), identifySubmissionOperationString(pOperationData), TEXT("Correction.txt")));
   
   // Return size of compressed file
   utilDeleteString(szFileName);
   return iFileSize;
}


/// Function name  : performBugReportNavigation
// Description     : Navigates to the user's bug folder. If the folder does it not exist, it is created
// 
// HINTERNET            hConnection : [in]     Server connection handle
// CONST TCHAR*         szUserName  : [in]     User's X-Universe forums username
// OPERATION_PROGRESS*  pProgress   : [in/out] Operation progress
// ERROR_STACK*         pError      : [out]    Error if unsuccessful, otherwise NULL
// 
// Return Value   : TRUE if succesful, otherwise FALSE
// 
BOOL  performBugReportNavigation(HINTERNET  hConnection, CONST TCHAR*  szUserName, OPERATION_PROGRESS*  pProgress, ERROR_STACK*  &pError)
{
   TCHAR*  szServerResponse = NULL;    // Server response, for error reporting

   // Prepare
   pError = NULL;

   // [PROGRESS] Update progress (60%)
   updateOperationProgressValue(pProgress, 60);

   /// [NAVIGATE] Attempt to navigate to user's folder
   if (!FtpSetCurrentDirectory(hConnection, szUserName))
   {
      // [PROGRESS] Update progress (80%)
      updateOperationProgressValue(pProgress, 80);

      /// [CREATE] Attempt to create user's folder
      if (!FtpCreateDirectory(hConnection, szUserName))
         // [ERROR] "Unable to create submission server folder '%s' : '%s'"
         pError = generateDualError(HERE(IDS_SUBMISSION_CREATE_FOLDER_FAILED), szUserName, szServerResponse = getBugReportServerResponse());
      
      /// [RE-NAVIGATE] Attempt to navigate to user's folder
      else if (!FtpSetCurrentDirectory(hConnection, szUserName))
         // [ERROR] "Unable to navigate to submission server folder '%s' : '%s'"
         pError = generateDualError(HERE(IDS_SUBMISSION_NAVIGATE_FOLDER_FAILED), szUserName, szServerResponse = getBugReportServerResponse());
   }

   // Cleanup and return TRUE if there were no errors
   utilSafeDeleteString(szServerResponse);
   return (pError == NULL);
}



/// Function name  : performBugReportSubmission
// Description     : Connects to the bug submission server, creates the necessary folders, and uploads the bug report
// 
// CONST RAW_FILE*      szSubmissionFile : [in]     GZip file to be uploaded
// CONST UINT           iFileSize        : [in]     Length of GZip file, in bytes
// CONST TCHAR*         szUserName       : [in]     User's X-Universe forums username
// OPERATION_PROGRESS*  pProgress        : [in/out] Operation progress
// ERROR_QUEUE*         pErrorQueue      : [in/out] Error queue, may already contain errors
// 
// Return Value   : OR_SUCCESS : The file was uploaded successfully, there were no errors
//                  OR_FAILURE : An error prevented the file from being uploaded.
// 
OPERATION_RESULT  performBugReportSubmission(CONST RAW_FILE*  szSubmissionFile, CONST UINT  iFileSize, CONST TCHAR*  szUserName, SUBMISSION_OPERATION*  pOperationData)
{
   TCHAR         *szRemoteFileName,          // Remote Filename for the submission, timestamped to ensure it is unique
                 *szLastError = NULL;        // Last system or WinINet error
   ERROR_STACK   *pError = NULL;             // Operation error
   HINTERNET      hConnection,               // Server connection handle
                  hInternet;                 // Internet handle
   
   // [PROGRESS] Define progress manually
   updateOperationProgressMaximum(pOperationData->pProgress, 100);

   // [INFO] "Connecting to submission server"
   VERBOSE("Connecting to 'ftp://%s'", szServerHostName);
   pushErrorQueue(pOperationData->pErrorQueue, generateDualInformation(HERE(IDS_PROGRESS_CONNECTING_SERVER)));

   /// [INITIALISE] Initialise internet API for asynchronous operation
   if (!(hInternet = InternetOpen(getAppName(), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, NULL)))
      // [ERROR] "Unable to initialise internet connection : '%s'"
      pError = generateDualError(HERE(IDS_SUBMISSION_INIT_FAILED), szLastError = getLastSystemError());
   
   else
   {
      // [PROGRESS] Update progress (25%)
      updateOperationProgressValue(pOperationData->pProgress, 25);

      /// [CONNECT] Attempt to connect to submission server
      if (!(hConnection = InternetConnect(hInternet, szServerHostName, INTERNET_DEFAULT_FTP_PORT, szServerUserName, szServerPassword, INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, NULL)))
         // [ERROR] "Unable to connect to submission server : '%s'"
         pError = generateDualError(HERE(IDS_SUBMISSION_CONNECTION_FAILED), szLastError = getBugReportServerResponse());
      
      /// [NAVIGATE] Attempt to navigate to user's folder
      else if (performBugReportNavigation(hConnection, szUserName, pOperationData->pProgress, pError))
      {
         // [STAGE] Move to the upload stage
         ASSERT(advanceOperationProgressStage(pOperationData->pProgress) == IDS_PROGRESS_UPLOADING_REPORT);

         // Generate remote filename
         szRemoteFileName = generateBugReportFileName(pOperationData);

         // [INFO] "Transferring bug report..."
         VERBOSE("Uploading bug report '%s'", szRemoteFileName);
         pushErrorQueue(pOperationData->pErrorQueue, generateDualInformation(HERE(IDS_PROGRESS_UPLOADING_REPORT)));

         /// [TRANSFER] Upload file using the new filename
         performBugReportTransfer(hConnection, szSubmissionFile, iFileSize, szRemoteFileName, pOperationData->pProgress, pError);

         // Cleanup
         utilDeleteString(szRemoteFileName);
      }

      // Cleanup
      InternetCloseHandle(hConnection);
      InternetCloseHandle(hInternet);
   }

   /// [ERROR] Add to output error queue
   if (pError)
      pushErrorQueue(pOperationData->pErrorQueue, pError);

   // Cleanup and return FAILURE if there was an error, otherwise SUCCESS
   utilSafeDeleteString(szLastError);
   return (pError ? OR_FAILURE : OR_SUCCESS);
}


/// Function name  : performBugReportTransfer
// Description     : Uploads a GZip file (Stored in a buffer) into the current folder on the server
// 
// HINTERNET            hConnection      : [in]     Server connection handle
// CONST RAW_FILE*      szSubmissionFile : [in]     GZip file to be uploaded
// CONST UINT           iFileSize        : [in]     Length of GZip file, in bytes
// CONST TCHAR*         szRemoteFileName : [in]     Remote file name
// OPERATION_PROGRESS*  pProgress        : [in/out] Operation progress
// ERROR_STACK*         pError           : [out]    Error if unsuccessful, otherwise NULL
// 
// Return Value   : TRUE if successful, otherwise FALSE
// 
BOOL   performBugReportTransfer(HINTERNET  hConnection, CONST RAW_FILE*  szSubmissionFile, CONST UINT  iFileSize, CONST TCHAR*  szRemoteFileName, OPERATION_PROGRESS*  pProgress, ERROR_STACK*  &pError)
{
   UINT       iTotalBytesWritten = NULL,    // Total bytes uploaded
              iBytesWritten = NULL;         // Bytes uploaded in current chunk
   TCHAR*     szServerResponse = NULL;      // Server response, if upload failed
   HINTERNET  hFile = NULL;                 // File transfer handle

   // Prepare
   pError = NULL;
   
   // [PROGRESS] Define progress as number of bytes transferred
   updateOperationProgressMaximum(pProgress, iFileSize);

   /// Initiate File transfer
   if (!(hFile = FtpOpenFile(hConnection, szRemoteFileName, GENERIC_WRITE, FTP_TRANSFER_TYPE_BINARY, NULL)))
      // [ERROR] "Unable to upload report to submission server : '%s'"
      pError = generateDualError(HERE(IDS_SUBMISSION_UPLOAD_FAILED), szServerResponse = getBugReportServerResponse());
   else
   {
      // Iterate through file chunks, abort on error
      while (!pError AND iTotalBytesWritten < iFileSize)
      {
         /// Attempt to upload current chunk
         if (!InternetWriteFile(hFile, &szSubmissionFile[iTotalBytesWritten], min(UPLOAD_CHUNK_SIZE, iFileSize - iTotalBytesWritten), (DWORD*)&iBytesWritten))
            // [ERROR] "Unable to upload report to submission server : '%s'"
            pError = generateDualError(HERE(IDS_SUBMISSION_UPLOAD_FAILED), szServerResponse = getBugReportServerResponse());
         else
         {
            // [SUCCESS] Update
            iTotalBytesWritten += iBytesWritten;
            updateOperationProgressValue(pProgress, iTotalBytesWritten);
         }
      }
   }
   
   // Cleanup and return TRUE if successful
   utilSafeDeleteString(szServerResponse);
   InternetCloseHandle(hFile);
   return (pError == NULL);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     THREAD FUNCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : threadprocSubmitReport
// Description     : Compresses the bug/correction submission and uploads it to the bug submission server
// 
// VOID*  pParameters   : SUBMISSION_OPERATION structure containing:
//                            -> [BUGS]       Nothing 
//                            -> [CORRECTION] Full correction text
//
// Return type : Ignored
//
// Operation Stages : THREE  (Preparation, Connection, Submission)
//
/// Operation Result : OR_SUCCESS - The submission was successfully uploaded, there were no errors.
///                    OR_FAILURE - An error occurred and the submission was NOT successfully uploaded.
//                     OR_ABORTED - Never
// 
BearScriptAPI
DWORD   threadprocSubmitReport(VOID*  pParameter)
{
   SUBMISSION_OPERATION*   pOperationData;      // Convenience pointer
   OPERATION_RESULT        eResult;             // Operation result, defaults to SUCCESS
   ERROR_QUEUE*            pErrorQueue;         // convenience pointer
   RAW_FILE*               szSubmissionFile;    // Buffer containing compressed submission
   UINT                    iSubmissionSize;     // Size of compressed submission, in bytes

   __try
   {
      // [DEBUGGING]
      CONSOLE_COMMAND_BOLD();
      SET_THREAD_NAME("Submission Upload");
      setThreadLanguage(getAppPreferences()->eAppLanguage);

      // [CHECK] Ensure parameter exists
      ASSERT(pParameter);
      
      // Prepare
      pOperationData   = (SUBMISSION_OPERATION*)pParameter;
      pErrorQueue      = pOperationData->pErrorQueue;
      szSubmissionFile = NULL;
      eResult          = OR_FAILURE;

      // [INFO] "Preparing bug report..."
      VERBOSE("Preparing to upload %s report", identifySubmissionOperationString(pOperationData)); 
      pushErrorQueue(pErrorQueue, generateDualInformation(HERE(IDS_PROGRESS_PREPARING_REPORT)));

      // [PROGRESS] Ensure first stage is set
      ASSERT(getOperationProgressStageID(pOperationData->pProgress) == IDS_PROGRESS_PREPARING_REPORT);

      // [CHECK] Are we submitting a bug?
      if (pOperationData->eType == OT_SUBMIT_BUG_REPORT)
         /// [BUG] Attempt to load and compress Console.log
         iSubmissionSize = performBugReportCompression(pOperationData, szSubmissionFile);
      else
         /// [CORRECTION] Compress correction text
         iSubmissionSize = performCorrectionReportCompression(pOperationData, szSubmissionFile);

      // [CHECK] Was compression successful?
      if (iSubmissionSize)
      {
         // [STAGE] Move to the connection stage
         ASSERT(advanceOperationProgressStage(pOperationData->pProgress) == IDS_PROGRESS_CONNECTING_SERVER);

         /// [SUCCESS] Attempt to connect to submission server and upload compressed file
         eResult = performBugReportSubmission(szSubmissionFile, iSubmissionSize, getAppPreferences()->szForumUserName, pOperationData);
      
         // Delete submission file
         deleteRawFileBuffer(szSubmissionFile);
      }
   }
   PUSH_CATCH(pErrorQueue)
   {
      // [FAILURE]
      eResult = OR_FAILURE;
   }

   // Cleanup and return result
   CONSOLE_COMPLETE("BUG SUBMISSION", eResult);
   closeThreadOperation(pOperationData, eResult);
   return THREAD_RETURN;
}

