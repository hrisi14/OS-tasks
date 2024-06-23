#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <err.h>
#include <unistd.h>


ssize_t getFileSize(int);

ssize_t getFileSize(int fd)  //or int, or off_t type
{
	struct stat info;

	int res = fstat(fd, &info);

	if (res<0)  {err(1, "Stat err!"); }

	return info.st_size;
}

typedef struct diff
{
	uint16_t offset;
	uint8_t original;
	uint8_t newByte;
   
} diff;

int main(int argc, char* argv[])
{
	if (argc!=3)
	{
		errx(1, "Wrong arg count!");
	}

	int f1 = open(argv[1], O_RDONLY);
	if (f1<0)  {err (1, "Could not open file!"); }

	int f2 = open(argv[2], O_RDONLY);

	if (f2<0) {err (1, "Could not open second file!"); }

   ssize_t size1 = getFileSize(f1);
   ssize_t size2 = getFileSize(f2);
  
   if (size1%sizeof(diff) != 0)
   {
   	   err(1, "Invalid file's content!");
   }

   if (size2 % sizeof(diff) != 0)
   {
   	   err (1, "Invalid file's comtent!");
   }
   
   //Check for the condition about a particular offset!

   if (size1 != size2)
   {
   	   err(1, "Different size!");
   }

   //Patch file's creation:

   int f3 = open(argv[3], O_CREAT|O_RDWR | S_IRUSR | S_IWUSR);

   uint16_t currentPos = 0;

   int readSize = 0;

   uint8_t byte1;
   uint8_t byte2;

   diff d;

   while ((readSize = read(f1, &byte1, sizeof(byte1)))>0)
   {
   	   if (read(f2, &byte2, sizeof(byte2))<0)
	   {
	   	   err(1, "Could not read from file2!");
	   }

	   if (byte1 != byte2)
	   {
	   	  d.offset = currentPos;
	   	  d.original = byte1;
	   	  d.newByte = byte2;

		  if (write(f3, &d, sizeof(d))<0)
		  {
		  	  err(1, "Could not write to patch file!");
		  }
	   }
   }

   if (readSize<0)  
   {
   	   err (1, "Could not read last byte!");
   }
   
  close(f1);
  close(f2);
  close(f3);
}
