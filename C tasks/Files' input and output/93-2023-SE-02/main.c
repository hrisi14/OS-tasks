#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <err.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

void getWord(char*, int, uint32_t);
void printDef(int);
void binarySearch(int, int, int, int, const char*);

void getWord(char* word, int fdDic, uint32_t index)
{
   if (lseek(fdDic, index+1, SEEK_SET)<0)
   {
   	   err(1, "Could not lseek in dictionary!");
   }

   char byte = 'a';

   int i = 0;

   while (true)
   {
      if (read(fdDic, &byte, sizeof(byte))<0)
	  {
	  	  err(1, "Could not read word from dictionary!");
	  }
 
 	   if (byte=='\n' || byte=='\0')
	  {
	  	  break;
	  }

	  word[i++] = byte;
   }

   word[i] = '\0';
}

void printDef(int fdDic)
{
   char byte;
   if (read(fdDic, &byte, sizeof(char))<0)
   {
   	   err(1, "Could not get to definition in the dictionary!");
   }

   if (byte != '\n' && byte != '\0')
   {
   	   write(1, &byte, sizeof(char));
   }

   int readSize;

   while((readSize = read(fdDic, &byte, sizeof(char)))>0)
   {
   	   if (byte==0x00)
	   {
	   	   break;  //end of definition
	   }

	   if (write(1, &byte, sizeof(char))<0)
	   {
	   	   err(1, "Error while printing definition!");
	   }
   }

   if (readSize<0)
   {
   	   err(1, "Error while reading definition!");
   }

}

void binarySearch(int l, int r, int fdDic, int fdInd, const char* searchedWord)
{
	char word[64];   
	while (l<=r)
	{
		int mid = l + (r-l)/2;

		uint32_t index;
		if (lseek(fdInd, mid*sizeof(uint32_t), SEEK_SET)<0)
		{
			err(1, "Could not lseek in the indexes file!");
		}
		if (read(fdInd, &index, sizeof(uint32_t))<0)
		{
			err(1, "Could not read index!");
		}

		getWord(word, fdDic, index);
		int cmp = strcmp(searchedWord, word);

        if (cmp==0)
		{
			printDef(fdDic);  //we assume that the offset in the dictionary is exactly the end of the searched word
			break;
		}
		else if (cmp<0)
		{
			r = mid-1;
		}
		else
		{
			l = mid+1;
		}
	}

}

int main (int argc, char* argv [])
{
	if (argc != 4)
	{
		errx(1, "Wrong arg count!");
	}

	int fdDic = open(argv[2], O_RDONLY);
	//check
	int fdInd = open(argv[3], O_RDONLY);
	//CHECK

    struct stat info;

    if (fstat(fdInd, &info)<0)
	{
		err(1, "Could not fstat!");
	}

	if (info.st_size%sizeof(uint32_t)!=0)
	{
		errx(1, "Invalid index file!");
	}

	int l = 0;
	int r = info.st_size/sizeof(uint32_t);

	binarySearch(l, r, fdDic, fdInd ,argv[1]);
 }
