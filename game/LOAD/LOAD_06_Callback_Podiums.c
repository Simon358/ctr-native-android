#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x80031a64-0x80031a78.
void LOAD_Callback_Podiums(struct LoadQueueSlot *lqs)
{
	sdata->load_inProgress = 0;
	data.podiumModel_podiumStands = (int)lqs->ptrDestination;
}
