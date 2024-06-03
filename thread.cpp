#include "thread.h"
#include "uthreads.h"
#include <iostream>

// Constructor for main
Thread::Thread (int tid)
    : tid (tid), state(RUNNING)
{
}
  Thread::Thread (int tid, void (*entry_point) (void), char *stack)
      : tid (tid),
        entry_point (entry_point), stack
            (stack), state(READY), bound(STACK_SIZE)
  {
  }