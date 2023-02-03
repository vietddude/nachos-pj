#include "syscall.h"

#define MAX_LENGTH 255

int main() {
    int first_id;
	int second_id;
	char first[MAX_LENGTH], second[MAX_LENGTH];
	int first_size, second_size, i;
	char c;
	
	PrintString("Input first file's name: ");
	ReadString(first, MAX_LENGTH);
	PrintString("Input second file's name: ");
	ReadString(second, MAX_LENGTH);

    PrintString("Concatenate from ");
    PrintString(second);
    PrintString(" to ");
    PrintString(first);
    PrintString(" ...\n");

    first_id = OpenFile(first, 3);
    second_id = OpenFile(second, 2);

      if (first_id == -1 || second_id == -1) {
        PrintString("Cannot open file\n");
        Halt();
    }
    first_size = SeekFile(-1, first_id);
    second_size = SeekFile(-1, second_id);
    SeekFile(0, second_id);
    for(i = 0; i < second_size; i++) {
        ReadFile(&c, 1, second_id);
        WriteFile(&c, 1, first_id);
    }

    CloseFile(first_id);
	CloseFile(second_id);

    PrintString("Done\n");
    Halt();
}