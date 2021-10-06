#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

int foo() {
    int operand1 = 30;
    int operand2 = 25;

    // -----------------------------------------
    // I believe this section prints the return address of the
    // current function, but I don't know how to double check.
    // It prints the right address in GDB, but I can't tell if it
    // works outside of it.
    uint64_t addr = 0;
    asm volatile (
        "mov $1, %%rax;"
        "mov (%%rbp, %%rax, 8), %0;"
        : "=rm" (addr)
    );
    printf("Ret Addr: 0x%" PRIx64 "\n", addr);
    // -----------------------------------------

    return operand1 + operand2;
}

int main(int argc, char *argv[])
{
    printf("30 + 25 = %d\n", foo());
	return EXIT_SUCCESS;	
}

