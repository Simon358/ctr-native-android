#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x80023d4c-0x80023ffc
void DISPLAY_Blur_Main(struct PushBuffer *pb, int strength)
{
	struct GameTracker *gGT = sdata->gGT;
	struct DB *backBuffer = gGT->backBuffer;
	u32 *prim = backBuffer->primMem.curr;
	u32 *nextPrim;
	s8 cameraID;
	u_long *ot;

	cameraID = *(s8 *)&pb->cameraID;

	if (strength < 1 || (((*(u8 *)&gGT->db[1 - gGT->swapchainIndex].unk_primMemRelated >> (cameraID & 0x1f)) & 1) == 0))
	{
		int x = pb->rect.x;
		int y = pb->rect.y;
		int w = pb->rect.w;
		int h = pb->rect.h;

		prim[8] = 0xe1000a00;
		prim[1] = 0xe1000a20;
		prim[2] = 0xe6000001;
		prim[9] = 0xe6000000;
		prim[4] = DISPLAY_Blur_PackS16Pair(x, y);
		prim[5] = DISPLAY_Blur_PackS16Pair(x + w, y);
		prim[6] = DISPLAY_Blur_PackS16Pair(x, y + h);
		prim[7] = DISPLAY_Blur_PackS16Pair(x + w, y + h);
		prim[3] = (strength < 0) ? 0x2affffff : 0x2a000000;

		ot = gGT->otSwapchainDB[gGT->swapchainIndex];
		prim[0] = (u32)*ot | 0x09000000;
		*ot = (u_long)DISPLAY_Blur_Ptr24(prim);
		nextPrim = prim + 10;
	}
	else
	{
		int wave = gGT->timer + cameraID;
		s16 *scratch = CTR_SCRATCHPAD_PTR(s16, 0);
		u32 oldTag;
		int blur;
		int insetX;
		int insetY;

		if ((cameraID & 1) != 0)
			wave = -wave;

		blur = MATH_Sin(wave * 100);
		if (blur < 0)
			blur = -blur;

		blur = (blur >> 2) + 0x400;
		if (strength < 0x1000)
			blur = (blur * strength) >> 12;

		ot = gGT->otSwapchainDB[gGT->swapchainIndex];
		oldTag = *ot;
		*ot = (u_long)DISPLAY_Blur_Ptr24(prim);

		scratch[4] = pb->rect.x;
		scratch[5] = pb->rect.y;
		scratch[6] = pb->rect.w;
		scratch[7] = pb->rect.h;

		insetX = ((blur * 9) >> 12) + 2;
		scratch[0] = backBuffer->dispEnv.disp.x + pb->rect.x + insetX;
		scratch[2] = pb->rect.w - (insetX * 2);

		insetY = ((blur * 6) >> 12) + 2;
		scratch[1] = backBuffer->dispEnv.disp.y + pb->rect.y + insetY;
		scratch[3] = pb->rect.h - (insetY * 2);

		nextPrim = DISPLAY_Blur_SubFunc(prim, scratch);
		nextPrim[-10] = oldTag | 0x09000000;
	}

	backBuffer->primMem.curr = nextPrim;
	*(u8 *)&backBuffer->unk_primMemRelated |= (u8)(1 << (cameraID & 0x1f));
}
