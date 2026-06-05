#include <common.h>

// copy/paste from gGT in namespace_Main.h
struct RenderList
{
	// 1P2P lists (might change in 3P4P)
	// [0] - 0x00: Force4x4
	// [1] - 0x08: DynamicSubdiv
	// [2] - 0x10: Force4x2
	// [3] - 0x18: Force4x1
	// [4] - 0x20: Water
	struct
	{
		void *ptrQuadBlocksRendered;
		struct BSP **bspListStart;
	} list[5];

	// 0x28
	struct BSP **bspListStart_FullDynamic;
	void *ptrQuadBlocksRendered_FullDynamic;

	// 0x30 large

	// one for each player
};

// From CTR 226 overlay
const int shiftTable226[24] = {0x18100800, 0x80900818, 0x81018,    0x98881000, 0x98081000, 0x881018,   0x80100818, 0x18900800,
                               0x10081898, 0x88881810, 0x18180810, 0x90980888, 0x90180888, 0x18980810, 0x88081810, 0x10881898,
                               0x8181090,  0x98981008, 0x10101808, 0x88901898, 0x88101898, 0x10901808, 0x98181008, 0x8981090};

int comboNum = 0;

int combinations[] = {
    0x0004080a, // 0 1 2 3 (sideways)
    0x00040a08, // 0 1 3 2
    0x0008040a, // 0 2 1 3
    0x00040a08, // 0 2 3 1
    0x000a0408, // 0 3 1 2
    0x000a0804, // 0 3 2 1

    0x0400080a, // 0 1 2 3
    0x04000a08, // 0 1 3 2
    0x0408000a, // 0 2 1 3
    0x04000a08, // 0 2 3 1
    0x040a0008, // 0 3 1 2
    0x040a0800, // 0 3 2 1

    0x0804000a, // 0 1 2 3
    0x08040a00, // 0 1 3 2
    0x0800040a, // 0 2 1 3
    0x08040a00, // 0 2 3 1
    0x080a0400, // 0 3 1 2
    0x080a0004, // 0 3 2 1

    0x0a040800, // 0 1 2 3
    0x0a040008, // 0 1 3 2
    0x0a080400, // 0 2 1 3
    0x0a040008, // 0 2 3 1
    0x0a000408, // 0 3 1 2
    0x0a000804, // 0 3 2 1
};

void TEST_226(struct RenderList *RL, struct PushBuffer *pb, struct mesh_info *mi, struct PrimMem *primMem, int *visMem10, int unk)
{
	s16 posScreen1[4];
	s16 posScreen2[4];
	s16 posScreen3[4];
	s16 posScreen4[4];

	struct LevVertex *pVA = &mi->ptrVertexArray[0];

#if 1
	// temporary, until CAMERA_ThTick is done
	pb->distanceToScreen_PREV = pb->distanceToScreen_CURR;

	// temporary, until PushBuffer_UpdateFrustum is done
	gte_SetGeomScreen(pb->distanceToScreen_PREV);
#endif

	MATRIX *mat2 = &pb->matrix_ViewProj;

	gte_SetRotMatrix(mat2);
	gte_SetTransMatrix(mat2);
	gte_SetGeomOffset(pb->rect.w >> 1, pb->rect.h >> 1);

	void *ot = &pb->ptrOT[0];

#if 0
	printf("\nDump Level:\n");
#endif

	int numBlock = 0;

	POLY_GT4 *p;
	void *pNext;
	void *pCurr;

#define USE_RL 0

#if USE_RL
	for (int i = 0; i < 5; i++)
	{
		if (RL->list[i].bspListStart == 0)
			continue;

		int *slot;
		struct BSP *bsp;
		int count = 0;

		for (slot = RL->list[i].bspListStart; slot[0] != 0; count++, slot = slot[0])
		{
			bsp = slot[1];
#else
	for (int i = 0; i < mi->numBspNodes; i++)
	{
		struct BSP *bsp = &mi->bspRoot[i];

		// draw leaf nodes only
		if ((bsp->flag & 1 == 0))
			continue;

		if (bsp->data.leaf.ptrQuadBlockArray == 0)
			continue;

		// This gives no improvement
#if 0
			// === Test every BSP block against Frustum ===

		s16 posBSP[3];
		int otZ_block;

		// minX, minY, minZ
		posBSP[0] = bsp->box.min[0];
		posBSP[1] = bsp->box.min[1];
		posBSP[2] = bsp->box.min[2];
		gte_ldv0(&posBSP[0]);
		gte_rtps();
		// edit: did I forget gte_avsz3(); the last time I tested?
		// Is that why the test gave no improvement?
		gte_stotz(&otZ_block);
		gte_stsxy(&posScreen1[0]);
		if (posScreen1[0] > 0) goto PassFrustumBSP;
		if (posScreen1[0] < pb->rect.w) goto PassFrustumBSP;
		if (otZ_block >= 0) continue;

		// fail Frustum BSP
		continue;

	PassFrustumBSP:
#endif


#endif
			for (int j = 0; j < bsp->data.leaf.numQuads; j++)
			{
				struct QuadBlock *block;
				block = &bsp->data.leaf.ptrQuadBlockArray[j];

				// dont invisible walls
				if ((block->quadFlags & (1 << 15)) != 0)
					continue;

				int boolWater = 0;

				// check for water
				if ((bsp->flag & 2) != 0)
				{
					POLY_F4 *p = primMem->curr;
					void *pNext = p + 1;

					p->r0 = 0x7F;
					p->g0 = 0x7F;
					p->b0 = 0xFF;
					setPolyF4(p);

					gte_ldv0(&pVA[block->index[0]].pos[0]);
					gte_rtps();
					gte_stsxy(&posScreen1[0]);

					gte_ldv0(&pVA[block->index[1]].pos[0]);
					gte_rtps();
					gte_stsxy(&posScreen2[0]);

					gte_ldv0(&pVA[block->index[2]].pos[0]);
					gte_rtps();
					gte_stsxy(&posScreen3[0]);

					gte_ldv0(&pVA[block->index[3]].pos[0]);
					gte_rtps();
					gte_stsxy(&posScreen4[0]);

					setXY4(p, (posScreen1[0]), (posScreen1[1]), // XY0
					       (posScreen2[0]), (posScreen2[1]),    // XY1
					       (posScreen3[0]), (posScreen3[1]),    // XY2
					       (posScreen4[0]), (posScreen4[1]));

					// automatic pass, if no frontface or backface culling
					int boolPassCull = (block->draw_order_low & 0x80000000) != 0;

					if (!boolPassCull)
					{
						// check CW/CCW culling
						int opZ;
						gte_nclip();
						gte_stopz(&opZ);
						boolPassCull = (opZ < 0);
					}

					if (boolPassCull)
					{
						s16 midX = (p->x0 + p->x1 + p->x2 + p->x3) / 4;
						s16 midY = (p->y0 + p->y1 + p->y2 + p->y3) / 4;

						s16 midArr[4] = {midX, midY, 0, 0};
						gte_ldv0(midArr);

						int otZ;
						gte_avsz3();
						gte_stotz(&otZ);

						if (otZ > 0)
						{
							if (otZ > 4080)
								otZ = 4080;

							AddPrim((u_long *)ot + (otZ >> 2), p);
							primMem->curr = pNext;
						}
					}

					// next quadblock
					continue;
				}

				int idHigh_ShapeOfZ[16] = {0, 4, 5, 6, 4, 1, 6, 7, 5, 6, 2, 8, 6, 7, 8, 3};

				int idLow_ShapeOfZ[4] = {0, 1, 2, 3};

#define L1Z(x) idLow_ShapeOfZ[x]
#define H1Z(x) idHigh_ShapeOfZ[0 + x]
#define H2Z(x) idHigh_ShapeOfZ[4 + x]
#define H3Z(x) idHigh_ShapeOfZ[8 + x]
#define H4Z(x) idHigh_ShapeOfZ[12 + x]

				int idHigh[16] = {
				    // 0
				    // H1Z(0), H1Z(1), H1Z(2), H1Z(3),
				    // H2Z(0), H2Z(1), H2Z(2), H2Z(3),
				    // H3Z(0), H3Z(1), H3Z(2), H3Z(3),
				    // H4Z(0), H4Z(1), H4Z(2), H4Z(3),

				    // 1
				    // H1Z(0), H1Z(1), H1Z(3), H1Z(2),
				    // H2Z(0), H2Z(1), H2Z(3), H2Z(2),
				    // H3Z(0), H3Z(1), H3Z(3), H3Z(2),
				    // H4Z(0), H4Z(1), H4Z(3), H4Z(2),

				    // 2
				    // H1Z(0), H1Z(2), H1Z(1), H1Z(3),
				    // H2Z(0), H2Z(2), H2Z(1), H2Z(3),
				    // H3Z(0), H3Z(2), H3Z(1), H3Z(3),
				    // H4Z(0), H4Z(2), H4Z(1), H4Z(3),

				    // 3
				    // H1Z(0), H1Z(2), H1Z(3), H1Z(1),
				    // H2Z(0), H2Z(2), H2Z(3), H2Z(1),
				    // H3Z(0), H3Z(2), H3Z(3), H3Z(1),
				    // H4Z(0), H4Z(2), H4Z(3), H4Z(1),

				    // 4
				    // H1Z(1), H1Z(0), H1Z(2), H1Z(3),
				    // H1Z(1), H1Z(0), H1Z(2), H1Z(3),
				    // H1Z(1), H1Z(0), H1Z(2), H1Z(3),
				    // H1Z(1), H1Z(0), H1Z(2), H1Z(3),
				    //
				    // H1Z(1), H1Z(0), H1Z(3), H1Z(2),
				    // H2Z(1), H2Z(0), H2Z(3), H2Z(2),
				    // H3Z(1), H3Z(0), H3Z(3), H3Z(2),
				    // H4Z(1), H4Z(0), H4Z(3), H4Z(2),
				    //
				    // H1Z(1), H1Z(2), H1Z(0), H1Z(3),
				    // H2Z(1), H2Z(2), H2Z(0), H2Z(3),
				    // H3Z(1), H3Z(2), H3Z(0), H3Z(3),
				    // H4Z(1), H4Z(2), H4Z(0), H4Z(3),
				    //
				    // H1Z(1), H1Z(2), H1Z(3), H1Z(0),
				    // H2Z(1), H2Z(2), H2Z(3), H2Z(0),
				    // H3Z(1), H3Z(2), H3Z(3), H3Z(0),
				    // H4Z(1), H4Z(2), H4Z(3), H4Z(0),
				    //
				    // H1Z(2), H1Z(1), H1Z(0), H1Z(3),
				    // H2Z(2), H2Z(1), H2Z(0), H2Z(3),
				    // H3Z(2), H3Z(1), H3Z(0), H3Z(3),
				    // H4Z(2), H4Z(1), H4Z(0), H4Z(3),
				    //
				    // H1Z(2), H1Z(1), H1Z(3), H1Z(0),
				    // H2Z(2), H2Z(1), H2Z(3), H2Z(0),
				    // H3Z(2), H3Z(1), H3Z(3), H3Z(0),
				    // H4Z(2), H4Z(1), H4Z(3), H4Z(0),
				    //
				    // H1Z(2), H1Z(0), H1Z(1), H1Z(3),
				    // H2Z(2), H2Z(0), H2Z(1), H2Z(3),
				    // H3Z(2), H3Z(0), H3Z(1), H3Z(3),
				    // H4Z(2), H4Z(0), H4Z(1), H4Z(3),
				    //
				    // H1Z(2), H1Z(0), H1Z(3), H1Z(1),
				    // H2Z(2), H2Z(0), H2Z(3), H2Z(1),
				    // H3Z(2), H3Z(0), H3Z(3), H3Z(1),
				    // H4Z(2), H4Z(0), H4Z(3), H4Z(1),
				    //
				    H1Z(3), H1Z(1), H1Z(2), H1Z(0), H2Z(3), H2Z(1), H2Z(2), H2Z(0), H3Z(3), H3Z(1), H3Z(2), H3Z(0), H4Z(3), H4Z(1), H4Z(2), H4Z(0),
				    //
				    // H1Z(3), H1Z(1), H1Z(0), H1Z(2),
				    // H2Z(3), H2Z(1), H2Z(0), H2Z(2),
				    // H3Z(3), H3Z(1), H3Z(0), H3Z(2),
				    // H4Z(3), H4Z(1), H4Z(0), H4Z(2),
				    //
				    // H1Z(3), H1Z(2), H1Z(1), H1Z(0),
				    // H2Z(3), H2Z(2), H2Z(1), H2Z(0),
				    // H3Z(3), H3Z(2), H3Z(1), H3Z(0),
				    // H4Z(3), H4Z(2), H4Z(1), H4Z(0),
				    //
				    // H1Z(3), H1Z(2), H1Z(0), H1Z(1),
				    // H2Z(3), H2Z(2), H2Z(0), H2Z(1),
				    // H3Z(3), H3Z(2), H3Z(0), H3Z(1),
				    // H4Z(3), H4Z(2), H4Z(0), H4Z(1)
				};

				int idLow[4] = {L1Z(3), L1Z(1), L1Z(2), L1Z(0)};

				// low LOD
				int num = 1;
				int *id = idLow;

				// high LOD
				num = 4;
				id = idHigh;

				for (int k = 0; k < num; k++)
				{
					p = primMem->curr;
					pNext = p + 1;
					pCurr = p;
					if (pNext > ((u32)primMem->end - 0x200))
						return;

					*(int *)&p->r0 = *(int *)&pVA[block->index[id[4 * k + 0]]].color_hi[0];
					*(int *)&p->r1 = *(int *)&pVA[block->index[id[4 * k + 1]]].color_hi[0];
					*(int *)&p->r2 = *(int *)&pVA[block->index[id[4 * k + 2]]].color_hi[0];
					*(int *)&p->r3 = *(int *)&pVA[block->index[id[4 * k + 3]]].color_hi[0];

					setPolyGT4(p);

					gte_ldv0(&pVA[block->index[id[4 * k + 0]]].pos[0]);
					gte_rtps();
					gte_stsxy(&posScreen1[0]);

					gte_ldv0(&pVA[block->index[id[4 * k + 1]]].pos[0]);
					gte_rtps();
					gte_stsxy(&posScreen2[0]);

					gte_ldv0(&pVA[block->index[id[4 * k + 2]]].pos[0]);
					gte_rtps();
					gte_stsxy(&posScreen3[0]);

					gte_ldv0(&pVA[block->index[id[4 * k + 3]]].pos[0]);
					gte_rtps();
					gte_stsxy(&posScreen4[0]);

					// to be in viewport, coordinates must be
					// X: [0, 0x40]
					// Y: [0, 0xA0]
					setXY4(p, (posScreen1[0]), (posScreen1[1]), // XY0
					       (posScreen2[0]), (posScreen2[1]),    // XY1
					       (posScreen3[0]), (posScreen3[1]),    // XY2
					       (posScreen4[0]), (posScreen4[1]));

					// if num == 1
					struct TextureLayout *tl = block->ptr_texture_low;

					if (num == 4)
					{
						// must remove flags that are stored in pointer
						u32 ptr = (u32)block->ptr_texture_mid[k];
						tl = ptr;

						if (ptr & 1)
						{
							ptr = ptr & ~(3);
							tl = *(int *)ptr;
						}

						if (tl != 0)
						{
							// move ptr to highest mid lod
							tl += 2;
						}
					}

					u32 draw_order_high = block->draw_order_high;
					u32 draw_order_low = block->draw_order_low;

					if (tl != 1)
					{
						u32 bits5 = (draw_order_low >> (8 + k * 5)) & 0x1F;
						char *tlByteAddr = (char *)tl;

						// u0, u1, u2, u3 {3,2,1,0}
						// int tlOffsets_RegularOrder = 0x0a080400;

						comboNum = sdata->gGT->timer >> 6;
						if (comboNum > 24)
							comboNum = 0;

						// This will do "FOR NOW"
						comboNum = 10;

						// match idLow: 2, 0, 3, 1 {1,3,0,2}
						int tlOffsets = combinations[comboNum];

						int tlU0 = (tlOffsets >> ((shiftTable226[bits5] >> 0x0) & 0xFF)) & 0xFF;
						int tlU1 = (tlOffsets >> ((shiftTable226[bits5] >> 0x8) & 0xFF)) & 0xFF;
						int tlU2 = (tlOffsets >> ((shiftTable226[bits5] >> 0x10) & 0xFF)) & 0xFF;
						int tlU3 = (tlOffsets >> ((shiftTable226[bits5] >> 0x18) & 0xFF)) & 0xFF;

						setUV4(p, *(u8 *)&tlByteAddr[tlU0 + 0], *(u8 *)&tlByteAddr[tlU0 + 1], *(u8 *)&tlByteAddr[tlU1 + 0], *(u8 *)&tlByteAddr[tlU1 + 1],
						       *(u8 *)&tlByteAddr[tlU2 + 0], *(u8 *)&tlByteAddr[tlU2 + 1], *(u8 *)&tlByteAddr[tlU3 + 0], *(u8 *)&tlByteAddr[tlU3 + 1]);

						p->clut = tl->clut;
						p->tpage = tl->tpage;
					}

					// automatic pass, if no frontface or backface culling
					int boolPassCull = (draw_order_low & 0x80000000) != 0;

					if (!boolPassCull)
					{
						// check CW/CCW culling
						int opZ;
						gte_nclip();
						gte_stopz(&opZ);
						boolPassCull = (opZ >= 0);
					}

					if (boolPassCull)
					{
						s16 midX = (p->x0 + p->x1 + p->x2 + p->x3) / 4;
						s16 midY = (p->y0 + p->y1 + p->y2 + p->y3) / 4;

						s16 midArr[4] = {midX, midY, 0, 0};
						gte_ldv0(midArr);

						int otZ;
						gte_avsz3();
						gte_stotz(&otZ);

						if (otZ > 0)
						{
							if (otZ > 4080)
								otZ = 4080;

							if ((p->tpage & 0x40) == 0)
								setSemiTrans(p, true);

							AddPrim((u_long *)ot + (otZ >> 2), pCurr);
							primMem->curr = pNext;
						}
					}
				}
			}
		}

#if USE_RL
	}
#endif
}
