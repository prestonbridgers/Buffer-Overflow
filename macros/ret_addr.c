#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

// This is the same return address printing function
// as is found is ../inline_asm/ret_addr.c but in macro
// form.
#define ret_addr() { \
    uint64_t addr; \
    asm volatile ( \
        "mov $1, %%rax;" \
        "mov (%%rbp, %%rax, 8), %0;" \
        : "=rm" (addr) ); \
    printf("Ret Addr: 0x%" PRIx64 "\n", addr); \
}

int foo() {
    int operand1 = 30;
    int operand2 = 25;

    ret_addr();

    return operand1 + operand2;
}

int main(int argc, char *argv[])
{
    printf("30 + 25 = %d\n", foo());
    return EXIT_SUCCESS;	
}

