#include  <ntddk.h>
 
#include   <windef.h>
UNICODE_STRING  devNameUnicd;
UNICODE_STRING  devLinkUnicd;


#define IOCTL_PASSPROCESSID \
    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)


NTSTATUS  DeviceIoControlDispatch(IN  PDEVICE_OBJECT  pDeviceObject,IN  PIRP  pIrp)
{
	  PIO_STACK_LOCATION  irpStack;
	  NTSTATUS  Status;
	  PVOID    InPutBuffer;
	  ULONG  ioControlCode;
	  ULONG  OutPutLen;
	  PEPROCESS Process;
	  
	  ULONG IOPL = 1;
	  HANDLE hProc;
	  HANDLE *pBuff = NULL;
	  Status=STATUS_SUCCESS;
	  irpStack=IoGetCurrentIrpStackLocation(pIrp);
	  ioControlCode=irpStack->Parameters.DeviceIoControl.IoControlCode;
	  switch(irpStack->MajorFunction)
	  {
		case  IRP_MJ_CREATE:
		  DbgPrint("Call IRP_MJ_CREATE\n");
		  break;
		case  IRP_MJ_CLOSE:
		  DbgPrint("Call IRP_MJ_CLOSE\n");
		  break;
		case  IRP_MJ_DEVICE_CONTROL:
		  OutPutLen=irpStack->Parameters.DeviceIoControl.OutputBufferLength;
		  switch(ioControlCode)
		  {
			case IOCTL_PASSPROCESSID:
				{
			     _asm
			     {
			     	   mov dx, 0x64
			     	   mov al, 0xFE
			     	   out dx, al
			    }
					
				}
			  break;
			default:
			  break;
		  }
		  break;
		default:
		  DbgPrint("no match control\n");
		  break;
	  }
	  pIrp->IoStatus.Status = Status; 
	  pIrp->IoStatus.Information = 0; 
	  IoCompleteRequest (pIrp, IO_NO_INCREMENT);
	  return  Status;
}

NTSTATUS  OnUnload  (IN  PDRIVER_OBJECT  pDriverObject)
{
  NTSTATUS  Status;
  DbgPrint("Onload called\n");
  if(pDriverObject->DeviceObject!=NULL)
  {
    Status=IoDeleteSymbolicLink(&devLinkUnicd);
    if(!NT_SUCCESS(Status)) 
    {
      DbgPrint(("IoDeleteSymbolicLink() failed\n"));
      return Status;
    }
    IoDeleteDevice(pDriverObject->DeviceObject);
  }
  return  STATUS_SUCCESS;
}

NTSTATUS  DriverEntry(IN  PDRIVER_OBJECT  pDriverObject,IN  PUNICODE_STRING  pRegistryPath)
{
  NTSTATUS  Status;
  PDEVICE_OBJECT  pDevice;
  RtlInitUnicodeString(&devNameUnicd,L"\\Device\\DIRECTHADRWARE");
  RtlInitUnicodeString(&devLinkUnicd,L"\\??\\DIRECTHADRWARE");
  Status=IoCreateDevice(pDriverObject,0,&devNameUnicd,FILE_DEVICE_UNKNOWN,
      0,FALSE,&pDevice);
  if(!NT_SUCCESS(Status)) 
  {
      DbgPrint(("Can not create device.\n"));
      return Status;
  }
  
  Status=IoCreateSymbolicLink(&devLinkUnicd,&devNameUnicd);
  if(!NT_SUCCESS(Status)) 
  {
        DbgPrint(("Can not create device.\n"));
        return Status;
  }

  pDriverObject->DriverUnload=OnUnload;
  pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL]=
	      pDriverObject->MajorFunction[IRP_MJ_CREATE]=
           pDriverObject->MajorFunction[IRP_MJ_CLOSE]= DeviceIoControlDispatch;

  return  STATUS_SUCCESS;
}