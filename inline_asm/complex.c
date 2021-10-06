#include <stdio.h>
#include <stdlib.h>

int sum(int val_1, int val_2);

int main(int argc, char *argv[])
{
    int operand1 = 20;
    int operand2 = 20;
    int output;

    asm volatile (
        "movl %1, %0;"           // Move operand1 to output
        "addl %2, %0;"           // Add operand2 to output
        : "=rm" (output)
        : "irm" (operand1), "irm" (operand2)
    );

    printf("%d + %d = %d\n", operand1, operand2, output);

    return EXIT_SUCCESS;
}


