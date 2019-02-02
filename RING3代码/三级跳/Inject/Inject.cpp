#include <winsock2.h>
#include <stdio.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

BOOL APIENTRY DllMain(HANDLE, DWORD, LPVOID);
DWORD WINAPI ThreadProc(LPVOID);
void Run(HWND hWnd);

HANDLE hThread;
BOOL ret;


BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			ret = FALSE;
			ret = TerminateThread(hThread, 0);
			if (!ret)
			{
				MessageBox(NULL, "TerminateThread error!", "error", MB_OK);
			}
			break;
    }
    return TRUE;
}

void Run(HWND hWnd)
{
	hThread = NULL;
	SendMessage(hWnd, WM_CLOSE, NULL, NULL);

	hThread = CreateThread(NULL, 0, ThreadProc, NULL, 0, NULL);
	if (hThread == NULL)
	{
		MessageBox(NULL, "CreateThread error!", "error", MB_OK);
	}
}

DWORD WINAPI ThreadProc(LPVOID lParam)
{
	ExitProcess(0);
	return 0;
}