#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x80031a08-0x80031a20.
void LOAD_Callback_Overlay_231(void)
{
	sdata->load_inProgress = 0;
	sdata->gGT->overlayIndex_Threads = 1;
}
