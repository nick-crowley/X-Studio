#pragma once
#include "BaseWindow.h"

namespace Windows
{

   /// Dialog
   class Dialog : public BaseWindow
   {
   public:
      Dialog();
      ~Dialog();

   protected:
      // Methods
      bool  Create(const TCHAR*  szTemplate, BaseWindow*  pParent, HINSTANCE  hInstance, const bool  bModal);

      // Handlers
      virtual bool  onPaint();

      // Virtual
      virtual const TCHAR*  getClassName() const    { return NULL; };
      virtual LRESULT       getHandledMessageResult(const UINT  iMessage);
      
      static LRESULT CALLBACK  DialogProc(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);

      // Data
      bool  m_bModal;
   };


} // END: namespace windows
