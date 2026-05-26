#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x80048a30-0x80048da0.
void SelectProfile_DrawGhostProfile(struct GhostProfile *profile, int posX, int posY, u32 isHighlighted, int unused, u16 menuFlag, s16 isLoading,
                                    s16 isUnavailable)
{
	struct GameTracker *gGT = sdata->gGT;
	RECT profileRect;
	RECT innerRect;

	(void)unused;

	profileRect.x = posX;
	profileRect.y = posY;
	profileRect.w = 0xc8;
	profileRect.h = 0x29;

	innerRect.x = posX + 6;
	innerRect.y = posY + 3;
	innerRect.w = 0xbc;
	innerRect.h = 0x23;

	if (isUnavailable != 0)
	{
		DecalFont_DrawLine(sdata->lngStrings[0x6d], posX + 0x64, posY + 0x11, FONT_SMALL, 0xffff8016);
		CTR_Box_DrawClearBox(&innerRect, (Color *)&sdata->redColor, ADD_DECAL, gGT->backBuffer->otMem.startPlusFour);
	}

	if (profile != NULL)
	{
		struct MetaDataLEV *mdLev = &data.metaDataLEV[profile->trackID];
		int iconID = data.MetaDataCharacters[profile->characterID].iconID;

		DecalFont_DrawLine(sdata->lngStrings[mdLev->name_LNG], posX + 0x64, posY + 0x1e, FONT_SMALL, 0xffff801d);
		DecalFont_DrawLine(RECTMENU_DrawTime(profile->trackTime), posX + 0x78, posY + 10, FONT_BIG, 0xffff8001);
		RECTMENU_DrawPolyGT4(gGT->ptrIcons[iconID], posX + 8, posY + 5, &gGT->backBuffer->primMem, gGT->backBuffer->otMem.startPlusFour, sdata->ghostIconColor,
		                     sdata->ghostIconColor, sdata->ghostIconColor, sdata->ghostIconColor, TRANS_50_DECAL, 0x1000);
	}
	else
	{
		int lngIndex = (isLoading != 0) ? 0x6c : 0xb5;
		int color = (isLoading != 0) ? 0xffff8001 : 0xffff8003;

		DecalFont_DrawLine(sdata->lngStrings[lngIndex], posX + 0x64, posY + 0x11, FONT_SMALL, color);
	}

	if (isHighlighted != 0)
	{
		Color *highlight = ((menuFlag & 0x10) != 0) ? &sdata->menuRowHighlight_Green : &sdata->menuRowHighlight_Normal;
		CTR_Box_DrawClearBox(&innerRect, highlight, TRANS_50_DECAL, gGT->backBuffer->otMem.startPlusFour);
	}

	RECTMENU_DrawInnerRect(&profileRect, (s16)menuFlag, gGT->backBuffer->otMem.startPlusFour);
}
