#ifndef _THREAD_H_
#define _THREAD_H_

#include "uthreads.h"
#include <stdio.h>
//#include <setjmp.h>


enum State
{
    READY, RUNNING, BLOCKED
};

class Thread
{
 public:

  Thread (int tid);
  Thread (int tid, void (*entry_point) (void));
  ~Thread();
  int get_tid();
  State get_state();
  thread_entry_point get_entry_point();
  int get_bound();
  char* get_stack();
  int get_quantum_counter();
  void set_state(State state);



//quantum counters

 private:
  int _tid;
  State _state;
  char *_stack;
  void (*_entry_point) (void);
  int _bound;
  int _quantum_counter;
  sigjmp_buf _env;



};

#endif //_THREAD_H_
