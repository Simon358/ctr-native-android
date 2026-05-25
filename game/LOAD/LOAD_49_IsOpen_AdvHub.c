#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x80034920-0x80034940.
int LOAD_IsOpen_AdvHub()
{
	return sdata->gGT->overlayIndex_Threads == 2;
}
