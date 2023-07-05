#include <ntddk.h>
#include <wdf.h>

DRIVER_UNLOAD KmdfDriverUnload;
EVT_WDF_DRIVER_DEVICE_ADD KmdfHelloWorldEvtDeviceAdd;

VOID ModuleLoadCallback(
    _In_opt_ PUNICODE_STRING FullImageName,
    _In_ HANDLE ProcessId,
    _In_ PIMAGE_INFO ImageInfo
);

extern "C" {
    DRIVER_INITIALIZE DriverEntry;
    _Use_decl_annotations_
        NTSTATUS DriverEntry(
            _In_ PDRIVER_OBJECT     DriverObject,
            _In_ PUNICODE_STRING    RegistryPath
        )
    {
        // NTSTATUS variable to record success or failure
        NTSTATUS status = STATUS_SUCCESS;

        // Allocate the driver configuration object
        WDF_DRIVER_CONFIG config;

        // Print "Hello World" for DriverEntry
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "KmdfHelloWorld: DriverEntry    7777  \n");



        // Register module load notification callback

        status = PsSetLoadImageNotifyRoutineEx(ModuleLoadCallback, PS_IMAGE_NOTIFY_CONFLICTING_ARCHITECTURE);
        if (!NT_SUCCESS(status))
        {
            // Handle error
            return status;
        }


        // Initialize the driver configuration object to register the
        // entry point for the EvtDeviceAdd callback, KmdfHelloWorldEvtDeviceAdd
        WDF_DRIVER_CONFIG_INIT(&config,
            KmdfHelloWorldEvtDeviceAdd
        );

        // Finally, create the driver object
        status = WdfDriverCreate(DriverObject,
            RegistryPath,
            WDF_NO_OBJECT_ATTRIBUTES,
            &config,
            WDF_NO_HANDLE
        );

        DriverObject->DriverUnload = KmdfDriverUnload;
        return status;

    }
}

// Callback function for module load notification
VOID ModuleLoadCallback(
    _In_opt_ PUNICODE_STRING FullImageName,
    _In_ HANDLE ProcessId,
    _In_ PIMAGE_INFO ImageInfo
)
{
    UNREFERENCED_PARAMETER(ProcessId);
    UNREFERENCED_PARAMETER(ImageInfo);

    if (FullImageName != NULL)
    {
        // Access and process the module information
        // Extract the required information and perform any desired actions
        KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Process name is %S\n", FullImageName->Buffer));

    }
}

NTSTATUS KmdfHelloWorldEvtDeviceAdd(
    _In_    WDFDRIVER       Driver,
    _Inout_ PWDFDEVICE_INIT DeviceInit
)
{
    // We're not using the driver object,
    // so we need to mark it as unreferenced
    UNREFERENCED_PARAMETER(Driver);

    NTSTATUS status;

    // Allocate the device object
    WDFDEVICE hDevice;

    // Print "Hello World"
    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "KmdfHelloWorld: KmdfHelloWorldEvtDeviceAdd\n"));

    // Create the device object
    status = WdfDeviceCreate(&DeviceInit,
        WDF_NO_OBJECT_ATTRIBUTES,
        &hDevice
    );
    return status;
}




// Driver unload routine
VOID KmdfDriverUnload(_In_ PDRIVER_OBJECT DriverObject)
{
    UNREFERENCED_PARAMETER(DriverObject);

    // Unregister module load notification callback

    PsRemoveLoadImageNotifyRoutine(ModuleLoadCallback);


    //DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "KmdfHelloWorld: BYE BYE  \n");
    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "KmdfHelloWorld: BYE BYE\n"));


}
