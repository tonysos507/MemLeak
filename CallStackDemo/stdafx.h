// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�

#include <Windows.h>
#include <WinBase.h>
#include <DbgHelp.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>

#define MAX_STACK_DEPTH 32

typedef struct _BacktraceInfo
{
	STACKFRAME64 sf;
}BACKTRACEINFO, *PBACKTRACEINFO;