#pragma once

#include "BaseWindow.h"
#include <commctrl.h>

namespace Windows
{

   class ProgressBar : public BaseWindow
   {
   public:
      ProgressBar();
      ~ProgressBar(void);

      bool   SetState(UINT  iState)                 { return SendMessage(PBM_SETSTATE,   iState, NULL)    ? true : false;  };
      bool   SetPosition(int  iPosition)            { return SendMessage(PBM_SETPOS,     iPosition, NULL) ? true : false;  };
      bool   SetRange(int  iMin, int  iMax)         { return SendMessage(PBM_SETRANGE32, iMin, iMax)      ? true : false;  };
      void   SetMarquee(bool  bOn, UINT  iRefresh)  { SendMessage(PBM_SETMARQUEE, bOn ? TRUE : FALSE, iRefresh);           };

   protected:
      virtual const TCHAR*  getClassName() const  { return PROGRESS_CLASS; };
   };

} // END: namespace Windows