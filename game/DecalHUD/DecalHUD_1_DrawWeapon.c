#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x80022ec4-0x80023054.
void DecalHUD_DrawWeapon(struct Icon *icon, s16 posX, s16 posY, struct PrimMem *primMem, u_long *ot, char transparency, s16 scale, char rot)
{
#if BUILD > SepReview
	if (!icon)
		return;
#endif

	POLY_FT4 *p = (POLY_FT4 *)primMem->curr;
	addPolyFT4(ot, p);

	u32 width = icon->texLayout.u1 - icon->texLayout.u0;
	u32 height = icon->texLayout.v2 - icon->texLayout.v0;
	u32 rightX = posX + FP_Mult(width, scale);
	u32 bottomY = posY + FP_Mult(height, scale);
	u32 sidewaysX = posX + FP_Mult(height, scale);
	u32 sidewaysY = posY + FP_Mult(width, scale);

	// instead of psn00bsdk's setXY4, this function uses a custom-made macro that resembles the compiler optimization used in the original code
	// the X and Y fields of the primitive will be dereferenced as combined 32-bit integers for each vertex
	// from this, the X and Y coordinates will be added onto these integers using bitwise OR
	// this originally caused a bug where if X is higher than 0xFFFF (by not being cast as unsigned 16-bits) it will overflow onto Y
	// for the sake of making this compile under the original file size of the function (0x190 bytes) this macro will be used with the proper variable casts
	if (!(rot & 1))
	{
		if (rot == 0)
		{
			setXY4CompilerHack(p, (u16)posX, posY, (u16)rightX, posY, (u16)posX, bottomY, (u16)rightX, bottomY);
		}
		else
		{
			setXY4CompilerHack(p, (u16)rightX, bottomY, (u16)posX, bottomY, (u16)rightX, posY, (u16)posX, posY);
		}
	}
	else
	{
		if (rot == 1)
		{
			setXY4CompilerHack(p, (u16)posX, sidewaysY, (u16)posX, posY, (u16)sidewaysX, sidewaysY, (u16)sidewaysX, posY);
		}
		else
		{
			setXY4CompilerHack(p, (u16)sidewaysX, posY, (u16)sidewaysX, sidewaysY, (u16)posX, posY, (u16)posX, sidewaysY);
		}
	}

	setIconUV(p, icon);

	// this function doesn't support coloring the primitives
	setShadeTex(p, true);

	if (transparency)
	{
		setTransparency(p, transparency);
	}

	primMem->curr = p + 1;
}
