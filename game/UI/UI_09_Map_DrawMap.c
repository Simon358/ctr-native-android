#include <common.h>

void UI_Map_DrawMap_ExtraFunc(struct Icon *icon, POLY_FT4 *p, s16 posX, s16 empty, struct PrimMem *primMem, u_long *otMem, u32 transparency);

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x8004d614-0x8004d8b4.
void UI_Map_DrawMap(struct Icon *mapTop, struct Icon *mapBottom, s16 posX, s16 posY, struct PrimMem *primMem, u_long *otMem, u32 colorID)
{
	s16 mapBottomHeight;
	s16 mapTopHeight;
	int iVar9;
	POLY_FT4 *p;
	u32 color;
	u32 transparency;
	struct GameTracker *gGT;

	gGT = sdata->gGT;

	iVar9 = 0;

	// draw minimap with neutral/none vertex color, minimap's regular color is white
	color = 0x808080;
	transparency = colorID;

	// draw map black
	// used for the minimap shadow in the track select screen
	if (colorID == 2)
	{
		color = 0;
		transparency = 0;
	}

	// draw minimap blue
	// used for the minimap outline in the track select screen
	if (colorID == 3)
	{
		color = 0x402000;
		transparency = 0;
	}

	if (gGT->level1->ptrSpawnType1 != 0)
	{
		void **pointers = ST1_GETPOINTERS(gGT->level1->ptrSpawnType1);
		iVar9 = (int)pointers[ST1_MAP];
	}

	// position of the bottom margin of the primitive for the bottom half of the minimap
	mapBottomHeight = mapBottom->texLayout.v2 - mapBottom->texLayout.v0;

	p = (POLY_FT4 *)primMem->curr;

	// if these conditions are met, then draw the top half of the minimap; otherwise, only draw the bottom half
	// not sure when the game ever draws only the bottom half
	if (((iVar9 != 0) && (*(s16 *)(iVar9 + 0x12) == 0)) ||

	    // if in main menu (character selection, track selection, any part of it)
	    ((gGT->gameMode1 & MAIN_MENU) != 0))
	{
		// r0, g0, b0 (vertex color)
		*(int *)&p->r0 = color;

		// position of the top margin of the primitive for the top half of the minimap
		mapTopHeight = posY - (((u16)mapTop->texLayout.v2 - (u16)mapTop->texLayout.v0) + mapBottomHeight);

		p->y0 = mapTopHeight;
		p->y1 = mapTopHeight;
		p->y2 = posY - mapBottomHeight;
		p->y3 = posY - mapBottomHeight;

		UI_Map_DrawMap_ExtraFunc(mapTop, p, posX, 0, primMem, otMem, transparency);

		p = p + 1;
	}

	// r0, g0, b0 (vertex color)
	*(int *)&p->r0 = color;

	p->y0 = posY - mapBottomHeight;
	p->y1 = posY - mapBottomHeight;
	p->y2 = posY;
	p->y3 = posY;

	UI_Map_DrawMap_ExtraFunc(mapBottom, p, posX, 0, primMem, otMem, transparency);

	primMem->curr = p + 1;
}

void UI_Map_DrawMap_ExtraFunc(struct Icon *icon, POLY_FT4 *p, s16 posX, s16 empty, struct PrimMem *primMem, u_long *otMem, u32 transparency)
{
	s16 leftX;
	s16 sizeX;

	sizeX = icon->texLayout.u1 - icon->texLayout.u0;

	// posX is the right side,
	// letftX is the left side
	leftX = posX - sizeX;

	p->x0 = leftX;
	p->x1 = posX;
	p->x2 = leftX;
	p->x3 = posX;

	// set header
	setPolyFT4(p);

	// UVs
	*(int *)&p->u0 = *(int *)&icon->texLayout.u0;
	*(int *)&p->u1 = *(int *)&icon->texLayout.u1;
	*(int *)&p->u2 = *(int *)&icon->texLayout.u2;
	*(s16 *)&p->u3 = *(s16 *)&icon->texLayout.u3;

	if (transparency != 0)
	{
		p->tpage = (p->tpage & 0xff9f) | ((u16)transparency << 5);
	}

	p->code |= 2;

	AddPrim(otMem, p);
}
