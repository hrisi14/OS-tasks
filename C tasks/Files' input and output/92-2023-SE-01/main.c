#include <fcntl.h>
#include <errno.h>
#include <err.h>
#include <stdint.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>

bool checkMessage(int);
void saveMessage(int, int, off_t);


bool checkMessage(int fd)
{
    uint8_t N;
    if (read(fd, &N, sizeof(uint8_t)) != sizeof(uint8_t))
	{
		err(1, "Could not read N!");
	}

	char expectedCheckSum=0x55^N;

	char byte;
	for (uint8_t i = 0; i<N-3; i++)
	{
	  if (read(fd, &byte, sizeof(char))<0)
	  {
	  	  err(1, "Could not read byte from source file!");
	  }

	  expectedCheckSum^=byte;
	}

	char checkSum;
	if (read(fd, &checkSum, sizeof(char))<0)
	{
		err(1, "Could not read checksum from source file!");
	}

	return (expectedCheckSum==checkSum);

}

void saveMessage(int f1, int f2, off_t currentPos)
{
   if (lseek(f1, currentPos, SEEK_SET)<0)
   {
   	   err(1, "Could not lseek in source file before saving message!");
   }

   char byte = 0x55;

   if(write(f2, &byte, sizeof(char))!=sizeof(char))
   {
   	   err(1, "Could not save message's beginning!");
   }

   uint8_t N;
   if (read(f1, &N, sizeof(uint8_t))!= sizeof(uint8_t))
   {
   	   err(1, "Could not read N!");
   }
   
   if(write(f2, &N, sizeof(uint8_t))!=sizeof(uint8_t))
   {
   	   err(1, "Could not save N!");
   }

   for (uint8_t i = 0; i<N-2; i++)
   {
   	   if (read(f1, &byte, sizeof(char))!=sizeof(char))
	   {
	   	   err(1, "Error while reading during saving process!");
	   }

	   if (write(f2, &byte, sizeof(char))!=sizeof(char))
	   {
	   	   err(1, "Error while writing when saving message!");
	   }
   }
}

int main(int argc, char* argv[])
{
   if (argc != 3)
   {
   	   errx(1, "Invalid arg count!");
   }

   int f1 = open(argv[1], O_RDONLY);
   if (f1<0) {err(1, "Could not open source file!");}

   int f2  = open(argv[2], O_CREAT|O_TRUNC|O_WRONLY, 0666);
   if(f2<0) { err(1, "Could not creat destination file!"); }

   int readSize;

   char byte;
   while ((readSize = read(f1, &byte, sizeof(char)))>0)
   {
   	   if (byte==0x55)
	   {
	   	   off_t currentPos = lseek(f1, 0, SEEK_CUR);
	   	   if (currentPos<0)
		   {
		   	   err(1, "Could not lseek in source file!");
		   }

	   	   if (checkMessage(f1))
		   {
             printf("Saving message.");
		   	 saveMessage(f1, f2, currentPos);
		   }
	       
	       //Returning back to initial position:
	       if (lseek(f1, currentPos, SEEK_SET)<0) //Do I need multiplication to sizeof(char) here?
		   {
		   	   err(1, "Could not lseek to initial position!");
		   }
	   }
   }

   if (readSize<0)
   {
   	   err(1, "Could not finish reading from source file successfully!");
   }

   close(f1);
   close(f2);
}
