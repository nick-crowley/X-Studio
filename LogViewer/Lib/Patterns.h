#pragma once


namespace Patterns
{
   /// COMMAND: Command
   class Command
   {
   public:
      Command(void) { };
      virtual ~Command(void) { };

      virtual bool  Execute()=0;
   };


   /// OBSERVER: Event
   class Event
   {
   public:
      Event(const int  iID) : iID(iID) {};

      const int  iID;
   };

   /// OBSERVER: Observer
   template <typename T>
   class Observer
   {
   public:
      Observer() {};

      virtual void  onSubjectEvent(T*  pSubject, const Event*  pEvent) =0;
   };

   /// OBSERVER: Subject
   template <typename T>
   class Subject
   {
   public: 
      typedef  Observer<T>      TObserver;
      typedef  set<TObserver*>  ObserverSet;

   public:
      Subject() {};

      void  AttachObserver(TObserver*  p)  { m_pObservers.insert(p); };
      void  DetachObserver(TObserver*  p)  { m_pObservers.erase(p);  };

   protected:
      void  NotifyObservers(const Event*  pEvent)
      {
         for (ObserverSet::iterator  pIterator = m_pObservers.begin(); pIterator != m_pObservers.end(); ++pIterator)
            (*pIterator)->onSubjectEvent( static_cast<T*>(this), pEvent );
      };

   protected:
      ObserverSet  m_pObservers;
   };

   

} // namespace Patterns