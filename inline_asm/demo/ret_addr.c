#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <inttypes.h>

// GLOBALS
uint64_t *ret_ptr;
uint64_t *stack_ptr;
uint64_t *buf_ptr;

// MACROS
#define BEFORE_UNSAFE_CALL() \
    asm volatile ( \
        "mov $1, %%r8;" \
        "not %%r8;" \
        "inc %%r8;" \
        "lea (%%rsp, %%r8, 8), %%r9;" \
        "mov %%r9, %0;" \
        : "=rm" (ret_ptr) );

#define GET_STACK_PTR() \
    asm volatile ( \
        "mov %%rsp, %0;" \
        : "=rm" (stack_ptr) );

#define GET_BUF_PTR(buf) \
    buf_ptr = (uint64_t*)buf;

/* Prints a single 8 byte word to stderr:
 *
 * Ex:
 * 0x0007ffffffffe80c: 0x0f0e0d0c0b0a0908
 *
 * line_ptr - The address to print
 */
void print_line(uint64_t *line_ptr)
{
    fprintf(stderr, "%#018" PRIx64 ": %#018" PRIx64, (uint64_t)line_ptr, *line_ptr);
    if (line_ptr == buf_ptr)
    {
        fprintf(stderr, " <- Buffer");
    }
    else if (line_ptr == stack_ptr)
    {
        fprintf(stderr, " <- %%RSP");
    }
    else if (line_ptr == ret_ptr)
    {
        fprintf(stderr, " <- Ret. Addr.");
    }
    fprintf(stderr, "\n");

}

/* Prints 8 byte lines from the stack starting at address stack_ptr.
 *
 * nlines - The number of lines to print
 * done   - 1 or 0 that determines whether to print an escape sequence that
 *          brings the cursor back up to the start of the stack that has been
 *          printed. Using this prevents the prompt from being printed inside
 *          the stack after program completion.
 */
void print_stack(int nlines, short done)
{
    uint64_t *tmp = stack_ptr;
    for (int i = 0; i < nlines; i++)
    {
        print_line(tmp);
        tmp++;
    }

    if (!done)
        fprintf(stderr, "\033[%dF", nlines);
}

/* Allocates a 16 byte buffer on the stack, sets each byte printing the stack
 * after each byte is set. A delay is used so that you can visually see the
 * stack setting each byte of the buffer.
 */
uint8_t foo() {
    GET_STACK_PTR();
    uint8_t my_buffer[16];
    GET_BUF_PTR(my_buffer);

    for (int i = 0; i < 16; i++)
    {
        my_buffer[i] = i;
        print_stack(10, i == 15 ? 1 : 0);
        usleep(500000);
    }

    return my_buffer[0];
}

/* Entry point of the program. Uses BEFORE_UNSAFE_CALL macro to set global
 * ret_ptr to the address of the return address, the instruction after the call
 * to foo.
 */
int main(int argc, char *argv[])
{
    BEFORE_UNSAFE_CALL();
    foo();
    return EXIT_SUCCESS;	
}

