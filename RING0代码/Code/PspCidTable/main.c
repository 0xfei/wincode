#include <ntddk.h>

typedef struct _OBJECT_HEADER {
    union {
        struct {
            LONG PointerCount;
            LONG HandleCount;
        };
        LIST_ENTRY Entry;
    };
    POBJECT_TYPE Type;
    UCHAR NameInfoOffset;
    UCHAR HandleInfoOffset;
    UCHAR QuotaInfoOffset;
    UCHAR Flags;

    union {
        PVOID QuotaBlockCharged;
    };

    PSECURITY_DESCRIPTOR SecurityDescriptor;

    QUAD Body;
} OBJECT_HEADER, *POBJECT_HEADER;

#define OBJECT_TO_OBJECT_HEADER(obj) \
CONTAINING_RECORD( (obj), OBJECT_HEADER, Body )

ULONG GetPspCidTable()
{
	PUCHAR addr;
	PUCHAR p;
	UNICODE_STRING szPsLookup;
	ULONG cid;
	
	RtlInitUnicodeString(&szPsLookup,
		L"PsLookupProcessByProcessId");
	addr = (PUCHAR)MmGetSystemRoutineAddress(&szPsLookup);
	for (p=addr; p<addr+PAGE_SIZE; p++)
	{
		if ((*(PUSHORT)p==0x35ff)&&(*(p+6)==0xe8))
		{
			cid = *(PULONG)(p+2);
			return cid;
		}
	}
	return 0;
}

VOID Unload(PDRIVER_OBJECT pDriverObject)
{
	DbgPrint("Unload!\n");
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, 
	PUNICODE_STRING szReg)
{
	ULONG addr, table1, table2;
	ULONG hHandleTable, hHandleTableEntry;
	ULONG dwTableCode;
	pDriverObject->DriverUnload = Unload;
	addr = GetPspCidTable();
	
	DbgPrint("PspCidTable address: 0x%08x",addr);
	hHandleTable = *(ULONG*)(addr);
	DbgPrint("HANDLE_TABLE : 0x%08x",hHandleTable);
	dwTableCode = *(ULONG*)(hHandleTable);
	DbgPrint("TableCode: 0x%08x\n", dwTableCode);
	if ((dwTableCode & 3)==1)
	{
		table1 = *(ULONG*)dwTableCode;
		table2 = *(ULONG*)(dwTableCode+4);
	} else
	if ((dwTableCode & 3)==0)
	{
		table1 = dwTableCode;
		table2 = 0;
	}
	DbgPrint("table1: 0x%08x,  table2: 0x%08x\n",table1,table2);
	
	hHandleTableEntry = (*(ULONG*)(table1 + 8))&0xfffffff8;
	DbgPrint("SYSTEM:  0x%08x\n", hHandleTableEntry);
	return STATUS_SUCCESS;
}
