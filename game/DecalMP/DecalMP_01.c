#include "DecalMP_Common.h"

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x80023488-0x80023640.
void DecalMP_01(struct GameTracker *gGT)
{
	if (gGT->numPlyrCurrGame == 0)
		return;

	int entryIndex = 0;

	for (int cameraID = 0; cameraID < gGT->numPlyrCurrGame; cameraID++)
	{
		struct PushBuffer *pb = &gGT->pushBuffer[cameraID];

		for (int driverID = 0; driverID < 8; driverID++)
		{
			struct Driver *driver = gGT->drivers[driverID];
			if (driver == NULL)
				continue;

			struct Instance *inst = driver->instSelf;
			struct InstDrawPerPlayer *idpp = DecalMP_GetIdpp(inst, cameraID);
			idpp->instFlags |= 0x300;

			if (driverID == cameraID)
				continue;

			struct DecalMPEntry *entry = DecalMP_GetEntry(gGT, entryIndex++);

			if (driver->kartState == KS_BLASTED)
			{
				entry->kartState = KS_BLASTED;
			}
			else if (entry->kartState == KS_BLASTED)
			{
				entry->kartState = 0;
				entry->timer = 1000;
			}

			entry->pb.matrix_ViewProj = pb->matrix_ViewProj;
			entry->pb.pos[0] = pb->pos[0];
			entry->pb.pos[1] = pb->pos[1];
			entry->pb.pos[2] = pb->pos[2];
			entry->pb.distanceToScreen_PREV = pb->distanceToScreen_PREV;
			entry->pb.rect = pb->rect;
			entry->pb.ptrOT = pb->ptrOT;
			entry->pb.cameraID = pb->cameraID;

			idpp->pushBuffer = &entry->pb;
			entry->inst = inst;
		}
	}
}
