#include <common.h>

void TEST_ThTickRunBucket(struct Thread *t)
{
	for (/**/; t != 0; t = t->siblingThread)
	{
		if ((t->flags & 0x800) != 0)
			continue;

		if (t->funcThTick == 0)
			continue;

		if (t->cooldownFrameCount != 0)
		{
			t->cooldownFrameCount--;
			continue;
		}

		t->funcThTick(t);
	}
}

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x80071694-0x800716ec as a
// native-equivalent divergence. Retail unwinds the scratchpad thread runner;
// native thread ticks are normal C calls, so the equivalent is a no-op return.
void ThTick_FastRET(struct Thread *t)
{
	(void)t;
}
