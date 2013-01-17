//
// Virtual File System Helpers.cpp : Helpers for the VFS -- direct disk access and decompression routines
//
// NB: Best viewed with tab size of 3 characters and Visual Studio's 'XML Doc Comment' syntax colouring
//         set to a colour that highly contrasts the 'C/C++ comment' syntax colouring
// 

#include "stdafx.h"
#include "zlib.h"

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          MACROS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Macro: extractWordFromByteBuffer
//  Description: Extracts a WORD from a byte buffer
//
// CONST BYTE*  pBuffer : [in] Byte buffer
//
// Return Value : WORD : Contents of the first two bytes
//
#define  extractWordFromByteBuffer(pBuffer)         (*((WORD*)(pBuffer)))

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                   CREATION / DESTRUCTION
/// ////////////////////////////////////////////////////////////////////////////////////////////////////


/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       HELPERS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : identifyDefaultFileExtension
// Description     : Identifies the default extension related to a specific file filter  (except 'All Files')
// 
// CONST FILE_FILTER  eFilter : [in] FileFilter
// 
// Return Value   : Default file extension, in '.abc' format
// 
CONST TCHAR*  identifyFileFilterSearchTerm(CONST FILE_FILTER  eFilter)
{
   CONST TCHAR*  szOutput;

   // Examine filter
   switch (eFilter)
   {
   /// [SCRIPT-XML] Uses .xml
   case FF_SCRIPT_XML_FILES:  szOutput = TEXT("*.xml");      break;

   /// [SCRIPT-PCK] Uses .pck 
   case FF_SCRIPT_PCK_FILES:  szOutput = TEXT("*.pck");      break;

   /// [PROJECTS] Uses .xprj
   case FF_PROJECT_FILES:     szOutput = TEXT("*.xprj");     break;

   /// [REMAINDER] Can be either .pck or .xml
   case FF_DOCUMENT_FILES: 
   case FF_LANGUAGE_FILES:    
   case FF_MISSION_FILES:     
   case FF_SCRIPT_FILES:      
   case FF_ALL_FILES:         
   default:                   szOutput = TEXT("*");          break;
   }

   // Return extension
   return szOutput;
}


/// Function name  : identifyVirtualSubFolderName
// Description     : Identify the first sub-folder of a file path
// 
// CONST TCHAR*  szFullPath    : [in]  FilePath to examine
// TCHAR*        szOutput      : [out] Resultant sub-folder name - if any
// CONST UINT    iOutputLength : [in]  Length of output buffer in characters
// 
// Return Value   : TRUE if filepath contains a sub-folder, FALSE otherwise
// 
//  test.txt           : no has sub-folder        
//  blog\henry.obj     : sub-folder is 'blog'
//  bill\ben\harry.txt : sub-folder is 'bill'. 'ben' is ignored.
//
BOOL  identifyVirtualSubFolderName(CONST TCHAR*  szSubPath, TCHAR*  szOutput, CONST UINT  iOutputLength)
{
   TCHAR*  szMarker;

   // Search for a backslash - delimits the first sub-folder
   if (szMarker = utilFindCharacter(szSubPath, '\\'))
      // Copy just the characters preceeding the backslash
      StringCchCopyN(szOutput, iOutputLength, szSubPath, (szMarker - szSubPath));
   
   // Return result
   return (szMarker != NULL);
}


/// Function name  : isPathSubFolderOf
// Description     : Determines whether a folder is equal to, or is a sub-folder of a specified path
// 
// CONST TCHAR*  szParentFolder  : [in] Path of the folder to test against
// CONST TCHAR*  szPath          : [in] Path of the folder to test
// 
// Return Value   : TRUE if 'szSubFolder' is a sub-folder of (or is equal to) 'szParentFolder'. Otherwise FALSE.
// 
BearScriptAPI
BOOL   isPathSubFolderOf(CONST TCHAR*  szParentFolder, CONST TCHAR*  szPath)
{
   // [CHECK] Is entire 'parent folder' path present in the 'sub folder' path?
   return utilCompareStringVariablesN(szPath, szParentFolder, lstrlen(szParentFolder));
}


/// Function name  : isVirtualFileInExactSubPath
// Description     : Determines whether an item in the VFS is exactly with the specified subpath  (ie. not a sub-folder, but exactly within that folder)
// 
// CONST TCHAR*  szFolderSubPath : [in] Subpath of the target folder
///                                          SubFolder: Must have a trailing backslash   RootFolder: Must be empty string
// CONST TCHAR*  szFileSubPath   : [in] SubPath of File/Folder to test
// 
// Return Value   : TRUE if file/folder is within target folder, otherwise FALSE.  (Also FALSE if szPath == szFolder)
// 
BOOL   isVirtualFileInExactSubFolder(CONST TCHAR*  szFolderSubPath, CONST TCHAR*  szFileSubPath)
{
   UINT  iFolderSubPathLength;
   BOOL  bResult;

   // [CHECK] Are we searching a subfolder?  ie. not the root
   if (szFolderSubPath[0])
   {
      // Prepare
      iFolderSubPathLength = lstrlen(szFolderSubPath);

      /// [SUB-FOLDER] Ensure entire subfolder is within filepath, and filepath contains no further backslashes
      bResult = utilCompareStringVariablesN(szFileSubPath, szFolderSubPath, iFolderSubPathLength) AND !utilFindCharacter(&szFileSubPath[iFolderSubPathLength], '\\');
   }
   else
      /// [ROOT] Ensure filepath contains no backslashes
      bResult = !utilFindCharacter(szFileSubPath, '\\');

   // Return result
   return bResult;
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : identifyFileTypeFromBuffer
// Description     : Identifies the type of a file from it's content, stored in a buffer
///                                       The file is truncated to 1,024 bytes
// 
// RAW_FILE*  szFileBuffer : [in]  Buffer to examine
// UINT      &iFileSize    : [in]  Length of buffer, in bytes
///                          [out] Truncated file size
// 
// Return Value   : File type if identified, otherwise FIF_NONE
// 
FILE_ITEM_FLAG   identifyFileTypeFromBuffer(RAW_FILE*  szFileBuffer, UINT  &iFileSize)
{
   FILE_ITEM_FLAG  eResult;     // Operation result
   CONST CHAR*     szBuffer;    // Convenience pointer

   // Prepare
   szBuffer = (CONST CHAR*)szFileBuffer;

   // Truncate file buffer to a maximum of 1024 bytes
   iFileSize = min(1024, iFileSize);
   szFileBuffer[iFileSize - 1] = NULL;

   /// [SCRIPTS] Search for the <SCRIPT> tag
   if (StrStrIA(szBuffer, "<script>"))
      eResult = FIF_SCRIPT;

   /// [MISSION DIRECTOR] Search for the beginning of the <DIRECTOR> tag
   else if (StrStrIA(szBuffer, "<director"))
      eResult = FIF_MISSION;
      
   /// [LANGUAGE FILES] Search for the beginning of the <LANGUAGE> tag
   else if (StrStrIA(szBuffer, "<language"))
      eResult = FIF_LANGUAGE;

   /// [PROJECT FILES] Search for the <PROJECT> tag
   else if (StrStrIA(szBuffer, "<project"))
      eResult = FIF_PROJECT;

   /// [GALAXY MAPS] Search for the <UNIVERSE> tag
   else if (StrStrIA(szBuffer, "<universe"))
      eResult = FIF_UNIVERSE;
      
   /// [CONVERSATIONS] Search for the beginning of the <CONVERSATIONS> tag
   else if (StrStrIA(szBuffer, "<conversations"))
      eResult = FIF_CONVERSATION;

   /// [UNKNOWN]
   else
      eResult = FIF_NONE;

   // Return result
   return eResult;
}


/// Function name  : identifyFileTypeAndProperties
// Description     : Determines the FileType of a FileItem. Also populates script properties for script FileItems
// 
// CONST FILE_SYSTEM*  pFileSystem : [in]     VirtualFileSystem object
// FILE_ITEM*          pFileItem   : [in/out] FileItem to examine
// 
VOID  identifyFileTypeAndProperties(CONST FILE_SYSTEM*  pFileSystem, FILE_ITEM*  pFileItem)
{
   CONST TCHAR*  szExtension;
   SCRIPT_FILE*  pScriptProperties;    // ScriptFile containing the properties of script FileItems
   ERROR_QUEUE*  pErrorQueue;          // File I/O ErrorQueue - ignored
   RAW_FILE*     szFileBuffer;         // File buffer
   UINT          iFileSize;            // Length of file buffer, in bytes

   // Prepare
   szExtension = PathFindExtension(pFileItem->szFullPath);

   // [CHECK] Ensure file is XML or PCK
   if ((~pFileItem->iAttributes INCLUDES FIF_FOLDER) AND (utilCompareString(szExtension, ".pck") OR utilCompareString(szExtension, ".xml") OR utilCompareString(szExtension, ".xprj")))
   {
      // Prepare
      pErrorQueue = createErrorQueue();

      // Read in file
      if (iFileSize = loadRawFileFromFileSystemByPath(pFileSystem, pFileItem->szFullPath, NULL, szFileBuffer, pErrorQueue))
      {
         /// [COMPRESSED FILES] Set the unpacked filesize
         //if (pFileItem->iAttributes INCLUDES FIF_VIRTUAL)
         pFileItem->iFileSize = iFileSize;

         /// Identify file type from content
         pFileItem->iAttributes |= identifyFileTypeFromBuffer(szFileBuffer, iFileSize);

         // Examine file type
         switch (extractFileItemType(pFileItem->iAttributes))
         {
         /// [SCRIPT] Set display type + script properties
         case FIF_SCRIPT:
            // Set display type
            StringCchCopy(pFileItem->szDisplayType, 80, TEXT("X³ MSCI Script"));

            // Generate ScriptFile from buffer
            pScriptProperties = createScriptFileFromFile(pFileItem->szFullPath, (RAW_FILE*)szFileBuffer, iFileSize);
            
            /// Extract script properties
            if (parseScriptPropertiesFromBuffer(pScriptProperties))
            {
               // Store script/game version
               pFileItem->iFileVersion = pScriptProperties->iVersion;
               pFileItem->eGameVersion = pScriptProperties->eGameVersion;

               // Manually force to the appropriate game version icon into the system ImageList
               pFileItem->iIconIndex = getSystemImageListGameVersionIcon(pFileItem->eGameVersion);

               // Store description if present
               if (pScriptProperties->szDescription)
                  StringCchCopy(pFileItem->szDescription, 128, pScriptProperties->szDescription);
            }
            else
               // [FAILED] Use the script document icon
               pFileItem->iIconIndex = getSystemImageListDocumentTypeIcon(FIF_SCRIPT);

            // Cleanup
            deleteScriptFile(pScriptProperties);
            break;

         /// [LANGUAGE] Set display type and icon
         case FIF_LANGUAGE:      
            StringCchCopy(pFileItem->szDisplayType, 80, TEXT("X³ Language File"));  
            pFileItem->iIconIndex = getSystemImageListDocumentTypeIcon(FIF_LANGUAGE);
            break;

         /// [MISSION] Set display type and icon
         case FIF_MISSION:       
            StringCchCopy(pFileItem->szDisplayType, 80, TEXT("X³ Mission Script")); 
            pFileItem->iIconIndex = getSystemImageListDocumentTypeIcon(FIF_MISSION);
            break;

         /// [PROJECT] Set display type
         case FIF_PROJECT:       
            StringCchCopy(pFileItem->szDisplayType, 80, TEXT("X-Studio Project"));  
            pFileItem->iIconIndex = getSystemImageListDocumentTypeIcon(FIF_PROJECT);
            break;

         /// [UNIVERSE] Set display type
         case FIF_UNIVERSE:      StringCchCopy(pFileItem->szDisplayType, 80, TEXT("X³ Galaxy Map"));  break;
         /// [CONVERSATION] Set display type
         case FIF_CONVERSATION:  StringCchCopy(pFileItem->szDisplayType, 80, TEXT("X³ Conversations File"));   break;
         }

         // Cleanup file buffer
         deleteRawFileBuffer(szFileBuffer);
      }
      else
         consolePrintError(lastErrorQueue(pErrorQueue));

      // Cleanup
      deleteErrorQueue(pErrorQueue);
   }
}


/// Function name  : loadCatalogueFile
// Description     : Attempts to load and decrypt a catalogue file
// 
// CONST TCHAR*  szFullPath : [in]  Full file path of the catalogue to load
// RAW_FILE*    &szOutput   : [out] Decrypted buffer containing the catalogue contents
// 
// Return Value   : Number of bytes read if succesful, otherwise NULL
// 
UINT   loadCatalogueFile(CONST TCHAR*  szFullPath, RAW_FILE*  &szOutput)
{
   ERROR_QUEUE*  pErrorQueue;
   UINT          iBytesRead;

   // Prepare
   pErrorQueue = createErrorQueue();

   // Load catalogue
   if (iBytesRead = loadRawFileFromUnderlyingFileSystemByPath(szFullPath, NULL, szOutput, pErrorQueue))
      // Decrypt buffer
      performCatalogueFileDecryption(PathFindFileName(szFullPath), szOutput, iBytesRead);

   // Cleanup
   deleteErrorQueue(pErrorQueue);
   return iBytesRead;
}


/// Function name  : loadRawFileFromUnderlyingFileSystemByPath
// Description     : Load either a physical or virtual file into an ANSI buffer.
//                    * To load a physical file specify NULL for of the 'pVirtualFile'.
//                    * To load a virtual file you must provide the path to the data file as the FilePath and 
//                        a VirtualFile object to identify the size and location of the file
///                                   NOTE: Will reject files with UTF-16 byte ordering
// 
// CONST TCHAR*  szFullPath   : [in]           FilePath of the file to load. For virtual files this should indicate the correct data file
// VIRTUAL_FILE* pVirtualFile : [in][optional] VirtualFile indicating the position and size of the virtual file
// RAW_FILE*    &szOutput     : [out]          New ANSI byte buffer if succesful, otherwise NULL.
// ERROR_QUEUE*  pErrorQueue  : [in/out]       Error message, if any
// 
// Return Value   : Number of bytes read if succesful, otherwise NULL
// 
BOOL   loadRawFileFromUnderlyingFileSystemByPath(CONST TCHAR*  szFullPath, CONST VIRTUAL_FILE*  pVirtualFile, RAW_FILE*  &pOutputBuffer, ERROR_QUEUE*  pErrorQueue)
{
   ERROR_STACK*  pError;
   RAW_FILE*     pInputBuffer;      // ANSI buffer to hold the file
   HANDLE        hFile;             // File handle
   TCHAR*        szSystemError,     // String version of 'GetLastError'
                 pByteOrdering[3];  // First three bytes of the file, used for checking byte ordering heading
   DWORD         iFileSize,         // File size in bytes (excluding the byte ordering header)
                 iBytesRead,        // Number of bytes read in a read operation
                 iErrorCode;        // System error code, if any

   // [TRACK]
   TRACK_FUNCTION();

   // Prepare
   iErrorCode    = NULL;
   iBytesRead    = NULL;
   iFileSize     = NULL;
   pInputBuffer  = NULL;
   pOutputBuffer = NULL;
   pError        = NULL;

   // [VERBOSE]
   if (pVirtualFile)
      VERBOSE("Loading virtual file '%s' from game catalogue '%s'", pVirtualFile->szFullPath, szFullPath);
   else
      VERBOSE("Loading physical file '%s'", szFullPath);

   /// Open file
   hFile = CreateFile(szFullPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

   // [CHECK] File opened OK?
   if (hFile == INVALID_HANDLE_VALUE)
      iErrorCode = GetLastError();
   else
   {
      /// Determine physical/virtual file size and create input buffer
      iFileSize    = (!pVirtualFile ? GetFileSize(hFile, NULL) : pVirtualFile->iPackedSize);
      pInputBuffer = createRawFileBuffer(iFileSize + 1);

      // [VIRTUAL FILES] Seek to the correct position within the data file
      if (pVirtualFile)
         SetFilePointer(hFile, pVirtualFile->iDataOffset, NULL, FILE_BEGIN);

      // [CHECK] Ensure file isn't empty
      if (!iFileSize)
         // [ERROR] "Cannot read from the file '%s' because it contains no data"
         pushErrorQueue(pErrorQueue, pError = generateDualError(HERE(IDS_FILE_EMPTY), szFullPath));

      /// [BYTE ORDER MARK] Check the first three bytes for a BOM
      else if (!ReadFile(hFile, (VOID*)pByteOrdering, 3, &iBytesRead, NULL))
         iErrorCode = GetLastError();

      // [UTF-16] Unsupported for the moment
      else if (utilCompareMemory(pByteOrdering, iByteOrderingUTF16_BE, 2) OR utilCompareMemory(pByteOrdering, iByteOrderingUTF16_LE, 2))
         // [ERROR] "Cannot read the file '%s' because it is using the currently unsupported UTF-16 byte ordering 0x%02X, 0x%02X"
         pushErrorQueue(pErrorQueue, pError = generateDualError(HERE(IDS_FILE_UTF16_UNSUPPORTED), szFullPath, (UINT)pByteOrdering[0], (UINT)pByteOrdering[1]));

      else
      {
         // [UTF-8] Read the remaining file normally
         if (utilCompareMemory(pByteOrdering, iByteOrderingUTF8, 3))
            iFileSize -= 3;
         // [NONE] Re-Seek to the to the start of the (virtual/physical) file
         else
            SetFilePointer(hFile, (pVirtualFile ? pVirtualFile->iDataOffset : 0), NULL, FILE_BEGIN);
   
         /// Attempt to read file
         if (!ReadFile(hFile, pInputBuffer, iFileSize, &iBytesRead, NULL))
            iErrorCode = GetLastError();
         else 
            // Null terminate input buffer buffer
            pInputBuffer[iBytesRead] = NULL;
      }
         
      // Close file
      utilCloseHandle(hFile);
   }

   /// If there was any kind of error then delete the input buffer (if it exists) 
   if (iErrorCode OR pError)
   {
      // Cleanup
      if (pInputBuffer)
         deleteRawFileBuffer(pInputBuffer);

      // [SYSTEM ERROR] Convert the system error into an error queue message
      if (!pError)
      {
         szSystemError = utilCreateEmptyString(ERROR_LENGTH);
         // Get system error and add to queue
         FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, iErrorCode, NULL, szSystemError, ERROR_LENGTH, NULL);
         pushErrorQueue(pErrorQueue, generateDualError(HERE(IDS_LOAD_FILE_SYSTEM_ERROR), szFullPath, szSystemError));
         // Cleanup
         utilDeleteString(szSystemError);
      }
   }
   
   /// Set output and return
   pOutputBuffer = pInputBuffer;
   END_TRACKING();
   return iBytesRead;
}


/// Function name  : performCatalogueFileDecryption  (Code converted from "X2FD" by doubleshadow)
// Description     : Decrypt an X3 catalogue file stored in a buffer by XOR'ing each byte in a cyclical pattern
//
// CONST TCHAR*  szFileName    : [in]     FileName, used for verbose reporting
// RAW_FILE*     szInput       : [in/out] Buffer containing an encrypted catalogue file, this will be decrypted in-place
// CONST UINT    iInputLength  : [in]     Length of input buffer, in bytes
// 
VOID  performCatalogueFileDecryption(CONST TCHAR*  szFileName, RAW_FILE  *szInput, CONST UINT  iInputLength)
{
	RAW_FILE  szDecryptionKeys[5]={ 0xDB, 0xDC, 0xDD, 0xDE, 0xDF };      /// Decryption keys identified by DoubleShadow

   // [VERBOSE]
   //VERBOSE("Decrypting game catalogue '%s'", szFileName);
	
   // Iterate through each byte in the input stream
   for (UINT iByte = 0, iKey = 0; iByte < iInputLength; iByte++)
   {
      // Move to the next decryption key
      iKey = (iByte % 5);
      // XOR the current character with the current decryption key
      szInput[iByte] ^= szDecryptionKeys[iKey];
      // Increase the current decryption key by 5
      szDecryptionKeys[iKey] += 5;
   }
}


/// Function name  : performRawFileDecryption
// Description     : Decrypts and/or Decompresses a .PCK file stored in a buffer
// 
// CONST TCHAR*          szFileName    : [in]           FileName, used for verbose reporting
/// RAW_FILE*           &szInputOutput : [in/out] [in]  Buffer containing encrypted/compressed .PCK file
///                                    :          [out] Buffer containing decrypted/decompressed plain text
// CONST UINT            iInputSize    : [in]           Size of the buffer on input
// CONST FILE_ITEM_FLAG  eFileSource   : [in]           Whether the file was stored in PHYSICAL or VIRTUAL file system
// ERROR_QUEUE*          pErrorQueue   : [in/out]       Error message if any. May already contain errors.
// 
// Return Value   : Size of the output buffer in bytes if successful, otherwise NULL
// 
UINT   performRawFileDecryption(CONST TCHAR*  szFileName, RAW_FILE*  &szInputOutput, CONST UINT  iInputSize, CONST FILE_ITEM_FLAG  eFileSource, ERROR_QUEUE*  pErrorQueue)
{
   RAW_FILE*  szDecompressed;       // Decompression result buffer
   UINT       iOutputSize;          // Output length in bytes
   BYTE       iDecryptionKey;       // Decryption key calculated from encrypted GZIP header
   WORD       iDecryptionKeyWord;   // Convenience value for expressing decryption key as a WORD

   // [TRACK]
   TRACK_FUNCTION();

   // Calculate decryption key from first byte
   iDecryptionKey     = (szInputOutput[0] ^ ENCRYPTED_GZIP_KEY);
   iDecryptionKeyWord = MAKEWORD(iDecryptionKey, iDecryptionKey);

   // Set default output length to input length
   iOutputSize = iInputSize;

   /// [CHECK] Is this an encrypted GZIP archive?   (Are the 2nd and 3rd bytes an encrypted GZIP header?)
   if ((extractWordFromByteBuffer(&szInputOutput[1]) ^ iDecryptionKeyWord) == GZIP_MAGIC_NUMBER)
   {
      // [VERBOSE]
      VERBOSE("Decompressing encrypted GZip archive '%s'", szFileName);

      // [DECRYPT] Decrypt remaining buffer using the decryption key above
      for (UINT iByte = 1; iByte < iInputSize; iByte++)
         // XOR each byte with decryption key
         szInputOutput[iByte] ^= iDecryptionKey;

      // [EXTRACT] Decompress remaining buffer into a new buffer
      iOutputSize = performGZipFileDecompression(&szInputOutput[1], (iInputSize - 1), szDecompressed, pErrorQueue);

      // Replace input buffer with new buffer
      deleteRawFileBuffer(szInputOutput);
      szInputOutput = szDecompressed;
   }
   // Examine the source of the file
   else switch (eFileSource)
   {
   /// [VIRTUAL] Decrypt all virtual files using a special decryption key
   case FIF_VIRTUAL:
      // [VERBOSE]
      VERBOSE("Decrypting virtual file '%s'", szFileName);

      // [DECRYPT] Decrypt entire using a hard-coded key
      for (UINT iByte = 0; iByte < iInputSize; iByte++)
         // XOR each byte with decryption key
         szInputOutput[iByte] ^= ENCRYPTED_PLAINTEXT_KEY;

      // Fall through...

   /// [PHYSICAL/VIRTUAL] Decompress file if required
   case FIF_PHYSICAL:
      // [CHECK] Is the file a GZIP archive?
      if (extractWordFromByteBuffer(szInputOutput) == GZIP_MAGIC_NUMBER)
      {
         // [VERBOSE]
         VERBOSE("Decompressing GZip archive '%s'", szFileName);

         // [EXTRACT] Decompress entire buffer into a new buffer
         iOutputSize = performGZipFileDecompression(szInputOutput, iInputSize, szDecompressed, pErrorQueue);

         // Replace input buffer with new buffer
         deleteRawFileBuffer(szInputOutput);
         szInputOutput = szDecompressed;
      }
   }

   // Return length of output buffer
   END_TRACKING();
   return iOutputSize;
}

#define     GZIP_CHUNK_SIZE    (32 * 1024)

/// Function name  : performGZipFileCompression
// Description     : Compress a buffer into a GZIP file
// 
// RAW_FILE*            szInput     : [in]     Input buffer to compress
// CONST UINT           iInputSize  : [in]     Size of the input buffer, in bytes
// RAW_FILE*           &szOutput    : [out]    New buffer containing the entire GZip file if successful, otherwise NULL
// OPERATION_PROGRESS*  pProgress   : [in/out] Progress object
// ERROR_QUEUE*         pErrorQueue : [in/out] Error message, if any
// 
// Return Value   : Length of GZip file buffer if successful, otherwise NULL
// 
UINT   performGZipFileCompression(CONST TCHAR*  szFileName, RAW_FILE*  szInput, CONST UINT  iInputSize, RAW_FILE*  &szOutput, OPERATION_PROGRESS*  pProgress, ERROR_QUEUE*  pErrorQueue)
{
   ERROR_STACK *pError;               // Operation error, if any
   GZIP_FILE    oFileData;            // Helper for holding all GZip file info
   z_stream     oDeflateData;         // Z-Lib compressor data
   INT          iResultCode,          // Z-Lib operation result code
                iCompressionMode;     // Z-Lib compressor parameter
   UINT         iFileNameLength,      // Filename length
                iOutputSize;          // Length of resultant GZip file
   RAW_FILE    *szMarker;             // Defines the current position within the output buffer
   TCHAR       *szErrorMessageW;      // UNICODE version of the error returned by Z-Lib (if any)
   CHAR        *szFileNameA;          // ANSI version of the input filename
	
   // [TRACK]
   TRACK_FUNCTION();

   // [CHECK] Ensure output does not exist
   ASSERT(!szOutput);

   // Prepare
   utilZeroObject(&oFileData, GZIP_FILE);
   utilZeroObject(&oDeflateData, z_stream);
   iOutputSize = NULL;
   pError      = NULL;

   // [PROGRESS]
   updateOperationProgressMaximum(pProgress, iInputSize);

   // Generate ANSI filename
   iFileNameLength  = lstrlen(szFileName);
   szFileNameA      = utilTranslateStringToANSI(szFileName, iFileNameLength);
   iFileNameLength += 1;      // Account for null terminator

   /// Create output buffer large enough to hold entire input buffer
   szOutput = createRawFileBuffer(GZIP_HEADER_SIZE + iFileNameLength + iInputSize + GZIP_FOOTER_SIZE);
   szMarker = szOutput;

   // Populate header and footer
   oFileData.oHeader.iCompressionType = GCT_DEFLATE;
   oFileData.oHeader.iMagicNumber     = GZIP_MAGIC_NUMBER;
   oFileData.oHeader.iFlags           = GF_FILENAME;
   oFileData.oFooter.iFileCRC         = crc32(crc32(0, NULL, 0), szInput, iInputSize);
   oFileData.oFooter.iUnPackedSize    = iInputSize;
   
   /// Output header
   utilCopyBuffer(szMarker, &oFileData.oHeader, GZIP_HEADER_SIZE);
   szMarker += GZIP_HEADER_SIZE;

   /// Output filename
   utilCopyBuffer(szMarker, szFileNameA, iFileNameLength);
   szMarker += iFileNameLength;

   // Define input buffer
   oDeflateData.next_in   = szInput;
   oDeflateData.avail_in  = iInputSize;

   // Define compression buffer
   oDeflateData.next_out  = szMarker;
   oDeflateData.avail_out = iInputSize;

   /// Initialise raw compression operation, set maximum compression and 
   if ((iResultCode = deflateInit2(&oDeflateData, Z_BEST_COMPRESSION, Z_DEFLATED, -15, 8, Z_DEFAULT_STRATEGY)) != Z_OK)
   {
      // [ERROR] "The file cannot be compressed due to an internal Z-Lib error. The error code was %d - '%s'."
      szErrorMessageW = utilTranslateStringToUNICODE(oDeflateData.msg, lstrlenA(oDeflateData.msg));
      pushErrorQueue(pErrorQueue, generateDualError(HERE(IDS_GZIP_ZLIB_DEFLATE_ERROR), iResultCode, szErrorMessageW));

      // Cleanup
      utilDeleteString(szErrorMessageW);
   }
   else 
   {
      /// Attempt to compress input directly into the output buffer
      do
      {
         // [INPUT] Define next whole chunk if available, otherwise partial chunk
         oDeflateData.next_in   = &szInput[oDeflateData.total_in];
         oDeflateData.avail_in  = min(GZIP_CHUNK_SIZE, iInputSize - oDeflateData.total_in);

         // [OUTPUT] Use remaining space in the output buffer
         oDeflateData.next_out  = &szMarker[oDeflateData.total_out];
         oDeflateData.avail_out = (iInputSize - oDeflateData.total_out);

         // [CHECK] Is this the last chunk?
         iCompressionMode = (oDeflateData.total_in + oDeflateData.avail_in != iInputSize ? Z_NO_FLUSH : Z_FINISH);

         /// Attempt to compress current chunk
         iResultCode = deflate(&oDeflateData, iCompressionMode);

         // [PROGRESS] Update progress
         updateOperationProgressMaximum(pProgress, oDeflateData.total_in);

         // [CHECK] Ensure compression was successful
         if (iCompressionMode == Z_NO_FLUSH AND iResultCode != Z_OK
             OR iCompressionMode == Z_FINISH AND iResultCode != Z_STREAM_END)
         {
            // [ERROR] "The file cannot be compressed due to an internal Z-Lib error. The error code was %d - '%s'."
            szErrorMessageW = utilTranslateStringToUNICODE(oDeflateData.msg, lstrlenA(oDeflateData.msg));
            pushErrorQueue(pErrorQueue, pError = generateDualError(HERE(IDS_GZIP_ZLIB_DEFLATE_ERROR), iResultCode, szErrorMessageW));

            // Cleanup and abort
            utilDeleteString(szErrorMessageW);
            break;
         }  
      }
      /// [LOOP] Abort processing once all data has been processed
      while (oDeflateData.total_in != iInputSize);

      // [CHECK] Ensure compression was successful
      if (!pError)
      {
         // Determine position of footer
         szMarker += oDeflateData.total_out;

         /// Copy footer to output
         utilCopyBuffer(szMarker, &oFileData.oFooter, GZIP_FOOTER_SIZE);

         // Calculate real output size
         iOutputSize = GZIP_HEADER_SIZE + iFileNameLength + oDeflateData.total_out +  GZIP_FOOTER_SIZE;
      }

      // Cleanup
      deflateEnd(&oDeflateData);
   }

   // [ERROR] Destroy output buffer
   if (iOutputSize == 0)
      deleteRawFileBuffer(szOutput);

   // Cleanup and return output size
   utilDeleteObject(szFileNameA);
   END_TRACKING();
   return iOutputSize;
}


/// Function name  : performGZipFileDecompression
// Description     : Decompress a GZIP file stored in a buffer
// 
// RAW_FILE*     szInput     : [in]     Input buffer containing the entire GZIP file, including the header
// CONST UINT    iInputSize  : [in]     Size of the input buffer, in bytes
// RAW_FILE*    &szOutput    : [out]    New buffer containing the uncompressed data from the file, or NULL if unsuccesful
// ERROR_QUEUE*  pErrorQueue : [in/out] Error message, if any
// 
// Return Value   : Number of bytes copied, or NULL if unsuccesful
// 
UINT   performGZipFileDecompression(RAW_FILE*  szInput, CONST UINT  iInputSize, RAW_FILE*  &szOutput, ERROR_QUEUE*  pErrorQueue)
{
   z_stream   oInflateData;         // Z-Lib decompressor data
   GZIP_FILE  oFileData;            // Contents of the GZip file specified in the input buffer
   UINT       iBytesDecompressed,   // Number of bytes decompressed
              iOutputCRC;           // CRC of the bytes decompressed
   INT        iOperationResult;     // Z-Lib decompression operation result code
   RAW_FILE  *szMarker,             // Input buffer iterator
             *szInputEnd;           // Convenience pointer marking the end of the input buffer
   TCHAR     *szErrorMessageW;      // UNICODE version of the error returned by Z-Lib (if any)
	
	// Prepare
   szOutput = NULL;
   utilZeroObject(&oFileData, GZIP_FILE);
   utilZeroObject(&oInflateData, z_stream);

   // Create convenience pointers
   szMarker   = szInput;
   szInputEnd = (szInput + iInputSize);

   /// Extract header
   utilCopyBuffer(&oFileData.oHeader, szMarker, GZIP_HEADER_SIZE);      /// [BUG] sizeof(GZIP_HEADER) returns 12 instead of 10
   szMarker += GZIP_HEADER_SIZE; 

   // [CHECK] Confirm it's a GZIP buffer
   if (oFileData.oHeader.iMagicNumber != GZIP_MAGIC_NUMBER)
   {  
      // [ERROR] "The file cannot be decompressed because it does not appear to be a valid GZIP file"
      generateQueuedError(pErrorQueue, HERE(IDS_GZIP_MAGIC_NUMBER));
      return NULL;
   }
   // Confirm valid compression type
   else if (oFileData.oHeader.iCompressionType != GCT_DEFLATE)
   {  // [ERROR] "The file cannot be decompressed because it's compression algoritm is not recognised"
      generateQueuedError(pErrorQueue, HERE(IDS_GZIP_COMPRESSION_TYPE));
      return NULL;
   }
	
   /// Extract footer
   utilCopyBuffer(&oFileData.oFooter, (szInputEnd - GZIP_FOOTER_SIZE), GZIP_FOOTER_SIZE);
   szInputEnd -= GZIP_FOOTER_SIZE;

   // [EXTRA] Skip any custom data, if present
   if (oFileData.oHeader.iFlags INCLUDES GF_EXTRA)
   {
      utilCopyObject(&oFileData.iExtraSize, szMarker, WORD);
      szMarker += (sizeof(WORD) + oFileData.iExtraSize);
   }
   // [FILENAME] Store the ANSI filename, if present
   if (oFileData.oHeader.iFlags INCLUDES GF_FILENAME)
   {
      oFileData.szFilename = (CONST CHAR*)szMarker;
      szMarker += (lstrlenA(oFileData.szFilename) + 1);
   }
   // [COMMENT] Store the ANSI comment, if present
   if (oFileData.oHeader.iFlags INCLUDES GF_COMMENT)
   {
      oFileData.szComment = (CONST CHAR*)szMarker;
      szMarker += (lstrlenA(oFileData.szComment) + 1);
   }
   // [HEADER CRC] Skip, if present
	if(oFileData.oHeader.iFlags INCLUDES GF_HEADER_CRC)
      szMarker += sizeof(WORD);
   
   // Create output buffer
   szOutput = createRawFileBuffer(oFileData.oFooter.iUnPackedSize);

   /// Set input and output buffer ('szMarker' now indicates the start of the compressed data stream)
   oInflateData.next_in   = szMarker;
   oInflateData.next_out  = szOutput;
   // Calculate input size and set output size
   oInflateData.avail_in  = (szInputEnd - szMarker);
   oInflateData.avail_out = oFileData.oFooter.iUnPackedSize;
	
   /// Inform ZLIB we've processed the header ourselves and decompress the buffer
   iOperationResult = inflateInit2(&oInflateData, -15);
   if (iOperationResult == Z_OK)
	   iOperationResult = inflate(&oInflateData, Z_FINISH);

   // Extract size and close decompressor
	iBytesDecompressed = oInflateData.total_out;
	inflateEnd(&oInflateData);
	
   // [CHECK] Abort if there were compression errors
   if (iOperationResult != Z_STREAM_END)
   {
      // [ERROR] The file cannot be decompressed due to an internal Z-Lib error. The error code was %d - '%s'.
      szErrorMessageW = utilTranslateStringToUNICODE(oInflateData.msg, lstrlenA(oInflateData.msg));
      pushErrorQueue(pErrorQueue, generateDualError(HERE(IDS_GZIP_ZLIB_INFLATE_ERROR), iOperationResult, szErrorMessageW));

      // Cleanup
      utilDeleteString(szErrorMessageW);
      deleteRawFileBuffer(szOutput);
      return FALSE;  
   }

   /// CRC decompressed data
   iOutputCRC = crc32(crc32(0, NULL, 0), szOutput, iBytesDecompressed);

   // [CHECK] Abort if CRC doesn't match the one specified in the file header
   if (oFileData.oFooter.iFileCRC != iOutputCRC)
   {  
      // [ERROR] The file cannot be decompressed because the CRC in the header 0x08%x does not match the CRC of the result 0x%08x
      pushErrorQueue(pErrorQueue, generateDualError(HERE(IDS_GZIP_CRC_FAILURE), oFileData.oFooter.iFileCRC, iOutputCRC));
      // Cleanup and return
      deleteRawFileBuffer(szOutput);
      return FALSE;
   }
   
   // Return number of bytes decompressed (ie. the size of the output file)
	return iBytesDecompressed;
}


/// Function name  : performFileItemFilterMatch
// Description     : Determines whether a FileItem matches the specified file filter
// 
// CONST FILE_ITEM*   pFileItem : [in] FileItem to match against the specified filter
// CONST FILE_FILTER  eFilter   : [in] Filter to match the specified FileItem against
// 
// Return Value   : TRUE if file matches filter, otherwise FALSE
// 
BOOL   performFileItemFilterMatch(CONST FILE_ITEM*  pFileItem, CONST FILE_FILTER  eFilter)
{
   BOOL   bResult;

   /// [FOLDER] Always display folders
   if (pFileItem->iAttributes INCLUDES FIF_FOLDER)
      bResult = TRUE;
   
   // [FILE] Determine whether the attributes match the filter
   else switch (eFilter)
   {
   /// [SCRIPT XML/PCK] Check for script attribute, and possibly file extension
   case FF_SCRIPT_PCK_FILES:  bResult = (pFileItem->iAttributes INCLUDES FIF_SCRIPT) AND utilCompareString(PathFindExtension(pFileItem->szFullPath), ".pck");     break;
   case FF_SCRIPT_XML_FILES:  bResult = (pFileItem->iAttributes INCLUDES FIF_SCRIPT) AND utilCompareString(PathFindExtension(pFileItem->szFullPath), ".xml");     break;

   /// [SCRIPT/LANGUAGE/MISSION/PROJECT] Check for appropriate attribute
   case FF_SCRIPT_FILES:      bResult = pFileItem->iAttributes INCLUDES FIF_SCRIPT;     break;
   case FF_LANGUAGE_FILES:    bResult = pFileItem->iAttributes INCLUDES FIF_LANGUAGE;   break;
   case FF_MISSION_FILES:     bResult = pFileItem->iAttributes INCLUDES FIF_MISSION;    break;
   case FF_PROJECT_FILES:     bResult = pFileItem->iAttributes INCLUDES FIF_PROJECT;    break;

   /// [DOCUMENT] Check for any attribute
   case FF_DOCUMENT_FILES:    bResult = pFileItem->iAttributes INCLUDES (FIF_SCRIPT WITH FIF_LANGUAGE WITH FIF_MISSION WITH FIF_PROJECT); break;

   /// [ALL FILES] Include regardless
   case FF_ALL_FILES:         bResult = TRUE;      break;
   }

   return bResult;
}


/// Function name  : performPhysicalFileSystemSearch
// Description     : Searches the virtual files within a FileSystem for files in the specified path that match the specified
//                    filter, convert them into FileItems and add them to the specified FileSearch object.
// 
// CONST FILE_SYSTEM*  pFileSystem  : [in/out]        FileSystem object containing the files to be searched
// CONST TCHAR*        szFolder     : [in]            Path to search. Must be the game folder or one of it's sub-folders.
// OPERATION_PROGRESS* pProgress    : [in] [optional] Operation progress
// CONST FILE_FILTER   eFilter      : [in]            Filter to filter results by
// FILE_SEARCH*        pOutput      : [out]           FileSearch object to fill with search results
// 
VOID  performPhysicalFileSystemSearch(CONST FILE_SYSTEM*  pFileSystem, CONST TCHAR*  szFolder, CONST FILE_FILTER  eFilter, OPERATION_PROGRESS*  pProgress, FILE_SEARCH*  pOutput)
{
   AVL_TREE_OPERATION*  pOperationData;      // Search operation data
   AVL_TREE*            pPotentialResults;   // Copy tree containing FileItems of all potential results
   WIN32_FIND_DATA      oResultData;         // File search result data
   HANDLE               hSearchHandle;       // File search handle
   TCHAR*               szSearchTerm;        // Search term for the physical file search
   BOOL                 bMoreFiles;          // FileSearch iterator flag

   // [TRACK]
   TRACK_FUNCTION();

   // Prepare
   utilZeroObject(&oResultData, WIN32_FIND_DATA);
   pPotentialResults = createAVLTree(extractFileItemTreeNode, NULL, createAVLTreeSortKey(AK_PATH, AO_DESCENDING), NULL, NULL);
   szSearchTerm      = utilCreateEmptyPath();
   bMoreFiles        = TRUE;

   // Generate the search term : "<folder>\*<extension>"
   StringCchCopy(szSearchTerm, MAX_PATH, szFolder);
   PathAddBackslash(szSearchTerm);
   StringCchCat(szSearchTerm, MAX_PATH, TEXT("*"));

   /// [PHYSICAL] Search for all physical files in the specified folder
   for (hSearchHandle = FindFirstFile(szSearchTerm, &oResultData); bMoreFiles AND (hSearchHandle != INVALID_HANDLE_VALUE); bMoreFiles = FindNextFile(hSearchHandle, &oResultData))
   {
      // [CHECK] Ensure file isn't a relative path or system file
      if (utilCompareString(oResultData.cFileName, ".") OR
          utilCompareString(oResultData.cFileName, "..") OR
          oResultData.dwFileAttributes INCLUDES FILE_ATTRIBUTE_SYSTEM)
          continue;

      // [CHECK] Ensure file extension matches search filter
      if (utilIncludes(oResultData.dwFileAttributes, FILE_ATTRIBUTE_DIRECTORY) OR PathMatchSpec(oResultData.cFileName, identifyFileFilterSearchTerm(eFilter)))
         /// [SUCCESS] Generate FileItem and add to temporary tree
         insertObjectIntoAVLTree(pPotentialResults, (LPARAM)createFileItemFromPhysical(szFolder, &oResultData));
   }

   // [CHECK] Is progress reporting required?
   if (pProgress)
      // [PROGRESS] Define progress as number of files found
      updateOperationProgressMaximum(pProgress, getTreeNodeCount(pPotentialResults));

   /// Perform operation multi-threaded 
   pOperationData = createAVLTreeThreadedOperation(treeprocSearchPhysicalFileSystem, ATT_INORDER, pProgress, 8);

   // Set properties
   pOperationData->xFirstInput = eFilter;
   pOperationData->pOutputTree = pOutput->pResultsTree;

   /// Perform search operation
   performOperationOnAVLTree(pPotentialResults, pOperationData);

   // Cleanup
   FindClose(hSearchHandle);
   deleteAVLTree(pPotentialResults);
   deleteAVLTreeOperation(pOperationData);
   utilDeleteString(szSearchTerm);
   END_TRACKING();
}


/// Function name  : performVirtualFileSystemSearch
// Description     : Searches the virtual files within a FileSystem for files in the specified path that match the specified
//                    filter, convert them into FileItems and add them to the specified FileSearch object.
// 
// CONST FILE_SYSTEM* pFileSystem  : [in/out] FileSystem object containing the files to be searched
// CONST TCHAR*       szFolder     : [in]     Path to search. Must be the game folder or one of it's sub-folders.
// CONST FILE_FILTER  eFilter      : [in]     Filter to filter results by
// FILE_SEARCH*       pOutput      : [out]    FileSearch object to fill with search results
// 
VOID  performVirtualFileSystemSearch(CONST FILE_SYSTEM*  pFileSystem, CONST TCHAR*  szFolder, CONST FILE_FILTER  eFilter, OPERATION_PROGRESS*  pProgress, FILE_SEARCH*  pOutput)
{
   AVL_TREE_OPERATION*   pOperationData;
   TCHAR*                szSearchFolder;

   // [CHECK] Input folder is a game-subfolder
   TRACK_FUNCTION();
   ASSERT(isPathSubFolderOf(pFileSystem->szGameFolder, szFolder));

   // Prepare
   szSearchFolder = utilCreateEmptyPath();

   /// Generate search folder: RootFolder==Empty string, SubFolder==Has trailing backslash   
   StringCchCopy(szSearchFolder, MAX_PATH, &szFolder[lstrlen(pFileSystem->szGameFolder)]);
   if (lstrlen(szSearchFolder))
      PathAddBackslash(szSearchFolder);

   // [PROGRESS] Define progress as number of files in VFS tree
   if (pProgress)
      updateOperationProgressMaximum(pProgress, getTreeNodeCount(pFileSystem->pFilesTree));

   /// Perform multi-threaded 
   pOperationData = createAVLTreeThreadedOperation(treeprocSearchVirtualFileSystem, ATT_INORDER, pProgress, 8);
 
   // Setup operation data
   pOperationData->pOutputTree  = pOutput->pResultsTree;
   pOperationData->xFirstInput  = (LPARAM)szSearchFolder; 
   pOperationData->xSecondInput = eFilter;

   /// Perform operation
   performOperationOnAVLTree(pFileSystem->pFilesTree, pOperationData);
   
   // Cleanup
   utilDeleteString(szSearchFolder);
   deleteAVLTreeOperation(pOperationData);
   END_TRACKING();
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                   TREE OPERATIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : treeprocSearchVirtualFileSystem
// Description     : Determines whether the VirtualFile at the current node matches the requested path and filter.
//                    If the node matches it's converted into a new FileItem and added to the output tree.
//                    It also detects any sub-folders within the search folder and converts them into new FileItems also.
// 
// AVL_TREE_NODE*       pCurrentNode   : [in]     Current node containing a VirtualFile
// AVL_TREE_OPERATION*  pOperationData : [in/out] Operation Data : 
///                                                 FILE_FILTER  xFirstInput  : [in]  Search result filter
///                                                 AVL_TREE*    pOutputTree  : [out] Output tree
// 
VOID  treeprocSearchPhysicalFileSystem(AVL_TREE_NODE*  pCurrentNode, AVL_TREE_OPERATION*  pOperationData)
{
   CONST TCHAR*   szExtension;    // File Extension of current file
   FILE_ITEM*     pCurrentFile;   // File/Folder being processed
   FILE_FILTER    eFilter;        // [FILES] File type filter

   // Extract parameters
   pCurrentFile = extractPointerFromTreeNode(pCurrentNode, FILE_ITEM);
   eFilter      = (FILE_FILTER)pOperationData->xFirstInput;

   // Determine file extension
   szExtension = PathFindExtension(pCurrentFile->szDisplayName);

   // [CHECK] Ensure file isn't a catalogue used by the VFS
   if ((utilCompareString(szExtension, ".cat") OR utilCompareString(szExtension, ".dat")) AND isCatalogueFilePresent(getFileSystem(), pCurrentFile->szFullPath))  
      /// [CATALOGUE] Destroy result
      deleteFileItem(pCurrentFile);
   else
   {
      // Extract file type and if appropriate, script properties
      identifyFileTypeAndProperties(getFileSystem(), pCurrentFile);         // Skipped if not PCK/XML

      // Use extended properties to determine whether FileItem matches filter
      if (performFileItemFilterMatch(pCurrentFile, eFilter))
      {
         // [SYNC] Wait for output tree
         if (pOperationData->bMultiThreaded)
            WaitForSingleObject(pOperationData->hOutputMutex, INFINITE);

         /// [MATCH] Insert into output tree
         insertObjectIntoAVLTree(pOperationData->pOutputTree, (LPARAM)pCurrentFile);

         // [SYNC] Release output tree
         if (pOperationData->bMultiThreaded)
            ReleaseMutex(pOperationData->hOutputMutex);
      }
      else
         /// [FAILED] Destroy result
         deleteFileItem(pCurrentFile);
   }
}


/// Function name  : treeprocSearchVirtualFileSystem
// Description     : Determines whether the VirtualFile at the current node matches the requested path and filter.
//                    If the node matches it's converted into a new FileItem and added to the output tree.
//                    It also detects any sub-folders within the search folder and converts them into new FileItems also.
// 
// AVL_TREE_NODE*       pCurrentNode   : [in]     Current node containing a VirtualFile
// AVL_TREE_OPERATION*  pOperationData : [in/out] Operation Data : 
///                                                CONST TCHAR*   xFirstInput  : [in]  SubPath of folder to search:  SubFolders must have a trailing backslash, RootFolder must be empty string
///                                                FILE_FILTER    xSecondInput : [in]  Search result filter
///                                                AVL_TREE*      pOutputTree  : [out] Output tree
// 
VOID  treeprocSearchVirtualFileSystem(AVL_TREE_NODE*  pCurrentNode, AVL_TREE_OPERATION*  pOperationData)
{
   CONST TCHAR   *szSearchFolder;       // SubPath of folder to search, empty string indicates main game folder.
   VIRTUAL_FILE  *pCurrentFile;         // File/Folder being processed
   FILE_FILTER    eFilter;              // [FILES] File type filter
   FILE_ITEM     *pSearchResult;        // Successful match, if any

   // Extract parameters
   pCurrentFile    = extractPointerFromTreeNode(pCurrentNode, VIRTUAL_FILE);
   szSearchFolder  = (CONST TCHAR*)pOperationData->xFirstInput;
   eFilter         = (FILE_FILTER)pOperationData->xSecondInput;
   
   // [CHECK] Ensure current file is within search folder
   if (isVirtualFileInExactSubFolder(szSearchFolder, pCurrentFile->szFullPath))
   {
      // Generate FileItem
      pSearchResult = createFileItemFromVirtualFile(pCurrentFile);

      /// [FILE] Filter result based on file contents
      if (~pCurrentFile->iAttributes INCLUDES FIF_FOLDER)
      {
         // [PCK/XML] Extract extended properties
         identifyFileTypeAndProperties(getFileSystem(), pSearchResult);       // Skipped if file is not PCK/XML

         // [FILTERED] Filter based on extended properties
         if (eFilter != FF_ALL_FILES AND !performFileItemFilterMatch(pSearchResult, eFilter))
            // [FAILED] Cleanup
            deleteFileItem(pSearchResult);
      }

      // [CHECK] Ensure item is a match
      if (pSearchResult)
      {
         // [SYNC] Wait for output tree
         if (pOperationData->bMultiThreaded)
            WaitForSingleObject(pOperationData->hOutputMutex, INFINITE);

         /// [MATCH] Add result to output tree
         if (!insertObjectIntoAVLTree(pOperationData->pOutputTree, (LPARAM)pSearchResult))
            // [DUPLICATE] A physical file with this name already exists (and takes precedence)
            deleteFileItem(pSearchResult);

         // [SYNC] Release output tree
         if (pOperationData->bMultiThreaded)
            ReleaseMutex(pOperationData->hOutputMutex);
      }
   }
}

/// ////////////////////////////////////////////////////////////////////////////////////////////////////
///                                          THREAD FUNCTIONS
/// ////////////////////////////////////////////////////////////////////////////////////////////////////

/// Function name  : threadprocFileSystemSearch
// Description     : 
// 
// VOID*  pParameter : [in] 
// 
// Return Value   : 
// 
BearScriptAPI
DWORD   threadprocFileSystemSearch(VOID*  pParameter)
{
   SEARCH_OPERATION*  pOperationData;

   // [DEBUGGING]
   TRACK_FUNCTION();
   SET_THREAD_NAME("File System Search");

   // [CHECK] Ensure parameter exists
   ASSERT(pParameter);

   // Prepare
   pOperationData = (SEARCH_OPERATION*)pParameter;

   // [COM]
   CoInitializeEx(NULL, COINIT_MULTITHREADED);

   // [STAGE] Define progress stage
   ASSERT(getOperationProgressStageID(pOperationData->pProgress) == IDS_PROGRESS_SEARCHING_PHYSICAL);

   /// Destroy previous FileSearch
   if (pOperationData->pFileSearch)
      deleteFileSearch(pOperationData->pFileSearch);

   /// Search the VirtualFileSystem for items matching the specified filter and folder.
   pOperationData->pFileSearch = performFileSystemSearch(getFileSystem(), pOperationData->szFolder, pOperationData->eFilter, pOperationData->eSortKey, pOperationData->eDirection, pOperationData->pProgress);
   
   // [DEBUG] Separate previous output from further output for claritfy
   VERBOSE_THREAD_COMPLETE("FILE SEARCH WORKER THREAD COMPLETED");

   // [COM]
   CoUninitialize();

   // Cleanup and return
   closeThreadOperation(pOperationData, OR_SUCCESS);
   END_TRACKING();
   return THREAD_RETURN;
}
