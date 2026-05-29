#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x80047da8-0x80047dfc.
void SelectProfile_QueueLoadHub_MenuProc(struct RectMenu *menu)
{
	struct GameTracker *gGT = sdata->gGT;

	// NOTE(aalhendi): Retail stores 0x27 before LOAD_LevelFile records prevLEV.
	gGT->levelID = MAIN_MENU_LEVEL;

	data.characterIDs[0] = sdata->advProgress.characterID;
	MainRaceTrack_RequestLoad(gGT->currLEV);
	RECTMENU_Hide(menu);
	return;
}
