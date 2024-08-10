#pragma once

#include <ntifs.h>

// communication
#define IO_GET_HELLOWORLD CTL_CODE(FILE_DEVICE_UNKNOWN, 0x999, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
// Maybe need to use direct I/O or METHOD_OUT_DIRECT instead of buffered?

NTSTATUS CreateCall(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS CloseCall(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS IoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp);