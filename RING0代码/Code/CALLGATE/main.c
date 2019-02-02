#include "ntddk.h"
#include "stdio.h"


#define GDT_LIMIT 0x3ff
#define GATE_TYPE 0xEC

typedef unsigned int DWORD;
typedef unsigned char BYTE;
typedef unsigned short WORD;

char outString[] = "abcdefg";

int now;
typedef struct _CALLGATE {
	WORD offsetl;
	WORD selector;
	BYTE count;
	BYTE type;
	WORD offseth;
} CALLGATE, *PCALLGATE;


__declspec(naked) void MyCALLGATE()
{
	__asm {
		cli;
		pushad;
		pushfd;
		mov eax, offset outString
		push eax
		mov eax, 80528e92h;
		call eax;
		pop eax
		popfd;
		popad;
		sti;
		retf;
	}
}

NTSTATUS AddCallGate(DWORD MyCALLGATE)
{
	char s[256];
	char gdt[6];
	DWORD base; 
	PCALLGATE pCallGate = NULL;

	now = 8;

	_asm sgdt gdt;
	base = *(DWORD*)(gdt + 2);

	while (now < GDT_LIMIT)
	{
		pCallGate = (PCALLGATE)(base + now);
		if ((pCallGate->type & 0x80) == 0)
		{
			_snprintf(s, 256, "%08x\n", now|3);
			DbgPrint(s);
			pCallGate->type = GATE_TYPE;
			pCallGate->offsetl = (WORD)((DWORD)MyCALLGATE & 0xFFFF);
			pCallGate->selector = 0x08;
			pCallGate->offseth = (WORD)((DWORD)MyCALLGATE >> 16);
			pCallGate->count = 0;
			DbgPrint("Add call gate!\n");
			break;
		}
		now += 8;
	}

	return STATUS_SUCCESS;
}

void Unload(PDRIVER_OBJECT driver)
{
	char gdt[6];
	DWORD base; 
	PCALLGATE pCallGate = NULL;

	_asm sgdt gdt;
	
	base = *(DWORD*)(gdt + 2);
	pCallGate = (PCALLGATE)(base + now);
	pCallGate->type = 0;
	DbgPrint("Unload!\n");
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT driver, IN PUNICODE_STRING szReg)
{
	AddCallGate((DWORD)MyCALLGATE);
	driver->DriverUnload = Unload;
	return STATUS_SUCCESS;
}