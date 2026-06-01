#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x80031e00-0x80031ee4.
void *LOAD_DramFile(void *bigfilePtr, int subfileIndex, void *ptrDestination, int *sizePtr, int callbackOrFlags)
{
	struct LoadQueueSlot lqs;
	void *loadedFile;

	if (callbackOrFlags == -1)
	{
		loadedFile = LOAD_ReadFile_ex(bigfilePtr, LT_GETADDR, subfileIndex, ptrDestination, sizePtr, NULL);

		lqs.ptrBigfileCdPos_UNUSED = bigfilePtr;
		lqs.flags = 0;
		lqs.type_UNUSED = LT_DRAM;
		lqs.subfileIndex = subfileIndex;
		lqs.ptrDestination = loadedFile;
		lqs.size_UNUSED = *sizePtr;
		lqs.callbackFuncPtr = NULL;

		LOAD_DramFileCallback(&lqs);

		return loadedFile;
	}

	if (callbackOrFlags == -2)
	{
		loadedFile = LOAD_ReadFile_ex(bigfilePtr, LT_GETADDR, subfileIndex, NULL, sizePtr, LOAD_DramFileCallback);
		data.currSlot.ptrDestination = loadedFile;
		*(void **)ptrDestination = loadedFile;
		return loadedFile;
	}

	return LOAD_ReadFile_ex(bigfilePtr, LT_GETADDR, subfileIndex, ptrDestination, sizePtr, LOAD_DramFileCallback);
}
