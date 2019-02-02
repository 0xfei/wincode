#include    <windows.h>
#include    <stdio.h>
#include    <winioctl.h>
#include    <string.h>
#define IOCTL_MODIFY_IOPMBASE  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
int main()
{
    HANDLE  hDevice;
    BOOL    Status;
    ULONG   dwReturn;
    DWORD procId;
    hDevice=CreateFile("\\\\.\\MODIFYIOPMBASE",GENERIC_READ | GENERIC_WRITE,
					  0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,
					  NULL);
    if(hDevice==INVALID_HANDLE_VALUE)
    {
		  printf("createfile wrong\n");
		  getchar();
		  return  0;
    }

	procId = GetCurrentProcessId();
    Status=DeviceIoControl(hDevice,IOCTL_MODIFY_IOPMBASE,&procId,4,
						  NULL,0,&dwReturn,NULL);
    if(!Status)
    {
      printf("Io wrong\n");
      return  0;
    }

	__asm
	{
		mov dx,0x64
		mov al,0xfe
		out dx,al
	}
    CloseHandle( hDevice );
    return  0;
}