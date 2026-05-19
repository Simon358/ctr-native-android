#include <common.h>

struct RenderBucketEntry
{
	struct Instance *inst;
	struct Instance *instPlayerBase;
};

typedef struct
{
	u_char x;
	u_char y;
	u_char z;
} RenderBucketCompVertex;

typedef struct
{
	u_char x;
	u_char y;
	u_char z;
	u_char w;
} RenderBucketVertex;

static int RenderBucket_GetSignedBits(unsigned int *vertData, int *bitIndex, int bits)
{
	int const b = *bitIndex >> 5;
	int const e = 32 - bits;
	int const s = e - (*bitIndex & 31);
	int const ret = s < 0 ? (vertData[b] << -s) | (vertData[b + 1] >> (s & 31)) : vertData[b] >> s;

	*bitIndex += bits;
	return (ret << e) >> e;
}

static struct ModelAnim *RenderBucket_GetAnim(struct Instance *inst, struct ModelHeader *mh)
{
	if (mh->ptrAnimations == 0)
		return 0;

	if (mh->numAnimations == 0)
		return 0;

	if (inst->animIndex >= mh->numAnimations)
		return 0;

	return mh->ptrAnimations[inst->animIndex];
}

static struct ModelFrame *RenderBucket_GetFrame(struct Instance *inst, struct ModelHeader *mh, struct ModelAnim **animOut)
{
	struct ModelAnim *anim;
	int frameIndex;
	char *firstFrame;

	*animOut = 0;

	if (mh->ptrFrameData != 0)
		return mh->ptrFrameData;

	anim = RenderBucket_GetAnim(inst, mh);
	if (anim == 0)
		return 0;

	if (anim->numFrames == 0)
		return 0;

	frameIndex = inst->animFrame;
	if (frameIndex < 0)
		frameIndex = 0;
	frameIndex %= anim->numFrames;

	firstFrame = (char *)MODELANIM_GETFRAME(anim);
	*animOut = anim;
	return (struct ModelFrame *)(firstFrame + (anim->frameSize * frameIndex));
}

static struct RenderBucketEntry *RenderBucket_QueueDraw(struct Instance *inst, struct RenderBucketEntry *rbi, int playerIndex, unsigned int lodMask,
                                                        int gameMode1)
{
	struct ModelHeader *mh;
	struct ModelAnim *anim;
	struct ModelFrame *frame;
	struct InstDrawPerPlayer *idpp;
	struct Instance *instPlayerBase;

	// NOTE(aalhendi): PSX-backfeed blocker: retail QueueDraw consumes implicit
	// scratchpad/register state from QueueLev/QueueNonLev. This native helper
	// spells that state out as C parameters until the full ASM audit is done.
	(void)gameMode1;

	if (inst == 0)
		return rbi;

	if (inst->model == 0)
		return rbi;

	if (inst->model->headers == 0)
		return rbi;

	if ((inst->flags & lodMask) == 0)
		return rbi;

	instPlayerBase = (struct Instance *)((char *)inst + (playerIndex * sizeof(struct InstDrawPerPlayer)));
	idpp = INST_GETIDPP(instPlayerBase);

	if (idpp->pushBuffer == 0)
		return rbi;

	mh = &inst->model->headers[0];
	frame = RenderBucket_GetFrame(inst, mh, &anim);
	if (frame == 0)
		return rbi;

	idpp->instFlags = inst->flags | 0x40;
	idpp->unkbc = inst->alphaScale;
	idpp->ptrCurrFrame = frame;
	idpp->ptrNextFrame = 0;
	idpp->ptrCommandList = mh->ptrCommandList;
	idpp->ptrTexLayout = mh->ptrTexLayout;
	idpp->ptrColorLayout = (unsigned int)mh->ptrColors;
	idpp->ptrDeltaArray = (anim != 0) ? (int)anim->ptrDeltaArray : 0;
	idpp->lodIndex = 0;
	idpp->mh = mh;
	idpp->unkE4 = 0;
	idpp->unkE8 = 0;
	idpp->unkEC = 0;
	idpp->unkF0 = 0;

	rbi->inst = inst;
	rbi->instPlayerBase = instPlayerBase;
	return rbi + 1;
}

void *RenderBucket_QueueLevInstances(struct CameraDC *cDC, u_long *otMem, void *rbi, char *lod, char numPlyr, int gameMode1)
{
	struct RenderBucketEntry *entry = (struct RenderBucketEntry *)rbi;
	unsigned int lodMask = (unsigned int)(unsigned char)(unsigned int)lod;
	int count = (int)(unsigned char)numPlyr;

	// NOTE(aalhendi): PSX-backfeed blocker: native C context replaces the
	// retail scratchpad register-save frame at 0x1f800000.
	(void)otMem;

	for (int player = count - 1; player >= 0; player--)
	{
		struct Instance **visInstSrc = cDC[player].visInstSrc;

		if (visInstSrc == 0)
			continue;

		for (; *visInstSrc != 0; visInstSrc++)
		{
			entry = RenderBucket_QueueDraw(*visInstSrc, entry, player, lodMask, gameMode1);
		}
	}

	return entry;
}

void *RenderBucket_QueueNonLevInstances(struct Item *item, u_long *otMem, void *rbi, char *lod, char numPlyr, int gameMode1)
{
	struct RenderBucketEntry *entry = (struct RenderBucketEntry *)rbi;
	unsigned int lodMask = (unsigned int)(unsigned char)(unsigned int)lod;
	int count = (int)(unsigned char)numPlyr;

	// NOTE(aalhendi): PSX-backfeed blocker: native C context replaces the
	// retail scratchpad register-save frame at 0x1f800000.
	(void)otMem;

	for (int player = count - 1; player >= 0; player--)
	{
		for (struct Item *curr = item; curr != 0; curr = curr->next)
		{
			entry = RenderBucket_QueueDraw((struct Instance *)curr, entry, player, lodMask, gameMode1);
		}
	}

	return entry;
}

static void RenderBucket_DrawEntry(struct Instance *inst, struct Instance *instPlayerBase, struct PrimMem *primMem)
{
	short posWorld1[4];
	short posWorld2[4];
	short posWorld3[4];
	struct InstDrawPerPlayer *idpp;
	struct PushBuffer *pb;
	struct ModelHeader *mh;
	struct ModelFrame *mf;
	struct ModelAnim *anim;
	char *vertData;
	u_int *pCmd;
	RenderBucketVertex tempCoords[4] = {0};
	int tempColor[4] = {0};
	RenderBucketVertex stack[256] = {0};
	int scale[3];
	MATRIX mvp;
	VECTOR pos;
	int bitIndex = 0;
	int x_alu = 0;
	int y_alu = 0;
	int z_alu = 0;
	int stripLength = 0;
	int vertexIndex = 0;

	// NOTE(aalhendi): Native C renderer for the RenderBucket execution bridge.
	// It replaces the TEST_DrawInstances call path, but the retail
	// DrawFunc/DrawInstPrim split still needs a symbol-by-symbol ASM audit.
	if (inst == 0)
		return;

	if (inst->model == 0)
		return;

	idpp = INST_GETIDPP(instPlayerBase);
	pb = idpp->pushBuffer;
	if (pb == 0)
		return;

	if ((idpp->instFlags & 0x40) == 0)
		return;

	if ((idpp->instFlags & 0x80) != 0)
		return;

	mh = idpp->mh;
	if (mh == 0)
		return;

	if ((mh->ptrCommandList == 0) || (mh->ptrColors == 0))
		return;

	mf = idpp->ptrCurrFrame;
	if (mf == 0)
		return;

	anim = RenderBucket_GetAnim(inst, mh);
	vertData = MODELFRAME_GETVERT(mf);

	scale[0] = (mh->scale[0] * inst->scale[0]) >> 12;
	scale[1] = (mh->scale[1] * inst->scale[1]) >> 12;
	scale[2] = (mh->scale[2] * inst->scale[2]) >> 12;

#define RB_MVP(row, col, index) ((pb->matrix_ViewProj.m[row][index] * ((inst->matrix.m[index][col] * scale[col]) >> 8)) >> 0x10)

	mvp.m[0][0] = RB_MVP(0, 0, 0) + RB_MVP(0, 0, 1) + RB_MVP(0, 0, 2);
	mvp.m[0][1] = RB_MVP(0, 1, 0) + RB_MVP(0, 1, 1) + RB_MVP(0, 1, 2);
	mvp.m[0][2] = RB_MVP(0, 2, 0) + RB_MVP(0, 2, 1) + RB_MVP(0, 2, 2);
	mvp.m[1][0] = RB_MVP(1, 0, 0) + RB_MVP(1, 0, 1) + RB_MVP(1, 0, 2);
	mvp.m[1][1] = RB_MVP(1, 1, 0) + RB_MVP(1, 1, 1) + RB_MVP(1, 1, 2);
	mvp.m[1][2] = RB_MVP(1, 2, 0) + RB_MVP(1, 2, 1) + RB_MVP(1, 2, 2);
	mvp.m[2][0] = RB_MVP(2, 0, 0) + RB_MVP(2, 0, 1) + RB_MVP(2, 0, 2);
	mvp.m[2][1] = RB_MVP(2, 1, 0) + RB_MVP(2, 1, 1) + RB_MVP(2, 1, 2);
	mvp.m[2][2] = RB_MVP(2, 2, 0) + RB_MVP(2, 2, 1) + RB_MVP(2, 2, 2);

#undef RB_MVP

	pos.vx = inst->matrix.t[0] - pb->matrix_Camera.t[0];
	pos.vy = inst->matrix.t[1] - pb->matrix_Camera.t[1];
	pos.vz = inst->matrix.t[2] - pb->matrix_Camera.t[2];

	ApplyMatrixLV(&pb->matrix_ViewProj, &pos, &mvp.t[0]);
	gte_SetRotMatrix(&mvp);
	gte_SetTransMatrix(&mvp);
	gte_SetGeomOffset(pb->rect.w >> 1, pb->rect.h >> 1);
	gte_SetGeomScreen(pb->distanceToScreen_PREV);

	idpp->mvp = mvp;

	if ((inst->flags & 0x400) != 0)
		return;

	pCmd = (u_int *)mh->ptrCommandList;
	pCmd++;

	for (; *pCmd != 0xffffffff; pCmd++, stripLength++)
	{
		u_short flags = (*pCmd >> 24) & 0xff;
		u_short stackIndex = (*pCmd >> 16) & 0xff;
		u_short colorIndex = (*pCmd >> 9) & 0x7f;
		u_short texIndex = *pCmd & 0x1ff;

		if ((flags & 4) == 0)
		{
			if ((anim != 0) && (anim->ptrDeltaArray != 0))
			{
				u_int temporal = anim->ptrDeltaArray[vertexIndex];
				u_char xBits = (temporal >> 6) & 7;
				u_char yBits = (temporal >> 3) & 7;
				u_char zBits = temporal & 7;
				u_char bx = (temporal >> 0x19) << 1;
				u_char by = (temporal << 7) >> 0x18;
				u_char bz = (temporal << 0xf) >> 0x18;

				if (xBits == 7)
					x_alu = 0;
				if (yBits == 7)
					y_alu = 0;
				if (zBits == 7)
					z_alu = 0;

				x_alu += RenderBucket_GetSignedBits((unsigned int *)vertData, &bitIndex, xBits + 1) + bx;
				y_alu += RenderBucket_GetSignedBits((unsigned int *)vertData, &bitIndex, yBits + 1) + by;
				z_alu += RenderBucket_GetSignedBits((unsigned int *)vertData, &bitIndex, zBits + 1) + bz;

				stack[stackIndex].x = x_alu;
				stack[stackIndex].y = z_alu;
				stack[stackIndex].z = y_alu;
			}
			else
			{
				RenderBucketCompVertex *ptrVerts = (RenderBucketCompVertex *)vertData;

				stack[stackIndex].x = ptrVerts[vertexIndex].x;
				stack[stackIndex].y = ptrVerts[vertexIndex].y;
				stack[stackIndex].z = ptrVerts[vertexIndex].z;
			}

			vertexIndex++;
		}

		tempCoords[0] = tempCoords[1];
		tempCoords[1] = tempCoords[2];
		tempCoords[2] = tempCoords[3];
		tempCoords[3] = stack[stackIndex];

		tempColor[0] = tempColor[1];
		tempColor[1] = tempColor[2];
		tempColor[2] = tempColor[3];
		tempColor[3] = mh->ptrColors[colorIndex];

		if ((flags & 0x40) != 0)
		{
			tempCoords[1] = tempCoords[0];
			tempColor[1] = tempColor[0];
		}

		if ((flags & 0x80) != 0)
			stripLength = 0;

		if (stripLength < 2)
			continue;

		posWorld1[0] = mf->pos[0] + tempCoords[1].x;
		posWorld1[1] = mf->pos[1] + tempCoords[1].z;
		posWorld1[2] = mf->pos[2] + tempCoords[1].y;
		posWorld1[3] = 0;
		gte_ldv0(&posWorld1[0]);

		posWorld2[0] = mf->pos[0] + tempCoords[2].x;
		posWorld2[1] = mf->pos[1] + tempCoords[2].z;
		posWorld2[2] = mf->pos[2] + tempCoords[2].y;
		posWorld2[3] = 0;
		gte_ldv1(&posWorld2[0]);

		posWorld3[0] = mf->pos[0] + tempCoords[3].x;
		posWorld3[1] = mf->pos[1] + tempCoords[3].z;
		posWorld3[2] = mf->pos[2] + tempCoords[3].y;
		posWorld3[3] = 0;
		gte_ldv2(&posWorld3[0]);

		gte_rtpt();

		int boolPassCull = ((flags & 0x10) == 0);
		if (!boolPassCull)
		{
			int opZ;

			gte_nclip();
			gte_stopz(&opZ);
			boolPassCull = (opZ >= 0);

			if ((flags & 0x20) != 0)
				boolPassCull = !boolPassCull;

			if ((inst->flags & REVERSE_CULL_DIRECTION) != 0)
				boolPassCull = !boolPassCull;
		}

		if (!boolPassCull)
			continue;

		int otZ;
		gte_avsz3();
		gte_stotz(&otZ);

		if (otZ <= 32)
			continue;

		otZ -= 32;
		if (otZ >= 4080)
			continue;

		if ((char *)primMem->curr + sizeof(POLY_GT3) >= (char *)primMem->endMin100)
			return;

		if (texIndex == 0)
		{
			POLY_G3 *p = primMem->curr;

			*(int *)&p->r0 = tempColor[1];
			*(int *)&p->r1 = tempColor[2];
			*(int *)&p->r2 = tempColor[3];
			setPolyG3(p);
			gte_stsxy3(&p->x0, &p->x1, &p->x2);
			AddPrim((u_long *)pb->ptrOT + (otZ >> 2), p);
			primMem->curr = p + 1;
		}
		else
		{
			struct TextureLayout *tex;
			POLY_GT3 *p;

			if (mh->ptrTexLayout == 0)
				continue;

			tex = mh->ptrTexLayout[texIndex - 1];
			if (tex == 0)
				continue;

			p = primMem->curr;
			*(int *)&p->r0 = tempColor[1];
			*(int *)&p->r1 = tempColor[2];
			*(int *)&p->r2 = tempColor[3];
			*(int *)&p->u0 = *(int *)&tex->u0;
			*(int *)&p->u1 = *(int *)&tex->u1;
			*(short *)&p->u2 = *(short *)&tex->u2;
			setPolyGT3(p);
			gte_stsxy3(&p->x0, &p->x1, &p->x2);
			AddPrim((u_long *)pb->ptrOT + (otZ >> 2), p);
			primMem->curr = p + 1;
		}
	}
}

void RenderBucket_Execute(void *param_1, struct PrimMem *param_2)
{
	struct RenderBucketEntry *entry = (struct RenderBucketEntry *)param_1;

	// NOTE(aalhendi): Native C implementation of the RenderBucket execution
	// contract. Full RenderBucket draw/decompression ASM audit is still pending.
	for (; entry->inst != 0; entry++)
	{
		RenderBucket_DrawEntry(entry->inst, entry->instPlayerBase, param_2);
	}
}
