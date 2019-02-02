#include "QV.h"

int ThreadWritePipe(LPVOID lpParameter)
{
	int ret;
	int WriteBytes;
	char Str[STR_MAX_LEN];
	PTHREAD_NEED_INFO info = lpParameter;

	while(1)
	{
		ret = recv(info->Socket, Str, STR_MAX_LEN, 0);
		
		ret = WriteFile(info->Pipe, Str,
			ret, &WriteBytes, NULL);
	}
}