#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x8003e59c-0x8003e600.
int MEMCARD_IsFile(int slotIdx, char *save_name)
{
	char name[64];

	MEMCARD_StringSet(name, slotIdx, save_name);

	int fd;

	fd = open(name, FASYNC | FWRITE);
	sdata->memcard_fd = fd;

	if (fd != -1)
	{
		close(fd);
		sdata->memcard_fd = -1;
		return MC_RETURN_IOE;
	}

	return MC_RETURN_NODATA;
}
