#include <common.h>

#if defined(CTR_NATIVE)
#ifndef CTR_NATIVE_MEMPACK_RETAIL_PRESSURE
#define CTR_NATIVE_MEMPACK_RETAIL_PRESSURE 1
#endif

#if CTR_NATIVE_MEMPACK_RETAIL_PRESSURE
// NOTE(aalhendi): Native pressure mode exposes the retail mempack window 0x800ba9f0-0x801ff800 inside a 2 MiB backing store.
#define CTR_NATIVE_MEMPACK_BUFFER_SIZE   0x200000u
#define CTR_NATIVE_MEMPACK_START_OFFSET  0xba9f0u
#define CTR_NATIVE_MEMPACK_RETAIL_SIZE   0x144e10u
#else
#define CTR_NATIVE_MEMPACK_BUFFER_SIZE   (32u * 1024u * 1024u)
#define CTR_NATIVE_MEMPACK_START_OFFSET  0u
#define CTR_NATIVE_MEMPACK_RETAIL_SIZE   CTR_NATIVE_MEMPACK_BUFFER_SIZE
#endif

char memory[CTR_NATIVE_MEMPACK_BUFFER_SIZE];
#endif

void CS_EndOfFile();
void RB_EndOfFile();
void AH_EndOfFile();
void MM_EndOfFile();
void OVR_Region3();

// NOTE(aalhendi): ASM-verified NTSC-U 926 PS1 path 0x8003e740-0x8003e80c; CTR_NATIVE uses host RAM.
void MEMPACK_Init(int ramSize)
{
	struct Mempack *ptrMempack;
	u32 startPtr;
	u32 maxOverlayEnd;
	int packSize;

#if defined(CTR_NATIVE)

	// must be a 24-bit address
	// Visual Studio -> Properties -> Linker -> Advanced ->
	// Base Address, Randomized Base Address, Fixed Base Address
	startPtr = (u32)&memory[CTR_NATIVE_MEMPACK_START_OFFSET];

	int boolValid = ((u32)&memory[0] < 0x01000000) && (startPtr < 0x01000000);

	printf("[CTR] Where does memory starts? (%s) %08x\n", boolValid ? "GOOD" : "BAD", (u32)&memory[0]);

	packSize = CTR_NATIVE_MEMPACK_RETAIL_SIZE;
	memset(memory, 0, sizeof(memory));

	MEMPACK_NewPack((void *)startPtr, packSize);
	sdata->PtrMempack->endOfMemory = &memory[CTR_NATIVE_MEMPACK_BUFFER_SIZE];

	printf("[CTR] MEMPACK native pressure: start=%08x size=%08x end=%08x\n",
	       startPtr,
	       packSize,
	       (u32)sdata->PtrMempack->endOfAllocator);

#else

	maxOverlayEnd = (u32)AH_EndOfFile;
	if (maxOverlayEnd < (u32)RB_EndOfFile)
		maxOverlayEnd = (u32)RB_EndOfFile;
	if (maxOverlayEnd < (u32)MM_EndOfFile)
		maxOverlayEnd = (u32)MM_EndOfFile;
	if (maxOverlayEnd < (u32)CS_EndOfFile)
		maxOverlayEnd = (u32)CS_EndOfFile;

	startPtr = (u32)OVR_Region3 + (((maxOverlayEnd - (u32)OVR_Region3) + 0x7ff) & ~0x7ff);
	packSize = ramSize - (int)(startPtr & 0xffffff) - 0x800;

	ptrMempack = sdata->PtrMempack;
	ptrMempack->start = (void *)startPtr;
	ptrMempack->endOfAllocator = (void *)(startPtr + packSize);
	ptrMempack->lastFreeByte = (void *)(startPtr + packSize);
	ptrMempack->packSize = packSize;
	ptrMempack->numBookmarks = 0;
	ptrMempack->endOfMemory = (void *)0x80200000;
	ptrMempack->firstFreeByte = (void *)startPtr;
#endif
}
