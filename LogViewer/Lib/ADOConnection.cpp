#include "StdAfx.h"
#include "ADOConnection.h"

namespace Database
{
   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                       ADO EXCEPTION
   /// /////////////////////////////////////////////////////////////////////////////////////////

   ADOException::ADOException(const TCHAR*  szLocation, _com_error&  oError, ADOConnection&  oConnection) : ComException(szLocation, oError)
   {
      m_szConnectionErrors = oConnection.GetErrors();
   }

   wstring  ADOException::getError()
   {
      // [OPTIONAL] Append any connection errors
      return (m_szConnectionErrors.empty() ? ComException::getError() : ComException::getError() + TEXT(" - ") + m_szConnectionErrors);
   }

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                 CONSTRUCTION / DESTRUCTION
   /// /////////////////////////////////////////////////////////////////////////////////////////

   ADOConnection::ADOConnection(void)
   {
      m_pConnection = NULL;
   }

   ADOConnection::~ADOConnection(void)
   {
      try
      {
         // Disconnect if necessary
         if (isConnected())
            Disconnect();
      }
      catch (ADOException& ex)
      {
         ex.Print(HERE);
      }
   }

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                       METHODS
   /// /////////////////////////////////////////////////////////////////////////////////////////

   bool  ADOConnection::ConnectAccess(const TCHAR*  szPath, const TCHAR*  szUsername, const TCHAR*  szPassword)
   {
      try
      {
         // [CHECK] Ensure no connection + properties present
         if (isConnected() OR !szPath OR !szUsername OR !szPassword)
            return false;

         // Generate connection string
         wstring  szConnection = STRING("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=") + szPath + TEXT(";");
         
         // Connect to database
         m_pConnection.CreateInstance("ADODB.Connection");
         m_pConnection->Open(szConnection.c_str(), szUsername, szPassword, ADO::adConnectUnspecified);

         return true;
      }
      catch (_com_error&  ex) 
      {
         throw ADOException(HERE, ex, *this);
      }
   }

   bool  ADOConnection::ConnectSQLServer()
   {
      try
      {
         if (isConnected())
            return false;

         // Generate connection string
         wstring  szConnection = TEXT("Provider=SQLNCLI;Server=.\\SQLEXPRESS;Database=master;Trusted_Connection=yes;");

         // Connect to database
         m_pConnection.CreateInstance("ADODB.Connection");
         m_pConnection->Open(szConnection.c_str(), TEXT(""), TEXT(""), ADO::adConnectUnspecified);

         return true;
      }
      catch (_com_error&  ex) 
      {
         throw ADOException(HERE, ex, *this);
      }
   }


   bool  ADOConnection::Disconnect()
   {
      try
      {
         if (!isConnected())
            return false;
         
         // Close database
         m_pConnection->Close();
         return true;
      }
      catch (_com_error&  ex) 
      {
         throw ADOException(HERE, ex, *this);
      }
   }

   
   wstring  ADOConnection::GetErrors() throw ()
   {
      wstring  szOutput;

      try
      {
         // Extract and concatenate all errors
         for (long i = 0; i < m_pConnection->Errors->Count; i++)
         {
            ADO::ErrorPtr  pError  = m_pConnection->Errors->GetItem(i);
            const TCHAR*   szError = (TCHAR*)pError->Description;

            if (szError)
               szOutput += wstring(szError) + TEXT("\n");
         }

         // Delete all errors
         m_pConnection->Errors->Clear();
      }
      catch (_com_error&) {
      }
      
      return szOutput;
   }


   bool  ADOConnection::hasErrors() throw ()
   {
      try
      {
         return isConnected() AND m_pConnection->Errors->Count > 0;
      }
      catch (_com_error&) {
         return false;
      }
      catch (ADOException&) {
         return false;
      }
   }


   bool  ADOConnection::isConnected()
   {
      try
      {
         return m_pConnection AND (m_pConnection->State INCLUDES ADO::adStateOpen);
      }
      catch (_com_error&  ex) 
      {
         throw ADOException(HERE, ex, *this);
      }
   }


} // END: namespace Database