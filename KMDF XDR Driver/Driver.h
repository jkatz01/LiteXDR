#pragma once

#include <ntifs.h>

#include "helper_funcs.h"

NTSTATUS DriverEntry(PDRIVER_OBJECT  DriverObject, PUNICODE_STRING RegistryPath);

NTSTATUS UnloadDriver(PDRIVER_OBJECT DriverObject);