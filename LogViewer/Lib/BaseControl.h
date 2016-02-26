#pragma once
#include "BaseWindow.h"
#include "CustomDraw.h"

using Graphics::CustomDrawImpl;

namespace Windows
{
   /// Base Control
   class BaseControl : public Windows::BaseWindow
   {
   public:
      BaseControl(void);
      ~BaseControl(void);

      // Methods
      bool       BeginDrag(HIMAGELIST  hImage, Point  ptHotSpot, DropData*  pDragData, UINT  iButton, BaseWindow*  pParent);
      void       onDragMouseMove();
      void       onDragMouseRelease();
      void       SetCustomDraw(CustomDrawImpl*  pImpl);
      void       SetMessageResult(LRESULT  iResult, const bool  bReflected);
      
      // Helpers
      bool       isDragDrop() const      { return m_pDragParent != NULL;                                            };
      bool       isReflected() const     { return (getParent() ? getParent()->isChildReflected(WindowID) : false);  };

      // Handlers
      virtual bool  onCustomDraw(const UINT  iControlID, NMCUSTOMDRAW*  pDrawData);
      /*virtual long  onCustomDrawCycle(const UINT  iControlID, CustomDraw  oDrawData)     { return CustomDraw::DoDefault;  };
      virtual long  onCustomDrawItem(const UINT  iControlID, CustomDraw  oDrawData)      { return CustomDraw::DoDefault;  };
      virtual long  onCustomDrawSubItem(const UINT  iControlID, CustomDraw  oDrawData)   { return CustomDraw::DoDefault;  };*/
      virtual bool  onMouseMove(Point  oPoint, const UINT  iFlags);
      virtual bool  onMouseLeftUp(Point  oPoint, const UINT  iFlags); 
      virtual bool  onMouseRightUp(Point  oPoint, const UINT  iFlags); 

      // Data
      BaseWindow      *m_pDragParent,      // Draw operation owner, used for painting. Usually the parent
                      *m_pDragOver;        // Window currently beneath cursor while dragging
      DropData        *m_pDragData;        // Drag data
      UINT             m_iDragButton;      // Drag button
      CustomDrawImpl  *m_pCustomDraw;      // Custom draw implementation
   
   protected:
      // Static Methods
      static HIMAGELIST  CreateDragImage(HICON  hIcon, const UINT  iSize);

      
   };

} // END: namespace Windows
