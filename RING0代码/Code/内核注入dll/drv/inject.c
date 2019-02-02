#include <ntifs.h>
#include <ntImage.h> 
#include "thunk.h"
#include "inject.h"

ULONG ProcessHashLen     = DEFAULT_PROCESS_HASH_LEN;

HLIST_FIRST     *ProcessHashTable;
ULONG           ProcessHashTableLen;
LIST_ENTRY      ProcessTable;
KMUTEX          ProcessMutex;
LONG            ProcessMemoryCount;
LONG            ProcessTableCount;
LONG            NonPagedPoolUsage;

PNT_PROTECT_VIRTUAL_MEMORY  NtProtectVirtualMemory;

PVOID ProcessAllocatePool(SIZE_T NumberOfBytes,ULONG FillZero)
{
    PVOID P;

    P = ExAllocatePool(NonPagedPool,NumberOfBytes);
    if(P)
    {
        InterlockedExchangeAdd(&NonPagedPoolUsage,(LONG)NumberOfBytes);
        InterlockedIncrement(&ProcessMemoryCount);          //内存计数
        if(FillZero)
            RtlZeroMemory(P,NumberOfBytes);                 //内存清0
    }
    return P;
}

VOID ProcessFreePool(PVOID P,SIZE_T NumberOfBytes)
{
    InterlockedExchangeAdd(&NonPagedPoolUsage,-(LONG)NumberOfBytes);
    InterlockedDecrement(&ProcessMemoryCount);              //内存计数
    ExFreePool(P);
}

VOID ProcessLock()
{
    KeWaitForMutexObject(&ProcessMutex,Executive,KernelMode,FALSE,NULL);
}

VOID ProcessUnlock()
{
    KeReleaseMutex(&ProcessMutex, FALSE);
}

PPROCESS_TABLE ProcessLookup(HANDLE Pid)
{
    PHLIST_FIRST pFirst = ProcessHashTable + CALC_PROCESS_HASH(Pid);
    PHLIST_ENTRY Link;
    PPROCESS_TABLE pProcess;

    if(!ProcessHashTable)
        return NULL;

    HLIST_FOR_EACH_ENTRY(pProcess,Link,pFirst,PROCESS_TABLE,HashList)
    {
        if(pProcess->Pid == Pid)
        {
            return pProcess;
        }
    }
    return NULL;
}

VOID ProcessDelete(HANDLE Pid)
{
    PPROCESS_TABLE pProcess;

    pProcess = ProcessLookup(Pid);
    if(pProcess)
    {
        ProcessTableCount--;
        RemoveEntryHList(&pProcess->HashList);
        RemoveEntryList(&pProcess->GlobalList);

        RtlFreeAnsiString(&pProcess->FullImageName);
        ProcessFreePool(pProcess,sizeof(PROCESS_TABLE));
    }
}

PPROCESS_TABLE ProcessAdd(HANDLE Pid)
{
    PHLIST_FIRST pFirst;
    PPROCESS_TABLE pProcess;

    if(!ProcessHashTable)
        return NULL;

    //判断是否已经存在
    pProcess = ProcessLookup(Pid);
    if(pProcess)
        return pProcess;

    pProcess = (PPROCESS_TABLE)ProcessAllocatePool(sizeof(PROCESS_TABLE),TRUE);
    if(!pProcess)
        return NULL; 

    pProcess->Pid = Pid;
    pProcess->FirstLoad = TRUE;
    pFirst = ProcessHashTable + CALC_PROCESS_HASH(Pid);

    //插入哈希表和全局链表
    InsertHListHead(pFirst,&pProcess->HashList);
    InsertTailList(&ProcessTable,&pProcess->GlobalList);

    ProcessTableCount++;

    return pProcess;
}


PVOID GetProcAddress(PVOID lpBase,PCHAR lpszAPIName)
{
    PIMAGE_DOS_HEADER pDosHdr;
    PIMAGE_NT_HEADERS32 pNtHdr32;
    PIMAGE_NT_HEADERS64 pNtHdr64;
    PIMAGE_EXPORT_DIRECTORY pExportDir;
    PULONG Functions;
    PUSHORT NameOrdinals;
    PULONG Names;
    ULONG i;

    pDosHdr = lpBase;
    pNtHdr32 = (PIMAGE_NT_HEADERS32)((ULONG_PTR)lpBase + pDosHdr->e_lfanew);
    pNtHdr64 = (PIMAGE_NT_HEADERS64)((ULONG_PTR)lpBase + pDosHdr->e_lfanew);

    if(pNtHdr32->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC)
    {
        pExportDir = (PIMAGE_EXPORT_DIRECTORY)pNtHdr32->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
        if(pExportDir)
            pExportDir = (PIMAGE_EXPORT_DIRECTORY)((ULONG_PTR)pExportDir + (ULONG_PTR)lpBase);
    }
    else
    {
        pExportDir = (PIMAGE_EXPORT_DIRECTORY)pNtHdr64->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
        if(pExportDir)
            pExportDir = (PIMAGE_EXPORT_DIRECTORY)((ULONG_PTR)pExportDir + (ULONG_PTR)lpBase);
    }

    if(!pExportDir)
        return NULL;

    Functions = (PULONG)((ULONG_PTR)lpBase + (ULONG_PTR)pExportDir->AddressOfFunctions);
    NameOrdinals = (PUSHORT)((ULONG_PTR)lpBase + (ULONG_PTR)pExportDir->AddressOfNameOrdinals);
    Names = (PULONG)((ULONG_PTR)lpBase + (ULONG_PTR)pExportDir->AddressOfNames);

    for(i = 0; i < pExportDir->NumberOfNames; i++)
    {
        ULONG j = NameOrdinals[i];

        if(!strcmp((PCHAR)((ULONG_PTR)lpBase + Names[i]), lpszAPIName))
        {
            return (PVOID)((ULONG_PTR)lpBase + Functions[j]);
        }
    }
    return NULL;
}

NTSTATUS IsWow64Process(HANDLE ProcessId, PBOOLEAN IsWow64)
{
    NTSTATUS Status;
#if defined (_WIN64)
    HANDLE hProcess = NULL;
#endif

    Status = STATUS_SUCCESS;
    *IsWow64 = FALSE;

#if defined (_WIN64)

    do {
        CLIENT_ID Cid;
        OBJECT_ATTRIBUTES oa;
        ULONG_PTR Wow64Information;
        ULONG ReturnedLength;

        Cid.UniqueProcess = ProcessId;
        Cid.UniqueThread = NULL;

        InitializeObjectAttributes(&oa, NULL, OBJ_KERNEL_HANDLE, NULL, NULL);

        Status = ZwOpenProcess(&hProcess, PROCESS_ALL_ACCESS, &oa, &Cid );

        if (!NT_SUCCESS(Status))
            break;

        Status = ZwQueryInformationProcess(hProcess, ProcessWow64Information, &Wow64Information, sizeof( ULONG_PTR ), &ReturnedLength);
        if (!NT_SUCCESS(Status))
            break;

        if (Wow64Information)
            *IsWow64 = TRUE;

    } while (FALSE);

    if (hProcess)
        ZwClose(hProcess);

#endif  //(_WIN64)

    UNREFERENCED_PARAMETER(ProcessId);
    return Status;
}

VOID CreateProcessNotifyRoutine(HANDLE ParentId, HANDLE ProcessId, BOOLEAN Create)
{
    UNREFERENCED_PARAMETER(ParentId);

    if(Create)
    {
        BOOLEAN IsWow64;
        NTSTATUS Status;
        PPROCESS_TABLE pProcess;

        //查询是否是 WOW64 进程
        Status = IsWow64Process(ProcessId, &IsWow64);
        if(!NT_SUCCESS(Status))
        {
            DbgPrint("IsWow64Process failed - PID: %d\n",ProcessId);
            return;
        }

        //添加进程链表
        ProcessLock();
        pProcess = ProcessAdd(ProcessId);
        if(pProcess)
        {
            pProcess->IsWow64 = IsWow64;
        }
        else
        {
            DbgPrint("ProcessAdd failed - PID: %d\n",ProcessId);
        }
        ProcessUnlock();

    }
    else
    {
        ProcessLock();
        ProcessDelete(ProcessId);
        ProcessUnlock();
    }
}

VOID KernelApcRoutine(PKAPC Apc, PKNORMAL_ROUTINE *NormalRoutine, PVOID *NormalContext, PVOID *SystemArgument1, PVOID *SystemArgument2)
{
    UNREFERENCED_PARAMETER(SystemArgument1);
    UNREFERENCED_PARAMETER(SystemArgument2);
    UNREFERENCED_PARAMETER(NormalContext);

    DbgPrint("User APC is being delivered - Apc: %p\n", Apc);

    if(PsIsThreadTerminating(PsGetCurrentThread())) 
    {
        //这种情况下NormalRoutine不会被调用
        *NormalRoutine = NULL;
    }

    ExFreePool(Apc);
}

BOOLEAN InsertUserApc(PKNORMAL_ROUTINE Routine, PVOID Context)
{
    PKAPC Apc;

    Apc = (PKAPC)ExAllocatePool(NonPagedPool, sizeof(KAPC));
    if(!Apc)
    {
        DbgPrint("InsertUserApc alloc failed\n");
        return FALSE;
    }

    KeInitializeApc( Apc,
        KeGetCurrentThread(),
        OriginalApcEnvironment,
        KernelApcRoutine,
        NULL,
        Routine,
        UserMode,
        Context );

    if(!KeInsertQueueApc(Apc, NULL, NULL, 0))
    {
        DbgPrint("KeInsertQueueApc failed\n");
        ExFreePool(Apc);
        return FALSE;
    }

    return TRUE;
}

NTSTATUS InsertUserApc32Bit(PPROCESS_TABLE pProcess,PKNORMAL_ROUTINE LoadInjectDllThunkRoutine,PINJECT_CONTEXT Context)
{
    PVOID Wow64ApcRoutine;
    WOW64_APC_CONTEXT ApcContext32;

    DbgPrint("Insert 32-bit APC Wow64ApcRoutine - PID: %d\n",pProcess->Pid);

    Wow64ApcRoutine = GetProcAddress(pProcess->Wow64Base, "Wow64ApcRoutine");
    if (!Wow64ApcRoutine)
    {
        DbgPrint("Get Wow64ApcRoutine failed - PID: %d\n",pProcess->Pid);
        return STATUS_PROCEDURE_NOT_FOUND;
    }

    //为 Wow64ApcRoutine 创建一个上下文并插入 APC 队列
    ApcContext32.Apc32BitRoutine = (ULONG)(ULONG_PTR)LoadInjectDllThunkRoutine;
    ApcContext32.Apc32BitContext = (ULONG)(ULONG_PTR)Context;

    if(!InsertUserApc((PKNORMAL_ROUTINE)Wow64ApcRoutine, (PVOID)ApcContext32.Apc64BitContext)) 
    {
        DbgPrint("InsertUserApc32Bit InsertUserApc failed - PID: %d\n",pProcess->Pid);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    return STATUS_SUCCESS;
}

/*
VOID InjectDllThunk(PVOID Context,PVOID SystemArgument1,PVOID SystemArgument2)
{
    UNICODE_STRING DllName;
    HANDLE DllHandle;
    PINJECT_CONTEXT InjectContext = (PINJECT_CONTEXT)Context;

    UNREFERENCED_PARAMETER(SystemArgument1);
    UNREFERENCED_PARAMETER(SystemArgument2);

    DllName.Length = InjectContext->DllNameLength;
    DllName.MaximumLength = InjectContext->DllNameLength;
    DllName.Buffer = InjectContext->DllName;

    InjectContext->LoadRoutine(NULL,NULL,&DllName,&DllHandle);
    InjectContext->TestAlertRoutine();
}
*/

CHAR InjectThunk32[] = {0x8B,0xFF,0x55,0x8B,0xEC,0x51,0x51,0x56,0x8B,0x75,0x08,0x0F,0xB7,0x46,0x10,0x66,
    0x89,0x45,0xF8,0x66,0x89,0x45,0xFA,0x8D,0x46,0x12,0x89,0x45,0xFC,0x8D,0x45,0x08,
    0x50,0x8D,0x45,0xF8,0x50,0x6A,0x00,0x6A,0x00,0xFF,0x16,0xFF,0x56,0x08,0x5E,0xC9,
    0xC2,0x0C,0x00};

CHAR InjectThunk64[] = {0x40,0x53,0x48,0x83,0xEC,0x30,0x0F,0xB7,0x41,0x10,0x48,0x8B,0xD9,0x4C,0x8D,0x4C,
    0x24,0x40,0x66,0x89,0x44,0x24,0x20,0x66,0x89,0x44,0x24,0x22,0x48,0x8D,0x41,0x12,
    0x4C,0x8D,0x44,0x24,0x20,0x33,0xD2,0x33,0xC9,0x48,0x89,0x44,0x24,0x28,0xFF,0x13,
    0xFF,0x53,0x08,0x48,0x83,0xC4,0x30,0x5B,0xC3};

NTSTATUS CommonInsertApc(PPROCESS_TABLE pProcess)
{
    NTSTATUS Status;

    //插入 APC
    if(!pProcess->IsWow64)
    {
        //复制代码
#ifdef _WIN64
        memcpy(pProcess->InjectCodePtr,InjectThunk64,sizeof(InjectThunk64));
#else
        memcpy(pProcess->InjectCodePtr,InjectThunk32,sizeof(InjectThunk32));
#endif
        Status = InsertUserApc((PKNORMAL_ROUTINE)pProcess->InjectCodePtr, pProcess->InjectContext);
    }
    else
    {
        //复制代码
        memcpy(pProcess->InjectCodePtr,InjectThunk32,sizeof(InjectThunk32));

        //如果是 WOW64，则插入32位 APC
        Status = InsertUserApc32Bit(pProcess, (PKNORMAL_ROUTINE)pProcess->InjectCodePtr, pProcess->InjectContext);
    }

    return Status;
}

NTSTATUS AllocateInjectContext(PPROCESS_TABLE pProcess)
{
    NTSTATUS Status;
    PCHAR InjectCodePtr = NULL;
    PINJECT_CONTEXT InjectContext = NULL;
    SIZE_T RegionSize = PAYLOAD_CODE_LEN + sizeof(INJECT_DLL_CONTEXT);
    UNICODE_STRING TargetDllName = RTL_CONSTANT_STRING(L"inject.dll");

    //申请用户态内存
    Status = ZwAllocateVirtualMemory( NtCurrentProcess(),
        (PVOID *)&InjectCodePtr,
        0,
        &RegionSize,
        MEM_COMMIT,
        PAGE_EXECUTE_READWRITE);

    if(!NT_SUCCESS(Status))
    {
        DbgPrint("AllocateInjectContext alloc failed - PID: %d\n", pProcess->Pid);
        return Status;
    }

    InjectContext = (PINJECT_CONTEXT)(InjectCodePtr + PAYLOAD_CODE_LEN);
    InjectContext->DllNameLength = TargetDllName.Length;
    RtlCopyMemory(InjectContext->DllName, TargetDllName.Buffer, TargetDllName.Length);

    pProcess->InjectCodePtr = InjectCodePtr;
    pProcess->InjectContext = InjectContext;

    return Status;
}

NTSTATUS InitializeNtFuncPtr(PPROCESS_TABLE pProcess)
{
    PVOID SystemDllBase = pProcess->IsWow64 ? pProcess->Wow64NtdllBase : pProcess->NtdllBase;
    PINJECT_CONTEXT Context = (PINJECT_CONTEXT)pProcess->InjectContext;

    Context->LoadRoutine = (PLDR_LOAD_DLL)GetProcAddress(SystemDllBase, "LdrLoadDll");
    Context->TestAlertRoutine = (PTEST_ALERT)GetProcAddress(SystemDllBase, "ZwTestAlert");

    if(!Context->LoadRoutine || !Context->TestAlertRoutine)
        return STATUS_PROCEDURE_NOT_FOUND;

    DbgPrint("Get LdrLoadDll @ %p\n",Context->LoadRoutine);

    return STATUS_SUCCESS;
}

//检查路径中的文件名是否一致
BOOLEAN CheckImageName(PUNICODE_STRING ImageName,PUNICODE_STRING CheckString)
{
    UNICODE_STRING Temp = *ImageName;
    USHORT Skip;

    if(ImageName->Length < CheckString->Length)
        return FALSE;

    Skip = (Temp.Length - CheckString->Length) / sizeof(WCHAR);
    Temp.Buffer += Skip;
    Temp.Length -= Skip * sizeof(WCHAR);
    Temp.MaximumLength -= Skip * sizeof(WCHAR);

    return RtlEqualUnicodeString(&Temp, CheckString, TRUE);
}

VOID LoadImageNotifyRoutine(PUNICODE_STRING FullImageName, HANDLE ProcessId, PIMAGE_INFO ImageInfo)
{
    PPROCESS_TABLE pProcess;
    BOOLEAN ShouldInsertApc = FALSE;
    BOOLEAN ForceDllInjection = TRUE;
    NTSTATUS Status = STATUS_SUCCESS;

    //锁进程链表
    ProcessLock();

    //查询进程
    pProcess = ProcessLookup(ProcessId);
    if(!pProcess)
    {
        ProcessUnlock();
        return;
    }

    if(pProcess->FirstLoad)
    {
        pProcess->FirstLoad = FALSE;
        RtlUnicodeStringToAnsiString(&pProcess->FullImageName,FullImageName,TRUE);
        pProcess->ImageBase = ImageInfo->ImageBase;
    }

    if(pProcess->DontInject)
    {
        //注入过程出错，被标记了不再注入
        DbgPrint("LoadImageNotifyRoutine DontInject - PID: %d\n",ProcessId);
        ProcessUnlock();
        return;
    }

    //需要找一个恰当的时机插入APC
    if(!pProcess->ApcInserted)
    {
        UNICODE_STRING NtdllName = RTL_CONSTANT_STRING(L"\\SystemRoot\\System32\\ntdll.dll");

        //APC还没有插入

        //检查是否正在装载 Ntdll.dll
        if(RtlEqualUnicodeString(FullImageName, &NtdllName, TRUE))
        {
            //正在装载 Ntdll.dll
            //这是插入 Dll 的最好机会
            //因为
            // 1.当前处于进程主线程上下文
            // 2.进程和线程在内核模式完成其初始化

            DbgPrint("NTDLL is being loaded  - PID: %d\n",ProcessId);

            //记下Ntdll的基地址
            pProcess->NtdllBase = ImageInfo->ImageBase;

            //用户模式的 thunk 函数装载目标 Dll 时需要 LdrLoadDll 等函数的指针，为其准备上下文
            Status = AllocateInjectContext(pProcess);
            if(NT_SUCCESS(Status) && !pProcess->IsWow64)
            {
                //如果进程不是Wow64，就可以取 NTDLL 函数指针，然后准备插入 APC 了
                //否则需要等待 WOW64 仿真DLL的加载
                Status = InitializeNtFuncPtr(pProcess);

                //可以准备插入 APC 了
                if(NT_SUCCESS(Status))
                    ShouldInsertApc = TRUE;
            }

            if(!NT_SUCCESS(Status))
            {
                //如果之前发生了错误，那么就标记这个进程 不再插入
                DbgPrint("Mark as DontInject because of some trouble - PID: %d\n", pProcess->Pid);
                pProcess->DontInject = TRUE;
            }

        }

        //WOW64 进程的处理方式有些不同
        //需要等待 Wow64.dll 加载
        //因为 Wow64 的 32位APC 是从Wow64.dll 的Wow64ApcRoutine 开始的
        if (pProcess->IsWow64)
        {
            UNICODE_STRING Wow64Name = RTL_CONSTANT_STRING(L"Wow64.dll");
            UNICODE_STRING Wow64NtdllName = RTL_CONSTANT_STRING(L"SysWOW64\\ntdll.dll");

            //检查是不是 Wow64.dll 或者 32位的 Ntdll.dll
            if(CheckImageName(FullImageName, &Wow64NtdllName))
            {
                //正在加载 32位 NTDLL
                DbgPrint("32bit NTDLL is being loaded - PID: %d\n",pProcess->Pid);

                pProcess->Wow64NtdllBase = ImageInfo->ImageBase;

                Status = InitializeNtFuncPtr(pProcess);
                if(!NT_SUCCESS(Status))
                {
                    //发生了错误，那么就标记这个进程 不再插入
                    DbgPrint("Mark as DontInject cuz InitializeNtFuncPtr failed - PID: %d\n", pProcess->Pid);
                    pProcess->DontInject = TRUE;
                }
                else if(pProcess->Wow64Base)
                {
                    //Wow64.dll 已加载，可以插入 APC
                    ShouldInsertApc = TRUE;
                }
            }
            else if(CheckImageName(FullImageName, &Wow64Name))
            {
                //正在加载 Wow64.dll
                DbgPrint("Wow64.dll is being loaded - PID: %d\n",pProcess->Pid);

                pProcess->Wow64Base = ImageInfo->ImageBase;

                if(pProcess->Wow64NtdllBase)
                {
                    //32位 NTDLL 已经加载了，可以插入 APC
                    ShouldInsertApc = TRUE;
                }
            }

            //警告！不要强迫 WOW64 进程立即执行 APC
            //因为 Wow64.dll 现在还没有初始化DLLEntry，需要初始化后才能执行 32位 APC。
            ForceDllInjection = FALSE;
        }
    }
    else if(pProcess->ForceDllInjection)
    {
        //强迫 APC 执行加载 DLL

        if(ExGetPreviousMode() == UserMode)
        {
            LARGE_INTEGER Delay = {0};

            //可以强迫 APC 执行了
            //因为用户模式 APC 队列不是空的（因为有 LoadInjectDllThunk）
            //当前线程将被设置 UserApcPending

            DbgPrint("Forcing APC delivering - PID: %d\n",pProcess->Pid);
            Status = KeDelayExecutionThread(UserMode,TRUE,&Delay);

            //标记不再强迫 APC 执行
            pProcess->ForceDllInjection = FALSE;
        }
    }

    if(ShouldInsertApc)
    {
        //是时候插入 APC 了

        Status = CommonInsertApc(pProcess);
        if(NT_SUCCESS(Status))
        {
            pProcess->ApcInserted = TRUE;

            if(ForceDllInjection)
                pProcess->ForceDllInjection = TRUE;
        }
        else
        {
            //发生了错误，那么就标记这个进程 不再插入
            DbgPrint("Mark as DontInject cuz CommonInsertApc failed - PID: %d\n", pProcess->Pid);
            pProcess->DontInject = TRUE;
        }
    }

    //解锁进程链表
    ProcessUnlock();
}

//初始化PROCESS
NTSTATUS ProcessInitialize()
{
    NTSTATUS Status = STATUS_UNSUCCESSFUL;

    do {
        //互斥锁，全局链表
        KeInitializeMutex(&ProcessMutex, 0);
        InitializeListHead(&ProcessTable);

        //哈希表
        ProcessHashTableLen = sizeof(HLIST_FIRST) * ProcessHashLen;
        ProcessHashTable = (HLIST_FIRST *)ProcessAllocatePool(ProcessHashTableLen,TRUE);
        if(!ProcessHashTable)
            break;

        //注册回调
        Status = PsSetCreateProcessNotifyRoutine(CreateProcessNotifyRoutine,FALSE);
        if(!NT_SUCCESS(Status))
            break;

        Status = PsSetLoadImageNotifyRoutine(LoadImageNotifyRoutine);
        if(!NT_SUCCESS(Status))
            break;

    } while (FALSE);

    if(!NT_SUCCESS(Status))
    {
        if(ProcessHashTable)
            ProcessFreePool(ProcessHashTable,ProcessHashTableLen);
        ProcessHashTable = NULL;
    }

    return Status;
}

VOID ProcessUnload()
{
    PLIST_ENTRY Link,LinkNext;
    PPROCESS_TABLE pProcess;

    //注销回调
    PsSetCreateProcessNotifyRoutine(CreateProcessNotifyRoutine,TRUE);
    PsRemoveLoadImageNotifyRoutine(LoadImageNotifyRoutine);

    //释放内存
    LIST_FOR_EACH_ENTRY_SAFE(pProcess,Link,LinkNext,&ProcessTable,PROCESS_TABLE,GlobalList)
    {
        LinkNext = Link->Flink;

        ProcessTableCount--;
        RemoveEntryHList(&pProcess->HashList);
        RemoveEntryList(&pProcess->GlobalList);

        ProcessFreePool(pProcess,sizeof(PROCESS_TABLE));
    }

    //释放哈希表
    if(ProcessHashTable)
        ProcessFreePool(ProcessHashTable,ProcessHashTableLen);
    ProcessHashTable = NULL;
}
