#include "ntddk.h"

struct SYS_SERVICE_TABLE { 
	void **ServiceTable; 
	unsigned long CounterTable; 
	unsigned long ServiceLimit; 
	void **ArgumentsTable; 
}; 

const WCHAR devicename[]=L"\\Device\\Protector";
const WCHAR devicelink[]=L"\\DosDevices\\PROTECTOR";

KEVENT event; 

ULONG Index,RealCallee;
char* output;
extern struct SYS_SERVICE_TABLE *KeServiceDescriptorTable; 

ULONG __stdcall check(PULONG arg)//���ָ��������ָ��
{

	HANDLE hand=0;
	PFILE_OBJECT file=0;
	POBJECT_HANDLE_INFORMATION info=0;
	ULONG a;
	char* buff;
	ANSI_STRING str; 
	LARGE_INTEGER li;
	li.QuadPart=-10000;
	
	if (( arg[4]&0xf0 ) == 0 ) return 1;//����־
	if (( arg[5]&0x01000000 ) == 0 ) return 1;//�������

	hand=(HANDLE)arg[6];//���ִ���ļ����
	ObReferenceObjectByHandle(hand,0,0,KernelMode,&file,info);//&file��ö�����ָ��
	if (!file) return 1;
	RtlUnicodeStringToAnsiString(&str,&file->FileName,1);
	a=str.Length;
	buff=str.Buffer;
	
	while(1)//ͨ��ѭ���ж��ǲ�����". "��־
	{
		if(buff[a]=='.')
		{
			a++;
			break;
		}
		a--;
	}
	ObDereferenceObject(file);

	if(_stricmp(&buff[a],"exe"))
	{
		RtlFreeAnsiString(&str);
		return 1;
	}//�ж��Ƿ�Ϊ��ִ���ļ�

	KeWaitForSingleObject(&event,Executive,KernelMode,0,0);//����ǰ�߳����ڵȴ�״̬֪���ź�̬
	strcpy(&output[8],buff);//��string���ƽ�buff
	RtlFreeAnsiString(&str);
	a=1;
	memmove(&output[0],&a,4);
	while(1)
	{	
		KeDelayExecutionThread(KernelMode,0,&li);//��ʱ�����ڵ�ǰ�̴߳��ڵȴ�״̬
		memmove(&a,&output[0],4);
		if (!a) break;
	}
	memmove(&a,&output[4],4);
	KeSetEvent(&event,0,0);
	return a;
}



//����ִ���ļ������ģ�����check()����
_declspec(naked) Proxy()
{
	_asm
	{
		pushfd
		pushad
		mov ebx,esp
		add ebx,40
		push ebx
		call check
		cmp eax,1//��check()�ķ���ֵ�ж��Ƿ��ÿ�ִ���ļ�����ִ��
		jne block
		popad
		popfd
		jmp RealCallee//ͨ��������ntcreatesection
	block:
		popad
		mov ebx, dword ptr[esp+8]
		mov dword ptr[ebx],0
		mov eax,0xC0000022L//������ͨ��������STATUS_ACCESS_DENIED
		popfd
		ret 32
	}
}



NTSTATUS DrvDispatch(IN PDEVICE_OBJECT device,IN PIRP Irp)
{
	UCHAR*buff=0; 
	ULONG a,base;

	PIO_STACK_LOCATION loc = IoGetCurrentIrpStackLocation(Irp);
	if (loc->Parameters.DeviceIoControl.IoControlCode==1000)
	{
		buff = (UCHAR*)Irp->AssociatedIrp.SystemBuffer;
		// hook service dispatch table
		memmove(&Index,buff,4);//���еĵ��ö��Ǵ�ntdll.dll��һ�����ֽ�ָ��MOV EAX, ServiceIndex��ʼ�����ֽ���������
		a=4*Index+(ULONG)KeServiceDescriptorTable->ServiceTable;//���û������еõ������ţ�aָ���������ַ
		base=(ULONG)MmMapIoSpace(MmGetPhysicalAddress((void*)a),4,0);//�������ַӳ�䵽�Ƿ�ҳ�أ���˿��Խ��ж�д�����ٶ�д����������Ե��鷳
		a=(ULONG)&Proxy;//aָ��Proxy�����ĵ�ַ
	
		_asm
		{
			mov eax,base
			mov ebx,dword ptr[eax]
			mov RealCallee,ebx//��ԭ��������ַ������ȫ�ֱ�����
			mov ebx,a
			mov dword ptr[eax],ebx//Proxy������ַд������������
		}

		memmove(&a,&buff[4],4);
		output=(char*)MmMapIoSpace(MmGetPhysicalAddress((void*)a),256,0);
	}
	
	Irp->IoStatus.Status=0;
	IoCompleteRequest(Irp,IO_NO_INCREMENT);
	return 0;
}

NTSTATUS DrvCreateClose(IN PDEVICE_OBJECT device,IN PIRP Irp)
{
	Irp->IoStatus.Information=0;
	Irp->IoStatus.Status=0;
	IoCompleteRequest(Irp,IO_NO_INCREMENT);
	return 0;
}

void DrvUnload(IN PDRIVER_OBJECT driver)
{
	UNICODE_STRING devlink;
	ULONG a,base;
	//unhook dispatch table
	a = 4*Index+(ULONG)KeServiceDescriptorTable->ServiceTable;
	base = (ULONG)MmMapIoSpace(MmGetPhysicalAddress((void*)a),4,0);
	_asm
	{
		mov eax,base
		mov ebx,RealCallee
		mov dword ptr[eax],ebx
	}

	//MmUnmapIoSpace(base,4);
	MmUnmapIoSpace(output,256);
	RtlInitUnicodeString(&devlink,devicelink);
	IoDeleteSymbolicLink(&devlink);
	IoDeleteDevice(driver->DeviceObject);
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT driver,IN PUNICODE_STRING path)
{
	PDEVICE_OBJECT devobject=0;
	UNICODE_STRING devlink,devname;
	ULONG a,b;

	RtlInitUnicodeString(&devname,devicename);
	RtlInitUnicodeString(&devlink,devicelink);
	
	IoCreateDevice(driver,256,&devname,FILE_DEVICE_UNKNOWN,0,TRUE,&devobject);
	IoCreateSymbolicLink(&devlink,&devname);

	driver->MajorFunction[IRP_MJ_DEVICE_CONTROL]=DrvDispatch;
	driver->MajorFunction[IRP_MJ_CREATE]=DrvCreateClose;
	driver->MajorFunction[IRP_MJ_CLOSE]=DrvCreateClose;
	driver->DriverUnload=DrvUnload;
	KeInitializeEvent(&event,SynchronizationEvent,1);
	return 0;
}