#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x80028410-0x8002843c.
void GhostTape_Destroy()
{
	if (sdata->ptrGhostTapePlaying != 0)
	{
		MEMPACK_ClearHighMem();
		sdata->ptrGhostTapePlaying = 0;
	}
}
