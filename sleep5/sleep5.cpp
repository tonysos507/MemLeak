// sleep5.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <windows.h>
#include <DbgHelp.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

struct MEMINFO
{
	std::vector<std::string> stack;
	size_t size;
};

int __cdecl main(int argc, char ** argv)
{
	std::ifstream infile(argv[1]);
	std::string line;
	std::map<void*, MEMINFO> memtbl;
	int linecount = 0;
	int allocount = 0;
	int deallocount = 0;
	void* currentp = 0;
	MEMINFO currents;
	HANDLE hProc = GetCurrentProcess();
	SymInitialize(hProc, NULL, TRUE);
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
				//DWORD64 offset;
				//sscanf(line.c_str(), "%d", &offset);
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
				//SymGetLineFromAddr(hProc, offset, &offsetFromline, &line);
				//SymGetSymFromAddr(hProc, offset, &offsetFromSymbol, (IMAGEHLP_SYMBOL*)pSym);
				//SymGetModuleInfo(hProc, offset, &modinfo);
				//snprintf(name, 1024, "%s!%s(%d):%s\r\n", modinfo.LoadedImageName, line.FileName, line.LineNumber, ((IMAGEHLP_SYMBOL*)pSym)->Name);


				currents.stack.push_back(line);
			}
		}
	}

	std::cout << "process " << linecount << " lines allocate " << allocount << " times deallocate " << deallocount  << " times " << std::endl;
	for (std::map<void*, MEMINFO>::iterator it = memtbl.begin(); it != memtbl.end(); it++)
	{
		std::cout << "left " << it->second.size << " bytes at " << it->first << std::endl;
		for (std::vector<std::string>::iterator its = it->second.stack.begin(); its != it->second.stack.end(); its++)
		{
			std::cout << *its << std::endl;
		}
	}

	return 0;
}