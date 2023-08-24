#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/utsname.h>
#include <utmp.h>
#include <sys/sysinfo.h>
#include <sys/resource.h>
#include <unistd.h>

typedef struct 
{
    double total;
    double usage;
} tuple;

//function to print new lines
void printSpace(int num);

//function to get number of cores
void numCores();

//prints the memory usage of the program
void getProgramUsage();

//gets the number of users on the system
int getNumUsers();

//prints all the users on the system
void writeUsers( int pipe);

// returns information about memory usage in a string 
double writeMemUsage( bool showGraphics, double prev, int pipe);

//prints the system information 
void printSysInfo();

//calculates cpu usage and prints the percentage
tuple writeProcessorUsage(double ptotal, double pusage, int i, int cpuPipe, int graphicsPipe);


