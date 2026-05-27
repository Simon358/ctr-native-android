#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x80042348-0x80042394.
void PROC_CollidePointWithBucket(struct Thread *th, struct BucketSearchParams *buf)
{
	// only used with drivers colliding
	// with other drivers, disabled online
	while (th != 0)
	{
		PROC_CollidePointWithSelf(th, buf);

		// next
		th = th->siblingThread;
	}
}
