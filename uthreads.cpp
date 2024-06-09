#include <iostream>
#include <string>
#include <map>
#include <list>
#include <sys/time.h>
#include <bits/stdc++.h>

#include "thread.h"
#include "uthreads.h"

using namespace std;

int USECS_IN_SEC = 1000000;

int quantum_length;
int quantum_counter;
list<Thread *> ready_queue;
std::map<int, Thread *> threads;
std::map<int, int> sleeping_threads;
Thread *running_thread = nullptr;
priority_queue<int, vector<int>, greater<int> > indexes; // Min heap
struct itimerval timer = {0};
sigset_t signals;

// Add counter map for blocked threads


void print_system_error (string error)
{
  std::cerr << "system error: " << error << std::endl;
  exit (1);
}

void print_library_error (string error)
{
  std::cerr << "thread library error: " << error << std::endl;
  return;
}

void blockTimer ()
{
  if (sigprocmask (SIG_BLOCK, &signals, NULL) == -1)
  {
    print_system_error ("sigblock error");
    exit (1);
  }
}

void resumeTimer ()
{
  if (sigprocmask (SIG_UNBLOCK, &signals, NULL) == -1)
  {
    print_system_error ("sigunblock error");
    exit (1);
  }
}

void switch_threads (bool is_terminating = false)
{
  if (ready_queue.empty () && running_thread != nullptr)
  {
      running_thread->set_state (READY);
      running_thread->set_state (RUNNING);
      return;
  };
  if (sigsetjmp (*(running_thread->get_env ()), 1) == 1)
  {
    return;
  }
  // Save current thread state
  if (!is_terminating)
  {
    running_thread->set_state (READY);
    // Move running thread to the ready queue
    ready_queue.push_back (running_thread);
  }
  // Get the next thread to run
  running_thread = ready_queue.front ();
  ready_queue.pop_front ();

  // Set the state to RUNNING and increment quantum counter
  running_thread->set_state (RUNNING);
  siglongjmp (*(running_thread->get_env ()), 1);

  if (setitimer (ITIMER_VIRTUAL, &timer, nullptr) == -1)
  {
    print_system_error ("setitimer error.");
    exit (1);
  }
}

void timer_handler (int sig, bool isTerminating = false)
{
  // Increment quantum counter and switch threads if needed
  quantum_counter++;
  vector<int> to_awake;

  for (auto thread = sleeping_threads.begin(); thread != sleeping_threads.end(); )
  {
    thread->second--;
    if (thread->second == 0)
    {
      to_awake.push_back (thread->first);
      sleeping_threads.erase (thread->first);

    }
  }

  for (int tid: to_awake)
  {
    sleeping_threads.erase (tid);
    if (threads[tid]->get_state () == READY)
    {
      ready_queue.push_back (threads[tid]);
    }
  }
  switch_threads (isTerminating);

}

void timer_handler_no_bool (int sig)
{
  timer_handler (sig, false);
}

int uthread_init (int quantum_usecs)
{
  if (quantum_usecs <= 0)
  {
    print_library_error ("quantum_usecs must be positive");
    return -1;
  }
  for (int i = 1; i < MAX_THREAD_NUM; i++)
  {
    indexes.push (i);
  }
  quantum_counter = 0;
  quantum_length = quantum_usecs;

  // Set up timer and signal handler
  struct sigaction sa = {0};

  sa.sa_handler = &timer_handler_no_bool;
  sigemptyset (&sa.sa_mask);
  sa.sa_flags = 0;

  if (sigaction (SIGVTALRM, &sa, NULL) < 0)
  {
    print_system_error ("sigaction error.");
  }

  timer.it_value.tv_sec = quantum_usecs / USECS_IN_SEC;
  timer.it_value.tv_usec = quantum_usecs % USECS_IN_SEC;
  timer.it_interval.tv_sec = quantum_usecs / USECS_IN_SEC;
  timer.it_interval.tv_usec = quantum_usecs % USECS_IN_SEC;

  // Start a virtual timer. It counts down whenever this process is executing.
  if (setitimer (ITIMER_VIRTUAL, &timer, nullptr))
  {
    print_system_error ("setitimer error.");
    exit (1);
  }
  // create sigset for blocking signals later on
  if (sigemptyset (&signals) == -1)
  {
    print_system_error ("sigemptyset error.");
    exit (1);
  }
  if (sigaddset (&signals, SIGVTALRM) == -1)
  {
    print_system_error ("sigaddset error.");
    exit (1);
  }
  Thread *main = new Thread (0);
  threads[0] = main;
// Add signals somehow
  running_thread = main;
  main->set_state (RUNNING);
  quantum_counter = 1;
  return 0;
}

int uthread_spawn (thread_entry_point entry_point)
{
  blockTimer ();
  if (!entry_point)
  {
    print_library_error ("entry_point must not be null");
    resumeTimer ();
    return -1;
  }
  if (ready_queue.size () >= MAX_THREAD_NUM)
  {
    print_library_error ("thread limit reached");
    resumeTimer ();
    return -1;
  }
  // Create new thread
  int next_index = indexes.top ();
  indexes.pop ();
  Thread *new_thread = new Thread (next_index, entry_point);
  threads[next_index] = new_thread;
  if (threads[next_index] != nullptr && threads[next_index]->get_state () ==
  READY)
  {
    ready_queue.push_back (new_thread);
  }
  resumeTimer ();
  return next_index;
}

int uthread_terminate (int tid)
{
  blockTimer ();
  if (tid == 0 && running_thread->get_tid() == 0) // Terminating main
  {
    ready_queue.clear ();
    sleeping_threads.clear ();
    for (auto thread = threads.begin(); thread != threads.end(); )
    {
      if(thread->second->get_tid() != 0)
      {
        delete thread->second;
        thread->second = nullptr;
        threads.erase (thread);
      }
    }
//    delete running_thread;
//    running_thread = nullptr;
//    resumeTimer();
    exit (0);
  }
  else
  {
    if(tid == 0)
    {
      print_library_error ("cannot terminate main thread");
      resumeTimer ();
      return -1;
    }
    else if (threads.find (tid) == threads.end ())
    {
      print_library_error ("in terminate: thread does not exist, tid: " + tid);
      resumeTimer ();
      return -1;
    }
    else
    {
//      Handle termination
      Thread *thread_to_terminate = threads[tid];
      if (sleeping_threads.find (tid) != sleeping_threads.end ())
      {
        sleeping_threads.erase (tid);
      }
      else if (thread_to_terminate->get_state () == READY &&
               std::find (ready_queue.begin (), ready_queue.end (), thread_to_terminate)
               != ready_queue.end ())
      {
        ready_queue.remove (thread_to_terminate);
      }
      else if (running_thread != nullptr && tid == running_thread->get_tid ())
      {
        indexes.push (tid);
        threads.erase (tid);
        delete thread_to_terminate;
        resumeTimer ();
        timer_handler (0, true);
        return 0;
      }
      threads.erase (tid);
      indexes.push (tid);
      delete thread_to_terminate;
      resumeTimer ();
      return 0;
    }
  }
}

int uthread_block (int tid)
{
  blockTimer ();
  if (threads.find (tid) == threads.end ())
  {
    print_library_error ("in block: thread does not exist, tid: " + tid);
    resumeTimer ();

    return -1;
  }
  else if (tid == 0)
  {
    print_library_error ("cannot block main thread");
    resumeTimer ();

    return -1;
  }
  // Change thread state
  if (threads[tid] != nullptr && threads[tid]->get_state () == RUNNING)
  {
    sigsetjmp (*(threads[tid]->get_env ()), 1);
    timer_handler (0, true);
  }
  if (std::find (ready_queue.begin (), ready_queue.end (), threads[tid]) !=
      ready_queue.end ())
  {
    ready_queue.remove (threads[tid]);
  }
  threads[tid]->set_state (BLOCKED);
  resumeTimer ();
  return 0;
}

int uthread_resume (int tid)
{
  blockTimer ();
  if (threads.find (tid) == threads.end ())
  {
    print_library_error ("in resume: thread does not exist, tid: " + tid);
    resumeTimer ();
    return -1;
  }
  else if (threads[tid]->get_state () != BLOCKED)
  {
    resumeTimer ();
    return 0;
  }

//  Change state to READY, add to queue
  threads[tid]->set_state (READY);
//  Add to ready only if it's not sleeping
  if (sleeping_threads.find (tid) == sleeping_threads.end ())
  {
    ready_queue.push_back (threads[tid]);
  }
  resumeTimer ();
  return 0;
}

int uthread_sleep (int num_quantums)
{
  blockTimer ();
  if (num_quantums <= 0)
  {
    print_library_error ("num_quantums must be positive");
    resumeTimer ();
    return -1;
  }
  if (uthread_get_tid () == 0)
  {
    print_library_error ("cannot put main thread to sleep");
    resumeTimer ();
    return -1;
  }
  sleeping_threads[uthread_get_tid ()] = num_quantums;
  if (threads[uthread_get_tid ()] != nullptr &&
      threads[uthread_get_tid ()]->get_state () == RUNNING)
  {
    timer_handler (0, true);
  }
  else if (threads[uthread_get_tid ()] != nullptr &&
           threads[uthread_get_tid ()]->get_state () == READY)
  {
    ready_queue.remove (threads[uthread_get_tid ()]);
  }
  resumeTimer ();

  return 0;
}

int uthread_get_tid ()
{
  blockTimer ();
  int id = running_thread->get_tid ();
  resumeTimer ();
  return id;
}

int uthread_get_total_quantums ()
{
  blockTimer ();
  int count = quantum_counter;
  resumeTimer ();
  return count;
}

int uthread_get_quantums (int tid)
{
  blockTimer ();
  if (threads.find (tid) == threads.end ())
  {
    print_library_error ("in get quantums: thread does not exist, tid " + tid);
    resumeTimer ();
    return -1;
  }
// Return thread quantum counter
  int counter = threads[tid]->get_quantum_counter ();
  resumeTimer ();
  return counter;
}





