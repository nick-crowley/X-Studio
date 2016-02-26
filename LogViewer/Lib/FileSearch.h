#pragma once
#include <shlwapi.h>

namespace Utils
{

   class FileSearch 
   {
   public:
      FileSearch();
      ~FileSearch();

      bool     isValid();
      void     Next();
      void     Search(wstring  szFolder, wstring  szTerm);
      
      wstring  getFileName();
      wstring  getFullPath();
      bool     hasExtension(wstring  szExtension);
      bool     isFile();
      bool     isFolder();

   protected:
      bool     isRelative();

      WIN32_FIND_DATA  m_oResult;
      HANDLE           m_hSearch;
      wstring          m_szFolder;
   };

} // END: namespace Utils