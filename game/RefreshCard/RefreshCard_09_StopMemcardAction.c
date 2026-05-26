#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800471ac-0x800471c4.
void RefreshCard_StopMemcardAction(void)
{
	sdata->unk8008d964 = 1;
	sdata->mcStart = 2;
}
