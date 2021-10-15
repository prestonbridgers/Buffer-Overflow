#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>

#include <pthread.h>

#include <curses.h>

#include <sys/inotify.h>

#define MAX_EVENTS 1024
#define LEN_NAME 16
#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (MAX_EVENTS * (EVENT_SIZE + LEN_NAME))

typedef struct
{
    WINDOW *win;
    FILE *file;
} winFile_t;

void *inotify_start(void *arg)
{
    int fd;
    int wd;
    int i = 0;
    int length;
    char buffer[256];
    char *watch_path = "log.txt";
    winFile_t *wf = (winFile_t*) arg;
    int cursY = 1;

    fprintf(stderr, "In inotify_start\n");

    // Initializing inotify
    fd = inotify_init();
    if (fd == -1)
    {
        printf("Fail to init inotify\n");
        exit(1);
    }


    // Adding ./log.txt to the watchlist (monitoring modifications)
    wd = inotify_add_watch(fd, watch_path, IN_MODIFY);

    if (wd == -1)
        exit(3);

    while (1)
    {
        int i = 0;
        int length;
        char buf[BUF_LEN];
        long last_read;
        int lastLen = 0;

        length = read(fd, buf, BUF_LEN);

        while(i < length)
        {
            fprintf(stderr, "Things are happening\n");
            struct inotify_event *event = (struct inotify_event *) &buf[i];

            // When the file is modified
            // Read a line from log.txt
            fseek(wf->file, last_read, SEEK_SET);
            fprintf(stderr, "last_read: %ld\n", last_read);
            //fseek(wf->file, 0, SEEK_SET);
            fgets(buffer, 256, wf->file);
            lastLen = strlen(buffer);

            fprintf(stderr, "Read \"%s\" from buffer\n", buffer);

            // Print that line to the window
            mvwprintw(wf->win, cursY, 1, "%s", buffer);
            box(wf->win, '|', '-');
            //mvwprintw(wf->win, cursY, 1, "Something happened");
            // Move the cursor to start of the next line in the window
            cursY++;

            // Refresh the screen
            wnoutrefresh(stdscr);
            wnoutrefresh(wf->win);
            doupdate();

            i += EVENT_SIZE + event->len;
            last_read = ftell(wf->file);
        }
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    FILE *output;
    WINDOW *std;
    WINDOW *stack;
    char title[] = "Welcome to Stack Visualizer v0.1";

    // redirect stdout to a file called log.txt
    output = fopen("./log.txt", "w+");
    if (output == NULL)
    {
        printf("Fail to open file\n");
        exit(1);
    }

	initscr();
    curs_set(0);
    cbreak();
    noecho();
    start_color();


    // Init colors
    init_pair(1, COLOR_CYAN, COLOR_BLACK);

    // Init windows
    std   = newwin(LINES - 1, COLS / 2, 1, 0);
    stack = newwin(LINES - 1, COLS / 2 - 1, 1, (COLS / 2) + 1);

    // Bordering windows
    box(std, '|', '-');
    box(stack, '|', '-');

    // Printing to windows
    mvwprintw(std, 1, 1, "This is program stdout"); 
    mvwprintw(stack, 1, 1, "This is program stack dumps"); 

    // Printing title to stdscr
    mvprintw(0, (COLS/2) - (strlen(title)/2), "%s", title);
    // Setting attributes of title
    mvchgat(0, 0, -1, A_BOLD, 1, NULL);

    // Refreshing
    wnoutrefresh(stdscr);
    wnoutrefresh(std);
    wnoutrefresh(stack);
    doupdate();


    // Run inotify monitor loop writing to std window and monitoring
    // stdout for modifications.
    pthread_t thread;
    winFile_t *wf = (winFile_t*) malloc(sizeof(wf));
    wf->win = std;
    wf->file = output;

    pthread_create(&thread, NULL, inotify_start, (void*) wf);
    sleep(1);

    fprintf(output, "This is some output\n");
    fflush(output);
    sleep(1);
    fprintf(output, "This is some more output\n");
    fflush(output);

    getch();
    fclose(output);
    delwin(std);
    delwin(stack);
    endwin();
	return EXIT_SUCCESS;	
}

