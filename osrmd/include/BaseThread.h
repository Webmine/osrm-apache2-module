#ifndef THREAD_INCLUDED
#define THREAD_INCLUDED


#include <pthread.h>

class BaseThread
{
public:
   BaseThread() {/* empty */}
   virtual ~BaseThread() {/* empty */}

   /** Returns true if the thread was successfully started, false if there was an error starting the thread */
   bool StartInternalThread()
   {
      return (pthread_create(&_thread, NULL, InternalThreadEntryFunc, this) == 0);
   }

   /** Will not return until the internal thread has exited. */
   void WaitForInternalThreadToExit()
   {
      (void) pthread_join(_thread, NULL);
   }

protected:
   /** Implement this method in your subclass with the code you want your thread to run. */
   virtual void InternalThreadEntry() = 0;

private:
   static void * InternalThreadEntryFunc(void * This) {((BaseThread *)This)->InternalThreadEntry(); return NULL;}

   pthread_t _thread;
};

#endif // THREAD_INCLUDED
