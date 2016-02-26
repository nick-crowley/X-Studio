#pragma once

#include "Dialog.h"

namespace Windows
{
   /// Message Pump
   class MessagePump
   {
   protected:
      // MSG wrapper
      class Message : public Utils::EmptyStruct<MSG>
      {
      public:
         Message(void) { };
         virtual ~Message(void) { };

         BOOL     GetNext(BaseWindow*  pWindow, UINT  iFilterMin = NULL, UINT  iFilterMax = NULL);
         BOOL     Dispatch();
         BOOL     DispatchDialog(Dialog*  pDialog);
         BOOL     Translate();

         wstring  toString();
      };

      // Function Object for dispatching modeless dialog messages
      class DialogDispatcher
      {
      public:
         DialogDispatcher(Message*  pMessage);

         void  operator()(Dialog*  pDialog);
         operator bool() { return m_bDispatched; };

      protected:
         Message*  m_pMessage;
         bool      m_bDispatched;
      };

   public:
      virtual int  Pump(BaseWindow*  pWindow);

      void   AddDialog(Dialog*  pDialog)       { m_pDialogs.insert(pDialog); };
      void   RemoveDialog(Dialog*  pDialog)    { m_pDialogs.erase(pDialog);  };

   protected:
      Message       m_oMessage;
      set<Dialog*>  m_pDialogs;
   };

}