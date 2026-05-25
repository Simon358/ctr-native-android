#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800b164c-0x800b1660.
void MM_Battle_CloseSubMenu(struct RectMenu *menu)
{
	menu->state |= 4;
}
