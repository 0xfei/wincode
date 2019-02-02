.386
.model flat, stdcall
option casemap:none

include modifyiopmbase.inc
.const
CCOUNTED_UNICODE_STRING "\\Device\\MODIFYIOPMBASE",g_usDeviceName,4
CCOUNTED_UNICODE_STRING "\\??\\MODIFYIOPMBASE",g_usSymbolicLinkName,4
.code
DispatchCreateClose proc pDeviceObject:PDEVICE_OBJECT,pIrp:PIRP
	
	mov eax,pIrp
	assume eax:ptr _IRP
	mov [eax].IoStatus.Status,STATUS_SUCCESS
	and [eax].IoStatus.Information,0
	fastcall IofCompleteRequest,pIrp,IO_NO_INCREMENT
	assume eax:nothing
	mov eax,STATUS_SUCCESS
	ret

DispatchCreateClose endp
DispatchControl proc uses esi edi pDeviceObject:PDEVICE_OBJECT,pIrp:PIRP
	LOCAL pIopm:PVOID
	LOCAL pProcess:PEPROCESS

	
	mov esi,pIrp
	assume esi:ptr _IRP
	mov [esi].IoStatus.Status,STATUS_SUCCESS
	and [esi].IoStatus.Information,0
	
	IoGetCurrentIrpStackLocation esi
	mov edi,eax
	assume edi:ptr IO_STACK_LOCATION
	.if [edi].Parameters.DeviceIoControl.IoControlCode == IOCTL_MODIFY_IOPMBASE
	    .if [edi].Parameters.DeviceIoControl.InputBufferLength == sizeof DWORD
	        invoke MmAllocateNonCachedMemory, IOPM_SIZE
	    	mov edx, [esi].AssociatedIrp.SystemBuffer
			mov edx, [edx]			
            .if eax != NULL
                mov pIopm, eax
                             
                invoke PsLookupProcessByProcessId, edx, addr pProcess
                .if eax == STATUS_SUCCESS
                   
                    invoke DbgPrint, $CTA0("PTR KPROCESS: %08X"), pProcess
                    invoke Ke386QueryIoAccessMap, 0, pIopm
                    .if al != 0
                        ; I/O access for 64h port
                        mov ecx, pIopm
                        add ecx, 64h / 8
                        mov eax, [ecx]
                        btr eax, 64h MOD 8
                        mov [ecx], eax

                        invoke Ke386SetIoAccessMap, 1, pIopm
                        .if al != 0
                            invoke Ke386IoSetAccessProcess, pProcess, 1
                            .if al != 0
                                invoke DbgPrint, $CTA0(" I/O permission is successfully given")
                            .else
                                invoke DbgPrint, $CTA0("I/O permission is failed")
                                mov [esi].IoStatus.Status, STATUS_IO_PRIVILEGE_FAILED
                            .endif
                        .else
                            mov [esi].IoStatus.Status, STATUS_IO_PRIVILEGE_FAILED
                        .endif
                    .else
                        mov [esi].IoStatus.Status, STATUS_IO_PRIVILEGE_FAILED
                    .endif
                    invoke ObDereferenceObject, pProcess
                .else
                    mov [esi].IoStatus.Status, STATUS_OBJECT_TYPE_MISMATCH
                .endif
                invoke MmFreeNonCachedMemory, pIopm, IOPM_SIZE
            .endif
	    .endif
	.else
		mov [esi].IoStatus.Status, STATUS_INVALID_DEVICE_REQUEST
	.endif
	
	push [esi].IoStatus.Status
	assume edi:nothing
	assume esi:nothing
	fastcall IofCompleteRequest,esi,IO_NO_INCREMENT
	pop eax
	ret

DispatchControl endp
DriverUnload proc pDriverObject:PDRIVER_OBJECT
	
	invoke IoDeleteSymbolicLink,addr g_usSymbolicLinkName
	mov eax,pDriverObject
	invoke IoDeleteDevice,(DRIVER_OBJECT PTR[eax]).DeviceObject
	ret

DriverUnload endp

DriverEntry proc pDriverObject:PDRIVER_OBJECT,pusRegistryPath:PUNICODE_STRING

    LOCAL status:NTSTATUS
    LOCAL pDeviceObject:PDEVICE_OBJECT
    
    mov status,STATUS_DEVICE_CONFIGURATION_ERROR
    invoke IoCreateDevice,pDriverObject,0,addr g_usDeviceName,FILE_DEVICE_UNKNOWN,0,FALSE,addr pDeviceObject
    .if eax == STATUS_SUCCESS
    	invoke IoCreateSymbolicLink,addr g_usSymbolicLinkName,addr g_usDeviceName
    	.if eax == STATUS_SUCCESS
    		mov eax,pDriverObject
    		assume eax:ptr DRIVER_OBJECT
    		
    		mov [eax].MajorFunction[IRP_MJ_CREATE * (sizeof PVOID)],offset DispatchCreateClose
    		mov [eax].MajorFunction[IRP_MJ_CLOSE *(sizeof PVOID)],offset DispatchCreateClose
    		mov [eax].MajorFunction[IRP_MJ_DEVICE_CONTROL *(sizeof PVOID)],offset DispatchControl
    		mov [eax].DriverUnload,offset DriverUnload
    		assume eax:nothing
    		mov status,STATUS_SUCCESS
    	.else
    		invoke IoDeleteDevice,pDeviceObject	
    	.endif
    .endif
    mov eax,status
    ret

DriverEntry endp

end DriverEntry
