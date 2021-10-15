# Buffer-Overflow
Buffer overflow research and ideas/implementations.

# Planned Features/Requirements

The following are planned features and requirements that I want the project
to have and conform to:

- **Pretty nCurses layout**: Half of the screen on the left is reserved for 
program stdout output. The other half is reserved for displaying stack info
such as return address and visual recognition of buffer overflow.
- **Output preservation**: Redirect program's output to a file. Monitor that
file for changes and print that output to a nCurses window.
- **Multithreaded**: The library will need to be multithreaded as it will need
to both constantly watch for output by the decorated program and do its own
work visualizing the stack.
- **Ease of use**: The library must be easy to use. Decorating an existing
program must be a simple, streamlined process. Macros will likely be used
liberally.

# Directory Structure

Code in this repository is organized by category with the use of
subdirectories. Below is a list of each sub directory and a general idea
of what types of code they hold.

## inline\_asm

This folder contains working assembly code written inline in a C file.

## macros

This folder contains working code involving the use of C preprocessor macros
instead of normal C functions.

## nCurses

This folder contains working nCurses programs as well as some notes I've jotted
down on nCurses and its Panel library.

### nCurses/demo

This folder contains a demo program. It's similar to the layout program in /nCurses,
however the demo includes the use of inotify to monitor changes to a file and
write those changes to the program output window in the nCurses TUI.

## output\_redirect

This folder contains program examples of redirecting stdout output to a new
file descriptor.

## inotify

This folder contains working examples of using the inotify subsystem to monitor
a file/directory for changes. This subsystem will be used to monitor the
decorated program for stdout output that has been redirected to a file.
