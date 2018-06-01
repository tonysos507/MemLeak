// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"

struct MEMINFO
{
	std::vector<std::string> stack;
	size_t size;
};

void GenRpt()
{
	std::ifstream infile("memleak.log");
	std::ofstream outfile("memleak.rpt");
	std::string line;
	std::map<void*, MEMINFO> memtbl;
	int linecount = 0;
	int allocount = 0;
	int deallocount = 0;
	void* currentp = 0;
	MEMINFO currents;
	while (std::getline(infile, line))
	{
		linecount++;
		size_t size;
		void* p;
		if (line.find("alloc ") == 0 && line.find(" bytes at ") != -1)
		{
			allocount++;
			sscanf(line.c_str(), "alloc %d bytes at %p.", &size, &p);
			if (currentp != 0)
			{
				memtbl[p] = currents;
				currents.size = 0;
				currents.stack.clear();
			}
			currentp = p;
			currents.size = size;
		}
		else if (line.find("dealloc at ") == 0)
		{
			deallocount++;
			sscanf(line.c_str(), "dealloc at %p.", &p);
			memtbl.erase(p);
			currentp = 0;
			currents.size = 0;
			currents.stack.clear();
		}
		else if (line.find("dllhook.dll Detoured success") == 0)
			continue;
		else if (line.find("dllhook.dll Removed hook") == 0)
			continue;
		else
		{
			if (currentp != 0)
			{
				DWORD64 offset;
				sscanf(line.c_str(), "%d", &offset);
				CHAR name[1024] = { 0 };
				DWORD offsetFromline;
				IMAGEHLP_LINE line = { 0 };
				line.SizeOfStruct = sizeof(line);
				IMAGEHLP_MODULE modinfo = { 0 };
				modinfo.SizeOfStruct = sizeof(IMAGEHLP_MODULE);
				DWORD offsetFromSymbol;
				CHAR pSym[sizeof(IMAGEHLP_SYMBOL) + 1024] = { 0 };
				((IMAGEHLP_SYMBOL*)pSym)->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
				((IMAGEHLP_SYMBOL*)pSym)->MaxNameLength = 1024;
				SymGetLineFromAddr(hProc, offset, &offsetFromline, &line);
				SymGetSymFromAddr(hProc, offset, &offsetFromSymbol, (IMAGEHLP_SYMBOL*)pSym);
				SymGetModuleInfo(hProc, offset, &modinfo);
				snprintf(name, 1024, "%s!%s(%d):%s", modinfo.LoadedImageName, line.FileName, line.LineNumber, ((IMAGEHLP_SYMBOL*)pSym)->Name);

				currents.stack.push_back(name);
			}
		}
	}

	outfile << "process " << linecount << " lines allocate " << allocount << " times deallocate " << deallocount << " times " << std::endl;
	for (std::map<void*, MEMINFO>::iterator it = memtbl.begin(); it != memtbl.end(); it++)
	{
		outfile << "left " << it->second.size << " bytes at " << it->first << std::endl;
		for (std::vector<std::string>::iterator its = it->second.stack.begin(); its != it->second.stack.end(); its++)
		{
			outfile << *its << std::endl;
		}
	}
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	LONG error;
	(void)hModule;
	(void)lpReserved;

	if (DetourIsHelperProcess())
	{
		return TRUE;
	}

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		hProc = GetCurrentProcess();
		SymInitialize(hProc, NULL, TRUE);
		plog = CreateFile(L"memleak.log", FILE_APPEND_DATA,
			FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (plog != INVALID_HANDLE_VALUE)
		{
			DWORD dwPos = SetFilePointer(plog, 0, NULL, FILE_END);
		}
		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)Truemalloc, mymalloc);
		DetourAttach(&(PVOID&)Truefree, myfree);
		error = DetourTransactionCommit();
		if (error == NO_ERROR)
		{
			if (plog != INVALID_HANDLE_VALUE)
			{
				DWORD writtenlen;
				char loginf[128];
				memset(loginf, 0, 128);
				snprintf(loginf, 128, "dllhook.dll Detoured success.\r\n");
				WriteFile(plog, loginf, strlen(loginf), &writtenlen, NULL);
			}
		}
		else
		{
			if (plog != INVALID_HANDLE_VALUE)
			{
				DWORD writtenlen;
				char loginf[128];
				memset(loginf, 0, 128);
				snprintf(loginf, 128, "dllhook.dll Detoured failed.\r\n");
				WriteFile(plog, loginf, strlen(loginf), &writtenlen, NULL);
			}
		}
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&(PVOID&)Truemalloc, mymalloc);
		DetourDetach(&(PVOID&)Truefree, myfree);
		error = DetourTransactionCommit();
		if (plog != INVALID_HANDLE_VALUE)
		{
			DWORD writtenlen;
			char loginf[128];
			memset(loginf, 0, 128);
			snprintf(loginf, 128, "dllhook.dll Removed hook.\r\n");
			WriteFile(plog, loginf, strlen(loginf), &writtenlen, NULL);
			CloseHandle(plog);
		}
		GenRpt();
		break;
	}
	return TRUE;
}

extern "C" __declspec(dllexport) void dummy(void) {
	return;
}