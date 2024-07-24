#include <fcntl.h>
#include <err.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

void processSDL(int, int, int);
void processSCL(int, int, int);

void processSDL(int sdlFd, int index, int resFd)
{
   uint16_t el;

   lseek(sdlFd, index*sizeof(uint16_t), SEEK_SET);
   read(sdlFd, &el, sizeof(uint16_t));
   write(resFd, &el, sizeof(uint16_t));
}

void processSCL(int sclFd, int sdlFd, int resFd)
{
        uint8_t byte;
    int readSize;

        int index = 0;
        while ((readSize = read(sclFd, &byte, sizeof(uint8_t)))>0)
        {
       for (uint8_t i = 1<<7; i>0; i>>=1)
           {
                   if (byte&i)
                   {
              processSDL(sdlFd, index, resFd);
                   }

                   index++;
           }
        }
}

int main (int argc, char* argv[])
{
        if (argc!=3)
        {
                errx(1, "Wrong arg count!");
        }

    int sclFd = open(argv[1], O_RDONLY);
    int sdlFd = open(argv[2], O_RDONLY);
    int resFd = open("/home/students/XXXXXXX/CTasks/85-SCL/Dir/output.bin", O_CREAT|O_TRUNC|O_RDWR, 0666);

    processSCL(sclFd, sdlFd, resFd);

    close(sclFd);
    close(sdlFd);
    close(resFd);
}
