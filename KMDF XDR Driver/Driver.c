#include "driver.h"
#include "events.h"
#include "communication.h"
#include "data.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (INIT, DriverEntry)
//#pragma alloc_text (PAGE, ToasterEvtDeviceAdd)
//#pragma alloc_text (PAGE, ToasterEvtIoRead)
//#pragma alloc_text (PAGE, ToasterEvtIoWrite)
//#pragma alloc_text (PAGE, ToasterEvtIoDeviceControl)
#endif


NTSTATUS
DriverEntry( IN PDRIVER_OBJECT  DriverObject, IN PUNICODE_STRING RegistryPath )
{
    NTSTATUS            status = STATUS_SUCCESS;
    WDF_DRIVER_CONFIG   config;

    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "XdrDriverEntry\n");

    // Register module load notification callback
    status = PsSetLoadImageNotifyRoutineEx(ModuleLoadCallback, PS_IMAGE_NOTIFY_CONFLICTING_ARCHITECTURE);
    if (!NT_SUCCESS(status))
    {
        // Handle error
        return status;
    }

    WDF_DRIVER_CONFIG_INIT( &config, XdrEvtDeviceAdd );

    // Create a framework driver object to represent our driver.
    status = WdfDriverCreate(
        DriverObject,
        RegistryPath,
        WDF_NO_OBJECT_ATTRIBUTES, // Driver Attributes
        &config,          // Driver Config Info
        WDF_NO_HANDLE
    );
    DriverObject->DriverUnload = XdrEvtDriverUnload;

    if (!NT_SUCCESS(status)) {
        KdPrint(("WdfDriverCreate failed with status 0x%x\n", status));
    }

    RtlInitUnicodeString(&dev, L"\\Device\\xdr");
    RtlInitUnicodeString(&dos, L"\\DosDevices\\xdr");

    IoCreateDevice(DriverObject, 0, &dev, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &pDeviceObject);
    if (!NT_SUCCESS(status)) {
        KdPrint(("Creating device failed\r\n"));
        return status;
    }
    status = IoCreateSymbolicLink(&dos, &dev);
    if (!NT_SUCCESS(status)) {
        KdPrint(("Creating symbolic link failed\r\n"));
        IoDeleteDevice(DriverObject->DeviceObject);
        return status;
    }

    DriverObject->MajorFunction[IRP_MJ_CREATE] = CreateCall;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = CloseCall;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoControl;

    DriverObject->Flags |= DO_DIRECT_IO;
    DriverObject->Flags &= ~DO_DEVICE_INITIALIZING;

    

    return status;
}


NTSTATUS XdrEvtDeviceAdd( IN WDFDRIVER       Driver, IN PWDFDEVICE_INIT DeviceInit )
{
    NTSTATUS               status = STATUS_SUCCESS;
    //PFDO_DATA              fdoData;
    WDF_OBJECT_ATTRIBUTES  fdoAttributes;
    WDFDEVICE              hDevice;

    UNREFERENCED_PARAMETER(Driver);

    PAGED_CODE();

    //KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "XdrDeviceAdded\n"));
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "XdrDriverAdded\n");


    //
    // Initialize attributes and a context area for the device object.
    //
    //
    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&fdoAttributes, FDO_DATA);

    //
    // Create a framework device object.This call will in turn create
    // a WDM device object, attach to the lower stack, and set the
    // appropriate flags and attributes.
    //
    status = WdfDeviceCreate(&DeviceInit, &fdoAttributes, &hDevice);
    if (!NT_SUCCESS(status)) {
        KdPrint(("WdfDeviceCreate failed with status code 0x%x\n", status));
        return status;
    }

    //
    // Get the device context by using the accessor function specified in
    // the WDF_DECLARE_CONTEXT_TYPE_WITH_NAME macro for FDO_DATA.
    //
    /*fdoData = XdrFdoGetData(hDevice);
    fdoData->TheThing = 17;
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "TheThing: %d\n", fdoData->TheThing);*/
    return status;
}

VOID XdrEvtDriverUnload(_In_ PDRIVER_OBJECT DriverObject)
{
    UNREFERENCED_PARAMETER(DriverObject);
    //PFDO_DATA              fdoData;
    //fdoData = XdrFdoGetData(DriverObject->DeviceObject);
    //fdoData->TheThing += 1;
    //DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "TheThing Unload: %d\n", fdoData->TheThing);

    // Unregister module load notification callback
    PsRemoveLoadImageNotifyRoutine(ModuleLoadCallback);

    IoDeleteSymbolicLink(&dos);
    IoDeleteDevice(DriverObject->DeviceObject);

    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "XdrEvtExit\n");
    //KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "KmdfHelloWorld: BYE BYE\n"));
}

