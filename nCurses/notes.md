# Initialization

\#initscr() <- initializes nCurses (see man curs\_initscr)

cbreak() <- Just call it

noecho() <- Doesn't echo input characters to the window (call it)

\#endwin() <- Must be called before program exit (see man curs\_initscr)

\#curs\_set() <- Sets the cursor visibility state (see man curs\_kernel)

Note: See man curs\_inopts for different initialization routine options and
what they do in detail.

Note: Functions with a # befor the name have different man pages.

# Datatypes

Look into the panel library (man panel).
I'll be using this.

Windows are of type WINDOW *

## Operations on Windows

Note: remove the w at the beginning of these routines to have them affect
stdscr (I won't be using stdscr, so all of the following include the 'w').

wgetch <- Reads character input from a given window (see man curs\_getch)

wmove <- Moves a window (see man curs\_mov)

waddch <- Adds a character to the window (see man curs\_addch)

waddstr <- Adds a string to the window (see man curs\_addstr)

wrefresh <- Called after manipulating windows (see man curs\_refresh)

newwin <- Creates a new window (see man curs\_window)

delwin <- Deletes a given window. Subwindows must be deleted before
the main window can be deleted. Frees all associated memory. (see
man curs\_window)

box/wborder <- Draws a border around a window (see man curs\_border)

wclear <- Clears the window (see man curs\_clear)

getyx <- Places current cursor position of given window in two integer vars.
(see man\_getyx)



# Environment Variables

LINES <- set to height in lines of the screen by initscr().

COLUMNS <- set to width in characters of the screen by initscr().

Some terminfo stuff, check man ncurses -> Environment variables.

# Routine and Arg Names

Routines prefixed with a 'w' require a window argument (WINDOW \*).
Those without generally will use stdscr.

Routines prefixed with mv require a y and x coordinate to move to before
performing the appropriate action. y is the row, and x is the column. The
upper left hand corner is always (0,0).

Routines prefixed with mvw take both a window argument and the x and y
coordinates. Window argument is always specified before the coordinates.







# Panel Library

See man panel.

A window is associated with every panel.

Compiled with -lpanel, include panel.h
