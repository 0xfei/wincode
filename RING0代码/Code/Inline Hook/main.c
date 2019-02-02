#include "ntddk.h"

#define NEWENTRYOFFSET 8
char old[10];
char outString[] = "Function: %ws\n";

__declspec(naked) MyMmGetSystemRoutineAddress()
{
	__asm{
		cli;
		push    ebp;
		mov     ebp,esp;
		sub     esp,20h;
		mov eax, [ebp + 8]
		push eax
		mov eax, offset outString;
		push eax;
		mov eax, 80528e92h;
		call eax;
		pop eax;
		sti;
		_emit 0xEA;
		_emit 0xAA;
		_emit 0xAA;
		_emit 0xAA;
		_emit 0xAA;
		_emit 0x80;
		_emit 0x00;
	}
}

void Hook()
{
	int i;
	UNICODE_STRING uniName;
	ULONG Address = 0, newEntry;
	unsigned char * realAddress;
	unsigned char * funcAddress;
	ULONG tempAddress;
	char temp[] = {
		0xEA,
		0xAA,
		0xAA,
		0xAA,
		0xAA,
		0x80,
		0x00,
		0x90
	};

	RtlInitUnicodeString(&uniName, L"MmGetSystemRoutineAddress");
	Address = (ULONG)MmGetSystemRoutineAddress(&uniName);
	if (Address == 0) 
	{
		DbgPrint("Cannot find address!\n");
		return;
	}
	realAddress = (unsigned char*)Address;
	newEntry = Address + NEWENTRYOFFSET;

	tempAddress = (ULONG)ExAllocatePool(NonPagedPool, 256);
	funcAddress = (unsigned char *)tempAddress;
	for (i =0; i<256; i++)
	{
		funcAddress[i] = 
			((unsigned char *)MyMmGetSystemRoutineAddress)[i];
	}
	*((unsigned long *)(&temp[1])) = (unsigned long)funcAddress;
	for (i=0; i<200; i++)
	{
		if ( (funcAddress[i] == 0xAA) &&
			(funcAddress[i+1] == 0xAA) &&
			(funcAddress[i+2] == 0xAA) &&
			(funcAddress[i+3] == 0xAA) )
		{
			*(ULONG*)(&funcAddress[i]) = newEntry;
			break;
		}
	}
	for (i = 0; i<NEWENTRYOFFSET; i++)
	{
		old[i] = realAddress[i];
		realAddress[i] = temp[i];
	}
}

void UnHook()
{
	int i;
	UNICODE_STRING uniName;
	ULONG Address = 0;
	unsigned char * realAddress;
	RtlInitUnicodeString(&uniName, L"MmGetSystemRoutineAddress");
	Address = (ULONG)MmGetSystemRoutineAddress(&uniName);
	if (Address == 0) return;
	realAddress = (unsigned char*)Address;
	for (i = 0; i<NEWENTRYOFFSET; i++)
	{
		realAddress[i] = old[i];
	}
}

void Unload(PDRIVER_OBJECT driver)
{
	UnHook();
	DbgPrint("Unload!\n");
}

NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING szReg)
{
	DbgPrint("DriverEntry");
	Hook();
	driver->DriverUnload = Unload;
	return STATUS_SUCCESS;
}