#include "QV.h"

int WaitForConnect(SOCKET Server, SOCKET * Client)
{
	struct sockaddr_in ClientAddr;
	int AddrLen = sizeof(ClientAddr);

	*Client = accept(Server, (struct sockaddr *)&ClientAddr, &AddrLen);
	if(*Client == INVALID_SOCKET)
	{
		return 0;
	}
	
	return 1;
}