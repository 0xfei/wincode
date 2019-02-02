#include "QV.h"


int CreateBind(
	int ListenPort,
	SOCKET * ListenSocket
	)
{
	SOCKET ServSocket;
	struct sockaddr_in LocalAddr;
	int ret;
	
	ServSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(ServSocket == INVALID_SOCKET)
	{
		return 0;
	}
	
	LocalAddr.sin_family = AF_INET;
	LocalAddr.sin_addr.s_addr = inet_addr("0.0.0.0");
	LocalAddr.sin_port = htons(ListenPort);
	
	ret = bind(ServSocket, (struct sockaddr *)&LocalAddr, sizeof(LocalAddr));
	if(ret != 0)
	{
		return 0;
	}
	
	ret = listen(ServSocket, 1);
	if(ret != 0)
	{
		return 0;
	}
	
	*ListenSocket = ServSocket;
	
	return 1;
}

int InitSocket()
{
	WSADATA wsData;
	
	if(WSAStartup(MAKEWORD(2,2), &wsData) != 0)
	{
		return 0;
	}
	
	return 1;
}

int CreateNetworkBind(int Port, SOCKET * ListenSocket)
{
	if(!InitSocket())
	{
		return 0;
	}
	
	if(!CreateBind(Port, ListenSocket))
	{
		WSACleanup();
		return 0;
	}
	
	return 1;

}