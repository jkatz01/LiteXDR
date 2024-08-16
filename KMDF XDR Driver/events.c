#include "events.h"
#include "data.h"
#include "helper_funcs.h"

void PrintProcBufferKernel()
{
    if (finished_test_print_buffer == FALSE) {
        SIZE_T i = 0;
        while (i < ProcBufferOffset) {
            SIZE_T h_image_size = ((PPROCESS_HEADER)((char*)ProcBufferPtr + i))->image_size;
            SIZE_T h_string_length = ((PPROCESS_HEADER)((char*)ProcBufferPtr + i))->string_length;
            DebugMessage("Image size: %zu, String size: %zu\n", h_image_size, h_string_length);
            i += sizeof(PROCESS_HEADER);
            PWCH string_buffer = ((PWCH)((char*)ProcBufferPtr + i));
            DebugMessage("Name: %S\n\n", string_buffer);
            i += h_string_length * sizeof(WCHAR);
        }
        finished_test_print_buffer = TRUE;
    }
    DebugMessage("Process data buffer is full, doing nothing");
}

//// Callback function for module load notification
VOID ModuleLoadCallback(_In_opt_ PUNICODE_STRING FullImageName, _In_ HANDLE ProcessId, _In_ PIMAGE_INFO ImageInfo)
{
    UNREFERENCED_PARAMETER(ProcessId);

    if (FullImageName != NULL)
    {
        // Add header to buffer
        // Add string data to buffer
        // Check they will fit inside buffer
        SIZE_T str_length_bytes = (SIZE_T)FullImageName->Length * sizeof(WCHAR);

        if (ProcBufferOffset + sizeof(PROCESS_HEADER) + str_length_bytes < ProcBufferLength) { //make sure there is space
            // Add header
            PROCESS_HEADER my_header = { ImageInfo->ImageSize, (SIZE_T)FullImageName->Length };
            *((PPROCESS_HEADER)((char*)ProcBufferPtr + ProcBufferOffset)) = my_header; //(char*) needed to do void pointer arithematic
            ProcBufferOffset += sizeof(PROCESS_HEADER);
            // Add data
            RtlCopyMemory(((char*)ProcBufferPtr + ProcBufferOffset), FullImageName->Buffer, (my_header.string_length * sizeof(WCHAR)) );
            ProcBufferOffset += (my_header.string_length * sizeof(WCHAR));
            ProcBufferBytesWritten = ProcBufferOffset;
        }
        else {
            ProcBufferBytesWritten = ProcBufferOffset;
            PrintProcBufferKernel();
        }
    }
}

// TODO: add function to reset buffer