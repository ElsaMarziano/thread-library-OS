#include <iostream>
#include <string>
#include "thread.h"
#include "uthreads.h"

using namespace std;

int quantum_length;
int quantum_counter;
LinkedList<Thread *> ready_queue;
std::map<int, Thread *> threads;
Thread *running_thread;
// Add counter map for blocked threads

void system_error (string error)
{
  std::cerr << "system error: " << error << std::endl;
  exit (1);
}

void print_library_error (string error)
{
  std::cerr << "thread library error: " << error << std::endl;
}

int uthread_init (int quantum_usecs)
{
  if (quantum_usecs <= 0)
  {
    print_library_error ("quantum_usecs must be positive");
    return -1;
  }
  quantum_counter = 0;
  quantum_length = quantum_usecs;
  Thread *main = new Thread (0);
  threads[0] = main;
// Add signals somehow
// Add timer ????
  running_thread = main;
  main->set_state (RUNNING);
  quantum_counter = 1;
  return 0;
}

int uthread_spawn (thread_entry_point entry_point)
{
  if (!entry_point)
  {
    print_library_error ("entry_point must not be null");
    return -1;
  }
  if (ready_queue.size () >= MAX_THREAD_NUM)
  {
    print_library_error ("thread limit reached");
    return -1;
  }
  // Create new thread
  int next_index = 1;
  Thread *new_thread = new Thread (next_index, entry_point);
  threads[next_index] = new_thread;
  if (threads.get_state () == READY) // Check this if is needed
  {
    ready_queue.push (new_thread);
  }

  return 0;
}

int uthread_terminate (int tid)
{
//  TODO check if need to free memory
  if (tid == 0)
  {
//     Check this is OK
    for (auto &thread: threads)
    {
      delete thread;
      threads.erase (thread.get_tid ());
    }
    exit (0);
  }
  else
  {
    if (threads.find (tid) == threads.end ())
    {
      print_library_error ("thread does not exist");
      return -1;
    }
    else
    {
//      Handle termination
// Add id to minheap
      delete threads[tid];
      threads.erase (tid);
      return 0;
    }
    return 0;
  }
}

int uthread_block (int tid)
{
  if (threads.find (tid) == threads.end ())
  {
    print_library_error ("thread does not exist");
    return -1;
  }
  else if (tid == 0)
  {
    print_library_error ("cannot block main thread");
    return -1;
  }
  // Change thread state
  if (threads[tid].get_state () == RUNNING)
  {
    // Handle thread blocking itself
  }
  threads[tid].set_state (BLOCKED);

  return 0;
}

int uthread_resume (int tid)
{
  if (threads.find (tid) == threads.end ())
  {
    print_library_error ("thread does not exist");
    return -1;
  }
  else if (threads[tid].get_state != BLOCKED)
  { return 0; }

//  Change state to READY
  threads[tid].set_state (READY);
// Add thread to queue
  ready_queue.push (threads[tid]);
  return 0;
}

int uthread_sleep (int num_quantums)
{
  if (num_quantums <= 0)
  {
    print_library_error ("num_quantums must be positive");
    return -1;
  }
  if (uthread_get_tid () == 0)
  {
    print_library_error ("cannot put main thread to sleep");
    return -1;
  }
//  Change state to BLOCKED
  uthread_block (running_thread->get_tid ());
//  Add to blocked queue
  return 0;
}

int uthread_get_tid ()
{

  return running_thread->get_tid ();
}

int uthread_get_total_quantums ()
{

  return quantum_counter;
}

int uthread_get_quantums (int tid)
{
  if (threads.find (tid) == threads.end ())
  {
    print_library_error ("thread does not exist");
    return -1;
  }
// Return thread quantum counter
  return threads[tid].get_quantum_counter ();
}



