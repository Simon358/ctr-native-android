#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x8003dc30-0x8003dc9c.
int MEMCARD_ReadFile(int start_offset, int size)
{
	if ((lseek(sdata->memcard_fd, start_offset, 0) >= 0) && (read(sdata->memcard_fd, sdata->memcard_ptrStart, size) >= 0))
	{
		// The read has started, the result will be found
		// the next time we wait for an event result
		return MC_RETURN_PENDING;
	}

	MEMCARD_CloseFile();
	return MC_RETURN_TIMEOUT;
}
