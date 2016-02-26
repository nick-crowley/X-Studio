#pragma once
#include "ADO.h"
#include "ComException.h"

namespace Database
{
   class ADOConnection;

   /// Exception
   class ADOException : public Utils::ComException
   {
   public:
      ADOException(const TCHAR*  szLocation, _com_error&  oError, ADOConnection&  oConnection);
      virtual ~ADOException() {};

   protected:
      virtual wstring  getType()  { return TEXT("Database::ADOException"); };
      virtual wstring  getError();

   private:
      wstring  m_szConnectionErrors;
   };


   /// Connection
   class ADOConnection
   {
   friend class ADORecordset;
   friend class ADOCommand;

   public:
      ADOConnection(void);
      virtual ~ADOConnection(void);

      bool     ConnectAccess(const TCHAR*  szPath, const TCHAR*  szUsername, const TCHAR*  szPassword);
      bool     ConnectSQLServer();
      bool     Disconnect();
      wstring  GetErrors() throw ();

      bool     hasErrors() throw ();
      bool     isConnected();

   protected:
      ADO::_ConnectionPtr  m_pConnection;
   };


} // END: namespace Database