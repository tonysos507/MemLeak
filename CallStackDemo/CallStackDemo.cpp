// CallStackDemo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <WinBase.h>
#include <DbgHelp.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_STACK_DEPTH 32

HANDLE hProc;

BOOL CALLBACK ReadProcMem(HANDLE hProcess, DWORD64 qwBaseAddress, PVOID lpBuffer, DWORD nSize, LPDWORD lpNumberOfBytesRead)
{
	SIZE_T st;
	BOOL bRet = ReadProcessMemory(hProc, (LPVOID)qwBaseAddress, lpBuffer, nSize, &st);
//	printf("ReadMemory: hProcess: %p, baseAddr: %p, buffer: %p, size: %d, read: %d, result: %d\n", hProc, (LPVOID)qwBaseAddress, lpBuffer, nSize, (DWORD)st, (DWORD)bRet);
	*lpNumberOfBytesRead = (DWORD)st;
	if (bRet == 0)
	{
		printf("error %d\n", GetLastError());
	}
	return bRet;
}

bool checkAPI()
{
	PVOID stackStart[MAX_STACK_DEPTH];
	DWORD dwHashCode = 0;
	USHORT frames = CaptureStackBackTrace((ULONG)0, (ULONG)10, stackStart, &dwHashCode);
	printf("No of frames found %d, hash code %d\n", frames, dwHashCode);

	hProc = GetCurrentProcess();
	SymInitialize(hProc, NULL, TRUE);
	HANDLE hThread = GetCurrentThread();
	STACKFRAME64 stackframe;
	CONTEXT context;
	memset(&context, 0, sizeof(context));
	context.ContextFlags = CONTEXT_FULL;
	__asm		call x
	__asm x:	pop eax
	__asm		mov context.Eip, eax
	__asm		mov context.Ebp, ebp
	__asm		mov context.Esp, esp

	memset(&stackframe, 0, sizeof(stackframe));
	stackframe.AddrPC.Offset = context.Eip;
	stackframe.AddrPC.Mode = AddrModeFlat;
	stackframe.AddrStack.Offset = context.Esp;
	stackframe.AddrStack.Mode = AddrModeFlat;
	stackframe.AddrFrame.Offset = context.Ebp;
	stackframe.AddrFrame.Mode = AddrModeFlat;

	IMAGEHLP_SYMBOL *pSym = NULL;
	pSym = (IMAGEHLP_SYMBOL*)malloc(sizeof(IMAGEHLP_SYMBOL) + 1024);
	memset(pSym, 0, sizeof(IMAGEHLP_SYMBOL) + 1024);
	pSym->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
	pSym->MaxNameLength = 1024;
	DWORD offsetFromSymbol;
	CHAR name[1024];

	for (int frameNum = 0; ; frameNum)
	{
		if (!StackWalk64(IMAGE_FILE_MACHINE_I386, hProc, hThread, &stackframe, &context, ReadProcMem, SymFunctionTableAccess64, SymGetModuleBase64, NULL))
			break;

		if (stackframe.AddrPC.Offset != 0)
		{
			if (SymGetSymFromAddr(hProc, stackframe.AddrPC.Offset, &offsetFromSymbol, pSym))
			{
				strncpy_s(name, 1024, pSym->Name, _TRUNCATE);
				name[1024 - 1] = 0;
				printf("%s\n", name);
			}
			else
				printf("error %d\n", GetLastError());
		}
	}


	return true;
}

int main()
{
	checkAPI();
    return 0;
}

