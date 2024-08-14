#include "events.h"
#include "data.h"
#include "helper_funcs.h"

//// Callback function for module load notification
VOID ModuleLoadCallback(_In_opt_ PUNICODE_STRING FullImageName, _In_ HANDLE ProcessId, _In_ PIMAGE_INFO ImageInfo)
{
    UNREFERENCED_PARAMETER(ProcessId);

    if (FullImageName != NULL)
    {
        //DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "Process name is %S  Size: %zu UnicodeStringMaxLength: %d\n", FullImageName->Buffer, ImageInfo->ImageSize, FullImageName->MaximumLength);
        //TODO: change fullimagename->buffer to image_info->base_address

        // Add header to buffer
        // Add string data to buffer
        // Check they will fit inside buffer

        // NOTE: ONLY ADDING HEADERS RIGHT NOW FOR TESTING
        if (ProcBufferOffset < ProcBufferLength) {
            PROCESS_HEADER my_header = { ImageInfo->ImageSize, (SIZE_T)FullImageName->Length };
            *((PPROCESS_HEADER)((char*)ProcBufferPtr + ProcBufferOffset)) = my_header; //(char*) needed to do void pointer arithematic
            ProcBufferOffset += sizeof(PROCESS_HEADER);
        }
        else {
            if (finished_test_print_buffer == FALSE) {
                PPROCESS_HEADER BufferPtr = (PPROCESS_HEADER)ProcBufferPtr;
                for (int i = 0; i < ProcBufferLength / sizeof(PROCESS_HEADER); i++) {
                    DebugMessage("Image size: %zu, String size: %zu", (BufferPtr + i)->image_size, (BufferPtr + i)->string_size);
                }
                finished_test_print_buffer = TRUE;
            }
            DebugMessage("Process data buffer is full, doing nothing");
        }
    }
}