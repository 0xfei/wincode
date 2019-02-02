#include <ntifs.h>

typedef struct _EX_QUEUE_WORKER_INFO
{
	ULONG QueueDisabled:1;
	ULONG MakeThreadsAsNecessary:1;
	ULONG WaitMode:1;
	ULONG WorkerCount:29;
} EX_QUEUE_WORKER_INFO, *PEX_QUEUE_WORKER_INFO;

typedef struct _EX_WORK_QUEUE
{
	KQUEUE WorkerQueue;
	LONG DynamicThreadCount;
	ULONG WorkItemsProcessed;
	ULONG WorkItemsProcessedLastPass;
	ULONG QueueDepthLastPass;
	EX_QUEUE_WORKER_INFO Info;
} EX_WORK_QUEUE, *PEX_WORK_QUEUE;

PEX_WORK_QUEUE g_ExWorkerQueue = NULL;
WORK_QUEUE_ITEM Item;

VOID Unload(PDRIVER_OBJECT Driver)
{
	DbgPrint("Unload!\n");
}


PVOID GetExWorkerQueue(PVOID Address)
{
	PUCHAR Index;
	PVOID Answer = NULL;

	for (Index = (PUCHAR)Address; MmIsAddressValid(Address); Index++)
	{
		if (*Index == 0x81 && *(Index+1) == 0xc6)
		{
			Answer = *(PVOID*)(Index+2);
			break;
		}
	}
	return Answer;
}

VOID Test(PVOID Data)
{
	DbgPrint("Test!!!\n");
}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING szUniString)
{
	DbgPrint("DriverEntry\n");

	g_ExWorkerQueue = (PEX_WORK_QUEUE)GetExWorkerQueue((PVOID)ExQueueWorkItem);
	DbgPrint("ExWorkerQueue: 0x%08x\n", g_ExWorkerQueue);

	ExInitializeWorkItem(&Item, Test, NULL);
	ExQueueWorkItem(&Item, DelayedWorkQueue);

	DriverObject->DriverUnload = Unload;

	return STATUS_SUCCESS;
}
