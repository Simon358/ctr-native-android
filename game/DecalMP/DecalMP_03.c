#include "DecalMP_Common.h"

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x80023784-0x80023a40.
void DecalMP_03(struct GameTracker *gGT)
{
	RECT viewport;
	viewport.w = 0x60;
	viewport.h = 0x40;

	int texY = (gGT->numPlyrCurrGame == 2) ? 0x180 : 0x100;
	int maxTexX = (gGT->numPlyrCurrGame > 2) ? 0x2c0 : 0x380;
	int texX = 0x1a0;

	for (int index = 0; index < 12; index++)
	{
		texX += 0x60;
		if (maxTexX < texX)
		{
			texX = 0x200;
			texY += 0x40;
		}

		struct DecalMPEntry *entry = DecalMP_GetEntry(gGT, index);
		if (entry->inst == NULL)
			return;

		int cameraID = entry->pb.cameraID;
		struct InstDrawPerPlayer *idpp = DecalMP_GetIdpp(entry->inst, cameraID);

		if ((idpp->instFlags & 0x140) != 0x140)
			continue;

		if (entry->boolUpdatedThisFrame != 0)
		{
			entry->timer = 0;
			entry->renderW = (s16)entry->pb.renderBucketScreenSize;
			entry->renderH = (s16)(entry->pb.renderBucketScreenSize >> 16);
			entry->lodIndex = idpp->lodIndex;

			viewport.x = (s16)texX;
			viewport.y = (s16)texY;

			PushBuffer_SetDrawEnv_DecalMP(entry->pb.renderBucketOTRangeEnd, gGT->backBuffer, &viewport, (s16)(texX - (s16)entry->pb.renderBucketScreenPos),
			                              (s16)(texY - (s16)(entry->pb.renderBucketScreenPos >> 16)), 0, 0, 0, 0, 1);
		}

		u32 *prim = gGT->backBuffer->primMem.curr;
		*(u8 *)((char *)prim + 7) = 0x2d;

		s16 x = entry->pb.rect.x;
		s16 y = entry->pb.rect.y;
		s16 w = entry->pb.rect.w;
		s16 h = entry->pb.rect.h;

		prim[2] = (u16)x | ((u32)(u16)y << 16);
		prim[4] = (u16)(x + w) | ((u32)(u16)y << 16);
		prim[6] = (u16)x | ((u32)(u16)(y + h) << 16);
		prim[8] = (u16)(x + w) | ((u32)(u16)(y + h) << 16);

		int u0 = texX & 0x3f;
		int v0 = texY & 0xff;
		int u1 = u0 + entry->renderW;
		int v1 = v0 + entry->renderH;
		if (v1 >= 0x100)
			v1 = 0xff;

		*(u16 *)&prim[3] = (u16)(u0 | (v0 << 8));
		*(u16 *)&prim[5] = (u16)(u1 | (v0 << 8));
		*(u16 *)&prim[7] = (u16)(u0 | (v1 << 8));
		*(u16 *)&prim[9] = (u16)(u1 | (v1 << 8));

		*(u16 *)((char *)prim + 0x16) = (u16)(((texY & 0x100) >> 4) | ((texX & 0x3ff) >> 6) | 0x100 | ((texY & 0x200) << 2));

		u_long *ot = gGT->pushBuffer[cameraID].ptrOT + (entry->pb.renderBucketOTByteOffset >> 2);
		prim[0] = *ot | 0x09000000;
		*ot = ((u32)prim)ADD_PSX_ADDRESS;
		gGT->backBuffer->primMem.curr = prim + 10;
	}
}
