#include <windows.h>

__declspec(dllexport) void manager()
{
	MessageBox(NULL, "TEST","TEST",MB_OK);
}

BOOL APIENTRY DllMain(HANDLE hModule, DWORD dwReason, LPVOID lParam)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		{
			MessageBox(NULL, "PROCESS_ATTACH!","TEST",MB_OK);
		}
	default:
		break;
	}
	return TRUE;
}
