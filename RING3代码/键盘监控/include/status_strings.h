/* Jungo Confidential. Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */

#ifndef _STATUS_STRINGS_H_
#define _STATUS_STRINGS_H_

#if defined(__cplusplus)
    extern "C" {
#endif

#include "windrvr.h"
        
const char * DLLCALLCONV Stat2Str(DWORD dwStatus);
#if defined(WIN32) && !defined(__KERNEL__)
BSTR DLLCALLCONV VB_Stat2Str(DWORD dwStatus);
#endif

#ifdef __cplusplus
}
#endif

#endif
