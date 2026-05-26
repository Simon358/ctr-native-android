#include <common.h>

#ifdef REBUILD_PC
struct rData rdata =
{
	// NOTE(aalhendi): retail is pre-shaped; UI_DrawRaceClock patches digits only.
	.s_timeString_empty = "  :  :  ",

	.s_oxide = "oxide",
	.s_fake = "fake",
	.s_pen = "pen",
	.s_ntropy = "ntropy",
	.s_joe = "joe",
	.s_roo = "roo",
	.s_papu = "papu",
	.s_pinstripe = "pinstripe",
	.s_pura = "pura",
	.s_polar = "polar",
	.s_dingo = "dingo",
	.s_ngin = "ngin",
	.s_coco = "coco",
	.s_tiny = "tiny",
	.s_cortex = "cortex",
	.s_crash = "crash",
};
#endif
