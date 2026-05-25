#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800b1830-0x800b1848.
void MM_Battle_Init(void)
{
	D230.battle_transitionFrames = 0xc;
	D230.battle_transitionState = 0;
}
