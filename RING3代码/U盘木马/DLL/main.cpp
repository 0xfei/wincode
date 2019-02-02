#include <windows.h>
#include <stdio.h>
#include "dbt.h"

#define DBT_DEVICEARRIVAL 0x8000

static HHOOK hHook = NULL;
HINSTANCE hInstance;
BOOL InstallHook();
void EndHook();
static int count = 0;

BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch(dwReason)
	{
	case DLL_PROCESS_ATTACH:
		hInstance = (HINSTANCE)hModule;
		break;
	default:
		break;
	}
	return TRUE;
}

LRESULT CALLBACK HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION)
	{
		PCWPSTRUCT msg = PCWPSTRUCT(lParam);
		if (msg->message == WM_DEVICECHANGE)
		{
			if ((msg->wParam == DBT_DEVICEARRIVAL)&&(count == 0))
			{
				count ++;
				MessageBox(NULL, "usb", "warn", MB_OK);
			}
		}
	}
	LRESULT ret = CallNextHookEx(hHook, nCode, wParam, lParam);
	return ret;
}

BOOL InstallHook()
{
	hHook = SetWindowsHookEx(WH_CALLWNDPROC, HookProc, hInstance, 0);
	return TRUE;
}

void EndHook()
{
	UnhookWindowsHookEx(hHook);
}
