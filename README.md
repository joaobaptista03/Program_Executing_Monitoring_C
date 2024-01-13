## Program Executing Monitoring

## Description

This University project involves creating a C program that manages and monitors process executions in a Unix-like system. It features process creation, execution using fork-exec patterns, real-time monitoring through FIFOs (named pipes), and performance analysis by tracking execution times. The project aims to provide practical insights into operating systems' process management and inter-process communication.

### Prerequisites

This program requires a Unix-like operating system and a C compiler. It has been tested on Ubuntu 20.04.

### Building

To build the program, navigate to the main directory and run the following command:

```
make
```

### Usage

To use the monitor, run the `monitor` executable. Then you'll need to open another terminal and run the `tracer` executable to execute commands while monitor's running.

The following arguments are supported in the tracer:


* `execute -u "prog-a arg1 [...] argN"` - Executes a program.
* `execute -p "prog-a arg1 [...] argN | prog-b arg1 [...] argN | prog-c arg1 [...] argN"` - Executes a pipeline of programs.
* `status` - Get the status of all currently executing processes.
* `stats_time;pid1;pid2;...;pidN` - Get the total execution time of a set of previously executed processes
