#include "ntddk.h" 
#include "main.h"

ULONG GetKernelBase(ULONG dwSomeWhere)
{
	ULONG Base, count, now;
	ULONG StartAddress = (ULONG)MmSystemRangeStart;
	if (dwSomeWhere >= StartAddress)
	{
		now = dwSomeWhere & !(PAGE_SIZE-1);
		count = (now - StartAddress) >> PAGE_SHIFT;
		while (count--)
		{
			if (MmIsAddressValid((ULONG*)now))
			{
				if (*(ULONG*)now == 0x00905A4D)
					return now;
			}
			now -= PAGE_SIZE;
		}
	}
	return 0;
}

void LookPE(ULONG base)
{
	ULONG index, addr = 0, i;
	PIMAGE_DOS_HEADER pDosHeader;
	PIMAGE_NT_HEADERS pNtHeader;
	PIMAGE_EXPORT_DIRECTORY exports;

	PCHAR pFuncName = NULL;
	PULONG pAddressOfFunction, pAddressOfNames;
	PULONG pAddressOfNameOdrinals;

	pDosHeader = (PIMAGE_DOS_HEADER)base;
	pNtHeader = (PIMAGE_NT_HEADERS)(base + pDosHeader->e_lfanew);
	exports = (PIMAGE_EXPORT_DIRECTORY)(base + 
		pNtHeader->OptionalHeader.DataDirectory[0].VirtualAddress);

	pAddressOfFunction = (PULONG)(base + exports->AddressOfFunctions);
	pAddressOfNames = (PULONG)(base + exports->AddressOfNames);
	pAddressOfNameOdrinals = (PULONG)(base + exports->AddressOfNameOrdinals);

	for (i = 0; i<exports->NumberOfNames; i++)
	{
		index = pAddressOfFunction[i];
		pFuncName = (PBYTE)(base + pAddressOfNames[i]);
		DbgPrint("%s\n", pFuncName);
	}
}

void Unload(PDRIVER_OBJECT driver)
{
	DbgPrint("Unload!\n");
}

NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING szReg)
{
	ULONG dwRet;
	_asm 
	{
		lea eax, [ebp+4];
		mov dwRet, eax;
	}
	if (MmIsAddressValid((ULONG*)dwRet))
	{
		dwRet = GetKernelBase(*(ULONG*)dwRet);
		DbgPrint("Kernel base is %p", dwRet);
		LookPE(0x804D8000);
	}
	driver->DriverUnload = Unload;
	return STATUS_SUCCESS;
}
