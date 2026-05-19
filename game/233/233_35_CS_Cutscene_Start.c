#include <common.h>

// for oxide intro and ND box
// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800b087c-0x800b0b38
void DECOMP_CS_Cutscene_Start(void)
{
	struct CsThreadInitData initData;

	struct GameTracker *gGT = sdata->gGT;

	DECOMP_CS_Thread_Init(0, OVR_233.s_introcam, 0, 0, 0);

	if ((gGT->gameMode2 & CREDITS) != 0)
	{
		OVR_233.isCutsceneOver = 0;

		DECOMP_CS_Credits_Init();

		DECOMP_CS_Instance_InitMatrix();
		return;
	}

	if (gGT->levelID != NAUGHTY_DOG_CRATE)
		return;

	DECOMP_CS_Instance_InitMatrix();

	initData.podiumPos[0] = 0;
	initData.podiumPos[1] = 0;
	initData.podiumPos[2] = 0;
	initData.characterPos[0] = 0;
	initData.characterPos[1] = 0;
	initData.characterPos[2] = 0;
	initData.rot[0] = 0;
	initData.rot[1] = 0;
	initData.rot[2] = 0;

	DECOMP_CS_Thread_Init(NDI_BOX_BOX_01, OVR_233.s_box1, (short *)&initData, 0, 0);
	DECOMP_CS_Thread_Init(NDI_BOX_BOX_02, OVR_233.s_box2, (short *)&initData, 0, 0);
	DECOMP_CS_Thread_Init(NDI_BOX_BOX_02_BOTTOM, OVR_233.s_box2_bottom, (short *)&initData, 0, 0);
	DECOMP_CS_Thread_Init(NDI_BOX_BOX_02_FRONT, OVR_233.s_box2_front, (short *)&initData, 0, 0);
	DECOMP_CS_Thread_Init(NDI_BOX_BOX_02A, OVR_233.s_box2_A, (short *)&initData, 0, 0);
	DECOMP_CS_Thread_Init(NDI_BOX_BOX_03, OVR_233.s_box3, (short *)&initData, 0, 0);
	DECOMP_CS_Thread_Init(NDI_BOX_CODE, OVR_233.s_code, (short *)&initData, 0, 0);
	DECOMP_CS_Thread_Init(NDI_BOX_GLOW, OVR_233.s_glow, (short *)&initData, 0, 0);
	DECOMP_CS_Thread_Init(NDI_BOX_LID, OVR_233.s_lid, (short *)&initData, 0, 0);
	DECOMP_CS_Thread_Init(NDI_BOX_LIDB, OVR_233.s_lidb, (short *)&initData, 0, 0);
	DECOMP_CS_Thread_Init(NDI_BOX_LIDC, OVR_233.s_lidc, (short *)&initData, 0, 0);
	DECOMP_CS_Thread_Init(NDI_BOX_LIDD, OVR_233.s_lidd, (short *)&initData, 0, 0);
	DECOMP_CS_Thread_Init(NDI_BOX_LID2, OVR_233.s_lid2, (short *)&initData, 0, 0);
	DECOMP_CS_Thread_Init(NDI_KART0, OVR_233.s_kart0, (short *)&initData, 0, 0);
	DECOMP_CS_Thread_Init(NDI_KART1, OVR_233.s_kart1, (short *)&initData, 0, 0);
	DECOMP_CS_Thread_Init(NDI_KART2, OVR_233.s_kart2, (short *)&initData, 0, 0);
	DECOMP_CS_Thread_Init(NDI_KART3, OVR_233.s_kart3, (short *)&initData, 0, 0);
	DECOMP_CS_Thread_Init(NDI_KART6, OVR_233.s_kart6, (short *)&initData, 0, 0);
	DECOMP_CS_Thread_Init(NDI_KART7, OVR_233.s_kart7, (short *)&initData, 0, 0);
}
