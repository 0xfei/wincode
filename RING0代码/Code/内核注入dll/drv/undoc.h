#ifndef _UNDOC_H
#define _UNDOC_H

#define SEC_IMAGE   0x1000000 

typedef enum _KAPC_ENVIRONMENT {
    OriginalApcEnvironment,
    AttachedApcEnvironment,
    CurrentApcEnvironment
} KAPC_ENVIRONMENT;

NTSYSAPI
    VOID
    KeInitializeApc (
    IN PRKAPC Apc,
    IN PRKTHREAD Thread,
    IN KAPC_ENVIRONMENT Environment,
    IN PKKERNEL_ROUTINE KernelRoutine,
    IN PKRUNDOWN_ROUTINE RundownRoutine OPTIONAL,
    IN PKNORMAL_ROUTINE NormalRoutine OPTIONAL,
    IN KPROCESSOR_MODE ApcMode OPTIONAL,
    IN PVOID NormalContext OPTIONAL
    );

NTSYSAPI
    BOOLEAN
    KeInsertQueueApc (
    IN PRKAPC Apc,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2,
    IN KPRIORITY Increment
    );

typedef union _WOW64_APC_CONTEXT {
    struct {
        ULONG Apc32BitContext;
        ULONG Apc32BitRoutine;
    };
    PVOID Apc64BitContext;
} WOW64_APC_CONTEXT;


NTSYSAPI 
    NTSTATUS 
    NTAPI
    ZwQueryInformationProcess(
    IN HANDLE ProcessHandle,
    IN PROCESSINFOCLASS ProcessInformationClass,
    OUT PVOID ProcessInformation,
    IN ULONG ProcessInformationLength,
    OUT PULONG ReturnLength OPTIONAL
    );

NTSYSAPI
    NTSTATUS
    NTAPI
    ZwAllocateVirtualMemory(
    __in HANDLE ProcessHandle,
    __inout PVOID *BaseAddress,
    __in ULONG_PTR ZeroBits,
    __inout PSIZE_T RegionSize,
    __in ULONG AllocationType,
    __in ULONG Protect
    );

NTSYSAPI
    BOOLEAN
    PsIsThreadTerminating(
    __in PETHREAD Thread
    );

typedef struct _RTL_PROCESS_MODULE_INFORMATION {
    HANDLE Section;                 // Not filled in
    PVOID MappedBase;
    PVOID ImageBase;
    ULONG ImageSize;
    ULONG Flags;
    USHORT LoadOrderIndex;
    USHORT InitOrderIndex;
    USHORT LoadCount;
    USHORT OffsetToFileName;
    UCHAR  FullPathName[ 256 ];
} RTL_PROCESS_MODULE_INFORMATION, *PRTL_PROCESS_MODULE_INFORMATION;

typedef struct _RTL_PROCESS_MODULES {
    ULONG NumberOfModules;
    RTL_PROCESS_MODULE_INFORMATION Modules[ 1 ];
} RTL_PROCESS_MODULES, *PRTL_PROCESS_MODULES;

#define SystemModuleInformation     11
NTSTATUS ZwQuerySystemInformation(ULONG SystemInformationClass,PVOID SystemInformation,ULONG SystemInformationLength,PULONG ReturnLength);

typedef struct _KSERVICE_TABLE_DESCRIPTOR {
    PULONG Base;
    PULONG Count;
    ULONG Limit;
    PCHAR Number;
} KSERVICE_TABLE_DESCRIPTOR, *PKSERVICE_TABLE_DESCRIPTOR;

ULONG_PTR ReadMSR(ULONG dwIndex);

typedef NTSTATUS (*PNT_PROTECT_VIRTUAL_MEMORY)(
    IN HANDLE               ProcessHandle,
    IN OUT PVOID            *BaseAddress,
    IN OUT PULONG           NumberOfBytesToProtect,
    IN ULONG                NewAccessProtection,
    OUT PULONG              OldAccessProtection
    );

#endif
