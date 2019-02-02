typedef ULONG (*pfnNtQueryInformationProcess)();

typedef struct _LDR_DATA_TABLE_ENTRY
{
	LIST_ENTRY InLoadOrderLinks;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
	PVOID DllBase;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	USHORT LoadCount;
	USHORT TlsIndex;
	union
	{
		LIST_ENTRY HashLinks;
		PVOID SectionPointer;
	};
	ULONG CheckSum;
	union
	{
		ULONG TimeDateStamp;
		PVOID LoadedImports;
	};
	PVOID EntryPointActivationContext;
	PVOID PatchInformation;
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;


BOOL HideMyself()
{
	HMODULE hMod = GetModuleHandle( _T( "ntdll.dll"));
	HMODULE hModMyself = GetModuleHandle( _T("dll.dll"));
	pfnNtQueryInformationProcess p = (pfnNtQueryInformationProcess)::GetProcAddress( hMod, "NtQueryInformationProcess");

	PROCESS_BASIC_INFORMATION stInfo = {0};
	DWORD dwRetnLen = 0;
	DWORD dw = p( GetCurrentProcess(), 0, &stInfo, sizeof(stInfo), &dwRetnLen);

	PPEB pPeb = stInfo.PebBaseAddress;
	PLIST_ENTRY ListHead, Current;
	PLDR_DATA_TABLE_ENTRY pstEntry = NULL;

	ListHead = &( stInfo.PebBaseAddress->Ldr->InLoadOrderModuleList);
	Current = ListHead->Flink;
	while ( Current != ListHead)
	{
		pstEntry = CONTAINING_RECORD( Current, LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
		//DebugOutW( L"Module:%s, base:0x%X\r\n", pstEntry->FullDllName.Buffer, pstEntry->EntryPoint);
		if ( pstEntry->DllBase == hModMyself)
		{
			pstEntry->InLoadOrderLinks.Flink->Blink = pstEntry->InLoadOrderLinks.Blink;
			pstEntry->InLoadOrderLinks.Blink->Flink = pstEntry->InLoadOrderLinks.Flink;
			DebugOut( _T( "Hide injected dll."));
			break;
		}
		Current = pstEntry->InLoadOrderLinks.Flink;
	}

	ListHead = &( stInfo.PebBaseAddress->Ldr->InMemoryOrderModuleList);
	Current = ListHead->Flink;
	while ( Current != ListHead)
	{
		pstEntry = CONTAINING_RECORD( Current, LDR_DATA_TABLE_ENTRY, InMemoryOrderModuleList);
		DebugOutW( L"Module:%s, base:0x%X\r\n", pstEntry->FullDllName.Buffer, pstEntry->EntryPoint);
		if ( pstEntry->DllBase == hModMyself)
		{
			pstEntry->InMemoryOrderModuleList.Flink->Blink = pstEntry->InMemoryOrderModuleList.Blink;
			pstEntry->InMemoryOrderModuleList.Blink->Flink = pstEntry->InMemoryOrderModuleList.Flink;
			DebugOut( _T( "Hide injected dll."));
			break;
		}
		Current = pstEntry->InMemoryOrderModuleList.Flink;
	}
	DebugOutW( L"\r\n");

	ListHead = &( stInfo.PebBaseAddress->Ldr->InInitializationOrderModuleList);
	Current = ListHead->Flink;
	while ( Current != ListHead)
	{
		pstEntry = CONTAINING_RECORD( Current, LDR_DATA_TABLE_ENTRY, InInitializationOrderModuleList);
		DebugOutW( L"Module:%s, base:0x%X\r\n", pstEntry->FullDllName.Buffer, pstEntry->EntryPoint);
		if ( pstEntry->DllBase == hModMyself)
		{
			pstEntry->InInitializationOrderModuleList.Flink->Blink = pstEntry->InInitializationOrderModuleList.Blink;
			pstEntry->InInitializationOrderModuleList.Blink->Flink = pstEntry->InInitializationOrderModuleList.Flink;
			DebugOut( _T( "Hide injected dll."));
			break;
		}
		Current = pstEntry->InInitializationOrderModuleList.Flink;
	}
	//DebugOut( _T("Out HideMyself\r\n"));
	return TRUE;
}