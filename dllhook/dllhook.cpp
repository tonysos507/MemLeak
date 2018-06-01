// dllhook.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"


void* (__cdecl* Truemalloc)(_In_ _CRT_GUARDOVERFLOW size_t _Size) = malloc;
void(__cdecl* Truefree)(_Pre_maybenull_ _Post_invalid_ void* _Block) = free;


HANDLE plog;
HANDLE hProc;

void* __cdecl mymalloc(_In_ _CRT_GUARDOVERFLOW size_t _Size)
{
	void* p = NULL;
	p = Truemalloc(_Size);
	
	DWORD writtenlen;
	char loginf[128] = { 0 };
	snprintf(loginf, 128, "alloc %d bytes at %p.\r\n", _Size, p);
	WriteFile(plog, loginf, strlen(loginf), &writtenlen, NULL);
	CONTEXT context = { 0 };
	context.ContextFlags = CONTEXT_FULL;
	__asm		call x
	__asm x:	pop eax
	__asm		mov context.Eip, eax
	__asm		mov context.Ebp, ebp
	__asm		mov context.Esp, esp
	HANDLE hThread = GetCurrentThread();
	STACKFRAME64 stackframe = { 0 };
	stackframe.AddrPC.Offset = context.Eip;
	stackframe.AddrPC.Mode = AddrModeFlat;
	stackframe.AddrStack.Offset = context.Esp;
	stackframe.AddrStack.Mode = AddrModeFlat;
	stackframe.AddrFrame.Offset = context.Ebp;
	stackframe.AddrFrame.Mode = AddrModeFlat;
	for (int frameNum = 0; frameNum < 9; frameNum++)
	{
		if (!StackWalk64(IMAGE_FILE_MACHINE_I386, hProc, hThread, &stackframe, &context, NULL, NULL, NULL, NULL))
			break;
		if (frameNum == 0)
			continue;
		if (stackframe.AddrPC.Offset != 0)
		{
			CHAR addroffset[128] = { 0 };
			snprintf(addroffset, 128, "%d\r\n", stackframe.AddrPC.Offset);
			WriteFile(plog, addroffset, strlen(addroffset), &writtenlen, NULL);


			//CHAR name[1024] = { 0 };
			//DWORD offsetFromline;
			//IMAGEHLP_LINE line = { 0 };
			//line.SizeOfStruct = sizeof(line);
			//IMAGEHLP_MODULE modinfo = { 0 };
			//modinfo.SizeOfStruct = sizeof(IMAGEHLP_MODULE);
			//DWORD offsetFromSymbol;
			//CHAR pSym[sizeof(IMAGEHLP_SYMBOL) + 1024] = { 0 };
			//((IMAGEHLP_SYMBOL*)pSym)->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
			//((IMAGEHLP_SYMBOL*)pSym)->MaxNameLength = 1024;
			//SymGetLineFromAddr(hProc, stackframe.AddrPC.Offset, &offsetFromline, &line);
			//SymGetSymFromAddr(hProc, stackframe.AddrPC.Offset, &offsetFromSymbol, (IMAGEHLP_SYMBOL*)pSym);
			//SymGetModuleInfo(hProc, stackframe.AddrPC.Offset, &modinfo);
			//snprintf(name, 1024, "%s!%s(%d):%s\r\n", modinfo.LoadedImageName, line.FileName, line.LineNumber, ((IMAGEHLP_SYMBOL*)pSym)->Name);
			//WriteFile(plog, name, strlen(name), &writtenlen, NULL);
		}
	}

	return p;
}


void __cdecl myfree(_Pre_maybenull_ _Post_invalid_ void* _Block)
{
	Truefree(_Block);

	DWORD writtenlen;
	char loginf[128];
	memset(loginf, 0, 128);
	snprintf(loginf, 128, "dealloc at %p.\r\n", _Block);
	WriteFile(plog, loginf, strlen(loginf), &writtenlen, NULL);
}