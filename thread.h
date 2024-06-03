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
  Thread::change_state(State state);



//quantum counters

 private:
  int tid;
  State state;
  char *stack;
  void (*entry_point) (void);
  int bound;
  int quantum_counter;


};

#endif //_THREAD_H_
