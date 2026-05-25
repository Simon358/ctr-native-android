#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800ad980-0x800ad98c.
struct RectMenu *MM_AdvNewLoad_GetMenuPtr(void)
{
	// menu for new/load
	return &D230.menuAdventure;
}
