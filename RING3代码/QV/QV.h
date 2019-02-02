#ifndef _QV_H_
#define _QV_H_

#include <winsock2.h>
#include <windows.h>
#include <stdio.h>

#pragma comment(lib,"ws2_32.lib")

typedef struct CMD_PIPE_INFO
{
	HANDLE Process;
	HANDLE InputPipe;
	HANDLE OutputPipe;
	HANDLE CmdInput;
	HANDLE CmdOutput;
}CMD_PIPE_INFO, *PCMD_PIPE_INFO;

typedef struct THREAD_NEED_INFO
{
	HANDLE Pipe;
	SOCKET Socket;
}THREAD_NEED_INFO,*PTHREAD_NEED_INFO;

#define STR_MAX_LEN 1024

int CreateNetworkBind(int Port, SOCKET * ListenSocket);

int CreateCmdProcess(PCMD_PIPE_INFO CmdPipe, HANDLE * CmdProc);

void CleanNetworkConnection(SOCKET ListenSocket);

int WaitForConnect(SOCKET Server, SOCKET * Client);

int ThreadWritePipe(LPVOID lpParameter);

int ThreadReadPipe(LPVOID lpParameter);

#endif