#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x80039e98-0x80039fa8.
void MainFreeze_IfPressStart(void)
{
	struct GameTracker *gGT = sdata->gGT;
	u32 gameMode1;
	struct RectMenu *menu;

	if (RaceFlag_IsFullyOnScreen() != 0)
		return;

	if ((gGT->renderFlags & 0x1000) != 0)
		return;

	if (sdata->AkuAkuHintState != 0)
		return;

	if (sdata->ptrActiveMenu != NULL)
		return;

	gameMode1 = gGT->gameMode1;

	if ((gameMode1 & (END_OF_RACE | PAUSE_ALL)) != 0)
		return;

	if (gGT->levelID == MAIN_MENU_LEVEL)
		return;

	if ((gameMode1 & GAME_CUTSCENE) != 0)
		return;

	if (gGT->boolDemoMode != 0)
		return;

	if ((u32)(gGT->levelID - OXIDE_ENDING) < 2)
		return;

	if (sdata->load_inProgress != 0)
		return;

	if ((gGT->gameMode2 & VEH_FREEZE_PODIUM) != 0)
		return;

	gGT->gameMode1 = gameMode1 | PAUSE_1;

	menu = MainFreeze_GetMenuPtr();
	menu->rowSelected = 0;

	RECTMENU_Show(menu);
	MainFrame_TogglePauseAudio(1);
	OtherFX_Play(1, 1);
	ElimBG_Activate(gGT);
}
