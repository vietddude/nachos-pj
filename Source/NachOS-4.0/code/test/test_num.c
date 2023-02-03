#include "syscall.h"

int main() {
    int num;
    PrintString("Enter a number: ");
    num = ReadNum();
    PrintString("Your number is: ");
    PrintNum(num);
    PrintString("\nGenerated random number: ");
    PrintNum(RandomNum());
    PrintChar('\n');

    Halt();
    /* Not reached */
}