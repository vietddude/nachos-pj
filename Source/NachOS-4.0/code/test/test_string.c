#include "syscall.h"

#define MAX_LENGTH 255
int main()
{   
    char arr[MAX_LENGTH];
    PrintString("Enter a string: ");
    ReadString(arr, MAX_LENGTH);

    PrintString("Your string is: ");
    PrintString(arr);
    PrintChar('\n');
    
    Halt();
    /* not reached */
}