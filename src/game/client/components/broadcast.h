/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */
#ifndef GAME_CLIENT_COMPONENTS_BROADCAST_H
#define GAME_CLIENT_COMPONENTS_BROADCAST_H

#include <engine/textrender.h>

#include <game/client/component.h>

class CBroadcast : public CComponent
{
	// broadcasts
	char m_aClientBroadcastText7[1024];
	char m_aBroadcastText[1024];
	int m_BroadcastTick;
	float m_BroadcastRenderOffset;
	float m_ClientBroadcastTime7;
	STextContainerIndex m_TextContainerIndex;
	STextContainerIndex m_ClientTextContainerIndex;

	void RenderClientBroadcast7();
	void RenderServerBroadcast();
	void OnBroadcastMessage(const CNetMsg_Sv_Broadcast *pMsg);

public:
	virtual int Sizeof() const override { return sizeof(*this); }
	virtual void OnReset() override;
	virtual void OnWindowResize() override;
	virtual void OnRender() override;
	virtual void OnMessage(int MsgType, void *pRawMsg) override;

	void DoClientBroadcast7(const char *pText);
};

#endif
