#include "events.h"
#include "data.h"
#include "helper_funcs.h"

//// Callback function for module load notification
VOID ModuleLoadCallback(_In_opt_ PUNICODE_STRING FullImageName, _In_ HANDLE ProcessId, _In_ PIMAGE_INFO ImageInfo)
{
    UNREFERENCED_PARAMETER(ProcessId);

    if (FullImageName != NULL)
    {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Process name is %S  Size: %zu UnicodeStringMaxLength: %d\n", FullImageName->Buffer, ImageInfo->ImageSize, FullImageName->MaximumLength);
        //TODO: change fullimagename->buffer to image_info->base_address

        if (ProcBufferOffset < ProcBufferAmount) {
            ProcBuffer[ProcBufferOffset].image_size = ImageInfo->ImageSize;
            RtlCopyUnicodeString(&ProcBuffer[ProcBufferOffset].process_name, FullImageName);
            ProcBufferOffset += 1;
        }
        else {
            DebugMessage("Process data buffer is full, doing nothing");
        }
    }
}