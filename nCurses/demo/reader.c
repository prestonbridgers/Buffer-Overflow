#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <curses.h>

#include "demo.h"

void *inotify_start(void *arg)
{
    int fd;
    int wd;
    char *watch_path = "log.txt";
    winFile_t *wf = (winFile_t*) arg;
    int cursY = 1;
    int cursX = 1;
    char c;
    long last_read;
    int lastLen = 0;

    // Initializing inotify
    fd = inotify_init1(IN_NONBLOCK);
    if (fd == -1)
    {
        fprintf(stderr, "Fail to init inotify\n");
        exit(1);
    }

    // Adding ./log.txt to the watchlist (monitoring modifications)
    wd = inotify_add_watch(fd, watch_path, IN_MODIFY);
    if (wd == -1)
    {
        fprintf(stderr, "Fail to add log.txt to inotify watch list\n");
        exit(1);
    }

    while (wf->isRunning)
    {
        int i = 0;
        int length;
        char buf[BUF_LEN];

        length = read(fd, buf, BUF_LEN);

        while(i < length)
        {
            fprintf(stderr, "A modification event happened on log.txt\n");
            struct inotify_event *event = (struct inotify_event *) &buf[i];

            // When the file is modified read log.txt
            fprintf(stderr, "Reading log.txt from byte %ld\n", last_read);
            fseek(wf->file, last_read, SEEK_SET);
            c = fgetc(wf->file);
            while (c != EOF)
            {
                // Increment cursY and set cursX to 1 on a \n character
                if (c == '\n')
                {
                    cursY++;
                    cursX = 1;
                }
                else
                {
                    // Print char to the window
                    mvwaddch(wf->win, cursY, cursX++, c);
                }
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

            // Increment and update variables
            i += EVENT_SIZE + event->len;
            last_read = ftell(wf->file);
        }
        usleep(500000);
    }

    fprintf(stderr, "inotify: isRunning is false, cleaning up...\n");

    // Removing wd from the fd's watchlist
    inotify_rm_watch(fd, wd);
    // Closing the inotify instance
    close(fd);
    return NULL;
}
