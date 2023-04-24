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

    process executing[100]; int nr_executing = 0;
    process executed[100]; int nr_executed = 0;

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

            if (strcmp(status,"executing") == 0) executing[nr_executing++] = this_process;
            if (strcmp(status,"executed") == 0) {
                executed[nr_executed++] = this_process;

                int index = -1;
                for(int i = 0; i < nr_executing; i++) {
                    if (executing[i].pid == this_process.pid) index = i;
                }

                for(int i = index; i < nr_executing; i++) {
                    executing[i] = executing[i+1];
                }

                nr_executing--;
            }
        }
    }
}