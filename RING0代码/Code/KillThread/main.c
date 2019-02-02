#include "ntddk.h"
#include "LDasm.h"

typedef enum _KAPC_ENVIRONMENT {
	OriginalApcEnvironment,
	AttachedApcEnvironment,
	CurrentApcEnvironment,
	InsertApcEnvironment
} KAPC_ENVIRONMENT;

NTKERNELAPI
void
KeInitializeApc(
	PKAPC Apc,
	PETHREAD Thread,
	KAPC_ENVIRONMENT Environment,
	PKKERNEL_ROUTINE KernelRoutine,
	PKRUNDOWN_ROUTINE RundownRoutine,
	PKNORMAL_ROUTINE NormalRoutine,
	KPROCESSOR_MODE ProcessorMode,
	PVOID NormalContext );

NTKERNELAPI
BOOLEAN
KeInsertQueueApc(
	PKAPC Apc,
	PVOID SystemArgument1,
	PVOID SystemArgument2,
	KPRIORITY Increment );

#define PS_CROSS_THREAD_FLAGS_SYSTEM 0x00000010UL

ULONG GetThreadFlagsOffset()
{
	UCHAR *cPtr, *pOpcode;
	ULONG Length;
	USHORT Offset;

	for (cPtr = (PUCHAR)PsTerminateSystemThread;
		cPtr < (PUCHAR)PsTerminateSystemThread + 0x100;
		cPtr += Length)
	{
		Length = SizeOfCode(cPtr, &pOpcode);
		if (!Length) break;
		if (*(USHORT *)pOpcode == 0x80F6)
		{
			Offset = *(USHORT *)((ULONG)pOpcode + 2);
			return Offset;
		}
	}
	return 0;
}

void KernelTerminateThreadRoutine(
	IN PKAPC Apc,
	IN OUT PKNORMAL_ROUTINE *NormalRoutine,
	IN OUT PVOID *NormalContext,
	IN OUT PVOID *SystemArgument1,
	IN OUT PVOID *SystemArgument2)
{
	ULONG ThreadFlagsOffset = GetThreadFlagsOffset();
	PULONG ThreadFlags;
	DbgPrint("[TerminateThread] KernelTerminateThreadRoutine.\n");
	ExFreePool(Apc);
	if (ThreadFlagsOffset)
	{
		ThreadFlags = (ULONG *)((ULONG)(PsGetCurrentThread()) + ThreadFlagsOffset);
		*ThreadFlags |= PS_CROSS_THREAD_FLAGS_SYSTEM;
		PsTerminateSystemThread(STATUS_SUCCESS);
	}
	else
	{
		DbgPrint("cannot get thread flags offset!\n");
	}
}

BOOLEAN TerminateThread(PETHREAD Thread)
{
	PKAPC Apc = NULL;
	BOOLEAN success = FALSE;
	if (!MmIsAddressValid(Thread))
		return FALSE;
	Apc = ExAllocatePool(NonPagedPool, sizeof(KAPC));
	KeInitializeApc(
		Apc,
		Thread,
		OriginalApcEnvironment,
		KernelTerminateThreadRoutine,
		NULL,
		NULL,
		KernelMode,
		NULL);
	success = KeInsertQueueApc(Apc,
		NULL,
		NULL,
		0);
	return success;
}

VOID Unload(PDRIVER_OBJECT driver)
{
	DbgPrint("[TerminateThread] Unloaded!\n");
}

NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING szReg)
{
	DbgPrint("[TerminateThread] DriverEntry!\n");
	TerminateThread((PETHREAD)0x81982540);
	driver->DriverUnload = Unload;
	return STATUS_SUCCESS;
}