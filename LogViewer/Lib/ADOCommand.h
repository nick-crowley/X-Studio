#pragma once
#include "ADO.h"
#include "ADORecordset.h"

namespace Database
{

   class ADOCommand
   {
   public:
      ADOCommand(ADOConnection&  oConnection);
      virtual ~ADOCommand();
      
      void  AddParameter(const TCHAR*  szName, wstring  szValue);
      void  AddParameter(const TCHAR*  szName, BYTE  iValue);
      void  AddParameter(const TCHAR*  szName, SHORT  iValue);
      void  AddParameter(const TCHAR*  szName, UINT  iValue);
      ADORecordset  Execute(const LONG  iFlags = ADO::adCmdText);
      void          SetQuery(const TCHAR*  szSQL);

   protected:
      ADOConnection&    m_oConnection;
      ADO::_CommandPtr  m_pCommand;
   };

} // END: namespace Database