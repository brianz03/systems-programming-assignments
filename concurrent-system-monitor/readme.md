## System Monitoring Tool -- Concurrency & Signals

C program that will report different metrics of the utilization of a given linux system concurrently


### Overview of functions 

void printSpace(int num);\
-prints n number of newlines

void numCores();\
-gets the number of cores

void getProgramUsage();\
-get the memory usage of the program

int getNumUsers();\
-returns the number of users

void writeUsers( int pipe);\
-writes to a pipe the each user which is logged in

double writeMemUsage( bool showGraphics, double prev, int pipe);\
-writes to a pipe 1 sample of memory info

void printSysInfo();\
-prints system information

tuple writeProcessorUsage(double ptotal, double pusage, int i, int cpuPipe, int graphicsPipe);\
-writes to 1 pipe the cpu usage, and the other pipe writes a string with the graphics samples

int main(int argc, char *argv[])\
-gets all command line arguments and calls print function

void print(bool showSystem, bool showUser, bool showGraphics, bool sequential, int samples, int tdelay)\
-creates pipes to communicate with processes, creates a fork for each process used to get memory, cpu, user
and then displays the data

void childHandler(int signum)\
-handle signal from children

void parentHandler (int signum)\
-handle signal from parent


### How to run

step 1: compile program: make

step 2: run program: ./a3

### Arguments
--system: display only system info

--user: display only user info

--graphics: shows graphics for memory and cpu. 
memory prints ':' or '#' for percent increase in usage
if usage is 0 it prints: \
|o    zero+  \
|@    zero-

cpu usage will print a '|' for each percent increase. If usage is 0 it will print 1 '|'


--sequential: display data without refreshing the screen

--samples=n: number of samples to take

--tdelay=n: delay between samples in 'n' seconds

user can also specify samples and delay by entering 2 numbers.
Ex: ./a1 5 4
the first integer will always be samples and second the delay
