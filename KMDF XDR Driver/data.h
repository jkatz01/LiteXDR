#pragma once
#include <ntdef.h>

#define MyTag 'abcd'

ULONG exampleNumber;
PDEVICE_OBJECT pDeviceObject;
UNICODE_STRING dev, dos;

typedef struct ProcHeader {
	SIZE_T image_size;
	SIZE_T string_size; 
} PROCESS_HEADER, *PPROCESS_HEADER;

void *ProcBufferPtr;
SIZE_T ProcBufferLength; // size of buffer in bytes
SIZE_T ProcBufferOffset; // Current offset of pointer

BOOLEAN finished_test_print_buffer;