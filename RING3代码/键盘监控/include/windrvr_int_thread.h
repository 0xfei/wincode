/* Jungo Confidential. Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */

#ifndef _WINDRVR_INT_THREAD_H_
#define _WINDRVR_INT_THREAD_H_

#include "windrvr.h"
#include "windrvr_events.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus 

typedef void (DLLCALLCONV * INT_HANDLER)(PVOID pData);
typedef INT_HANDLER INT_HANDLER_FUNC;

// OLD prototypes for backward compatibility
// implemented in src/windrvr_int_thread.c
BOOL InterruptThreadEnable(HANDLE *phThread, HANDLE hWD, WD_INTERRUPT *pInt,
    INT_HANDLER func, PVOID pData);
void InterruptThreadDisable(HANDLE hThread);

// implemented in src/windrvr_events.c
event_handle_t *event_register(HANDLE hWD, WD_EVENT *event,
    EVENT_HANDLER func, void *data);
void event_unregister(HANDLE hWD, event_handle_t *handle);


// New prototypes. Functions return status.
DWORD DLLCALLCONV InterruptEnable(HANDLE *phThread, HANDLE hWD,
    WD_INTERRUPT *pInt, INT_HANDLER func, PVOID pData);

DWORD DLLCALLCONV InterruptDisable(HANDLE hThread);

#ifdef __cplusplus
}
#endif  // __cplusplus 

#endif

