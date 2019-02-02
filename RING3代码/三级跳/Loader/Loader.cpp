#include <windows.h>
#include <stdio.h>

BOOL WINAPI DllMain(HANDLE, DWORD, LPVOID);
LRESULT CALLBACK GetMsgProc(int code, WPARAM wParam, LPARAM lParam);
void SetHook(HWND);
void Unhook();
HHOOK hHook;
HANDLE hInst;

#pragma data_seg(".shared")
HWND s_hWnd = NULL;
#pragma data_seg()

BOOL WINAPI DllMain(HANDLE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	hInst = hInstance;

	return TRUE;
}

void SetHook(HWND hWnd)
{
	s_hWnd = hWnd;
	hHook = NULL;

	hHook = SetWindowsHookEx(WH_GETMESSAGE, GetMsgProc, (HINSTANCE)hInst, 0);
	if (hHook == NULL)
	{
		MessageBox(NULL, "SetWindowsHookEx error", "error", MB_OK);
	}
}

void Unhook()
{
	UnhookWindowsHookEx(hHook);
}

LRESULT CALLBACK GetMsgProc(int code, WPARAM wParam, LPARAM lParam)
{
	CallNextHookEx(hHook, code, wParam, lParam);

	static bool old = FALSE;

	char szBuffer[MAX_PATH];
	memset(szBuffer, 0, sizeof(szBuffer));

	GetModuleFileName(NULL, szBuffer, MAX_PATH);

	char *name = _strupr(_strdup("notepad.exe"));
	char *now = _strupr(_strdup(szBuffer));

	if (strstr(now, name) && !old)
	{
		old = TRUE;
		HMODULE hDll = NULL;
		hDll = LoadLibrary("Inject.dll");
		typedef void (*RUN)(HWND);
		RUN Run = (RUN)GetProcAddress(hDll, "Run");
		Run(s_hWnd);
	}
	return 0;
}