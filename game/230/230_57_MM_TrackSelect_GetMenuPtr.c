#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800b0eac-0x800b0eb8.
struct RectMenu *MM_TrackSelect_GetMenuPtr(void)
{
	return &D230.menuTrackSelect;
}
