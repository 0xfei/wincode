#include <windows.h>
#include <stdio.h>
#include <aclapi.h>
#include <conio.h>

typedef LONG NTSTATUS;
typedef ULONG ACCESS_MASK;

#define STATUS_INFO_LENGTH_MISMATCH ((NTSTATUS)0xC0000004L)
#define STATUS_ACCESS_DENIED ((NTSTATUS)0xC0000022L)
#define NT_SUCCESS(status) ((NTSTATUS)status >= 0)


#define InitializeObjectAttributes( p, n, a, r, s)	\
	{												\
		(p)->Length = sizeof( OBJECT_ATTRIBUTES );	\
		(p)->RootDirectory = r; (p)->Attributes = a;\
		(p)->ObjectName = n;						\
		(p)->SecurityDescriptor = s;				\
		(p)->SecurityQualityOfService = NULL;		\
	}

typedef enum _SYSTEM_INFORMATION_CLASS
{
	SystemBasicInformation,                // 00 Y N
	SystemProcessorInformation,            // 01 Y N
	SystemPerformanceInformation,          // 02 Y N
	SystemTimeOfDayInformation,            // 03 Y N
	SystemNotImplemented1,                 // 04 Y N
	SystemProcessesAndThreadsInformation,  // 05 Y N
	SystemCallCounts,                      // 06 Y N
	SystemConfigurationInformation,        // 07 Y N
	SystemProcessorTimes,                  // 08 Y N
	SystemGlobalFlag,                      // 09 Y Y
	SystemNotImplemented2,                 // 10 Y N
	SystemModuleInformation,               // 11 Y N
	SystemLockInformation,                 // 12 Y N
	SystemNotImplemented3,                 // 13 Y N
	SystemNotImplemented4,                 // 14 Y N
	SystemNotImplemented5,                 // 15 Y N
	SystemHandleInformation,               // 16 Y N
	SystemObjectInformation,               // 17 Y N
	SystemPagefileInformation,             // 18 Y N
	SystemInstructionEmulationCounts,      // 19 Y N
	SystemInvalidInfoClass1,               // 20
	SystemCacheInformation,                // 21 Y Y
	SystemPoolTagInformation,              // 22 Y N
	SystemProcessorStatistics,             // 23 Y N
	SystemDpcInformation,                  // 24 Y Y
	SystemNotImplemented6,                 // 25 Y N
	SystemLoadImage,                       // 26 N Y
	SystemUnloadImage,                     // 27 N Y
	SystemTimeAdjustment,                  // 28 Y Y
	SystemNotImplemented7,                 // 29 Y N
	SystemNotImplemented8,                 // 30 Y N
	SystemNotImplemented9,                 // 31 Y N
	SystemCrashDumpInformation,            // 32 Y N
	SystemExceptionInformation,            // 33 Y N
	SystemCrashDumpStateInformation,       // 34 Y Y/N
	SystemKernelDebuggerInformation,       // 35 Y N
	SystemContextSwitchInformation,        // 36 Y N
	SystemRegistryQuotaInformation,        // 37 Y Y
	SystemLoadAndCallImage,                // 38 N Y
	SystemPrioritySeparation,              // 39 N Y
	SystemNotImplemented10,                // 40 Y N
	SystemNotImplemented11,                // 41 Y N
	SystemInvalidInfoClass2,               // 42
	SystemInvalidInfoClass3,               // 43
	SystemTimeZoneInformation,             // 44 Y N
	SystemLookasideInformation,            // 45 Y N
	SystemSetTimeSlipEvent,                // 46 N Y
	SystemCreateSession,                   // 47 N Y
	SystemDeleteSession,                   // 48 N Y
	SystemInvalidInfoClass4,               // 49
	SystemRangeStartInformation,           // 50 Y N
	SystemVerifierInformation,             // 51 Y Y
	SystemAddVerifier,                     // 52 N Y
	SystemSessionProcessesInformation      // 53 Y N
} SYSTEM_INFORMATION_CLASS;

typedef struct _SYSTEM_HANDLE_TABLE_ENTRY_INFO {
	USHORT UniqueProcessId;
	USHORT CreatorBackTraceIndex;
	UCHAR ObjectTypeIndex;
	UCHAR HandleAttributes;
	USHORT HandleValue;
	PVOID Object;
	ULONG GrantedAccess;
} SYSTEM_HANDLE_TABLE_ENTRY_INFO, *PSYSTEM_HANDLE_TABLE_ENTRY_INFO;

typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} UNICODE_STRING,*PUNICODE_STRING;

typedef struct _OBJECT_ATTRIBUTES {
	ULONG Length;
	HANDLE RootDirectory;
	PUNICODE_STRING ObjectName;
	ULONG Attributes;
	PVOID SecurityDescriptor; 
	PVOID SecurityQualityOfService; 
} OBJECT_ATTRIBUTES,*POBJECT_ATTRIBUTES;

typedef struct _CLIENT_ID {
	HANDLE UniqueProcess;
	HANDLE UniqueThread;
} CLIENT_ID,*PCLIENT_ID;

typedef enum _SYSTEM_HANDLE_TYPE
{
	OB_TYPE_UNKNOWN,	//0
	OB_TYPE_TYPE,		//1
	OB_TYPE_DIRECTORY,	//2
	OB_TYPE_SYMBOLIC_LINK,//3
	OB_TYPE_TOKEN,		//4
	OB_TYPE_PROCESS,	//5
	OB_TYPE_THREAD,		//6
	OB_TYPE_UNKNOWN_7,	//7
	OB_TYPE_EVENT,		//8
	OB_TYPE_EVENT_PAIR,	//9
	OB_TYPE_MUTANT,		//10
	OB_TYPE_UNKNOWN_11,	//11
	OB_TYPE_SEMAPHORE,	//12
	OB_TYPE_TIMER,		//13
	OB_TYPE_PROFILE,	//14
	OB_TYPE_WINDOW_STATION,//15
	OB_TYPE_DESKTOP,	//16
	OB_TYPE_SECTION,	//17
	OB_TYPE_KEY,		//18
	OB_TYPE_PORT,		//19
	OB_TYPE_WAITABLE_PORT,//20
	OB_TYPE_UNKNOWN_21,	
	OB_TYPE_UNKNOWN_22,
	OB_TYPE_UNKNOWN_23,
	OB_TYPE_UNKNOWN_24,
	OB_TYPE_IO_COMPLETION,//25
	OB_TYPE_FILE		//26
} SYSTEM_HANDLE_TYPE;

typedef NTSTATUS 
(__stdcall *ZWQUERYSYSTEMINFORMATION ) (
	SYSTEM_INFORMATION_CLASS SystemInformationClass, 
	PVOID SystemInformation, 
	ULONG SystemInformationLength,
	PULONG ReturnLength OPTIONAL 
	); 


typedef NTSTATUS
(__stdcall *ZWOPENPROCESS) (
	PHANDLE ProcessHandle,
	ULONG DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	PCLIENT_ID ClientId
	);

typedef NTSTATUS
(__stdcall *ZWALLOCATEVIRTUALMEMORY) (
	HANDLE ProcessHandle,
	PVOID *BaseAddress,
	ULONG ZeroBits,
	PSIZE_T RegionSize,
	ULONG AllocationType,
	ULONG Protect
	);


typedef NTSTATUS
(__stdcall *ZWFREEVIRTUALMEMORY)(
	HANDLE ProcessHandle,
	PVOID *BaseAddress,
	PSIZE_T RegionSize,
	ULONG FreeType
	);

typedef NTSTATUS
(__stdcall *ZWOPENTHREAD)(
	PHANDLE ThreadHandle,
	ACCESS_MASK DesiredAccess,
	POBJECT_ATTRIBUTES ObjectAttributes,
	PCLIENT_ID ClientId
	);

typedef NTSTATUS
(__stdcall *ZWDUPLICATEOBJECT)(
	HANDLE SourceProcessHandle,
	HANDLE SourceHandle,
	HANDLE TargetProcessHandle,
	PHANDLE TargetHandle,
	ACCESS_MASK DesiredAccess,
	ULONG HandleAttributes,
	ULONG Options
	);

typedef NTSTATUS
(__stdcall *ZWCLOSE)(
	HANDLE Handle
	);

typedef DWORD
(__stdcall *RTLNTSTATUSTODOSERROR)(
	NTSTATUS status
);

ZWQUERYSYSTEMINFORMATION ZwQuerySystemInformation; 
ZWOPENPROCESS ZwOpenProcess;
ZWALLOCATEVIRTUALMEMORY ZwAllocateVirtualMemory;
ZWFREEVIRTUALMEMORY ZwFreeVirtualMemory;
ZWOPENTHREAD ZwOpenThread;
ZWDUPLICATEOBJECT ZwDuplicateObject;
ZWCLOSE ZwClose;
RTLNTSTATUSTODOSERROR RtlNtStatusToDosError;

PSYSTEM_HANDLE_TABLE_ENTRY_INFO pHandleInfo;

DWORD pGetProcessId(void);
DWORD pGetThreadId(void);

void PrintZwError(char* message, NTSTATUS status)
{
	char *errMsg;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		RtlNtStatusToDosError(status),MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&errMsg,
		0,
		NULL);
	printf("%s: %s Error code=0x%08x\n", message, errMsg, status);
	LocalFree(errMsg);
}

void CloseMyHandle()
{
	HANDLE hCurProcess, hSrcProcessHandle, hDstHandle;
	HANDLE hMyProcess = INVALID_HANDLE_VALUE, hMyThread = INVALID_HANDLE_VALUE;
	DWORD nBufferLen = 0x40000, nRetLen = 0;
	DWORD HandleCount, NumberOfHandles;
	DWORD pMyProcessObject = 0, pMyThreadObject = 0, pObject;
	CLIENT_ID myCid, tmpCid;
	PVOID pBuffer = NULL;
	NTSTATUS status;
	OBJECT_ATTRIBUTES ObjectAttributes;
	
	myCid.UniqueProcess = (HANDLE)pGetProcessId();
	myCid.UniqueThread = (HANDLE)pGetThreadId();
	InitializeObjectAttributes(
		&ObjectAttributes,
		NULL,
		0,
		NULL,
		NULL);
	status = ZwOpenProcess(
		&hMyProcess,
		PROCESS_ALL_ACCESS,
		&ObjectAttributes,
		&myCid );
	status= ZwOpenThread(
		&hMyThread,
		THREAD_ALL_ACCESS,
		&ObjectAttributes,
		&myCid);
	printf("Process: %0x, Thread: %08x\n", 
		hMyProcess,
		hMyThread);

	hCurProcess = GetCurrentProcess();
	status = ZwAllocateVirtualMemory(
		hCurProcess,
		&pBuffer,
		0,
		&nBufferLen,
		MEM_COMMIT,
		PAGE_READWRITE);
	if (!NT_SUCCESS(status))
	{
		printf("Allocate memory failed!\n");
		return;
	}
	printf("Allocate memory: %08x\n", pBuffer);

	status = ZwQuerySystemInformation(
		SystemHandleInformation,
		pBuffer,
		nBufferLen,
		&nRetLen);
	printf("Searching handles......\n");
	HandleCount = *(DWORD*)pBuffer;
	printf("Handle count: %d\n", HandleCount);
	printf("Memory need: 0x%08x\n", 
		HandleCount * sizeof(SYSTEM_HANDLE_TABLE_ENTRY_INFO)+4);

	if (HandleCount > 1)
	{
		NumberOfHandles = *(DWORD*)pBuffer;
		pHandleInfo = (PSYSTEM_HANDLE_TABLE_ENTRY_INFO)
			((char*)pBuffer + sizeof(DWORD));
		do 
		{
			if ( pHandleInfo->HandleValue == (USHORT)hMyThread)
			{
				if (pHandleInfo->UniqueProcessId == (USHORT)myCid.UniqueProcess)
				{
					pMyThreadObject = *(DWORD*)&(pHandleInfo->Object);
					printf("Thread found!\n");
				}
			}
			if ( pHandleInfo->HandleValue == (USHORT)hMyProcess)
			{
				if (pHandleInfo->UniqueProcessId == (USHORT)myCid.UniqueProcess)
				{
					pMyProcessObject = *(DWORD*)&(pHandleInfo->Object);
					printf("Process found!\n");
				}
			}
			++pHandleInfo;
			--NumberOfHandles;
		} while (NumberOfHandles);
	}
	
	ZwClose(hMyProcess);
	ZwClose(hMyThread);
	printf("Start close......\n");

	if (HandleCount >=1 )
	{
		pHandleInfo = (PSYSTEM_HANDLE_TABLE_ENTRY_INFO)
			((char*)pBuffer + sizeof(DWORD));
		do 
		{
			pObject = *(DWORD*)&(pHandleInfo->Object);

			if (pObject == pMyProcessObject || pObject == pMyThreadObject)
			{
				printf("Found handle=0x%08x OwnerPID = %4d\n", 
					pHandleInfo->HandleValue,
					pHandleInfo->UniqueProcessId);
				tmpCid.UniqueProcess = (HANDLE)pHandleInfo->UniqueProcessId;
				tmpCid.UniqueThread = 0;
				InitializeObjectAttributes(&ObjectAttributes,
					NULL,
					0,
					NULL,
					NULL);
				status = ZwOpenProcess(
					&hSrcProcessHandle,
					PROCESS_DUP_HANDLE,
					&ObjectAttributes,
					&tmpCid );
				if (!status)
				{
					status = ZwDuplicateObject(
						hSrcProcessHandle,
						(void*)pHandleInfo->HandleValue,
						hCurProcess,
						&hDstHandle,
						0,
						0,
						DUPLICATE_CLOSE_SOURCE);
					if (!status)
					{
						ZwClose(hDstHandle);
						printf("close handle\n");
					}
					ZwClose(hSrcProcessHandle);
				}
			}
			++pHandleInfo;
			--HandleCount;
		} while (HandleCount);
	}
	ZwFreeVirtualMemory(
		hCurProcess,
		&pBuffer,
		&nBufferLen,
		MEM_RELEASE);
}

void EnableDebugPriv()
{
	HANDLE hToken;
	LUID uid;
	TOKEN_PRIVILEGES tkp;
	OpenProcessToken(
		GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, 
		&hToken);
	LookupPrivilegeValue(NULL,
		SE_DEBUG_NAME,
		&uid);
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = uid;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken,FALSE, &tkp, 
		sizeof(tkp), NULL, NULL);
	CloseHandle(hToken);
}

DWORD __declspec(naked) pGetProcessId()
{
	__asm
	{
		mov eax, fs:[0x18];
		mov eax,[eax + 0x20];
		retn;
	}
}

DWORD __declspec(naked) pGetThreadId()
{
	__asm
	{
		mov eax, fs:[0x18];
		mov eax, [eax+0x20];
		retn;
	}
}

int main()
{
	HANDLE h;
	HMODULE hDll = LoadLibrary(L"ntdll.dll");
	ZwQuerySystemInformation = (ZWQUERYSYSTEMINFORMATION)GetProcAddress(hDll,"ZwQuerySystemInformation"); 
	ZwOpenProcess = (ZWOPENPROCESS)GetProcAddress(hDll,"ZwOpenProcess");
	ZwAllocateVirtualMemory = (ZWALLOCATEVIRTUALMEMORY)GetProcAddress(hDll,"ZwAllocateVirtualMemory");
	ZwFreeVirtualMemory = (ZWFREEVIRTUALMEMORY)GetProcAddress(hDll,"ZwFreeVirtualMemory");
	ZwOpenThread = (ZWOPENTHREAD)GetProcAddress(hDll,"ZwOpenThread");
	ZwDuplicateObject = (ZWDUPLICATEOBJECT)GetProcAddress(hDll,"ZwDuplicateObject");
	ZwClose = (ZWCLOSE)GetProcAddress(hDll,"ZwClose");
	RtlNtStatusToDosError = (RTLNTSTATUSTODOSERROR)GetProcAddress(hDll,"RtlNtStatusToDosError");
	
	EnableDebugPriv();
	h = OpenProcess(
		PROCESS_QUERY_INFORMATION,
		0,
		GetProcessId(NULL));
	printf("Handle := %08x\n", h);
	CloseMyHandle();
	FreeLibrary(hDll);
	printf("End.\n");
	return 0;
}