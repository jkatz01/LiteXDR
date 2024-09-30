#pragma once

#include "communication.h"

#define PROC_BUFFER_MAX_SIZE 2400

class KernelInterface {
public:
	HANDLE	DriverHandle;
	PVOID	Buffer;
	ULONG	ReturnLength;

	KernelInterface(LPCSTR RegistryPath) {
		DriverHandle = CreateFileA(RegistryPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
		Buffer = malloc(PROC_BUFFER_MAX_SIZE);
		ReturnLength = 0;
	}

	void PrintProcBufferKernel()
	{
		SIZE_T i = 0;
		while (i < ReturnLength) {
			SIZE_T h_image_size = ((PPROCESS_HEADER)((char*)Buffer + i))->image_size;
			SIZE_T h_string_length = ((PPROCESS_HEADER)((char*)Buffer + i))->string_length;
			std::cout << "Image size: " << h_image_size << " String size: " << h_string_length << std::endl;
			i += sizeof(PROCESS_HEADER);
			PWCH string_buffer = ((PWCH)((char*)Buffer + i));
			printf("Name: %S\n", string_buffer);
			i += h_string_length * sizeof(WCHAR);
		}
	}

	// Convert process list to easier data format

	void GetProcBuffer() {
		if (DriverHandle != INVALID_HANDLE_VALUE && DriverHandle != NULL) {
			BOOL IoSuccess = DeviceIoControl(DriverHandle, IO_RECEIVE_PROC_BUFFER, NULL, 0, Buffer, PROC_BUFFER_MAX_SIZE, &ReturnLength, 0);
			if (!IoSuccess) {
				std::cout << "DeviceIoControl failed" << std::endl;
			}
			else {
				std::cout << "DeviceIoControl success. " << ReturnLength << " bytes written into buffer." << std::endl;
			}
		}
	}

	ULONG GetExampleNumber() {
		if (DriverHandle == INVALID_HANDLE_VALUE) {
			return 0;
		}

		ULONG exampleNumber = 0; //The buffer to read
		DWORD Bytes;

		if (DeviceIoControl(DriverHandle, IO_RECEIVE_HELLOWORLD, &exampleNumber, sizeof(exampleNumber), &exampleNumber, sizeof(exampleNumber), &Bytes, NULL)) {
			return exampleNumber;
		}
		return 0;
	}
};