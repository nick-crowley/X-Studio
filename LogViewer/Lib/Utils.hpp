#pragma once

namespace Utils
{
   /// EmptyStruct: Empties a structure before use
   template <typename T>
   class EmptyStruct : public T
   {
   public:
      EmptyStruct()
      {
         ZeroMemory(dynamic_cast<T*>(this), sizeof(T));
      };
   };

   /// StringBuffer: Creates a temporary ANSI/UNICODE string buffer
   template <typename Type>
   class StringBuffer
   {
   public:
      StringBuffer(const UINT  iLength) : szBuffer(NULL) 
      { 
         szBuffer = new Type[iLength]; 
         szBuffer[0] = NULL;  
      };
      ~StringBuffer() 
      { 
         if (szBuffer) 
            delete [] szBuffer; 
      };

      // Methods: TCHAR* getBuffer()  +  wstring toString()
      Type*               getBuffer()  { return szBuffer;                      };
      basic_string<Type>  toString()   { return basic_string<Type>(szBuffer);  };

      // Casts: Access to above methods
      operator Type*()                 { return getBuffer();                   };
      operator basic_string<Type>()    { return toString();                    };

   protected:
      Type*   szBuffer;
   };

   // Convenience types
   typedef StringBuffer<TCHAR>  StringBufferW;
   typedef StringBuffer<CHAR>   StringBufferA;


   // Helper functions
   int      DisplayException(const CHAR*  szLocation, HWND  hParent, std::exception&  ex);
   void     Printf(const TCHAR*  szFormat, ...);

} // END: namespace Utils
