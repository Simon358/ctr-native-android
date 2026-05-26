#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x80046a74-0x80046a90.
void RefreshCard_Unknown1(void)
{
	sdata->memcardUnk1 = (sdata->memcardUnk1 | 6) & ~8;
}
