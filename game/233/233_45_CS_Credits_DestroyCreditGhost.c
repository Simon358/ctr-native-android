#include <common.h>

// ASM-verified: 0x800b885c-0x800b88c8
void CS_Credits_DestroyCreditGhost(void)
{
	for (int i = 0; i < 5; i++)
	{
		DECOMP_INSTANCE_Death(creditsBSS.creditsObj.creditGhostInst[i]);
	}

	DECOMP_MEMPACK_ClearHighMem();
}
