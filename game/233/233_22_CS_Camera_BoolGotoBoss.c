#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800aed48-0x800aedf8
u_char DECOMP_CS_Camera_BoolGotoBoss(void)
{
	struct GameTracker *gGT = sdata->gGT;

	// If just got 18th relic
	if ((gGT->podiumRewardID == STATIC_RELIC) && (gGT->currAdvProfile.numRelics >= 18))
	{
		// If Oxide was not beaten twice yet
		if ((sdata->advProgress.rewards[3] & 0x100000) == 0)
			return 1;
	}

	// If just unlocked Key
	if (gGT->podiumRewardID == STATIC_KEY)
		return 1;

	struct Instance *inst = gGT->drivers[0]->instSelf;
	short *posCoords = gGT->level1->ptrSpawnType2_PosRot[1].posCoords;

	// TRUE if TeleportSelf did NOT spawn on podium (goto boss door)
	return (inst->matrix.t[0] != posCoords[0]) || (inst->matrix.t[2] != posCoords[2]);
}
