#include <common.h>

void CS_OVR233_InitData(void);

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x80031a38-0x80031a50.
void LOAD_Callback_Overlay_233(void)
{
	sdata->load_inProgress = 0;
	sdata->gGT->overlayIndex_Threads = 3;

#ifdef CTR_NATIVE
	// NOTE(aalhendi): native-only reset/init for source-owned overlay 233 data.
	CS_OVR233_InitData();
#endif
}
