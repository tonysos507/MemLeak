// dllhook.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"


void* (__cdecl* Truemalloc)(_In_ _CRT_GUARDOVERFLOW size_t _Size) = malloc;
void(__cdecl* Truefree)(_Pre_maybenull_ _Post_invalid_ void* _Block) = free;


HANDLE plog;

void* __cdecl mymalloc(_In_ _CRT_GUARDOVERFLOW size_t _Size)
{
	DWORD tid = GetCurrentThreadId();
	void* p = NULL;
	p = Truemalloc(_Size);
	
	DWORD writtenlen;
	char loginf[128];
	memset(loginf, 0, 128);
	snprintf(loginf, 128, "%d alloc %d bytes at %p.\r\n", tid, _Size, p);
	WriteFile(plog, loginf, strlen(loginf), &writtenlen, NULL);

	return p;
}


void __cdecl myfree(_Pre_maybenull_ _Post_invalid_ void* _Block)
{
	Truefree(_Block);
	DWORD tid = GetCurrentThreadId();

	DWORD writtenlen;
	char loginf[128];
	memset(loginf, 0, 128);
	snprintf(loginf, 128, "%d dealloc at %p.\r\n", tid, _Block);
	WriteFile(plog, loginf, strlen(loginf), &writtenlen, NULL);
}