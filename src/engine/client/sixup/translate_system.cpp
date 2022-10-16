#include <engine/textrender.h>

#include "../client.h"

int CClient::TranslateSysMsg(int *pMsgID, bool System, CUnpacker *pUnpacker, CPacker &Packer, CNetChunk *pPacket)
{
	if(!System)
		return -1;

	Packer.Reset();

	if(*pMsgID == protocol7::NETMSG_MAP_CHANGE)
	{
		*pMsgID = NETMSG_MAP_CHANGE;
		const char *pMapName = pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES);
		int MapCrc = pUnpacker->GetInt();
		int Size = pUnpacker->GetInt();
		m_TranslationContext.m_MapDownloadChunksPerRequest = pUnpacker->GetInt();
		int ChunkSize = pUnpacker->GetInt();
		// void *pSha256 = pUnpacker->GetRaw(); // probably safe to ignore
		Packer.AddString(pMapName, 0);
		Packer.AddInt(MapCrc);
		Packer.AddInt(Size);
		m_TranslationContext.m_MapdownloadTotalsize = Size;
		m_TranslationContext.m_MapDownloadChunkSize = ChunkSize;
		return 0;
	}
	else if(*pMsgID == protocol7::NETMSG_SERVERINFO)
	{
		net_addr_str(&pPacket->m_Address, m_CurrentServerInfo.m_aAddress, sizeof(m_CurrentServerInfo.m_aAddress), true);
		str_copy(m_CurrentServerInfo.m_aVersion, pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES), sizeof(m_CurrentServerInfo.m_aVersion));
		str_copy(m_CurrentServerInfo.m_aName, pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES), sizeof(m_CurrentServerInfo.m_aName));
		str_clean_whitespaces(m_CurrentServerInfo.m_aName);
		pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES); // Hostname
		str_copy(m_CurrentServerInfo.m_aMap, pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES), sizeof(m_CurrentServerInfo.m_aMap));
		str_copy(m_CurrentServerInfo.m_aGameType, pUnpacker->GetString(CUnpacker::SANITIZE_CC | CUnpacker::SKIP_START_WHITESPACES), sizeof(m_CurrentServerInfo.m_aGameType));
		int Flags = pUnpacker->GetInt();
		if(Flags & SERVER_FLAG_PASSWORD)
			m_CurrentServerInfo.m_Flags |= SERVER_FLAG_PASSWORD;
		// ddnets http master server handles timescore for us already
		// if(Flags&SERVER_FLAG_TIMESCORE)
		// 	m_CurrentServerInfo.m_Flags |= SERVER_FLAG_TIMESCORE;
		pUnpacker->GetInt(); // Server level
		m_CurrentServerInfo.m_NumPlayers = pUnpacker->GetInt();
		m_CurrentServerInfo.m_MaxPlayers = pUnpacker->GetInt();
		m_CurrentServerInfo.m_NumClients = pUnpacker->GetInt();
		m_CurrentServerInfo.m_MaxClients = pUnpacker->GetInt();
		return 0;
	}
	else if(*pMsgID == protocol7::NETMSG_RCON_AUTH_ON)
	{
		*pMsgID = NETMSG_RCON_AUTH_STATUS;
		Packer.AddInt(1); // authed
		Packer.AddInt(1); // cmdlist
		return 0;
	}
	else if(*pMsgID == protocol7::NETMSG_RCON_AUTH_OFF)
	{
		*pMsgID = NETMSG_RCON_AUTH_STATUS;
		Packer.AddInt(0); // authed
		Packer.AddInt(0); // cmdlist
		return 0;
	}
	else if(*pMsgID == protocol7::NETMSG_MAP_DATA)
	{
		// not binary compatible but translation happens on unpack
		*pMsgID = NETMSG_MAP_DATA;
	}
	else if(*pMsgID >= protocol7::NETMSG_CON_READY && *pMsgID <= protocol7::NETMSG_INPUTTIMING)
	{
		*pMsgID = *pMsgID - 1;
	}
	else if(*pMsgID == protocol7::NETMSG_RCON_LINE)
	{
		*pMsgID = NETMSG_RCON_LINE;
	}
	else if(*pMsgID == protocol7::NETMSG_RCON_CMD_ADD)
	{
		*pMsgID = NETMSG_RCON_CMD_ADD;
	}
	else if(*pMsgID == protocol7::NETMSG_RCON_CMD_REM)
	{
		*pMsgID = NETMSG_RCON_CMD_REM;
	}
	else if(*pMsgID == protocol7::NETMSG_PING_REPLY)
	{
		*pMsgID = NETMSG_PING_REPLY;
	}
	else if(*pMsgID == protocol7::NETMSG_MAPLIST_ENTRY_ADD || *pMsgID == protocol7::NETMSG_MAPLIST_ENTRY_REM)
	{
		// This is just a nice to have so silently dropping that is fine
		return -1;
	}
	else if(*pMsgID >= NETMSG_INFO && *pMsgID <= NETMSG_MAP_DATA)
	{
		return -1; // same in 0.6 and 0.7
	}
	else if(*pMsgID < OFFSET_UUID)
	{
		dbg_msg("sixup", "drop unknown sys msg=%d", *pMsgID);
		return -1;
	}

	return -1;
}
