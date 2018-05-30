// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"

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
		break;
	}
	return TRUE;
}

extern "C" __declspec(dllexport) void dummy(void) {
	return;
}