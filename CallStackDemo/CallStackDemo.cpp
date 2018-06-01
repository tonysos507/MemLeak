// CallStackDemo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"




HANDLE hProc;
std::vector<BACKTRACEINFO> backtracetbl;

BOOL CALLBACK ReadProcMem(HANDLE hProcess, DWORD64 qwBaseAddress, PVOID lpBuffer, DWORD nSize, LPDWORD lpNumberOfBytesRead)
{
	SIZE_T st;
	BOOL bRet = ReadProcessMemory(hProc, (LPVOID)qwBaseAddress, lpBuffer, nSize, &st);
//	printf("ReadMemory: hProcess: %p, baseAddr: %p, buffer: %p, size: %d, read: %d, result: %d\n", hProc, (LPVOID)qwBaseAddress, lpBuffer, nSize, (DWORD)st, (DWORD)bRet);
	*lpNumberOfBytesRead = (DWORD)st;
	if (bRet == 0)
	{
		printf("ReadProcMem - error %d\n", GetLastError());
	}
	return bRet;
}

bool checkAPI()
{
	CONTEXT context;
	memset(&context, 0, sizeof(context));
	context.ContextFlags = CONTEXT_FULL;
	__asm		call x
	__asm x:	pop eax
	__asm		mov context.Eip, eax
	__asm		mov context.Ebp, ebp
	__asm		mov context.Esp, esp

	PVOID stackStart[MAX_STACK_DEPTH];
	DWORD dwHashCode = 0;
	USHORT frames = CaptureStackBackTrace((ULONG)0, (ULONG)10, stackStart, &dwHashCode);
	printf("No of frames found %d, hash code %d\n", frames, dwHashCode);


	HANDLE hThread = GetCurrentThread();
	STACKFRAME64 stackframe;
	memset(&stackframe, 0, sizeof(stackframe));
	stackframe.AddrPC.Offset = context.Eip;
	stackframe.AddrPC.Mode = AddrModeFlat;
	stackframe.AddrStack.Offset = context.Esp;
	stackframe.AddrStack.Mode = AddrModeFlat;
	stackframe.AddrFrame.Offset = context.Ebp;
	stackframe.AddrFrame.Mode = AddrModeFlat;

	for (int frameNum = 0; ; frameNum)
	{
		if (!StackWalk64(IMAGE_FILE_MACHINE_I386, hProc, hThread, &stackframe, &context, NULL, NULL, NULL, NULL))
			break;

		if (stackframe.AddrPC.Offset != 0)
		{
			backtracetbl.push_back({ stackframe });
		}
	}


	return true;
}

int main()
{
	hProc = GetCurrentProcess();
	SymInitialize(hProc, NULL, TRUE);
	checkAPI();

	DWORD offsetFromSymbol;
	IMAGEHLP_SYMBOL *pSym = NULL;
	pSym = (IMAGEHLP_SYMBOL*)malloc(sizeof(IMAGEHLP_SYMBOL) + 1024);
	memset(pSym, 0, sizeof(IMAGEHLP_SYMBOL) + 1024);
	pSym->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
	pSym->MaxNameLength = 1024;

	CHAR name[1024] = { 0 };
	for (std::vector<BACKTRACEINFO>::iterator it = backtracetbl.begin(); it != backtracetbl.end(); it++)
	{
		DWORD offsetFromline;
		IMAGEHLP_LINE line;
		memset(&line, 0, sizeof(line));
		line.SizeOfStruct = sizeof(line);
		IMAGEHLP_MODULE modinfo = { 0 };
		modinfo.SizeOfStruct = sizeof(IMAGEHLP_MODULE);
		if (SymGetLineFromAddr(hProc, it->sf.AddrPC.Offset, &offsetFromline, &line) != FALSE)
		{
		}

		if (SymGetSymFromAddr(hProc, it->sf.AddrPC.Offset, &offsetFromSymbol, pSym))
		{
		}

		if (SymGetModuleInfo(hProc, it->sf.AddrPC.Offset, &modinfo) == FALSE)
			printf("SymGetModuleInfo error %d\n", GetLastError());

		snprintf(name, 1024, "%s!%s(%d):%s", modinfo.LoadedImageName, line.FileName, line.LineNumber, pSym->Name);
		printf("%s\n", name);
		memset(name, 0, 1024);
	}


    return 0;
}

