#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

// This is the same return address printing function
// as is found is ../inline_asm/ret_addr.c but in macro
// form.

#define INIT_ADDR(label) \
    uint64_t *a##label; \
    uint64_t *b##label; \
    asm volatile ( \
        "mov $1, %%r8;" \
        "lea (%%rbp, %%r8, 8), %0;" \
        : "=rm" (a##label) );

#define FINAL_ADDR(label)  \
    asm volatile ( \
        "mov $1, %%r8;" \
        "lea (%%rbp, %%r8, 8), %0;" \
        : "=rm" (b##label) );


// Both pointers passed to this function are aliased.
// This causes initial != final to always return false.
void checkOF(uint64_t initial, uint64_t final)
{
    if (initial != final)
        printf("Buffer overflow detected\n");
    else
        printf("Buffer overflow not detected\n");
    return;
}

void target()
{
    INIT_ADDR(addr);

    printf("Target function was called\n");

    // Restore the return address to main
    *aaddr = 0x55555555526d;

    FINAL_ADDR(addr);
    checkOF(*aaddr, *baddr);
    return;
}

int foo() {
    INIT_ADDR(addr);

    int operand1 = 30;
    int operand2 = 25;

    // Artificially modifying the return address
    *aaddr = 0x555555555186;

    FINAL_ADDR(addr);
    checkOF(*aaddr, *baddr);
    return operand1 + operand2;
}

int main(int argc, char *argv[])
{
    printf("30 + 25 = %d\n", foo());
    printf("foo() returned to main()\n");
    return EXIT_SUCCESS;	
}

