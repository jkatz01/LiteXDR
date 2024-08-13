#include "driver.h"
#include "events.h"
#include "communication.h"
#include "data.h"

NTSTATUS DriverEntry( PDRIVER_OBJECT  DriverObject, PUNICODE_STRING RegistryPath )
{
    UNREFERENCED_PARAMETER(RegistryPath);
    NTSTATUS    status = STATUS_SUCCESS;

    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "XdrDriverEntry\n");

    // Register module load notification callback
    status = PsSetLoadImageNotifyRoutineEx(ModuleLoadCallback, PS_IMAGE_NOTIFY_CONFLICTING_ARCHITECTURE);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    DriverObject->DriverUnload = UnloadDriver;

    RtlInitUnicodeString(&dev, L"\\Device\\xdr");
    RtlInitUnicodeString(&dos, L"\\DosDevices\\xdr");

    IoCreateDevice(DriverObject, 0, &dev, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &pDeviceObject);
    IoCreateSymbolicLink(&dos, &dev);


    DriverObject->MajorFunction[IRP_MJ_CREATE] = CreateCall;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = CloseCall;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoControl;

    DriverObject->Flags |= DO_DIRECT_IO;
    DriverObject->Flags &= ~DO_DEVICE_INITIALIZING;

    UNICODE_STRING source_string = RTL_CONSTANT_STRING(L"hello driver string");
    UNICODE_STRING dest_string = {0};
    USHORT length;
    length = source_string.Length;
    dest_string.Buffer = (PWCHAR) ExAllocatePool2(POOL_FLAG_NON_PAGED, length, MyTag);
    if (dest_string.Buffer != NULL) {
        dest_string.Length = length;
        dest_string.MaximumLength = length;
        RtlMoveMemory(dest_string.Buffer, source_string.Buffer, source_string.Length);
        DebugMessage("allocated string is %wZ \r\n", &dest_string);
        ExFreePool(dest_string.Buffer);
    }
    else {
        DebugMessage("Failed to allocate memory\n");
    }

    return status;
}

NTSTATUS UnloadDriver( PDRIVER_OBJECT DriverObject )
{
    UNREFERENCED_PARAMETER(DriverObject);

    PsRemoveLoadImageNotifyRoutine(ModuleLoadCallback);
    ExFreePool(ProcBuffer);

    IoDeleteSymbolicLink(&dos);
    IoDeleteDevice(DriverObject->DeviceObject);

    DebugMessage("XdrEvtExit\n");

    return STATUS_SUCCESS;
}

