#include "StdAfx.h"
#include "BaseControl.h"

using Graphics::CustomDraw;

namespace Windows
{

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                    CONSTANTS / GLOBALS
   /// /////////////////////////////////////////////////////////////////////////////////////////

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                 CONSTRUCTION / DESTRUCTION
   /// /////////////////////////////////////////////////////////////////////////////////////////

   BaseControl::BaseControl(void)
   {
      // Zero drag + CustomDraw data
      m_pDragParent = NULL;
      m_pDragOver   = NULL;
      m_pDragData   = NULL;
      m_pCustomDraw = NULL;

      // Use SystemDraw by default
      SetCustomDraw(new Graphics::SystemDrawImpl);
   }

   BaseControl::~BaseControl(void)
   {
      // Delete CustomDraw Impl
      delete m_pCustomDraw;
   }

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                 PROTECTED  METHODS
   /// /////////////////////////////////////////////////////////////////////////////////////////

   bool  BaseControl::BeginDrag(HIMAGELIST  hImage, Point  ptHotSpot, DropData*  pDragData, UINT  iButton, BaseWindow*  pParent)
   {
      Point      ptInitial = Point(pParent->WindowRect()) + ptHotSpot;
      CursorPos  ptCursor;

      // [CHECK] Ensure button is Left/Right and image exists
      if (!hImage OR (iButton != MK_LBUTTON AND iButton != MK_RBUTTON))
         return false;

      // Store parent + data
      m_pDragParent = pParent;
      m_pDragData   = pDragData;
      m_iDragButton = iButton;
      m_pDragOver   = NULL;

      // Capture mouse
      SetCapture(Handle);

      // Define hotspot and Display image
      ImageList_BeginDrag(hImage, 0, ptInitial.x, ptInitial.y);     
      ImageList_DragEnter(pParent->Handle, ptCursor.x, ptCursor.y);
      return true;
   }


   void  BaseControl::onDragMouseMove()
   {
      // [CHECK] Ensure drag/drop in progress
      if (isDragDrop())
      {
         CursorPos     ptCursor;
         BaseWindow*   pDropTarget;
         bool          bAccepted = false;

         // Update image
         ImageList_DragMove(ptCursor.x, ptCursor.y);

         // [CHECK] Locate any child window beneath cursor
         if ((pDropTarget = LookupWindowAtPoint(CursorPos())) AND pDropTarget != m_pDragParent)
         {
            // [FOUND] Pass to 'OnDragging' handler
            bAccepted = (m_iDragButton == MK_LBUTTON ? pDropTarget->onMouseLeftDrag(this, m_pDragData) : pDropTarget->onMouseRightDrag(this, m_pDragData));

            // [CHANGED] Send WM_MOUSELEAVE to a window if we leave without dropping
            if (pDropTarget != m_pDragOver)
            {
               if (m_pDragOver)
                  m_pDragOver->SendMessage(WM_MOUSELEAVE, 0, 0);

               // Update current drag target
               m_pDragOver = pDropTarget;
            }
         }

         // [CURSOR] Display default or 'Blocked' cursor
         if (bAccepted OR pDropTarget == this)
            SetCursor((HCURSOR)getClassLong(GCLP_HCURSOR));
         else
            SetCursor(LoadCursor(NULL, IDC_NO));
      }
   }


   void  BaseControl::onDragMouseRelease()
   {
      // [CHECK] Ensure drag/drop in progress
      if (isDragDrop())
      {
         BaseWindow*   pDropTarget;

         // Hide/destroy image
         ImageList_DragLeave(m_pDragParent->Handle);
         ImageList_EndDrag();

         // [CHECK] Locate any child window beneath cursor
         if ((pDropTarget = LookupWindowAtPoint(CursorPos())) AND pDropTarget != m_pDragParent)
            // [FOUND] Pass to 'OnDragDrop' handler
            m_iDragButton == MK_LBUTTON ? pDropTarget->onMouseLeftDrop(this, m_pDragData) : pDropTarget->onMouseRightDrop(this, m_pDragData);

         // Release capture, cleanup
         ReleaseCapture();
         delete m_pDragData;
         m_pDragParent = NULL;
      }
   }


   void  BaseControl::SetCustomDraw(CustomDrawImpl*  pImpl)
   {
      // Replace existing CustomDraw
      if (m_pCustomDraw)
         delete m_pCustomDraw;
      m_pCustomDraw = pImpl;
   }

   void  BaseControl::SetMessageResult(LRESULT  iResult, const bool  bReflected)
   {
      // Set or Reflect result as desired
      bReflected ? getParent()->SetMessageResult(iResult) : BaseWindow::SetMessageResult(iResult);
   }

   // /////////////////////////////////////////////////////////////////////////////////////////
   //                                       STATIC
   // /////////////////////////////////////////////////////////////////////////////////////////

   HIMAGELIST  BaseControl::CreateDragImage(HICON  hIcon, const UINT  iSize)
   {
      HIMAGELIST  hDragImage;

      // Create 32bit ImageList and insert icon
      if (hDragImage = ImageList_Create(iSize, iSize, ILC_COLOR32 | ILC_MASK, 1, 1))
         ImageList_AddIcon(hDragImage, hIcon);

      // Return ImageList
      return hDragImage;
   }

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                       HANDLERS
   /// /////////////////////////////////////////////////////////////////////////////////////////

   bool  BaseControl::onCustomDraw(const UINT  iControlID, NMCUSTOMDRAW*  pDrawData)
   {
      UINT   iResultCode = 0;      // CustomDraw notification result code

      // Examine current draw stage
      switch (pDrawData->dwDrawStage)
      {
      /// [PRE-CYCLE] Request appropriate notifications
      case CDDS_PREERASE: 
      case CDDS_PREPAINT:
         switch (m_pCustomDraw->getMode())
         {
         // [OFF] Disable custom draw
         case CustomDrawImpl::Off:   
            iResultCode = CDRF_DODEFAULT; break;

         // [CYCLE] Request Cycle notifications
         case CustomDrawImpl::Cycle: 
            iResultCode = m_pCustomDraw->onDrawCycle(iControlID, CustomDraw(pDrawData->dwDrawStage, pDrawData));  break; // Return DoDefault/SkipDefault/FontChanged/NotifyPainted/NotifyErased
            
         // [ITEMS/SUBITEMS] Request Item + Cycle notifications
         default:
            iResultCode = CDRF_NOTIFYITEMDRAW | m_pCustomDraw->onDrawCycle(iControlID, CustomDraw(pDrawData->dwDrawStage, pDrawData));  break;  // Return DoDefault/SkipDefault/FontChanged/NotifyPainted/NotifyErased
         }
         break;

      /// [PRE-ITEM] Draw/Erase/Modify Item  (Optionally Request SubItems)
      case CDDS_ITEMPREERASE:
      case CDDS_ITEMPREPAINT:
         // Notify Pre-Item
         iResultCode = m_pCustomDraw->onDrawItem(iControlID, CustomDraw(pDrawData->dwDrawStage, pDrawData));        // Return DoDefault/SkipDefault/FontChanged/NotifyPainted/NotifyErased

         // [SUBITEM] Request SubItem drawing
         if (m_pCustomDraw->getMode() == CustomDrawImpl::SubItems)
            iResultCode |= CDRF_NOTIFYSUBITEMDRAW;
         break;

      /// [PRE-SUBITEM] Draw/Erase/Modify SubItem
      case CDDS_SUBITEM WITH CDDS_ITEMPREERASE:
      case CDDS_SUBITEM WITH CDDS_ITEMPREPAINT:
         iResultCode = m_pCustomDraw->onDrawSubItem(iControlID, CustomDraw(pDrawData->dwDrawStage, pDrawData));     // Return DoDefault/SkipDefault/FontChanged/NotifyPainted/NotifyErased
         break;

      /// [POST-SUBITEM] Cleanup SubItem
      case CDDS_SUBITEM WITH CDDS_ITEMPOSTERASE:
      case CDDS_SUBITEM WITH CDDS_ITEMPOSTPAINT:
         iResultCode = m_pCustomDraw->onDrawSubItem(iControlID, CustomDraw(pDrawData->dwDrawStage, pDrawData));    // Return DoDefault/SkipDefault
         break;

      /// [POST-ITEM] Cleanup Item
      case CDDS_ITEMPOSTERASE:
      case CDDS_ITEMPOSTPAINT:
         iResultCode = m_pCustomDraw->onDrawItem(iControlID, CustomDraw(pDrawData->dwDrawStage, pDrawData));       // Return DoDefault/SkipDefault
         break;

      /// [POST-CYLE] Cleanup cycle
      case CDDS_POSTERASE:
      case CDDS_POSTPAINT:
         iResultCode = m_pCustomDraw->onDrawCycle(iControlID, CustomDraw(pDrawData->dwDrawStage, pDrawData));      // Return DoDefault/SkipDefault
         break;
      }

      // Set/Reflect message result 
      SetMessageResult(iResultCode, isReflected());
      return Handled;
   }


   bool  BaseControl::onMouseMove(Point  oPoint, const UINT  iFlags)
   {
      // [CHECK] Is a drag/drop operation in progress?
      if (isDragDrop())
         onDragMouseMove();
      
      // Do not prevent message from being processed by base class (if any)
      return Unhandled;
   }


   bool  BaseControl::onMouseLeftUp(Point  oPoint, const UINT  iFlags)
   {
      if (isDragDrop() AND m_iDragButton == MK_LBUTTON)
         onDragMouseRelease();

      // Do not prevent message from being processed by base class (if any)
      return Unhandled;
   }

   
   bool  BaseControl::onMouseRightUp(Point  oPoint, const UINT  iFlags)
   {
      if (isDragDrop() AND m_iDragButton == MK_RBUTTON)
         onDragMouseRelease();

      // Do not prevent message from being processed by base class (if any)
      return Unhandled;
   }

} // END: namespace Windows