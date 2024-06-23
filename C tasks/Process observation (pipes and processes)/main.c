#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

void  execQ(char*, char**);
void saveInFile(int, int, int, int);
bool stopCondition(int, int, int, int);

void execQ(char* Q, char** params)
{
	pid_t pid = fork();
    if (pid<0)
	{
		err(1, "Could not fork!");
	}

	if (pid==0)
	{
	    execvp(Q, params);  //since the sys call execvp takes an array (not a list!)of arguments 
    	err(1, "Could not exec Q");
	}
}

void saveInFile(int fd, int startTime, int endTime, int exitCode)
{
	if (write(fd, &startTime, sizeof(startTime))<0)
	{
		err(1, "Could not save startTime!");
	}

	if (write(fd, &endTime, sizeof(endTime))<0)
	{
		err(1, "Could not save endTime!");
	}

	if (write(fd, &exitCode, sizeof(exitCode))<0)
	{
		err(1, "Could not save exit code!");
	}

	char byte='\n';
	if (write (fd, &byte, sizeof(byte))<0)
	{
		err(1, "New line!");
	}
}

bool stopCondition(int exitCode, int prag, int startTime, int endTime)
{
     return (exitCode!=0 && (endTime-startTime)<prag);
}

int main(int argc, char* argv[])
{
   if (argc < 3)
   {
   	   errx(1, "Wrong arg count!");
   }

   //Task's solution algorithm: Q is a P's child. P calls exec on Q multiple times until 
   // the stop condition has been fulfilled

   //function for Q's execution -> forking multiple times there
   // startTime = time (NULL)
   // execQ 
   // endTime = time (NULL)
   // wait status na Q- is this the exit code?
   // open run.log- file and save sT, exitCode and eT
   //stopCondition check-> in bool stopCheck
   //if true -> kill Q  

   char* endPtr;

   int prag = strtol(argv[1], &endPtr, 10);

   if (*argv[1]=='\0' || *endPtr != '\0')
   {
      errx(1, "Expected integer, got %s", argv[1]);
   }

   if (prag < 1 || prag > 9)
   {
   	   errx(1, "Incorrect interval!");
   }

    char** parametersOfQ = argv + 2; 
    //parametersOfQ[argc-2] = NULL;  //No need for this since argv is a null-terminated string array by default

	int counter = 0;

	int fd = open("~/hrisiFile2" , O_RDWR|O_CREAT| O_TRUNC|S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (fd<0) {err(1, "Could not open file!");}

	
    while (true)
	{
		int startTime = time(NULL); 

	    execQ (argv[2], parametersOfQ);

		int status;

		if (wait(&status)==-1) {err(1, "Wait!");}
       
       	int exitCode;

		if (WIFEXITED(status))
		{
		  exitCode = WEXITSTATUS(status);	
		}
		else
		{
			exitCode = 129; //particular task's conventions
		}

		int endTime = time(NULL);

		saveInFile(fd, startTime, endTime, exitCode);

		if (stopCondition(exitCode, prag, startTime, endTime))
		{
			counter++;
			if (counter==2)
			{
				break;
			}
		}
		else
		{
			counter = 0;
		}     
	}

	close(fd);
 
}
