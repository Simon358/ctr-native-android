#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800abaa8-0x800abaf0.
void MM_Battle_DrawIcon_Character(struct Icon *icon, int posX, int posY, struct PrimMem *primMem, uint32_t *ot, char transparency, s16 scale)
{
	if (icon == 0)
	{
		return;
	}
	DecalHUD_DrawPolyFT4(icon, posX, posY, primMem, ot, transparency, scale);
}

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800b164c-0x800b1660.
void MM_Battle_CloseSubMenu(struct RectMenu *menu)
{
	menu->state |= 4;
}

enum
{
	BATTLE_TRANSITION_FRAME_COUNT = 0xc,
	BATTLE_TRANSITION_META_COUNT = 8,
	BATTLE_SETTINGS_COUNT = 5,
	BATTLE_TEAM_COUNT = 4,
	BATTLE_VALID_TEAM_MIN = 0,
	BATTLE_VALID_TEAM_MAX = 3,
	BATTLE_ROW_TYPE = 0,
	BATTLE_ROW_LENGTH = 1,
	BATTLE_ROW_TEAMS = 2,
	BATTLE_ROW_WEAPON_TOP = 3,
	BATTLE_ROW_WEAPON_BOTTOM = 4,
	BATTLE_ROW_START = 5,
	BATTLE_ROW_LIFE_COUNT = 10,
	BATTLE_WEAPON_ROW_COUNT = 2,
	BATTLE_WEAPONS_PER_ROW = 6,
	BATTLE_WEAPON_ITEM_COUNT = 11,
	BATTLE_REQUIRED_WEAPON_FLAGS = 0xcde,
	BATTLE_CONFIRM_INPUT = BTN_CROSS_one | BTN_CIRCLE,
	BATTLE_BACK_INPUT = BTN_SQUARE_one | BTN_TRIANGLE,
	BATTLE_MENU_INPUT = BTN_UP | BTN_DOWN | BTN_LEFT | BTN_RIGHT | BATTLE_CONFIRM_INPUT | BATTLE_BACK_INPUT,
	BATTLE_TIME_LIMIT_TYPE_ROW = 1,
	BATTLE_LIFE_LIMIT_TYPE_ROW = 2,
	BATTLE_MINUTES_TO_EVENT_TIME = 0xe100,
	BATTLE_INACTIVE_TEAM_POINTS = -500,
	BATTLE_ICON_SCALE = 0x1000,
	BATTLE_WEAPON_ICON_ROTATE_RIGHT = 1,
	BATTLE_WEAPON_DISABLED_TEXT_COLOR = 0x15,
	BATTLE_WEAPON_ENABLED_TEXT_COLOR = 4,
};

// NOTE(aalhendi): ASM-verified NTSC-U 926 overlay 230 0x800b1660-0x800b1830.
void MM_Battle_DrawIcon_Weapon(struct Icon *icon, u32 posX, int posY, struct PrimMem *primMem, u32 *ot, char transparency, s16 scale, u16 rotation,
                               const u32 *color)
{
	if (!icon)
	{
		return;
	}

	POLY_FT4 *p = (POLY_FT4 *)primMem->cursor;

	u32 uv0 = CTR_ReadU32LE(&icon->texLayout.u0);
	u32 uv1 = CTR_ReadU32LE(&icon->texLayout.u1);
	u32 uv2 = CTR_ReadU32LE(&icon->texLayout.u2);
	s32 scaledWidth = (((s32)((u8)icon->texLayout.u1 - (u8)icon->texLayout.u0)) * scale) >> 0xc;
	s32 scaledHeight = (((s32)((u8)icon->texLayout.v2 - (u8)icon->texLayout.v0)) * scale) >> 0xc;
	u32 code = 0x2c000000;
	u32 packedY = (u32)(u16)posY << 0x10;
	u32 packedBottomY;
	u32 packedSideY;
	u32 rightX;
	u32 sidewaysX;

	if ((u8)transparency != 0)
	{
		code = 0x2e000000;
		uv1 = (uv1 & 0xff9fffff) | ((((u32)(u8)transparency - 1) << 0x15));
	}

	CtrGpu_WriteColorCode(&p->r0, (*color & 0xffffff) | code);
	CtrGpu_WritePackedUVWord(&p->u0, uv0);
	CtrGpu_WritePackedUVWord(&p->u1, uv1);
	CtrGpu_WritePackedUV(&p->u2, (u16)uv2);
	CtrGpu_WritePackedUV(&p->u3, (u16)(uv2 >> 0x10));

	if ((rotation & 1) != 0)
	{
		sidewaysX = posX + scaledHeight;
		packedSideY = packedY + ((u32)scaledWidth << 0x10);

		if ((s16)rotation == BATTLE_WEAPON_ICON_ROTATE_RIGHT)
		{
			CtrGpu_WritePackedXY(&p->x1, posX | packedY);
			CtrGpu_WritePackedXY(&p->x3, sidewaysX | packedY);
			CtrGpu_WritePackedXY(&p->x0, posX | packedSideY);
			CtrGpu_WritePackedXY(&p->x2, sidewaysX | packedSideY);
		}
		else
		{
			CtrGpu_WritePackedXY(&p->x2, posX | packedY);
			CtrGpu_WritePackedXY(&p->x0, sidewaysX | packedY);
			CtrGpu_WritePackedXY(&p->x3, posX | packedSideY);
			CtrGpu_WritePackedXY(&p->x1, sidewaysX | packedSideY);
		}
	}
	else
	{
		rightX = posX + scaledWidth;
		packedBottomY = packedY + ((u32)scaledHeight << 0x10);

		if (((u32)rotation << 0x10) == 0)
		{
			CtrGpu_WritePackedXY(&p->x0, posX | packedY);
			CtrGpu_WritePackedXY(&p->x1, rightX | packedY);
			CtrGpu_WritePackedXY(&p->x2, posX | packedBottomY);
			CtrGpu_WritePackedXY(&p->x3, rightX | packedBottomY);
		}
		else
		{
			CtrGpu_WritePackedXY(&p->x3, posX | packedY);
			CtrGpu_WritePackedXY(&p->x2, rightX | packedY);
			CtrGpu_WritePackedXY(&p->x1, posX | packedBottomY);
			CtrGpu_WritePackedXY(&p->x0, rightX | packedBottomY);
		}
	}

	p->tag = CtrGpu_PackOTTag(*ot, 0x9000000);
	*ot = CtrGpu_PrimToOTLink24(p);

	primMem->cursor = p + 1;
}

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800b1830-0x800b1848.
void MM_Battle_Init(void)
{
	D230.battle_transitionFrames = BATTLE_TRANSITION_FRAME_COUNT;
	D230.battle_transitionState = ENTERING_MENU;
}

void MM_Battle_MenuProc(struct RectMenu *unused)
{
	(void)unused;
	char numPlyr;
	int i, j;
	RECT weaponHighlightRect;
	u16 teamSegmentWidths[4];
	RECT weaponPanelInsetRect;
	s16 teamPlayerCounts[4];
	RECT teamHighlightRect;
	RECT teamColorRect;
	RECT panelRect;

	struct RectMenu *box;
	struct GameTracker *gGT = sdata->gGT;
	s16 afterLengthY;
	int teamPanelX;

	// save all five battle settings,
	// these are selected rows from all battle options
	for (i = 0; i < BATTLE_SETTINGS_COUNT; i++)
	{
		D230.battleMenuArray[i]->rowSelected = sdata->battleSettings[i];
	}

	s16 nextTransitionFrames = D230.battle_transitionFrames;
	if (D230.battle_transitionState != IN_MENU)
	{
		if ((s16)D230.battle_transitionState < 2)
		{
			// if transitioning in
			if (D230.battle_transitionState == ENTERING_MENU)
			{
				MM_TransitionInOut(&D230.transitionMeta_battle[0], (int)D230.battle_transitionFrames, BATTLE_TRANSITION_META_COUNT);

				// reduce frames
				nextTransitionFrames = D230.battle_transitionFrames - 1;

				// if finished
				if (D230.battle_transitionFrames == 0)
				{
					// menu is now in focus
					D230.battle_transitionState = IN_MENU;
					nextTransitionFrames = D230.battle_transitionFrames;
				}
			}
		}
		else
		{
			// if transitioning out
			if (D230.battle_transitionState == EXITING_MENU)
			{
				MM_TransitionInOut(&D230.transitionMeta_battle[0], (int)D230.battle_transitionFrames, BATTLE_TRANSITION_META_COUNT);

				// count frames
				D230.battle_transitionFrames++;

				nextTransitionFrames = D230.battle_transitionFrames;

				// if 12 frames past
				if (BATTLE_TRANSITION_FRAME_COUNT < D230.battle_transitionFrames)
				{
					// if starting race
					if (D230.battle_postTransition_boolStart != 0)
					{
						// passthrough Menu for funcPtr "QueueLoadTrack"
						sdata->ptrDesiredMenu = &data.menuQueueLoadTrack;
						return;
					}

					// == else goBack ==

					MM_TrackSelect_Init();
					sdata->ptrDesiredMenu = &D230.menuTrackSelect;

					return;
				}
			}
		}
	}
	D230.battle_transitionFrames = nextTransitionFrames;

	// There are no battle teams (clear flags)
	gGT->battleSetup.teamFlags = 0;

	// there are no battle teams (clear amount of teams)
	gGT->battleSetup.numTeams = 0;

	numPlyr = gGT->numPlyrNextGame;

	// loop through all players
	for (i = 0; i < numPlyr; i++)
	{
		// get the team of each player
		u32 teamFlag = (s16)(1 << gGT->battleSetup.teamOfEachPlayer[i]);

		// If we have not accounted for this team existing
		if ((gGT->battleSetup.teamFlags & teamFlag) == 0)
		{
			// This team now exists
			gGT->battleSetup.teamFlags |= teamFlag;

			// increase number of teams
			gGT->battleSetup.numTeams++;
		}
	}

	// Reset team points
	for (int i = 0; i < BATTLE_TEAM_COUNT; i++)
	{
		if ((gGT->battleSetup.teamFlags & (1 << i)) == 0)
		{
			gGT->battleSetup.pointsPerTeam[i] = BATTLE_INACTIVE_TEAM_POINTS;
		}
		else
		{
			gGT->battleSetup.pointsPerTeam[i] = 0;
		}
	}

	// Related to Battle mode
	if ((
	        // If number of teams is less than 2
	        ((gGT->battleSetup.numTeams) < 2) ||

	        // If no weapons are slected
	        ((gGT->battleSetup.enabledWeapons & BATTLE_REQUIRED_WEAPON_FLAGS) == 0)) &&

	    // If you are hovering over row 5 (Start Battle)
	    (sdata->battleSetupRowHighlighted == BATTLE_ROW_START))
	{
		// Move cursor back to row 4 (in weapons selection)
		sdata->battleSetupRowHighlighted = BATTLE_ROW_WEAPON_BOTTOM;
	}

	for (i = 0; i < numPlyr; i++)
	{
		// If you are selecting "Teams" row
		if (sdata->battleSetupRowHighlighted == BATTLE_ROW_TEAMS)
		{
			// If you press Left on D-Pad or move stick to the Left
			if ((sdata->buttonTapPerPlayer[i] & 4) != 0)
			{
				// If you have room to move left
				// if your team number is more than 0
				if (BATTLE_VALID_TEAM_MIN < gGT->battleSetup.teamOfEachPlayer[i])
				{
					// play sound
					// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800b1b54-0x800b1b6c for battle team-left SFX.
					OtherFX_Play(0, 1);

					// Move your icon to the left
					gGT->battleSetup.teamOfEachPlayer[i]--;
				}

				// clear the gamepad input so that it
				// does not use this frame's input on the next frame
				sdata->buttonTapPerPlayer[i] = 0;
			}

			// If you press Right on D-Pad or move stick to the Right
			if ((sdata->buttonTapPerPlayer[i] & 8) != 0)
			{
				// If there is room to move right,
				// If your team number is less than 3
				if (gGT->battleSetup.teamOfEachPlayer[i] < BATTLE_VALID_TEAM_MAX)
				{
					// play sound
					// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800b1bc0-0x800b1bd8 for battle team-right SFX.
					OtherFX_Play(0, 1);

					// Move your icon to the right
					gGT->battleSetup.teamOfEachPlayer[i]++;
				}

				// clear the gamepad input so that it
				// does not use this frame's input on the next frame
				sdata->buttonTapPerPlayer[i] = 0;
			}
		}
	}

	// make a copy of the row you have highlighted
	s16 previousHighlightedRow = sdata->battleSetupRowHighlighted;

	if ((D230.battle_transitionState == IN_MENU) &&

	    // If you press D-pad or Cross, Square, Triangle, Circle
	    ((sdata->buttonTapPerPlayer[0] & BATTLE_MENU_INPUT) != 0))
	{
		// if you are not in any drop-down menu
		if ((s16)sdata->battleSetupExpandMenu < 0)
		{
			int buttonTapP1 = sdata->buttonTapPerPlayer[0];

			// If you dont press Up
			if ((buttonTapP1 & 1) == 0)
			{
				// If you dont press Down
				if ((buttonTapP1 & 2) == 0)
				{
					// If you dont press Left
					if ((buttonTapP1 & 4) == 0)
					{
						// If you dont press Right
						if ((buttonTapP1 & 8) == 0)
						{
							// If you dont press Cross or Circle
							if ((buttonTapP1 & BATTLE_CONFIRM_INPUT) == 0)
							{
								// If you press Square or Trianlge
								if ((buttonTapP1 & BATTLE_BACK_INPUT) != 0)
								{
									// Play "Go Back" sound
									// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800b20f8-0x800b2110 for battle setup back SFX.
									OtherFX_Play(2, 1);

									// go back when transition is done, dont start race
									D230.battle_postTransition_boolStart = 0;

									// start transition out
									D230.battle_transitionState = EXITING_MENU;
								}
							}

							// If you press Cross or Circle
							else
							{
								// Play sound
								// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800b1de4-0x800b1df4 for battle setup confirm SFX.
								OtherFX_Play(1, 1);

								switch (sdata->battleSetupRowHighlighted)
								{
									// If row selected can be
									// expanded (type, length, lives)
								case BATTLE_ROW_TYPE:
								case BATTLE_ROW_LENGTH:
								case BATTLE_ROW_LIFE_COUNT:
									// Row expanded = row selected
									sdata->battleSetupExpandMenu = sdata->battleSetupRowHighlighted;
									break;

									// If row selected is 3 or 4
								case BATTLE_ROW_WEAPON_TOP:
								case BATTLE_ROW_WEAPON_BOTTOM:
								{
									// Enable or disable a weapon when you click it
									int weaponIndex =
									    (int)sdata->battleSetupWeaponHighlighted +
									    ((int)(((u32)sdata->battleSetupRowHighlighted - BATTLE_ROW_WEAPON_TOP) * 0x10000) >> 0x10) * BATTLE_WEAPONS_PER_ROW;
									gGT->battleSetup.enabledWeapons ^= D230.battleWeaponItems[weaponIndex].enabledWeaponFlag;
									break;
								}

									// If row selected is "start battle",
									// all code below is for starting the battle
								case BATTLE_ROW_START:

									// get game mode, minus some flags
									gGT->gameMode1 &= ~(POINT_LIMIT | LIFE_LIMIT | TIME_LIMIT);

									u32 battleModeFlags = D230.battleTypeGameMode1Flags[D230.menuBattleType.rowSelected];
									gGT->gameMode1 |= battleModeFlags;

									if ((battleModeFlags & TIME_LIMIT) != 0)
									{
										// point limit
										gGT->gameMode1 |= POINT_LIMIT;
									}

									if (((gGT->gameMode1 & LIFE_LIMIT) != 0) && (0 < D230.battleLifeTimeMinutes[D230.menuBattleLengthLifeTime.rowSelected]))
									{
										// time limit
										gGT->gameMode1 |= TIME_LIMIT;
									}

									// set kill limit
									gGT->battleSetup.killLimit = D230.battlePointLimitValues[D230.menuBattleLengthPoints.rowSelected];

									// if time limit
									s32 eventTimeMinutes;
									if (D230.menuBattleType.rowSelected == BATTLE_TIME_LIMIT_TYPE_ROW)
									{
										eventTimeMinutes = D230.battleTimeLimitMinutes[D230.menuBattleLengthTimeTime.rowSelected];
									}

									else
									{
										eventTimeMinutes = D230.battleLifeTimeMinutes[D230.menuBattleLengthLifeTime.rowSelected];
									}

									// set time limit based on number of minutes
									gGT->originalEventTime = eventTimeMinutes;
									i = gGT->originalEventTime;
									if (0 < i)
									{
										gGT->originalEventTime = i * BATTLE_MINUTES_TO_EVENT_TIME;
									}

									gGT->battleSetup.numWeapons = 0;

									// life limit
									gGT->battleSetup.lifeLimit = D230.battleLifeLimitValues[D230.menuBattleLengthLifeLife.rowSelected];

									// write RNG array of weaponIDs, based on weapon flags,
									// loop through 14 "possible" weapons
									for (u32 weaponFlagIndex = 0; weaponFlagIndex < 0xe; weaponFlagIndex++)
									{
										// bit flag of weapons enabled
										if ((gGT->battleSetup.enabledWeapons & 1 << weaponFlagIndex) != 0)
										{
											// write weaponID in RNG array
											gGT->battleSetup.RNG_itemSetCustom[gGT->battleSetup.numWeapons] = weaponFlagIndex;

											// increment number of weapons RNG can choose from
											gGT->battleSetup.numWeapons++;
										}
									}

									// start battle when transition is done
									D230.battle_postTransition_boolStart = 1;

									// start transition out
									D230.battle_transitionState = EXITING_MENU;

									// check if player changed team,
									// then clear stats if a change happened

									for (i = 0; i < numPlyr; i++)
									{
										if (sdata->teamOfEachPlayer[i] != gGT->battleSetup.teamOfEachPlayer[i])
										{
											MainStats_ClearBattleVS();
										}
										sdata->teamOfEachPlayer[i] = gGT->battleSetup.teamOfEachPlayer[i];
									}

									sdata->buttonTapPerPlayer[1] = 0;
									sdata->buttonTapPerPlayer[2] = 0;
									sdata->buttonTapPerPlayer[3] = 0;
								}
							}
						}

						// If you press Right
						else
						{
							// if row 3 or 4 (weapons)
							if ((u32)sdata->battleSetupRowHighlighted - BATTLE_ROW_WEAPON_TOP < BATTLE_WEAPON_ROW_COUNT)
							{
								// change which weapon is highlighted
								sdata->battleSetupWeaponHighlighted++;
							}

							else
							{
								if ((D230.menuBattleType.rowSelected == BATTLE_LIFE_LIMIT_TYPE_ROW) && (sdata->battleSetupRowHighlighted == BATTLE_ROW_LENGTH))
								{
									sdata->battleSetupRowHighlighted = BATTLE_ROW_LIFE_COUNT;
								}
							}
						}
					}

					// If you press Left
					else
					{
						// if row 3 or 4 (weapons)
						if ((u32)sdata->battleSetupRowHighlighted - BATTLE_ROW_WEAPON_TOP < BATTLE_WEAPON_ROW_COUNT)
						{
							// change which weapon is highlighted
							sdata->battleSetupWeaponHighlighted--;
						}

						else
						{
							if ((D230.menuBattleType.rowSelected == BATTLE_LIFE_LIMIT_TYPE_ROW) && (sdata->battleSetupRowHighlighted == BATTLE_ROW_LIFE_COUNT))
							{
								goto LAB_800b1d7c;
							}
						}
					}
				}

				// If you press Down
				else
				{
					if ((D230.menuBattleType.rowSelected == BATTLE_LIFE_LIMIT_TYPE_ROW) && (sdata->battleSetupRowHighlighted == BATTLE_ROW_LIFE_COUNT))
					{
						sdata->battleSetupRowHighlighted = D230.menuBattleType.rowSelected;
					}
					else
					{
						// Move one row down
						sdata->battleSetupRowHighlighted++;

						// If you go below row 5 (Start Battle)
						if (BATTLE_ROW_START < sdata->battleSetupRowHighlighted)
						{
							// Go back to row 5
							sdata->battleSetupRowHighlighted = BATTLE_ROW_START;
						}
					}
				}
			}

			// If you press Up
			else
			{
				if ((D230.menuBattleType.rowSelected == BATTLE_LIFE_LIMIT_TYPE_ROW) && (sdata->battleSetupRowHighlighted == BATTLE_ROW_LIFE_COUNT))
				{
				LAB_800b1d7c:
					sdata->battleSetupRowHighlighted = D230.battle_transitionState;
				}
				else
				{
					// Go up one row
					sdata->battleSetupRowHighlighted--;

					// If you go above the top row (0)
					if (sdata->battleSetupRowHighlighted < 0)
					{
						// Go back to the top row
						sdata->battleSetupRowHighlighted = BATTLE_ROW_TYPE;
					}
				}
			}

			// If you are a row less than 5,
			// any row except the bottom
			if ((u32)sdata->battleSetupRowHighlighted - BATTLE_ROW_WEAPON_TOP < BATTLE_WEAPON_ROW_COUNT)
			{
				i = (u32)sdata->battleSetupRowHighlighted - (BATTLE_ROW_WEAPON_TOP - 1);
				if (sdata->battleSetupWeaponHighlighted < 0)
				{
					sdata->battleSetupWeaponHighlighted = 0;
				}
				if (6 - (i) < (int)sdata->battleSetupWeaponHighlighted)
				{
					sdata->battleSetupWeaponHighlighted = 6 - (s16)i;
				}
			}

			if (sdata->battleSetupRowHighlighted != previousHighlightedRow)
			{
				// Play sound
				// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800b2178-0x800b2194 for battle row-change SFX.
				OtherFX_Play(0, 1);
			}
		}

		// if you are in any drop-down menu
		else
		{
			box = NULL;

			// If you are chosing Length
			// Dropdown menu for Points (5, 10, 15)
			// Dropdown menu for Time (3 minutes, 6 minutes, etc)
			if (sdata->battleSetupExpandMenu == BATTLE_ROW_LENGTH)
			{
				if (D230.menuBattleType.rowSelected == BATTLE_TIME_LIMIT_TYPE_ROW)
				{
					box = &D230.menuBattleLengthTimeTime;
				}
				else
				{
					if ((s16)D230.menuBattleType.rowSelected < 2)
					{
						if (D230.menuBattleType.rowSelected == BATTLE_ROW_TYPE)
						{
							box = &D230.menuBattleLengthPoints;
						}
					}
					else
					{
						if (D230.menuBattleType.rowSelected == BATTLE_LIFE_LIMIT_TYPE_ROW)
						{
							box = &D230.menuBattleLengthLifeTime;
						}
					}
				}
			}

			// If not choosing Length
			else
			{
				// < 2 means 0 or 1, seems pointless,
				// considering the previous != 1 check and next == 0 check
				if (sdata->battleSetupExpandMenu < 2)
				{
					// if you are chosing type of battle
					// Dropdown menu for (Point Limit, Life Limit, TIme Limit)
					if (sdata->battleSetupExpandMenu == BATTLE_ROW_TYPE)
					{
						box = &D230.menuBattleType;
					}
				}

				// sdata->battleSetupExpandMenu == BATTLE_ROW_TEAMS
				else
				{
					// If you're not chosing life limit
					// Dropdown for 3 lives, 6 lives, 9 lives
					if (sdata->battleSetupExpandMenu == BATTLE_ROW_LIFE_COUNT)
					{
						box = &D230.menuBattleLengthLifeLife;
					}
				}
			}
			if (box != NULL)
			{
				RECTMENU_ProcessInput(box);
				if ((box->state & ONLY_DRAW_TITLE) != 0)
				{
					box->state &= ~(ONLY_DRAW_TITLE);

					sdata->battleSetupExpandMenu = -1;
				}
			}
		}

		// clear gamepad input (for menus)
		RECTMENU_ClearInput();
	}

	struct TransitionMeta *tmbattle = &D230.transitionMeta_battle[0];

	DecalFont_DrawLine(sdata->lngStrings[LNG_SETUP_BATTLE], tmbattle[9].currX + 0x100, tmbattle[9].currY + 10, 1, 0xffff8000);

	DecalFont_DrawLine(sdata->lngStrings[LNG_TYPE], tmbattle[1].currX + 0x8c + 0, tmbattle[1].currY + 0x24, 1, 0x4000);

	D230.menuBattleType.state &= ~(0x100 | SHOW_ONLY_HIGHLIT_ROW);

	// if you are not choosing type of battle
	if (sdata->battleSetupExpandMenu != BATTLE_ROW_TYPE)
	{
		D230.menuBattleType.state |= 0x40;
	}

	if (sdata->battleSetupRowHighlighted != BATTLE_ROW_TYPE)
	{
		D230.menuBattleType.state |= 0x100;
	}

	RECTMENU_DrawSelf(&D230.menuBattleType, tmbattle[0].currX + 0x9c + 0, tmbattle[0].currY + 0x24, 0x134);

	s16 menuHeight = 0xd;
	RECTMENU_GetHeight(&D230.menuBattleType, &menuHeight, 0);
	s16 lengthRowY = menuHeight + 0x20;

	DecalFont_DrawLine(sdata->lngStrings[LNG_LENGTH], tmbattle[3].currX + 0x8c + 0, tmbattle[3].currY + lengthRowY + 4, 1, 0x4000);

	if (D230.menuBattleType.rowSelected == BATTLE_TIME_LIMIT_TYPE_ROW)
	{
		box = &D230.menuBattleLengthTimeTime;
	}
	else
	{
		afterLengthY = lengthRowY;
		if (BATTLE_TIME_LIMIT_TYPE_ROW < D230.menuBattleType.rowSelected)
		{
			if (D230.menuBattleType.rowSelected == BATTLE_LIFE_LIMIT_TYPE_ROW)
			{
				D230.menuBattleLengthLifeTime.state &= ~(0x100 | SHOW_ONLY_HIGHLIT_ROW);

				if (sdata->battleSetupExpandMenu != BATTLE_ROW_LENGTH)
				{
					D230.menuBattleLengthLifeTime.state |= 0x40;
				}
				if (sdata->battleSetupRowHighlighted != BATTLE_ROW_LENGTH)
				{
					D230.menuBattleLengthLifeTime.state |= 0x100;
				}

				RECTMENU_DrawSelf(&D230.menuBattleLengthLifeTime, tmbattle[2].currX + 0x9c + 0, tmbattle[2].currY + lengthRowY + 4, 0x8e);

				D230.menuBattleLengthLifeLife.state &= ~(0x100 | SHOW_ONLY_HIGHLIT_ROW);

				if (sdata->battleSetupExpandMenu != BATTLE_ROW_LIFE_COUNT)
				{
					D230.menuBattleLengthLifeLife.state |= 0x40;
				}
				if (sdata->battleSetupRowHighlighted != BATTLE_ROW_LIFE_COUNT)
				{
					D230.menuBattleLengthLifeLife.state |= 0x100;
				}

				RECTMENU_DrawSelf(&D230.menuBattleLengthLifeLife, tmbattle[2].currX + 0x142 - 0, tmbattle[2].currY + lengthRowY + 4, 0x8e);

				s16 lifeTimeMenuHeight = 0xd;
				RECTMENU_GetHeight(&D230.menuBattleLengthLifeTime, &lifeTimeMenuHeight, 0);
				s16 lifeCountMenuHeight = 0xd;
				RECTMENU_GetHeight(&D230.menuBattleLengthLifeLife, &lifeCountMenuHeight, 0);
				afterLengthY = lifeCountMenuHeight + lengthRowY;
				if (lifeCountMenuHeight < lifeTimeMenuHeight)
				{
					afterLengthY = lifeTimeMenuHeight + lengthRowY;
				}
			}
			goto LAB_800b25f0;
		}
		if (D230.menuBattleType.rowSelected != BATTLE_ROW_TYPE)
		{
			goto LAB_800b25f0;
		}
		box = &D230.menuBattleLengthPoints;
	}

	box->state &= ~(0x100 | SHOW_ONLY_HIGHLIT_ROW);

	if (sdata->battleSetupExpandMenu != BATTLE_ROW_LENGTH)
	{
		box->state |= 0x40;
	}
	if (sdata->battleSetupRowHighlighted != BATTLE_ROW_LENGTH)
	{
		box->state |= 0x100;
	}

	RECTMENU_DrawSelf(box, tmbattle[2].currX + 0x9c + 0, tmbattle[2].currY + lengthRowY + 4, 0x134);

	menuHeight = 0xd;
	RECTMENU_GetHeight(box, &menuHeight, 0);
	afterLengthY = menuHeight + lengthRowY;

LAB_800b25f0:

	teamPanelX = 0x9f;

	DecalFont_DrawLine(sdata->lngStrings[LNG_TEAMS], tmbattle[5].currX + 0x8c + 0, tmbattle[5].currY + afterLengthY + 10, 1, 0x4000);

	i = 4;
	int finalTeamWidth = 0;

	for (j = 0; j < BATTLE_TEAM_COUNT; j++)
	{
		int totalTeamWidth = i;
		int teamIndex = (int)(s16)j;
		teamPlayerCounts[teamIndex] = 0;
		teamSegmentWidths[teamIndex] = 4;

		for (i = 0; i < numPlyr; i++)
		{
			int playerTeam = (int)gGT->battleSetup.teamOfEachPlayer[i];
			if (playerTeam == teamIndex)
			{
				teamPlayerCounts[playerTeam]++;
				teamSegmentWidths[playerTeam] += 0x2a;
				totalTeamWidth = totalTeamWidth + 0x2a;
			}
		}

		finalTeamWidth = totalTeamWidth;
		i = totalTeamWidth + 4;
	}

	u32 remainingTeamWidth = 0x12e - finalTeamWidth;
	i = (int)(remainingTeamWidth);
	u32 sharedTeamPadding = i + 3;
	if ((int)sharedTeamPadding < 0)
	{
		sharedTeamPadding = i + 6;
	}
	sharedTeamPadding = sharedTeamPadding >> 2;

	for (i = 0; i < BATTLE_TEAM_COUNT; i++)
	{
		teamSegmentWidths[i] += sharedTeamPadding;
		remainingTeamWidth = remainingTeamWidth - sharedTeamPadding;
		if ((int)(remainingTeamWidth) < (int)(sharedTeamPadding))
		{
			sharedTeamPadding = remainingTeamWidth;
		}
	}

	uint32_t *ot = gGT->backBuffer->otMem.uiOT;

	for (i = 0; i < BATTLE_TEAM_COUNT; i++)
	{
		u16 teamSegmentWidth = teamSegmentWidths[i];
		int playerIconX = teamPanelX + (teamSegmentWidth >> 1) + (int)teamPlayerCounts[i] * -0x15;

		for (int playerIndex = 0; playerIndex < numPlyr; playerIndex++)
		{
			if (gGT->battleSetup.teamOfEachPlayer[playerIndex] == i)
			{
				s16 iconX = (s16)playerIconX;
				playerIconX = playerIconX + 0x2a;

				MM_Battle_DrawIcon_Character(gGT->ptrIcons[data.MetaDataCharacters[data.characterIDs[playerIndex]].iconID],
				                             (int)tmbattle[4].currX + (int)iconX + 0, (int)tmbattle[4].currY + (int)afterLengthY + 6,

				                             &gGT->backBuffer->primMem, gGT->pushBuffer_UI.ptrOT, 1, BATTLE_ICON_SCALE);
			}
		}

		teamColorRect.h = 0x1a;
		teamColorRect.x = tmbattle[4].currX + (s16)teamPanelX + 0;
		teamColorRect.y = tmbattle[4].currY + afterLengthY + 5;
		teamPanelX = teamPanelX + (u32)teamSegmentWidth;

		teamColorRect.w = teamSegmentWidth;

		Color color;
		color.self = *data.ptrColor[PLAYER_BLUE + i];
		CTR_Box_DrawSolidBox(&teamColorRect, color, ot);
	}

	if (sdata->battleSetupRowHighlighted == BATTLE_ROW_TEAMS)
	{
		teamHighlightRect.w = 0x134;
		teamHighlightRect.h = 0x1e;
		teamHighlightRect.x = tmbattle[4].currX + 0x9c + 0;
		teamHighlightRect.y = tmbattle[4].currY + afterLengthY + 3;

		CTR_Box_DrawClearBox(&teamHighlightRect, &sdata->menuRowHighlight_Normal, TRANS_50_DECAL, (uint32_t *)ot);
	}

	panelRect.w = 0x140;
	panelRect.h = 0x24;
	panelRect.x = tmbattle[4].currX + 0x96 + 0;
	panelRect.y = tmbattle[4].currY + afterLengthY;

	// Draw 2D Menu rectangle background
	RECTMENU_DrawInnerRect(&panelRect, 0, ot);

	DecalFont_DrawLine(sdata->lngStrings[LNG_WEAPONS], tmbattle[7].currX + 0x8c + 0, tmbattle[7].currY + afterLengthY + 0x44, 1, 0x4000);

	// make flashing color for error message

	// set default color
	s16 flashingErrorColor = -0x7fff;

	// if time on timer is odd
	if ((sdata->frameCounter & 1) != 0)
	{
		// change color
		flashingErrorColor = -0x7ffd;
	}

	i = 0;

	// If you have no weapons selected, which are in flags "0xcde"
	if ((gGT->battleSetup.enabledWeapons & BATTLE_REQUIRED_WEAPON_FLAGS) == 0)
	{
		// THERE MUST BE
		j = 0xac;

		// AT LEAST ONE WEAPON
		i = 0xad;
	}

	// if you have at least one weapon selected
	else
	{
		// No error so far
		j = 0;

		// If number of teams is less than 2
		if (gGT->battleSetup.numTeams < 2)
		{
			// THERE MUST BE
			j = 0xaa;

			// TWO OR MORE TEAMS
			i = 0xab;
		}
	}

	// If you have no errors that prevent
	// the player from starting the Battle
	if (j == 0)
	{
		D230.menuBattleStartGame.state &= ~(0x100 | SHOW_ONLY_HIGHLIT_ROW);

		if (sdata->battleSetupExpandMenu != BATTLE_ROW_START)
		{
			D230.menuBattleStartGame.state |= SHOW_ONLY_HIGHLIT_ROW;
		}
		if (sdata->battleSetupRowHighlighted != BATTLE_ROW_START)
		{
			D230.menuBattleStartGame.state |= 0x100;
		}
		RECTMENU_DrawSelf(&D230.menuBattleStartGame, tmbattle[8].currX + 0x9c + 0, tmbattle[8].currY + afterLengthY + 0x78, 0x134);

		menuHeight = 0xd;
		RECTMENU_GetHeight(&D230.menuBattleStartGame, &menuHeight, 0);
	}

	// If you have no errors that prevent
	// the player from starting the Battle
	else
	{
		// Print two lines of error text,
		// one on top of the other, centered text,
		// 0x100 for halfway on the X-axis,
		// flashing color

		DecalFont_DrawLine(sdata->lngStrings[j], 0x100, afterLengthY + 0x6a, 1, (int)flashingErrorColor);
		DecalFont_DrawLine(sdata->lngStrings[i], 0x100, afterLengthY + 0x7a, 1, (int)flashingErrorColor);
	}
	i = 0;
	panelRect.w = 0x140;
	panelRect.h = 0x44;
	panelRect.x = tmbattle[6].currX + 0x96 + 0;
	panelRect.y = tmbattle[6].currY + afterLengthY + 0x2a;

	// Loop through all 11 weapon icons
	for (i = 0; i < BATTLE_WEAPON_ITEM_COUNT; i++)
	{
		int weaponIndex = (int)(s16)i;
		j = (weaponIndex / BATTLE_WEAPONS_PER_ROW);

		const struct BattleWeaponMenuItem *weaponItem = &D230.battleWeaponItems[weaponIndex];
		const u32 *weaponColor = &D230.battleWeaponEnabledColor;
		u32 weaponTextColor = BATTLE_WEAPON_ENABLED_TEXT_COLOR;

		// Check if this weapon is not enabled
		if ((gGT->battleSetup.enabledWeapons & weaponItem->enabledWeaponFlag) == 0)
		{
			weaponColor = &D230.battleWeaponDisabledColor;
			weaponTextColor = BATTLE_WEAPON_DISABLED_TEXT_COLOR;
		}

		// weaponIndex % 6
		// Go to 2nd row after 6th icon
		int weaponPosX = (u32)panelRect.x + 6 + (weaponIndex % BATTLE_WEAPONS_PER_ROW) * 0x34 + j * 0x1a;

		j = (u32)panelRect.y + 2 + j * 0x20;

		// If the icon is bowling bomb or missile on the 2nd row
		if (((i - 7U) & 0xffff) < 2)
		{
			// draw the "3" over the icons
			DecalFont_DrawLine(&R230.s_3[0], weaponPosX, j, 2, weaponTextColor);
		}

		MM_Battle_DrawIcon_Weapon(gGT->ptrIcons[weaponItem->iconID], weaponPosX, j, &gGT->backBuffer->primMem, (u32 *)gGT->pushBuffer_UI.ptrOT, 1,
		                          BATTLE_ICON_SCALE, BATTLE_WEAPON_ICON_ROTATE_RIGHT, weaponColor);
	}

	if ((u32)sdata->battleSetupRowHighlighted - BATTLE_ROW_WEAPON_TOP < BATTLE_WEAPON_ROW_COUNT)
	{
		s16 highlightedWeaponX = panelRect.x + sdata->battleSetupWeaponHighlighted * 0x34;
		weaponHighlightRect.x = highlightedWeaponX + 4;
		if (sdata->battleSetupRowHighlighted == BATTLE_ROW_WEAPON_BOTTOM)
		{
			weaponHighlightRect.x = highlightedWeaponX + 0x1e;
		}
		weaponHighlightRect.w = 0x34;
		weaponHighlightRect.h = 0x20;
		weaponHighlightRect.y = panelRect.y + (sdata->battleSetupRowHighlighted - BATTLE_ROW_WEAPON_TOP) * 0x20 + 2;

		CTR_Box_DrawClearBox(&weaponHighlightRect, &sdata->menuRowHighlight_Normal, TRANS_50_DECAL, ot);
	}

	weaponPanelInsetRect.x = panelRect.x + 3;
	weaponPanelInsetRect.y = panelRect.y + 2;
	weaponPanelInsetRect.w = panelRect.w - 6;
	weaponPanelInsetRect.h = panelRect.h - 4;

	Color boxColor = {.self = D230.battleWeaponPanelColor};
	CTR_Box_DrawClearBox(&weaponPanelInsetRect, &boxColor, TRANS_50_DECAL, ot);

	RECTMENU_DrawInnerRect(&panelRect, 0, ot);

	// save all five battle settings
	// these are selected rows from all battle options
	for (i = 0; i < BATTLE_SETTINGS_COUNT; i++)
	{
		sdata->battleSettings[i] = D230.battleMenuArray[i]->rowSelected;
	}
	return;
}
