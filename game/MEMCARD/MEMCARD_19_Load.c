#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x8003e29c-0x8003e344.
u8 MEMCARD_Load(int slotIdx, char *name, u8 *ptrMemcard, int memcardFileSize, u32 param5)
{
	if (sdata->memcard_stage != MC_STAGE_IDLE)
		return MC_RETURN_TIMEOUT;

	if (MEMCARD_NewTask(slotIdx, name, ptrMemcard, memcardFileSize, 0) != 0)
		return MC_RETURN_TIMEOUT;

	sdata->memcard_fd = open(sdata->s_memcardFileCurr, FASYNC | FREAD);

	if (sdata->memcard_fd == -1)
	{
		MEMCARD_CloseFile();
		return MC_RETURN_NODATA;
	}

	if ((param5 & 2) != 0)
		sdata->memcardStatusFlags |= 8;
	else
		sdata->memcardStatusFlags &= ~8;

	sdata->memcard_stage = MC_STAGE_LOAD_PART0_START;
	return MEMCARD_ReadFile(0, 0x80);
}
