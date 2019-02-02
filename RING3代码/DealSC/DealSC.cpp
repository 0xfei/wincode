#include <windows.h>
#include <Winsvc.h>
#include <cstdio>

SC_HANDLE scm;

int EnumelateServices()
{
	
	if ( (scm = OpenSCManager(
		NULL, 
		NULL, 
		SC_MANAGER_ALL_ACCESS)
		) == NULL ) {
		printf("Open SCManager Error!\n");
		//return 0;
	}

	LPENUM_SERVICE_STATUS lpServices = NULL;
	DWORD nSize = 0, n, nResumeHandle = 0, i;
	lpServices = (LPENUM_SERVICE_STATUS)LocalAlloc(LPTR, 64 * 1024);
	EnumServicesStatus(scm, 
		SERVICE_WIN32, 
		SERVICE_STATE_ALL, 
		(LPENUM_SERVICE_STATUS)lpServices, 
		64*1024, 
		&nSize, 
		&n, 
		&nResumeHandle);

	LPQUERY_SERVICE_CONFIG ServiceInfo = NULL;
	for ( i=0; i < n; i++) {
		SC_HANDLE service = NULL;
		DWORD dwBytesNeeded = 0, cbBufSize=0;
		service = OpenService(scm, 
			lpServices[i].lpServiceName, 
			SERVICE_ALL_ACCESS);
		if (!(QueryServiceConfig(service, 
			NULL, 
			0, 
			&dwBytesNeeded))) {
			DWORD dwError = GetLastError();
			if (dwError == ERROR_INSUFFICIENT_BUFFER) {
				cbBufSize = dwBytesNeeded;
			} else
			{
				printf("Error!\n");
				return 0;
			}
		}

		ServiceInfo = (LPQUERY_SERVICE_CONFIG)LocalAlloc(LPTR, dwBytesNeeded);
		QueryServiceConfig(service, 
			ServiceInfo, 
			cbBufSize, 
			&dwBytesNeeded);
		printf("%s||",ServiceInfo->lpBinaryPathName);
		printf("%s||",lpServices[i].lpServiceName);
		printf("%s||",lpServices[i].lpDisplayName);
		if (lpServices[i].ServiceStatus.dwCurrentState != SERVICE_STOPPED) {
			printf("START\n");
		} else {
			printf("STOP\n");
		}
	}
	LocalFree(lpServices);
	return 0;
}


BOOL StartServices(SC_HANDLE service, BOOL start)
{
	SERVICE_STATUS status;
	BOOL isSuccess = QueryServiceStatus(service, &status);
	if (!isSuccess) {
		printf("Query services status error");
		return FALSE;
	}
	if (start) {
		if (status.dwCurrentState == SERVICE_STOPPED) {
			isSuccess  = StartService(service, 0, NULL);
			return isSuccess;
		} else return TRUE;
	} else {
		isSuccess = ControlService(service, SERVICE_CONTROL_STOP, &status);
		return isSuccess;
	}
}

BOOL CreateServer()
{
	SC_HANDLE service;
	service = CreateService(
		scm, 
		"test", 
		"test", 
		SERVICE_ALL_ACCESS, 
		SERVICE_WIN32_OWN_PROCESS, 
		SERVICE_AUTO_START, 
		SERVICE_ERROR_NORMAL, 
		"C:\\1.exe", 
		0,
		0,
		0,
		0,
		0 );
	if (!service) return FALSE;
	CloseServiceHandle(service);
	return TRUE;
}

BOOL ChangeServer(SC_HANDLE service)
{
	SC_LOCK scLock;
	scLock = LockServiceDatabase(scm);
	ChangeServiceConfig(service, 
		SERVICE_NO_CHANGE,
		SERVICE_DISABLED,
		SERVICE_NO_CHANGE,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL);
	UnlockServiceDatabase(scLock);
	return TRUE;
}

int main()
{
	EnumelateServices();
	return 0;
}
