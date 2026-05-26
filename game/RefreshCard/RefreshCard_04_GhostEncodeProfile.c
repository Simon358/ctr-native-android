#include <common.h>

static int RefreshCard_GhostProfileNameExists(char *profileName)
{
	int i;

	for (i = 0; i < sdata->numGhostProfilesSaved; i++)
	{
		if (strcmp(sdata->ghostProfile_memcard[i].profile_name, profileName) == 0)
			return 1;
	}

	return 0;
}

// NOTE(aalhendi): ASM-verified NTSC-U 926 0x80046c30-0x80047034.
void RefreshCard_GhostEncodeProfile(u32 slotIndex, u16 characterID, u16 levelID, int time, char *name)
{
	char description[0x80];
	char scrambled[0x80] = {0};
	s32 characterID32 = (s16)characterID;
	u32 packed;
	int isUnique;

	do
	{
		isUnique = 1;

		if (time > 0x8c9ff)
			time = 0x8c9ff;

		packed = (u32)characterID32 | ((u32)((s32)(s16)levelID << 4)) | ((u32)time << 9) | (slotIndex << 0x1d);
		data.s_BASCUS_94426G_Question[13] = RefreshCard_GhostEncodeByte(packed & 0x3f);
		data.s_BASCUS_94426G_Question[14] = RefreshCard_GhostEncodeByte((packed >> 6) & 0x3f);
		data.s_BASCUS_94426G_Question[15] = RefreshCard_GhostEncodeByte((packed >> 0xc) & 0x3f);
		data.s_BASCUS_94426G_Question[16] = RefreshCard_GhostEncodeByte((packed >> 0x12) & 0x3f);
		data.s_BASCUS_94426G_Question[17] = RefreshCard_GhostEncodeByte((packed >> 0x18) & 0x3f);
		data.s_BASCUS_94426G_Question[18] = RefreshCard_GhostEncodeByte(packed >> 0x1e);
		data.s_BASCUS_94426G_Question[19] = '\0';

		if (RefreshCard_GhostProfileNameExists(data.s_BASCUS_94426G_Question) != 0)
			isUnique = 0;

		slotIndex = (slotIndex + 1) & 7;
	} while (isUnique == 0);

	description[0] = '\0';

	strcat(&description[strlen(description)], sdata->lngStrings[data.metaDataLEV[(s16)levelID].name_LNG]);
	strcat(description, sdata->strcatData1_colon);
	strcat(&description[strlen(description)], sdata->lngStrings[data.MetaDataCharacters[(s16)characterID].name_LNG_long]);
	strcat(description, sdata->strcatData1_colon);
	strcat(description, (char *)RECTMENU_DrawTime(time));

	CTR_ScrambleGhostString(scrambled, description);
	memcpy(sdata->memcardIcon_HeaderGHOST, scrambled, 0x3e);

	struct GhostProfile *profile = &sdata->ghostProfile_current;
	memcpy(profile->profile_name, data.s_BASCUS_94426G_Question, sizeof(data.s_BASCUS_94426G_Question));
	profile->profile_name[sizeof(data.s_BASCUS_94426G_Question)] = data.s_BASCUS_94426G_Star[0];

	memcpy(profile->SubmitName_name, name, sizeof(profile->SubmitName_name));

	*(u8 *)&profile->alwaysOne = 1;
	profile->trackID = levelID;
	profile->characterID = characterID;
	*(s16 *)&profile->memcardProfileIndex = slotIndex;
	profile->trackTime = time;
}
