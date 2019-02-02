#include "ntddk.h"
#include "windef.h"

extern POBJECT_TYPE *PsProcessType;

void InlineHookObreferenceObjectByHandle();
void Unhook();

char ProcessName[] = "notepad.exe";

int NewObReferenceObjectByHandle(
	HANDLE Handle,
	ACCESS_MASK DesiredAccess,
	POBJECT_TYPE ObjectType,
	KPROCESSOR_MODE AccessMode,
	PVOID *Object,
	POBJECT_HANDLE_INFORMATION HandleInformation
);

__declspec(naked)  NewFunction(
	IN HANDLE  Handle,
	IN ACCESS_MASK  DesiredAccess,
	IN POBJECT_TYPE  ObjectType  OPTIONAL,
	IN KPROCESSOR_MODE  AccessMode,
	OUT PVOID  *Object,
	OUT POBJECT_HANDLE_INFORMATION  HandleInformation  OPTIONAL
	)
{
	_asm
	{
		mov edi,edi;
		push ebp;
		mov ebp,esp;
		push [ebp+0x1c];
		push [ebp+0x18];
		push [ebp+0x14];
		push [ebp+0x10];
		push [ebp+0xc];
		push [ebp+8];
		call NewObReferenceObjectByHandle;
		cmp eax,1;
		jnz end;
		mov [ebp+8],-1;
	end:
		mov eax,ObReferenceObjectByHandle;
		add eax,5;
		jmp eax;   
	}
}

int NewObReferenceObjectByHandle(
	HANDLE Handle,
	ACCESS_MASK DesiredAccess,
	POBJECT_TYPE ObjectType,
	KPROCESSOR_MODE AccessMode,
	PVOID *Object,
	POBJECT_HANDLE_INFORMATION HandleInformation
	)
{
	PEPROCESS Process;
	KIRQL oldIrql;
	int JmpOffset, ans = 0;
	unsigned char JmpCode[5] = { 0xE9, 0xAA, 0xAA, 0xAA, 0xAA};
	unsigned char Code[5] = { 0x8B, 0xFF, 0x55, 0x8B, 0xEC };

	if (ObjectType == *PsProcessType)
	{
		oldIrql = KeRaiseIrqlToDpcLevel();
		__asm
		{
			cli;
			mov eax, cr0;
			and eax, not 10000H;
			mov cr0, eax;
		}
		RtlCopyMemory(ObReferenceObjectByHandle, Code, 5);
		ObReferenceObjectByHandle(Handle, DesiredAccess, 
			ObjectType, AccessMode, &Process, NULL);
		if (_stricmp((char*)((char*)Process+0x174), ProcessName) == 0)
		{
			ans = 1;
		}
		JmpOffset = (char*)NewFunction - (char*)ObReferenceObjectByHandle - 5;
		RtlCopyMemory(JmpCode + 1, &JmpOffset, 4);
		RtlCopyMemory(ObReferenceObjectByHandle, JmpCode, 5);
		__asm
		{
			mov eax, cr0;
			or eax, 10000H;
			mov cr0, eax;
			sti;
		}
		KeLowerIrql(oldIrql);
	}
	return ans;
}

void Hook()
{
	int JmpOffset;
	unsigned char JmpCode[5] = { 0xE9, 0xAA, 0xAA, 0xAA, 0xAA };
	KIRQL oldIrql;

	JmpOffset = (char*)NewFunction - (char*)ObReferenceObjectByHandle - 5;
	RtlCopyMemory( JmpCode+1, &JmpOffset, 4);
	oldIrql = KeRaiseIrqlToDpcLevel();
	__asm
	{
		cli;
		mov eax, cr0;
		and eax, not 10000H;
		mov cr0, eax;
	}
	RtlCopyMemory(ObReferenceObjectByHandle, JmpCode, 5);
	__asm
	{
		mov eax, cr0;
		or eax, 10000h;
		mov cr0, eax;
		sti;
	}
	KeLowerIrql(oldIrql);
}

VOID Unload(PDRIVER_OBJECT  DriverObject)
{  
	KIRQL oldIrql;
	LARGE_INTEGER Delay;
	unsigned char Code[5]={0x8b,0xff,0x55,0x8b,0xec};

	Delay.QuadPart = -5000000;
	KeDelayExecutionThread(KernelMode, TRUE, &Delay);
	oldIrql = KeRaiseIrqlToDpcLevel();
	__asm
	{
		cli;             
		mov eax,cr0;     
		and eax,not 10000H;
		mov cr0,eax;
	}
	RtlCopyMemory(ObReferenceObjectByHandle,Code,5);
	__asm
	{
		mov eax, cr0;
		or eax, 10000H
		mov cr0, eax
		sti;
	}
	KeLowerIrql(oldIrql);
}

NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING szReg)
{
	driver->DriverUnload = Unload;
	Hook();
	return STATUS_SUCCESS;
}
