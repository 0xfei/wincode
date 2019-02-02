#include <windows.h>
#include <stdio.h>
#include <string.h>

typedef struct _remoteparam {
	DWORD rpWaitForSingleObject;
	DWORD rpOpenProcess;
	DWORD rpWinExec;
	DWORD rpProcessPID;
	HANDLE rpProcessHandle;
	char path[MAX_PATH];
}REMOTEPARAM;

typedef BOOL (_stdcall *pEnumProcesses)(DWORD* pProcessIds,DWORD cb,DWORD* pBytesReturned);
typedef BOOL (_stdcall *pEnumProcessModules)(HANDLE hProcess,HMODULE* lphModule,DWORD cb,LPDWORD lpcbNeeded);
typedef DWORD (_stdcall *pGetModuleBaseName)(HANDLE hProcess, HMODULE hModule, LPTSTR lpFilename, DWORD nSize);


DWORD WINAPI remote(LPVOID pvparam)
{
	REMOTEPARAM *rp = (REMOTEPARAM*)pvparam;
	typedef UINT(WINAPI *EWinExec)(LPCSTR, UINT);
	typedef HANDLE(WINAPI *EOpenProcess)(DWORD, BOOL, DWORD);
	typedef DWORD(WINAPI *EWaitForSingleObject)(HANDLE, DWORD);

	EWinExec tWinExec = (EWinExec)rp->rpWinExec;
	EOpenProcess tOpenProcess = (EOpenProcess)rp->rpOpenProcess;
	EWaitForSingleObject tWaitForSingleObject = (EWaitForSingleObject)rp->rpWaitForSingleObject;

	rp->rpProcessHandle = tOpenProcess(PROCESS_ALL_ACCESS, FALSE, rp->rpProcessPID);
	tWaitForSingleObject(rp->rpProcessHandle, INFINITE);
	tWinExec(rp->path, SW_SHOW);
	return 0;
}

BOOL EnablePriv()
{
	HANDLE hToken;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken)) {
		TOKEN_PRIVILEGES tkp;
		LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tkp.Privileges[0].Luid);
		tkp.PrivilegeCount = 1;
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL);
	}
	return 0;
}

DWORD processtopid(char* processname)
{
	DWORD lpidprocesses[1024], cbneeded, cprocesses;
	HANDLE hProcess;
	HMODULE hModule;
	UINT i;
	TCHAR normalname[MAX_PATH] = "UnkownProcess";
	
	HMODULE hPsDll = LoadLibrary("PSAPI.DLL");
	pEnumProcesses EnumProcesses = (pEnumProcesses)GetProcAddress(hPsDll, "EnumProcesses");
	pEnumProcessModules EnumProcessModules = (pEnumProcessModules)GetProcAddress(hPsDll, "EnumProcessModules");
	pGetModuleBaseName GetModuleBaseName = (pGetModuleBaseName)GetProcAddress(hPsDll, "GetModuleFileNameExA");

	if (!EnumProcesses(lpidprocesses, sizeof(lpidprocesses), &cbneeded)) {
		return -1;
	}
	cprocesses = cbneeded/sizeof(DWORD);
	for (i = 0; i < cprocesses; i++) {
		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, lpidprocesses[i]);
		if (hProcess) {
			if (EnumProcessModules(hProcess, &hModule, sizeof(hModule), &cbneeded)) {
				GetModuleBaseName(hProcess, hModule, normalname, sizeof(normalname));
				if (strstr(normalname,processname) != NULL) {
					CloseHandle(hProcess);
					return lpidprocesses[i];
				}
			}
		}
	}
	CloseHandle(hProcess);
	return 0;
}

HANDLE CreateRemoteThreadProc(char* ProcessName)
{
	HANDLE hThreadHandle;
	char szFilePath[MAX_PATH];
	GetModuleFileName(NULL, szFilePath, MAX_PATH);

	int procID = processtopid(ProcessName);
	printf("The process id is: %d\n", procID);

	HINSTANCE hKernel32;
	HANDLE rphandle;
	char *pRemoteAddress;
	char *pRemoteParame;
	int cb;
	rphandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, procID);
	if (rphandle == NULL) {
		printf("Open remote process error!\n");
		return NULL;
	} else {
		printf("Open remote process success.\n");
	}

	cb = sizeof(char)*4*1024;
	pRemoteAddress = (PTSTR)VirtualAllocEx(rphandle, NULL, cb, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (pRemoteAddress == NULL) {
		printf("VirtualAllocEx error!\n");
		CloseHandle(rphandle);
	} else {
		printf("VirtualAllocEx success.\n");
	}

	if (WriteProcessMemory(rphandle, pRemoteAddress, (LPVOID)remote, cb, NULL) == FALSE) {
		printf("WriteProcessMemory error!\n");
		CloseHandle(rphandle);
	} else {
		printf("WriteProcessMemory success.\n");
	}

	REMOTEPARAM rp;
	memset(&rp, 0, sizeof(rp));
	hKernel32 = GetModuleHandle("kernel32.dll");
	if (hKernel32 == NULL) {
		printf("Get kernel32 handle error!\n");
		CloseHandle(rphandle);
		return 0;
	}
	rp.rpProcessPID = GetCurrentProcessId();
	rp.rpOpenProcess = (DWORD)GetProcAddress(hKernel32, "OpenProcess");
	rp.rpWaitForSingleObject = (DWORD)GetProcAddress(hKernel32, "WaitForSingleObject");
	rp.rpWinExec = (DWORD)GetProcAddress(hKernel32, "WinExec");
	strcpy(rp.path, szFilePath);

	cb = sizeof(char)*sizeof(rp);
	pRemoteParame = (PTSTR)VirtualAllocEx(rphandle, NULL, cb, MEM_COMMIT, PAGE_READWRITE);
	if (pRemoteParame == NULL) {
		printf("VirtualAllocEx error!\n");
	} else {
		printf("VirtualAllocEx success!\n");
	}

	if (WriteProcessMemory(rphandle, pRemoteParame, (LPVOID)&rp, cb, NULL) == FALSE) {
		printf("writeprocess memory for parameter error!\n");
	} else {
		printf("writeprocess memory for parameter success.\n");
	}

	hThreadHandle = CreateRemoteThread(rphandle, NULL, 0, (LPTHREAD_START_ROUTINE)pRemoteAddress, pRemoteParame,	0, NULL);
	if (hThreadHandle == NULL) {
		printf("Create remote thread error!\n");
	} else {
		printf("Create remote thread success.\n");
	}
	return hThreadHandle;
}

int main()
{
	HANDLE RemoteThreadHandle;
	EnablePriv();
	RemoteThreadHandle = CreateRemoteThreadProc("notepad.exe");
	WaitForSingleObject(RemoteThreadHandle, INFINITE);
	return 0;
}
