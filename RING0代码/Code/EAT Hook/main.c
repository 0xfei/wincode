#include "ntddk.h"
#include "main.h"
#pragma comment(lib, "ntdll.lib")

#define FUNCNAME "PsGetCurrentProcessId"
DWORD old, count = 0;
typedef HANDLE (*FUNCTION)();

HANDLE NewFunction()
{
	count++;
	return ((FUNCTION)old)();
}

DWORD GetModuleBaseAddress(PBYTE ModuleName)
{
	NTSTATUS status;
	DWORD size = 0, index;
	PDWORD buffer;
	PSYSTEM_MODULE_INFORMATION module;
	DWORD ModuleAddress = 0;

	ZwQuerySystemInformation(
		SystemModuleInformation, 
		NULL, 
		&size,
		&size);
	if (size == 0)
	{
		DbgPrint("SystemModuleInformation length cannot be defined!\n");
		return 0;
	}
	buffer = ExAllocatePool(NonPagedPool, size);
	if (!buffer)
	{
		DbgPrint("ExAllocatePool error!\n");
		return 0;
	}
	status = ZwQuerySystemInformation(
		SystemModuleInformation, 
		buffer, 
		size, 
		0);
	if (status != STATUS_SUCCESS)
	{
		DbgPrint("Query Information error!\n");
		return 0;
	}
	module = (PSYSTEM_MODULE_INFORMATION)((PDWORD)buffer + 1);
	for (index = 0; index < *buffer; index++)
	{
		if (_stricmp(module[index].ImageName + module[index].ModuleNameOffset, 
			ModuleName) == 0)
		{
			ModuleAddress = (DWORD)module[index].Base;
			DbgPrint("Module found at: %08x\n", ModuleAddress);
		}
	}
	ExFreePool(buffer);
	return ModuleAddress;
}

VOID Hook(BOOLEAN hook)
{
	DWORD base, index, addr = 0, i;
	PIMAGE_DOS_HEADER pDosHeader;
	PIMAGE_NT_HEADERS pNtHeader;
	PIMAGE_EXPORT_DIRECTORY exports;

	PBYTE pFuncName = NULL;
	PDWORD pAddressOfFunction, pAddressOfNames;
	PWORD pAddressOfNameOdrinals;

	base = GetModuleBaseAddress("ntkrnlpa.exe");
	DbgPrint("base: %08x\n", base);

	pDosHeader = (PIMAGE_DOS_HEADER)base;
	pNtHeader = (PIMAGE_NT_HEADERS)(base + pDosHeader->e_lfanew);
	exports = (PIMAGE_EXPORT_DIRECTORY)(base + 
		pNtHeader->OptionalHeader.DataDirectory[0].VirtualAddress);

	pAddressOfFunction = (PDWORD)(base + exports->AddressOfFunctions);
	pAddressOfNames = (PDWORD)(base + exports->AddressOfNames);
	pAddressOfNameOdrinals = (PWORD)(base + exports->AddressOfNameOrdinals);

	for (i = 0; i<exports->NumberOfNames; i++)
	{
		index = pAddressOfFunction[i];
		pFuncName = (PBYTE)(base + pAddressOfNames[i]);
		if (_stricmp(pFuncName, FUNCNAME) == 0)
		{
			addr = base + (DWORD)pAddressOfFunction[index];
			break;
		}
	}

	if (addr == 0)
	{
		DbgPrint("addr is 0\n");
		return;
	}

	if (hook)
	{
		_asm
		{
			cli;
			mov eax, cr0;
			and eax, not 10000h;
			mov cr0, eax;
			sti;
		}
		DbgPrint("PsGetCurrentProcessId : %08x\n", addr);
		pAddressOfFunction[index]  = (DWORD)NewFunction - base;
		old = addr;
		_asm
		{
			cli;
			mov eax, cr0;
			or eax, 10000h;
			mov cr0, eax;
			sti;
		}
	} else
	{
		_asm
		{
			cli;
			mov eax, cr0;
			and eax, not 10000h;
			mov cr0, eax;
			sti;
		}
		pAddressOfFunction[index]  = (DWORD)old - base;
		_asm
		{
			cli;
			mov eax, cr0;
			or eax, 10000h;
			mov cr0, eax;
			sti;
		}
	}
}

VOID Unload(PDRIVER_OBJECT driver)
{
	Hook(0);
	DbgPrint("Unload!\n");
	DbgPrint("PsGetCurrentProcessId called %d times\n", count);
}

NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING szReg)
{
	Hook(0);
	DbgPrint("DriverEntry!\n");
	driver->DriverUnload = Unload;
	return STATUS_SUCCESS;
}
