#include "StdAfx.h"
#include "FileSearch.h"

namespace Utils
{

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                 CONSTRUCTION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////

FileSearch::FileSearch()
{
   ZeroMemory(&m_oResult, sizeof(WIN32_FIND_DATA));
   m_hSearch = INVALID_HANDLE_VALUE;
}

FileSearch::~FileSearch() 
{
   if (m_hSearch != INVALID_HANDLE_VALUE)
      FindClose(m_hSearch);
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       METHODS
/// /////////////////////////////////////////////////////////////////////////////////////////

void  FileSearch::Next()
{
   if (m_hSearch != INVALID_HANDLE_VALUE)
   {
      /// Retrieve next result
      if (!FindNextFile(m_hSearch, &m_oResult))
      {
         FindClose(m_hSearch);
         m_hSearch = INVALID_HANDLE_VALUE;
      }
      // [RELATIVE] Skip relative folders
      else if (isRelative())
         Next();
   }
}

bool  FileSearch::isValid()
{
   return m_hSearch != INVALID_HANDLE_VALUE;
}

void  FileSearch::Search(wstring  szFolder, wstring  szTerm)
{
   if (m_hSearch == INVALID_HANDLE_VALUE OR szFolder.empty() OR szTerm.empty())
   {
      // Ensure trailing backslash
      if (szFolder[szFolder.length() - 1] != '\\')
         szFolder.append(L"\\");

      // Store folder
      m_szFolder = szFolder;

      /// Perform search
      m_hSearch = FindFirstFile((szFolder + szTerm).c_str(), &m_oResult);

      // [RELATIVE] Skip relative folders
      while (isRelative())
         Next();
   }
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                       PROPERTIES
/// /////////////////////////////////////////////////////////////////////////////////////////

wstring  FileSearch::getFileName()
{
   return wstring(m_oResult.cFileName);
}

wstring  FileSearch::getFullPath()
{
   return m_szFolder + getFileName();
}

bool  FileSearch::hasExtension(wstring  szExtension)
{
   wstring  szFileName = getFileName();

   return szExtension == PathFindExtension(szFileName.c_str());
}

bool  FileSearch::isFile()
{
   return (m_oResult.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == NULL;
}

bool  FileSearch::isFolder()
{
   return (m_oResult.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != NULL;
}

bool  FileSearch::isRelative()
{
   return getFileName() == TEXT("..") || getFileName() == TEXT(".");
}

} // END: namespace Utils