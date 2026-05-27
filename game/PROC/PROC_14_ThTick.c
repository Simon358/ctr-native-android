#include <common.h>

enum
{
	THTICK_MAX_PENDING = 128
};

static void ThTick_PushPending(struct Thread **pending, int *count, struct Thread *thread)
{
	if (thread == NULL)
		return;

	if (*count >= THTICK_MAX_PENDING)
		return;

	pending[*count] = thread;
	(*count)++;
}

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800715e8-0x80071694.
void ThTick_RunBucket(struct Thread *thread)
{
	struct Thread *pending[THTICK_MAX_PENDING];
	int count = 0;

	ThTick_PushPending(pending, &count, thread);

	while (count > 0)
	{
		struct Thread *t = pending[--count];

		ThTick_PushPending(pending, &count, t->siblingThread);

		if (t->cooldownFrameCount < 0)
			continue;

		if (t->cooldownFrameCount != 0)
		{
			t->cooldownFrameCount--;
			continue;
		}

		if (t->funcThTick != NULL)
			t->funcThTick(t);

		ThTick_PushPending(pending, &count, t->childThread);
	}
}

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x80071694-0x800716ec as native-equivalent divergence.
void ThTick_FastRET(struct Thread *thread)
{
	(void)thread;
}

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800716ec-0x80071704 as native-equivalent divergence.
void ThTick_SetAndExec(struct Thread *thread, void (*funcThTick)(struct Thread *))
{
	thread->funcThTick = funcThTick;
	funcThTick(thread);
}

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x80071704-0x8007170c.
void ThTick_Set(struct Thread *thread, void (*funcThTick)(struct Thread *))
{
	thread->funcThTick = funcThTick;
}
