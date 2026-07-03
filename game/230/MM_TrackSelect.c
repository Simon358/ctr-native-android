#include <common.h>

enum TrackSelectVideoState
{
	MM_TRACK_VIDEO_ICON = 1,
	MM_TRACK_VIDEO_START_STREAM = 2,
	MM_TRACK_VIDEO_PLAYING = 3,
};

enum
{
	MM_TRACK_VIDEO_PREVIEW_WAIT_FRAMES = 21,
	MM_TRACK_VIDEO_WIDTH = 0xb0,
	MM_TRACK_VIDEO_HEIGHT = 0x4b,
	MM_TRACK_VIDEO_FRAME_SRC_OFFSET_X = 3,
	MM_TRACK_VIDEO_FRAME_SRC_OFFSET_Y = 2,
	MM_TRACK_VIDEO_FRAME_WIDTH = 0xaa,
	MM_TRACK_VIDEO_FRAME_HEIGHT = 0x47,
	MM_TRACK_VIDEO_FLAGS = MM_VIDEO_FLAG_LOOP,
	MM_TRACK_VIDEO_ICON_INDEX = 0x3f,
	MM_TRACK_VIDEO_SCREEN_W = 0x200,
	MM_TRACK_VIDEO_SCREEN_H = 0xd8,
	MM_TRACK_SELECT_BATTLE_TRACK_COUNT = 7,
	MM_TRACK_SELECT_ARCADE_TRACK_COUNT = 18,
	MM_TRACK_SELECT_TRANSITION_FRAMES = 12,
	MM_TRACK_SELECT_SLIDE_FRAMES = 8,
	MM_TRACK_SELECT_TRACK_CHANGE_FRAMES = 3,
	MM_TRACK_SELECT_CENTER_ROW = 4,
	MM_TRACK_SELECT_VISIBLE_ROWS = 9,
	MM_TRACK_SELECT_ROW_ANGLE_STEP = 0x73,
	MM_TRACK_SELECT_ROW_W = 0x100,
	MM_TRACK_SELECT_ROW_H = 0x19,
	MM_TRACK_SELECT_ROW_X_RADIUS = 0x19,
	MM_TRACK_SELECT_ROW_X_SHIFT = 9,
	MM_TRACK_SELECT_ROW_X_OFFSET = -0xb4,
	MM_TRACK_SELECT_ROW_Y_RADIUS = 200,
	MM_TRACK_SELECT_ROW_Y_SHIFT = 0xc,
	MM_TRACK_SELECT_ROW_Y_OFFSET = 0x60,
	MM_TRACK_SELECT_TT_STAR_COUNT = 2,
	MM_TRACK_SELECT_TT_STAR_ICON = 0x37,
	MM_TRACK_SELECT_MAP_LAYER_COUNT = 6,
	MM_TRACK_SELECT_MAP_BOX_H = 100,
	MM_TRACK_SELECT_PREVIEW_X = 0x134,
	MM_TRACK_SELECT_PREVIEW_Y_WITH_MAP = 5,
	MM_TRACK_SELECT_PREVIEW_Y_NO_MAP = 0x3a,
};

CTR_STATIC_ASSERT(MM_TRACK_VIDEO_ICON == 1);
CTR_STATIC_ASSERT(MM_TRACK_VIDEO_PLAYING == 3);

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800afa44-0x800afa94.
void MM_TrackSelect_Video_SetDefaults(void)
{
	// clear RECT
	sdata->videoSTR_src_vramRect.x = 0;
	sdata->videoSTR_src_vramRect.y = 0;
	sdata->videoSTR_src_vramRect.w = 0;
	sdata->videoSTR_src_vramRect.h = 0;

	// VRAM destination
	sdata->videoSTR_dst_vramX = 0;
	sdata->videoSTR_dst_vramY = 0;

	// track icon has been viewed for zero frames
	D230.trackSel_videoPreviewFrames = 0;

	// Data is not allocated for TrackSel videos
	D230.trackSel_videoMemAllocated = 0;

	D230.trackSel_videoStateCurr = MM_TRACK_VIDEO_ICON;
	D230.trackSel_videoStatePrev = MM_TRACK_VIDEO_ICON;
}

// NOTE(aalhendi): ASM-verified against NTSC-U 926 overlay 230 0x800afa94-0x800afaf0.
void MM_TrackSelect_Video_State(int resetPreview)
{
	// if viewing new icon this frame
	if (resetPreview == 1)
	{
		// icon has been viewed for zero frames
		D230.trackSel_videoPreviewFrames = 0;

		// player sees a track icon (not video)
		D230.trackSel_videoStateCurr = MM_TRACK_VIDEO_ICON;

		return;
	}

	// if player sees a track icon
	if (D230.trackSel_videoStateCurr == MM_TRACK_VIDEO_ICON)
	{
		// wait before starting the preview video
		if (D230.trackSel_videoPreviewFrames < MM_TRACK_VIDEO_PREVIEW_WAIT_FRAMES)
		{
			D230.trackSel_videoPreviewFrames++;
		}
		else
		{
			// allocate video memory, prepare to play video
			D230.trackSel_videoStateCurr = MM_TRACK_VIDEO_START_STREAM;
		}
	}
}

#ifdef CTR_NATIVE
#include <platform/native_str.h>

static void MM_TrackSelect_Video_DrawNativePreview(RECT *r, int srcX, int srcY)
{
	struct GameTracker *gGT = sdata->gGT;
	u32 *prim = (u32 *)gGT->backBuffer->primMem.cursor;
	u32 *ot = gGT->pushBuffer_UI.ptrOT;
	u32 oldTag = (u32)*ot;
	u32 *nextPrim;
	struct DisplayBlurTile tile[2] = {
	    {
	        .srcX = (s16)srcX,
	        .srcY = (s16)srcY,
	        .srcW = MM_TRACK_VIDEO_FRAME_WIDTH,
	        .srcH = MM_TRACK_VIDEO_FRAME_HEIGHT,
	        .dstX = (s16)(r->x + MM_TRACK_VIDEO_FRAME_SRC_OFFSET_X),
	        .dstY = (s16)(r->y + MM_TRACK_VIDEO_FRAME_SRC_OFFSET_Y),
	        .dstW = MM_TRACK_VIDEO_FRAME_WIDTH,
	        .dstH = MM_TRACK_VIDEO_FRAME_HEIGHT,
	    },
	};

	// NOTE(aalhendi): Retail copies decoded MDEC output with MoveImage. Native
	// presents menus from queued primitives, so draw the same VRAM rectangle as
	// a 16-bit textured quad instead of relying on a CPU-side VRAM copy.
	*ot = (u32)CtrGpu_PrimToOTLink24(prim);
	nextPrim = DISPLAY_Blur_SubFunc(prim, &tile[0]);
	((POLY_FT4 *)nextPrim - 1)->tag = CtrGpu_PackOTTag(oldTag, 0x09000000);
	gGT->backBuffer->primMem.cursor = nextPrim;
}
#endif

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800afaf0-0x800aff58 PSX path.
void MM_TrackSelect_Video_Draw(RECT *r, struct MainMenu_LevelRow *selectMenu, int trackIndex, int stopVideo, u16 rectFlags)
{
	u8 u0;
	u8 v0;
	u16 tpage;
	int srcX;
	int srcY;
	struct GameTracker *gGT = sdata->gGT;
	struct BigHeader *bh = sdata->ptrBigfileCdPos_2;
	struct BigEntry *entry = BIG_GETENTRY(bh);
	int videoID;

	selectMenu = &selectMenu[trackIndex];
	videoID = selectMenu->videoID;

	if ((entry[videoID].size == 0) ||

	    // Video off-screen
	    (r->x < 0) || (r->y < 0) || ((r->x + r->w) > MM_TRACK_VIDEO_SCREEN_W) || ((r->y + r->h) > MM_TRACK_VIDEO_SCREEN_H))
	{
		// draw icon
		D230.trackSel_videoStateCurr = MM_TRACK_VIDEO_ICON;
	}
#ifdef CTR_NATIVE
	else
	{
		if ((D230.trackSel_videoStateCurr == MM_TRACK_VIDEO_START_STREAM) && (D230.trackSel_videoStatePrev == MM_TRACK_VIDEO_ICON))
		{
			if (NativeSTR_StartTrackPreviewFromBigfileSector(entry[videoID].offset, selectMenu->videoLength) != 0)
			{
				D230.trackSel_videoMemAllocated = D230.trackSel_videoStatePrev;
			}
		}

		if (((D230.trackSel_videoStatePrev == MM_TRACK_VIDEO_PLAYING) || (D230.trackSel_videoStateCurr == MM_TRACK_VIDEO_PLAYING)) ||
		    (D230.trackSel_videoStateCurr == MM_TRACK_VIDEO_START_STREAM))
		{
			int uploaded;

			tpage = gGT->ptrIcons[MM_TRACK_VIDEO_ICON_INDEX]->texLayout.tpage;
			u0 = gGT->ptrIcons[MM_TRACK_VIDEO_ICON_INDEX]->texLayout.u0;
			v0 = gGT->ptrIcons[MM_TRACK_VIDEO_ICON_INDEX]->texLayout.v0;
			srcX = (u16)u0 + (tpage & 0xf) * 0x40;
			srcY = (u16)v0 + (tpage & 0x10) * 0x10 + (s16)(((u32)tpage & 0x800) >> 2);
			uploaded = NativeSTR_UploadNextFrame(srcX, srcY);

			if ((uploaded == 1) && (D230.trackSel_videoStateCurr == MM_TRACK_VIDEO_START_STREAM))
			{
				D230.trackSel_videoStateCurr = MM_TRACK_VIDEO_PLAYING;
			}

			if (D230.trackSel_videoStateCurr == MM_TRACK_VIDEO_PLAYING)
			{
				MM_TrackSelect_Video_DrawNativePreview(r, srcX + MM_TRACK_VIDEO_FRAME_SRC_OFFSET_X, srcY + MM_TRACK_VIDEO_FRAME_SRC_OFFSET_Y);
			}
		}
	}
#else
	else
	{
		if ((D230.trackSel_videoStateCurr == MM_TRACK_VIDEO_START_STREAM) && (D230.trackSel_videoStatePrev == MM_TRACK_VIDEO_ICON))
		{
			// If you have not allocated memory for video yet
			if (D230.trackSel_videoMemAllocated == 0)
			{
				// Allocate memory for video in Track Selection
				MM_Video_AllocMem(MM_TRACK_VIDEO_WIDTH, MM_TRACK_VIDEO_HEIGHT, MM_TRACK_VIDEO_FLAGS, 0, 0);

				// You have now allocated the memory
				D230.trackSel_videoMemAllocated = D230.trackSel_videoStatePrev;
			}

			// CD position of video, and numFrames
			MM_Video_StartStream(bh->cdpos + entry[videoID].offset, selectMenu->videoLength);
		}

		if (((D230.trackSel_videoStatePrev == MM_TRACK_VIDEO_PLAYING) || (D230.trackSel_videoStateCurr == MM_TRACK_VIDEO_PLAYING)) ||
		    (D230.trackSel_videoStateCurr == MM_TRACK_VIDEO_START_STREAM))
		{
			tpage = gGT->ptrIcons[MM_TRACK_VIDEO_ICON_INDEX]->texLayout.tpage;
			u0 = gGT->ptrIcons[MM_TRACK_VIDEO_ICON_INDEX]->texLayout.u0;
			v0 = gGT->ptrIcons[MM_TRACK_VIDEO_ICON_INDEX]->texLayout.v0;
			srcX = (u16)u0 + (tpage & 0xf) * 0x40;
			srcY = (u16)v0 + (tpage & 0x10) * 0x10 + (s16)(((u32)tpage & 0x800) >> 2);

			// Decode into the icon's VRAM page; the copied rectangle starts inside it.
			int decodedFrame = MM_Video_DecodeFrame(srcX, srcY);

			if ((decodedFrame == 1) && (D230.trackSel_videoStateCurr == MM_TRACK_VIDEO_START_STREAM))
			{
				D230.trackSel_videoStateCurr = MM_TRACK_VIDEO_PLAYING;
			}
			if (D230.trackSel_videoStatePrev == MM_TRACK_VIDEO_PLAYING)
			{
				// RECT position (x,y)
				sdata->videoSTR_src_vramRect.x = srcX + MM_TRACK_VIDEO_FRAME_SRC_OFFSET_X;
				sdata->videoSTR_src_vramRect.y = srcY + MM_TRACK_VIDEO_FRAME_SRC_OFFSET_Y;

				// RECT size (w,h)
				sdata->videoSTR_src_vramRect.w = MM_TRACK_VIDEO_FRAME_WIDTH;
				sdata->videoSTR_src_vramRect.h = MM_TRACK_VIDEO_FRAME_HEIGHT;

				// VRAM destination (x,y) on swapchain image
				sdata->videoSTR_dst_vramX = gGT->db[gGT->swapchainIndex].dispEnv.disp.x + (r->x + MM_TRACK_VIDEO_FRAME_SRC_OFFSET_X);
				sdata->videoSTR_dst_vramY = gGT->db[gGT->swapchainIndex].dispEnv.disp.y + (r->y + MM_TRACK_VIDEO_FRAME_SRC_OFFSET_Y);

				// enable video copy, give src and dst
				MainFrame_InitVideoSTR(1, &sdata->videoSTR_src_vramRect, sdata->videoSTR_dst_vramX, sdata->videoSTR_dst_vramY);
			}
		}
	}
#endif

	// if not playing video, draw icon
	if (D230.trackSel_videoStateCurr != MM_TRACK_VIDEO_PLAYING)
	{
		// Draw Video icon
		RECTMENU_DrawPolyGT4(gGT->ptrIcons[selectMenu->videoThumbnail], (r->x + MM_TRACK_VIDEO_FRAME_SRC_OFFSET_X), (r->y + MM_TRACK_VIDEO_FRAME_SRC_OFFSET_Y),
		                     &gGT->backBuffer->primMem, gGT->pushBuffer_UI.ptrOT, D230.videoCol, D230.videoCol, D230.videoCol, D230.videoCol, 0, FP(1.0));
	}

#ifndef CTR_NATIVE
	if (D230.trackSel_videoStatePrev == MM_TRACK_VIDEO_ICON)
	{
		// disable video copy
		MainFrame_InitVideoSTR(0, 0, 0, 0);
	}

	// First frame to start video
	if ((stopVideo == 1) && (D230.trackSel_videoMemAllocated == 1))
	{
		D230.trackSel_videoStateCurr = MM_TRACK_VIDEO_ICON;
	}

	// First frame to stop video
	if ((D230.trackSel_videoStateCurr == MM_TRACK_VIDEO_ICON) && (D230.trackSel_videoStatePrev != MM_TRACK_VIDEO_ICON))
	{
		MM_Video_StopStream();
	}

	// First frame to start video,
	// but this time after stopping video safely
	if ((stopVideo == 1) && (D230.trackSel_videoMemAllocated == 1))
	{
		MM_Video_ClearMem();

		D230.trackSel_videoMemAllocated = 0;
	}
#else
	if (D230.trackSel_videoStatePrev == MM_TRACK_VIDEO_ICON)
	{
		MainFrame_InitVideoSTR(0, 0, 0, 0);
	}

	if ((stopVideo == 1) && (D230.trackSel_videoMemAllocated == 1))
	{
		D230.trackSel_videoStateCurr = MM_TRACK_VIDEO_ICON;
	}

	if ((D230.trackSel_videoStateCurr == MM_TRACK_VIDEO_ICON) && (D230.trackSel_videoStatePrev != MM_TRACK_VIDEO_ICON))
	{
		NativeSTR_Stop();
	}

	if ((stopVideo == 1) && (D230.trackSel_videoMemAllocated == 1))
	{
		NativeSTR_Stop();
		D230.trackSel_videoMemAllocated = 0;
	}
#endif

	D230.trackSel_videoStatePrev = D230.trackSel_videoStateCurr;

	// Draw 2D Menu rectangle background
	RECTMENU_DrawInnerRect(r, (s16)(rectFlags | 1), gGT->backBuffer->otMem.uiOT);
}

// NOTE(aalhendi): ASM-verified against NTSC-U 926 overlay 230 0x800aff58-0x800affd0.
char MM_TrackSelect_boolTrackOpen(struct MainMenu_LevelRow *menuSelect)
{
	s16 flag = menuSelect->unlock;

	if (flag == -1)
	{
		return true;
	}

	if (flag == -2)
	{
		return sdata->gGT->numPlyrNextGame == 1;
	}

	if (flag < 0)
	{
		return false;
	}

	return (sdata->gameProgress.unlocks[flag >> 5] >> (flag & 0x1f)) & 1;
}

// NOTE(aalhendi): ASM-verified against NTSC-U 926 overlay 230 0x800affd0-0x800b00d4.
void MM_TrackSelect_Init(void)
{
	struct MainMenu_LevelRow *selectMenu;
	s16 numTracks;

	// lap selection menu is closed by default
	D230.trackSel_lapBoxOpen = false;
	D230.trackSel_transitionState = ENTERING_MENU;

	// set track index to the index selected in track selection menu, starts at 0 for both Arcade and Battle
	D230.menuTrackSelect.rowSelected = sdata->trackSelBackup;

	// 12 frames when moving between selection
	D230.trackSel_transitionFrames = MM_TRACK_SELECT_TRANSITION_FRAMES;

	// Set menu and num of tracks based on game mode
	if ((sdata->gGT->gameMode1 & BATTLE_MODE) != 0)
	{
		selectMenu = D230.battleTracks;
		numTracks = MM_TRACK_SELECT_BATTLE_TRACK_COUNT;
	}
	else
	{
		selectMenu = D230.arcadeTracks;
		numTracks = MM_TRACK_SELECT_ARCADE_TRACK_COUNT;
	}

	// If you scroll past the max number of tracks, go back to the first track
	if (numTracks <= sdata->trackSelBackup)
	{
		D230.menuTrackSelect.rowSelected = 0;
	}

	// Loop through all tracks until an unlocked track is found
	while (!MM_TrackSelect_boolTrackOpen(&selectMenu[D230.menuTrackSelect.rowSelected]))
	{
		D230.menuTrackSelect.rowSelected++;

		// If track index goes too high, reset to zero
		if (numTracks <= D230.menuTrackSelect.rowSelected)
		{
			D230.menuTrackSelect.rowSelected = 0;
		}
	}

	D230.trackSel_currTrack = D230.menuTrackSelect.rowSelected;

	MM_TrackSelect_Video_SetDefaults();
}

void MM_TrackSelect_MenuProc(struct RectMenu *menu)
{
	s16 currTrack;
	s16 elapsedFrames;
	s16 numTracks;

	struct MainMenu_LevelRow *selectMenu;
	struct GameTracker *gGT = sdata->gGT;

	elapsedFrames = D230.trackSel_transitionFrames;

	// NOTE(aalhendi): ASM-verified NTSC-U 926 overlay 230 0x800b00d4-0x800b02b0.
	// if you are not in track selection menu
	if (D230.trackSel_transitionState != IN_MENU)
	{
		// if transitioning in
		if (D230.trackSel_transitionState == ENTERING_MENU)
		{
			// make error message posY appear
			// near bottom of screen
			sdata->errorMessagePosIndex = 1;

			// if you are in Battle mode
			if ((gGT->gameMode1 & BATTLE_MODE) != 0)
			{
				// make error message posY appear
				// near top of screen
				sdata->errorMessagePosIndex = 2;
			}

			MM_TransitionInOut(&D230.transitionMeta_trackSel[0], elapsedFrames, MM_TRACK_SELECT_SLIDE_FRAMES);

			// ran out of frames
			if (elapsedFrames == 0)
			{
				// menu is now in focus
				D230.trackSel_transitionState = IN_MENU;
			}
			else
			{
				elapsedFrames--;
			}
		}
		// transitioning out
		else if (D230.trackSel_transitionState == EXITING_MENU)
		{
			MM_TransitionInOut(&D230.transitionMeta_trackSel[0], elapsedFrames, MM_TRACK_SELECT_SLIDE_FRAMES);
			elapsedFrames++;

			if (elapsedFrames > MM_TRACK_SELECT_TRANSITION_FRAMES)
			{
				sdata->errorMessagePosIndex = 0;

				// if track has not been chosen
				if (D230.trackSel_startRaceAfterFadeOut == 0)
				{
					// return to character selection
					sdata->ptrDesiredMenu = &D230.menuCharacterSelect;
					MM_Characters_RestoreIDs();
					return;
				}

				// if track has been chosen

				// if you are in battle mode
				if ((gGT->gameMode1 & BATTLE_MODE) != 0)
				{
					// open weapon selection menu
					sdata->ptrDesiredMenu = &D230.menuBattleWeapons;
					MM_Battle_Init();
					return;
				}

				// if you are not in battle mode

				// if you are in time trial mode
				if ((gGT->gameMode1 & TIME_TRIAL) != 0)
				{
					// allocate room at the end of RAM for ghosts
					sdata->ptrGhostTapePlaying = MEMPACK_AllocHighMem(0x3e00 /*, R230.s_loaded_ghost_data*/);

					memset(sdata->ptrGhostTapePlaying, 0, 0x28);

					// by default, dont show ghost in race
					sdata->boolReplayHumanGhost = 0;

					SelectProfile_ToggleMode(SELECT_PROFILE_SCREEN_GHOST);

					// open the ghost selection menu
					sdata->ptrDesiredMenu = &data.menuGhostSelection;
					return;
				}

				// passthrough Menu for the function
				// QueueLoadTrack
				sdata->ptrDesiredMenu = &data.menuQueueLoadTrack;

				// make error message posY appear
				// near middle of screen
				sdata->errorMessagePosIndex = 0;

				return;
			}
		}
	}
	D230.trackSel_transitionFrames = elapsedFrames;

	// default arcade tracks
	selectMenu = &D230.arcadeTracks[0];
	numTracks = MM_TRACK_SELECT_ARCADE_TRACK_COUNT;

	// if you are in battle mode
	if ((gGT->gameMode1 & BATTLE_MODE) != 0)
	{
		selectMenu = &D230.battleTracks[0];
		numTracks = MM_TRACK_SELECT_BATTLE_TRACK_COUNT;
	}

	currTrack = menu->rowSelected;
	sdata->trackSelBackup = currTrack;

	// NOTE(aalhendi): ASM-verified NTSC-U 926 overlay 230 0x800b02b0-0x800b04c8.
	// if lap selection menu is closed
	if (D230.trackSel_lapBoxOpen == 0)
	{
		int importantButton = sdata->buttonTapPerPlayer[0] & (BTN_UP | BTN_DOWN | BTN_TRIANGLE | BTN_SQUARE_one | BTN_CROSS_one | BTN_CIRCLE);

		if (
		    // if not changing levels
		    (D230.trackSel_trackChangeFrames == 0) &&

		    // only check buttons if IN_MENU
		    (D230.trackSel_transitionState == IN_MENU) &&

		    // desired button pressed
		    (importantButton != 0))
		{
			switch (importantButton)
			{
			case BTN_UP:

				// look for unlocked track
				do
				{
					currTrack--;

					// if index is negative
					if (currTrack < 0)
					{
						// set to the last track
						currTrack = numTracks - 1;
					}

				} while (!MM_TrackSelect_boolTrackOpen(&selectMenu[currTrack]));

				D230.trackSel_currTrack = currTrack;
				D230.trackSel_trackChangeFrames = MM_TRACK_SELECT_TRACK_CHANGE_FRAMES;
				D230.trackSel_trackChangeDirection = 1;

				// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800b034c-0x800b035c for track-select previous SFX.
				OtherFX_Play(0, 1);
				break;

			case BTN_DOWN:

				// look for unlocked track
				do
				{
					currTrack++;

					// if you go beyond max number of tracks
					if (currTrack >= numTracks)
					{
						// set to the first track
						currTrack = 0;
					}

				} while (!MM_TrackSelect_boolTrackOpen(&selectMenu[currTrack]));

				D230.trackSel_currTrack = currTrack;
				D230.trackSel_trackChangeFrames = MM_TRACK_SELECT_TRACK_CHANGE_FRAMES;
				D230.trackSel_trackChangeDirection = -1;

				// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800b03bc-0x800b03cc for track-select next SFX.
				OtherFX_Play(0, 1);
				break;

			case BTN_CROSS_one:
			case BTN_CIRCLE:

				// "enter/confirm" sound
				// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800b0434-0x800b0444 for track-select confirm SFX.
				OtherFX_Play(1, 1);

				// if not Battle or Time Trial, open LapSelectMenu
				if ((gGT->gameMode1 & (BATTLE_MODE | TIME_TRIAL)) == 0)
				{
					// open lap select menu
					D230.trackSel_lapBoxOpen = D230.trackSel_transitionState;
					break;
				}

				// if Battle or Time Trial, skip straight to level
				D230.trackSel_startRaceAfterFadeOut = D230.trackSel_transitionState;
				D230.trackSel_transitionState = EXITING_MENU;
				break;

			case BTN_TRIANGLE:
			case BTN_SQUARE_one:

				// "go back" sound
				// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800b0490-0x800b04a4 for track-select back SFX.
				OtherFX_Play(2, 1);

				D230.trackSel_startRaceAfterFadeOut = 0;
				D230.trackSel_transitionState = EXITING_MENU;
				break;
			default:
				break;
			}

			// clear gamepad input (for menus)
			RECTMENU_ClearInput();
		}
	}

	// if lap selection menu is open
	else
	{
		s16 lapSelTransitionState = 0;

		// copy LapRow from 8d920 to temp variable b55ae
		D230.menuLapSel.rowSelected = sdata->uselessLapRowCopy;

		// If you're in track selection menu
		if (D230.trackSel_transitionState == IN_MENU)
		{
			lapSelTransitionState = RECTMENU_ProcessInput(&D230.menuLapSel);
		}

		RECTMENU_DrawSelf(&D230.menuLapSel, D230.transitionMeta_trackSel[2].currX, D230.transitionMeta_trackSel[2].currY, 0xa4);

		// put LapRow back into 8d920
		sdata->uselessLapRowCopy = D230.menuLapSel.rowSelected;

		// get lap count
		gGT->numLaps = D230.lapRowVal[D230.menuLapSel.rowSelected];

		// if it is time to start the race
		if (lapSelTransitionState == 1)
		{
			// try to start the race
			D230.trackSel_transitionState = EXITING_MENU;

			// if this is 1 (which it is), the race starts,
			// otherwise, you go back to character selection
			D230.trackSel_startRaceAfterFadeOut = lapSelTransitionState;
		}

		// If it is not time to start the race
		else
		{
			if (lapSelTransitionState == -1)
			{
				// close lap selection menu
				D230.trackSel_lapBoxOpen = 0;
			}
		}

		// If "One Lap Race" Cheat is enabled
		if ((gGT->gameMode2 & CHEAT_ONELAP) != 0)
		{
			// Set number of Laps to 1
			gGT->numLaps = 1;
		}
	}

	// decrease frame from track list motion
	int trackChangeFrames = D230.trackSel_trackChangeFrames + -1;
	if ((0 < D230.trackSel_trackChangeFrames) && (D230.trackSel_trackChangeFrames = trackChangeFrames, trackChangeFrames == 0))
	{
		menu->rowSelected = D230.trackSel_currTrack;
	}

	// not transitioning
	u32 resetPreviewVideo = 0;

	// if you are transitioning out of level selection
	if ((D230.trackSel_trackChangeFrames != 0) || (D230.trackSel_transitionState == EXITING_MENU))
	{
		// transitioning,
		// which means stop drawing track video,
		// just draw icon
		resetPreviewVideo = 1;
	}

	MM_TrackSelect_Video_State(resetPreviewVideo);

	gGT->currLEV = selectMenu[menu->rowSelected].levID;
	s32 scanTrack = (int)menu->rowSelected + -1;

	for (s32 hiddenRowIndex = 0; hiddenRowIndex < MM_TRACK_SELECT_CENTER_ROW; hiddenRowIndex++)
	{
		u32 trackOpen;

		do
		{
			currTrack = (s16)scanTrack;
			if (scanTrack < 0)
			{
				currTrack = numTracks - 1;
			}

			trackOpen = MM_TrackSelect_boolTrackOpen(&selectMenu[currTrack]);

			scanTrack = currTrack - 1;
		} while ((trackOpen & 0xffff) == 0);
	}

	s32 rowIndex = 0;
	s32 rowPhase = 0;

	// loop through tracks in track list
	do
	{
		// This part actually "moves" the rows,
		// when pressing the Up and Down buttons on D-Pad
		u32 rowAngle = ((rowPhase >> 0x10) + -MM_TRACK_SELECT_CENTER_ROW) * MM_TRACK_SELECT_ROW_ANGLE_STEP;
		if (0 < D230.trackSel_trackChangeFrames)
		{
			rowAngle = rowAngle + (((MM_TRACK_SELECT_TRACK_CHANGE_FRAMES - D230.trackSel_trackChangeFrames) * MM_TRACK_SELECT_ROW_ANGLE_STEP) /
			                       MM_TRACK_SELECT_TRACK_CHANGE_FRAMES) *
			                          (int)D230.trackSel_trackChangeDirection;
		}

		RECT rowRect;
		rowRect.w = MM_TRACK_SELECT_ROW_W;
		rowRect.h = MM_TRACK_SELECT_ROW_H;

		// posX of track list
		s32 rowX = (u32)D230.transitionMeta_trackSel[0].currX + (MATH_Cos(rowAngle) * MM_TRACK_SELECT_ROW_X_RADIUS >> MM_TRACK_SELECT_ROW_X_SHIFT) +
		           MM_TRACK_SELECT_ROW_X_OFFSET;

		// posY of track list
		s32 rowBaseY = (u32)D230.transitionMeta_trackSel[0].currY + (MATH_Sin(rowAngle) * MM_TRACK_SELECT_ROW_Y_RADIUS >> MM_TRACK_SELECT_ROW_Y_SHIFT);

		s16 rowY = (s16)rowBaseY + MM_TRACK_SELECT_ROW_Y_OFFSET;
		rowRect.x = (s16)rowX;
		rowRect.y = rowY;

		// if you are in time trial mode
		if ((gGT->gameMode1 & TIME_TRIAL) != 0)
		{
			// backup level ID
			s16 previousLevelID = gGT->levelID;

			// draw stars if N Tropy or Oxide are beaten,
			// loop twice
			for (s32 starIndex = 0; starIndex < MM_TRACK_SELECT_TT_STAR_COUNT; starIndex++)
			{
				// set level ID to the level you're hovering on, in the main menu
				gGT->levelID = selectMenu[currTrack].levID;

				// (useless?)
				GAMEPROG_GetPtrHighScoreTrack();

				int timeTrialFlags = sdata->gameProgress.highScoreTracks[gGT->levelID].timeTrialFlags;

				// if star is earned
				if (((timeTrialFlags >> D230.timeTrialFlagGet[starIndex]) & 1) != 0)
				{
					// pointer to color data of star
					u32 *starColor = data.ptrColor[D230.timeTrialStarCol[starIndex]];

					struct Icon **iconPtrArray = ICONGROUP_GETICONS(gGT->iconGroup[5]);

					DecalHUD_DrawPolyGT4(iconPtrArray[MM_TRACK_SELECT_TT_STAR_ICON], rowX + MM_TRACK_SELECT_ROW_W + 4, (int)rowY + starIndex * 8 + 4,

					                     // pointer to PrimMem struct
					                     &gGT->backBuffer->primMem,

					                     // pointer to OT mem
					                     gGT->pushBuffer_UI.ptrOT,

					                     // color data
					                     starColor[0], starColor[1], starColor[2], starColor[3],

					                     0, FP(1.0));
				}
			}
			// restore levelID
			gGT->levelID = previousLevelID;

			// (useless?)
			GAMEPROG_GetPtrHighScoreTrack();
		}

		// Draw string
		DecalFont_DrawLine(sdata->lngStrings[data.metaDataLEV[selectMenu[currTrack].levID].name_LNG], (rowX + 8), (rowBaseY + 0x65), FONT_BIG, ORANGE);

		if ((D230.trackSel_trackChangeFrames == 0) && ((s16)rowIndex == MM_TRACK_SELECT_CENTER_ROW))
		{
			// if you are in time trial mode
			if ((gGT->gameMode1 & TIME_TRIAL) != 0)
			{
				// Check if this track has Ghost Data
				u32 ghostExists = RefreshCard_BoolGhostForLEV(selectMenu[currTrack].levID);

				// If this track has Ghost Data
				if ((ghostExists & 0xffff) != 0)
				{
					u32 ghostTextFlags;

					// Flash Colors
					if ((sdata->frameCounter & 4) == 0)
					{
						ghostTextFlags = (JUSTIFY_CENTER | WHITE);
					}
					else
					{
						ghostTextFlags = (JUSTIFY_CENTER | PERIWINKLE);
					}

					DecalFont_DrawLine(sdata->lngStrings[LNG_GHOST_DATA_EXISTS], (rowX + 8 + 0x78), (rowBaseY + 0x76), FONT_SMALL, ghostTextFlags);
				}
			}
			RECT highlightRect;
			highlightRect.x = rowRect.x + 6;
			highlightRect.y = rowRect.y + 4;
			highlightRect.w = rowRect.w - 12;
			highlightRect.h = rowRect.h - 8;

			CTR_Box_DrawClearBox(&highlightRect, &sdata->menuRowHighlight_Normal, TRANS_50_DECAL, gGT->backBuffer->otMem.uiOT);
		}

		// Draw 2D Menu rectangle background
		RECTMENU_DrawInnerRect(&rowRect, 0, gGT->backBuffer->otMem.uiOT);

		u32 trackOpen;

		do
		{
			currTrack++;

			if (numTracks <= currTrack)
			{
				currTrack = 0;
			}
			trackOpen = MM_TrackSelect_boolTrackOpen(&selectMenu[currTrack]);

		} while ((trackOpen & 0xffff) == 0);

		rowIndex = rowIndex + 1;
		rowPhase = rowIndex * 0x10000;
		if (MM_TRACK_SELECT_VISIBLE_ROWS <= rowIndex)
		{
			RECT previewRect;
			previewRect.w = MM_TRACK_VIDEO_WIDTH;
			previewRect.h = MM_TRACK_VIDEO_HEIGHT;

			// posX of "SELECT LEVEL"
			previewRect.x = D230.transitionMeta_trackSel[1].currX + MM_TRACK_SELECT_PREVIEW_X;

			// posY of "SELECT LEVEL"
			// near-top if map exists, near-mid if no map
			previewRect.y = D230.transitionMeta_trackSel[1].currY + MM_TRACK_SELECT_PREVIEW_Y_NO_MAP;

			if (-1 < selectMenu[menu->rowSelected].mapTextureID)
			{
				previewRect.y = D230.transitionMeta_trackSel[1].currY + MM_TRACK_SELECT_PREVIEW_Y_WITH_MAP;
			}

			// _D230.trackSel_lapBoxOpen is the boolean to show
			// the selection menu for number of laps:
			// 3, 5, 7

			// If the lap selection menu is closed
			if (D230.trackSel_lapBoxOpen == 0)
			{
				DecalFont_DrawLine(sdata->lngStrings[LNG_SELECT_LEVEL_SELECT], (D230.transitionMeta_trackSel[3].currX + 0x18c),
				                   (D230.transitionMeta_trackSel[3].currY + (u32)previewRect.y), FONT_BIG, (JUSTIFY_CENTER | ORANGE));

				DecalFont_DrawLine(sdata->lngStrings[LNG_LEVEL], (D230.transitionMeta_trackSel[3].currX + 0x18c),
				                   (D230.transitionMeta_trackSel[3].currY + (u32)previewRect.y + 0x10), FONT_BIG, (JUSTIFY_CENTER | ORANGE));
			}

			// next, draw the map icon, below "SELECT LEVEL",
			// exactly 0x22 (34) pixels below the text
			previewRect.y += 0x22;

			if ((-1 < selectMenu[menu->rowSelected].mapTextureID) &&

			    // If lap selection menu is closed
			    (D230.trackSel_lapBoxOpen == 0))
			{
				int mapID = selectMenu[menu->rowSelected].mapTextureID;
				struct Icon *iconMap0 = gGT->ptrIcons[mapID + 0];
				struct Icon *iconMap1 = gGT->ptrIcons[mapID + 1];

				// icon data
				char mapTopV2 = iconMap0->texLayout.v2;
				char mapTopV0 = iconMap0->texLayout.v0;
				char mapBottomV2 = iconMap1->texLayout.v2;
				char mapBottomV0 = iconMap1->texLayout.v0;

				int mapWidth = (iconMap0->texLayout.u1 - iconMap0->texLayout.u0);
				int mapHeight = ((((u32)mapTopV2 - (u32)mapTopV0) + (u32)mapBottomV2) - (u32)mapBottomV0);

				// draw six track minimaps on menu
				// map 1 is the regular color, which is white
				// map 2 is blue and shifted 2px to the left
				// map 3 is blue and shifted 2px to the right
				// map 4 is blue and shifted 1px downwards
				// map 5 is blue and shifted 1px upwards
				// map 6 is black and shifted 6px downwards and 12px to the right
				for (s32 mapLayer = 0; mapLayer < MM_TRACK_SELECT_MAP_LAYER_COUNT; mapLayer++)
				{
					UI_Map_DrawMap(
					    // top half
					    iconMap0,

					    // bottom half
					    iconMap1,

					    // X
					    D230.drawMapOffset[mapLayer].offsetX + previewRect.x + (D230.transitionMeta_trackSel[2].currX - D230.transitionMeta_trackSel[1].currX) +
					        (MM_TRACK_VIDEO_WIDTH >> 1) + (mapWidth >> 1),

					    // Y
					    D230.drawMapOffset[mapLayer].offsetY + previewRect.y + (D230.transitionMeta_trackSel[2].currY - D230.transitionMeta_trackSel[1].currY) +
					        0x49 + (MM_TRACK_SELECT_MAP_BOX_H >> 1) + (mapHeight >> 1),

					    // pointer to PrimMem struct
					    &gGT->backBuffer->primMem,

					    // pointer to OT mem
					    gGT->pushBuffer_UI.ptrOT,

					    // 1 = draw map with regular color (white) - used for the main layer of the minimap in the track select screen
					    // 2 = draw map blue - used for the outline of the minimap in the track select screen
					    // 3 = draw map black - used for the shadow of the minimap in the track select screen
					    D230.drawMapOffset[mapLayer].type);
				}
			}

			MM_TrackSelect_Video_Draw(&previewRect, selectMenu, (int)(s16)D230.trackSel_currTrack, (u32)(D230.trackSel_transitionState == EXITING_MENU), 0);

			return;
		}
	} while (true);
}

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800b0eac-0x800b0eb8.
struct RectMenu *MM_TrackSelect_GetMenuPtr(void)
{
	return &D230.menuTrackSelect;
}
