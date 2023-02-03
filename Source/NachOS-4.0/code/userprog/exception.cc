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
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
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
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions
//	is in machine.h.
//----------------------------------------------------------------------

#define FILE_MAX_NAME 32


void IncreaseProgramCounter()
{
	/* set previous programm counter (debugging only)*/
	kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

	/* set next programm counter for brach execution */
	kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
}

void ExceptionHandler(ExceptionType which)
{
	int type = kernel->machine->ReadRegister(2);

	DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

	switch (which)
	{
	/* do nothing*/
	case NoException:
		return;

	case PageFaultException:
		cerr << "Page fault exception\n";
		SysHalt();
		break;

	case ReadOnlyException:
		cerr << "Read only exception\n";
		SysHalt();
		break;

	case BusErrorException:
		cerr << "Bus error exception\n";
		SysHalt();
		break;

	case AddressErrorException:
		cerr << "Address error exception\n";
		SysHalt();
		break;

	case OverflowException:
		cerr << "Overflow exception\n";
		SysHalt();
		break;

	case IllegalInstrException:
		cerr << "Illegal instruction exception\n";
		SysHalt();
		break;

	case NumExceptionTypes:
		cerr << "Number exception\n";
		SysHalt();
		break;

	case SyscallException:
		switch (type)
		{
		case SC_Halt:
		{
			DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

			SysHalt();
			ASSERTNOTREACHED();
			break;
		}

		case SC_ReadNum:
		{
			int result;
			result = SysReadNum();

			DEBUG(dbgSys, "ReadNum returning with " << result << "\n");
			kernel->machine->WriteRegister(2, (int)result);

			IncreaseProgramCounter();
			return;
			ASSERTNOTREACHED();
			break;
		}

		case SC_PrintNum:
		{
			DEBUG(dbgSys, "PrintNum: " << kernel->machine->ReadRegister(4) << "\n");
			SysPrintNum((int)kernel->machine->ReadRegister(4));

			IncreaseProgramCounter();
			return;
			ASSERTNOTREACHED();
			break;
		}

		case SC_ReadChar:
		{
			char result;
			result = SysReadChar();

			DEBUG(dbgSys, "ReadChar returning with " << result << "\n");
			kernel->machine->WriteRegister(2, (int)result);

			IncreaseProgramCounter();
			return;
			ASSERTNOTREACHED();
			break;
		}

		case SC_PrintChar:
		{
			char c = (char)kernel->machine->ReadRegister(4);
			DEBUG(dbgSys, "PrintChar: " << c << "\n");
			SysPrintChar(c);

			IncreaseProgramCounter();
			return;
			ASSERTNOTREACHED();
			break;
		}

		case SC_RandomNum:
		{
			int result;
			result = SysRandomNum();
			kernel->machine->WriteRegister(2, (int)result);
			DEBUG(dbgSys, "RandomNum returning with " << result << "\n");

			IncreaseProgramCounter();
			return;
			ASSERTNOTREACHED();
			break;
		}

		case SC_ReadString:
		{
			DEBUG(dbgSys, "ReadString called!\n");
			char *virtAddr = (char *)kernel->machine->ReadRegister(4); // address of string
			int length = kernel->machine->ReadRegister(5);			   // length of string
			int result = SysReadString(virtAddr, length);
			DEBUG(dbgSys, "ReadString returning with " << result << "\n");
			IncreaseProgramCounter();
			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_PrintString:
		{
			DEBUG(dbgSys, "PrintString at address: " << kernel->machine->ReadRegister(4));
			SysPrintString((int)kernel->machine->ReadRegister(4));

			IncreaseProgramCounter();
			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_Add:
		{
			DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");

			/* Process SysAdd Systemcall*/
			int result;
			result = SysAdd(/* int op1 */ (int)kernel->machine->ReadRegister(4),
							/* int op2 */ (int)kernel->machine->ReadRegister(5));

			DEBUG(dbgSys, "Add returning with " << result << "\n");
			/* Prepare Result */
			kernel->machine->WriteRegister(2, (int)result);

			IncreaseProgramCounter();
			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_CreateFile:
		{
			int result;
			int virtAddr = (int)kernel->machine->ReadRegister(4);
			char *fileName = User2System(virtAddr, FILE_MAX_NAME + 1);

			DEBUG(dbgSys, "Create file " << fileName << "\n");
			result = SysCreateFile(fileName);
			DEBUG(dbgSys, "Create file returning with " << result << "\n");
			kernel->machine->WriteRegister(2, result);

			IncreaseProgramCounter();
			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_OpenFile:
		{
			int virtAddr = (int)kernel->machine->ReadRegister(4);
			int type = (int)kernel->machine->ReadRegister(5);
			int result;

			DEBUG(dbgSys, "Open file at: " << virtAddr << "\n");
			result = SysOpenFile(virtAddr, type);
			DEBUG(dbgSys, "Open file returning with " << result << "\n");
			kernel->machine->WriteRegister(2, result);

			IncreaseProgramCounter();
			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_CloseFile:
		{
			int virtAddr = kernel->machine->ReadRegister(4);
			DEBUG(dbgSys, "Close file at: " << virtAddr << "\n");
			int result = SysCloseFile(virtAddr);
			DEBUG(dbgSys, "Close file returning with " << result << "\n");
			kernel->machine->WriteRegister(2, result);

			IncreaseProgramCounter();
			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_ReadFile:
		{
			int bufAddr = kernel->machine->ReadRegister(4);
			int size = kernel->machine->ReadRegister(5);
			int id = kernel->machine->ReadRegister(6);

			DEBUG(dbgSys, "Read file at: " << bufAddr << "\n");
			int result = SysReadFile(bufAddr, size, id);
			DEBUG(dbgSys, "Read file returning with " << result << "\n");
			kernel->machine->WriteRegister(2, result);

			IncreaseProgramCounter();
			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_WriteFile:
		{
			int bufAddr = kernel->machine->ReadRegister(4);
			int size = kernel->machine->ReadRegister(5);
			int id = kernel->machine->ReadRegister(6);

			DEBUG(dbgSys, "Write file at: " << bufAddr << "\n");
			int result = SysWriteFile(bufAddr, size, id);
			DEBUG(dbgSys, "Write file returning with " << result << "\n");
			kernel->machine->WriteRegister(2, result);

			IncreaseProgramCounter();
			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_SeekFile:
		{
			int pos = kernel->machine->ReadRegister(4);
			int m_index = kernel->machine->ReadRegister(5);
			DEBUG(dbgSys, "Seek at " << pos << "\n");
			int result = SysSeekFile(pos, m_index);
			DEBUG(dbgSys, "Seek returning with " << result << "\n");
			kernel->machine->WriteRegister(2, result);

			IncreaseProgramCounter();
			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_RemoveFile:
		{
			int buffAddr = kernel->machine->ReadRegister(4);
			char *fileName = User2System(buffAddr, FILE_MAX_NAME + 1);
			DEBUG(dbgSys, "Remove file: " << fileName << "\n");
			int result = SysRemoveFile(fileName);
			DEBUG(dbgSys, "Remove file returning with " << result << "\n");

			IncreaseProgramCounter();
			return;
			ASSERTNOTREACHED();
			break;
		}
		default:
			cerr << "Unexpected system call " << type << "\n";
			break;
		}
		break;
	default:
		cerr << "Unexpected user mode exception" << (int)which << "\n";
		break;
	}
	ASSERTNOTREACHED();
}
