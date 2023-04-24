#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#define BUFFSIZE 1024

int main(int argc, char **argv) {

    if (mkfifo("tmp/fifo", 0600) < 0) {
        write(2, "Error creating fifo.", 21);
        exit(EXIT_FAILURE);
    }

    int fifo = open("tmp/fifo", O_RDONLY);
        if (fifo < 0) {
            write(2, "Error opening fifo.", 20);
            exit(EXIT_FAILURE);
        }

    typedef struct process {
        int pid;
        char *prog_name;
        long int secs;
        long int milis;
    } process;

    process *executing = malloc(sizeof(process) * 10); 
    if (executing == NULL) {
        printf("Failed to allocate memory for executing array.");
        exit(EXIT_FAILURE);
    }
    int nr_executing = 0, capacity_executing = 10;

    process *executed = malloc(sizeof(process) * 10); 
    if (executed == NULL) {
        printf("Failed to allocate memory for executed array.");
        exit(EXIT_FAILURE);
    }
    int nr_executed = 0, capacity_executed = 10;


    int leitura;
    char buf[BUFFSIZE];
    while(1) {
        while ((leitura = read(fifo, buf, BUFFSIZE)) > 0) {
            char *status = strtok(buf,";");

            process this_process = {
                atoi(strtok(NULL, ";")),
                strtok(NULL, ";"),
                atol(strtok(NULL, ";")),
                atol(strtok(NULL, ";"))
            };

            if (strcmp(status,"executing") == 0) {
                if (nr_executing < capacity_executing) executing[nr_executing++] = this_process;
                else {
                    capacity_executing *= 2;
                    executing = realloc(executing, capacity_executing * sizeof(process));
                    if (executing == NULL) {
                        printf("Failed to allocate memory for executing array.");
                        exit(EXIT_FAILURE);
                    }
                    executing[nr_executing++] = this_process;
                }
            }

            if (strcmp(status,"executed") == 0) {
                if (nr_executed < capacity_executed) executed[nr_executed++] = this_process;
                else {
                    capacity_executed *= 2;
                    executed = realloc(executed, capacity_executed * sizeof(process));
                    if (executed == NULL) {
                        printf("Failed to allocate memory for executed array.");
                        exit(EXIT_FAILURE);
                    }
                    executed[nr_executed++] = this_process;
                }

                int index = -1;
                for(int i = 0; i < nr_executing; i++) {
                    if (executing[i].pid == this_process.pid) index = i;
                }

                for(int i = index; i < nr_executing; i++) {
                    executing[i] = executing[i+1];
                }

                nr_executing--;
            }

            for(int i = 0; i < nr_executing; i++) printf("executing %d\n", executing[i].pid);
            for(int i = 0; i < nr_executed; i++) printf("executed %d\n", executed[i].pid);
        }
    }

    free(executed);
    free(executing);
}