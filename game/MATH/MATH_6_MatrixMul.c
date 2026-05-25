#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x8003d460-0x8003d4e4.
void MATH_MatrixMul(MATRIX *output, MATRIX *input, VECTOR *rotate)
{
	MATRIX *rotateMatrix = (MATRIX *)rotate;

	MatrixRotate(output, input, rotateMatrix);
	ApplyMatrixLV(input, (VECTOR *)rotateMatrix->t, (VECTOR *)output->t);

	output->t[0] += input->t[0];
	output->t[1] += input->t[1];
	output->t[2] += input->t[2];
}

void DECOMP_MATH_MatrixMul(MATRIX *output, MATRIX *input, VECTOR *rotate)
{
	MATH_MatrixMul(output, input, rotate);
}
