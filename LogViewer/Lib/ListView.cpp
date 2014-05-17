#include "StdAfx.h"
#include "ListView.h"
#include "NotifyMessage.hpp"

using namespace Utils;

namespace Windows
{

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                    CONSTANTS / GLOBALS
   /// /////////////////////////////////////////////////////////////////////////////////////////

   class ListViewEdit : public Edit
   {
   public:
      ListViewEdit() {};
      bool  Create(const TCHAR*  szText, const UINT  iItem, const UINT  iSubItem, BaseWindow*  pParent, HINSTANCE  hInstance, Rect  oRect);

   protected:
      virtual bool  onKillFocus(HWND  hNextWnd);
      virtual bool  onKeyDown(const UINT  iVirtKey);

      UINT               m_iItem, m_iSubItem;     // Item identification
      static const UINT  s_iControlID = 42;       // Control ID
   };

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                 CONSTRUCTION / DESTRUCTION
   /// /////////////////////////////////////////////////////////////////////////////////////////

   ListView::ListView()
   {
      m_pEdit = NULL;
   }


   ListView::~ListView(void)
   {
      // Ensure edit is deleted (should not happen)
      if (m_pEdit)
         delete m_pEdit;
   }

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                       METHODS
   /// /////////////////////////////////////////////////////////////////////////////////////////

   bool  ListView::BeginLabelEdit(const UINT  iItem, const UINT  iSubItem)
   {
      Rect     rcItem;
      wstring  szItem;

      // Lookup sub-item rectangle
      if (GetSubItemRect(iItem, iSubItem, LVIR_LABEL, rcItem))
      {
         // Get item text
         szItem = GetItemText(iItem, iSubItem, 256);

         // Create Edit control
         m_pEdit = new ListViewEdit;
         m_pEdit->Create(szItem.c_str(), iItem, iSubItem, this, Instance, rcItem);
         SetWindowFont(m_pEdit->Handle, GetStockFont(ANSI_VAR_FONT), FALSE);
         
         // Display Edit and focus
         m_pEdit->ShowAndUpdate();
         m_pEdit->SetFocus();
         return true;
      }
      
      return false;
   }


   bool  ListView::Create(DWORD  dwStyle, BaseWindow*  pParent, const UINT  iControlID, HINSTANCE  hInstance, Rect  oRect)
   {
      return BaseWindow::Create(NULL, dwStyle, NULL, pParent, iControlID, hInstance, &oRect);
   }


   bool  ListView::GetItemRect(const UINT  iItem, const UINT  iFlags, Rect&  rcItem)
   {
      rcItem.left = iFlags;
      return SendMessage(LVM_GETITEMRECT, iItem, (LPARAM)(RECT*)&rcItem) ? true : false;
   }

   wstring  ListView::GetItemText(const UINT  iItem, const UINT  iSubItem, const UINT  iMaxLength)
   {
      StringBufferW  szBuffer(iMaxLength);

      // [VIRTUAL] Retrieve text via LVN_GETDISPINFO
      if (Style INCLUDES LVS_OWNERDATA)
      {
         NotifyMessage<NMLVDISPINFO>  oMessage(LVN_GETDISPINFO, Handle, WindowID);

         // Prepare item request
         oMessage.item.iItem      = iItem;
         oMessage.item.iSubItem   = iSubItem;
         oMessage.item.mask       = LVIF_TEXT;
         oMessage.item.pszText    = szBuffer;
         oMessage.item.cchTextMax = iMaxLength;
         oMessage.Send(Handle);
      }
      else
         // [NON-VIRTUAL] Retrieve via LVM_GETITEMTEXT
         ListView_GetItemText(Handle, iItem, iSubItem, szBuffer, iMaxLength);

      // Return as string
      return szBuffer;
   }

   bool  ListView::GetSubItemRect(const UINT  iItem, const UINT  iSubItem, const UINT  iFlags, Rect&  rcItem)
   {
      rcItem.left = iFlags;
      rcItem.top  = iSubItem;
      return SendMessage(LVM_GETSUBITEMRECT, iItem, (LPARAM)(RECT*)&rcItem) ? true : false;
   }

   bool  ListView::HitTest(const Point  oPoint, LVHITTESTINFO*  pHitTest)
   {
      pHitTest->pt = (POINT)oPoint;
      return ListView_SubItemHitTest(Handle, pHitTest) != -1;
   }

   bool  ListView::InsertColumn(const TCHAR*  szText, const UINT  iWidth, const UINT  iSubItem)
   {
      EmptyStruct<LVCOLUMN>  oColumn;

      // Set properties
      oColumn.mask     = LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
      oColumn.pszText  = const_cast<TCHAR*>(szText);
      oColumn.cx       = iWidth;
      oColumn.iSubItem = iSubItem;

      // Insert
      if (SendMessage(LVM_INSERTCOLUMN, iSubItem, reinterpret_cast<LPARAM>(&oColumn)) == -1)
         throw WindowsException(HERE, TEXT("Unable to insert column"));
      
      return true;
   }

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                     MESSAGE HANDLERS
   /// /////////////////////////////////////////////////////////////////////////////////////////
   
   bool  ListView::onCustomDraw(const UINT  iControlID, NMCUSTOMDRAW*  pDrawData)
   {
      // Ignore CustomDraw notifications from the Header ctrl
      return iControlID == WindowID ? BaseControl::onCustomDraw(iControlID, pDrawData) : Unhandled;
   }


   bool  ListView::onLabelEditEnd(NMHDR*  pHeader, LVITEM&  oItem)
   {
      // Delete EditCtrl
      delete m_pEdit;
      m_pEdit = NULL;

      return Handled;
   }


   bool  ListView::onNotify(const UINT  iControlID, NMHDR*  pHeader)
   {
      bool   bResult = Unhandled;

      try
      {
         switch (pHeader->code)
         {
         case LVN_BEGINDRAG:     bResult = onBeginDrag(pHeader, reinterpret_cast<NMLISTVIEW*>(pHeader));             break;
         case LVN_COLUMNCLICK:   bResult = onColumnClick(pHeader, reinterpret_cast<NMLISTVIEW*>(pHeader));           break;
         case LVN_GETDISPINFO:   bResult = onRetrieveItem(pHeader, reinterpret_cast<NMLVDISPINFO*>(pHeader)->item);  break;
         case LVN_ENDLABELEDIT:  bResult = onLabelEditEnd(pHeader, reinterpret_cast<NMLVDISPINFO*>(pHeader)->item);  break;

         default:                bResult = BaseControl::onNotify(iControlID, pHeader);                                break;
         }

         return bResult;
      }
      catch (WindowsException&  ex) 
      {
         ex.Display(HERE, Handle);
         return Unhandled;
      }
   }

   // /////////////////////////////////////////////////////////////////////////////////////////
   //                                     EDIT: METHODS 
   // /////////////////////////////////////////////////////////////////////////////////////////

   bool  ListViewEdit::Create(const TCHAR*  szText, const UINT  iItem, const UINT  iSubItem, BaseWindow*  pParent, HINSTANCE  hInstance, Rect  oRect)
   {
      // Store item index
      m_iItem    = iItem;
      m_iSubItem = iSubItem;

      // Create Edit + Subclass
      return Edit::Create(szText, WS_CHILD | WS_BORDER | WS_VISIBLE | ES_LEFT, pParent, s_iControlID, hInstance, oRect) AND Subclass();
   }

   // /////////////////////////////////////////////////////////////////////////////////////////
   //                                   EDIT: HANDLERS
   // /////////////////////////////////////////////////////////////////////////////////////////

   bool  ListViewEdit::onKillFocus(HWND  hNextWnd)
   {
      Destroy();
      return Unhandled;  // Pass to base
   }


   bool  ListViewEdit::onKeyDown(const UINT  iVirtKey)
   {
      NotifyMessage<NMLVDISPINFO>  oMessage(LVN_ENDLABELEDIT, Handle, WindowID);
      wstring                      szText;

      switch (iVirtKey)
      {
      // [RETURN] Pass text to parent, then destroy
      case VK_RETURN:
         szText = getText();

         oMessage.item.mask     = LVIF_TEXT;
         oMessage.item.pszText  = const_cast<TCHAR*>(szText.c_str());
         oMessage.item.iItem    = m_iItem;
         oMessage.item.iSubItem = m_iSubItem;

         oMessage.Send(getParentHandle());

         // Fall through...

      // [ESCAPE] Destroy by switching focus to parent
      case VK_ESCAPE:
         getParent()->SetFocus();
         break;
      }

      return Unhandled;  // Pass to base
   }

} // END: namespace Windows