#include "syscall.h"
#include "copyright.h"

#define M 100

int main()
{

    int A[M];
    int n,i,j, temp, choice;

    do
    {
        PrintString("Nhap n (n <= 100): ");
        n = ReadInt();
        if (n > 100)
        {
            PrintString("Vui long nhap lai \n");
        }
    } while (n < 1 || n>100);

    PrintString("Nhap mang A: \n");

    for (i = 0; i < n; i++)
    {
        A[i] = ReadInt();
    }
    

    while (1)
    {
	PrintString("\n");
    PrintString("1 - Sap xep tang dan. \n");
    PrintString("2 - Sap xep giam dan. \n");
    PrintString("3 - Mang sau khi sap xep. \n");
    PrintString("Nhap bat ky de thoat. \n");
    PrintString("------------------------------------- \n");
        PrintString("=> Nhap lua chon: ");
        choice = ReadInt();

        if (choice == 1)
        {
            for (i = 0; i < n - 1; i++)
            {
                for (j = i + 1; j < n; j++)
                {
                    if (A[j] < A[i])
                    {
                        temp = A[j];
                        A[j] = A[i];
                        A[i] = temp;
                    }
                }
            }
        }

        else if (choice == 2)
        {
            for (i = 0; i < n - 1; i++)
            {
                for (j = i + 1; j < n; j++)
                {
                    if (A[j] > A[i])
                    {
                        temp = A[j];
                        A[j] = A[i];
                        A[i] = temp;
                    }
                }
            }
        }
        else if (choice == 3)
        {
            PrintString("\n");
            PrintString("Mang A (da sap xep): ");
            for (i = 0; i < n; i++)
            {
                PrintInt(A[i]);
                PrintChar(' ');
            }
            PrintString("\n");
        }
        else
        {
            break;
        }

    }
    return 0;
}
