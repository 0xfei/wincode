#include <ntddk.h>
#include "thunk.h"

VOID LoadInjectDllThunk(PVOID Context,PVOID SystemArgument1,PVOID SystemArgument2)
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