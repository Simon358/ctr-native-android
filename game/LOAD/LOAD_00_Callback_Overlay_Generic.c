#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800319e8-0x800319f4.
void LOAD_Callback_Overlay_Generic(struct LoadQueueSlot *lqs)
{
	(void)lqs;
	sdata->load_inProgress = 0;
}
