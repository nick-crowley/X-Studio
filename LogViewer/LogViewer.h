#pragma once



using namespace Windows;


/// Application:
class Application : public Windows::MessagePump
{
protected:
   Application(void) {};
   ~Application(void) {};
   Application(const Application&  r) {};
   Application& operator=(const Application&  r) {};

   static Application   m_oApp;
   friend Application&  getApp();

public:
   bool   Create(HINSTANCE  hInstance);
   void   Destroy();

   HINSTANCE    getInstance()   { return m_oApp.m_hInstance;  };

protected:
   HINSTANCE    m_hInstance;
   
};

// Helpers
Application&    getApp();




/// MainWindow: 
class MainWindow : public Windows::BaseWindow
{
   typedef  vector<wstring>        MessageLog;
   typedef  map<int, MessageLog>   ThreadMap;

public:
   MainWindow(HINSTANCE  hInstance);
   virtual ~MainWindow(void);

   bool  Create(const TCHAR*  szTitle, Windows::Rect*  pRect, HINSTANCE  hInstance);
   bool  ReadLog(const TCHAR*  szLogPath);

protected:
   int      findNextException(int  iThreadIndex, int  iMessageIndex);
   wstring  getMessageByIndex(int  iThreadIndex, int  iMessageIndex);
   UINT     getMessageCount(int  iThreadIndex);
   UINT     getThreadID(int  iIndex);
   UINT     getThreadIndex(int  iID);

   virtual const TCHAR*  getClassName() const  { return TEXT("LV_MainWindow"); };

   virtual bool  onCreate(CREATESTRUCT*  pCreationData);
   virtual bool  onClose();
   virtual bool  onDestroy();

   bool          onDropFiles(HDROP  hDrop);
   virtual bool  onKeyDown(const UINT  iVirtKey);
   virtual bool  onNotify(const UINT  iControlID, NMHDR*  pHeader);
   virtual bool  onRetrieveItem(NMHDR*  pHeader, LVITEM&  oItem);
   virtual bool  onSize(const UINT  eType, const UINT  iWidth, const UINT  iHeight);
   bool          onThreadSelectionChanged(int  iItem, bool  bSelected);
   virtual bool  onUnhandledMessage(UINT  iMessage, WPARAM  wParam, LPARAM  lParam);


   ListView   m_oThreadList,
              m_oMessageList;

   ThreadMap  m_oLogFile;
};


// ChangeWindowMessageFilterEx() - Adjusts the message filter to allow drag'n'drop functionality
//
typedef   BOOL                  (*CHANGE_WINDOWS_MESSAGE_FILTER_EX)(HWND, UINT, DWORD, CHANGEFILTERSTRUCT*);
#define  WM_COPYGLOBALDATA     0x49 

