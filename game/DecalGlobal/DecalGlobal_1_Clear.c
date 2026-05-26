#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x80022b9c-0x80022bdc.
void DecalGlobal_Clear(struct GameTracker *gGT)
{
	memset(&gGT->ptrIcons, 0, sizeof(gGT->ptrIcons));
	memset(&gGT->iconGroup, 0, sizeof(gGT->iconGroup));
}
