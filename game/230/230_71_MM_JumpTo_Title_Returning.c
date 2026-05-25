#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800b4334-0x800b4364.
void MM_JumpTo_Title_Returning(void)
{
	// return to main menu from another menu
	D230.MM_State = 3;

	// return to main menu
	sdata->ptrDesiredMenu = &D230.menuMainMenu;

	D230.countMeta0xD = D230.title_numFrameTotal;
}
