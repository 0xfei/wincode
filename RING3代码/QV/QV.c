#include "QV.h"


int main(int argc, char** argv)
{
	int ListenPort = 7890;
	SOCKET ListenSocket;
	SOCKET ClientSocket;
	CMD_PIPE_INFO CmdPipe;
	HANDLE CmdProc;
	THREAD_NEED_INFO ReadThread;
	THREAD_NEED_INFO WriteThread;
	int ReadThreadID, WriteThreadID;

	if(!CreateNetworkBind(ListenPort, &ListenSocket))
	{
		return 0;
	}

	if(!WaitForConnect(ListenSocket, &ClientSocket))
	{
		return 0;
	}

	if(!CreateCmdProcess(&CmdPipe, &CmdProc))
	{
		return 0;
	}

	ReadThread.Pipe = CmdPipe.InputPipe;
	ReadThread.Socket = ClientSocket;
	WriteThread.Pipe = CmdPipe.OutputPipe;
	WriteThread.Socket = ClientSocket;

	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadReadPipe, &ReadThread, 0, &ReadThreadID);
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadWritePipe, &WriteThread, 0, &WriteThreadID);

	system("pause");

	TerminateThread(OpenThread(THREAD_TERMINATE, 0, ReadThreadID), 0);
	TerminateThread(OpenThread(THREAD_TERMINATE, 0, WriteThreadID), 0);
	TerminateProcess(CmdProc, 0);

	CleanNetworkConnection(ListenSocket);

	return 0;
}