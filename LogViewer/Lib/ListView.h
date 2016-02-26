#pragma once

#include "BaseControl.h"
#include "Edit.h"
#include <commctrl.h>


namespace Windows
{
   class ListViewEdit;

   class ListView : public BaseControl
   {
   public:
      ListView();
      ~ListView(void);

      // Methods
      bool     BeginLabelEdit(const UINT  iItem, const UINT  iSubItem);
      bool     Create(DWORD  dwStyle, BaseWindow*  pParent, const UINT  iControlID, HINSTANCE  hInstance, Rect  oRect);
      bool     GetItemRect(const UINT  iItem, const UINT  iFlags, Rect&  rcItem);
      wstring  GetItemText(const UINT  iItem, const UINT  iSubItem, const UINT  iMaxLength);
      bool     GetSubItemRect(const UINT  iItem, const UINT  iSubItem, const UINT  iFlags, Rect&  rcItem);
      bool     HitTest(const Point  oPoint, LVHITTESTINFO*  pHitTest);
      bool     InsertColumn(const TCHAR*  szText, const UINT  iWidth, const UINT  iSubItem);

      // Helpers
      long     GetItemCount()                                                 { return SendMessage(LVM_GETITEMCOUNT, NULL, NULL);                                  };
      long     GetSelectedCount()                                             { return SendMessage(LVM_GETSELECTEDCOUNT, NULL, NULL);                              };
      bool     EnsureVisible(const int  iIndex, const bool  bPartial)         { return SendMessage(LVM_ENSUREVISIBLE, iIndex, bPartial?TRUE:FALSE) ? true : false; };
      long     GetNextItem(const int  iStart, const UINT  iFlags)             { return SendMessage(LVM_GETNEXTITEM, iStart, iFlags);                               };
      bool     RedrawItems(const int  iFirst, const int  iLast)               { return SendMessage(LVM_REDRAWITEMS, iFirst, iLast) ? true : false;                 };
      bool     SetColumnWidth(const int  iColumn, const int  iWidth)          { return SendMessage(LVM_SETCOLUMNWIDTH, iColumn, iWidth) ? true : false;            };
      bool     SetItemCount(const int  iCount, const DWORD  dwFlags = NULL)   { return SendMessage(LVM_SETITEMCOUNT, iCount, (LPARAM)dwFlags) ? true : false;      };
      void     SetItemState(const int  iIndex, DWORD  dwState, DWORD dwMask)  { ListView_SetItemState(Handle, iIndex, dwState, dwMask);                            };
      bool     SetExtendedStyle(DWORD  dwMask, DWORD  dwStyle)                { return SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, dwMask, dwStyle) ? true : false;  };
      
   protected:
      // Methods
      virtual const TCHAR*  getClassName() const                              { return WC_LISTVIEW; };

      // Handlers
      virtual bool  onCustomDraw(const UINT  iControlID, NMCUSTOMDRAW*  pDrawData);
      virtual bool  onNotify(const UINT  iControlID, NMHDR*  pHeader);
      
      // Custom Handlers
      virtual bool  onBeginDrag(NMHDR*  pHeader, NMLISTVIEW*  pItem)          { return Unhandled;             };
      virtual bool  onColumnClick(NMHDR*  pHeader, NMLISTVIEW*  pColumn)      { return Unhandled;             };
      virtual bool  onRetrieveItem(NMHDR*  pHeader, LVITEM&  oItem)           { return Unhandled;             };
      virtual bool  onLabelEditEnd(NMHDR*  pHeader, LVITEM&  oItem);

      // Label Edit
      ListViewEdit*  m_pEdit;
   
   };

   /*virtual long  onCustomDrawCycle(const UINT  iControlID, CustomDraw  oDrawData)    { return onCustomDrawCycle(&oDrawData);    };
   virtual long  onCustomDrawItem(const UINT  iControlID, CustomDraw  oDrawData)     { return onCustomDrawItem(&oDrawData);     };
   virtual long  onCustomDrawSubItem(const UINT  iControlID, CustomDraw  oDrawData)  { return onCustomDrawSubItem(&oDrawData);  };*/

   /*virtual long  onCustomDrawCycle(LVCustomDraw  oDrawData)                { return CustomDraw::DoDefault; };
   virtual long  onCustomDrawItem(LVCustomDraw  oDrawData)                 { return CustomDraw::DoDefault; };
   virtual long  onCustomDrawSubItem(LVCustomDraw  oDrawData)              { return CustomDraw::DoDefault; };*/

} // END: namespace Windows