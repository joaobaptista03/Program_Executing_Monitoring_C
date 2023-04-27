## Monitor

This is a simple C program that acts as a monitor for a set of processes. 

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
