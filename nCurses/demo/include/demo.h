#ifndef DEMO_H_INCLUDED
#define DEMO_H_INCLUDED

#include <curses.h>

#define BEFORE_UNSAFE_CALL(ptr) \
    asm volatile ( \
    "mov $1, %%r8;" \
    "not %%r8;" \
    "inc %%r8;" \
    "lea (%%rsp, %%r8, 8), %%r9;" \
    "mov %%r9, %0;" \
    : "=rm" (ptr) );

#define GET_STACK_PTR(ptr) \
    asm volatile ( \
    "mov %%rsp, %0;" \
    : "=rm" (ptr) );

typedef struct
{
    WINDOW *win;
    FILE *file;
    int isRunning;
} winFile_t;

typedef struct
{
    uint64_t *return_ptr;
    uint64_t *rsp;
    uint64_t *buffer_ptr;
} MemInfo;

// Defined in reader.c
void *inotify_start(void *arg);

#endif
