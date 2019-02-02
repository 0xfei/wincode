#include "QV.h"

void CleanNetworkConnection(SOCKET ListenSocket)
{
	closesocket(ListenSocket);
	WSACleanup();
}