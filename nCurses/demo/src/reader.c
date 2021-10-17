#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <curses.h>

#include "../include/demo.h"

#define MAX_EVENTS 1024
#define LEN_NAME 16
#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (MAX_EVENTS * (EVENT_SIZE + LEN_NAME))

void *inotify_start(void *arg)
{
    int fd; // File descriptor for the inotify instance
    int wd; // File descript for the inotify watch list
    char *watch_path = "output.txt"; // The file I want to watch
    winFile_t *wf = (winFile_t*) arg; // Casting argument to the expected struct
    int cursY = 1; // Initial y pos of the nCurses cursor
    int cursX = 1; // Initial x pos of the nCurses cursor
    char c; // Stores characters read from file when modification has occured
    long last_read = 0; // The byte number of the last read character in output.txt

    // Initializing inotify in non-blocking mode
    fd = inotify_init1(IN_NONBLOCK);
    if (fd == -1)
    {
        fprintf(stderr, "Fail to init inotify\n");
        exit(1);
    }

    // Adding ./output.txt to the watchlist (monitoring modifications)
    wd = inotify_add_watch(fd, watch_path, IN_MODIFY);
    if (wd == -1)
    {
        fprintf(stderr, "Fail to add output.txt to inotify watch list\n");
        exit(1);
    }

    // Main inotify loop (kills when the isRunning flag in the passed struct
    // is set to 0)
    while (wf->isRunning)
    {
        int i = 0; // Keeps track of where, in the buffer below, the current
                   // inotify event we are handling is
        int length; // length (amount of) inotify events that happened
        char buf[BUF_LEN]; // Buffer for the inotify events

        // Reading from the inotify instance, normally blocks until new data
        // has arrived at the stream (an event has occured)
        length = read(fd, buf, BUF_LEN);

        // While there are more events in the buffer to process
        while(i < length)
        {
            fprintf(stderr, "A modification event happened on output.txt\n");
            // Get a handle to the current event
            struct inotify_event *event = (struct inotify_event *) &buf[i];

            fprintf(stderr, "Reading output.txt from byte %ld\n", last_read);

            // Seeking to the end of what we've read in the file so far
            fseek(wf->file, last_read, SEEK_SET);

            // Loop that reads the new lines of the file character by character
            c = fgetc(wf->file);
            while (c != EOF)
            {
                if (c == '\n')
                {
                    cursY++;
                    cursX = 1;
                }
                else
                {
                    // Print the read character to the nCurses window
                    mvwaddch(wf->win, cursY, cursX++, c);
                }
                // Read the next character
                c = fgetc(wf->file);
                if (c == EOF)
                    fprintf(stderr, "Reached the EOF character\n\n");
            }

            // Rebox the window
            box(wf->win, '|', '-');

            // Refresh the screen
            wnoutrefresh(stdscr);
            wnoutrefresh(wf->win);
            doupdate();

            // Increment i to the index of the next event in the buffer
            i += EVENT_SIZE + event->len;
            // Set the last read byte to the current cursor position after
            // reading from the file
            last_read = ftell(wf->file);
        }
        // Sleep for half a second to save cpu.
        // Effectively limits inotify "updates" to twice every second.
        usleep(500000);
    }

    fprintf(stderr, "inotify: isRunning is false, cleaning up...\n");

    // Removing wd from the fd's watchlist
    inotify_rm_watch(fd, wd);
    // Closing the inotify instance
    close(fd);
    return NULL;
}
