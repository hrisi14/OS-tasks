#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <err.h>

uint16_t getResult(uint8_t);

uint16_t getResult(uint8_t byte)
{
    uint16_t result = 0; 
    uint8_t mask = 1<<7;

	for (int i = 0; i<8; i++)
	{
		result<<=2;

		if (byte & mask)
		{
		   result|=2;
		}
		else
		{
		  result |= 1;	
		}

		mask>>=1;
	}
	return result;
}

int main(int argc, char* argv[])
{
    if (argc != 3)
	{
		err(1, "Wrong arg count!");
	}

 	int f1= open(argv[1], O_RDONLY);
    
    if (f1<0)  { err(1, "Could not open first file!"); }

    int f2 = open(argv[2], O_CREAT| O_TRUNC|O_RDWR, 0777); //Need third parameter (perm mode) when opening a file with O_CREAT's flag

    if (f2<0) { err(1, "Could not open second file!"); }

    uint8_t byte;
    int readSize;

    while((readSize = read(f1, &byte, sizeof(byte)))>0)
	{
		uint16_t result = getResult(byte);
        //Endianness!!!

        uint8_t* ptr = (uint8_t*)(&result);
        
        //Saving the two bytes in reverse order because of endianness:

		if (write(f2, &ptr[1], sizeof(uint8_t)) != sizeof(uint8_t))
		{
			err(1, "Could not save result to output.bin!");
		}

		if (write(f2, &ptr[0], sizeof(uint8_t)) != sizeof(uint8_t))
		{
			err(1, "Could not save result to output.bin!");
		}
	}

	if (readSize<0) { err(1, "Reading from input bin!"); }

    close(f1);
    close(f2);
}
