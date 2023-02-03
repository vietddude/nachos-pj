#include "syscall.h"

#define MAX_LENGTH 32

int main() {
    char fileName[MAX_LENGTH];

    PrintString("Input file's name: ");
    ReadString(fileName, MAX_LENGTH);
    if(RemoveFile(fileName) != 0) {
        PrintString("Cannot delete file!\n");
        Halt();
    }
    PrintString("Delete complete!\n");

    Halt();
}