#include <windows.h>
#include <winsock.h>
#include <stdio.h>
#include <string.h>
#pragma comment(lib, "ws2_32.lib")

typedef struct stc2
{
	char user[128];
	char pass[128];
	char buffer[256];
} stc;

typedef struct tag_request_t {
	unsigned char len_token;
	char *token;
	unsigned char fucntion_id;
	unsigned int len_data;	//Must be big endian in network byte order
	char *data;
} request_t;

request_t Request;
stc temp;

int main()
{
	WSADATA wsaData;
	SOCKADDR_IN serverAddr, clientAddr;
	SOCKET server, client;
	int port = 30000;
	char buffer[512];
	WORD ver = MAKEWORD(1,1);
	DWORD length;
	WSAStartup(ver, &wsaData);

	server = socket(AF_INET, SOCK_STREAM, 0);

	serverAddr.sin_port = htons(port);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	
	bind(server, &serverAddr, sizeof(serverAddr));
	listen(server, 5);

	while (1)
	{
		client = accept(server, &clientAddr, &length);
		recv(client, buffer, 512);
		Request.len_token = *(unsigned char*)&buffer[0];
		Request.token = new char[Request.len_token];
		strncpy(Request.token, &buffer[1], Request.len_token);
		Request.fucntion_id = *(unsigned char*)&buffer[Request.len_token + 1];
		Request.len_data = *(unsigned int*)&buffer[Request.len_token + 2];
		Request.data = new char[Request.len_data];
		memcpy(&Request.data, &buffer[Request.len_token+2+4], Request.len_data);

	}

	closesocket(server);
	WSACleanup();
	
	system("pause");
	return 0;
}