#include "StdAfx.h"
#include "Dialog.h"

namespace Windows
{

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                   CREATION / DESTRUCTION
   /// /////////////////////////////////////////////////////////////////////////////////////////

   Dialog::Dialog()
   {
      m_bModal = false;
   }

   Dialog::~Dialog()
   {
      // [MODAL] Ensure base destructor does not attempt to call 'DestroyWindow'
      if (m_bModal)
         ReleaseHandle();
   }

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                      PROTECTED METHODS
   /// /////////////////////////////////////////////////////////////////////////////////////////

   bool   Dialog::Create(const TCHAR*  szTemplate, BaseWindow*  pParent, HINSTANCE  hInstance, const bool  bModal)
   {
      HWND  hDialog;

      // [CHECK] Ensure window does not exist
      if (Handle)
         return false;

      // Store properties
      m_bModal = bModal;
      //m_oClass.hInstance = hInstance;

      // Attempt to create modal/modeless dialog
      if (bModal)
         hDialog = (HWND)DialogBoxParam(hInstance, szTemplate, pParent->Handle, (DLGPROC)DialogProc, (LPARAM)this);
      else
         hDialog = CreateDialogParam(hInstance, szTemplate, pParent->Handle, (DLGPROC)DialogProc, (LPARAM)this);

      // [CHECK] Ensure dialog created successfully
      if (!hDialog OR hDialog == (HWND)-1)
         throw WindowsException(HERE, TEXT("Unable to create dialog"));

      // [MODAL] Reset window handle
      if (bModal)
         ReleaseHandle();

      return true;
   }

   LRESULT  Dialog::getHandledMessageResult(const UINT  iMessage)
   {
      switch (iMessage)
      {
      // [INIT / COLOUR] Return value directly
      case WM_INITDIALOG:
      case WM_CTLCOLORBTN:
      case WM_CTLCOLORDLG:
      case WM_CTLCOLOREDIT:
      case WM_CTLCOLORLISTBOX:
      case WM_CTLCOLORSCROLLBAR:
      case WM_CTLCOLORSTATIC:
         return getMessageResult();

      // [NOTIFICATION] Return TRUE, Save value into window handle.
      case WM_NOTIFY:
         SetWindowLong(Handle, DWL_MSGRESULT, getMessageResult()); 
         return TRUE;

      // [DEFAULT] Return TRUE
      default:
         return TRUE;
      }

   }

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                     MESSAGE HANDLERS
   /// /////////////////////////////////////////////////////////////////////////////////////////

   bool  Dialog::onPaint()
   { 
      return false; 
   }

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                     STATIC METHODS
   /// /////////////////////////////////////////////////////////////////////////////////////////

   LRESULT CALLBACK  Dialog::DialogProc(HWND  hDialog, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
   {
      BaseWindow* pWindow = NULL;
      bool        bHandled;

      /// Lookup matching window object
      pWindow = LookupWindow(hDialog);

      /// [NEW WINDOW] Insert into map
      if (!pWindow AND iMessage == WM_INITDIALOG)
      {
         // [DIALOG] Extract window handle from LPARAM
         if (pWindow = reinterpret_cast<BaseWindow*>(lParam))
         {
            // Acquire handle. Add to map.
            pWindow->AcquireHandle(hDialog);
            pWindow->AcquireWindow();
         }
      }

      /// [EXISTS] Dispatch to handlers
      if (pWindow AND (bHandled = pWindow->DelegateMessage(iMessage, wParam, lParam)))
         // [HANDLED] Return TRUE (most of the time)
         return pWindow->getHandledMessageResult(iMessage);
      
      // [ERROR/UNHANDLED] Return FALSE
      return FALSE;
   }

} // END: namespace Windows