#include "StdAfx.h"
#include "ADOCommand.h"

namespace Database
{

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                 CONSTRUCTION / DESTRUCTION
   /// /////////////////////////////////////////////////////////////////////////////////////////

   ADOCommand::ADOCommand(ADOConnection&  oConnection) : m_oConnection(oConnection)
   { 
      m_pCommand.CreateInstance("ADODB.Command"); 
   }

   ADOCommand::~ADOCommand(void)
   {
   }

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                       METHODS
   /// /////////////////////////////////////////////////////////////////////////////////////////

   void  ADOCommand::SetQuery(const TCHAR*  szSQL)
   { 
      try
      {
         //m_pCommand->CommandType = ADO::adCmdStoredProc;
         //m_pCommand->NamedParameters = true;

         m_pCommand->CommandType = ADO::adCmdText;
         m_pCommand->CommandText = szSQL; 
      }
      catch (_com_error&  ex) 
      {
         throw ADOException(HERE, ex, m_oConnection);
      }
   }


   void  ADOCommand::AddParameter(const TCHAR*  szName, wstring  szValue)
   {
      try
      {
         ADO::_ParameterPtr pParameter = m_pCommand->CreateParameter(szName, ADO::adBSTR, ADO::adParamInput, szValue.length() * sizeof(TCHAR), _bstr_t(szValue.c_str()) );
         m_pCommand->Parameters->Append(pParameter);
      }
      catch (_com_error&  ex) 
      {
         throw ADOException(HERE, ex, m_oConnection);
      }
   }


   void  ADOCommand::AddParameter(const TCHAR*  szName, BYTE  iValue)
   {
      try
      {
         ADO::_ParameterPtr pParameter = m_pCommand->CreateParameter(szName, ADO::adTinyInt, ADO::adParamInput, sizeof(BYTE), _variant_t((CHAR)iValue) );
         m_pCommand->Parameters->Append(pParameter);
      }
      catch (_com_error&  ex) 
      {
         throw ADOException(HERE, ex, m_oConnection);
      }
   }


   void  ADOCommand::AddParameter(const TCHAR*  szName, SHORT  iValue)
   {
      try
      {
         ADO::_ParameterPtr pParameter = m_pCommand->CreateParameter(szName, ADO::adSmallInt, ADO::adParamInput, sizeof(SHORT), _variant_t((SHORT)iValue) );
         m_pCommand->Parameters->Append(pParameter);
      }
      catch (_com_error&  ex) 
      {
         throw ADOException(HERE, ex, m_oConnection);
      }
   }


   void  ADOCommand::AddParameter(const TCHAR*  szName, UINT  iValue)
   {
      try
      {
         ADO::_ParameterPtr pParameter = m_pCommand->CreateParameter(szName, ADO::adInteger, ADO::adParamInput, sizeof(UINT), _variant_t((LONG)iValue) );
         m_pCommand->Parameters->Append(pParameter);
      }
      catch (_com_error&  ex)
      {
         throw ADOException(HERE, ex, m_oConnection);
      }
   }


   ADORecordset  ADOCommand::Execute(const LONG  iFlags)
   {
      try
      {
         //ADORecordset  oRecordset(m_oConnection);
         ADO::_RecordsetPtr  pRecordset;

         /// Set connection and execute
         m_pCommand->ActiveConnection = m_oConnection.m_pConnection;
         pRecordset = m_pCommand->Execute(NULL, NULL, iFlags);

         // Return resultant RS
         return ADORecordset(pRecordset, m_oConnection);
      }
      catch (_com_error&  ex)
      {
         throw ADOException(HERE, ex, m_oConnection);
      }
   }

} // END: namespace Database
