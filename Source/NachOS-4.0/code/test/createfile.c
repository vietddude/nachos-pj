#include "syscall.h"

#define MAX_LENGTH 32

int main()
{
    char fileName[MAX_LENGTH];
    int length;

    PrintString("Enter file's name: ");
    ReadString(fileName, MAX_LENGTH);

    if (CreateFile(fileName) == 0) {
        PrintString("Create file successfully!\n");
    }
    else PrintString("Cannot create file!");

    Halt();
}  