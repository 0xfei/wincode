
//
// PortTalk.C
// Craig.Peacock@beyondlogic.org
// http://www.beyondlogic.org
//
// Port Talk Driver for Windows NT / Windows 2000
//
//
//
// Sunday 11th June 2000
//

#include <ntddk.h>

#define	IOPM_SIZE	0x2000

typedef UCHAR IOPM[IOPM_SIZE];

IOPM *IOPM_local = 0;

void Ke386SetIoAccessMap(int, IOPM *);
void Ke386QueryIoAccessMap(int, IOPM *);
void Ke386IoSetAccessProcess(PEPROCESS, int);

NTSTATUS
PortTalkDeviceControl(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    );

VOID PortTalkUnload(IN PDRIVER_OBJECT DriverObject);

NTSTATUS PsLookupProcessByProcessId(IN ULONG ulProcId,OUT struct _EPROCESS ** pEProcess);



NTSTATUS PortTalkCreateDispatch(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp
    )
{
	Irp->IoStatus.Information = 0;

	Irp->IoStatus.Status = STATUS_SUCCESS;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}



NTSTATUS DriverEntry(
    IN PDRIVER_OBJECT DriverObject,
    IN PUNICODE_STRING RegistryPath
    )
{
	PDEVICE_OBJECT deviceObject;

	int a;
	int b;

	NTSTATUS status;

	WCHAR NameBuffer[] = L"\\Device\\PortTalk";

	WCHAR DOSNameBuffer[] = L"\\DosDevices\\PortTalk";

	UNICODE_STRING uniNameString, uniDOSString;

	KdPrint( ("PORTTALK: Porttalk has Loaded") );

	IOPM_local = MmAllocateNonCachedMemory(sizeof(IOPM));

	if(IOPM_local == 0)
		return STATUS_INSUFFICIENT_RESOURCES;

	//RtlZeroMemory(IOPM_local, sizeof(IOPM));

	RtlFillMemory(IOPM_local, sizeof(IOPM), 0xFF);

	KdPrint( ("PORTTALK: Memory Allocated at %X\n",IOPM_local) );

	//b = 0;
	//for (a = 0; a <= 0x1FFF; a++)
	//{
	// KdPrint( ("PORTTALK: Address %X = %X\n",b,*(*IOPM_local + a) ) );
	// b += 0x08;
	//}

	RtlInitUnicodeString(&uniNameString, NameBuffer);

	RtlInitUnicodeString(&uniDOSString, DOSNameBuffer);

	status = IoCreateDevice(DriverObject, 
				0,
				&uniNameString,
				FILE_DEVICE_UNKNOWN,
				0, 
				FALSE, 
				&deviceObject);

	if(!NT_SUCCESS(status))
		return status;

	status = IoCreateSymbolicLink (&uniDOSString, &uniNameString);

	if (!NT_SUCCESS(status))
		return status;

	DriverObject->MajorFunction[IRP_MJ_CREATE] = PortTalkCreateDispatch;
	
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = PortTalkDeviceControl;
	
	DriverObject->DriverUnload = PortTalkUnload;

	return STATUS_SUCCESS;
}

NTSTATUS
PortTalkDeviceControl(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP pIrp
    )

{
    PIO_STACK_LOCATION  irpSp;

    NTSTATUS            ntStatus = STATUS_SUCCESS;// Assume success

    ULONG               inBufLength; // Input buffer length
    ULONG               outBufLength; // Output buffer length
    ULONG               inBuf; // pointer to Input and output buffer
    PUCHAR 		CharBuffer; 
    PUSHORT		ShortBuffer;
    PULONG		LongBuffer;
    PVOID 		ioBuffer;

    USHORT Offset;
    UCHAR Value;

    int a;
    int b;

    ULONG ProcessID;	

    struct _EPROCESS *Process;

    irpSp = IoGetCurrentIrpStackLocation( pIrp );

    inBufLength = irpSp->Parameters.DeviceIoControl.InputBufferLength;

    ioBuffer    = pIrp->AssociatedIrp.SystemBuffer;

    CharBuffer  = (PUCHAR) ioBuffer;
    ShortBuffer = (PUSHORT) ioBuffer;
    LongBuffer  = (PULONG) ioBuffer;

    switch ( irpSp->Parameters.DeviceIoControl.IoControlCode )

     {

      case 0x04: 

        KdPrint( ("PORTTALK: IOCTL 0x04 - Set IOPM of ProcessID") );

	ProcessID = LongBuffer[0];
	
	KdPrint( ("PORTTALK: ProcessID Received is %d\n",ProcessID) );

	PsLookupProcessByProcessId(ProcessID, &Process);

	KdPrint( ("PORTTALK: Pointer to Process is %X\n",Process) );

	KdPrint( ("PORTTALK: Address = %X\n",*(*IOPM_local + 0x6F) ) );

	Ke386SetIoAccessMap(1, IOPM_local);

	Ke386IoSetAccessProcess(Process, 1);

        pIrp->IoStatus.Information = 1;

	ntStatus = STATUS_SUCCESS;

        break;

      case 0x08:
 
	KdPrint( ("PORTTALK: IOCTL 0x08 - READ_PORT_UCHAR 0x%X",ShortBuffer[0]) );

	(UCHAR)Value = READ_PORT_UCHAR((PUCHAR)ShortBuffer[0]);

	KdPrint( ("PORTTALK: Value Read %X",Value) );

	CharBuffer[0] = Value;
	
	pIrp->IoStatus.Information = 1;
	
	ntStatus = STATUS_SUCCESS;
	
	break;

      case 0x0C:

	KdPrint( ("PORTTALK: IOCTL 0x0C - WRITE_PORT_UCHAR(0x%X,0x%X)",ShortBuffer[0], CharBuffer[2]) );
  
	WRITE_PORT_UCHAR((PUCHAR)ShortBuffer[0], CharBuffer[2]);

	pIrp->IoStatus.Information = 1;

	ntStatus = STATUS_SUCCESS;

	break;

      case 0x10: //(16)

	KdPrint( ("PORTTALK: IOCTL 0x10 - RTLFillMemory (Turn Off All Access)") );

	RtlFillMemory(IOPM_local, sizeof(IOPM), 0xFF);

	pIrp->IoStatus.Information = 1;

	ntStatus = STATUS_SUCCESS;

	break;

      case 0x14: //(20)

	KdPrint( ("PORTTALK: IOCTL 0x14 - RTLZeroMemory (Allow excusive Access)") );

	RtlZeroMemory(IOPM_local, sizeof(IOPM));

	pIrp->IoStatus.Information = 1;

	ntStatus = STATUS_SUCCESS;

	break;

      case 0x18: //(24)

	KdPrint( ("PORTTALK: IOCTL 0x18 - Set IO Permission Bitmap") );
  
	Offset = ShortBuffer[0];

	Value  = CharBuffer[2];

	KdPrint( ("PORTTALK: Offset = %X, Value = %X\n",Offset,Value) );
  
	*(*IOPM_local + Offset) = Value;

	pIrp->IoStatus.Information = 1;

	ntStatus = STATUS_SUCCESS;

	break;

    }

    pIrp->IoStatus.Status = ntStatus;

    IoCompleteRequest( pIrp, IO_NO_INCREMENT );

    return ntStatus;
}


VOID PortTalkUnload(IN PDRIVER_OBJECT DriverObject)
{
	
	WCHAR DOSNameBuffer[] = L"\\DosDevices\\PortTalk";

	UNICODE_STRING uniDOSString;

	KdPrint( ("PORTTALK: PortTalk is Unloading . .\n") );

	if(IOPM_local) MmFreeNonCachedMemory(IOPM_local, sizeof(IOPM));

	RtlInitUnicodeString(&uniDOSString, DOSNameBuffer);

	IoDeleteSymbolicLink (&uniDOSString);

	IoDeleteDevice(DriverObject->DeviceObject);
}