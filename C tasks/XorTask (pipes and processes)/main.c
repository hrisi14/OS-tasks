#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct file
{
   char fileName [8];
   uint32_t offset;
   uint32_t length;

}file;

void processXor(file*, int);

void processXor( file* strFile, int toFd)
{
	printf("sth");

	int fd = open(strFile->fileName, O_RDONLY);
    if (fd<0) { err(1, "Could not open file!" ); }

    uint16_t childResult=0;
    
    off_t seeking = lseek(fd, sizeof(uint16_t)*strFile->offset, SEEK_SET);
    if (seeking<0) {err(1, "Could not lseek!"); }

	//read and xor all the elCount- number of elements in the file
    for (uint32_t i = 0; i<strFile->length; i++)
	{
		uint16_t element;
		if (read(fd, &element, sizeof(element))!=sizeof(element)) 
		{
			err (1, "Could not read element!"); 
		}
		
        //Debugging: printf("Child xor-ed.");

		childResult ^= element;
	}

	close (fd);
    write(toFd, &childResult, sizeof(childResult));

}

int main(int argc, char* argv [])
{
   if (argc!=2)
   {
 	 errx(1, "Wrong arg count!");
   }

   
   //Algorithm for ensuring parallel processes' work:
   //iterating through the source file and forking for every new file name
  //then calling another function that gets the particular file's elements and xor-s them

    int fd = open(argv[1], O_RDONLY);
    if (fd<0) { err(1, "Could not open file!"); };
   

   	struct stat info;
    int statRes = fstat(fd, &info);
 
 	if (statRes<0) {err(1, "Could not stat file's size!");}

    if (info.st_size % sizeof(file)) {err (1, "Invalid file (size)!"); }

   int totalChildren = 0;
   uint16_t finalResult = 0; 
	int p_fds[2];

	if (pipe(p_fds)<0)
	{
		err (1, "Could not pipe!");
	}
    
    int readSize = 0;

	file strFile;

    while((readSize = read(fd, &strFile, sizeof(strFile)))>0)  
	{
    
        if (readSize<sizeof(file))
		{
			break;  //end of file or invalid bytes
		}

		totalChildren++;

       	pid_t pid = fork();
       	if (pid<0) { err(1, "Could not fork!"); };

	 if (pid==0)
	 {
	    close(fd);
	    close(p_fds[0]);
    	processXor(&strFile, p_fds[1]);
        close (p_fds[1]);  //Closing pipe' ends that are not used to escape deadlock 
        exit(0); //for every kid
	 }
  }

    close (p_fds[1]);
	close (fd);

	uint16_t ch;

   	while ((readSize = read(p_fds[0], &ch, sizeof (ch)))>0)
	{
      // Debugging: printf("Parent xor-ed!");
      finalResult^=ch;
	}

	close (p_fds[0]);

	if (readSize<0)
	{
		err(1, "Parent could not read from pipe!");
	}

   	//wait for all the children to finish
         
    for (int i = 0; i<totalChildren; i++)
	{
		int status;
		if (wait(&status)==-1)
		{
			err(1, "Wait");
		}
		
		if (WIFEXITED(status))
		{
            dprintf(1, "Exit status of child: %d \n", WEXITSTATUS(status));
		}
		else
		{
			//child has been killed

			if (WIFSIGNALED(status))
			{
				dprintf(1, "Signal that has caused the child to terminate: %d \n", WTERMSIG(status));
			}
		}
	}
	dprintf(1, "Result: %.4x\n", finalResult);
}
