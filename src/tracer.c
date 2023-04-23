#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "../headers/tracer_execute_u.h"
#include "../headers/tracer_execute_p.h"

#define BUFSIZE 1024

int main(int argc, char **argv) {
    /* INITIALIZING CLOCK */
    clock_t start = clock();

    if (strcmp(argv[1], "execute") == 0) {
        if (strcmp(argv[2], "-u") == 0) execute_u(argv[3]);
        else if (strcmp(argv[2], "-p") == 0) execute_p(argv[3]);
        else perror("Invalid command, options: -u / -p");
    }

    /* PROGRAM RUN TIME CALCULATION */
    clock_t end = clock();
    double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("PROGRAM FINISHED (%f seconds)\n\n", cpu_time_used );

    return 0;
}