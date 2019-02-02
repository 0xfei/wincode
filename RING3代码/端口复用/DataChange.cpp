#include <windows.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

DWORD readwrite(SOCKET tr, SOCKET tc)
{
	char bufc[40960], bufr[40960];
	char* recvbufpc;
	char* recvbufpr;
	long recvr, recvc;
	FD_SET ding, ding1;
	int err, ret;
	SOCKET clientr = tr, clientc = tc;

	recvr = recvc = 0;
	struct timeval TimeOut;
	TimeOut.tv_sec = 0;
	TimeOut.tv_usec = 1000;
	FD_ZERO(&ding);
	FD_ZERO(&ding1);
	FD_SET(clientc, &ding);
	FD_SET(clientr, &ding);

	while (FD_ISSET(clientc, &ding) && FD_ISSET(clientr, &ding))
	{
		ding1 = ding;
		if Er = select(16, &ding1, 0, 0, &TimeOut);
		if ( (Er==SOCKET_ERROR) )
		{
			FD_CLR(clientr, &ding1);
			FD_CLR(clientc, &ding1);
		}
		if (FD_ISSET(clientc, &ding1))
		{
			ret = recv(clientc, bufc, sizeof(bufc), 0);
			if (ret == SOCKET_ERROR || ret == 0 || ret == WSAECONNRESET)
			{
				FD_ZERO(&ding);
			}
			else
			{
				recvc = ret;
				bufc[ret] = '\0';
				recvbufpc = bufc;
			}
		}
		if (FD_ISSET(clientr, &ding1))
		{
			ret = recv(clientr, bufr, sizeof(bufr), 0);
			if (ret == SOCKET_ERROR || ret == 0 || ret == WSAECONNRESET)
			{
				FD_ZERO(&ding);
			}
			else
			{
				recvr = ret;
				bufr[ret] = '\0';
				recvbufpr = bufr;
			}
		}

print:
		if (recvr)
		{
			ret = send(clientc, recvbufpr, recvr, 0);
			if (ret > 0)
			{
				recvbufpr += ret;
				recvr -= ret;
			}
			else
			{
				err = WSAGetLastError();
				if (err != WSAETIMEDOUT)
				{
					FD_ZERO(&ding);
					recvr = 0;
				}
			}
		}
		if (recvc)
		{
			ret = send(clientr, recvbufpc, recvc, 0);
			if (ret > 0)
			{
				recvbufpc += ret;
				recvc -= ret;
			}
			else
			{
				err = WSAGetLastError();
				if (err != WSAETIMEDOUT)
				{
					FD_ZERO(&ding);
					recvc = 0;
				}
			}
		}
		Sleep(1);
		if (recvr || recvc) goto print;
	}
	closesocket(clientr);
	closesocket(clientc);
	return 1;
}