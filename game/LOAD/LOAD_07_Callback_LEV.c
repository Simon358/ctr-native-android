#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x80031a78-0x80031aa4.
void LOAD_Callback_LEV(struct LoadQueueSlot *lqs)
{
	if ((lqs->flags & LT_GETADDR) == 0)
		sdata->load_inProgress = 0;

	sdata->ptrLevelFile = (struct Level *)lqs->ptrDestination;
}
