#pragma once

#include <ntifs.h>

#define DebugMessage(x, ...) DbgPrintEx(0, 0, x, __VA_ARGS__)

NTSTATUS DriverEntry(PDRIVER_OBJECT  DriverObject, PUNICODE_STRING RegistryPath);

NTSTATUS UnloadDriver(PDRIVER_OBJECT DriverObject);