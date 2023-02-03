#include "syscall.h"

#define MAX_LENGTH 32

int main() {
    char fileName[MAX_LENGTH];
    int size_file, i = 0;
    char c;
    int fileId;

    PrintString("Enter file's name: ");
    ReadString(fileName, MAX_LENGTH);
    
    // type 2: read only
    // type 3: read and write 
    if ((fileId = OpenFile(fileName, 3)) == -1) {
        PrintString("Cannot open file!\n");
        Halt();
    }
     
    size_file = SeekFile(-1, fileId);
    SeekFile(0, fileId);

    for(; i < size_file; i++)
    {
        ReadFile(&c, 1, fileId);
        PrintChar(c);
    }
    PrintString("\n");

    CloseFile(fileId);

    Halt();
}