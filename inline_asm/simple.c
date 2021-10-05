#include <stdio.h>
#include <stdlib.h>

int sum(int val_1, int val_2);

int main(int argc, char *argv[])
{
    int operand1 = 20;
    int operand2 = 20;
    printf("%d + %d = %d\n", operand1, operand2, sum(operand1, operand2));
    return EXIT_SUCCESS;	
}

asm(
    "sum:;"
    "   pushq %rbp;"                // Push frame ptr to stack
    "   movq %rsp, %rbp;"           // Move stack ptr to the frame ptr
    "   movl %esi, %eax;"           // Move val_1 to %eax
    "   addl %edi, %eax;"           // Add val_2 to %eax
    "   leave;"                     // Copy frame ptr to stack ptr and pop
                                    //    the old frame ptr
    "   ret;"                       // Pops ret value from stack and transfers
                                    //    control to that address
   );

