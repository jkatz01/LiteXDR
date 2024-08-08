#pragma once

#include <ntddk.h>

VOID ModuleLoadCallback(_In_opt_ PUNICODE_STRING FullImageName, _In_ HANDLE ProcessId, _In_ PIMAGE_INFO ImageInfo);