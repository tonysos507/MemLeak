// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�: 
#include <windows.h>



// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�


#include <stdio.h>
#include "stdlib.h"
#include "detours.h"

extern void* (__cdecl* Truemalloc)(_In_ _CRT_GUARDOVERFLOW size_t _Size);
extern void(__cdecl* Truefree)(_Pre_maybenull_ _Post_invalid_ void* _Block);
void* __cdecl mymalloc(_In_ _CRT_GUARDOVERFLOW size_t _Size);
void __cdecl myfree(_Pre_maybenull_ _Post_invalid_ void* _Block);

extern HANDLE plog;