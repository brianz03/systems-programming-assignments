## System Monitoring Tool

C program that will report different metrics of the utilization of a given system. Designed to work in a Linux type OS.

### Overview of functions

int main(int argc, char** argv)\
-reads the command line arguments and calls the function print


void print(bool showSystem, bool showUser, bool showGraphics, bool sequential, int samples, int tdelay)\
-prints the information. Depending on values passed to print, it can show only system, user info or both
and user can specify the number of samples and the delay between each sample.\
-if the user enters invalid values for samples or delay, the program should use the default values\
-if --system and --user are indicated, the last command line argument is used

void cpuUsage()\
-prints the cpu usage in a percentage\
-reads the file /proc/stat and sums the values of the top row and calculates the usage by dividing 4th column 
by the sum of all values. The percentage is the time spent in the states that are not idle

void printCores()\ 
-prints the number of cores 

void printUsers()\
-prints users and information about each user from the utmp file

void printSysInfo()\ 
-prints system info 

void printMemUsage(char *str)\ 
-prints 1 sample for memory usage

void printProgramUsage()\
-prints all of the samples to make the screen look like it is refreshing

void printSpace(int num)\
-prints 'num' number of new lines

### How to run 
compile program: gcc a1.c -o a1\
step 2: run program: ./a1

### Command line arguments
--system: display only system info

--user: display only user info

--sequential: display data without refreshing the screen

--samples=n: number of samples to take

--tdelay=n: delay between samples in 'n' seconds

user can also specify samples and delay by entering 2 numbers.
Ex: ./a1 5 4
the first integer will always be samples and second the delay