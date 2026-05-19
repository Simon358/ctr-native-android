#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800b0248-0x800b0300
void DECOMP_CS_Podium_Stand_Init(short *podiumData)
{
	struct Instance *inst = DECOMP_INSTANCE_BirthWithThread(STATIC_PODIUM, OVR_233.s_podium, SMALL, OTHER, DECOMP_CS_Podium_Stand_ThTick, 0, 0);

	// if the instance was built
	if (inst == NULL)
		return;

	// set funcThDestroy to remove instance from instance pool
	inst->thread->funcThDestroy = DECOMP_PROC_DestroyInstance;

	inst->matrix.t[0] = podiumData[0];
	inst->matrix.t[1] = podiumData[1];
	inst->matrix.t[2] = podiumData[2];

	inst->unk51 += 2;
	inst->unk50 += 2;

	podiumData[12] = podiumData[8];
	podiumData[13] = podiumData[9];
	podiumData[14] = podiumData[10];

	ConvertRotToMatrix(&inst->matrix, &podiumData[12]);
}
