#pragma once

#include <dshow.h>
#include "ComException.h"
#include "Patterns.h"

using Windows::BaseWindow;

namespace Graphics
{
   /// Exception
   class DirectShowException : public Utils::ComException
   {
   public:
      DirectShowException(const TCHAR*  szLocation, HRESULT  hResult) : ComException(szLocation, hResult) {};
      virtual ~DirectShowException() {};

   protected:
      virtual wstring  getType()   { return TEXT("Graphics::DirectShowException"); };
      virtual wstring  getError();
   };
  

   /// Direct Show
   class DirectShow : public Patterns::Subject<DirectShow>
   {
   public:
      enum Events { Event_Loaded, Event_Resumed, Event_Paused, Event_Stopped, Event_Completed };
      enum State  { Invalid, Created, Stopped, Playing, Paused };

      static const long  MinVolume = 0,
                         MaxVolume = 10000;

   public:
      DirectShow(void);
      ~DirectShow(void);

      // Creation / Destruction
      bool   Create();
      bool   Destroy();
      bool   ProcessEvents();
      bool   RegisterWindow(BaseWindow*  pParent, const long  iMessageID);

      // Playback control
      bool   LoadFile(const TCHAR*  szFullPath);
      long   getDuration();
      long   getPosition();
      State  getState() throw();
      long   getVolume();
      bool   Pause();
      bool   Resume();
      bool   Seek(const long  iPosition);
      bool   SetVolume(const long  iVolume);
      bool   Stop();

   protected:
      bool   EnsureStateUpdated();
      State  getInternalState() throw();
      bool   isRendered() throw();
      void   Release();

      virtual void   onEvent(const int  eEvent);
      
      // Data
      State             m_eState;
      long              m_iMessageID;
      BaseWindow*       m_pParent;


   protected:
      _COM_SMARTPTR_TYPEDEF(IGraphBuilder, __uuidof(IGraphBuilder));
      _COM_SMARTPTR_TYPEDEF(IMediaControl, __uuidof(IMediaControl));
      _COM_SMARTPTR_TYPEDEF(IMediaSeeking, __uuidof(IMediaSeeking));
      _COM_SMARTPTR_TYPEDEF(IMediaEventEx, __uuidof(IMediaEventEx));
      _COM_SMARTPTR_TYPEDEF(IBasicAudio, __uuidof(IBasicAudio));

      // Interfaces
      IGraphBuilderPtr  m_pGraphBuilder;
      IMediaEventExPtr  m_pEvents;
      IMediaControlPtr  m_pControl;
      IMediaSeekingPtr  m_pSeeking;
      IBasicAudioPtr    m_pAudio;
      

   protected:
      // Converts between milliseconds and 'reference time'
      class ReferenceTime
      {
      public:
         ReferenceTime()                        : m_iTime(0) {};
         ReferenceTime(long  iMilliseconds)     : m_iTime(iMilliseconds * 10000) {};
         ReferenceTime(__int64  iReferenceTime) : m_iTime(iReferenceTime) {};

         operator long()     { return (long)(m_iTime / 10000); };
         operator __int64()  { return m_iTime;                 };
         operator __int64*() { return &m_iTime;                };

      protected:
         __int64  m_iTime;
      };
   };


} // END: namespace Graphics
