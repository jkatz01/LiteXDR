#include "data.h"

void DeleteBufferContents() {
	 RtlZeroMemory(ProcBufferPtr, ProcBufferLength);
	 ProcBufferOffset = 0;
	 ProcBufferBytesWritten = 0;
	 buffer_full = FALSE;
}