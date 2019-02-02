
include w2k\ntstatus.inc
include w2k\ntddk.inc
include w2k\ntoskrnl.inc
include w2k\hal.inc
include w2k\w2kundoc.inc
includelib c:\masm32\lib\w2k\ntoskrnl.lib
includelib c:\masm32\lib\w2k\hal.lib
include c:\masm32\Macros\Strings.mac

IOPM_SIZE equ 2000h
IOCTL_MODIFY_IOPMBASE	        equ CTL_CODE(FILE_DEVICE_UNKNOWN, 800h, METHOD_BUFFERED, FILE_ANY_ACCESS)
