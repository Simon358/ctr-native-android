#include <common.h>

struct CSInterpolateLinePacket
{
	u32 tag;
	u32 drawMode;
	u32 pad;
	u32 colorAndCode;
	u32 xy0;
	u32 xy1;
};

_Static_assert(sizeof(struct CSInterpolateLinePacket) == 0x18);
_Static_assert(offsetof(struct CSInterpolateLinePacket, tag) == 0x00);
_Static_assert(offsetof(struct CSInterpolateLinePacket, drawMode) == 0x04);
_Static_assert(offsetof(struct CSInterpolateLinePacket, pad) == 0x08);
_Static_assert(offsetof(struct CSInterpolateLinePacket, colorAndCode) == 0x0C);
_Static_assert(offsetof(struct CSInterpolateLinePacket, xy0) == 0x10);
_Static_assert(offsetof(struct CSInterpolateLinePacket, xy1) == 0x14);

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800ae318-0x800ae54c
void CS_Thread_InterpolateFramesMS(struct Thread *t)
{
	struct GameTracker *gGT = sdata->gGT;
	struct Instance *inst = t->inst;
	struct PrimMem *primMem;
	struct CSInterpolateLinePacket *packet;
	void *end;
	u16 curr[3];
	u16 next[3];
	int depth;

	CS_Instance_GetFrameData(inst, inst->animIndex, inst->animFrame, curr, NULL, 0);
	CS_Instance_GetFrameData(inst, inst->animIndex, inst->animFrame, next, NULL, 1);

	curr[0] = (u16)(curr[0] + (u16)inst->matrix.t[0]);
	curr[1] = (u16)(curr[1] + (u16)inst->matrix.t[1]);
	curr[2] = (u16)(curr[2] + (u16)inst->matrix.t[2]);

	next[0] = (u16)(next[0] + (u16)inst->matrix.t[0]);
	next[1] = (u16)(next[1] + (u16)inst->matrix.t[1]);
	next[2] = (u16)(next[2] + (u16)inst->matrix.t[2]);

	primMem = &gGT->backBuffer->primMem;
	packet = primMem->cursor;
	end = primMem->guardEnd;

	if ((uintptr_t)(packet + 1) >= (uintptr_t)end)
		return;

	gte_SetRotMatrix(&gGT->pushBuffer[0].matrix_ViewProj);
	gte_SetTransMatrix(&gGT->pushBuffer[0].matrix_ViewProj);

	MTC2((u32)curr[0] | ((u32)curr[1] << 16), 0);
	MTC2((u32)curr[2], 1);
	MTC2((u32)next[0] | ((u32)next[1] << 16), 2);
	MTC2((u32)next[2], 3);
	gte_rtpt();

	packet->xy0 = MFC2(12);
	packet->xy1 = MFC2(13);

	depth = MFC2(17);
	if ((u32)(depth - 1) < 0x11ff)
	{
		u32 color = 0x3f;
		int otIndex;
		u32 *ot;

		packet->drawMode = 0xe1000a20;
		packet->pad = 0;

		if (depth > 0xa00)
		{
			int fade = (0x1200 - depth) * 0x3f;

			color = fade >> 11;
			if (fade < 0)
				color = (fade + 0x7ff) >> 11;
		}

		packet->colorAndCode = color | (color << 8) | (color << 16) | 0x42000000;

		otIndex = depth >> 6;
		if (otIndex > 0x3ff)
			otIndex = 0x3ff;

		ot = (u32 *)&gGT->pushBuffer[0].ptrOT[otIndex];
		packet->tag = (*ot & 0xffffff) | 0x05000000;
		*ot = CtrGpu_PrimToOTLink24(packet);
		packet++;
	}

	primMem->cursor = packet;
}
