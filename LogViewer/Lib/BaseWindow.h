#pragma once

#include "Utils.hpp"
#include "CommonTypes.h"
#include "BaseException.h"
#include "DeviceContext.h"


using Utils::EmptyStruct;
using Graphics::DeviceContext;

namespace Windows
{
   /// WindowsException - Wraps GetLastError()
   class WindowsException : public Utils::BaseException
   {
   public:
      WindowsException(const TCHAR*  szLocation);
      WindowsException(const TCHAR*  szLocation, const TCHAR*  szDetail);
      virtual ~WindowsException() {};

   protected:
      virtual wstring  getError();
      virtual wstring  getType()   { return TEXT("Windows::WindowsException"); };

   private:
      wstring  m_szDetail;
   };

   

   /// BaseWindow
   class BaseWindow
   {
   friend class Dialog;
   friend class BaseControl;

   protected:
      class WindowClass;
      typedef map<UINT, BaseWindow*>  ControlMap;
      typedef map<HWND, BaseWindow*>  WindowMap;

   public:
      BaseWindow();
      virtual ~BaseWindow();

      // Methods
      bool     Attach(HWND  hWindow);
      bool     Attach(BaseWindow*  pWindow, const UINT  iControlID);
     CursorPos ClientCursor() const;
      Rect     ClientRect() const;
      bool     Detach();
      bool     Destroy();
      Rect     getChildRect(const UINT  iControlID) const;
      long     getClassLong(const int  iIndex) const;
      long     getMouseTracking();
      wstring  getText() const;
      long     getWindowLong(const int  iIndex) const;
      int      MessageBox(wstring  szMessage, wstring  szTitle, const int  iFlags = MB_OK | MB_ICONERROR);
      bool     ModifyStyle(DWORD  dwAdd, DWORD  dwRemove);
      bool     Resize(const Size  oSize);
      bool     SetRect(const Rect  rcPosition);
      bool     ShowAndUpdate(const UINT  iMode = SW_SHOW);
      bool     TrackMouse(int  iFlags, int  iHoverTime = HOVER_DEFAULT);
      Rect     WindowRect() const;

      // Helpers
      bool     Enable(const bool  bEnable)                                        { return ::EnableWindow(Handle, (BOOL)bEnable) ? true : false;             };
      bool     KillTimer(const UINT  iID)                                         { return ::KillTimer(Handle, (UINT_PTR)iID) ? true : false;                };
      bool     InvalidateRect(Rect*  pRect, const bool  bErase)                   { return ::InvalidateRect(Handle, pRect, (BOOL)bErase) ? true : false;     };
      LRESULT  PostMessage(const UINT  iMessage, WPARAM  wParam, LPARAM  lParam)  { return ::PostMessage(Handle, iMessage, wParam, lParam);                  };
      HWND     SetFocus()                                                         { return ::SetFocus(Handle);                                               };
      bool     SetText(const wstring  szText)                                     { return ::SetWindowText(Handle, szText.c_str()) ? true : false;           };
      bool     SetTimer(const UINT  iID, const UINT  iLength)                     { return ::SetTimer(Handle, (UINT_PTR)iID, iLength, NULL) ? true : false;  };
      LRESULT  SendMessage(const UINT  iMessage, WPARAM  wParam, LPARAM  lParam)  { return ::SendMessage(Handle, iMessage, wParam, lParam);                  };
      bool     Show(const UINT  iMode)                                            { return ::ShowWindow(Handle, iMode) ? true : false;                       };
      bool     Update()                                                           { return ::UpdateWindow(Handle) ? true : false;                            };
      
      // Properties
      __declspec(property(get=getHandle))            HWND       Handle;
      __declspec(property(get=getInstance))          HINSTANCE  Instance;
      __declspec(property(get=getText, put=SetText)) wstring    Text;
      __declspec(property(get=getStyle))             DWORD      Style;
      __declspec(property(get=getWindowID))          UINT       WindowID;

      // Properties
      DeviceContext getDC()                                { return DeviceContext(this);                       };
      HINSTANCE     getInstance() const                    { return (HINSTANCE)getWindowLong(GWLP_HINSTANCE);  };
      HWND          getHandle() const                      { return m_hWnd;                                    };
      DWORD         getStyle() const                       { return GetWindowStyle(Handle);                    };
      UINT          getWindowID() const                    { return GetDlgCtrlID(Handle);                      };
      bool          isSubclassed() const                   { return m_pfnBaseProc != NULL;                     };

      // Hierarchy
      HWND          getChildHandle(const UINT  iID) const  { return ::GetDlgItem(Handle, iID);                 };
      BaseWindow*   getChild(const UINT  iID) const        { return LookupWindow(getChildHandle(iID));         };
      HWND          getParentHandle() const                { return ::GetParent(Handle);                       };
      BaseWindow*   getParent() const                      { return LookupWindow(getParentHandle());           };

   protected:
      enum     MessageResult  { Handled = true, Unhandled = false };

      // Methods
      bool     Create(const TCHAR*  szTitle, DWORD  dwStyle, DWORD  dwStyleEx, BaseWindow*  pParent, const UINT  iControlID, HINSTANCE  hInstance, Rect*  pRect);
      bool     DelegateMessage(UINT  iMessage, WPARAM  wParam, LPARAM  lParam);
      bool     isChildReflected(const UINT  iControlID) const;
      void     ReflectChild(const UINT  iControlID, BaseWindow*  pChild);
      bool     Subclass();
      void     SetMessageResult(LRESULT  iResult);

      // Static
      static LRESULT CALLBACK  WindowProc(HWND  hWindow, UINT  iMessage, WPARAM  wParam, LPARAM  lParam);
      static BaseWindow*       LookupWindow(HWND  hWindow);
      static BaseWindow*       LookupWindowAtPoint(Point  ptScreen);

      // Virtual
      virtual const TCHAR*  getClassName() const = NULL;

      // Data
      HWND          m_hWnd;
      WindowClass*  m_pClass;

   private:
      // Handles
      void     AcquireHandle(HWND  hWindow)  { m_hWnd = hWindow;           };
      void     AcquireWindow()               { m_pWindows[Handle] = this;  };
      void     ReleaseHandle()               { m_hWnd = NULL;              };
      void     ReleaseWindow()               { m_pWindows.erase(Handle);   };

      // Messages
      LRESULT          BaseWindowProc(UINT  iMessage, WPARAM  wParam, LPARAM  lParam);
      virtual LRESULT  getHandledMessageResult(const UINT  iMessage);
      bool             UnSubclass();      

      // Helpers
      LRESULT          getMessageResult() const    { return m_iMsgResult;   };

      // Window
      ControlMap   m_pReflectors;
      WNDPROC      m_pfnBaseProc;
      LRESULT      m_iMsgResult;

      // Static
      static WindowMap  m_pWindows;

   /// Handlers:
   protected:
      // System Message Handlers
      virtual bool  onClose()                                                             { return Unhandled; };
      virtual bool  onCreate(CREATESTRUCT*  pCreationData);
      virtual bool  onDestroy()                                                           { return Unhandled; };
      virtual bool  onInitDialog()                                                        { return Unhandled; };
      
      // Notification
      virtual bool  onCommand(const UINT  iControlID, const UINT  iMessage, HWND  hCtrl)  { return Unhandled; };
      virtual bool  onNotify(const UINT  iControlID, NMHDR*  pHeader);

      // General
      virtual bool  onKillFocus(HWND  hNextWnd)                                           { return Unhandled; };
      virtual bool  onSetFocus(HWND  hPrevWnd)                                            { return Unhandled; };
      virtual bool  onTimer(const UINT  iID)                                              { return Unhandled; };
      virtual bool  onHorizontalScroll(const UINT  eType, const UINT  iPos, HWND  hCtrl)  { return Unhandled; };
      virtual bool  onVerticalScroll(const UINT  eType, const UINT  iPos, HWND  hCtrl)    { return Unhandled; };
      virtual bool  onSize(const UINT  eType, const UINT  iWidth, const UINT  iHeight)    { return Unhandled; };

      // Keyboard
      virtual bool  onKeyCharacter(const TCHAR  chChar)                                   { return Unhandled; };
      virtual bool  onKeyDown(const UINT  iVirtKey)                                       { return Unhandled; };
      virtual bool  onKeyUp(const UINT  iVirtKey)                                         { return Unhandled; };
      
      // Mouse
      virtual bool  onMouseHover(Point  oPoint, const UINT  iFlags)                       { return Unhandled; };
      virtual bool  onMouseMove(Point  oPoint, const UINT  iFlags)                        { return Unhandled; };
      virtual bool  onMouseLeave()                                                        { return Unhandled; };

      virtual bool  onMouseLeftClick(const UINT  iControlID, NMHDR*  pHeader)             { return Unhandled; };
      virtual bool  onMouseLeftDoubleClick(Point  oPoint, const UINT  iFlags)             { return Unhandled; };
      virtual bool  onMouseLeftDoubleClick(const UINT  iControlID, NMHDR*  pHeader)       { return Unhandled; };
      virtual bool  onMouseLeftDown(Point  oPoint, const UINT  iFlags)                    { return Unhandled; };
      virtual bool  onMouseLeftUp(Point  oPoint, const UINT  iFlags)                      { return Unhandled; };

      virtual bool  onMouseRightClick(const UINT  iControlID, NMHDR*  pHeader)            { return Unhandled; };
      virtual bool  onMouseRightDoubleClick(Point  oPoint, const UINT  iFlags)            { return Unhandled; };
      virtual bool  onMouseRightDoubleClick(const UINT  iControlID, NMHDR*  pHeader)      { return Unhandled; };
      virtual bool  onMouseRightDown(Point  oPoint, const UINT  iFlags)                   { return Unhandled; };
      virtual bool  onMouseRightUp(Point  oPoint, const UINT  iFlags)                     { return Unhandled; };

      // Drag n Drop
      virtual bool  onMouseLeftDrag(BaseWindow*  pSource, DropData*  pData)               { return Unhandled; };
      virtual bool  onMouseLeftDrop(BaseWindow*  pSource, DropData*  pData)               { return Unhandled; };
      virtual bool  onMouseRightDrag(BaseWindow*  pSource, DropData*  pData)              { return Unhandled; };
      virtual bool  onMouseRightDrop(BaseWindow*  pSource, DropData*  pData)              { return Unhandled; };

      // Drawing
      virtual bool  onCustomDraw(const UINT  iControlID, NMCUSTOMDRAW*  pHeader)          { return Unhandled; };
      virtual bool  onOwnerDraw(const UINT  iControlID, DRAWITEMSTRUCT*  pDrawItem);
      virtual bool  onEraseBackground(HDC  hDC)                                           { return Unhandled; };
      virtual bool  onPaint()                                                             { return Unhandled; };

      // Custom Message Handlers
      virtual bool  onUserMessage(UINT  iMessage, WPARAM  wParam, LPARAM  lParam)         { return Unhandled; };
      virtual bool  onUnhandledMessage(UINT  iMessage, WPARAM  wParam, LPARAM  lParam)    { return Unhandled; };
      virtual bool  onSubclassed()                                                        { return Unhandled; };

   /// WindowClass
   protected:
      class WindowClass : public Utils::EmptyStruct<WNDCLASS>
      {
      public:
         WindowClass() {};
         WindowClass(const TCHAR*  szName, HINSTANCE  hInstance, UINT  iStyle, UINT  iWndExtra, HICON  hIcon, const TCHAR*  szMenu);

         bool  Register();
         bool  UnRegister();
      };

   };


} // END: namespace windows
