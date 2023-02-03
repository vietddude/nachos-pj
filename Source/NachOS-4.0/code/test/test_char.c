#include "syscall.h" 

int main()
{
    char c;
    PrintString("Enter a character: ");
    c = ReadChar();

    PrintString("Your character is: ");
    PrintChar(c);
    PrintChar('\n');

    Halt();
}