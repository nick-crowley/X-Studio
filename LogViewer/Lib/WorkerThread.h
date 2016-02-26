#pragma once


namespace Threads
{

   class WorkerThread
   {
   public:
      enum  ThreadState { Nothing, Running, Suspended, Terminated };
      //typedef DWORD (__stdcall *ThreadProc)(Thread*);

   public:
      WorkerThread(void);
      virtual ~WorkerThread(void);

      bool  Stop(const DWORD  dwMilliseconds);
      bool  Terminate();

      bool  Suspend();
      bool  Resume();

      ThreadState  getState()  { return m_eState; };

   protected:
      bool  Create(const bool  bSuspended);
      bool  isStopping();

      virtual DWORD  ThreadMain()=0;

      HANDLE m_hThread;

   private:
      static DWORD CALLBACK  ThreadWrapper(void*  pParameter);

      //ThreadProc  m_pfnThread;
      HANDLE      m_hAborted;

      volatile ThreadState  m_eState;
   };

} // END: namespace Threads