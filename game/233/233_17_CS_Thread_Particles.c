#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800abdd4-0x800abf70
void CS_Thread_Particles(struct Thread *t)
{
	struct CutsceneObj *cs = t->object;
	struct Instance *inst = t->inst;
	struct unknown233 *entry;
	s8 particleID;

	if (inst == NULL)
		return;

	if ((inst->flags & HIDE_MODEL) != 0)
		return;

	particleID = cs->particleID;
	if ((u8)particleID >= 9)
		return;

	entry = &OVR_233.particleConfigs[(int)particleID];

	while (1)
	{
		u8 *meta = (u8 *)&entry->unknown2;
		int iconGroupIndex = meta[0];
		int frameOffset = meta[1];
		int count = meta[2];
		int flags = meta[3];
		s8 modelDelta = ((s8 *)&entry->unknown3)[0];

		for (int i = 0; i < count; i++)
		{
			struct Particle *p = Particle_Init(0, sdata->gGT->iconGroup[iconGroupIndex], (struct ParticleEmitter *)entry->unknown1);

			if (p != NULL)
			{
				s16 pos[3];

				CS_Instance_GetFrameData(inst, inst->animIndex, inst->animFrame, (u16 *)pos, NULL, frameOffset);

				p->axis[0].startVal += (pos[0] + inst->matrix.t[0]) << 8;
				p->axis[1].startVal += (pos[1] + inst->matrix.t[1]) << 8;
				p->axis[2].startVal += (pos[2] + inst->matrix.t[2]) << 8;
				p->unk18 = inst->unk50 + modelDelta;
			}
		}

		if ((flags & 1) == 0)
			break;

		entry++;
	}
}
