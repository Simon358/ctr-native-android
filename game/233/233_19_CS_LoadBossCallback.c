#include <common.h>

// ASM-verified: 0x800ae81c, 20 bytes, leaf function
void DECOMP_CS_LoadBossCallback(struct LoadQueueSlot *lqs)
{
	void *ptr = lqs->ptrDestination;
	sdata->load_inProgress = 0;
	OVR_233.ptrModelBossHead = ptr;
}
