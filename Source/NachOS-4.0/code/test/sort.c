#include "syscall.h"

int main()
{
    int n;
    int a[100];
    int order; // 1: increasing, 2: decreasing
    int i, j, temp;

    do
    {
        PrintString("Enter size of array (0<n<=100): ");
        n = ReadNum();
        if (n < 0 || n > 100)
            PrintString("Try again!\n");
    } while (n < 0 || n > 100);

    
    for (i = 0; i < n; i++)
    {
        PrintString("Enter a[");
        PrintNum(i);
        PrintString("]: ");
        a[i] = ReadNum();
    }

    do
    {
        PrintString("Choose order to sort (1. Increasing, 2. Decreasing): ");
        order = ReadNum();
        if (order != 1 && order != 2)
            PrintString("Try again!\n");
    } while (order != 2 && order != 1);

    // bubble sort in increasing order
    if (order == 1)
    {
        for (i = 0; i < n; i++)
            for (j = 0; j < n - i - 1; j++)
                if (a[j] > a[j + 1])
                {
                    temp = a[j];
                    a[j] = a[j + 1];
                    a[j + 1] = temp;
                }
    }
    // bubble sort in decreasing order
    else if (order == 2)
    {
        for (i = 0; i < n; i++)
            for (j = 0; j < n - i - 1; j++)
                if (a[j] < a[j + 1])
                {
                    temp = a[j];
                    a[j] = a[j + 1];
                    a[j + 1] = temp;
                }
    }

    PrintString("Sorted array ");
    if (order == 1)
        PrintString("in increasing order: \n");
    else if (order == 2)
        PrintString("in decreasing order: \n");

    for (i = 0; i < n; i++)
    {
        PrintNum(a[i]);
        PrintChar(' ');
    }
    PrintString("\n");

    Halt();
    return 0;
}