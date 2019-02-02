#include "ntddk.h"

ULONG OldFunc;

VOID Unload(PDRIVER_OBJECT driver )
{
	_asm
	{
		mov ecx, 0x176
		xor edx,edx
		mov eax, OldFunc
		wrmsr
	}
}


__declspec(naked) MyKiFastCallEntry()
{
	_asm jmp [OldFunc]
}

NTSTATUS DriverEntry(PDRIVER_OBJECT driver, IN PUNICODE_STRING szReg )
{
	driver->DriverUnload  = Unload; 

	_asm {
		mov ecx, 0x176
		rdmsr
		mov OldFunc, eax
		mov eax, MyKiFastCallEntry
		wrmsr
	}

	return STATUS_SUCCESS;
}