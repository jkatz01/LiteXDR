#include "communication.h"

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
    ULONG ByteIO = 0;

    PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp); //stack we need to communicate with driver

    ULONG ControlCode = stack->Parameters.DeviceIoControl.IoControlCode;

    if (ControlCode == IO_GET_HELLOWORLD) {
        PULONG Output = (PULONG)Irp->AssociatedIrp.SystemBuffer; // Can cast this to any struct/output you want
        *Output = 1234567;

        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Helloworld requested.\n");

        status = STATUS_SUCCESS;
        ByteIO = sizeof(*Output);
    }
    else {
        ByteIO = 0;
    }

    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = ByteIO;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return status;
}