#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x80031a50-0x80031a64.
void LOAD_Callback_MaskHints3D(struct LoadQueueSlot *lqs)
{
	sdata->load_inProgress = 0;
	sdata->modelMaskHints3D = (struct Model *)lqs->ptrDestination;
}
