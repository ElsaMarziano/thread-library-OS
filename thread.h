#ifndef THREAD_H
#define THREAD_H

#include "uthreads.h"
#include <setjmp.h>

enum State
{
    READY, RUNNING, BLOCKED
};

class Thread
{
 public:

  Thread (int tid);
  Thread (int tid, void (*entry_point) (void));
  ~Thread ();
  int get_tid ();
  State get_state ();
  thread_entry_point get_entry_point ();
  int get_bound ();
  char *get_stack ();
  int get_quantum_counter ();
  void set_state (State state);
  sigjmp_buf *get_env ();



//quantum counters

 private:
  int _tid;
  void (*_entry_point) (void);
  State _state;
  char *_stack;
  int _bound;
  int _quantum_counter;
  sigjmp_buf _env;

};

#endif //THREAD_H