#include "ntddk.h"
#include "windef.h"

typedef enum _SYSTEM_INFORMATION_CLASS {
	SystemBasicInformation,
	SystemProcessorInformation,
	SystemPerformanceInformation,
	SystemTimeOfDayInformation,
	SystemPathInformation,
	SystemProcessInformation,
	SystemCallCountInformation,
	SystemDeviceInformation,
	SystemProcessorPerformanceInformation,
	SystemFlagsInformation,
	SystemCallTimeInformation,
	SystemModuleInformation,
	SystemLocksInformation,
	SystemStackTraceInformation,
	SystemPagedPoolInformation,
	SystemNonPagedPoolInformation,
	SystemHandleInformation,
	SystemObjectInformation,
	SystemPageFileInformation,
	SystemVdmInstemulInformation,
	SystemVdmBopInformation,
	SystemFileCacheInformation,
	SystemPoolTagInformation,
	SystemInterruptInformation,
	SystemDpcBehaviorInformation,
	SystemFullMemoryInformation,
	SystemLoadGdiDriverInformation,
	SystemUnloadGdiDriverInformation,
	SystemTimeAdjustmentInformation,
	SystemSummaryMemoryInformation,
	SystemNextEventIdInformation,
	SystemEventIdsInformation,
	SystemCrashDumpInformation,
	SystemExceptionInformation,
	SystemCrashDumpStateInformation,
	SystemKernelDebuggerInformation,
	SystemContextSwitchInformation,
	SystemRegistryQuotaInformation,
	SystemExtendServiceTableInformation,
	SystemPrioritySeperation,
	SystemPlugPlayBusInformation,
	SystemDockInformation,
	SystemPowerInformation2,
	SystemProcessorSpeedInformation,
	SystemCurrentTimeZoneInformation,
	SystemLookasideInformation
} SYSTEM_INFORMATION_CLASS, *PSYSTEM_INFORMATION_CLASS;

typedef struct _SYSTEM_MODULE_INFORMATION
{
	HANDLE Section;
	PVOID MappedBase;
	PVOID Base;
	ULONG Size;
	ULONG Flags;
	USHORT Index;
	USHORT Unknown;
	USHORT LoadCount;
	USHORT ModuleNameOffset;
	CHAR ImageName[256];
} SYSTEM_MODULE_INFORMATION, *PSYSTEM_MODULE_INFORMATION;

typedef struct _SysModuleList 
{
	ULONG Count;
	SYSTEM_MODULE_INFORMATION smi[1];
} SystemModuleList, *PSystemModuleList;

NTSYSAPI NTSTATUS NTAPI ZwQuerySystemInformation(   
	IN SYSTEM_INFORMATION_CLASS SystemInformationClass,   
	IN PVOID SystemInformation,   
	IN ULONG SystemInformationLength,   
	OUT PULONG ReturnLength);

void GetBaseImageOfNtoskrnl()
{
	ULONG length = 0, i;
	PSystemModuleList info = NULL;
	PSYSTEM_MODULE_INFORMATION modules;
	ZwQuerySystemInformation(SystemModuleInformation, NULL, 0, &length);
	if (length == 0)
	{
		DbgPrint("Get length error!\n");
		return;
	}
	info = ExAllocatePool(NonPagedPool, length);
	if (info == NULL)
	{
		DbgPrint("ExAllocatePool error!\n");
		return;
	}
	ZwQuerySystemInformation(SystemModuleInformation, info, length, NULL);

	modules = (PSYSTEM_MODULE_INFORMATION)((PULONG)info + 1);
	for (i=0; i<info->Count; i++)
	{
		DbgPrint("module name %d: %s with 0x%p offset %d\n", i, modules->ImageName, modules->Base, modules->ModuleNameOffset);
		modules++;
	}
	ExFreePool(info);
}

void Unload(PDRIVER_OBJECT driver)
{
	DbgPrint("Unload!\n");
}

NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING szReg)
{
	DbgPrint("DriverEntry!\n");

	GetBaseImageOfNtoskrnl();
	driver->DriverUnload = Unload;
	return STATUS_SUCCESS;
}