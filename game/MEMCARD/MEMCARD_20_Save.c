#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 PS1 path 0x8003e344-0x8003e51c; CTR_NATIVE writes host files directly.
u8 MEMCARD_Save(int slotIdx, char *name, char *param_3, u8 *ptrMemcard, int memcardFileSize, u32 param6)

{
#ifdef CTR_NATIVE
	char path[0x40];
	char *nameNoDevice;
	FILE *file;
	size_t wroteIcon;
	size_t wroteData;

	(void)slotIdx;
	(void)param_3;
	(void)param6;

	nameNoDevice = name;
	if (strncmp(nameNoDevice, "bu00:", 5) == 0)
		nameNoDevice += 5;

	sdata->crc16_checkpoint_byteIndex = 0;
	sdata->crc16_checkpoint_status = 0;
	MEMCARD_ChecksumSave(ptrMemcard, memcardFileSize);

	snprintf(path, sizeof(path), "%s", nameNoDevice);

	file = fopen(path, "wb");
	if (file == NULL)
		return MC_RETURN_FULL;

	wroteIcon = fwrite(&data.memcardIcon_Ghost[0], 1, 0x100, file);
	wroteData = fwrite(ptrMemcard, 1, memcardFileSize, file);
	fclose(file);

	if ((wroteIcon != 0x100) || (wroteData != (size_t)memcardFileSize))
		return MC_RETURN_TIMEOUT;

	return MC_RETURN_IOE;
#else
	if (sdata->memcard_stage != MC_STAGE_IDLE)
		return MC_RETURN_TIMEOUT;

	sdata->memcardIconSize = 0x100;

	if (MEMCARD_NewTask(slotIdx, name, ptrMemcard, memcardFileSize, 0) != 0)
		return MC_RETURN_TIMEOUT;

	u8 *icon = (u8 *)&data.memcardIcon_PsyqHand[0];

	if (((param6 & 1) == 0) && (((sdata->memcardIconSize + memcardFileSize * 2 + 0x1fff) >> 13) >= 2))
	{
		icon[3] = (sdata->memcardIconSize + memcardFileSize + 0x1fff) >> 13;
		sdata->memcardStatusFlags |= 4;
	}
	else
	{
		sdata->memcardStatusFlags &= ~4;
		icon[3] = (sdata->memcardIconSize + memcardFileSize * 2 + 0x1fff) >> 13;
	}

	for (int i = 0; i < 0x40; i += 2)
	{
		icon[i + 4] = 0x81;
		icon[i + 5] = 0x40;
	}

	if (param_3[0] != '\0')
	{
		for (int i = 0; (i < 0x40) && (param_3[i] != '\0'); i++)
			icon[i + 4] = param_3[i];
	}

	MEMCARD_ChecksumSave(ptrMemcard, memcardFileSize);

	sdata->memcard_fd = open(sdata->s_memcardFileCurr, (icon[3] << 16) | FCREATE);

	if (sdata->memcard_fd != -1)
	{
		close(sdata->memcard_fd);
		sdata->memcard_fd = -1;
	}

	sdata->memcard_fd = open(sdata->s_memcardFileCurr, FASYNC | FWRITE);

	if (sdata->memcard_fd == -1)
	{
		MEMCARD_CloseFile();
		return MC_RETURN_FULL;
	}
	else
	{
		sdata->memcard_stage = MC_STAGE_SAVE_PART0_START;
		return MEMCARD_WriteFile(0, icon, sdata->memcardIconSize);
	}
#endif
}
