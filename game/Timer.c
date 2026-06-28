#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x8004b31c-0x8004b370.
void Timer_Init()
{
	EnterCriticalSection();
	StopRCnt(DescRC + 1);
	SetRCnt(DescRC + 1, 0xffff, 0x2000);
	StartRCnt(DescRC + 1);
	ExitCriticalSection();
}

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x8004b370-0x8004b3a4.
void Timer_Destroy()
{
	EnterCriticalSection();
	StopRCnt(DescRC + 1);
	ExitCriticalSection();
}

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x8004b3a4-0x8004b41c.
int Timer_GetTime_Total()
{
	int rcntTotal = sdata->rcntTotalUnits;
	int rcnt = GetRCnt(DescRC + 1);
	int sysClock = rcntTotal + rcnt;

	if (rcnt < 100)
	{
		sysClock = sdata->rcntTotalUnits + rcnt;
	}

	return (sysClock * 1000) / 0x147e;
}

// Usage: elapsed(frameStart, &frameStart)
// will overwrite new frameStart, and return
// elapsed time since previous frameStart
// NOTE(aalhendi): ASM-verified NTSC-U 926 0x8004b41c-0x8004b470.
int Timer_GetTime_Elapsed(int oldVal, int *retVal)
{
	int newVal = Timer_GetTime_Total();

	if (retVal != 0)
	{
		*retVal = newVal;
	}

	// impossible?
	if (newVal < oldVal)
	{
		newVal += 0xc7e18;
	}

	return newVal - oldVal;
}
