#include "ntddk.h"

NTKERNELAPI
NTSTATUS
PsLookupProcessByProcessId(
	HANDLE ProcessId,
	PEPROCESS *Process
	);

NTKERNELAPI
VOID
KeAttachProcess (
	PEPROCESS Process
	);

NTKERNELAPI
VOID
KeDetachProcess(
	);

wchar_t origiBaseName[1024];
wchar_t origiFullName[1024];

typedef struct _PEB_LDR_DATA {
	ULONG Length;
	BOOLEAN Initialized;
	PVOID SsHandle;
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
}PEB_LDR_DATA, *PPEB_LDR_DATA;


typedef struct _LDR_DATA_TABLE_ENTRY {
	LIST_ENTRY InLoadOrderLinks;
	LIST_ENTRY InMemoryOrderLinks;
	LIST_ENTRY InInitializationOrderLinks;
	PVOID DllBase;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	USHORT LoadCount;
	USHORT TlsIndex;
	union {
		LIST_ENTRY HashLinks;
		struct {
			PVOID SectionPointer;
			ULONG CheckSum;
		};
	};
	union {
		struct {
			ULONG TimeDateStamp;
		};
		struct {
			PVOID LoadedImports;
		};
	};
	struct _ACTIVATION_CONTEXT * EntryPointActivationContext;
	PVOID PatchInformation;
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;


NTSTATUS ModifyModuleName(BOOLEAN change)
{
	NTSTATUS status;
	PEPROCESS Process;
	ULONG ulPEB;
	PPEB_LDR_DATA pLdr;
	PLDR_DATA_TABLE_ENTRY pLdt = NULL;
	PLIST_ENTRY pList,pHead;
	wchar_t wszFakePath[] = L"test.com";

	status = PsLookupProcessByProcessId(
		(HANDLE)1576,
		&Process );
	if ( status!= STATUS_SUCCESS ) 
	{
		DbgPrint("PsLookupProcessByProcessId = 0x%08lX",status);
		return status;
	}
	ObDereferenceObject(Process);

	KeAttachProcess(Process);
	DbgPrint("Process : 0x%08lX. \n",Process);
	ulPEB = *(ULONG *)((ULONG)Process + 0x1B0);
	DbgPrint("PEB : 0x%08lX. \n",ulPEB);
	__try
	{
		pLdr = *(PPEB_LDR_DATA *)(ulPEB + 0x00C);
		DbgPrint("pLdr 0x%08lX. \n",pLdr);
		pHead = pLdr->InLoadOrderModuleList.Flink;
		pList = pHead;

		do 
		{
			pLdt = CONTAINING_RECORD(pList, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
			if ( pLdt->EntryPoint && pLdt->FullDllName.Buffer && 
				pLdt->BaseDllName.Buffer && pLdt->DllBase )
			{
				DbgPrint("FullDllName : %S \n",pLdt->FullDllName.Buffer);
				DbgPrint("BaseDllName : %S \n",pLdt->BaseDllName.Buffer);
				DbgPrint("DllBase : 0x%08lX. \n",pLdt->DllBase);
				DbgPrint("DllSize : 0x%08lX. \n",pLdt->SizeOfImage);
				DbgPrint("--------------");
				if ( (ULONG)pLdt->DllBase == 0x7C800000 ) 
				{
					DbgPrint("Modify.");
					DbgPrint("--------------");
					if (change)
					{
						wcscpy(origiBaseName,pLdt->BaseDllName.Buffer);
						wcscpy(origiFullName,pLdt->FullDllName.Buffer);
						wcscpy(pLdt->FullDllName.Buffer,wszFakePath);
						wcscpy(pLdt->BaseDllName.Buffer,wszFakePath);
					}
					else
					{
						wcscpy(pLdt->FullDllName.Buffer,origiFullName);
						wcscpy(pLdt->BaseDllName.Buffer,origiBaseName);
					}
				}
			}
			pList = pList->Flink;
		} while ( pList != pHead );
	} 
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		status = GetExceptionCode();
		DbgPrint("GetExceptionCode() = 0x%08lX. \n",status );
	}
	KeDetachProcess();

	return status;
}

void Unload(PDRIVER_OBJECT driver)
{
	DbgPrint("Unload!\n");
	ModifyModuleName(FALSE);
}

NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING szReg)
{
	DbgPrint("DriverEntry!\n");
	ModifyModuleName(TRUE);
	driver->DriverUnload = Unload;
	return STATUS_SUCCESS;
}
