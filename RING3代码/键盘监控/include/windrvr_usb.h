/* Copyright (c) 2008 Jungo Ltd.  All Rights reserved. */

/* 
 * This program is free software; you can redistribute it and/or modify it 
 * under the terms of the GNU General Public License version 2 as published by 
 * the Free Software Foundation. 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License v2 for 
 * more details.
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, contact Jungo Ltd. at 
 * http://www.jungo.com/openrg/opensource_ack.html
 */

#if !defined(_WINDRVR_USB_H_)
#define _WINDRVR_USB_H_

#if defined(LINUX)
    #if !defined(__P_TYPES__)
        #define __P_TYPES__
        typedef void VOID;
        typedef unsigned char UCHAR;
        typedef unsigned short USHORT;
        typedef unsigned int UINT;
        typedef unsigned long ULONG;
        typedef ULONG BOOL;
        typedef void *PVOID;
        typedef unsigned char *PBYTE;
        typedef char CHAR;
        typedef char *PCHAR;
        typedef unsigned short *PWORD;
        typedef unsigned long DWORD, *PDWORD;
        typedef int PRCHANDLE;
        typedef PVOID HANDLE;
        typedef long LONG;
    #endif
    #if !defined(TRUE)
        #define TRUE 1
    #endif
    #if !defined(FALSE)
        #define FALSE 0
    #endif
#endif  

typedef enum {
    PIPE_TYPE_CONTROL     = 0,
    PIPE_TYPE_ISOCHRONOUS = 1,
    PIPE_TYPE_BULK        = 2,
    PIPE_TYPE_INTERRUPT   = 3
} USB_PIPE_TYPE;

#define WD_USB_MAX_PIPE_NUMBER 32
#define WD_USB_MAX_ENDPOINTS WD_USB_MAX_PIPE_NUMBER
#define WD_USB_MAX_INTERFACES 30

typedef enum {
    WDU_DIR_IN     = 1,
    WDU_DIR_OUT    = 2,
    WDU_DIR_IN_OUT = 3
} WDU_DIR;

/* USB TRANSFER options */
enum {
    USB_ISOCH_FULL_PACKETS_ONLY = 0x20,
    /* Windows only, ignored on other OS: */
    USB_ABORT_PIPE = 0x40,
    USB_ISOCH_NOASAP = 0x80,
    USB_BULK_INT_URB_SIZE_OVERRIDE_128K = 0x100, /* Force a 128KB maximum 
                                                    URB size */
    /* All OS */
    USB_STREAM_OVERWRITE_BUFFER_WHEN_FULL = 0x200,

    /* The following flags are no longer used beginning with v6.0: */
    USB_TRANSFER_HALT = 0x1,
    USB_SHORT_TRANSFER = 0x2,
    USB_FULL_TRANSFER = 0x4,
    USB_ISOCH_ASAP = 0x8,
    USB_ISOCH_RESET = 0x10
};

typedef PVOID WDU_REGISTER_DEVICES_HANDLE;

#define WDU_ENDPOINT_TYPE_MASK 0x03

#define WDU_ENDPOINT_DIRECTION_MASK 0x80
#define WDU_ENDPOINT_ADDRESS_MASK 0x0f
/* test direction bit in the bEndpointAddress field of an endpoint
 * descriptor. */
#define WDU_ENDPOINT_DIRECTION_OUT(addr) \
    (!((addr) & WDU_ENDPOINT_DIRECTION_MASK))
#define WDU_ENDPOINT_DIRECTION_IN(addr) \
    ((addr) & WDU_ENDPOINT_DIRECTION_MASK)
#define WDU_GET_MAX_PACKET_SIZE(x) \
    ((USHORT) (((x) & 0x7ff) * (1 + (((x) & 0x1800) >> 11))))

typedef struct
{
    DWORD dwNumber;        // Pipe 0 is the default pipe
    DWORD dwMaximumPacketSize;
    DWORD type;            // USB_PIPE_TYPE
    DWORD direction;       // WDU_DIR
                           // Isochronous, Bulk, Interrupt are either USB_DIR_IN or USB_DIR_OUT
                           // Control are USB_DIR_IN_OUT
    DWORD dwInterval;      // interval in ms relevant to Interrupt pipes
} WD_USB_PIPE_INFO, WD_USB_PIPE_INFO_V43, WDU_PIPE_INFO;

typedef struct 
{
    UCHAR bLength;
    UCHAR bDescriptorType;
    UCHAR bInterfaceNumber;
    UCHAR bAlternateSetting;
    UCHAR bNumEndpoints;
    UCHAR bInterfaceClass;
    UCHAR bInterfaceSubClass;
    UCHAR bInterfaceProtocol;
    UCHAR iInterface;
} WDU_INTERFACE_DESCRIPTOR;

typedef struct 
{
    UCHAR bLength;
    UCHAR bDescriptorType;
    UCHAR bEndpointAddress;
    UCHAR bmAttributes;
    USHORT wMaxPacketSize;
    UCHAR bInterval;
} WDU_ENDPOINT_DESCRIPTOR;

typedef struct 
{
    UCHAR bLength;
    UCHAR bDescriptorType;
    USHORT wTotalLength;
    UCHAR bNumInterfaces;
    UCHAR bConfigurationValue;
    UCHAR iConfiguration;
    UCHAR bmAttributes;
    UCHAR MaxPower;
} WDU_CONFIGURATION_DESCRIPTOR;

typedef struct 
{
    UCHAR bLength;
    UCHAR bDescriptorType;
    USHORT bcdUSB;
    UCHAR bDeviceClass;
    UCHAR bDeviceSubClass;
    UCHAR bDeviceProtocol;
    UCHAR bMaxPacketSize0;

    USHORT idVendor;
    USHORT idProduct;
    USHORT bcdDevice;
    UCHAR iManufacturer;
    UCHAR iProduct;
    UCHAR iSerialNumber;
    UCHAR bNumConfigurations;
} WDU_DEVICE_DESCRIPTOR;

typedef struct
{
    WDU_INTERFACE_DESCRIPTOR Descriptor;
    WDU_ENDPOINT_DESCRIPTOR *pEndpointDescriptors;
    WDU_PIPE_INFO *pPipes;
} WDU_ALTERNATE_SETTING;

typedef struct
{
    WDU_ALTERNATE_SETTING *pAlternateSettings;
    DWORD dwNumAltSettings;
    WDU_ALTERNATE_SETTING *pActiveAltSetting;
} WDU_INTERFACE;

typedef struct
{
    WDU_CONFIGURATION_DESCRIPTOR Descriptor; 
    DWORD dwNumInterfaces; 
    WDU_INTERFACE *pInterfaces;
} WDU_CONFIGURATION;

typedef struct {
    WDU_DEVICE_DESCRIPTOR Descriptor;
    WDU_PIPE_INFO Pipe0;
    WDU_CONFIGURATION *pConfigs;
    WDU_CONFIGURATION *pActiveConfig;
    WDU_INTERFACE *pActiveInterface[WD_USB_MAX_INTERFACES];
} WDU_DEVICE;

/* note: any devices found matching this table will be controlled */
typedef struct
{
    USHORT wVendorId;
    USHORT wProductId;
    UCHAR bDeviceClass;
    UCHAR bDeviceSubClass;
    UCHAR bInterfaceClass;
    UCHAR bInterfaceSubClass;
    UCHAR bInterfaceProtocol;
} WDU_MATCH_TABLE;

typedef struct 
{
    DWORD dwUniqueID;
    PVOID pBuf;
    DWORD dwBytes;
    DWORD dwOptions;
} WDU_GET_DEVICE_DATA;

/* dwOptions values for WDU_GET_DEVICE_DATA structures; 
 * Currently unsupported */
typedef enum
{
    WDU_FORCE_CONFIG_REQ = 0x1
} WDU_GET_DEVICE_DATA_OPTION;

/* these enum values can be used as dwProperty values, see structure
 * WD_GET_DEVICE_PROPERTY below. */
typedef enum 
{
    WdDevicePropertyDeviceDescription,
    WdDevicePropertyHardwareID,
    WdDevicePropertyCompatibleIDs,
    WdDevicePropertyBootConfiguration,
    WdDevicePropertyBootConfigurationTranslated,
    WdDevicePropertyClassName,
    WdDevicePropertyClassGuid,
    WdDevicePropertyDriverKeyName,
    WdDevicePropertyManufacturer,
    WdDevicePropertyFriendlyName,
    WdDevicePropertyLocationInformation,
    WdDevicePropertyPhysicalDeviceObjectName,
    WdDevicePropertyBusTypeGuid,
    WdDevicePropertyLegacyBusType,
    WdDevicePropertyBusNumber,
    WdDevicePropertyEnumeratorName,
    WdDevicePropertyAddress,
    WdDevicePropertyUINumber,
    WdDevicePropertyInstallState,
    WdDevicePropertyRemovalPolicy
} WD_DEVICE_REGISTRY_PROPERTY;

typedef struct 
{
    DWORD dwUniqueID;
    DWORD dwInterfaceNum;
    DWORD dwAlternateSetting;
    DWORD dwOptions;
} WDU_SET_INTERFACE;

typedef struct
{
    DWORD dwUniqueID;
    DWORD dwPipeNum;
    DWORD dwOptions;
} WDU_RESET_PIPE;

typedef enum {
    WDU_WAKEUP_ENABLE = 0x1,
    WDU_WAKEUP_DISABLE = 0x2
} WDU_WAKEUP_OPTIONS;

typedef enum {
    WDU_SELECTIVE_SUSPEND_SUBMIT = 0x1,
    WDU_SELECTIVE_SUSPEND_CANCEL = 0x2,
} WDU_SELECTIVE_SUSPEND_OPTIONS;

typedef struct
{
    DWORD dwUniqueID;
    DWORD dwPipeNum;
    DWORD dwOptions;
} WDU_HALT_TRANSFER;

typedef struct
{
    DWORD dwUniqueID;
    DWORD dwOptions;
} WDU_WAKEUP;

typedef struct
{
    DWORD dwUniqueID;
    DWORD dwOptions;
} WDU_SELECTIVE_SUSPEND;

typedef struct
{
    DWORD dwUniqueID;
    DWORD dwOptions;
} WDU_RESET_DEVICE;

typedef struct
{
    DWORD dwUniqueID;
    DWORD dwPipeNum; /* Pipe number on device. */
    DWORD fRead; /* TRUE for read (IN) transfers; FALSE for write (OUT)
                    transfers. */
    DWORD dwOptions; /* USB_TRANSFER options:
                        USB_ISOCH_FULL_PACKETS_ONLY - For isochronous 
                        transfers only. If set, only full packets will be 
                        transmitted and the transfer function will return 
                        when the amount of bytes left to transfer is less 
                        than the maximum packet size for the pipe (the 
                        function will return without transmitting the 
                        remaining bytes). */
    PVOID pBuffer;   /* Pointer to buffer to read/write. */
    DWORD dwBufferSize; /* Amount of bytes to transfer. */
    DWORD dwBytesTransferred; /* Returns the number of bytes actually
                                 read/written */
    UCHAR SetupPacket[8]; /* Setup packet for control pipe transfer. */
    DWORD dwTimeout; /* Timeout for the transfer in milliseconds. Set to 0 for
                        infinite wait. */
} WDU_TRANSFER;

typedef struct
{
    DWORD dwUniqueID;
    UCHAR bType; 
    UCHAR bIndex;
    USHORT wLength;
    PVOID pBuffer;
    USHORT wLanguage;
} WDU_GET_DESCRIPTOR;

typedef struct
{
    DWORD dwUniqueID;
    DWORD dwOptions;
    DWORD dwPipeNum;
    DWORD dwBufferSize;
    DWORD dwRxSize;
    BOOL  fBlocking;
    DWORD dwRxTxTimeout;
    DWORD dwReserved;
} WDU_STREAM;

typedef struct
{
    DWORD dwUniqueID;
    DWORD dwOptions;
    BOOL  fIsRunning;
    DWORD dwLastError;
    DWORD dwBytesInBuffer;
    DWORD dwReserved;
} WDU_STREAM_STATUS;

#endif
