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

ULONG __stdcall check(PULONG arg)//获得指向服务参数指针
{

	HANDLE hand=0;
	PFILE_OBJECT file=0;
	POBJECT_HANDLE_INFORMATION info=0;
	ULONG a;
	char* buff;
	ANSI_STRING str; 
	LARGE_INTEGER li;
	li.QuadPart=-10000;
	
	if (( arg[4]&0xf0 ) == 0 ) return 1;//检测标志
	if (( arg[5]&0x01000000 ) == 0 ) return 1;//检测属性

	hand=(HANDLE)arg[6];//获得执行文件句柄
	ObReferenceObjectByHandle(hand,0,0,KernelMode,&file,info);//&file获得对象体指针
	if (!file) return 1;
	RtlUnicodeStringToAnsiString(&str,&file->FileName,1);
	a=str.Length;
	buff=str.Buffer;
	
	while(1)//通过循环判断是不是有". "标志
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
	}//判断是否为可执行文件

	KeWaitForSingleObject(&event,Executive,KernelMode,0,0);//将当前线程置于等待状态知道信号态
	strcpy(&output[8],buff);//将string复制进buff
	RtlFreeAnsiString(&str);
	a=1;
	memmove(&output[0],&a,4);
	while(1)
	{	
		KeDelayExecutionThread(KernelMode,0,&li);//在时间间隔内当前线程处于等待状态
		memmove(&a,&output[0],4);
		if (!a) break;
	}
	memmove(&a,&output[4],4);
	KeSetEvent(&event,0,0);
	return a;
}



//保存执行文件上下文，调用check()函数
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
		cmp eax,1//由check()的返回值判断是否让可执行文件继续执行
		jne block
		popad
		popfd
		jmp RealCallee//通过，调用ntcreatesection
	block:
		popad
		mov ebx, dword ptr[esp+8]
		mov dword ptr[ebx],0
		mov eax,0xC0000022L//不让其通过，返回STATUS_ACCESS_DENIED
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
		memmove(&Index,buff,4);//所有的调用都是从ntdll.dll以一条五字节指令MOV EAX, ServiceIndex开始，四字节是索引号
		a=4*Index+(ULONG)KeServiceDescriptorTable->ServiceTable;//从用户程序中得到索引号，a指向服务函数地址
		base=(ULONG)MmMapIoSpace(MmGetPhysicalAddress((void*)a),4,0);//将物理地址映射到非分页池，因此可以进行读写，减少读写服务表保护属性的麻烦
		a=(ULONG)&Proxy;//a指向Proxy函数的地址
	
		_asm
		{
			mov eax,base
			mov ebx,dword ptr[eax]
			mov RealCallee,ebx//将原服务函数地址保存在全局变量中
			mov ebx,a
			mov dword ptr[eax],ebx//Proxy函数地址写进服务函数表中
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