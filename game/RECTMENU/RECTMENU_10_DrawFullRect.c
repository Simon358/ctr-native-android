#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800459ec-0x80045b1c.
void RECTMENU_DrawFullRect(struct RectMenu *menu, RECT *inner)
{
	u32 *rgb;
	RECT outer;
	struct GameTracker *gGT = sdata->gGT;

	// if title text exists
	if ((-1 < menu->stringIndexTitle) && ((menu->state & 4) == 0))
	{
		rgb = (menu->drawStyle & 0x10) ? &sdata->battleSetup_Color_UI_2 : &sdata->battleSetup_Color_UI_1;

		outer.x = inner->x + 3;
		outer.y = inner->y + 6;

		// pixel-height of non-title menu rows
		if ((menu->state & 0x80) == 0)
		{
			outer.y = inner->y + 9 + data.font_charPixHeight[1];
		}
		else if ((menu->state & 0x4000) == 0)
		{
			outer.y += data.font_charPixHeight[2];
		}
		else
		{
			outer.y += data.font_charPixHeight[1];
		}

		outer.h = 2;
		outer.w = inner->w - 6;

		Color color;
		color.self = *rgb;
		RECTMENU_DrawOuterRect_Edge(&outer, color, (menu->drawStyle | 0x20), gGT->backBuffer->otMem.startPlusFour);
	}
	RECTMENU_DrawInnerRect(inner, menu->drawStyle, gGT->backBuffer->otMem.startPlusFour);
}
