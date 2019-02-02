/* Jungo Confidential. Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */

#ifndef _WINDRVR_EVENTS_H_
#define _WINDRVR_EVENTS_H_

#include "windrvr.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus 

typedef void (*EVENT_HANDLER)(WD_EVENT *pEvent, void *pData);
typedef void event_handle_t;

DWORD DLLCALLCONV EventRegister(HANDLE *phEvent, HANDLE hWD, WD_EVENT *pEvent,
    EVENT_HANDLER pFunc, void *pData);
DWORD DLLCALLCONV EventUnregister(HANDLE hEvent);

WD_EVENT * DLLCALLCONV EventAlloc(DWORD dwNumMatchTables);
void DLLCALLCONV EventFree(WD_EVENT *pe);
WD_EVENT * DLLCALLCONV EventDup(WD_EVENT *peSrc);
WD_EVENT * DLLCALLCONV UsbEventCreate(WDU_MATCH_TABLE *pMatchTables, 
    DWORD dwNumMatchTables, DWORD dwOptions, DWORD dwAction);
WD_EVENT * DLLCALLCONV PciEventCreate(WD_PCI_ID cardId, WD_PCI_SLOT pciSlot,
    DWORD dwOptions, DWORD dwAction);
WD_EVENT * DLLCALLCONV PcmciaEventCreate(WD_PCMCIA_ID deviceId, WD_PCMCIA_SLOT pcmciaSlot,
    DWORD dwOptions, DWORD dwAction);
#ifdef __cplusplus
}
#endif  // __cplusplus 

#endif

