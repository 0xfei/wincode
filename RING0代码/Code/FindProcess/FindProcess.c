#include<ntddk.h>

#define  EPROCESS_SIZE       0x25C
#define  PEB_OFFSET          0x1B0
#define  FILE_NAME_OFFSET    0x174
#define  PROCESS_LINK_OFFSET 0x088
#define  PROCESS_ID_OFFSET   0x084
#define  EXIT_TIME_OFFSET    0x078
#define  OBJECT_HEADER_SIZE  0x018
#define  OBJECT_TYPE_OFFSET  0x008

#define PDE_INVALID 2 
#define PTE_INVALID 1 
#define VALID 0 

ULONG     pebAddress;
PEPROCESS pSystem;
ULONG     pObjectTypeProcess;

BOOLEAN IsaRealProcess(ULONG i);
VOID    WorkThread(IN PVOID pContext);
ULONG   GetPebAddress();
VOID    EnumProcess();
VOID    ShowProcess(ULONG pEProcess);

void OnUnload(IN PDRIVER_OBJECT DriverObject)
{
	DbgPrint("Unload!\n");
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject,IN PUNICODE_STRING RegistryPath)
{
	HANDLE hThread;
	DriverObject->DriverUnload = OnUnload;
	
	pSystem    = PsGetCurrentProcess();
	pebAddress = GetPebAddress();
	pObjectTypeProcess = *(PULONG)((ULONG)pSystem - OBJECT_HEADER_SIZE + OBJECT_TYPE_OFFSET);
	
	PsCreateSystemThread(
		&hThread,
		0, 
		NULL,
		0,
		NULL,
		WorkThread,
		NULL );
	return STATUS_SUCCESS;
}

VOID WorkThread(IN PVOID pContext) 
{
	EnumProcess();
	PsTerminateSystemThread(STATUS_SUCCESS);  
}

ULONG  GetPebAddress()
{
	ULONG Address;
	PEPROCESS pEProcess;
	pEProcess = (PEPROCESS)((ULONG)((PLIST_ENTRY)((ULONG)pSystem + PROCESS_LINK_OFFSET))->Flink - PROCESS_LINK_OFFSET);
	Address = *(PULONG)((ULONG)pEProcess + PEB_OFFSET);
	return Address & 0xFFFF0000;
}

VOID EnumProcess()
{
	ULONG uSystemAddress = (ULONG)pSystem;
	ULONG i;
	ULONG Address;
	ULONG ret;
	
	DbgPrint("-------------------------------------------");
	DbgPrint("EProcess    PID    ImageFileName");
	DbgPrint("---------------------------------");
	
	for(i = 0x80000000; i < uSystemAddress; i += 4)
	{
		
		if (MmIsAddressValid((PULONG)i))
		{
			Address = *(PULONG)i;
			if (( Address & 0xFFFF0000) == pebAddress)
			{
				if(IsaRealProcess(i))
				{
					ShowProcess(i - PEB_OFFSET);
					i += EPROCESS_SIZE;
				}
			}
		}
		else
		{
			i -= 4;
			i += 0x1000;
		}
	}
	ShowProcess(uSystemAddress);
	DbgPrint("-------------------------------------------");
}

void ShowProcess(ULONG pEProcess)
{
	PLARGE_INTEGER ExitTime;
	ULONG PID;
	PUCHAR pFileName;
	
	ExitTime = (PLARGE_INTEGER)(pEProcess + EXIT_TIME_OFFSET);  
	if(ExitTime->QuadPart != 0)
		return;
	PID = *(PULONG)(pEProcess + PROCESS_ID_OFFSET);
	pFileName = (PUCHAR)(pEProcess + FILE_NAME_OFFSET);
	DbgPrint("0x%08X  %04d   %s",pEProcess,PID,pFileName);
}

BOOLEAN IsaRealProcess(ULONG i) 
{
	NTSTATUS STATUS; 
	PUNICODE_STRING pUnicode; 
	UNICODE_STRING Process; 
	ULONG pObjectType;
	ULONG ObjectTypeAddress;
	
	if (!MmIsAddressValid((PULONG)(i-PEB_OFFSET)))
		return FALSE;
	ObjectTypeAddress = i - PEB_OFFSET - OBJECT_HEADER_SIZE + OBJECT_TYPE_OFFSET;
	
	if (MmIsAddressValid((PULONG)ObjectTypeAddress))
		pObjectType = *(PULONG)ObjectTypeAddress;
	else
		return FALSE;
	
	return (pObjectTypeProcess==pObjectType);
} 