#ifndef DEMO_H_INCLUDED
#define DEMO_H_INCLUDED

typedef struct
{
    WINDOW *win;
    FILE *file;
    int isRunning;
} winFile_t;

// Defined in reader.c
void *inotify_start(void *arg);

#endif
