#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800348e8-0x80034908.
int LOAD_IsOpen_RacingOrBattle()
{
	return sdata->gGT->overlayIndex_Threads == 1;
}
