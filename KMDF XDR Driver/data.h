#pragma once
#include <ntdef.h>

#define MyTag 'abcd'

ULONG exampleNumber;
PDEVICE_OBJECT pDeviceObject;
UNICODE_STRING dev, dos;

typedef struct ProcData {
	SIZE_T image_size;
	UNICODE_STRING process_name; 
} PROCESS_DATA, *PPROCESS_DATA;

PPROCESS_DATA ProcBuffer;