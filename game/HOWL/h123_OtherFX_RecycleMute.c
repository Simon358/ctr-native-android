#include <common.h>

void DECOMP_OtherFX_RecycleMute(int *soundID_Count)
{
	if (*soundID_Count != 0)
	{
		DECOMP_OtherFX_Stop1(*soundID_Count);
		*soundID_Count = 0;
	}
}
