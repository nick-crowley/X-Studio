#pragma once
#include "Utils.hpp"
#include "BaseWindow.h"

namespace Windows
{
   class BaseWindow;

   /// Point
   class Point : public Utils::EmptyStruct<POINT>
   {
   public:
      Point() {};
      Point(const long  x, const long  y);
      Point(RECT  rcRect);
      Point(LPARAM  lParam);
      ~Point() {};

      Point  operator+ (const Point&  pt)       { return Point(x + pt.x, y + pt.y);              };
      Point  operator- (const Point&  pt)       { return Point(x - pt.x, y - pt.y);              };
   };


   /// Rect
   class Rect : public Utils::EmptyStruct<RECT>
   {
   public:
      Rect() {};
      Rect(const RECT&  rc);
      Rect(const long  iLeft, const long  iTop, const long  iRight, const long  iBottom);
      Rect(const Point&  ptMidPoint, const long  iWidth, const long  iHeight);
      virtual ~Rect() {};

      bool   Contains(const Point&  pt) const   { return PtInRect(this, pt) ? true : false;     };
      long   Width() const                      { return right - left;                          };
      long   Height() const                     { return bottom - top;                          };
      Point  MidPoint() const                   { return Point(Width() / 2, Height() / 2);      };

      Rect   operator+ (const Point&  pt);
   };


   /// Size
   class Size : public Utils::EmptyStruct<SIZE>
   {
   public:
      Size() {};
      Size(const long  width, const long  height);
      ~Size() {};
   };


   /// Cursor Position
   class CursorPos : public Point
   {
   public:
      CursorPos();
      CursorPos(const BaseWindow*  pWindow);

   private:
      bool  m_bScreen;
   };


   /// Drag-Drop data
   class DropData
   {
   public:
      DropData() {};
      virtual ~DropData() {};
   };
}