#ifndef _THREAD_H_
#define _THREAD_H_

enum State
{
    READY, RUNNING, BLOCKED
};

class Thread
{
 public:

  Thread (int tid);
  Thread (int tid, void (*entry_point) (void), char *stack);
  ~Thread();
  Thread::get_tid();
  Thread::get_state();
  Thread::get_entry_point();
  Thread::get_bound();
  Thread::get_stack();
  Thread::get_quantum_counter();
  Thread::set_state(State state);



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
