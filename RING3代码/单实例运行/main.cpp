#include <windows.h>

#pragma data_seg("shared")
int volatile Only = 0;
#pragma data_seg()

#pragma comment(linker, "/section:shared, RWS")

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	HANDLE hObject = CreateMutex(NULL, FALSE, "test");
		if (GetLastError() == ERROR_ALREADY_EXISTS) {
			CloseHandle(hObject);
			MessageBox(NULL, "NO", NULL, MB_OK);
			ExitProcess(1);
		}
	while (1) {
		Sleep(1);
	}
	return 0;
}