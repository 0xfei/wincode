#include <ntifs.h>
#include <windef.h>

#define DLL_PROCESS_ATTACH 1

BOOL WINAPI DllMain(HINSTANCE DllInstance, DWORD Reason, LPVOID Reserved)
{
    UNREFERENCED_PARAMETER(DllInstance);
    UNREFERENCED_PARAMETER(Reserved);

    switch( Reason )
    { 
    case DLL_PROCESS_ATTACH:
        DbgPrint("inject.dll @ DLL_PROCESS_ATTACH\n");
        break;

    default:
        break;
    }

    return TRUE;  
}