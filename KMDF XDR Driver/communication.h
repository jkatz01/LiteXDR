#pragma once

#include <ntifs.h>

// communication
// Receive = user receives
// Send = user sends
#define IO_RECEIVE_HELLOWORLD CTL_CODE(FILE_DEVICE_UNKNOWN, 0x900, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_RECEIVE_PROC_BUFFER CTL_CODE(FILE_DEVICE_UNKNOWN, 0x901, METHOD_BUFFERED, FILE_WRITE_DATA)

// Maybe need to use direct I/O or METHOD_OUT_DIRECT instead of buffered?

NTSTATUS CreateCall(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS CloseCall(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS IoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp);