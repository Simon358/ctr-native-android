#include <common.h>


void *PlayerLastSpinFuncTable[0xD] = {0,
                                      VehPhysProc_SpinLast_Update,
                                      VehPhysProc_SpinLast_PhysLinear,
                                      VehPhysProc_Driving_Audio,
                                      VehPhysProc_SpinLast_PhysAngular,
                                      VehPhysForce_OnApplyForces,
                                      COLL_MOVED_PlayerSearch,
                                      VehPhysForce_CollideDrivers,
                                      COLL_FIXED_PlayerSearch,
                                      VehPhysGeneral_JumpAndFriction,
                                      VehPhysForce_TranslateMatrix,
                                      VehFrameProc_LastSpin,
                                      VehEmitter_DriverMain};

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x80064254-0x800642ec.
void VehPhysProc_SpinLast_Init(struct Thread *t, struct Driver *d)
{
	int i;

	for (i = 0; i < 0xD; i++)
	{
		d->funcPtrs[i] = PlayerLastSpinFuncTable[i];
	}
}
