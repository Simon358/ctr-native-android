#ifndef PLATFORM_NATIVE_STATE_H
#define PLATFORM_NATIVE_STATE_H

#include <macros.h>

int NativeState_GetSize(void);
int NativeState_Capture(void *dst, int dstSize);
int NativeState_Restore(const void *src, int srcSize);

#endif
