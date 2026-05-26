#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800457b0-0x800459ec.
void RECTMENU_DrawInnerRect(RECT *r, int type, u_long *ot)
{
	int *colorDataNormal;
	int *colorDataSpecial;
	int drawMode;
	RECT adjustedRect;

	colorDataNormal = &sdata->battleSetup_Color_UI_1;
	if ((type & 0x10) != 0)
	{
		colorDataNormal = &sdata->battleSetup_Color_UI_2;
	}

	if ((type & 2) == 0)
	{
		Color color;
		color.self = *colorDataNormal;
		RECTMENU_DrawOuterRect_HighLevel(r, color, (int)(s16)(type | 0x20), ot);
	}

	adjustedRect.x = r->x;
	adjustedRect.y = r->y;
	adjustedRect.w = r->w;
	adjustedRect.h = r->h;

	if ((type & 8) == 0)
	{
		if ((type & 2) == 0)
		{
			adjustedRect.x += 3;
			adjustedRect.y += 2;
			adjustedRect.w -= 6;
			adjustedRect.h -= 4;
		}

		if ((type & 1) == 0)
		{
			drawMode = ((type & 0x100) != 0) ? 2 : 0;
			colorDataSpecial = ((type & 0x100) != 0) ? &sdata->DrawSolidBoxData[1] : &sdata->DrawSolidBoxData[2];

			CTR_Box_DrawClearBox(&adjustedRect, (Color *)colorDataSpecial, drawMode, ot);
		}
		else
		{
			Color *color = (Color *)&sdata->DrawSolidBoxData[0];
			CTR_Box_DrawSolidBox(&adjustedRect, *color, ot);
		}
	}

	if ((type & 4) == 0)
	{
		s16 horizontalOffset = ((type & 0x80) != 0) ? 4 : 0xc;
		s16 verticalOffset = ((type & 0x40) != 0) ? 2 : 6;

		adjustedRect.x = r->x + r->w;
		adjustedRect.y = r->y + verticalOffset;
		adjustedRect.w = horizontalOffset;
		adjustedRect.h = r->h;

		int *color = &sdata->DrawSolidBoxData[0];
		CTR_Box_DrawClearBox(&adjustedRect, (Color *)color, 0, ot);

		adjustedRect.x = r->x + horizontalOffset;
		adjustedRect.y = r->y + r->h;
		adjustedRect.w = r->w - horizontalOffset;
		adjustedRect.h = verticalOffset;
		CTR_Box_DrawClearBox(&adjustedRect, (Color *)color, 0, ot);
	}

	return;
}
