#pragma once
#include <ntdef.h>
#include <ntddk.h>

#define MyTag 'abcd'

#define PROC_BUFFER_MAX_SIZE 2400

ULONG exampleNumber;
PDEVICE_OBJECT pDeviceObject;
UNICODE_STRING dev, dos;

typedef struct ProcHeader {
	// string is WCHAR
	SIZE_T image_size;
	SIZE_T string_length; //dont forget sizeof(WCHAR)
} PROCESS_HEADER, *PPROCESS_HEADER;

void *ProcBufferPtr;
SIZE_T ProcBufferLength; // size of buffer in bytes
SIZE_T ProcBufferOffset; // Current offset of pointer
SIZE_T ProcBufferBytesWritten; //TODO: is this necessary?

void DeleteBufferContents();

BOOLEAN buffer_full;