#include <common.h>

// TODO(aalhendi): still WIP. should polish more
#define PSX_OVR233_BASE          0x800AB9F0U
#define OVR233_PTR(psx_addr)     ((char *)((uintptr_t)&OVR_233 + ((psx_addr) - PSX_OVR233_BASE)))
#define OVR233_PTR_INT(psx_addr) ((int *)((uintptr_t)&OVR_233 + ((psx_addr) - PSX_OVR233_BASE)))

void DECOMP_CS_Thread_MoveOnPath(struct Thread *t);
void DECOMP_CS_Thread_Particles(struct Thread *t);
void DECOMP_CS_Thread_InterpolateFramesMS(struct Thread *t);

extern struct OVR233_Garage gGarage;

void DECOMP_CS_Thread_ThTick(struct Thread *t)
{
	short pos[3];
	struct CutsceneObj *cs = t->object;
	struct Instance *inst = t->inst;
	struct Instance *parentInst;
	struct Thread *parentThread;

	if (DECOMP_CS_Thread_UseOpcode(inst, cs))
		t->flags |= 0x800;

	DECOMP_CS_Thread_MoveOnPath(t);
	DECOMP_CS_Thread_AnimateScale(t);
	DECOMP_CS_Thread_Particles(t);

	if ((cs->flags & 0x40) != 0)
		DECOMP_CS_Thread_InterpolateFramesMS(t);

	// ASM: 0x800ae5dc - parent-thread frameOverrideRoot processing
	if (inst != 0)
	{
		parentThread = t->parentThread;

		if (parentThread != 0)
		{
			if ((cs->flags & 0x4) == 0)
			{
				parentInst = parentThread->inst;

				DECOMP_CS_Instance_GetFrameData(parentInst, parentInst->animIndex, parentInst->animFrame, (u_short *)pos, 0, 0);

				inst->matrix.t[0] = parentInst->matrix.t[0] + pos[0];
				inst->matrix.t[1] = parentInst->matrix.t[1] + pos[1];
				inst->matrix.t[2] = parentInst->matrix.t[2] + pos[2];

				if ((cs->flags & 0x10) == 0)
				{
					ConvertRotToMatrix(&inst->matrix, &pos[0]);
				}
			}
		}

		// ASM: 0x800ae6b4 - flag 0x8: write bone Y to OVR_233 global
		if ((cs->flags & 0x8) != 0)
		{
			DECOMP_CS_Instance_GetFrameData(inst, inst->animIndex, inst->animFrame, (u_short *)pos, 0, 0);

			*OVR233_PTR_INT(0x800b0b7c) = pos[1];

			inst = t->inst;
			if (inst == 0)
				goto thTick_epilogue;
		}

		// ASM: 0x800ae6fc - flag 0x2: random alphaScale for fade effect
		if ((cs->flags & 0x2) != 0)
		{
			inst->alphaScale = 0;

			if ((sdata->gGT->gameMode2 & 0x1) != 0)
			{
				inst->alphaScale = (DECOMP_MixRNG_Scramble() & 0x7ff) + 1024;
			}
		}
	}

	// ASM: 0x800ae744 - subtitle rendering
	if (cs->Subtitles.lngIndex > 0)
	{
		struct GameTracker *gGT = sdata->gGT;
		u_short textRect[6];

		textRect[0] = cs->Subtitles.textPos[1] - 236;
		textRect[1] = cs->Subtitles.colors - 4;
		textRect[2] = 472;
		textRect[3] = (cs->Subtitles.font << 0x10) >> 0x10;
		textRect[4] = cs->Subtitles.textPos[0] + 8;
		textRect[5] = 460;

		DECOMP_DecalFont_DrawMultiLine(sdata->lngStrings[cs->Subtitles.lngIndex], textRect[0], textRect[1], textRect[2], textRect[3], textRect[5]);

		DECOMP_RECTMENU_DrawInnerRect((u_short *)&textRect, 4, gGT->pushBuffer[0].ptrOT);
	}

thTick_epilogue:
	// ASM: 0x800ae7dc - check isCutsceneOver, re-apply death flag
	if (OVR_233.isCutsceneOver != 0)
	{
		t->flags |= 0x800;
	}
}

struct Thread *DECOMP_CS_Thread_Init(short modelID, char *name, short *param_3, short param_4, struct Thread *parent)
{
	struct GameTracker *gGT = sdata->gGT;
	struct CutsceneObj *cs;
	struct Instance *inst;
	struct Thread *t;
	char *scriptPtr;
	u_int bucket;
	short *meta;

	if (modelID == NOFUNC)
	{
		inst = NULL;

		t = DECOMP_PROC_BirthWithObject(SIZE_RELATIVE_POOL_BUCKET(0x60, NONE, MEDIUM, CAMERA), DECOMP_CS_Thread_ThTick, name, parent);

		if (t == NULL)
			return NULL;
	}
	else
	{
		bucket = OTHER;

		if ((u_int)(modelID - NDI_KART6) < 2)
			bucket = AKUAKU;

		if ((u_int)(modelID - NDI_KART0) < 4)
			bucket = GHOST;

		inst = DECOMP_INSTANCE_BirthWithThread(modelID, name, MEDIUM, bucket, DECOMP_CS_Thread_ThTick, 0x60, parent);

		if (inst == NULL)
			return NULL;

		t = inst->thread;
		t->funcThDestroy = DECOMP_PROC_DestroyInstance;
	}

	cs = t->object;

	cs->metadata = (int *)&cs->decodedOpcode;
	cs->frameOverrideRoot = NULL;
	cs->prevOpcode = (char *)-1;
	cs->Subtitles.lngIndex = -1;

	if (modelID == NOFUNC)
	{
		int level = gGT->levelID;

		if (level == NAUGHTY_DOG_CRATE)
		{
			scriptPtr = OVR233_PTR(0x800b4990);
		}
		else if (level == OXIDE_ENDING)
		{
			scriptPtr = OVR233_PTR(0x800b46fc);
		}
		else if (level == OXIDE_TRUE_ENDING)
		{
			scriptPtr = OVR233_PTR(0x800b472c);
		}
		else if ((gGT->gameMode2 & CREDITS) == 0)
		{
			scriptPtr = *(char **)(OVR233_PTR(0x800b46d8) + (level - 30) * 4);
		}
		else
		{
			scriptPtr = *(char **)(OVR233_PTR(0x800b4928) + (level - 44) * 4);
		}
	}
	else
	{
		// FIX(aalhendi): was `>` (off-by-one), ASM shows `0xb5 < param_1` → `>=`
		if (modelID >= NDI_BOX_BOX_01)
		{
			if ((u_int)(modelID - NDI_BOX_BOX_01) < 0x2b)
			{
				scriptPtr = *(char **)(OVR233_PTR(0x800b5a7c) + (modelID - NDI_BOX_BOX_01) * 4);
			}
			else
			{
				scriptPtr = OVR233_PTR(0x800b2e28);
			}

			DECOMP_CS_ScriptCmd_OpcodeAt(cs, scriptPtr);

			if ((u_int)(modelID - NDI_KART0) < 4)
			{
				cs->frameOverrideRoot = (int *)((uintptr_t)&OVR_233 + (modelID * 8 + 0x800b6d28 - PSX_OVR233_BASE));
			}

			goto after_opcode;
		}

		if ((u_int)(modelID - STATIC_PINHEAD) < 5)
		{
			scriptPtr = OVR233_PTR(0x800b2e28);
		}
		else if (modelID == STATIC_DINGOFIRE)
		{
			scriptPtr = OVR233_PTR(0x800b2e40);
		}
		else if ((u_int)(modelID - STATIC_TAWNA1) < 4)
		{
			if (gGT->gameMode2 & CREDITS)
				scriptPtr = OVR233_PTR(0x800b17dc);
			else
				scriptPtr = OVR233_PTR(0x800b17b4);
		}
		else if ((u_int)(modelID - STATIC_CRASHDANCE) < 0x10)
		{
			char *base;
			int off = (modelID - STATIC_CRASHDANCE) * 4;

			if (modelID == gGT->podium_modelIndex_First)
				base = OVR233_PTR(0x800b2e78);
			else
				base = OVR233_PTR(0x800b418c);

			scriptPtr = *(char **)(base + off);
		}
		else
		{
			scriptPtr = OVR233_PTR(0x800b2e28);
		}
	}

	DECOMP_CS_ScriptCmd_OpcodeAt(cs, scriptPtr);

after_opcode:

	cs->unk18 = cs->metadata[2];

	meta = (short *)cs->metadata;
	cs->unk14 = meta[2] + (short)(((DECOMP_MixRNG_Scramble() >> 2 & 0xfff) * ((meta[3] - meta[2]) + 1)) >> 0xc);

	if (inst != NULL)
	{
		MTC2(*(int *)(param_3 + 4), 0);
		MTC2(*(int *)(param_3 + 6), 1);
		gte_llv0();

		int rx = MFC2(25);
		int ry = MFC2(26);
		int rz = MFC2(27);

		inst->matrix.t[0] = rx + param_3[0];
		inst->matrix.t[1] = ry + param_3[1];
		inst->matrix.t[2] = rz + param_3[2];

		if (gGT->levelID != NAUGHTY_DOG_CRATE)
		{
			inst->scale[0] = 0x2800;
			inst->scale[1] = 0x2800;
			inst->scale[2] = 0x2800;
		}

		if ((u_int)(gGT->levelID - GEM_STONE_VALLEY) < 5)
		{
			inst->unk50 -= 4;
			inst->unk51 -= 4;
		}

		param_3[0xc] = param_3[8];
		param_3[0xe] = param_3[10];
		param_3[0xd] = param_3[9] + param_4;

		ConvertRotToMatrix(&inst->matrix, param_3 + 0xc);

		cs->unk1c = param_3[0xd] & 0xfff;
		cs->unk20 = param_3[0xc] & 0xfff;
		cs->unk22 = param_3[0xd] & 0xfff;
		cs->unk24 = param_3[0xe] & 0xfff;
	}

	cs->particleID = 0xff;
	cs->unk28 = 0;
	cs->unk1e = 0;
	cs->flags = 0;
	cs->scaleSpeed = 0;
	cs->desiredScale = 0x2800;

	cs->unk4 = 0;
	cs->unk6 = 0;
	cs->unk8 = 0x2e808080;
	cs->unk_C = 0;
	cs->unk_E = 0;

	cs->ptrIcons = (struct IconGroup *)((char *)gGT->iconGroup[0] + sizeof(struct IconGroup));

	return t;
}
