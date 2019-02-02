/* Jungo Confidential. Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */

/*
 * W i n D r i v e r
 * =================
 *
 * FOR DETAILS ON THE WinDriver FUNCTIONS, PLEASE SEE THE WinDriver MANUAL
 * OR INCLUDED HELP FILES.
 *
 * This file may not be distributed, it may only be used for development
 * or evaluation purposes. The only exception is distribution to Linux.
 * For details refer to \WinDriver\docs\license.txt.
 *
 * Web site: http://www.jungo.com
 * Email:    support@jungo.com
 */
#ifndef _WINDRVR_H_
#define _WINDRVR_H_
#if defined(__cplusplus)
    extern "C" {
#endif

#include "wd_ver.h"

// these lines break standard VxWorks builds. remove the -asni flag from the
// compiler flags (from the makefile or the Tornado IDE) to compile WinDriver
// related source code.
#if defined(WIN32)
    #define DLLCALLCONV __stdcall
#else
    #define DLLCALLCONV
#endif
        
#if defined(VXWORKS)
    #define WD_PROD_NAME "DriverBuilder"
#else
    #define WD_PROD_NAME "WinDriver"
#endif

#if !defined(SPARC) && (defined(__sparc__) || defined (__sparc) || \
        defined(sparc))
    #define SPARC
#endif

#if !defined(POWERPC) && !defined(PPC64) && !defined(IA64) && \
    !defined(x86) && ( defined(LINUX) || (defined(WIN32) && \
    !defined(WINCE) && !defined(_ALPHA_)) || (defined(SOLARIS) \
     && defined(i386)) )
    #define x86
#endif

#if !defined(x86_64) && defined(x86) && defined(KERNEL_64BIT)
    #define x86_64
#endif

#if defined(VXWORKS)
    #if defined(MBX860)
        #define WD_CPU_SPEC " PPC860"
    #elif defined(MCP750)
        #define WD_CPU_SPEC " PPC750"
    #elif defined(x86)
        #define WD_CPU_SPEC " X86"
    #else
        #define WD_CPU_SPEC ""
    #endif
#elif defined(WINCE)
    #if defined(ARM)
        #define WD_CPU_SPEC " ARM"
    #elif defined(MIPS)
        #define WD_CPU_SPEC " MIPS"
    #elif defined(x86)
        #define WD_CPU_SPEC " x86"
    #else
        #define WD_CPU_SPEC ""
    #endif
#else
    #if defined(SPARC)
        #define WD_CPU_SPEC " Sparc"
    #elif defined(POWERPC)
        #define WD_CPU_SPEC " PowerPC"
    #elif defined(PPC64)
        #define WD_CPU_SPEC " ppc64"
    #elif defined(x86_64)
        #define WD_CPU_SPEC " x86_64"
    #elif defined(IA64)
        #define WD_CPU_SPEC " ia64"
    #else
        #define WD_CPU_SPEC " X86"
    #endif
#endif

#if defined(WINNT)
    #define WD_FILE_FORMAT " SYS"
#elif defined(WINCE)
    #define WD_FILE_FORMAT " DLL"
#else
    #define WD_FILE_FORMAT ""
#endif

#if defined(KERNEL_64BIT)
    #define WD_DATA_MODEL " 64bit"
#else
    #define WD_DATA_MODEL " 32bit"
#endif

#define WD_VER_STR  WD_PROD_NAME " v" WD_VERSION_STR \
        " Jungo (c) 1997 - 2008 Build Date: " __DATE__ \
        WD_CPU_SPEC WD_DATA_MODEL WD_FILE_FORMAT

#if !defined(UNIX) && (defined(LINUX) || defined(SOLARIS) || defined(VXWORKS))
    #define UNIX
#endif

#if !defined(WIN32) && (defined(WINCE) || defined(WINNT))
    #define WIN32
#endif

#if defined(_WIN32_WCE) && !defined(WINCE)
    #define WINCE
#endif

#if !defined(WIN32) && !defined(WINCE) && !defined(UNIX)
    #define WIN32
#endif

#if defined(_KERNEL) && !defined(__KERNEL__)
    #define __KERNEL__
#endif

#if defined( __KERNEL__) && !defined(_KERNEL)
    #define _KERNEL
#endif

#if !defined(POWERPC) && defined(VXWORKS) && !defined(x86)
    #define POWERPC
#endif
#if !defined(_BIGENDIAN) && (defined(SPARC) || defined(POWERPC) || \
    defined(PPC64))
    #define _BIGENDIAN
#endif

/* Structures on PPC64 are 8 byte aligned, even on 32 bit applications, 
 * therefore packing is necessary. */
#if defined(LINUX) && defined(PPC64)
#pragma pack(push, 1)
#endif

#if defined(UNIX)
    #if !defined(__P_TYPES__)
        #define __P_TYPES__
        #if !defined(VXWORKS)
            typedef void VOID;
            typedef unsigned char UCHAR;
            typedef unsigned short USHORT;
            typedef unsigned int UINT;
            typedef unsigned long ULONG;
            typedef ULONG BOOL;
        #endif
        typedef void *PVOID;
        typedef unsigned char *PBYTE;
        typedef char CHAR;
        typedef char *PCHAR;
        typedef unsigned short *PWORD;
        typedef unsigned long DWORD, *PDWORD;
        typedef PVOID HANDLE;
    #endif
    #if !defined(__KERNEL__)
        #include <string.h>
        #include <ctype.h>
        #include <stdlib.h>
    #endif
    #ifndef TRUE
        #define TRUE 1
    #endif
    #ifndef FALSE
        #define FALSE 0
    #endif
    #define __cdecl
    #define WINAPI

    #if defined(__KERNEL__)
        #if defined(SOLARIS)
            #include <sys/conf.h>
            #include <sys/modctl.h>
            #include <sys/ksynch.h>
            #include <sys/cmn_err.h>
            #include <sys/types.h>
            #include <sys/file.h>
            #include <sys/errno.h>
            #include <sys/open.h>
            #include <sys/cred.h>
            #include <sys/kmem.h>
            #include <sys/devops.h>
            #include <sys/spl.h>
            #include <sys/varargs.h>
            #include <sys/time.h>
            #include <sys/proc.h>
            #include <sys/stat.h>
            #include <sys/ddi_impldefs.h>
            #include <sys/ddi_implfuncs.h>
            #include <sys/ddidmareq.h>
            #include <sys/promif.h>
            #include <sys/ddi.h>
            #include <sys/sunddi.h>
            #include <sys/stream.h>

            #ifdef u // solaris defines u in sys/user.h for imbicilic reasons.
                #undef u
            #endif

            #ifdef BYTE //defined in sys/promif.h
                #undef BYTE
            #endif
        #elif defined(LINUX)
            #include <asm/ioctl.h> /* for _IO macros */
            #include <linux/errno.h> /* For mapping linux statuses codes WD
                                        status codes */
        #endif
    #else
        #include <unistd.h>
        #if defined(LINUX)
            #include <sys/ioctl.h> /* for BSD ioctl() */
            #include <unistd.h>
        #else
            #include <unistd.h> /* for SVR4 ioctl()*/
        #endif
        #if defined(VXWORKS)
            #include <vxWorks.h>
            #undef SPARC /* defined in vxworks.h */
            #include <string.h>
            #include <memLib.h>
            #include <stdlib.h>
            #include <taskLib.h>
            #include <ioLib.h>
            #include <iosLib.h>
            #include <taskLib.h>
            #include <semLib.h>
            #include <timers.h>
        #endif
        #include <sys/types.h>
        #include <sys/stat.h>
        #include <fcntl.h>
    #endif
    #if defined(SOLARIS)
        typedef uint64_t UINT64;
    #else
        typedef unsigned long long UINT64;
    #endif
#elif defined(WINCE)
    #include <windows.h>
    #include <winioctl.h>
    typedef char CHAR;
    typedef unsigned __int64 UINT64;
    #define perror(str) fprintf(stderr, "%s: error no. %d\n", (str), \
        GetLastError())
    #if defined(PRIO_WIN32_TO_CE)
        #define WD_CE6_PRIORITY(prio) PRIO_WIN32_TO_CE(prio)
    #else
        /* define CE6_256 priority level as defined in winbase.h 
         * MAX_CE_PRIORITY_LEVELS = 256
         * MAX_WIN32_PRIORITY_LEVELS = 8 
         * this definition exist only for CE 6.00 and up
         */
        #define WD_CE6_PRIORITY(prio) ((prio) + 256 - 8)
    #endif
#elif defined(WIN32)
    #if defined(__KERNEL__)
        int sprintf(char *buffer, const char *format, ...);
    #else
        #include <windows.h>
        #include <winioctl.h>
    #endif
    typedef unsigned __int64 UINT64;
#endif

#if !defined(__KERNEL__)
    #include <stdarg.h>
    #if !defined(va_copy) && !defined(__va_copy) 
        #define va_copy(ap2,ap1) (ap2)=(ap1)
    #endif
    #if !defined(va_copy) && defined(__va_copy) 
        #define va_copy __va_copy
    #endif
#endif

#ifndef WINAPI
    #define WINAPI
#endif

#if !defined(_WINDEF_)
    typedef unsigned char BYTE;
    typedef unsigned short int WORD;
#endif

#if !defined(_BASETSD_H_)
    typedef unsigned int UINT32;
#endif

// formatting for printing a 64bit variable
#if defined(SOLARIS)
    #if defined(_LP64)
        #define PRI64   "l"
    #else
        #define PRI64   "ll"
    #endif
#elif defined(UNIX)
    #define PRI64       "ll"
#elif defined(WIN32)
    #define PRI64       "I64"
#else
#endif

// formatting for printing a kernel pointer
#if defined(KERNEL_64BIT)
    #define KPRI PRI64
#else
    #define KPRI "l"
#endif

/*
 * The KPTR is guaranteed to be the same size as a kernel-mode pointer
 * The UPTR is guaranteed to be the same size as a user-mode pointer
 */
#if defined(KERNEL_64BIT)
    typedef UINT64 KPTR;
#else
    typedef DWORD KPTR;
#endif

#if defined(__KERNEL_)
    typedef KPTR UPTR;
#else
    #if defined(UNIX)
        typedef unsigned long UPTR;
    #else
        typedef size_t UPTR;
    #endif
#endif

#if !defined(VXWORKS)
typedef UINT64 DMA_ADDR;
typedef UINT64 PHYS_ADDR;
#else
typedef DWORD DMA_ADDR;
typedef DWORD PHYS_ADDR;
#endif

#include "windrvr_usb.h"

#if defined(WIN32)
    #if !defined(inline)
        #define inline __inline
    #endif
#endif

// IN WD_TRANSFER_CMD and WD_Transfer() DWORD stands for 32 bits and QWORD is 64
// bit.
typedef enum
{
    CMD_NONE = 0,       // No command
    CMD_END = 1,        // End command
    CMD_MASK = 2,       // Interrupt Mask

    RP_BYTE = 10,       // Read port byte
    RP_WORD = 11,       // Read port word
    RP_DWORD = 12,      // Read port dword
    WP_BYTE = 13,       // Write port byte
    WP_WORD = 14,       // Write port word
    WP_DWORD = 15,      // Write port dword
    RP_QWORD = 16,      // Read port qword
    WP_QWORD = 17,      // Write port qword
    

    RP_SBYTE = 20,      // Read port string byte
    RP_SWORD = 21,      // Read port string word
    RP_SDWORD = 22,     // Read port string dword
    WP_SBYTE = 23,      // Write port string byte
    WP_SWORD = 24,      // Write port string word
    WP_SDWORD = 25,     // Write port string dword
    RP_SQWORD = 26,      // Read port string qword
    WP_SQWORD = 27,      // Write port string qword

    RM_BYTE = 30,       // Read memory byte
    RM_WORD = 31,       // Read memory word
    RM_DWORD = 32,      // Read memory dword
    WM_BYTE = 33,       // Write memory byte
    WM_WORD = 34,       // Write memory word
    WM_DWORD = 35,      // Write memory dword
    RM_QWORD = 36,      // Read memory qword
    WM_QWORD = 37,      // Write memory qword

    RM_SBYTE = 40,      // Read memory string byte
    RM_SWORD = 41,      // Read memory string word
    RM_SDWORD = 42,     // Read memory string dword
    WM_SBYTE = 43,      // Write memory string byte
    WM_SWORD = 44,      // Write memory string word
    WM_SDWORD = 45,     // Write memory string dword
    RM_SQWORD = 46,     // Read memory string quad word
    WM_SQWORD = 47      // Write memory string quad word
} WD_TRANSFER_CMD;

enum { WD_DMA_PAGES = 256 };

enum {
    DMA_KERNEL_BUFFER_ALLOC = 0x1, // The system allocates a contiguous buffer.
        // The user does not need to supply linear address.

    DMA_KBUF_BELOW_16M = 0x2, // If DMA_KERNEL_BUFFER_ALLOC is used,
        // this will make sure it is under 16M.

    DMA_LARGE_BUFFER = 0x4, // If DMA_LARGE_BUFFER is used,
        // the maximum number of pages are dwPages, and not
        // WD_DMA_PAGES. If you lock a user buffer (not a kernel
        // allocated buffer) that is larger than 1MB, then use this
        // option and allocate memory for pages.

    DMA_ALLOW_CACHE = 0x8,  // Allow caching of contiguous memory. 

    DMA_KERNEL_ONLY_MAP = 0x10, // Only map to kernel, dont map to user-mode.
        // relevant with DMA_KERNEL_BUFFER_ALLOC flag only

    DMA_FROM_DEVICE = 0x20, // memory pages are locked to be written by device
    
    DMA_TO_DEVICE = 0x40, // memory pages are locked to be read by device

    DMA_TO_FROM_DEVICE = (DMA_FROM_DEVICE | DMA_TO_DEVICE), // memory pages are
        // locked for both read and write
    
    DMA_ALLOW_64BIT_ADDRESS = 0x80, // Use this value for devices that support
                                    // 64-bit DMA addressing.

    DMA_ALLOW_NO_HCARD = 0x100, // allow memory lock without hCard 
};

#define DMA_DIRECTION_MASK DMA_TO_FROM_DEVICE

/* Macros for backwards compatibility */
#define DMA_READ_FROM_DEVICE DMA_FROM_DEVICE
#define DMA_WRITE_TO_DEVICE DMA_TO_DEVICE

enum {
    WD_MATCH_EXCLUDE = 0x1 // exclude if there is a match
};

enum {
    WD_CPCI_PNP_SUPPORT = 0x1 // set this flag in case we want to use OS PnP
                              // support
};

typedef struct
{
    DMA_ADDR pPhysicalAddr;   // Physical address of page.
    DWORD dwBytes;          // Size of page.
} WD_DMA_PAGE, WD_DMA_PAGE_V80;

typedef struct
{
    DWORD hDma;             // Handle of dma buffer.
    PVOID pUserAddr;        // Beginning of buffer.
    KPTR  pKernelAddr;      // Kernel mapping of kernel allocated buffer
    DWORD dwBytes;          // Size of buffer.
    DWORD dwOptions;        // Allocation options:
                            // DMA_KERNEL_BUFFER_ALLOC, DMA_KBUF_BELOW_16M,
                            // DMA_LARGE_BUFFER
                            // DMA_ALLOW_CACHE, DMA_KERNEL_ONLY_MAP,
                            // DMA_FROM_DEVICE,
                            // DMA_TO_DEVICE, DMA_ALLOW_64BIT_ADDRESS
    DWORD dwPages;          // Number of pages in buffer.
    DWORD hCard;            // Handle of relevant card as received from 
                            // WD_CardRegister()
    WD_DMA_PAGE Page[WD_DMA_PAGES];
} WD_DMA, WD_DMA_V80;

typedef struct
{
    KPTR dwPort;       // IO port for transfer or kernel memory address.
    DWORD cmdTrans;    // Transfer command WD_TRANSFER_CMD.

    // Parameters used for string transfers:
    DWORD dwBytes;     // For string transfer.
    DWORD fAutoinc;    // Transfer from one port/address
                       // or use incremental range of addresses.
    DWORD dwOptions;   // Must be 0.
    union
    {
        BYTE Byte;     // Use for 8 bit transfer.
        WORD Word;     // Use for 16 bit transfer.
        UINT32 Dword;   // Use for 32 bit transfer.
        UINT64 Qword;  // Use for 64 bit transfer.
        PVOID pBuffer; // Use for string transfer.
    } Data;
} WD_TRANSFER, WD_TRANSFER_V61;

enum {
    INTERRUPT_LATCHED = 0x00,
    INTERRUPT_LEVEL_SENSITIVE = 0x01,
    INTERRUPT_CMD_COPY = 0x02,
    INTERRUPT_CE_INT_ID = 0x04,
    INTERRUPT_CMD_RETURN_VALUE = 0x08,
    INTERRUPT_MESSAGE = 0x10,
    INTERRUPT_MESSAGE_X = 0x20
};

typedef struct
{
    DWORD hKernelPlugIn;
    DWORD dwMessage;
    PVOID pData;
    DWORD dwResult;
} WD_KERNEL_PLUGIN_CALL, WD_KERNEL_PLUGIN_CALL_V40;

typedef enum {
    INTERRUPT_RECEIVED = 0, /* Interrupt was received */
    INTERRUPT_STOPPED,      /* Interrupt was disabled during wait */
    INTERRUPT_INTERRUPTED   /* Wait was interrupted before an actual hardware interrupt was received */
} WD_INTERRUPT_WAIT_RESULT;

typedef struct
{
    DWORD hInterrupt;    // Handle of interrupt.
    DWORD dwOptions;     // Interrupt options: can be INTERRUPT_CMD_COPY

    WD_TRANSFER *Cmd;    // Commands to do on interrupt.
    DWORD dwCmds;        // Number of commands.

    // For WD_IntEnable():
    WD_KERNEL_PLUGIN_CALL kpCall; // Kernel PlugIn call.
    DWORD fEnableOk;     // TRUE if interrupt was enabled (WD_IntEnable() succeed).

    // For WD_IntWait() and WD_IntCount():
    DWORD dwCounter;     // Number of interrupts received.
    DWORD dwLost;        // Number of interrupts not yet dealt with.
    DWORD fStopped;      // Was interrupt disabled during wait.

    DWORD dwLastMessage; // Message data of the last received MSI (Windows)
    DWORD dwEnabledIntType; // Interrupt type that was actually enabled
} WD_INTERRUPT, WD_INTERRUPT_V91;

typedef struct
{
    DWORD dwVer;
    CHAR cVer[128];
} WD_VERSION, WD_VERSION_V30;

enum
{
    LICENSE_DEMO     = 0x00000001,
    LICENSE_WD       = 0x00000004,
    LICENSE_IO       = 0x00000008,
    LICENSE_MEM      = 0x00000010,
    LICENSE_INT      = 0x00000020,
    LICENSE_PCI      = 0x00000040,
    LICENSE_DMA      = 0x00000080,
    LICENSE_NT       = 0x00000100,
    LICENSE_95       = 0x00000200,
    LICENSE_ISAPNP   = 0x00000400,
    LICENSE_PCMCIA   = 0x00000800,
    LICENSE_PCI_DUMP = 0x00001000,
    LICENSE_MSG_GEN  = 0x00002000,
    LICENSE_MSG_EDU  = 0x00004000,
    LICENSE_MSG_INT  = 0x00008000,
    LICENSE_KER_PLUG = 0x00010000,
    LICENSE_LINUX    = 0x00020000,
    LICENSE_CE       = 0x00080000,
    LICENSE_VXWORKS  = 0x10000000,
    LICENSE_THIS_PC  = 0x00100000,
    LICENSE_WIZARD   = 0x00200000,
    LICENSE_KD       = 0x00400000,
    LICENSE_SOLARIS  = 0x00800000,
    LICENSE_CPU0     = 0x00040000,
    LICENSE_CPU1     = 0x01000000,
    LICENSE_CPU2     = 0x02000000,
    LICENSE_CPU3     = 0x04000000,
    LICENSE_USB      = 0x08000000
};

enum
{
    LICENSE2_EVENT   = 0x00000008,
    LICENSE2_WDLIB   = 0x00000010,
    LICENSE2_WDF     = 0x00000020,
    LICENSE2_WDF_MSD = 0x00000040,
    LICENSE2_VISTA   = 0x00000080,
    LICENSE2_UT      = 0x00000100
};

enum
{
    LICENSE_OS_WITH_WIZARD = LICENSE_95 | LICENSE_NT | LICENSE_LINUX | LICENSE_SOLARIS,
    LICENSE_OS_WITHOUT_WIZARD = LICENSE_CE | LICENSE_VXWORKS
};

enum
{
    LICENSE_CPU_ALL = LICENSE_CPU3 | LICENSE_CPU2 | LICENSE_CPU1 |
        LICENSE_CPU0,
    LICENSE_X86 = LICENSE_CPU0,
    LICENSE_ALPHA = LICENSE_CPU1,
    LICENSE_SPARC = LICENSE_CPU1 | LICENSE_CPU0,
    LICENSE_PPC = LICENSE_CPU2,
    LICENSE_X86_64 = LICENSE_CPU2 | LICENSE_CPU0,
    LICENSE_IA64 = LICENSE_CPU1 | LICENSE_CPU2,
    LICENSE_PPC64 = LICENSE_CPU2 | LICENSE_CPU1 | LICENSE_CPU0
};

#define WD_LICENSE_LENGTH 128
typedef struct
{
    CHAR cLicense[WD_LICENSE_LENGTH]; // Buffer with license string to put.
                      // If empty string then get current license setting
                      // into dwLicense.
    DWORD dwLicense;  // Returns license settings: LICENSE_DEMO, LICENSE_WD 
                      // etc..., or 0 for invalid license.
    DWORD dwLicense2; // Returns additional license settings, if dwLicense 
                      // could not hold all the information.
                      // Then dwLicense will return 0.
} WD_LICENSE, WD_LICENSE_V44;

enum
{
    WD_BUS_USB = (int)0xfffffffe,
    WD_BUS_UNKNOWN = 0,
    WD_BUS_ISA = 1,
    WD_BUS_EISA = 2, // ISA PnP belongs to this classification
    WD_BUS_PCI = 5,
    WD_BUS_PCMCIA = 8
};
typedef DWORD WD_BUS_TYPE;

typedef struct
{
    WD_BUS_TYPE dwBusType;        // Bus Type: ISA, EISA, PCI, PCMCIA.
    DWORD dwBusNum;         // Bus number.
    DWORD dwSlotFunc;       // Slot number on Bus.
} WD_BUS, WD_BUS_V30;

typedef enum
{
    ITEM_NONE=0,
    ITEM_INTERRUPT=1,
    ITEM_MEMORY=2,
    ITEM_IO=3,
    ITEM_BUS=5
} ITEM_TYPE;

typedef enum
{ 
    WD_ITEM_DO_NOT_MAP_KERNEL = 1, // skip the mapping of physical memory to kernel address space
    WD_ITEM_ALLOW_CACHE = 2        // map physical memory as cached (not applicable for PCI memory)
} WD_ITEM_OPTIONS;

typedef struct
{
    DWORD item; // ITEM_TYPE
    DWORD fNotSharable;
    DWORD dwReserved; // Reserved for internal use
    DWORD dwOptions; // WD_ITEM_OPTIONS
    union
    {
        struct
        { // ITEM_MEMORY
            DWORD dwPhysicalAddr;     // Physical address on card.
            DWORD dwBytes;            // Address range.
            KPTR dwTransAddr;         // Returns the address to pass on to
                                      //transfer commands.
            UPTR dwUserDirectAddr;    // Returns the address for direct user
                                      // read/write.
            DWORD dwCpuPhysicalAddr;  // Returns the CPU physical address
            DWORD dwBar;              // Base Address Register number of PCI
                                      // card.
        } Mem;
        struct
        { // ITEM_IO
            KPTR dwAddr;          // Beginning of io address.
            DWORD dwBytes;        // IO range.
            DWORD dwBar;          // Base Address Register number of PCI card.
        } IO;
        struct
        { // ITEM_INTERRUPT
            DWORD dwInterrupt; // Number of interrupt to install.
            DWORD dwOptions;   // Interrupt options:
                               //   INTERRUPT_LATCHED - latched,
                               //   INTERRUPT_LEVEL_SENSITIVE - level sensitive,
                               //   INTERRUPT_MESSAGE - MSI.
                               //   INTERRUPT_MESSAGE_X - MSI-X.
            DWORD hInterrupt;  // Returns the handle of the interrupt installed.
        } Int;
        WD_BUS Bus; // ITEM_BUS
        struct
        {
            DWORD dw1, dw2, dw3, dw4; // Reserved for internal use
            KPTR dw5; // Reserved for internal use
        } Val;
    } I;
} WD_ITEMS, WD_ITEMS_V30;

enum { WD_CARD_ITEMS = 20 };

typedef struct
{
    DWORD dwItems;
    WD_ITEMS Item[WD_CARD_ITEMS];
} WD_CARD, WD_CARD_V30;

enum { CARD_VX_NO_MMU_INIT = 0x4000000 };

typedef struct
{
    WD_CARD Card;           // Card to register.
    DWORD fCheckLockOnly;   // Only check if card is lockable, return hCard=1 if
                            // OK.
    DWORD hCard;            // Handle of card.
    DWORD dwOptions;        // Should be zero.
    CHAR cName[32];         // Name of card.
    CHAR cDescription[100]; // Description.
} WD_CARD_REGISTER, WD_CARD_REGISTER_V40;

typedef struct
{
    DWORD hCard;            // Handle of card.  
    WD_TRANSFER *Cmd;       // Buffer with WD_TRANSFER commands
    DWORD dwCmds;           // Number of commands.
    DWORD dwOptions;        // 0 (default) or WD_FORCE_CLEANUP
} WD_CARD_CLEANUP;

enum { WD_FORCE_CLEANUP = 0x1 };

enum { WD_PCI_CARDS = 100 };

typedef struct
{
    DWORD dwBus;
    DWORD dwSlot;
    DWORD dwFunction;
} WD_PCI_SLOT;

typedef struct
{
    DWORD dwVendorId;
    DWORD dwDeviceId;
} WD_PCI_ID;

typedef struct
{
    WD_PCI_ID searchId;     // If dwVendorId==0 - scan all vendor IDs.
                            // If dwDeviceId==0 - scan all device IDs.
    DWORD dwCards;          // Number of cards found.
    WD_PCI_ID cardId[WD_PCI_CARDS]; // VendorID & DeviceID of cards found.
    WD_PCI_SLOT cardSlot[WD_PCI_CARDS]; // Pci slot info of cards found.
    DWORD dwOptions;        
} WD_PCI_SCAN_CARDS, WD_PCI_SCAN_CARDS_V80;

enum {
    WD_PCI_SCAN_DEFAULT = 0x0,
    WD_PCI_SCAN_BY_TOPOLOGY = 0x1
};
    
typedef struct
{
    WD_PCI_SLOT pciSlot;    // Pci slot.
    WD_CARD Card;           // Get card parameters for pci slot.
} WD_PCI_CARD_INFO, WD_PCI_CARD_INFO_V30;

typedef enum
{
    PCI_ACCESS_OK = 0,
    PCI_ACCESS_ERROR = 1,
    PCI_BAD_BUS = 2,
    PCI_BAD_SLOT = 3
} PCI_ACCESS_RESULT;

typedef enum
{
    PCMCIA_ACCESS_OK = 0,
    PCMCIA_BAD_SOCKET = 1,
    PCMCIA_BAD_OFFSET = 2,
    PCMCIA_ACCESS_ERROR =3
} PCMCIA_ACCESS_RESULT;

typedef struct
{
    WD_PCI_SLOT pciSlot;    // Pci bus, slot and function number.
    PVOID       pBuffer;    // Buffer for read/write.
    DWORD       dwOffset;   // Offset in pci configuration space to read/write from.
    DWORD       dwBytes;    // Bytes to read/write from/to buffer.
                            // Returns the number of bytes read/written.
    DWORD       fIsRead;    // If 1 then read pci config, 0 write pci config.
    DWORD       dwResult;   // PCI_ACCESS_RESULT
} WD_PCI_CONFIG_DUMP, WD_PCI_CONFIG_DUMP_V30;

enum { WD_ISAPNP_CARDS = 16 };
enum { WD_ISAPNP_COMPATIBLE_IDS = 10 };
enum { WD_ISAPNP_COMP_ID_LENGTH = 7 }; // ISA compressed ID is 7 chars long.
enum { WD_ISAPNP_ANSI_LENGTH = 32 }; // ISA ANSI ID is limited to 32 chars long.
typedef CHAR WD_ISAPNP_COMP_ID[WD_ISAPNP_COMP_ID_LENGTH+1];
typedef CHAR WD_ISAPNP_ANSI[WD_ISAPNP_ANSI_LENGTH+1+3]; // Add 3 bytes for DWORD alignment.
typedef struct
{
    WD_ISAPNP_COMP_ID cVendor; // Vendor ID.
    DWORD dwSerial; // Serial number of card.
} WD_ISAPNP_CARD_ID;

typedef struct
{
    WD_ISAPNP_CARD_ID cardId;  // VendorID & serial number of cards found.
    DWORD dwLogicalDevices;    // Logical devices on the card.
    BYTE bPnPVersionMajor;     // ISA PnP version Major.
    BYTE bPnPVersionMinor;     // ISA PnP version Minor.
    BYTE bVendorVersionMajor;  // Vendor version Major.
    BYTE bVendorVersionMinor;  // Vendor version Minor.
    WD_ISAPNP_ANSI cIdent;     // Device identifier.
} WD_ISAPNP_CARD, WD_ISAPNP_CARD_V40;

typedef struct
{
    WD_ISAPNP_CARD_ID searchId; // If searchId.cVendor[0]==0 - scan all vendor IDs.
                                // If searchId.dwSerial==0 - scan all serial numbers.
    DWORD dwCards;              // Number of cards found.
    WD_ISAPNP_CARD Card[WD_ISAPNP_CARDS]; // Cards found.
} WD_ISAPNP_SCAN_CARDS, WD_ISAPNP_SCAN_CARDS_V40;

typedef struct
{
    WD_ISAPNP_CARD_ID cardId;   // VendorID and serial number of card.
    DWORD dwLogicalDevice;      // Logical device in card.
    WD_ISAPNP_COMP_ID cLogicalDeviceId; // Logical device ID.
    DWORD dwCompatibleDevices;  // Number of compatible device IDs.
    WD_ISAPNP_COMP_ID CompatibleDevice[WD_ISAPNP_COMPATIBLE_IDS]; // Compatible device IDs.
    WD_ISAPNP_ANSI cIdent;      // Device identifier.
    WD_CARD Card;               // Get card parameters for the ISA PnP card.
} WD_ISAPNP_CARD_INFO, WD_ISAPNP_CARD_INFO_V40;

typedef enum
{
    ISAPNP_ACCESS_OK = 0,
    ISAPNP_ACCESS_ERROR = 1,
    ISAPNP_BAD_ID = 2
} ISAPNP_ACCESS_RESULT;

typedef struct
{
    WD_ISAPNP_CARD_ID cardId; // VendorID and serial number of card.
    DWORD dwOffset;   // Offset in ISA PnP configuration space to read/write from.
    DWORD fIsRead;    // If 1 then read ISA PnP config, 0 write ISA PnP config.
    BYTE  bData;      // Result data of byte read/write.
    DWORD dwResult;   // ISAPNP_ACCESS_RESULT.
} WD_ISAPNP_CONFIG_DUMP, WD_ISAPNP_CONFIG_DUMP_V40;

// PCMCIA Card Services

// Extreme case - two PCMCIA slots and two multi-function (4 functions) cards
enum
{
    WD_PCMCIA_CARDS = 8,
    WD_PCMCIA_VERSION_LEN = 4,
    WD_PCMCIA_MANUFACTURER_LEN = 48,
    WD_PCMCIA_PRODUCTNAME_LEN = 48,
    WD_PCMCIA_MAX_SOCKET = 2,
    WD_PCMCIA_MAX_FUNCTION = 2
};

typedef struct
{
    BYTE uBus;         // bus number (first bus is 0)
    BYTE uSocket;      // socket number (first socket is 0)
    BYTE uFunction;    // function number (first function is 0)
    BYTE uPadding;     // 1 byte padding so structure will be 4 bytes aligned
} WD_PCMCIA_SLOT, WD_PCMCIA_SLOT_V622;

typedef struct
{
    WORD wManufacturerId; // card manufacturer
    WORD wCardId;         // card type and model
} WD_PCMCIA_ID;

typedef struct
{
    WD_PCMCIA_ID searchId;           // device ID to search for
    DWORD dwCards;                   // number of cards found
    WD_PCMCIA_ID cardId[WD_PCMCIA_CARDS]; // device IDs of cards found
    WD_PCMCIA_SLOT cardSlot[WD_PCMCIA_CARDS]; // pcmcia slot info of cards found
    DWORD dwOptions;  // reserved for future use, set to 0
} WD_PCMCIA_SCAN_CARDS, WD_PCMCIA_SCAN_CARDS_V622;

typedef struct
{
    WD_PCMCIA_SLOT pcmciaSlot; // pcmcia slot
    WD_CARD Card;              // get card parameters for pcmcia slot
    CHAR cVersion[WD_PCMCIA_VERSION_LEN];
    CHAR cManufacturer[WD_PCMCIA_MANUFACTURER_LEN];
    CHAR cProductName[WD_PCMCIA_PRODUCTNAME_LEN];
    WORD wManufacturerId;    // card manufacturer
    WORD wCardId;            // card type and model
    WORD wFuncId;            // card function code
    DWORD dwOptions;  // reserved for future use, set to 0
} WD_PCMCIA_CARD_INFO, WD_PCMCIA_CARD_INFO_V622;

typedef struct
{
    WD_PCMCIA_SLOT pcmciaSlot;
    PVOID pBuffer;    // buffer for read/write
    DWORD dwOffset;   // offset in pcmcia attribute space to read/write from
    DWORD dwBytes;    // bytes to read/write from/to buffer
                      //    returns the number of bytes read/written
    DWORD fIsRead;    // if 1 then read pcmcia attrib, 0 write pcmcia attrib
    DWORD dwResult;   // PCMCIA_ACCESS_RESULT
    DWORD dwOptions;  // reserved for future use, set to 0
} WD_PCMCIA_CONFIG_DUMP, WD_PCMCIA_CONFIG_DUMP_V622;

typedef enum {
    WD_PCMCIA_ACC_SPEED_DEFAULT, // use default access speed
    WD_PCMCIA_ACC_SPEED_250NS,
    WD_PCMCIA_ACC_SPEED_200NS,
    WD_PCMCIA_ACC_SPEED_150NS,
    WD_PCMCIA_ACC_SPEED_100NS
} WD_PCMCIA_ACC_SPEED;

typedef enum {
    WD_PCMCIA_ACC_WIDTH_DEFAULT, // use default bus width
    WD_PCMCIA_ACC_WIDTH_8BIT,
    WD_PCMCIA_ACC_WIDTH_16BIT
} WD_PCMCIA_ACC_WIDTH;

typedef enum {
    WD_PCMCIA_VPP_DEFAULT, // use default power level of the PCMCIA Vpp pin
    WD_PCMCIA_VPP_OFF,     // set voltage on the Vpp pin to zero (disable)
    WD_PCMCIA_VPP_ON,      // set voltage on the Vpp pin to 12V (enable)
    WD_PCMCIA_VPP_AS_VCC   // set voltage on the Vpp pin equal to Vcc
} WD_PCMCIA_VPP;

typedef struct
{
    DWORD dwOptions;  // reserved for future use, set to 0
    WD_PCMCIA_SLOT pcmciaSlot; // physical location on PCMCIA bus
    BYTE uAccessSpeed; // one of WD_PCMCIA_ACC_SPEED_XXX values
    BYTE uBusWidth; // one of WD_PCMCIA_ACC_WIDTH_XXX values
    BYTE uVppLevel; // one of WD_PCMCIA_VPP_XXX values
    BYTE uReserved;
    DWORD dwCardBase; // offset in the PCMCIA device's memory where
                      // the memory mapping begins
} WD_PCMCIA_CONTROL, WD_PCMCIA_CONTROL_V623;

enum { SLEEP_BUSY = 0, SLEEP_NON_BUSY = 1 };
typedef struct
{
    DWORD dwMicroSeconds; // Sleep time in Micro Seconds (1/1,000,000 Second)
    DWORD dwOptions;      // can be: SLEEP_NON_BUSY (10000 uSec +)
} WD_SLEEP, WD_SLEEP_V40;

typedef enum
{
    D_OFF       = 0,
    D_ERROR     = 1,
    D_WARN      = 2,
    D_INFO      = 3,
    D_TRACE     = 4
} DEBUG_LEVEL;

typedef enum
{
    S_ALL       = (int)0xffffffff,
    S_IO        = 0x00000008,
    S_MEM       = 0x00000010,
    S_INT       = 0x00000020,
    S_PCI       = 0x00000040,
    S_DMA       = 0x00000080,
    S_MISC      = 0x00000100,
    S_LICENSE   = 0x00000200,
    S_ISAPNP    = 0x00000400,
    S_PCMCIA    = 0x00000800,
    S_PNP       = 0x00001000,
    S_CARD_REG  = 0x00002000,
    S_KER_DRV   = 0x00004000,
    S_USB       = 0x00008000,
    S_KER_PLUG  = 0x00010000,
    S_EVENT     = 0x00020000
} DEBUG_SECTION;

typedef enum
{
    DEBUG_STATUS = 1,
    DEBUG_SET_FILTER = 2,
    DEBUG_SET_BUFFER = 3,
    DEBUG_CLEAR_BUFFER = 4,
    DEBUG_DUMP_SEC_ON = 5,
    DEBUG_DUMP_SEC_OFF = 6,
    KERNEL_DEBUGGER_ON = 7,
    KERNEL_DEBUGGER_OFF = 8
} DEBUG_COMMAND;

typedef struct
{
    DWORD dwCmd;     // DEBUG_COMMAND: DEBUG_STATUS, DEBUG_SET_FILTER, DEBUG_SET_BUFFER, DEBUG_CLEAR_BUFFER
    // used for DEBUG_SET_FILTER
    DWORD dwLevel;   // DEBUG_LEVEL: D_ERROR, D_WARN..., or D_OFF to turn debugging off
    DWORD dwSection; // DEBUG_SECTION: for all sections in driver: S_ALL
                     // for partial sections: S_IO, S_MEM...
    DWORD dwLevelMessageBox; // DEBUG_LEVEL to print in a message box
    // used for DEBUG_SET_BUFFER
    DWORD dwBufferSize; // size of buffer in kernel
} WD_DEBUG, WD_DEBUG_V40;

typedef struct
{
    PCHAR pcBuffer;  // buffer to receive debug messages
    DWORD dwSize;    // size of buffer in bytes
} WD_DEBUG_DUMP, WD_DEBUG_DUMP_V40;

typedef struct
{
    CHAR pcBuffer[256];
    DWORD dwLevel;
    DWORD dwSection;
} WD_DEBUG_ADD, WD_DEBUG_ADD_V503;

typedef struct
{
    DWORD hKernelPlugIn;
    PCHAR pcDriverName;
    PCHAR pcDriverPath; // Should be NULL (exists for backwards compatibility).
                        // The driver will be searched in the operating 
                        // system's drivers/modules directory.
    PVOID pOpenData;
} WD_KERNEL_PLUGIN, WD_KERNEL_PLUGIN_V40;

typedef enum
{
    EVENT_STATUS_OK = 0
} EVENT_STATUS;

#define WD_USB_MAX_DEVICE_NUMBER 30

typedef struct
{
    DWORD dwVendorId;
    DWORD dwProductId;
} WD_USB_ID;

// For backwards compatibility:
#ifndef LINUX
typedef enum {
    USB_DIR_IN     = 1,
    USB_DIR_OUT    = 2,
    USB_DIR_IN_OUT = 3
} USB_DIR;
#endif

typedef struct
{
    DWORD dwNumInterfaces;
    DWORD dwValue;
    DWORD dwAttributes;
    DWORD MaxPower;
} WD_USB_CONFIG_DESC;

typedef struct
{
    DWORD dwNumber;
    DWORD dwAlternateSetting;
    DWORD dwNumEndpoints;
    DWORD dwClass;
    DWORD dwSubClass;
    DWORD dwProtocol;
    DWORD dwIndex;
} WD_USB_INTERFACE_DESC, WD_USB_INTERFACE_DESC_V43;

typedef struct
{
    DWORD dwEndpointAddress;
    DWORD dwAttributes;
    DWORD dwMaxPacketSize;
    DWORD dwInterval;
} WD_USB_ENDPOINT_DESC, WD_USB_ENDPOINT_DESC_V43;

typedef struct
{
    WD_USB_INTERFACE_DESC Interface;
    WD_USB_ENDPOINT_DESC Endpoints[WD_USB_MAX_ENDPOINTS];
} WD_USB_INTERFACE;

typedef struct
{
    DWORD uniqueId;
    DWORD dwConfigurationIndex;
    WD_USB_CONFIG_DESC configuration;
    DWORD dwInterfaceAlternatives;
    WD_USB_INTERFACE Interface[WD_USB_MAX_INTERFACES];
    DWORD dwStatus;  // Configuration status code - see WD_ERROR_CODES enum definition.
                     // WD_USBD_STATUS_SUCCESS for a successful configuration.
} WD_USB_CONFIGURATION, WD_USB_CONFIGURATION_V52;

typedef struct
{
    DWORD   fBusPowered;
    DWORD   dwPorts;              // Number of ports on this hub.
    DWORD   dwCharacteristics;    // Hub Characteristics.
    DWORD   dwPowerOnToPowerGood; // Port power on till power good in 2ms.
    DWORD   dwHubControlCurrent;  // Max current in mA.
} WD_USB_HUB_GENERAL_INFO, WD_USB_HUB_GENERAL_INFO_V43;

#define WD_SINGLE_INTERFACE 0xFFFFFFFF

typedef struct
{
    WD_USB_ID deviceId;
    DWORD dwHubNum; // Unused
    DWORD dwPortNum; // Unused
    DWORD fHub; // Unused
    DWORD fFullSpeed; // Unused
    DWORD dwConfigurationsNum; 
    DWORD deviceAddress; // Unused
    WD_USB_HUB_GENERAL_INFO hubInfo; // Unused
    DWORD deviceClass;  
    DWORD deviceSubClass;      
    DWORD dwInterfaceNum; // For a single device WinDriver sets this
                          // value to WD_SINGLE_INTERFACE
} WD_USB_DEVICE_GENERAL_INFO;

typedef struct
{
    DWORD dwPipes;
    WD_USB_PIPE_INFO Pipe[WD_USB_MAX_PIPE_NUMBER];
} WD_USB_DEVICE_INFO, WD_USB_DEVICE_INFO_V43;

// IOCTL Structures
typedef struct
{
    WD_USB_ID searchId; // If dwVendorId==0 - scan all vendor IDs.
                        // If dwProductId==0 - scan all product IDs.
    DWORD dwDevices;
    DWORD uniqueId[WD_USB_MAX_DEVICE_NUMBER]; // a unique id to identify the device
    WD_USB_DEVICE_GENERAL_INFO deviceGeneralInfo[WD_USB_MAX_DEVICE_NUMBER];
    DWORD dwStatus;     // Scan status code - see WD_ERROR_CODES enum definition.
                        // WD_USBD_STATUS_SUCCESS for a successful scan.
} WD_USB_SCAN_DEVICES, WD_USB_SCAN_DEVICES_V52;

typedef enum 
{
    WD_DEVICE_PCI = 0x1,
    WD_DEVICE_USB = 0x2
} WD_GET_DEVICE_PROPERTY_OPTION;

typedef struct
{
    union 
    {
        HANDLE hDevice;
        DWORD dwUniqueID;
    } h;
    PVOID pBuf;
    DWORD dwBytes;
    DWORD dwProperty;
    DWORD dwOptions;
} WD_GET_DEVICE_PROPERTY;

typedef enum {
    WD_STATUS_SUCCESS = 0x0L,
    WD_STATUS_INVALID_WD_HANDLE = 0xffffffff,
    
    WD_WINDRIVER_STATUS_ERROR = 0x20000000L,

    WD_INVALID_HANDLE = 0x20000001L,
    WD_INVALID_PIPE_NUMBER = 0x20000002L,
    WD_READ_WRITE_CONFLICT = 0x20000003L,    // request to read from an 
        // OUT (write) pipe or request to write to an IN (read) pipe
    WD_ZERO_PACKET_SIZE = 0x20000004L,       // maximum packet size is zero
    WD_INSUFFICIENT_RESOURCES = 0x20000005L,
    WD_UNKNOWN_PIPE_TYPE = 0x20000006L,
    WD_SYSTEM_INTERNAL_ERROR = 0x20000007L,
    WD_DATA_MISMATCH = 0x20000008L,
    WD_NO_LICENSE = 0x20000009L,
    WD_NOT_IMPLEMENTED = 0x2000000aL,
    WD_KERPLUG_FAILURE = 0x2000000bL,
    WD_FAILED_ENABLING_INTERRUPT = 0x2000000cL,
    WD_INTERRUPT_NOT_ENABLED = 0x2000000dL,
    WD_RESOURCE_OVERLAP = 0x2000000eL,
    WD_DEVICE_NOT_FOUND = 0x2000000fL,
    WD_WRONG_UNIQUE_ID = 0x20000010L,
    WD_OPERATION_ALREADY_DONE = 0x20000011L,
    WD_USB_DESCRIPTOR_ERROR = 0x20000012L,
    WD_SET_CONFIGURATION_FAILED = 0x20000013L,
    WD_CANT_OBTAIN_PDO = 0x20000014L,
    WD_TIME_OUT_EXPIRED = 0x20000015L,
    WD_IRP_CANCELED = 0x20000016L,
    WD_FAILED_USER_MAPPING = 0x20000017L,
    WD_FAILED_KERNEL_MAPPING = 0x20000018L,
    WD_NO_RESOURCES_ON_DEVICE = 0x20000019L,
    WD_NO_EVENTS = 0x2000001aL,
    WD_INVALID_PARAMETER = 0x2000001bL,
    WD_INCORRECT_VERSION = 0x2000001cL,
    WD_TRY_AGAIN = 0x2000001dL,
    WD_WINDRIVER_NOT_FOUND = 0x2000001eL,
    WD_INVALID_IOCTL = 0x2000001fL,
    WD_OPERATION_FAILED = 0x20000020L,
    WD_INVALID_32BIT_APP = 0x20000021L,
    WD_TOO_MANY_HANDLES = 0x20000022L,
    WD_NO_DEVICE_OBJECT = 0x20000023L,
    // The following status codes are returned by USBD:
    // USBD status types:
    WD_USBD_STATUS_SUCCESS = 0x00000000L,
    WD_USBD_STATUS_PENDING = 0x40000000L,
    WD_USBD_STATUS_ERROR = (int)0x80000000L,
    WD_USBD_STATUS_HALTED = (int)0xC0000000L,

    // USBD status codes:
    // NOTE: The following status codes are comprised of one of the status 
    // types above and an error code [i.e. 0xXYYYYYYYL - where: X = status type;
    // YYYYYYY = error code].
    // The same error codes may also appear with one of the other status types 
    // as well.

    // HC (Host Controller) status codes.
    // [NOTE: These status codes use the WD_USBD_STATUS_HALTED status type]:
    WD_USBD_STATUS_CRC = (int)0xC0000001L,
    WD_USBD_STATUS_BTSTUFF = (int)0xC0000002L,
    WD_USBD_STATUS_DATA_TOGGLE_MISMATCH = (int)0xC0000003L,
    WD_USBD_STATUS_STALL_PID = (int)0xC0000004L,
    WD_USBD_STATUS_DEV_NOT_RESPONDING = (int)0xC0000005L,
    WD_USBD_STATUS_PID_CHECK_FAILURE = (int)0xC0000006L,
    WD_USBD_STATUS_UNEXPECTED_PID = (int)0xC0000007L,
    WD_USBD_STATUS_DATA_OVERRUN = (int)0xC0000008L,
    WD_USBD_STATUS_DATA_UNDERRUN = (int)0xC0000009L,
    WD_USBD_STATUS_RESERVED1 = (int)0xC000000AL,
    WD_USBD_STATUS_RESERVED2 = (int)0xC000000BL,
    WD_USBD_STATUS_BUFFER_OVERRUN = (int)0xC000000CL,
    WD_USBD_STATUS_BUFFER_UNDERRUN = (int)0xC000000DL,
#if defined(WINCE)
    WD_USBD_STATUS_NOT_ACCESSED_ALT = (int)0xC000000FL,  // HCD maps this to E when encountered 
    WD_USBD_STATUS_NOT_ACCESSED = (int)0xC000000EL,  
#else
    WD_USBD_STATUS_NOT_ACCESSED = (int)0xC000000FL,    
#endif
    WD_USBD_STATUS_FIFO = (int)0xC0000010L,

#if defined(WIN32)
    WD_USBD_STATUS_XACT_ERROR = (int)0xC0000011L,
    WD_USBD_STATUS_BABBLE_DETECTED = (int)0xC0000012L,
    WD_USBD_STATUS_DATA_BUFFER_ERROR = (int)0xC0000013L,
#endif
    
#if defined(WINCE)
    WD_USBD_STATUS_ISOCH = 0xC0000100L,
    WD_USBD_STATUS_CANCELED = 0xC0000101L,
    WD_USBD_STATUS_NOT_COMPLETE = (int)0xC0000103L,
    WD_USBD_STATUS_CLIENT_BUFFER = (int)0xC0000104L,
#else
    WD_USBD_STATUS_CANCELED = (int)0xC0010000L,
#endif

    // Returned by HCD (Host Controller Driver) if a transfer is submitted to 
    // an endpoint that is stalled:
    WD_USBD_STATUS_ENDPOINT_HALTED = (int)0xC0000030L,

    // Software status codes
    // [NOTE: The following status codes have only the error bit set]:
    WD_USBD_STATUS_NO_MEMORY = (int)0x80000100L,
    WD_USBD_STATUS_INVALID_URB_FUNCTION = (int)0x80000200L,
    WD_USBD_STATUS_INVALID_PARAMETER = (int)0x80000300L,

    // Returned if client driver attempts to close an endpoint/interface
    // or configuration with outstanding transfers:
    WD_USBD_STATUS_ERROR_BUSY = (int)0x80000400L,

    // Returned by USBD if it cannot complete a URB request. Typically this
    // will be returned in the URB status field when the Irp is completed
    // with a more specific error code. [The Irp status codes are indicated 
    // in WinDriver's Debug Monitor tool (wddebug/wddebug_gui):
    WD_USBD_STATUS_REQUEST_FAILED = (int)0x80000500L,

    WD_USBD_STATUS_INVALID_PIPE_HANDLE = (int)0x80000600L,

    // Returned when there is not enough bandwidth available
    // to open a requested endpoint:
    WD_USBD_STATUS_NO_BANDWIDTH = (int)0x80000700L,

    // Generic HC (Host Controller) error:
    WD_USBD_STATUS_INTERNAL_HC_ERROR = (int)0x80000800L,

    // Returned when a short packet terminates the transfer
    // i.e. USBD_SHORT_TRANSFER_OK bit not set:
    WD_USBD_STATUS_ERROR_SHORT_TRANSFER = (int)0x80000900L,

    // Returned if the requested start frame is not within
    // USBD_ISO_START_FRAME_RANGE of the current USB frame,
    // NOTE: The stall bit is set:
    WD_USBD_STATUS_BAD_START_FRAME = (int)0xC0000A00L,

    // Returned by HCD (Host Controller Driver) if all packets in an 
    // isochronous transfer complete with an error:
    WD_USBD_STATUS_ISOCH_REQUEST_FAILED = (int)0xC0000B00L,

    // Returned by USBD if the frame length control for a given
    // HC (Host Controller) is already taken by another driver:
    WD_USBD_STATUS_FRAME_CONTROL_OWNED = (int)0xC0000C00L,

    // Returned by USBD if the caller does not own frame length control and
    // attempts to release or modify the HC frame length:
    WD_USBD_STATUS_FRAME_CONTROL_NOT_OWNED = (int)0xC0000D00L

#if defined(WIN32)
    ,
    // Additional USB 2.0 software error codes added for USB 2.0:
    WD_USBD_STATUS_NOT_SUPPORTED = (int)0xC0000E00L, // Returned for APIS not supported/implemented
    WD_USBD_STATUS_INAVLID_CONFIGURATION_DESCRIPTOR = (int)0xC0000F00L,
    WD_USBD_STATUS_INSUFFICIENT_RESOURCES = (int)0xC0001000L, 
    WD_USBD_STATUS_SET_CONFIG_FAILED = (int)0xC0002000L,
    WD_USBD_STATUS_BUFFER_TOO_SMALL = (int)0xC0003000L,
    WD_USBD_STATUS_INTERFACE_NOT_FOUND = (int)0xC0004000L,
    WD_USBD_STATUS_INAVLID_PIPE_FLAGS = (int)0xC0005000L,
    WD_USBD_STATUS_TIMEOUT = (int)0xC0006000L,
    WD_USBD_STATUS_DEVICE_GONE = (int)0xC0007000L,
    WD_USBD_STATUS_STATUS_NOT_MAPPED = (int)0xC0008000L,

    // Extended isochronous error codes returned by USBD. 
    // These errors appear in the packet status field of an isochronous
    // transfer.

    // For some reason the controller did not access the TD associated with 
    // this packet:
    WD_USBD_STATUS_ISO_NOT_ACCESSED_BY_HW = (int)0xC0020000L,   
    // Controller reported an error in the TD. 
    // Since TD errors are controller specific they are reported 
    // generically with this error code:
    WD_USBD_STATUS_ISO_TD_ERROR = (int)0xC0030000L,   
    // The packet was submitted in time by the client but 
    // failed to reach the miniport in time:
    WD_USBD_STATUS_ISO_NA_LATE_USBPORT = (int)0xC0040000L, 
    // The packet was not sent because the client submitted it too late 
    // to transmit:
    WD_USBD_STATUS_ISO_NOT_ACCESSED_LATE = (int)0xC0050000L
#endif
} WD_ERROR_CODES;

typedef struct
{
    DWORD hDevice;      // Handle of USB device to read from or write to.
    DWORD dwPipe;       // Pipe number on device.
    DWORD fRead;        // TRUE for read (IN) transfers; FALSE for write (OUT) transfers.
    DWORD dwOptions;    // USB_TRANSFER options:
                        //    USB_TRANSFER_HALT halts the pervious transfer.
                        //    USB_SHORT_TRANSFER - if set, WD_UsbTransfer() will return
                        //    once a data transfer occurs (within the dwTimeout period),
                        //    even if the device sent less data than requested in dwBytes.
    PVOID pBuffer;      // Pointer to buffer to read/write.
    DWORD dwBytes;      // Amount of bytes to transfer.
    DWORD dwTimeout;    // Timeout for the transfer in milliseconds. Set to 0 for infinite wait.
    DWORD dwBytesTransfered;    // Returns the number of bytes actually read/written
    BYTE  SetupPacket[8];       // Setup packet for control pipe transfer.
    DWORD fOK;          // TRUE if transfer succeeded.
    DWORD dwStatus;     // Transfer status code - see WD_ERROR_CODES enum definition.
                        // WD_USBD_STATUS_SUCCESS for a successful transfer.
} WD_USB_TRANSFER, WD_USB_TRANSFER_V52;

typedef struct {
    DWORD uniqueId;              // The device unique ID.
    DWORD dwConfigurationIndex;  // The index of the configuration to register.
    DWORD dwInterfaceNum;        // Interface to register.
    DWORD dwInterfaceAlternate;
    DWORD hDevice;               // Handle of device.
    WD_USB_DEVICE_INFO Device;   // Description of the device.
    DWORD dwOptions;             // Should be zero.
    CHAR  cName[32];             // Name of card.
    CHAR  cDescription[100];     // Description.
    DWORD dwStatus;              // Register status code - see WD_ERROR_CODES enum definition.
                                 // WD_USBD_STATUS_SUCCESS for a successful registration.
} WD_USB_DEVICE_REGISTER, WD_USB_DEVICE_REGISTER_V52;

typedef struct
{
    DWORD hDevice;
    DWORD dwPipe;
    DWORD dwStatus;   // Reset status code - see WD_ERROR_CODES enum definition.
                      // WD_USBD_STATUS_SUCCESS for a successful reset.
} WD_USB_RESET_PIPE, WD_USB_RESET_PIPE_V52;

typedef enum
{
    WD_INSERT                  = 0x1,
    WD_REMOVE                  = 0x2,
    WD_CPCI_REENUM             = 0x8,    
    WD_POWER_CHANGED_D0        = 0x10,  //power states for the power management.
    WD_POWER_CHANGED_D1        = 0x20,
    WD_POWER_CHANGED_D2        = 0x40,
    WD_POWER_CHANGED_D3        = 0x80,
    WD_POWER_SYSTEM_WORKING    = 0x100,
    WD_POWER_SYSTEM_SLEEPING1  = 0x200,
    WD_POWER_SYSTEM_SLEEPING2  = 0x400,
    WD_POWER_SYSTEM_SLEEPING3  = 0x800,
    WD_POWER_SYSTEM_HIBERNATE  = 0x1000,
    WD_POWER_SYSTEM_SHUTDOWN   = 0x2000
} WD_EVENT_ACTION;

#define WD_CPCI_INSERT (WD_INSERT | WD_CPCI_REENUM)
#define WD_CPCI_REMOVE (WD_REMOVE | WD_CPCI_REENUM)

typedef enum
{
    WD_ACKNOWLEDGE              = 0x1,
    WD_ACCEPT_CONTROL           = 0x2  // used in WD_EVENT_SEND (acknowledge)
} WD_EVENT_OPTION;

#define WD_ACTIONS_POWER (WD_POWER_CHANGED_D0 | WD_POWER_CHANGED_D1 | WD_POWER_CHANGED_D2 | \
    WD_POWER_CHANGED_D3 | WD_POWER_SYSTEM_WORKING | WD_POWER_SYSTEM_SLEEPING1 | \
    WD_POWER_SYSTEM_SLEEPING3 | WD_POWER_SYSTEM_HIBERNATE | WD_POWER_SYSTEM_SHUTDOWN)
#define WD_ACTIONS_ALL (WD_ACTIONS_POWER | WD_INSERT | WD_REMOVE)

typedef struct
{
    DWORD handle;
    DWORD dwAction; // WD_EVENT_ACTION
    DWORD dwStatus; // EVENT_STATUS
    DWORD dwEventId;
    WD_BUS_TYPE dwCardType; //WD_BUS_PCI, WD_BUS_USB, WD_BUS_PCMCIA
    DWORD hKernelPlugIn;
    DWORD dwOptions; // WD_EVENT_OPTION
    union
    {
        struct
        {
            WD_PCI_ID cardId;
            WD_PCI_SLOT pciSlot;
        } Pci;
        struct
        {
            WD_USB_ID deviceId;
            DWORD dwUniqueID;
        } Usb;
        struct
        {
            WD_PCMCIA_ID deviceId;
            WD_PCMCIA_SLOT slot;
        } Pcmcia;
    } u;
    DWORD dwEventVer;
    DWORD dwNumMatchTables;
    WDU_MATCH_TABLE matchTables[1];
} WD_EVENT, WD_EVENT_V622; 

typedef struct
{
    DWORD applications_num;
    DWORD devices_num;
} WD_USAGE;

enum 
{ 
    WD_USB_HARD_RESET = 1,
    WD_USB_CYCLE_PORT = 2
};

typedef struct
{
    DWORD hDevice;
    DWORD dwOptions; // USB_RESET options:
                     //    WD_USB_HARD_RESET - will reset the device
                     //    even if it is not disabled.
                     //    After using this option it is advised to
                     //    un-register the device (WD_UsbDeviceUnregister())
                     //    and register it again - to make sure that the
                     //    device has all its resources.
    DWORD dwStatus;  // Reset status code - see WD_ERROR_CODES enum definition.
                     // WD_USBD_STATUS_SUCCESS for a successful reset.
} WD_USB_RESET_DEVICE, WD_USB_RESET_DEVICE_V52;

typedef struct
{
    WORD wVendorId;
    WORD wDeviceId;
    WORD wSubVendorId;
    WORD wSubDeviceId;
    DWORD dwOptions; // WD_MATCH_EXCLUDE
} WD_HS_MATCH_TABLE;

typedef struct {
    DWORD handle;
    DWORD dwOptions; // WD_CPCI_PNP_SUPPORT in case we want to use OS PnP support
    DWORD dwNumMatchTables;
    WD_HS_MATCH_TABLE matchTables[1];
} WD_HS_WATCH, WD_HS_WATCH_V622;

#ifndef BZERO
    #define BZERO(buf) memset(&(buf), 0, sizeof(buf))
#endif

#ifndef INVALID_HANDLE_VALUE
    #define INVALID_HANDLE_VALUE ((HANDLE)(-1))
#endif

#ifndef CTL_CODE
    #define CTL_CODE(DeviceType, Function, Method, Access) ( \
        ((DeviceType)<<16) | ((Access)<<14) | ((Function)<<2) | (Method) \
    )

    #define METHOD_BUFFERED   0
    #define METHOD_IN_DIRECT  1
    #define METHOD_OUT_DIRECT 2
    #define METHOD_NEITHER    3
    #define FILE_ANY_ACCESS   0
    #define FILE_READ_ACCESS  1    // file & pipe
    #define FILE_WRITE_ACCESS 2    // file & pipe
#endif

#if defined(LINUX) && defined(KERNEL_64BIT)
    #define WD_TYPE 0
    #define WD_CTL_CODE(wFuncNum) _IOC(_IOC_READ|_IOC_WRITE, WD_TYPE, wFuncNum, 0)
    #define WD_CTL_DECODE_FUNC(IoControlCode) _IOC_NR(IoControlCode)
    #define WD_CTL_DECODE_TYPE(IoControlCode) _IOC_TYPE(IoControlCode)
#elif defined(UNIX)
    #define WD_TYPE 0
    #define WD_CTL_CODE(wFuncNum) (wFuncNum)
    #define WD_CTL_DECODE_FUNC(IoControlCode) (IoControlCode)
    #define WD_CTL_DECODE_TYPE(IoControlCode) (WD_TYPE)
#else
    // Device type
    #define WD_TYPE 38200
    #if defined(KERNEL_64BIT)
        #define FUNC_MASK 0x400 
    #else
        #define FUNC_MASK 0x0 
    #endif
    #define WD_CTL_CODE(wFuncNum) CTL_CODE(WD_TYPE, (wFuncNum | FUNC_MASK), METHOD_NEITHER, FILE_ANY_ACCESS)
    #define WD_CTL_DECODE_FUNC(IoControlCode) ((IoControlCode >> 2) & 0xfff)
    #define WD_CTL_DECODE_TYPE(IoControlCode) DEVICE_TYPE_FROM_CTL_CODE(IoControlCode)
#endif

#if defined(LINUX)
    #define WD_CTL_IS_64BIT_AWARE(IoControlCode) (_IOC_DIR(IoControlCode) & (_IOC_READ|_IOC_WRITE))
#elif defined(UNIX)
    #define WD_CTL_IS_64BIT_AWARE(IoControlCode) TRUE
#else
    #define WD_CTL_IS_64BIT_AWARE(IoControlCode) (WD_CTL_DECODE_FUNC(IoControlCode) & FUNC_MASK)
#endif

// WinDriver function IOCTL calls.  For details on the WinDriver functions,
// see the WinDriver manual or included help files.

#define IOCTL_WD_DMA_LOCK                 WD_CTL_CODE(0x9be)
#define IOCTL_WD_DMA_UNLOCK               WD_CTL_CODE(0x902)
#define IOCTL_WD_TRANSFER                 WD_CTL_CODE(0x98c)
#define IOCTL_WD_MULTI_TRANSFER           WD_CTL_CODE(0x98d)
#define IOCTL_WD_PCI_SCAN_CARDS           WD_CTL_CODE(0x9a3)
#define IOCTL_WD_PCI_GET_CARD_INFO        WD_CTL_CODE(0x90f)
#define IOCTL_WD_VERSION                  WD_CTL_CODE(0x910)
#define IOCTL_WD_PCI_CONFIG_DUMP          WD_CTL_CODE(0x91a)
#define IOCTL_WD_KERNEL_PLUGIN_OPEN       WD_CTL_CODE(0x91b)
#define IOCTL_WD_KERNEL_PLUGIN_CLOSE      WD_CTL_CODE(0x91c)
#define IOCTL_WD_KERNEL_PLUGIN_CALL       WD_CTL_CODE(0x91d)
#define IOCTL_WD_INT_ENABLE               WD_CTL_CODE(0x9b6)
#define IOCTL_WD_INT_DISABLE              WD_CTL_CODE(0x9bb)
#define IOCTL_WD_INT_COUNT                WD_CTL_CODE(0x9ba)
#define IOCTL_WD_ISAPNP_SCAN_CARDS        WD_CTL_CODE(0x924)
#define IOCTL_WD_ISAPNP_CONFIG_DUMP       WD_CTL_CODE(0x926)
#define IOCTL_WD_SLEEP                    WD_CTL_CODE(0x927)
#define IOCTL_WD_DEBUG                    WD_CTL_CODE(0x928)
#define IOCTL_WD_DEBUG_DUMP               WD_CTL_CODE(0x929)
#define IOCTL_WD_CARD_UNREGISTER          WD_CTL_CODE(0x92b)
#define IOCTL_WD_ISAPNP_GET_CARD_INFO     WD_CTL_CODE(0x92d)
#define IOCTL_WD_PCMCIA_SCAN_CARDS        WD_CTL_CODE(0x996)
#define IOCTL_WD_PCMCIA_GET_CARD_INFO     WD_CTL_CODE(0x997)
#define IOCTL_WD_PCMCIA_CONFIG_DUMP       WD_CTL_CODE(0x998)
#define IOCTL_WD_CARD_REGISTER            WD_CTL_CODE(0x9bf)
#define IOCTL_WD_INT_WAIT                 WD_CTL_CODE(0x9b9)
#define IOCTL_WD_LICENSE                  WD_CTL_CODE(0x952)
#define IOCTL_WD_USB_RESET_PIPE           WD_CTL_CODE(0x971)
#define IOCTL_WD_USB_RESET_DEVICE         WD_CTL_CODE(0x93f)
#define IOCTL_WD_USB_SCAN_DEVICES         WD_CTL_CODE(0x969)
#define IOCTL_WD_USB_TRANSFER             WD_CTL_CODE(0x967)
#define IOCTL_WD_USB_DEVICE_REGISTER      WD_CTL_CODE(0x968)
#define IOCTL_WD_USB_DEVICE_UNREGISTER    WD_CTL_CODE(0x970)
#define IOCTL_WD_USB_GET_CONFIGURATION    WD_CTL_CODE(0x974)
#define IOCTL_WD_EVENT_REGISTER           WD_CTL_CODE(0x9c0)
#define IOCTL_WD_EVENT_UNREGISTER         WD_CTL_CODE(0x987)
#define IOCTL_WD_EVENT_PULL               WD_CTL_CODE(0x988)
#define IOCTL_WD_EVENT_SEND               WD_CTL_CODE(0x989)
#define IOCTL_WD_DEBUG_ADD                WD_CTL_CODE(0x964)
#define IOCTL_WD_USB_RESET_DEVICEEX       WD_CTL_CODE(0x973)
#define IOCTL_WD_USAGE                    WD_CTL_CODE(0x976)
#define IOCTL_WDU_GET_DEVICE_DATA         WD_CTL_CODE(0x9a7)
#define IOCTL_WDU_SET_INTERFACE           WD_CTL_CODE(0x981)
#define IOCTL_WDU_RESET_PIPE              WD_CTL_CODE(0x982)
#define IOCTL_WDU_TRANSFER                WD_CTL_CODE(0x983)
#define IOCTL_WDU_HALT_TRANSFER           WD_CTL_CODE(0x985)
#define IOCTL_WD_WATCH_PCI_START          WD_CTL_CODE(0x9c1)
#define IOCTL_WD_WATCH_PCI_STOP           WD_CTL_CODE(0x99a)
#define IOCTL_WDU_WAKEUP                  WD_CTL_CODE(0x98a)
#define IOCTL_WDU_RESET_DEVICE            WD_CTL_CODE(0x98b)
#define IOCTL_WD_GET_DEVICE_PROPERTY      WD_CTL_CODE(0x990)
#define IOCTL_WD_CARD_CLEANUP_SETUP       WD_CTL_CODE(0x995)
#define IOCTL_WD_PCMCIA_CONTROL           WD_CTL_CODE(0x99b)
#define IOCTL_WD_DMA_SYNC_CPU             WD_CTL_CODE(0x99f)
#define IOCTL_WD_DMA_SYNC_IO              WD_CTL_CODE(0x9a0)
#define IOCTL_WDU_STREAM_OPEN             WD_CTL_CODE(0x9a8)
#define IOCTL_WDU_STREAM_CLOSE            WD_CTL_CODE(0x9a9)
#define IOCTL_WDU_STREAM_START            WD_CTL_CODE(0x9af)
#define IOCTL_WDU_STREAM_STOP             WD_CTL_CODE(0x9b0)
#define IOCTL_WDU_STREAM_FLUSH            WD_CTL_CODE(0x9aa)
#define IOCTL_WDU_STREAM_GET_STATUS       WD_CTL_CODE(0x9b5)
#define IOCTL_WDU_SELECTIVE_SUSPEND       WD_CTL_CODE(0x9ae)
#define IOCTL_WD_INTERRUPT_DONE_CE        WD_CTL_CODE(0x9bc)

#if defined(UNIX)
    typedef struct
    {
        DWORD dwHeader;
        PVOID pData;
        DWORD dwSize;
    } WD_IOCTL_HEADER;

#define WD_IOCTL_HEADER_CODE 0xa410b413UL
#endif

#if defined(WIN32) || defined(WINCE)
    #define WD_DRIVER_NAME_PREFIX "\\\\.\\"
#elif defined(LINUX) || defined(SOLARIS)
    #define WD_DRIVER_NAME_PREFIX "/dev/"
#elif defined(VXWORKS)
    #define WD_DRIVER_NAME_PREFIX "/"
#endif

#define WD_DEFAULT_DRIVER_NAME WD_DRIVER_NAME_PREFIX "windrvr6"

#define WD_MAX_DRIVER_NAME_LENGTH 128

#if defined(WD_DRIVER_NAME_CHANGE)
    const char * DLLCALLCONV WD_DriverName(const char *sName);    
    /* Get driver name */
    #define WD_DRIVER_NAME WD_DriverName(NULL)
#else
    #define WD_DRIVER_NAME WD_DEFAULT_DRIVER_NAME
#endif

#if defined(__KERNEL__)
    HANDLE __cdecl WD_Open(void);
    void __cdecl WD_Close(HANDLE hWD);
    DWORD __cdecl KP_DeviceIoControl(DWORD dwFuncNum, HANDLE h, PVOID pParam,
        DWORD dwSize);
    #define WD_FUNCTION(wFuncNum, h, pParam, dwSize, fWait) \
        KP_DeviceIoControl(\
            (DWORD) wFuncNum, h,\
            (PVOID) pParam, (DWORD) dwSize\
            )
#else
    typedef enum 
    {
        OS_NOT_SET = 0,
        OS_CAN_NOT_DETECT,
        OS_WIN_NT_4, /* Windows NT 4.0 */
        OS_WIN_NT_5, /* Windows 2000, XP, Server 2003 */
        OS_WIN_NT_6, /* Windows Vista */
        OS_LINUX,
        OS_SOLARIS,
        OS_VXWORKS,
        OS_OS2,
        OS_WINCE_4,  /* Windows CE 4 and 5 */
        OS_WINCE_6   /* Windows CE 6 */
    } OS_TYPE;

    OS_TYPE DLLCALLCONV get_os_type(void);

    #if defined(UNIX)
        static inline ULONG WD_FUNCTION_LOCAL(int wFuncNum, HANDLE h,
                PVOID pParam, DWORD dwSize, BOOL fWait)
        {
            WD_IOCTL_HEADER ioctl_hdr;
            ioctl_hdr.dwHeader = WD_IOCTL_HEADER_CODE;
            ioctl_hdr.pData = pParam;
            ioctl_hdr.dwSize = dwSize;
            (void)fWait;
            #if defined(VXWORKS)
                return (ULONG) ioctl((int)(h), wFuncNum, (int)&ioctl_hdr);
            #elif defined(LINUX) || defined(SOLARIS)
                return (ULONG) ioctl((int)(DWORD)(h), wFuncNum, &ioctl_hdr);
            #endif
        }

        #if defined(VXWORKS)
            #define WD_OpenLocal()\
                ((HANDLE)open(WD_DRIVER_NAME, O_RDWR, 0644))
        #else
            #define WD_OpenLocal()\
                ((HANDLE)(DWORD)open(WD_DRIVER_NAME, O_RDWR))
            #define WD_OpenStreamLocal(read,sync) \
                ((HANDLE)(DWORD)open(WD_DRIVER_NAME, \
                    ((read) ? O_RDONLY : O_WRONLY) |\
                    ((sync) ? O_SYNC : O_NONBLOCK)))
        #endif

        #define WD_CloseLocal(h)\
            close((int)(DWORD)(h))

        #define WD_UStreamRead(hFile, pBuffer, dwNumberOfBytesToRead, \
            dwNumberOfBytesRead)\
            WD_NOT_IMPLEMENTED

        #define WD_UStreamWrite(hFile, pBuffer, dwNumberOfBytesToWrite, \
            dwNumberOfBytesWritten)\
            WD_NOT_IMPLEMENTED

    #elif defined(WINCE) && defined(_WIN32_WCE_EMULATION)
        HANDLE WINAPI WCE_EMU_WD_Open();
        void WINAPI WCE_EMU_WD_Close(HANDLE hWD);
        BOOL WINAPI WCE_EMU_WD_FUNCTION(DWORD wFuncNum, HANDLE h, PVOID
            pParam, DWORD dwSize);
        #define WD_OpenLocal() WCE_EMU_WD_Open()
        #define WD_CloseLocal(h) WCE_EMU_WD_Close(h)
        #define WD_FUNCTION_LOCAL(wFuncNum, h, pParam, dwSize, fWait) \
            WCE_EMU_WD_FUNCTION(wFuncNum, h, pParam, dwSize)
    #elif defined(WIN32) || defined(WINCE)
        #define WD_CloseLocal(h)\
            CloseHandle(h)

        #define WD_UStreamRead(hFile, pBuffer, dwNumberOfBytesToRead, \
            dwNumberOfBytesRead)\
            ReadFile(hFile, pBuffer, dwNumberOfBytesToRead, \
                dwNumberOfBytesRead, NULL) ? WD_STATUS_SUCCESS : \
                WD_OPERATION_FAILED

        #define WD_UStreamWrite(hFile, pBuffer, dwNumberOfBytesToWrite, \
            dwNumberOfBytesWritten)\
            WriteFile(hFile, pBuffer, dwNumberOfBytesToWrite, \
                dwNumberOfBytesWritten, NULL) ? WD_STATUS_SUCCESS : \
                WD_OPERATION_FAILED

        #if defined(WINCE)
            void DLLCALLCONV OsSetMaxProcPermissions(void);
            #if defined(WD_CE_ENHANCED_INTR)
                #define InterruptDisable CE_InterruptDisable
                #include <ceddk.h>
                #include <pkfuncs.h>
                #undef InterruptDisable 
            #endif

            #define WD_OpenLocal()\
                (OsSetMaxProcPermissions(), CreateFile(\
                    TEXT("WDR1:"),\
                    GENERIC_READ,\
                    FILE_SHARE_READ | FILE_SHARE_WRITE,\
                    NULL, OPEN_EXISTING, 0, NULL))

            #define WD_OpenStreamLocal(read,sync)\
                (OsSetMaxProcPermissions(), CreateFile(\
                    TEXT("WDR1:"),\
                    (read) ? GENERIC_READ : GENERIC_WRITE,\
                    FILE_SHARE_READ | FILE_SHARE_WRITE,\
                    NULL, OPEN_EXISTING, (sync) ? 0 : FILE_FLAG_OVERLAPPED,\
                    NULL))
            
            #define WD_CE_PRIORITY(prio)\
                (get_os_type() == OS_WINCE_6 ? WD_CE6_PRIORITY(prio) : prio)
            
            static DWORD WD_FUNCTION_LOCAL(int wFuncNum, HANDLE h,
                PVOID pParam, DWORD dwSize, BOOL fWait)
            {
                DWORD dwTmp;
                DWORD rc = WD_WINDRIVER_STATUS_ERROR;
                ((DWORD) DeviceIoControl(h, (DWORD)wFuncNum, pParam, dwSize,
                    &rc, sizeof(DWORD), &dwTmp, NULL));
                return rc;
            }
        #elif defined(WIN32)
            #define WD_OpenLocal()\
                CreateFileA(\
                    WD_DRIVER_NAME,\
                    GENERIC_READ,\
                    FILE_SHARE_READ | FILE_SHARE_WRITE,\
                    NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL)

            #define WD_OpenStreamLocal(read,sync) \
                CreateFileA(\
                    WD_DRIVER_NAME,\
                    (read) ? GENERIC_READ : GENERIC_WRITE,\
                    FILE_SHARE_READ | FILE_SHARE_WRITE,\
                    NULL, OPEN_EXISTING, (sync) ? 0 : FILE_FLAG_OVERLAPPED, NULL)

            static DWORD WD_FUNCTION_LOCAL(int wFuncNum, HANDLE h,
                PVOID pParam, DWORD dwSize, BOOL fWait)
            {
                DWORD dwTmp;
                HANDLE hWD = fWait ? WD_OpenLocal() : h;
                DWORD rc = WD_WINDRIVER_STATUS_ERROR;
                if (hWD==INVALID_HANDLE_VALUE)
                    return (DWORD)WD_STATUS_INVALID_WD_HANDLE;
                DeviceIoControl(hWD, (DWORD)wFuncNum, pParam, dwSize, &rc,
                    sizeof(DWORD), &dwTmp, NULL);
                if (fWait)
                    WD_CloseLocal(hWD);
                return rc;
            }
        #endif
    #endif
    #define WD_FUNCTION WD_FUNCTION_LOCAL
    #define WD_Close WD_CloseLocal
    #define WD_Open WD_OpenLocal
    #define WD_StreamOpen WD_OpenStreamLocal
    #define WD_StreamClose WD_CloseLocal
#endif

#define SIZE_OF_WD_DMA(pDma) \
    ((DWORD)(sizeof(WD_DMA) + ((pDma)->dwPages <= WD_DMA_PAGES ? 0 : ((pDma)->dwPages - WD_DMA_PAGES) * sizeof(WD_DMA_PAGE))))
#define SIZE_OF_WD_EVENT(pEvent) \
    ((DWORD)(sizeof(WD_EVENT) + sizeof(WDU_MATCH_TABLE) * ((pEvent)->dwNumMatchTables - 1)))
#define SIZE_OF_WD_HS_WATCH(pHsWatch) \
    ((DWORD)(sizeof(WD_HS_WATCH) + sizeof(WD_HS_MATCH_TABLE) * ((pHsWatch)->dwNumMatchTables - 1)))

#define WD_Debug(h,pDebug)\
    WD_FUNCTION(IOCTL_WD_DEBUG, h, pDebug, sizeof (WD_DEBUG), FALSE)
#define WD_DebugDump(h,pDebugDump)\
    WD_FUNCTION(IOCTL_WD_DEBUG_DUMP, h, pDebugDump, sizeof (WD_DEBUG_DUMP), FALSE)
#define WD_DebugAdd(h, pDebugAdd)\
    WD_FUNCTION(IOCTL_WD_DEBUG_ADD, h, pDebugAdd, sizeof(WD_DEBUG_ADD), FALSE)
#define WD_Transfer(h,pTransfer)\
    WD_FUNCTION(IOCTL_WD_TRANSFER, h, pTransfer, sizeof (WD_TRANSFER), FALSE)
#define WD_MultiTransfer(h,pTransferArray,dwNumTransfers)\
    WD_FUNCTION(IOCTL_WD_MULTI_TRANSFER, h, pTransferArray, sizeof (WD_TRANSFER) * (dwNumTransfers), FALSE)
#define WD_DMALock(h,pDma)\
    WD_FUNCTION(IOCTL_WD_DMA_LOCK, h, pDma, SIZE_OF_WD_DMA(pDma), FALSE)
#define WD_DMAUnlock(h,pDma)\
    WD_FUNCTION(IOCTL_WD_DMA_UNLOCK, h, pDma, SIZE_OF_WD_DMA(pDma), FALSE) 
#define WD_DMASyncCpu(h,pDma)\
    WD_FUNCTION(IOCTL_WD_DMA_SYNC_CPU, h, pDma, SIZE_OF_WD_DMA(pDma), FALSE)
#define WD_DMASyncIo(h,pDma)\
    WD_FUNCTION(IOCTL_WD_DMA_SYNC_IO, h, pDma, SIZE_OF_WD_DMA(pDma), FALSE)
#define WD_CardRegister(h,pCard)\
    WD_FUNCTION(IOCTL_WD_CARD_REGISTER, h, pCard, sizeof(WD_CARD_REGISTER),\
        FALSE)
#define WD_CardUnregister(h,pCard)\
    WD_FUNCTION(IOCTL_WD_CARD_UNREGISTER, h, pCard, sizeof(WD_CARD_REGISTER),\
        FALSE)
#define WD_CardCleanupSetup(h,pCardCleanup)\
    WD_FUNCTION(IOCTL_WD_CARD_CLEANUP_SETUP, h, pCardCleanup, \
        sizeof(WD_CARD_CLEANUP), FALSE)
#define WD_PciScanCards(h,pPciScan)\
    WD_FUNCTION(IOCTL_WD_PCI_SCAN_CARDS, h, pPciScan,\
        sizeof(WD_PCI_SCAN_CARDS), FALSE)
#define WD_PciGetCardInfo(h,pPciCard)\
    WD_FUNCTION(IOCTL_WD_PCI_GET_CARD_INFO, h, pPciCard, \
        sizeof(WD_PCI_CARD_INFO), FALSE)
#define WD_PciConfigDump(h,pPciConfigDump)\
    WD_FUNCTION(IOCTL_WD_PCI_CONFIG_DUMP, h, pPciConfigDump, \
        sizeof(WD_PCI_CONFIG_DUMP), FALSE)
#define WD_Version(h,pVerInfo)\
    WD_FUNCTION(IOCTL_WD_VERSION, h, pVerInfo, sizeof (WD_VERSION), FALSE)
#define WD_License(h,pLicense)\
    WD_FUNCTION(IOCTL_WD_LICENSE, h, pLicense, sizeof (WD_LICENSE), FALSE)
#define WD_KernelPlugInOpen(h,pKernelPlugIn)\
    WD_FUNCTION(IOCTL_WD_KERNEL_PLUGIN_OPEN, h, pKernelPlugIn, \
        sizeof(WD_KERNEL_PLUGIN), FALSE)
#define WD_KernelPlugInClose(h,pKernelPlugIn)\
    WD_FUNCTION(IOCTL_WD_KERNEL_PLUGIN_CLOSE, h, pKernelPlugIn, \
        sizeof(WD_KERNEL_PLUGIN), FALSE)
#define WD_KernelPlugInCall(h,pKernelPlugInCall)\
    WD_FUNCTION(IOCTL_WD_KERNEL_PLUGIN_CALL, h, pKernelPlugInCall, \
        sizeof(WD_KERNEL_PLUGIN_CALL), FALSE)
#define WD_IntEnable(h,pInterrupt)\
    WD_FUNCTION(IOCTL_WD_INT_ENABLE, h, pInterrupt, sizeof (WD_INTERRUPT), \
        FALSE)
#define WD_IntDisable(h,pInterrupt)\
    WD_FUNCTION(IOCTL_WD_INT_DISABLE, h, pInterrupt, sizeof (WD_INTERRUPT), \
        FALSE)
#define WD_IntCount(h,pInterrupt)\
    WD_FUNCTION(IOCTL_WD_INT_COUNT, h, pInterrupt, sizeof (WD_INTERRUPT), FALSE)
#define WD_IntWait(h,pInterrupt)\
    WD_FUNCTION(IOCTL_WD_INT_WAIT, h, pInterrupt, sizeof (WD_INTERRUPT), TRUE)
#define WD_IsapnpScanCards(h,pIsapnpScan)\
    WD_FUNCTION(IOCTL_WD_ISAPNP_SCAN_CARDS, h, pIsapnpScan, \
        sizeof(WD_ISAPNP_SCAN_CARDS), FALSE)
#define WD_IsapnpGetCardInfo(h,pIsapnpCard)\
    WD_FUNCTION(IOCTL_WD_ISAPNP_GET_CARD_INFO, h, pIsapnpCard, \
        sizeof(WD_ISAPNP_CARD_INFO), FALSE)
#define WD_IsapnpConfigDump(h,pIsapnpConfigDump)\
    WD_FUNCTION(IOCTL_WD_ISAPNP_CONFIG_DUMP, h, pIsapnpConfigDump, \
        sizeof(WD_ISAPNP_CONFIG_DUMP), FALSE)
#define WD_PcmciaScanCards(h,pPcmciaScan)\
    WD_FUNCTION(IOCTL_WD_PCMCIA_SCAN_CARDS, h, pPcmciaScan, \
        sizeof(WD_PCMCIA_SCAN_CARDS), FALSE)
#define WD_PcmciaGetCardInfo(h,pPcmciaCard)\
    WD_FUNCTION(IOCTL_WD_PCMCIA_GET_CARD_INFO, h, pPcmciaCard, \
        sizeof(WD_PCMCIA_CARD_INFO), FALSE)
#define WD_PcmciaConfigDump(h,pPcmciaConfigDump)\
    WD_FUNCTION(IOCTL_WD_PCMCIA_CONFIG_DUMP, h, pPcmciaConfigDump, \
        sizeof(WD_PCMCIA_CONFIG_DUMP), FALSE)
#define WD_PcmciaControl(h,pPcmciaControl)\
    WD_FUNCTION(IOCTL_WD_PCMCIA_CONTROL, h, pPcmciaControl, \
        sizeof(WD_PCMCIA_CONTROL), FALSE)
#define WD_Sleep(h,pSleep)\
    WD_FUNCTION(IOCTL_WD_SLEEP, h, pSleep, sizeof (WD_SLEEP), FALSE)
#define WD_UsbScanDevice(h, pScan)\
    WD_FUNCTION(IOCTL_WD_USB_SCAN_DEVICES, h, pScan, \
        sizeof(WD_USB_SCAN_DEVICES), FALSE)
#define WD_UsbGetConfiguration(h, pConfig) \
    WD_FUNCTION(IOCTL_WD_USB_GET_CONFIGURATION, h, pConfig, \
        sizeof(WD_USB_CONFIGURATION), FALSE)
#define WD_UsbDeviceRegister(h, pRegister)\
    WD_FUNCTION(IOCTL_WD_USB_DEVICE_REGISTER, h, pRegister, \
        sizeof(WD_USB_DEVICE_REGISTER), FALSE)
#define WD_UsbTransfer(h, pTrans)\
    WD_FUNCTION(IOCTL_WD_USB_TRANSFER, h, pTrans, sizeof(WD_USB_TRANSFER), TRUE)
#define WD_UsbDeviceUnregister(h, pRegister)\
    WD_FUNCTION(IOCTL_WD_USB_DEVICE_UNREGISTER, h, pRegister, \
        sizeof(WD_USB_DEVICE_REGISTER), FALSE)
#define WD_UsbResetPipe(h, pResetPipe)\
    WD_FUNCTION(IOCTL_WD_USB_RESET_PIPE, h, pResetPipe, \
        sizeof(WD_USB_RESET_PIPE), FALSE)
#define WD_UsbResetDevice(h, hDevice)\
    WD_FUNCTION(IOCTL_WD_USB_RESET_DEVICE, h, &hDevice, sizeof(DWORD), FALSE)
#define WD_EventRegister(h, pEvent) \
    WD_FUNCTION(IOCTL_WD_EVENT_REGISTER, h, pEvent, SIZE_OF_WD_EVENT(pEvent), \
        FALSE)
#define WD_EventUnregister(h, pEvent) \
    WD_FUNCTION(IOCTL_WD_EVENT_UNREGISTER, h, pEvent, \
        SIZE_OF_WD_EVENT(pEvent), FALSE)
#define WD_EventPull(h,pEvent) \
    WD_FUNCTION(IOCTL_WD_EVENT_PULL, h, pEvent, SIZE_OF_WD_EVENT(pEvent), FALSE)
#define WD_EventSend(h,pEvent) \
    WD_FUNCTION(IOCTL_WD_EVENT_SEND, h, pEvent, SIZE_OF_WD_EVENT(pEvent), FALSE)
#define WD_UsbResetDeviceEx(h, pReset)\
    WD_FUNCTION(IOCTL_WD_USB_RESET_DEVICEEX, h, pReset, \
        sizeof(WD_USB_RESET_DEVICE), FALSE)
#define WD_Usage(h, pStop) \
    WD_FUNCTION(IOCTL_WD_USAGE, h, pStop, sizeof(WD_USAGE), FALSE)

#define WD_UGetDeviceData(h, pGetDevData) \
    WD_FUNCTION(IOCTL_WDU_GET_DEVICE_DATA, h, pGetDevData, \
        sizeof(WDU_GET_DEVICE_DATA), FALSE);
#define WD_GetDeviceProperty(h, pGetDevProperty) \
    WD_FUNCTION(IOCTL_WD_GET_DEVICE_PROPERTY, h, pGetDevProperty, \
        sizeof(WD_GET_DEVICE_PROPERTY), FALSE);
#define WD_USetInterface(h, pSetIfc) \
    WD_FUNCTION(IOCTL_WDU_SET_INTERFACE, h, pSetIfc, \
        sizeof(WDU_SET_INTERFACE), FALSE);
#define WD_UResetPipe(h, pResetPipe) \
    WD_FUNCTION(IOCTL_WDU_RESET_PIPE, h, pResetPipe, \
        sizeof(WDU_RESET_PIPE), FALSE);
#define WD_UTransfer(h, pTrans) \
    WD_FUNCTION(IOCTL_WDU_TRANSFER, h, pTrans, sizeof(WDU_TRANSFER), TRUE);
#define WD_UHaltTransfer(h, pHaltTrans) \
    WD_FUNCTION(IOCTL_WDU_HALT_TRANSFER, h, pHaltTrans, \
        sizeof(WDU_HALT_TRANSFER), FALSE);
#define WD_WatchPciStart(h, pHsWatch)\
    WD_FUNCTION(IOCTL_WD_WATCH_PCI_START, h, pHsWatch, \
        SIZE_OF_WD_HS_WATCH(pHsWatch), FALSE)
#define WD_WatchPciStop(h, pHsWatch)\
    WD_FUNCTION(IOCTL_WD_WATCH_PCI_STOP, h, pHsWatch, \
        SIZE_OF_WD_HS_WATCH(pHsWatch), FALSE)
#define WD_UWakeup(h, pWakeup) \
    WD_FUNCTION(IOCTL_WDU_WAKEUP, h, pWakeup, sizeof(WDU_WAKEUP), FALSE);
#define WD_USelectiveSuspend(h, pSelectiveSuspend) \
    WD_FUNCTION(IOCTL_WDU_SELECTIVE_SUSPEND, h, pSelectiveSuspend, \
        sizeof(WDU_SELECTIVE_SUSPEND), FALSE);
#define WD_UResetDevice(h, pResetDevice) \
    WD_FUNCTION(IOCTL_WDU_RESET_DEVICE, h, pResetDevice, \
        sizeof(WDU_RESET_DEVICE), FALSE);
#define WD_UStreamOpen(h, pStream) \
    WD_FUNCTION(IOCTL_WDU_STREAM_OPEN, h, pStream, sizeof(WDU_STREAM), FALSE);
#define WD_UStreamClose(h, pStream) \
    WD_FUNCTION(IOCTL_WDU_STREAM_CLOSE, h, pStream, sizeof(WDU_STREAM), FALSE);
#define WD_UStreamStart(h, pStream) \
    WD_FUNCTION(IOCTL_WDU_STREAM_START, h, pStream, sizeof(WDU_STREAM), FALSE);
#define WD_UStreamStop(h, pStream) \
    WD_FUNCTION(IOCTL_WDU_STREAM_STOP, h, pStream, sizeof(WDU_STREAM), FALSE);
#define WD_UStreamFlush(h, pStream) \
    WD_FUNCTION(IOCTL_WDU_STREAM_FLUSH, h, pStream, sizeof(WDU_STREAM), FALSE);
#define WD_UStreamGetStatus(h, pStreamStatus) \
    WD_FUNCTION(IOCTL_WDU_STREAM_GET_STATUS, h, pStreamStatus, \
        sizeof(WDU_STREAM_STATUS), FALSE);
#define WD_InterruptDoneCe(h, pInt) \
    WD_FUNCTION(IOCTL_WD_INTERRUPT_DONE_CE, h, pInt, sizeof(WD_INTERRUPT), \
        FALSE);

#define __ALIGN_DOWN(val,alignment) ( (val) & ~((alignment) - 1) )
#define __ALIGN_UP(val,alignment) \
    ( ((val) + (alignment) - 1) & ~((alignment) - 1) )

#ifdef WDLOG
#include "wd_log.h"
#endif

#ifndef MIN
    #define MIN(a,b) ((a) > (b) ? (b) : (a))
#endif
#ifndef MAX
    #define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif
#define SAFE_STRING(s) ((s) ? (s) : "")

#if defined(LINUX) && defined(PPC64)
#pragma pack(pop)
#endif

#ifdef __cplusplus
}
#endif

#endif

