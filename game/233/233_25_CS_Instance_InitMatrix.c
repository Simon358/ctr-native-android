#include <common.h>

void DECOMP_CS_Instance_InitMatrix(void)
{
	if (OVR_233.cs_initMatrixBool != 0)
		return;

	OVR_233.cs_initMatrixBool = 1;

	MATRIX mat;
	int pos[3];

	for (int i = 0; i < 4; i++)
	{
		char *data = (char *)OVR_233.cs_initMatrixTable[i].data;
		int count = OVR_233.cs_initMatrixTable[i].count;

		if (data == NULL || count <= 0)
			continue;

		for (int j = 0; j < count; j++)
		{
			char *entry = data + j * 0x20;

			ConvertRotToMatrix(&mat, (short *)(entry + 8));

			pos[0] = *(short *)(entry + 0x10);
			pos[1] = *(short *)(entry + 0x12);
			pos[2] = *(short *)(entry + 0x14);

			MatrixRotate((MATRIX *)(entry + 8), (short *)pos, &mat);
		}
	}
}
