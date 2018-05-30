// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件: 
#include <windows.h>



// TODO:  在此处引用程序需要的其他头文件


#include <stdio.h>
#include "stdlib.h"
#include "detours.h"

extern void* (__cdecl* Truemalloc)(_In_ _CRT_GUARDOVERFLOW size_t _Size);
extern void(__cdecl* Truefree)(_Pre_maybenull_ _Post_invalid_ void* _Block);
void* __cdecl mymalloc(_In_ _CRT_GUARDOVERFLOW size_t _Size);
void __cdecl myfree(_Pre_maybenull_ _Post_invalid_ void* _Block);

extern HANDLE plog;