// TODO(aalhendi): This entire file is a hack. D230/D231/D232 use proper C struct
// initializers with &D230.field self-references (see game/230/D230.c). This file
// instead loads the raw overlay binary at runtime and does pointer rebasing.
// Replace with proper static initialization once the overlay data is fully reverse-engineered into named struct fields.

#include <common.h>
#include <stdio.h>

#define PSX_OVR233_BASE  0x800AB9F0U
#define OVR233_DATA_SIZE 48528

extern typeof(OVR_233) OVR_233;

void DECOMP_CS_OVR233_LoadData(void)
{
	FILE *f = fopen("assets/bigfile/overlays/233_Threads_Cutscene.bin", "rb");
	if (!f)
	{
		fprintf(stderr, "[CTR] Failed to open 233_Threads_Cutscene.bin\n");
		return;
	}

	fread(&OVR_233, 1, OVR233_DATA_SIZE, f);
	fclose(f);

	uint32_t delta = (uintptr_t)&OVR_233 - PSX_OVR233_BASE;

	uint32_t *p = (uint32_t *)&OVR_233;
	uint32_t ovr_start = PSX_OVR233_BASE;
	uint32_t ovr_end = PSX_OVR233_BASE + 56844;
	int count = OVR233_DATA_SIZE / 4;

#define CODE_REGION_END 0x3E4

	for (int i = CODE_REGION_END / 4; i < count; i++)
	{
		if (p[i] >= ovr_start && p[i] < ovr_end)
		{
			p[i] += delta;
		}
	}
}
