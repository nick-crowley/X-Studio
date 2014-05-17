#include "StdAfx.h"
#include "BaseWindow.h"


using Utils::StringBufferW;

namespace Windows
{
   // /////////////////////////////////////////////////////////////////////////////////////////
   //                                   WINDOWS EXCEPTION
   // /////////////////////////////////////////////////////////////////////////////////////////

   WindowsException::WindowsException(const TCHAR*  szLocation) : Utils::BaseException(szLocation, (long)::GetLastError())
   {
   }

   WindowsException::WindowsException(const TCHAR*  szLocation, const TCHAR*  szDetail) : Utils::BaseException(szLocation, (long)::GetLastError()), m_szDetail(szDetail)
   {
   }

   wstring  WindowsException::getError()
   {
      Utils::StringBuffer<TCHAR>  szSystemError(256);

      // Lookup error
      ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM WITH FORMAT_MESSAGE_IGNORE_INSERTS, NULL, (DWORD)getCode(), NULL, szSystemError, 256, NULL);

      // [OPTIONAL] Prepend detail
      return m_szDetail.empty() ? szSystemError : m_szDetail + TEXT(" - ") + (TCHAR*)szSystemError;
   }

   // /////////////////////////////////////////////////////////////////////////////////////////
   //                                   WINDOW CLASS
   // /////////////////////////////////////////////////////////////////////////////////////////

   BaseWindow::WindowClass::WindowClass(const TCHAR*  szName, HINSTANCE  hInstance, UINT  iStyle, UINT  iWndExtra, HICON  hIcon, const TCHAR*  szMenu)
   {
      // Set properties
      lpszClassName = szName;
      hInstance     = hInstance;
      this->hIcon   = hIcon;
      style			  = iStyle;
      iWndExtra     = iWndExtra;
	   lpszMenuName  = szMenu;
	   
      // Set common options
      hCursor		  = LoadCursor(NULL, IDC_ARROW);
	   hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
      lpfnWndProc	  = BaseWindow::WindowProc;
   }

   bool  BaseWindow::WindowClass::Register()
   {
      return ::RegisterClass(this) ? true : false;
   }

   bool  BaseWindow::WindowClass::UnRegister()
   {
      return ::UnregisterClass(lpszClassName, hInstance) ? true : false;
   }

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                   CONSTANTS / GLOBALS
   /// /////////////////////////////////////////////////////////////////////////////////////////

   BaseWindow::WindowMap  BaseWindow::m_pWindows;

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                   CREATION / DESTRUCTION
   /// /////////////////////////////////////////////////////////////////////////////////////////

   BaseWindow::BaseWindow()
   {
      // Zero everything
      m_hWnd        = NULL;
      m_pfnBaseProc = NULL;
   }

   BaseWindow::~BaseWindow()
   {
      try
      {
         if (Handle)
            Destroy();
      }
      catch (WindowsException&  e) {
         e.Print(HERE);
      }
   }


   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                      PUBLIC METHODS
   /// /////////////////////////////////////////////////////////////////////////////////////////

   bool  BaseWindow::Attach(HWND  hWindow)
   {
      if (Handle)
         return false;

      AcquireHandle(hWindow);
      return true;
   }

   bool   BaseWindow::Attach(BaseWindow*  pWindow, const UINT  iControlID)
   {
      return Attach(pWindow->getChildHandle(iControlID));
   }

   CursorPos  BaseWindow::ClientCursor() const
   {
      return CursorPos(this);
   }

   Rect  BaseWindow::ClientRect() const
   {
      Rect  rcClient;

      GetClientRect(Handle, &rcClient);
      return rcClient;
   }


   bool  BaseWindow::Detach()
   {
      if (!Handle)
         return false;

      // [SUBCLASSED] Unsubclass
      if (isSubclassed())
         UnSubclass();

      ReleaseHandle();
      return true;
   }

   bool   BaseWindow::Destroy()
   {
      if (!Handle)
         return false;

      // Attempt to destroy window
      if (!DestroyWindow(Handle))
         throw WindowsException(HERE, TEXT("Unable to destroy window"));
      
      // Remove window handle from map and release.
      ReleaseWindow();
      ReleaseHandle();

      // Cleanup
      m_pReflectors.clear();
      m_pfnBaseProc = NULL;
      return true;
   }

   Rect     BaseWindow::getChildRect(const UINT  iControlID) const
   {
      Rect  rcChild;

      // Lookup window
      if (::GetWindowRect(getChildHandle(iControlID), &rcChild))
      {
         // Convert points from screen co-ords to client co-ords (for this window)
         ::ScreenToClient(Handle, reinterpret_cast<POINT*>(&rcChild.left));
         ::ScreenToClient(Handle, reinterpret_cast<POINT*>(&rcChild.right));
      }

      return rcChild;
   }

   long     BaseWindow::getClassLong(const int  iIndex) const
   {
      return GetClassLongPtr(Handle, iIndex);
   }

   long     BaseWindow::getMouseTracking()
   {
      EmptyStruct<TRACKMOUSEEVENT>  oEvent;

      oEvent.cbSize      = sizeof(TRACKMOUSEEVENT);
      oEvent.dwFlags     = TME_QUERY;
      oEvent.hwndTrack   = Handle;
      ::TrackMouseEvent(&oEvent);
      
      return (long)oEvent.dwFlags;
   }

   wstring  BaseWindow::getText() const
   {
      size_t         iLength = GetWindowTextLength(Handle) + 1;
      StringBufferW  szBuffer(iLength);

      ::GetWindowText(Handle, szBuffer, iLength);
      return szBuffer;
   }

   long     BaseWindow::getWindowLong(const int  iIndex) const
   {
      return GetWindowLongPtr(Handle, iIndex);
   }

   /*bool  BaseWindow::isBeneathCursor(const ScreenCursor&  ptCursor) const
   {
      return WindowRect().Contains(ptCursor);
   }*/

   int   BaseWindow::MessageBox(wstring  szMessage, wstring  szTitle, const int  iFlags)
   {
      return ::MessageBox(Handle, szMessage.c_str(), szTitle.c_str(), iFlags);
   }

   bool  BaseWindow::ModifyStyle(DWORD  dwAdd, DWORD  dwRemove)
   {
      return SetWindowLong(Handle, GWL_STYLE, (Style & ~dwRemove) | dwAdd) ? true : false;
   }

   bool  BaseWindow::Resize(const Size  oSize)
   {
      return ::SetWindowPos(Handle, NULL, NULL, NULL, oSize.cx, oSize.cy, SWP_NOZORDER | SWP_NOMOVE) ? true : false; 
   }

   bool  BaseWindow::SetRect(const Rect  rcPosition)
   { 
      return ::SetWindowPos(Handle, NULL, rcPosition.left, rcPosition.top, rcPosition.Width(), rcPosition.Height(), SWP_NOZORDER) ? true : false; 
   }

   bool  BaseWindow::ShowAndUpdate(const UINT  iMode)
   {
      return Show(iMode) AND Update();
   }

   bool  BaseWindow::TrackMouse(int  iFlags, int  iHoverTime)
   {
      EmptyStruct<TRACKMOUSEEVENT>  oEvent;

      oEvent.cbSize      = sizeof(TRACKMOUSEEVENT);
      oEvent.dwFlags     = iFlags;
      oEvent.hwndTrack   = Handle;
      oEvent.dwHoverTime = (DWORD)iHoverTime;

      return ::TrackMouseEvent(&oEvent) ? true : false;
   }

   Rect  BaseWindow::WindowRect() const
   {
      Rect  rcClient;

      GetWindowRect(Handle, &rcClient);
      return rcClient;
   }

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                      PROTECTED METHODS
   /// /////////////////////////////////////////////////////////////////////////////////////////

   bool   BaseWindow::Create(const TCHAR*  szTitle, DWORD  dwStyle, DWORD  dwStyleEx, BaseWindow*  pParent, const UINT  iControlID, HINSTANCE  hInstance, Rect*  pRect)
   {
      //EmptyStruct<WNDCLASS>  oClass;

      HWND  hParent = (pParent? pParent->Handle : NULL),
            Handle    = NULL;

      // [CHECK] Ensure window doesn't already exist
      if (Handle)
         return false;

      // [CHECK] Ensure window class has been registered
      /*if (!GetClassInfo(hInstance, getClassName(), &oClass) AND !RegisterClass(hInstance))
         throw WindowsException(HERE, TEXT("Unable to register window class"));*/
         
      /// Attempt to create window
      if (pRect)
         Handle = CreateWindowEx(dwStyleEx, getClassName(), szTitle, dwStyle, pRect->left, pRect->top, pRect->Width(), pRect->Height(), hParent, (HMENU)(iControlID), hInstance, (VOID*)this);
      else
         Handle = CreateWindowEx(dwStyleEx, getClassName(), szTitle, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hParent, (HMENU)(iControlID), hInstance, (VOID*)this);

      // [CHECK] Ensure window was created
      if (!Handle)
         throw WindowsException(HERE, TEXT("Unable to create window class"));

      // Acquire handle
      AcquireHandle(Handle);
      return true;
   }


   LRESULT  BaseWindow::getHandledMessageResult(const UINT  iMessage)
   {
      switch (iMessage)
      {
      // [CREATE / COLOUR / NOTIFY] Return value directly
      case WM_CREATE:
      case WM_CTLCOLORBTN:
      case WM_CTLCOLORDLG:
      case WM_CTLCOLOREDIT:
      case WM_CTLCOLORLISTBOX:
      case WM_CTLCOLORSCROLLBAR:
      case WM_CTLCOLORSTATIC:
      case WM_NOTIFY:
         return getMessageResult();

      // [DEFAULT] Return 0
      default:
         return 0;
      }
   }
   

   bool  BaseWindow::isChildReflected(const UINT  iControlID) const
   { 
      return m_pReflectors.find(iControlID) != m_pReflectors.end(); 
   }


   void  BaseWindow::ReflectChild(const UINT  iControlID, BaseWindow*  pChild)
   {
      m_pReflectors[iControlID] = pChild;
   }

   void  BaseWindow::SetMessageResult(LRESULT  iResult)
   { 
      m_iMsgResult = iResult;  
   }

   /*void  BaseWindow::SetReflectedMessageResult(LRESULT  iResult)
   {
      if (getParent())
         getParent()->SetMessageResult(iResult);
   }*/

   bool  BaseWindow::Subclass()
   {
      // [CHECK] Ensure not already subclassed
      if (isSubclassed() OR !Handle)
         return false;

      // Subclass window 
      m_pfnBaseProc = reinterpret_cast<WNDPROC>(SetWindowLong(Handle, GWL_WNDPROC, reinterpret_cast<LONG>(WindowProc)));

      if (!m_pfnBaseProc)
         throw WindowsException(HERE, TEXT("Unable to subclass window"));
      
      // Add window to map
      AcquireWindow();
      
      // [EVENT] OnSubclassed
      onSubclassed();
      return true;
   }

   // /////////////////////////////////////////////////////////////////////////////////////////
   //                                    MESSAGE HANDLERS
   // /////////////////////////////////////////////////////////////////////////////////////////

   bool  BaseWindow::onCreate(CREATESTRUCT*  pCreationData)
   { 
      // [DEFAULT] Return 0 to continue creation
      SetMessageResult(0);
      return Handled; 
   }

   bool  BaseWindow::onNotify(const UINT  iControlID, NMHDR*  pHeader)
   {
      // Determine whether to reflect to child or handle ourselves
      BaseWindow*  pHandler = (isChildReflected(iControlID) ? m_pReflectors[iControlID] : this);

      // Delegate to common handler if possible
      switch (pHeader->code)
      {
      case NM_CUSTOMDRAW:  return pHandler->onCustomDraw(iControlID, reinterpret_cast<NMCUSTOMDRAW*>(pHeader));
      case NM_CLICK:       return pHandler->onMouseLeftClick(iControlID, pHeader);
      case NM_DBLCLK:      return pHandler->onMouseLeftDoubleClick(iControlID, pHeader);
      case NM_RCLICK:      return pHandler->onMouseRightClick(iControlID, pHeader);
      case NM_RDBLCLK:     return pHandler->onMouseRightDoubleClick(iControlID, pHeader);
      }
      
      // [DEFAULT] Reflect to child, or return 'Unhandled'
      return (pHandler != this ? pHandler->onNotify(iControlID, pHeader) : Unhandled);
   }

   bool  BaseWindow::onOwnerDraw(const UINT  iControlID, DRAWITEMSTRUCT*  pDrawItem)
   {
      // [REFLECTION] Reflect to child
      if (isChildReflected(pDrawItem->CtlID))
         return m_pReflectors[pDrawItem->CtlID]->onOwnerDraw(pDrawItem->CtlID, pDrawItem);

      // [NONE] Return unhandled
      return Unhandled;
   }

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                      PRIVATE METHODS
   /// /////////////////////////////////////////////////////////////////////////////////////////

   bool  BaseWindow::DelegateMessage(UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
   {
      bool   bHandled = false;

      // Set default message result to NULL
      SetMessageResult(NULL);

      try
      {
         switch (iMessage)
         {
         // Creation
         case WM_CREATE:         bHandled = onCreate(reinterpret_cast<CREATESTRUCT*>(lParam));                                   break;
         case WM_INITDIALOG:     bHandled = onInitDialog();                                                                      break;

         // Destruction
         case WM_DESTROY:        bHandled = onDestroy();  
            ReleaseWindow();  
            ReleaseHandle();  // Remove window handle from map and release.
            break;

         // General
         case WM_CLOSE:          bHandled = onClose();                                                                           break;
         case WM_SETFOCUS:       bHandled = onSetFocus((HWND)wParam);                                                            break;
         case WM_KILLFOCUS:      bHandled = onKillFocus((HWND)wParam);                                                           break;
         case WM_TIMER:          bHandled = onTimer((UINT)wParam);                                                               break;
         case WM_HSCROLL:        bHandled = onHorizontalScroll(LOWORD(wParam), HIWORD(wParam), reinterpret_cast<HWND>(lParam));  break;
         case WM_VSCROLL:        bHandled = onVerticalScroll(LOWORD(wParam), HIWORD(wParam), reinterpret_cast<HWND>(lParam));    break;
         case WM_SIZE:           bHandled = onSize(wParam, LOWORD(lParam), HIWORD(lParam));                                      break;

         // Notification
         case WM_COMMAND:        bHandled = onCommand(LOWORD(wParam), HIWORD(wParam), reinterpret_cast<HWND>(lParam));           break;
         case WM_NOTIFY:         bHandled = onNotify(wParam, reinterpret_cast<NMHDR*>(lParam));                                  break;
     
         // Keyboard
         case WM_CHAR:           bHandled = onKeyCharacter((TCHAR)wParam);                     break;
         case WM_KEYDOWN:        bHandled = onKeyDown((UINT)wParam);                           break;
         case WM_KEYUP:          bHandled = onKeyUp((UINT)wParam);                             break;

         // Mouse Buttons
         case WM_LBUTTONDBLCLK:  bHandled = onMouseLeftDoubleClick( Point(lParam), wParam);    break;
         case WM_RBUTTONDBLCLK:  bHandled = onMouseRightDoubleClick(Point(lParam), wParam);    break;
         case WM_LBUTTONDOWN:    bHandled = onMouseLeftDown( Point(lParam), wParam);           break;
         case WM_RBUTTONDOWN:    bHandled = onMouseRightDown(Point(lParam), wParam);           break;
         case WM_LBUTTONUP:      bHandled = onMouseLeftUp( Point(lParam), wParam);             break;
         case WM_RBUTTONUP:      bHandled = onMouseRightUp(Point(lParam), wParam);             break;

         // Mouse Position
         case WM_MOUSEMOVE:      bHandled = onMouseMove(Point(lParam), wParam);                break;
         case WM_MOUSEHOVER:     bHandled = onMouseHover(Point(lParam), wParam);               break;
         case WM_MOUSELEAVE:     bHandled = onMouseLeave();                                    break;

         // Painting
         case WM_PAINT:          bHandled = onPaint();                                                        break;
         case WM_DRAWITEM:       bHandled = onOwnerDraw(wParam, reinterpret_cast<DRAWITEMSTRUCT*>(lParam));   break;
         case WM_ERASEBKGND:     bHandled = onEraseBackground((HDC)wParam);                                   break;
                              
         // Remainder
         default:                bHandled = (iMessage >= WM_USER ? onUserMessage(iMessage, wParam, lParam) : onUnhandledMessage(iMessage, wParam, lParam));       break;
         }

         /// Return whether handled
         return bHandled;
      }
      catch (Utils::BaseException&  e)
      {
         e.Display(HERE, Handle);
      }
      catch (std::exception&  e)
      {
         Utils::DisplayException(HERE_A, Handle, e);
      }

      // [ERROR] Return 'Unhandled'
      return Unhandled;
   }


   LRESULT  BaseWindow::BaseWindowProc(UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
   {
      LRESULT  iResult = CallWindowProc(m_pfnBaseProc, Handle, iMessage, wParam, lParam);

      // [DESTROY] Unsubclass window
      if (iMessage == WM_DESTROY)
         UnSubclass();     // Must be done after WM_DESTROY is dispatched to base window proc

      return iResult;
   }


   bool  BaseWindow::UnSubclass()
   {
      if (!isSubclassed() OR !Handle)
         return false;

      // Unsubclass window 
      if (!SetWindowLong(Handle, GWL_WNDPROC, reinterpret_cast<LONG>(m_pfnBaseProc)))
         throw WindowsException(HERE, TEXT("Unable to unsubclass window"));
         
      // Remove window handle from map
      ReleaseWindow();
      m_pfnBaseProc = NULL;
      return true;
   }

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                     STATIC METHODS
   /// /////////////////////////////////////////////////////////////////////////////////////////

   BaseWindow*  BaseWindow::LookupWindow(HWND  hWindow)
   {
      // Extract object from map, or return NULL
      return (m_pWindows.find(hWindow) != m_pWindows.end() ? m_pWindows[hWindow] : NULL);
   }

   BaseWindow*  BaseWindow::LookupWindowAtPoint(Point  ptScreen)
   {
      return LookupWindow( WindowFromPoint(ptScreen) );
   }

   LRESULT CALLBACK  BaseWindow::WindowProc(HWND  hWindow, UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
   {
      BaseWindow* pWindow = NULL;
      LRESULT     iResult = 0;
      bool        bHandled;

      /// Lookup matching window object
      pWindow = LookupWindow(hWindow);

      /// [NEW WINDOW] Insert into map
      if (!pWindow AND iMessage == WM_CREATE)
      {
         // [WINDOW] Extract window handle from CREATESTRUCT
         if (pWindow = (BaseWindow*)reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams)
         {
            // Acquire handle. Add to map.
            pWindow->AcquireHandle(hWindow);
            pWindow->AcquireWindow();
         }
      }

      if (pWindow)
      {
         /// [DEFAULT] Dispatch to handlers
         if (bHandled = pWindow->DelegateMessage(iMessage, wParam, lParam))
            // [HANDLED] Return 0 (most of the time)
            return pWindow->getHandledMessageResult(iMessage);
         
         /// [SUBCLASSED] Pass to base proc
         if (pWindow->isSubclassed())
            return pWindow->BaseWindowProc(iMessage, wParam, lParam);
      }
      
      // [UNHANDLED or ERROR] Pass to DefWndProc
      return DefWindowProc(hWindow, iMessage, wParam, lParam);
   }


   


} // END: namespace Windows
