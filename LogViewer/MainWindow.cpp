
#include "StdAfx.h"
#include "LogViewer.h"
#include <sstream>

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    CONSTANTS / GLOBALS
/// /////////////////////////////////////////////////////////////////////////////////////////

#define  IDC_THREAD_LIST      100
#define  IDC_MESSAGE_LIST     200

int       g_iMainThreadID = NULL;

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                 CONSTRUCTION / DESTRUCTION
/// /////////////////////////////////////////////////////////////////////////////////////////


MainWindow::MainWindow(HINSTANCE  hInstance) 
{
   // Register custom window class
   m_pClass = new WindowClass(getClassName(), hInstance, CS_HREDRAW | CS_VREDRAW, 0, LoadIcon(hInstance, getClassName()), getClassName()); 
   m_pClass->Register();
}


MainWindow::~MainWindow()
{
   // Unregister WindowClass
   m_pClass->UnRegister();
   delete m_pClass;
}


/// /////////////////////////////////////////////////////////////////////////////////////////
///                                     PUBLIC METHODS
/// /////////////////////////////////////////////////////////////////////////////////////////


bool  MainWindow::Create(const TCHAR*  szTitle, Windows::Rect*  pRect, HINSTANCE  hInstance)
{
   return BaseWindow::Create(szTitle, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, NULL, NULL, NULL, hInstance, pRect);
}


bool  MainWindow::ReadLog(const TCHAR*  szLogPath)
{
   ifstream  oLogFile(szLogPath, ios::in);
   int       iThreadID;
   string    szLine;

   // Ensure file was opened
   if (!oLogFile.is_open())
      return false;

   /// Clear previous data
   m_oLogFile.clear();
   g_iMainThreadID = -1;
  
   /// Iterate through each line
   while (oLogFile.good())
   {
      wstring  szTextW;
      string   szTextA;

      // Prepare
      getline(oLogFile, szLine);

      /// [THREAD] Set thread ID
      if (szLine.length() > 8 AND szLine.substr(0, 2) == "0x")
      {
         // [HEX]  0xnnnn: <Text>
         sscanf_s(szLine.substr(2, 4).c_str(), "%04x", &iThreadID);
         szTextA = szLine.substr(8);

         // Identify main thread ID
         if (g_iMainThreadID == -1)
            g_iMainThreadID = iThreadID;
      }
      else
         // [TEXT] Trim leading spaces
         szTextA = szLine.substr(szLine.find_first_not_of(" ") != string::npos ? szLine.find_first_not_of(" ") : 0);
      
      // Convert string to UNICODE
      szTextW.assign(szTextA.begin(), szTextA.end());

      /// Append string to current thread
      MessageLog&  oThreadLog = m_oLogFile[iThreadID];
      oThreadLog.push_back( szTextW );
   }

   /// Display main UI thread and it's messages
   m_oThreadList.SetItemCount(m_oLogFile.size());
   ListView_SetItemState(m_oThreadList.Handle, getThreadIndex(g_iMainThreadID), LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

   oLogFile.close();
   return 1;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    PROTECTED METHODS
/// /////////////////////////////////////////////////////////////////////////////////////////

int  MainWindow::findNextException(int  iIndex, int  iMessageIndex)
{
   MessageLog&  log = m_oLogFile[getThreadID(iIndex)];
   int  count = log.size();
   
   // Find text
   for (int i = iMessageIndex; i < count; ++i)
      if (log[i].find(TEXT("EXCEPTION")) != wstring::npos)
         return i;
   
   // NOT FOUND: return -1
   return -1;
}

wstring  MainWindow::getMessageByIndex(int  iThreadIndex, int  iMessageIndex)
{
   MessageLog&  log = m_oLogFile[getThreadID(iThreadIndex)];
   
   // Return message, if found
   return (iMessageIndex < (int)log.size() ? log.at(iMessageIndex) : TEXT(""));
}

UINT  MainWindow::getMessageCount(int  iIndex)
{
   return m_oLogFile[getThreadID(iIndex)].size();
}

UINT  MainWindow::getThreadID(int  iIndex)
{
   int  i = 0;

   /// Iterate through thread map
   for (ThreadMap::iterator  it = m_oLogFile.begin(); it != m_oLogFile.end(); ++it)
   {
      // [FOUND] Set output and return true
      if (iIndex == i++)
         return it->first;
   }

   // [NOT FOUND] Return 0
   return 0;
}

UINT  MainWindow::getThreadIndex(int  iID)
{
   int  iIndex = 0;

   /// Iterate through thread map
   for (ThreadMap::iterator  it = m_oLogFile.begin(); it != m_oLogFile.end(); ++it)
   {
      // [FOUND] Set output and return true
      if (it->first == iID)
         return iIndex;

      iIndex++;
   }

   // [NOT FOUND] Return 0
   return 0;
}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                    PRIVATE METHODS
/// /////////////////////////////////////////////////////////////////////////////////////////

//MessageLog  MainWindow::getMessages(int  iIndex)
//{
//}

/// /////////////////////////////////////////////////////////////////////////////////////////
///                                   MESSAGE HANDLERS
/// /////////////////////////////////////////////////////////////////////////////////////////

bool  MainWindow::onCreate(CREATESTRUCT*  pCreationData)
{
   Rect  rcThreads  = Rect(0,   0, 150,                  ClientRect().Height()),
         rcMessages = Rect(150, 0, ClientRect().Width(), ClientRect().Height());

   // Creates ListViews
   m_oThreadList.Create(WS_VISIBLE|WS_CHILD|WS_BORDER|LVS_REPORT|LVS_OWNERDATA|LVS_SINGLESEL|LVS_SHOWSELALWAYS, this, IDC_THREAD_LIST, getInstance(), rcThreads);
   m_oMessageList.Create(WS_VISIBLE|WS_CHILD|WS_BORDER|LVS_REPORT|LVS_OWNERDATA|LVS_SINGLESEL|LVS_SHOWSELALWAYS, this, IDC_MESSAGE_LIST, getInstance(), rcMessages);

   // Setup ListViews
   m_oThreadList.SendMessageW(LVM_SETVIEW, LV_VIEW_DETAILS, NULL);
   m_oThreadList.InsertColumn(TEXT("Threads"), 150, 0);
   m_oThreadList.SetExtendedStyle(LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

   m_oMessageList.SendMessageW(LVM_SETVIEW, LV_VIEW_DETAILS, NULL);
   m_oMessageList.InsertColumn(TEXT("Messages"), ClientRect().Width() - 150, 0);
   m_oMessageList.SetExtendedStyle(LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

   // Accept files
   DragAcceptFiles(Handle, TRUE);

   return Handled;
}

bool  MainWindow::onClose()
{
   PostMessage(WM_QUIT, 0, 0);
   return Handled;
}

bool  MainWindow::onDestroy()
{
   // Cleanup
   m_oThreadList.Destroy();
   m_oMessageList.Destroy();

   return Handled;
}


bool  MainWindow::onDropFiles(HDROP  hDrop)
{
   TCHAR  szLogPath[MAX_PATH];

   /// Extract file path and open log
   if (DragQueryFile(hDrop, 0, szLogPath, MAX_PATH))
   {
      // Prepare
      /// Display threads and messages
      m_oThreadList.SetItemCount(0);
      m_oMessageList.SetItemCount(0);

      /// Open log
      ReadLog(szLogPath);
   }

   // Cleanup
   DragFinish(hDrop);
   return Handled;
}


bool  MainWindow::onKeyDown(const UINT  iVirtKey)
{
   // Letter 'e'
   if (HIBYTE(GetAsyncKeyState(VK_CONTROL)) && (iVirtKey == 'e' || iVirtKey == 'E'))
   {
      // Find next exception message
      int index = findNextException(max(0,m_oThreadList.GetNextItem(-1, LVNI_SELECTED)), 1+m_oMessageList.GetNextItem(-1, LVNI_SELECTED));
      m_oMessageList.SetItemState(-1, NULL, LVIS_SELECTED);

      // NOT FOUND: Jump to top
      if (index == -1)
         index = 0;

      // Select next item
      m_oMessageList.SetItemState(index, LVIS_SELECTED, LVIS_SELECTED);
      m_oMessageList.EnsureVisible(index, false);
   }

   return Handled; 
}


bool  MainWindow::onNotify(const UINT  iControlID, NMHDR*  pHeader)
{
   switch (pHeader->code)
   {
   /// [REQUEST DATA] Lookup data
   case LVN_GETDISPINFO:
      return onRetrieveItem(pHeader, reinterpret_cast<NMLVDISPINFO*>(pHeader)->item);

   /// [SELECTION CHANGED] Display new messages
   case LVN_ITEMCHANGED:
      if (pHeader->idFrom == IDC_THREAD_LIST)
      {
         NMLISTVIEW*  pItem = reinterpret_cast<NMLISTVIEW*>(pHeader);

         // Ensure item has been selected/de-selected
         if ((pItem->uOldState | pItem->uNewState) & LVIS_SELECTED)
            return onThreadSelectionChanged(pItem->iItem, pItem->uNewState & LVIS_SELECTED ? true : false);
      }
      break;

   /// [KEY DOWN]
   case LVN_KEYDOWN:
      if (pHeader->idFrom == IDC_MESSAGE_LIST)
         return onKeyDown(reinterpret_cast<NMLVKEYDOWN*>(pHeader)->wVKey);
      break;
   }

   return Unhandled;
}


bool  MainWindow::onRetrieveItem(NMHDR*  pHeader, LVITEM&  oItem)
{
   MessageLog*  pMessages = NULL;
   UINT         iThreadID;

   switch (pHeader->idFrom)
   {
   /// [THREAD LIST] Lookup and thread ID
   case IDC_THREAD_LIST:
      if (oItem.mask & LVIF_TEXT)
      {
         // Lookup thread ID 
         iThreadID = getThreadID(oItem.iItem);
         const TCHAR* format = (iThreadID == g_iMainThreadID ? TEXT("Main 0x%04x (%d)") : TEXT("Thread 0x%04x (%d)"));
         StringCchPrintf(oItem.pszText, oItem.cchTextMax, format, iThreadID, getMessageCount(oItem.iItem));
      }
      break;

   /// [MESSAGE LIST] Lookup thread and retrieve desired message 
   case IDC_MESSAGE_LIST:
      if (oItem.mask & LVIF_TEXT)
      {
         // Lookup message and display
         wstring  szMessage = getMessageByIndex(m_oThreadList.GetNextItem(-1, LVNI_SELECTED), oItem.iItem);
         StringCchCopy(oItem.pszText, oItem.cchTextMax, szMessage.c_str());
      }
      break;
   }

   return Handled;
}


bool  MainWindow::onSize(const UINT  eType, const UINT  iWidth, const UINT  iHeight)
{
   Rect  rcThreads  = Rect(0,   0, 150,    iHeight),
         rcMessages = Rect(150, 0, iWidth, iHeight);

   // Resize ListViews
   m_oThreadList.SetRect(rcThreads);
   m_oMessageList.SetRect(rcMessages);

   // Resize Message column
   m_oMessageList.SendMessage(LVM_SETCOLUMNWIDTH, 0, LVSCW_AUTOSIZE_USEHEADER);

   return Handled;
}

bool  MainWindow::onThreadSelectionChanged(int  iItem, bool  bSelected)
{
   MessageLog*  pMessages = NULL;

   /// Set or reset item count
   m_oMessageList.SetItemCount(bSelected ? getMessageCount(iItem) : 0);

   return Handled;
}

bool  MainWindow::onUnhandledMessage(UINT  iMessage, WPARAM  wParam, LPARAM  lParam)
{
   if (iMessage == WM_DROPFILES)
      return onDropFiles((HDROP)wParam);

   return Unhandled;
}