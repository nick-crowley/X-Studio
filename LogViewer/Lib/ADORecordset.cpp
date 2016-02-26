#include "StdAfx.h"
#include "ADORecordset.h"

namespace Database
{
      
   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                 CONSTRUCTION / DESTRUCTION
   /// /////////////////////////////////////////////////////////////////////////////////////////

   ADORecordset::ADORecordset(ADOConnection&  oConnection) : m_oConnection(oConnection)
   {
      m_pRecordset.CreateInstance(TEXT("ADODB.Recordset"));
   }

   ADORecordset::ADORecordset(ADO::_RecordsetPtr  pRecordset, ADOConnection&  oConnection) : m_oConnection(oConnection)
   {
      m_pRecordset = pRecordset;
   }


   ADORecordset::~ADORecordset(void)
   {
 
   }

   ADORecordset&  ADORecordset::operator=(const ADORecordset&  oRS)
   {
      // Copy connection. Replace Recordset.
      m_oConnection = oRS.m_oConnection;
      m_pRecordset  = oRS.m_pRecordset;

      return *this;
   }
   
   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                       METHODS
   /// /////////////////////////////////////////////////////////////////////////////////////////

   void  ADORecordset::Attach(ADO::_RecordsetPtr  pRecordset)
   {
      // Attach new recordset
      if (!m_pRecordset)
         m_pRecordset = pRecordset;
   }

   bool  ADORecordset::Open(const TCHAR*  szQuery, enum ADO::CursorTypeEnum  eCursorType, enum ADO::LockTypeEnum  eLockType, long  iFlags) 
   {
      try
      {
         // [CHECK] Ensure connection is open + Recordset is not
         if (!m_oConnection.isConnected() OR isOpen())
            return false;

         /// Open Recordset
         m_pRecordset->PutRefActiveConnection(m_oConnection.m_pConnection);
         m_pRecordset->Open(szQuery, vtMissing, eCursorType, eLockType, iFlags);
         return true;
      }
      catch (_com_error&  ex) {
         throw ADOException(HERE, ex, m_oConnection);
      }
      catch (ADOException&) {
         throw;
      }
   }


   bool  ADORecordset::Close() 
   {
      try
      {
         // [CHECK] Ensure Recordset is open
         if (!isOpen())
            return false;
         
         /// Close Recordset
         m_pRecordset->Close();
         return true;
      }
      catch (_com_error&  ex) {
         throw ADOException(HERE, ex, m_oConnection);
      }
      catch (ADOException&) {
         throw;
      }
   }


   bool  ADORecordset::Move(const LONG  iRecords) 
   {
      try
      {
         /// Move Recordset
         m_pRecordset->Move(iRecords);
         return true;
      }
      catch (_com_error&  ex) {
         throw ADOException(HERE, ex, m_oConnection);
      }
   }


   bool  ADORecordset::MoveNext() 
   {
      try
      {
         /// Move Recordset
         m_pRecordset->MoveNext();
         return true;
      }
      catch (_com_error&  ex) {
         throw ADOException(HERE, ex, m_oConnection);
      }
   }


   LONG  ADORecordset::getValueByIndex(const LONG  iField) 
   {
      try
      {
         return m_pRecordset->Fields->Item[iField]->Value;
      }
      catch (_com_error&  ex) {
         throw ADOException(HERE, ex, m_oConnection);
      }
   }


   LONG  ADORecordset::getValueByName(const TCHAR*  szField) 
   {
      try
      {
         return m_pRecordset->Fields->Item[szField]->Value;
      }
      catch (_com_error&  ex) {
         throw ADOException(HERE, ex, m_oConnection);
      }
   }


   wstring  ADORecordset::getStringByIndex(const LONG  iField) 
   {
      try
      {
         return (const TCHAR*)(_bstr_t)m_pRecordset->Fields->Item[iField]->Value;
      }
      catch (_com_error&  ex) {
         throw ADOException(HERE, ex, m_oConnection);
      }
   }


   wstring  ADORecordset::getStringByName(const TCHAR*  szField) 
   {
      try
      {
         return (const TCHAR*)(_bstr_t)m_pRecordset->Fields->Item[szField]->Value;
      }
      catch (_com_error&  ex) {
         throw ADOException(HERE, ex, m_oConnection);
      }
   }


   bool  ADORecordset::isEOF() 
   {
      try 
      {
         /// Query Recordset
         return m_pRecordset->EndOfFile ? true : false;
      }
      catch (_com_error&  ex) {
         throw ADOException(HERE, ex, m_oConnection);
      }
   }


   bool  ADORecordset::isOpen() 
   {
      try
      {
         return m_pRecordset AND (m_pRecordset->State INCLUDES ADO::adStateOpen);
      }
      catch (_com_error&  ex) {
         throw ADOException(HERE, ex, m_oConnection);
      }
   }


} // END: namespace Database