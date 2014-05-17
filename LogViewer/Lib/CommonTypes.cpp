#include "StdAfx.h"
#include "CommonTypes.h"

namespace Windows
{
   
   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                       RECTANGLE
   /// /////////////////////////////////////////////////////////////////////////////////////////

   Rect::Rect(const RECT&  rc)
   {
      left   = rc.left;
      right  = rc.right;
      top    = rc.top;
      bottom = rc.bottom;
   }


   Rect::Rect(const long  iLeft, const long  iTop, const long  iRight, const long  iBottom) 
   { 
      left   = iLeft;
      top    = iTop;
      right  = iRight;
      bottom = iBottom;
   }

   Rect::Rect(const Point&  ptMidPoint, const long  iWidth, const long  iHeight)
   {
      left  = ptMidPoint.x - (iWidth/2);
      right = ptMidPoint.x + (iWidth/2);

      top    = ptMidPoint.y - (iHeight/2);
      bottom = ptMidPoint.y + (iHeight/2);
   }

   Rect  Rect::operator+ (const Point&  pt)
   { 
      return Rect(left + pt.x, top + pt.y, right + pt.x, bottom + pt.y); 
   };

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                       POINT
   /// /////////////////////////////////////////////////////////////////////////////////////////

   Point::Point(const long  x, const long  y)
   {
      this->x = x;
      this->y = y;
   }

   Point::Point(RECT  rcRect)
   {
      x = rcRect.left;
      y = rcRect.top;
   }

   Point::Point(LPARAM  lParam)
   {
      x = GET_X_LPARAM(lParam);
      y = GET_Y_LPARAM(lParam);
   }

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                       SIZE
   /// /////////////////////////////////////////////////////////////////////////////////////////

   Size::Size(const long  width, const long  height)
   {
      cx = width;
      cy = height;
   }

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                     CURSOR POS
   /// /////////////////////////////////////////////////////////////////////////////////////////

   CursorPos::CursorPos() : m_bScreen(true)
   { 
      GetCursorPos(this);  
   }

   CursorPos::CursorPos(const BaseWindow*  pWindow) : m_bScreen(true)
   { 
      GetCursorPos(this); 
      ScreenToClient(pWindow->Handle, this);  
   }
   
} // END: namespace Windows