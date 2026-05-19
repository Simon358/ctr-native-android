// NOTE(aalhendi): Native bridge for the still-raw overlay 233 data section.
// D230/D231/D232 use C initializers with named self-references; 233 still loads
// the retail overlay blob and rebases pointer words until the data is fully named.
// TODO(aalhendi): fully name data

#include <common.h>
#include <stdio.h>

#define PSX_OVR233_BASE  0x800AB9F0U
#define OVR233_DATA_SIZE 48528

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
