#include <common.h>

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x8004aa08-0x8004aa60
void SubmitName_RestoreName(s16 param_1)
{
	struct GameTracker *gGT = sdata->gGT;

	// Time Trial or Adventure
	sdata->data10_bbb[0xd] = param_1;

	// copy the last string you typed the last time you were in
	// the OSK menu, back into the menu, avoid typing a second time
	memmove(gGT->currNameEntered, gGT->prevNameEntered, 0x11);

	// "A" or "SAVE"
	s16 cursor = 0;
	if (gGT->currNameEntered[0] != 0)
	{
		cursor = 1001;
	}

	gGT->typeCursorPosition = cursor;
}

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x8004aa60-0x8004b144 for the
// retail path. CTR_NATIVE adds host keyboard shortcuts before retail input.

#ifdef CTR_NATIVE
int kbCurr = 0;
int kbPrev = 0;
void SubmitName_UseKeyboard(int key)
{
	kbCurr = key;
}
#endif

s16 SubmitName_DrawMenu(u16 string)
{
	s16 currNameLength;
	int currNameWidth;
	u32 uVar2;
	u32 cursorCharacter;
	int cursorPosition;
	u32 keyboardCharacter;
	char *currNameEntered;
	u16 strColorBlink;
	u16 blinkWhite;
	int j;
	int i;
	u32 soundID;
	s16 currNameLengthIncrement;
	RECT r;
	u16 stringCopy;
	s16 nameLength;
	s16 local_38;
	int letterID;
	int strlenCurrNameEnteredInt;
	u32 keyboardCharacterTopByte;

	struct GameTracker *gGT = sdata->gGT;

	soundID = 0;
	local_38 = 0;
	nameLength = 0;
	stringCopy = string;
	strlenCurrNameEnteredInt = strlen(gGT->currNameEntered);
	currNameLength = strlenCurrNameEnteredInt;
	currNameEntered = gGT->currNameEntered;
	blinkWhite = ((sdata->typeTimer >> 0) & 1) << 2;

	while (currNameEntered[0] != 0)
	{
		if (currNameEntered[0] > 2)
		{
			nameLength++;
		}

		currNameEntered++;
	}

	cursorPosition = gGT->typeCursorPosition;
	if ((cursorPosition > 38) && (cursorPosition < 1000))
	{
		cursorPosition = 38;
	}

	sdata->typeTimer++;
	letterID = 0;

	// grid of letters, 13x3
	for (i = 0; i < 3; i++)
	{
		for (j = 0; j < 13; j++)
		{
			// Not certain if this is "needed" by the Japan version,
			// or if this was a mistake, by using "int" and pointer to int, that
			// makes a 4-byte string that works fine, have not tested japan yet

#if 0

	// char[4] keyboardString instead of int
			keyboardCharacter = data.unicodeAscii[letterID];
			keyboardCharacterTopByte = keyboardCharacter & 0xff00;
			if ((keyboardCharacter & 0xff00) == 0x1000)
			{
				keyboardCharacter = keyboardCharacter & 0xff;
				keyboardCharacterTopByte = 0;
			}

			// if American letters (char in array of s16)
			if (keyboardCharacterTopByte == 0)
			{
				keyboardString[1] = 0;
				keyboardString[0] = keyboardCharacter;
			}

			// if Japan letters (s16 in array of s16)
			else
			{
				keyboardString[2] = 0;
				keyboardString[0] = (keyboardCharacter << 16) >> 24;
				keyboardString[1] = keyboardCharacter;
			}

#endif

			char keyboardString[3];
			keyboardCharacter = data.unicodeAscii[letterID];
			keyboardCharacterTopByte = keyboardCharacter & 0xff00;
			if (keyboardCharacterTopByte == 0x1000)
			{
				keyboardCharacter &= 0xff;
				keyboardCharacterTopByte = 0;
			}

			if (keyboardCharacterTopByte == 0)
			{
				keyboardString[0] = keyboardCharacter;
				keyboardString[1] = 0;
			}
			else
			{
				keyboardString[0] = (keyboardCharacter << 16) >> 24;
				keyboardString[1] = keyboardCharacter;
				keyboardString[2] = 0;
			}

			// LETTER button blink
			strColorBlink = 0;
			if (cursorPosition == letterID)
			{
				strColorBlink = blinkWhite;
			}

			// LETTER button draw
			DecalFont_DrawLine(keyboardString,

			                   // j*22 + 116,
			                   j * 22 + 116,

			                   i * 18 + 88, FONT_BIG, strColorBlink);

			letterID++;
		}
	}

	DecalFont_DrawLine(sdata->lngStrings[LNG_PLEASE_ENTER_YOUR_NAME], 256, 44, FONT_BIG, (JUSTIFY_CENTER | ORANGE));

	// player name
	DecalFont_DrawLine(gGT->currNameEntered,

	                   // original name max len = 8,
	                   // 192 = 256 - iconW(16) * halfMaxLen(4)
	                   192,

	                   68, FONT_BIG, WHITE);

	if (((sdata->typeTimer & 2) != 0) && (currNameLength < 16))
	{
		currNameWidth = DecalFont_GetLineWidth(gGT->currNameEntered, FONT_BIG);

		DecalFont_DrawLine(sdata->str_underscore, currNameWidth + 192,

		                   68, FONT_BIG, ORANGE);
	}

	// SAVE button blink
	strColorBlink = 0;
	if (cursorPosition == 1001)
	{
		strColorBlink = blinkWhite;
	}

	// SAVE button draw
	DecalFont_DrawLine(sdata->lngStrings[stringCopy],

	                   // 472 is (r.x + r.w - 8)
	                   472,

	                   150, FONT_BIG, (JUSTIFY_RIGHT | strColorBlink));

	// CANCEL button blink
	strColorBlink = 0;
	if (cursorPosition == 1000)
	{
		strColorBlink = blinkWhite;
	}

	DecalFont_DrawLine(sdata->lngStrings[LNG_CANCEL],

	                   // 40 is (r.x + 8)
	                   40,

	                   150, 1, strColorBlink);

	// leftX = 32 (256-224)
	// rightX = 480 (256+224)
	// r.w = 32
	// r.w = 448

	// in 16x9,
	// subtract 1/8 from r.x
	// subtract 1/4 from r.w
	r.x = 32;
	r.w = 448;

	r.y = 62;
	r.h = 2;
	Color color;
	color.self = sdata->battleSetup_Color_UI_1;
	RECTMENU_DrawOuterRect_Edge(&r, color, 0x20, gGT->backBuffer->otMem.uiOT);

	r.y = 39;
	r.h = 130;
	RECTMENU_DrawInnerRect(&r, 0, gGT->backBuffer->otMem.uiOT);

	int tap = sdata->buttonTapPerPlayer[0];

#ifdef CTR_NATIVE
	// NOTE(aalhendi): native keyboard shortcut; retail input remains gamepad-driven.

	if (kbCurr != kbPrev)
	{
		// Between A and Z
		if ((kbCurr >= 4) && (kbCurr <= 29))
		{
			// SDL_KEY_A = 4
			kbCurr -= 4;

			// cursor position of 'A'
			kbCurr += 0;

			cursorPosition = kbCurr;
			soundID = 1;
			tap = BTN_CIRCLE;
		}

		// '0' key
		if (kbCurr == 39)
		{
			cursorPosition = 2 * 13;
			tap = BTN_CIRCLE;
		}

		// Between 1 and 9
		if ((kbCurr >= 30) && (kbCurr <= 38))
		{
			// SDL_SCANCODE_1 = 30
			kbCurr -= 30;

			// cursor position of '1'
			kbCurr += 2 * 13 + 1;

			cursorPosition = kbCurr;
			tap = BTN_CIRCLE;
		}

		// Escape
		if (kbCurr == 41)
		{
			cursorPosition = 1000;
			tap = BTN_CIRCLE;
		}

		// Backspace
		if (kbCurr == 42)
		{
			cursorPosition = kbCurr - 4;
			tap = BTN_CIRCLE;
		}

		// Space
		if (kbCurr == 44)
		{
			cursorPosition = 37;
			tap = BTN_CIRCLE;
		}
	}

	kbPrev = kbCurr;

	// Press Enter
	if (NikoGetEnterKey())
	{
		cursorPosition = 1001;
		tap = BTN_CIRCLE;
	}

#endif

	if ((tap & (BTN_UP | BTN_DOWN | BTN_LEFT | BTN_RIGHT)) == 0)
	{
		if ((tap & BTN_START) == 0)
		{
			if ((tap & (BTN_TRIANGLE | BTN_SQUARE_one)) == 0)
			{
				if (tap & (BTN_CIRCLE | BTN_CROSS_one))
				{
					// Go Back button
					if (cursorPosition == 38)
					{
						soundID = 2;
						if (currNameLength != 0)
						{
							gGT->currNameEntered[currNameLength - 1] = 0;
						}
					}

					// not Go Back button
					else if (cursorPosition < 38)
					{
						// Save or Cancel
						cursorCharacter = data.unicodeAscii[cursorPosition];
						if ((data.unicodeAscii[cursorPosition] & 0xff00) == 0x1000)
						{
							cursorCharacter &= 0xff;
						}

						// too many letters
						soundID = 5;

						if (nameLength < 8)
						{
							soundID = 1;

							currNameLengthIncrement = currNameLength;
							if (cursorCharacter & 0xff00)
							{
								currNameLengthIncrement = currNameLength + 1;
								gGT->currNameEntered[currNameLength] = (cursorCharacter << 16) >> 24;
							}

							gGT->currNameEntered[currNameLengthIncrement] = cursorCharacter;
						}
					}

					else
					{
						// SAVE button
						if (cursorPosition == 1001)
						{
							soundID = 2;
							local_38 = 1;
							memmove(gGT->prevNameEntered, gGT->currNameEntered, 0x11);
						}

						// CANCEL button
						else
						{
							soundID = 0;
							if (cursorPosition != 1000)
							{
								goto LAB_8004b0dc;
							}
							soundID = 3;
							local_38 = -1;
						}

						RECTMENU_ClearInput();
					}
				}
			}

			// Backspace
			else
			{
				// no sound
				soundID = 0;
				if (currNameLength != 0)
				{
					// go back one character
					soundID = 4;
					gGT->currNameEntered[currNameLength - 1] = 0;

					if (gGT->currNameEntered[currNameLength - 2] < 3)
					{
						gGT->currNameEntered[currNameLength - 2] = 0;
					}
				}
			}
		}
		else
		{
			soundID = 3;
			if (cursorPosition == 1000)
			{
				local_38 = -1;
			}
			else
			{
				cursorPosition = 1000;
				soundID = 1;
			}
		}
	}
	else
	{
		int tap = sdata->buttonTapPerPlayer[0];

		if (tap & BTN_UP)
		{
			cursorPosition -= 13;
		}
		if (tap & BTN_DOWN)
		{
			cursorPosition += 13;
		}
		if (tap & BTN_LEFT)
		{
			cursorPosition--;
		}
		if (tap & BTN_RIGHT)
		{
			cursorPosition++;
		}

		if (cursorPosition < 0)
		{
			cursorPosition = 1001;
		}

		uVar2 = cursorPosition - 500;
		if ((38 < cursorPosition) && (cursorPosition < 500))
		{
			cursorPosition = 1001;
			uVar2 = 501;
		}
		if ((uVar2 & 0xffff) < 500)
		{
			cursorPosition = 38;
		}
		soundID = 1;

		// == Naughty Dog Bug ==
		// This used to be 1002, which would allow you to
		// press Right on SAVE, and cursor would go off-screen
		if (cursorPosition > 1001)
		{
			cursorPosition = 0;
		}
	}

LAB_8004b0dc:

	if (soundID != 0)
	{
		// NOTE(aalhendi): ASM-verified NTSC-U 926 0x8004b0dc-0x8004b0f8 for keyboard SFX lookup/play.
		OtherFX_Play(data.soundIndexArray[soundID], 1);
	}
	gGT->typeCursorPosition = cursorPosition;
	return local_38;
}

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x8004b144-0x8004b230.
void SubmitName_MenuProc(struct RectMenu *menu)
{
	struct GameTracker *gGT = sdata->gGT;

	s16 selection = SubmitName_DrawMenu(0x13f);
	menu->rowSelected = selection;

	// not finished yet
	if (selection == 0)
	{
		return;
	}

	// if name entered for Time Trial
	if (sdata->data10_bbb[0xd] == 1)
	{
		// if hit CANCEL
		if (selection < 0)
		{
			// end of race menu with "Save Ghost" option
			extern struct RectMenu menu224;
			sdata->ptrDesiredMenu = &menu224;
		}

		// if hit SAVE
		else
		{
			// GhostMode
			SelectProfile_ToggleMode(0x31);
			sdata->ptrDesiredMenu = &data.menuGhostSelection;
		}
	}

	// if name entered for Adventure
	else if (sdata->data10_bbb[0xd] == 0)
	{
		// if hit CANCEL
		if (selection < 0)
		{
			// Change active Menu back to Adv char select
			sdata->ptrDesiredMenu = CS_Garage_GetMenuPtr();
			CS_Garage_ZoomOut(1);
		}
		else
		{
			// make backup of name entered
			memmove(sdata->advProgress.name, gGT->prevNameEntered, sizeof(gGT->prevNameEntered));

			// AdventureMode
			SelectProfile_ToggleMode(1);
			sdata->ptrDesiredMenu = &data.menuFourAdvProfiles;
		}
	}
}
