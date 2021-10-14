#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	// Redirecting stdout to a file?
    freopen("log.txt", "w", stdout);

    // Writing to that file with printf?
    printf("Hello World\n");

    // Closing the file
    fclose(stdout);
	return EXIT_SUCCESS;
}

