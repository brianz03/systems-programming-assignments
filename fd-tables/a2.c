#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <sys/types.h>
#include <pwd.h>
#include <errno.h>
#include <limits.h>

#pragma GCC diagnostic ignored "-Wformat-truncation"


void printInfo(int pid, bool PPflag, bool systemWideFlag, bool VnodesFlag, bool compositeFlag, int *offendingCount, int threshold, int **pidArr, int **fdArr)
{
	//prints the fds for a pid

	char path[4096];
	snprintf(path, 4096, "/proc/%d/fd", pid);
	struct stat st;
	char fdpath[4096];
	char filename[4096];
	ssize_t count;
	DIR * dirp;
	struct dirent * dp;
	int counter = 0;

	//open proc/pid/fd folder
	dirp = opendir(path);
	if (dirp == NULL)
	{
		//perror("opendir");
		return;
	}

	while ((dp = readdir(dirp)) != NULL)
	{
		//loop through all file descriptors
		if (!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, ".."))
		{
			continue;
		}

		snprintf(fdpath, 4096, "%s/%s", path, dp->d_name);
		count = readlink(fdpath, filename, 4096);

		//get counts all the file descriptors for threshold flag   
		if (count == -1)
		{
			perror("readlink");
			continue;
		}
			counter++;

		filename[count] = '\0';

		if (stat(fdpath, &st) == -1)
		{
			perror("stat");
			continue;
		}

		//prints the table depending on flags
		if (PPflag == true)
		{
			printf(" %d       %s\n", pid, dp->d_name);
		}
		else if (systemWideFlag == true)
		{
			printf(" %d      %s     %s\n", pid, dp->d_name, filename);
		}
		else if (VnodesFlag == true)
		{
			printf(" %s      %ld\n", dp->d_name, st.st_ino);
		}
		else if (compositeFlag == true)
		{
			printf(" %d       %s        %s     %ld\n", pid, dp->d_name, filename, st.st_ino);
			
	
		}
	}

	//if the number of fd's is greater than the threshold, the pid and count are stored.
	if (counter > threshold)
	{
		int size = sizeof(int) + sizeof(int) * ((*offendingCount));
		*pidArr = realloc(*pidArr, size);
		(*pidArr)[*offendingCount] = pid;
		*fdArr = realloc(*fdArr, size);
		(*fdArr)[*offendingCount] = counter;
		*offendingCount += 1;
	}

	closedir(dirp);

}

void getPID(bool PPflag, bool systemWideFlag, bool VnodesFlag, bool compositeFlag, int specifiedPID, int *offendingCount, int threshold, int **pidArr, int **fdArr)
{
	DIR * dirp;
	struct dirent * dp;
	struct passwd *p = getpwuid(getuid());
	struct stat buf;
	int pid = -1;
	//check if positional argument is used
	if (specifiedPID >= 0)
	{
		//printf("pid %d\n", specifiedPID);
		printInfo(specifiedPID, PPflag, systemWideFlag, VnodesFlag, compositeFlag, offendingCount, threshold, pidArr, fdArr);
		*offendingCount = 0;
		getPID(false, false, false, false, -1, offendingCount, threshold, pidArr, fdArr);
	}
	else
	{
		dirp = opendir("/proc");
		while ((dp = readdir(dirp)) != NULL)
		{
			// loop through proc folder and get the pids, then they are sent to printInfo function
			sscanf(dp->d_name, "%d", &pid);
			lstat(dp->d_name, &buf);

			if (pid <= 0 )
			{
				continue;
			}

			if (buf.st_uid == p->pw_uid)
			{
				printInfo(pid, PPflag, systemWideFlag, VnodesFlag, compositeFlag, offendingCount, threshold, pidArr, fdArr);
			}
		}

		closedir(dirp);
	}
}

int main(int argc, char *argv[])
{
	bool showPP = false;
	bool showSystemWide = false;
	bool showVnodes = false;
	bool showComposite = false;
	int threshold = -1;
	int pid = -1;
	int offendingCount = 0;
	int *pidArr = NULL;
	int *fdArr = NULL;
	
	//get command line arguments
	for (int i = 1; i < argc; i++)
	{
		char *str = argv[i];
		int num = atoi(argv[i]);

		if (strcmp(argv[i], "--per-process") == 0)
		{
			showPP = true;
		}
		else if (strcmp(argv[i], "--systemWide") == 0)
		{
			showSystemWide = true;
		}
		else if (strcmp(argv[i], "--Vnodes") == 0)
		{
			showVnodes = true;
		}
		else if (strcmp(argv[i], "--composite") == 0)
		{
			showComposite = true;
		}	else if (strncmp(argv[i], "--threshold=", 12) == 0)
		{
			strtok(str, "=");
			char *num = strtok(NULL, "=");
			if (num != NULL)
			{
				threshold = strtol(num, NULL, 10);
			}
		}
		else if (num >= 1)
		{
			pid = num;
		}
	}

	//if no flags given, all tables are printed
	if (showPP == false && showSystemWide == false && showVnodes == false && showComposite == false)
	{
		showPP = true;
		showSystemWide = true;
		showVnodes = true;
		showComposite = true;
	}

	//prints the table names and calls function to get PIDs
	if (showPP == true)
	{
		printf(" PID     FD \n");
		printf("============\n");

		offendingCount = 0;
		getPID(true, false, false, false, pid, &offendingCount, threshold, &pidArr, &fdArr);
		printf("\n");
	}

	if (showSystemWide == true)
	{
		printf(" PID     FD      Filename \n");
		printf("========================================\n");
		offendingCount = 0;
		getPID(false, true, false, false, pid, &offendingCount, threshold, &pidArr, &fdArr);
		printf("\n");
	}

	if (showVnodes == true)
	{
		printf("   FD            Inode\n");
		printf("========================================\n");
		offendingCount = 0;
		getPID(false, false, true, false, pid, &offendingCount, threshold, &pidArr, &fdArr);
		printf("\n");
	}

	if (showComposite == true)
	{
		printf(" PID     FD       Filename       Inode\n");
		printf("===============================================\n");
		
		offendingCount = 0;
		getPID(false, false, false, true, pid, &offendingCount, threshold, &pidArr, &fdArr);
		printf("\n");
		
	}

	//prints the offending proccesses if specified
	if (threshold >= 0)
	{
		printf("## Offending processes:\n");

		for (int i = 0; i < offendingCount; i++)
		{
			if (i == 0)
			{
				printf("%d (%d)", pidArr[i], fdArr[i]);
				
			}
			else
			{
				printf(", %d (%d)", pidArr[i], fdArr[i]);
				
			}
		}
	}

	free(pidArr);
	free(fdArr);

}