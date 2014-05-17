#include "StdAfx.h"
#include "DirectShow.h"

using Utils::StringBufferW;

namespace Graphics
{
   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                   EXCEPTION
   /// /////////////////////////////////////////////////////////////////////////////////////////

   wstring  DirectShowException::getError()
   {
      StringBufferW  szError(MAX_ERROR_TEXT_LEN);

      // Lookup error
      AMGetErrorText((HRESULT)getCode(), szError, MAX_ERROR_TEXT_LEN);

      // Append to default COM error
      return ComException::getError() + TEXT(" - ") + (TCHAR*)szError;
   }

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                  MACROS / CONSTANTS
   /// /////////////////////////////////////////////////////////////////////////////////////////

   /// Macro: CHECK_HR(HRESULT)
   // Description: Throws a ComException if the operation fails 
   //
   #define CHECK_HR(hExpr)    { HRESULT  Macro_hResult; if (FAILED(Macro_hResult = (hExpr))) throw DirectShowException(HERE, Macro_hResult); }

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                 CONSTRUCTION / DESTRUCTION
   /// /////////////////////////////////////////////////////////////////////////////////////////

   DirectShow::DirectShow(void)
   {
      // Setup
      m_pParent    = NULL;
      m_iMessageID = NULL;
      m_eState     = Invalid;

      // Interfaces
      m_pGraphBuilder = NULL;
      m_pAudio        = NULL;
      m_pSeeking      = NULL;
      m_pEvents       = NULL;
      m_pSeeking      = NULL;
   }

   DirectShow::~DirectShow(void)
   {
      // Destroy
      Destroy();
   }

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                     PUBLIC  METHODS
   /// /////////////////////////////////////////////////////////////////////////////////////////

   bool  DirectShow::Create()
   {
      try
      {
         switch (getInternalState())
         {
         /// [PLAYING] Attempt to stop playback
         case Playing:
         case Paused:
            Stop();  
            // Fall through...

         /// [EXISTS] Destroy graph
         case Stopped:
         case Created:
            Destroy();
            break;
         } 

         // Create FilterGraph, Events, Controller and Volume Interfaces
         CHECK_HR(m_pGraphBuilder.CreateInstance(CLSID_FilterGraph));
         CHECK_HR(m_pGraphBuilder.QueryInterface(IID_IMediaEventEx, &m_pEvents));
         CHECK_HR(m_pGraphBuilder.QueryInterface(IID_IMediaControl, &m_pControl));
         CHECK_HR(m_pGraphBuilder.QueryInterface(IID_IBasicAudio, &m_pAudio));

         // Set state
         m_eState = Created;
         return true;
      }
      catch (DirectShowException&)
      {
         // [ERROR] Ensure graph never enters partial state
         Destroy();
         throw;
      }
   }


   bool  DirectShow::Destroy()
   {
      try
      {
         switch (getInternalState())
         {
         /// [NULL] Return TRUE
         case Invalid:
            return true;

         /// [PLAYING] Attempt to stop playback
         case Playing:
         case Paused:
            Stop();  
            break;
         } 

         // Unregister events
         CHECK_HR(m_pEvents->SetNotifyWindow(NULL, m_iMessageID, NULL));
      }
      catch (DirectShowException&  ex) {
         ex.Print(HERE);
      }

      // Release all + update state
      Release();
      m_eState = Invalid;
      return true;
   }


   bool  DirectShow::ProcessEvents()
   {
      long  iEvent = NULL, wParam = NULL, lParam = NULL;

      // [CHECK] Ensure graph exists
      if (getInternalState() == Invalid) 
         return false;

      /// Extract any events from the queue
      while (SUCCEEDED(m_pEvents->GetEvent(&iEvent, &wParam, &lParam, 0)))
      {
          switch (iEvent)
          { 
          // [EVENT] Completed
          case EC_COMPLETE:
             EnsureStateUpdated();
             onEvent(Event_Completed);
             break;
          } 

          // Cleanup
          CHECK_HR(m_pEvents->FreeEventParams(iEvent, wParam, lParam));
      }

      return true;
   }


   bool  DirectShow::RegisterWindow(BaseWindow*  pParent, const long  iMessageID)
   {
      // [CHECK] Ensure created + parent + message are present
      if (getInternalState() == Invalid OR !pParent OR !iMessageID)
         return false;

      // Store notification window data
      m_pParent    = pParent;
      m_iMessageID = iMessageID;

      // Register notification window
      CHECK_HR(m_pEvents->SetNotifyWindow((OAHWND)m_pParent->Handle, m_iMessageID, NULL));
      return true;
   }


   bool  DirectShow::LoadFile(const TCHAR*  szFullPath)
   {
      try
      {
         switch (getInternalState())
         {
         /// [EXISTS] Destroy graph
         case Playing:
         case Paused:
         case Stopped:
            Destroy();
            // Fall through..

         /// [NULL] Create graph using previous parameters
         case Invalid:
            Create();
            RegisterWindow(m_pParent, m_iMessageID);
            break;
         } 

         // Render file + create Seeking interface
         CHECK_HR(m_pGraphBuilder->RenderFile(szFullPath, NULL));
         CHECK_HR(m_pGraphBuilder.QueryInterface(IID_IMediaSeeking, &m_pSeeking));

         // Update state
         m_eState = Stopped;

         // [EVENT] Loaded
         onEvent(Event_Loaded);
         return true;
      }
      catch (DirectShowException&)
      {
         // [ERROR] Ensure graph never enters partial state
         Destroy();
         throw;
      }
   }


   long  DirectShow::getDuration()
   {
      ReferenceTime   oTime;
      DWORD           dwCaps = AM_SEEKING_CanGetDuration;

      // [CHECK] Ensure graph is rendered and has required capabilities
      if (isRendered() AND m_pSeeking->CheckCapabilities(&dwCaps) == S_OK)
         /// Retrieve duration
         CHECK_HR(m_pSeeking->GetDuration((__int64*)oTime));
      
      // Return duration in milliseconds
      return (long)oTime; 
   }


   long  DirectShow::getPosition()
   {
      ReferenceTime   oTime;

      // [CHECK] Ensure graph rendered
      if (isRendered())
         /// Retrieve position
         CHECK_HR(m_pSeeking->GetCurrentPosition((__int64*)oTime));

      // Return position in milliseconds
      return (long)oTime;
   }


   DirectShow::State   DirectShow::getState() throw()
   { 
      switch (m_eState)
      {
      case Invalid:
      case Created:
      case Stopped:
         return Stopped;

      default:
         return m_eState;
      }
   }


   DirectShow::State   DirectShow::getInternalState() throw()
   { 
      return m_eState;              
   }
   

   long  DirectShow::getVolume()
   {
      long  iVolume = MaxVolume;

      // [CHECK] Ensure graph is rendered
      if (isRendered()) 
         CHECK_HR(m_pAudio->get_Volume(&iVolume));

      // Map from -10,000->0 to 0->10,000
      return iVolume + 10000;
   }


   bool  DirectShow::Pause()
   {
      // [CHECK] Ensure graph is playing
      if (getInternalState() != Playing)
         return false;

      /// Pause graph + wait
      CHECK_HR(m_pControl->Pause());
      EnsureStateUpdated();

      // [EVENT] Paused
      onEvent(Event_Paused);
      return true;    
   }


   bool  DirectShow::Resume()
   {
      // [CHECK] Ensure graph is paused/stopped
      if (getInternalState() != Paused AND getInternalState() != Stopped)
         return false;

      /// Play graph + wait
      CHECK_HR(m_pControl->Run());
      EnsureStateUpdated();

      // [EVENT] Resumed
      onEvent(Event_Resumed);
      return true;
   }


   bool  DirectShow::Seek(const long  iPosition)
   {
      ReferenceTime   oTime(iPosition);
      DWORD           dwCaps = AM_SEEKING_CanSeekAbsolute;

      // [CHECK] Ensure graph is rendered and can be seeked
      if (!isRendered() OR m_pSeeking->CheckCapabilities(&dwCaps) != S_OK)
         return false;

      /// Set position
      CHECK_HR(m_pSeeking->SetPositions((__int64*)oTime, AM_SEEKING_AbsolutePositioning, NULL, AM_SEEKING_NoPositioning));
      return true;
   }


   bool  DirectShow::SetVolume(const long  iVolume)
   {
      // [CHECK] Ensure graph is rendered
      if (!isRendered()) 
         return false;

      /// Adjust volume
      CHECK_HR(m_pAudio->put_Volume(iVolume - 10000));  // Map from 0->10,000 to -10,000->0
      return true;
   }
   

   bool  DirectShow::Stop()
   {
      // [CHECK] Ensure playing or paused
      if (getInternalState() != Playing AND getInternalState() != Paused)
         return false;

      /// Stop graph + wait
      CHECK_HR(m_pControl->Stop());
      EnsureStateUpdated();

      /// Seek to the beginning
      Seek(0);

      // [EVENT] Completed
      onEvent(Event_Stopped);
      return true;
   }

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                 PROTECTED METHODS
   /// /////////////////////////////////////////////////////////////////////////////////////////

   bool  DirectShow::EnsureStateUpdated()
   {
      HRESULT        hResult;
      OAFilterState  eState;

      // [CHECK] Ensure graph exists
      if (getInternalState() == Invalid)
         return false;

      // Wait in 50ms intervals until state change is complete
      while (hResult = m_pControl->GetState(50, &eState))
      {
         if (hResult == VFW_S_STATE_INTERMEDIATE)
            continue;
         
         // Throw on error
         CHECK_HR(hResult);
      }

      // Convert state
      switch (eState)
      {
      case State_Stopped:  m_eState = Stopped;  break;
      case State_Running:  m_eState = Playing;  break;
      case State_Paused:   m_eState = Paused;   break;
      }

      return true;
   }


   bool  DirectShow::isRendered() throw()
   { 
      return getInternalState() >= Stopped; 
   }


   void  DirectShow::onEvent(const int  eEvent)
   {
      Patterns::Event  oEvent(eEvent);

      /// [COMPLETED] Seek graph to the beginning
      if (eEvent == Event_Completed)
         Seek(0);

      // Notify Observers
      NotifyObservers(&oEvent);
   }


   void  DirectShow::Release()
   {
      // Release interfaces in reverse order
      if (m_pEvents)
         m_pEvents.Release();

      if (m_pSeeking)
         m_pSeeking.Release();

      if (m_pControl)
         m_pControl.Release();

      if (m_pAudio)
         m_pAudio.Release();

      if (m_pGraphBuilder)
         m_pGraphBuilder.Release();
   }


} // END: namespace Graphics
