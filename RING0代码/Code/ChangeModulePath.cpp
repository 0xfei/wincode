#include <windows.h>
#include <stdio.h>

typedef struct _UNICODE_STRING 
{
    USHORT Length;
    USHORT MaximumLength;
    PWSTR Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _PEB_LDR_DATA
{
	ULONG Length;
	BOOLEAN Initialized;
	PVOID SsHandle; 
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList; 
	LIST_ENTRY InInitializationOrderModuleList;
} PEB_LDR_DATA,*PPEB_LDR_DATA; 


typedef struct _LDR_MODULE
{
	LIST_ENTRY          InLoadOrderModuleList;
	LIST_ENTRY          InMemoryOrderModuleList;  
	LIST_ENTRY          InInitializationOrderModuleList; 
	void*               BaseAddress;  
	void*               EntryPoint;   
	ULONG               SizeOfImage;
	UNICODE_STRING		FullDllName;
	UNICODE_STRING      BaseDllName;
	ULONG               Flags;
	SHORT               LoadCount;
	SHORT               TlsIndex;
	HANDLE              SectionHandle;
	ULONG               CheckSum;
	ULONG               TimeDateStamp;
} LDR_MODULE, *PLDR_MODULE;


int main(int argc, char* argv[])
{
	PEB_LDR_DATA *pPEBLDR;
	LDR_MODULE *pLdrMod;
	LIST_ENTRY *pListEntry,*pStart;
	DWORD dwKernelBase;
	PWSTR fakemodulename=L"假名字在此";
	PWSTR fakemodulepath=L"这是假路径，看能骗到谁";
	_asm
	{
			mov eax,fs:[0x30] //TEB->PEB
			mov eax,[eax+0xC] //PEB->Ldr
			mov pPEBLDR,eax
	}
	printf("准备改名字了...\n");
	dwKernelBase=(DWORD)GetModuleHandle("kernel32.dll");
	printf("Base of kernel32.dll is 0x%08x\n",dwKernelBase);
	pStart=pListEntry=(LIST_ENTRY*)(PUCHAR)&(pPEBLDR->InLoadOrderModuleList);
	do 
	{
		pListEntry=pListEntry->Flink;
		pLdrMod=(LDR_MODULE*)(pListEntry);
		if ((DWORD)pLdrMod->BaseAddress==dwKernelBase)
		{
			printf("Found kernel32.dll...");
			lstrcpyW(pLdrMod->BaseDllName.Buffer,fakemodulename);
			lstrcpyW(pLdrMod->FullDllName.Buffer,fakemodulepath);
			printf("Change OK.\n");
			break;
		}
		
	} while(pListEntry!=pStart);
	printf("现在你可以用工具来看看本进程的模块列表\n");
	while (1)
	{
		Sleep(1000);
	}
	return 0;
}

