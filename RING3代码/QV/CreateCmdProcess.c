#include "QV.h"


int CreateCmdProcess(PCMD_PIPE_INFO CmdPipe, HANDLE * CmdProc)
{
	int ret;
	STARTUPINFO CmdStartInfo = {0};
	PROCESS_INFORMATION CmdInfo;
	SECURITY_ATTRIBUTES sa1 = {0},sa2 = {0};
	sa1.bInheritHandle = TRUE;
	sa2.bInheritHandle = TRUE;
	
	ret = CreatePipe(&CmdPipe->CmdInput,
		&CmdPipe->OutputPipe, &sa1, STR_MAX_LEN);
	if(ret == 0)
	{
		return 0;
	}
	
	ret = CreatePipe(&CmdPipe->InputPipe,
		&CmdPipe->CmdOutput, &sa2, 0);
	if(ret == 0)
	{
		return 0;
	}
	
	GetStartupInfo(&CmdStartInfo);
	CmdStartInfo.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	CmdStartInfo.wShowWindow = SW_SHOW;
	CmdStartInfo.hStdInput = CmdPipe->CmdInput;
	CmdStartInfo.hStdOutput = CmdPipe->CmdOutput;
	CmdStartInfo.hStdError = CmdPipe->CmdOutput;
	
	ret = CreateProcess(NULL, "c:\\windows\\system32\\cmd.exe", NULL, NULL, 1, 
		0, NULL, NULL, &CmdStartInfo, &CmdInfo);
	if(ret == 0)
	{
		ret = GetLastError();
		return 0;
	}
	
	CmdPipe->Process = CmdInfo.hProcess;
	
	*CmdProc = CmdInfo.hProcess;

	return 1;
	
}