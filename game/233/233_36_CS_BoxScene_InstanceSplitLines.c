#include <common.h>

void DECOMP_CS_BoxScene_InstanceSplitLines(void)
{
	short split = OVR_233.VertSplitLine;
	struct Thread *t = sdata->gGT->threadBuckets[GHOST].thread;

	while (t != NULL)
	{
		t->inst->vertSplit = split;
		t = t->siblingThread;
	}
}
