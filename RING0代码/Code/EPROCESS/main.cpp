#include "ntddk.h"

VOID Unload(IN PDRIVER_OBJECT pDriverObject)
{
	DbgPrint("Unload driver!");
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT pDriverObject, IN PUNICODE_STRING szRegisterPath)
{
	ULONG p, ObjectTable;
	int pID;
	PLIST_ENTRY ActiveProcessLinks, now;
	DbgPrint("DriverEntry!!\n");

	p = (ULONG)PsGetCurrentProcess();
	pID = *(int*)(p + 0x84);
	ObjectTable = *(PULONG)(p + 0xC4);
	ActiveProcessLinks = (PLIST_ENTRY)(p + 0x88);
	DbgPrint("PROCESS ID: %d\n", pID);
	now = ActiveProcessLinks->Flink;
	while (now != ActiveProcessLinks)
	{
		p = (ULONG)(now) - 0x88;
		pID = *(int*)(p + 0x84);
		if (pID < 0) pID = 0;
		DbgPrint("PROCESS ID: %d\n", pID);
		now = now->Flink;
	}
	
	DbgPrint("Something happend!!!!!............!\n");
	ActiveProcessLinks = (PLIST_ENTRY)(ObjectTable+0x1C);
	pID = *(int*)(ObjectTable + 0x8);
	DbgPrint("PROCESS ID: %d\n", pID);
	now = ActiveProcessLinks->Flink;
	while (now != ActiveProcessLinks)
	{
		p = (ULONG)(now) - 0x1C;
		pID = *(int*)(p + 0x8);
		if (pID < 0) pID = 0;
		DbgPrint("PROCESS ID: %d\n", pID);
		now = now->Flink;
	}
	
	pDriverObject->DriverUnload = Unload;
	return STATUS_SUCCESS;
}
