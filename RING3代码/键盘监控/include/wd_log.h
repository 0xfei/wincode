/* Jungo Confidential. Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */

#ifndef _WD_LOG_H_
#define _WD_LOG_H_

#ifdef __cplusplus
    extern "C" {
#endif

ULONG DLLCALLCONV WdFunctionLog(DWORD wFuncNum, HANDLE h, PVOID pParam,
    DWORD dwSize, BOOL fWait);
HANDLE DLLCALLCONV WD_OpenLog(void);
void DLLCALLCONV WD_CloseLog(HANDLE hWD);

DWORD DLLCALLCONV WD_LogStart(const char *sFileName, const char *sMode);
VOID DLLCALLCONV WD_LogStop(void);
VOID DLLCALLCONV WD_LogAdd(const char *sFormat, ...);

#undef WD_FUNCTION
#undef WD_Close
#undef WD_Open

#define WD_FUNCTION WdFunctionLog
#define WD_Close WD_CloseLog
#define WD_Open WD_OpenLog

#ifdef __cplusplus
} 
#endif
        
#endif

