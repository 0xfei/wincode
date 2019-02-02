#include "ntddk.h"
#include "pe.h"
#include "ntifs.h"

DWORD UsersharedMemory = 0x7FFE0800;
DWORD KerlsharedMemory = 0xFFDF0800;

char temp[] = {
	0x90,
	0xb8, 0xaa,0xaa, 0xaa, 0xaa,
	0xff, 0xe0
};
char s[256];

NTSTATUS Hook(PIMAGE_DOS_HEADER base, HANDLE dwProcessID)
{
	PIMAGE_DOS_HEADER dosHeader;
	PIMAGE_NT_HEADERS pNTHeader;
	PIMAGE_IMPORT_DESCRIPTOR pIID;
	PIMAGE_IMPORT_BY_NAME pImportByName;
	DWORD RVA;
	PDWORD pIAT, pINT;

	int count, index;
	char *dllName = NULL;
	char *dllTarget = "kernel32.dll";
	char *funcTarget = "GetProcAddress";
	PMDL MyMDL;
	PDWORD MappedTable;
	
	dosHeader = base;
	pNTHeader = (PIMAGE_NT_HEADERS)(dosHeader->e_lfanew + (DWORD)base);

	if (pNTHeader->Signature != IMAGE_NT_SIGNATURE)
		return STATUS_INVALID_IMAGE_FORMAT;

	RVA = pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
	pIID = (PIMAGE_IMPORT_DESCRIPTOR)(RVA + (DWORD)base);

	for (count = 0; pIID[count].Characteristics!=0; count++)
	{
		dllName = (char *)(pIID[count].Name + (DWORD)base);
		pIAT = (PDWORD)((DWORD)base + (DWORD)pIID[count].FirstThunk);
		pINT = (PDWORD)((DWORD)base + (DWORD)pIID[count].OriginalFirstThunk);
		for (index=0; pIAT[index]!=0; index++)
		{
			if ((pINT[index] & IMAGE_ORDINAL_FLAG) != IMAGE_ORDINAL_FLAG)
			{
				pImportByName = (PIMAGE_IMPORT_BY_NAME)(pINT[index] + (DWORD)base);
				if ((_stricmp(dllName, dllTarget) == 0) &&
					(_stricmp(pImportByName->Name, funcTarget) == 0))
				{
					MyMDL = MmCreateMdl(NULL, &pIAT[index], 4);
					if (!MyMDL) return STATUS_UNSUCCESSFUL;
					MmBuildMdlForNonPagedPool(MyMDL);
					MyMDL->MdlFlags |= MDL_MAPPED_TO_SYSTEM_VA;
					MappedTable = MmMapLockedPages(MyMDL, KernelMode);
					RtlCopyMemory((PVOID)KerlsharedMemory, temp, 8);
					RtlCopyMemory((PVOID)(KerlsharedMemory+2), (PVOID)&pIAT[index], 4);
					*MappedTable = UsersharedMemory;
					MmUnmapLockedPages(MappedTable,MyMDL);
					IoFreeMdl(MyMDL);
				}
			}
		}
	}
	return STATUS_SUCCESS;
}


NTSTATUS HookStart(HANDLE dwProcessID)
{
	ULONG pEProcess;
	PLIST_ENTRY pCurrentList = NULL, pTempList = NULL, pLoadOrderModuleList, list;
	PPEB pPeb = NULL;
	ULONG hModule, temp;
	PsLookupProcessByProcessId(dwProcessID,(PEPROCESS*)&pEProcess);
	pPeb = (PPEB)(*(PULONG)(pEProcess + PEBOFFSET));
	if(pPeb != NULL)
	{
		KeAttachProcess((PRKPROCESS)pEProcess);
		pLoadOrderModuleList = pPeb->LoaderData->InLoadOrderModuleList.Flink;
		list = pLoadOrderModuleList;
		do
		{
			UNICODE_STRING pstrTemp = ((PLDR_MODULE)list)->FullDllName;
			DbgPrint("module name = %ws\n\n\n\n",pstrTemp.Buffer);
			if(wcsstr(pstrTemp.Buffer,L".exe") != NULL)	
			{
				hModule = (ULONG)((PLDR_MODULE)list)->BaseAddress;
				temp = *(PULONG)hModule; 
				DbgPrint("Find Module baseAaddress = %x\n\n\n",hModule);
				Hook((PIMAGE_DOS_HEADER)hModule,dwProcessID);
				break;
			}
			list = list->Flink;
		} while(list != pLoadOrderModuleList);
		KeDetachProcess();
	}
	return STATUS_SUCCESS;
}


void MyFunc(IN PUNICODE_STRING dllName,
			IN HANDLE hProcess,
			IN PIMAGE_INFO pImageInfo)
{
	UNICODE_STRING targetDll;
	RtlInitUnicodeString(&targetDll, L"\\WINDOWS\\system32\\kernel32.dll");
	if (RtlCompareUnicodeString(dllName, &targetDll, FALSE) == 0)
	{
		DbgPrint(" imageInfo->ImageBase:%x  ProcessId : %d\n", pImageInfo->ImageBase, hProcess);
		HookStart(hProcess);
	}
}

void Unload(PDRIVER_OBJECT driver)
{

	DbgPrint("Unload\n");
}

NTSTATUS DriverEntry(PDRIVER_OBJECT driver, PUNICODE_STRING szReg)
{
	DbgPrint("DriverEntry\n");
	driver->DriverUnload = Unload;
	PsSetLoadImageNotifyRoutine((PLOAD_IMAGE_NOTIFY_ROUTINE)MyFunc);
	return STATUS_SUCCESS;
}

