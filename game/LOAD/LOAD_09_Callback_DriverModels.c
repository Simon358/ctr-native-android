#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x80031b00-0x80031b14.
void LOAD_Callback_DriverModels(struct LoadQueueSlot *lqs)
{
	sdata->load_inProgress = 0;
	sdata->ptrMPK = (int)lqs->ptrDestination;
}
