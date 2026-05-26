#pragma once

#include <common.h>

struct DecalMPEntry
{
	s16 timer;
	u8 kartState;
	u8 pad3[3];
	s16 boolUpdatedThisFrame;
	struct Instance *inst;
	u8 padC[4];
	s16 renderW;
	s16 renderH;
	int lodIndex;
	struct PushBuffer pb;
};

_Static_assert(sizeof(struct DecalMPEntry) == 0x128);

static inline struct DecalMPEntry *DecalMP_GetEntry(struct GameTracker *gGT, int index)
{
	return (struct DecalMPEntry *)(void *)&gGT->DecalMP[index];
}

static inline struct InstDrawPerPlayer *DecalMP_GetIdpp(struct Instance *inst, int cameraID)
{
	return (struct InstDrawPerPlayer *)((char *)INST_GETIDPP(inst) + (cameraID * sizeof(struct InstDrawPerPlayer)));
}
