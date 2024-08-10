#pragma once

#include "communication.h"

class KernelInterface {
public:
	HANDLE hDriver;

	KernelInterface(LPCSTR RegistryPath) {
		hDriver = CreateFileA(RegistryPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
	}

	DWORD GetExampleNumber() {
		if (hDriver == INVALID_HANDLE_VALUE) {
			return 0;
		}

		ULONG exampleNumber = 0; //The buffer to read
		DWORD Bytes;

		if (DeviceIoControl(hDriver, IO_GET_HELLOWORLD, &exampleNumber, sizeof(exampleNumber), &exampleNumber, sizeof(exampleNumber), &Bytes, NULL)) {
			return exampleNumber;
		}
		return 0;
	}
};