
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <err.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>


int main(int argc, char* argv[])
{
        if(argc!=3)
        {
                errx(1, "Wrong arg count!");
        }

        int fd = open(argv[1], O_RDONLY);

        int resFd = open(argv[2], O_CREAT|O_TRUNC|O_RDWR, 0666);

        if (fd<0)
        {
                err(1, "Could not open input file!");
        }
        int pFd[2];
    if (pipe(pFd)<0)
        {
                err(1, "Could not create a pipe!");
        }

        pid_t pid = fork();
        if (pid<0)
        {
                err(1, "Could not fork!");
        }

        if (pid==0)
        {
                close(pFd[0]);

            if (dup2(pFd[1], 1)<0)
                {
                        err(1, "Could not write to pipe!");
                }

            close(pFd[1]);
            execlp("cat", "cat", argv[1], (char*)NULL);
            err(1, "Could not exec!");
    }
        else
        {
                close(pFd[1]);
                int status;
                wait(&status);

                if (WIFEXITED(status))
                {
                        if (WEXITSTATUS(status)!=0)
                        {
                                warnx("Child exited with status %d", WEXITSTATUS(status));
                        }
                }
                else
                {
                        warnx("Child has been killed!");
                }

                uint8_t byte;
                int readSize;

                while ((readSize = read(pFd[0], &byte, sizeof(uint8_t)))>0)
                {
                        if (byte==0x7D) //escape character
                        {
                                read(pFd[0], &byte, sizeof(uint8_t));
                                uint8_t newByte = byte^0x20;
                                write(resFd, &newByte, sizeof(uint8_t));
                        }
                        else if (byte==0x55)
                        {
                                continue;
                        }
                        else
                        {
                          write(resFd, &byte, sizeof(uint8_t));
                        }
                }

                if (readSize<0)
                {
                        err(1, "Error while reading data from stream!");
                }
        }

        close(pFd[0]);
        close(fd);
        close(resFd);

        return 0;
}
