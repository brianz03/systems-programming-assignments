#include "stats_functions.h"
#include <signal.h>
#include <sys/wait.h>

//handles parent signal
void parentHandler (int signum){
  char answer;

        if (signum == SIGUSR1){
            return;
        }


        if (signum == SIGINT){
            
        printf("Do you want to quit the program? y/n: ");
        scanf("%c",&answer);
                if(answer=='y'){
                        
                        exit(0);
            }
                else{
                    kill(0, SIGCONT);
                 }
        }

}

//handle child signal
void childHandler(int signum){
        kill(getpid(), SIGSTOP);
}



//function which prints all the system information
void print(bool showSystem, bool showUser, bool showGraphics, bool sequential, int samples, int tdelay){
    
    char memSample[1000];
    char memSamples[samples][1000];
    char cpuGFXSample[1000];
    char cpuUse[1000];
    char cpuSamples[samples][1000];
    char userSample[1000];
    int j;
    double prev = -1000;
    tuple cpudata;
    double ptotal = 0.0;
    double pusage = 0.0;
    cpudata.total = ptotal;
    cpudata.usage = pusage;
    int status = 0;

// create pipes and check for error
    int mem_pipe[2], cpu_pipe[2], graphics_pipe[2], user_pipe[2];
    if (pipe(mem_pipe) < 0)
    {
        perror("pipe");
        exit(1);
    }
    if (pipe(cpu_pipe) < 0 )
    {
        perror("pipe");
        exit(1);
    }
    if (pipe(graphics_pipe)  < 0 )
    {
        perror("pipe");
        exit(1);
    }
    if (pipe(user_pipe) < 0 )
    {
        perror("pipe");
        exit(1);
    }

//memory pipe fork
    pid_t mem_pid = fork();
    if (mem_pid < 0)
    {
        perror("fork");
        exit(1);
    }
    else if (mem_pid == 0)
    {
        
signal(SIGINT, childHandler);
        close(mem_pipe[0]); 
        for (int i = 0; i < samples; i++)
        {   
            prev = writeMemUsage( showGraphics, prev, mem_pipe[1]); 
            sleep(tdelay);                
        }
        exit(0); 
    }
    else {
        close(mem_pipe[1]);
    }


// cpu fork
    pid_t cpu_pid = fork();
    if (cpu_pid < 0)
    {
        perror("fork");
        exit(1);
    }
    else if (cpu_pid == 0)
    {
        
signal(SIGINT, childHandler);
        close(cpu_pipe[0]); 
        close(graphics_pipe[0]); 
        for (int i = 0; i < samples; i++)
        {   
            cpudata = writeProcessorUsage(cpudata.total, cpudata.usage, i, cpu_pipe[1], graphics_pipe[1]);
            sleep(tdelay);                
        }
        exit(0); 
    }
    else {

close(cpu_pipe[1]);
close(graphics_pipe[1]);
    }

//user fork
    pid_t user_pid = fork();
    if (user_pid < 0)
    {
        perror("fork");
        exit(1);
    }
    else if (user_pid == 0)
    {
        
signal(SIGINT, childHandler);
        close(user_pipe[0]); 
        for (int i = 0; i < samples; i++)
        {   
            writeUsers(user_pipe[1]);
            sleep(tdelay);   
        }
        exit(0); 
    }
    else {
        
        close(user_pipe[1]);
    }




signal(SIGINT, parentHandler);
signal(SIGUSR1, parentHandler);

  for (int i=0; i < samples; i++){ //loops for number of samples specified
  
//read data from children
     read(mem_pipe[0], memSample, sizeof(memSample)); 
     strcpy(memSamples[i], memSample);

     read(cpu_pipe[0], cpuUse, sizeof(cpuUse)); 
     read(graphics_pipe[0], cpuGFXSample, sizeof(cpuGFXSample)); 
     strcpy(cpuSamples[i], cpuGFXSample);
 
if (sequential == false){   //checks if sequential is checked, if it is false, the screen gets cleared each time
		printf("\033c");
	}
	
    

    printf("Nbr of samples: %d -- every %d secs\n", samples, tdelay);
    getProgramUsage();
	printf("--------------------------------------------------\n");

    if (showSystem == true){ 
	printf("### Memory ### (Phys.Used/Tot -- Virtual Used/Tot)\n");

   
    for(j=0; j<= i; j++){  //prints all the samples for memory usage
	    printf("%s\n", memSamples[j]);
		
	}
    printSpace(samples - i - 1);    
	printf("--------------------------------------------------\n");
}

if (showUser == true){
    printf("### Sessions/users ###\n");
    
        for (int i = 0; i < getNumUsers(); i++) {
            read(user_pipe[0], userSample, sizeof(userSample));
            printf("%s\n", userSample);
        }

	printf("--------------------------------------------------\n");
    
}

if (showSystem == true){
    numCores();
    printf("%s",cpuUse);
    
    if (showGraphics == true){
    for(j=0; j<= i; j++){  //prints all the samples for memory usage
	    printf("%s\n", cpuSamples[j]);
		
	    }
    }
    
    }

    } 
     waitpid(cpu_pid, &status, 0);
     waitpid(mem_pid, &status, 0);
     waitpid(user_pid, &status, 0);

     if (showUser == false || (showSystem == true && showGraphics == true && showUser == true)){
        
	    printf("--------------------------------------------------\n");
     }
    
    printSysInfo();
    
     
    
}  



int main(int argc, char *argv[]){
    bool showSystem = true;
    bool showUser = true;
    bool showGraphics = false;
    bool sequential = false;
    int samples = 10;
    int tdelay = 1;
    int count = 0;

signal(SIGTSTP, SIG_IGN);

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
if (samples <= 0 ) {
    samples = 10;
} 
if (tdelay <= 0){
    tdelay = 1;
}

//sends all the arguments to the print function
print(showSystem, showUser, showGraphics, sequential, samples, tdelay);
}

