#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void parentF(int, int, int, int);
void handleChild(int, int);

void parentF(int toChild, int fromChild, int N, int D)
{
	for (int i = 0; i<N; i++)
  {
	ssize_t bytesToWrite=strlen("DING");
	
	if ((write(1, "DING" , bytesToWrite))!= bytesToWrite)
	{
		err(1, "Parent could not print!");	
	}
   
   	int dummyPar=9;

	if ((write(toChild, &dummyPar, sizeof(dummyPar))) != sizeof(dummyPar))
	{
		err(1, "Could not write dummy par!");
	}

    if ((read(fromChild, &dummyPar, sizeof(dummyPar)))<=0)
	{
		err(1, "No signal from child!");
	}
	sleep(D);
  }
}

void handleChild(int toParent, int fromParent)
{
   int dummyPar;    
   ssize_t readSize;

   while((readSize=read(fromParent, &dummyPar, sizeof(dummyPar)))>0)
   {
      ssize_t bytesToPrint=strlen("DONG");
   if( (write (1, "DONG", bytesToPrint)) != bytesToPrint)
   {
      err(1, "Child could not print!");
   }
  
   dummyPar=8;

   if((write(toParent, &dummyPar, sizeof(dummyPar)))!=sizeof(dummyPar))
   {
   	   err(1, "Child could not signal!");
   }
  
  }

   if (readSize<0)
   {
   	   err(1, "No signal from parent!");
   }
}

int main(int argc, char* argv[])
{
	 //argv[1] = N
    //argv[2] = D

    if (argc != 3)
	{
		errx(1, "Wrong args count!");
	}
     
    char* endPtr;

    int N=strtol(argv[1], &endPtr, 10);
    int D=strtol(argv[2], &endPtr, 10);

	if (*argv[1]=='\0' || *endPtr!='\0')
	{
       err(1, "N must be an integer, got %s", argv[1]);
	}
	
    if (N<0)
    {
    	err(1, "N must be non-negative!");
    }

	if (*argv[2]=='\0' || *endPtr!='\0')
	{
        err(1,"D must be an integer, got %s", argv[2]);
	}

	if (D<0)
	{
		err(1, "D must be non-negative!");
	}


    int parentToChild[2];
    int childToParent[2];
    
    if (pipe(parentToChild) == -1)
	{
		err (1, "Could not pipe!");
	}

	if (pipe(childToParent)==-1)
	{
		err(1, "Could not pipe!");
	}

	pid_t pid = fork();

	if (pid==-1)
	{
		errx(1, "Could not fork!");
	}
 
	if (pid==0)
	{
		close(parentToChild[1]);
		close(childToParent[0]);  
	    
	    handleChild(childToParent[1], parentToChild[0]);	
		
		close(parentToChild[0]);
		close(childToParent[1]);
	}

	//parent:
   close(childToParent[1]);
   close (parentToChild[0]);  
   parentF(parentToChild[1], childToParent[0], N, D);


   close(parentToChild[1]);
   close(childToParent[0]);

   int status;
 	if((wait(&status))==-1)
	{
		err (1, "wait");
	}

	if (WIFEXITED(status)!=0)
	{
		err(1,"Child has been killed!");
	}
    
}

