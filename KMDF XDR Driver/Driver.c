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
    ProcBufferAmount = 100;
    ProcBufferOffset = 0;
    ProcBufferLength = ProcBufferAmount * sizeof(PROCESS_DATA);
    ProcBuffer = (PPROCESS_DATA) ExAllocatePool2(POOL_FLAG_NON_PAGED, ProcBufferLength, MyTag);
    if (ProcBuffer == NULL) {
        DebugMessage("Failed to allocate memory for ProcBuffer\n");
    }
    else {
        DebugMessage("allocated %zu bytes for ProcBuffer\n", ProcBufferLength);
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

