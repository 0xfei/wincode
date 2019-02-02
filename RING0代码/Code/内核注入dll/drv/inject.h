#include "advlist.h"
#include "undoc.h"

#ifndef _INJECT_H
#define _INJECT_H

#define DEFAULT_PROCESS_HASH_LEN    10003
#define CALC_PROCESS_HASH(Pid)      ((ULONG_PTR)Pid % ProcessHashLen)

typedef struct _PROCESS_TABLE{
    HLIST_ENTRY     HashList;
    LIST_ENTRY      GlobalList;

    HANDLE          Pid;
    ANSI_STRING     FullImageName;
    BOOLEAN         FirstLoad;
    BOOLEAN         ApcInserted;
    BOOLEAN         ForceDllInjection;
    BOOLEAN         DontInject;

    BOOLEAN         IsWow64;
    PVOID           ImageBase;
    PVOID           NtdllBase;
    PVOID           Wow64NtdllBase;
    PVOID           Wow64Base;

    PVOID           InjectCodePtr;
    PVOID           InjectContext;
}PROCESS_TABLE,*PPROCESS_TABLE;

NTSTATUS UndocInitialize();
NTSTATUS ProcessInitialize();
VOID ProcessUnload();

#endif
