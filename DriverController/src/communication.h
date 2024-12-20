#pragma once

#include <Windows.h>

#define IO_RECEIVE_HELLOWORLD CTL_CODE(FILE_DEVICE_UNKNOWN, 0x900, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define IO_RECEIVE_PROC_BUFFER CTL_CODE(FILE_DEVICE_UNKNOWN, 0x901, METHOD_BUFFERED, FILE_READ_DATA)

// Struct used in buffer
typedef struct ProcHeader {
	// string is WCHAR
	SIZE_T image_size;
	SIZE_T string_length;
} PROCESS_HEADER, * PPROCESS_HEADER;