#include <common.h>

static int PickupBots_IsBotWeaponReady(struct Driver *driver)
{
	return ((driver->actionsFlagSet & 0x100000) != 0) && ((driver->botData.botFlags & 2) == 0) && ((driver->actionsFlagSet & 0x2000000) == 0) &&
	       (driver->botData.weaponCooldown == 0) && (driver->instTntRecv == NULL) && (driver->clockReceive == 0);
}

static int PickupBots_IsCloseToPlayer(struct Driver *player, struct Driver *bot)
{
	int x = player->instSelf->matrix.t[0] - bot->instSelf->matrix.t[0];
	int z = player->instSelf->matrix.t[2] - bot->instSelf->matrix.t[2];

	return (u32)((x * x + z * z) - 0x90001) < 0x13affff;
}

static void PickupBots_SetCooldown(struct Driver *bot)
{
	bot->botData.weaponCooldown = (MixRNG_Scramble() & 0xff) + 0xf0;
}

static void PickupBots_PlayVoice(u32 voiceID, struct Driver *attacker, struct Driver *victim)
{
	Voiceline_RequestPlay(voiceID, data.characterIDs[attacker->driverID], data.characterIDs[victim->driverID]);
}

static void PickupBots_UpdateArcade(void)
{
	struct GameTracker *gGT = sdata->gGT;

	for (int i = 0; i < (u8)gGT->numPlyrCurrGame; i++)
	{
		struct Driver *player = gGT->drivers[i];

		if (player->driverRank != 0)
		{
			struct Driver *bot = gGT->driversInRaceOrder[player->driverRank - 1];

			if (PickupBots_IsBotWeaponReady(bot) && PickupBots_IsCloseToPlayer(player, bot))
			{
				int rng = MixRNG_Scramble() % 200;

				if (rng == 0)
				{
					int weaponID;

					if ((bot->lapIndex != 0) && (MixRNG_Scramble() % 100 < 0x32))
					{
						bot->numWumpas = 10;
					}

					if ((gGT->elapsedEventTime & 1) != 0)
					{
						bot->heldItemID = 3;

						if ((player->actionsFlagSet & 0x100000) == 0)
						{
							PickupBots_PlayVoice(0xf, bot, player);
						}

						weaponID = 3;
					}
					else
					{
						bot->heldItemID = 4;

						if ((player->actionsFlagSet & 0x100000) == 0)
						{
							PickupBots_PlayVoice(0xf, bot, player);
						}

						weaponID = 4;
					}

					VehPickupItem_ShootNow(bot, weaponID, 0);
					bot->numWumpas = 0;
					PickupBots_SetCooldown(bot);
				}
				else if (rng == 1)
				{
					bot->heldItemID = 1;

					if ((player->actionsFlagSet & 0x100000) == 0)
					{
						PickupBots_PlayVoice(10, bot, player);
					}

					VehPickupItem_ShootNow(bot, 2, 0);
					PickupBots_SetCooldown(bot);
				}
				else if (rng == 2)
				{
					bot->heldItemID = 2;

					if ((player->actionsFlagSet & 0x100000) == 0)
					{
						PickupBots_PlayVoice(11, bot, player);
					}

					VehPickupItem_ShootNow(bot, 2, 0);
					PickupBots_SetCooldown(bot);
				}

				bot->heldItemID = 0xf;
			}
		}

		if (player->driverRank < 3)
		{
			struct Driver *bot = gGT->driversInRaceOrder[player->driverRank + 1];

			if (PickupBots_IsBotWeaponReady(bot) && (((u32)player->lapIndex < (s8)gGT->numLaps) || (player->distanceToFinish_curr > 16000)) &&
			    PickupBots_IsCloseToPlayer(player, bot))
			{
				int rng = MixRNG_Scramble() % 800;
				int weaponID = 0xf;

				if ((rng < 2) && (bot->lapIndex != (u8)((s8)gGT->numLaps - 1)))
				{
					weaponID = 2;
				}
				else if (rng < 4)
				{
					weaponID = 1;
				}

				if (weaponID != 0xf)
				{
					bot->heldItemID = weaponID;

					if ((player->actionsFlagSet & 0x100000) == 0)
					{
						PickupBots_PlayVoice(11, bot, player);
					}

					VehPickupItem_ShootNow(bot, 2, 0);
					PickupBots_SetCooldown(bot);
				}

				bot->heldItemID = 0xf;
			}
		}
	}
}

static void PickupBots_SetBossCooldown(struct MetaDataBOSS *bossMeta)
{
	struct GameTracker *gGT = sdata->gGT;

	sdata->bossWeaponCooldown =
	    (RngDeadCoed((u32 *)&sdata->const_0x30215400) & 0x10) + bossMeta->weaponCooldown + 0xc + ((s8)sdata->advProgress.timesLostBossRace[gGT->bossID] * 4);
}

static struct MetaDataBOSS *PickupBots_GetInitialBossMeta(void)
{
	struct GameTracker *gGT = sdata->gGT;

	if (gGT->levelID == OXIDE_STATION)
	{
		return data.bossWeaponMetaPtr[0];
	}

	return data.bossWeaponMetaPtr[data.metaDataLEV[gGT->levelID].hubID];
}

static void PickupBots_AdvanceBossMeta(struct Driver *boss)
{
	struct GameTracker *gGT = sdata->gGT;
	struct MetaDataBOSS *bossMeta = sdata->bossWeaponMeta;
	struct MetaDataBOSS *nextMeta = &bossMeta[1];

	if (nextMeta->throwFlag == 0)
	{
		int threshold = gGT->level1->ptr_restart_points[bossMeta->trackCheckpoint].distToFinish << 3;

		if (threshold < (int)boss->distanceToFinish_curr)
		{
			int preservedThrow = -1;

			if (((bossMeta->weaponType == 0x66) || (bossMeta->weaponType == 0x64)) && (sdata->unk_8008d42C == 5))
			{
				preservedThrow = bossMeta->throwFlag;
			}

			bossMeta = PickupBots_GetInitialBossMeta();

			if (preservedThrow != -1)
			{
				bossMeta->throwFlag = preservedThrow;
			}
		}
	}
	else
	{
		int threshold = gGT->level1->ptr_restart_points[nextMeta->trackCheckpoint].distToFinish << 3;

		if ((int)boss->distanceToFinish_curr < threshold)
		{
			int preservedThrow = -1;

			if (((bossMeta->weaponType == 0x66) || (bossMeta->weaponType == 0x64)) && (sdata->unk_8008d42C == 5))
			{
				preservedThrow = bossMeta->throwFlag;
			}

			bossMeta = nextMeta;

			if (preservedThrow != -1)
			{
				bossMeta->throwFlag = preservedThrow;
			}
		}
	}

	sdata->bossWeaponMeta = bossMeta;
}

static void PickupBots_UpdateBossPathRequest(struct Driver *boss)
{
	if (sdata->bossWeaponMeta->unk1 != 0)
	{
		return;
	}

	if (sdata->unk_8008d428 == 0x1e)
	{
		if ((boss->botData.botFlags & 0x80) != 0)
		{
			return;
		}

		if (sdata->unk_8008d42a == 0)
		{
			if (boss->botData.botPath == 2)
			{
				boss->botData.desiredPath_BossOnly = 1;
				sdata->unk_8008d428 = 0;
				boss->botData.botFlags |= 0x40;
			}
			else if (boss->botData.botPath == 1)
			{
				boss->botData.desiredPath_BossOnly = 0;
				sdata->unk_8008d428 = 0;
				sdata->unk_8008d42a = boss->botData.botPath;
				boss->botData.botFlags |= 0x40;
			}
		}
		else
		{
			if (boss->botData.botPath == 0)
			{
				boss->botData.desiredPath_BossOnly = 1;
				sdata->unk_8008d428 = 0;
				boss->botData.botFlags |= 0x40;
			}
			else if (boss->botData.botPath == 1)
			{
				boss->botData.desiredPath_BossOnly = 2;
				sdata->unk_8008d428 = 0;
				sdata->unk_8008d42a = 0;
				boss->botData.botFlags |= 0x40;
			}
		}
	}
	else if ((boss->botData.botFlags & 0x40) == 0)
	{
		sdata->unk_8008d428++;
	}
}

static int PickupBots_GetBossWeaponID(struct MetaDataBOSS *bossMeta)
{
	int weaponID = bossMeta->weaponType;

	if (weaponID == 0x64)
	{
		weaponID = 3;
	}
	else if (weaponID == 0x65)
	{
		weaponID = 1;
	}
	else if (weaponID == 0x66)
	{
		weaponID = 4;
	}
	else if (weaponID == 0xf)
	{
		weaponID = -1;
	}

	return weaponID;
}

static int PickupBots_UpdateBossJuice(struct MetaDataBOSS *bossMeta, int weaponID)
{
	u16 juiceFlag = bossMeta->juiceFlag;

	if ((juiceFlag & 2) == 0)
	{
		sdata->unk_8008d42C = 0;
		return weaponID;
	}

	if (sdata->unk_8008d42C < 5)
	{
		sdata->unk_8008d42C++;
		return weaponID;
	}

	if (bossMeta->weaponType == 0x64)
	{
		weaponID = 3;

		if (bossMeta->throwFlag != 3)
		{
			bossMeta->throwFlag = 3;
			sdata->unk_8008d42C = 5;
			bossMeta->juiceFlag = juiceFlag | 1;
			return weaponID;
		}
	}
	else if (bossMeta->weaponType == 0x65)
	{
		weaponID = 1;

		if ((juiceFlag & 1) == 0)
		{
			bossMeta->juiceFlag = juiceFlag | 1;
			sdata->unk_8008d42C = 5;
			return 3;
		}

		bossMeta->juiceFlag = juiceFlag & ~1;
		sdata->unk_8008d42C = 0;
		return weaponID;
	}
	else if (bossMeta->weaponType == 0x66)
	{
		weaponID = 4;

		if (bossMeta->throwFlag != 3)
		{
			bossMeta->throwFlag = 3;
			sdata->unk_8008d42C = 5;
			bossMeta->juiceFlag |= 1;
			return weaponID;
		}
	}
	else
	{
		return weaponID;
	}

	bossMeta->throwFlag = 2;
	sdata->unk_8008d42C = 0;
	bossMeta->juiceFlag &= ~1;
	return weaponID;
}

static void PickupBots_UpdateBoss(void)
{
	struct GameTracker *gGT = sdata->gGT;
	struct Driver *boss = gGT->drivers[1];
	struct Driver *player = gGT->drivers[0];
	struct MetaDataBOSS *bossMeta = sdata->bossWeaponMeta;

	if (((boss->botData.botFlags & 2) != 0) || ((boss->actionsFlagSet & 0x2000000) != 0) || (boss->instTntRecv != NULL) || (boss->clockReceive != 0) ||
	    (boss->botData.unk5bc.ai_speedLinear < 0x1f41))
	{
		PickupBots_SetBossCooldown(bossMeta);
		return;
	}

	PickupBots_AdvanceBossMeta(boss);
	bossMeta = sdata->bossWeaponMeta;

	PickupBots_UpdateBossPathRequest(boss);

	if (sdata->bossWeaponCooldown > 0)
	{
		sdata->bossWeaponCooldown--;
		return;
	}

	PickupBots_SetBossCooldown(bossMeta);

	int weaponID = PickupBots_UpdateBossJuice(bossMeta, PickupBots_GetBossWeaponID(bossMeta));
	int throwFlag = bossMeta->throwFlag;
	int weaponFlags = (throwFlag == 2);

	if (weaponID >= 0)
	{
		u8 oldWumpa = boss->numWumpas;
		boss->numWumpas = ((bossMeta->juiceFlag & 1) != 0) ? 10 : 0;
		boss->heldItemID = weaponID;

		if ((u16)(weaponID - 3) < 2)
		{
			PickupBots_PlayVoice(0xf, boss, player);
		}
		else
		{
			weaponFlags |= 2;
			PickupBots_PlayVoice(10, boss, player);
		}

		if (boss->heldItemID == 1)
		{
			VehPickupItem_ShootNow(boss, 2, (s16)weaponFlags);
		}
		else if ((boss->heldItemID == 4) && (weaponFlags == 1) && (gGT->levelID == OXIDE_STATION))
		{
			VehPickupItem_ShootNow(boss, weaponID, 1);
			VehPickupItem_ShootNow(boss, weaponID, 1);
		}
		else
		{
			VehPickupItem_ShootNow(boss, weaponID, (s16)weaponFlags);

			if ((boss->heldItemID == 3) && (bossMeta->throwFlag == 3) && (sdata->unk_8008d42C != 5))
			{
				sdata->unk_8008d42C = 5;
			}
		}

		boss->heldItemID = 0xf;
		boss->numWumpas = oldWumpa;
	}
}

void PickupBots_Update(void)
{
	// NOTE(aalhendi): ASM-verified NTSC-U 926 0x800408b8-0x800414f4.
	struct GameTracker *gGT = sdata->gGT;

	if (((u8)gGT->numBotsNextGame == 0) || (gGT->elapsedEventTime < 0x4b00))
	{
		if (gGT->gameMode1 >= 0)
		{
			return;
		}

		if (gGT->elapsedEventTime < 0x12c0)
		{
			return;
		}
	}

	if ((gGT->gameMode1 & (ADVENTURE_BOSS | END_OF_RACE)) != ADVENTURE_BOSS)
	{
		if ((u8)gGT->numPlyrCurrGame == 0)
		{
			return;
		}

		PickupBots_UpdateArcade();
		return;
	}

	PickupBots_UpdateBoss();
}
