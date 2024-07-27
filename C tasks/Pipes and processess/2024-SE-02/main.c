#include <fcntl.h>
#include <errno.h>
#include <err.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>

const int MAX = 65536;
void execProgram(const char*, int);
void saveRandomData(int, const int8_t*, uint16_t);
void readRandomData(int, int, int8_t*, uint16_t*);

void readRandomData(int randFd, int toChild, int8_t* bytes, uint16_t* S)
{
        if (read(randFd, S, sizeof(uint16_t))<0)
        {
                err(1, "Could not read S!");
        }

    int index = 0;
    int8_t byte;

    for (uint16_t i = 0; i<*S; i++)
        {
                if (read(randFd, &byte, sizeof(int8_t))<0)
                {
                        err(1, "Could not read a random byte!");
                }

        if (write(toChild, &byte, sizeof(int8_t))<0)
                {
                        err(1, "Could not write data to child process!");
                }
                bytes[index++]=byte;
        }
}

void saveRandomData(int resFd, const int8_t* bytes, uint16_t S)
{
        for (uint16_t i = 0; i<S; i++)
        {
         if (write(resFd, &bytes[i], sizeof(int8_t))<0)
            {
              err(1, "Could not save data to result file!");
            }
        }
}

void execProgram(const char* program, int fromParent)
{
   int res = execl(program, program, NULL);  
   if (res<0)
   {
           err(1, "Could not exec!");
   }
}

int main (int argc, char* argv[])
{
   if (argc!=4)
   {
           err(1, "Wrong arg count!");
   }

   char *endPtr;
   uint8_t N = strtol(argv[2], &endPtr, 10);

   if (*argv[2]=='\0' || *endPtr!='\0')
   {
           errx(1, "Could not convert argument to number!");
   }


   int randFd = open("/dev/urandom", O_RDONLY);
   if (randFd<0)
   {
           err(1, "Could not open the file with random bytes!");
   }

   int resFd = open(argv[3], O_RDWR);
   if(resFd<0)
   {
           err(1, "Could not open result file!");
   }

   int exitFd = open("/dev/null", O_WRONLY);
   if (exitFd<0) {err(1, "Could not open /dev/null to redirect exit!");}

   int8_t bytes[MAX];
   uint16_t S;

   for (int8_t i = 0; i<N; i++)
  {
   int pFds[2];
   if( pipe(pFds)<0)
   {
           err(1, "Could not create a pipe!");
   }

    pid_t pid = fork(); 
    if (pid<0) {err(1, "Could not fork!"); }
   
    if (pid!=0)
   {
           readRandomData(randFd, pFds[1], bytes, &S);
       close(pFds[1]);
   }

   if (pid==0)
   {
          if (dup2 (pFds[0], 0)==-1) {err(1, "Could not redirect child's stdin!");}

          if (dup2(exitFd, 1)==-1)
          {
                  err(1, "Could not redirect child's output!");
          }

          if(dup2(exitFd, 2)==-1)
          {
                  err(1, "Could not redirect child's error output!");
          }
          close(pFds[1]);
      execProgram(argv[1], pFds[0]);
   }

   int status;
   if (wait(&status)<0)
   {
           err(1, "Error while waiting child!");
   }

   if (!WIFEXITED(status))
   {
           saveRandomData(resFd, bytes, S);
           close(randFd);
           close(resFd);
           close(exitFd);
           return 42;
   }
  }

   close(randFd);
   close(resFd);
   close(exitFd);
   return 0;
}
