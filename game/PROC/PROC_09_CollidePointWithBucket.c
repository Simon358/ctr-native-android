#include <common.h>

void DECOMP_PROC_CollidePointWithBucket(struct Thread *th, short *vec3_pos)
{
	// only used with drivers colliding
	// with other drivers, disabled online
	while (th != 0)
	{
		DECOMP_PROC_CollidePointWithSelf(th, (struct BucketSearchParams *)vec3_pos);

		// next
		th = th->siblingThread;
	}
}