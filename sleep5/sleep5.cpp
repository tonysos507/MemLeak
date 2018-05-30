// sleep5.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>

struct MEMINFO
{
	DWORD tid;
	size_t size;
};

int __cdecl main(int argc, char ** argv)
{
	std::ifstream infile(argv[1]);
	std::string line;
	std::map<void*, MEMINFO> memtbl;
	int linecount = 0;
	while (std::getline(infile, line))
	{
		linecount++;
		DWORD tid;
		size_t size;
		void* p;
		if (line.find("alloc") != -1 && line.find("bytes") != -1)
		{
			sscanf(line.c_str(), "%d alloc %d bytes at %p.", &tid, &size, &p);
			memtbl[p] = { tid, size };
		}
		else if (line.find("dealloc") != -1)
		{
			sscanf(line.c_str(),  "%d dealloc at %p.", &tid, &p);
			memtbl.erase(p);
		}
	}

	std::cout << "process " << linecount << " lines" << std::endl;
	for (std::map<void*, MEMINFO>::iterator it = memtbl.begin(); it != memtbl.end(); it++)
	{
		std::cout << "thread " << it->second.tid << " left " << it->second.size << " bytes at " << it->first << std::endl;
	}

	return 0;
}