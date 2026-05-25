#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800319f4-0x80031a08.
void LOAD_Callback_Overlay_230(void)
{
	sdata->load_inProgress = 0;
	sdata->gGT->overlayIndex_Threads = 0;
}
