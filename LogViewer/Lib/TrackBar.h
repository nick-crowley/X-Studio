#pragma once
#include "basewindow.h"

namespace Windows
{

   class TrackBar : public BaseWindow
   {
   public:
      TrackBar();
      ~TrackBar(void);

      bool  Create(const DWORD  dwStyle, BaseWindow*  pParent, const UINT  iControlID, HINSTANCE  hInstance, Rect  oRect);

      int   GetPosition()                                       { return SendMessage(TBM_GETPOS, NULL, NULL);                              };
      void  SetPosition(int  iPos, bool  bRedraw)               { SendMessage(TBM_SETPOS, bRedraw ? TRUE : FALSE, iPos);                   };
      void  SetRange(short  iMin, short  iMax, bool  bRedraw)   { SendMessage(TBM_SETRANGE, bRedraw ? TRUE : FALSE, MAKELONG(iMin, iMax)); };
      void  SetRangeEx(long  iLine, long  iPage)                { SendMessage(TBM_SETPAGESIZE, NULL, iPage); 
                                                                  SendMessage(TBM_SETLINESIZE, NULL, iLine);                               };
      void  SetMaximum(long  iMax, bool  bRedraw)               { SendMessage(TBM_SETRANGEMAX, bRedraw ? TRUE : FALSE, iMax);              };
      void  SetMinimum(long  iMin, bool  bRedraw)               { SendMessage(TBM_SETRANGEMIN, bRedraw ? TRUE : FALSE, iMin);              };
      void  SetTickFrequency(short  iFrequency)                 { SendMessage(TBM_SETTICFREQ, (WPARAM)iFrequency, NULL);                   };

   protected:
      virtual const TCHAR*  getClassName() const  { return TRACKBAR_CLASS; };
   };

} // END: namespace Windows