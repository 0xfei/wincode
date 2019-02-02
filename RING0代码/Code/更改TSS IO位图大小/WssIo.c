/*****************************************************************
�ļ���        : WssIo.c
����          : ����˿ڶ�д
����          : sinister
����޸�����  : 2002-11-02
*****************************************************************/

#include "ntddk.h"
#include "string.h"

#ifndef DWORD
#define DWORD unsigned int
#endif

#ifndef WORD
#define WORD unsigned short
#endif

#define NOT_BUSY            9
#define DEFAULT_TSS_LIMIT   0x20ab  //ȱʡ�� TSS �ν���
#define NEW_TSS_LIMIT       0x2fff //0x2fab

static NTSTATUS  MydrvDispatch (IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
VOID DriverUnload (IN PDRIVER_OBJECT pDriverObject);

#pragma pack(push,1)


typedef struct tagGDTR{
    WORD    wLimit;
    DWORD   *dwBase;
}GDTR, *PGDTR;

typedef struct tagGDTENTRY{
    DWORD dwLimit : 16;
    DWORD dwBaselo : 16;
    DWORD dwBasemid : 8;
    DWORD dwType : 4;
    DWORD dwSystem : 1;
    DWORD dwDpl : 2;
    DWORD dwPresent : 1;
    DWORD dwLimithi : 4;
    DWORD dwAvailable : 1;
    DWORD dwZero : 1;
    DWORD dwSize : 1;
    DWORD dwGranularity : 1;
    DWORD dwBasehi : 8;
} GDTENTRY, *PGDTENTRY;

#pragma pack(pop)

VOID SetAllPorcIO( int iTrue )
{
    GDTR        gdtr;
    PGDTENTRY    gdt;

    WORD        TSSseg;


    __asm {
        cli                            // �����ж�
        sgdt  gdtr                    // �õ� GDT ����ַ��ν���
        str   TSSseg                // �õ� TSS ѡ����
        movzx esi,TSSseg            // ��չ�� ESI ���Ա����
        add   esi,gdtr.dwBase       // �õ� TSS �� GDT ��������
        mov   gdt,esi
    }

    if ( iTrue == 1 )
    {
        gdt->dwLimit = NEW_TSS_LIMIT;    // �����µ� TSS �ν���
    }

    else {
        gdt->dwLimit = DEFAULT_TSS_LIMIT;
    }

    gdt->dwType = NOT_BUSY;            //��������ΪNOTBUSY״̬

    __asm {
        ltr TSSseg                // �����úõ� TSS ����װ��
        sti                        // ���ж�
    }


}


// �������
NTSTATUS  DriverEntry( IN PDRIVER_OBJECT DriverObject,  IN PUNICODE_STRING RegistryPath )
{
    
    UNICODE_STRING  nameString, linkString;
    PDEVICE_OBJECT  deviceObject;
    NTSTATUS        status;
    HANDLE          hHandle;
    int                i;
    

    //ж������
    DriverObject->DriverUnload = DriverUnload;

    //�����豸
    RtlInitUnicodeString( &nameString, L"\\Device\\WssIo" );
    
    status = IoCreateDevice( DriverObject,
                             0,
                             &nameString,
                             FILE_DEVICE_UNKNOWN,
                             0,
                             TRUE,
                             &deviceObject
                           );
                          

    if (!NT_SUCCESS( status ))
        return status;
    

    RtlInitUnicodeString( &linkString, L"\\DosDevices\\WssIo" );

    status = IoCreateSymbolicLink (&linkString, &nameString);

    if (!NT_SUCCESS( status ))
    {
        IoDeleteDevice (DriverObject->DeviceObject);
        return status;
    }    
    
    SetAllPorcIO(1);

    for ( i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++)    {

          DriverObject->MajorFunction[i] = MydrvDispatch;
    }

      DriverObject->DriverUnload = DriverUnload;
    
  return STATUS_SUCCESS;
}



//�����豸�������

static NTSTATUS MydrvDispatch (IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0L;
    IoCompleteRequest( Irp, 0 );
    return Irp->IoStatus.Status;
    
}



VOID DriverUnload (IN PDRIVER_OBJECT    pDriverObject)
{
    UNICODE_STRING  nameString;

    SetAllPorcIO(0);

    RtlInitUnicodeString( &nameString, L"\\DosDevices\\WssIo" );    
    IoDeleteSymbolicLink(&nameString);
    IoDeleteDevice(pDriverObject->DeviceObject);

    return;
}