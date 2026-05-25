#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800b64d4-0x800b64f4.
void MM_Video_ClearMem(void)
{
	MEMPACK_PopState();
}
