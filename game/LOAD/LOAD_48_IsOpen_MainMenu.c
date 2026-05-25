#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x80034908-0x80034920.
int LOAD_IsOpen_MainMenu()
{
	return sdata->gGT->overlayIndex_Threads == 0;
}
