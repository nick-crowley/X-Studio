
#pragma once
#include "Macros.h"

namespace Graphics
{
   /// Generic CustomDraw Data
   class CustomDraw
   {
   public:
      // Stages and Return-Codes
      enum  DrawCommand  { SkipDefault = CDRF_SKIPDEFAULT, DoDefault = CDRF_DODEFAULT, FontChanged = CDRF_NEWFONT, NotifyPainted = CDRF_NOTIFYPOSTPAINT, NotifyErased = CDRF_NOTIFYPOSTERASE };
      enum  DrawStage    { PrePaint = CDDS_PREPAINT, PostPaint = CDDS_POSTPAINT, PreErase = CDDS_PREERASE, PostErase = CDDS_POSTERASE };

   public:
      CustomDraw(DWORD  dwStage, NMCUSTOMDRAW*  pData) : m_pData(pData)  {};

      // Properties
      READONLY_PROPERTY(DrawStage, Stage,    identifyDrawStage(m_pData->dwDrawStage));
      READONLY_PROPERTY(ULONG_PTR, Item,     m_pData->dwItemSpec);
      READONLY_PROPERTY(LPARAM,    Param,    m_pData->lItemlParam);
      READONLY_PROPERTY(UINT,      State,    m_pData->uItemState);
      READONLY_PROPERTY(Rect,      ItemRect, m_pData->rc);
      READONLY_PROPERTY(HDC,       DC,       m_pData->hdc);

   protected:
      CustomDraw(CustomDraw*  pData) : m_pData(pData->m_pData)  {};
      
      static DrawStage  identifyDrawStage(DWORD  dwStage);

      NMCUSTOMDRAW*  m_pData;
   };


   /// ListView CustomDraw Data
   class LVCustomDraw : public CustomDraw
   {
   public:
      LVCustomDraw(CustomDraw*  pBase) : CustomDraw(pBase) { m_pExtra = (NMLVCUSTOMDRAW*)m_pData; };

      // Properties
      READONLY_PROPERTY(UINT,      Alignment,  m_pExtra->uAlign);
      READONLY_PROPERTY(UINT,      SubItem,    m_pExtra->iSubItem);
      READONLY_PROPERTY(Rect,      TextRect,   m_pExtra->rcText);
      READONLY_PROPERTY(DWORD,     ItemType,   m_pExtra->dwItemType);
      READONLY_PROPERTY(COLORREF,  FaceColour, m_pExtra->clrFace);
      READWRITE_PROPERTY(COLORREF, BackColour, m_pExtra->clrTextBk);
      READWRITE_PROPERTY(COLORREF, TextColour, m_pExtra->clrText);

   protected:
      NMLVCUSTOMDRAW*  m_pExtra;
   };

   /// CustomDraw Implementation
   class CustomDrawImpl
   {
   public:
      enum  DrawMode   { Off, Cycle, Items, SubItems };

   public:
      CustomDrawImpl() : m_eMode(Off)  {};
      virtual ~CustomDrawImpl()        {};

      // Methods
      DrawMode  getMode() const                   { return m_eMode;   };
      void      SetMode(const DrawMode  eMode)    { m_eMode = eMode;  };

      // Drawing Handlers
      virtual long  onDrawCycle(const UINT  iControlID, CustomDraw  oDrawData) = NULL;
      virtual long  onDrawItem(const UINT  iControlID, CustomDraw  oDrawData) = NULL;
      virtual long  onDrawSubItem(const UINT  iControlID, CustomDraw  oDrawData) = NULL;

   protected:
      DrawMode  m_eMode;
   };


   // Implementation:  Off
   class SystemDrawImpl : public CustomDrawImpl
   {
   public:
      SystemDrawImpl()   { SetMode(Off); };

      virtual long  onDrawCycle(const UINT  iControlID, CustomDraw  oDrawData)     { return CustomDraw::DoDefault;  };
      virtual long  onDrawItem(const UINT  iControlID, CustomDraw  oDrawData)      { return CustomDraw::DoDefault;  };
      virtual long  onDrawSubItem(const UINT  iControlID, CustomDraw  oDrawData)   { return CustomDraw::DoDefault;  };
   };


   // Implementation:  ListView
   class CustomDrawListView : public CustomDrawImpl
   {
   public:
      virtual long  onDrawCycle(const UINT  iControlID, CustomDraw  oDrawData)      { return onDrawCycle(iControlID, LVCustomDraw(&oDrawData));    };
      virtual long  onDrawItem(const UINT  iControlID, CustomDraw  oDrawData)       { return onDrawItem(iControlID, LVCustomDraw(&oDrawData));     };
      virtual long  onDrawSubItem(const UINT  iControlID, CustomDraw  oDrawData)    { return onDrawSubItem(iControlID, LVCustomDraw(&oDrawData));  };

      virtual long  onDrawCycle(const UINT  iControlID, LVCustomDraw  oDrawData)    { return CustomDraw::DoDefault;  };
      virtual long  onDrawItem(const UINT  iControlID, LVCustomDraw  oDrawData)     { return CustomDraw::DoDefault;  };
      virtual long  onDrawSubItem(const UINT  iControlID, LVCustomDraw  oDrawData)  { return CustomDraw::DoDefault;  };
   };

} // END: namespace Graphics 
