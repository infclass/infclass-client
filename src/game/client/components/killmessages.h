/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_CLIENT_COMPONENTS_KILLMESSAGES_H
#define GAME_CLIENT_COMPONENTS_KILLMESSAGES_H
#include <game/client/component.h>

#include <game/client/render.h>
class CKillMessages : public CComponent
{
	int m_SpriteQuadContainerIndex;
	int m_InfWeaponOffset;
	enum
	{
		MAX_KILLMSGS = 5,
		MAX_KILLMSG_TEAM_MEMBERS = 4,
	};

public:
	// kill messages
	struct CKillMsg
	{
		int m_InfDamageType = -1;
		int m_Weapon;

		int m_aVictimIds[MAX_KILLMSG_TEAM_MEMBERS];
		int m_VictimDDTeam;
		char m_aVictimName[64];
		STextContainerIndex m_VictimTextContainerIndex;
		float m_VictimTextWidth;
		CTeeRenderInfo m_aVictimRenderInfo[MAX_KILLMSG_TEAM_MEMBERS];
		int m_KillerID;
		char m_aKillerName[64];
		STextContainerIndex m_KillerTextContainerIndex;
		float m_KillerTextWidth;
		CTeeRenderInfo m_KillerRenderInfo;

		int m_AssistantID = -1;
		char m_aAssistantName[64];
		STextContainerIndex m_AssistantTextContainerIndex;
		float m_AssistantTextWidth;
		CTeeRenderInfo m_AssistantRenderInfo;

		STextContainerIndex m_AssistantPlusContainerIndex;
		float m_AssistantPlusWidth;

		int m_ModeSpecial; // for CTF, if the guy is carrying a flag for example
		int m_Tick;
		int m_FlagCarrierBlue;
		int m_TeamSize;
	};

private:
	void CreateKillmessageNamesIfNotCreated(CKillMsg &Kill);

public:
	CKillMsg m_aKillmsgs[MAX_KILLMSGS];
	int m_KillmsgCurrent;

	virtual int Sizeof() const override { return sizeof(*this); }
	virtual void OnWindowResize() override;
	virtual void OnReset() override;
	virtual void OnRender() override;
	virtual void OnMessage(int MsgType, void *pRawMsg) override;
	virtual void OnInit() override;

	void RefindSkins();
};

#endif
