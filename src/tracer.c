#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/time.h>

#define BUFSIZE 1024

void execute_u(char *args, int fifo) {
    pid_t pid = getpid();
    char pid_write[20]; sprintf(pid_write, "PROCESS ID: %d\n", pid);
    write(1, pid_write, strlen(pid_write));

    char *prog_name = strtok(args, " ");
    struct timeval tv; gettimeofday(&tv, NULL);
    char send_fifo[BUFSIZ]; sprintf(send_fifo, "executing;%d;%s;%ld;%ld", pid, prog_name, tv.tv_sec, tv.tv_usec);
    write(fifo, send_fifo, BUFSIZE);

    char executing[BUFSIZE]; sprintf(executing, "EXECUTING \"%s\"\n", args);
    write(1, executing, strlen(executing));

    char *arg_str = strtok(NULL, "");
    char **args_list = NULL;
    int num_args = 0;
    char *arg = strtok(arg_str, " ");
    while (arg != NULL) {
        num_args++;
        args_list = realloc(args_list, sizeof(char*) * num_args);
        args_list[num_args - 1] = strdup(arg);
        arg = strtok(NULL, " ");
    }

    int fork_child = fork();
    if (fork_child == -1) write(STDERR_FILENO, "Error creating child process.", 30);
    else if (fork_child == 0) execvp(prog_name, args_list);
    else {
        wait(NULL);

        gettimeofday(&tv, NULL);
        char send_fifo[BUFSIZ]; sprintf(send_fifo, "executed;%d;%s;%ld;%ld", pid, prog_name, tv.tv_sec, tv.tv_usec);
        write(fifo, send_fifo, BUFSIZE);

        for (int i = 0; i < num_args; i++) {
            free(args_list[i]);
        }
        free(args_list);
        free(arg_str);
        free(arg);
    }
}

char *strtrim(char *str) {
    char *end;
    while (isspace(*str)) { // skip leading whitespace
        str++;
    }
    if (*str == '\0') { // empty string
        return str;
    }
    end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) { // skip trailing whitespace
        end--;
    }
    *(end + 1) = '\0'; // terminate the trimmed string
    return str;
}

void execute_p(char *args, int fifo) {
    char *token = strtok(args, "|");
    int count = 0;
    char *trimmed;
    while (token != NULL) {
        count++;
        trimmed = strtrim(token);
        token = strtok(NULL, "|");

        int pid_child = fork();
        if (pid_child == -1) {
            write(STDERR_FILENO, "Error creating child process.\n", 30);
            exit(EXIT_FAILURE);
        }
        if (pid_child == 0) {
            char print[50]; sprintf(print, "----------------------------\nINPUT Nº %d\n", count);
            write(1, print, strlen(print));
            execute_u(trimmed, fifo);
        }
        else wait(NULL);
    }

    puts("");

    free(token);
}

void status() {

}

int main(int argc, char **argv) {

    /* INITIALIZING CLOCK */
    clock_t start = clock();

    if ((argc != 4 && argc != 2)) {
        write(STDERR_FILENO, "Invalid number of arguments.\n", 30);
        exit(EXIT_FAILURE);
    }

    else if (argc == 4) {
        int fifo = open("tmp/fifo", O_WRONLY);
            if (fifo < 0) {
                write(2, "Error opening fifo.", 20);
                exit(EXIT_FAILURE);
            }

        if (strcmp(argv[1], "execute") == 0) {
            if (strcmp(argv[2], "-u") == 0) execute_u(argv[3], fifo);
            else if (strcmp(argv[2], "-p") == 0) execute_p(argv[3], fifo);
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

    /* PROGRAM RUN TIME CALCULATION */
    clock_t end = clock();
    double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("PROGRAM FINISHED (%f seconds)\n\n", cpu_time_used );

    return 0;
}