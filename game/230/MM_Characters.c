#include <common.h>

enum
{
	MM_CHARACTER_SELECT_SCREEN_W = 0x200,
	MM_CHARACTER_SELECT_SCREEN_H = 0xd8,
	MM_CHARACTER_SELECT_DISTANCE_TO_SCREEN = 0x100,
	MM_CHARACTER_SELECT_MODEL_MOVE_FP = 0x1000,
	MM_CHARACTER_SELECT_ICON_COUNT = 0xf,
	MM_CHARACTER_SELECT_EXPANSION_ICON_FIRST = 0xc,
	MM_CHARACTER_SELECT_DEFAULT_DRIVER_COUNT = 8,
	MM_CHARACTER_SELECT_MAX_PLAYERS = 4,
	MM_CHARACTER_SELECT_LIMITED_LAYOUT_OFFSET = 4,
	MM_CHARACTER_SELECT_FULL_LAYOUT_COUNT = 2,
	MM_CHARACTER_SELECT_TRANSITION_FRAMES = 0xc,
	MM_CHARACTER_SELECT_TRANSITION_STEP = 8,
	MM_CHARACTER_SELECT_ANGLE_STEP = 0x400,
	MM_CHARACTER_SELECT_ANGLE_OFFSET = 400,
	MM_CHARACTER_SELECT_SPIN_STEP = 0x40,
	MM_CHARACTER_SELECT_LAYOUT_3P = 2,
	MM_CHARACTER_SELECT_LAYOUT_4P = 3,
	MM_CHARACTER_SELECT_LAYOUT_1P_LIMITED = 4,
	MM_CHARACTER_SELECT_LAYOUT_2P_LIMITED = 5,
	MM_CHARACTER_SELECT_TITLE_TRANSITION_INDEX = 15,
	MM_CHARACTER_SELECT_DRIVER_WINDOW_TRANSITION_FIRST = 0x10,
	MM_CHARACTER_SELECT_3P_TITLE_X = 0x9c,
	MM_CHARACTER_SELECT_3P_SELECT_Y = 0x14,
	MM_CHARACTER_SELECT_3P_CHARACTER_Y = 0x26,
	MM_CHARACTER_SELECT_4P_TITLE_X = 0xfc,
	MM_CHARACTER_SELECT_4P_SELECT_Y = 8,
	MM_CHARACTER_SELECT_4P_CHARACTER_Y = 0x18,
	MM_CHARACTER_SELECT_LIMITED_TITLE_X = 0xfc,
	MM_CHARACTER_SELECT_LIMITED_TITLE_Y = 10,
	MM_CHARACTER_SELECT_DIR_UP = 0,
	MM_CHARACTER_SELECT_DIR_DOWN = 1,
	MM_CHARACTER_SELECT_DIR_LEFT = 2,
	MM_CHARACTER_SELECT_DIR_RIGHT = 3,
	MM_CHARACTER_SELECT_INPUT_DPAD = BTN_RIGHT | BTN_LEFT | BTN_DOWN | BTN_UP,
	MM_CHARACTER_SELECT_INPUT_MENU = BTN_TRIANGLE | BTN_CIRCLE | BTN_SQUARE_one | BTN_CROSS_one,
	MM_CHARACTER_SELECT_INPUT_CONFIRM = BTN_CIRCLE | BTN_CROSS_one,
	MM_CHARACTER_SELECT_INPUT_BACK = BTN_TRIANGLE | BTN_SQUARE_one,
	MM_CHARACTER_SELECT_ICON_DECAL_OFFSET_X = 6,
	MM_CHARACTER_SELECT_ICON_DECAL_OFFSET_Y = 4,
	MM_CHARACTER_SELECT_ICON_RECT_W = 0x34,
	MM_CHARACTER_SELECT_ICON_RECT_H = 0x21,
	MM_CHARACTER_SELECT_CURSOR_LABEL_OFFSET_X = -6,
	MM_CHARACTER_SELECT_CURSOR_LABEL_OFFSET_Y = -3,
	MM_CHARACTER_SELECT_HIGHLIGHT_OFFSET_X = 3,
	MM_CHARACTER_SELECT_HIGHLIGHT_OFFSET_Y = 2,
	MM_CHARACTER_SELECT_HIGHLIGHT_W = 0x2e,
	MM_CHARACTER_SELECT_HIGHLIGHT_H = 0x1d,
	MM_CHARACTER_SELECT_SELECTED_BORDER_COUNT = 2,
	MM_CHARACTER_SELECT_SELECTED_BORDER_INSET_X = 3,
	MM_CHARACTER_SELECT_SELECTED_BORDER_INSET_Y = 2,
	MM_CHARACTER_SELECT_SELECTED_BORDER_SHRINK_W = 6,
	MM_CHARACTER_SELECT_SELECTED_BORDER_SHRINK_H = 4,
	MM_CHARACTER_SELECT_4P_NAME_BOTTOM_OFFSET = -6,
};

// NOTE(aalhendi): ASM-verified NTSC-U 926 overlay 230 0x800ad98c-0x800ada4c.
void MM_Characters_AnimateColors(u8 *colorData, s16 playerID, s16 flag)
{
	u8 colorAdjustmentValue;
	u32 trigApproximationIndex;
	u32 trigApprox;

	// access int RGBA as a char array,
	// for editing components of color
	u8 *ptrColor;
	ptrColor = (u8 *)data.ptrColor[playerID + PLAYER_BLUE];

	trigApprox = 0;

	// if player has not selected character yet
	// see MM_Characters_MenuProc
	if (flag == 0)
	{
		trigApproximationIndex = sdata->frameCounter * 0x100 + playerID * 0x400;

		// approximate trigonometry
		trigApprox = CTR_ReadU32LE(&data.trigApprox[trigApproximationIndex & 0x3ff]);

		if ((trigApproximationIndex & 0x400) == 0)
		{
			trigApprox = trigApprox << 0x10;
		}
		trigApprox = trigApprox >> 0x10;

		if ((trigApproximationIndex & 0x800) != 0)
		{
			trigApprox = -(int)trigApprox;
		}
	}

	colorAdjustmentValue = 0;
	if (0xc00 < (int)trigApprox)
	{
		colorAdjustmentValue = ((trigApprox << 7) >> 0xc);
	}

	colorData[0] = ptrColor[0] | colorAdjustmentValue;
	colorData[1] = ptrColor[1] | colorAdjustmentValue;
	colorData[2] = ptrColor[2] | colorAdjustmentValue;
	colorData[3] = 0;

	return;
}

// NOTE(aalhendi): ASM-verified against NTSC-U 926 overlay 230 0x800ada4c-0x800adae4.
int MM_Characters_GetNextDriver(s16 dpad, char characterID)
{
	char nextIcon;
	s16 unlocked;
	char newDriver;

	nextIcon = D230.csm_Active[(s32)characterID].indexNext[dpad];
	unlocked = D230.csm_Active[(s32)nextIcon].unlockFlags;

	// set new driver to the driver
	// you'd get when pressing Up button
	newDriver = nextIcon;

	if (
	    // if desired driver is not unlocked by default
	    (unlocked != -1) &&

	    (((sdata->gameProgress.unlocks[unlocked >> 5] >> (unlocked & 0x1f)) & 1) == 0))
	{
		// set new driver to the driver you already have
		newDriver = characterID;
	}

	// return new driver
	return newDriver;
}

// used for preventing players highlighting the same character
// also for when you go left of komodo joe's icon
// NOTE(aalhendi): ASM-verified against NTSC-U 926 overlay 230 0x800adae4-0x800adb64.
u32 MM_Characters_boolIsInvalid(s16 *iconPerPlayer, s16 characterID, s16 player)
{
	s16 i = 0;

	// if there are players
	if (sdata->gGT->numPlyrNextGame)
	{
		// loop through players
		for (i = 0; i < sdata->gGT->numPlyrNextGame; i++)
		{
			// if driver is taken
			if ((i != player) && (characterID == iconPerPlayer[i]))
			{
				return 1;
			}
		}
	}

	// if driver is not taken
	return 0;
}

// NOTE(aalhendi): ASM-verified NTSC-U 926 overlay 230 0x800adb64-0x800adc0c.
// Search for character model by string,
// specific to main menu lev, altered in oxide mod
struct Model *MM_Characters_GetModelByName(int *name)
{
	struct Model **models;
	struct Model *model;
	struct Level *level1 = sdata->gGT->level1;

	// if LEV is invalid
	if (level1 == NULL)
	{
		return NULL;
	}

	models = level1->ptrModelsPtrArray;
	if (models == NULL)
	{
		return NULL;
	}

	// loop through all models in array
	// of model pointers, until nullptr
	for (model = models[0]; model != NULL; models++, model = models[0])
	{
		int *modelName = (int *)model;

		if ((modelName[0] == name[0]) && (modelName[1] == name[1]) && (modelName[2] == name[2]) && (modelName[3] == name[3]))
		{
			// found it
			return model;
		}
	}
	return NULL;
}

// NOTE(aalhendi): ASM-verified NTSC-U 926 overlay 230 0x800adc0c-0x800ae0bc PSX path.
void MM_Characters_DrawWindows(b32 boolShowDrivers)
{
	struct GameTracker *gGT = sdata->gGT;
	SVec3 rot;

	if (boolShowDrivers != 0)
	{
		// enable drawing wheels
		gGT->renderFlags |= RENDER_FLAG_TIRES;
	}

	for (s32 playerIndex = 0; playerIndex < gGT->numPlyrNextGame; playerIndex++)
	{
		s16 *windowPos = &D230.characterSelect_ptrWindowXY[playerIndex * 2];
		struct TransitionMeta *tMeta = &D230.ptrTransitionMeta[playerIndex];

		struct PushBuffer *pb = &gGT->pushBuffer[playerIndex];
		pb->rect.x = windowPos[0] + tMeta[0x10].currX;
		pb->rect.y = windowPos[1] + tMeta[0x10].currY;
		pb->rect.w = D230.characterSelect_sizeX;
		pb->rect.h = D230.characterSelect_sizeY;

		// negative StartX
		if ((s16)pb->rect.x < 0)
		{
			pb->rect.w -= pb->rect.x;
			pb->rect.x = 0;
			if ((s16)pb->rect.w < 0)
			{
				pb->rect.w = 0;
			}
		}

		// negative StartY
		if ((s16)pb->rect.y < 0)
		{
			pb->rect.h -= pb->rect.y;
			pb->rect.y = 0;
			if ((s16)pb->rect.h < 0)
			{
				pb->rect.h = 0;
			}
		}

		// startX + sizeX out of bounds
		if ((MM_CHARACTER_SELECT_SCREEN_W < pb->rect.x + pb->rect.w) && (pb->rect.w = MM_CHARACTER_SELECT_SCREEN_W - pb->rect.x, pb->rect.w < 0))
		{
			pb->rect.x = MM_CHARACTER_SELECT_SCREEN_W;
			pb->rect.w = 0;

#ifdef CTR_NATIVE
			// NOTE(aalhendi): Native renderer guard; retail leaves w at zero.
			pb->rect.w = 1;
#endif
		}

		// startY + sizeY out of bounds
		if ((MM_CHARACTER_SELECT_SCREEN_H < pb->rect.y + pb->rect.h) && (pb->rect.h = MM_CHARACTER_SELECT_SCREEN_H - pb->rect.y, pb->rect.h < 0))
		{
			pb->rect.y = MM_CHARACTER_SELECT_SCREEN_H;
			pb->rect.h = 0;

#ifdef CTR_NATIVE
			// NOTE(aalhendi): Native renderer guard; retail leaves h at zero.
			pb->rect.h = 1;
#endif
		}

		// distanceToScreen
		pb->distanceToScreen_CURR = MM_CHARACTER_SELECT_DISTANCE_TO_SCREEN;
		pb->distanceToScreen_PREV = MM_CHARACTER_SELECT_DISTANCE_TO_SCREEN;

		// pushBuffer pos and rot to all zero
		pb->pos.x = 0;
		pb->pos.y = 0;
		pb->pos.z = 0;
		pb->rot.x = 0;
		pb->rot.y = 0;
		pb->rot.z = 0;

		// player -> instance
		struct Instance *driverInst = gGT->drivers[playerIndex]->instSelf;

		// Make Visible
		driverInst->flags &= ~HIDE_MODEL;

		// if driver is off-screen
		if ((gGT->numPlyrNextGame <= playerIndex) || (boolShowDrivers == 0))
		{
			// invisible
			driverInst->flags |= HIDE_MODEL;
		}

		struct InstDrawPerPlayer *idpp = INST_GETIDPP(driverInst);

		// clear pushBuffer in every InstDrawPerPlayer
		idpp[0].pushBuffer = 0;
		idpp[1].pushBuffer = 0;
		idpp[2].pushBuffer = 0;
		idpp[3].pushBuffer = 0;

		// set pushBuffer in InstDrawPerPlayer,
		// so that each camera can only see one driver
		idpp[playerIndex].pushBuffer = pb;

		s16 *currCharacterID = &D230.characterSelect_charIDs_curr[playerIndex];

		driverInst->animFrame = 0;
		driverInst->vertSplit = 0;

		struct Model *model = MM_Characters_GetModelByName((int *)data.MetaDataCharacters[(int)*currCharacterID].name_Debug);

		// set modelPtr in Instance
		driverInst->model = model;

		// CameraDC, freecam mode
		gGT->cameraDC[playerIndex].cameraMode = 3;

		// Set position of player
		driverInst->matrix.t[0] = D230.csm_instPos.x;
		driverInst->matrix.t[1] = D230.csm_instPos.y;
		driverInst->matrix.t[2] = D230.csm_instPos.z;

		s16 *moveTimer = &D230.characterSelect_modelMoveTimer[playerIndex];
		s16 nextMoveTimer = *moveTimer + -1;

		// If no transition between players
		if (*moveTimer == 0)
		{
			// compare to character ID
			if (*currCharacterID != data.characterIDs[playerIndex])
			{
				*moveTimer = D230.characterSelect_modelMoveFrames << 1;
				D230.characterSelect_charIDs_desired[playerIndex] = data.characterIDs[playerIndex];
			}
		}

		// if transition between players
		else
		{
			// get timer
			*moveTimer = nextMoveTimer;

			s32 slideDirection;
			s32 slideOffset;

			// if timer is before midpoint
			if ((int)nextMoveTimer < (int)D230.characterSelect_modelMoveFrames)
			{
				// make driver fly off screen
				*currCharacterID = D230.characterSelect_charIDs_desired[playerIndex];
				s32 moveFrameScale = RaceFlag_MoveModels((int)nextMoveTimer, (int)D230.characterSelect_modelMoveFrames);

				// direction moving
				slideDirection = -D230.characterSelect_modelMoveDir[playerIndex];
				slideOffset = moveFrameScale * D230.characterSelect_modelSlideDistance >> 0xc;
			}

			// if timer is after midpoint
			else
			{
				// make new driver fly on screen
				s32 moveFrameScale =
				    RaceFlag_MoveModels((int)nextMoveTimer - (int)D230.characterSelect_modelMoveFrames, (int)D230.characterSelect_modelMoveFrames);

				// direction moving
				slideDirection = D230.characterSelect_modelMoveDir[playerIndex];
				slideOffset = (MM_CHARACTER_SELECT_MODEL_MOVE_FP - moveFrameScale) * (int)D230.characterSelect_modelSlideDistance >> 0xc;
			}

			driverInst->matrix.t[0] += slideDirection * slideOffset;
		}

		// driver rotation
		rot.x = D230.csm_instRot.x;
		rot.y = D230.csm_instRot.y + D230.characterSelect_angle[playerIndex];
		rot.z = D230.csm_instRot.z;

		ConvertRotToMatrix(&driverInst->matrix, &rot);
	}
	return;
}

// NOTE(aalhendi): ASM-verified NTSC-U 926 overlay 230 0x800ae0bc-0x800ae274.
void MM_Characters_SetMenuLayout(void)
{
	b32 expandRoster = false;

	// By default, draw "Select character" in 3P menu
	D230.isRosterExpanded = 0;

	s32 numPlyrNextGame = sdata->gGT->numPlyrNextGame;
	s32 layoutIndex = numPlyrNextGame - 1;

	// Loop through bottom characters,
	// if any are unlocked, use expanded
	for (s32 iconIndex = MM_CHARACTER_SELECT_EXPANSION_ICON_FIRST; iconIndex < MM_CHARACTER_SELECT_ICON_COUNT; iconIndex++)
	{
		// OG game code
		u16 unlocked = D230.csm_1P2P[iconIndex].unlockFlags;

		if (((sdata->gameProgress.unlocks[unlocked >> 5] >> (unlocked & 0x1f)) & 1) != 0)
		{
			expandRoster = true;
			break;
		}
	}

	if (
	    // if 1P2P (0 or 1)
	    (layoutIndex < MM_CHARACTER_SELECT_FULL_LAYOUT_COUNT) &&

	    // if very few characters are unlocked
	    (!expandRoster))
	{
		// layout [4] and [5] for 1P2P without expansion
		layoutIndex += MM_CHARACTER_SELECT_LIMITED_LAYOUT_OFFSET;
	}

	D230.isRosterExpanded = expandRoster;

	D230.characterSelectIconLayout = layoutIndex;

	D230.csm_instPos.y = D230.driverPosY[layoutIndex];
	D230.csm_instPos.z = D230.driverPosZ[layoutIndex];

	D230.characterSelect_sizeX = D230.windowW[layoutIndex];
	D230.characterSelect_sizeY = D230.windowH[layoutIndex];

	D230.characterSelect_ptrWindowXY = D230.ptrSelectWindowPos[layoutIndex];

	D230.csm_Active = D230.ptrCsmArr[layoutIndex];

	D230.textPos = D230.textPosArr[layoutIndex];

	D230.ptrTransitionMeta = D230.ptr_transitionMeta_csm[numPlyrNextGame - 1];

	return;
}

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800ae274-0x800ae2c0.
void MM_Characters_BackupIDs(void)
{
	for (s32 driverIndex = 0; driverIndex < MM_CHARACTER_SELECT_DEFAULT_DRIVER_COUNT; driverIndex++)
	{
		// make a backup when you leave character selection,
		// backup is restored when you go back to selection
		sdata->characterIDs_backup[driverIndex] = data.characterIDs[driverIndex];
	}
	return;
}

// NOTE(aalhendi): ASM-verified NTSC-U 926 overlay 230 0x800ae2c0-0x800ae464.
void MM_Characters_PreventOverlap(void)
{
	struct GameTracker *gGT = sdata->gGT;
	s8 availableDefaultCharacters[MM_CHARACTER_SELECT_DEFAULT_DRIVER_COUNT];

	// default 0,1,2,3,4,5,6,7
	CTR_WriteU32LE((u8 *)&availableDefaultCharacters[0], (u32)R230.characterID_default[0]);
	CTR_WriteU32LE((u8 *)&availableDefaultCharacters[4], (u32)R230.characterID_default[1]);

	for (s32 playerIndex = 0; playerIndex < gGT->numPlyrNextGame; playerIndex++)
	{
		// get character ID
		s32 characterID = data.characterIDs[playerIndex];

		// if not a secret character
		if (characterID < MM_CHARACTER_SELECT_DEFAULT_DRIVER_COUNT)
		{
			// character is taken
			availableDefaultCharacters[characterID] = -1;
		}
	}

	for (s32 playerIndex = 1; playerIndex < gGT->numPlyrNextGame; playerIndex++)
	{
		for (s32 previousPlayer = 0; previousPlayer < playerIndex; previousPlayer++)
		{
			// if two characters are the same
			if (data.characterIDs[playerIndex] == data.characterIDs[previousPlayer])
			{
				// look for a new character
				for (s32 defaultIndex = 0; defaultIndex < MM_CHARACTER_SELECT_DEFAULT_DRIVER_COUNT; defaultIndex++)
				{
					// get default character
					s8 *defaultCharacter = &availableDefaultCharacters[defaultIndex];
					s8 freeCharacter = *defaultCharacter;

					// if character is not taken
					if (-1 < freeCharacter)
					{
						// assign free character
						data.characterIDs[playerIndex] = (s16)freeCharacter;

						// character is now taken
						*defaultCharacter = -1;

						break;
					}
				}
			}
		}
	}
	return;
}

// NOTE(aalhendi): ASM-verified NTSC-U 926 overlay 230 0x800ae464-0x800ae6b0.
void MM_Characters_RestoreIDs(void)
{
	struct GameTracker *gGT = sdata->gGT;

	// erase select bits
	sdata->characterSelectFlags = 0;
	D230.transitionFrames = MM_CHARACTER_SELECT_TRANSITION_FRAMES;
	D230.isMenuTransitioning = ENTERING_MENU;

	// This uses 80086e84, which controls character IDs
	for (s32 driverIndex = 0; driverIndex < MM_CHARACTER_SELECT_DEFAULT_DRIVER_COUNT; driverIndex++)
	{
		// set character ID to the last ID you entered
		data.characterIDs[driverIndex] = sdata->characterIDs_backup[driverIndex];
	}

	MM_Characters_SetMenuLayout();

	for (s32 iconIndex = 0; iconIndex < MM_CHARACTER_SELECT_ICON_COUNT; iconIndex++)
	{
		// would not need this if CSM was sorted
		// by order of character ID

		// Basically sets them to 0, 1, 2, 3, 4... up to 0xE,
		// setting Oxide's manually to 0xF is needed to make his icon appear

		D230.characterMenuID[(s32)D230.csm_Active[iconIndex].characterID] = iconIndex;
	}

	for (s32 playerIndex = 0; playerIndex < gGT->numPlyrNextGame; playerIndex++)
	{
		// Determine if this icon is unlocked (and drawing)

		// get character ID
		s16 *currID = &data.characterIDs[playerIndex];

		// get unlock requirement for this character
		s16 unlocked = D230.csm_Active[(s32)*currID].unlockFlags;

		if (
		    // If Icon has an unlock requirement
		    (unlocked != -1) &&

		    // If Character is Locked
		    (((sdata->gameProgress.unlocks[unlocked >> 5] >> (unlocked & 0x1f)) & 1) == 0))
		{
			// change character to Crash
			*currID = CRASH_BANDICOOT;
		}
	}

	MM_Characters_PreventOverlap();

	for (s32 playerIndex = 0; playerIndex < gGT->numPlyrNextGame; playerIndex++)
	{
		// set name string ID to the character ID of each player.
		// The string will only draw if both these variables match
		D230.characterSelect_charIDs_curr[playerIndex] = data.characterIDs[playerIndex];
		D230.characterSelect_charIDs_desired[playerIndex] = data.characterIDs[playerIndex];

		// something to do with transitioning between icons
		D230.characterSelect_modelMoveTimer[playerIndex] = 0;

		// rotation of each driver, 90 degrees difference
		D230.characterSelect_angle[playerIndex] = (playerIndex * MM_CHARACTER_SELECT_ANGLE_STEP) + MM_CHARACTER_SELECT_ANGLE_OFFSET;
	}

	MM_Characters_DrawWindows(0);
	return;
}

// NOTE(aalhendi): ASM-verified against NTSC-U 926 overlay 230 0x800ae6b0-0x800ae74c.
void MM_Characters_HideDrivers(void)
{
	struct GameTracker *gGT = sdata->gGT;

	for (s32 playerIndex = 0; playerIndex < MM_CHARACTER_SELECT_MAX_PLAYERS; playerIndex++)
	{
		PushBuffer_Init(&gGT->pushBuffer[playerIndex], 0, 1);

		gGT->drivers[playerIndex]->instSelf->flags |= HIDE_MODEL;
	}

	return;
}

void MM_Characters_MenuProc(struct RectMenu *unused)
{
	(void)unused;
	u8 numPlyrNextGame;
	int candidateTaken;
	int deadEndCandidateFree;
	u16 playerSelectFlag;
	s16 nextIcon;
	u32 button;
	int intermediateIcon;
	u8 *outlineColorData;
	u32 characterSelectType;
	u32 fontType;
	u32 iconColor;
	s16 previousCandidateIcon;
	int nextIconCopy;
	s16 alternateIcon;
	s16 currentIcon;
	s16 candidateIcon;
	s16 *playerIconSlot;
	u32 k;
	struct CharacterSelectMeta *preInputCharacterMeta;
	s16 iconPerPlayer[4];
	u8 playerColorData[8];
	Color color;

	u8 colorRGBA[4];

	RECT r1;
	RECT *r = &r1;
	RECT r58;

	s16 hitNavigationDeadEnd;
	s16 *iconPerPlayerPtr;

	int i;
	int j;
	int posX;
	int posY;
	s16 playerIcon;
	int direction;

	s16 *windowPos;
	struct CharacterSelectMeta *csm_Active;

	struct GameTracker *gGT = sdata->gGT;

	u32 *ot = gGT->backBuffer->otMem.uiOT;

	for (i = 0; i < MM_CHARACTER_SELECT_MAX_PLAYERS; i++)
	{
		iconPerPlayer[i] = D230.characterMenuID[data.characterIDs[i]];
	}

	// if menu is not in focus
	if (D230.isMenuTransitioning != IN_MENU)
	{
		MM_TransitionInOut(D230.ptrTransitionMeta, (int)D230.transitionFrames, MM_CHARACTER_SELECT_TRANSITION_STEP);
	}

	MM_Characters_SetMenuLayout();
	MM_Characters_DrawWindows(1);

	// if transitioning in
	if (D230.isMenuTransitioning == ENTERING_MENU)
	{
		// if no more frames
		if (D230.transitionFrames == 0)
		{
			// menu is now in focus
			D230.isMenuTransitioning = IN_MENU;
		}
		else
		{
			D230.transitionFrames--;
		}
	}

	// if transitioning out
	if (D230.isMenuTransitioning == EXITING_MENU)
	{
		// increase frame
		D230.transitionFrames++;

		// if more than 12 frames
		if (D230.transitionFrames > MM_CHARACTER_SELECT_TRANSITION_FRAMES)
		{
			// Make a backup of the characters
			// you selected in character selection screen
			MM_Characters_BackupIDs();

			// if returning to main menu
			if (D230.movingToTrackMenu == 0)
			{
				MM_JumpTo_Title_Returning();
				MM_Characters_HideDrivers();
				return;
			}

			MM_Characters_HideDrivers();

			// if you are in a cup
			if ((gGT->gameMode2 & CUP_ANY_KIND) != 0)
			{
				sdata->ptrDesiredMenu = &D230.menuCupSelect;
				MM_CupSelect_Init();
				return;
			}

			// if going to track selection
			sdata->ptrDesiredMenu = &D230.menuTrackSelect;
			MM_TrackSelect_Init();
			return;
		}
	}

	posX = D230.ptrTransitionMeta[MM_CHARACTER_SELECT_TITLE_TRANSITION_INDEX].currX;
	posY = D230.ptrTransitionMeta[MM_CHARACTER_SELECT_TITLE_TRANSITION_INDEX].currY;

	char *characterSelectString;
	switch (D230.characterSelectIconLayout)
	{
	// 3P character selection
	case MM_CHARACTER_SELECT_LAYOUT_3P:

		// If you have a lot of characters unlocked, do not draw SELECT CHARACTER
		if (D230.isRosterExpanded)
		{
			goto dontDrawSelectCharacter;
		}

		DecalFont_DrawLine(sdata->lngStrings[LNG_SELECT_CHARACTER_SELECT], posX + MM_CHARACTER_SELECT_3P_TITLE_X, posY + MM_CHARACTER_SELECT_3P_SELECT_Y,
		                   FONT_BIG, (JUSTIFY_CENTER | ORANGE));
		characterSelectType = FONT_BIG;

		characterSelectString = sdata->lngStrings[LNG_CHARACTER];

		posX = posX + MM_CHARACTER_SELECT_3P_TITLE_X;
		posY = posY + MM_CHARACTER_SELECT_3P_CHARACTER_Y;
		break;

	// 4P character selection
	case MM_CHARACTER_SELECT_LAYOUT_4P:

		// If Fake Crash is unlocked, do not draw "Select Character"
		if (sdata->gameProgress.unlockFlags & UNLOCK_FAKE_CRASH)
		{
			goto dontDrawSelectCharacter;
		}

		DecalFont_DrawLine(sdata->lngStrings[LNG_SELECT_CHARACTER_SELECT], posX + MM_CHARACTER_SELECT_4P_TITLE_X, posY + MM_CHARACTER_SELECT_4P_SELECT_Y,
		                   FONT_CREDITS, (JUSTIFY_CENTER | ORANGE));
		characterSelectType = FONT_CREDITS;

		characterSelectString = sdata->lngStrings[LNG_CHARACTER];

		posX = posX + MM_CHARACTER_SELECT_4P_TITLE_X;
		posY = posY + MM_CHARACTER_SELECT_4P_CHARACTER_Y;
		break;

	// If you are in 1P or 2P character selection,
	// when you do NOT have a lot of characters selected
	case MM_CHARACTER_SELECT_LAYOUT_1P_LIMITED:
	case MM_CHARACTER_SELECT_LAYOUT_2P_LIMITED:
		characterSelectType = FONT_BIG;

		characterSelectString = sdata->lngStrings[LNG_SELECT_CHARACTER];

		posX = posX + MM_CHARACTER_SELECT_LIMITED_TITLE_X;
		posY = posY + MM_CHARACTER_SELECT_LIMITED_TITLE_Y;
		break;

	default:
		goto dontDrawSelectCharacter;
	}

	// Draw String
	DecalFont_DrawLine(characterSelectString, posX, posY, characterSelectType, (JUSTIFY_CENTER | ORANGE));

dontDrawSelectCharacter:

	iconPerPlayerPtr = &iconPerPlayer[0];

	for (i = 0; i < gGT->numPlyrNextGame; i++)
	{
		playerSelectFlag = (u16)(1 << i);
		currentIcon = iconPerPlayerPtr[i];
		candidateIcon = currentIcon;

		MM_Characters_AnimateColors(playerColorData, i, (int)(s16)(sdata->characterSelectFlags & playerSelectFlag));

		preInputCharacterMeta = &D230.csm_Active[currentIcon];

		if (

		    (D230.isMenuTransitioning == IN_MENU) && (
		                                                 // get input from this player
		                                                 button = sdata->buttonTapPerPlayer[i],

		                                                 // If you press the D-Pad, or Cross, Square, Triangle, Circle
		                                                 button & (MM_CHARACTER_SELECT_INPUT_DPAD | MM_CHARACTER_SELECT_INPUT_MENU)))
		{
			// if character has not been selected by this player
			if (((int)(s16)sdata->characterSelectFlags >> i & 1U) == 0)
			{
				// If you pressed any of the D-pad buttons
				if ((button & MM_CHARACTER_SELECT_INPUT_DPAD) != 0)
				{
					hitNavigationDeadEnd = 0;

					// If you do not press Up
					if ((button & BTN_UP) == 0)
					{
						// If you do not press Down
						if ((button & BTN_DOWN) == 0)
						{
							// This must be if you press Left,
							// because the variable will change
							// if it is anything that isn't Left

							// Left
							direction = MM_CHARACTER_SELECT_DIR_LEFT;

							// If you press Left
							if ((button & BTN_LEFT) != 0)
							{
								goto LAB_800aec08;
							}

							// At this point, you must have pressed Right

							// Right
							direction = MM_CHARACTER_SELECT_DIR_RIGHT;

							// Move down character selection list
							D230.characterSelect_modelMoveDir[i] = 1;
						}

						// If you pressed Down
						else
						{
							// Down
							direction = MM_CHARACTER_SELECT_DIR_DOWN;

							// Move down character selection list
							D230.characterSelect_modelMoveDir[i] = 1;
						}
					}

					// If you pressed Up
					else
					{
						// Up
						direction = MM_CHARACTER_SELECT_DIR_UP;
					LAB_800aec08:
						// If you press Up or Left

						// Move up character selection list
						(D230.characterSelect_modelMoveDir)[i] = 0xffff;
					}

					j = i;
					playerIconSlot = &iconPerPlayerPtr[j];
					previousCandidateIcon = candidateIcon;
					do
					{
						candidateIcon = MM_Characters_GetNextDriver(direction, previousCandidateIcon);
						alternateIcon = candidateIcon;

						if (candidateIcon == previousCandidateIcon)
						{
							hitNavigationDeadEnd = 1;
							nextIcon = MM_Characters_GetNextDriver(direction, (int)(s16)*playerIconSlot);
							nextIconCopy = (int)nextIcon;
							candidateIcon = MM_Characters_GetNextDriver((u32)(u8)D230.getNextDriver1[direction], nextIconCopy);
							intermediateIcon = (int)(s16)candidateIcon;

							if ((((intermediateIcon == alternateIcon) || (nextIconCopy == alternateIcon)) || (nextIconCopy == intermediateIcon)) ||
							    (button = MM_Characters_boolIsInvalid(iconPerPlayerPtr, intermediateIcon, j), (button & 0xffff) != 0))
							{
								nextIcon = MM_Characters_GetNextDriver((u32)(u8)D230.getNextDriver1[direction], (int)(s16)*playerIconSlot);
								intermediateIcon = (int)nextIcon;
								candidateIcon = MM_Characters_GetNextDriver(direction, intermediateIcon);
								alternateIcon = (int)(s16)candidateIcon;

								if (((alternateIcon == previousCandidateIcon) || (intermediateIcon == previousCandidateIcon)) ||
								    ((intermediateIcon == alternateIcon ||
								      (button = MM_Characters_boolIsInvalid(iconPerPlayerPtr, alternateIcon, j), (button & 0xffff) != 0))))
								{
									nextIcon = MM_Characters_GetNextDriver(direction, (int)(s16)*playerIconSlot);
									intermediateIcon = (int)nextIcon;
									candidateIcon = MM_Characters_GetNextDriver((u32)(u8)D230.getNextDriver2[direction], intermediateIcon);
									alternateIcon = (int)(s16)candidateIcon;

									if (((alternateIcon == previousCandidateIcon) || (intermediateIcon == previousCandidateIcon)) ||
									    ((intermediateIcon == alternateIcon ||
									      (button = MM_Characters_boolIsInvalid(iconPerPlayerPtr, alternateIcon, j), (button & 0xffff) != 0))))
									{
										nextIcon = MM_Characters_GetNextDriver((u32)(u8)D230.getNextDriver2[direction], (int)(s16)*playerIconSlot);
										intermediateIcon = (int)nextIcon;
										candidateIcon = MM_Characters_GetNextDriver(direction, intermediateIcon);
										alternateIcon = (int)(s16)candidateIcon;

										if ((((alternateIcon == previousCandidateIcon) || (intermediateIcon == previousCandidateIcon)) ||
										     (intermediateIcon == alternateIcon)) ||
										    (button = MM_Characters_boolIsInvalid(iconPerPlayerPtr, alternateIcon, j), (button & 0xffff) != 0))
										{
											candidateIcon = (u32)*playerIconSlot;
										}
									}
								}
							}
						}
						candidateTaken = false;

						for (k = 0; k < (u32)gGT->numPlyrNextGame; k++)
						{
							if (((int)k != j) && ((s16)candidateIcon == iconPerPlayerPtr[k]))
							{
								candidateTaken = true;
								break;
							}
						}

						if (previousCandidateIcon << 0x10 != candidateIcon << 0x10)
						{
							// Play sound
							// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800aeeb8-0x800aeecc for character cursor-change SFX.
							OtherFX_Play(0, 1);
						}
						if (hitNavigationDeadEnd != 0)
						{
							deadEndCandidateFree = !candidateTaken;
							candidateTaken = false;
							if (deadEndCandidateFree)
							{
								break;
							}
							candidateIcon = (u32)*playerIconSlot;
						}
						previousCandidateIcon = candidateIcon;
					} while (candidateTaken);
				}
				currentIcon = (u16)candidateIcon;

				for (j = 0; j < gGT->numPlyrNextGame; j++)
				{
					if ((j != i) && ((s16)candidateIcon == iconPerPlayerPtr[j]))
					{
						candidateIcon = (u32)(u16)iconPerPlayerPtr[i];
					}
					currentIcon = (u16)candidateIcon;
				}

				// If this player pressed Cross or Circle
				if (((sdata->buttonTapPerPlayer)[i] & MM_CHARACTER_SELECT_INPUT_CONFIRM) != 0)
				{
					// this player has now selected a character
					sdata->characterSelectFlags = sdata->characterSelectFlags | (u16)(1 << i);

					numPlyrNextGame = gGT->numPlyrNextGame;

					// Play sound
					// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800aefa4-0x800aefe4 for character confirm SFX.
					OtherFX_Play(1, 1);

					// if all players have selected their characters
					if ((int)(s16)sdata->characterSelectFlags == (1 << numPlyrNextGame) - 1)
					{
						// move to track selection
						D230.movingToTrackMenu = 1;
						D230.isMenuTransitioning = EXITING_MENU;
					}
				}

				if (
				    // if this is the first iteration of the loop
				    ((i & 0xffff) == 0) &&

				    // if you press Square or Triangle
				    ((sdata->buttonTapPerPlayer[0] & MM_CHARACTER_SELECT_INPUT_BACK) != 0))
				{
					// return to main menu
					D230.movingToTrackMenu = 0;
					D230.isMenuTransitioning = EXITING_MENU;

					// Play sound
					// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800af01c-0x800af054 for character-select back SFX.
					OtherFX_Play(2, 1);
				}
			}
			else
			{
				// if you press Square or Triangle
				if ((button & MM_CHARACTER_SELECT_INPUT_BACK) != 0)
				{
					// Play sound
					// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800af060-0x800af074 for character deselect SFX.
					OtherFX_Play(2, 1);

					// this player has de-selected their character
					sdata->characterSelectFlags = sdata->characterSelectFlags & ~playerSelectFlag;
				}
			}

			// clear input
			sdata->buttonTapPerPlayer[i] = 0;
		}

		iconPerPlayerPtr[i] = currentIcon;

		// transition of each icon
		struct TransitionMeta *currentIconTransition = &D230.ptrTransitionMeta[currentIcon];

		// if player has not selected a character
		if (((sdata->characterSelectFlags >> i) & 1U) == 0)
		{
			// draw string
			// "1", "2", "3", "4", above the character icon
			DecalFont_DrawLine(D230.PlayerNumberStrings[i], currentIconTransition->currX + (u32)preInputCharacterMeta->posX - 6,
			                   currentIconTransition->currY + (u32)preInputCharacterMeta->posY - 3, FONT_BIG, WHITE);
			outlineColorData = playerColorData;
		}
		else
		{
			outlineColorData = (u8 *)&D230.characterSelect_Outline;
		}

		r->x = currentIconTransition->currX + preInputCharacterMeta->posX;
		r->y = currentIconTransition->currY + preInputCharacterMeta->posY;
		r->w = MM_CHARACTER_SELECT_ICON_RECT_W;
		r->h = MM_CHARACTER_SELECT_ICON_RECT_H;

		color = *(Color *)outlineColorData;
		RECTMENU_DrawOuterRect_HighLevel(r, color, 0, ot);
	}

	MM_Characters_PreventOverlap();

	csm_Active = D230.csm_Active;

	// loop through character icons
	for (i = 0; i < MM_CHARACTER_SELECT_ICON_COUNT; i++)
	{
		s16 unlockRequirement = csm_Active->unlockFlags;
		if (
		    // If Icon is unlocked by default,
		    (unlockRequirement == -1) ||

		    // if character is unlocked
		    // from 4-byte variable that handles all rewards
		    // also the variable written by cheats
		    (((sdata->gameProgress.unlocks[unlockRequirement >> 5] >> (unlockRequirement & 0x1f)) & 1) != 0))
		{
			iconColor = D230.characterSelect_NeutralColor;

			for (j = 0; j < gGT->numPlyrNextGame; j++)
			{
				if (((s16)i == iconPerPlayer[j]) &&

				    // if player selected a character
				    (((int)(s16)sdata->characterSelectFlags >> (j & 0x1fU) & 1U) != 0))
				{
					iconColor = D230.characterSelect_ChosenColor;
				}
			}

			struct TransitionMeta *iconTransition = &D230.ptrTransitionMeta[i];

			RECTMENU_DrawPolyGT4(gGT->ptrIcons[data.MetaDataCharacters[csm_Active->characterID].iconID],
			                     iconTransition->currX + csm_Active->posX + MM_CHARACTER_SELECT_ICON_DECAL_OFFSET_X,
			                     iconTransition->currY + csm_Active->posY + MM_CHARACTER_SELECT_ICON_DECAL_OFFSET_Y,

			                     &gGT->backBuffer->primMem, gGT->pushBuffer_UI.ptrOT,

			                     iconColor, iconColor, iconColor, iconColor, TRANS_50_DECAL, FP(1.0));
		}

		csm_Active++;
	}

	// reset
	csm_Active = D230.csm_Active;

	for (i = 0; i < MM_CHARACTER_SELECT_MAX_PLAYERS; i++)
	{
		data.characterIDs[i] = csm_Active[(int)iconPerPlayer[i]].characterID;
	}

	for (i = 0; i < gGT->numPlyrNextGame; i++)
	{
		j = i;
		playerIcon = iconPerPlayer[j];
		csm_Active = &D230.csm_Active[playerIcon];

		// if player has not selected a character
		if (((int)(s16)sdata->characterSelectFlags >> j & 1U) == 0)
		{
			MM_Characters_AnimateColors((u8 *)&colorRGBA, j,

			                            // flags of which characters are selected
			                            (int)(s16)(sdata->characterSelectFlags & (u16)(1 << j)));

			colorRGBA[0] = (u8)((int)((u32)colorRGBA[0] << 2) / 5);
			colorRGBA[1] = (u8)((int)((u32)colorRGBA[1] << 2) / 5);
			colorRGBA[2] = (u8)((int)((u32)colorRGBA[2] << 2) / 5);

			struct TransitionMeta *selectedIconTransition = &D230.ptrTransitionMeta[playerIcon];

			r->x = selectedIconTransition->currX + csm_Active->posX + MM_CHARACTER_SELECT_HIGHLIGHT_OFFSET_X;
			r->y = selectedIconTransition->currY + csm_Active->posY + MM_CHARACTER_SELECT_HIGHLIGHT_OFFSET_Y;
			r->w = MM_CHARACTER_SELECT_HIGHLIGHT_W;
			r->h = MM_CHARACTER_SELECT_HIGHLIGHT_H;

			Color color = *(Color *)&colorRGBA;
			// this draws the flashing blue square that appears when you highlight a character in the character select screen
			CTR_Box_DrawSolidBox(r, color, ot);
		}
		if ((D230.characterSelect_modelMoveTimer[j] == 0) && (D230.characterSelect_charIDs_curr[j] == data.characterIDs[j]))
		{
			// get number of players
			numPlyrNextGame = gGT->numPlyrNextGame;

			// if number of players is 1 or 2
			fontType = FONT_CREDITS;

			// if number of players is 3 or 4
			if (numPlyrNextGame >= 3)
			{
				fontType = FONT_SMALL;
			}

			struct TransitionMeta *driverWindowTransition = &D230.ptrTransitionMeta[j + MM_CHARACTER_SELECT_DRIVER_WINDOW_TRANSITION_FIRST];
			s16 nameBaseY = driverWindowTransition->currY + D230.characterSelect_ptrWindowXY[j * 2 + 1];
			s16 nameYOffset = (s16)((((u32)(numPlyrNextGame < 3) ^ 1) << 0x12) >> 0x10);
			s16 nameY;

			if ((numPlyrNextGame == 4) && (j > 1))
			{
				nameY = nameBaseY + nameYOffset + MM_CHARACTER_SELECT_4P_NAME_BOTTOM_OFFSET;
			}
			else
			{
				nameY = nameBaseY + D230.textPos + nameYOffset;
			}

			// draw string
			DecalFont_DrawLine(sdata->lngStrings[data.MetaDataCharacters[csm_Active->characterID].name_LNG_long],
			                   (int)driverWindowTransition->currX + D230.characterSelect_ptrWindowXY[j * 2] + (int)((u32)D230.characterSelect_sizeX >> 1),
			                   (int)nameY, fontType, (JUSTIFY_CENTER | ORANGE));
		}

		// spin the character
		D230.characterSelect_angle[i] += MM_CHARACTER_SELECT_SPIN_STEP;
	}

	// reset
	csm_Active = D230.csm_Active;

	// loop through all icons
	for (i = 0; i < MM_CHARACTER_SELECT_ICON_COUNT; i++)
	{
		s16 unlockRequirement = csm_Active[i].unlockFlags;

		if (
		    // If Icon is unlocked (from array of icons)
		    (unlockRequirement == -1) ||

		    // if character is unlocked
		    // from 4-byte variable that handles all rewards
		    // also the variable written by cheats
		    ((sdata->gameProgress.unlocks[unlockRequirement >> 5] >> (unlockRequirement & 0x1fU) & 1) != 0))
		{
			struct TransitionMeta *iconTransition = &D230.ptrTransitionMeta[i];

			r->x = iconTransition->currX + csm_Active[i].posX;
			r->y = iconTransition->currY + csm_Active[i].posY;
			r->w = MM_CHARACTER_SELECT_ICON_RECT_W;
			r->h = MM_CHARACTER_SELECT_ICON_RECT_H;

			// Draw 2D Menu rectangle background
			RECTMENU_DrawInnerRect(r, 0, ot);
		}
	}

	windowPos = D230.characterSelect_ptrWindowXY;

	for (i = 0; i < gGT->numPlyrNextGame; i++)
	{
		j = i;
		struct TransitionMeta *driverWindowTransition = &D230.ptrTransitionMeta[j + MM_CHARACTER_SELECT_DRIVER_WINDOW_TRANSITION_FIRST];

		// store window width and height in one 4-byte variable
		r->x = driverWindowTransition->currX + windowPos[0];
		r->y = driverWindowTransition->currY + windowPos[1];
		r->w = D230.characterSelect_sizeX;
		r->h = D230.characterSelect_sizeY;

		MM_Characters_AnimateColors((u8 *)&colorRGBA, j,

		                            // flags of which characters are selected
		                            ((int)(s16)sdata->characterSelectFlags >> j ^ 1U) & 1);

		color = *(Color *)&colorRGBA;
		RECTMENU_DrawOuterRect_HighLevel(r, color, 0, ot);

		// if player selected a character
		if (((int)(s16)sdata->characterSelectFlags >> j & 1U) != 0)
		{
			r58.x = r->x;
			r58.y = r->y;
			r58.w = r->w;
			r58.h = r->h;

			for (s32 borderIndex = 0; borderIndex < MM_CHARACTER_SELECT_SELECTED_BORDER_COUNT; borderIndex++)
			{
				r58.x += MM_CHARACTER_SELECT_SELECTED_BORDER_INSET_X;
				r58.y += MM_CHARACTER_SELECT_SELECTED_BORDER_INSET_Y;
				r58.w -= MM_CHARACTER_SELECT_SELECTED_BORDER_SHRINK_W;
				r58.h -= MM_CHARACTER_SELECT_SELECTED_BORDER_SHRINK_H;

				colorRGBA[0] = (u8)((int)((u32)colorRGBA[0] << 2) / 5);
				colorRGBA[1] = (u8)((int)((u32)colorRGBA[1] << 2) / 5);
				colorRGBA[2] = (u8)((int)((u32)colorRGBA[2] << 2) / 5);

				color = *(Color *)&colorRGBA;
				RECTMENU_DrawOuterRect_HighLevel(&r58, color, 0, ot);
			}
		}
		windowPos = windowPos + 2;

		// Draw 2D Menu rectangle background
		RECTMENU_DrawInnerRect(r, 9, &ot[3]);

		// not screen-space anymore,
		// this is viewport-space
		r->x = 0;
		r->y = 0;

		RECTMENU_DrawRwdBlueRect(r, &D230.characterSelect_BlueRectColors[0], &gGT->pushBuffer[i].ptrOT[0x3ff], &gGT->backBuffer->primMem);
	}
	return;
}
