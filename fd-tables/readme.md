## Recreating the System-Wide FD Tables

Program which displays the tables used by the OS to keep track of open files, assignation of File Descriptors and processess.

The program, will generate:

* process FD table
* System-wide FD Table
* Vnodes FD table, and
* a composed view of the previous table

### Overview of functions

int main(int argc, char *argv[])\
-reads the command line arguments, sets the proper flags and calls the print function. After
showing all the info for the pids, it shows the flagged pids.

void getPID(bool PPflag, bool systemWideFlag, bool VnodesFlag, bool compositeFlag, int specifiedPID, int *offendingCount, int threshold, int **pidArr, int **fdArr)\
-gets all the pids in the proc folder and for each pid, calls the function to print the info for the pid

void printInfo(int pid, bool PPflag, bool systemWideFlag, bool VnodesFlag, bool compositeFlag, int *offendingCount, int threshold, int **pidArr ,int **fdArr)\
-prints the info for pid, fd, inode and filename for the pid specified. 
-writes to txt file or binary if specified
-calculates the offending processes

### How to run

step 1: type make

step 2: run program: ./a2 (arg 1) (arg 2) (arg n)

### Arguments

--per-process: show only process FD table

--systemWide: show only system wide FD table

--Vnodes: show Vnodes table

--composite: show composite table

--threshold=N: where N denotes an integer, indicating that processes which have a number 
of FD assigned larger than X should be flagged in the output. For this it will list the 
PID and number of assigned FDs, e.g. PID (FD)
