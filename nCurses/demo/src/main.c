#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <pthread.h>

#include <curses.h>

#include "../include/demo.h"

int main(int argc, char *argv[])
{
    FILE *output;
    WINDOW *std;
    WINDOW *stack;
    char title[] = "Welcome to Stack Visualizer v0.1";

    // redirect stdout to a file called output.txt
    output = fopen("./output.txt", "w+");
    if (output == NULL)
    {
        fprintf(stderr, "Fail to open file\n");
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
    winFile_t *wf = (winFile_t*) malloc(sizeof(winFile_t));
    wf->win = std;
    wf->file = output;
    wf->isRunning = 1;

    pthread_create(&thread, NULL, inotify_start, (void*) wf);
    // Is this 0.1 second sleep necessary?
    // Could do an initial read before starting inotify to prevent the need
    // for a delay.
    usleep(100000); 

    // Example modification made to the output file that should be
    // reflected on the nCurses program output window
    int i;
    for (i = 0; i < 20; i++)
        fprintf(output, "Loop iteration: #%d\n", i + 1);
    fflush(output);

    sleep(1);
    for (i = 0; i < 5; i++)
        fprintf(output, "Loop iteration: #%d\n", i + 1);
    fflush(output);

    getch();
    wf->isRunning = 0;
    pthread_join(thread, NULL);

    free(wf);
    fclose(output);
    delwin(std);
    delwin(stack);
    endwin();
	return EXIT_SUCCESS;	
}

