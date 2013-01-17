//
// Scratch Pad.cpp : Testing code
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

VOID  mutex()
{
   /*pOperation->hMutex = CreateMutex(NULL, FALSE, NULL);

   WaitForSingleObject(pData->hMutex, INFINITE);
   pData->iProgress++;
   ReleaseMutex(pData->hMutex);*/
}


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       FUNCTIONS
/// /////////////////////////////////////////////////////////////////////////////////////////

BearScriptAPI
VOID  testCompression()
{
   //RAW_FILE     *szOutput,
   //             *szVerification;
   //ERROR_QUEUE  *pErrorQueue;
   ////CONST CHAR   *szInput = "The quick brown bear jumped over the lazy dog";
   //CONST CHAR   *szInput = "bear bear bear bear bear bear bear bear bear bear bear";
   //UINT          iOutputLength,
   //              iBytesWritten;
   //
   //// Prepare
   //szOutput       = NULL;
   //szVerification = NULL;
   //pErrorQueue    = createErrorQueue();

   //// Attempt to compress
   //if (iOutputLength = performGZipFileCompression(TEXT("Filename.txt"), (RAW_FILE*)szInput, lstrlenA(szInput), szOutput, pErrorQueue))
   //   /// [SUCCESS] Output file as ANSI
   //   iBytesWritten = saveRawFileToFileSystem(TEXT("c:\\temp\\CompressionTest.zip"), szOutput, iOutputLength, pErrorQueue);
   //
   //iBytesWritten = performGZipFileDecompression(szOutput, iOutputLength, szVerification, pErrorQueue);

   //// Cleanup
   //deleteErrorQueue(pErrorQueue);
}



//VOID  testListIterator()
//{
//   LIST_ITERATOR*  pIterator;
//
//   ////////////////////////////
//
//   pIterator = createIterator(pDocumentsData->pDocumentList);
//
//   while (getCurrentItem(pIterator, (LPARAM&)pDocument))
//   {
//      moveNextItem(pIterator);
//   }
//
//   deleteIterator(pIterator);
//
//   //////////////////////////
//   
//   for (pIterator = createIterator(pDocumentsData->pDocumentList); getCurrentItem(pIterator, (LPARAM&)pDocument); moveNextItem(pIterator))
//   {
//   }
//
//   deleteIterator(pIterator);
//
//   /////////////////////
//
//   pIterator = createIterator(pDocumentsData->pDocumentList);
//
//   do
//   {
//      getCurrentItem(pIterator, pExample);
//   } 
//   while (moveNextItem(pIterator));
//
//   deleteIterator(pIterator);
//}



BOOL  testPatternMatch(CONST TCHAR *szWild, CONST TCHAR *szInput)
{
   CONST TCHAR *cp = NULL, *mp = NULL;


   while ((szInput[0]) AND (szWild[0] != '*')) 
   {
      if ((szWild[0] != szInput[0]) AND (szWild[0] != '?')) 
         return FALSE;
   
      szWild++;
      szInput++;
   }

   while (szInput[0]) 
   {
      if (szWild[0] == '*') 
      {
         if (!*++szWild) 
            return TRUE;
         
         mp = szWild;
         cp = (szInput + 1);
      } 
      else if ((szWild[0] == szInput[0]) OR (szWild[0] == '?')) 
      {
         szWild++;
         szInput++;
      } 
      else 
      {
         szWild = mp;
         szInput = cp++;
      }
   }

   while (szWild[0] == '*') 
      szWild++;
   
   return !szWild[0];
}


BearScriptAPI
VOID  testStrangePCKFile()
{
   ERROR_QUEUE*  pErrorQueue;
   RAW_FILE     *szRawFile,
                *szDecompressed;
   UINT          iBytesRead,
                 iBytesDecompressed;

   CONST TCHAR*  szProblemFile = TEXT("C:\\Program Files (x86)\\EGOSOFT\\X3 Terran Conflict\\types\\TWareT.pck");
   CONST TCHAR*  szWorkingFile = TEXT("C:\\Program Files (x86)\\EGOSOFT\\X3 Terran Conflict\\t\\0002-L049.pck");

   // Prepare
   pErrorQueue = createErrorQueue();

   /// Read problem file
   if (iBytesRead = loadRawFileFromUnderlyingFileSystemByPath(szProblemFile, NULL, szRawFile, pErrorQueue))
   {
      /// Attempt to decompress
      if (iBytesDecompressed = performGZipFileDecompression(szRawFile, iBytesRead, szDecompressed, pErrorQueue))
         // Cleanup
         deleteRawFileBuffer(szDecompressed);

      /// [FAILED] Decrypt and try decompressing again
      else
      {
         /// [RESULT] WorkingFile == FILETYPE_DEFLATE
         ///          ProblemFile == FILETYPE_PCK

         

         CONST CHAR*  szType;
         TCHAR        iMagicNumber;

	      iMagicNumber = szRawFile[0] ^ 0xC8;

	      if((szRawFile[1] ^ iMagicNumber)==0x1F && (szRawFile[2] ^ iMagicNumber)==0x8B) 
         {
		      szType = "X2FD_FILETYPE_PCK";

            /// NEW: Decryption algorithm
            for (UINT i = 0; i < iBytesRead; i++)
               szRawFile[i] ^= iMagicNumber;
         }
      
	      else if (szRawFile[0] == 0x1F && szRawFile[1] == 0x8B)
		      szType = "X2FD_FILETYPE_DEFLATE";
	      else
		      szType = "X2FD_FILETYPE_PLAIN";

         // Decrypt
         //performDataFileDecryption(szRawFile, iBytesRead);

         // Try decompression again
         if (iBytesDecompressed = performGZipFileDecompression(&szRawFile[1], iBytesRead - 1, szDecompressed, pErrorQueue))
            // Cleanup
            deleteRawFileBuffer(szDecompressed);
      }

      // Cleanup
      deleteRawFileBuffer(szRawFile);
   }

   // Cleanup
   deleteErrorQueue(pErrorQueue);
}



VOID   functorTestMultiThreaded(AVL_TREE_NODE*  pNode, AVL_TREE_OPERATION*  pData)
{
   GAME_STRING*  pGameString;
   TCHAR*        szOutput;

   pGameString = (GAME_STRING*)pNode->pData;

   if (generateConvertedString(pGameString->szText, SPC_SCRIPT_DISPLAY_TO_EXTERNAL, szOutput))
      utilDeleteString(szOutput);
}


BOOL   testTree()
{
   //AVL_TREE_OPERATION* pOperation;
   //GAME_STRING*        pGameString;
   //AVL_TREE*           pTree;
   //DWORD               dwNow;

   //// Prepare
   //pTree      = createGameStringTreeByPageID();
   //pOperation = createAVLTreeOperation(functorTestMultiThreaded, ATT_INORDER);
   //dwNow      = GetTickCount();

   //pOperation->iTotalThreadCount = 4;

   //// Add 20,000 strings
   //for (UINT iPage = 0; iPage < 1000; iPage++)
   //{
   //   for (UINT iString = 0; iString < 30; iString++)
   //   {
   //      pGameString = createGameString(TEXT("while $patrol.sector.list < 4 something bongo INCLUDES ello % blah \" humpty dumpty [ % blah bingo meh hello doggy { WOO"), iString, iPage, ST_EXTERNAL);
   //      insertObjectIntoAVLTree(pTree, (LPARAM)pGameString);
   //   }
   //}

   //CONSOLE("Adding %u strings took %u ms", getTreeNodeCount(pTree), GetTickCount() - dwNow);
   //dwNow = GetTickCount();

   //// Perform threaded operation on the tree
   //performOperationOnAVLTree(pTree, pOperation);

   //CONSOLE("Operation on strings took %u ms", GetTickCount() - dwNow);

   //// Cleanup and Return
   //deleteAVLTreeOperation(pOperation);
   //deleteAVLTree(pTree);
   return TRUE;
}



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////



/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     RELEVANT X2FD stuff
/// /////////////////////////////////////////////////////////////////////////////////////////
//
//// 1. file is opened
////
//bool filebuffer::openFile(const char *pszName, int nAccess, int nCreateDisposition, int nFileType)
//{	
//	bool bRes;
//	
//	error(0);
//	
//	if(nFileType==X2FD_FILETYPE_AUTO)
//		nFileType = GetFileCompressionType(pszName);
//	
//	if(nFileType != X2FD_FILETYPE_PLAIN)
//		bRes = openFileCompressed(pszName, nAccess, nCreateDisposition, nFileType);
//	else
//		bRes = openFilePlain(pszName, nAccess, nCreateDisposition);
//	
//	return bRes;
//}
//
//// 2. Compression type is determined
////
//int GetFileCompressionType(const char *pszName)
//{
//	unsigned char data[3];
//	io64::file file=io64::file::open(pszName, _O_RDONLY WITH _O_BINARY);
//	if(file.valid()){
//		file.read(data, sizeof(data));
//		file.close();
//		return GetBufferCompressionType(data, sizeof(data));
//	}
//	else
//		return X2FD_FILETYPE_PLAIN;
//}
//
//int GetBufferCompressionType(const unsigned char *data, const io64::file::size& size)
//{
//	int type = -1;
//	if(size >= 3){
//		unsigned char magic=data[0] ^ 0xC8;
//		if((data[1] ^ magic)==0x1F && (data[2] ^ magic)==0x8B) 
//			type = X2FD_FILETYPE_PCK;
//	}
//	if(type == -1){
//		if(size >= 2 && (data[0] == 0x1F && data[1] == 0x8B))
//			type = X2FD_FILETYPE_DEFLATE;
//		else
//			type = X2FD_FILETYPE_PLAIN;
//	}	
//	return type;
//}
//
//// 3. [COMPRESSED] File is decompressed
////
//bool filebuffer::openFileCompressed(const char *pszName, int nAccess, int nCreateDisposition, int compressionType)
//{
//	bool bEmptyFile=false, bRes=true;
//	io64::file::size filesize;
//	
//	int omode=_O_BINARY, share = _SH_DENYNO;
//	if(nCreateDisposition == X2FD_CREATE_NEW) 
//		omode|=_O_CREAT;
//	if(nAccess==X2FD_READ) {
//		omode|=_O_RDONLY;
//		share=_SH_DENYWR;
//	}
//	else if(nAccess==X2FD_WRITE) {
//		omode|=_O_RDWR;
//		share=_SH_DENYRW;
//	}
//	file=io64::file::open(pszName, omode, _S_IREAD WITH _S_IWRITE, share);
//	if(!file.valid()){
//		error(fileerror(file.error()));
//		bRes=false;
//	}
//	bEmptyFile=((filesize=file.getSize()) == 0);
//	if(bRes){
//		// unpack file only if it's not empty
//		if(bEmptyFile==false){
//			byte *buffer;
//			io64::file::size size;
//			time_t mtime;
//         /// HERE: DecompressFile
//			error(DecompressFile(file, compressionType, &buffer, &size, &mtime));
//			if(bRes=(error() == 0 OR error() == X2FD_E_GZ_FLAGS)){
//				data(buffer, (size_t)size, (size_t)size);
//				this->mtime(mtime);
//				// close the file - it won't be needed in the future
//				if(nAccess!=X2FD_WRITE) {
//					file.close();
//				}
//			}
//		}
//		else{
//			io64::file::stat fs;
//			file.fstat(&fs);
//			mtime(fs.mtime);
//			dirty(true);
//		}
//      /// This is just setting the compression type flag
//		if(bRes){
//			strcreate(this->pszName, pszName);
//			if(compressionType == X2FD_FILETYPE_PCK)
//				type|=filebuffer::ISPCK;
//			else if(compressionType == X2FD_FILETYPE_DEFLATE)
//				type|=filebuffer::ISDEFLATE;
//			type|=filebuffer::ISFILE;
//			binarysize((size_t)filesize);
//		}
//	}
//	return bRes;
//}
//
//// 4. Decompression routine
////
//int DecompressFile(io64::file& file, int compressionMethod, unsigned char **out_data, io64::file::size *out_size, time_t  *mtime)
//{
//	int nRes;
//	unsigned char *data;
//	io64::file::size size=file.getSize();
//	data=new unsigned char[(size_t)size];
//	if(data==NULL)
//		return X2FD_E_MALLOC;
//	
//	file.read(data, size);
//	nRes = DecompressBuffer(data, size, compressionMethod, out_data, out_size, mtime);
//	delete[] data;
//	return nRes;
//}
//
//// 5. Decompression helper
////
//int DecompressBuffer(unsigned char *in_data, const io64::file::size& in_size, int compressionMethod, unsigned char **out_data, io64::file::size *out_size, time_t *mtime)
//{
//	if(in_size <= 0)
//		return X2FD_E_FILE_EMPTY;
//	
//	unsigned char magic=in_data[0] ^ 0xC8;
//	int nRes=0;
//	int offset = 0;
//
//   /// I think THIS is the special code!
//	
//	if(compressionMethod == X2FD_FILETYPE_PCK){
//		offset = 1;
//		for(size_t i=0; i < in_size; i++){
//			in_data[i] ^= magic;
//		}
//	}
//	gzreader gz;
//	bool bRes=gz.unpack(in_data + offset, (size_t)in_size - offset);
//	nRes = TranslateGZError(gz.error()); // set the error regardless of the return value
//	
//	if(bRes==false){
//		gz.flush();
//		*out_data=NULL;
//		*out_size=0;
//		*mtime=0;
//	}
//	else{
//		*out_data=gz.outdata();
//		*out_size=gz.outsize();
//		*mtime=gz.mtime();
//	}
//	return nRes;
//}
