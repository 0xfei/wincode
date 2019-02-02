#ifndef _THUNK_H
#define _THUNK_H

typedef NTSTATUS NTAPI LDR_LOAD_DLL (IN PWSTR DllPath OPTIONAL,IN PULONG DllCharacteristics OPTIONAL,IN PUNICODE_STRING DllName,OUT PVOID *DllHandle);
typedef LDR_LOAD_DLL *PLDR_LOAD_DLL;

typedef NTSTATUS NTAPI TEST_ALERT(VOID);
typedef TEST_ALERT *PTEST_ALERT;

#define MAX_PATH            260
#define PAYLOAD_CODE_LEN    64

typedef struct _INJECT_DLL_CONTEXT {
    typedef union{
        ULONG64         Pad1;
        PLDR_LOAD_DLL   LoadRoutine;
    };
    typedef union{
        ULONG64         Pad2;
        PTEST_ALERT     TestAlertRoutine;
    };
    USHORT              DllNameLength;
    WCHAR               DllName[MAX_PATH];
}INJECT_DLL_CONTEXT, *PINJECT_CONTEXT;

#endif