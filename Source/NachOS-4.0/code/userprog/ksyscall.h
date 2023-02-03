/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__
#define __USERPROG_KSYSCALL_H__

#define __STDC_LIMIT_MACROS

#include <stdint.h>
#include "kernel.h"
#include "synchconsole.h"

#define BUFFER_MAX_LENGTH 255

/*
Input: - User space address (int)
 - Limit of buffer (int)
Output:- Buffer (char*)
Purpose: Copy buffer from User memory space to System memory space
*/
char *User2System(int virtAddr, int limit)
{
	int i; // index
	int oneChar;
	char *kernelBuf = NULL;

	kernelBuf = new char[limit + 1]; // need for terminal string
	if (kernelBuf == NULL)
		return kernelBuf;
	memset(kernelBuf, 0, limit + 1);

	// printf("\n Filename u2s:");
	for (i = 0; i < limit; i++)
	{
		kernel->machine->ReadMem(virtAddr + i, 1, &oneChar);
		kernelBuf[i] = (char)oneChar;
		// printf("%c",kernelBuf[i]);
		if (oneChar == 0)
			break;
	}
	return kernelBuf;
}

/*
Input: - User space address (int)
 - Limit of buffer (int)
 - Buffer (char[])
Output:- Number of bytes copied (int)
Purpose: Copy buffer from System memory space to User memory space
*/
int System2User(int virtAdd, int len, char *buffer)
{
	if (len < 0)
		return -1;
	if (len == 0)
		return len;

	int i = 0;
	int oneChar = 0;

	do
	{
		oneChar = (int)buffer[i];
		kernel->machine->WriteMem(virtAdd + i, 1, oneChar);
		i++;
	} while (i < len && oneChar != 0);

	return i;
}

void SysHalt()
{
	kernel->interrupt->Halt();
}

int SysAdd(int op1, int op2)
{
	return op1 + op2;
}

// Read an integer from console
int SysReadNum()
{
	/* int = [-2147483648 , 2147483647] -> max length = 11 */
	const int MAX_LENGTH = 11;
	char numString[MAX_LENGTH] = {0};
	long int result = 0;

	// get array of number in char type
	for (int i = 0; i < MAX_LENGTH; i++)
	{
		char c = 0;
		c = kernel->synchConsoleIn->GetChar();

		if (i == 0 && c == '-')
			numString[0] = c;
		else if (c > '0' && c < '9')
			numString[i] = c;
		else
			break;
	}
	if (strcmp(numString, "-2147483648") == 0)
		return INT32_MIN;
	else if (strcmp(numString, "2147483647") == 0)
		return INT32_MAX;


	// check if first char is "-", if true set pos=1
	int pos = (numString[0] == '-') ? 1 : 0;

	// convert char array to int
	while (pos < MAX_LENGTH && numString[pos] >= '0' && numString[pos] <= '9')
		result = result * 10 + numString[pos++] - '0';

	// add negative if exist
	result = (numString[0] == '-') ? (result * -1) : result;
	
	return result;
}

// Print an integer to console
void SysPrintNum(int number)
{
	/*int: [-2147483648 , 2147483647] --> max length = 11*/
	const int MAX_LENGTH = 11;
	char numString[MAX_LENGTH] = {0};
	int reverseString[MAX_LENGTH] = {0}, pos = 0, rev_pos = 0;

	// check if negative
	if (number < 0)
	{
		number = -number;
		numString[pos++] = '-';
	}

	// convert to int array but reversed
	do
	{
		reverseString[rev_pos++] = number % 10;
		number /= 10;
	} while (number);

	// convert again to char array in right order
	while (rev_pos)
		numString[pos++] = '0' + (char)reverseString[--rev_pos];

	// print each character out
	for (int i = 0; i < pos; i++)
		kernel->synchConsoleOut->PutChar(numString[i]);
}

// Read a character from keyboard
char SysReadChar()
{
	return kernel->synchConsoleIn->GetChar();
}

void SysPrintChar(char character)
{
	kernel->synchConsoleOut->PutChar(character);
}

int SysRandomNum()
{
	RandomInit((unsigned int)time(NULL));
	return RandomNumber();
}

// Read a string from console and store
int SysReadString(char *virtAddr, int length)
{
	if (length < 0)
		return -1;

	char *buffer = new char[length + 1];
	if (buffer == NULL)
		return -1;

	// get char from console and put into buffer
	int i = -1;
	while (i < length)
	{
		char c = kernel->synchConsoleIn->GetChar();
		if (c != '\n')
			buffer[++i] = c;
		else
			break;
	}
	buffer[i + 1] = 0;							// mark end of string
	System2User((int)virtAddr, length, buffer); // copy to user memory
	delete[] buffer;
	return i + 1;
}

int SysPrintString(int virtAddr)
{
	// copy buffer from user memory space to system memory space
	char *sysBuffer = User2System(virtAddr, BUFFER_MAX_LENGTH);

	if (sysBuffer == NULL)
		return -1;

	// print each character in buffer to console
	int index = 0;
	int count = 0;
	while (sysBuffer[index] != 0)
	{
		kernel->synchConsoleOut->PutChar(sysBuffer[index]);
		index++;
		count++;

		// if system buffer is full but the string is not ended
		if (index == BUFFER_MAX_LENGTH)
		{
			// re-allocate system buffer
			delete[] sysBuffer;
			sysBuffer = NULL;

			// continue to copy next buffer
			virtAddr += BUFFER_MAX_LENGTH;
			sysBuffer = User2System(virtAddr, BUFFER_MAX_LENGTH);

			// return if system does not have enough memory
			if (sysBuffer == NULL)
				return count;

			index = 0;
		}
	}
	// clear memory
	delete[] sysBuffer;
	return count;
}

int SysCreateFile(char *buff) {
	if (buff == NULL)
		return -1;
	if (kernel->fileSystem->Create(buff, 0) == false)
		return -1;
	return 0;
}

int SysOpenFile(int bufAddr, int type) {
	if ((type < 0 || type > 3) && kernel->fileSystem->index > 20)
		return -1;

	char *buf = User2System(bufAddr, BUFFER_MAX_LENGTH);
	if (strcmp(buf, "stdin") == 0)
	{
		DEBUG(dbgSys, "stdin mode\n");
		return 0;
	}
	if (strcmp(buf, "stdout") == 0)
	{
		DEBUG(dbgSys, "stdout mode\n");
		return 1;
	}
	if ((kernel->fileSystem->openf[kernel->fileSystem->index] = kernel->fileSystem->Open(buf, type)) != NULL)
	{
		DEBUG(dbgSys, "Open file successfully\n");
		delete[] buf;
		return kernel->fileSystem->index-1;
	}
	else
	{
		DEBUG(dbgSys, "Can not open file\n");
		delete[] buf;
		return -1;
	};
}

int SysCloseFile(int m_index) {
	if (kernel->fileSystem->openf[m_index] == NULL)
		return -1;
	delete kernel->fileSystem->openf[m_index];
	kernel->fileSystem->openf[m_index] = NULL;
	return 0;
}

int SysReadFile(int bufAddr, int length, int m_index) {
	int old_pos;
	int new_pos;

	// Check m_index
	if (m_index < 0 || m_index > 20)
		return -1;
	
	// check openf[m_index]
	if (kernel->fileSystem->openf[m_index] == NULL)
		return -1;
	
	old_pos = kernel->fileSystem->openf[m_index]->GetCurrentPos();
	char *buf = User2System(bufAddr, length);
	
	if (kernel->fileSystem->openf[m_index]->type == 1) // stdout
		kernel->synchConsoleOut->PutChar(*buf);

	if ((kernel->fileSystem->openf[m_index]->Read(buf, length)) > 0)
	{
		// Copy data from kernel to user space
		new_pos = kernel->fileSystem->openf[m_index]->GetCurrentPos();
		System2User(bufAddr, new_pos - old_pos + 1, buf);
		return new_pos - old_pos + 1;
	}
	
	delete[] buf;
	return -1;
}

int SysWriteFile(int bufAddr, int NumBuf, int m_index) {
	int old_pos;
	int new_pos;
	// Check m_index
	if (m_index < 0 || m_index > 20)
		return -1;
	// check openf[m_index]
	if (kernel->fileSystem->openf[m_index] == NULL)
		return -1;
	old_pos = kernel->fileSystem->openf[m_index]->GetCurrentPos();

	// type must equals '0'
	char *buf = User2System(bufAddr, NumBuf);
	if (kernel->fileSystem->openf[m_index]->type == 0 ||
		kernel->fileSystem->openf[m_index]->type == 3)
	{
		if ((kernel->fileSystem->openf[m_index]->Write(buf, NumBuf)) > 0)
		{
			// Copy data from kernel to user space
			new_pos = kernel->fileSystem->openf[m_index]->GetCurrentPos();
			return new_pos - old_pos + 1;
		}
		else if (kernel->fileSystem->openf[m_index]->type == 1)
		{
			delete[] buf;
			return -1;
		}
	}
	// Write data to console
	if (kernel->fileSystem->openf[m_index]->type == 3)
	{
		int i = 0;
		DEBUG(dbgSys, "stdout mode\n");
		while (buf[i] != 0 && buf[i] != '\n')
		{
			kernel->synchConsoleOut->PutChar(*(buf + i));
			i++;
		}
		buf[i] = '\n';
		kernel->synchConsoleOut->PutChar(*(buf + i));
		return i-1;
	}
	delete[] buf;
	return -1;
}

int SysSeekFile(int pos, int m_index) {
	if (m_index < 0 || m_index > 20)
		return -1;
	// check openf[m_index]
	if (kernel->fileSystem->openf[m_index] == NULL)
		return -1;
	pos = (pos == -1) ? kernel->fileSystem->openf[m_index]->Length() : pos;
	if (pos > kernel->fileSystem->openf[m_index]->Length() || pos < 0)
		return -1;
	kernel->fileSystem->openf[m_index]->Seek(pos);
	return pos;
}

int SysRemoveFile(char* fileName) {
	if ((kernel->fileSystem->openf[kernel->fileSystem->index] = kernel->fileSystem->Open(fileName)) != NULL)
	{
		DEBUG(dbgSys, "This file is open!\n");
		int res = SysCloseFile(kernel->fileSystem->index-1);
		if (res == 0) {
			DEBUG(dbgSys, "File is closed!\n");
		}
		else 
		{
			DEBUG(dbgSys, "Cannot close file!\n");
			return -1;
		}
	}

	kernel->fileSystem->Remove(fileName);

	DEBUG(dbgSys, "This file is deleted!\n");
	return 0;
}
#endif /* ! __USERPROG_KSYSCALL_H__ */
