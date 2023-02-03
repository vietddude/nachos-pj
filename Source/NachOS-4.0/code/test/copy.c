/* copy program */
#include "syscall.h"
#include "copyright.h"

#define MAX_LENGTH 255

int main(int argc, char* argv[])
{
	int source_id;
	int dest_id;
	char source_file[MAX_LENGTH], dest_file[MAX_LENGTH];
	int source_size, i;
	char c;
	
	PrintString("Input source file's name: ");
	ReadString(source_file, MAX_LENGTH);
	PrintString("Input destination file's name: ");
	ReadString(dest_file, MAX_LENGTH);

    PrintString("Copy content from ");
    PrintString(source_file);
    PrintString(" to ");
    PrintString(dest_file);
    PrintString("...\n");

	source_id = OpenFile(source_file, 1);
	CreateFile(dest_file);
	dest_id = OpenFile(dest_file, 0);
	if (source_id == -1 || dest_id == -1)
	{
		int errorId = source_id == 0 ? 1 : 2;
		PrintString("Can not open file \n");
		PrintString("Terminate program\n");
		Halt();
	}
	source_size = SeekFile(-1, source_id);
	SeekFile(0, source_id);
	SeekFile(0, dest_id);
	for (i = 0; i < source_size; ++i)
	{
		ReadFile(&c, 1, source_id);
		WriteFile(&c, 1, dest_id);
	}
	CloseFile(source_id);
	CloseFile(dest_id);
	Halt();
}
