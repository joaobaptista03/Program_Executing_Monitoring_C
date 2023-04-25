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

    typedef struct executing_process {
        int pid;
        char *prog_name;
        long int secs;
        long int milis;
    } executing_process;

    typedef struct executed_process {
        int pid;
        char *prog_name;
        float exec_time;
    } executed_process;

    executing_process *executing = malloc(sizeof(executing_process) * 10); 
    if (executing == NULL) {
        printf("Failed to allocate memory for executing array.");
        exit(EXIT_FAILURE);
    }
    int nr_executing = 0, capacity_executing = 10;

    executed_process *executed = malloc(sizeof(executed_process) * 10); 
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

            executing_process this_process = {
                atoi(strtok(NULL, ";")),
                strtok(NULL, ";"),
                atol(strtok(NULL, ";")),
                atol(strtok(NULL, ";"))
            };

            if (strcmp(status,"executing") == 0) {
                if (nr_executing < capacity_executing) executing[nr_executing++] = this_process;
                else {
                    capacity_executing *= 2;
                    executing = realloc(executing, capacity_executing * sizeof(executing_process));
                    if (executing == NULL) {
                        printf("Failed to allocate memory for executing array.");
                        exit(EXIT_FAILURE);
                    }
                    executing[nr_executing++] = this_process;
                }
            }

            if (strcmp(status,"executed") == 0) {
                
                float exec_time = 0;
                for(int i = 0; i < nr_executing; i++) {
                    if (executing[i].pid == this_process.pid) {
                        if (this_process.secs == executing[i].secs) exec_time = (float) (this_process.milis - executing[i].milis) / (float) 1000;
                        else {
                            exec_time = ((float) this_process.milis + (float) (1000000 - executing[i].milis)) / (float) 1000;
                        }
                    }
                }

                this_process.secs = 0;
                this_process.milis = exec_time;

                executed_process new_executed = {
                    this_process.pid,
                    this_process.prog_name,
                    exec_time
                };

                if (nr_executed < capacity_executed) executed[nr_executed++] = new_executed;
                else {
                    capacity_executed *= 2;
                    executed = realloc(executed, capacity_executed * sizeof(executed_process));
                    if (executed == NULL) {
                        printf("Failed to allocate memory for executed array.");
                        exit(EXIT_FAILURE);
                    }
                    executed[nr_executed++] = new_executed;
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

            for(int i = 0; i < nr_executed; i++) printf("executed in %f milliseconds\n", executed[i].exec_time);
        }
    }

    free(executed);
    free(executing);
}