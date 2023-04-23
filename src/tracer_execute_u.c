#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define BUFFSIZE 1024

void execute_u(char *args) {
    pid_t pid = getpid();
    char pid_write[20]; sprintf(pid_write, "PROCESS ID: %d\n", pid);
    write(1, pid_write, strlen(pid_write));

    char executing[BUFFSIZE]; sprintf(executing, "EXECUTING \"%s\"\n", args);
    write(1, executing, strlen(executing));

    char *prog_name = strtok(args, " ");
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

    execvp(prog_name, args_list);

    for(int i = 0; i < num_args; i++) free(args_list[i]); free(args_list);
    free(prog_name);
    free(arg_str);
    free(arg);
}