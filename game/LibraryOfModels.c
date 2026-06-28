#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x8003147c-0x800314c0.
void LibraryOfModels_Store(struct GameTracker *gGT, u32 numModels, struct Model **ptrModelArray)
{
	while (numModels != 0)
	{
		struct Model *m = *ptrModelArray;
		if (m == NULL)
		{
			return;
		}
		if (m->id != -1)
		{
			gGT->modelPtr[m->id] = m;
		}
		numModels--;
		ptrModelArray++;
	}
}

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800314c0-0x800314e0.
void LibraryOfModels_Clear(struct GameTracker *gGT)
{
	int i;
	for (i = 0; i < 0xe2; i++)
	{
		gGT->modelPtr[i] = 0;
	}
}
