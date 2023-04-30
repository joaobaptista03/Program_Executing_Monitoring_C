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
#include <stdbool.h>

#define BUFSIZE 1024

void execute_u(char *args, int fifo) {
    pid_t pid = 0;
    int pd[2]; pipe(pd);

    char *prog_name = strtok(args, " ");
    char *arg_str = strtok(NULL, "");
    char **args_list = malloc(sizeof(char*)); args_list[0] = prog_name; 
    int num_args = 1;

    while (arg_str != NULL) {
        num_args++;
        args_list = realloc(args_list, sizeof(char*) * num_args);
        args_list[num_args - 1] = strdup(arg_str);
        arg_str = strtok(NULL, "");
    }

    args_list = realloc(args_list, sizeof(char*) * (num_args + 1));
    args_list[num_args] = NULL;

    int fork_child = fork();

    if (fork_child == -1) {
        write(2, "Error creating child process.\n", 31);
        exit(EXIT_FAILURE);
    }

    else if (fork_child == 0) {
        pid_t pid = getpid();
        close(pd[0]); write(pd[1], &pid, sizeof(pid_t));
        char pid_write[20]; sprintf(pid_write, "PROCESS ID: %d\n", pid);
        write(1, pid_write, strlen(pid_write));

        char executing[BUFSIZE]; sprintf(executing, "EXECUTING \"%s\"\n", args);
        write(1, executing, strlen(executing));

        struct timeval tv; gettimeofday(&tv, NULL);
        char send_fifo[BUFSIZ]; sprintf(send_fifo, "executing;%d;%s;%ld;%ld", getpid(), prog_name, tv.tv_sec, tv.tv_usec);
        write(fifo, send_fifo, BUFSIZE);

        if (execvp(prog_name, args_list) < 0) {
            write(1, "Invalid command.\n", 18);
            gettimeofday(&tv, NULL);
            char send_fifo2[BUFSIZ]; sprintf(send_fifo2, "executed;%d;%s;%ld;%ld", getpid(), prog_name, tv.tv_sec, tv.tv_usec);
            write(fifo, send_fifo2, BUFSIZE);
            _exit(EXIT_FAILURE);
        }
    }

    else {
        wait(NULL);
        close(pd[1]);
        read(pd[0], &pid, sizeof(pid_t));
    }

    struct timeval tv2;
    gettimeofday(&tv2, NULL);
    char send_fifo2[BUFSIZ]; sprintf(send_fifo2, "executed;%d;%s;%ld;%ld", pid, prog_name, tv2.tv_sec, tv2.tv_usec);
    write(fifo, send_fifo2, BUFSIZE);
    for (int i = 1; i < num_args; i++) {
        free(args_list[i]);
    }
    free(args_list);
}

char *strtrim(char *str) {
    char *end;
    while (isspace(*str)) {
        str++;
    }
    if (*str == '\0') {
        return str;
    }
    end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) {
        end--;
    }
    *(end + 1) = '\0';
    return str;
}

void execute_p(char *args, int fifo) {
    char *command;
    char **commands;
    int num_commands = 0;

    char pid_write[30]; sprintf(pid_write, "\nMAIN PROCESS ID: %d\n", getpid());
    write(1, pid_write, strlen(pid_write));

    for (command = strtok(args, "|"); command != NULL; command = strtok(NULL, "")) {
        num_commands++;
        commands = realloc(commands, sizeof(char*) * num_commands);
        commands[num_commands - 1] = strdup(command);
    }

    for (int i = 1; i <= num_commands; i++) {
        char print[50]; sprintf(print, "----------------------------\nINPUT Nº %d\n", i);
        write(1, print, strlen(print));
        char *trimmed = strtrim(commands[i-1]);
        execute_u(trimmed, fifo);
    }

    for (int i = 1; i < num_commands; i++) {
        free(commands[i]);
    }
    free(commands);
}

void status(int write_fifo, int read_fifo) {
    write(write_fifo, "status;", 7);

    char buf[BUFSIZE];
    while ((read(read_fifo, buf, BUFSIZE) > 0) && (strcmp(buf, "finished") != 0)) puts(buf);
}

void stats_time(int write_fifo, int read_fifo, char **argv, int args) {
    char newargs[BUFSIZE]; sprintf(newargs, "stats_time;");
    for(int i = 2; i < args; i++) {
        strcat(newargs, argv[i]);
        strcat(newargs, ";");
    }
    strcat(newargs, "0");
    write(write_fifo, newargs, sizeof(newargs));

    char buf[BUFSIZE];
    read(read_fifo, buf, BUFSIZE);
    printf("Total execution time: %s ms.\n", buf);
}

int main(int argc, char **argv) {
    struct timeval tv_start; gettimeofday(&tv_start, NULL);

    if ((argc < 2)) {
        write(STDERR_FILENO, "Invalid number of arguments.\n", 30);
        exit(EXIT_FAILURE);
    }

    int write_fifo = open("tmp/ttm", O_WRONLY);
        if (write_fifo < 0) {
            write(2, "Error opening writing fifo.\n", 29);
            exit(EXIT_FAILURE);
        }

    int read_fifo = open("tmp/mtt", O_RDONLY);
        if (read_fifo < 0) {
            write(2, "Error opening reading fifo.\n", 29);
            exit(EXIT_FAILURE);
        }

    if (strcmp(argv[1], "exit") == 0) write(write_fifo, "exit;", 6);

    else if (strcmp(argv[1], "execute") == 0) {
        if (strcmp(argv[2], "-u") == 0) execute_u(argv[3], write_fifo);
        else if (strcmp(argv[2], "-p") == 0) execute_p(argv[3], write_fifo);
        else {
            write(STDERR_FILENO, "Invalid execute argument.\n", 27);
            exit(EXIT_FAILURE);
        }
    }
    
    else if (strcmp(argv[1], "status") == 0) status(write_fifo, read_fifo);

    else if (strcmp(argv[1], "stats-time") == 0) {
        stats_time(write_fifo, read_fifo, argv, argc);
    }

    else write(STDERR_FILENO, "Invalid command.\n", 18);

    struct timeval tv_end; gettimeofday(&tv_end, NULL);
    float exectime = 0;
    if (tv_end.tv_sec == tv_start.tv_sec) exectime = (float) (tv_end.tv_usec - tv_start.tv_usec) / (float) (1000);
    else exectime = ((float) tv_end.tv_usec + (float) (1000000 - tv_start.tv_usec)) / (float) 1000;
    printf("\nPROGRAM FINISHED (%f milliseconds)\n-------------------------------------\n", exectime);

    return 0;
}