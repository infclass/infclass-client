#ifndef ENGINE_SHARED_TRANSLATION_CONTEXT_H
#define ENGINE_SHARED_TRANSLATION_CONTEXT_H

#include <engine/shared/protocol.h>

class CTranslationContext
{
public:
	CTranslationContext()
	{
		Reset();
	}

	void Reset()
	{
		m_LocalClientID = -1;
		m_ShouldSendGameInfo = false;
		m_GameFlags = 0;
		m_ScoreLimit = 0;
		m_TimeLimit = 0;
		m_MatchNum = 0;
		m_MatchCurrent = 0;
		mem_zero(m_aDamageTaken, sizeof(m_aDamageTaken));
		mem_zero(m_aDamageTakenTick, sizeof(m_aDamageTakenTick));
		m_FlagCarrierBlue = 0;
		m_FlagCarrierRed = 0;
		m_TeamscoreRed = 0;
		m_TeamscoreBlue = 0;
	}

	// TODO: this struct is not really used
	//       it should be integreated into the menu to
	//       show which features are active
	//
	//       but this is not done to keep 0.7 code out of
	//       the 0.6 code base
	struct CServerSettings
	{
		bool m_KickVote;
		int m_KickMin;
		bool m_SpecVote;
		bool m_TeamLock;
		bool m_TeamBalance;
		int m_PlayerSlots;
	} m_ServerSettings;

	struct CClientData
	{
		CClientData()
		{
			Reset();
		}

		void Reset()
		{
			m_Active = false;

			m_UseCustomColor = 0;
			m_ColorBody = 0;
			m_ColorFeet = 0;

			str_copy(m_aName, "name", sizeof(m_aName));
			str_copy(m_aClan, "clan", sizeof(m_aClan));
			m_Country = 0;
			str_copy(m_aSkinName, "default", sizeof(m_aSkinName));
			m_SkinColor = 0;
			m_Team = 0;
			m_PlayerFlags7 = 0;
		}

		bool m_Active;

		int m_UseCustomColor;
		int m_ColorBody;
		int m_ColorFeet;

		char m_aName[MAX_NAME_LENGTH];
		char m_aClan[MAX_CLAN_LENGTH];
		int m_Country;
		char m_aSkinName[64];
		int m_SkinColor;
		int m_Team;
		int m_PlayerFlags7;
	};

	CClientData m_aClients[MAX_CLIENTS];
	int m_aDamageTaken[MAX_CLIENTS];
	float m_aDamageTakenTick[MAX_CLIENTS];

	int m_LocalClientID;

	bool m_ShouldSendGameInfo;
	int m_GameStateFlags7;
	int m_GameFlags;
	int m_ScoreLimit;
	int m_TimeLimit;
	int m_MatchNum;
	int m_MatchCurrent;

	int m_MapdownloadTotalsize;
	int m_MapDownloadChunkSize;
	int m_MapDownloadChunksPerRequest;

	int m_FlagCarrierBlue;
	int m_FlagCarrierRed;
	int m_TeamscoreRed;
	int m_TeamscoreBlue;
};

#endif
