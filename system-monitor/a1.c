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

//function to print new lines
void printSpace(int num){
    int i;
    for (i = 0; i < num; i++){
        printf("\n");
    }
}

//prints the memory usage of the program
void printProgramUsage(){
   struct rusage r_usage;
   getrusage(RUSAGE_SELF, &r_usage);
   printf("Memory usage: %ld kilobytes\n",r_usage.ru_maxrss);
}


// returns information about memory usage in a string '*str' 
void printMemUsage(char *str) {
    struct sysinfo mem;
    //calculates memory usage
    sysinfo(&mem);
    double ptotal = (double)mem.totalram / (1024 * 1024 * 1024);
    double pused = (double)(mem.totalram - mem.freeram) / (1024 * 1024 * 1024);
    double vtotal = (double)(mem.totalram + mem.totalswap) / (1024 * 1024 * 1024);
    double vused = (double)(mem.totalram + mem.totalswap - (mem.freeram + mem.freeswap)) / (1024 * 1024 * 1024);
    sprintf(str, "\n%.2f GB / %.2f GB  --  %.2f GB / %.2f GB", pused, ptotal, vused, vtotal);
    printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");
   
}
//prints the system information 
void printSysInfo() {

	struct utsname buf;
    uname(&buf);
	printf("\n### System Information ###\n");
	printf("System Name = %s\n", buf.sysname);
	printf("Machine Name =  %s\n", buf.nodename);
	printf("Version = %s\n", buf.version);
	printf("Release = %s\n", buf.release);
	printf("Architecture = %s\n", buf.machine);
    printf("--------------------------------------------------\n");
	return;
}
//prints all the users on the system
void printUsers(){
    struct utmp *n;
 	setutent();
 	n=getutent();
	printf("### Sessions/users ###\n");
 	while(n!=NULL){
  		if(n->ut_type==7){
 			printf("%s          ",n->ut_name);
   			printf("%s",n->ut_line);
   			printf(" ");
   			printf(" (");
   			printf("%s",n->ut_host);
   			printf(")\n");
  		}
  n=getutent();
 }
 printf("--------------------------------------------------\n");
}
//prints the number of cores
void printCores() {
    int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    printf("Number of cores: %d\n", num_cores);
	printf("--------------------------------------------------");
}

//calculates cpu usage and prints the percentage
void cpuUsage(){
    FILE *f;
    f = fopen("/proc/stat","r");
    double user, nice, system, idle, iowait, irq, softirq;
    double total, percentage;
    fscanf(f, "cpu %lf %lf %lf %lf %lf %lf %lf", &user, &nice, &system, &idle, &iowait, &irq, &softirq);
    fclose(f);
    total = user + nice + system + idle + iowait + irq + softirq;
    percentage = (1.0 - (idle/total))*100;
    printf("total cpu use = %.2f%%\n", percentage);
}

//function which prints all the system information
void print(bool showSystem, bool showUser, bool showGraphics, bool sequential, int samples, int tdelay){
    
    char str[43];
    char memSamples[samples][43];
    int i;
    int j;

  for (int i=0; i < samples; i++){ //loops for number of samples specified
	
	if (sequential == false){   //checks if sequential is checked, if it is false, the screen gets cleared each time
		printf("\033c");
	}
	
    printf("\nNbr of samples: %d -- every %d secs\n", samples, tdelay);
    printProgramUsage();
	printf("--------------------------------------------------\n");
    if (showSystem == true){ //is the user specifies to show system, only system usage is shown
	
    printMemUsage(str);
    strcpy(memSamples[i], str);
    
	printf("%s", *memSamples);
		
	
    printSpace(samples - i);  
	 printf("--------------------------------------------------\n");
	}
	 if (showUser == true){
	 	printUsers();
	 }
    if (showSystem == true){
	
    cpuUsage();
    printCores();
	}
    sleep(tdelay); //sleeps for amount of time specified
    
  }
printSysInfo(); //print system info after looping
}

int main(int argc, char *argv[]){
    int i;
    bool showSystem = true;
    bool showUser = true;
    bool showGraphics = false;
    bool sequential = false;
    int samples = 10;
    int tdelay = 1;
    int count = 0;

for(int i = 1;i<argc;i++){ //loops through the command line arguments and sets the proper values
    char *str = argv[i];
    int num = atoi(argv[i]); 
   
   
		if(strcmp(argv[i],"--system")== 0){  //checks if user wants to show only system usage
				showSystem= true;            //if user enter both --system and --user the last argument will be used
				showUser= false;
		}

        else if(strcmp(argv[i],"--user")== 0){
				showSystem= false;
				showUser= true;

		}

        else if(num != 0){ //if command line argument is an integer, set value to samples or tdelay.
				if (count == 0 && num >=1){  //if user enters more than 2 numbers, the first 2 are used 
                    samples = num;
                    
                }
                else if (count == 1 && num >=1){
                    tdelay = num;
                }
            count++;
		}
        else if(strcmp(argv[i],"--graphics")==0||strcmp(argv[i],"--g")==0){
			showGraphics= true;
        }
        else if(strcmp(argv[i],"--sequential")== 0){
				sequential = true;

		}
		else if(strncmp(argv[i],"--samples=", 10)==0){
            strtok(str, "=");
			char *num = strtok(NULL, "=");
            if (num != NULL){
            samples = strtol(num, NULL, 10);
            }
            
		}
		else if(strncmp(argv[i],"--tdelay=", 9)==0){
			strtok(str, "=");
            char *num = strtok(NULL, "=");
            if (num != NULL){
            tdelay = strtol(num, NULL, 10); 
            }
        }
}

//sends all the arguments to the print function
print(showSystem, showUser, showGraphics, sequential, samples, tdelay);
    
}