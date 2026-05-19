#include <common.h>

// Make the trophy bounce 3 times
// Then start ThTick3
// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800afbc8-0x800afcc4
void CS_Podium_Prize_ThTick2(struct Thread *th)
{
	int currScale;

	// get object from thread
	// should replace with struct Prize in 233
	short *prize = th->object;

	// get instance from thread
	struct Instance *inst = th->inst;

	short frameIndex = prize[0x15];

	// bouncing scale animation
	if (frameIndex < 5)
	{
		// if even frame
		if ((frameIndex & 1) == 0)
		{
			// scaleX
			currScale = inst->scale[0] + 800 + frameIndex * 400;

			if ((frameIndex + 1) * 0x28a + 0x2000 < currScale)
			{
				// frame counter
				frameIndex += 1;
			}
		}
		else
		{
			// scaleX
			currScale = inst->scale[0] - 800;

			if (currScale < 0x1001)
			{
				// frame counter
				frameIndex += 1;
			}
		}

		prize[0x15] = frameIndex;

		// scaleY and scaleZ
		inst->scale[0] = currScale;
		inst->scale[1] = currScale;
		inst->scale[2] = currScale;

		CS_Podium_Prize_Spin(inst, prize);
	}
	else
	{
		ThTick_SetAndExec(th, CS_Podium_Prize_ThTick3);
	}
}
