#include "ntddk.h"

extern NTKERNELAPI PVOID MmSystemRangeStart;

ULONG FindBase(ULONG need)
{
	ULONG start, temp, now;
	__asm
	{
		mov eax,MmSystemRangeStart;
		mov eax,[eax];
		mov eax,[eax];
		mov start, eax;
	}
	if (need >= start)
	{
		need = need & (!(PAGE_SIZE - 1));
		temp = (need - start) >> PAGE_SHIFT;
		while (temp--)
		{
			if (MmIsAddressValid(need))
			{
				now = *(PULONG*)need;
				if (now == 0x00905A4D)
				{
					return now;
				}
			}
			need -= PAGE_SIZE;
		}
	}
}

void Unload(DRIVER_OBJECT driver)
{
	DbgPrint("Unload!\n");
}

NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING)
{
	PULONG AddressOfRet;
	ULONG Ret = 0;

	DbgPrint("DriverEntry!\n");
	driver->DriverUnload = Unload;
	
	__asm
	{
		lea ecx, [ebp+4];
		mov AddressOfRet, ecx;
	}
	if (!MmIsAddressValid(AddressOfRet))
	{
		DbgPrint("AddressOfRet error!\n");
		return STATUS_SUCCESS;
	}
	
	Ret = FindBase(*AddressOfRet);
	
	if (!Ret)
	{
		DbgPrint("Cannot find base address of ntoskrnl.exe!\n");
	} else
	{
		DbgPrint("Base %p\n", Ret);
	}
	return STATUS_SUCCESS;
}