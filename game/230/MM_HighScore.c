#include <common.h>

enum
{
	MM_HIGHSCORE_MAIN_TRANSITION_MAX_FRAME = 0xc,
	MM_HIGHSCORE_SLIDE_TRANSITION_FRAMES = 8,
	MM_HIGHSCORE_LAST_ARCADE_TRACK = 0x11,
	MM_HIGHSCORE_TRACK_SLIDE_STEP_X = 0x40,
	MM_HIGHSCORE_ROW_SLIDE_STEP_Y = 0x1b,
	MM_HIGHSCORE_OFFSCREEN_X = 0x200,
	MM_HIGHSCORE_OFFSCREEN_Y = 0xd8,
	MM_HIGHSCORE_WIPE_RECT_W = 0x228,
	MM_HIGHSCORE_WIPE_RECT_H = 0x19,
	MM_HIGHSCORE_WIPE_RECT_X_OFFSET = -0x14,
	MM_HIGHSCORE_WIPE_RECT_Y_OFFSET = 9,
};

// NOTE(aalhendi): ASM-verified against NTSC-U 926 overlay 230 0x800b2f0c-0x800b2fbc.
void MM_HighScore_Text3D(char *string, int posX, int posY, s16 font, u32 flags)
{
	// draw a string
	DecalFont_DrawLine(string, posX, posY, font, flags);

	// draw the same string in a different place
	DecalFont_DrawLine(string, (posX + 3), (posY + 1), font, (flags & (JUSTIFY_CENTER | JUSTIFY_RIGHT)) | BLACK);
}

// NOTE(aalhendi): ASM-verified NTSC-U 926 overlay 230 0x800b2fbc-0x800b3914.
void MM_HighScore_Draw(u16 trackIndex, u32 rowIndex, u32 posX, u32 posY)
{
	s32 i;
	s16 numColor;
	s16 lineWidth;
	u32 *colorPtr;
	s16 levelID;
	struct HighScoreEntry *entry;
	RECT videoBox;
	s16 offsetX;
	s16 offsetY;

	struct GameTracker *gGT = sdata->gGT;

	offsetX = (s16)posX;
	offsetY = (s16)posY;

	levelID = D230.arcadeTracks[trackIndex].levID;

	lineWidth = DecalFont_GetLineWidth(sdata->lngStrings[data.metaDataLEV[levelID].name_LNG], 1);
	lineWidth = lineWidth >> 1;

	// get color data
	numColor = ((sdata->frameCounter & 4) == 0) ? RED : ORANGE;
	colorPtr = data.ptrColor[numColor];

	struct Icon **iconPtrArray = ICONGROUP_GETICONS(gGT->iconGroup[4]);

	// Draw arrow pointing Left
	DecalHUD_Arrow2D(iconPtrArray[0x38], D230.transitionMeta_HighScores[0].currX + (offsetX - lineWidth) + 0xec,
	                 D230.transitionMeta_HighScores[0].currY + offsetY + 0x15, &gGT->backBuffer->primMem, gGT->pushBuffer_UI.ptrOT, colorPtr[0], colorPtr[1],
	                 colorPtr[2], colorPtr[3], 0, 0x1000, 0x800);

	// Draw arrow pointing Right
	DecalHUD_Arrow2D(iconPtrArray[0x38], D230.transitionMeta_HighScores[0].currX + (lineWidth + offsetX) + 0x112,
	                 D230.transitionMeta_HighScores[0].currY + offsetY + 0x15, &gGT->backBuffer->primMem, gGT->pushBuffer_UI.ptrOT, colorPtr[0], colorPtr[1],
	                 colorPtr[2], colorPtr[3], 0, 0x1000, 0);

	// draw track name
	DecalFont_DrawLine(sdata->lngStrings[data.metaDataLEV[levelID].name_LNG], D230.transitionMeta_HighScores[0].currX + (s16)(posX + 0x100),
	                   D230.transitionMeta_HighScores[0].currY + (s16)(posY + 0xe), FONT_BIG, JUSTIFY_CENTER);

	u32 iconColor = D230.highscore_iconColor;

	MM_HighScore_Text3D(sdata->lngStrings[LNG_BEST_TRACK_TIMES], D230.transitionMeta_HighScores[1].currX + offsetX + 0x20,
	                    D230.transitionMeta_HighScores[1].currY + offsetY + 0x2b, FONT_SMALL, 0);

	// first entry: Time Trial or Relic
	entry = &sdata->gameProgress.highScoreTracks[levelID].scoreEntry[rowIndex * 6];

	// if Time Trial
	// with ghost stars, and Best Lap
	if ((rowIndex & 0xffff) == 0)
	{
		int prevLevelID = gGT->levelID;

		gGT->levelID = levelID;
		GAMEPROG_GetPtrHighScoreTrack();

		// draw ghost stars
		for (i = 0; i < 2; i++)
		{
			if (((sdata->gameProgress.highScoreTracks[gGT->levelID].timeTrialFlags >> D230.highscore_ghostBeatFlags[i]) & 1) != 0)
			{
				colorPtr = data.ptrColor[D230.colorIndexArray[i]];

				struct Icon **ptrIconArray;
				ptrIconArray = ICONGROUP_GETICONS(gGT->iconGroup[5]);

				DecalHUD_DrawPolyGT4(ptrIconArray[0x37], D230.transitionMeta_HighScores[0].currX + offsetX + (i * 0x10) + 0xf0,
				                     D230.transitionMeta_HighScores[0].currY + offsetY + 4,
				                     // pointer to PrimMem struct
				                     &gGT->backBuffer->primMem,
				                     // pointer to OT mem
				                     gGT->pushBuffer_UI.ptrOT, colorPtr[0], colorPtr[1], colorPtr[2], colorPtr[3], 0, 0x1000);
			}
		}

		gGT->levelID = prevLevelID;
		GAMEPROG_GetPtrHighScoreTrack();

		MM_HighScore_Text3D(sdata->lngStrings[LNG_BEST_LAP_TIME], D230.transitionMeta_HighScores[7].currX + offsetX + 0x124,
		                    D230.transitionMeta_HighScores[7].currY + offsetY + 0x2b, FONT_SMALL, 0);

		// Character Name
		MM_HighScore_Text3D(entry[0].name, D230.transitionMeta_HighScores[7].currX + offsetX + 0x160, D230.transitionMeta_HighScores[7].currY + offsetY + 0x39,
		                    FONT_BIG, entry[0].characterID + 5);

		// Draw time string
		MM_HighScore_Text3D(RECTMENU_DrawTime(entry[0].time), D230.transitionMeta_HighScores[7].currX + offsetX + 0x160,
		                    D230.transitionMeta_HighScores[7].currY + offsetY + 0x4a, FONT_SMALL, 0);

		// Character Icon
		RECTMENU_DrawPolyGT4(gGT->ptrIcons[data.MetaDataCharacters[entry[0].characterID].iconID], D230.transitionMeta_HighScores[7].currX + (offsetX + 0x124),
		                     D230.transitionMeta_HighScores[7].currY + (offsetY + 0x38), &gGT->backBuffer->primMem, (gGT->pushBuffer_UI).ptrOT, iconColor,
		                     iconColor, iconColor, iconColor, 1, 0x1000);
	}

	// Draw five "best track times"
	// Icon, Name, and Time
	for (i = 0; i < 5; i++)
	{
		s32 j = i + 2;

		// Character Icon
		RECTMENU_DrawPolyGT4(gGT->ptrIcons[data.MetaDataCharacters[entry[i + 1].characterID].iconID], D230.transitionMeta_HighScores[j].currX + offsetX + 0x20,
		                     D230.transitionMeta_HighScores[j].currY + offsetY + (i * 0x1f) + 0x39, &gGT->backBuffer->primMem, gGT->pushBuffer_UI.ptrOT,
		                     iconColor, iconColor, iconColor, iconColor, 1, 0x1000);

		// draw the name string
		MM_HighScore_Text3D(entry[i + 1].name, D230.transitionMeta_HighScores[j].currX + offsetX + 0x5c,
		                    D230.transitionMeta_HighScores[j].currY + offsetY + (i * 0x1f) + 0x39, FONT_BIG, entry[i + 1].characterID + 5);

		// draw the Time string
		MM_HighScore_Text3D(RECTMENU_DrawTime(entry[i + 1].time), D230.transitionMeta_HighScores[j].currX + offsetX + 0x5c,
		                    D230.transitionMeta_HighScores[j].currY + offsetY + (i * 0x1f) + 0x4a, FONT_SMALL, 0);
	}

	videoBox.w = 0xb0;
	videoBox.h = 0x4b;
	videoBox.x = D230.transitionMeta_HighScores[9].currX + offsetX + 0x124;
	videoBox.y = D230.transitionMeta_HighScores[9].currY + offsetY + 0x5a;

	MM_TrackSelect_Video_Draw(&videoBox, &D230.arcadeTracks[0], trackIndex, (D230.highScore_transitionState == EXITING_MENU), 0);
}

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800b3914-0x800b3954.
void MM_HighScore_Init(void)
{
	D230.highScore_transitionState = ENTERING_MENU;
	D230.highScore_transitionFrames[0] = MM_HIGHSCORE_MAIN_TRANSITION_MAX_FRAME;
	D230.highScore_rowDesired = 0;
	D230.highScore_rowCurr = 0;

	// reset all video variables
	MM_TrackSelect_Video_SetDefaults();
}

// NOTE(aalhendi): ASM-verified NTSC-U 926 overlay 230 0x800b3954-0x800b3fe4.
void MM_HighScore_MenuProc(struct RectMenu *menu_unused)
{
	(void)menu_unused;
	u8 videoResetRequested;
	u8 slideReachedTarget;
	s16 nextFrameCount;
	u32 trackOpen;
	s32 menuResult;
	s32 videoState;
	s32 nextOffsetX;
	s32 nextOffsetY;
	s32 currOffsetX;
	s32 currOffsetY;
	RECT wipeRect;

	videoResetRequested = false;
	if (D230.highScore_transitionState != IN_MENU)
	{
		nextFrameCount = D230.highScore_transitionFrames[0];
		if (D230.highScore_transitionState < EXITING_MENU) // entering_menu, in_menu
		{
			if (D230.highScore_transitionState == ENTERING_MENU)
			{
				MM_TransitionInOut(D230.transitionMeta_HighScores, (int)D230.highScore_transitionFrames[0], MM_HIGHSCORE_SLIDE_TRANSITION_FRAMES);
				nextFrameCount = D230.highScore_transitionFrames[0] + -1;
				if (D230.highScore_transitionFrames[0] == 0)
				{
					D230.highScore_transitionState = IN_MENU;
					nextFrameCount = D230.highScore_transitionFrames[0];
				}
			}
		}
		else if (((D230.highScore_transitionState == EXITING_MENU) && (D230.highScore_transitionFrames[1] == 0)) && (D230.highScore_transitionFrames[2] == 0))
		{
			MM_TransitionInOut(D230.transitionMeta_HighScores, (int)D230.highScore_transitionFrames[0], MM_HIGHSCORE_SLIDE_TRANSITION_FRAMES);
			D230.highScore_transitionFrames[0] = D230.highScore_transitionFrames[0] + 1;
			nextFrameCount = D230.highScore_transitionFrames[0];
			if (MM_HIGHSCORE_MAIN_TRANSITION_MAX_FRAME < D230.highScore_transitionFrames[0])
			{
				MM_JumpTo_Title_Returning();
				return;
			}
		}
		D230.highScore_transitionFrames[0] = nextFrameCount;
		if (D230.highScore_transitionState != IN_MENU)
		{
			goto LAB_OVR_230__800b3c78;
		}
	}
	if ((sdata->buttonTapPerPlayer[0] & 1) == 0)
	{
		if (((sdata->buttonTapPerPlayer[0] & 2) != 0) && (D230.menuHighScore.rowSelected < 1))
		{
			videoResetRequested = true;
		}
	}
	else if (D230.menuHighScore.rowSelected == 1)
	{
		videoResetRequested = true;
	}
	// if player didn't press any of the "back" buttons
	if ((sdata->buttonTapPerPlayer[0] & (BTN_SQUARE_one | BTN_TRIANGLE)) == 0)
	{
		if ((sdata->buttonTapPerPlayer[0] & BTN_LEFT) == 0)
		{
			if ((sdata->buttonTapPerPlayer[0] & BTN_RIGHT) == 0)
			{
				menuResult = RECTMENU_ProcessInput(&D230.menuHighScore);
				if ((s16)menuResult == -1)
				{
					D230.highScore_transitionState = EXITING_MENU;
				}
				else if (((s16)menuResult == 1) && (D230.menuHighScore.rowSelected == 2))
				{
					D230.highScore_transitionState = D230.menuHighScore.rowSelected;
				}
				if (((u16)D230.menuHighScore.rowSelected < 2) && (D230.highScore_rowDesired != D230.menuHighScore.rowSelected))
				{
					D230.highScore_verticalMove[1] = -1;
					if (D230.menuHighScore.rowSelected != 0)
					{
						D230.highScore_verticalMove[1] = 1;
					}
					D230.highScore_rowDesired = D230.menuHighScore.rowSelected;
				}
			}
			else
			{
				videoResetRequested = true;
				D230.highScore_horizontalMove[1] = 1;
				do
				{
					D230.highScore_trackDesired = D230.highScore_trackDesired + 1;
					if (MM_HIGHSCORE_LAST_ARCADE_TRACK < D230.highScore_trackDesired)
					{
						D230.highScore_trackDesired = 0;
					}
					trackOpen = MM_TrackSelect_boolTrackOpen(D230.arcadeTracks + D230.highScore_trackDesired);
				} while ((trackOpen & 0xffff) == 0);
			}
		}
		else
		{
			videoResetRequested = true;
			D230.highScore_horizontalMove[1] = -1;
			do
			{
				D230.highScore_trackDesired = D230.highScore_trackDesired - 1;
				if (D230.highScore_trackDesired < 0)
				{
					D230.highScore_trackDesired = MM_HIGHSCORE_LAST_ARCADE_TRACK;
				}
				trackOpen = MM_TrackSelect_boolTrackOpen(D230.arcadeTracks + D230.highScore_trackDesired);
			} while ((trackOpen & 0xffff) == 0);
		}
	}
	else
	{
		videoResetRequested = true;
		// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800b3ad8-0x800b3ae4 for high-score back SFX.
		OtherFX_Play(2, 1);
		D230.highScore_transitionState = EXITING_MENU;
	}

LAB_OVR_230__800b3c78:

	videoState = 0;
	if ((((videoResetRequested) || (D230.highScore_transitionFrames[1] != 0)) || (D230.highScore_transitionFrames[2] != 0)) ||
	    (D230.highScore_transitionState == EXITING_MENU))
	{
		videoState = 1;
	}

	MM_TrackSelect_Video_State(videoState);
	nextFrameCount = D230.highScore_transitionFrames[1] + -1;
	if (D230.highScore_transitionFrames[1] == 0)
	{
		nextFrameCount = D230.highScore_transitionFrames[2] + -1;
		if (D230.highScore_transitionFrames[2] == 0)
		{
			if (D230.highScore_trackCurr == D230.highScore_trackDesired)
			{
				if (D230.highScore_rowCurr != D230.highScore_rowDesired)
				{
					D230.highScore_transitionFrames[2] = MM_HIGHSCORE_SLIDE_TRANSITION_FRAMES;
					D230.highScore_verticalMove[0] = D230.highScore_verticalMove[1];
				}
			}
			else
			{
				D230.highScore_transitionFrames[1] = MM_HIGHSCORE_SLIDE_TRANSITION_FRAMES;
				D230.highScore_horizontalMove[0] = D230.highScore_horizontalMove[1];
			}
		}
		else
		{
			slideReachedTarget = D230.highScore_transitionFrames[2] == 1;
			D230.highScore_transitionFrames[2] = nextFrameCount;
			if (slideReachedTarget)
			{
				D230.highScore_rowCurr = D230.highScore_rowDesired;
			}
		}
	}
	else
	{
		slideReachedTarget = D230.highScore_transitionFrames[1] == 1;
		D230.highScore_transitionFrames[1] = nextFrameCount;
		if (slideReachedTarget)
		{
			D230.highScore_trackCurr = D230.highScore_trackDesired;
		}
	}

	RECTMENU_DrawSelf(&D230.menuHighScore, D230.transitionMeta_HighScores[10].currX, D230.transitionMeta_HighScores[10].currY, 0xa4);

	currOffsetY = 0;
	currOffsetX = 0;

	if (D230.highScore_transitionFrames[1] == 0)
	{
		currOffsetY =
		    (MM_HIGHSCORE_SLIDE_TRANSITION_FRAMES - D230.highScore_transitionFrames[2]) * D230.highScore_verticalMove[0] * MM_HIGHSCORE_ROW_SLIDE_STEP_Y;
	}

	else
	{
		currOffsetX =
		    (MM_HIGHSCORE_SLIDE_TRANSITION_FRAMES - D230.highScore_transitionFrames[1]) * D230.highScore_horizontalMove[0] * MM_HIGHSCORE_TRACK_SLIDE_STEP_X;
	}

	u32 *ot = sdata->gGT->backBuffer->otMem.uiOT;

	if (((currOffsetX != -MM_HIGHSCORE_OFFSCREEN_X) && (currOffsetX != MM_HIGHSCORE_OFFSCREEN_X)) &&
	    ((currOffsetY != -MM_HIGHSCORE_OFFSCREEN_Y && (currOffsetY != MM_HIGHSCORE_OFFSCREEN_Y))))
	{
		MM_HighScore_Draw(D230.highScore_trackCurr, (int)D230.highScore_rowCurr, (int)(s16)currOffsetX, (int)(s16)currOffsetY);
		if (D230.highScore_transitionFrames[2] != 0)
		{
			// draw rectangle
			wipeRect.w = MM_HIGHSCORE_WIPE_RECT_W;
			wipeRect.h = MM_HIGHSCORE_WIPE_RECT_H;
			wipeRect.x = D230.transitionMeta_HighScores[0].currX + MM_HIGHSCORE_WIPE_RECT_X_OFFSET;
			wipeRect.y = D230.transitionMeta_HighScores[0].currY + (s16)currOffsetY + MM_HIGHSCORE_WIPE_RECT_Y_OFFSET;
			RECTMENU_DrawInnerRect(&wipeRect, 0, ot);
		}
	}
	nextOffsetX = 0;
	nextOffsetY = 0;
	if (D230.highScore_transitionFrames[1] == 0)
	{
		nextOffsetY = D230.highScore_transitionFrames[2] * -MM_HIGHSCORE_ROW_SLIDE_STEP_Y * (int)D230.highScore_verticalMove[0];
	}
	else
	{
		nextOffsetX = D230.highScore_transitionFrames[1] * -MM_HIGHSCORE_TRACK_SLIDE_STEP_X * (int)D230.highScore_horizontalMove[0];
	}
	if (((currOffsetX != nextOffsetX) || (currOffsetY != nextOffsetY)) &&
	    ((nextOffsetX != -MM_HIGHSCORE_OFFSCREEN_X &&
	      (((nextOffsetX != MM_HIGHSCORE_OFFSCREEN_X && (nextOffsetY != -MM_HIGHSCORE_OFFSCREEN_Y)) && (nextOffsetY != MM_HIGHSCORE_OFFSCREEN_Y))))))
	{
		MM_HighScore_Draw(D230.highScore_trackDesired, (int)D230.highScore_rowDesired, (int)(s16)nextOffsetX, (int)(s16)nextOffsetY);
	}

	// draw rectangle
	wipeRect.w = MM_HIGHSCORE_WIPE_RECT_W;
	wipeRect.h = MM_HIGHSCORE_WIPE_RECT_H;
	wipeRect.x = D230.transitionMeta_HighScores[0].currX + MM_HIGHSCORE_WIPE_RECT_X_OFFSET;
	wipeRect.y = D230.transitionMeta_HighScores[0].currY + (s16)nextOffsetY + MM_HIGHSCORE_WIPE_RECT_Y_OFFSET;
	RECTMENU_DrawInnerRect(&wipeRect, 0, ot);

	return;
}
