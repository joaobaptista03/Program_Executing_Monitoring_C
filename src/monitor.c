#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <sys/time.h>

#define BUFFSIZE 1024

int main(int argc, char **argv) {
    if (mkfifo("tmp/ttm", 0600) < 0) {
        write(2, "Error creating fifo.\n", 22);
        exit(EXIT_FAILURE);
    }

    if (mkfifo("tmp/mtt", 0600) < 0) {
        write(2, "Error creating fifo.\n", 22);
        exit(EXIT_FAILURE);
    }

    int read_fifo = open("tmp/ttm", O_RDONLY);
    if (read_fifo < 0) {
        write(2, "Error opening reading fifo.\n", 29);
        exit(EXIT_FAILURE);
    }

    int write_fifo = open("tmp/mtt", O_WRONLY);
    if (write_fifo < 0) {
        write(2, "Error opening writing fifo.\n", 29);
        exit(EXIT_FAILURE);
    }

    typedef struct executing_process {
        int pid;
        char *prog_name;
        long int secs;
        long int usec;
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

    double total_ms = 0;
    int exit_flag = 0;
    char buf[BUFFSIZE];

    while(exit_flag == 0) {
        
        while (read(read_fifo, buf, BUFFSIZE) > 0) {
            char *end_ptr;
            char *command = strtok(buf,";");

            if (strcmp(command, "exit") == 0) {
                exit_flag = 1;
                break;
            }

            else if (strcmp(command,"status") == 0) {
                for (int i = 0; i < nr_executing; i++) {
                    float exec_time = 0;
                    struct timeval tv_now; gettimeofday(&tv_now, NULL);
                    if (tv_now.tv_sec == executing[i].secs) exec_time = (float) (tv_now.tv_usec - executing[i].usec) / (float) 1000;
                    else {
                        exec_time = ((float) tv_now.tv_usec + (float) (1000000 - executing[i].usec)) / (float) 1000;
                    }
                    char status[100]; sprintf(status, "PID: %d; Program: %s; Execution Time: %f\n", executing[i].pid, executing[i].prog_name, exec_time);
                    write(write_fifo, status, sizeof(status));
                }
                write(write_fifo, "finished", 9);
            }

            else if (strcmp(command,"stats_time") == 0) {
                int nr_pid;
                while ((nr_pid = strtol(strtok(NULL, ";"), &end_ptr, 10)) != 0) {
                    for(int i = 0; i < nr_executed; i++) {
                        if (executed[i].pid == nr_pid) {
                            total_ms += executed[i].exec_time;
                        }
                    }
                }
                char totms[20]; sprintf(totms, "%f", total_ms);
                total_ms = 0;
                write(write_fifo, totms, sizeof(totms));
            }

            else {
                char *pid = strtok(NULL, ";");
                char *prog_name = strtok(NULL, ";");
                char *secs = strtok(NULL, ";");
                char *usec = strtok(NULL, ";");

                executing_process this_process = {
                    atoi(pid),
                    prog_name,
                    atol(secs),
                    atol(usec)
                };

                if (strcmp(command,"executing") == 0) {
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

                else if (strcmp(command,"executed") == 0) {
                    float exec_time = 5;
                    int index = -1;
                    for(int i = 0; i < nr_executing; i++) {
                        if (executing[i].pid == this_process.pid) {
                            index = i;
                            if (this_process.secs == executing[i].secs) exec_time = (float) (this_process.usec - executing[i].usec) / (float) 1000;
                            else {
                                exec_time = ((float) this_process.usec + (float) (1000000 - executing[i].usec)) / (float) 1000;
                            }
                            break;
                        }
                    }

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

                    for(int i = index; i < nr_executing; i++) {
                        executing[i] = executing[i+1];
                    }

                    nr_executing--;
                }
            }
        }
    }

    free(executing);
    free(executed);

    return 0;
}