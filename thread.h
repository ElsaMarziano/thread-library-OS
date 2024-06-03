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
//  Thread::terminate();

//  terminate
//state change
//return id??
//get entry point
//quantum counters

 private:
  int tid;
  State state;
  char *stack;
  void (*entry_point) (void);
  int bound;


};

#endif //_THREAD_H_
