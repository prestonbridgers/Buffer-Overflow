#ifndef DEMO_H_INCLUDED
#define DEMO_H_INCLUDED

#define MAX_EVENTS 1024
#define LEN_NAME 16
#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (MAX_EVENTS * (EVENT_SIZE + LEN_NAME))

#define READ_BUF_SIZE 256


typedef struct
{
    WINDOW *win;
    FILE *file;
    int isRunning;
} winFile_t;

// Defined in reader.c
void *inotify_start(void *arg);

#endif
