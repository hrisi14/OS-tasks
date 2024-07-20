#include <errno.h>
#include <err.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>

bool check(uint16_t, uint64_t, uint64_t);
void swapIfNeeded(int, uint16_t, uint32_t, uint32_t, int);
void processComparator(int, int, int);

typedef struct Header
{
	uint32_t magic;
	uint32_t count;
} Header;

typedef struct Comp
{
	uint32_t magic1;  //=0xAFBC7A37
	uint16_t magic2; //=0x1C27

	uint16_t reserved;
	uint64_t count;
}Comp;

typedef struct Data
{
	uint16_t type;
	uint16_t reserved[3];
	uint32_t offset1;
	uint32_t offset2;
}Data;


bool check(uint16_t type, uint64_t num1, uint64_t num2)
{
	if (type==0)
	{
		return !(num1<num2);
	}

	if(type==1)
	{
		return !(num1>num2);
	}

	return false;
}

void swapIfNeeded(int fdData, uint16_t type, uint32_t offset1, uint32_t offset2, int currentPos)
{
    lseek(fdData, currentPos, SEEK_SET);

	int num1Pos;
	int num2Pos;

	if ((num1Pos=lseek(fdData, offset1*sizeof(uint64_t), SEEK_CUR))<0)
	{
		err(1, "Could not offset in data.bin file!");
	}

	uint64_t num1;
	uint64_t num2;

	read(fdData, &num1, sizeof(uint64_t));

	lseek(fdData, currentPos, SEEK_SET);
	num2Pos = lseek(fdData, offset2*sizeof(uint64_t), SEEK_CUR);
	read(fdData, &num2, sizeof(uint64_t));

	if (check(type, num1, num2))
	{
    
    lseek(fdData, num1Pos, SEEK_SET);
	write(fdData, &num2, sizeof(uint64_t));

	lseek(fdData, num2Pos, SEEK_SET);
	write(fdData, &num1, sizeof(uint64_t));
	}
}

void processComparator(int fdData, int fdComp, int currentPos)
{
	Comp comp;
	read(fdComp, &comp, sizeof(Comp));  //Add checks!!!

	for (uint64_t i=0; i<comp.count; i++)
	{
		Data element;
		read(fdComp, &element, sizeof(Data));

		swapIfNeeded(fdData, element.type, element.offset1, element.offset2, currentPos);
	}
}


int main (int argc, char* argv[])
{
	if (argc!=3)
	{
		errx(1, "Wrong arg count!");
	}

    //To do: some checks for files' validation!!!
    
    int fdData = open(argv[1], O_RDWR);
    if(fdData<0) {err(1, "Could not open data.bin- file!"); }

    int fdComp = open(argv[2], O_RDONLY);
    if (fdComp<0) {err(1, "Could not open compartor file!");}

	lseek(fdData, 0, SEEK_SET);
	Header h;
	read(fdData, &h, sizeof(Header));
	int currentPos = lseek(fdData, 0, SEEK_CUR);

    processComparator(fdData, fdComp, currentPos);
    close(fdData);
    close(fdComp);

}
