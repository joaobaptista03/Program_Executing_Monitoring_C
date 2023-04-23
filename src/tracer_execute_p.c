#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/wait.h>

#include "../headers/tracer_execute_u.h"
#include "../headers/random.h"

void execute_p(char *args) {
    char *token = strtok(args, "|");
    int count = 0;
    char *trimmed;
    while (token != NULL) {
        count++;
        trimmed = strtrim(token);
        token = strtok(NULL, "|");

        int pid_child = fork();
        if (pid_child == -1) perror("Error creating child process!");
        if (pid_child == 0) {
            char print[50]; sprintf(print, "----------------------------\nINPUT Nº %d\n", count);
            write(1, print, strlen(print));
            execute_u(trimmed);
        }
        else wait(NULL);
    }

    puts("");

    free(token);
}