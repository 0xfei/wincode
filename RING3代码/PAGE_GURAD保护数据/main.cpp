#include <windows.h>
#include <windef.h>
#include <iostream>
#include <list>
#include <stdio.h>
#pragma comment(lib, "ntdll.lib")

typedef struct _MAD_MEM
{
	MEMORY_BASIC_INFORMATION mbi;
	BOOL bAccess;
}MAD_MEM, *PMAD_MEM;

typedef ULONG (WINAPI *pRtlExceptionDispatcher)(PEXCEPTION_RECORD pExcptRec, CONTEXT* pContext);
static pRtlExceptionDispatcher OldRtlExceptionDispatcher = NULL;

BOOL bHook = FALSE;
std::list<PMAD_MEM> memoryBlocks;

typedef PIMAGE_NT_HEADERS
(*PRtlImageNtHeader)(PVOID ModuleAddress);

PRtlImageNtHeader RtlImageNtHeader;

BOOL Check(PEXCEPTION_RECORD pExcptRec, CONTEXT *pContext)
{
	ULONG_PTR Eip = (ULONG_PTR)pExcptRec->ExceptionAddress;
	if (!memoryBlocks.empty() && pExcptRec->ExceptionCode == EXCEPTION_GUARD_PAGE)
	{
		std::list<PMAD_MEM>::iterator b;
		for (b = memoryBlocks.begin(); b != memoryBlocks.end(); b++)
		{
			PMAD_MEM mBlock = *b;
			if (Eip >= (ULONG_PTR)mBlock->mbi.BaseAddress
				&& Eip<= (ULONG_PTR)mBlock->mbi.BaseAddress + mBlock->mbi.RegionSize)
			{
				mBlock->bAccess = TRUE;
				*b = mBlock;
				return TRUE;
			}
		}
	}
	return FALSE;
}

ULONG WINAPI MyRtlExceptionDispatcher( PEXCEPTION_RECORD pExcptRec, CONTEXT *pContext)
{
	if (Check(pExcptRec, pContext)) return 1;
	return OldRtlExceptionDispatcher(pExcptRec, pContext);
}

void Install()
{
	BYTE *pAddr = (BYTE*)::GetProcAddress(::GetModuleHandleA("ntdll.dll"), "KiUserExceptionDispatcher");
	if (bHook || !pAddr) return;
	while (*pAddr != 0xE8)
	{
		pAddr++;
	}
	printf("%08x\n", pAddr);
	OldRtlExceptionDispatcher = (pRtlExceptionDispatcher)((*(DWORD*)(pAddr+1))+5+(DWORD)pAddr); 
	printf("%08x\n", OldRtlExceptionDispatcher);
	DWORD dwNewAddr = (DWORD)MyRtlExceptionDispatcher - (DWORD)pAddr - 5;
	DWORD dwOld;
	VirtualProtect((LPVOID)pAddr, 0x1000, PAGE_EXECUTE_READWRITE, &dwOld);
	RtlCopyMemory((PVOID)((DWORD)pAddr + 1), (PVOID)&dwNewAddr, 4);
	bHook = TRUE;
}

DWORD WINAPI MADThread(LPVOID Param)
{
	std::list<PMAD_MEM>::iterator b;
	for (b=memoryBlocks.begin(); b!=memoryBlocks.end(); b++)
	{
		DWORD dwOld;
		PMAD_MEM mBlock = *b;
		mBlock->bAccess = FALSE;
		*b = mBlock;
		VirtualProtect(
			mBlock->mbi.BaseAddress, 
			mBlock->mbi.RegionSize,
			mBlock->mbi.Protect | PAGE_GUARD,
			&dwOld);
	}
	while (1)
	{
		std::list<PMAD_MEM>::iterator b;
		for (b = memoryBlocks.begin(); b!=memoryBlocks.end(); b++)
		{
			DWORD dwOld;
			PMAD_MEM mBlock = *b;
			MEMORY_BASIC_INFORMATION mbi;
			VirtualQueryEx(
				GetCurrentProcess(),
				mBlock->mbi.BaseAddress,
				&mbi,
				sizeof(mbi));
			if ((mbi.Protect & PAGE_GUARD) != PAGE_GUARD)
			{
				if (!mBlock->bAccess)
				{
					ExitProcess(-1);
				}
			}
			mBlock->bAccess = FALSE;
			*b = mBlock;
			VirtualProtect(
				mBlock->mbi.BaseAddress,
				mBlock->mbi.RegionSize,
				mBlock->mbi.Protect | PAGE_GUARD,
				&dwOld);
		}
		Sleep(1000*30);
	}
}

void InitMAD()
{
	MEMORY_BASIC_INFORMATION meminfo;
	unsigned char *addr = NULL;
	unsigned char *endaddr = NULL;
	DWORD totalBytes = 0;

	Install();

	RtlImageNtHeader = (PRtlImageNtHeader)GetProcAddress(::GetModuleHandleA("ntdll.dll"), "RtlImageNtHeader");
	VirtualQuery((LPCVOID)GetModuleHandleA("game.dll"), &meminfo, sizeof(meminfo));
	addr = (unsigned char*)meminfo.AllocationBase;
	endaddr = addr + RtlImageNtHeader(addr)->OptionalHeader.SizeOfImage;

	while (addr < endaddr)
	{
		if (!VirtualQueryEx(GetCurrentProcess(), addr, &meminfo, sizeof(meminfo)))
			break;
		BOOL Commited = meminfo.State & MEM_COMMIT;
		BOOL Readable = meminfo.Protect & (PAGE_READWRITE | PAGE_READONLY);
		BOOL Guarded = meminfo.Protect & PAGE_GUARD;

		if (Commited && Readable && !Guarded)
		{
			totalBytes += (DWORD)meminfo.RegionSize;
			PMAD_MEM pNew = new MAD_MEM;
			pNew->mbi = meminfo;
			pNew->bAccess = FALSE;
			memoryBlocks.push_back(pNew);
		}
		addr = (unsigned char *)meminfo.BaseAddress + meminfo.RegionSize;
	}
	CreateThread(
		NULL, 
		0,
		MADThread,
		NULL, 
		0,
		NULL);
}

int main()
{
	Install();
	return 0;
}