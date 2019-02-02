#include "ntddk.h"

typedef unsigned int DWORD;
typedef unsigned short WORD;
typedef unsigned char BYTE;
typedef DWORD* PDWORD;
typedef BYTE* PBYTE;
typedef WORD* PWORD;

PVOID* NewSystemCallTable;
PMDL MyMDL;

#define INDEX(func) *(PULONG)((PUCHAR)func + 1)
#define HOOK(func, newfunc, old) \
	old = (PVOID)InterlockedExchange( (PULONG)&NewSystemCallTable[INDEX(func)], (ULONG)newfunc)
#define UNHOOK(func, old) \
	InterlockedExchange( (PULONG)&NewSystemCallTable[INDEX(func)], (ULONG)old)

#pragma pack(1)
typedef struct _KeServiceDescriptorEntry {
	PDWORD ServiceTableEntry;
	PDWORD ServiceCounterTableBase;
	DWORD NumberOfServices;
	PBYTE  ParamTableBase;
} ServiceDescriptorTableEntry_t, *PServiceDescriptorTableEntry_t;
#pragma pack()
__declspec(dllimport) ServiceDescriptorTableEntry_t KeServiceDescriptorTable;

typedef struct _FILE_BOTH_DIR_INFORMATION {
	ULONG           NextEntryOffset;
	ULONG           FileIndex;
	LARGE_INTEGER   CreationTime;
	LARGE_INTEGER   LastAccessTime;
	LARGE_INTEGER   LastWriteTime;
	LARGE_INTEGER   ChangeTime;
	LARGE_INTEGER   EndOfFile;
	LARGE_INTEGER   AllocationSize;
	ULONG           FileAttributes;
	ULONG           FileNameLength;
	ULONG           EaSize;
	CCHAR           ShortNameLength;
	WCHAR           ShortName[12];
	WCHAR           FileName[1];
} FILE_BOTH_DIR_INFORMATION, *PFILE_BOTH_DIR_INFORMATION;

NTSYSAPI
NTSTATUS
NTAPI ZwQueryDirectoryFile(
	IN  HANDLE FileHandle,
	IN  HANDLE Event OPTIONAL,
	IN  PIO_APC_ROUTINE ApcRoutine OPTIONAL,
	IN  PVOID ApcContext OPTIONAL,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	OUT PVOID FileInformation,
	IN  ULONG Length,
	IN  FILE_INFORMATION_CLASS FileInformationClass,
	IN  BOOLEAN ReturnSingleEntry,
	IN  PUNICODE_STRING FileName OPTIONAL,
	IN  BOOLEAN RestartScan
	);

typedef NTSTATUS (*ZWQUERYDIRECTORYFILE)(
	IN  HANDLE FileHandle,
	IN  HANDLE Event OPTIONAL,
	IN  PIO_APC_ROUTINE ApcRoutine OPTIONAL,
	IN  PVOID ApcContext OPTIONAL,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	OUT PVOID FileInformation,
	IN  ULONG Length,
	IN  FILE_INFORMATION_CLASS FileInformationClass,
	IN  BOOLEAN ReturnSingleEntry,
	IN  PUNICODE_STRING FileName OPTIONAL,
	IN  BOOLEAN RestartScan
);

ZWQUERYDIRECTORYFILE OldZwQueryFileDirectoryFile;

NTSTATUS NewZwQueryDirectoryFile(
	IN  HANDLE FileHandle,
	IN  HANDLE Event OPTIONAL,
	IN  PIO_APC_ROUTINE ApcRoutine OPTIONAL,
	IN  PVOID ApcContext OPTIONAL,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	OUT PVOID FileInformation,
	IN  ULONG Length,
	IN  FILE_INFORMATION_CLASS FileInformationClass,
	IN  BOOLEAN ReturnSingleEntry,
	IN  PUNICODE_STRING FileName OPTIONAL,
	IN  BOOLEAN RestartScan
)
{
	DbgPrint("New called %wZ!\n", FileName);
	return OldZwQueryFileDirectoryFile(
		FileHandle,
		Event,
		ApcRoutine,
		ApcContext,
		IoStatusBlock,
		FileInformation,
		Length,
		FileInformationClass,
		ReturnSingleEntry,
		FileName,
		RestartScan);
}

NTSTATUS Hook()
{
	MyMDL = MmCreateMdl(
		NULL,
		KeServiceDescriptorTable.ServiceTableEntry,
		KeServiceDescriptorTable.NumberOfServices * 4);
	if (MyMDL == NULL)
		return STATUS_UNSUCCESSFUL;

	MmBuildMdlForNonPagedPool(MyMDL);
	MyMDL->MdlFlags |= MDL_MAPPED_TO_SYSTEM_VA;
	NewSystemCallTable = MmMapLockedPages(MyMDL, KernelMode);
	if (!NewSystemCallTable)
		return STATUS_UNSUCCESSFUL;

	HOOK(ZwQueryDirectoryFile, NewZwQueryDirectoryFile, OldZwQueryFileDirectoryFile);
	return STATUS_SUCCESS;
}

NTSTATUS Unhook()
{
	if (NewSystemCallTable)
	{
		UNHOOK(ZwQueryDirectoryFile, OldZwQueryFileDirectoryFile);
		MmUnmapLockedPages(NewSystemCallTable, MyMDL);
		IoFreeMdl(MyMDL);
	}
	return STATUS_SUCCESS;
}

NTSTATUS Unload(PDRIVER_OBJECT driver)
{
	NTSTATUS status;
	DbgPrint("Unload!\n");
	return Unhook();
}

NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING szReg)
{
	DbgPrint("DriverEntry!\n");
	driver->DriverUnload = Unload;
	Hook();
	return STATUS_SUCCESS;
}