#include "DecalMP_Common.h"

static inline u32 DecalMP_OtLink(void *ptr)
{
#ifdef CTR_NATIVE
	// NOTE(aalhendi): Native OT entries store host pointers; retail stores
	// masked 24-bit PSX links.
	return (u32)ptr;
#else
	return (u32)ptr & 0xffffff;
#endif
}

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x80023640-0x80023784.
void DecalMP_02(struct GameTracker *gGT)
{
	for (int index = 0; index < 12; index++)
	{
		struct DecalMPEntry *entry = DecalMP_GetEntry(gGT, index);
		if (entry->inst == NULL)
			return;

		int cameraID = entry->pb.cameraID;
		struct InstDrawPerPlayer *idpp = DecalMP_GetIdpp(entry->inst, cameraID);
		s16 timer = 1000;

		if ((idpp->instFlags & 0x140) == 0x140)
		{
			timer = entry->timer;
			int minFrames = entry->pb.renderBucketOTByteOffset >> 3;
			if (minFrames < 2)
				minFrames = 2;

			if ((timer < 1000) && (((timer <= minFrames) && (entry->lodIndex == idpp->lodIndex)) || (((gGT->timer ^ index) & 1) == 0)))
			{
				idpp->instFlags |= 0x80;
				entry->boolUpdatedThisFrame = 0;
			}
			else
			{
				entry->boolUpdatedThisFrame = 1;

				if ((entry->pb.ptrOT != NULL) && (entry->pb.renderBucketOTRangeEnd != NULL))
				{
					u_long *cameraOT = gGT->pushBuffer[cameraID].ptrOT;
					*entry->pb.ptrOT = cameraOT[0x3ff];
					cameraOT[0x3ff] = DecalMP_OtLink(entry->pb.renderBucketOTRangeEnd);
				}
			}

			timer++;
			if ((gGT->gameMode1 & PAUSE_ALL) == 0)
				entry->timer = timer;
		}
		else
		{
			entry->timer = timer;
		}
	}
}
