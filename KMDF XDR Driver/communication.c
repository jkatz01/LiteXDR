#include "communication.h"
#include "helper_funcs.h"
#include "data.h"

//// Communication
NTSTATUS CreateCall(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    UNREFERENCED_PARAMETER(DeviceObject);
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "CreateCall was called, connection established.\n");

    return STATUS_SUCCESS;
}

NTSTATUS CloseCall(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    UNREFERENCED_PARAMETER(DeviceObject);
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;

    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Connection terminated.\n");

    return STATUS_SUCCESS;
}

NTSTATUS IoControl(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    UNREFERENCED_PARAMETER(DeviceObject);
    NTSTATUS status = STATUS_UNSUCCESSFUL;
    ULONG ReturnLength = 0;

    PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp); //stack we need to communicate with driver
    ULONG ControlCode = stack->Parameters.DeviceIoControl.IoControlCode;

    PVOID Buffer = Irp->AssociatedIrp.SystemBuffer;
    //ULONG BufferInLength = stack->Parameters.DeviceIoControl.InputBufferLength;
    //ULONG BufferOutLength = stack->Parameters.DeviceIoControl.OutputBufferLength;

    PULONG OutputNumber = (PULONG)Buffer;

    switch (ControlCode) {
        case IO_RECEIVE_HELLOWORLD:
            DebugMessage("Helloworld requested.\n");
            *OutputNumber = 1234567;
            status = STATUS_SUCCESS;
            ReturnLength = sizeof(*OutputNumber);
            break;
        case IO_RECEIVE_PROC_BUFFER:
            DebugMessage("Process buffer requested.\n");
            RtlCopyMemory(Buffer, ProcBufferPtr, ProcBufferBytesWritten);
            ReturnLength = (ULONG)ProcBufferBytesWritten;
            status = STATUS_SUCCESS;
            break;
        default:
            status = STATUS_INVALID_PARAMETER;
            break;
    }

    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = ReturnLength;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return status;
}