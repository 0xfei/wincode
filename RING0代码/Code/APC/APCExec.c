#include "Process.h"

const WCHAR devicename[]=L"\\Device\\MyRPC";
const WCHAR devicelink[]=L"\\??\\MyRPC";

VOID KE_Unload(PDRIVER_OBJECT  driver)
{
	UNICODE_STRING devlink;
	RtlInitUnicodeString(&devlink,devicelink);
	IoDeleteSymbolicLink(&devlink);
	IoDeleteDevice(driver->DeviceObject);
	DbgPrint("KernelExec -> Driver Unloaded");
}

NTSTATUS KE_Dispatch(PDEVICE_OBJECT DeviceObject, PIRP pIrp)
{
	PIO_STACK_LOCATION irpsp;
	NTSTATUS status = STATUS_SUCCESS;
	PUCHAR charBuffer;
	ULONG Length = 0;

	irpsp = IoGetCurrentIrpStackLocation(pIrp);
	charBuffer = (PUCHAR)pIrp->AssociatedIrp.SystemBuffer;;

	switch (irpsp->MajorFunction)
	{
	case IRP_MJ_CREATE:
		DbgPrint("IRP_MJ_CREATE");
		break;
	case IRP_MJ_CLOSE:
		DbgPrint("IRP_MJ_CLOSE");
		break;
	case IRP_MJ_DEVICE_CONTROL:
		{
			switch (irpsp->Parameters.DeviceIoControl.IoControlCode)
			{
				default:
					DbgPrint("Process : %s\n", charBuffer);
					RunProcess(charBuffer);
					break;
			}
		}
	default:
		status = STATUS_SUCCESS;
	}
	pIrp->IoStatus.Status=status;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp,IO_NO_INCREMENT);
	return 0;
}

NTSTATUS DriverEntry(PDRIVER_OBJECT  pDriverObject, PUNICODE_STRING  pRegistryPath)
{

	NTSTATUS NtStatus = STATUS_SUCCESS;
	PDEVICE_OBJECT pDeviceObject = NULL;
	UNICODE_STRING usDeviceName, usDosDeviceName;

	RtlInitUnicodeString(&usDeviceName, devicename);
	RtlInitUnicodeString(&usDosDeviceName, devicelink);

	IoCreateDevice(
		pDriverObject, 
		0, 
		&usDeviceName, 
		FILE_DEVICE_UNKNOWN, 
		0, 
		FALSE, 
		&pDeviceObject );
	IoCreateSymbolicLink(&usDosDeviceName, &usDeviceName);

	if(NtStatus == STATUS_SUCCESS)
	{
		pDriverObject->MajorFunction[IRP_MJ_CLEANUP]           = KE_Dispatch; 
		pDriverObject->MajorFunction[IRP_MJ_CLOSE]             = KE_Dispatch;
		pDriverObject->MajorFunction[IRP_MJ_CREATE]            = KE_Dispatch;
		pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL]    = KE_Dispatch;
		pDriverObject->MajorFunction[IRP_MJ_INTERNAL_DEVICE_CONTROL] = KE_Dispatch;
		pDriverObject->MajorFunction[IRP_MJ_READ]              = KE_Dispatch;
		pDriverObject->MajorFunction[IRP_MJ_WRITE]             = KE_Dispatch;
		pDriverObject->DriverUnload =  KE_Unload;

		DbgPrint("KernelExec -> Driver Loaded");
	}
	return NtStatus;
}

