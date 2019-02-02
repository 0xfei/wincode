#include "ntddk.h"
#include "stdio.h"

typedef unsigned short WORD;
typedef unsigned long ULONG, DWORD;
typedef unsigned char BYTE;

typedef struct _IDTINFO
{
	WORD IDTLimit;
	WORD LowIDTBase;
	WORD HiIDTBase;
} IDTINFO;

#define MAKELONG(a,b) ((DWORD) (((WORD) (a)) | (((DWORD) ((WORD) (b))) << 16)))

#pragma pack(1)
typedef struct _IDTENTRY
{
	WORD LowOffset;
	WORD selector;
	BYTE unused_lo;
	unsigned char unused_hi:5;
	unsigned char DPL:2;
	unsigned char P:1;
	WORD HiOffset;
} IDTENTRY;
#pragma pack()

IDTINFO IdtInfo;
IDTENTRY *IdtEntries;
DWORD old[256];
DWORD count[256];
BYTE* tables;

#define MIN_IDT 0
#define MAX_IDT 0xFF

char template[] = {
	0x90,                    //nop, debug
	0x60,                    //pushad
	0x9C,                    //pushfd
	0xB8, 0xAA, 0x00, 0x00, 0x00,            //mov eax, AAh
	0x50,                    //push eax
	0x9A, 0x11, 0x22, 0x33, 0x44, 0x08, 0x00,        //call 08:44332211h
	0x58,                    //pop eax
	0x9D,                    //popfd
	0x61,                    //popad
	0xEA, 0x11, 0x22, 0x33, 0x44, 0x08, 0x00          //jmp 08:44332211h
};

void __stdcall NewISR(DWORD nouse)
{
	unsigned long *index;
	unsigned long i;

	__asm mov eax,[ebp+0Ch]
	__asm mov i, eax

	i = i & 0xFF;
	index = &count[i];
	InterlockedIncrement(index);
}

void HookIDT()
{
	int i, offset = 0;
	char* entry;
	for (i = MIN_IDT; i < MAX_IDT; i++)
	{
		old[i] = MAKELONG(IdtEntries[i].LowOffset, IdtEntries[i].HiOffset);
		entry = tables + offset;
		memcpy(entry, template, sizeof(template));
		entry[4] = (BYTE)i;
		*((DWORD*)(&entry[10])) = (DWORD)NewISR;
		*((DWORD*)(&entry[20])) = (DWORD)old[i];
		__asm cli
		IdtEntries[i].LowOffset = (WORD)entry;
		IdtEntries[i].HiOffset = (WORD)((DWORD)entry >> 16);
		__asm sti
		offset += sizeof(template);
	}
}

void UnHookIDT()
{
	int i;
	char s[256];
	for (i = MIN_IDT; i < MAX_IDT; i++)
	{
		__asm cli
		IdtEntries[i].LowOffset = (WORD)old[i];
		IdtEntries[i].HiOffset = (WORD)(old[i]>> 16);
		__asm sti
		_snprintf(s, 256, "%d: %08x count: %d\n", i, old[i], count[i]);
		DbgPrint(s);
	}
}

VOID Unload(PDRIVER_OBJECT driver)
{
	UnHookIDT();
	ExFreePool(tables);
}

NTSTATUS DriverEntry( PDRIVER_OBJECT driver, PUNICODE_STRING szReg)
{
	_asm sidt IdtInfo
	IdtEntries = (IDTENTRY *)MAKELONG(IdtInfo.LowIDTBase, IdtInfo.HiIDTBase);
	tables = ExAllocatePool(NonPagedPool, sizeof(template)*256);
	HookIDT();
	DbgPrint("DriverEntry success!\n");
	driver->DriverUnload = Unload;
	return STATUS_SUCCESS;
}
