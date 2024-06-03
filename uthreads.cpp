#include <queue>
#include <iostream>
#include <string>
#include "thread.h"
#include "uthreads.h"

using namespace std;

int quantum_length;
int quantum_counter;
queue<Thread *> ready_queue;
std::map<int, Thread*> threads;
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
    print_library_error("quantum_usecs must be positive");
    return -1;
  }
  quantum_counter=0;
  quantum_length = quantum_usecs;
  Thread * main = new Thread (0);
  threads[0] = main;
//  Check if main needs to be running when constructed or alter
// Add signals somehow
// Add timer ????
quantum_counter=1;
  return 0;
}

int uthread_spawn (thread_entry_point entry_point)
{
  if (!entry_point)
  {
    print_library_error("entry_point must not be null");
    return -1;
  }
  if (ready_queue.size () >= MAX_THREAD_NUM)
  {
    print_library_error("thread limit reached");
    return -1;
  }
  return 0;
}


