#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x8003c508-0x8003c518.
void MainLoadVLC_Callback(struct LoadQueueSlot *param_1)
{
	// VLC is now loaded
	sdata->bool_IsLoaded_VlcTable = 1;

#ifdef CTR_NATIVE
	// NOTE(aalhendi): Native keeps the loaded VLC pointer in host-visible state.
	sdata->ptrVlcTable = param_1->ptrDestination;
#endif
}
