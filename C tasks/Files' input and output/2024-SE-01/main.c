
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <err.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

void zeroReplace(int, off_t);
bool check(int, off_t);

static uint8_t zeroArr[504];  //static global arr-> initialized with zeros by default

typedef struct Record
{
        uint64_t next;
        uint8_t data[504];
}Record;

void zeroReplace(int fd, off_t index)
{
        if(lseek(fd, index, SEEK_SET)<0)
        {
                err(1, "Could not lseek in source file!");
        }

    uint64_t next = 0x00;
    if (write(fd, &next, sizeof(uint64_t))<0 || write(fd, zeroArr, 504*sizeof(uint8_t))<0)
        {
                err(1, "Could not write zeros in source file!");
        }
}

bool check(int tmpFd, off_t index)
{
    if (lseek(tmpFd, index, SEEK_SET)<0)
        {
                err(1, "Could not lseek in tmpFile!");
        }
        off_t value;

        if (read(tmpFd, &value, sizeof(off_t))<0)
        {
                err(1, "Could not read from tempFile!");
        }
        return value==index;
}

int main(int argc, char* argv[])
{
        if (argc!=2)
        {
          err(1, "Wrong arg count!");
        }

    int fd = open(argv[1], O_RDWR);
    if (fd<0) {err(1, "Could not open source file!");}

    char tempFileName[] = "/tmp/tmpFXXXXXX";

        int tmpFd = mkstemp(tempFileName);
        if(tmpFd<0)
        {
                err(1, "mkstemp");
        }
    int readSize;

        Record rec;

    while(true)
        {
                off_t currentPos = lseek(fd, 0, SEEK_CUR);
                if(currentPos<0)
                {
                        err(1, "Could not lseek in source file!");
                }

        write(tmpFd, &currentPos, sizeof(off_t)); 
        read(fd, &rec, sizeof(Record));
        if (rec.next==0) {break;} 

        lseek(fd, rec.next*sizeof(Record), SEEK_SET);
        }

        lseek(fd, 0, SEEK_SET);

        while((readSize = read(fd, &rec, sizeof(Record)))>0)
        {
                off_t currentPos = lseek(fd, 0, SEEK_CUR);
                if (!check(tmpFd, currentPos))
                {
                        zeroReplace(fd, currentPos);
                }
        }

    if (readSize<0)
        {
                err(1, "Error while processing source file!");
        }

        close(fd);
        close(tmpFd);
        unlink(tempFileName);
}
