#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

// This is the same return address printing function
// as is found is ../inline_asm/ret_addr.c but in macro
// form.
#define RET_ADDR() {  \
    asm volatile ( \
        "mov $1, %%r8;" \
        "lea (%%rbp, %%r8, 8), %0;" \
        : "=rm" (addr) );}

void target()
{
    uint64_t *addr;
    RET_ADDR();

    printf("Target function was called\n");

    // Restore the return address to main
    *addr = 0x555555555226;
    return;
}

int foo() {
    uint64_t *addr; // Tmp variable for the macro
    uint64_t ret_addr; // tmp variable to hold initial return address

    // Store return address in adr at the beginning of the function
    RET_ADDR();
    // Save initial return addres into ret_addr
    ret_addr = *addr;

    int operand1 = 30;
    int operand2 = 25;

    // Artificially modifying the return address
    *addr = 0x555555555149;

    // Setting addr to the new, modified return address
    RET_ADDR();
    if (ret_addr != *addr)
        printf("Buffer overflow detected\n");
    else
        printf("Buffer overflow not detected\n");

    return operand1 + operand2;
}

int main(int argc, char *argv[])
{
    printf("30 + 25 = %d\n", foo());
    return EXIT_SUCCESS;	
}

