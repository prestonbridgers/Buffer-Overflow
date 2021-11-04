#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include "demo.h"

MemInfo mem_info;

void
foo()
{
    GET_STACK_PTR(mem_info.rsp);

    const int buffer_size = 8;
    uint8_t byte_buffer[buffer_size];
    int i, j;

    mem_info.buffer_ptr = (uint64_t*) byte_buffer;

    for (i = 0; i < buffer_size; i++) {
        byte_buffer[i] = i;

        uint64_t *tmp = mem_info.rsp;
        j = 0;
        while (j != 40)
        {
            printf("%#018"PRIx64"", *tmp);
            if (tmp == mem_info.buffer_ptr)
                printf(" <- Buffer\n");
            else if (tmp == mem_info.return_ptr)
                printf(" <- Ret. Addr.\n");
            else
                printf("\n");
            tmp--;
            j++;
        }
        printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    }
}


int
main(int argc, char *argv[])
{
    BEFORE_UNSAFE_CALL(mem_info.return_ptr);
	foo();
	return EXIT_SUCCESS;
}

