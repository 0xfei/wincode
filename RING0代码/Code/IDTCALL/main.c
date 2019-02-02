#include "ntddk.h"
#include "stdio.h"


#define IDT_LIMIT (0xff * 8)
#define GATE_TYPE 0xEE

typedef unsigned int DWORD;
typedef unsigned char BYTE;
typedef unsigned short WORD;

char outString[] = "abcdefg";

int now;
typedef struct _IDTGATE {
	WORD offsetl;
	WORD selector;
	BYTE count;
	BYTE type;
	WORD offseth;
} IDTGATE, *PIDTGATE;


__declspec(naked) void MyCALLGATE()
{
	__asm {
		cli;
		pushad;
		pushfd;
		mov eax, offset outString;
		push eax;
		mov eax, 80528e92h;
		call eax;
		pop eax;
		popfd;
		popad;
		sti;
		iretd;
	}
}

NTSTATUS AddCallGate(DWORD MyCALLGATE)
{
	char s[256];
	char idt[6];
	DWORD base; 
	PIDTGATE pCallGate = NULL;

	now = 0;

	_asm sidt idt;
	base = *(DWORD*)(idt + 2);

	while (now < IDT_LIMIT)
	{
		pCallGate = (PIDTGATE)(base + now);
		if ((pCallGate->type & 0x80) == 0)
		{
			_snprintf(s, 256, "%08x\n", now/8);
			DbgPrint(s);
			pCallGate->type = GATE_TYPE;
			pCallGate->offsetl = (WORD)((DWORD)MyCALLGATE & 0xFFFF);
			pCallGate->selector = 0x08;
			pCallGate->offseth = (WORD)((DWORD)MyCALLGATE >> 16);
			pCallGate->count = 0;
			DbgPrint("Add IDT gate!\n");
			break;
		}
		now += 8;
	}

	return STATUS_SUCCESS;
}

void Unload(PDRIVER_OBJECT driver)
{
	char idt[6];
	DWORD base; 
	PIDTGATE pCallGate = NULL;

	_asm sidt idt;

	base = *(DWORD*)(idt + 2);
	pCallGate = (PIDTGATE)(base + now);
	pCallGate->type = 0;
	DbgPrint("Unload!\n");
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT driver, IN PUNICODE_STRING szReg)
{
	AddCallGate((DWORD)MyCALLGATE);
	driver->DriverUnload = Unload;
	return STATUS_SUCCESS;
}