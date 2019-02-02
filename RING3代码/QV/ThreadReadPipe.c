#include "QV.h"

int ThreadReadPipe(LPVOID lpParameter)
{
	int ret;
	int ReadBytes;
	char Str[STR_MAX_LEN];
	PTHREAD_NEED_INFO info = lpParameter;

	while(1)
	{
		memset(Str, 0, sizeof(Str));

		ret = ReadFile(info->Pipe, Str,
			STR_MAX_LEN, &ReadBytes, NULL);

		send(info->Socket, Str, ReadBytes, 0);
	}
}