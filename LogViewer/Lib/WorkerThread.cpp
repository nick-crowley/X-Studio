#include "StdAfx.h"
#include "WorkerThread.h"

using Windows::WindowsException;

namespace Threads
{
   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                 CONSTRUCTION / DESTRUCTION
   /// /////////////////////////////////////////////////////////////////////////////////////////

   WorkerThread::WorkerThread(void)
   {
      m_hThread = NULL;
      m_eState  = Nothing;

      /// Create unsignaled 'stopping' event
      if ((m_hAborted = CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL)
         throw WindowsException(HERE, TEXT("Unable to create thread 'stopping' event"));
   }

   WorkerThread::~WorkerThread(void)
   {
      try
      {
         // [CHECK] Stop and terminate thread if necessary
         if (m_eState == Running || m_eState == Suspended)
            Stop(500);
      }
      catch (WindowsException&  e) {
         e.Print(HERE);
      }

      // Destroy 'stopping' event
      CloseHandle(m_hAborted);
   }

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                      STATIC METHODS
   /// /////////////////////////////////////////////////////////////////////////////////////////

   DWORD CALLBACK  WorkerThread::ThreadWrapper(void*  pParameter)
   {
      WorkerThread*  pThread = reinterpret_cast<WorkerThread*>(pParameter);
      DWORD          dwReturn;

      // Set 'running' state
      pThread->m_eState = Running;

      /// Execute worker thread function
      dwReturn = pThread->ThreadMain();

      // Set 'terminated' state and zero thread handle
      pThread->m_eState  = Terminated;
      pThread->m_hThread = NULL;
      return dwReturn;
   }

   /// /////////////////////////////////////////////////////////////////////////////////////////
   ///                                       METHODS
   /// /////////////////////////////////////////////////////////////////////////////////////////

   bool  WorkerThread::Create(const bool  bSuspended)
   {
      // [CHECK] Ensure thread does not exist yet
      if (m_eState != Nothing)
         return false;

      /// Launch thread
      m_hThread = CreateThread(NULL, NULL, ThreadWrapper, reinterpret_cast<void*>(this), (bSuspended ? CREATE_SUSPENDED : NULL), NULL);

      if (!m_hThread)
         throw WindowsException(HERE, TEXT("Unable to create thread"));

      // [SUSPENDED] Set state
      if (bSuspended)
         m_eState = Suspended;

      return true;
   }


   bool  WorkerThread::Stop(const DWORD  dwMilliseconds)
   {
      // [CHECK] Ensure thread exists
      if (m_eState == Nothing || m_eState == Terminated)
         return false;

      /// Signal thread to stop
      if (!SetEvent(m_hAborted))
         throw WindowsException(HERE, TEXT("Unable to signal thread to stop"));
      
      /// Wait for thread to stop
      switch (WaitForSingleObject(m_hThread, dwMilliseconds))
      {
      // [STOPPED] Success!
      case WAIT_OBJECT_0: 
         return true;

      // [TIMEOUT] Terminate
      case WAIT_TIMEOUT:
         return Terminate();

      // [ERROR]
      default:
         throw WindowsException(HERE, TEXT("Unable to wait on stopping thread"));
      }
   }

   bool  WorkerThread::Terminate()
   {
      // [CHECK] Ensure thread exists
      if (m_eState == Nothing || m_eState == Terminated)
         return false;

      /// Terminate thread
      if (!TerminateThread(m_hThread, 1))
         throw WindowsException(HERE, TEXT("Unable to terminate thread"));
      
      m_eState = Terminated;
      m_hThread = NULL;
      return true;
   }

   bool  WorkerThread::Suspend()
   {
      // [CHECK] Ensure thread is running
      if (m_eState != Running)
         return false;

      /// Suspend thread
      if (SuspendThread(m_hThread) == -1)
         throw WindowsException(HERE, TEXT("Unable to suspend thread"));
      
      m_eState = Suspended;
      return true;
   }

   bool  WorkerThread::Resume()
   {
      // [CHECK] Ensure thread is suspended
      if (m_eState != Suspended)
         return false;

      /// Resume thread
      if (ResumeThread(m_hThread) == -1)
         throw WindowsException(HERE, TEXT("Unable to resume thread"));
      
      m_eState = Running;
      return true;
   }

   bool  WorkerThread::isStopping()
   {
      // [CHECK] Check whether 'stop' event has been signalled
      return (WaitForSingleObject(m_hAborted, 0) == WAIT_OBJECT_0);
   }

} // END: namespace Threads