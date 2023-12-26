#include "compression.h"
#include "snapshot.h"
#include "uuid_manager.h"

#include <climits>
#include <cstdlib>

#include <base/math.h>
#include <base/system.h>

#include <engine/shared/protocolglue.h>
#include <engine/shared/translation_context.h>

#include <game/generated/protocol.h>
#include <game/generated/protocol7.h>
#include <game/generated/protocolglue.h>

#include <game/gamecore.h>

int CSnapshot::TranslateSevenToSix(CSnapshot *pSixSnapDest, CTranslationContext &TranslationContext, float LocalTime) const
{
	CSnapshotBuilder Builder;
	Builder.Init();

	// hack to put game info in the snap
	// even though in 0.7 we get it as a game message
	// this message has to be on the top
	// the client looks at the items in order and needs the gameinfo at the top
	// otherwise it will not render skins with team colors
	if(TranslationContext.m_ShouldSendGameInfo)
	{
		void *pObj = Builder.NewItem(NETOBJTYPE_GAMEINFO, 0, sizeof(CNetObj_GameInfo));
		if(!pObj)
			return -1;

		int GameStateFlagsSix = 0;
		if(TranslationContext.m_GameStateFlags7 & protocol7::GAMESTATEFLAG_GAMEOVER)
			GameStateFlagsSix |= GAMESTATEFLAG_GAMEOVER;
		if(TranslationContext.m_GameStateFlags7 & protocol7::GAMESTATEFLAG_SUDDENDEATH)
			GameStateFlagsSix |= GAMESTATEFLAG_SUDDENDEATH;
		if(TranslationContext.m_GameStateFlags7 & protocol7::GAMESTATEFLAG_PAUSED)
			GameStateFlagsSix |= GAMESTATEFLAG_PAUSED;

		/*
			These are 0.7 only flags that we just ignore for now

			GAMESTATEFLAG_WARMUP
			GAMESTATEFLAG_ROUNDOVER
			GAMESTATEFLAG_STARTCOUNTDOWN
		*/

		CNetObj_GameInfo Info6 = {};
		Info6.m_GameFlags = TranslationContext.m_GameFlags;
		Info6.m_GameStateFlags = GameStateFlagsSix;
		Info6.m_ScoreLimit = TranslationContext.m_ScoreLimit;
		Info6.m_TimeLimit = TranslationContext.m_TimeLimit;
		Info6.m_RoundNum = TranslationContext.m_MatchNum;
		Info6.m_RoundCurrent = TranslationContext.m_MatchCurrent;
		mem_copy(pObj, &Info6, sizeof(CNetObj_GameInfo));
	}

	for(int i = 0; i < MAX_CLIENTS; i++)
	{
		const CTranslationContext::CClientData &Client = TranslationContext.m_aClients[i];
		if(!Client.m_Active)
			continue;

		void *pObj = Builder.NewItem(NETOBJTYPE_CLIENTINFO, i, sizeof(CNetObj_ClientInfo));
		if(!pObj)
			return -2;

		CNetObj_ClientInfo Info6 = {};
		StrToInts(&Info6.m_Name0, 4, Client.m_aName);
		StrToInts(&Info6.m_Clan0, 3, Client.m_aClan);
		Info6.m_Country = Client.m_Country;
		StrToInts(&Info6.m_Skin0, 6, Client.m_aSkinName);
		Info6.m_UseCustomColor = Client.m_UseCustomColor;
		Info6.m_ColorBody = Client.m_ColorBody;
		Info6.m_ColorFeet = Client.m_ColorFeet;
		mem_copy(pObj, &Info6, sizeof(CNetObj_ClientInfo));
	}

	bool NewGameData = false;

	for(int i = 0; i < m_NumItems; i++)
	{
		const CSnapshotItem *pItem7 = GetItem(i);
		int Size = GetItemSize(i);
		// the first few items are a full match
		// no translation needed
		if(pItem7->Type() == protocol7::NETOBJTYPE_PROJECTILE ||
			pItem7->Type() == protocol7::NETOBJTYPE_LASER ||
			pItem7->Type() == protocol7::NETOBJTYPE_FLAG)
		{
			void *pObj = Builder.NewItem(pItem7->Type(), pItem7->ID(), Size);
			if(!pObj)
				return -4;

			mem_copy(pObj, pItem7->Data(), Size);
		}
		else if(pItem7->Type() == protocol7::NETOBJTYPE_PICKUP)
		{
			void *pObj = Builder.NewItem(NETOBJTYPE_PICKUP, pItem7->ID(), sizeof(CNetObj_Pickup));
			if(!pObj)
				return -5;

			const protocol7::CNetObj_Pickup *pPickup7 = (const protocol7::CNetObj_Pickup *)pItem7->Data();
			CNetObj_Pickup Pickup6 = {};
			Pickup6.m_X = pPickup7->m_X;
			Pickup6.m_Y = pPickup7->m_Y;
			PickupType_SevenToSix(pPickup7->m_Type, Pickup6.m_Type, Pickup6.m_Subtype);

			mem_copy(pObj, &Pickup6, sizeof(CNetObj_Pickup));
		}
		else if(pItem7->Type() == protocol7::NETOBJTYPE_GAMEDATA)
		{
			const protocol7::CNetObj_GameData *pGameData = (const protocol7::CNetObj_GameData *)pItem7->Data();
			TranslationContext.m_GameStateFlags7 = pGameData->m_GameStateFlags;
			// TODO: this 0.7 item also includes m_GameStartTick
			// TODO: this 0.7 item also includes m_GameStateEndTick
		}
		else if(pItem7->Type() == protocol7::NETOBJTYPE_GAMEDATATEAM)
		{
			// 0.7 added GameDataTeam and GameDataFlag
			// both items merged together have all fields of the 0.6 GameData
			// so if we get either one of them we store the details in the translation context
			// and build one GameData snap item after this loop
			const protocol7::CNetObj_GameDataTeam *pTeam7 = (const protocol7::CNetObj_GameDataTeam *)pItem7->Data();

			TranslationContext.m_TeamscoreRed = pTeam7->m_TeamscoreRed;
			TranslationContext.m_TeamscoreBlue = pTeam7->m_TeamscoreBlue;
			NewGameData = true;
		}
		else if(pItem7->Type() == protocol7::NETOBJTYPE_GAMEDATAFLAG)
		{
			const protocol7::CNetObj_GameDataFlag *pFlag7 = (const protocol7::CNetObj_GameDataFlag *)pItem7->Data();

			TranslationContext.m_FlagCarrierRed = pFlag7->m_FlagCarrierRed;
			TranslationContext.m_FlagCarrierBlue = pFlag7->m_FlagCarrierBlue;
			// pFlag7->m_FlagDropTickRed; // TODO: use this
			// pFlag7->m_FlagDropTickBlue; // TODO: use this
			NewGameData = true;
		}
		else if(pItem7->Type() == protocol7::NETOBJTYPE_CHARACTER)
		{
			void *pObj = Builder.NewItem(NETOBJTYPE_CHARACTER, pItem7->ID(), sizeof(CNetObj_Character));
			if(!pObj)
				return -6;

			const protocol7::CNetObj_Character *pChar7 = (const protocol7::CNetObj_Character *)pItem7->Data();

			CNetObj_Character Char6 = {};
			// character core is unchanged
			mem_copy(&Char6, pItem7->Data(), sizeof(CNetObj_CharacterCore));

			Char6.m_PlayerFlags = 0;
			if(pItem7->ID() >= 0 && pItem7->ID() < MAX_CLIENTS)
				Char6.m_PlayerFlags = PlayerFlags_SevenToSix(TranslationContext.m_aClients[pItem7->ID()].m_PlayerFlags7);
			Char6.m_Health = pChar7->m_Health;
			Char6.m_Armor = pChar7->m_Armor;
			Char6.m_AmmoCount = pChar7->m_AmmoCount;
			Char6.m_Weapon = pChar7->m_Weapon;
			Char6.m_Emote = pChar7->m_Emote;
			Char6.m_AttackTick = pChar7->m_AttackTick;

			// got 0.7
			// int m_Health;
			// int m_Armor;
			// int m_AmmoCount;
			// int m_Weapon;
			// int m_Emote;
			// int m_AttackTick;
			// int m_TriggeredEvents; // TODO: do we need events what do they do?

			// want 0.6
			// int m_PlayerFlags;
			// int m_Health;
			// int m_Armor;
			// int m_AmmoCount;
			// int m_Weapon;
			// int m_Emote;
			// int m_AttackTick;

			mem_copy(pObj, &Char6, sizeof(CNetObj_Character));
		}
		else if(pItem7->Type() == protocol7::NETOBJTYPE_PLAYERINFO)
		{
			void *pObj = Builder.NewItem(NETOBJTYPE_PLAYERINFO, pItem7->ID(), sizeof(CNetObj_PlayerInfo));
			if(!pObj)
				return -7;

			// got 0.7
			// int m_PlayerFlags;
			// int m_Score;
			// int m_Latency;

			// want 0.6
			// int m_Local;
			// int m_ClientID;
			// int m_Team;
			// int m_Score;
			// int m_Latency;

			const protocol7::CNetObj_PlayerInfo *pInfo7 = (const protocol7::CNetObj_PlayerInfo *)pItem7->Data();
			CNetObj_PlayerInfo Info6 = {};
			Info6.m_Local = TranslationContext.m_LocalClientID == pItem7->ID(); // TODO: is this too hacky?
			Info6.m_ClientID = pItem7->ID(); // TODO: not sure if this actually works
			Info6.m_Team = 0;
			if(pItem7->ID() >= 0 && pItem7->ID() < MAX_CLIENTS)
			{
				Info6.m_Team = TranslationContext.m_aClients[pItem7->ID()].m_Team;
				TranslationContext.m_aClients[pItem7->ID()].m_PlayerFlags7 = pInfo7->m_PlayerFlags;
			}
			Info6.m_Score = pInfo7->m_Score;
			Info6.m_Latency = pInfo7->m_Latency;
			mem_copy(pObj, &Info6, sizeof(CNetObj_PlayerInfo));
		}
		else if(pItem7->Type() == protocol7::NETOBJTYPE_SPECTATORINFO)
		{
			void *pObj = Builder.NewItem(NETOBJTYPE_SPECTATORINFO, pItem7->ID(), sizeof(CNetObj_SpectatorInfo));
			if(!pObj)
				return -8;

			const protocol7::CNetObj_SpectatorInfo *pSpec7 = (const protocol7::CNetObj_SpectatorInfo *)pItem7->Data();
			CNetObj_SpectatorInfo Spec6 = {};
			Spec6.m_SpectatorID = pSpec7->m_SpectatorID;
			if(pSpec7->m_SpecMode == protocol7::SPEC_FREEVIEW)
				Spec6.m_SpectatorID = SPEC_FREEVIEW;
			Spec6.m_X = pSpec7->m_X;
			Spec6.m_Y = pSpec7->m_Y;
			mem_copy(pObj, &Spec6, sizeof(CNetObj_SpectatorInfo));
		}
		else if(pItem7->Type() == protocol7::NETEVENTTYPE_EXPLOSION)
		{
			void *pEvent = Builder.NewItem(NETEVENTTYPE_EXPLOSION, pItem7->ID(), sizeof(CNetEvent_Explosion));
			if(!pEvent)
				return -9;

			const protocol7::CNetEvent_Explosion *pExplosion7 = (const protocol7::CNetEvent_Explosion *)pItem7->Data();
			CNetEvent_Explosion Explosion6 = {};
			Explosion6.m_X = pExplosion7->m_X;
			Explosion6.m_Y = pExplosion7->m_Y;
			mem_copy(pEvent, &Explosion6, sizeof(CNetEvent_Explosion));
		}
		else if(pItem7->Type() == protocol7::NETEVENTTYPE_SPAWN)
		{
			void *pEvent = Builder.NewItem(NETEVENTTYPE_SPAWN, pItem7->ID(), sizeof(CNetEvent_Spawn));
			if(!pEvent)
				return -10;

			const protocol7::CNetEvent_Spawn *pSpawn7 = (const protocol7::CNetEvent_Spawn *)pItem7->Data();
			CNetEvent_Spawn Spawn6 = {};
			Spawn6.m_X = pSpawn7->m_X;
			Spawn6.m_Y = pSpawn7->m_Y;
			mem_copy(pEvent, &Spawn6, sizeof(CNetEvent_Spawn));
		}
		else if(pItem7->Type() == protocol7::NETEVENTTYPE_HAMMERHIT)
		{
			void *pEvent = Builder.NewItem(NETEVENTTYPE_HAMMERHIT, pItem7->ID(), sizeof(CNetEvent_HammerHit));
			if(!pEvent)
				return -11;

			const protocol7::CNetEvent_HammerHit *pHammerHit7 = (const protocol7::CNetEvent_HammerHit *)pItem7->Data();
			CNetEvent_HammerHit HammerHit6 = {};
			HammerHit6.m_X = pHammerHit7->m_X;
			HammerHit6.m_Y = pHammerHit7->m_Y;
			mem_copy(pEvent, &HammerHit6, sizeof(CNetEvent_HammerHit));
		}
		else if(pItem7->Type() == protocol7::NETEVENTTYPE_DEATH)
		{
			void *pEvent = Builder.NewItem(NETEVENTTYPE_DEATH, pItem7->ID(), sizeof(CNetEvent_Death));
			if(!pEvent)
				return -12;

			const protocol7::CNetEvent_Death *pDeath7 = (const protocol7::CNetEvent_Death *)pItem7->Data();
			CNetEvent_Death Death6 = {};
			Death6.m_X = pDeath7->m_X;
			Death6.m_Y = pDeath7->m_Y;
			Death6.m_ClientID = pDeath7->m_ClientID;
			mem_copy(pEvent, &Death6, sizeof(CNetEvent_Death));
		}
		else if(pItem7->Type() == protocol7::NETEVENTTYPE_SOUNDWORLD)
		{
			void *pEvent = Builder.NewItem(NETEVENTTYPE_SOUNDWORLD, pItem7->ID(), sizeof(CNetEvent_SoundWorld));
			if(!pEvent)
				return -13;

			const protocol7::CNetEvent_SoundWorld *pSoundWorld7 = (const protocol7::CNetEvent_SoundWorld *)pItem7->Data();
			CNetEvent_SoundWorld SoundWorld6 = {};
			SoundWorld6.m_X = pSoundWorld7->m_X;
			SoundWorld6.m_Y = pSoundWorld7->m_Y;
			SoundWorld6.m_SoundID = pSoundWorld7->m_SoundID;
			mem_copy(pEvent, &SoundWorld6, sizeof(CNetEvent_SoundWorld));
		}
		else if(pItem7->Type() == protocol7::NETEVENTTYPE_DAMAGE)
		{
			// 0.7 introduced amount for damage indicators
			// so for one 0.7 item we might create multiple 0.6 ones
			const protocol7::CNetEvent_Damage *pDmg7 = (const protocol7::CNetEvent_Damage *)pItem7->Data();

			int Amount = pDmg7->m_HealthAmount + pDmg7->m_ArmorAmount;
			if(Amount < 1)
				continue;

			int ClientID = pDmg7->m_ClientID;
			if(ClientID < 0 || ClientID >= MAX_CLIENTS)
			{
				// TODO: test this case
				//       seems like the vanilla server only ever sets a valid ClientID
				//       this branch is only used by modded 0.7 servers that set a invalid ClientID
				void *pEvent = Builder.NewItem(NETEVENTTYPE_DAMAGEIND, pItem7->ID(), sizeof(CNetEvent_DamageInd));
				if(!pEvent)
					return -14;

				CNetEvent_DamageInd Dmg6 = {};
				Dmg6.m_X = pDmg7->m_X;
				Dmg6.m_Y = pDmg7->m_Y;
				Dmg6.m_Angle = pDmg7->m_Angle;
				mem_copy(pEvent, &Dmg6, sizeof(CNetEvent_DamageInd));
				continue;
			}

			TranslationContext.m_aDamageTaken[ClientID]++;

			float Angle;
			// create healthmod indicator
			if(LocalTime < TranslationContext.m_aDamageTakenTick[ClientID] + 0.5f)
			{
				// make sure that the damage indicators don't group together
				Angle = TranslationContext.m_aDamageTaken[ClientID] * 0.25f;
			}
			else
			{
				TranslationContext.m_aDamageTaken[ClientID] = 0;
				Angle = 0;
			}

			TranslationContext.m_aDamageTakenTick[ClientID] = LocalTime;

			float a = 3 * pi / 2 + Angle;
			float s = a - pi / 3;
			float e = a + pi / 3;
			for(int k = 0; k < Amount; k++)
			{
				// pItem7->ID() is reused that is technically wrong
				// but the client implementation does not look at the ids
				// and renders the damage indicators just fine
				void *pEvent = Builder.NewItem(NETEVENTTYPE_DAMAGEIND, pItem7->ID(), sizeof(CNetEvent_DamageInd));
				if(!pEvent)
					return -15;

				CNetEvent_DamageInd Dmg6 = {};
				Dmg6.m_X = pDmg7->m_X;
				Dmg6.m_Y = pDmg7->m_Y;
				float f = mix(s, e, float(k + 1) / float(Amount + 2));
				Dmg6.m_Angle = (int)(f * 256.0f);
				mem_copy(pEvent, &Dmg6, sizeof(CNetEvent_DamageInd));
			}
		}
	}

	if(NewGameData)
	{
		void *pObj = Builder.NewItem(NETOBJTYPE_GAMEDATA, 0, sizeof(CNetObj_GameData));
		if(!pObj)
			return -16;

		CNetObj_GameData GameData = {};
		GameData.m_TeamscoreRed = TranslationContext.m_TeamscoreRed;
		GameData.m_TeamscoreBlue = TranslationContext.m_TeamscoreBlue;
		GameData.m_FlagCarrierRed = TranslationContext.m_FlagCarrierRed;
		GameData.m_FlagCarrierBlue = TranslationContext.m_FlagCarrierBlue;
		mem_copy(pObj, &GameData, sizeof(CNetObj_GameData));
	}

	return Builder.Finish(pSixSnapDest);
}
