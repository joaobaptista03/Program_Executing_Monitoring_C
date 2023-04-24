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

    if (mkfifo("../tmp/fifo.txt", 0600) < 0) {
        write(2, "Error creating fifo.", 21);
        exit(EXIT_FAILURE);
    }

    int fifo = open("../tmp/fifo.txt", O_RDONLY);
        if (fifo < 0) {
            write(2, "Error opening fifo.", 20);
            exit(EXIT_FAILURE);
        }

    int leitura;
    char buf[BUFFSIZE];
    while(1) {
        while ((leitura = read(fifo, buf, BUFFSIZE)) > 0) {
            
        }
    }
}