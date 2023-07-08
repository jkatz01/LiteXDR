#include "driver.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (INIT, DriverEntry)
//#pragma alloc_text (PAGE, ToasterEvtDeviceAdd)
//#pragma alloc_text (PAGE, ToasterEvtIoRead)
//#pragma alloc_text (PAGE, ToasterEvtIoWrite)
//#pragma alloc_text (PAGE, ToasterEvtIoDeviceControl)
#endif

//// Callback function for module load notification
VOID ModuleLoadCallback(_In_opt_ PUNICODE_STRING FullImageName, _In_ HANDLE ProcessId, _In_ PIMAGE_INFO ImageInfo)
{
    UNREFERENCED_PARAMETER(ProcessId);

    if (FullImageName != NULL)
    {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Process name is %S  Size: %zu\n", FullImageName->Buffer, ImageInfo->ImageSize);
        //TODO: change fullimagename->buffer to image_info->base_address
    }
}

NTSTATUS
DriverEntry(
    IN PDRIVER_OBJECT  DriverObject,
    IN PUNICODE_STRING RegistryPath
)
{
    NTSTATUS            status = STATUS_SUCCESS;
    WDF_DRIVER_CONFIG   config;

    //KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "XdrDriverEntry\n"));
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "XdrDriverEntry\n");



    // Register module load notification callback

    status = PsSetLoadImageNotifyRoutineEx(ModuleLoadCallback, PS_IMAGE_NOTIFY_CONFLICTING_ARCHITECTURE);
    if (!NT_SUCCESS(status))
    {
        // Handle error
        return status;
    }

    WDF_DRIVER_CONFIG_INIT(
        &config,
        XdrEvtDeviceAdd
    );


    //
    // Create a framework driver object to represent our driver.
    //
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

    return status;
}


NTSTATUS
XdrEvtDeviceAdd(
    IN WDFDRIVER       Driver,
    IN PWDFDEVICE_INIT DeviceInit
)
{
    NTSTATUS               status = STATUS_SUCCESS;
    PFDO_DATA              fdoData;
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
    fdoData = XdrFdoGetData(hDevice);
    fdoData->TheThing = 17;
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "TheThing: %d\n", fdoData->TheThing);
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

    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "XdrEvtExit\n");
    //KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "KmdfHelloWorld: BYE BYE\n"));
}


//#include <ntddk.h>
//#include <wdf.h>
//#include "Driver.h"
//
//DRIVER_UNLOAD KmdfDriverUnload;
//EVT_WDF_DRIVER_DEVICE_ADD MyEvtDeviceAdd;
//
//#define MAX_PR_NAME_SIZE 128
//
//typedef struct _MY_DEVICE_CONTEXT
//{
//    INT Thing;
//} MY_DEVICE_CONTEXT, *PMY_DEVICE_CONTEXT;
//
//
///*
//typedef struct {
//    SINGLE_LIST_ENTRY SingleListEntry;
//    CHAR ProcessName[MAX_PR_NAME_SIZE];
//    SIZE_T ImageSize;
//}PROC_INFO_ENTRY, *P_PROC_INFO_ENTRY; //This creates a point that you can refer to as PXXX instead of XXX * i think???
//
//void PushProcessEntry(PSINGLE_LIST_ENTRY _ListHead, P_PROC_INFO_ENTRY Entry) {
//    KeWaitForSingleObject(PrMutex, Executive, KernelMode, FALSE, 0);
//    PushEntryList(_ListHead, &(Entry->SingleListEntry));
//    KeReleaseMutex(PrMutex, FALSE);
//}
//
//P_PROC_INFO_ENTRY PopProcessEntry(PSINGLE_LIST_ENTRY _ListHead) {
//    PSINGLE_LIST_ENTRY SingleListEntry;
//    SingleListEntry = PopEntryList(_ListHead);
//    return CONTAINING_RECORD(SingleListEntry, PROC_INFO_ENTRY, SingleListEntry);
//}
//*/
//
//// Callback function for module load notification
//VOID ModuleLoadCallback(_In_opt_ PUNICODE_STRING FullImageName, _In_ HANDLE ProcessId, _In_ PIMAGE_INFO ImageInfo)
//{
//    UNREFERENCED_PARAMETER(ProcessId);
//
//    if (FullImageName != NULL)
//    {
//        // Access and process the module information
//        // Extract the required information and perform any desired actions
//        KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Process name is %S  Size: %zu\n", FullImageName->Buffer, ImageInfo->ImageSize));
//
//        //Create node to add in linked list
//        //TODO: change fullimagename->buffer to image_info->base_address
//        /*
//        P_PROC_INFO_ENTRY ListNode = (P_PROC_INFO_ENTRY)ExAllocatePool2(POOL_FLAG_PAGED, sizeof(PROC_INFO_ENTRY), 1);
//        //RtlCopyMemory(ListNode->ProcessName, FullImageName->Buffer, (size_t)FullImageName->Length);
//        ListNode->ProcessName[0] = 'T';
//        ListNode->ProcessName[1] = 'E';
//        ListNode->ProcessName[2] = 'S';
//        ListNode->ProcessName[3] = 'T';
//        ListNode->ProcessName[4] = '\0';
//        ListNode->ImageSize = ImageInfo->ImageSize;
//
//        PushProcessEntry(&ListHead, ListNode);
//        */
//    }
//}
//
//
////VOID PopAllTemp() {
////    P_PROC_INFO_ENTRY entry;
////    while (true) {
////        if (ListHead.Next == NULL) {
////            return;
////        }
////        entry = PopProcessEntry(&ListHead);
////        ExFreePool2(entry, 1, NULL, 0); 
////    }
////}
//DRIVER_INITIALIZE DriverEntry;
//
//_Use_decl_annotations_ NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
//{
//    // NTSTATUS variable to record success or failure
//    NTSTATUS status = STATUS_SUCCESS;
//
//    // Allocate the driver configuration object
//    WDF_DRIVER_CONFIG config;
//
//    // Initialize mutex
//    //KeInitializeMutex(DriverData->ProcMutex, PASSIVE_LEVEL);
//
//    //// Initialize linked list
//    //ListHead.Next = NULL;
//
//    // Print "Hello World" for DriverEntry
//    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "KmdfHelloWorld: DriverEntry    7777  \n");
//
//    // Register module load notification callback
//
//    status = PsSetLoadImageNotifyRoutineEx(ModuleLoadCallback, PS_IMAGE_NOTIFY_CONFLICTING_ARCHITECTURE);
//    if (!NT_SUCCESS(status))
//    {
//        // Handle error
//        return status;
//    }
//
//
//    // Initialize the driver configuration object to register the
//    // entry point for the EvtDeviceAdd callback, KmdfHelloWorldEvtDeviceAdd
//    WDF_DRIVER_CONFIG_INIT(&config,
//        MyEvtDeviceAdd
//    );
//
//    // Finally, create the driver object
//    status = WdfDriverCreate(DriverObject,
//        RegistryPath,
//        WDF_NO_OBJECT_ATTRIBUTES,
//        &config,
//        WDF_NO_HANDLE
//    );
//    DriverObject->DriverUnload = KmdfDriverUnload;
//
//    return status;
//
//}
//
//NTSTATUS MyEvtDeviceAdd(_In_ WDFDRIVER Driver, _Inout_ PWDFDEVICE_INIT DeviceInit)
//{
//    // We're not using the driver object,
//    // so we need to mark it as unreferenced
//    UNREFERENCED_PARAMETER(Driver);
//
//    NTSTATUS               status = STATUS_SUCCESS;
//    WDF_IO_QUEUE_CONFIG    queueConfig;
//    WDF_OBJECT_ATTRIBUTES  fdoAttributes;
//    WDFDEVICE              hDevice;
//    WDFQUEUE               queue;
//
//    PAGED_CODE();
//
//    // Print "Hello World"
//    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "KmdfHelloWorld: KmdfHelloWorldEvtDeviceAdd\n"));
//
//    // Initialize attributes and a context area for the device object.
//    //
//    //
//    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&fdoAttributes, FDO_DATA);
//
//    // Create the device object
//    status = WdfDeviceCreate(&DeviceInit, WDF_NO_OBJECT_ATTRIBUTES, &hDevice);
//
//    return status;
//}
//
//// Driver unload routine
//VOID XdrDriverUnload(_In_ PDRIVER_OBJECT DriverObject)
//{
//    UNREFERENCED_PARAMETER(DriverObject);
//
//    // Unregister module load notification callback
//    PsRemoveLoadImageNotifyRoutine(ModuleLoadCallback);
//    // now we know the callback will not be used
//    // mutex lock to make sure all existing callbacks ended
//    // mutex release 
//    //KeWaitForSingleObject(PrMutex, Executive, KernelMode, FALSE, 0);
//    //KeReleaseMutex(PrMutex, FALSE);
//    //PopAllTemp();
//
//    //DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "KmdfHelloWorld: BYE BYE  \n");
//    KdPrintEx((DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "KmdfHelloWorld: BYE BYE\n"));
//}