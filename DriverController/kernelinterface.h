#pragma once

#include "communication.h"

#define PROC_BUFFER_MAX_SIZE 2400

class KernelInterface {
public:
	HANDLE DriverHandle;
	PVOID Buffer;
	ULONG ReturnLength;

	KernelInterface(LPCSTR RegistryPath) {
		DriverHandle = CreateFileA(RegistryPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
		Buffer = malloc(PROC_BUFFER_MAX_SIZE);
		ReturnLength = 0;
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

	void GetProcBuffer() {
		if (DriverHandle != INVALID_HANDLE_VALUE && DriverHandle != NULL) {
			if (!DeviceIoControl(DriverHandle, IO_RECEIVE_PROC_BUFFER, NULL, 0, Buffer, PROC_BUFFER_MAX_SIZE, &ReturnLength, 0)) {
				std::cout << "DeviceIoControl failed" << std::endl;
			}
			else {
				std::cout << "DeviceIoControl success. " << ReturnLength << " bytes written into buffer." << std::endl;
			}
		}
	}
};