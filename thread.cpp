#include "thread.h"
#include "uthreads.h"
#include <iostream>

// Constructor for main
Thread::Thread(int tid)
        : tid(tid), state(RUNNING), quantum_counter(0)
{
}

Thread::Thread(int tid, void (*entry_point)(void), char *stack)
        : tid(tid),
          entry_point(entry_point), stack
                  (stack), state(READY), bound(STACK_SIZE), quantum_counter(0)
{

}

Thread::~Thread()
{
    delete[] stack;
}

Thread::get_tid()
{
    return tid;
}

Thread::get_state()
{
    return state;
}

Thread::get_entry_point()
{
    return entry_point;
}

Thread::get_bound()
{
    return bound;
}

Thread::get_stack()
{
    return stack;
}

Thread::change_state(State
state)
{
    if (state == RUNNING)
    {
        quantum_counter++;
    }
    state = state;
}