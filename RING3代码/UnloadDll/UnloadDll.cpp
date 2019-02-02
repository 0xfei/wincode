#include <windows.h>
#include <Tlhelp32.h>
#include <string.h>
#include <stdio.h>

VOID EnableDebug()
{
	HANDLE hToken;
	LUID uID;
	TOKEN_PRIVILEGES tp;
	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken);
	LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &uID);
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = uID;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
	CloseHandle(hToken);
}

int main()
{
	char process[MAX_PATH] = {0};
	char dll[MAX_PATH] = {0};
	int processID = 0;
	HANDLE hSnap, Process, hModule;
	BOOL hProcess;
	PROCESSENTRY32 pe;
	MODULEENTRY32 mo;

	printf("Enter the process name or the process id:  ");
	scanf("%s", process);
	if (strstr(process, ".exe"))
	{
		//
	}
	else if (process[0] >= '1' && process[0] <= '9')
	{
		processID = atoi(process);
	}
	else
	{
		printf("Error!!!\n");
		exit(0);
	}

	EnableDebug();

	hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap == NULL)
	{
		printf("CreateToolhelp32Snapshot error!!\n");
		exit(0);
	}
	pe.dwSize = sizeof(pe);

	hProcess = Process32First(hSnap, &pe);
	while (hProcess)
	{
		//printf("PID: %d, name: %s\n", pe.th32ProcessID, pe.szExeFile);
		if (processID != 0 && pe.th32ProcessID == (unsigned int)processID) break;
		else if (strcmp(process, pe.szExeFile) == 0) break;
		hProcess =  Process32Next(hSnap, &pe);
	}

	if (!hProcess)
	{
		printf("cannot find the process.");
		exit(0);
	}

	Process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
	if (Process == NULL)
	{
		printf("Open process error!\n");
		exit(0);
	}


	printf("Enter the dll you want to unload:   ");
	scanf("%s",dll);
	hModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pe.th32ProcessID);
	if (hModule == NULL)
	{
		printf("Get module error!\n");
		exit(0);
	}
	mo.dwSize = sizeof(mo);
	hProcess = Module32First(hModule, &mo);
	while (hProcess)
	{
		if (strcmp(mo.szExePath,dll) == 0) break;
		hProcess = Module32Next(hModule, &mo);
	}

	if (!hProcess)
	{
		printf("cannot find dll.\n");
		exit(0);
	}


	return 0;
}