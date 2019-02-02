/* Jungo Confidential. Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */

#ifndef _WD_UTILS_H_
#define _WD_UTILS_H_

#if defined (__KERNEL_DRIVER__)
    #include "kd.h"
#elif defined (__KERNEL__)
    #include "kpstdlib.h"
#endif

#include "windrvr.h"

#if defined(__cplusplus)
extern "C" {
#endif

#if !defined MAX_PATH
    #define MAX_PATH 4096
#endif

#if !defined(LINUX) && !defined(SOLARIS)
    #define snprintf _snprintf
    #if !defined(vsnprintf)
        #define vsnprintf _vsnprintf
    #endif
#endif

typedef void (DLLCALLCONV *HANDLER_FUNC)(void *pData);

#if !defined(WIN32) || defined(WINCE) || defined(_MT) 
DWORD DLLCALLCONV ThreadStart(HANDLE *phThread, HANDLER_FUNC pFunc, void *pData);
void DLLCALLCONV ThreadWait(HANDLE hThread);
// For backward compatibility - wrapper to ThreadWait:
void DLLCALLCONV ThreadStop(HANDLE hThread);
#endif

DWORD DLLCALLCONV OsEventCreate(HANDLE *phOsEvent);
void DLLCALLCONV OsEventClose(HANDLE hOsEvent);
DWORD DLLCALLCONV OsEventWait(HANDLE hOsEvent, DWORD dwSecTimeout);
DWORD DLLCALLCONV OsEventSignal(HANDLE hOsEvent);
DWORD DLLCALLCONV OsEventReset(HANDLE hOsEvent);
DWORD DLLCALLCONV OsMutexCreate(HANDLE *phOsMutex);
void DLLCALLCONV OsMutexClose(HANDLE hOsMutex);
DWORD DLLCALLCONV OsMutexLock(HANDLE hOsMutex);
DWORD DLLCALLCONV OsMutexUnlock(HANDLE hOsMutex);

void DLLCALLCONV SleepWrapper(DWORD dwMicroSecs);

#if !defined(__KERNEL__)
void DLLCALLCONV vPrintDbgMessage(DWORD dwLevel, DWORD dwSection, 
    const char *format, va_list ap);
void DLLCALLCONV PrintDbgMessage(DWORD dwLevel, DWORD dwSection, 
    const char *format, ...);
int DLLCALLCONV GetPageSize(void);
DWORD DLLCALLCONV GetPageCount(void *pBuf, DWORD dwSize);
BOOL DLLCALLCONV UtilGetFileSize(const PCHAR sFileName, DWORD *dwFileSize, 
    PCHAR sErrString);

DWORD DLLCALLCONV UtilGetStringFromUser(PCHAR pcString, DWORD dwSizeStr, 
    const CHAR *pcInputText, const CHAR *pcDefaultString);

DWORD DLLCALLCONV UtilGetFileName(PCHAR pcFileName, DWORD dwFileNameSize, 
    const CHAR *pcDefaultFileName);

void DLLCALLCONV UtilClrScr(void);
#endif

#if defined(LINUX) || defined(SOLARIS)
    #if !defined(stricmp)
        #define stricmp strcasecmp
    #endif
    #if !defined(strnicmp)
        #define strnicmp strncasecmp
    #endif
#endif
#if defined(WINCE)
    #if !defined(stricmp)
        #define stricmp _stricmp
    #endif
    #if !defined(strnicmp)
        #define strnicmp _strnicmp
    #endif
#endif
#if !defined(INFINITE)
    #define INFINITE 0xffffffff 
#endif
#ifdef __cplusplus
}
#endif

#endif
