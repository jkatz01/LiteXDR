#if !defined(_XDR_H_)
#define _XDR_H_

#include <ntddk.h>
#include <wdf.h>

#define NTSTRSAFE_LIB
#include <ntstrsafe.h>

#include "wmilib.h"
#include <initguid.h>

//
// The device extension for the device object
//
typedef struct _FDO_DATA
{
    int TheThing;
}  FDO_DATA, * PFDO_DATA;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(FDO_DATA, XdrFdoGetData)

DRIVER_INITIALIZE DriverEntry;
DRIVER_UNLOAD XdrEvtDriverUnload;

EVT_WDF_DRIVER_DEVICE_ADD XdrEvtDeviceAdd;

EVT_WDF_DEVICE_CONTEXT_CLEANUP XdrEvtDeviceContextCleanup;
//
// Io events callbacks.
//
//EVT_WDF_IO_QUEUE_IO_READ XdrEvtIoRead;
//EVT_WDF_IO_QUEUE_IO_WRITE XdrEvtIoWrite;
//EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL XdrEvtIoDeviceControl;
//EVT_WDF_DEVICE_FILE_CREATE XdrEvtDeviceFileCreate;
//EVT_WDF_FILE_CLOSE XdrEvtFileClose;


#endif  // _XDR_H_
