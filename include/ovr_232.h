#ifndef CTR_NATIVE_OVR_232_H
#define CTR_NATIVE_OVR_232_H


struct MaskHint
{
	struct MaskHint *self; // why?
	s16 scale;

	// end of struct
};

enum BossGarageDoorDirectionID
{
	BOSS_GARAGE_DOOR_CLOSING = -1,
	BOSS_GARAGE_DOOR_STOPPED = 0,
	BOSS_GARAGE_DOOR_OPENING = 1,
};

typedef s32 BossGarageDoorDirection;

CTR_STATIC_ASSERT(BOSS_GARAGE_DOOR_CLOSING == -1);
CTR_STATIC_ASSERT(BOSS_GARAGE_DOOR_STOPPED == 0);
CTR_STATIC_ASSERT(BOSS_GARAGE_DOOR_OPENING == 1);
CTR_STATIC_ASSERT(sizeof(BossGarageDoorDirection) == 0x4);

struct BossGarageDoor
{
	BossGarageDoorDirection direction;

	// so you can't spam open/close
	int cooldown;

	struct Instance *garageTopInst;

	SVec3 rot;
	s16 _pad_rot;

	// 0x14 bytes large
};

CTR_STATIC_ASSERT(sizeof(struct BossGarageDoor) == 0x14);

#if 0
struct AdvPause {

};
#endif

enum WoodDoorCamFlags
{
	WdCam_None = 0,
	WdCam_FlyingOut = 1,
	WdCam_FullyOut = 2,
	WdCam_FlyingIn = 4,
	WdCam_CutscenePlaying = 0x10
};

typedef s16 WoodDoorCamFlagSet;

CTR_STATIC_ASSERT(WdCam_None == 0);
CTR_STATIC_ASSERT(WdCam_FlyingOut == 1);
CTR_STATIC_ASSERT(WdCam_FullyOut == 2);
CTR_STATIC_ASSERT(WdCam_FlyingIn == 4);
CTR_STATIC_ASSERT(WdCam_CutscenePlaying == 0x10);
CTR_STATIC_ASSERT(sizeof(WoodDoorCamFlagSet) == 0x2);

struct WoodDoor
{
	struct Instance *otherDoor;
	struct Instance *keyInst[4];

	// 0x14 (5)
	SVec3 doorRot;
	s16 _pad_doorRot;

	// 0x1c (7)
	WoodDoorCamFlagSet camFlags;
	s16 camTimer_unused;

	// 0x20 (8)
	int hudFlags;

	// 0x24 (9)
	s16 frameCount_unused;
	s16 frameCount_doorOpenAnim;

	// 0x28 (10)
	SVec3 keyRot;
	s16 _pad_keyRot;

	// 0x30 (12)
	s16 keyOrbit;
	s16 keyShrinkFrame;

	// 0x34
	s16 doorID;
	s16 padding_0x36;

	// 0x38 bytes large
};

CTR_STATIC_ASSERT(sizeof(struct WoodDoor) == 0x38);

enum WarpPadInstanceSet
{
	// instances that appear
	// when warppad is closed
	WPIS_CLOSED_ITEM = 0,
	WPIS_CLOSED_X,
	WPIS_CLOSED_1S,
	WPIS_CLOSED_10S,

	// istances that appear
	// when warppad is open
	WPIS_OPEN_BEAM,
	WPIS_OPEN_RING1,
	WPIS_OPEN_RING2,
	WPIS_OPEN_PRIZE1,
	WPIS_OPEN_PRIZE2,
	WPIS_OPEN_PRIZE3,

	WPIS_NUM_INSTANCES
};

enum AdventureHubItemTypeID
{
	// Boss-garage and marker values are data IDs. Route values select lock behavior,
	// not a unique source/destination pair.
	AH_HUB_ITEM_RIPPER_ROO_GARAGE = 0,
	AH_HUB_ITEM_PAPU_PAPU_GARAGE,
	AH_HUB_ITEM_KOMODO_JOE_GARAGE,
	AH_HUB_ITEM_PINSTRIPE_GARAGE,
	AH_HUB_ITEM_OXIDE_WARPPAD,
	AH_HUB_ITEM_SAVE_LOAD_MARKER = 100,
	AH_HUB_ITEM_ROUTE_KEY1_IF_BEACH = -1,
	AH_HUB_ITEM_ROUTE_OPEN_A = -2,
	AH_HUB_ITEM_ROUTE_OPEN_B = -3,
	AH_HUB_ITEM_ROUTE_KEY2 = -4,
	AH_HUB_ITEM_ROUTE_KEY3 = -5,
};

typedef s16 AdventureHubItemType;

CTR_STATIC_ASSERT(AH_HUB_ITEM_RIPPER_ROO_GARAGE == 0);
CTR_STATIC_ASSERT(AH_HUB_ITEM_PAPU_PAPU_GARAGE == 1);
CTR_STATIC_ASSERT(AH_HUB_ITEM_KOMODO_JOE_GARAGE == 2);
CTR_STATIC_ASSERT(AH_HUB_ITEM_PINSTRIPE_GARAGE == 3);
CTR_STATIC_ASSERT(AH_HUB_ITEM_OXIDE_WARPPAD == 4);
CTR_STATIC_ASSERT(AH_HUB_ITEM_SAVE_LOAD_MARKER == 100);
CTR_STATIC_ASSERT(AH_HUB_ITEM_ROUTE_KEY1_IF_BEACH == -1);
CTR_STATIC_ASSERT(AH_HUB_ITEM_ROUTE_OPEN_A == -2);
CTR_STATIC_ASSERT(AH_HUB_ITEM_ROUTE_OPEN_B == -3);
CTR_STATIC_ASSERT(AH_HUB_ITEM_ROUTE_KEY2 == -4);
CTR_STATIC_ASSERT(AH_HUB_ITEM_ROUTE_KEY3 == -5);
CTR_STATIC_ASSERT(sizeof(AdventureHubItemType) == 0x2);

enum AHWarpPadVisualState
{
	AH_WP_VISUAL_LOCKED = 0,
	AH_WP_VISUAL_TROPHY_OPEN,
	AH_WP_VISUAL_COMPLETE,
	AH_WP_VISUAL_RELIC_TOKEN_OPEN,
	AH_WP_VISUAL_COLOR_CYCLE_OPEN,
	AH_WP_VISUAL_COUNT,
};

enum AHPausePageType
{
	AH_PAUSE_PAGE_HUB = 0,
	AH_PAUSE_PAGE_TOKEN_TOTALS,
	AH_PAUSE_PAGE_RELIC_TOTALS,
};

enum AHPauseIconIndex
{
	AH_PAUSE_ICON_NONE = -1,
	AH_PAUSE_ICON_FIRST_GEM = 0,
	AH_PAUSE_ICON_BOSS_KEY = 5,
	AH_PAUSE_ICON_SAPPHIRE_RELIC = 6,
	AH_PAUSE_ICON_GOLD_RELIC = 7,
	AH_PAUSE_ICON_PLATINUM_RELIC = 8,
	AH_PAUSE_ICON_FIRST_TOKEN = 9,
	AH_PAUSE_ICON_TROPHY = 14,
};

enum AHPauseConstants
{
	AH_PAUSE_MENU_PAGE_COUNT = 7,
	AH_PAUSE_MEMBER_COUNT = 0xe,
	AH_PAUSE_ICON_COUNT = 15,
	AH_PAUSE_GEM_ICON_COUNT = 5,
	AH_PAUSE_TOKEN_ICON_COUNT = 5,
	AH_PAUSE_PURPLE_TOKEN_INDEX = 4,
	AH_PAUSE_RELIC_ICON_COUNT = 3,
	AH_PAUSE_CTR_TOKEN_TRACK_COUNT = 0x10,
	AH_PAUSE_RELIC_TRACK_COUNT = 0x12,
	AH_PAUSE_PURPLE_TOKEN_COUNT = 4,
};

struct WarpPad
{
	// 0x0
	struct Instance *inst[WPIS_NUM_INSTANCES];

	// 0x28
	SVec3 spinRot_Prize;
	s16 _pad_spinRot_Prize;

	// 0x30
	SVec3 spinRot_Wisp[2];
	s16 _pad_spinRot_Wisp[2];

	// 0x40
	SVec3 spinRot_Beam;
	s16 _pad_spinRot_Beam;

	// 0x48
	SVec3 spinRot_Rewards;
	s16 _pad_spinRot_Rewards;

	// 0x50
	SVec3 lightDirGem;
	s16 _pad_lightDirGem;

	// 0x58
	SVec3 lightDirRelic;
	s16 _pad_lightDirRelic;

	// 0x60
	SVec3 lightDirToken;
	s16 _pad_lightDirToken;

	// 0x68
	s16 digit10s;

	// 0x6a
	s16 digit1s;

	// 0x6c (1b*4)
	s16 levelID;

	// 0x6e
	// 0/3    1/3     2/3
	// 0x0    0x555   0xAAA
	u16 thirds[3];

	// 0x74
	s16 boolEnteredWarppad;

	// 0x76
	s16 framesWarping;

	// 0x78 -- size
};

struct SaveObj
{
	// 0x0
	struct Instance *inst;
	// 0x4
	u16 flags;
	// 0x6
	s16 scanlineFrame;
	// 0x8
	u8 hudFlagBackup;

	// total size unk
};

struct OverlayRDATA_232
{
	// 0x800aba3c
	s16 battleTrackPurpleTokenOffset[8];

	// 0x800aba4c
	s16 bossTracks[6];

	// 0x800aba58
	s16 bossIDs[6];

	// 0x800aba64
	char s_garage[8];
	char s_garagetop[0xc];
	char s_saveobj[8];
	char s_scan[8];
	char s_key[4];

	// 0x800aba8c
	s16 keyFrame[0xc];

	// 0x800abaa4
	char s_door[8];

	// 0x800abaac
	u32 warppadColorJumpTable[AH_WP_VISUAL_COUNT];

	// 0x800abac0
	u32 unk_800abac0;

	// 0x800abac4
	char s_format[8];

	// 0x800abacc
	char s_PAUSE[8];

	// 0x800abad4
	char s_pause[8];
};

struct OverlayDATA_232
{
	// 800b4ddc (5 light directions plus padding)
	SVec3 lightDirGem[5];
	s16 _pad_lightDirGemTable;

	// 800b4dfc (5 light directions plus padding)
	SVec3 lightDirRelic[5];
	s16 _pad_lightDirRelicTable;

	// 800b4e1c (5 light directions plus padding)
	SVec3 lightDirToken[5];
	s16 _pad_lightDirTokenTable;

	// 800b4e3c
	struct MenuRow rowsTokenRelic[3];

	// 800b4e50
	struct RectMenu menuTokenRelic;

	// 800b4e7c
	s16 arrKeysNeeded[5];

	// 800b4e86
	s16 levelID;

	// 800b4e88
	int battleCrystalChallengeTime[7];

	// 800b4ea4
	s16 saveObjCameraOffset[4];

	// 800b4eac
	s16 primOffsetXY_LoadSave[5 * 2];

	// 800b4ec0
	s16 primOffsetXY_HubArrow[5 * 2];

	struct HubItem
	{
		// 0x0
		s16 posX;
		s16 posY;

		// 0x4
		s16 angle;

		// 0x6
		AdventureHubItemType iconType;

		// 0x8 -- size
	}
	// 800b4ed4
	// 2 arrows, boss, save/load, null(0xFFFF)
	hubItems_hub1[5],
	    // 800b4efc
	    hubItems_hub2[5],
	    // 800b4f24
	    hubItems_hub3[5],
	    // 800b4f4c (3 arrows)
	    hubItems_hub4[6],
	    // 800b4f7c (1 arrow)
	    hubItems_hub5[4];

	// 800b4f9c -- array of pointers:
	//		800b4ed4 800b4efc 800b4f24
	//		800b4f4c 800b4f7c
	s16 *hubItemsXY_ptrArray[5];

	// 800b4fb0
	s16 hubArrowXY_Inner[2 * 3];

	// 800b4fbc
	s16 hubArrowXY_Outter[2 * 4];

	// 800b4fcc
	s16 loadSave_pos[2 * 4];

	// 800b4fdc
	int loadSave_col[4]; // maybe should be `char*` instead of `int`

	// 800b4fec
	s16 hubArrow_pos[2 * 3];

	// 800B4FF8
	int hubArrow_col1[3]; // maybe should be `char*` instead of `int`

	// 800b5004
	int hubArrow_col2[3];

	// 800b5010
	int hubArrowGray1[3];
	int hubArrowGray2[3];

	// 800b5028
	// 8 bytes each
	struct
	{
		// can be -1 if not hub page
		s16 hubID;

		// can be -1 for hubs, which then
		// get name from MetaDataLev
		s16 titleLng;

		// enum AHPausePageType
		s16 type;

		s16 characterID_Boss;
	} advPausePages[AH_PAUSE_MENU_PAGE_COUNT];

	// 0x800B5060
	// 0,1,2,3,4: Gems
	// 5: Key (for all pages)
	// 6,7,8: 3-Relic page
	// 9,10,11,12,13: Tokens
	// 14: Trophy
	struct
	{
		// 0x0
		s16 modelID;
		s16 scale;

		// 0x4
		int color;

		// 0x8
		// same for all gems
		int instFlags;

		// 0xC
		// same for all gems
		SVec3 lightDir;
		s16 _pad_lightDir;


		// 0x14 bytes each
	} advPauseInst[AH_PAUSE_ICON_COUNT];

	// 0x800B518C
	struct RectMenu menuHintMenu;

	// 0x800B51B8
	s16 fiveArrow_pos[2 * 3];

	// 0x800b51c4
	int fiveArrow_col1[3];

	// 0x800b51d0
	int fiveArrow_col2[3];

	// 0x800b51dc
	SVec3 maskPos;
	s16 _pad_maskPos;

	// 0x800b51e4
	SVec3 maskRot;
	s16 _pad_maskRot;

	// 0x800b51ec
	s16 maskScale;

	// 0x800b51ee
	s16 maskCooldown;

	// 0x800b51f0
	SVec3 maskOffsetPos;
	s16 _pad_maskOffsetPos;

	// 0x800b51f8
	SVec3 maskOffsetRot;
	s16 _pad_maskOffsetRot;

	// 0x800b5200
	s16 maskVars[12];

	// 0x800b5218
	int maskFrameCurr;

	// 0x800b521c
	struct ParticleEmitter emSet_maskSpawn[0xA];

	// 0x800b5384
	struct ParticleEmitter emSet_maskLeave[0xA];

	// 0x800b54ec
	s16 maskAudioSettings[4];

	// 800b54f4
	// 20 hints, last two entries are null
	s16 hintMenu_lngIndexArr[22];

	// 800b5520
	SVec3 eyePos;
	s16 _pad_eyePos;

	// 800b5528
	SVec3 lookAtPos;
	s16 _pad_lookAtPos;

	// 800b5530
	int colorQuad[4]; // maybe should be `char*` instead of `int`

	// 800b5540
	int colorTri[3]; // maybe should be `char*` instead of `int`

	// 800b554c
	s16 pausePageDir;

	// 800b554e
	s16 pausePageTimer;

	// 800b5550
	s16 pausePagePrev;

	// 800b5552
	s16 pausePageCurr;

	// 800b5554
	s16 pausePageDir_dup;
	s16 padding3;

	// 800b5558
	s16 maskHintID;
	s16 padding_maskHintID;

	// 800b555c
	int maskAngle;

	// 800b5560
	SVec3 maskCamPosStart;
	s16 _pad_maskCamPosStart;

	// 800b5568
	SVec3 maskCamRotStart;
	s16 _pad_maskCamRotStart;

	// 800b5570
	s16 maskWarppadDelayFrames;
	s16 padding_maskWarppadDelayFrames;

	// 800b5574
	s16 maskWarppadBoolInterrupt;
	s16 padding_maskWarppadBoolInterrupt;

	// 800b5578
	struct PauseObject *ptrPauseObject;

	struct PauseObject
	{
		// 0x0
		struct
		{
			// 0x0
			s16 indexAdvPauseInst;
			s16 unlockFlag;

			// 0x4
			SVec3 rot;
			s16 _pad_rot;

			// 0xC
			struct Instance *inst;

			// 0x10 -- size
		} PauseMember[AH_PAUSE_MEMBER_COUNT];

		// 0xE0
		struct Thread *t;

		// 0xe4 -- size
	} pauseObject; // 800b557c

	// 800B5660
	int hintMenu_boolViewHint;

	// 800B5664
	int hintMenu_scrollIndex;

	// 800B5668
	char audioBackup[4];

	// 800B566c
	s16 maskSpawnFrame;
	s16 padding4;

	// 800b5670
	// Suppresses lower-priority hub-route arrows when an open boss/warppad arrow is drawn.
	s16 mapPriorityArrowDrawn;
	s16 padding_800b5672;
};

#define OFFSETOF_D232(ELEMENT) ((u32)0x800b4ddc + OFFSETOF(struct OverlayDATA_232, ELEMENT))

CTR_STATIC_ASSERT(OFFSETOF_D232(maskWarppadDelayFrames) == 0x800b5570);
CTR_STATIC_ASSERT(OFFSETOF_D232(maskWarppadBoolInterrupt) == 0x800b5574);
CTR_STATIC_ASSERT(OFFSETOF_D232(ptrPauseObject) == 0x800b5578);

extern struct OverlayRDATA_232 R232;
extern struct OverlayDATA_232 D232;

#endif
