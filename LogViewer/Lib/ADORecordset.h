#pragma once

#include "ADO.h"

namespace Database
{

   class ADORecordset
   {
   public:
      ADORecordset(ADOConnection&  oConnection);
      ADORecordset(ADO::_RecordsetPtr  pRecordset, ADOConnection&  oConnection);
      virtual ~ADORecordset(void);

      ADORecordset&  operator=(const ADORecordset&  oRS);

      void  Attach(ADO::_RecordsetPtr  pRecordset);
      bool  Open(const TCHAR*  szQuery, enum ADO::CursorTypeEnum  eCursorType, enum ADO::LockTypeEnum  eLockType, long  iFlags = ADO::adCmdText);
      bool  Close();
      bool  Move(const LONG  iRecords);
      bool  MoveNext();

      LONG     getValueByIndex(const LONG  iField);
      LONG     getValueByName(const TCHAR*  szField);
      wstring  getStringByIndex(const LONG  iField);
      wstring  getStringByName(const TCHAR*  szField);
      bool     isEOF();
      bool     isOpen();

   protected:
      ADOConnection&      m_oConnection;
      ADO::_RecordsetPtr  m_pRecordset;
   };

} // END: namespace Database