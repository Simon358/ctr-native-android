#include <common.h>

static s32 CAM_FollowDriver_TrackPath_MulLo(s32 a, s32 b)
{
	return (s32)(u32)((s64)a * (s64)b);
}

static struct CheckpointNode *CAM_FollowDriver_TrackPath_GetNode(struct CameraDC *cDC, struct CheckpointNode *node, int speed)
{
	struct CheckpointNode *nodes = sdata->gGT->level1->ptr_restart_points;
	u8 nodeIndex;

	if (speed > 0)
	{
		nodeIndex = node->nextIndex_backward;
		if ((cDC->flags & 0x100) != 0 && node->nextIndex_right != 0xff)
			nodeIndex = node->nextIndex_right;
	}
	else
	{
		nodeIndex = node->nextIndex_forward;
		if ((cDC->flags & 0x100) != 0 && node->nextIndex_left != 0xff)
			nodeIndex = node->nextIndex_left;
	}

	return &nodes[nodeIndex];
}

static int CAM_FollowDriver_TrackPath_Length(struct CheckpointNode *from, struct CheckpointNode *to, int *dx, int *dy, int *dz)
{
	*dx = to->pos[0] - from->pos[0];
	*dy = to->pos[1] - from->pos[1];
	*dz = to->pos[2] - from->pos[2];

	return SquareRoot0_stub(CAM_FollowDriver_TrackPath_MulLo(*dx, *dx) + CAM_FollowDriver_TrackPath_MulLo(*dy, *dy) +
	                        CAM_FollowDriver_TrackPath_MulLo(*dz, *dz));
}

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800198f8-0x80019e7c.
u32 CAM_FollowDriver_TrackPath(struct CameraDC *cDC, s16 *pos, int speed, int update)
{
	struct CheckpointNode *curr = (struct CheckpointNode *)cDC->unk88;
	struct CheckpointNode *next = CAM_FollowDriver_TrackPath_GetNode(cDC, curr, speed);
	struct CheckpointNode *angleNext;
	int pathProgress;
	int segmentLength;
	int ratio;
	int dx;
	int dy;
	int dz;
	int yaw;
	int nextYaw;
	int yawDelta;

	segmentLength = CAM_FollowDriver_TrackPath_Length(curr, next, &dx, &dy, &dz);

	if ((sdata->gGT->gameMode1 & 0xf) != 0)
		pathProgress = 0;
	else if (speed > 0)
		pathProgress = cDC->unk94 + speed;
	else
		pathProgress = cDC->unk94 - speed;

	while (pathProgress >= segmentLength)
	{
		pathProgress -= segmentLength;
		curr = next;
		next = CAM_FollowDriver_TrackPath_GetNode(cDC, curr, speed);
		segmentLength = CAM_FollowDriver_TrackPath_Length(curr, next, &dx, &dy, &dz);
	}

	if (update)
	{
		cDC->unk94 = pathProgress;
		cDC->unk88 = curr;
	}

	if (segmentLength != 0)
		ratio = (pathProgress << 12) / segmentLength;
	else
		ratio = 0;

	pos[0] = curr->pos[0] + (s16)((CAM_FollowDriver_TrackPath_MulLo(dx, ratio)) >> 12);
	pos[1] = curr->pos[1] + (s16)((CAM_FollowDriver_TrackPath_MulLo(dy, ratio)) >> 12) + 0x80;
	pos[2] = curr->pos[2] + (s16)((CAM_FollowDriver_TrackPath_MulLo(dz, ratio)) >> 12);

	yaw = (s16)ratan2(dx, dz) + 0x800;
	angleNext = CAM_FollowDriver_TrackPath_GetNode(cDC, next, speed);
	nextYaw = (s16)ratan2(angleNext->pos[0] - next->pos[0], angleNext->pos[2] - next->pos[2]) + 0x800;

	yawDelta = (nextYaw - yaw) & 0xfff;
	if (yawDelta >= 0x800)
		yawDelta -= 0x1000;

	return (yaw + (CAM_FollowDriver_TrackPath_MulLo(yawDelta, ratio) >> 12)) & 0xfff;
}
