#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x80031a20-0x80031a38.
void LOAD_Callback_Overlay_232(void)
{
	sdata->load_inProgress = 0;
	sdata->gGT->overlayIndex_Threads = 2;
}
