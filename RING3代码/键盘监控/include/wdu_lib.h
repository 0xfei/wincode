/* Jungo Confidential. Copyright (c) 2008 Jungo Ltd.  http://www.jungo.com */

#ifndef _WD_USB_H_
#define _WD_USB_H_

#include "windrvr.h"

#if defined(__cplusplus)
    extern "C" {
#endif

typedef PVOID WDU_DRIVER_HANDLE; 
typedef PVOID WDU_DEVICE_HANDLE;
typedef PVOID WDU_STREAM_HANDLE;

typedef WORD WDU_LANGID;

/*
 * User Callbacks
 */

/*
 * Function typedef: WDU_ATTACH_CALLBACK()
 *   WinDriver calls this function with any new device that is attached, 
 *   matches the given criteria, and if WD_ACKNOWLEDGE was passed to WDU_Init()
 *   in dwOptions - not controlled yet by another driver.
 *   This callback is called once for each matching interface.
 * Parameters:
 *   [in] hDevice:     A unique identifier for the device/interface
 *   [in] pDeviceInfo: Pointer to device configuration details.
 *                     This pointer is valid until the end of the function.
 *   [in] pUserData:   Pointer to user data that was passed to WDU_Init (in the
 *                     event table).
 * Return Value:
 *   If WD_ACKNOWLEDGE was passed to WDU_Init(), the implementor should check & 
 *   return if he wants to control the device.
 */
typedef BOOL (DLLCALLCONV *WDU_ATTACH_CALLBACK)(WDU_DEVICE_HANDLE hDevice, 
    WDU_DEVICE *pDeviceInfo, PVOID pUserData);

/*
 * Function typedef: WDU_DETACH_CALLBACK()
 *   WinDriver calls this function when a controlled device has been detached 
 *   from the system.
 * Parameters:
 *   [in] hDevice:      A unique identifier for the device/interface.
 *   [in] pUserData:    Pointer to user data that was passed to WDU_Init.
 * Return Value:
 *   None.
 */
typedef void (DLLCALLCONV *WDU_DETACH_CALLBACK)(WDU_DEVICE_HANDLE hDevice,
    PVOID pUserData);

/*
 * Function typedef: WDU_POWER_CHANGE_CALLBACK()
 *   (currently (CE.Net 4.10) pRegisterNotificationRoutine will pass only 
 *   USB_CLOSE_DEVICE event (see microsoft documentation)).
 * Parameters:
 *   [in] hDevice:      A unique identifier for the device/interface.
 *   [in] dwPowerState: Number of the power state selected.
 *   [in] pUserData:    Pointer to user data that was passed to WDU_Init (in the
 *                      event table).
 * Return Value:
 *   TRUE/FALSE; Currently there is no significance to the return value.
 */
typedef BOOL (DLLCALLCONV *WDU_POWER_CHANGE_CALLBACK)(WDU_DEVICE_HANDLE hDevice,
    DWORD dwPowerState, PVOID pUserData);

/*
 * struct definitions
 */
typedef struct
{
    WDU_ATTACH_CALLBACK pfDeviceAttach;
    WDU_DETACH_CALLBACK pfDeviceDetach;
    WDU_POWER_CHANGE_CALLBACK pfPowerChange;
    PVOID pUserData;  /* pointer to pass in each callback */
} WDU_EVENT_TABLE;

/*
 * API Functions
 */

/*
 * Function: WDU_Init()
 *   Starts listening to devices matching a criteria, and registers
 *   notification callbacks for those devices.
 * Parameters:
 *   [out] phDriver:        Handle to this registration of events & criteria.
 *   [in] pMatchTables:     Array of match tables defining the devices-criteria.
 *   [in] dwNumMatchTables: Number of elements in pMatchTables.
 *   [in] pEventTable:      Notification callbacks when the device's status
 *                          changes.
 *   [in] sLicense:         WinDriver's license string.
 *   [in] dwOptions:        Can be 0 or:
 *                          WD_ACKNOWLEDGE - The user can seize control over the
 *                              device in WDU_ATTACH_CALLBACK return value.
 * Return Value:
 *   WinDriver Error Code
 */
DWORD DLLCALLCONV WDU_Init(WDU_DRIVER_HANDLE *phDriver,
    WDU_MATCH_TABLE *pMatchTables, DWORD dwNumMatchTables,
    WDU_EVENT_TABLE *pEventTable, const char *sLicense, DWORD dwOptions);

/*
 * Function: WDU_Uninit()
 *   Stops listening to devices matching the criteria, and unregisters the 
 *   notification callbacks for those devices.
 * Parameters:
 *   [in] hDriver: Handle to the registration, received from WDU_Init.
 * Return Value:
 *   None
 */
void DLLCALLCONV WDU_Uninit(WDU_DRIVER_HANDLE hDriver);

/*
 * Function: WDU_GetDeviceAddr()
 *   Gets USB address that the device uses. The address number is written to
 *   the caller supplied pAddress.
 * Parameters:
 *   [in] hDevice:   A unique identifier for the device/interface.
 *   [out] pAddress: Pointer to DWORD, in which the result will be returned.
 * Return Value:
 *   WinDriver Error Code
 * NOTE: This function is supported only on Windows 2000 and higher.
 */
DWORD DLLCALLCONV WDU_GetDeviceAddr(WDU_DEVICE_HANDLE hDevice, DWORD *pAddress);

/*
 * Function: WDU_GetDeviceRegistryProperty()
 *   Gets the specified registry property of a given device.
 * Parameters:
 *  [in] hDevice:     A unique identifier of the device/interface.
 *  [out] pBuffer:    Pointer to a user allocated buffer to be filled with the
 *                    requested registry property. The function will fill the
 *                    buffer only if the buffer size, as indicated in the input
 *                    value of the pdwSize parameter, is sufficient - i.e. >=
 *                    the property's size, as returned via pdwSize.
 *                    pBuffer can be set to NULL when using the function
 *                    only to retrieve the size of the registry property (see
 *                    pdwSize).
 *  [in/out] pdwSize: As input, points to a value indicating the size of the
 *                    user-supplied buffer (pBuffer); if pBuffer is set to
 *                    NULL, the input value of this parameter is ignored.
 *                    As output, points to a value indicating the required
 *                    buffer size for storing the registry property.
 *  [in] property:    The ID of the registry property to be retrieved - see the
 *                    WD_DEVICE_REGISTRY_PROPERTY enumeration in windrvr.h.
 *                    Note: String registry properties are in WCHAR format.
 * Return Value:
 *   WinDriver Error Code.
 * NOTE: 
 * --  When the size of the provided user buffer (pBuffer) - *pdwSize (input) -
 *     is not sufficient to hold the requested registry property, the function
 *     returns WD_INVALID_PARAMETER.
 * --  This function is supported only on Windows 2000 and higher.
 */
DWORD DLLCALLCONV WDU_GetDeviceRegistryProperty(WDU_DEVICE_HANDLE hDevice, 
    PVOID pBuffer, PDWORD pdwSize, WD_DEVICE_REGISTRY_PROPERTY property);

/*
 * Function: WDU_GetDeviceInfo()
 *   Gets configuration information from the device including all the 
 *   descriptors in a WDU_DEVICE struct. The caller should free *ppDeviceInfo 
 *   after using it by calling WDU_PutDeviceInfo().
 * Parameters:
 *   [in]  hDevice:      A unique identifier for the device/interface.
 *   [out] ppDeviceInfo: Pointer to a pointer to a buffer containing the
 *                       device information.
 * Return Value:
 *   WinDriver Error Code
 */
DWORD DLLCALLCONV WDU_GetDeviceInfo(WDU_DEVICE_HANDLE hDevice,
    WDU_DEVICE **ppDeviceInfo);

/*
 * Function: WDU_PutDeviceInfo()
 *   Receives a device information pointer, allocated with a previous 
 *   WDU_GetDeviceInfo() call, in order to perform the necessary cleanup.
 * Parameters:
 *   [in] pDeviceInfo: Pointer to a buffer containing the device 
 *                     information, as returned by a previous call to
 *                     WDU_GetDeviceInfo().
 * Return Value:
 *   None
 */
void DLLCALLCONV WDU_PutDeviceInfo(WDU_DEVICE *pDeviceInfo);

/*
 * Function: WDU_SetInterface()
 *   Sets the alternate setting for the specified interface.
 * Parameters:
 *   [in] hDevice:              A unique identifier for the device/interface.
 *   [in] dwInterfaceNum:       The interface's number.
 *   [in] dwAlternateSetting:   The desired alternate setting value.
 * Return Value:
 *   WinDriver Error Code
 */
DWORD DLLCALLCONV WDU_SetInterface(WDU_DEVICE_HANDLE hDevice,
    DWORD dwInterfaceNum, DWORD dwAlternateSetting);

/* NOT IMPLEMENTED YET */
DWORD DLLCALLCONV WDU_SetConfig(WDU_DEVICE_HANDLE hDevice, DWORD dwConfigNum);
/* NOT IMPLEMENTED YET */

/*
 * Function: WDU_ResetPipe()
 *   Resets a pipe.
 * Parameters:
 *   [in] hDevice:      A unique identifier for the device/interface.
 *   [in] dwPipeNum:    Pipe number.
 * Return Value:
 *   WinDriver Error Code
 */
DWORD DLLCALLCONV WDU_ResetPipe(WDU_DEVICE_HANDLE hDevice, DWORD dwPipeNum);

/*
 * Function: WDU_ResetDevice()
 *   Resets a device (supported only on Windows and Windows CE 5.0 and newer).
 * Parameters:
 *   [in] hDevice:   A unique identifier for the device/interface
 *   [in] dwOptions: Can be 0 or:
 *                   WD_USB_HARD_RESET - will reset the device even if it is
 *                       not disabled.
 *                       After using this option it is advised to set the
 *                       interface of the device (WDU_SetInterface()).
 *                   WD_USB_CYCLE_PORT - will simulate unplugging and
 *                       replugging the device, prompting the operating
 *                       system to enumerate the device without resetting it.
 *                       This option is available only on Windows XP and
 *                       newer.
 * Return Value:
 *   WinDriver Error Code
 * NOTE: This function is supported only on Windows and Windows CE, beginning
 *       with Windows CE 5.0.
 *       The CYCLE_PORT option is supported only on Windows XP and newer.
 */
DWORD DLLCALLCONV WDU_ResetDevice(WDU_DEVICE_HANDLE hDevice, DWORD dwOptions);

/*
 * Function: WDU_Wakeup()
 *   Enables/Disables wakeup feature.
 * Parameters:
 *   [in] hDevice:      A unique identifier for the device/interface
 *   [in] dwOptions:    Can be set to either of the following options:
 *                      WDU_WAKEUP_ENABLE - will enable wakeup.
 *                      or:
 *                      WDU_WAKEUP_DISABLE - will disable wakeup.
 * Return Value:
 *   WinDriver Error Code
 * NOTE: This function is supported only on Windows.
 */
DWORD DLLCALLCONV WDU_Wakeup(WDU_DEVICE_HANDLE hDevice, DWORD dwOptions);

/*
 * Function: WDU_SelectiveSuspend()
 *   Submits a request to suspend a given device (selective suspend), or
 *   cancels a previous suspend request.
 * Parameters:
 *   [in] hDevice:   A unique identifier for the device/interface.
 *   [in] dwOptions: Can be set to either of the following
 *                   WDU_SELECTIVE_SUSPEND_OPTIONS enumeration values:
 *                   WDU_SELECTIVE_SUSPEND_SUBMIT - submit a request to
 *                       suspend the device.
 *                   WDU_SELECTIVE_SUSPEND_CANCEL - cancel a previous
 *                       suspend request for the device.
 * Return Value:
 *   WinDriver Error Code.
 *   If a suspend request is received while the device is busy, the function
 *   returns WD_OPERATION_FAILED.
 * NOTE: This function is supported only on Windows XP and higher.
 */
DWORD DLLCALLCONV WDU_SelectiveSuspend(WDU_DEVICE_HANDLE hDevice,
    DWORD dwOptions);

/*
 * Function: WDU_Transfer()
 *   Transfers data to/from a device.
 * Parameters:
 *   [in] hDevice:   A unique identifier for the device/interface.
 *   [in] dwPipeNum: The number of the pipe through which the data is
 *                   transferred.
 *   [in] fRead:     TRUE for read, FALSE for write.
 *   [in] dwOptions: Can be a bit-mask of any of the following options:
 *                   USB_ISOCH_NOASAP - For isochronous data transfers.
 *                       Setting this option instructs the lower driver
 *                       (usbd.sys) to use a preset frame number (instead of
 *                       the next available frame) while performing the data
 *                       transfer. Use this flag if you notice unused frames
 *                       during the transfer, on low-speed or full-speed
 *                       devices (USB 1.1 only) and on Windows only
 *                       (excluding Windows CE).
 *                   USB_ISOCH_RESET - Resets the isochronous pipe before the
 *                       data transfer. It also resets the pipe after minor
 *                       errors, consequently allowing the transfer to
 *                       continue.
 *                   USB_ISOCH_FULL_PACKETS_ONLY - When set, do not transfer
 *                       less than packet size on isochronous pipes.
 *                   USB_BULK_INT_URB_SIZE_OVERRIDE_128K - Limits the size of
 *                       the USB Request Block (URB) to 128KB.
 *   [in] pBuffer: location of the data buffer
 *   [in] dwBufferSize:         Number of the bytes to transfer.
 *   [out] pdwBytesTransferred: Number of bytes actually transferred.
 *   [in] pSetupPacket:         8-bytes packet to transfer to control pipes.
 *   [in] dwTimeout:            Maximum time, in milliseconds, to complete a
 *                              transfer. Zero = infinite wait.
 * Return Value:
 *   WinDriver Error Code
 */
DWORD DLLCALLCONV WDU_Transfer(WDU_DEVICE_HANDLE hDevice, DWORD dwPipeNum,
    DWORD fRead, DWORD dwOptions, PVOID pBuffer, DWORD dwBufferSize,
    PDWORD pdwBytesTransferred, PBYTE pSetupPacket, DWORD dwTimeout);

/*
 * Function: WDU_HaltTransfer()
 *   Halts the transfer on the specified pipe (only one simultaneous transfer
 *   per-pipe is allowed by WinDriver).
 * Parameters:
 *   [in] hDevice:      A unique identifier for the device/interface.
 *   [in] dwPipeNum:    Pipe number.
 * Return Value:
 *   WinDriver Error Code
 */
DWORD DLLCALLCONV WDU_HaltTransfer(WDU_DEVICE_HANDLE hDevice, DWORD dwPipeNum);

/*
 * Simplified transfers - for a specific pipe
 */

DWORD DLLCALLCONV WDU_TransferDefaultPipe(WDU_DEVICE_HANDLE hDevice,
    DWORD fRead, DWORD dwOptions, PVOID pBuffer, DWORD dwBufferSize,
    PDWORD pdwBytesTransferred, PBYTE pSetupPacket, DWORD dwTimeout);

DWORD DLLCALLCONV WDU_TransferBulk(WDU_DEVICE_HANDLE hDevice, DWORD dwPipeNum,
    DWORD fRead, DWORD dwOptions, PVOID pBuffer, DWORD dwBufferSize,
    PDWORD pdwBytesTransferred, DWORD dwTimeout);

DWORD DLLCALLCONV WDU_TransferIsoch(WDU_DEVICE_HANDLE hDevice, DWORD dwPipeNum,
    DWORD fRead, DWORD dwOptions, PVOID pBuffer, DWORD dwBufferSize,
    PDWORD pdwBytesTransferred, DWORD dwTimeout);

DWORD DLLCALLCONV WDU_TransferInterrupt(WDU_DEVICE_HANDLE hDevice,
    DWORD dwPipeNum, DWORD fRead, DWORD dwOptions, PVOID pBuffer,
    DWORD dwBufferSize, PDWORD pdwBytesTransferred, DWORD dwTimeout);

/*
 * Function: WDU_GetLangIDs()
 *   Reads a list of supported language IDs and/or the number of supported 
 *   language IDs from a device.
 * Parameters:
 *   [in] hDevice:                A unique identifier for the device/interface.
 *   [out] pbNumSupportedLangIDs: Pointer to the number of supported language
 *                                IDs, to be filled by the function.
 *                                Can be NULL if bNumLangIDs is not 0 and
 *                                pLangIDs is not NULL. If NULL, the function
 *                                will not return the number of supported
 *                                language IDs for the device.
 *   [out] pLangIDs:              Array of language IDs. If bNumLangIDs is not
 *                                0 the function will fill this array with the
 *                                supported language IDs for the device.
 *                                If bNumLangIDs < the number of supported
 *                                language IDs for the device, only the first
 *                                bNumLangIDs supported language IDs will be
 *                                read from the device and returned in the
 *                                pLangIDs array.
 *   [in] bNumLangIDs:            Number of IDs in pLangIDs array. If 0, the
 *                                function will only return the number of
 *                                supported language IDs.
 * Return Value:
 *   WinDriver Error Code
 * NOTE: If no language IDs are supported for the device 
 *       (*pbNumSupportedLangIDs == 0) the function returns 
 *       WD_STATUS_SUCCESS.
 */
DWORD DLLCALLCONV WDU_GetLangIDs(WDU_DEVICE_HANDLE hDevice, 
    PBYTE pbNumSupportedLangIDs, WDU_LANGID *pLangIDs, BYTE bNumLangIDs);

/*
 * Function: WDU_GetStringDesc()
 *   Reads a string descriptor from a device by string index.
 * Parameters:
 *   [in]  hDevice:     A unique identifier for the device/interface.
 *   [in]  bStrIndex:   Index of the string descriptor to read.
 *   [out] pbBuf:       Pointer to a buffer to be filled with the string
 *                      descriptor that is read from the device.
 *                      If the buffer is smaller than the string descriptor
 *                      (dwBufSize < *pdwDescSize), the returned descriptor will
 *                      be truncated to dwBufSize bytes.
 *   [in]  dwBufSize:   The size of the pbBuf buffer, in bytes.
 *   [in]  langID:      The language ID to be used in the get string descriptor
 *                      request that is sent to the device. If langID is 0, the
 *                      request will use the first supported language ID
 *                      returned by the device.
 *   [out] pdwDescSize: An optional DWORD pointer to be filled with the size of
 *                      the string descriptor read from the device.
 *                      If this parameter is NULL, the function will not return
 *                      the size of the string descriptor.
 * Return Value:
 *   WinDriver Error Code
 */
DWORD DLLCALLCONV WDU_GetStringDesc(WDU_DEVICE_HANDLE hDevice, BYTE bStrIndex, 
    PBYTE pbBuf, DWORD dwBufSize, WDU_LANGID langID, PDWORD pdwDescSize);


/*
 * Streaming Functions
 */

/*
 * NOTE: The streaming functions are currently supported only on
 *       Windows 2000 SP4 and higher.
 */

/*
 * Function: WDU_StreamOpen()
 *   Opens a new data stream for the specified pipe.
 *   A stream can be associated with any pipe except the control pipe (Pipe 0).
 *   The stream's data transfer direction -- read/write -- is derived from the
 *   direction of its pipe.
 * Parameters:
 *   [in] hDevice:          A unique identifier for the device/interface.
 *   [in] dwPipeNum:        The number of the pipe for which to open the stream.
 *   [in] dwBufferSize:     The size, in bytes, of the stream's data buffer. 
 *   [in] dwRxSize:         The size, in bytes, of the data blocks that the
 *                          stream reads from the device.
 *                          This parameter is relevant only for read streams
 *                          and must be <= dwBufferSize.
 *   [in] fBlocking:        TRUE for a blocking stream (performs blocking I/O);
 *                          FALSE for a non-blocking stream (non-blocking I/O).
 *   [in] dwOptions:        Can be a bit-mask of any of the following options:
 *                      USB_ISOCH_NOASAP - For isochronous data transfers.
 *                          Setting this option instructs the lower driver
 *                          (usbd.sys) to use a preset frame number (instead of
 *                          the next available frame) while performing the data
 *                          transfer. Use this flag if you notice unused frames
 *                          during the transfer, on low-speed or full-speed
 *                          devices (USB 1.1 only) and only on Windows.
 *                      USB_ISOCH_RESET - Resets the isochronous pipe before 
 *                          the data transfer. It also resets the pipe after
 *                          minor errors, consequently allowing the transfer to
 *                          continue.
 *                      USB_ISOCH_FULL_PACKETS_ONLY - When set, do not transfer
 *                          less than packet size on isochronous pipes.
 *                      USB_BULK_INT_URB_SIZE_OVERRIDE_128K - Limits the size of
 *                          the USB Request Block (URB) to 128KB.
 *                      USB_STREAM_OVERWRITE_BUFFER_WHEN_FULL - When there is
 *                          not enough free space in a read stream's data
 *                          buffer to complete the transfer, overwrite old data
 *                          in the buffer. (Applicable only to read streams).
 *   [in]  dwRxTxTimeout:   Maximum time, in milliseconds, for the completion of
 *                          a data transfer between the stream and the device.
 *                          Zero = infinite wait.
 *   [out] phStream:        Pointer to a unique identifier for the stream, to
 *                          be returned by the function and passed to the other
 *                          WDU_StreamXXX() functions.
 * Return Value:
 *   WinDriver Error Code
 */
DWORD DLLCALLCONV WDU_StreamOpen(WDU_DEVICE_HANDLE hDevice, DWORD dwPipeNum, 
    DWORD dwBufferSize, DWORD dwRxSize, BOOL fBlocking, DWORD dwOptions, 
    DWORD dwRxTxTimeout, WDU_STREAM_HANDLE *phStream);

/*
 * Function: WDU_StreamClose()
 *   Closes an open stream.
 *   The function stops the stream, including flushing its data to the device
 *   (in the case of a write stream), before closing it.
 * Parameters:
 *   [in] hStream:  A unique identifier for the stream, as returned by
 *                  WDU_StreamOpen().
 * Return Value:
 *   WinDriver Error Code
 */
DWORD DLLCALLCONV WDU_StreamClose(WDU_STREAM_HANDLE hStream);

/*
 * Function: WDU_StreamStart()
 *   Starts a stream, i.e. starts transfers between the stream and the device.
 *   Data will be transferred according to the stream's direction - read/write.
 * Parameters:
 *   [in] hStream:  A unique identifier for the stream, as returned by
 *                  WDU_StreamOpen().
 * Return Value:
 *   WinDriver Error Code
 */
DWORD DLLCALLCONV WDU_StreamStart(WDU_STREAM_HANDLE hStream);

/*
 * Function: WDU_StreamStop()
 *   Stops a stream, i.e. stops transfers between the stream and the device.
 *   In the case of a write stream, the function flushes the stream - i.e.
 *   writes its contents to the device - before stopping it.
 * Parameters:
 *   [in] hStream:  A unique identifier for the stream, as returned by
 *                  WDU_StreamOpen().
 * Return Value:
 *   WinDriver Error Code
 */
DWORD DLLCALLCONV WDU_StreamStop(WDU_STREAM_HANDLE hStream);

/*
 * Function: WDU_StreamFlush()
 *   Flushes a stream, i.e. writes the entire contents of the stream's data
 *   buffer to the device (relevant for a write stream), and blocks until the
 *   completion of all pending I/O on the stream.
 *   This function can be called for both blocking and non-blocking streams.
 * Parameters:
 *   [in] hStream:  A unique identifier for the stream, as returned by
 *                  WDU_StreamOpen().
 * Return Value:
 *   WinDriver Error Code
 */
DWORD DLLCALLCONV WDU_StreamFlush(WDU_STREAM_HANDLE hStream);

/*
 * Function: WDU_StreamRead()
 *   Reads data from a read stream to the application.
 *   For a blocking stream (fBlocking=TRUE - see WDU_StreamOpen()), the call
 *   to this function is blocked until the specified amount of data is read, or
 *   until the stream's attempt to read from the device times out (i.e. the
 *   timeout period for transfers between the stream and the device, as set in
 *   the dwRxTxTimeout WDU_StreamOpen() parameter, expires).
 *   For a non-blocking stream, the function transfers to the application as
 *   much of the requested data as possible, subject to the amount of data
 *   currently available in the stream's data buffer, and returns immediately.
 *   For both blocking and non-blocking transfers, the function returns the
 *   amount of bytes that were actually read from the stream within the
 *   pdwBytesRead parameter.
 * Parameters:
 *   [in] hStream:          A unique identifier for the stream, as returned by
 *                          WDU_StreamOpen().
 *   [out] pBuffer:         Pointer to a data buffer to be filled with the data
 *                          read from the stream.
 *   [in] bytes:            Number of bytes to read from the stream.
 *   [out] pdwBytesRead:    Pointer to a value indicating the number of bytes
 *                          actually read from the stream.
 * Return Value:
 *   WinDriver Error Code
 */
DWORD DLLCALLCONV WDU_StreamRead(HANDLE hStream, PVOID pBuffer, DWORD bytes,
    DWORD *pdwBytesRead);

/*
 * Function: WDU_StreamWrite()
 *   Writes data from the application to a write stream.
 *   For a blocking stream (fBlocking=TRUE - see WDU_StreamOpen()), the call
 *   to this function is blocked until the entire data (*pBuffer) is written to
 *   the stream's data buffer, or until the stream's attempt to write to the
 *   device times out (i.e. the timeout period for transfers between the stream
 *   and the device, as set in the dwRxTxTimeout WDU_StreamOpen() parameter,
 *   expires).
 *   For a non-blocking stream (fBlocking=FALSE), the function writes as much
 *   of the write data as currently possible to the stream's data buffer, and
 *   returns immediately.
 *   For both blocking and non-blocking transfers, the function returns the
 *   amount of bytes that were actually written to the stream within the
 *   pdwBytesWritten parameter.
 * Parameters:
 *   [in] hStream:          A unique identifier for the stream, as returned by
 *                          WDU_StreamOpen().
 *   [in] pBuffer:          Pointer to a data buffer containing the data to
 *                          write to the stream.
 *   [in] bytes:            Number of bytes to write to the stream.
 *   [out] pdwBytesWritten: Pointer to a value indicating the number of bytes
 *                          actually written to the stream.
 * Return Value:
 *   WinDriver Error Code
 */
DWORD DLLCALLCONV WDU_StreamWrite(HANDLE hStream, const PVOID pBuffer, 
    DWORD bytes, DWORD *pdwBytesWritten);

/*
 * Function: WDU_StreamGetStatus()
 *   Returns a stream's current status.
 *   This function can be called for both blocking and non-blocking streams.
 * Parameters:
 *   [in] hStream:           A unique identifier for the stream, as returned by
 *                           WDU_StreamOpen().
 *   [out] pfIsRunning       Pointer to a boolean value indicating the stream's
 *                           current state:
 *                           TRUE - the stream is currently running;
 *                           FALSE - the stream is currently stopped.
 *   [out] pdwLastError:     Pointer to the last error associated with the
 *                           stream. Note: Calling this function also resets
 *                           the stream's last error.
 *   [out] pdwBytesInBuffer: Pointer to the current bytes count in the stream's
 *                           data buffer.
 *
 * Return Value:
 *   WinDriver Error Code
 */
DWORD DLLCALLCONV WDU_StreamGetStatus(WDU_STREAM_HANDLE hStream, 
    BOOL *pfIsRunning, DWORD *pdwLastError, DWORD *pdwBytesInBuffer);

#ifdef __cplusplus
}
#endif

#endif
