#pragma once


namespace Graphics
{
   
   /// Base class
   class GDIObject 
   {
   friend class DeviceContext;

   protected:
      GDIObject();
      GDIObject(HGDIOBJ  hObject);
   public:
      virtual ~GDIObject();

      // Copy
      GDIObject(GDIObject&  rThat)             { Transfer(rThat);               };
      GDIObject& operator =(GDIObject&  rThat) { Transfer(rThat); return *this; };

      // Methods
      bool     Attach(HGDIOBJ  hObject);
      void     Detach();
      bool     Destroy();

   protected:
      void     Transfer(GDIObject&  rThat);

      HGDIOBJ  m_hObject;
   };


   /// Brush
   class Brush : public GDIObject
   {
   public:
      Brush() {};
      Brush(HBRUSH  hBrush);
      Brush(COLORREF  clColour);
      Brush(int  iSysColour);

      operator HBRUSH() { return (HBRUSH)m_hObject; };
   };

   // StockBrush
   class StockBrush : public Brush 
   {
   public: StockBrush(int  iIndex) : Brush((HBRUSH)GetStockObject(iIndex)) {};
   };


   /// Pen
   class Pen : public GDIObject
   {
   public:
      Pen() {};
      Pen(HPEN  hPen);
      Pen(COLORREF  clColour);
      Pen(int  iWidth, COLORREF  clColour);
      Pen(int  iStyle, int  iWidth, COLORREF  clColour);
   };

   // StockPen
   class StockPen : public Pen
   {
   public: StockPen(int  iIndex) : Pen((HPEN)GetStockObject(iIndex)) {};
   };


   /// Font
   class Font : public GDIObject
   {
   public:
      Font() {};
      Font(HFONT  hFont);
      Font(wstring  szFont, int  iHeight);
      Font(wstring  szFont, int  iHeight, bool  bBold, bool  bItalic, bool  bUnderline);

   private:
      static HFONT  Create(const TCHAR*  szFont, int  iHeight, int  iWeight, BOOL  bItalic, BOOL  bUnderline);
   };

   // StockFont
   class StockFont : public Font
   {
   public: StockFont(int  iIndex) : Font((HFONT)GetStockObject(iIndex)) {};
   };

} // END: namespace Graphics