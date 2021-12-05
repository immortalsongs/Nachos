// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#define MAX_LENGTH 255
// To not show error
//#include "synchcons.h"
//SynchConsole *gSynchConsole=new SynchConsole();

#include "machine.h"
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// And don't forget to incremenSt the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions
//	are in machine.h.
//----------------------------------------------------------------------

void incPC()
{
	int temp;
	temp=machine->ReadRegister(PCReg);
	machine->WriteRegister(PrevPCReg, temp);
	temp = machine->ReadRegister(NextPCReg);
	machine->WriteRegister(PCReg, temp);
	machine->WriteRegister(NextPCReg, temp + 4);
}

char* User2System(int virtAddr, int limit)
{
	int i; //chi so index
	int oneChar;
	char* kernelBuf = NULL;
	kernelBuf = new char[limit + 1]; //can cho chuoi terminal
	if (kernelBuf == NULL)
		return kernelBuf;
	
	memset(kernelBuf, 0, limit + 1);
	
	for (i = 0; i < limit; i++)
	{
		machine->ReadMem(virtAddr + i, 1, &oneChar);
		kernelBuf[i] = (char)oneChar;
		if (oneChar == 0)
		break;
	}
	return kernelBuf;
}

int System2User(int virtAddr, int len, char* buffer)
{
	if (len < 0) return -1;
	if (len == 0)return len;
	int i = 0;
	int oneChar = 0;
	do{
		oneChar = (int)buffer[i];
		machine->WriteMem(virtAddr + i, 1, oneChar);
		i++;
	} 
	while (i < len && oneChar != 0);
	return i;
}

void ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);
    switch (which)
    {
	case NoException:
		return;
	
	case PageFaultException:
		DEBUG('a', "\n No valid translation found");
		printf("\n\n No valid translation found");
		interrupt->Halt();
		break;
	
	case ReadOnlyException:
		DEBUG('a', "\n Write attempted to page marked read-only");
		printf("\n\n Write attempted to page marked read-only");
		interrupt->Halt();
		break;
	
	case BusErrorException:
		DEBUG('a', "\n Translation resulted invalid physical address");
		printf("\n\n Translation resulted invalid physical address");
		interrupt->Halt();
		break;
	case AddressErrorException:
		DEBUG('a', "\n Unaligned reference or one that was beyond the end of the address space");
		printf("\n\n Unaligned reference or one that was beyond the end of the address space");
		interrupt->Halt();
		break;
	
	case OverflowException:
		DEBUG('a', "\nInteger overflow in add or sub.");
		printf("\n\n Integer overflow in add or sub.");
		interrupt->Halt();
		break;
	
	case IllegalInstrException:
		DEBUG('a', "\n Unimplemented or reserved instr.");
		printf("\n\n Unimplemented or reserved instr.");
		interrupt->Halt();
		break;
	
	case NumExceptionTypes:
		DEBUG('a', "\n Number exception types");
		printf("\n\n Number exception types");
		interrupt->Halt();
		break;
    case SyscallException:
    {
        SynchConsole *gSynchConsole = new SynchConsole();
        switch (type)
        {
        case SC_Halt:
        {
            DEBUG('a', "\nShutdown, initiated by user program. ");
		printf("\nShutdown, initiated by user program. ");
		interrupt->Halt();
			return;
        }
        case SC_ReadChar:
        {
            char c;
            gSynchConsole->Read(&c,1);
            machine->WriteRegister(2, c);
            break;
        }

        case SC_PrintChar:
        {
            char c = (char)machine->ReadRegister(4);
            gSynchConsole->Write(&c, 1);
            break;
        }

        case SC_ReadInt:
        {
            
            char c;
        int kq = 0;
        	int numLen = 0; //độ dài chuỗi
        bool isNegative = false; // xác định có phải số âm hay không?
        bool is_Over32bit = false; // kiểm tra xem số đó có vượt quá 32 bit hay không?
		
        while (1)
        {
			gSynchConsole->Read(&c, 1);
			//Check số âm:
			if (numLen == 0 && c == '-')
			{
				isNegative = true;
				numLen++;
				continue;
			}
            if ((c == ' ') || (c == '\0'))
            {
                break;
            }

			//Check số
			if ((c < '0') || (c > '9'))
			{
				kq = 0;
				break;
			}
			else
			{
				if (!is_Over32bit)
				{
					int n = (int)(c - 48);
					int temp = kq * 10 + n;
					if ((temp <= kq) && (kq != 0))
					{
						is_Over32bit = true;
					}
					else
					{
						kq = temp;
					}

				}
			}
            numLen++;
        }
		if (is_Over32bit)
		{
			if (isNegative)
			{
				kq = 0x80000000;
			}
			else
			{
				kq = 0x7FFFFFFF;
			}
		}
		else
		{
			if (isNegative)
			{
				kq = -kq;
			}
		}
        machine->WriteRegister(2, kq);
		break;
        }

        case SC_PrintInt:
        {
            int num = machine->ReadRegister(4);
    char* str = new char[11]; // Dãy chữ số của num
    char neg = '-';
    if (num < 0)
    {
        gSynchConsole->Write(&neg, 1);
        num = -num;
    }

    int temp = num;
    int i = 0;
    while (temp > 0)
    {
        str[i] = (temp % 10) + 48;
        temp = temp / 10;
        i++;
    }
    for (int j = 0; j < i/2; j++)
    {
        char temp2;
        temp2 = str[j];
        str[j] = str[i - j-1];
        str[i - j-1] = temp2;
    }
    for (int j = 0; j < i; j++)
    {
        gSynchConsole->Write(str+j, 1);
    }
    delete[] str;
    break;
        }
        
        default:
        {
            printf("Unexpected user mode exception %d %d\n", which, type);
            interrupt->Halt();
            break;
        }
        }
        delete gSynchConsole;
        incPC();
        break;
    }
    
    }
}
