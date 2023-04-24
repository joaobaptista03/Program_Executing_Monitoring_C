#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "../headers/tracer_execute_u.h"
#include "../headers/tracer_execute_p.h"
#include "../headers/tracer_status.h"

#define BUFSIZE 1024

int main(int argc, char **argv) {

    /* INITIALIZING CLOCK */
    clock_t start = clock();

    if (argc == 4) {

        if (strcmp(argv[1], "execute") == 0) {

            if (strcmp(argv[2], "-u") == 0) execute_u(argv[3]);

            else if (strcmp(argv[2], "-p") == 0) execute_p(argv[3]);

            else {
                write(STDERR_FILENO, "Invalid execute argument.\n", 27);
                exit(EXIT_FAILURE);
            }
            
        }

        else {
            write(STDERR_FILENO, "Invalid command.\n", 18);
            exit(EXIT_FAILURE);
        }

    }

    else if (argc == 2) {
        status();
    }

    else {
        write(STDERR_FILENO, "Invalid number of arguments.\n", 30);
        exit(EXIT_FAILURE);
    }

    /* PROGRAM RUN TIME CALCULATION */
    clock_t end = clock();
    double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("PROGRAM FINISHED (%f seconds)\n\n", cpu_time_used );

    return 0;
}