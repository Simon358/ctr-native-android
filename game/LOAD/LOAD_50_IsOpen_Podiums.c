#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x80034940-0x80034960.
int LOAD_IsOpen_Podiums()
{
	return sdata->gGT->overlayIndex_Threads == 3;
}
