This is a *C* program that acts as a monitor for executing and executed processes. It creates two named pipes using the mkfifo function, one for reading and one for writing. Then, it opens the pipes for reading and writing, respectively, using the open function. The program reads from the read pipe in a while loop until it is closed, and parses each message received to perform an action accordingly.

The program defines two structs, executing_process and executed_process, which hold information about executing and executed processes, respectively. It also creates two arrays of these structs, executing and executed, and uses realloc to resize them dynamically as needed.

The program supports three commands:

- executing;PID;prog_name;sec;usec: Adds information about a new executing process to the executing array.
- executed;PID;prog_name;sec;usec: Adds information about a new executed process to the executed array, and calculates its execution time based on the information in the executing array, removing it from the executing array.
- status: Prints the status of all executing processes, along with their execution times.
- stats_time;PID1;PID2;...: Calculates the total execution time for each PID listed in the message.

If an error occurs during the creation or opening of the named pipes, or if memory allocation fails, the program writes an error message to the standard error stream and exits with a failure status code.

Note that the program assumes that the tmp directory exists in the current directory, and that the user has the necessary permissions to create files in it.
