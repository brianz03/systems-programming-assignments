#include "stats_functions.h"


//function to print new lines
void printSpace(int num){
    int i;
    for (i = 0; i < num; i++){
        printf("\n");
    }
}

void numCores(){
    int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    printf("Number of cores: %d\n", num_cores);
}

//prints the memory usage of the program
void getProgramUsage(){
   struct rusage r_usage;
   getrusage(RUSAGE_SELF, &r_usage);
   printf("Memory usage: %ld kilobytes\n",r_usage.ru_maxrss);
}


// returns information about memory usage in a string 
double writeMemUsage( bool showGraphics, double prev, int pipe) {
    struct sysinfo mem;
    double usage ,dif, bars;
    char str[1000];
    //calculates memory usage
    
    sysinfo(&mem);
    double ptotal = (double)mem.totalram / (1024 * 1024 * 1024);
    double pused = (double)(mem.totalram - mem.freeram) / (1024 * 1024 * 1024);
    double vtotal = (double)(mem.totalram + mem.totalswap) / (1024 * 1024 * 1024);
    double vused = (double)(mem.totalram + mem.totalswap - (mem.freeram + mem.freeswap)) / (1024 * 1024 * 1024);
    if (showGraphics == false){
    sprintf(str, "%.2f GB / %.2f GB  --  %.2f GB / %.2f GB", pused, ptotal, vused, vtotal);
    write(pipe, str, strlen(str) +1);
  
    }
    else{
        
        usage = pused;
        dif = pused - prev;
        bars = dif * 100;

         if (prev == -1000.0){
            dif =0;
            bars = 0;
        }
        
        char *h = malloc((int)bars + 1);
        memset(h, '#', (int)bars);
        h[(int)bars] = '\0';

        if (dif == 0.0){
            sprintf(str, "%.2f GB / %.2f GB  --  %.2f GB / %.2f GB    |o %.2f (%.2f)", pused, ptotal, vused, vtotal, dif, pused);
            write(pipe, str, strlen(str)+1);
        }else if (bars < 0.0){
            memset(h, ':', (int)bars);
            sprintf(str, "%.2f GB / %.2f GB  --  %.2f GB / %.2f GB    |%s@ %.2f (%.2f)", pused, ptotal, vused, vtotal, h, dif, pused);
            write(pipe, str, strlen(str)+1);
            
        }else{
            memset(h, '#', (int)bars);
            sprintf(str, "%.2f GB / %.2f GB  --  %.2f GB / %.2f GB    |%s* %.2f (%.2f)", pused, ptotal, vused, vtotal, h, dif, pused);
            write(pipe, str, strlen(str)+1);
        }
        
        return usage;

    }

return 0.0;  
}
//prints the system information 
void printSysInfo() {

	struct utsname buf;
    uname(&buf);
	printf("### System Information ###\n");
	printf("System Name = %s\n", buf.sysname);
	printf("Machine Name =  %s\n", buf.nodename);
	printf("Version = %s\n", buf.version);
	printf("Release = %s\n", buf.release);
	printf("Architecture = %s\n", buf.machine);
    printf("--------------------------------------------------\n");
	return;
}

int getNumUsers(){
 struct utmp *n;
 	setutent();
 	n=getutent();
    int count = 0;
 	while(n!=NULL){
  		if(n->ut_type==7){
 			count++;
  		}
  n=getutent();

}
return count;
}


//prints all the users on the system
void writeUsers( int pipe){
    struct utmp *n;
 	setutent();
 	n=getutent();
    char memSample[1000];

 	while(n!=NULL){
  		if(n->ut_type==7){
 			sprintf(memSample, "%s          %s  (%s)",n->ut_name, n->ut_line, n->ut_host);
   			write(pipe, memSample, sizeof(memSample));
  		}
  n=getutent();
 }

}

//calculates cpu usage and prints the percentage
tuple writeProcessorUsage( double ptotal, double pusage, int i, int cpuPipe, int graphicsPipe){
    tuple cpudata;
    FILE *f;
    f = fopen("/proc/stat","r");
    double user, nice, system, idle, iowait, irq, softirq;
    double total, usage, util;
    char cpuEntry[1000];
    fscanf(f, "cpu %lf %lf %lf %lf %lf %lf %lf", &user, &nice, &system, &idle, &iowait, &irq, &softirq);
    fclose(f);

    total = user + nice + system + idle + iowait + irq + softirq;
    usage = total - idle;
    util = ((usage - pusage) / (total - ptotal)) *100;
    if (i == 0){
       util = 0;
    }
    int bars = (int) (util);
    if (bars == 0){
        bars +=1;
    }

      char *h = malloc(bars + 1);
        memset(h, '|', bars);
        h[bars] = '\0';
    sprintf(cpuEntry, "total cpu use = %.2f%%\n", util);
    write(cpuPipe, cpuEntry, strlen(cpuEntry) +1);

    sprintf(cpuEntry, "         %s %.2f%%", h, util);
    write(graphicsPipe, cpuEntry, strlen(cpuEntry)+1 );

    cpudata.total = total;
    cpudata.usage = usage;
    return cpudata;
}




