#include "thread.h"
#include <iostream>

#ifdef __x86_64__
/* code for 64 bit Intel arch */

typedef unsigned long address_t;
#define JB_SP 6
#define JB_PC 7

/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */
address_t translate_address(address_t addr)
{
    address_t ret;
    asm volatile("xor    %%fs:0x30,%0\n"
        "rol    $0x11,%0\n"
                 : "=g" (ret)
                 : "0" (addr));
    return ret;
}

#else
/* code for 32 bit Intel arch */

typedef unsigned int address_t;
#define JB_SP 4
#define JB_PC 5


/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */
address_t translate_address(address_t addr)
{
    address_t ret;
    asm volatile("xor    %%gs:0x18,%0\n"
                 "rol    $0x9,%0\n"
            : "=g" (ret)
            : "0" (addr));
    return ret;
}


#endif

// Constructor for main
Thread::Thread(int tid)
        : tid(tid), state(READY), quantum_counter(0)
{
    _stack = new char[STACK_SIZE];
}

Thread::Thread(int tid, void (*entry_point)(void))
        : tid(tid),
          entry_point(entry_point), state(READY), bound(STACK_SIZE), quantum_counter(0)
{
    _stack = new char[STACK_SIZE];
    address_t sp, pc;
    sp = (address_t) stack + STACK_SIZE - sizeof(address_t);
    pc = (address_t) entry_point;
    if (sigsetjmp(_env, 1) == 0) {
        (_env->__jmpbuf)[JB_SP] = translate_address(sp);
        (_env->__jmpbuf)[JB_PC] = translate_address(pc);
        sigemptyset(&_env->__saved_mask);
    }
}

Thread::~Thread()
{
    delete[] stack;
}

int Thread::get_tid() {return tid;}

State Thread::get_state() {return state;}

thread_entry_point Thread::get_entry_point() {return entry_point;}

int Thread::get_bound() {return bound;}

char* Thread::get_stack() {return stack;}

int Thread::get_quantum_counter() {return quantum_counter;}

State Thread::set_state(State state)
{
    if (state == RUNNING && _state == READY)
    {
        quantum_counter++;
        siglongjmp(_env, 1);
    }
    else if (_state == RUNNING && state != RUNNING)
    {
        sigsetjmp(_env, 1);
    }
    _state = state;
}