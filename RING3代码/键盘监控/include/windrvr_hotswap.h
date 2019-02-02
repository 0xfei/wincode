/* Jungo Confidential. Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */

#ifndef _WINDRVR_HOTSWAP_H_
#define _WINDRVR_HOTSWAP_H_

#include "windrvr.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus 

WD_HS_WATCH * DLLCALLCONV HSWatchAlloc(DWORD dwNumMatchTables);
void DLLCALLCONV HSWatchFree(WD_HS_WATCH *pWatch);
BOOL DLLCALLCONV HSConfigurePci(WD_EVENT *pEvent);

#ifdef __cplusplus
}
#endif  // __cplusplus 

#endif

