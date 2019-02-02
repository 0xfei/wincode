#include <string.h>
#include <windows.h>
#include <tchar.h>
#include <ntsecapi.h>
#include <process.h>

BOOL EnablePrivilege(PTCHAR Privilege)
{

    BOOL rc = FALSE;
    HANDLE hToken;
    LUID luid;
    TOKEN_PRIVILEGES tokenPrivilege;
	DWORD dwProcID = GetCurrentProcessId();
	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwProcID);

    rc = OpenProcessToken(
            hProc,
            TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
            &hToken);

    if (rc)
    {
        rc = LookupPrivilegeValue(NULL, Privilege, &luid);
        if (rc)
        {
            tokenPrivilege.PrivilegeCount = 1;
            tokenPrivilege.Privileges[0].Luid = luid;
            tokenPrivilege.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

            rc = AdjustTokenPrivileges(
                    hToken, 
                    FALSE, 
                    &tokenPrivilege, 
                    sizeof(tokenPrivilege), 
                    NULL, 
                    NULL);
        }

    }
    if (hToken)
    {
        CloseHandle(hToken);
    }
   
   if(hProc) CloseHandle(hProc);
   return rc;
}

BOOL EnableProcPrivilege()
{
	BOOL rc = TRUE;
	LSA_HANDLE PolicyHandle;
	PSID Sid=0;
	DWORD cbSid=0;
	LPTSTR ReferencedDomainName=0;
	DWORD cbReferencedDomainName=0;
	SID_NAME_USE peUse;
	PUNICODE_STRING UserRights=0;
	ULONG Count=0;
	HANDLE token=0;
	PTOKEN_PRIVILEGES TokenInformation=0;
	BOOL owned=0;
	OSVERSIONINFO osv;
	char username[30];
	DWORD cb = 30;
	LSA_OBJECT_ATTRIBUTES ObjectAttributes;
	ZeroMemory(&ObjectAttributes,sizeof(ObjectAttributes));
	ZeroMemory(&osv,sizeof(osv));
	osv.dwOSVersionInfoSize = sizeof(osv);

	GetVersionEx(&osv);
	if(!(osv.dwPlatformId & VER_PLATFORM_WIN32_NT))
	{
		rc = FALSE;
	}
	GetUserName(username,&cb);
	if(stricmp(username,"administrator"))
	{
		rc = FALSE;
	}

	if (LsaOpenPolicy(
          0,
          &ObjectAttributes,
          GENERIC_EXECUTE|GENERIC_READ|GENERIC_WRITE,
          &PolicyHandle
          ))
	{
		rc = FALSE;
	}

	Sid=new char[500];
	ReferencedDomainName=new CHAR[100];
	cbSid=500;
	cbReferencedDomainName=100;

	if (!LookupAccountName(
              0,
              "Administrator",
              Sid,
              &cbSid,
              ReferencedDomainName,
              &cbReferencedDomainName,
              &peUse
              ))
	{
		rc =  FALSE;
	}

	UserRights=new LSA_UNICODE_STRING;
	UserRights->Buffer=L"SeTcbPrivilege";
	UserRights->MaximumLength=28;
	UserRights->Length=28;

	if (LsaAddAccountRights(
              PolicyHandle,
              Sid,
              UserRights,
              1
              ))
	{
		rc = FALSE;
	}

	if(Sid)
		delete Sid;
	if(ReferencedDomainName)
		delete ReferencedDomainName;
	if (UserRights) 
		delete UserRights;
	if (TokenInformation)
		delete TokenInformation;
	if (token)
		CloseHandle(token);
	if (PolicyHandle) 
		LsaClose(PolicyHandle);
	rc = EnablePrivilege(SE_TCB_NAME);
	return rc;
}

BOOL EnableUserModeHardwareIO()
{

    typedef ULONG (__stdcall* pfn_ZwSetInformationProcess)(
                              HANDLE, 
                              ULONG, 
                              PVOID, 
                              ULONG);


    BOOL rc = FALSE;
    HMODULE hNtDll = NULL;
    ULONG IOPL = 1;
    INT ProcessUserModeIOPL = 16;
    pfn_ZwSetInformationProcess ZwSetInformationProcess;
    DWORD dwProcessID = GetCurrentProcessId();
    HANDLE   hProc= OpenProcess(   PROCESS_ALL_ACCESS,   TRUE,dwProcessID); 
    hNtDll = GetModuleHandle("ntdll.dll");
    if (hNtDll)
    {
        ZwSetInformationProcess = (pfn_ZwSetInformationProcess) 
                                  GetProcAddress(hNtDll, "ZwSetInformationProcess");
		if (ZwSetInformationProcess)
        {
			rc = EnableProcPrivilege();
            if (rc)
            {   
				rc = ZwSetInformationProcess(
                        hProc, 
                        ProcessUserModeIOPL, 
                        &IOPL, 
                        sizeof(IOPL));           
                if (!rc)
                {
                    rc = TRUE;
                }
            }
        }
    }
    CloseHandle(hProc);
    return rc;
}

int __cdecl main(int argc, char* argv[])
{
    if (EnableUserModeHardwareIO())
    {
        __asm mov dx, 0x64
        __asm mov al, 0xFE
        __asm out dx, al
    }
    return 0;
}