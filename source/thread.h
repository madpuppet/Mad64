#pragma once

/**************************************************************************
Thread  -  simple single job thread

just does one job and finishes
usage:
create a class that inherits from Thread, using whatever constructors & initialisers to prepare the data
implement a Go() function to do the work.  When Go() returns, the IsFinished() function will return true

MyThreadClass *pMyThread = new MyThreadClass( ... work data ... )
pMyThread->Start();  // creates the thread and calls the Go() function
if (pMyThread->IsFinished())
delete pMyThread;

***************************************************************************/

#include <thread>
#include <condition_variable>
#include <mutex>
#include <string>
using namespace std;

#define NULL_THREAD std::thread::id()
typedef std::thread::id ThreadID;

class Thread
{
public:
    static ThreadID CurrentThreadID();

    Thread(const string &name);
    virtual ~Thread();

    /**
    * Starts this thread.
    **/

    void Start(bool lowPriority = true);
    void Stop();

    /**
    * Request thread to terminate
    * child should take whatever steps necessary to signal the thread to exit
    **/
    virtual void Terminate() { m_terminate = true; }

    /**
    * Does not return until this thread exits.
    **/
    void WaitForThreadCompletion();
    bool IsFinished() const { return m_finished; }
    int  GetResult() const { return m_result; }

    /**
    * Abstract function -- your thread code goes in here, in a derived class.
    *
    * Return an integer value that can be retrieved by the host application.
    **/
    virtual int Go() = 0;

    // name of thread..
    const string &GetName() { return m_name; }

    // run the thread (called from Main thread routine 
    // - calls Go() internally, and then exits the thread with the return code from Go())
    void Begin();

protected:
    std::thread m_thread;

    int m_result;                   // thread quit result
    volatile bool m_finished;       // this is set if the thread has finished
    volatile bool m_terminate;
    string m_name;                // thread name - appears in dev studio list
};

class Semaphore
{
public:

    /**
    * @param maximumValue The maximum value for this semaphore.
    **/
    Semaphore(int initialCount, int maximumCount);
    ~Semaphore();

    /**
    * Block this thread's execution until this semaphore becomes available.
    * After Wait() returns, the caller is responsible for calling Signal() as soon
    * as possible, once it has finished using whatever resource the scSemaphore is
    * protecting.
    **/
    void Wait();

    /**
    * When finished using the resource protected by this semaphore, call Signal()
    * to let the system know that someone else may now make use of that resource.
    **/
    void Signal();

protected:
    std::mutex m_mutex;
    std::condition_variable m_variable;
    int m_count;
};

class Mutex
{
public:
    Mutex();
    ~Mutex();

    void Lock();
    void Release();

protected:
    std::mutex m_mutex;
};

class ScopedMutexLock
{
public:
    ScopedMutexLock(Mutex &mutex) : m_mutex(&mutex)
    {
        m_mutex->Lock();
    }
    ~ScopedMutexLock()
    {
        m_mutex->Release();
    }
protected:
    Mutex *m_mutex;
};

